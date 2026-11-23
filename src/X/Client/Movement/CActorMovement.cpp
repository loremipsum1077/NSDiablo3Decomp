#include "CActorMovement.h"
#include "../RActors/RActorAnimation.h"
#include "../CWorld.h"
#include "../../Common/AnimCommon.h"
#include "../../Common/Definitions/AnimTreeDefinition.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/Marker/Marker.h"
#include "../../Sigma/RefString.h"

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../CGame.h"
#include "../Console/LocalPlayerClamping.h"
#include "../RActors/RActors.h"
#include "../RActors/RActorAttachments.h"
#include "../Attachments.h"
#include "../CPlayer.h"
#include "../../Common/Player.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../Physics/DominoClient.h"
#include "../../Common/Definitions/AnimationDefinition.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/MemoryPools.h"

BOOL NavMeshGetHeightAndNormal(const NavMeshHandle & tNavMeshHandle, const Vector3D & wp, const BOOL fPinToSquareBounds, WorldDistance & flHeight, Vector3D & wvNormal);  // X/Common/Nav/NavigationMesh.cpp:783

void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp

BOOL RActorAnimationIsAnimTreeEnabled(const RActor & tRActor);  // X/Client/RActors/RActorAnimation.cpp:1962
BOOL RActorAnimTreeLayerIsActive(const RActor & tRActor, AnimLayer eAnimLayer);  // X/Client/RActors/RActorAnimation.cpp:2645
void RActorAnimationUpdateAnimTreeState(RActor & tRActor, float flSpeed, Angle aYawSpeed, BOOL bTurn180Trigger, BOOL bTurn180Active, float flTurn180AnimScale, BOOL bAimEnabled, const Vector3D & wpAimTarget);  // X/Client/RActors/RActorAnimation.cpp:2338
float CPlayerGetMoveSpeedForStickInput(ActorCommonData & tACDPlayer, Player & tPlayer, SNO snoPower, float flMinClientWalkSpeed);  // X/Client/CPlayerInput.cpp:3255
void RActorAnimationScaleToVelocity(RActor & tRActor, const WorldDistance wdMag);  // X/Client/RActors/RActorAnimation.cpp:1521
void RActorAnimTreeLayerScaleToVelocity(RActor & tRActor, AnimLayer eAnimLayer, const WorldDistance wdMag);  // X/Client/RActors/RActorAnimation.cpp:2762
uint32 AnimPlayerPlayAnimationsWithTiming(ActorCommonData & tACD, AnimationTiming * ptAnimationTiming, int32 nNumAnims, BOOL bUseAnimationTranslation, AnimReason eAnimReason, BOOL fAnimWasPreplayed, BOOL bDontSendToClient);  // X/Common/AnimPlayer.cpp:1119
uint32 AnimPlayerGetCurrentAnimTag(ActorCommonData & tACD);  // X/Common/AnimPlayer.cpp:1856
uint32 AnimPlayerGetWalkCoolDownTag(ActorCommonData & tACD, uint32 dwAnimTag);  // X/Common/AnimPlayer.cpp
void RActorsIntersectSphere(const Sphere & tSphere, RActorID idRActorIgnore, RActorIDListPooled & listRActors);  // X/Client/RActors/RActors.cpp
BOOL RActorGetAnimLayerForTag(RActor * ptRActor, uint32 dwAnimationTag, AnimLayer & eAnimLayer);  // X/Client/RActors/RActorAnimation.cpp:2087
void AnimPlayerPlayIdle(ActorCommonData & tACD);  // X/Common/AnimPlayer.cpp:1376
void CPlayerInputClearCachedPower(Player * ptPlayer);  // X/Client/CPlayerInput.cpp:7208
void CPlayerInputClearClientWalkPower(Player * ptPlayer);  // X/Client/CPlayerInput.cpp:7217
void RActorInternalSetCurrentFacing(RActor * ptRActor, Angle aYaw);  // X/Client/RActors/RActors.cpp:6314
void RActorInternalSetPosition(RActor * ptRActor, const Vector3D & wpPos, BOOL bAddWalkingEffects, SSceneID idSScene);  // X/Client/RActors/RActors.cpp:6192
extern DominoWorld * gptPhysicsAPIImpl;
RActorID RActorFindByANN(const ACDNetworkName idANN);  // X/Client/RActors/RActors.cpp:6546
namespace LocalPlayerClamping { void ClearInactiveFlagForController(int32 nControllerIndex, ActivityReason eReason); }  // LocalPlayerClamping.cpp:278
void AnimationGetAvgVelocity(const AnimationDefinition * pdefAnim, const int32 nPermutationIndex, const float flTranslationScale, const BOOL bMultipleBySpeedScale, Vector3D & wvVel);  // X/Client/Animation.cpp
WorldDistance AnimGetWalkingSpeed(const AnimationDefinition * pdefAnim, float flSpeedFactor);  // X/Common/AnimPlayer.cpp

struct CActorMovementGameGlobals {  // size 0x30
    XChainedFixedBlockAllocator<CActorMovement> poolCActorMovement;  // 0x00
};


void CActorMovementInitializeForGame()
{
    CActorMovementGameGlobals *& ptCActorMovementGlobals = CGameGet().ptCActorMovementGlobals;
    ptCActorMovementGlobals = (CActorMovementGameGlobals *)SigmaMemoryNew(sizeof(CActorMovementGameGlobals), 4, NULL, TRUE);
    ptCActorMovementGlobals->poolCActorMovement.Initialize();
    ptCActorMovementGlobals->poolCActorMovement.SetBlockCount_Tracked(1024);
}




void CActorMovementDisposeForGame()
{
    CActorMovementGameGlobals *& ptCActorMovementGlobals = CGameGet().ptCActorMovementGlobals;
    ptCActorMovementGlobals->poolCActorMovement.Dispose();
    SigmaMemoryFree(ptCActorMovementGlobals, NULL);
    ptCActorMovementGlobals = NULL;
}



BOOL CActorMovementInitialize(RActor * ptRActor, const WorldPlace & placeStart)
{
    CActorMovementGameGlobals * ptCActorMovementGlobals = CGameGet().ptCActorMovementGlobals;

    CActorMovement * ptCActorMovement = ptCActorMovementGlobals->poolCActorMovement.Alloc_Tracked();
    BOOL bInitialized = FALSE;
    if (ptCActorMovement)
    {
        if (!ptCActorMovement->Initialize(ptRActor->id, placeStart))
        {
            ptCActorMovementGlobals->poolCActorMovement.Free(ptCActorMovement);
            return FALSE;
        }
        bInitialized = TRUE;
        ptCActorMovement->SetUpdateInterval(1);
        ptRActor->ptCActorMovement = ptCActorMovement;
        ActorSteer & tSteer = ptCActorMovement->GetSteer();
        tSteer.SetUnitSteer(0.0f);
        ptCActorMovement->SetIgnoreCollision(TRUE);
    }
    return bInitialized;
}
















void CActorMovementDispose(RActor * ptRActor)
{
    if (ptRActor->ptCActorMovement)
    {
        CActorMovementGameGlobals * ptCActorMovementGlobals = CGameGet().ptCActorMovementGlobals;
        ptRActor->ptCActorMovement->Dispose();
        ptCActorMovementGlobals->poolCActorMovement.Free(ptRActor->ptCActorMovement);
        ptRActor->ptCActorMovement = NULL;
    }
}








inline CActorMovement::~CActorMovement()
{
}




BOOL CActorMovement::Initialize(RActorID idRActor, const WorldPlace & placeStart)
{
    if (idRActor == RACTORID_INVALID)
        return FALSE;

    RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
        return FALSE;

    m_idRActor = idRActor;
    m_uNextClientUpdateTime = 0;
    m_uNextPositionSendTime = 0;
    m_uMoveStopTime = 0;
    m_uNextBounceTime = 0;
    m_aOldYaw = m_aDesiredYaw = ptRActor->aYaw;
    m_aYawVelocity = 0.0f;
    m_bTurn180 = FALSE;
    m_bTurn180Trigger = FALSE;
    m_flTurn180AnimScale = 1.0f;





    m_vOldPlace = placeStart.wp;
    m_aNewYaw = m_aOldYaw;
    m_vNewPlace = placeStart.wp;
    m_flNewPlaceTime = 0.0f;
    m_flNewYawTime = 0.0f;
    m_flCurrentCamHeight = 0.0f;
    m_uCamStartTime = 0;
    m_uCamEndTime = 0;
    m_vCamStartPos = cgwvNull;





    m_vCachedDest = cgwvNull;
    m_flCachedMovementSpeed = 0.0f;
    m_aCachedDesiredYaw = 0.0f;
    m_uCachedMoveFlags = 0;
    m_uCachedAnimTag = (uint32)-1;
    m_vCachedPositionSent.x = 100000.0f;
    m_vCachedPositionSent.y = 100000.0f;
    m_vCachedPositionSent.z = 100000.0f;

    return ActorMovement::Initialize(placeStart);
}
















void CActorMovement::Dispose()
{
    ActorMovement::Dispose();
}








void CActorMovement::GetPlaceAndRadius(WorldPlace & tPlace, float & flRadius, BOOL bGetMovingRadius) const
{
    const RActor * ptRActor = RActorGet(m_idRActor);
    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (ptACD)
    {
        tPlace.idSWorld = ptACD->tLocation.tWorldLocation.idSWorld;
        tPlace.wp = ptACD->tLocation.tWorldLocation.transform.wp;
    }
    else
    {
        tPlace.idSWorld = ptRActor->tLocation.tWorldLocation.idSWorld;
        tPlace.wp = ptRActor->wpWalkingPos;
    }
    flRadius = GetRadius(bGetMovingRadius);
}

















float CActorMovement::GetRadius(BOOL bGetMovingRadius) const
{
    float flRadius;
    const RActor * ptRActor = RActorGet(m_idRActor);
    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (ptACD)
    {
        if (ptACD->tCollSettings.eCollisionShape == 1)
        {
            const AxialCylinder & tCyl = *(const AxialCylinder *)ptACD->tCollSettings.arAxialCylinderBytes;
            flRadius = tCyl.wdRadius;
        }
        else
        {
            flRadius = ptACD->tLocation.tWorldLocation.wcylBounds.wdRadius;
        }
    }
    else
    {
        flRadius = ptRActor->tLocation.tWorldLocation.wcylBounds.wdRadius;
    }
    if (bGetMovingRadius)
        flRadius *= m_flRadiusScalar;
    return flRadius;
}


















void CActorMovement::GetPlace(WorldPlace & tPlace) const
{
    const RActor * ptRActor = RActorGet(m_idRActor);
    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (ptACD)
    {
        tPlace.idSWorld = ptACD->tLocation.tWorldLocation.idSWorld;
        tPlace.wp = ptACD->tLocation.tWorldLocation.transform.wp;
    }
    else
    {
        tPlace.idSWorld = ptRActor->tLocation.tWorldLocation.idSWorld;
        tPlace.wp = ptRActor->wpWalkingPos;
    }
}















SSceneID CActorMovement::GetSSceneID() const
{
    SSceneID idSScene;
    const RActor * ptRActor = RActorGet(m_idRActor);
    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (ptACD)
        idSScene = ACDGetCurrentSSceneID(*ptACD);
    else if (ptRActor->tLocation.eActorLocation == 0)
        idSScene = ptRActor->tLocation.tWorldLocation.idSScene;
    else
        idSScene = SSCENEID_INVALID;
    return idSScene;
}











ACDID CActorMovement::GetACDID() const
{
    RActor * ptRActor = RActorTryToGet(m_idRActor);
    if (ptRActor)
        return ptRActor->idACD;
    return ACDID_INVALID;
}










void CActorMovement::QueueUpdate(uint32 uDesiredTickDelay)
{
    uint32 uNow = CGameGetClientTicks();
    uint32 uDesiredTime = uNow + uDesiredTickDelay;
    if (m_uNextClientUpdateTime > uNow)
    {
        uDesiredTime = Math::Min(m_uNextClientUpdateTime, uDesiredTime);
    }
    m_uNextClientUpdateTime = uDesiredTime;
}









void CActorMovement::SetNewPlace(const WorldPlace & placeNew, BOOL bSnap, BOOL bClientAuth, BOOL bSendToClient)
{
    RActor * ptRActor = RActorGet(m_idRActor);

    if (Math::Abs(placeNew.wp.x) >= 99000.0f ||
        Math::Abs(placeNew.wp.y) >= 99000.0f ||
        Math::Abs(placeNew.wp.z) >= 99000.0f)
    {
        RActorFlagForDeletion(ptRActor);
        return;
    }

    SSceneID idSScene = SceneCommonGetSceneIDFromWorldPlace(placeNew, FALSE);
    BOOL bSetRActorPosition = TRUE;
    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (ptACD)
    {
        WorldLocation & loc = ptACD->GetWorldLocation();
        loc.transform.wp = placeNew.wp;
        loc.idSScene = idSScene;
        ptACD->UpdateCollision();
        bSetRActorPosition = FALSE;
        WorldLocation & locActor = ptRActor->tLocation.tWorldLocation;
        locActor.idSWorld = loc.idSWorld;
    }

    m_vOldPlace = bSnap ? placeNew.wp : m_vNewPlace;
    m_vNewPlace = placeNew.wp;
    m_flNewPlaceTime = (float)(CGameGetClientTicks() + 1) * (1.0f / 60.0f);

    if (bSetRActorPosition)
    {
        if (ptRActor->ptActorAnim == NULL && m_eMovementType != MOVEMENT_TYPE_NONE)
            RActorSnapToPosition(ptRActor, placeNew.wp, idSScene, NULL, ptACD);
        else
            RActorInternalSetPosition(ptRActor, placeNew.wp, TRUE, idSScene);
    }
    PositionChanged(placeNew, idSScene);
}
















void CActorMovement::SetNewFacing(Angle aFacing, BOOL bSnap, BOOL bSendUpdate)
{
    m_aDesiredYaw = aFacing;
    if (bSnap)
    {
        RActor * ptRActor = RActorGet(m_idRActor);
        m_aOldYaw = aFacing;
        m_aNewYaw = aFacing;
        m_flNewYawTime = (float)(CGameGetClientTicks() + 1) * (1.0f / 60.0f);
        ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
        if (ptACD)
        {
            WorldLocation & loc = ptACD->GetWorldLocation();
            loc.transform.q.SetRotation(cgwvZAxis, aFacing);
        }
        else
        {
            RActorInternalSetCurrentFacing(ptRActor, aFacing);
        }
    }
}










uint32 CActorMovement::GetTimeNow() const
{
    return CGameGetClientTicks();
}




void CActorMovement::UpdateWalking()
{
    uint32 uNow = CGameGetClientTicks();
    if (m_uNextClientUpdateTime <= uNow)
    {
        ActorMovement::UpdateWalking();
        UpdateAnim(FALSE);
        uint32 dwNow = CGameGetClientTicks();
        if (dwNow >= m_uNextPositionSendTime || m_uMoveStopTime == uNow)
        {
            SendPositionToServer(12);
            m_uNextPositionSendTime = dwNow + 12;
        }
    }
    UpdateFacing();
}













void CActorMovement::UpdateWalkingRemote()
{
    uint32 uNow = CGameGetClientTicks();
    if (m_uNextClientUpdateTime == uNow)
    {
        ActorMovement::UpdateWalkingRemote();
        UpdateAnim(FALSE);
    }
    UpdateFacing();
}








void CActorMovement::UpdateWalkingInPlace()
{
    uint32 dwNow = GetTimeNow();
    if (m_uNextClientUpdateTime == dwNow)
    {
        ActorMovement::UpdateWalkingInPlace();
        UpdateAnim(FALSE);
    }
    UpdateFacing();
}






void CActorMovement::UpdateDetPath()
{
    uint32 uNow = CGameGetClientTicks();
    if (m_uNextClientUpdateTime <= uNow)
    {
        ActorMovement::UpdateDetPath();
        UpdateAnim(FALSE);
    }
    UpdateFacing();
}








void CActorMovement::UpdateAnim(BOOL bReturnIfMissingIntro)
{
    // Plays the walk animation if we are moving.
    RActor * ptRActor = RActorGet(m_idRActor);

    if (IsMoving() && m_eMovementType != MOVEMENT_TYPE_WALK_FINISHING)
    {
        if (ptRActor->idACD != ACDID_INVALID)
        {
            ActorCommonData & tACD = ACDGet(ptRActor->idACD);

            if (m_eAnimTag != -1)
            {
                AnimLayer eAnimLayer = ANIM_LAYER_NONE;
                if (!RActorAnimationIsAnimTreeEnabled(*ptRActor) || (RActorGetAnimLayerForTag(ptRActor, m_eAnimTag, eAnimLayer) && eAnimLayer != ANIM_LAYER_NONE))
                {
                    AnimReason eAnimReason = AnimPlayerGetAnimReason(tACD);
                    uint32 dwCurrentAnimTag = AnimPlayerGetCurrentAnimTag(tACD);
                    if (eAnimReason != ANIM_REASON_WALKING || dwCurrentAnimTag != (uint32)m_eAnimTag)
                    {
                        if (m_uMoveFlags & (1 << MOVEBIT_START_WALK_AFTER_INTRO))
                        {
                            AnimationTiming tAnimationTiming;
                            AnimPlayerGetAnimationTiming(tACD, m_eAnimTag, (uint32)-2, 1.0f, tAnimationTiming);
                            if (bReturnIfMissingIntro && tAnimationTiming.tAnimSpec[1].snoAnim == -1)
                            {
                                return;
                            }

                            tAnimationTiming.tAnimSpec[1].dwAnimationTag = (uint32)-1;
                            tAnimationTiming.tAnimSpec[1].dwDuration = 0;
                            tAnimationTiming.tAnimSpec[1].snoAnim = -1;
                            tAnimationTiming.tAnimSpec[2].dwAnimationTag = (uint32)-1;
                            tAnimationTiming.tAnimSpec[2].dwDuration = 0;
                            tAnimationTiming.tAnimSpec[2].snoAnim = -1;
                            AnimPlayerPlayAnimationsWithTiming(tACD, &tAnimationTiming, 1, FALSE, ANIM_REASON_WALKING, FALSE, FALSE);
                        }
                        else
                        {
                            AnimPlayerPlayAnimation(tACD, m_eAnimTag, (uint32)-2, FALSE, ANIM_REASON_WALKING, 0.0f, FALSE, 1.0f);
                        }
                    }
                }
            }

            if (AnimPlayerGetAnimReason(tACD) == ANIM_REASON_WALKING)
            {
                if (RActorAnimationIsAnimTreeEnabled(*ptRActor))
                {
                    RActorAnimTreeLayerScaleToVelocity(*ptRActor, ANIM_LAYER_NPC, m_flCurrentSpeed);
                    RActorAnimTreeLayerScaleToVelocity(*ptRActor, ANIM_LAYER_SPECIAL_MOVE, m_flCurrentSpeed);
                }
                else
                {
                    RActorAnimationScaleToVelocity(*ptRActor, m_flCurrentSpeed);
                }
            }
        }
        else if (m_eAnimTag != -1)
        {
            PlayAnimClientOnly(m_eAnimTag);
        }
    }
}






















void CActorMovement::UpdateAnimBlend()

{
    RActor * ptRActor = RActorGet(m_idRActor);

    if (RActorAnimationIsAnimTreeEnabled(*ptRActor))
    {
        BOOL bAimEnabled = FALSE;
        Vector3D wpAimTarget(0.0f, 0.0f, 0.0f);


















        float flSpeed = m_flCurrentSpeed;
        if (m_eAnimTag == -1 &&
            IsMoving() &&
            !RActorAnimTreeLayerIsActive(*ptRActor, ANIM_LAYER_ACTION))
        {
            // Use the stick input speed for the local player.
            ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
            Player * ptPlayer = PlayerGetByACD(ptRActor->idACD);
            flSpeed = (ptACD && ptPlayer) ? CPlayerGetMoveSpeedForStickInput(*ptACD, *ptPlayer, -1, 0.0f) : 0.0f;
        }

        RActorAnimationUpdateAnimTreeState(*ptRActor, flSpeed, m_aYawVelocity, m_bTurn180Trigger, m_bTurn180, m_flTurn180AnimScale, bAimEnabled, wpAimTarget);
        m_bTurn180Trigger = FALSE;
    }
}



void CActorMovement::UpdateFixed()
{
    uint32 uNow = CGameGetClientTicks();
    if (m_uNextClientUpdateTime == uNow)
        ActorMovement::UpdateFixed();
}






void CActorMovement::UpdateArc()
{
    uint32 uNow = CGameGetClientTicks();
    if (m_uNextClientUpdateTime == uNow)
    {
        ActorMovement::UpdateArc();
        if (m_uMoveFlags & (1 << MOVEBIT_START_WALK_AFTER_INTRO))
            UpdateAnim(TRUE);
        if (m_uMoveFlags & (1 << MOVEBIT_USE_DESIRED_HEIGHT_FOR_CAMERA))
        {
            float flA = m_flGravity * 0.5f;
            if (flA < -1e-6f)
            {
                WorldPlace placeCurr;
                GetPlace(placeCurr);
                float flB = m_vVel.z;
                float flC = placeCurr.wp.z - m_flDesiredHeight;
                float flUnitInterp;
                float flDiscriminant = flB * flB - 4.0f * flA * flC;
                if (flDiscriminant < 0.0f)
                {
                    flUnitInterp = 1.0f;
                }
                else
                {
                    float flTicksLeft = (-flB - Math::Sqrt(flDiscriminant)) / (2.0f * flA);
                    if (flTicksLeft > 1e-6f)
                    {
                        float flTicksToAdvance = (float)m_uUpdateInterval;
                        flUnitInterp = Math::Pin(flTicksToAdvance / flTicksLeft, 0.0f, 1.0f);
                    }
                    else
                    {
                        flUnitInterp = 0.0f;
                    }
                }
                m_flCurrentCamHeight = Math::Lerp(m_flCurrentCamHeight, m_flDesiredHeight, flUnitInterp);
            }
        }
    }
}











BOOL CActorMovement::ProjCollisionResponse(ImpactData &)
{
    if (m_uMoveFlags & (1 << MOVEBIT_IS_PROJECTILE))
    {
        RActor * ptRActor = RActorTryToGet(m_idRActor);
        if (ptRActor)
        {
            ptRActor->dwFlags &= ~(1 << 6);
            ptRActor->dwFlags |= (1 << 4);
            AttachmentListHandleParentDeletion(ptRActor->tAttachmentList);
        }
    }
    return TRUE;
}












void CActorMovement::ProjNavReflectResponse()
{
}




void CActorMovement::ProjToadIdleResponse()
{
}










































void CActorMovement::MoveTo(
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
    if (!IsMoving())
    {

        m_uNextClientUpdateTime = 0;
    }


    RActor * ptRActor = RActorGet(m_idRActor);

    if (flSpeed <= 1e-6f && !(uMoveFlags & (1 << MOVEBIT_START_WALK_AFTER_INTRO)))
    {
        if (eAnimTag == -1)
        {

            flSpeed = GetDesiredSpeed();
        }
        else if (ptRActor->idACD != ACDID_INVALID)
        {
            ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
            if (ptACD)
            {
                flSpeed = ptACD->GetWalkingSpeed(eAnimTag);
                uMoveFlags |= (1 << MOVEBIT_USE_WALKING_SPEED);
            }
        }
        else
        {
            SNO snoAnim = ActorGetAnim(ptRActor->snoActor, -1, eAnimTag);

            Vector3D wvVel;
            ScopedAnimationDefinitionLockR pdefAnim(snoAnim);
            AnimationGetAvgVelocity(pdefAnim, 0, 1.0f, TRUE, wvVel);
            flSpeed = AnimGetWalkingSpeed(pdefAnim, 1.0f);
        }
    }

    if (IsMoving() && m_eAnimTag != -1 && m_eAnimTag != eAnimTag)
    {



        PlayIdle();
    }

    ActorMovement::MoveTo(placeDest, flSpeed, flAccel, eAnimTag, uMoveFlags, flMaxRangeFromDest, idACDTarget, snoPowerPassability, dwWalkInPlaceTicks, nMaxTicksToWalk, flMaxPathfindingDist, pfnCallback);

    m_uMoveStopTime = 0;




}






void CActorMovement::Interpolate()

{
    float flActualTime = CGameGetClientTime();

    // Interpolate from where we were at the last tick to where we are at the next one.



    float flPlaceRatio = 1.0f - Math::Pin((m_flNewPlaceTime - flActualTime) / (1.0f / 60.0f), 0.0f, 1.0f);
    float flYawRatio = 1.0f - Math::Pin((m_flNewYawTime - flActualTime) / (1.0f / 60.0f), 0.0f, 1.0f);





    RActor * ptRActor = RActorGet(m_idRActor);
    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);

    SSceneID idSScene = ptACD ? ptACD->GetWorldLocation().idSScene : SSCENEID_INVALID;

    Vector3D vInterpolatedPlace = m_vNewPlace * flPlaceRatio + m_vOldPlace * (1.0f - flPlaceRatio);

    if (ptRActor->ptActorAnim == NULL && m_eMovementType != MOVEMENT_TYPE_NONE)
    {
        RActorSnapToPosition(ptRActor, vInterpolatedPlace, idSScene, NULL, ptACD);
    }
    else
    {
        RActorInternalSetPosition(ptRActor, vInterpolatedPlace, TRUE, idSScene);
    }

    Angle aInterpolatedYaw;
    Angle aYawDiff = m_aOldYaw - m_aNewYaw;

    if (aYawDiff < -HALF_CIRCLE)
    {
        aInterpolatedYaw = Math::Lerp(m_aOldYaw + FULL_CIRCLE, m_aNewYaw, flYawRatio);
    }
    else if (aYawDiff > HALF_CIRCLE)
    {
        aInterpolatedYaw = Math::Lerp(m_aOldYaw, m_aNewYaw + FULL_CIRCLE, flYawRatio);
    }
    else
    {
        aInterpolatedYaw = Math::Lerp(m_aOldYaw, m_aNewYaw, flYawRatio);
    }

    if (aInterpolatedYaw > FULL_CIRCLE)
    {
        aInterpolatedYaw -= FULL_CIRCLE;
    }
    else if (aInterpolatedYaw < 0.0f)
    {
        aInterpolatedYaw += FULL_CIRCLE;
    }

    RActorInternalSetCurrentFacing(ptRActor, aInterpolatedYaw);
}































































































































































































































































void CActorMovement::ArrivingAtDest(uint32 uTicksAway, BOOL, BOOL, ACDID)
{
    m_uMoveStopTime = GetTimeNow() + uTicksAway;
}







void CActorMovement::PlayIdle()
{
    if (!(m_uMoveFlags & (1 << MOVEBIT_DONT_AUTOPLAY_IDLE)))
    {
        RActor * ptRActor = RActorGet(m_idRActor);
        if (ptRActor->idACD == ACDID_INVALID)
        {
            PlayAnimClientOnly(0x11000);
        }
        else
        {
            AnimLayer eAnimLayer = ANIM_LAYER_NONE;
            if (!RActorAnimationIsAnimTreeEnabled(*ptRActor) || (RActorGetAnimLayerForTag(ptRActor, m_eAnimTag, eAnimLayer) && eAnimLayer != ANIM_LAYER_NONE))
                AnimPlayerPlayIdle(ACDGet(ptRActor->idACD));
        }
        m_eAnimTag = -1;
    }
}















void CActorMovement::SendPositionToServer(uint32 uTicksToCheck)
{
    const RActor * ptRActor = RActorGet(m_idRActor);

    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (!ptACD)
        return;









    WorldPlace placeCurr;
    GetPlace(placeCurr);

    if (uTicksToCheck)
    {
        PerformCollision(uTicksToCheck, NULL);


        Vector3D vDelta(m_placeDest.wp.x - placeCurr.wp.x, m_placeDest.wp.y - placeCurr.wp.y, 0.0f);
        if (vDelta.MagnitudeSquared() >= EPSILON * EPSILON)
        {
            TargetWillBeReached(vDelta, uTicksToCheck, NULL);
        }
    }




    WorldPlace placePredicted;
    placePredicted.wp = placeCurr.wp + GetVelocity() * (float)uTicksToCheck;

    if (m_snoPowerPassability == -1 || !PowerShouldUseClientWalking(*ptACD, AutoPowerDefinitionR(m_snoPowerPassability)))
    {
        Vector3D vDiff = placePredicted.wp - m_vCachedPositionSent;
        if (Math::Abs(vDiff.x) < 0.01f && Math::Abs(vDiff.y) < 0.01f && Math::Abs(vDiff.z) < 0.01f)
            return;
    }

    m_vCachedPositionSent = placePredicted.wp;

    ACDClientTranslateMessage tMessage;



    tMessage.uTickBase = CGameGetClientTicks();
    tMessage.uTicksAhead = uTicksToCheck;
    tMessage.vDest = placePredicted.wp;
    tMessage.flMovementSpeed = m_flCurrentSpeed;
    tMessage.aDesiredYaw = m_aDesiredYaw;
    tMessage.uMoveFlags = m_uMoveFlags & ~((1 << MOVEBIT_AUTO_UPDATE_YAW) | (1 << MOVEBIT_REMOTE_MOVEMENT));
    tMessage.eAnimTag = m_eAnimTag;

    if (m_snoPowerPassability != -1)
    {
        PowerTemplate eTemplate = PowerGetTemplate(AutoPowerDefinitionR(m_snoPowerPassability), ptACD->id);
        if (eTemplate != 3)
        {
            tMessage.snoPower.Set(m_snoPowerPassability);
        }
    }



    MessageSendToServer(CMSG_ACD_TRANSLATE_NORMAL, &tMessage, sizeof(tMessage), PlayerGetByRActor(m_idRActor));
}
void CActorMovement::SendFacingToServer(BOOL bSnapFacing)
{
    RActor * ptRActor = RActorGet(m_idRActor);
    if (ACDTryToGet(ptRActor->idACD))
    {
        ACDTranslateFacingMessage tMessage;
        tMessage.ann = ANNFromACD(ptRActor->idACD);
        tMessage.aDesiredYaw = m_aDesiredYaw;
        tMessage.bSnapFacing = bSnapFacing;
        MessageSendToServer(CMSG_ACD_TRANSLATE_FACING, &tMessage, sizeof(tMessage), PlayerGetByRActor(m_idRActor));
    }
}














void CActorMovement::SendAckResponse(uint32 uAckValue)
{
    RActor * ptRActor = RActorTryToGet(m_idRActor);
    if (ptRActor && PlayerIsLocal(m_idRActor))
    {
        ACDTranslateAckMessage tMessage;
        tMessage.uAckValue = uAckValue;
        MessageSendToServer(CMSG_ACD_TRANSLATE_ACK, &tMessage, sizeof(tMessage), PlayerGetByRActor(m_idRActor));
    }
}





void CActorMovement::SnapPlaceToGround(WorldPlace & placeCurrent) const
{
    {
        NavMeshHandle tNavMeshHandle;
        Vector3D vNormal;
        ScenesCommonNavMeshNodeFromPlace(placeCurrent, &tNavMeshHandle, FALSE, FALSE);
        NavMeshGetHeightAndNormal(tNavMeshHandle, placeCurrent.wp, FALSE, placeCurrent.wp.z, vNormal);
    }
}


















void CActorMovement::Stuck()
{
    m_uMoveFlags |= (1 << MOVEBIT_ARRIVING_AT_DESTINATION);
    ArrivingAtDest(0, FALSE, FALSE, ACDID_INVALID);
}





























































































































































void CActorMovement::ProcessSnappedMessage(ACDTranslateSnappedMessage & tMessage)
{
    uint32 uMoveFlags = tMessage.uMoveFlags;
    WorldPlace placeDest;
    placeDest.idSWorld = CGameGetCurrentSWorldID();
    placeDest.wp = tMessage.vDest;

    BOOL bSnap;
    if (uMoveFlags & (1 << MOVEBIT_SNAP_SOFT))
    {
        WorldPlace placeCurrent;
        GetPlace(placeCurrent);
        if (placeCurrent.idSWorld != placeDest.idSWorld || (placeDest.wp - placeCurrent.wp).MagnitudeSquared() > 400.0f)
            bSnap = TRUE;
        else
            bSnap = FALSE;
    }
    else
    {
        bSnap = (uMoveFlags & (1 << MOVEBIT_SNAP)) != 0;
    }

    if (tMessage.uCameraSmoothingTime.Exists())
    {
        WorldPlace placeCurrent;
        GetPlace(placeCurrent);
        if (placeCurrent.idSWorld == placeDest.idSWorld)
        {
            uint32 uEndTime = tMessage.uCameraSmoothingTime;
            if (uEndTime > CGameGetPredictedServerTime())
            {
                uint32 uDuration = uEndTime - CGameGetPredictedServerTime();
                m_uCamStartTime = CGameGetClientTicks();
                m_uCamEndTime = m_uCamStartTime + uDuration;
                m_vCamStartPos = placeCurrent.wp;
            }
        }
    }

    if (bSnap)
    {
        StopNow();
        SetNewPlace(placeDest, TRUE, FALSE, TRUE);
        Player * ptPlayer = PlayerGetByACD(GetACDID());
        if (ptPlayer && PlayerIsLocal(ptPlayer))
            LocalPlayerClamping::ClearInactiveFlagForController(ptPlayer->nControllerIndex, LocalPlayerClamping::AR_MOVEMENT);
    }

    SetNewFacing(tMessage.aDesiredYaw, tMessage.bSnapFacing, TRUE);

    if (tMessage.uAckValue.Exists())
        SendAckResponse(tMessage.uAckValue);
}
















void CActorMovement::ProcessFacingMessage(ACDTranslateFacingMessage & tMessage)
{
    SetNewFacing(tMessage.aDesiredYaw, tMessage.bSnapFacing, TRUE);
}




void CActorMovement::PlayCurrentAnim(float flUnitStartTime, AnimReason eAnimReason)
{
    RActor * ptRActor = RActorGet(m_idRActor);
    if (ptRActor->idACD != ACDID_INVALID && m_eAnimTag != -1)
    {
        AnimLayer eAnimLayer = ANIM_LAYER_NONE;
        ActorCommonData & tACD = ACDGet(ptRActor->idACD);
        if ((!RActorAnimationIsAnimTreeEnabled(*ptRActor) || (RActorGetAnimLayerForTag(ptRActor, m_eAnimTag, eAnimLayer) && eAnimLayer != ANIM_LAYER_NONE)) &&
            (!(m_uMoveFlags & (1 << MOVEBIT_IS_PROJECTILE)) || AnimPlayerGetAnimReason(tACD) == ANIM_REASON_IDLE))
        {
            AnimPlayerPlayAnimation(tACD, m_eAnimTag, (uint32)-2, FALSE, eAnimReason, flUnitStartTime, FALSE, 1.0f);
        }
    }
}











void CActorMovement::ProcessTranslateFixedMessage(ACDTranslateFixedMessage & tMessage)
{
    uint32 uFlags = tMessage.uMoveFlags | (1 << MOVEBIT_REMOTE_MOVEMENT);
    MoveAtFixedVelocity(tMessage.vVel, uFlags, tMessage.eAnimTag, tMessage.snoPowerPassability);
    PlayCurrentAnim(0.0f, ANIM_REASON_WALKING);
}






void CActorMovement::ProcessFixedUpdateMessage(ACDTranslateFixedUpdateMessage & tMessage)
{
    if (m_eMovementType == MOVEMENT_TYPE_FIXED_PROJECTILE || m_eMovementType == MOVEMENT_TYPE_FIXED_PROJECTILE_COLLIDING)
    {
        WorldPlace placeCurr;
        m_vVel = tMessage.vVel;
        m_flDesiredSpeed = m_vVel.Magnitude();
        m_vLastVel = m_vVel;
        m_pfnCallback = NULL;
        GetPlace(placeCurr);
        placeCurr.wp = tMessage.vPos;
        SetNewPlace(placeCurr, FALSE, FALSE, TRUE);
        Angle aYaw = Math::ATan(m_vVel.y, m_vVel.x);
        SetNewFacing(aYaw, TRUE, TRUE);
        QueueUpdate(GetEarliestUpdateTickOffset());
    }
}










void CActorMovement::ProcessTranslateArcMessage(ACDTranslateArcMessage & tMessage)
{
    uint32 uFlags = tMessage.uMoveFlags | (1 << MOVEBIT_REMOTE_MOVEMENT);
    WorldPlace placeCurr;
    GetPlace(placeCurr);
    placeCurr.wp = tMessage.vPos;
    SetNewPlace(placeCurr, FALSE, FALSE, TRUE);
    if (uFlags & (1 << MOVEBIT_ARC_UNTIL_DEST_HEIGHT))
    {
        m_flDesiredHeight = tMessage.flDesiredHeight;
        m_flCurrentCamHeight = placeCurr.wp.z;
        uFlags |= (1 << MOVEBIT_USE_DESIRED_HEIGHT_FOR_CAMERA);
    }
    MoveWithArc(tMessage.eAnimTag, tMessage.eLandingAnimTag, uFlags, tMessage.flGravity, tMessage.vVel, tMessage.snoPowerPassability, tMessage.flUnitBounce, NULL);
    PlayCurrentAnim(0.0f, ANIM_REASON_POWER);
    if (PlayerIsLocal(m_idRActor))
    {
        RActor * ptRActor = RActorGet(m_idRActor);
        CPlayerInputClearCachedPower(PlayerGetByACD(ptRActor->idACD));
        CPlayerInputClearClientWalkPower(PlayerGetByACD(ptRActor->idACD));
    }
}













void CActorMovement::ProcessTranslateDetPathMessage(ACDTranslateDetPathMessage & tMessage)
{
    uint32 uFlags = tMessage.uMoveFlags | (1 << MOVEBIT_REMOTE_MOVEMENT);
    SetUpdateInterval(1);
    StartDetPath(tMessage.eDPath, tMessage.wvVelocity, uFlags, tMessage.eAnimTag, tMessage.snoPowerPassability, tMessage.aOriginalYaw, tMessage.vStartPos, tMessage.tSeed, tMessage.nVar0, tMessage.flVar0, tMessage.flVar1, tMessage.flVar2, tMessage.flVar3, tMessage.flVar4, tMessage.flSpeedMult);
    PlayCurrentAnim(0.0f, ANIM_REASON_WALKING);
}






















void CActorMovement::ProcessTranslateDetPathSinMessage(ACDTranslateDetPathSinMessage & tMessage)
{
    uint32 uFlags = tMessage.uMoveFlags | (1 << MOVEBIT_REMOTE_MOVEMENT);
    SetUpdateInterval(1);
    StartDetPathSinWave(tMessage.eDPath, tMessage.wvVelocity, uFlags, tMessage.eAnimTag, tMessage.snoPowerPassability, tMessage.aOriginalYaw, tMessage.vStartPos, tMessage.tSeed, tMessage.nVar0, tMessage.flVar0, tMessage.tSinData, tMessage.flSpeedMult);
    PlayCurrentAnim(0.0f, ANIM_REASON_WALKING);
}




















void CActorMovement::ProcessTranslateDetPathSpiralMessage(ACDTranslateDetPathSpiralMessage & tMessage)
{
    uint32 uFlags = tMessage.uMoveFlags | (1 << MOVEBIT_REMOTE_MOVEMENT);
    SetUpdateInterval(1);
    StartDetPathSpiral(tMessage.eDPath, tMessage.vStartPos, tMessage.vTargetPos, uFlags, tMessage.eAnimTag, tMessage.flVar0, tMessage.flVar1, tMessage.tSinData, tMessage.flSpeedMult);
    PlayCurrentAnim(0.0f, ANIM_REASON_WALKING);
}

















void CActorMovement::ProcessTranslateDetPathPieWedgeMessage(ACDTranslateDetPathPieWedgeMessage & tMessage)
{
    uint32 uFlags = tMessage.uMoveFlags | (1 << MOVEBIT_REMOTE_MOVEMENT);
    SetUpdateInterval(1);
    StartDetPathPieWedge(tMessage.vStartPos, tMessage.vFirstTargetPos, uFlags, tMessage.eAnimTag, tMessage.tPieData, tMessage.flCurTime, tMessage.flSpeedMult);
    PlayCurrentAnim(0.0f, ANIM_REASON_WALKING);
}















void CActorMovement::ProcessTranslateStoppedMessage(ACDTranslateStoppedMessage & tMessage)
{
    WorldPlace placeNew;
    GetPlace(placeNew);
    placeNew.wp = tMessage.vPos;
    SetNewPlace(placeNew, FALSE, FALSE, TRUE);
    StopProjectile();
}










void CActorMovement::GetACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs) const
{
    if (tPlace.idSWorld != CGameGetCurrentSWorldID())
        return;

    Sphere tSphere;
    tSphere.Set(tPlace.wp, wdRadius);

    RActorID idRActorIgnore = RACTORID_INVALID;
    ActorCommonData * ptACD = ACDTryToGet(idACDIgnore);
    if (ptACD)
        idRActorIgnore = (RActorID)ptACD->idOwner;

    RActorIDListPooled listRActors(MemoryPoolsGetDATAIDListPool());
    RActorsIntersectSphere(tSphere, idRActorIgnore, listRActors);


    POS pos = listRActors.GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)listRActors.GetNext(pos);

        listACDs.AddTail_Tracked(RActorGet(idRActor)->idACD);
    }
}


void CActorMovement::StopNow()
{
    if (!IsMoving())
        return;




    SNO snoPowerPassability = m_snoPowerPassability;

    m_uMoveStopTime = GetTimeNow();
    m_uNextClientUpdateTime = 0;
    m_snoPowerPassability = -1;





    if (m_eMovementType >= MOVEMENT_TYPE_WALKING && m_eMovementType <= MOVEMENT_TYPE_WALK_FINISHING && !(m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT)))




    {
        SendPositionToServer(0);
    }




    ACDID idACD = GetACDID();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    ActorAnimationTag eAnimTag;
    if (ptACD &&
        (eAnimTag = AnimPlayerGetWalkCoolDownTag(*ptACD, m_eAnimTag)) != -1 &&


        AnimPlayerGetCurrentAnimTag(*ptACD) == m_eAnimTag)
    {
        float flScalar = 1.0f;


        if (ACDIsPlayer(ptACD->id) && PowerGetTemplate(AutoPowerDefinitionR(snoPowerPassability), ptACD->id) == 30)
        {

            ScopedPowerDefinitionLockR pdefPower(snoPowerPassability);
            if (pdefPower)
            {
                flScalar = PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_SPECIALWALK_PLAYER_END_ANIM_SCALAR, -1, -1, 0);
            }
        }

        AnimPlayerPlayAnimation(*ptACD, eAnimTag, -1, FALSE, ANIM_REASON_PREPLAY_POWER, 0.0f, FALSE, flScalar);
    }
    else
    {
        PlayIdle();
    }


    ActorMovement::StopNow();
}





SNO CActorMovement::GetActorSNO() const
{
    RActor * ptRActor = RActorGet(m_idRActor);
    return ptRActor->snoActor;
}






ACDID CActorMovement::PerformProjectileCollision(uint32 & uTicksToTravel) const
{


    DisplayInternalError(NULL, "", 1975, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    return ACDID_INVALID;
}




void CActorMovement::SetTilt(TiltType nTiltType, BOOL bSnap)
{
    if (nTiltType == 0)
        return;

    WorldPlace placeCurr;
    GetPlace(placeCurr);
    NavMeshHandle tNavMeshHandle;
    ScenesCommonNavMeshNodeFromPlace(placeCurr, &tNavMeshHandle, FALSE, FALSE);
    if (NavMeshHandleIsNone(tNavMeshHandle))
        return;

    Vector3D vNormal;
    NavMeshGetHeightAndNormal(tNavMeshHandle, placeCurr.wp, FALSE, placeCurr.wp.z, vNormal);
    if (vNormal.z < 3.0f)
        return;

    RActor * ptRActor = RActorGet(m_idRActor);
    ptRActor->tTilt.Set(nTiltType, bSnap, vNormal, m_aDesiredYaw);
}















void CActorMovement::SyncToPosition(const Vector3D & vPos, const BOOL bSnap)
{
    WorldPlace placeCurr;
    GetPlace(placeCurr);

    if (IsInArcingMovement())
    {
        m_placeDest.idSWorld = placeCurr.idSWorld;
        m_placeDest.wp = vPos;
        m_uMoveFlags |= (1 << MOVEBIT_ARC_UNTIL_DEST_HEIGHT);
        return;
    }

    StopNow();

    Vector3D vDelta = vPos - placeCurr.wp;
    if (bSnap || vDelta.MagnitudeSquared() > 25.0f)
    {
        placeCurr.wp = vPos;
        SetNewPlace(placeCurr, TRUE, FALSE, TRUE);
    }
}












































































BOOL CActorMovement::RaycastAgainstWorld(const WorldPlace & placeStart, const WorldPlace & placeEnd, Vector3D & vIntersect, Vector3D & vNormal, float & flUnitTraveled, uint16 uExcludeMask) const
{
    RActorID idRActor = LocalPlayerRActorGet();
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor && ptRActor->tLocation.eActorLocation == 0)
    {
        SWorldID idSWorldLocal = ptRActor->tLocation.tWorldLocation.idSWorld;
        if (idSWorldLocal == placeStart.idSWorld && idSWorldLocal == placeEnd.idSWorld)
        {
            RayCastInput tInput;
            tInput.wpFrom = placeStart.wp;
            tInput.wpTo = placeEnd.wp;
            tInput.uIncludeMask = 1;
            tInput.uExcludeMask = uExcludeMask;
            RayCastOutput tOutput;
            BOOL bHit = gptPhysicsAPIImpl->RayCast(&tOutput, &tInput);
            vIntersect = tOutput.wpPoint;
            vNormal = tOutput.wvNormal;
            return bHit;
        }
    }
    vNormal = cgwvZAxis;
    vIntersect = placeStart.wp;
    flUnitTraveled = 0.0f;
    return TRUE;
}





















void CActorMovement::PlayPreplayedAnim(ActorAnimationTag eAnimTag, float flSpeedScalar)
{
    ACDID idACD = GetACDID();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
        AnimPlayerPlayAnimation(*ptACD, eAnimTag, (uint32)-1, FALSE, ANIM_REASON_POWER, 0.0f, FALSE, flSpeedScalar);
}
















void CActorMovement::Bounced()
{
    RActor * ptRActor = RActorTryToGet(m_idRActor);
    if (ptRActor)
    {
        uint32 uNow = GetTimeNow();
        if (uNow >= m_uNextBounceTime)
        {
            {
                KeyedMessage tMsg(MSGKEY_RESERVED_COLLISION, 0.0f, Shape::DEFAULT_MATERIAL);
                RActorSendMsgKey(m_idRActor, tMsg, MSGPASS_TO_ACTORCHAIN);
            }
            m_uNextBounceTime = uNow + 5;
        }
    }
}
static const float CAMERA_OFFSET_MAX = 75.0f;
BOOL CActorMovement::GetCameraTargetPos(
    Vector3D & vPos)
{
    RActor * ptRActor = RActorTryToGet(m_idRActor);
    if (!ptRActor) return FALSE;
    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (ptACD && ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_DONT_UPDATE_CAMERA_WHILE_ATTACHED)))
    {
        ACDNetworkName annParent = (ACDNetworkName)ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_ATTACHED_TO_ANN));
        if (annParent != ANN_INVALID)
        {
            RActorID idRActorParent = RActorFindByANN(annParent);
            RActor * ptRActorParent = RActorTryToGet(idRActorParent);
            if (ptRActorParent)
                return FALSE;
        }
    }
    vPos = ptRActor->transformClient.wp;
    if (IsMoving() &&
        IsInArcingMovement() && (m_uMoveFlags & (1 << MOVEBIT_USE_DESIRED_HEIGHT_FOR_CAMERA)))
        vPos.z = m_flCurrentCamHeight;
    if (m_uCamEndTime)
    {
        uint32 uNow = CGameGetClientTicks();
        if (uNow >= m_uCamEndTime)
        {
            m_uCamStartTime = m_uCamEndTime = 0;
        }
        else
        {
            int32 nTotalTime = m_uCamEndTime - m_uCamStartTime;
            int32 nCurrTime = uNow - m_uCamStartTime;
            nTotalTime = Math::Max(nTotalTime, 1);
            float flUnitTime = (float)nCurrTime / (float)nTotalTime;
            float flInvUnitTime = Math::Pin(1.0f - flUnitTime, 0.0f, 1.0f);
            Vector3D vDelta = m_vCamStartPos - vPos;
            float flLength = 0.0f;
            if (vDelta.MagnitudeSquared() > 1e-12f)
                flLength = vDelta.Normalize();
            flLength = Math::Min(flLength, CAMERA_OFFSET_MAX);
            vDelta *= flInvUnitTime * flLength;
            vPos += vDelta;
        }
    }
    return TRUE;
}
// DWARF declarations from the animation and camera owners.
void AnimationSetChannelPlayParams(AnimChannelPlayParams &, const AnimationDefinition *, RNG &);
void AnimationChannelPlay(RActor *, const AnimChannelPlayParams &, const BOOL);
void AnimTreeLayerPlay(RActor *, uint32, const AnimChannelPlayParams &, BOOL, BOOL);
void AnimTreeLayerSetPoseFromLayer(RActor *, uint32, uint32);
SNO AnimTreeLayerGetSNO(const AnimTree &, const AnimTreeDefinition &, uint32);
SNO AnimationChannelGetSNO(const AnimChannel &);
void AnimTagGetAdditionalTag(const uint32, uint32 &);
uint32 RActorGetAnimTreeLayerHash(AnimLayer);
BOOL RActorGetAnimTreeLayerImmediatelyActive(AnimLayer);
BOOL RActorGetAnimTreeManualRampOut(RActor *, uint32);
void RActorAnimTreeLayerRampOutBlend(RActor &, AnimLayer);
void RActorAnimTreeLayerRampOutBlendForManual(RActor &, AnimLayer);
void RActorAnimTreeLayerRampOutBlendForLooping(RActor &, AnimLayer);
void SmoothMovementAngle(const Angle, const Angle, const Angle, const Angle, const Angle, const Angle, Angle &);

// On its DWARF lines 1118-1221 (#line); the !bAnimTreeEnabled branches come first (rows 1132 -> 1134
// AnimationChannelGetSNO -> 1138, and 1166 -> 1168 AnimationChannelPlay -> 1172); ccmp operands give
// `snoCurrentAnim == snoAnim`.
#line 1118
void CActorMovement::PlayAnimClientOnly(
    uint32 dwAnimTag)
{



    RActor * ptRActor = RActorGet(m_idRActor);
    if (!ptRActor->ptActorAnim)
    {
        return;
    }

    BOOL bAnimTreeEnabled = RActorAnimationIsAnimTreeEnabled(*ptRActor);
    SNO snoCurrentAnim = -1;
    if (!bAnimTreeEnabled)
    {
        snoCurrentAnim = AnimationChannelGetSNO(ptRActor->ptActorAnim->tAnimChannel);
    }
    else
    {
        AnimLayer eAnimLayer = ANIM_LAYER_NONE;
        if (RActorGetAnimLayerForTag(ptRActor, dwAnimTag, eAnimLayer))
        {
            uint32 dwAnimLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
            ScopedAnimTreeDefinitionLockR pdefAnimTree(ptRActor->ptActorAnim->snoAnimTree);
            snoCurrentAnim = AnimTreeLayerGetSNO(*ptRActor->ptActorAnim->ptAnimTree, *pdefAnimTree, dwAnimLayerHash);
        }
    }

    SNO snoAnim = ActorGetAnim(ptRActor->snoActor, -1, dwAnimTag);
    if (snoAnim == -1 || snoCurrentAnim == snoAnim)
    {
        return;
    }









    RNG & rng = CGameGlobalsGet().rngLocal;
    AnimChannelPlayParams tChannelPlayParams;
    ScopedAnimationDefinitionLockR pdefAnim(snoAnim);
    AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);

    if (!bAnimTreeEnabled)
    {
        AnimationChannelPlay(ptRActor, tChannelPlayParams, FALSE);
    }
    else
    {
        AnimLayer eAnimLayer = ANIM_LAYER_NONE;
        if (RActorGetAnimLayerForTag(ptRActor, dwAnimTag, eAnimLayer))
        {


            if (eAnimLayer == ANIM_LAYER_IDLE || eAnimLayer == ANIM_LAYER_DEATH)
            {
                RActorAnimTreeLayerRampOutBlendForManual(*ptRActor, ANIM_LAYER_ACTION);
                RActorAnimTreeLayerRampOutBlendForLooping(*ptRActor, ANIM_LAYER_ACTION);
                RActorAnimTreeLayerRampOutBlendForLooping(*ptRActor, ANIM_LAYER_ACTION_ALT);
                RActorAnimTreeLayerRampOutBlendForLooping(*ptRActor, ANIM_LAYER_SPECIAL_MOVE);
            }
            else if (eAnimLayer == ANIM_LAYER_SPECIAL_MOVE)
            {
                RActorAnimTreeLayerRampOutBlend(*ptRActor, ANIM_LAYER_ACTION);
                RActorAnimTreeLayerRampOutBlend(*ptRActor, ANIM_LAYER_ACTION_ALT);
            }

            uint32 dwAnimLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
            BOOL bImmediatelyActive = RActorGetAnimTreeLayerImmediatelyActive(eAnimLayer);
            BOOL bManualRampOut = RActorGetAnimTreeManualRampOut(ptRActor, dwAnimTag);

            if (eAnimLayer == ANIM_LAYER_NPC)
            {
                if (!RActorAnimTreeLayerIsActive(*ptRActor, ANIM_LAYER_CACHE))
                {
                    uint32 dwCacheLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_CACHE);
                    AnimTreeLayerSetPoseFromLayer(ptRActor, dwCacheLayerHash, dwAnimLayerHash);
                }
            }

            AnimTreeLayerPlay(ptRActor, dwAnimLayerHash, tChannelPlayParams, bImmediatelyActive, bManualRampOut);
        }



        uint32 dwAdditionalTag;
        AnimTagGetAdditionalTag(dwAnimTag, dwAdditionalTag);

        if (dwAdditionalTag != uint32(-1))
        {
            SNO snoAdditionalAnim = ActorGetAnim(ptRActor->snoActor, -1, dwAdditionalTag);
            if (snoAdditionalAnim != -1)
            {
                PlayAnimClientOnly(dwAdditionalTag);
            }
        }
    }

}

// Inferences from the original's code: soft snap is an if/else storing TRUE/FALSE (the original threads it and calls PlayerIsLocal on each path), the 0x11060 retro tag is tested before 0x11068, placeDest is declared before placeCurr (stack slots 0x30/0x20), DistanceSquared/Distance2DSquared operand order from the fsub operands.
// Matched (o02) once laid on DWARF lines 1420-1566 (#line). Inferred from rows: uFlags masked on 1447 and stored on 1448;
// `uMoveFlags |= 0x20` on 1460 (the orr feeding MoveTo has row 1460); the optional reads as default + if
// (csel on 1484/1490/1496/1555, loads on 1486/1492/1498/1557); DWARF local ptRActor (1526) added.
#line 1420
void CActorMovement::ProcessTranslateMessage(
    ACDTranslateNormalMessage & tMessage)
{

    if (tMessage.vDest.Exists())
    {
        m_vCachedDest = tMessage.vDest;
    }

    if (tMessage.aDesiredYaw.Exists())
    {
        m_aCachedDesiredYaw = tMessage.aDesiredYaw;
    }

    if (tMessage.flMovementSpeed.Exists())
    {
        m_flCachedMovementSpeed = tMessage.flMovementSpeed;
    }

    BOOL bSnapped = FALSE;
    BOOL bSoftSnap = FALSE;
    if (tMessage.uMoveFlags.Exists())
    {
        uint32 uFlags = tMessage.uMoveFlags;
        bSnapped = (uFlags & (1 << 8)) != 0;
        bSoftSnap = (uFlags & (1 << 21)) != 0;

        uFlags &= ~((1 << 8) | (1 << 21));
        m_uCachedMoveFlags = uFlags;
    }

    if (tMessage.eAnimTag.Exists())
    {
        m_uCachedAnimTag = tMessage.eAnimTag;
    }

    const Vector3D & vDest = m_vCachedDest;
    const Angle aDesiredYaw = m_aCachedDesiredYaw;
    const WorldDistance flMovementSpeed = m_flCachedMovementSpeed;
    uint32 uMoveFlags = m_uCachedMoveFlags;
    uMoveFlags |= 0x20;
    uint32 dwAnimTag = m_uCachedAnimTag;


    if (CWorldIsRetro())
    {
        RActor * ptRActor = RActorGet(m_idRActor);


        if (ACDIsPlayer(ptRActor->idACD))
        {
            ActorCommonData & tACD = ACDGet(ptRActor->idACD);
            if (dwAnimTag == 0x11060 && AnimPlayerAnimExists(tACD, 0x11070))
            {
                dwAnimTag = 0x11070;
            }
            else if (dwAnimTag == 0x11068 && AnimPlayerAnimExists(tACD, 0x11080))
            {
                dwAnimTag = 0x11080;
            }
        }
    }

    uint32 dwWalkInPlaceTicks = 0;
    if (tMessage.uWalkInPlaceTicks.Exists())
    {
        dwWalkInPlaceTicks = tMessage.uWalkInPlaceTicks;
    }

    SNO snoPowerPassability = -1;
    if (tMessage.snoPowerPassability.Exists())
    {
        snoPowerPassability = tMessage.snoPowerPassability;
    }

    int32 nMaxTicksToWalk = -1;
    if (tMessage.nMaxTicksToWalk.Exists())
    {
        nMaxTicksToWalk = tMessage.nMaxTicksToWalk;
    }

    WorldPlace placeDest;
    placeDest.idSWorld = CGameGetCurrentSWorldID();
    placeDest.wp = vDest;

    WorldPlace placeCurr;
    GetPlace(placeCurr);

    if (bSoftSnap)
    {



        if (placeCurr.idSWorld != placeDest.idSWorld ||
            Vector3D::DistanceSquared(placeCurr.wp, placeDest.wp) > 400.0f)
        {
            bSnapped = TRUE;
        }
        else
        {
            bSnapped = FALSE;
        }
    }

    if (PlayerIsLocal(m_idRActor))
    {
        RActor * ptRActor = RActorGet(m_idRActor);
        CPlayerInputClearClientWalkPower(PlayerGetByACD(ptRActor->idACD));
    }

    if (bSnapped)
    {
        StopNow();
        SetNewPlace(placeDest, TRUE, FALSE, TRUE);
    }
    else if (tMessage.vDest.Exists() &&
        (flMovementSpeed > EPSILON || dwWalkInPlaceTicks ||
        (nMaxTicksToWalk > 0 && placeCurr.idSWorld == placeDest.idSWorld && Vector3D::Distance2DSquared(placeCurr.wp, placeDest.wp) > EPSILON)))
    {

        MoveTo(placeDest, flMovementSpeed, 0.0f, (ActorAnimationTag)dwAnimTag, uMoveFlags, 0.0f, ACDID_INVALID, snoPowerPassability, dwWalkInPlaceTicks, nMaxTicksToWalk, 0.0f, NULL);
    }












    BOOL bSnapFacing = FALSE;
    if (tMessage.bSnapFacing.Exists())
    {
        bSnapFacing = tMessage.bSnapFacing;
    }

    SetNewFacing(aDesiredYaw, bSnapFacing, TRUE);

    if (tMessage.uAckValue.Exists())
    {
        SendAckResponse(tMessage.uAckValue);
    }
}

// Re-laid on DWARF lines 973-1114 (#line). The five constants are block statics on 1023-1027
// (DWARF: function-level DIEs with no location); SmoothMovementAngle spans 1085-1092 (argument rows 1086/1087/1091).
// Row 996 has m_aNewYaw's load at col 57 and m_aDesiredYaw's at col 68: SignedAngularDifference(m_aNewYaw, m_aDesiredYaw),
// with MathGeneral.inl's body NormalizeAngle(b - a) (DWARF param locations of every inlined copy).


#line 973
void CActorMovement::UpdateFacing()

{








    RActor * ptRActor = RActorGet(m_idRActor);
    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    BOOL bFrozen = ptACD && ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_FROZEN));

    if (!bFrozen)
    {



        const float cflTurnExpApproach = 0.7f;


        Angle aDeltaYaw = Math::SignedAngularDifference(m_aNewYaw, m_aDesiredYaw);

        Angle aNewYaw = 0.0f;
        const Angle aMinYaw = Math::DegToAngle(1.0f);
        if (Math::Abs(aDeltaYaw) < aMinYaw)
        {

            aNewYaw = m_aDesiredYaw;
            m_aYawVelocity = 0.0f;

            if (PlayerIsLocal(ptRActor->id))
            {
                m_bTurn180 = FALSE;
            }
        }



        else if (PlayerIsLocal(ptRActor->id))
        {





            ScopedGlobalDefinitionLockR pdefGlobal;

            static const float IS_MOVING_THRESHOLD = 0.12f;
            static const float TURN_180_TIME = 0.58f;
            static const float TURN_180_THRESHOLD = Math::DegToAngle(120.0f);
            static const float DELTA_YAW_MIN_LIMIT = Math::DegToAngle(10.0f);
            static const float DELTA_YAW_MAX_LIMIT = Math::DegToAngle(155.0f);

            float flYawVelocityLimiterPerTick = Math::DegToAngle(pdefGlobal->flControllerNormalTurnSpeed) / 60.0f;
            float flTurn180VelocityPerTick = PI / TURN_180_TIME / 60.0f;


            if (!m_bTurn180 && m_flCurrentSpeed < IS_MOVING_THRESHOLD &&

                Math::Abs(aDeltaYaw) > TURN_180_THRESHOLD)
            {
                m_bTurn180 = TRUE;
                m_bTurn180Trigger = TRUE;
                m_flTurn180AnimScale = PI / Math::Abs(aDeltaYaw);
            }


            if (m_flCurrentSpeed >= IS_MOVING_THRESHOLD)
            {
                m_bTurn180 = FALSE;
            }


            if (m_bTurn180)
            {
                m_aYawVelocity = Math::Pin(aDeltaYaw, -flTurn180VelocityPerTick, flTurn180VelocityPerTick);
                aNewYaw = Math::NormalizeAngle(m_aNewYaw + m_aYawVelocity);
            }
            else if (m_flCurrentSpeed > IS_MOVING_THRESHOLD ||

                Math::Abs(aDeltaYaw) < DELTA_YAW_MIN_LIMIT || Math::Abs(aDeltaYaw) > DELTA_YAW_MAX_LIMIT)
            {


                m_aYawVelocity = aDeltaYaw * cflTurnExpApproach;
                aNewYaw = Math::NormalizeAngle(m_aNewYaw + m_aYawVelocity);
            }
            else
            {
                m_aYawVelocity = Math::Pin(aDeltaYaw, -flYawVelocityLimiterPerTick, flYawVelocityLimiterPerTick);
                aNewYaw = Math::NormalizeAngle(m_aNewYaw + m_aYawVelocity);
            }
        }
        else
        {


            ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
            Angle aMaxTurningRate = TagMapGetFloat(pdefActor->hTagMap, 0x10063);
            Angle aMaxTurningAcceleration = TagMapGetFloat(pdefActor->hTagMap, 0x10064);
            Angle aMaxTurningDecceleration = TagMapGetFloat(pdefActor->hTagMap, 0x10065);

            if (ptACD)
            {
                aMaxTurningRate *= ptACD->AttributesGetFloat(FastAttribKey(ATTRIBUTE_MAX_TURN_RATE_SCALAR));
                aMaxTurningAcceleration *= ptACD->AttributesGetFloat(FastAttribKey(ATTRIBUTE_MAX_TURN_ACCEL_SCALAR));
                aMaxTurningDecceleration *= ptACD->AttributesGetFloat(FastAttribKey(ATTRIBUTE_MAX_TURN_DECCEL_SCALAR));
            }

            SmoothMovementAngle(
                m_aYawVelocity,
                m_aNewYaw,
                aMaxTurningRate,
                aMaxTurningAcceleration,
                aMaxTurningDecceleration,
                m_aDesiredYaw,
                aNewYaw);

            m_aYawVelocity = Math::SignedAngularDifference(m_aNewYaw, aNewYaw);
        }


        m_aOldYaw = m_aNewYaw;
        m_aNewYaw = aNewYaw;
        m_flNewYawTime = float(CGameGetClientTicks() + 1) * (1.0f / 60.0f);

        if (ptACD)
        {
            WorldLocation & loc = ptACD->GetWorldLocation();
            loc.transform.q.SetRotation(cgwvZAxis, aNewYaw);
        }
        else
        {
            RActorInternalSetCurrentFacing(ptRActor, aNewYaw);
        }
    }

    UpdateAnimBlend();
}
#line 2477
CRefString CActorMovement::DebugGetDumpString()
{

    RActor * ptRActor = RActorTryToGet(m_idRActor);
    WorldPlace tPlace;
    float flRadius;
    GetPlaceAndRadius(tPlace, flRadius, FALSE);
    CRefString strResult;
    CRefString strTemp;
    strResult.Format("RActor = %s\n", ptRActor ? MarkerNameGetString(ptRActor->tName) : "INVALID RACTOR");
    strTemp.Format("SWorldID = 0x%x [%.4f, %.4f, %.4f] Radius = %.4f\n", tPlace.idSWorld, tPlace.wp.x, tPlace.wp.y, tPlace.wp.z, flRadius);
    strResult += strTemp;
    strTemp.Format("NavCell SSceneID (0x%x) [%s], uCellIndex = 0x%x\n", m_hNavCell.idSScene, SceneCommonDataTryToGet(m_hNavCell.idSScene) ? SNOToString(SCENE_SNO, SceneCommonDataGet(m_hNavCell.idSScene).snoScene, FALSE).str() : "INVALID SCENE", m_hNavCell.uCellIndex);
    strResult += strTemp;
    ACDID idACD = GetACDID();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        SSceneID idSScene = ACDGetCurrentSSceneID(*ptACD);
        strTemp.Format("ACD SSceneID (0x%x) [%s]\n", idSScene, SceneCommonDataTryToGet(idSScene) ? SNOToString(SCENE_SNO, SceneCommonDataGet(idSScene).snoScene, FALSE).str() : "INVALID SCENE");
    }
    else
        strTemp = "INVALID ACD\n";
    strResult += strTemp;
    if (!ptRActor)
        strTemp = "INVALID RACTOR\n";
    else if (ptRActor->tLocation.eActorLocation == AL_WORLD)
    {
        SSceneID idSScene = ptRActor->tLocation.tWorldLocation.idSScene;
        strTemp.Format("RActor SSceneID (0x%x) [%s]\n", idSScene, SceneCommonDataTryToGet(idSScene) ? SNOToString(SCENE_SNO, SceneCommonDataGet(idSScene).snoScene, FALSE).str() : "INVALID SCENE");
    }
    else
        strTemp.Format("Actor Location is not AL_WORLD! (%d)\n", ptRActor->tLocation.eActorLocation);
    strResult += strTemp;
    NavMeshHandle hNavMeshHandle;
    ScenesCommonNavMeshNodeFromPlace(tPlace, &hNavMeshHandle, FALSE, FALSE);
    strTemp.Format("SSceneID from place: (0x%x) [%s]\n", hNavMeshHandle.idSScene, SceneCommonDataTryToGet(hNavMeshHandle.idSScene) ? SNOToString(SCENE_SNO, SceneCommonDataGet(hNavMeshHandle.idSScene).snoScene, FALSE).str() : "INVALID SCENE");
    strResult += strTemp;
    return strResult;
}
