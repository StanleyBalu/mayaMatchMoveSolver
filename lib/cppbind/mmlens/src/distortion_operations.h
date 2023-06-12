/*
 * Copyright (C) 2023 David Cattermole.
 *
 * This file is part of mmSolver.
 *
 * mmSolver is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * mmSolver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with mmSolver.  If not, see <https://www.gnu.org/licenses/>.
 * ====================================================================
 *
 */

#include <mmcore/mmdata.h>
#include <mmlens/_cxxbridge.h>

#include <cassert>
#include <type_traits>

namespace mmlens {

// Apply lens distortion to a single 2D coordinate.
template <DistortionDirection DIRECTION, class IN_TYPE, class OUT_TYPE,
          class LENS_TYPE>
std::pair<OUT_TYPE, OUT_TYPE> apply_lens_distortion_once(
    const IN_TYPE in_x, const IN_TYPE in_y,
    const CameraParameters camera_parameters, const double film_back_radius_cm,
    LENS_TYPE lens) {
    auto out_x = static_cast<OUT_TYPE>(0);
    auto out_y = static_cast<OUT_TYPE>(0);

    if (DIRECTION == DistortionDirection::kUndistort) {
        // Undistort
        const auto in_point_unit = mmdata::Vector2D(in_x, in_y);
        const mmdata::Vector2D in_point_dn = unit_to_diagonal_normalized(
            camera_parameters, film_back_radius_cm, in_point_unit);

        const mmdata::Vector2D undistorted_point_dn = lens.eval(in_point_dn);

        const mmdata::Vector2D undistorted_point_unit =
            diagonal_normalized_to_unit(camera_parameters, film_back_radius_cm,
                                        undistorted_point_dn);

        out_x = static_cast<OUT_TYPE>(undistorted_point_unit.x_);
        out_y = static_cast<OUT_TYPE>(undistorted_point_unit.y_);
    } else {
        // Distort
        //
        // This operation requires iteration to calculate the correct
        // 2D coordinate, which is a lot slower than the undistortion
        // operation.

        // TODO: Look up an initial value from a thread-safe
        // structure that holds a grid of values at vertices which
        // we can interpolate as an initial guess.
        const auto use_guess = false;
        auto guess_point_x = in_x;
        auto guess_point_y = in_y;
        const auto guess_point_unit =
            mmdata::Vector2D(guess_point_x, guess_point_y);
        const mmdata::Vector2D guess_point_dn = unit_to_diagonal_normalized(
            camera_parameters, film_back_radius_cm, guess_point_unit);

        const auto in_point_unit = mmdata::Vector2D(in_x, in_y);
        const mmdata::Vector2D in_point_dn = unit_to_diagonal_normalized(
            camera_parameters, film_back_radius_cm, in_point_unit);

        mmdata::Vector2D distorted_point_dn;
        if (use_guess) {
            // A guess can be used to reduce the number of
            // iterations required to get a good result,
            // increasing performance.
            distorted_point_dn = lens.map_inverse(in_point_dn, guess_point_dn);
        } else {
            distorted_point_dn = lens.map_inverse(in_point_dn);
        }

        const mmdata::Vector2D distorted_point_unit =
            diagonal_normalized_to_unit(camera_parameters, film_back_radius_cm,
                                        distorted_point_dn);

        out_x = static_cast<OUT_TYPE>(distorted_point_unit.x_);
        out_y = static_cast<OUT_TYPE>(distorted_point_unit.y_);
    }

    return std::make_pair(out_x, out_y);
}

// Apply lens distortion to a pixel.
template <DistortionDirection DIRECTION, size_t OUT_DATA_STRIDE, class IN_TYPE,
          class OUT_TYPE, class LENS_TYPE>
void apply_lens_distortion_to_pixel(const IN_TYPE in_x, const IN_TYPE in_y,
                                    OUT_TYPE* out_pixel,
                                    const CameraParameters camera_parameters,
                                    const double film_back_radius_cm,
                                    LENS_TYPE lens) {
    // TODO: See if we can improve performance by calculating 2, 4 or
    // 8 points at once? Try to use SIMD.
    if (DIRECTION == DistortionDirection::kUndistort ||
        DIRECTION == DistortionDirection::kRedistort) {
        const auto out_xy =
            apply_lens_distortion_once<DIRECTION, IN_TYPE, OUT_TYPE, LENS_TYPE>(
                in_x, in_y, camera_parameters, film_back_radius_cm, lens);

        // Convert back to -0.5 to 0.5 coordinate space.
        OUT_TYPE out_x = static_cast<OUT_TYPE>(out_xy.first - 0.5);
        OUT_TYPE out_y = static_cast<OUT_TYPE>(out_xy.second - 0.5);
        if (std::is_same<OUT_TYPE, float>::value) {
            // Converting to -0.5 to 0.5 coordinate space is not
            // important if we are writing to a 'float' data type,
            // since we can assume that f32 data will be the output
            // and will not be processed further.
            out_x = static_cast<OUT_TYPE>(out_xy.first);
            out_y = static_cast<OUT_TYPE>(out_xy.second);
        }

        out_pixel[0] = out_x;
        out_pixel[1] = out_y;
    } else {
        // It is a logical error if trying to calculate both
        // undistortion and redistortion and trying to output to less
        // than 4 values.
        assert(OUT_DATA_STRIDE >= 4);

        // Calculate both directions, and pack into the output data.
        const auto out_undistort_xy =
            apply_lens_distortion_once<DistortionDirection::kUndistort, IN_TYPE,
                                       OUT_TYPE, LENS_TYPE>(
                in_x, in_y, camera_parameters, film_back_radius_cm, lens);
        const auto out_redistort_xy =
            apply_lens_distortion_once<DistortionDirection::kRedistort, IN_TYPE,
                                       OUT_TYPE, LENS_TYPE>(
                in_x, in_y, camera_parameters, film_back_radius_cm, lens);

        // Convert back to -0.5 to 0.5 coordinate space.
        auto out_undistort_x =
            static_cast<OUT_TYPE>(out_undistort_xy.first - 0.5);
        auto out_undistort_y =
            static_cast<OUT_TYPE>(out_undistort_xy.second - 0.5);
        auto out_redistort_x =
            static_cast<OUT_TYPE>(out_redistort_xy.first - 0.5);
        auto out_redistort_y =
            static_cast<OUT_TYPE>(out_redistort_xy.second - 0.5);
        if (std::is_same<OUT_TYPE, float>::value) {
            // Converting to -0.5 to 0.5 coordinate space is not
            // important if we are writing to a 'float' data type,
            // since we can assume that f32 data will be the output
            // and will not be processed further.
            out_undistort_x = static_cast<OUT_TYPE>(out_undistort_xy.first);
            out_undistort_y = static_cast<OUT_TYPE>(out_undistort_xy.second);
            out_redistort_x = static_cast<OUT_TYPE>(out_redistort_xy.first);
            out_redistort_y = static_cast<OUT_TYPE>(out_redistort_xy.second);
        }

        if (DIRECTION == DistortionDirection::kUndistortAndRedistort) {
            out_pixel[0] = out_undistort_x;
            out_pixel[1] = out_undistort_y;
            out_pixel[2] = out_redistort_x;
            out_pixel[3] = out_redistort_y;
        } else if (DIRECTION == DistortionDirection::kRedistortAndUndistort) {
            out_pixel[0] = out_redistort_x;
            out_pixel[1] = out_redistort_y;
            out_pixel[2] = out_undistort_x;
            out_pixel[3] = out_undistort_y;
        }
    }
    return;
}

// Apply lens distortion to a buffer of data.
//
// The in_data and out_data may be the same or different pointers.
//
// It is possible to use in_data as 2 x f64 and out_data as 4 x
// f32. This may seem strange but is legal and is memory efficient.
//
// Reusing the same memory is more efficient as we reduce the amount
// of memory used and also increase the CPU cache use per-pixel being
// processed.
template <DistortionDirection DIRECTION, size_t IN_DATA_STRIDE,
          size_t OUT_DATA_STRIDE, class IN_TYPE, class OUT_TYPE,
          class LENS_TYPE>
void apply_lens_distortion_to_buffer(
    const IN_TYPE* in_data, const size_t in_data_size, OUT_TYPE* out_data,
    const size_t out_data_size, const CameraParameters camera_parameters,
    const double film_back_radius_cm, LENS_TYPE lens) {
    const size_t input_data_num_total = in_data_size / IN_DATA_STRIDE;
    for (size_t i = 0; i < input_data_num_total; i++) {
        const size_t in_index = i * IN_DATA_STRIDE;

        // The lens distortion operation expects values 0.0 to 1.0,
        // but our inputs are -0.5 to 0.5, therefore we must convert.
        const auto in_x = static_cast<double>(in_data[in_index + 0]) + 0.5;
        const auto in_y = static_cast<double>(in_data[in_index + 1]) + 0.5;

        // The '{}' scope is used to ensure that the in_x/in_y values
        // are read first before we write to the out_data, because in
        // theory both in_data and out_data may point to the same
        // memory, but we are interpreting the memory as different
        // types.
        {
            const size_t out_index = i * OUT_DATA_STRIDE;
            OUT_TYPE* out_pixel = out_data + out_index;

            apply_lens_distortion_to_pixel<DIRECTION, OUT_DATA_STRIDE, double,
                                           OUT_TYPE, LENS_TYPE>(
                in_x, in_y, out_pixel, camera_parameters, film_back_radius_cm,
                lens);
        }
    }
    return;
}

// Apply lens distortion to 'identity' coordinate data.
template <DistortionDirection DIRECTION, size_t OUT_DATA_STRIDE, class OUT_TYPE,
          class LENS_TYPE>
void apply_lens_distortion_from_identity(
    const size_t image_width, const size_t image_height, OUT_TYPE* out_data,
    const size_t out_data_size, const CameraParameters camera_parameters,
    const double film_back_radius_cm, LENS_TYPE lens) {
    for (auto row = 0; row < image_height; row++) {
        for (auto column = 0; column < image_width; column++) {
            const size_t index = (row * image_width) + column;

            const double in_x = static_cast<double>(column) /
                                static_cast<double>(image_width - 1);
            const double in_y = static_cast<double>(row) /
                                static_cast<double>(image_height - 1);

            const size_t out_index = index * OUT_DATA_STRIDE;
            OUT_TYPE* out_pixel = out_data + out_index;

            apply_lens_distortion_to_pixel<DIRECTION, OUT_DATA_STRIDE, double,
                                           OUT_TYPE, LENS_TYPE>(
                in_x, in_y, out_pixel, camera_parameters, film_back_radius_cm,
                lens);
        }
    }
    return;
}

}  // namespace mmlens
