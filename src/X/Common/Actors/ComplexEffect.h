// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Actors/ComplexEffect.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

enum {
    SCOMPLEXEFFECTID_INVALID = -1,
};

typedef int32 SComplexEffectID;

enum {
    COMPLEXEFFECT_TYPE_NORMAL = 0,
    COMPLEXEFFECT_TYPE_BEAM_TARGET = 1,
    COMPLEXEFFECT_TYPE_SPECIAL_ROPE = 2,
    COMPLEXEFFECT_TYPE_COUNT = 3,
};

typedef int32 ComplexEffectType;

enum {
    MAX_COMPLEX_EFFECTS = 128,
};

