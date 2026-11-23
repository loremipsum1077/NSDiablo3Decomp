// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/SScene.h
// Subset: SScene's id, SceneCommonData and level-area ids, and SSceneTryToGet. The other
// members are padding (invented names) until someone needs them.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/CommonTypes.h"
#include "../Common/SceneCommon.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"

typedef int32 LevelAreaID;

struct SceneScript {  // size 0x8
    SNO snoScript;  // 0x00
    uint16 nAdventureParentIndex;  // 0x04
};
static_assert(sizeof(SceneScript) == 0x8, "SceneScript");

typedef XGrowableList<SceneScript, const SceneScript &> SceneScriptList;

typedef XPooledList<unsigned long, unsigned long> HeroIdListPooled;

struct SScene {  // size 0x3b8
    SSceneID id;  // 0x00
    SceneCommonData tSCD;  // 0x08
    LevelAreaID arLevelAreaIds[4];  // 0x2c0
    XPooledDataIDList listACDInScene;  // 0x2d0
    SceneScriptList listScriptsInScene;  // 0x2f8
    HeroIdListPooled listSeenPlayerIDs;  // 0x350
    dmBodyId bodyId;  // 0x378
    BOOL bHasWaypoint;  // 0x380
    PRTransform transWaypoint;  // 0x384
    SNO snoWaypointTexture;  // 0x3a0
    float fWaypointMaxRange;  // 0x3a4
    float fWaypointMinRange;  // 0x3a8
    float fWaypointDiscoveryRange;  // 0x3ac
    BOOL bIsVista;  // 0x3b0
};
static_assert(sizeof(SScene) == 0x3b8, "SScene");

SScene * SSceneTryToGet(SSceneID idSScene);
SScene * SSceneGet(SSceneID idSScene);  // SScene.cpp:393
void SSceneFlagForRemoval(SSceneID idSScene);  // SScene.cpp:269

void SScenePutScriptInScene(const SSceneID idSScene, const SNO snoScript,
                            const uint16 nAdventureParentIndex);  // SScene.cpp:540
