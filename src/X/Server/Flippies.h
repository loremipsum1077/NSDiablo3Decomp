// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Flippies.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"

enum {
    MAX_FLIPPIES_IN_GROUP = 32,
    INITIAL_FLIPPY_COUNT = 64,
};

enum FlippyFlags {
    FF_INVALID = -1,
    FF_NO_HALO_SOUND = 0,
    FF_LAUNCHED = 1,
    FF_LANDED = 2,
    FF_BIAS_TOWARD_CAMERA = 3,
    FF_NUM_FLAGS = 4,
};

enum {
    FLIPPYID_INVALID = -1,
};

typedef int32 FlippyID;

