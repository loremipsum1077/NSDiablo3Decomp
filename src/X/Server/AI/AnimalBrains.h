// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/AI/AnimalBrains.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"

enum {
    PIG_CAGED_MEMORY_TICKS_MIN = 300,
    PIG_CAGED_MEMORY_TICKS_DELTA = 200,
    PIG_CAGED_VIEW_DEGREES = 180,
};

enum {
    PIG_CAGED_SKILLINDEX_CLOSE = 0,
    PIG_CAGED_SKILLINDEX_SNIFF = 1,
};

struct PigCagedBrainData {  // size 0x8
    ACDID idACDSniffed;  // 0x00
    uint32 dwACDMemoryTime;  // 0x04
};
static_assert(sizeof(PigCagedBrainData) == 0x8, "PigCagedBrainData");

PigCagedBrainData * PigCagedTryGetBrainData(ACDID idACD);  // AnimalBrains.cpp:223
