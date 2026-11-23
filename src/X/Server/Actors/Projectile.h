// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Actors/Projectile.h
// Subset.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/DetPaths.h"
#include "../../Common/Target.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/Vector.h"

typedef int32 SpellEffect;

enum AimMethod {
    AIMMETHOD_LINEAR = 0,
    AIMMETHOD_FIXED_GRAVITY = 1,
};

enum {
    PROJECTILE_DETONATE_AFTER_FIXED_TIME_BIT = 0,
    PROJECTILE_GUIDED_BIT = 1,
    PROJECTILE_DONT_HIT_ACDS_BIT = 2,
    PROJECTILE_LOB_BIT = 3,
    PROJECTILE_DONT_DESTROY_ON_IMPACT = 4,
    PROJECTILE_HUGS_GROUND_BIT = 5,
    PROJECTILE_DONT_HIT_MONSTERS_BIT = 6,
    PROJECTILE_DONT_PERFORM_MARBLE_ROLL_TO_CASTER_BIT = 7,
    PROJECTILE_NAV_REFLECT = 8,
    PROJECTILE_USE_ACTOR_STATIC_FLAGS_FOR_SPAWN = 9,
};

BOOL ProjectileIsReflected(ACDID idACDProjectile);  // Projectile.cpp:1105
struct ImpactData;
BOOL ProjectileHadCollision(ImpactData & tImpactData);
struct WorldPlace;
void ProjectileSetDetonationTime(ACDID idACDProjectile, uint32 dwDetTime);  // Projectile.cpp:652
void ProjectileSetSpeedMultiplier(ACDID idACDProjectile, float flSpeedMultiplier, int32 nDuration, BOOL bAdjustDetonationTimes, BOOL bSmoothSlow, BOOL bForce);  // Projectile.cpp:958
void ProjectileSeekPlace(ACDID idACDProjectile, WorldPlace tPlace, float flMaxAngleToRotate, BOOL bYawOnly);  // Projectile.cpp:1374
ACDID ProjectileGetReflectedByACD(ACDID idACDProjectile);  // Projectile.cpp:1116
BOOL ProjectileDestroyOnImpact(ACDID idACDProjectile);  // Projectile.cpp:1365
int32 ProjectileActorGetHitListCount(ACDID idACDProjectile);  // Projectile.cpp:1592
float ProjectileGetSpeedMultiplier(ACDID idACDProjectile);  // Projectile.cpp:1051

struct ProjectileCurvedOffsetData {  // size 0x4
    float flLateralOffset;  // 0x00
};
static_assert(sizeof(ProjectileCurvedOffsetData) == 0x4, "ProjectileCurvedOffsetData");

struct ProjectileRenderingData {  // size 0x8
    WorldDistance wdRopeLength;  // 0x00
    SNO snoPR;  // 0x04
};
static_assert(sizeof(ProjectileRenderingData) == 0x8, "ProjectileRenderingData");

struct ProjectileAnimationData {  // size 0x14
    WorldPlace placeStart;  // 0x00
    WorldDistance wdSpeed;  // 0x10
};
static_assert(sizeof(ProjectileAnimationData) == 0x14, "ProjectileAnimationData");

// The DWARF lists ProjectileAimingData() (line 65) and ProjectileData() (line 95) as inline
// constructors; every caller shows only Target::Target() on tDest, so the implicit ones are used.
struct ProjectileAimingData {  // size 0x38
    AimMethod eAimMethod;  // 0x00
    Target tDest;  // 0x04
    Vector3D wvTargetVel;  // 0x20
    WorldDistance wdExtraDistanceOnFirstStep;  // 0x2c
    WorldDistance wdGravity;  // 0x30
    float flHeightAboveSource;  // 0x34
};
static_assert(sizeof(ProjectileAimingData) == 0x38, "ProjectileAimingData");

struct ProjectileDurationData {  // size 0x4
    uint32 dwTime;  // 0x00
};
static_assert(sizeof(ProjectileDurationData) == 0x4, "ProjectileDurationData");

struct ProjectileResponseData {  // size 0x4
    float flUnitBounceSpeed;  // 0x00
};
static_assert(sizeof(ProjectileResponseData) == 0x4, "ProjectileResponseData");

struct ProjectileSpiralData {  // size 0x10
    WorldPlace placeSpiralCenter;  // 0x00
};
static_assert(sizeof(ProjectileSpiralData) == 0x10, "ProjectileSpiralData");

struct ProjectileData {  // size 0xbc
    uint32 dwFlags;  // 0x00
    SpellEffect eSpellEffect;  // 0x04
    SNO snoPower;  // 0x08
    ProjectileRenderingData tRenderingData;  // 0x0c
    ProjectileAnimationData tAnimationData;  // 0x14
    ProjectileAimingData tAimingData;  // 0x28
    ProjectileDurationData tDurationData;  // 0x60
    ProjectileResponseData tResponseData;  // 0x64
    ACDID idACDSnapSource;  // 0x68
    int32 nSnapHardPoint;  // 0x6c
    DeterministicPathType eDPath;  // 0x70
    DPathSinData tDPathSinData;  // 0x74
    DPathPieData tDPathPieData;  // 0x90
    ProjectileCurvedOffsetData tDPathCurvedOffsetData;  // 0xa8
    ProjectileSpiralData tDPathSpiralData;  // 0xac
};
static_assert(sizeof(ProjectileData) == 0xbc, "ProjectileData");

struct ActorCommonData;
const ActorCommonData * ProjectileGetOwnerACD(ACDID idACDProjectile);  // Projectile.cpp:1080
