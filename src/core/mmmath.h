/*
 * Copyright (C) 2021 David Cattermole.
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
 * Mathematical functions for vectors, points, lines and matrices.
 */

#ifndef MAYA_MM_SOLVER_CORE_CALIBRATION_MATH_H
#define MAYA_MM_SOLVER_CORE_CALIBRATION_MATH_H

// Internal
#include "core/mmdata.h"

namespace mmmath {

double
length(mmdata::Point2D a);

double
length(mmdata::Vector3D a);

// Distance between A and B.
double
distance(mmdata::Point2D a, mmdata::Point2D b);

// Distance between A and B.
double
distance(mmdata::Vector3D a, mmdata::Vector3D b);

mmdata::Point2D
normalize(mmdata::Point2D vector);

mmdata::Vector3D
normalize(mmdata::Vector3D vector);

// Difference between A and B points.
mmdata::Point2D
subtract(mmdata::Point2D a, mmdata::Point2D b);

// Dot product.
double
dot(mmdata::Point2D a, mmdata::Point2D b);

// Dot product.
double
dot(mmdata::Vector3D a, mmdata::Vector3D b);

// Cross product.
mmdata::Vector3D
cross(mmdata::Vector3D a, mmdata::Vector3D b);

double
determinant(mmdata::Matrix4x4 m);

mmdata::Matrix4x4
inverse(mmdata::Matrix4x4 m);

mmdata::Matrix4x4
matrixMultiply(mmdata::Matrix4x4 a, mmdata::Matrix4x4 b);

void
transform(mmdata::Matrix4x4 m,
          mmdata::Point3D point,
          mmdata::Point3D &outPoint);

// The point at which 2 2D lines intersect, assuming the lines continue into infinity.
bool
infiniteLineIntersection(
    mmdata::Point2D pointA,
    mmdata::Point2D pointB,
    mmdata::Point2D pointC,
    mmdata::Point2D pointD,
    mmdata::Point2D &outPoint
);

double
cosineAngleBetweenLines(
    mmdata::LinePair2D linePair);

} // namespace math


#endif // MAYA_MM_SOLVER_CORE_CALIBRATION_MATH_H
