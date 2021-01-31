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
Window for the Fast bake tool.

Usage::

   import mmSolver.tools.fastbake.ui.fastbake_window as fastbake_window
   fastbake_window.main()

"""

import mmSolver.ui.qtpyutils as qtpyutils
qtpyutils.override_binding_order()

import Qt.QtCore as QtCore
import Qt.QtGui as QtGui
import Qt.QtWidgets as QtWidgets

import mmSolver.logger
import mmSolver.ui.uiutils as uiutils
import mmSolver.ui.helputils as helputils
import mmSolver.ui.commonmenus as commonmenus
import mmSolver.tools.fastbake.constant as const
import mmSolver.tools.fastbake.ui.fastbake_layout as fastbake_layout
import mmSolver.tools.fastbake.tool as tool

LOG = mmSolver.logger.get_logger()
baseModule, BaseWindow = uiutils.getBaseWindow()


def _open_help():
    src = helputils.get_help_source()
    page = 'tools_generaltools.html#fast-bake'
    helputils.open_help_in_browser(page=page, help_source=src)
    return


class FastBakeWindow(BaseWindow):
    name = 'FastBakeWindow'

    def __init__(self, parent=None, name=None):
        super(FastBakeWindow, self).__init__(parent, name=name)
        self.setupUi(self)
        self.addSubForm(fastbake_layout.FastBakeLayout)

        self.setWindowTitle(const.WINDOW_TITLE)
        self.setWindowFlags(QtCore.Qt.Tool)

        # Standard Buttons
        self.baseHideStandardButtons()
        self.applyBtn.show()
        self.closeBtn.show()
        self.applyBtn.setText('Bake')

        self.applyBtn.clicked.connect(tool.main)

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

    def reset_options(self):
        form = self.getSubForm()
        form.reset_options()
        return


def main(show=True, auto_raise=True, delete=False):
    win = FastBakeWindow.open_window(
        show=show,
        auto_raise=auto_raise,
        delete=delete
    )
    return win
