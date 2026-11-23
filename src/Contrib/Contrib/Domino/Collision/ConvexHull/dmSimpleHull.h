// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Collision/ConvexHull/dmSimpleHull.h

#pragma once

#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"

struct dmSimpleHullInput {  // size 0x20
    const dm_vec4 * pPoints;  // 0x00
    dm_int32 count;  // 0x08
    dm_int32 maxCount;  // 0x0c
    dm_float32 weldTolerance;  // 0x10
    dm_float32 edgeTolerance;  // 0x14
    dm_float32 faceTolerance;  // 0x18
};
static_assert(sizeof(dmSimpleHullInput) == 0x20, "dmSimpleHullInput");


class dmHullBuilder;

bool dmCreateSimpleHull(dmHullBuilder * pHull, const dmSimpleHullInput * pInput);
