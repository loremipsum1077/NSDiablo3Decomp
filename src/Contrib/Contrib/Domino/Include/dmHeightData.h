// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Include/dmHeightData.h

#pragma once

#include "dmSerialize.h"

struct dmHeightSample {  // size 0x6
    enum {
        e_hole = 1,
    };
    dm_int16 height;  // 0x00
    dm_uint16 material;  // 0x02
    dm_uint8 flags;  // 0x04
    dm_uint8 _pad;  // 0x05
};
static_assert(sizeof(dmHeightSample) == 0x6, "dmHeightSample");

struct dmHeightData {  // size 0x20
    dm_int32 GetByteCount() const;
    dm_int32 countX;  // 0x00
    dm_int32 countY;  // 0x04
    dm_float32 cellWidth;  // 0x08
    dmHeightSample * aSamples;  // 0x10
    dm_float32 center;  // 0x18
    dm_float32 scale;  // 0x1c
};
static_assert(sizeof(dmHeightData) == 0x20, "dmHeightData");

struct dmHeightMaterial {  // size 0x8
    dm_float32 friction;  // 0x00
    dm_float32 restitution;  // 0x04
};
static_assert(sizeof(dmHeightMaterial) == 0x8, "dmHeightMaterial");

