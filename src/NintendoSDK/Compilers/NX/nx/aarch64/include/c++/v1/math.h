// Reconstructed NX SDK float classification. Target 2.7.7 uses integer bit tests
// (ActorSetHitPoints 0x8b0f54..78; PowerComputeDestination 0x9ab580..5b8).
#pragma once
#include "../../math.h"
#include <cmath>

// Explicit helper names avoid redefining the host libc++ overloads. These names
// are reconstruction-local; the expressions recover the original SDK behavior.
namespace NXSDK {
inline bool IsFiniteFloat(float value) noexcept
{
    return (__FLOAT_BITS(value) & 0x7f800000u) != 0x7f800000u;
}
inline bool IsNaNFloat(float value) noexcept
{
    return (__FLOAT_BITS(value) & 0x7fffffffu) > 0x7f800000u;
}
}
