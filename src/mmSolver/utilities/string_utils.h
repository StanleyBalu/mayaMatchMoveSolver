/*
 * Copyright (C) 2018, 2019 David Cattermole.
 *
 * This file is part of mmSolver.
 *
 * mmSolver is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * mmSolver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with mmSolver.  If not, see <https://www.gnu.org/licenses/>.
 * ====================================================================
 *
 * Generic string helper functions.
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

// STL
#include <cmath>    // fabs
#include <sstream>  // stringstream
#include <string>

namespace mmstring {

/*! Convert a number to a string
 *
 * @tparam NUM_TYPE
 * @param num
 * @return
 */
template <typename NUM_TYPE>
std::string numberToString(NUM_TYPE num) {
    std::stringstream ss;
    ss << num;
    return ss.str();
}

/*! Convert a string to a number.
 *
 * @tparam NUM_TYPE
 * @param text
 * @return
 */
template <typename NUM_TYPE>
NUM_TYPE stringToNumber(const std::string &text) {
    std::stringstream ss(text);
    NUM_TYPE result;
    ss >> result;
    if (!result) {
        result = 0;
    }
    return result;
}

}  // namespace mmstring

#endif  // STRING_UTILS_H
