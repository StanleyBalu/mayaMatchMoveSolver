# Copyright (C) 2020, 2022, 2024 David Cattermole.
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
Library functions for creating and modifying image planes.
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import maya.cmds

import mmSolver.logger
import mmSolver.api as mmapi
import mmSolver.utils.node as node_utils
import mmSolver.utils.constant as const_utils
import mmSolver.utils.python_compat as pycompat
import mmSolver.utils.imageseq as imageseq_utils
import mmSolver.tools.createimageplane._lib.utilities as lib_utils


LOG = mmSolver.logger.get_logger()


def _create_transform_attrs(image_plane_tfm):
    # Depth attribute
    attr = 'depth'
    maya.cmds.addAttr(
        image_plane_tfm,
        longName=attr,
        attributeType='double',
        minValue=0.0,
        defaultValue=1.0,
    )
    maya.cmds.setAttr(image_plane_tfm + '.' + attr, keyable=True)

    # Focal Length attribute
    attr = 'focalLength'
    maya.cmds.addAttr(
        image_plane_tfm,
        longName=attr,
        attributeType='double',
        minValue=0.1,
        defaultValue=35.0,
    )
    maya.cmds.setAttr(image_plane_tfm + '.' + attr, keyable=True)

    # Horizontal Film Aperture attribute
    attr = 'horizontalFilmAperture'
    value = 36.0 / 25.4  # 35mm film in inches.
    maya.cmds.addAttr(
        image_plane_tfm,
        longName=attr,
        attributeType='double',
        minValue=0.001,
        defaultValue=value,
    )
    maya.cmds.setAttr(image_plane_tfm + '.' + attr, keyable=True)

    # Vertical Film Aperture attribute
    attr = 'verticalFilmAperture'
    value = 24.0 / 25.4  # 35mm film in inches.
    maya.cmds.addAttr(
        image_plane_tfm,
        longName=attr,
        attributeType='double',
        minValue=0.001,
        defaultValue=value,
    )
    maya.cmds.setAttr(image_plane_tfm + '.' + attr, keyable=True)

    # Pixel Aspect Ratio attribute
    attr = 'pixelAspect'
    value = 1.0
    maya.cmds.addAttr(
        image_plane_tfm,
        longName=attr,
        attributeType='double',
        minValue=0.001,
        defaultValue=value,
    )
    maya.cmds.setAttr(image_plane_tfm + '.' + attr, keyable=True)

    # Horizontal Film Offset attribute
    attr = 'horizontalFilmOffset'
    maya.cmds.addAttr(
        image_plane_tfm,
        longName=attr,
        attributeType='double',
        minValue=0.0,
        defaultValue=0.0,
    )
    maya.cmds.setAttr(image_plane_tfm + '.' + attr, keyable=True)

    # Vertical Film Offset attribute
    attr = 'verticalFilmOffset'
    maya.cmds.addAttr(
        image_plane_tfm,
        longName=attr,
        attributeType='double',
        minValue=0.0,
        defaultValue=0.0,
    )
    maya.cmds.setAttr(image_plane_tfm + '.' + attr, keyable=True)
    return


def _create_image_plane_shape_attrs(image_plane_shp):
    # Image Load Enable attribute
    attr = 'imageLoadEnable'
    maya.cmds.addAttr(
        image_plane_shp, longName=attr, attributeType='bool', defaultValue=1
    )
    node_attr = image_plane_shp + '.' + attr
    maya.cmds.setAttr(node_attr, keyable=True)

    # Choose which image sequence path to use. This is only visible in
    # the Attribute Editor, because this attribute must trigger a
    # callback when changed to update the underlying 'file' node.
    attr = 'imageSequenceSlot'
    maya.cmds.addAttr(
        image_plane_shp,
        longName=attr,
        attributeType='enum',
        enumName="main=0:alternate1=1:alternate2=2:alternate3=3",
    )
    node_attr = image_plane_shp + '.' + attr
    maya.cmds.setAttr(node_attr, keyable=False)

    # Image Sequence attribute
    attr = 'imageSequence'
    nice_name = 'Image Sequence'
    attr_suffix_list = ['Main', 'Alternate1', 'Alternate2', 'Alternate3']
    nice_suffix_list = [' (Main)', ' (Alt 1)', ' (Alt 2)', ' (Alt 3)']
    for attr_suffix, nice_suffix in zip(attr_suffix_list, nice_suffix_list):
        maya.cmds.addAttr(
            image_plane_shp,
            longName=attr + attr_suffix,
            niceName=nice_name + nice_suffix,
            dataType='string',
        )

    # Image Sequence Frame attribute
    attr = 'imageSequenceFrame'
    maya.cmds.addAttr(
        image_plane_shp, longName=attr, attributeType='double', defaultValue=0.0
    )
    node_attr = image_plane_shp + '.' + attr
    maya.cmds.setAttr(node_attr, keyable=True)
    lib_utils.force_connect_attr('time1.outTime', node_attr)

    attr = 'imageSequenceFirstFrame'
    maya.cmds.addAttr(
        image_plane_shp,
        longName=attr,
        niceName='First Frame',
        attributeType='long',
        defaultValue=0,
    )
    node_attr = image_plane_shp + '.' + attr
    maya.cmds.setAttr(node_attr, keyable=False)
    maya.cmds.setAttr(node_attr, channelBox=True)

    attr = 'imageSequenceFrameOutput'
    maya.cmds.addAttr(
        image_plane_shp,
        longName=attr,
        niceName='Frame Output',
        attributeType='double',
        defaultValue=0.0,
    )
    node_attr = image_plane_shp + '.' + attr
    maya.cmds.setAttr(node_attr, keyable=True)
    dst_node_attr = image_plane_shp + '.imageFrameNumber'
    lib_utils.force_connect_attr(node_attr, dst_node_attr)

    # Image Sequence details.
    maya.cmds.addAttr(
        image_plane_shp,
        longName='imageSequenceStartFrame',
        niceName='Start Frame',
        attributeType='long',
        defaultValue=0,
    )
    maya.cmds.addAttr(
        image_plane_shp,
        longName='imageSequenceEndFrame',
        niceName='End Frame',
        attributeType='long',
        defaultValue=0,
    )
    maya.cmds.addAttr(
        image_plane_shp,
        longName='imageSequencePadding',
        niceName='Padding',
        attributeType='long',
        defaultValue=0,
    )

    # Mesh Resolution attribute
    attr = 'meshResolution'
    maya.cmds.addAttr(
        image_plane_shp,
        longName=attr,
        attributeType='long',
        minValue=1,
        maxValue=256,
        defaultValue=32,
    )
    node_attr = image_plane_shp + '.' + attr
    maya.cmds.setAttr(node_attr, keyable=True)
    return


def create_transform_node(name_tfm, cam_tfm, cam_shp):
    """
    Create a default polygon image plane under camera.
    """
    assert isinstance(name_tfm, pycompat.TEXT_TYPE)
    mmapi.load_plugin()
    tfm = maya.cmds.createNode('mmImagePlaneTransform', name=name_tfm, parent=cam_tfm)

    # Create (dynamic) attributes.
    _create_transform_attrs(tfm)

    # Image plane depth be far away, but still visible.
    far_clip_plane = maya.cmds.getAttr(cam_tfm + '.farClipPlane')
    maya.cmds.setAttr(tfm + '.depth', far_clip_plane * 0.9)

    # Connect camera attributes
    attrs = [
        'focalLength',
        'horizontalFilmAperture',
        'verticalFilmAperture',
        'horizontalFilmOffset',
        'verticalFilmOffset',
    ]
    for attr in attrs:
        src = cam_shp + '.' + attr
        dst = tfm + '.' + attr
        if not maya.cmds.isConnected(src, dst):
            lib_utils.force_connect_attr(src, dst)

    return tfm


def create_shape_node(
    name_img_shp,
    tfm,
    cam_shp,
    poly_plane_node_network,
):
    """
    Convert mesh to a mmImagePlaneShape.
    """
    assert isinstance(name_img_shp, pycompat.TEXT_TYPE)

    img_plane_poly_shp = poly_plane_node_network.mesh_shape
    img_plane_poly_shp_original = poly_plane_node_network.mesh_shape_original
    poly_creator = poly_plane_node_network.plane_creator

    mmapi.load_plugin()
    shp = maya.cmds.createNode('mmImagePlaneShape', name=name_img_shp, parent=tfm)

    maya.cmds.setAttr(shp + '.localPositionX', channelBox=False)
    maya.cmds.setAttr(shp + '.localPositionY', channelBox=False)
    maya.cmds.setAttr(shp + '.localPositionZ', channelBox=False)
    maya.cmds.setAttr(shp + '.localScaleX', channelBox=False)
    maya.cmds.setAttr(shp + '.localScaleY', channelBox=False)
    maya.cmds.setAttr(shp + '.localScaleZ', channelBox=False)

    maya.cmds.setAttr(shp + '.cameraWidthInch', channelBox=False)
    maya.cmds.setAttr(shp + '.cameraHeightInch', channelBox=False)

    # Image plane hash will be used to stop updating the Viewport 2.0
    # shape when the lens distortion values stay the same.
    lens_eval_node = maya.cmds.createNode('mmLensEvaluate')
    lib_utils.force_connect_attr(cam_shp + '.outLens', lens_eval_node + '.inLens')
    lib_utils.force_connect_attr(lens_eval_node + '.outHash', shp + '.lensHashCurrent')

    maya.cmds.reorder(shp, back=True)
    maya.cmds.reorder(img_plane_poly_shp_original, back=True)
    maya.cmds.reorder(img_plane_poly_shp, back=True)

    _create_image_plane_shape_attrs(shp)

    # Nodes to drive the image plane shape.
    lib_utils.force_connect_attr(img_plane_poly_shp + '.outMesh', shp + '.geometryNode')
    lib_utils.force_connect_attr(cam_shp + '.message', shp + '.cameraNode')

    # The image drives the pixel aspect ratio of the image plane.
    lib_utils.force_connect_attr(shp + '.imagePixelAspect', tfm + '.pixelAspect')

    # Set the camera size of the image plane shape HUD.
    lib_utils.force_connect_attr(
        tfm + '.horizontalFilmAperture', shp + '.cameraWidthInch'
    )
    lib_utils.force_connect_attr(
        tfm + '.verticalFilmAperture', shp + '.cameraHeightInch'
    )

    # Mesh Resolution attr drives the plane sub-divisions.
    node_attr = shp + '.meshResolution'
    lib_utils.force_connect_attr(node_attr, poly_creator + '.subdivisionsWidth')
    lib_utils.force_connect_attr(node_attr, poly_creator + '.subdivisionsHeight')

    # Mesh doesn't need to be visible to drive the image plane shape
    # node drawing.
    maya.cmds.setAttr(img_plane_poly_shp + '.intermediateObject', 1)

    # Add extra message attributes for finding nodes during callbacks.
    maya.cmds.addAttr(shp, longName='imagePlaneShapeNode', attributeType='message')
    return shp


def set_image_sequence(shp, image_sequence_path, attr_name):
    assert maya.cmds.nodeType(shp) == 'mmImagePlaneShape'
    assert node_utils.attribute_exists(attr_name, shp) is True

    format_style = const_utils.IMAGE_SEQ_FORMAT_STYLE_FIRST_FRAME
    (
        file_pattern,
        _,
        _,
        _,
        _,
    ) = imageseq_utils.expand_image_sequence_path(image_sequence_path, format_style)
    first_frame_file_seq = file_pattern.replace('\\', '/')

    mmapi.load_plugin()
    try:
        image_width_height = maya.cmds.mmReadImage(
            first_frame_file_seq, query=True, widthHeight=True
        )
    except RuntimeError:
        image_width_height = None
        LOG.warn('Failed to read file: %r', first_frame_file_seq)

    if image_width_height is not None:
        image_width = image_width_height[0]
        image_height = image_width_height[1]

        if not node_utils.node_is_referenced(shp):
            maya.cmds.setAttr(shp + '.imageWidth', lock=False)
            maya.cmds.setAttr(shp + '.imageHeight', lock=False)

        maya.cmds.setAttr(shp + '.imageWidth', image_width)
        maya.cmds.setAttr(shp + '.imageHeight', image_height)

        if not node_utils.node_is_referenced(shp):
            maya.cmds.setAttr(shp + '.imageWidth', lock=True)
            maya.cmds.setAttr(shp + '.imageHeight', lock=True)

    format_style = const_utils.IMAGE_SEQ_FORMAT_STYLE_HASH_PADDED
    (
        file_pattern,
        start_frame,
        end_frame,
        pad_num,
        is_seq,
    ) = imageseq_utils.expand_image_sequence_path(image_sequence_path, format_style)

    maya.cmds.setAttr(shp + '.' + attr_name, file_pattern, type='string')

    if not node_utils.node_is_referenced(shp):
        maya.cmds.setAttr(shp + '.imageSequenceStartFrame', lock=False)
        maya.cmds.setAttr(shp + '.imageSequenceEndFrame', lock=False)
        maya.cmds.setAttr(shp + '.imageSequencePadding', lock=False)

    maya.cmds.setAttr(shp + '.imageSequenceFirstFrame', start_frame)
    maya.cmds.setAttr(shp + '.imageSequenceStartFrame', start_frame)
    maya.cmds.setAttr(shp + '.imageSequenceEndFrame', end_frame)
    maya.cmds.setAttr(shp + '.imageSequencePadding', pad_num)

    if not node_utils.node_is_referenced(shp):
        maya.cmds.setAttr(shp + '.imageSequenceStartFrame', lock=True)
        maya.cmds.setAttr(shp + '.imageSequenceEndFrame', lock=True)
        maya.cmds.setAttr(shp + '.imageSequencePadding', lock=True)
    return


def get_shape_node(image_plane_tfm):
    shape = None
    shapes = (
        maya.cmds.listRelatives(
            image_plane_tfm, shapes=True, fullPath=True, type='mmImagePlaneShape'
        )
        or []
    )
    if len(shapes) > 0:
        shape = shapes[0]
    return shape


def get_transform_node(image_plane_shp):
    tfm = None
    tfms = (
        maya.cmds.listRelatives(
            image_plane_shp, parent=True, fullPath=True, type='mmImagePlaneTransform'
        )
        or []
    )
    if len(tfms) > 0:
        tfm = tfms[0]
    return tfm


def get_image_plane_node_pair(node):
    node_type = maya.cmds.nodeType(node)
    assert node_type in ['mmImagePlaneShape', 'mmImagePlaneTransform']
    tfm = None
    shp = None
    if node_type == 'mmImagePlaneTransform':
        shp = get_shape_node(node)
        tfm = node
    else:
        # Given a shape, we can look at our parent node to find the
        # transform.
        tfm = get_transform_node(node)
        shp = node
    return tfm, shp
