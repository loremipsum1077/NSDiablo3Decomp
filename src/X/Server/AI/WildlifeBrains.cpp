// Original file: X/Server/Ai/WildlifeBrains.cpp
#include "../Actors/Actors.h"
#include "./ActorAI.h"
#include "./BrainComponents.h"
#include "./BrainStance.h"
#include "./ProximityDetectorBrain.h"
#include "./WildlifeBrains.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/AI/ActorAISwig.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Monsters.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/MemoryPools.h"

enum { ACTOR_SPHERE_TREE_FLAGS_ALL = 7 };  // X/Common/Actors/ActorCollision.h
uint32 AnimPlayerGetCurrentAnimTag(ActorCommonData & tACD);  // X/Common/AnimPlayer.cpp:1856
void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);  // ActorCollision.cpp:317
ACDID BrainAPIActorGetClosestTarget(ActorID idActor, SNO snoPower, BOOL bTargetFriendlies, BrainAPITargetFilterCallback pfnFilterCallback);  // X/Server/AI/BrainComponents.cpp:2542
void SimpleBrainActionUpdate(ActorID idActor);  // X/Server/Ai/SanctuaryBrains.cpp:80
void ProximityDetectorBrainInitialize(const ActorID idActor);  // X/Server/AI/ProximityDetectorBrain.cpp:118
ProximityDetectorBrainData * ProximityDetectorTryGetData(ProximityDetectorDataID idData);  // X/Server/AI/ActorAI.cpp:4719

enum {
    WOODWRAITH_FLAG_ACTIVATED = 3,
    WOODWRAITH_FLAG_DONE_TRANSFORMING = 0,
};

enum {
    WOODWRAITH_NUM_INACTIVE_STATES = 3,
};

//@ 50
static const float WOODWRAITH_WAKEUP_RADIUS_SQR = 60.0f * 60.0f;
static const float WOODWRAITH_AWAKEN_OTHERS_RADIUS = 110.0f;

//@ 55
void WoodWraithBrainInitialize(const ActorID idActor)
{

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    ActorAI & tAI = ActorGetAI(idActor);
    tAI.pBrainData = SigmaMemoryNew(sizeof(WoodWraithBrainData), 4, SGameManager::GameGet().ptMemoryInterface, TRUE);

//@ 65
    if (!(tAI.dwPersistentFlags & (1 << WOODWRAITH_FLAG_ACTIVATED)))
    {
        uint32 dwInactiveAnimTag;
        switch (RandRange_Internal(SGameGlobalsGet().rng, 0, WOODWRAITH_NUM_INACTIVE_STATES))
        {
        case 1:
            dwInactiveAnimTag = TAG_ACTOR_ANIMATION_INACTIVE_02;
            break;
        case 2:
            dwInactiveAnimTag = TAG_ACTOR_ANIMATION_INACTIVE_03;
            break;
        default:
            dwInactiveAnimTag = TAG_ACTOR_ANIMATION_INACTIVE_01;
            break;
        }
//@ 80
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_BLOCKS_PROJECTILES), 1);
        BrainStanceSetIdleAnimation(idActor, dwInactiveAnimTag);
        tACD.SetTargetable(FALSE);
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_INVULNERABLE), 1);
        tAI.dwPersistentFlags &= ~(1 << WOODWRAITH_FLAG_DONE_TRANSFORMING);
    }
}

//@ 90
void WoodWraithBrainDispose(const ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    SigmaMemoryFree(tAI.pBrainData, SGameManager::GameGet().ptMemoryInterface); tAI.pBrainData = NULL;
}

//@ 99
void sWoodWraithWakeUp(ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    if (tAI.dwPersistentFlags & (1 << WOODWRAITH_FLAG_ACTIVATED))
    {
        return;
    }

//@ 108
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
    {
        return;
    }
    ActorCommonData & tACD = ACDGet(idACD);

    uint32 dwAnimTagAwaken;
    switch (AnimPlayerGetCurrentAnimTag(tACD))
    {
    case TAG_ACTOR_ANIMATION_INACTIVE_02:
        dwAnimTagAwaken = TAG_ACTOR_ANIMATION_TRANSFORM_02;
        break;
    case TAG_ACTOR_ANIMATION_INACTIVE_03:
        dwAnimTagAwaken = TAG_ACTOR_ANIMATION_TRANSFORM_03;
        break;
    default:
        dwAnimTagAwaken = TAG_ACTOR_ANIMATION_TRANSFORM_01;
        break;
    }
//@ 128
    tACD.ChangeDamageState(1);

    BrainAPIDoPlayAnimation(idActor, dwAnimTagAwaken, (uint32)-1, FALSE, FALSE, 0.0f, FALSE, 1.0f);
    BrainStanceSetIdleAnimation(idActor, TAG_ACTOR_ANIMATION_NEUTRAL);
    tAI.dwPersistentFlags |= (1 << WOODWRAITH_FLAG_ACTIVATED);

    ptACD->AttributesSetFloat(FastAttribKey(ATTRIBUTE_LOOK_TARGET_BROADCAST_INTENSITY), 10.0f);
    ptACD->AttributesSetFloat(FastAttribKey(ATTRIBUTE_LOOK_TARGET_BROADCAST_RADIUS), 40.0f);
}

//@ 140
static void sWoodWraithTryAwakenOthers(WorldPlace wpCenter, WorldDistance wdRadius)
{



    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    ACDsIntersectSphere(wpCenter, wdRadius, ACDID_INVALID, listACDs, ACTOR_SPHERE_TREE_FLAGS_ALL);


    POS pos = listACDs.GetHeadPosition();
    while (pos)
    {

        ACDID idACD = (ACDID)listACDs.GetNext(pos);


        ActorID idActor = (ActorID)ACDGet(idACD).idOwner;
        if (!ActorHasBeenKilled(idActor))
        {
            ActorAI * ptAI = ActorTryToGetAI(idActor);
            if (ptAI && ptAI->nBrainType == ACTORBRAIN_WOODWRAITH &&
                !(ptAI->dwPersistentFlags & (1 << WOODWRAITH_FLAG_ACTIVATED)) &&
                !ActorAIIsStatusFlagSet(idActor, ACTOR_AI_DISABLE_UPDATES_BIT))
            {
                sWoodWraithWakeUp(idActor);
                break;
            }
        }
    }


}

//@ 175
void WoodWraithBrainActionUpdate(ActorID idActor)
{

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
    {
        return;
    }
//@ 184
    ActorAI & tAI = ActorGetAI(idActor);
    if (!(tAI.dwPersistentFlags & (1 << WOODWRAITH_FLAG_ACTIVATED)))
    {

        ACDID idACDTarget = BrainAPIActorGetClosestTarget(idActor, SNO_NONE, FALSE, NULL);
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget)
        {
            WorldPlace placeCurr = ptACD->GetPlace();
            WorldPlace placeTarget = ptACDTarget->GetPlace();
            if (Vector3D::Distance2DSquared(placeCurr.wp, placeTarget.wp) <= WOODWRAITH_WAKEUP_RADIUS_SQR)
            {
                sWoodWraithWakeUp(idActor);
            }
        }
        return;
    }

//@ 204
    if (!(tAI.dwPersistentFlags & (1 << WOODWRAITH_FLAG_DONE_TRANSFORMING)))
    {
        ptACD->AttributesSetInt(FastAttribKey(ATTRIBUTE_BLOCKS_PROJECTILES), 0);
        ptACD->SetTargetable(TRUE);
        ptACD->AttributesSetInt(FastAttribKey(ATTRIBUTE_INVULNERABLE), 0);
        tAI.dwPersistentFlags |= (1 << WOODWRAITH_FLAG_DONE_TRANSFORMING);
    }

    sWoodWraithTryAwakenOthers(ptACD->GetPlace(), WOODWRAITH_AWAKEN_OTHERS_RADIUS);

    SimpleBrainActionUpdate(idActor);
}

//@ 219
void sSporePodActivated(ActorID idActor, ACDID idACDTrespasser)


{

//@ 225
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD == ACDID_INVALID)
    {
        return;
    }

    ActorCommonData & tACD = ACDGet(idACD);
    if (tACD.IsAlive())
    {




//@ 238
        BrainAPIDoDeath((ActorID)tACD.idOwner, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
    }
}

//@ 243
void SporePodBrainInitFunc(ActorID idActor)
{

    ACDID idACD = ACDFindByActor(idActor);
    if (idACD == ACDID_INVALID)
    {
        return;
    }


//@ 253
    ProximityDetectorBrainInitialize(idActor);


    ActorCommonData & tACD = ACDGet(idACD);
    MonsterSkillDeclaration tMonsterSkillDec = MonsterGetMonsterSkill(tACD.tSNOName, 0);
    float flRadius = tMonsterSkillDec.flUseRangeMax;

    ActorAI & tAI = ActorGetAI(idActor);
    ProximityDetectorBrainData * ptProxData = ProximityDetectorTryGetData(tAI.idProximityData);
    if (ptProxData)
    {
        ptProxData->tShape.eShape = AI_PROXIMITY_SHAPE_SPHERE;
        ptProxData->tShape.flRadius = flRadius;
        ptProxData->pOnPlayerEnteredCallback = sSporePodActivated;
    }
}
