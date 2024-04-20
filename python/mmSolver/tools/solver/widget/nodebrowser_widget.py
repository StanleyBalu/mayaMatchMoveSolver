# Copyright (C) 2019 David Cattermole.
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
Browser widget class for visualising nodes.

This class is designed to be sub-classed with functionality for final use.
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import mmSolver.ui.qtpyutils as qtpyutils

qtpyutils.override_binding_order()

import mmSolver.ui.Qt.QtCore as QtCore
import mmSolver.ui.Qt.QtWidgets as QtWidgets

import mmSolver.logger
import mmSolver.tools.solver.widget.ui_nodebrowser_widget as ui_nodebrowser_widget

LOG = mmSolver.logger.get_logger()


class NodeBrowserWidget(QtWidgets.QWidget):

    nodeAdded = QtCore.Signal()
    nodeRemoved = QtCore.Signal()
    viewUpdated = QtCore.Signal()
    dataChanged = QtCore.Signal()

    def __init__(self, parent=None, *args, **kwargs):
        super(NodeBrowserWidget, self).__init__(parent=parent, *args, **kwargs)
        self.ui = ui_nodebrowser_widget.Ui_Form()
        self.ui.setupUi(self)

        # Add and Remove buttons
        self.ui.add_toolButton.clicked.connect(self.addClicked)
        self.ui.remove_toolButton.clicked.connect(self.removeClicked)
        return

    def addClicked(self):
        raise NotImplementedError

    def removeClicked(self):
        raise NotImplementedError
