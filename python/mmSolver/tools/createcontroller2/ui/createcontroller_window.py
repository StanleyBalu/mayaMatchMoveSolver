# Copyright (C) 2021 Patcha Saheb Binginapalli.
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
Window for the Create Controller tool.

Usage::

   import mmSolver.tools.createcontroller2.ui.createcontroller_window as createcontroller_window
   createcontroller_window.main()

"""

import mmSolver.ui.qtpyutils as qtpyutils
qtpyutils.override_binding_order()

import mmSolver.ui.Qt.QtCore as QtCore
import mmSolver.ui.Qt.QtGui as QtGui
import mmSolver.ui.Qt.QtWidgets as QtWidgets

import mmSolver.logger
import mmSolver.ui.uiutils as uiutils
import mmSolver.ui.helputils as helputils
import mmSolver.ui.commonmenus as commonmenus
import mmSolver.utils.tools as tools_utils
import mmSolver.utils.constant as const_utils
import mmSolver.tools.createcontroller2.ui.createcontroller_layout as createcontroller_layout

LOG = mmSolver.logger.get_logger()
baseModule, BaseWindow = uiutils.getBaseWindow()
WINDOW_TITLE = 'Create Controller'


def _open_help():
    src = helputils.get_help_source()
    page = 'tools_generaltools.html#create-controller'
    helputils.open_help_in_browser(page=page, help_source=src)
    return


class CreateControllerWindow(BaseWindow):
    name = 'CreateController2Window'

    def __init__(self, parent=None, name=None):
        super(CreateControllerWindow, self).__init__(parent, name=name)
        self.setupUi(self)
        self.addSubForm(createcontroller_layout.CreateControllerLayout)

        self.setWindowTitle(WINDOW_TITLE)
        self.setWindowFlags(QtCore.Qt.Tool)

        # Standard Buttons
        self.baseHideStandardButtons()
        self.applyBtn.show()
        self.closeBtn.show()
        self.applyBtn.setText('Create Controller')

        self.applyBtn.setSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        self.applyBtn.setMinimumWidth(160)
        self.applyBtn.clicked.connect(self.create_controller)

        self.closeBtn.setSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        self.closeBtn.setMinimumWidth(120)

        # Hide irrelevant stuff
        self.baseHideProgressBar()

        self.add_menus(self.menubar)
        self.menubar.show()

    def add_menus(self, menubar):
        edit_menu = QtWidgets.QMenu('Edit', menubar)
        commonmenus.create_edit_menu_items(
            edit_menu,
            reset_settings_func=self.reset_options)
        menubar.addMenu(edit_menu)

        help_menu = QtWidgets.QMenu('Help', menubar)
        commonmenus.create_help_menu_items(
            help_menu,
            tool_help_func=_open_help)
        menubar.addMenu(help_menu)

    def create_controller(self):
        form = self.getSubForm()
        ctx = tools_utils.tool_context(
            use_undo_chunk=True,
            restore_current_frame=True,
            use_dg_evaluation_mode=True,
            disable_viewport=True,
            disable_viewport_mode=const_utils.DISABLE_VIEWPORT_MODE_VP1_VALUE)
        with ctx:
            form.create_controller_button_clicked()
        return

    def reset_options(self):
        form = self.getSubForm()
        form.reset_options()


def main(show=True, auto_raise=True, delete=False):
    win = CreateControllerWindow.open_window(
        show=show,
        auto_raise=auto_raise,
        delete=delete
    )
    return win
