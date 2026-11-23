// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Ai/TargetEvaluate.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

enum {
    TARGETEVALUATE_NONE = -1,
    TARGETEVALUATE_RANDOM_TARGET = 0,
    TARGETEVALUATE_CLOSEST_TARGET = 1,
    TARGETEVALUATE_FARTHEST_TARGET = 2,
    TARGETEVALUATE_HAS_POWER_BUFF = 3,
    TARGETEVALUATE_RANDOM_TARGET_NOT_LARGE_OR_KNOCKBACKABLE = 4,
    TARGETEVALUATE_CLOSEST_PLAYER_OR_PET_NEEDS_HEAL = 5,
    TARGETEVALUATE_CLOSEST_TARGET_IN_PIE = 6,
};

typedef int32 TargetEvaluate;

enum TargetTeam {
    TARGET_TEAM_ENEMY = 0,
    TARGET_TEAM_FRIENDLY = 1,
};
