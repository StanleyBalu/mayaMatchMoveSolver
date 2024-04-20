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
File Formats for loadmarker tool.

This file is used to automatically load all formats so the user
doesn't need to import each format individually.
"""

# Do not remove these imports, they are used to automatically load and
# add the different file formats to the FormatManager.
import mmSolver.utils.loadmarker.formats.pftrack2dt
import mmSolver.utils.loadmarker.formats.rz2
import mmSolver.utils.loadmarker.formats.tdetxt
import mmSolver.utils.loadmarker.formats.uvtrack
