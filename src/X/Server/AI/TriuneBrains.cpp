// Original file: X/Server/Ai/TriuneBrains.cpp
#include "../Actors/Actors.h"
#include "../Actors/ActorPowers.h"
#include "./ActorAI.h"
#include "./BrainComponents.h"
#include "./BrainStance.h"
#include "./TriuneBrains.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../Powers/Powers.h"
#include "../Powers/PowerAPI.h"
#include "../Powers/PowerHelper.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/AI/ActorAISwig.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Definitions/MonsterDefinition.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/Monsters.h"
#include "../../Common/MonsterStruct.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../../Common/Target.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/MemoryPools.h"

void BrainAPIChooseSkill(ActorID idActor, BrainAttackSettings & tAttackSettings);  // BrainComponents.cpp:7823
const MonsterAwarenessInfo * BrainAPIActorGetAwareness(ACDID idACD, WorldPlace & placeCenter);  // BrainComponents.cpp:8123
void ACDGetNavMeshHandleWithPlace(const ActorCommonData & tACD, NavMeshHandle & tNavMeshHandle, WorldPlace & tPlace);  // ActorCommonData.cpp:4668
BOOL ActorPowersPrimaryIsNotInterruptable(ActorID idActor);  // ActorPowers.cpp:253

struct TriuneVesselBrainData {  // size 0xc
    uint32 dwActivationDuration;  // 0x00
    uint32 dwTickActivated;  // 0x04
    BOOL bTransformed;  // 0x08
};

struct TriuneBerserkerBrainData {  // size 0x4
    BOOL bStuck;  // 0x00
};

//@ 42
static const float TRIUNESUMMONER_SUMMON_RADIUS = 10.0f;  // name and value from the DWARF (DW_AT_const_value 0x41200000); its only uses are dead in the original
static const Angle TRIUNESUMMONER_SUMMON_ANGLE_INC = 2.0943952f;  // DWARF DW_AT_const_value 0x40060a92, also the literal at 0x7100e3c2fc

//@ 47
void TriuneSummonerBrainInitialize(const ActorID idActor)
{

    ActorAI & tAI = ActorGetAI(idActor);
    Actor * ptActor = ActorGet(idActor);

    ActorCommonData & tACD = ACDGet(ptActor->idACD);


    TriuneSummonerBrainData * pBrainData = (TriuneSummonerBrainData *)SigmaMemoryNew(sizeof(TriuneSummonerBrainData), 4, SGameManager::GameGet().ptMemoryInterface, TRUE);
    tAI.pBrainData = pBrainData;



    for (int32 i = 0; i < TRIUNE_SUMMONER_NUM_CLONES; i++)
    {
        pBrainData->idACDClone[i] = ACDID_INVALID;
    }


    if (tACD.tSNOName.snoHandle == GlobalSNOGet(GLOBALSNO_MONSTER_TRIUNE_SUMMONER_CLONE))
    {
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_HAS_SPECIAL_DEATH_ANIMTAG), 1);
        pBrainData->idACDMaster = ACDID_INVALID;
    }
    else
    {
        pBrainData->idACDMaster = ptActor->idACD;
    }
}

//@ 80
void TriuneSummonerBrainDispose(const ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    SigmaMemoryFree(tAI.pBrainData, SGameManager::GameGet().ptMemoryInterface); tAI.pBrainData = NULL;
}

//@ 89
void TriuneSummonerBrainUpdate(ActorID idActor)
{

    Actor * ptActor = ActorGet(idActor);


    ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);
    if (ptACD == NULL)
    {
        return;
    }
}

//@ 150
BOOL sTriuneSummoner_FindSummonNavPlaces(ACDID idACD)
{

    TriuneSummonerBrainData * pBrainData = TriuneSummonerTryGetBrainData(idACD);
    if (pBrainData == NULL)
    {
        return FALSE;
    }

    ActorCommonData & tACD = ACDGet(idACD);
    ACDGetNavMeshHandleWithPlace(tACD, pBrainData->tSummonNavPlace.tNavMeshHandle, pBrainData->tSummonNavPlace.tPlace);

    WorldPlace tCheckPlace;
    Vector3D vCenterPos = tACD.GetPlace().wp;

    Angle aAngle = RandAngle(SGameGlobalsGet().rng);
    for (int32 i = 0; i < TRIUNE_SUMMONER_NUM_SUMMON_SPOTS; i++)
    {
        tCheckPlace.wp.x = vCenterPos.x + Math::Sin(aAngle) * TRIUNESUMMONER_SUMMON_RADIUS;
        tCheckPlace.wp.y = vCenterPos.y + Math::Cos(aAngle) * TRIUNESUMMONER_SUMMON_RADIUS;




        aAngle = Math::NormalizeAngle(aAngle + TRIUNESUMMONER_SUMMON_ANGLE_INC);
    }
    return TRUE;
}

//@ 181
static int32 sTriuneSummoner_CountClones(ActorID idActor)
{

    Actor * ptActor = ActorGet(idActor);
    if (ptActor == NULL)
    {
        return 0;
    }

    TriuneSummonerBrainData * pBrainData = TriuneSummonerTryGetBrainData(ptActor->idACD);
    if (pBrainData == NULL)
    {
        return 0;
    }

    int32 nCloneCount = 0;

    for (int32 i = 0; i < TRIUNE_SUMMONER_NUM_CLONES; i++)
    {
        ActorCommonData * ptACDClone = ACDTryToGet(pBrainData->idACDClone[i]);
        if (ptACDClone)
        {
            ActorID idActorClone = (ActorID)ACDGetOwner(pBrainData->idACDClone[i]);
            if (idActorClone != ACTORID_INVALID)
            {
                if (ptACDClone->IsAlive())
                {
                    nCloneCount++;
                }
            }
        }
    }

    return nCloneCount;
}

//@ 218
void TriuneSummonerBrainActionUpdate(ActorID idActor)
{

    Actor * ptActor = ActorGet(idActor);

    ActorCommonData & tACD = ACDGet(ptActor->idACD);

    TriuneSummonerBrainData * pBrainData = TriuneSummonerTryGetBrainData(ptActor->idACD);
    if (pBrainData == NULL)
    {
        return;
    }

    BrainAttackSettings tAttackSettings;
    BrainAPIGetAttackSettings(idActor, tAttackSettings);
    tAttackSettings.snoPower = SNO_NONE;
    BrainAPIChooseSkill(idActor, tAttackSettings);

    int32 nTargets = BrainAPIGetNumEnemyTargets(idActor);
    if (!(nTargets > 0))
    {
        BrainAPIDoDefaultWander(idActor);
        return;
    }

    BOOL bOverrideWithDefaultAttack = FALSE;







    SNO snoShieldPower = tACD.AttributesGetPower(TRIUNESUMMONER_SKILLINDEX_SHIELD);
    SNO snoSummonPower = tACD.AttributesGetPower(TRIUNESUMMONER_SKILLINDEX_SUMMONDEMON);
    if (PowerAPI_BuffFindOnACD(snoShieldPower, tACD, ACDID_INVALID) != BUFFID_INVALID && snoSummonPower != SNO_NONE)
    {
        tAttackSettings.snoPower = snoSummonPower;
    }

    if (tAttackSettings.snoPower == tACD.AttributesGetPower(TRIUNESUMMONER_SKILLINDEX_CREATECLONES))
    {

        const float flDamageThreshold = 0.75f;
        if (tACD.GetHitpointPercent() > flDamageThreshold)
        {
            bOverrideWithDefaultAttack = TRUE;
        }

        else if (sTriuneSummoner_CountClones(idActor) > 0)
        {
            bOverrideWithDefaultAttack = TRUE;
        }



        else if (!sTriuneSummoner_FindSummonNavPlaces(ptActor->idACD))
        {
            bOverrideWithDefaultAttack = TRUE;
        }
    }
    else if (tAttackSettings.snoPower == snoSummonPower || tAttackSettings.snoPower == snoShieldPower)
    {
        WorldPlace placeCenter;
        const int32 nMaxSummonedDemons = 3;
        const MonsterAwarenessInfo * ptAwareness = BrainAPIActorGetAwareness(tACD.id, placeCenter);




        int32 nExistingUnits = ACDCountNearbySummonedUnits(tACD, ptAwareness->flCombatRadius);
        if (nExistingUnits >= nMaxSummonedDemons)
        {
            bOverrideWithDefaultAttack = TRUE;
        }
        else
        {

            if (PowerAPI_BuffFindOnACD(snoShieldPower, tACD, ACDID_INVALID) == BUFFID_INVALID)
            {
                tAttackSettings.snoPower = snoShieldPower;
            }
        }
    }

    if (bOverrideWithDefaultAttack)
    {
        SNO snoProjectileAttack = tACD.AttributesGetPower(TRIUNESUMMONER_SKILLINDEX_PROJECTILE);
        ActorID idActorTarget = BrainAPIActorChooseTarget(idActor, snoProjectileAttack, FALSE, NULL);
        if (idActorTarget != ACTORID_INVALID)
        {
            tAttackSettings.snoPower = snoProjectileAttack;
            tAttackSettings.tTarget.InitializeFromACD(ACDFindByActor(idActorTarget), 0, 0);
        }
        else
        {
            tAttackSettings.snoPower = SNO_NONE;
        }
    }

    if (tAttackSettings.snoPower != SNO_NONE)
    {
        BrainAPIDoAttack(idActor, tAttackSettings);
    }
    else
    {
        BrainAPIDoDefaultWander(idActor);
    }
}

//@ 324
TriuneSummonerBrainData * TriuneSummonerTryGetBrainData(ACDID idACD)
{

    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        ActorID idActor = (ActorID)ptACD->idOwner;
        ActorAI & tAI = ActorGetAI(idActor);
        if (tAI.nBrainType == ACTORBRAIN_TRIUNE_SUMMONER)
        {
            return (TriuneSummonerBrainData *)tAI.pBrainData;
        }
    }
    return NULL;
}

//@ 342
void TriuneSummoner_KillAllClones(ActorID idActor)
{

    Actor * ptActor = ActorGet(idActor);
    if (ptActor == NULL)
    {
        return;
    }

    TriuneSummonerBrainData * pBrainData = TriuneSummonerTryGetBrainData(ptActor->idACD);
    if (pBrainData == NULL)
    {
        return;
    }

    for (int32 i = 0; i < TRIUNE_SUMMONER_NUM_CLONES; i++)
    {
        if (pBrainData->idACDClone[i] != ACDID_INVALID)
        {
            ActorID idActorClone = (ActorID)ACDGetOwner(pBrainData->idACDClone[i]);
            if (idActorClone != ACTORID_INVALID)
            {
                BrainAPIDoDeath(idActorClone, idActor, TRUE, TRUE, TRUE, NULL, NULL);
            }
            pBrainData->idACDClone[i] = ACDID_INVALID;
        }
    }
}

//@ 373
BOOL TriuneSummonerBrainOnDeath(const ActorID idActor, const ActorID idKillerActor)
{
    TriuneSummoner_KillAllClones(idActor);
    return FALSE;
}

//@ 394
void TriuneVesselBrainInitialize(const ActorID idActor)
{

    ActorAI & tAI = ActorGetAI(idActor);
    TriuneVesselBrainData * pBrainData = (TriuneVesselBrainData *)SigmaMemoryNew(sizeof(TriuneVesselBrainData), 4, SGameManager::GameGet().ptMemoryInterface, TRUE);
    tAI.pBrainData = pBrainData;


    pBrainData->dwActivationDuration = 60 * (2 + RandRange_Internal(SGameGlobalsGet().rng, 0, 13));
    pBrainData->dwTickActivated = (uint32)-1;
    pBrainData->bTransformed = FALSE;
}

//@ 409
void TriuneVesselBrainDispose(const ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    SigmaMemoryFree(tAI.pBrainData, SGameManager::GameGet().ptMemoryInterface); tAI.pBrainData = NULL;
}

//@ 418
void TriuneVesselBrainActionUpdate(ActorID idActor)
{

    ACDID idACD = ACDFindByActor(idActor);

    ActorCommonData & tACD = ACDGet(idACD);
    ActorAI & tAI = ActorGetAI(idActor);

    TriuneVesselBrainData * pBrainData = (TriuneVesselBrainData *)tAI.pBrainData;

    if (!tACD.IsAlive())
    {
        return;
    }

    if (pBrainData->dwTickActivated == (uint32)-1)
    {

        BrainAPIUpdateTargets(idActor, FALSE);
        int32 nTargets = BrainAPIGetNumEnemyTargets(idActor);
        if (nTargets > 0)
        {

            pBrainData->dwTickActivated = GameGetServerTime();
        }



        AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_NORMAL_ATTACK_01);
        BrainAPIDoIdle(idActor);
        return;
    }

    if (pBrainData->dwTickActivated + pBrainData->dwActivationDuration < GameGetServerTime())
    {







        ScopedMonsterDefinitionLockR pdefMonster(tACD.GetMonsterSNO());
        SNO snoPower = SNO_NONE;
        if (pdefMonster)
        {
            snoPower = pdefMonster->tSkills[0].snoPower;
        }

        if (snoPower == SNO_NONE)
        {
            snoPower = GlobalSNOGet(GLOBALSNO_POWERS_TRANSFORM_TO_ACTIVATED_TRIUNE);
        }

        Target tTarget;
        tTarget.InitializeFromACD(idACD, 0, 0);

        if (PowerCanUseNow(tACD, AutoPowerDefinitionR(snoPower), &tTarget, NULL))
        {



            PowerID idPower = PowerStart(idACD, snoPower, tTarget);
            if (idPower != POWERID_INVALID)
            {
                pBrainData->bTransformed = TRUE;


                XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
                PowerAPIChooseTargets tTargets;
                PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);
                PowerAPI_DoChooseTargets(tTargets, listTargets);
                POS pos = listTargets.GetHeadPosition();
                while (pos)
                {
                    ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
                    ActorID idActorTarget = (ActorID)ACDGetOwner(idACDTarget);
                    if (idActorTarget != ACTORID_INVALID)
                    {
                        ActorAI * ptAITarget = ActorTryToGetAI(idActorTarget);
                        if (ptAITarget && ptAITarget->nBrainType == ACTORBRAIN_TRIUNEVESSEL)
                        {
                            TriuneVesselBrainData * pTargetBrainData;
                            ActorCommonData & tACDTarget = ACDGet(idACDTarget);
                            pTargetBrainData = (TriuneVesselBrainData *)ptAITarget->pBrainData;

                            if (!pTargetBrainData->bTransformed &&
                                pTargetBrainData->dwTickActivated == (uint32)-1 &&
                                tACDTarget.IsAlive())
                            {
                                pTargetBrainData->dwTickActivated = GameGetServerTime();
                                AnimPlayerSetIdleAnimation(tACDTarget, TAG_ACTOR_ANIMATION_NORMAL_ATTACK_01);
                                BrainAPIDoIdle(idActorTarget);
                            }
                        }
                    }
                }
            }
        }
    }
}

//@ 526
void TriuneBerserkerBrainInitialize(ActorID idActor)
{

    ActorAI & tAI = ActorGetAI(idActor);
    TriuneBerserkerBrainData * pBrainData = (TriuneBerserkerBrainData *)SigmaMemoryNew(sizeof(TriuneBerserkerBrainData), 4, SGameManager::GameGet().ptMemoryInterface, TRUE);
    tAI.pBrainData = pBrainData;

    pBrainData->bStuck = FALSE;
}

//@ 539
void TriuneBerserkerBrainDispose(ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    SigmaMemoryFree(tAI.pBrainData, SGameManager::GameGet().ptMemoryInterface); tAI.pBrainData = NULL;
}

//@ 548
void TriuneBerserkerBrainMissed(ActorID idActor)
{

    ActorAI & tAI = ActorGetAI(idActor);

    TriuneBerserkerBrainData * pBrainData = (TriuneBerserkerBrainData *)tAI.pBrainData;


    if (ActorPowersPrimaryIsNotInterruptable(idActor))
    {
        return;
    }


    pBrainData->bStuck = TRUE;

    AnimationTiming tAnimationTiming;
    ActorCommonData & tACD = ACDGet(ACDFindByActor(idActor));

    AnimPlayerGetAnimationTiming(tACD, TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_02, (uint32)-1, 1.0f, tAnimationTiming);
    uint32 dwTimeStuck = tAnimationTiming.dwCycleTime * RandRange_Internal(SGameGlobalsGet().rng, 1, 1);

    BrainAPIDoPlayAnimation(idActor, TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_02, dwTimeStuck, FALSE, FALSE, 0.0f, FALSE, 1.0f);
    AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_02);
}

//@ 577
void TriuneBerserkerBrainActionUpdate(ActorID idActor)
{

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    ActorAI & tAI = ActorGetAI(idActor);

    TriuneBerserkerBrainData * pBrainData = (TriuneBerserkerBrainData *)tAI.pBrainData;


    if (pBrainData->bStuck)
    {
        pBrainData->bStuck = FALSE;
        BrainAPIDoPlayAnimation(idActor, TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_03, (uint32)-1, FALSE, FALSE, 0.0f, FALSE, 1.0f);
        AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_NEUTRAL);
        return;
    }

    BrainAttackSettings tAttackSettings;
    BrainAPIGetAttackSettings(idActor, tAttackSettings);
    tAttackSettings.snoPower = SNO_NONE;
    BrainAPIChooseSkill(idActor, tAttackSettings);


    int32 nTargets = BrainAPIGetNumEnemyTargets(idActor);
    if (nTargets <= 0 || tAttackSettings.snoPower == SNO_NONE)
    {
        BrainAPIDoDefaultWander(idActor);
    }
    else
    {
        BrainAPIDoAttack(idActor, tAttackSettings);
    }
}
