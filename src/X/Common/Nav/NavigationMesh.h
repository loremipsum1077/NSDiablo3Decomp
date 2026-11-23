// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Nav/NavigationMesh.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "../SNO/SNOFiles.h"
#include "../WorldPlace.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Vector.h"

class NavMeshDef;

static const float cflGridCellWidth = 2.5f;

typedef int32 LevelAreaID;

typedef int32 Team;

enum {
    MARBLE_ROLL_FAT = 0,
    MARBLE_ROLL_ONE_SQUARE = 1,
    MARBLE_ROLL_IGNORE_HEIGHT_ON_DEST = 2,
    MARBLE_ROLL_TRIGGER_UNAVOIDABLE_PROXIMITY_TRIGGERS = 3,
};

enum {
    CELL_STATIC_ALLOW_WALK = 0,
    CELL_STATIC_ALLOW_FLIER = 1,
    CELL_STATIC_ALLOW_SPIDER = 2,
    CELL_STATIC_LEVELAREA_BIT_0 = 3,
    CELL_STATIC_LEVELAREA_BIT_1 = 4,
    CELL_STATIC_SUBSCENE = 5,
    CELL_STATIC_NO_SPAWN = 6,
    CELL_STATIC_SPECIAL_0 = 7,
    CELL_STATIC_SPECIAL_1 = 8,
    CELL_STATIC_ALLOW_PROJECTILE = 10,
    CELL_STATIC_ALLOW_GHOST = 11,
    CELL_STATIC_ROUNDED_CORNER_0 = 12,
    CELL_STATIC_ROUNDED_CORNER_1 = 13,
    CELL_STATIC_ROUNDED_CORNER_2 = 14,
    CELL_STATIC_ROUNDED_CORNER_3 = 15,
    CELL_STATIC_FLAG_COUNT = 16,
    CELL_STATIC_ALLOW_ALL = 3079,
    CELL_STATIC_MASK_ALL_DRAWN = 3583,
    CELL_STATIC_MASK_WALK = 3079,
    CELL_STATIC_MASK_FLY = 3138,
    CELL_STATIC_MASK_GHOST = 2112,
    CELL_STATIC_MASK_BLOCK = 64,
    CELL_STATIC_MASK_PROJ = 3136,
    CELL_STATIC_MASK_SPIDER = 3078,
    CELL_STATIC_MASK_NO_SPAWN = 3143,
    CELL_STATIC_MASK_SPECIAL_BASE = 3079,
    CELL_STATIC_MASK_LEVEL_0 = 0,
    CELL_STATIC_MASK_LEVEL_1 = 8,
    CELL_STATIC_MASK_LEVEL_2 = 16,
    CELL_STATIC_MASK_LEVEL_3 = 24,
    CELL_STATIC_OBSERVER = 7,
};

enum {
    CELL_STATIC_SPECIAL_ALLOW_SPAWN = 0,
    CELL_STATIC_SPECIAL_LEVELAREA_BIT_0 = 1,
    CELL_STATIC_SPECIAL_LEVELAREA_BIT_1 = 2,
};

enum {
    PASSABILITY_UNDEFINED = -1,
    PASSABILITY_PLAYER = 0,
    PASSABILITY_PLAYER_AI_OFF = 1,
    PASSABILITY_PROJECTILE = 2,
    PASSABILITY_SPIDER = 3,
    PASSABILITY_MONSTER = 4,
    PASSABILITY_TOWNPORTAL = 5,
    PASSABILITY_FLIPPIES = 6,
    PASSABILITY_UNKNOWN_OBJECT = 7,
    PASSABILITY_NONMOVER = 8,
    PASSABILITY_CHARGE = 9,
    PASSABILITY_NEW_ENCOUNTER = 10,
    PASSABILITY_PULL_FROM_ACTOR = 11,
    PASSABILITY_FLYER = 12,
    PASSABILITY_LEAP = 13,
    PASSABILITY_OWNED = 14,
    PASSABILITY_GIZMO_OPERATE = 15,
    PASSABILITY_GHOST = 16,
    PASSABILITY_ALLOW_PROJECTILES = 18,
};

typedef int32 PassabilityType;

enum {
    ADJ_NEG_X = 0,
    ADJ_POS_Y = 1,
    ADJ_POS_X = 2,
    ADJ_NEG_Y = 3,
};

enum {
    CRNR_NEG_X_NEG_Y = 0,
    CRNR_NEG_X_POS_Y = 1,
    CRNR_POS_X_POS_Y = 2,
    CRNR_POS_X_NEG_Y = 3,
};

struct PassabilitySettings {  // size 0x10
    PassabilityType ePassabilityType;  // 0x00
    SNO snoActor;  // 0x04
    Angle aYaw;  // 0x08
    float flScale;  // 0x0c
};
static_assert(sizeof(PassabilitySettings) == 0x10, "PassabilitySettings");

struct NavPlace {  // size 0x18
    WorldPlace tPlace;  // 0x00
    NavMeshHandle tNavMeshHandle;  // 0x10
};
static_assert(sizeof(NavPlace) == 0x18, "NavPlace");

typedef XListMemoryPool<NavMeshHandle> TNavMeshHandlePool;

struct NavMeshFloodHandle {  // size 0x10
    NavMeshDef * ptNavMeshDef;  // 0x00
    SSceneID idSScene;  // 0x08
    int32 nSquareIndex;  // 0x0c
};
static_assert(sizeof(NavMeshFloodHandle) == 0x10, "NavMeshFloodHandle");

typedef XListMemoryPool<NavMeshFloodHandle> TNavMeshFloodHandlePool;

typedef XListMemoryPool<Vector3D> TPathPointPool;

typedef BOOL (*NavMeshRollMarbleCallBack)(void *, const NavMeshHandle &);

struct NavMeshRollData {  // size 0x88
    NavMeshRollMarbleCallBack pCallbackFunction;  // 0x00
    void * pCallbackData;  // 0x08
    NavMeshHandle tNavMeshHandleStartPathingSpace;  // 0x10
    NavMeshHandle tNavMeshHandleStartCenter;  // 0x18
    Vector3D wpStart;  // 0x20
    Vector3D wpDest;  // 0x2c
    uint32 dwMarbleRollFlags;  // 0x38
    PassabilitySettings tPassability;  // 0x3c
    PassabilitySettings tDestinationPassability;  // 0x4c
    WorldDistance wdMaxRollDistance;  // 0x5c
    ACDID idACD;  // 0x60
    Vector3D wpEnd;  // 0x64
    NavMeshHandle tNavMeshHandleEndPathingSpace;  // 0x70
    NavMeshHandle tNavMeshHandleEndCenter;  // 0x78
    WorldDistance wdResultRollDistance;  // 0x80
};
static_assert(sizeof(NavMeshRollData) == 0x88, "NavMeshRollData");


// Functions defined in this file (bodies are not in the DWARF):
//   line   227  NavPlace::NavPlace(const NavPlace &)  [inlined into 10 functions]
//   line   227  NavPlace & NavPlace::operator=(const NavPlace &)  [inlined into 2 functions]

// Variables defined in this file:
//   line    23  static const float cflHeadBumpHeight
//   line    24  static const float cflGridCellWidth
//   line    25  static const float cflHalfGridCellWidth

// NavigationMesh.cpp:1804,3645. Declared for X/Common/ActorCommonData.cpp.
SNO NavMeshHandleGetLevelArea(const NavMeshHandle & tNavMeshHandle);
BOOL NavMeshHandleIsActivated(const NavMeshHandle & tNavMeshHandle);
// NavigationMesh.cpp:1815,2411. Declared for X/Common/ActorCommonData.cpp.
typedef int32 LevelAreaID;
LevelAreaID NavMeshHandleGetLevelAreaID(const NavMeshHandle & tNavMeshHandle, BOOL bForDistribution);
BOOL NavMeshHandleIsNone(const NavMeshHandle & tNavMeshHandle);
struct NavMeshInst;
NavMeshInst * NavMeshInstGetByScene(SSceneID idSScene);  // NavigationMesh.cpp:2397
WorldDistance NavMeshGetHeightWorldSpace(const NavMeshHandle & tNavMeshHandle);  // NavigationMesh.cpp:771

BOOL NavMeshHandleGetStaticFlag(const NavMeshHandle & tNavMeshHandle, int32 nFlag);  // NavigationMesh.cpp

struct NavCellHandle;
BOOL NavMeshCheckLOSToACD(NavCellHandle hNavCellStart, const WorldPlace & placeStart, ACDID idACDDest, const PassabilityType ePassabilityType, BOOL bWantProjectileLOS);  // NavigationMesh.cpp:3536

// Declaration placement inferred from the owning API; DWARF records definitions in NavigationMesh.cpp.
void NavMeshGetCenterPlace(const NavMeshHandle & tNavMeshHandle, WorldPlace & tPlaceCenter);  // line 992
uint32 NavMeshHandleGetStaticFlags(const NavMeshHandle & tNavMeshHandle);  // line 1895
uint32 NavMeshGetDynamicFlagsForPassability(PassabilityType ePassability);  // line 3187
