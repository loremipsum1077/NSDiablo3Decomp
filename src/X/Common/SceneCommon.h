// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/SceneCommon.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/Domino/Include/dmId.h"
#include "CommonTypes.h"
#include "DRLGEncounters.h"
#include "Definitions/WorldDefinition.h"
#include "SNO/SNOFiles.h"
#include "SNO/SNOName.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/Math/Vector.h"

struct InactiveMarkerList;
class NavMeshInst;
struct NavZoneInstance;
struct Player;
struct WorldPlace;
class Frustum;

enum {
    MAX_SCENES = 5120,
};

typedef int32 PassabilityType;

enum SceneFlags {
    SCENE_ACTIVATED = 0,
    SCENE_WAS_ACTIVATED_BEFORE = 1,
    SCENE_DONT_INSTANTIATE_MONSTERS = 2,
    SCENE_LOCKED = 3,
    SCENE_FLAGGED_FOR_DELETE = 4,
    SCENE_NAVMESH_LOADED = 5,
    SCENE_EXCLUSIONS_INITIALIZED = 6,
    SCENE_HAS_ANY_EXCLUSIONS = 7,
};

enum CSceneDataArrayIterateMode {
    SkipHolodeckScenes = 0,
    IncludeHolodeckScenes = 1,
};

typedef XPooledList<int, int> SceneChildIDListPooled;

struct EnvAppData {  // size 0x34
    SNO snoAppearance;  // 0x00
    SNO snoPhysMesh;  // 0x04
    PRTransform transform;  // 0x08
    float flScale;  // 0x24
    dmBodyId bodyId;  // 0x28
    SSceneID idSScene;  // 0x30
};
static_assert(sizeof(EnvAppData) == 0x34, "EnvAppData");

typedef XListMemoryPool<EnvAppData> TEnvAppDataPool;

typedef XPooledList<EnvAppData, const EnvAppData &> TEnvAppDataList;

struct GizmoLocPlacement {  // size 0x34
    SNOName snoname;  // 0x00
    DRLGGizmoPlace tPlace;  // 0x08
};
static_assert(sizeof(GizmoLocPlacement) == 0x34, "GizmoLocPlacement");

typedef XListMemoryPool<GizmoLocPlacement> TGizmoLocPlacementDataPool;

typedef XPooledList<GizmoLocPlacement, const GizmoLocPlacement &> TGizmoLocPlacementList;

struct SceneCommonData {  // size 0x2b8
    SSceneID idSScene;  // 0x00
    SWorldID idSWorld;  // 0x04
    SceneSpecification tSceneSpec;  // 0x08
    SNO snoScene;  // 0xe4
    PRTransform transformWorld;  // 0xe8
    PRTransform transformInvWorld;  // 0x104
    AABB aabbBoundsLocalLogical;  // 0x120
    AABB aabbBoundsLocalPhysical;  // 0x138
    AABB aabbMarkerSetBoundsLocalPhysical;  // 0x150
    Rect2D rect2DSceneWorld;  // 0x168
    IVector2D tNavMeshOffset;  // 0x178
    float flNavMeshOffsetZ;  // 0x180
    NavMeshInst * ptNavMeshInst;  // 0x188
    NavZoneInstance * ptNavZoneInst;  // 0x190
    InactiveMarkerList * ptMarkerScene;  // 0x198
    InactiveMarkerList * ptMarkersToBeActivated;  // 0x1a0
    uint32 dwSceneFlags;  // 0x1a8
    int32 nLockReferenceCount;  // 0x1ac
    uint32 dwPlayerLastSeenTime;  // 0x1b0
    uint32 dwScenePlayerSeenBits;  // 0x1b4
    uint32 dwPrefetchScenePlayerSeenBits;  // 0x1b8
    SceneChildIDListPooled tChildSceneIDs;  // 0x1c0
    SSceneID idSSceneParent;  // 0x1e8
    SNO snoSceneGroup;  // 0x1ec
    SNO snoMarkerSetSrc;  // 0x1f0
    int nMarkerSetSrcIndex;  // 0x1f4
    int32 nInstanceUID;  // 0x1f8
    LabelListPooled listAppliedLabels;  // 0x200
    TEnvAppDataList listEnvApps;  // 0x228
    TGizmoLocPlacementList listGizmoLocPlacements;  // 0x250
    TGizmoLocPlacementList listGizmoLocLoot;  // 0x278
    uint32 * ptSpawnBitMask;  // 0x2a0
    uint32 uSpawnBitMaskDWordSize;  // 0x2a8
    uint32 * ptSpawnBitMaskElites;  // 0x2b0
};
static_assert(sizeof(SceneCommonData) == 0x2b8, "SceneCommonData");

typedef TDataArrayIterator<void> SceneCommonDataIterator;

SceneCommonData & SceneCommonDataGet(SSceneID idSScene);
SceneCommonData * SceneCommonDataTryToGet(SSceneID idSScene);
void SceneCreateAVFrustum(Player * ptPlayer, Frustum & tFrustum);
void SceneCreatePlayerFrustum(const Player * ptPlayer, Frustum & tFrustum, Vector3D & vCenterPos);
void SceneActivateVisible(SWorldID idSWorld, const Frustum & tFrustum, const Frustum & tPrefetchFrustum, const Frustum * ptAVFrustum, const Vector3D & vCenter, Player ** pptPlayers, int32 nPlayerCount);
void SceneDeactivateUnseen();
void SceneCheckInactiveMarkers(SWorldID idSWorld, Frustum & tFrustum, Vector3D & vCenterPos);

struct NavMeshStats {  // size 0x8
    NavMeshStats();
    int32 nNavMeshTotalSquareCount;  // 0x00
    int32 nNavMeshPlayableSquareCount;  // 0x04
    float GetSqFtArea() const;
    float GetSqFtPlayableArea() const;
};
static_assert(sizeof(NavMeshStats) == 0x8, "NavMeshStats");


// Functions defined in this file (bodies are not in the DWARF):
//   line    67  EnvAppData & EnvAppData::operator=(const EnvAppData &)  [inlined into 2 functions]
//   line    80  GizmoLocPlacement & GizmoLocPlacement::operator=(const GizmoLocPlacement &)  [inlined into 1 functions]
//   line    89  SceneCommonData::SceneCommonData()  [inlined into 2 functions]
//   line    89  SceneCommonData::~SceneCommonData()  [inlined into 2 functions]

// SceneCommon.cpp:1117,1547. Original NoSpawnTree::Populate callees.
void SceneGetLogicalAABBInWorldSpace(const SceneCommonData & tSCD, AABB & aabb);
SceneCommonData * SceneCommonDataArrayIterate(SceneCommonDataIterator & tSCDIterator, SceneCommonData *& ptSCD, CSceneDataArrayIterateMode eIterateMode);

// SceneCommon.cpp:1149,2673. Declared for X/Common/ActorCommonData.cpp.
SSceneID SceneCommonGetSceneIDFromWorldPlace(const WorldPlace & tPlace, BOOL bIncludeInactiveScenes);
class ActorCommonData;
BOOL SceneCommonCanTownPortal(const SSceneID idSScene, const ActorCommonData & tACD);
BOOL ScenesCommonNavMeshNodeFromPlace(const WorldPlace & tPlace, NavMeshHandle * ptNavMeshHandle, const BOOL fCanActivateScenes, const BOOL fProcessInactive);  // SceneCommon.cpp:1240

SNO ScenesCommonLevelAreaFromPlace(SSceneID idSScene, Vector3D wp, BOOL fProcessInactive);
