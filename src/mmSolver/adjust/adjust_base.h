/*
 * Copyright (C) 2018, 2019 David Cattermole.
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
 * Uses Non-Linear Least Squares algorithm to
 * calculate attribute values based on 2D-to-3D error measurements
 * through a pinhole camera.
 */


#ifndef MM_SOLVER_CORE_BUNDLE_ADJUST_BASE_H
#define MM_SOLVER_CORE_BUNDLE_ADJUST_BASE_H

// STL
#include <string>
#include <vector>
#include <utility>
#include <cmath>
#include <cassert>

// Maya
#include <maya/MPoint.h>
#include <maya/MStringArray.h>
#include <maya/MAnimCurveChange.h>
#include <maya/MDGModifier.h>
#include <maya/MComputation.h>

// MM Solver
#include "mmSolver/utilities/debug_utils.h"
#include "mmSolver/mayahelper/maya_camera.h"
#include "mmSolver/mayahelper/maya_marker.h"
#include "mmSolver/mayahelper/maya_bundle.h"
#include "mmSolver/mayahelper/maya_attr.h"
#include "adjust_defines.h"
#include "adjust_data.h"
#include "adjust_solveFunc.h"


typedef std::vector<std::vector<bool> > BoolList2D;
typedef std::pair<int, int> IndexPair;
typedef std::vector<std::pair<int, int> > IndexPairList;
typedef std::pair<int, std::string> SolverTypePair;

typedef IndexPairList::iterator IndexPairListIt;
typedef IndexPairList::const_iterator IndexPairListCIt;


std::vector<SolverTypePair> getSolverTypes();


SolverTypePair getSolverTypeDefault();


double parameterBoundFromInternalToExternal(
    const double value,
    const double xmin,
    const double xmax,
    const double offset,
    const double scale);


double parameterBoundFromExternalToInternal(
    const double value,
    const double xmin,
    const double xmax,
    const double offset,
    const double scale);


void lossFunctionTrivial(
    const double z,
    double &rho0,
    double &rho1,
    double &rho2);


void lossFunctionSoftL1(
    const double z,
    double &rho0,
    double &rho1,
    double &rho2);


void lossFunctionCauchy(
    const double z,
    double &rho0,
    double &rho1,
    double &rho2);


void applyLossFunctionToErrors(
    const int numberOfErrors,
    double *f,
    int loss_type,
    double loss_scale);


bool get_initial_parameters(
    const int numberOfParameters,
    std::vector<double> &paramList,
    std::vector<std::pair<int, int> > &paramToAttrList,
    AttrPtrList &attrList,
    MTimeArray &frameList,
    MStringArray &outResult);


bool set_maya_attribute_values(
    const int numberOfParameters,
    std::vector<std::pair<int, int> > &paramToAttrList,
    AttrPtrList &attrList,
    std::vector<double> &paramList,
    MTimeArray &frameList,
    MDGModifier &dgmod,
    MAnimCurveChange &curveChange);


bool compute_error_stats(
    const int numberOfMarkerErrors,
    const SolverData &userData,
    double &out_errorAvg,
    double &out_errorMin,
    double &out_errorMax);


void logResultsSolveDetails(
    SolverResult &solverResult,
    SolverData &userData,
    SolverTimer &timer,
    int numberOfParameters,
    int numberOfMarkerErrors,
    int numberOfAttrStiffnessErrors,
    int numberOfAttrSmoothnessErrors,
    bool verbose,
    std::vector<double> &paramList,
    MStringArray &outResult);


MStatus logResultsObjectCounts(
    const int numberOfParameters,
    const int numberOfErrors,
    const int numberOfMarkerErrors,
    const int numberOfAttrStiffnessErrors,
    const int numberOfAttrSmoothnessErrors,
    MStringArray &outResult);


MStatus logResultsMarkerAffectsAttribute(
    const MarkerPtrList markerList,
    const AttrPtrList attrList,
    const BoolList2D markerToAttrList,
    MStringArray &outResult);


bool solve(
    SolverOptions &solverOptions,
    CameraPtrList &cameraList,
    MarkerPtrList &markerList,
    BundlePtrList &bundleList,
    AttrPtrList &attrList,
    const MTimeArray &frameList,
    StiffAttrsPtrList &stiffAttrsList,
    SmoothAttrsPtrList &smoothAttrsList,
    MDGModifier &dgmod,
    MAnimCurveChange &curveChange,
    MComputation &computation,
    MStringArray &printStatsList,
    bool with_verbosity,
    MStringArray &outResult);

#endif // MM_SOLVER_CORE_BUNDLE_ADJUST_BASE_H
