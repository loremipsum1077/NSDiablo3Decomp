// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/AI/TriuneBrains.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Nav/NavigationMesh.h"

enum {
    TRIUNE_SUMMONER_NUM_CLONES = 2,
    TRIUNE_SUMMONER_NUM_SUMMON_SPOTS = 3,
};

enum {
    TRIUNESUMMONER_SKILLINDEX_PROJECTILE = 0,
    TRIUNESUMMONER_SKILLINDEX_SUMMONDEMON = 1,
    TRIUNESUMMONER_SKILLINDEX_CREATECLONES = 2,
    TRIUNESUMMONER_SKILLINDEX_SHIELD = 3,
};

struct TriuneSummonerBrainData {  // size 0x70
    NavPlace tSummonNavPlace;  // 0x00
    NavPlace tSplitNavPlace[3];  // 0x18
    ACDID idACDClone[2];  // 0x60
    ACDID idACDMaster;  // 0x68
    uint32 nNextSummonTime;  // 0x6c
};
static_assert(sizeof(TriuneSummonerBrainData) == 0x70, "TriuneSummonerBrainData");


// X/Server/Ai/TriuneBrains.cpp:324
TriuneSummonerBrainData * TriuneSummonerTryGetBrainData(ACDID idACD);
