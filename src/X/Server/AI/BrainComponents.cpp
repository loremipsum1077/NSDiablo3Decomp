// Reconstructed from original DWARF layouts/inline trees and original instructions.
// Original owner: X/Server/AI/BrainComponents.cpp. Unwritten functions remain open.
#include "BrainComponents.h"
#include "ActorAI.h"
#include "AIStates.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../Timers.h"
#include "../Actors/Actors.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/CombatLog.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/MonsterStruct.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../Actors/ActorPowers.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../SGameConstants.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/Nav/NavZone.h"
#include "../../Common/Attributes/FastAttribConstants.h"
#include "../../Common/Monsters.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/WorldCommonCollision.h"
#include "../../Common/NodePathSwig.h"
#include "../../Common/Attributes/FastAttribute.h"
#include <float.h>
#include "../Powers/Powers.h"
#include "../Powers/PowerAPI.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../Crafting/SItemCrafting.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/ItemsSwig.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/Console/Avenger.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Common/GameConstants.h"
#include "../../Common/GameCommonData.h"
#include "../PlayerList.h"
#include "../../Common/ActorCommonDataSwig.h"
#include "../../Common/Definitions/MonsterDefinition.h"
#include "../../Common/World.h"
#include "../../Common/WorldPlace.h"
#include "../../Common/Game.h"
#include "../Actors/ActorEffect.h"
#include "../Movement/SActorMovement.h"
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Player.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../Powers/PowerHelper.h"
#include "../../Common/AI/ActorAISwig.h"
#include "../../Sigma/MemoryPools.h"

enum EAttackStates {
    ATTACKSTATE_IDLE = 0,
    ATTACKSTATE_CLOSING = 1,
    ATTACKSTATE_ATTACKING = 2,
    ATTACKSTATE_COOLING = 3,
    ATTACKSTATE_WAITING = 4,
};

struct BrainComponentJob {  // size 0x14
    WorldPlace placeJobCenter;  // 0x00
    Angle aJobYaw;  // 0x10
};
static_assert(sizeof(BrainComponentJob) == 0x14, "BrainComponentJob");

struct BrainComponentWander {  // size 0x34
    uint32 dwTimeNextWander;  // 0x00
    WorldPlace placeWanderCenter;  // 0x04
    uint32 dwTimerMin;  // 0x14
    uint32 dwTimerDelta;  // 0x18
    float flMinDistance;  // 0x1c
    float flMaxDistance;  // 0x20
    float flWanderRadiusMin;  // 0x24
    float flWanderRadiusMax;  // 0x28
    Angle aMaxAngle;  // 0x2c
    uint32 dwTagAnimation;  // 0x30
};
static_assert(sizeof(BrainComponentWander) == 0x34, "BrainComponentWander");

struct BrainComponentAttack {  // size 0x68
    BrainAttackSettings tSettings;  // 0x00
    EAttackStates nAttackState;  // 0x4c
    BOOL bNeedWarmUp;  // 0x50
    BOOL bAttackQueued;  // 0x54
    BOOL bClosingCooldownQueued;  // 0x58
    uint32 dwClosingCooldownTime;  // 0x5c
    uint32 dwWalkTimer;  // 0x60
    uint32 dwWalkCooldownTimer;  // 0x64
};
static_assert(sizeof(BrainComponentAttack) == 0x68, "BrainComponentAttack");

struct BrainComponentWalkTo {  // size 0x1c
    WorldPlace placeGotoPoint;  // 0x00
    WorldDistance wdSpeed;  // 0x10
    TranslateType nTranslateType;  // 0x14
    uint32 dwAnimTag;  // 0x18
};
static_assert(sizeof(BrainComponentWalkTo) == 0x1c, "BrainComponentWalkTo");

struct BrainComponentCharge {  // size 0x38
    Target tTarget;  // 0x00
    SNO snoPower;  // 0x1c
    uint32 dwAttackFlags;  // 0x20
    WorldPlace placeTarget;  // 0x24
    ACDID idACDTarget;  // 0x34
};
static_assert(sizeof(BrainComponentCharge) == 0x38, "BrainComponentCharge");

struct BrainComponentCircle {  // size 0x2c
    Target tTarget;  // 0x00
    SNO snoPower;  // 0x1c
    BOOL bFaceTarget;  // 0x20
    TimerID idTimer;  // 0x24
    CircleDirection eCircleDirection;  // 0x28
};
static_assert(sizeof(BrainComponentCircle) == 0x2c, "BrainComponentCircle");

struct BrainComponentRunAway {  // size 0x24
    Target tTarget;  // 0x00
    SNO snoPower;  // 0x1c
    TranslateType eTranslateType;  // 0x20
};
static_assert(sizeof(BrainComponentRunAway) == 0x24, "BrainComponentRunAway");

struct BrainComponentBlind {  // size 0x1c
    uint32 dwStartTime;  // 0x00
    uint32 dwEndTime;  // 0x04
    uint32 dwTimeNextWander;  // 0x08
    uint32 dwTimerMin;  // 0x0c
    uint32 dwTimerDelta;  // 0x10
    float flWanderRadiusMin;  // 0x14
    float flWanderRadiusDelta;  // 0x18
};
static_assert(sizeof(BrainComponentBlind) == 0x1c, "BrainComponentBlind");

struct BrainComponentDeath {  // size 0x4
    uint32 dwTimeToRemoveBody;  // 0x00
};
static_assert(sizeof(BrainComponentDeath) == 0x4, "BrainComponentDeath");

struct BrainComponentMovement {  // size 0x20
    uint32 dwRunTimer;  // 0x00
    uint32 dwWalkTimer;  // 0x04
    uint32 dwBlockTimer;  // 0x08
    uint32 dwBlockCooldownTimer;  // 0x0c
    float flMoveDirection;  // 0x10
    uint32 dwZigZagTimer;  // 0x14
    float flZigZagDirection;  // 0x18
    uint32 dwSprintRunSelected;  // 0x1c
};
static_assert(sizeof(BrainComponentMovement) == 0x20, "BrainComponentMovement");

struct BrainComponentSpecialPower {  // size 0x20
    Target tTarget;  // 0x00
    SNO snoPower;  // 0x1c
};
static_assert(sizeof(BrainComponentSpecialPower) == 0x20, "BrainComponentSpecialPower");

struct BrainComponentTarget {  // size 0x78
    TTargetList listTargets;  // 0x00
    TTargetList listFriendlies;  // 0x28
    ActorID idTarget;  // 0x50
    BOOL bIncrementedAttackerCount;  // 0x54
    BOOL bHasAttackerSlotReserved;  // 0x58
    uint32 dwNextTargetScanTime;  // 0x5c
    uint32 dwLastTimeWithEnemyTarget;  // 0x60
    MonsterAwarenessInfo tAwareness;  // 0x64
};
static_assert(sizeof(BrainComponentTarget) == 0x78, "BrainComponentTarget");

struct BrainComponentData {  // size 0x278
    BrainSkillSettings tSkillSettings[8];  // 0x00
    BrainComponentJob tJob;  // 0x40
    BrainComponentWander tWander;  // 0x54
    BrainComponentAttack tAttack;  // 0x88
    BrainComponentWalkTo tWalkTo;  // 0xf0
    BrainComponentCircle tCircle;  // 0x10c
    BrainComponentCharge tCharge;  // 0x138
    BrainComponentRunAway tRunAway;  // 0x170
    BrainComponentDeath tDeath;  // 0x194
    BrainComponentMovement tMovement;  // 0x198
    BrainComponentTarget tTarget;  // 0x1b8
    BrainComponentSpecialPower tSpecialPower;  // 0x230
    BrainComponentBlind tBlind;  // 0x250
    ActiveComponent nActiveComponent;  // 0x26c
    ActiveComponent nNextComponent;  // 0x270
};
static_assert(sizeof(BrainComponentData) == 0x278, "BrainComponentData");

static_assert(__builtin_offsetof(BrainComponentData, tTarget) == 0x1b8, "BrainComponentData::tTarget");
static_assert(__builtin_offsetof(BrainComponentData, nActiveComponent) == 0x26c, "BrainComponentData::nActiveComponent");

struct BrainComponentGlobals {  // size 0x70
    XChainedFixedBlockAllocator<BrainComponentData> poolBrainComponents;  // 0x00
    XListMemoryPool<TargetInfo> poolTargetInfo;  // 0x30
    uint32 uAttackersPerPlayerThrottlingThreshold;  // 0x60
    float flMaxThrottledAbandonDistance;  // 0x64
    float flMaxThrottledAbandonTime;  // 0x68
    BOOL bPreventWarnNearbyAlliesWhenWarned;  // 0x6c
};
static_assert(sizeof(BrainComponentGlobals) == 0x70, "BrainComponentGlobals");


enum { GB_INVALID = -1 };  // X/Common/GameBalance.h
enum { ACTOR_SPHERE_TREE_FLAGS_ALL = 7 };  // X/Common/Actors/ActorCollision.h
enum { FLOATING_NUMBER_TYPE_DODGE = 6, FLOATING_NUMBER_TYPE_DODGED = 7, FLOATING_NUMBER_TYPE_BLOCK = 8 };  // X/Common/Player.h







struct ClosestEnemyCallbackData {  // Original size 0x8.
    ACDID idACDClosest;  // 0x00
    WorldDistance wdClosestDistanceSq;  // 0x04
};
static_assert(sizeof(ClosestEnemyCallbackData) == 0x8, "ClosestEnemyCallbackData");

// X/Common/Player.cpp:1659; declaration not yet present in adopted Player.h.
BOOL ACDIsPlayer(const ACDID idACD);

// Original declarations from headers not yet fully adopted; retain their call ABI.
// ActorCommonData.cpp:4934; PowersCommon.cpp:4532; BrainStance.cpp:2416/2388.
BOOL ACDIsInDeactivatedScene(const ActorCommonData & tACD);
uint32 PowerGetCoolDown(const ActorCommonData & tACD);
void BrainStanceDoIdle(ActorID idActor);
void BrainStanceDoPlayAnimation(const ActorID idActor, const uint32 dwAnimTag, const uint32 dwDuration, const BOOL bAllowPlayingAnimInterrupt, const BOOL bInterruptableAnim, float flUnitStartTime, const BOOL bDontSendToClient, const float flSpeedScalar);
void ActorPowersInterruptPrimary(ActorID idActor, BOOL bIgnoreContactFrame);
void PlayerBrainClearQueuedPower(ACDID idACD);  // PlayerBrain.cpp: original declaration not yet adopted
BOOL BrainAPIResurrectByType(ACDID idACD, RevivalType eRevivalType);  // BrainComponents.cpp:2981
const MonsterAwarenessInfo * BrainAPIActorGetAwareness(ACDID idACD, WorldPlace & placeCenter);  // BrainComponents.cpp:8123
Angle ACDGetYaw(ACDID idACD);  // ActorCommonData.cpp:2997
ActorBrain ActorGetBrainType(ActorID idActor);  // ActorAI.cpp
void ActorAIResetWarnNearbyAlliesAboutTargetTime(ActorID idActor);  // ActorAI.cpp:4624
BOOL AIWalkNodePath(ActorID idActor, WorldPlace & placeGotoPoint);  // ActorAI.cpp:3425
void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);  // X/Common/Actors/ActorCollision.cpp:317
void BrainAPIActorAddTarget(ActorID idActor, ActorCommonData & tACD, ActorID idTarget, BOOL bRequireLOS, BOOL bAddSummoner, uint32 uDelayTicks, BOOL bForceTarget);  // BrainComponents.cpp:9453
void BrainAPIActorReleaseTarget(ActorID idActor);  // BrainComponents.cpp:8709
static void sReportBadPower(const ActorID idActor);  // BrainComponents.cpp:7586
void ActorAISetTauntTarget(ActorID idActorTauntee, ACDID idACDTaunter);  // ActorAI.cpp:2107
void PowerSetCoolDown(ActorCommonData & tACD, const uint32 dwCooldown);  // X/Server/Powers/Powers.cpp:1715
static void sActorAddTarget(ACDID idACDTarget, ActorID idActor, ActorCommonData & tACD, BOOL bRequireLOS);  // BrainComponents.cpp:8248
inline void BrainAPIPlayerActorAddTarget(ActorID idActor, ActorID idTarget, BOOL bRequireLOS);  // BrainComponents.cpp:9441
BOOL NavMeshCheckLOSToACD(NavCellHandle hNavCellStart, const WorldPlace & placeStart, ACDID idACDDest, const PassabilityType ePassabilityType, BOOL bWantProjectileLOS);  // X/Common/Nav/NavigationMesh.cpp:3536
WorldDistance PowerGetDistanceViaWorldPlace(const WorldPlace & placeStart, const WorldPlace & placeTarget, WorldDistance wdStartHeight, const WorldDistance wdStartRadius, WorldDistance wdTargetHeight, const WorldDistance wdTargetRadius, const WorldDistance wdLongReach);  // X/Common/Powers/PowersCommon.cpp:8290
void BrainStanceSetPower(ActorID idActor, SNO snoPower);  // BrainStance.cpp:1002
void BrainAPIDoAIAction(ActorID idActor, const BrainAttackSettings & tAttackSettings);  // BrainComponents.cpp:4589
BrainAction PowerGetBrainAction(SNO snoPower);  // X/Server/Powers/Powers.cpp:5896
inline BOOL BrainAPIIsAIAction(SNO snoAction);  // BrainComponents.cpp:4580
BOOL WorldCollSweptPoint(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Vector2D & wpStart, const Vector2D & wpEnd, const WorldCollParams & tParams, uint32 dwFlags, uint32 dwFlags2, ACDID idACDIgnore, WorldCollResult & tResult);  // X/Common/WorldCommonCollision.cpp
struct NodePathNode;
NodePathNode * NodePathGetNode(PathID idPath, PathNodeID idPathNode);  // X/Common/NodePath.cpp:629
PathNodeID NodePathGetNextNodeAndPosition(PathID idPath, const Vector3D & wpPos, const NodePathNode * ptPathNodeDest, Vector3D & wpPosClosest);  // X/Common/NodePath.cpp:665
void BrainComponentDoMovement(ActorID idActor, const WorldPlace & placeTarget, WorldDistance wdSpeed, TranslateType nTranslateType, uint32 dwAnimTag, WorldDistance wdMaxRangeFromDest, LPCSTR sTraceString, ACDID idACDTarget, BOOL bIgnoreMovementStyles, SNO snoPowerPassability);  // BrainComponents.cpp:5102

// Original lines 312-316; names, initial values and strings from DWARF/initializer.
static XVarScope s_scopeBrainComponents("BrainComponents", 0);
XVarUint32 s_varAttackersPerPlayerThrottlingThreshold(s_scopeBrainComponents, "AttackersPerPlayerThrottlingThreshold", 10);
XVarFloat s_varMaxThrottledAbandonDistance(s_scopeBrainComponents, "MaxThrottledAbandonDistance", 60.0f);
XVarFloat s_varMaxThrottledAbandonTime(s_scopeBrainComponents, "MaxThrottledAbandonTime", 1.0f);
XVarBool s_varPreventWarnNearbyAlliesWhenWarned(s_scopeBrainComponents, "PreventWarnNearbyAlliesWhenWarned", true);

// Original line 341.
void BrainComponentInitForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    BrainComponentGlobals *& ptBrainComponentGlobals = tSGame.ptBrainComponentGlobals;
    ptBrainComponentGlobals = (BrainComponentGlobals *)SigmaMemoryNew(sizeof(BrainComponentGlobals), 4, tSGame.ptMemoryInterface, TRUE);
    ptBrainComponentGlobals->poolBrainComponents.Initialize();
    ptBrainComponentGlobals->poolBrainComponents.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptBrainComponentGlobals->poolBrainComponents.SetBlockCount_Tracked(150);
    ptBrainComponentGlobals->poolTargetInfo.Initialize();
    ptBrainComponentGlobals->poolTargetInfo.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptBrainComponentGlobals->poolTargetInfo.SetBlockCount_Tracked(150);
    ptBrainComponentGlobals->uAttackersPerPlayerThrottlingThreshold = 0;
    ptBrainComponentGlobals->flMaxThrottledAbandonDistance = 0.0f;
    ptBrainComponentGlobals->flMaxThrottledAbandonTime = 0.0f;
    ptBrainComponentGlobals->bPreventWarnNearbyAlliesWhenWarned = FALSE;
}

// Original line 365.
void BrainComponentDisposeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    BrainComponentGlobals *& ptBrainComponentGlobals = tSGame.ptBrainComponentGlobals;
    if (ptBrainComponentGlobals)
    {
        ptBrainComponentGlobals->poolBrainComponents.Dispose();
        ptBrainComponentGlobals->poolTargetInfo.Dispose();
        SigmaMemoryFree(ptBrainComponentGlobals, tSGame.ptMemoryInterface);
        ptBrainComponentGlobals = NULL;
    }
}

// Original line 382: external linkage, with no original out-of-line inventory body.
inline BrainComponentData * sGetBrainComponentData(ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    return tAI.ptBrainComponentData;
}

// Original line 392.
void BrainComponentUpdateServerVariables()
{
    BrainComponentGlobals * ptBrainComponentGlobals = SGameManager::GameGet().ptBrainComponentGlobals;
    ptBrainComponentGlobals->uAttackersPerPlayerThrottlingThreshold = s_varAttackersPerPlayerThrottlingThreshold.Get();
    ptBrainComponentGlobals->flMaxThrottledAbandonDistance = s_varMaxThrottledAbandonDistance.Get();
    ptBrainComponentGlobals->flMaxThrottledAbandonTime = s_varMaxThrottledAbandonTime.Get();
    ptBrainComponentGlobals->bPreventWarnNearbyAlliesWhenWarned = s_varPreventWarnNearbyAlliesWhenWarned.Get();
}

// Original line 406.
static void sBrainComponentMovementInitialize(ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentMovement & tMovement = ptComp->tMovement;
    tMovement.dwRunTimer = 0;
    tMovement.dwWalkTimer = 0;
    tMovement.dwBlockTimer = 0;
    tMovement.dwBlockCooldownTimer = 0;
    tMovement.flMoveDirection = 0.0f;
    tMovement.dwZigZagTimer = 0;
    tMovement.flZigZagDirection = 0.0f;
    tMovement.dwSprintRunSelected = TAG_ACTOR_ANIMATION_NEUTRAL;
}

// Original line 425.
void BrainComponentsTriggerUpdate(ActorID idActor, BOOL bRunAIUpdate)
{
    if (ActorHasBrain(idActor)) {
        ActorAI & tAI = ActorGetAI(idActor);
        uint32 dwNow = GameGetServerTime();
        tAI.dwNextBrainComponentUpdateTime = dwNow;
        if (bRunAIUpdate)
            ServerQueueUpdateFunction(0, tAI.tAIUpdateData);
    }
}

// Original line 449.
BOOL ActorIsInDeactivatedScene(ActorID idActor)
{
    const Actor * ptActor = ActorGet(idActor);
    return ACDIsInDeactivatedScene(ACDGet(ptActor->idACD));
}

// Original line 460.
static void sBrainComponentSetStatus(ActorID idActor, LPCSTR sBrainComponentStatus)
{
    ActorAI & tAI = ActorGetAI(idActor);
    tAI.sBrainComponentStatus = sBrainComponentStatus;
}

// Original line 471.
static void sAttackReleaseTargets(ActorID idActor, BrainComponentData * ptComp)
{
    BrainComponentAttack & tAttack = ptComp->tAttack;
    BrainAPIActorReleaseTarget(idActor);
    tAttack.nAttackState = ATTACKSTATE_IDLE;
}

// Original line 484.
static void sAttackStop(ActorID idActor, BrainComponentData * ptComp)
{
    sAttackReleaseTargets(idActor, ptComp);
    BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
    BrainStanceDoIdle(idActor);
}

// Original line 497.
static void sBrainChangeActiveComponent(ActorID idActor, ActiveComponent nNewComponent)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    const Actor * ptActor = ActorGet(idActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    if (ACDIsPlayer(tACD.id) || tACD.IsPlayerTeam() ||
        (tACD.IsMonster() && tACD.eMonsterRarity == MR_BOSS) ||
        BrainAPIIsActorExcludedFromPerformanceOptimizations(tACD))
        BrainComponentsTriggerUpdate(idActor, FALSE);
    ptComp->nNextComponent = nNewComponent;
    sBrainComponentMovementInitialize(idActor);
    if (ptComp->nNextComponent != ptComp->nActiveComponent) {
        AITrace(idActor, "new component %d", nNewComponent);
        BrainStanceDoIdle(idActor);
    }
}

void BrainAPIDoFeared(ActorID idActor, ACDID idACDFearSource);  // line 4030
WorldDistance BrainAPIGetDistanceToTarget(const ActorID idActor, const ActorID idTarget);  // line 8618

// Original line 530.
static void sChangedComponents(ActorID idActor, BrainComponentData * ptComp)
{
    switch (ptComp->nActiveComponent) {
    case ACTIVE_COMPONENT_ATTACK:
        sAttackReleaseTargets(idActor, ptComp);
        {
            BrainComponentWander & tWander = ptComp->tWander;
            const Actor * ptActor = ActorGet(idActor);
            tWander.placeWanderCenter = ACDGet(ptActor->idACD).GetPlace();
        }
        break;

    case ACTIVE_COMPONENT_FEARED: {
        const Actor * ptActor = ActorGet(idActor);
        ACDID idACD = ptActor->idACD;
        ActorCommonData & tACD = ACDGet(idACD);
        if (tACD.IsFeared()) {
            ptComp->nNextComponent = ACTIVE_COMPONENT_FEARED;
            BrainAPIDoFeared(idActor, ptComp->tRunAway.tTarget.idACDTarget);
            return;
        }
        break;
    }

    case ACTIVE_COMPONENT_IDLE:
    case ACTIVE_COMPONENT_CIRCLE:
    case ACTIVE_COMPONENT_RUNAWAY: {
        BrainComponentCircle & tCircle = ptComp->tCircle;
        TimerUnregister(tCircle.idTimer);
        tCircle.idTimer = TIMERID_INVALID;
        break;
    }

    default:
        break;
    }

    ptComp->nActiveComponent = ptComp->nNextComponent;
}

// Original line 594.
BOOL BrainConditionIsReady(ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    uint32 dwNow = GameGetServerTime();
    if (tAI.eBrainCondition == BRAIN_CONDITION_READY ||
        tAI.eBrainCondition == BRAIN_CONDITION_ABORTED ||
        tAI.eBrainCondition == BRAIN_CONDITION_TIMEOUT)
        return TRUE;
    if (tAI.dwConditionTimeout < dwNow) {
        BrainConditionSetCondition(idActor, BRAIN_CONDITION_TIMEOUT);
        return TRUE;
    }
    return FALSE;
}

// Original line 618.
void BrainConditionSetCondition(ActorID idActor, BrainCondition eBrainCondition)
{
    ActorAI & tAI = ActorGetAI(idActor);
    if (eBrainCondition == BRAIN_CONDITION_PERFORMING) {
        uint32 dwNow = GameGetServerTime();
        ActorCommonData & tACD = ACDGet(ACDFindByActor(idActor));
        tAI.dwConditionTimeout = Math::Max(dwNow, PowerGetCoolDown(tACD)) + 90;
    } else if (eBrainCondition == BRAIN_CONDITION_ABORTED) {
        BrainComponentData * ptComp = sGetBrainComponentData(idActor);
        if (ptComp->nActiveComponent == ACTIVE_COMPONENT_CHARGE)
            BrainAPIDoIdle(idActor);
    }
    tAI.eBrainCondition = eBrainCondition;
}

// Original line 648; inlined into BrainAPIDoBlind and other condition users.
inline void BrainConditionSetConditionAndTime(ActorID idActor, BrainCondition eBrainCondition, uint32 dwTimeoutTime)
{
    ActorAI & tAI = ActorGetAI(idActor);
    uint32 dwNow = GameGetServerTime();
    tAI.dwConditionTimeout = dwNow + dwTimeoutTime;
    tAI.eBrainCondition = eBrainCondition;
}

// Original line 663.
BrainCondition BrainConditionGetCondition(ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    return tAI.eBrainCondition;
}

void BrainAPIDoCharge(ActorID idActor, BrainAttackSettings & tAttackSettings);  // line 2652

// Original line 2253.
void BrainAPIGetRunAwayPlace(ACDID idACDFearTarget, const Target & tTargetFearSource, WorldPlace & placeDest,
                             float flDistanceMin, float flDistanceMax)
{
    ActorCommonData & tACDFearTarget = ACDGet(idACDFearTarget);
    placeDest = tACDFearTarget.GetPlace();
    Vector3D vDelta(0.0f, 0.0f, 0.0f);

    if (tTargetFearSource.eType == TT_ACTOR) {
        ActorCommonData * ptACDSource = ACDTryToGet(tTargetFearSource.idACDTarget);
        if (ptACDSource && ptACDSource->IsAlive()) {
            WorldPlace placeFearSource;
            tTargetFearSource.GetPlace(placeFearSource);
            if (placeDest.idSWorld == placeFearSource.idSWorld) {
                vDelta = placeDest.wp - placeFearSource.wp;
                vDelta.z = 0.0f;
            }
        }
    }

    if (!tACDFearTarget.IsAlive() || vDelta.MagnitudeSquared() < EPSILON * EPSILON) {
        ActorID idActor = (ActorID)ACDGetOwner(idACDFearTarget);
        Actor * ptActor = ActorTryToGet(idActor);
        if (ptActor) {
            vDelta.x = Math::Cos(ptActor->aYaw);
            vDelta.y = Math::Sin(ptActor->aYaw);
            vDelta.z = 0.0f;
        } else {
            vDelta = cgwvXAxis;
        }
    }

    vDelta.Normalize();
    vDelta *= RandRangeFloat_Internal(SGameGlobalsGet().rng, flDistanceMin, flDistanceMax - flDistanceMin);
    placeDest.wp = placeDest.wp + vDelta;
}

// Original line 2558.

#line 2558 "X/Server/AI/BrainComponents.cpp"
BOOL BrainComponentChargeDoPower(
    ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentCharge & tCharge = ptComp->tCharge;


    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);


    if (ptComp->nActiveComponent == ACTIVE_COMPONENT_CHARGE)
    {
        if (tCharge.idACDTarget != ACDID_INVALID)
        {
            ActorCommonData & tACDTarget = ACDGet(tCharge.idACDTarget);
            WorldPlace placeCurrent = tACD.GetPlace();
            WorldPlace placeTarget = tACDTarget.GetPlace();

            if (placeCurrent.idSWorld == placeTarget.idSWorld)
            {

                Vector3D wvBetween;
                WorldPlaceGetVectorBetween(placeCurrent, placeTarget, wvBetween);
                Vector2D wvBetween2D(wvBetween.x, wvBetween.y);


                float flChargerAngle = Math::AngleToDeg(ACDGetYaw(ACDFindByActor(idActor)));
                float flTargetVecAngle = Math::AngleToDeg(wvBetween2D.ToYaw());
                float flAngleDiff = Math::Abs(flChargerAngle - flTargetVecAngle);
                if (flAngleDiff < 25.0f)
                {

                    WorldDistance wdDist = BrainAPIGetDistanceToTarget(idActor, (ActorID)ACDGetOwner(tCharge.idACDTarget));
                    if (tCharge.snoPower == SNO_NONE && wdDist > 6.0f ||
                        wdDist > PowerGetRange(&tACD, AutoPowerDefinitionR(tCharge.snoPower), TRUE))
                    {

                        BrainAttackSettings tAttackSettings;
                        tAttackSettings.snoPower = tACD.AttributesGetPower(1);
                        tAttackSettings.tTarget.InitializeFromACD(tCharge.idACDTarget, FALSE, FALSE);




                        ptComp->nNextComponent = ACTIVE_COMPONENT_IDLE;
                        sChangedComponents(idActor, ptComp);


                        BrainConditionSetCondition(idActor, BRAIN_CONDITION_READY);
                        BrainAPIDoCharge(idActor, tAttackSettings);
                        ptComp->nActiveComponent = ACTIVE_COMPONENT_CHARGE;
                        return TRUE;

                    }
                }
            }
        }


        if (tCharge.snoPower != SNO_NONE)
        {



            ptComp->nNextComponent = ACTIVE_COMPONENT_IDLE;
            sChangedComponents(idActor, ptComp);


            BrainConditionSetCondition(idActor, BRAIN_CONDITION_READY);

            BrainAttackSettings tAttackSettings;
            tAttackSettings.dwAttackWarmUp = 0;
            tAttackSettings.dwAttackWarmUpDelta = 0;
            tAttackSettings.snoPower = tCharge.snoPower;
            tAttackSettings.tTarget = tCharge.tTarget;
            tAttackSettings.dwAttackFlags = (1 << ATTACK_DONT_CLOSE) | (1 << ATTACK_ONLY_DO_ONCE);
            tAttackSettings.uAICoolDownKey = 0;
            tAttackSettings.uAICoolDownTicks = 0;
            tAttackSettings.eAnimTag = TAG_ACTOR_ANIMATION_INVALID;
            tAttackSettings.flSpeed = 0.0f;
            tAttackSettings.idACDCastLocation = ACDID_INVALID;
            BrainAPIDoAttack(idActor, tAttackSettings);




            ptComp->nActiveComponent = ACTIVE_COMPONENT_CHARGE;
            return TRUE;

        }
    }


    return FALSE;
}
#line 593 "X/Server/AI/BrainComponents.cpp"

// Original line 2713.
void sBrainAPIResurrectToCorpseFromTimer(TimerID, DataID, void * pData)
{
    ACDID idACDPlayer = (ACDID)(intptr_t)pData;
    BrainAPIResurrectByType(idACDPlayer, REVIVAL_TYPE_AT_CORPSE_NOW);
}

void GizmoOperate(ActorCommonData & tACDGizmo, ActorCommonData * ptACDOperator, BOOL bForceOperation,
                  BOOL bAlreadyConfirmed);  // SGizmo.cpp:2007
void BrainAPIDoResurrect(ActorID idActor, BOOL bForce);  // line 7464
void BrainAPIApplyResurrectionBuffs(ActorCommonData & tACD);  // line 2902

void SPlayerDisconnect(Player & tPlayer, int32 eQuitReason, BOOL bHonorLogoutTimerRules);  // X/Server/SPlayer.h (QuitReason is int32)
void SGameStatsClearForPlayerDeath(Player & tPlayer);  // X/Server/SGameStats.cpp
void SPortalMovePlayerToTeamSpawnPoint(const Player & tPlayer);  // X/Server/SPortal.cpp
void SPortalMovePlayerToActSpawnPoint(const Player & tPlayer);  // X/Server/SPortal.cpp
void PVPDuelingWarpPlayerToRandomRespawnLocation(Player & tPlayer);  // X/Server/PVPDueling.cpp
void GizmoSetHearthPortal(Player & tPlayer, const WorldPlace & place, BOOL bOpen);  // X/Server/SGizmo.cpp
void SWorldPlayerWarpToLastCheckpoint(const Player & tPlayer);  // X/Server/SWorld.cpp
int32 SPlayerGetUpscalingLevel();  // X/Server/SPlayer.cpp
void SPlayerAddUpscaleBaseBuff(ActorCommonData & tACD, int32 nLevels);  // X/Server/SPlayer.cpp
void SPlayerGhostedBuffApply(ActorCommonData & tACD, BOOL bApply);  // X/Server/SPlayer.cpp

// Original line 2981.
BOOL BrainAPIResurrectByType(ACDID idACD, RevivalType eRevivalType)
{
    if (!ACDIsPlayer(idACD))
        return FALSE;

    Player * ptPlayer = PlayerGetByACD(idACD);
    if (!ptPlayer)
        return FALSE;

    ActorCommonData & tACD = ACDGet(idACD);
    ActorID idPlayer = (ActorID)tACD.idOwner;
    BOOL bHardCoreMode = ptPlayer->tHeroStateData.uPlayerFlags & 1;
    if (bHardCoreMode && !GameTypeIsPvP() && !tACD.IsDueling())
    {
        SPlayerDisconnect(*ptPlayer, 8, TRUE);
        return TRUE;
    }

    PowerAPI_BuffRemoveLookswitchBuffs(tACD);

    if (GameTypeIsPvP())
    {
        SGameStatsClearForPlayerDeath(*ptPlayer);
        SPortalMovePlayerToTeamSpawnPoint(*ptPlayer);
        tACD.AttributesSetInt(ATTRIBUTE_ON_DEATH_ACCOLADE_0, -1);
    }
    else if (tACD.IsDueling() && eRevivalType != REVIVAL_TYPE_IN_TOWN)
    {
        if (eRevivalType != REVIVAL_TYPE_AT_CORPSE_NOW_DO_NOT_WARP)
            PVPDuelingWarpPlayerToRandomRespawnLocation(*ptPlayer);
    }
    else
    {
        switch (eRevivalType)
        {
            case REVIVAL_TYPE_AT_CHECKPOINT:
                SWorldPlayerWarpToLastCheckpoint(*ptPlayer);
                break;

            case REVIVAL_TYPE_IN_TOWN:
                if (ptPlayer->tLastCheckpoint.IsValid())
                    GizmoSetHearthPortal(*ptPlayer, ptPlayer->tLastCheckpoint.place, TRUE);
                SPortalMovePlayerToActSpawnPoint(*ptPlayer);
                break;

            case REVIVAL_TYPE_AT_CORPSE_DELAYED:
            {
                Player * ptOriginalPlayer = ptPlayer;
                ScopedGlobalDefinitionLockR pdefGlobal;
                uint32 dwDelay = (uint32)(pdefGlobal->flPlayerDelayBeforeCorpseRevival * TICKS_PER_SECOND);
                if (dwDelay)
                {
                    uint32 nLocalPlayerCount = 1;
                    for (Player * ptPlayer2 = PlayerGetFirstInGame(); ptPlayer2; ptPlayer2 = PlayerGetNextInGame(ptPlayer2))
                    {
                        if (PlayersAreLocalToEachOther(*ptPlayer2, *ptOriginalPlayer) && ptPlayer2->nPlayerIndex != ptOriginalPlayer->nPlayerIndex)
                            nLocalPlayerCount++;
                    }

                    if (nLocalPlayerCount == 1 && tACD.AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER))
                        dwDelay = 0;

                    ptPlayer->idTimerCorpseResurrect = TimerRegister(idACD, TIMER_OWNER_ACD, dwDelay, (void *)(intptr_t)tACD.id, sBrainAPIResurrectToCorpseFromTimer);
                    uint32 dwNow = GameGetServerTime();
                    tACD.AttributesSetInt(ATTRIBUTE_CORPSE_RESURRECTION_ALLOWED_GAMETIME, dwNow + dwDelay);
                    return FALSE;
                }
                break;
            }

            case REVIVAL_TYPE_AT_CORPSE_NOW:
            case REVIVAL_TYPE_AT_CORPSE_NOW_DO_NOT_WARP:
                break;

            default:
                return FALSE;
        }
    }

    ptPlayer->dwTickLastResurrection = GameGetServerTime();

    {
        ScopedGlobalDefinitionLockR pdefGlobals;
        int32 nLevelGap = pdefGlobals->nApprenticeshipLevelGap;
        int32 nUpscalingLevel = SPlayerGetUpscalingLevel();
        if (tACD.AttributesGetInt(ATTRIBUTE_LEVEL) + nLevelGap < nUpscalingLevel)
            SPlayerAddUpscaleBaseBuff(tACD, nUpscalingLevel - nLevelGap);
    }

    BrainAPIDoResurrect(idPlayer, FALSE);

    if (GameIsParamFlagSet(GPF_APPLY_RESURRECTION_BUFFS))
        BrainAPIApplyResurrectionBuffs(tACD);

    if (GameTypeIsPvP())
    {
        GameCommonData & tGCD = GameGetActiveGameCommonData();
        uint32 dwNow = GameGetServerTime();
        if (tGCD.tGameSyncedData.dwRoundStart != dwNow)
            SPlayerGhostedBuffApply(tACD, FALSE);
    }
    else
        SPlayerGhostedBuffApply(tACD, FALSE);

    return TRUE;
}


// Original line 2740.
void sBrainAPIReviveHirelingFromTimer(TimerID, DataID idOwner, void * pData)
{
    ACDID idACDHeadstone = (ACDID)idOwner;
    ACDID idACDHireling = (ACDID)(intptr_t)pData;
    ActorCommonData * ptACDHeadstone = ACDTryToGet(idACDHeadstone);

    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);

    // The hireling is gone; clean up its headstone.
    if (!ptACDHireling) {
        if (ptACDHeadstone) {
            const ActorID idActorHeadstone = (ActorID)ptACDHeadstone->idOwner;
            ActorFlagForDeletion(idActorHeadstone);
        }
        return;
    }

    // Wait for the owner to be revived first.
    Player * ptPlayer = ptACDHireling->GetPetOwnerPlayer();
    if (ptPlayer) {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer && !ptACDPlayer->IsAlive())
            return;
    }

    if (ptACDHeadstone) {
        // Operating the headstone revives the hireling.
        GizmoOperate(*ptACDHeadstone, NULL, TRUE, FALSE);
    } else {
        ActorID idHireling = (ActorID)ptACDHireling->idOwner;
        BrainAPIDoResurrect(idHireling, FALSE);
    }
}

// Original line 2844.
ACDID GetLastHeadstoneACD(const ActorCommonData & tACDVictim)
{
    ACDNetworkName idANNHeadstone = (ACDNetworkName)tACDVictim.AttributesGetInt(ATTRIBUTE_HEADSTONE_PLAYER_ANN);
    ACDID idACDHeadstone = ACDFromANN(idANNHeadstone);
    return idACDHeadstone;
}

// Original line 2858.
void SetLastHeadstoneACD(ActorCommonData & tACDVictim, const ACDID idACDHeadstone)
{
    tACDVictim.AttributesSetInt(ATTRIBUTE_HEADSTONE_PLAYER_ANN, ANNFromACD(idACDHeadstone));
    ActorCommonData * ptACDHeadstone = ACDTryToGet(idACDHeadstone);
    if (ptACDHeadstone)
        ptACDHeadstone->AttributesSetInt(ATTRIBUTE_HEADSTONE_PLAYER_ANN, ANNFromACD(tACDVictim.id));
}

// Original line 2878.
BOOL DeleteLastHeadstone(ActorCommonData & tACDVictim)
{
    ACDID idACDPreviousHeadstone = GetLastHeadstoneACD(tACDVictim);
    SetLastHeadstoneACD(tACDVictim, ACDID_INVALID);
    ActorCommonData * ptACDHeadstone = ACDTryToGet(idACDPreviousHeadstone);
    if (ptACDHeadstone) {
        const ActorID idActorPreviousHeadstone = (ActorID)ptACDHeadstone->idOwner;
        ActorFlagForDeletion(idActorPreviousHeadstone);
        return TRUE;
    }
    return FALSE;
}

// Original line 3134.
BOOL BrainAPIResurrectToCheckpoint(ACDID idACD)
{
    return BrainAPIResurrectByType(idACD, REVIVAL_TYPE_AT_CHECKPOINT);
}

// Original line 1556.
static BOOL sBrainComponentChooseTarget(ActorID idActor, SNO snoPower, const Target & tTargetInput, Target & tTargetResult)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    if (!tTargetInput.IsNone()) {
        tTargetResult = tTargetInput;
    } else {
        ACDID idACDTarget = ACDID_INVALID;
        ActorID idTarget = BrainAPIActorChooseTarget(idActor, snoPower, FALSE, NULL);
        const Actor * ptTargetActor = ActorTryToGet(idTarget);
        if (ptTargetActor)
            idACDTarget = ptTargetActor->idACD;
        tTargetResult.InitializeFromACD(idACDTarget, FALSE, FALSE);
    }
    const Actor * ptActor = ActorTryToGet(idActor);
    if (tTargetResult.IsInvalidForSource(ACDTryToGet(ptActor->idACD))) {
        sAttackStop(idActor, ptComp);
        sBrainComponentSetStatus(idActor, "Targetless");
        tTargetResult = cgTargetNone;
        return FALSE;
    }
    return TRUE;
}

// Original line 1641.
static void sBrainComponentUpdateSkillTimer(ACDID idACD, SNO snoPower)
{
    BOOL fFoundPowerInSkills = FALSE;
    int32 nPowerToUse = 0;
    SkillDeclaration tSkillDec;
    const ActorCommonData & tACD = ACDGet(idACD);
    if (tACD.hGB.eType == GB_INVALID)
        return;
    for (int32 i = 0; i < 8 && !fFoundPowerInSkills; i++) {
        tSkillDec = MonsterGetSkill(tACD.tSNOName, i);
        if (tSkillDec.snoPower == snoPower) {
            nPowerToUse = i;
            fFoundPowerInSkills = TRUE;
        }
    }
    if (!fFoundPowerInSkills)
        return;
    {
        MonsterSkillDeclaration tMonsterSkillDec = MonsterGetMonsterSkill(tACD.tSNOName, nPowerToUse);
        if (tMonsterSkillDec.flTimer > 0.0f) {
            ActorAI & tAI = ActorGetAI((ActorID)ACDGetOwner(idACD));
            if (tAI.ptBrainComponentData) {
                uint32 dwNow = GameGetServerTime();
                tAI.ptBrainComponentData->tSkillSettings[nPowerToUse].dwDelayTime = dwNow + (uint32)(tMonsterSkillDec.flTimer * 60.0f);
            }
        }
    }
}

// Original line 2020.
static BOOL sEnsureLOSDestination(const Actor * ptActor, WorldPlace & placeDest, const Target & tTarget)
{
    BOOL bPassed;
    WorldPlace placeStart;
    if (ptActor && tTarget.eType == TT_ACTOR) {
        bPassed = tTarget.GetPlace(placeStart);
        if (ptActor->ptSActorMovement) {
            NavCellHandle tNavCell;
            NavCellHandleFromWorldPlace(placeStart, tNavCell);
            ActorCommonData & tACD = ACDGet(ptActor->idACD);
            ACDCollSettings tACDCollSettings = tACD.GetCollSettings();
            WorldCollParams tWorldCollParams(tACDCollSettings);
            ACDID idACDTarget = tTarget.GetACD();
            WorldCollResult tResult;
            bPassed = WorldCollSweptPoint(placeStart.idSWorld, tNavCell, (const Vector2D &)placeStart.wp, (const Vector2D &)placeDest.wp, tWorldCollParams, 0, 0, idACDTarget, tResult);
            if (!tResult.bCollision)
                return bPassed;
        }
        placeDest = placeStart;
    }
    return bPassed;
}

// Original line 3392.
static bool sBrainAPICanDo(ActorID idActor)
{
    if (!ActorTryToGetAI(idActor))
        return false;
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    if (ActorHasBeenKilled(idActor)) {
        AITrace(idActor, "tried but can't BrainAPIDo AI right now cause killed");
        return false;
    }
    if (ActorIsInDeactivatedScene(idActor)) {
        BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
        BrainAPIDoIdle(idActor);
        return false;
    }
    if (BrainGetStanceType(idActor) == STANCE_ATTACHED &&
        ACDGet(ActorGet(idActor)->idACD).IsAttached())
        return false;
    if (ptComp->nActiveComponent == ACTIVE_COMPONENT_CHARGE)
        return false;
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD) {
        ActorMovement * ptMove = ptACD->GetActorMovement();
        if (ptMove && ptMove->IsInArcingMovement())
            return false;
    }
    return true;
}

// Original line 3448.
void BrainAPIDoIdle(ActorID idActor)
{
    if (!ActorHasBrain(idActor) || ActorHasBeenKilled(idActor))
        return;
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_IDLE);
    sBrainComponentSetStatus(idActor, "Idle");
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentCircle & tCircle = ptComp->tCircle;
    tCircle.bFaceTarget = FALSE;
    tCircle.tTarget.Clear();
}

// Original line 3469.
void BrainAPIDoIdleAndFace(ActorID idActor, const WorldPlace & placeTarget)
{
    BrainAPIDoIdle(idActor);
    BrainAPIDoFace(idActor, placeTarget);
}

// Original line 3480.
void BrainAPIDoFace(ActorID idActor, const WorldPlace & placeTarget)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor && ptActor->ptSActorMovement) {
        ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);
        if (ptACD) {
            if (ptACD->GetPlace().idSWorld == placeTarget.idSWorld)
                ptActor->ptSActorMovement->FaceTarget(placeTarget.wp, FALSE);
        }
    }
}

// Original line 3502.
void BrainAPIDoPlayAnimation(const ActorID idActor, const uint32 dwAnimTag, const uint32 dwDuration, const BOOL bAllowPlayingAnimInterrupt, const BOOL bInterruptableAnim, float flUnitStartTime, const BOOL bDontSendToClient, const float flSpeedScalar)
{
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_ANIMATION);
    sBrainComponentSetStatus(idActor, "Anim");
    BrainStanceDoPlayAnimation(idActor, dwAnimTag, dwDuration, bAllowPlayingAnimInterrupt,
                              bInterruptableAnim, flUnitStartTime, bDontSendToClient, flSpeedScalar);
}

// Original line 3521.
void BrainAPIGetAttackSettings(ActorID idActor, BrainAttackSettings & tAttackSettings)
{
    const Actor * ptActor = ActorGet(idActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    tAttackSettings.tTarget = cgTargetNone;
    tAttackSettings.snoPower = tACD.AttributesGetPower(0);
    tAttackSettings.dwAttackWarmUp = tACD.AttributesGetInt(ATTRIBUTE_INITIAL_COOLDOWN_MIN_TOTAL);
    tAttackSettings.dwAttackWarmUpDelta = tACD.AttributesGetInt(ATTRIBUTE_INITIAL_COOLDOWN_DELTA_TOTAL);
    tAttackSettings.dwAttackFlags = 0;
    tAttackSettings.uAICoolDownKey = 0;
    tAttackSettings.uAICoolDownTicks = 0;
    tAttackSettings.eAnimTag = TAG_ACTOR_ANIMATION_INVALID;
    tAttackSettings.flSpeed = 0.0f;
    tAttackSettings.idACDCastLocation = ACDID_INVALID;
    tAttackSettings.eAIStateFrom = AIStateGetCurrState(idActor);
    tAttackSettings.nSuccessfulCasts = 0;
}

// Original line 3831.
void sBrainLookAtCallback(TimerID, DataID idOwner, void *)
{
    ActorID idActor = (ActorID)idOwner;
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor && ptActor->ptAI) {
        BrainComponentData * ptComp = sGetBrainComponentData(idActor);
        if (ptComp && ptComp->tCircle.bFaceTarget && !ptComp->tCircle.tTarget.IsNone()) {
            WorldPlace placeTarget;
            ptComp->tCircle.tTarget.GetPlace(placeTarget);
            BrainAPIDoFace(idActor, placeTarget);
            ptComp->tCircle.idTimer = TimerRegister(idActor, TIMER_OWNER_ACTOR, 1, NULL, sBrainLookAtCallback);
        }
    }
}

// Original line 3856.
void BrainAPIDoLookAt(ActorID idActor, const Target & tTarget, BOOL bTrack)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentCircle & tCircle = ptComp->tCircle;
    TimerUnregister(tCircle.idTimer);
    tCircle.idTimer = TIMERID_INVALID;
    if (!tTarget.IsNone() && tTarget.IsInWorld()) {
        WorldPlace placeTarget;
        tTarget.GetPlace(placeTarget);
        BrainAPIDoFace(idActor, placeTarget);
        if (bTrack) {
            tCircle.bFaceTarget = TRUE;
            tCircle.tTarget = tTarget;
            tCircle.idTimer = TimerRegister(idActor, TIMER_OWNER_ACTOR, 1, NULL, sBrainLookAtCallback);
        }
    }
}

// Original line 3606.

void BrainAPIDoAttack(ActorID idActor, const BrainAttackSettings & tAttackSettings)
{
    ACDID idACD = ACDFindByActor(idActor);
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentAttack & tAttack = ptComp->tAttack;
    if (!sBrainAPICanDo(idActor))
        return;
    if (tAttackSettings.snoPower == SNO_NONE) {
        sReportBadPower(idActor);
        BrainComponentData * ptComp2 = sGetBrainComponentData(idActor);
        sAttackStop(idActor, ptComp2);
        sBrainComponentSetStatus(idActor, "Powerless");
        return;
    }
    if (!BrainConditionIsReady(idActor))
        return;
    BrainStanceSetPower(idActor, tAttackSettings.snoPower);
    if (BrainAPIIsAIAction(tAttackSettings.snoPower)) {
        BrainAPIDoAIAction(idActor, tAttackSettings);
        if (tAttackSettings.tTarget.eType != TT_NONE)
            sBrainComponentUpdateSkillTimer(idACD, tAttackSettings.snoPower);
        return;
    }
    const Actor * ptActor = ActorGet(idActor);
    ActorAI & tAI = *ptActor->ptAI;
    uint32 dwNow = GameGetServerTime();
    tAI.dwDelayBrainActionTime = dwNow;
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_ATTACK);
    sBrainComponentSetStatus(idActor, "Attack");
    BrainConditionSetCondition(idActor, BRAIN_CONDITION_PERFORMING);
    tAttack.tSettings = tAttackSettings;
    ScopedActorDefinitionLockR pdefActor(ptActor->snoActor);
    if (TagMapTagExists(pdefActor->hTagMap, TAG_ACTOR_AI_NEVER_CLOSE_WHILE_ATTACKING) &&
        TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_AI_NEVER_CLOSE_WHILE_ATTACKING) == 1)
        tAttack.tSettings.dwAttackFlags |= 1;
    if (TagMapTagExists(pdefActor->hTagMap, TAG_ACTOR_AI_NEVER_TURN_DURING_COOLDOWN) &&
        TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_AI_NEVER_TURN_DURING_COOLDOWN) == 1)
        tAttack.tSettings.dwAttackFlags |= 0x10;
}

// Original line 3685.
void BrainAPIDoSpecialPower(ActorID idActor, SNO snoPower, const Target & tTarget, uint32 dwTimeOut)
{
    if (!sBrainAPICanDo(idActor))
        return;
    if (snoPower == SNO_NONE) {
        BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
        sReportBadPower(idActor);
        BrainAPIDoIdle(idActor);
        return;
    }
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
        return;
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentSpecialPower & tSpecialPower = ptComp->tSpecialPower;
    tSpecialPower.snoPower = snoPower;
    tSpecialPower.tTarget = tTarget;
    BrainConditionSetCondition(idActor, BRAIN_CONDITION_READY);
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_SPECIAL_POWER);
    uint32 dwNow = GameGetServerTime();
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    PowerSetCoolDown(tACD, dwNow + dwTimeOut);
}

// Original line 3729.

#line 3729 "X/Server/AI/BrainComponents.cpp"
void BrainAPIDoCircle(
    ActorID idActor,
    SNO snoPower,
    const Target & tTarget,
    CircleDirection eCircleDirection,
    BOOL bFaceTarget)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentCircle & tCircle = ptComp->tCircle;

    if (!sBrainAPICanDo(idActor))
    {
        return;
    }

    if (snoPower == SNO_NONE)
    {
        BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
        sReportBadPower(idActor);
        BrainAPIDoIdle(idActor);
        return;
    }


    if (ptComp->nActiveComponent != ACTIVE_COMPONENT_CIRCLE || eCircleDirection == CIRCLE_DIRECTION_RANDOM)
    {
        tCircle.eCircleDirection = eCircleDirection;
    }


    if (tCircle.eCircleDirection == CIRCLE_DIRECTION_RANDOM || tCircle.eCircleDirection == CIRCLE_DIRECTION_RANDOM_ONCE)
    {
        if (RandFloat_Internal(SGameGlobalsGet().rng) < 0.5f)
        {
            tCircle.eCircleDirection = CIRCLE_DIRECTION_CW;
        }
        else
        {
            tCircle.eCircleDirection = CIRCLE_DIRECTION_CCW;
        }
    }

    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_CIRCLE);

    BrainStanceSetIdleAnimation(idActor, TAG_ACTOR_ANIMATION_NEUTRAL);
    sBrainComponentSetStatus(idActor, "Circle");

    tCircle.snoPower = snoPower;
    tCircle.tTarget = tTarget;
    tCircle.bFaceTarget = bFaceTarget;



    if (bFaceTarget)
    {
        BrainAPIDoLookAt(idActor, tTarget, TRUE);
    }
}
#line 1097 "X/Server/AI/BrainComponents.cpp"

// Original line 3790.
void BrainAPIDoBackpedal(ActorID idActor, const Target & tTargetInput, uint32, float, SNO snoPower)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentRunAway & tRunAway = ptComp->tRunAway;
    if (!sBrainAPICanDo(idActor))
        return;
    if (snoPower == SNO_NONE) {
        BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
        sReportBadPower(idActor);
        BrainAPIDoIdle(idActor);
        return;
    }
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_RUNAWAY);
    BrainStanceSetIdleAnimation(idActor, TAG_ACTOR_ANIMATION_NEUTRAL);
    sBrainComponentSetStatus(idActor, "Backpedal");
    tRunAway.snoPower = snoPower;
    tRunAway.tTarget = tTargetInput;
    tRunAway.eTranslateType = TRANSLATE_TYPE_BACKPEDAL;
    BrainConditionSetConditionAndTime(idActor, BRAIN_CONDITION_PERFORMING, 0);
}

// Original line 3885.
void BrainAPIDoRunAway(ActorID idActor, SNO snoPower, const Target & tTarget)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentRunAway & tRunAway = ptComp->tRunAway;
    if (!sBrainAPICanDo(idActor))
        return;
    if (snoPower == SNO_NONE) {
        BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
        sReportBadPower(idActor);
        BrainAPIDoIdle(idActor);
        return;
    }
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_RUNAWAY);
    BrainStanceSetIdleAnimation(idActor, TAG_ACTOR_ANIMATION_NEUTRAL);
    sBrainComponentSetStatus(idActor, "Run Away");
    tRunAway.snoPower = snoPower;
    tRunAway.tTarget = tTarget;
    tRunAway.eTranslateType = TRANSLATE_TYPE_CLOSING;
    BrainConditionSetConditionAndTime(idActor, BRAIN_CONDITION_PERFORMING, 0);
}

// Original line 3920.
void BrainAPIDoFeared(ActorID idActor, ACDID idACDFearSource)
{
    if (idACDFearSource == ACDID_INVALID)
        return;
    if (!sBrainAPICanDo(idActor))
        return;
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID && ACDGet(idACD).AttributesGetInt(ATTRIBUTE_FROZEN))
        return;
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentRunAway & tRunAway = ptComp->tRunAway;
    tRunAway.tTarget.eType = TT_ACTOR;
    tRunAway.tTarget.idACDTarget = idACDFearSource;
    tRunAway.eTranslateType = TRANSLATE_TYPE_CLOSING;
    ActorPowersInterruptPrimary(idActor, TRUE);
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_FEARED);
    BrainStanceSetIdleAnimation(idActor, TAG_ACTOR_ANIMATION_NEUTRAL);
    sBrainComponentSetStatus(idActor, "Feared");
    BrainConditionSetCondition(idActor, BRAIN_CONDITION_PERFORMING);
    PlayerBrainClearQueuedPower(idACD);
}

// Original line 3959.
void BrainAPIDoFrozen(ActorID idActor)
{
    if (!sBrainAPICanDo(idActor))
        return;
    ActorPowersInterruptPrimary(idActor, TRUE);
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD) {
        ActorMovement * ptMove = ptACD->GetActorMovement();
        if (ptMove)
            ptMove->StopNow();
    }
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_FROZEN);
    sBrainComponentSetStatus(idActor, "Frozen");
    PlayerBrainClearQueuedPower(idACD);
}

// Original line 3988.
void BrainAPIDoStun(ActorID idActor)
{
    if (!sBrainAPICanDo(idActor))
        return;
    ActorPowersInterruptPrimary(idActor, TRUE);
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD) {
        ActorMovement * ptMove = ptACD->GetActorMovement();
        if (ptMove)
            ptMove->StopNow();
    }
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_STUNNED);
    sBrainComponentSetStatus(idActor, "Stunned");
    PlayerBrainClearQueuedPower(idACD);
}

// Original line 4017.
void BrainAPIDoForceGrip(ActorID idActor)
{
    if (!sBrainAPICanDo(idActor))
        return;
    ActorPowersInterruptPrimary(idActor, TRUE);
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD) {
        ActorMovement * ptMove = ptACD->GetActorMovement();
        if (ptMove)
            ptMove->StopNow();
    }
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_FORCE_GRIPPED);
    sBrainComponentSetStatus(idActor, "ForceGripped");
    PlayerBrainClearQueuedPower(idACD);
}

// Original line 4046.
void BrainAPIDoBlind(ActorID idActor, uint32 dwDuration)
{
    if (!sBrainAPICanDo(idActor))
        return;
    ActorPowersInterruptPrimary(idActor, TRUE);
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD) {
        ActorMovement * ptMove = ptACD->GetActorMovement();
        if (ptMove)
            ptMove->StopNow();
    }
    uint32 dwNow = GameGetServerTime();
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentBlind & tBlind = ptComp->tBlind;
    tBlind.flWanderRadiusMin = 3.0f;
    tBlind.flWanderRadiusDelta = 7.0f;
    tBlind.dwTimerMin = 30;
    tBlind.dwTimerDelta = 30;
    tBlind.dwTimeNextWander = 0;
    tBlind.dwStartTime = dwNow;
    tBlind.dwEndTime = dwNow + dwDuration;
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_BLIND);
    BrainConditionSetConditionAndTime(idActor, BRAIN_CONDITION_DUMB, dwDuration);
    sBrainComponentSetStatus(idActor, "Blind");
    PlayerBrainClearQueuedPower(idACD);
}

// Original line 4089.
void BrainAPIUndoBlind(ActorID idActor)
{
    BrainConditionSetCondition(idActor, BRAIN_CONDITION_READY);
    BrainAPIDoIdle(idActor);
}

// Original line 4098.
void BrainAPIDoTaunt(ActorID idActor, ACDID idACDTauntTarget, int32 nDuration, BOOL bSuppressTauntEffects)
{
    if (!sBrainAPICanDo(idActor))
        return;
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return;
    if (!ptACD->AttributesGetInt(ATTRIBUTE_UNINTERRUPTIBLE)) {
        ActorMovement * ptMove = ptACD->GetActorMovement();
        if (ptMove) {
            ptMove->StopNow();
            if (idACDTauntTarget != ACDID_INVALID) {
                ActorCommonData * ptACDTauntTarget = ACDTryToGet(idACDTauntTarget);
                if (ptACDTauntTarget)
                    BrainAPIDoFace(idActor, ptACDTauntTarget->GetPlace());
            }
        }
    }
    ptACD->AttributesRefreshTimerGroup(ATT_TAUNT, nDuration <= 0 ? 0x7fffffff : nDuration);
    ActorAISetTauntTarget(idActor, idACDTauntTarget);
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_TAUNTED);
    sBrainComponentSetStatus(idActor, "Taunted");
    PlayerBrainClearQueuedPower(idACD);
    if (!bSuppressTauntEffects)
        ACDPlayEffect(idACD, EFFECT_PLAY_EFFECT_GROUP, GlobalSNOGet(GLOBALSNO_EFFECTGROUP_TAUNT), ACDID_INVALID);
}

// Original line 4160.
void BrainAPIDoCharm(ActorID idActor, int32)
{
    if (!sBrainAPICanDo(idActor))
        return;
    ActorPowersInterruptPrimary(idActor, TRUE);
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD) {
        ActorMovement * ptMove = ptACD->GetActorMovement();
        if (ptMove)
            ptMove->StopNow();
    }
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_IDLE);
    sBrainComponentSetStatus(idActor, "Idle");
    PlayerBrainClearQueuedPower(idACD);
}

// Original line 4263.
void BrainAPIGetWanderSettings(ActorID idActor, BrainWanderSettings & tWanderSettings)
{
    Actor * ptActor = ActorGet(idActor);
    ScopedActorDefinitionLockR pdefActor(ptActor->snoActor);
    tWanderSettings.flMinDistance = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_AI_WANDER_MIN_DISTANCE);
    tWanderSettings.flMaxDistance = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_AI_WANDER_MAX_DISTANCE);
    tWanderSettings.flWanderRadiusMin = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_AI_WANDER_RADIUS_MIN);
    tWanderSettings.flWanderRadiusMax = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_AI_WANDER_RADIUS);
    tWanderSettings.aMaxAngle = Math::DegToAngle(TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_AI_WANDER_MAX_ANGLE));
    tWanderSettings.dwTimerMin = TagMapGetDWORD(pdefActor->hTagMap, TAG_ACTOR_AI_WANDER_IDLE_TIMER_MIN);
    tWanderSettings.dwTimerDelta = TagMapGetDWORD(pdefActor->hTagMap, TAG_ACTOR_AI_WANDER_IDLE_TIMER_DELTA);
    tWanderSettings.dwTagAnimationWalking = TAG_ACTOR_ANIMATION_WALK;
    tWanderSettings.dwTagAnimationIdling = uint32(-1);
    tWanderSettings.bResetWanderCenter = FALSE;
    tWanderSettings.placeWander = cgplaceNull;
}

// Original line 4288.
void BrainAPISetWanderCenter(ActorID idActor, const WorldPlace & wpWanderCenter)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    ptComp->tWander.placeWanderCenter = wpWanderCenter;
}

// Original line 4299.
void BrainAPIDoDefaultWander(ActorID idActor)
{
    BrainWanderSettings tWanderSettings;
    BrainAPIGetWanderSettings(idActor, tWanderSettings);
    BrainAPIDoWander(idActor, tWanderSettings);
}

// Original line 4310.
void BrainAPIDoWander(ActorID idActor, BrainWanderSettings & tWanderSettings)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentWander & tWander = ptComp->tWander;
    if (!sBrainAPICanDo(idActor))
        return;
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_WANDER);
    sBrainComponentSetStatus(idActor, "Wander");
    if (tWanderSettings.flWanderRadiusMax < 0.000001f) {
        BrainAPIDoIdle(idActor);
        return;
    }
    tWander.dwTimerMin = tWanderSettings.dwTimerMin;
    tWander.dwTimerDelta = tWanderSettings.dwTimerDelta;
    tWander.flMinDistance = tWanderSettings.flMinDistance;
    tWander.flMaxDistance = tWanderSettings.flMaxDistance;
    tWander.flWanderRadiusMin = tWanderSettings.flWanderRadiusMin;
    tWander.flWanderRadiusMax = tWanderSettings.flWanderRadiusMax;
    tWander.dwTagAnimation = tWanderSettings.dwTagAnimationWalking;
    tWander.aMaxAngle = tWanderSettings.aMaxAngle;
    if (tWanderSettings.bResetWanderCenter)
        tWander.placeWanderCenter = tWanderSettings.placeWander;
    if (tWanderSettings.dwTagAnimationIdling != uint32(-1))
        BrainStanceSetIdleAnimation(idActor, tWanderSettings.dwTagAnimationIdling);
}

// Original line 4355.
void BrainAPIDoMinionWander(ActorID idActor, BrainWanderSettings & tWanderSettings)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentWander & tWander = ptComp->tWander;
    if (!sBrainAPICanDo(idActor))
        return;
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_WANDER_MINION);
    sBrainComponentSetStatus(idActor, "Wander (Minion)");
    if (tWanderSettings.flWanderRadiusMax < 0.000001f) {
        BrainAPIDoIdle(idActor);
        return;
    }
    tWander.dwTimerMin = tWanderSettings.dwTimerMin;
    tWander.dwTimerDelta = tWanderSettings.dwTimerDelta;
    tWander.flMinDistance = tWanderSettings.flMinDistance;
    tWander.flMaxDistance = tWanderSettings.flMaxDistance;
    tWander.flWanderRadiusMin = tWanderSettings.flWanderRadiusMin;
    tWander.flWanderRadiusMax = tWanderSettings.flWanderRadiusMax;
    tWander.dwTagAnimation = tWanderSettings.dwTagAnimationWalking;
    tWander.aMaxAngle = tWanderSettings.aMaxAngle;
    if (tWanderSettings.bResetWanderCenter)
        tWander.placeWanderCenter = tWanderSettings.placeWander;
    if (tWanderSettings.dwTagAnimationIdling != uint32(-1))
        BrainStanceSetIdleAnimation(idActor, tWanderSettings.dwTagAnimationIdling);
}

// Original line 4400.

void BrainAPIDoClose(ActorID idActor, const Target & tTargetInput, uint32 dwAnimTag, float flSpeed, SNO snoPower)
{
    if (!sBrainAPICanDo(idActor))
        return;
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentWalkTo & tWalkTo = ptComp->tWalkTo;
    Target tTarget;
    if (!sBrainComponentChooseTarget(idActor, SNO_NONE, tTargetInput, tTarget))
        return;
    WorldPlace placeTarget;
    tTarget.GetPlace(placeTarget);
    tWalkTo.placeGotoPoint = placeTarget;
    tWalkTo.wdSpeed = flSpeed;
    tWalkTo.nTranslateType = TRANSLATE_TYPE_CLOSING;
    tWalkTo.dwAnimTag = dwAnimTag;
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_WALK);
    sBrainComponentSetStatus(idActor, "Close");
    BrainConditionSetConditionAndTime(idActor, BRAIN_CONDITION_PERFORMING, 0);
    BrainComponentDoMovement(idActor, tWalkTo.placeGotoPoint, tWalkTo.wdSpeed, tWalkTo.nTranslateType, tWalkTo.dwAnimTag,
                             0.1f, "Closing", tTarget.GetACD(), FALSE, snoPower);
}

// Original line 4580.
inline BOOL BrainAPIIsAIAction(SNO snoAction)
{
    BrainAction eAction = PowerGetBrainAction(snoAction);
    return eAction != BRAIN_ACTION_NONE;
}

// Original line 4452.

void BrainAPIDoRunNearby(ActorID idActor, const Target & tTargetInput, float flDistanceMin, float flDistanceDelta, SNO snoPower)
{
    if (!sBrainAPICanDo(idActor))
        return;
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentWalkTo & tWalkTo = ptComp->tWalkTo;
    Target tTarget;
    if (!sBrainComponentChooseTarget(idActor, SNO_NONE, tTargetInput, tTarget))
        return;
    WorldPlace placeTarget;
    Vector3D wvOffset;
    ACDID idACD = ACDFindByActor(idActor);
    tTarget.GetPlace(placeTarget);
    WorldPlaceGetVectorBetween(placeTarget, ACDGet(idACD).GetPlace(), wvOffset);
    wvOffset.Normalize();
    Angle aRotate = Math::DegToAngle(RandRangeFloat_Internal(SGameGlobalsGet().rng, 35.0f, 100.0f));
    if (RandFloat_Internal(SGameGlobalsGet().rng) < 0.5f)
        aRotate += PI;
    Vector3D::Rotate(cgwvZAxis, aRotate, wvOffset);
    wvOffset *= RandRangeFloat_Internal(SGameGlobalsGet().rng, flDistanceMin, flDistanceDelta);
    WorldPlace placeResult;
    WorldPlaceOffset(placeTarget, wvOffset, placeResult);
    sEnsureLOSDestination(ActorGet(idActor), placeResult, tTarget);
    tWalkTo.placeGotoPoint = placeResult;
    tWalkTo.wdSpeed = 0.0f;
    tWalkTo.nTranslateType = TRANSLATE_TYPE_CLOSING;
    tWalkTo.dwAnimTag = TAG_ACTOR_ANIMATION_NEUTRAL;
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_WALK);
    sBrainComponentSetStatus(idActor, "RunNearby");
    BrainConditionSetConditionAndTime(idActor, BRAIN_CONDITION_PERFORMING, 0);
    BrainComponentDoMovement(idActor, tWalkTo.placeGotoPoint, tWalkTo.wdSpeed, tWalkTo.nTranslateType, tWalkTo.dwAnimTag,
                             0.1f, "Closing", ACDID_INVALID, FALSE, snoPower);
}

// Original line 4544.
void BrainAPIDoWalk(ActorID idActor, const WorldPlace & placeGotoPoint, WorldDistance wdSpeed, TranslateType nTranslateType, uint32 dwAnimTag)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentWalkTo & tWalkTo = ptComp->tWalkTo;
    if (!sBrainAPICanDo(idActor))
        return;
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_WALK);
    sBrainComponentSetStatus(idActor, "Walk");
    tWalkTo.placeGotoPoint = placeGotoPoint;
    tWalkTo.wdSpeed = wdSpeed;
    tWalkTo.nTranslateType = nTranslateType;
    tWalkTo.dwAnimTag = dwAnimTag;
    BrainStanceDoTranslate(idActor, placeGotoPoint, wdSpeed, 0.0f, nTranslateType,
                           dwAnimTag, 0.0f, "BrainAPIDoWalk", ACDID_INVALID, -1);
}

// Original line 5248; externally linked callback.
ACDIterateResult sFindClosestEnemy(const WorldDistance, const ACDID idACDSource, const ACDID idACDFound, void * pData)
{
    ClosestEnemyCallbackData & tCallbackData = *static_cast<ClosestEnemyCallbackData *>(pData);
    ActorCommonData & tACDSource = ACDGet(idACDSource);
    ActorCommonData & tACDFound = ACDGet(idACDFound);
    if (ACDIsHostile(tACDSource, tACDFound, TRUE)) {
        if (tCallbackData.idACDClosest == ACDID_INVALID) {
            tCallbackData.idACDClosest = idACDFound;
            tCallbackData.wdClosestDistanceSq = ACDGetDistanceSquared(tACDSource, tACDFound, FALSE);
        } else {
            WorldDistance wdDistSq = ACDGetDistanceSquared(tACDSource, tACDFound, FALSE);
            if (wdDistSq < tCallbackData.wdClosestDistanceSq) {
                tCallbackData.idACDClosest = idACDFound;
                tCallbackData.wdClosestDistanceSq = wdDistSq;
            }
        }
    }
    return ACD_ITERATE_CONTINUE;
}

// Original line 5287.
ACDID BrainAPIFindClosestEnemy(const ActorID idActor, const WorldDistance wdRadius)
{
    ACDID idACD = ActorGet(idActor)->idACD;
    ClosestEnemyCallbackData tCallbackData;
    tCallbackData.idACDClosest = ACDID_INVALID;
    tCallbackData.wdClosestDistanceSq = 0.0f;
    ACDIterateRadius(wdRadius, idACD, sFindClosestEnemy, &tCallbackData);
    return tCallbackData.idACDClosest;
}

// Original line 5309.
ActiveComponent BrainAPIGetActiveComponent(ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    return ptComp->nActiveComponent;
}

// Original line 5319.
void BrainAPITriggerUpdate(ActorID idActor, BOOL fSuccessful)
{
    if (ActorHasBrain(idActor)) {
        ActorAI & tAI = ActorGetAI(idActor);
        uint32 dwNow = GameGetServerTime();
        tAI.dwDelayBrainUpdateTime = dwNow;
        tAI.dwDelayBrainDeadUpdateTime = dwNow;
        tAI.dwDelayBrainActionTime = dwNow;
        tAI.eBrainCondition = fSuccessful ? BRAIN_CONDITION_READY : BRAIN_CONDITION_ABORTED;
    }
}

BOOL PowerActorCanResurrect(ActorID idActor);  // X/Server/ActorIsAffected.h
void BrainStanceDoResurrect(ActorID idActor);  // BrainStance.cpp:1067
void STrickleTriggerUpdate();  // STrickle.cpp:129
void ScriptEventOnActorResurrected(ACDID idACDActor);  // X/Server/LuaScript/ScriptEvent.h
void SPlayerActivatePet(Player & tPlayer, GBID gbidPet);  // X/Server/SPlayer.h

BOOL PowerIsCoolDownDone(const ActorCommonData & tACD, SNO snoPower);  // X/Common/Powers/PowersCommon.cpp

// Original line 673.
static void sBrainComponentJobInitialize(ActorID idActor)
{
    const Actor * ptActor = ActorGet(idActor);
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentJob & tJob = ptComp->tJob;
    tJob.placeJobCenter = ACDGet(ptActor->idACD).GetPlace();
    tJob.aJobYaw = ptActor->aYaw;
}

// Original line 724.
static void sBrainComponentWanderInitialize(ActorID idActor)
{
    const Actor * ptActor = ActorGet(idActor);
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentWander & tWander = ptComp->tWander;
    tWander.dwTimeNextWander = 0;
    tWander.placeWanderCenter = ACDGet(ptActor->idACD).GetPlace();
}

// Original line 1024.
static void sBrainComponentAttackInitialize(ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentAttack & tAttack = ptComp->tAttack;
    BrainAPIGetAttackSettings(idActor, tAttack.tSettings);
    tAttack.dwWalkCooldownTimer = 0;
    tAttack.dwClosingCooldownTime = 0;
    tAttack.dwWalkTimer = 0;
    tAttack.bAttackQueued = FALSE;
    tAttack.bClosingCooldownQueued = FALSE;
    tAttack.nAttackState = ATTACKSTATE_IDLE;
    tAttack.bNeedWarmUp = FALSE;
}

// Original line 1044.
static void sBrainComponentSkillInitialize(ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    for (int32 i = 0; i < 8; i++) {
        ptComp->tSkillSettings[i].fSkillEnable = TRUE;
        ptComp->tSkillSettings[i].dwDelayTime = 0;
    }
}

// Original line 1059.
static void sBrainComponentTargetInitialize(ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentTarget & tTarget = ptComp->tTarget;
    BrainComponentGlobals * ptGlobals = SGameManager::GameGet().ptBrainComponentGlobals;
    tTarget.listTargets.Initialize(&ptGlobals->poolTargetInfo);
    tTarget.listFriendlies.Initialize(&ptGlobals->poolTargetInfo);
    tTarget.dwLastTimeWithEnemyTarget = 0;
    tTarget.idTarget = ACTORID_INVALID;
    tTarget.bIncrementedAttackerCount = FALSE;
    tTarget.bHasAttackerSlotReserved = FALSE;
    tTarget.dwNextTargetScanTime = 0;
    ActorCommonData & tACD = ACDGet(ACDFindByActor(idActor));
    if (tACD.GetGizmoType() == ACTORGIZMO_SPAWNER) {
        ScopedActorDefinitionLockR pdefActor(tACD.GetActorSNO());
        const CTagMap * hTagMap = pdefActor->hTagMap;
        float flRadius = TagMapTagExists(hTagMap, TAG_ACTOR_BEHAVIOR_RADIUS_TRIGGER_RADIUS) ? TagMapGetFloat(hTagMap, TAG_ACTOR_BEHAVIOR_RADIUS_TRIGGER_RADIUS) : 15.0f;
        tTarget.tAwareness.flIdleRadius = flRadius;
        tTarget.tAwareness.flCombatRadius = flRadius;
        tTarget.tAwareness.flTargetAbandonTime = 0.0f;
    } else if (tACD.IsMonster()) {
        MonsterGetAwareness(tACD.tSNOName, tTarget.tAwareness);
        if (tTarget.tAwareness.flCombatRadius < 1e-06f)
            tTarget.tAwareness.flCombatRadius += 20.0f;
    } else {
        tTarget.tAwareness.flIdleRadius = 0.0f;
        tTarget.tAwareness.flCombatRadius = 0.0f;
        tTarget.tAwareness.flTargetAbandonTime = 0.0f;
    }
    tTarget.tAwareness.flCombatRadius = Math::Max(tTarget.tAwareness.flIdleRadius, tTarget.tAwareness.flCombatRadius);
}

// Original line 3211.
void BrainComponentInitializeForActor(ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    tAI.ptBrainComponentData = SGameManager::GameGet().ptBrainComponentGlobals->poolBrainComponents.Alloc_Tracked();
    SigmaMemoryClear(tAI.ptBrainComponentData, sizeof(BrainComponentData));
    sGetBrainComponentData(idActor)->nActiveComponent = ACTIVE_COMPONENT_IDLE;
    sBrainComponentSkillInitialize(idActor);
    sBrainComponentJobInitialize(idActor);
    sBrainComponentWanderInitialize(idActor);
    sBrainComponentAttackInitialize(idActor);
    sBrainComponentMovementInitialize(idActor);
    sBrainComponentTargetInitialize(idActor);
    ActorGetAI(idActor).eBrainCondition = BRAIN_CONDITION_READY;
}

// Original line 1120.
static void sBrainComponentTargetDispose(ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    tAI.ptBrainComponentData->tTarget.listFriendlies.Dispose();
    tAI.ptBrainComponentData->tTarget.listTargets.Dispose();
}

// Original line 3236.
void BrainComponentDisposeForActor(ActorID idActor)
{
    ActorAI & tAI = ActorGetAI(idActor);
    sBrainComponentTargetDispose(idActor);
    SGameManager::GameGet().ptBrainComponentGlobals->poolBrainComponents.Free(tAI.ptBrainComponentData);
}

// Original line 4195.
void BrainAPIDoCharge(ActorID idActor, BrainAttackSettings & tAttackSettings)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    ActorCommonData & tACD = ACDGet(ACDFindByActor(idActor));
    if (!sBrainAPICanDo(idActor))
        return;
    if (tAttackSettings.snoPower == SNO_NONE) {
        BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
        sReportBadPower(idActor);
        BrainAPIDoIdle(idActor);
        return;
    }
    if (!BrainConditionIsReady(idActor))
        return;
    if (PowerIsCoolDownDone(tACD, tAttackSettings.snoPower) || tAttackSettings.snoPower == SNO_NONE) {
        sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_CHARGE);
        sBrainComponentSetStatus(idActor, "Charge");
        BrainConditionSetCondition(idActor, BRAIN_CONDITION_PERFORMING);
        BrainComponentCharge & tCharge = ptComp->tCharge;
        tCharge.snoPower = tAttackSettings.snoPower;
        tCharge.tTarget = tAttackSettings.tTarget;
        tCharge.idACDTarget = tAttackSettings.tTarget.idACDTarget;
        switch (tCharge.tTarget.eType) {
            case TT_POINT:
                break;
            case TT_ACTOR:
                tCharge.tTarget.InitializeFromPlace(ACDGet(tCharge.tTarget.idACDTarget).GetPlace());
                break;
            default:
                DisplayInternalError(NULL, "", 4254, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
        tCharge.dwAttackFlags = tAttackSettings.dwAttackFlags;
    } else {
        BrainAPIGetAttackSettings(idActor, tAttackSettings);
        BrainAPIDoAttack(idActor, tAttackSettings);
        uint32 dwNow = GameGetServerTime();
        int32 nTimeout = (int32)((PowerGetCoolDown(tACD) - dwNow) * 0.66f);
        ActorAI & tAI = ActorGetAI(idActor);
        uint32 dwTimeout = GameGetServerTime() + nTimeout;
        tAI.eBrainCondition = BRAIN_CONDITION_PERFORMING;
        tAI.dwConditionTimeout = dwTimeout;
    }
}

// Original line 7464.
void BrainAPIDoResurrect(ActorID idActor, BOOL bForce)
{
    if (!bForce && !PowerActorCanResurrect(idActor))
        return;

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    Player * ptPlayer = PlayerGetByACD(idACD);
    if (ptPlayer && (ptPlayer->tHeroStateData.uPlayerFlags & 1) && !GameTypeIsPvP() && !tACD.IsDueling())
        return;

    PowerAPI_BuffRemoveLookswitchBuffs(tACD);
    ActorFlagSet(idActor, ACTOR_DYING, FALSE);
    BrainStanceDoResurrect(idActor);
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_IDLE);
    ActorGetAI(idActor).sBrainComponentStatus = "Idle";

    BOOL bResAsObserver = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_RESURRECT_AS_OBSERVER));
    if (ptPlayer)
    {
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_WAITING_TO_ACCEPT_RESURRECTION), FALSE);
        DeleteLastHeadstone(tACD);
        ACDPlayEffect(idACD, EFFECT_PLAYER_RESURRECT, 0, ACDID_INVALID);
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_HIDDEN), FALSE);
        TimerUnregister(ptPlayer->idTimerForcedResurrect);
        ptPlayer->idTimerForcedResurrect = TIMERID_INVALID;
        TimerUnregister(ptPlayer->idTimerCorpseResurrect);
        ptPlayer->idTimerCorpseResurrect = TIMERID_INVALID;
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_CHECKPOINT_RESURRECTION_ALLOWED_GAMETIME), 0);
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_CHECKPOINT_RESURRECTION_FORCED_GAMETIME), 0);
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_CORPSE_RESURRECTION_ALLOWED_GAMETIME), 0);
        STrickleTriggerUpdate();
        GameTypeIsPvP();
    }
    else if (ACDIsHireling(tACD))
    {
        DeleteLastHeadstone(tACD);
        ACDPlayEffect(idACD, EFFECT_PLAYER_RESURRECT, 0, ACDID_INVALID);
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_HIDDEN), FALSE);
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_CHECKPOINT_RESURRECTION_FORCED_GAMETIME), 0);
        STrickleTriggerUpdate();
    }

    if (bResAsObserver)
    {
        tACD.SetObserver(TRUE);
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_RESURRECT_AS_OBSERVER), FALSE);
    }

    tACD.ResetLifeStats(FALSE, TRUE, TRUE, TRUE, TRUE);
    CombatLog::LogEvent(CombatLog::LOG_EVENT_HEALING_DONE, tACD.id, tACD.id, SNO_NONE, tACD.GetHitPointsMaxTotal(), 0.0f, 6);

    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    ptComp->tDeath.dwTimeToRemoveBody = (uint32)-1;
    ScriptEventOnActorResurrected(idACD);
    if (!ptPlayer)
        return;

    ACDID idACDHireling = PlayerGetHireling(idACD);
    if (idACDHireling != ACDID_INVALID)
    {
        ActorCommonData & tACDHireling = ACDGet(idACDHireling);
        if (!tACDHireling.IsAlive())
            BrainAPIDoResurrect((ActorID)tACDHireling.idOwner, FALSE);
    }

    if (!tACD.IsDueling() && tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LAST_COSMETIC_PET)) != -1)
    {
        SPlayerActivatePet(*ptPlayer, tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LAST_COSMETIC_PET)));
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_LAST_COSMETIC_PET), -1);
    }
}

// Original line 7598.
void BrainAPIWarnAllyAboutTarget(ActorID idActor, ActorCommonData & tACD, Team, ActorID idTargetActor, BOOL bDelay)
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
        return;
    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_TO_PLAYERS_BIT)) {
        ACDID idACDTarget = ACDFindByActor(idTargetActor);
        if (ACDIsPlayer(idACDTarget))
            return;
    }
    uint32 uDelayTicks = bDelay ? RandRange_Internal(SGameGlobalsGet().rng, 30, 60) : 0;
    BrainAPIActorAddTarget(idActor, tACD, idTargetActor, FALSE, TRUE, uDelayTicks, FALSE);
    ActorAI * ptActorAI = ActorTryToGetAI(idActor);
    if (ptActorAI)
        ptActorAI->dwLastAllyWarnTime = GameGetServerTime();
    SGame & tSGame = SGameManager::GameGet();
    BrainComponentGlobals *& ptBrainComponentGlobals = tSGame.ptBrainComponentGlobals;
    if (ptBrainComponentGlobals && ptBrainComponentGlobals->bPreventWarnNearbyAlliesWhenWarned)
        ActorAIResetWarnNearbyAlliesAboutTargetTime(idActor);
}

// Original line 5347.
inline WorldDistance BrainAPIGetDistanceViaWorldPlace(const WorldPlace placeStart, const WorldPlace placeTarget, const WorldDistance wdStartHeight, const WorldDistance wdStartRadius, const WorldDistance wdTargetHeight, const WorldDistance wdTargetRadius, const WorldDistance wdLongReach);

WorldDistance BrainAPIGetDistanceToTarget(const ActorID idActor, const ActorID idTarget)
{
    const Actor * ptTarget = ActorGet(idTarget);
    const Actor * ptActor = ActorGet(idActor);
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    ActorCommonData & tACDTarget = ACDGet(ptTarget->idACD);
    WorldPlace placeActor = tACD.GetPlace();
    WorldPlace placeTarget = tACDTarget.GetPlace();
    if (placeActor.idSWorld != placeTarget.idSWorld)
        return MAX_WORLDDISTANCE;
    WorldDistance wdActorHeight = tACD.GetHeight();
    WorldDistance wdActorRadius = tACD.GetRadius();
    WorldDistance wdTargetHeight = tACDTarget.GetHeight();
    WorldDistance wdTargetRadius = tACDTarget.GetRadius();
    return BrainAPIGetDistanceViaWorldPlace(placeActor, placeTarget, wdActorHeight, wdActorRadius, wdTargetHeight, wdTargetRadius, 0.0f);
}

// Original line 5376.
inline WorldDistance BrainAPIGetDistanceViaWorldPlace(const WorldPlace placeStart, const WorldPlace placeTarget, const WorldDistance wdStartHeight, const WorldDistance wdStartRadius, const WorldDistance wdTargetHeight, const WorldDistance wdTargetRadius, const WorldDistance wdLongReach)
{
    WorldDistance wdDistance = PowerGetDistanceViaWorldPlace(placeStart, placeTarget, wdStartHeight, wdStartRadius, wdTargetHeight, wdTargetRadius, wdLongReach);
    return wdDistance;
}

BOOL PlayerAffectedByDurability(const ActorCommonData & tACDPlayer);  // X/Common/Player.cpp:5096
extern const InventorySlotType cgtInventorySlotsThatTakeDamage[14];  // X/Common/Items.cpp:8952

// Original line 5494.
static void sDamageItem(ActorCommonData & tACDPlayer, const InventorySlotType eSlot, const float flDurabilityPctLoss)
{
    ACDID idACDItem = ACDInventoryGetSquareItem(tACDPlayer, eSlot, IVector2D(0, 0));
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem && ptACDItem->IsItem()) {
        int32 nDurabilityMax = ptACDItem->AttributesGetInt(ATTRIBUTE_DURABILITY_MAX);
        int32 nDurabilityLoss = (int32)(flDurabilityPctLoss * nDurabilityMax);
        ItemModifyDurability(tACDPlayer, *ptACDItem, -nDurabilityLoss);
    }
}

// Original line 5511.
void sProcessDurabilityLoss(ActorCommonData * ptACDPlayer)
{
    if (!ptACDPlayer || !PlayerAffectedByDurability(*ptACDPlayer))
        return;

    int32 nLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);

    ScopedGameBalanceDefinitionLockR ptExpTableDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
    const ExperienceTable & tExpTable = ptExpTableDef->tExperienceTable;

    float flDurabilityPctLoss = tExpTable.ptExperienceLevels[nLevel - 1].flDurabilityLossPct;

    for (int32 nIndex = 0; cgtInventorySlotsThatTakeDamage[nIndex] != SLOT_NONE; nIndex++)
        sDamageItem(*ptACDPlayer, cgtInventorySlotsThatTakeDamage[nIndex], flDurabilityPctLoss);
}

// Original line 5536.
ACDID TryToGetExistingAvenger(PlayerAvengerData & tPlayerAvengerData, ACDID & idACDKiller)
{
    ACDID idAvenger = ACDID_INVALID;
    if (tPlayerAvengerData.bHasAvenger && tPlayerAvengerData.bHasSpawned) {
        ACDID idCheckAvenger = ACDFromANN(tPlayerAvengerData.annTarget);
        ActorCommonData * ptAvenger = ACDTryToGet(idCheckAvenger);
        ActorCommonData * ptACDKiller = ACDTryToGet(idACDKiller);
        if (ptAvenger && ptACDKiller && ACDGetLevelArea(*ptAvenger) == ACDGetLevelArea(*ptACDKiller))
            idAvenger = idCheckAvenger;
    }
    return idAvenger;
}

// Original line 5558.

Player * TryToUpgradeAvenger(ACDID & idACDAvenger)
{
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
    POS pos = listPlayers.GetHeadPosition();
    while (pos) {
        Player * ptOtherPlayer = listPlayers.GetNext(pos);
        if (ptOtherPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE))
            break;

        ACDID idACDExistingAvenger = TryToGetExistingAvenger(ptOtherPlayer->tSoloAvenger, idACDAvenger);
        if (idACDExistingAvenger == ACDID_INVALID)
            idACDExistingAvenger = TryToGetExistingAvenger(ptOtherPlayer->tFriendAvenger, idACDAvenger);

        if (idACDExistingAvenger != ACDID_INVALID) {
            ActorCommonData * ptAvenger = ACDTryToGet(idACDExistingAvenger);
            if (ptAvenger) {
                idACDAvenger = idACDExistingAvenger;
                return ptOtherPlayer;
            }
        }
    }
    return NULL;
}

// Original line 6074.
void BrainAPIRemoveCorpse(ActorID idActorCorpse)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActorCorpse);
    ptComp->tDeath.dwTimeToRemoveBody = GameGetServerTime() + 1;
}

ACDIterateResult sFindDisposableCorpse(const WorldDistance, const ACDID, const ACDID idACDFound, void * ptCallbackData);  // line 6158

// Original line 6219.
void sCleanDisposableCorpsesAround(const ACDID idACD)
{
    const WorldDistance CORPSE_SEARCH_RADIUS = 45.0f;
    ACDIDListPooled tListACDCorpses(MemoryPoolsGetDATAIDListPool());
    ACDIterateRadius(CORPSE_SEARCH_RADIUS, idACD, sFindDisposableCorpse, &tListACDCorpses);

    const int32 CORPSE_LIMIT_IN_AREA = 20;
    int32 nCorpseCount = tListACDCorpses.Count();
    if (nCorpseCount >= CORPSE_LIMIT_IN_AREA) {
        int32 nNumCorpsesToRemove = nCorpseCount - CORPSE_LIMIT_IN_AREA;
        while (nNumCorpsesToRemove--) {
            BrainAPIRemoveCorpse((ActorID)ACDGetOwner((ACDID)tListACDCorpses.GetHead()));
            tListACDCorpses.RemoveHead();
        }
    }
}

// Original line 7586.
static void sReportBadPower(const ActorID idActor)
{
    const Actor * ptActor = ActorGet(idActor);
    (void)ptActor;
}

void GetACDsInRadius(const ActorCommonData & tACD, const float flRadius, XPooledDataIDList & listACDs);  // X/Common/Actors/ActorCollision.cpp:601
BOOL ACDIsFriendly(Team eTeam1, Team eTeam2);  // ActorCommonData.cpp:1891; belongs in ActorCommonData.h
BOOL ACDIsHostile(Team eTeam1, Team eTeam2);  // ActorCommonData.cpp:2013; belongs in ActorCommonData.h
void BrainAPIWarnPackAboutTarget(ActorID idActor, ActorID idTargetActor);  // line 7693

// Original line 7643.

void BrainAPIWarnNearbyAlliesAboutTarget(ActorID idActor, ActorID idTargetActor)
{
    ACDID idACD = ACDFindByActor(idActor);
    ACDID idACDTarget = ACDFindByActor(idTargetActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    if (!ptComp || !ptACD || !ptACDTarget)
        return;

    const BrainComponentTarget & tTarget = ptComp->tTarget;
    Team eActorTeam = ptACD->GetTeam();
    Team eTargetActorTeam = ptACDTarget->GetTeam();

    XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
    GetACDsInRadius(*ptACD, tTarget.tAwareness.flWarnOthersRadius, listACDsInRadius);
    POS pos = listACDsInRadius.GetHeadPosition();
    while (pos) {
        ACDID idACDAlly = (ACDID)listACDsInRadius.GetNext(pos);
        ActorCommonData & tACDAlly = ACDGet(idACDAlly);
        if (!tACDAlly.IsAlive() || tACDAlly.IsWreckableObject())
            continue;

        Team eAllyTeam = tACDAlly.GetTeam();
        if (ACDIsFriendly(eActorTeam, eAllyTeam) && ACDIsHostile(eAllyTeam, eTargetActorTeam)) {
            ActorID idActorAlly = (ActorID)ACDGetOwner(idACDAlly);
            BrainAPIWarnAllyAboutTarget(idActorAlly, tACDAlly, eAllyTeam, idTargetActor, TRUE);
        }
    }
    BrainAPIWarnPackAboutTarget(idActor, idTargetActor);
}

// Original line 7693.
void BrainAPIWarnPackAboutTarget(ActorID idActor, ActorID idTargetActor)
{
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD) {
        ACDPack * ptACDPack = ACDPackTryToGet(ptACD->idACDPack);
        if (ptACDPack) {
            const ACDIDListPooled * listPack = ptACDPack->GetMemberList();
            POS posPack = listPack->GetHeadPosition();
            while (posPack) {
                ACDID idACDAlly = (ACDID)listPack->GetNext(posPack);
                ActorID idActorAlly = (ActorID)ACDGetOwner(idACDAlly);
                ActorCommonData & tACDAlly = ACDGet(idACDAlly);
                Team eAllyTeam = tACDAlly.GetTeam();
                BrainAPIWarnAllyAboutTarget(idActorAlly, tACDAlly, eAllyTeam, idTargetActor, TRUE);
            }
        }
    }
}

// From X/Server/AI/ActorAI.h (not declared in src/ yet).
void ActorAIWasAttacked(ActorID idTargetActor, ActorID idSourceActor);  // ActorAI.cpp:1273
BOOL ActorAICanWarnNearbyAlliesAboutTarget(ActorID idActor);  // ActorAI.cpp:4582
BOOL ActorAIWasAttackedCanAddTarget(ActorID idActor);  // ActorAI.cpp:4603
void ActorAIResetWarnNearbyAlliesAboutTargetTime(ActorID idActor);  // ActorAI.cpp:4624
void ActorAIResetWasAttackedAddTargetCooldown(ActorID idActor);  // ActorAI.cpp:4641
void BrainAPIActorAddTarget(ActorID idActor, ActorID idTarget, BOOL bRequireLOS, BOOL bAddSummoner, uint32 uDelayTicks, BOOL bForceTarget);  // line 9431

// Original line 7723.
void BrainComponentWasAttacked(ActorID idTargetActor, ActorID idSourceActor)
{
    ActorAIWasAttacked(idTargetActor, idSourceActor);
    if (idSourceActor == ACTORID_INVALID)
        return;

    ACDID idACDSource = ACDFindByActor(idSourceActor);
    ActorCommonData & tACDSource = ACDGet(idACDSource);

    ActorID idActorSummoner = ACTORID_INVALID;
    ACDID idACDSummoner = (ACDID)tACDSource.AttributesGetInt(ATTRIBUTE_SUMMONED_BY_ACDID);
    ActorCommonData * ptACDSummoner = ACDTryToGet(idACDSummoner);
    if (ptACDSummoner && ptACDSummoner->GetTeam() == tACDSource.GetTeam())
        idActorSummoner = (ActorID)ptACDSummoner->idOwner;

    Player * ptPlayerOwner = tACDSource.GetPetOwnerPlayer();
    ActorCommonData * ptACDOwner = ptPlayerOwner ? ACDTryToGet(ptPlayerOwner->idPlayerACD) : NULL;
    ActorID idActorOwner = ptACDOwner ? (ActorID)ptACDOwner->idOwner : ACTORID_INVALID;

    if (ActorAIWasAttackedCanAddTarget(idTargetActor)) {
        BrainAPIActorAddTarget(idTargetActor, idSourceActor, FALSE, FALSE, 0, FALSE);
        if (idActorSummoner != ACTORID_INVALID)
            BrainAPIActorAddTarget(idTargetActor, idActorSummoner, FALSE, FALSE, 0, FALSE);
        if (idActorOwner != ACTORID_INVALID)
            BrainAPIActorAddTarget(idTargetActor, idActorOwner, FALSE, FALSE, 0, FALSE);
        ActorAIResetWasAttackedAddTargetCooldown(idTargetActor);
    }

    if (tACDSource.IsAlive() && ACDIsPlayer(idACDSource) && ActorAIWasAttackedCanAddTarget(idSourceActor)) {
        BrainAPIActorAddTarget(idSourceActor, idTargetActor, FALSE, FALSE, 0, FALSE);
        ActorAIResetWasAttackedAddTargetCooldown(idSourceActor);
    }

    if (ptACDOwner && ptACDOwner->IsAlive() && ActorAIWasAttackedCanAddTarget(idActorOwner)) {
        BrainAPIActorAddTarget(idActorOwner, idTargetActor, FALSE, FALSE, 0, FALSE);
        ActorAIResetWasAttackedAddTargetCooldown(idActorOwner);
    }

    if (!ActorAICanWarnNearbyAlliesAboutTarget(idTargetActor))
        return;
    if (ActorTryToGetAI(idTargetActor)) {
        BrainAPIWarnNearbyAlliesAboutTarget(idTargetActor, idSourceActor);
        if (idActorSummoner != ACTORID_INVALID)
            BrainAPIWarnNearbyAlliesAboutTarget(idTargetActor, idActorSummoner);
        if (idActorOwner != ACTORID_INVALID)
            BrainAPIWarnNearbyAlliesAboutTarget(idTargetActor, idActorOwner);
    }
    ActorAIResetWarnNearbyAlliesAboutTargetTime(idTargetActor);
}

// Original line 7974.
int32 BrainAPIGetNumEnemyTargets(ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    const BrainComponentTarget & tTarget = ptComp->tTarget;
    int32 nNumTargets = tTarget.listTargets.Count();
    return nNumTargets;
}

// Original line 7987.
BOOL BrainAPIHasEnemyTargetForInCombatStatus(ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    const BrainComponentTarget & tTarget = ptComp->tTarget;
    ActorCommonData & tACD = ACDGet(ACDFindByActor(idActor));
    WorldPlace placeSource = tACD.GetPlace();
    POS pos = tTarget.listTargets.GetHeadPosition();
    while (pos) {
        const TargetInfo & tActorTarget = tTarget.listTargets.GetNext(pos);
        const Actor * ptTargetActor = ActorTryToGet(tActorTarget.idTarget);
        if (!ptTargetActor)
            continue;
        const float flMaxDistanceSquared = 1600.0f;
        ActorCommonData & tACDTarget = ACDGet(ACDFindByActor(tActorTarget.idTarget));
        if (ACDGetDistanceSquared(tACD, tACDTarget, TRUE) > flMaxDistanceSquared)
            continue;
        {
            NavMeshHandle tNavMeshHandle;
            ACDGetNavMeshHandle(tACD, tNavMeshHandle);
            SceneCommonData * ptSCD = SceneCommonDataTryToGet(tNavMeshHandle.idSScene);
            if (ptSCD && (ptSCD->dwSceneFlags & 1)) {
                NavCellHandle hNavCellStart;
                NavCellHandleFromWorldPlace(placeSource, hNavCellStart);
                if (NavMeshCheckLOSToACD(hNavCellStart, placeSource, tACDTarget.id, 2, FALSE))
                    return TRUE;
            }
        }
    }
    return FALSE;
}

// Original line 8049.
int32 BrainAPIGetNumEnemyPlayerTargets(ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    const BrainComponentTarget & tTarget = ptComp->tTarget;
    int32 nNumTargets = 0;
    POS pos = tTarget.listTargets.GetHeadPosition();
    while (pos) {
        const TargetInfo & tActorTarget = tTarget.listTargets.GetNext(pos);
        Actor * ptTargetActor = ActorTryToGet(tActorTarget.idTarget);
        if (!ptTargetActor)
            continue;
        if (ACDIsPlayer(ptTargetActor->idACD))
            ++nNumTargets;
    }
    return nNumTargets;
}

// Original line 8081.
BOOL BrainAPIActorHasTarget(ActorID idActor, ActorID idTarget)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    const BrainComponentTarget & tTarget = ptComp->tTarget;
    POS pos = tTarget.listTargets.GetHeadPosition();
    uint32 uNow = GameGetServerTime();
    while (pos) {
        const TargetInfo & tActorTarget = tTarget.listTargets.GetNext(pos);
        if (tActorTarget.idTarget == idTarget && uNow >= tActorTarget.dwActiveTime)
            return TRUE;
    }
    pos = tTarget.listFriendlies.GetHeadPosition();
    while (pos) {
        const TargetInfo & tActorTarget = tTarget.listFriendlies.GetNext(pos);
        if (tActorTarget.idTarget == idTarget && uNow >= tActorTarget.dwActiveTime)
            return TRUE;
    }
    return FALSE;
}

BOOL WorldIsRotated(const SNO snoWorld);  // World.cpp:775; belongs in X/Common/World.h

// Original line 8121.
const float cgflOneOverRoot2 = 0.70710678f;

// Original line 8123.
const MonsterAwarenessInfo * BrainAPIActorGetAwareness(ACDID idACD, WorldPlace & placeCenter)
{
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    const BrainComponentTarget & tTargetData = ptComp->tTarget;
    ActorCommonData & tACD = ACDGet(idACD);
    WorldPlace tPlace;

    if (tACD.IsPet()) {
        BOOL bForceSelfAsCenter = FALSE;
        SNO snoMonster = tACD.GetMonsterSNO();
        if (snoMonster != SNO_NONE) {
            ScopedMonsterDefinitionLockR pdefMonster(snoMonster);
            if (pdefMonster->dwFlags & (1 << MONSTERDEF_ALWAYS_USES_SELF_FOR_AWARENESS_RADIUS_BIT))
                bForceSelfAsCenter = TRUE;
        }
        ACDID idACDPetOwner = ACDGetPetOwner(tACD.id);
        ActorCommonData * ptACDPetOwner = ACDTryToGet(idACDPetOwner);
        if (!bForceSelfAsCenter && ptACDPetOwner && ptACDPetOwner->GetPlace().idSWorld == tACD.GetPlace().idSWorld)
            tPlace = ptACDPetOwner->GetPlace();
        else
            tPlace = tACD.GetPlace();
    } else {
        tPlace = tACD.GetPlace();
    }

    Vector3D wvOffset = (cgwvXAxis + cgwvYAxis) * cgflOneOverRoot2;
    Team eTeam = tACD.GetTeam();
    if (WorldIsRotated(WorldCommonGetWorldSNO(tPlace.idSWorld)))
        wvOffset = cgwvXAxis;
    if (GameIsParamFlagSet(GPF_REVERSE_CAMERA_FOR_TEAM_1) ? eTeam != TEAM_PLAYER_1
                                                         : (eTeam >= TEAM_PLAYER_FIRST && eTeam <= TEAM_PLAYER_LAST))
        wvOffset = -wvOffset;
    wvOffset *= tTargetData.tAwareness.flIdleRadius * 0.15f;
    WorldPlaceOffset(tPlace, wvOffset, placeCenter);
    return &tTargetData.tAwareness;
}

// Original line 8202.
float BrainAPIActorGetAwarenessScanRadius(ACDID idACD, WorldPlace & placeCenter)
{
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    const MonsterAwarenessInfo * ptAwareness = BrainAPIActorGetAwareness(idACD, placeCenter);
    float flTargetScanRadius = (AIStateGetCurrState(idActor) == AISTATE_TYPE_IDLE) ? ptAwareness->flIdleRadius : ptAwareness->flCombatRadius;
    return flTargetScanRadius;
}

// Original line 8217.
void BrainAPIActorSetAwarenessRadii(ACDID idACD, float flIdleRadius, float flCombatRadius)
{
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    BrainComponentData * ptBrainComponentData = sGetBrainComponentData(idActor);
    MonsterAwarenessInfo & tAwareness = ptBrainComponentData->tTarget.tAwareness;
    tAwareness.flIdleRadius = flIdleRadius;
    tAwareness.flCombatRadius = flCombatRadius;
}

// Original line 8232.
void BrainAPIActorScanAwarenessForTargets(ACDID idACD, XPooledDataIDList & listACDs)
{
    WorldPlace placeCenter;
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    (void)idActor;
    float flCombatRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);
    ACDsIntersectSphere(placeCenter, flCombatRadius, ACDID_INVALID, listACDs, ACTOR_SPHERE_TREE_FLAGS_ALL);
}

// Original line 8248.
static void sActorAddTarget(ACDID idACDTarget, ActorID idActor, ActorCommonData & tACD, BOOL bRequireLOS)
{
    ActorID idTargetActor = (ActorID)ACDGetOwner(idACDTarget);
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    BrainAPIActorAddTarget(idActor, tACD, idTargetActor, bRequireLOS ? TRUE : !GameTypeIsPvP(), TRUE, 0, FALSE);
    if (tACDTarget.IsAlive() && ACDIsPlayer(idACDTarget))
        BrainAPIPlayerActorAddTarget(idTargetActor, idActor, FALSE);
}

// Original line 8271.
static void sActorAddTargetsInArea(ActorID idActor)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
        return;

    ACDID idACDLeader = ActorGet(idActor)->idACD;
    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    BrainAPIActorScanAwarenessForTargets(idACDLeader, listACDs);

    WorldPlace placeCenter;
    const MonsterAwarenessInfo * ptAwareness = BrainAPIActorGetAwareness(idACDLeader, placeCenter);
    BOOL bRequireLOS = ptAwareness->bRequireLOSForAllTargets;
    ActorCommonData & tACD = ACDGet(idACDLeader);
    POS pos = listACDs.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listACDs.GetNext(pos);
        sActorAddTarget(idACDTarget, idActor, tACD, bRequireLOS);
    }
}

// Original line 8302.
BOOL BrainAPIActorAIIsInactive(const ActorCommonData & tACD)
{
    if (tACD.IsMonster()) {
        ActorID idActor = (ActorID)ACDGetOwner(tACD.id);
        ActorBrain eBrain = ActorGetBrainType(idActor);
        if (eBrain == ACTORBRAIN_SPAWNER || eBrain == ACTORBRAIN_DO_NOTHING)
            return FALSE;
        AIStateType eState = AIStateGetCurrState(idActor);
        if (eState == AISTATE_TYPE_SETUP || eState == AISTATE_TYPE_IDLE)
            return TRUE;
    }
    return FALSE;
}

// Original line 8331.
BOOL BrainAPIIsActorExcludedFromPerformanceOptimizations(const ActorCommonData & tACD)
{
    if (tACD.snoActor == GlobalSNOGet(GLOBALSNO_ACTOR_FROZEN_PULSE) ||
        tACD.snoActor == GlobalSNOGet(GLOBALSNO_ACTOR_WEREWOLF_WHITE) ||
        tACD.snoActor == GlobalSNOGet(GLOBALSNO_ACTOR_WEREWOLF_BLACK))
        return TRUE;
    return FALSE;
}

BOOL ACDInSameWorld(const ActorCommonData & tACD1, const ActorCommonData & tACD2);  // X/Common/ActorCommonData.cpp
BOOL ACDCanAttackTarget(const ActorCommonData & tACD, const Target & tTarget, AutoDefinitionR<PowerDefinition> pdefPower,
                        uint32 * pdwFailureReasons);  // X/Server/AI/ActorAI.cpp

// Original line 8347.
static ActorID sActorChooseClosestTarget(ActorID idActor, SNO snoPower, const TTargetList * pList, BOOL bJustChooseClosest,
                                         BrainAPITargetFilterCallback pfnFilterCallback, BOOL bWantNotKnockbackable,
                                         BOOL bPenalizeInvulnerableNPCs, BOOL bActiveEnemiesOnly)
{
    const float INVULNERABLE_NPC_PENALTY = 100000.0f;
    const float RANDOM_WEIGHT_MAX = 4.0f;
    const float HIRELING_BIAS = 0.0f;
    const float PREVIOUS_TARGET_BONUS = 8.0f;
    const float DISTANCE_WEIGHT = 2.0f;
    const float ATTACKER_WEIGHT = 0.0f;

    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    const BrainComponentTarget & tTarget = ptComp->tTarget;
    float flRandomWeightMax = RANDOM_WEIGHT_MAX;
    ACDID idACDMember = ActorGet(idActor)->idACD;
    ActorCommonData & tACDMember = ACDGet(idACDMember);

    ActorID idBestTarget = ACTORID_INVALID;
    float flBestScore = FLT_MAX;

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    uint32 uNow = GameGetServerTime();

    POS pos = pList->GetHeadPosition();
    while (pos) {
        const TargetInfo & tActorTarget = pList->GetNext(pos);
        BOOL bForcedTarget = tActorTarget.dwLastSeenTime == 0;

        const Actor * ptTargetActor = ActorTryToGet(tActorTarget.idTarget);
        if (!ptTargetActor)
            continue;

        if (uNow < tActorTarget.dwActiveTime)
            continue;

        ACDID idACDTarget = ptTargetActor->idACD;
        ActorCommonData & tACDTarget = ACDGet(idACDTarget);
        if (bActiveEnemiesOnly && BrainAPIActorAIIsInactive(tACDTarget))
            continue;

        if (!ACDInSameWorld(tACDMember, tACDTarget))
            continue;

        if (!bForcedTarget && !tACDTarget.IsTargetable())
            continue;

        if (tACDTarget.IsNecroCorpse())
            continue;

        ACDID idACDTargetAttachedTo = tACDTarget.GetAttachedToACD();
        if (idACDTargetAttachedTo != ACDID_INVALID &&
            idACDTargetAttachedTo != tACDMember.id)
            continue;

        if (snoPower != SNO_NONE) {
            if (ActorHasBeenKilled(tActorTarget.idTarget))
                continue;

            Target tTargetLoc;
            tTargetLoc.InitializeFromACD(idACDTarget, FALSE, bForcedTarget);
            if (!ACDCanAttackTarget(tACDMember, tTargetLoc, AutoDefinitionR<PowerDefinition>(snoPower), NULL))
                continue;
        }

        if (pfnFilterCallback && !pfnFilterCallback(tACDMember.id, idACDTarget))
            continue;

        if (bWantNotKnockbackable && tACDTarget.AttributesGetInt(ATTRIBUTE_IMMUNE_TO_KNOCKBACK))
            continue;

        WorldDistance wdSeparation = WorldPlaceDistance(tACDTarget.GetPlace(), tACDMember.GetPlace());
        float flScore = wdSeparation;

        if (!bJustChooseClosest) {
            int32 nAttackerCount = ptTargetActor->nAttackerCount;
            flScore = DISTANCE_WEIGHT * wdSeparation + ATTACKER_WEIGHT * nAttackerCount;

            if (tActorTarget.idTarget == tTarget.idTarget)
                flScore -= PREVIOUS_TARGET_BONUS;

            if (tACDTarget.IsPet())
                flScore += HIRELING_BIAS;

            float flCustomWeight = tACDTarget.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_CUSTOM_TARGET_WEIGHT, tACDMember.snoActor));
            flScore -= flCustomWeight;

            float flRandomWeightFactor = RandRangeFloat_Internal(tSGameGlobals.rng, 0.0f, flRandomWeightMax);
            flScore -= flRandomWeightFactor;
        }

        if (bPenalizeInvulnerableNPCs && tACDTarget.IsNPC() &&
            (tACDTarget.AttributesGetInt(ATTRIBUTE_INVULNERABLE) || tACDTarget.AttributesGetInt(ATTRIBUTE_NO_DAMAGE)))
            flScore += INVULNERABLE_NPC_PENALTY;

        if (flScore < flBestScore) {
            flBestScore = flScore;
            idBestTarget = tActorTarget.idTarget;
        }
    }
    return idBestTarget;
}

// Original line 8510.
inline BOOL BrainAPIWantsAttackerSlot(ActorCommonData & tACD)
{
    if (tACD.IsNormalMonster())
        return FALSE;
    return TRUE;
}

// Original line 8525.
BOOL sReserveAttackerSlot(Actor * ptActor, Actor * ptActorTarget)
{
    ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);
    if (!ptActorTarget || !ptACD)
        return FALSE;
    if (BrainAPIWantsAttackerSlot(*ptACD)) {
        ScopedGlobalDefinitionLockR pdefGlobals;
        if (ptACD->IsLargeMonster()) {
            if (ptActorTarget->nAttackerLargeSlotsUsed < pdefGlobals->nMaximumLargeAttackersPerPlayer) {
                ptActorTarget->nAttackerLargeSlotsUsed++;
                return TRUE;
            }
        } else {
            if (ptActorTarget->nAttackerSlotsUsed < pdefGlobals->nMaximumAttackersPerPlayer) {
                ptActorTarget->nAttackerSlotsUsed++;
                return TRUE;
            }
        }
    }
    return FALSE;
}

// Original line 8565.
inline void sReleaseAttackerSlot(Actor * ptActor, Actor * ptActorTarget)
{
    ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);
    if (ptACD) {
        if (ptACD->IsLargeMonster())
            ptActorTarget->nAttackerLargeSlotsUsed--;
        else
            ptActorTarget->nAttackerSlotsUsed--;
    }
}

// From X/Server/Ai/TargetEvaluate.h (not in src/ yet).
enum TargetTeam {
    TARGET_TEAM_ENEMY = 0,
    TARGET_TEAM_FRIENDLY = 1,
};
BOOL TargetEvaluateIsOnTargetTeam(ACDID idACD, ACDID idACDTarget, TargetTeam eTargetTeam, BOOL bIgnoreAlive);  // TargetEvaluate.cpp:352

// Original line 8587.
void BrainAPISetTarget(ActorID idActor, ActorID idTarget)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
        return;

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    BOOL bForcedTarget = FALSE;
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentTarget & tTarget = ptComp->tTarget;

    if (ptACD) {
        ACDID idACDForcedEnemy = (ACDID)ptACD->AttributesGetInt(ATTRIBUTE_FORCED_ENEMY_ACDID);
        ActorCommonData * ptACDForcedEnemy = ACDTryToGet(idACDForcedEnemy);
        bForcedTarget = ptACDForcedEnemy && (ActorID)ACDGetOwner(idACDForcedEnemy) == idTarget;
    }

    if (tTarget.idTarget == idTarget && !bForcedTarget) {
        if (!tTarget.bIncrementedAttackerCount)
            return;
        if (tTarget.bHasAttackerSlotReserved)
            return;
        Actor * ptActorTarget = ActorTryToGet(idTarget);
        if (!ptActorTarget)
            return;
        if (sReserveAttackerSlot(ptActor, ptActorTarget))
            tTarget.bHasAttackerSlotReserved = TRUE;
        return;
    }

    if (tTarget.bIncrementedAttackerCount && !bForcedTarget) {
        Actor * ptActorPrevTarget = ActorTryToGet(tTarget.idTarget);
        if (ptActorPrevTarget) {
            ptActorPrevTarget->nAttackerCount--;
            if (tTarget.bHasAttackerSlotReserved)
                sReleaseAttackerSlot(ptActor, ptActorPrevTarget);
            if (ptActorPrevTarget->nAttackerCount < 0)
                ptActorPrevTarget->nAttackerCount = 0;
            if (ptActorPrevTarget->nAttackerSlotsUsed < 0)
                ptActorPrevTarget->nAttackerSlotsUsed = 0;
            if (ptActorPrevTarget->nAttackerLargeSlotsUsed < 0)
                ptActorPrevTarget->nAttackerLargeSlotsUsed = 0;
        }
    }

    tTarget.idTarget = idTarget;
    ACDID idACDTarget = ACDTryFindByActor(idTarget);
    tTarget.bIncrementedAttackerCount =
        TargetEvaluateIsOnTargetTeam(ptActor->idACD, idACDTarget, TARGET_TEAM_ENEMY, FALSE) && !bForcedTarget;
    tTarget.bHasAttackerSlotReserved = FALSE;
    if (tTarget.bIncrementedAttackerCount) {
        Actor * ptActorTarget = ActorTryToGet(tTarget.idTarget);
        if (!ptActorTarget)
            return;
        ptActorTarget->nAttackerCount++;
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACD && ptACDTarget &&
            MonsterGetTargetStyle(ptACD->tSNOName, ptACD->eMonsterRarity) == AITARGET_STYLE_CRUSADER_HEAVENS_FURY) {
            Player * ptPlayer = ptACD->GetPetOwnerPlayer();
            ptACDTarget->AttributesSetInt(MakeAttribKey(ATTRIBUTE_HEAVENS_FURY_ATTACKER, ptPlayer ? ptPlayer->nPlayerIndex : -1), ptACD->id);
        }
        if (sReserveAttackerSlot(ptActor, ptActorTarget))
            tTarget.bHasAttackerSlotReserved = TRUE;
    }
}

// Original line 8709.
void BrainAPIActorReleaseTarget(ActorID idActor)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor || !ptActor->ptAI)
        return;
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentTarget & tTarget = ptComp->tTarget;
    if (tTarget.bIncrementedAttackerCount) {
        Actor * ptActorPrevTarget = ActorTryToGet(tTarget.idTarget);
        if (ptActorPrevTarget) {
            ptActorPrevTarget->nAttackerCount--;
            if (tTarget.bHasAttackerSlotReserved)
                sReleaseAttackerSlot(ptActor, ptActorPrevTarget);
            if (ptActorPrevTarget->nAttackerCount < 0)
                ptActorPrevTarget->nAttackerCount = 0;
            if (ptActorPrevTarget->nAttackerSlotsUsed < 0)
                ptActorPrevTarget->nAttackerSlotsUsed = 0;
            if (ptActorPrevTarget->nAttackerLargeSlotsUsed < 0)
                ptActorPrevTarget->nAttackerLargeSlotsUsed = 0;
        }
    }
    tTarget.idTarget = ACTORID_INVALID;
    tTarget.bIncrementedAttackerCount = FALSE;
    tTarget.bHasAttackerSlotReserved = FALSE;
}

// Original line 8765.
BOOL BrainAPIHasAttackerSlotReservedForTarget(ActorID idActor, ActorID idTarget)
{
    Actor * ptTarget = ActorTryToGet(idTarget);
    if (!ptTarget)
        return TRUE;
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
        return TRUE;
    ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);
    if (!ptACD || !BrainAPIWantsAttackerSlot(*ptACD))
        return TRUE;
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentTarget & tTarget = ptComp->tTarget;
    if (tTarget.idTarget == idTarget)
        return tTarget.bHasAttackerSlotReserved;
    ScopedGlobalDefinitionLockR pdefGlobals;
    return ptTarget->nAttackerSlotsUsed < pdefGlobals->nMaximumAttackersPerPlayer;
}

// Original line 8832.
ActorID BrainAPIGetTauntTarget(ActorID idActor)
{
    const Actor * ptActor = ActorGet(idActor);
    if (!ptActor)
        return ACTORID_INVALID;
    ACDID idACD = ptActor->idACD;
    if (idACD == ACDID_INVALID)
        return ACTORID_INVALID;
    ActorCommonData & tACD = ACDGet(idACD);
    int32 nTaunt = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_ATTRIBUTE_TIMER, ATT_TAUNT));
    if (nTaunt > 0) {
        ActorAI & tAI = ActorGetAI(idActor);
        ACDID idACDTarget = tAI.idACDTauntTarget;
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget && (ptACDTarget->IsAlive() || ptACDTarget->AttributesGetInt(ATTRIBUTE_IS_POWER_PROXY)) &&
            !ACDIsFriendly(tACD, *ptACDTarget, TRUE))
            return (ActorID)ACDGetOwner(idACDTarget);
    }
    return ACTORID_INVALID;
}

// Original line 8878.
ACDID BrainAPIActorGetClosestTarget(ActorID idActor, SNO snoPower, BOOL bTargetFriendlies, BrainAPITargetFilterCallback pfnFilterCallback)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    const BrainComponentTarget & tTarget = ptComp->tTarget;
    BrainAPIUpdateTargets(idActor, FALSE);
    const TTargetList * pList = bTargetFriendlies ? &tTarget.listFriendlies : &tTarget.listTargets;
    ActorID idActorTarget = sActorChooseClosestTarget(idActor, snoPower, pList, FALSE, pfnFilterCallback, FALSE,
                                                      !bTargetFriendlies, FALSE);
    if (idActorTarget == ACTORID_INVALID)
        return ACDID_INVALID;
    return ACDTryFindByActor(idActorTarget);
}

// Original line 8909; externally linked callback.
BOOL sBrainAPITargetFilterExcludeInCombatWithAlly(ACDID idACD, ACDID idACDTarget)
{
    ActorCommonData & tACD = ACDGet(idACD);
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    ACDID idACDLastDamaged = (ACDID)tACDTarget.AttributesGetInt(ATTRIBUTE_LAST_ACD_ATTACKED_BY);
    ActorCommonData * ptACDLastDamaged = ACDTryToGet(idACDLastDamaged);
    if (ptACDLastDamaged && ptACDLastDamaged->IsAlive() && idACDLastDamaged != idACD &&
        ptACDLastDamaged->GetMonsterSNO() == tACD.GetMonsterSNO())
        return FALSE;
    return TRUE;
}

// Original line 8929.
ACDID BrainAPIActorGetClosestPlusAllyTarget(ActorID idActor)
{
    ACDID idACDTarget = BrainAPIActorGetClosestTarget(idActor, SNO_NONE, FALSE, sBrainAPITargetFilterExcludeInCombatWithAlly);
    if (idACDTarget == ACDID_INVALID)
        idACDTarget = BrainAPIActorGetClosestTarget(idActor, SNO_NONE, FALSE, NULL);
    return idACDTarget;
}

// Original line 8944; externally linked callback, returning the raw attribute value.
BOOL sBrainAPITargetFilterExcludeNonLeaders(ACDID, ACDID idACDTarget)
{
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    BOOL bLeader = tACDTarget.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEADER));
    return bLeader;
}

// Original line 8954.
ACDID BrainAPIActorGetClosestFriendlyLeaderTarget(ActorID idActor)
{
    ACDID idACDTarget = BrainAPIActorGetClosestTarget(idActor, SNO_NONE, TRUE, sBrainAPITargetFilterExcludeNonLeaders);
    return idACDTarget;
}

// Original line 8963.

#line 8963 "X/Server/AI/BrainComponents.cpp"
ACDID BrainAPIActorGetHighestThreatTarget(
    ActorID idActor,
    BOOL bActiveEnemiesOnly)
{
    Actor * ptActor = ActorGet(idActor);
    ActorAI * ptAI = ptActor->ptAI;
    ActorCommonData & tACD = ACDGet(ACDFindByActor(idActor));
    if (!ptAI->bMaintainThreatList)
    {

        ptAI->bMaintainThreatList = TRUE;
    }


    POS pos = ptAI->tThreatList.GetHeadPosition();
    ACDID idACDWinner = ACDID_INVALID;
    float flHighestThreat = 0.0f;
    while (pos)
    {
        ThreatListEntry & tThreatListEntry = ptAI->tThreatList.GetNext(pos);
        if (idACDWinner == ACDID_INVALID || tThreatListEntry.flDamageReceived > flHighestThreat)
        {
            ActorCommonData * ptACDTarget = ACDTryToGet(tThreatListEntry.idACD);
            if (ptACDTarget && tACD.GetSWorldID() == ptACDTarget->GetSWorldID())
            {
                if (bActiveEnemiesOnly && BrainAPIActorAIIsInactive(*ptACDTarget))
                {
                    continue;
                }

                idACDWinner = tThreatListEntry.idACD;
                flHighestThreat = tThreatListEntry.flDamageReceived;
            }
        }
    }

    if (idACDWinner != ACDID_INVALID)
    {
        return idACDWinner;
    }
    else if (bActiveEnemiesOnly)
    {
        return ACDID_INVALID;
    }
    else
    {

        return BrainAPIActorGetClosestTarget(idActor, SNO_NONE, FALSE, NULL);
    }
}
#line 2627 "X/Server/AI/BrainComponents.cpp"

// Original line 9017; externally linked callback.
BOOL sBrainAPITargetFilterExcludeLastAttacked(ACDID idACD, ACDID idACDTarget)
{
    ActorCommonData & tACD = ACDGet(idACD);
    ACDID idACDLastAttacked = (ACDID)tACD.AttributesGetInt(ATTRIBUTE_LAST_ACD_ATTACKED);
    return idACDLastAttacked != idACDTarget;
}

// Original line 9032.
ACDID BrainAPIActorGetSwitchOnAttackTarget(ActorID idActor)
{
    ACDID idACDTarget = BrainAPIActorGetClosestTarget(idActor, SNO_NONE, FALSE, sBrainAPITargetFilterExcludeLastAttacked);
    if (idACDTarget == ACDID_INVALID)
        idACDTarget = BrainAPIActorGetClosestTarget(idActor, SNO_NONE, FALSE, NULL);
    return idACDTarget;
}

void BrainAPISetTarget(ActorID idActor, ActorID idTarget);  // line 8587
ACDID BrainAPIChoosePrimaryTarget(ActorCommonData & tACD, BOOL bActiveEnemiesOnly);  // line 11130

// Original line 9048.
ActorID BrainAPIActorChooseTarget(ActorID idActor, SNO snoPower, BOOL bJustChooseClosest, BrainAPITargetFilterCallback pfnFilterCallback)
{
    ActorID idTarget = BrainAPIGetTauntTarget(idActor);
    if (idTarget == ACTORID_INVALID ||
        (snoPower != SNO_NONE && !PowerCastTargetEnemies(AutoPowerDefinitionR(snoPower)))) {

        BrainComponentData * ptComp = sGetBrainComponentData(idActor);
        const BrainComponentTarget & tTarget = ptComp->tTarget;

        BrainAPIUpdateTargets(idActor, FALSE);

        if (snoPower == SNO_NONE) {
            idTarget = sActorChooseClosestTarget(idActor, snoPower, &tTarget.listTargets, bJustChooseClosest, pfnFilterCallback,
                                                 FALSE, TRUE, FALSE);
        } else if (PowerCastTargetEnemies(AutoPowerDefinitionR(snoPower))) {
            ACDID idACD = ACDFindByActor(idActor);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            idTarget = ACTORID_INVALID;
            if (ptACD) {
                ACDID idACDTarget = BrainAPIChoosePrimaryTarget(*ptACD, FALSE);
                idTarget = (ActorID)ACDGetOwner(idACDTarget);
            }
        } else if (PowerCastTargetAllies(AutoPowerDefinitionR(snoPower))) {
            idTarget = sActorChooseClosestTarget(idActor, snoPower, &tTarget.listFriendlies, bJustChooseClosest, pfnFilterCallback,
                                                 FALSE, FALSE, FALSE);
        }
    }

    BrainAPISetTarget(idActor, idTarget);
    return idTarget;
}

// Original line 9102.
inline void sRemoveEnemyTarget(ActorID idActor, ActorID idTarget)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    if (ptComp) {
        BrainComponentTarget & tTarget = ptComp->tTarget;
        TTargetList * pList = &tTarget.listTargets;
        POS pos = pList->GetHeadPosition();
        while (pos) {
            TargetInfo & tTargetInfo = pList->GetNext(pos);
            if (tTargetInfo.idTarget == idTarget) {
                // The node's data is its first member, so the info's address is its position.
                pList->RemoveAt((POS)&tTargetInfo);
            }
        }
    }
}

// Original line 9128.
static void sActorRemoveInvalidTargets(ActorID idActor, TTargetList * pList, TargetTeam eTargetType)
{
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return;

    BrainComponentGlobals * ptBrainComponentGlobals = SGameManager::GameGet().ptBrainComponentGlobals;
    WorldPlace placeCenter;
    const MonsterAwarenessInfo * ptAwareness = BrainAPIActorGetAwareness(ptACD->id, placeCenter);
    uint32 dwNow = GameGetServerTime();
    BOOL fNavCellInitialized = FALSE;
    BOOL bNormalMonster = ptACD->IsNormalMonster();
    NavCellHandle hNavCellStart = { -1, 0 };

    POS pos = pList->GetHeadPosition();
    while (pos)
    {
        POS posCur = pos;
        TargetInfo tTarget = pList->GetNext(pos);
        const Actor * ptActorTarget = ActorTryToGet(tTarget.idTarget);
        if (!ptActorTarget)
        {
            pList->RemoveAt(posCur);
            continue;
        }

        {
            ACDID idACDTarget = ptActorTarget->idACD;
            ActorCommonData & tACDTarget = ACDGet(idACDTarget);
            BOOL bHostile = ACDIsHostile(*ptACD, tACDTarget, FALSE);
            BOOL bForcedTarget = tTarget.dwLastSeenTime == 0;
            BOOL bTargetCanBeSeen = TRUE;

            if (!bForcedTarget)
            {
                if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
                    bTargetCanBeSeen = FALSE;
                else if ((SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_TO_PLAYERS_BIT) || sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_FLYTHROUGH_BIT)) &&
                         bHostile && ACDIsPlayer(tACDTarget.id))
                    bTargetCanBeSeen = FALSE;
                else if (bHostile && tACDTarget.IsStealthed())
                    bTargetCanBeSeen = FALSE;
                else if (tACDTarget.IsWreckableObject())
                    bTargetCanBeSeen = FALSE;
                else if (!tACDTarget.IsAlive())
                    bTargetCanBeSeen = FALSE;
            }

            if (bTargetCanBeSeen &&
                !tACDTarget.AttributesGetInt(ATTRIBUTE_CANNOT_BE_ADDED_TO_AI_TARGET_LIST) &&
                tACDTarget.GetLocation().eActorLocation == AL_WORLD &&
                TargetEvaluateIsOnTargetTeam(ptACD->id, tACDTarget.id, eTargetType, bForcedTarget) &&
                ACDInSameWorld(*ptACD, tACDTarget))
            {
                if (ptAwareness->bRequireLOSForAllTargets)
                {
                    WorldPlace placeSource = ptACD->GetPlace();
                    if (!fNavCellInitialized)
                    {
                        NavCellHandleFromWorldPlace(placeSource, hNavCellStart);
                        fNavCellInitialized = TRUE;
                    }
                    bTargetCanBeSeen = NavMeshCheckLOSToACD(hNavCellStart, placeSource, idACDTarget, 2  /* PASSABILITY_PROJECTILE */, FALSE);
                }

                if (bTargetCanBeSeen)
                {
                    if (bForcedTarget)
                        continue;

                    float flAbandonDistance = ptAwareness->flCombatRadius;
                    float flAbandonTime = ptAwareness->flTargetAbandonTime;
                    if (bNormalMonster && bHostile)
                    {
                        int32 nAttackerCount = ptActorTarget->nAttackerCount;
                        Player * ptPlayerOwner = tACDTarget.GetPetOwnerPlayer();
                        if (ptPlayerOwner)
                        {
                            Actor * ptActorOwner = ActorTryToGet((ActorID)ACDGetOwner(ptPlayerOwner->idPlayerACD));
                            if (ptActorOwner)
                                nAttackerCount += ptActorOwner->nAttackerCount;
                        }

                        int32 nAttackerCap = ptBrainComponentGlobals->uAttackersPerPlayerThrottlingThreshold;
                        if (nAttackerCap > 0 && nAttackerCount >= nAttackerCap)
                        {
                            flAbandonDistance = Math::Min(flAbandonDistance, ptBrainComponentGlobals->flMaxThrottledAbandonDistance);
                            flAbandonTime = Math::Min(flAbandonTime, ptBrainComponentGlobals->flMaxThrottledAbandonTime);
                        }
                    }

                    WorldDistance wdSeparation = WorldPlaceDistance(tACDTarget.GetPlace(), placeCenter);
                    if (wdSeparation < flAbandonDistance)
                        continue;

                    {
                        uint32 dwAbandonTicks = Math::FloatToInt(flAbandonTime * TICKS_PER_SECOND);
                        if (dwNow <= tTarget.dwLastSeenTime + dwAbandonTicks)
                            continue;
                    }
                }
            }

            if (eTargetType == TARGET_TEAM_ENEMY && ACDIsPlayer(tACDTarget.id))
                sRemoveEnemyTarget(tTarget.idTarget, idActor);
        }

        pList->RemoveAt(posCur);
    }
}

float HeroGetAwarenessRadius(ACDID idACDPlayer);  // Hero.cpp:66; belongs in X/Common/Hero.h (not in src/ yet)

// Original line 9293.
#line 9293 "X/Server/AI/BrainComponents.cpp"
static void sActorPlayerRemoveInvalidTargets(
    ActorID idActor,
    TTargetList * pList)
{

    ACDID idACDPlayer = ACDFindByActor(idActor);
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);


    float flPlayerAwarenessDist = HeroGetAwarenessRadius(idACDPlayer);
    float flPlayerAwarenessDistSqr = flPlayerAwarenessDist * flPlayerAwarenessDist;
    WorldPlace placePlayer = tACDPlayer.GetPlace();

    POS pos = pList->GetHeadPosition();
    while (pos)
    {
        POS posCur = pos;
        TargetInfo tTarget = pList->GetNext(pos);

        const Actor * ptActorTarget = ActorTryToGet(tTarget.idTarget);
        if (ptActorTarget)
        {
            ACDID idACDTarget = ptActorTarget->idACD;
            ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);

            if (ptACDTarget &&
                ptACDTarget->IsAlive())
            {
                WorldPlace placeTarget = ptACDTarget->GetPlace();
                if (placePlayer.idSWorld == placeTarget.idSWorld)
                {
                    float flCheckRadiusSqr;
                    if (ACDIsPlayer(idACDTarget))
                    {
                        flCheckRadiusSqr = flPlayerAwarenessDistSqr;
                    }
                    else if (ptACDTarget->IsMonster())
                    {
                        float flAbandonRadius = MonsterGetScanRadius(ptACDTarget->tSNOName);
                        flCheckRadiusSqr = flAbandonRadius * flAbandonRadius;
                    }
                    else
                        continue;
                    if (flCheckRadiusSqr <= 1e-12f || Vector3D::Distance2DSquared(placePlayer.wp, placeTarget.wp) <= flCheckRadiusSqr)
                    {
                        continue;
                    }

                }
            }
        }

        pList->RemoveAt(posCur);
    }
}
#line 2856 "X/Server/AI/BrainComponents.cpp"

// Original line 9352.
#line 9352 "X/Server/AI/BrainComponents.cpp"
static void sActorTargetScan(
    ActorID idActor)
{

    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentTarget & tTarget = ptComp->tTarget;

    ACDID idACD = ACDFindByActor(idActor);
    if (ACDIsPlayer(idACD))
    {
        sActorPlayerRemoveInvalidTargets(idActor, &tTarget.listTargets);
    }
    else
    {
        sActorRemoveInvalidTargets(idActor, &tTarget.listTargets, TARGET_TEAM_ENEMY);
        sActorRemoveInvalidTargets(idActor, &tTarget.listFriendlies, TARGET_TEAM_FRIENDLY);
        sActorAddTargetsInArea(idActor);
    }
}
#line 2874 "X/Server/AI/BrainComponents.cpp"

// Original line 9375.

#line 9375 "X/Server/AI/BrainComponents.cpp"
void BrainAPIUpdateTargets(
    ActorID idActor,
    BOOL bForceUpdate)
{

    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentTarget & tTarget = ptComp->tTarget;

    uint32 dwNow = GameGetServerTime();

    if (tTarget.dwNextTargetScanTime < dwNow || bForceUpdate)
    {
        sActorTargetScan(idActor);

        if (tTarget.listTargets.Count() > 0)
        {
            tTarget.dwLastTimeWithEnemyTarget = dwNow;
        }



        uint32 dwScanInterval = RandRange_Internal(SGameGlobalsGet().rng, 50, 70 - 50);


        tTarget.dwNextTargetScanTime = dwNow + dwScanInterval;
    }
}
#line 2892 "X/Server/AI/BrainComponents.cpp"

// Original line 9405.
#include "../../Common/Nav/NavigationMesh.h"
ActorID GizmoAddBySNO(int32 nLevel, ActorAddStruct & tActorAdd);  // X/Server/Actors/Actors.cpp:5803
#line 2897 "X/Server/AI/BrainComponents.cpp"

static void sCreateHeadstone(
    ActorCommonData & tACDVictim)
{
    DeleteLastHeadstone(tACDVictim);

    // constant flags, no code of their own (folded into the store on 2914: mov #0x4d0 = bits 4, 6, 7, 10)
    uint32 dwFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) |
                     (1 << ACTORADD_DONT_PERTURB) |
                     (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION) |
                     (1 << ACTORADD_DOESNT_DEACTIVATE);

    WorldPlace tPlaceHeadstone = tACDVictim.GetPlace();
    ActorAddStruct tActorAdd;

    BOOL bVictimIsHireling = ACDIsHireling(tACDVictim);
    tActorAdd.snoActor = (bVictimIsHireling ? GlobalSNOGet(GLOBALSNO_ACTOR_HIRELING_HEADSTONE) : GlobalSNOGet(GLOBALSNO_ACTOR_PLAYER_HEADSTONE));
    tActorAdd.dwActorAddFlags = dwFlags;
    tActorAdd.tPlace = tPlaceHeadstone;
    ScenesCommonNavMeshNodeFromPlace(tActorAdd.tPlace, &tActorAdd.tNavMeshHandle, FALSE, FALSE);

    ACDID idACDNewHeadstone = ACDID_INVALID;
    if (NavMeshHandleIsNone(tActorAdd.tNavMeshHandle))
    {
        // no navmesh under the corpse: no headstone. Lines 2920-2929 leave no code;
        // this branch only keeps ACDID_INVALID (2919:0 mov w24, #-1). Their original
        // text is unknown (comments, a compiled-out trace or assert).



    }
    else
    {
        tActorAdd.tPlace.idSWorld = tPlaceHeadstone.idSWorld;
        tActorAdd.idMarkersSScene = ACDGetCurrentSSceneID(tACDVictim);

        // 0x3f490fdb = PI / 4
        Angle aAngle = PI / 4.0f;
        tActorAdd.qOrientation.FromYaw(aAngle);

        // nLevel -1 (2935:32 mov w24, #-1 feeds w0 of GizmoAddBySNO)
        ActorID idActorNewHeadstone = GizmoAddBySNO(-1, tActorAdd);
        idACDNewHeadstone = ACDTryFindByActor(idActorNewHeadstone);
    }


    SetLastHeadstoneACD(tACDVictim, idACDNewHeadstone);

    uint32 dwNow = GameGetServerTime();

    // DefinitionLockR<GlobalDefinition> rows on 2948
    ScopedGlobalDefinitionLockR pdefGlobal;

    uint32 dwDelay;
    if (bVictimIsHireling)
    {
        dwDelay = (int)(pdefGlobal->ptServerData->flHirelingResurrectionTimer) * TICKS_PER_SECOND;
        TimerRegister(idACDNewHeadstone, TIMER_OWNER_ACD, dwDelay, (void*)tACDVictim.id, sBrainAPIReviveHirelingFromTimer);
        tACDVictim.AttributesSetInt(ATTRIBUTE_CHECKPOINT_RESURRECTION_FORCED_GAMETIME, dwNow + dwDelay);
    }
    else
    {
        Player * ptPlayer = PlayerGetByACD(tACDVictim.id);
        if (ptPlayer)
        {
            float flReviveTime = pdefGlobal->ptServerData->flPlayerBaseResurrectionDelay;

            // boss fights: no delay when alone, else the boss-fight delay
            if (tACDVictim.AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER))
            {
                flReviveTime = PlayerGetNumPlayers() == 1 ? 0 : pdefGlobal->flPlayerBaseBossFightResurrectionDelay;
            }

            tACDVictim.AttributesSetInt(ATTRIBUTE_CHECKPOINT_RESURRECTION_ALLOWED_GAMETIME, dwNow + (int)(flReviveTime * TICKS_PER_SECOND));
        }
    }


    STrickleTriggerUpdate();
}

#line 2894 "X/Server/AI/BrainComponents.cpp"
uint32 BrainAPIGetTimeSinceEnemyTarget(ActorID idActor)
{
    BrainAPIUpdateTargets(idActor, FALSE);
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    const BrainComponentTarget & tTarget = ptComp->tTarget;
    if (tTarget.listTargets.Count() > 0)
        return 0;
    uint32 dwNow = GameGetServerTime();
    uint32 dwLastTimeWithEnemyTarget = Math::Min(dwNow, tTarget.dwLastTimeWithEnemyTarget);
    return dwNow - dwLastTimeWithEnemyTarget;
}

// Original line 10015.
const TTargetList * BrainAPIGetEnemyTargetList(ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    const TTargetList * pList = 0;
    if (ptComp) {
        const BrainComponentTarget & tTarget = ptComp->tTarget;
        pList = &tTarget.listTargets;
    }
    return pList;
}

// Original line 10031.
const TTargetList * BrainAPIGetFriendlyTargetList(ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    const TTargetList * pList = 0;
    if (ptComp) {
        const BrainComponentTarget & tTarget = ptComp->tTarget;
        pList = &tTarget.listFriendlies;
    }
    return pList;
}

// Original line 10314.

static void sSendFloatingMessageToPlayer(Player * ptPlayer, const ACDID idACDTarget, const FloatingNumberType eType)
{
    FloatingNumberMessage tMessage;
    tMessage.annTarget = ANNFromACD(idACDTarget);
    tMessage.flDamage = 0.0f;
    tMessage.eType = eType;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(ptPlayer);
    MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));
}

// Original line 10331.
void BrainAPIDodged(ACDID idACDAttacker, ACDID idACDDefender)
{
    if (ACDIsPlayer(idACDAttacker)) {
        Player * ptPlayer = PlayerGetByACD(idACDAttacker);
        if (ptPlayer)
            sSendFloatingMessageToPlayer(ptPlayer, idACDDefender, FLOATING_NUMBER_TYPE_DODGE);
    }
    if (ACDIsPlayer(idACDDefender)) {
        Player * ptPlayer = PlayerGetByACD(idACDDefender);
        if (ptPlayer) {
            sSendFloatingMessageToPlayer(ptPlayer, idACDDefender, FLOATING_NUMBER_TYPE_DODGED);
            ACDPlayEffectForPlayer(ptPlayer->idPlayerACD, idACDDefender, EFFECT_DODGED_LOCAL, 0);
        }
    }
}

// Original line 10359.
void BrainAPIBlocked(ACDID idACDAttacker, ACDID idACDDefender)
{
    if (ACDIsPlayer(idACDAttacker)) {
        Player * ptPlayer = PlayerGetByACD(idACDAttacker);
        if (ptPlayer)
            sSendFloatingMessageToPlayer(ptPlayer, idACDDefender, FLOATING_NUMBER_TYPE_BLOCK);
    }
    if (ACDIsPlayer(idACDDefender)) {
        Player * ptPlayer = PlayerGetByACD(idACDDefender);
        if (ptPlayer) {
            sSendFloatingMessageToPlayer(ptPlayer, idACDDefender, FLOATING_NUMBER_TYPE_BLOCK);
            ACDPlayEffectForPlayer(ptPlayer->idPlayerACD, idACDDefender, EFFECT_BLOCKED_LOCAL, 0);
        }
    }
}

// Original line 10083.
void BrainAPIGetFrontOfPlace(const ActorCommonData & tACDTarget, float flDist, WorldPlace & placeTarget)
{
    placeTarget = tACDTarget.GetPlace();
    Angle aTargetYaw = ACDGetYaw(tACDTarget.id);
    Vector3D vOffset(Math::Cos(aTargetYaw), Math::Sin(aTargetYaw), 0.0f);
    vOffset *= flDist;
    placeTarget.wp += vOffset;
}

void PowerAPI_RecomputeHitpointHealTarget(ActorCommonData * ptACD);  // PowerAPI.cpp:2269; not in PowerAPI.h yet

// Original line 10387.

#line 10387 "X/Server/AI/BrainComponents.cpp"
void BrainAPIApplyResurrectionBuffs(
    ActorCommonData & tACD)
{
    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_RESURRECTION_BUFF);
    PowerID idPower = PowerStart(tACD.id, snoPower, cgTargetNone);
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }


    float flSeconds = PowerGetFormulaValue(idPower, TAG_POWER_RESURRECTION_BUFF_TIME);
    uint32 uTicks = Math::FloatToInt(flSeconds * TICKS_PER_SECOND);


    float flHPMax = tACD.GetHitPointsMaxTotal();
    float flHealthMultToStart = PowerGetFormulaValue(idPower, TAG_POWER_RESURRECTION_HEALTH_MULT_TO_START);
    float flHPCurr = Math::Pin(flHPMax * flHealthMultToStart, 0.0f, flHPMax);
    tACD.SetHitPoints(flHPCurr, TRUE, FALSE);

    float flHealAmount = flHPMax - flHPCurr;
    if (flHealAmount > EPSILON && flSeconds > EPSILON && uTicks > 0)
    {
        float flRegenPerSecond = flHealAmount / flSeconds;

        BuffID idBuff = PowerAPI_BuffFindOnACD(ptPower->snoPower, tACD, ACDID_INVALID);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff)
        {
            int32 nTicksLeft = 0;
            uint32 uNow = GameGetServerTime();
            uint32 uEndTime = ptBuff->dwStartTick + ptBuff->dwDuration;
            if (uEndTime > uNow)
            {
                nTicksLeft = uEndTime - uNow;
            }

            int32 nDeltaTicks = uTicks - nTicksLeft;
            PowerAPI_BuffModifyDuration(idBuff, nDeltaTicks, FALSE, TRUE);
        }
        else
        {
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
            tBuffStartData.nMaxStackCount = 0;
            tBuffStartData.dwDurationMin = uTicks;
            FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_HEALTH_GLOBE, flRegenPerSecond);
            PowerAPI_BuffAddToACD(tBuffStartData, tACD);
            PowerAPI_RecomputeHitpointHealTarget(&tACD);
        }
    }
}
#line 3032 "X/Server/AI/BrainComponents.cpp"

// Original line 10443.
ACDID BrainAPIGetActorAttackTarget(ACDID idACDAttacker)
{
    ACDID idACDTarget = ACDID_INVALID;
    ActorID idActor = (ActorID)ACDGetOwner(idACDAttacker);
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor) {
        BrainComponentData * ptComp = sGetBrainComponentData(idActor);
        if (ptComp->nActiveComponent == ACTIVE_COMPONENT_ATTACK)
            idACDTarget = ptComp->tAttack.tSettings.tTarget.GetACD();
    }
    return idACDTarget;
}

// Original line 9426.
void BrainAPIActorAddTarget(ActorID idActor, ActorID idTarget, BOOL bRequireLOS, BOOL bAddSummoner, uint32 uDelayTicks, BOOL bForceTarget)
{
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    BrainAPIActorAddTarget(idActor, tACD, idTarget, bRequireLOS, bAddSummoner, uDelayTicks, bForceTarget);
}

// Original line 9441.
inline void BrainAPIPlayerActorAddTarget(ActorID idActor, ActorID idTarget, BOOL bRequireLOS)
{
    BrainAPIActorAddTarget(idActor, idTarget, bRequireLOS, FALSE, 0, FALSE);
}

// Original line 9705.

void BrainAPIReleaseAllTargets(ActorID idActor)
{
    if (idActor == ACTORID_INVALID || !ActorTryToGetAI(idActor))
        return;

    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    if (ptComp) {
        BrainComponentTarget & tTarget = ptComp->tTarget;
        TTargetList * pList = &tTarget.listTargets;

        POS pos = pList->GetHeadPosition();
        while (pos) {
            TargetInfo & tTargetInfo = pList->GetNext(pos);
            Actor * ptActorTarget = ActorTryToGet(tTargetInfo.idTarget);
            if (!ptActorTarget)
                continue;

            if (ACDIsPlayer(ptActorTarget->idACD)) {
                sRemoveEnemyTarget(tTargetInfo.idTarget, idActor);
            }
        }

        pList->RemoveAll();

        pList = &tTarget.listFriendlies;
        pList->RemoveAll();
    }
}

// Original line 10097.
void BrainAPIDoRunInFront(ActorID idActor, const Target & tTarget, float flDist, uint32 dwAnimTag, SNO snoPower)
{
    if (!sBrainAPICanDo(idActor))
        return;
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentWalkTo & tWalkTo = ptComp->tWalkTo;
    ACDID idACDTarget = tTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget) {
        WorldPlace placeTarget;
        BrainAPIGetFrontOfPlace(*ptACDTarget, flDist, placeTarget);
        tWalkTo.placeGotoPoint = placeTarget;
        tWalkTo.wdSpeed = 0.0f;
        tWalkTo.nTranslateType = TRANSLATE_TYPE_CLOSING;
        tWalkTo.dwAnimTag = dwAnimTag;
        sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_WALK);
        sBrainComponentSetStatus(idActor, "RunInFront");
        BrainConditionSetConditionAndTime(idActor, BRAIN_CONDITION_PERFORMING, 0);
        BrainComponentDoMovement(idActor, tWalkTo.placeGotoPoint, tWalkTo.wdSpeed, tWalkTo.nTranslateType, tWalkTo.dwAnimTag,
                                 0.1f, "Closing", ACDID_INVALID, FALSE, snoPower);
    }
}

// Original line 10150.
void BrainAPIReturnToPath(ActorID idActor, SNO)
{
    if (AIHasNodePath(idActor)) {
        ActorAI * ptAI = ActorTryToGetAI(idActor);
        ACDID idACD = ACDFindByActor(idActor);
        ActorCommonData & tACD = ACDGet(idACD);
        WorldPlace tPlace = tACD.GetPlace();
        AINodePathData * ptPathData = ptAI->ptNodePathData;
        NodePathNode * pDestNode = NodePathGetNode(ptPathData->idPath, ptPathData->idPathNodeDest);
        Vector3D vClosestPos;
        ptPathData->idPathNodeCurr = NodePathGetNextNodeAndPosition(ptPathData->idPath, tPlace.wp, pDestNode, vClosestPos);
        if (ptPathData->idPathNodeCurr != PATHNODEID_INVALID) {
            WorldPlace placeTarget;
            placeTarget = tPlace;
            placeTarget.wp = vClosestPos;
            BrainComponentData * ptComp = sGetBrainComponentData(idActor);
            BrainComponentWalkTo & tWalkTo = ptComp->tWalkTo;
            tWalkTo.placeGotoPoint = placeTarget;
            tWalkTo.wdSpeed = 0.0f;
            tWalkTo.nTranslateType = TRANSLATE_TYPE_CLOSING;
            tWalkTo.dwAnimTag = TAG_ACTOR_ANIMATION_NEUTRAL;
            sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_WALK);
            sBrainComponentSetStatus(idActor, "ReturnToPath");
            BrainConditionSetConditionAndTime(idActor, BRAIN_CONDITION_PERFORMING, 0);
            BrainComponentDoMovement(idActor, tWalkTo.placeGotoPoint, tWalkTo.wdSpeed, tWalkTo.nTranslateType, tWalkTo.dwAnimTag,
                                     0.1f, "ReturnToPath", ACDID_INVALID, FALSE, SNO_NONE);
        }
    }
}

void ACDSetYaw(ACDID idACD, const Angle aYaw);  // ActorCommonData.cpp:3027

// Original line 10211.
void BrainAPIReturnToGuardObject(ActorID idActor, SNO snoPower)
{
    const float RETURN_RADIUS = 1.0f;

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);

    ACDID idACDGuardObject = (ACDID)tACD.AttributesGetInt(ATTRIBUTE_GUARD_OBJECT_ACDID);
    ActorCommonData * ptACDGuardObject = ACDTryToGet(idACDGuardObject);
    if (!ptACDGuardObject || !ptACDGuardObject->IsInWorld())
        return;

    WorldPlace placeSelf = tACD.GetPlace();
    WorldPlace placeGuardObject = ptACDGuardObject->GetPlace();
    if (!WorldPlaceInSameWorld(placeSelf, placeGuardObject))
        return;

    float flDistanceSq = WorldPlaceDistanceSquared(placeSelf, placeGuardObject);
    if (flDistanceSq < RETURN_RADIUS * RETURN_RADIUS) {
        ACDSetYaw(idACD, ACDGetYaw(idACDGuardObject));
        BrainAPIDoIdle(idActor);
        return;
    }

    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentWalkTo & tWalkTo = ptComp->tWalkTo;
    tWalkTo.placeGotoPoint = placeGuardObject;
    tWalkTo.wdSpeed = 0.0f;
    tWalkTo.nTranslateType = TRANSLATE_TYPE_CLOSING;
    tWalkTo.dwAnimTag = TAG_ACTOR_ANIMATION_NEUTRAL;
    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_WALK);

    sBrainComponentSetStatus(idActor, "ReturnToGuardObject");
    BrainConditionSetConditionAndTime(idActor, BRAIN_CONDITION_PERFORMING, 0);
    BrainComponentDoMovement(idActor, tWalkTo.placeGotoPoint, tWalkTo.wdSpeed, tWalkTo.nTranslateType, tWalkTo.dwAnimTag,
                             0.1f, "ReturnToGuardObject", ACDID_INVALID, FALSE, SNO_NONE);
}

// Original line 10276.
void BrainAPIFollowPath(ActorID idActor, SNO)
{
    WorldPlace placeTarget;
    BOOL bUsingPath = AIWalkNodePath(idActor, placeTarget);
    if (bUsingPath) {
        BrainComponentData * ptComp = sGetBrainComponentData(idActor);
        BrainComponentWalkTo & tWalkTo = ptComp->tWalkTo;
        tWalkTo.placeGotoPoint = placeTarget;
        tWalkTo.wdSpeed = 0.0f;
        tWalkTo.nTranslateType = TRANSLATE_TYPE_CLOSING;
        tWalkTo.dwAnimTag = TAG_ACTOR_ANIMATION_NEUTRAL;
        sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_WALK);
        sBrainComponentSetStatus(idActor, "FollowPath");
        BrainConditionSetConditionAndTime(idActor, BRAIN_CONDITION_PERFORMING, 0);
        BrainComponentDoMovement(idActor, tWalkTo.placeGotoPoint, tWalkTo.wdSpeed, tWalkTo.nTranslateType, tWalkTo.dwAnimTag,
                                 0.1f, "FollowPath", ACDID_INVALID, FALSE, -1);
    }
}

// Original line 10485.
static void sCopyTargetList(const TTargetList & tListFrom, ActorID idActorTo, ACDID idACDTo)
{
    ActorCommonData & tACDTo = ACDGet(idACDTo);
    POS pos = tListFrom.GetHeadPosition();
    while (pos) {
        const TargetInfo & tTargetInfo = tListFrom.GetNext(pos);
        const Actor * ptActor = ActorTryToGet(tTargetInfo.idTarget);
        if (ptActor)
            sActorAddTarget(ptActor->idACD, idActorTo, tACDTo, FALSE);
    }
}

// Original line 10506.

void BrainAPIInheritTargetList(const Actor & tActorFrom, Actor & tActorTo)
{
    if (tActorFrom.ptAI && tActorTo.ptAI && tActorFrom.ptAI->ptBrainComponentData && tActorTo.ptAI->ptBrainComponentData) {
        const BrainComponentTarget & tTargetFrom = tActorFrom.ptAI->ptBrainComponentData->tTarget;
        sCopyTargetList(tTargetFrom.listTargets, tActorTo.id, tActorTo.idACD);
        sCopyTargetList(tTargetFrom.listFriendlies, tActorTo.id, tActorTo.idACD);
    }
}

// Original line 10522.
void BrainAPITryToFlee(ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return;

    BOOL bFearImmune = ptACD->AttributesGetInt(ATTRIBUTE_FEAR_IMMUNE);
    if (!bFearImmune) {
        ActorID idActor = (ActorID)ACDGetOwner(idACD);
        ActorAI * ptActorAI = ActorTryToGetAI(idActor);
        if (ptActorAI) {
            uint32 dwNow = GameGetServerTime();
            if (dwNow >= ptActorAI->dwNextFleeTime) {
                float flFleeChance;
                float flFleeCooldownMin;
                float flFleeCooldownDelta;
                MonsterGetFleeInfo(ptACD->tSNOName, flFleeChance, flFleeCooldownMin, flFleeCooldownDelta);
                if (flFleeChance > EPSILON &&
                    RandFloat_Internal(SGameGlobalsGet().rng) <= flFleeChance) {
                    AIStateSetState((ActorID)ACDGetOwner(idACD), AISTATE_TYPE_FLEEING, FALSE);
                    if (flFleeCooldownMin + flFleeCooldownDelta > EPSILON) {
                        float flCooldown = RandRangeFloat_Internal(SGameGlobalsGet().rng, flFleeCooldownMin, flFleeCooldownDelta);
                        int32 nCooldownTicks = Math::FloatToInt(flCooldown * TICKS_PER_SECOND);
                        ptActorAI->dwNextFleeTime = dwNow + Math::Max(nCooldownTicks, 0);
                    }
                }
            }
        }
    }
}

// Original line 10466.
BOOL BrainAPIActorHasQueuedAttack(ActorID idActor)
{
    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (!ptAI)
        return FALSE;
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    if (ptComp->nActiveComponent != ACTIVE_COMPONENT_ATTACK)
        return FALSE;
    const BrainComponentAttack & tAttack = ptComp->tAttack;
    return tAttack.bAttackQueued;
}

BOOL ACDIsFollower(const ActorCommonData & tACD);  // ActorCommonData.cpp:8206; belongs in ActorCommonData.h

// Original lines 10564-10573.
const float THREATENED_FEET_PER_ATTACKER = 10.0f;
const float THREATENED_FEET_PER_ATTACKER_FOR_PETS_FOLLOWER_NPCS = 20.0f;
const float THREATENED_FEET_BONUS_FOR_CURRENT_TARGET = 1.0f;
const float THREATENED_FEET_PENALTY_FOR_PETS = 60.0f;
const float THREATENED_FEET_PENALTY_FOR_FOLLOWERS_NPCS = 60.0f;
const float THREATENED_FEET_PENALTY_CHARMED_TARGET = 10.0f;
const int32 THREATENED_NUM_ATTACKERS_BEFORE_PET_PENALTY = 1;
const int32 THREATENED_NUM_ATTACKERS_BEFORE_FOLLOWER_NPC_PENALTY = 0;
const float THREATENED_PENALTY_FOR_INVULNERABLE_NPCS = 100000.0f;
const float THREATENED_PENALTY_FOR_ATTACKER_CAP = 100000.0f;

typedef BOOL (*ClosestPlusThreatenedTargetCustomScoreFunctionPtr)(float &, ActorCommonData &, ActorID, ActorID);

// Original line 10575.
ACDID sPickClosestPlusThreatened(ActorCommonData & tACD, const TTargetList & tList, ActorID idActorCurrentTarget,
                                 BOOL bActiveEnemiesOnly,
                                 ClosestPlusThreatenedTargetCustomScoreFunctionPtr pfnCustomScore)
{
    if (tList.Count() <= 0)
        return ACDID_INVALID;

    WorldPlace placeCurr = tACD.GetPlace();
    BOOL bAttackerCharmed = tACD.IsCharmed();
    BOOL bPet = tACD.IsPet();
    BOOL bFollowerOrNPC = ACDIsHireling(tACD) || ACDIsFollower(tACD) ||
                          (tACD.IsNPC() && TeamsAreFriendly(tACD.GetTeam(), TEAM_PLAYER_0));
    ACDID idACDWinner = ACDID_INVALID;
    float flWinningDist = FLT_MAX;
    uint32 uNow = GameGetServerTime();

    POS pos = tList.GetHeadPosition();
    while (pos) {
        const TargetInfo & tActorTarget = tList.GetNext(pos);
        if (uNow < tActorTarget.dwActiveTime)
            continue;

        ActorID idActorTarget = tActorTarget.idTarget;
        Actor * ptActorTarget = ActorTryToGet(idActorTarget);
        if (ptActorTarget) {
            ACDID idACDTarget = ACDFindByActor(idActorTarget);
            ActorCommonData & tACDTarget = ACDGet(idACDTarget);
            if (!tACDTarget.IsInWorld())
                continue;
            if (bActiveEnemiesOnly && BrainAPIActorAIIsInactive(tACDTarget))
                continue;

            WorldPlace placeTarget = tACDTarget.GetPlace();
            float flDist = Vector3D::Distance2D(placeCurr.wp, placeTarget.wp);
            int32 nAttackerCount = ptActorTarget->nAttackerCount;
            if (idActorTarget == idActorCurrentTarget) {
                flDist -= THREATENED_FEET_BONUS_FOR_CURRENT_TARGET;
                nAttackerCount--;
            }

            BOOL bPetTarget = tACDTarget.IsPet();
            BOOL bFollowerOrNPCTarget = ACDIsHireling(tACDTarget) || ACDIsFollower(tACDTarget) ||
                                        (tACDTarget.IsNPC() && TeamsAreFriendly(tACDTarget.GetTeam(), TEAM_PLAYER_0));
            if (bPetTarget || bFollowerOrNPCTarget) {
                if (tACDTarget.AttributesGetInt(ATTRIBUTE_INVULNERABLE) || tACDTarget.AttributesGetInt(ATTRIBUTE_NO_DAMAGE))
                    flDist += THREATENED_PENALTY_FOR_INVULNERABLE_NPCS;
                flDist += nAttackerCount * THREATENED_FEET_PER_ATTACKER_FOR_PETS_FOLLOWER_NPCS;
                if (bFollowerOrNPCTarget) {
                    if (nAttackerCount >= THREATENED_NUM_ATTACKERS_BEFORE_FOLLOWER_NPC_PENALTY)
                        flDist += THREATENED_FEET_PENALTY_FOR_FOLLOWERS_NPCS;
                } else if (nAttackerCount >= THREATENED_NUM_ATTACKERS_BEFORE_PET_PENALTY) {
                    flDist += THREATENED_FEET_PENALTY_FOR_PETS;
                }
            } else {
                flDist += nAttackerCount * THREATENED_FEET_PER_ATTACKER;
            }

            if ((bAttackerCharmed || bPet || bFollowerOrNPC) && tACDTarget.IsCharmed())
                flDist += THREATENED_FEET_PENALTY_CHARMED_TARGET;

            ScopedGlobalDefinitionLockR pdefGlobals;
            if (ptActorTarget->nAttackerSlotsUsed >= pdefGlobals->nMaximumAttackersPerPlayer)
                flDist += THREATENED_PENALTY_FOR_ATTACKER_CAP;

            if (pfnCustomScore && !pfnCustomScore(flDist, tACD, idActorCurrentTarget, idActorTarget))
                continue;

            if (flDist < flWinningDist) {
                flWinningDist = flDist;
                idACDWinner = idACDTarget;
            }
        }
    }
    return idACDWinner;
}

// Original line 10708.

#line 10708 "X/Server/AI/BrainComponents.cpp"
ACDID sPickClosestPlusAlly(
    ActorCommonData & tACD,
    const BrainComponentTarget & tTarget,
    BOOL bActiveEnemiesOnly)
{
    WorldPlace placeCurr = tACD.GetPlace();
    ACDID idACDWinner = ACDID_INVALID;
    float flWinningDist = FLT_MAX;
    ACDID idACDWinnerFiltered = ACDID_INVALID;
    float flWinningDistFiltered = FLT_MAX;
    uint32 uNow = GameGetServerTime();

    POS pos = tTarget.listTargets.GetHeadPosition();
    while (pos)
    {
        const TargetInfo & tActorTarget = tTarget.listTargets.GetNext(pos);
        if (uNow < tActorTarget.dwActiveTime)
        {
            continue;
        }

        ActorID idActorTarget = tActorTarget.idTarget;
        Actor * ptActorTarget = ActorTryToGet(idActorTarget);
        if (ptActorTarget)
        {
            ACDID idACDTarget = ACDFindByActor(idActorTarget);
            ActorCommonData & tACDTarget = ACDGet(idACDTarget);

            if (bActiveEnemiesOnly && BrainAPIActorAIIsInactive(tACDTarget))
            {
                continue;
            }

            WorldPlace placeTarget = tACDTarget.GetPlace();
            float flDist = Vector3D::Distance2D(placeCurr.wp, placeTarget.wp);

            if (flDist < flWinningDist)
            {
                flWinningDist = flDist;
                idACDWinner = idACDTarget;
            }

            if (sBrainAPITargetFilterExcludeInCombatWithAlly(tACD.id, idACDTarget))
            {
                if (flDist < flWinningDistFiltered)
                {
                    flWinningDistFiltered = flDist;
                    idACDWinnerFiltered = idACDTarget;
                }
            }
        }
    }

    if (idACDWinnerFiltered != ACDID_INVALID)
    {
        idACDWinner = idACDWinnerFiltered;
    }

    return idACDWinner;
}
#line 3416 "X/Server/AI/BrainComponents.cpp"


// Original line 10772.
inline ACDID sPickHighestThreat(ActorID idActor, BOOL bActiveEnemiesOnly)
{
    return BrainAPIActorGetHighestThreatTarget(idActor, bActiveEnemiesOnly);
}

// Original line 10781.

#line 10781 "X/Server/AI/BrainComponents.cpp"
ACDID sPickSwitchOnAttack(
    ActorCommonData & tACD,
    const BrainComponentTarget & tTarget,
    BOOL bActiveEnemiesOnly)
{
    WorldPlace placeCurr = tACD.GetPlace();
    ACDID idACDWinner = ACDID_INVALID;
    float flWinningDist = FLT_MAX;
    ACDID idACDWinnerFiltered = ACDID_INVALID;
    float flWinningDistFiltered = FLT_MAX;
    uint32 uNow = GameGetServerTime();

    POS pos = tTarget.listTargets.GetHeadPosition();
    while (pos)
    {
        const TargetInfo & tActorTarget = tTarget.listTargets.GetNext(pos);
        if (uNow < tActorTarget.dwActiveTime)
        {
            continue;
        }

        ActorID idActorTarget = tActorTarget.idTarget;
        Actor * ptActorTarget = ActorTryToGet(idActorTarget);
        if (ptActorTarget)
        {
            ACDID idACDTarget = ACDFindByActor(idActorTarget);
            ActorCommonData & tACDTarget = ACDGet(idACDTarget);
            if (bActiveEnemiesOnly && BrainAPIActorAIIsInactive(tACDTarget))
            {
                continue;
            }

            WorldPlace placeTarget = tACDTarget.GetPlace();
            float flDist = Vector3D::Distance2D(placeCurr.wp, placeTarget.wp);

            if (flDist < flWinningDist)
            {
                flWinningDist = flDist;
                idACDWinner = idACDTarget;
            }

            if (sBrainAPITargetFilterExcludeLastAttacked(tACD.id, idACDTarget))
            {
                if (flDist < flWinningDistFiltered)
                {
                    flWinningDistFiltered = flDist;
                    idACDWinnerFiltered = idACDTarget;
                }
            }
        }
    }

    if (idACDWinnerFiltered != ACDID_INVALID)
    {
        idACDWinner = idACDWinnerFiltered;
    }

    return idACDWinner;
}
#line 3470 "X/Server/AI/BrainComponents.cpp"

// Original line 10891.
ACDID sPickLowestHealthPlayer(const BrainComponentTarget & tTarget, BOOL bActiveEnemiesOnly)
{
    ACDID idACDWinner = ACDID_INVALID;
    float flWinningHealth = FLT_MAX;
    if (tTarget.listTargets.Count() <= 0)
        return idACDWinner;

    uint32 uNow = GameGetServerTime();

    POS pos = tTarget.listTargets.GetHeadPosition();
    while (pos) {
        const TargetInfo & tActorTarget = tTarget.listTargets.GetNext(pos);
        if (uNow < tActorTarget.dwActiveTime)
            continue;

        ActorID idActorTarget = tActorTarget.idTarget;
        Actor * ptActorTarget = ActorTryToGet(idActorTarget);
        if (ptActorTarget) {
            ACDID idACDTarget = ACDFindByActor(idActorTarget);
            if (ACDIsPlayer(idACDTarget)) {
                ActorCommonData & tACDTarget = ACDGet(idACDTarget);
                if (bActiveEnemiesOnly && BrainAPIActorAIIsInactive(tACDTarget))
                    continue;

                float flCurrHealth = tACDTarget.GetHitPointsCurrent();
                if (flCurrHealth < flWinningHealth) {
                    flWinningHealth = flCurrHealth;
                    idACDWinner = idACDTarget;
                }
            }
        }
    }
    return idACDWinner;
}

// Original line 10940.
ACDID sPickRandomTarget(const BrainComponentTarget & tTarget, BOOL bActiveEnemiesOnly)
{
    POS pos;
    int32 nNumValid = 0;
    uint32 uNow = GameGetServerTime();

    pos = tTarget.listTargets.GetHeadPosition();
    while (pos) {
        const TargetInfo & tActorTarget = tTarget.listTargets.GetNext(pos);
        if (uNow < tActorTarget.dwActiveTime)
            continue;

        ActorID idActorTarget = tActorTarget.idTarget;
        Actor * ptActorTarget = ActorTryToGet(idActorTarget);
        if (ptActorTarget) {
            ACDID idACDTarget = ACDFindByActor(idActorTarget);
            ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
            if (bActiveEnemiesOnly && ptACDTarget && BrainAPIActorAIIsInactive(*ptACDTarget))
                continue;
            nNumValid++;
        }
    }

    int32 nCount = 0;
    ACDID idACDWinner = ACDID_INVALID;
    if (nNumValid > 0) {
        int32 nRandomPick = RandRange_Internal(SGameGlobalsGet().rng, 0, nNumValid - 1);

        pos = tTarget.listTargets.GetHeadPosition();
        while (pos) {
            const TargetInfo & tActorTarget = tTarget.listTargets.GetNext(pos);
            if (uNow < tActorTarget.dwActiveTime)
                continue;

            ActorID idActorTarget = tActorTarget.idTarget;
            Actor * ptActorTarget = ActorTryToGet(idActorTarget);
            if (ptActorTarget) {
                ACDID idACDTarget = ACDFindByActor(idActorTarget);
                ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
                if (bActiveEnemiesOnly && ptACDTarget && BrainAPIActorAIIsInactive(*ptACDTarget))
                    continue;

                if (nRandomPick <= nCount) {
                    idACDWinner = ACDFindByActor(idActorTarget);
                    break;
                }
                nCount++;
            }
        }
    }
    return idACDWinner;
}

BOOL HeroIsRanged(ACDID idACDPlayer);  // Hero.cpp:92; belongs in X/Common/Hero.h (not in src/ yet)

// Original line 10844.
ACDID sPickPlayersByRanged(ActorCommonData & tACD, const BrainComponentTarget & tTarget, BOOL bPickRanged,
                           BOOL bActiveEnemiesOnly)
{
    POS pos;
    BrainComponentGlobals * ptBrainComponentGlobals = SGameManager::GameGet().ptBrainComponentGlobals;
    TTargetList listPlayers(&ptBrainComponentGlobals->poolTargetInfo);
    uint32 uNow = GameGetServerTime();

    pos = tTarget.listTargets.GetHeadPosition();
    while (pos) {
        const TargetInfo & tActorTarget = tTarget.listTargets.GetNext(pos);
        if (uNow < tActorTarget.dwActiveTime)
            continue;

        ActorID idActorTarget = tActorTarget.idTarget;
        Actor * ptActorTarget = ActorTryToGet(idActorTarget);
        if (ptActorTarget) {
            ACDID idACDTarget = ACDFindByActor(idActorTarget);
            ActorCommonData & tACDTarget = ACDGet(idACDTarget);
            if (bActiveEnemiesOnly && BrainAPIActorAIIsInactive(tACDTarget))
                continue;

            if (ACDIsPlayer(tACDTarget.id)) {
                BOOL bRanged = HeroIsRanged(tACDTarget.id);
                if ((bPickRanged && bRanged) || (!bRanged && !bPickRanged))
                    listPlayers.AddTail_Tracked(tActorTarget);
            }
        }
    }

    ACDID idACDClosest = sPickClosestPlusThreatened(tACD, listPlayers, tTarget.idTarget, bActiveEnemiesOnly, NULL);
    return idACDClosest;
}

// Original line 11019.
ACDID sPickMeleePetTarget(ActorCommonData & tACD, const BrainComponentTarget & tTarget, BOOL bActiveEnemiesOnly)
{
    POS pos;
    if (tTarget.listTargets.Count() <= 0)
        return ACDID_INVALID;

    Player * ptPlayerOwner = tACD.GetPetOwnerPlayer();
    if (!ptPlayerOwner)
        return sPickClosestPlusThreatened(tACD, tTarget.listTargets, tTarget.idTarget, bActiveEnemiesOnly, NULL);

    ActorCommonData & tACDOwner = ACDGet(ptPlayerOwner->idPlayerACD);
    WorldPlace placeOwner = tACDOwner.GetPlace();
    WorldPlace placeCurr = tACD.GetPlace();
    float flWinningDist = FLT_MAX;
    ACDID idACDWinner = ACDID_INVALID;
    uint32 uNow = GameGetServerTime();

    pos = tTarget.listTargets.GetHeadPosition();
    while (pos) {
        const TargetInfo & tActorTarget = tTarget.listTargets.GetNext(pos);
        if (uNow < tActorTarget.dwActiveTime)
            continue;

        ActorID idActorTarget = tActorTarget.idTarget;
        Actor * ptActorTarget = ActorTryToGet(idActorTarget);
        if (ptActorTarget) {
            ACDID idACDTarget = ACDFindByActor(idActorTarget);
            ActorCommonData & tACDTarget = ACDGet(idACDTarget);
            if (bActiveEnemiesOnly && BrainAPIActorAIIsInactive(tACDTarget))
                continue;

            WorldPlace placeTarget = tACDTarget.GetPlace();
            float flDistFromPet = Vector3D::Distance2D(placeCurr.wp, placeTarget.wp);
            float flDistFromOwner = Vector3D::Distance2D(placeOwner.wp, placeTarget.wp);
            float flDist = flDistFromOwner * 0.75f + flDistFromPet * 0.25f + (float)ptActorTarget->nAttackerCount * 10.0f;
            if (idActorTarget == tTarget.idTarget)
                flDist -= 10.0f;
            if (ACDIsPlayer(idACDTarget))
                flDist -= 45.0f;
            if (flDist < flWinningDist) {
                flWinningDist = flDist;
                idACDWinner = idACDTarget;
            }
        }
    }
    return idACDWinner;
}

// Original line 11102; externally linked callback.
BOOL sCrusaderHeavensFuryCustomScoring(float &, ActorCommonData & tACD, ActorID, ActorID idActorScoreTarget)
{
    if (idActorScoreTarget == ACTORID_INVALID)
        return FALSE;
    ACDID idACDTarget = ACDTryFindByActor(idActorScoreTarget);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget)
        return FALSE;
    Player * ptPlayer = tACD.GetPetOwnerPlayer();
    int32 nPlayerIndex = ptPlayer ? ptPlayer->nPlayerIndex : -1;
    ACDID idACDHeavensFuryAttacker =
        (ACDID)ptACDTarget->AttributesGetInt(MakeAttribKey(ATTRIBUTE_HEAVENS_FURY_ATTACKER, nPlayerIndex));
    if (idACDHeavensFuryAttacker != tACD.id && idACDHeavensFuryAttacker != ACDID_INVALID &&
        ACDTryToGet(idACDHeavensFuryAttacker))
        return FALSE;
    return TRUE;
}

// Original line 11130.
ACDID BrainAPIChoosePrimaryTarget(ActorCommonData & tACD, BOOL bActiveEnemiesOnly)
{
    ACDID idACDTarget = ACDID_INVALID;
    ActorID idActor = (ActorID)ACDGetOwner(tACD.id);
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    if (ptComp) {
        BrainAPIUpdateTargets(idActor, FALSE);

        AITargetStyle eTargetStyle = MonsterGetTargetStyle(tACD.tSNOName, tACD.eMonsterRarity);
        const BrainComponentTarget & tTarget = ptComp->tTarget;
        switch (eTargetStyle) {
        case AITARGET_STYLE_CLOSEST_PLUS_THREATENED:
            idACDTarget = sPickClosestPlusThreatened(tACD, tTarget.listTargets, tTarget.idTarget, bActiveEnemiesOnly, NULL);
            break;

        case AITARGET_STYLE_CLOSEST: {
            ActorID idActorTarget = sActorChooseClosestTarget(idActor, SNO_NONE, &tTarget.listTargets, FALSE, NULL, FALSE, TRUE,
                                                              bActiveEnemiesOnly);
            idACDTarget = ACDTryFindByActor(idActorTarget);
            break;
        }

        case AITARGET_STYLE_CLOSEST_PLUS_ALLY:
            idACDTarget = sPickClosestPlusAlly(tACD, tTarget, bActiveEnemiesOnly);
            break;

        case AITARGET_STYLE_CLOSEST_PLUS_NOT_KNOCKBACKABLE: {
            ActorID idActorTarget = sActorChooseClosestTarget(idActor, SNO_NONE, &tTarget.listTargets, FALSE, NULL, TRUE, TRUE,
                                                              bActiveEnemiesOnly);
            idACDTarget = ACDTryFindByActor(idActorTarget);
            break;
        }

        case AITARGET_STYLE_HIGHEST_THREAT:
            idACDTarget = sPickHighestThreat(idActor, bActiveEnemiesOnly);
            break;

        case AITARGET_STYLE_SWITCH_ON_ATTACK:
            idACDTarget = sPickSwitchOnAttack(tACD, tTarget, bActiveEnemiesOnly);
            break;

        case AITARGET_STYLE_RANDOM:
            idACDTarget = sPickRandomTarget(tTarget, bActiveEnemiesOnly);
            break;

        case AITARGET_STYLE_ASSASSIN:
            idACDTarget = sPickLowestHealthPlayer(tTarget, bActiveEnemiesOnly);
            if (idACDTarget == ACDID_INVALID)
                idACDTarget = sPickClosestPlusThreatened(tACD, tTarget.listTargets, tTarget.idTarget, bActiveEnemiesOnly, NULL);
            break;

        case AITARGET_STYLE_RANGED_FOCUSED:
            idACDTarget = sPickPlayersByRanged(tACD, tTarget, TRUE, bActiveEnemiesOnly);
            if (idACDTarget == ACDID_INVALID)
                idACDTarget = sPickClosestPlusThreatened(tACD, tTarget.listTargets, tTarget.idTarget, bActiveEnemiesOnly, NULL);
            break;

        case AITARGET_STYLE_MELEE_FOCUSED:
            idACDTarget = sPickPlayersByRanged(tACD, tTarget, FALSE, bActiveEnemiesOnly);
            if (idACDTarget == ACDID_INVALID)
                idACDTarget = sPickClosestPlusThreatened(tACD, tTarget.listTargets, tTarget.idTarget, bActiveEnemiesOnly, NULL);
            break;

        case AITARGET_STYLE_MELEE_PET:
            idACDTarget = sPickMeleePetTarget(tACD, tTarget, bActiveEnemiesOnly);
            break;

        case AITARGET_STYLE_CRUSADER_HEAVENS_FURY:
            idACDTarget = sPickClosestPlusThreatened(tACD, tTarget.listTargets, tTarget.idTarget, bActiveEnemiesOnly,
                                                     sCrusaderHeavensFuryCustomScoring);
            break;
        }
    }
    return idACDTarget;
}

void InitializeDamage(DamageAttribute & tDamageAttribute, FastAttribKey tKey, float flValue, int32 nValue, int32 nDuration);  // Powers.cpp:4433

// Original line 11214.
void BrainAPIDoScriptedDeath(ActorCommonData & tACD, DeathType eDeathType, BOOL bCrit, BOOL bDropLoot, ActorID idKiller)
{
    ACDID idACD = tACD.id;
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    int32 nPrevDeathType = tACD.AttributesGetInt(ATTRIBUTE_DEATH_TYPE_OVERRIDE);
    tACD.AttributesSetInt(ATTRIBUTE_DEATH_TYPE_OVERRIDE, eDeathType);

    PowerAPIDamage tProxyDamage;
    PowerHelper_DamageInit(tProxyDamage, POWERID_INVALID, idACD);
    DamageAttribute attribDamage;
    InitializeDamage(attribDamage, ATTRIBUTE_HITPOINTS_CUR, -FLT_MAX, 0, 0);
    tProxyDamage.listDamage.AddTail_Tracked(attribDamage);
    tProxyDamage.eDeliveryMechanism = 1;  // PAYLOAD_RAY in the reference GameBalanceDefinition.h

    float flDamage[NUM_DAMAGE_TYPES] = { 0.0f };

    {
        DamageType eDamageType;
        switch (eDeathType) {
            case DEATH_FIRE: eDamageType = DAMAGE_TYPE_FIRE; break;
            case DEATH_01: eDamageType = DAMAGE_TYPE_PHYSICAL; break;
            case DEATH_COLD: eDamageType = DAMAGE_TYPE_COLD; break;
            case DEATH_LIGHTNING: eDamageType = DAMAGE_TYPE_LIGHTNING; break;
            case DEATH_ARCANE: eDamageType = DAMAGE_TYPE_ARCANE; break;
            case DEATH_HOLY: eDamageType = DAMAGE_TYPE_HOLY; break;
            case DEATH_POISON: eDamageType = DAMAGE_TYPE_POISON; break;
            default: eDamageType = DAMAGE_TYPE_NONE; break;
        }
        if (eDamageType != DAMAGE_TYPE_NONE) {
            tProxyDamage.bCriticalHit = bCrit;
            flDamage[eDamageType] = 1.0f;
        } else {
            tProxyDamage.bCriticalHit = FALSE;
        }
    }

    const BOOL fNoExp = !bDropLoot;
    const BOOL fNoHealth = !bDropLoot;
    if (tProxyDamage.bCriticalHit)
        BrainAPIDoDeath(idActor, idKiller, fNoExp, !bDropLoot, fNoHealth, &tProxyDamage, flDamage);
    else
        BrainAPIDoDeath(idActor, idKiller, fNoExp, !bDropLoot, fNoHealth, &tProxyDamage, NULL);
    tACD.AttributesSetInt(ATTRIBUTE_DEATH_TYPE_OVERRIDE, nPrevDeathType);
}

#line 5456 "X/Server/AI/BrainComponents.cpp"
void sCleanupAttributesOnDeath(
    ActorID idDeadActor)
{
    const Actor * ptActor = ActorGet(idDeadActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);

    tACD.AttributesSetInt(ATTRIBUTE_FROZEN, 0);


    for (int32 iTimerType = 0; iTimerType < NUM_ATTRIBUTE_TIMER_TYPES; ++iTimerType)
    {
        int32 iTimerDuration = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_ATTRIBUTE_TIMER, iTimerType));

        if (iTimerDuration > 0)
        {
            ActorTimerEffectsExpireGroup(idDeadActor, iTimerType);
            tACD.AttributesSetInt(MakeAttribKey(ATTRIBUTE_ATTRIBUTE_TIMER, iTimerType), 0);
        }
    }

    ActorPowersCancelForDeath(idDeadActor);


    ResourceType eType = tACD.GetPrimaryResourceType();
    if (eType != RESOURCE_TYPE_INVALID)
    {
        tACD.ResourceAttributeSetFloat(ATTRIBUTE_RESOURCE_CUR, 0.0f, eType);
    }

    eType = tACD.GetSecondaryResourceType();
    if (eType != RESOURCE_TYPE_INVALID)
    {
        tACD.ResourceAttributeSetFloat(ATTRIBUTE_RESOURCE_CUR, 0.0f, eType);
    }
}

#line 6098 "X/Server/AI/BrainComponents.cpp"
static BOOL sIsDisposableCorpse(
    const ACDID idACD)
{

    if (!ActorHasBeenKilled((ActorID)ACDGetOwner(idACD)))
        return FALSE;





    const ActorCommonData & tACD = ACDGet(idACD);
    if (tACD.IsAlive())
        return FALSE;




    if (!tACD.IsMonster())
        return FALSE;




    const ActorID idActor = (ActorID)ACDGetOwner(idACD);
    const ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (!ptAI)
        return FALSE;




    if (tACD.eMonsterRarity == MR_RARE || tACD.eMonsterRarity == MR_UNIQUE || tACD.eMonsterRarity == MR_HIRELING || tACD.eMonsterRarity == MR_BOSS)
        return FALSE;







    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentDeath & tCompDeath = ptComp->tDeath;
    if (tCompDeath.dwTimeToRemoveBody != (uint32)-1)
    {
        uint32 dwNow = GameGetServerTime();

        if (dwNow - tCompDeath.dwTimeToRemoveBody < 300)
            return FALSE;

    }

    return TRUE;
}






ACDIterateResult sFindDisposableCorpse(
    const WorldDistance,
    const ACDID,
    const ACDID idACDFound,
    void * ptCallbackData)
{

    if (sIsDisposableCorpse(idACDFound))
    {
        ACDIDListPooled & tListACDCorpses = *(ACDIDListPooled *)ptCallbackData;
        const ActorID idActorFound = (ActorID)ACDGetOwner(idACDFound);
        ActorAI & tAI = ActorGetAI(idActorFound);


        if (tListACDCorpses.IsEmpty())
        {
            tListACDCorpses.AddTail_Tracked(idACDFound);
        }
        else
        {
            BOOL bAdded = FALSE;
            POS pos = tListACDCorpses.GetHeadPosition();
            while (pos)
            {

                POS posCurrent = pos;

                ACDID idACDCorpseOther = (ACDID)tListACDCorpses.GetNext(pos);


                ActorID idActorCorpseOther = (ActorID)ACDGetOwner(idACDCorpseOther);
                const ActorAI & tAICorpseOther = ActorGetAI(idActorCorpseOther);
                if (tAI.dwTimeOfDeath <= tAICorpseOther.dwTimeOfDeath)
                {


                    tListACDCorpses.InsertBefore_Tracked(posCurrent, idACDFound);
                    bAdded = TRUE;
                    break;
                }


            }





            if (!bAdded)
            {
                tListACDCorpses.AddTail_Tracked(idACDFound);
            }
        }
    }

    return ACD_ITERATE_CONTINUE;
}

#line 7814 "X/Server/AI/BrainComponents.cpp"
struct SkillWeightInfo
{
    SNO snoPower;
    uint32 dwWeight;
    Target tTarget;
};
static_assert(sizeof(SkillWeightInfo) == 0x24 && __builtin_offsetof(SkillWeightInfo, snoPower) == 0 && __builtin_offsetof(SkillWeightInfo, dwWeight) == 4 && __builtin_offsetof(SkillWeightInfo, tTarget) == 8, "SkillWeightInfo layout");
static const int32 MAX_AISTATE_CHOOSE_SKILL_TRIES = 3;

void BrainAPIChooseSkill(
    ActorID idActor,
    BrainAttackSettings & tAttackSettings)
{
    ACDID idACD = ACDFindByActor(idActor);

    tAttackSettings.tTarget = cgTargetNone;
    tAttackSettings.snoPower = SNO_NONE;
    tAttackSettings.eAIStateFrom = AIStateGetCurrState(idActor);
    tAttackSettings.nSuccessfulCasts = 0;

    if (idACD == ACDID_INVALID)
        return;




    ActorAI & tAI = ActorGetAI(idActor);
    if (tAI.ptAIStateInstData)
    {
        int32 nChooseSkillTries = 0;
        while (nChooseSkillTries < MAX_AISTATE_CHOOSE_SKILL_TRIES)
        {
            BOOL bStateSwitchOccurred = AIStateChooseSkill(idActor, tAttackSettings);
            if (bStateSwitchOccurred && tAttackSettings.snoPower == SNO_NONE)
                ++nChooseSkillTries;
            else
                break;
        }

        return;
    }





    SkillWeightInfo tSkillWeight[MAX_MONSTER_SKILLS];

    const ActorCommonData & tACD = ACDGet(idACD);


    SNO snoMonster = tACD.GetMonsterSNO();
    if (snoMonster == SNO_NONE)
        return;





    const MonsterDefinition * ptDefMonster = (const MonsterDefinition *)gptGroupDefMonster->LockR(snoMonster, FALSE);
    uint32 dwNow = GameGetServerTime();
    uint32 dwTotalWeight = 0;

    for (int32 nSkill = 0; nSkill < MAX_MONSTER_SKILLS; ++nSkill)
    {
        SkillWeightInfo & tWeightInfo = tSkillWeight[nSkill];
        tWeightInfo.snoPower = SNO_NONE;

        SNO snoPower = ptDefMonster->tSkills[nSkill].snoPower;
        if (snoPower == SNO_NONE || ptDefMonster->tMonsterSkills[nSkill].dwWeight == 0)
            continue;




        if (tAI.ptBrainComponentData && !tAI.ptBrainComponentData->tSkillSettings[nSkill].fSkillEnable)
            continue;




        if (tAI.ptBrainComponentData->tSkillSettings[nSkill].dwDelayTime != 0 && dwNow <= tAI.ptBrainComponentData->tSkillSettings[nSkill].dwDelayTime)
            continue;





        if (!PowerCanUseNow(tACD, snoPower, NULL, NULL))
            continue;



        if (PowerWantsActorTarget(snoPower))
        {

            ActorID idTarget = BrainAPIActorChooseTarget(idActor, snoPower, FALSE, NULL);
            if (idTarget == ACTORID_INVALID)
                continue;




            if (ptDefMonster->tMonsterSkills[nSkill].flUseRangeMin != 0.0f ||
                ptDefMonster->tMonsterSkills[nSkill].flUseRangeMax != 0.0f)
            {
                WorldDistance wdDist = BrainAPIGetDistanceToTarget(idActor, idTarget);


                if (wdDist < ptDefMonster->tMonsterSkills[nSkill].flUseRangeMin ||
                    wdDist >= ptDefMonster->tMonsterSkills[nSkill].flUseRangeMax)
                    continue;

            }



            tWeightInfo.tTarget.idACDTarget = ACDFindByActor(idTarget);
            tWeightInfo.tTarget.eType = TT_ACTOR;
        }
        else
        {
            tWeightInfo.tTarget = cgTargetNone;
        }


        tWeightInfo.dwWeight = ptDefMonster->tMonsterSkills[nSkill].dwWeight;
        dwTotalWeight += tWeightInfo.dwWeight;
        tWeightInfo.snoPower = snoPower;
    }

    gptGroupDefMonster->UnlockR((void *&)ptDefMonster);


    if (dwTotalWeight)
    {
        uint32 dwRandomValue = Rand_Internal(SGameGlobalsGet().rng, dwTotalWeight - 1);
        uint32 dwCurrentWeight = 0;
        for (int32 nSkill = 0; nSkill < MAX_MONSTER_SKILLS; ++nSkill)
        {

            if (tSkillWeight[nSkill].snoPower != SNO_NONE)
            {
                dwCurrentWeight += tSkillWeight[nSkill].dwWeight;
                if (dwRandomValue <= dwCurrentWeight)
                {
                    tAttackSettings.snoPower = tSkillWeight[nSkill].snoPower;
                    tAttackSettings.tTarget = tSkillWeight[nSkill].tTarget;
                    break;
                }
            }
        }
    }


}

#include "../../Common/AnimPlayer.h"
// DWARF records only CPP homes for these declarations; no header record found.
void ActorPowersDetachPrimary(ActorID idActor); // ActorPowers.cpp:359
void BrainAPIDoFollow(ActorID idActor, const Target & tFollowTarget, SNO snoPower, float flNextUpdateDelay, bool bAddNoise); // BrainComponents.cpp:9779
void BrainDelayNextUpdate(ActorID idActor, int32 nMinTime, int32 nDeltaTime); // ActorAI.cpp:1402


// Erased argument spelling inferred: snoAction for path calls, settings speed for Backpedal, flDelayMin for Follow.
#line 4589 "X/Server/AI/BrainComponents.cpp"
void BrainAPIDoAIAction(
    ActorID idActor,
    const BrainAttackSettings & tAttackSettings)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);

    if (!ptComp) {

        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


    if (ptComp->nActiveComponent == ACTIVE_COMPONENT_CHARGE)
        return;




    SNO snoAction = tAttackSettings.snoPower;
    const Target & tTarget = tAttackSettings.tTarget;

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    ScopedPowerDefinitionLockR pdefAction(snoAction);
    float flDelayMin = PowerGetFormulaValue(&tACD, pdefAction, TAG_POWER_AI_ACTION_DURATION_MIN, -1, ATTRIBGROUPID_INVALID, 0);
    float flDelayDelta = PowerGetFormulaValue(&tACD, pdefAction, TAG_POWER_AI_ACTION_DURATION_DELTA, -1, ATTRIBGROUPID_INVALID, 0);

    BrainAction eAction = PowerGetBrainAction(snoAction);
    switch (eAction) {
    case BRAIN_ACTION_WANDER:
    {

        BrainWanderSettings tWanderSettings;
        BrainAPIGetWanderSettings(idActor, tWanderSettings);
        BrainAPIDoWander(idActor, tWanderSettings);
        break;
    }
    case BRAIN_ACTION_WANDER_RUN:
    {
        BrainWanderSettings tWanderSettings;
        BrainAPIGetWanderSettings(idActor, tWanderSettings);
        if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_RUN))
            tWanderSettings.dwTagAnimationWalking = TAG_ACTOR_ANIMATION_RUN;
        BrainAPIDoWander(idActor, tWanderSettings);
        break;
    }


    case BRAIN_ACTION_WANDER_TOWN:
    {
        BrainWanderSettings tWanderSettings;
        BrainAPIGetWanderSettings(idActor, tWanderSettings);
        if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_TOWNWALK))
            tWanderSettings.dwTagAnimationWalking = TAG_ACTOR_ANIMATION_TOWNWALK;
        BrainAPIDoWander(idActor, tWanderSettings);
        break;
    }


    case BRAIN_ACTION_WANDER_MINION:
    {
        BrainWanderSettings tWanderSettings;
        BrainAPIGetWanderSettings(idActor, tWanderSettings);
        BrainAPIDoMinionWander(idActor, tWanderSettings);
        break;
    }
    case BRAIN_ACTION_IDLE:

        if (tTarget.IsNone()) {

            BrainAPIDoIdle(idActor);
        }
        else {

            WorldPlace placeTarget;
            tTarget.GetPlace(placeTarget);
            BrainAPIDoIdleAndFace(idActor, placeTarget);
        }
        break;
    case BRAIN_ACTION_STRAFE: BrainAPIDoIdle(idActor); break;

    case BRAIN_ACTION_LOOKAT:
        BrainAPIDoIdle(idActor);
        BrainAPIDoLookAt(idActor, tTarget, TRUE);
        break;


    case BRAIN_ACTION_RUNAWAY:
        BrainAPIDoRunAway(idActor, snoAction, tTarget);
        break;


    case BRAIN_ACTION_CIRCLE:

        BrainAPIDoCircle(idActor, snoAction, tTarget, CIRCLE_DIRECTION_RANDOM_ONCE, FALSE);
        break;

    case BRAIN_ACTION_CIRCLE_STRAFE:
        BrainAPIDoCircle(idActor, snoAction, tTarget, CIRCLE_DIRECTION_RANDOM_ONCE, TRUE);
        break;
    case BRAIN_ACTION_ORBIT:
        BrainAPIDoCircle(idActor, snoAction, tTarget, CIRCLE_DIRECTION_RANDOM_ONCE, FALSE);
        break;



    case BRAIN_ACTION_CLOSE:
    {
        uint32 dwAnimTag;
        if (tAttackSettings.eAnimTag != TAG_ACTOR_ANIMATION_INVALID) {
            dwAnimTag = tAttackSettings.eAnimTag;
        }
        else {


            dwAnimTag = PowerGeneralTagGetInt(pdefAction, TAG_POWER_ANIMATION_TAG);
        }

        BrainAPIDoClose(idActor, tTarget, dwAnimTag, tAttackSettings.flSpeed, snoAction);
        break;
    }
    case BRAIN_ACTION_RUN_NEARBY:
    {
        float flDistMin = PowerGetFormulaValue(&tACD, pdefAction, TAG_POWER_RUN_NEARBY_DISTANCE_MIN, -1, ATTRIBGROUPID_INVALID, 0);
        float flDistDelta = PowerGetFormulaValue(&tACD, pdefAction, TAG_POWER_RUN_NEARBY_DISTANCE_DELTA, -1, ATTRIBGROUPID_INVALID, 0);
        BrainAPIDoRunNearby(idActor, tTarget, flDistMin, flDistDelta, snoAction);
        break;
    }







    case BRAIN_ACTION_FOLLOW:
        flDelayMin = RandRangeFloat_Internal(SGameGlobalsGet().rng, flDelayMin, flDelayDelta);
        flDelayDelta = 0.0f;
        BrainAPIDoFollow(idActor, tTarget, snoAction, flDelayMin, false);
        break;


    case BRAIN_ACTION_FOLLOW_NATURAL:
        flDelayMin = RandRangeFloat_Internal(SGameGlobalsGet().rng, flDelayMin, flDelayDelta);
        flDelayDelta = 0.0f;
        BrainAPIDoFollow(idActor, tTarget, snoAction, flDelayMin, true);
        break;
    case BRAIN_ACTION_RUN_IN_FRONT:
    {

        uint32 dwAnimTag = PowerGeneralTagGetInt(pdefAction, TAG_POWER_ANIMATION_TAG);

        float flDist = PowerGetFormulaValue(&tACD, pdefAction, TAG_POWER_RUN_IN_FRONT_DISTANCE, -1, ATTRIBGROUPID_INVALID, 0);
        BrainAPIDoRunInFront(idActor, tTarget, flDist, dwAnimTag, snoAction);
    }
    // The original branch falls through into ReturnToPath.

    case BRAIN_ACTION_RETURN_TO_PATH:
        BrainAPIReturnToPath(idActor, snoAction);
        break;



    case BRAIN_ACTION_RETURN_TO_GUARD_OBJECT:
        BrainAPIReturnToGuardObject(idActor, snoAction);
        break;



    case BRAIN_ACTION_FOLLOW_PATH:
        BrainAPIFollowPath(idActor, snoAction);
        break;


    case BRAIN_ACTION_BACKPEDAL:
    {
        uint32 dwAnimTag;
        if (tAttackSettings.eAnimTag != TAG_ACTOR_ANIMATION_INVALID) {
            dwAnimTag = tAttackSettings.eAnimTag;
        }
        else {


            dwAnimTag = PowerGeneralTagGetInt(pdefAction, TAG_POWER_ANIMATION_TAG);
        }

        BrainAPIDoBackpedal(idActor, tTarget, dwAnimTag, tAttackSettings.flSpeed, snoAction);
        break;
    }
    default:


        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    ActorPowersDetachPrimary(idActor);


    BrainDelayNextUpdate(idActor, Math::FloatToInt(flDelayMin * TICKS_PER_SECOND), Math::FloatToInt(flDelayDelta * TICKS_PER_SECOND));


    AIStateActivateCooldown(idActor, tAttackSettings.uAICoolDownKey, tAttackSettings.uAICoolDownTicks, NULL);
}

#include "../../Common/Nav/NavZoneIntersect.h"
#include "../../Sigma/Math/Sphere.h"
// These functions have only NavZoneIntersect.cpp records in the DWARF; no header declaration is recorded.
BOOL NavZoneCollideSweptCircle(const NavCellHandle & hNavCellStart, const Circle & wc, const Vector2D & wvDirection, uint16 uOneRequiredFlag, uint16 uPreventFlags, uint32 uCollFlags, NavCellCollideResult & tResult);
BOOL NavZoneCollideSweptPoint(const NavCellHandle & hNavCellStart, const Vector2D & wpStart, const Vector2D & wvDirection, uint16 uOneRequiredFlag, uint16 uPreventFlags, uint32 uCollFlags, NavCellCollideResult & tResult);

#line 9779 "X/Server/AI/BrainComponents.cpp"
void BrainAPIDoFollow(
    ActorID idActor,
    const Target & tFollowTarget,
    SNO snoPower,
    float flNextUpdateDelay,
    bool bAddNoise)
{

    if (!sBrainAPICanDo(idActor))
        return;





    ScopedPowerDefinitionLockR pdefPower(snoPower);

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    float flFollowStopDist = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_FOLLOW_STOP_DISTANCE, -1, ATTRIBGROUPID_INVALID, 0);
    float flFollowMaxDist = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_FOLLOW_START_DISTANCE, -1, ATTRIBGROUPID_INVALID, 0);
    float flRunInFrontDist = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_RUN_IN_FRONT_DISTANCE, -1, ATTRIBGROUPID_INVALID, 0);
    float flOffsetMin = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_RUN_NEARBY_DISTANCE_MIN, -1, ATTRIBGROUPID_INVALID, 0);
    float flOffsetMax = flOffsetMin + PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_RUN_NEARBY_DISTANCE_DELTA, -1, ATTRIBGROUPID_INVALID, 0);

    BOOL bMatchSpeed = PowerGeneralTagGetBool(pdefPower, TAG_POWER_FOLLOW_MATCH_TARGET_SPEED);
    uint32 dwWalkAnimTag = PowerGeneralTagGetInt(pdefPower, TAG_POWER_FOLLOW_WALK_ANIM_TAG);

    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentWalkTo & tWalkTo = ptComp->tWalkTo;

    if (tFollowTarget.IsNone())
        return;





    WorldPlace placeTarget;
    tFollowTarget.GetPlace(placeTarget);
    WorldPlace placeUs = tACD.GetPlace();
    if (placeUs.idSWorld != placeTarget.idSWorld)
        return;




    ACDID idACDTarget = tFollowTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget) {




        float flCurrentDistSqr = Vector3D::Distance2DSquared(placeTarget.wp, placeUs.wp);
        BOOL bInStopRange = flCurrentDistSqr <= flFollowStopDist * flFollowStopDist;
        if (!bInStopRange)
            BrainAPIDoRunNearby(idActor, tFollowTarget, 0.0f, 0.0f, snoPower);
        return;
    }





    sBrainChangeActiveComponent(idActor, ACTIVE_COMPONENT_WALK);
    sBrainComponentSetStatus(idActor, "Close");

    Angle aTargetYaw = ACDGetYaw(idACDTarget);
    Vector3D vTargetFacing(Math::Cos(aTargetYaw), Math::Sin(aTargetYaw), 0.0f);





    WorldPlace placeGotoPoint = placeTarget;
    placeGotoPoint.wp += vTargetFacing * flRunInFrontDist;




    Vector3D vToTarget = placeTarget.wp - placeUs.wp;
    Vector3D vTargetOffset(vTargetFacing);
    Vector3D::Rotate(cgwvZAxis, PI / 2.0f, vTargetOffset);
    float flProjection = Vector3D::DotProduct(vTargetOffset, -vToTarget);
    if (flProjection < 0.0f) {

        vTargetOffset = -vTargetOffset;
    }
    placeGotoPoint.wp += vTargetOffset * Math::Pin(fabsf(flProjection), flOffsetMin, flOffsetMax);

    if (bAddNoise) {


        placeGotoPoint.wp += -vTargetFacing * (flOffsetMax - fabsf(flProjection));
        placeGotoPoint.wp += -vTargetFacing * (Math::Sin(Math::Pin(fabsf(flProjection), 0.0f, PI * 2.0f)) * 0.5f);
    }



    NavCellHandle hNavCellStart;
    NavCellHandleFromWorldPlace(placeTarget, hNavCellStart);

    ACDCollSettings tCollSettings;
    tCollSettings = tACD.tCollSettings;

    Vector2D vToDest = placeGotoPoint.wp.As2D() - placeTarget.wp.As2D();
    Circle circleCurr;
    circleCurr.Set(placeTarget.wp.x, placeTarget.wp.y, tACD.GetRadius());

    BOOL bCollided = FALSE;
    NavCellCollideResult tStaticResult;
    if (circleCurr.wdRadius < 0.01f) {

        bCollided = NavZoneCollideSweptPoint(hNavCellStart, circleCurr.wpCenter, vToDest,
            tCollSettings.uStaticOneRequiredFlags, tCollSettings.uStaticPreventFlags,
            (1 << NAV_ZONE_COLL_ROUNDED_CORNERS) | (1 << NAV_ZONE_COLL_IGNORE_COLLISION_IF_MOVING_AWAY), tStaticResult);
    }
    else {





        bCollided = NavZoneCollideSweptCircle(hNavCellStart, circleCurr, vToDest,
            tCollSettings.uStaticOneRequiredFlags, tCollSettings.uStaticPreventFlags,
            (1 << NAV_ZONE_COLL_ROUNDED_CORNERS) | (1 << NAV_ZONE_COLL_IGNORE_COLLISION_IF_MOVING_AWAY), tStaticResult);
    }





    if (bCollided) {

        placeGotoPoint.wp.As2D() = tStaticResult.wpEnd;


        if (flRunInFrontDist != 0.0f &&
            (placeGotoPoint.wp - placeTarget.wp).MagnitudeSquared() < flFollowStopDist * flFollowStopDist) {

            placeGotoPoint.wp.As2D() = placeUs.wp.As2D();
        }
    }





    ActorMovement * ptMove = tACD.GetActorMovement();
    float flCurrentDistSqr = Vector3D::Distance2DSquared(placeGotoPoint.wp, placeUs.wp);
    if ((!ptMove->IsMoving() || (tWalkTo.placeGotoPoint.wp - placeGotoPoint.wp).MagnitudeSquared() < 0.25f * 0.25f) &&
        flCurrentDistSqr < flFollowStopDist * flFollowStopDist) {

        Player * ptPlayerOwner = tACD.GetPetOwnerPlayer();
        if (ptPlayerOwner && ptPlayerOwner->idPlayerACD == idACDTarget && vTargetFacing.MagnitudeSquared() > EPSILON) {

            WorldPlace wpFaceTarget;
            ptMove->GetPlace(wpFaceTarget);
            wpFaceTarget.wp += vTargetFacing;
            BrainAPIDoFace(idActor, wpFaceTarget);
        }
        else {


            BrainAPIDoFace(idActor, placeTarget);
        }
        return;
    }


    tWalkTo.placeGotoPoint = placeGotoPoint;
    tWalkTo.wdSpeed = 0.0f;
    tWalkTo.dwAnimTag = TAG_ACTOR_ANIMATION_NEUTRAL;
    tWalkTo.nTranslateType = TRANSLATE_TYPE_CLOSING;




    ActorMovement * ptTargetMove = ptACDTarget->GetActorMovement();
    float flTargetSpeed = 0.0f;
    if (ptTargetMove) {

        flTargetSpeed = ptTargetMove->GetDesiredSpeed();
        if (flTargetSpeed > 0.0f) {

            flTargetSpeed = Math::Max(0.27f, flTargetSpeed);
        }
    }
    if (bMatchSpeed && flTargetSpeed > 0.0f &&
        !(ptTargetMove->m_uMoveFlags & (1 << MOVEBIT_FROM_POWER))) {



        if (tACD.AttributesGetFloat(ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT) > 0.0f) {

            tWalkTo.wdSpeed = 0.0f;
        }
        else {



            BOOL bInMaxRange = flCurrentDistSqr <= flFollowMaxDist * flFollowMaxDist;
            if (bInMaxRange) {

                tWalkTo.wdSpeed = flTargetSpeed;


                if (dwWalkAnimTag != uint32(TAG_ACTOR_ANIMATION_INVALID)) {

                    tWalkTo.dwAnimTag = dwWalkAnimTag;
                }
            }
            else {



                tWalkTo.wdSpeed = flTargetSpeed * 1.4f;
            }
        }
    }
    BrainComponentDoMovement(
        idActor,
        tWalkTo.placeGotoPoint,
        tWalkTo.wdSpeed,
        tWalkTo.nTranslateType,
        tWalkTo.dwAnimTag,
        0.1f, "Closing", ACDID_INVALID, FALSE, snoPower);




}

#include "../LuaScript/ScriptEvent.h"
#include "../../Common/Nav/NavigationMesh.h"

#line 9453 "X/Server/AI/BrainComponents.cpp"
void BrainAPIActorAddTarget(
    ActorID idActor,
    ActorCommonData & tACD,
    ActorID idTarget,
    BOOL bRequireLOS,
    BOOL bAddSummoner,
    uint32 uDelayTicks,
    BOOL bForceTarget)
{


    if (idActor == idTarget)
        return;



    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor || !ptActor->ptAI)
        return;




    if (ActorAIIsStatusFlagSet(idActor, ACTOR_AI_DISABLE_UPDATES_BIT))
        return;



    ActorBrain eBrain = ptActor->ptAI->nBrainType;
    if (eBrain == ACTORBRAIN_UNDEFINED || eBrain == ACTORBRAIN_NOBRAIN || eBrain == ACTORBRAIN_DO_NOTHING)
        return;





    BrainComponentData * ptComp = ptActor->ptAI->ptBrainComponentData;
    BrainComponentTarget & tTarget = ptComp->tTarget;

    Actor * ptActorTarget = ActorGet(idTarget);
    ACDID idACDTarget = ptActorTarget->idACD;
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);

    if (tACDTarget.GetLocation().eActorLocation != AL_WORLD ||
        !ACDInSameWorld(tACD, tACDTarget))
        return;



    if (tACDTarget.AttributesGetInt(ATTRIBUTE_CANNOT_BE_ADDED_TO_AI_TARGET_LIST))
        return;



    BOOL bFriendly = TargetEvaluateIsOnTargetTeam(tACD.id, tACDTarget.id, TARGET_TEAM_FRIENDLY, bForceTarget);
    if (!bForceTarget) {

        if (!tACDTarget.IsAlive())
            return;



        ActorType eActorType = tACDTarget.GetActorType();
        if (eActorType != ACTORTYPE_PLAYER && eActorType != ACTORTYPE_MONSTER)
            return;




        if (tACDTarget.IsWreckableObject())
            return;



        if (!tACDTarget.IsTargetable())
            return;



        if (tACDTarget.IsNecroCorpse())
            return;



        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
            return;



        if (!bFriendly) {

            if (((SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_TO_PLAYERS_BIT)) ||
                 (sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_FLYTHROUGH_BIT))) &&
                ACDIsPlayer(tACDTarget.id))
                return;










            if (tACDTarget.IsStealthed())
                return;




            if (bRequireLOS) {

                NavMeshHandle tNavMeshHandle;
                ACDGetNavMeshHandle(tACD, tNavMeshHandle);


                SceneCommonData * ptSCD = SceneCommonDataTryToGet(tNavMeshHandle.idSScene);
                if (!ptSCD || !(ptSCD->dwSceneFlags & (1 << SCENE_ACTIVATED)))
                    return;




                NavCellHandle hNavCellStart;
                WorldPlace placeSource = tACD.GetPlace();
                NavCellHandleFromWorldPlace(placeSource, hNavCellStart);
                if (!NavMeshCheckLOSToACD(hNavCellStart, placeSource, tACDTarget.id, PASSABILITY_PROJECTILE, FALSE))
                    return;
            }




            if (tACD.IsNormalMonster()) {


                int32 nAttackerCount = ptActorTarget->nAttackerCount;


                Player * ptPlayerOwner = tACDTarget.GetPetOwnerPlayer();
                if (ptPlayerOwner) {

                    Actor * ptActorOwner = ActorTryToGet((ActorID)ACDGetOwner(ptPlayerOwner->idPlayerACD));
                    if (ptActorOwner)
                        nAttackerCount += ptActorOwner->nAttackerCount;
                }



                BrainComponentGlobals * ptBrainComponentGlobals = SGameManager::GameGet().ptBrainComponentGlobals;

                int32 nAttackerCap = ptBrainComponentGlobals->uAttackersPerPlayerThrottlingThreshold;

                if (nAttackerCap > 0 && nAttackerCount >= nAttackerCap) {


                    float flDistSquared = WorldPlaceDistanceSquared(tACD.GetPlace(), tACDTarget.GetPlace());
                    if (flDistSquared > ptBrainComponentGlobals->flMaxThrottledAbandonDistance * ptBrainComponentGlobals->flMaxThrottledAbandonDistance)
                        return;
                }
            }
        }
    }













    TTargetList * pListTargets;
    if (bFriendly) {

        pListTargets = &tTarget.listFriendlies;
    }
    else {
        pListTargets = &tTarget.listTargets;
    }


    uint32 dwNow = GameGetServerTime();
    uint32 uActiveTime = dwNow + uDelayTicks;

    POS pos = pListTargets->GetHeadPosition();


    while (pos) {

        TargetInfo & tTargetInfo = pListTargets->GetNext(pos);
        if (tTargetInfo.idTarget == idTarget) {




            if (uActiveTime < tTargetInfo.dwActiveTime) {

                tTargetInfo.dwActiveTime = uActiveTime;
            }
            return;
        }
    }



    TargetInfo tTargetNew;
    tTargetNew.idTarget = idTarget;
    tTargetNew.dwLastSeenTime = bForceTarget ? 0 : dwNow;
    tTargetNew.dwActiveTime = uActiveTime;
    pListTargets->AddTail_Tracked(tTargetNew);

    ScriptEventOnActorSighted(tACD.id, idACDTarget);



    if (bAddSummoner) {


        ACDID idACDSummoner = (ACDID)tACDTarget.AttributesGetInt(ATTRIBUTE_SUMMONED_BY_ACDID);
        if (ACDTryToGet(idACDSummoner)) {

            BrainAPIActorAddTarget(idActor, (ActorID)ACDGetOwner(idACDSummoner), FALSE, FALSE, 0, FALSE);
        }


        Player * ptPlayer = tACDTarget.GetPetOwnerPlayer();
        if (ptPlayer) {

            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer) {

                ActorID idActorOwner = (ActorID)ptACDPlayer->idOwner;
                BrainAPIActorAddTarget(idActor, idActorOwner, FALSE, FALSE, 0, FALSE);


                if (ptACDPlayer->IsAlive()) {

                    BrainAPIActorAddTarget(idActorOwner, idActor, FALSE, FALSE, 0, FALSE);
                }
            }
        }
    }
}

#include "../../Common/Shell/CmdLine.h"
#include "../../Sigma/Sigma.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
// DWARF records only Player.cpp:1634 for this function; no header declaration is recorded.
PlayerAvengerData * GetPlayerAvengerData(Player * ptPlayer, AvengerSource eAvengerSource);


#line 5594 "X/Server/AI/BrainComponents.cpp"
void sAttemptToCreateAvenger(
    ACDID idACDKiller,
    Player * ptPlayer)
{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) || !ptPlayer)
        return;



    if (ptPlayer->eAvengerSource == AVENGER_SOURCE_HARDCORE)
        return;




    Player * ptPlayerKiller = ACDGetControllingPlayer(idACDKiller);
    if (ptPlayerKiller)
        return;




    PlayerAvengerData * ptPlayerAvengerData = GetPlayerAvengerData(ptPlayer, ptPlayer->eAvengerSource);




    Player * ptAvengerOwner = TryToUpgradeAvenger(idACDKiller);
    BOOL bUpgradeExistingAvenger = FALSE;
    if (ptAvengerOwner) {
        bUpgradeExistingAvenger = TRUE;
        ptPlayer = ptAvengerOwner;


        ActorCommonData * ptACDAvengerOwner = ACDTryToGet(ptAvengerOwner->idPlayerACD);
        if (ptACDAvengerOwner) {

            Avenger::Console::ResetAvengerSpawnTimer(*ptACDAvengerOwner);
        }
    }
    ActorCommonData * ptACDKiller = ACDTryToGet(idACDKiller);
    if (!ptACDKiller || !ptPlayer->bIsPrimaryPlayerServerOnly)
        return;



    if (!bUpgradeExistingAvenger) {



        AppGlobals & tAppGlobals = sgtAppGlobals;
        if (!(ptACDKiller->dwFlags & (1 << ACD_IS_FROM_DISTRIBUTION_BIT)) &&
            !(tAppGlobals.dwDebugFlags2 & (1 << APP_GLOBAL2_AVENGER_IGNORE_DISTRO_BIT)))
            return;



        if (!ptACDKiller->IsNormalMonster())
            return;



        ScopedMonsterDefinitionLockR pdefMonster(ptACDKiller->GetMonsterSNO());
        if (!pdefMonster->bCanBeAvenger)
            return;



        ScopedGlobalDefinitionLockR pdefGlobal;
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (!ptACDPlayer)
            return;


        int32 nPlayerLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
        if (nPlayerLevel < pdefGlobal->nAvengerMinimumPlayerLevel)
            return;



        Blizzard::Time::Millisecond uNow = Blizzard::Time::Milliseconds();
        if (Blizzard::Time::GetTimeElapsed(ptPlayerAvengerData->uLastCreationTime, uNow) < pdefGlobal->nAvengerCreationCooldown * MACHINE_TICKS_PER_SECOND)
            return;



        float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
        if (flRoll >= pdefGlobal->flAvengerCreationChance &&
            !(tAppGlobals.dwDebugFlags2 & (1 << APP_GLOBAL2_AVENGER_ALWAYS_CREATE_BIT)))
            return;


    }


    BOOL bCreateAvenger = FALSE;
    switch (ptPlayer->eAvengerSource) {
    case AVENGER_SOURCE_SOLO:

        bCreateAvenger = !ptPlayer->tSoloAvenger.bHasAvenger || bUpgradeExistingAvenger;
        break;

    case AVENGER_SOURCE_FRIEND:
        bCreateAvenger = TRUE;
        break;

    }



    if (!bCreateAvenger)
    {
        return;
    }


    if (!ptACDKiller->ptAvengerData) {

        ptACDKiller->ptAvengerData = new AvengerData;
    }

    int32 nAffixBucket = 0;
    if (bUpgradeExistingAvenger) {

        nAffixBucket = ptPlayerAvengerData->nAvengerAffixBucket;
        ptPlayerAvengerData->ClearCurrentAvenger();
    }
    else {


        int32 nNumberOfBuckets = Avenger::Console::AvengerGetNumberOfAffixBuckets();
        nAffixBucket = RandRange_Internal(SGameGlobalsGet().rng, 0, nNumberOfBuckets - 1);

        ptACDKiller->ptAvengerData->eAvengerSource = ptPlayer->eAvengerSource;
    }

    AvengerVictimArray & arVictims = ptACDKiller->ptAvengerData->arVictims;
    BOOL bRetire = (int32)arVictims.size() >= Avenger::Console::GetMaxPlayerKills();

    if (!bRetire) {


        AvengerVictim tNewVictim;
        tNewVictim.eClass = PlayerGetPlayerClass(ptPlayer);
        tNewVictim.bIsFemale = (ptPlayer->tHeroStateData.uPlayerFlags >> PLAYERFLAG_FEMALE) & 1;
        arVictims.push_back(tNewVictim);

        ptPlayerAvengerData->uLastCreationTime = Blizzard::Time::Milliseconds();
    }


    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, *ptPlayer);
    AvengerInfoMessage tMessage;
    tMessage.annTarget = ANNFromACD(idACDKiller);
    tMessage.nPlayerIndexOfAvengerTarget = ptPlayer->nPlayerIndex;
    tMessage.eAvengerSource = ptPlayer->eAvengerSource;
    tMessage.nAffixBucket = nAffixBucket;
    tMessage.bRetired = bRetire;
    Avenger::Console::VictimsCopyArrayToMessage(ptACDKiller->ptAvengerData->arVictims, tMessage);
    Blizzard::String::Copy(tMessage.szName, "", sizeof(tMessage.szName));
    MessageSendToClient(listDestPlayers, SMSG_AVENGER_CREATION, &tMessage, sizeof(tMessage));

    ptACDKiller->eMonsterRarity = MR_UNIQUE;
    ptACDKiller->SetInvulnerable(TRUE);



    if (ptACDKiller->ptAvengerData->eAvengerSource == AVENGER_SOURCE_SOLO && !bRetire) {

        ptPlayerAvengerData->bHasAvenger = TRUE;
        ptPlayerAvengerData->bHasSpawned = FALSE;
        CRefString sAvengerName(ptPlayer->usHeroName);
        ptPlayerAvengerData->sAvengerName = sAvengerName;
        ptPlayerAvengerData->eAvengerSource = ptACDKiller->ptAvengerData->eAvengerSource;
        ptPlayerAvengerData->nAvengerAffixBucket = nAffixBucket;
        Avenger::Console::VictimsCopyArrayToArray(arVictims, ptPlayerAvengerData->arVictims);
    }

    Avenger::Console::PlayExitEffect(*ptACDKiller);
}

#include "../../Sigma/Math/Cylinder.h"
// DWARF records only CPP homes for these functions, with no header declaration recorded.
BOOL GBGetMovementStyleDef(const GBID gbidMovementStyle, MovementStyleDefinition & tMovementStyleDef); // GameBalanceDefinition.cpp:4223
void ACDsIntersectCapsule(SWorldID idSWorld, const Capsule & capsule, XPooledDataIDList & listACDs, BOOL bForPower); // ActorCollision.cpp:518

#line 4795 "X/Server/AI/BrainComponents.cpp"
static BOOL sBrainComponentDoMovementStyleSwitch(
    const ActorID idActor,
    BrainComponentMovement & tMovement,
    const MovementStyleDefinition & tMovementStyleData,
    WorldPlace & placeGotoPoint,
    uint32 & dwAnimTag)
{
    if (tMovementStyleData.fWalkSwitchStyle &&
        tMovementStyleData.flSwitchChance < EPSILON) {
        dwAnimTag = TAG_ACTOR_ANIMATION_WALK;
        return TRUE;
    }
    else if (tMovementStyleData.fSprintStyle) {
        dwAnimTag = TAG_ACTOR_ANIMATION_SPRINT;
        return TRUE;
    }



    uint32 dwNow = GameGetServerTime();
    if (dwNow > tMovement.dwWalkTimer) {


        tMovement.dwRunTimer = dwNow + RandRange_Internal(SGameGlobalsGet().rng, Math::FloatToInt(tMovementStyleData.flRunTimer * TICKS_PER_SECOND), Math::FloatToInt(tMovementStyleData.flRunDeltaTimer * TICKS_PER_SECOND));


        tMovement.dwWalkTimer = tMovement.dwRunTimer + RandRange_Internal(SGameGlobalsGet().rng, Math::FloatToInt(tMovementStyleData.flWalkTimer * TICKS_PER_SECOND), Math::FloatToInt(tMovementStyleData.flWalkDeltaTimer * TICKS_PER_SECOND));
    }



    if (dwNow >= tMovement.dwRunTimer) {


        if (dwNow < tMovement.dwRunTimer + TICKS_PER_SECOND / 4 &&
            RandFloat_Internal(SGameGlobalsGet().rng) > tMovementStyleData.flSwitchChance) {

            tMovement.dwWalkTimer = 0;
        }
        else {

            if (tMovementStyleData.fWalkSwitchStyle) {
                dwAnimTag = TAG_ACTOR_ANIMATION_WALK;
            }
            else {


                Actor * ptActor = ActorGet(idActor);
                ptActor->ptSActorMovement->FaceTarget(placeGotoPoint.wp, FALSE);
                BrainStanceDoIdle(idActor);
                return FALSE;
            }
        }
    }
    return TRUE;
}





static BOOL sBrainComponentDoMovementStyleBlock(
    const ActorID idActor,
    BrainComponentMovement & tMovement,
    const MovementStyleDefinition & tMovementStyleData,
    WorldPlace & placeGotoPoint,
    uint32 & dwAnimTag)
{
    uint32 dwNow = GameGetServerTime();
    ACDID idACD = ACDFindByActor(idActor);

    if (dwNow >= tMovement.dwBlockCooldownTimer) {

        ActorCommonData & tACD = ACDGet(idACD);


        WorldPlace placeACD = tACD.GetPlace();
        Vector3D wvToTarget;
        WorldPlaceGetVectorBetween(placeACD, placeGotoPoint, wvToTarget);
        wvToTarget.Normalize();
        wvToTarget *= tMovementStyleData.flBlockDistance;
        WorldPlace placeBlocked;
        WorldPlaceOffset(placeACD, wvToTarget, placeBlocked);




        placeACD.wp.z += 0.2f;
        placeBlocked.wp.z += 0.2f;


        Capsule wCapsule;
        wCapsule.wp1 = placeBlocked.wp;
        wCapsule.wp2 = placeACD.wp;
        wCapsule.flRadius = tACD.GetRadius();

        XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectCapsule(placeACD.idSWorld, wCapsule, listTargets, FALSE);


        BOOL fBlocked = FALSE;
        POS pos = listTargets.GetHeadPosition();
        while (pos) {

            ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
            if (idACDTarget != idACD) {

                ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
                if (ptACDTarget &&
                    ptACDTarget->IsAlive() &&
                    (tMovementStyleData.fWalkBlockStyle || !ACDIsPlayer(idACDTarget))) {
                    fBlocked = TRUE;
                    break;
                }
            }
        }



        if (fBlocked) {

            if (!(dwNow < tMovement.dwBlockCooldownTimer + TICKS_PER_SECOND / 4 &&
                RandFloat_Internal(SGameGlobalsGet().rng) > tMovementStyleData.flBlockChance)) {










                tMovement.dwBlockTimer = dwNow + RandRange_Internal(SGameGlobalsGet().rng, Math::FloatToInt(tMovementStyleData.flBlockTimer * TICKS_PER_SECOND), Math::FloatToInt(tMovementStyleData.flBlockDeltaTimer * TICKS_PER_SECOND));


                tMovement.dwBlockCooldownTimer = tMovement.dwBlockTimer + RandRange_Internal(SGameGlobalsGet().rng, Math::FloatToInt(tMovementStyleData.flBlockCooldown * TICKS_PER_SECOND), Math::FloatToInt(tMovementStyleData.flBlockCooldownDelta * TICKS_PER_SECOND));


                tMovement.dwRunTimer = 0;


                if (tMovementStyleData.fWalkBlockStyle) {
                    dwAnimTag = TAG_ACTOR_ANIMATION_WALK;
                }
                else {


                    Actor * ptActor = ActorGet(idActor);

                    ptActor->ptSActorMovement->FaceTarget(placeGotoPoint.wp, FALSE);
                    BrainStanceDoIdle(idActor);
                    return FALSE;
                }
            }
        }
    }

    else if (dwNow < tMovement.dwBlockTimer) {





        if (tMovementStyleData.fWalkBlockStyle) {
            dwAnimTag = TAG_ACTOR_ANIMATION_WALK;
        }
        else {


            Actor * ptActor = ActorGet(idActor);
            ptActor->ptSActorMovement->FaceTarget(placeGotoPoint.wp, FALSE);
            BrainStanceDoIdle(idActor);
            return FALSE;
        }
    }
    return TRUE;
}





static void sBrainComponentDoMovementStyleFlank(
    const ActorID idActor,
    BrainComponentMovement & tMovement,
    const MovementStyleDefinition & tMovementStyleData,
    WorldPlace & placeGotoPoint,
    uint32)
{
    if (tMovement.flMoveDirection <= 0.0f) {

        tMovement.flMoveDirection = RandRangeFloat_Internal(SGameGlobalsGet().rng, tMovementStyleData.flFlankAngle, tMovementStyleData.flFlankDeltaAngle);



        if (RandFloat_Internal(SGameGlobalsGet().rng) < 0.5f) {


            tMovement.flMoveDirection = 360.0f - tMovement.flMoveDirection;
        }
    }

    const WorldPlace tPlace = ACDGet(ACDFindByActor(idActor)).GetPlace();


    WorldDistance wdDistanceSquared = WorldPlaceDistanceSquared(tPlace, placeGotoPoint);
    float flRotate = tMovement.flMoveDirection;
    if (wdDistanceSquared > tMovementStyleData.flFlankMaxDistance * tMovementStyleData.flFlankMaxDistance) {
        flRotate = 0.0f;
    }
    else {



        float flPeakDistanceSq = tMovementStyleData.flFlankPeakDistance * tMovementStyleData.flFlankPeakDistance;
        if (wdDistanceSquared < flPeakDistanceSq) {

            if (flRotate < 180.0f) {

                flRotate *= wdDistanceSquared / flPeakDistanceSq;
            }
            else {

                flRotate = (360.0f - flRotate) * (wdDistanceSquared / flPeakDistanceSq);
                flRotate = 360.0f - flRotate;
            }
        }
    }

    Vector3D wvToTarget;
    WorldPlaceGetVectorBetween(tPlace, placeGotoPoint, wvToTarget);

    Angle aRotate = Math::DegToAngle(flRotate);
    Vector3D::Rotate(cgwvZAxis, aRotate, wvToTarget);

    WorldPlaceOffset(tPlace, wvToTarget, placeGotoPoint);
}



static void sBrainComponentDoMovementStyleZigZag(
    const ActorID idActor,
    BrainComponentMovement & tMovement,
    const MovementStyleDefinition & tMovementStyleData,
    WorldPlace & placeGotoPoint,
    uint32)
{
    if (tMovement.flZigZagDirection <= 0.0f) {

        tMovement.flZigZagDirection = RandRangeFloat_Internal(SGameGlobalsGet().rng, tMovementStyleData.flZigZagAngle, tMovementStyleData.flZigZagDeltaAngle);



        if (RandFloat_Internal(SGameGlobalsGet().rng) < 0.5f) {


            tMovement.flZigZagDirection = 360.0f - tMovement.flZigZagDirection;
        }
    }

    uint32 dwNow = GameGetServerTime();
    if (tMovement.dwZigZagTimer <= dwNow) {


        tMovement.flZigZagDirection = 360.0f - tMovement.flZigZagDirection;
        tMovement.dwZigZagTimer = dwNow + Math::FloatToInt(tMovementStyleData.flZigZagTimer * TICKS_PER_SECOND);
    }

    const WorldPlace tPlace = ACDGet(ACDFindByActor(idActor)).GetPlace();


    WorldDistance wdDistanceSquared = WorldPlaceDistanceSquared(tPlace, placeGotoPoint);
    float flRotate = tMovement.flZigZagDirection;
    if (wdDistanceSquared > tMovementStyleData.flZigZagMaxDistance * tMovementStyleData.flZigZagMaxDistance) {
        flRotate = 0.0f;
    }
    else {



        float flPeakDistanceSq = tMovementStyleData.flZigZagPeakDistance * tMovementStyleData.flZigZagPeakDistance;
        if (wdDistanceSquared < flPeakDistanceSq) {

            if (flRotate < 180.0f) {

                flRotate *= wdDistanceSquared / flPeakDistanceSq;
            }
            else {

                flRotate = (360.0f - flRotate) * (wdDistanceSquared / flPeakDistanceSq);
                flRotate = 360.0f - flRotate;
            }
        }
    }

    Vector3D wvToTarget;
    WorldPlaceGetVectorBetween(tPlace, placeGotoPoint, wvToTarget);

    Angle aRotate = Math::DegToAngle(flRotate);
    Vector3D::Rotate(cgwvZAxis, aRotate, wvToTarget);

    WorldPlaceOffset(tPlace, wvToTarget, placeGotoPoint);
}
#include "../../Common/Actors/ActorEffectSwig.h"
#include "../../Common/Powers/SpellEffectCommon.h"
#include "../../Common/Quests/Quests.h"
#include "../SPlayer.h"
#include "../SWorld.h"
#include "../ConversationMgr.h"
#include "../System/BossEncounter.h"
#include "../System/SVoteKick.h"
#include "../System/SNephalemRift.h"
#include "../System/SetEncounter.h"
BOOL TradeIsTrading(const ACDID idACDPlayer);  // X/Common/Trade.cpp:270
void STradeCancel(ACDID idACDPlayer);  // X/Server/STrade.cpp:756
#line 5412 "X/Server/AI/BrainComponents.cpp"

static BOOL sACDDropsLootOnDeath(
    const ACDID idACDDying)
{

    ActorCommonData & tACDDying = ACDGet(idACDDying);
    if (tACDDying.IsWreckableObject())
    {
        // wreckables never drop loot on death
        return FALSE;
    }
    if (ACDIsPlayer(tACDDying.id) || tACDDying.IsPet())
    {
        // neither do players or pets
        return FALSE;
    }
    if (ACDIsInTieredLootRun(tACDDying))
    {
        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            // BlizzCon rifts: only the treasure goblins drop loot

            const int32 TREASURE_GOBLIN_B = 5985;
            const int32 TREASURE_GOBLIN_C = 5987;
            const int32 TREASURE_GOBLIN_F = 408989;

            if (tACDDying.snoActor == TREASURE_GOBLIN_B || tACDDying.snoActor == TREASURE_GOBLIN_C || tACDDying.snoActor == TREASURE_GOBLIN_F)
                return TRUE;
        }
        return FALSE;
    }

    return TRUE;
}





#line 5102 "X/Server/AI/BrainComponents.cpp"
void BrainComponentDoMovement(
    ActorID idActor,
    const WorldPlace & placeTarget,
    WorldDistance wdSpeed,
    TranslateType nTranslateType,
    uint32 dwAnimTag,
    WorldDistance wdMaxRangeFromDest,
    LPCSTR sTraceString,
    ACDID idACDTarget,
    BOOL bIgnoreMovementStyles,
    SNO snoPowerPassability)
{

    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    if (!ptComp) {

        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    BrainComponentMovement & tMovement = ptComp->tMovement;

    WorldPlace placeGotoPoint = placeTarget;

    float flSpeedScalar = 0.0f;
    float flAccel = 0.0f;
    if (!bIgnoreMovementStyles) {

        ActorAI & tAI = ActorGetAI(idActor);
        MovementStyleDefinition tMovementStyleData;
        GBGetMovementStyleDef(tAI.gbidMovementStyle, tMovementStyleData);

        if (dwAnimTag == TAG_ACTOR_ANIMATION_NEUTRAL) {

            if (tMovementStyleData.fWalkSwitchStyle || tMovementStyleData.fStopSwitchStyle || tMovementStyleData.fSprintStyle) {




                if (!sBrainComponentDoMovementStyleSwitch(idActor, tMovement, tMovementStyleData, placeGotoPoint, dwAnimTag))
                    return;
            }



            if (tMovementStyleData.fWalkBlockStyle || tMovementStyleData.fStopBlockStyle) {


                if (!sBrainComponentDoMovementStyleBlock(idActor, tMovement, tMovementStyleData, placeGotoPoint, dwAnimTag))
                    return;
            }
        }



        if (tMovementStyleData.fSprintRunRandomStyle) {

            if (tMovement.dwSprintRunSelected == TAG_ACTOR_ANIMATION_NEUTRAL) {

                tMovement.dwSprintRunSelected = RandFloat_Internal(SGameGlobalsGet().rng) > tMovementStyleData.flSwitchChance ? TAG_ACTOR_ANIMATION_RUN : TAG_ACTOR_ANIMATION_SPRINT;
            }
            dwAnimTag = tMovement.dwSprintRunSelected;
        }



        if (tMovementStyleData.fFlankStyle) {



            sBrainComponentDoMovementStyleFlank(idActor, tMovement, tMovementStyleData, placeGotoPoint, 0);
        }

        if (tMovementStyleData.fZigZagStyle) {



            sBrainComponentDoMovementStyleZigZag(idActor, tMovement, tMovementStyleData, placeGotoPoint, 0);
        }

        flSpeedScalar = tMovementStyleData.flSpeedScalar;

        flAccel = tMovementStyleData.flAccel > EPSILON ? tMovementStyleData.flAccel : 0.0f;
    }




    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);

    if (dwAnimTag == TAG_ACTOR_ANIMATION_NEUTRAL) {
        dwAnimTag = TAG_ACTOR_ANIMATION_RUN;
    }




    if (dwAnimTag == TAG_ACTOR_ANIMATION_RUN && ptACD && AnimPlayerAnimExists(*ptACD, TAG_ACTOR_ANIMATION_TOWNRUN)) {

        AIStateType eStateType = AIStateGetCurrState(idActor);
        if (eStateType == AISTATE_TYPE_IDLE || eStateType == AISTATE_TYPE_SETUP) {
            dwAnimTag = TAG_ACTOR_ANIMATION_TOWNRUN;
        }
    }




    if (flSpeedScalar > EPSILON) {

        if (wdSpeed <= EPSILON && ptACD) {



            wdSpeed = ptACD->GetWalkingSpeed(dwAnimTag);
        }


        wdSpeed *= 1.0f + flSpeedScalar;
    }

    BrainStanceDoTranslate(idActor, placeGotoPoint, wdSpeed, flAccel, nTranslateType, dwAnimTag, wdMaxRangeFromDest, sTraceString, idACDTarget, snoPowerPassability);










}

// Declarations from the original defining compilation units.
BOOL AIStateIsKeyOnCooldown(ActorID idActor, uint32 uAICooldownKey); // AIStates.cpp:6327
EBrainPowerResult BrainStanceFailureReason(ActorID idActor, SNO snoPower, const Target & tTarget, BOOL bHoldPosition); // BrainStance.cpp:938
BOOL PowerIsCoolDownDone(const ActorCommonData & tACD, SNO snoPower); // PowersCommon.cpp:4470
BOOL PowerRollToTargetNavPlace(ACDID idACD, const Target & tTarget, const PassabilityType ePassabilityType, const BOOL fIgnoreDestinationHeight, NavPlace & tNavPlace); // PowersCommon.cpp:5139
void PowerSetCoolDown(ActorCommonData & tACD, const uint32 dwCooldown); // Powers.cpp:1715
#line 1131 "X/Server/AI/BrainComponents.cpp"
static EBrainPowerResult sAttackFailureReason(
    ActorID idActor,
    const Target & tTarget,
    SNO snoPower,
    BOOL bHoldPosition)
{

    ACDID idACD = ActorGet(idActor)->idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentAttack & tAttack = ptComp->tAttack;

    EBrainPowerResult eReason = BrainStanceFailureReason(idActor, snoPower, tTarget, bHoldPosition);





    if (eReason == BRAIN_POWER_GET_CLOSER &&
        tAttack.bAttackQueued == TRUE) {


        if (PowerIsCloseEnough(tACD.id, snoPower, &tTarget, 0.0f, 5.0f, NULL, FALSE))
            eReason = BRAIN_POWER_START;
    }



    else if (eReason == BRAIN_POWER_QUEUE_AND_STAND &&
             BrainGetStanceType(idActor) == STANCE_SKILL &&
             !PowerIsCoolDownDone(tACD, snoPower)) {

        eReason = BRAIN_POWER_RETARGET;
    }










    if (tACD.IsMonster() &&
        tACD.eMonsterRarity != MR_BOSS &&
        !BrainAPIIsActorExcludedFromPerformanceOptimizations(tACD) &&
        !tACD.IsPlayerTeam()) {

        if (AIStateIsKeyOnCooldown(idActor, tAttack.tSettings.uAICoolDownKey))

            eReason = BRAIN_POWER_FAIL;




        if (tAttack.tSettings.nSuccessfulCasts > 0 &&
            tAttack.tSettings.snoPower != GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT))

            return BRAIN_POWER_FAIL;

    }



    if (eReason != BRAIN_POWER_START && eReason != BRAIN_POWER_RETARGET)

        return eReason;





    if (!(tAttack.tSettings.dwAttackFlags & (1 << ATTACK_DONT_CLOSE))) {




        PowerStartLOSType eLOSType = PowerGetStartLOS(snoPower);
        if (eLOSType == START_LOS_NORMAL) {


            NavMeshHandle tNavMeshHandle;
            ACDGetNavMeshHandle(tACD, tNavMeshHandle);

            NavPlace tNavPlace;
            BOOL bLOS = PowerRollToTargetNavPlace(
                tACD.id, tTarget, PASSABILITY_PROJECTILE, FALSE, tNavPlace);
            if (!bLOS)

                return BRAIN_POWER_NO_LINE_OF_SIGHT;


        }
    }





    if (tAttack.bNeedWarmUp) {

        tAttack.bNeedWarmUp = FALSE;

        BOOL bDoWarmUp = TRUE;
        if (snoPower != SNO_NONE) {

            ScopedPowerDefinitionLockR pdefPower(snoPower);
            bDoWarmUp = PowerGeneralTagGetBool(pdefPower, TAG_POWER_USES_ATTACK_WARMUP);
        }

        if (bDoWarmUp) {


            uint32 dwCoolDownMin = tAttack.tSettings.dwAttackWarmUp;
            uint32 dwCoolDownDelta = tAttack.tSettings.dwAttackWarmUpDelta;
            uint32 dwWarmUp = RandRange_Internal(SGameGlobalsGet().rng, dwCoolDownMin, dwCoolDownDelta);
            if (dwWarmUp) {

                uint32 dwNow = GameGetServerTime();
                PowerSetCoolDown(tACD, dwNow + dwWarmUp);
                return BRAIN_POWER_QUEUE_AND_STAND;
            }
        }
    }

    if (!PowerCanTarget(snoPower, idACD, &tTarget))

        return BRAIN_POWER_FAIL;

    return eReason;

}
#line 1343 "X/Server/AI/BrainComponents.cpp"
static BOOL sCloseOnLocation(
    ActorCommonData & tACD,
    ActorCommonData & tACDLocation)
{
    ActorID idActor = (ActorID)ACDGetOwner(tACD.id);

    if (tACDLocation.GetLocationType() != AL_WORLD)

        return FALSE;



    float flTotalRadius = tACD.GetRadius() + tACDLocation.GetRadius();
    WorldPlace placeCurr = tACD.GetPlace();
    WorldPlace placeLocation = tACDLocation.GetPlace();
    Vector2D vDelta2D = (const Vector2D &)placeLocation.wp - (const Vector2D &)placeCurr.wp;
    float flDist2DSqr = vDelta2D.MagnitudeSquared();
    if (flDist2DSqr <= flTotalRadius * flTotalRadius)

        return TRUE;


    BrainComponentDoMovement(
        idActor,
        placeLocation,
        0.0f,
        TRANSLATE_TYPE_CLOSING,
        TAG_ACTOR_ANIMATION_NEUTRAL,
        3.0f,
        "ClosingToLocation",
        tACDLocation.id, TRUE, SNO_NONE);

    return FALSE;

}
#line 5627 "X/Server/AI/BrainComponents.cpp"

PowerID BrainStanceDoPower(ActorID idActor, const Target & tTarget, SNO snoPower, EBrainPowerResult eReason, void * ptExtraPowerStartData, const PreplayedAnimData * ptPreplayedAnimData, int32 nComboLevel, uint32 dwTargetMessageFlags); // BrainStance.cpp:2482
void SPlayerClearEnemyList(Player & tPlayer);  // X/Server/SPlayer.cpp:5149
void ServerMessageSendVictimMsg(const int32 nPlayerVictimIndex, const ACDID idACDKiller, const SNO snoPowerDmgSource);  // X/Server/Network/ServerMessageSend.cpp:1591
void SPlayerSetKiller(Player & tPlayer, const GBHandle & hGBKiller, SNO snoKiller, MonsterRarity eKillerRarity, const GBID * pRareNameGBIDs, SNO snoKilledIn);  // X/Server/SPlayer.cpp:11418
BOOL ACDIsPet(const ACDID idACD);  // X/Common/Player.cpp:1666
void PowerAPI_BuffApplyOnKillEffects(float * flDamageDone, PowerAPIDamage * ptPowerDamage, ACDID idACDAttacker, ACDID idACDKilled);  // X/Server/Powers/PowerAPI.cpp:6241
void PowerAPI_BuffApplyOnWreckableDestroyedEffects(ACDID idACDAttacker, ACDID idACDKilled);  // X/Server/Powers/PowerAPI.cpp:6578
int32 ExperienceGetPlayersEligibleForKill(ACDID idACDVictim, PlayerList & listPlayersToEarnXP);  // X/Server/Experience.cpp:882
void TraceStat(LPCSTR sEventType, ACDID idACD, LPCSTR cpFmtStr, ...);  // X/Server/SGame.cpp:7255
ACDID ACDGetPlayerACDOnSameTeam(const ACDID idACD);  // X/Common/Player.cpp:1415
void GizmoOperateReformingDestroyable(ActorCommonData & tACD);  // X/Server/SGizmo.cpp:3237
BOOL PowerActorCanDie(ActorID idActor);  // X/Server/ActorIsAffected.cpp:351
BOOL ACDIsTrackedInOpenWorld(const ActorCommonData & tACD);  // X/Server/SGizmo.cpp:99
void SGameStatsRecordKill(ActorCommonData & tACDVictim, ACDID idACDKiller, const PowerAPIDamage * ptDamage);  // X/Server/SGameStats.cpp:695
#line 5779 "X/Server/AI/BrainComponents.cpp"

static void sBrainAPIDeathPlayerLogic(
    const ACDID idACDVictim,
    const ACDID idACDKiller,
    const SNO snoPowerDmgSource)
{

    if (TradeIsTrading(idACDVictim))
    {
        STradeCancel(idACDVictim);
    }

    ActorCommonData *ptACDVictim = ACDTryToGet(idACDVictim);

    PowerAPI_RecomputeHitpointHealTarget(ptACDVictim);

    Player *ptPlayer = PlayerGetByACD(idACDVictim);

    if (ptACDVictim && ptACDVictim->AttributesGetInt(ATTRIBUTE_CURRENT_COSMETIC_PET) != -1)
    {
        // remember the cosmetic pet so it comes back with the player
        ptACDVictim->AttributesSetInt(ATTRIBUTE_LAST_COSMETIC_PET, ptACDVictim->AttributesGetInt(ATTRIBUTE_CURRENT_COSMETIC_PET));
    }

    // pets die with their owner
    SPlayerKillAllPets(*ptPlayer, FALSE, TRUE, TRUE, FALSE);
    SPlayerClearEnemyList(*ptPlayer);


    Player *ptPlayerKiller = ACDGetControllingPlayer(idACDKiller);

    if (ptACDVictim && ptACDVictim->IsDueling())
    {
        if (ptPlayerKiller)
        {
            DuelResultMessage tMessage;
            tMessage.nPlayerKillerIndex = ptPlayerKiller->nPlayerIndex;
            tMessage.nPlayerVictimIndex = ptPlayer->nPlayerIndex;
            tMessage.snoPowerDmgSource = snoPowerDmgSource;

            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
            MessageSendToClient(listDestPlayers, SMSG_DUEL_RESULT, &tMessage, sizeof(tMessage));
        }
    }
    else
    {
        ServerMessageSendVictimMsg(ptPlayer->nPlayerIndex, idACDKiller, snoPowerDmgSource);
    }

    // durability loss on death
    sProcessDurabilityLoss(ptACDVictim);


    // remember what killed the player (not for player kills)
    ActorCommonData *ptACDKiller = ACDTryToGet(idACDKiller);
    ACDID idACDSummoner = ACDGetSummoner(idACDKiller);
    if (ACDIsPlayer(idACDKiller) || (idACDSummoner != ACDID_INVALID && ACDIsPlayer(idACDSummoner)))
    {
        // killed by a player or a player's summon
    }
    else if (ptACDKiller)
    {
        SNO snoWorld = SNO_NONE;
        WorldPlace tPlace = ptACDKiller->GetPlace();
        if(tPlace.idSWorld != SWORLDID_INVALID)
        {
            const WorldCommonData &tWCD = WorldGetCommonData(tPlace.idSWorld);
            snoWorld = tWCD.snoWorld;
        }
        SPlayerSetKiller(*ptPlayer, ptACDKiller->GetGBHandle(), ptACDKiller->snoActor, ptACDKiller->eMonsterRarity, ptACDKiller->pRareNameGBIDs, snoWorld);
    }

    sAttemptToCreateAvenger(idACDKiller, ptPlayer);
}





static void sDoLifeOnKill(
    ActorCommonData & tACDKiller)
{
    float flHealthPerKill = tACDKiller.AttributesGetFloat(ATTRIBUTE_HITPOINTS_ON_KILL_TOTAL);
    if (Math::Abs(flHealthPerKill) > EPSILON)
    {
        tACDKiller.Heal(flHealthPerKill, TRUE);
        CombatLog::LogEvent(
            CombatLog::LOG_EVENT_HEALING_DONE,
            tACDKiller.id,
            tACDKiller.id,
            SNO_NONE,
            flHealthPerKill,
            0.0f,
            HEAL_TYPE_ON_KILL);
    }

    if (ACDIsPet(tACDKiller.id))
    {
        Player *ptPlayerOwner = tACDKiller.GetPetOwnerPlayer();
        if (ptPlayerOwner)
        {
            ActorCommonData *ptACDPlayerOwner = ACDTryToGet(ptPlayerOwner->idPlayerACD);
            if (ptACDPlayerOwner && ptACDPlayerOwner->IsAlive())
                sDoLifeOnKill(*ptACDPlayerOwner);
        }   }   }
void SPlayerAllPlayersLearnLore(const SNO snoLore);  // X/Server/SPlayer.cpp:7245
enum { MF_TREASURE_GOBLIN = 11 };  // X/Common/ActorCommonDataSwig.h:41 (kept local, as in SGame.cpp)
void SQuestsOnActorKilled(ACDID idACDKiller, ActorCommonData & tACDVictim);  // X/Server/Quests/SQuests.cpp:4585
void SPlayerIncreaseKillCounter(const ACDID idACDAttacker, const ACDID idACDVictim, PowerAPIDamage * ptDamage);  // X/Server/SPlayer.cpp:9880

static void sBrainAPIDeathExecuteSlayingPowers(
    float * flDamageDone,
    PowerAPIDamage * ptPowerDamage,
    ActorCommonData & tACDKiller,
    ActorCommonData & tACDVictim)
{
	if (tACDVictim.IsWreckableObject())
	{
        PowerAPI_BuffApplyOnWreckableDestroyedEffects(tACDKiller.id, tACDVictim.id);
        return;
	}

    Target tTarget;
    tTarget.InitializeFromACD(tACDVictim.id, FALSE, FALSE);






    BOOL bFriendly = ACDIsFriendly(tACDKiller, tACDVictim, FALSE);
    if (!bFriendly)
    {
        PowerAPI_BuffApplyOnKillEffects(flDamageDone, ptPowerDamage, tACDKiller.id, tACDVictim.id);

        PlayerList listPlayersToEarnXP(PlayerListGetElementPool());
        int32 nNumPlayers = ExperienceGetPlayersEligibleForKill(tACDVictim.id, listPlayersToEarnXP);
        if (nNumPlayers)
        {
            POS pos = listPlayersToEarnXP.GetHeadPosition();
            while (pos)
            {
                Player *ptPlayer = listPlayersToEarnXP.GetNext(pos);
                if (ptPlayer->idPlayerACD == tACDKiller.id)
                    continue;

                // everyone else eligible for the kill gets the assist

                ActorCommonData &tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
                PowerAPI_BuffApplyOnAssistEffects(tACDPlayer.id, tACDVictim.id);
            }
        }
    }

    // resource on kill, primary then secondary resource

    {
        ResourceType eType = tACDKiller.GetPrimaryResourceType();
        float flManaPerKill = tACDKiller.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_ON_KILL, eType);
        if (Math::Abs(flManaPerKill) > EPSILON)
        {
            tACDKiller.AddResource(eType, flManaPerKill, FALSE);
        }
    }

    {
        ResourceType eType = tACDKiller.GetSecondaryResourceType();
        if (eType != RESOURCE_TYPE_INVALID)
        {
            float flManaPerKill = tACDKiller.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_ON_KILL, eType);
            if (Math::Abs(flManaPerKill) > EPSILON)
            {
                tACDKiller.AddResource(eType, flManaPerKill, FALSE);
            }
        }
    }


    if (!bFriendly)
    {
        sDoLifeOnKill(tACDKiller);
    }
}




static uint32 sBrainAPIDeathCalculateBodyRemoveTime(
    ACDID idACDVictim,
    BOOL fDeathReplacementEffect)
{
    uint32 dwNow = GameGetServerTime();

    ActorCommonData & tACDVictim = ACDGet(idACDVictim);
    BOOL fVictimIsPlayer = ACDIsPlayer(tACDVictim.id);
    BOOL fVictimIsHireling = ACDIsHireling(tACDVictim);
    SNO snoResurrectPower = (SNO)tACDVictim.AttributesGetInt(ATTRIBUTE_RESURRECTION_POWER);

    uint32 dwResult = dwNow;
    if (fVictimIsPlayer)
    {
        dwResult = dwNow + TICKS_PER_SECOND;
    }
    else if (fVictimIsHireling)
    {
        dwResult = (uint32)-1;  // hirelings are never removed (mov w23, #-1 is a line-0 row: tail-merged)
    }
    else if (snoResurrectPower != SNO_NONE)
    {
        // the body stays until the resurrection power is done with it
        float flDelayInSeconds = PowerGetFormulaValue(&tACDVictim, AutoPowerDefinitionR(snoResurrectPower), TAG_POWER_DEAD_TIME_UNTIL_RESURRECT, -1, -1, 0);
        dwResult = dwNow + Math::FloatToInt(flDelayInSeconds * TICKS_PER_SECOND);
    }
    else if (fDeathReplacementEffect)
    {
        // the death replacement effect takes the body away:
        // remove it on the next tick
        dwResult = dwNow + 1;
    }
    else if (tACDVictim.AttributesGetInt(ATTRIBUTE_REMOVES_BODY_ON_DEATH))
    {
        BOOL bFades = TRUE;
        Actor *ptActor = ActorTryToGet((ActorID)ACDGetOwner(tACDVictim.id));
        if (ptActor)
        {
            ScopedActorDefinitionLockR ptDefActor(ptActor->snoActor);
            bFades = (BOOL)TagMapGetInt(ptDefActor->hTagMap, TAG_ACTOR_FADE_CORPSE_ON_DEAD);
        }

        if (bFades)
        {
            dwResult = dwNow + TICKS_PER_SECOND;
        }
    }
    // everything else: the corpse stays for five minutes,
    // except in PvP games where it goes at once
    // (18000 ticks = 5 * 60 * TICKS_PER_SECOND)

    else if (GameTypeIsPvP())
    {
        // dwResult stays dwNow
    }
    else
    {
        dwResult = dwNow + 18000;
    }

    return dwResult;
}




static void sBrainTraceDeathStat(
    ACDID idACDVictim,
    ACDID idACDKiller,
    uint32 dwGameEventNumber)
{
    if (!TraceStatEnabled())
        return;



    ActorCommonData *ptVictim = &ACDGet(idACDVictim);
    ActorCommonData *ptKiller = ACDTryToGet(idACDKiller);

    CRefString sVictimName = SNOToString(ACTOR_SNO, ptVictim->snoActor, FALSE);
    CRefString sKillerName = "NOKILLER";

    if (ptKiller)
    {
        sKillerName = SNOToString(ACTOR_SNO, ptKiller->snoActor, FALSE);
    }

    if (ACDIsPlayer(idACDVictim))
    {
        TraceStat("playerdied", idACDVictim, "%u\t%s", dwGameEventNumber, sKillerName.str());
        return;
    }
    // credit the kill to the killer's player, when there is one
    ACDID idACDForInfo = idACDKiller;
    ACDID idACDPlayerTeam = ACDGetPlayerACDOnSameTeam(idACDKiller);

    if (idACDPlayerTeam != ACDID_INVALID)
    {
        idACDForInfo = idACDPlayerTeam;
    }

    TraceStat("kill", idACDForInfo, "%u\t%s\t%s", dwGameEventNumber, sKillerName.str(), sVictimName.str());
}


#line 1950 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentSpecialPowerUpdate(
    ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentSpecialPower & tSpecialPower = ptComp->tSpecialPower;

    if (BrainConditionGetCondition(idActor) == BRAIN_CONDITION_READY) {

        BrainConditionSetCondition(idActor, BRAIN_CONDITION_PERFORMING);
        BrainStanceDoPower(idActor, tSpecialPower.tTarget, tSpecialPower.snoPower, BRAIN_POWER_START, NULL, NULL, 0, 0);
    }


}
#line 2407 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentFrozenUpdate(
    ActorID idActor)
{
    const Actor * ptActor = ActorGet(idActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);


    if (tACD.AttributesGetInt(ATTRIBUTE_FROZEN) <= 0) {

        BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
        BrainAPIDoIdle(idActor);
    }


}
#line 2425 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentStunnedUpdate(
    ActorID idActor)
{
    const Actor * ptActor = ActorGet(idActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);


    if (tACD.AttributesGetInt(ATTRIBUTE_STUNNED) <= 0) {

        BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
        BrainAPIDoIdle(idActor);
    }


}
#line 2442 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentForceGrippedUpdate(
    ActorID idActor)
{
    const Actor * ptActor = ActorGet(idActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);


    if (tACD.AttributesGetInt(ATTRIBUTE_FORCE_GRIPPED) <= 0) {

        BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
        BrainAPIDoIdle(idActor);
    }

}
#line 5696 "X/Server/AI/BrainComponents.cpp"

BOOL BrainTimerTestAndUpdate(uint32 dwTimeout, uint32 dwTimeoutDelta, uint32 & dwNextTime); // BrainStance.cpp:212
void BrainStanceSetWalking(ActorID idActor); // BrainStance.cpp:3265
#line 687 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentJobUpdate(
    ActorID idActor)
{
    const Actor * ptActor = ActorGet(idActor);
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentJob & tJob = ptComp->tJob;

    WorldPlace tPlace = ACDGet(ptActor->idACD).GetPlace();
    WorldDistance wdSeparation = WorldPlaceDistance(tPlace, tJob.placeJobCenter);

    if (wdSeparation > 0.5f) {

        sBrainComponentSetStatus(ptActor->id, "GotoJob");

        BrainStanceDoTranslate(
            idActor,
            tJob.placeJobCenter,
            0.0f, 0.0f,
            TRANSLATE_TYPE_CHEAP,
            TAG_ACTOR_ANIMATION_WALK,
            0.0f,
            "Job", ACDID_INVALID, SNO_NONE);


        BrainStanceSetIdleAnimation(ptActor->id, TAG_ACTOR_ANIMATION_UNALERT_NEUTRAL);

        return;
    }

    sBrainComponentSetStatus(ptActor->id, "Job");

    BrainStanceSetIdleAnimation(ptActor->id, TAG_ACTOR_ANIMATION_JOB);




}
#line 1965 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentBlindUpdate(
    ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentBlind & tBlind = ptComp->tBlind;

    if (BrainConditionIsReady(idActor))

        return;



    uint32 dwNow = GameGetServerTime();

    if (tBlind.dwEndTime < dwNow) {

        BrainAPIUndoBlind(idActor);
        return;
    }


    sBrainComponentSetStatus(idActor, "Blind");

    BOOL bTimeToWander = BrainTimerTestAndUpdate(tBlind.dwTimerMin, tBlind.dwTimerDelta, tBlind.dwTimeNextWander);
    if (!bTimeToWander)

        return;


    Actor * ptActor = ActorTryToGet(idActor);
    ActorCommonData * ptACD = ptActor ? ACDTryToGet(ptActor->idACD) : NULL;
    if (!ptACD)

        return;

    WorldPlace placeDest = ptACD->GetPlace();
    placeDest.wp.x += RandRangeFloat_Internal(SGameGlobalsGet().rng, -tBlind.flWanderRadiusMin, tBlind.flWanderRadiusDelta);
    placeDest.wp.y += RandRangeFloat_Internal(SGameGlobalsGet().rng, -tBlind.flWanderRadiusMin, tBlind.flWanderRadiusDelta);

    BrainStanceDoTranslate(
        idActor,
        placeDest,
        0.0f, 0.0f,
        TRANSLATE_TYPE_CHEAP,
        TAG_ACTOR_ANIMATION_WALK,
        1.5f,
        "Blind", ACDID_INVALID, SNO_NONE);


    BrainStanceSetWalking(idActor);



}
void AIQueueClearList(ActorID idActor);  // X/Server/LuaScript/ScriptUtil.cpp:92
void ExperienceGainForKill(ACDID idACDVictim, ACDID idACDKiller, BOOL bGainXP, BOOL bDropLoot, uint32 dwGameEventNumber);  // X/Server/Experience.cpp:952
void SGameSendDungeonFinderProgressToPlayer(Player & tPlayer);  // X/Server/SWorld.cpp:722
void PowerAPI_BuffRemoveOnDeath(ActorCommonData & tACD, ACDID idACDKiller);  // X/Server/Powers/PowerAPI.cpp:5894
void SPlayerAddNephalemBuffStack(ACDID idACDPlayer);  // X/Server/SGizmo.cpp:61
void PVPOnDeathPvPArena(ACDID idACDKiller, ACDID idACDVictim, SNO snoKillingPower);  // X/Server/PVP.cpp:1091
void ServerMessageSendEliteKilledMsg(const Player & tPlayer, const ActorCommonData & tACDMonster);  // X/Server/Network/ServerMessageSend.cpp:1676
void SPlayerEvaluateMultiplayerBuff();  // X/Server/SHero.cpp:124
void LootDropLootRunProgressGlobes(const ACDID idACDLootee, const ACDID idACDKiller, const BOOL bIsTiered, const int32 nNumToDropOverride);  // X/Server/Loot/Loot.cpp:1333
BOOL ACDRollDropHealth(const ActorCommonData & tACDVictim, const ACDID idACDKiller, const BOOL fSpender);  // X/Common/ActorCommonData.cpp:9636
void LootDropNegativeHealthGlobe(const ACDID idACDTarget, const ACDID idACDKiller);  // X/Server/Loot/Loot.cpp:1218
void ACDHandleNecromancerCorpseSpawn(const ActorCommonData & tACDVictim);  // X/Common/ActorCommonData.cpp:6127
SpellEffect ActorPowersGetSpellEffect(ActorID idActor);  // X/Server/Actors/Projectile.cpp:64
BOOL PowerAtDestination(const PowerID idPower, const ACDID idACDProjectile, const ACDID idACDIntersected, const SpellEffect eSpellEffect);  // X/Server/Powers/Powers.cpp:3755
void PlayerRemovePet(ACDID idACDPlayer, ACDID idACDPet, BOOL bDisplayChatMessage);  // X/Common/Player.cpp (_Z15PlayerRemovePet5ACDIDS_i)
ACDID PowerStartOnProxy(ACDID idACDSource, SNO snoProxyActor, SNO snoPower, WorldPlace tPlace, Quaternion qOrientation, Target tTarget, uint32 dwDelay, BOOL bCopyAttributesFromSource);  // X/Server/Powers/Powers.cpp:5718
BOOL PowerCheckForSpecialSkillDeath(const SNO snoPower, const RuneType eRune);  // X/Server/Powers/Powers.cpp:5913
SpecialDeathType PowerGetSpecialDeathType(AutoPowerDefinitionR pdefPower, RuneType eRune);  // X/Common/Powers/PowersCommon.cpp:498
enum { GB_MONSTER_AFFIXES = 18 };  // X/Common/Definitions/GameBalanceDefinition.h:66 (kept local, as GB_INVALID above)
#line 6254 "X/Server/AI/BrainComponents.cpp"

static ActorTag sGetProxyActorTagForOnDeathPower(
    SNO snoPower)
{
    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_ON_DEATH_FIRE))
    {
        return TAG_ACTOR_ON_DEATH_FIRE_PROXY_ACTOR;
    }
    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_ON_DEATH_LIGHTNING))
    {
        return TAG_ACTOR_ON_DEATH_LIGHTNING_PROXY_ACTOR;
    }
    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_ON_DEATH_COLD))
    {
        return TAG_ACTOR_ON_DEATH_COLD_PROXY_ACTOR;
    }
    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_ON_DEATH_ARCANE))
    {
        return TAG_ACTOR_ON_DEATH_ARCANE_PROXY_ACTOR;
    }
    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_ON_DEATH_HOLY))
    {
        return TAG_ACTOR_ON_DEATH_HOLY_PROXY_ACTOR;
    }
    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_ON_DEATH_POISON))
    {
        return TAG_ACTOR_ON_DEATH_POISON_PROXY_ACTOR;
    }

    // not an on-death power (the caller's test of this value is folded into the b.ne at 6278)
    return TAG_ACTOR_INVALID;
}



static void sGetCritDeathReplacementEffect(
    const float * pflKillingBlowDamage,
    EffectType & eDeathReplacementEffect,
    int32 & nParam)
{
	// nParam is 0 for every crit death (w24 = 0 is a line-0 row in each case)
	nParam = 0;

	DamageType ePredominantDamage = GetPredominantDamageType(pflKillingBlowDamage, FALSE);
	switch (ePredominantDamage)
	{
	case DAMAGE_TYPE_FIRE:
		eDeathReplacementEffect = EFFECT_CRIT_DEATH_FIRE;
		break;
	case DAMAGE_TYPE_LIGHTNING:
		eDeathReplacementEffect = EFFECT_CRIT_DEATH_LIGHTNING;
		break;
	case DAMAGE_TYPE_COLD:
		eDeathReplacementEffect = EFFECT_CRIT_DEATH_COLD;
		break;
	case DAMAGE_TYPE_POISON:
		eDeathReplacementEffect = EFFECT_CRIT_DEATH_POISON;
		break;
	case DAMAGE_TYPE_ARCANE:
		eDeathReplacementEffect = EFFECT_CRIT_DEATH_ARCANE;
		break;
	case DAMAGE_TYPE_HOLY:
		eDeathReplacementEffect = EFFECT_CRIT_DEATH_HOLY;
		break;
	default:
		eDeathReplacementEffect = EFFECT_CRIT_DEATH_PHYSICAL;
		break;
	}
}







void BrainStanceDoDeath(ActorID idActor, const PowerAPIDamage * ptDamage, const float * pflKillingBlowDamage, const BOOL fDeathReplacementEffect, const BOOL fUseSpecialSkillDeath, EffectType & eDeathEffect);  // X/Server/AI/BrainStance.cpp:2818
void ACDCollDisposeSphereNode(ActorCommonData & tACD);  // X/Server/Powers/SpellEffect.cpp:14948

static void sNotifyAboutAllyDeath(
    ActorID idActor)
{
    ActorAI *ptActorAI = ActorTryToGetAI(idActor);
    if (idActor != ACTORID_INVALID && ptActorAI)
    {
        ACDID idACD = ACDTryFindByActor(idActor);
        ActorCommonData *ptACD = ACDTryToGet(idACD);
        if (ptACD &&
            ptACD->IsAlive())
        {
            ptActorAI->dwLastAllyDeathTime = GameGetServerTime();

            BrainAPITryToFlee(idACD);
        }
    }
}



static void sBrainAPINotifyNearbyAlliesAboutDeath(
    ActorID idActorDying)
{
    ACDID idACD = ACDFindByActor(idActorDying);

    BrainComponentData * ptComp = sGetBrainComponentData(idActorDying);
    if (!ptComp || idACD == ACDID_INVALID)
    {
        return;
    }

    const BrainComponentTarget & tTarget = ptComp->tTarget;
    ActorCommonData & tACD = ACDGet(idACD);

    XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
    GetACDsInRadius(tACD, tTarget.tAwareness.flWarnOthersRadius, listACDsInRadius);
    POS pos = listACDsInRadius.GetHeadPosition();
    while (pos)
    {
        ACDID idACDAlly = (ACDID)listACDsInRadius.GetNext(pos);

        ActorCommonData & tACDAlly = ACDGet(idACDAlly);

        if (!ACDIsFriendly(tACD, tACDAlly, FALSE) ||
            !tACDAlly.IsAlive())
        {
            continue;
        }

        if (tACDAlly.IsWreckableObject())
        {
            continue;
        }

        if (!tACDAlly.IsMonster())
        {
            continue;
        }

        ActorID idActorAlly = (ActorID)ACDGetOwner(idACDAlly);
        sNotifyAboutAllyDeath(idActorAlly);
    }

    // and the rest of the pack, wherever they are
	ACDPack *ptACDPack = ACDPackTryToGet(tACD.idACDPack);
	if (ptACDPack)
	{
        const ACDIDListPooled *listPack = ptACDPack->GetMemberList();
        POS posPack = listPack->GetHeadPosition();
        while (posPack)
        {
            ACDID idACDAlly = (ACDID)listPack->GetNext(posPack);

            ActorID idActorAlly = (ActorID)ACDGetOwner(idACDAlly);
            sNotifyAboutAllyDeath(idActorAlly);
        }
	}
}



static SNO sGetDeathReplacementEffect(
    ActorCommonData * ptACD,
    SNO snoActor,
    DeathReplacementType eDeathReplacementType)
{
    SNO snoEffectGroup = SNO_NONE;

    ScopedActorDefinitionLockR pdefActor(snoActor);
    switch (eDeathReplacementType)
    {
        case DEATH_REPLACEMENT_BLEED:
            snoEffectGroup = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_EFFECT_DEATH_BLEED);
            break;
        case DEATH_REPLACEMENT_FATALITY:
            snoEffectGroup = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_EFFECT_DEATH_FATALITY);
            break;
        case DEATH_REPLACEMENT_EXPLODING_PALM:
            snoEffectGroup = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_EFFECT_DEATH_EXPLODING_PALM);
            break;
        // the rest have no effect group: they become a death-type override (add w2, w19, #6 = DEATH_LAVA - DEATH_REPLACEMENT_LAVA)
        case DEATH_REPLACEMENT_LAVA:
        case DEATH_REPLACEMENT_DISINTEGRATE:
        case DEATH_REPLACEMENT_PULVERIZE:
        case DEATH_REPLACEMENT_PLAGUED:
        case DEATH_REPLACEMENT_DISMEMBER:
        case DEATH_REPLACEMENT_DECAPITATION:
        case DEATH_REPLACEMENT_ACID:
        case DEATH_REPLACEMENT_SACRIFICE:
        case DEATH_REPLACEMENT_SPIRIT:
            {
                DeathType eDeathType = (DeathType)((eDeathReplacementType - DEATH_REPLACEMENT_LAVA) + DEATH_LAVA);
                ptACD->AttributesSetInt(ATTRIBUTE_DEATH_TYPE_OVERRIDE, eDeathType);
            }
            break;
        default:
            break;
    }

    return snoEffectGroup;
}



void sNPCClearConversationIcons(
    ActorCommonData & tACDVictim)
{
    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        tACDVictim.AttributesSetInt(MakeAttribKey(ATTRIBUTE_CONVERSATION_ICON, i), 0);
    }
}




extern "C" BOOL sub_71004b8d40(const ActorCommonData & tACD);  // 2.7.7 only, unnamed (0x71004b8d40, dark alchemy range): checks a buff (SNO at 0x710190121c) and ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES bit 1, then the players local to tACD; TRUE keeps rest XP on death
void BrainAPIDoDeath(
    ActorID idDeadActor,
    ActorID idKillerActor,
    const BOOL fNoExp,
    const BOOL fNoLoot,
    const BOOL fNoHealthDrop,
    PowerAPIDamage * ptDamage,
    float * pflDamageDone)
{
    Actor * ptDeadActor = ActorGet(idDeadActor);

    const Actor * ptKillerActor = ActorTryToGet(idKillerActor);





    uint32 dwGameEventNumber = SGameGetEventNumber();

    ACDID idACDVictim = ptDeadActor->idACD;
    ActorCommonData & tACDVictim = ACDGet(idACDVictim);
    if (tACDVictim.GetGizmoType() == ACTORGIZMO_REFORMING_DESTROYABLE_OBJECT)
    {
        GizmoOperateReformingDestroyable(tACDVictim);
        return;  // no row of its own: merged into the tail call at 6492
    }


    BOOL fVictimGetsHeadstone = FALSE;  // no row: the FALSE stores are line 0 / 6539 (phi of the paths below)

    Player * ptPlayer = PlayerGetByACD(idACDVictim);
    if (ptPlayer)
    {
        BOOL bHardCoreMode = (ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE)) && !GameTypeIsPvP();
        if (bHardCoreMode && !tACDVictim.IsDueling())
        {

            ptPlayer->tHeroStateData.uPlayerFlags |= (1 << PLAYERFLAG_DECEASED);

            tACDVictim.MarkHeroStatePlayerFlagsChanged();

            SPlayerAddToSaveList(*ptPlayer, 0, PlayerSaveReason_Dead, PLAYER_SAVE_MODE_END_OF_FRAME);

























            BossEncounterManager::PlayerRemoved(ptPlayer, FALSE);
            NephalemRiftManager::PlayerRemoved(ptPlayer, FALSE);
            SetEncounterManager::PlayerRemoved(ptPlayer, FALSE);
            Minimap::OnRemovedPlayer(*ptPlayer);
        }
        else
        {
            // no row: the TRUE store is line 0 (+0x118 orr w8,#1; str [sp,#0x84])
            fVictimGetsHeadstone = TRUE;
        }

        if (GameCommonDataExists() && GameGetActiveGameCommonData().tGameSyncedData.nHunterPlayerIdx >= 0 &&
            ptPlayer->nPlayerIndex == GameGetActiveGameCommonData().tGameSyncedData.nHunterPlayerIdx)
            fVictimGetsHeadstone = FALSE;  // no row: csel at 6547:13
    }



    ACDID idACDKiller = ACDID_INVALID;  // no row: mov w23,#-1 is line 0
    if (ptKillerActor)
    {
        idACDKiller = ptKillerActor->idACD;
    }

    if (!PowerActorCanDie(idDeadActor))
    {
        return;  // no row: cbz to the epilogue at 6560
    }


    if (ActorFlagIsSet(idDeadActor, ACTOR_DYING))
    {
        return;  // no row: cbnz to the epilogue at 6566
    }

    if (tACDVictim.IsNPC())
    {
        sNPCClearConversationIcons(tACDVictim);
    }

    if (GameIsOpenWorld() && ACDIsTrackedInOpenWorld(tACDVictim))
    {

        QuestsAddOpenWorldTrackedSNO(tACDVictim.snoActor);
    }

    ActorAI & tAI = ActorGetAI(idDeadActor);


    if (tAI.pBrainOnDeathCallback)
    {
        if (tAI.pBrainOnDeathCallback(idDeadActor, idKillerActor))
        {
            return;  // no row: cbnz to the epilogue at 6587
        }
    }



    ActorFlagSet(idDeadActor, ACTOR_DYING, TRUE);

    SGameStatsRecordKill(tACDVictim, idACDKiller, ptDamage);

    BOOL bIsTreasureGoblin = (tACDVictim.IsMonster() && tACDVictim.IsMonsterTeam() && MonsterGetMonsterFamily(tACDVictim.tSNOName) == MF_TREASURE_GOBLIN);
    BOOL bGrantBuff = tACDVictim.DoesMonsterGrantEliteBuff();

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();

    ActorCommonData * ptACDKiller = ACDTryToGet(idACDKiller);
    if (ptACDKiller)
    {
        uint32 dwNow = GameGetServerTime();
        ptACDKiller->AttributesSetInt(ATTRIBUTE_LAST_ACD_KILLED_TIME, dwNow);
        const ActorCommonData * ptACDOwner = NULL;
        if (ptACDKiller->IsPet())
        {
            ptACDOwner = ACDTryToGet(ptACDKiller->GetPetOwnerPlayer()->idPlayerACD);
        }
        else
        {
            ptACDOwner = ACDIsPlayer(ptACDKiller->id) ? ptACDKiller : NULL;
        }
        if (ptACDOwner)  // no row: folded into the two assignments above
        {

            for (int32 i = PETTYPE_FIRST; i < NUM_PET_TYPES; i++)
            {
                int32 nPos;
                ACDID idACDPet = PlayerGetFirstPet(ptACDOwner->id, i, nPos);
                while (idACDPet != ACDID_INVALID)
                {
                    ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
                    if (ptACDPet)
                    {
                        ptACDPet->AttributesSetInt(ATTRIBUTE_LAST_ACD_KILLED_TIME, dwNow);
                    }
                    idACDPet = PlayerGetNextPet(ptACDOwner->id, i, nPos);
                }
            }
        }


        if (ptACDOwner && tACDVictim.IsMonster() && !GameTypeIsPvP())
        {
            ScopedMonsterDefinitionLockR pdefMonster(tACDVictim.GetMonsterSNO());
            if (pdefMonster->snoLore != SNO_NONE)
            {
                SPlayerAllPlayersLearnLore(pdefMonster->snoLore);
            }

            if (tACDVictim.eMonsterRarity == MR_CHAMPION || tACDVictim.eMonsterRarity == MR_RARE ||
                tACDVictim.eMonsterRarity == MR_UNIQUE || tACDVictim.eMonsterRarity == MR_BOSS)

            {

                tSGameGlobals.dwLastChampRareUniqueKilled = GameGetServerTime();
            }
        }
        Player * ptPlayer2 = PlayerGetByACD(ptACDKiller->id);
    }














    if (tACDVictim.IsAvenger())
    {
        Avenger::Console::RemoveShadowClones(tACDVictim.ptAvengerData->arVictimIDs);


        Player * ptPlayer2 = PlayerGetByPlayerIndex(tACDVictim.ptAvengerData->nPlayerIndex);
        PlayerAvengerData * ptPlayerAvengerData = GetPlayerAvengerData(ptPlayer2, tACDVictim.ptAvengerData->eAvengerSource);

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer2->idPlayerACD);
        if (ptACDPlayer)
        {
            Avenger::Console::ResetAvengerSpawnTimer(*ptACDPlayer);
        }

        ptPlayerAvengerData->ClearCurrentAvenger();


        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
        AvengerInfoMessage tMessage;
        tMessage.nPlayerIndexOfAvengerTarget = tACDVictim.ptAvengerData->nPlayerIndex;
        Avenger::Console::VictimsCopyArrayToMessage(tACDVictim.ptAvengerData->arVictims, tMessage);
        tMessage.eAvengerSource = tACDVictim.ptAvengerData->eAvengerSource;
        tMessage.nAffixBucket = tACDVictim.ptAvengerData->nAffixBucket;
        tMessage.bRetired = FALSE;
        Blizzard::String::Copy(tMessage.szName, tACDVictim.ptAvengerData->sName.str(), sizeof(tMessage.szName));
        MessageSendToClient(listDestPlayers, SMSG_AVENGER_KILLED, &tMessage, sizeof(tMessage));
    }


    SQuestsOnActorKilled(idACDKiller, tACDVictim);

    if (!fNoExp && !fNoLoot && !fNoHealthDrop && !tACDVictim.IsPet())
    {
        SPlayerIncreaseKillCounter(idACDKiller, idACDVictim, ptDamage);
    }

    if (!tACDVictim.IsDueling() && !sub_71004b8d40(tACDVictim))
    {
        tACDVictim.SetRestExperience(0);
    }

    AIQueueClearList(idDeadActor);


    if (tACDVictim.IsAttached())
    {
        ACDID idACDOwner = tACDVictim.GetAttachedToACD();
        ACDGet(idACDOwner).DetachACD(tACDVictim);
    }

    if (tACDVictim.HasAttachedACD())
    {
        ACDID idACDAttachment = tACDVictim.GetAttachedACD();
        tACDVictim.DetachACD(ACDGet(idACDAttachment));
    }


    sBrainAPINotifyNearbyAlliesAboutDeath(idDeadActor);

    if (tACDVictim.IsGizmo())
    {
        tACDVictim.GizmoOnDeath(idACDKiller);
    }
    else
    {
        BOOL fDropsLoot = (!fNoLoot && sACDDropsLootOnDeath(idACDVictim));
        BOOL fGainXP = !fNoExp;  // no row: cset w2,eq is GVN-merged into the !fNoExp at 6702:9
        if (fGainXP || fDropsLoot)
        {
            ExperienceGainForKill(idACDVictim, idACDKiller, fGainXP, fDropsLoot, dwGameEventNumber);
        }























        if (fGainXP &&
            !tACDVictim.AttributesGetInt(ATTRIBUTE_GRANTS_NO_XP) &&
            !tACDVictim.IsPet() &&
            !tACDVictim.IsPlayerTeam() &&
            WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(tACDVictim.GetSWorldID())))
        {
            ScopedGlobalDefinitionLockR pdefGlobals;
            float flMaxXPPercent = pdefGlobals->flLootRunXPPercentToComplete;
            if (ACDIsInTieredLootRun(tACDVictim))
            {
                flMaxXPPercent = pdefGlobals->flTieredLootRunXPPercentToComplete;
            }

            const uint32 nLevels = 3;  // no row: folded into fmov s1,#3.0 at 6799 and cmp #3 at 6820


            ScopedMonsterDefinitionLockR pdefMonster(tACDVictim.GetMonsterSNO());
            float flProgressIncrement = pdefMonster->pctAttributeModifiers[GBA_EXPERIENCE];
















            uint32 nCurrentLevel = Math::FloatToInt(tSGameGlobals.tDungeonFinderData.flDungeonFinderProgress * nLevels / flMaxXPPercent);
            nCurrentLevel = Math::Min(nCurrentLevel, nLevels);

            tSGameGlobals.tDungeonFinderData.flDungeonFinderProgress += flProgressIncrement;

            uint32 nNewLevel = Math::FloatToInt(tSGameGlobals.tDungeonFinderData.flDungeonFinderProgress * nLevels / flMaxXPPercent);
            nNewLevel = Math::Min(nNewLevel, nLevels);

            PlayerList listPlayers(PlayerListGetElementPool());
            PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
            POS pos = listPlayers.GetHeadPosition();
            while (pos)
            {
                Player * ptPlayer2 = listPlayers.GetNext(pos);
                SGameSendDungeonFinderProgressToPlayer(*ptPlayer2);
            }

            if (nNewLevel != nCurrentLevel)
            {
                ScriptEventOnUserEvent("LootRun_BonusBarBuff", idACDKiller, nNewLevel);
                // Inference (6820): `==` on the clamped level; the original compares the unclamped FloatToInt result (cmp w23,#3; b.lo), which `>=` does not give
                if (nNewLevel == nLevels)
                {
                    ScriptEventOnUserEvent("LRALLMONSTERSKILLED", tACDVictim.id, 0);
                    ACDID idACDBoss = SWorldDungeonFinderSpawnBoss(tACDVictim.GetPlace());

                    ActorCommonData * ptACDBoss = ACDTryToGet(idACDBoss);
                    if (ptACDBoss)
                    {
                        ptACDBoss->AttributesSetInt(ATTRIBUTE_IS_LOOT_RUN_BOSS, TRUE);
                        ptACDBoss->AttributesSetInt(ATTRIBUTE_NEVER_DEACTIVATES, TRUE);

                        tSGameGlobals.tDungeonFinderData.snoBossSpawned = ptACDBoss->snoActor;
                    }
                }
            }
        }
    }

    RuneType ePowerDmgSourceRune = RUNE_TYPE_INVALID;
    SNO snoPowerDmgSource = PowerAPI_GetDamageSourcePowerSNO(ptDamage, &ePowerDmgSourceRune);


    PowerAPI_BuffRemoveOnDeath(tACDVictim, idACDKiller);


    if (tACDVictim.GetHitPointsCurrent() > 0.0f)
    {
        tACDVictim.SetHitPoints(0.0f, TRUE, FALSE);
    }
    ActorClearStaggeredDamage(ptDeadActor->id);

    SNO snoResurrectPower = (SNO)tACDVictim.AttributesGetInt(ATTRIBUTE_RESURRECTION_POWER);
    if (snoResurrectPower != SNO_NONE)
    {
        ActorFlagSet(idDeadActor, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
    }

    sBrainTraceDeathStat(idACDVictim, idACDKiller, dwGameEventNumber);

    if (bGrantBuff)
    {
        PlayerList listPlayersToEarnXP(PlayerListGetElementPool());
        ExperienceGetPlayersEligibleForKill(idACDVictim, listPlayersToEarnXP);

        POS pos = listPlayersToEarnXP.GetHeadPosition();
        while (pos)
        {
            Player * ptPlayer2 = listPlayersToEarnXP.GetNext(pos);
            SPlayerAddNephalemBuffStack(ptPlayer2->idPlayerACD);
        }
    }

    if (GameTypeIsPvP())
    {
        PVPOnDeathPvPArena(idACDKiller, idACDVictim, snoPowerDmgSource);
    }
    else if (tACDVictim.IsDueling() && ACDIsPlayer(idACDVictim))
    {
        Player * ptPlayerKiller = ACDGetControllingPlayer(idACDKiller);
        if (ptPlayerKiller)
        {

            Target tTarget;
            tTarget.InitializeFromPlace(tACDVictim.GetPlace());
            PowerStart(ptPlayerKiller->idPlayerACD, GlobalSNOGet(GLOBALSNO_POWERS_BANNER_DROP_PVP), tTarget);
        }
    }
    else if (bGrantBuff || bIsTreasureGoblin)
    {
        Player * ptPlayerKiller = ACDGetControllingPlayer(idACDKiller);
        if (ptPlayerKiller)
        {
            WorldPlace placeVictim = tACDVictim.GetPlace();
            Actor * ptActorKiller = ActorTryToGet(idKillerActor);
            if (ptActorKiller && ptActorKiller->ptSActorMovement)
            {
                ptActorKiller->ptSActorMovement->SnapPlaceToGround(placeVictim);
            }

            Target tTarget;
            tTarget.InitializeFromPlace(placeVictim);

            ServerMessageSendEliteKilledMsg(*ptPlayerKiller, tACDVictim);
        }
    }

    if (ptPlayer)
    {
        if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && ACDIsParticipatingInTieredLootRun(tACDVictim))
        {
            int32 nCurrentDeaths = tACDVictim.AttributesGetInt(ATTRIBUTE_TIERED_LOOT_RUN_DEATH_COUNT);


            ScopedGlobalDefinitionLockR pdefGlobals;

            float flDeathDelay = Math::Min(pdefGlobals->flTieredLootRunDelayDeathMax,
                                           pdefGlobals->flTieredLootRunDelayPerDeath * nCurrentDeaths);

            uint32 dwAllowedResTime = GameGetServerTime() +
                Math::FloatToInt(flDeathDelay * TICKS_PER_SECOND) +
                Math::FloatToInt(pdefGlobals->ptServerData->flPlayerBaseResurrectionDelay * TICKS_PER_SECOND);
            tACDVictim.AttributesSetInt(ATTRIBUTE_TIERED_LOOT_RUN_CORPSE_RESURRECTION_ALLOWED_GAMETIME, dwAllowedResTime);

            nCurrentDeaths++;
            tACDVictim.AttributesSetInt(ATTRIBUTE_TIERED_LOOT_RUN_DEATH_COUNT, nCurrentDeaths);

            NephalemRiftManager::GreaterRiftPlayerState * ptGreaterRiftPlayerState = NephalemRiftManager::GetGreaterRiftPlayerState(ptPlayer);
            if (ptGreaterRiftPlayerState)
            {
                ptGreaterRiftPlayerState->nDeathCount = nCurrentDeaths;
            }
        }

        ACDID idACDKillerToReport = idACDKiller;
        SNO snoPowerDmgSourceToReport = snoPowerDmgSource;

        if (GameTypeIsPvP() && ptDamage)
        {

            Power * ptPower = PowerTryToGet(ptDamage->idPower);
            if (ptPower)
            {

                if (TeamsAreFriendly(ptPlayer->eTeam, ptPower->eTeam) &&
                    ptPower->idACDLastReflectedBy != ACDID_INVALID &&
                    ptPower->snoPowerLastReflectedBy != SNO_NONE)
                {
                    Player * ptPlayerToReport = ACDGetControllingPlayer(ptPower->idACDLastReflectedBy);
                    if (ptPlayerToReport)
                    {
                        idACDKillerToReport = ptPlayerToReport->idPlayerACD;
                        snoPowerDmgSourceToReport = ptPower->snoPowerLastReflectedBy;
                    }
                }
            }
        }

        sBrainAPIDeathPlayerLogic(idACDVictim, idACDKillerToReport, snoPowerDmgSourceToReport);

        if (PlayerIsHardcoreAndDead(*ptPlayer))
        {
            SPlayerEvaluateMultiplayerBuff();
        }
    }


    if ((tACDVictim.eMonsterRarity == MR_RARE || tACDVictim.eMonsterRarity == MR_CHAMPION) &&
        !tACDVictim.AttributesGetInt(ATTRIBUTE_ILLUSION))
    {
        BOOL bIsLastInPack = FALSE;  // no row: folded into the branches at 6970/6984
        if (tACDVictim.eMonsterRarity == MR_RARE)
        {
            bIsLastInPack = TRUE;  // no row: b.ne at 6970 falls to 6986
        }
        else
        {
            ACDPack * ptACDPack = ACDPackTryToGet(tACDVictim.idACDPack);
            if (ptACDPack)
            {
                int32 nACDsLeft = ptACDPack->GetTotalACDCount(FALSE);
                bIsLastInPack = (nACDsLeft == 0);  // no row: compare is the cbnz at 6984
            }
        }

        if (bIsLastInPack)
        {
            if (ACDIsInLootRun(tACDVictim))
            {
                LootDropLootRunProgressGlobes(tACDVictim.id, idACDKiller, ACDIsInTieredLootRun(tACDVictim), 0);
            }
        }
    }

    if (ACDIsInTieredLootRun(tACDVictim) &&
        ACDIsPlayer(tACDVictim.id))
    {

        ++tSGameGlobals.tDungeonFinderData.nDeaths;


        if (GameCommonDataExists() && GameGetActiveGameCommonData().tGameSyncedData.nHunterPlayerIdx >= 0)
        {
            Player * ptPlayer2 = PlayerGetByACD(tACDVictim.id);
            if (ptPlayer2)
            {
                if (ptPlayer2->nPlayerIndex == GameGetActiveGameCommonData().tGameSyncedData.nHunterPlayerIdx)
                {
                    LootDropLootRunProgressGlobes(tACDVictim.id, idACDKiller, TRUE, 8);
                }
                else
                {

                    ScopedGlobalDefinitionLockR pdefGlobals;
                    float flMaxProgress = pdefGlobals->flTieredLootRunXPPercentToComplete;
                    float flPenalty = 0.035f * flMaxProgress;
                    tSGameGlobals.tDungeonFinderData.flDungeonFinderProgress -= flPenalty;
                    tSGameGlobals.tDungeonFinderData.flDungeonFinderProgress = Math::Max(0.0f, tSGameGlobals.tDungeonFinderData.flDungeonFinderProgress);

                    PlayerList listPlayers(PlayerListGetElementPool());
                    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
                    POS pos = listPlayers.GetHeadPosition();
                    while (pos)
                    {
                        Player * ptCurrPlayer = listPlayers.GetNext(pos);
                        SGameSendDungeonFinderProgressToPlayer(*ptCurrPlayer);
                    }
                }
            }
        }
    }

    if (!fNoHealthDrop && !tACDVictim.AttributesGetInt(ATTRIBUTE_NO_HEALTH_DROP))
    {
        HealthDropInfo tHealthDropInfo;
        if (MonsterGetHealthDropInfo(tACDVictim.tSNOName, tACDVictim.eMonsterRarity, tHealthDropInfo))
        {
            BOOL fTryToDropGlobe = TRUE;  // no row: the w19 constants are line 0
            BOOL fPowerSpender = FALSE;  // no row
            if (tHealthDropInfo.eHealthDropStyle == HEALTH_DROP_STYLE_NEVER)
            {
                fTryToDropGlobe = FALSE;  // no row: mov w19,wzr at 0:29
            }
            else if (tHealthDropInfo.eHealthDropStyle == HEALTH_DROP_STYLE_ON_PACK_DEATH)  // no row: cmp #6 at 0:17
            {
                ACDPack * ptACDPack = ACDPackTryToGet(tACDVictim.idACDPack);
                if (ptACDPack)
                {
                    int32 nACDsLeft = ptACDPack->GetTotalACDCount(FALSE);
                    if (nACDsLeft > 0)
                    {
                        fTryToDropGlobe = FALSE;  // no row: cset w19,lt at 7048
                    }
                }
            }





            Power * ptPower = NULL;
            if (ptDamage)
            {
                float flPowerCost = PowerGetFormulaValue(ptDamage->idPower, TAG_POWER_COST_RESOURCE);
                ptPower = PowerTryToGet(ptDamage->idPower);
                fPowerSpender = flPowerCost > EPSILON;
            }



            if (!ptPower || (ptPower->dwFlags & (1 << PF_GLOBE_DROPPED)))
            {
                // Inference (7069-7072): no rows of their own; columns 17/26/29 and the 32-bit dwFlags test fit this form
                fTryToDropGlobe = FALSE;
            }
            if (fTryToDropGlobe && ACDRollDropHealth(tACDVictim, idACDKiller, fPowerSpender))
            {












                ptPower->dwFlags |= (1 << PF_GLOBE_DROPPED);

                LootDropHealth(idACDVictim, dwGameEventNumber, idACDKiller, -1);
            }
            else
            {

                LootDropNegativeHealthGlobe(idACDVictim, idACDKiller);
            }
        }

        ACDHandleNecromancerCorpseSpawn(tACDVictim);
    }


    PowerID idOwnerPower = ActorPowersGetOwnerPower(idDeadActor);
    if (idOwnerPower != POWERID_INVALID)
    {
        SpellEffect eSpellEffect = ActorPowersGetSpellEffect(idDeadActor);

        ActorPowersSetOwnerPower(idDeadActor, POWERID_INVALID);


        PowerAtDestination(idOwnerPower, idACDVictim, ACDID_INVALID, eSpellEffect);
    }


    if (tACDVictim.IsPet() && snoResurrectPower == SNO_NONE)
    {

        ACDID idACDOwner = ACDGetPetOwner(tACDVictim.id);
        if (idACDOwner != ACDID_INVALID)
        {
            if (ACDIsHireling(tACDVictim))
            {

                fVictimGetsHeadstone = TRUE;  // no row: str [sp,#0x84] at 0:18

            }
            else
            {

                PlayerRemovePet(idACDOwner, tACDVictim.id, TRUE);
            }
        }
    }


    BrainAPIActorReleaseTarget(idDeadActor);


    BrainAPIReleaseAllTargets(idDeadActor);

    ScopedActorDefinitionLockR pdefActor(ptDeadActor->snoActor);
    BOOL fRemovesCorpseOnDead = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_REMOVES_CORPSE_ON_DEAD);

    SNO snoOnDeathPower;
    if (tACDVictim.eMonsterRarity == MR_RARE)
    {
        snoOnDeathPower = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_POWER_ON_RARE_DEATH);
    }
    else
    {
        snoOnDeathPower = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_POWER_ON_DEATH);
    }
    if (snoOnDeathPower != SNO_NONE)
    {
        WorldPlace tPlace = tACDVictim.GetPlace();
        const WorldLocation locVictim = tACDVictim.GetWorldLocation();
        tPlace.wp = locVictim.transform * pdefActor->wpLocationPowerSrc;

        Target tTarget;
        tTarget.InitializeFromPlace(tPlace);
        PowerStart(idACDVictim, snoOnDeathPower, tTarget);
    }

    SNO snoProxyPowerOnDeathPower = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_PROXY_POWER_ON_DEATH_POWER);
    SNO snoProxyPowerOnDeathProxyActor = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_PROXY_POWER_ON_DEATH_PROXY_ACTOR);

    if (snoProxyPowerOnDeathPower != SNO_NONE && snoProxyPowerOnDeathProxyActor != SNO_NONE)
    {
        float nSlowTimeScalar = tACDVictim.AttributesGetFloat(ATTRIBUTE_SLOWTIME_DEBUFF);
        if (nSlowTimeScalar <= 0.0f) nSlowTimeScalar = 1.0f;


        int32 nDelay = (int32)(TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_PROXY_POWER_ON_DEATH_DELAY) * TICKS_PER_SECOND / nSlowTimeScalar);
        Target tTarget;
        WorldPlace placeCenter = tACDVictim.GetPlace();
        tTarget.InitializeFromPlace(placeCenter);
        ACDID idACDProxy = PowerStartOnProxy(tACDVictim.id, snoProxyPowerOnDeathProxyActor, snoProxyPowerOnDeathPower, placeCenter, tACDVictim.GetOrientation(), tTarget, nDelay, TRUE);
    }


    SNO snoOnDeathActorSpawn = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_SPAWN_ACTOR_ON_DEATH);
    if (snoOnDeathActorSpawn != SNO_NONE)
    {
        WorldPlace tPlace = tACDVictim.GetPlace();
        const WorldLocation locVictim = tACDVictim.GetWorldLocation();
        tPlace.wp = locVictim.transform * pdefActor->wpLocationPowerSrc;

        Quaternion qOrientation = tACDVictim.GetOrientation();
        ActorAddStruct tActorAdd;
        tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
        tActorAdd.tSNOName.snoHandle = snoOnDeathActorSpawn;
        tActorAdd.snoActor = snoOnDeathActorSpawn;
        tActorAdd.dwActorAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
        tActorAdd.tPlace = tPlace;
        tActorAdd.qOrientation = qOrientation;
        tActorAdd.eTeam = tACDVictim.GetTeam();

        ActorAdd(tActorAdd);
    }

    BOOL fDeathReplacementPower = FALSE;  // no row of its own

    if (tACDVictim.AttributesGetInt(ATTRIBUTE_FORCE_NO_DEATH_ANIMATION))

        fDeathReplacementPower = TRUE;


    if (tACDVictim.eMonsterRarity == MR_RARE)
    {
        for (int i=0; i<8; i++)
        {
            GBID gbidAffix = tACDVictim.pMonsterAffixGBIDs[i];
            if (gbidAffix != GBID_NONE)
            {
                GBHandle hGB; hGB.Set(GB_MONSTER_AFFIXES, gbidAffix);  // Set is inlined (GameBalance.h:65-66 rows), no row of its own on 7215
                if (!GBIDExists(hGB))
                    continue;






                const GameBalanceDefinition * pdefGB;
                const MonsterAffixesEntry * ptAffix = (const MonsterAffixesEntry *)GBLockR(hGB, pdefGB, TRUE);
                if (ptAffix->snoOnDeathPower != SNO_NONE)
                {
                    ActorTag eProxyActorTag = sGetProxyActorTagForOnDeathPower(ptAffix->snoOnDeathPower);
                    if (eProxyActorTag != TAG_ACTOR_INVALID)  // no row: folded into the helper's last compare (6278:21 b.ne)
                    {
                        SNO snoProxyActor = TagMapGetSNO(pdefActor->hTagMap, eProxyActorTag);

                        if (snoProxyActor != SNO_NONE)
                        {
                            // Inference: placement of this assignment (its value reaches [sp,#0x9c] as a phi copy on the 7239 row)
                            fDeathReplacementPower = TRUE;
                            Target tTarget;
                            tTarget.InitializeFromPlace(tACDVictim.GetPlace());
                            PowerStartOnProxy(idACDVictim, snoProxyActor, ptAffix->snoOnDeathPower, tACDVictim.GetPlace(), tACDVictim.GetOrientation(), tTarget, 0, FALSE);
                        }
                    }
                }
                GBUnlockR(pdefGB);
            }
        }
    }




    BOOL nPreventDeathReplacement;
    if (tACDVictim.IsGizmo())
    {
        nPreventDeathReplacement = TRUE;
    }
    else
    {
        nPreventDeathReplacement = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_PREVENT_DEATH_REPLACEMENT);
    }

    BOOL fUseSpecialSkillDeath = PowerCheckForSpecialSkillDeath(snoPowerDmgSource, ePowerDmgSourceRune);
    if (!fDeathReplacementPower && fUseSpecialSkillDeath)
    {
		SpecialDeathType eSpecialDeath = PowerGetSpecialDeathType(snoPowerDmgSource, ePowerDmgSourceRune);
		if (eSpecialDeath != -1)  // SpecialDeathType: -1 is the "none" value (cmn w22,#1)
			nPreventDeathReplacement = TRUE;
    }



    BOOL fDeathReplacementEffect = FALSE;
    EffectType eDeathReplacementEffect = EFFECT_INVALID;
    int32 nDeathReplacementEffectParam = 0;

    if (fDeathReplacementPower)
    {
        fDeathReplacementEffect = TRUE;
    }
    else
    {
        SNO snoDeathReplacementEffect = tACDVictim.AttributesGetInt(ATTRIBUTE_DEATH_REPLACEMENT_EFFECT_GROUP_SNO);
        if (ptDamage && snoDeathReplacementEffect == SNO_NONE)
        {


            snoDeathReplacementEffect = sGetDeathReplacementEffect(&tACDVictim, ptDeadActor->snoActor, ptDamage->eDeathReplacementType);
        }

        if (tACDVictim.eMonsterRarity == MR_RARE)
        {

            fDeathReplacementEffect = !nPreventDeathReplacement;
            eDeathReplacementEffect = EFFECT_DEATH_RARE_MONSTER;
        }
        else if (snoDeathReplacementEffect != SNO_NONE)
        {
            fDeathReplacementEffect = TRUE; eDeathReplacementEffect = EFFECT_PLAY_EFFECT_GROUP;
            nDeathReplacementEffectParam = snoDeathReplacementEffect;
            tACDVictim.AttributesSetInt(ATTRIBUTE_DEATH_REPLACEMENT_EFFECT_GROUP_SNO, snoDeathReplacementEffect);
        }
        else if (ptDamage && ptDamage->bShattered)
        {
            fDeathReplacementEffect = TRUE;
            eDeathReplacementEffect = EFFECT_CRIT_DEATH_COLD;
        }
        else
        {

            BOOL fWasDOT = ptDamage && ptDamage->eDeliveryMechanism == 4;  // PayloadDeliveryMechanism 4 (cmp #4)
            if (!fWasDOT && !nPreventDeathReplacement)
            {
                BOOL fCrit = ptDamage && (ptDamage->bCriticalHit || ptDamage->bForceCriticalDeath);
                if (pflDamageDone && fCrit)
                {
                    fDeathReplacementEffect = TRUE;
                    sGetCritDeathReplacementEffect(pflDamageDone, eDeathReplacementEffect, nDeathReplacementEffectParam);
                }
            }
        }
    }


    sCleanupAttributesOnDeath(idDeadActor);
	if (ptACDKiller)
	{
		sBrainAPIDeathExecuteSlayingPowers(pflDamageDone, ptDamage, *ptACDKiller, tACDVictim);
	}


    sBrainChangeActiveComponent(idDeadActor, ACTIVE_COMPONENT_DEATH);
    sBrainComponentSetStatus(idDeadActor, "Death");
    BrainConditionSetCondition(idDeadActor, BRAIN_CONDITION_ABORTED);

    EffectType eDeathEffect = EFFECT_DEATH;
    BrainStanceDoDeath(idDeadActor, ptDamage, pflDamageDone, fDeathReplacementEffect, fUseSpecialSkillDeath, eDeathEffect);

    if (!fVictimGetsHeadstone)
    {


        BOOL fRagDolled = tACDVictim.AttributesGetInt(ATTRIBUTE_COULD_HAVE_RAGDOLLED);
        if (fRagDolled || fRemovesCorpseOnDead)
        {




            tACDVictim.AttributesSetInt(ATTRIBUTE_DELETED_ON_SERVER, TRUE);
        }
    }




    if (fDeathReplacementEffect && GameGetGameType() == GameTypeIsPvP())  // binary compares GameGetGameType() with the result of a call to GameTypeIsPvP (possibly an ICF-folded function)
    {
        tACDVictim.Hide(TRUE);
    }


    uint32 dwNow = GameGetServerTime();
    tAI.dwTimeOfDeath = dwNow;
    tAI.dwDelayBrainDeadUpdateTime = dwNow;

    BrainComponentData * ptComp = sGetBrainComponentData(idDeadActor);
    BrainComponentDeath & tDeath = ptComp->tDeath;

    tDeath.dwTimeToRemoveBody = sBrainAPIDeathCalculateBodyRemoveTime(idACDVictim, fDeathReplacementEffect);
    // Inference (7370): operand order from the single column-32 row (the `||` after fDeathReplacementPower); the branch order is the compiler's
    if (fDeathReplacementPower || snoResurrectPower != SNO_NONE || fDeathReplacementEffect)
    {
        if (ACDIsPlayer(idACDVictim))
        {

            tACDVictim.AttributesSetInt(ATTRIBUTE_HIDDEN, TRUE);


            if (ptDeadActor->ptSActorMovement)
            {
                ptDeadActor->ptSActorMovement->StopAndSyncPosition(TRUE, FALSE);
            }

            if (!fDeathReplacementPower)
            {
                ACDPlayEffect(idACDVictim, eDeathReplacementEffect, nDeathReplacementEffectParam, ACDID_INVALID);
            }
        }
        else
        {
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

            DeathFadeTimeMessage tMessage;
            tMessage.ann = ANNFromACD(idACDVictim);

            if (snoResurrectPower != SNO_NONE)
            {
                tMessage.nTimeUntilFade = -1;
            }
            else
            {
                tMessage.nTimeUntilFade = 0;
            }

            tMessage.nFadeTime = 0;
            tMessage.bRecurseForAttachments = FALSE;
            MessageSendToClient(listDestPlayers, SMSG_SET_DEATH_FADE_TIMES, &tMessage, sizeof(tMessage));

            if (!fDeathReplacementPower)
            {
                ACDPlayEffect(idACDVictim, eDeathReplacementEffect, nDeathReplacementEffectParam, ACDID_INVALID);
            }
        }
    }
    else
    {
        ACDPlayEffect(idACDVictim, eDeathEffect, 0, ACDID_INVALID);
    }


    ScriptEventOnActorKilled(idACDVictim, idACDKiller);


    ConversationMgr::CancelConversationsForDeadParticipant(idACDVictim);


    tACDVictim.RemoveFromGroups(idACDKiller);

    if (!ACDTryToGet(idACDVictim))
    {
        return;
    }


	tAI.idACDKiller = idACDKiller;


    if (GameIsParamFlagSet(GPF_CREATE_HEADSTONES_UPON_PLAYER_DEATH) && fVictimGetsHeadstone)
    {

        sCreateHeadstone(tACDVictim);
    }



    sCleanDisposableCorpsesAround(idACDVictim);



    if (snoResurrectPower == SNO_NONE)
    {
        ACDCollDisposeSphereNode(tACDVictim);
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetThatKnowACD(listDestPlayers, idACDVictim, FALSE);
        ANNDataMessage tMessage;
        tMessage.ann = ANNFromACD(idACDVictim);
        MessageSendToClient(listDestPlayers, SMSG_REMOVE_FROM_SPHERE_LIST, &tMessage, sizeof(tMessage));
    }
}
#line 2220 "X/Server/AI/BrainComponents.cpp"
static ActorAnimationTag sDetermineRunAwayAnimTag(
    ACDID idACD)
{
    ActorAnimationTag eAnimTag = -1;


    ActorCommonData & tACD = ACDGet(idACD);
    if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_RUNAWAY))

        eAnimTag = TAG_ACTOR_ANIMATION_RUNAWAY;

    else if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_SPRINT))

        eAnimTag = TAG_ACTOR_ANIMATION_SPRINT;

    else if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_RUN))

        eAnimTag = TAG_ACTOR_ANIMATION_RUN;

    else if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_WALK))

        eAnimTag = TAG_ACTOR_ANIMATION_WALK;


    return eAnimTag;

}
#line 2458 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentTauntedUpdate(
    ActorID idActor)
{

    ActorID idTauntTarget = BrainAPIGetTauntTarget(idActor);
    ACDID idACDTauntTarget = ACDTryFindByActor(idTauntTarget);
    ActorCommonData * ptACDTauntTarget = ACDTryToGet(idACDTauntTarget);

    if (ptACDTauntTarget) {




        ActorCommonData & tACD = ACDGet(ACDFindByActor(idActor));
        sCloseOnLocation(tACD, *ptACDTauntTarget);

    }


}
#line 2306 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentRunAwayUpdate(
    ActorID idActor)
{
    const Actor * ptActor = ActorGet(idActor);
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentRunAway & tRunAway = ptComp->tRunAway;


    if (!sBrainComponentChooseTarget(idActor, SNO_NONE, tRunAway.tTarget, tRunAway.tTarget))

        return;


    if (tRunAway.snoPower == GlobalSNOGet(GLOBALSNO_POWERS_WALK))

        return;


    WorldPlace placeDest;
    BrainAPIGetRunAwayPlace(ptActor->idACD, tRunAway.tTarget, placeDest, 25.0f, 25.0f);

    uint32 dwAnimTag = sDetermineRunAwayAnimTag(ptActor->idACD);
    if (dwAnimTag == (uint32)-1)

        return;



    if (tRunAway.eTranslateType == TRANSLATE_TYPE_BACKPEDAL) {

        BrainAPIDoLookAt(idActor, tRunAway.tTarget, TRUE);

        ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);
        if (ptACD &&
            AnimPlayerAnimExists(*ptACD, TAG_ACTOR_ANIMATION_BACKPEDAL))

            dwAnimTag = TAG_ACTOR_ANIMATION_BACKPEDAL;

    }



    BrainComponentDoMovement(
        idActor,
        placeDest,
        0.0f,
        tRunAway.eTranslateType,
        dwAnimTag, 0.0f, "Run Away", ACDID_INVALID, FALSE, SNO_NONE);




}
#line 2360 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentFearedUpdate(
    ActorID idActor)
{
    const Actor * ptActor = ActorGet(idActor);
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentRunAway & tRunAway = ptComp->tRunAway;
    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);


    uint32 dwAnimTag = sDetermineRunAwayAnimTag(ptActor->idACD);






    if (tACD.AttributesGetInt(ATTRIBUTE_FEARED) <= 0 || dwAnimTag == (uint32)-1) {


        BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
        BrainAPIDoIdle(idActor);
        BrainStanceForceUpdate(idActor);
        return;
    }

    WorldPlace tTargetPlace;
    BrainAPIGetRunAwayPlace(idACD, tRunAway.tTarget, tTargetPlace, 25.0f, 25.0f);

    if (ACDIsPlayer(idACD))

        tACD.PickupNearby();



    BrainStanceDoTranslate(
        idActor,
        tTargetPlace,
        0.0f, 0.0f,
        TRANSLATE_TYPE_CLOSING,
        dwAnimTag, 0.0f, "Feared", ACDID_INVALID, SNO_NONE);




}
#line 5944 "X/Server/AI/BrainComponents.cpp"

BOOL PowerHasContactFrame(AutoPowerDefinitionR pdefPower); // PowersCommon.cpp:2055
BOOL PowerNeverUpdatesFacingStarting(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, ACDID idACDTarget); // PowersCommon.cpp:4857
void PowerGetStartRange(ACDID idACD, AutoPowerDefinitionR pdefPower, const Target & tTarget, WorldDistance & wdAttackRange); // PowersCommon.cpp:5379
#line 1268 "X/Server/AI/BrainComponents.cpp"
static EBrainPowerResult sAttackWithoutClosing(
    ActorID idActor,
    const Target & tTarget,
    SNO snoPower,
    BOOL bForceImmediate,
    BrainAttackSettings & tAttackSettings)
{

    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentAttack & tAttack = ptComp->tAttack;

    EBrainPowerResult eReason = sAttackFailureReason(idActor, tTarget, snoPower, TRUE);

    if (eReason == BRAIN_POWER_QUEUE_AND_STAND)

        eReason = BRAIN_POWER_START;


    PowerID idPower = -1;
    switch (eReason) {

    case BRAIN_POWER_START:
    case BRAIN_POWER_RETARGET:
        idPower = BrainStanceDoPower(idActor, tTarget, snoPower, eReason, NULL, NULL, tAttackSettings.nComboLevel, 0);
        break;

    case BRAIN_POWER_GET_CLOSER:
        if (tAttack.tSettings.dwAttackFlags & (1 << ATTACK_EVEN_IF_OUT_OF_RANGE))

            idPower = BrainStanceDoPower(idActor, tTarget, snoPower, BRAIN_POWER_GET_CLOSER, NULL, NULL, 0, 0);
        break;

    case BRAIN_POWER_QUEUE_AND_STAND:
    case BRAIN_POWER_CANT_INTERRUPT:
    case BRAIN_POWER_FAIL:
    case BRAIN_POWER_NO_LINE_OF_SIGHT:
    case BRAIN_POWER_IN_UNCOMPLETED_MOVEMENT:
        break;

    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower) {

        ptPower->uAICoolDownKey = tAttackSettings.uAICoolDownKey;
        ptPower->uAICoolDownTicks = tAttackSettings.uAICoolDownTicks;
        ++tAttack.tSettings.nSuccessfulCasts;
    }

    return eReason;
}

#line 1324 "X/Server/AI/BrainComponents.cpp"
static void sTryActivateClosingCooldown(ACDID idACD, BrainComponentAttack & tAttack)
{
    if (tAttack.bClosingCooldownQueued) {

        tAttack.bClosingCooldownQueued = FALSE;
        uint32 dwNow = GameGetServerTime();
        ActorCommonData & tACD = ACDGet(idACD);
        uint32 dwCoolDownMin = tACD.AttributesGetInt(ATTRIBUTE_CLOSING_COOLDOWN_MIN_TOTAL);
        uint32 dwCoolDownDelta = tACD.AttributesGetInt(ATTRIBUTE_CLOSING_COOLDOWN_DELTA_TOTAL);
        if (dwCoolDownMin + dwCoolDownDelta != 0) {

            uint32 dwCoolDown = RandRange_Internal(SGameGlobalsGet().rng, dwCoolDownMin, dwCoolDownDelta);
            tAttack.dwClosingCooldownTime = dwNow + dwCoolDown;
        }
    }
}

#line 1382 "X/Server/AI/BrainComponents.cpp"
static EBrainPowerResult sAttackCloseAndAttack(
    ActorID idActor,
    const Target & tTarget,
    SNO snoPower,
    BOOL bForceImmediate,
    BrainAttackSettings & tAttackSettings)
{

    if (snoPower == SNO_NONE)

        return BRAIN_POWER_FAIL;


    TranslateType nTranslateType = TRANSLATE_TYPE_CLOSING;

    ACDID idACD = ActorGet(idActor)->idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    BOOL fIsPlayer = ACDIsPlayer(idACD);
    if (fIsPlayer)

        nTranslateType = TRANSLATE_TYPE_PLAYER_CLOSING;


    ActorCommonData * ptACDCastLocation = ACDTryToGet(tAttackSettings.idACDCastLocation);
    if (ptACDCastLocation) {


        if (!sCloseOnLocation(tACD, *ptACDCastLocation))

            return BRAIN_POWER_CLOSING;
    }


    EBrainPowerResult eReason = sAttackFailureReason(idActor, tTarget, snoPower, FALSE);






    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentAttack & tAttack = ptComp->tAttack;


    if (eReason == BRAIN_POWER_GET_CLOSER) {

        sTryActivateClosingCooldown(idACD, tAttack);
        uint32 dwNow = GameGetServerTime();
        if (dwNow < tAttack.dwClosingCooldownTime)

            eReason = BRAIN_POWER_QUEUE_AND_STAND;
    }


    switch (eReason) {

    case BRAIN_POWER_START:
    case BRAIN_POWER_RETARGET: {

        PowerID idPower = BrainStanceDoPower(idActor, tTarget, snoPower, eReason, NULL, NULL, tAttackSettings.nComboLevel, 0);
        Power * ptPower = PowerTryToGet(idPower);
        if (ptPower) {

            ptPower->uAICoolDownKey = tAttackSettings.uAICoolDownKey;
            ptPower->uAICoolDownTicks = tAttackSettings.uAICoolDownTicks;
            ++tAttack.tSettings.nSuccessfulCasts;

            if (!PowerHasContactFrame(snoPower) || !PowerIsPrimary(AutoPowerDefinitionR(snoPower), ptPower->idACD))



                AIStateActivateCooldown(*ptPower);
        }
        return eReason;
    }

    case BRAIN_POWER_QUEUE_AND_STAND:
    case BRAIN_POWER_CANT_INTERRUPT:
    case BRAIN_POWER_FAIL:
    case BRAIN_POWER_IN_UNCOMPLETED_MOVEMENT:
        return eReason;

    case BRAIN_POWER_GET_CLOSER:
    case BRAIN_POWER_NO_LINE_OF_SIGHT:
        break;


    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    WorldDistance wdAttackRange;
    WorldPlace placeAttack;

    if (eReason == BRAIN_POWER_NO_LINE_OF_SIGHT) {


        tTarget.GetPlace(placeAttack);
        wdAttackRange = 0.0f;
    }
    else {

        BOOL bResult = tTarget.GetAttackPosition(placeAttack);
        if (!bResult)

            return BRAIN_POWER_FAIL;


        PowerGetStartRange(idACD, snoPower, tTarget, wdAttackRange);
    }

    uint32 dwAnimationTag = TAG_ACTOR_ANIMATION_NEUTRAL;
    ACDID idACDTarget = ACDID_INVALID;
    if (tTarget.eType == TT_ACTOR)

        idACDTarget = tTarget.idACDTarget;





    BrainComponentDoMovement(
        idActor,
        placeAttack,
        0.0f,
        nTranslateType,
        dwAnimationTag,
        wdAttackRange,
        "Closing", idACDTarget, eReason == BRAIN_POWER_NO_LINE_OF_SIGHT, SNO_NONE);

    return BRAIN_POWER_CLOSING;
}


#line 1688 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentAttackFail(
    ActorID idActor,
    BrainComponentAttack & tAttack,
    const Target & tTarget,
    SNO snoPower)
{



    BrainComponentData * ptComp = sGetBrainComponentData(idActor);

    if (tAttack.tSettings.dwAttackFlags & (1 << ATTACK_ONLY_DO_ONCE)) {

        sAttackStop(idActor, ptComp);
        BrainAPIDoIdle(idActor);
    }
    else {

        sAttackReleaseTargets(idActor, ptComp);
    }

}


#line 1714 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentAttackStarted(
    ActorID idActor,
    BrainComponentAttack & tAttack)
{
    ACDID idACD = ActorGet(idActor)->idACD;
    ActorCommonData & tACD = ACDGet(idACD);



    if (PowerIsOffensive(AutoPowerDefinitionR(tAttack.tSettings.snoPower))) {

        uint32 dwNow = GameGetServerTime();
        uint32 dwAttackCoolDown = tACD.AttributesGetInt(ATTRIBUTE_ATTACK_COOLDOWN_MIN_TOTAL);
        uint32 dwAttackCoolDownDelta = tACD.AttributesGetInt(ATTRIBUTE_ATTACK_COOLDOWN_DELTA_TOTAL);
        uint32 dwCoolDown = RandRange_Internal(SGameGlobalsGet().rng, dwAttackCoolDown, dwAttackCoolDownDelta);
        PowerSetCoolDown(tACD, dwNow + dwCoolDown);
    }

    BrainConditionSetCondition(idActor, BRAIN_CONDITION_READY);


    sBrainComponentUpdateSkillTimer(tACD.id, tAttack.tSettings.snoPower);


    sBrainComponentMovementInitialize(idActor);
}


#line 1744 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentAttackUpdate(
    ActorID idActor)
{
    const Actor * ptActor = ActorGet(idActor);
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentAttack & tAttack = ptComp->tAttack;
    BrainAttackSettings & tSettings = tAttack.tSettings;
    ActorCommonData & tACD = ACDGet(ptActor->idACD);

    if (tACD.HasAttachedACD() && AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_GRAB_IDLE))

        BrainStanceSetIdleAnimation(ptActor->id, TAG_ACTOR_ANIMATION_GRAB_IDLE);


    Target tTarget;

    if (!PowerWantsActorTarget(tSettings.snoPower) && tSettings.tTarget.eType == TT_NONE) {


        tTarget.eType = TT_POINT;
        tTarget.tPlace = tACD.GetPlace();
    }
    else {

        BOOL bFoundTarget = sBrainComponentChooseTarget(idActor, tSettings.snoPower, tSettings.tTarget, tTarget);

        if (!bFoundTarget)
        {
            if (tSettings.dwAttackFlags & (1 << ATTACK_ONLY_DO_ONCE))
            {
                BrainAPIDoIdle(idActor);
            }
            return;
        }

    }






    tSettings.nComboLevel = 0;
    BOOL bComboPower = PowerIsComboPower(AutoPowerDefinitionR(tSettings.snoPower));
    if (bComboPower) {

        tSettings.nComboLevel = tACD.GetComboLevel();
        if (tSettings.nComboLevel != 0) {


            ScopedGlobalDefinitionLockR pdefGlobal;
            uint32 dwComboTimeout = Math::FloatToInt(pdefGlobal->flComboClientTimeout * TICKS_PER_SECOND);
            if (tACD.AttributesGetInt(ATTRIBUTE_COMBO_TIME_LAST_MOVE) + dwComboTimeout < GameGetServerTime())

                tSettings.nComboLevel = 0;
        }
    }


    WorldPlace placeTarget;
    tTarget.GetPlace(placeTarget);

    EBrainPowerResult eResult;

    BOOL bForceImmediate = (tSettings.dwAttackFlags & (1 << ATTACK_FORCE_IMMEDIATELY)) != 0;
    if (tSettings.dwAttackFlags & (1 << ATTACK_DONT_CLOSE))

        eResult = sAttackWithoutClosing(idActor, tTarget, tSettings.snoPower, bForceImmediate, tSettings);
    else


        eResult = sAttackCloseAndAttack(idActor, tTarget, tSettings.snoPower, bForceImmediate, tSettings);


    switch (eResult) {

    case BRAIN_POWER_START:


        sBrainComponentSetStatus(idActor, "Attacking");
        tAttack.nAttackState = ATTACKSTATE_ATTACKING;
        sBrainComponentAttackStarted(idActor, tAttack);
        if (tSettings.dwAttackFlags & (1 << ATTACK_ONLY_DO_ONCE))

            BrainAPIDoIdle(idActor);

        tAttack.bAttackQueued = FALSE;
        if (PowerCausesClosingCooldown(tSettings.snoPower))

            tAttack.bClosingCooldownQueued = TRUE;

        if (bComboPower) {

            int32 nNewComboLevel = (tSettings.nComboLevel + 1) % 3;
            tACD.SetComboLevel(nNewComboLevel);
        }
        break;


    case BRAIN_POWER_RETARGET:


        sBrainComponentSetStatus(idActor, "Attacking");
        tAttack.nAttackState = ATTACKSTATE_ATTACKING;
        break;


    case BRAIN_POWER_CLOSING:


        sBrainComponentSetStatus(idActor, "Closing");
        tAttack.nAttackState = ATTACKSTATE_CLOSING;
        tAttack.bNeedWarmUp = TRUE;
        break;


    case BRAIN_POWER_FAIL:


        sBrainComponentAttackFail(idActor, tAttack, tTarget, tSettings.snoPower);
        sBrainComponentSetStatus(idActor, "-");
        tAttack.nAttackState = ATTACKSTATE_IDLE;
        break;


    case BRAIN_POWER_CANT_INTERRUPT:


        break;


    case BRAIN_POWER_IN_UNCOMPLETED_MOVEMENT:



        if (ActorPowersGetPrimary(idActor) == -1)

            ptActor->ptSActorMovement->StopSoon();



        tAttack.bAttackQueued = TRUE;
        break;


    case BRAIN_POWER_NO_LINE_OF_SIGHT:



        tAttack.bAttackQueued = FALSE;

    case BRAIN_POWER_GET_CLOSER: {



        sAttackReleaseTargets(idActor, ptComp);
        tAttack.bNeedWarmUp = TRUE;

        BOOL bDontUpdateFacing = PowerNeverUpdatesFacingStarting(tSettings.snoPower, tACD.id, tSettings.tTarget.GetACD());
        if (!bDontUpdateFacing)
            if (eResult != BRAIN_POWER_NO_LINE_OF_SIGHT && !(tSettings.dwAttackFlags & (1 << ATTACK_DONT_FACE)))





                ptActor->ptSActorMovement->FaceTarget(placeTarget.wp, FALSE);


        sBrainComponentSetStatus(idActor, "TooFar");
        tAttack.nAttackState = ATTACKSTATE_IDLE;

        if ((tSettings.dwAttackFlags & (1 << ATTACK_ONLY_DO_ONCE)) && (tSettings.dwAttackFlags & (1 << ATTACK_DONT_CLOSE)))
        {
            BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
            BrainAPIDoIdle(idActor);
        }
        break;
    }

    case BRAIN_POWER_QUEUE_AND_STAND: {



        BrainStanceDoIdle(idActor);



        BOOL bDontUpdateFacing = PowerNeverUpdatesFacingStarting(tSettings.snoPower, tACD.id, tSettings.tTarget.GetACD());
        if (!bDontUpdateFacing && !(tSettings.dwAttackFlags & (1 << ATTACK_DONT_FACE)))


            ptActor->ptSActorMovement->FaceTarget(placeTarget.wp, FALSE);


        sBrainComponentSetStatus(idActor, "Cooling");
        tAttack.nAttackState = ATTACKSTATE_COOLING;
        break;
    }
    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}

#line 6416 "X/Server/AI/BrainComponents.cpp"

BOOL BrainStanceIsWalking(ActorID idActor); // BrainStance.cpp:2440
#line 2070 "X/Server/AI/BrainComponents.cpp"
static uint32 sBrainComponentGetCircleAnimTag(
    ActorID idActor,
    const BrainComponentCircle & tCircle)
{

    ACDID idACD = ActorGet(idActor)->idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD) {


        if (tCircle.eCircleDirection == CIRCLE_DIRECTION_CW) {

            if (tCircle.bFaceTarget) {

                if (AnimPlayerAnimExists(*ptACD, TAG_ACTOR_ANIMATION_STRAFELEFT))

                    return TAG_ACTOR_ANIMATION_STRAFELEFT;
            }

            else {

                if (AnimPlayerAnimExists(*ptACD, TAG_ACTOR_ANIMATION_CIRCLELEFT))

                    return TAG_ACTOR_ANIMATION_CIRCLELEFT;
            }
        }

        else if (tCircle.eCircleDirection == CIRCLE_DIRECTION_CCW) {

            if (tCircle.bFaceTarget) {

                if (AnimPlayerAnimExists(*ptACD, TAG_ACTOR_ANIMATION_STRAFERIGHT))

                    return TAG_ACTOR_ANIMATION_STRAFERIGHT;
            }

            else {

                if (AnimPlayerAnimExists(*ptACD, TAG_ACTOR_ANIMATION_CIRCLERIGHT))

                    return TAG_ACTOR_ANIMATION_CIRCLERIGHT;
            }
        }



        if (tCircle.bFaceTarget) {

            if (AnimPlayerAnimExists(*ptACD, TAG_ACTOR_ANIMATION_STRAFE))

                return TAG_ACTOR_ANIMATION_STRAFE;
        }

        else {

            if (AnimPlayerAnimExists(*ptACD, TAG_ACTOR_ANIMATION_CIRCLE))

                return TAG_ACTOR_ANIMATION_CIRCLE;
        }
    }

    return TAG_ACTOR_ANIMATION_NEUTRAL;
}


#line 2138 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentCircleUpdate(
    ActorID idActor)
{
    const Actor * ptActor = ActorGet(idActor);
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentCircle & tCircle = ptComp->tCircle;

    if (!sBrainComponentChooseTarget(idActor, SNO_NONE, tCircle.tTarget, tCircle.tTarget))

        return;


    sBrainComponentSetStatus(idActor, "Closing");

    if (tCircle.snoPower == GlobalSNOGet(GLOBALSNO_POWERS_WALK))

        return;



    WorldPlace placeTarget;
    tCircle.tTarget.GetPlace(placeTarget);

    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    WorldPlace tPlace = tACD.GetPlace();


    Angle aRotate = Math::DegToAngle(280.0f);

    Vector3D wvToTarget;
    WorldPlaceGetVectorBetween(tPlace, placeTarget, wvToTarget);



    if (wvToTarget.MagnitudeSquared() < 25.0f) {

        wvToTarget = tACD.GetForward() * 5.0f;
    }
    else {

        if (tCircle.eCircleDirection == CIRCLE_DIRECTION_CW)
        {
            Vector3D::Rotate(cgwvZAxis, FULL_CIRCLE - aRotate, wvToTarget);
        }
        else
        {
            Vector3D::Rotate(cgwvZAxis, aRotate, wvToTarget);
        }
    }

    WorldPlace placeDest;
    WorldPlaceOffset(tPlace, wvToTarget, placeDest);



    sEnsureLOSDestination(ptActor, placeDest, tCircle.tTarget);

    TranslateType eTranslateType = TRANSLATE_TYPE_WALKING;
    uint32 dwAnimTag = sBrainComponentGetCircleAnimTag(idActor, tCircle);

    if (tCircle.bFaceTarget) {

        eTranslateType = TRANSLATE_TYPE_STRAFE;


        BrainAPIDoLookAt(idActor, tCircle.tTarget, TRUE);
    }

    BrainComponentDoMovement(
        idActor, placeDest, 0.0f,
        eTranslateType,
        dwAnimTag, 0.0f, "Circling", ACDID_INVALID, FALSE, SNO_NONE);




}


#line 2479 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentChargeUpdate(
    ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentCharge & tCharge = ptComp->tCharge;

    if (!sBrainComponentChooseTarget(idActor, tCharge.snoPower, tCharge.tTarget, tCharge.tTarget))

        return;



    sBrainComponentSetStatus(idActor, "Charging");

    if (tCharge.snoPower == GlobalSNOGet(GLOBALSNO_POWERS_WALK))

        return;


    ACDID idACD = ActorGet(idActor)->idACD;
    ActorCommonData & tACD = ACDGet(idACD);


    WorldDistance wdAttackRange = 0.0f;
    if (tCharge.snoPower != SNO_NONE)

        PowerGetStartRange(idACD, tCharge.snoPower, tCharge.tTarget, wdAttackRange);


    BOOL fCharge = TRUE;
    WorldPlace placeCharge;
    if (!tCharge.tTarget.IsInvalidForSource(&tACD)) {

        if (wdAttackRange > 0.0f &&
            tCharge.idACDTarget != ACDID_INVALID &&
            ACDInSameWorld(ACDGet(tCharge.idACDTarget), tACD)) {


            WorldDistance wdDist = BrainAPIGetDistanceToTarget(idActor, (ActorID)ACDGetOwner(tCharge.idACDTarget));
            if (wdDist <= wdAttackRange)

                fCharge = FALSE;
        }
    }
    else {

        fCharge = FALSE;
    }




    if (fCharge)

        tCharge.tTarget.GetPlace(placeCharge);

    else


        placeCharge = tACD.GetPlace();



    WorldDistance wdSpeed = tACD.GetWalkingSpeed(TAG_ACTOR_ANIMATION_CHARGING_RUN);


    BrainStanceDoTranslate(
        idActor,
        placeCharge,
        wdSpeed, 0.0f,
        TRANSLATE_TYPE_CHARGE,
        TAG_ACTOR_ANIMATION_CHARGING_RUN,
        wdAttackRange,
        "Charge", ACDID_INVALID, SNO_NONE);
}

#line 2658 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentWalkUpdate(
    ActorID idActor)
{
    if (BrainStanceIsWalking(idActor))

        return;


    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentWalkTo & tWalkTo = ptComp->tWalkTo;

    const Actor * ptActor = ActorGet(idActor);
    ACDID idACD = ptActor->idACD;
    WorldPlace tPlace = ACDGet(idACD).GetPlace();

    if (!WorldPlaceInSameWorld(tWalkTo.placeGotoPoint, tPlace)) {

        BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
        BrainAPIDoIdle(idActor);
        return;
    }

    WorldDistance wdBetweenSquared = WorldPlaceDistanceSquared(tWalkTo.placeGotoPoint, tPlace);

    if (wdBetweenSquared < 1.0f) {

        BrainAPIDoIdle(idActor);
        return;
    }


    if (tWalkTo.nTranslateType == TRANSLATE_TYPE_QUEST) {



        BrainStanceDoTranslate(
            idActor,
            tWalkTo.placeGotoPoint,
            tWalkTo.wdSpeed, 0.0f,
            TRANSLATE_TYPE_QUEST,

            tWalkTo.dwAnimTag, 0.0f, "Quest", ACDID_INVALID, SNO_NONE);
    }
    else {

        BrainAPIDoIdle(idActor);
    }


}


#line 2790 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentsPlayerDeadTimer(
    ActorID idActor)
{
    ACDID idACD = ActorGet(idActor)->idACD;

    Player * ptPlayer = ACDGetControllingPlayer(idACD);
    AppGlobals & tAppGlobals = sgtAppGlobals;

    if (tAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_INSTANT_RESURRECTION_BIT)) {


        BrainAPIDoResurrect(idActor, FALSE);
    }

    else {


        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        if (!tACDPlayer.IsDueling()) {

            int32 nDeathCount = tACDPlayer.AttributesGetInt(ATTRIBUTE_DEATH_COUNT);
            ++nDeathCount;
            tACDPlayer.AttributesSetInt(ATTRIBUTE_DEATH_COUNT, nDeathCount);
        }


        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, idACD);
        PlayerIndexMessage tMessage;
        tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;

        MessageSendToClient(listDestPlayers, SMSG_PLAYER_HAS_DIED, &tMessage, sizeof(tMessage));
    }

}

#line 3142 "X/Server/AI/BrainComponents.cpp"
static void sResurrectACDWithPower(
    ACDID idACD,
    SNO snoResurrectPower)
{
    ActorCommonData & tACD = ACDGet(idACD);
    tACD.ResetLifeStats(FALSE, TRUE, TRUE, TRUE, FALSE);
    ActorID idActor = (ActorID)tACD.idOwner;


    ActorFlagSet(idActor, ACTOR_DYING, FALSE);
    BrainStanceDoPower(idActor, cgTargetNone, snoResurrectPower, BRAIN_POWER_START, NULL, NULL, 0, 0);
    BrainStanceSetIdleAnimation(idActor, TAG_ACTOR_ANIMATION_NEUTRAL);
    ActorFlagSet(idActor, ACTOR_DOESNT_DEACTIVATE_BIT, FALSE);



    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
    ANNDataMessage tMessage;
    tMessage.ann = ANNFromACD(idACD);
    MessageSendToClient(listDestPlayers, SMSG_RESET_DEATH_FADE_TIMES, &tMessage, sizeof(tMessage));

}

#line 6757 "X/Server/AI/BrainComponents.cpp"

#line 747 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentWanderUpdate(
    ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentWander & tWander = ptComp->tWander;

    sBrainComponentSetStatus(idActor, "Wandering");


    if (GameCommonDataExists() && GameGetActiveGameCommonData().tGameSyncedData.nHunterPlayerIdx >= 0)
    {
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (ptPlayer->nPlayerIndex == GameGetActiveGameCommonData().tGameSyncedData.nHunterPlayerIdx)
            {
                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACDPlayer)
                {
                    ACDID idACDMonster = ActorGet(idActor)->idACD;
                    ActorCommonData * ptACDMonster = ACDTryToGet(idACDMonster);
                    if (ptACDMonster && ptACDMonster->IsElite())
                    {
                        WorldPlace placeHunter = ptACDPlayer->GetPlace();
                        WorldPlace placeMonster = ptACDMonster->GetPlace();
                        if (placeHunter.idSWorld == placeMonster.idSWorld)
                        {
                            Vector2D vDiff = (placeHunter.wp - placeMonster.wp).As2D();


                            if (vDiff.MagnitudeSquared() <= 10000.0f)
                            {


                                BrainComponentDoMovement(
                                    idActor,
                                    ptACDPlayer->GetPlace(),
                                    0.7f,
                                    TRANSLATE_TYPE_CLOSING,
                                    TAG_ACTOR_ANIMATION_NEUTRAL,
                                    3.0f,
                                    "ClosingToLocation",
                                    ptACDPlayer->id, TRUE, SNO_NONE);
                                return;
                            }
                        }
                    }
                }
            }
        }
    }













    BOOL bTimeToWander = BrainTimerTestAndUpdate(tWander.dwTimerMin, tWander.dwTimerDelta, tWander.dwTimeNextWander);
    if (!bTimeToWander)
        return;



    SGameGlobals & tSGameGlobals = SGameGlobalsGet();

    Angle aAngle;
    WorldPlace placeWander;
    BOOL bUseStandardRadiusTarget = TRUE;
    Actor * ptActor = ActorGet(idActor);
    WorldPlace placeCurrent = ACDGet(ptActor->idACD).GetPlace();
    if (tWander.aMaxAngle > 0.0f && tWander.flWanderRadiusMax > EPSILON)
    {










        Vector3D vFromCenter = placeCurrent.wp - tWander.placeWanderCenter.wp;

        float flMagSq = vFromCenter.MagnitudeSquared();
        if (flMagSq > EPSILON * EPSILON)
        {

            bUseStandardRadiusTarget = FALSE;

            float flUnitAngleLimit = Math::Min(flMagSq / (tWander.flWanderRadiusMax * tWander.flWanderRadiusMax), 1.0f);

            Angle aAngleLimit = flUnitAngleLimit * PI;


            Angle aTargetAngle = RandRangeFloat_Internal(tSGameGlobals.rng, -0.5f * tWander.aMaxAngle, tWander.aMaxAngle);
            aTargetAngle = Math::NormalizeAngle(ptActor->aYaw + aTargetAngle);



            Angle aAngleTowardsOuterEdge = Math::ATan(vFromCenter.y, vFromCenter.x);
            aAngle = Math::NormalizeAngle(aTargetAngle - aAngleTowardsOuterEdge);
            if (aAngle > PI)
            {
                aAngle -= FULL_CIRCLE;
                aAngle = Math::Min(aAngle, -aAngleLimit);
            }
            else
            {
                aAngle = Math::Max(aAngle, aAngleLimit);
            }
            aAngle = Math::NormalizeAngle(aAngleTowardsOuterEdge + aAngle);


            float flMaxDistance = Math::Min(tWander.flWanderRadiusMax * 2.0f, tWander.flMaxDistance);
            float flDistToTravel = RandRangeFloat_Internal(tSGameGlobals.rng, tWander.flMinDistance, flMaxDistance);
            Vector3D wvDirection;
            wvDirection.x = flDistToTravel * Math::Cos(aAngle);
            wvDirection.y = flDistToTravel * Math::Sin(aAngle);
            wvDirection.z = 0.0f;
            WorldPlaceOffset(placeCurrent, wvDirection, placeWander);
        }
    }



    if (bUseStandardRadiusTarget)
    {
        WorldDistance wdRadius = RandRangeFloat_Internal(tSGameGlobals.rng, tWander.flWanderRadiusMin, tWander.flWanderRadiusMax);
        aAngle = RandAngle(tSGameGlobals.rng);
        Vector3D wvDirection;
        wvDirection.x = wdRadius * Math::Cos(aAngle);
        wvDirection.y = wdRadius * Math::Sin(aAngle);
        wvDirection.z = 0.0f;

        WorldPlaceOffset(tWander.placeWanderCenter, wvDirection, placeWander);
    }



    Vector3D vDiff = placeWander.wp - placeCurrent.wp;
    float flMagSq = vDiff.MagnitudeSquared();
    if (flMagSq < tWander.flMinDistance * tWander.flMinDistance ||
        flMagSq > tWander.flMaxDistance * tWander.flMaxDistance)
    {
        if (flMagSq <= EPSILON * EPSILON)
        {
            vDiff = cgwvXAxis;
        }
        else
        {
            float flMag = vDiff.Normalize();
            flMag = Math::Pin(flMag, tWander.flMinDistance, tWander.flMaxDistance);
            vDiff *= flMag;
        }

        placeWander.wp = placeCurrent.wp + vDiff;
    }

    BrainStanceDoTranslate(
        idActor,
        placeWander,
        0.0f,
        0.0f,
        TRANSLATE_TYPE_CHEAP,
        tWander.dwTagAnimation,
        0.0f, "Wander", ACDID_INVALID, SNO_NONE);


}

#line 926 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentWanderMinionUpdate(
    ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentWander & tWander = ptComp->tWander;

    sBrainComponentSetStatus(idActor, "Wandering (Minion)");

    BOOL bTimeToWander = BrainTimerTestAndUpdate(tWander.dwTimerMin, tWander.dwTimerDelta, tWander.dwTimeNextWander);
    if (!bTimeToWander)
        return;





    Actor * ptActor = ActorGet(idActor);


    ActorCommonData * ptACDActor = ACDTryToGet(ptActor->idACD);
    if (!ptACDActor)
        return;



    ActorCommonData * ptACDOwner = ACDTryToGet(ACDGetPetOwner(ptActor->idACD));
    if (!ptACDOwner)
        return;



    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    WorldPlace placeCurrent = ptACDActor->GetPlace();


    const Angle aAngleAtMinDistance = 30.0f;


    WorldDistance flDist = Math::Sqrt(ACDGetDistanceSquared(*ptACDActor, *ptACDOwner, FALSE));
    flDist = Math::Min(flDist, tWander.flMaxDistance);

    float flPctMaxDistance = flDist / tWander.flMaxDistance;
    Angle aCalcedDeflectionAngle = tWander.aMaxAngle * flPctMaxDistance + aAngleAtMinDistance;


    const float flAngleVariance = 30.0f;


    Angle aBoundedRndDeflectionAngle = aCalcedDeflectionAngle - flAngleVariance + RandFloat_Internal(tSGameGlobals.rng) * flAngleVariance * 2.0f;


    RandRangeFast(tSGameGlobals.rng, 0, 1);




    WorldPlace placeWander;
    Vector3D wvDirection;
    wvDirection.x = flDist * Math::Cos(aBoundedRndDeflectionAngle);
    wvDirection.y = flDist * Math::Sin(aBoundedRndDeflectionAngle);
    wvDirection.z = 0.0f;
    WorldPlaceOffset(placeCurrent, wvDirection, placeWander);



    Vector3D vDiff = placeWander.wp - placeCurrent.wp;
    float flMagSq = vDiff.MagnitudeSquared();
    if (flMagSq < tWander.flMinDistance * tWander.flMinDistance ||
        flMagSq > tWander.flMaxDistance * tWander.flMaxDistance)
    {
        if (flMagSq <= EPSILON * EPSILON)
        {
            vDiff = cgwvXAxis;
        }
        else
        {
            float flMag = vDiff.Normalize();
            flMag = Math::Pin(flMag, tWander.flMinDistance, tWander.flMaxDistance);
            vDiff *= flMag;
        }

        placeWander.wp = placeCurrent.wp + vDiff;
    }

    BrainStanceDoTranslate(
        idActor,
        placeWander,
        0.0f,
        0.0f,
        TRANSLATE_TYPE_CHEAP,
        tWander.dwTagAnimation,
        0.0f, "Wander (Minion)", ACDID_INVALID, SNO_NONE);


}

#line 2828 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentsRemoveBody(
    ActorID idActor)
{
    ACDID idACD = ActorGet(idActor)->idACD;

    if (ACDIsPlayer(idACD))
    {
        sBrainComponentsPlayerDeadTimer(idActor);
    }
    else
    {
        ActorFlagForDeletion(idActor);
    }

}

#line 3168 "X/Server/AI/BrainComponents.cpp"
static void sBrainComponentDeathUpdate(
    ActorID idActor)
{
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);
    BrainComponentDeath & tDeath = ptComp->tDeath;



    if (tDeath.dwTimeToRemoveBody == (uint32)-1)
        return;




    uint32 dwNow = GameGetServerTime();
    if (dwNow >= tDeath.dwTimeToRemoveBody)
    {
        BOOL bRemoveBody = TRUE;

        ACDID idACD = ACDFindByActor(idActor);
        if (idACD != ACDID_INVALID)
        {
            SNO snoResurrectPower = ACDGet(idACD).AttributesGetInt(ATTRIBUTE_RESURRECTION_POWER);
            if (snoResurrectPower != SNO_NONE)
            {
                sResurrectACDWithPower(idACD, snoResurrectPower);
                bRemoveBody = FALSE;
            }
        }

        if (bRemoveBody)
        {

            sBrainComponentsRemoveBody(idActor); }


        tDeath.dwTimeToRemoveBody = (uint32)-1;
    }
}

ActorAI & ActorGetAI(Actor & tActor); // ActorAI.cpp:1324
#line 3247 "X/Server/AI/BrainComponents.cpp"
// NON_MATCHING: the attack and fear blocks are emitted in a different order, with remaining register-allocation and branch differences.
void BrainComponentUpdate(
    Actor & tActor)
{
    ActorAI & tAI = ActorGetAI(tActor);
    ActorID idActor = tActor.id;
    BrainComponentData * ptComp = sGetBrainComponentData(idActor);

    uint32 dwNow = GameGetServerTime();
    const int32 BRAIN_COMPONENT_UPDATE_INTERVAL = 20;
    tAI.dwNextBrainComponentUpdateTime = dwNow + BRAIN_COMPONENT_UPDATE_INTERVAL;

    if (ptComp->nNextComponent != ptComp->nActiveComponent)
    {
        sChangedComponents(idActor, ptComp);
    }

    switch (ptComp->nActiveComponent)
    {
    case ACTIVE_COMPONENT_IDLE:
    case ACTIVE_COMPONENT_ANIMATION:
        break;





    case ACTIVE_COMPONENT_JOB:

        sBrainComponentJobUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_WANDER:

        sBrainComponentWanderUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_WANDER_MINION:

        sBrainComponentWanderMinionUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_ATTACK:

        sBrainComponentAttackUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_CIRCLE:

        sBrainComponentCircleUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_RUNAWAY:

        sBrainComponentRunAwayUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_FEARED:

        sBrainComponentFearedUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_FROZEN:

        sBrainComponentFrozenUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_FORCE_GRIPPED:

        sBrainComponentForceGrippedUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_STUNNED:

        sBrainComponentStunnedUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_TAUNTED:

        sBrainComponentTauntedUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_CHARGE:

        sBrainComponentChargeUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_DEATH:

        sBrainComponentDeathUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_WALK:

        sBrainComponentWalkUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_SPECIAL_POWER:

        sBrainComponentSpecialPowerUpdate(idActor);
        break;



    case ACTIVE_COMPONENT_BLIND:

        sBrainComponentBlindUpdate(idActor);
        break;


    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}

#line 8904 "X/Server/AI/BrainComponents.cpp"
