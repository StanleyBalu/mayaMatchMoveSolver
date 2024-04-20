# Copyright (C) 2022 Patcha Saheb Binginapalli.
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
The main component of the user interface for the camera/object
track scale rig bake window.
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import maya.cmds

import mmSolver.ui.qtpyutils as qtpyutils

qtpyutils.override_binding_order()

import mmSolver.ui.Qt.QtWidgets as QtWidgets

import mmSolver.logger
import mmSolver.utils.tools as tools_utils
import mmSolver.utils.constant as const_utils
import mmSolver.utils.time as time_utils
import mmSolver.tools.cameraobjectscaleadjust.constant as const
import mmSolver.tools.cameraobjectscaleadjust.ui.ui_cameraobjectscaleadjust_layout as ui_layout
import mmSolver.tools.loadmarker.lib.utils as cam_lib
import mmSolver.tools.cameraobjectscaleadjust.lib as lib


LOG = mmSolver.logger.get_logger()


def _transform_has_constraints(tfm_node):
    constraints = (
        maya.cmds.listRelatives(tfm_node, children=True, type='pointConstraint') or []
    )
    constraints += (
        maya.cmds.listRelatives(tfm_node, children=True, type='orientConstraint') or []
    )
    constraints += (
        maya.cmds.listRelatives(tfm_node, children=True, type='scaleConstraint') or []
    )
    constraints += (
        maya.cmds.listRelatives(tfm_node, children=True, type='parentConstraint') or []
    )
    has_constraints = len(constraints) > 0
    return has_constraints


def unlock_node_attrs(tfm_node):
    axes = ['x', 'y', 'z']
    attrs = ['t', 'r', 's']
    for axis in axes:
        for attr in attrs:
            node_attr = tfm_node + '.' + attr + axis
            if maya.cmds.getAttr(node_attr, lock=True):
                maya.cmds.setAttr(node_attr, lock=False)
    return


class CameraObjectScaleAdjustLayout(QtWidgets.QWidget, ui_layout.Ui_Form):
    def __init__(self, parent=None, *args, **kwargs):
        super(CameraObjectScaleAdjustLayout, self).__init__(*args, **kwargs)
        self.setupUi(self)

        # Create Button Groups (because we get compile errors if
        # trying to create these in the .ui file).
        self.scale_rig_type_btnGrp = QtWidgets.QButtonGroup()
        self.scale_rig_type_btnGrp.addButton(self.cameraTrackScaleRadioButton)
        self.scale_rig_type_btnGrp.addButton(self.bodyTrackScaleRadioButton)

        # Create connections
        self.sceneGetButton.clicked.connect(self.scene_get_button_clicked)
        self.rigsGetButton.clicked.connect(self.controls_get_button_clicked)
        self.cameraTrackScaleRadioButton.toggled.connect(self.scale_rig_options_changed)
        self.bodyTrackScaleRadioButton.toggled.connect(self.scale_rig_options_changed)

        self.populate_ui()

    def scale_rig_options_changed(self):
        if self.bodyTrackScaleRadioButton.isChecked() is True:
            enable_flag = False
            self.sceneLineEdit.clear()
        else:
            enable_flag = True
        self.set_widget_enable_flag(enable_flag)

    def reset_options(self):
        self.scaleRigNameLineEdit.clear()
        self.sceneLineEdit.clear()
        self.cameraListComboBox.setCurrentIndex(0)
        self.rigsLineEdit.clear()
        self.bodyTrackScaleRadioButton.setChecked(True)

    def populate_ui(self):
        # Add camera items
        all_camera_nodes = cam_lib.get_cameras()
        active_camera = cam_lib.get_active_viewport_camera()
        for cam in all_camera_nodes:
            cam_tfm = cam.get_transform_node()
            self.cameraListComboBox.addItem(str(cam_tfm))
        if active_camera:
            active_camera_tfm = active_camera.get_transform_node()
            for count, cam in enumerate(all_camera_nodes):
                if cam.get_transform_node() == active_camera_tfm:
                    self.cameraListComboBox.setCurrentIndex(count)
                    break
        self.set_widget_enable_flag(False)

    def set_widget_enable_flag(self, enable_flag_bool):
        self.sceneLabel.setEnabled(enable_flag_bool)
        self.sceneLineEdit.setEnabled(enable_flag_bool)
        self.sceneGetButton.setEnabled(enable_flag_bool)

    def scene_get_button_clicked(self):
        """Get Scene node selection."""
        selection = maya.cmds.ls(selection=True, long=True, transforms=True) or []
        if len(selection) != 1:
            LOG.warn('Please select exactly one scene.')
            return
        has_constraints = _transform_has_constraints(selection)
        if has_constraints is True:
            LOG.warn('Scene has constraints already.')
            return
        self.sceneLineEdit.setText(str(selection[0]))
        unlock_node_attrs(selection[0])

    def controls_get_button_clicked(self):
        """Get Rig node(s) selection."""
        selection = maya.cmds.ls(selection=True, long=True, transforms=True) or []
        if len(selection) < 1:
            LOG.warn('Please select at least one rig control.')
            return
        for node in selection:
            unlock_node_attrs(node)
            has_constraints = _transform_has_constraints(node)
            if has_constraints is True:
                LOG.warn('Rig(s) have constraints already.')
                return
        text = ''
        for node in selection:
            text += str(node) + ', '
        text = text.strip(' ').strip(',')
        self.rigsLineEdit.setText(text)

    def create_scale_rig_button_clicked(self):
        name = self.scaleRigNameLineEdit.text() or None
        camera = self.cameraListComboBox.currentText() or None
        scene = self.sceneLineEdit.text() or None
        rig_controls = self.rigsLineEdit.text() or None
        if rig_controls:
            rig_controls = rig_controls.split(',')
        body_scale_checked = self.bodyTrackScaleRadioButton.isChecked()
        camera_scale_checked = self.cameraTrackScaleRadioButton.isChecked()
        framerange = time_utils.get_maya_timeline_range_inner()

        ctx = tools_utils.tool_context(
            use_undo_chunk=True,
            restore_current_frame=True,
            use_dg_evaluation_mode=True,
            disable_viewport=True,
            disable_viewport_mode=const_utils.DISABLE_VIEWPORT_MODE_VP1_VALUE,
        )
        with ctx:
            if body_scale_checked is True:
                if None in [name, camera, rig_controls]:
                    LOG.warn('Please select scale rig name, camera and rigs.')
                    return
                lib.create_scale_rig(
                    name,
                    camera,
                    scene,
                    rig_controls,
                    framerange,
                    const.SCALE_RIG_TYPE_OBJECT_TRACK,
                )
            if camera_scale_checked is True:
                if None in [name, camera, scene, rig_controls]:
                    LOG.warn('Please select scale rig name, camera, scene and rigs.')
                    return
                lib.create_scale_rig(
                    name,
                    camera,
                    scene,
                    rig_controls,
                    framerange,
                    const.SCALE_RIG_TYPE_CAMERA_TRACK,
                )
        return
