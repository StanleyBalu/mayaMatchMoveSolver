#!/usr/bin/env bash
#
# Copyright (C) 2019, 2022 David Cattermole.
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
# ---------------------------------------------------------------------
#
# Builds the Maya MatchMove Solver project.

# Maya
MAYA_VERSION=2023
MAYA_LOCATION=/usr/autodesk/maya2023/

# Executable names/paths used for build process.
PYTHON_EXE=python
CMAKE_EXE=cmake3
RUST_CARGO_EXE=cargo

# C++ Standard to use.
CXX_STANDARD=14

# The -e flag causes the script to exit as soon as one command returns
# a non-zero exit code.
set -ev

CWD=`pwd`

# These scripts assume 'RUST_CARGO_EXE' has been set to the Rust
# 'cargo' executable.
source "${CWD}/scripts/internal/build_rust_library_linux.bash" mmscenegraph

source "${CWD}/scripts/internal/build_mmSolver_linux.bash"

cd ${CWD}
