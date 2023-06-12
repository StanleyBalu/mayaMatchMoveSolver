#include <array>
#include <cstdint>
#include <type_traits>

#include "mmlens/_cxx.h"
#include "mmlens/_symbol_export.h"

namespace mmlens {
enum class DistortionDirection : ::std::uint8_t;
enum class LensModelState : ::std::uint8_t;
enum class LensModelType : ::std::uint8_t;
struct CameraParameters;
struct LensParameters;
struct Parameters3deClassic;
struct Parameters3deRadialStdDeg4;
struct Parameters3deAnamorphicStdDeg4;
struct Parameters3deAnamorphicStdDeg4Rescaled;
}  // namespace mmlens

namespace mmlens {
#ifndef CXXBRIDGE1_ENUM_mmlens$DistortionDirection
#define CXXBRIDGE1_ENUM_mmlens$DistortionDirection
enum class DistortionDirection : ::std::uint8_t {
    kUndistort = 0,
    kRedistort = 1,
    kUndistortAndRedistort = 2,
    kRedistortAndUndistort = 3,
    kNumDistortionDirection = 4,
};
#endif  // CXXBRIDGE1_ENUM_mmlens$DistortionDirection

#ifndef CXXBRIDGE1_ENUM_mmlens$LensModelState
#define CXXBRIDGE1_ENUM_mmlens$LensModelState
enum class LensModelState : ::std::uint8_t {
    kUninitialized = 0,
    kClean = 1,
    kDirty = 2,
    kNumLensModelState = 3,
};
#endif  // CXXBRIDGE1_ENUM_mmlens$LensModelState

#ifndef CXXBRIDGE1_ENUM_mmlens$LensModelType
#define CXXBRIDGE1_ENUM_mmlens$LensModelType
enum class LensModelType : ::std::uint8_t {
    kUninitialized = 0,
    kPassthrough = 1,
    k3deClassic = 2,
    k3deRadialStdDeg4 = 3,
    k3deAnamorphicStdDeg4 = 4,
    k3deAnamorphicStdDeg4Rescaled = 5,
    kNumLensModelType = 6,
};
#endif  // CXXBRIDGE1_ENUM_mmlens$LensModelType

#ifndef CXXBRIDGE1_STRUCT_mmlens$CameraParameters
#define CXXBRIDGE1_STRUCT_mmlens$CameraParameters
struct CameraParameters final {
    double focal_length_cm;
    double film_back_width_cm;
    double film_back_height_cm;
    double pixel_aspect;
    double lens_center_offset_x_cm;
    double lens_center_offset_y_cm;

    bool operator==(const CameraParameters &) const noexcept;
    bool operator!=(const CameraParameters &) const noexcept;
    bool operator<(const CameraParameters &) const noexcept;
    bool operator<=(const CameraParameters &) const noexcept;
    bool operator>(const CameraParameters &) const noexcept;
    bool operator>=(const CameraParameters &) const noexcept;
    using IsRelocatable = ::std::true_type;
};
#endif  // CXXBRIDGE1_STRUCT_mmlens$CameraParameters

#ifndef CXXBRIDGE1_STRUCT_mmlens$LensParameters
#define CXXBRIDGE1_STRUCT_mmlens$LensParameters
struct LensParameters final {
    ::mmlens::LensModelType mode;
    ::std::array<double, 5> values;

    using IsRelocatable = ::std::true_type;
};
#endif  // CXXBRIDGE1_STRUCT_mmlens$LensParameters

#ifndef CXXBRIDGE1_STRUCT_mmlens$Parameters3deClassic
#define CXXBRIDGE1_STRUCT_mmlens$Parameters3deClassic
struct Parameters3deClassic final {
    double distortion;
    double anamorphic_squeeze;
    double curvature_x;
    double curvature_y;
    double quartic_distortion;

    bool operator==(const Parameters3deClassic &) const noexcept;
    bool operator!=(const Parameters3deClassic &) const noexcept;
    bool operator<(const Parameters3deClassic &) const noexcept;
    bool operator<=(const Parameters3deClassic &) const noexcept;
    bool operator>(const Parameters3deClassic &) const noexcept;
    bool operator>=(const Parameters3deClassic &) const noexcept;
    using IsRelocatable = ::std::true_type;
};
#endif  // CXXBRIDGE1_STRUCT_mmlens$Parameters3deClassic

#ifndef CXXBRIDGE1_STRUCT_mmlens$Parameters3deRadialStdDeg4
#define CXXBRIDGE1_STRUCT_mmlens$Parameters3deRadialStdDeg4
struct Parameters3deRadialStdDeg4 final {
    double degree2_distortion;
    double degree2_u;
    double degree2_v;
    double degree4_distortion;
    double degree4_u;
    double degree4_v;
    double cylindric_direction;
    double cylindric_bending;

    bool operator==(const Parameters3deRadialStdDeg4 &) const noexcept;
    bool operator!=(const Parameters3deRadialStdDeg4 &) const noexcept;
    bool operator<(const Parameters3deRadialStdDeg4 &) const noexcept;
    bool operator<=(const Parameters3deRadialStdDeg4 &) const noexcept;
    bool operator>(const Parameters3deRadialStdDeg4 &) const noexcept;
    bool operator>=(const Parameters3deRadialStdDeg4 &) const noexcept;
    using IsRelocatable = ::std::true_type;
};
#endif  // CXXBRIDGE1_STRUCT_mmlens$Parameters3deRadialStdDeg4

#ifndef CXXBRIDGE1_STRUCT_mmlens$Parameters3deAnamorphicStdDeg4
#define CXXBRIDGE1_STRUCT_mmlens$Parameters3deAnamorphicStdDeg4
struct Parameters3deAnamorphicStdDeg4 final {
    double degree2_cx02;
    double degree2_cy02;
    double degree2_cx22;
    double degree2_cy22;
    double degree4_cx04;
    double degree4_cy04;
    double degree4_cx24;
    double degree4_cy24;
    double degree4_cx44;
    double degree4_cy44;
    double lens_rotation;
    double squeeze_x;
    double squeeze_y;

    bool operator==(const Parameters3deAnamorphicStdDeg4 &) const noexcept;
    bool operator!=(const Parameters3deAnamorphicStdDeg4 &) const noexcept;
    bool operator<(const Parameters3deAnamorphicStdDeg4 &) const noexcept;
    bool operator<=(const Parameters3deAnamorphicStdDeg4 &) const noexcept;
    bool operator>(const Parameters3deAnamorphicStdDeg4 &) const noexcept;
    bool operator>=(const Parameters3deAnamorphicStdDeg4 &) const noexcept;
    using IsRelocatable = ::std::true_type;
};
#endif  // CXXBRIDGE1_STRUCT_mmlens$Parameters3deAnamorphicStdDeg4

#ifndef CXXBRIDGE1_STRUCT_mmlens$Parameters3deAnamorphicStdDeg4Rescaled
#define CXXBRIDGE1_STRUCT_mmlens$Parameters3deAnamorphicStdDeg4Rescaled
struct Parameters3deAnamorphicStdDeg4Rescaled final {
    double degree2_cx02;
    double degree2_cy02;
    double degree2_cx22;
    double degree2_cy22;
    double degree4_cx04;
    double degree4_cy04;
    double degree4_cx24;
    double degree4_cy24;
    double degree4_cx44;
    double degree4_cy44;
    double lens_rotation;
    double squeeze_x;
    double squeeze_y;
    double rescale;

    bool operator==(
        const Parameters3deAnamorphicStdDeg4Rescaled &) const noexcept;
    bool operator!=(
        const Parameters3deAnamorphicStdDeg4Rescaled &) const noexcept;
    bool operator<(
        const Parameters3deAnamorphicStdDeg4Rescaled &) const noexcept;
    bool operator<=(
        const Parameters3deAnamorphicStdDeg4Rescaled &) const noexcept;
    bool operator>(
        const Parameters3deAnamorphicStdDeg4Rescaled &) const noexcept;
    bool operator>=(
        const Parameters3deAnamorphicStdDeg4Rescaled &) const noexcept;
    using IsRelocatable = ::std::true_type;
};
#endif  // CXXBRIDGE1_STRUCT_mmlens$Parameters3deAnamorphicStdDeg4Rescaled

extern "C" {
bool mmlens$cxxbridge1$CameraParameters$operator$eq(
    const CameraParameters &, const CameraParameters &) noexcept;
bool mmlens$cxxbridge1$CameraParameters$operator$ne(
    const CameraParameters &, const CameraParameters &) noexcept;
bool mmlens$cxxbridge1$CameraParameters$operator$lt(
    const CameraParameters &, const CameraParameters &) noexcept;
bool mmlens$cxxbridge1$CameraParameters$operator$le(
    const CameraParameters &, const CameraParameters &) noexcept;
bool mmlens$cxxbridge1$CameraParameters$operator$gt(
    const CameraParameters &, const CameraParameters &) noexcept;
bool mmlens$cxxbridge1$CameraParameters$operator$ge(
    const CameraParameters &, const CameraParameters &) noexcept;
bool mmlens$cxxbridge1$Parameters3deClassic$operator$eq(
    const Parameters3deClassic &, const Parameters3deClassic &) noexcept;
bool mmlens$cxxbridge1$Parameters3deClassic$operator$ne(
    const Parameters3deClassic &, const Parameters3deClassic &) noexcept;
bool mmlens$cxxbridge1$Parameters3deClassic$operator$lt(
    const Parameters3deClassic &, const Parameters3deClassic &) noexcept;
bool mmlens$cxxbridge1$Parameters3deClassic$operator$le(
    const Parameters3deClassic &, const Parameters3deClassic &) noexcept;
bool mmlens$cxxbridge1$Parameters3deClassic$operator$gt(
    const Parameters3deClassic &, const Parameters3deClassic &) noexcept;
bool mmlens$cxxbridge1$Parameters3deClassic$operator$ge(
    const Parameters3deClassic &, const Parameters3deClassic &) noexcept;
bool mmlens$cxxbridge1$Parameters3deRadialStdDeg4$operator$eq(
    const Parameters3deRadialStdDeg4 &,
    const Parameters3deRadialStdDeg4 &) noexcept;
bool mmlens$cxxbridge1$Parameters3deRadialStdDeg4$operator$ne(
    const Parameters3deRadialStdDeg4 &,
    const Parameters3deRadialStdDeg4 &) noexcept;
bool mmlens$cxxbridge1$Parameters3deRadialStdDeg4$operator$lt(
    const Parameters3deRadialStdDeg4 &,
    const Parameters3deRadialStdDeg4 &) noexcept;
bool mmlens$cxxbridge1$Parameters3deRadialStdDeg4$operator$le(
    const Parameters3deRadialStdDeg4 &,
    const Parameters3deRadialStdDeg4 &) noexcept;
bool mmlens$cxxbridge1$Parameters3deRadialStdDeg4$operator$gt(
    const Parameters3deRadialStdDeg4 &,
    const Parameters3deRadialStdDeg4 &) noexcept;
bool mmlens$cxxbridge1$Parameters3deRadialStdDeg4$operator$ge(
    const Parameters3deRadialStdDeg4 &,
    const Parameters3deRadialStdDeg4 &) noexcept;
bool mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4$operator$eq(
    const Parameters3deAnamorphicStdDeg4 &,
    const Parameters3deAnamorphicStdDeg4 &) noexcept;
bool mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4$operator$ne(
    const Parameters3deAnamorphicStdDeg4 &,
    const Parameters3deAnamorphicStdDeg4 &) noexcept;
bool mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4$operator$lt(
    const Parameters3deAnamorphicStdDeg4 &,
    const Parameters3deAnamorphicStdDeg4 &) noexcept;
bool mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4$operator$le(
    const Parameters3deAnamorphicStdDeg4 &,
    const Parameters3deAnamorphicStdDeg4 &) noexcept;
bool mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4$operator$gt(
    const Parameters3deAnamorphicStdDeg4 &,
    const Parameters3deAnamorphicStdDeg4 &) noexcept;
bool mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4$operator$ge(
    const Parameters3deAnamorphicStdDeg4 &,
    const Parameters3deAnamorphicStdDeg4 &) noexcept;
bool mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4Rescaled$operator$eq(
    const Parameters3deAnamorphicStdDeg4Rescaled &,
    const Parameters3deAnamorphicStdDeg4Rescaled &) noexcept;
bool mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4Rescaled$operator$ne(
    const Parameters3deAnamorphicStdDeg4Rescaled &,
    const Parameters3deAnamorphicStdDeg4Rescaled &) noexcept;
bool mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4Rescaled$operator$lt(
    const Parameters3deAnamorphicStdDeg4Rescaled &,
    const Parameters3deAnamorphicStdDeg4Rescaled &) noexcept;
bool mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4Rescaled$operator$le(
    const Parameters3deAnamorphicStdDeg4Rescaled &,
    const Parameters3deAnamorphicStdDeg4Rescaled &) noexcept;
bool mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4Rescaled$operator$gt(
    const Parameters3deAnamorphicStdDeg4Rescaled &,
    const Parameters3deAnamorphicStdDeg4Rescaled &) noexcept;
bool mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4Rescaled$operator$ge(
    const Parameters3deAnamorphicStdDeg4Rescaled &,
    const Parameters3deAnamorphicStdDeg4Rescaled &) noexcept;
}  // extern "C"

bool CameraParameters::operator==(const CameraParameters &rhs) const noexcept {
    return mmlens$cxxbridge1$CameraParameters$operator$eq(*this, rhs);
}

bool CameraParameters::operator!=(const CameraParameters &rhs) const noexcept {
    return mmlens$cxxbridge1$CameraParameters$operator$ne(*this, rhs);
}

bool CameraParameters::operator<(const CameraParameters &rhs) const noexcept {
    return mmlens$cxxbridge1$CameraParameters$operator$lt(*this, rhs);
}

bool CameraParameters::operator<=(const CameraParameters &rhs) const noexcept {
    return mmlens$cxxbridge1$CameraParameters$operator$le(*this, rhs);
}

bool CameraParameters::operator>(const CameraParameters &rhs) const noexcept {
    return mmlens$cxxbridge1$CameraParameters$operator$gt(*this, rhs);
}

bool CameraParameters::operator>=(const CameraParameters &rhs) const noexcept {
    return mmlens$cxxbridge1$CameraParameters$operator$ge(*this, rhs);
}

bool Parameters3deClassic::operator==(
    const Parameters3deClassic &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deClassic$operator$eq(*this, rhs);
}

bool Parameters3deClassic::operator!=(
    const Parameters3deClassic &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deClassic$operator$ne(*this, rhs);
}

bool Parameters3deClassic::operator<(
    const Parameters3deClassic &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deClassic$operator$lt(*this, rhs);
}

bool Parameters3deClassic::operator<=(
    const Parameters3deClassic &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deClassic$operator$le(*this, rhs);
}

bool Parameters3deClassic::operator>(
    const Parameters3deClassic &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deClassic$operator$gt(*this, rhs);
}

bool Parameters3deClassic::operator>=(
    const Parameters3deClassic &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deClassic$operator$ge(*this, rhs);
}

bool Parameters3deRadialStdDeg4::operator==(
    const Parameters3deRadialStdDeg4 &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deRadialStdDeg4$operator$eq(*this, rhs);
}

bool Parameters3deRadialStdDeg4::operator!=(
    const Parameters3deRadialStdDeg4 &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deRadialStdDeg4$operator$ne(*this, rhs);
}

bool Parameters3deRadialStdDeg4::operator<(
    const Parameters3deRadialStdDeg4 &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deRadialStdDeg4$operator$lt(*this, rhs);
}

bool Parameters3deRadialStdDeg4::operator<=(
    const Parameters3deRadialStdDeg4 &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deRadialStdDeg4$operator$le(*this, rhs);
}

bool Parameters3deRadialStdDeg4::operator>(
    const Parameters3deRadialStdDeg4 &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deRadialStdDeg4$operator$gt(*this, rhs);
}

bool Parameters3deRadialStdDeg4::operator>=(
    const Parameters3deRadialStdDeg4 &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deRadialStdDeg4$operator$ge(*this, rhs);
}

bool Parameters3deAnamorphicStdDeg4::operator==(
    const Parameters3deAnamorphicStdDeg4 &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4$operator$eq(*this,
                                                                        rhs);
}

bool Parameters3deAnamorphicStdDeg4::operator!=(
    const Parameters3deAnamorphicStdDeg4 &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4$operator$ne(*this,
                                                                        rhs);
}

bool Parameters3deAnamorphicStdDeg4::operator<(
    const Parameters3deAnamorphicStdDeg4 &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4$operator$lt(*this,
                                                                        rhs);
}

bool Parameters3deAnamorphicStdDeg4::operator<=(
    const Parameters3deAnamorphicStdDeg4 &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4$operator$le(*this,
                                                                        rhs);
}

bool Parameters3deAnamorphicStdDeg4::operator>(
    const Parameters3deAnamorphicStdDeg4 &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4$operator$gt(*this,
                                                                        rhs);
}

bool Parameters3deAnamorphicStdDeg4::operator>=(
    const Parameters3deAnamorphicStdDeg4 &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4$operator$ge(*this,
                                                                        rhs);
}

bool Parameters3deAnamorphicStdDeg4Rescaled::operator==(
    const Parameters3deAnamorphicStdDeg4Rescaled &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4Rescaled$operator$eq(
        *this, rhs);
}

bool Parameters3deAnamorphicStdDeg4Rescaled::operator!=(
    const Parameters3deAnamorphicStdDeg4Rescaled &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4Rescaled$operator$ne(
        *this, rhs);
}

bool Parameters3deAnamorphicStdDeg4Rescaled::operator<(
    const Parameters3deAnamorphicStdDeg4Rescaled &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4Rescaled$operator$lt(
        *this, rhs);
}

bool Parameters3deAnamorphicStdDeg4Rescaled::operator<=(
    const Parameters3deAnamorphicStdDeg4Rescaled &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4Rescaled$operator$le(
        *this, rhs);
}

bool Parameters3deAnamorphicStdDeg4Rescaled::operator>(
    const Parameters3deAnamorphicStdDeg4Rescaled &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4Rescaled$operator$gt(
        *this, rhs);
}

bool Parameters3deAnamorphicStdDeg4Rescaled::operator>=(
    const Parameters3deAnamorphicStdDeg4Rescaled &rhs) const noexcept {
    return mmlens$cxxbridge1$Parameters3deAnamorphicStdDeg4Rescaled$operator$ge(
        *this, rhs);
}
}  // namespace mmlens
