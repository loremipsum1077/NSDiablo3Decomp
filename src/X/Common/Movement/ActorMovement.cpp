// Original file: X/Common/Movement/ActorMovement.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include <math.h>
#include "../ActorCommonData.h"
#include "../Game.h"
#include "../SceneCommon.h"
#include "../Player.h"
#include "../Nav/NavZone.h"
#include "../Definitions/ActorDefinition.h"
#include "../Marker/SmartMarker.h"
#include "../ActorCommonCollision.h"
#include "../WorldCommonCollision.h"
#include "../../Server/Powers/PowerAPI.h"
#include "../../Server/Actors/Projectile.h"
#include "../Powers/PowersCommon.h"
#include "../GlobalSNO.h"
#include "../TagMaps/Tags.h"
#include "../TagMaps/ActorTags.h"
#include "../DetPaths.h"
#include "../../Server/SGame.h"
#include "ActorMovement.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/MemoryPools.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../GameConstants.h"  // OO_TICKS_PER_SECOND (UpdateWalking)
int32 ProjectileGetTicksTillDetonate(ACDID idACDProjectile);  // X/Common/Actors/ProjectileCommon.cpp:30
void ACDCollGetSettings(SNO snoActor, ACDCollSettings & tCollSettings);  // X/Common/ActorCommonCollision.cpp
BOOL ProjectileValidateMaxDuration(ACDID idACDProjectile);  // X/Server/Actors/Projectile.cpp
BOOL PowerGetCollFlags(AutoPowerDefinitionR pdefPower, PowerPassabilityType ePowerPassabilityType, ActorCollisionFlags & tCollFlags);  // X/Common/Powers/PowersCommon.cpp
BOOL NavZoneCollideSweptPoint3D(SWorldID idSWorld, const Vector3D & wpStart, const Vector3D & wvDirection, uint16 uOneRequiredFlag, uint16 uPreventFlags, uint32 uCollFlags, float & flISectT, Vector3D & wpCollision);  // X/Common/Nav/NavZoneIntersect.cpp:2173
BOOL WorldCollSweptCircle(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Circle & wcStart, const Vector2D & wpDest, const WorldCollParams & tWorldCollParams, uint32 uCollTestFlags, uint32 uACDCollNarrowFlags, ACDID idACDIgnore, WorldCollResult & tResult);  // X/Common/WorldCommonCollision.cpp:147
void DetPathStart(ActorMovement & tMove, DeterministicPathData & tDPath);  // X/Common/DetPaths.cpp:1108
#include "../../Sigma/Math/Ray.h"  // Ray2D (TargetWillBeReached)
#include "../../Sigma/Math/Intersect.h"  // Intersect::Ray2DAndCircle (TargetWillBeReached)
// Declarations the original gets from headers not yet adopted.
BOOL ACDCollBroadCircle(SWorldID idSWorld, const Vector2D & wpCenter, float flRadius, const ACDCollParams & tACDCollParams, int32 nFlags, ACDCollBroadResult & tResult, ACDID idACDIgnore);  // X/Common/ActorCommonCollision.cpp
BOOL ACDCollNarrowSweptCircle(const ACDCollBroadResult & tBroadResult, const Circle & wcStart, const Vector2D & wpDest, uint32 uFlags, ACDID idACDSelf, ACDCollNarrowResult & tResult);  // X/Common/ActorCommonCollision.cpp
#include "../Nav/NavZoneIntersect.h"
#include "../Nav/NavCellPath.h"
BOOL NavZoneCollideSweptCircle(const NavCellHandle & hNavCellStart, const Circle & wcStart, const Vector2D & wpDest, uint16 uOneRequiredFlags, uint16 uPreventFlags, uint32 uCollFlags, NavCellCollideResult & tResult);  // X/Common/Nav/NavZoneIntersect.cpp
BOOL NavZoneCollideSweptPoint(const NavCellHandle & hNavCellStart, const Vector2D & wpStart, const Vector2D & wpDest, uint16 uOneRequiredFlags, uint16 uPreventFlags, uint32 uCollFlags, NavCellCollideResult & tResult);  // X/Common/Nav/NavZoneIntersect.cpp
BOOL ProjectileCanCollisionWithACD(ACDID idACDProjectile, ActorCommonData & tACDIntersected);  // X/Server/Actors/Projectile.cpp:526
uint32 DetPathGetUpdateInterval(DeterministicPathType eDType);  // X/Common/DetPaths.cpp:1204
BOOL DetPathUpdate(ActorMovement & tMove, Vector3D & vVel, DeterministicPathData & tDPath, float flUpdateTime);  // X/Common/DetPaths.cpp:1150
BOOL DetPathProcessNavReflect(ActorMovement & tMove, DeterministicPathData & tDPath, const WorldCollResult & tCollisionResult);  // X/Common/DetPaths.cpp:1321
#include "../../Client/Gfx/GfxVB.h"  // was reached through SmartMarker.h:18 (the original SmartMarker.h does not include it)
















const float flOOSecondsToUnStuck = 0.4f;








ActorMovement::~ActorMovement()
{
}



BOOL ActorMovement::Initialize(const WorldPlace & placeStart)

{
	m_nCurrCell = -1;

	ACDID idACD = GetACDID();
	m_uUpdateInterval = ACDIsPlayer(idACD) ? 12 : 12;





	m_flDesiredSpeed = 0.0f;
	m_flCurrentSpeed = 0.0f;

	m_flAccelMag = 1.0f;

	m_flMaxRangeFromDest = 0.0f;
	m_flUnitBounce = 0.0f;
	m_eAnimTag = -1;
	m_eLandingAnimTag = -1;
	m_uMoveFlags = 0;
	m_idACDTarget = ACDID_INVALID;
	m_uMinTicksToAllowTravel = 0;
	m_eMovementType = MOVEMENT_TYPE_NONE;
	m_flGravity = 0.0f;
	m_flDesiredHeight = 0.0f;
	m_vVel = cgwvNull;
	m_vLastVel = cgwvNull;
	m_snoPowerPassability = -1;
	m_eSnapState = SNAP_NONE;

	m_uNextUpdateTick = GetTimeNow();

	m_lastFindSource = cgplaceNull;
	m_lastFindTarget = cgplaceNull;
	m_uLastFindTime = 0;

	NavCellHandleFromWorldPlace(placeStart, m_hNavCell);

	if (m_hNavCell.idSScene != SSCENEID_INVALID)
	{
		OnSceneChanged(SSCENEID_INVALID, m_hNavCell.idSScene);
	}

	m_vCurrPathDest = cgwvNull;
	m_vCurrSteerVel = cgwvNull;
	m_placeDest = cgplaceNull;

	m_placeStart = cgplaceNull;
	m_vInitialVelocity = cgwvNull;

	m_tSteer.Initialize(this);

	m_flMinDistToWalk = 0.0f;
	m_flMaxPathfindingDist = 0.0f;

	SigmaMemoryClear(&m_tDPath, sizeof(m_tDPath));
	m_tDPath.eDPath = DPATH_NONE;

	m_placeLastStamp = cgplaceNull;
	m_uLastStampTime = 0;
	m_flLastStampSpeed = 0.0f;
	m_uMinDurationEndStampTime = 0;
	m_bWaitAtDestination = FALSE;

	m_nMaxTicksToWalk = -1;

	m_pfnCallback = NULL;

	SNO snoActor = GetActorSNO();
	m_flRadiusScalar = 1.0f;
	if (snoActor != -1)
	{
		ScopedActorDefinitionLockR pdefActor(snoActor);
		m_flRadiusScalar = pdefActor->tCollData.flMovingRadiusScalar;
	}

	m_uPathfindThrottleTicks = 20;
	m_flPathfindThrottleRadius = 1.0f;
	ActorCommonData * ptACD = ACDTryToGet(idACD);
	if (ptACD && ptACD->IsMonster() && ptACD->IsMonsterTeam() && ptACD->eMonsterRarity == 0)
	{
		m_uPathfindThrottleTicks = 60;
		m_flPathfindThrottleRadius = 3.0f;
	}

	return TRUE;
}



void ActorMovement::Dispose()
{
	SSceneID idSScene = GetSSceneID();
	if (SceneCommonDataTryToGet(idSScene))
	{
		OnSceneChanged(idSScene, SSCENEID_INVALID);
	}
}




void ActorMovement::MoveTo(
    const WorldPlace & placeDest,
    float flSpeed,
    float flAccel,
    ActorAnimationTag eAnimTag,
    uint32 uMoveFlags,
    float flMaxRangeFromDest,
    ACDID idACDTarget,
    SNO snoPowerPassability,
    uint32 dwWalkInPlaceTicks,
    int32 nMaxTicksToWalk,
    float flMaxPathfindingDist,
    ActorMoveDoneCallback pfnCallback)
{
    WorldPlace placeCurr;
    GetPlace(placeCurr);

    m_placeDest = placeDest;
    m_placeStart = cgplaceNull;
    m_vInitialVelocity = cgwvNull;
    m_vCurrPathDest = cgwvNull;
    m_vCurrSteerVel = cgwvNull;
    m_flDesiredSpeed = flSpeed;
    m_flMaxRangeFromDest = Math::Max(0.0f, flMaxRangeFromDest);
    m_flUnitBounce = 0.0f;

    m_uMoveFlags = uMoveFlags;
    m_idACDTarget = idACDTarget;
    m_eAnimTag = eAnimTag;
    m_eMovementType = MOVEMENT_TYPE_WALKING;
    m_eLastDynPathDirection = DYNPATH_DIRECTION_NONE;
    m_bFailedToFindDynamicPath = FALSE;
    m_snoPowerPassability = snoPowerPassability;
    m_pfnCallback = pfnCallback;
    m_flGravity = 0.0f;	m_vLastVel = cgwvNull;
    m_nMaxTicksToWalk = nMaxTicksToWalk;
    m_flMaxPathfindingDist = flMaxPathfindingDist;

    if (dwWalkInPlaceTicks)
    {
        m_uMinDurationEndStampTime = GetTimeNow() + dwWalkInPlaceTicks;
        m_bWaitAtDestination = TRUE;
    }
    else
    {
        m_uMinDurationEndStampTime = 0;
        m_bWaitAtDestination = FALSE;
    }
    // inference: no row of its own; 237:22 is the < of a test on m_flAccelMag, so the parameter is stored first
    m_flAccelMag = flAccel;
    if (m_flAccelMag < EPSILON)
    {
        m_flAccelMag = 10000.0f;
    }

    if (m_uMoveFlags & (1 << MOVEBIT_NO_SNAP_TO_GROUND))
    {
        Vector3D vDelta = placeDest.wp - placeCurr.wp;
        vDelta.z = 0.0f;
        float flMagSqr = vDelta.MagnitudeSquared();
        if (flMagSqr > EPSILON * EPSILON && flSpeed > EPSILON)
        {
            float flTotalDist = Math::Sqrt(flMagSqr);
            float flEstimatedTicks = flTotalDist / flSpeed;
            float flDeltaZ = placeDest.wp.z - placeCurr.wp.z;
            m_vVel.z = flDeltaZ / flEstimatedTicks;
        }
    }


    if (m_nMaxTicksToWalk > 0 && (m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT)))
    {

        GetPlace(placeCurr);

        Vector3D vDelta = m_placeDest.wp - placeCurr.wp;
        float flDesiredSpeedDist = m_flDesiredSpeed * (float)m_nMaxTicksToWalk;

        if (placeCurr.idSWorld == m_placeDest.idSWorld &&
            vDelta.MagnitudeSquared() > flDesiredSpeedDist * flDesiredSpeedDist)
        {
            m_flDesiredSpeed = vDelta.Magnitude() / (float)m_nMaxTicksToWalk;
        }
    }

















    QueueUpdate(GetEarliestUpdateTickOffset());




}



BOOL ActorMovement::Update()

{

	// NON_MATCHING: 4 bytes over; the not-moving early return is placed out of line











	if (m_eMovementType != MOVEMENT_TYPE_FIXED_PROJECTILE &&
		m_eMovementType != MOVEMENT_TYPE_FIXED_PROJECTILE_COLLIDING &&
		m_eMovementType != MOVEMENT_TYPE_ARC)
	{

		const NavZoneInstance * ptZoneInstance = NavCellHandleToZoneInstance(m_hNavCell);
		if (!ptZoneInstance && IsMoving())
		{
			return TRUE;
		}
	}


	switch (m_eMovementType)
	{
	case MOVEMENT_TYPE_WALKING:
		if (m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT))
		{
			UpdateWalkingRemote();
		}
		else
		{
			UpdateWalking();
		}
		break;


	case MOVEMENT_TYPE_WALKING_IN_PLACE:
		UpdateWalkingInPlace();
		break;



	case MOVEMENT_TYPE_WALK_FINISHING:
		StopNow();
		break;


	case MOVEMENT_TYPE_FIXED_PROJECTILE:
		if (!ProjectileValidateMaxDuration(GetACDID()))
		{
			return TRUE;
		}

		UpdateFixed();
		break;


	case MOVEMENT_TYPE_FIXED_PROJECTILE_COLLIDING:
		if (!ProjectileValidateMaxDuration(GetACDID()))
		{
			return TRUE;
		}

		ProjCollisionResponse(m_idACDTarget);
		break;


	case MOVEMENT_TYPE_STOPPED_PROJECTILE:
		if (!ProjectileValidateMaxDuration(GetACDID()))
		{
			return TRUE;
		}

		UpdateStoppedProjectile();
		break;


	case MOVEMENT_TYPE_ARC:
		if (!ProjectileValidateMaxDuration(GetACDID()))
		{
			return TRUE;
		}

		UpdateArc();
		break;




	case MOVEMENT_TYPE_NONE:

		UpdateFacing();
		break;



	case MOVEMENT_TYPE_CLIENT_AUTH:
		ArrivingAtDest(0, FALSE, FALSE, ACDID_INVALID);
		m_eMovementType = MOVEMENT_TYPE_NONE;
		break;



	case MOVEMENT_TYPE_CLIENT_AUTH_FACING:
		UpdateClientFacing();
		break;


	case MOVEMENT_TYPE_DETPATH:
		if ((m_tDPath.eDPath != DPATH_SPIRAL_IMMEDIATE && m_tDPath.eDPath != DPATH_SPIRAL) ||
			Math::Abs(m_tDPath.tSinData.flSpeed) >= EPSILON)
		{	if (!ProjectileValidateMaxDuration(GetACDID()))
			{
				return TRUE;
			}
		}
		UpdateDetPath();
		break;

	default:
		DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
		break;
	}

	if (m_uMoveFlags & (1 << MOVEBIT_IS_PROJECTILE))
	{
		SNO snoActor = GetActorSNO();
		if (snoActor != -1)
		{
			ScopedActorDefinitionLockR pdefActor(snoActor);
			if (pdefActor &&
				TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_PROJECTILE_MATCH_YAW_TO_VELOCITY))
			{
				Angle aYaw = Math::ATan(m_vVel.y, m_vVel.x);
				SetNewFacing(aYaw, TRUE, FALSE);
			}
		}
	}

	return FALSE;
}






static void sCapVelocityToReasonableValue(Vector3D & vVel)
{
	// Cap the velocity to something reasonable.
	if (vVel.MagnitudeSquared() > 50.0f * 50.0f)
	{
		vVel.Normalize();
		vVel *= 50.0f;
	}
}





void ActorMovement::UpdateWalking()

{








    m_uMoveFlags &= ~(1 << MOVEBIT_WAS_STUCK);

    WorldPlace placeCurr;
    float flRadius;
    GetPlaceAndRadius(placeCurr, flRadius, TRUE);

    if (placeCurr.idSWorld != m_placeDest.idSWorld)
    {
        StopNow();
        return;
    }

    m_placeLastStamp = placeCurr;
    m_uLastStampTime = GetTimeNow();
    m_flLastStampSpeed = m_flCurrentSpeed;

    ACDID idACD = GetACDID();
    ActorCommonData *ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        if (m_uMoveFlags & (1 << MOVEBIT_USE_WALKING_SPEED))
        {
            m_flDesiredSpeed = ptACD->GetWalkingSpeed(m_eAnimTag);
        }
    }








    m_vCurrPathDest = m_placeDest.wp;

    BOOL bLastLeg = TRUE;
    if (!(m_uMoveFlags & (1 << MOVEBIT_NO_PATHFINDING)))
    {




        PathPoint2DList listPathPoints;
        listPathPoints.SetPool(NavCellPathGetPointPool());

        ACDCollSettings tCollSettings;
        GetCollSettings(tCollSettings);



        BOOL fDoPathFind = FALSE;
        if ((placeCurr.idSWorld != m_lastFindSource.idSWorld) ||
            (Vector3D::Distance(placeCurr.wp, m_lastFindSource.wp) > m_flPathfindThrottleRadius) ||
            (m_placeDest.idSWorld != m_lastFindTarget.idSWorld) ||
            (Vector3D::Distance(m_placeDest.wp, m_lastFindTarget.wp) > m_flPathfindThrottleRadius) ||
            (m_uLastStampTime - m_uLastFindTime > m_uPathfindThrottleTicks))
        {
            fDoPathFind = TRUE;
        }

        if (fDoPathFind && FindPath(m_placeDest, listPathPoints, tCollSettings.uStaticOneRequiredFlags, tCollSettings.uStaticPreventFlags, NULL) == PATH_RESULT_FOUND)
        {
            m_lastFindSource = placeCurr;
            m_lastFindTarget = m_placeDest;
            m_uLastFindTime = m_uLastStampTime;
            POS pos = listPathPoints.GetHeadPosition();
            BOOL bPathValid = TRUE;
            if (m_flMaxPathfindingDist > EPSILON)
            {

                float flTotalDist = 0.0f;
                Vector2D vLastPoint = placeCurr.wp.As2D();

                while (pos)
                {
                    Vector2D & vPoint = listPathPoints.GetNext(pos);
                    Vector2D vDelta = vPoint - vLastPoint;
                    flTotalDist += vDelta.Magnitude();
                    vLastPoint = vPoint;

                    if (flTotalDist >= m_flMaxPathfindingDist)
                    {
                        bPathValid = FALSE;
                        break;
                    }
                }

                pos = listPathPoints.GetHeadPosition();
            }

            while (bPathValid && pos)
            {
                Vector2D & vPoint = listPathPoints.GetNext(pos);
                Vector2D vDiffFromStart;
                vDiffFromStart.x = vPoint.x - placeCurr.wp.x;
                vDiffFromStart.y = vPoint.y - placeCurr.wp.y;
                if (vDiffFromStart.MagnitudeSquared() > flRadius * flRadius)
                {
                    m_vCurrPathDest.x = vPoint.x;
                    m_vCurrPathDest.y = vPoint.y;
                    if (pos)
                        // inference: no row of its own; folded with 576 into one cset
                        bLastLeg = FALSE;
                    break;
                }
            }
        }


        if (m_uMoveFlags & (1 << MOVEBIT_DYNAMIC_PATHING))
        {
            Vector2D vFoundDest;
            DynPathDirection eLastDirection = m_eLastDynPathDirection;
            if (FindDynamicPath(placeCurr, flRadius, m_placeDest.wp.As2D(), vFoundDest, eLastDirection, m_eLastDynPathDirection, tCollSettings) == PATH_RESULT_FOUND)
            {
                m_vCurrPathDest.As2D() = vFoundDest;
                m_bFailedToFindDynamicPath = FALSE;
            }
            else if (FindDynamicPath(placeCurr, flRadius, m_vCurrPathDest.As2D(), vFoundDest, eLastDirection, m_eLastDynPathDirection, tCollSettings) == PATH_RESULT_FOUND)
            {
                m_vCurrPathDest.As2D() = vFoundDest;
                m_bFailedToFindDynamicPath = FALSE;
            }
            else
            {
                m_eLastDynPathDirection = DYNPATH_DIRECTION_NONE;
                m_bFailedToFindDynamicPath = TRUE;
            }
        }
        else
        {
            m_eLastDynPathDirection = DYNPATH_DIRECTION_NONE;
            m_bFailedToFindDynamicPath = FALSE;
        }
    }

    Vector3D & vPosCurr = placeCurr.wp;
    Vector3D vDelta = m_vCurrPathDest - vPosCurr;
    if (!ChecksVertical())
        vDelta.z = 0.0f;  // inference: no row of its own; the select carries 614



    if (vDelta.MagnitudeSquared() < 0.0001f)
    {

        ArrivingAtDest(0, FALSE, FALSE, ACDID_INVALID);
        if (ptACD && !ACDTryToGet(idACD))
        {
            return;
        }


        StopWalk();
        return;
    }


    uint32 uUpdateTicks = GetUpdateInterval();
    if (m_nMaxTicksToWalk >= 0)
    {
        uUpdateTicks = Math::Min(uUpdateTicks, (uint32)m_nMaxTicksToWalk);
    }

    float flAccelMag = m_flAccelMag * (float)uUpdateTicks;
    Vector2D vAccel;
    if (m_uMoveFlags & (1 << MOVEBIT_NO_ACCEL))
    {
        vAccel = vDelta.As2D() * m_flDesiredSpeed;
        m_vVel = cgwpNull;
    }
    else
    {
        Vector2D vDelta2D = vDelta.As2D();
        if (vDelta2D.MagnitudeSquared() > EPSILON * EPSILON)
        {
            vDelta2D.Normalize();
            vAccel = vDelta2D * flAccelMag;
        }
        else
        {
            vAccel = cgwpNull.As2D();
        }
    }

    uint32 uTravelTicks = uUpdateTicks;

    sCapVelocityToReasonableValue(m_vVel);



    Vector2D & vVel2D = m_vVel.As2D();
    float flDistToTravel = (float)uUpdateTicks * m_flCurrentSpeed;
    float flBiasRangeMin = flDistToTravel;
    float flBiasRangeMax = Math::Max(10.0f, flDistToTravel);
    if (!this->IsUsingChargeSteering() && vDelta.MagnitudeSquared() <= flBiasRangeMax * flBiasRangeMax)
    {
        float flBiasRangeDelta = flBiasRangeMax - flBiasRangeMin;
        float flUnitBias = 0.0f;
        if (flBiasRangeDelta >= EPSILON)
        {
            float flDistLeft = vDelta.Magnitude();
            flUnitBias = (flDistLeft - flBiasRangeMin) / flBiasRangeDelta;
            flUnitBias = Math::Pin(flUnitBias, 0.0f, 1.0f);
        }

        Vector2D vUnitDelta2D = vDelta.As2D();
        if (vUnitDelta2D.MagnitudeSquared() > EPSILON * EPSILON)
        {
            vUnitDelta2D.Normalize();
            vVel2D = (flUnitBias * vVel2D) + ((1.0f - flUnitBias) * vUnitDelta2D);
        }
    }

    SteerResult eSteerResult = STEER_RESULT_NORMAL;

    if (!(m_uMoveFlags & (1 << MOVEBIT_NO_STEERING)))
    {


        eSteerResult = m_tSteer.ModifyAccelForSteering(vAccel, bLastLeg);
    }

    if (eSteerResult == STEER_RESULT_FULL_STEERING || eSteerResult == STEER_RESULT_FULL_STEERING_ALLOW_180)
    {





        if (vAccel.MagnitudeSquared() > EPSILON * EPSILON)
        {
            float flScopedAccelMag = vAccel.Normalize();
            float flSpeed = vVel2D.Magnitude();
            vVel2D = vAccel * (flScopedAccelMag + flSpeed);
        }
    }
    else
    {
        vVel2D += vAccel;
    }

    if (vVel2D.MagnitudeSquared() > EPSILON * EPSILON)
    {
        m_flCurrentSpeed = vVel2D.Normalize();
        m_flCurrentSpeed = Math::Min(m_flCurrentSpeed, m_flDesiredSpeed);
        vVel2D *= m_flCurrentSpeed;
    }
    else
    {
        m_flCurrentSpeed = 0.0f;
    }

    m_uMoveFlags &= ~(1 << MOVEBIT_ARRIVING_AT_DESTINATION);

    m_vCurrSteerVel = m_vVel * (float)uTravelTicks;

    ACDID idACDBlocker = ACDID_INVALID;
    if (!(m_uMoveFlags & (1 << MOVEBIT_IGNORE_COLLISION)))
    {


        idACDBlocker = PerformCollision(uTravelTicks, NULL);
    }



    Vector3D vArrivalPos;
    if (TargetWillBeReached(vDelta, uTravelTicks, &vArrivalPos))
    {
        m_uMoveFlags |= (1 << MOVEBIT_ARRIVING_AT_DESTINATION);
    }

    WorldPlace placeNew(m_placeDest);
    Vector3D vTravelThisTick;
    ComputeTravelThisIteration(uTravelTicks, vTravelThisTick);



    BOOL bCantMoveMinimum = FALSE;
    if (!m_tSteer.HuggedThisTick() && m_flMinDistToWalk > EPSILON)
    {
        if (vTravelThisTick.MagnitudeSquared() >= EPSILON * EPSILON)
        {
            Vector3D vMinTravel = vTravelThisTick;
            vMinTravel.Normalize();
            vMinTravel *= 0.5f;

            WorldCollResult tResult;
            bCantMoveMinimum = PerformCollision(vMinTravel, tResult);
        }
        else
        {
            bCantMoveMinimum = TRUE;
        }
    }




    if (bCantMoveMinimum && !(m_uMoveFlags & (1 << MOVEBIT_CLIENT_AUTH_POWER)))
    {
        StopNow();
        return;
    }

    placeNew.wp = placeCurr.wp + vTravelThisTick;

    if (!isfinite(placeNew.wp.x) ||
        !isfinite(placeNew.wp.y) ||
        !isfinite(placeNew.wp.z))
    {
        StopNow();
        return;
    }




    Vector3D vDistMoved = placeNew.wp - placeCurr.wp;
    m_uNextUpdateTick = GetTimeNow() + uTravelTicks;

    if (m_nMaxTicksToWalk >= 0)
    {
        m_nMaxTicksToWalk -= uTravelTicks;
        m_nMaxTicksToWalk = Math::Max(m_nMaxTicksToWalk, 0);
    }

    if (IsArrivingAtFinalDest() || m_nMaxTicksToWalk == 0)
    {
        ArrivingAtDest(uTravelTicks, FALSE, FALSE, ACDID_INVALID);

        if (ptACD && !ACDTryToGet(idACD))
        {
            return;
        }







        placeNew.wp = vArrivalPos;
        m_placeDest.wp = vArrivalPos;

        if (uTravelTicks > 0)
        {
            QueueUpdate(uTravelTicks);
        }
        else
        {
            StopWalk();
        }
    }
    else if (!(m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT)) &&
             vDistMoved.MagnitudeSquared() <= EPSILON * EPSILON && !(m_uMoveFlags & (1 << MOVEBIT_CLIENT_AUTH_POWER)))
    {


        BOOL bWithinRadiusOfTarget = FALSE;
        float flCombinedRadius = flRadius + 1.0f;
        if (Vector3D::Distance2DSquared(placeCurr.wp, m_placeDest.wp) <= flCombinedRadius * flCombinedRadius)
        {
            bWithinRadiusOfTarget = TRUE;
        }
        else
        {
            ACDID idACDTarget = GetTargetACD();
            ActorCommonData *ptACDTarget = ACDTryToGet(idACDTarget);
            if (ptACDTarget && ptACDTarget->IsInWorld())
            {
                ActorMovement *ptMoveTarget = ptACDTarget->GetActorMovement();
                if (ptMoveTarget)
                {
                    WorldPlace placeTarget;
                    float flRadiusTarget = 0.0f;
                    ptMoveTarget->GetPlaceAndRadius(placeTarget, flRadiusTarget, FALSE);
                    flCombinedRadius += flRadiusTarget;
                    if (Vector3D::Distance2DSquared(placeCurr.wp, placeTarget.wp) <= flCombinedRadius * flCombinedRadius)
                        bWithinRadiusOfTarget = TRUE;  // inference: no row of its own; shares the store at 840
                }
            }
        }



        ArrivingAtDest(0, TRUE, bWithinRadiusOfTarget, idACDBlocker);

        if (ptACD && !ACDTryToGet(idACD))
        {
            return;
        }



        StopWalk();

        m_uMoveFlags |= (1 << MOVEBIT_WAS_STUCK);
        Stuck();
        return;
    }
    else
    {
        QueueUpdate(uTravelTicks);
    }

    if (!(m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT)) &&
        m_tSteer.GetStuckUnitValue() > EPSILON)
    {
        float flUpdateSeconds = (float)uUpdateTicks * OO_TICKS_PER_SECOND;
        float flStuckMod = flUpdateSeconds * flOOSecondsToUnStuck;
        m_tSteer.ModifyStuckUnitValue(-flStuckMod);
    }

    if ((m_uMoveFlags & (1 << MOVEBIT_AUTO_UPDATE_YAW)) && IsMoving() && vVel2D.MagnitudeSquared() > EPSILON)
    {
        Angle aFacingYaw = Math::ATan(vVel2D.y, vVel2D.x);
        SetNewFacing(aFacingYaw, FALSE, TRUE);
    }

    if (m_uMoveFlags & (1 << MOVEBIT_NO_SNAP_TO_GROUND))
    {
        placeNew.wp.z += vTravelThisTick.z;
        if (m_vVel.z < 0.0f)
        {
            placeNew.wp.z = Math::Max(placeNew.wp.z, m_placeDest.wp.z);
        }
        else
        {
            placeNew.wp.z = Math::Min(placeNew.wp.z, m_placeDest.wp.z);
        }
    }
    else
    {
        SnapPlaceToGround(placeNew);
    }

    if (m_uMoveFlags & (1 << MOVEBIT_ACTIVATE_TRIGGERS))
    {


        ActivateTriggersToDest(placeNew);

        if (!IsMoving())
            return;
    }






    SetNewPlace(placeNew, FALSE, FALSE, TRUE);

    if (m_snoPowerPassability != -1 && PowerGeneralTagGetBool(m_snoPowerPassability, TAG_POWER_PICKUP_ITEMS_WHILE_MOVING))
    {
        ActorCommonData *ptScopedACD = ACDTryToGet(GetACDID());
        if (ptScopedACD)
        {
            ptScopedACD->PickupLine(placeCurr.wp, placeNew.wp);
        }
    }
}





void ActorMovement::UpdateWalkingRemote()

{










    WorldPlace placeCurr;
    GetPlace(placeCurr);
    if (placeCurr.idSWorld != m_placeDest.idSWorld)
    {
        return;
    }

    BOOL bArriving;
    uint32 uTravelTicks = GetUpdateInterval();

    Vector3D vDelta = m_placeDest.wp - placeCurr.wp;
    if (!(m_uMoveFlags & (1 << MOVEBIT_CHECK_VERTICAL))) vDelta.z = 0.0f;  // inference: the select has no row of its own; all of it sits on 969




    Vector3D vArrivalPos(placeCurr.wp);
    float flDeltaMagSqr = vDelta.MagnitudeSquared();
    if (flDeltaMagSqr < 0.0001f)
    {


        uTravelTicks = 0;
        bArriving = TRUE;  // inference: no row of its own; the store shares 980
    }
    else
    {
        float flAccelMag = m_flAccelMag * (float)uTravelTicks;
        Vector2D vAccel = vDelta.As2D() * flAccelMag;
        Vector2D & vVel2D = m_vVel.As2D();
        vVel2D += vAccel;

        if (m_vVel.MagnitudeSquared() > EPSILON * EPSILON)
        {
            m_flCurrentSpeed = vVel2D.Normalize();
            m_flCurrentSpeed = Math::Min(m_flCurrentSpeed, m_flDesiredSpeed);
            vVel2D *= m_flCurrentSpeed;
        }
        else
        {
            m_flCurrentSpeed = 0.0f;
        }

        bArriving = TargetWillBeReached(vDelta, uTravelTicks, &vArrivalPos);
    }

    WorldPlace placeNew(m_placeDest);
    Vector3D vTravelThisTick;
    ComputeTravelThisIteration(uTravelTicks, vTravelThisTick);

    BOOL bSnap = FALSE;
    if (flDeltaMagSqr >= 225.0f)
    {
        WorldCollResult tResult;
        bSnap = PerformCollision(vTravelThisTick, tResult);
    }






    if (!bSnap)
    {
        placeNew.wp = placeCurr.wp + vTravelThisTick;
    }

    if (!(m_uMoveFlags & (1 << MOVEBIT_NO_SNAP_TO_GROUND)))
    {
        SnapPlaceToGround(placeNew);
    }

    m_uNextUpdateTick = GetTimeNow() + uTravelTicks;

    if (bArriving)
    {
        ArrivingAtDest(uTravelTicks, FALSE, FALSE, ACDID_INVALID);

        placeNew.wp = vArrivalPos;

        if (uTravelTicks)
        {
            QueueUpdate(uTravelTicks);
        }
        else
        {
            StopWalk();
        }
    }
    else
    {
        QueueUpdate(GetUpdateInterval());
    }

    SetNewPlace(placeNew, FALSE, FALSE, TRUE);
}



void ActorMovement::UpdateWalkingInPlace()
{
	m_vVel = cgwvNull;

	uint32 dwNow = GetTimeNow();
	if (m_uMinDurationEndStampTime <= dwNow)
	{
		m_eMovementType = MOVEMENT_TYPE_WALKING;
		m_vVel = m_vLastVel;
		m_bWaitAtDestination = FALSE;
		QueueUpdate(1);
	}
	else
	{
		uint32 dwDelay = m_uMinDurationEndStampTime - dwNow;
		dwDelay = Math::Min(dwDelay, 127u);
		QueueUpdate(dwDelay);
	}
}



void ActorMovement::UpdateStoppedProjectile()
{
	m_vVel = cgwvNull;

	if (m_uMoveFlags & (1 << MOVEBIT_IS_PROJECTILE))
	{
		ACDID idACDProj = GetACDID();
		int32 nTicksTillDetonate = ProjectileGetTicksTillDetonate(idACDProj);
		if (nTicksTillDetonate == 0)
		{
			BOOL bStoppedAtTarget = ProjCollisionResponse(ACDID_INVALID);
			(void)bStoppedAtTarget;
			return;
		}
	}

	QueueUpdate(GetUpdateInterval());
}







const int32 MAX_NUM_PROJECTILE_COLLISIONS_PER_TICK = 5;
// NON_MATCHING: same size (1852 B), same calls/control flow; FP register allocation differs: the original keeps tCollisionResult.flT in s14 (loaded right after the PerformCollision call) and spills the 2500.0f cap to the stack, ours spills flT and keeps 2500.0f in a register (~60 insns differ). Tried return/break at 1118, bCollision init, condition orders.
void ActorMovement::UpdateFixed()

{
    BOOL bStopIterations = FALSE;

    for (int32 nNumCollisions = 0; !bStopIterations; nNumCollisions++)
    {
        if (nNumCollisions >= MAX_NUM_PROJECTILE_COLLISIONS_PER_TICK)
        {


            QueueUpdate(1);
            break;
        }

        WorldPlace placeCurr;
        GetPlace(placeCurr);

        m_placeLastStamp = placeCurr;
        m_uLastStampTime = GetTimeNow();
        m_flLastStampSpeed = m_flCurrentSpeed;

        uint32 uTravelTicks = GetUpdateInterval();
        BOOL bProjectile = m_uMoveFlags & (1 << MOVEBIT_IS_PROJECTILE);
        BOOL bHugsGround = !(m_uMoveFlags & (1 << MOVEBIT_NO_SNAP_TO_GROUND));

        ACDID idACDHit = ACDID_INVALID;

        int32 nTicksTillDetonate = 0;
        if (bProjectile)
        {
            ACDID idACDProj = GetACDID();

            nTicksTillDetonate = ProjectileGetTicksTillDetonate(idACDProj);
            if (nTicksTillDetonate != -1)
            {
                uTravelTicks = Math::Min((int32)uTravelTicks, nTicksTillDetonate);
            }
        }

        WorldCollResult tCollisionResult;
        // inference: no row of its own (the zero reaches 1212 as a phi on the IGNORE_COLLISION path)
        tCollisionResult.bCollision = FALSE;
        if (!(m_uMoveFlags & (1 << MOVEBIT_IGNORE_COLLISION)))
        {
            ImpactData tImpactData(GetACDID());
            if (!bHugsGround)
            {
                PerformCollisionAgainstPhysicsMesh(uTravelTicks, tImpactData);
            }
            tImpactData.idACDIntersected = PerformCollision(uTravelTicks, &tCollisionResult);


























            if (IsServerProjectile())
            {
                idACDHit = PerformProjectileCollision(uTravelTicks);




                if (idACDHit == ACDID_INVALID && tImpactData.idACDIntersected != ACDID_INVALID)
                {
                    ActorCommonData *ptACDIntersected = ACDTryToGet(tImpactData.idACDIntersected);
                    if (ptACDIntersected && ProjectileCanCollisionWithACD(GetACDID(), *ptACDIntersected))
                        idACDHit = tImpactData.idACDIntersected;
                }
            }
        }




        Vector3D vTravelThisTick;
        ComputeTravelThisIteration(uTravelTicks, vTravelThisTick);

        WorldPlace placeNew;
        placeNew.idSWorld = placeCurr.idSWorld;
        placeNew.wp = placeCurr.wp + vTravelThisTick;

        BOOL bNavReflectResponse = FALSE;
        if (uTravelTicks == 0 && idACDHit == ACDID_INVALID &&

            (m_uMoveFlags & (1 << MOVEBIT_NAV_REFLECT)) && tCollisionResult.bCollision)
        {



            if (tCollisionResult.flT <= EPSILON)
            {
                placeNew.wp -= GetVelocity();
            }


            uTravelTicks = 1;
            bNavReflectResponse = TRUE;
            m_vVel = Reflect(m_vVel, tCollisionResult.wvNormal);


            ForceSendUpdate();
        }













        if (bHugsGround)
        {
            SnapPlaceToGround(placeNew);
        }

        SetNewPlace(placeNew, FALSE, FALSE, TRUE);

        if (uTravelTicks == 0 && bProjectile)
        {
            if ((m_uMoveFlags & (1 << MOVEBIT_STOP_ON_IMPACT)) && idACDHit == ACDID_INVALID && nTicksTillDetonate > 0)
            {


                StopProjectile();
                return;
            }


            bStopIterations = ProjCollisionResponse(idACDHit);
        }
        else
        {

            if (bNavReflectResponse)
            {
                ProjNavReflectResponse();
            }

            if (idACDHit != ACDID_INVALID)
            {
                m_eMovementType = MOVEMENT_TYPE_FIXED_PROJECTILE_COLLIDING;
                m_idACDTarget = idACDHit;
            }

            QueueUpdate(uTravelTicks);
            bStopIterations = TRUE;
        }

        m_vLastVel = GetVelocity();
    }
}

static float MOVEMENT_ARC_MINIMUM_REFLECT_SPEED = 0.5f;
static float MOVEMENT_ARC_MINIMUM_REFLECT_SPEED_SQR = MOVEMENT_ARC_MINIMUM_REFLECT_SPEED * MOVEMENT_ARC_MINIMUM_REFLECT_SPEED;
static float MOVEMENT_ARC_TERMINAL_VELOCITY = 20.0f;
static float MOVEMENT_ARC_TERMINAL_VELOCITY_SQR = MOVEMENT_ARC_TERMINAL_VELOCITY * MOVEMENT_ARC_TERMINAL_VELOCITY;


void ActorMovement::UpdateArc()
{
	WorldPlace placeCurr;
	GetPlace(placeCurr);

	m_placeLastStamp = placeCurr;
	m_uLastStampTime = GetTimeNow();
	m_flLastStampSpeed = m_flCurrentSpeed;

	uint32 uTravelTicks = GetUpdateInterval();
	ImpactData tImpactData(GetACDID());
	BOOL bPhysicsColl = FALSE;
	BOOL bProjectile = m_uMoveFlags & (1 << MOVEBIT_IS_PROJECTILE);

	if (bProjectile)
	{
		ACDID idACDProj = GetACDID();
		int32 nTicksTillDetonate = ProjectileGetTicksTillDetonate(idACDProj);
		if (nTicksTillDetonate != -1)
		{
			int32 nTicks = uTravelTicks;
			uTravelTicks = Math::Min(nTicks, nTicksTillDetonate);
		}
	}

	if (m_uMoveFlags & ((1 << MOVEBIT_ARC_UNTIL_DEST_HEIGHT) | (1 << MOVEBIT_USE_DESIRED_HEIGHT_FOR_CAMERA)))
	{
		float flDesiredPositionOffset = m_flDesiredHeight - placeCurr.wp.z;
		float flTicks = ComputeFallingTicksToHitTarget(m_flGravity, m_vVel.z, flDesiredPositionOffset);
		int32 nImpactTicks = Math::FloatFloorToInt(flTicks);
		uTravelTicks = Math::Pin(nImpactTicks, 0, (int32)uTravelTicks);
	}
	else if (bProjectile)
	{
		bPhysicsColl = PerformCollisionAgainstPhysicsMesh(uTravelTicks, tImpactData);
		if (!(m_uMoveFlags & (1 << MOVEBIT_IGNORE_COLLISION)))
		{
			tImpactData.idACDIntersected = PerformCollision(uTravelTicks, NULL);
			if (!(m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT)))
			{
				tImpactData.idACDIntersected = PerformProjectileCollision(uTravelTicks);
			}
		}
	}
	else
	{
		if (!(m_uMoveFlags & (1 << MOVEBIT_IGNORE_COLLISION)))
		{
			tImpactData.idACDIntersected = PerformCollision(uTravelTicks, NULL);
		}
		if (uTravelTicks == 0)
		{
			m_vVel.x = 0.0f;
			m_vVel.y = 0.0f;
			uTravelTicks = 1;
		}
		if (m_vVel.z < 1e-6f)
		{
			CollideAgainstHeight(uTravelTicks);
		}
	}

	Vector3D vTravelThisTick;
	ComputeTravelThisIteration(uTravelTicks, vTravelThisTick);
	Vector3D vNormal;
	float flUnitTraveled;

	if (bPhysicsColl && m_flUnitBounce > 1e-6f && uTravelTicks == 0 && tImpactData.idACDIntersected == ACDID_INVALID &&
		Vector3D::DotProduct(tImpactData.vNormal, m_vVel) <= 0.0f)
	{
		ComputeTravelThisIteration(1, vTravelThisTick);

		if (vTravelThisTick.MagnitudeSquared() > MOVEMENT_ARC_MINIMUM_REFLECT_SPEED_SQR ||
			Vector3D::DotProduct(tImpactData.vNormal, cgwvZAxis) < 0.5f)
		{
			Bounced();

			Vector3D vIntersect;
			WorldPlace placeEnd;
			uTravelTicks = 1;
			vTravelThisTick = Reflect(vTravelThisTick, tImpactData.vNormal);

			placeEnd.idSWorld = placeCurr.idSWorld;
			placeEnd.wp = placeCurr.wp + vTravelThisTick;
			BOOL bCollided = RaycastAgainstWorld(placeCurr, placeEnd, vIntersect, vNormal, flUnitTraveled, 0x2c0);
			if (bCollided)
			{
				vTravelThisTick = cgwvNull;
			}

			m_vVel = Reflect(m_vVel, tImpactData.vNormal);
			m_vVel *= m_flUnitBounce;
			ForceSendUpdate();
		}
	}

	m_vVel.z += m_flGravity * uTravelTicks;
	if (m_vVel.MagnitudeSquared() > MOVEMENT_ARC_TERMINAL_VELOCITY_SQR)
	{
		m_vVel.Normalize();
		m_vVel *= MOVEMENT_ARC_TERMINAL_VELOCITY;
	}

	WorldPlace placeNew;
	placeNew.idSWorld = placeCurr.idSWorld;
	placeNew.wp = placeCurr.wp + vTravelThisTick;

	if (m_uMoveFlags & (1 << MOVEBIT_ACTIVATE_TRIGGERS))
	{
		ActivateTriggersToDest(placeNew);
		if (!IsMoving())
		{
			return;
		}
	}

	if (!bProjectile && !(m_uMoveFlags & ((1 << MOVEBIT_ARC_UNTIL_DEST_HEIGHT) | (1 << MOVEBIT_USE_DESIRED_HEIGHT_FOR_CAMERA))) && m_vVel.z <= 0.0f)
	{
		WorldPlace placeGround(placeNew);
		SnapPlaceToGround(placeGround);
		if (placeNew.wp.z < placeGround.wp.z)
		{
			uTravelTicks = 0;
			placeNew = placeGround;
		}
	}

	SetNewPlace(placeNew, FALSE, FALSE, TRUE);

	if (uTravelTicks != 0)
	{
		QueueUpdate(uTravelTicks);
	}
	else if (bProjectile)
	{
		BOOL bStoppedAtTarget = ProjCollisionResponse(tImpactData);
		if (bStoppedAtTarget)
		{
			StopNow();
			SnapToNewPlace(placeNew, TRUE, FALSE, TRUE, 0);
		}
		else
		{
			QueueUpdate(1);
		}
	}
	else
	{
		if (m_uMoveFlags & (1 << MOVEBIT_ARC_UNTIL_DEST_HEIGHT))
		{
			placeNew = m_placeDest;
		}
		StopNow();
		SnapToNewPlace(placeNew, TRUE, TRUE, TRUE, 0);
		ForceBrainUpdate();
		ArrivingAtDest(0, FALSE, FALSE, ACDID_INVALID);

		if (m_eLandingAnimTag != -1)
		{
			float flScalar = 1.0f;
			ACDID idACD = GetACDID();
			ActorCommonData * ptACD = ACDTryToGet(idACD);
			if (ptACD && ACDIsPlayer(idACD))
			{
				SNO snoPower = GetPowerPassability();
				if (snoPower != -1)
				{
					flScalar = PowerGetFormulaValue(ptACD, AutoPowerDefinitionR(snoPower), TAG_POWER_SPECIALWALK_PLAYER_END_ANIM_SCALAR, -1, -1, 0);
				}
			}
			PlayPreplayedAnim(m_eLandingAnimTag, flScalar);
		}
	}

	m_vLastVel = GetVelocity();
}













































void ActorMovement::UpdateDetPathPlace(
    uint32 uTravelTicks)
{
    WorldPlace placeCurr;
    GetPlace(placeCurr);

    Vector3D vToDest;
    ComputeTravelThisIteration(uTravelTicks, vToDest);

    WorldPlace placeNew;
    placeNew.idSWorld = placeCurr.idSWorld;
    placeNew.wp = placeCurr.wp + vToDest;

    if (!(m_uMoveFlags & (1 << MOVEBIT_NO_SNAP_TO_GROUND)))
    {
        SnapPlaceToGround(placeNew);
    }

    SetNewPlace(placeNew, FALSE, FALSE, TRUE);
}



void ActorMovement::UpdateDetPath()

{

    uint32 uTravelTicks = GetUpdateInterval();
    float flUpdateTime = (float)uTravelTicks;
    uint32 uNewUpdate = DetPathGetUpdateInterval(m_tDPath.eDPath);



    DeterministicPathData tOrigDPath = m_tDPath;
    Vector3D vOrigVel = m_vVel;



    if (IsServerProjectile())
    {
        m_tDPath.flSpeedMult = ProjectileGetSpeedMultiplier(GetACDID());
    }


    DetPathUpdate(*this, m_vVel, m_tDPath, flUpdateTime);

    WorldPlace placeCurr;
    GetPlace(placeCurr);

    m_placeLastStamp = placeCurr;
    m_uLastStampTime = GetTimeNow();
    m_flLastStampSpeed = m_vVel.Magnitude();

    ACDID idACDHit = ACDID_INVALID;

    if (!IsRemoteMovement())
    {
        m_uUpdateInterval = uNewUpdate;
    }

    int32 nTicksTillDetonate = 0;
    BOOL bProjectile = m_uMoveFlags & (1 << MOVEBIT_IS_PROJECTILE);
    if (bProjectile)
    {
        ACDID idACDProj = GetACDID();

        nTicksTillDetonate = ProjectileGetTicksTillDetonate(idACDProj);
        if (nTicksTillDetonate != -1)
        {
            uTravelTicks = Math::Min((int32)uTravelTicks, nTicksTillDetonate);
        }
    }

    WorldCollResult tCollisionResult;
    tCollisionResult.bCollision = FALSE;

    if (!(m_uMoveFlags & (1 << MOVEBIT_IGNORE_COLLISION)))
    {
        ACDID idACDHitCollision = PerformCollision(uTravelTicks, &tCollisionResult);


        if (!(m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT)))
        {
            idACDHit = PerformProjectileCollision(uTravelTicks);

            if (idACDHit == ACDID_INVALID)
            {

                ActorCommonData *ptACDIntersected = ACDTryToGet(idACDHitCollision);
                if (ptACDIntersected && ProjectileCanCollisionWithACD(GetACDID(), *ptACDIntersected))
                    idACDHit = idACDHitCollision;
            }
        }
    }




    UpdateDetPathPlace(uTravelTicks);

    if (uTravelTicks == 0 && idACDHit == ACDID_INVALID &&

        (m_uMoveFlags & (1 << MOVEBIT_NAV_REFLECT)) &&
        tCollisionResult.bCollision &&
        DetPathProcessNavReflect(*this, m_tDPath, tCollisionResult))
    {
        ProjNavReflectResponse();

        ForceSendUpdate();

        QueueUpdate(1);
        if (!(m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT)))
        {
            m_uUpdateInterval = 1;
        }
    }
    else if (idACDHit != ACDID_INVALID || uTravelTicks == 0)
    {
        if ((m_uMoveFlags & (1 << MOVEBIT_STOP_ON_IMPACT)) && idACDHit == ACDID_INVALID && nTicksTillDetonate > 0)
        {


            StopProjectile();
        }
        else
        {
            BOOL bStoppedAtTarget = ProjCollisionResponse(idACDHit);
            if (!bStoppedAtTarget

                && !ProjectileIsReflected(GetACDID()))
            {





                m_tDPath = tOrigDPath;
                m_vVel = vOrigVel;

                DetPathUpdate(*this, m_vVel, m_tDPath, 1.0f);
                UpdateDetPathPlace(1);
                QueueUpdate(1);
            }
        }
    }
    else
    {
        QueueUpdate(uTravelTicks);
    }


    if (IsServerProjectile())
    {


        ACDID idACD = GetACDID();
        float flProjSpeedMult = ProjectileGetSpeedMultiplier(idACD);
        m_vLastVel = cgwvXAxis * flProjSpeedMult;
    }

}



BOOL ActorMovement::TargetWillBeReached(
    const Vector3D & vToCurrentWalkTarget,
    uint32 & uTicksToTravel,
    Vector3D * pvArrivalPos) const
{
    WorldPlace placeCurr;
    GetPlace(placeCurr);
    if (pvArrivalPos)
    {
        *pvArrivalPos = placeCurr.wp;
    }

    Vector3D vToTarget = m_placeDest.wp - placeCurr.wp;
    // inference: no row of its own; the z select is folded into the Magnitude at 1693
    if (!(m_uMoveFlags & (1 << MOVEBIT_CHECK_VERTICAL)))
        vToTarget.z = 0.0f;

    float flDistLeft = vToTarget.Magnitude();
    // inference: the null test at 1696 is ACDTryToGet's own (no row); 1698 col 13 and 1706 col 54 put those lines at depth 2 and 3
    ActorCommonData *ptACDTarget = ACDTryToGet(m_idACDTarget);
    if (ptACDTarget)
    {
        if (!ptACDTarget->IsInWorld())
        {
            // inference: this branch leaves no code (cbz from 1698 straight to 1716);
            // ptACDTarget is not read again
            ptACDTarget = NULL;
        }
        else
        {
            WorldPlace placeTargetACD = ptACDTarget->GetPlace();
            vToTarget = placeTargetACD.wp - placeCurr.wp;
            // inference: no row of its own; the z select is folded into the Magnitude at 1712
            if (!(m_uMoveFlags & (1 << MOVEBIT_CHECK_VERTICAL)))
                vToTarget.z = 0.0f;

            flDistLeft = Math::Min(flDistLeft, vToTarget.Magnitude());
        }
    }

    if (flDistLeft <= m_flMaxRangeFromDest)
    {
        uTicksToTravel = 0;
        return TRUE;
    }



    if (uTicksToTravel == 0)
        return FALSE;



    if (GetVelocity().MagnitudeSquared() < EPSILON * EPSILON)
        return FALSE;



    Vector3D vDirMovement = vToCurrentWalkTarget;
    if (!(m_uMoveFlags & (1 << MOVEBIT_CHECK_VERTICAL)))
        vDirMovement.z = 0.0f;



    if (vDirMovement.MagnitudeSquared() < EPSILON * EPSILON)
        return FALSE;


    vDirMovement.Normalize();

    Vector3D vTravelThisTick;
    ComputeTravelThisIteration(uTicksToTravel, vTravelThisTick);

    float flProjDist = Vector3D::DotProduct(vDirMovement, vTravelThisTick);
    if (flProjDist < (flDistLeft - m_flMaxRangeFromDest) || flProjDist < EPSILON)
        return FALSE;




    Ray2D rayTravel;
    rayTravel.wpOrigin.x = placeCurr.wp.x;
    rayTravel.wpOrigin.y = placeCurr.wp.y;
    rayTravel.wvDirection.x = vTravelThisTick.x;
    rayTravel.wvDirection.y = vTravelThisTick.y;
    float flDistToTravel = rayTravel.wvDirection.Normalize();
    Circle circleDest;
    float flMaxRange = Math::Max(m_flMaxRangeFromDest, 0.01f);
    circleDest.Set(m_placeDest.wp.x, m_placeDest.wp.y, flMaxRange);

    float flT = 0.0f;
    if (!Intersect::Ray2DAndCircle(rayTravel, circleDest, flT))
        return FALSE;



    if (flDistToTravel < flT)
        return FALSE;



    float flPercentTraveled = flT / flDistToTravel;
    uTicksToTravel = Math::FloatRoundToInt(flPercentTraveled * (float)uTicksToTravel);

    Vector2D vDelta = rayTravel.wvDirection;
    vDelta *= flT;
    if (pvArrivalPos)
    {
        pvArrivalPos->x += vDelta.x;
        pvArrivalPos->y += vDelta.y;

        if (m_uMoveFlags & (1 << MOVEBIT_CHECK_VERTICAL))
        {
            pvArrivalPos->z += flPercentTraveled * vTravelThisTick.z;
            if (vTravelThisTick.z < 0.0f)
            {
                pvArrivalPos->z = Math::Max(pvArrivalPos->z, m_placeDest.wp.z);
            }
            else
            {
                pvArrivalPos->z = Math::Min(pvArrivalPos->z, m_placeDest.wp.z);
            }
        }
    }

    return TRUE;
}




















BOOL ActorMovement::CollideAgainstHeight(uint32 & uTicksToTravel) const
{
	WorldPlace placeCurr;
	GetPlace(placeCurr);

	NavMeshHandle hNavMeshCurr;
	ScenesCommonNavMeshNodeFromPlace(placeCurr, &hNavMeshCurr, FALSE, FALSE);
	if (hNavMeshCurr.idSScene == SSCENEID_INVALID)
	{
		uTicksToTravel = 0;
		return TRUE;
	}

	Vector3D vToDest;
	ComputeTravelThisIteration(uTicksToTravel, vToDest);

	float flT;
	Vector3D wpColl;
	ACDCollSettings tCollSettings;
	GetCollSettings(tCollSettings);
	if (NavZoneCollideSweptPoint3D(placeCurr.idSWorld, placeCurr.wp, vToDest, tCollSettings.uStaticOneRequiredFlags, tCollSettings.uStaticPreventFlags, 0, flT, wpColl))
	{
		uTicksToTravel = Math::FloatFloorToInt(flT * uTicksToTravel);
		return TRUE;
	}
	return FALSE;
}







BOOL ActorMovement::IsMoving() const

{
	if (m_eMovementType == MOVEMENT_TYPE_NONE ||
		m_eMovementType == MOVEMENT_TYPE_CLIENT_AUTH ||
		m_eMovementType == MOVEMENT_TYPE_CLIENT_AUTH_FACING ||
		m_eMovementType == MOVEMENT_TYPE_STOPPED_PROJECTILE)
	{
		return FALSE;
	}

	return TRUE;
}









BOOL ActorMovement::PerformCollision(const Vector3D & vToDest, WorldCollResult & tResult) const
{
	tResult.bCollision = FALSE;

	WorldPlace placeCurr;
	float flRadius;
	GetPlaceAndRadius(placeCurr, flRadius, TRUE);

	Circle circleCurr;
	circleCurr.Set(placeCurr.wp.x, placeCurr.wp.y, flRadius);
	Vector3D vDest = placeCurr.wp + vToDest;

	ACDCollSettings tCollSettings;
	GetCollSettings(tCollSettings);

	WorldCollParams tWorldCollParams(tCollSettings);
	tResult.idACDHit = ACDID_INVALID;
	ACDID idACD = GetACDID();
	ActorCommonData * ptACD = ACDTryToGet(idACD);

	if ((m_uMoveFlags & (1 << MOVEBIT_IS_PROJECTILE)) && GameActiveGameCommonDataIsClient())
	{
		tWorldCollParams.uSpecialFlags |= 2;
	}

	if (ptACD && ptACD->AttributesGetInt(ATTRIBUTE_MOVEMENT_DESTROYS_WALLER_WALLS) > 0)
	{
		tWorldCollParams.uSpecialFlags |= 4;
	}

	const NavCellHandle & hNavCellCurr = GetNavCell();
	if (vToDest.MagnitudeSquared() > 300.0f * 300.0f)
	{




		tResult.idACDHit = ACDID_INVALID;
		tResult.wpEnd = placeCurr.wp;
		tResult.hNavCellEnd = hNavCellCurr;
		tResult.flT = 0.0f;
		tResult.wpContact = placeCurr.wp;
		tResult.wvNormal = cgwvXAxis;
		tResult.bCollision = TRUE;
		return TRUE;
	}

	BOOL bCollided;
	bCollided = WorldCollSweptCircle(placeCurr.idSWorld, hNavCellCurr, circleCurr, vDest.As2D(), tWorldCollParams, 3, 1, idACD, tResult);
	return bCollided;
}



















ACDID ActorMovement::PerformCollision(uint32 & uTicksToTravel, WorldCollResult * tOutResult) const
{
	Vector3D vToDest;
	ComputeTravelThisIteration(uTicksToTravel, vToDest);
	float flTravelDist = vToDest.Magnitude();

	ACDID idACDHit;
	WorldCollResult tResult;
	BOOL bCollided = PerformCollision(vToDest, tResult);
	idACDHit = tResult.idACDHit;
	if (bCollided)
	{
		if (flTravelDist > 1e-6f)
		{
			float flTotalDist = Math::Max(0.0f, flTravelDist * tResult.flT - 0.1f);
			uTicksToTravel = Math::FloatFloorToInt((flTotalDist / flTravelDist) * uTicksToTravel);
		}
		else
		{
			uTicksToTravel = 0;
		}
	}

	if (tOutResult)
	{
		*tOutResult = tResult;
	}
	return idACDHit;
}










BOOL ActorMovement::PerformCollisionAgainstPhysicsMesh(uint32 & uTicksToTravel, ImpactData & tImpactData) const
{
	Vector3D vToDest;
	ComputeTravelThisIteration(uTicksToTravel, vToDest);
	float flTravelDist = vToDest.Magnitude();

	WorldPlace placeCurr;
	GetPlace(placeCurr);
	WorldPlace placeEnd = placeCurr;
	placeEnd.wp = placeCurr.wp + vToDest;

	Vector3D vIntersect;
	Vector3D vNormal;
	float flUnitTraveled;
	BOOL bCollided = RaycastAgainstWorld(placeCurr, placeEnd, vIntersect, vNormal, flUnitTraveled, 0x2c0);
	if (bCollided)
	{
		if (flTravelDist > 1e-6f)
		{
			float flTotalDist = Math::Max(0.0f, flTravelDist * flUnitTraveled - 0.1f);
			float flUnitTime = flTotalDist / flTravelDist;
			flUnitTime = Math::Pin(flUnitTime, 0.0f, 1.0f);
			float flTicks = flUnitTime * uTicksToTravel;
			int32 nTicks = Math::FloatFloorToInt(flTicks);
			uTicksToTravel = Math::Max(nTicks, 0);
		}
		else
		{
			uTicksToTravel = 0;
		}

		tImpactData.vPosition = vIntersect;
		tImpactData.vNormal = vNormal;
	}

	return bCollided;
}











void ActorMovement::StopNow()
{
	ACDID idACD = GetACDID();
	ActorCommonData * ptACD = ACDTryToGet(idACD);
	if (ptACD)
	{
		ptACD->PickupNearby();
	}

	if (m_eMovementType != MOVEMENT_TYPE_NONE)
	{
		m_eMovementType = MOVEMENT_TYPE_NONE;
		m_uNextUpdateTick = 0;
		m_vVel = cgwvNull;
		m_flCurrentSpeed = 0.0f;
		m_bFailedToFindDynamicPath = FALSE;
	}
}








void ActorMovement::StopSoon()
{
	if (IsMoving())
	{
		m_uMoveFlags |= (1 << MOVEBIT_ARRIVING_AT_DESTINATION);

		WorldPlace placeCurr;
		GetPlace(placeCurr);
		m_placeDest = placeCurr;
	}
}







void ActorMovement::StopWalk()

{
    if (m_eMovementType != MOVEMENT_TYPE_NONE)
    {
        if (m_bWaitAtDestination)
        {
            m_eMovementType = MOVEMENT_TYPE_WALKING_IN_PLACE;
        }
        else
        {
            m_eMovementType = MOVEMENT_TYPE_WALK_FINISHING;
        }

        QueueUpdate(1);
    }
}



BOOL ActorMovement::StopProjectile()
{
	if (IsMoving())
	{
		StopNow();
		m_eMovementType = MOVEMENT_TYPE_STOPPED_PROJECTILE;
		QueueUpdate(1);
		return TRUE;
	}
	return FALSE;
}







void ActorMovement::MoveAtFixedVelocity(Vector3D & vVel, uint32 uMoveFlags, ActorAnimationTag eAnimTag, SNO snoPowerPassability)
{
	if (IsServerProjectile())
		uMoveFlags |= (1 << MOVEBIT_IS_PROJECTILE);

	m_placeDest = cgplaceNull;
	m_placeStart = cgplaceNull;
	m_vInitialVelocity = cgwvNull;
	m_flDesiredSpeed = vVel.Magnitude();
	m_flMaxRangeFromDest = 0.0f;
	m_flUnitBounce = 0.0f;
	m_uMoveFlags = uMoveFlags;
	m_idACDTarget = ACDID_INVALID;
	m_flGravity = 0.0f;
	m_eAnimTag = eAnimTag;
	m_vVel = vVel;
	m_vLastVel = vVel;
	m_snoPowerPassability = snoPowerPassability;
	m_eMovementType = MOVEMENT_TYPE_FIXED_PROJECTILE;
	m_pfnCallback = NULL;

	QueueUpdate(GetEarliestUpdateTickOffset());
}














void ActorMovement::FaceTarget(const Vector3D & vTarget, BOOL bSnap)
{
	WorldPlace placeCurr;
	GetPlace(placeCurr);

	Vector3D vDelta = vTarget - placeCurr.wp;
	vDelta.z = 0.0f;

	if (vDelta.MagnitudeSquared() > 0.1f)
	{
		Angle aYaw = Math::ATan(vDelta.y, vDelta.x);
		SetNewFacing(aYaw, bSnap, TRUE);
		QueueUpdate(0);
	}
}





float ActorMovement::ComputeFallingTicksToHitTarget(float flGravity, float flInitialSpeed, float flDesiredPositionOffset)
{
	float flA = 0.5f * flGravity;
	float flB = flInitialSpeed;
	float flC = -flDesiredPositionOffset;

	float flDiscriminant = flB * flB - 4.0f * flA * flC;
	flDiscriminant = Math::Max(flDiscriminant, 0.0f);
	float flTicks = (-flB - Math::Sqrt(flDiscriminant)) / (2.0f * flA);
	return flTicks;
}





















BOOL ActorMovement::MoveWithArc(const WorldPlace & placeDest, ActorAnimationTag eFlightAnimTag, ActorAnimationTag eLandingAnimTag, uint32 uMoveFlags, float flGravity, float flRelativeHeight, SNO snoPowerPassability, float flUnitBounce)
{
	WorldPlace placeCurr;	GetPlace(placeCurr);
	if (placeDest.idSWorld != placeCurr.idSWorld)
	{
		return FALSE;
	}

	if (Vector3D::Distance2DSquared(placeCurr.wp, placeDest.wp) >= 500.0f * 500.0f)
	{
		ACDID idACD = GetACDID();
		return FALSE;
	}











	float flDesiredPositionOffset = Math::Max(flRelativeHeight + 1.0f, placeDest.wp.z - placeCurr.wp.z);
	flDesiredPositionOffset = Math::Max(flDesiredPositionOffset, 1.0f);


	float flInitialSpeed = Math::Sqrt(flGravity * -2.0f * flDesiredPositionOffset);
	float flTicks = ComputeFallingTicksToHitTarget(flGravity, flInitialSpeed, placeDest.wp.z - placeCurr.wp.z);
	flTicks = Math::Max(flTicks, EPSILON);



	float flOOTicks = 1.0f / flTicks;

	Vector3D vInitialVel;
	Vector3D vToDest2D = placeDest.wp - placeCurr.wp;
	vToDest2D.z = 0.0f;
	if (vToDest2D.MagnitudeSquared() > EPSILON * EPSILON)
	{
		float flDist = vToDest2D.Normalize();
		m_flDesiredSpeed = flOOTicks * flDist;
		vInitialVel = vToDest2D * m_flDesiredSpeed;
	}
	else
	{
		m_flDesiredSpeed = 0.0f;
		vInitialVel.x = 0.0f;
		vInitialVel.y = 0.0f;
	}

	vInitialVel.z = flOOTicks * (-0.5f * flGravity * flTicks * flTicks - placeCurr.wp.z + placeDest.wp.z);




	return MoveWithArc(eFlightAnimTag, eLandingAnimTag, uMoveFlags, flGravity, vInitialVel, snoPowerPassability, flUnitBounce, &placeDest);
}



BOOL ActorMovement::MoveWithArc(ActorAnimationTag eFlightAnimTag, ActorAnimationTag eLandingAnimTag, uint32 uMoveFlags, float flGravity, Vector3D & vInitialVel, SNO snoPowerPassability, float flUnitBounce, const WorldPlace * ptPlaceDest)
{
	if (IsServerProjectile())
	{
		uMoveFlags |= (1 << MOVEBIT_IS_PROJECTILE);
	}

	m_uMoveFlags = uMoveFlags;
	m_placeDest = cgplaceNull;
	GetPlace(m_placeStart);
	m_vInitialVelocity = vInitialVel;

	if (m_uMoveFlags & (1 << MOVEBIT_ARC_UNTIL_DEST_HEIGHT))
	{
		if (ptPlaceDest)
		{
			m_placeDest = *ptPlaceDest;
			m_flDesiredHeight = m_placeDest.wp.z;
		}
		else
		{
			m_uMoveFlags &= ~(1 << MOVEBIT_ARC_UNTIL_DEST_HEIGHT);
		}
	}

	m_flMaxRangeFromDest = 0.0f;
	m_flUnitBounce = flUnitBounce;
	m_idACDTarget = ACDID_INVALID;
	m_flGravity = flGravity;
	m_vVel = vInitialVel;
	m_vLastVel = m_vVel;
	m_snoPowerPassability = snoPowerPassability;
	m_eMovementType = MOVEMENT_TYPE_ARC;
	m_eAnimTag = eFlightAnimTag;
	m_eLandingAnimTag = eLandingAnimTag;
	m_pfnCallback = NULL;
	m_uMinDurationEndStampTime = 0;
	m_bWaitAtDestination = FALSE;
	m_nMaxTicksToWalk = -1;

	QueueUpdate(GetEarliestUpdateTickOffset());
	return TRUE;
}

















void ActorMovement::ComputeTravelThisIteration(uint32 uTicks, Vector3D & vResult) const
{
	float flTicks = (float)uTicks;

	vResult = GetVelocity() * flTicks;

	if (Math::Abs(m_flGravity) > 1e-6f)
	{
		vResult.z += m_flGravity * 0.5f * flTicks * flTicks;
	}

	if (!isfinite(vResult.x) ||
		!isfinite(vResult.y) ||
		!isfinite(vResult.z))
	{
		vResult = cgwvNull;
	}
}





void ActorMovement::GetCollSettings(ACDCollSettings & tCollSettings) const
{
	GetCollSettings(m_snoPowerPassability, 0, tCollSettings);
}








void ActorMovement::GetCollSettings(SNO snoPower, PowerPassabilityType ePowerPassabilityType, ACDCollSettings & tCollSettings) const
{
	ACDID idACD = GetACDID();
	ActorCommonData * ptACD = ACDTryToGet(idACD);
	if (ptACD)
	{
		tCollSettings = ptACD->tCollSettings;
	}
	else
	{

		SNO snoActor = GetActorSNO();


		ACDCollGetSettings(snoActor, tCollSettings);

		ScopedActorDefinitionLockR ptActorDef(snoActor);
		WorldPlace tPlace;
		GetPlace(tPlace);
		AxialCylinder * tCyl = (AxialCylinder *)tCollSettings.arAxialCylinderBytes;
		tCyl->wpBase = tPlace.wp;
		tCyl->wdHeight = ptActorDef->tCollData.wcylCollision.wdHeight;
		tCyl->wdRadius = ActorGetCollisionCylinderRadius(ptActorDef);
	}



	if (snoPower != -1)
	{
		if (ptACD)
		{

			SNO snoPowerPassabilityOverride = -1;
			if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_WALK))
			{
				snoPowerPassabilityOverride = ptACD->AttributesGetInt(ATTRIBUTE_WALK_PASSABILITY_POWER_SNO);
			}

			if (snoPowerPassabilityOverride == -1)
			{
				snoPowerPassabilityOverride = ptACD->AttributesGetInt(ATTRIBUTE_PASSABILITY_POWER_SNO);
			}
			if (snoPowerPassabilityOverride != -1)
			{
				snoPower = snoPowerPassabilityOverride;
			}
		}


		ActorCollisionFlags tCollFlags;

		if (PowerGetCollFlags(AutoPowerDefinitionR(snoPower), ePowerPassabilityType, tCollFlags))
		{
			tCollSettings.uDynamicFlagsNoOverlap = tCollFlags.uDynamicFlagsNoOverlap;
			tCollSettings.uDynamicMyFlags = tCollFlags.uDynamicMyFlags;
			tCollSettings.uStaticOneRequiredFlags = tCollFlags.uStaticOneRequiredFlags;
			tCollSettings.uStaticPreventFlags = tCollFlags.uStaticPreventFlags;
		}
	}




	if (ACDIsPlayer(idACD))
	{
		if (GameActiveGameCommonDataIsServer())
		{
			if (SGameGlobalsGet().dwFlags & 0x200)
				tCollSettings.uDynamicFlagsNoOverlap &= ~0x213;
		}
		else if (sgtAppGlobals.dwDebugFlags & 0x8)
		{
			tCollSettings.uDynamicFlagsNoOverlap &= ~0x213;
		}
	}
}
void ActorMovement::PositionChanged(
    const WorldPlace & placeCurr,
    SSceneID idSScene)
{
    NavCellHandle hNavCellPrev = m_hNavCell;

    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
    if (!ptSCD || !ptSCD->ptNavZoneInst)
    {
        m_hNavCell.uCellIndex = (uint32)-1;
        m_hNavCell.idSScene = SSCENEID_INVALID;
    }
    else
    {
        const NavZoneInstance * ptZoneInst = ptSCD->ptNavZoneInst;
        ScopedNavZoneDefLock tZoneDefLocker(ptZoneInst);

        NavCellHandleFromPoint(ptZoneInst, placeCurr.wp.As2D(), m_hNavCell);
    }


    if (hNavCellPrev.idSScene != m_hNavCell.idSScene)
    {
        OnSceneChanged(hNavCellPrev.idSScene, m_hNavCell.idSScene);
    }
}





BOOL ActorMovement::CheckCylinderTeleportBlocker(
	const Vector2D & vStart,
	const Vector2D & vDest,
	float flRadius,
	const ActorCommonData & tACDBlocker) const
{
	const ACDCollSettings & tBlockerCollSettings = tACDBlocker.tCollSettings;
	const AxialCylinder & tCyl = tBlockerCollSettings.GetCyl();






	if (tCyl.wdRadius <= 11.0f) { return TRUE; }




	if (Vector2D::DistanceSquared(vStart, vDest) > 900.0f) { return TRUE; }




	float flCombinedRadius = flRadius + tCyl.wdRadius + 0.000001f;
	float flDistToStartSqr = Vector2D::DistanceSquared(vStart, tCyl.wpBase.As2D());
	if (flDistToStartSqr < flCombinedRadius * flCombinedRadius) { return TRUE; }





	float flDistToDestSqr = Vector2D::DistanceSquared(vDest, tCyl.wpBase.As2D());
	return flDistToDestSqr < flCombinedRadius * flCombinedRadius;
}









BOOL ActorMovement::CheckLineForACDBlockers(
	const Vector2D & vStart,
	const Vector2D & vDest,
	SWorldID idSWorld,
	float flRadius,
	ACDCollParams & tACDCollParams) const
{
	Circle wcStart;  // DWARF inlines Circle::Set(float,float,float) at 2537 (col 24 = vStart.x), but that overload as src defines it is 8 bytes short; the Vector2D overload matches
	wcStart.Set(vStart, flRadius);
	ACDCollBroadResult tBroadResult;
	ACDCollNarrowResult tNarrowResult;
	Vector2D wpBroadCenter = (vStart + vDest) * 0.5f;
	float flBroadRadius = Vector2D::Distance(wpBroadCenter, vStart) + flRadius;
	if (ACDCollBroadCircle(idSWorld, wpBroadCenter, flBroadRadius, tACDCollParams, 0, tBroadResult, ACDID_INVALID))
	{
		if (ACDCollNarrowSweptCircle(tBroadResult, wcStart, vDest, 1, GetACDID(), tNarrowResult))
		{
			if (tNarrowResult.nIndexHit >= 0 &&
				tNarrowResult.nIndexHit < (int32)tBroadResult.uCount)
			{
				ACDID idACDHit = tBroadResult.arACDISect[tNarrowResult.nIndexHit].idACD;
				ActorCommonData * ptACDHit = ACDTryToGet(idACDHit);
				if (ptACDHit)
				{
					const ACDCollSettings & tACDHitCollSettings = ptACDHit->tCollSettings;
					if ((tACDHitCollSettings.uDynamicMyFlags & (1 << COLL_DYN_TELEPORT_BLOCKER)) &&
						tACDHitCollSettings.eCollisionShape == COLLISION_SHAPE_CYLINDER)
					{


						return CheckCylinderTeleportBlocker(vStart, vDest, flRadius, *ptACDHit);
					}
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}







PathResult ActorMovement::FindPath(
	const WorldPlace & placeTarget,
	PathPoint2DList & listPathPoints,
	uint16 uStaticOneRequiredFlags,
	uint16 uStaticPreventFlags,
	ACDCollParams * ptACDCollParams) const
{
	WorldPlace placeCurr;
	float flRadius;
	GetPlaceAndRadius(placeCurr, flRadius, TRUE);

	NavCellHandle hNavCellDest;
	NavCellHandleFromWorldPlace(placeTarget, hNavCellDest);

	PathResult eResult = PATH_RESULT_NOT_FOUND;
	if (m_hNavCell == hNavCellDest)
	{
		eResult = PATH_RESULT_NOT_NECESSARY;
	}
	else
	{
		Vector2D vToDest = placeTarget.wp.As2D() - placeCurr.wp.As2D();
		if (vToDest.MagnitudeSquared() <= 5625.0f)
		{
			NavCellCollideResult tStaticResult;
			BOOL bCollided;
			if (flRadius > 0.01f)
			{
				Circle circleCurr;
				circleCurr.Set(placeCurr.wp.x, placeCurr.wp.y, flRadius);
				bCollided = NavZoneCollideSweptCircle(m_hNavCell, circleCurr, vToDest, uStaticOneRequiredFlags, uStaticPreventFlags, 3, tStaticResult);
			}
			else
			{






				bCollided = NavZoneCollideSweptPoint(m_hNavCell, placeCurr.wp.As2D(), vToDest, uStaticOneRequiredFlags, uStaticPreventFlags, 3, tStaticResult);
			}
			if (!bCollided)
			{
				eResult = PATH_RESULT_NOT_NECESSARY;
			}
		}
	}












	if (eResult == PATH_RESULT_NOT_NECESSARY)
	{
		if (ptACDCollParams && CheckLineForACDBlockers(placeCurr.wp.As2D(), placeTarget.wp.As2D(), placeCurr.idSWorld, flRadius, *ptACDCollParams))
		{
			eResult = PATH_RESULT_NOT_FOUND;
		}
		return eResult;
	}

	BOOL bFoundPath;
	if (hNavCellDest.idSScene != SSCENEID_INVALID)
	{
		NavCellPathFindData tPathFindData;
		NavCellPathFindDataInitialize(tPathFindData);
		tPathFindData.flMaxRangeFromDest = flRadius;
		tPathFindData.uMaxNodesToExplore = 150;
		tPathFindData.uOneRequiredFlags = uStaticOneRequiredFlags;
		tPathFindData.wpPosStart = placeCurr.wp;
		tPathFindData.hNavCellStart = m_hNavCell;
		tPathFindData.wpNavCellEnd = placeTarget.wp;
		tPathFindData.hNavCellEnd = hNavCellDest;
		tPathFindData.uPathSmoothingFlags = 0;
		tPathFindData.uPathSmoothingIterations = 2;
		tPathFindData.ptListPathPoints = &listPathPoints;
		if (NavCellHandleToZoneInstance(tPathFindData.hNavCellStart) && NavCellHandleToZoneInstance(tPathFindData.hNavCellEnd))
		{



			bFoundPath = NavCellFindPath(tPathFindData);
			if (ptACDCollParams && bFoundPath)
			{

				POS pos = listPathPoints.GetHeadPosition();
				Vector2D vLastPoint = placeCurr.wp.As2D();
				while (pos)
				{
					Vector2D & vCurrPoint = listPathPoints.GetNext(pos);
					if (CheckLineForACDBlockers(vCurrPoint, vLastPoint, placeCurr.idSWorld, flRadius, *ptACDCollParams))
					{
						bFoundPath = FALSE;
						break;
					}
					vLastPoint = vCurrPoint;
				}
			}
			if (bFoundPath)
			{
				return PATH_RESULT_FOUND;
			}
		}
	}



	return PATH_RESULT_NOT_FOUND;
}



static const float DYNPATH_DISTANCE_BIAS = 10.0f;

PathResult ActorMovement::FindDynamicPath(const WorldPlace & placeStart, float flRadius, const Vector2D & vDest, Vector2D & vNewDest, const DynPathDirection eDirectionLast, DynPathDirection & eDirectionChosen, const ACDCollSettings & tACDCollSettings) const
{
	float flMaxSearchDistance = flRadius * 10.0f + 25.0f;
	eDirectionChosen = DYNPATH_DIRECTION_NONE;

	float flDistRight;
	float flDistLeft;
	Vector2D vNextNodeRight;
	Vector2D vNextNodeLeft;
	BOOL bPathFoundRight = FindDynPathInDirection(placeStart, flRadius, vDest, flMaxSearchDistance, DYNPATH_DIRECTION_RIGHT, tACDCollSettings, vNextNodeRight, flDistRight);
	BOOL bPathFoundLeft = FindDynPathInDirection(placeStart, flRadius, vDest, flMaxSearchDistance, DYNPATH_DIRECTION_LEFT, tACDCollSettings, vNextNodeLeft, flDistLeft);

	if (!bPathFoundLeft && !bPathFoundRight)
	{
		vNewDest = vDest;
		return PATH_RESULT_NOT_FOUND;
	}

	if (eDirectionLast == DYNPATH_DIRECTION_LEFT && bPathFoundRight)
	{
		flDistRight += DYNPATH_DISTANCE_BIAS;
	}
	else if (eDirectionLast == DYNPATH_DIRECTION_RIGHT && bPathFoundLeft)
	{
		flDistLeft += DYNPATH_DISTANCE_BIAS;
	}

	vNewDest = vNextNodeRight;
	eDirectionChosen = DYNPATH_DIRECTION_RIGHT;
	if (!bPathFoundRight || (bPathFoundLeft && flDistLeft < flDistRight))
	{
		vNewDest = vNextNodeLeft;
		eDirectionChosen = DYNPATH_DIRECTION_LEFT;
	}

	return PATH_RESULT_FOUND;
}




















BOOL ActorMovement::CheckDynPath(const WorldCollParams & tWorldCollParams, const Vector2D & vStart, const Vector2D & vEnd, SWorldID idSWorld, float flRadius, ACDID idACDIgnore, WorldCollResult & tResult) const
{
	Circle wcStart;
	wcStart.wpCenter = vStart;  // original 0x7100652800 (Vector2D copy) and line 2765 radius store come before the placeStart stores (2769-2771)
	wcStart.wdRadius = flRadius;
	NavCellHandle hNavCellStart;
	WorldPlace placeStart;

	placeStart.wp.As2D() = vStart;
	placeStart.wp.z = 0.0f;
	placeStart.idSWorld = idSWorld;

	if (!NavCellHandleFromWorldPlace(placeStart, hNavCellStart))
	{
		return FALSE;
	}

	BOOL bCollision = WorldCollSweptCircle(idSWorld, hNavCellStart, wcStart, vEnd, tWorldCollParams, 3, 1, idACDIgnore, tResult);
	if (bCollision)
	{
		Vector2D vDiff = tResult.wpEnd.As2D() - vStart;
		if (vDiff.MagnitudeSquared() > 0.25f)
		{
			float flLength = vDiff.Normalize();
			flLength -= 0.5f;
			vDiff *= flLength;
			vDiff += vStart;
			tResult.wpEnd.Set(vDiff.x, vDiff.y, 0.0f);
		}
	}

	return !bCollision;
}






















BOOL ActorMovement::FindDynPathInDirection(
    const WorldPlace & placeOrig,
    float flRadius,
    const Vector2D & vFinalDest,
    float flMaxDist,
    DynPathDirection eDirection,
    const ACDCollSettings & tACDCollSettings,
    Vector2D & vFirstNode,
    float & flDistUsed) const
{
    flDistUsed = 0.0f;

    ACDID idACD = GetACDID();
    ActorCommonData *ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return FALSE;
    }

    WorldCollParams tWorldCollParams(tACDCollSettings);

    // inference: the original turns off its own dynamic flags while it sweeps (restored at 2912)
    uint32 uOrigDynamicMyFlags = ptACD->tCollSettings.uDynamicMyFlags;
    ptACD->tCollSettings.uDynamicMyFlags = 0;

    BOOL bSteerPointFound = FALSE;
    Vector2D vCurrPos = placeOrig.wp.As2D();
    Vector2D vNextPos = vFinalDest;
    BOOL bPathFound = FALSE;
    ACDID idACDLastSteerObject = ACDID_INVALID;
    for (int32 i = 0; i < 16; i++)  // inference: 16 iterations (cmp #0xe / b.hi before the increment); no named constant in the DWARF
    {
        if (flDistUsed >= flMaxDist)
        {
            break;
        }

        // inference: tResultFinal and tResultNext have no location in the DWARF
        WorldCollResult tResultFinal;
        BOOL bPathClear = CheckDynPath(tWorldCollParams, vCurrPos, vFinalDest, placeOrig.idSWorld, flRadius, ACDID_INVALID, tResultFinal);
        ActorCommonData *ptACDClosest = ACDTryToGet(tResultFinal.idACDHit);
        if (bPathClear ||
            (ptACDClosest && ptACDClosest->id == GetTargetACD()))
        {

            if (!bSteerPointFound)
            {

                vFirstNode = vFinalDest;
            }

            flDistUsed += Vector2D::Distance(tResultFinal.wpEnd.As2D(), vNextPos);
            bPathFound = TRUE;
            break;
        }



        WorldCollResult * ptResult = &tResultFinal;
        WorldCollResult tResultNext;
        Vector2D vLastPos = vCurrPos;
        bPathClear = CheckDynPath(tWorldCollParams, vCurrPos, vNextPos, placeOrig.idSWorld, flRadius, idACDLastSteerObject, tResultNext);
        if (!bPathClear)
        {

            ptACDClosest = ACDTryToGet(tResultNext.idACDHit);
            ptResult = &tResultNext;
        }

        // inference: one Vector2D copy at 2879 from either source (pointer select in the original)
        vCurrPos = bPathClear ? vNextPos : tResultNext.wpEnd.As2D();


        WorldCollResult tResultToStart;
        if (CheckDynPath(tWorldCollParams, placeOrig.wp.As2D(), vCurrPos, placeOrig.idSWorld, flRadius, ACDID_INVALID, tResultToStart))
        {
            bSteerPointFound = TRUE;
            vFirstNode = vCurrPos;
        }

        flDistUsed += Vector2D::Distance(vLastPos, vCurrPos);

        if (ptACDClosest)
        {
            if (ptACDClosest->tCollSettings.eCollisionShape == COLLISION_SHAPE_BOX)
            {
                FindNextSteerPointOBB(*ptACDClosest, vCurrPos, flRadius, ptResult->wvNormal, eDirection, vNextPos);
            }
            else
            {
                FindNextSteerPointCylinder(*ptACDClosest, vCurrPos, flRadius, eDirection, vNextPos);
            }

            idACDLastSteerObject = ptACDClosest->id;
        }
        else
        {

            FindNextSteerPointStatic(vCurrPos, ptResult->wvNormal.As2D(), eDirection, vNextPos);
            idACDLastSteerObject = ACDID_INVALID;
        }
    }

    ptACD->tCollSettings.uDynamicMyFlags = uOrigDynamicMyFlags;

    return bPathFound;
}



static void sGetBoxCornerPos(
    int32 nCorner,
    const Vector2D & vExt,
    Vector2D & vPos)
{
    if (nCorner < CORNER_BR)
    {
        vPos.y = -vExt.y;
    }
    else
    {
        vPos.y = vExt.y;
    }

    if (nCorner == CORNER_TR || nCorner == CORNER_BR)
    {
        vPos.x = -vExt.x;
    }
    else
    {
        vPos.x = vExt.x;
    }
}




void ActorMovement::FindNextSteerPointOBB(
    const ActorCommonData & tACDOBB,
    const Vector2D & vImpactPos,
    float flRadius,
    const Vector3D & vCollNormal,
    DynPathDirection eDirection,
    Vector2D & vResult) const
{


    const OBB & tOBB = tACDOBB.tCollSettings.GetOBB();
    PRTransform invTrans = tOBB.transform.Inverse();
    Vector2D vCurrPosOBBspace = invTrans * vImpactPos;
    Vector2D vNormPosOBBspace;

    if (tOBB.wvExt.x > EPSILON)
    {
        vNormPosOBBspace.x = vCurrPosOBBspace.x / tOBB.wvExt.x;
    }
    else
    {
        vNormPosOBBspace.x = 0.0f;
    }

    if (tOBB.wvExt.y > EPSILON)
    {
        vNormPosOBBspace.y = vCurrPosOBBspace.y / tOBB.wvExt.y;
    }
    else
    {
        vNormPosOBBspace.y = 0.0f;
    }



    int32 nCorner;


    if (Math::Abs(vNormPosOBBspace.x) > Math::Abs(vNormPosOBBspace.y))
    {



        nCorner = (vNormPosOBBspace.x < 0.0f) ? CORNER_TR : CORNER_BL;




    }
    else
    {



        nCorner = (vNormPosOBBspace.y < 0.0f) ? CORNER_TL : CORNER_BR;




    }

    if (eDirection == DYNPATH_DIRECTION_LEFT)
    {

        ++nCorner;
        if (nCorner > CORNER_BL)
        {
            nCorner = CORNER_TL;
        }
    }

    Vector2D vExt = tOBB.wvExt.As2D();
    vExt.x += flRadius;
    vExt.y += flRadius;

    vExt.x += 1.0f;
    vExt.y += 1.0f;

    Vector2D vNextPos;
    sGetBoxCornerPos(nCorner, vExt, vNextPos);

    const float CLOSE_DISTANCE_TO_WAYPOINT = 0.01f;
    const float CLOSE_DISTANCE_TO_WAYPOINT_SQR = CLOSE_DISTANCE_TO_WAYPOINT * CLOSE_DISTANCE_TO_WAYPOINT;
    if (Vector2D::DistanceSquared(vCurrPosOBBspace, vNextPos) <= CLOSE_DISTANCE_TO_WAYPOINT_SQR)
    {
        if (eDirection == DYNPATH_DIRECTION_RIGHT)
        {
            nCorner = (nCorner < CORNER_TR) ? CORNER_BL : nCorner - 1;
        }
        else
        {








            nCorner = (nCorner > CORNER_BR) ? CORNER_TL : nCorner + 1;
        }




        sGetBoxCornerPos(nCorner, vExt, vNextPos);
    }

    vResult = tOBB.transform * vNextPos;
}














void ActorMovement::FindNextSteerPointCylinder(const ActorCommonData & tACDCyl, const Vector2D & vImpactPos, float flRadius, DynPathDirection eDirection, Vector2D & vResult) const
{
	WorldPlace placeCyl = tACDCyl.GetPlace();

	Vector3D vCurrPos(vImpactPos.x, vImpactPos.y, placeCyl.wp.z);
	Vector3D vToCyl = placeCyl.wp - vCurrPos;
	vToCyl.z = 0.0f;


	if (vToCyl.MagnitudeSquared() < EPSILON * EPSILON)
	{
		vToCyl = cgwvXAxis;
	}
	else
	{
		vToCyl.Normalize();
	}




	float flRadiusCyl = tACDCyl.GetRadius();
	float flCombinedRadius = flRadiusCyl + flRadius + 1.0f;


	if (flRadiusCyl > 1.0f)
	{
		float flUnitMod = flRadiusCyl - 1.0f;
		flUnitMod *= (1.0f / 9.0f);

		flCombinedRadius += Math::Pin(flUnitMod, 0.0f, 1.0f) * 10.0f;
	}


	Vector3D vRight = Vector3D::CrossProduct(vToCyl, cgwvZAxis);
	vToCyl *= flCombinedRadius;
	vRight *= flCombinedRadius * 0.5f;
	Vector3D vBasePos = placeCyl.wp - vToCyl;
	vResult = (eDirection == DYNPATH_DIRECTION_LEFT) ? (vBasePos + -vRight).As2D() : (vBasePos + vRight).As2D();
}







void ActorMovement::FindNextSteerPointStatic(
    const Vector2D & vImpactPos,
    const Vector2D & vImpactNormal,
    DynPathDirection eDirection,
    Vector2D & vResult) const
{
    Vector3D vNormal(vImpactNormal.x, vImpactNormal.y, 0.0f);
    Vector3D vLeft = Vector3D::CrossProduct(vNormal, cgwvZAxis);
    vLeft *= 5.0f;
    if (eDirection == DYNPATH_DIRECTION_LEFT)
    {
        vResult = vImpactPos + vLeft.As2D();
    }
    else
    {
        vResult = vImpactPos - vLeft.As2D();
    }
}



void ActorMovement::SnapToNewPlace(const WorldPlace & placeNew, BOOL bSnapToGround, BOOL bSoftSnap, BOOL bSendToClient, uint32 uCameraSmoothingTicks)
{
	m_uLastStampTime = 0;

	if (bSnapToGround)
	{
		WorldPlace placeSnapped(placeNew);
		SnapPlaceToGround(placeSnapped);
		SetNewPlace(placeSnapped, TRUE, FALSE, bSendToClient);
	}
	else
	{
		SetNewPlace(placeNew, TRUE, FALSE, bSendToClient);
	}

	StopNow();
}











BOOL ActorMovement::HasFailedDynamicPath()
{
	return m_bFailedToFindDynamicPath;
}




void ActorMovement::OnSceneChanged(SSceneID idSSceneOld, SSceneID idSSceneNew)
{
}







void ActorMovement::StartDetPath(DeterministicPathType eDPathType,
	const Vector3D & vVel,
	uint32 uMoveFlags,
	ActorAnimationTag eAnimTag,
	SNO snoPowerPassability,
	Angle aOriginalYaw,
	const Vector3D & vStartPos,
	const RNG & tSeed,
	int32 nVar0,
	float flVar0,
	float flVar1,
	float flVar2,
	float flVar3,
	float flVar4,
	float flSpeedMult)
{
	if (IsServerProjectile())
	{
		uMoveFlags |= (1 << MOVEBIT_IS_PROJECTILE);
	}

	m_tDPath.eDPath = eDPathType;
	m_flDesiredSpeed = vVel.Magnitude();
	m_flCurrentSpeed = m_flDesiredSpeed;
	m_uMoveFlags = uMoveFlags;
	m_eAnimTag = eAnimTag;
	m_vVel = vVel;
	m_vLastVel = cgwvXAxis;
	m_snoPowerPassability = snoPowerPassability;
	m_tDPath.aOriginalYaw = aOriginalYaw;
	m_tDPath.vStartPos = vStartPos;
	m_tDPath.tSeed = tSeed;
	m_tDPath.nVar0 = nVar0;
	m_tDPath.flVar0 = flVar0;
	m_tDPath.flVar1 = flVar1;
	m_tDPath.flVar2 = flVar2;
	m_tDPath.flVar3 = flVar3;
	m_tDPath.flVar4 = flVar4;
	m_tDPath.flSpeedMult = flSpeedMult;

	m_eMovementType = MOVEMENT_TYPE_DETPATH;
	m_placeDest = cgplaceNull;
	m_placeStart = cgplaceNull;
	m_vInitialVelocity = cgwvNull;

	m_flMaxRangeFromDest = 0.0f;
	m_flUnitBounce = 0.0f;
	m_idACDTarget = ACDID_INVALID;
	m_flGravity = 0.0f;

	Angle aYaw = Math::ATan(m_vVel.y, m_vVel.x);
	SetNewFacing(aYaw, FALSE, TRUE);

	DetPathStart(*this, m_tDPath);
}




void ActorMovement::StartDetPathSinWave(DeterministicPathType eDPathType,
	const Vector3D & vVel,
	uint32 uMoveFlags,
	ActorAnimationTag eAnimTag,
	SNO snoPowerPassability,
	Angle aOriginalYaw,
	const Vector3D & vStartPos,
	const RNG & tSeed,
	int32 nVar0,
	float flVar0,
	const DPathSinData & tSinData,
	float flSpeedMult)
{
	m_tDPath.tSinData = tSinData;
	StartDetPath(eDPathType, vVel, uMoveFlags, eAnimTag, snoPowerPassability, aOriginalYaw, vStartPos, tSeed, nVar0, flVar0, 0.0f, 0.0f, 0.0f, 0.0f, flSpeedMult);
}



















void ActorMovement::StartDetPathSpiral(DeterministicPathType eDPathType,
	const Vector3D & vStartPos,
	const Vector3D & vTargetPos,
	uint32 uMoveFlags,
	ActorAnimationTag eAnimTag,
	float flVar0,
	float flVar1,
	const DPathSinData & tSinData,
	float flSpeedMult)
{
	m_tDPath.tSinData = tSinData;
	m_tDPath.vTargetPos = vTargetPos;
	StartDetPath(eDPathType, cgwvNull, uMoveFlags, eAnimTag, -1, 0.0f, vStartPos, m_tDPath.tSeed, 0, flVar0, flVar1, 0.0f, 0.0f, 0.0f, flSpeedMult);
}



















void ActorMovement::StartDetPathPieWedge(const Vector3D & vStartPos,
	const Vector3D & vFirstTargetPos,
	uint32 uMoveFlags,
	ActorAnimationTag eAnimTag,
	const DPathPieData & tPieData,
	float flStartRunTime,
	float flSpeedMult)
{
	m_tDPath.vTargetPos = vFirstTargetPos;
	m_tDPath.tPieData = tPieData;
	StartDetPath(DPATH_PIE_WEDGE, cgwvNull, uMoveFlags, eAnimTag, -1, 0.0f, vStartPos, m_tDPath.tSeed, 0, 0.0f, 0.0f, 0.0f, flStartRunTime, 0.0f, flSpeedMult);
}



















Vector3D ActorMovement::GetVelocity() const
{
	Vector3D vVel(m_vVel);

	if (IsServerProjectile() && m_eMovementType != MOVEMENT_TYPE_DETPATH)
	{
		ACDID idACD = GetACDID();
		float flProjSpeedMult = ProjectileGetSpeedMultiplier(idACD);
		vVel *= flProjSpeedMult;
	}

	sCapVelocityToReasonableValue(vVel);
	return vVel;
}







inline BOOL ActorMovement::ProjCollisionResponse(ACDID idACDObject)
{

	ImpactData tImpactData(GetACDID());
	tImpactData.idACDIntersected = idACDObject;

	return ProjCollisionResponse(tImpactData);
}



inline Vector3D ActorMovement::Reflect(const Vector3D & vVel, const Vector3D & vNormal)
{
	// Reflect the velocity about the normal.
	Vector3D vReflectedVel = -vVel;
	float flProjLength = Vector3D::DotProduct(vReflectedVel, vNormal);
	vReflectedVel = ((vNormal * flProjLength)
		- vReflectedVel)
		* 2.0f + vReflectedVel;
	return vReflectedVel;
}







void ActorMovement::GetCurrentInterpolatedPos(Vector3D & vPos) const
{
	WorldPlace placeCurr;	GetPlace(placeCurr);

	uint32 uNow = GetTimeNow();
	if (m_uLastStampTime != 0 && uNow >= m_uLastStampTime &&

		placeCurr.idSWorld == m_placeLastStamp.idSWorld)
	{
		float flTicks = (float)(uNow - m_uLastStampTime);
		if (m_eMovementType == MOVEMENT_TYPE_FIXED_PROJECTILE || m_eMovementType == MOVEMENT_TYPE_FIXED_PROJECTILE_COLLIDING)
		{
			Vector3D vVel = GetVelocity();
			Vector3D vDelta = flTicks * vVel;
			vPos = vDelta + m_placeLastStamp.wp;
			return;
		}

		float flDistInterpolated = m_flLastStampSpeed * flTicks;
		if (flDistInterpolated > EPSILON)
		{
			Vector3D vToTarget = placeCurr.wp - m_placeLastStamp.wp;
			if (vToTarget.MagnitudeSquared() > flDistInterpolated * flDistInterpolated)
			{
				vToTarget.Normalize();
				vToTarget *= flDistInterpolated;
				vPos = m_placeLastStamp.wp + vToTarget;
				return;
			}
		}
	}

	vPos = placeCurr.wp;
}























































































































































































































































































































static const TypeDescriptorField ACDTranslateNormalMessage_Fields[14] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDTranslateNormalMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_DATAID, offsetof(ACDTranslateNormalMessage, ann), NULL, 1),
    OptionalTypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateNormalMessage, vDest), NULL, 1),
    OptionalTypeDescriptorField("", DT_ANGLE, offsetof(ACDTranslateNormalMessage, aDesiredYaw), NULL, 1),
    OptionalBoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateNormalMessage, bSnapFacing), NULL, NULL, (const void *)1),
    OptionalTypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateNormalMessage, flMovementSpeed), NULL, 1),
    OptionalBoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateNormalMessage, uMoveFlags), NULL, NULL, (const void *)0x3ffffff),
    OptionalBoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateNormalMessage, eAnimTag), NULL, (const void *)-1, (const void *)0xfffff),
    OptionalTypeDescriptorField("", DT_INT, offsetof(ACDTranslateNormalMessage, uWalkInPlaceTicks), NULL, 1),
    OptionalTypeDescriptorField("", DT_SNO, offsetof(ACDTranslateNormalMessage, snoPowerPassability), NULL, 1),
    OptionalBoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateNormalMessage, uAckValue), NULL, NULL, (const void *)0xffff),
    OptionalTypeDescriptorField("", DT_INT, offsetof(ACDTranslateNormalMessage, eMoveType), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(ACDTranslateNormalMessage, nMaxTicksToWalk), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDTranslateNormalMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_ACDTranslateNormalMessage("ACDTranslateNormalMessage", 0, ACDTranslateNormalMessage_Fields, 14, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_TRANSLATE_NORMAL_MESSAGE = &TypeDescriptor_ACDTranslateNormalMessage;

static const TypeDescriptorField ACDTranslateSnappedMessage_Fields[9] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDTranslateSnappedMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_DATAID, offsetof(ACDTranslateSnappedMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateSnappedMessage, vDest), NULL, 1),
    TypeDescriptorField("", DT_ANGLE, offsetof(ACDTranslateSnappedMessage, aDesiredYaw), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateSnappedMessage, bSnapFacing), NULL, NULL, (const void *)1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateSnappedMessage, uMoveFlags), NULL, NULL, (const void *)0x3ffffff),
    OptionalBoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateSnappedMessage, uAckValue), NULL, NULL, (const void *)0xffff),
    OptionalTypeDescriptorField("", DT_INT, offsetof(ACDTranslateSnappedMessage, uCameraSmoothingTime), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDTranslateSnappedMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_ACDTranslateSnappedMessage("ACDTranslateSnappedMessage", 0, ACDTranslateSnappedMessage_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_TRANSLATE_SNAPPED_MESSAGE = &TypeDescriptor_ACDTranslateSnappedMessage;

static const TypeDescriptorField ACDTranslateSyncMessage_Fields[6] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDTranslateSyncMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_DATAID, offsetof(ACDTranslateSyncMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateSyncMessage, vPos), NULL, 1),
    OptionalBoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateSyncMessage, bSnap), NULL, NULL, (const void *)1),
    OptionalBoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateSyncMessage, uAckValue), NULL, NULL, (const void *)0xffff),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDTranslateSyncMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_ACDTranslateSyncMessage("ACDTranslateSyncMessage", 0, ACDTranslateSyncMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_TRANSLATE_SYNC_MESSAGE = &TypeDescriptor_ACDTranslateSyncMessage;

static const TypeDescriptorField ACDTranslateAckMessage_Fields[3] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDTranslateAckMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDTranslateAckMessage, uAckValue), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDTranslateAckMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_ACDTranslateAckMessage("ACDTranslateAckMessage", 0, ACDTranslateAckMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_TRANSLATE_ACK_MESSAGE = &TypeDescriptor_ACDTranslateAckMessage;

static const TypeDescriptorField ACDTranslateFacingMessage_Fields[5] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDTranslateFacingMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_DATAID, offsetof(ACDTranslateFacingMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_ANGLE, offsetof(ACDTranslateFacingMessage, aDesiredYaw), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateFacingMessage, bSnapFacing), NULL, NULL, (const void *)1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDTranslateFacingMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_ACDTranslateFacingMessage("ACDTranslateFacingMessage", 0, ACDTranslateFacingMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_TRANSLATE_FACING_MESSAGE = &TypeDescriptor_ACDTranslateFacingMessage;

static const TypeDescriptorField ACDTranslateFixedMessage_Fields[7] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDTranslateFixedMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_DATAID, offsetof(ACDTranslateFixedMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateFixedMessage, vVel), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateFixedMessage, uMoveFlags), NULL, NULL, (const void *)0x3ffffff),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateFixedMessage, eAnimTag), NULL, (const void *)-1, (const void *)0xfffff),
    TypeDescriptorField("", DT_SNO, offsetof(ACDTranslateFixedMessage, snoPowerPassability), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDTranslateFixedMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_ACDTranslateFixedMessage("ACDTranslateFixedMessage", 0, ACDTranslateFixedMessage_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_TRANSLATE_FIXED_MESSAGE = &TypeDescriptor_ACDTranslateFixedMessage;

static const TypeDescriptorField ACDTranslateArcMessage_Fields[12] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDTranslateArcMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_DATAID, offsetof(ACDTranslateArcMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateArcMessage, vPos), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateArcMessage, vVel), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateArcMessage, uMoveFlags), NULL, NULL, (const void *)0x3ffffff),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateArcMessage, eAnimTag), NULL, (const void *)-1, (const void *)0xfffff),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateArcMessage, eLandingAnimTag), NULL, (const void *)-1, (const void *)0xfffff),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateArcMessage, flGravity), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(ACDTranslateArcMessage, snoPowerPassability), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateArcMessage, flUnitBounce), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateArcMessage, flDesiredHeight), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDTranslateArcMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_ACDTranslateArcMessage("ACDTranslateArcMessage", 0, ACDTranslateArcMessage_Fields, 12, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_TRANSLATE_ARC_MESSAGE = &TypeDescriptor_ACDTranslateArcMessage;

static const TypeDescriptorField ACDTranslateDetPathMessage_Fields[19] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDTranslateDetPathMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_DATAID, offsetof(ACDTranslateDetPathMessage, ann), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathMessage, eDPath), NULL, NULL, (const void *)0xd),
    TypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathMessage, tSeed), NULL, 1),
    TypeDescriptorField("", DT_INT, 0x14, NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateDetPathMessage, wvVelocity), NULL, 1),
    TypeDescriptorField("", DT_ANGLE, offsetof(ACDTranslateDetPathMessage, aOriginalYaw), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateDetPathMessage, vStartPos), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathMessage, uMoveFlags), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathMessage, eAnimTag), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(ACDTranslateDetPathMessage, snoPowerPassability), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathMessage, nVar0), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateDetPathMessage, flVar0), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateDetPathMessage, flVar1), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateDetPathMessage, flVar2), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateDetPathMessage, flVar3), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateDetPathMessage, flVar4), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateDetPathMessage, flSpeedMult), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDTranslateDetPathMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_ACDTranslateDetPathMessage("ACDTranslateDetPathMessage", 0, ACDTranslateDetPathMessage_Fields, 19, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_TRANSLATE_DETPATH_MESSAGE = &TypeDescriptor_ACDTranslateDetPathMessage;

static const TypeDescriptorField DPathSinData_Fields[8] = { TypeDescriptorField("", DT_FLOAT, offsetof(DPathSinData, flSinIncPerTick), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DPathSinData, flSinIncAccel), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DPathSinData, flLateralMaxDistance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DPathSinData, flOOLateralDistanceToScale), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DPathSinData, flLateralStartDistance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DPathSinData, flSpeed), NULL, 1),
    TypeDescriptorField("", DT_DATAID, offsetof(DPathSinData, annOwner), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DPathSinData), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_DPathSinData("DPathSinData", 0, DPathSinData_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_DETPATH_SINDATA = &TypeDescriptor_DPathSinData;

static const TypeDescriptorField DPathPieData_Fields[5] = { TypeDescriptorField("", DT_VECTOR3D, offsetof(DPathPieData, vTargetPos2), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DPathPieData, flStraightSpeed), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DPathPieData, flArcSpeed), NULL, 1),
    TypeDescriptorField("", DT_DATAID, offsetof(DPathPieData, annOwner), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DPathPieData), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_DPathPieData("DPathPieData", 0, DPathPieData_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_DETPATH_PIEDATA = &TypeDescriptor_DPathPieData;

static const TypeDescriptorField ACDTranslateDetPathSinMessage_Fields[16] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDTranslateDetPathSinMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_DATAID, offsetof(ACDTranslateDetPathSinMessage, ann), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathSinMessage, eDPath), NULL, NULL, (const void *)0xd),
    TypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathSinMessage, tSeed), NULL, 1),
    TypeDescriptorField("", DT_INT, 0x14, NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateDetPathSinMessage, wvVelocity), NULL, 1),
    TypeDescriptorField("", DT_ANGLE, offsetof(ACDTranslateDetPathSinMessage, aOriginalYaw), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateDetPathSinMessage, vStartPos), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathSinMessage, uMoveFlags), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathSinMessage, eAnimTag), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(ACDTranslateDetPathSinMessage, snoPowerPassability), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathSinMessage, nVar0), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateDetPathSinMessage, flVar0), NULL, 1),
    TypeDescriptorField("", DT_DETPATH_SINDATA, offsetof(ACDTranslateDetPathSinMessage, tSinData), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateDetPathSinMessage, flSpeedMult), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDTranslateDetPathSinMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_ACDTranslateDetPathSinMessage("ACDTranslateDetPathSinMessage", 0, ACDTranslateDetPathSinMessage_Fields, 16, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_TRANSLATE_DETPATH_SIN_MESSAGE = &TypeDescriptor_ACDTranslateDetPathSinMessage;

static const TypeDescriptorField ACDTranslateDetPathSpiralMessage_Fields[12] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDTranslateDetPathSpiralMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_DATAID, offsetof(ACDTranslateDetPathSpiralMessage, ann), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathSpiralMessage, eDPath), NULL, NULL, (const void *)0xd),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateDetPathSpiralMessage, vStartPos), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateDetPathSpiralMessage, vTargetPos), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathSpiralMessage, uMoveFlags), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathSpiralMessage, eAnimTag), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateDetPathSpiralMessage, flVar0), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateDetPathSpiralMessage, flVar1), NULL, 1),
    TypeDescriptorField("", DT_DETPATH_SINDATA, offsetof(ACDTranslateDetPathSpiralMessage, tSinData), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateDetPathSpiralMessage, flSpeedMult), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDTranslateDetPathSpiralMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_ACDTranslateDetPathSpiralMessage("ACDTranslateDetPathSpiralMessage", 0, ACDTranslateDetPathSpiralMessage_Fields, 12, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_TRANSLATE_DETPATH_SPIRAL_MESSAGE = &TypeDescriptor_ACDTranslateDetPathSpiralMessage;

static const TypeDescriptorField ACDTranslateDetPathPieWedgeMessage_Fields[9] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDTranslateDetPathPieWedgeMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_DATAID, offsetof(ACDTranslateDetPathPieWedgeMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateDetPathPieWedgeMessage, vStartPos), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateDetPathPieWedgeMessage, vFirstTargetPos), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathPieWedgeMessage, uMoveFlags), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDTranslateDetPathPieWedgeMessage, eAnimTag), NULL, 1),
    TypeDescriptorField("", DT_DETPATH_PIEDATA, offsetof(ACDTranslateDetPathPieWedgeMessage, tPieData), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDTranslateDetPathPieWedgeMessage, flSpeedMult), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDTranslateDetPathPieWedgeMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_ACDTranslateDetPathPieWedgeMessage("ACDTranslateDetPathPieWedgeMessage", 0, ACDTranslateDetPathPieWedgeMessage_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_TRANSLATE_DETPATH_PIE_WEDGE_MESSAGE = &TypeDescriptor_ACDTranslateDetPathPieWedgeMessage;

static const TypeDescriptorField ACDTranslateFixedUpdateMessage_Fields[5] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDTranslateFixedUpdateMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_DATAID, offsetof(ACDTranslateFixedUpdateMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateFixedUpdateMessage, vPos), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateFixedUpdateMessage, vVel), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDTranslateFixedUpdateMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_ACDTranslateFixedUpdateMessage("ACDTranslateFixedUpdateMessage", 0, ACDTranslateFixedUpdateMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_TRANSLATE_FIXED_UPDATE_MESSAGE = &TypeDescriptor_ACDTranslateFixedUpdateMessage;

static const TypeDescriptorField ACDTranslateStoppedMessage_Fields[4] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDTranslateStoppedMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_DATAID, offsetof(ACDTranslateStoppedMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDTranslateStoppedMessage, vPos), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDTranslateStoppedMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_ACDTranslateStoppedMessage("ACDTranslateStoppedMessage", 0, ACDTranslateStoppedMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_TRANSLATE_STOPPED_MESSAGE = &TypeDescriptor_ACDTranslateStoppedMessage;

static const TypeDescriptorField ACDClientTranslateMessage_Fields[10] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDClientTranslateMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDClientTranslateMessage, uTickBase), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDClientTranslateMessage, uTicksAhead), NULL, NULL, (const void *)0xc),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ACDClientTranslateMessage, vDest), NULL, 1),
    TypeDescriptorField("", DT_ANGLE, offsetof(ACDClientTranslateMessage, aDesiredYaw), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDClientTranslateMessage, flMovementSpeed), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDClientTranslateMessage, uMoveFlags), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDClientTranslateMessage, eAnimTag), NULL, (const void *)-1, (const void *)0xfffff),
    OptionalTypeDescriptorField("", DT_SNO, offsetof(ACDClientTranslateMessage, snoPower), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDClientTranslateMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_ACDClientTranslateMessage("ACDClientTranslateMessage", 0, ACDClientTranslateMessage_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_CLIENT_TRANSLATE_MESSAGE = &TypeDescriptor_ACDClientTranslateMessage;
