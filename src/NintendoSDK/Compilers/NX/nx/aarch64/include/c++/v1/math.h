// NX SDK libc++ float classification. The SDK (clang-for-nx 1.4.9, musl C library)
// classifies a float by its bits: DWARF of 2.6.2's _wrap_ActorSetHitPoints names
// __libcpp_isfinite<float>/__libcpp_isnan<float> (lines 427/429 and 495/497), and 2.7.7
// still uses the integer masks (ActorSetHitPoints 0x8b0f54..78, PowerComputeDestination
// 0x9ab580..5b8). The build container's libc++ 7 defines isnan(float) itself with a
// floating-point builtin, and that definition can neither be replaced nor specialized.
//
// So, as musl's own <math.h> does, isfinite/isnan are macros again after <cmath>: the
// game's isfinite(x)/isnan(x) calls keep their spelling, float arguments get the SDK's
// bit tests and other types the standard functions. Include after game headers, which
// may define isfinite, and only in files that do not spell std::isfinite/std::isnan.
#pragma once
#include "../../math.h"
#include <cmath>
#undef isfinite
#undef isnan

// Reconstruction-local namespace; the bodies are the SDK's.
namespace nx_sdk_libcpp {
// Original line 427; bit classification from line 429.
inline bool __libcpp_isfinite(float __lcpp_x) noexcept
{
    return (__FLOAT_BITS(__lcpp_x) & 0x7f800000u) != 0x7f800000u;
}
// Original line 495; bit classification from line 497.
inline bool __libcpp_isnan(float __lcpp_x) noexcept
{
    return (__FLOAT_BITS(__lcpp_x) & 0x7fffffffu) > 0x7f800000u;
}
template <class _A1> inline bool __libcpp_isfinite(_A1 __lcpp_x) noexcept { return std::isfinite(__lcpp_x); }
template <class _A1> inline bool __libcpp_isnan(_A1 __lcpp_x) noexcept { return std::isnan(__lcpp_x); }
}

#define isfinite(x) (::nx_sdk_libcpp::__libcpp_isfinite(x))
#define isnan(x) (::nx_sdk_libcpp::__libcpp_isnan(x))
