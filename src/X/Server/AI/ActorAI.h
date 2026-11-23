// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/AI/ActorAI.h
// Subset: the queued-action API the power scripts call, plus the ActorAI fields the champion
// brain needs (BrainInitFunc / pOriginalActionUpdateFunc).  The rest of ActorAI is padding
// (invented names) until someone needs it.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../SGame.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Common/NodePathSwig.h"
#include "../../Common/WorldPlace.h"
#include "../../Common/GameBalance.h"
#include "../../Common/AI/ActorAISwig.h"

struct BrainComponentData;
struct AIStateInstanceData;
typedef int32 BrainCondition;

enum {
    QUEUED_ACTION_INVALID = -1,
    QUEUED_ACTION_POWER = 0,
    QUEUED_ACTION_ANIMATION = 1,
    QUEUED_ACTION_FACE_ACTOR = 2,
    QUEUED_ACTION_FACE_SAME_AS_ACTOR = 3,
    QUEUED_ACTION_DELETE = 4,
    QUEUED_ACTION_KILL = 5,
    QUEUED_ACTION_WALK_FORWARD = 6,
    QUEUED_ACTION_MOVE_TO_LOCATION = 7,
    QUEUED_ACTION_SEND_EVENT = 8,
    QUEUED_ACTION_WAIT_FOR_CONVERSATION = 9,
    QUEUED_ACTION_COUNT = 10,
};

typedef int32 AIQueuedActionType;

struct AffixSelectionEntry {  // size 0x8
    GBID gbidAffix;  // 0x00
    int32 nFrequencyWeight;  // 0x04
};
static_assert(sizeof(AffixSelectionEntry) == 0x8, "AffixSelectionEntry");

typedef XPooledList<AffixSelectionEntry, const AffixSelectionEntry &> AffixSelectionList;

// X/Server/AI/ActorAI.h:83
typedef void (*BrainInitFunc)(ActorID);
typedef BOOL (*BrainUpdateFunc)(ActorID);
typedef BOOL (*BrainOnDeathCallback)(ActorID, ActorID);

struct BrainStanceData;
struct AILureData;
class StateMachineComponent;

enum ActorAIStatusFlag {
    ACTOR_AI_HAS_BRAIN_BIT = 0,
    ACTOR_AI_BROADCAST_BIT = 1,
    ACTOR_AI_DISABLE_UPDATES_BIT = 2,
    ACTOR_AI_FOLLOW_LEADER_BIT = 3,
    ACTOR_AI_NUM_FLAGS = 4,
};

struct AINodePathData {  // size 0xc
    PathID idPath;  // 0x00
    PathNodeID idPathNodeCurr;  // 0x04
    PathNodeID idPathNodeDest;  // 0x08
};
static_assert(sizeof(AINodePathData) == 0xc, "AINodePathData");

typedef XListMemoryPool<AffixSelectionEntry> TAffixSelectionPool;
typedef int32 ActorAnimationTag;
typedef int32 ProximityDetectorDataID;
typedef XPooledDataIDList AIQueuedIDListPooled;
typedef int32 AIQueuedCalloutID;
typedef int32 AIQueuedActionID;

enum {
    AIQID_INVALID = -1,
};
enum {
    AIQCID_INVALID = -1,
};
enum {
    AISAVEID_INVALID = -1,
};
enum {
    TETHER_ALWAYS = 0,
    TETHER_WHEN_IDLE = 1,
};
typedef int32 TetherType;

struct TetherData {  // size 0x1c
    BOOL bRetreating;  // 0x00
    TetherType eTetherType;  // 0x04
    float flRadius;  // 0x08
    WorldPlace placeTetherPoint;  // 0x0c
};
static_assert(sizeof(TetherData) == 0x1c, "TetherData");

struct ThreatListEntry {  // size 0x8
    ACDID idACD;  // 0x00
    float flDamageReceived;  // 0x04
};
static_assert(sizeof(ThreatListEntry) == 0x8, "ThreatListEntry");

typedef XPooledList<ThreatListEntry, ThreatListEntry> TThreatList;

struct ActorAI {  // size 0x218
    ActorBrain nBrainType;  // 0x00
    union {  // size 0x8
        void * pBrainData;  // 0x00
        StateMachineComponent * pSMComponent;  // 0x00
    };  // 0x08
    uint8 pbNewBrainDataBuf[64];  // 0x10
    BrainCondition eBrainCondition;  // 0x50
    uint32 dwConditionTimeout;  // 0x54
    BrainComponentData * ptBrainComponentData;  // 0x58
    BrainStanceData * ptBrainStanceData;  // 0x60
    AIStateInstanceData * ptAIStateInstData;  // 0x68
    AILureData * ptLureData;  // 0x70
    BrainInitFunc pBrainInitFunc;  // 0x78
    BrainInitFunc pBrainDisposeFunc;  // 0x80
    BrainInitFunc pBrainUpdateFunc;  // 0x88
    BrainInitFunc pBrainActionUpdateFunc;  // 0x90
    BrainInitFunc pBrainDeadUpdateFunc;  // 0x98
    BrainOnDeathCallback pBrainOnDeathCallback;  // 0xa0
    BrainInitFunc pOriginalActionUpdateFunc;  // 0xa8 (line 164)
    BrainUpdateFunc pBrainTetherUpdateFunc;  // 0xb0
    LPCSTR sBrainName;  // 0xb8
    LPCSTR sBrainStatus;  // 0xc0
    LPCSTR sBrainComponentStatus;  // 0xc8
    LPCSTR sBrainStance;  // 0xd0
    int32 nUpdateInterval;  // 0xd8
    int32 nRandomNoiseInterval;  // 0xdc
    uint32 dwNextBrainUpdateTime;  // 0xe0
    uint32 * pdwCurrentDelayTimer;  // 0xe8
    uint32 dwDelayBrainUpdateTime;  // 0xf0
    uint32 dwDelayBrainDeadUpdateTime;  // 0xf4
    uint32 dwDelayBrainActionTime;  // 0xf8
    uint32 dwNextBrainComponentUpdateTime;  // 0xfc
    uint32 dwNextBrainStanceUpdateTime;  // 0x100
    uint32 dwNextBroadcastTime;  // 0x104
    uint32 dwNextAllowedWarnNearbyAlliesTime;  // 0x108
    uint32 dwNextAllowedOnAttackedAddTargetTime;  // 0x10c
    uint32 dwTimeOfDeath;  // 0x110
    uint32 dwTimeOfDead;  // 0x114
    uint32 dwPersistentFlags;  // 0x118
    ACDID idACDKiller;  // 0x11c
    ActorID idLastDamageSource;  // 0x120
    uint32 dwLastAttackTime;  // 0x124
    uint32 dwLastDamageTime;  // 0x128
    uint32 dwStatusFlags;  // 0x12c
    uint32 dwLastAllyDeathTime;  // 0x130
    uint32 dwLastAllyWarnTime;  // 0x134
    ActorID idLeader;  // 0x138
    uint32 dwLastLeaderFollowTime;  // 0x13c
    uint32 dwNextFleeTime;  // 0x140
    ACDID idACDTauntTarget;  // 0x144
    ProximityDetectorDataID idProximityData;  // 0x148
    AINodePathData * ptNodePathData;  // 0x150
    GBID gbidMovementStyle;  // 0x158
    AIQueuedIDListPooled listAIQueuedIDs;  // 0x160
    AIQueuedIDListPooled listAIQueuedCalloutIDs;  // 0x188
    UpdateData tAIUpdateData;  // 0x1b0
    TetherData tAITetherData;  // 0x1d0
    BOOL bMaintainThreatList;  // 0x1ec
    TThreatList tThreatList;  // 0x1f0
};
static_assert(sizeof(ActorAI) == 0x218, "ActorAI");

void AIQueueCheckActionCompleted(ActorID idActor, SNO snoPower, AIQueuedActionType eType);  // ActorAI.cpp:2624

void AITrace(int id, const char* szFormat, ...);  // ActorAI.cpp:1796

ActorAI & ActorGetAI(const ActorID idActor);  // ActorAI.cpp (0x7100770120)

void BrainSetStatus(ActorID idActor, LPCSTR sBrainStatus);  // ActorAI.cpp:1391

static_assert(__builtin_offsetof(ActorAI, ptBrainComponentData) == 0x58, "ActorAI::ptBrainComponentData");
static_assert(__builtin_offsetof(ActorAI, pOriginalActionUpdateFunc) == 0xa8, "ActorAI::pOriginalActionUpdateFunc");
static_assert(__builtin_offsetof(ActorAI, dwDelayBrainUpdateTime) == 0xf0, "ActorAI::dwDelayBrainUpdateTime");
static_assert(__builtin_offsetof(ActorAI, tAIUpdateData) == 0x1b0, "ActorAI::tAIUpdateData");
ActorAI * ActorTryToGetAI(const ActorID idActor);  // ActorAI.cpp:1334
BOOL ActorHasBrain(ActorID idActor);  // ActorAI.cpp:881
BOOL ActorHasBeenKilled(const ActorID idActor);  // ActorAI.cpp:1169
BrainCondition BrainConditionGetCondition(ActorID idActor);  // BrainComponents.cpp:663

void BrainConditionSetCondition(ActorID idActor, BrainCondition eBrainCondition);  // BrainComponents.cpp:618

typedef int32 PlayerWarpReason;  // X/Common/World.h
class Target;
class ActorCommonData;
BOOL ACDWarpToTarget(ActorCommonData & tACDToWarp, const Target & tWarpTarget, const PlayerWarpReason eReason, float flMinWarpRadius, BOOL bFailOnBadLocation);  // ActorAI.cpp:1830

BOOL ACDIsSummoned(ACDID idACD);  // ActorAI.cpp
BOOL AIHasNodePath(ActorID idActor);  // ActorAI.cpp
struct WorldPlace;
BOOL AIIsTargetOnPath(ActorID idActor, const WorldPlace & tPlace, float flDist);  // ActorAI.cpp
BOOL AIIsDerailedFromPath(ActorID idActor, float flDist);  // ActorAI.cpp
BOOL AITargetIsAheadOnPath(ActorID idActor, const WorldPlace & tPlace, float flDist);  // ActorAI.cpp
struct Actor;
void ActorAISetStatusFlag(const ActorID idActor, const ActorAIStatusFlag eFlag, const BOOL bValue);  // ActorAI.cpp:2069
BOOL ActorAIIsStatusFlagSet(const ActorID idActor, const ActorAIStatusFlag eFlag);  // ActorAI.cpp:2083
BOOL ActorAIIsStatusFlagSet(Actor & tActor, const ActorAIStatusFlag eFlag);  // ActorAI.cpp:2093
int32 AIQueueGetCount(ActorID idActor);  // ActorAI.cpp:2195
BOOL ActorIsUsingAIQueue(ActorID idActor);  // ActorAI.cpp:2187

BOOL AISetNodePath(ActorID idActor, PathID idPath, PathNodeID idPathNodeDest);

struct ProximityDetectorBrainData;
ProximityDetectorBrainData * ProximityDetectorTryGetData(ProximityDetectorDataID idData);  // ActorAI.cpp:4719
