// Original file: X/Server/Ai/AnimalBrains.cpp
#include "../Actors/Actors.h"
#include "./ActorAI.h"
#include "./AnimalBrains.h"
#include "./BrainComponents.h"
#include "./BrainStance.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../Powers/PowerAPI.h"
#include "../Powers/PowerHelper.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/AI/ActorAISwig.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Monsters.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/Player.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../../Common/Target.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Sigma/Random.h"

void ACDGetNavMeshHandleWithPlace(const ActorCommonData & tACD, NavMeshHandle & tNavMeshHandle, WorldPlace & tPlace);  // ActorCommonData.cpp:4668
void PowerAPI_DoFindAllPossibleTargets(const PowerAPIChooseTargets & tTargetingInfo, XPooledDataIDList & listTargets);  // PowerAPI.cpp:6247

//@ 35
void PigCagedBrainInitialize(const ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    PigCagedBrainData * pBrainData = (PigCagedBrainData *)SigmaMemoryNew(sizeof(PigCagedBrainData), 4, SGameManager::GameGet().ptMemoryInterface, TRUE);
    tAI.pBrainData = pBrainData;


//@ 43
    pBrainData->idACDSniffed = ACDID_INVALID;
    pBrainData->dwACDMemoryTime = 0;
}

//@ 49
void PigCagedBrainDispose(const ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    SigmaMemoryFree(tAI.pBrainData, SGameManager::GameGet().ptMemoryInterface); tAI.pBrainData = NULL;
}

//@ 58
static BOOL sPigCagedOkayToTargetACD(ACDID idACD, ACDID idACDTarget)
{


    if (idACDTarget == ACDID_INVALID || idACD == ACDID_INVALID)
    {
        return FALSE;
    }

//@ 67
    PigCagedBrainData * pBrainData = PigCagedTryGetBrainData(idACD);
    if (pBrainData)
    {
        if (GameGetServerTime() < pBrainData->dwACDMemoryTime && idACDTarget ==
            pBrainData->idACDSniffed)
        {
            return FALSE;
        }
    }
    return TRUE;
}

//@ 82
static void sPigCagedRememberActor(ACDID idACD, ActorID idActorTarget)
{


    PigCagedBrainData * pBrainData = PigCagedTryGetBrainData(idACD);
    if (pBrainData)
    {
        pBrainData->idACDSniffed = ACDFindByActor(idActorTarget);
        pBrainData->dwACDMemoryTime = RandRange_Internal(SGameGlobalsGet().rng, PIG_CAGED_MEMORY_TICKS_MIN, PIG_CAGED_MEMORY_TICKS_DELTA) +



            GameGetServerTime();
    }
}

//@ 100
static ActorID sPigCagedFindTarget(ActorID idActor, float flRadius)
{


    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL)
    {
        return ACTORID_INVALID;
    }
    ActorCommonData & tACDActor = ACDGet(ptActor->idACD);

    NavPlace tNavPlace;
    ACDGetNavMeshHandleWithPlace(ACDGet(ptActor->idACD), tNavPlace.tNavMeshHandle, tNavPlace.tPlace);
    PowerAPIChooseTargets tTargetingInfo;
    tTargetingInfo.idPower = POWER_NONE;
    tTargetingInfo.dwFlags = 0;
    Target tTarget;
    tTarget.eType = TT_POINT;
    tTarget.tPlace = tNavPlace.tPlace;
    tTarget.tPlace.wp.x += Math::Cos(ptActor->aYaw);
    tTarget.tPlace.wp.y += Math::Sin(ptActor->aYaw);
    PowerHelper_ChooseTargetsInitGeometryPie(tNavPlace.tPlace, tTarget, flRadius, tACDActor.GetHeight(), PIG_CAGED_VIEW_DEGREES, tTargetingInfo.tGeometry);

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoFindAllPossibleTargets(tTargetingInfo, listTargets);

    float flWinningDistSq = 1e10f;
    ACDID idWinningACD = ACDID_INVALID;
    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData & tACDTarget = ACDGet(idACDTarget);
        if (idACDTarget != ptActor->idACD &&
            sPigCagedOkayToTargetACD(ptActor->idACD, idACDTarget) &&
            !tACDTarget.IsWreckableObject() &&
            (ACDIsPlayer(idACDTarget) || tACDTarget.IsMonster()))
        {
            WorldPlace wpTarget = tACDTarget.GetPlace();
            if (wpTarget.idSWorld == tNavPlace.tPlace.idSWorld)
            {
                float flDistSq = Vector3D::DistanceSquared(wpTarget.wp, tNavPlace.tPlace.wp);
                if (flDistSq < flWinningDistSq)
                {
                    flWinningDistSq = flDistSq;
                    idWinningACD = idACDTarget;
                }
            }
        }
    }

//@ 151
    if (idWinningACD == ACDID_INVALID)
    {
        return ACTORID_INVALID;
    }
    return (ActorID)ACDGetOwner(idWinningACD);
}

//@ 161
void PigCagedBrainActionUpdate(ActorID idActor)
{

    Actor * ptActor = ActorGet(idActor);
    ActorCommonData & tACD = ACDGet(ptActor->idACD);

    BrainAttackSettings tAttackSettings;
    BrainAPIGetAttackSettings(idActor, tAttackSettings);
    tAttackSettings.snoPower = SNO_NONE;


    SNO snoSniffPower = tACD.AttributesGetPower(PIG_CAGED_SKILLINDEX_SNIFF);
    if (snoSniffPower != SNO_NONE)
    {
        MonsterSkillDeclaration tMonsterSkillDec = MonsterGetMonsterSkill(tACD.tSNOName, PIG_CAGED_SKILLINDEX_SNIFF);
        ActorID idActorTarget = sPigCagedFindTarget(idActor, tMonsterSkillDec.flUseRangeMax);
        if (idActorTarget != ACTORID_INVALID)
        {
            Actor * ptActorTarget = ActorTryToGet(idActorTarget);
            if (ptActorTarget)
            {

                tAttackSettings.snoPower = snoSniffPower;
                tAttackSettings.tTarget.eType = TT_ACTOR;
                tAttackSettings.tTarget.idACDTarget = ptActorTarget->idACD;
                sPigCagedRememberActor(ptActor->idACD, idActorTarget);
                BrainStanceSetIdleAnimation(idActor, TAG_ACTOR_ANIMATION_NEUTRAL);
            }
        }
    }

//@ 192
    if (tAttackSettings.snoPower == SNO_NONE)
    {
        SNO snoClosePower = tACD.AttributesGetPower(PIG_CAGED_SKILLINDEX_CLOSE);
        if (snoClosePower != SNO_NONE)
        {
            MonsterSkillDeclaration tMonsterSkillDec = MonsterGetMonsterSkill(tACD.tSNOName, PIG_CAGED_SKILLINDEX_CLOSE);
            ActorID idActorTarget = sPigCagedFindTarget(idActor, tMonsterSkillDec.flUseRangeMax);
            if (idActorTarget != ACTORID_INVALID)
            {
                Actor * ptActorTarget = ActorTryToGet(idActorTarget);
                if (ptActorTarget)
                {

                    tAttackSettings.snoPower = snoClosePower;
                    tAttackSettings.tTarget.eType = TT_ACTOR;
                    tAttackSettings.tTarget.idACDTarget = ptActorTarget->idACD;
                }
            }
        }
    }

//@ 212
    if (tAttackSettings.snoPower != SNO_NONE)
    {
        BrainAPIDoAttack(idActor, tAttackSettings);
    }
    else
    {
        BrainAPIDoDefaultWander(idActor);
    }
}

//@ 223
PigCagedBrainData * PigCagedTryGetBrainData(ACDID idACD)
{

    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        ActorID idActor = (ActorID)ptACD->idOwner;
        ActorAI & tAI = ActorGetAI(idActor);
        if (tAI.nBrainType == ACTORBRAIN_PIG_CAGED)
        {
            return (PigCagedBrainData *)tAI.pBrainData;
        }
    }
    return NULL;
}
