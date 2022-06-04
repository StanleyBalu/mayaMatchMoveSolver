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
 * Debugging Utils - printing and benchmarking.
 */

#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

// STL
#include <cstdint>   // uint32_t, uint64_t
#include <iomanip>   // setfill, setw
#include <iostream>  // cout, cerr, endl
#include <string>    // string

// Maya
#include <maya/MStreamUtils.h>
#include <maya/MTypes.h>

#ifdef _WIN32
#include <Windows.h>  // GetSystemTime
#include <intrin.h>
#ifdef max
// On Windows max is defined as a macro, but this
// conflicts with the C++ standard, so we undef it after
// including it in 'Windows.h'.
#undef max
#endif
#else
// Linux Specific Functions
#include <sys/time.h>  // gettimeofday
#endif

// Debug defines...
//#ifndef NDEBUG
//#  define MMSOLVER_DBG(x)
//#else
#ifdef _WIN32  // Windows MSVC
#define MMSOLVER_DBG(x)                                              \
    __pragma(warning(push)) __pragma(warning(disable : 4127)) do {   \
        MStreamUtils::stdErrorStream()                               \
            << __FILE__ << ':' << __LINE__ << ' ' << x << std::endl; \
    }                                                                \
    while (0)                                                        \
        ;                                                            \
    __pragma(warning(pop))
#else  // Linux and MacOS
#define MMSOLVER_DBG(x)                                              \
    do {                                                             \
        MStreamUtils::stdErrorStream()                               \
            << __FILE__ << ':' << __LINE__ << ' ' << x << std::endl; \
    } while (0)
#endif
//#endif // NDEBUG

#ifdef _WIN32  // Windows MSVC
#define MMSOLVER_VRB(x)                                            \
    __pragma(warning(push)) __pragma(warning(disable : 4127)) do { \
        if (verbose) {                                             \
            MStreamUtils::stdErrorStream() << x << std::endl;      \
        }                                                          \
    }                                                              \
    while (0)                                                      \
        ;                                                          \
    __pragma(warning(pop))
#else  // Linux and MacOS
#define MMSOLVER_VRB(x)                                       \
    do {                                                      \
        if (verbose) {                                        \
            MStreamUtils::stdErrorStream() << x << std::endl; \
        }                                                     \
    } while (0)
#endif

#ifdef _WIN32  // Windows MSVC
#define MMSOLVER_ERR(x)                                                \
    __pragma(warning(push)) __pragma(warning(disable : 4127)) do {     \
        MStreamUtils::stdErrorStream() << "ERROR: " << x << std::endl; \
    }                                                                  \
    while (0)                                                          \
        ;                                                              \
    __pragma(warning(pop))
#else  // Linux and MacOS
#define MMSOLVER_ERR(x)                                                \
    do {                                                               \
        MStreamUtils::stdErrorStream() << "ERROR: " << x << std::endl; \
    } while (0)
#endif

#ifdef _WIN32  // Windows MSVC
#define MMSOLVER_WRN(x)                                                  \
    __pragma(warning(push)) __pragma(warning(disable : 4127)) do {       \
        MStreamUtils::stdErrorStream() << "WARNING: " << x << std::endl; \
    }                                                                    \
    while (0)                                                            \
        ;                                                                \
    __pragma(warning(pop))
#else  // Linux and MacOS
#define MMSOLVER_WRN(x)                                                  \
    do {                                                                 \
        MStreamUtils::stdErrorStream() << "WARNING: " << x << std::endl; \
    } while (0)
#endif

#ifdef _WIN32  // Windows MSVC
#define MMSOLVER_INFO(x)                                           \
    __pragma(warning(push)) __pragma(warning(disable : 4127)) do { \
        MStreamUtils::stdErrorStream() << x << std::endl;          \
    }                                                              \
    while (0)                                                      \
        ;                                                          \
    __pragma(warning(pop))
#else  // Linux and MacOS
#define MMSOLVER_INFO(x)                                  \
    do {                                                  \
        MStreamUtils::stdErrorStream() << x << std::endl; \
    } while (0)
#endif

// Used to indicate to the user that a variable is not used, and
// avoids the compilier from printing warnings/errors about unused
// variables.
//
// https://stackoverflow.com/questions/308277/what-are-the-consequences-of-ignoring-warning-unused-parameter/308286#308286
#ifdef _WIN32  // Windows MSVC

#define UNUSED(expr)                                               \
    __pragma(warning(push)) __pragma(warning(disable : 4127)) do { \
        (void)(expr);                                              \
    }                                                              \
    while (0)                                                      \
        ;                                                          \
    __pragma(warning(pop))
#else  // Linux and MacOS
#define UNUSED(expr)  \
    do {              \
        (void)(expr); \
    } while (0)
#endif

namespace debug {

using Ticks = unsigned long long;

// Get time of day with high accuracy, on both Windows and Linux.
//
// http://stackoverflow.com/questions/1861294/how-to-calculate-execution-time-of-a-code-snippet-in-cw
using Timestamp = unsigned long long;

// Measuring CPU Clock-Cycles on Windows or Linux.
// http://stackoverflow.com/questions/13772567/get-cpu-cycle-count
#ifdef _WIN32
DWORD64 rdtsc();
#else
uint64_t rdtsc();
#endif

// Get the current time - used for profiling and debug.
Timestamp get_timestamp();

// CPU Clock-cycle timing.
//
// Article1:
// http://lemire.me/blog/2012/06/20/do-not-waste-time-with-stl-vectors/
// Article2: http://stackoverflow.com/questions/13772567/get-cpu-cycle-count
// Code:
// https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/blob/master/2012/06/20/testvector.cpp
//
//
// Example Code Start:
//   CPUBenchmark time;
//   const size_t N = 100 * 1000 * 1000 ;
//   time.start();
//   std::cout.precision(3);
//   std::cout << " report speed in CPU cycles per integer" << std::endl;
//   std::cout << std::endl << "ignore this:" << runtestnice(N) << std::endl;
//   std::cout << "with push_back:"<<(time.stop()*1.0/N)<<std::endl;
// Example Code End:
//

// Wrapper struct around assembly clock cycle timer.
struct CPUBenchmark {
public:
    CPUBenchmark() : ticktime(0), ticktimeTotal(0) { start(); }

    Ticks ticktime;
    Ticks ticktimeTotal;

    void start();
    Ticks stop();
    Ticks get_ticks(uint32_t loopNums = 0);
    void print(std::string heading, uint32_t loopNums = 0);
};

// Wrapper struct around 'get_timestamp' timer.
struct TimestampBenchmark {
public:
    TimestampBenchmark() : timestamp(0), timestampTotal(0) { start(); }

    Timestamp timestamp;
    Timestamp timestampTotal;

    void start();
    Timestamp stop();

    double get_seconds(uint32_t loopNums = 0);
    void print(std::string heading, uint32_t loopNums = 0);
    void printInSec(std::string heading, uint32_t loopNums = 0);
};

}  // namespace debug

#endif  // DEBUG_UTILS_H
