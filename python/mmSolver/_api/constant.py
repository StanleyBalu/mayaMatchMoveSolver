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

import maya.OpenMaya as OpenMaya

# Plug-in names
PLUGIN_NAMES = [
    'mmSolver'
]

LOG_LEVEL_DEFAULT = 'info'

# Maya node prefixes and suffixes
MARKER_NAME_PREFIX = 'marker'
MARKER_NAME_SUFFIX = '_MKR'
BUNDLE_NAME_PREFIX = 'bundle'
BUNDLE_NAME_SUFFIX = '_BND'


# list of characters that are not supported in maya for node names.
# Note, only ':' and '_' are not in the list.
BAD_MAYA_CHARS = [
    ' ', '#', '-', '@', '!', '$', '%', '^', '&', '*',
    '+', '=', '/', '\\', '~', '`', '.', ',', '?', ';', '|',
    '(', ')', '[', ']', '{', '}', '<', '>',
    '\'', '\"',
]


# Object Types as constants
OBJECT_TYPE_UNKNOWN = 'unknown'
OBJECT_TYPE_ATTRIBUTE = 'attribute'
OBJECT_TYPE_MARKER = 'marker'
OBJECT_TYPE_CAMERA = 'camera'
OBJECT_TYPE_MARKER_GROUP = 'markergroup'
OBJECT_TYPE_BUNDLE = 'bundle'
OBJECT_TYPE_COLLECTION = 'collection'
OBJECT_TYPE_IMAGE_PLANE = 'imageplane'
OBJECT_TYPE_LIST = [
    OBJECT_TYPE_UNKNOWN,
    OBJECT_TYPE_ATTRIBUTE,
    OBJECT_TYPE_MARKER,
    OBJECT_TYPE_CAMERA,
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
SOLVER_TYPE_DEFAULT = SOLVER_TYPE_CMINPACK_LMDER


# Time evaluation modes.
TIME_EVAL_MODE_DG_CONTEXT = 0
TIME_EVAL_MODE_SET_TIME = 1
TIME_EVAL_MODE_DEFAULT = TIME_EVAL_MODE_DG_CONTEXT
TIME_EVAL_MODE_LIST = [
    TIME_EVAL_MODE_DG_CONTEXT,
    TIME_EVAL_MODE_SET_TIME,
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
MARKER_TRANSFORM_NODE_TYPE = 'transform'
MARKER_SHAPE_NODE_TYPE = 'locator'
BUNDLE_TRANSFORM_NODE_TYPE = 'transform'
BUNDLE_SHAPE_NODE_TYPE = 'locator'
CAMERA_TRANSFORM_NODE_API_TYPES = (
    OpenMaya.MFn.kTransform,
    OpenMaya.MFn.kPluginTransformNode,
    OpenMaya.MFn.kUnknownTransform,
)
CAMERA_SHAPE_NODE_API_TYPES = (
    OpenMaya.MFn.kCamera,
)


# Icon names
MARKER_SHAPE_ICON_NAME = 'out_mmMarkerShape.png'
BUNDLE_SHAPE_ICON_NAME = 'out_mmBundleShape.png'


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


# Default plate fallback values.
DEFAULT_PLATE_WIDTH = 2048
DEFAULT_PLATE_HEIGHT = 1556


# Root Frame Strategies.
ROOT_FRAME_STRATEGY_GLOBAL_VALUE = 0
ROOT_FRAME_STRATEGY_FWD_PAIR_VALUE = 1 # forward pair.
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

ROBUST_LOSS_TYPE_DEFAULT_VALUE = ROBUST_LOSS_TYPE_TRIVIAL_VALUE


# Standard Solver Default Values
SOLVER_STD_USE_SINGLE_FRAME_DEFAULT_VALUE = False
SOLVER_STD_SINGLE_FRAME_DEFAULT_VALUE = None
SOLVER_STD_ONLY_ROOT_FRAMES_DEFAULT_VALUE = False
SOLVER_STD_GLOBAL_SOLVE_DEFAULT_VALUE = False
SOLVER_STD_EVAL_COMPLEX_GRAPHS_DEFAULT_VALUE = False
SOLVER_STD_ROOT_FRAME_STRATEGY_DEFAULT_VALUE = ROOT_FRAME_STRATEGY_DEFAULT_VALUE
SOLVER_STD_BLOCK_ITERATION_NUM_DEFAULT_VALUE = 3
SOLVER_STD_ROOT_ITERATION_NUM_DEFAULT_VALUE = 100
SOLVER_STD_ANIM_ITERATION_NUM_DEFAULT_VALUE = 100
SOLVER_STD_LINEUP_ITERATION_NUM_DEFAULT_VALUE = 100


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
EVENT_NAME_COLLECTION_CREATED = 'collection_created'
EVENT_NAME_COLLECTION_MARKERS_CHANGED = 'collection_markers_changed'
EVENT_NAME_COLLECTION_ATTRS_CHANGED = 'collection_attributes_changed'
EVENT_NAME_ATTRIBUTE_STATE_CHANGED = 'attributes_state_changed'
EVENT_NAME_ATTRIBUTE_CONNECTION_CHANGED = 'attribute_connection_changed'
EVENT_NAME_NODE_NAME_CHANGED = 'node_name_changed'
EVENT_NAME_NODE_DELETED = 'node_deleted'
EVENT_NAME_MEMBERSHIP_CHANGED = 'node_membership_changed'
EVENT_NAME_LIST = [
    EVENT_NAME_MARKER_CREATED,
    EVENT_NAME_BUNDLE_CREATED,
    EVENT_NAME_COLLECTION_CREATED,
    EVENT_NAME_COLLECTION_MARKERS_CHANGED,
    EVENT_NAME_COLLECTION_ATTRS_CHANGED,
    EVENT_NAME_ATTRIBUTE_STATE_CHANGED,
    EVENT_NAME_ATTRIBUTE_CONNECTION_CHANGED,
    EVENT_NAME_NODE_NAME_CHANGED,
    EVENT_NAME_NODE_DELETED,
    EVENT_NAME_MEMBERSHIP_CHANGED,
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
