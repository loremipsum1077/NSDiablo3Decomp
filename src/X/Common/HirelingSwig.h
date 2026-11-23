// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/HirelingSwig.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"

enum {
    HC_NONE = 0,
    HC_TEMPLAR = 1,
    HC_SCOUNDREL = 2,
    HC_ENCHANTRESS = 3,
    NUM_HIRELING_CLASSES = 4,
    HC_FIRST = 1,
};

enum {
    HT_NORMAL = 0,
    HT_PROXY = 1,
    HT_SUMMONED = 2,
    NUM_HIRELING_TYPES = 3,
};

typedef int32 HirelingType;

