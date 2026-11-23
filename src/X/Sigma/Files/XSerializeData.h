#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

struct SerializeData {  // size 0x8
    uint32 dwOffset;  // 0x00
    uint32 dwSize;  // 0x04
    // Line 18; the check and the division are one statement (line 20). Proven by
    // StringListGroupDef::Postprocess (where the check folds away) and Achievements::PostprocessStaticData.
    uint32 ElementCount(uint32 dwElementSize) const { return dwSize ? dwSize / dwElementSize : 0; }
};
static_assert(sizeof(SerializeData) == 0x8, "SerializeData");
