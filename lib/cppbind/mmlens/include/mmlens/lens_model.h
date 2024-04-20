/*
 * Copyright (C) 2020, 2023 David Cattermole.
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
 * Abstract base-class for all the LensModels.
 */

#ifndef MM_LENS_LENS_MODEL_H
#define MM_LENS_LENS_MODEL_H

// STL
#include <mmcore/mmhash.h>
#include <mmlens/_cxx.h>
#include <mmlens/_cxxbridge.h>

#include <memory>

namespace mmlens {

class LensModel {
public:
    virtual ~LensModel() = 0;

    LensModel(LensModelType type)
        : m_type(type)
        , m_state(LensModelState::kDirty)
        , m_camera{3.0, 3.6, 2.4, 1.0, 0.0, 0.0}
        // TODO: Pre-compute this value.
        , m_film_back_radius_cm(1.0)
        , m_inputLensModel{} {};

    LensModel(const LensModel &rhs)
        : m_type(rhs.getType())
        // The 'm_state' must not be copied so that intermediate data
        // is recomputed before the distortion calculations are called
        // again.
        , m_state(LensModelState::kDirty)
        , m_camera{rhs.getFocalLength(),       rhs.getFilmBackWidth(),
                   rhs.getFilmBackHeight(),    rhs.getPixelAspect(),
                   rhs.getLensCenterOffsetX(), rhs.getLensCenterOffsetY()}
        // TODO: Can we re-compute this value rather than using
        // 'm_state'?
        , m_film_back_radius_cm(1.0)
        , m_inputLensModel{rhs.getInputLensModel()} {};

    virtual std::unique_ptr<LensModel> cloneAsUniquePtr() const = 0;
    virtual std::shared_ptr<LensModel> cloneAsSharedPtr() const = 0;

    LensModelType getType() const { return m_type; }
    LensModelState getState() const { return m_state; }
    double getFocalLength() const { return m_camera.focal_length_cm; }
    double getFilmBackWidth() const { return m_camera.film_back_width_cm; }
    double getFilmBackHeight() const { return m_camera.film_back_height_cm; }
    double getPixelAspect() const { return m_camera.pixel_aspect; }
    double getLensCenterOffsetX() const {
        return m_camera.lens_center_offset_x_cm;
    }
    double getLensCenterOffsetY() const {
        return m_camera.lens_center_offset_y_cm;
    }

    void setType(const LensModelType value) {
        bool same_value = m_type == value;
        if (!same_value) {
            m_state = LensModelState::kDirty;
            m_type = value;
        }
    }

    void setFocalLength(const double value) {
        bool same_value = m_camera.focal_length_cm == value;
        if (!same_value) {
            m_state = LensModelState::kDirty;
            m_camera.focal_length_cm = value;
        }
    }

    void setFilmBackWidth(const double value) {
        bool same_value = m_camera.film_back_width_cm == value;
        if (!same_value) {
            m_state = LensModelState::kDirty;
            m_camera.film_back_width_cm = value;
        }
    }

    void setFilmBackHeight(const double value) {
        bool same_value = m_camera.film_back_height_cm == value;
        if (!same_value) {
            m_state = LensModelState::kDirty;
            m_camera.film_back_height_cm = value;
        }
    }

    void setPixelAspect(const double value) {
        bool same_value = m_camera.pixel_aspect == value;
        if (!same_value) {
            m_state = LensModelState::kDirty;
            m_camera.pixel_aspect = value;
        }
    }

    void setLensCenterOffsetX(const double value) {
        bool same_value = m_camera.lens_center_offset_x_cm == value;
        if (!same_value) {
            m_state = LensModelState::kDirty;
            m_camera.lens_center_offset_x_cm = value;
        }
    }

    void setLensCenterOffsetY(const double value) {
        bool same_value = m_camera.lens_center_offset_y_cm == value;
        if (!same_value) {
            m_state = LensModelState::kDirty;
            m_camera.lens_center_offset_y_cm = value;
        }
    }

    std::shared_ptr<LensModel> getInputLensModel() const {
        return m_inputLensModel;
    }
    void setInputLensModel(std::shared_ptr<LensModel> value) {
        bool same_value = m_inputLensModel == value;
        if (!same_value) {
            m_state = LensModelState::kDirty;
            m_inputLensModel = value;
        }
    }

    // The 'x' and 'y' values are expected to be in the standard
    // Marker coordinate positions for MM Solver, where (0, 0) means
    // the center of the camera, (-0.5, -0.5) means lower-left of the
    // camera frustum and (+0.5, +0.5) means upper-right of the camera
    // frustum.
    virtual void applyModelUndistort(const double x, const double y,
                                     double &out_x, double &out_y) = 0;
    virtual void applyModelDistort(const double x, const double y,
                                   double &out_x, double &out_y) = 0;

    virtual mmhash::HashValue hashValue() = 0;

protected:
    std::shared_ptr<LensModel> m_inputLensModel;
    LensModelType m_type;
    LensModelState m_state;
    CameraParameters m_camera;
    double m_film_back_radius_cm;
};

inline LensModel::~LensModel() {}

}  // namespace mmlens

#endif  // MM_LENS_LENS_MODEL_H
