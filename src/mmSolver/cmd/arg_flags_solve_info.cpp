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
 */

#include "arg_flags_solve_info.h"

// STL
#include <cassert>

// Maya
#include <maya/MArgDatabase.h>
#include <maya/MObject.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MSyntax.h>

// Internal Objects
#include "arg_flags_solve_scene_graph.h"
#include "mmSolver/adjust/adjust_base.h"
#include "mmSolver/adjust/adjust_data.h"
#include "mmSolver/utilities/debug_utils.h"

namespace mmsolver {

void createSolveInfoSyntax_solverType(MSyntax &syntax) {
    syntax.addFlag(TAU_FLAG, TAU_FLAG_LONG, MSyntax::kDouble);
    syntax.addFlag(EPSILON1_FLAG, EPSILON1_FLAG_LONG, MSyntax::kDouble);
    syntax.addFlag(EPSILON2_FLAG, EPSILON2_FLAG_LONG, MSyntax::kDouble);
    syntax.addFlag(EPSILON3_FLAG, EPSILON3_FLAG_LONG, MSyntax::kDouble);
    syntax.addFlag(DELTA_FLAG, DELTA_FLAG_LONG, MSyntax::kDouble);
    syntax.addFlag(AUTO_DIFF_TYPE_FLAG, AUTO_DIFF_TYPE_FLAG_LONG,
                   MSyntax::kUnsigned);
    syntax.addFlag(AUTO_PARAM_SCALE_FLAG, AUTO_PARAM_SCALE_FLAG_LONG,
                   MSyntax::kUnsigned);
    syntax.addFlag(ROBUST_LOSS_TYPE_FLAG, ROBUST_LOSS_TYPE_FLAG_LONG,
                   MSyntax::kUnsigned);
    syntax.addFlag(ROBUST_LOSS_SCALE_FLAG, ROBUST_LOSS_SCALE_FLAG_LONG,
                   MSyntax::kDouble);
    syntax.addFlag(SOLVER_TYPE_FLAG, SOLVER_TYPE_FLAG_LONG, MSyntax::kUnsigned);
    syntax.addFlag(ITERATIONS_FLAG, ITERATIONS_FLAG_LONG, MSyntax::kUnsigned);
}

void createSolveInfoSyntax_other(MSyntax &syntax) {
    syntax.addFlag(ACCEPT_ONLY_BETTER_FLAG, ACCEPT_ONLY_BETTER_FLAG_LONG,
                   MSyntax::kBoolean);
    syntax.addFlag(FRAME_SOLVE_MODE_FLAG, FRAME_SOLVE_MODE_FLAG_LONG,
                   MSyntax::kUnsigned);

    syntax.addFlag(IMAGE_WIDTH_FLAG, IMAGE_WIDTH_FLAG_LONG, MSyntax::kDouble);

    createSolveSceneGraphSyntax(syntax);
    syntax.addFlag(TIME_EVAL_MODE_FLAG, TIME_EVAL_MODE_FLAG_LONG,
                   MSyntax::kUnsigned);
}

void createSolveInfoSyntax_removeUnused(MSyntax &syntax) {
    syntax.addFlag(REMOVE_UNUSED_MARKERS_FLAG, REMOVE_UNUSED_MARKERS_FLAG_LONG,
                   MSyntax::kBoolean);
    syntax.addFlag(REMOVE_UNUSED_ATTRIBUTES_FLAG,
                   REMOVE_UNUSED_ATTRIBUTES_FLAG_LONG, MSyntax::kBoolean);
}

void createSolveInfoSyntax_v1(MSyntax &syntax) {
    createSolveInfoSyntax_solverType(syntax);
    createSolveInfoSyntax_removeUnused(syntax);
    createSolveInfoSyntax_other(syntax);
}

void createSolveInfoSyntax_v2(MSyntax &syntax) {
    createSolveInfoSyntax_solverType(syntax);
    createSolveInfoSyntax_other(syntax);
}

void createSolveInfoSyntax(MSyntax &syntax, const int command_version = 0) {
    if (command_version == 2) {
        createSolveInfoSyntax_v2(syntax);
    } else {
        createSolveInfoSyntax_v1(syntax);
    }
}

MStatus parseSolveInfoArguments_solverType(
    const MArgDatabase &argData, int &out_iterations, double &out_tau,
    double &out_epsilon1, double &out_epsilon2, double &out_epsilon3,
    double &out_delta, int &out_autoDiffType, int &out_autoParamScale,
    int &out_robustLossType, double &out_robustLossScale, int &out_solverType,
    bool &out_supportAutoDiffForward, bool &out_supportAutoDiffCentral,
    bool &out_supportParameterBounds, bool &out_supportRobustLoss) {
    MStatus status = MStatus::kSuccess;

    // Get 'Solver Type'
    SolverTypePair solverType = getSolverTypeDefault();
    out_solverType = solverType.first;
    if (argData.isFlagSet(SOLVER_TYPE_FLAG)) {
        status = argData.getFlagArgument(SOLVER_TYPE_FLAG, 0, out_solverType);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    // Set defaults based on solver type chosen.
    if (out_solverType == SOLVER_TYPE_CMINPACK_LMDIF) {
        out_iterations = CMINPACK_LMDIF_ITERATIONS_DEFAULT_VALUE;
        out_tau = CMINPACK_LMDIF_TAU_DEFAULT_VALUE;
        out_epsilon1 = CMINPACK_LMDIF_EPSILON1_DEFAULT_VALUE;
        out_epsilon2 = CMINPACK_LMDIF_EPSILON2_DEFAULT_VALUE;
        out_epsilon3 = CMINPACK_LMDIF_EPSILON3_DEFAULT_VALUE;
        out_delta = CMINPACK_LMDIF_DELTA_DEFAULT_VALUE;
        out_autoDiffType = CMINPACK_LMDIF_AUTO_DIFF_TYPE_DEFAULT_VALUE;
        out_autoParamScale = CMINPACK_LMDIF_AUTO_PARAM_SCALE_DEFAULT_VALUE;
        out_robustLossType = CMINPACK_LMDIF_ROBUST_LOSS_TYPE_DEFAULT_VALUE;
        out_robustLossScale = CMINPACK_LMDIF_ROBUST_LOSS_SCALE_DEFAULT_VALUE;
        out_supportAutoDiffForward =
            CMINPACK_LMDIF_SUPPORT_AUTO_DIFF_FORWARD_VALUE;
        out_supportAutoDiffCentral =
            CMINPACK_LMDIF_SUPPORT_AUTO_DIFF_CENTRAL_VALUE;
        out_supportParameterBounds =
            CMINPACK_LMDIF_SUPPORT_PARAMETER_BOUNDS_VALUE;
        out_supportRobustLoss = CMINPACK_LMDIF_SUPPORT_ROBUST_LOSS_VALUE;
    } else if (out_solverType == SOLVER_TYPE_CMINPACK_LMDER) {
        out_iterations = CMINPACK_LMDER_ITERATIONS_DEFAULT_VALUE;
        out_tau = CMINPACK_LMDER_TAU_DEFAULT_VALUE;
        out_epsilon1 = CMINPACK_LMDER_EPSILON1_DEFAULT_VALUE;
        out_epsilon2 = CMINPACK_LMDER_EPSILON2_DEFAULT_VALUE;
        out_epsilon3 = CMINPACK_LMDER_EPSILON3_DEFAULT_VALUE;
        out_delta = CMINPACK_LMDER_DELTA_DEFAULT_VALUE;
        out_autoDiffType = CMINPACK_LMDER_AUTO_DIFF_TYPE_DEFAULT_VALUE;
        out_autoParamScale = CMINPACK_LMDER_AUTO_PARAM_SCALE_DEFAULT_VALUE;
        out_robustLossType = CMINPACK_LMDER_ROBUST_LOSS_TYPE_DEFAULT_VALUE;
        out_robustLossScale = CMINPACK_LMDER_ROBUST_LOSS_SCALE_DEFAULT_VALUE;
        out_supportAutoDiffForward =
            CMINPACK_LMDER_SUPPORT_AUTO_DIFF_FORWARD_VALUE;
        out_supportAutoDiffCentral =
            CMINPACK_LMDER_SUPPORT_AUTO_DIFF_CENTRAL_VALUE;
        out_supportParameterBounds =
            CMINPACK_LMDER_SUPPORT_PARAMETER_BOUNDS_VALUE;
        out_supportRobustLoss = CMINPACK_LMDER_SUPPORT_ROBUST_LOSS_VALUE;
    } else if (out_solverType == SOLVER_TYPE_LEVMAR) {
        out_iterations = LEVMAR_ITERATIONS_DEFAULT_VALUE;
        out_tau = LEVMAR_TAU_DEFAULT_VALUE;
        out_epsilon1 = LEVMAR_EPSILON1_DEFAULT_VALUE;
        out_epsilon2 = LEVMAR_EPSILON2_DEFAULT_VALUE;
        out_epsilon3 = LEVMAR_EPSILON3_DEFAULT_VALUE;
        out_delta = LEVMAR_DELTA_DEFAULT_VALUE;
        out_autoDiffType = LEVMAR_AUTO_DIFF_TYPE_DEFAULT_VALUE;
        out_autoParamScale = LEVMAR_AUTO_PARAM_SCALE_DEFAULT_VALUE;
        out_robustLossType = LEVMAR_ROBUST_LOSS_TYPE_DEFAULT_VALUE;
        out_robustLossScale = LEVMAR_ROBUST_LOSS_SCALE_DEFAULT_VALUE;
        out_supportAutoDiffForward = LEVMAR_SUPPORT_AUTO_DIFF_FORWARD_VALUE;
        out_supportAutoDiffCentral = LEVMAR_SUPPORT_AUTO_DIFF_CENTRAL_VALUE;
        out_supportParameterBounds = LEVMAR_SUPPORT_PARAMETER_BOUNDS_VALUE;
        out_supportRobustLoss = LEVMAR_SUPPORT_ROBUST_LOSS_VALUE;
    } else {
        MMSOLVER_MAYA_ERR(
            "Solver Type is invalid. "
            << "Value may be 0 or 1 (0 == levmar, 1 == cminpack_lm);"
            << "value=" << out_solverType);
        status = MS::kFailure;
        status.perror(
            "Solver Type is invalid. Value may be 0 or 1 (0 == levmar, 1 == "
            "cminpack_lm).");
        return status;
    }

    // Get 'Iterations'
    if (argData.isFlagSet(ITERATIONS_FLAG)) {
        status = argData.getFlagArgument(ITERATIONS_FLAG, 0, out_iterations);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    // Get 'Tau'
    if (argData.isFlagSet(TAU_FLAG)) {
        status = argData.getFlagArgument(TAU_FLAG, 0, out_tau);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }
    out_tau = std::max(0.0, out_tau);
    out_tau = std::min(out_tau, 1.0);
    assert((out_tau >= 0.0) && (out_tau <= 1.0));

    // Get 'Epsilon1'
    if (argData.isFlagSet(EPSILON1_FLAG)) {
        status = argData.getFlagArgument(EPSILON1_FLAG, 0, out_epsilon1);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    // Get 'Epsilon2'
    if (argData.isFlagSet(EPSILON2_FLAG)) {
        status = argData.getFlagArgument(EPSILON2_FLAG, 0, out_epsilon2);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    // Get 'Epsilon3'
    if (argData.isFlagSet(EPSILON3_FLAG)) {
        status = argData.getFlagArgument(EPSILON3_FLAG, 0, out_epsilon3);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    // Get 'Delta'
    if (argData.isFlagSet(DELTA_FLAG)) {
        status = argData.getFlagArgument(DELTA_FLAG, 0, out_delta);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    // Get 'Auto Differencing Type'
    if (argData.isFlagSet(AUTO_DIFF_TYPE_FLAG)) {
        status =
            argData.getFlagArgument(AUTO_DIFF_TYPE_FLAG, 0, out_autoDiffType);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    // Get 'Auto Parameter Scaling'
    if (argData.isFlagSet(AUTO_PARAM_SCALE_FLAG)) {
        status = argData.getFlagArgument(AUTO_PARAM_SCALE_FLAG, 0,
                                         out_autoParamScale);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    // Get 'Robust Loss Type'
    if (argData.isFlagSet(ROBUST_LOSS_TYPE_FLAG)) {
        status = argData.getFlagArgument(ROBUST_LOSS_TYPE_FLAG, 0,
                                         out_robustLossType);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    // Get 'Robust Loss Scale'
    if (argData.isFlagSet(ROBUST_LOSS_SCALE_FLAG)) {
        status = argData.getFlagArgument(ROBUST_LOSS_SCALE_FLAG, 0,
                                         out_robustLossScale);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    return status;
}

MStatus parseSolveInfoArguments_removeUnused(const MArgDatabase &argData,
                                             bool &out_removeUnusedMarkers,
                                             bool &out_removeUnusedAttributes) {
    MStatus status = MStatus::kSuccess;

    // Get 'Remove Unused Markers'
    out_removeUnusedMarkers = REMOVE_UNUSED_MARKERS_DEFAULT_VALUE;
    if (argData.isFlagSet(REMOVE_UNUSED_MARKERS_FLAG)) {
        status = argData.getFlagArgument(REMOVE_UNUSED_MARKERS_FLAG, 0,
                                         out_removeUnusedMarkers);
        CHECK_MSTATUS(status);
    }

    // Get 'Remove Unused Attributes'
    out_removeUnusedAttributes = REMOVE_UNUSED_ATTRIBUTES_DEFAULT_VALUE;
    if (argData.isFlagSet(REMOVE_UNUSED_ATTRIBUTES_FLAG)) {
        status = argData.getFlagArgument(REMOVE_UNUSED_ATTRIBUTES_FLAG, 0,
                                         out_removeUnusedAttributes);
        CHECK_MSTATUS(status);
    }

    return status;
}

MStatus parseSolveInfoArguments_other(const MArgDatabase &argData,
                                      SceneGraphMode &out_sceneGraphMode,
                                      int &out_timeEvalMode,
                                      bool &out_acceptOnlyBetter,
                                      FrameSolveMode &out_frameSolveMode,
                                      double &out_imageWidth) {
    MStatus status = MStatus::kSuccess;

    // Get 'Scene Graph Mode'
    status = parseSolveSceneGraphArguments(argData, out_sceneGraphMode);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    // Get 'Time Evaluation Mode'
    out_timeEvalMode = TIME_EVAL_MODE_DEFAULT_VALUE;
    if (argData.isFlagSet(TIME_EVAL_MODE_FLAG)) {
        status =
            argData.getFlagArgument(TIME_EVAL_MODE_FLAG, 0, out_timeEvalMode);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    // Get 'Accept Only Better'
    out_acceptOnlyBetter = ACCEPT_ONLY_BETTER_DEFAULT_VALUE;
    if (argData.isFlagSet(ACCEPT_ONLY_BETTER_FLAG)) {
        status = argData.getFlagArgument(ACCEPT_ONLY_BETTER_FLAG, 0,
                                         out_acceptOnlyBetter);
        CHECK_MSTATUS(status);
    }

    // Get 'Frame Solve Mode'
    auto frameSolveMode = FRAME_SOLVE_MODE_DEFAULT_VALUE;
    if (argData.isFlagSet(FRAME_SOLVE_MODE_FLAG)) {
        status =
            argData.getFlagArgument(FRAME_SOLVE_MODE_FLAG, 0, frameSolveMode);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }
    out_frameSolveMode = static_cast<FrameSolveMode>(frameSolveMode);

    // Get 'Image Width'
    out_imageWidth = IMAGE_WIDTH_DEFAULT_VALUE;
    if (argData.isFlagSet(IMAGE_WIDTH_FLAG)) {
        status = argData.getFlagArgument(IMAGE_WIDTH_FLAG, 0, out_imageWidth);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    return status;
}

MStatus parseSolveInfoArguments_v1(
    const MArgDatabase &argData, int &out_iterations, double &out_tau,
    double &out_epsilon1, double &out_epsilon2, double &out_epsilon3,
    double &out_delta, int &out_autoDiffType, int &out_autoParamScale,
    int &out_robustLossType, double &out_robustLossScale, int &out_solverType,
    SceneGraphMode &out_sceneGraphMode, int &out_timeEvalMode,
    bool &out_acceptOnlyBetter, FrameSolveMode &out_frameSolveMode,
    bool &out_supportAutoDiffForward, bool &out_supportAutoDiffCentral,
    bool &out_supportParameterBounds, bool &out_supportRobustLoss,
    bool &out_removeUnusedMarkers, bool &out_removeUnusedAttributes,
    double &out_imageWidth) {
    MStatus status = MStatus::kSuccess;

    status = parseSolveInfoArguments_solverType(
        argData, out_iterations, out_tau, out_epsilon1, out_epsilon2,
        out_epsilon3, out_delta, out_autoDiffType, out_autoParamScale,
        out_robustLossType, out_robustLossScale, out_solverType,
        out_supportAutoDiffForward, out_supportAutoDiffCentral,
        out_supportParameterBounds, out_supportRobustLoss);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    status = parseSolveInfoArguments_other(
        argData, out_sceneGraphMode, out_timeEvalMode, out_acceptOnlyBetter,
        out_frameSolveMode, out_imageWidth);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    status = parseSolveInfoArguments_removeUnused(
        argData, out_removeUnusedMarkers, out_removeUnusedAttributes);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    return status;
}

MStatus parseSolveInfoArguments_v2(
    const MArgDatabase &argData, int &out_iterations, double &out_tau,
    double &out_epsilon1, double &out_epsilon2, double &out_epsilon3,
    double &out_delta, int &out_autoDiffType, int &out_autoParamScale,
    int &out_robustLossType, double &out_robustLossScale, int &out_solverType,
    SceneGraphMode &out_sceneGraphMode, int &out_timeEvalMode,
    bool &out_acceptOnlyBetter, FrameSolveMode &out_frameSolveMode,
    bool &out_supportAutoDiffForward, bool &out_supportAutoDiffCentral,
    bool &out_supportParameterBounds, bool &out_supportRobustLoss,
    double &out_imageWidth) {
    MStatus status = MStatus::kSuccess;

    status = parseSolveInfoArguments_solverType(
        argData, out_iterations, out_tau, out_epsilon1, out_epsilon2,
        out_epsilon3, out_delta, out_autoDiffType, out_autoParamScale,
        out_robustLossType, out_robustLossScale, out_solverType,
        out_supportAutoDiffForward, out_supportAutoDiffCentral,
        out_supportParameterBounds, out_supportRobustLoss);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    status = parseSolveInfoArguments_other(
        argData, out_sceneGraphMode, out_timeEvalMode, out_acceptOnlyBetter,
        out_frameSolveMode, out_imageWidth);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return status;
}

}  // namespace mmsolver
