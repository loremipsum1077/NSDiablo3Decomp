// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Powers/Powers.h
// Subset: Power with the members PowerScript.cpp uses; the rest is padding (invented names)
// until someone needs it. The full reference header pulls in most of the server.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/Powers/PowersStruct.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/Target.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/DataStructures/XPooledMap.h"
#include "../SGame.h"

class Stopwatch;
struct lua_State;
struct PowerAPIDamage;
struct PowerAPIBuffInstance;
class PowerAPIBuffStartData;

typedef int32 AttribGroupID;

typedef int32 BuffID;

typedef int32 ScriptID;

typedef int32 ScriptInstanceID;

typedef int32 ResourceType;

struct UpdateTargetData {  // size 0x1c
    Target tNewTarget;  // 0x00
};
static_assert(sizeof(UpdateTargetData) == 0x1c, "UpdateTargetData");

// Original Powers.h:58 (implicit constructor).
struct PowerStartData {  // size 0x60
    ACDID idACD;  // 0x00
    ACDID idACDAlternateSource;  // 0x04
    SNO snoPower;  // 0x08
    Target tTarget;  // 0x0c
    void * ptExtraPowerStartData;  // 0x28
    int32 nSkillLevel;  // 0x30
    PreplayedAnimData tPreplayedAnimData;  // 0x34
    int32 nComboLevel;  // 0x40
    uint32 dwAnimTagToUse;  // 0x44
    BOOL bFreeCast;  // 0x48
    BOOL bDoesntAnimate;  // 0x4c
    BOOL bAllowHitProcs;  // 0x50
    uint32 dwTargetMessageFlags;  // 0x54
    RuneType eRuneTypeOverride;  // 0x58
    uint32 dwParam;  // 0x5c
};
static_assert(sizeof(PowerStartData) == 0x60, "PowerStartData");

enum {
    POWER_NONE = -1,
    POWER_POPULATE_ATTRIBUTES = 0,
    POWER_UI_DAMAGE_NUMBERS = 1,
    POWER_CREATE = 2,
    POWER_START = 3,
    POWER_CONTACT = 4,
    POWER_IMPACT = 5,
    POWER_UPDATE_TARGET = 6,
    POWER_INTERRUPT = 7,
    POWER_ACTOR_DONE = 8,
    POWER_ARRIVED = 9,
    POWER_END = 10,
    POWER_WALL_BOUNCE = 11,
    POWER_TOAD_IDLE = 12,
    POWER_BOSS_ENCOUNTER_CLEANUP = 13,
};

typedef int32 PowerMessage;

enum TargetCriteriaLifeStatus {
    TCLS_NOT_DEAD = 0,
    TCLS_ALIVE_ONLY = 1,
    TCLS_DEAD_ONLY = 2,
    TCLS_ANY = 3,
};

enum TargetCriteriaTeamRelationship {
    TCTR_AFFECTS_SCENERY = 0,
    TCTR_AFFECTS_ENEMIES = 1,
    TCTR_AFFECTS_ALLIES = 2,
    TCTR_AFFECTS_NECRO_CORPSES_ONLY = 3,
    TCTR_IGNORES_NON_PLAYERS = 4,
    TCTR_IGNORES_PETS = 5,
    TCTR_IGNORES_WRECKABLES = 6,
    TCTR_IGNORES_PLAYERS = 7,
    TCTR_IGNORES_CASTER = 8,
};

struct TargetCriteria {  // size 0xc
    uint32 dwFlags;  // 0x00
    TargetCriteriaLifeStatus eLifeStatus;  // 0x04
    uint32 dwTeamRelationshipFlags;  // 0x08
};
static_assert(sizeof(TargetCriteria) == 0xc, "TargetCriteria");

// Original Powers.h:243.
struct PowerCursor {  // size 0x20
    WorldPlace tPlace;  // 0x00
    Quaternion qOrientation;  // 0x10
};
static_assert(sizeof(PowerCursor) == 0x20, "PowerCursor");

typedef XPooledList<PowerCursor, PowerCursor> CursorListPooled;

typedef XPooledList<int, int> ScriptInstanceIDListPooled;

typedef XPooledList<PowerAPIDamage *, PowerAPIDamage *> PowerAPIDamageListPooled;

typedef XPooledList<float *, float *> DamageListPooled;

// Original Powers.h:326: buff start data are stored by value.
typedef XPooledList<PowerAPIBuffStartData, PowerAPIBuffStartData> BuffListPooled;

enum PowerFlags {
    PF_INTERRUPTABLE = 0,
    PF_FINISHED = 1,
    PF_CANCEL = 2,
    PF_INITAL_COSTS_PAID = 3,
    PF_STAMINA_COST_PAID = 4,
    PF_FAILED = 5,
    PF_MANUALLY_FINISH = 6,
    PF_WAS_REFLECTED = 7,
    PF_STOPPING = 8,
    PF_ONE_TIME_EVENT_USED = 9,
    PF_INTERRUPTED = 10,
    PF_USING_CLIENT_AUTO_WALK = 11,
    PF_FINISHING_ANIM = 12,
    PF_GLOBE_DROPPED = 13,
    PF_CONSOLE_HAS_DAMAGED_OR_BUFFED = 14,
    PF_CONSOLE_MOVE_TOWARDS_TARGET = 15,
    PF_HAS_SENT_DURING_BUFF_WARNING = 16,
    PF_IGNORE_RANGE = 17,
    PF_DISALLOW_SCRIPT_WAIT = 18,
    PF_DISALLOW_PAYLOAD_APPLICATION = 19,
    PF_COUNT = 20,
};

typedef int32 PowerTemplate;  // X/Common/Powers/PowersCommon.h
typedef int32 Team;
typedef int32 TimerID;
typedef int32 HitSoundType;
typedef int32 PayloadType;
typedef int32 BrainAction;
struct PowerAPIAttackPayload;
struct PowerAPIChooseTargets;

struct PowerTimer {  // size 0x10
    TimerID idTimer;  // 0x00
    void * pCallbackParameter;  // 0x08
};
static_assert(sizeof(PowerTimer) == 0x10, "PowerTimer");

// Original Powers.h:229.
struct AffectedACD {  // size 0x8
    ACDID idACDSource;  // 0x00
    ACDID idACDTarget;  // 0x04
    BOOL operator==(const AffectedACD & tSrc) const {
        return idACDSource == tSrc.idACDSource && idACDTarget == tSrc.idACDTarget;
    }
};
static_assert(sizeof(AffectedACD) == 0x8, "AffectedACD");

// Original Powers.h:254-268.
struct AreaDamageAccumulatorKey {  // size 0x8
    ACDID idACDAttacker;  // 0x00
    ACDID idACDTarget;  // 0x04

    // Powers.h:254. Body inferred from the copy inlined into XMapAssoc's constructor in
    // ActorAccumulateAreaDamage (Actors.cpp:7354): both ids stored as -1 on Powers.h:256.
    AreaDamageAccumulatorKey() {
        idACDAttacker = idACDTarget = ACDID_INVALID;
    }

    // Powers.h:260. The DWARF names neither parameter and has no inlined copy; names and body
    // are inferred.
    AreaDamageAccumulatorKey(const ACDID idAttacker, const ACDID idTarget) {
        idACDAttacker = idAttacker;
        idACDTarget = idTarget;
    }

    // Powers.h:268; the comparison sits on Powers.h:271 (attacker first, then target, per the
    // inlined copies in Actors.cpp).
    BOOL operator==(const AreaDamageAccumulatorKey & tOther) const {
        return idACDAttacker == tOther.idACDAttacker && idACDTarget == tOther.idACDTarget;
    }
};
static_assert(sizeof(AreaDamageAccumulatorKey) == 0x8, "AreaDamageAccumulatorKey");

typedef XPooledDataIDList SummonedActorsList;
typedef XPooledList<PowerTimer, PowerTimer> PowerTimerListPooled;
typedef XPooledList<AffectedACD, AffectedACD> AffectedACDListPooled;
typedef XPooledList<PowerAPIAttackPayload, PowerAPIAttackPayload> PayloadListPooled;
typedef XPooledList<PowerAPIChooseTargets, PowerAPIChooseTargets> ChooseTargetsListPooled;
typedef XPooledMap<unsigned int, unsigned int, int, int, 16> MapIntValues;
typedef XPooledMap<unsigned int, unsigned int, float, float, 16> MapFloatValues;

struct Power {  // size 0x718
    PowerID id;  // 0x00
    ACDID idACD;  // 0x04
    ACDID idACDAlternateSource;  // 0x08
    SNO snoCasterActor;  // 0x0c
    SNO snoPower;  // 0x10
    int32 nCurrentSpellEffect;  // 0x14
    PowerTemplate ePowerTemplate;  // 0x18
    BOOL bFreeCast;  // 0x1c
    BOOL bDoesntAnimate;  // 0x20
    BOOL bAllowHitProcs;  // 0x24
    Target tTarget;  // 0x28
    SNO snoProjectile;  // 0x44
    HitSoundType eHitsound;  // 0x48
    SummonedActorsList tListSummonedActors;  // 0x50
    WorldPlace wpStartLocation;  // 0x78
    Quaternion qStartOrientation;  // 0x88
    TimerID idTimerContact;  // 0x98
    TimerID idTimerInterrupt;  // 0x9c
    TimerID idTimerActorDone;  // 0xa0
    PowerTimerListPooled listTimers;  // 0xa8
    AffectedACDListPooled listAffectedACDs;  // 0xd0
    ScriptInstanceIDListPooled listScriptInstanceIDs;  // 0xf8
    PayloadListPooled listPayloads;  // 0x120
    ChooseTargetsListPooled listChooseTargets;  // 0x148
    CursorListPooled listCursors;  // 0x170
    BuffListPooled listBuffs;  // 0x198
    XPooledDataIDList listBuffInstances;  // 0x1c0
    PowerAPIDamageListPooled listPowerAPIDamage;  // 0x1e8
    DamageListPooled listDamage;  // 0x210
    MapIntValues mapIntValues;  // 0x238
    MapFloatValues mapFloatValues;  // 0x2e0
    XPooledDataIDList listInitialMeleeRangeACDs;  // 0x388
    XPooledDataIDList listActiveCallbackBuffs;  // 0x3b0
    uint32 dwAnimationTag;  // 0x3d8
    PreplayedAnimData tPreplayedAnimData;  // 0x3dc
    SNO snoContactAnimation[3];  // 0x3e8
    int32 nContactAnimationPermutation[3];  // 0x3f4
    AttribGroupID idFastAttrib;  // 0x400
    Team eTeam;  // 0x404
    TargetCriteria tTargetCriteria;  // 0x408
    XPooledDataIDList listACDsTargetted;  // 0x418
    UpdateData tPowerUpdateData;  // 0x440
    uint32 dwStartTick;  // 0x460
    uint32 dwFirstContactTick;  // 0x464
    uint32 dwLastContactTick;  // 0x468
    uint32 dwCurrentContactFrame;  // 0x46c
    BOOL bDidFirstHitResourceGains;  // 0x470
    uint32 uAICoolDownKey;  // 0x474
    uint32 uAICoolDownTicks;  // 0x478
    uint32 dwParam;  // 0x47c
    float flParam;  // 0x480
    float flParam2;  // 0x484
    float flParam3;  // 0x488
    int32 nSkillLevel;  // 0x48c
    int32 nComboLevel;  // 0x490
    ACDID idACDReflectedBy;  // 0x494
    SNO snoPowerLastReflectedBy;  // 0x498
    ACDID idACDLastReflectedBy;  // 0x49c
    uint32 dwFlags;  // 0x4a0
    SNO snoScript;  // 0x4a4
    ScriptID idScript;  // 0x4a8
    lua_State * ptLuaState;  // 0x4b0
    XPooledDataIDList listACDsTracked;  // 0x4b8
    uint32 nScriptCalls;  // 0x4e0
    uint32 nLockGlobalsCount;  // 0x4e4
    uint32 nExpensiveScriptCalls;  // 0x4e8
    int32 nPayloadCount;  // 0x4ec
    float flExecutionTime;  // 0x4f0
    Stopwatch * ptCurrentStopwatch;  // 0x4f8
    WorldPlace placeStartTarget;  // 0x500
    uint32 dwLastLifeOnHitTick;  // 0x510
    uint8 pPowerTemplateData[512];  // 0x514
    uint32 dwCookieValue;  // 0x714
};
static_assert(sizeof(Power) == 0x718, "Power");

struct ACDSortedTarget;  // X/Common/Powers/PowersStruct.h
typedef XPooledMap<AreaDamageAccumulatorKey, AreaDamageAccumulatorKey, float, float, 16> MapAreaDamageAccumulators;

struct PowerGlobals {  // size 0x4b0
    TChainedDataArray<Power, int> * pArrayPowers;  // 0x00
    XListMemoryPool<DamageAttribute> tDamageAttributePool;  // 0x08
    XListMemoryPool<PowerTimer> tPowerTimerPool;  // 0x38
    XListMemoryPool<AffectedACD> tAffectedACDPool;  // 0x68
    XListMemoryPool<int> tScriptInstanceIDPool;  // 0x98
    XListMemoryPool<PowerAPIAttackPayload> tAttackPayloadPool;  // 0xc8
    XListMemoryPool<PowerAPIChooseTargets> tChooseTargetsPool;  // 0xf8
    XListMemoryPool<PowerAPIBuffInstance> tBuffInstancePool;  // 0x128
    XListMemoryPool<PowerCursor> tCursorPool;  // 0x158
    XListMemoryPool<PowerAPIBuffStartData> tBuffStartDataPool;  // 0x188
    TChainedDataArray<PowerAPIBuffInstance, int> * pArrayBuffs;  // 0x1b8
    XPooledDataIDList listBuffsFlaggedForDeletion;  // 0x1c0
    XListMemoryPool<ACDSortedTarget> tACDSortedTargetPool;  // 0x1e8
    XListMemoryPool<PowerAPIDamage *> tPowerAPIDamagePool;  // 0x218
    XListMemoryPool<float *> tDamagePool;  // 0x248
    XPooledMap<int, int, int, int, 16> tScriptIDs;  // 0x278
    XMapMemoryPool<int, int> tScriptIDPool;  // 0x320
    XMapMemoryPool<unsigned int, int> tMapIntValuesPool;  // 0x350
    XMapMemoryPool<unsigned int, float> tMapFloatValuesPool;  // 0x380
    XMapMemoryPool<AreaDamageAccumulatorKey, float> tMapAreaDamageAccumulatorsPool;  // 0x3b0
    MapAreaDamageAccumulators tMapAreaDamageAccumulators;  // 0x3e0
    int32 nMaxAreaEffectTargetCount;  // 0x488
    int32 nMaxProjectileImpactCount;  // 0x48c
    UpdateData tScriptedPowerUpdateData;  // 0x490
};
static_assert(sizeof(PowerGlobals) == 0x4b0, "PowerGlobals");

typedef int32 SpellEffect;  // X/Common/Powers/SpellEffectCommon.h

enum {
    SPELL_INVALID = -1,  // X/Common/Powers/SpellEffectCommon.h
};

void PowerForceStop(PowerID idPower, BOOL bIgnoreContactFrame);  // Powers.cpp:1477
void SpecificPowerSendMessage(PowerID idPower, PowerMessage eMessage, void * pParam);  // Powers.cpp:2295
int32 PowerGetMaxAreaEffectTargetCount();  // Powers.cpp:5986
void PowerTriggerCooldown(ActorCommonData * ptACD, SNO snoPower, float flOverridenCooldown);  // Powers.cpp:1725
void PowerReduceCooldown(ActorCommonData * ptACD, SNO snoPower, int32 nReductionInTicks);  // Powers.cpp:1746
void PowerReduceCooldownByPercentage(ActorCommonData * ptACD, SNO snoPower, float flReductionPct);  // Powers.cpp:1807
void PowerInterrupt(PowerID idPower, BOOL bIgnoreContactFrame);  // Powers.cpp:1546
ActorCommonData * PowerGetOwner(PowerID idPower);  // Powers.cpp:2226
ActorCommonData * PowerGetOwner(const Power * ptPower);  // Powers.cpp:2235
Power * PowerGet(PowerID idPower);  // Powers.cpp:3787
Power * PowerTryToGet(PowerID idPower);  // Powers.cpp:3798
ACDID PowerGetAlternateSource(PowerID idPower);  // Powers.cpp:2251
int32 PowerGetLevel(const PowerID idPower);  // Powers.cpp:4778
float PowerGetProcScalar(Power & tPower);  // Powers.cpp:5346
float PowerGetAttackSpeed(PowerID idPower);  // Powers.cpp:2004
RuneType PowerGetRuneType(const Power * ptPower);  // Powers.cpp:5859
DamageType PowerGetDamageType(const Power * ptPower);  // Powers.cpp:5887
float PowerGetFormulaValue(PowerID idPower, uint32 dwPowerTag);  // Powers.cpp:5210
Team PowerGetTeam(PowerID idPower);  // Powers.cpp:2134
void PowerSetParam(PowerID idPower, uint32 dwParam);  // Powers.cpp:5036
PayloadType PowerGetPayload(SNO snoPower, uint32 nContactFrame);  // Powers.cpp:2144
SNO PowerGetEffect(PowerID idPower, uint32 dwTag);  // Powers.cpp:2376
SNO PowerGetSNO(PowerID idPower);  // Powers.cpp (_Z11PowerGetSNOi, called from Power_Enchantress.cpp:100)
SpellEffect PowerGetInterruptedEffect(PowerID idPower);  // Powers.cpp:4371
SpellEffect PowerGetNextSpellEffect(PowerID idPower);  // Powers.cpp:4325
void PowerAddAffectedACD(PowerID idPower, const ACDID idACDTarget, const ACDID idACDSource);  // Powers.cpp:4617
int32 PowerGetMaxProjectileImpactCount();  // Powers.cpp:6000

BOOL PowerIsInterruptable(PowerID idPower);
void PowerDetachFromActor(PowerID idPower, BOOL bIgnoreContactFrame);
void PowerCheckForDone(PowerID idPower);

static const int32 INITIAL_POWERS_COUNT = 128;  // Powers.h:571
static const int32 INITIAL_BUFFS_COUNT = 256;  // Powers.h:572

static_assert(__builtin_offsetof(Power, tListSummonedActors) == 0x50, "Power::tListSummonedActors");

static_assert(__builtin_offsetof(Power, listBuffInstances) == 0x1c0, "Power::listBuffInstances");

static_assert(__builtin_offsetof(PowerGlobals, pArrayBuffs) == 0x1b8, "PowerGlobals::pArrayBuffs");

static_assert(__builtin_offsetof(Power, listBuffs) == 0x198, "Power::listBuffs");

static_assert(__builtin_offsetof(Power, wpStartLocation) == 0x78, "Power::wpStartLocation");
static_assert(__builtin_offsetof(Power, qStartOrientation) == 0x88, "Power::qStartOrientation");
static_assert(__builtin_offsetof(Power, listCursors) == 0x170, "Power::listCursors");

PowerID PowerStart(ACDID idACD, SNO snoPower, const Target & tTarget);  // Powers.cpp:2737
void PowerRunChild(PowerID idPower, void * pData);  // Powers.cpp:4731
SNO PowerGetChildPower(PowerID idPower);  // Powers.cpp:4407
uint32 PowerGetParam(PowerID idPower);  // Powers.cpp:5051
void PowerClearDuringBuffs(const Power * ptPower, const ActorCommonData * ptACD);  // Powers.cpp:1375
ACDID PowerGetTargetACD(const Power * ptPower);  // Powers.cpp:1129
ACDID PowerGetTargetACD(PowerID idPower);
struct NavPlace;
void PowerGetACDPoint(PowerID idPower, const ACDID idACD, NavPlace & tNavPlace, SNO snoAnim, int32 nAnimPerm, Vector3D * pvOffset);  // Powers.cpp:1011
void PowerSnapshotMeleeRange(Power * ptPower, const ACDID idACDPlayer, float flLongReach, const Target & tTarget);  // Powers.cpp:2590
void PowerSetStoredFloat(Power * ptPower, LPCSTR szName, float flValue);
float PowerGetStoredFloat(Power * ptPower, LPCSTR szName);
void PowerSetStoredInt(Power * ptPower, LPCSTR szName, int32 nValue);  // Powers.cpp:5807
BOOL PowerGetTarget(const PowerID idPower, Target & tTarget);  // Powers.cpp:1149
void PowerResetSkills(ActorCommonData & tACD, BOOL bNewHero);  // Powers.cpp:4801
void PowerStartDataInit(PowerStartData & tPowerStartData);  // Powers.cpp:5066
PowerID PowerStartEx(const PowerStartData & tPowerStartData, AttribGroupID idFastAttribSource, BOOL bForceStart, BOOL bIgnoreRange);  // Powers.cpp:2836
uint32 PowerFailureReasons(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, BOOL bIsAI, BOOL bFreeCast);  // Powers.cpp:1622
BOOL PowerCanUseNow(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, uint32 * pdwFailureReasons);  // Powers.cpp:1681
BOOL PowerActorCanDo(ActorID idActor, SNO snoPower);  // ActorIsAffected.cpp:270
struct Player;
BOOL PowerIsAssigned(SNO snoPower, Player * ptPlayer);  // Powers.cpp:1895
BOOL PowerUseAlternateAnim(PowerID idPower);  // Powers.cpp:2415

#line 523 "X/Server/Powers/Powers.h"
typedef XPooledList<ACDSortedTarget, ACDSortedTarget> XPooledACDSortedTargetList;
