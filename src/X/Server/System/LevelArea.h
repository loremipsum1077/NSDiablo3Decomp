// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/System/LevelArea.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/DataStructures/XPooledList.h"

enum EncounterRand {
    ENCOUNTER_RAND_A = 0,
    ENCOUNTER_RAND_B = 1,
    ENCOUNTER_RAND_C = 2,
    NUM_ENCOUNTER_RANDS = 3,
};

enum {
    LEVELAREAID_INVALID = -1,
};

typedef int32 LevelAreaID;

enum {
    LEVELAREA_FLAG_ACTIVATED = 0,
};

typedef XPooledDataIDList SpawnGroupIndexList;

struct LevelArea {  // size 0x50
    LevelAreaID id;  // 0x00
    SWorldID idSWorld;  // 0x04
    SNO snoLevelArea;  // 0x08
    int32 nPopulationIndex;  // 0x0c
    SpawnGroupIndexList listSpawnGroupIndexes;  // 0x10
    SSceneID idSSceneWaypoint;  // 0x38
    int32 nEncounterRand[3];  // 0x3c
    uint32 dwFlags;  // 0x48
    uint32 dwDeleteTime;  // 0x4c
};
static_assert(sizeof(LevelArea) == 0x50, "LevelArea");

LevelArea * LevelAreaTryToGet(LevelAreaID idLevelArea);  // LevelArea.cpp:499

class CRefString;
void LevelAreaGetName(const SNO snoLevelArea, CRefString & sName);  // LevelArea.cpp:605

SNO LevelAreaGetSNO(LevelAreaID idLevelArea);  // LevelArea.cpp

struct RNG;
// LevelArea.cpp:141, :486, :581
LevelAreaID LevelAreaCreate(SNO snoLevelArea, SWorldID idSWorld, RNG & tRng);
LevelArea & LevelAreaGet(LevelAreaID idLevelArea);
LevelAreaID LevelAreaFind(const SNO snoLevelArea, const SWorldID idSWorld);
