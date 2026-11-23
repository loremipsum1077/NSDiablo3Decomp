// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/AI/WildlifeBrains.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

struct WoodWraithBrainData {  // size 0x8
    uint32 dwSpecialSkillCooldownTime[2];  // 0x00
};
static_assert(sizeof(WoodWraithBrainData) == 0x8, "WoodWraithBrainData");
