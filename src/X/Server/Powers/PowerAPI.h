// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Powers/PowerAPI.h
// Subset: the structs PowerScript.cpp touches. Members it does not use are padding (invented
// names) until someone needs them.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/Powers/PowerAPISwig.h"
#include "../../Common/Powers/PowersStruct.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Common/Target.h"
#include "../../Common/WorldPlace.h"
#include "Powers.h"
#include "../Actors/Projectile.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/SigmaTypes.h"

typedef int32 CalloutEventType;

typedef int32 LootContext;

struct CCInfo {  // size 0xc
    int32 nCurrentCCPriority;  // 0x00
    ACDID idACDAttacker;  // 0x04
    int32 eType;  // 0x08 (FloatingNumberType)
};
static_assert(sizeof(CCInfo) == 0xc, "CCInfo");

struct ImpactData {  // size 0x28
    ACDID idACDProjectile;  // 0x00
    ACDID idACDIntersected;  // 0x04
    SpellEffect eSpellEffect;  // 0x08
    Vector3D vPosition;  // 0x0c
    Vector3D vNormal;  // 0x18
    BOOL bStop;  // 0x24
    ImpactData() { Initialize(); }  // line 54
    ImpactData(ACDID idACDCollidingProj) {  // line 55
        Initialize();
        idACDProjectile = idACDCollidingProj;
    }
    // line 64. Body written from the copy inlined into PowerAtDestination (Powers.cpp:3760); the
    // id stores are dead there, so their values are a guess.
    void Initialize() {
        idACDProjectile = ACDID_INVALID;
        idACDIntersected = ACDID_INVALID;
        eSpellEffect = SPELL_INVALID;
        vPosition = cgwvNull;
        vNormal = cgwvNull;
        bStop = TRUE;
    }
};
static_assert(sizeof(ImpactData) == 0x28, "ImpactData");

struct PowerAPIAnimation {  // size 0x58
    PowerID idPower;  // 0x00
    ACDID idACDSource;  // 0x04
    Target tTarget;  // 0x08
    int32 nCooldown;  // 0x24
    BOOL bSetAimTarget;  // 0x28
    BOOL bTurnToFaceTarget;  // 0x2c
    uint32 dwDuration;  // 0x30
    uint32 dwAnimationTag;  // 0x34
    WorldPlace wpDestination;  // 0x38
    float flSpeedScalar;  // 0x48
    PreplayedAnimData tPreplayedAnimData;  // 0x4c
};
static_assert(sizeof(PowerAPIAnimation) == 0x58, "PowerAPIAnimation");

typedef int32 HitSoundType;

typedef int32 PickupAction;

typedef int32 PayloadDeliveryMechanism;

struct PowerAPIDamage;
typedef int32 KnockbackStyle;

typedef int32 DeathReplacementType;

struct DamageRangeFloat {  // size 0x8
    float flMin;  // 0x00
    float flDelta;  // 0x04
};
static_assert(sizeof(DamageRangeFloat) == 0x8, "DamageRangeFloat");

enum AttackDirection {
    ATTACKDIRECTION_STRAIGHT = 0,
    ATTACKDIRECTION_LEFT = 1,
    ATTACKDIRECTION_RIGHT = 2,
};

class PowerAPIBuffStartData;
typedef void (*BuffStartCallback)(const PowerID, ACDID, void *, const PowerAPIDamage &, PowerAPIBuffStartData &);

struct PowerAPIAttackPayload {  // size 0x198
    PowerID idPower;  // 0x00
    PayloadDeliveryMechanism eDeliveryMechanism;  // 0x04
    SNO snoProjectileToStick;  // 0x08
    float flBaseDamageScalar;  // 0x0c
    float flPetDamageScalar;  // 0x10
    float flProcScalar;  // 0x14
    DamageRangeFloat arCombatDamage[7];  // 0x18
    float flDamageDurationInSecs[7];  // 0x50
    float flPrimaryAttributeBonusMultiplier;  // 0x6c
    float flClassBonusMultiplier;  // 0x70
    float flSkillsBonusMultiplier;  // 0x74
    DamageRangeFloat rangeStunLength;  // 0x78
    DamageRangeFloat rangeFearLength;  // 0x80
    DamageRangeFloat rangeFreezeLength;  // 0x88
    float flFreezeBreakChance;  // 0x90
    DamageRangeFloat rangeWebLength;  // 0x94
    DamageRangeFloat rangeImmobolizeLength;  // 0x9c
    DamageRangeFloat rangeSlowLength;  // 0xa4
    DamageRangeFloat rangeBlindLength;  // 0xac
    BOOL bBlindBreaksOnDamage;  // 0xb4
    float flSlowAmount;  // 0xb8
    DamageRangeFloat rangeManaDrain;  // 0xbc
    BOOL bChill;  // 0xc4
    float flChillMagnitude;  // 0xc8
    DamageRangeFloat rangeChillLength;  // 0xcc
    SNO snoTargetImpactParticle;  // 0xd4
    SNO snoTargetImpactEffectGroup;  // 0xd8
    SNO snoTargetAttackedParticle;  // 0xdc
    DeathReplacementType eDeathReplacementType;  // 0xe0
    BOOL bAlwaysHits;  // 0xe4
    BOOL bForcedPhysical;  // 0xe8
    BOOL bCanCrit;  // 0xec
    float flPowerAdditionalChanceToCriticalHit;  // 0xf0
    float flPowerCritDamageBonusPercent;  // 0xf4
    float flCritCap;  // 0xf8
    float flChanceToStun;  // 0xfc
    float flPowerAdditionalChanceToCrushingBlow;  // 0x100
    KnockbackStyle eKnockBack;  // 0x104
    Vector3D wvKnockbackDirection;  // 0x108
    WorldDistance wdKnockbackMagnitude;  // 0x114
    float flKnockbackHeightMin;  // 0x118
    float flKnockbackHeightDelta;  // 0x11c
    float flKnockbackGravityMin;  // 0x120
    float flKnockbackGravityDelta;  // 0x124
    BOOL bKnockbackIgnoresWeight;  // 0x128
    WorldPlace tKnockbackPlace;  // 0x12c
    SNO snoKnockbackPower;  // 0x13c
    BOOL bKnockbackAffectedByReduction;  // 0x140
    BOOL bContinuousKnockbackPower;  // 0x144
    BOOL bKnockdown;  // 0x148
    BOOL bAlwaysCauseGetHit;  // 0x14c
    BOOL bNeverCauseGetHit;  // 0x150
    BOOL bCausesAmbientDamageEffects;  // 0x154
    BOOL bIgnoreDamageBonusAndReduction;  // 0x158
    AttackDirection eAttackDirection;  // 0x15c
    int32 nDOTMaxStackCount;  // 0x160
    BuffStackType eDOTStackType;  // 0x164
    BuffStartCallback pDOTBuffStartCallback;  // 0x168
    void * pDOTBuffStartData;  // 0x170
    ACDID idACDFloatingCombatTextSourceOverride;  // 0x178
    BOOL bTriggersHitProcs;  // 0x17c
    BOOL bIgnoreCharmDamageScalar;  // 0x180
    BOOL bSplashDamage;  // 0x184
    BOOL bDisableHitEffects;  // 0x188
    BOOL bUseHighestElementalDamageBonus;  // 0x18c
    BOOL bIsAOEThorns;  // 0x190
    BOOL bForceCriticalDeath;  // 0x194
};
static_assert(sizeof(PowerAPIAttackPayload) == 0x198, "PowerAPIAttackPayload");

struct PowerAPIDamage {  // size 0xe8
    PowerID idPower;  // 0x00
    PayloadDeliveryMechanism eDeliveryMechanism;  // 0x04
    BOOL bCriticalHit;  // 0x08
    BOOL bCrushingBlow;  // 0x0c
    BOOL bShattered;  // 0x10
    BOOL bForceCriticalDeath;  // 0x14
    BOOL bDamageWasBlocked;  // 0x18
    BOOL bCausesTransform;  // 0x1c
    BOOL bPhysical;  // 0x20
    BOOL bWasImmune;  // 0x24
    BOOL bWasChill;  // 0x28
    ACDID idACDMainActor;  // 0x2c
    ACDID idACDSource;  // 0x30
    ACDID idACDTarget;  // 0x34
    ACDID idACDFloatingCombatTextSourceOverride;  // 0x38
    DamageList listDamage;  // 0x40
    HitSoundType eHitsound;  // 0x68
    float flDamageBlocked;  // 0x6c
    BOOL bProcessedShields;  // 0x70
    KnockbackStyle eKnockBack;  // 0x74
    Vector3D wvKnockbackDirection;  // 0x78
    WorldDistance wdKnockbackMagnitude;  // 0x84
    float flKnockbackHeight;  // 0x88
    float flKnockbackGravity;  // 0x8c
    BOOL bKnockbackIgnoresWeight;  // 0x90
    WorldPlace tKnockbackPlace;  // 0x94
    SNO snoKnockbackPower;  // 0xa4
    BOOL bContinuousKnockbackPower;  // 0xa8
    BOOL bKnockdown;  // 0xac
    BOOL bAlwaysCauseGetHit;  // 0xb0
    BOOL bNeverCauseGetHit;  // 0xb4
    AttackDirection eAttackDirection;  // 0xb8
    SNO snoAltDamageSourcePower;  // 0xbc
    DeathReplacementType eDeathReplacementType;  // 0xc0
    float flProcScalar;  // 0xc4
    BOOL bTriggersHitProcs;  // 0xc8
    BOOL bIgnoreDamageBonusAndReduction;  // 0xcc
    BOOL bDisableHitEffects;  // 0xd0
    BOOL bIsStaggerDamage;  // 0xd4
    BOOL bSplashDamage;  // 0xd8
    BOOL bIsAOEThorns;  // 0xdc
    BOOL bDontTriggerThreatListUpdate;  // 0xe0
};
static_assert(sizeof(PowerAPIDamage) == 0xe8, "PowerAPIDamage");

struct PowerAPIDestroyProjectile {  // size 0xc
    PowerID idPower;  // 0x00
    ACDID idACDProjectile;  // 0x04
    ACDID idACDIntersected;  // 0x08
};
static_assert(sizeof(PowerAPIDestroyProjectile) == 0xc, "PowerAPIDestroyProjectile");

struct PowerAPIBuffInstance;
typedef int32 TimerID;  // X/Server/Timers.h

typedef void (*OnPeriodicEffectCallback)(PowerAPIBuffInstance *, void *);
typedef void (*OnDoDamageEffectCallback)(PowerAPIBuffInstance *, PowerAPIDamage &, float *, ACDID, ACDID);
typedef void (*OnGetHitEffectCallback)(PowerAPIBuffInstance *, void *, PowerAPIDamage &, float, ACDID, ACDID);
typedef void (*OnHitEffectCallback)(PowerAPIBuffInstance *, void *, PowerAPIDamage &, float, float *, ACDID, ACDID, ACDID);
typedef void (*OnFirstHitAttemptEffectCallback)(PowerAPIBuffInstance *, void *, ACDID, ACDID, PowerID, SNO);
typedef void (*OnAttackEffectCallback)(PowerAPIBuffInstance *, void *, ACDID, ACDID, PowerID, SNO);
typedef void (*OnDodgeEffectCallback)(PowerAPIBuffInstance *, void *, ACDID, ACDID, const PowerAPIAttackPayload &);
typedef void (*OnCostEffectCallback)(PowerAPIBuffInstance *, void *, ACDID, PowerID, SNO, float, ResourceType, float, BOOL, float);
typedef void (*OnHealFromSkillEffectCallback)(PowerAPIBuffInstance *, ACDID, PowerID, SNO, float, float, int32);
typedef void (*DamageShieldCallback)(PowerAPIBuffInstance *, ACDID, float *, PowerAPIDamage &, void *);
typedef void (*OnKillCallback)(PowerAPIBuffInstance *, void *, float *, PowerAPIDamage *, ACDID, ACDID);
typedef void (*OnAssistCallback)(PowerAPIBuffInstance *, void *, ACDID, ACDID);
typedef void (*OnItemPickUpCallback)(PowerAPIBuffInstance *, void *, ACDID, ACDID, PickupAction);
typedef void (*OnDeathCallback)(PowerAPIBuffInstance *, void *, ACDID, ACDID);
typedef void (*OnExpirationCallback)(PowerAPIBuffInstance *, ACDID, void *);
typedef void (*OnRemovalCallback)(PowerAPIBuffInstance *, BuffRemovalReason, void *);
typedef BOOL (*OnReflectCallback)(PowerAPIBuffInstance *, ImpactData *, void *);
typedef void (*OnProjectileCreateCallback)(PowerAPIBuffInstance *, void *, ACDID, ACDID);
typedef void (*OnKnockbackLandCallback)(PowerAPIBuffInstance *);
typedef void (*OnPetAddCallback)(PowerAPIBuffInstance *, ACDID, ACDID, PetType, SNO);
typedef void (*OnResourceGainCallback)(PowerAPIBuffInstance *, float);
typedef void (*OnGetHitWithCrowdControlCallback)(PowerID, PowerAPIBuffInstance *, ACDID, ACDID, SNO, CrowdControlType, float, float);
typedef void (*OnHitWithCrowdControlCallback)(PowerID, PowerAPIBuffInstance *, ACDID, ACDID, SNO, CrowdControlType, float, float);
typedef void (*OnEquipmentChangeCallback)(PowerAPIBuffInstance *, ACDID, InventorySlotType, EquipmentChangeReason);
typedef void (*OnSkillChangeCallback)(PowerAPIBuffInstance *, ACDID);
typedef void (*OnMessageCallback)(PowerAPIBuffInstance *, ACDID, int32, int32, float);
typedef void (*OnCalloutEventCallback)(PowerAPIBuffInstance *, ACDID, CalloutEventType);
typedef void (*OnGizmoOperatedCallback)(PowerAPIBuffInstance *, ACDID, ActorCommonData &);
typedef void (*OnWreckableDestroyedCallback)(PowerAPIBuffInstance *, ACDID, ACDID);
typedef void (*OnTeleportedCallback)(PowerAPIBuffInstance *, ACDID);
typedef void (*OnPowerDamageInitCallback)(PowerAPIBuffInstance *, ACDID, ACDID, PowerAPIDamage &);
typedef void (*OnLootDropCallback)(PowerAPIBuffInstance *, ACDID, ActorCommonData &, LootContext);
typedef void (*OnItemSoldCallback)(PowerAPIBuffInstance *, ACDID, ActorCommonData &);
typedef void (*OnLevelCallback)(PowerAPIBuffInstance *, ACDID);
typedef void (*OnCorpseConsumedCallback)(PowerAPIBuffInstance *, ACDID, ACDID);
typedef void (*OnPetDismissCallback)(PowerAPIBuffInstance *, ACDID, ACDID);

class PowerAPIBuffStartData {  // size 0x298
public:
    PowerAPIBuffStartData();
    PowerAPIBuffStartData(PowerID);
    ~PowerAPIBuffStartData();
    PowerID idPower;  // 0x00
    SNO snoPower;  // 0x04
    ACDID idACDSource;  // 0x08
    int32 nBuffLevel;  // 0x0c
    BuffStackType eStackType;  // 0x10
    int32 nMaxStackCount;  // 0x14
    int32 nMaxInstanceCount;  // 0x18
    uint32 dwStartTick;  // 0x1c
    uint32 dwDurationMin;  // 0x20
    uint32 dwDurationDelta;  // 0x24
    uint32 dwRefreshRate;  // 0x28
    uint32 dwInternalCooldownDuration;  // 0x2c
    AttribGroupID idFastAttrib;  // 0x30
    OnPeriodicEffectCallback pPeriodicSpellEffect;  // 0x38
    void * pPeriodicSpellEffectData;  // 0x40
    OnDoDamageEffectCallback pOnDoDamageEffect;  // 0x48
    void * pOnDoDamageEffectData;  // 0x50
    OnGetHitEffectCallback pOnGetHitEffect;  // 0x58
    void * pOnGetHitEffectData;  // 0x60
    OnHitEffectCallback pOnHitEffect;  // 0x68
    void * pOnHitEffectData;  // 0x70
    OnFirstHitAttemptEffectCallback pOnFirstHitAttemptEffect;  // 0x78
    void * pOnFirstHitAttemptEffectData;  // 0x80
    OnAttackEffectCallback pOnAttackEffect;  // 0x88
    void * pOnAttackEffectData;  // 0x90
    OnDodgeEffectCallback pOnDodgeEffect;  // 0x98
    void * pOnDodgeEffectData;  // 0xa0
    OnCostEffectCallback pOnCostEffect;  // 0xa8
    void * pOnCostEffectData;  // 0xb0
    OnHealFromSkillEffectCallback pOnHealFromSkillEffect;  // 0xb8
    OnKillCallback pOnKillEffect;  // 0xc0
    void * pOnKillEffectData;  // 0xc8
    OnAssistCallback pOnAssistEffect;  // 0xd0
    void * pOnAssistEffectData;  // 0xd8
    OnItemPickUpCallback pOnItemPickUpEffect;  // 0xe0
    void * pOnItemPickUpEffectData;  // 0xe8
    DamageShieldCallback pDamageShieldEffect;  // 0xf0
    void * pDamageShieldEffectData;  // 0xf8
    int32 nDamageShieldPriority;  // 0x100
    OnExpirationCallback pExpirationEffect;  // 0x108
    void * pExpirationEffectData;  // 0x110
    OnRemovalCallback pRemovalEffect;  // 0x118
    void * pRemovalEffectData;  // 0x120
    OnDeathCallback pDeathEffect;  // 0x128
    void * pDeathEffectData;  // 0x130
    OnReflectCallback pReflectEffect;  // 0x138
    OnReflectCallback pShouldReflectTest;  // 0x140
    void * pReflectEffectData;  // 0x148
    OnProjectileCreateCallback pOnProjectileCreateEffect;  // 0x150
    void * pOnProjectileCreateEffectData;  // 0x158
    OnKnockbackLandCallback pOnKnockbackLandEffect;  // 0x160
    OnPetAddCallback pOnPetAddEffect;  // 0x168
    void * pOnPetAddEffectData;  // 0x170
    OnResourceGainCallback pOnResourceGainEffect;  // 0x178
    OnGetHitWithCrowdControlCallback pOnGetHitWithCrowdControlEffect;  // 0x180
    void * pOngetHitwithCrowdControlData;  // 0x188
    BOOL bOnGetHitWithCrowdControlCallOnRefresh;  // 0x190
    OnHitWithCrowdControlCallback pOnHitWithCrowdControlEffect;  // 0x198
    BOOL bOnHitWithCrowdControlCallOnRefresh;  // 0x1a0
    OnEquipmentChangeCallback pOnEquipmentChangeEffect;  // 0x1a8
    OnSkillChangeCallback pOnSkillChangeEffect;  // 0x1b0
    OnMessageCallback pOnMessageEffect;  // 0x1b8
    OnCalloutEventCallback pOnCalloutEventEffect;  // 0x1c0
    OnGizmoOperatedCallback pOnGizmoOperatedEffect;  // 0x1c8
    OnWreckableDestroyedCallback pOnWreckableDestroyedEffect;  // 0x1d0
    OnTeleportedCallback pOnTeleportedEffect;  // 0x1d8
    OnPowerDamageInitCallback pOnPowerDamageInitEffect;  // 0x1e0
    OnLootDropCallback pOnLootDropEffect;  // 0x1e8
    OnItemSoldCallback pOnItemSoldEffect;  // 0x1f0
    OnLevelCallback pOnLevelEffect;  // 0x1f8
    OnCorpseConsumedCallback pOnCorpseConsumedEffect;  // 0x200
    OnPetDismissCallback pOnPetDismissEffect;  // 0x208
    int32 nBuffIndex;  // 0x210
    int32 nBuffData;  // 0x214
    float flBuffData;  // 0x218
    Attrib eAppearanceAttribute;  // 0x21c
    int32 nAppearanceParameter;  // 0x220
    SNO snoEndingEffect;  // 0x224
    uint32 nLookOverrideHash;  // 0x228
    DamageType eDamageType;  // 0x22c
    float flDamagePerSecond;  // 0x230
    float flPrimaryAttributeBonusMultiplier;  // 0x234
    float flClassBonusMultiplier;  // 0x238
    float flSkillsBonusMultiplier;  // 0x23c
    SNO snoDOTDamageSourcePower;  // 0x240
    float flPetDamageScalar;  // 0x244
    float flProcScalar;  // 0x248
    float flSlowAmount;  // 0x24c
    float flAttackSpeedReductionAmount;  // 0x250
    float flChillMagnitude;  // 0x254
    bool bPositiveEffect : 1;  // 0x258
    bool bRemoveOnDeath : 1;  // 0x258
    bool bRemoveOnRespec : 1;  // 0x258
    bool bRemoveOnPassiveApplication : 1;  // 0x258
    bool bRemoveOnGreaterRiftEntry : 1;  // 0x258
    bool bKeepForProfileSnapshot : 1;  // 0x258
    bool bTriggersHitProcs : 1;  // 0x258
    bool bUndispellable : 1;  // 0x258
    bool bDoNotCopy : 1;  // 0x259
    bool bIsCopy : 1;  // 0x259
    bool bHitsOnFirstTick : 1;  // 0x259
    bool bTriggerOnHitForPets : 1;  // 0x259
    bool bTriggerOnDamageForPets : 1;  // 0x259
    bool bProcessDamageShieldOnStaggerDamage : 1;  // 0x259
    bool bInfinite : 1;  // 0x259
    bool bDamageShieldIsEvaluatedLast : 1;  // 0x259
    bool bAppliedDamageReductionEffects : 1;  // 0x25a
    bool bDOTCrit : 1;  // 0x25a
    bool bDOTCanCrit : 1;  // 0x25a
    bool bHideDOTDamageEffects : 1;  // 0x25a
    bool bUsePowersProcScalar : 1;  // 0x25a
    bool bStun : 1;  // 0x25a
    bool bFear : 1;  // 0x25a
    bool bFreeze : 1;  // 0x25a
    bool bImmobilize : 1;  // 0x25b
    bool bBlind : 1;  // 0x25b
    bool bCharm : 1;  // 0x25b
    bool bTaunt : 1;  // 0x25b
    bool bSlow : 1;  // 0x25b
    bool bForceGrip : 1;  // 0x25b
    bool bAttackSpeedReduction : 1;  // 0x25b
    bool bChill : 1;  // 0x25b
    bool bFailsIfStunImmune : 1;  // 0x25c
    bool bFailsIfFearImmune : 1;  // 0x25c
    bool bFailsIfFreezeImmune : 1;  // 0x25c
    bool bFailsIfForceGripImmune : 1;  // 0x25c
    bool bFailsIfImmobilizeImmune : 1;  // 0x25c
    bool bFailsIfBlindImmune : 1;  // 0x25c
    bool bFailsIfCharmImmune : 1;  // 0x25c
    bool bFailsIfSlowImmune : 1;  // 0x25c
    bool bFailsIfAttackSpeedReductionImmune : 1;  // 0x25d
    bool bFailsIfChillImmune : 1;  // 0x25d
    bool bAura : 1;  // 0x25d
    bool bChanneled : 1;  // 0x25d
    bool bUsesLockout : 1;  // 0x25d
    bool bUseHighestElementalDamageBonus : 1;  // 0x25d
    bool bSaveCooldown : 1;  // 0x25d
    bool bSuppressTauntEffects : 1;  // 0x25d
    float flShieldAmount;  // 0x260
    int32 nAuraPriority;  // 0x264
    XPooledDataIDList listAIStatesToMerge;  // 0x268
    BuffExclusiveType eExclusiveType;  // 0x290
    void Initialize();
    void InitializeFromBuffID(BuffID);
    BOOL IsInitialized() const { return m_bInitialized; }  // Original PowerAPI.h:1136
private:
    void _Initialize(PowerID _idPower);
    BOOL m_bInitialized;  // 0x294
};
static_assert(sizeof(PowerAPIBuffStartData) == 0x298, "PowerAPIBuffStartData");

struct PowerAPIBuffInstance {  // size 0x2e0
    BuffID id;  // 0x00
    BOOL bDeleted;  // 0x04
    BOOL bDoNotCopy;  // 0x08
    BOOL bUndispellable;  // 0x0c
    BOOL bIsCopy;  // 0x10
    ACDID idACDAffected;  // 0x14
    PowerID idPower;  // 0x18
    ACDID idACDSource;  // 0x1c
    SNO snoPower;  // 0x20
    BOOL bPositiveEffect;  // 0x24
    BOOL bRemoveOnDeath;  // 0x28
    BOOL bRemoveOnRespec;  // 0x2c
    BOOL bRemoveOnPassiveApplication;  // 0x30
    BOOL bRemoveOnGreaterRiftEntry;  // 0x34
    BOOL bKeepForProfileSnapshot;  // 0x38
    BOOL bTriggersHitProcs;  // 0x3c
    BOOL bAppliesOnlyToSpecificPowers;  // 0x40
    int32 nBuffLevel;  // 0x44
    BuffStackType eStackType;  // 0x48
    BOOL bHitsOnFirstTick;  // 0x4c
    BOOL bTriggerOnHitForPets;  // 0x50
    BOOL bTriggerOnDamageForPets;  // 0x54
    BOOL bProcessDamageShieldOnStaggerDamage;  // 0x58
    BOOL bSaveCooldown;  // 0x5c
    BOOL bAppliedDamage;  // 0x60
    int32 nMaxStackCount;  // 0x64
    uint32 dwStartTick;  // 0x68
    uint32 dwInitialInstanceStartTick;  // 0x6c
    uint32 dwDuration;  // 0x70
    uint32 dwRefreshRate;  // 0x74
    uint32 dwInternalCooldownDuration;  // 0x78
    uint32 dwInternalCooldownReadyTick;  // 0x7c
    uint32 uLastDOTTick;  // 0x80
    BOOL bInfinite;  // 0x84
    AttribGroupID idFastAttrib;  // 0x88
    OnPeriodicEffectCallback pPeriodicSpellEffect;  // 0x90
    void * pPeriodicSpellEffectData;  // 0x98
    OnDoDamageEffectCallback pOnDoDamageEffect;  // 0xa0
    void * pOnDoDamageEffectData;  // 0xa8
    OnGetHitEffectCallback pOnGetHitEffect;  // 0xb0
    void * pOnGetHitEffectData;  // 0xb8
    OnHitEffectCallback pOnHitEffect;  // 0xc0
    void * pOnHitEffectData;  // 0xc8
    OnFirstHitAttemptEffectCallback pOnFirstHitAttemptEffect;  // 0xd0
    void * pOnFirstHitAttemptEffectData;  // 0xd8
    OnAttackEffectCallback pOnAttackEffect;  // 0xe0
    void * pOnAttackEffectData;  // 0xe8
    OnDodgeEffectCallback pOnDodgeEffect;  // 0xf0
    void * pOnDodgeEffectData;  // 0xf8
    OnCostEffectCallback pOnCostEffect;  // 0x100
    void * pOnCostEffectData;  // 0x108
    OnHealFromSkillEffectCallback pOnHealFromSkillEffect;  // 0x110
    OnKillCallback pOnKillEffect;  // 0x118
    void * pOnKillEffectData;  // 0x120
    OnAssistCallback pOnAssistEffect;  // 0x128
    void * pOnAssistEffectData;  // 0x130
    OnItemPickUpCallback pOnItemPickUpEffect;  // 0x138
    void * pOnItemPickUpEffectData;  // 0x140
    DamageShieldCallback pDamageShieldEffect;  // 0x148
    void * pDamageShieldEffectData;  // 0x150
    BOOL bDamageShieldIsEvaluatedLast;  // 0x158
    int32 nDamageShieldPriority;  // 0x15c
    OnExpirationCallback pExpirationEffect;  // 0x160
    void * pExpirationEffectData;  // 0x168
    OnRemovalCallback pRemovalEffect;  // 0x170
    void * pRemovalEffectData;  // 0x178
    OnDeathCallback pDeathEffect;  // 0x180
    void * pDeathEffectData;  // 0x188
    OnReflectCallback pShouldReflectTest;  // 0x190
    OnReflectCallback pReflectEffect;  // 0x198
    void * pReflectEffectData;  // 0x1a0
    OnProjectileCreateCallback pOnProjectileCreateEffect;  // 0x1a8
    void * pOnProjectileCreateEffectData;  // 0x1b0
    OnKnockbackLandCallback pOnKnockbackLandEffect;  // 0x1b8
    OnPetAddCallback pOnPetAddEffect;  // 0x1c0
    void * pOnPetAddEffectData;  // 0x1c8
    OnResourceGainCallback pOnResourceGainEffect;  // 0x1d0
    OnGetHitWithCrowdControlCallback pOnGetHitWithCrowdControlEffect;  // 0x1d8
    void * pOnGetHitwithCrowdControlData;  // 0x1e0
    BOOL bOnGetHitWithCrowdControlCallOnRefresh;  // 0x1e8
    OnHitWithCrowdControlCallback pOnHitWithCrowdControlEffect;  // 0x1f0
    BOOL bOnHitWithCrowdControlCallOnRefresh;  // 0x1f8
    OnEquipmentChangeCallback pOnEquipmentChangeEffect;  // 0x200
    OnSkillChangeCallback pOnSkillChangeEffect;  // 0x208
    OnMessageCallback pOnMessageEffect;  // 0x210
    OnCalloutEventCallback pOnCalloutEventEffect;  // 0x218
    OnGizmoOperatedCallback pOnGizmoOperatedEffect;  // 0x220
    OnWreckableDestroyedCallback pOnWreckableDestroyedEffect;  // 0x228
    OnTeleportedCallback pOnTeleportedEffect;  // 0x230
    OnPowerDamageInitCallback pOnPowerDamageInitEffect;  // 0x238
    OnLootDropCallback pOnLootDropEffect;  // 0x240
    OnItemSoldCallback pOnItemSoldEffect;  // 0x248
    OnLevelCallback pOnLevelEffect;  // 0x250
    OnCorpseConsumedCallback pOnCorpseConsumedEffect;  // 0x258
    OnPetDismissCallback pOnPetDismissEffect;  // 0x260
    FastAttribKey tKeyAppearance;  // 0x268
    int32 nBuffIndex;  // 0x26c
    int32 nBuffData;  // 0x270
    float flBuffData;  // 0x274
    int32 nLookOverrideHash;  // 0x278
    SNO snoEndingEffect;  // 0x27c
    TimerID idTimerDone;  // 0x280
    TimerID idTimerPeriodicEffect;  // 0x284
    float flShieldAmount;  // 0x288
    BOOL bIsAura;  // 0x28c
    int32 nAuraPriority;  // 0x290
    BOOL bIsChanneled;  // 0x294
    BOOL bUsesLockout;  // 0x298
    XPooledDataIDList listAIStatesToMerge;  // 0x2a0
    BuffExclusiveType eExclusiveType;  // 0x2c8
    DamageType eDamageType;  // 0x2cc
    float flDamagePerSecond;  // 0x2d0
    SNO snoDOTDamageSourcePower;  // 0x2d4
    BOOL bDOTCrit;  // 0x2d8
    float flProcScalar;  // 0x2dc
};
static_assert(sizeof(PowerAPIBuffInstance) == 0x2e0, "PowerAPIBuffInstance");

// ---------------------------------------------------------------------------
// ChooseTargets types, used by Enchantress_Disorient
// (_Z21Enchantress_DisorientiPv, 0x71009693a0, Power_Enchantress.cpp:53-56).
// Layout taken from the DWARF (build/dwarf/headers/X/Server/Powers/PowerAPI.h).
//   ChooseTargetGeometry::ChooseTargetGeometry (PowerAPI.h:476) constructs
//     tSingleTarget (:428), tMelee (:435), tPie (:442); each holds one Target
//     (Target.h:31) whose Target::Clear lands at +0x50 / +0x74 / +0xa4.
//   PowerAPIChooseTargets ctor (PowerAPI.h:500) clears tGeometry (0x134 bytes)
//     and dwItemLabelMask (0x14 bytes) at PowerAPI.h:502-503.
// ---------------------------------------------------------------------------

enum ChooseTargetsGeometryType {
    CTT_SPHERE = 0,
    CTT_SINGLE_TARGET = 1,
    CTT_RAY = 2,
    CTT_AOE = 3,
    CTT_MELEE = 4,
    CTT_NO_LINE_OF_SIGHT = 5,
    CTT_FRONT = 6,
    CTT_BACK = 7,
    CTT_PIE = 8,
    CTT_CAPSULE = 9,
    CTT_BOX = 10,
    CTT_CYLINDER = 11,
};

struct ChooseTargetGeometrySphere {  // size 0x14
    WorldPlace placeCenter;  // 0x00
    WorldDistance wdRadius;  // 0x10
};
static_assert(sizeof(ChooseTargetGeometrySphere) == 0x14, "ChooseTargetGeometrySphere");

struct ChooseTargetGeometryRay {  // size 0x20
    WorldPlace placeStart;  // 0x00
    WorldPlace placeEnd;  // 0x10
};
static_assert(sizeof(ChooseTargetGeometryRay) == 0x20, "ChooseTargetGeometryRay");

struct ChooseTargetGeometrySingleTarget {  // size 0x30
    WorldPlace placeSource;  // 0x00
    Target tTarget;  // 0x10
    WorldDistance wdSearchRadius;  // 0x2c
};
static_assert(sizeof(ChooseTargetGeometrySingleTarget) == 0x30, "ChooseTargetGeometrySingleTarget");

struct ChooseTargetGeometryMelee {  // size 0x24
    ACDID idACDSource;  // 0x00
    Target tTarget;  // 0x04
    int32 nMeleeRange;  // 0x20
};
static_assert(sizeof(ChooseTargetGeometryMelee) == 0x24, "ChooseTargetGeometryMelee");

struct ChooseTargetGeometryPie {  // size 0x44
    WorldPlace placeCenter;  // 0x00
    Target tTarget;  // 0x10
    WorldDistance wdRadius;  // 0x2c
    WorldDistance wdHeight;  // 0x30
    Vector3D wvPieVector;  // 0x34
    int32 nDegrees;  // 0x40
};
static_assert(sizeof(ChooseTargetGeometryPie) == 0x44, "ChooseTargetGeometryPie");

struct ChooseTargetGeometryCapsule {  // size 0x28
    WorldPlace place0;  // 0x00
    Vector3D vDir;  // 0x10
    float flLength;  // 0x1c
    float flRadius;  // 0x20
    float flBonusHeight;  // 0x24
};
static_assert(sizeof(ChooseTargetGeometryCapsule) == 0x28, "ChooseTargetGeometryCapsule");

struct ChooseTargetGeometryBox {  // size 0x24
    WorldPlace placeCenter;  // 0x00
    Vector3D vDir;  // 0x10
    float flLength;  // 0x1c
    float flWidth;  // 0x20
};
static_assert(sizeof(ChooseTargetGeometryBox) == 0x24, "ChooseTargetGeometryBox");

struct ChooseTargetGeometryCylinder {  // size 0x18
    WorldPlace place0;  // 0x00
    float flRadius;  // 0x10
    float flHeight;  // 0x14
};
static_assert(sizeof(ChooseTargetGeometryCylinder) == 0x18, "ChooseTargetGeometryCylinder");

struct ChooseTargetGeometry {  // size 0x134
    ChooseTargetsGeometryType eType;  // 0x00
    ChooseTargetGeometrySphere tSphere;  // 0x04
    ChooseTargetGeometryRay tRay;  // 0x18
    ChooseTargetGeometrySingleTarget tSingleTarget;  // 0x38
    ChooseTargetGeometryMelee tMelee;  // 0x68
    ChooseTargetGeometryPie tPie;  // 0x8c
    ChooseTargetGeometryCapsule tCapsule;  // 0xd0
    ChooseTargetGeometryBox tBox;  // 0xf8
    ChooseTargetGeometryCylinder tCylinder;  // 0x11c
};
static_assert(sizeof(ChooseTargetGeometry) == 0x134, "ChooseTargetGeometry");

enum ChooseTargetFlags {
    CTF_CLOSEST_ONLY_BIT = 0,
    CTF_IGNORE_ON_AFFECTED_LIST_BIT = 1,
    CTF_DONT_PERFORM_LOS_CHECK = 2,
    CTF_USE_MELEE_SNAPSHOT = 3,
    CTF_IGNORE_STEALTHED = 4,
    CTF_PERFORM_PROJECTILE_LOS_CHECK = 5,
    CTF_TARGET_ITEMS_ONLY = 6,
    CTF_FILTER_ITEMS_BY_LABEL = 7,
};

struct PowerAPIChooseTargets {  // size 0x154
    PowerAPIChooseTargets()
        : idPower(POWER_NONE)
        , dwFlags(0)
        , ePassabilityType(2)
    {
        SigmaMemoryClear(&tGeometry, sizeof(tGeometry));  // PowerAPI.h:502
        SigmaMemoryClear(dwItemLabelMask, sizeof(dwItemLabelMask));  // PowerAPI.h:503
    }
    PowerID idPower;  // 0x00
    uint32 dwFlags;  // 0x04
    ChooseTargetGeometry tGeometry;  // 0x08
    PassabilityType ePassabilityType;  // 0x13c
    uint32 dwItemLabelMask[5];  // 0x140
};
static_assert(sizeof(PowerAPIChooseTargets) == 0x154, "PowerAPIChooseTargets");

void PowerAPI_DoAnimation(const PowerAPIAnimation & tAnimation);
void PowerAPI_DestroyProjectile(const PowerAPIDestroyProjectile & tProjectile);
BOOL PowerAPI_ChanneledSpellDoContinualCosts(PowerID idPower, float fResourceCostModifier);  // PowerAPI.cpp:7353
int32 PowerAPI_HealthCostsForTargets(PowerID idPower, int32 nNumTargets);  // PowerAPI.cpp:7448
BuffID PowerAPI_BuffAddToACD(const PowerAPIBuffStartData & tBuffStartData, ActorCommonData & tACD);  // PowerAPI.cpp:8504

// Called from X/Server/Powers/Power_Enchantress.cpp (_Z21Enchantress_DisorientiPv).
// Manglings:
//   _Z24PowerAPI_DoChooseTargetsRK21PowerAPIChooseTargetsR11XPooledListIiiE
//   _Z18PowerAPI_DoPayloadRK21PowerAPIAttackPayloadR11XPooledListIiiE5ACDID
void PowerAPI_DoChooseTargets(const PowerAPIChooseTargets & tTargets, XPooledList<int, int> & listTargets);  // PowerAPI.cpp
int32 PowerAPI_DoPayload(const PowerAPIAttackPayload & tPayload, XPooledDataIDList & listTargets, const ACDID idACDOverrideAttacker);  // PowerAPI.cpp:4873

PowerAPIBuffInstance * PowerAPI_BuffTryToGet(BuffID idBuff);
void PowerAPI_ResetCooldowns(ActorCommonData & tACD);  // PowerAPI.cpp:2119
void PowerAPI_DoBurrowIn(ActorCommonData * ptACDTarget, BOOL bInvisibleDuring, WeaponClass eBurrowWeaponClass, int32 nAppearanceParameter, BOOL bAlphaAttachments);
void PowerAPI_FreeFromRoot(ACDID idACD);
void PowerAPI_FreeFromStun(ACDID idACD);
void PowerAPI_FreeFromFear(ACDID idACD);
void PowerAPI_FreeFromSnare(ACDID idACD);
uint32 PowerAPI_BuffModifyDuration(BuffID idBuff, int32 nDeltaTicks, BOOL bWantInfinite, BOOL bResetStartTick);  // PowerAPI.cpp:11139
void PowerAPI_BuffApplyOnMessageEffects(ACDID idACDCaster, SNO snoTargetPower, int32 nBuffIndex, int32 dwParam0, int32 dwParam1, float flParam2);  // PowerAPI.cpp:11914
void PowerAPI_BuffFlagForDeletion(BuffID idBuff, BOOL bPlayEndEffect, BOOL bUpdateAuras, BuffRemovalReason eBuffRemovalReason, BOOL bDealPendingDOTDamage);

static_assert(__builtin_offsetof(PowerAPIBuffInstance, bRemoveOnRespec) == 0x2c, "PowerAPIBuffInstance::bRemoveOnRespec");

static_assert(__builtin_offsetof(PowerAPIBuffInstance, bRemoveOnPassiveApplication) == 0x30, "PowerAPIBuffInstance::bRemoveOnPassiveApplication");

static_assert(__builtin_offsetof(PowerAPIBuffInstance, dwStartTick) == 0x68, "PowerAPIBuffInstance::dwStartTick");

// Original PowerAPI.cpp:10745; the pointer overload is a separate owner function.
void PowerAPI_BuffFlagForDeletion(PowerAPIBuffInstance * ptBuff, BOOL bPlayEndEffect, BOOL bUpdateAuras, BuffRemovalReason eBuffRemovalReason, BOOL bDealPendingDOTDamage);
void PowerAPI_BuffSetEndTick(BuffID idBuff, uint32 dwEndTick);  // PowerAPI.cpp:11104

// Recovered PowerAPI.cpp buff query, cleanup and callback entry points.
BuffID PowerAPI_BuffFindOnACD(SNO snoPower, const ActorCommonData & tACD, ACDID idACDSource);
BuffID PowerAPI_BuffFindOnACD(SNO snoPower, int32 nBuffIndex, const ActorCommonData & tACD, ACDID idACDSource, BOOL bAllowDeleted);
BuffID PowerAPI_BuffFindOnACD(SNO snoPower, const ActorCommonData * ptACD, ACDID idACDSource);
BuffID PowerAPI_BuffFindOnACDByPowerID(SNO snoPower, const ActorCommonData * ptACD, PowerID idPower);
BuffID PowerAPI_BuffFindOnACDByPowerID(SNO snoPower, int32 nBuffIndex, const ActorCommonData * ptACD, PowerID idPower, ACDID idACDSource, BOOL bAllowDeleted);
BuffID PowerAPI_BuffFindOnACDByBuffData(SNO snoPower, int32 nBuffIndex, int32 nBuffData, const ActorCommonData & tACD, ACDID idACDSource, BOOL bAllowDeleted);
int32 PowerAPI_BuffCountOnACD(SNO snoPower, int32 nBuffIndex, ActorCommonData & tACD, const ACDID idACDSource);
BOOL PowerAPI_BuffHasLookswitchBuff(ActorCommonData & tACD);
void PowerAPI_BuffRemoveLookswitchBuffs(ActorCommonData & tACD);
void PowerAPI_BuffFlagAllForDeletion(ActorCommonData & tACD);
void PowerAPI_BuffRemoveAllNegativeBuffs(ActorCommonData & tACD);
void PowerAPI_BuffRemoveExclusiveType(ActorCommonData & tACD, BuffExclusiveType eExclusiveType);
void PowerAPI_BuffApplyOnAssistEffects(ACDID idACDAttacker, ACDID idACDKilled);
void PowerAPI_BuffApplyOnItemPickUpEffects(ACDID idACD, ACDID idACDItem, PickupAction ePickUpAction);
void PowerAPI_BuffApplyOnAttackEffects(ACDID idACDAttacker, ACDID idACDTarget, PowerID idPowerFrom, SNO snoPower);
void PowerAPI_BuffApplyOnProjectileCreateEffects(ACDID idACDAttacker, ACDID idACDProjectile);
void PowerAPI_BuffApplyOnKnockbackLandEffects(ACDID idACD);
void PowerAPI_BuffApplyOnEquipmentChangeEffects(ACDID idACDActor, InventorySlotType eSlot, EquipmentChangeReason eReason);
void PowerAPI_BuffApplyOnResourceGainEffects(ACDID idACD, float flFuryAdded);
void PowerAPI_BuffApplyOnSkillChangeEffects(ACDID idACDActor);
void PowerAPI_BuffApplyOnCalloutEventEffects(ACDID idACDActor, CalloutEventType eEvent);
void PowerAPI_BuffApplyOnGizmoOperatedEffects(ACDID idACDActor, ActorCommonData & tACDGizmo);
void PowerAPI_BuffApplyOnTeleportedEffects(ACDID idACDActor);
void PowerAPI_BuffApplyOnLootDropEffect(ACDID idACDPlayer, ActorCommonData & tACDItem, LootContext eLootContext);
void PowerAPI_BuffApplyOnItemSoldEffect(ACDID idACDPlayer, ActorCommonData & tACDItem);
void PowerAPI_BuffApplyOnLevelEffect(ActorCommonData & tACD);
void PowerAPI_BuffApplyOnCorpseConsumedEffect(ActorCommonData & tACD, ACDID idACDCorpse);
void PowerAPI_BuffApplyOnPetDismissEffect(ActorCommonData & tACD, ACDID idACDPlayer, ACDID idACDPet);
static_assert(__builtin_offsetof(PowerAPIBuffInstance, idACDSource) == 0x1c, "PowerAPIBuffInstance::idACDSource");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, snoPower) == 0x20, "PowerAPIBuffInstance::snoPower");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, bPositiveEffect) == 0x24, "PowerAPIBuffInstance::bPositiveEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, bUndispellable) == 0xc, "PowerAPIBuffInstance::bUndispellable");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, nBuffData) == 0x270, "PowerAPIBuffInstance::nBuffData");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, nLookOverrideHash) == 0x278, "PowerAPIBuffInstance::nLookOverrideHash");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, eExclusiveType) == 0x2c8, "PowerAPIBuffInstance::eExclusiveType");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnAttackEffect) == 0xe0, "PowerAPIBuffInstance::pOnAttackEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnAttackEffectData) == 0xe8, "PowerAPIBuffInstance::pOnAttackEffectData");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnAssistEffect) == 0x128, "PowerAPIBuffInstance::pOnAssistEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnAssistEffectData) == 0x130, "PowerAPIBuffInstance::pOnAssistEffectData");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnItemPickUpEffect) == 0x138, "PowerAPIBuffInstance::pOnItemPickUpEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnItemPickUpEffectData) == 0x140, "PowerAPIBuffInstance::pOnItemPickUpEffectData");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnProjectileCreateEffect) == 0x1a8, "PowerAPIBuffInstance::pOnProjectileCreateEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnProjectileCreateEffectData) == 0x1b0, "PowerAPIBuffInstance::pOnProjectileCreateEffectData");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnKnockbackLandEffect) == 0x1b8, "PowerAPIBuffInstance::pOnKnockbackLandEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnResourceGainEffect) == 0x1d0, "PowerAPIBuffInstance::pOnResourceGainEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnLootDropEffect) == 0x240, "PowerAPIBuffInstance::pOnLootDropEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnItemSoldEffect) == 0x248, "PowerAPIBuffInstance::pOnItemSoldEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnEquipmentChangeEffect) == 0x200, "PowerAPIBuffInstance::pOnEquipmentChangeEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnSkillChangeEffect) == 0x208, "PowerAPIBuffInstance::pOnSkillChangeEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnCalloutEventEffect) == 0x218, "PowerAPIBuffInstance::pOnCalloutEventEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnGizmoOperatedEffect) == 0x220, "PowerAPIBuffInstance::pOnGizmoOperatedEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnTeleportedEffect) == 0x230, "PowerAPIBuffInstance::pOnTeleportedEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnLevelEffect) == 0x250, "PowerAPIBuffInstance::pOnLevelEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnCorpseConsumedEffect) == 0x258, "PowerAPIBuffInstance::pOnCorpseConsumedEffect");
static_assert(__builtin_offsetof(PowerAPIBuffInstance, pOnPetDismissEffect) == 0x260, "PowerAPIBuffInstance::pOnPetDismissEffect");

static_assert(__builtin_offsetof(PowerAPIBuffStartData, pOnProjectileCreateEffect) == 0x150, "PowerAPIBuffStartData::pOnProjectileCreateEffect");

// line 38
enum AuraType {
    AURA_TYPE_MASTER_ONLY = 0,
    AURA_TYPE_SELF_AND_MASTER = 1,
    AURA_TYPE_ALL = 2,
    NUM_AURA_TYPES = 3,
};
typedef void (*AuraCallback)(PowerID, ACDID);
void PowerAPI_AuraSetupBuffStartData(PowerAPIBuffStartData & tBuffStartData, PowerID idPower);  // PowerAPI.cpp:13004
void PowerAPI_AuraStart(PowerID idPower, AuraCallback pCallback, AuraType eAuraType, BOOL bRemoveOnDeath);  // PowerAPI.cpp:13127
struct PowerAPIExplosion {  // size 0x28
    SNO snoExplosionActor;  // 0x00
    WorldPlace placeSource;  // 0x04
    Quaternion qOrientation;  // 0x14
    uint32 dwDuration;  // 0x24
};
static_assert(sizeof(PowerAPIExplosion) == 0x28, "PowerAPIExplosion");
void PowerAPI_DoExplosion(const PowerAPIExplosion & tExplosion);  // PowerAPI.cpp:6587

void PowerAPI_DoBurrowOut(ActorCommonData * ptACDTarget, BOOL bSetUsedSpecialSpawn);  // PowerAPI.cpp:13491
BOOL PowerAPI_DoPayload(const PowerAPIAttackPayload & tPayload, const ACDID idACDTarget, const ACDID idACDOverrideAttacker);  // PowerAPI.cpp:4137

struct PowerAPISummonActor {  // size 0x40
    PowerID idPower;  // 0x00
    SNO snoActor;  // 0x04
    int32 nActorLevel;  // 0x08
    WorldPlace placeSummonLocation;  // 0x0c
    ACDID idACDCorpse;  // 0x1c
    Quaternion qOrientation;  // 0x20
    uint32 dwAddFlags;  // 0x30
    uint32 dwAnimTag;  // 0x34
    Team eTeamOverride;  // 0x38
    BOOL bNoSpawnAnimPower;  // 0x3c
};
static_assert(sizeof(PowerAPISummonActor) == 0x40, "PowerAPISummonActor");
ACDID PowerAPI_SummonActor(const PowerAPISummonActor & tSummonInfo);  // PowerAPI.cpp:6616
struct PowerAPICreateProjectile {  // size 0xc0
    PowerID idPower;  // 0x00
    ProjectileData tProjectileData;  // 0x04
};
static_assert(sizeof(PowerAPICreateProjectile) == 0xc0, "PowerAPICreateProjectile");
ACDID PowerAPI_DoProjectile(PowerAPICreateProjectile & tProjectileInfo);  // PowerAPI.cpp:4991
enum {  // original PowerAPI.h:398
    BUFFID_INVALID = -1,
};
void PowerAPI_BuffFlagAllFromPowerForDeletion(ActorCommonData & tACD, SNO snoPower, int32 nBuffIndex, BuffRemovalReason eRemovalReason, ACDID idActorSource);  // PowerAPI.cpp:10989

int32 PowerAPI_GetSummonMaxTotal(ActorCommonData & tACD, SNO snoPower);  // PowerAPI.cpp
int32 PowerAPI_GetSummonMaxConcurrent(ActorCommonData & tACD, SNO snoPower);  // PowerAPI.cpp
struct PowerAPICosts {  // size 0x8
    PowerID idPower;  // 0x00
    ACDID idACD;  // 0x04
};
static_assert(sizeof(PowerAPICosts) == 0x8, "PowerAPICosts");
SNO PowerAPI_GetDamageSourcePowerSNO(const PowerAPIDamage * ptDamage, RuneType * peRuneType);  // PowerAPI.cpp:12711
void PowerAPI_BuffBackdate(BuffID idBuff, uint32 dwElapsedTime);  // PowerAPI.cpp:11230

void PowerAPI_DoDamage(PowerAPIDamage & tDamage, BOOL fForceAllowDamage, float * pflDamageDone);

void PowerAPI_BuffApplyOnCostEffects(ACDID idACD, PowerID idPowerFrom, SNO snoPower, float flResourceCost, ResourceType eResourceType, float flHealthCost, BOOL bCostsWereCharged, float flHealthCostUndiscounted);  // PowerAPI.cpp:12184

float PowerAPI_ComputeMeleeReach(PowerID idPower, ACDID idACDTarget);  // PowerAPI.cpp:5354

void PowerAPI_BuffApplyOnHealFromSkillEffects(ACDID idACD, PowerID idPowerFrom, SNO snoPower, float flHealAmount, float flOverhealAmount, int32 nNumberOfHeals);  // PowerAPI.cpp:12215

void PowerAPI_DoFindAllPossibleTargets(const PowerAPIChooseTargets & tTargetingInfo, XPooledDataIDList & listTargets);  // PowerAPI.cpp:6247
