#pragma once

#include "SigmaTypes.h"

struct RNG {
    uint32 dwSeed;
    uint32 dwCarry;
};

// X/Sigma/Random.h:24. Conversion of the 32-bit draw to [0, 1) (original bytes 0x2F800000).
static const float flInvDwordMax = 2.3283064e-10f;

void RandInit(RNG& rng);
void RandSetSeed_Internal(RNG& rng, const uint32 dwNewSeed);
uint32 RandGetSeed(RNG& rng);
uint32 RandGetDWord(RNG& rng);

// X/Sigma/Random.h:140, :159 and :184. Bodies are not in the DWARF; written from callers
// (TriggerConditions::GetDelay). The range argument is the inclusive maximum.
inline uint32 RandFast(RNG& rng, const int32 x) {
    return RandGetDWord(rng) & x;
}

// Line 166 tests for a power of two, :168 is the modulo and :172 the RandFast call
// (MonsterGetRandomSummonActor), so the general case comes first.
inline uint32 Rand_Internal(RNG& rng, const uint32 nDelta) {
    int32 nMax = nDelta + 1;
    if ((nMax & nDelta) != 0)
        return RandGetDWord(rng) % (uint32)nMax;
    return RandFast(rng, nDelta);
}

inline uint32 RandRange_Internal(RNG& rng, const int32 min, const int32 delta) {
    return min + Rand_Internal(rng, delta);
}

// X/Sigma/Random.h:200. Body not in the DWARF; written from GenericFloatFuzz: a value in [0, 1).
inline float RandFloat_Internal(RNG& rng) {
    return (float)RandGetDWord(rng) * (1.0f / 4294967296.0f);
}

// X/Sigma/Random.h:230. Body not in the DWARF; reconstructed from its three inlined copies
// (RandomPoint.cpp:25, :62 and :255). The non-zero `flDelta` test is the `fcmp`/`b.eq`
// guard emitted in GetRandomRadius (`b.eq` skips the draw straight to the caller's `fsqrt`,
// F1:236 / F1:242); the multiply is `(rand * flInvDwordMax) * flDelta + flMin` as emitted at
// RandomPoint.cpp:25 (`fadd s0, s0, s0` for flDelta == 2) and RandomPoint.cpp:62.
inline float RandRangeFloat_Internal(RNG& rng, const float flMin, const float flDelta) {
    if (flDelta != 0.0f)
        return flMin + flDelta * ((float)RandGetDWord(rng) * flInvDwordMax);
    return flMin;
}

// X/Sigma/Random.h:252. `RandAngle` draws in [0, full turn). The literal below is invented (the
// DWARF records no full-turn variable here); it is the original at `s_intTypeLookup+0x190`
// (bytes d90fc940), which reads 6.28318 and is two ULP below MathGeneral.h's FULL_CIRCLE
// (0x40C90FDB, `s_intTypeLookup+0xa4`) that NormalizeAngle uses.
inline float RandAngle(RNG& rng) {
    return RandRangeFloat_Internal(rng, 0.0f, 6.2831845f);
}

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(RNG) == 0x8, "RNG");
static_assert(offsetof(RNG, dwSeed) == 0x0, "RNG");
static_assert(offsetof(RNG, dwCarry) == 0x4, "RNG");
















































































// X/Sigma/Random.h:147. Body from the abstract DIE: RandFast is inlined at line 152 with x = delta (BrainComponentUpdate, BrainComponents.cpp:977).
inline uint32 RandRangeFast(
    RNG & rng,
    const int32 min,
    const int32 delta)
{
    return min + RandFast(rng, delta);
}
