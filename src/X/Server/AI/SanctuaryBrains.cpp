// Original file: X/Server/Ai/SanctuaryBrains.cpp
#include "../Actors/Actors.h"
#include "../Actors/ActorPowers.h"
#include "./ActorAI.h"
#include "./BrainComponents.h"
#include "./BrainStance.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../Movement/SActorMovement.h"
#include "../Powers/Powers.h"
#include "../Powers/PowerAPI.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/ActorCommonCollision.h"
#include "../../Common/AI/ActorAISwig.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/Monsters.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/MonsterStruct.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Common/Target.h"
#include "../../Common/WorldCommonCollision.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Random.h"

void BrainAPIChooseSkill(ActorID idActor, BrainAttackSettings & tAttackSettings);  // BrainComponents.cpp:7823
const MonsterAwarenessInfo * BrainAPIActorGetAwareness(ACDID idACD, WorldPlace & placeCenter);  // BrainComponents.cpp:8123
void BrainAPIActorReleaseTarget(ActorID idActor);  // BrainComponents.cpp:8709
void BrainAPIReleaseAllTargets(ActorID idActor);  // BrainComponents.cpp:9705
void AIStopTetherRetreat(ActorID idActor, ActorAI & tAI);  // ActorAI.cpp:3923
void ActorAISetState(ActorID idActor, uint32 dwState);  // ActorAI.cpp:4502
uint32 ActorAIGetState(ActorID idActor);  // ActorAI.cpp:4528
BOOL WorldCollSweptCircle(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Circle & wcStart, const Vector2D & wpDest, const WorldCollParams & tWorldCollParams, uint32 uCollTestFlags, uint32 uACDCollNarrowFlags, ACDID idACDIgnore, WorldCollResult & tResult);  // WorldCommonCollision.cpp:147
void PowerApplyGenericAttributeBuffAttributes(ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, AttribGroupID idFastAttrib);  // PowersCommon.cpp:8452

struct ZombieBrainData {  // size 0xc
    BOOL fAlreadyDead;  // 0x00
    uint32 dwSpawnCrawlerTime;  // 0x04
    BOOL fAllowCrawlerSpawn;  // 0x08
};

enum ZombieBrainState {
    ZOMBIE_STATE_NONE = 0,
    ZOMBIE_STATE_PRESPAWN = 1,
    ZOMBIE_STATE_TELEPORTED = 2,
    ZOMBIE_STATE_SPAWNED = 3,
};

struct SkeletonBrainData {  // size 0xc
    BOOL fAlreadyDead;  // 0x00
    int32 nRandomDelayInTicks;  // 0x04
    BOOL fShieldBroken;  // 0x08
};

//@ 80
void SimpleBrainActionUpdate(ActorID idActor)
{

    BrainAttackSettings tAttackSettings;


    BrainAPIGetAttackSettings(idActor, tAttackSettings);

    BrainAPIChooseSkill(idActor, tAttackSettings);


    if (tAttackSettings.snoPower == SNO_NONE)
    {



        BrainAPIActorReleaseTarget(idActor);


        ActorAI & tAI = ActorGetAI(idActor);
        if (tAI.ptAIStateInstData == NULL && BrainAPIGetNumEnemyTargets(idActor) == 0)
        {
            BrainAPIDoDefaultWander(idActor);
        }
        else if (tAttackSettings.tTarget.eType != TT_NONE)
        {
            BrainAPIDoIdleAndFace(idActor, ACDGet(tAttackSettings.tTarget.idACDTarget).GetWorldPlaceCenter());
        }
        else
        {
            BrainAPIDoIdle(idActor);
        }
    }
    else
    {




        BrainAPIDoAttack(idActor, tAttackSettings);
    }
}

//@ 128
BOOL TetherBrainUpdate(ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    TetherData & tTetherData = tAI.tAITetherData;

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);

    const WorldPlace & placeMe = tACD.GetPlace();
    const WorldPlace & placeTether = tTetherData.placeTetherPoint;


    if (placeMe.idSWorld != placeTether.idSWorld)
    {
        return FALSE;
    }

    float flDistanceSq = Vector3D::DistanceSquared(placeMe.wp, placeTether.wp);
    float RETURN_RADIUS = Math::Min(tTetherData.flRadius, 8.0f);


    if (tTetherData.bRetreating)
    {
        BOOL bKeepGoing = flDistanceSq > RETURN_RADIUS * RETURN_RADIUS;
        if (tTetherData.eTetherType == TETHER_WHEN_IDLE)
        {
            bKeepGoing = bKeepGoing && tAI.ptAIStateInstData->eCurrState == 0;
        }

        if (bKeepGoing)
        {
            BrainStanceDoTranslate(idActor, placeTether, 0.0f, 0.0f, 4, TAG_ACTOR_ANIMATION_RUN, 5.0f, "Tether", ACDID_INVALID, SNO_NONE);
        }
        else
        {
            AIStopTetherRetreat(idActor, tAI);
            BrainAPIReleaseAllTargets(idActor);
        }
    }
    else
    {
        BOOL bStartRetreating = flDistanceSq > tTetherData.flRadius * tTetherData.flRadius;
        if (tTetherData.eTetherType == TETHER_WHEN_IDLE)
        {
            bStartRetreating = bStartRetreating && tAI.ptAIStateInstData->eCurrState == 0;
        }

        if (bStartRetreating)
        {
            tTetherData.bRetreating = TRUE;
            BrainAPIDoIdle(idActor);
            SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_AI_EVADE_BUFF);
            PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
            tBuffStartData.snoPower = snoPower;
            tBuffStartData.eStackType = (BuffStackType)0;
            tBuffStartData.bInfinite = true;
            tBuffStartData.bUndispellable = true;
            tBuffStartData.bPositiveEffect = true;
            tBuffStartData.nBuffIndex = 0;
            PowerApplyGenericAttributeBuffAttributes(tACD, AutoPowerDefinitionR(snoPower), tBuffStartData.idFastAttrib);
            PowerAPI_BuffAddToACD(tBuffStartData, tACD);
            BrainStanceDoTranslate(idActor, placeTether, 0.0f, 0.0f, 4, TAG_ACTOR_ANIMATION_RUN, RETURN_RADIUS, "Tether", ACDID_INVALID, SNO_NONE);
            tACD.tCollSettings.uDynamicFlagsNoOverlap = 0;
            tACD.ModifiedMark(ACDMODIFIED_COLL_FLAGS);
        }
    }
    return tTetherData.bRetreating;
}

// ZOMBIEBRAIN_SPAWN_DISTANCE_MIN/_DELTA (lines 245-246) and ZOMBIEBRAIN_SPAWN_DELAY_MIN/_DELTA
// (lines 248-249) are named in the DWARF but have no surviving use; their values are not observable.
//@ 245





static const float ZOMBIEBRAIN_AUTO_SPAWN_CHANCE = 1.0f;

//@ 260
void ZombieBrainInitialize(const ActorID idActor)
{

    ActorAI & tAI = ActorGetAI(idActor);
    ACDID idACD = ACDFindByActor(idActor);

    if (ActorAIGetState(idActor) == ZOMBIE_STATE_PRESPAWN ||
        RandFloat_Internal(SGameGlobalsGet().rng) > ZOMBIEBRAIN_AUTO_SPAWN_CHANCE)
    {
        ActorAISetState(idActor, ZOMBIE_STATE_PRESPAWN);


        ActorCommonData & tACD = ACDGet(idACD);
        tACD.Hide(TRUE);
        tACD.SetTargetable(FALSE);
    }
    else
    {
        ActorAISetState(idActor, ZOMBIE_STATE_SPAWNED);
    }



    tAI.pBrainData = SigmaMemoryNew(sizeof(ZombieBrainData), 4, SGameManager::GameGet().ptMemoryInterface, TRUE);
    ((ZombieBrainData *)tAI.pBrainData)->fAlreadyDead = FALSE;
    ((ZombieBrainData *)tAI.pBrainData)->fAllowCrawlerSpawn = TRUE;
}

//@ 288
void ZombieBrainDispose(const ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    SigmaMemoryFree(tAI.pBrainData, SGameManager::GameGet().ptMemoryInterface); tAI.pBrainData = NULL;
}

//@ 335
void ZombieBrainActionUpdate(ActorID idActor)
{

    SimpleBrainActionUpdate(idActor);
}

//@ 343
BOOL ZombieCrawlerGetSpawnNavPlace(ACDID idACDSpawner, NavPlace & tResultingNavPlace)


{

    const Actor * ptSpawnerActor = ActorGet((ActorID)ACDGetOwner(idACDSpawner));
    ScopedActorDefinitionLockR ptSpawnerActorDef(ptSpawnerActor->snoActor);
    ActorCommonData & tACDSpawner = ACDGet(idACDSpawner);
    const WorldLocation locSpawner = tACDSpawner.GetWorldLocation();
    WorldPlace tStartPlace = tACDSpawner.GetPlace();
    tResultingNavPlace.tPlace = tStartPlace;
    tResultingNavPlace.tPlace.wp = locSpawner.transform * ptSpawnerActorDef->wpLocationPowerSrc;


    SNOName tSNOName(ACTOR_SNO, GlobalSNOGet(GLOBALSNO_MONSTER_ZOMBIE_CRAWLER_A));
    if (!MonsterExists(tSNOName))
    {
        return FALSE;
    }


    SActorMovement * ptMove = ptSpawnerActor->ptSActorMovement;
    if (ptMove == NULL)
    {
        return FALSE;
    }



    float flRadius = tACDSpawner.GetRadius();

    ACDCollSettings tACDCollSettings;
    ptMove->GetCollSettings(tACDCollSettings);
    tACDCollSettings.uDynamicFlagsNoOverlap &= ~0x10f;





    WorldCollParams tCollParams(tACDCollSettings);
    Circle circleStart;
    WorldCollResult tResult;
    circleStart.Set(tStartPlace.wp.x, tStartPlace.wp.y, flRadius);

    WorldCollSweptCircle(tStartPlace.idSWorld,
        ptMove->GetNavCell(), circleStart, (const Vector2D &)tResultingNavPlace.tPlace.wp, tCollParams, 0, 0, idACDSpawner, tResult);








    tResultingNavPlace.tPlace.wp = tResult.wpEnd;
    ScenesCommonNavMeshNodeFromPlace(tResultingNavPlace.tPlace, &tResultingNavPlace.tNavMeshHandle, FALSE, FALSE);

    ptMove->SnapPlaceToGround(tResultingNavPlace.tPlace);

    return TRUE;
}

//@ 404
void ZombieBrainDeadUpdate(ActorID idActor)
{

    ActorAI & tAI = ActorGetAI(idActor);



    ZombieBrainData * pBrainData = (ZombieBrainData *)tAI.pBrainData;



    if (!pBrainData->fAlreadyDead && tAI.dwTimeOfDeath && pBrainData->dwSpawnCrawlerTime)
    {
        uint32 dwNow = GameGetServerTime();
        if (pBrainData->dwSpawnCrawlerTime <= dwNow)
        {

            pBrainData->fAlreadyDead = TRUE;
            SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_SUMMON_ZOMBIE_CRAWLER);
            Target tTarget;
            ACDID idACD = ACDFindByActor(idActor);
            tTarget.InitializeFromACD(idACD, FALSE, FALSE);
            PowerStart(idACD, snoPower, tTarget);
        }
        else
        {


            ActorAI & tAILoc = ActorGetAI(idActor);
            tAILoc.dwNextBrainUpdateTime = Math::Min(pBrainData->dwSpawnCrawlerTime, tAILoc.dwNextBrainUpdateTime);
        }
    }
}

//@ 439
BOOL ZombieBrainOnDeath(const ActorID idActor, const ActorID idKillerActor)


{

    ActorAI & tAI = ActorGetAI(idActor);


    ZombieBrainData * pBrainData = (ZombieBrainData *)tAI.pBrainData;
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);




    pBrainData->dwSpawnCrawlerTime = 0;
    if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_DEATH_SPECIAL) && pBrainData->fAllowCrawlerSpawn)
    {
        const float ZOMBIE_CHANCE_TO_SPAWN_CRAWLER = 0.5f;
        float flRandom = RandFloat_Internal(SGameGlobalsGet().rng);
        NavPlace tNavPlace;

        if (flRandom < ZOMBIE_CHANCE_TO_SPAWN_CRAWLER &&
            ZombieCrawlerGetSpawnNavPlace(idACD, tNavPlace) &&
            !tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_BUFF_VISUAL_EFFECT, 27)))
        {
            tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_HAS_SPECIAL_DEATH_ANIMTAG), 1);

            AnimationTiming tAnimationTiming;
            AnimPlayerGetAnimationTiming(tACD, TAG_ACTOR_ANIMATION_DEATH_SPECIAL, (uint32)-1, 1.0f, tAnimationTiming);
            pBrainData->dwSpawnCrawlerTime = tAnimationTiming.dwExecuteTime[0] + GameGetServerTime();

            tAI.dwNextBrainUpdateTime = Math::Min(pBrainData->dwSpawnCrawlerTime, tAI.dwNextBrainUpdateTime);
        }
    }

    return FALSE;
}

//@ 480
void ZombieBrainDisableZombieCrawlerSpawnOnDeath(const ActorID idActor)
{

    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI == NULL)
    {
        return;
    }

    if (ptAI->nBrainType != ACTORBRAIN_ZOMBIE)
    {
        return;
    }


    ZombieBrainData * pBrainData = (ZombieBrainData *)ptAI->pBrainData;
    pBrainData->fAllowCrawlerSpawn = FALSE;
}

//@ 509
void SkeletonBrainInitialize(const ActorID idActor)
{

    ActorAI & tAI = ActorGetAI(idActor);


    tAI.pBrainData = SigmaMemoryNew(sizeof(SkeletonBrainData), 4, SGameManager::GameGet().ptMemoryInterface, TRUE);
    ((SkeletonBrainData *)tAI.pBrainData)->fAlreadyDead = FALSE;

    ((SkeletonBrainData *)tAI.pBrainData)->nRandomDelayInTicks = (int32)(RandRangeFloat_Internal(SGameGlobalsGet().rng, 1.0f, 3.0f) * 60.0f);
    ((SkeletonBrainData *)tAI.pBrainData)->fShieldBroken = FALSE;

    if (tAI.nBrainType == ACTORBRAIN_SKELETON_SHIELDBEARER)
    {
        ACDID idACD = ACDFindByActor(idActor);
        ActorCommonData & tACD = ACDGet(idACD);
        tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_BREAKABLE_SHIELD_HP), PowerGetFormulaValue(&tACD, AutoPowerDefinitionR(GlobalSNOGet(GLOBALSNO_POWERS_SHIELD_SKELETON_SHIELD)), TAG_POWER_SHIELD_DAMAGE_ABSORB_AMOUNT, -1, -1, 0));
    }
}

//@ 529
void SkeletonBrainDispose(const ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    SigmaMemoryFree(tAI.pBrainData, SGameManager::GameGet().ptMemoryInterface); tAI.pBrainData = NULL;
}

//@ 539
void SkeletonBrainDeadUpdate(ActorID idActor)
{

    ActorAI & tAI = ActorGetAI(idActor);
    (void)tAI;
}

//@ 552
static void sSetShieldSkeletonAttributes(ACDID idACD)
{

    ActorCommonData & tACD = ACDGet(idACD);
    SNO snoShieldSkeletonActor = MonsterGetSummonActor(tACD.tSNOName, 0);

    if (snoShieldSkeletonActor == SNO_NONE)
    {
        return;
    }


    ScopedActorDefinitionLockR pdefActor(snoShieldSkeletonActor);
    SNO snoShieldSkeletonMonster = pdefActor->snoMonster;
    int32 nLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    float flWalkSpeed = GetMonsterLevelAttribute(nLevel, snoShieldSkeletonMonster, 54);
    float flRunSpeed = GetMonsterLevelAttribute(nLevel, snoShieldSkeletonMonster, 55);
    float flSprintSpeed = GetMonsterLevelAttribute(nLevel, snoShieldSkeletonMonster, 43);
    float flStrafeSpeed = GetMonsterLevelAttribute(nLevel, snoShieldSkeletonMonster, 129);
    tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_WALKING_RATE), flWalkSpeed);
    tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_RUNNING_RATE), flRunSpeed);
    tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_SPRINTING_RATE), flSprintSpeed);
    tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_STRAFING_RATE), flStrafeSpeed);


    tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_CURRENT_WEAPONCLASS), 9);
}

//@ 580
static void sSetSkeletonAttributes(ActorCommonData & tACD)
{

    SNO snoBaseSkeletonActor = MonsterGetSummonActor(tACD.tSNOName, 1);

    if (snoBaseSkeletonActor == SNO_NONE)
    {
        return;
    }


    ScopedActorDefinitionLockR pdefActor(snoBaseSkeletonActor);
    SNO snoBaseSkeletonMonster = pdefActor->snoMonster;
    int32 nLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    float flWalkSpeed = GetMonsterLevelAttribute(nLevel, snoBaseSkeletonMonster, 54);
    float flRunSpeed = GetMonsterLevelAttribute(nLevel, snoBaseSkeletonMonster, 55);
    float flSprintSpeed = GetMonsterLevelAttribute(nLevel, snoBaseSkeletonMonster, 43);
    float flStrafeSpeed = GetMonsterLevelAttribute(nLevel, snoBaseSkeletonMonster, 129);
    tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_WALKING_RATE), flWalkSpeed);
    tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_RUNNING_RATE), flRunSpeed);
    tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_SPRINTING_RATE), flSprintSpeed);
    tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_STRAFING_RATE), flStrafeSpeed);


    tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_CURRENT_WEAPONCLASS), -1);
}

//@ 607
void SkeletonShieldBearerLowerShield(ACDID idACD)
{



    SNO snoShieldPower = GlobalSNOGet(GLOBALSNO_POWERS_SHIELD_SKELETON_SHIELD);
    ActorCommonData & tACD = ACDGet(idACD);
    BuffID idShieldBuff = PowerAPI_BuffFindOnACD(snoShieldPower, tACD, ACDID_INVALID);
    if (idShieldBuff != BUFFID_INVALID)
    {
        sSetSkeletonAttributes(tACD);
        PowerAPI_BuffFlagForDeletion(idShieldBuff, FALSE, TRUE, (BuffRemovalReason)0, TRUE);
    }
}

//@ 623
void SkeletonShieldBearerBrainUpdate(ActorID idActor)
{

    Actor * ptActor = ActorGet(idActor);


    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    ActorAI & tAI = ActorGetAI(idActor);
    SkeletonBrainData * ptBrainData = (SkeletonBrainData *)tAI.pBrainData;


    if (!ptBrainData->fShieldBroken && tACD.IsAlive())
    {
        float flShieldHealth = tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_BREAKABLE_SHIELD_HP));



        int nValue = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_DAMAGE_STATE_CURRENT));
        int nMax = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_DAMAGE_STATE_MAX));
        if (flShieldHealth < 0.000001f || (nValue > 0 && nMax > 0))
        {

            ptBrainData->fShieldBroken = TRUE;
            tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_BREAKABLE_SHIELD_HP), 0.0f);


            sSetSkeletonAttributes(tACD);
        }
        else
        {

            SNO snoShieldPower = GlobalSNOGet(GLOBALSNO_POWERS_SHIELD_SKELETON_SHIELD);
            BuffID idShieldBuff = PowerAPI_BuffFindOnACD(snoShieldPower, tACD, ACDID_INVALID);

            uint32 dwNow = GameGetServerTime();
            int32 nNumTargets = BrainAPIGetNumEnemyTargets(idActor);
            int32 dwAttackShieldCooldown = 30;




            if (idShieldBuff == BUFFID_INVALID && nNumTargets > 0 &&

                dwNow > tAI.dwLastAttackTime + dwAttackShieldCooldown)
            {
                sSetShieldSkeletonAttributes(idACD);

                Target tTarget;
                tTarget.InitializeFromACD(idACD, FALSE, FALSE);
                PowerStart(idACD, snoShieldPower, tTarget);
                return;
            }
        }
    }

    SimpleBrainActionUpdate(idActor);
}

//@ 685
void SkeletonSummonerBrainInitialize(ActorID idActor)
{




    ActorAI & tAI = ActorGetAI(idActor);
    tAI.pbNewBrainDataBuf[0] = TRUE;
}

//@ 695
void SkeletonSummonerBrainUpdate(ActorID idActor)
{

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);

    ActorAI & tAI = ActorGetAI(idActor);
    if (tAI.pbNewBrainDataBuf[0])
    {
        if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMON_COUNT)) == 0)
        {
            SNO snoSummonOnSpawnPower = GlobalSNOGet(GLOBALSNO_POWERS_SKELETON_SUMMON_ONSPAWN);
            Target tTarget;
            tTarget.InitializeFromACD(idACD, FALSE, FALSE);
            PowerStart(idACD, snoSummonOnSpawnPower, tTarget);
        }

        tAI.pbNewBrainDataBuf[0] = FALSE;
    }

    BrainAttackSettings tAttackSettings;
    BrainAPIGetAttackSettings(idActor, tAttackSettings);
    BrainAPIChooseSkill(idActor, tAttackSettings);

    int32 nTargets = BrainAPIGetNumEnemyTargets(idActor);

    if (nTargets == 0)
    {
        BrainAPIDoDefaultWander(idActor);
        return;
    }

    SNO snoProjectilePower = tACD.AttributesGetPower(1);
    SNO snoSummonPower = tACD.AttributesGetPower(2);


    if (tAttackSettings.snoPower == snoSummonPower)
    {


        int32 nCount = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMON_COUNT));
        ScopedPowerDefinitionLockR pdefSummonPower(snoSummonPower);
        if (nCount < (int32)PowerGetFormulaValue(&tACD, pdefSummonPower, TAG_POWER_SKELETON_MAX_TOTAL_SUMMON_COUNT, -1, -1, 0))
        {
            WorldPlace placeCenter;
            const MonsterAwarenessInfo * ptAwareness = BrainAPIActorGetAwareness(tACD.id, placeCenter);



            int32 nMaxSummonedSkeletons = (int32)PowerGetFormulaValue(&tACD, pdefSummonPower, TAG_POWER_SKELETON_MAX_SUMMON_COUNT, -1, -1, 0);

            int32 nExistingUnits = ACDCountNearbySummonedUnits(tACD, ptAwareness->flCombatRadius);
            if (nExistingUnits >= nMaxSummonedSkeletons)
            {
                tAttackSettings.snoPower = snoProjectilePower;
                ActorID idTarget = BrainAPIActorChooseTarget(idActor, SNO_NONE, FALSE, NULL);
                Actor * ptActor = ActorTryToGet(idTarget);
                if (ptActor)
                {
                    Target tTarget;
                    tTarget.InitializeFromACD(ptActor->idACD, FALSE, FALSE);
                    tAttackSettings.tTarget = tTarget;
                }
                else
                {
                    tAttackSettings.snoPower = SNO_NONE;
                }
            }
        }
        else
        {
            tAttackSettings.snoPower = SNO_NONE;
        }
    }


    if (tAttackSettings.snoPower == SNO_NONE)
    {
        if (tAttackSettings.tTarget.eType != TT_NONE)
        {
            BrainAPIDoIdleAndFace(idActor, ACDGet(tAttackSettings.tTarget.idACDTarget).GetWorldPlaceCenter());
        }
        else
        {
            BrainAPIDoIdle(idActor);
        }
    }
    else
    {

        BrainAPIDoAttack(idActor, tAttackSettings);
    }
}

//@ 793
void FallenShamanBrainInitialize(ActorID idActor)
{




    ActorAI & tAI = ActorGetAI(idActor);
    tAI.pbNewBrainDataBuf[0] = TRUE;
}

//@ 803
void FallenShamanBrainUpdate(ActorID idActor)
{

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    ActorAI & tAI = ActorGetAI(idActor);
    if (tAI.pbNewBrainDataBuf[0])
    {
        if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMON_COUNT)) == 0)
        {
            SNO snoSummonOnSpawnPower = GlobalSNOGet(GLOBALSNO_POWERS_FALLEN_SUMMON_ONSPAWN);
            Target tTarget;
            tTarget.InitializeFromACD(idACD, FALSE, FALSE);
            PowerStart(idACD, snoSummonOnSpawnPower, tTarget);
        }

        tAI.pbNewBrainDataBuf[0] = FALSE;
    }

    BrainAttackSettings tAttackSettings;
    BrainAPIGetAttackSettings(idActor, tAttackSettings);
    BrainAPIChooseSkill(idActor, tAttackSettings);

    int32 nTargets = BrainAPIGetNumEnemyTargets(idActor);

    if (nTargets == 0)
    {
        BrainAPIDoDefaultWander(idActor);
        return;
    }

    SNO snoProjectilePower = tACD.AttributesGetPower(1);
    SNO snoSummonPower = tACD.AttributesGetPower(2);

    if (!SNOFileExists(POWER_SNO, snoProjectilePower))
    {
        return;
    }



    if (!SNOFileExists(POWER_SNO, snoSummonPower))
    {
        return;
    }



    if (tAttackSettings.snoPower == snoSummonPower)
    {



        WorldPlace placeCenter;
        const MonsterAwarenessInfo * ptAwareness = BrainAPIActorGetAwareness(tACD.id, placeCenter);
        ScopedPowerDefinitionLockR pdefSummonPower(snoSummonPower);



        int32 nMaxSummonedFallen = (int32)PowerGetFormulaValue(&tACD, pdefSummonPower, TAG_POWER_FALLEN_MAX_SUMMON_COUNT, -1, -1, 0);

        int32 nExistingUnits = ACDCountNearbySummonedUnits(tACD, ptAwareness->flCombatRadius);
        if (nExistingUnits >= nMaxSummonedFallen)
        {
            tAttackSettings.snoPower = snoProjectilePower;
            ActorID idTarget = BrainAPIActorChooseTarget(idActor, SNO_NONE, FALSE, NULL);
            Actor * ptActor = ActorTryToGet(idTarget);
            if (ptActor)
            {
                Target tTarget;
                tTarget.InitializeFromACD(ptActor->idACD, FALSE, FALSE);
                tAttackSettings.tTarget = tTarget;
            }
            else
            {
                tAttackSettings.snoPower = SNO_NONE;
            }
        }
    }


    if (tAttackSettings.snoPower == SNO_NONE)
    {
        if (tAttackSettings.tTarget.eType != TT_NONE)
        {
            BrainAPIDoIdleAndFace(idActor, ACDGet(tAttackSettings.tTarget.idACDTarget).GetWorldPlaceCenter());
        }
        else
        {
            BrainAPIDoIdle(idActor);
        }
    }
    else
    {

        BrainAPIDoAttack(idActor, tAttackSettings);
    }
}
