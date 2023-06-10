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

#include "test_batch_3de_classic.h"

#include <mmlens/mmlens.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include "common.h"

int test_batch_3de_classic() {
    const auto test_name = "test_batch_3de_classic";
    const auto identity_prefix = "test_batch_3de_classic: start";
    const auto undistort_prefix = "test_batch_3de_classic: undistort";
    const auto undistort_compare = " -> ";
    const auto redistort_prefix = "test_batch_3de_classic: redistort";
    const auto redistort_compare = " -> ";
    const auto print_prefix = "test_batch_3de_classic: output";
    const auto print_compare = " == ";
    const bool do_print = true;

    const size_t width = 8;
    const size_t height = 8;
    const size_t in_num_channels = 2;    // 2D data
    const size_t temp_num_channels = 2;  // 2D data
    const size_t out_num_channels = 4;   // 4 channels - RGBA

    const bool print_identity = false;
    std::vector<double> in_data_vec =
        generate_st_map_identity<kCoordinateSystemNDC, double, print_identity>(
            identity_prefix, width, height, in_num_channels);
    std::vector<double> temp_data_vec(width * height * temp_num_channels);
    std::vector<float> out_data_vec(width * height * out_num_channels);

    double* in_data = &in_data_vec[0];
    double* temp_data = &temp_data_vec[0];
    float* out_data = &out_data_vec[0];
    const size_t in_data_size = width * height * in_num_channels;
    const size_t temp_data_size = width * height * temp_num_channels;
    const size_t out_data_size = width * height * out_num_channels;

    const double focal_length_cm = 3.5;
    const double film_back_width_cm = 3.6;
    const double film_back_height_cm = 2.4;
    const double pixel_aspect = 1.0;
    const double lens_center_offset_x_cm = 0.0;
    const double lens_center_offset_y_cm = 0.0;
    const mmlens::CameraParameters camera_parameters{
        focal_length_cm, film_back_width_cm,      film_back_height_cm,
        pixel_aspect,    lens_center_offset_x_cm, lens_center_offset_y_cm};

    const double film_back_radius_cm =
        mmlens::compute_diagonal_normalized_camera_factor(camera_parameters);

    auto lens = mmlens::TdeClassicParameters();
    lens.distortion = 0.1;
    lens.anamorphic_squeeze = 1.0;
    lens.curvature_x = 0.0;
    lens.curvature_y = 0.0;
    lens.quartic_distortion = 0.1;

    mmlens::apply_undistort_3de_classic_f64__2d_to_f64_2d(
        in_data, in_data_size, temp_data, temp_data_size, camera_parameters,
        film_back_radius_cm, lens);
    if (do_print) {
        print_data_2d_compare(undistort_prefix, undistort_compare, width,
                              height, in_num_channels, temp_num_channels,
                              in_data, temp_data);
    }

    mmlens::apply_redistort_3de_classic_f64_2d_to_f32_4d(
        temp_data, temp_data_size, out_data, out_data_size, camera_parameters,
        film_back_radius_cm, lens);
    if (do_print) {
        print_data_2d_compare(redistort_prefix, redistort_compare, width,
                              height, temp_num_channels, out_num_channels,
                              temp_data, out_data);
    }

    print_data_2d_compare(print_prefix, print_compare, width, height,
                          in_num_channels, out_num_channels, in_data, out_data);

    return 0;
}
