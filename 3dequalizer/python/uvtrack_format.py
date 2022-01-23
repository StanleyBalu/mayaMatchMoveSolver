# -*- mode: python-mode; python-indent-offset: 4 -*-
#
# Copyright (C) 2018, 2019, 2020, 2021 David Cattermole.
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
# ----------------------------------------------------------------------
"""
Generate track data from the given 2D points in 3DEqualizer.

Files in 'UV Track' format should have file extension '.uv'.

See ./python/mmSolver/tools/loadmarker/lib/formats/uvtrack.py for details
of the '.uv' file format.
"""
# 3DE4.script.hide:     true

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import sys
import json

import vl_sdv
import tde4


IS_PYTHON_2 = sys.version_info[0] == 2
if IS_PYTHON_2 is True:
    TEXT_TYPE = basestring
    INT_TYPES = (int, long)
else:
    TEXT_TYPE = str
    INT_TYPES = (int, )

# UV Track format
# This is copied from 'mmSolver.tools.loadmarker.constant module',
UV_TRACK_FORMAT_VERSION_UNKNOWN = -1
UV_TRACK_FORMAT_VERSION_1 = 1
UV_TRACK_FORMAT_VERSION_2 = 2
UV_TRACK_FORMAT_VERSION_3 = 3
UV_TRACK_FORMAT_VERSION_4 = 4

UV_TRACK_HEADER_VERSION_2 = {
    'version': UV_TRACK_FORMAT_VERSION_2,
}

UV_TRACK_HEADER_VERSION_3 = {
    'version': UV_TRACK_FORMAT_VERSION_3,
}

UV_TRACK_HEADER_VERSION_4 = {
    'version': UV_TRACK_FORMAT_VERSION_4,
}

# Preferred UV Track format version (changes the format
# version used for writing data).
UV_TRACK_FORMAT_VERSION_PREFERRED = UV_TRACK_FORMAT_VERSION_4

# Do we have support for new features of 3DE tde4 module?
SUPPORT_PERSISTENT_ID = 'getPointPersistentID' in dir(tde4)
SUPPORT_CAMERA_FRAME_OFFSET = 'getCameraFrameOffset' in dir(tde4)
SUPPORT_POINT_WEIGHT_BY_FRAME = 'getPointWeightByFrame' in dir(tde4)
SUPPORT_CLIPBOARD = 'setClipboardString' in dir(tde4)
SUPPORT_POINT_VALID_MODE = 'getPointValidMode' in dir(tde4)
SUPPORT_POINT_SURVEY_XYZ_ENABLED = 'getPointSurveyXYZEnabledFlags' in dir(tde4)
SUPPORT_RS_ENABLED = 'getCameraRollingShutterEnabledFlag' in dir(tde4)
SUPPORT_RS_DISTANCE = 'getCameraRollingShutterContentDistance' in dir(tde4)
SUPPORT_PROJECT_NOTES = 'getProjectNotes' in dir(tde4)

# Rolling Shutter values stored in the 3DE4 Attribute Editor Project
# Notes.
RS_DISTANCE_DEFAULT_FALLBACK = 100.0
RS_DISTANCE_PROJECT_NOTES_LABEL = 'RS Content Distance = {number}'
RS_DISTANCE_KEY = 'rscontentdistance='


def _parse_rs_distance_line(line, original_line):
    value = None
    msg = ('Could not get Rolling Shutter Content Distance from '
           '3DE Project Notes.\nLine is incorrectly formatted: %r\n'
           'Correct formatting is: %r')
    start, sep, end = line.partition(RS_DISTANCE_KEY)
    found = len(sep) > 0
    if found is True:
        try:
            value = float(end)
        except ValueError:
            print(msg % (original_line, RS_DISTANCE_PROJECT_NOTES_LABEL))
    return value


def _filter_project_notes_line(line):
    line_to_parse = line.lower()

    # Remove excessive whitespace between words.
    whitespace_chars = [' ', '\t', '\r', '\v', '\f']
    for char in whitespace_chars:
        line_to_parse = line_to_parse.replace(char, '')
    return line_to_parse


def _parse_rs_distance_from_project_notes(notes):
    rs_distance = None
    lines = notes.split('\n')
    for line in lines:
        line_to_parse = _filter_project_notes_line(line)
        if RS_DISTANCE_KEY in line_to_parse:
            rs_distance = _parse_rs_distance_line(line_to_parse, line)
    return rs_distance


def _get_rs_distance_from_project_notes():
    rs_distance = None
    if SUPPORT_PROJECT_NOTES is True:
        notes = tde4.getProjectNotes()
        rs_distance = _parse_rs_distance_from_project_notes(notes)
    return rs_distance


def set_rs_distance_into_project_notes(rs_distance):
    assert isinstance(rs_distance, float)
    assert SUPPORT_PROJECT_NOTES is True
    notes = tde4.getProjectNotes()
    label = RS_DISTANCE_PROJECT_NOTES_LABEL.format(number=rs_distance)
    existing_value = _get_rs_distance_from_project_notes()
    if existing_value is None:
        # Add to top of the project notes, in case someone is using
        # the bottom of the notes to embed JSON data or something like
        # that.
        new_notes = label + '\n' + notes
    else:
        lines = notes.split('\n')
        new_lines = []
        for line in lines:
            line_to_parse = _filter_project_notes_line(line)
            if RS_DISTANCE_KEY in line_to_parse:
                line = label
            new_lines.append(line)
        new_notes = '\n'.join(new_lines)
    tde4.setProjectNotes(new_notes)
    return


def get_rs_distance(camera):
    if SUPPORT_RS_DISTANCE is True:
        # For 3DE4 Release 6 and above.
        rs_distance = tde4.getCameraRollingShutterContentDistance(camera)
    else:
        # For 3DE4 Release 5 and below, use the default content
        # distance value, unless the user overrides it with a
        # special tag in the 3DE project notes.
        rs_distance = _get_rs_distance_from_project_notes()
        if rs_distance is None:
            rs_distance = RS_DISTANCE_DEFAULT_FALLBACK
    assert isinstance(rs_distance, float)
    return rs_distance


def set_rs_distance(camera, rs_distance):
    assert isinstance(rs_distance, float)
    if SUPPORT_RS_DISTANCE is True:
        # For 3DE4 Release 6 and above.
        tde4.setCameraRollingShutterContentDistance(camera, rs_distance)
    elif SUPPORT_PROJECT_NOTES is True:
        # For 3DE4 Release 2 to Release 5 (with support for project
        # notes), set a special tag in the 3DE project notes.
        set_rs_distance_into_project_notes(rs_distance)
    return


def _apply_rs_correction(dt, q_minus, q_center, q_plus):
    """
    Apply time-blend between three different 3D positions.

    :param dt: The blend value between all three positions.
    :param q_minus: First 3D position.
    :param q_center: Middle 3D position.
    :param q_plus: Last 3D position.

    :return: 2D point blended.
    """
    a = q_center
    b = (q_plus - q_minus) / 2.0
    c = -q_center + (q_plus + q_minus) / 2.0
    return a + dt * b + dt * dt * c


def _convert_2d_to_3d_point_undistort(point_group, camera,
                                      fbw, fbh, lcox, lcoy,
                                      camera_fov,
                                      frame, pos, depth):
    """
    Convert a 2D point (undistorted) into a 3D point, in world space.

    :param point_group: Camera Point Group for camera.
    :param camera: The camera to use for rolling shutter calculations.
    :param fbw: Camera's film back width value.
    :param fbh: Camera's film back height value.
    :param lcox: Camera lens lens center offset X value.
    :param lcoy: Camera lens lens center offset Y value.
    :param camera_fov: Camera Field of View as list of left, right,
                       bottom and top.
    :param frame: The 2D point's frame number (in internal 3DE frame numbers).
    :param pos: Input 2D data.
    :param depth: The content distance to calculate rolling shutter at.

    :return: Corrected 2D point.
    :rtype: vec2d
    """
    focal = tde4.getCameraFocalLength(camera, frame)
    r3d = vl_sdv.mat3d(tde4.getPGroupRotation3D(point_group, camera, frame))
    p3d = vl_sdv.vec3d(tde4.getPGroupPosition3D(point_group, camera, frame))
    left, right, bottom, top = camera_fov

    p2d = [0, 0]
    p2d[0] = (pos[0] - left) / (right - left)
    p2d[1] = (pos[1] - bottom) / (top - bottom)
    p2d = tde4.removeDistortion2D(camera, frame, p2d)

    p2d_cm = vl_sdv.vec2d((p2d[0] - 0.5) * fbw - lcox,
                          (p2d[1] - 0.5) * fbh - lcoy)
    homogeneous_point = r3d * vl_sdv.vec3d(p2d_cm[0], p2d_cm[1], -focal).unit()
    out_point = homogeneous_point * depth + p3d
    return out_point


def _remove_rs_from_2d_point(point_group, camera, frame, input_2d, depth):
    """
    Correct Rolling Shutter for the given input_2d point data, on frame.

    :param point_group: Camera Point Group for camera.
    :param camera: The camera to use for rolling shutter calculations.
    :param frame: The 2D point's frame number (in internal 3DE frame numbers).
    :param input_2d: Input 2D data.
    :param depth: The content distance to calculate rolling shutter at.

    :return: 2D point with corrected position.
    :rtype: [float, float]
    """
    assert isinstance(input_2d, vl_sdv.vec2d)
    num_frames = tde4.getCameraNoFrames(camera)
    if num_frames == 1:
        return input_2d

    # Static camera and lens values.
    camera_fps = tde4.getCameraFPS(camera)
    camera_fov = tde4.getCameraFOV(camera)
    lens = tde4.getCameraLens(camera)
    fbw = tde4.getLensFBackWidth(lens)
    fbh = tde4.getLensFBackHeight(lens)
    lcox = tde4.getLensLensCenterX(lens)
    lcoy = tde4.getLensLensCenterY(lens)
    rs_time_shift = tde4.getCameraRollingShutterTimeShift(camera)
    rs_value = rs_time_shift * camera_fps

    # Sample at previous frame
    prev_pos = vl_sdv.vec3d(0, 0, 0)
    prev_frame = frame - 1
    if frame > 1:
        prev_pos = _convert_2d_to_3d_point_undistort(
            point_group, camera,
            fbw, fbh, lcox, lcoy,
            camera_fov,
            prev_frame, input_2d, depth)

    # Sample at next frame
    next_pos = vl_sdv.vec3d(0, 0, 0)
    next_frame = frame + 1
    if frame < num_frames:
        next_pos = _convert_2d_to_3d_point_undistort(
            point_group, camera,
            fbw, fbh, lcox, lcoy,
            camera_fov,
            next_frame, input_2d, depth)

    # Sample at current frame
    curr_pos = _convert_2d_to_3d_point_undistort(
        point_group, camera,
        fbw, fbh, lcox, lcoy,
        camera_fov,
        frame, input_2d, depth)

    # Blend previous, next and current frame values based on the
    # position of the 2D point vertical position and the rolling
    # shutter value.
    if frame == 1:
        prev_pos = curr_pos + (curr_pos - next_pos)
    if frame == num_frames:
        next_pos = curr_pos + (curr_pos - prev_pos)
    t = rs_value * (1.0 - input_2d[1])
    curr_pos = _apply_rs_correction(-t, prev_pos, curr_pos, next_pos)

    # Back-projection
    focal = tde4.getCameraFocalLength(camera, frame)
    r3d = vl_sdv.mat3d(tde4.getPGroupRotation3D(point_group, camera, frame))
    p3d = vl_sdv.vec3d(tde4.getPGroupPosition3D(point_group, camera, frame))
    d = r3d.trans() * (curr_pos - p3d)
    p2d = [0, 0]
    p2d[0] = (d[0] * focal / (-d[2] * fbw)) + (lcox / fbw) + 0.5
    p2d[1] = (d[1] * focal / (-d[2] * fbh)) + (lcoy / fbh) + 0.5
    p = tde4.applyDistortion2D(camera, frame, p2d)
    left, right, bottom, top = camera_fov
    p = vl_sdv.vec2d((p[0] * (right - left)) + left,
                     (p[1] * (top - bottom)) + bottom)
    v = (input_2d + (input_2d - p)).list()
    return v


def _get_point_valid_mode(point_group, point):
    """
    Is the point valid in various positions?

    This function has backwards compatibility built-in, so older
    versions of 3DE can use it reliably and get consistent results.

    :param point_group: The 3DE Point Group containing 'point'
    :type point_group: str

    :param point: 3DE Point to check.
    :type point: str

    :returns: A 'point_valid_mode', 1 of 3 strings;
              'POINT_VALID_INSIDE_FOV',
              'POINT_VALID_INSIDE_FRAME', or
              'POINT_VALID_ALWAYS'.
    :rtype: str
    """
    valid_mode = 'POINT_VALID_INSIDE_FRAME'
    if SUPPORT_POINT_VALID_MODE is True:
        valid_mode = tde4.getPointValidMode(point_group, point)
    else:
        valid_outside = tde4.getPointValidOutsideFOVFlag(point_group, point)
        if valid_outside == 1:
            valid_mode = 'POINT_VALID_ALWAYS'
    return valid_mode


def _is_valid_position(pos_2d, camera_fov, valid_mode):
    """
    Is the 2D position is valid for the point 'valid_mode' and camera FOV?

    :param pos_2d: 2D point position to check.
    :type pos_2d: [float, float]

    :param camera_fov: The Camera FOV as given by 'tde4.getCameraFOV' command.
    :type camera_fov: [float, float, float, float]

    :param valid_mode: The point valid mode, as given by
                       'tde4.getPointValidMode' command.
    :type valid_mode: str

    :returns: If the 2D position given is valid, based on the 'valid_mode'.
    :rtype: bool
    """
    value = True
    if valid_mode == 'POINT_VALID_ALWAYS':
        pass
    elif valid_mode == 'POINT_VALID_INSIDE_FRAME':
        if ((pos_2d[0] < 0.0)
                or (pos_2d[0] > 1.0)
                or (pos_2d[1] < 0.0)
                or (pos_2d[1] > 1.0)):
            value = False
    elif valid_mode == 'POINT_VALID_INSIDE_FOV':
        left, right, bottom, top = camera_fov
        if ((pos_2d[0] < left)
                or (pos_2d[0] > right)
                or (pos_2d[1] < bottom)
                or (pos_2d[1] > top)):
            value = False
    return value


def _get_point_weight(point_group, point, camera, frame):
    """
    Get the 2D point weight.

    This function has backwards compatibility built-in, so older
    versions of 3DE can use it reliably and get consistent results.

    :param point_group: The 3DE Point Group containing 'point'
    :type point_group: str

    :param point: 3DE Point to query.
    :type point: str

    :param camera: The 3DE Camera containing the 3DE 'point'.
    :type camera: str

    :param frame: 3DE Frame number (1-based) to get weight from.
    :type frame: float

    :returns: A floating-point weight value.
    :rtype: float
    """
    weight = 1.0
    if SUPPORT_POINT_WEIGHT_BY_FRAME is True:
        weight = tde4.getPointWeightByFrame(
            point_group,
            point,
            camera,
            frame
        )
    return weight


def _get_3d_data_from_point(point_group, point):
    """
    Get 3D data structure from 3DE point.

    :param point_group: 3DE point group id.
    :type point_group: str

    :param point: 3DE point id.
    :type point: str

    :returns: Dictionary of the point 3d.
    :rtype: dict
    """
    has_pos = tde4.isPointCalculated3D(point_group, point)
    pos_3d = (None, None, None)
    if has_pos:
        pos_3d = tde4.getPointCalcPosition3D(point_group, point)

    x_lock = False
    y_lock = False
    z_lock = False
    survey_mode = tde4.getPointSurveyMode(point_group, point)
    if survey_mode not in ['SURVEY_FREE']:
        x_lock = True
        y_lock = True
        z_lock = True
        if SUPPORT_POINT_SURVEY_XYZ_ENABLED is True:
            xyz_lock = tde4.getPointSurveyXYZEnabledFlags(point_group, point)
            x_lock, y_lock, z_lock = xyz_lock

    point_3d_data = {
        'x': pos_3d[0],
        'y': pos_3d[1],
        'z': pos_3d[2],
        'x_lock': bool(x_lock),
        'y_lock': bool(y_lock),
        'z_lock': bool(z_lock),
    }
    return point_3d_data


def generate(point_group, camera, points, fmt=None, **kwargs):
    """
    Return a str, ready to be written to a text file.

    :param point_group: The 3DE Point Group containing 'points'
    :type point_group: str

    :param camera: The 3DE Camera containing 2D 'points' data.
    :type camera: str

    :param points: The list of 3DE Points representing 2D data to
                   save.
    :type points: list of str

    :param fmt: The format to generate, either
                UV_TRACK_FORMAT_VERSION_1, UV_TRACK_FORMAT_VERSION_2
                or UV_TRACK_FORMAT_VERSION_3.
    :type fmt: None or UV_TRACK_FORMAT_VERSION_*

    Supported 'kwargs':
    - undistort - (True or False) Should points be undistorted? (Format
                   v1 and v2)

    - start_frame - (int) Frame '1' 3DE should be mapped to this
                     value. (Format v1, v2 and v3)

    - rs_distance - (None or float) The rolling shutter (RS) content
                    distance. If None, no RS is calculated.
    """
    if fmt is None:
        fmt = UV_TRACK_FORMAT_VERSION_PREFERRED
    data = ''
    if fmt == UV_TRACK_FORMAT_VERSION_1:
        data = _generate_v1(point_group, camera, points, **kwargs)
    elif fmt == UV_TRACK_FORMAT_VERSION_2:
        data = _generate_v2(point_group, camera, points, **kwargs)
    elif fmt == UV_TRACK_FORMAT_VERSION_3:
        data = _generate_v3(point_group, camera, points, **kwargs)
    elif fmt == UV_TRACK_FORMAT_VERSION_4:
        data = _generate_v4(point_group, camera, points, **kwargs)
    return data


def _generate_v1(point_group, camera, points,
                 start_frame=None,
                 undistort=False,
                 rs_distance=None):
    """
    Generate the UV file format contents, using a basic ASCII format.

    Each point will store:
    - Point name
    - X, Y position (in UV coordinates, per-frame)
    - Point weight (per-frame)

    :param point_group: The 3DE Point Group containing 'points'
    :type point_group: str

    :param camera: The 3DE Camera containing 2D 'points' data.
    :type camera: str

    :param points: The list of 3DE Points representing 2D data to
                   save.
    :type points: list of str

    :param start_frame: The frame number to be considered at
                       'first frame'. Defaults to 1001 if
                       set to None.
    :type start_frame: None or int

    :param undistort: Should we apply undistortion to the 2D points
                      data? Yes or no.
    :type undistort: bool

    :param rs_distance: If not None, correct rolling shutter effects on
        the 2D points at the content distance rs_distance.
    :type rs_distance: None or float

    :returns: A ASCII format string, with the UV Track data in it.
    :rtype: str
    """
    assert isinstance(point_group, TEXT_TYPE)
    assert isinstance(camera, TEXT_TYPE)
    assert isinstance(points, (list, tuple))
    assert start_frame is None or isinstance(start_frame, int)
    assert isinstance(undistort, bool)
    assert rs_distance is None or isinstance(rs_distance, float)
    if start_frame is None:
        start_frame = 1001
    data_str = ''

    cam_num_frames = tde4.getCameraNoFrames(camera)
    camera_fov = tde4.getCameraFOV(camera)

    if len(points) == 0:
        return data_str

    frame0 = int(start_frame)
    frame0 -= 1

    data_str += '{0:d}\n'.format(len(points))

    for point in points:
        name = tde4.getPointName(point_group, point)
        c2d = tde4.getPointPosition2DBlock(
            point_group, point, camera,
            1, cam_num_frames
        )
        valid_mode = _get_point_valid_mode(point_group, point)

        # Write per-frame position data
        num_valid_frame = 0
        pos_list = []
        weight_list = []
        frame = 1  # 3DE starts at frame '1' regardless of the 'start-frame'.
        for v in c2d:
            if v[0] == -1.0 or v[1] == -1.0:
                # No valid data here.
                frame += 1
                continue

            # Does the 2D point go outside the camera FOV? Is that ok?
            valid = tde4.isPointPos2DValid(
                point_group,
                point,
                camera,
                frame
            )
            if valid == 0:
                # No valid data here.
                frame += 1
                continue

            # Check if we're inside the FOV / Frame or not.
            valid_pos = _is_valid_position(v, camera_fov, valid_mode)
            if valid_pos is False:
                frame += 1
                continue

            # Number of points with valid positions
            num_valid_frame += 1

            f = frame + frame0
            if rs_distance is not None:
                v = vl_sdv.vec2d(v[0], v[1])
                v = _remove_rs_from_2d_point(
                    point_group, camera, frame, v, rs_distance)
            if undistort is True:
                v = tde4.removeDistortion2D(camera, frame, v)
            weight = _get_point_weight(point_group, point, camera, frame)

            pos_list.append((f, v))
            weight_list.append((f, weight))
            frame += 1

        # add data
        data_str += name + '\n'
        data_str += '{0:d}\n'.format(num_valid_frame)
        for pos_data, weight_data in zip(pos_list, weight_list):
            f = pos_data[0]
            v = pos_data[1]
            w = weight_data[1]
            assert f == weight_data[0]
            data_str += '%d %.15f %.15f %.8f\n' % (f, v[0], v[1], w)

    return data_str


def _generate_camera_data(camera, lens, frame0):
    camera_data = {}
    cam_num_frames = tde4.getCameraNoFrames(camera)

    img_width = tde4.getCameraImageWidth(camera)
    img_height = tde4.getCameraImageHeight(camera)
    camera_data['resolution'] = (img_width, img_height)

    film_back_x = tde4.getLensFBackWidth(lens)
    film_back_y = tde4.getLensFBackHeight(lens)
    camera_data['film_back_cm'] = (film_back_x, film_back_y)

    lco_x = tde4.getLensLensCenterX(lens)
    lco_y = tde4.getLensLensCenterY(lens)
    camera_data['lens_center_offset_cm'] = (lco_x, lco_y)

    camera_data['per_frame'] = []
    for frame in range(1, cam_num_frames):
        # Note: In 3DEqualizer, film back and lens center is assumed
        # to be static, only focal length can be dynamic.
        focal_length = tde4.getCameraFocalLength(camera, frame)
        frame_data = {
            'frame': frame + frame0,
            'focal_length_cm': focal_length,
        }
        camera_data['per_frame'].append(frame_data)

    return camera_data


def _generate_v2_v3_and_v4(point_group, camera, points,
                           version=None,
                           **kwargs):
    """
    Generate the UV file format contents, using JSON format.

    Set the individual _generate_v2 or _generate_v3 functions for
    details of what is stored.

    :param point_group: The 3DE Point Group containing 'points'
    :type point_group: str

    :param camera: The 3DE Camera containing 2D 'points' data.
    :type camera: str

    :param points: The list of 3DE Points representing 2D data to
                   save.
    :type points: list of str

    :param version: The version of file to generate,
                    UV_TRACK_FORMAT_VERSION_2 or
                    UV_TRACK_FORMAT_VERSION_3.
    :type version: int

    :param start_frame: Format v2 and v3; The frame number to be
                        considered at 'first frame'.
                        Defaults to 1001 if set to None.
    :type start_frame: None or int

    :param undistort: Format v2; Should we apply undistortion to the 2D
                      points data? Yes or no.
    :type undistort: bool

    :param rs_distance: If not None, correct rolling shutter effects on
        the 2D points at the content distance rs_distance.
    :type rs_distance: None or float

    :returns: A JSON format string, with the UV Track data in it.
    :rtype: str
    """
    assert isinstance(point_group, TEXT_TYPE)
    assert isinstance(camera, TEXT_TYPE)
    assert isinstance(points, (list, tuple))
    assert isinstance(version, INT_TYPES)
    assert version in [UV_TRACK_FORMAT_VERSION_2,
                       UV_TRACK_FORMAT_VERSION_3,
                       UV_TRACK_FORMAT_VERSION_4]

    start_frame = kwargs.get('start_frame')
    assert start_frame is None or isinstance(start_frame, int)
    if start_frame is None:
        start_frame = 1001

    undistort = None
    if version == UV_TRACK_FORMAT_VERSION_2:
        undistort = kwargs.get('undistort')
        assert isinstance(undistort, bool)

    rs_distance = kwargs.get('rs_distance')
    assert rs_distance is None or isinstance(rs_distance, float)

    data = None
    if version == UV_TRACK_FORMAT_VERSION_2:
        data = UV_TRACK_HEADER_VERSION_2.copy()
    elif version == UV_TRACK_FORMAT_VERSION_3:
        data = UV_TRACK_HEADER_VERSION_3.copy()
    elif version == UV_TRACK_FORMAT_VERSION_4:
        data = UV_TRACK_HEADER_VERSION_4.copy()
    else:
        raise ValueError("Version number is invalid; %r" % version)

    # Static camera and lens values.
    cam_num_frames = tde4.getCameraNoFrames(camera)
    camera_fov = tde4.getCameraFOV(camera)

    if len(points) == 0:
        return ''

    frame0 = int(start_frame)
    frame0 -= 1

    data['num_points'] = len(points)
    data['is_undistorted'] = None
    if version == UV_TRACK_FORMAT_VERSION_2:
        data['is_undistorted'] = bool(undistort)

    data['points'] = []
    for point in points:
        point_data = {}

        # Query point information
        name = tde4.getPointName(point_group, point)
        uid = None
        if SUPPORT_PERSISTENT_ID is True:
            uid = tde4.getPointPersistentID(point_group, point)
        point_set = tde4.getPointSet(point_group, point)
        point_set_name = None
        if point_set is not None:
            point_set_name = tde4.getSetName(point_group, point_set)
        point_data['name'] = name
        point_data['id'] = uid
        point_data['set_name'] = point_set_name
        valid_mode = _get_point_valid_mode(point_group, point)

        # Get the 3D point position
        if version in [UV_TRACK_FORMAT_VERSION_3,
                       UV_TRACK_FORMAT_VERSION_4]:
            point_data['3d'] = _get_3d_data_from_point(point_group, point)

        # Write per-frame position data
        frame = 1  # 3DE starts at frame '1' regardless of the 'start frame'.
        point_data['per_frame'] = []
        pos_block = tde4.getPointPosition2DBlock(
            point_group, point, camera,
            1, cam_num_frames
        )
        for pos in pos_block:
            if pos[0] == -1.0 or pos[1] == -1.0:
                # No valid data here.
                frame += 1
                continue

            # Is the 2D point obscured?
            valid = tde4.isPointPos2DValid(
                point_group,
                point,
                camera,
                frame
            )
            if valid == 0:
                # No valid data here.
                frame += 1
                continue

            # Check if we're inside the FOV / Frame or not.
            valid_pos = _is_valid_position(pos, camera_fov, valid_mode)
            if valid_pos is False:
                frame += 1
                continue

            pos_undist = pos
            if rs_distance is not None:
                pos_undist = vl_sdv.vec2d(pos_undist[0], pos_undist[1])
                pos_undist = _remove_rs_from_2d_point(
                    point_group, camera, frame, pos_undist, rs_distance)
            if undistort is True or undistort is None:
                pos_undist = tde4.removeDistortion2D(
                    camera, frame, pos_undist)
            weight = _get_point_weight(point_group, point, camera, frame)

            f = frame + frame0
            frame_data = {
                'frame': f,
                'pos': pos_undist,
                'weight': weight
            }
            if version in [UV_TRACK_FORMAT_VERSION_3,
                           UV_TRACK_FORMAT_VERSION_4]:
                frame_data['pos_dist'] = pos
            point_data['per_frame'].append(frame_data)
            frame += 1

        data['points'].append(point_data)

    if version == UV_TRACK_FORMAT_VERSION_4:
        lens = tde4.getCameraLens(camera)
        data['camera'] = _generate_camera_data(camera, lens, frame0)

    data_str = json.dumps(data)
    return data_str


def _generate_v2(point_group, camera, points,
                 start_frame=None,
                 undistort=False,
                 rs_distance=None):
    """
    Generate the UV file format contents, using JSON format.

    Each point will store:
    - Point name
    - X, Y position (in UV coordinates, per-frame)
    - Point weight (per-frame)
    - Point Set name
    - Point 'Persistent ID'

    :param point_group: The 3DE Point Group containing 'points'
    :type point_group: str

    :param camera: The 3DE Camera containing 2D 'points' data.
    :type camera: str

    :param points: The list of 3DE Points representing 2D data to
                   save.
    :type points: list of str

    :param start_frame: The frame number to be considered at
                       'first frame'. Defaults to 1001 if
                       set to None.
    :type start_frame: None or int

    :param undistort: Should we apply undistortion to the 2D points
                      data? Yes or no.
    :type undistort: bool

    :param rs_distance: If not None, correct rolling shutter effects on
        the 2D points at the content distance rs_distance.
    :type rs_distance: None or float

    :returns: A JSON format string, with the UV Track data in it.
    :rtype: str
    """
    return _generate_v2_v3_and_v4(
        point_group,
        camera,
        points,
        version=UV_TRACK_FORMAT_VERSION_2,
        start_frame=start_frame,
        undistort=undistort,
        rs_distance=rs_distance
    )


def _generate_v3(point_group, camera, points,
                 start_frame=None,
                 rs_distance=None):
    """
    Generate the UV file format contents, using JSON format.

    Each point will store:
    - Point name
    - X, Y position (in UV coordinates, per-frame) as distorted and
      undistorted
    - Point weight (per-frame)
    - Point Set name
    - Point 'Persistent ID'
    - 3D Point X, Y and Z
    - 3D Point X, Y and Z locked status.

    :param point_group: The 3DE Point Group containing 'points'
    :type point_group: str

    :param camera: The 3DE Camera containing 2D 'points' data.
    :type camera: str

    :param points: The list of 3DE Points representing 2D data to
                   save.
    :type points: list of str

    :param start_frame: The frame number to be considered at
                        'first frame'. Defaults to 1001 if set to None.
    :type start_frame: None or int

    :param rs_distance: If not None, correct rolling shutter effects on
        the 2D points at the content distance rs_distance.
    :type rs_distance: None or float

    :returns: A JSON format string, with the UV Track data in it.
    :rtype: str
    """
    return _generate_v2_v3_and_v4(
        point_group,
        camera,
        points,
        version=UV_TRACK_FORMAT_VERSION_3,
        start_frame=start_frame,
        rs_distance=rs_distance
    )


def _generate_v4(point_group, camera, points,
                 start_frame=None,
                 rs_distance=None):
    """
    Generate the UV file format contents, using JSON format.

    Each point will store:
    - Point name
    - X, Y position (in UV coordinates, per-frame) as distorted and
      undistorted
    - Point weight (per-frame)
    - Point Set name
    - Point 'Persistent ID'
    - 3D Point X, Y and Z
    - 3D Point X, Y and Z locked status.

    :param point_group: The 3DE Point Group containing 'points'
    :type point_group: str

    :param camera: The 3DE Camera containing 2D 'points' data.
    :type camera: str

    :param points: The list of 3DE Points representing 2D data to
                   save.
    :type points: list of str

    :param start_frame: The frame number to be considered at
                        'first frame'. Defaults to 1001 if set to None.
    :type start_frame: None or int

    :param rs_distance: If not None, correct rolling shutter effects on
        the 2D points at the content distance rs_distance.
    :type rs_distance: None or float

    :returns: A JSON format string, with the UV Track data in it.
    :rtype: str
    """
    return _generate_v2_v3_and_v4(
        point_group,
        camera,
        points,
        version=UV_TRACK_FORMAT_VERSION_4,
        start_frame=start_frame,
        rs_distance=rs_distance
    )
