// Original file: X/Server/Ai/SiegebreakerDemonBrain.cpp
#include "../Actors/Actors.h"
#include "../Actors/ActorPowers.h"
#include "./ActorAI.h"
#include "./BrainComponents.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../Powers/Powers.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/Player.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../../Common/Target.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Random.h"

WorldDistance BrainAPIGetDistanceToTarget(const ActorID idActor, const ActorID idTarget);  // BrainComponents.cpp:5347
void BrainAPIChooseSkill(ActorID idActor, BrainAttackSettings & tAttackSettings);  // BrainComponents.cpp:7823
Player * PlayerFindClosest(const WorldPlace & tPlace, const WorldDistance wdRadius);  // Player.cpp:3052
ACDID ACDFindClosestOfType(const SNO snoActor, const WorldDistance & wdRadius, const WorldPlace & tPlaceCenter);  // ActorCommonData.cpp:4739

enum SiegebreakerDemonSkills {
    LOOK_AROUND = 0,
    BITE = 1,
    STOMP = 2,
    GRABTOBITE = 3,
    MINI_CHARGE = 4,
    CHARGE = 5,
    GRAB = 6,
    POUND = 7,
    SIEGEBREAKERDEMON_NUM_SKILLS = 8,
};

struct ChosenSkill {  // size 0x14
    int32 nSkillIndex;  // 0x00
    SNO snoPower;  // 0x04
    ActorID idActorTarget;  // 0x08
    BOOL bInRange;  // 0x0c
    WorldDistance wdDistToTarget;  // 0x10
};

struct SiegebreakerDemonBrainData {  // size 0x24
    BOOL bPlayedIntro;  // 0x00
    BOOL bPlayedSecondRoar;  // 0x04
    Vector3D wvLastChargePosition;  // 0x08
    uint32 dwLastAttack;  // 0x14
    uint32 dwNumPounds;  // 0x18
    uint32 dwState;  // 0x1c
    ACDID idACDBiteTarget;  // 0x20
};

//@ 64
static BOOL sGetValidPower(const Actor * ptActor, const int32 nSkillIndex, ChosenSkill & tChosenSkill)



{


    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    SNO snoPower = tACD.AttributesGetPower(nSkillIndex);

    if (snoPower == SNO_NONE)
    {
        return FALSE;
    }

    if (!PowerIsBasicAttack(AutoPowerDefinitionR(snoPower)))
    {
        return FALSE;
    }


    ACDID idACD = ptActor->idACD;
    ActorID idActor = ptActor->id;
    ActorID idActorTarget = BrainAPIActorChooseTarget(idActor, snoPower, FALSE, NULL);

    if (idActorTarget == ACTORID_INVALID)
    {
        return FALSE;
    }

    tChosenSkill.wdDistToTarget = BrainAPIGetDistanceToTarget(idActor, idActorTarget);


    WorldDistance wdAttackRadius = PowerGetRange(&tACD, AutoPowerDefinitionR(snoPower), TRUE);

    tChosenSkill.bInRange = tChosenSkill.wdDistToTarget < wdAttackRadius;









    tChosenSkill.idActorTarget = idActorTarget;
    tChosenSkill.nSkillIndex = nSkillIndex;
    tChosenSkill.snoPower = snoPower;
    return TRUE;
}

// sChooseSkill and SiegebreakerDemonBrainUpdate (line 566, below) exist in the DWARF but the linker
// removed SiegebreakerDemonBrainUpdate, so neither has code in the binary. Only the declarations the
// DWARF records are written here: the skill table, its loop, and the locals at lines 134-139. The
// skill selection itself (lines 150-288: CHANCE_TO_GRAB, CHANCE_TO_CHARGE, CHANCE_TO_WALK_CLOSE,
// CHANCE_TO_CHARGE_AT_LOW_HEALTH, CHANCE_TO_STOMP and their rolls) is NOT recoverable and is left out.
// The second sGetValidPower loop is what keeps sGetValidPower out of line in the original.
//@ 118
static ChosenSkill sChooseSkill(const Actor * ptActor)






{
    ChosenSkill tAvailableSkills[SIEGEBREAKERDEMON_NUM_SKILLS];


    for (int32 i = 0; i < SIEGEBREAKERDEMON_NUM_SKILLS; i++)
    {
        tAvailableSkills[i].snoPower = SNO_NONE;
        sGetValidPower(ptActor, i, tAvailableSkills[i]);
    }

    RNG & tRNG = SGameGlobalsGet().rng;

    ActorAI & tAI = ActorGetAI(ptActor->id);
    SiegebreakerDemonBrainData * ptBrainData = (SiegebreakerDemonBrainData *)tAI.pBrainData;
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    float flHealthPercent = tACD.GetHitpointPercent();
    (void)tRNG; (void)ptBrainData; (void)flHealthPercent;

    return tAvailableSkills[MINI_CHARGE];
}

//@ 289
static ChosenSkill sChooseSkillWWI(const Actor * ptActor)
{

    ChosenSkill tAvailableSkills[SIEGEBREAKERDEMON_NUM_SKILLS];

    for (int32 i = 0; i < SIEGEBREAKERDEMON_NUM_SKILLS; i++)
    {
        tAvailableSkills[i].snoPower = SNO_NONE;
        sGetValidPower(ptActor, i, tAvailableSkills[i]);
    }

    RNG & tRNG = SGameGlobalsGet().rng;

    ActorAI & tAI = ActorGetAI(ptActor->id);
    SiegebreakerDemonBrainData * ptBrainData = (SiegebreakerDemonBrainData *)tAI.pBrainData;
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    tACD.GetHitpointPercent();



    const float flClosestTargetSearchRadius = 60.0f;


    if (tACD.HasAttachedACD())
    {
        if (ptBrainData->dwState == 3)
        {
            tAvailableSkills[POUND].idActorTarget = (ActorID)ACDGetOwner(tACD.GetAttachedACD());
            return tAvailableSkills[POUND];
        }
        else
        {

            tAvailableSkills[LOOK_AROUND].idActorTarget = (ActorID)ACDGetOwner(ptBrainData->idACDBiteTarget);
            return tAvailableSkills[BITE];
        }
    }

//@ 326
    switch (ptBrainData->dwState)
    {
    case 0:
        return tAvailableSkills[MINI_CHARGE];
    case 1:
        return tAvailableSkills[LOOK_AROUND];
//@ 340
    case 2:
        return tAvailableSkills[STOMP];
//@ 346
    case 3:
        {
            ACDID idACDFemaleWitchDoctor = ACDFindClosestOfType(SNOFileLookupGUID(ACTOR_SNO, "WitchDoctor_Female"), flClosestTargetSearchRadius, tACD.GetPlace());
            if (idACDFemaleWitchDoctor != ACDID_INVALID)
            {
                tAvailableSkills[GRAB].idActorTarget = (ActorID)ACDGetOwner(idACDFemaleWitchDoctor);
                return tAvailableSkills[GRAB];
            }
        }
        break;
//@ 360
    case 4:
        {
            if (ptBrainData->dwLastAttack == CHARGE)
            {
                return tAvailableSkills[LOOK_AROUND];
            }

            float flChargeRoll = RandRangeFloat_Internal(tRNG, 0.0f, 1.0f);
            if (flChargeRoll < 0.5f)
            {
                return tAvailableSkills[CHARGE];
            }
        }
        break;
//@ 394
    case 5:
        {
            float flLookAround = RandRangeFloat_Internal(tRNG, 0.0f, 1.0f);
            if (flLookAround < 0.25f) return tAvailableSkills[LOOK_AROUND];
        }
        break;
//@ 407
    case 6:
        {
            ACDID idACDMaleBarbarian = ACDFindClosestOfType(SNOFileLookupGUID(ACTOR_SNO, "Barbarian_Male"), flClosestTargetSearchRadius, tACD.GetPlace());
            if (idACDMaleBarbarian != ACDID_INVALID)
            {

                tAvailableSkills[GRABTOBITE].idActorTarget = (ActorID)ACDGetOwner(idACDMaleBarbarian);
                ptBrainData->idACDBiteTarget = idACDMaleBarbarian;
                return tAvailableSkills[GRABTOBITE];
            }
        }
        break;
    case 7:
        return tAvailableSkills[MINI_CHARGE];
    }

    return tAvailableSkills[MINI_CHARGE];
}

//@ 436
void SiegebreakerDemonBrainInitialize(const ActorID idActor)
{

    ActorAI & tAI = ActorGetAI(idActor);


    SiegebreakerDemonBrainData * ptBrainData = (SiegebreakerDemonBrainData *)SigmaMemoryNew(sizeof(SiegebreakerDemonBrainData), 4, SGameManager::GameGet().ptMemoryInterface, TRUE);

    ptBrainData->wvLastChargePosition = cgwvNull;
    ptBrainData->bPlayedIntro = FALSE;
    ptBrainData->bPlayedSecondRoar = FALSE;
    ptBrainData->dwLastAttack = (uint32)-1;
    ptBrainData->dwNumPounds = 0;
    ptBrainData->dwState = 0;
    ptBrainData->idACDBiteTarget = ACDID_INVALID;



    tAI.pBrainData = ptBrainData;
}

//@ 459
void SiegebreakerDemonBrainDispose(const ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    SigmaMemoryFree(tAI.pBrainData, SGameManager::GameGet().ptMemoryInterface); tAI.pBrainData = NULL;
}

//@ 469
void SiegebreakerDemonBrainUpdateWWI(ActorID idActor)
{

//@ 473
    if (!BrainConditionIsReady(idActor))
    {
        return;
    }

//@ 479
    if (ActorPowersGetPrimary(idActor) != POWER_NONE)
    {
        return;
    }

//@ 484
    BrainAttackSettings tAttackSettings;
    BrainAPIGetAttackSettings(idActor, tAttackSettings);
    BrainAPIChooseSkill(idActor, tAttackSettings);

    Actor * ptActor = ActorGet(idActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    ActorAI & tAI = ActorGetAI(idActor);
    SiegebreakerDemonBrainData * ptBrainData = (SiegebreakerDemonBrainData *)tAI.pBrainData;
    ptBrainData->wvLastChargePosition = cgwvNull;
    ActorFlagSet(idActor, ACTOR_NO_COLLISION_BIT, FALSE);

    int32 nTargets = BrainAPIGetNumEnemyTargets(idActor);
    if (nTargets == 0 && !tACD.HasAttachedACD())
    {
        BrainAPIDoDefaultWander(idActor);
        return;
    }

//@ 504
    if (!ptBrainData->bPlayedIntro)
    {
        const WorldDistance INTRO_DISTANCE = 200.0f;
        Player * ptPlayer = PlayerFindClosest(tACD.GetPlace(), INTRO_DISTANCE);


        if (ptPlayer)
        {
            ptBrainData->bPlayedIntro = TRUE;
            tAttackSettings.snoPower = SNOFileLookupGUID(POWER_SNO, "SiegebreakerDemon_Charge");
            tAttackSettings.tTarget.InitializeFromACD(ptPlayer->idPlayerACD, FALSE, FALSE);
            BrainAPIDoAttack(idActor, tAttackSettings);
            return;
        }



//@ 521
        BrainAPIDoIdle(idActor);
        return;
    }

//@ 526
    if (!ptBrainData->bPlayedSecondRoar)
    {
        BrainAPIDoPlayAnimation(idActor, TAG_ACTOR_ANIMATION_TALK2, (uint32)-1, FALSE, FALSE, 0.0f, FALSE, 1.0f);
        ptBrainData->bPlayedSecondRoar = TRUE;
        return;
    }



//@ 535
    ChosenSkill tChosenSkill = sChooseSkillWWI(ptActor);
    tAttackSettings.snoPower = tChosenSkill.snoPower;
    if (tChosenSkill.idActorTarget != ACTORID_INVALID && ActorTryToGet(tChosenSkill.idActorTarget))
    {
        tAttackSettings.tTarget.InitializeFromACD(ACDFindByActor(tChosenSkill.idActorTarget), FALSE, FALSE);
    }


//@ 543
    if (tAttackSettings.snoPower == SNO_NONE)
    {
        BrainAPIDoDefaultWander(idActor);
        return;
    }


    ptBrainData->dwLastAttack = tChosenSkill.nSkillIndex;



    if (tChosenSkill.nSkillIndex == POUND || tChosenSkill.nSkillIndex == BITE)
    {
        tAttackSettings.dwAttackFlags |= (1 << ATTACK_DONT_CLOSE) | (1 << ATTACK_DONT_FACE);
    }
    BrainAPIDoAttack(idActor, tAttackSettings);



}

// Removed by the linker (DW_AT_low_pc 0); see the note above sChooseSkill. Locals and their lines are
// from the DWARF; the statements between them mirror SiegebreakerDemonBrainUpdateWWI and are a
// reconstruction, not evidence.
//@ 566
void SiegebreakerDemonBrainUpdate(
    ActorID idActor)
{
    if (!BrainConditionIsReady(idActor))
    {
        return;
    }

    if (ActorPowersGetPrimary(idActor) != POWER_NONE)
    {
        return;
    }

//@ 580
    BrainAttackSettings tAttackSettings;
    BrainAPIGetAttackSettings(idActor, tAttackSettings);
    Actor * ptActor = ActorGet(idActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    ActorAI & tAI = ActorGetAI(idActor);
    SiegebreakerDemonBrainData * ptBrainData = (SiegebreakerDemonBrainData *)tAI.pBrainData;
    ptBrainData->wvLastChargePosition = cgwvNull;


    int32 nTargets = BrainAPIGetNumEnemyTargets(idActor);
    if (nTargets == 0 && !tACD.HasAttachedACD())
    {
        BrainAPIDoDefaultWander(idActor);
        return;
    }

//@ 632
    ChosenSkill tChosenSkill = sChooseSkill(ptActor);
    tAttackSettings.snoPower = tChosenSkill.snoPower;
    if (tChosenSkill.idActorTarget != ACTORID_INVALID && ActorTryToGet(tChosenSkill.idActorTarget))
    {
        tAttackSettings.tTarget.InitializeFromACD(ACDFindByActor(tChosenSkill.idActorTarget), FALSE, FALSE);
    }

    if (tAttackSettings.snoPower == SNO_NONE)
    {
        BrainAPIDoDefaultWander(idActor);
        return;
    }

    ptBrainData->dwLastAttack = tChosenSkill.nSkillIndex;
    BrainAPIDoAttack(idActor, tAttackSettings);
}

//@ 692
void SiegebreakerDemonBrainSetLastChargePosition(ActorID idActor, Vector3D wvPosition)
{

    ActorAI & tAI = ActorGetAI(idActor);
    SiegebreakerDemonBrainData * ptBrainData = (SiegebreakerDemonBrainData *)tAI.pBrainData;
    ptBrainData->wvLastChargePosition = wvPosition;

}

//@ 702
Vector3D SiegebreakerDemonBrainGetLastChargePosition(ActorID idActor)
{

    ActorAI & tAI = ActorGetAI(idActor);
    SiegebreakerDemonBrainData * ptBrainData = (SiegebreakerDemonBrainData *)tAI.pBrainData;
    return ptBrainData->wvLastChargePosition;

}
