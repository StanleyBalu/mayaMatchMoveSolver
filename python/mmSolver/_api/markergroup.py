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
Defines a MarkerGroup node.
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import warnings

import maya.cmds
import maya.OpenMaya as OpenMaya

import mmSolver.logger
import mmSolver.utils.node as node_utils
import mmSolver.utils.python_compat as pycompat
import mmSolver._api.camera as camera
import mmSolver._api.utils as api_utils
import mmSolver._api.constant as const


LOG = mmSolver.logger.get_logger()


class MarkerGroup(object):
    """
    The MarkerGroup to transform markers into camera-space.
    """

    def __init__(self, node=None, name=None):
        """
        Initialize the MarkerGroup with the given Maya node.

        :param node: Maya node to attach to.
        :type node: str or None

        :param name: This is a backwards compatible kwarg for 'node'.
        :type name: None or str
        """
        if name is not None:
            msg = (
                "mmSolver.api.MarkerGroup(name=value), "
                "'name' is a deprecated flag, use 'node' "
            )
            warnings.warn(msg, DeprecationWarning)
            node = name
        self._mfn_tfm = None
        if node is not None:
            self.set_node(node)
        return

    def __repr__(self):
        result = '<{class_name}('.format(class_name=self.__class__.__name__)
        result += '{hash} node={node}'.format(
            hash=hex(hash(self)),
            node=self.get_node(),
        )
        result += ')>'
        return result

    def get_node(self):
        """
        Get the MarkerGroup node.

        :return: The markerGroup node or None
        :rtype: None or str or unicode
        """
        node = None
        if self._mfn_tfm is None:
            return node
        try:
            obj = self._mfn_tfm.object()
        except RuntimeError:
            obj = None
        if obj is not None and obj.isNull() is False:
            try:
                node = self._mfn_tfm.fullPathName()
            except RuntimeError:
                pass
        if isinstance(node, pycompat.TEXT_TYPE) and len(node) == 0:
            node = None
        return node

    def set_node(self, node):
        """
        Set the MarkerGroup to use this Maya node.

        :param node: Maya node path.
        :type node: str
        """
        assert isinstance(node, pycompat.TEXT_TYPE)
        assert maya.cmds.objExists(node)
        assert api_utils.get_object_type(node) == const.OBJECT_TYPE_MARKER_GROUP

        self._mfn_tfm = None
        tfm_dag = node_utils.get_as_dag_path(node)
        if tfm_dag is not None:
            assert (
                maya.cmds.nodeType(tfm_dag.fullPathName()) == 'mmMarkerGroupTransform'
            )
            self._mfn_tfm = OpenMaya.MFnDagNode(tfm_dag)

        if self._mfn_tfm is None:
            self._mfn_tfm = OpenMaya.MFnDagNode()
        return

    ############################################################################

    def get_camera(self):
        """
        Get the attached camera node.

        :return: Camera node or None.
        :rtype: Camera or None
        """
        mkr_node = self.get_node()

        cam_tfm, cam_shp = node_utils.get_camera_above_node(mkr_node)

        # Make the camera object.
        cam = None
        if cam_tfm is not None and cam_shp is not None:
            cam = camera.Camera(transform=cam_tfm, shape=cam_shp)
        return cam

    ############################################################################

    def create_node(self, name='markerGroup1', cam=None):
        """
        Create a MarkerGroup node network.

        :param name: The newly created node's name.
        :type name: str

        :param cam: The camera to attach to.
        :type cam: Camera

        :return: MarkerGroup object with node set as newly created node.
        """
        assert isinstance(name, pycompat.TEXT_TYPE)
        assert isinstance(cam, camera.Camera)

        cam_tfm = cam.get_transform_node()
        cam_shp = cam.get_shape_node()

        mkr_grp = maya.cmds.createNode(
            'mmMarkerGroupTransform', name=name, parent=cam_tfm
        )
        mkr_grp = node_utils.get_long_name(mkr_grp)
        mkr_scl = maya.cmds.createNode('mmMarkerScale')
        self.set_node(mkr_grp)

        # Add attr and connect depth
        depth = 1.0
        maya.cmds.addAttr(
            mkr_grp, longName='depth', at='double', minValue=0.0, defaultValue=1.0
        )
        maya.cmds.setAttr(mkr_grp + '.depth', keyable=True)
        maya.cmds.setAttr(mkr_grp + '.depth', depth)
        maya.cmds.connectAttr(mkr_grp + '.depth', mkr_scl + '.depth')

        # Add overscan X and Y
        mode_attr = mkr_scl + '.overscanMode'
        mode = 1  # 1='overscan x and y' mode
        maya.cmds.setAttr(mode_attr, mode)
        for axis in ['x', 'y']:
            attr_name = 'overscan' + axis.upper()
            maya.cmds.addAttr(
                mkr_grp, longName=attr_name, at='double', minValue=0.0, defaultValue=1.0
            )
            src = mkr_grp + '.' + attr_name
            dst = mkr_scl + '.overscanInverse' + axis.upper()
            maya.cmds.setAttr(src, keyable=True)
            maya.cmds.connectAttr(src, dst)

        # Connect camera attributes
        maya.cmds.connectAttr(cam_shp + '.focalLength', mkr_scl + '.focalLength')
        maya.cmds.connectAttr(cam_shp + '.cameraAperture', mkr_scl + '.cameraAperture')
        maya.cmds.connectAttr(cam_shp + '.filmOffset', mkr_scl + '.filmOffset')

        # Connect marker scale to marker group
        maya.cmds.connectAttr(mkr_scl + '.outScale', mkr_grp + '.scale')
        maya.cmds.connectAttr(mkr_scl + '.outTranslate', mkr_grp + '.translate')

        # Lock and hide all the attributes
        attrs = [
            'tx',
            'ty',
            'tz',
            'rx',
            'ry',
            'rz',
            'sx',
            'sy',
            'sz',
            'shearXY',
            'shearXZ',
            'shearYZ',
            'rotateOrder',
            'rotateAxisX',
            'rotateAxisY',
            'rotateAxisZ',
        ]
        for attr in attrs:
            maya.cmds.setAttr(mkr_grp + '.' + attr, lock=True)
            maya.cmds.setAttr(mkr_grp + '.' + attr, keyable=False, channelBox=False)

        return self

    ############################################################################

    def is_valid(self):
        """
        Check if this object's internal Maya representation is valid or not.

        :return: True or False, is this MarkerGroup valid?
        :rtype: bool
        """
        tfm = self.get_node()
        if (tfm is None) or (maya.cmds.objExists(tfm) is False):
            return False
        return True
