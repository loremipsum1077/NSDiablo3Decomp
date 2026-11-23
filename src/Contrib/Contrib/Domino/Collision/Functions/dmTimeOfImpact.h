// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Collision/Functions/dmTimeOfImpact.h

#pragma once

#include "dmDistance.h"
#include "dmHomingFunction.h"
#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"

enum dmTOICode {
    dm_toiHit = 0,
    dm_toiMiss = 1,
    dm_toiOverlap = 2,
};

struct dmTOIInput {  // size 0x110
    dmGJKCache cache;  // 0x00
    dmGJKProxy proxyA;  // 0x10
    dmGJKProxy proxyB;  // 0x30
    dmSweep sweepA;  // 0x50
    dmSweep sweepB;  // 0xb0
};
static_assert(sizeof(dmTOIInput) == 0x110, "dmTOIInput");

struct dmTOIOutput {  // size 0x50
    dmGJKCache cache;  // 0x00
    dm_vec4 pointA;  // 0x10
    dm_vec4 pointB;  // 0x20
    dm_vec4 eta;  // 0x30
    dmTOICode exitCode;  // 0x40
    dm_int32 iterationCount;  // 0x44
    dm_int32 maxRootIterationCount;  // 0x48
};
static_assert(sizeof(dmTOIOutput) == 0x50, "dmTOIOutput");

struct dmShapeCastInput {  // size 0xb0
    dmGJKProxy proxyA;  // 0x00
    dmGJKProxy proxyB;  // 0x20
    dmTransform transformA;  // 0x40
    dmTransform transformB;  // 0x60
    dm_vec4 translation;  // 0x80
    dm_vec4 maxLambda;  // 0x90
    dm_vec4 tolerance;  // 0xa0
};
static_assert(sizeof(dmShapeCastInput) == 0xb0, "dmShapeCastInput");


// Functions defined in this file (bodies are not in the DWARF):
//   line    29  dmTOIInput::dmTOIInput()  [inlined into 2 functions]
//   line    51  dmShapeCastInput::dmShapeCastInput()  [inline]

void dmTimeOfImpact(dmTOIOutput * pOutput, const dmTOIInput * pInput);
