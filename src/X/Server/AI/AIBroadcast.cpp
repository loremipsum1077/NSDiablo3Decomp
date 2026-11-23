// Original file: X/Server/AI/AIBroadcast.cpp
#include "../Actors/Actors.h"
#include "./ActorAI.h"
#include "../SGame.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/MarkerTags.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/MemoryPools.h"

struct Player;
struct LooteeInfo;
void ACDsCylinderIntersectSphere(const WorldPlace & placeCenter, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);  // ActorCollision.cpp:349
void AIStateMerge(ActorID idActor, SNO snoAIState, uint32 uLifetime, ACDID idACDSource);  // X/Server/AI/AIStates.cpp:6195
BOOL ConditionEvaluate(const SNO snoCondition, const Player * ptPlayer, const ACDID idACD, const LooteeInfo * ptLooteeInfo);  // ConditionDefinition.cpp:640

struct BroadcastTags {  // size 0x18
    int32 nActorTagSNO;  // 0x00
    int32 nActorTagRadius;  // 0x04
    int32 nActorTagCondition;  // 0x08
    int32 nMarkerTagSNO;  // 0x0c
    int32 nMarkerTagRadius;  // 0x10
    int32 nMarkerTagCondition;  // 0x14
};

//@ 32
static const BroadcastTags scgtBroadcastTags[8] = {
    { TAG_ACTOR_AI_BROADCAST_SNO_0, TAG_ACTOR_AI_BROADCAST_RADIUS_0, TAG_ACTOR_AI_BROADCAST_CONDITION_0, TAG_MARKER_AI_BROADCAST_SNO_0, TAG_MARKER_AI_BROADCAST_RADIUS_0, TAG_MARKER_AI_BROADCAST_CONDITION_0 },
    { TAG_ACTOR_AI_BROADCAST_SNO_1, TAG_ACTOR_AI_BROADCAST_RADIUS_1, TAG_ACTOR_AI_BROADCAST_CONDITION_1, TAG_MARKER_AI_BROADCAST_SNO_1, TAG_MARKER_AI_BROADCAST_RADIUS_1, TAG_MARKER_AI_BROADCAST_CONDITION_1 },
    { TAG_ACTOR_AI_BROADCAST_SNO_2, TAG_ACTOR_AI_BROADCAST_RADIUS_2, TAG_ACTOR_AI_BROADCAST_CONDITION_2, TAG_MARKER_AI_BROADCAST_SNO_2, TAG_MARKER_AI_BROADCAST_RADIUS_2, TAG_MARKER_AI_BROADCAST_CONDITION_2 },
    { TAG_ACTOR_AI_BROADCAST_SNO_3, TAG_ACTOR_AI_BROADCAST_RADIUS_3, TAG_ACTOR_AI_BROADCAST_CONDITION_3, TAG_MARKER_AI_BROADCAST_SNO_3, TAG_MARKER_AI_BROADCAST_RADIUS_3, TAG_MARKER_AI_BROADCAST_CONDITION_3 },
    { TAG_ACTOR_AI_BROADCAST_SNO_4, TAG_ACTOR_AI_BROADCAST_RADIUS_4, TAG_ACTOR_AI_BROADCAST_CONDITION_4, TAG_MARKER_AI_BROADCAST_SNO_4, TAG_MARKER_AI_BROADCAST_RADIUS_4, TAG_MARKER_AI_BROADCAST_CONDITION_4 },
    { TAG_ACTOR_AI_BROADCAST_SNO_5, TAG_ACTOR_AI_BROADCAST_RADIUS_5, TAG_ACTOR_AI_BROADCAST_CONDITION_5, TAG_MARKER_AI_BROADCAST_SNO_5, TAG_MARKER_AI_BROADCAST_RADIUS_5, TAG_MARKER_AI_BROADCAST_CONDITION_5 },
    { TAG_ACTOR_AI_BROADCAST_SNO_6, TAG_ACTOR_AI_BROADCAST_RADIUS_6, TAG_ACTOR_AI_BROADCAST_CONDITION_6, TAG_MARKER_AI_BROADCAST_SNO_6, TAG_MARKER_AI_BROADCAST_RADIUS_6, TAG_MARKER_AI_BROADCAST_CONDITION_6 },
    { TAG_ACTOR_AI_BROADCAST_SNO_7, TAG_ACTOR_AI_BROADCAST_RADIUS_7, TAG_ACTOR_AI_BROADCAST_CONDITION_7, TAG_MARKER_AI_BROADCAST_SNO_7, TAG_MARKER_AI_BROADCAST_RADIUS_7, TAG_MARKER_AI_BROADCAST_CONDITION_7 },
};

//@ 46
void AIBroadcastSetActive(Actor & tActor, BOOL bOn)

{
    if (bOn)
    {
        for (int32 i = 0; i < 8; i++)
        {
            SNO snoAIState = ActorGetInheritedTagMapValueSNO(&tActor, scgtBroadcastTags[i].nActorTagSNO, scgtBroadcastTags[i].nMarkerTagSNO);
            if (snoAIState != SNO_NONE)
            {
                ActorAISetStatusFlag(tActor.id, ACTOR_AI_BROADCAST_BIT, TRUE);
                return;
            }
        }
    }

//@ 63
    ActorAISetStatusFlag(tActor.id, ACTOR_AI_BROADCAST_BIT, FALSE);
}

//@ 68
static void sApplyBroadcastToRadius(ActorCommonData & tACD, SNO snoAIState, float flRadius, SNO snoCondition)




{
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    WorldPlace placeCurr = tACD.GetPlace();
    ACDsCylinderIntersectSphere(placeCurr, flRadius, tACD.id, listTargets, 7);

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDCurr = (ACDID)listTargets.GetNext(pos);
        ActorCommonData * ptACDCurr = ACDTryToGet(idACDCurr);
        if (ptACDCurr && ptACDCurr->IsAlive() && ptACDCurr->IsMonster())
        {
            if (ConditionEvaluate(snoCondition, NULL, idACDCurr, NULL))
            {
                uint32 uLifetime = 31;
                ActorID idActorCurr = (ActorID)ACDGetOwner(idACDCurr);
                AIStateMerge(idActorCurr, snoAIState, uLifetime, tACD.id);
            }
        }
    }
}

//@ 97
void AIBroadcastUpdate(Actor & tActor)
{
    ACDID idACD = ACDFindByActor(tActor.id);
    ActorCommonData & tACD = ACDGet(idACD);
    ActorAI * ptAI = ActorTryToGetAI(tActor.id);
    if (ptAI == NULL)
    {
        return;
    }

//@ 108
    for (int32 i = 0; i < 8; i++)
    {
        SNO snoAIState = ActorGetInheritedTagMapValueSNO(&tActor, scgtBroadcastTags[i].nActorTagSNO, scgtBroadcastTags[i].nMarkerTagSNO);
        if (snoAIState != SNO_NONE)
        {
            float flRadius = ActorGetInheritedTagMapValueFloat(&tActor, scgtBroadcastTags[i].nActorTagRadius, scgtBroadcastTags[i].nMarkerTagRadius);
            SNO snoCondition = ActorGetInheritedTagMapValueSNO(&tActor, scgtBroadcastTags[i].nActorTagCondition, scgtBroadcastTags[i].nMarkerTagCondition);
            sApplyBroadcastToRadius(tACD, snoAIState, flRadius, snoCondition);
        }
    }

//@ 120
    uint32 uNow = GameGetServerTime();
    ptAI->dwNextBroadcastTime = uNow + 30;
}
