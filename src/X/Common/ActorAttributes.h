// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/ActorAttributes.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"

enum {
    SKILL_QUERY_EXCLUDE_STANDARD_ATTACK = 0,
    SKILL_QUERY_EXCLUDE_NON_USABLE = 1,
    SKILL_QUERY_EXCLUDE_NON_LMB = 2,
    SKILL_QUERY_EXCLUDE_TOGGLEABLES = 3,
    SKILL_QUERY_EXCLUDE_MOUSE_ASSIGNABLE = 4,
    SKILL_QUERY_EXCLUDE_NON_HOTBAR = 5,
    SKILL_QUERY_EXCLUDE_NON_MOUSE_ASSIGNABLE = 6,
    SKILL_QUERY_EXCLUDE_NON_TOGGLEABLE = 7,
};

typedef int32 AttribGroupID;

