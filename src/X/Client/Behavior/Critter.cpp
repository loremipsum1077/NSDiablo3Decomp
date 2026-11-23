// Original file: X/Client/Behavior/Critter.cpp
#include "Behavior.h"
#include "../CGame.h"
#include "../Explosions.h"
#include "../Animation.h"
#include "../Movement/CActorMovement.h"
#include "../RActors/RActors.h"
#include "../RActors/RActorAnimation.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/AnimCommon.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Definitions/AnimationDefinition.h"
#include "../../Common/Definitions/AnimTreeDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/ItemsSwig.h"
#include "../../Common/Nav/NavCellHandle.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/Nav/NavZone.h"
#include "../../Common/Player.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Random.h"

// Declared here because their headers do not declare them yet. Each comment names the defining file.
WorldPlace RActorGetWorldPlace(const RActor * ptRActor);  // X/Client/RActors/RActors.cpp:6737
Player * PlayerFindClosest(const WorldPlace & tPlace, const WorldDistance wdRadius);  // X/Common/Player.cpp:3052
uint32 NavMeshGetDynamicFlagsForPassability(PassabilityType ePassability);  // X/Common/Nav/NavigationMesh.cpp:3187
BOOL WorldCollSearchForLocation(const WorldPlace & placeStart, uint16 uOneRequiredStaticFlags, uint32 uObjectNoOverlapFlags, float flRadius, float flMinSearchRadius, float flMaxSearchRadius, ACDID idACDIgnore, Vector3D & wpEnd, NavCellHandle & hNavCellEnd, int32 eTeam, uint16 uArg10, ACDID idACDArg11);  // X/Common/Nav/NavigationMesh.cpp
// X/Client/RActors/RActorAnimation.cpp, X/Client/Animation.cpp
BOOL RActorAnimationIsAnimTreeEnabled(const RActor & tRActor); BOOL RActorGetAnimLayerForTag(RActor * ptRActor, uint32 dwAnimationTag, AnimLayer & eAnimLayer); uint32 RActorGetAnimTreeLayerHash(AnimLayer eAnimLayer); BOOL RActorGetAnimTreeLayerImmediatelyActive(AnimLayer eAnimLayer); BOOL RActorGetAnimTreeManualRampOut(RActor * ptRActor, uint32 dwAnimationTag);
SNO AnimationChannelGetSNO(const AnimChannel & tChannel); SNO AnimTreeLayerGetSNO(const AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash); void AnimationChannelPlay(RActor * ptRActor, const AnimChannelPlayParams & tChannelPlayParams, BOOL bDisableBlending); void AnimTreeLayerPlay(RActor * ptRActor, uint32 dwLayerHash, const AnimChannelPlayParams & tChannelPlayParams, BOOL bImmediatelyActive, BOOL bManualRampOut);

enum CritterState {
    CS_INVALID = -1,
    CS_NONE = 0,
    CS_IDLE = 1,
    CS_WANDER = 2,
    CS_DEAD = 3,
    CS_NUM_STATES = 4,
};

struct CritterBehaviorData {  // size 0x20
    CritterState eState;  // 0x00
    float flNextWanderTime;  // 0x04
    WorldPlace tPlaceWanderDestination;  // 0x08
    float flStartedWanderingTime;  // 0x18
    float flScaredUntilTime;  // 0x1c
};
static_assert(sizeof(CritterBehaviorData) == 0x20, "CritterBehaviorData");











static WorldDistance CLOSE_ENOUGH_DISTANCE = 5.0f;
static WorldDistance CLOSE_ENOUGH_DISTANCE_SQ = CLOSE_ENOUGH_DISTANCE * CLOSE_ENOUGH_DISTANCE;

static void sStartWander(RActor * ptRActor, const WorldPlace * ptPlaceDest, const BOOL & bScared);








static void sCritterBehaviorDataInit(CritterBehaviorData & tCritterData)
{

    tCritterData.tPlaceWanderDestination.idSWorld = SWORLDID_INVALID;
    tCritterData.tPlaceWanderDestination.wp.Set(0.0f, 0.0f, 0.0f);
    tCritterData.eState = CS_IDLE;
    tCritterData.flNextWanderTime = 0.0f;
    tCritterData.flStartedWanderingTime = 0.0f;
    tCritterData.flScaredUntilTime = 0.0f;
}



CritterBehaviorData & sGetCritterData(RActor * ptRActor)
{

    const Behavior * ptBehavior = BehaviorGet(ptRActor);


    return *(CritterBehaviorData *)ptBehavior->ptBehaviorData;
}



static BOOL isCurrentlyScared(RActor *, const CritterBehaviorData & tCritterData)
{


    float flNow = CGameGetClientTime();
    return flNow < tCritterData.flScaredUntilTime;
}



static void sSetWanderTimer(RActor * ptRActor, CritterBehaviorData & tCritterData)
{


    float flMinDelay = (float)ActorDefinitionGetTagMapValueDWORD(TAG_ACTOR_BEHAVIOR_CRITTER_MIN_WANDER_DELAY, ptRActor->snoActor) * (1.0f / 60.0f);
    float flMaxDelay = (float)ActorDefinitionGetTagMapValueDWORD(TAG_ACTOR_BEHAVIOR_CRITTER_MAX_WANDER_DELAY, ptRActor->snoActor) * (1.0f / 60.0f);

    float flNow = CGameGetClientTime();
    tCritterData.flNextWanderTime = flNow + GameRandRangeFloat(flMinDelay, flMaxDelay);
}




void BehaviorCritterInit(RActor * ptRActor)

{
    Behavior * ptBehavior = BehaviorGet(ptRActor);



    CritterBehaviorData * ptCritterData = (CritterBehaviorData *)SigmaMemoryNew(sizeof(CritterBehaviorData), 4, NULL, TRUE);


    sCritterBehaviorDataInit(*ptCritterData);



    ptBehavior->ptBehaviorData = ptCritterData;


    sSetWanderTimer(ptRActor, *ptCritterData);

}



void BehaviorCritterDispose(RActor * ptRActor)

{
    Behavior * ptBehavior = BehaviorGet(ptRActor);


    SigmaMemoryFree(ptBehavior->ptBehaviorData, NULL);
    ptBehavior->ptBehaviorData = NULL;
}



static BOOL sCritterCanBeScared(RActor * ptRActor)
{

    BOOL bCanBeScared = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_BEHAVIOR_SCARED_OF_PLAYERS, ptRActor->snoActor);
    return bCanBeScared;
}



static BOOL sPlaceIsAroundScaryThings(RActor * ptRActor, const WorldPlace & tPlace)
{


    WorldDistance wdScaredRadiusSq = ActorDefinitionGetTagMapValueFloat(TAG_ACTOR_BEHAVIOR_CRITTER_SCARED_RADIUS, ptRActor->snoActor);
    wdScaredRadiusSq *= wdScaredRadiusSq;


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {


        WorldPlace tPlacePlayer = ACDGet(ptPlayer->idPlayerACD).GetPlace();
        if (WorldPlaceInSameWorld(tPlace, tPlacePlayer))
        {


            WorldDistance wdDistSq = WorldPlaceDistanceSquared(tPlace, tPlacePlayer);
            if (wdDistSq <= wdScaredRadiusSq)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}







static BOOL sFindWanderLocation(RActor * ptRActor, WorldPlace & tPlace)
{



    WorldPlace tPlaceSource = RActorGetWorldPlace(ptRActor);
    WorldPlace tPlaceEnd = tPlaceSource;


    WorldDistance wdMinWanderRadius = ActorDefinitionGetTagMapValueFloat(TAG_ACTOR_BEHAVIOR_CRITTER_MIN_WANDER_RADIUS, ptRActor->snoActor);
    WorldDistance wdMaxWanderRadius = ActorDefinitionGetTagMapValueFloat(TAG_ACTOR_BEHAVIOR_CRITTER_MAX_WANDER_RADIUS, ptRActor->snoActor);

    NavCellHandle hNavCellStart;
    NavCellHandle hNavCellEnd;
    Vector3D wpEnd;



    uint32 uObjectNoOverlapFlags = NavMeshGetDynamicFlagsForPassability(PASSABILITY_MONSTER);

    NavCellHandleFromWorldPlace(tPlaceSource, hNavCellStart);

    if (hNavCellStart.idSScene == SSCENEID_INVALID)
    {
        return FALSE;
    }
    uint16 uOneRequiredStaticFlags = 1;
    float flMyRadius = 1.0f;
    float flRadiusRange = Math::Max(wdMaxWanderRadius - wdMinWanderRadius - flMyRadius, 2.5f);
    int32 nTries = 32;
    BOOL bFound = FALSE;

    RNG & tRNG = CGameGlobalsGet().rngLocal;




    for (int32 i = 0; i < nTries; i++)
    {
        float flRadius = wdMinWanderRadius + RandFloat_Internal(tRNG) * flRadiusRange;


        bFound = WorldCollSearchForLocation(tPlaceSource, uOneRequiredStaticFlags, uObjectNoOverlapFlags, flMyRadius, flRadius, wdMaxWanderRadius, ACDID_INVALID, wpEnd, hNavCellEnd, 1, 0, ACDID_INVALID);











        if (bFound == TRUE)
        {
            tPlaceEnd.wp = wpEnd;


            if (sCritterCanBeScared(ptRActor) == TRUE &&
                sPlaceIsAroundScaryThings(ptRActor, tPlaceEnd))
            {
                continue;
            }


            tPlace = tPlaceEnd;
            break;
        }
    }

    return bFound;
}






static void sStartWander(RActor * ptRActor, const WorldPlace * ptPlaceDest, const BOOL & bScared)



{
    CritterBehaviorData & tCritterData = sGetCritterData(ptRActor);


    if (!ptPlaceDest)
    {
        BOOL bPicked = sFindWanderLocation(ptRActor, tCritterData.tPlaceWanderDestination);
        if (!bPicked)
        {


            sSetWanderTimer(ptRActor, tCritterData);
            return;
        }
    }
    else
    {



        tCritterData.tPlaceWanderDestination = *ptPlaceDest;
    }

    uint32 dwAnimTag = TAG_ACTOR_ANIMATION_WALK;
    ActorTag eSpeedTag = TAG_ACTOR_BEHAVIOR_CRITTER_WANDER_SPEED;

    if (bScared)
    {
        eSpeedTag = TAG_ACTOR_BEHAVIOR_CRITTER_RUN_SPEED;

        BOOL bRunWhenScared = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_BEHAVIOR_CRITTER_RUN_ANIM_WHEN_SCARED, ptRActor->snoActor);

        if (bRunWhenScared)
        {
            dwAnimTag = TAG_ACTOR_ANIMATION_RUN;
        }
    }
    float flSpeed = ActorDefinitionGetTagMapValueFloat(eSpeedTag, ptRActor->snoActor);


    if (ptRActor->ptCActorMovement)
    {
        uint32 uMoveFlags = 0x16;








        ptRActor->ptCActorMovement->MoveTo(tCritterData.tPlaceWanderDestination, flSpeed, 0.0f, dwAnimTag, uMoveFlags, 0.0f, ACDID_INVALID, SNO_NONE, 0, -1, 0.0f, NULL);













        tCritterData.flStartedWanderingTime = CGameGetClientTime();


        tCritterData.eState = CS_WANDER;
    }
}



static void sUpdateIdle(RActor * ptRActor)
{

    CritterBehaviorData & tCritterData = sGetCritterData(ptRActor);


    if (isCurrentlyScared(ptRActor, tCritterData))
    {
        return;
    }
    float flNow = CGameGetClientTime();


    if (flNow >= tCritterData.flNextWanderTime)
    {


        sStartWander(ptRActor, NULL, FALSE);
    }
}






static void sCancelWander(RActor * ptRActor, CritterBehaviorData & tCritterData)
{




    if (tCritterData.eState == CS_WANDER)
    {


        tCritterData.flStartedWanderingTime = 0.0f;
        tCritterData.eState = CS_IDLE;


        if (ptRActor->ptCActorMovement)
        {
            ptRActor->ptCActorMovement->StopNow();
        }


        sSetWanderTimer(ptRActor, tCritterData);
    }
}





static void sUpdateWander(RActor * ptRActor)
{

    CritterBehaviorData & tCritterData = sGetCritterData(ptRActor);
    WorldPlace tPlace = RActorGetWorldPlace(ptRActor);
    float flNow = CGameGetClientTime();


    float flWanderTime = flNow - tCritterData.flStartedWanderingTime;


    if (tPlace.idSWorld == tCritterData.tPlaceWanderDestination.idSWorld)
    {
        WorldDistance wdDistSq = WorldPlaceDistanceSquared(tPlace, tCritterData.tPlaceWanderDestination);


        float flTooLongTime = (float)ActorDefinitionGetTagMapValueDWORD(TAG_ACTOR_BEHAVIOR_CRITTER_WANDER_TOO_LONG_TIME, ptRActor->snoActor) * (1.0f / 60.0f);
        if (wdDistSq > CLOSE_ENOUGH_DISTANCE_SQ && flWanderTime < flTooLongTime)
        {
            return;
        }
    }


    sCancelWander(ptRActor, tCritterData);
}



static void sCritterPlayAnimation(RActor * ptRActor, uint32 dwAnimTag)
{





    if (!ptRActor->ptActorAnim)
    {
        return;
    }
    SNO snoCurrentAnim = SNO_NONE;
    BOOL bAnimTreeEnabled = RActorAnimationIsAnimTreeEnabled(*ptRActor);

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

    SNO snoAnim = ActorGetAnim(ptRActor->snoActor, WEAPONCLASS_NONE, dwAnimTag);
    if (snoCurrentAnim == snoAnim)
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
            uint32 dwAnimLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
            BOOL bImmediatelyActive = RActorGetAnimTreeLayerImmediatelyActive(eAnimLayer);
            BOOL bManualRampOut = RActorGetAnimTreeManualRampOut(ptRActor, dwAnimTag);
            AnimTreeLayerPlay(ptRActor, dwAnimLayerHash, tChannelPlayParams, bImmediatelyActive, bManualRampOut);
        }
    }
}



static void sKillCritter(RActor * ptRActorCritter)

{
    CritterBehaviorData & tCritterData = sGetCritterData(ptRActorCritter);

    sCancelWander(ptRActorCritter, tCritterData);


    tCritterData.eState = CS_DEAD;


    sCritterPlayAnimation(ptRActorCritter, TAG_ACTOR_ANIMATION_DEATH_01);

    ptRActorCritter->flLife = ptRActorCritter->flTimeUntilFade;



    ptRActorCritter->dwFlags |= 0x400;
}



static void sUpdateCrush(RActor * ptRActorCritter, CritterBehaviorData & tCritterData)
{


    WorldPlace tPlaceCritter = RActorGetWorldPlace(ptRActorCritter);


    if (tCritterData.eState == CS_DEAD)
    {
        return;
    }


    BOOL bCanBeCrushed = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_BEHAVIOR_CRITTER_CAN_BE_SQUISHED, ptRActorCritter->snoActor);
    if (!bCanBeCrushed)
    {
        return;
    }


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {


        const RActor * ptRActorPlayer = RActorTryToGet(ptPlayer->idPlayerRActor);




        if (ptRActorPlayer && ptRActorPlayer->ptCActorMovement && ptRActorPlayer->ptCActorMovement->IsMoving())
        {


            WorldPlace tPlacePlayer = RActorGetWorldPlace(ptRActorPlayer);
            WorldDistance wdDist = WorldPlaceDistance(tPlaceCritter, tPlacePlayer);


            if (wdDist <= 2.0f)
            {


                sKillCritter(ptRActorCritter);
            }
        }
    }
}








static void sFindScaredRunToDestination(RActor * ptRActor, const WorldPlace & tPlaceSource, const WorldPlace & tPlaceScaredOf, WorldPlace & tPlaceChosen)
{
    Vector3D wvFromScaryThing;





    WorldPlaceGetVectorBetween(tPlaceScaredOf, tPlaceSource, wvFromScaryThing);
    wvFromScaryThing.Normalize();




    WorldDistance wdMinScaredRunAwayDistance = ActorDefinitionGetTagMapValueFloat(TAG_ACTOR_BEHAVIOR_CRITTER_MIN_SCARED_RUN_AWAY_DISTANCE, ptRActor->snoActor);
    WorldDistance wdMaxScaredRunAwayDistance = ActorDefinitionGetTagMapValueFloat(TAG_ACTOR_BEHAVIOR_CRITTER_MAX_SCARED_RUN_AWAY_DISTANCE, ptRActor->snoActor);
    WorldDistance wdScaredRunAwayDistance = ClientRandRangeFloat(wdMinScaredRunAwayDistance, wdMaxScaredRunAwayDistance);
    wvFromScaryThing.x *= wdScaredRunAwayDistance;
    wvFromScaryThing.y *= wdScaredRunAwayDistance;


    tPlaceChosen.idSWorld = tPlaceSource.idSWorld;
    tPlaceChosen.wp.Set(
        wvFromScaryThing.x + tPlaceScaredOf.wp.x,
        wvFromScaryThing.y + tPlaceScaredOf.wp.y,
        wvFromScaryThing.z + tPlaceScaredOf.wp.z);
}




static void sUpdateScared(RActor * ptRActor, CritterBehaviorData & tCritterData)
{


    float flNow = CGameGetClientTime();


    if (tCritterData.eState == CS_DEAD)
    {
        return;
    }


    if (flNow < tCritterData.flScaredUntilTime)
    {
        return;
    }


    if (!sCritterCanBeScared(ptRActor))
    {
        return;
    }


    WorldPlace tPlaceCritter = RActorGetWorldPlace(ptRActor);


    WorldDistance wdScaredRadius = ActorDefinitionGetTagMapValueFloat(TAG_ACTOR_BEHAVIOR_CRITTER_SCARED_RADIUS, ptRActor->snoActor);
    const Player * ptPlayer = PlayerFindClosest(tPlaceCritter, wdScaredRadius);
    if (ptPlayer)
    {
        WorldPlace tPlaceDest;


        sFindScaredRunToDestination(
            ptRActor,
            tPlaceCritter,
            ACDGet(ptPlayer->idPlayerACD).GetPlace(),
            tPlaceDest);


        sStartWander(ptRActor, &tPlaceDest, TRUE);



        float flMinScaredTime = (float)ActorDefinitionGetTagMapValueInt(TAG_ACTOR_BEHAVIOR_CRITTER_MIN_SCARED_TIME, ptRActor->snoActor) * (1.0f / 60.0f);
        float flMaxScaredTime = (float)ActorDefinitionGetTagMapValueInt(TAG_ACTOR_BEHAVIOR_CRITTER_MAX_SCARED_TIME, ptRActor->snoActor) * (1.0f / 60.0f);
        float flScaredTime = ClientRandRangeFloat(flMinScaredTime, flMaxScaredTime);
        tCritterData.flScaredUntilTime = flNow + flScaredTime;
    }
}





BehaviorStatus BehaviorCritterUpdate(RActor * ptRActor)

{
    CritterBehaviorData & tCritterData = sGetCritterData(ptRActor);










    switch (tCritterData.eState)
    {
    case CS_DEAD:
        return BEHAVIOR_STATUS_OK;

    case CS_IDLE:





        sUpdateIdle(ptRActor);
        break;

    case CS_WANDER:



        sUpdateWander(ptRActor);
        break;

    default:









        DisplayInternalError(NULL, "", 714, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    }




    sUpdateScared(ptRActor, tCritterData);


    sUpdateCrush(ptRActor, tCritterData);

    return BEHAVIOR_STATUS_OK;
}




void BehaviorCritterExplosion(const RActorID idRActor, const ExplosionImpulse & tExplosion)

{
    if (!tExplosion.IsMelee() && idRActor != RACTORID_INVALID)
    {
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {
            float flLethalExplosionMag = ActorDefinitionGetTagMapValueFloat(TAG_ACTOR_BEHAVIOR_CRITTER_LETHAL_EXPLOSION_MAGNITUDE, ptRActor->snoActor);
            WorldPlace tPlaceCritter = RActorGetWorldPlace(ptRActor);
            Vector3D vImpulse = tExplosion.ComputeImpulse(tPlaceCritter.wp);
            if (vImpulse.MagnitudeSquared() >= flLethalExplosionMag * flLethalExplosionMag)
            {
                sKillCritter(ptRActor);
            }
        }
    }
}




BOOL RActorIsCritterWithRActorID(const RActorID idRActor)

{
    RActor * ptRActor = RActorGet(idRActor);
    const Behavior * ptBehavior = BehaviorGet(ptRActor);
    if (ptBehavior && ptBehavior->eType == BT_CRITTER)
    {
        return TRUE;
    }

    return FALSE;
}



BOOL RActorIsCritterWithSNO(SNO snoActor)

{
    BehaviorType eBehaviorType = BT_INVALID;

    if (snoActor == SNO_NONE)
    {
        return FALSE;
    }

    ScopedActorDefinitionLockR ptDefActor(snoActor);
    const CTagMap * hTagMap = ptDefActor->hTagMap;
    if (TagMapTagExists(hTagMap, TAG_ACTOR_BEHAVIOR_TYPE))
    {
        eBehaviorType = TagMapGetInt(hTagMap, TAG_ACTOR_BEHAVIOR_TYPE);
    }

    return eBehaviorType == BT_CRITTER;
}
