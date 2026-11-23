// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Crafting/ItemCraftingSwig.h
// Crafter types only; nothing here has code.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

enum {
    CRAFTER_TYPE_INVALID = -1,
    CRAFTER_TYPE_BLACKSMITH = 0,
    CRAFTER_TYPE_JEWEL = 1,
    CRAFTER_TYPE_MYSTIC = 2,
    CRAFTER_TYPE_HORADRIM = 3,
    CRAFTER_TYPE_LAST_IMPROVEABLE = 3,
    CRAFTER_TYPE_JEWEL_UPGRADE = 4,
    CRAFTER_TYPE_COUNT = 5,
};

typedef int32 CrafterType;

