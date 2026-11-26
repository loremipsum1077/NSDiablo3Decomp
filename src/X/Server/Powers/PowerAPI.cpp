#include "PowerAPI.h"

#include "../SGameManager.h"
#include "../../Common/Player.h"
#include "../../Common/ActorCommonDataSwig.h"
#include "../../Sigma/DataStructures/ChainedDataArrayTemplated.h"
#include "../../Common/Monsters.h"
#include "../../Common/Achievements/AchievementDefines.h"
#include "../Actors/Actors.h"
#include "PowerHelper.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Sigma/MemoryPools.h"
#include <float.h>
#include "../SGame.h"
#include "../SGameConstants.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/GameConstants.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../PlayerList.h"
#include "../AI/BrainComponents.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../Movement/SActorMovement.h"
#include "../Timers.h"
#include "../Actors/ActorEffect.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "PowerTrigger.h"
#include "../SGameStats.h"
#include "../SGameStatsSwig.h"

struct DamageShieldPriorityEntry {  // size 0xc
    DamageShieldPriorityEntry();
    BuffID idBuff;  // 0x00
    int32 nPriority;  // 0x04
    int32 nTicksLeft;  // 0x08
};

#include "../Achievements/AchievementModifierEvaluationLogic.h"
namespace Achievement {
typedef int32 EventID;















}  // namespace Achievement

// Declared here from their definitions elsewhere (headers not adopted yet).
AttribGroupID FastAttribLightCreate(FastAttribType eType);  // X/Common/Attributes/FastAttribute.cpp:4571
BOOL PowerContactTagGetBool(PowerID idPower, uint32 dwTag);  // X/Server/Powers/Powers.cpp:4208
float PowerGetCostReductionPercent(const ActorCommonData & tACD, const ResourceType eResourceType, const DamageType eDamageType, const SNO snoPower);  // X/Common/Powers/PowersCommon.cpp:4090
AttribGroupID PowerGetAttributeGroup(const PowerID idPower);  // X/Server/Powers/Powers.cpp:1164
float PowerGetResourceCost(PowerID idPower);  // X/Server/Powers/Powers.cpp:1941
void SPlayerRegisterPowerForRecharge(Player * ptPlayer, SNO snoPower);  // X/Server/SPlayer.cpp:7045

BOOL HeroIsAwareOfPlayer(ACDID idACDPlayer, ACDID idACDTarget);  // X/Common/Hero.cpp:118
ActorID SceneryAdd(const ActorAddStruct& tActorAddOriginal);  // X/Server/Actors/Actors.cpp:5865
BOOL ProjectileReflect(ACDID idACDProjectile, ACDID idACDReflector, BOOL bReflectYawOnly, Angle aYawOffset, Team eTeam, float flSpeedMult);  // X/Server/Actors/Projectile.cpp:1127
void BrainComponentWasAttacked(ActorID idTargetActor, ActorID idSourceActor);  // X/Server/AI/BrainComponents.cpp:7722
void BrainStanceReset(ActorID idActor);  // X/Server/AI/BrainStance.h
namespace CombatLog { typedef int32 LogEventType; void LogEvent(LogEventType eType, ACDID idACD1, ACDID idACD2, SNO snoData, float flParam1, float flParam2, uint32 dwParam1); }  // X/Common/CombatLog.cpp:610
void PowerHelper_DamageAdd(PowerAPIDamage & tDamageInfo, FastAttribKey tKey, float flAmount, int32 nDuration);  // PowerHelper.cpp:550
void PowerAPI_DoDamage(PowerAPIDamage & tDamage, BOOL fForceAllowDamage, float * pflDamageDone);  // PowerAPI.cpp:453
void ActorSetPowerState(ActorID idActor, LPCSTR szState);
CRefString PowerTrace(SNO snoPower);
void PowerTriggerPlayTriggerAnimation(PowerID idPower, uint32 dwAnimationTag, uint32 dwDuration, float flSpeedScalar, BOOL bSetAimTarget, const PreplayedAnimData & tPreplayedAnimData, AnimReason eAnimReason, BOOL bParam8, BOOL bParam9);
BOOL PowerBuffIsDisplayed(AutoPowerDefinitionR pdefPower, int32 nBuffIndex);  // X/Common/Powers/PowersCommon.cpp
BOOL PowerGetBuffAttributes(int32 nBuffIndex, Attrib & eAttribStart, Attrib & eAttribEnd, Attrib & eAttribCount);  // X/Common/Powers/PowersCommon.cpp:5977
void FastAttribSetValueToDefault(const AttribGroupID idAttribGroup, const FastAttribKey tKey);  // X/Common/Attributes/FastAttribute.cpp
BOOL PowerHasEscapeRange(const Power & tPower);  // X/Server/Powers/Powers.cpp
float PowerGetRange(PowerID idPower, BOOL bUseBonusRange);  // X/Server/Powers/Powers.cpp
void PowerHelper_CostsInit(PowerID idPower, PowerAPICosts & tCosts);  // PowerHelper.cpp:568
void PowerAPI_BuffApplyOnCostEffects(ACDID idACD, PowerID idPowerFrom, SNO snoPower, float flResourceCost, ResourceType eResourceType, float flHealthCost, BOOL bCostsWereCharged, float flHealthCostUndiscounted);  // PowerAPI.cpp:12184
ResourceType PowerGetResourceType(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp:5444
static void sApplyThorns(float flThornsPercent, float * pflDamageDone, ActorCommonData & tACDTarget, ActorCommonData & tACDAttacker, Power * ptPower, BOOL bOnFirstHit);

enum { FLOATING_NUMBER_TYPE_BROKE_ROOT = 24, FLOATING_NUMBER_TYPE_BROKE_SNARE = 25, FLOATING_NUMBER_TYPE_BROKE_FEAR = 26, FLOATING_NUMBER_TYPE_IMMUNE = 27 };






void PowerGetNavPlaceFromACD(const ActorCommonData * ptACD, NavPlace & tNavPlace);  // X/Server/Powers/PowerHelper.cpp

// Declared here from their definitions elsewhere (headers not adopted yet); used by PowerAPI_BuffAddToACD.
BOOL PowerActorCanBeKnockedBack(ActorID idActor, PowerID idPower);  // X/Server/ActorIsAffected.cpp:430
void ActorPowersApplyBuffInstanceCap(ActorID idActor, SNO snoPower, int32 nBuffIndex, int32 nCap);  // X/Server/Actors/ActorPowers.cpp:621
void SPlayerRefreshKillCounterTime(ActorCommonData * ptACDPlayer);  // X/Server/SPlayer.cpp:8857
void PowerHelper_ApplyDamageBonusMultipliers(const ActorCommonData & tACDTarget, SNO snoPower, DamageType eDamageType, float flAttackBonusMultiplier, float flClassBonusMultiplier, float flSkillsBonusMultiplier, float & flDamage);  // PowerHelper.cpp:451
void BrainAPIDoStun(ActorID idActor);  // X/Server/AI/BrainComponents.cpp:3988
void BrainAPIDoForceGrip(ActorID idActor);  // BrainComponents.cpp:4017
void BrainAPIDoFrozen(ActorID idActor);  // BrainComponents.cpp:3959
void BrainAPIDoBlind(ActorID idActor, uint32 dwDuration);  // BrainComponents.cpp:4046
void BrainAPIDoFeared(ActorID idActor, ACDID idACDFearSource);  // BrainComponents.cpp:3920
void BrainAPIDoTaunt(ActorID idActor, ACDID idACDTauntTarget, int32 nDuration, BOOL bSuppressTauntEffects);  // BrainComponents.cpp:4098
void BrainAPIDoCharm(ActorID idActor, int32);  // BrainComponents.cpp:4160
BOOL PowerBuffVisuallyAppliesToPets(AutoPowerDefinitionR pdefPower, int32 nBuffIndex);  // X/Common/Powers/PowersCommon.cpp:1569
void VisualInventoryUpdateItem(ActorCommonData & tACDItem);  // X/Common/VisualInventory.cpp:1236
void VisualInventoryResetAllSlots(const ACDID idACD);  // VisualInventory.cpp:632
BOOL AIStateOnHitReaction(const ACDID idACD);  // X/Server/AI/AIStates.cpp:5987
void AIStateMerge(ActorID idActor, SNO snoAIState, uint32 uLifetime, BuffID idBuff);  // AIStates.cpp:6207
BOOL ItemIsEquippedButDisabled(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:8690
AttribGroupID SItemGetAttributeGroup(const ActorCommonData & tACDItem);  // X/Server/SItems.cpp:5470
BOOL ItemIsBroken(const ActorCommonData & tACDItem);  // Items.cpp:8672
BOOL FastAttribHasChild(AttribGroupID idAttribGroupParent, AttribGroupID idAttribGroupChild);  // FastAttribute.cpp:4275
void FastAttribLightCopy(AttribGroupID idAttribGroupSource, AttribGroupID idAttribGroupDest);  // FastAttribute.cpp:4615
void FastAttribSetValueToDefault(FastAttribGroup * ptAttribGroup, FastAttribKey tKey);  // FastAttribute.cpp:4384
int32 PowerAttributesGetInt(const PowerID idPower, FastAttribKey tKey);  // PowerAttributes.cpp:29
float PowerAttributesGetFloat(PowerID idPower, FastAttribKey tKey);  // PowerAttributes.cpp:17
enum { FLOATING_NUMBER_TYPE_ROOT = 12, FLOATING_NUMBER_TYPE_STUN = 13, FLOATING_NUMBER_TYPE_BLIND = 14, FLOATING_NUMBER_TYPE_FROZEN = 15, FLOATING_NUMBER_TYPE_FEAR = 16, FLOATING_NUMBER_TYPE_CHARM = 17, FLOATING_NUMBER_TYPE_TAUNT = 18, FLOATING_NUMBER_TYPE_SNARE = 19, FLOATING_NUMBER_TYPE_ATTACK_SPEED_SLOW = 20 };  // X/Common/Player.h FloatingNumberType
static void sTriggerCCCallbacksOnRefresh(PowerAPIBuffInstance & tBuff);
static void sBuffPulseSpellEffect(PowerID, void * pData);
static void sBuffDoneCallback(TimerID idTimer, DataID id, void * lpData);
static void sApplyResistance(PowerID idPower, const ActorCommonData & tACDTarget, const DamageType eDamageType, float & flDamageDone);
static void sApplyMiscDamageReduction(const PowerID idPower, const ActorCommonData & tACDTarget, const PayloadDeliveryMechanism eDeliveryMechanism, float & flDamageDone, float & flDamageReductionTurnsIntoHealTotal, const DamageType eDamageType);
static void sApplyMiscDamageBonuses(const PowerID idPower, const ActorCommonData & tACDTarget, float & flDamageDone, const DamageType eDamageType, const PayloadDeliveryMechanism eDeliveryMechanism, BOOL bUseHighestElementalDamageBonus);
static BOOL sCheckForCrit(PowerID idPower, BOOL bCanCrit, float flPowerAdditionalChanceToCriticalHit, float flCritCap, const ActorCommonData & tACDTarget, const ActorCommonData * ptACDMainActor);
static void sPowerAPI_CheckForOnHitProcs(Power & tPower, ActorCommonData & tACDAttacker, ActorCommonData & tACDTarget, KnockbackStyle eKnockbackStyle, float flProcScalar, float * flDamageDone, float flDamageTotal, const PowerAPIAttackPayload * ptPayload, const PowerAPIBuffInstance * ptBuff, BOOL bCrit);
static void sUpdateAuras(ActorCommonData & tACD);
static void sApplyOnGetHitBuffs(PowerAPIDamage & tPowerDamage, float flHitpointDamage, ACDID idACDAttacker, ACDID idACDTarget);
static void sApplyOnHitBuffs(PowerAPIDamage & tPowerDamage, float flHitpointDamage, float * flDamageDone, ACDID idACDAttacker, ACDID idACDTarget, ACDID idACDFromPet);
static void sPowerAPI_CheckForOnCriticalProcs(const PowerAPIDamage & tDamage, ActorCommonData * ptACDAttacker, ActorCommonData & tACDTarget);
BOOL PowerAPI_BuffIsSnare(const PowerAPIBuffStartData & tData);
void PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(PowerID idPowerFrom, ACDID idACDAttacker, ACDID idACDTarget, SNO snoPower, CrowdControlType eCCType, uint32 dwDuration, float flProcScalar, BOOL bRefresh);  // PowerAPI.cpp:11722
void PowerAPI_BuffApplyOnHitWithCrowdControlEffects(PowerID idPowerFrom, ACDID idACDAttacker, ACDID idACDTarget, SNO snoPower, CrowdControlType eCCType, uint32 dwDuration, float flProcScalar, BOOL bRefresh);  // PowerAPI.cpp:11803







void PowerAPI_BuffRefreshDuration(BuffID idBuff);  // PowerAPI.cpp:11310
void PowerAPI_DoFirstHitEffects(ACDID idACDMainActor, ACDID idACDTarget, Power * ptPower);  // PowerAPI.cpp:13724
BOOL ActorIsUsingAIQueue(ActorID idActor);  // X/Server/AI/ActorAI.cpp:2187
typedef int32 ItemLabel;  // X/Common/Items.h
BOOL ItemHasLabel(const GBID gbidItem, const ItemLabel eItemLabel);  // X/Common/Items.cpp:2977
void ACDInventoryEquipmentOrSkillChanged(ActorCommonData & tACD);  // X/Common/ActorInventory.cpp:9033

uint32 PowerBuffGetLockoutTime(ActorCommonData & tACD, const PowerAPIBuffInstance * ptBuff);  // PowerAPI.cpp:12566
void PowerBuffSetLockoutTime(ActorCommonData & tACD, const PowerAPIBuffInstance * ptBuff, uint32 dwLockoutTime);  // PowerAPI.cpp:12614

void PowerAPI_BuffDealDOTDamage(PowerAPIBuffInstance & tBuff, uint32 uInterval);  // PowerAPI.cpp:12342
void BrainStanceForceUpdate(ActorID idActor);  // X/Server/AI/BrainStance.cpp:1334
void BrainAPIUndoBlind(ActorID idActor);  // X/Server/AI/BrainComponents.cpp:4089
void AIStateRemoveMergesFromBuff(ActorID idActor, BuffID idBuff);  // X/Server/AI/AIStates.cpp:6259
void PowerTriggerRemoveTimerTrigger(PowerID idPower, TimerID idTimer);  // PowerTrigger.cpp:703

void ActorTriggerAttributeUpdate(ActorID idActor);  // X/Server/Actors/Actors.cpp:3770

#include <limits.h>
#include "../../Sigma/BlzExt/BlzVectorHelper.h"
void ActorHeal(ACDID idACD, float flValue, HealType eHealType, SNO snoSource, ACDID idACDSource, BOOL bPlayEffect);  // X/Server/LuaScript/ScriptUtil.cpp:1837 (first parameter is VACDID, a typedef of ACDID)
// Declared here from their definitions elsewhere (headers not adopted yet); used by PowerAPI_DoDamage.
BOOL PowerACDCanTakeDamage(ActorCommonData & tACD);  // X/Server/ActorIsAffected.cpp:29
BOOL ACDIsPet(const ACDID idACD);  // X/Common/Player.cpp:1666
void BrainAPIBlocked(ACDID idACDAttacker, ACDID idACDDefender);  // X/Server/AI/BrainComponents.cpp:2961
void BrainUpCallOnDamage(PowerAPIDamage & tDamage, float * pflDamageDone);  // X/Server/AI/BrainComponents.cpp
void ActorAccumulateAreaDamage(ACDID idACDAttacker, ACDID idACDTarget, float flDamage);  // X/Server/Actors/Actors.cpp
void SGameStatsRecordDamage(ACDID idACDAttacker, ACDID idACDTarget, float flDamage, SNO snoPower, PowerID idPower, BOOL bCriticalHit, DamageType eDamageType, PayloadDeliveryMechanism eDeliveryMechanism);  // X/Server/SGameStats.cpp
static void sApplyOnDoDamageBuffs(PowerAPIDamage & tPowerDamage, float * flDamageDone);
static void sModifyReflectedDamage(Power * ptPower, float & flDamageDone);
static void sApplySplashDamage(const PowerAPIDamage & tDamage, const float * pflDamageDone, const ActorCommonData & tACDTarget);
static BOOL sApplyDamageShieldBuffs(ACDID idACDTarget, float * flDamageDone, PowerAPIDamage & tPowerDamage);
static BOOL sApplyStaggerDamage(ActorCommonData & tACDTarget, float * pflDamageDone);
static void sApplyLifeOrManaStealEffects(const PowerID idPower, ActorCommonData & tACDAttacker, ActorCommonData & tACDTarget, float & flDamageDone);


DataID TimerTryToGetOwner(const TimerID idTimer);  // X/Server/Timers.cpp:420
uint32 TimerGetTime(const TimerID idTimer);  // X/Server/Timers.cpp:458


#include "../../Common/TagMaps/Tags.h"
#include "../../Common/TagMaps/ActorTags.h"


void PowerHelper_AttackPayloadInit_Blank(PowerAPIAttackPayload & tPayload);  // X/Server/Powers/PowerHelper.cpp:55
PlayerClass PlayerGetPlayerClass(const ACDID idACD);  // X/Common/Player.cpp
float PowerGetClassBonusPercent(AttribGroupID idFastAttrib, PlayerClass eClass);  // X/Common/Powers/PowersCommon.cpp:1219
static void sApplyChill(const PowerID idPowerFrom, float flProcScalar, const int32 nChillDuration, const float flChillMagnitude, ActorCommonData & tACDTarget);
static void sApplyStun(const PowerID idPowerFrom, float flProcScalar, const int32 nStunLength, ActorCommonData & tACDTarget);
static void sApplyImmobilize(const PowerID idPowerFrom, float flProcScalar, const int32 nImmobilizeDuration, ActorCommonData & tACDTarget);
static void sApplySlow(const PowerID idPowerFrom, float flProcScalar, const int32 nSlowDuration, const float flSlowAmount, ActorCommonData & tACDTarget);
static void sApplyFear(const PowerID idPowerFrom, float flProcScalar, const int32 nFearDuration, ActorCommonData & tACDTarget);
static void sApplyFreeze(const PowerID idPowerFrom, float flProcScalar, const int32 nFreezeDuration, ActorCommonData & tACDTarget);
static void sApplyBlind(const PowerID idPowerFrom, float flProcScalar, const int32 nBlindDuration, ActorCommonData & tACDTarget, const BOOL bBlindBreaksOnDamage);


PayloadType PowerGetPayload(PowerID idPower);  // X/Server/Powers/Powers.cpp:2162
const GameBalanceDefinition * MonsterLevelTableLockR();  // X/Common/Definitions/GameBalanceDefinition.cpp:2281
void MonsterLevelTableUnlockR(const GameBalanceDefinition * lpDef);  // X/Common/Definitions/GameBalanceDefinition.cpp:2290
BOOL PowerCheckToHit(const PowerAPIAttackPayload & tPayload, const ActorCommonData & tACDTarget);  // X/Server/Powers/Powers.cpp:4475
BOOL PowerCheckToDodge(const PowerAPIAttackPayload & tPayload, const ActorCommonData & tACDTarget, ActorCommonData * ptACDAttacker);  // X/Server/Powers/Powers.cpp:4531
void PowerAPI_BuffApplyOnPowerDamageInitEffect(ACDID idACDSource, ACDID idACDTarget, PowerAPIDamage & tPowerDamage);  // PowerAPI.cpp:12043
void BrainAPIDodged(ACDID idACDAttacker, ACDID idACDDefender);  // X/Server/AI/BrainAPI.cpp:10331
static void sPowerAPI_Missed(const PowerAPIAttackPayload & tPayload, ActorCommonData & tACDTarget);
static void sPowerAPI_Dodged(const PowerAPIAttackPayload & tPayload, ActorCommonData & tACDTarget);
static void sApplyOnDodgeBuffs(ACDID idACDAttacker, ACDID idACDTarget, const PowerAPIAttackPayload & tPayload);
int32 PlayerGetNumPlayersForDifficulty();  // X/Common/Player.cpp:2159
BOOL PowerIsPhysical(PowerID idPower);  // X/Server/Powers/Powers.cpp:1210
void ServerMessageProjectileStick(const ActorCommonData & tACDSource, const ActorCommonData & tACDTarget, const SNO snoActorAttachment);  // X/Server/Network/ServerMessageSend.cpp:4855
BOOL PowerAPI_StickProjectileIntoActor(const PowerID idPower, const ActorCommonData & tACDTarget, const SNO snoProjectileToStick, const BOOL bOverrideDontStick);  // PowerAPI.cpp:6890
enum { FLOATING_NUMBER_TYPE_CRUSHING_BLOW = 28 };  // X/Common/Player.h
float AdjustHPForPlayerCount(const float flHP, const int32 nOldPlayerMult, const int32 nNewPlayerMult);  // X/Common/Monsters.cpp:1543

// PowerAPI.cpp:247. This helper intentionally uses the unchecked original lookup.
static PowerAPIBuffInstance * sBuffGet(BuffID idBuff)
{
    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    return ptPowerGlobals->pArrayBuffs->ElementGet(idBuff);
}

// PowerAPI.cpp:358
static void sApplyDamageAttribute(const PowerAPIDamage & tDamage, const DamageAttribute & tDamageAttribute)
{
    FastAttribKey tKey = tDamageAttribute.tKey;
    Attrib eAttrib = KeyGetAttrib(tKey);
    if (eAttrib == ATTRIBUTE_NONE)
        return;
    ACDID idACD = tDamage.idACDTarget;
    ActorCommonData & tACD = ACDGet(idACD);
    AttributeDataType eDamageAttributeDataType = KeyGetDataType(tKey);
    if (tDamageAttribute.nDuration == 0)
    {
        switch (eDamageAttributeDataType)
        {
        case 0:
            switch (eAttrib)
            {
            case ATTRIBUTE_HITPOINTS_CUR:
                {
                    float flHP = tACD.GetHitPointsCurrent();
                    flHP += tDamageAttribute.flValue;
                    tACD.SetHitPoints(flHP, TRUE, TRUE);
                    tACD.AttributesSetFloat(ATTRIBUTE_LAST_DAMAGE_AMOUNT, tDamageAttribute.flValue);
                }
                break;
            case ATTRIBUTE_RESOURCE_CUR:
                tACD.AddResource(tACD.GetPrimaryResourceType(), tDamageAttribute.flValue, FALSE);
                break;
            default:
                {
                    float flNewAttributeValue = tACD.AttributesGetFloat(tKey);
                    flNewAttributeValue += tDamageAttribute.flValue;
                    tACD.AttributesSetFloat(tKey, flNewAttributeValue);
                }
                break;
            }
            break;
        case 1:
            {
                int32 nNewAttributeValue = tACD.AttributesGetInt(tKey);
                nNewAttributeValue += tDamageAttribute.nValue;
                tACD.AttributesSetInt(tKey, nNewAttributeValue);
            }
            break;
        default:
            DisplayInternalError(NULL, "", 421, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            break;
        }
    }
    else
    {
        SNO snoBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_DAMAGE_ATTRIBUTE_BUFF);
        PowerAPIBuffStartData tBuffStartData(tDamage.idPower);
        tBuffStartData.dwDurationMin = tDamageAttribute.nDuration;
        tBuffStartData.bPositiveEffect = FALSE;
        tBuffStartData.eStackType = 1;
        tBuffStartData.snoPower = snoBuffPower;
        if (eAttrib == ATTRIBUTE_STUNNED)
        {
            BrainStanceForceUpdate((ActorID)ACDGetOwner(idACD));
        }
        switch (eDamageAttributeDataType)
        {
        case 0:
            FastAttribSetValueFloat(tBuffStartData.idFastAttrib, tKey, tDamageAttribute.flValue);
            break;
        case 1:
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, tKey, tDamageAttribute.nValue);
            break;
        }
        PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(idACD));
    }
}


// PowerAPI.cpp:453
// NON_MATCHING: 12 bytes long (7772 vs 7760). Every instruction of the original is reproduced; the differences all follow from
// two register-allocation decisions: (1) the spill slots of tACDTarget and snoPower are swapped (original [sp,#0x30] and
// [sp,#0x2c]), so the tACDTarget/pflDamageDone spills at line 499 do not pair into one stp, that block is not tail-duplicated
// and the blocks for lines 493/499-547 and 554-603 are laid out in a different order; (2) inside sApplyOnDoDamageBuffs the
// original reloads flDamageDone into x27 once before the pet-owner loop (line 10537), ours reloads it at the call (10547).
// o02: on DWARF lines 453-769 (#line); statements placed from the line rows (columns of 509-603 give the nesting).
#line 453
void PowerAPI_DoDamage(
    PowerAPIDamage & tDamage,
    BOOL fForceAllowDamage,
    float * pflDamageDone)
{


    ActorCommonData & tACDTarget = ACDGet(tDamage.idACDTarget);
    if (ActorFlagIsSet((ActorID)tACDTarget.idOwner, ACTOR_GOD_MODE_BIT))
    {
        return;
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHOW_PAYLOADS))
    {
        WorldPlace placeACD = tACDTarget.GetPlace();
    }


    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_PVP_GAME_OVER_BIT))
    {
        return;
    }


    if (!fForceAllowDamage && !PowerACDCanTakeDamage(tACDTarget))
    {
        return;
    }

    Power * ptPower = PowerTryToGet(tDamage.idPower);
    SNO snoPower = SNO_NONE;
    if (ptPower)
    {

        ptPower->dwFlags |= (1 << PF_CONSOLE_HAS_DAMAGED_OR_BUFFED);

        snoPower = PowerAPI_GetDamageSourcePowerSNO(&tDamage, NULL);


        if (FastAttribGetValueInt(ptPower->idFastAttrib, ATTRIBUTE_DOES_NO_DAMAGE))
        {
            return;
        }
    }

    if (pflDamageDone && !tDamage.bIgnoreDamageBonusAndReduction)
    {
        sApplyOnDoDamageBuffs(tDamage, pflDamageDone);


        if (ptPower)
        {
            ActorCommonData * ptACDPowerOwner = ACDTryToGet(ptPower->idACD);
            if (ptACDPowerOwner)
            {
                if (!ACDIsPet(ptACDPowerOwner->id) || ACDIsHireling(*ptACDPowerOwner))
                {
                    float flMultiplicativeDamageBonus = ptACDPowerOwner->AttributesGetFloat(ATTRIBUTE_MULTIPLICATIVE_DAMAGE_PERCENT_BONUS);
                    float flMultiplicativeSkillDamageBonus = ptACDPowerOwner->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_MULTIPLICATIVE_DAMAGE_PERCENT_BONUS_FOR_SKILL, ptPower->snoPower));
                    float flMultiplicativeDamageBonusNoPets = ptACDPowerOwner->AttributesGetFloat(ATTRIBUTE_MULTIPLICATIVE_DAMAGE_PERCENT_BONUS_NO_PETS);

                    float flMultiplicativeDamageBonusIfTargetCCd = ptACDPowerOwner->AttributesGetFloat(ATTRIBUTE_MULTIPLICATIVE_DAMAGE_PERCENT_BONUS_AGAINST_CC_TARGET);
                    float flMultiplicativeDamageBonusIfTargetSlowed = ptACDPowerOwner->AttributesGetFloat(ATTRIBUTE_MULTIPLICATIVE_DAMAGE_PERCENT_BONUS_AGAINST_SLOWED_TARGET);

                    float flMultiplicativeDamageBonusForPlayer = 1.0f;
                    Player * ptPlayer = PlayerGetByACD(ptPower->idACD);
                    if (ptPlayer)
                    {
                        FastAttribKey attribKey = MakeAttribKey(ATTRIBUTE_MULTIPLICATIVE_DAMAGE_PERCENT_BONUS_FOR_PLAYER, ptPlayer->nPlayerIndex);
                        flMultiplicativeDamageBonusForPlayer = tACDTarget.AttributesGetFloat(attribKey);
                    }

                    for (DamageType eDamageType = 0; eDamageType < NUM_DAMAGE_TYPES; eDamageType++)
                    {
                        sModifyReflectedDamage(ptPower, pflDamageDone[eDamageType]);

                        if (!tDamage.bIgnoreDamageBonusAndReduction)
                        {
                            pflDamageDone[eDamageType] *= flMultiplicativeDamageBonus;
                            pflDamageDone[eDamageType] *= flMultiplicativeSkillDamageBonus;
                            pflDamageDone[eDamageType] *= flMultiplicativeDamageBonusNoPets;
                            pflDamageDone[eDamageType] *= flMultiplicativeDamageBonusForPlayer;

                            if (tACDTarget.IsCCD())
                            {
                                pflDamageDone[eDamageType] *= flMultiplicativeDamageBonusIfTargetCCd;
                            }

                            if (tACDTarget.IsSlowed())
                            {
                                pflDamageDone[eDamageType] *= flMultiplicativeDamageBonusIfTargetSlowed;
                            }

                            pflDamageDone[eDamageType] = blz::max(0.0f, pflDamageDone[eDamageType]);
                        }
                    }
                }
                else
                {

                    ACDID idACDSummonedBy = (ACDID)FastAttribGetValueInt(ptACDPowerOwner->idFastAttrib, ATTRIBUTE_SUMMONED_BY_ACDID);
                    ActorCommonData * ptACDPetOwner = ACDTryToGet(idACDSummonedBy);
                    if (ptACDPetOwner)
                    {
                        float flMultiplicativeDamageBonus = ptACDPetOwner->AttributesGetFloat(ATTRIBUTE_MULTIPLICATIVE_DAMAGE_PERCENT_BONUS);


                        float flMultiplicativePetDamageBonus = ptACDPowerOwner->AttributesGetFloat(ATTRIBUTE_MULTIPLICATIVE_DAMAGE_PERCENT_BONUS);


                        SNO snoSummoningPower = ptACDPowerOwner->AttributesGetInt(ATTRIBUTE_SUMMONED_BY_SNO);
                        float flMultiplicativeSkillDamageBonus = ptACDPetOwner->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_MULTIPLICATIVE_DAMAGE_PERCENT_BONUS_FOR_SKILL, snoSummoningPower));


                        flMultiplicativeSkillDamageBonus *= ptACDPetOwner->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_MULTIPLICATIVE_DAMAGE_PERCENT_BONUS_FOR_SKILL, snoPower));


                        float flMultiplicativeDamageBonusIfTargetCCd = ptACDPetOwner->AttributesGetFloat(ATTRIBUTE_MULTIPLICATIVE_DAMAGE_PERCENT_BONUS_AGAINST_CC_TARGET);
                        float flMultiplicativeDamageBonusIfTargetSlowed = ptACDPetOwner->AttributesGetFloat(ATTRIBUTE_MULTIPLICATIVE_DAMAGE_PERCENT_BONUS_AGAINST_SLOWED_TARGET);

                        float flMultiplicativeDamageBonusForPlayer = 1.0f;
                        Player * ptPlayer = PlayerGetByACD(idACDSummonedBy);
                        if (ptPlayer)
                        {
                            FastAttribKey attribKey = MakeAttribKey(ATTRIBUTE_MULTIPLICATIVE_DAMAGE_PERCENT_BONUS_FOR_PLAYER, ptPlayer->nPlayerIndex);
                            flMultiplicativeDamageBonusForPlayer = tACDTarget.AttributesGetFloat(attribKey);
                        }

                        for (DamageType eDamageType = 0; eDamageType < NUM_DAMAGE_TYPES; eDamageType++)
                        {
                            sModifyReflectedDamage(ptPower, pflDamageDone[eDamageType]);

                            if (!tDamage.bIgnoreDamageBonusAndReduction)
                            {
                                pflDamageDone[eDamageType] *= flMultiplicativeDamageBonus;
                                pflDamageDone[eDamageType] *= flMultiplicativeSkillDamageBonus;
                                pflDamageDone[eDamageType] *= flMultiplicativePetDamageBonus;
                                pflDamageDone[eDamageType] *= flMultiplicativeDamageBonusForPlayer;

                                if (tACDTarget.IsCCD())
                                {
                                    pflDamageDone[eDamageType] *= flMultiplicativeDamageBonusIfTargetCCd;
                                }

                                if (tACDTarget.IsSlowed())
                                {
                                    pflDamageDone[eDamageType] *= flMultiplicativeDamageBonusIfTargetSlowed;
                                }

                                pflDamageDone[eDamageType] = blz::max(0.0f, pflDamageDone[eDamageType]);
                            }
                        }
                    }
                }
            }
        }
    }

    if (tDamage.bTriggersHitProcs || (pflDamageDone && tDamage.bIsAOEThorns))
    {
        sApplySplashDamage(tDamage, pflDamageDone, tACDTarget);
    }


    if (!tDamage.bProcessedShields)
    {
        float flUnadjustedDamage = 0.0f;
        POS pos = tDamage.listDamage.GetHeadPosition();
        POS posFound = NULL;
        while (pos)
        {
            posFound = pos;
            const DamageAttribute & tDamageAttribute = tDamage.listDamage.GetNext(pos);
            if (KeyGetAttrib(tDamageAttribute.tKey) == ATTRIBUTE_HITPOINTS_CUR)
            {
                flUnadjustedDamage = tDamageAttribute.flValue;
                break;
            }
        }

        if (flUnadjustedDamage < 0.0f)
        {
            BOOL bAdjustDamage = sApplyDamageShieldBuffs(tDamage.idACDTarget, pflDamageDone, tDamage);
            if (!tDamage.bIsStaggerDamage)
            {
                bAdjustDamage = sApplyStaggerDamage(tACDTarget, pflDamageDone) || bAdjustDamage;
            }
            if (pflDamageDone && bAdjustDamage)
            {
                float flAdjustedDamage = 0.0f;
                for (int32 iDamageType = 0; iDamageType < NUM_DAMAGE_TYPES; iDamageType++)
                {
                    flAdjustedDamage -= pflDamageDone[iDamageType];
                }
                DamageAttribute & tDamageAttribute = tDamage.listDamage.GetAt(posFound);
                tDamageAttribute.flValue = flAdjustedDamage;
            }
        }
    }

    if (tDamage.bDamageWasBlocked)
    {
        BrainAPIBlocked(tDamage.idACDSource, tDamage.idACDTarget);






        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(PlayerGetByACD(tDamage.idACDTarget));

        if (tEvaluationData.GetPlayer())
        {
            tEvaluationData.SetACD(ACDTryToGet(tDamage.idACDSource));
            tEvaluationData.SetPower(snoPower);


            tEvaluationData.EvaluateEvent(
                tEvaluationData.GetPlayer(),
                Achievement::ACH_EVENT_PLAYER_BLOCKED_ATTACK,
                snoPower,
                1, (uint32)sub_71007b5e70());


            tEvaluationData.EvaluateEvent(
                tEvaluationData.GetPlayer(),
                Achievement::ACH_EVENT_PLAYER_BLOCKED_DAMAGE,
                snoPower,
                Math::Max(0, Math::FloatRoundToInt(tDamage.flDamageBlocked)), (uint32)sub_71007b5e70());
        }
    }


    BOOL bFakeDamage = FALSE;
    ActorCommonData * ptACDSource = ACDTryToGet(tDamage.idACDSource);
    if (ptACDSource &&
        ptACDSource->AttributesGetInt(ATTRIBUTE_DOES_FAKE_DAMAGE) &&
        ptACDSource->IsAlive())
    {
        bFakeDamage = TRUE;
    }

    if (!bFakeDamage)
    {

        if (ptACDSource && ptACDSource->IsPet())
        {
            Player * ptPlayerOwner = ptACDSource->GetPetOwnerPlayer();
            if (ptPlayerOwner)
            {
                tACDTarget.TagForExp(ptPlayerOwner->idPlayerACD);
            }
        }




        else if (ptACDSource && ptACDSource->IsMonster())
        {
            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                if (ptACDSource->GetExpTagPlayer(*ptPlayer))
                {
                    tACDTarget.TagForExp(ptPlayer->idPlayerACD);
                }
            }
        }
        else
        {
            tACDTarget.TagForExp(tDamage.idACDSource);
        }

        POS pos = tDamage.listDamage.GetHeadPosition();
        while (pos)
        {
            const DamageAttribute & tDamageAttribute = tDamage.listDamage.GetNext(pos);
            sApplyDamageAttribute(tDamage, tDamageAttribute);
        }
    }


    float flHitPointDamage = 0.0f;
    if (pflDamageDone)
    {
        for (int32 nDamageType = 0; nDamageType < NUM_DAMAGE_TYPES; nDamageType++)
        {
            flHitPointDamage += pflDamageDone[nDamageType];
        }
    }

    if (tDamage.eDeliveryMechanism != PDM_DOT && tDamage.eDeliveryMechanism != PDM_THORNS && tDamage.bTriggersHitProcs)
    {
        sApplyOnGetHitBuffs(tDamage, flHitPointDamage, tDamage.idACDMainActor, tDamage.idACDTarget);
    }

    if (pflDamageDone && ptACDSource && tDamage.bTriggersHitProcs)
    {
        for (DamageType eDamageType = 0; eDamageType < NUM_DAMAGE_TYPES; eDamageType++)
        {
            if (pflDamageDone[eDamageType] > EPSILON)
            {
                sApplyLifeOrManaStealEffects(tDamage.idPower, *ptACDSource, tACDTarget, pflDamageDone[eDamageType]);
            }
        }
    }

    SGameStatsRecordDamage(tDamage.idACDMainActor, tDamage.idACDTarget, flHitPointDamage, snoPower, tDamage.idPower, tDamage.bCriticalHit, GetPredominantDamageType(pflDamageDone, FALSE), tDamage.eDeliveryMechanism);

    BrainUpCallOnDamage(tDamage, pflDamageDone);

    if (tDamage.eDeliveryMechanism != PDM_DOT && tDamage.eDeliveryMechanism != PDM_THORNS && tDamage.bTriggersHitProcs)
    {
        sApplyOnHitBuffs(tDamage, flHitPointDamage, pflDamageDone, tDamage.idACDMainActor, tDamage.idACDTarget, ACDID_INVALID);
    }
}
#line 548


// PowerAPI.cpp:773
void PowerAPI_DoHitpointRegen(ActorCommonData & tACD, float flHitpointsPerSecond, uint32 dwTimeStep)
{
    if (flHitpointsPerSecond > EPSILON)
    {
        float flRegenAmount = (dwTimeStep * flHitpointsPerSecond) / TICKS_PER_SECOND;
        tACD.Heal(flRegenAmount, TRUE);
        CombatLog::LogEvent(10, tACD.id, tACD.id, -1, flRegenAmount, 0.0f, 8);
    }
    else if (flHitpointsPerSecond < EPSILON)
    {
        float flDamageDone[7];
        for (int32 i = 1; i < 7; i++)
            flDamageDone[i] = 0.0f;
        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_SLOW_KILL_BIT))
            flHitpointsPerSecond *= 0.01f;
        float flHitPointDamage = 0.0f;
        flHitPointDamage -= (flHitpointsPerSecond * dwTimeStep) / TICKS_PER_SECOND;
        flDamageDone[0] = flHitPointDamage;
        if (flHitPointDamage > EPSILON)
        {
            PowerAPIDamage tDamageOverTime;
            PowerHelper_DamageInit(tDamageOverTime, -1, tACD.id);
            PowerHelper_DamageAdd(tDamageOverTime, ATTRIBUTE_HITPOINTS_CUR, -flHitPointDamage, 0);
            tDamageOverTime.eDeliveryMechanism = PDM_DOT;
            PowerAPI_DoDamage(tDamageOverTime, FALSE, flDamageDone);
        }
    }
}

// PowerAPI.cpp:815
static void sEvaluateAppliedBuffAchievements(
    const PowerAPIBuffInstance & tBuff,
    const ActorCommonData & tACDTarget,
    BOOL bAppliedCCEffect)
{

    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetACD(&tACDTarget);
        tEvaluationData.SetBuff(&tBuff);

        const Player * ptCastingPlayer = PlayerGetByACD(tBuff.idACDSource);
        if (ptCastingPlayer)
        {
            const Player * ptTargetPlayer = PlayerGetByACD(tACDTarget.id);
            if (ptTargetPlayer)
            {
                tEvaluationData.SetPlayer(ptTargetPlayer);

                Achievement::EventID eEventID = (ptTargetPlayer == ptCastingPlayer) ? Achievement::ACH_EVENT_PLAYER_BUFFED_SELF : Achievement::ACH_EVENT_PLAYER_BUFFED_PLAYER;
                tEvaluationData.EvaluateEvent(ptCastingPlayer, eEventID, tBuff.snoPower, 1, (uint32)sub_71007b5e70());
            }

            tEvaluationData.SetPlayer(ptCastingPlayer);

            if (ACDIsHireling(tACDTarget))
            {
                tEvaluationData.EvaluateEvent(ptCastingPlayer, Achievement::ACH_EVENT_PLAYER_BUFFED_FOLLOWER, tBuff.snoPower, 1, (uint32)sub_71007b5e70());
            }
            else if (tACDTarget.IsMonster() && bAppliedCCEffect)
            {
                tEvaluationData.EvaluateEvent(ptCastingPlayer, Achievement::ACH_EVENT_PLAYER_APPLIED_CC_TO_MONSTER, tBuff.snoPower, 1, (uint32)sub_71007b5e70());
            }
        }
    }
}

// PowerAPI.cpp:864
static void sEvaluateBuffRemovedAchievements(const PowerAPIBuffInstance & tBuff)
{
    Achievement::EvaluationData tEvaluationData;
    ActorCommonData * ptTarget = ACDTryToGet(tBuff.idACDAffected);
    if (ptTarget)
    {
        tEvaluationData.SetACD(ptTarget);
        tEvaluationData.SetBuff(&tBuff);
        const Player * ptTargetPlayer = PlayerGetByACD(tBuff.idACDAffected);
        if (ptTargetPlayer)
        {
            tEvaluationData.SetPlayer(ptTargetPlayer);
            tEvaluationData.EvaluateEvent(ptTargetPlayer, Achievement::ACH_EVENT_PLAYER_BUFF_REMOVED, tBuff.snoPower, 1, (uint32)sub_71007b5e70());
        }
        else if (ptTarget->IsMonster())
        {
            ptTargetPlayer = PlayerGetByACD(tBuff.idACDSource);
            if (ptTargetPlayer)
            {
                tEvaluationData.SetPlayer(ptTargetPlayer);
                tEvaluationData.EvaluateEvent(ptTargetPlayer, Achievement::ACH_EVENT_MONSTER_BUFF_REMOVED, tBuff.snoPower, 1, (uint32)sub_71007b5e70());
            }
        }
    }
}

// PowerAPI.cpp:913
static void sPowerAPI_CheckForOnHitProcs(Power & tPower, ActorCommonData & tACDAttacker, ActorCommonData & tACDTarget, KnockbackStyle eKnockbackStyle, float flProcScalar, float * flDamageDone, float flDamageTotal, const PowerAPIAttackPayload * ptPayload, const PowerAPIBuffInstance * ptBuff, BOOL bCrit)
{
    if (tACDTarget.IsWreckableObject())
        return;
    float flPowerProcCoefficient = flProcScalar;
    if (flPowerProcCoefficient < 0.0f)
    {
        flPowerProcCoefficient = PowerGetProcScalar(tPower);
    }
    {
        ResourceType eType = tACDAttacker.GetPrimaryResourceType();
        float flManaPerHit = flPowerProcCoefficient * tACDAttacker.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_ON_HIT, eType);
        if (Math::Abs(flManaPerHit) > EPSILON)
        {
            tACDAttacker.AddResource(eType, flManaPerHit, TRUE);
        }
    }
    {
        ResourceType eType = tACDAttacker.GetSecondaryResourceType();
        if (eType != RESOURCE_TYPE_INVALID)
        {
            float flManaPerHit = flPowerProcCoefficient * tACDAttacker.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_ON_HIT, eType);
            if (Math::Abs(flManaPerHit) > EPSILON)
            {
                tACDAttacker.AddResource(eType, flManaPerHit, TRUE);
            }
        }
    }
    float flHealthPerHit = tACDAttacker.AttributesGetFloat(ATTRIBUTE_HITPOINTS_ON_HIT);
    if (flPowerProcCoefficient > EPSILON && Math::Abs(flHealthPerHit) > EPSILON &&
        !tPower.bDoesntAnimate &&
        !tPower.bFreeCast)
    {
        uint32 dwNow = GameGetServerTime();
        uint32 * pdwLastLifeOnHitTick = &tPower.dwLastLifeOnHitTick;
        BOOL bDoLifeOnHit = FALSE;
        uint32 dwCooldownMinTime = Math::FloatToInt(tACDAttacker.AttributesGetFloat(ATTRIBUTE_POWER_COOLDOWN_MIN_TIME) * TICKS_PER_SECOND);
        if (!PowerIsPrimary(AutoPowerDefinitionR(tPower.snoPower), tACDAttacker.id) &&
            PowerGetCastingCooldown(AutoPowerDefinitionR(tPower.snoPower), tACDAttacker) <= dwCooldownMinTime)
        {
        }
        else
        {
            if (PowerIsChannelled(tACDAttacker, AutoPowerDefinitionR(tPower.snoPower)))
            {
                Player * ptPlayer = PlayerGetByACD(tACDAttacker.id);
                if (ptPlayer)
                {
                    pdwLastLifeOnHitTick = NULL;
                    for (int32 i = 0; i < 6; i++)
                    {
                        SNO snoHotbarSkill = ptPlayer->tHeroStateData.tSavedData.tActiveSkills[i].snoSkill;
                        if (tACDAttacker.AttributesGetInt(ATTRIBUTE_SKILL_OVERRIDE_ACTIVE))
                        {
                            SNO snoOverrideSkill = tACDAttacker.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE, i));
                            if (snoOverrideSkill != SNO_NONE)
                            {
                                snoHotbarSkill = snoOverrideSkill;
                            }
                        }
                        if (snoHotbarSkill == tPower.snoPower)
                        {
                            pdwLastLifeOnHitTick = &ptPlayer->tHeroStateData.tSavedData.tActiveSkills[i].dwLastLifeOnHitTick;
                            break;
                        }
                    }
                }
                float flAttacksPerSecond = tACDAttacker.AttributesGetFloat(ATTRIBUTE_ATTACKS_PER_SECOND_TOTAL);
                uint32 dwAttackSpeed = (uint32)((1.0f / flAttacksPerSecond) * TICKS_PER_SECOND);
                if (pdwLastLifeOnHitTick && dwNow >= *pdwLastLifeOnHitTick + dwAttackSpeed)
                {
                    bDoLifeOnHit = TRUE;
                }
            }
            else if (*pdwLastLifeOnHitTick == 0)
            {
                bDoLifeOnHit = TRUE;
            }
        }
        if (bDoLifeOnHit)
        {
            tACDAttacker.Heal(flHealthPerHit, TRUE);
            CombatLog::LogEvent(10,
                                tACDAttacker.id,
                                tACDAttacker.id,
                                SNO_NONE, flHealthPerHit, 0.0f, 10);
            *pdwLastLifeOnHitTick = dwNow;
        }
    }
    ScopedGlobalDefinitionLockR pdefGlobal;
    if (!(SGameGlobalsGet().dwFlags2 & (1 << SGAMEGLOBALS2_DISABLE_ELEMENTAL_DAMAGE_PROCS)) &&
        tPower.idFastAttrib != -1)
    {
        float flFearProcChance = flPowerProcCoefficient * (1.0f - (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_ON_HIT_FEAR_PROC_CHANCE)) * (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_WEAPON_ON_HIT_FEAR_PROC_CHANCE_CURRENTHAND)));
        float flStunProcChance = flPowerProcCoefficient * (1.0f - (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_ON_HIT_STUN_PROC_CHANCE)) * (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_WEAPON_ON_HIT_STUN_PROC_CHANCE_CURRENTHAND)));
        float flBlindProcChance = flPowerProcCoefficient * (1.0f - (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_ON_HIT_BLIND_PROC_CHANCE)) * (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_WEAPON_ON_HIT_BLIND_PROC_CHANCE_CURRENTHAND)));
        float flFreezeProcChance = flPowerProcCoefficient * (1.0f - (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_ON_HIT_FREEZE_PROC_CHANCE)) * (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_WEAPON_ON_HIT_FREEZE_PROC_CHANCE_CURRENTHAND)));
        float flChillProcChance = flPowerProcCoefficient * (1.0f - (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_ON_HIT_CHILL_PROC_CHANCE)) * (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_WEAPON_ON_HIT_CHILL_PROC_CHANCE_CURRENTHAND)));
        float flSlowProcChance = flPowerProcCoefficient * (1.0f - (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_ON_HIT_SLOW_PROC_CHANCE)) * (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_WEAPON_ON_HIT_SLOW_PROC_CHANCE_CURRENTHAND)));
        float flImmobilizeProcChance = flPowerProcCoefficient * (1.0f - (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_ON_HIT_IMMOBILIZE_PROC_CHANCE)) * (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_WEAPON_ON_HIT_IMMOBILIZE_PROC_CHANCE_CURRENTHAND)));
        float flKnockbackProcChance = flPowerProcCoefficient * (1.0f - (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_ON_HIT_KNOCKBACK_PROC_CHANCE)) * (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_WEAPON_ON_HIT_KNOCKBACK_PROC_CHANCE_CURRENTHAND)));
        float flBleedProcChance = flPowerProcCoefficient * (1.0f - (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_ON_HIT_BLEED_PROC_CHANCE)) * (1.0f - FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_WEAPON_ON_HIT_BLEED_PROC_CHANCE_CURRENTHAND)));
        float flPercentBleedProcChance = flPowerProcCoefficient * FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_WEAPON_ON_HIT_PERCENT_BLEED_PROC_CHANCE_CURRENTHAND);
        if (tACDTarget.IsAlive() &&
            !tACDTarget.IsWreckableObject() &&
            !ActorIsUsingAIQueue((ActorID)ACDGetOwner(tACDTarget.id)))
        {
            if (flFearProcChance > EPSILON)
            {
                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                CombatLog::LogEvent(11,
                                    tPower.idACD,
                                    tACDTarget.id,
                                    GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_FEARED),
                                    flRoll, flFearProcChance, 0);
                if (flRoll <= flFearProcChance)
                {
                    sApplyFear(tPower.id, flProcScalar, 120, tACDTarget);
                }
            }
            if (flStunProcChance > EPSILON)
            {
                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                CombatLog::LogEvent(11,
                                    tPower.idACD,
                                    tACDTarget.id,
                                    GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_STUNNED),
                                    flRoll, flStunProcChance, 0);
                if (flRoll <= flStunProcChance)
                {
                    sApplyStun(tPower.id, flProcScalar, 120, tACDTarget);
                }
            }
            if (flBlindProcChance > EPSILON)
            {
                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                CombatLog::LogEvent(11,
                                    tPower.idACD,
                                    tACDTarget.id,
                                    GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_FEARED),
                                    flRoll, flBlindProcChance, 0);
                if (flRoll <= flBlindProcChance)
                {
                    sApplyBlind(tPower.id, flProcScalar, 120, tACDTarget, FALSE);
                }
            }
            if (flFreezeProcChance > EPSILON)
            {
                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                CombatLog::LogEvent(11,
                                    tPower.idACD,
                                    tACDTarget.id,
                                    GlobalSNOGet(GLOBALSNO_POWERS_CRIT_DEBUFF_COLD),
                                    flRoll, flFreezeProcChance, 0);
                if (flRoll <= flFreezeProcChance)
                {
                    sApplyFreeze(tPower.id, flProcScalar, 120, tACDTarget);
                }
            }
            if (flChillProcChance > EPSILON)
            {
                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                CombatLog::LogEvent(11,
                                    tPower.idACD,
                                    tACDTarget.id,
                                    GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_CHILLED),
                                    flRoll, flChillProcChance, 0);
                if (flRoll <= flChillProcChance)
                {
                    sApplyChill(tPower.id, flProcScalar, 120, 0.5f, tACDTarget);
                }
            }
            if (flSlowProcChance > EPSILON)
            {
                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                CombatLog::LogEvent(11,
                                    tPower.idACD,
                                    tACDTarget.id,
                                    GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_SLOWED),
                                    flRoll, flSlowProcChance, 0);
                if (flRoll <= flSlowProcChance)
                {
                    sApplySlow(tPower.id, flProcScalar, 120, 0.5f, tACDTarget);
                }
            }
            if (flImmobilizeProcChance > EPSILON)
            {
                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                CombatLog::LogEvent(11,
                                    tPower.idACD,
                                    tACDTarget.id,
                                    GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_ROOTED),
                                    flRoll, flImmobilizeProcChance, 0);
                if (flRoll <= flImmobilizeProcChance)
                {
                    sApplyImmobilize(tPower.id, flProcScalar, 120, tACDTarget);
                }
            }
            if (eKnockbackStyle == KNOCKBACK_NONE && flKnockbackProcChance > EPSILON)
            {
                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                CombatLog::LogEvent(11,
                                    tPower.idACD,
                                    tACDTarget.id,
                                    GlobalSNOGet(GLOBALSNO_POWERS_KNOCKBACK),
                                    flRoll, flKnockbackProcChance, 0);
                if (flRoll <= flKnockbackProcChance)
                {
                    PowerAPIAttackPayload tPayload;
                    PowerHelper_AttackPayloadInit_Blank(tPayload);
                    tPayload.idPower = tPower.id;
                    tPayload.eDeliveryMechanism = 5;
                    tPayload.flBaseDamageScalar = 0.0f;
                    tPayload.wdKnockbackMagnitude = 13.0f;
                    tPayload.flKnockbackHeightMin = 2.0f;
                    tPayload.bContinuousKnockbackPower = FALSE;
                    tPayload.flKnockbackGravityMin = -0.11f;
                    tPayload.flKnockbackHeightDelta = 0.0f;
                    tPayload.bKnockbackIgnoresWeight = FALSE;
                    tPayload.flKnockbackGravityDelta = 0.0f;
                    tPayload.snoKnockbackPower = GlobalSNOGet(GLOBALSNO_POWERS_KNOCKBACK);
                    tPayload.bKnockbackAffectedByReduction = TRUE;
                    tPayload.eKnockBack = KNOCKBACK_FROM_ATTACKER;
                    PowerAPI_DoPayload(tPayload, tACDTarget.id, ACDID_INVALID);
                }
            }
            if (flBleedProcChance > EPSILON)
            {
                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                CombatLog::LogEvent(11,
                                    tPower.idACD,
                                    tACDTarget.id,
                                    GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_BLEED),
                                    flRoll, flBleedProcChance, 0);
                if (flRoll <= flBleedProcChance)
                {
                    float flBaseDPS = FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_WEAPON_ON_HIT_BLEED_PROC_DAMAGE_BASE_CURRENTHAND) / pdefGlobal->ptServerData->flBleedDuration;
                    float flDeltaDPS = RandFloat_Internal(SGameGlobalsGet().rng) * (FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_WEAPON_ON_HIT_BLEED_PROC_DAMAGE_BASE_CURRENTHAND) / pdefGlobal->ptServerData->flBleedDuration);
                    SNO snoBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_BLEED);
                    PowerAPIBuffStartData tBuffStartData(POWER_NONE);
                    tBuffStartData.idPower = tPower.id;
                    tBuffStartData.dwDurationMin = (uint32)(pdefGlobal->ptServerData->flBleedDuration * TICKS_PER_SECOND);
                    tBuffStartData.eStackType = 2;
                    tBuffStartData.nMaxStackCount = 1;
                    tBuffStartData.snoPower = snoBuffPower;
                    tBuffStartData.snoDOTDamageSourcePower = snoBuffPower;
                    tBuffStartData.nBuffIndex = 0;
                    tBuffStartData.flDamagePerSecond = flBaseDPS + flDeltaDPS;
                    tBuffStartData.bPositiveEffect = FALSE;
                    tBuffStartData.eDamageType = 0;
                    tBuffStartData.bTriggersHitProcs = FALSE;
                    tBuffStartData.bAppliedDamageReductionEffects = FALSE;
                    tBuffStartData.flPrimaryAttributeBonusMultiplier = PowerComputeDamageScalarFromPrimaryAttribute(tACDAttacker.idFastAttrib) + 1.0f;
                    tBuffStartData.flClassBonusMultiplier = PowerGetClassBonusPercent(tACDAttacker.idFastAttrib, PlayerGetPlayerClass(tACDAttacker.id)) + 1.0f;
                    FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_VISUAL_EFFECT_START, snoBuffPower), 1);
                    PowerAPI_BuffAddToACD(tBuffStartData, tACDTarget);
                }
            }
            if (flPercentBleedProcChance > EPSILON)
            {
                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                CombatLog::LogEvent(11,
                                    tPower.idACD,
                                    tACDTarget.id,
                                    GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_BLEED),
                                    flRoll, flPercentBleedProcChance, 0);
                if (flRoll <= flPercentBleedProcChance)
                {
                    float flDamagePercent = FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_WEAPON_ON_HIT_PERCENT_BLEED_PROC_DAMAGE_CURRENTHAND);
                    float flDuration = FastAttribGetValueFloat(tPower.idFastAttrib, ATTRIBUTE_WEAPON_ON_HIT_PERCENT_BLEED_PROC_DURATION_CURRENTHAND);
                    float flDamage = 0.0f;
                    float flAttackSpeed = 0.0f;
                    PowerActorGetWeaponDamageAndAttackSpeed(tACDAttacker, flDamage, flAttackSpeed, FALSE, POWER_NONE, FALSE);
                    float flScopedDamageTotal = flDamagePercent * flDamage;
                    if (flDuration > EPSILON && flScopedDamageTotal > EPSILON)
                    {
                        float flDPS = flScopedDamageTotal / flDuration;
                        SNO snoBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_BLEED);
                        PowerAPIBuffStartData tBuffStartData(POWER_NONE);
                        tBuffStartData.idPower = tPower.id;
                        tBuffStartData.dwDurationMin = (uint32)(flDuration * TICKS_PER_SECOND);
                        tBuffStartData.eStackType = 2;
                        tBuffStartData.nMaxStackCount = 1;
                        tBuffStartData.snoPower = snoBuffPower;
                        tBuffStartData.snoDOTDamageSourcePower = snoBuffPower;
                        tBuffStartData.nBuffIndex = 0;
                        tBuffStartData.flDamagePerSecond = flDPS;
                        tBuffStartData.bPositiveEffect = FALSE;
                        tBuffStartData.eDamageType = 0;
                        tBuffStartData.bTriggersHitProcs = FALSE;
                        tBuffStartData.bAppliedDamageReductionEffects = FALSE;
                        tBuffStartData.bUsePowersProcScalar = TRUE;
                        tBuffStartData.flPrimaryAttributeBonusMultiplier = PowerComputeDamageScalarFromPrimaryAttribute(tACDAttacker.idFastAttrib) + 1.0f;
                        tBuffStartData.flClassBonusMultiplier = PowerGetClassBonusPercent(tACDAttacker.idFastAttrib, PlayerGetPlayerClass(tACDAttacker.id)) + 1.0f;
                        FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_VISUAL_EFFECT_START, snoBuffPower), 1);
                        PowerAPI_BuffAddToACD(tBuffStartData, tACDTarget);
                    }
                }
            }
        }
    }
}


// PowerAPI.cpp:1555
static void sPowerAPI_CheckForOnCriticalProcs(const PowerAPIDamage & tDamage, ActorCommonData * ptACDAttacker, ActorCommonData & tACDTarget)
{
    if (tACDTarget.IsWreckableObject())
        return;
    Target tTarget;
    tTarget.InitializeFromACD(tDamage.idACDTarget, FALSE, FALSE);
    if (ptACDAttacker)
    {
        float flPowerProcScalar = tDamage.flProcScalar;
        {
            ResourceType eType = ptACDAttacker->GetPrimaryResourceType();
            if (eType != -1)
            {
                float flManaPerHit = flPowerProcScalar * ptACDAttacker->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_ON_CRIT, eType);
                if (Math::Abs(flManaPerHit) > EPSILON)
                    ptACDAttacker->AddResource(eType, flManaPerHit, FALSE);
            }
        }
        {
            ResourceType eType = ptACDAttacker->GetSecondaryResourceType();
            if (eType != -1)
            {
                float flManaPerHit = flPowerProcScalar * ptACDAttacker->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_ON_CRIT, eType);
                if (Math::Abs(flManaPerHit) > EPSILON)
                    ptACDAttacker->AddResource(eType, flManaPerHit, FALSE);
            }
        }
        if (ACDIsPlayer(ptACDAttacker->id))
        {
            uint32 dwNow = GameGetServerTime();
            uint32 uNextEffectTime = ptACDAttacker->AttributesGetInt(ATTRIBUTE_CRIT_EFFECT_TIME);
            if (dwNow >= uNextEffectTime)
            {
                SNO snoCritEffect = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_PLAYER_CRIT);
                if (snoCritEffect != SNO_NONE)
                {
                    ACDPlayEffect(ptACDAttacker->id, EFFECT_PLAY_EFFECT_GROUP, snoCritEffect, ptACDAttacker->id);
                    ptACDAttacker->AttributesSetInt(ATTRIBUTE_CRIT_EFFECT_TIME, dwNow + 20);
                }
            }
        }
    }
}

// PowerAPI.cpp:1840
static inline void sApplyThornsOnFirstHit(ActorCommonData & tACDTarget, ActorCommonData & tACDAttacker, Power * ptPower)
{
    if (ptPower->snoPower != GlobalSNOGet(GLOBALSNO_POWERS_THORNS))
    {
        float flOnFirstHitThornsPercent = tACDAttacker.AttributesGetFloat(ATTRIBUTE_ON_FIRST_HIT_THORNS_PERCENT);
        if (flOnFirstHitThornsPercent > EPSILON)
            sApplyThorns(flOnFirstHitThornsPercent, NULL, tACDTarget, tACDAttacker, ptPower, TRUE);
    }
}

// PowerAPI.cpp:1861
static BOOL sPayloadIsUnavoidable(const PowerAPIAttackPayload & tPayload)
{
    if (tPayload.eDeliveryMechanism == PDM_THORNS || tPayload.bAlwaysHits)
    {
        return TRUE;
    }
    if (tPayload.idPower != POWERID_INVALID)
    {
        PayloadType ePayload = PowerGetPayload(tPayload.idPower);
        if (ePayload == 2 || ePayload == 3)
        {
            return TRUE;
        }
    }
    return FALSE;
}


// PowerAPI.cpp:1896
extern "C" uint32 sub_71007b75d0();  // 2.7.7 only, unnamed, defined in SGame.cpp

static void sSendHealthCostAchievementEvent(ActorCommonData * ptACDPlayer, Power * ptPower, int32 nNumberOfHeals)
{
    Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
    if (ptPlayer)
    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(ptPlayer);
        if (tEvaluationData.GetPlayer())
        {
            tEvaluationData.SetPower(ptPower);
            tEvaluationData.EvaluateEvent(tEvaluationData.GetPlayer(), Achievement::ACH_EVENT_SPEND_HEALTH, nNumberOfHeals, sub_71007b75d0(), (uint32)sub_71007b5e70());
        }
    }
}

// PowerAPI.cpp:1932
static BOOL sBuffIsAppliedOnlyToSpecificPowers(
    const PowerAPIBuffInstance & tBuff)
{
    if (tBuff.nBuffIndex >= 0 && tBuff.nBuffIndex < 32)
    {
        if (tBuff.snoPower != SNO_NONE)
        {
            ScopedPowerDefinitionLockR pdefPower(tBuff.snoPower);
            const BuffDef & tBuffDef = pdefPower->tPower.arBuff[tBuff.nBuffIndex];
            if (tBuffDef.serBuffFilters.dwSize)
                return TRUE;
        }
    }
    return FALSE;
}

// PowerAPI.cpp:1955
static BOOL sApplyBlockAmount(const ACDID idACDDamageFrom, const ActorCommonData & tACDTarget, float & flDamageBlocked, float * flDamageDone)
{
    float flTotalDamage = 0.0f;
    int32 nDamageType;
    for (nDamageType = 0; nDamageType < 7; nDamageType++)
    {
        if (flDamageDone[nDamageType] > EPSILON)
        {
            flTotalDamage += flDamageDone[nDamageType];
        }
    }
    if (flTotalDamage <= EPSILON)
    {
        return FALSE;
    }

    float flBlockAmountMin = tACDTarget.AttributesGetFloat(ATTRIBUTE_BLOCK_AMOUNT_TOTAL_MIN);
    float flBlockAmountDelta = tACDTarget.AttributesGetFloat(ATTRIBUTE_BLOCK_AMOUNT_ITEM_DELTA);
    flBlockAmountDelta *= RandFloat_Internal(SGameGlobalsGet().rng);
    float flTotalBlockAmount = flBlockAmountMin + flBlockAmountDelta;

    float flOOTotalDamage = 1.0f / flTotalDamage;

    for (nDamageType = 0; nDamageType < 7; nDamageType++)
    {
        if (flDamageDone[nDamageType] > EPSILON)
        {
            float flDamageReduction = flOOTotalDamage * flDamageDone[nDamageType] * flTotalBlockAmount;
            flDamageDone[nDamageType] = Math::Max(0.0f, flDamageDone[nDamageType] - flDamageReduction);
        }
    }

    flDamageBlocked = Math::Min(flTotalDamage, flTotalBlockAmount);
    return TRUE;
}


// PowerAPI.cpp:2003
static BOOL sTargetBlocked(const PowerAPIAttackPayload & tPayload, ActorCommonData & tACDTarget, float & flDamageBlocked, float * flDamageDone)
{
    PayloadDeliveryMechanism eDeliveryMechanism = tPayload.eDeliveryMechanism;
    if (eDeliveryMechanism == PDM_THORNS)
    {
        return FALSE;
    }

    if (tPayload.bAlwaysHits)
    {
        return FALSE;
    }

    float flBlockChance = tACDTarget.AttributesGetFloat(ATTRIBUTE_BLOCK_CHANCE_CAPPED_TOTAL);

    ActorCommonData * ptACDPowerOwner = PowerGetOwner(tPayload.idPower);

    float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);

    SNO snoPower = SNO_NONE;
    Power * ptPower = PowerTryToGet(tPayload.idPower);
    if (ptPower)
    {
        snoPower = ptPower->snoPower;
    }

    CombatLog::LogEvent(6,
        ptACDPowerOwner ? ptACDPowerOwner->id : ACDID_INVALID,
        tACDTarget.id,
        snoPower, flBlockChance, flRoll, 0);

    if (flRoll <= flBlockChance)
    {
        BOOL fBlockedDamage = sApplyBlockAmount(ptACDPowerOwner ? ptACDPowerOwner->id : ACDID_INVALID, tACDTarget, flDamageBlocked, flDamageDone);
        return fBlockedDamage;
    }

    return FALSE;
}


// PowerAPI.cpp:2119
void PowerAPI_ResetCooldowns(ActorCommonData & tACD)
{
    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_COOLDOWN);
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && ptBuff->snoPower == snoPower)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, 0, TRUE);
        }
    }
}

// PowerAPI.cpp:2139
static void sApplyArmor(
    PowerID idPower,
    const ActorCommonData & tACDTarget,
    float & flDamageDone)
{
    if (idPower != POWERID_INVALID)
    {
        int32 nAttackerLevel = PowerAttributesGetInt(idPower, ATTRIBUTE_LEVEL);

        if (GameTypeIsPvP())
        {
            int32 nPVPLevelEqualizerLevel = PowerAttributesGetInt(idPower, ATTRIBUTE_PVP_LEVEL_EQUALIZER_LEVEL);
            if (nPVPLevelEqualizerLevel > 0)
            {
                nAttackerLevel = nPVPLevelEqualizerLevel;
            }
        }

        float flPercentMitigation = PowerComputeArmorDamageMitigation(tACDTarget, nAttackerLevel);

        flDamageDone *= (1.0f - flPercentMitigation);
    }
}

// PowerAPI.cpp:2321
static void sApplyMiscDamageAbsorb(
    const ActorCommonData & tACDTarget,
    float & flDamageDone,
    const DamageType eDamageType)
{
    if (eDamageType == 0)
    {
        float flAbsorb = tACDTarget.AttributesGetFloat(ATTRIBUTE_DAMAGE_ABSORB_PERCENT);
        if (flAbsorb > 0.0f)
        {
            flDamageDone = (1.0f - flAbsorb) * flDamageDone;
        }
    }

    float flAbsorbAll = tACDTarget.AttributesGetFloat(ATTRIBUTE_DAMAGE_ABSORB_PERCENT_ALL);
    if (Math::Abs(flAbsorbAll) > EPSILON)
    {
        flDamageDone = (1.0f - flAbsorbAll) * flDamageDone;
    }

    float flClassDamageReductionPercent = 0.0f;
    if (GameTypeIsPvP() || tACDTarget.IsDueling())
    {
        flClassDamageReductionPercent = tACDTarget.AttributesGetFloat(FastAttribKey(ATTRIBUTE_CLASS_DAMAGE_REDUCTION_PERCENT_PVP));
    }
    else
    {
        flClassDamageReductionPercent = tACDTarget.AttributesGetFloat(FastAttribKey(ATTRIBUTE_CLASS_DAMAGE_REDUCTION_PERCENT));
    }

    if (flClassDamageReductionPercent > EPSILON)
    {
        flDamageDone = (1.0f - flClassDamageReductionPercent) * flDamageDone;
    }
}

// PowerAPI.cpp:2360
static void sPetScalar(const PowerAPIAttackPayload & tPayload, const ActorCommonData & tACDTarget, float & flDamageDone)
{
    if (tACDTarget.IsPet())
    {
        flDamageDone = tPayload.flPetDamageScalar * flDamageDone;
    }
}


// PowerAPI.cpp:2373
static void sCharmedMonsterScalar(const PowerAPIAttackPayload & tPayload, const ActorCommonData & tACDTarget, float & flDamageDone)
{
    if (tPayload.bIgnoreCharmDamageScalar)
    {
        return;
    }
    Team ePowerTeam = PowerGetTeam(tPayload.idPower);
    if (TeamIsMonsterTeam(ePowerTeam) &&
        TeamIsMonsterTeam(tACDTarget.GetTeam()))
    {
        flDamageDone = tPayload.flPetDamageScalar * flDamageDone;
        const GameBalanceDefinition * ptMonsterLevelGBDef = MonsterLevelTableLockR();
        if (ptMonsterLevelGBDef)
        {
            int32 nPowerLevel = PowerGetLevel(tPayload.idPower);
            if (nPowerLevel <= 0)
            {
                ActorCommonData * ptACDOwner = PowerGetOwner(tPayload.idPower);
                if (ptACDOwner)
                {
                    nPowerLevel = ptACDOwner->AttributesGetInt(ATTRIBUTE_LEVEL);
                }
            }
            if (nPowerLevel > 0)
            {
                float flDamageModifier = ptMonsterLevelGBDef->tMonsterLevelTable.ptMonsterLevels[nPowerLevel - 1].flAttributesHirelingDamage /
                    ptMonsterLevelGBDef->tMonsterLevelTable.ptMonsterLevels[nPowerLevel - 1].flAttributesDamage;
                flDamageDone = flDamageDone * flDamageModifier;
            }
            MonsterLevelTableUnlockR(ptMonsterLevelGBDef);
        }
    }
}


// PowerAPI.cpp:2421
static void sModifyReflectedDamage(Power * ptPower, float & flDamageDone)
{
    if (ptPower->dwFlags & (1 << PF_WAS_REFLECTED))
    {
        float flReflectDamageReductionScalar = FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_PROJECTILE_REFLECT_DAMAGE_SCALAR);
        if (flReflectDamageReductionScalar < 1.0f)
        {
            flDamageDone *= flReflectDamageReductionScalar;
        }
        if (FastAttribGetValueInt(ptPower->idFastAttrib, ATTRIBUTE_PROJECTILE_REFLECT_HAS_DAMAGE_CAP))
        {
            float flReflectRemainingDamageCap = FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_PROJECTILE_REFLECT_DAMAGE_CAP_REMAINING);
            if (flReflectRemainingDamageCap > EPSILON)
            {
                float flPostDamageReduction = Math::Max(flReflectRemainingDamageCap - flDamageDone, 0.0f);
                FastAttribSetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_PROJECTILE_REFLECT_DAMAGE_CAP_REMAINING, flPostDamageReduction);
                flDamageDone = flReflectRemainingDamageCap - flPostDamageReduction;
            }
            else
            {
                flDamageDone = 0.0f;
            }
        }
    }
}


// PowerAPI.cpp:2453
static void sApplyLifeOrManaStealEffects(const PowerID idPower, ActorCommonData & tACDAttacker, ActorCommonData & tACDTarget, float & flDamageDone)
{
    if (tACDTarget.IsWreckableObject())
        return;
    float flStealHitpointPercent = PowerAttributesGetFloat(idPower, ATTRIBUTE_STEAL_HEALTH_PERCENT);
    float flPowerLifeStealPercent = PowerGetFormulaValue(idPower, TAG_POWER_LIFESTEAL_PERCENT);
    flStealHitpointPercent += flPowerLifeStealPercent;
    if (Math::Abs(flStealHitpointPercent) > EPSILON)
    {
        float flHitpointGain = flStealHitpointPercent * flDamageDone;
        if (flHitpointGain > EPSILON)
        {
            ActorHeal(tACDAttacker.id, flHitpointGain, HEAL_TYPE_LIFE_STEAL, PowerGetSNO(idPower), ACDID_INVALID, TRUE);
        }
    }
    float flStealManaPercent = PowerAttributesGetFloat(idPower, ATTRIBUTE_STEAL_MANA_PERCENT);
    if (Math::Abs(flStealManaPercent) > EPSILON)
    {
        float flManaGain = flStealManaPercent * flDamageDone;
        tACDAttacker.AddResource(RESOURCE_TYPE_MANA, flManaGain, FALSE);
        float flOldMana = tACDTarget.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, RESOURCE_TYPE_MANA);
        float flNewMana = Math::Max(flOldMana - flManaGain, 0.0f);
        tACDTarget.ResourceAttributeSetFloat(ATTRIBUTE_RESOURCE_CUR, flNewMana, RESOURCE_TYPE_MANA);
    }
}


// PowerAPI.cpp:2494
static void sQueueCCFloatMessage(
    const ACDID idACDAttacker,
    const ACDID idACDTarget,
    FloatingNumberType eType)
{
    int32 nPriority = 0;
    switch (eType)
    {
    case FLOATING_NUMBER_TYPE_CHARM:
        nPriority = 7;
        break;
    case FLOATING_NUMBER_TYPE_STUN:
        nPriority = 6;
        break;
    case FLOATING_NUMBER_TYPE_BLIND:
        nPriority = 5;
        break;
    case FLOATING_NUMBER_TYPE_FEAR:
        nPriority = 4;
        break;
    case FLOATING_NUMBER_TYPE_ROOT:
        nPriority = 3;
        break;
    case FLOATING_NUMBER_TYPE_FROZEN:
        nPriority = 2;
        break;
    case FLOATING_NUMBER_TYPE_SNARE:
        nPriority = 1;
        break;
    }
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();

    CCInfo * ptCCInfo = tSGameGlobals.tMapPendingCCMessages.LookupPtr(idACDTarget);
    if (ptCCInfo)
    {
        if (ptCCInfo->nCurrentCCPriority < nPriority)
        {
            ptCCInfo->eType = eType;
            ptCCInfo->idACDAttacker = idACDAttacker;
            ptCCInfo->nCurrentCCPriority = nPriority;
        }
    }
    else
    {
        CCInfo tNewCCInfo;
        tNewCCInfo.nCurrentCCPriority = nPriority;
        tNewCCInfo.idACDAttacker = idACDAttacker;
        tNewCCInfo.eType = eType;

        tSGameGlobals.tMapPendingCCMessages.SetAt_Tracked(idACDTarget, tNewCCInfo);
    }
}

// PowerAPI.cpp:2568
static void sApplyCrit(
    PowerID idPower,
    ActorCommonData * ptACDMainActor,
    float & flDamageDone,
    float flPowerCritDamageBonusPercent,
    float flCritCap,
    ActorCommonData & tACDTarget)
{
    float flBonusDamageScalar = PowerComputeCritDamageScalar(ptACDMainActor);
    float flTotal = flBonusDamageScalar + flPowerCritDamageBonusPercent;
    if (ptACDMainActor)
    {
        float flActorCap = ptACDMainActor->AttributesGetFloat(ATTRIBUTE_CRIT_DAMAGE_CAP);
        if (flActorCap > EPSILON)
        {
            flTotal = Math::Min(flActorCap, flTotal);
        }
    }
    float flBonusDamagePercentUncapped = PowerAttributesGetFloat(idPower, ATTRIBUTE_CRIT_DAMAGE_PERCENT_UNCAPPED);
    flTotal += flBonusDamagePercentUncapped;

    flTotal = Math::Min(flCritCap + 1.0f, flTotal);
    flDamageDone *= flTotal;
}

// PowerAPI.cpp:2606
static float sGetCrushingBlowHandicapScalar()
{
    float flScalar = 1.0f;
    int32 nHandicapLevel = GameGetHandicapLevel();
    if (nHandicapLevel >= 0)
    {
        HandicapLevel * ptLevel = GameGetInfoForHandicapLevel(nHandicapLevel);
        if (ptLevel)
        {
            flScalar = ptLevel->tHandicapValues.flCrushingBlowScalar;
        }
    }
    return flScalar;
}


// PowerAPI.cpp:2626
static void sApplyCrushingBlow(
    PowerID idPower,
    float & flDamageDone,
    ACDID idACDAttacker,
    ActorCommonData & tACDTarget)
{
    ScopedGlobalDefinitionLockR pdefGlobal;

    float flCurrentHitpoints = tACDTarget.GetHitPointsCurrent();
    float flCrushingPercent;
    if (tACDTarget.eMonsterRarity == MR_BOSS)
    {
        flCrushingPercent = pdefGlobal->flCrushingBlowDamagePercentBosses;
    }
    else if (tACDTarget.IsElite())
    {
        flCrushingPercent = pdefGlobal->flCrushingBlowDamagePercentElite;
    }
    else
    {
        flCrushingPercent = pdefGlobal->flCrushingBlowDamagePercentNormal;
    }

    float flDamage = flCurrentHitpoints * flCrushingPercent;

    flDamage = AdjustHPForPlayerCount(flDamage, Math::Max(PlayerGetNumPlayersForDifficulty() - 1, 0), 0);

    flDamage *= sGetCrushingBlowHandicapScalar();

    float flAttackSpeedMult = 1.0f;
    if (pdefGlobal->ptServerData &&
        pdefGlobal->ptServerData->flCrushingBlowDamageExponent > EPSILON)
    {
        float flAttackSpeed = 1.0f;
        Power * ptPower = PowerTryToGet(idPower);
        if (ptPower)
        {
            flAttackSpeed = PowerGetAttackSpeed(idPower);
        }

        float flDenom = Math::Pow(flAttackSpeed, pdefGlobal->ptServerData->flCrushingBlowDamageExponent);
        if (flDenom > EPSILON)
        {
            flAttackSpeedMult = 1.0f / flDenom;
        }
    }

    flDamage *= flAttackSpeedMult;

    flDamageDone += flDamage;

    Player * ptPlayer = PlayerGetByACD(idACDAttacker);
    if (ptPlayer && (ptPlayer->dwServerSidePrefFlags & (1 << 3)))
    {
        FloatingNumberMessage tMessage;
        tMessage.annTarget = ANNFromACD(tACDTarget.id);
        tMessage.flDamage = flDamage;
        tMessage.eType = FLOATING_NUMBER_TYPE_CRUSHING_BLOW;

        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(ptPlayer);

        Player * ptPlayerTarget = ACDGetControllingPlayer(tACDTarget.id);
        if (ptPlayerTarget)
        {
            listDestPlayers.AddTail_Tracked(ptPlayerTarget);
        }

        MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));
    }
}


// PowerAPI.cpp:2707
static void sApplySplashDamage(const PowerAPIDamage & tDamage, const float * pflDamageDone, const ActorCommonData & tACDTarget)
{
    if (!pflDamageDone)
        return;
    if (tDamage.eDeliveryMechanism != PDM_THORNS || tDamage.bIsAOEThorns)
    {
        Power * ptPower = PowerTryToGet(tDamage.idPower);
        if (!ptPower)
            return;
        if (tDamage.flProcScalar < EPSILON && !tDamage.bIsAOEThorns)
            return;
        ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
        if (!ptACDOwner)
            return;
        if (!ACDIsHostile(*ptACDOwner, tACDTarget, FALSE) &&
            !tACDTarget.IsWreckableObject())
            return;
        float flSplashDamagePercent = FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_SPLASH_DAMAGE_EFFECT_PERCENT);
        if (flSplashDamagePercent > EPSILON)
        {
            ScopedGlobalDefinitionLockR pdefGlobal;
            float flSplashDamageProcChance = pdefGlobal->flSplashDamageEffectProcChance;
            float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
            if (flRoll <= flSplashDamageProcChance)
            {
                float flTotalDamage = 0.0f;
                for (int32 i = 0; i < NUM_DAMAGE_TYPES; i++)
                {
                    flTotalDamage += flSplashDamagePercent * pflDamageDone[i];
                }
                WorldPlace placeTarget = tACDTarget.GetPlace();
                XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
                PowerAPIChooseTargets tChooseTargets;
                PowerHelper_ChooseTargetsInit(tDamage.idPower, tChooseTargets);
                PowerHelper_ChooseTargetsInitGeometrySphere(placeTarget,
                                                            pdefGlobal->flSplashDamageEffectRadius,
                                                            tChooseTargets.tGeometry);
                PowerAPI_DoChooseTargets(tChooseTargets, listTargets);
                POS pos = listTargets.GetHeadPosition();
                while (pos)
                {
                    POS posOld = pos;
                    ACDID idACDSplashTarget = (ACDID)listTargets.GetNext(pos);
                    ActorCommonData * ptACDSPlashTarget = ACDTryToGet(idACDSplashTarget);
                    if (!ptACDSPlashTarget ||
                        idACDSplashTarget == tACDTarget.id ||
                        (ACDIsFriendly(*ptACDOwner, *ptACDSPlashTarget, FALSE) &&
                         !ptACDSPlashTarget->IsWreckableObject()))
                    {
                        listTargets.RemoveAt(posOld);
                    }
                }
                int32 nTargetsHit = 0;
                int32 nTargetCap = PowerGetMaxAreaEffectTargetCount();
                POS posTarget = listTargets.GetTailPosition();
                while (posTarget)
                {
                    ACDID idACDTarget = (ACDID)listTargets.GetPrev(posTarget);
                    if (nTargetsHit++ >= nTargetCap)
                        break;
                    ActorAccumulateAreaDamage(ptACDOwner->id, idACDTarget, flTotalDamage);
                }
            }
        }
    }
}


// PowerAPI.cpp:2830
static BOOL sApplyStaggerDamage(ActorCommonData & tACDTarget, float * pflDamageDone)
{
    Actor * ptTargetActor = ActorTryToGet((ActorID)tACDTarget.idOwner);
    if (!pflDamageDone || !ptTargetActor)
        return FALSE;
    const float flStaggerDamageFraction = tACDTarget.AttributesGetFloat(ATTRIBUTE_STAGGERED_DAMAGE_FRACTION);
    const float flStaggerDuration = tACDTarget.AttributesGetFloat(ATTRIBUTE_STAGGERED_DAMAGE_DURATION);
    if (flStaggerDamageFraction > EPSILON && flStaggerDuration > EPSILON)
    {
        float flStaggerHitPointDamage = 0.0f;
        for (int32 nDamageType = 0; nDamageType < NUM_DAMAGE_TYPES; nDamageType++)
        {
            float flStaggerDamagePerDamageType = flStaggerDamageFraction * pflDamageDone[nDamageType];
            flStaggerHitPointDamage += flStaggerDamagePerDamageType;
            pflDamageDone[nDamageType] -= flStaggerDamagePerDamageType;
        }
        if (!ptTargetActor->ptStaggerDamageInfo)
        {
            ptTargetActor->ptStaggerDamageInfo = new StaggerDamageInfo;
        }
        StaggerDamageInfo * ptStaggerDamageInfo = ptTargetActor->ptStaggerDamageInfo;
        ptStaggerDamageInfo->flStaggeredDamageDuration = flStaggerDuration;
        int32 nStaggerBuckets = (int32)(flStaggerDuration * TICKS_PER_SECOND / 12.0f);
        int32 nCurrentNumStaggerBuckets = (int32)ptStaggerDamageInfo->arStaggeredDamageBuckets.size();
        if (nCurrentNumStaggerBuckets == 0)
        {
            for (int32 i = 0; i < nStaggerBuckets; i++)
            {
                ptStaggerDamageInfo->arStaggeredDamageBuckets.push_back(0.0f);
            }
            uint32 uNow = GameGetServerTime();
            ptStaggerDamageInfo->uStaggeredDamageLastTickCheck = uNow;
        }
        else if (nCurrentNumStaggerBuckets != nStaggerBuckets)
        {
            float flTotalOldDamage = 0.0f;
            for (int32 i = 0; i < nCurrentNumStaggerBuckets; i++)
            {
                flTotalOldDamage += ptStaggerDamageInfo->arStaggeredDamageBuckets[i];
            }
            ptStaggerDamageInfo->arStaggeredDamageBuckets.clear();
            float flOldDamagePerBucket = flTotalOldDamage / nStaggerBuckets;
            for (int32 i = 0; i < nStaggerBuckets; i++)
            {
                ptStaggerDamageInfo->arStaggeredDamageBuckets.push_back(flOldDamagePerBucket);
            }
            uint32 uNow = GameGetServerTime();
            ptStaggerDamageInfo->uStaggeredDamageLastTickCheck = uNow;
        }
        float flDamagePerBucket = flStaggerHitPointDamage / nStaggerBuckets;
        for (int32 i = 0; i < nStaggerBuckets; i++)
        {
            ptStaggerDamageInfo->arStaggeredDamageBuckets[i] += flDamagePerBucket;
        }
    }
    return TRUE;
}


// PowerAPI.cpp:2905
static void sConvertDamageToDOT(
    const PowerAPIAttackPayload & tPayload,
    ActorCommonData & tACDTarget,
    const PowerAPIDamage & tPowerDamage,
    const DamageType eDamageType,
    float & flDamageDone)
{
    Power * ptPower = PowerTryToGet(tPayload.idPower);
    if (ptPower)
    {
        float flDuration = tPayload.flDamageDurationInSecs[eDamageType];

        BuffID idBuffExisting = BUFFID_INVALID;

        if (tPayload.nDOTMaxStackCount == 1)
        {
            if (tPayload.eDOTStackType == 7 || tPayload.eDOTStackType == 2)
            {
                idBuffExisting = PowerAPI_BuffFindOnACD(PowerGetSNO(tPayload.idPower), tACDTarget, PowerGetOwner(tPayload.idPower)->id);
            }
            else
            {
                idBuffExisting = PowerAPI_BuffFindOnACD(PowerGetSNO(tPayload.idPower), tACDTarget, ACDID_INVALID);
            }
        }

        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuffExisting);
        if (ptBuff)
        {
            uint32 dwNow = GameGetServerTime();
            uint32 dwBuffEndTime = ptBuff->dwStartTick + ptBuff->dwDuration;
            float flTimeLeft = 0.0f;
            if (dwBuffEndTime > dwNow)
            {
                flTimeLeft = (float)(dwBuffEndTime - dwNow) * OO_TICKS_PER_SECOND;
            }

            float flDamageLeft = flTimeLeft * ptBuff->flDamagePerSecond;
            if (flDamageLeft > flDamageDone)
            {
                return;
            }
        }

        PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
        tBuffStartData.idPower = tPayload.idPower;
        tBuffStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_DOT_DEBUFF);
        tBuffStartData.bPositiveEffect = FALSE;
        tBuffStartData.nBuffIndex = 0;
        tBuffStartData.eStackType = tPayload.eDOTStackType;
        tBuffStartData.nMaxStackCount = tPayload.nDOTMaxStackCount;
        tBuffStartData.eDamageType = eDamageType;
        tBuffStartData.flDamagePerSecond = flDamageDone / flDuration;
        tBuffStartData.flPrimaryAttributeBonusMultiplier = tPayload.flPrimaryAttributeBonusMultiplier;
        tBuffStartData.flClassBonusMultiplier = tPayload.flClassBonusMultiplier;
        tBuffStartData.flSkillsBonusMultiplier = tPayload.flSkillsBonusMultiplier;
        tBuffStartData.dwDurationMin = Math::FloatToInt(flDuration * TICKS_PER_SECOND);
        tBuffStartData.idACDSource = tPowerDamage.idACDSource;
        tBuffStartData.snoDOTDamageSourcePower = PowerGetSNO(tPayload.idPower);
        tBuffStartData.bAppliedDamageReductionEffects = TRUE;
        tBuffStartData.bDOTCrit = tPowerDamage.bCriticalHit;
        tBuffStartData.flProcScalar = tPowerDamage.flProcScalar;
        tBuffStartData.bUsePowersProcScalar = FALSE;
        tBuffStartData.bTriggersHitProcs = tPayload.bTriggersHitProcs;

        if (tPayload.pDOTBuffStartCallback)
        {
            tPayload.pDOTBuffStartCallback(tPayload.idPower, tACDTarget.id, tPayload.pDOTBuffStartData, tPowerDamage, tBuffStartData);
        }

        PowerAPI_BuffAddToACD(tBuffStartData, tACDTarget);
    }
}


// PowerAPI.cpp:2988
static void sSendImmuneMessage(ACDID idACDTarget, PowerID idPower)
{
    ActorCommonData * ptACDAttacker = PowerGetOwner(idPower);
    if (!ptACDAttacker)
        return;
    Player * ptPlayer = PlayerGetByACD(ptACDAttacker->id);
    if (ptPlayer)
    {
        FloatingNumberMessage tMessage;
        tMessage.annTarget = ANNFromACD(idACDTarget);
        tMessage.flDamage = 0.0f;
        tMessage.eType = FLOATING_NUMBER_TYPE_IMMUNE;
        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(ptPlayer);
        MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));
    }
}

// PowerAPI.cpp:3016
static void sApplyChill(const PowerID idPowerFrom, float flProcScalar, const int32 nChillDuration, const float flChillMagnitude, ActorCommonData & tACDTarget)
{
    int32 nChillBuffDuration = nChillDuration;
    float flChillBuffMagnitude = flChillMagnitude;
    SNO snoBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_CHILLED);
    if (nChillBuffDuration == 0)
    {
        ScopedGlobalDefinitionLockR pdefGlobal;
        float flValue = pdefGlobal->ptServerData->flChillDefaultDuration;
        if (ACDIsPlayer(tACDTarget.id))
            flValue = pdefGlobal->ptServerData->flChillDefaultDurationPlayer;
        uint32 dwDefaultChillDuration = (uint32)(flValue * TICKS_PER_SECOND * (tACDTarget.AttributesGetFloat(ATTRIBUTE_CHILLED_DURATION_BONUS_PERCENT) + 1.0f));
        nChillBuffDuration = dwDefaultChillDuration;
    }
    if (flChillBuffMagnitude < EPSILON)
    {
        ScopedGlobalDefinitionLockR pdefGlobal;
        flChillBuffMagnitude = pdefGlobal->ptServerData->flChillDefaultMagnitude;
        if (ACDIsPlayer(tACDTarget.id))
            flChillBuffMagnitude = pdefGlobal->ptServerData->flChillDefaultMagnitudePlayer;
    }
    PowerAPIBuffStartData tBuffStartData(idPowerFrom);
    ActorCommonData * ptACDMainActor = PowerGetOwner(idPowerFrom);
    if (ptACDMainActor)
    {
        tBuffStartData.idACDSource = ptACDMainActor->id;
    }
    else
    {
        tBuffStartData.idACDSource = ACDID_INVALID;
    }
    tBuffStartData.snoPower = snoBuffPower;
    tBuffStartData.dwDurationMin = nChillBuffDuration;
    tBuffStartData.bPositiveEffect = FALSE;
    tBuffStartData.eStackType = 4;
    tBuffStartData.nMaxStackCount = 1;
    tBuffStartData.bChill = TRUE;
    tBuffStartData.bFailsIfChillImmune = TRUE;
    tBuffStartData.flChillMagnitude = flChillBuffMagnitude;
    tBuffStartData.nBuffIndex = 0;
    tBuffStartData.flProcScalar = flProcScalar;
    tBuffStartData.bUsePowersProcScalar = FALSE;
    PowerAPI_BuffAddToACD(tBuffStartData, tACDTarget);
}

// PowerAPI.cpp:3081
static void sApplyChillEffects(const PowerAPIAttackPayload & tPayload, ActorCommonData & tACDTarget, PowerAPIDamage & tPowerDamage)
{
    tPowerDamage.bWasChill = tPayload.bChill;
    if (tPayload.bChill)
    {
        int32 nChillLength = (int32)RandRangeFloat_Internal(SGameGlobalsGet().rng, tPayload.rangeChillLength.flMin, tPayload.rangeChillLength.flDelta);
        sApplyChill(tPayload.idPower, tPayload.flProcScalar, nChillLength, tPayload.flChillMagnitude, tACDTarget);
    }
}


// PowerAPI.cpp:3098
static void sApplyStun(const PowerID idPowerFrom, float flProcScalar, const int32 nStunLength, ActorCommonData & tACDTarget)
{
    if (nStunLength > 0)
    {
        SNO snoBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_STUNNED);
        PowerAPIBuffStartData tBuffStartData(idPowerFrom);
        ActorCommonData * ptACDMainActor = PowerGetOwner(idPowerFrom);
        if (ptACDMainActor)
        {
            tBuffStartData.idACDSource = ptACDMainActor->id;
        }
        else
        {
            tBuffStartData.idACDSource = ACDID_INVALID;
        }
        tBuffStartData.snoPower = snoBuffPower;
        tBuffStartData.dwDurationMin = nStunLength;
        tBuffStartData.bPositiveEffect = FALSE;
        tBuffStartData.eStackType = 3;
        tBuffStartData.bStun = TRUE;
        tBuffStartData.bFailsIfStunImmune = TRUE;
        tBuffStartData.nBuffIndex = 0;
        tBuffStartData.flProcScalar = flProcScalar;
        tBuffStartData.bUsePowersProcScalar = FALSE;
        BuffID idBuff = PowerAPI_BuffAddToACD(tBuffStartData, tACDTarget);
        ActorID idActor = (ActorID)ACDGetOwner(tACDTarget.id);
        Actor * ptActor = ActorGet(idActor);
        if (idBuff != BUFFID_INVALID && ptActor->ptSActorMovement && tACDTarget.IsAlive() && !ptActor->ptSActorMovement->IsInArcingMovement())
            ptActor->ptSActorMovement->StopAndSyncPosition(TRUE, FALSE);
    }
}

// PowerAPI.cpp:3142
static void sApplyStunEffects(const PowerAPIAttackPayload & tPayload, ActorCommonData & tACDTarget, PowerAPIDamage & tPowerDamage)
{
    if (tPayload.rangeStunLength.flMin < 1.0f)
    {
        return;
    }

    BOOL bForcedStun = FALSE;

    if (!bForcedStun)
    {
        if (RandFloat_Internal(SGameGlobalsGet().rng) > tPayload.flChanceToStun)
        {
            return;
        }
    }

    int32 nStunLength = (int32)RandRangeFloat_Internal(SGameGlobalsGet().rng, tPayload.rangeStunLength.flMin, tPayload.rangeStunLength.flDelta);
    if (nStunLength > 0)
    {
        sApplyStun(tPayload.idPower, tPayload.flProcScalar, nStunLength, tACDTarget);
    }
}


// PowerAPI.cpp:3192
static void sApplyWebbedEffect(const PowerAPIAttackPayload & tPayload, ActorCommonData & tACDTarget, PowerAPIDamage & tPowerDamage)
{
    const float flWebMax = tPayload.rangeWebLength.flMin + tPayload.rangeWebLength.flDelta;
    if (flWebMax > 1.0f)
    {
        if (tACDTarget.AttributesGetInt(ATTRIBUTE_SLOWDOWN_IMMUNE))
        {
            sSendImmuneMessage(tACDTarget.id, tPayload.idPower);
            return;
        }

        SNO snoBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_SLOWED);
        float flWebAmount = 0.45f;
        const int32 nWebLength = (int32)RandRangeFloat_Internal(SGameGlobalsGet().rng, tPayload.rangeWebLength.flMin, tPayload.rangeWebLength.flDelta);

        PowerAPIBuffStartData tBuffStartData(tPayload.idPower);
        tBuffStartData.dwDurationMin = nWebLength;
        tBuffStartData.bPositiveEffect = FALSE;
        tBuffStartData.eStackType = 2;
        tBuffStartData.nMaxStackCount = 1024;
        tBuffStartData.snoPower = snoBuffPower;
        tBuffStartData.nBuffIndex = 0;

        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT, flWebAmount);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_WEBBED, 1);
        PowerAPI_BuffAddToACD(tBuffStartData, tACDTarget);
    }
}


// PowerAPI.cpp:3228
static void sApplyImmobilize(const PowerID idPowerFrom, float flProcScalar, const int32 nImmobilizeDuration, ActorCommonData & tACDTarget)
{
    if (nImmobilizeDuration > 0)
    {
        SNO snoBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_ROOTED);
        PowerAPIBuffStartData tBuffStartData(idPowerFrom);
        tBuffStartData.dwDurationMin = nImmobilizeDuration;
        tBuffStartData.bPositiveEffect = FALSE;
        tBuffStartData.eStackType = 3;
        tBuffStartData.snoPower = snoBuffPower;
        tBuffStartData.bImmobilize = TRUE;
        tBuffStartData.bFailsIfImmobilizeImmune = TRUE;
        tBuffStartData.nBuffIndex = 0;
        tBuffStartData.flProcScalar = flProcScalar;
        tBuffStartData.bUsePowersProcScalar = FALSE;
        BuffID idBuff = PowerAPI_BuffAddToACD(tBuffStartData, tACDTarget);
        ActorID idActor = (ActorID)ACDGetOwner(tACDTarget.id);
        Actor * ptActor = ActorGet(idActor);
        if (idBuff != BUFFID_INVALID && ptActor->ptSActorMovement && tACDTarget.IsAlive())
            ptActor->ptSActorMovement->StopAndSyncPosition(TRUE, FALSE);
    }
}

// PowerAPI.cpp:3262
static void sApplyImmobilizeEffect(const PowerAPIAttackPayload & tPayload, ActorCommonData & tACDTarget, PowerAPIDamage & tPowerDamage)
{
    const float flImmobolizeMax = tPayload.rangeImmobolizeLength.flMin + tPayload.rangeImmobolizeLength.flDelta;
    if (flImmobolizeMax > 0.0f)
    {
        const int32 nImmobolizeLength = (int32)RandRangeFloat_Internal(SGameGlobalsGet().rng, tPayload.rangeImmobolizeLength.flMin, tPayload.rangeImmobolizeLength.flDelta);
        sApplyImmobilize(tPayload.idPower, tPayload.flProcScalar, nImmobolizeLength, tACDTarget);
    }
}


// PowerAPI.cpp:3279
static void sApplySlow(const PowerID idPowerFrom, float flProcScalar, const int32 nSlowDuration, const float flSlowAmount, ActorCommonData & tACDTarget)
{
    if (nSlowDuration > 0)
    {
        SNO snoBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_SLOWED);
        PowerAPIBuffStartData tBuffStartData(idPowerFrom);
        ActorCommonData * ptACDMainActor = PowerGetOwner(idPowerFrom);
        if (ptACDMainActor)
        {
            tBuffStartData.idACDSource = ptACDMainActor->id;
        }
        else
        {
            tBuffStartData.idACDSource = ACDID_INVALID;
        }
        tBuffStartData.dwDurationMin = nSlowDuration;
        tBuffStartData.bPositiveEffect = FALSE;
        tBuffStartData.eStackType = 2;
        tBuffStartData.nMaxStackCount = 1024;
        tBuffStartData.snoPower = snoBuffPower;
        tBuffStartData.bSlow = TRUE;
        tBuffStartData.bFailsIfSlowImmune = TRUE;
        tBuffStartData.flSlowAmount = flSlowAmount;
        tBuffStartData.nBuffIndex = 0;
        tBuffStartData.flProcScalar = flProcScalar;
        tBuffStartData.bUsePowersProcScalar = FALSE;
        PowerAPI_BuffAddToACD(tBuffStartData, tACDTarget);
    }
}


// PowerAPI.cpp:3318
static void sApplySlowEffect(const PowerAPIAttackPayload & tPayload, ActorCommonData & tACDTarget, PowerAPIDamage & tPowerDamage)
{
    const float flSlowMax = tPayload.rangeSlowLength.flMin + tPayload.rangeSlowLength.flDelta;
    if (flSlowMax > 1.0f)
    {
        const int32 nSlowLength = (int32)RandRangeFloat_Internal(SGameGlobalsGet().rng, tPayload.rangeSlowLength.flMin, tPayload.rangeSlowLength.flDelta);
        sApplySlow(tPayload.idPower, tPayload.flProcScalar, nSlowLength, tPayload.flSlowAmount, tACDTarget);
    }
}


// PowerAPI.cpp:3336
static void sApplyManaDrainEffect(const PowerAPIAttackPayload & tPayload, PowerAPIDamage & tPowerDamage)
{
    const float flManaDrainMax = tPayload.rangeManaDrain.flMin + tPayload.rangeManaDrain.flDelta;
    if (flManaDrainMax > 1.0f)
    {
        const float flManaDrainAmount = RandRangeFloat_Internal(SGameGlobalsGet().rng, tPayload.rangeManaDrain.flMin, tPayload.rangeManaDrain.flDelta);

        PowerHelper_DamageAdd(tPowerDamage, MakeAttribKey(ATTRIBUTE_RESOURCE_CUR, 0), -flManaDrainAmount, 0);
    }
}


// PowerAPI.cpp:3351
static void sApplyFear(const PowerID idPowerFrom, float flProcScalar, const int32 nFearDuration, ActorCommonData & tACDTarget)
{
    if (nFearDuration > 0)
    {
        SNO snoBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_FEARED);
        PowerAPIBuffStartData tBuffStartData(idPowerFrom);
        ActorCommonData * ptACDMainActor = PowerGetOwner(idPowerFrom);
        if (ptACDMainActor)
        {
            tBuffStartData.idACDSource = ptACDMainActor->id;
        }
        else
        {
            tBuffStartData.idACDSource = ACDID_INVALID;
        }
        tBuffStartData.bPositiveEffect = FALSE;
        tBuffStartData.eStackType = 3;
        tBuffStartData.nMaxStackCount = 1;
        tBuffStartData.dwDurationMin = nFearDuration;
        tBuffStartData.snoPower = snoBuffPower;
        tBuffStartData.bFear = TRUE;
        tBuffStartData.bFailsIfFearImmune = TRUE;
        tBuffStartData.nBuffIndex = 0;
        tBuffStartData.flProcScalar = flProcScalar;
        tBuffStartData.bUsePowersProcScalar = FALSE;
        PowerAPI_BuffAddToACD(tBuffStartData, tACDTarget);
    }
}


// PowerAPI.cpp:3388
static void sApplyFearEffects(const PowerAPIAttackPayload & tPayload, ActorCommonData & tACDTarget, PowerAPIDamage & tPowerDamage)
{
    if (tPayload.rangeFearLength.flMin < 1.0f)
    {
        return;
    }

    int32 nFearLength = (int32)RandRangeFloat_Internal(SGameGlobalsGet().rng, tPayload.rangeFearLength.flMin, tPayload.rangeFearLength.flDelta);
    if (nFearLength > 0)
    {
        if (tPayload.idPower != POWERID_INVALID)
        {
            sApplyFear(tPayload.idPower, tPayload.flProcScalar, nFearLength, tACDTarget);
        }
    }
}


// PowerAPI.cpp:3489
static void sApplyFreeze(const PowerID idPowerFrom, float flProcScalar, const int32 nFreezeDuration, ActorCommonData & tACDTarget)
{
    if (nFreezeDuration > 0)
    {
        PowerAPIBuffStartData tBuffStartData(idPowerFrom);
        tBuffStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_CRIT_DEBUFF_COLD);
        ActorCommonData * ptACDMainActor = PowerGetOwner(idPowerFrom);
        if (ptACDMainActor)
        {
            tBuffStartData.idACDSource = ptACDMainActor->id;
        }
        else
        {
            tBuffStartData.idACDSource = ACDID_INVALID;
        }
        tBuffStartData.bPositiveEffect = FALSE;
        tBuffStartData.eStackType = 3;
        tBuffStartData.nMaxStackCount = 1;
        tBuffStartData.dwDurationMin = nFreezeDuration;
        tBuffStartData.bFreeze = TRUE;
        tBuffStartData.bFailsIfFreezeImmune = TRUE;
        tBuffStartData.nBuffIndex = 0;
        tBuffStartData.flProcScalar = flProcScalar;
        tBuffStartData.bUsePowersProcScalar = FALSE;
        BuffID idBuff = PowerAPI_BuffAddToACD(tBuffStartData, tACDTarget);
        ActorID idActor = (ActorID)ACDGetOwner(tACDTarget.id);
        Actor * ptActor = ActorGet(idActor);
        if (idBuff != BUFFID_INVALID && ptActor->ptSActorMovement && !ptActor->ptSActorMovement->IsInArcingMovement() && tACDTarget.IsAlive())
            ptActor->ptSActorMovement->StopAndSyncPosition(TRUE, FALSE);
    }
}

// PowerAPI.cpp:3539
static void sApplyFreezeEffects(const PowerAPIAttackPayload & tPayload, ActorCommonData & tACDTarget, PowerAPIDamage & tPowerDamage)
{
    if (tPayload.rangeFreezeLength.flMin < EPSILON)
    {
        return;
    }

    int32 dwDuration = (int32)RandRangeFloat_Internal(SGameGlobalsGet().rng, tPayload.rangeFreezeLength.flMin, tPayload.rangeFreezeLength.flDelta);

    if (dwDuration > 0)
    {
        sApplyFreeze(tPayload.idPower, tPayload.flProcScalar, dwDuration, tACDTarget);
    }
}


// PowerAPI.cpp:3586
// bBlindBreaksOnDamage is unused in both inlined copies of the original (CheckForOnHitProcs with FALSE, DoPayload with tPayload.bBlindBreaksOnDamage).
static void sApplyBlind(const PowerID idPowerFrom, float flProcScalar, const int32 nBlindDuration, ActorCommonData & tACDTarget, const BOOL bBlindBreaksOnDamage)
{
    if (nBlindDuration > 0)
    {
        SNO snoBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_DEBUFF_BLIND);
        PowerAPIBuffStartData tBuffStartData(idPowerFrom);
        ActorCommonData * ptACDMainActor = PowerGetOwner(idPowerFrom);
        if (ptACDMainActor)
        {
            tBuffStartData.idACDSource = ptACDMainActor->id;
        }
        else
        {
            tBuffStartData.idACDSource = ACDID_INVALID;
        }
        tBuffStartData.dwDurationMin = nBlindDuration;
        tBuffStartData.bPositiveEffect = FALSE;
        tBuffStartData.eStackType = 3;
        tBuffStartData.snoPower = snoBuffPower;
        tBuffStartData.bBlind = TRUE;
        tBuffStartData.bFailsIfBlindImmune = TRUE;
        tBuffStartData.nBuffIndex = 0;
        tBuffStartData.flProcScalar = flProcScalar;
        tBuffStartData.bUsePowersProcScalar = FALSE;
        PowerAPI_BuffAddToACD(tBuffStartData, tACDTarget);
    }
}

// PowerAPI.cpp:3631
static void sApplyBlindEffect(const PowerAPIAttackPayload & tPayload, ActorCommonData & tACDTarget, PowerAPIDamage & tPowerDamage)
{
    const float flBlindMax = tPayload.rangeBlindLength.flMin + tPayload.rangeBlindLength.flDelta;
    if (flBlindMax > 1.0f)
    {
        const int32 nBlindLength = (int32)RandRangeFloat_Internal(SGameGlobalsGet().rng, tPayload.rangeBlindLength.flMin, tPayload.rangeBlindLength.flDelta);
        if (nBlindLength > 0)
        {
            sApplyBlind(tPayload.idPower, tPayload.flProcScalar, nBlindLength, tACDTarget, tPayload.bBlindBreaksOnDamage);
        }
    }
}


// PowerAPI.cpp:3652
inline float sGetLastRecoilUpdateTimeDifference(ACDID idACDTarget, uint32 dwTime)
{
    ActorID idActor = (ActorID)ACDGetOwner(idACDTarget);
    const Actor * ptActor = ActorGet(idActor);
    if (ptActor->dwTimeSinceLastRecoilUpdate == (uint32)-1)
        return 0.0f;
    return (float)(dwTime - ptActor->dwTimeSinceLastRecoilUpdate) / TICKS_PER_SECOND;
}

// PowerAPI.cpp:3670
inline void sSetLastRecoilUpdateTime(ACDID idACDTarget, uint32 dwTime)
{
    ActorID idActor = (ActorID)ACDGetOwner(idACDTarget);
    Actor * ptActor = ActorGet(idActor);
    ptActor->dwTimeSinceLastRecoilUpdate = dwTime;
}

// PowerAPI.cpp:3683
void sUpdateRecoil(ACDID idACDTarget)
{
    uint32 dwNow = GameGetServerTime();
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    float flSecondsSinceLastUpdate = sGetLastRecoilUpdateTimeDifference(idACDTarget, dwNow);
    float flGetHitCurr = tACDTarget.AttributesGetFloat(ATTRIBUTE_GETHIT_CURRENT);
    float flGetHitRecovery = tACDTarget.AttributesGetFloat(ATTRIBUTE_GETHIT_RECOVERY);
    float flNewGetHitAmount = Math::Max(flGetHitCurr - flSecondsSinceLastUpdate * flGetHitRecovery, 0.0f);
    sSetLastRecoilUpdateTime(idACDTarget, dwNow);
    tACDTarget.AttributesSetFloat(ATTRIBUTE_GETHIT_CURRENT, flNewGetHitAmount);
}

// PowerAPI.cpp:3703
static void sUpdateCombatStatus(ActorCommonData & tACDAttacker, ActorCommonData & tACDTarget)
{
    if (ACDIsPlayer(tACDAttacker.id) && ACDIsPlayer(tACDTarget.id))
    {
        if (HeroIsAwareOfPlayer(tACDAttacker.id, tACDTarget.id))
        {
            tACDTarget.SetCombatStatus(TRUE);
            tACDTarget.SetCombatWithPlayerStatus(TRUE);
        }
    }
    else
    {
        tACDTarget.SetCombatStatus(TRUE);
    }
    if (!tACDTarget.IsWreckableObject() && (tACDTarget.IsMonster() || ACDIsHostile(tACDAttacker, tACDTarget, FALSE)))
    {
        tACDAttacker.SetCombatStatus(TRUE);
        if (tACDAttacker.IsPet())
        {
            Player * ptPlayer = tACDAttacker.GetPetOwnerPlayer();
            if (ptPlayer)
            {
                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACDPlayer)
                    ptACDPlayer->SetCombatStatus(TRUE);
            }
        }
    }
}

// PowerAPI.cpp:3832
static void sApplyDamageReduction(ActorCommonData & tACDTarget, float * flDamageDone, SNO snoPower)
{
    uint32 dwNow = GameGetServerTime();

    for (int32 i = 0; i < 7; i++)
    {
        float flDamage = flDamageDone[i];
        if (flDamage <= EPSILON)
        {
            continue;
        }

        float flTotal = tACDTarget.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_REDUCTION_TOTAL, i));
        if (flTotal <= EPSILON)
        {
            continue;
        }

        float flCurrent = tACDTarget.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_REDUCTION_CURRENT, i));
        if (flCurrent < flTotal)
        {
            uint32 dwLastTime = Math::Min(dwNow, (uint32)tACDTarget.AttributesGetInt(MakeAttribKey(ATTRIBUTE_DAMAGE_REDUCTION_LAST_TICK, i)));

            int32 nTicksSinceLast = dwNow - dwLastTime;
            if (nTicksSinceLast >= 30)
            {
                flCurrent = flTotal;
            }
            else
            {
                flCurrent = Math::Min(flTotal, flCurrent + flTotal * nTicksSinceLast * (1.0f / 30.0f));
            }
        }

        tACDTarget.AttributesSetInt(MakeAttribKey(ATTRIBUTE_DAMAGE_REDUCTION_LAST_TICK, i), dwNow);

        float flOrigDamage = flDamage;

        flCurrent -= flDamage;
        if (flCurrent < 0.0f)
        {
            flDamage = -flCurrent;
            flCurrent = 0.0f;
        }
        else
        {
            flDamage = 0.0f;
        }

        CombatLog::LogEvent(5,
            ACDID_INVALID,
            tACDTarget.id,
            snoPower, flOrigDamage, flDamage, 0);

        tACDTarget.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_REDUCTION_CURRENT, i), flCurrent);
        flDamageDone[i] = flDamage;
    }
}


// PowerAPI.cpp:3903
BOOL sCheckAndApplyShatter(PowerID idPower, const ActorCommonData & tACDTarget, float * flDamageDone)
{
    if (tACDTarget.IsElite())
        return FALSE;
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
        return FALSE;
    float flChance = FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_SHATTER_CHANCE);
    if (flChance > EPSILON && tACDTarget.AttributesGetInt(ATTRIBUTE_FROZEN) > 0)
    {
        float flProcCoefficient = PowerGetProcScalar(*ptPower);
        if (flProcCoefficient > EPSILON)
        {
            flChance *= flProcCoefficient;
            float flHPUnitVal = tACDTarget.GetHitpointPercent();
            flHPUnitVal *= flHPUnitVal;
            flChance *= Math::Pin(1.0f - flHPUnitVal, 0.0f, 1.0f);
            if (!tACDTarget.IsElite())
            {
                ScopedGlobalDefinitionLockR pdefGlobal;
                if (pdefGlobal->ptServerData)
                    flChance *= pdefGlobal->ptServerData->flShatterMultiplierNonElites;
            }
            float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
            if (flRoll <= flChance)
            {
                float flHP = tACDTarget.GetHitPointsCurrent();
                flDamageDone[DAMAGE_TYPE_COLD] = flHP;
                return TRUE;
            }
        }
    }
    return FALSE;
}

// PowerAPI.cpp:4137
// inferred (line 4440/4441): one condition split over two lines; 4440 has the && at col 48 right after PDM_DOT, 4441 starts with
// !tPayload.bSplashDamage (member col 19, && col 33). The order of the remaining operands is the one that reproduces the
// original branch order (clang emits the cheap tests of this chain in reverse source order).
// ptActor (the attacker's Actor) is what the easy-kill and
// no-damage-range checks test. The value of GetDPSValue at 4446 is unused in the original too.

BOOL PowerAPI_DoPayload(
    const PowerAPIAttackPayload & tPayload,
    const ACDID idACDTarget,
    const ACDID idACDOverrideAttacker)
{
    PowerID idPower = tPayload.idPower;
    Power * ptPower = PowerTryToGet(idPower);

    ActorCommonData * ptACDMainActor = PowerGetOwner(idPower);
    ActorCommonData * ptACDOverrideAttacker = ACDTryToGet(idACDOverrideAttacker);
    if (ptACDOverrideAttacker)
    {
        ptACDMainActor = ptACDOverrideAttacker;
    }

    ACDID idACDMainActor = ptACDMainActor ? ptACDMainActor->id : ACDID_INVALID;
    const Actor * ptActor = NULL;

    if (tPayload.snoTargetAttackedParticle != SNO_NONE)
    {
        ACDPlayEffect(idACDTarget, EFFECT_CREATE_PARTICLE, tPayload.snoTargetAttackedParticle, idACDMainActor);
    }

    if (ptACDMainActor)
    {
        ptActor = ActorGet((ActorID)ptACDMainActor->idOwner);
    }

    if (ptPower)
    {
        ptPower->nPayloadCount++;
    }

    if (ptACDMainActor && tPayload.bTriggersHitProcs)
    {
        PowerAPI_DoFirstHitEffects(ptACDMainActor->id, idACDTarget, ptPower);
    }

    if (!ACDTryToGet(idACDTarget))
    {
        return FALSE;
    }

    ActorCommonData & tACDTarget = ACDGet(idACDTarget);

    if (ptACDMainActor)
    {
        sUpdateCombatStatus(*ptACDMainActor, tACDTarget);
    }

    if (!sPayloadIsUnavoidable(tPayload))
    {
        if (PowerCheckToDodge(tPayload, tACDTarget, ptACDMainActor))
        {
            sPowerAPI_Dodged(tPayload, tACDTarget);
            sApplyOnDodgeBuffs(idACDMainActor, idACDTarget, tPayload);

            if (ptACDMainActor &&
                (tPayload.eDeliveryMechanism == PDM_MELEE || tPayload.eDeliveryMechanism == PDM_RANGED) &&
                tPayload.bTriggersHitProcs)
            {
                float flDamageDone[7];
                SigmaMemoryFill(flDamageDone, 0, sizeof(flDamageDone));
                float flPowerProcScalar = ptPower ? PowerGetProcScalar(*ptPower) : 0.0f;
                sApplyThorns(flPowerProcScalar, flDamageDone, *ptACDMainActor, tACDTarget, ptPower, FALSE);
            }

            return FALSE;
        }

        if (!PowerCheckToHit(tPayload, tACDTarget))
        {
            sPowerAPI_Missed(tPayload, tACDTarget);
            return FALSE;
        }
    }

    const Actor * ptTargetActor = ActorTryToGet((ActorID)ACDGetOwner(idACDTarget));
    if (ActorFlagIsSet(ptTargetActor->id, ACTOR_GOD_MODE_BIT))
    {
        return TRUE;
    }

    if (ActorFlagIsSet(ptTargetActor->id, ACTOR_DEACTIVATING))
    {
        return TRUE;
    }

    if (tACDTarget.AttributesGetInt(ATTRIBUTE_INVULNERABLE))
    {
        CombatLog::LogEvent(3, idACDMainActor, idACDTarget,
            ptPower ? ptPower->snoPower : SNO_NONE, 0.0f, 0.0f, 0);

        sSendImmuneMessage(idACDTarget, idPower);
        return TRUE;
    }

    if (ptACDMainActor &&
        ptACDMainActor->AttributesGetInt(ATTRIBUTE_DOES_NO_DAMAGE) > 0)
    {
        return TRUE;
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_PVP_GAME_OVER_BIT))
    {
        return TRUE;
    }

    if (ptPower)
    {
        if (FastAttribGetValueInt(ptPower->idFastAttrib, ATTRIBUTE_DOES_NO_DAMAGE) > 0)
        {
            return TRUE;
        }
    }

    BOOL fConvertsDamageToDOT = FALSE;
    if (tACDTarget.IsAlive())
    {
        fConvertsDamageToDOT = !tACDTarget.ConvertsDamageToDamageState();
    }

    PowerAPIDamage tPowerDamage;
    PowerHelper_DamageInit(tPowerDamage, idPower, idACDTarget);
    tPowerDamage.eDeliveryMechanism = tPayload.eDeliveryMechanism;
    tPowerDamage.idACDFloatingCombatTextSourceOverride = tPayload.idACDFloatingCombatTextSourceOverride;
    tPowerDamage.flProcScalar = tPayload.flProcScalar;
    if (ptACDMainActor)
    {
        tPowerDamage.idACDMainActor = ptACDMainActor->id;
    }

    if (tPayload.bSplashDamage)
    {
        tPowerDamage.snoAltDamageSourcePower = GlobalSNOGet(GLOBALSNO_POWERS_SPLASH_DAMAGE_PROC);
        tPowerDamage.bSplashDamage = TRUE;
    }

    float flPowerAdditionalChanceToCriticalHit = tPayload.flPowerAdditionalChanceToCriticalHit + FastAttribGetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_CRIT_PERCENT_BONUS, ptPower->snoPower));
    tPowerDamage.bCriticalHit = sCheckForCrit(tPayload.idPower, tPayload.bCanCrit, flPowerAdditionalChanceToCriticalHit, tPayload.flCritCap, tACDTarget, ptACDMainActor);

    float flPowerAdditionalChanceToCrushingBlow = tPayload.flPowerAdditionalChanceToCrushingBlow;
    tPowerDamage.bCrushingBlow = FALSE;

    if (ptACDMainActor)
    {
        tPowerDamage.idACDSource = ptACDMainActor->id;
    }

    PowerAPI_BuffApplyOnPowerDamageInitEffect(tPowerDamage.idACDSource, idACDTarget, tPowerDamage);

    BOOL bDOT = FALSE;
    float flDamageDone[7];
    float flPreDamageDone[7];

    float flDamageReductionTurnsIntoHealTotal = 0.0f;
    BOOL bWasImmune = FALSE;

    for (DamageType eDamageType = 0; eDamageType < 7; eDamageType++)
    {
        BOOL bTargetIsImmune = tACDTarget.AttributesGetInt(MakeAttribKey(ATTRIBUTE_IMMUNITY, eDamageType));

        if (ptActor && eDamageType == 0)
        {
            if (ActorFlagIsSet(ptActor->id, ACTOR_EASYKILL_BIT))
            {
                bTargetIsImmune = FALSE;
            }
        }

        if (bTargetIsImmune)
        {
            flDamageDone[eDamageType] = 0.0f;
            flPreDamageDone[eDamageType] = 0.0f;

            bWasImmune = TRUE;
            continue;
        }

        if (ptActor &&
            (ActorFlagIsSet(ptActor->id, ACTOR_NO_DAMAGE_RANGE_BIT) ||
             (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_NO_DAMAGE_RANGE_BIT))))
        {
            flDamageDone[eDamageType] = tPayload.arCombatDamage[eDamageType].flMin + tPayload.arCombatDamage[eDamageType].flDelta * 0.5f;
        }
        else
        {
            flDamageDone[eDamageType] = RandRangeFloat_Internal(SGameGlobalsGet().rng, tPayload.arCombatDamage[eDamageType].flMin, tPayload.arCombatDamage[eDamageType].flDelta);
        }

        float flDamageDoneReductionPercent = FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_DAMAGE_DONE_REDUCTION_PERCENT);
        if (flDamageDoneReductionPercent > EPSILON && !tPayload.bIgnoreDamageBonusAndReduction)
        {
            flDamageDone[eDamageType] = (1.0f - flDamageDoneReductionPercent) * flDamageDone[eDamageType];
        }

        flPreDamageDone[eDamageType] = flDamageDone[eDamageType];

        if (flDamageDone[eDamageType] < EPSILON)
        {
            continue;
        }

        if (!tPayload.bIgnoreDamageBonusAndReduction)
        {
            sApplyResistance(tPayload.idPower, tACDTarget, eDamageType, flDamageDone[eDamageType]);
            sApplyMiscDamageReduction(tPayload.idPower, tACDTarget, tPayload.eDeliveryMechanism, flDamageDone[eDamageType], flDamageReductionTurnsIntoHealTotal, eDamageType);

            sApplyArmor(tPayload.idPower, tACDTarget, flDamageDone[eDamageType]);

            if (tPowerDamage.bCriticalHit && ptPower)
            {
                sApplyCrit(tPayload.idPower, ptACDMainActor, flDamageDone[eDamageType], tPayload.flPowerCritDamageBonusPercent, tPayload.flCritCap, tACDTarget);
            }

            PowerHelper_ApplyDamageBonusMultipliers(tACDTarget,
                ptPower->snoPower,
                eDamageType,
                tPayload.flPrimaryAttributeBonusMultiplier,
                tPayload.flClassBonusMultiplier,
                tPayload.flSkillsBonusMultiplier,
                flDamageDone[eDamageType]);

            sApplyMiscDamageBonuses(tPayload.idPower, tACDTarget, flDamageDone[eDamageType], eDamageType, tPayload.eDeliveryMechanism, tPayload.bUseHighestElementalDamageBonus);

            sApplyMiscDamageAbsorb(tACDTarget, flDamageDone[eDamageType], eDamageType);

            sPetScalar(tPayload, tACDTarget, flDamageDone[eDamageType]);

            sCharmedMonsterScalar(tPayload, tACDTarget, flDamageDone[eDamageType]);
        }

        if (fConvertsDamageToDOT &&
            tPayload.flDamageDurationInSecs[eDamageType] > EPSILON)
        {
            sConvertDamageToDOT(tPayload, tACDTarget, tPowerDamage, eDamageType, flDamageDone[eDamageType]);
            flDamageDone[eDamageType] = 0.0f;
            bDOT = TRUE;
        }
    }

    if (tPowerDamage.bCrushingBlow)
    {
        sApplyCrushingBlow(idPower, flDamageDone[0], idACDMainActor, tACDTarget);
    }

    if (bWasImmune)
    {
        sSendImmuneMessage(idACDTarget, idPower);
    }

    if (tPayload.eDeliveryMechanism == PDM_DOT &&
        !tPayload.bSplashDamage && !bWasImmune && !bDOT && fConvertsDamageToDOT && ptACDMainActor)
    {
        ptACDMainActor->GetDPSValue(ACDIsDualWielding(*ptACDMainActor, TRUE));
    }


    sApplyDamageReduction(tACDTarget, flDamageDone, ptPower ? ptPower->snoPower : SNO_NONE);

    float flDamageBlocked = 0.0f;
    BOOL fBlocked = sTargetBlocked(tPayload, tACDTarget, flDamageBlocked, flDamageDone);
    tPowerDamage.bShattered = FALSE;

    if (fBlocked)
    {
        tPowerDamage.bDamageWasBlocked = TRUE;
        tPowerDamage.flDamageBlocked = flDamageBlocked;
    }
    else
    {
        tPowerDamage.bShattered = sCheckAndApplyShatter(tPayload.idPower, tACDTarget, flDamageDone);
    }

    tPowerDamage.bPhysical = tPayload.bForcedPhysical || (ptPower && PowerIsPhysical(idPower));
    tPowerDamage.bTriggersHitProcs = tPayload.bTriggersHitProcs;
    tPowerDamage.bIgnoreDamageBonusAndReduction = tPayload.bIgnoreDamageBonusAndReduction;
    tPowerDamage.bIsAOEThorns = tPayload.bIsAOEThorns;

    if (ptActor &&
        ActorFlagIsSet(ptActor->id, ACTOR_EASYKILL_BIT))
    {
        DamageType eExtraDamageType = GetPredominantDamageType(flDamageDone, FALSE);

        if (eExtraDamageType < 0 || eExtraDamageType >= 7)
        {
            eExtraDamageType = 0;
        }

        const float fEasyKillDamage = 1000000000000000.0f;
        flDamageDone[eExtraDamageType] = fEasyKillDamage;
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_SLOW_KILL_BIT))
    {
        for (int32 iDamageType = 0; iDamageType < 7; iDamageType++)
        {
            flDamageDone[iDamageType] *= 0.01f;
        }
    }

    float flHPMaxTotal = tACDTarget.GetHitPointsMaxTotal();
    float flHPCurr = tACDTarget.GetHitPointsCurrent();

    float flHitPointDamage =
        flDamageDone[0] +
        flDamageDone[1] +
        flDamageDone[2] +
        flDamageDone[3] +
        flDamageDone[4] +
        flDamageDone[5] +
        flDamageDone[6];

    if (flHitPointDamage > EPSILON)
    {
        bWasImmune = FALSE;
    }

    tPowerDamage.bWasImmune = bWasImmune;

    if (!tACDTarget.IsWreckableObject())
    {
        if (!ActorIsUsingAIQueue((ActorID)ACDGetOwner(tACDTarget.id)))
        {
            sApplyStunEffects(tPayload, tACDTarget, tPowerDamage);
            sApplyChillEffects(tPayload, tACDTarget, tPowerDamage);
            sApplyFreezeEffects(tPayload, tACDTarget, tPowerDamage);
            sApplyFearEffects(tPayload, tACDTarget, tPowerDamage);
            sApplyWebbedEffect(tPayload, tACDTarget, tPowerDamage);
            sApplyImmobilizeEffect(tPayload, tACDTarget, tPowerDamage);
            sApplySlowEffect(tPayload, tACDTarget, tPowerDamage);
            sApplyBlindEffect(tPayload, tACDTarget, tPowerDamage);
        }

        sApplyManaDrainEffect(tPayload, tPowerDamage);
    }

    tPowerDamage.eKnockBack = tPayload.eKnockBack;
    tPowerDamage.wvKnockbackDirection = tPayload.wvKnockbackDirection;
    tPowerDamage.wdKnockbackMagnitude = tPayload.wdKnockbackMagnitude;
    tPowerDamage.flKnockbackHeight = tPayload.flKnockbackHeightMin + tPayload.flKnockbackHeightDelta * RandFloat_Internal(SGameGlobalsGet().rng);
    tPowerDamage.flKnockbackGravity = tPayload.flKnockbackGravityMin + tPayload.flKnockbackGravityDelta * RandFloat_Internal(SGameGlobalsGet().rng);
    tPowerDamage.bKnockbackIgnoresWeight = tPayload.bKnockbackIgnoresWeight;
    tPowerDamage.bContinuousKnockbackPower = tPayload.bContinuousKnockbackPower;
    tPowerDamage.tKnockbackPlace = tPayload.tKnockbackPlace;
    tPowerDamage.bKnockdown = tPayload.bKnockdown;
    tPowerDamage.bAlwaysCauseGetHit = tPayload.bAlwaysCauseGetHit;
    tPowerDamage.bNeverCauseGetHit = tPayload.bNeverCauseGetHit;
    tPowerDamage.snoKnockbackPower = tPayload.snoKnockbackPower;
    tPowerDamage.eAttackDirection = tPayload.eAttackDirection;
    tPowerDamage.eDeathReplacementType = tPayload.eDeathReplacementType;
    tPowerDamage.bDisableHitEffects = tPayload.bDisableHitEffects;
    tPowerDamage.bForceCriticalDeath = tPayload.bForceCriticalDeath;

    if (tPowerDamage.eKnockBack != KNOCKBACK_NONE)
    {
        if (tPowerDamage.flKnockbackHeight < EPSILON || (tPowerDamage.flKnockbackGravity > -EPSILON && tPowerDamage.flKnockbackGravity < EPSILON))
        {
            CRefString sError;
            sError.Format("PowerAPI_DoPayload: PowerSNO (%s) Power ID (ID - %d) tried to apply payload that has a knockback with Height or Gravity <= 0, This causes undefined behaviour, likely the intent was to use a force grip CC instead!\n",
                ptPower ? SNOToString(POWER_SNO, ptPower->snoPower, FALSE).str() : "[unknown]",
                ptPower ? ptPower->id : -1);

        }
    }

    if ((sgtAppGlobals.dwDebugFlags2 & (1 << APP_GLOBAL2_ALWAYS_PLAY_KNOCKBACK_BIT)) && tPowerDamage.eKnockBack == KNOCKBACK_NONE)
    {
        const float DEBUG_KNOCKBACK_MAGNITUDE = 20.0f;
        const float DEBUG_KNOCKBACK_HEIGHT = 2.0f;
        tPowerDamage.wdKnockbackMagnitude = DEBUG_KNOCKBACK_MAGNITUDE;
        tPowerDamage.flKnockbackHeight = DEBUG_KNOCKBACK_HEIGHT;
        tPowerDamage.eKnockBack = 1;
    }

    if (tPowerDamage.eKnockBack != KNOCKBACK_NONE)
    {
        float flKnockbackScalar = PowerAttributesGetFloat(tPowerDamage.idPower, ATTRIBUTE_KNOCKBACK_ATTACK_SCALAR) + 1.0f;
        tPowerDamage.wdKnockbackMagnitude *= flKnockbackScalar;
        tPowerDamage.flKnockbackHeight = flKnockbackScalar * tPowerDamage.flKnockbackHeight;

        if (tPowerDamage.eKnockBack != KNOCKBACK_NONE && tPayload.bKnockbackAffectedByReduction)
        {
            const GameBalanceDefinition * pdefGBEliteMods = NULL;
            const EliteModifierData * pEliteMods = NULL;
            tACDTarget.GetEliteModifierData(&pdefGBEliteMods, &pEliteMods);
            if (pEliteMods)
            {
                if (RandFloat_Internal(SGameGlobalsGet().rng) < pEliteMods->flKnockbackResist)
                {
                    tPowerDamage.eKnockBack = KNOCKBACK_NONE;
                    tPowerDamage.wdKnockbackMagnitude = 0.0f;
                }
                else
                {
                    tPowerDamage.wdKnockbackMagnitude *= (1.0f - pEliteMods->flKnockbackER);
                }

                if (Math::Abs(tPowerDamage.wdKnockbackMagnitude) + EPSILON < pEliteMods->flKnockbackMin)
                {
                    tPowerDamage.eKnockBack = KNOCKBACK_NONE;
                    tPowerDamage.wdKnockbackMagnitude = 0.0f;
                }
                GBUnlockR(pdefGBEliteMods);
            }
        }
    }

    if (bDOT && flHitPointDamage <= EPSILON)
    {
        tPowerDamage.eDeliveryMechanism = PDM_DOT;
    }

    if (flHitPointDamage > EPSILON)
    {
        PowerHelper_DamageAdd(tPowerDamage, ATTRIBUTE_HITPOINTS_CUR, -flHitPointDamage, 0);

        if (tPayload.eDeliveryMechanism != PDM_THORNS)
        {
            float flGetHitDamage = flHitPointDamage * PowerAttributesGetFloat(idPower, ATTRIBUTE_GETHIT_DAMAGE_SCALAR) +
                PowerAttributesGetFloat(idPower, ATTRIBUTE_GETHIT_DAMAGE);

            if (flGetHitDamage > EPSILON)
            {
                sUpdateRecoil(idACDTarget);
                PowerHelper_DamageAdd(tPowerDamage, ATTRIBUTE_GETHIT_CURRENT, flGetHitDamage, 0);
            }
        }

        float flDamageToManaPercent = tACDTarget.AttributesGetFloat(ATTRIBUTE_DAMAGE_TO_MANA);
        float flDamageToManaAmount = flHitPointDamage * flDamageToManaPercent;
        if (flDamageToManaAmount > EPSILON || flDamageToManaAmount < -EPSILON)
        {
            PowerHelper_DamageAdd(tPowerDamage, MakeAttribKey(ATTRIBUTE_RESOURCE_CUR, 0), flDamageToManaAmount, 0);
        }

        if (tACDTarget.ConvertsDamageToDamageState())
        {
            if (PowerACDCanTakeDamage(tACDTarget) &&
                !PowerContactTagGetBool(idPower, 0x501e8))
            {
                int32 nDamageStateMax = tACDTarget.GetDamageStateMax();
                int32 nCurrDamageState = tACDTarget.IncDamageState();
                if (nCurrDamageState >= nDamageStateMax)
                {
                    ActorID idKiller = ACTORID_INVALID;
                    if (ptActor)
                    {
                        idKiller = ptActor->id;
                    }

                    BrainAPIDoDeath((ActorID)ACDGetOwner(tACDTarget.id), idKiller, FALSE, FALSE, FALSE, &tPowerDamage, flDamageDone);
                }
            }
        }
        else
        {
            PowerAPI_DoDamage(tPowerDamage, 0, flDamageDone);
        }

        if (tACDTarget.IsMonster() && ACDIsPlayer(idACDMainActor))
        {
            SPlayerRefreshKillCounterTime(ptACDMainActor);
        }
    }
    else
    {
        PowerAPI_DoDamage(tPowerDamage, 0, NULL);
    }

    if (ptACDMainActor)
    {
        if (!bDOT && tPowerDamage.bCriticalHit)
        {
            sPowerAPI_CheckForOnCriticalProcs(tPowerDamage, ptACDMainActor, tACDTarget);
        }

        if (tPayload.eDeliveryMechanism == PDM_MELEE ||
            tPayload.eDeliveryMechanism == PDM_RANGED ||
            (flHitPointDamage > EPSILON && tPayload.eDeliveryMechanism == PDM_MAGIC))
        {
            if (flHitPointDamage > EPSILON)
            {
                sPowerAPI_CheckForOnHitProcs(*ptPower, *ptACDMainActor, tACDTarget, tPowerDamage.eKnockBack, tPayload.flProcScalar, flDamageDone, flHitPointDamage, &tPayload, NULL, tPowerDamage.bCriticalHit);
            }
        }

        if ((tPayload.eDeliveryMechanism == PDM_MELEE || tPayload.eDeliveryMechanism == PDM_RANGED) &&
            tPayload.bTriggersHitProcs)
        {
            sApplyThorns(tPowerDamage.flProcScalar, flDamageDone, *ptACDMainActor, tACDTarget, ptPower, FALSE);
        }

        if (flDamageReductionTurnsIntoHealTotal > EPSILON)
        {
            ActorHeal(tACDTarget.id, flDamageReductionTurnsIntoHealTotal, 7, ptPower->snoPower, tACDTarget.id, TRUE);
        }
    }

    if (flHitPointDamage > EPSILON &&
        !tPayload.bDisableHitEffects &&
        !tACDTarget.IsWreckableObject() &&
        tACDTarget.GetGizmoType() != 8 &&
        tPayload.eDeliveryMechanism != PDM_DOT && !bWasImmune)
    {
        if (ptPower && PowerContactTagGetBool(AutoPowerDefinitionR(ptPower->snoPower), 0xa7000, ptPower->dwCurrentContactFrame))
        {
            SNO snoEffectGroup;
            if (tPowerDamage.bCriticalHit)
            {
                snoEffectGroup = PowerContactTagGetSNO(AutoPowerDefinitionR(ptPower->snoPower), 0xa7001, ptPower->dwCurrentContactFrame);
            }
            else
            {
                snoEffectGroup = PowerContactTagGetSNO(AutoPowerDefinitionR(ptPower->snoPower), 0xa7002, ptPower->dwCurrentContactFrame);
            }

            ACDPlayHitEffectOverride(tACDTarget.id, ptPower->idACD, snoEffectGroup);
        }
        else
        {
            DamageType eGetHitDamageType = GetPredominantDamageType(flDamageDone, FALSE);
            ACDID idACDAttackerForEffect = ptPower->idACD;
            if (tPayload.idACDFloatingCombatTextSourceOverride != ACDID_INVALID)
            {
                idACDAttackerForEffect = tPayload.idACDFloatingCombatTextSourceOverride;
            }

            ACDPlayHitEffect(idACDTarget, idACDAttackerForEffect, eGetHitDamageType, tPowerDamage.bCriticalHit);
        }

        if (tPayload.snoProjectileToStick != SNO_NONE)
        {
            PowerAPI_StickProjectileIntoActor(idPower, tACDTarget, tPayload.snoProjectileToStick, FALSE);
        }
    }

    if (tPayload.bCausesAmbientDamageEffects)
    {
        SNO snoAmbientDamageEffect = ActorDefinitionGetTagMapValueSNO(0x13590, tACDTarget.snoActor);
        if (snoAmbientDamageEffect != SNO_NONE)
        {
            uint32 uNow = GameGetServerTime();
            uint32 uLastAmbientEffectTime = tACDTarget.AttributesGetInt(ATTRIBUTE_AMBIENT_DAMAGE_EFFECT_LAST_TIME);
            if (uNow > uLastAmbientEffectTime + 20)
            {
                ACDPlayEffect(tACDTarget.id, EFFECT_PLAY_EFFECT_GROUP, snoAmbientDamageEffect, idACDMainActor);
                tACDTarget.AttributesSetInt(ATTRIBUTE_AMBIENT_DAMAGE_EFFECT_LAST_TIME, uNow);
            }
        }
    }

    if (flHitPointDamage > EPSILON &&
        tPowerDamage.bCriticalHit &&
        GetPredominantDamageType(flPreDamageDone, TRUE) == 0)
    {
        ACDID idBloodEffectACD = ACDID_INVALID;
        BOOL fCausesCritBloodEffect = tPayload.eDeliveryMechanism == PDM_MELEE;
        if (fCausesCritBloodEffect)
        {
            if (!tACDTarget.IsGizmo())
            {
                if (!ActorDefinitionGetTagMapValueInt(0x13600, tACDTarget.snoActor))
                {
                    if (ACDIsPlayer(idACDMainActor))
                    {
                        idBloodEffectACD = idACDMainActor;
                    }

                    ACDPlayEffect(idACDTarget, EFFECT_CRITICAL_HIT, idBloodEffectACD, idACDMainActor);
                }
            }
        }
    }

    if (tPayload.snoTargetImpactParticle != SNO_NONE &&
        tPowerDamage.listDamage.Count() > 0)
    {
        ACDPlayEffect(idACDTarget, EFFECT_CREATE_PARTICLE, tPayload.snoTargetImpactParticle, idACDMainActor);
    }

    if (tPayload.snoTargetImpactEffectGroup != SNO_NONE &&
        tPowerDamage.listDamage.Count() > 0 && flHitPointDamage > EPSILON)
    {
        ACDPlayEffect(idACDTarget, EFFECT_PLAY_EFFECT_GROUP, tPayload.snoTargetImpactEffectGroup, idACDMainActor);
    }

    return TRUE;
}

// PowerAPI.cpp:4873
int32 PowerAPI_DoPayload(const PowerAPIAttackPayload & tPayload, XPooledDataIDList & listTargets, const ACDID idACDOverrideAttacker)
{
    int32 nTargetsHit = 0;
    int32 nTargetCap = PowerGetMaxAreaEffectTargetCount();
    POS posTarget = listTargets.GetTailPosition();
    while (posTarget)
    {
        POS posPrev = posTarget;
        ACDID idACDTarget = (ACDID)listTargets.GetPrev(posTarget);
        if (nTargetsHit < nTargetCap && PowerAPI_DoPayload(tPayload, idACDTarget, idACDOverrideAttacker))
            nTargetsHit++;
        else
            listTargets.RemoveAt(posPrev);
    }
    return nTargetsHit;
}

// PowerAPI.cpp:5129
void PowerAPI_DestroyProjectile(const PowerAPIDestroyProjectile & tProjectileInfo)
{
    ActorID idProjectileActor = (ActorID)ACDGetOwner(tProjectileInfo.idACDProjectile);
    ActorFlagForDeletion(idProjectileActor);
}

// PowerAPI.cpp:5158
void PowerAPI_DoAnimation(const PowerAPIAnimation & tAttackInfo)
{
    SNO snoPower = PowerGetSNO(tAttackInfo.idPower);
    ActorSetPowerState((ActorID)ACDGetOwner(tAttackInfo.idACDSource), CRefString().Format("Animating for: %s", PowerTrace(snoPower).str()));
    AnimReason eAnimReason = ANIM_REASON_POWER;
    ActorCommonData * ptACDSource = ACDTryToGet(tAttackInfo.idACDSource);
    if (ptACDSource && PowerIsChannelled(*ptACDSource, AutoPowerDefinitionR(snoPower)))
        eAnimReason = ANIM_REASON_PREPLAY_CHANNELLED;
    PowerTriggerPlayTriggerAnimation(tAttackInfo.idPower, tAttackInfo.dwAnimationTag, tAttackInfo.dwDuration, tAttackInfo.flSpeedScalar, tAttackInfo.bSetAimTarget, tAttackInfo.tPreplayedAnimData, eAnimReason, FALSE, TRUE);
}

// PowerAPI.cpp:5334
static BOOL sPowerTargetIsInMeleeSnapshot(const Power * ptPower, const ACDID idACDTarget)
{
    POS pos = ptPower->listInitialMeleeRangeACDs.GetHeadPosition();
    while (pos)
    {
        ACDID idACDCurr = (ACDID)ptPower->listInitialMeleeRangeACDs.GetNext(pos);
        if (idACDCurr == idACDTarget)
            return TRUE;
    }
    return FALSE;
}

// PowerAPI.cpp:5354
float PowerAPI_ComputeMeleeReach(PowerID idPower, ACDID idACDTarget)
{
    if (idACDTarget != ACDID_INVALID)
    {
        Power * ptPower = PowerTryToGet(idPower);
        if (ptPower && PowerHasEscapeRange(*ptPower))
        {
            ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);
            BOOL bIsPlayer = ACDIsPlayer(ptPower->idACD);
            if (!bIsPlayer || sPowerTargetIsInMeleeSnapshot(ptPower, idACDTarget))
            {
                float flEscapeAttackRadius = PowerGetFormulaValue(idPower, TAG_POWER_ESCAPE_ATTACK_RADIUS);
                if (!bIsPlayer && ptPower->placeStartTarget.idSWorld != SWORLDID_INVALID)
                {
                    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
                    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
                    if (ptACD && ptACDTarget)
                    {
                        WorldPlace placeACD = ptACD->GetPlace();
                        WorldPlace placeTarget = ptACDTarget->GetPlace();
                        if (placeACD.idSWorld == placeTarget.idSWorld && placeACD.idSWorld == ptPower->placeStartTarget.idSWorld)
                        {
                            Vector2D vToCurr((placeTarget.wp - placeACD.wp).x, (placeTarget.wp - placeACD.wp).y);
                            Vector2D vToOrig((ptPower->placeStartTarget.wp - placeACD.wp).x, (ptPower->placeStartTarget.wp - placeACD.wp).y);
                            if (vToCurr.MagnitudeSquared() > EPSILON * EPSILON &&
                                vToOrig.MagnitudeSquared() > EPSILON * EPSILON)
                            {
                                vToCurr.Normalize();
                                vToOrig.Normalize();
                                float flOrigRange = PowerGetRange(idPower, TRUE);
                                if (flOrigRange < flEscapeAttackRadius)
                                {
                                    float flDeltaRangeTotal = flEscapeAttackRadius - flOrigRange;
                                    float flCosAng = Vector2D::DotProduct(vToCurr, vToOrig);
                                    float flDeltaRange = 0.0f;
                                    if (flCosAng < 0.0f)
                                        flDeltaRange = -(flCosAng * flDeltaRangeTotal);
                                    flEscapeAttackRadius = flOrigRange + flDeltaRange;
                                }
                            }
                        }
                    }
                }
                return flEscapeAttackRadius;
            }
        }
    }
    return 0.0f;
}

// PowerAPI.cpp:5776
BOOL PowerAPI_TargetIsWithinArc(WorldPlace placeSource, Vector3D wvVectorToUse, Angle aMaxAngle, ACDID idACDTarget)
{
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    Vector3D wvSourceToDest;
    wvSourceToDest = wvVectorToUse;
    wvSourceToDest.z = 0.0f;
    WorldPlace placeTarget = tACDTarget.GetPlace();
    Vector3D wvSourceToTarget;
    WorldPlaceGetVectorBetween(placeSource, placeTarget, wvSourceToTarget);
    wvSourceToTarget.z = 0.0f;
    WorldDistance wdDistanceToTarget = wvSourceToTarget.Magnitude();
    WorldDistance wdCylinderRadius = tACDTarget.GetRadius();
    if (wdDistanceToTarget < EPSILON || wdCylinderRadius > wdDistanceToTarget)
        return TRUE;
    Angle aCylArc = Math::ASin(wdCylinderRadius / wdDistanceToTarget);
    Angle aDestToTarget = Vector3D::AngleBetween(wvSourceToDest, wvSourceToTarget);
    if (aDestToTarget > HALF_CIRCLE)
        aDestToTarget = FULL_CIRCLE - aDestToTarget;
    return aDestToTarget < aCylArc + aMaxAngle;
}

// PowerAPI.cpp:5919
static inline void PowerAPI_CapsuleFindCenter(const ChooseTargetGeometryCapsule & tCapsule, WorldPlace & placeCenter)
{
    placeCenter = tCapsule.place0;
    placeCenter.wp += tCapsule.vDir * (tCapsule.flLength * 0.5f);
}

// PowerAPI.cpp:6102

// Jump table: 0,6,7 sphere; 1,4 target; 2 ray; 8 pie; 9 capsule; 10 box; 5 no-LOS; 3 default.

#line 6102
// NON_MATCHING: The ray and box case bodies appear in the opposite order. The jump table selects the same geometry fields.
void sPowerAPIGetTargettingSourcePlace(
    const ChooseTargetGeometry & tGeometry,
    WorldPlace & tPlace)
{
    switch (tGeometry.eType)
    {
    case CTT_SPHERE:
    case CTT_FRONT:
    case CTT_BACK:
        tPlace = tGeometry.tSphere.placeCenter;
        break;

    case CTT_SINGLE_TARGET:
    case CTT_MELEE:
        tGeometry.tSingleTarget.tTarget.GetPlace(tPlace);
        break;

    case CTT_PIE:
        tPlace = tGeometry.tPie.placeCenter;
        break;

    case CTT_CAPSULE:
        tPlace = tGeometry.tCapsule.place0;
        break;

    case CTT_BOX:
        tPlace = tGeometry.tBox.placeCenter;
        break;

    case CTT_RAY:
        tPlace = tGeometry.tRay.placeStart;
        break;




    case CTT_NO_LINE_OF_SIGHT:
        DisplayInternalError(NULL, "", 6197, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    default:
        DisplayInternalError(NULL, "", 6200, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}
#line 3106

// PowerAPI.cpp:6149
ACDID sPowerAPIChooseClosestTarget(const ChooseTargetGeometry & tGeometry, XPooledDataIDList & listTargets)
{
    if (listTargets.Count() == 0 || tGeometry.eType == CTT_NO_LINE_OF_SIGHT)
        return ACDID_INVALID;
    WorldPlace placeTarget;
    sPowerAPIGetTargettingSourcePlace(tGeometry, placeTarget);
    WorldDistance wdClosestDistance = MAX_WORLDDISTANCE;
    ACDID result = ACDID_INVALID;
    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDCandidate = (ACDID)listTargets.GetNext(pos);
        WorldPlace placeCanidate = ACDGet(idACDCandidate).GetPlace();
        WorldDistance wdDistanceSquared = WorldPlaceDistanceSquared(placeTarget, placeCanidate);
        if (wdDistanceSquared < wdClosestDistance)
        {
            wdClosestDistance = wdDistanceSquared;
            result = idACDCandidate;
        }
    }
    return result;
}

// X/Server/Powers/PowerAPI.h; layout from DWARF.
struct PowerAPIChooseOpenGround {  // size 0x20
    NavPlace tNavPlace;  // 0x00
    float flSkipDistance;  // 0x18
    BOOL bUseActorPassability;  // 0x1c
};
static_assert(sizeof(PowerAPIChooseOpenGround) == 0x20, "PowerAPIChooseOpenGround");
BOOL NavMeshIsPointOnHandle(const NavMeshHandle & tNavMeshHandle, const Vector3D & wp);  // NavigationMesh.cpp:676

// PowerAPI.cpp:6190
BOOL PowerAPI_DoChooseOpenGround(SNO snoActor, float flScale, const PowerAPIChooseOpenGround & tOpenGroundInfo, WorldPlace & placeOpenSpot, NavMeshHandle & tNavMeshHandleResult)
{
    if (NavMeshHandleIsNone(tOpenGroundInfo.tNavPlace.tNavMeshHandle)) {
        return FALSE;
    }
    if (!NavMeshIsPointOnHandle(tOpenGroundInfo.tNavPlace.tNavMeshHandle, tOpenGroundInfo.tNavPlace.tPlace.wp)) {
        return FALSE;
    }

    PassabilitySettings tPassability;
    PassabilitySettingsInit(tPassability);
    tPassability.ePassabilityType = tOpenGroundInfo.bUseActorPassability ? PASSABILITY_PULL_FROM_ACTOR : PASSABILITY_MONSTER;
    tPassability.flScale = flScale;
    BOOL fSpotFound = FindPerturbedSpawnLocation(snoActor, tOpenGroundInfo.tNavPlace.tNavMeshHandle, tPassability, tOpenGroundInfo.flSkipDistance, 0, placeOpenSpot, tNavMeshHandleResult, 1);
    return fSpotFound != FALSE;
}

// PowerAPI.cpp:6235
BOOL PowerAPI_DoChooseOpenGround(ActorCommonData & tACD, const PowerAPIChooseOpenGround & tOpenGroundInfo, WorldPlace & placeOpenSpot, NavMeshHandle & tNavMeshHandleResult)
{
    return PowerAPI_DoChooseOpenGround(tACD.snoActor, tACD.GetScale(), tOpenGroundInfo, placeOpenSpot, tNavMeshHandleResult);
}

// PowerAPI.cpp:6511
BOOL PowerAPI_DoChooseExplosion(const ChooseTargetGeometry & tGeometry, const XPooledDataIDList & listTargets, WorldPlace & placeExplosion)
{
    switch (tGeometry.eType)
    {
    case CTT_SINGLE_TARGET:
        tGeometry.tSingleTarget.tTarget.GetPlace(placeExplosion);
        break;
    case CTT_MELEE:
        if (listTargets.Count() > 0)
        {
            ACDID idACD = (ACDID)listTargets.GetHead();
            placeExplosion = ACDGet(idACD).GetPlace();
        }
        else
        {
            tGeometry.tMelee.tTarget.GetPlace(placeExplosion);
        }
        break;
    case CTT_RAY:
        if (listTargets.Count() > 0)
        {
            ACDID idACD = (ACDID)listTargets.GetHead();
            placeExplosion = ACDGet(idACD).GetPlace();
        }
        else
        {
            DisplayInternalError(NULL, "", 6606, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
        break;
    case CTT_NO_LINE_OF_SIGHT:
        return FALSE;
    case CTT_SPHERE:
    case CTT_FRONT:
    case CTT_BACK:
        placeExplosion = tGeometry.tSphere.placeCenter;
        break;
    case CTT_PIE:
        placeExplosion = tGeometry.tPie.placeCenter;
        break;
    case CTT_CAPSULE:
        PowerAPI_CapsuleFindCenter(tGeometry.tCapsule, placeExplosion);
        break;
    case CTT_BOX:
        placeExplosion = tGeometry.tBox.placeCenter;
        break;
    default:
        DisplayInternalError(NULL, "", 6635, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    }
    return TRUE;
}

// PowerAPI.cpp:6587
void PowerAPI_DoExplosion(const PowerAPIExplosion & tExplosion)
{
    if (tExplosion.snoExplosionActor == SNO_NONE || tExplosion.dwDuration == 0)
        return;
    ActorAddStruct tActorAdd;
    tActorAdd.snoActor = tExplosion.snoExplosionActor;
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    tActorAdd.tPlace = tExplosion.placeSource;
    tActorAdd.qOrientation = tExplosion.qOrientation;
    tActorAdd.flScale = 1.0f;
    ActorID idExplosion = SceneryAdd(tActorAdd);
    if (idExplosion != ACTORID_INVALID)
        ActorDeleteSoon(idExplosion, tExplosion.dwDuration);
}

// PowerAPI.cpp:6826
int32 PowerAPI_GetSummonCountPer(ActorCommonData & tACD, SNO snoPower)
{
    int32 nSummonCountPer = (int32)PowerGetFormulaValue(&tACD, AutoPowerDefinitionR(snoPower), TAG_POWER_SKELETON_SUMMON_COUNT, -1, -1, 0);
    if (nSummonCountPer == -1)
        nSummonCountPer = MonsterGetSummonCountPer(tACD.tSNOName);
    return nSummonCountPer;
}

// PowerAPI.cpp:6842
float PowerAPI_GetSummonLifetime(ActorCommonData & tACD, SNO snoPower)
{
    float flSummonLifetime = PowerGetFormulaValue(&tACD, AutoPowerDefinitionR(snoPower), TAG_POWER_SUMMONED_ACTOR_LIFE_DURATION, -1, -1, 0);
    if (flSummonLifetime < 0.0f)
        flSummonLifetime = MonsterGetSummonLifetime(tACD.tSNOName);
    return flSummonLifetime;
}

// PowerAPI.cpp:6858
int32 PowerAPI_GetSummonMaxConcurrent(ActorCommonData & tACD, SNO snoPower)
{
    int32 nSummonMaxConcurrent = (int32)PowerGetFormulaValue(&tACD, AutoPowerDefinitionR(snoPower), TAG_POWER_SKELETON_MAX_SUMMON_COUNT, -1, -1, 0);
    if (nSummonMaxConcurrent == -1)
        nSummonMaxConcurrent = MonsterGetSummonMaxConcurrent(tACD.tSNOName);
    return nSummonMaxConcurrent;
}

// PowerAPI.cpp:6874
int32 PowerAPI_GetSummonMaxTotal(ActorCommonData & tACD, SNO snoPower)
{
    int32 nSummonMaxTotal = (int32)PowerGetFormulaValue(&tACD, AutoPowerDefinitionR(snoPower), TAG_POWER_SKELETON_MAX_TOTAL_SUMMON_COUNT, -1, -1, 0);
    if (nSummonMaxTotal == -1)
        nSummonMaxTotal = MonsterGetSummonMaxTotal(tACD.tSNOName);
    return nSummonMaxTotal;
}

// PowerAPI.cpp:6890
// Removed by the linker in the original (DWARF concrete DIE has low_pc 0); it survives only inlined into PowerAPI_DoPayload at
// line 4768 with bOverrideDontStick = FALSE. The return values and the bOverrideDontStick test are reconstructed, not evidenced.
BOOL PowerAPI_StickProjectileIntoActor(
    const PowerID idPower,
    const ActorCommonData & tACDTarget,
    const SNO snoProjectileToStick,
    const BOOL bOverrideDontStick)
{
    if (!tACDTarget.IsAlive())
    {
        return FALSE;
    }

    float flChance = ActorDefinitionGetTagMapValueFloat(0x10117, tACDTarget.snoActor);

    if (bOverrideDontStick ||
        RandFloat_Internal(SGameGlobalsGet().rng) < flChance)
    {
        ActorCommonData * ptACDSource = PowerGetOwner(idPower);
        if (ptACDSource)
        {
            ServerMessageProjectileStick(*ptACDSource, tACDTarget, snoProjectileToStick);
            return TRUE;
        }
    }
    return FALSE;
}

// PowerAPI.cpp:6925
static void sPowerAPI_Missed(const PowerAPIAttackPayload & tPayload, ActorCommonData & tACDTarget)
{
    if (!tACDTarget.IsAlive())
        return;
    ActorCommonData * ptACDSource = PowerGetOwner(tPayload.idPower);
    ActorID idActorSrc = ptACDSource ? (ActorID)ptACDSource->idOwner : ACTORID_INVALID;
    ActorID idActorTarget = (ActorID)ACDGetOwner(tACDTarget.id);
    BrainComponentWasAttacked(idActorTarget, idActorSrc);
    if (ACDIsPlayer(tACDTarget.id) && !tACDTarget.AttributesGetInt(ATTRIBUTE_GETHIT_IMMUNE))
    {
        ActorMovement * ptMove = tACDTarget.GetActorMovement();
        if (!ptMove || !ptMove->IsMoving())
        {
            uint32 dwAnimTag;
            float flChance;
            const float CHANCE_TO_PARRY = 1.0f;
            const float CHANCE_TO_DEFLECT = 1.0f;
            switch (tPayload.eDeliveryMechanism)
            {
            case 1:
                dwAnimTag = TAG_ACTOR_ANIMATION_PARRY;
                flChance = CHANCE_TO_PARRY;
                break;
            case 2:
                dwAnimTag = TAG_ACTOR_ANIMATION_DEFLECT;
                flChance = CHANCE_TO_DEFLECT;
                break;
            case 3:
                dwAnimTag = TAG_ACTOR_ANIMATION_DEFLECT;
                flChance = CHANCE_TO_DEFLECT;
                break;
            default:
                return;
            }
            if (RandFloat_Internal(SGameGlobalsGet().rng) < flChance)
                BrainAPIDoPlayAnimation(idActorTarget, dwAnimTag, (uint32)-1, FALSE, TRUE, 0.0f, FALSE, 1.0f);
        }
    }
}

// PowerAPI.cpp:6986
static void sPowerAPI_Dodged(const PowerAPIAttackPayload & tPayload, ActorCommonData & tACDTarget)
{
    ActorCommonData * ptACDOwner = PowerGetOwner(tPayload.idPower);
    ACDID idACDOwner = ACDID_INVALID;
    if (ptACDOwner)
    {
        idACDOwner = ptACDOwner->id;
    }
    BrainAPIDodged(idACDOwner, tACDTarget.id);
    sPowerAPI_Missed(tPayload, tACDTarget);
}


// PowerAPI.cpp:7116
BOOL PowerAPI_DoCosts(const PowerAPICosts & tCostInfo, float flResourceCostModifier)
{
    PowerID idPower = tCostInfo.idPower;
    ACDID idACDMainActor = tCostInfo.idACD;

    Power * ptPower = PowerGet(idPower);
    SNO snoPower = ptPower->snoPower;

    ActorCommonData & tACDMainActor = ACDGet(idACDMainActor);
    ResourceType eResourceType = PowerGetResourceType(tACDMainActor, AutoPowerDefinitionR(snoPower));

    float flResourceCost = PowerGetResourceCost(idPower) * flResourceCostModifier;

    ActorCommonData * ptACDPowerOwner = ACDTryToGet(ptPower->idACD);

    float flHealthCostPercentOfMaxHealth = PowerGetFormulaValue(ptACDPowerOwner,
                                                                AutoPowerDefinitionR(snoPower),
                                                                TAG_POWER_COST_HEALTH,
                                                                ptPower->nSkillLevel,
                                                                PowerGetAttributeGroup(idPower),
                                                                0);
    float flHealthCost = 0.0f;
    if (ptACDPowerOwner)
        flHealthCost = flHealthCostPercentOfMaxHealth * ptACDPowerOwner->AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_TOTAL);

    float flChargesCost = PowerGetFormulaValue(ptACDPowerOwner,
                                               AutoPowerDefinitionR(snoPower),
                                               TAG_POWER_COST_CHARGES,
                                               ptPower->nSkillLevel,
                                               PowerGetAttributeGroup(idPower),
                                               0);

    float flHealthCostDiscount = Math::Pin(tACDMainActor.AttributesGetFloat(ATTRIBUTE_HEALTH_COST_DISCOUNT), 0.0f, 1.0f);

    const Actor * ptActor = ActorGet((ActorID)ACDGetOwner(idACDMainActor));
    BOOL bSkipChargingResources = FALSE;
    BOOL bSkipChargingHealth = FALSE;
    if (ActorFlagIsSet(ptActor->id, ACTOR_GOD_MODE_BIT) ||
        ptPower->bFreeCast ||
        tACDMainActor.AttributesGetInt(ATTRIBUTE_FREE_CAST_ALL))
    {
        bSkipChargingResources = TRUE;
        bSkipChargingHealth = TRUE;
    }
    else if (flHealthCostDiscount >= 0.999999f)
    {
        bSkipChargingHealth = TRUE;
    }

    float flPrimaryResourceOverridenCost = tACDMainActor.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_PRIMARY_RESOURCE_COST_OVERRIDE, snoPower));
    float flSecondaryResourceOverridenCost = tACDMainActor.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_SECONDARY_RESOURCE_COST_OVERRIDE, snoPower));

    if (flPrimaryResourceOverridenCost < FLT_MAX && flPrimaryResourceOverridenCost > 0.0f)
    {
        eResourceType = tACDMainActor.GetPrimaryResourceType();
    }
    else if (flPrimaryResourceOverridenCost > 0.0f && flSecondaryResourceOverridenCost < FLT_MAX)
    {
        eResourceType = tACDMainActor.GetSecondaryResourceType();
    }

    BOOL bOverriddenCosts = FALSE;
    if (flPrimaryResourceOverridenCost < FLT_MAX)
    {
        if (eResourceType == tACDMainActor.GetPrimaryResourceType())
        {
            flResourceCost = flPrimaryResourceOverridenCost;
            bOverriddenCosts = TRUE;
        }
    }
    if (flSecondaryResourceOverridenCost < FLT_MAX)
    {
        if (eResourceType == tACDMainActor.GetSecondaryResourceType())
        {
            flResourceCost = flSecondaryResourceOverridenCost;
            bOverriddenCosts = TRUE;
        }
    }

    if (flResourceCost > EPSILON)
    {
        float flCostReductionPercent = PowerGetCostReductionPercent(tACDMainActor, eResourceType, PowerGetDamageType(ptPower), snoPower);
        flResourceCost *= flCostReductionPercent;

        float flCostReduction = tACDMainActor.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_COST_REDUCTION_TOTAL, eResourceType) + tACDMainActor.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_RESOURCE_REDUCTION, snoPower));
        float flPowerCostReductionCoefficient = PowerGetFormulaValue(idPower, TAG_POWER_COST_RESOURCE_REDUCTION_COEFFICIENT);
        flResourceCost -= flCostReduction * flPowerCostReductionCoefficient;
    }

    float flHealthCostUndiscounted = flHealthCost * (tACDMainActor.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_HEALTH_COST_SCALAR, snoPower)) + 1.0f);
    flHealthCost = flHealthCostUndiscounted;

    if (flHealthCostUndiscounted > EPSILON)
    {
        float flHPCurrent = tACDMainActor.GetHitPointsCurrent();

        flHealthCost = (1.0f - flHealthCostDiscount) * flHealthCostUndiscounted;
        flHPCurrent -= flHealthCost;

        if (bSkipChargingHealth)
        {
            flHealthCost = 0.0f;
        }
        else
        {
            if (flHPCurrent < 1.0f)
            {
                ptPower->dwFlags |= 0x20;
                return FALSE;
            }

            tACDMainActor.SetHitPoints(flHPCurrent, TRUE, TRUE);
        }
    }

    if (flHealthCostUndiscounted > 0.0f)
    {
        sSendHealthCostAchievementEvent(&tACDMainActor, ptPower, 1);
    }

    PowerAPI_BuffApplyOnCostEffects(idACDMainActor, tCostInfo.idPower, PowerGetSNO(tCostInfo.idPower), flResourceCost, eResourceType, flHealthCost, !bSkipChargingResources, flHealthCostUndiscounted);

    if (bSkipChargingResources)
    {
        if (tACDMainActor.AttributesGetInt(ATTRIBUTE_FREE_CAST_ALL))
        {
            tACDMainActor.HealFromResourceSpent(flResourceCost, eResourceType);
        }
    }
    else
    {
        if (flResourceCost > EPSILON)
        {
            BOOL bIgnoreMinimumResourceCost = PowerIsChannelled(tACDMainActor, AutoPowerDefinitionR(snoPower)) ||
                                              PowerShouldUseClientWalking(tACDMainActor, AutoPowerDefinitionR(snoPower)) || bOverriddenCosts;
            BOOL bResourceSpent = tACDMainActor.SpendResource(flResourceCost, bIgnoreMinimumResourceCost, eResourceType, ptPower, TRUE);
            if (!bResourceSpent)
            {
                ptPower->dwFlags |= 0x20;
                return FALSE;
            }
        }

        if (flChargesCost > EPSILON)
        {
            int32 nChargesCost = Math::FloatRoundToInt(flChargesCost);
            int32 nCurrCharges = tACDMainActor.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_CHARGES, snoPower));
            if (nCurrCharges < nChargesCost)
            {
                ptPower->dwFlags |= 0x20;
                return FALSE;
            }

            int32 nMaxCharges = PowerActorGetMaxCharges(tACDMainActor, snoPower);
            nCurrCharges = Math::Max(nCurrCharges - nChargesCost, 0);
            nCurrCharges = Math::Min(nMaxCharges, nCurrCharges);
            nCurrCharges = Math::Min(nCurrCharges, 99);
            tACDMainActor.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL_CHARGES, snoPower), nCurrCharges);
            SPlayerRegisterPowerForRecharge(PlayerGetByACD(tACDMainActor.id), snoPower);
        }
    }

    return TRUE;
}

// PowerAPI.cpp:7448
// inferred: health-cost scalar applied as a second statement (fmul rows at 7488 and 7491); skip test spelled == FALSE (7522:21 cbnz,
// the 7541 `!bSkipChargingHealth` argument is a separate eor); RESOURCE_TYPE_INVALID from the 7541 column offsets
int32 PowerAPI_HealthCostsForTargets(PowerID idPower, int32 nNumTargets)
{
    int32 nTargetsPaidFor = 0;
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
        if (ptACD && ptACD->IsAlive())
        {
            const Actor * ptActor = ActorGet((ActorID)ACDGetOwner(ptACD->id));
            PowerAPICosts tCostInfo;
            PowerHelper_CostsInit(idPower, tCostInfo);
            ACDID idACDMainActor = tCostInfo.idACD;
            Power * ptLocPower = PowerGet(tCostInfo.idPower);
            SNO snoPower = ptLocPower->snoPower;
            ScopedPowerDefinitionLockR pdefPower(snoPower);
            ActorCommonData & tACDMainActor = ACDGet(idACDMainActor);
            float flHealthCostDiscount = Math::Pin(tACDMainActor.AttributesGetFloat(ATTRIBUTE_HEALTH_COST_DISCOUNT), 0.0f, 1.0f);
            BOOL bSkipChargingHealth = FALSE;
            if (ptACD->AttributesGetInt(ATTRIBUTE_FREE_CAST_ALL) || ActorFlagIsSet(ptActor->id, ACTOR_GOD_MODE_BIT) || flHealthCostDiscount >= 0.999999f)
                bSkipChargingHealth = TRUE;
            float flHealthCost = 0.0f;
            float flHealthCostDiscounted = 0.0f;
            nTargetsPaidFor = nNumTargets;
            {
                float flHealthCostPerTargetPercent = PowerGetFormulaValue(ptACD, AutoPowerDefinitionR(pdefPower), TAG_POWER_COST_HEALTH_PER_TARGET, ptPower->nSkillLevel, ptPower->idFastAttrib, 0);
                float flHealthCostPerTarget = flHealthCostPerTargetPercent * ptACD->AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_TOTAL);
                flHealthCostPerTarget *= tACDMainActor.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_HEALTH_COST_SCALAR, snoPower)) + 1.0f;
                float flHPCurrent = ptACD->GetHitPointsCurrent();
                float flHealthCostPerTargetDiscounted = (1.0f - flHealthCostDiscount) * flHealthCostPerTarget;
                int32 nTargetsICanAfford = nNumTargets;
                if (flHealthCostPerTargetDiscounted > EPSILON)
                    nTargetsICanAfford = Math::FloatFloorToInt(flHPCurrent / flHealthCostPerTargetDiscounted);
                nTargetsICanAfford = Math::Pin(nTargetsICanAfford, 0, nNumTargets);
                flHealthCost = flHealthCostPerTarget * nTargetsICanAfford;
                flHealthCostDiscounted = flHealthCostPerTargetDiscounted * nTargetsICanAfford;
                if (bSkipChargingHealth == FALSE)
                {
                    tACDMainActor.SetHitPoints(Math::Max(1.0f, flHPCurrent - flHealthCostDiscounted), TRUE, TRUE);
                    nTargetsPaidFor = nTargetsICanAfford;
                }
            }
            if (flHealthCost > 0.0f)
                sSendHealthCostAchievementEvent(&tACDMainActor, ptPower, nNumTargets);
            PowerAPI_BuffApplyOnCostEffects(idACDMainActor, ptLocPower->id, PowerGetSNO(tCostInfo.idPower), 0.0f, RESOURCE_TYPE_INVALID, flHealthCostDiscounted, !bSkipChargingHealth, flHealthCost);
        }
    }
    return nTargetsPaidFor;
}
// PowerAPI.cpp:7550
void PowerAPI_MissileImpact(const PowerID idPower, void * pData)
{
    ImpactData * ptImpact = (ImpactData *)pData;
    if (ptImpact)
    {
        ACDID idACDTarget = ptImpact->idACDIntersected;
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget && ptACDTarget->IsAlive())
        {
            PowerAPIAttackPayload tPayload;
            PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
            PowerAPI_DoPayload(tPayload, idACDTarget, ACDID_INVALID);
        }
        ActorID idProjectile = (ActorID)ACDGetOwner(ptImpact->idACDProjectile);
        if (idProjectile != ACTORID_INVALID)
            ActorFlagForDeletion(idProjectile);
    }
}

// PowerAPI.cpp:7586
void PowerAPI_MissileImpactOnce(const PowerID idPower, void * pData)
{
    ImpactData * ptImpact = (ImpactData *)pData;
    if (ptImpact)
    {
        ACDID idACDTarget = ptImpact->idACDIntersected;
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget && ptACDTarget->IsAlive())
        {
            PowerAPIAttackPayload tPayload;
            PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
            PowerAPI_DoPayload(tPayload, idACDTarget, ACDID_INVALID);
            PowerAddAffectedACD(idPower, idACDTarget, ACDID_INVALID);
        }
        ActorID idProjectile = (ActorID)ACDGetOwner(ptImpact->idACDProjectile);
        if (idProjectile != ACTORID_INVALID)
            ActorFlagForDeletion(idProjectile);
    }
}

// PowerAPI.cpp:7683
void PowerAPI_ForceTarget(ACDID idACDReceiver, ACDID idACDForcedTarget)
{
    ActorCommonData * ptACDReceiver = ACDTryToGet(idACDReceiver);
    if (ptACDReceiver)
        ptACDReceiver->AttributesSetInt(ATTRIBUTE_FORCED_ENEMY_ACDID, idACDForcedTarget);
}

// PowerAPI.cpp:7850
static void sCopyAIStateList(XPooledDataIDList & listDest,
                             const XPooledDataIDList & listSrc)
{
    POS pos = listSrc.GetHeadPosition();
    while (pos)
    {
        SNO snoAIState = listSrc.GetNext(pos);
        listDest.AddTail_Tracked(snoAIState);
    }
}

// PowerAPI.cpp:7863
static void sMergeAIStatesOntoActor(
    ActorCommonData & tACD,
    const PowerAPIBuffInstance & tBuff)
{
    ActorID idActor = (ActorID)ACDGetOwner(tACD.id);
    POS pos = tBuff.listAIStatesToMerge.GetHeadPosition();
    while (pos)
    {
        SNO snoAIState = tBuff.listAIStatesToMerge.GetNext(pos);
        AIStateMerge(idActor, snoAIState, 0, tBuff.id);
    }
}

// PowerAPI.cpp:7878
PowerAPIBuffStartData::PowerAPIBuffStartData()
{
    m_bInitialized = FALSE;
}

// PowerAPI.cpp:7885
PowerAPIBuffStartData::PowerAPIBuffStartData(PowerID idPower)
{
    m_bInitialized = FALSE;
    _Initialize(idPower);
}

// PowerAPI.cpp:7893
PowerAPIBuffStartData::~PowerAPIBuffStartData()
{
    if (m_bInitialized)
    {
        FastAttribGroupDelete(idFastAttrib);
    }
    listAIStatesToMerge.RemoveAll();
}

// PowerAPI.cpp:7903
// Statement order reconstructed from the line table (each merged store/memset carries the line of one of its statements) and
// from how the compiler merged the zero stores: a bitfield or non-zero assignment ends a run of mergeable zero stores, so the
// memset boundaries of the original (and of the copy inlined into sPowerAPI_CheckForOnHitProcs) fix where those sit. Which
// bitfield stands at which of those places is not recoverable (they fold into one load at line 7908 and one store at 8047).
void PowerAPIBuffStartData::_Initialize(PowerID _idPower)
{
    idFastAttrib = FastAttribLightCreate(0);
    idPower = _idPower;
    bPositiveEffect = FALSE;
    bRemoveOnDeath = TRUE;
    bRemoveOnRespec = FALSE;
    bRemoveOnPassiveApplication = TRUE;
    bRemoveOnGreaterRiftEntry = FALSE;
    bKeepForProfileSnapshot = FALSE;
    bTriggersHitProcs = FALSE;
    bUndispellable = FALSE;
    bDoNotCopy = FALSE;
    bIsCopy = FALSE;
    bHitsOnFirstTick = FALSE;
    bTriggerOnHitForPets = FALSE;
    bTriggerOnDamageForPets = FALSE;
    bProcessDamageShieldOnStaggerDamage = FALSE;
    eStackType = 0;
    dwStartTick = 0;
    dwDurationMin = 0;
    dwDurationDelta = 0;
    dwRefreshRate = 0;
    dwInternalCooldownDuration = 0;
    bInfinite = FALSE;
    nBuffLevel = 0;
    nMaxStackCount = 0;
    nMaxInstanceCount = 0;
    pPeriodicSpellEffect = NULL;
    pPeriodicSpellEffectData = NULL;
    pOnDoDamageEffect = NULL;
    pOnDoDamageEffectData = NULL;
    pOnGetHitEffect = NULL;
    pOnGetHitEffectData = NULL;
    pOnHitEffect = NULL;
    pOnHitEffectData = NULL;
    pOnFirstHitAttemptEffect = NULL;
    pOnFirstHitAttemptEffectData = NULL;
    pOnAttackEffect = NULL;
    pOnAttackEffectData = NULL;
    pOnDodgeEffect = NULL;
    pOnDodgeEffectData = NULL;
    pOnCostEffect = NULL;
    pOnCostEffectData = NULL;
    pOnHealFromSkillEffect = NULL;
    pOnKillEffect = NULL;
    pOnKillEffectData = NULL;
    pOnAssistEffect = NULL;
    pOnAssistEffectData = NULL;
    pOnProjectileCreateEffect = NULL;
    pOnProjectileCreateEffectData = NULL;
    pOnKnockbackLandEffect = NULL;
    pOnItemPickUpEffect = NULL;
    pOnItemPickUpEffectData = NULL;
    pDamageShieldEffect = NULL;
    pDamageShieldEffectData = NULL;
    bDamageShieldIsEvaluatedLast = FALSE;
    nDamageShieldPriority = 0;
    pExpirationEffect = NULL;
    pExpirationEffectData = NULL;
    pRemovalEffect = NULL;
    pRemovalEffectData = NULL;
    pDeathEffect = NULL;
    pDeathEffectData = NULL;
    pReflectEffect = NULL;
    pShouldReflectTest = NULL;
    pReflectEffectData = NULL;
    pOnPetAddEffect = NULL;
    pOnPetAddEffectData = NULL;
    pOnResourceGainEffect = NULL;
    pOnGetHitWithCrowdControlEffect = NULL;
    pOngetHitwithCrowdControlData = NULL;
    pOnCalloutEventEffect = NULL;
    pOnGizmoOperatedEffect = NULL;
    pOnWreckableDestroyedEffect = NULL;
    pOnTeleportedEffect = NULL;
    pOnPowerDamageInitEffect = NULL;
    pOnLootDropEffect = NULL;
    pOnItemSoldEffect = NULL;
    pOnLevelEffect = NULL;
    pOnCorpseConsumedEffect = NULL;
    pOnPetDismissEffect = NULL;
    pOnHitWithCrowdControlEffect = NULL;
    pOnEquipmentChangeEffect = NULL;
    pOnSkillChangeEffect = NULL;
    pOnMessageEffect = NULL;
    nAppearanceParameter = -1;
    nLookOverrideHash = 0;
    eDamageType = -1;
    flPrimaryAttributeBonusMultiplier = 1.0f;
    flDamagePerSecond = 0.0f;
    snoDOTDamageSourcePower = SNO_NONE;
    flPetDamageScalar = 1.0f;
    flClassBonusMultiplier = 1.0f;
    flSkillsBonusMultiplier = 1.0f;
    bAppliedDamageReductionEffects = FALSE;
    bDOTCrit = FALSE;
    bDOTCanCrit = TRUE;
    bHideDOTDamageEffects = FALSE;
    bStun = FALSE;
    bFear = FALSE;
    bFreeze = FALSE;
    bImmobilize = FALSE;
    bBlind = FALSE;
    bCharm = FALSE;
    bTaunt = FALSE;
    bSlow = FALSE;
    bForceGrip = FALSE;
    bAttackSpeedReduction = FALSE;
    bChill = FALSE;
    bFailsIfStunImmune = FALSE;
    bFailsIfFearImmune = FALSE;
    bFailsIfFreezeImmune = FALSE;
    bFailsIfForceGripImmune = FALSE;
    bFailsIfImmobilizeImmune = FALSE;
    bFailsIfBlindImmune = FALSE;
    bFailsIfCharmImmune = FALSE;
    bFailsIfSlowImmune = FALSE;
    bFailsIfAttackSpeedReductionImmune = FALSE;
    bFailsIfChillImmune = FALSE;
    bAura = FALSE;
    flSlowAmount = 0.0f;
    flChillMagnitude = 0.0f;
    flAttackSpeedReductionAmount = 0.0f;
    nBuffIndex = 0;
    nBuffData = 0;
    flBuffData = 0.0f;
    eAppearanceAttribute = ATTRIBUTE_BUFF_VISUAL_EFFECT;
    flShieldAmount = 0.0f;
    nAuraPriority = 0;
    bChanneled = FALSE;
    bUsesLockout = FALSE;
    bUseHighestElementalDamageBonus = FALSE;
    bSaveCooldown = FALSE;
    bSuppressTauntEffects = FALSE;
    listAIStatesToMerge.SetPool(MemoryPoolsGetDATAIDListPool());
    eExclusiveType = 0;
    bUsePowersProcScalar = TRUE;
    if (_idPower != POWER_NONE)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(_idPower);
        idACDSource = ptACDOwner ? ptACDOwner->id : ACDID_INVALID;
        snoPower = PowerGetSNO(_idPower);
        nBuffLevel = PowerGetLevel(_idPower);
        int32 nDurationMin = Math::FloatToInt(PowerGetFormulaValue(_idPower, TAG_POWER_BUFF_DURATION_MIN) * TICKS_PER_SECOND);
        int32 nDurationDelta = Math::FloatToInt(PowerGetFormulaValue(_idPower, TAG_POWER_BUFF_DURATION_DELTA) * TICKS_PER_SECOND);
        int32 nDurationBetweenPulses = Math::FloatToInt(PowerGetFormulaValue(_idPower, TAG_POWER_BUFF_DURATION_BETWEEN_PULSES) * TICKS_PER_SECOND);
        dwDurationMin = Math::Max(nDurationMin, 0);
        dwDurationDelta = Math::Max(nDurationDelta, 0);
        dwRefreshRate = Math::Max(nDurationBetweenPulses, 0);
        snoEndingEffect = SNO_NONE;
        Power * ptPower = PowerTryToGet(_idPower);
        if (ptPower)
            flProcScalar = PowerGetProcScalar(*ptPower);
        if (PowerContactTagGetBool(_idPower, TAG_POWER_PAYLOAD_AFFECTED_BY_PET_AOE_SCALAR))
        {
            float flScalar;
            ScopedGlobalDefinitionLockR pdefGlobal;
            flScalar = Math::Max(0.0f, pdefGlobal->ptServerData->flPetAoEDamageScalar);
            flPetDamageScalar = flScalar;
        }
    }
    else
    {
        nBuffLevel = 0;
        snoPower = SNO_NONE;
        dwDurationMin = 0;
        dwDurationDelta = 0;
        dwRefreshRate = 0;
        idACDSource = ACDID_INVALID;
        snoEndingEffect = SNO_NONE;
        flProcScalar = 0.0f;
    }
    m_bInitialized = TRUE;
}

// PowerAPI.cpp:8109

void PowerAPIBuffStartData::Initialize()
{
    _Initialize(POWER_NONE);
}

// PowerAPI.cpp:8255. Uses the existing chained-array lookup and TLS accessors.
PowerAPIBuffInstance * PowerAPI_BuffTryToGet(BuffID idBuff)
{
    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    return ptPowerGlobals->pArrayBuffs->ElementTryToGet(idBuff);
}

// PowerAPI.cpp:8265
static void sBuffPulseSpellEffect(
    PowerID,
    void * pData)
{
    BuffID idBuff = (BuffID)(int64)pData;
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (!ptBuff)
    {
        return;
    }

    if (ptBuff->bDeleted)
    {
        return;
    }

    if (!PowerTryToGet(ptBuff->idPower))
    {
        return;
    }

    uint32 dwNow = GameGetServerTime();


    if (dwNow > ptBuff->dwStartTick + ptBuff->dwDuration && !ptBuff->bInfinite)
    {
        return;
    }

    ActorCommonData * ptACDAffected = ACDTryToGet(ptBuff->idACDAffected);
    if (!ptACDAffected)
    {
        return;
    }

    if (ptBuff->bIsChanneled)
    {
        uint32 dwChannelLockoutTime = ptACDAffected->AttributesGetInt(ATTRIBUTE_POWER_CHANNEL_LOCKOUT_TIME);
        if (dwChannelLockoutTime > dwNow)
        {
            PowerTriggerAddTimerTrigger(ptBuff->idPower, dwChannelLockoutTime - dwNow, sBuffPulseSpellEffect, (void *)(int64)idBuff);
            return;
        }
    }
    else if (ptBuff->bUsesLockout)
    {
        uint32 dwBuffLockoutTime = PowerBuffGetLockoutTime(*ptACDAffected, ptBuff);
        if (dwBuffLockoutTime > dwNow)
        {
            PowerTriggerAddTimerTrigger(ptBuff->idPower, dwBuffLockoutTime - dwNow, sBuffPulseSpellEffect, (void *)(int64)idBuff);
            return;
        }
    }

    uint32 dwLockoutTime = ptBuff->dwRefreshRate;


    if (!ptBuff->pPeriodicSpellEffect)
    {
        return;
    }

    ptBuff->pPeriodicSpellEffect(ptBuff, ptBuff->pPeriodicSpellEffectData);



    if (!ptBuff->bDeleted)
    {
        if (ptBuff->bIsChanneled)
        {
            ptACDAffected->AttributesSetInt(ATTRIBUTE_POWER_CHANNEL_LOCKOUT_TIME, dwLockoutTime + dwNow);
        }
        else if (ptBuff->bUsesLockout)
        {
            PowerBuffSetLockoutTime(*ptACDAffected, ptBuff, dwLockoutTime + dwNow);
        }
    }

    if (ptBuff->dwRefreshRate)
    {
        PowerTriggerAddTimerTrigger(ptBuff->idPower, ptBuff->dwRefreshRate, sBuffPulseSpellEffect, (void *)(int64)idBuff);
    }
}

// PowerAPI.cpp:8380
static void sBuffDoneCallback(TimerID idTimer, DataID id, void * lpData)
{
    BuffID idBuff = id;
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (!ptBuff)
        return;
    if (ptBuff->idPower != POWER_NONE)
        PowerTryToGet(ptBuff->idPower);
    if (ptBuff->bInfinite)
    {
        ptBuff->idTimerDone = TimerRegister(idBuff, TIMER_OWNER_BUFF, ptBuff->dwDuration, NULL, sBuffDoneCallback);
    }
    else
    {
        if (ptBuff->pExpirationEffect)
            ptBuff->pExpirationEffect(ptBuff, ptBuff->idACDAffected, ptBuff->pExpirationEffectData);
        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, 1, TRUE);  // BUFF_REMOVAL_REASON_EXPIRED
    }
}

// PowerAPI.cpp:8416
static void sTriggerCCCallbacksOnRefresh(
    PowerAPIBuffInstance & tBuff)
{
    AttribGroupID idGroup = tBuff.idFastAttrib;
    if (FastAttribGroupIsValid(idGroup) && FastAttribGetValueInt(idGroup, ATTRIBUTE_STUNNED))
    {
        PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_STUN, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
        PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_STUN, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
    }
    if (FastAttribGroupIsValid(idGroup) && FastAttribGetValueInt(idGroup, ATTRIBUTE_FORCE_GRIPPED))
    {
        PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_FORCE_GRIP, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
        PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_FORCE_GRIP, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
    }
    if (FastAttribGroupIsValid(idGroup) && FastAttribGetValueInt(idGroup, ATTRIBUTE_FROZEN))
    {
        PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_FREEZE, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
        PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_FREEZE, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
    }
    if (FastAttribGroupIsValid(idGroup) && FastAttribGetValueInt(idGroup, ATTRIBUTE_BLIND))
    {
        PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_BLIND, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
        PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_BLIND, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
    }
    if (FastAttribGroupIsValid(idGroup) && FastAttribGetValueInt(idGroup, ATTRIBUTE_FEARED))
    {
        PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_FEAR, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
        PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_FEAR, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
    }
    if (FastAttribGroupIsValid(idGroup) && FastAttribGetValueInt(idGroup, ATTRIBUTE_IMMOBOLIZED))
    {
        PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_IMMOBILIZE, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
        PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_IMMOBILIZE, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
    }
    if (FastAttribGroupIsValid(idGroup) && FastAttribGetValueInt(idGroup, ATTRIBUTE_CHARMED))
    {
        PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_CHARM, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
        PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_CHARM, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
    }
    if (FastAttribGroupIsValid(idGroup) && FastAttribGetValueInt(idGroup, ATTRIBUTE_TAUNTED))
    {
        PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_TAUNT, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
        PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_TAUNT, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
    }
    if (FastAttribGroupIsValid(idGroup) && FastAttribGetValueInt(idGroup, ATTRIBUTE_SLOW))
    {
        PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_SLOW, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
        PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_SLOW, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
    }
    if (FastAttribGroupIsValid(idGroup) && FastAttribGetValueInt(idGroup, ATTRIBUTE_CHILLED))
    {
        PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_CHILL, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
        PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_CHILL, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
    }
    if (FastAttribGroupIsValid(idGroup) && FastAttribGetValueInt(idGroup, ATTRIBUTE_ATTACK_SLOW))
    {
        PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_ATTACKSPEED_SLOW, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
        PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuff.idPower, tBuff.idACDSource, tBuff.idACDAffected, tBuff.snoPower, CROWD_CONTROL_ATTACKSPEED_SLOW, tBuff.dwDuration, tBuff.flProcScalar, TRUE);
    }
}

// PowerAPI.cpp:8477
void sUpdateCCResistance(
    ActorCommonData & tACD,
    uint32 dwNewDuration,
    float flCurrentCCResistance)
{
    uint32 dwNow = GameGetServerTime();

    uint32 dwCCProjectedEndTime = Math::Max<uint32>(tACD.AttributesGetInt(ATTRIBUTE_CROWD_CONTROL_PROJECTED_END_TIME), dwNow);

    uint32 dwNewEndTime = dwNow + dwNewDuration;

    if (dwNewEndTime > dwCCProjectedEndTime)
    {
        float flAdditionalCCResistance = (float)(dwNewEndTime - dwCCProjectedEndTime) * 0.1f / 60.0f;
        if (flCurrentCCResistance >= 0.9f)
        {
            flAdditionalCCResistance = Math::Max(0.2f, flAdditionalCCResistance);
        }
        tACD.AttributesSetFloat(ATTRIBUTE_CROWD_CONTROL_RESISTANCE, flCurrentCCResistance + flAdditionalCCResistance);
        tACD.AttributesSetInt(ATTRIBUTE_CROWD_CONTROL_PROJECTED_END_TIME, dwNewEndTime);
    }
}

// PowerAPI.cpp:8504
BuffID PowerAPI_BuffAddToACD(
    const PowerAPIBuffStartData & tBuffStartData,
    ActorCommonData & tACD)
{
    if (!tBuffStartData.IsInitialized())
    {
        DisplayInternalError(NULL, "", 8568, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    Power * ptPower = PowerTryToGet(tBuffStartData.idPower);

    if (ptPower)
    {
        if (!tBuffStartData.bPositiveEffect)
        {
            ptPower->dwFlags |= (1 << PF_CONSOLE_HAS_DAMAGED_OR_BUFFED);
        }
    }

    ActorID idActor = (ActorID)ACDGetOwner(tACD.id);
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor || (ptActor->dwActorFlags & (1 << ACTOR_DELETING_BIT)))
    {
        return BUFFID_INVALID;
    }

    ACDID idACDForwardee = (ACDID)tACD.AttributesGetInt(ATTRIBUTE_ACTOR_FORWARDS_BUFFS);
    if (idACDForwardee != ACDID_INVALID)
    {
        ActorCommonData * ptACDForward = ACDTryToGet(idACDForwardee);
        if (!ptACDForward)
        {
            return BUFFID_INVALID;
        }

        return PowerAPI_BuffAddToACD(tBuffStartData, *ptACDForward);
    }

    if (PowerAPI_BuffIsSnare(tBuffStartData) && tACD.AttributesGetInt(ATTRIBUTE_SLOWDOWN_IMMUNE))
    {
        sSendImmuneMessage(tACD.id, tBuffStartData.idPower);
        return BUFFID_INVALID;
    }

    uint32 dwNewDuration = RandRange_Internal(SGameGlobalsGet().rng, tBuffStartData.dwDurationMin, tBuffStartData.dwDurationDelta);

    if (!tBuffStartData.bPositiveEffect && !tBuffStartData.bUndispellable && !tBuffStartData.bInfinite)
    {
        float flDebuffDurationReductionPercent = tACD.AttributesGetFloat(ATTRIBUTE_DEBUFF_DURATION_REDUCTION_PERCENT);
        if (flDebuffDurationReductionPercent > EPSILON)
        {
            dwNewDuration = Math::Max<uint32>((uint32)(dwNewDuration * (1.0f - flDebuffDurationReductionPercent)), 1);
        }
    }

    if (tBuffStartData.bInfinite)
    {
        dwNewDuration = 0;
    }
    else if (ptPower)
    {
        float flTotalBonusPercent = 1.0f;

        if (tBuffStartData.bStun)
        {
            flTotalBonusPercent = Math::Max(flTotalBonusPercent, FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_STUN_DURATION_BONUS_PERCENT) + 1.0f);
        }
        if (tBuffStartData.bImmobilize)
        {
            flTotalBonusPercent = Math::Max(flTotalBonusPercent, FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_IMMOBILIZE_DURATION_BONUS_PERCENT) + 1.0f);
        }
        if (tBuffStartData.bCharm)
        {
            flTotalBonusPercent = Math::Max(flTotalBonusPercent, FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_CHARM_DURATION_BONUS_PERCENT) + 1.0f);
        }
        if (tBuffStartData.bBlind)
        {
            flTotalBonusPercent = Math::Max(flTotalBonusPercent, FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_BLIND_DURATION_BONUS_PERCENT) + 1.0f);
        }
        if (tBuffStartData.bFear)
        {
            flTotalBonusPercent = Math::Max(flTotalBonusPercent, FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_FEAR_DURATION_BONUS_PERCENT) + 1.0f);
        }
        if (tBuffStartData.bFreeze)
        {
            flTotalBonusPercent = Math::Max(flTotalBonusPercent, FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_FREEZE_DURATION_BONUS_PERCENT) + 1.0f);
        }
        if (tBuffStartData.bChill)
        {
            flTotalBonusPercent = Math::Max(flTotalBonusPercent, FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_CHILL_DURATION_BONUS_PERCENT) + 1.0f);
        }
        if (tBuffStartData.bSlow)
        {
            flTotalBonusPercent = Math::Max(flTotalBonusPercent, FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_SLOW_DURATION_BONUS_PERCENT) + 1.0f);
        }

        if (flTotalBonusPercent > 1.0f)
        {
            dwNewDuration = Math::FloatToInt(flTotalBonusPercent * dwNewDuration);
        }
    }

    const GameBalanceDefinition * pdefGBEliteMods = NULL;
    const EliteModifierData * pEliteMods = NULL;
    tACD.GetEliteModifierData(&pdefGBEliteMods, &pEliteMods);

    BOOL bEliteStunImmune = FALSE;
    BOOL bEliteImmobilizeImmune = FALSE;
    BOOL bEliteForceGripImmune = FALSE;
    BOOL bEliteFearImmune = FALSE;
    BOOL bEliteFreezeImmune = FALSE;
    BOOL bEliteBlindImmune = FALSE;
    BOOL bEliteCharmImmune = FALSE;
    BOOL bEliteSlowImmune = FALSE;
    BOOL bEliteChillImmune = FALSE;
    BOOL bEliteAttackSpeedReductionImmune = FALSE;
    float flSlowAmount = tBuffStartData.flSlowAmount;
    float flChillMagnitude = tBuffStartData.flChillMagnitude;

    ScopedGlobalDefinitionLockR pdefGlobal;

    if (flChillMagnitude < EPSILON && tBuffStartData.bChill)
    {
        flChillMagnitude = pdefGlobal->ptServerData->flChillDefaultMagnitude;
    }

    float flAttackSpeedReductionAmount = tBuffStartData.flAttackSpeedReductionAmount;

    BOOL bAlreadySlowed = tACD.AttributesGetInt(ATTRIBUTE_SLOW);
    BOOL bAlreadyAttackSlowed = tACD.AttributesGetInt(ATTRIBUTE_ATTACK_SLOW);
    BOOL bAlreadyChilled = tACD.AttributesGetInt(ATTRIBUTE_CHILLED);

    uint32 dwOriginalDuration = dwNewDuration;

    float flCCReductionPercent = Math::Pin(1.0f - tACD.AttributesGetFloat(ATTRIBUTE_CROWD_CONTROL_REDUCTION), 0.0f, 1.0f);

    if ((GameTypeIsPvP() || tACD.IsDueling()) && ACDIsPlayer(tACD.id))
    {
        ScopedGlobalDefinitionLockR pdefGlobalScoped;

        flCCReductionPercent = Math::Pin(flCCReductionPercent * (1.0f - pdefGlobalScoped->ptServerData->flPVPPlayerCCReduction), 0.0f, 1.0f);
    }

    float flCurrentCCResistance = tACD.AttributesGetFloat(ATTRIBUTE_CROWD_CONTROL_RESISTANCE);
    BOOL bUpdateCCResistance = FALSE;

    if (tBuffStartData.bStun)
    {
        uint32 dwModifiedDuration = dwOriginalDuration;
        if (pEliteMods)
        {
            float flMaxCCReduction = Math::Min(pEliteMods->flStunDR, flCurrentCCResistance);

            dwModifiedDuration = Math::FloatToInt((1.0f - flMaxCCReduction) * dwOriginalDuration);
            bEliteStunImmune = (int32)dwModifiedDuration < pEliteMods->nStunDM;
        }

        dwModifiedDuration = (uint32)(flCCReductionPercent * dwModifiedDuration);

        if (tACD.AttributesGetInt(ATTRIBUTE_STUN_IMMUNE) || bEliteStunImmune || tACD.IsGizmo())
        {
            PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuffStartData.idPower, tBuffStartData.idACDSource, tACD.id, tBuffStartData.snoPower, CROWD_CONTROL_STUN, dwModifiedDuration, tBuffStartData.flProcScalar, FALSE);
            if (tBuffStartData.bFailsIfStunImmune)
            {
                if (!bEliteStunImmune)
                {
                    sSendImmuneMessage(tACD.id, tBuffStartData.idPower);
                }
                if (pdefGBEliteMods)
                {
                    GBUnlockR(pdefGBEliteMods);
                }
                return BUFFID_INVALID;
            }
        }
        else
        {
            bUpdateCCResistance = TRUE;
            dwNewDuration = Math::Min(dwModifiedDuration, dwNewDuration);
        }
    }

    if (tBuffStartData.bImmobilize)
    {
        ActorMovement * ptActorMovement = tACD.GetActorMovement();
        if (ptActorMovement && ((ptActorMovement->m_uMoveFlags & (1 << MOVEBIT_FROM_POWER)) || ptActorMovement->IsInArcingMovement()) && tACD.IsMonster()) bEliteImmobilizeImmune = TRUE;

        uint32 dwModifiedDuration = dwOriginalDuration;
        if (pEliteMods)
        {
            float flMaxCCReduction = Math::Min(pEliteMods->flRootDR, flCurrentCCResistance);

            dwModifiedDuration = Math::FloatToInt((1.0f - flMaxCCReduction) * dwOriginalDuration);
            if ((int32)dwModifiedDuration < pEliteMods->nRootDM) bEliteImmobilizeImmune = TRUE;
        }

        dwModifiedDuration = (uint32)(flCCReductionPercent * dwModifiedDuration);

        if (tACD.AttributesGetInt(ATTRIBUTE_ROOT_IMMUNE) || bEliteImmobilizeImmune)
        {
            PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuffStartData.idPower, tBuffStartData.idACDSource, tACD.id, tBuffStartData.snoPower, CROWD_CONTROL_IMMOBILIZE, dwModifiedDuration, tBuffStartData.flProcScalar, FALSE);
            if (tBuffStartData.bFailsIfImmobilizeImmune)
            {
                if (!bEliteImmobilizeImmune)
                {
                    sSendImmuneMessage(tACD.id, tBuffStartData.idPower);
                }
                if (pdefGBEliteMods)
                {
                    GBUnlockR(pdefGBEliteMods);
                }
                return BUFFID_INVALID;
            }
        }
        else
        {
            bUpdateCCResistance = TRUE;
            dwNewDuration = Math::Min(dwModifiedDuration, dwNewDuration);
        }
    }

    if (tBuffStartData.bForceGrip)
    {
        if ((tBuffStartData.idPower == POWERID_INVALID ||
             tACD.AttributesGetInt(ATTRIBUTE_LAST_CONTINUOUS_KNOCKBACK_POWER_ID) != tBuffStartData.idPower) &&
            flCurrentCCResistance >=
            0.65f)
        {
            bEliteForceGripImmune = TRUE;
        }

        uint32 dwModifiedDuration = dwOriginalDuration;
        if (pEliteMods)
        {
            float flMaxCCReduction = Math::Min(pEliteMods->flStunDR, flCurrentCCResistance);

            dwModifiedDuration = Math::FloatToInt((1.0f - flMaxCCReduction) * dwOriginalDuration);
            if ((int32)dwModifiedDuration < pEliteMods->nStunDM) bEliteForceGripImmune = TRUE;
        }

        dwModifiedDuration = (uint32)(flCCReductionPercent * dwModifiedDuration);

        if (tACD.AttributesGetInt(ATTRIBUTE_STUN_IMMUNE) || bEliteForceGripImmune || !PowerActorCanBeKnockedBack(idActor, tBuffStartData.idPower))
        {
            PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuffStartData.idPower, tBuffStartData.idACDSource, tACD.id, tBuffStartData.snoPower, CROWD_CONTROL_FORCE_GRIP, dwModifiedDuration, tBuffStartData.flProcScalar, FALSE);
            if (tBuffStartData.bFailsIfForceGripImmune)
            {
                if (!bEliteForceGripImmune)
                {
                    sSendImmuneMessage(tACD.id, tBuffStartData.idPower);
                }
                if (pdefGBEliteMods)
                {
                    GBUnlockR(pdefGBEliteMods);
                }
                return BUFFID_INVALID;
            }
        }
        else
        {
            bUpdateCCResistance = TRUE;
            dwNewDuration = Math::Min(dwModifiedDuration, dwNewDuration);
        }
    }

    if (tBuffStartData.bFreeze)
    {
        uint32 dwModifiedDuration = dwOriginalDuration;
        if (pEliteMods)
        {
            float flMaxCCReduction = Math::Min(pEliteMods->flStunDR, flCurrentCCResistance);

            dwModifiedDuration = Math::FloatToInt((1.0f - flMaxCCReduction) * dwOriginalDuration);
            bEliteFreezeImmune = (int32)dwModifiedDuration < pEliteMods->nStunDM;
        }

        dwModifiedDuration = (uint32)(flCCReductionPercent * dwModifiedDuration);

        if (tACD.AttributesGetInt(ATTRIBUTE_FREEZE_IMMUNE) || bEliteFreezeImmune)
        {
            PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuffStartData.idPower, tBuffStartData.idACDSource, tACD.id, tBuffStartData.snoPower, CROWD_CONTROL_FREEZE, dwModifiedDuration, tBuffStartData.flProcScalar, FALSE);
            if (tBuffStartData.bFailsIfFreezeImmune)
            {
                if (!bEliteFreezeImmune)
                {
                    sSendImmuneMessage(tACD.id, tBuffStartData.idPower);
                }
                if (pdefGBEliteMods)
                {
                    GBUnlockR(pdefGBEliteMods);
                }
                return BUFFID_INVALID;
            }
        }
        else
        {
            bUpdateCCResistance = TRUE;
            dwNewDuration = Math::Min(dwModifiedDuration, dwNewDuration);
        }
    }

    if (tBuffStartData.bFear)
    {
        uint32 dwModifiedDuration = dwOriginalDuration;
        if (pEliteMods)
        {
            float flMaxCCReduction = Math::Min(pEliteMods->flFearDR, flCurrentCCResistance);

            dwModifiedDuration = Math::FloatToInt((1.0f - flMaxCCReduction) * dwOriginalDuration);
            bEliteFearImmune = (int32)dwModifiedDuration < pEliteMods->nFearDM;
        }

        dwModifiedDuration = (uint32)(flCCReductionPercent * dwModifiedDuration);

        if (tACD.AttributesGetInt(ATTRIBUTE_FEAR_IMMUNE) || bEliteFearImmune)
        {
            PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuffStartData.idPower, tBuffStartData.idACDSource, tACD.id, tBuffStartData.snoPower, CROWD_CONTROL_FEAR, dwModifiedDuration, tBuffStartData.flProcScalar, FALSE);
            if (!bEliteFearImmune)
            {
                sSendImmuneMessage(tACD.id, tBuffStartData.idPower);
            }

            if (tBuffStartData.bFailsIfFearImmune)
            {
                if (pdefGBEliteMods)
                {
                    GBUnlockR(pdefGBEliteMods);
                }
                return BUFFID_INVALID;
            }
        }
        else
        {
            bUpdateCCResistance = TRUE;
            dwNewDuration = Math::Min(dwModifiedDuration, dwNewDuration);
        }
    }

    if (tBuffStartData.bBlind)
    {
        uint32 dwModifiedDuration = dwOriginalDuration;
        if (pEliteMods)
        {
            float flMaxCCReduction = Math::Min(pEliteMods->flStunDR, flCurrentCCResistance);

            dwModifiedDuration = Math::FloatToInt((1.0f - flMaxCCReduction) * dwOriginalDuration);
            bEliteBlindImmune = (int32)dwModifiedDuration < pEliteMods->nStunDM;
        }

        dwModifiedDuration = (uint32)(flCCReductionPercent * dwModifiedDuration);

        if (tACD.AttributesGetInt(ATTRIBUTE_BLIND_IMMUNE) || bEliteBlindImmune)
        {
            PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuffStartData.idPower, tBuffStartData.idACDSource, tACD.id, tBuffStartData.snoPower, CROWD_CONTROL_BLIND, dwModifiedDuration, tBuffStartData.flProcScalar, FALSE);
            if (tBuffStartData.bFailsIfBlindImmune)
            {
                if (!bEliteBlindImmune)
                {
                    sSendImmuneMessage(tACD.id, tBuffStartData.idPower);
                }
                if (pdefGBEliteMods)
                {
                    GBUnlockR(pdefGBEliteMods);
                }
                return BUFFID_INVALID;
            }
        }
        else
        {
            bUpdateCCResistance = TRUE;
            dwNewDuration = Math::Min(dwModifiedDuration, dwNewDuration);
        }
    }

    if (tBuffStartData.bCharm || tBuffStartData.bTaunt)
    {
        uint32 dwModifiedDuration = dwOriginalDuration;
        if (pEliteMods)
        {
            float flMaxCCReduction = Math::Min(pEliteMods->flCharmDR, flCurrentCCResistance);

            dwModifiedDuration = Math::FloatToInt((1.0f - flMaxCCReduction) * dwOriginalDuration);
            bEliteCharmImmune = (int32)dwModifiedDuration < pEliteMods->nCharmDM;
        }

        dwModifiedDuration = (uint32)(flCCReductionPercent * dwModifiedDuration);

        if (tACD.AttributesGetInt(ATTRIBUTE_CHARM_IMMUNE) || bEliteCharmImmune)
        {
            PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuffStartData.idPower, tBuffStartData.idACDSource, tACD.id, tBuffStartData.snoPower, CROWD_CONTROL_CHARM, dwModifiedDuration, tBuffStartData.flProcScalar, FALSE);
            if (tBuffStartData.bFailsIfCharmImmune)
            {
                if (!bEliteCharmImmune)
                {
                    sSendImmuneMessage(tACD.id, tBuffStartData.idPower);
                }
                if (pdefGBEliteMods)
                {
                    GBUnlockR(pdefGBEliteMods);
                }
                return BUFFID_INVALID;
            }
        }
        else
        {
            bUpdateCCResistance = TRUE;
            dwNewDuration = Math::Min(dwModifiedDuration, dwNewDuration);
        }
    }

    if (tBuffStartData.bSlow)
    {
        uint32 dwModifiedDuration = dwOriginalDuration;
        if (pEliteMods)
        {
            dwModifiedDuration = Math::FloatToInt(dwOriginalDuration * (1.0f - pEliteMods->flMovementSlowDR));
            bEliteSlowImmune = (int32)dwModifiedDuration < pEliteMods->nMovementSlowDM;

            flSlowAmount *= (1.0f - pEliteMods->flMovementSlowER);
        }

        flSlowAmount = flCCReductionPercent * flSlowAmount;

        if (tACD.AttributesGetInt(ATTRIBUTE_SLOWDOWN_IMMUNE) || bEliteSlowImmune)
        {
            PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuffStartData.idPower, tBuffStartData.idACDSource, tACD.id, tBuffStartData.snoPower, CROWD_CONTROL_SLOW, dwModifiedDuration, tBuffStartData.flProcScalar, FALSE);
            if (tBuffStartData.bFailsIfSlowImmune)
            {
                if (!bEliteSlowImmune)
                {
                    sSendImmuneMessage(tACD.id, tBuffStartData.idPower);
                }
                if (pdefGBEliteMods)
                {
                    GBUnlockR(pdefGBEliteMods);
                }
                return BUFFID_INVALID;
            }
        }
        else
        {
            dwNewDuration = Math::Min(dwModifiedDuration, dwNewDuration);
        }
    }

    if (tBuffStartData.bChill)
    {
        uint32 dwModifiedDuration = dwOriginalDuration;
        if (pEliteMods)
        {
            dwModifiedDuration = Math::FloatToInt(dwOriginalDuration * (1.0f - pEliteMods->flMovementSlowDR));
            bEliteChillImmune = (int32)dwModifiedDuration < pEliteMods->nMovementSlowDM;

            flChillMagnitude *= (1.0f - pEliteMods->flMovementSlowER);
        }

        flChillMagnitude = flCCReductionPercent * flChillMagnitude;

        if (tACD.AttributesGetInt(ATTRIBUTE_SLOWDOWN_IMMUNE) || bEliteChillImmune)
        {
            PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuffStartData.idPower, tBuffStartData.idACDSource, tACD.id, tBuffStartData.snoPower, CROWD_CONTROL_CHILL, dwModifiedDuration, tBuffStartData.flProcScalar, FALSE);
            if (tBuffStartData.bFailsIfChillImmune)
            {
                if (!bEliteChillImmune)
                {
                    sSendImmuneMessage(tACD.id, tBuffStartData.idPower);
                }
                if (pdefGBEliteMods)
                {
                    GBUnlockR(pdefGBEliteMods);
                }
                return BUFFID_INVALID;
            }
        }
        else
        {
            dwNewDuration = Math::Min(dwModifiedDuration, dwNewDuration);
        }
    }

    if (tBuffStartData.bAttackSpeedReduction)
    {
        uint32 dwModifiedDuration = dwOriginalDuration;
        if (pEliteMods)
        {
            dwModifiedDuration = Math::FloatToInt(dwOriginalDuration * (1.0f - pEliteMods->flAttackSpeedDR));
            bEliteAttackSpeedReductionImmune = (int32)dwModifiedDuration < pEliteMods->nAttackSpeedDM;

            flAttackSpeedReductionAmount *= (1.0f - pEliteMods->flAttackSpeedER);
        }

        flAttackSpeedReductionAmount = flCCReductionPercent * flAttackSpeedReductionAmount;

        if (tACD.AttributesGetInt(ATTRIBUTE_ATTACKSPEED_REDUCTION_IMMUNE) || bEliteAttackSpeedReductionImmune)
        {
            PowerAPI_BuffApplyOnHitWithCrowdControlEffects(tBuffStartData.idPower, tBuffStartData.idACDSource, tACD.id, tBuffStartData.snoPower, CROWD_CONTROL_ATTACKSPEED_SLOW, dwModifiedDuration, tBuffStartData.flProcScalar, FALSE);
            if (tBuffStartData.bFailsIfAttackSpeedReductionImmune)
            {
                if (pdefGBEliteMods)
                {
                    GBUnlockR(pdefGBEliteMods);
                }
                return BUFFID_INVALID;
            }
        }
        else
        {
            dwNewDuration = Math::Min(dwModifiedDuration, dwNewDuration);
        }
    }

    if (pdefGBEliteMods)
    {
        GBUnlockR(pdefGBEliteMods);
    }
    else
    {
        bUpdateCCResistance = FALSE;
    }

    PowerAPIBuffInstance * ptExistingBuff = NULL;

    BuffID idBuffExisting = PowerAPI_BuffFindOnACD(tBuffStartData.snoPower, tBuffStartData.nBuffIndex, tACD, ACDID_INVALID, FALSE);
    if (idBuffExisting != BUFFID_INVALID)
    {
        ptExistingBuff = sBuffGet(idBuffExisting);

        BOOL bStack = FALSE;
        switch (tBuffStartData.eStackType)
        {
        case BUFF_STACKTYPE_ALWAYS:
        case BUFF_STACKTYPE_ONE_PER_SOURCE:
        case BUFF_STACKTYPE_REFRESH:
        case BUFF_STACKTYPE_ONE_PER_SOURCE_REFRESH:
            bStack = TRUE;
            break;
        case BUFF_STACKTYPE_NEVER:
        case BUFF_STACKTYPE_LONGEST:
        case BUFF_STACKTYPE_EXTEND:
            bStack = FALSE;
            break;
        case BUFF_STACKTYPE_EXISTING:
            return BUFFID_INVALID;
        default:
            DisplayInternalError(NULL, "", 9176, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }

        if (tBuffStartData.eStackType == BUFF_STACKTYPE_ONE_PER_SOURCE_REFRESH || tBuffStartData.eStackType == BUFF_STACKTYPE_REFRESH)
        {

            int32 nBuffCount = 0;
            POS pos = tACD.listBuffs.GetHeadPosition();
            while (pos)
            {
                BuffID idBuff = tACD.listBuffs.GetNext(pos);

                PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
                if (!ptBuff->bDeleted &&

                    ptBuff->snoPower == tBuffStartData.snoPower &&
                    ptBuff->nBuffIndex == tBuffStartData.nBuffIndex &&
                    (tBuffStartData.eStackType != BUFF_STACKTYPE_ONE_PER_SOURCE_REFRESH ||
                     (tBuffStartData.idACDSource != ACDID_INVALID &&
                      ptBuff->idACDSource == tBuffStartData.idACDSource)))
                {
                    PowerAPI_BuffRefreshDuration(idBuff);
                    nBuffCount++;
                }
            }

            if (nBuffCount >= tBuffStartData.nMaxStackCount)
            {
                sTriggerCCCallbacksOnRefresh(*ptExistingBuff);
                if (bUpdateCCResistance)
                {
                    sUpdateCCResistance(tACD, dwNewDuration, flCurrentCCResistance);
                }
                return ptExistingBuff->id;
            }
        }

        if (bStack && (tBuffStartData.eStackType == BUFF_STACKTYPE_ONE_PER_SOURCE_REFRESH || tBuffStartData.eStackType == BUFF_STACKTYPE_ONE_PER_SOURCE))
        {
            int32 nStacksFromSource = PowerAPI_BuffCountOnACD(tBuffStartData.snoPower, tBuffStartData.nBuffIndex, tACD, tBuffStartData.idACDSource);
            if (nStacksFromSource >= tBuffStartData.nMaxStackCount)
            {
                BuffID idScopedBuffExisting = PowerAPI_BuffFindOnACD(tBuffStartData.snoPower, tBuffStartData.nBuffIndex, tACD, tBuffStartData.idACDSource, FALSE);
                if (idScopedBuffExisting != BUFFID_INVALID)
                {
                    ptExistingBuff = sBuffGet(idScopedBuffExisting);

                    if (tBuffStartData.nBuffLevel < ptExistingBuff->nBuffLevel)
                    {
                        return BUFFID_INVALID;
                    }

                    PowerAPI_BuffFlagForDeletion(idScopedBuffExisting, FALSE, FALSE, BUFF_REMOVAL_REASON_REPLACED, TRUE);
                }
            }
        }
        else if (bStack && PowerAPI_BuffCountOnACD(tBuffStartData.snoPower, tBuffStartData.nBuffIndex, tACD, ACDID_INVALID) < tBuffStartData.nMaxStackCount)
        {
        }
        else
        {
            switch (tBuffStartData.eStackType)
            {
            case BUFF_STACKTYPE_LONGEST:
                if (GameGetServerTime() + dwNewDuration < ptExistingBuff->dwStartTick + ptExistingBuff->dwDuration)
                {
                    return BUFFID_INVALID;
                }
                break;

            case BUFF_STACKTYPE_EXTEND:
                if (ptExistingBuff->idACDSource == tBuffStartData.idACDSource)
                {
                    if (!tBuffStartData.bInfinite)
                    {
                        PowerAPI_BuffModifyDuration(ptExistingBuff->id, dwNewDuration, FALSE, TRUE);
                    }
                    sTriggerCCCallbacksOnRefresh(*ptExistingBuff);
                    if (bUpdateCCResistance)
                    {
                        sUpdateCCResistance(tACD, dwNewDuration, flCurrentCCResistance);
                    }
                    return ptExistingBuff->id;
                }
                else
                {
                    if (GameGetServerTime() + dwNewDuration < ptExistingBuff->dwStartTick + ptExistingBuff->dwDuration)
                    {
                        return BUFFID_INVALID;
                    }
                }
                break;

            default:
                if (tBuffStartData.nBuffLevel < ptExistingBuff->nBuffLevel)
                {
                    return BUFFID_INVALID;
                }
                break;
            }

            PowerAPI_BuffFlagForDeletion(idBuffExisting, FALSE, FALSE, BUFF_REMOVAL_REASON_REPLACED, TRUE);
        }
    }

    PowerAPI_BuffRemoveExclusiveType(tACD, tBuffStartData.eExclusiveType);

    if (bUpdateCCResistance)
    {
        sUpdateCCResistance(tACD, dwNewDuration, flCurrentCCResistance);
    }

    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    BuffID idBuff = ptPowerGlobals->pArrayBuffs->ElementNew();

    if (idBuff == BUFFID_INVALID)
    {
        TDataArrayIterator<PowerAPIBuffInstance> tIt;
        while (ptPowerGlobals->pArrayBuffs->Iterate(tIt))
        {
        }

        DisplayInternalError(NULL, "", 9325, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    if (tBuffStartData.nMaxInstanceCount > 0)
    {
        ActorPowersApplyBuffInstanceCap((ActorID)ACDGetOwner(tBuffStartData.idACDSource), tBuffStartData.snoPower, tBuffStartData.nBuffIndex, tBuffStartData.nMaxInstanceCount - 1);
    }

    PowerAPIBuffInstance * ptBuff = ptPowerGlobals->pArrayBuffs->ElementGet(idBuff);

    uint32 dwNow = GameGetServerTime();

    ptBuff->bDeleted = FALSE;
    ptBuff->bDoNotCopy = tBuffStartData.bDoNotCopy;
    ptBuff->bUndispellable = tBuffStartData.bUndispellable;
    ptBuff->bIsCopy = tBuffStartData.bIsCopy;
    ptBuff->bPositiveEffect = tBuffStartData.bPositiveEffect;
    ptBuff->bRemoveOnDeath = tBuffStartData.bRemoveOnDeath;
    ptBuff->bRemoveOnRespec = tBuffStartData.bRemoveOnRespec;
    ptBuff->bRemoveOnPassiveApplication = tBuffStartData.bRemoveOnPassiveApplication;
    ptBuff->bRemoveOnGreaterRiftEntry = tBuffStartData.bRemoveOnGreaterRiftEntry;
    ptBuff->bKeepForProfileSnapshot = tBuffStartData.bKeepForProfileSnapshot;
    ptBuff->bTriggersHitProcs = tBuffStartData.bTriggersHitProcs;
    ptBuff->bHitsOnFirstTick = tBuffStartData.bHitsOnFirstTick;
    ptBuff->bTriggerOnHitForPets = tBuffStartData.bTriggerOnHitForPets;
    ptBuff->bTriggerOnDamageForPets = tBuffStartData.bTriggerOnDamageForPets;
    ptBuff->bProcessDamageShieldOnStaggerDamage = tBuffStartData.bProcessDamageShieldOnStaggerDamage;
    ptBuff->eStackType = tBuffStartData.eStackType;
    ptBuff->dwDuration = dwNewDuration;
    ptBuff->dwRefreshRate = tBuffStartData.dwRefreshRate;
    ptBuff->dwStartTick = dwNow;
    ptBuff->dwInternalCooldownDuration = tBuffStartData.dwInternalCooldownDuration;
    ptBuff->dwInternalCooldownReadyTick = 0;
    ptBuff->uLastDOTTick = dwNow;
    ptBuff->bInfinite = tBuffStartData.bInfinite;
    ptBuff->snoPower = tBuffStartData.snoPower;
    ptBuff->idPower = tBuffStartData.idPower;
    ptBuff->nBuffLevel = tBuffStartData.nBuffLevel;
    ptBuff->nMaxStackCount = tBuffStartData.nMaxStackCount;
    ptBuff->pPeriodicSpellEffect = tBuffStartData.pPeriodicSpellEffect;
    ptBuff->pPeriodicSpellEffectData = tBuffStartData.pPeriodicSpellEffectData;
    ptBuff->pOnDoDamageEffect = tBuffStartData.pOnDoDamageEffect;
    ptBuff->pOnDoDamageEffectData = tBuffStartData.pOnDoDamageEffectData;
    ptBuff->pOnGetHitEffect = tBuffStartData.pOnGetHitEffect;
    ptBuff->pOnGetHitEffectData = tBuffStartData.pOnGetHitEffectData;
    ptBuff->pOnHitEffect = tBuffStartData.pOnHitEffect;
    ptBuff->pOnHitEffectData = tBuffStartData.pOnHitEffectData;
    ptBuff->pOnFirstHitAttemptEffect = tBuffStartData.pOnFirstHitAttemptEffect;
    ptBuff->pOnFirstHitAttemptEffectData = tBuffStartData.pOnFirstHitAttemptEffectData;
    ptBuff->pOnAttackEffect = tBuffStartData.pOnAttackEffect;
    ptBuff->pOnAttackEffectData = tBuffStartData.pOnAttackEffectData;
    ptBuff->pOnDodgeEffect = tBuffStartData.pOnDodgeEffect;
    ptBuff->pOnDodgeEffectData = tBuffStartData.pOnDodgeEffectData;
    ptBuff->pOnCostEffect = tBuffStartData.pOnCostEffect;
    ptBuff->pOnCostEffectData = tBuffStartData.pOnCostEffectData;
    ptBuff->pOnHealFromSkillEffect = tBuffStartData.pOnHealFromSkillEffect;
    ptBuff->pOnKillEffect = tBuffStartData.pOnKillEffect;
    ptBuff->pOnKillEffectData = tBuffStartData.pOnKillEffectData;
    ptBuff->pOnAssistEffect = tBuffStartData.pOnAssistEffect;
    ptBuff->pOnAssistEffectData = tBuffStartData.pOnAssistEffectData;
    ptBuff->pOnProjectileCreateEffect = tBuffStartData.pOnProjectileCreateEffect;
    ptBuff->pOnProjectileCreateEffectData = tBuffStartData.pOnProjectileCreateEffectData;
    ptBuff->pOnKnockbackLandEffect = tBuffStartData.pOnKnockbackLandEffect;
    ptBuff->pOnPetAddEffect = tBuffStartData.pOnPetAddEffect;
    ptBuff->pOnMessageEffect = tBuffStartData.pOnMessageEffect;
    ptBuff->pOnCalloutEventEffect = tBuffStartData.pOnCalloutEventEffect;
    ptBuff->pOnGizmoOperatedEffect = tBuffStartData.pOnGizmoOperatedEffect;
    ptBuff->pOnWreckableDestroyedEffect = tBuffStartData.pOnWreckableDestroyedEffect;
    ptBuff->pOnTeleportedEffect = tBuffStartData.pOnTeleportedEffect;
    ptBuff->pOnPowerDamageInitEffect = tBuffStartData.pOnPowerDamageInitEffect;
    ptBuff->pOnLootDropEffect = tBuffStartData.pOnLootDropEffect;
    ptBuff->pOnItemSoldEffect = tBuffStartData.pOnItemSoldEffect;
    ptBuff->pOnLevelEffect = tBuffStartData.pOnLevelEffect;
    ptBuff->pOnCorpseConsumedEffect = tBuffStartData.pOnCorpseConsumedEffect;
    ptBuff->pOnPetDismissEffect = tBuffStartData.pOnPetDismissEffect;
    ptBuff->pOnPetAddEffectData = tBuffStartData.pOnPetAddEffectData;
    ptBuff->pOnGetHitWithCrowdControlEffect = tBuffStartData.pOnGetHitWithCrowdControlEffect;
    ptBuff->pOnGetHitwithCrowdControlData = tBuffStartData.pOngetHitwithCrowdControlData;
    ptBuff->bOnGetHitWithCrowdControlCallOnRefresh = tBuffStartData.bOnGetHitWithCrowdControlCallOnRefresh;
    ptBuff->pOnHitWithCrowdControlEffect = tBuffStartData.pOnHitWithCrowdControlEffect;
    ptBuff->bOnHitWithCrowdControlCallOnRefresh = tBuffStartData.bOnHitWithCrowdControlCallOnRefresh;
    ptBuff->pOnEquipmentChangeEffect = tBuffStartData.pOnEquipmentChangeEffect;
    ptBuff->pOnSkillChangeEffect = tBuffStartData.pOnSkillChangeEffect;
    ptBuff->pOnResourceGainEffect = tBuffStartData.pOnResourceGainEffect;
    ptBuff->pOnItemPickUpEffect = tBuffStartData.pOnItemPickUpEffect;
    ptBuff->pOnItemPickUpEffectData = tBuffStartData.pOnItemPickUpEffectData;
    ptBuff->pDamageShieldEffect = tBuffStartData.pDamageShieldEffect;
    ptBuff->pDamageShieldEffectData = tBuffStartData.pDamageShieldEffectData;
    ptBuff->bDamageShieldIsEvaluatedLast = tBuffStartData.bDamageShieldIsEvaluatedLast;
    ptBuff->nDamageShieldPriority = tBuffStartData.nDamageShieldPriority;
    ptBuff->pExpirationEffect = tBuffStartData.pExpirationEffect;
    ptBuff->pExpirationEffectData = tBuffStartData.pExpirationEffectData;
    ptBuff->pRemovalEffect = tBuffStartData.pRemovalEffect;
    ptBuff->pRemovalEffectData = tBuffStartData.pRemovalEffectData;
    ptBuff->pDeathEffect = tBuffStartData.pDeathEffect;
    ptBuff->pDeathEffectData = tBuffStartData.pDeathEffectData;
    ptBuff->pReflectEffect = tBuffStartData.pReflectEffect;
    ptBuff->pShouldReflectTest = tBuffStartData.pShouldReflectTest;
    ptBuff->pReflectEffectData = tBuffStartData.pReflectEffectData;
    ptBuff->tKeyAppearance = MakeAttribKey(tBuffStartData.eAppearanceAttribute, tBuffStartData.nAppearanceParameter);
    ptBuff->snoEndingEffect = tBuffStartData.snoEndingEffect;
    ptBuff->nBuffIndex = tBuffStartData.nBuffIndex;
    ptBuff->nBuffData = tBuffStartData.nBuffData;
    ptBuff->flBuffData = tBuffStartData.flBuffData;
    ptBuff->flShieldAmount = tBuffStartData.flShieldAmount;
    ptBuff->bIsAura = tBuffStartData.bAura;
    ptBuff->nAuraPriority = tBuffStartData.nAuraPriority;
    ptBuff->bIsChanneled = tBuffStartData.bChanneled;
    ptBuff->bUsesLockout = tBuffStartData.bUsesLockout;
    ptBuff->eExclusiveType = tBuffStartData.eExclusiveType;
    ptBuff->dwInitialInstanceStartTick = ptExistingBuff ? ptExistingBuff->dwInitialInstanceStartTick : dwNow;
    ptBuff->nLookOverrideHash = tBuffStartData.nLookOverrideHash;
    ptBuff->bSaveCooldown = tBuffStartData.bSaveCooldown;
    ptBuff->bAppliedDamage = FALSE;

    if (ptPower && tBuffStartData.bUsePowersProcScalar)
    {
        ptBuff->flProcScalar = PowerGetProcScalar(*ptPower);
    }
    else
    {
        ptBuff->flProcScalar = tBuffStartData.flProcScalar;
    }

    ptBuff->idTimerDone = TIMERID_INVALID;
    ptBuff->idTimerPeriodicEffect = TIMERID_INVALID;

    ptBuff->listAIStatesToMerge.SetPool(MemoryPoolsGetDATAIDListPool());
    sCopyAIStateList(ptBuff->listAIStatesToMerge, tBuffStartData.listAIStatesToMerge);

    ptBuff->idACDAffected = tACD.id;
    ptBuff->idACDSource = tBuffStartData.idACDSource;
    if (ptBuff->idACDSource == ACDID_INVALID && ptBuff->idPower != POWERID_INVALID)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(ptBuff->idPower);
        if (ptACDOwner)
        {
            ptBuff->idACDSource = ptACDOwner->id;
        }
        else
        {
            ptBuff->idACDSource = ACDID_INVALID;
        }
    }

    ptBuff->idTimerPeriodicEffect = TIMERID_INVALID;

    if (ptBuff->pPeriodicSpellEffect && tBuffStartData.idPower != POWERID_INVALID)
    {
        if (ptBuff->bHitsOnFirstTick)
        {
            ptBuff->idTimerPeriodicEffect = PowerTriggerAddTimerTrigger(tBuffStartData.idPower, 1, sBuffPulseSpellEffect, (void *)(int64)idBuff);
        }
        else
        {
            ptBuff->idTimerPeriodicEffect = PowerTriggerAddTimerTrigger(tBuffStartData.idPower, ptBuff->dwRefreshRate, sBuffPulseSpellEffect, (void *)(int64)idBuff);
        }
    }

    BOOL bSourceDoesNoDamage = FALSE;
    ActorCommonData * ptACDSource = ACDTryToGet(ptBuff->idACDSource);
    if (ptACDSource)
    {
        bSourceDoesNoDamage = ptACDSource->AttributesGetInt(ATTRIBUTE_DOES_NO_DAMAGE);
    }

    BOOL bPowerDoesNoDamage = FALSE;
    if (ptPower)
    {
        bPowerDoesNoDamage = FastAttribGetValueInt(ptPower->idFastAttrib, ATTRIBUTE_DOES_NO_DAMAGE);
    }

    if (!bSourceDoesNoDamage && !bPowerDoesNoDamage)
    {
        ptBuff->eDamageType = tBuffStartData.eDamageType;
        ptBuff->snoDOTDamageSourcePower = tBuffStartData.snoDOTDamageSourcePower;
        ptBuff->bDOTCrit = tBuffStartData.bDOTCrit;
    }

    BOOL bIsDOT = Math::Abs(tBuffStartData.flDamagePerSecond) > EPSILON;
    if (bIsDOT && !bSourceDoesNoDamage && !bPowerDoesNoDamage)
    {
        if (ACDIsPlayer(tBuffStartData.idACDSource) &&
            tACD.IsMonster())
        {
            SPlayerRefreshKillCounterTime(ACDTryToGet(tBuffStartData.idACDSource));
        }

        float flDamagePerSecond = tBuffStartData.flDamagePerSecond;

        if (!tBuffStartData.bAppliedDamageReductionEffects)
        {
            BOOL bTargetIsImmune = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_IMMUNITY, tBuffStartData.eDamageType));
            if (bTargetIsImmune)
            {
                flDamagePerSecond = 0.0f;
            }

            float flDamageReductionTurnsIntoHealTotal = 0.0f;

            if (ptPower)
            {
                float flDamageDoneReductionPercent = FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_DAMAGE_DONE_REDUCTION_PERCENT);
                if (flDamageDoneReductionPercent > EPSILON)
                {
                    flDamagePerSecond = (1.0f - flDamageDoneReductionPercent) * flDamagePerSecond;
                }
            }

            sApplyResistance(tBuffStartData.idPower, tACD, tBuffStartData.eDamageType, flDamagePerSecond);

            sApplyMiscDamageReduction(tBuffStartData.idPower, tACD, PDM_DOT, flDamagePerSecond, flDamageReductionTurnsIntoHealTotal, tBuffStartData.eDamageType);

            sApplyArmor(tBuffStartData.idPower, tACD, flDamagePerSecond);

            ActorCommonData * ptACDSourceScoped = ACDTryToGet(tBuffStartData.idACDSource);

            float flBonusCritChance = 0.0f;
            if (ptPower)
            {
                flBonusCritChance = FastAttribGetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_CRIT_PERCENT_BONUS, ptPower->snoPower));
            }

            BOOL fCrit = sCheckForCrit(tBuffStartData.idPower, tBuffStartData.bDOTCanCrit, 0.0f, 1000.0f, tACD, ptACDSourceScoped);
            if (fCrit && PowerTryToGet(tBuffStartData.idPower))
            {
                sApplyCrit(tBuffStartData.idPower, ptACDSourceScoped, flDamagePerSecond, 0.0f, 1000.0f, tACD);
                ptBuff->bDOTCrit = TRUE;
            }

            PowerHelper_ApplyDamageBonusMultipliers(tACD,
                ptBuff->snoPower,
                ptBuff->eDamageType,
                tBuffStartData.flPrimaryAttributeBonusMultiplier,
                tBuffStartData.flClassBonusMultiplier,
                tBuffStartData.flSkillsBonusMultiplier,
                flDamagePerSecond);

            sApplyMiscDamageBonuses(tBuffStartData.idPower, tACD, flDamagePerSecond, tBuffStartData.eDamageType, PDM_DOT, tBuffStartData.bUseHighestElementalDamageBonus);

            sApplyMiscDamageAbsorb(tACD, flDamagePerSecond, tBuffStartData.eDamageType);

            if (tACD.IsPet())
            {
                flDamagePerSecond = tBuffStartData.flPetDamageScalar * flDamagePerSecond;
            }
        }

        ptBuff->flDamagePerSecond = flDamagePerSecond;

        if (tBuffStartData.idFastAttrib != ATTRIBGROUPID_INVALID)
        {
            FastAttribGroup * ptBuffStartDataFastAttrib = FastAttribGroupGet(tBuffStartData.idFastAttrib);
            FastAttribSetValueFloat(ptBuffStartDataFastAttrib, ATTRIBUTE_DOT_DPS, ptBuff->flDamagePerSecond);


            if (!tBuffStartData.bHideDOTDamageEffects)
            {
                switch (ptBuff->eDamageType)
                {
                case 0:
                    FastAttribSetValueInt(ptBuffStartDataFastAttrib, ATTRIBUTE_BLEEDING, TRUE);
                    break;

                case 1:
                    FastAttribSetValueInt(ptBuffStartDataFastAttrib, MakeAttribKey(ATTRIBUTE_BUFF_VISUAL_EFFECT, 0x19), TRUE);
                    break;

                case 2:
                    FastAttribSetValueInt(ptBuffStartDataFastAttrib, MakeAttribKey(ATTRIBUTE_BUFF_VISUAL_EFFECT, 0x4d), TRUE);
                    break;

                case 4:
                    FastAttribSetValueInt(ptBuffStartDataFastAttrib, MakeAttribKey(ATTRIBUTE_BUFF_VISUAL_EFFECT, 0x23), TRUE);
                    break;

                case 6:
                    FastAttribSetValueInt(ptBuffStartDataFastAttrib, MakeAttribKey(ATTRIBUTE_BUFF_VISUAL_EFFECT, 0x24), TRUE);
                    break;

                case 5:
                    FastAttribSetValueInt(ptBuffStartDataFastAttrib, MakeAttribKey(ATTRIBUTE_BUFF_VISUAL_EFFECT, 0x4c), TRUE);
                    break;
                }
            }
        }
    }
    else
    {
        if (tBuffStartData.idFastAttrib != ATTRIBGROUPID_INVALID)
        {
            FastAttribGroup * ptBuffStartDataFastAttrib = FastAttribGroupGet(tBuffStartData.idFastAttrib);

            if (FastAttribExists(ptBuffStartDataFastAttrib, ATTRIBUTE_DOT_DPS))
            {
                FastAttribSetValueToDefault(ptBuffStartDataFastAttrib, ATTRIBUTE_DOT_DPS);
                FastAttribSetValueToDefault(ptBuffStartDataFastAttrib, ATTRIBUTE_BLEEDING);
                FastAttribSetValueToDefault(ptBuffStartDataFastAttrib, MakeAttribKey(ATTRIBUTE_BUFF_VISUAL_EFFECT, 0x19));
                FastAttribSetValueToDefault(ptBuffStartDataFastAttrib, MakeAttribKey(ATTRIBUTE_BUFF_VISUAL_EFFECT, 0x23));
                FastAttribSetValueToDefault(ptBuffStartDataFastAttrib, MakeAttribKey(ATTRIBUTE_BUFF_VISUAL_EFFECT, 0x24));
                FastAttribSetValueToDefault(ptBuffStartDataFastAttrib, MakeAttribKey(ATTRIBUTE_BUFF_VISUAL_EFFECT, 0x4c));
                FastAttribSetValueToDefault(ptBuffStartDataFastAttrib, MakeAttribKey(ATTRIBUTE_BUFF_VISUAL_EFFECT, 0x4d));
            }
        }
    }

    Attrib eAttribStart = ATTRIBUTE_NONE;
    Attrib eAttribEnd = ATTRIBUTE_NONE;
    Attrib eAttribCount = ATTRIBUTE_NONE;
    BOOL bValid = PowerGetBuffAttributes(ptBuff->nBuffIndex, eAttribStart, eAttribEnd, eAttribCount);

    if (!ptBuff->bInfinite)
    {
        uint32 dwEndTick = ptBuff->dwDuration + dwNow;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib,
            MakeAttribKey(eAttribStart, ptBuff->snoPower), dwNow);

        FastAttribSetValueInt(tBuffStartData.idFastAttrib,
            MakeAttribKey(eAttribEnd, ptBuff->snoPower), dwEndTick);
    }

    if (FastAttribGetValueInt(tBuffStartData.idFastAttrib,
        MakeAttribKey(eAttribCount, ptBuff->snoPower)) == 0)
    {
        FastAttribSetValueInt(tBuffStartData.idFastAttrib,
            MakeAttribKey(eAttribCount, ptBuff->snoPower), 1);
    }

    BOOL bDidCC = FALSE;

    ptBuff->bAppliesOnlyToSpecificPowers = sBuffIsAppliedOnlyToSpecificPowers(*ptBuff);
    if (tBuffStartData.idFastAttrib != ATTRIBGROUPID_INVALID || tBuffStartData.eAppearanceAttribute != ATTRIBUTE_NONE)
    {
        ptBuff->idFastAttrib = FastAttribLightCreate(0);
        FastAttribGroup * ptBuffFastAttrib = FastAttribGroupGet(ptBuff->idFastAttrib);

        if (tBuffStartData.idFastAttrib != ATTRIBGROUPID_INVALID)
        {
            FastAttribLightCopy(tBuffStartData.idFastAttrib, ptBuff->idFastAttrib);
        }

        if (tBuffStartData.eAppearanceAttribute != ATTRIBUTE_NONE)
        {
            FastAttribSetValueInt(ptBuffFastAttrib, ptBuff->tKeyAppearance, 1);
        }

        if (tBuffStartData.pDamageShieldEffect)
        {
            FastAttribSetValueInt(ptBuffFastAttrib, ATTRIBUTE_DAMAGE_SHIELD, 1);
        }

        BOOL bDidStun = FALSE;
        BOOL bDidFreeze = FALSE;
        BOOL bDidFear = FALSE;
        BOOL bDidBlind = FALSE;
        BOOL bDidImmobilize = FALSE;
        BOOL bDidTaunt = FALSE;
        BOOL bDidForceGrip = FALSE;
        BOOL bDidCharm = FALSE;

        if (!ActorIsUsingAIQueue((ActorID)ACDGetOwner(ptBuff->idACDAffected)))
        {
            if (tBuffStartData.bStun && !tACD.AttributesGetInt(ATTRIBUTE_STUN_IMMUNE) && !bEliteStunImmune)
            {
                FastAttribSetValueInt(ptBuffFastAttrib, ATTRIBUTE_STUNNED, TRUE);
                bDidStun = TRUE;
                sQueueCCFloatMessage(ptBuff->idACDSource, tACD.id, FLOATING_NUMBER_TYPE_STUN);
                PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_STUN, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
                PowerAPI_BuffApplyOnHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_STUN, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
            }
            if (tBuffStartData.bForceGrip && !tACD.AttributesGetInt(ATTRIBUTE_STUN_IMMUNE) && !bEliteForceGripImmune && PowerActorCanBeKnockedBack(idActor, tBuffStartData.idPower))
            {
                FastAttribSetValueInt(ptBuffFastAttrib, ATTRIBUTE_FORCE_GRIPPED, TRUE);
                bDidForceGrip = TRUE;
                sQueueCCFloatMessage(ptBuff->idACDSource, tACD.id, FLOATING_NUMBER_TYPE_STUN);
                PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_FORCE_GRIP, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
                PowerAPI_BuffApplyOnHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_FORCE_GRIP, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
            }
            if (tBuffStartData.bFreeze && !tACD.AttributesGetInt(ATTRIBUTE_FREEZE_IMMUNE) && !bEliteFreezeImmune)
            {
                FastAttribSetValueInt(ptBuffFastAttrib, ATTRIBUTE_FROZEN, TRUE);
                bDidFreeze = TRUE;

                ActorCommonData * ptACDSourceScoped = ACDTryToGet(ptBuff->idACDSource);
                if (ptACDSourceScoped)
                {
                    float flFreezeDamagePercentBonus = ptACDSourceScoped->AttributesGetFloat(ATTRIBUTE_FREEZE_DAMAGE_PERCENT_BONUS);
                    if (flFreezeDamagePercentBonus > EPSILON)
                    {
                        FastAttribSetValueFloat(ptBuffFastAttrib, ATTRIBUTE_AMPLIFYDAMAGE_PERCENT, flFreezeDamagePercentBonus);
                    }
                }

                sQueueCCFloatMessage(ptBuff->idACDSource, tACD.id, FLOATING_NUMBER_TYPE_FROZEN);
                PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_FREEZE, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
                PowerAPI_BuffApplyOnHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_FREEZE, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
            }
            if (tBuffStartData.bBlind && !tACD.AttributesGetInt(ATTRIBUTE_BLIND_IMMUNE) && !bEliteBlindImmune)
            {
                FastAttribSetValueInt(ptBuffFastAttrib, ATTRIBUTE_BLIND, TRUE);
                bDidBlind = TRUE;
                sQueueCCFloatMessage(ptBuff->idACDSource, tACD.id, FLOATING_NUMBER_TYPE_BLIND);
                PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_BLIND, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
                PowerAPI_BuffApplyOnHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_BLIND, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
            }
            if (tBuffStartData.bFear && !tACD.AttributesGetInt(ATTRIBUTE_FEAR_IMMUNE) && !bEliteFearImmune)
            {
                FastAttribSetValueInt(ptBuffFastAttrib, ATTRIBUTE_FEARED, TRUE);
                bDidFear = TRUE;
                sQueueCCFloatMessage(ptBuff->idACDSource, tACD.id, FLOATING_NUMBER_TYPE_FEAR);
                PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_FEAR, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
                PowerAPI_BuffApplyOnHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_FEAR, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
            }
            if (tBuffStartData.bImmobilize && !tACD.AttributesGetInt(ATTRIBUTE_ROOT_IMMUNE) && !bEliteImmobilizeImmune)
            {
                bDidImmobilize = TRUE;
                FastAttribSetValueInt(ptBuffFastAttrib, ATTRIBUTE_IMMOBOLIZED, TRUE);
                sQueueCCFloatMessage(ptBuff->idACDSource, tACD.id, FLOATING_NUMBER_TYPE_ROOT);
                PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_IMMOBILIZE, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
                PowerAPI_BuffApplyOnHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_IMMOBILIZE, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
            }
            if (tBuffStartData.bCharm && !tACD.AttributesGetInt(ATTRIBUTE_CHARM_IMMUNE) && !bEliteCharmImmune)
            {
                if (tACD.IsMonster())
                {
                    SNO snoAIStateToMerge = GlobalSNOGet(GLOBALSNO_AISTATE_GENERIC_CHARMED);
                    if (!ptBuff->listAIStatesToMerge.Find(snoAIStateToMerge))
                    {
                        ptBuff->listAIStatesToMerge.AddTail_Tracked(snoAIStateToMerge);
                    }

                    FastAttribSetValueInt(ptBuffFastAttrib, ATTRIBUTE_CHARMED, TRUE);
                    bDidCharm = TRUE;

                    sQueueCCFloatMessage(ptBuff->idACDSource, tACD.id, FLOATING_NUMBER_TYPE_CHARM);
                }

                PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_CHARM, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
                PowerAPI_BuffApplyOnHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_CHARM, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
            }

            if (tBuffStartData.bTaunt && !tACD.AttributesGetInt(ATTRIBUTE_CHARM_IMMUNE) && !bEliteCharmImmune)
            {
                SNO snoAIStateToMerge = SNO_NONE;
                if (tACD.IsMonster())
                {
                    snoAIStateToMerge = GlobalSNOGet(GLOBALSNO_AISTATE_GENERIC_TAUNTED_MELEE);

                    if (snoAIStateToMerge != SNO_NONE)
                    {
                        if (!ptBuff->listAIStatesToMerge.Find(snoAIStateToMerge))
                        {
                            ptBuff->listAIStatesToMerge.AddTail_Tracked(snoAIStateToMerge);
                        }
                        FastAttribSetValueInt(ptBuffFastAttrib, ATTRIBUTE_TAUNTED, TRUE);
                        bDidTaunt = TRUE;

                        if (!tBuffStartData.bSuppressTauntEffects)
                        {
                            sQueueCCFloatMessage(ptBuff->idACDSource, tACD.id, FLOATING_NUMBER_TYPE_TAUNT);
                        }
                    }
                }
                if (!tBuffStartData.bSuppressTauntEffects)
                {
                    PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_TAUNT, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
                    PowerAPI_BuffApplyOnHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_TAUNT, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
                }
            }
            if (tBuffStartData.bSlow && !tACD.AttributesGetInt(ATTRIBUTE_SLOWDOWN_IMMUNE) && !bEliteSlowImmune)
            {
                FastAttribSetValueFloat(ptBuffFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT, flSlowAmount);
                FastAttribSetValueInt(ptBuffFastAttrib, ATTRIBUTE_SLOW, TRUE);

                if (!bAlreadySlowed && !bAlreadyChilled)
                {
                    sQueueCCFloatMessage(ptBuff->idACDSource, tACD.id, FLOATING_NUMBER_TYPE_SNARE);
                }

                PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_SLOW, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
                PowerAPI_BuffApplyOnHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_SLOW, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
            }
            if (tBuffStartData.bChill && !tACD.AttributesGetInt(ATTRIBUTE_SLOWDOWN_IMMUNE) && !bEliteChillImmune)
            {
                FastAttribSetValueFloat(ptBuffFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT, flChillMagnitude);
                FastAttribSetValueInt(ptBuffFastAttrib, ATTRIBUTE_CHILLED, TRUE);
                FastAttribSetValueInt(ptBuffFastAttrib, ATTRIBUTE_SLOW, TRUE);

                if (!bAlreadySlowed && !bAlreadyChilled)
                {
                    sQueueCCFloatMessage(ptBuff->idACDSource, tACD.id, FLOATING_NUMBER_TYPE_SNARE);
                }

                PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_CHILL, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
                PowerAPI_BuffApplyOnHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_CHILL, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
            }
            if (tBuffStartData.bAttackSpeedReduction && !bEliteAttackSpeedReductionImmune)
            {
                FastAttribSetValueFloat(ptBuffFastAttrib, ATTRIBUTE_ATTACKS_PER_SECOND_PERCENT_REDUCTION, flAttackSpeedReductionAmount);
                FastAttribSetValueInt(ptBuffFastAttrib, ATTRIBUTE_ATTACK_SLOW, TRUE);

                if (!bAlreadyAttackSlowed)
                {
                    sQueueCCFloatMessage(ptBuff->idACDSource, tACD.id, FLOATING_NUMBER_TYPE_ATTACK_SPEED_SLOW);
                }

                PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_ATTACKSPEED_SLOW, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
                PowerAPI_BuffApplyOnHitWithCrowdControlEffects(ptBuff->idPower, ptBuff->idACDSource, ptBuff->idACDAffected, ptBuff->snoPower, CROWD_CONTROL_ATTACKSPEED_SLOW, ptBuff->dwDuration, ptBuff->flProcScalar, FALSE);
            }

            if (bIsDOT && !bSourceDoesNoDamage && ptBuff->bTriggersHitProcs)
            {
                Power * ptPowerScoped = PowerTryToGet(ptBuff->idPower);
                ActorCommonData * ptACDFrom = ACDTryToGet(ptBuff->idACDSource);
                if (ptPowerScoped && ptACDFrom)
                {
                    sPowerAPI_CheckForOnHitProcs(*ptPowerScoped, *ptACDFrom, tACD, 0, ptBuff->flProcScalar, NULL, 0.0f, NULL, NULL, FALSE);
                }
            }
        }

        sMergeAIStatesOntoActor(tACD, *ptBuff);

        if (!ptBuff->bAppliesOnlyToSpecificPowers)
        {

            tACD.AttributesMergeAttributeGroup(ptBuff->idFastAttrib);
        }

        ActorID idActorScoped = (ActorID)tACD.idOwner;
        if (bDidStun)
        {
            BrainAPIDoStun(idActorScoped);
        }

        if (bDidForceGrip)
        {
            BrainAPIDoForceGrip(idActorScoped);
        }

        if (bDidFreeze)
        {
            BrainAPIDoFrozen(idActorScoped);
        }

        if (bDidBlind)
        {
            BrainAPIDoBlind(idActorScoped, ptBuff->dwDuration);
        }

        if (bDidFear)
        {
            BrainAPIDoFeared(idActorScoped, ptBuff->idACDSource);
        }

        if (bDidTaunt)
        {
            ACDID idACDTauntTarget = (ACDID)FastAttribGetValueInt(ptBuffFastAttrib, ATTRIBUTE_TAUNT_TARGET_ACDID);
            BrainAPIDoTaunt(idActorScoped, idACDTauntTarget, ptBuff->dwDuration, tBuffStartData.bSuppressTauntEffects);
        }

        if (bDidCharm)
        {
            BrainAPIDoCharm(idActorScoped, ptBuff->dwDuration);
        }

        if (bDidImmobilize)
        {
            ActorMovement * ptMove = tACD.GetActorMovement();
            if (ptMove)
            {
                ptMove->StopNow();
            }
        }

        bDidCC = bDidStun || bDidFreeze || bDidFear || bDidBlind || bDidImmobilize || bDidCharm;
    }

    if (ptPower && tACD.IsPet())
    {
        ScopedPowerDefinitionLockR pdefPower(ptBuff->snoPower);
        if (!PowerBuffVisuallyAppliesToPets(pdefPower, ptBuff->nBuffIndex))
        {
            RuneType eRuneType = PowerGetRuneType(ptPower);
            Attrib eAttrib = PowerRuneTypeAndBuffIndexToAttribute(ptBuff->snoPower, eRuneType, ptBuff->nBuffIndex);
            if (eAttrib != ATTRIBUTE_NONE)
            {
                FastAttribSetValueInt(ptBuff->idFastAttrib, MakeAttribKey(eAttrib, ptBuff->snoPower), 0);
            }
        }
    }

    if (!ptBuff->bInfinite)
    {
        ptBuff->idTimerDone = TimerRegister(idBuff, TIMER_OWNER_BUFF, ptBuff->dwDuration, NULL, sBuffDoneCallback);
    }

    tACD.listBuffs.AddTail_Tracked(idBuff);


    if (ptBuff->idPower != POWERID_INVALID)
    {

        Power * ptPowerScoped = PowerTryToGet(ptBuff->idPower);

        ptPowerScoped->listBuffInstances.AddTail_Tracked(idBuff);

        sEvaluateAppliedBuffAchievements(*ptBuff, tACD, bDidCC);
    }

    if (tACD.IsItem())
    {
        VisualInventoryUpdateItem(tACD);
    }

    if (ptBuff->bTriggersHitProcs)
    {
        PowerAPIDamage tDamage;
        PowerHelper_DamageInit(tDamage, ptBuff->idPower, tACD.id);
        tDamage.flProcScalar = ptBuff->flProcScalar;
        tDamage.bCriticalHit = ptBuff->bDOTCrit;

        float flHitPointDamage[7];
        SigmaMemoryClear(flHitPointDamage, sizeof(flHitPointDamage));
        int32 nDamageType = (ptBuff->eDamageType == -1) ? 0 : ptBuff->eDamageType;

        if (bIsDOT && !bSourceDoesNoDamage && !bPowerDoesNoDamage)
        {
            tDamage.eDeliveryMechanism = PDM_DOT;
            flHitPointDamage[nDamageType] = ptBuff->flDamagePerSecond * ((float)ptBuff->dwDuration * OO_TICKS_PER_SECOND);
        }

        if (ptBuff->flDamagePerSecond > EPSILON)
        {
            PowerAPI_DoFirstHitEffects(tBuffStartData.idACDSource, tACD.id, ptPower);
        }

        sApplyOnGetHitBuffs(tDamage, flHitPointDamage[nDamageType], ptBuff->idACDSource, tACD.id);
        sApplyOnHitBuffs(tDamage, flHitPointDamage[nDamageType], flHitPointDamage, ptBuff->idACDSource, tACD.id, ACDID_INVALID);
        if (ptACDSource)
        {
            sApplyThorns(tDamage.flProcScalar, flHitPointDamage, *ptACDSource, tACD, ptPower, FALSE);

            if (ptBuff->bDOTCrit)
            {
                sPowerAPI_CheckForOnCriticalProcs(tDamage, ptACDSource, tACD);
            }
        }

        AIStateOnHitReaction(tACD.id);
    }

    if (ptBuff->bIsAura)
    {
        sUpdateAuras(tACD);
    }

    if (ptBuff->nLookOverrideHash)
    {
        if (ACDIsPlayer(tACD.id))
        {

            ptBuff->bRemoveOnDeath = FALSE;
        }

        ActorLookOverrideChangedMessage tOverrideMessage;
        tOverrideMessage.ann = ANNFromACD(tACD.id);
        tOverrideMessage.nChange = 1;
        tOverrideMessage.nLookOverrideHash = ptBuff->nLookOverrideHash;

        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllConnected(listDestPlayers, -1, FALSE);

        MessageSendToClient(listDestPlayers, SMSG_ACTOR_LOOK_OVERRIDE_CHANGED, &tOverrideMessage, sizeof(tOverrideMessage));
    }

    if (FastAttribGroupIsValid(ptBuff->idFastAttrib) && FastAttribExists(ptBuff->idFastAttrib, ATTRIBUTE_ALLOW_2H_AND_SHIELD))
    {

        ACDID idACDItemRight = ACDInventoryGetSquareItem(tACD, 3, IVector2D(0, 0));  // InventorySlotType 3
        ActorCommonData * ptACDItemRight = ACDTryToGet(idACDItemRight);
        if (ptACDItemRight && ptACDItemRight->IsItem() && ItemHasLabel(ptACDItemRight->hGB.gbid, 0x1a) && ItemIsEquippedButDisabled(*ptACDItemRight))  // ItemLabel 26
        {
            ptACDItemRight->AttributesSetInt(ATTRIBUTE_ITEM_EQUIPPED_BUT_DISABLED, 0);

            AttribGroupID idFastAttribItem = SItemGetAttributeGroup(*ptACDItemRight);
            if (!ItemIsBroken(*ptACDItemRight) &&
                !ItemIsEquippedButDisabled(*ptACDItemRight) &&
                !FastAttribHasChild(tACD.idFastAttrib, idFastAttribItem))
            {
                tACD.AttributesMergeAttributeGroup(idFastAttribItem);
                ACDInventoryEquipmentOrSkillChanged(tACD);
                VisualInventoryResetAllSlots(tACD.id);
            }
        }
    }

    if (!ptBuff->bInfinite && FastAttribGroupIsValid(ptBuff->idFastAttrib))
    {
        uint32 uUpdateTicks = ActorGetAttributeUpdateTicks();
        uint32 dwTickGap = (uUpdateTicks + ptBuff->dwDuration - 1) % uUpdateTicks;
        if (ptActor->tAttrUpdateData.dwGameTime - dwNow > dwTickGap)
        {


            float flHitpointRegen = FastAttribGetValueFloat(ptBuff->idFastAttrib, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND);
            flHitpointRegen += FastAttribGetValueFloat(ptBuff->idFastAttrib, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_BONUS);
            flHitpointRegen *= (tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_REGEN_BONUS_PERCENT) + 1.0f);
            flHitpointRegen *= Math::Pin(1.0f - tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_REGEN_REDUCTION_PERCENT), 0, 1);

            flHitpointRegen += FastAttribGetValueFloat(ptBuff->idFastAttrib, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_HEALTH_GLOBE);
            PowerAPI_DoHitpointRegen(tACD, flHitpointRegen, uUpdateTicks);
        }
    }

    return idBuff;
}

// PowerAPI.cpp:10017
static void sPowerAPI_BuffDelete(PowerAPIBuffInstance * ptBuff)
{
    if (!ptBuff->bDeleted)
        PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);

    ActorCommonData * ptACD = ACDTryToGet(ptBuff->idACDAffected);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.Find(ptBuff->id);
        if (pos)
            ptACD->listBuffs.RemoveAt(pos);
    }

    if (ptBuff->idPower != POWERID_INVALID)
    {
        Power * ptPower = PowerTryToGet(ptBuff->idPower);
        if (ptPower)
        {
            POS pos = ptPower->listBuffInstances.Find(ptBuff->id);
            ptPower->listBuffInstances.RemoveAt(pos);
        }
    }

    sEvaluateBuffRemovedAchievements(*ptBuff);
    PowerCheckForDone(ptBuff->idPower);

    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    ptPowerGlobals->pArrayBuffs->ElementDelete(ptBuff);
}

// PowerAPI.cpp:10076
BuffID PowerAPI_BuffFindOnACD(SNO snoPower, const ActorCommonData & tACD, ACDID idACDSource)
{
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
        if (!ptBuff->bDeleted && ptBuff->snoPower == snoPower &&
            (idACDSource == ACDID_INVALID || ptBuff->idACDSource == idACDSource))
            return idBuff;
    }
    return -1;
}

// PowerAPI.cpp:10105
BuffID PowerAPI_BuffFindOnACD(SNO snoPower, int32 nBuffIndex, const ActorCommonData & tACD, ACDID idACDSource, BOOL bAllowDeleted)
{
    BuffID idDeletedBuff = -1;
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
        if (ptBuff->snoPower == snoPower)
        {
            if (nBuffIndex != -1 && ptBuff->nBuffIndex != nBuffIndex)
                continue;
            if (idACDSource == ACDID_INVALID || ptBuff->idACDSource == idACDSource)
            {
                if (!ptBuff->bDeleted)
                    return idBuff;
                idDeletedBuff = idBuff;
            }
        }
    }
    return bAllowDeleted ? idDeletedBuff : -1;
}

// PowerAPI.cpp:10166
BuffID PowerAPI_BuffFindOnACD(SNO snoPower, const ActorCommonData * ptACD, ACDID idACDSource)
{
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
            if (!ptBuff->bDeleted && ptBuff->snoPower == snoPower &&
                (idACDSource == ACDID_INVALID || ptBuff->idACDSource == idACDSource))
                return idBuff;
        }
    }
    return -1;
}

// PowerAPI.cpp:10197
BuffID PowerAPI_BuffFindOnACDByPowerID(SNO snoPower, const ActorCommonData * ptACD, PowerID idPower)
{
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
            if (!ptBuff->bDeleted && ptBuff->snoPower == snoPower &&
                (idPower == -1 || ptBuff->idPower == idPower))
                return idBuff;
        }
    }
    return -1;
}

// PowerAPI.cpp:10228
BuffID PowerAPI_BuffFindOnACDByPowerID(SNO snoPower, int32 nBuffIndex, const ActorCommonData * ptACD, PowerID idPower, ACDID idACDSource, BOOL bAllowDeleted)
{
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
            if (ptBuff->snoPower == snoPower)
            {
                if (nBuffIndex != -1 && ptBuff->nBuffIndex != nBuffIndex)
                    continue;
                if (idPower != -1 && ptBuff->idPower != idPower)
                    continue;
                if (!bAllowDeleted && ptBuff->bDeleted)
                    continue;
                if (idACDSource != ACDID_INVALID && ptBuff->idACDSource != idACDSource)
                    continue;
                return idBuff;
            }
        }
    }
    return -1;
}

// PowerAPI.cpp:10300
BuffID PowerAPI_BuffFindOnACDByBuffData(SNO snoPower, int32 nBuffIndex, int32 nBuffData, const ActorCommonData & tACD, ACDID idACDSource, BOOL bAllowDeleted)
{
    BuffID idDeletedBuff = -1;
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
        if (ptBuff->snoPower == snoPower)
        {
            if (nBuffIndex != -1 && ptBuff->nBuffIndex != nBuffIndex)
                continue;
            if (ptBuff->nBuffData != nBuffData)
                continue;
            if (idACDSource == ACDID_INVALID)
            {
                if (!ptBuff->bDeleted)
                    return idBuff;
                idDeletedBuff = idBuff;
            }
            else if (ptBuff->idACDSource == idACDSource)
            {
                if (!ptBuff->bDeleted)
                    return idBuff;
                idDeletedBuff = idBuff;
            }
        }
    }
    return bAllowDeleted ? idDeletedBuff : -1;
}

// PowerAPI.cpp:10366
int32 PowerAPI_BuffCountOnACD(SNO snoPower, int32 nBuffIndex, ActorCommonData & tACD, const ACDID idACDSource)
{
    int32 nCount = 0;
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
        if (!ptBuff->bDeleted && ptBuff->snoPower == snoPower &&
            (nBuffIndex == -1 || ptBuff->nBuffIndex == nBuffIndex))
        {
            if (idACDSource != ACDID_INVALID && ptBuff->idACDSource != idACDSource)
                continue;
            ++nCount;
        }
    }
    return nCount;
}

// PowerAPI.cpp:10491
static void sApplyOnDoDamageBuffs(PowerAPIDamage & tPowerDamage, float * flDamageDone)
{
    ActorCommonData * ptACDAttacker = ACDTryToGet(tPowerDamage.idACDSource);
    if (!ptACDAttacker)
        return;
    POS pos = ptACDAttacker->listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = ptACDAttacker->listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnDoDamageEffect)
        {
            if (PowerTryToGet(ptBuff->idPower))
            {
                ptBuff->pOnDoDamageEffect(ptBuff, tPowerDamage, flDamageDone, tPowerDamage.idACDSource, tPowerDamage.idACDTarget);
            }
        }
    }
    Player * ptPlayer = ptACDAttacker->GetPetOwnerPlayer();
    if (ptPlayer)
    {
        ActorCommonData * ptACDOwner = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDOwner)
        {
            float flOldProcScalar = tPowerDamage.flProcScalar;
            tPowerDamage.flProcScalar *= ptACDAttacker->AttributesGetFloat(ATTRIBUTE_PET_PROC_SCALAR);
            POS posScoped = ptACDOwner->listBuffs.GetHeadPosition();
            while (posScoped)
            {
                BuffID idBuff = ptACDOwner->listBuffs.GetNext(posScoped);
                PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
                if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnDoDamageEffect && ptBuff->bTriggerOnDamageForPets)
                {
                    if (PowerTryToGet(ptBuff->idPower))
                    {
                        ptBuff->pOnDoDamageEffect(ptBuff, tPowerDamage, flDamageDone, tPowerDamage.idACDSource, tPowerDamage.idACDTarget);
                    }
                }
            }
            tPowerDamage.flProcScalar = flOldProcScalar;
        }
    }
    {
        float flUnadjustedDamage = 0.0f;
        POS posScoped = tPowerDamage.listDamage.GetHeadPosition();
        POS posFound = NULL;
        while (posScoped)
        {
            posFound = posScoped;
            const DamageAttribute & tDamageAttribute = tPowerDamage.listDamage.GetNext(posScoped);
            if (KeyGetAttrib(tDamageAttribute.tKey) == ATTRIBUTE_HITPOINTS_CUR)
            {
                flUnadjustedDamage = tDamageAttribute.flValue;
                break;
            }
        }
        if (flUnadjustedDamage < 0.0f)
        {
            float flAdjustedDamage = 0.0f;
            for (int32 iDamageType = 0; iDamageType < NUM_DAMAGE_TYPES; iDamageType++)
            {
                flAdjustedDamage -= flDamageDone[iDamageType];
            }
            DamageAttribute & tDamageAttribute = tPowerDamage.listDamage.GetAt(posFound);
            tDamageAttribute.flValue = flAdjustedDamage;
        }
    }
}


// PowerAPI.cpp:10423
void PowerAPI_BuffCopyAllVisualsToACD(const ActorCommonData & tACD, ActorCommonData & tACDDest)
{
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && !ptBuff->bDoNotCopy)
        {
            PowerAPIBuffStartData tBuffStartData(POWER_NONE);
            SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_COPIED_VISUAL_EFFECTS_BUFF);
            tBuffStartData.snoPower = snoPower;
            tBuffStartData.eStackType = 1;
            tBuffStartData.nMaxStackCount = 100;
            tBuffStartData.bIsCopy = TRUE;
            tBuffStartData.bInfinite = ptBuff->bInfinite;
            if (ptBuff->idPower == POWER_NONE)
            {
                continue;
            }
            Power * ptSourcePower = PowerTryToGet(ptBuff->idPower);
            if (!ptSourcePower)
            {
                continue;
            }
            if (tACDDest.IsPet())
            {
                ScopedPowerDefinitionLockR pdefPower(ptBuff->snoPower);
                if (!PowerBuffVisuallyAppliesToPets(pdefPower, ptBuff->nBuffIndex))
                {
                    continue;
                }
            }
            RuneType eRuneType = PowerGetRuneType(ptSourcePower);
            Attrib eAttrib = PowerRuneTypeAndBuffIndexToAttribute(ptBuff->snoPower, eRuneType, ptBuff->nBuffIndex);
            if (eAttrib != ATTRIBUTE_NONE)
            {
                FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(eAttrib, ptBuff->snoPower), 1);
            }
            if (!ptBuff->bInfinite && ptBuff->idTimerDone != TIMERID_INVALID && ptBuff->idTimerDone != 0 && TimerTryToGetOwner(ptBuff->idTimerDone) != -1)
            {
                uint32 dwTimeRemaining = TimerGetTime(ptBuff->idTimerDone) - GameGetServerTime();
                tBuffStartData.dwDurationMin = dwTimeRemaining;
            }
            else
            {
                tBuffStartData.bInfinite = TRUE;
                tBuffStartData.dwDurationMin = 0;
            }
            PowerAPI_BuffAddToACD(tBuffStartData, tACDDest);
        }
    }
}

// PowerAPI.cpp:10590
static void sApplyOnGetHitBuffs(PowerAPIDamage & tPowerDamage, float flHitpointDamage, ACDID idACDAttacker, ACDID idACDTarget)
{
    ActorCommonData & tACD = ACDGet(idACDTarget);
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnGetHitEffect)
        {
            if (PowerTryToGet(ptBuff->idPower) || ptBuff->idPower == POWER_NONE)
                ptBuff->pOnGetHitEffect(ptBuff, ptBuff->pOnGetHitEffectData, tPowerDamage, flHitpointDamage, idACDAttacker, idACDTarget);
        }
    }
}

// PowerAPI.cpp:10622
static void sApplyOnHitBuffs(PowerAPIDamage & tPowerDamage, float flHitpointDamage, float * flDamageDone, ACDID idACDAttacker, ACDID idACDTarget, ACDID idACDFromPet)
{
    ActorCommonData * ptACDAttacker = ACDTryToGet(idACDAttacker);
    if (ptACDAttacker && ptACDAttacker->IsAlive())
    {
        POS pos = ptACDAttacker->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACDAttacker->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnHitEffect)
            {
                BOOL bFromPet = idACDFromPet != ACDID_INVALID;
                if ((!bFromPet || ptBuff->bTriggerOnHitForPets) && PowerTryToGet(ptBuff->idPower))
                    ptBuff->pOnHitEffect(ptBuff, ptBuff->pOnHitEffectData, tPowerDamage, flHitpointDamage, flDamageDone, idACDAttacker, idACDTarget, idACDFromPet);
            }
        }
        Player * ptPlayer = ptACDAttacker->GetPetOwnerPlayer();
        if (ptPlayer)
        {
            float flOldProcScalar = tPowerDamage.flProcScalar;
            tPowerDamage.flProcScalar *= ptACDAttacker->AttributesGetFloat(ATTRIBUTE_PET_PROC_SCALAR);
            sApplyOnHitBuffs(tPowerDamage, flHitpointDamage, flDamageDone, ptPlayer->idPlayerACD, tPowerDamage.idACDTarget, idACDAttacker);
            tPowerDamage.flProcScalar = flOldProcScalar;
        }
    }
}

// PowerAPI.cpp:10673
static inline void sApplyOnFirstHitAttemptBuffs(ACDID idACDAttacker, ACDID idACDTarget, PowerID idPowerFrom, SNO snoPower)
{
    ActorCommonData * ptACDAttacker = ACDTryToGet(idACDAttacker);
    if (ptACDAttacker && ptACDAttacker->IsAlive())
    {
        POS pos = ptACDAttacker->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACDAttacker->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnFirstHitAttemptEffect)
                ptBuff->pOnFirstHitAttemptEffect(ptBuff, ptBuff->pOnFirstHitAttemptEffectData, idACDAttacker, idACDTarget, idPowerFrom, snoPower);
        }
    }
}

// PowerAPI.cpp:10704
static void sApplyOnDodgeBuffs(ACDID idACDAttacker, ACDID idACDTarget, const PowerAPIAttackPayload & tPayload)
{
    ActorCommonData & tACD = ACDGet(idACDTarget);
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnDodgeEffect)
        {
            ptBuff->pOnDodgeEffect(ptBuff, ptBuff->pOnDodgeEffectData, idACDAttacker, idACDTarget, tPayload);
        }
    }
}

// PowerAPI.cpp:10728
void PowerAPI_BuffFlagForDeletion(BuffID idBuff, BOOL bPlayEndEffect, BOOL bUpdateAuras, BuffRemovalReason eBuffRemovalReason, BOOL bDealPendingDOTDamage)
{
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff)
        PowerAPI_BuffFlagForDeletion(ptBuff, bPlayEndEffect, bUpdateAuras, eBuffRemovalReason, bDealPendingDOTDamage);
}

// PowerAPI.cpp:10745
// inferred: line 10815 loads bIsAura at col 17 (condition col 9) and branches at col 25 (the first &&), so bIsAura is the first
// operand; the bUpdateAuras/ptACD order is the one that reproduces the cbz order
void PowerAPI_BuffFlagForDeletion(
    PowerAPIBuffInstance * ptBuff,
    BOOL bPlayEndEffect,
    BOOL bUpdateAuras,
    BuffRemovalReason eBuffRemovalReason,
    BOOL bDealPendingDOTDamage)
{
    if (ptBuff->bDeleted)
    {
        return;
    }

    ptBuff->bDeleted = TRUE;


    if (!ActorFlagIsSet((ActorID)ACDGetOwner(ptBuff->idACDAffected), ACTOR_DEACTIVATING) &&
        !ActorFlagIsSet((ActorID)ACDGetOwner(ptBuff->idACDAffected), ACTOR_DELETING_BIT) && eBuffRemovalReason != BUFF_REMOVAL_REASON_DEATH && bDealPendingDOTDamage)
    {


        PowerAPI_BuffDealDOTDamage(*ptBuff, 0);
    }

    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    ptPowerGlobals->listBuffsFlaggedForDeletion.AddTail_Tracked(ptBuff->id);

    ActorCommonData * ptACD = ACDTryToGet(ptBuff->idACDAffected);
    if (FastAttribGroupIsValid(ptBuff->idFastAttrib) && ptACD &&
        FastAttribGroupIsValid(ptACD->idFastAttrib))
    {

        if (ptACD->IsAlive())
        {
            ActorID idActor = (ActorID)ACDGetOwner(ptACD->id);

            if (FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_STUNNED) ||
                FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_FROZEN) ||
                FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_FORCE_GRIPPED))
            {
                BrainStanceForceUpdate(idActor);
            }

            if (FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_BLIND))
            {
                BrainAPIUndoBlind(idActor);
            }
        }

        if (!ptBuff->bAppliesOnlyToSpecificPowers)
        {
            FastAttribUnmerge(ptACD->idFastAttrib, ptBuff->idFastAttrib);
        }
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();

    if (ptBuff->pRemovalEffect && !(tSGameGlobals.dwFlags & (1 << 1)))
    {
        ptBuff->pRemovalEffect(ptBuff, eBuffRemovalReason, ptBuff->pRemovalEffectData);
    }

    if (ptBuff->bIsAura && bUpdateAuras && ptACD)
    {
        sUpdateAuras(*ptACD);
    }

    if (ptACD)
    {
        ActorID idActor = (ActorID)ACDGetOwner(ptACD->id);
        AIStateRemoveMergesFromBuff(idActor, ptBuff->id);
    }

    if (bPlayEndEffect && ptBuff->snoEndingEffect != SNO_NONE)
    {
        ACDPlayEffect(ptBuff->idACDAffected, EFFECT_PLAY_EFFECT_GROUP, ptBuff->snoEndingEffect, ptBuff->idACDSource);
    }

    if (ptBuff->idFastAttrib != ATTRIBGROUPID_INVALID)
    {
        FastAttribGroupDelete(ptBuff->idFastAttrib);
    }

    if (ptBuff->idTimerPeriodicEffect != TIMERID_INVALID)
    {
        PowerTriggerRemoveTimerTrigger(ptBuff->idPower, ptBuff->idTimerPeriodicEffect);
    }

    if (ptBuff->idTimerDone != TIMERID_INVALID)
    {
        TimerUnregister(ptBuff->idTimerDone);
    }

    if (ptACD)
    {
        ptACD->ModifiedMark(0);

        if (ptACD->IsItem())
        {
            VisualInventoryUpdateItem(*ptACD);
        }

        ACDID idACDSource = ptBuff->idACDSource;
        ActorCommonData * ptACDSource = ACDTryToGet(idACDSource);
        if (ptACDSource
            && ptACDSource->IsPet())
        {
            idACDSource = ACDGetPetOwner(ptACDSource->id);
        }
        if (ACDIsPlayer(idACDSource))
        {
            Player * ptPlayerMain = PlayerGetByACD(idACDSource);
            if (ptPlayerMain)
            {
                SGameStatsPlayerCounterIncrement(ptPlayerMain, STAT_COUNTER_PLAYER_POWER_BUFF_UPTIME, ptBuff->snoPower, ptBuff->nBuffIndex, (float)(GameGetServerTime() - ptBuff->dwStartTick) / 60.0f);
                SGameStatsPlayerCounterIncrement(ptPlayerMain, STAT_COUNTER_PLAYER_POWER_BUFF_APPLICATION, ptBuff->snoPower, ptBuff->nBuffIndex, 1.0f);
            }
        }

        if (ptBuff->nLookOverrideHash)
        {
            ActorLookOverrideChangedMessage tOverrideMessage;
            tOverrideMessage.ann = ANNFromACD(ptACD->id);
            tOverrideMessage.nChange = -1;
            tOverrideMessage.nLookOverrideHash = ptBuff->nLookOverrideHash;

            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetAllConnected(listDestPlayers, -1, FALSE);

            MessageSendToClient(listDestPlayers, SMSG_ACTOR_LOOK_OVERRIDE_CHANGED, &tOverrideMessage, sizeof(tOverrideMessage));
        }
    }

    ptBuff->listAIStatesToMerge.RemoveAll();
}


// PowerAPI.cpp:10905
void PowerAPI_BuffRemoveOnDeath(ActorCommonData & tACD, ACDID idACDKiller)
{
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
        if (ptBuff->bDeleted)
            continue;
        if (ptBuff->pDeathEffect)
            ptBuff->pDeathEffect(ptBuff, ptBuff->pDeathEffectData, tACD.id, idACDKiller);
        if (ptBuff->bRemoveOnDeath)
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, 5, TRUE);  // BUFF_REMOVAL_REASON_DEATH
    }
}

// PowerAPI.cpp:10933
BOOL PowerAPI_BuffHasLookswitchBuff(ActorCommonData & tACD)
{
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
        if (ptBuff->nLookOverrideHash)
            return TRUE;
    }
    return FALSE;
}

// PowerAPI.cpp:10954
void PowerAPI_BuffRemoveLookswitchBuffs(ActorCommonData & tACD)
{
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
        if (ptBuff->nLookOverrideHash)
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEATH, TRUE);
    }
}

// PowerAPI.cpp:10974
void PowerAPI_BuffFlagAllForDeletion(ActorCommonData & tACD)
{
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    }
}

// PowerAPI.cpp:10989
void PowerAPI_BuffFlagAllFromPowerForDeletion(ActorCommonData & tACD, SNO snoPower, int32 nBuffIndex, BuffRemovalReason eRemovalReason, ACDID idActorSource)
{
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
        if (!ptBuff->bDeleted &&
            ptBuff->snoPower == snoPower &&
            (idActorSource == ACDID_INVALID || ptBuff->idACDSource == idActorSource) &&
            (nBuffIndex == -1 || ptBuff->nBuffIndex == nBuffIndex))
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, eRemovalReason, TRUE);
        }
    }
}

// PowerAPI.cpp:11020
void PowerAPI_BuffDeleteAllFlagged()
{
    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;

    while (!ptPowerGlobals->listBuffsFlaggedForDeletion.IsEmpty())
    {
        BuffID idBuff = ptPowerGlobals->listBuffsFlaggedForDeletion.GetHead();
        ptPowerGlobals->listBuffsFlaggedForDeletion.RemoveHead();
        PowerAPIBuffInstance * ptBuff = ptPowerGlobals->pArrayBuffs->ElementGet(idBuff);
        if (ptBuff)
            sPowerAPI_BuffDelete(ptBuff);
    }
}

// PowerAPI.cpp:11045
void PowerAPI_BuffRemoveAllNegativeBuffs(ActorCommonData & tACD)
{
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && !ptBuff->bPositiveEffect && !ptBuff->bUndispellable)
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, FALSE);
    }
}

// PowerAPI.cpp:11064
void PowerAPI_BuffRemoveExclusiveType(ActorCommonData & tACD, BuffExclusiveType eExclusiveType)
{
    if (eExclusiveType == BUFF_EXCLUSIVE_TYPE_NONE)
        return;
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->eExclusiveType == eExclusiveType)
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    }
}

// PowerAPI.cpp:11088
void PowerAPI_BuffDeleteAll()
{
    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;

    TDataArrayIterator<PowerAPIBuffInstance> tIt;
    while (PowerAPIBuffInstance * ptBuff = ptPowerGlobals->pArrayBuffs->Iterate(tIt))
    {
        sPowerAPI_BuffDelete(ptBuff);
    }

    ptPowerGlobals->pArrayBuffs->DeleteAll();
}

// PowerAPI.cpp:11104
void PowerAPI_BuffSetEndTick(BuffID idBuff, uint32 dwEndTick)
{
    PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
    if (ptBuff)
    {
        // inferred: line 11111 tests bInfinite (member at col 15, condition col 7) and branches straight to the return at 11135
        if (ptBuff->bInfinite)
            return;
        uint32 dwNow = GameGetServerTime();
        // DWARF: dwCurrEndTick is computed at 11119, before the condition at 11121; deletion (11123) comes before the delta (11127)
        uint32 dwCurrEndTick = ptBuff->dwStartTick + ptBuff->dwDuration;
        if (dwNow >= dwEndTick || dwNow >= dwCurrEndTick)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, 0, TRUE);
        }
        else
        {
            int32 nDeltaTick = dwEndTick - dwCurrEndTick;
            if (nDeltaTick != 0)
                PowerAPI_BuffModifyDuration(idBuff, nDeltaTick, FALSE, TRUE);
        }
    }
}

// PowerAPI.cpp:11139

uint32 PowerAPI_BuffModifyDuration(BuffID idBuff, int32 nDeltaTicks, BOOL bWantInfinite, BOOL bResetStartTick)
{
    uint32 dwTicksLeft = 0;
    PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
    if (ptBuff)
    {
        uint32 dwNow = GameGetServerTime();
        int32 nCurrDuration = ptBuff->dwDuration + nDeltaTicks;
        ptBuff->dwDuration = Math::Max(nCurrDuration, 0);
        int32 nTicksLeft = ptBuff->bInfinite ? ptBuff->dwDuration : ptBuff->dwStartTick + ptBuff->dwDuration - dwNow;
        if (!bWantInfinite && nTicksLeft <= 0)
        {
            PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, 0, TRUE);  // original line 11159, laid out before the timer body
        }
        else
        {
            if (TimerExists(ptBuff->idTimerDone))
                TimerUnregister(ptBuff->idTimerDone);
            if (ptBuff->bInfinite)
                ptBuff->bInfinite = FALSE;
            if (bWantInfinite)
            {
                dwTicksLeft = 0;
                ptBuff->bInfinite = TRUE;
            }
            else
            {
                dwTicksLeft = nTicksLeft;
                ptBuff->idTimerDone = TimerRegister(idBuff, TIMER_OWNER_BUFF, dwTicksLeft, NULL, sBuffDoneCallback);
            }
            if ((PowerBuffIsDisplayed(AutoPowerDefinitionR(ptBuff->snoPower), ptBuff->nBuffIndex) ||
                 PowerBuffShowsActiveOnSkillButton(AutoPowerDefinitionR(ptBuff->snoPower), ptBuff->nBuffIndex)) && ptBuff->idFastAttrib != -1)
            {
                ActorCommonData * ptACD = ACDTryToGet(ptBuff->idACDAffected);
                if (ptACD)
                {
                    Attrib eAttribStart, eAttribEnd, eAttribCount;
                    PowerGetBuffAttributes(ptBuff->nBuffIndex, eAttribStart, eAttribEnd, eAttribCount);
                    uint32 dwEndTick = dwNow + dwTicksLeft;
                    if (bResetStartTick)
                        FastAttribSetValueInt(ptBuff->idFastAttrib, MakeAttribKey(eAttribStart, ptBuff->snoPower), dwNow);
                    if (bWantInfinite)
                        FastAttribSetValueToDefault(ptBuff->idFastAttrib, MakeAttribKey(eAttribEnd, ptBuff->snoPower));
                    else
                        FastAttribSetValueInt(ptBuff->idFastAttrib, MakeAttribKey(eAttribEnd, ptBuff->snoPower), dwEndTick);
                    FastAttribUnmerge(ptACD->idFastAttrib, ptBuff->idFastAttrib);
                    ptACD->AttributesMergeAttributeGroup(ptBuff->idFastAttrib);
                }
            }
        }
    }
    return dwTicksLeft;
}

// PowerAPI.cpp:11230
void PowerAPI_BuffBackdate(BuffID idBuff, uint32 dwElapsedTime)
{
    PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
    uint32 dwNow = GameGetServerTime();
    if (ptBuff)
    {
        // inferred: separate checks on lines 11239 / 11245 / 11251 (each its own branch row), early-return form
        if (ptBuff->bInfinite)
            return;
        if (ptBuff->dwStartTick != dwNow)
            return;
        // inferred: line 11251 loads ptBuff->dwDuration at col 21 (condition col 13), so the member operand comes first
        if (ptBuff->dwDuration > dwElapsedTime && dwNow > dwElapsedTime)
        {
            if (TimerExists(ptBuff->idTimerDone))
                TimerUnregister(ptBuff->idTimerDone);
            ptBuff->idTimerDone = TimerRegister(idBuff, TIMER_OWNER_BUFF, ptBuff->dwDuration - dwElapsedTime, NULL, sBuffDoneCallback);
            ptBuff->dwStartTick -= dwElapsedTime;
            ptBuff->dwInitialInstanceStartTick = ptBuff->dwStartTick;
            uint32 dwEndTick = ptBuff->dwStartTick + ptBuff->dwDuration;
            if ((PowerBuffIsDisplayed(AutoPowerDefinitionR(ptBuff->snoPower), ptBuff->nBuffIndex) ||
                 PowerBuffShowsActiveOnSkillButton(AutoPowerDefinitionR(ptBuff->snoPower), ptBuff->nBuffIndex)) && ptBuff->idFastAttrib != -1)
            {
                ActorCommonData * ptACD = ACDTryToGet(ptBuff->idACDAffected);
                if (ptACD)
                {
                    Attrib eAttribStart, eAttribEnd, eAttribCount;
                    PowerGetBuffAttributes(ptBuff->nBuffIndex, eAttribStart, eAttribEnd, eAttribCount);
                    FastAttribSetValueInt(ptBuff->idFastAttrib, MakeAttribKey(eAttribStart, ptBuff->snoPower), ptBuff->dwStartTick);
                    FastAttribSetValueInt(ptBuff->idFastAttrib, MakeAttribKey(eAttribEnd, ptBuff->snoPower), dwEndTick);
                    FastAttribUnmerge(ptACD->idFastAttrib, ptBuff->idFastAttrib);
                    ptACD->AttributesMergeAttributeGroup(ptBuff->idFastAttrib);
                }
            }
        }
    }
}

// PowerAPI.cpp:11310
void PowerAPI_BuffRefreshDuration(BuffID idBuff)
{
    PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
    if (ptBuff && !ptBuff->bInfinite && !ptBuff->bDeleted)
    {
        uint32 dwNow = GameGetServerTime();
        int32 nDuration = ptBuff->dwDuration;
        int32 nTicksLeft = ptBuff->dwStartTick + nDuration - dwNow;
        if (nTicksLeft <= 0)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, 0, TRUE);
        }
        else
        {
            uint32 dwTicksLeft = nDuration;
            if (TimerExists(ptBuff->idTimerDone))
                TimerUnregister(ptBuff->idTimerDone);
            ptBuff->idTimerDone = TimerRegister(idBuff, TIMER_OWNER_BUFF, dwTicksLeft, NULL, sBuffDoneCallback);
            ptBuff->dwStartTick = dwNow;
            if ((PowerBuffIsDisplayed(AutoPowerDefinitionR(ptBuff->snoPower), ptBuff->nBuffIndex) ||
                 PowerBuffShowsActiveOnSkillButton(AutoPowerDefinitionR(ptBuff->snoPower), ptBuff->nBuffIndex)) && ptBuff->idFastAttrib != -1)
            {
                ActorCommonData * ptACD = ACDTryToGet(ptBuff->idACDAffected);
                if (ptACD)
                {
                    Attrib eAttribStart, eAttribEnd, eAttribCount;
                    PowerGetBuffAttributes(ptBuff->nBuffIndex, eAttribStart, eAttribEnd, eAttribCount);
                    uint32 dwEndTick = dwNow + dwTicksLeft;
                    FastAttribSetValueInt(ptBuff->idFastAttrib, MakeAttribKey(eAttribStart, ptBuff->snoPower), dwNow);
                    FastAttribSetValueInt(ptBuff->idFastAttrib, MakeAttribKey(eAttribEnd, ptBuff->snoPower), dwEndTick);
                    FastAttribUnmerge(ptACD->idFastAttrib, ptBuff->idFastAttrib);
                    ptACD->AttributesMergeAttributeGroup(ptBuff->idFastAttrib);
                }
            }
        }
    }
}

// PowerAPI.cpp:11385
inline DamageShieldPriorityEntry::DamageShieldPriorityEntry()
    : idBuff(-1)
    , nPriority(-1)
    , nTicksLeft(0)
{
}


// PowerAPI.cpp:11397
static int32 sCompareDamageShieldPriorityEntries(const void * lhs, const void * rhs)
{
    const DamageShieldPriorityEntry * pLeft = (const DamageShieldPriorityEntry *)lhs;
    const DamageShieldPriorityEntry * pRight = (const DamageShieldPriorityEntry *)rhs;
    if (pLeft->nPriority < pRight->nPriority)
        return 1;
    if (pRight->nPriority < pLeft->nPriority)
        return -1;
    return (pLeft->nTicksLeft < pRight->nTicksLeft) ? -1 : (pRight->nTicksLeft < pLeft->nTicksLeft);
}

// PowerAPI.cpp:11421
static BOOL sApplyDamageShieldBuffs(ACDID idACDTarget, float * flDamageDone, PowerAPIDamage & tPowerDamage)
{
    if (tPowerDamage.bProcessedShields)
        return FALSE;
    tPowerDamage.bProcessedShields = TRUE;
    if (!flDamageDone)
        return FALSE;
    ActorCommonData & tACD = ACDGet(idACDTarget);
    BOOL bProcessedShields = FALSE;
    uint32 dwNow = GameGetServerTime();
    blz::vector<DamageShieldPriorityEntry> tDamageShieldPriorityArray;
    POS posBuff = tACD.listBuffs.GetHeadPosition();
    while (posBuff)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(posBuff);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pDamageShieldEffect && (!tPowerDamage.bIsStaggerDamage || ptBuff->bProcessDamageShieldOnStaggerDamage))
        {
            DamageShieldPriorityEntry tEntry;
            tEntry.idBuff = ptBuff->id;
            tEntry.nPriority = ptBuff->nDamageShieldPriority;
            int32 nDuration = ptBuff->dwDuration;
            tEntry.nTicksLeft = nDuration - (dwNow - ptBuff->dwStartTick);
            if (ptBuff->bDamageShieldIsEvaluatedLast)
            {
                tEntry.nPriority = INT_MAX;
            }
            tDamageShieldPriorityArray.push_back(tEntry);
        }
    }
    blzext::vector_sort(tDamageShieldPriorityArray, sCompareDamageShieldPriorityEntries);
    for (uint32 i = 0; i < tDamageShieldPriorityArray.size(); i++)
    {
        const DamageShieldPriorityEntry & tEntry = tDamageShieldPriorityArray[i];
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(tEntry.idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pDamageShieldEffect)
        {
            ptBuff->pDamageShieldEffect(ptBuff, idACDTarget, flDamageDone, tPowerDamage, ptBuff->pDamageShieldEffectData);
            bProcessedShields = TRUE;
        }
    }
    return bProcessedShields;
}

// PowerAPI.cpp:11489
void PowerAPI_BuffApplyOnKillEffects(float * flDamageDone, PowerAPIDamage * ptPowerDamage, ACDID idACDAttacker, ACDID idACDKilled)
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_SHUTTING_DOWN))
        return;
    ActorCommonData & tACD = ACDGet(idACDAttacker);
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnKillEffect)
            ptBuff->pOnKillEffect(ptBuff, ptBuff->pOnKillEffectData, flDamageDone, ptPowerDamage, idACDAttacker, idACDKilled);
    }
}

// PowerAPI.cpp:11524
void PowerAPI_BuffApplyOnAssistEffects(ACDID idACDAttacker, ACDID idACDKilled)
{
    ActorCommonData & tACD = ACDGet(idACDAttacker);
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnAssistEffect)
            ptBuff->pOnAssistEffect(ptBuff, ptBuff->pOnAssistEffectData, idACDAttacker, idACDKilled);
    }
}

// PowerAPI.cpp:11549
void PowerAPI_BuffApplyOnItemPickUpEffects(ACDID idACD, ACDID idACDItem, PickupAction ePickUpAction)
{
    ActorCommonData & tACD = ACDGet(idACD);
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnItemPickUpEffect)
            ptBuff->pOnItemPickUpEffect(ptBuff, ptBuff->pOnItemPickUpEffectData, idACD, idACDItem, ePickUpAction);
    }
}

// PowerAPI.cpp:11573
void PowerAPI_BuffApplyOnAttackEffects(ACDID idACDAttacker, ACDID idACDTarget, PowerID idPowerFrom, SNO snoPower)
{
    ActorCommonData & tACD = ACDGet(idACDAttacker);
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnAttackEffect)
            ptBuff->pOnAttackEffect(ptBuff, ptBuff->pOnAttackEffectData, idACDAttacker, idACDTarget, idPowerFrom, snoPower);
    }
}

// PowerAPI.cpp:11600
BOOL PowerAPI_BuffApplyReflectEffects(ACDID idACDTarget, ImpactData * ptImpact)
{
    ActorCommonData & tACD = ACDGet(idACDTarget);
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pShouldReflectTest && ptBuff->pShouldReflectTest(ptBuff, ptImpact, ptBuff->pReflectEffectData))
        {
            BOOL bReflected = TRUE;
            if (ptBuff->pReflectEffect)
                bReflected = ptBuff->pReflectEffect(ptBuff, ptImpact, ptBuff->pReflectEffectData);
            else
                ProjectileReflect(ptImpact->idACDProjectile, idACDTarget, FALSE, 0.0f, (Team)-1, 1.0f);
            if (!bReflected)
                continue;
            ptImpact->bStop = FALSE;
            return TRUE;
        }
    }
    return FALSE;
}

// PowerAPI.cpp:11645
void PowerAPI_BuffApplyOnProjectileCreateEffects(ACDID idACDAttacker, ACDID idACDProjectile)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDAttacker);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnProjectileCreateEffect)
                ptBuff->pOnProjectileCreateEffect(ptBuff, ptBuff->pOnProjectileCreateEffectData, idACDAttacker, idACDProjectile);
        }
    }
}

// PowerAPI.cpp:11672
void PowerAPI_BuffApplyOnKnockbackLandEffects(ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnKnockbackLandEffect)
                ptBuff->pOnKnockbackLandEffect(ptBuff);
        }
    }
}

// PowerAPI.cpp:11696
void PowerAPI_BuffApplyOnPetAddEffects(ACDID idACDSummoner, ACDID idACDPet, PetType ePetType, SNO snoPet)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDSummoner);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnPetAddEffect)
                ptBuff->pOnPetAddEffect(ptBuff, idACDSummoner, idACDPet, ePetType, snoPet);
        }
    }
}

// PowerAPI.cpp:11722
void PowerAPI_BuffApplyOnGetHitWithCrowdControlEffects(
    PowerID idPowerFrom,
    ACDID idACDAttacker,
    ACDID idACDTarget,
    SNO snoPower,
    CrowdControlType eCCType,
    uint32 dwDuration,
    float flProcScalar,
    BOOL bRefresh)
{


    ActorCommonData * ptACD = ACDTryToGet(idACDTarget);
    if (ptACD)
    {
        if (GameTypeIsPvP())
        {
            Player * ptPlayerAttacker = ACDGetControllingPlayer(idACDAttacker);

            SNO snoPowerToReport = snoPower;
            Power * ptPowerFrom = PowerTryToGet(idPowerFrom);
            if (ptPowerFrom)
            {
                snoPowerToReport = ptPowerFrom->snoPower;
            }

            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(ptPlayerAttacker);
            tEvaluationData.SetPower(snoPowerToReport);

            if (ACDIsPlayer(idACDTarget) && ptPlayerAttacker)
            {
                switch (eCCType)
                {
                case CROWD_CONTROL_STUN:
                    SGameStatsPlayerCounterIncrement(ptPlayerAttacker, STAT_COUNTER_PLAYER_APPLIED_STUN_TO_PLAYER, 0, 0, 1.0f);
                    tEvaluationData.EvaluateEvent(ptPlayerAttacker, Achievement::ACH_EVENT_PVP_PLAYER_APPLIED_STUN_TO_PLAYER, snoPowerToReport, (uint32)sub_71007b5e70());
                    break;

                case CROWD_CONTROL_IMMOBILIZE:
                    SGameStatsPlayerCounterIncrement(ptPlayerAttacker, STAT_COUNTER_PLAYER_APPLIED_IMMOBILIZE_TO_PLAYER, 0, 0, 1.0f);
                    tEvaluationData.EvaluateEvent(ptPlayerAttacker, Achievement::ACH_EVENT_PVP_PLAYER_APPLIED_IMMOBILIZE_TO_PLAYER, snoPowerToReport, (uint32)sub_71007b5e70());
                    break;

                case CROWD_CONTROL_FEAR:
                    SGameStatsPlayerCounterIncrement(ptPlayerAttacker, STAT_COUNTER_PLAYER_APPLIED_FEAR_TO_PLAYER, 0, 0, 1.0f);
                    tEvaluationData.EvaluateEvent(ptPlayerAttacker, Achievement::ACH_EVENT_PVP_PLAYER_APPLIED_FEAR_TO_PLAYER, snoPowerToReport, (uint32)sub_71007b5e70());
                    break;

                case CROWD_CONTROL_FREEZE:
                    SGameStatsPlayerCounterIncrement(ptPlayerAttacker, STAT_COUNTER_PLAYER_APPLIED_FREEZE_TO_PLAYER, 0, 0, 1.0f);
                    tEvaluationData.EvaluateEvent(ptPlayerAttacker, Achievement::ACH_EVENT_PVP_PLAYER_APPLIED_FREEZE_TO_PLAYER, snoPowerToReport, (uint32)sub_71007b5e70());
                    break;

                case CROWD_CONTROL_BLIND:
                    SGameStatsPlayerCounterIncrement(ptPlayerAttacker, STAT_COUNTER_PLAYER_APPLIED_BLIND_TO_PLAYER, 0, 0, 1.0f);
                    tEvaluationData.EvaluateEvent(ptPlayerAttacker, Achievement::ACH_EVENT_PVP_PLAYER_APPLIED_BLIND_TO_PLAYER, snoPowerToReport, (uint32)sub_71007b5e70());
                    break;

                }
            }
        }

        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff)
            {
                if (!bRefresh || ptBuff->bOnGetHitWithCrowdControlCallOnRefresh)
                {
                    if (!ptBuff->bDeleted && ptBuff->pOnGetHitWithCrowdControlEffect)
                    {
                        ptBuff->pOnGetHitWithCrowdControlEffect(idPowerFrom, ptBuff, idACDAttacker, idACDTarget, snoPower, eCCType, dwDuration * OO_TICKS_PER_SECOND, flProcScalar);
                    }
                }
            }
        }
    }
}

// PowerAPI.cpp:11803
void PowerAPI_BuffApplyOnHitWithCrowdControlEffects(PowerID idPowerFrom, ACDID idACDAttacker, ACDID idACDTarget, SNO snoPower, CrowdControlType eCCType, uint32 dwDuration, float flProcScalar, BOOL bRefresh)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDAttacker);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && (!bRefresh || ptBuff->bOnHitWithCrowdControlCallOnRefresh) && !ptBuff->bDeleted && ptBuff->pOnHitWithCrowdControlEffect)
                ptBuff->pOnHitWithCrowdControlEffect(idPowerFrom, ptBuff, idACDAttacker, idACDTarget, snoPower, eCCType, dwDuration * OO_TICKS_PER_SECOND, flProcScalar);
        }
    }
}

// PowerAPI.cpp:11840
void PowerAPI_BuffApplyOnEquipmentChangeEffects(ACDID idACDActor, InventorySlotType eSlot, EquipmentChangeReason eReason)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnEquipmentChangeEffect)
                ptBuff->pOnEquipmentChangeEffect(ptBuff, idACDActor, eSlot, eReason);
        }
    }
}

// PowerAPI.cpp:11865
void PowerAPI_BuffApplyOnResourceGainEffects(ACDID idACD, float flFuryAdded)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnResourceGainEffect)
                ptBuff->pOnResourceGainEffect(ptBuff, flFuryAdded);
        }
    }
}

// PowerAPI.cpp:11890
void PowerAPI_BuffApplyOnSkillChangeEffects(ACDID idACDActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnSkillChangeEffect)
                ptBuff->pOnSkillChangeEffect(ptBuff, idACDActor);
        }
    }
}

// PowerAPI.cpp:11914
void PowerAPI_BuffApplyOnMessageEffects(ACDID idACDCaster, SNO snoTargetPower, int32 nBuffIndex, int32 dwParam0, int32 dwParam1, float flParam2)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDCaster);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && ptBuff->snoPower == snoTargetPower && ptBuff->nBuffIndex == nBuffIndex && !ptBuff->bDeleted && ptBuff->pOnMessageEffect)
                ptBuff->pOnMessageEffect(ptBuff, idACDCaster, dwParam0, dwParam1, flParam2);
        }
    }
}

// PowerAPI.cpp:11943
void PowerAPI_BuffApplyOnCalloutEventEffects(ACDID idACDActor, CalloutEventType eEvent)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnCalloutEventEffect)
                ptBuff->pOnCalloutEventEffect(ptBuff, idACDActor, eEvent);
        }
    }
}

// PowerAPI.cpp:11968
void PowerAPI_BuffApplyOnGizmoOperatedEffects(ACDID idACDActor, ActorCommonData & tACDGizmo)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnGizmoOperatedEffect)
                ptBuff->pOnGizmoOperatedEffect(ptBuff, idACDActor, tACDGizmo);
        }
    }
}

// PowerAPI.cpp:11993
void PowerAPI_BuffApplyOnWreckableDestroyedEffects(ACDID idACDAttacker, ACDID idACDKilled)
{
    ActorCommonData * ptACDAttacker = ACDTryToGet(idACDAttacker);
    ActorCommonData * ptACDKilled = ACDTryToGet(idACDKilled);
    if (ptACDAttacker && ptACDKilled)
    {
        POS pos = ptACDAttacker->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACDAttacker->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnWreckableDestroyedEffect)
                ptBuff->pOnWreckableDestroyedEffect(ptBuff, idACDAttacker, idACDKilled);
        }
    }
}

// PowerAPI.cpp:12019
void PowerAPI_BuffApplyOnTeleportedEffects(ACDID idACDActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnTeleportedEffect)
                ptBuff->pOnTeleportedEffect(ptBuff, idACDActor);
        }
    }
}

// PowerAPI.cpp:12043
void PowerAPI_BuffApplyOnPowerDamageInitEffect(ACDID idACDSource, ACDID idACDTarget, PowerAPIDamage & tPowerDamage)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDSource);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnPowerDamageInitEffect)
                ptBuff->pOnPowerDamageInitEffect(ptBuff, idACDSource, idACDTarget, tPowerDamage);
        }
    }
}

// PowerAPI.cpp:12069
void PowerAPI_BuffApplyOnLootDropEffect(ACDID idACDPlayer, ActorCommonData & tACDItem, LootContext eLootContext)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer && ACDIsPlayer(idACDPlayer))
    {
        POS pos = ptACDPlayer->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACDPlayer->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnLootDropEffect)
                ptBuff->pOnLootDropEffect(ptBuff, idACDPlayer, tACDItem, eLootContext);
        }
    }
}

// PowerAPI.cpp:12095
void PowerAPI_BuffApplyOnItemSoldEffect(ACDID idACDPlayer, ActorCommonData & tACDItem)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer && ACDIsPlayer(idACDPlayer))
    {
        POS pos = ptACDPlayer->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACDPlayer->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnItemSoldEffect)
                ptBuff->pOnItemSoldEffect(ptBuff, idACDPlayer, tACDItem);
        }
    }
}

// PowerAPI.cpp:12120
void PowerAPI_BuffApplyOnLevelEffect(ActorCommonData & tACD)
{
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnLevelEffect)
            ptBuff->pOnLevelEffect(ptBuff, tACD.id);
    }
}

// PowerAPI.cpp:12140
void PowerAPI_BuffApplyOnCorpseConsumedEffect(ActorCommonData & tACD, ACDID idACDCorpse)
{
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnCorpseConsumedEffect)
            ptBuff->pOnCorpseConsumedEffect(ptBuff, tACD.id, idACDCorpse);
    }
}

// PowerAPI.cpp:12161
void PowerAPI_BuffApplyOnPetDismissEffect(ActorCommonData & tACD, ACDID idACDPlayer, ACDID idACDPet)
{
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnPetDismissEffect)
            ptBuff->pOnPetDismissEffect(ptBuff, idACDPlayer, idACDPet);
    }
}

// PowerAPI.cpp:12184
void PowerAPI_BuffApplyOnCostEffects(ACDID idACD, PowerID idPowerFrom, SNO snoPower, float flResourceCost, ResourceType eResourceType, float flHealthCost, BOOL bCostsWereCharged, float flHealthCostUndiscounted)
{
    ActorCommonData & tACD = ACDGet(idACD);
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnCostEffect)
            ptBuff->pOnCostEffect(ptBuff, ptBuff->pOnCostEffectData, idACD, idPowerFrom, snoPower, flResourceCost, eResourceType, flHealthCost, bCostsWereCharged, flHealthCostUndiscounted);
    }
}

// PowerAPI.cpp:12215
void PowerAPI_BuffApplyOnHealFromSkillEffects(ACDID idACD, PowerID idPowerFrom, SNO snoPower, float flHealAmount, float flOverhealAmount, int32 nNumberOfHeals)
{
    ActorCommonData & tACD = ACDGet(idACD);
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->pOnHealFromSkillEffect)
            ptBuff->pOnHealFromSkillEffect(ptBuff, idACD, idPowerFrom, snoPower, flHealAmount, flOverhealAmount, nNumberOfHeals);
    }
}

// PowerAPI.cpp:12244
void PowerAPI_BuffMergeAttribsForPowers(ActorCommonData & tACD, SNO snoPower, XPooledDataIDList & listFastAttribs)
{
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff == NULL || ptBuff->bDeleted)
            continue;

        int32 nBuffIdx = ptBuff->nBuffIndex;
        if (nBuffIdx < 0 || nBuffIdx >= 32)
            continue;
        if (ptBuff->snoPower == SNO_NONE)
            continue;

        ScopedPowerDefinitionLockR pdefPower(ptBuff->snoPower);
        const BuffDef & tBuffDef = pdefPower->tPower.arBuff[nBuffIdx];
        if (tBuffDef.serBuffFilters.dwSize)
        {
            int32 nEntryCount = tBuffDef.serBuffFilters.ElementCount(sizeof(BuffFilterDef));
            for (int32 i = 0; i < nEntryCount; i++)
            {
                const BuffFilterDef & tBuffFilterDef = tBuffDef.arBuffFilters[i];
                if (tBuffFilterDef.snoPower == snoPower)
                {
                    if (tBuffFilterDef.dwRunes != 0)
                    {
                        RuneType eRuneType = PowerGetRuneTypeForACD(tACD, snoPower);
                        if ((tBuffFilterDef.dwRunes & (1 << (eRuneType + 1))) == 0)
                            break;
                    }
                    FastAttribMerge(tACD.idFastAttrib, ptBuff->idFastAttrib);
                    listFastAttribs.AddTail_Tracked(ptBuff->idFastAttrib);
                    break;
                }
            }
        }
    }
}

// PowerAPI.cpp:12301
void PowerAPI_BuffUnmergeAttribsForPowers(ActorCommonData & tACD, XPooledDataIDList & listFastAttribs)
{
    POS pos = listFastAttribs.GetHeadPosition();
    while (pos)
    {
        AttribGroupID idFastAttrib = listFastAttribs.GetNext(pos);
        FastAttribUnmerge(tACD.idFastAttrib, idFastAttrib);
    }
    listFastAttribs.RemoveAll();
}

// PowerAPI.cpp:12318
BOOL PowerAPI_BuffIsSnare(const PowerAPIBuffInstance * ptBuff)
{
    return FastAttribGetValueFloat(ptBuff->idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT) > EPSILON;
}

// PowerAPI.cpp:12329
BOOL PowerAPI_BuffIsSnare(
    const PowerAPIBuffStartData & tData)
{
    return FastAttribGetValueFloat(tData.idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT) > EPSILON;
}

// PowerAPI.cpp:12342
void PowerAPI_BuffDealDOTDamage(
    PowerAPIBuffInstance & tBuff,
    uint32 uInterval)
{
    if (Math::Abs(tBuff.flDamagePerSecond) > EPSILON)
    {
        uint32 uNow = GameGetServerTime();
        uint32 uTicksElapsed = uNow - tBuff.uLastDOTTick;
        float flDamageDealt = uTicksElapsed * tBuff.flDamagePerSecond * OO_TICKS_PER_SECOND;
        if (Math::Abs(flDamageDealt) > EPSILON)
        {
            SGameGlobals & tSGameGlobals = SGameGlobalsGet();
            if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_SLOW_KILL_BIT))
            {
                flDamageDealt *= 0.01f;
            }

            float flDamageDone[7] = { 0.0f };
            flDamageDone[tBuff.eDamageType] += flDamageDealt;

            PowerAPIDamage tDamageOverTime;
            PowerID idPowerToUse =
                PowerTryToGet(tBuff.idPower) ? tBuff.idPower : POWERID_INVALID;

            PowerHelper_DamageInit(tDamageOverTime, idPowerToUse, tBuff.idACDAffected);
            PowerHelper_DamageAdd(tDamageOverTime, ATTRIBUTE_HITPOINTS_CUR, -flDamageDealt, 0);
            tDamageOverTime.idACDMainActor = tDamageOverTime.idACDSource = tBuff.idACDSource;

            tDamageOverTime.eDeliveryMechanism = PDM_DOT;
            tDamageOverTime.snoAltDamageSourcePower = tBuff.snoDOTDamageSourcePower;
            tDamageOverTime.bCriticalHit = tBuff.bDOTCrit;
            tDamageOverTime.flProcScalar = tBuff.flProcScalar;
            if (tBuff.bAppliedDamage)
            {
                tDamageOverTime.bDontTriggerThreatListUpdate = TRUE;
            }

            PowerAPI_DoDamage(tDamageOverTime, FALSE, flDamageDone);

            tBuff.bAppliedDamage = TRUE;

            if (ACDIsPlayer(tBuff.idACDSource))
            {
                ActorCommonData * ptACDPlayer = ACDTryToGet(tBuff.idACDSource);
                if (ptACDPlayer)
                {
                    ActorCommonData * ptACDAffected = ACDTryToGet(tBuff.idACDAffected);
                    if (ptACDAffected)
                    {
                        sUpdateCombatStatus(*ptACDPlayer, *ptACDAffected);

                        uint32 uNextCombatTickCheck = uNow + uInterval + 1;
                        ptACDPlayer->AttributesSetInt(ATTRIBUTE_IN_COMBAT_NEXT_TICK_CHECK, uNextCombatTickCheck);
                    }
                }
            }

            ActorID idActor = (ActorID)ACDGetOwner(tBuff.idACDAffected);
            ActorTriggerAttributeUpdate(idActor);
        }

        tBuff.uLastDOTTick = uNow;
    }
}

// PowerAPI.cpp:12498
void PowerAPI_DealFrailtyDamage(
    ActorCommonData & tACD,
    const float flHP)
{
    uint32 uNow = GameGetServerTime();
    uint32 uNextFrailtyTick = tACD.AttributesGetInt(ATTRIBUTE_FRAILTY_NEXT_APPLICATION_TICK);

    if (uNextFrailtyTick < uNow)
    {
        float flHPMaxTotal = tACD.GetHitPointsMaxTotal();


        tACD.AttributesSetInt(ATTRIBUTE_FRAILTY_NEXT_APPLICATION_TICK, uNow + 60);

        const float HITPOINTS_MAX = FLT_MAX;
        const float fFrailtyDamage = HITPOINTS_MAX;

        float flDamageDone[7] = {};
        flDamageDone[0] = fFrailtyDamage;

        ACDID idACDAttacker = ACDID_INVALID;
        FastAttribKey tFrailtyKey = MakeAttribKey(ATTRIBUTE_NECROMANCER_CURSED, GlobalSNOGet(GLOBALSNO_POWERS_NECRO_FRAILTY_PASSIVE_MANAGER));
        int32 nOwnerPlayerIndex = tACD.AttributesGetInt(tFrailtyKey);
        if (nOwnerPlayerIndex != -1)
        {
            Player * ptPlayer = PlayerGetByPlayerIndex(nOwnerPlayerIndex);
            if (ptPlayer)
            {
                idACDAttacker = ptPlayer->idPlayerACD;
            }
        }

        PowerAPIDamage tDamage;
        PowerHelper_DamageInit(tDamage, POWERID_INVALID, tACD.id);
        PowerHelper_DamageAdd(tDamage, ATTRIBUTE_HITPOINTS_CUR, -fFrailtyDamage, 0);
        tDamage.idACDTarget = tACD.id;
        tDamage.idACDMainActor = idACDAttacker;
        tDamage.idACDSource = ACDID_INVALID;
        tDamage.bCriticalHit = FALSE;
        tDamage.eDeliveryMechanism = PDM_THORNS;
        tDamage.snoAltDamageSourcePower = SNO_NONE;

        tDamage.flProcScalar = 0.0f;
        tDamage.bProcessedShields = FALSE;
        tDamage.bIsStaggerDamage = FALSE;
        tDamage.bIgnoreDamageBonusAndReduction = TRUE;
        tDamage.bForceCriticalDeath = TRUE;
        tDamage.bPhysical = TRUE;
        PowerAPI_DoDamage(tDamage, FALSE, flDamageDone);

        Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(tACD.id));
        if (ptActor)
        {
            ActorTriggerAttributeUpdate(ptActor->id);
        }
    }
}

// PowerAPI.cpp:12423
void PowerAPI_DealStaggeredDamage(
    Actor * ptActor)
{
    ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);
    if (!ptACD || !ptActor->ptStaggerDamageInfo)
    {
        return;
    }

    StaggerDamageInfo * ptDamageInfo = ptActor->ptStaggerDamageInfo;
    int32 uStaggeredDamageTotalTicks = (int32)(ptDamageInfo->flStaggeredDamageDuration * 60.0f);
    int32 nNumberOfBuckets = ptDamageInfo->arStaggeredDamageBuckets.size();
    uint32 nTicksPerBucket = uStaggeredDamageTotalTicks / nNumberOfBuckets;


    uint32 uNow = GameGetServerTime();
    uint32 uTicksElapsed = uNow - ptDamageInfo->uStaggeredDamageLastTickCheck;
    ptDamageInfo->uStaggeredDamageLastTickCheck = uNow;
    ptDamageInfo->nStaggeredDamagePosition = (uTicksElapsed + ptDamageInfo->nStaggeredDamagePosition) % uStaggeredDamageTotalTicks;
    int32 nCurrentBucket = ptDamageInfo->nStaggeredDamagePosition / nTicksPerBucket;
    nCurrentBucket = Math::Pin(nCurrentBucket, 0, nNumberOfBuckets - 1);

    if (ptDamageInfo->nStaggeredDamagePreviousBucket >= nNumberOfBuckets)
    {
        ptDamageInfo->nStaggeredDamagePreviousBucket = 0;
    }

    float flDamage = 0.0f;
    while (ptDamageInfo->nStaggeredDamagePreviousBucket != nCurrentBucket)
    {

        flDamage += ptDamageInfo->arStaggeredDamageBuckets[ptDamageInfo->nStaggeredDamagePreviousBucket];
        ptDamageInfo->arStaggeredDamageBuckets[ptDamageInfo->nStaggeredDamagePreviousBucket] = 0.0f;


        ptDamageInfo->nStaggeredDamagePreviousBucket = (ptDamageInfo->nStaggeredDamagePreviousBucket + 1) % nNumberOfBuckets;
    }
    if (flDamage > EPSILON)
    {
        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_SLOW_KILL_BIT))
        {
            flDamage *= 0.01f;
        }

        float flDamageDone[7] = {};
        flDamageDone[0] = flDamage + flDamageDone[0];

        PowerAPIDamage tDamageOverTime;
        PowerHelper_DamageInit(tDamageOverTime, POWERID_INVALID, ptActor->idACD);
        PowerHelper_DamageAdd(tDamageOverTime, ATTRIBUTE_HITPOINTS_CUR, -flDamage, 0);
        tDamageOverTime.idACDMainActor = ptActor->idACD;
        tDamageOverTime.idACDSource = ACDID_INVALID;
        tDamageOverTime.eDeliveryMechanism = PDM_DOT;
        tDamageOverTime.snoAltDamageSourcePower = SNO_NONE;
        tDamageOverTime.bCriticalHit = FALSE;
        tDamageOverTime.flProcScalar = 0.0f;
        tDamageOverTime.bProcessedShields = FALSE;
        tDamageOverTime.bIsStaggerDamage = TRUE;
        PowerAPI_DoDamage(tDamageOverTime, FALSE, flDamageDone);

        ActorTriggerAttributeUpdate(ptActor->id);
    }
}

// PowerAPI.cpp:12566
uint32 PowerBuffGetLockoutTime(
    ActorCommonData & tACD,
    const PowerAPIBuffInstance * ptBuff)
{
    Attrib eAttrib = ATTRIBUTE_POWER_BUFF_0_LOCKOUT_TIME;
    switch (ptBuff->nBuffIndex)
    {
    case 1: eAttrib = ATTRIBUTE_POWER_BUFF_1_LOCKOUT_TIME; break;
    case 2: eAttrib = ATTRIBUTE_POWER_BUFF_2_LOCKOUT_TIME; break;
    case 3: eAttrib = ATTRIBUTE_POWER_BUFF_3_LOCKOUT_TIME; break;
    case 4: eAttrib = ATTRIBUTE_POWER_BUFF_4_LOCKOUT_TIME; break;
    case 5: eAttrib = ATTRIBUTE_POWER_BUFF_5_LOCKOUT_TIME; break;
    case 6: eAttrib = ATTRIBUTE_POWER_BUFF_6_LOCKOUT_TIME; break;
    case 7: eAttrib = ATTRIBUTE_POWER_BUFF_7_LOCKOUT_TIME; break;
    case 8: eAttrib = ATTRIBUTE_POWER_BUFF_8_LOCKOUT_TIME; break;
    case 9: eAttrib = ATTRIBUTE_POWER_BUFF_9_LOCKOUT_TIME; break;
    case 10: eAttrib = ATTRIBUTE_POWER_BUFF_10_LOCKOUT_TIME; break;
    case 11: eAttrib = ATTRIBUTE_POWER_BUFF_11_LOCKOUT_TIME; break;
    case 12: eAttrib = ATTRIBUTE_POWER_BUFF_12_LOCKOUT_TIME; break;
    case 13: eAttrib = ATTRIBUTE_POWER_BUFF_13_LOCKOUT_TIME; break;
    case 14: eAttrib = ATTRIBUTE_POWER_BUFF_14_LOCKOUT_TIME; break;
    case 15: eAttrib = ATTRIBUTE_POWER_BUFF_15_LOCKOUT_TIME; break;
    case 16: eAttrib = ATTRIBUTE_POWER_BUFF_16_LOCKOUT_TIME; break;
    case 17: eAttrib = ATTRIBUTE_POWER_BUFF_17_LOCKOUT_TIME; break;
    case 18: eAttrib = ATTRIBUTE_POWER_BUFF_18_LOCKOUT_TIME; break;
    case 19: eAttrib = ATTRIBUTE_POWER_BUFF_19_LOCKOUT_TIME; break;
    case 20: eAttrib = ATTRIBUTE_POWER_BUFF_20_LOCKOUT_TIME; break;
    case 21: eAttrib = ATTRIBUTE_POWER_BUFF_21_LOCKOUT_TIME; break;
    case 22: eAttrib = ATTRIBUTE_POWER_BUFF_22_LOCKOUT_TIME; break;
    case 23: eAttrib = ATTRIBUTE_POWER_BUFF_23_LOCKOUT_TIME; break;
    case 24: eAttrib = ATTRIBUTE_POWER_BUFF_24_LOCKOUT_TIME; break;
    case 25: eAttrib = ATTRIBUTE_POWER_BUFF_25_LOCKOUT_TIME; break;
    case 26: eAttrib = ATTRIBUTE_POWER_BUFF_26_LOCKOUT_TIME; break;
    case 27: eAttrib = ATTRIBUTE_POWER_BUFF_27_LOCKOUT_TIME; break;
    case 28: eAttrib = ATTRIBUTE_POWER_BUFF_28_LOCKOUT_TIME; break;
    case 29: eAttrib = ATTRIBUTE_POWER_BUFF_29_LOCKOUT_TIME; break;
    case 30: eAttrib = ATTRIBUTE_POWER_BUFF_30_LOCKOUT_TIME; break;
    case 31: eAttrib = ATTRIBUTE_POWER_BUFF_31_LOCKOUT_TIME; break;
    }

    return tACD.AttributesGetInt(MakeAttribKey(eAttrib, ptBuff->snoPower));
}

// PowerAPI.cpp:12614
void PowerBuffSetLockoutTime(
    ActorCommonData & tACD,
    const PowerAPIBuffInstance * ptBuff,
    uint32 dwLockoutTime)
{
    Attrib eAttrib = ATTRIBUTE_POWER_BUFF_0_LOCKOUT_TIME;
    switch (ptBuff->nBuffIndex)
    {
    case 1: eAttrib = ATTRIBUTE_POWER_BUFF_1_LOCKOUT_TIME; break;
    case 2: eAttrib = ATTRIBUTE_POWER_BUFF_2_LOCKOUT_TIME; break;
    case 3: eAttrib = ATTRIBUTE_POWER_BUFF_3_LOCKOUT_TIME; break;
    case 4: eAttrib = ATTRIBUTE_POWER_BUFF_4_LOCKOUT_TIME; break;
    case 5: eAttrib = ATTRIBUTE_POWER_BUFF_5_LOCKOUT_TIME; break;
    case 6: eAttrib = ATTRIBUTE_POWER_BUFF_6_LOCKOUT_TIME; break;
    case 7: eAttrib = ATTRIBUTE_POWER_BUFF_7_LOCKOUT_TIME; break;
    case 8: eAttrib = ATTRIBUTE_POWER_BUFF_8_LOCKOUT_TIME; break;
    case 9: eAttrib = ATTRIBUTE_POWER_BUFF_9_LOCKOUT_TIME; break;
    case 10: eAttrib = ATTRIBUTE_POWER_BUFF_10_LOCKOUT_TIME; break;
    case 11: eAttrib = ATTRIBUTE_POWER_BUFF_11_LOCKOUT_TIME; break;
    case 12: eAttrib = ATTRIBUTE_POWER_BUFF_12_LOCKOUT_TIME; break;
    case 13: eAttrib = ATTRIBUTE_POWER_BUFF_13_LOCKOUT_TIME; break;
    case 14: eAttrib = ATTRIBUTE_POWER_BUFF_14_LOCKOUT_TIME; break;
    case 15: eAttrib = ATTRIBUTE_POWER_BUFF_15_LOCKOUT_TIME; break;
    case 16: eAttrib = ATTRIBUTE_POWER_BUFF_16_LOCKOUT_TIME; break;
    case 17: eAttrib = ATTRIBUTE_POWER_BUFF_17_LOCKOUT_TIME; break;
    case 18: eAttrib = ATTRIBUTE_POWER_BUFF_18_LOCKOUT_TIME; break;
    case 19: eAttrib = ATTRIBUTE_POWER_BUFF_19_LOCKOUT_TIME; break;
    case 20: eAttrib = ATTRIBUTE_POWER_BUFF_20_LOCKOUT_TIME; break;
    case 21: eAttrib = ATTRIBUTE_POWER_BUFF_21_LOCKOUT_TIME; break;
    case 22: eAttrib = ATTRIBUTE_POWER_BUFF_22_LOCKOUT_TIME; break;
    case 23: eAttrib = ATTRIBUTE_POWER_BUFF_23_LOCKOUT_TIME; break;
    case 24: eAttrib = ATTRIBUTE_POWER_BUFF_24_LOCKOUT_TIME; break;
    case 25: eAttrib = ATTRIBUTE_POWER_BUFF_25_LOCKOUT_TIME; break;
    case 26: eAttrib = ATTRIBUTE_POWER_BUFF_26_LOCKOUT_TIME; break;
    case 27: eAttrib = ATTRIBUTE_POWER_BUFF_27_LOCKOUT_TIME; break;
    case 28: eAttrib = ATTRIBUTE_POWER_BUFF_28_LOCKOUT_TIME; break;
    case 29: eAttrib = ATTRIBUTE_POWER_BUFF_29_LOCKOUT_TIME; break;
    case 30: eAttrib = ATTRIBUTE_POWER_BUFF_30_LOCKOUT_TIME; break;
    case 31: eAttrib = ATTRIBUTE_POWER_BUFF_31_LOCKOUT_TIME; break;
    }

    tACD.AttributesSetInt(MakeAttribKey(eAttrib, ptBuff->snoPower), dwLockoutTime);
}

// PowerAPI.cpp:12663
void PowerAPI_SetNormalAttackReplacement(ACDID idACD, SNO snoPower)
{
    ActorCommonData & tACD = ACDGet(idACD);
    tACD.AttributesSetInt(ATTRIBUTE_NORMAL_ATTACK_REPLACEMENT_POWER_SNO, snoPower);
}

// PowerAPI.cpp:12673
PowerID PowerAPI_StartSpawnAnimPower(const ACDID idACD, uint32 dwAnimTag)
{
    // DWARF: tACD is a function-scope local; ACDGet inlined at 12677, before the if at 12678
    ActorCommonData & tACD = ACDGet(idACD);
    if (dwAnimTag != (uint32)-1)
    {
        uint32 dwAnimTagToUse = dwAnimTag;
        if (!AnimPlayerAnimExists(tACD, dwAnimTag))
        {
            dwAnimTagToUse = TAG_ACTOR_ANIMATION_GETTING_SUMMONED;
            if (dwAnimTag == TAG_ACTOR_ANIMATION_GETTING_SUMMONED || !AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_GETTING_SUMMONED))
                return POWER_NONE;
        }
        PowerStartData tPowerStartData;
        PowerStartDataInit(tPowerStartData);
        tPowerStartData.idACD = tACD.id;
        tPowerStartData.idACDAlternateSource = tACD.id;
        tPowerStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_SUMMONED);
        tPowerStartData.dwAnimTagToUse = dwAnimTagToUse;
        return PowerStartEx(tPowerStartData, -1, FALSE, FALSE);
    }
    return POWER_NONE;
}

// PowerAPI.cpp:12711
SNO PowerAPI_GetDamageSourcePowerSNO(const PowerAPIDamage * ptDamage, RuneType * peRuneType)
{
    SNO snoPower = SNO_NONE;
    RuneType eRune = -1;
    if (ptDamage && ptDamage->snoAltDamageSourcePower != SNO_NONE)
    {
        snoPower = ptDamage->snoAltDamageSourcePower;
    }
    else if (ptDamage)
    {
        Power * ptPower = PowerTryToGet(ptDamage->idPower);
        if (ptPower)
        {
            snoPower = ptPower->snoPower;
            eRune = PowerGetRuneType(ptPower);
        }
    }
    if (peRuneType)
        *peRuneType = eRune;
    return snoPower;
}

// PowerAPI.cpp:12744

static void sPowerAPI_SendBrokeCrowdControlFloatMessage(ACDID idACD, FloatingNumberType eType)
{
    if (ACDIsPlayer(idACD))
    {
        Player * ptPlayer = PlayerGetByACD(idACD);
        if (ptPlayer)
        {
            FloatingNumberMessage tMessage;
            tMessage.annTarget = ANNFromACD(idACD);
            tMessage.flDamage = 0.0f;
            tMessage.eType = eType;
            PlayerList listDestPlayers(PlayerListGetElementPool());
            if (!GameTypeIsPvP())
                listDestPlayers.AddTail_Tracked(ptPlayer);
            else
                PlayerListGetThatKnowACD(listDestPlayers, idACD, FALSE);
            MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));
        }
    }
}

// PowerAPI.cpp:12777
void PowerAPI_FreeFromRoot(const ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return;
    BOOL bFreed = FALSE;
    POS pos = ptACD->listBuffs.GetHeadPosition();
    while (pos && ptACD->listBuffs.Count() > 0 && ptACD->IsAlive())
    {
        BuffID idBuff = ptACD->listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && !ptBuff->bUndispellable)
        {
            if (FastAttribGroupIsValid(ptBuff->idFastAttrib) && FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_IS_ROOTED))
            {
                bFreed = TRUE;
                PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, 0, TRUE);
            }
            if (FastAttribGroupIsValid(ptBuff->idFastAttrib) && FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_IMMOBOLIZED))
            {
                bFreed = TRUE;
                PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, 0, TRUE);
            }
        }
    }
    if (bFreed)
        sPowerAPI_SendBrokeCrowdControlFloatMessage(idACD, FLOATING_NUMBER_TYPE_BROKE_ROOT);
}

// PowerAPI.cpp:12817
void PowerAPI_FreeFromSnare(const ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return;
    BOOL bFreed = FALSE;
    POS pos = ptACD->listBuffs.GetHeadPosition();
    while (pos && ptACD->listBuffs.Count() > 0 && ptACD->IsAlive())
    {
        BuffID idBuff = ptACD->listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && !ptBuff->bUndispellable && PowerAPI_BuffIsSnare(ptBuff))
        {
            bFreed = TRUE;
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, 0, TRUE);
        }
    }
    if (bFreed)
        sPowerAPI_SendBrokeCrowdControlFloatMessage(idACD, FLOATING_NUMBER_TYPE_BROKE_SNARE);
}

// PowerAPI.cpp:12930
void PowerAPI_FreeFromFear(const ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return;
    BOOL bFreed = ptACD->AttributesGetInt(ATTRIBUTE_FEARED) != 0;
    ptACD->AttributesSetInt(ATTRIBUTE_FEARED, 0);
    POS pos = ptACD->listBuffs.GetHeadPosition();
    while (pos && ptACD->listBuffs.Count() > 0 && ptACD->IsAlive())
    {
        BuffID idBuff = ptACD->listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && !ptBuff->bUndispellable && FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_FEARED))
        {
            bFreed = TRUE;
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, 0, TRUE);
        }
    }
    if (bFreed)
    {
        BrainStanceReset((ActorID)ACDGetOwner(idACD));
        sPowerAPI_SendBrokeCrowdControlFloatMessage(idACD, FLOATING_NUMBER_TYPE_BROKE_FEAR);
    }
}

// PowerAPI.cpp:12972
ACDID PowerAPI_GetRootSource(const ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos && ptACD->listBuffs.Count() > 0 && ptACD->IsAlive())
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted && FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_IS_ROOTED))
                return ptBuff->idACDSource;
        }
    }
    return ACDID_INVALID;
}

// PowerAPI.cpp:13002
static const int32 AURA_DURATION_SLOP_TICKS = 20;

// PowerAPI.cpp:13004
void PowerAPI_AuraSetupBuffStartData(PowerAPIBuffStartData & tBuffStartData, PowerID idPower)
{
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.bAura = TRUE;
    tBuffStartData.eStackType = 0;  // BUFF_STACKTYPE_NEVER
    tBuffStartData.nBuffIndex = 1;
    tBuffStartData.bHitsOnFirstTick = TRUE;
    tBuffStartData.bRemoveOnDeath = TRUE;
    tBuffStartData.dwDurationMin = tBuffStartData.dwRefreshRate + AURA_DURATION_SLOP_TICKS;
    tBuffStartData.dwDurationDelta = 0;
}

// PowerAPI.cpp:13025
static void sApplyAuraToMaster(PowerAPIBuffInstance * ptBuff, void * pData)
{
    if (pData)
    {
        PowerID idPower = ptBuff->idPower;
        ActorCommonData * ptACDFollower = PowerGetOwner(idPower);
        if (ptACDFollower)
        {
            ACDID idACDMaster = ACDGetPetOwner(ptACDFollower->id);
            if (idACDMaster != ACDID_INVALID)
            {
                ActorCommonData * ptACDMaster = ACDTryToGet(idACDMaster);
                if (ptACDMaster && ptACDMaster->IsAlive())
                {
                    WorldPlace wpFollower = ptACDFollower->GetPlace();
                    WorldPlace wpMaster = ptACDMaster->GetPlace();
                    if (wpMaster.idSWorld == wpFollower.idSWorld)
                    {
                        AuraCallback pCallback = (AuraCallback)pData;
                        pCallback(idPower, idACDMaster);
                    }
                }
            }
        }
    }
}

// PowerAPI.cpp:13058
static void sApplyAuraToMasterAndSelf(PowerAPIBuffInstance * ptBuff, void * pData)
{
    if (pData)
    {
        PowerID idPower = ptBuff->idPower;
        ActorCommonData * ptACDFollower = PowerGetOwner(idPower);
        if (ptACDFollower)
        {
            AuraCallback pCallback = (AuraCallback)pData;
            pCallback(idPower, ptACDFollower->id);
            sApplyAuraToMaster(ptBuff, pData);
        }
    }
}

// PowerAPI.cpp:13083
static void sApplyAuraToAreaOfEffect(PowerAPIBuffInstance * ptBuff, void * pData)
{
    if (pData)
    {
        PowerID idPower = ptBuff->idPower;
        XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
        float flRadius = PowerGetFormulaValue(idPower, TAG_POWER_AURA_AFFECTED_RADIUS);
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        NavPlace tNavPlace;
        PowerGetNavPlaceFromACD(ptACDOwner, tNavPlace);
        PowerAPIChooseTargets tChooseTargets;
        PowerHelper_ChooseTargetsInit(idPower, tChooseTargets);
        PowerHelper_ChooseTargetsInitGeometrySphere(tNavPlace.tPlace, flRadius, tChooseTargets.tGeometry);
        PowerAPI_DoChooseTargets(tChooseTargets, listTargets);
        AuraCallback pCallback = (AuraCallback)pData;
        POS pos = listTargets.GetHeadPosition();
        while (pos)
        {
            ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
            pCallback(idPower, idACDTarget);
        }
        sApplyAuraToMaster(ptBuff, pData);
    }
}

// PowerAPI.cpp:13127
void PowerAPI_AuraStart(PowerID idPower, AuraCallback pCallback, AuraType eAuraType, BOOL bRemoveOnDeath)
{
    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
        return;
    PowerAPIBuffStartData tBuffStartData(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.eStackType = 0;  // BUFF_STACKTYPE_NEVER
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.bHitsOnFirstTick = TRUE;
    tBuffStartData.bRemoveOnDeath = bRemoveOnDeath;
    switch (eAuraType)
    {
    case AURA_TYPE_MASTER_ONLY:
        tBuffStartData.pPeriodicSpellEffect = sApplyAuraToMaster;
        break;
    case AURA_TYPE_SELF_AND_MASTER:
        tBuffStartData.pPeriodicSpellEffect = sApplyAuraToMasterAndSelf;
        break;
    default:
        tBuffStartData.pPeriodicSpellEffect = sApplyAuraToAreaOfEffect;
        break;
    }
    tBuffStartData.pPeriodicSpellEffectData = (void *)pCallback;
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);
}

// PowerAPI.cpp:13170
void PowerAPI_RecomputeHitpointHealTarget(ActorCommonData * ptACD)
{
    float flTotalHealAmount = 0.0f;
    uint32 uNow = GameGetServerTime();
    if (ptACD)
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted)
            {
                float flRegenPerSec = FastAttribGetValueFloat(ptBuff->idFastAttrib, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_HEALTH_GLOBE);
                if (flRegenPerSec > EPSILON)
                {
                    uint32 uEndTick = ptBuff->dwStartTick + ptBuff->dwDuration;
                    uint32 uDurationInTicks = uEndTick > uNow ? uEndTick - uNow : 0;
                    float flDurationInSecs = uDurationInTicks * OO_TICKS_PER_SECOND;
                    float flHealAmount = flRegenPerSec * flDurationInSecs;
                    flTotalHealAmount += flHealAmount;
                }
            }
        }
        float flHPMax = ptACD->GetHitPointsMaxTotal();
        float flHPCurr = ptACD->GetHitPointsCurrent();
        ptACD->AttributesSetFloat(ATTRIBUTE_HITPOINTS_HEALED_TARGET, Math::Min(flHPMax, flTotalHealAmount + flHPCurr));
    }
}

// PowerAPI.cpp:13378
void sBurrowInRemoved(PowerAPIBuffInstance * ptBuff, BuffRemovalReason, void *)
{
    if (ptBuff)
    {
        ActorCommonData * ptACDTarget = ACDTryToGet(ptBuff->idACDAffected);
        if (ptACDTarget && ptACDTarget->IsAlive())
        {
            ptACDTarget->EnableCollision(TRUE);
            WeaponClass eOldWeaponClass = ptBuff->nBuffData;
            if (eOldWeaponClass != -1)
                ptACDTarget->AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, eOldWeaponClass);
        }
    }
}

// PowerAPI.cpp:13403
void PowerAPI_DoBurrowIn(ActorCommonData * ptACDTarget, BOOL bInvisibleDuring, WeaponClass eBurrowWeaponClass, int32 nAppearanceParameter, BOOL bAlphaAttachments)
{
    if (!ptACDTarget)
        return;
    SNO snoBurrowBuff = GlobalSNOGet(GLOBALSNO_POWERS_BURROW_START_BUFF);
    BuffID idBurrowBuff = PowerAPI_BuffFindOnACD(snoBurrowBuff, *ptACDTarget, ACDID_INVALID);
    if (idBurrowBuff != BUFFID_INVALID)
        return;
    PowerAPIBuffStartData tBuffStartData(POWER_NONE);
    tBuffStartData.snoPower = snoBurrowBuff;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.eStackType = 0;
    tBuffStartData.nMaxStackCount = 1;
    tBuffStartData.nAppearanceParameter = nAppearanceParameter;
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_BURROWED, 1);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, 1);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, 1);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_AI_IN_SPECIAL_STATE, 1);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DODGE_CHANCE_BONUS, 1.0f);
    if (bInvisibleDuring)
    {
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, 1);
    }
    if (bAlphaAttachments)
    {
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_ALPHA_ATTACHMENTS, 1);
    }
    idBurrowBuff = PowerAPI_BuffAddToACD(tBuffStartData, *ptACDTarget);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBurrowBuff);
    if (ptBuff)
    {
        ptACDTarget->EnableCollision(FALSE);
        ptBuff->pRemovalEffect = sBurrowInRemoved;
        if (eBurrowWeaponClass != -1)
        {
            WeaponClass eOldWeaponClass = ptACDTarget->AttributesGetInt(ATTRIBUTE_CURRENT_WEAPONCLASS);
            if (eOldWeaponClass == -1)
            {
                ScopedActorDefinitionLockR pdefActor(ptACDTarget->snoActor);
                if (pdefActor)
                {
                    eOldWeaponClass = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_ANIM_WEAPON_CLASS);
                }
            }
            if (eOldWeaponClass == -1)
            {
                eOldWeaponClass = 0;
            }
            ptBuff->nBuffData = eOldWeaponClass;
            ptACDTarget->AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, eBurrowWeaponClass);
        }
    }
}

// PowerAPI.cpp:13491
void PowerAPI_DoBurrowOut(ActorCommonData * ptACDTarget, BOOL bSetUsedSpecialSpawn)
{
    if (ptACDTarget)
    {
        SNO snoBurrowBuff = GlobalSNOGet(GLOBALSNO_POWERS_BURROW_START_BUFF);
        BuffID idBorrowBuff = PowerAPI_BuffFindOnACD(snoBurrowBuff, *ptACDTarget, ACDID_INVALID);
        if (idBorrowBuff != BUFFID_INVALID)
        {
            PowerAPI_BuffFlagForDeletion(idBorrowBuff, FALSE, TRUE, 0, TRUE);
            if (bSetUsedSpecialSpawn)
                ptACDTarget->AttributesSetInt(ATTRIBUTE_AI_USED_SPAWN_ANIM, TRUE);
        }
    }
}

// PowerAPI.cpp:13544
BOOL PowerAPI_BuffRequestCancel(const Player & tPlayerRequester, const SNO snoPower, const int32 nBuffIndex)
{
    if (snoPower == SNO_NONE)
        return FALSE;
    if (nBuffIndex < 0 || nBuffIndex >= 32)
        return FALSE;

    int32 nTag = 0;
    ScopedPowerDefinitionLockR pdefPower(snoPower);
    switch (nBuffIndex)
    {
        case 0: nTag = TAG_POWER_BUFF_0_PLAYER_CAN_CANCEL; break;
        case 1: nTag = TAG_POWER_BUFF_1_PLAYER_CAN_CANCEL; break;
        case 2: nTag = TAG_POWER_BUFF_2_PLAYER_CAN_CANCEL; break;
        case 3: nTag = TAG_POWER_BUFF_3_PLAYER_CAN_CANCEL; break;
        case 4: nTag = TAG_POWER_BUFF_4_PLAYER_CAN_CANCEL; break;
        case 5: nTag = TAG_POWER_BUFF_5_PLAYER_CAN_CANCEL; break;
        case 6: nTag = TAG_POWER_BUFF_6_PLAYER_CAN_CANCEL; break;
        case 7: nTag = TAG_POWER_BUFF_7_PLAYER_CAN_CANCEL; break;
        case 8: nTag = TAG_POWER_BUFF_8_PLAYER_CAN_CANCEL; break;
        case 9: nTag = TAG_POWER_BUFF_9_PLAYER_CAN_CANCEL; break;
        case 10: nTag = TAG_POWER_BUFF_10_PLAYER_CAN_CANCEL; break;
        case 11: nTag = TAG_POWER_BUFF_11_PLAYER_CAN_CANCEL; break;
        case 12: nTag = TAG_POWER_BUFF_12_PLAYER_CAN_CANCEL; break;
        case 13: nTag = TAG_POWER_BUFF_13_PLAYER_CAN_CANCEL; break;
        case 14: nTag = TAG_POWER_BUFF_14_PLAYER_CAN_CANCEL; break;
        case 15: nTag = TAG_POWER_BUFF_15_PLAYER_CAN_CANCEL; break;
        case 16: nTag = TAG_POWER_BUFF_16_PLAYER_CAN_CANCEL; break;
        case 17: nTag = TAG_POWER_BUFF_17_PLAYER_CAN_CANCEL; break;
        case 18: nTag = TAG_POWER_BUFF_18_PLAYER_CAN_CANCEL; break;
        case 19: nTag = TAG_POWER_BUFF_19_PLAYER_CAN_CANCEL; break;
        case 20: nTag = TAG_POWER_BUFF_20_PLAYER_CAN_CANCEL; break;
        case 21: nTag = TAG_POWER_BUFF_21_PLAYER_CAN_CANCEL; break;
        case 22: nTag = TAG_POWER_BUFF_22_PLAYER_CAN_CANCEL; break;
        case 23: nTag = TAG_POWER_BUFF_23_PLAYER_CAN_CANCEL; break;
        case 24: nTag = TAG_POWER_BUFF_24_PLAYER_CAN_CANCEL; break;
        case 25: nTag = TAG_POWER_BUFF_25_PLAYER_CAN_CANCEL; break;
        case 26: nTag = TAG_POWER_BUFF_26_PLAYER_CAN_CANCEL; break;
        case 27: nTag = TAG_POWER_BUFF_27_PLAYER_CAN_CANCEL; break;
        case 28: nTag = TAG_POWER_BUFF_28_PLAYER_CAN_CANCEL; break;
        case 29: nTag = TAG_POWER_BUFF_29_PLAYER_CAN_CANCEL; break;
        case 30: nTag = TAG_POWER_BUFF_30_PLAYER_CAN_CANCEL; break;
        case 31: nTag = TAG_POWER_BUFF_31_PLAYER_CAN_CANCEL; break;
    }

    BOOL bCanCancel = PowerGeneralTagGetBool(AutoPowerDefinitionR(pdefPower), nTag);
    if (!bCanCancel)
        return FALSE;

    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayerRequester.idPlayerACD);
    if (ptACDPlayer)
    {
        BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, nBuffIndex, *ptACDPlayer, ACDID_INVALID, FALSE);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff)
        {
            PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_PLAYER_CANCELLED, TRUE);
            return TRUE;
        }
    }
    return FALSE;
}

// PowerAPI.cpp:13724
float PowerGetClassBonusPercent(AttribGroupID idFastAttrib, PlayerClass eClass);  // X/Common/Powers/PowersCommon.cpp:1219

void PowerAPI_UpdateSummonedActorAttributes(ActorCommonData & tACDSummoned, ActorCommonData & tACDOwner, Power * ptPower)
{
    if (!ACDIsPlayer(tACDOwner.id))
    {
        return;
    }

    AttribGroupID idFastAttribToUse = tACDOwner.idFastAttrib;

    FastAttribGroup * ptFastAttribToUse = tACDOwner.ptFastAttribGroup;

    if (ptPower)
    {
        idFastAttribToUse = ptPower->idFastAttrib;
        ptFastAttribToUse = FastAttribGroupGet(idFastAttribToUse);
    }

    tACDSummoned.AttributesSetInt(ATTRIBUTE_PRIMARY_DAMAGE_ATTRIBUTE, FastAttribGetValueInt(ptFastAttribToUse, ATTRIBUTE_PRIMARY_DAMAGE_ATTRIBUTE));

    tACDSummoned.AttributesSetFloat(ATTRIBUTE_STRENGTH, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_STRENGTH_TOTAL));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_DEXTERITY, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_DEXTERITY_TOTAL));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_INTELLIGENCE, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_INTELLIGENCE_TOTAL));

    tACDSummoned.AttributesSetFloat(ATTRIBUTE_CRIT_PERCENT_BASE, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_CRIT_PERCENT_BASE));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_CRIT_PERCENT_BONUS_CAPPED, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_CRIT_PERCENT_BONUS_CAPPED));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_CRIT_PERCENT_BONUS_UNCAPPED, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_CRIT_PERCENT_BONUS_UNCAPPED));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_CRIT_PERCENT_CAP, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_CRIT_PERCENT_CAP));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_CRIT_DAMAGE_PERCENT, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_CRIT_DAMAGE_PERCENT));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_CRIT_DAMAGE_PERCENT_UNCAPPED, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_CRIT_DAMAGE_PERCENT_UNCAPPED));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_CRIT_DAMAGE_CAP, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_CRIT_DAMAGE_CAP));

    tACDSummoned.AttributesSetFloat(ATTRIBUTE_DODGE_CHANCE_BONUS, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_DODGE_CHANCE_BONUS));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_DODGE_CHANCE_BONUS_MELEE, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_DODGE_CHANCE_BONUS_MELEE));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_DODGE_CHANCE_BONUS_RANGED, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_DODGE_CHANCE_BONUS_RANGED));

    tACDSummoned.AttributesSetFloat(ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_ELITES, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_ELITES));

    for (int32 i = 0; i < NUM_DAMAGE_TYPES; i++)
    {
        tACDSummoned.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE, i), FastAttribGetValueFloat(ptFastAttribToUse, MakeAttribKey(ATTRIBUTE_RESISTANCE, i)));
        tACDSummoned.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE_PERCENT, i), FastAttribGetValueFloat(ptFastAttribToUse, MakeAttribKey(ATTRIBUTE_RESISTANCE_PERCENT, i)));
        tACDSummoned.AttributesSetFloat(ATTRIBUTE_RESIST_ALL, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_RESIST_ALL));
        tACDSummoned.AttributesSetFloat(ATTRIBUTE_RESIST_PERCENT_ALL, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_RESIST_PERCENT_ALL));

        tACDSummoned.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_AMPLIFYDAMAGE_TYPE_PERCENT, i), FastAttribGetValueFloat(ptFastAttribToUse, MakeAttribKey(ATTRIBUTE_AMPLIFYDAMAGE_TYPE_PERCENT, i)));
        tACDSummoned.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, i), FastAttribGetValueFloat(ptFastAttribToUse, MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, i)));
    }

    for (int32 i = 0; i < NUM_MONSTER_TYPE; i++)
    {
        tACDSummoned.AttributesSetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_MONSTER_TYPE, i), FastAttribGetValueFloat(ptFastAttribToUse, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_MONSTER_TYPE, i)));
    }

    tACDSummoned.AttributesSetFloat(ATTRIBUTE_ARMOR, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_ARMOR));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_ARMOR_BONUS_PERCENT, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_ARMOR_BONUS_PERCENT));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_ARMOR_ITEM_TOTAL, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_ARMOR_ITEM_TOTAL));

    tACDSummoned.AttributesSetFloat(ATTRIBUTE_VITALITY, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_VITALITY_TOTAL));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_HITPOINTS_FACTOR_VITALITY, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_HITPOINTS_FACTOR_VITALITY));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_HITPOINTS_MAX_BONUS, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_HITPOINTS_MAX_BONUS));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_HITPOINTS_MAX_PERCENT_BONUS, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_HITPOINTS_MAX_PERCENT_BONUS));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_HITPOINTS_MAX_PERCENT_BONUS_ITEM, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_HITPOINTS_MAX_PERCENT_BONUS_ITEM));

    float flBaseHPRegen = FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND);
    float flBonusHPRegen = FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_BONUS);
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_BONUS, flBaseHPRegen + flBonusHPRegen);
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_HITPOINTS_REGEN_BONUS_PERCENT, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_HITPOINTS_REGEN_BONUS_PERCENT));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_HITPOINTS_REGEN_REDUCTION_PERCENT, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_HITPOINTS_REGEN_REDUCTION_PERCENT));

    tACDSummoned.AttributesSetFloat(ATTRIBUTE_HITPOINTS_ON_HIT, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_HITPOINTS_ON_HIT));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_HITPOINTS_ON_KILL_TOTAL, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_HITPOINTS_ON_KILL_TOTAL));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_STEAL_HEALTH_PERCENT, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_STEAL_HEALTH_PERCENT));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_HEALTH_GLOBE_BONUS_HEALTH, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_HEALTH_GLOBE_BONUS_HEALTH));

    tACDSummoned.AttributesSetFloat(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_MELEE, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_MELEE));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_RANGED, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_RANGED));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_ELITES, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_ELITES));
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_CROWD_CONTROL_REDUCTION, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_CROWD_CONTROL_REDUCTION));

    PlayerClass ePlayerClass = PlayerGetPlayerClass(&tACDOwner);
    float flClassBonusPercent = PowerGetClassBonusPercent(idFastAttribToUse, ePlayerClass);
    float flDamagePercentFromSkills = FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS);
    tACDSummoned.AttributesSetFloat(ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS, flClassBonusPercent + flDamagePercentFromSkills);

    tACDSummoned.AttributesSetFloat(ATTRIBUTE_DAMAGE_DONE_REDUCTION_PERCENT, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_DAMAGE_DONE_REDUCTION_PERCENT));

    tACDSummoned.AttributesSetFloat(ATTRIBUTE_SPLASH_DAMAGE_EFFECT_PERCENT, FastAttribGetValueFloat(ptFastAttribToUse, ATTRIBUTE_SPLASH_DAMAGE_EFFECT_PERCENT));
}

void PowerAPI_DoFirstHitEffects(ACDID idACDMainActor, ACDID idACDTarget, Power * ptPower)
{
    if (ptPower && !ptPower->bDidFirstHitResourceGains)
    {
        ptPower->bDidFirstHitResourceGains = TRUE;
        sApplyOnFirstHitAttemptBuffs(idACDMainActor, idACDTarget, ptPower->id, ptPower->snoPower);
        if (ACDIsPlayer(idACDMainActor))
        {
            float flResourceGain = 0.0f;
            PlayerClass ePlayerClass = PlayerGetPlayerClass(idACDMainActor);
            if (ePlayerClass == PLAYER_CLASS_BARBARIAN)
                flResourceGain = PowerGetFormulaValue(ptPower->id, TAG_POWER_RESOURCE_GAINED_ON_FIRST_HIT);
            else if (ptPower->snoPower == GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT) && ePlayerClass == PLAYER_CLASS_CRUSADER)
                flResourceGain = PowerGetFormulaValue(ptPower->id, TAG_POWER_SCRIPT_FORMULA_1);
            else if (ptPower->snoPower == GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_RANGED_PROJECTILE) && ePlayerClass == PLAYER_CLASS_DEMONHUNTER)
                flResourceGain = PowerGetFormulaValue(ptPower->id, TAG_POWER_SCRIPT_FORMULA_2);
            if (flResourceGain > EPSILON)
            {
                ActorCommonData * ptACDMainActor = ACDTryToGet(idACDMainActor);
                if (ptACDMainActor)
                {
                    ResourceType eType = PowerGetResourceType(*ptACDMainActor, AutoPowerDefinitionR(ptPower->snoPower));
                    ptACDMainActor->AddResource(eType, flResourceGain, TRUE);
                }
            }
        }
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        ActorCommonData * ptACDAttacker = ACDTryToGet(idACDMainActor);
        if (ptACDTarget && ptACDAttacker)
            sApplyThornsOnFirstHit(*ptACDTarget, *ptACDAttacker, ptPower);
    }
}


#include "../../Common/CombatLog.h"
#line 2060
static void sApplyResistance(
    PowerID idPower,
    const ActorCommonData & tACDTarget,
    const DamageType eDamageType,
    float & flDamageDone)
{
    float flDamageTakenPercent = 1.0f;
    ACDID idACDAttacker = ACDID_INVALID;
    if (idPower != POWERID_INVALID) {
#line 2071
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        if (ptACDOwner)
            idACDAttacker = ptACDOwner->id;
#line 2077
        int32 nAttackerLevel = PowerAttributesGetInt(idPower, ATTRIBUTE_LEVEL);
        if (GameTypeIsPvP()) {
#line 2081
            int32 nPVPLevelEqualizerLevel = PowerAttributesGetInt(idPower, ATTRIBUTE_PVP_LEVEL_EQUALIZER_LEVEL);
            if (nPVPLevelEqualizerLevel > 0)
                nAttackerLevel = nPVPLevelEqualizerLevel;
        }
#line 2088
        float flAttackerPenetration = PowerAttributesGetFloat(idPower, MakeAttribKey(ATTRIBUTE_RESISTANCE_PENETRATION_TOTAL, eDamageType));
#line 2090
        float flTargetResistance = tACDTarget.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE_TOTAL, eDamageType));
        float flResistanceTotal = flTargetResistance - flAttackerPenetration;
#line 2093
        float flPercentMitigation = PowerComputeResistanceDamageMitigation(flResistanceTotal, nAttackerLevel);
        flDamageTakenPercent = 1.0f - flPercentMitigation;
    }
#line 2098
    SNO snoPower = SNO_NONE;
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
        snoPower = ptPower->snoPower;
    CombatLog::LogEvent(CombatLog::LOG_EVENT_RESISTANCE_APPLIED, idACDAttacker,
                        tACDTarget.id, snoPower, flDamageTakenPercent, flDamageDone, eDamageType);
    flDamageDone *= flDamageTakenPercent;
}
#line 7716

#line 2165
static void sApplyMiscDamageReduction(
    const PowerID idPower,
    const ActorCommonData & tACDTarget,
    const PayloadDeliveryMechanism eDeliveryMechanism,
    float & flDamageDone,
    float & flDamageReductionTurnsIntoHealTotal,
    const DamageType eDamageType)
{
    if (idPower != POWERID_INVALID) {
        if (eDeliveryMechanism == PDM_MELEE) {
#line 2177
            float flMeleeReductionPercent = tACDTarget.AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_MELEE);
            if (flMeleeReductionPercent > EPSILON)
                flDamageDone *= 1.0f - flMeleeReductionPercent;
        } else if (eDeliveryMechanism == PDM_RANGED) {
#line 2186
            float flRangedReductionPercent = tACDTarget.AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_RANGED);
            if (flRangedReductionPercent > EPSILON)
                flDamageDone *= 1.0f - flRangedReductionPercent;
        }
        if (eDeliveryMechanism == PDM_MELEE || eDeliveryMechanism == PDM_RANGED ||
            eDeliveryMechanism == PDM_MAGIC || eDeliveryMechanism == PDM_DOT) {
#line 2198
            float flReductionPercentTurnsIntoHeal = tACDTarget.AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_TURNS_INTO_HEAL);
            if (flReductionPercentTurnsIntoHeal > EPSILON) {
                flDamageReductionTurnsIntoHealTotal += flDamageDone - flDamageDone * (1.0f - flReductionPercentTurnsIntoHeal);
                flDamageDone *= 1.0f - flReductionPercentTurnsIntoHeal;
            }
        }
#line 2206
        Power * ptPower = PowerGet(idPower);
        if (ptPower) {
#line 2209
            ActorCommonData * ptACDSource = ACDTryToGet(ptPower->idACD);
            if (ptACDSource && ptACDSource->IsElite()) {
#line 2214
                float flReductionFromElites = tACDTarget.AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_ELITES);
                if (flReductionFromElites > EPSILON)
                    flDamageDone *= 1.0f - flReductionFromElites;
            }
        }
#line 2223
        float flDamageReductionPercentFromType = tACDTarget.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_TYPE, eDamageType));
        if (flDamageReductionPercentFromType > EPSILON)
            flDamageDone *= 1.0f - flDamageReductionPercentFromType;
    }
}
#line 7766

#line 3961
static BOOL sCheckForCrit(
    PowerID idPower,
    BOOL bCanCrit,
    float flPowerAdditionalChanceToCriticalHit,
    float flCritCap,
    const ActorCommonData & tACDTarget,
    const ActorCommonData * ptACDMainActor)
{
    if (idPower != POWERID_INVALID && PowerContactTagGetBool(idPower, TAG_POWER_DOESNT_CRIT))
        return FALSE;
#line 3975
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower && (ptPower->dwFlags & (1 << PF_WAS_REFLECTED)))
        return FALSE;
    if (tACDTarget.AttributesGetInt(ATTRIBUTE_IGNORES_CRITICAL_HITS) || !bCanCrit)
        return FALSE;
    if (sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_ALL_HITS_CRIT_BIT))
        return TRUE;
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_ALL_HITS_CRIT))
        return TRUE;
    if (flCritCap < EPSILON)
        return FALSE;
#line 4007
    float flCritChance = flPowerAdditionalChanceToCriticalHit;
    flCritChance += PowerComputeCritChance(ptACDMainActor, FALSE);
    flCritChance += tACDTarget.AttributesGetFloat(ATTRIBUTE_BONUS_CHANCE_TO_BE_CRIT_HIT);
    if (ptACDMainActor) {
        flCritChance += tACDTarget.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_BONUS_CHANCE_TO_BE_CRIT_HIT_BY_ACTOR, ptACDMainActor->id));
#line 4017
        ACDID idACDOwner = ACDGetControllingActor(ptACDMainActor->id);
        if (idACDOwner != ptACDMainActor->id)
            flCritChance += tACDTarget.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_BONUS_CHANCE_TO_BE_CRIT_HIT_BY_ACTOR, idACDOwner));
    }
#line 4024
    float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
#line 4034
    SNO snoPower = SNO_NONE;
    if (ptPower)
        snoPower = ptPower->snoPower;
    CombatLog::LogEvent(CombatLog::LOG_EVENT_CRITICAL_HIT,
                        ptACDMainActor ? ptACDMainActor->id : ACDID_INVALID,
                        tACDTarget.id, snoPower, flCritChance, flRoll, 0);
    return flRoll <= flCritChance;
}
#line 7813

#line 2233
static void sApplyMiscDamageBonuses(
    const PowerID idPower,
    const ActorCommonData & tACDTarget,
    float & flDamageDone,
    const DamageType eDamageType,
    const PayloadDeliveryMechanism eDeliveryMechanism,
    BOOL bUseHighestElementalDamageBonus)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
        return;
    if (tACDTarget.IsMonster()) {
#line 2250
        MonsterType eMonsterType = MonsterGetMonsterType(tACDTarget.tSNOName);
        float flDamageBonusVsMonsterType = FastAttribGetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_MONSTER_TYPE, eMonsterType));
        if (flDamageBonusVsMonsterType > EPSILON)
            flDamageDone *= 1.0f + flDamageBonusVsMonsterType;
    }
    if (tACDTarget.IsElite()) {
#line 2260
        float flDamageVsEliteBonus = FastAttribGetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_DAMAGE_PERCENT_BONUS_VS_ELITES);
        if (flDamageVsEliteBonus > EPSILON)
            flDamageDone *= 1.0f + flDamageVsEliteBonus;
    }
#line 2270
    float flDamageDealtTypeBonus = 0.0f;
    DamageType ePowerDamageType = PowerGetDamageType(ptPower);
    if (bUseHighestElementalDamageBonus) {
#line 2274
        for (int32 nDamageType = DAMAGE_TYPE_PHYSICAL; nDamageType < NUM_DAMAGE_TYPES; ++nDamageType) {
#line 2276
            float flDamageDealtTypeBonusCur = FastAttribGetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, nDamageType));
            flDamageDealtTypeBonus = Math::Max(flDamageDealtTypeBonus, flDamageDealtTypeBonusCur);
        }
    } else if (ePowerDamageType != DAMAGE_TYPE_NONE) {
        flDamageDealtTypeBonus = FastAttribGetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, ePowerDamageType));
    } else if (eDeliveryMechanism != PDM_THORNS) {
        flDamageDealtTypeBonus = FastAttribGetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, eDamageType));
    }
    if (flDamageDealtTypeBonus > EPSILON)
        flDamageDone *= 1.0f + flDamageDealtTypeBonus;
#line 2303
    ActorCommonData * ptACDPet = ACDTryToGet(ptPower->idACD);
    if (ptACDPet) {
#line 2306
        SNO snoSummon = FastAttribGetValueInt(ptACDPet->idFastAttrib, ATTRIBUTE_SUMMONED_BY_SNO);
        ActorCommonData * ptACDSummonner = ACDTryToGet(ACDID(FastAttribGetValueInt(ptACDPet->idFastAttrib, ATTRIBUTE_SUMMONED_BY_ACDID)));
        if (snoSummon != INVALID_SNO && ptACDSummonner) {
#line 2310
            float flPowerDamageBonus = FastAttribGetValueFloat(ptACDSummonner->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_DAMAGE_PERCENT_BONUS, snoSummon));
            if (flPowerDamageBonus > EPSILON)
                flDamageDone *= 1.0f + flPowerDamageBonus;
        }
    }
}
#line 7871

#line 257
static void sUpdateAuras(
    ActorCommonData & tACD)
{
    POS pos = tACD.listBuffs.GetHeadPosition();
    PowerAPIBuffInstance * ptWinningBuff = NULL;
    while (pos) {
#line 265
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
        if (!ptBuff->bIsAura)
            continue;
        if (ptBuff->bDeleted)
            continue;
        if (!ptWinningBuff) {
            ptWinningBuff = ptBuff;
            continue;
        }
        if (ptWinningBuff->idACDSource == tACD.id && ptBuff->idACDSource != tACD.id)
            continue;
        if (ptBuff->nBuffLevel > ptWinningBuff->nBuffLevel) {
            ptWinningBuff = ptBuff;
            continue;
        }
        if (ptBuff->nBuffLevel < ptWinningBuff->nBuffLevel)
            continue;
        if (ptBuff->nAuraPriority > ptWinningBuff->nAuraPriority || ptBuff->nAuraPriority == -1)
            ptWinningBuff = ptBuff;
    }
    pos = tACD.listBuffs.GetHeadPosition();
    while (pos) {
#line 306
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = sBuffGet(idBuff);
        if (!ptBuff->bIsAura)
            continue;
        if (!ptBuff->bDeleted && ptBuff != ptWinningBuff) {
#line 320
            Power * ptPower = PowerTryToGet(ptBuff->idPower);
#line 322
            RuneType eRuneType = PowerGetRuneType(ptPower);
            Attrib eAttrib = PowerRuneTypeAndBuffIndexToAttribute(ptBuff->snoPower, eRuneType, ptBuff->nBuffIndex);
            FastAttribKey tKey = MakeAttribKey(eAttrib, ptBuff->snoPower);
            if (FastAttribGetValueInt(ptBuff->idFastAttrib, tKey)) {
                FastAttribSetValueInt(ptBuff->idFastAttrib, tKey, 0);
                FastAttribUnmerge(tACD.idFastAttrib, ptBuff->idFastAttrib);
                tACD.AttributesMergeAttributeGroup(ptBuff->idFastAttrib);
            }
        }
    }
    if (ptWinningBuff) {
#line 338
        Power * ptPower = PowerTryToGet(ptWinningBuff->idPower);
        RuneType eRuneType = PowerGetRuneType(ptPower);
        Attrib eAttrib = PowerRuneTypeAndBuffIndexToAttribute(ptWinningBuff->snoPower, eRuneType, ptWinningBuff->nBuffIndex);
        if (eAttrib != ATTRIBUTE_NONE) {
#line 343
            FastAttribKey tKey = MakeAttribKey(eAttrib, ptWinningBuff->snoPower);
            if (!FastAttribGetValueInt(ptWinningBuff->idFastAttrib, tKey)) {
                FastAttribSetValueInt(ptWinningBuff->idFastAttrib, tKey, 1);
                FastAttribUnmerge(tACD.idFastAttrib, ptWinningBuff->idFastAttrib);
                tACD.AttributesMergeAttributeGroup(ptWinningBuff->idFastAttrib);
            }
        }
    }
}
#line 7940

#line 12851
void PowerAPI_FreeFromStun(
    const ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return;
#line 12860
    BOOL bFreed = FALSE;
    BOOL bFreedBlind = FALSE;
    BOOL bFreedFreeze = FALSE;

    if (ptACD->AttributesGetInt(ATTRIBUTE_STUNNED) ||
        ptACD->AttributesGetInt(ATTRIBUTE_FROZEN) ||
        ptACD->AttributesGetInt(ATTRIBUTE_BLIND) ||
        ptACD->AttributesGetInt(ATTRIBUTE_FORCE_GRIPPED))
        bFreed = TRUE;
#line 12872
    ptACD->AttributesSetInt(ATTRIBUTE_STUNNED, 0);
    ptACD->AttributesSetInt(ATTRIBUTE_FROZEN, 0);
    ptACD->AttributesSetInt(ATTRIBUTE_BLIND, 0);
    ptACD->AttributesSetInt(ATTRIBUTE_FORCE_GRIPPED, 0);

    POS pos = ptACD->listBuffs.GetHeadPosition();
    while (pos && ptACD->listBuffs.Count() > 0 && ptACD->IsAlive())
    {
        BuffID idBuff = ptACD->listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && !ptBuff->bUndispellable)
        {
            if (FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_STUNNED))
            {
                PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
                bFreed = TRUE;
            }
            else if (FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_FORCE_GRIPPED))
            {
                PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
                bFreed = TRUE;
            }
            else if (FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_FROZEN))
            {
                bFreed = bFreedFreeze = TRUE;
                PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
            }
#line 12900
            else if (FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_BLIND))
            {
                bFreed = bFreedBlind = TRUE;
                PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
            }
        }
    }
#line 12909
    if (bFreed)
    {
        BrainStanceReset((ActorID)ACDGetOwner(idACD));

        if (bFreedFreeze)
        {
            sPowerAPI_SendBrokeCrowdControlFloatMessage(idACD, FLOATING_NUMBER_TYPE_BROKE_FREEZE);
        }
        else if (bFreedBlind)
        {
            sPowerAPI_SendBrokeCrowdControlFloatMessage(idACD, FLOATING_NUMBER_TYPE_BROKE_BLIND);
        }
        else
        {
            sPowerAPI_SendBrokeCrowdControlFloatMessage(idACD, FLOATING_NUMBER_TYPE_BROKE_STUN);
        }
    }
}

#include "../Actors/ActorPowers.h"

float PowerGetContinualResourceCost(PowerID idPower);  // Powers.cpp:1952; no header declaration in DWARF.

#line 7353
BOOL PowerAPI_ChanneledSpellDoContinualCosts(
    PowerID idPower,
    float fResourceCostModifier)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
        if (ptACD && ptACD->IsAlive())
        {
#line 7364
            ActorID idActor = (ActorID)ACDGetOwner(ptPower->idACD);
            if (ActorPowersGetPrimary(idActor) == idPower)
            {
#line 7368
                const Actor * ptActor = ActorGet((ActorID)ACDGetOwner(ptACD->id));
                const Player * ptPlayer = PlayerGetByACD(ptPower->idACD);

                PowerAPICosts tCostInfo;
                PowerHelper_CostsInit(idPower, tCostInfo);
                PowerID idLocPower = tCostInfo.idPower;
                ACDID idACDMainActor = tCostInfo.idACD;

                Power * ptLocPower = PowerGet(idLocPower);
                SNO snoPower = ptLocPower->snoPower;
                ScopedPowerDefinitionLockR pdefPower(snoPower);

                ActorCommonData & tACDMainActor = ACDGet(idACDMainActor);
                ResourceType eResourceType = PowerGetResourceType(tACDMainActor, AutoPowerDefinitionR(snoPower));

                float flResourceCost = PowerGetContinualResourceCost(idLocPower) * fResourceCostModifier;

                BOOL bSkipChargingResources = FALSE;
                if (ptACD->AttributesGetInt(ATTRIBUTE_FREE_CAST_ALL))
                    bSkipChargingResources = TRUE;
#line 7391
                if (ActorFlagIsSet(ptActor->id, ACTOR_GOD_MODE_BIT))
                    bSkipChargingResources = TRUE;
#line 7397
                if (flResourceCost > EPSILON)
                {
                    float flChannelCostOverride = tACDMainActor.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_CHANNEL_COST_OVERRIDE, pdefPower->tHeader.snoHandle));
                    if (flChannelCostOverride < FLT_MAX)
                    {
                        if (eResourceType == ptACD->GetSecondaryResourceType())
                            flResourceCost = flChannelCostOverride;
#line 7406
                        else if (eResourceType == ptACD->GetPrimaryResourceType())
                            flResourceCost = flChannelCostOverride;
                    }
#line 7413
                    float flCostReductionPercent = PowerGetCostReductionPercent(tACDMainActor, eResourceType, PowerGetDamageType(ptLocPower), snoPower);
                    flResourceCost *= flCostReductionPercent;
#line 7417
                    float flCostReduction = tACDMainActor.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_COST_REDUCTION_TOTAL, eResourceType) + tACDMainActor.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_RESOURCE_REDUCTION, snoPower));
                    float flPowerCostReductionCoefficient = PowerGetFormulaValue(idLocPower, TAG_POWER_COST_RESOURCE_REDUCTION_COEFFICIENT);
                    flResourceCost -= flCostReduction * flPowerCostReductionCoefficient;
                }
                PowerAPI_BuffApplyOnCostEffects(idACDMainActor, ptLocPower->id, PowerGetSNO(tCostInfo.idPower), flResourceCost, eResourceType, 0.0f, !bSkipChargingResources, 0.0f);
                if (!bSkipChargingResources && flResourceCost > EPSILON)
                {
#line 7427
                    BOOL bIgnoreMinimumResourceCost = PowerIsChannelled(tACDMainActor, AutoPowerDefinitionR(snoPower)) || PowerShouldUseClientWalking(tACDMainActor, AutoPowerDefinitionR(snoPower));

                    BOOL bResourceSpent = tACDMainActor.SpendResource(flResourceCost, bIgnoreMinimumResourceCost, eResourceType, ptLocPower, TRUE);
                    if (!bResourceSpent)
                    {
                        ptLocPower->dwFlags |= 1 << PF_FAILED;
                        return FALSE;
                    }
                }
                return TRUE;
            }
        }
    }
    return FALSE;
}

#include "../../Common/WorldCommonCollision.h"
#include "../../Common/ActorCommonCollision.h"
#include "../../Common/Nav/NavZone.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Definitions/MonsterDefinition.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/MonsterTags.h"

// These function declarations are recorded at their original .cpp definitions in the DWARF.
BOOL WorldCollSweptPoint(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Vector2D & wpStart, const Vector2D & wpDest, const WorldCollParams & tWorldCollParams, uint32 uCollTestFlags, uint32 uACDCollNarrowFlags, ACDID idACDIgnore, WorldCollResult & tResult);  // WorldCommonCollision.cpp:62
uint32 NavMeshGetStaticFlagsForPassability(PassabilityType ePassability);  // NavigationMesh.cpp:3180
uint32 NavMeshGetDynamicFlagsForPassability(PassabilityType ePassability);  // NavigationMesh.cpp:3187
PassabilityType NavMeshDeterminePassabilityType(SNO snoActor);  // NavigationMesh.cpp:3194
BOOL NavMeshPlaceCanEnterForSpawning(const WorldPlace & placeSpawn, const PassabilityType ePassabilityType);  // NavigationMesh.cpp:3320
BOOL DominoRaycastAgainstWorld(const WorldPlace & placeStart, const WorldPlace & placeEnd, Vector3D & vCollidePoint, Vector3D & vCollideNormal, float & flUnitTravel, uint16 uExcludeMask);  // DominoServer.cpp:317
ACDID ProjectileAdd(const ProjectileData & tProjectileData);  // Projectile.cpp:268
void ActorPowersSetSpellEffect(ActorID idActor, SpellEffect eSpellEffectToRun);  // ActorPowers.cpp:446

#line 4923
static SNO sGetProjectileEffect(
    PowerID idPower,
    ActorCommonData & tACDProjectile)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
        return SNO_NONE;
#line 4933
    if (!PowerIsBasicAttack(AutoPowerDefinitionR(ptPower->snoPower)))
        return SNO_NONE;
#line 4938
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);

    DamageType eWinningDamageType = DAMAGE_TYPE_NONE;

    float flWinningDmg = 0.0f;

    for (int32 i = DAMAGE_TYPE_FIRE; i < NUM_DAMAGE_TYPES; ++i)
    {
        float flDamage = tPayload.arCombatDamage[i].flMin + tPayload.arCombatDamage[i].flDelta;
        if (flDamage > flWinningDmg)
        {
            flWinningDmg = flDamage;
            eWinningDamageType = i;
        }
    }
#line 4955
    SNO snoEffectGroup = SNO_NONE;

    ScopedActorDefinitionLockR pdefActor(tACDProjectile.snoActor);
    switch (eWinningDamageType)
    {
    case DAMAGE_TYPE_FIRE: snoEffectGroup = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_EFFECT_PROJECTILE_FIRE_EFFECTGROUP); break;
    case DAMAGE_TYPE_LIGHTNING: snoEffectGroup = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_EFFECT_PROJECTILE_LIGHTNING_EFFECTGROUP); break;
    case DAMAGE_TYPE_COLD: snoEffectGroup = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_EFFECT_PROJECTILE_ICE_EFFECTGROUP); break;
    case DAMAGE_TYPE_POISON: snoEffectGroup = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_EFFECT_PROJECTILE_POISON_EFFECTGROUP); break;
    case DAMAGE_TYPE_ARCANE: snoEffectGroup = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_EFFECT_PROJECTILE_ARCANE_EFFECTGROUP); break;
    case DAMAGE_TYPE_HOLY: snoEffectGroup = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_EFFECT_PROJECTILE_HOLY_EFFECTGROUP); break;
    }
    return snoEffectGroup;
}

#line 4991
ACDID PowerAPI_DoProjectile(
    PowerAPICreateProjectile & tProjectileInfo)
{
    Power * ptPower = PowerGet(tProjectileInfo.idPower);
#line 4998
    BOOL bDontPerformMarbleRoll = tProjectileInfo.tProjectileData.dwFlags & (1 << PROJECTILE_DONT_PERFORM_MARBLE_ROLL_TO_CASTER_BIT);
    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (ptACD && !bDontPerformMarbleRoll)
    {
        WorldPlace placeACD = ptACD->GetPlace();
        NavCellHandle hNavCellCenter;
        NavCellHandleFromWorldPlace(placeACD, hNavCellCenter);

        PassabilityType ePassabilityType = NavMeshDeterminePassabilityType(tProjectileInfo.tProjectileData.tRenderingData.snoPR);

        WorldCollParams tCollParams;
        tCollParams.uOneRequiredStaticFlags = NavMeshGetStaticFlagsForPassability(ePassabilityType);
        tCollParams.uObjectNoOverlapFlags = NavMeshGetDynamicFlagsForPassability(ePassabilityType) | (1 << COLL_DYN_ACTOR);
        tCollParams.eTeam = ptACD->GetTeam();
        tCollParams.uSpecialFlags |= 1 << COLL_PARAMS_FROM_POWER;

        WorldCollResult tResult;
        WorldCollSweptPoint(
            placeACD.idSWorld,
            hNavCellCenter,
            (const Vector2D &)placeACD.wp,
            (const Vector2D &)tProjectileInfo.tProjectileData.tAnimationData.placeStart.wp,
            tCollParams,
            0,
            0,
            ptACD->id,
            tResult);

        tProjectileInfo.tProjectileData.tAnimationData.placeStart.wp.x = tResult.wpEnd.x;
        tProjectileInfo.tProjectileData.tAnimationData.placeStart.wp.y = tResult.wpEnd.y;

        Vector3D vIntersect;
        Vector3D vNormal;
        float flUnitTraveled;
        WorldPlace placeRayStart = placeACD;
        placeRayStart.wp.z = tProjectileInfo.tProjectileData.tAnimationData.placeStart.wp.z;

        BOOL bCollided = DominoRaycastAgainstWorld(placeRayStart, tProjectileInfo.tProjectileData.tAnimationData.placeStart, vIntersect, vNormal, flUnitTraveled, (1 << COLLISION_CATEGORY_WATER) | (1 << COLLISION_CATEGORY_FENCE) | (1 << COLLISION_CATEGORY_SMOKE));
        if (bCollided)
        {
            Vector3D vDelta = vIntersect - placeRayStart.wp;
            if (vDelta.MagnitudeSquared() >= 0.25f)
            {
                float flLength = vDelta.Normalize();

                flLength -= 0.25f;
                tProjectileInfo.tProjectileData.tAnimationData.placeStart.wp = placeRayStart.wp + vDelta * flLength;
            }
            else
            {
                tProjectileInfo.tProjectileData.tAnimationData.placeStart.wp = placeRayStart.wp;
            }
        }
#line 5053
        if (!NavMeshPlaceCanEnterForSpawning(tProjectileInfo.tProjectileData.tAnimationData.placeStart, ePassabilityType))
        {
#line 5057
            tProjectileInfo.tProjectileData.tAnimationData.placeStart.wp.x = placeACD.wp.x;
            tProjectileInfo.tProjectileData.tAnimationData.placeStart.wp.y = placeACD.wp.y;
        }
    }
    if (tProjectileInfo.tProjectileData.tRenderingData.snoPR == SNO_NONE)
        return ACDID_INVALID;
#line 5069
    ACDID idACDProjectile = ProjectileAdd(tProjectileInfo.tProjectileData);
    if (idACDProjectile == ACDID_INVALID)
        return ACDID_INVALID;
#line 5076
    ActorCommonData & tACDProjectile = ACDGet(idACDProjectile);

    ActorID idActorProjectile = (ActorID)tACDProjectile.idOwner;
    ActorPowersSetOwnerPower(idActorProjectile, tProjectileInfo.idPower);
    ActorPowersSetSpellEffect(idActorProjectile, tProjectileInfo.tProjectileData.eSpellEffect);

    SSceneID idSScene = ACDGetCurrentSSceneID(tACDProjectile);
    if (idSScene == SSCENEID_INVALID)
    {
#line 5086
        ImpactData tImpactData(idACDProjectile);
        ProjectileHadCollision(tImpactData);
    }
#line 5091
    if (ptACD && ptACD->id)
    {
        PowerAPI_BuffApplyOnProjectileCreateEffects(ptACD->id, idACDProjectile);
    }
#line 5097
    tACDProjectile.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_A, ptPower->snoPower), FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_A, ptPower->snoPower)));
    tACDProjectile.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_B, ptPower->snoPower), FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_B, ptPower->snoPower)));
    tACDProjectile.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_C, ptPower->snoPower), FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_C, ptPower->snoPower)));
    tACDProjectile.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_D, ptPower->snoPower), FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_D, ptPower->snoPower)));
    tACDProjectile.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_E, ptPower->snoPower), FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_E, ptPower->snoPower)));

    SNO snoProjectileEffect = sGetProjectileEffect(tProjectileInfo.idPower, tACDProjectile);
    if (snoProjectileEffect != SNO_NONE)
    {
        tACDProjectile.AttributesSetInt(ATTRIBUTE_PROJECTILE_EFFECT_SNO, snoProjectileEffect);
    }
#line 5110
    if (ptACD)
    {
        SNO snoMonster = ActorGetMonsterSNO(ptACD->snoActor);
        if (snoMonster != SNO_NONE)
        {
            ScopedMonsterDefinitionLockR pdefMonster(snoMonster);
            if (TagMapGetInt(pdefMonster->hTagMap, TAG_MONSTER_FORCE_REMOTE_PLAYER_EFFECTS))
            {
                tACDProjectile.AttributesSetInt(ATTRIBUTE_FORCE_REMOTE_FX, TRUE);
            }
        }
    }
    return idACDProjectile;
}

// DWARF records only CPP homes for these declarations; no header declaration was found.
BOOL ItemHasLabelMask(const GBID gbidItem, const uint32 * pdwItemLabelMask); // Items.cpp:2886
BOOL PowerHasAffectedACD(PowerID idPower, const PowerDefinition * pdefPower, const ACDID idACDTarget, const ACDID idACDSource); // Powers.cpp:4694

// --- PowerAPI_DoFindAllPossibleTargets and its file-local helpers (PowerAPI.cpp:5199-6305 in the DWARF).
// Not laid out on the DWARF lines; each definition names its DWARF line. Debug drawing guarded by
// SGAMEGLOBALS_SHOW_PAYLOADS is compiled out in this build: only the SGameGlobalsGet() call remains
// in the original (e.g. PowerAPI.cpp:5315:21, 5565:21, 5723:13), so those blocks are written empty.
#include "../../Common/Actors/ActorCollision.h"
#include "../AI/ActorAI.h"
#include "../../Sigma/Math/Intersect.h"
void ACDsCylinderIntersectCylinder(const WorldPlace & wpCenter, const WorldDistance wdRadius, const WorldDistance wdHeight, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);  // X/Common/Actors/ActorCollision.cpp:383
void ACDsIntersectCapsule(SWorldID idSWorld, const Capsule & capsule, XPooledDataIDList & listACDs, BOOL bForPower);  // X/Common/Actors/ActorCollision.cpp:518
void ACDsIntersectLineSeg(const WorldPlace & placeStart, const WorldPlace & placeEnd, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);  // X/Common/Actors/ActorCollision.cpp:457
void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);  // X/Common/Actors/ActorCollision.cpp:317
Angle ACDGetYaw(ACDID idACD);  // X/Common/ActorCommonData.cpp:2997

// PowerAPI.cpp:5200
static void sPowerAPICullTargetsNotInLOS(
    const WorldPlace & tPlace,
    XPooledDataIDList & listTargets,
    const PowerAPIChooseTargets & tTargetingInfo)
{
    if (tTargetingInfo.dwFlags & (1 << CTF_DONT_PERFORM_LOS_CHECK))
        return;

    NavCellHandle hNavCellStart;
    NavCellHandleFromWorldPlace(tPlace, hNavCellStart);

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        POS posToDeleteAt = pos;
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);

        BOOL bLOS = NavMeshCheckLOSToACD(hNavCellStart, tPlace, idACDTarget, tTargetingInfo.ePassabilityType, (tTargetingInfo.dwFlags & (1 << CTF_PERFORM_PROJECTILE_LOS_CHECK)) != 0);

        if (!bLOS)
        {
            listTargets.RemoveAt(posToDeleteAt);
        }
    }
}

// PowerAPI.cpp:5233
static void sPowerAPICullTargetsDotProduct(
    const WorldPlace & tPlace,
    const Angle & aYaw,
    XPooledDataIDList & listTargets,
    const BOOL & bPositiveDotProduct)
{
    Vector3D wvForward = cgwvXAxis;
    Vector3D::Rotate(cgwvZAxis, aYaw, wvForward);

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        POS posCurrent = pos;
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);

        Vector3D wvToTarget = ACDGet(idACDTarget).GetPlace().wp - tPlace.wp;
        WorldDistance wdDot = Vector3D::DotProduct(wvToTarget, wvForward);
        if (bPositiveDotProduct)
        {
            if (wdDot < 0.0f)
            {
                listTargets.RemoveAt(posCurrent);
            }
        }
        else
        {
            if (wdDot >= 0.0f)
            {
                listTargets.RemoveAt(posCurrent);
            }
        }
    }
}

// PowerAPI.cpp:5277
static void sPowerAPICullTargetsNotInFront(
    const WorldPlace & tPlace,
    const Angle & aYaw,
    XPooledDataIDList & listTargets)
{
    sPowerAPICullTargetsDotProduct(tPlace, aYaw, listTargets, TRUE);
}

// PowerAPI.cpp:5287
static void sPowerAPICullTargetsNotInBack(
    const WorldPlace & tPlace,
    const Angle & aYaw,
    XPooledDataIDList & listTargets)
{
    sPowerAPICullTargetsDotProduct(tPlace, aYaw, listTargets, FALSE);
}

// PowerAPI.cpp:5298
static void sPowerAPIFindSingleTarget(
    const ChooseTargetGeometrySingleTarget & tSingleTarget,
    XPooledDataIDList & listTargets,
    const PowerAPIChooseTargets & tTargetingInfo)
{
    switch (tSingleTarget.tTarget.eType)
    {
    case TT_NONE:
        break;
    case TT_ACTOR:
        listTargets.AddTail_Tracked(tSingleTarget.tTarget.idACDTarget);
        break;
    case TT_POINT:
        {
            uint32 dwSphereTreeFlags = (tTargetingInfo.dwFlags & (1 << CTF_TARGET_ITEMS_ONLY)) ? (1 << ACTOR_SPHERE_TREE_ITEMS_IN_WORLD) : ACTOR_SPHERE_TREE_FLAGS_NO_ITEMS;
            ACDsCylinderIntersectSphere(tSingleTarget.tTarget.tPlace, tSingleTarget.wdSearchRadius, ACDID_INVALID, listTargets, dwSphereTreeFlags);

            if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHOW_PAYLOADS))
            {
            }
        }
        break;
    default:
        DisplayInternalError(NULL, "", 5326, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    }

    sPowerAPICullTargetsNotInLOS(tSingleTarget.placeSource, listTargets, tTargetingInfo);
}

// PowerAPI.cpp:5456
static void sPowerAPIFindMeleeTargets(
    const PowerID idPower,
    const ACDID idACDSource,
    const Target & tTarget,
    XPooledDataIDList & listTargets,
    const PowerAPIChooseTargets & tTargetingInfo)
{
    SNO snoPower = PowerGetSNO(idPower);
    Power * ptPower = PowerTryToGet(idPower);
    Target tTargetSource = tTarget;

    ActorCommonData * ptACDSource = ACDTryToGet(idACDSource);
    if (!ptACDSource)
        return;

    WorldPlace placeSource = ptACDSource->GetPlace();

    switch (tTargetSource.eType)
    {
    case TT_NONE:
        break;
    case TT_ACTOR:
    case TT_ACTOR_WITH_FALLBACK_POINT:
        {
            ActorCommonData * ptACDTargetSource = ACDTryToGet(tTargetSource.GetACD());
            if (!ptACDTargetSource || ptACDTargetSource->tLocation.eActorLocation != AL_WORLD)
            {
                Vector3D wvFacing = cgwvXAxis;
                Angle aFacing = ptACDSource->GetOrientation().ToYaw();
                Vector3D::Rotate(cgwvZAxis, aFacing, wvFacing);
                WorldPlace tPlace = ptACDSource->GetPlace();
                tPlace.wp += wvFacing;
                tTargetSource.InitializeFromPlace(tPlace);
            }
            else
            {
                float flMeleeRange = (float)tTargetingInfo.tGeometry.tMelee.nMeleeRange;
                float flMeleeReach = PowerAPI_ComputeMeleeReach(idPower, tTargetSource.GetACD());
                BOOL bIgnoreRange = ptPower && (ptPower->dwFlags & (1 << PF_IGNORE_RANGE));
                BOOL bCloseEnough = PowerIsCloseEnough(idACDSource, AutoPowerDefinitionR(snoPower), &tTargetSource, flMeleeRange, flMeleeReach, NULL, bIgnoreRange);

                BOOL bLOS = TRUE;
                if (!(tTargetingInfo.dwFlags & (1 << CTF_DONT_PERFORM_LOS_CHECK)))
                {
                    NavCellHandle hNavCellStart;
                    NavCellHandleFromWorldPlace(placeSource, hNavCellStart);
                    bLOS = NavMeshCheckLOSToACD(hNavCellStart, placeSource, tTargetSource.idACDTarget, tTargetingInfo.ePassabilityType, FALSE);
                }

                if (bCloseEnough && bLOS)
                {
                    listTargets.AddTail_Tracked(tTargetSource.idACDTarget);
                    return;
                }
            }
        }
        // fall through: the original continues into the TT_POINT case (no branch between +0x36c and +0x370)
    case TT_POINT:
        {
            float flMeleeRange = (float)tTargetingInfo.tGeometry.tMelee.nMeleeRange;

            WorldPlace placeTarget;
            tTargetSource.GetPlace(placeTarget);

            Vector3D wvFacing = cgwvXAxis;
            Angle aFacing = ptACDSource->GetOrientation().ToYaw();
            Vector3D::Rotate(cgwvZAxis, aFacing, wvFacing);

            if (placeTarget.idSWorld != placeSource.idSWorld)
            {
                placeTarget = placeSource;
                placeTarget.wp += wvFacing;
            }

            Vector3D wvSourceToTarget;
            WorldPlaceGetVectorBetween(placeSource, placeTarget, wvSourceToTarget);
            wvSourceToTarget.Normalize();

            ActorCommonData & tACDSource = ACDGet(idACDSource);
            WorldDistance wdSourceRadius = tACDSource.GetRadius();
            WorldDistance wdAttackRadius = (flMeleeRange > 0.0f) ? flMeleeRange : PowerGetRange(idPower, FALSE);
            WorldDistance wdRange = wdSourceRadius + wdAttackRadius;

            wvSourceToTarget *= wdRange;

            WorldPlace placeCenter;
            WorldPlaceOffset(placeSource, wvSourceToTarget, placeCenter);

            Power * ptScopedPower = PowerGet(idPower);
            const TargetCriteria & tTargetCriteria = ptScopedPower->tTargetCriteria;

            XPooledDataIDList listTargetsInSphere(MemoryPoolsGetDATAIDListPool());
            uint32 dwSphereTreeFlags = (tTargetingInfo.dwFlags & (1 << CTF_TARGET_ITEMS_ONLY)) ? (1 << ACTOR_SPHERE_TREE_ITEMS_IN_WORLD) : ACTOR_SPHERE_TREE_FLAGS_NO_ITEMS;
            ACDsCylinderIntersectSphere(placeCenter, wdRange, idACDSource, listTargetsInSphere, dwSphereTreeFlags);

            if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHOW_PAYLOADS))
            {
            }

            ACDID idACDClosest = ACDID_INVALID;
            float flCrossProductMagnitudeSquaredToClosestTarget = MAX_WORLDDISTANCE;
            NavCellHandle hNavCellStart = { SSCENEID_INVALID, 0 };
            BOOL fNavCellInitialized = FALSE;
            POS pos = listTargetsInSphere.GetHeadPosition();
            while (pos)
            {
                ACDID idACDTarget = (ACDID)listTargetsInSphere.GetNext(pos);
                if (!PowerCanTarget(idPower, idACDTarget, ACDID_INVALID))
                {
                    continue;
                }

                Target tScopedTarget;
                tScopedTarget.InitializeFromACD(idACDTarget, FALSE, FALSE);

                float flMeleeReach = PowerAPI_ComputeMeleeReach(idPower, tScopedTarget.GetACD());
                BOOL bIgnoreRange = ptScopedPower && (ptScopedPower->dwFlags & (1 << PF_IGNORE_RANGE));
                BOOL bCloseEnough = PowerIsCloseEnough(idACDSource, AutoPowerDefinitionR(snoPower), &tScopedTarget, wdAttackRadius, flMeleeReach, NULL, bIgnoreRange);
                if (!bCloseEnough)
                {
                    continue;
                }

                if (!(tTargetingInfo.dwFlags & (1 << CTF_DONT_PERFORM_LOS_CHECK)))
                {
                    if (!fNavCellInitialized)
                    {
                        NavCellHandleFromWorldPlace(placeSource, hNavCellStart);
                        fNavCellInitialized = TRUE;
                    }
                    BOOL bLOS = NavMeshCheckLOSToACD(hNavCellStart, placeSource, idACDTarget, PASSABILITY_PROJECTILE, FALSE);
                    if (!bLOS)
                    {
                        continue;
                    }
                }

                if ((tTargetCriteria.eLifeStatus == TCLS_NOT_DEAD) && ActorHasBeenKilled((ActorID)ACDGetOwner(idACDTarget)))
                {
                    continue;
                }

                WorldPlace wpTarget;
                tScopedTarget.GetPlace(wpTarget);

                Vector3D wvToTestTarget;
                WorldPlaceGetVectorBetween(placeSource, wpTarget, wvToTestTarget);
                wvToTestTarget.z = 0.0f;

                float flCrossProductMagnitudeSquared = Vector3D::CrossProduct(wvFacing, wvToTestTarget).MagnitudeSquared();
                if (flCrossProductMagnitudeSquared < flCrossProductMagnitudeSquaredToClosestTarget)
                {
                    flCrossProductMagnitudeSquaredToClosestTarget = flCrossProductMagnitudeSquared;
                    idACDClosest = idACDTarget;
                }
            }

            if (idACDClosest != ACDID_INVALID)
            {
                listTargets.AddTail_Tracked(idACDClosest);
            }
        }
        break;
    default:
        DisplayInternalError(NULL, "", 5642, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    }
}

// PowerAPI.cpp:5649
static void sPowerAPIFindLineTargets(
    const WorldPlace & placeStart,
    const WorldPlace & placeEnd,
    XPooledDataIDList & listTargets,
    uint32 dwSphereTreeFlags)
{
    NavCellHandle hNavCell;
    NavCellHandleFromWorldPlace(placeStart, hNavCell);

    WorldCollParams tCollParams;
    tCollParams.uOneRequiredStaticFlags = (1 << CELL_STATIC_ALLOW_PROJECTILE);

    WorldCollResult tResult;
    WorldCollSweptPoint(placeStart.idSWorld, hNavCell, placeStart.wp.As2D(), placeEnd.wp.As2D(), tCollParams, 0, 0, ACDID_INVALID, tResult);

    WorldPlace placeCollided;
    placeCollided.idSWorld = placeEnd.idSWorld;
    placeCollided.wp = tResult.wpEnd;
    ACDsIntersectLineSeg(placeStart, placeCollided, listTargets, dwSphereTreeFlags);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHOW_PAYLOADS))
    {
    }
}

// PowerAPI.cpp:5689
static void sPowerAPIFindSphereTargets(
    const WorldPlace & placeCenter,
    const WorldDistance wdRadius,
    XPooledDataIDList & listTargets,
    const PowerAPIChooseTargets & tTargetingInfo)
{
    uint32 dwSphereTreeFlags = (tTargetingInfo.dwFlags & (1 << CTF_TARGET_ITEMS_ONLY)) ? (1 << ACTOR_SPHERE_TREE_ITEMS_IN_WORLD) : ACTOR_SPHERE_TREE_FLAGS_NO_ITEMS;
    ACDsCylinderIntersectSphere(placeCenter, wdRadius, ACDID_INVALID, listTargets, dwSphereTreeFlags);

    sPowerAPICullTargetsNotInLOS(placeCenter, listTargets, tTargetingInfo);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHOW_PAYLOADS))
    {
    }
}

// PowerAPI.cpp:5709
static void sPowerAPIFindFrontTargets(
    const ACDID idACDPowerOwner,
    const WorldPlace & placeCenter,
    const WorldDistance wdRadius,
    XPooledDataIDList & listTargets,
    const PowerAPIChooseTargets & tTargetingInfo)
{
    uint32 dwSphereTreeFlags = (tTargetingInfo.dwFlags & (1 << CTF_TARGET_ITEMS_ONLY)) ? (1 << ACTOR_SPHERE_TREE_ITEMS_IN_WORLD) : ACTOR_SPHERE_TREE_FLAGS_NO_ITEMS;
    ACDsCylinderIntersectSphere(placeCenter, wdRadius, ACDID_INVALID, listTargets, dwSphereTreeFlags);
    if (idACDPowerOwner != ACDID_INVALID)
    {
        Angle aYaw = ACDGetYaw(idACDPowerOwner);
        sPowerAPICullTargetsNotInFront(placeCenter, ACDGetYaw(idACDPowerOwner), listTargets);

        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHOW_PAYLOADS))
        {
            Vector3D vSphereDir(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
        }
    }
    else
    {
        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHOW_PAYLOADS))
        {
        }
    }

    sPowerAPICullTargetsNotInLOS(placeCenter, listTargets, tTargetingInfo);
}

// PowerAPI.cpp:5742
static void sPowerAPIFindBackTargets(
    const ACDID idACDPowerOwner,
    const WorldPlace & placeCenter,
    const WorldDistance wdRadius,
    XPooledDataIDList & listTargets,
    const PowerAPIChooseTargets & tTargetingInfo)
{
    uint32 dwSphereTreeFlags = (tTargetingInfo.dwFlags & (1 << CTF_TARGET_ITEMS_ONLY)) ? (1 << ACTOR_SPHERE_TREE_ITEMS_IN_WORLD) : ACTOR_SPHERE_TREE_FLAGS_NO_ITEMS;
    ACDsCylinderIntersectSphere(placeCenter, wdRadius, ACDID_INVALID, listTargets, dwSphereTreeFlags);
    if (idACDPowerOwner != ACDID_INVALID)
    {
        Angle aYaw = ACDGetYaw(idACDPowerOwner);
        sPowerAPICullTargetsNotInBack(placeCenter, aYaw, listTargets);

        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHOW_PAYLOADS))
        {
            Vector3D vSphereDir(Math::Cos(aYaw + HALF_CIRCLE), Math::Sin(aYaw + HALF_CIRCLE), 0.0f);
        }
    }
    else
    {
        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHOW_PAYLOADS))
        {
        }
    }

    sPowerAPICullTargetsNotInLOS(placeCenter, listTargets, tTargetingInfo);
}

// PowerAPI.cpp:5818
static void sPowerAPIFindPieTargets(
    const WorldPlace & placeCenter,
    const WorldDistance wdRadius,
    const WorldDistance wdHeight,
    const Target & tTarget,
    XPooledDataIDList & listTargets,
    int32 nDegrees,
    Vector3D wvPieVector,
    const PowerAPIChooseTargets & tTargetingInfo)
{
    uint32 dwSphereTreeFlags = (tTargetingInfo.dwFlags & (1 << CTF_TARGET_ITEMS_ONLY)) ? (1 << ACTOR_SPHERE_TREE_ITEMS_IN_WORLD) : ACTOR_SPHERE_TREE_FLAGS_NO_ITEMS;
    ACDsCylinderIntersectCylinder(placeCenter, wdRadius, wdHeight, ACDID_INVALID, listTargets, dwSphereTreeFlags);

    Vector3D wvVectorToUse;
    if (Math::Abs(wvPieVector.x) > EPSILON || Math::Abs(wvPieVector.y) > EPSILON || Math::Abs(wvPieVector.z) > EPSILON)
    {
        wvVectorToUse = wvPieVector;
    }
    else
    {
        if (tTarget.IsInvalid())
        {
            listTargets.RemoveAll();
            return;
        }
        if (!tTarget.IsInWorld())
        {
            listTargets.RemoveAll();
            return;
        }
        WorldPlace placeDest;
        tTarget.GetPlace(placeDest);
        if (placeDest.idSWorld != placeCenter.idSWorld)
        {
            listTargets.RemoveAll();
            return;
        }
        WorldPlaceGetVectorBetween(placeCenter, placeDest, wvVectorToUse);
        wvVectorToUse.z = 0.0f;
        if (wvVectorToUse.MagnitudeSquared() < EPSILON)
        {
            listTargets.RemoveAll();
            return;
        }
    }

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        POS posLast = pos;
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        if (!PowerAPI_TargetIsWithinArc(placeCenter, wvVectorToUse, nDegrees * (PI / 180.0f) * 0.5f, idACDTarget))
        {
            listTargets.RemoveAt(posLast);
        }
    }

    sPowerAPICullTargetsNotInLOS(placeCenter, listTargets, tTargetingInfo);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHOW_PAYLOADS))
    {
    }
}

// PowerAPI.cpp:5901
static void sPowerAPIFindCylinderTargets(
    const ChooseTargetGeometryCylinder & tCylinder,
    XPooledDataIDList & listTargets,
    const PowerAPIChooseTargets & tTargetingInfo)
{
    uint32 dwSphereTreeFlags = (tTargetingInfo.dwFlags & (1 << CTF_TARGET_ITEMS_ONLY)) ? (1 << ACTOR_SPHERE_TREE_ITEMS_IN_WORLD) : ACTOR_SPHERE_TREE_FLAGS_NO_ITEMS;
    ACDsCylinderIntersectCylinder(tCylinder.place0, tCylinder.flRadius, tCylinder.flHeight, ACDID_INVALID, listTargets, dwSphereTreeFlags);

    sPowerAPICullTargetsNotInLOS(tCylinder.place0, listTargets, tTargetingInfo);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHOW_PAYLOADS))
    {
    }
}

// PowerAPI.cpp:5930
static void sAdjustCapsuleTargetForBonusHeight(
    Vector3D & vTargetPos,
    const Vector3D & vComparePos,
    float flBonusHeight)
{
    float flDeltaZ = vTargetPos.z - vComparePos.z;
    if (flDeltaZ >= -flBonusHeight && flDeltaZ <= flBonusHeight)
    {
        vTargetPos.z = vComparePos.z;
    }
    else
    {
        vTargetPos.z += (flDeltaZ > 0.0f) ? -flBonusHeight : flBonusHeight;
    }
}

// PowerAPI.cpp:5950
static void sPowerAPIFindCapsuleTargets(
    const ChooseTargetGeometryCapsule & tCapsule,
    XPooledDataIDList & listTargets,
    const PowerAPIChooseTargets & tTargetingInfo)
{
    WorldPlace placeCenter;
    PowerAPI_CapsuleFindCenter(tCapsule, placeCenter);

    Capsule tBroadPhaseCapsule;
    tBroadPhaseCapsule.flRadius = tCapsule.flRadius + tCapsule.flBonusHeight;
    tBroadPhaseCapsule.wp1 = tCapsule.place0.wp;
    tBroadPhaseCapsule.wp2 = tCapsule.place0.wp + tCapsule.vDir * tCapsule.flLength;
    uint32 dwSphereTreeFlags = (tTargetingInfo.dwFlags & (1 << CTF_TARGET_ITEMS_ONLY)) ? (1 << ACTOR_SPHERE_TREE_ITEMS_IN_WORLD) : ACTOR_SPHERE_TREE_FLAGS_NO_ITEMS;
    ACDsIntersectCapsule(placeCenter.idSWorld, tBroadPhaseCapsule, listTargets, dwSphereTreeFlags);

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        POS posLast = pos;
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData & tACDTarget = ACDGet(idACDTarget);

        float flTargetRadius = tACDTarget.GetRadius();
        WorldPlace placeTarget = tACDTarget.GetWorldPlaceCenter();

        float flCombinedRadius = flTargetRadius + tCapsule.flRadius;
        float flCombinedRadiusSqr = flCombinedRadius * flCombinedRadius;

        Vector3D vToTarget = placeTarget.wp - tCapsule.place0.wp;
        float flTargetProj = Vector3D::DotProduct(vToTarget, tCapsule.vDir);
        if (flTargetProj <= 0.0f)
        {
            sAdjustCapsuleTargetForBonusHeight(placeTarget.wp, tCapsule.place0.wp, tCapsule.flBonusHeight);
            float flDeltaSqr = Vector3D::DistanceSquared(tCapsule.place0.wp, placeTarget.wp);
            if (flDeltaSqr <= flCombinedRadiusSqr)
            {
                continue;
            }
        }
        else if (flTargetProj >= tCapsule.flLength)
        {
            Vector3D vEndPoint = tCapsule.place0.wp;
            vEndPoint += tCapsule.vDir * tCapsule.flLength;
            sAdjustCapsuleTargetForBonusHeight(placeTarget.wp, vEndPoint, tCapsule.flBonusHeight);
            float flDeltaSqr = Vector3D::DistanceSquared(vEndPoint, placeTarget.wp);
            if (flDeltaSqr <= flCombinedRadiusSqr)
            {
                continue;
            }
        }
        else
        {
            Vector3D vProjPoint = tCapsule.vDir * flTargetProj;
            vProjPoint += tCapsule.place0.wp;
            float flDeltaSqr = Vector3D::DistanceSquared(vProjPoint, placeTarget.wp);
            if (flDeltaSqr <= flCombinedRadiusSqr)
            {
                continue;
            }

            sAdjustCapsuleTargetForBonusHeight(placeTarget.wp, vProjPoint, tCapsule.flBonusHeight);

            // Inferred: no row of its own; the original reuses the x/y partial sum of the dot product at 5981.
            float flProj2D = vToTarget.x * tCapsule.vDir.x + vToTarget.y * tCapsule.vDir.y;
            Vector2D vProjPoint2D = tCapsule.vDir.As2D() * flProj2D;
            vProjPoint2D += tCapsule.place0.wp.As2D();
            float flDeltaSqr2D = Vector2D::DistanceSquared(vProjPoint2D, placeTarget.wp.As2D());
            if (flDeltaSqr2D <= flCombinedRadiusSqr)
            {
                continue;
            }

            {
                // Inferred: flDeltaSqr3D (DWARF 6039) has no location; the original compares the x/y part of
                // flDeltaSqr (6016) again, so it is the planar distance to vProjPoint.
                float flDeltaSqr3D = Vector3D::Distance2DSquared(vProjPoint, placeTarget.wp);
                if (flDeltaSqr3D <= flCombinedRadiusSqr)
                {
                    continue;
                }
            }
        }

        listTargets.RemoveAt(posLast);
    }

    sPowerAPICullTargetsNotInLOS(tCapsule.place0, listTargets, tTargetingInfo);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHOW_PAYLOADS))
    {
    }
}

// PowerAPI.cpp:6061
static void sPowerAPIFindBoxTargets(
    const ChooseTargetGeometryBox & tBox,
    XPooledDataIDList & listTargets,
    const PowerAPIChooseTargets & tTargetingInfo)
{
    OBB wobb;
    wobb.wvExt.Set(tBox.flLength * 0.5f, tBox.flWidth * 0.5f, 100.0f);
    wobb.transform.wp = tBox.placeCenter.wp;
    wobb.transform.q.FromYaw(tBox.vDir.As2D().ToYaw());

    float flMaxRadius = Math::Max(wobb.wvExt.x, wobb.wvExt.y);
    uint32 dwSphereTreeFlags = (tTargetingInfo.dwFlags & (1 << CTF_TARGET_ITEMS_ONLY)) ? (1 << ACTOR_SPHERE_TREE_ITEMS_IN_WORLD) : ACTOR_SPHERE_TREE_FLAGS_NO_ITEMS;
    ACDsIntersectSphere(tBox.placeCenter, flMaxRadius, ACDID_INVALID, listTargets, dwSphereTreeFlags);

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        POS posCurrent = pos;
        ACDID idACDDetected = (ACDID)listTargets.GetNext(pos);
        WorldPlace wpDetected = ACDGet(idACDDetected).GetWorldPlaceCenter();
        Sphere ws;
        ws.wpCenter = wpDetected.wp;
        ws.wdRadius = 0.25f;

        if (!Intersect::SphereAndOBB(ws, wobb, NULL))
        {
            listTargets.RemoveAt(posCurrent);
        }
    }

    sPowerAPICullTargetsNotInLOS(tBox.placeCenter, listTargets, tTargetingInfo);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHOW_PAYLOADS))
    {
    }
}

// PowerAPI.cpp:6247
void PowerAPI_DoFindAllPossibleTargets(
    const PowerAPIChooseTargets & tTargetingInfo,
    XPooledDataIDList & listTargets)
{
    const ChooseTargetGeometry & tGeometry = tTargetingInfo.tGeometry;

    switch (tGeometry.eType)
    {
    case CTT_SINGLE_TARGET:
        sPowerAPIFindSingleTarget(tGeometry.tSingleTarget, listTargets, tTargetingInfo);
        break;
    case CTT_MELEE:
        sPowerAPIFindMeleeTargets(tTargetingInfo.idPower, tGeometry.tMelee.idACDSource, tGeometry.tMelee.tTarget, listTargets, tTargetingInfo);
        break;
    case CTT_RAY:
        {
            uint32 dwSphereTreeFlags = (tTargetingInfo.dwFlags & (1 << CTF_TARGET_ITEMS_ONLY)) ? (1 << ACTOR_SPHERE_TREE_ITEMS_IN_WORLD) : ACTOR_SPHERE_TREE_FLAGS_NO_ITEMS;
            sPowerAPIFindLineTargets(tGeometry.tRay.placeStart, tGeometry.tRay.placeEnd, listTargets, dwSphereTreeFlags);
        }
        break;
    case CTT_SPHERE:
        sPowerAPIFindSphereTargets(tGeometry.tSphere.placeCenter, tGeometry.tSphere.wdRadius, listTargets, tTargetingInfo);
        break;
    case CTT_FRONT:
        sPowerAPIFindFrontTargets(PowerGetOwner(tTargetingInfo.idPower)->id, tGeometry.tSphere.placeCenter, tGeometry.tSphere.wdRadius, listTargets, tTargetingInfo);
        break;
    case CTT_BACK:
        sPowerAPIFindBackTargets(PowerGetOwner(tTargetingInfo.idPower)->id, tGeometry.tSphere.placeCenter, tGeometry.tSphere.wdRadius, listTargets, tTargetingInfo);
        break;
    case CTT_PIE:
        sPowerAPIFindPieTargets(tGeometry.tPie.placeCenter, tGeometry.tPie.wdRadius, tGeometry.tPie.wdHeight,
            tGeometry.tPie.tTarget, listTargets, tGeometry.tPie.nDegrees,
            tGeometry.tPie.wvPieVector, tTargetingInfo);
        break;
    case CTT_CAPSULE:
        sPowerAPIFindCapsuleTargets(tGeometry.tCapsule, listTargets, tTargetingInfo);
        break;
    case CTT_BOX:
        sPowerAPIFindBoxTargets(tGeometry.tBox, listTargets, tTargetingInfo);
        break;
    case CTT_CYLINDER:
        sPowerAPIFindCylinderTargets(tGeometry.tCylinder, listTargets, tTargetingInfo);
        break;
    // Inferred: the jump table sends CTT_NO_LINE_OF_SIGHT straight to the epilogue (+0xd18), not to default.
    case CTT_NO_LINE_OF_SIGHT:
        break;
    default:
        {
            // Inferred: ptPower (DWARF 6294, no location) feeds an assert compiled out of this build; only
            // the PowerGet call remains (tail call at 6294:26).
            Power *ptPower = PowerGet(tTargetingInfo.idPower);
        }
        break;
    }
}


#line 6310
void PowerAPI_DoChooseTargets(
    const PowerAPIChooseTargets & tTargetingInfo,
    XPooledDataIDList & listTargets)
{
    const ChooseTargetGeometry & tGeometry = tTargetingInfo.tGeometry;

    PowerAPI_DoFindAllPossibleTargets(tTargetingInfo, listTargets);

    if (tTargetingInfo.dwFlags & (1 << CTF_USE_MELEE_SNAPSHOT))
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(tTargetingInfo.idPower);
        if (ptACDOwner && ACDIsPlayer(ptACDOwner->id))
        {
            NavCellHandle hNavCellStart = { SSCENEID_INVALID, 0 };
            BOOL fNavCellInitialized = FALSE;
            Power * ptPower = PowerGet(tTargetingInfo.idPower);
            ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);

            POS pos = ptPower->listInitialMeleeRangeACDs.GetHeadPosition();
            while (pos)
            {
                ACDID idACDCurr = (ACDID)ptPower->listInitialMeleeRangeACDs.GetNext(pos);
                ActorCommonData * ptACDCurr = ACDTryToGet(idACDCurr);
                if (!ptACDCurr || !ptACDCurr->IsAlive() || ptACDCurr->GetLocationType() != AL_WORLD)
                {
                    continue;
                }

                if (!listTargets.Find(idACDCurr))
                {
                    Target tTarget;
                    PowerGetTarget(tTargetingInfo.idPower, tTarget);

                    float flMeleeReach = PowerAPI_ComputeMeleeReach(tTargetingInfo.idPower, idACDCurr);
                    float flTargetInfoRange = 0.0f;
                    int32 nDegrees = 0;
                    switch (tGeometry.eType)
                    {

                    case CTT_SPHERE:
                    case CTT_FRONT:
                    case CTT_BACK:
                        flTargetInfoRange = tGeometry.tSphere.wdRadius;
                        break;
                    case CTT_MELEE:
                        flTargetInfoRange = (float)tGeometry.tMelee.nMeleeRange;
                        break;
                    case CTT_SINGLE_TARGET:
                        flTargetInfoRange = tGeometry.tSingleTarget.wdSearchRadius;
                        break;
                    case CTT_PIE:
                        flTargetInfoRange = tGeometry.tPie.wdRadius;
                        break;

                    case CTT_CAPSULE:
                        flTargetInfoRange = tGeometry.tCapsule.flLength;
                        break;

                    case CTT_RAY:
                        flTargetInfoRange = Vector3D::Distance2D(tGeometry.tRay.placeStart.wp, tGeometry.tRay.placeEnd.wp);
                        break;
                    case CTT_BOX:
                        flTargetInfoRange = tGeometry.tBox.flLength;
                        break;
                    case CTT_CYLINDER:
                        flTargetInfoRange = tGeometry.tCylinder.flRadius;
                        break;
                    }



                    BOOL bIgnoreRange = (ptPower->dwFlags & (1 << PF_IGNORE_RANGE)) != 0;
                    BOOL bCloseEnough = PowerIsCloseEnough(ptACDOwner->id,
                        pdefPower, &tTarget, flTargetInfoRange, flMeleeReach, NULL, bIgnoreRange);






                    if (bCloseEnough)
                    {
                        BOOL bLOS = TRUE;
                        WorldPlace placeSource = ptACDOwner->GetPlace();
                        if (!(tTargetingInfo.dwFlags & (1 << CTF_DONT_PERFORM_LOS_CHECK)))
                        {
                            if (!fNavCellInitialized)
                            {
                                NavCellHandleFromWorldPlace(placeSource, hNavCellStart);
                                fNavCellInitialized = TRUE;
                            }
                            bLOS = NavMeshCheckLOSToACD(hNavCellStart, placeSource, idACDCurr, tTargetingInfo.ePassabilityType, FALSE);
                        }

                        if (bLOS)
                            listTargets.AddTail_Tracked(idACDCurr);
                    }
                }
            }
        }
    }



    POS pos = listTargets.GetTailPosition();
    while (pos)
    {
        POS posToDeleteAt = pos;
        ACDID idACDTarget = (ACDID)listTargets.GetPrev(pos);
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (!ptACDTarget)
        {
            listTargets.RemoveAt(posToDeleteAt);
            continue;
        }

        if (tTargetingInfo.dwFlags & (1 << CTF_TARGET_ITEMS_ONLY))
        {
            if (!ptACDTarget->IsItem())
            {
                listTargets.RemoveAt(posToDeleteAt);
                continue;
            }

            if ((tTargetingInfo.dwFlags & (1 << CTF_FILTER_ITEMS_BY_LABEL)) &&
                !ItemHasLabelMask(ptACDTarget->hGB.gbid, tTargetingInfo.dwItemLabelMask))
            {
                listTargets.RemoveAt(posToDeleteAt);
                continue;
            }
        }

        if (!PowerCanTarget(tTargetingInfo.idPower, idACDTarget, ACDID_INVALID))
        {
            listTargets.RemoveAt(posToDeleteAt);
            continue;
        }
        else if ((tTargetingInfo.dwFlags & (1 << CTF_IGNORE_STEALTHED)) && ptACDTarget->IsStealthed())
        {
            listTargets.RemoveAt(posToDeleteAt);
        }
    }





    pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget)
        {
            ACDID idACDBuffForwardee = (ACDID)ptACDTarget->AttributesGetInt(ATTRIBUTE_ACTOR_FORWARDS_BUFFS);
            ActorCommonData * ptACDBuffForwardee = ACDTryToGet(idACDBuffForwardee);
            if (ptACDBuffForwardee)
            {
                POS posForwardee = listTargets.Find(idACDBuffForwardee);
                if (posForwardee)
                {
                    listTargets.RemoveAt(posForwardee);
                }
            }
        }
    }



    if ((tTargetingInfo.dwFlags & (1 << CTF_CLOSEST_ONLY_BIT)) && listTargets.Count())
    {
        ACDID idACDClosest = sPowerAPIChooseClosestTarget(tGeometry, listTargets);
        listTargets.RemoveAll();
        listTargets.AddHead_Tracked(idACDClosest);
    }


    if (tTargetingInfo.dwFlags & (1 << CTF_IGNORE_ON_AFFECTED_LIST_BIT))
    {
        Power * ptPower = PowerGet(tTargetingInfo.idPower);

        ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);

        POS posTarget = listTargets.GetHeadPosition();
        while (posTarget)
        {
            POS posCurrent = posTarget;
            ACDID idACDTarget = (ACDID)listTargets.GetNext(posTarget);

            if (PowerHasAffectedACD(tTargetingInfo.idPower, pdefPower, idACDTarget, ACDID_INVALID))
            {



                listTargets.RemoveAt(posCurrent);
            }
        } }
}

#include "../../Common/Hyperlink.h"
#include "../../Common/Marker/Marker.h"
#include "../../Common/SceneCommon.h"
#include "../SWorld.h"
BOOL SWorldIsFlaggedForDeletion(SWorldID idSWorld); // DWARF CPP-only home SWorld.cpp:346; no header declaration found.
#line 6616
ACDID PowerAPI_SummonActor(
    const PowerAPISummonActor & tSummonInfo)
{


    Stopwatch tStopwatch;


    if (SWorldIsFlaggedForDeletion(tSummonInfo.placeSummonLocation.idSWorld))
    {
        return ACDID_INVALID;
    }

    Power * ptPower = PowerGet(tSummonInfo.idPower);
    ActorCommonData * ptACDSummoner = ACDTryToGet(ptPower->idACD);
    int32 nLevel = tSummonInfo.nActorLevel;
    if (nLevel <= 0)
    {
        if (PowerContactTagGetBool(ptPower->id, TAG_POWER_SUMMONED_ACTOR_USE_DEFAULT_LEVEL))
        {
            nLevel = -1;
        }

        else if (ptACDSummoner)
        {
            nLevel = ptACDSummoner->AttributesGetInt(ATTRIBUTE_LEVEL);
        }
        else
        {
            nLevel = 1;
        }
    }

    int32 idACDSummoner = -1;
    if (ptACDSummoner)
    {
        idACDSummoner = ptACDSummoner->GenerateSummonerID();
    }



    Team eTeam;
    if (tSummonInfo.eTeamOverride != TEAM_INVALID)
        eTeam = tSummonInfo.eTeamOverride;
    else
    {
        if (tSummonInfo.idACDCorpse == ACDID_INVALID)
        {
            if (!ptACDSummoner)
            {
                return ACDID_INVALID;
            }



            if (!ACDIsPlayer(ptACDSummoner->id) || !ptACDSummoner->IsDueling())
            {
                eTeam = ptACDSummoner->GetNaturalTeam();
            }
            else
            {
                eTeam = ptACDSummoner->GetTeam();
            }
        }
        else
        {
            eTeam = ACDGet(tSummonInfo.idACDCorpse).GetTeam();
        }
    }
    CRefString sActorName("Summoned_");
    sActorName += SNOToString(ACTOR_SNO, tSummonInfo.snoActor, FALSE);

    float flTime;
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    ActorAddStruct tActorAdd;
    tActorAdd.dwActorAddFlags = tSummonInfo.dwAddFlags;
    if (ptACDSummoner && (ptACDSummoner->dwFlags & (1 << ACD_IS_FROM_DISTRIBUTION_BIT)))
    {
        tActorAdd.dwActorAddFlags |= (1 << ACTORADD_IS_FROM_DISTRIBUTION);
    }
    tActorAdd.tPlace = tSummonInfo.placeSummonLocation;
    tActorAdd.qOrientation = tSummonInfo.qOrientation;
    tActorAdd.eTeam = eTeam;
    tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
    tActorAdd.tSNOName.snoHandle = tSummonInfo.snoActor;
    tActorAdd.tMarkersName = MarkerStringHash(sActorName);
    tActorAdd.nLevel = nLevel;
    tActorAdd.bIsSummon = TRUE;
    ScenesCommonNavMeshNodeFromPlace(tSummonInfo.placeSummonLocation, &tActorAdd.tNavMeshHandle, FALSE, FALSE);

    if (NavMeshHandleIsNone(tActorAdd.tNavMeshHandle))
    {
        CRefString sMessage;
        sMessage.Format("PowerAPI_SummonActor: Adding %s failed because of bad spawn location.", sActorName.str());

        SWorld * ptSWorld = SWorldTryToGet(tActorAdd.tPlace.idSWorld);
        if (ptSWorld)
        {
            sMessage += CRefString().Format("  Location: [%0.2f, %0.2f, %0.2f] in %s.",
                tActorAdd.tPlace.wp.x,
                tActorAdd.tPlace.wp.y,
                tActorAdd.tPlace.wp.z,
                HyperlinkCreateFromSNO(SNOName(WORLD_SNO, ptSWorld->tWCD.snoWorld), NULL).str());
        }

        sMessage += "\n";



        return ACDID_INVALID;
    }

    tActorAdd.snoActor = tSummonInfo.snoActor;

    ActorID idActor = GBAdd(tActorAdd);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (idActor == ACTORID_INVALID)
    {
        return ACDID_INVALID;
    }


    Actor * ptActor = ActorTryToGet(idActor);

    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    tACD.AttributesSetInt(ATTRIBUTE_SUMMONED_BY_ACDID, idACDSummoner);
    tACD.AttributesSetInt(ATTRIBUTE_SUMMONED_BY_SNO, ptPower->snoPower);
    tACD.AttributesSetFloat(ATTRIBUTE_ATTACKS_PER_SECOND_PERCENT_CAP, 999.0f);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (!tSummonInfo.bNoSpawnAnimPower)
    {
        uint32 dwAnimTag = tSummonInfo.dwAnimTag;

        if (dwAnimTag == 0 || dwAnimTag == TAG_ACTOR_ANIMATION_INVALID)
        {


            dwAnimTag = PowerContactTagGetBool(tSummonInfo.idPower, TAG_POWER_PLAY_SUMMONED_BY_MONSTER_ANIMATION) ?
                TAG_ACTOR_ANIMATION_GETTING_SUMMONED_BY_MONSTER : TAG_ACTOR_ANIMATION_GETTING_SUMMONED;
        }


        PowerAPI_StartSpawnAnimPower(idACD, dwAnimTag);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }
    }


    ActorTriggerAttributeUpdate(ptActor->id);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    return idACD;
}

#include "../../Common/Actors/ActorEffectSwig.h"
#line 1619
static void sApplyThorns(
    float flPowerProcScalar,
    float * pflDamageDone,
    ActorCommonData & tACDAttacker,
    ActorCommonData & tACDTarget,
    Power * ptPower,
    BOOL bForceThornsAoE)
{








    if (!tACDAttacker.IsTargetable())
    {
        return;
    }

    if (!tACDAttacker.IsAlive())
    {
        return;
    }

    if (!tACDTarget.IsAlive())
    {
        return;
    }

    if (tACDAttacker.id == tACDTarget.id)
    {
        return;
    }


    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Blank(tPayload);

    tPayload.flPetDamageScalar = 1.0f;


    BOOL fHaveDamageToApply = FALSE;
    for (int32 i = 0; i < NUM_DAMAGE_TYPES; ++i)
    {

        float flPercent = tACDTarget.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_THORNS_PERCENT_TOTAL, i)) * flPowerProcScalar;
        if (flPercent > EPSILON)
        {

            tPayload.arCombatDamage[i].flMin = flPercent * pflDamageDone[i];
            tPayload.arCombatDamage[i].flDelta = 0.0f;


            if (tPayload.arCombatDamage[i].flMin > EPSILON)
            {
                fHaveDamageToApply = TRUE;
            }
        }


        float flFixed = tACDTarget.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_THORNS_FIXED_TOTAL, i)) * flPowerProcScalar;
        if (flFixed > EPSILON)
        {

            tPayload.arCombatDamage[i].flMin += flFixed;
            fHaveDamageToApply = TRUE;
        }




        if (tPayload.arCombatDamage[i].flMin > EPSILON)
        {
            ScopedGlobalDefinitionLockR pdefGlobal;



            float flPrimaryAttributeMultiplier = PowerComputeDamageScalarFromPrimaryAttribute(tACDTarget.idFastAttrib);
            flPrimaryAttributeMultiplier *= pdefGlobal->ptServerData->flThornsPrimaryStatMultiplier;

            tPayload.arCombatDamage[i].flMin *= (1.0f + flPrimaryAttributeMultiplier);


            float flSkillPercentDamageMultiplier = tACDTarget.AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS);
            tPayload.arCombatDamage[i].flMin *= (1.0f + flSkillPercentDamageMultiplier);


            if (i == DAMAGE_TYPE_PHYSICAL)
            {
                float flPhysicalPercentDamageMultiplier = tACDTarget.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_PERCENT_BONUS, DAMAGE_TYPE_PHYSICAL));
                tPayload.arCombatDamage[i].flMin *= (1.0f + flPhysicalPercentDamageMultiplier);
            }
        }
    }


    if (!fHaveDamageToApply)
    {
        return;
    }


    float flDamageTotal = 0.0f;
    int32 nNumDamageTypes = 0;
    for (int32 i = 0; i < NUM_DAMAGE_TYPES; ++i)
    {
        flDamageTotal += tPayload.arCombatDamage[i].flMin;


        if (tACDTarget.AttributesGetInt(MakeAttribKey(ATTRIBUTE_THORNS_HAS_DAMAGE_TYPE, i)))
        {
            ++nNumDamageTypes;
        }
    }


    if (nNumDamageTypes > 0)
    {

        PowerHelper_AttackPayloadInit_Blank(tPayload);

        for (int32 i = 0; i < NUM_DAMAGE_TYPES; ++i)
        {
            if (tACDTarget.AttributesGetInt(MakeAttribKey(ATTRIBUTE_THORNS_HAS_DAMAGE_TYPE, i)))
            {
                tPayload.arCombatDamage[i].flMin += flDamageTotal / (float)nNumDamageTypes;
            }
        }
    }


    Target tTarget;
    tTarget.eType = TT_ACTOR;
    tTarget.idACDTarget = tACDAttacker.id;


    tPayload.idPower = PowerStart(tACDTarget.id, GlobalSNOGet(GLOBALSNO_POWERS_THORNS), tTarget);
    if (ptPower && tPayload.idPower != POWERID_INVALID &&

        !ptPower->bDoesntAnimate &&
        !ptPower->bFreeCast &&
        !PowerIsPassive(AutoPowerDefinitionR(ptPower->snoPower)))
    {
        tPayload.eDeliveryMechanism = PDM_THORNS;
        tPayload.eKnockBack = KNOCKBACK_NONE;
        tPayload.snoTargetImpactParticle = SNO_NONE;
        tPayload.snoTargetImpactEffectGroup = SNO_NONE;
        tPayload.snoTargetAttackedParticle = SNO_NONE;
        tPayload.bNeverCauseGetHit = TRUE;
        tPayload.eDeathReplacementType = DEATH_REPLACEMENT_NONE;



        BOOL bApplyThornsToAttacker = TRUE;
        float flThornAOERadius = tACDTarget.AttributesGetFloat(ATTRIBUTE_THORNS_AOE_RADIUS);
        if (flThornAOERadius > EPSILON)
        {
            tPayload.bIsAOEThorns = TRUE;

            uint32 uNow = GameGetServerTime();
            uint32 uNextAOETime = tACDTarget.AttributesGetInt(ATTRIBUTE_THORNS_AOE_RADIUS_NEXT_TIME);
            if (bForceThornsAoE || uNow >= uNextAOETime)
            {
                WorldPlace placeTarget = tACDTarget.GetPlace();
                XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
                PowerAPIChooseTargets tChooseTargets;
                PowerHelper_ChooseTargetsInit(tPayload.idPower, tChooseTargets);
                PowerHelper_ChooseTargetsInitGeometrySphere(placeTarget, flThornAOERadius, tChooseTargets.tGeometry);



                PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

                POS posTarget = listTargets.GetHeadPosition();
                while (posTarget)
                {
                    ACDID idACDHit = (ACDID)listTargets.GetNext(posTarget);
                    ActorCommonData * ptACDHit = ACDTryToGet(idACDHit);
                    if (ptACDHit &&
                        ptACDHit->id != tACDTarget.id &&
                        !ACDIsFriendly(*ptACDHit, tACDTarget, TRUE))
                    {
                        PowerAPI_DoPayload(tPayload, idACDHit, ACDID_INVALID);

                        if (idACDHit == tACDAttacker.id)
                            bApplyThornsToAttacker = FALSE;
                    }
                }



                uNextAOETime = uNow + 20;
                tACDTarget.AttributesSetInt(ATTRIBUTE_THORNS_AOE_RADIUS_NEXT_TIME, uNextAOETime);
            }

            tPayload.bIsAOEThorns = FALSE;
        }

        if (bApplyThornsToAttacker)
        {
            PowerAPI_DoPayload(tPayload, tACDAttacker.id, ACDID_INVALID);
        }

        if (!tACDTarget.AttributesGetInt(ATTRIBUTE_SUPRESS_THORNS_EFFECTS))
        {
            const uint32 THORNS_MINIMUM_EFFECT_INTERVAL = 60;
            uint32 dwLastEffectTick = tACDAttacker.AttributesGetInt(ATTRIBUTE_THORNS_EFFECT_LAST_PLAYED_TICK);
            uint32 dwNow = GameGetServerTime();
            if (dwNow - dwLastEffectTick > THORNS_MINIMUM_EFFECT_INTERVAL)
            {
                ACDPlayEffect(tACDAttacker.id, EFFECT_PLAY_EFFECT_GROUP, GlobalSNOGet(GLOBALSNO_EFFECTGROUP_THORNS), tACDAttacker.id);
                tACDAttacker.AttributesSetInt(ATTRIBUTE_THORNS_EFFECT_LAST_PLAYED_TICK, dwNow);
            }
        }
    }
}
