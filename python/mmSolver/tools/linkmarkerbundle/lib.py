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
Library functions to perform 'Link / Unlink Marker Bundle' tool functions.
"""

import mmSolver.api as mmapi
import mmSolver.logger


LOG = mmSolver.logger.get_logger()


def link_marker_bundle(mkr_node, bnd_node):
    """
    Try to connect the two marker and bundle nodes.
    """
    mkr = mmapi.Marker(node=mkr_node)
    bnd = mmapi.Bundle(node=bnd_node)
    cam_from_mkr = mkr.get_camera()
    cam_from_mkr_uid = cam_from_mkr.get_shape_uid()

    # Check the bundle doesn't already have a marker attached to the
    # same camera as 'mkr'
    bad_mkr = None
    connected_mkr_list = bnd.get_marker_list()
    for conn_mkr in connected_mkr_list:
        conn_cam = conn_mkr.get_camera()
        conn_cam_uid = conn_cam.get_shape_uid()
        if conn_cam_uid == cam_from_mkr_uid:
            bad_mkr = conn_mkr
            break

    valid = bad_mkr is None
    if valid:
        # No problem, set the bundle.
        mkr.set_bundle(bnd)
    else:
        msg = 'Cannot link {mkr} to {bnd}; '
        msg += 'bundle is already connected to Marker {mkr2} under camera {cam}.'
        msg = msg.format(
            mkr=repr(mkr_node),
            bnd=repr(bnd_node),
            mkr2=repr(bad_mkr.get_node()),
            cam=repr(cam_from_mkr.get_shape_node()),
        )
        LOG.warning(msg)
    return valid
