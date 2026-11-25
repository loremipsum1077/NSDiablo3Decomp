// Original SDK libc++ math.h float instantiations, evidenced by DWARF in
// _wrap_ActorSetHitPoints. Include after game headers, which may define isfinite.
#pragma once
#include "../../math.h"
#include <cmath>
#undef isfinite
#undef isnan
// clang 7's libc++ (2.7.7 build) already has float overloads of isnan/isfinite that
// call __libcpp_isnan/__libcpp_isfinite on the float itself; specializing them after
// that instantiation is an error, so these specializations are for libc++ 5 only.
#if _LIBCPP_VERSION < 7000
// The build container already defines these global primary templates through
// <cmath>, but promotes float to double. Explicit float specializations avoid
// redefining those templates while preserving the original names and bodies.
// Other arithmetic overloads retain the container's standard implementation.
// Original line 427; bit classification from line 429.
template <> inline bool __libcpp_isfinite<float>(float __lcpp_x) noexcept
{
    return (__FLOAT_BITS(__lcpp_x) & 0x7f800000u) != 0x7f800000u;
}
// Original line 495; bit classification from line 497.
template <> inline bool __libcpp_isnan<float>(float __lcpp_x) noexcept
{
    return (__FLOAT_BITS(__lcpp_x) & 0x7fffffffu) > 0x7f800000u;
}
// Original line 439; call at line 441.
template <> inline bool isfinite<float>(float __lcpp_x) noexcept
{
    return __libcpp_isfinite(__lcpp_x);
}
// Original line 505; call at line 507.
template <> inline bool isnan<float>(float __lcpp_x) noexcept
{
    return __libcpp_isnan(__lcpp_x);
}
#endif
