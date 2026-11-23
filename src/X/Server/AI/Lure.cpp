// Original file: X/Server/AI/Lure.cpp
#include "./Lure.h"
#include "../Actors/Actors.h"
#include "./ActorAI.h"
#include "../SGame.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Nav/NavCellHandle.h"
#include "../../Common/Nav/NavZone.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/MemoryPools.h"
#include <float.h>
#include <limits.h>

// Declared here until their headers carry them: SMarker.cpp:1231, ActorCollision.cpp:349, NavigationMesh.cpp:3536, AIStates.cpp:6219 and :6233.
int32 MarkerGetACDsLinkedByActor(const ActorID idActor, XPooledDataIDList & listACDs, LPCSTR pszGroupName); void ACDsCylinderIntersectSphere(const WorldPlace & placeCenter, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);
BOOL NavMeshCheckLOSToACD(NavCellHandle hNavCellStart, const WorldPlace & placeStart, ACDID idACDDest, const PassabilityType ePassabilityType, BOOL bWantProjectileLOS);
void AIStateMergeBroadcastPower(ActorID idActor, SNO snoPower, uint32 uLifetime, ACDID idACDSource, ACDID idACDCastLocation, uint32 uPriority); BOOL AIStateHasExistingMerge(ActorID idActor, ACDID idACDSourceIgnore);

enum {
    LURE_BROADCAST_DURATION_TICKS = 50,
    LURE_POWER_PRIORITY = 50,
};

void LureUpdate(ActorCommonData & tACD, AILureData & tLureData);


BOOL LureInitialize(ActorCommonData & tACD, AILureData & tLureData)


{


    SigmaMemoryClear(&tLureData, sizeof(AILureData));



    for (int32 i = 0; i < LURE_MAX_AFFECTED; i++) { tLureData.arLured[i].idACD = ACDID_INVALID; tLureData.arLured[i].idACDLocation = ACDID_INVALID; tLureData.arLured[i].nLocationIdx = -1; }




    for (int32 i = 0; i < LURE_MAX_LOCATIONS; i++)
    {
        tLureData.arLocationUsedCount[i] = 0;
    }

    return TRUE;
}



static BOOL sOkayToLure(ActorCommonData & tACDLure, ActorCommonData & tACDTarget, BOOL bRequireNearbyAlly, BOOL bRequireNearbyEnemy)




{
    if (!bRequireNearbyAlly && !bRequireNearbyEnemy)
    {
        return TRUE;
    }



    ActorID idActorNode = (ActorID)ACDGetOwner(tACDLure.id);
    XPooledDataIDList listLinkedACDs(MemoryPoolsGetDATAIDListPool());
    MarkerGetACDsLinkedByActor(idActorNode, listLinkedACDs, NULL);
    if (listLinkedACDs.Count() == 1)
    {
        ACDID idACDCenter = (ACDID)listLinkedACDs.GetHead();
        ActorCommonData * ptACDCenter = ACDTryToGet(idACDCenter);
        if (ptACDCenter && ptACDCenter->IsAlive())
        {
            int32 bCenterDeactivated = ptACDCenter->AttributesGetInt(ATTRIBUTE_DEACTIVATE_LURE);
            if (bCenterDeactivated > 0)
            {
                return FALSE;
            }
        }
    }


    WorldPlace placeLure = tACDLure.GetPlace(); int32 nSameTeamCount = 0; int32 nHostileCount = 0;
    Team eACDTeam = tACDTarget.GetTeam();
    POS pos;

    XPooledDataIDList listSurroundingTargets(MemoryPoolsGetDATAIDListPool());
    ACDsCylinderIntersectSphere(placeLure, 40.0f, tACDTarget.id, listSurroundingTargets, 7);
    pos = listSurroundingTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDCurr = (ACDID)listSurroundingTargets.GetNext(pos);
        ActorCommonData * ptACDCurr = ACDTryToGet(idACDCurr);
        if (ptACDCurr)
        {
            if (ACDIsHostile(tACDTarget, *ptACDCurr, FALSE))
            {
                nHostileCount++;
            }
            else if (eACDTeam == ptACDCurr->GetTeam())
            {
                nSameTeamCount++;
            }
        }
    }


    if (bRequireNearbyAlly && nSameTeamCount == 0)
    {
        return FALSE;
    }

    if (bRequireNearbyEnemy && nHostileCount == 0) { return FALSE; }

    return TRUE;
}





static ACDID sGetNearestLureTarget(ActorCommonData & tACD, const WorldPlace & placeCenter, BOOL bPerformLOSCheck)



{
    ScopedActorDefinitionLockR pdefActor(tACD.snoActor);
    float flRadius = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_LURE_RADIUS);
    SNO snoActor0 = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_LURE_ACTOR_SNO_0);
    SNO snoActor1 = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_LURE_ACTOR_SNO_1);
    SNO snoActor2 = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_LURE_ACTOR_SNO_2);
    SNO snoActor3 = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_LURE_ACTOR_SNO_3);
    POS pos; BOOL fNavCellInitialized = FALSE;
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    ACDsCylinderIntersectSphere(placeCenter, flRadius, tACD.id, listTargets, 7);
    float flWinningDistSqr = FLT_MAX; ACDID idACDWinner = ACDID_INVALID;
    BOOL bOnlyNormalRarities = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_LURE_ONLY_NORMAL_RARITIES);
    ActorCommonData * ptACDWinner;


    NavCellHandle hNavCellStart = { SSCENEID_INVALID, 0 };

    pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget &&
            ptACDTarget->snoActor != SNO_NONE &&
            !ptACDTarget->AttributesGetInt(ATTRIBUTE_IGNORE_LURE))
        {
            if (bOnlyNormalRarities && ptACDTarget->eMonsterRarity != 0)
            {
                continue;
            }

            if (ptACDTarget->snoActor == snoActor0 || ptACDTarget->snoActor == snoActor1 || ptACDTarget->snoActor == snoActor2 || ptACDTarget->snoActor == snoActor3)
            {
                ActorID idActorTarget = (ActorID)ACDGetOwner(idACDTarget);
                if (AIStateHasExistingMerge(idActorTarget, ACDID_INVALID))
                {
                    continue;
                }

                BOOL bLOS = TRUE;
                if (bPerformLOSCheck)
                {
                    if (!fNavCellInitialized)
                    {
                        NavCellHandleFromWorldPlace(placeCenter, hNavCellStart); fNavCellInitialized = TRUE;
                    }

                    bLOS = NavMeshCheckLOSToACD(hNavCellStart, placeCenter, idACDTarget, 4, FALSE);
                }

                if (bLOS)
                {
                    WorldPlace placeTarget = ptACDTarget->GetPlace();
                    float flDistSqr = Vector2D::DistanceSquared((const Vector2D &)placeCenter.wp, (const Vector2D &)placeTarget.wp);
                    if (flDistSqr < flWinningDistSqr)
                    {
                        flWinningDistSqr = flDistSqr;
                        idACDWinner = idACDTarget;
                    }
                }
            }
        }
    }

    ptACDWinner = ACDTryToGet(idACDWinner);
    if (ptACDWinner)
    {
        BOOL bRequireNearbyAlly = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_LURE_REQUIRE_NEARBY_ALLY);
        BOOL bRequireNearbyEnemy = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_LURE_REQUIRE_NEARBY_ENEMY);
        if (!sOkayToLure(tACD, *ptACDWinner, bRequireNearbyAlly, bRequireNearbyEnemy))
        {
            idACDWinner = ACDID_INVALID;
        }
    }

    return idACDWinner;
}






void LureUpdate(ActorCommonData & tACD, AILureData & tLureData)


{
    ScopedActorDefinitionLockR pdefActor(tACD.snoActor);

    ActorID idActor = (ActorID)ACDGetOwner(tACD.id);

    XPooledDataIDList listLinkedACDs(MemoryPoolsGetDATAIDListPool());
    int32 nNumLinks = MarkerGetACDsLinkedByActor(idActor, listLinkedACDs, NULL);
    if (listLinkedACDs.Count() < nNumLinks)
    {
        return;
    }


    if (tACD.AttributesGetInt(ATTRIBUTE_DEACTIVATE_LURE) > 0)
    {
        return;
    }

    int32 nDesiredMaxToLure = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_LURE_MAX_SIMULTANEOUS);
    SNO snoPower = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_LURE_POWER);
    int32 nMaxToLure = Math::Min<int32>(nDesiredMaxToLure, LURE_MAX_AFFECTED);
    for (int32 i = 0; i < nMaxToLure; i++)
    {
        ACDID idACDTarget = ACDID_INVALID;
        ACDID idACDCastLocation = ACDID_INVALID;
        LuredActor & tLuredActor = tLureData.arLured[i];

        if (tLuredActor.idACD != ACDID_INVALID)
        {
            BOOL bRequireNearbyAlly = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_LURE_REQUIRE_NEARBY_ALLY);
            BOOL bRequireNearbyEnemy = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_LURE_REQUIRE_NEARBY_ENEMY);
            ActorCommonData * ptACDExistingTarget = ACDTryToGet(tLuredActor.idACD);
            if (ptACDExistingTarget &&
                ptACDExistingTarget->IsAlive() &&
                sOkayToLure(tACD, *ptACDExistingTarget, bRequireNearbyAlly, bRequireNearbyEnemy) &&
                !ptACDExistingTarget->AttributesGetInt(ATTRIBUTE_IGNORE_LURE))
            {

                idACDTarget = ptACDExistingTarget->id;
                idACDCastLocation = tLuredActor.idACDLocation;
            }
            else
            {


                tLuredActor.idACD = ACDID_INVALID;
                tLuredActor.idACDLocation = ACDID_INVALID;
                if (tLuredActor.nLocationIdx >= 0)
                {

                    if (tLureData.arLocationUsedCount[tLuredActor.nLocationIdx] > 0)
                    {
                        tLureData.arLocationUsedCount[tLuredActor.nLocationIdx]--;
                    }

                    tLuredActor.nLocationIdx = -1;
                }
            }
        }

        if (idACDTarget == ACDID_INVALID)
        {
            BOOL bRequireLOS = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_LURE_REQUIRE_LOS);
            BOOL bUseCastLocations = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_LURE_USE_CAST_LOCATIONS);
            if (bUseCastLocations)
            {
                if (listLinkedACDs.Count() > 0)
                {
                    int32 nWinningCount = INT_MAX;
                    int32 nWinningIdx = -1;
                    ACDID idACDWinningCastLocation = ACDID_INVALID;
                    ACDID idACDWinningTarget = ACDID_INVALID;
                    int32 nIdx = 0;

                    POS pos = listLinkedACDs.GetHeadPosition();
                    while (pos && nIdx < LURE_MAX_LOCATIONS)
                    {
                        ACDID idACDLoc = (ACDID)listLinkedACDs.GetNext(pos);
                        ActorCommonData * ptACDLoc = ACDTryToGet(idACDLoc);
                        if (ptACDLoc)
                        {
                            ACDID idACDTargetForThisLocation = sGetNearestLureTarget(tACD, ptACDLoc->GetPlace(), bRequireLOS);
                            if (idACDTargetForThisLocation != ACDID_INVALID &&
                                tLureData.arLocationUsedCount[nIdx] < nWinningCount)
                            {
                                nWinningCount = tLureData.arLocationUsedCount[nIdx];
                                nWinningIdx = nIdx;
                                idACDWinningCastLocation = idACDLoc;
                                idACDWinningTarget = idACDTargetForThisLocation;
                            }
                        }

                        nIdx++;
                    }

                    if (nWinningIdx >= 0)
                    {




                        tLureData.arLocationUsedCount[nWinningIdx]++;
                        tLuredActor.idACDLocation = idACDWinningCastLocation; tLuredActor.nLocationIdx = nWinningIdx;
                        idACDTarget = idACDWinningTarget;
                        idACDCastLocation = idACDWinningCastLocation;
                    }
                }
            }
            else
            {




                idACDTarget = sGetNearestLureTarget(tACD, tACD.GetPlace(), bRequireLOS);
            }

            if (idACDTarget != ACDID_INVALID)
            {

                tLuredActor.idACD = idACDTarget;
                tLuredActor.idACDLocation = ACDID_INVALID; tLuredActor.nLocationIdx = -1;
            }
        }

        if (idACDTarget != ACDID_INVALID)
        {

            ActorID idActorTarget = (ActorID)ACDGetOwner(idACDTarget);

            AIStateMergeBroadcastPower(idActorTarget, snoPower, LURE_BROADCAST_DURATION_TICKS, tACD.id, idACDCastLocation, LURE_POWER_PRIORITY);
        }
    }
}
