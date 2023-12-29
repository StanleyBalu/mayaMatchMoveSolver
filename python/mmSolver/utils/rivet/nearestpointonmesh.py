# Copyright (C) 2014, 2022, 2023 David Cattermole.
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
Query the Nearest Point On a Mesh surface.
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import collections

import maya.cmds

import mmSolver.logger


LOG = mmSolver.logger.get_logger()

NearestPointData = collections.namedtuple(
    'NearestPointData', ['position', 'normal', 'coords', 'face_index']
)


def get_nearest_point_on_mesh(mesh_shape, in_position):
    """
    Get data on the mesh surface, for the nearest position to
    'in_position'.
    """
    assert maya.cmds.objExists(mesh_shape)
    assert isinstance(in_position, (tuple, list)) and len(in_position) == 3

    maya.cmds.loadPlugin('nearestPointOnMesh', quiet=True)

    tmp_node = maya.cmds.createNode('nearestPointOnMesh')
    maya.cmds.connectAttr(mesh_shape + '.worldMesh[0]', tmp_node + '.inMesh')

    maya.cmds.setAttr(tmp_node + '.inPositionX', in_position[0])
    maya.cmds.setAttr(tmp_node + '.inPositionY', in_position[1])
    maya.cmds.setAttr(tmp_node + '.inPositionZ', in_position[2])

    position = (
        maya.cmds.getAttr(tmp_node + '.positionX'),
        maya.cmds.getAttr(tmp_node + '.positionY'),
        maya.cmds.getAttr(tmp_node + '.positionZ'),
    )
    normal = (
        maya.cmds.getAttr(tmp_node + '.normalX'),
        maya.cmds.getAttr(tmp_node + '.normalY'),
        maya.cmds.getAttr(tmp_node + '.normalZ'),
    )
    coords = (
        maya.cmds.getAttr(tmp_node + '.parameterU'),
        maya.cmds.getAttr(tmp_node + '.parameterV'),
    )
    face_index = maya.cmds.getAttr(tmp_node + '.nearestFaceIndex')
    result = NearestPointData(
        position=position, normal=normal, coords=coords, face_index=face_index
    )

    maya.cmds.delete(tmp_node)
    return result
