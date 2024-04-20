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
Test the camera matrix generation in the mmSolver project; it
should match Maya perfectly.
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import unittest

try:
    import maya.standalone

    maya.standalone.initialize()
except RuntimeError:
    pass
import maya.cmds


import test.test_solver.solverutils as solverUtils


# @unittest.skip
class TestProjectionMatrix(solverUtils.SolverTestCase):
    def run_camera_matrix(
        self,
        filmback_width=1.0,
        filmback_height=1.0,
        filmback_offset_x=0.0,
        filmback_offset_y=0.0,
        focal_length=50.0,
        film_fit=0,
        render_width=1920,
        render_height=1080,
    ):
        cam_tfm, cam_shp = self.create_camera('cam')
        maya.cmds.setAttr(cam_tfm + '.horizontalFilmAperture', filmback_width)
        maya.cmds.setAttr(cam_tfm + '.verticalFilmAperture', filmback_height)
        maya.cmds.setAttr(cam_tfm + '.horizontalFilmOffset', filmback_offset_x)
        maya.cmds.setAttr(cam_tfm + '.verticalFilmOffset', filmback_offset_y)
        maya.cmds.setAttr(cam_tfm + '.focalLength', focal_length)
        maya.cmds.setAttr(cam_tfm + '.nearClipPlane', 0.1)

        # film fit; 0=fill, 1=horizontal, 2=vertical, 3=overscan
        maya.cmds.setAttr(cam_tfm + '.filmFit', film_fit)

        # Render settings resolution
        res = 'defaultResolution'
        imageAspectRatio = float(render_width) / float(render_height)
        maya.cmds.setAttr(res + '.width', render_width)
        maya.cmds.setAttr(res + '.height', render_height)
        maya.cmds.setAttr(res + '.deviceAspectRatio', imageAspectRatio)

        # Run test command
        result = maya.cmds.mmTestCameraMatrix(cam_tfm, cam_shp)
        self.assertTrue(result)

    def launch_film_fit(self, film_fit):
        line = '=' * 80

        print(line)
        print('Normal usage, square filmback, no offsets')
        self.run_camera_matrix(
            filmback_width=1.0,
            filmback_height=1.0,
            filmback_offset_x=0.0,
            filmback_offset_y=0.0,
            focal_length=50.0,
            film_fit=film_fit,
            render_width=1920,
            render_height=1080,
        )

        print(line)
        print('Square filmback with offset values')
        self.run_camera_matrix(
            filmback_width=1.0,
            filmback_height=1.0,
            filmback_offset_x=0.5,
            filmback_offset_y=0.5,
            focal_length=50.0,
            film_fit=film_fit,
            render_width=1920,
            render_height=1080,
        )

        print(line)
        print("'Landscape' sized filmback (width longer than height)")
        self.run_camera_matrix(
            filmback_width=2.0,
            filmback_height=1.0,
            filmback_offset_x=0.0,
            filmback_offset_y=0.0,
            focal_length=50.0,
            film_fit=film_fit,
            render_width=1920,
            render_height=1080,
        )

        print(line)
        print("'Landscape' sized filmback (width longer than height), with offsets")
        self.run_camera_matrix(
            filmback_width=2.0,
            filmback_height=1.0,
            filmback_offset_x=1.0,
            filmback_offset_y=1.0,
            focal_length=50.0,
            film_fit=film_fit,
            render_width=1920,
            render_height=1080,
        )

        print(line)
        print("'Portrait' sized filmback (height longer than width)")
        self.run_camera_matrix(
            filmback_width=1.0,
            filmback_height=2.0,
            filmback_offset_x=0.0,
            filmback_offset_y=0.0,
            focal_length=50.0,
            film_fit=film_fit,
            render_width=1920,
            render_height=1080,
        )

        print(line)
        print("'Portrait' sized filmback (height longer than width), with offsets")
        self.run_camera_matrix(
            filmback_width=1.0,
            filmback_height=2.0,
            filmback_offset_x=1.0,
            filmback_offset_y=1.0,
            focal_length=50.0,
            film_fit=film_fit,
            render_width=1920,
            render_height=1080,
        )

        print(line)
        print('Non-standard render resolution')
        self.run_camera_matrix(
            filmback_width=1.0,
            filmback_height=1.0,
            filmback_offset_x=0.0,
            filmback_offset_y=0.0,
            focal_length=50.0,
            film_fit=film_fit,
            render_width=1080,
            render_height=1920,
        )

        print(line)
        print('Non-standard render resolution, with offsets')
        self.run_camera_matrix(
            filmback_width=1.0,
            filmback_height=1.0,
            filmback_offset_x=1.0,
            filmback_offset_y=1.0,
            focal_length=50.0,
            film_fit=film_fit,
            render_width=1080,
            render_height=1920,
        )

    def test_film_fit(self):
        # Test all film fit values;
        line = '-' * 100

        print('test film fit: horizontal')
        self.launch_film_fit(1)  # 1=horizontal
        print(line)

        print('test film fit: vertical')
        self.launch_film_fit(2)  # 2=vertical
        print(line)

        print('test film fit: fill')
        self.launch_film_fit(0)  # 0=fill
        print(line)

        print('test film fit: overscan')
        self.launch_film_fit(3)  # 3=overscan
        print(line)
        return


if __name__ == '__main__':
    prog = unittest.main()
