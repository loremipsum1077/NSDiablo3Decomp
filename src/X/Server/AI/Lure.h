// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/AI/Lure.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"

enum {
    LURE_DATA_BLOCK_COUNT = 64,
    LURE_MAX_ACTOR_SNOS = 4,
    LURE_MAX_AFFECTED = 16,
    LURE_MAX_LOCATIONS = 24,
};

struct LuredActor {  // size 0xc
    ACDID idACD;  // 0x00
    ACDID idACDLocation;  // 0x04
    int32 nLocationIdx;  // 0x08
};
static_assert(sizeof(LuredActor) == 0xc, "LuredActor");

struct AILureData {  // size 0x120
    LuredActor arLured[16];  // 0x00
    int32 arLocationUsedCount[24];  // 0xc0
};
static_assert(sizeof(AILureData) == 0x120, "AILureData");
