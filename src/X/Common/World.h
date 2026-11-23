// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/World.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "SNO/SNOFiles.h"
#include "WorldPlace.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Math/Quaternion.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/SigmaTypes.h"

struct ActorSphereTree;
struct InactiveMarkerList;
struct NavMeshWorldStitching;
class XSphereArray;

enum {
    MAX_WORLDS = 256,
    DEFAULT_MAX_LIVE_WORLDS = 256,
};

typedef int32 MonsterRarity;

typedef int32 Team;

typedef int32 LevelAreaID;

typedef int32 PlayerWarpReason;

enum WorldStatusBits {
    WORLD_STATUS_CLEAR_BIT = 0,
    WORLD_STATUS_BIT_COUNT = 1,
};

struct WorldSyncedData {  // size 0xc
    SNO snoWeatherOverride;  // 0x00
    float flWeatherIntensityOverride;  // 0x04
    uint32 dwWeatherIntensityOverrideEnd;  // 0x08
};
static_assert(sizeof(WorldSyncedData) == 0xc, "WorldSyncedData");

struct WorldSyncedDataMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    SWorldID idSWorld;  // 0x08
    WorldSyncedData tSyncedData;  // 0x0c
};
static_assert(sizeof(WorldSyncedDataMessage) == 0x18, "WorldSyncedDataMessage");

struct WorldCommonData {  // size 0x70
    SNO snoWorld;  // 0x00
    SNO snoDungeonFinderSourceWorld;  // 0x04
    SWorldID idSWorld;  // 0x08
    BOOL fTreatAsOneScene;  // 0x0c
    NavMeshWorldStitching * ptWorldStitching;  // 0x10
    ActorSphereTree * ptActorSphereTreeNormal;  // 0x18
    ActorSphereTree * ptActorSphereTreeFastMover;  // 0x20
    ActorSphereTree * ptActorSphereTreeItemsInWorld;  // 0x28
    XSphereArray * ptACDCollSphereTree;  // 0x30
    XSphereArray * ptACDBlockerCollSphereTree;  // 0x38
    uint32 dwWorldStatusBits;  // 0x40
    IVector2D pointWorldOrigin;  // 0x44
    InactiveMarkerList * ptMarkerWorld;  // 0x50
    InactiveMarkerList * ptMarkersToActivate;  // 0x58
    uint32 dwPlayerLastSeenTime;  // 0x60
    WorldSyncedData tWorldSyncedData;  // 0x64
};
static_assert(sizeof(WorldCommonData) == 0x70, "WorldCommonData");

struct StartLocationOverride {  // size 0x28
    SNO snoWorld;  // 0x00
    WorldPlace placeEntrance;  // 0x04
    Quaternion qEntrance;  // 0x14
    BOOL fSceneEditModeWorldOrigin;  // 0x24
};
static_assert(sizeof(StartLocationOverride) == 0x28, "StartLocationOverride");

typedef void (*WorldCreatedCallback)(SWorldID, ACDID, ACDID, uint32, PlayerWarpReason);

struct DungeonFinderNameOverride {  // size 0x8
    int32 nPrefixIndex;  // 0x00
    int32 nNounIndex;  // 0x04
};
static_assert(sizeof(DungeonFinderNameOverride) == 0x8, "DungeonFinderNameOverride");

typedef TDataArrayIterator<void> WorldCommonDataIterator;

WorldCommonData * WorldTryGetCommonData(SWorldID idSWorld);

// Defined in X/Common/World.cpp:324. Declared here as callers outside World.cpp need it.
WorldCommonData & WorldGetCommonData(SWorldID idSWorld);


// World.cpp (X/Common/Game.cpp GameGetCustomCrashDescription).
SNO WorldCommonGetWorldSNO(SWorldID idSWorld);

// World.cpp:869.
BOOL WorldIsDungeonFinderPlaceHolder(const SNO snoWorld);

int32 WorldGetDungeonFinderLevel(const SNO snoWorld);
