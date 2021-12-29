//
// Copyright (C) 2020, 2021 David Cattermole.
//
// This file is part of mmSolver.
//
// mmSolver is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// mmSolver is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with mmSolver.  If not, see <https://www.gnu.org/licenses/>.
// ====================================================================
//

use crate::constant::FrameValue;
use crate::constant::Real;

#[derive(Debug, Clone)]
pub struct AnimDenseAttr {
    // TODO: Add run-length-encoding (RLE) for the values since there
    // may be cases where most values in the vector will be the same
    // value - this is a memory usage optimization.
    //
    // However such an optimization may cause re-allocations when
    // setting values on the attribute, which could degrade
    // performance.
    pub values: Vec<Real>,
    pub frame_start: FrameValue,
}

impl AnimDenseAttr {
    pub fn new() -> Self {
        Self {
            values: Vec::<Real>::new(),
            frame_start: 0,
        }
    }

    pub fn get_value(&self, frame: FrameValue) -> Real {
        let f = (self.frame_start - frame) as usize;
        self.values[f]
    }

    pub fn set_value(&mut self, frame: FrameValue, value: Real) {
        let f = (self.frame_start - frame) as usize;
        self.values[f] = value;
    }

    pub fn get_values(&self) -> &Vec<Real> {
        &self.values
    }

    pub fn set_values(&mut self, values: Vec<Real>) {
        self.values = values;
    }
}
