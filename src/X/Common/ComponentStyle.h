// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/ComponentStyle.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 InventorySlotType;

enum {
    COMPONENTSTYLE_NONE = 0,
    COMPONENTSTYLE_LIGHT_A = 1,
    COMPONENTSTYLE_LIGHT_B = 2,
    COMPONENTSTYLE_MEDIUM_A = 3,
    COMPONENTSTYLE_MEDIUM_B = 4,
    COMPONENTSTYLE_HEAVY_A = 5,
    COMPONENTSTYLE_HEAVY_B = 6,
    COMPONENTSTYLE_LIGHT_C = 7,
    COMPONENTSTYLE_MEDIUM_C = 8,
    COMPONENTSTYLE_HEAVY_C = 9,
    COMPONENTSTYLE_CLASS_LIGHT_A = 10,
    COMPONENTSTYLE_CLASS_LIGHT_B = 11,
    COMPONENTSTYLE_CLASS_LIGHT_C = 12,
    COMPONENTSTYLE_CLASS_MEDIUM_A = 13,
    COMPONENTSTYLE_CLASS_MEDIUM_B = 14,
    COMPONENTSTYLE_CLASS_MEDIUM_C = 15,
    COMPONENTSTYLE_CLASS_HEAVY_A = 16,
    COMPONENTSTYLE_CLASS_HEAVY_B = 17,
    COMPONENTSTYLE_CLASS_HEAVY_C = 18,
    NUM_COMPONENTSTYLES = 19,
};

typedef int32 ComponentStyle;


// ComponentStyle.cpp:31.
LPCSTR InventorySlotToComponentString(InventorySlotType eSlot);
