// Original file: X/Server/Actors/Projectile.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Target.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/WorldPlace.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/MemoryPools.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../Timers.h"
#include "../Powers/Powers.h"
#include "Actors.h"
#include "ActorPowers.h"
#include "Projectile.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../Movement/SActorMovement.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../Powers/PowerAPI.h"
#include "ActorEffect.h"
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../../Common/ActorCommonDataSwig.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"

struct ActorProjectile {  // size 0xa0
    uint32 dwFlags;  // 0x00
    SNO snoPower;  // 0x04
    Target tDest;  // 0x08
    Vector3D wvVel;  // 0x24
    AimMethod eAimMethod;  // 0x30
    WorldDistance wdGravity;  // 0x34
    Vector3D wpStartedFlight;  // 0x38
    int32 nTotalFlightTime;  // 0x44
    ACDID idACDSnapSource;  // 0x48
    int32 nSnapHardPoint;  // 0x4c
    float flSpeedMultiplier;  // 0x50
    uint32 dwSlowUntilTick;  // 0x54
    uint32 dwStartOfSlowTick;  // 0x58
    BOOL bSmoothSlow;  // 0x5c
    BOOL bWasReflected;  // 0x60
    ACDID idACDReflectedBy;  // 0x64
    XPooledDataIDList listACDsHit;  // 0x68
    float flUnitBounceSpeed;  // 0x90
    float wdLaunchZVelocity;  // 0x94
    TimerID m_idSpeedResetTimer;  // 0x98
};
static_assert(sizeof(ActorProjectile) == 0xa0, "ActorProjectile");

int32 ProjectileGetTicksTillDetonate(ACDID idACDProjectile);  // X/Common/ProjectileCommon.cpp:30
BOOL ACDIgnoresProjectiles(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp:9167
BOOL ACDBlocksProjectiles(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp:9198
BOOL AIStateOnHitReaction(const ACDID idACD);  // X/Server/AI/AIStates.cpp:5987
SpellEffect ActorPowersGetSpellEffect(ActorID idActor);  // X/Server/Actors/ActorPowers.cpp:457
//  // X/Common/ProjectileCommon.cpp:30

struct ActorProjectileGlobals {  // size 0x30
    XChainedFixedBlockAllocator<ActorProjectile> poolActorProjectiles;  // 0x00
};
static_assert(sizeof(ActorProjectileGlobals) == 0x30, "ActorProjectileGlobals");

void ACDsIntersectCapsule(SWorldID idSWorld, const Capsule & capsule, XPooledDataIDList & listACDs, BOOL bForPower);  // X/Common/Actors/ActorCollision.cpp:518
ActorID SceneryAdd(const ActorAddStruct & tActorAddOriginal);  // X/Server/Actors/Actors.cpp:5865
BOOL NavMeshGetHeightAndNormal(const NavMeshHandle & tNavMeshHandle, const Vector3D & wp, const BOOL fPinToSquareBounds, WorldDistance & flHeight, Vector3D & wvNormal);  // X/Common/Nav/NavigationMesh.cpp:783









void ActorProjectileInitializeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    ActorProjectileGlobals *& ptActorProjectileGlobals = tSGame.ptActorProjectileGlobals;
    ptActorProjectileGlobals = (ActorProjectileGlobals *)SigmaMemoryNew(sizeof(ActorProjectileGlobals), 4, tSGame.ptMemoryInterface, TRUE);
    ptActorProjectileGlobals->poolActorProjectiles.Initialize();
    ptActorProjectileGlobals->poolActorProjectiles.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptActorProjectileGlobals->poolActorProjectiles.SetGrow(XALLOC_GROW_LINEAR, 512);
    ptActorProjectileGlobals->poolActorProjectiles.SetBlockCount_Tracked(512);
}






void ActorProjectileDisposeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    ActorProjectileGlobals *& ptActorProjectileGlobals = tSGame.ptActorProjectileGlobals;
    if (ptActorProjectileGlobals)
    {
        ptActorProjectileGlobals->poolActorProjectiles.Dispose();
        SigmaMemoryFree(ptActorProjectileGlobals, tSGame.ptMemoryInterface);
        ptActorProjectileGlobals = NULL;
    }
}





static ActorProjectile * sProjectileGet(ACDID idACDProjectile)
{

    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    Actor * ptActor = ActorTryToGet(idActorProjectile);




    return ptActor ? ptActor->ptProjectile : NULL;  // 125:12 and 125:31; the NULL arm survives when inlined into ProjectileAdd
}




static ActorProjectile * sTryToProjectileGet(ACDID idACDProjectile)
{
    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    Actor * ptActor = ActorTryToGet(idActorProjectile);
    if (!ptActor)
    {
        return NULL;
    }
    return ptActor->ptProjectile;
}







static void sAimLinear(
    const Vector3D & wpSrc,
    const Vector3D & wpDest,
    const WorldDistance & wdSpeed,
    const WorldDistance & wdExtraDistanceOnFirstStep,
    Vector3D & wvDirection,
    float & flEstimatedFlightTime)
{



	wvDirection = wpDest - wpSrc;
	float flDist = wvDirection.Normalize();
	float flDistMinusExtra = Math::Max(flDist - wdExtraDistanceOnFirstStep, 0.0f);
	flEstimatedFlightTime = flDistMinusExtra / wdSpeed;
}






void ProjectileDispose(const ACDID idACDProjectile)
{
    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    Actor * ptActorProjectile = ActorGet(idActorProjectile);
    if (!ptActorProjectile->ptProjectile)
    {
        return;
    }

    ptActorProjectile->ptProjectile->listACDsHit.RemoveAll();

    SGame & tSGame = SGameManager::GameGet();
    tSGame.ptActorProjectileGlobals->poolActorProjectiles.Free(ptActorProjectile->ptProjectile);
}




BOOL ProjectileHadCollision(ImpactData & tImpactData)
{
    ActorCommonData * ptACDProjectile = ACDTryToGet(tImpactData.idACDProjectile);
    if (!ptACDProjectile)
    {
        return FALSE;
    }

    ActorCommonData * ptACDImpacted = ACDTryToGet(tImpactData.idACDIntersected);
    if (ptACDImpacted)
    {
        if (ptACDProjectile->GetPlace().idSWorld != ptACDImpacted->GetPlace().idSWorld)
        {
            return FALSE;
        }

        if (ptACDImpacted->IsGizmo() && ptACDImpacted->AttributesGetInt(ATTRIBUTE_INVULNERABLE) > 0)
        {
            tImpactData.idACDIntersected = ACDID_INVALID;
        }

        if (ACDBlocksProjectiles(*ptACDImpacted))
        {
            tImpactData.idACDIntersected = ACDID_INVALID;
        }

        if (ptACDImpacted->GetActorType() == ACTORTYPE_SERVER_PROP)
        {
            tImpactData.idACDIntersected = ACDID_INVALID;
        }

        if (ActorDefinitionGetTagMapValueInt(TAG_ACTOR_FORCE_GETHIT_EFFECT_ON_PROJECTILE_COLLISION, ptACDImpacted->snoActor))
        {
            ACDPlayEffect(ptACDImpacted->id, EFFECT_HIT, 0, ACDID_INVALID);
        }
    }

    if (AIStateOnHitReaction(tImpactData.idACDIntersected))
    {
        ActorProjectile * ptProjectile = sTryToProjectileGet(tImpactData.idACDProjectile);
        if (ptProjectile)
        {
            ptProjectile->listACDsHit.AddTail_Tracked(tImpactData.idACDIntersected);
        }
        return FALSE;
    }

    ActorID idActorProjectile = (ActorID)ACDGetOwner(tImpactData.idACDProjectile);
    PowerID idPower = ActorPowersGetOwnerPower(idActorProjectile);
    SpellEffect eSpellEffect = ActorPowersGetSpellEffect(idActorProjectile);
    BOOL fStoppedAtTarget = TRUE;
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        tImpactData.eSpellEffect = eSpellEffect;
        SpecificPowerSendMessage(idPower, POWER_IMPACT, &tImpactData);
        fStoppedAtTarget = tImpactData.bStop;
    }

    ActorProjectile * ptProjectile = sTryToProjectileGet(tImpactData.idACDProjectile);
    if (ptProjectile)
    {
        ptProjectile->listACDsHit.AddTail_Tracked(tImpactData.idACDIntersected);
    }
    return fStoppedAtTarget;
}














ACDID ProjectileAdd(
    const ProjectileData & tProjectileData)
{
    WorldPlace placeDest;
	tProjectileData.tAimingData.tDest.GetPlace(placeDest);

    WorldPlace placeStart = tProjectileData.tAnimationData.placeStart;
	if (!WorldPlaceInSameWorld(placeStart, placeDest))
	{
		return ACDID_INVALID;
	}





    float flSpeed = tProjectileData.tAnimationData.wdSpeed;
    if (flSpeed < 0.001f)
    {
        flSpeed = 0.001f;
    }






    BOOL bHugsGround = tProjectileData.dwFlags & (1 << PROJECTILE_HUGS_GROUND_BIT);
    if (bHugsGround)
    {

        NavMeshHandle tNavMeshHandle;
        Vector3D vNormal;
        ScenesCommonNavMeshNodeFromPlace(placeStart, &tNavMeshHandle, FALSE, FALSE);
        NavMeshGetHeightAndNormal(tNavMeshHandle, placeStart.wp, FALSE, placeStart.wp.z, vNormal);


        placeDest.wp.z = placeStart.wp.z;
    }

    Vector3D wvDirection;
    float flEstimatedFlightTime;

    sAimLinear(placeStart.wp, placeDest.wp, flSpeed, tProjectileData.tAimingData.wdExtraDistanceOnFirstStep, wvDirection, flEstimatedFlightTime);







    int32 nTotalFlightTime = Math::FloatToInt(flEstimatedFlightTime);

    if (tProjectileData.tAimingData.eAimMethod == AIMMETHOD_FIXED_GRAVITY)
    {
        nTotalFlightTime = (uint32)(Vector3D::Distance2D(placeStart.wp, placeDest.wp) / flSpeed);
    }

	if (wvDirection.MagnitudeSquared() < EPSILON)
	{
		return ACDID_INVALID;
	}





    ScopedActorDefinitionLockR pdefActor(tProjectileData.tRenderingData.snoPR);
    BOOL bForceYawOnly = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_PROJECTILE_FORCE_ACTOR_YAW_ONLY);
    Quaternion qOrientation;
    if (bForceYawOnly)
    {
        Angle aYaw = wvDirection.As2D().ToYaw();
        qOrientation.FromYaw(aYaw);
    }
    else
    {
        qOrientation.FromViewVector(
            wvDirection,
            cgwvZAxis,
            cgwvXAxis,
            -cgwvYAxis,
            cgwvZAxis);
    }

	ActorAddStruct tActorAdd;
	tActorAdd.snoActor = tProjectileData.tRenderingData.snoPR;
	tActorAdd.dwActorAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION) | (1 << ACTORADD_PROJECTILE) | (1 << ACTORADD_DOESNT_DEACTIVATE) | (1 << ACTORADD_FAST_MOVER);




    tActorAdd.tPlace = placeStart;
    tActorAdd.qOrientation = qOrientation;
    ScenesCommonNavMeshNodeFromPlace(placeStart, &tActorAdd.tNavMeshHandle, FALSE, FALSE);

    if (tProjectileData.dwFlags & (1 << PROJECTILE_USE_ACTOR_STATIC_FLAGS_FOR_SPAWN))
    {
        tActorAdd.dwActorAddFlags |= (1 << ACTORADD_USE_ACTOR_STATIC_FLAGS_FOR_SPAWN);
    }

    if (Math::Abs(placeStart.wp.x) >= 100000.0f ||
        Math::Abs(placeStart.wp.y) >= 100000.0f ||
        !isfinite(placeStart.wp.x) ||
        !isfinite(placeStart.wp.y))
    {
        return ACDID_INVALID;
    }









	ActorID idActor = SceneryAdd(tActorAdd);

	if (idActor == ACTORID_INVALID)
	{
		return ACDID_INVALID;
	}


	Actor *ptActor = ActorGet(idActor);

	ACDID idACDProjectile = ptActor->idACD;

	ActorProjectileGlobals *ptActorProjectileGlobals = SGameManager::GameGet().ptActorProjectileGlobals;
	ptActor->ptProjectile = ptActorProjectileGlobals->poolActorProjectiles.Alloc_Tracked();

	ActorProjectile * ptProjectile = sProjectileGet(idACDProjectile);

	ptProjectile->dwFlags = tProjectileData.dwFlags;
    ptProjectile->snoPower = tProjectileData.snoPower;
	ptProjectile->eAimMethod = tProjectileData.tAimingData.eAimMethod;
	ptProjectile->wdGravity = tProjectileData.tAimingData.wdGravity;
	ptProjectile->tDest = tProjectileData.tAimingData.tDest;
	ptProjectile->idACDSnapSource = tProjectileData.idACDSnapSource;
	ptProjectile->nSnapHardPoint = tProjectileData.nSnapHardPoint;
	ptProjectile->wpStartedFlight = placeStart.wp;
    ptProjectile->flSpeedMultiplier = 1.0f;
    ptProjectile->dwSlowUntilTick = 0;
    ptProjectile->dwStartOfSlowTick = 0;
    ptProjectile->bSmoothSlow = TRUE;
    ptProjectile->bWasReflected = FALSE;
    ptProjectile->idACDReflectedBy = ACDID_INVALID;
    ptProjectile->listACDsHit.SetPool(MemoryPoolsGetDATAIDListPool());
    ptProjectile->flUnitBounceSpeed = tProjectileData.tResponseData.flUnitBounceSpeed;
    ptProjectile->m_idSpeedResetTimer = TIMERID_INVALID;

	if (ptProjectile->dwFlags & (1 << PROJECTILE_DETONATE_AFTER_FIXED_TIME_BIT))
	{
		ProjectileSetDetonationTime(idACDProjectile, tProjectileData.tDurationData.dwTime);
	}

	ptProjectile->nTotalFlightTime = nTotalFlightTime;

	SSceneID idSScene = ACDGetCurrentSSceneID(ACDGet(idACDProjectile));
	if (idSScene == SSCENEID_INVALID)
	{
		return idACDProjectile;
	}


    Vector3D wvWalkingVelocity = wvDirection * flSpeed;

    if (!ptActor->ptSActorMovement)
    {
        return idACDProjectile;
    }



    if (tProjectileData.tAimingData.eAimMethod == AIMMETHOD_FIXED_GRAVITY)
    {
        ptActor->ptSActorMovement->MoveWithArc(placeDest,
            TAG_ACTOR_ANIMATION_NEUTRAL,
            (ActorAnimationTag)-1,
            0,
            ptProjectile->wdGravity,
            tProjectileData.tAimingData.flHeightAboveSource,
            SNO_NONE,
            tProjectileData.tResponseData.flUnitBounceSpeed);
    }

    uint32 uMoveFlags = 0;
    if (!bHugsGround)
    {
        uMoveFlags |= (1 << MOVEBIT_NO_SNAP_TO_GROUND);
    }

    if (tProjectileData.dwFlags & (1 << PROJECTILE_NAV_REFLECT))
    {
        uMoveFlags |= (1 << MOVEBIT_NAV_REFLECT);
    }

    switch (tProjectileData.eDPath) {
    case DPATH_NONE:
        if (tProjectileData.tAimingData.eAimMethod != AIMMETHOD_FIXED_GRAVITY)
        {
            ptActor->ptSActorMovement->MoveAtFixedVelocity(wvWalkingVelocity, uMoveFlags, TAG_ACTOR_ANIMATION_NEUTRAL, SNO_NONE);
        }
        break;
    case DPATH_SIN_WAVE:
    case DPATH_PEDAL_CURVE:

        ptActor->ptSActorMovement->MoveWithSinWave(
            tProjectileData.eDPath,
            wvWalkingVelocity,
            uMoveFlags,
            (ActorAnimationTag)-1,
            tProjectileData.snoPower,
            tProjectileData.tDPathSinData);
        break;

    case DPATH_SPIRAL:
    case DPATH_SPIRAL_IMMEDIATE:
        ptActor->ptSActorMovement->MoveWithSpiral(
            tProjectileData.eDPath,
            tProjectileData.tDPathSpiralData.placeSpiralCenter.wp,
            uMoveFlags,
            (ActorAnimationTag)-1,
            tProjectileData.tDPathSinData);
        break;

    case DPATH_PIE_WEDGE:
        ptActor->ptSActorMovement->MoveWithPieWedge(placeDest.wp, uMoveFlags, (ActorAnimationTag)-1,
            tProjectileData.tDPathPieData);
        break;


    default:
        if (tProjectileData.eDPath == DPATH_CURVED_OFFSET)
        {
            ptActor->ptSActorMovement->MoveWithCurvedOffset(
                wvWalkingVelocity,
                uMoveFlags,
                (ActorAnimationTag)-1,
                tProjectileData.tDPathCurvedOffsetData);
        }
        else
        {
            ptActor->ptSActorMovement->MoveWithDetPath(
                tProjectileData.eDPath,
                wvWalkingVelocity,
                uMoveFlags,
                (ActorAnimationTag)-1,
                tProjectileData.snoPower);
        }
    }
    return idACDProjectile;
}




BOOL ProjectileCanCollisionWithACD(ACDID idACDProjectile, ActorCommonData & tACDIntersected)
{
    if (ACDIgnoresProjectiles(tACDIntersected) || tACDIntersected.id == idACDProjectile)
    {
        return FALSE;
    }

    if (ACDBlocksProjectiles(tACDIntersected))
    {
        return TRUE;
    }

    if (tACDIntersected.IsAttached())
    {
        return FALSE;
    }

    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (ptACDProjectile)
    {
        ACDID idACDForcedTarget = (ACDID)ptACDProjectile->AttributesGetInt(ATTRIBUTE_PROJECTILE_REFLECT_FORCED_TARGET);
        if (idACDForcedTarget != ACDID_INVALID && idACDForcedTarget != tACDIntersected.id)
        {
            return FALSE;
        }
    }

    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    PowerID idPower = ActorPowersGetOwnerPower(idActorProjectile);
    Power * ptPower = PowerGet(idPower);
    ActorProjectile * ptProjectile = sTryToProjectileGet(idACDProjectile);

    if (ProjectileIsReflected(idACDProjectile))
    {
        ptPower->dwFlags |= (1 << PF_WAS_REFLECTED);
    }

    if (ptProjectile && tACDIntersected.IsMonster() && (ptProjectile->dwFlags & (1 << PROJECTILE_DONT_HIT_MONSTERS_BIT)))
    {
        return FALSE;
    }

    BOOL bCanTarget = PowerCanTarget(idPower, tACDIntersected.id, idACDProjectile);
    ptPower->dwFlags &= ~(1 << PF_WAS_REFLECTED);
    return bCanTarget && (!ptProjectile || !ptProjectile->listACDsHit.Find(tACDIntersected.id));
}
















































































void ProjectileSetDetonationTime(ACDID idACDProjectile, uint32 dwDetTime)
{
    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (ptACDProjectile)
    {
        uint32 dwNow = GameGetServerTime();
        ptACDProjectile->AttributesSetInt(ATTRIBUTE_PROJECTILE_DETONATE_TIME, dwNow + dwDetTime);
    }
}






BOOL ProjectileTestCollisionAlongLine(
    ACDID idACDProjectile,
    const WorldPlace & placeStart,
    const WorldPlace & placeDest,
    ACDID & idACDHitInCollision,
    float & flUnitDistanceTraveled)
{


    flUnitDistanceTraveled = 1.0f;

	ActorProjectile * ptProjectile = sProjectileGet(idACDProjectile);
	if (ptProjectile->dwFlags & (1 << PROJECTILE_DONT_HIT_ACDS_BIT))
	{
		return FALSE;
	}

    ACDCollSettings tCollSettings;
    ActorCommonData & tACD = ACDGet(idACDProjectile);
    tCollSettings = tACD.tCollSettings;
    if (tCollSettings.eCollisionShape != COLLISION_SHAPE_CYLINDER)
    {
        return FALSE;
    }


    if (tACD.tLocation.eActorLocation != AL_WORLD)
    {
        return FALSE;
    }



    if (placeStart.idSWorld != placeDest.idSWorld)
    {
        return FALSE;
    }

    static const float flCollHeight = 8.0f;
    static const float flCollHalfHeight = 4.0f;  // value from the fmaxnm constant; the spelling (literal vs flCollHeight / 2) is inferred
    const AxialCylinder & tCyl = *(const AxialCylinder *)tCollSettings.arAxialCylinderBytes;  // no GetCyl inline instance in this CU's DWARF; same form as SActorMovement.cpp:844
    float flRadiusNormal = tCyl.wdRadius;
    float flRadiusSecondary = tACD.tLocation.tWorldLocation.wcylBounds.wdRadius;
    float flRadiusMonster = Math::Max(flRadiusNormal, flRadiusSecondary);
    float flRadiusBroad = Math::Max(flRadiusMonster, flCollHalfHeight);


    Capsule tCapsuleBroad;
    tCapsuleBroad.wp1 = placeStart.wp;

    tCapsuleBroad.wp2 = placeDest.wp;

    tCapsuleBroad.flRadius = flRadiusBroad;

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    ACDsIntersectCapsule(placeStart.idSWorld, tCapsuleBroad, listTargets, FALSE);

	idACDHitInCollision = ACDID_INVALID;
    float flTWinner = MAX_WORLDDISTANCE;

    Vector3D vToDest = placeDest.wp - placeStart.wp;
    Vector3D vDir = vToDest;
    float flLength = 0.0f;
    if (vToDest.MagnitudeSquared() > EPSILON * EPSILON)  // inferred spelling: the constant is 1e-12f
    {

        flLength = vDir.Normalize();
    }
    else
    {
        vDir = cgwvXAxis;
    }


	POS pos = listTargets.GetHeadPosition();
	while (pos)
	{
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData & tACDTarget = ACDGet(idACDTarget);
        if (!ProjectileCanCollisionWithACD(idACDProjectile, tACDTarget))
        {
            continue;
        }

        const WorldLocation & tLocation = tACDTarget.GetWorldLocation();
        const Vector3D & vTarget = tLocation.transform.wp;
        Vector3D vToTarget = vTarget - placeStart.wp;


        Vector2D vDir2D = vDir.As2D();
        vDir2D.Normalize();

        float flProj2D = Vector2D::DotProduct(vToTarget.As2D(), vDir2D);
        if (flProj2D >= flTWinner)
        {
            continue;
        }


        float flRadiusCombined = tACDTarget.GetRadius() + (
            tACDTarget.IsMonster() ? flRadiusMonster : flRadiusNormal);








        if (flProj2D < -flRadiusCombined)
        {
            continue;
        }


        if (flProj2D > flLength + flRadiusCombined)
        {
            continue;
        }


        Vector2D vProjPoint2D = vDir2D * flProj2D;
        vProjPoint2D += placeStart.wp.As2D();


        float flRadiusCombinedSqr = flRadiusCombined * flRadiusCombined;
        if (flProj2D <= 0.0f)
        {
            if (Vector2D::DistanceSquared(placeStart.wp.As2D(), vTarget.As2D()) > flRadiusCombinedSqr)
            {
                continue;
            }
        }
        else if (flProj2D >= flLength)
        {
            if (Vector2D::DistanceSquared(placeDest.wp.As2D(), vTarget.As2D()) > flRadiusCombinedSqr)
            {
                continue;
            }
        }
        else
        {
            if (Vector2D::DistanceSquared(vProjPoint2D, vTarget.As2D()) > flRadiusCombinedSqr)
            {
                continue;
            }
        }


        float flT = Vector3D::DotProduct(vDir, vToTarget);
        Vector3D vProjPoint = vDir * flT;
        vProjPoint += placeStart.wp;

        if (vTarget.z + tCyl.wdHeight < vProjPoint.z - flCollHeight)
        {
            continue;
        }

        if (vTarget.z > vProjPoint.z + tLocation.wcylBounds.wdHeight)
        {
            continue;
        }

        idACDHitInCollision = idACDTarget;
        flTWinner = flProj2D;
	}

    if (flLength > EPSILON)
    {
        float flCollideLength = Math::Min(flTWinner, flLength);
        flUnitDistanceTraveled = flCollideLength / flLength;
        flUnitDistanceTraveled = Math::Pin(flUnitDistanceTraveled, 0.0f, 1.0f);
    }

	if (idACDHitInCollision != ACDID_INVALID)
	{
		return TRUE;
	}
	// inferred: the FALSE return has no line-table row (cset at 841:26)
	return FALSE;



















































































}


void sProjectileResetSpeed(TimerID, DataID idProjectile, void *)
{
    Actor * ptActor = ActorTryToGet((ActorID)idProjectile);
    if (!ptActor)
    {
        return;
    }

    ActorProjectile * ptProjectile = sTryToProjectileGet(ptActor->idACD);
    if (ptProjectile)
    {
        ptProjectile->m_idSpeedResetTimer = -1;
        ProjectileSetSpeedMultiplier(ptActor->idACD, 1.0f, -1, TRUE, TRUE, TRUE);
    }
}










void ProjectileSetSpeedMultiplier(
    ACDID idACDProjectile,
    float flSpeedMultiplier,
    int32 nDuration,
    BOOL bAdjustDetonationTimes,
    BOOL bSmoothSlow,
    BOOL bForce)
{
    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (!ptACDProjectile)
    {
        return;
    }

    if (!bForce)
    {
        ScopedActorDefinitionLockR pdefActor(ptACDProjectile->snoActor);
        BOOL bCanBeSlowed = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_PROJECTILE_CAN_BE_SLOWED);
        if (!bCanBeSlowed)
        {
            return;
        }
    }

    ActorProjectile * ptProjectile = sProjectileGet(idACDProjectile);


    uint32 dwNow = GameGetServerTime();


    if (ptProjectile->eAimMethod != AIMMETHOD_FIXED_GRAVITY && bAdjustDetonationTimes)
    {
        if (ptProjectile->flSpeedMultiplier > flSpeedMultiplier)
        {

            const float flMaxDurationMultiplier = 10.0f;
            float flDurationMultiplier = flMaxDurationMultiplier;

            if (flSpeedMultiplier > EPSILON)
            {
                flDurationMultiplier = Math::Min(flMaxDurationMultiplier, ptProjectile->flSpeedMultiplier / flSpeedMultiplier);
            }

            if ((ptProjectile->dwFlags & (1 << PROJECTILE_DETONATE_AFTER_FIXED_TIME_BIT)) != 0)
            {

                ProjectileSetDetonationTime(idACDProjectile, flDurationMultiplier * ProjectileGetTicksTillDetonate(idACDProjectile));
            }
        }
        else if (flSpeedMultiplier > ptProjectile->flSpeedMultiplier)
        {
            float flDurationMultiplier = 1.0f;
            if (flSpeedMultiplier > EPSILON)
            {
                flDurationMultiplier = Math::Min(1.0f, ptProjectile->flSpeedMultiplier / flSpeedMultiplier);
            }

            if ((ptProjectile->dwFlags & (1 << PROJECTILE_DETONATE_AFTER_FIXED_TIME_BIT)) != 0)
            {
                int32 nTimeTillFixedTime = ProjectileGetTicksTillDetonate(idACDProjectile);
                ProjectileSetDetonationTime(idACDProjectile, flDurationMultiplier * nTimeTillFixedTime);
            }
        }
    }


    ptProjectile->flSpeedMultiplier = flSpeedMultiplier;

    if (dwNow >= ptProjectile->dwSlowUntilTick)
    {
        ptProjectile->dwStartOfSlowTick = dwNow;
    }

    if (TimerExists(ptProjectile->m_idSpeedResetTimer))
    {
        TimerUnregister(ptProjectile->m_idSpeedResetTimer);
        ptProjectile->m_idSpeedResetTimer = -1;
    }

    if (nDuration > 0)
    {
        ptProjectile->m_idSpeedResetTimer = TimerRegister(ACDGetOwner(idACDProjectile), TIMER_OWNER_ACTOR, nDuration, NULL, sProjectileResetSpeed);
    }


    ptProjectile->dwSlowUntilTick = dwNow + nDuration;

    ptProjectile->bSmoothSlow = bSmoothSlow;
}




float ProjectileGetSpeedMultiplier(
    ACDID idACDProjectile)
{
    ActorProjectile * ptProjectile = sProjectileGet(idACDProjectile);

    if (ptProjectile->flSpeedMultiplier > 1e-06f)
    {
        uint32 dwNow = GameGetServerTime();
        if (dwNow < ptProjectile->dwSlowUntilTick)
        {

            if (ptProjectile->bSmoothSlow)
            {
                const int32 nRampUpTime = 15;
                uint32 dwLengthOfSlow = dwNow - ptProjectile->dwStartOfSlowTick;
                return 1.0f + (ptProjectile->flSpeedMultiplier - 1.0f) * Math::Min(1.0f, (float)dwLengthOfSlow / nRampUpTime);
            }
            else
            {
                return 1.0f + (ptProjectile->flSpeedMultiplier - 1.0f);
            }
        }
    }

    return 1.0f;
}



const ActorCommonData * ProjectileGetOwnerACD(ACDID idACDProjectile)
{
    PowerID idPower = ActorPowersGetOwnerPower((ActorID)ACDGetOwner(idACDProjectile));
    return PowerGetOwner(idPower);
}






void ProjectileSetReflected(ACDID idACDProjectile, ACDID idACDReflectedBy, BOOL bWasReflected)
{
    ActorProjectile * ptProjectile = sProjectileGet(idACDProjectile);
    ptProjectile->bWasReflected = bWasReflected;
    ptProjectile->idACDReflectedBy = idACDReflectedBy;
}








BOOL ProjectileIsReflected(ACDID idACDProjectile)
{
    ActorProjectile * ptProjectile = sProjectileGet(idACDProjectile);
    return ptProjectile->bWasReflected;
}






ACDID ProjectileGetReflectedByACD(ACDID idACDProjectile)
{
    ActorProjectile * ptProjectile = sProjectileGet(idACDProjectile);
    return ptProjectile->idACDReflectedBy;
}






BOOL ProjectileReflect(
    ACDID idACDProjectile,
    ACDID idACDReflector,
    BOOL bReflectYawOnly,
    Angle aYawOffset,
    Team eTeam,
    float flSpeedMult)
{
    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    ActorCommonData * ptACDReflector = ACDTryToGet(idACDReflector);
    if (!ptACDProjectile || !ptACDReflector)
    {
        return FALSE;
    }

    ScopedActorDefinitionLockR pdefActor(ptACDProjectile->snoActor);
    BOOL bCanBeReflected = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_PROJECTILE_CAN_BE_REFLECTED);
    if (!bCanBeReflected)
    {
        return FALSE;
    }

    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    Actor * ptActorProjectile = ActorTryToGet(idActorProjectile);
    if (!ptActorProjectile ||
        !ptActorProjectile->ptSActorMovement ||
        !ptActorProjectile->ptSActorMovement->IsMoving())
    {
        return FALSE;
    }

    ProjectileSetReflected(idACDProjectile, idACDReflector, TRUE);


    ActorProjectile * ptProjectile = sProjectileGet(idACDProjectile);

    if (TimerExists(ptProjectile->m_idSpeedResetTimer))
    {
        TimerUnregister(ptProjectile->m_idSpeedResetTimer);
        ptProjectile->m_idSpeedResetTimer = TIMERID_INVALID;
        ProjectileSetSpeedMultiplier(idACDProjectile, 1.0f, -1, TRUE, TRUE, FALSE);
    }


    if (eTeam != TEAM_INVALID)
    {
        ptACDProjectile->SetTeam(eTeam);
    }
    else
    {

        ptACDProjectile->SetTeam(ptACDReflector->GetTeam());
    }




    {
        SActorMovement * ptMove = ptActorProjectile->ptSActorMovement;
        Vector3D wvVelocity = ptMove->GetVelocity();
        BOOL bIsArcing = ptMove->IsInArcingMovement();
        if (!bIsArcing)
        {
            ptMove->StopNow();
        }
        Angle aYawDelta = Math::NormalizeAngle(aYawOffset + PI);
        ptMove->SetNewFacing(Math::NormalizeAngle(aYawDelta + ptActorProjectile->aYaw), FALSE, TRUE);

        if (bIsArcing || bReflectYawOnly)
        {
            Vector3D::Rotate(cgwvZAxis, aYawDelta, wvVelocity);
        }
        else
        {
            wvVelocity = -wvVelocity;
            Vector3D::Rotate(cgwvZAxis, aYawOffset, wvVelocity);
        }

        if (flSpeedMult > EPSILON && !bIsArcing)
        {
            wvVelocity *= flSpeedMult;
        }

        ActorUpdateOrientation(idActorProjectile, TRUE);

        if (bIsArcing)
        {
            ptMove->ReflectArcingProjectile(wvVelocity);
        }
        else
        {
            uint32 uMoveFlags = (1 << MOVEBIT_NO_SNAP_TO_GROUND);
            ptMove->MoveAtFixedVelocity(wvVelocity, uMoveFlags, TAG_ACTOR_ANIMATION_NEUTRAL, SNO_NONE);
        }
    }

    return TRUE;
}



void ProjectileAdjustFacing(ACDID idACDProjectile, const Vector3D wvNewFacing, BOOL bAdjustYawOnly)
{
    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    Actor * ptActorProjectile = ActorTryToGet(idActorProjectile);
    if (ptActorProjectile)
    {
        Angle aNewYaw = ((const Vector2D &)wvNewFacing).ToYaw();
        SActorMovement * ptMove = ptActorProjectile->ptSActorMovement;
        if (ptMove->GetMovementType() == MOVEMENT_TYPE_DETPATH)
        {
            ptMove->DetPathAdjustFacing(aNewYaw);
            return;
        }
        Vector3D wvVelocity = ptMove->GetBaseVelocity();
        Angle aYawDelta = Math::NormalizeAngle(aNewYaw - ptActorProjectile->aYaw);
        if (bAdjustYawOnly)
        {
            Vector3D::Rotate(cgwvZAxis, aYawDelta, wvVelocity);
        }
        else
        {
            wvVelocity = wvNewFacing * wvVelocity.Magnitude();
        }
        ScopedActorDefinitionLockR pdefActor(ptActorProjectile->snoActor);
        BOOL bForceYawOnly = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_PROJECTILE_FORCE_ACTOR_YAW_ONLY);
        Quaternion qNewFacing;
        if (bForceYawOnly)
        {
            qNewFacing.FromYaw(aNewYaw);
        }
        else
        {
            qNewFacing.FromViewVector(wvNewFacing, cgwvZAxis, cgwvXAxis, -cgwvYAxis, cgwvZAxis);
        }
        ActorSetOrientation(idActorProjectile, qNewFacing, FALSE);
        ptMove->ChangeFixedVelocity(wvVelocity);
    }
}










































BOOL ProjectileIsLobbed(ACDID idACDProjectile)
{
    ActorProjectile * ptProjectile = sTryToProjectileGet(idACDProjectile);
    if (!ptProjectile)
    {
        return FALSE;
    }
    return ptProjectile->eAimMethod == AIMMETHOD_FIXED_GRAVITY;
}
















































BOOL ProjectileDestroyOnImpact(ACDID idACDProjectile)
{
    ActorProjectile * ptProjectile = sProjectileGet(idACDProjectile);
    return !(ptProjectile->dwFlags & (1 << PROJECTILE_DONT_DESTROY_ON_IMPACT));
}




void ProjectileSeekPlace(
    ACDID idACDProjectile,
    WorldPlace tPlace,
    float flMaxAngleToRotate,
    BOOL bYawOnly)
{
    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (!ptACDProjectile)
    {
        return;
    }

    if (ptACDProjectile->GetPlace().idSWorld != tPlace.idSWorld)
    {
        return;
    }

    if (Vector3D::Distance2DSquared(ptACDProjectile->GetPlace().wp, tPlace.wp) < EPSILON)
    {
        return;
    }

	ActorMovement *ptProjectileMovement = ptACDProjectile->GetActorMovement();
	if (ptProjectileMovement && !(ptProjectileMovement->m_uMoveFlags & (1 << MOVEBIT_NO_SNAP_TO_GROUND)))
	{
		ptProjectileMovement->SnapPlaceToGround(tPlace);
	}

    SActorMovement *ptMove = (SActorMovement *)ptACDProjectile->GetActorMovement();
    if (!ptMove)
    {
        return;
    }

    if (ProjectileIsReflected(idACDProjectile))
    {
        return;
    }

    Vector3D wvFacing = ptMove->GetVelocity();
    if (wvFacing.MagnitudeSquared() < EPSILON * EPSILON)  // inferred spelling: the constant is 1e-12f, EPSILON (MathGeneral.h:11) squared
    {
        wvFacing = cgwvXAxis;
    }
    else
    {
        wvFacing.Normalize();
    }
    Vector3D wvToTarget;
    WorldPlace placeInterp = ptACDProjectile->GetPlace();
    ptMove->GetCurrentInterpolatedPos(placeInterp.wp);
    WorldPlaceGetVectorBetween(placeInterp, tPlace, wvToTarget);
    Vector3D wvNewFacing = wvToTarget;
    if (wvToTarget.MagnitudeSquared() < EPSILON * EPSILON ||
        (!isfinite(wvNewFacing.x) ||
         (!isfinite(wvNewFacing.y) ||
          !isfinite(wvNewFacing.z))))
    {
        wvNewFacing = cgwvXAxis;
    }
    else
    {
        wvNewFacing.Normalize();
    }

    Vector3D wvRotateAbout;
    if (bYawOnly)
    {

        wvToTarget.z = wvFacing.z = 0.0f;
        // inferred: the original loads cgwvZAxis once at line 1452 for both paths (tail-merged)
        wvRotateAbout = cgwvZAxis;
    }
    else
    {
        wvRotateAbout = Vector3D::CrossProduct(wvFacing, wvToTarget);
        if (wvRotateAbout.MagnitudeSquared() < EPSILON * EPSILON)
        {
            wvRotateAbout = cgwvZAxis;
        }
        else
        {
            wvRotateAbout.Normalize();
        }
    }

    if (!wvRotateAbout.IsNormalized())
    {
        return;
    }

    Angle aDifference = Vector3D::AngleBetween(wvFacing, wvToTarget);
    if (aDifference > PI)
    {
        aDifference = aDifference - FULL_CIRCLE;
    }

    Angle aMaxAngleToRotate = Math::DegToAngle(flMaxAngleToRotate);
    if (Math::Abs(aDifference) > aMaxAngleToRotate)
    {
        Quaternion qRot;
        if (bYawOnly && aDifference < 0.0f)
        {
            qRot.SetRotation(wvRotateAbout, -aMaxAngleToRotate);
        }
        else
        {
            qRot.SetRotation(wvRotateAbout, aMaxAngleToRotate);
        }
        wvNewFacing = qRot.Rotate(wvFacing);
    }

    ProjectileAdjustFacing(idACDProjectile, wvNewFacing, bYawOnly);
}


void ProjectileSeekActor(ACDID idACDProjectile, ACDID idACDTarget, float flMaxAngleToRotate, BOOL bYawOnly)
{
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget)
    {
        ProjectileSeekPlace(idACDProjectile, ptACDTarget->GetWorldPlaceCenter(), flMaxAngleToRotate, bYawOnly);
    }
}






void ProjectileClearHitList(ACDID idACDProjectile)
{
    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    Actor * ptActorProjectile = ActorGet(idActorProjectile);
    if (ptActorProjectile && ptActorProjectile->ptProjectile)
    {
        ptActorProjectile->ptProjectile->listACDsHit.RemoveAll();
    }
}













void ProjectileAddActorToHitList(
    ACDID idACDProjectile,
    ACDID idACDActor)
{
    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    Actor * ptActorProjectile = ActorGet(idActorProjectile);
    ActorCommonData * ptActor = ACDTryToGet(idACDActor);
    if (!ptActorProjectile)
    {
        return;
    }


    if (!ptActorProjectile->ptProjectile)
    {
        return;
    }


    if (!ptActor)
    {
        return;
    }


    if (!ptActorProjectile->ptProjectile->listACDsHit.Find(idACDActor))
    {
        ptActorProjectile->ptProjectile->listACDsHit.AddTail_Tracked(idACDActor);
    }
}


BOOL ProjectileActorIsOnHitList(
    ACDID idACDProjectile,
    ACDID idACDActor)
{
    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    Actor * ptActorProjectile = ActorGet(idActorProjectile);
    ActorCommonData * ptActor = ACDTryToGet(idACDActor);

    if (!ptActorProjectile)
    {
        return FALSE;
    }


    if (!ptActorProjectile->ptProjectile)
    {
        return FALSE;
    }


    if (!ptActor)
    {
        return FALSE;
    }


    return ptActorProjectile->ptProjectile->listACDsHit.Find(idACDActor) != NULL;




}


int32 ProjectileActorGetHitListCount(ACDID idACDProjectile)
{
    Actor * ptActorProjectile = ActorGet((ActorID)ACDGetOwner(idACDProjectile));
    if (ptActorProjectile && ptActorProjectile->ptProjectile)
    {
        return ptActorProjectile->ptProjectile->listACDsHit.Count();
    }
    return 0;
}
