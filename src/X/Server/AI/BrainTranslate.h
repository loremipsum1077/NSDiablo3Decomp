// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/AI/BrainTranslate.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/MathGeneral.h"

typedef int32 TranslateType;

struct BrainComponentTranslate {  // size 0x34
    ACDID idACD;  // 0x00
    WorldPlace placeGotoPoint;  // 0x04
    WorldDistance wdSpeed;  // 0x14
    TranslateType nTranslateType;  // 0x18
    uint32 dwAnimTag;  // 0x1c
    WorldDistance wdMaxRangeFromDest;  // 0x20
    uint32 dwTimeStartedWalking;  // 0x24
    uint32 dwDoneWaitingTime;  // 0x28
    uint32 dwDoneSteppingAwayTime;  // 0x2c
    ACDID idACDTarget;  // 0x30
};
static_assert(sizeof(BrainComponentTranslate) == 0x34, "BrainComponentTranslate");

void BrainTranslateCancel(BrainComponentTranslate & tTranslate, BOOL bForceByPopping);  // BrainTranslate.cpp:122
void BrainTranslateInitForGame();  // BrainTranslate.cpp:65
void BrainTranslateDisposeForGame();  // BrainTranslate.cpp:78
