# Copyright (C) 2018, 2019 David Cattermole.
#
# This file is part of mmSolver.
#
# mmSolver is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# mmSolver is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with mmSolver.  If not, see <https://www.gnu.org/licenses/>.
#
"""
Constant values used in the API.
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import maya.OpenMaya as OpenMaya

# Plug-in names
PLUGIN_NAMES = ['mmSolver']

# Available log levels.
#
# NOTE: These values are also part of the Solver UI. If you change
# these values, make sure to also update the Solver UI, see
# ./python/mmSolver/tools/solver/constant.py
LOG_LEVEL_ERROR = 'error'
LOG_LEVEL_WARNING = 'warning'
LOG_LEVEL_INFO = 'info'
LOG_LEVEL_VERBOSE = 'verbose'
LOG_LEVEL_DEBUG = 'debug'
LOG_LEVEL_LIST = [
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_VERBOSE,
    LOG_LEVEL_DEBUG,
]
# NOTE: These values must match the values in the C++ 'LogLevel' enum,
# see ./src/mmSolver/adjust/adjust_data.h
LOG_LEVEL_ERROR_VALUE = 0
LOG_LEVEL_WARNING_VALUE = 1
LOG_LEVEL_INFO_VALUE = 2
LOG_LEVEL_VERBOSE_VALUE = 3
LOG_LEVEL_DEBUG_VALUE = 4
LOG_LEVEL_NAME_TO_VALUE_MAP = {
    LOG_LEVEL_ERROR: LOG_LEVEL_ERROR_VALUE,
    LOG_LEVEL_WARNING: LOG_LEVEL_WARNING_VALUE,
    LOG_LEVEL_INFO: LOG_LEVEL_INFO_VALUE,
    LOG_LEVEL_VERBOSE: LOG_LEVEL_VERBOSE_VALUE,
    LOG_LEVEL_DEBUG: LOG_LEVEL_DEBUG_VALUE,
}
LOG_LEVEL_DEFAULT = LOG_LEVEL_INFO
LOG_LEVEL_DEFAULT_VALUE = LOG_LEVEL_INFO_VALUE


# Maya node prefixes and suffixes
MARKER_NAME_PREFIX = 'marker'
MARKER_NAME_SUFFIX = '_MKR'
BUNDLE_NAME_PREFIX = 'bundle'
BUNDLE_NAME_SUFFIX = '_BND'
LINE_NAME_PREFIX = 'line'
LINE_NAME_SUFFIX = '_LN'


# list of characters that are not supported in maya for node names.
# Note, only ':' and '_' are not in the list.
BAD_MAYA_CHARS = [
    ' ',
    '#',
    '-',
    '@',
    '!',
    '$',
    '%',
    '^',
    '&',
    '*',
    '+',
    '=',
    '/',
    '\\',
    '~',
    '`',
    '.',
    ',',
    '?',
    ';',
    '|',
    '(',
    ')',
    '[',
    ']',
    '{',
    '}',
    '<',
    '>',
    '\'',
    '\"',
]


# Object Types as constants
OBJECT_TYPE_UNKNOWN = 'unknown'
OBJECT_TYPE_ATTRIBUTE = 'attribute'
OBJECT_TYPE_MARKER = 'marker'
OBJECT_TYPE_CAMERA = 'camera'
OBJECT_TYPE_LINE = 'line'
OBJECT_TYPE_LENS = 'lens'
OBJECT_TYPE_MARKER_GROUP = 'markergroup'
OBJECT_TYPE_BUNDLE = 'bundle'
OBJECT_TYPE_COLLECTION = 'collection'
OBJECT_TYPE_IMAGE_PLANE = 'imageplane'
OBJECT_TYPE_LIST = [
    OBJECT_TYPE_UNKNOWN,
    OBJECT_TYPE_ATTRIBUTE,
    OBJECT_TYPE_MARKER,
    OBJECT_TYPE_CAMERA,
    OBJECT_TYPE_LENS,
    OBJECT_TYPE_MARKER_GROUP,
    OBJECT_TYPE_BUNDLE,
    OBJECT_TYPE_COLLECTION,
    OBJECT_TYPE_IMAGE_PLANE,
]


# Solver objects.
SOLVER_DATA_DEFAULT = {
    'enabled': True,
}


# Solver Types
SOLVER_TYPE_LEVMAR = 0
SOLVER_TYPE_CMINPACK_LM = 1
SOLVER_TYPE_CMINPACK_LMDIF = 1
SOLVER_TYPE_CMINPACK_LMDER = 2
SOLVER_TYPE_CERES = 3
SOLVER_TYPE_DEFAULT = SOLVER_TYPE_CMINPACK_LMDER
SOLVER_TYPE_LIST = [
    # levmar is not included in this list because it is deprecated.
    SOLVER_TYPE_CMINPACK_LMDIF,
    SOLVER_TYPE_CMINPACK_LMDER,
    SOLVER_TYPE_CERES,
]


# Scene Graph Modes
SCENE_GRAPH_MODE_MAYA_DAG = 0
SCENE_GRAPH_MODE_MM_SCENE_GRAPH = 1
SCENE_GRAPH_MODE_DEFAULT = SCENE_GRAPH_MODE_MAYA_DAG
SCENE_GRAPH_MODE_LIST = [
    SCENE_GRAPH_MODE_MAYA_DAG,
    SCENE_GRAPH_MODE_MM_SCENE_GRAPH,
]
SCENE_GRAPH_MODE_MAYA_DAG_NAME = "maya_dag"
SCENE_GRAPH_MODE_MM_SCENE_GRAPH_NAME = "mmscenegraph"
SCENE_GRAPH_MODE_NAME_LIST = [
    SCENE_GRAPH_MODE_MAYA_DAG_NAME,
    SCENE_GRAPH_MODE_MM_SCENE_GRAPH_NAME,
]
SCENE_GRAPH_MODE_MAYA_DAG_LABEL = "Maya DAG"
SCENE_GRAPH_MODE_MM_SCENE_GRAPH_LABEL = "MM Scene Graph"
SCENE_GRAPH_MODE_LABEL_LIST = [
    SCENE_GRAPH_MODE_MAYA_DAG_LABEL,
    SCENE_GRAPH_MODE_MM_SCENE_GRAPH_LABEL,
]

# Time evaluation modes.
TIME_EVAL_MODE_DG_CONTEXT = 0
TIME_EVAL_MODE_SET_TIME = 1
TIME_EVAL_MODE_DEFAULT = TIME_EVAL_MODE_DG_CONTEXT
TIME_EVAL_MODE_LIST = [
    TIME_EVAL_MODE_DG_CONTEXT,
    TIME_EVAL_MODE_SET_TIME,
]


# Frame Solve Mode - how to solve a bunch of frames given to mmSolver?
FRAME_SOLVE_MODE_ALL_FRAMES_AT_ONCE = 0
FRAME_SOLVE_MODE_PER_FRAME = 1
FRAME_SOLVE_MODE_DEFAULT = FRAME_SOLVE_MODE_ALL_FRAMES_AT_ONCE
FRAME_SOLVE_MODE_LIST = [
    FRAME_SOLVE_MODE_ALL_FRAMES_AT_ONCE,
    FRAME_SOLVE_MODE_PER_FRAME,
]


# Attribute States
ATTR_STATE_INVALID = 0
ATTR_STATE_STATIC = 1
ATTR_STATE_ANIMATED = 2
ATTR_STATE_LOCKED = 3


# Auto Differencing Types
AUTO_DIFF_TYPE_FORWARD = 0
AUTO_DIFF_TYPE_CENTRAL = 1
AUTO_DIFF_TYPE_LIST = [
    AUTO_DIFF_TYPE_FORWARD,
    AUTO_DIFF_TYPE_CENTRAL,
]


# Default node Colours for Markers and bundles.
MARKER_COLOUR_RGB = (1.0, 0.0, 0.0)
BUNDLE_COLOUR_RGB = (0.0, 1.0, 0.0)


# Node Types.
MARKER_TRANSFORM_OLD_NODE_TYPE = 'transform'
MARKER_TRANSFORM_NODE_TYPE = 'mmMarkerTransform'
MARKER_SHAPE_NODE_TYPE = 'mmMarkerShape'
BUNDLE_TRANSFORM_NODE_TYPE = 'transform'
BUNDLE_SHAPE_NODE_TYPE = 'mmBundleShape'
CAMERA_TRANSFORM_NODE_API_TYPES = (
    OpenMaya.MFn.kTransform,
    OpenMaya.MFn.kPluginTransformNode,
    OpenMaya.MFn.kUnknownTransform,
)
CAMERA_SHAPE_NODE_API_TYPES = (OpenMaya.MFn.kCamera,)
LINE_TRANSFORM_NODE_TYPE = 'transform'
LINE_SHAPE_NODE_TYPE = 'mmLineShape'
LENS_NODE_TYPE_DEFAULT = 'mmLensModel3de'
LENS_DATA_TYPE_ID = 0x0012F184
LENS_TOGGLE_NODE_NAME = 'lensDistortionToggle1'
LINE_POINT_INTERSECT_NODE_TYPE = 'mmLinePointIntersect'


# Icon names
MARKER_SHAPE_ICON_NAME = 'out_mmMarkerShape.png'
BUNDLE_SHAPE_ICON_NAME = 'out_mmBundleShape.png'
LINE_SHAPE_ICON_NAME = 'out_mmLineShape.png'
LENS_SHAPE_ICON_NAME = 'out_mmLensShape.png'


# Collection Attribute Names
COLLECTION_ATTR_LONG_NAME_SOLVER_LIST = 'solver_list'
COLLECTION_ATTR_LONG_NAME_SOLVER_RESULTS = 'solver_results'
COLLECTION_ATTR_LONG_NAME_DEVIATION = 'deviation'
COLLECTION_ATTR_LONG_NAME_SOLVE_TIMESTAMP = 'solve_timestamp'
COLLECTION_ATTR_LONG_NAME_SOLVE_DURATION = 'solve_duration'
COLLECTION_ATTR_LONG_NAME_ATTR_DETAILS = 'attribute_details_{name}'


# Marker Attribute Names
MARKER_ATTR_LONG_NAME_ENABLE = 'enable'
MARKER_ATTR_LONG_NAME_WEIGHT = 'weight'
MARKER_ATTR_LONG_NAME_DEVIATION = 'deviation'
MARKER_ATTR_LONG_NAME_AVG_DEVIATION = 'averageDeviation'
MARKER_ATTR_LONG_NAME_MAX_DEVIATION = 'maximumDeviation'
MARKER_ATTR_LONG_NAME_MAX_DEV_FRAME = 'maximumDeviationFrame'
MARKER_ATTR_LONG_NAME_BUNDLE = 'bundle'
MARKER_ATTR_LONG_NAME_MARKER_NAME = 'markerName'
MARKER_ATTR_LONG_NAME_MARKER_ID = 'markerId'
MARKER_ATTR_LONG_NAME_MARKER_USED_HINT = 'markerUsedHint'


# Line Attribute Names
LINE_ATTR_LONG_NAME_ENABLE = 'enable'
LINE_ATTR_LONG_NAME_WEIGHT = 'weight'


# Default plate fallback values.
DEFAULT_PLATE_WIDTH = 2048
DEFAULT_PLATE_HEIGHT = 1556


# Root Frame Strategies.
ROOT_FRAME_STRATEGY_GLOBAL_VALUE = 0
ROOT_FRAME_STRATEGY_FWD_PAIR_VALUE = 1  # forward pair.
ROOT_FRAME_STRATEGY_FWD_PAIR_AND_GLOBAL_VALUE = 2
ROOT_FRAME_STRATEGY_FWD_INCREMENT_VALUE = 3  # forward increment.

ROOT_FRAME_STRATEGY_VALUE_LIST = [
    ROOT_FRAME_STRATEGY_GLOBAL_VALUE,
    ROOT_FRAME_STRATEGY_FWD_PAIR_VALUE,
    ROOT_FRAME_STRATEGY_FWD_PAIR_AND_GLOBAL_VALUE,
    ROOT_FRAME_STRATEGY_FWD_INCREMENT_VALUE,
]
# ROOT_FRAME_STRATEGY_DEFAULT_VALUE = ROOT_FRAME_STRATEGY_FWD_INCREMENT_VALUE
ROOT_FRAME_STRATEGY_DEFAULT_VALUE = ROOT_FRAME_STRATEGY_GLOBAL_VALUE


# Robust Loss Type
ROBUST_LOSS_TYPE_TRIVIAL_VALUE = 0
ROBUST_LOSS_TYPE_SOFT_L_ONE_VALUE = 1
ROBUST_LOSS_TYPE_CAUCHY_VALUE = 2

ROBUST_LOSS_TYPE_VALUE_LIST = [
    ROBUST_LOSS_TYPE_TRIVIAL_VALUE,
    ROBUST_LOSS_TYPE_SOFT_L_ONE_VALUE,
    ROBUST_LOSS_TYPE_CAUCHY_VALUE,
]

ROBUST_LOSS_TYPE_DEFAULT_VALUE = ROBUST_LOSS_TYPE_CAUCHY_VALUE


# Standard Solver Default Values
SOLVER_STD_USE_SINGLE_FRAME_DEFAULT_VALUE = False
SOLVER_STD_SINGLE_FRAME_DEFAULT_VALUE = None
SOLVER_STD_ONLY_ROOT_FRAMES_DEFAULT_VALUE = False
SOLVER_STD_GLOBAL_SOLVE_DEFAULT_VALUE = False
SOLVER_STD_TRIANGULATE_BUNDLES_DEFAULT_VALUE = False
SOLVER_STD_USE_ATTR_BLOCKS_DEFAULT_VALUE = True
SOLVER_STD_EVAL_OBJECT_RELATIONSHIPS_DEFAULT_VALUE = False
SOLVER_STD_EVAL_COMPLEX_GRAPHS_DEFAULT_VALUE = False
SOLVER_STD_SOLVER_TYPE_DEFAULT_VALUE = SOLVER_TYPE_DEFAULT
SOLVER_STD_SCENE_GRAPH_MODE_DEFAULT_VALUE = SCENE_GRAPH_MODE_DEFAULT
SOLVER_STD_ROOT_FRAME_STRATEGY_DEFAULT_VALUE = ROOT_FRAME_STRATEGY_DEFAULT_VALUE
SOLVER_STD_BLOCK_ITERATION_NUM_DEFAULT_VALUE = 5
SOLVER_STD_ROOT_ITERATION_NUM_DEFAULT_VALUE = 100
SOLVER_STD_ANIM_ITERATION_NUM_DEFAULT_VALUE = 100
SOLVER_STD_LINEUP_ITERATION_NUM_DEFAULT_VALUE = 100
SOLVER_STD_SOLVE_FOCAL_LENGTH_DEFAULT_VALUE = True
SOLVER_STD_SOLVE_LENS_DISTORTION_DEFAULT_VALUE = True


# Camera Solver Default Values
SOLVER_CAM_SOLVER_TYPE_DEFAULT_VALUE = SOLVER_TYPE_DEFAULT
SOLVER_CAM_BUNDLE_ITERATION_NUM_DEFAULT_VALUE = 5
SOLVER_CAM_ROOT_ITERATION_NUM_DEFAULT_VALUE = 100
SOLVER_CAM_ANIM_ITERATION_NUM_DEFAULT_VALUE = 10
SOLVER_CAM_TRIANGULATE_BUNDLES_DEFAULT_VALUE = True
SOLVER_CAM_ADJUST_EVERY_N_POSES_DEFAULT_VALUE = 10
SOLVER_CAM_SCENE_SCALE_DEFAULT_VALUE = 10.0
SOLVER_CAM_SOLVE_FOCAL_LENGTH_DEFAULT_VALUE = True
SOLVER_CAM_SOLVE_LENS_DISTORTION_DEFAULT_VALUE = True


# Execute validation mode
VALIDATE_MODE_PRE_VALIDATE_VALUE = 'pre_validate'
VALIDATE_MODE_AT_RUNTIME_VALUE = 'at_runtime'
VALIDATE_MODE_NONE_VALUE = 'none'
VALIDATE_MODE_VALUE_LIST = [
    VALIDATE_MODE_PRE_VALIDATE_VALUE,
    VALIDATE_MODE_AT_RUNTIME_VALUE,
    VALIDATE_MODE_NONE_VALUE,
]


# Event Names
EVENT_NAME_MARKER_CREATED = 'marker_created'
EVENT_NAME_BUNDLE_CREATED = 'bundle_created'
EVENT_NAME_LINE_CREATED = 'line_created'
EVENT_NAME_COLLECTION_CREATED = 'collection_created'
EVENT_NAME_COLLECTION_MARKERS_CHANGED = 'collection_markers_changed'
EVENT_NAME_COLLECTION_LINES_CHANGED = 'collection_line_changed'
EVENT_NAME_COLLECTION_ATTRS_CHANGED = 'collection_attributes_changed'
EVENT_NAME_ATTRIBUTE_STATE_CHANGED = 'attributes_state_changed'
EVENT_NAME_ATTRIBUTE_CONNECTION_CHANGED = 'attribute_connection_changed'
EVENT_NAME_NODE_NAME_CHANGED = 'node_name_changed'
EVENT_NAME_NODE_DELETED = 'node_deleted'
EVENT_NAME_MEMBERSHIP_CHANGED = 'node_membership_changed'
EVENT_NAME_MAYA_SCENE_CLOSING = 'maya_scene_closing'
EVENT_NAME_LIST = [
    EVENT_NAME_MARKER_CREATED,
    EVENT_NAME_BUNDLE_CREATED,
    EVENT_NAME_LINE_CREATED,
    EVENT_NAME_COLLECTION_CREATED,
    EVENT_NAME_COLLECTION_MARKERS_CHANGED,
    EVENT_NAME_COLLECTION_LINES_CHANGED,
    EVENT_NAME_COLLECTION_ATTRS_CHANGED,
    EVENT_NAME_ATTRIBUTE_STATE_CHANGED,
    EVENT_NAME_ATTRIBUTE_CONNECTION_CHANGED,
    EVENT_NAME_NODE_NAME_CHANGED,
    EVENT_NAME_NODE_DELETED,
    EVENT_NAME_MEMBERSHIP_CHANGED,
    EVENT_NAME_MAYA_SCENE_CLOSING,
]


# Internal values to indicate if an object (Attribute or Marker) is
# used by the solver.
ATTRIBUTE_USED_HINT_UNKNOWN_VALUE = 0
ATTRIBUTE_USED_HINT_USED_VALUE = 1
ATTRIBUTE_USED_HINT_NOT_USED_VALUE = -1
ATTRIBUTE_USED_HINT_DEFAULT_VALUE = ATTRIBUTE_USED_HINT_UNKNOWN_VALUE
ATTRIBUTE_USED_HINT_LIST = [
    ATTRIBUTE_USED_HINT_UNKNOWN_VALUE,
    ATTRIBUTE_USED_HINT_USED_VALUE,
    ATTRIBUTE_USED_HINT_NOT_USED_VALUE,
]

MARKER_USED_HINT_UNKNOWN_VALUE = 0
MARKER_USED_HINT_USED_VALUE = 1
MARKER_USED_HINT_NOT_USED_VALUE = -1
MARKER_USED_HINT_DEFAULT_VALUE = MARKER_USED_HINT_UNKNOWN_VALUE
MARKER_USED_HINT_LIST = [
    MARKER_USED_HINT_UNKNOWN_VALUE,
    MARKER_USED_HINT_USED_VALUE,
    MARKER_USED_HINT_NOT_USED_VALUE,
]


# Action Status values
# Used when a test action is run - did it succeed, fail, etc?
ACTION_STATUS_SUCCESS = 'success'
ACTION_STATUS_FAILED = 'failed'
ACTION_STATUS_LIST = [
    ACTION_STATUS_SUCCESS,
    ACTION_STATUS_FAILED,
]


# Attribute Solver Types
ATTR_SOLVER_TYPE_REGULAR = 'regular'
ATTR_SOLVER_TYPE_BUNDLE_TRANSFORM = 'bundle_transform'
ATTR_SOLVER_TYPE_CAMERA_TRANSLATE = 'camera_translate'
ATTR_SOLVER_TYPE_CAMERA_ROTATE = 'camera_rotate'
ATTR_SOLVER_TYPE_CAMERA_INTRINSIC = 'camera_intrinsic'
ATTR_SOLVER_TYPE_LENS_DISTORTION = 'lens_distortion'
ATTR_SOLVER_TYPE_LIST = [
    ATTR_SOLVER_TYPE_REGULAR,
    ATTR_SOLVER_TYPE_BUNDLE_TRANSFORM,
    ATTR_SOLVER_TYPE_CAMERA_TRANSLATE,
    ATTR_SOLVER_TYPE_CAMERA_ROTATE,
    ATTR_SOLVER_TYPE_CAMERA_INTRINSIC,
    ATTR_SOLVER_TYPE_LENS_DISTORTION,
]
