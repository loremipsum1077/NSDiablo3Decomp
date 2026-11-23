// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Powers/PowersCommon.h
// Subset: declarations of the PowersCommon.cpp functions the server powers call.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../ActorCommonData.h"
#include "../Attributes/FastAttribConstantsSwig.h"
#include "../Definitions/PowerDefinition.h"
#include "../TagMaps/PowerTags.h"

typedef int32 AttribGroupID;

class CRefString;

BOOL PowerHasInfiniteAnimationTiming(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp:294
typedef int32 PlayerClass;  // X/Common/GameSwig.h

PlayerClass PowerGetClassRestriction(AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp:322
BOOL PowerIsBasicAttack(AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp:735
BOOL PowerIsChannelled(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp:775
BOOL PowerReuseScriptState(AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp:2071
BOOL PowerWantsActorTarget(AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp:1094
uint32 PowerGetCoolDown(const ActorCommonData & tACD);  // PowersCommon.cpp:4532
float PowerGetFormulaValue(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower, uint32 dwPowerTag, int32 nLevel, AttribGroupID idAttribGroup, uint32 nContactFrame);  // PowersCommon.cpp:5546
BOOL PowerCanMoveForwardBeforeContactFrame(AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp:5671
float PowerGetBaseDamageScalar(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp:4656
DamageType PowerGetDamageType(AutoPowerDefinitionR pdefPower, const RuneType eRuneType);  // PowersCommon.cpp:5944

// Declared here from its definition in PowersCommon.cpp; the localized power-name
// label UISkills::GetName fetches from the POWERS string list. Returns a
// CRefString by value (_Z17PowerGetNameLabeli, 0x710067a330).
CRefString PowerGetNameLabel(SNO snoPower);

// Called from X/Server/Powers/Power_Enchantress.cpp (_Z17Enchantress_CharmiPv,
// 0x71009695a0, Power_Enchantress.cpp:101). Mangling _Z36PowerRuneTypeAndBuffIndexToAttributeiii.
Attrib PowerRuneTypeAndBuffIndexToAttribute(SNO snoPower, int32 nRuneType, int32 nBuffIndex);  // PowersCommon.cpp

// Power policy queries; declarations from PowersCommon.cpp.
BOOL PowerIsCompletedWhenWalkingStops(AutoPowerDefinitionR pdefPower);
BOOL PowerCanSteer(AutoPowerDefinitionR pdefPower);
BOOL PowerIsTranslate(AutoPowerDefinitionR pdefPower);
enum
{
    HITSOUND_NONE = 0,
    HITSOUND_BLADE = 1,
    HITSOUND_BLUNT = 2,
    HITSOUND_HTH = 3,
    HITSOUND_ARROW = 4,
    HITSOUND_FIRE = 5,
    HITSOUND_ICE = 6,
    HITSOUND_LIGHTNING = 7,
    HITSOUND_POISON = 8,
    HITSOUND_IMMUNE = 9,
    HITSOUND_NUM_HITSOUNDS = 10,
};
typedef int32 HitSoundType;

BOOL PowerIsOffensive(AutoPowerDefinitionR pdefPower);
BOOL PowerIsPassive(AutoPowerDefinitionR pdefPower);
BOOL PowerCanPathDuringWalk(AutoPowerDefinitionR pdefPower);
BOOL PowerIsKnockbackMovementPower(AutoPowerDefinitionR pdefPower);
BOOL PowerPreplaysAnimation(AutoPowerDefinitionR pdefPower);
BOOL PowerIsComboPower(AutoPowerDefinitionR pdefPower);
BOOL PowerRefreshesComboLevel(AutoPowerDefinitionR pdefPower);
BOOL PowerHasScaledAnimationTiming(AutoPowerDefinitionR pdefPower);
BOOL PowerWalksCloserIfOutOfRange(AutoPowerDefinitionR pdefPower);
BOOL PowerIsAimedAtGround(AutoPowerDefinitionR pdefPower);
BOOL PowerTargetGroundOnly(AutoPowerDefinitionR pdefPower);
BOOL PowerTargetNavMeshOnly(AutoPowerDefinitionR pdefPower);
BOOL PowerTargetPathableOnly(AutoPowerDefinitionR pdefPower);
BOOL PowerTargetsContactPlaneOnly(AutoPowerDefinitionR pdefPower);
BOOL PowerPlayerCanUseWhenDead(AutoPowerDefinitionR pdefPower);
BOOL PowerIsPunch(AutoPowerDefinitionR pdefPower);
BOOL PowerIsDisplayed(AutoPowerDefinitionR pdefPower);
BOOL PowerStickyTargetsActorsWhileSweeping(AutoPowerDefinitionR pdefPower);
BOOL PowerIsEmote(AutoPowerDefinitionR pdefPower);
BOOL PowerAffectedByDualWield(AutoPowerDefinitionR pdefPower);
BOOL PowerContactFreezesFacing(AutoPowerDefinitionR pdefPower);
BOOL PowerSnapsToFacing(AutoPowerDefinitionR pdefPower);
BOOL PowerClientControlsFacing(AutoPowerDefinitionR pdefPower);
BOOL PowerExtendsTargetToAttackRadius(AutoPowerDefinitionR pdefPower);
BOOL PowerCausesClosingCooldown(AutoPowerDefinitionR pdefPower);
BOOL PowerNoInterruptTimer(AutoPowerDefinitionR pdefPower);
BOOL PowerCastTargetAllies(AutoPowerDefinitionR pdefPower);
BOOL PowerCastTargetEnemies(AutoPowerDefinitionR pdefPower);
BOOL PowerCastTargetNeutral(AutoPowerDefinitionR pdefPower);
BOOL PowerNoAffectedACD(AutoPowerDefinitionR pdefPower);
BOOL PowerTracksAffectedACDInstanceWide(AutoPowerDefinitionR pdefPower);
BOOL PowerIsCancellable(AutoPowerDefinitionR pdefPower);
BOOL PowerIsCancellableByAnyPower(AutoPowerDefinitionR pdefPower);
BOOL PowerIsCancellableByWalking(AutoPowerDefinitionR pdefPower);
BOOL PowerCancelsOtherPowers(AutoPowerDefinitionR pdefPower);
BOOL PowerIsNoRecastUntilLastAnimFrame(AutoPowerDefinitionR pdefPower);
BOOL PowerCancelsOnInterruptFrame(AutoPowerDefinitionR pdefPower);
BOOL PowerSnapsToGround(AutoPowerDefinitionR pdefPower);
BOOL PowerApplyPassiveAfterItemPassives(AutoPowerDefinitionR pdefPower);
BOOL PowerGeneralTagGetBool(AutoPowerDefinitionR pdefPower, uint32 dwTag);
int32 PowerGeneralTagGetInt(AutoPowerDefinitionR pdefPower, uint32 dwTag);
GBID PowerGeneralTagGetGBID(AutoPowerDefinitionR pdefPower, uint32 dwTag);
float PowerGeneralTagGetFloat(AutoPowerDefinitionR pdefPower, uint32 dwTag);
SNO PowerGeneralTagGetSNO(AutoPowerDefinitionR pdefPower, uint32 dwTag);
BOOL PowerIsOffensive(SNO snoPower);
BOOL PowerIsPassive(SNO snoPower);
BOOL PowerIsComboPower(SNO snoPower);
int32 PowerActorGetMaxCharges(const ActorCommonData & tACD, SNO snoPower);
float PowerGetAnimationSpeedForPower(const ACDID idACD, AutoPowerDefinitionR pdefPower, const int32 nComboLevel);  // PowersCommon.cpp:8234
BOOL PowerBuffMergesTooltip(AutoPowerDefinitionR pdefPower, int32 nBuffIndex);
BOOL PowerBuffShowsActiveOnSkillButton(AutoPowerDefinitionR pdefPower, int32 nBuffIndex);

enum {
    UNDEFINED_POWER_TEMPLATE = 0,
    WALK_POWER = 3,
    GIZMO_POWER = 4,
    ANIMATION_POWER = 6,
    HEARTBEAT_POWER = 7,
    ONSLAY_POWER = 8,
    ONGETHIT_POWER = 9,
    ONCRITICAL_POWER = 11,
    POTION_POWER = 21,
    USE_ITEM_POWER = 23,
    ONCREATE_POWER = 24,
    PROC_POWER = 25,
    SPECIALWALK_POWER = 30,
    SECONDARY_ANIMATION_POWER = 35,
    LOOPING_ANIMATION_POWER = 40,
    ATTACKMOVE_POWER = 41,
};

typedef int32 PowerTemplate;

enum {
    CONTACT_FRAME_NONE = -1,
    CONTACT_FRAME_NORMAL = 0,
    CONTACT_FRAME_IN_END = 1,
    CONTACT_FRAME_IN_START = 2,
};

typedef int32 ContactFrameType;

enum {
    CHARGE_STEERING_NONE = 0,
    CHARGE_STEERING_WITH_PATHING = 1,
    CHARGE_STEERING_DYNAMIC_ONLY = 2,
};

typedef int32 ChargeSteeringType;

enum {
    POWER_HOTBAR_EXCLUSIVE_TYPE_NONE = 0,
    POWER_HOTBAR_EXCLUSIVE_TYPE_CRUSADER_LAW = 1,
    POWER_HOTBAR_EXCLUSIVE_TYPE_MONK_MANTRA = 2,
    POWER_HOTBAR_EXCLUSIVE_TYPE_COUNT = 3,
};

typedef int32 PowerHotbarExclusiveType;

typedef int32 AutoAssignLocation;

enum {
    START_LOS_NORMAL = 0,
    START_LOS_PROJECTILE = 1,
    START_LOS_MONSTER = 2,
    START_LOS_CHARGE = 3,
    START_LOS_NOCHECK = 4,
};

typedef int32 PowerStartLOSType;


PowerTemplate PowerGetTemplate(AutoPowerDefinitionR pdefPower, ACDID idACDOwner);
BOOL PowerContactTagGetBool(AutoPowerDefinitionR pdefPower, uint32 dwTag, uint32 nContactFrame);
int32 PowerContactTagGetInt(AutoPowerDefinitionR pdefPower, uint32 dwTag, uint32 nContactFrame);
float PowerContactTagGetFloat(AutoPowerDefinitionR pdefPower, uint32 dwTag, uint32 nContactFrame);
SNO PowerContactTagGetSNO(AutoPowerDefinitionR pdefPower, uint32 dwTag, uint32 nContactFrame);
BOOL PowerGeneralTagExists(AutoPowerDefinitionR pdefPower, uint32 dwTag);
BOOL PowerContactTagExists(AutoPowerDefinitionR pdefPower, uint32 dwTag, uint32 nContactFrame);
BOOL PowerChecksVerticalMovement(AutoPowerDefinitionR pdefPower);
ChargeSteeringType PowerGetChargeSteeringType(AutoPowerDefinitionR pdefPower);
PowerHotbarExclusiveType PowerGetHotbarExclusiveType(AutoPowerDefinitionR pdefPower);
BOOL PowerUsesMainHandOnly(AutoPowerDefinitionR pdefPower, int32 nComboLevel);
ContactFrameType PowerGetContactFrameType(AutoPowerDefinitionR pdefPower, int32 nContactFrame);
SNO PowerConsumedItem(AutoPowerDefinitionR pdefPower);
PowerStartLOSType PowerGetStartLOS(AutoPowerDefinitionR pdefPower);
BOOL PowerIsGizmo(AutoPowerDefinitionR pdefPower);
BOOL PowerRequiresActorTarget(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);
BOOL PowerBreaksStun(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerBreaksFear(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerIsInvulnerableDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerIsInvisibleDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerIsUntargetableDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerIsUninterruptableDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerCannotDieDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerCanUseWhenFeared(ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerBreaksSnare(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerBreaksRoot(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerBreaksImmobilize(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerIsImmuneToStunDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerIsImmuneToSnareDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerIsImmuneToRootDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerIsImmuneToFearDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerIsRecoilImmune(ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerIsKnockbackImmune(ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);
BOOL PowerRequiresTarget(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);

typedef int32 RuneType;
RuneType PowerGetRuneTypeForACD(const ActorCommonData & tACD, SNO snoPower);

// PowersCommon.h:35; value present in the formula recursion guard.
static const int POWER_FORMULA_STACK_OVERFLOW_LIMIT = 32;
BOOL PowerIsPrimary(SNO snoPower, ACDID idACD);  // PowersCommon.cpp:698

// X/Common/Powers/PowersCommon.cpp (called from X/Common/ActorAttributes.cpp).
BOOL PowerIsToggleable(AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp:744
SNO PowerSNOFromAttributeKey(const FastAttribKey & tKey);  // PowersCommon.cpp:8349

float PowerGetAttackRadiusAtLevel(const ActorCommonData * ptACDOwner, AutoPowerDefinitionR pdefPower, const int32 nSkillLevel, const BOOL bUseBonusRange);  // PowersCommon.cpp:2880
uint32 PowerGetCastingCooldown(AutoPowerDefinitionR pdefPower, const ActorCommonData & tACD);  // PowersCommon.cpp:4558

// X/Common/Powers/PowersCommon.cpp (called from X/Client/UserInterface/Interface/UIToolTips.cpp).
typedef int32 PowerID;  // X/Common/Powers/PowersStruct.h
float PowerComputeDamageScalarFromPrimaryAttributeValue(float flPrimaryAttributeValue);  // PowersCommon.cpp:3647
float PowerComputeCritDamageScalar(const ActorCommonData * ptACDAttacker);  // PowersCommon.cpp:3718
float PowerComputeArmorDamageMitigation(const ActorCommonData & tACDTarget, const int32 nAttackerLevel);  // PowersCommon.cpp:3819
void PowerActorGetWeaponDamageAndAttackSpeed(const ActorCommonData & tACD, float & flDamage, float & flAttackSpeed, const BOOL bWantBonuses, const PowerID idPower, const BOOL bUsedForDPS);  // PowersCommon.cpp:4391
float PowerComputeDamageScalarFromPrimaryAttribute(AttribGroupID idFastAttrib);  // PowersCommon.cpp:3655
float PowerComputeCritChance(const ActorCommonData * ptACDAttacker, BOOL bUsedForDPS);  // PowersCommon.cpp:3688
typedef int32 PayloadDeliveryMechanism;  // X/Common/Powers/PowerAPISwig.h
float PowerComputeDodgeChance(const ActorCommonData & tACDDefender, int32 nLevel, PayloadDeliveryMechanism eDeliveryType, BOOL bUsedForToughness);  // PowersCommon.cpp:3776
class Target;  // X/Common/Target.h
BOOL PowerIsPrimary(AutoPowerDefinitionR pdefPower, ACDID idACD);  // PowersCommon.cpp:671
BOOL PowerCanTarget(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, const Target * ptTarget);  // PowersCommon.cpp:3246
BOOL PowerCanTarget(const PowerID idPower, const ACDID idACDTarget, const ACDID idACDSource);  // PowersCommon.cpp:2976
float PowerGetRange(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower, const BOOL bUseBonusRange);  // PowersCommon.cpp:2937
void PowerPayloadGetBonusMultipliers(AttribGroupID idFastAttrib, SNO snoPower, PowerID idPower, PlayerClass eClass, float & flPrimaryAttributeBonusMultiplier, float & flClassBonusMultiplier, float & flSkillsBonusMultiplier);  // PowersCommon.cpp:2323
BOOL PowerActorHasResourceMinCostToStart(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp
BOOL PowerShouldUseClientWalking(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp
uint32 PowerACDCanStartPowerFailureReasons(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, BOOL bIsAI, BOOL bFreeCast);  // PowersCommon.cpp:2514

struct ElementDamage {  // size 0x8
    float flTotalMin;  // 0x00
    float flTotalDelta;  // 0x04
};
static_assert(sizeof(ElementDamage) == 0x8, "ElementDamage");

struct PowerDamageData {  // size 0x44
    ElementDamage tElementDamage[7];  // 0x00
    float flPrimaryAttributeBonusMultiplier;  // 0x38
    float flClassBonusMultiplier;  // 0x3c
    float flSkillsBonusMultiplier;  // 0x40
};
static_assert(sizeof(PowerDamageData) == 0x44, "PowerDamageData");

struct PowerAPIAttackPayload;  // X/Server/Powers/PowerAPI.h
void PowerGetDamageData(AutoPowerDefinitionR pdefPower, const ActorCommonData * ptACDOwner, PowerID idPower, PowerDamageData & tData, PowerAPIAttackPayload * ptPayload, BOOL bForceUseACDAttributes);  // PowersCommon.cpp:2367

enum PowerFailureFlags {
    PS_CANCELABLE_SKILLS_ACTIVE = 0,
    PS_UNCANCELABLE_SKILLS_ACTIVE = 1,
    PS_NOT_CLOSE_ENOUGH_BIT = 2,
    PS_NEED_COOLDOWN_BIT = 3,
    PS_NEED_EMOTE_COOLDOWN_BIT = 4,
    PS_NEED_HEALTH_BIT = 5,
    PS_NEED_CHARGES_BIT = 6,
    PS_NEED_RESOURCE_BIT = 7,
    PS_NOT_IN_TOWN = 8,
    PS_INVALID_TARGET = 9,
    PS_INVALID_SOURCE = 10,
    PS_NO_LINE_OF_SIGHT = 11,
    PS_WRONG_WEAPON = 12,
    PS_IMMOBILIZED = 13,
    PS_FEARED = 14,
    PS_NOT_IN_COMBAT = 15,
    PS_ONLY_IN_TOWN_PORTAL_AREA = 16,
    PS_TARGET_NOT_IN_TELEPORTABLE_AREA = 17,
    PS_UNUSABLE_DURING_BOSS_ENCOUNTER = 18,
    PS_NEED_GENERAL_COOLDOWN_BIT = 19,
    PS_SILENCED = 20,
    PS_FROZEN = 21,
    PS_UNABLE_TO_FREECAST_BIT = 22,
    PS_TARGET_IS_DEAD = 23,
    PS_MISSING_ITEM = 24,
    PS_CASTER_IS_DEAD = 25,
    PS_CASTER_CANT_START_DISPLAYED_POWERS = 26,
    PS_CASTER_IS_STUNNED = 27,
    PS_POWER_DISABLED = 28,
    PS_NEED_SKILLPOINT = 29,
    PS_NEED_CORPSES = 30,
    PS_NEED_TARGET = 31,
    PS_COUNT = 32,
};

// PowersCommon.cpp functions defined after their first caller in the file.
ResourceType PowerGetResourceType(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp:5444
int32 PowerGetLevel(const ActorCommonData * ptACD, SNO snoPower);  // PowersCommon.cpp:5531
BOOL PowerActorMeetsActiveSkillMasteryRequirement(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp:3881
BOOL PowerActorMeetsTraitMasteryRequirement(const ActorCommonData & tACD, SNO snoPower);  // PowersCommon.cpp:3921
BOOL PowerIsCloseEnough(ACDID idACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, float wdRange, float wdLongReach, float * pflDistance, BOOL bIgnoreRange);  // PowersCommon.cpp:5072
float PowerGetCoolDownReductionPercent(SNO snoPower, const ActorCommonData & tACD);  // PowersCommon.cpp:4541

float PowerComputeResistanceDamageMitigation(float flResistanceTotal, int32 nAttackerLevel);  // PowersCommon.cpp:3760

float PowerGetCostReductionPercent(const ActorCommonData & tACD, const ResourceType eResourceType, const DamageType eDamageType, const SNO snoPower);  // PowersCommon.cpp:4090

WorldDistance PowerGetDistanceViaWorldPlace(const WorldPlace & placeStart, const WorldPlace & placeTarget, WorldDistance wdStartHeight, const WorldDistance wdStartRadius, WorldDistance wdTargetHeight, const WorldDistance wdTargetRadius, const WorldDistance wdLongReach);  // PowersCommon.cpp:8290

// X/Common/Powers/PowersCommon.h:92, raw enum and typedef.
typedef int32 ResourceCheckResult;
enum {
    RESOURCE_CHECK_OK = 0,
    RESOURCE_CHECK_NEEDS_FREECAST = 2,
};
