#include "ActorAI.h"
#include "BrainTranslate.h"
#include "BrainStance.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../Actors/Actors.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Player.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/Random.h"
#include "BrainComponents.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../Powers/PowerAPI.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Common/Achievements/AchievementDefines.h"
#include "../Actors/ActorPowers.h"
#include "../../Common/WorldPlace.h"
#include "../Movement/SActorMovement.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Target.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../Actors/ActorEffect.h"
#include "../Powers/Powers.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/TagMaps/PowerTags.h"

#include "../Achievements/AchievementModifierEvaluationLogic.h"
namespace Achievement {
typedef int32 EventID;













}  // namespace Achievement

struct BrainStanceData {  // size 0x44
    BrainComponentTranslate tTranslate;  // 0x00
    EStanceType eStanceType;  // 0x34
    uint32 dwTimer;  // 0x38
    SNO snoPower;  // 0x3c
    SNO snoLastSkillStancePower;  // 0x40
};

struct BrainStanceGlobals {  // size 0x30
    XChainedFixedBlockAllocator<BrainStanceData> poolBrainStances;  // 0x00
};

void ActorPowersInterruptPrimary(ActorID idActor, BOOL bIgnoreContactFrame);  // X/Server/Actors/ActorPowers.cpp
uint32 AnimPlayerGetWalkCoolDownTag(ActorCommonData & tACD, uint32 dwAnimTag);  // X/Common/AnimPlayer.cpp
BOOL PowerActorCanPlayAnimation(ActorID idActor, BOOL bAllowPlayingAnimInterrupt);  // X/Server/Powers/Powers.cpp
void BrainComponentDisposeForActor(ActorID idActor);  // X/Server/AI/BrainComponents.cpp
void AnimPlayerPlayIdle(ActorCommonData & tACD);  // X/Common/AnimPlayer.cpp
BOOL PowerActorCanDoAnything(ActorID idActor);  // X/Server/Powers/Powers.cpp
BOOL PowerActorCanGracefullyStopSkill(ActorID idActor);  // X/Server/Powers/Powers.cpp
void ActorPowersDetachPrimary(ActorID idActor);  // X/Server/Actors/ActorPowers.cpp
void BrainComponentInitializeForActor(ActorID idActor);  // X/Server/AI/BrainComponents.cpp
BOOL PowerActorCanDoGetHit(ActorID idActor);  // X/Server/Powers/Powers.cpp
BOOL PowerContactTagGetBool(PowerID idPower, uint32 dwTag);  // X/Server/Powers/Powers.cpp:4208
void PlayerBrainTryQueuedPower(ACDID idACDPlayer);  // X/Server/AI/PlayerBrain.cpp:407
static void sBrainStanceFrozen(ActorID idActor);
static void sBrainStanceAttached(ActorID idActor);
static void sBrainStanceFeared(ActorID idActor);
void ActorAIRunUpdateNow(Actor & tActor);  // ActorAI.cpp:1658
ActorBrain ActorGetBrainType(const ActorID idActor);  // X/Server/AI/ActorAI.cpp:1355
void HirelingBrainClearAttack(ActorID idHireling);  // X/Server/AI/HirelingBrain.cpp:1770
BOOL PowerUpdateTarget(PowerID idPower, const UpdateTargetData * ptUpdateTargetData);  // X/Server/Powers/Powers.cpp:999
void TraceStat(LPCSTR sEventType, ACDID idACD, LPCSTR cpFmtStr, ...);  // X/Server/SGame.cpp:7255
void PowerAPI_BuffRemoveOnDeath(ActorCommonData & tACD, ACDID idACDKiller);  // X/Server/Powers/PowerAPI.cpp:10905
ActorAI & ActorGetAI(Actor & tActor);  // X/Server/AI/ActorAI.cpp:1324
void BrainUpdate(Actor & tActor);  // X/Server/AI/ActorAI.cpp:1486
void BrainComponentUpdate(Actor & tActor);  // X/Server/AI/BrainComponents.cpp:3248
void AIBroadcastUpdate(Actor & tActor);  // X/Server/AI/AIBroadcast.cpp:97
void BrainStanceDoDead(ActorID idActor);
BOOL BrainComponentChargeDoPower(ActorID idActor);  // X/Server/AI/BrainComponents.cpp:2558
BOOL PowerNeedsUpcallFromWalking(PowerID idPower);  // X/Server/Powers/Powers.cpp:4279
BOOL PowerAtDestination(const PowerID idPower, const ACDID idACDProjectile, const ACDID idACDIntersected, const SpellEffect eSpellEffect);  // X/Server/Powers/Powers.cpp:3754
void BrainStanceDoIdle(ActorID idActor);
BOOL PowerIgnoresRangeOnShiftClick(AutoPowerDefinitionR pdefPower);  // X/Common/Powers/PowersCommon.cpp:1047
BOOL PowerClipsTargetToAttackRadius(AutoPowerDefinitionR pdefPower);  // X/Common/Powers/PowersCommon.cpp:1061
BOOL PowerChecksPathAndCliptoPathableAttackRadius(SNO snoPower);  // X/Common/Powers/PowersCommon.cpp:1077
BOOL PowerActorCanWalk(ActorID idActor);  // X/Server/ActorIsAffected.cpp:112
BOOL IsTriggerEventTargetingActor(ActorID idActor);  // X/Server/LuaScript/LuaScript.cpp:642
BOOL PowerActorCanBeDead(ActorID idActor);  // X/Server/ActorIsAffected.cpp:414
BOOL PowerActorCanBeKnockedBack(ActorID idActor, PowerID idPower);  // X/Server/ActorIsAffected.cpp:430
void PowerSetCoolDown(ActorCommonData & tACD, const uint32 dwCooldown);  // X/Server/Powers/Powers.cpp:1715
void AnimPlayerUpdateAnimEndTime(ActorCommonData & tACD, uint32 dwNewEndTime);  // X/Common/AnimPlayer.cpp:2034
void BrainTriggerUpdate(ActorID idActor);
BOOL BrainStanceNeedToPlayWalkCoolDown(ActorID idActor);
static void sBrainStanceStunned(ActorID idActor);
static void sBrainStanceForceGripped(ActorID idActor);
static void sBrainStancePlayAnimation(ActorID idActor, uint32 dwAnimTag, EStanceType eStanceType, uint32 dwDuration, float flUnitStartTime, const BOOL bDontSendToClient, const float flSpeedScalar);
static void sBrainStanceFigureCurrentStance(ActorID idActor);

// BrainStance.cpp:109
void BrainStanceInitForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    BrainStanceGlobals *& ptBrainStanceGlobals = tSGame.ptBrainStanceGlobals;
    ptBrainStanceGlobals = (BrainStanceGlobals *)SigmaMemoryNew(sizeof(BrainStanceGlobals), 4, tSGame.ptMemoryInterface, TRUE);
    ptBrainStanceGlobals->poolBrainStances.Initialize();
    ptBrainStanceGlobals->poolBrainStances.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptBrainStanceGlobals->poolBrainStances.SetBlockCount_Tracked(150);
}

// BrainStance.cpp:124
void BrainStanceDisposeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    BrainStanceGlobals *& ptBrainStanceGlobals = tSGame.ptBrainStanceGlobals;
    if (ptBrainStanceGlobals)
    {
        ptBrainStanceGlobals->poolBrainStances.Dispose();
        SigmaMemoryFree(ptBrainStanceGlobals, tSGame.ptMemoryInterface);
        ptBrainStanceGlobals = NULL;
    }
}

// BrainStance.cpp:140
static BrainStanceData * sGetBrainStanceData(ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    return tAI.ptBrainStanceData;
}

// BrainStance.cpp:150
static void sBrainStanceSetTimer(ActorID idActor, uint32 dwLength)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    if (dwLength == (uint32)-1)
        ptStance->dwTimer = (uint32)-1;
    else
        ptStance->dwTimer = GameGetServerTime() + dwLength;
}

// BrainStance.cpp:169
static void sBrainStanceTypeSet(ActorID idActor, EStanceType eStanceType, SNO snoPower)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    ptStance->eStanceType = eStanceType;
    sBrainStanceSetTimer(idActor, (uint32)-1);
    if (snoPower != -1)
        ptStance->snoLastSkillStancePower = snoPower;
}

// BrainStance.cpp:189
static void sBrainStanceTranslateInitialize(ActorID idActor)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    BrainComponentTranslate & tTranslate = ptStance->tTranslate;
    ACDID idACD = ACDFindByActor(idActor);
    tTranslate.placeGotoPoint = cgplaceNull;
    tTranslate.wdSpeed = 0.0f;
    tTranslate.nTranslateType = TRANSLATE_TYPE_WALKING;
    tTranslate.dwAnimTag = (uint32)-1;
    tTranslate.wdMaxRangeFromDest = 0.0f;
    tTranslate.dwTimeStartedWalking = 0;
    tTranslate.dwDoneWaitingTime = 0;
    tTranslate.dwDoneSteppingAwayTime = 0;
    tTranslate.idACD = idACD;
    tTranslate.idACDTarget = ACDID_INVALID;
}

// BrainStance.cpp:212
BOOL BrainTimerTestAndUpdate(uint32 dwTimeout, uint32 dwTimeoutDelta, uint32 & dwNextTime)
{
    uint32 dwNow = GameGetServerTime();
    if (dwNow < dwNextTime)
        return FALSE;
    uint32 dwTimer = RandRange_Internal(SGameGlobalsGet().rng, dwTimeout, dwTimeoutDelta);
    dwNextTime = dwNow + dwTimer;
    return TRUE;
}

// BrainStance.cpp:234
static void sBrainStancePrepAnimation(ActorID idActor, EStanceType eStanceType)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    BrainTranslateCancel(ptStance->tTranslate, TRUE);
    sBrainStanceSetTimer(idActor, (uint32)-1);
    sBrainStanceTypeSet(idActor, STANCE_CHANGING, -1);
    ActorPowersInterruptPrimary(idActor, TRUE);
    sBrainStanceTypeSet(idActor, eStanceType, -1);
}

// BrainStance.cpp:254
static void sBrainStancePlayAnimation(ActorID idActor, uint32 dwAnimTag, EStanceType eStanceType, uint32 dwDuration, float flUnitStartTime, const BOOL bDontSendToClient, const float flSpeedScalar)
{
    sBrainStancePrepAnimation(idActor, eStanceType);
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    AnimReason eAnimReason = ANIM_REASON_STANCE;
    if (eStanceType == STANCE_PLAY_INTERRUPTABLE_ANIMATION)
        eAnimReason = ANIM_REASON_INTERRUPTABLE_ANIMATION;
    uint32 dwPlayLength = AnimPlayerPlayAnimation(tACD, dwAnimTag, dwDuration, TRUE, eAnimReason, flUnitStartTime, bDontSendToClient, flSpeedScalar);
    if (dwPlayLength != (uint32)-2)
        sBrainStanceSetTimer(idActor, dwPlayLength);
}

// BrainStance.cpp:938
// NON_MATCHING: on DWARF lines 938-1039 with the DWARF locals (x13); RETARGET folding differs (see notes)
#line 938
EBrainPowerResult BrainStanceFailureReason(
    ActorID idActor,
    SNO snoPower,
    const Target & tTarget,
    BOOL bHoldPosition)
{
    // DWARF: nResult is the constant BRAIN_POWER_FAIL until the chain's results merge (no final else).
    EBrainPowerResult nResult = BRAIN_POWER_FAIL;

    ACDID idACD = ActorGet(idActor)->idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    uint32 dwFlags = PowerFailureReasons(tACD, AutoPowerDefinitionR(snoPower), &tTarget, FALSE, FALSE);

    // Bit 0 is ignored (952 col 34 is the '&').
    uint32 dwFlagsToCheck = dwFlags & ~1u;

    if (!dwFlagsToCheck)
    {
        nResult = BRAIN_POWER_START;
    }
    else if (bHoldPosition && dwFlagsToCheck == 0x4 &&

            (PowerIgnoresRangeOnShiftClick(AutoPowerDefinitionR(snoPower)) ||
            PowerClipsTargetToAttackRadius(AutoPowerDefinitionR(snoPower)) ||
            PowerChecksPathAndCliptoPathableAttackRadius(snoPower)))
    {
        nResult = BRAIN_POWER_START;
    }
    else if (dwFlags & 0x800)
    {
        nResult = BRAIN_POWER_NO_LINE_OF_SIGHT;
    }
    else if (dwFlags & 0x800000)
    {
        nResult = BRAIN_POWER_FAIL;
    }
    else if (dwFlags & 0x4)
    {
        nResult = BRAIN_POWER_GET_CLOSER;
    }
    else if (dwFlags & 0x2)
    {
        nResult = BRAIN_POWER_CANT_INTERRUPT;
    }
    else if (dwFlags & 0x18)
    {
        nResult = BRAIN_POWER_QUEUE_AND_STAND;
    }
    else if (dwFlags & 0x80000)
    {
        nResult = BRAIN_POWER_QUEUE_AND_STAND;
    }










    if (SGameGlobalsGet().dwFlags & 0x20)
        nResult = BRAIN_POWER_FAIL;



    if (nResult != BRAIN_POWER_GET_CLOSER)
    {
        if (BrainStanceNeedToPlayWalkCoolDown(idActor))
        {
            nResult = BRAIN_POWER_QUEUE_AND_STAND;
        }
        else
        if (!PowerActorCanDo(idActor, snoPower))
        {
            nResult = BRAIN_POWER_CANT_INTERRUPT;
        }
        else {
            Actor * ptActor = ActorGet(idActor);
            if (ptActor->ptSActorMovement &&
                ptActor->ptSActorMovement->IsMoving() && nResult != BRAIN_POWER_NO_LINE_OF_SIGHT)
            {
                nResult = BRAIN_POWER_IN_UNCOMPLETED_MOVEMENT;
            }
        }
    }

    if (nResult == BRAIN_POWER_START)
    {
        SNO snoPrimaryPower = ActorPowersGetPrimaryPower(idActor);

        BrainStanceData * ptStance = sGetBrainStanceData(idActor);
        if (snoPrimaryPower == snoPower && ptStance->eStanceType == STANCE_SKILL)
            nResult = BRAIN_POWER_RETARGET;
    }



    return nResult;

}
#line 291

// BrainStance.cpp:1071
void BrainStanceDisposeForActor(ActorID idActor)
{
    BrainComponentDisposeForActor(idActor);
    ActorAI & tAI = ActorGetAI(idActor);
    BrainStanceGlobals * ptBrainStanceGlobals = SGameManager::GameGet().ptBrainStanceGlobals;
    ptBrainStanceGlobals->poolBrainStances.Free(tAI.ptBrainStanceData);
}

// BrainStance.cpp:574 (inlined only; param idActor and local ptStance from the DWARF)
static void sBrainStanceTriggerTranslateUpdate(ActorID idActor)
{
    sBrainStanceSetTimer(idActor, (uint32)-1);
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    if (ptStance->eStanceType == STANCE_FEARED) return;
    sBrainStanceTypeSet(idActor, STANCE_WALKING, -1);
}
// BrainStance.cpp:1199 (inlined only; param tActor and local ptStance from the DWARF)
static void sBrainStanceTimerElapsed(Actor & tActor)
{
    if (!ActorTryToGetAI(tActor.id)) return;
    BrainStanceData * ptStance = sGetBrainStanceData(tActor.id);
    switch (ptStance->eStanceType)  // jump table 0x7100e596f0: IDLE/SKILL/DEAD/WALKING/CHANGING take the default (line 1238)
    {
    case STANCE_WALK_WARM_UP:
        sBrainStanceTriggerTranslateUpdate(tActor.id);
        break;
    case STANCE_DYING:
        BrainStanceDoDead(tActor.id);
        break;
    case STANCE_PLAY_ANIMATION:
        AIQueueCheckActionCompleted(tActor.id, -1, 1);
        sBrainStanceFigureCurrentStance(tActor.id);
        break;
    case STANCE_GETHIT:
    case STANCE_KNOCK_BACK:
    case STANCE_STUNNED:
    case STANCE_WALK_COOL_DOWN:
    case STANCE_PLAY_INTERRUPTABLE_ANIMATION:
    case STANCE_FEARED:
    case STANCE_ATTACHED:
    case STANCE_FROZEN:
    case STANCE_FORCE_GRIPPED:
        sBrainStanceFigureCurrentStance(tActor.id);
        break;
    default:
        DisplayInternalError(NULL, "", 1238, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    }
}
// BrainStance.cpp:1245
void BrainRunUpdates(Actor & tActor)
{
    ActorAI & tAI = ActorGetAI(tActor);
    BrainStanceData * ptStance = sGetBrainStanceData(tActor.id);
    uint32 dwNow = GameGetServerTime();
    BOOL bTimeToUpdateBrain = FALSE;
    BOOL bTimeToUpdateBrainComponent = FALSE;
    if (tAI.dwNextBrainUpdateTime <= dwNow) bTimeToUpdateBrain = TRUE;
    if (tAI.dwNextBrainComponentUpdateTime <= dwNow) bTimeToUpdateBrainComponent = TRUE;
    if (bTimeToUpdateBrain) BrainUpdate(tActor);
    if (bTimeToUpdateBrain || bTimeToUpdateBrainComponent) BrainComponentUpdate(tActor);  // BrainStance.cpp:1279 col 25 is the '||'
    if (tAI.dwNextBrainStanceUpdateTime <= dwNow && !ActorHasBeenKilled(tActor.id))
    {
        ActorCommonData & tACD = ACDGet(tActor.idACD);
        if (!tActor.ptSActorMovement || !tActor.ptSActorMovement->IsInArcingMovement())
        {
            BrainStanceData * ptCurStance = sGetBrainStanceData(tActor.id);
            if (tACD.IsAttached())
                sBrainStanceAttached(tActor.id);
            else if (ptCurStance->eStanceType == STANCE_ATTACHED)
                sBrainStanceFigureCurrentStance(tActor.id);
            if (tACD.AttributesGetInt(ATTRIBUTE_FORCE_GRIPPED) > 0)
                sBrainStanceForceGripped(tActor.id);
            else if (ptCurStance->eStanceType == STANCE_FORCE_GRIPPED)
                sBrainStanceFigureCurrentStance(tActor.id);
            if (tACD.AttributesGetInt(ATTRIBUTE_STUNNED) > 0)
            {
                if (ptCurStance->eStanceType != STANCE_KNOCK_BACK)
                    sBrainStanceStunned(tActor.id);
            }
            else if (ptCurStance->eStanceType == STANCE_STUNNED)
            {
                sBrainStanceFigureCurrentStance(tActor.id);
            }
            int32 nFrozenAttribute = tACD.AttributesGetInt(ATTRIBUTE_FROZEN);
            if (nFrozenAttribute > 0)
                sBrainStanceFrozen(tActor.id);
            else if (ptCurStance->eStanceType == STANCE_FROZEN)
                sBrainStanceFigureCurrentStance(tActor.id);
            int32 nFearedAttribute = tACD.AttributesGetInt(ATTRIBUTE_FEARED);
            if (nFrozenAttribute == 0 && nFearedAttribute > 0)
                sBrainStanceFeared(tActor.id);
            else if (ptCurStance->eStanceType == STANCE_FEARED)
                sBrainStanceFigureCurrentStance(tActor.id);
            switch (ptCurStance->eStanceType)
            {
            case STANCE_STUNNED:
            case STANCE_FEARED:
            case STANCE_FROZEN:
            case STANCE_FORCE_GRIPPED:
                break;
            default:
                ActorGetAI(tActor).dwNextBrainStanceUpdateTime = (uint32)-1;
                break;
            }
        }
    }
    if (ptStance->dwTimer <= dwNow) sBrainStanceTimerElapsed(tActor);
    if (ActorAIIsStatusFlagSet(tActor, ACTOR_AI_BROADCAST_BIT) && tAI.dwNextBroadcastTime <= dwNow)
        AIBroadcastUpdate(tActor);
}

// BrainStance.cpp:1457
void sBrainEvaluateDamageDoneAchievements(PowerAPIDamage & tDamage, float flTotalDamage)
{
    Achievement::EvaluationData tEvaluationData;
    tEvaluationData.SetPower(&tDamage);
    Player * ptPlayer = PlayerGetByACD(tDamage.idACDTarget);
    if (ptPlayer)
    {
        tEvaluationData.SetPlayer(ptPlayer);
        tEvaluationData.SetACD(ACDTryToGet(tDamage.idACDSource));
        if (PowerTryToGet(tDamage.idPower))
        {
            SNO snoPower = tEvaluationData.GetPowerSNO();
            tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_PLAYER_HIT_BY_POWER, snoPower, 1);
            if (flTotalDamage > 0.0f)
                tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_PLAYER_DAMAGED_BY_POWER, snoPower, 1);
        }
    }
    else
    {
        ptPlayer = ACDGetControllingPlayer(tDamage.idACDSource);
        if (ptPlayer)
        {
            ActorCommonData * ptACDTarget = ACDTryToGet(tDamage.idACDTarget);
            if (ptACDTarget && ptACDTarget->IsMonster())
            {
                tEvaluationData.SetPlayer(ptPlayer);
                tEvaluationData.SetACD(ptACDTarget);
                tEvaluationData.EvaluateEvent(tEvaluationData.GetPlayer(), Achievement::ACH_EVENT_MONSTER_DAMAGED_BY_PLAYER, ptACDTarget->GetActorSNO(), 1);
                tEvaluationData.EvaluateEvent(tEvaluationData.GetPlayer(), Achievement::ACH_EVENT_PLAYER_DEALT_DAMAGE_TO_MONSTER, ptACDTarget->GetActorSNO(), Math::Max((int32)flTotalDamage, 0));
            }
        }
    }
}

// BrainStance.cpp:2191
void BrainUpCallOnPowerStop(ActorID idActor, PowerID idPower, BOOL bAllowAIUpdate)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor || !ActorHasBrain(idActor))
        return;
    sBrainStanceFigureCurrentStance(idActor);
    ACDID idACD = ActorGet(idActor)->idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD || !ptACD->IsAlive())
        return;
    Power * ptPower = PowerTryToGet(idPower);
    if (bAllowAIUpdate && ptPower)
    {
        if (PowerGeneralTagGetBool(AutoPowerDefinitionR(ptPower->snoPower), TAG_POWER_CALL_AIUPDATE_IMMEDIATELY_UPON_TERMINATION))
            ActorAIRunUpdateNow(*ptActor);
    }
}

// BrainStance.cpp:409
static void sBrainStanceFeared(ActorID idActor)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    ptStance->eStanceType = STANCE_FEARED;
}

// BrainStance.cpp:419
static void sBrainStanceAttached(ActorID idActor)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    if (ptStance->eStanceType == STANCE_ATTACHED)
        return;
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    ActorAnimationTag eAttachedAnimTag = TAG_ACTOR_ANIMATION_STRUGGLE;
    ActorAnimationTag eOverrideAnimTag = (ActorAnimationTag)tACD.AttributesGetInt(ATTRIBUTE_OVERRIDE_ATTACHED_ANIM);
    if (eOverrideAnimTag != -1)
        eAttachedAnimTag = eOverrideAnimTag;
    sBrainStancePlayAnimation(idActor, eAttachedAnimTag, STANCE_ATTACHED, (uint32)-2, 0.0f, FALSE, 1.0f);
    BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
    BrainAPIDoIdle(idActor);
}

// BrainStance.cpp:364
static void sBrainStanceForceGripped(ActorID idActor)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    if (ptStance->eStanceType == STANCE_FORCE_GRIPPED)
        return;
    ACDID idACD = ActorGet(idActor)->idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return;
    ActorMovement * ptMove = ptACD->GetActorMovement();
    if (ptMove->IsMoving() && (ptMove->m_uMoveFlags & (1 << MOVEBIT_KNOCKBACK)))
        return;
    uint32 dwForceGripAnimTag = TAG_ACTOR_ANIMATION_KNOCK_BACK;
    BOOL bHasAnim = AnimPlayerAnimExists(*ptACD, dwForceGripAnimTag);
    if (!ptACD->IsInKnockback())
        sBrainStancePlayAnimation(idActor, bHasAnim ? dwForceGripAnimTag : TAG_ACTOR_ANIMATION_NEUTRAL, STANCE_FORCE_GRIPPED, (uint32)-2, 0.0f, FALSE, 1.0f);
    BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
}

// BrainStance.cpp:318
static void sBrainStanceStunned(ActorID idActor)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    if (ptStance->eStanceType == STANCE_STUNNED)
        return;
    ACDID idACD = ActorGet(idActor)->idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return;
    ActorMovement * ptMove = ptACD->GetActorMovement();
    if (ptMove->IsMoving() && (ptMove->m_uMoveFlags & (1 << MOVEBIT_KNOCKBACK)))
        return;
    uint32 dwStunAnimTag = TAG_ACTOR_ANIMATION_STUNNED;
    BOOL bHasAnim = AnimPlayerAnimExists(*ptACD, dwStunAnimTag);
    if (!ptACD->IsInKnockback())
        sBrainStancePlayAnimation(idActor, bHasAnim ? dwStunAnimTag : TAG_ACTOR_ANIMATION_NEUTRAL, STANCE_STUNNED, (uint32)-2, 0.0f, FALSE, 1.0f);
    BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
}

// BrainStance.cpp:287
static void sBrainStanceFrozen(ActorID idActor)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    if (ptStance->eStanceType == STANCE_FROZEN)
        return;
    ACDID idACD = ActorGet(idActor)->idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        ActorMovement * ptMove = ptACD->GetActorMovement();
        if (ptMove && !ptMove->IsInArcingMovement())
            ptMove->StopNow();
    }
    BrainTranslateCancel(sGetBrainStanceData(idActor)->tTranslate, TRUE);
    sBrainStanceSetTimer(idActor, (uint32)-1);
    sBrainStanceTypeSet(idActor, STANCE_CHANGING, -1);
    ActorPowersInterruptPrimary(idActor, TRUE);
    sBrainStanceTypeSet(idActor, STANCE_FROZEN, -1);
    ActorCommonData & tACD = ACDGet(ActorGet(idActor)->idACD);
    AnimPlayerUpdateAnimEndTime(tACD, (uint32)-2);
    BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
}

// BrainStance.cpp:449
static void sBrainStanceFigureCurrentStance(ActorID idActor)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    if (ptStance->eStanceType == STANCE_DYING || ptStance->eStanceType == STANCE_DEAD)
        return;
    if (ptStance->eStanceType == STANCE_WALKING)
        BrainTranslateCancel(ptStance->tTranslate, FALSE);
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    if (tACD.IsAttached())
    {
        sBrainStanceAttached(idActor);
    }
    else if (tACD.AttributesGetInt(ATTRIBUTE_FORCE_GRIPPED) > 0)
    {
        sBrainStanceForceGripped(idActor);
    }
    else if (tACD.AttributesGetInt(ATTRIBUTE_STUNNED) > 0)
    {
        sBrainStanceStunned(idActor);
    }
    else if (tACD.AttributesGetInt(ATTRIBUTE_FROZEN) > 0)
    {
        sBrainStanceFrozen(idActor);
    }
    else if (tACD.AttributesGetInt(ATTRIBUTE_FEARED) > 0)
    {
        sBrainStanceFeared(idActor);
    }
    else
    {
        if (ptStance->eStanceType == STANCE_FEARED)
            BrainTranslateCancel(ptStance->tTranslate, FALSE);
        if (ptStance->eStanceType != STANCE_IDLE)
            AITrace(idActor, "stance idle");
        sBrainStanceSetTimer(idActor, (uint32)-1);
        sBrainStanceTypeSet(idActor, STANCE_IDLE, -1);
        AnimPlayerPlayIdle(tACD);
        PlayerBrainTryQueuedPower(idACD);
        if (ACDIsPlayer(tACD.id) || tACD.IsPlayerTeam() || (tACD.IsMonster() && tACD.eMonsterRarity == 7) ||
            BrainAPIIsActorExcludedFromPerformanceOptimizations(tACD))
            BrainTriggerUpdate(idActor);
    }
}

// BrainStance.cpp:2244 (inlined only; return type void and locals idPower/bNeedsUpcall/idACD from the DWARF)
static void sBrainWalkingSkillBlocked(ActorID idActor)
{
    PowerID idPower = ActorPowersGetPrimary(idActor);
    if (idPower == POWERID_INVALID) return;
    BOOL bNeedsUpcall = PowerNeedsUpcallFromWalking(idPower);
    if (!bNeedsUpcall) return;
    ACDID idACD = ActorGet(idActor)->idACD;
    PowerAtDestination(idPower, idACD, ACDID_INVALID, 0);
}
// BrainStance.cpp:2281
void BrainUpCallAtDestination(ActorID idActor, BOOL bStuck, BOOL bWithinRadiusOfTarget, ACDID idACDBlocker)
{
    if (!ActorHasBrain(idActor)) return;
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    if (ptStance->snoPower != SNO_NONE && PowerIsCompletedWhenWalkingStops(ptStance->snoPower))
        if (!bStuck || bWithinRadiusOfTarget)
        {
            AIQueueCheckActionCompleted(idActor, ptStance->snoPower, QUEUED_ACTION_POWER);
            AIQueueCheckActionCompleted(idActor, -1, QUEUED_ACTION_MOVE_TO_LOCATION);
        }
    if (!ActorTryToGet(idActor)) return;
    BOOL bClientWalking = FALSE;
    PowerID idPower = ActorPowersGetPrimary(idActor);
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower && PowerShouldUseClientWalking(tACD, ptPower->snoPower)) bClientWalking = TRUE;
    if (bStuck)
    {
        tACD.AttributesSetInt(ATTRIBUTE_LAST_BLOCKED_ACDID, idACDBlocker);
        tACD.AttributesSetInt(ATTRIBUTE_LAST_BLOCKED_TIME, GameGetServerTime());
    }
    if (ptStance->eStanceType == STANCE_SKILL && !bClientWalking)
        sBrainWalkingSkillBlocked(idActor);
    else if (ptStance->eStanceType == STANCE_WALKING && !BrainComponentChargeDoPower(idActor))
    {
        AITrace(idActor, "at destination");
        BrainStanceDoIdle(idActor);
        if (ACDIsPlayer(tACD.id) || tACD.IsPlayerTeam() || (tACD.IsMonster() && tACD.eMonsterRarity == 7) || BrainAPIIsActorExcludedFromPerformanceOptimizations(tACD))
        {
            ActiveComponent eActiveComponent = BrainAPIGetActiveComponent(idActor);
            if (eActiveComponent == ACTIVE_COMPONENT_WALK) BrainAPITriggerUpdate(idActor, TRUE);
            BrainComponentsTriggerUpdate(idActor, TRUE);
        }
    }
}
// BrainStance.cpp:2460 (inlined only; params idACD/snoPower and local sPower from the DWARF)
static void sBrainStateTraceUsePowerStat(ACDID idACD, SNO snoPower)
{
    if (!TraceStatEnabled()) return;
    if (!ACDIsPlayer(idACD)) return;
    CRefString sPower = SNOToString(POWER_SNO, snoPower, FALSE);
    TraceStat("skill", idACD, "%s", sPower.str());
}
// BrainStance.cpp:2482
PowerID BrainStanceDoPower(ActorID idActor, const Target & tTarget, SNO snoPower, EBrainPowerResult eReason, void * ptExtraPowerStartData,
                           const PreplayedAnimData * ptPreplayedAnimData, int32 nComboLevel, uint32 dwTargetMessageFlags)
{
    PowerID idPower = POWERID_INVALID;
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    if (ActorGetBrainType(idActor) == ACTORBRAIN_HIRELING) HirelingBrainClearAttack(idActor);
    if (eReason == BRAIN_POWER_START)
    {
        BrainTranslateCancel(ptStance->tTranslate, TRUE);
        sBrainStanceSetTimer(idActor, (uint32)-1);
        ACDID idACD = ActorGet(idActor)->idACD;
        sBrainStanceTypeSet(idActor, STANCE_CHANGING, -1);
        PowerStartData tPowerStartData;
        PowerStartDataInit(tPowerStartData);
        tPowerStartData.idACD = idACD;
        tPowerStartData.idACDAlternateSource = idACD;
        tPowerStartData.snoPower = snoPower;
        tPowerStartData.tTarget = tTarget;
        tPowerStartData.ptExtraPowerStartData = ptExtraPowerStartData;
        tPowerStartData.nComboLevel = nComboLevel;
        tPowerStartData.dwTargetMessageFlags = dwTargetMessageFlags;
        if (ptPreplayedAnimData) tPowerStartData.tPreplayedAnimData = *ptPreplayedAnimData;
        idPower = PowerStartEx(tPowerStartData, ATTRIBGROUPID_INVALID, FALSE, FALSE);
        if (ActorPowersGetPrimary(idActor) != POWERID_INVALID)
        {
            AITrace(idActor, "do power %s", SNOToString(POWER_SNO, snoPower, FALSE).str());
            sBrainStateTraceUsePowerStat(idACD, snoPower);
            if (!PowerIsGizmo(snoPower)) sBrainStanceTypeSet(idActor, STANCE_SKILL, snoPower); else sBrainStanceTypeSet(idActor, STANCE_IDLE, snoPower);
        }
        else
        {
            if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO)) AITrace(idActor, "do instant power %s", SNOToString(POWER_SNO, snoPower, FALSE).str());
            else AITrace(idActor, "can't do power %s", SNOToString(POWER_SNO, snoPower, FALSE).str());
            BrainStanceDoIdle(idActor);
        }
    }
    else if (eReason == BRAIN_POWER_RETARGET)
    {
        BrainStanceData * ptStance2 = sGetBrainStanceData(idActor);
        if (ptStance2->eStanceType != STANCE_SKILL) return POWERID_INVALID;
        PowerID idPrimaryPower = ActorPowersGetPrimary(idActor);
        UpdateTargetData tUpdateTargetData;
        tUpdateTargetData.tNewTarget = tTarget;
        PowerUpdateTarget(idPrimaryPower, &tUpdateTargetData);
        AITrace(idActor, "retarget power %s", SNOToString(POWER_SNO, snoPower, FALSE).str());
        return idPrimaryPower;
    }
    return idPower;
}


// BrainStance.cpp:2775
void BrainStanceDoDeathReplacement(ActorID idActor, ActorID idKillerActor)
{
    Actor * ptActor = idActor != ACTORID_INVALID ? ActorTryToGet(idActor) : NULL;
    ACDID idACD = ptActor ? ptActor->idACD : ACDID_INVALID;
    ACDID idACDKiller = (ACDID)idKillerActor;
    if (idKillerActor != ACTORID_INVALID)
    {
        Actor * ptKillerActor = ActorTryToGet(idKillerActor);
        idACDKiller = ptKillerActor ? ptKillerActor->idACD : ACDID_INVALID;
    }
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return;
    SNO snoPower = ptACD->AttributesGetInt(ATTRIBUTE_DEATH_REPLACEMENT_POWER_SNO);
    if (snoPower == -1)
        return;
    ActorCommonData * ptACDOwner = ACDTryToGet(ptACD->id);
    if (!ActorTryToGet(ptACDOwner ? (ActorID)ptACDOwner->idOwner : ACTORID_INVALID))
        return;
    ptACD->SetHitPoints(0.001f, TRUE, FALSE);
    ptACD->SetTargetable(FALSE);
    BrainTranslateCancel(sGetBrainStanceData(idActor)->tTranslate, TRUE);
    sBrainStanceSetTimer(idActor, (uint32)-1);
    sBrainStanceTypeSet(idActor, STANCE_CHANGING, -1);
    ActorPowersInterruptPrimary(idActor, TRUE);
    sBrainStanceTypeSet(idActor, STANCE_IDLE, -1);
    ptACD->SetInvulnerable(TRUE);
    PowerAPI_BuffRemoveOnDeath(*ptACD, idACDKiller);
    ptACD->SetInvulnerable(FALSE);
    ptACD->RestoreStatus();
    ptACD->SetTargetable(TRUE);
    Target tTarget;
    tTarget.InitializeFromACD(idACD, FALSE, FALSE);
    PowerStart(idACD, snoPower, tTarget);
}

// BrainStance.cpp:3205
void BrainStanceDoKnockDown(ActorID idActor, uint32 dwAnimTag)
{
    if (!PowerActorCanBeKnockedBack(idActor, POWERID_INVALID))
        return;
    ActorCommonData & tACD = ACDGet(ActorGet(idActor)->idACD);
    if (!AnimPlayerAnimExists(tACD, dwAnimTag))
        return;
    ACDPlayEffect(tACD.id, EFFECT_KNOCK_BACK, 0, ACDID_INVALID);
    BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
    WorldPlace placeActor = tACD.GetPlace();
    SNO snoKnockdownPower = GlobalSNOGet(GLOBALSNO_POWERS_KNOCKDOWN);
    PowerSetCoolDown(tACD, 0);
    Target tKnockdownTarget;
    tKnockdownTarget.InitializeFromPlace(placeActor);
    uint32 dwFailureReasons = PowerACDCanStartPowerFailureReasons(tACD, AutoPowerDefinitionR(snoKnockdownPower), &tKnockdownTarget, FALSE, FALSE);
    if (dwFailureReasons == 0)
    {
        ActorMovement * ptMove = tACD.GetActorMovement();
        if (ptMove)
            ptMove->StopNow();
        PowerStartData tPowerStartData;
        PowerStartDataInit(tPowerStartData);
        tPowerStartData.idACD = tACD.id;
        tPowerStartData.snoPower = snoKnockdownPower;
        tPowerStartData.tTarget.InitializeFromPlace(placeActor);
        PowerStartEx(tPowerStartData, ATTRIBGROUPID_INVALID, FALSE, FALSE);
    }
}

// BrainStance.cpp:535
static void sBrainStanceStop(ActorID idActor)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    BrainTranslateCancel(ptStance->tTranslate, TRUE);
    sBrainStanceTypeSet(idActor, STANCE_CHANGING, -1);
    ActorPowersInterruptPrimary(idActor, FALSE);
    sBrainStanceFigureCurrentStance(idActor);
}

// BrainStance.cpp:551
void BrainTriggerUpdate(ActorID idActor)
{
    if (ActorHasBrain(idActor))
    {
        ActorAI & tAI = ActorGetAI(idActor);
        uint32 dwNow = GameGetServerTime();
        tAI.dwNextBrainUpdateTime = dwNow;
        if (ACDIsPlayer(ActorGet(idActor)->idACD))
            ServerQueueUpdateFunction(0, tAI.tAIUpdateData);
    }
}

// BrainStance.cpp:617
BOOL BrainStanceNeedToPlayWalkCoolDown(ActorID idActor)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    if (ptStance->eStanceType == STANCE_WALKING && !ActorIsInDeactivatedScene(idActor))
    {
        ACDID idACD = ACDFindByActor(idActor);
        ActorCommonData & tACD = ACDGet(idACD);
        uint32 dwCoolDownTag = AnimPlayerGetWalkCoolDownTag(tACD, ptStance->tTranslate.dwAnimTag);
        return dwCoolDownTag != (uint32)-1;
    }
    return FALSE;
}

// BrainStance.cpp:2949
void BrainStanceDoDead(ActorID idActor)
{
    if (!PowerActorCanBeDead(idActor))
        return;
    BrainTranslateCancel(sGetBrainStanceData(idActor)->tTranslate, TRUE);
    sBrainStanceSetTimer(idActor, (uint32)-1);
    sBrainStanceTypeSet(idActor, STANCE_CHANGING, -1);
    ActorPowersInterruptPrimary(idActor, FALSE);
    sBrainStanceTypeSet(idActor, STANCE_DEAD, -1);
    Actor * ptActor = ActorGet(idActor);
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    BOOL bCouldHaveRagdolled = tACD.AttributesGetInt(ATTRIBUTE_COULD_HAVE_RAGDOLLED);
    BOOL bFrozen = tACD.AttributesGetInt(ATTRIBUTE_FROZEN);
    if (bFrozen)
        tACD.AttributesSetInt(ATTRIBUTE_FROZEN, 0);
    uint32 dwDeadBodyAnimTag = tACD.AttributesGetInt(ATTRIBUTE_DEAD_BODY_ANIMTAG);
    ScopedActorDefinitionLockR pdefActor(ptActor->snoActor);
    BOOL bRemovesCorpse = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_REMOVES_CORPSE_ON_DEAD);
    if (!bCouldHaveRagdolled)
    {
        AnimPlayerSetIdleAnimation(tACD, dwDeadBodyAnimTag != (uint32)-1 ? dwDeadBodyAnimTag : TAG_ACTOR_ANIMATION_NEUTRAL);
        AnimPlayerPlayIdle(tACD);
    }
    ActorGetAI(idActor).dwTimeOfDead = GameGetServerTime();
    ACDPlayEffect(tACD.id, EFFECT_DEAD, 0, ACDID_INVALID);
    if (tACD.AttributesGetInt(ATTRIBUTE_RESURRECTION_POWER) == -1)
    {
        if (bRemovesCorpse || bCouldHaveRagdolled)
        {
            if (!ACDIsPlayer(tACD.id) && tACD.AttributesGetInt(ATTRIBUTE_HEADSTONE_PLAYER_ANN) == -1)
            {
                if (bFrozen)
                    ActorDeleteSoon(idActor, 2);
                else
                    ActorFlagForDeletion(idActor);
            }
        }
        else if (!TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_CORPSE_DELETES_INSTEAD_OF_INACTIVE))
        {
            ActorFlagSet(idActor, ACTOR_DYING, FALSE);
        }
    }
}

// BrainStance.cpp:645
static void sBrainStancePlayWalkCoolDown(ActorID idActor)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    BrainComponentTranslate & tTranslate = ptStance->tTranslate;
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    const int32 MIN_TIME_BEFORE_PLAY_COOLDOWN = 30;
    uint32 dwNow = GameGetServerTime();
    if (dwNow < tTranslate.dwTimeStartedWalking + MIN_TIME_BEFORE_PLAY_COOLDOWN)
    {
        AnimPlayerSendIdle(tACD);
        sBrainStanceFigureCurrentStance(idActor);
    }
    else
    {
        uint32 dwCoolDownTag = AnimPlayerGetWalkCoolDownTag(tACD, tTranslate.dwAnimTag);
        sBrainStancePlayAnimation(idActor, dwCoolDownTag, STANCE_WALK_COOL_DOWN, (uint32)-1, 0.0f, FALSE, 1.0f);
    }
}

// BrainStance.cpp:674
BOOL BrainStanceIsInWalkingStance(ActorID idActor)
{
    if (!ActorHasBrain(idActor))
        return FALSE;
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    BOOL bResult = FALSE;
    if (ptStance->eStanceType == STANCE_WALKING || ptStance->eStanceType == STANCE_FEARED)
        bResult = TRUE;
    return bResult;
}

// BrainStance.cpp:835
void BrainStanceDoTranslate(ActorID idActor, const WorldPlace & placeGotoPoint, WorldDistance wdSpeed, float flAccel, TranslateType nTranslateType,
                            uint32 dwAnimTag, WorldDistance wdMaxRangeFromDest, LPCSTR sTraceString, ACDID idACDTarget, SNO snoPowerPassability)
{
    if (!PowerActorCanWalk(idActor))
    {
        AITrace(idActor, "can't walk cuz !PowerActorCanWalk (%s)", sTraceString);
        return;
    }
    if (ActorIsInDeactivatedScene(idActor))
    {
        AITrace(idActor, "can't walk cuz scene deactivated (%s)", sTraceString);
        return;
    }
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    BrainComponentTranslate & tTranslate = ptStance->tTranslate;
    if (BrainStanceIsInWalkingStance(idActor))
    {
        BOOL bSameDest = FALSE;
        if (WorldPlaceInSameWorld(tTranslate.placeGotoPoint, placeGotoPoint))
        {
            WorldDistance wdBetweenSquared = WorldPlaceDistanceSquared(tTranslate.placeGotoPoint, placeGotoPoint);
            if (wdBetweenSquared < 1.0f)  // BrainStance.cpp:872 col 25 is the '<' of an if; the assignment has no row (cset)
                bSameDest = TRUE;
        }
        if (tTranslate.nTranslateType == nTranslateType && tTranslate.dwAnimTag == dwAnimTag && bSameDest)  // BrainStance.cpp:878-879 '&&' columns give this order
        {
            if (wdSpeed != 0.0f)
                tTranslate.wdSpeed = wdSpeed;
            return;
        }
    }
    else
    {
        uint32 dwNow = GameGetServerTime();
        tTranslate.dwTimeStartedWalking = dwNow;
    }
    Actor * ptActor = ActorGet(idActor);
    if (!ptActor->ptSActorMovement) return;
    ActorCommonData * ptACD = ACDTryToGet(tTranslate.idACD);
    if (ptACD && wdSpeed < 1e-06f)  // BrainStance.cpp:905 columns 9/15/26: ptACD is tested before the speed compare
        wdSpeed = ptACD->GetWalkingSpeed(dwAnimTag);
    uint32 uMoveFlags = (1 << MOVEBIT_AUTO_UPDATE_YAW);
    if (nTranslateType != TRANSLATE_TYPE_CHEAP && (ACDIsPlayer(tTranslate.idACD) || IsTriggerEventTargetingActor(idActor)))
        uMoveFlags |= (1 << MOVEBIT_ACTIVATE_TRIGGERS);
    if (ptACD && ptACD->eMonsterRarity == 7)
        uMoveFlags |= (1 << MOVEBIT_DYNAMIC_PATHING);
    ptActor->ptSActorMovement->MoveTo(placeGotoPoint, wdSpeed, flAccel, dwAnimTag, uMoveFlags, wdMaxRangeFromDest, idACDTarget, snoPowerPassability, 0, -1, 0.0f, NULL);
}

// BrainStance.cpp:1044
void BrainStanceInitializeForActor(ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    BrainStanceGlobals * ptBrainStanceGlobals = SGameManager::GameGet().ptBrainStanceGlobals;
    tAI.ptBrainStanceData = ptBrainStanceGlobals->poolBrainStances.Alloc_Tracked();
    SigmaMemoryClear(tAI.ptBrainStanceData, sizeof(BrainStanceData));
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    sBrainStanceTranslateInitialize(idActor);
    sBrainStanceTypeSet(idActor, STANCE_IDLE, -1);
    ptStance->snoLastSkillStancePower = -1;
    ptStance->dwTimer = (uint32)-1;
    ptStance->snoPower = -1;
    BrainComponentInitializeForActor(idActor);
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    AnimPlayerPlayIdle(tACD);
}

// BrainStance.cpp:1313
void BrainStanceChangeBrain(const ActorID idActor, ActorBrain nBrain)
{
    sBrainStanceStop(idActor);
}

// BrainStance.cpp:1325
void BrainStanceReset(const ActorID idActor)
{
    sBrainStanceStop(idActor);
}

// BrainStance.cpp:1334
void BrainStanceForceUpdate(ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    uint32 dwNow = GameGetServerTime();
    tAI.dwNextBrainStanceUpdateTime = dwNow;
}

// BrainStance.cpp:1381
BOOL sCheckRecoil(const PowerAPIDamage & tDamage, ActorID idActorTarget, ACDID idACDTarget)
{
    if (tDamage.idPower != POWERID_INVALID && PowerContactTagGetBool(tDamage.idPower, 0x50120))
        return FALSE;
    if (tDamage.bNeverCauseGetHit)
        return FALSE;
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    if (tACDTarget.AttributesGetInt(ATTRIBUTE_GETHIT_IMMUNE))
        return FALSE;
    if (tACDTarget.AttributesGetInt(ATTRIBUTE_ALWAYS_PLAYS_GETHIT) > 0)
        return TRUE;
    if (!PowerActorCanDoGetHit(idActorTarget))
        return FALSE;
    float flNewGetHitAmount = tACDTarget.AttributesGetFloat(ATTRIBUTE_GETHIT_CURRENT);
    if (flNewGetHitAmount > tACDTarget.AttributesGetFloat(ATTRIBUTE_GETHIT_MAX))
    {
        tACDTarget.AttributesSetFloat(ATTRIBUTE_GETHIT_CURRENT, 0.0f);
        return TRUE;
    }
    return FALSE;
}

// BrainStance.cpp:2230
void BrainUpCallOnWalk(ActorID idActor)
{
    if (ActorHasBrain(idActor))
        sBrainStanceFigureCurrentStance(idActor);
}

// BrainStance.cpp:2267
void BrainStanceSetPower(ActorID idActor, SNO snoPower)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    if (ptStance)
        ptStance->snoPower = snoPower;
}

// BrainStance.cpp:2369
EStanceType BrainGetStanceType(ActorID idActor)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    return ptStance->eStanceType;
}

// BrainStance.cpp:2388
void BrainStanceDoPlayAnimation(const ActorID idActor, const uint32 dwAnimTag, const uint32 dwDuration, const BOOL bAllowPlayingAnimInterrupt, const BOOL bInterruptableAnim, float flUnitStartTime, const BOOL bDontSendToClient, const float flSpeedScalar)
{
    if (PowerActorCanPlayAnimation(idActor, bAllowPlayingAnimInterrupt))
        sBrainStancePlayAnimation(idActor, dwAnimTag, bInterruptableAnim ? STANCE_PLAY_INTERRUPTABLE_ANIMATION : STANCE_PLAY_ANIMATION, dwDuration, flUnitStartTime, bDontSendToClient, flSpeedScalar);
}

// BrainStance.cpp:2416
void BrainStanceDoIdle(ActorID idActor)
{
    if (PowerActorCanDoAnything(idActor))
        sBrainStanceFigureCurrentStance(idActor);
    else if (BrainStanceNeedToPlayWalkCoolDown(idActor))
        sBrainStancePlayWalkCoolDown(idActor);
    else if (PowerActorCanGracefullyStopSkill(idActor))
        ActorPowersDetachPrimary(idActor);
}

// BrainStance.cpp:2440
BOOL BrainStanceIsWalking(ActorID idActor)
{
    BrainStanceData * ptStance = sGetBrainStanceData(idActor);
    switch (ptStance->eStanceType)
    {
    case STANCE_WALK_WARM_UP:
    case STANCE_WALKING:
    case STANCE_WALK_COOL_DOWN:
    case STANCE_FEARED:
        return TRUE;
    default:
        return FALSE;
    }
}

// BrainStance.cpp:2589
void BrainStanceSetIdleAnimation(ActorID idActor, uint32 dwIdleAnimationTag)
{
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    AnimPlayerSetIdleAnimation(tACD, dwIdleAnimationTag);
}

// BrainStance.cpp:2762
void sRemoveCollision(TimerID, DataID, void * ptData)
{
    ActorCommonData * ptACD = (ActorCommonData *)ptData;
    ptACD->EnableCollision(FALSE);
    ptACD->AttributesSetFloat(ATTRIBUTE_DODGE_CHANCE_BONUS_RANGED, 1.0f);
}

// BrainStance.cpp:2910
void BrainStanceDoResurrect(ActorID idActor)
{
    sBrainStanceTypeSet(idActor, STANCE_CHANGING, -1);
    sBrainStanceFigureCurrentStance(idActor);
    Actor * ptActor = ActorGet(idActor);
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_RESURRECTION))
        sBrainStancePlayAnimation(idActor, TAG_ACTOR_ANIMATION_RESURRECTION, STANCE_PLAY_ANIMATION, (uint32)-1, 0.0f, FALSE, 1.0f);
    AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_NEUTRAL);
}

// BrainStance.cpp:3265
void BrainStanceSetWalking(ActorID idActor)
{
    sBrainStanceSetTimer(idActor, (uint32)-1);
    sBrainStanceTypeSet(idActor, STANCE_WALKING, -1);
}

static BOOL sBrainStanceRunGetHitLogic(
    const PowerAPIDamage & tDamage)
{
    ACDID idACDTarget = tDamage.idACDTarget; // Inference: ACDGetOwner returns DataID; convert to the DWARF ActorID local below.
    ActorID idActor = (ActorID)ACDGetOwner(idACDTarget);



    if (tDamage.idPower != -1 &&

        PowerContactTagGetBool(tDamage.idPower, TAG_POWER_NEVER_CAUSES_RECOIL))
    {
        return FALSE;
    }


    if (tDamage.bNeverCauseGetHit)
    {
        return FALSE;
    }

    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    if (tACDTarget.AttributesGetInt(ATTRIBUTE_GETHIT_IMMUNE))
    {
        return FALSE;
    }

    if (!PowerActorCanDoGetHit(idActor))
    {
        return FALSE;
    }

    if (tACDTarget.AttributesGetInt(ATTRIBUTE_ALWAYS_PLAYS_GETHIT) > 0)
    {
        return TRUE;
    }

    if (tDamage.bAlwaysCauseGetHit)
    {
        return TRUE;
    }


    if (tACDTarget.AttributesGetFloat(ATTRIBUTE_BREAKABLE_SHIELD_HP) > EPSILON)
    {
        return TRUE;
    }

    // Inference: no hit-point entry leaves zero damage, as the original loop's initial accumulator does.
    float flHitPointDamage = 0.0f;
    POS pos = tDamage.listDamage.GetHeadPosition();
    while (pos)
    {
        const DamageAttribute & tDamageAttribute = tDamage.listDamage.GetNext(pos);
        if (KeyGetAttrib(tDamageAttribute.tKey) == ATTRIBUTE_HITPOINTS_CUR)
        {
            flHitPointDamage = -tDamageAttribute.flValue;
        }
    }

    if (flHitPointDamage < EPSILON)
    {
        return FALSE;
    }
    // Inference: degenerate maximum health or denominator leaves chance one in the merged original tail.
    float flChanceToPlayGetHit = 1.0f;
    const float flTargetMax = tACDTarget.GetHitPointsMaxTotal();

    if (flTargetMax >= EPSILON)
    {





        ScopedGlobalDefinitionLockR pdefGlobal;

        float flDenom = pdefGlobal->ptServerData->flMonsterGetHitHealthPercent * flTargetMax;
        if (flDenom >= EPSILON)
        {
            flChanceToPlayGetHit = flHitPointDamage / flDenom;
        }
    }






    ActorCommonData * ptACDDamageSource = ACDTryToGet(tDamage.idACDSource);
    if (ptACDDamageSource)
    {
        float flChanceBoost = ptACDDamageSource->AttributesGetFloat(ATTRIBUTE_MONSTER_PLAY_GETHIT_BONUS);
        flChanceToPlayGetHit += flChanceBoost;
    }

    if (flChanceToPlayGetHit < EPSILON)
    {
        return FALSE;
    }

    float flRandom = RandFloat_Internal(SGameGlobalsGet().rng);

    return flRandom <= flChanceToPlayGetHit;
}










// Inference: declare the canonical type locally; DWARF BrainStance.h:73, fields75/76, size8 offsets0/4.
struct KnockbackPowerData {
    float flHeight;
    float flGravity;
};
// DWARF X/Server/Powers/Powers.cpp:5482.
BOOL PowerComputeDestination(SNO snoPower, const ActorMovement & tMovement, const WorldPlace & placeTarget, WorldPlace & placeResult, BOOL bActivateTriggers, float flSearchRadiusMax);
// DWARF X/Server/AI/PlayerBrain.cpp:405.
void PlayerBrainClearQueuedPower(ACDID idACDPlayer);
#include "../SGameStats.h"

// DWARF X/Server/Experience.cpp:840.
ACDID ExperienceGetUltimateOwner(const ActorCommonData & tACD);
// DWARF X/Server/Network/ServerMessageSend.cpp:1541.
void ServerMessageSendEliteEngagedMsg(const Player & tPlayer, const ActorCommonData & tACDMonster);
// DWARF X/Common/ActorCommonDataSwig.h:32.
enum { MF_TREASURE_GOBLIN = 11 };
// DWARF X/Server/SPlayer.cpp:4421,4546.
void PlayerRecordDamageDone(const ACDID idACDPlayer, float flDamage, const ACDID idACDTarget, const BOOL bCriticalHit, const BOOL bDOT);
void PlayerRecordDamageTaken(const ACDID idACDPlayer, float flDamage, const ACDID idACDAttacker, const BOOL bCriticalHit, const BOOL bDOT);
// DWARF X/Common/Items.cpp:8973.
void ItemsTakeDamage(ActorCommonData * ptACDOwner);
// DWARF X/Server/AI/BrainComponents.cpp:7722.
void BrainComponentWasAttacked(ActorID idTargetActor, ActorID idSourceActor);
#include "../STalk.h"
#include "../LuaScript/ScriptEvent.h"
// DWARF X/Server/Powers/Powers.cpp:5100.
void PowerAbortChannelled(ACDID idACDPlayer, SNO snoPower);
// DWARF X/Server/Powers/PowerAPI.cpp:11722,11803.
void PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(PowerID idPowerFrom, ACDID idACDAttacker, ACDID idACDTarget, SNO snoPower, CrowdControlType eCCType, uint32 dwDuration, float flProcScalar, BOOL bRefresh);
void PowerAPI_BuffApplyOnHitWithCrowdControlEffects(PowerID idPowerFrom, ACDID idACDAttacker, ACDID idACDTarget, SNO snoPower, CrowdControlType eCCType, uint32 dwDuration, float flProcScalar, BOOL bRefresh);
// DWARF X/Server/AI/BrainStance.cpp:3044.
void BrainStanceDoKnockBack(PowerID idPower, ActorID idActor, Angle aYaw, WorldDistance wdKnockbackMagnitude, float flHeight, float flGravity, uint32 dwAnimTag, BOOL bKnockbackIgnoresWeight, SNO snoKnockbackPower, BOOL bSuperKnockback, BOOL bContinuousKnockbackPower);




















































































































static BOOL sBrainOnDamageCallCheckForDeath(
    ActorID idTarget)
{
    const Actor * ptActorTarget = ActorGet(idTarget);
    ACDID idACDTarget = ptActorTarget->idACD;

    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    return tACDTarget.GetHitPointsCurrent() <= 0.0f;
}








static BOOL sActorHasSkills(
    const ACDID idACDTarget)
{
    SNO snoPower = ACDGet(idACDTarget).AttributesGetPower(0);
    return snoPower != SNO_NONE;
}


























































static void sProcessDamageState(
    ActorCommonData & tACDTarget)
{
    int32 nDamageStateMax = tACDTarget.AttributesGetInt(ATTRIBUTE_DAMAGE_STATE_MAX);
    if (nDamageStateMax > 0)
    {
        float flHPMax = tACDTarget.GetHitPointsMaxTotal();
        if (flHPMax > EPSILON)
        {
            int32 nDamageStateCurr = tACDTarget.AttributesGetInt(ATTRIBUTE_DAMAGE_STATE_CURRENT);
            nDamageStateCurr = Math::Pin(nDamageStateCurr, 0, nDamageStateMax);

            float flHPCurr = tACDTarget.GetHitPointsCurrent();
            float flTrueDamageState = (float)(nDamageStateMax + 1) * (flHPMax - flHPCurr) / flHPMax;
            int32 nTrueDamageState = Math::FloatFloorToInt(flTrueDamageState);
            nTrueDamageState = Math::Pin(nTrueDamageState, 0, nDamageStateMax);

            while (nDamageStateCurr < nTrueDamageState)
            {
                ++nDamageStateCurr;
                tACDTarget.ChangeDamageState(nDamageStateCurr);
            }
        }
    }
}






























































































const uint32 ENGAGED_TICKS = 180;
static void sRefreshSpecialMonsterEngagementTracker(
    ActorCommonData & tACDMonster,
    ActorCommonData & tACDEngager)
{
    if (!tACDMonster.IsElite() || (tACDMonster.eMonsterRarity >= MR_HIRELING && !ACDIsInLootRun(tACDMonster)))
    {
        return;
    }

    ACDID idACDUltimateEngager = ExperienceGetUltimateOwner(tACDEngager);
    Player * ptPlayer = PlayerGetByACD(idACDUltimateEngager);
    if (!ptPlayer)
    {
        return;
    }
    // Inference: the initial engaged monster remains the input unless its hostile owner qualifies below.
    ActorCommonData * ptACDMonsterEngaged = &tACDMonster;
    ACDID idACDOwner = ExperienceGetUltimateOwner(tACDMonster);





    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (idACDOwner != tACDMonster.id && ptACDOwner &&

        ptACDOwner->IsMonster() &&
        TeamsAreFriendly(ptACDOwner->GetTeam(), tACDMonster.GetTeam()) &&
        !TeamsAreFriendly(ptACDOwner->GetTeam(), tACDEngager.GetTeam()))
    {
        // Inference: dead qualifying owners return without broadcasting, from branch at 1581.
        if (!ptACDOwner->IsAlive())
        {
            return;
        }
        ptACDMonsterEngaged = ptACDOwner;
    }



    else if (idACDOwner != ACDID_INVALID && !ptACDOwner)
    {
        return;
    }

    ACDPack * ptPack = ACDPackTryToGet(ptACDMonsterEngaged->idACDPack);
    BOOL bBroadcastSent;
    if (ptPack)
    {
        bBroadcastSent = ptPack->bBroadcastSent;
    }
    else
    {
        bBroadcastSent = ptACDMonsterEngaged->AttributesGetInt(ATTRIBUTE_ELITE_ENGAGED);
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDUltimateEngager);
    if (ptACDPlayer)
    {
        uint32 uEngagedTime = GameGetServerTime() + ENGAGED_TICKS;
        Attrib eAttrib = ATTRIBUTE_ENGAGED_WITH_RARE_TIME;
        SNOName tSNONameMonster(ACTOR_SNO, ptACDMonsterEngaged->snoActor);
        MonsterFamily eMonsterFamily = MonsterGetMonsterFamily(tSNONameMonster);
        if (eMonsterFamily == MF_TREASURE_GOBLIN)
        {
            eAttrib = ATTRIBUTE_ENGAGED_WITH_TREASURE_GOBLIN_TIME;
        }

        ptACDPlayer->AttributesSetInt(eAttrib, uEngagedTime);
    }

    if (!bBroadcastSent)
    {
        if (ptPack)
        {
            ACDID idACDPackLeader = ptPack->GetLeader();


            const ACDIDListPooled * listPack = ptPack->GetMemberList();
            POS pos = listPack->GetHeadPosition();
            while (pos)
            {
                ACDID idACD = (ACDID)listPack->GetNext(pos);
                ActorCommonData * ptACD = ACDTryToGet(idACD);
                if (ptACD && ptACD->eMonsterRarity == MR_RARE)
                {
                    idACDPackLeader = idACD;
                    break;
                }
            }

            ActorCommonData * ptACDPackLeader = ACDTryToGet(idACDPackLeader);
            if (ptACDPackLeader)
            {
                ServerMessageSendEliteEngagedMsg(*ptPlayer, *ptACDPackLeader);
            }
            else
            {
                ServerMessageSendEliteEngagedMsg(*ptPlayer, *ptACDMonsterEngaged);
            }

            ptPack->bBroadcastSent = TRUE;
        }
        else
        {
            ServerMessageSendEliteEngagedMsg(*ptPlayer, *ptACDMonsterEngaged);
            ptACDMonsterEngaged->AttributesSetInt(ATTRIBUTE_ELITE_ENGAGED, TRUE);
        }
    }
}



static void sRefreshSpecialMonsterEngagementTracker(
    ACDID idACDTarget,
    ActorCommonData * ptACDDamageSource)
{
    if (idACDTarget != ACDID_INVALID && ptACDDamageSource)
    {
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget)
        {
            if (ptACDTarget->IsMonster())
            {
                sRefreshSpecialMonsterEngagementTracker(*ptACDTarget, *ptACDDamageSource);
            }

            if (ptACDDamageSource->IsMonster())
            {
                sRefreshSpecialMonsterEngagementTracker(*ptACDDamageSource, *ptACDTarget);
            }
        }
    }
}



// NON_MATCHING: 5412 vs 5456 bytes; get-hit/knockback branch layout and spills differ (frame 0xb0 vs 0xc0); tried combined guards and explicit facing-flag clearing.
void BrainUpCallOnDamage(
    PowerAPIDamage & tDamage,
    float * pflDamageDone)
{


    ACDID idACDTarget = tDamage.idACDTarget; // Inference: ACDGetOwner returns DataID; convert to the DWARF ActorID local below.
    ActorID idTargetActor = (ActorID)ACDGetOwner(idACDTarget);
    ActorCommonData * ptACDDamageSource = ACDTryToGet(tDamage.idACDSource);
    // Inference: absent damage arrays leave the original zero accumulator.
    float flTotalDamage = 0.0f;
    if (pflDamageDone)
    {
        for (int32 nDamageType = 0; nDamageType < NUM_DAMAGE_TYPES; ++nDamageType)
        {
            flTotalDamage += pflDamageDone[nDamageType];
        }
    }

    sBrainEvaluateDamageDoneAchievements(tDamage, flTotalDamage);


    ACDID idACDController = tDamage.idACDFloatingCombatTextSourceOverride != ACDID_INVALID ? tDamage.idACDFloatingCombatTextSourceOverride : ACDGetControllingActor(tDamage.idACDSource);
    if (ACDTryToGet(idACDController))
    {
        if (ACDIsPlayer(idACDController) && pflDamageDone)
        {
            PlayerRecordDamageDone(idACDController, flTotalDamage, idACDTarget, tDamage.bCriticalHit, tDamage.eDeliveryMechanism == PDM_DOT);
        }
    }
    else
    {


        Power * ptPower = PowerTryToGet(tDamage.idPower);
        if (ptPower)
        {
            ACDID idACDSummonedBy = (ACDID)FastAttribGetValueInt(ptPower->idFastAttrib, ATTRIBUTE_SUMMONED_BY_ACDID);
            ActorCommonData * ptACDSummonedBy = ACDTryToGet(idACDSummonedBy);
            if (ptACDSummonedBy && ACDIsPlayer(idACDSummonedBy) && pflDamageDone)
            {

                PlayerRecordDamageDone(idACDSummonedBy, flTotalDamage, idACDTarget, tDamage.bCriticalHit, tDamage.eDeliveryMechanism == PDM_DOT);
            }
        }
    }

    if (ACDIsPlayer(idACDTarget) && pflDamageDone)
    {

        PlayerRecordDamageTaken(idACDTarget, flTotalDamage, tDamage.idACDSource, tDamage.bCriticalHit, tDamage.eDeliveryMechanism == PDM_DOT);
    }

    sRefreshSpecialMonsterEngagementTracker(idACDTarget, ptACDDamageSource);

    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    ItemsTakeDamage(ptACDDamageSource);

    if (!ActorHasBrain(idTargetActor))
    {
        return;
    }

    ItemsTakeDamage(&tACDTarget);

    if (tACDTarget.AttributesGetInt(ATTRIBUTE_INVULNERABLE) > 0)
    {
        return;
    }

    if (tACDTarget.AttributesGetInt(ATTRIBUTE_CANNOT_DIE_DURING) > 0)
    {


        if (tACDTarget.GetHitPointsCurrent() < EPSILON)
        {
            tACDTarget.SetHitPoints(EPSILON, TRUE, FALSE);
            tACDTarget.AttributesSetInt(ATTRIBUTE_QUEUE_DEATH, TRUE);
        }
    }


    if (tACDTarget.AttributesGetInt(ATTRIBUTE_UNINTERRUPTIBLE) > 0)
    {
        return;
    }


    Actor * ptTargetActor = ActorGet(idTargetActor);

    ActorAI * ptTargetActorAI = ptTargetActor->ptAI;

    if (ptTargetActorAI->bMaintainThreatList &&
        tDamage.eDeliveryMechanism != PDM_THORNS &&
        !tDamage.bDontTriggerThreatListUpdate)
    {

        // Inference: the controlling actor is the threat fallback when the immediate source is not targetable.
        ACDID idACDThreatSource = idACDController;
        if (ptACDDamageSource && ptACDDamageSource->IsTargetable())
        {
            idACDThreatSource = tDamage.idACDSource;
        }





        // Inference: missing targetable source retains the controlling actor; positive damage gates threat updates.
        if (idACDThreatSource != ACDID_INVALID && flTotalDamage > EPSILON)
        {
            BOOL bFound = FALSE;
            POS pos = ptTargetActorAI->tThreatList.GetHeadPosition();
            POS posPrevious;
            while (pos)
            {
                posPrevious = pos;
                ThreatListEntry & tThreatListEntry = ptTargetActorAI->tThreatList.GetNext(pos);
                // Inference: the found flag and early exit preserve the original first-match update path.
                if (tThreatListEntry.idACD == tDamage.idACDSource)
                {
                    tThreatListEntry.flDamageReceived += flTotalDamage;
                    bFound = TRUE;
                    break;
                }
                if (!ACDTryToGet(tThreatListEntry.idACD))
                {
                    ptTargetActorAI->tThreatList.RemoveAt(posPrevious);
                }
            }

            if (!bFound)
            {
                ThreatListEntry tNewThreatListEntry;
                tNewThreatListEntry.idACD = idACDThreatSource;
                tNewThreatListEntry.flDamageReceived = flTotalDamage;
                ptTargetActorAI->tThreatList.AddTail_Tracked(tNewThreatListEntry);
            }
        }
    }
    // Inference: ACDGetOwner DataID converts to the DWARF ActorID local.
    ActorID idLastDamageMainActor = (ActorID)ACDGetOwner(tDamage.idACDMainActor);




    if (idLastDamageMainActor != ACTORID_INVALID)
    {




        tACDTarget.AttributesSetInt(ATTRIBUTE_LAST_DAMAGE_MAINACTOR, idLastDamageMainActor);
        tACDTarget.AttributesSetInt(ATTRIBUTE_LAST_DAMAGE_ACD, ANNFromACD(ActorGet(idLastDamageMainActor)->idACD));
    }
    else
    { // Inference: the last-damage attribute stores the DWARF ActorID consumed below.
        idLastDamageMainActor = (ActorID)tACDTarget.AttributesGetInt(ATTRIBUTE_LAST_DAMAGE_MAINACTOR);


        if (!ActorTryToGet(idLastDamageMainActor))
        {
            idLastDamageMainActor = ACTORID_INVALID;
            tACDTarget.AttributesSetInt(ATTRIBUTE_LAST_DAMAGE_MAINACTOR, ACTORID_INVALID);
            tACDTarget.AttributesSetInt(ATTRIBUTE_LAST_DAMAGE_ACD, ACTORID_INVALID);
        }
    }

    if (tDamage.eDeliveryMechanism != PDM_THORNS)
    {
        BrainComponentWasAttacked(idTargetActor, idLastDamageMainActor);
    }
    // Inference: the facing flag begins clear and is set only by source-facing knockback below.
    BOOL bIsEventWithFacing = FALSE;

    if (!tDamage.bDamageWasBlocked && !tDamage.bDisableHitEffects && tDamage.eDeliveryMechanism != PDM_DOT)
    {
        if (tDamage.eHitsound != 0)
        {
            if (tDamage.bWasImmune)
            {
                ACDPlayEffect(tACDTarget.id, EFFECT_PLAY_HITSOUND, HITSOUND_IMMUNE, ACDID_INVALID);
            }
            else
            {
                ACDPlayEffect(tACDTarget.id, EFFECT_PLAY_HITSOUND, tDamage.eHitsound, ACDID_INVALID);
            }
        }
        if (!tDamage.bWasImmune)
        {


            ACDPlayEffect(tACDTarget.id, EFFECT_HIT, 0, ACDID_INVALID);
        }
    }

    if (ACDIsPlayer(tACDTarget.id))
    {


        if (!tDamage.bDamageWasBlocked)
        {
            ACDPlayEffectForPlayer(tACDTarget.id, tACDTarget.id, EFFECT_HIT_LOCAL, 0);
        }




        if (STalkIsTalking(tACDTarget.id))
        {
            STalkForceDoneTalking(tACDTarget.id);
        }
    }

    ScriptEventOnActorDamaged(tACDTarget.id, tDamage.idACDSource);
    if (tDamage.bCriticalHit)
    {
        ScriptEventOnActorScoredCriticalHit(tDamage.idACDSource, tACDTarget.id);
    }
    // Inference: the event flags begin clear, matching the shared zero-setting tails.
    BOOL bPlayGetHitAnim = FALSE;
    BOOL bPlayKnockBack = FALSE;
    BOOL bPlayKnockDown = FALSE;

    BOOL bDied = sBrainOnDamageCallCheckForDeath(idTargetActor) && !tACDTarget.AttributesGetInt(ATTRIBUTE_CANNOT_DIE_DURING);
    if (bDied)
    {
    }
    // Inference: dead actors skip every non-death animation event in this conditional chain.
    else if (tDamage.eKnockBack == KNOCKBACK_FROM_ATTACKER &&
        PowerActorCanBeKnockedBack(idTargetActor, tDamage.idPower) &&
        !tACDTarget.AttributesGetInt(ATTRIBUTE_IMMUNE_TO_KNOCKBACK))
    {
        bPlayKnockBack = TRUE;
        bIsEventWithFacing = TRUE;
    }
    else if (tDamage.eKnockBack == KNOCKBACK_DIRECTION &&
        PowerActorCanBeKnockedBack(idTargetActor, tDamage.idPower) &&
        !tACDTarget.AttributesGetInt(ATTRIBUTE_IMMUNE_TO_KNOCKBACK))
    {
        bPlayKnockBack = TRUE;
    }
    else if (tDamage.eKnockBack == KNOCKBACK_FROM_PLACE &&
        PowerActorCanBeKnockedBack(idTargetActor, tDamage.idPower) &&
        !tACDTarget.AttributesGetInt(ATTRIBUTE_IMMUNE_TO_KNOCKBACK))
    {
        bPlayKnockBack = TRUE;
    }
    else if (tDamage.bKnockdown &&
        PowerActorCanBeKnockedBack(idTargetActor, tDamage.idPower) &&
        !tACDTarget.AttributesGetInt(ATTRIBUTE_IMMUNE_TO_KNOCKBACK))
    {
        bPlayKnockDown = TRUE;
    }
    else if (tACDTarget.IsMonster())
    {

        if (tDamage.eDeliveryMechanism != PDM_DOT)
        {




            bPlayGetHitAnim = sBrainStanceRunGetHitLogic(tDamage);

            // Inference: recovered dead flag tracks the original search ending at the first hit-point entry.
            BOOL fFoundGetHitDamage = FALSE;
            POS pos = tDamage.listDamage.GetHeadPosition();
            while (pos)
            {
                DamageAttribute & tDamageAttribute = tDamage.listDamage.GetNext(pos);
                if (KeyGetAttrib(tDamageAttribute.tKey) == ATTRIBUTE_HITPOINTS_CUR)
                {
                    fFoundGetHitDamage = TRUE;
                    break;
                }
            }
            // Inference: debug bit 25 forces get-hit; original loads this bit after the retained search.
            if (sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_ALWAYS_PLAY_GETHIT_BIT))
            {
                bPlayGetHitAnim = TRUE;
            }
        }
    }
    else
    {

        if (tDamage.eDeliveryMechanism != PDM_DOT)
        {




            bPlayGetHitAnim = sCheckRecoil(tDamage, idTargetActor, tACDTarget.id);





            // Inference: the original retained the attribute search although its found flag is dead.
            BOOL fFoundGetHitDamage = FALSE;
            POS pos = tDamage.listDamage.GetHeadPosition();
            while (pos)
            {
                DamageAttribute & tDamageAttribute = tDamage.listDamage.GetNext(pos);
                if (KeyGetAttrib(tDamageAttribute.tKey) == ATTRIBUTE_GETHIT_CURRENT)
                {
                    fFoundGetHitDamage = TRUE;
                    break;
                }
            }
            // Inference: bit 25 forces get-hit after the recoil result, as in the monster branch.
            if (sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_ALWAYS_PLAY_GETHIT_BIT))
            {
                bPlayGetHitAnim = TRUE;
            }
        }
    }

    // Inference: disable source-facing events for brainless, wreckable, skill-less, or stunned targets.
    ActorBrain eBrainType = ActorGetBrainType(idTargetActor);
    BOOL fWreckableObject = tACDTarget.IsWreckableObject();
    if (eBrainType == ACTORBRAIN_DO_NOTHING || fWreckableObject ||

        !sActorHasSkills(tACDTarget.id) ||
        tACDTarget.IsStunned())
    {
        bIsEventWithFacing = FALSE;
    }


    if (bIsEventWithFacing)
    {
        if (ptACDDamageSource)
        {
            ActorMovement * ptMove = tACDTarget.GetActorMovement();
            if (ptMove)
            {
                WorldPlace placeTarget = ptACDDamageSource->GetPlace();
                ptMove->FaceTarget(placeTarget.wp, FALSE);
            }
        }
    }
    // Inference: the ordinary animation is the default selected by the original conditional chain.
    uint32 dwGetHitAnimTag = TAG_ACTOR_ANIMATION_GETHIT;
    if (tDamage.bCausesTransform)
    {
        dwGetHitAnimTag = TAG_ACTOR_ANIMATION_TRANSFORM_01;
        bPlayGetHitAnim = TRUE;
    }

    else if (bPlayKnockDown && AnimPlayerAnimExists(tACDTarget, TAG_ACTOR_ANIMATION_SUPER_KNOCK_BACK_END))
    {
        dwGetHitAnimTag = TAG_ACTOR_ANIMATION_SUPER_KNOCK_BACK_END;
    }
    else if (tDamage.bCriticalHit && AnimPlayerAnimExists(tACDTarget, TAG_ACTOR_ANIMATION_GETHIT_CRITICAL))
    {
        dwGetHitAnimTag = TAG_ACTOR_ANIMATION_GETHIT_CRITICAL;
    }
    else if (bPlayKnockBack && AnimPlayerAnimExists(tACDTarget, TAG_ACTOR_ANIMATION_KNOCK_BACK))
    {
        dwGetHitAnimTag = TAG_ACTOR_ANIMATION_KNOCK_BACK;
    }
    // Inference: the original compares GameGetGameType() directly to GameTypeIsPvP(); retain that comparison.
    if (fWreckableObject ||
        (ACDIsPlayer(tACDTarget.id) && GameGetGameType() == GameTypeIsPvP()))
    {
        bPlayGetHitAnim = FALSE;
    }

    ActorMovement * ptMove = tACDTarget.GetActorMovement();


    if (ptMove && ptMove->IsInArcingMovement())
    {
        bPlayGetHitAnim = FALSE;
    }




    if (bPlayKnockBack || bPlayKnockDown || bPlayGetHitAnim)
    {
        PowerID idPrimaryPower = ActorPowersGetPrimary(idTargetActor);
        Power * ptPrimaryPower = PowerTryToGet(idPrimaryPower);
        if (ptPrimaryPower && PowerIsChannelled(tACDTarget, AutoPowerDefinitionR(ptPrimaryPower->snoPower)))
        {
            PowerAbortChannelled(idACDTarget, ptPrimaryPower->snoPower);
        }
        ACDPlayEffect(idACDTarget, EFFECT_INTERRUPTED, 0, ACDID_INVALID);
    }

    sProcessDamageState(tACDTarget);
    // Inference: missing powers leave SNO_NONE, from original null-power branch.
    SNO snoPower = SNO_NONE;
    Power * ptPower = PowerTryToGet(tDamage.idPower);
    if (ptPower)
    {
        snoPower = ptPower->snoPower;
    }

    if (bDied)
    {

        SNO snoDeathReplacementPower = tACDTarget.AttributesGetInt(ATTRIBUTE_DEATH_REPLACEMENT_POWER_SNO);
        if (snoDeathReplacementPower != SNO_NONE)
        { // Inference: ACDGetOwner DataID supplies the DWARF ActorID killer parameter below.
            BrainStanceDoDeathReplacement(idTargetActor, (ActorID)ACDGetOwner(tDamage.idACDSource));
        }
        else
        {
            BrainAPIDoDeath(idTargetActor, idLastDamageMainActor, FALSE, FALSE, FALSE, &tDamage, pflDamageDone);
        }
    }
    else if (bPlayKnockBack)
    {

        Vector3D wvKnockbackDirection;
        if (tDamage.eKnockBack == KNOCKBACK_DIRECTION)
        {
            wvKnockbackDirection = tDamage.wvKnockbackDirection;
        }
        else if (tDamage.eKnockBack == KNOCKBACK_FROM_PLACE)
        {
            wvKnockbackDirection = tACDTarget.GetWorldLocation().transform.wp - tDamage.tKnockbackPlace.wp;
        }
        else
        { // Inference: an absent source leaves the default direction, as in the original branch.
            if (ptACDDamageSource)
            {

                wvKnockbackDirection = tACDTarget.GetWorldLocation().transform.wp - ptACDDamageSource->GetWorldLocation().transform.wp;
            }
        }



        // Inference: zero-length knockback leaves zero yaw, from original conditional tail.
        Angle aYaw = 0.0f;
        if (wvKnockbackDirection.MagnitudeSquared() > EPSILON * EPSILON)
        {

            Vector3D wvXYPlanarKnockbackDirection(wvKnockbackDirection);
            wvXYPlanarKnockbackDirection.z = 0.0f;
            aYaw = Vector3D::AngleBetween(cgwvXAxis, wvXYPlanarKnockbackDirection) + PI;
        }
        // Inference: super-knockback is requested only when both tag and animation exist.
        BOOL bDoSuperKnockback = FALSE;
        if (PowerContactTagGetBool(tDamage.idPower, TAG_POWER_CAUSES_SUPER_KNOCKBACK))
        {
            bDoSuperKnockback = AnimPlayerAnimExists(tACDTarget, TAG_ACTOR_ANIMATION_SUPER_KNOCK_BACK);
        }



        BrainStanceDoKnockBack(tDamage.idPower, idTargetActor, aYaw, tDamage.wdKnockbackMagnitude, tDamage.flKnockbackHeight, tDamage.flKnockbackGravity,
            dwGetHitAnimTag, tDamage.bKnockbackIgnoresWeight, tDamage.snoKnockbackPower, bDoSuperKnockback, tDamage.bContinuousKnockbackPower);

        PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(tDamage.idPower, tDamage.idACDSource, tDamage.idACDTarget, snoPower, CROWD_CONTROL_KNOCKBACK, 0, 1.0f, FALSE);
    }
    else if (bPlayKnockDown)
    {
        BrainStanceDoKnockDown(idTargetActor, dwGetHitAnimTag);
    }


    else if (bPlayGetHitAnim)
    {


        // Inference: original constants 5 and 19 imply maximum 24, minimum 5 and their difference.
        static const float GETHIT_TICKS_MAX = 24.0f;
        static const float GETHIT_TICKS_MIN = 5.0f;
        static const float GETHIT_TICKS_DELTA = GETHIT_TICKS_MAX - GETHIT_TICKS_MIN;

        float flUnitCCReduction = tACDTarget.AttributesGetFloat(ATTRIBUTE_CROWD_CONTROL_REDUCTION);
        flUnitCCReduction = Math::Pin(flUnitCCReduction, 0.0f, 1.0f);
        float flTicks = (1.0f - flUnitCCReduction) * GETHIT_TICKS_DELTA;
        flTicks += GETHIT_TICKS_MIN;

        uint32 nPlayerGetHitAnimTime = Math::FloatToInt(flTicks);
        uint32 nAnimLength = (uint32)-1;
        if (ACDIsPlayer(idACDTarget))
        {
            nAnimLength = nPlayerGetHitAnimTime;
        }

        sBrainStancePlayAnimation(idTargetActor, dwGetHitAnimTag, STANCE_GETHIT, nAnimLength, 0.0f, FALSE, 1.0f);

        BrainConditionSetCondition(idTargetActor, BRAIN_CONDITION_ABORTED);
    }



    if (tDamage.eKnockBack != KNOCKBACK_NONE || tDamage.bKnockdown)
    {
        PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tDamage.idPower, tDamage.idACDSource, tDamage.idACDTarget, snoPower, CROWD_CONTROL_KNOCKBACK, 0, 1.0f, FALSE);
    }
}



















































































































































































































































































































































































































#include "../../Common/TagMaps/AnimTags.h"
#include "../../Common/Powers/SpellEffectCommon.h"
#include "../Timers.h"
// DWARF external data defined in X/Common/TagMaps/AnimTags.cpp:632.
extern const DeathAnimPair sgtDeathAnimTable[];
// DWARF X/Common/Powers/PowersCommon.cpp:498.
SpecialDeathType PowerGetSpecialDeathType(AutoPowerDefinitionR pdefPower, RuneType eRune);
// DWARF X/Server/ActorIsAffected.cpp:351.
BOOL PowerActorCanDie(ActorID idActor);


static int32 sGetDeathType(
    const ActorCommonData & tACD,
    const PowerAPIDamage * ptDamage,
    const float * pflKillingBlowDamage,
    const BOOL fUseSpecialSkillDeath)
{
    int32 nDeathTypeOverride = tACD.AttributesGetInt(ATTRIBUTE_DEATH_TYPE_OVERRIDE);
    if (nDeathTypeOverride != -1)
        return nDeathTypeOverride;



    if (tACD.AttributesGetInt(ATTRIBUTE_HAS_SPECIAL_DEATH_ANIMTAG) == TRUE)
        return DEATH_SPECIAL;











    if (ptDamage && fUseSpecialSkillDeath)
    {


        RuneType ePowerDmgSourceRune = RUNE_TYPE_INVALID;
        SNO snoPowerDmgSource = PowerAPI_GetDamageSourcePowerSNO(ptDamage, &ePowerDmgSourceRune);
        SpecialDeathType eSpecialDeath = PowerGetSpecialDeathType(AutoPowerDefinitionR(snoPowerDmgSource), ePowerDmgSourceRune);
        switch (eSpecialDeath)
        {
        case SPECIALDEATH_LAVA: return DEATH_LAVA;
        case SPECIALDEATH_DISINTEGRATE: return DEATH_DISINTEGRATE;
        case SPECIALDEATH_PULVERIZE: return DEATH_PULVERIZE;
        case SPECIALDEATH_PLAGUED: return DEATH_PLAGUED;
        case SPECIALDEATH_DISMEMBER: return DEATH_DISMEMBER;
        case SPECIALDEATH_DECAPITATION: return DEATH_DECAPITATION;
        case SPECIALDEATH_ACID: return DEATH_ACID;
        case SPECIALDEATH_SACRIFICE: return DEATH_SACRIFICE;
        case SPECIALDEATH_SPIRIT: return DEATH_SPIRIT;
        }
    }





    if (pflKillingBlowDamage)
    {

        DamageType ePredominantDamage = GetPredominantDamageType(pflKillingBlowDamage, FALSE);
        switch (ePredominantDamage)
        {
        case DAMAGE_TYPE_FIRE: return DEATH_FIRE;
        case DAMAGE_TYPE_LIGHTNING: return DEATH_LIGHTNING;
        case DAMAGE_TYPE_COLD: return DEATH_COLD;
        case DAMAGE_TYPE_POISON: return DEATH_POISON;
        case DAMAGE_TYPE_ARCANE: return DEATH_ARCANE;
        case DAMAGE_TYPE_HOLY: return DEATH_HOLY;
        }
    }
    // Inference: the original's shared fallback selects the first normal death.
    return DEATH_01;
}
















static void sSelectProperDeathAnim(
    DeathAnimPair & tAnimPair,
    ActorCommonData & tACD,
    const PowerAPIDamage * ptDamage,
    const float * pflKillingBlowDamage,
    const BOOL fUseSpecialSkillDeath,
    EffectType & eDeathEffect)
{
    int32 nDeathType = sGetDeathType(tACD, ptDamage, pflKillingBlowDamage, fUseSpecialSkillDeath);


    if (nDeathType == DEATH_01 ||
        !AnimPlayerAnimExists(tACD, sgtDeathAnimTable[nDeathType].dwDeathAnimTag))
    {
        // Inference: the first normal death exists; count subsequent consecutive variants.
        int32 nNumDeathAnims = 1;
        for (int32 i = 1; i < NUM_NORMAL_DEATH_VARIATIONS; ++i)
        {
            if (!AnimPlayerAnimExists(tACD, sgtDeathAnimTable[i].dwDeathAnimTag))
                break;
            ++nNumDeathAnims;
        }




        nDeathType = RandRange_Internal(SGameGlobalsGet().rng, 0, nNumDeathAnims - 1);
    }

    if ((nDeathType == DEATH_DISMEMBER || nDeathType == DEATH_DECAPITATION) &&
        PowerAPI_BuffHasLookswitchBuff(tACD))
        nDeathType = DEATH_01;




    tAnimPair = sgtDeathAnimTable[nDeathType];

    switch (nDeathType)
    {
    case DEATH_FIRE: eDeathEffect = EFFECT_DEATH_FIRE; break;
    case DEATH_COLD: eDeathEffect = EFFECT_DEATH_COLD; break;
    case DEATH_LIGHTNING: eDeathEffect = EFFECT_DEATH_LIGHTNING; break;
    case DEATH_ARCANE: eDeathEffect = EFFECT_DEATH_ARCANE; break;
    case DEATH_HOLY: eDeathEffect = EFFECT_DEATH_HOLY; break;
    case DEATH_POISON: eDeathEffect = EFFECT_DEATH_POISON; break;
    case DEATH_LAVA: eDeathEffect = EFFECT_DEATH_LAVA; break;
    case DEATH_DISINTEGRATE: eDeathEffect = EFFECT_DEATH_DISINTEGRATE; break;
    case DEATH_PULVERIZE: eDeathEffect = EFFECT_DEATH_PULVERIZE; break;
    case DEATH_PLAGUED: eDeathEffect = EFFECT_DEATH_PLAGUED; break;
    case DEATH_SPIRIT: eDeathEffect = EFFECT_DEATH_SPIRIT; break;
    }
}

















































































void BrainStanceDoDeath(
    ActorID idActor,
    const PowerAPIDamage * ptDamage,
    const float * pflKillingBlowDamage,
    const BOOL fDeathReplacementEffect,
    const BOOL fUseSpecialSkillDeath,
    EffectType & eDeathEffect)
{
    if (!PowerActorCanDie(idActor))
        return;



    ACDID idACD = ActorGet(idActor)->idACD;
    ActorCommonData & tACD = ACDGet(idACD); // Inference: absent animation playback leaves a zero duration.
    uint32 dwPlayLength = 0;

    if (tACD.AttributesGetInt(ATTRIBUTE_FROZEN))
    {
        tACD.AttributesSetInt(ATTRIBUTE_FROZEN, FALSE);
    }

    sBrainStancePrepAnimation(idActor, STANCE_DYING);



    if (fDeathReplacementEffect)
    { // Inference: replacement effects leave the neutral idle animation.
        uint32 dwAnimTag = TAG_ACTOR_ANIMATION_NEUTRAL;
        tACD.AttributesSetInt(ATTRIBUTE_DEAD_BODY_ANIMTAG, dwAnimTag);
        AnimPlayerSetIdleAnimation(tACD, dwAnimTag);
    }
    else
    {
        DeathAnimPair tAnimPair;
        sSelectProperDeathAnim(tAnimPair, tACD, ptDamage, pflKillingBlowDamage, fUseSpecialSkillDeath, eDeathEffect);

        tACD.AttributesSetInt(ATTRIBUTE_COULD_HAVE_RAGDOLLED, FALSE);
        AnimationTiming tAnimationTiming[3];
        int32 nNumAnims = 0;


        float flSpeedScalar = tACD.AttributesGetFloat(ATTRIBUTE_SLOWTIME_DEBUFF); // Inference: nonpositive speed falls back to normal animation speed.
        if (flSpeedScalar <= 0.0f)
            flSpeedScalar = 1.0f;

        if (AnimPlayerGetAnimationTiming(tACD, tAnimPair.dwDeathAnimTag, -1, flSpeedScalar, tAnimationTiming[nNumAnims]))
            ++nNumAnims;



        if (AnimPlayerGetAnimationTiming(tACD, TAG_ACTOR_ANIMATION_DEAD_INTRO, -1, flSpeedScalar, tAnimationTiming[nNumAnims]))
        {
            ++nNumAnims;
        }

        if (nNumAnims)
        {
            dwPlayLength = AnimPlayerPlayAnimationsWithTiming(tACD, tAnimationTiming, nNumAnims, TRUE, ANIM_REASON_DEATH, 0, FALSE);

            if (tACD.IsWreckableObject())
            {
                uint32 dwDelay = tAnimationTiming[0].dwExecuteTime[0];
                TimerRegister(tACD.id, TIMER_OWNER_ACD, dwDelay, &tACD, sRemoveCollision);
            }
        }




        BOOL fRagDolled = tACD.AttributesGetInt(ATTRIBUTE_COULD_HAVE_RAGDOLLED);
        if (!fRagDolled)
        {
            uint32 uDeadAnimTag = tAnimPair.dwDeadAnimTag; // Inference: absent selected dead animation falls back to DEAD_01.
            if (!AnimPlayerAnimExists(tACD, uDeadAnimTag))
                uDeadAnimTag = TAG_ACTOR_ANIMATION_DEAD_01;


            tACD.AttributesSetInt(ATTRIBUTE_DEAD_BODY_ANIMTAG, uDeadAnimTag);
            AnimPlayerSetIdleAnimation(tACD, uDeadAnimTag);
        }
    }
    // Inference: the death stance timer has a minimum of one tick.
    if (!dwPlayLength)
        dwPlayLength = 1;
    sBrainStanceSetTimer(idActor, dwPlayLength);

}










































































































































void BrainStanceDoKnockBack(
    PowerID idPower,
    ActorID idActor,
    Angle aYaw,
    WorldDistance wdKnockbackMagnitude,
    float flHeight,
    float flGravity,
    uint32 dwAnimTag,
    BOOL bKnockbackIgnoresWeight,
    SNO snoKnockbackPower,
    BOOL bSuperKnockback,
    BOOL bContinuousKnockbackPower)
{



    if (!PowerActorCanBeKnockedBack(idActor, idPower))
    {
        return;
    }

    ACDID idACD = ActorGet(idActor)->idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    const ActorMovement * ptMove = tACD.GetActorMovement();
    if (!ptMove)
    {
        return;
    }

    ACDPlayEffect(tACD.id, EFFECT_KNOCK_BACK, 0, ACDID_INVALID);

    if (snoKnockbackPower == SNO_NONE)
    {
        snoKnockbackPower = GlobalSNOGet(GLOBALSNO_POWERS_KNOCKBACK);
    }


    WorldPlace placeActor = tACD.GetPlace();

    aYaw = Math::NormalizeAngle(aYaw + PI);


    WorldDistance wdDistance = wdKnockbackMagnitude;

    if (!bKnockbackIgnoresWeight)
    {
        float flKnockback = tACD.AttributesGetFloat(ATTRIBUTE_KNOCKBACK_WEIGHT);
        wdDistance *= flKnockback;
    }

    Vector3D wvOffset = cgwvXAxis * wdDistance;
    Vector3D::Rotate(cgwvZAxis, aYaw, wvOffset);

    WorldPlace placeDest;
    WorldPlaceOffset(placeActor, wvOffset, placeDest);


    PowerSetCoolDown(tACD, 0);
    // Inference: the original passes TRUE and the literal destination clip distance 10.0f.
    PowerComputeDestination(snoKnockbackPower, *ptMove, placeDest, placeDest, TRUE, 10.0f);

    Target tKnockbackTarget;
    tKnockbackTarget.InitializeFromPlace(placeDest);

    uint32 dwFailureReasons = PowerACDCanStartPowerFailureReasons(tACD, AutoPowerDefinitionR(snoKnockbackPower), &tKnockbackTarget, FALSE, FALSE);

    if (dwFailureReasons == 0)
    {
        if (GameTypeIsPvP())
        {
            Power * ptPower = PowerTryToGet(idPower);
            if (ACDIsPlayer(tACD.id) && ptPower)
            {

                Player * ptPlayerAttacker = ACDGetControllingPlayer(ptPower->idACD);
                if (ptPlayerAttacker)
                {



                    {
                        Achievement::EvaluationData tEvaluationData;
                        tEvaluationData.SetPlayer(ptPlayerAttacker);
                        tEvaluationData.SetPower(ptPower->snoPower);
                        tEvaluationData.EvaluateEvent(ptPlayerAttacker, Achievement::ACH_EVENT_PVP_PLAYER_APPLIED_KNOCKBACK_TO_PLAYER, ptPower->snoPower);
                    }

                    SGameStatsPlayerCounterIncrement(ptPlayerAttacker, STAT_COUNTER_PLAYER_APPLIED_KNOCKBACK_TO_PLAYER, 0, 0, 1.0f);
                }
            }
        }

        ActorPowersInterruptPrimary(idActor, TRUE);
        BrainAPIDoIdle(idActor);
        KnockbackPowerData tPowerData;
        tPowerData.flHeight = flHeight;
        tPowerData.flGravity = flGravity;
        BrainStanceDoPower(idActor, tKnockbackTarget, snoKnockbackPower, BRAIN_POWER_START, &tPowerData, NULL, 0, 0);


        PlayerBrainClearQueuedPower(idACD);


        float flTicks;
        if (Math::Abs(flGravity) < EPSILON)
        {
            // Inference: zero-gravity knockback uses the original 60-tick duration.
            flTicks = 60.0f;
        }
        else
        {



            flTicks = ActorMovement::ComputeFallingTicksToHitTarget(flGravity, ptMove->GetVelocity().Magnitude(), wdDistance);
            SNO snoKnockbackLand = tACD.GetSpecificAnim(TAG_ACTOR_ANIMATION_KNOCK_BACK_LAND);
            if (snoKnockbackLand != SNO_NONE)
            {
                AnimationTiming tAnimationTiming;
                if (AnimPlayerGetAnimationTiming(tACD, TAG_ACTOR_ANIMATION_KNOCK_BACK_LAND, (uint32)-1, 1.0f, tAnimationTiming))
                {
                    flTicks += tAnimationTiming.dwExecuteTime[0];
                }
            }
        }
        float flCurrentCCResistance = tACD.AttributesGetFloat(ATTRIBUTE_CROWD_CONTROL_RESISTANCE);

        float flAdditionalCCResistance;
        if (bContinuousKnockbackPower)
        {

            // Inference: resistance threshold 0.65 and increments 0.2/0.05 are original literals.
            if (flCurrentCCResistance >= 0.65f)
            {
                flAdditionalCCResistance = 0.05f;
            }
            else
            {
                flAdditionalCCResistance = 0.2f;
            }
        }
        else
        {
            // Inference: ordinary knockback adds the original literal 0.4 resistance.
            flAdditionalCCResistance = 0.4f;
        }
        tACD.AttributesSetFloat(ATTRIBUTE_CROWD_CONTROL_RESISTANCE, flCurrentCCResistance + flAdditionalCCResistance);

        if (idPower != -1 && bContinuousKnockbackPower)
        {
            tACD.AttributesSetInt(ATTRIBUTE_LAST_CONTINUOUS_KNOCKBACK_POWER_ID, idPower);
        }

        uint32 dwNow = GameGetServerTime();
        tACD.AttributesSetInt(ATTRIBUTE_CROWD_CONTROL_PROJECTED_END_TIME, dwNow + (int32)flTicks);
    }
}
