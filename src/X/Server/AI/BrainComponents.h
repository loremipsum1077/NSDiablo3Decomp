// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/AI/BrainComponents.h
// Subset: the BrainAPI entry points the brain action updates call.  The rest of the file
// (brain settings structs, enums) is not needed yet.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Target.h"

// X/Server/AI/BrainComponents.cpp:3685
void BrainAPIDoSpecialPower(ActorID idActor, SNO snoPower, const Target & tTarget, uint32 dwTimeOut);

#include "../../Common/AI/ActorAIStatesSwig.h"
#include "BrainStance.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"

class PowerAPIDamage;

// BrainComponents.h:107. Target supplies the implicit constructor.
struct BrainAttackSettings {
    Target tTarget;
    SNO snoPower;
    uint32 dwAttackWarmUp;
    uint32 dwAttackWarmUpDelta;
    uint32 dwAttackFlags;
    uint32 uAICoolDownKey;
    uint32 uAICoolDownTicks;
    ActorAnimationTag eAnimTag;
    float flSpeed;
    ACDID idACDCastLocation;
    int32 nComboLevel;
    AIStateType eAIStateFrom;
    int32 nSuccessfulCasts;
};
static_assert(sizeof(BrainAttackSettings) == 0x4c, "BrainAttackSettings");

struct BrainWanderSettings {
    uint32 dwTimerMin;
    uint32 dwTimerDelta;
    float flMinDistance;
    float flMaxDistance;
    float flWanderRadiusMin;
    float flWanderRadiusMax;
    Angle aMaxAngle;
    uint32 dwTagAnimationWalking;
    uint32 dwTagAnimationIdling;
    BOOL bResetWanderCenter;
    WorldPlace placeWander;
};
static_assert(sizeof(BrainWanderSettings) == 0x38, "BrainWanderSettings");

typedef BOOL (*BrainAPITargetFilterCallback)(ACDID, ACDID);

// Definitions recorded in BrainComponents.cpp, lines 3521, 3606, 4263, 4310,
// 4544, 6469 and 9048 respectively.
void BrainAPIGetAttackSettings(ActorID idActor, BrainAttackSettings & tAttackSettings);
void BrainAPIDoAttack(ActorID idActor, const BrainAttackSettings & tAttackSettings);
void BrainAPIGetWanderSettings(ActorID idActor, BrainWanderSettings & tWanderSettings);
void BrainAPIDoWander(ActorID idActor, BrainWanderSettings & tWanderSettings);
void BrainAPIDoWalk(ActorID idActor, const WorldPlace & placeGotoPoint, WorldDistance wdSpeed, TranslateType nTranslateType, uint32 dwAnimTag);
void BrainAPIDoDeath(ActorID idDeadActor, ActorID idKillerActor, const BOOL fNoExp, const BOOL fNoLoot, const BOOL fNoHealthDrop, PowerAPIDamage * ptDamage, float * pflDamageDone);
ActorID BrainAPIActorChooseTarget(ActorID idActor, SNO snoPower, BOOL bJustChooseClosest, BrainAPITargetFilterCallback pfnFilterCallback);

// Original component layouts used by BrainComponents.cpp.
#include "../../Sigma/DataStructures/XPooledList.h"
enum AttackFlags {
    ATTACK_DONT_CLOSE = 0,
    ATTACK_ONLY_DO_ONCE = 1,
    ATTACK_EVEN_IF_OUT_OF_RANGE = 2,
    ATTACK_DONT_PLAY_UNALERT_NEUTRAL = 3,
    ATTACK_DONT_FACE = 4,
    ATTACK_FORCE_IMMEDIATELY = 5,
};

enum ActiveComponent {
    ACTIVE_COMPONENT_IDLE = 0,
    ACTIVE_COMPONENT_JOB = 1,
    ACTIVE_COMPONENT_WANDER = 2,
    ACTIVE_COMPONENT_ATTACK = 3,
    ACTIVE_COMPONENT_CIRCLE = 4,
    ACTIVE_COMPONENT_CHARGE = 5,
    ACTIVE_COMPONENT_RUNAWAY = 6,
    ACTIVE_COMPONENT_FEARED = 7,
    ACTIVE_COMPONENT_DEATH = 8,
    ACTIVE_COMPONENT_WALK = 9,
    ACTIVE_COMPONENT_3STATE_WALK = 10,
    ACTIVE_COMPONENT_ANIMATION = 11,
    ACTIVE_COMPONENT_SPECIAL_POWER = 12,
    ACTIVE_COMPONENT_FROZEN = 13,
    ACTIVE_COMPONENT_BLIND = 14,
    ACTIVE_COMPONENT_STUNNED = 15,
    ACTIVE_COMPONENT_TAUNTED = 16,
    ACTIVE_COMPONENT_FORCE_GRIPPED = 17,
    ACTIVE_COMPONENT_WANDER_MINION = 18,
};

enum {
    BRAIN_CONDITION_NONE = -1,
    BRAIN_CONDITION_READY = 0,
    BRAIN_CONDITION_PERFORMING = 1,
    BRAIN_CONDITION_ABORTED = 2,
    BRAIN_CONDITION_TIMEOUT = 3,
    BRAIN_CONDITION_DUMB = 4,
};

enum {
    BRAIN_ACTION_NONE = -1,
    BRAIN_ACTION_WANDER = 0,
    BRAIN_ACTION_IDLE = 1,
    BRAIN_ACTION_STRAFE = 2,
    BRAIN_ACTION_CIRCLE = 3,
    BRAIN_ACTION_RUNAWAY = 4,
    BRAIN_ACTION_CLOSE = 5,
    BRAIN_ACTION_RUN_NEARBY = 6,
    BRAIN_ACTION_WANDER_RUN = 7,
    BRAIN_ACTION_CIRCLE_STRAFE = 8,
    BRAIN_ACTION_FOLLOW = 9,
    BRAIN_ACTION_RUN_IN_FRONT = 10,
    BRAIN_ACTION_RETURN_TO_PATH = 11,
    BRAIN_ACTION_FOLLOW_PATH = 12,
    BRAIN_ACTION_ORBIT = 13,
    BRAIN_ACTION_LOOKAT = 14,
    BRAIN_ACTION_RETURN_TO_GUARD_OBJECT = 15,
    BRAIN_ACTION_WANDER_TOWN = 16,
    BRAIN_ACTION_BACKPEDAL = 17,
    BRAIN_ACTION_WANDER_MINION = 18,
    BRAIN_ACTION_FOLLOW_NATURAL = 19,
};

enum CircleDirection {
    CIRCLE_DIRECTION_RANDOM = 0,
    CIRCLE_DIRECTION_RANDOM_ONCE = 1,
    CIRCLE_DIRECTION_CW = 2,
    CIRCLE_DIRECTION_CCW = 3,
};

struct BrainSkillSettings {  // size 0x8
    BOOL fSkillEnable;  // 0x00
    uint32 dwDelayTime;  // 0x04
};
static_assert(sizeof(BrainSkillSettings) == 0x8, "BrainSkillSettings");

struct TargetInfo {  // size 0xc
    ActorID idTarget;  // 0x00
    uint32 dwLastSeenTime;  // 0x04
    uint32 dwActiveTime;  // 0x08
};
static_assert(sizeof(TargetInfo) == 0xc, "TargetInfo");

typedef XPooledList<TargetInfo, const TargetInfo &> TTargetList;


ActiveComponent BrainAPIGetActiveComponent(ActorID idActor);
void BrainComponentsTriggerUpdate(ActorID idActor, BOOL bRunAIUpdate);
void BrainAPITriggerUpdate(ActorID idActor, BOOL fSuccessful);
void BrainAPISetWanderCenter(ActorID idActor, const WorldPlace & wpWanderCenter);
void BrainAPIDoDefaultWander(ActorID idActor);
int32 BrainAPIGetNumEnemyTargets(ActorID idActor);
int32 BrainAPIGetNumEnemyPlayerTargets(ActorID idActor);
BOOL BrainAPIActorHasTarget(ActorID idActor, ActorID idTarget);
void BrainAPIUpdateTargets(ActorID idActor, BOOL bForceUpdate);
uint32 BrainAPIGetTimeSinceEnemyTarget(ActorID idActor);
const TTargetList * BrainAPIGetEnemyTargetList(ActorID idActor);
const TTargetList * BrainAPIGetFriendlyTargetList(ActorID idActor);
BOOL BrainAPIActorHasQueuedAttack(ActorID idActor);
class ActorCommonData;
BOOL BrainAPIIsActorExcludedFromPerformanceOptimizations(const ActorCommonData & tACD);

BOOL ActorIsInDeactivatedScene(ActorID idActor);
BOOL BrainConditionIsReady(ActorID idActor);
void BrainAPIDoIdle(ActorID idActor);
void BrainAPIDoIdleAndFace(ActorID idActor, const WorldPlace & placeTarget);
void BrainAPIDoFace(ActorID idActor, const WorldPlace & placeTarget);
void BrainAPIDoPlayAnimation(const ActorID idActor, const uint32 dwAnimTag, const uint32 dwDuration, const BOOL bAllowPlayingAnimInterrupt, const BOOL bInterruptableAnim, float flUnitStartTime, const BOOL bDontSendToClient, const float flSpeedScalar);
void BrainAPIUndoBlind(ActorID idActor);
void BrainAPIWarnNearbyAlliesAboutTarget(ActorID idActor, ActorID idTargetActor);  // BrainComponents.cpp:7643

float BrainAPIActorGetAwarenessScanRadius(ACDID idACD, WorldPlace & placeCenter);  // BrainComponents.cpp
BOOL BrainAPIHasAttackerSlotReservedForTarget(ActorID idActor, ActorID idActorTarget);  // BrainComponents.cpp
BOOL BrainAPIActorAIIsInactive(const ActorCommonData & tACD);  // BrainComponents.cpp

#include "../../Common/TeamCommon.h"
class ActorCommonData;
void BrainAPIWarnAllyAboutTarget(ActorID idActor, ActorCommonData & tACD, Team, ActorID idTargetActor, BOOL bDelay);  // BrainComponents.cpp:7598

// BrainComponents.cpp:11130; no recovered header declaration. Owning API header placement is inferred.
ACDID BrainAPIChoosePrimaryTarget(ActorCommonData & tACD, BOOL bActiveEnemiesOnly);
// BrainComponents.cpp:8878; no recovered header declaration. Owning API header placement is inferred.
ACDID BrainAPIActorGetClosestTarget(ActorID idActor, SNO snoPower, BOOL bTargetFriendlies, BrainAPITargetFilterCallback pfnFilterCallback);
