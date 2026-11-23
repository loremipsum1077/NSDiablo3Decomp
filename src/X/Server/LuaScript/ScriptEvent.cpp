// Reconstructed from original 2.6.2 DWARF and instruction listings.
#include "ScriptEventInternal.h"
#include "../SGameManager.h"
#include "../../Common/Game.h"
#include "../../Common/Definitions/QuestDefinition.h"
#include "../../Common/SNO/SymbolicValue.h"
#include "../SConductor.h"
#include "../Definitions/ScriptDefinition.h"
#include "../Definitions/ConductorDefinition.h"
#include "LuaScriptUtil.h"
#include "../SGameConstants.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../Actors/Actors.h"
#include "../Quests/SQuests.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/MemoryPools.h"
#include "../../../Contrib/Contrib/lua/src/lua.h"
void sScriptEventTriggerForActors(const XPooledDataIDList &, ACDID, ScriptEventType, BOOL &);
#include <cstring>
#include <new>

enum
{
    DEFAULT_SCRIPT_EVENT_ARRAY_SIZE = 32,
    MAX_ACTIVE_LISTENERS = 16,
    MAX_APP_SCRIPT_EVENT_LISTENER_DEFINITIONS = 1024,
    MAX_GAME_SCRIPT_EVENT_LISTENER_DEFINITIONS = 64,
    MAX_LISTENER_INSTANCES = 64,
};

enum ListenerType
{
    LISTENER_TYPE_NONE = 0,
    LISTENER_TYPE_SCRIPT = 1,
    LISTENER_TYPE_SCRIPT_INSTANCE = 2,
};

struct ScriptEventAppGlobals
{                                                          // size 0x278
    ScriptEventDefinition *arScriptEventDefs[61];          // 0x00
    ACDIDParam *ptDefaultACDIDDefParam;                    // 0x1e8
    FloatRangeParam *ptDefaultRangeDefParam;               // 0x1f0
    ConversationParam *ptDefaultConvDefParam;              // 0x1f8
    SceneIDParam *ptDefaultSceneIDDefParam;                // 0x200
    LevelAreaParam *ptDefaultLevelAreaDefParam;            // 0x208
    StringParam *ptDefaultStringDefParam;                  // 0x210
    TimeParam *ptDefaultTimeParam;                         // 0x218
    AnimTagParam *ptDefaultAnimTagParam;                   // 0x220
    QuestNameParam *ptDefaultQuestNameParam;               // 0x228
    TimeIntervalParam *ptDefaultTimeIntervalParam;         // 0x230
    DWORDParam *ptDefaultDwordParam;                       // 0x238
    ActorGroupParam *ptDefaultActorGroupParam;             // 0x240
    FloatRangeInsideParam *ptDefaultFloatRangeInsideParam; // 0x248
    TimedEventParam *ptDefaultTimedEventParam;             // 0x250
    WorldParam *ptDefaultWorldParam;                       // 0x258
    PowerParam *ptDefaultPowerParam;                       // 0x260
    QuestRangeParam *ptDefaultQuestRangeParam;             // 0x268
    BossEncounterParam *ptDefaultBossEncounterParam;       // 0x270
};
static_assert(sizeof(ScriptEventAppGlobals) == 0x278, "ScriptEventAppGlobals");

enum
{
    SCRIPT_EVENT_LISTENER_FLAGGED_FOR_DELETE = 0,
};

struct ScriptEventListenerInstance
{                                                  // size 0x18
    ScriptEventListenerDefinitionID idListenerDef; // 0x00
    ScriptID idScript;                             // 0x04
    ScriptInstanceID idScriptInstance;             // 0x08
    ListenerType eListenerType;                    // 0x0c
    uint32 dwLastDispatchTime;                     // 0x10
    uint32 dwFlags;                                // 0x14
};
static_assert(sizeof(ScriptEventListenerInstance) == 0x18, "ScriptEventListenerInstance");

typedef XPooledList<ScriptEventListenerInstance, ScriptEventListenerInstance &> PooledScriptEventListenerInstanceList;

typedef XListMemoryPool<ScriptEventListenerInstance> ScriptEventListenerInstanceListPool;

enum
{
    SCRIPT_EVENT_GAME_GLOBALS_LISTENER_FLAGGED_FOR_DELETE = 0,
};

struct ScriptEventGameGlobals
{                                                                          // size 0x220
    ScriptEventListenerDefinitionDataArray *arScriptEventListeners;        // 0x00
    ScriptEventListenerDefinitionDataArray *arScriptEventWaitListeners;    // 0x08
    ScriptEventListenerInstanceListPool *poolScriptEventListenerInstances; // 0x10
    PooledScriptEventListenerInstanceList *arActiveListenerLists[61];      // 0x18
    uint32 dwLastHeartbeat;                                                // 0x200
    uint32 dwFlags;                                                        // 0x204
    int32 nTotalEventsDispatched;                                          // 0x208
    int32 nTotalEventsEasyOut;                                             // 0x20c
    int32 nTotalListenersConsidered;                                       // 0x210
    int32 nTotalListenersCalled;                                           // 0x214
    int32 nTotalInstancesNotified;                                         // 0x218
};
static_assert(sizeof(ScriptEventGameGlobals) == 0x220, "ScriptEventGameGlobals");

// Exact values decoded from original main.elf at 0x71010c4390, 61 x 0x88 bytes.
static ScriptEventDescription sgScriptEventDescriptions[NUM_SCRIPT_EVENTS] = {
    {EVENT_ACTOR_KILLED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorKilled", "idActorKiller", NULL, NULL},
     {"idActorKilled", "idActorKiller", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_GROUP_KILLED,
     "",
     "",
     {ACTOR_GROUP_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"szGroup", "idActorKiller", NULL, NULL},
     {"szGroup", "idActorKiller", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_ACTOR_DAMAGED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorDamaged", "idActorDamager", NULL, NULL},
     {"idActorDamaged", "idActorDamager", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_ACTOR_SCORED_CRITICAL_HIT,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorAttacker", "idActorVictim", NULL, NULL},
     {"idActorAttacker", "idActorVictim", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_ACTOR_SIGHTED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActor", "idTarget", NULL, NULL},
     {"idActor", "idTarget", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_ACTOR_SPAWNED_BY_SPAWNER,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorSpawner", "idActorSpawned", NULL, NULL},
     {"idActorSpawner", "idActorSpawned", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_ACTOR_HEALTH_ENTERED_RANGE,
     "",
     "",
     {ACDID_PARAM_TYPE, FLOAT_RANGE_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActor", "flMin, flMax", NULL, NULL},
     {"idActor", "flPrevHP, flHp", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_ACTOR_REACHED_PATH_NODE,
     "",
     "",
     {ACDID_PARAM_TYPE, STRING_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActor", "sPathNodeName", NULL, NULL},
     {"idActor", "sPathNodeName", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_ACTOR_ACTIVATED,
     "",
     "",
     {ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorActivated", NULL, NULL, NULL},
     {"idActorActivated", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_ACTOR_REROLLED_INVENTORY,
     "",
     "",
     {ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActor", NULL, NULL, NULL},
     {"idActor", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_ACTOR_HEALED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, FLOAT_RANGE_INSIDE_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorHealed", "idActorSource", "flAmountHealed", NULL},
     {"idActorHealed", "idActorSource", "flAmountHealed", NULL},
     {"", "", "", NULL}},
    {EVENT_ACTOR_RESURRECTED,
     "",
     "",
     {ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorResurrected", NULL, NULL, NULL},
     {"idActorResurrected", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_ACTOR_GAINED_LEVEL,
     "",
     "",
     {ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorLeveled", NULL, NULL, NULL},
     {"idActorLeveled", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_ACTOR_USED_POWER,
     "",
     "",
     {ACDID_PARAM_TYPE, POWER_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActor", "sPowerName", "idTarget", NULL},
     {"idActor", "sPowerName", "idTarget", NULL},
     {"", "", "", NULL}},
    {EVENT_CONVERSATION_FINISHED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, CONV_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorNPC", "idActorPlayer", "sConversation", NULL},
     {"idActorNPC", "idActorPlayer", "sConversation", NULL},
     {"", "", "", NULL}},
    {EVENT_HEART_BEAT,
     "",
     "",
     {TIME_INTERVAL_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"flInterval", NULL, NULL, NULL},
     {"flGameTime", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_GAME_TIME,
     "",
     "",
     {TIME_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"flGameTime", NULL, NULL, NULL},
     {"flGameTime", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_TIMED_EVENT_TIME_ELAPSED,
     "",
     "",
     {TIMED_EVENT_PARAM_TYPE, TIME_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"sTimedEvent", "fTimeElapsed", NULL, NULL},
     {"sTimedEvent", "fTimeElapsed", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_TIMED_EVENT_TIME_REMAINING,
     "",
     "",
     {TIMED_EVENT_PARAM_TYPE, TIME_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"sTimedEvent", "fTimeRemaining", NULL, NULL},
     {"sTimedEvent", "fTimeRemaining", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_PLAYER_ENTERED_GAME,
     "",
     "",
     {ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorPlayer", NULL, NULL, NULL},
     {"idActorPlayer", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_PLAYER_LEFT_GAME,
     "",
     "",
     {ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorPlayer", NULL, NULL, NULL},
     {"idActorPlayer", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_PLAYER_ENTERED_WORLD,
     "",
     "",
     {ACDID_PARAM_TYPE, WORLD_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorPlayer", "sWorldName", NULL, NULL},
     {"idActorPlayer", "sWorldName", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_PLAYER_ENTERED_LEVEL_AREA,
     "",
     "",
     {ACDID_PARAM_TYPE, LEVELAREA_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorPlayer", "sLevelAreaName", NULL, NULL},
     {"idActorPlayer", "sLevelAreaName", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_PLAYER_PICKED_UP_ITEM,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorPlayer", "idItem", NULL, NULL},
     {"idActorPlayer", "idItem", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_PLAYER_DROPPED_ITEM,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorPlayer", "idItem", NULL, NULL},
     {"idActorPlayer", "idItem", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_PLAYER_KILL_COUNTER,
     "",
     "",
     {ACDID_PARAM_TYPE, DWORD_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorPlayer", "nKillCounter", NULL, NULL},
     {"idActorPlayer", "nKillCounter", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_PLAYER_TOWN_PORTAL,
     "",
     "",
     {ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorPlayer", NULL, NULL, NULL},
     {"idActorPlayer", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_HIRELING_EQUIPPED_ITEM,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorHireling", "idItem", NULL, NULL},
     {"idActorHireling", "idItem", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_LOOT_DROPPED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, DWORD_PARAM_TYPE},
     {"idActorLootee", "idActorLooter", "idActorBestItem", "nNumItems"},
     {"idActorLootee", "idActorLooter", "idActorBestItem", "nNumItems"},
     {"", "", "", ""}},
    {EVENT_CONVERSATION_STARTED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, CONV_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorNPC", "idActorPlayer", "sConversation", NULL},
     {"idActorNPC", "idActorPlayer", "sConversation", NULL},
     {"", "", "", NULL}},
    {EVENT_PLAYER_IN_RANGE_OF_ACTOR,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, FLOAT_RANGE_INSIDE_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorDetector", "idActor", "fRange", NULL},
     {"idActorDetector", "idActor", "fRange", NULL},
     {"", "", "", NULL}},
    {EVENT_TRIGGER_ON_ACTOR_ENTERED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorDetector", "idActor", NULL, NULL},
     {"idActorDetector", "idActor", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_TRIGGER_ON_ACTOR_EXITED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorDetector", "idActor", NULL, NULL},
     {"idActorDetector", "idActor", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_TRIGGER_PULSE_ACTORS_IN_BOUNDS,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorDetector", "idActor", NULL, NULL},
     {"idActorDetector", "idActors", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_GIZMO_OPERATED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorGizmo", "idActorPlayer", NULL, NULL},
     {"idActorGizmo", "idActorPlayer", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_GIZMO_CAN_OPERATE,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorGizmo", "idActorPlayer", NULL, NULL},
     {"idActorGizmo", "idActorPlayer", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_GATE_GIZMO_OPENED,
     "",
     "",
     {ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorGizmo", NULL, NULL, NULL},
     {"idActorGizmo", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_DOOR_COMPLETELY_OPENED,
     "",
     "",
     {ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idDoor", NULL, NULL, NULL},
     {"idDoor", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_HELPER_ATTACHED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorHelper", "idActorPlayer", NULL, NULL},
     {"idActorHelper", "idActorPlayer", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_HELPER_DISMISSED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorHelper", "idActorPlayer", NULL, NULL},
     {"idActorHelper", "idActorPlayer", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_HELPER_GUARD_OBJECT,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorHelper", "idActorObject", NULL, NULL},
     {"idActorHelper", "idActorObject", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_ACTOR_HIRELING_LEARNED_SKILL,
     "",
     "",
     {ACDID_PARAM_TYPE, POWER_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorHireling", "sSkillName", NULL, NULL},
     {"idActorHireling", "sSkillName", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_HIRELING_HIRED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorHireling", "idActorPlayer", NULL, NULL},
     {"idActorHireling", "idActorPlayer", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_HIRELING_DISMISSED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorHireling", "idActorPlayer", NULL, NULL},
     {"idActorHireling", "idActorPlayer", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_USER_EVENT,
     "",
     "",
     {STRING_PARAM_TYPE, ACDID_PARAM_TYPE, DWORD_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"sEventName", "idActor", "nUserParam", NULL},
     {"sEventName", "idActor", "nUserParam", NULL},
     {"", "", "", NULL}},
    {EVENT_QUEST_STEP_ASSIGNED,
     "",
     "",
     {QUEST_NAME_PARAM_TYPE, STRING_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"sQuestName", "sStepName", NULL, NULL},
     {"sQuestName", "sStepName", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_QUEST_STEP_COMPLETED,
     "",
     "",
     {QUEST_NAME_PARAM_TYPE, STRING_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"sQuestName", "sStepName", "idActorPlayer", NULL},
     {"sQuestName", "sStepName", "idActorPlayer", NULL},
     {"", "", NULL, NULL}},
    {EVENT_QUEST_OBJECTIVE_PROGRESSED,
     "",
     "",
     {QUEST_NAME_PARAM_TYPE, STRING_PARAM_TYPE, ACDID_PARAM_TYPE, DWORD_PARAM_TYPE},
     {"sQuestName", "sStepName", "idActorPlayer", "nCountRemaining"},
     {"sQuestName", "sStepName", "idActorPlayer", "nCountRemaining"},
     {"", "", "", ""}},
    {EVENT_QUEST_OBJECTIVE_COMPLETED,
     "",
     "",
     {QUEST_NAME_PARAM_TYPE, STRING_PARAM_TYPE, DWORD_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"sQuestName", "sStepName", "nObjectiveIndex", NULL},
     {"sQuestName", "sStepName", "nObjectiveIndex", NULL},
     {"", "", "", NULL}},
    {EVENT_QUESTRANGE_CHANGED,
     "",
     "",
     {QUESTRANGE_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"sQuestRangeName", NULL, NULL, NULL},
     {"sQuestRangeName", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_QUEST_TIMER_RUNNING_OUT,
     "",
     "",
     {QUEST_NAME_PARAM_TYPE, STRING_PARAM_TYPE, TIME_PARAM_TYPE, TIME_INTERVAL_PARAM_TYPE},
     {"sQuestName", "sStepName", "flTimeRemaining", "flInterval"},
     {"sQuestName", "sStepName", "flTimeRemaining", "flGameTime"},
     {"", "", "", ""}},
    {EVENT_QUEST_COMPLETED,
     "",
     "",
     {QUEST_NAME_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"sQuestName", NULL, NULL, NULL},
     {"sQuestName", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_WORLD_DELETED,
     "",
     "",
     {WORLD_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"sWorldName", NULL, NULL, NULL},
     {"sWorldName", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_AIQUEUE_EMPTY,
     "",
     "",
     {ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActor", NULL, NULL, NULL},
     {"idActor", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_AIQUEUE_ACTION_COMPLETE,
     "",
     "",
     {ACDID_PARAM_TYPE, DWORD_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActor", "idQAction", NULL, NULL},
     {"idActor", "idQAction", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_BOSS_ENCOUNTER_RESET,
     "",
     "",
     {BOSS_ENCOUNTER_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"sBossEncounterName", NULL, NULL, NULL},
     {"sBossEncounterName", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_SCRIPTED_SEQUENCE_SKIPPED,
     "",
     "",
     {ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idPlayer", NULL, NULL, NULL},
     {"idPlayer", NULL, NULL, NULL},
     {"", NULL, NULL, NULL}},
    {EVENT_CHANGED_ACT,
     "",
     "",
     {INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {NULL, NULL, NULL, NULL},
     {NULL, NULL, NULL, NULL},
     {NULL, NULL, NULL, NULL}},
    {EVENT_NPC_VENDOR_OPERATED,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorVendor", "idActorPlayer", NULL, NULL},
     {"idActorVendor", "idActorPlayer", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_DEVILS_HAND_TRACKING,
     "",
     "",
     {ACDID_PARAM_TYPE, ACDID_PARAM_TYPE, INVALID_PARAM_TYPE, INVALID_PARAM_TYPE},
     {"idActorPlayer", "sDevilsHandItem", NULL, NULL},
     {"idActorPlayer", "sDevilsHandItem", NULL, NULL},
     {"", "", NULL, NULL}},
    {EVENT_PLAYER_ENTERED_HORADRIC_QUEST_LEVEL_AREA,
     "",
     "",
     {ACDID_PARAM_TYPE, LEVELAREA_PARAM_TYPE, QUEST_NAME_PARAM_TYPE, STRING_PARAM_TYPE},
     {"idActorPlayer", "sLevelAreaName", "sQuestName", "sAct"},
     {"idActorPlayer", "sLevelAreaName", "sQuestName", "sAct"},
     {"", "", "", ""}},
};

// ScriptEvent.cpp:1654. All callers pass valid event types.

static void sActivateListenerForScriptInstance(ScriptEventListenerDefinition *, ScriptInstanceID);

// ScriptEvent.cpp:1654.
ScriptEventDescription *ScriptEventGetDescription(ScriptEventType eEventType)
{
    for (int32 i = 0;; ++i)
        if (sgScriptEventDescriptions[i].eEventType == eEventType)
            return &sgScriptEventDescriptions[i];
}

// ScriptEvent.cpp:1676.
void ScriptEventInitializeForApp()
{
    ScriptEventAppGlobals *&ptScriptEventGlobals = sgtAppGlobals.ptScriptEventAppGlobals;
    ptScriptEventGlobals = (ScriptEventAppGlobals *)SigmaMemoryNew(sizeof(ScriptEventAppGlobals), 4, NULL, 1);
    ptScriptEventGlobals->ptDefaultACDIDDefParam = new ACDIDParam(ACDID_INVALID);
    ptScriptEventGlobals->ptDefaultRangeDefParam = new FloatRangeParam(0.0f, 100.0f);
    ptScriptEventGlobals->ptDefaultConvDefParam = new ConversationParam(-1);
    ptScriptEventGlobals->ptDefaultSceneIDDefParam = new SceneIDParam(-1);
    ptScriptEventGlobals->ptDefaultLevelAreaDefParam = new LevelAreaParam(-1);
    ptScriptEventGlobals->ptDefaultStringDefParam = new StringParam("");
    ptScriptEventGlobals->ptDefaultTimeParam = new TimeParam(0xffffffff, ScriptEventVariantParam::LESS_THAN_THIS_VALUE);
    ptScriptEventGlobals->ptDefaultAnimTagParam = new AnimTagParam(0xffffffff);
    ptScriptEventGlobals->ptDefaultQuestNameParam = new QuestNameParam("");
    ptScriptEventGlobals->ptDefaultTimeIntervalParam = new TimeIntervalParam(0.0f);
    ptScriptEventGlobals->ptDefaultDwordParam = new DWORDParam(0xffffffff);
    ptScriptEventGlobals->ptDefaultActorGroupParam = new ActorGroupParam(0xffffffff);
    ptScriptEventGlobals->ptDefaultFloatRangeInsideParam = new FloatRangeInsideParam(0.0f);
    ptScriptEventGlobals->ptDefaultTimedEventParam = new TimedEventParam(-1);
    ptScriptEventGlobals->ptDefaultWorldParam = new WorldParam(-1);
    ptScriptEventGlobals->ptDefaultPowerParam = new PowerParam(-1);
    ptScriptEventGlobals->ptDefaultQuestRangeParam = new QuestRangeParam(-1);
    ptScriptEventGlobals->ptDefaultBossEncounterParam = new BossEncounterParam(-1);
    for (int32 i = 0; i < NUM_SCRIPT_EVENTS; ++i)
    {
        ScriptEventDescription &tDescription = sgScriptEventDescriptions[i];
        ScriptEventVariableType *arInitializers = tDescription.arParamTypes;
        ScriptEventParam *arParams[MAX_NUM_SCRIPT_EVENT_PARAMS];
        int32 nNumInitializers = 0;
        for (; nNumInitializers < MAX_NUM_SCRIPT_EVENT_PARAMS && arInitializers[nNumInitializers] != INVALID_PARAM_TYPE;
             ++nNumInitializers)
        {
            switch (arInitializers[nNumInitializers])
            {
            case ACDID_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultACDIDDefParam;
                break;
            case FLOAT_RANGE_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultRangeDefParam;
                break;
            case CONV_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultConvDefParam;
                break;
            case SCENEID_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultSceneIDDefParam;
                break;
            case LEVELAREA_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultLevelAreaDefParam;
                break;
            case STRING_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultStringDefParam;
                break;
            case TIME_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultTimeParam;
                break;
            case ANIMTAG_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultAnimTagParam;
                break;
            case QUEST_NAME_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultQuestNameParam;
                break;
            case TIME_INTERVAL_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultTimeIntervalParam;
                break;
            case DWORD_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultDwordParam;
                break;
            case ACTOR_GROUP_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultActorGroupParam;
                break;
            case FLOAT_RANGE_INSIDE_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultFloatRangeInsideParam;
                break;
            case TIMED_EVENT_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultTimedEventParam;
                break;
            case WORLD_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultWorldParam;
                break;
            case POWER_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultPowerParam;
                break;
            case QUESTRANGE_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultQuestRangeParam;
                break;
            case BOSS_ENCOUNTER_PARAM_TYPE:
                arParams[nNumInitializers] = ptScriptEventGlobals->ptDefaultBossEncounterParam;
                break;
            default:
                DisplayInternalError(NULL, "", 1734, ERROR_INTERNAL_FATAL);
                ExitProgram(ERROR_INTERNAL_FATAL);
            }
        }
        ScriptEventType eEventType = tDescription.eEventType;
        ptScriptEventGlobals->arScriptEventDefs[eEventType] =
            new ScriptEventDefinition(eEventType, nNumInitializers, arParams, tDescription.szParamNames);
    }
}

// ScriptEvent.cpp:1750.
void ScriptEventDisposeForApp()
{
    ScriptEventAppGlobals *&ptScriptEventGlobals = sgtAppGlobals.ptScriptEventAppGlobals;
    for (int32 i = 0; i < NUM_SCRIPT_EVENTS; ++i)
    {
        delete ptScriptEventGlobals->arScriptEventDefs[i];
        ptScriptEventGlobals->arScriptEventDefs[i] = NULL;
    }
    ::delete ptScriptEventGlobals->ptDefaultACDIDDefParam;
    ptScriptEventGlobals->ptDefaultACDIDDefParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultRangeDefParam;
    ptScriptEventGlobals->ptDefaultRangeDefParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultConvDefParam;
    ptScriptEventGlobals->ptDefaultConvDefParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultSceneIDDefParam;
    ptScriptEventGlobals->ptDefaultSceneIDDefParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultLevelAreaDefParam;
    ptScriptEventGlobals->ptDefaultLevelAreaDefParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultStringDefParam;
    ptScriptEventGlobals->ptDefaultStringDefParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultTimeParam;
    ptScriptEventGlobals->ptDefaultTimeParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultAnimTagParam;
    ptScriptEventGlobals->ptDefaultAnimTagParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultQuestNameParam;
    ptScriptEventGlobals->ptDefaultQuestNameParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultTimeIntervalParam;
    ptScriptEventGlobals->ptDefaultTimeIntervalParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultDwordParam;
    ptScriptEventGlobals->ptDefaultDwordParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultActorGroupParam;
    ptScriptEventGlobals->ptDefaultActorGroupParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultFloatRangeInsideParam;
    ptScriptEventGlobals->ptDefaultFloatRangeInsideParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultTimedEventParam;
    ptScriptEventGlobals->ptDefaultTimedEventParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultWorldParam;
    ptScriptEventGlobals->ptDefaultWorldParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultPowerParam;
    ptScriptEventGlobals->ptDefaultPowerParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultQuestRangeParam;
    ptScriptEventGlobals->ptDefaultQuestRangeParam = NULL;
    ::delete ptScriptEventGlobals->ptDefaultBossEncounterParam;
    ptScriptEventGlobals->ptDefaultBossEncounterParam = NULL;
    SigmaMemoryFree(ptScriptEventGlobals, NULL);
    ptScriptEventGlobals = NULL;
}

// ScriptEvent.cpp:1785.
void ScriptEventInitializeForGame()
{
    ScriptEventGameGlobals *&ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;
    ptScriptEventGlobals = (ScriptEventGameGlobals *)SigmaMemoryNew(sizeof(ScriptEventGameGlobals), 4, NULL, 1);
    ptScriptEventGlobals->arScriptEventListeners = TGrowableDataArrayCreateTracked<ScriptEventListenerDefinition, int>(
        "ScriptEventListenerDefinitions", NULL, MAX_APP_SCRIPT_EVENT_LISTENER_DEFINITIONS);
    ptScriptEventGlobals->arScriptEventWaitListeners =
        TGrowableDataArrayCreateTracked<ScriptEventListenerDefinition, int>("ScriptEventGameListenerDefinitions", NULL,
                                                                            MAX_GAME_SCRIPT_EVENT_LISTENER_DEFINITIONS);
    ptScriptEventGlobals->poolScriptEventListenerInstances = new ScriptEventListenerInstanceListPool();
    ptScriptEventGlobals->poolScriptEventListenerInstances->SetBlockCount_Tracked(MAX_LISTENER_INSTANCES);
    for (int32 i = 0; i < NUM_SCRIPT_EVENTS; ++i)
        ptScriptEventGlobals->arActiveListenerLists[i] =
            new PooledScriptEventListenerInstanceList(ptScriptEventGlobals->poolScriptEventListenerInstances);
}

// ScriptEvent.cpp:1809.
void ScriptEventDisposeForGame()
{
    ScriptEventGameGlobals *&ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;
    for (int32 i = 0; i < NUM_SCRIPT_EVENTS; ++i)
    {
        delete ptScriptEventGlobals->arActiveListenerLists[i];
        ptScriptEventGlobals->arActiveListenerLists[i] = NULL;
    }
    delete ptScriptEventGlobals->poolScriptEventListenerInstances;
    ptScriptEventGlobals->poolScriptEventListenerInstances = NULL;
    {
        TDataArrayIterator<ScriptEventListenerDefinition> tIter;
        while (ScriptEventListenerDefinition *ptListenerDef =
                   ptScriptEventGlobals->arScriptEventWaitListeners->Iterate(tIter))
        {
            SigmaMemoryFree(ptListenerDef->szFuncName, NULL);
            ptListenerDef->szFuncName = NULL;
        }
        TDataArrayDispose(ptScriptEventGlobals->arScriptEventWaitListeners);
    }
    {
        TDataArrayIterator<ScriptEventListenerDefinition> tIter;
        while (ScriptEventListenerDefinition *ptListenerDef =
                   ptScriptEventGlobals->arScriptEventListeners->Iterate(tIter))
        {
            SigmaMemoryFree(ptListenerDef->szFuncName, NULL);
            ptListenerDef->szFuncName = NULL;
        }
        TDataArrayDispose(ptScriptEventGlobals->arScriptEventListeners);
    }
    SigmaMemoryFree(ptScriptEventGlobals, NULL);
    ptScriptEventGlobals = NULL;
}

// ScriptEvent.cpp:1853.
void ScriptEventUpdate()
{
    ScriptEventGameGlobals *ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;
    uint32 dwGameTime = GameGetActiveGameCommonData().dwServerTime;
    uint32 dwElapsed = dwGameTime - ptScriptEventGlobals->dwLastHeartbeat;
    if (dwElapsed > TICKS_PER_HEARTBEAT)
    {
        ScriptEventOnTick(dwGameTime);
        ScriptEventOnHeartBeat(dwGameTime);
        ptScriptEventGlobals->dwLastHeartbeat = dwGameTime;
    }
    if (ptScriptEventGlobals->dwFlags & (1 << SCRIPT_EVENT_GAME_GLOBALS_LISTENER_FLAGGED_FOR_DELETE))
    {
        ptScriptEventGlobals->dwFlags &= ~(1 << SCRIPT_EVENT_GAME_GLOBALS_LISTENER_FLAGGED_FOR_DELETE);
        for (int32 i = 0; i < NUM_SCRIPT_EVENTS; ++i)
        {
            PooledScriptEventListenerInstanceList *listListenerInsts = ptScriptEventGlobals->arActiveListenerLists[i];
            POS pos = listListenerInsts->GetHeadPosition();
            while (pos)
            {
                POS posCur = pos;
                ScriptEventListenerInstance &tListenerInst = listListenerInsts->GetNext(pos);
                if (tListenerInst.dwFlags & (1 << SCRIPT_EVENT_LISTENER_FLAGGED_FOR_DELETE))
                    listListenerInsts->RemoveAt(posCur);
            }
        }
    }
}

// ScriptEvent.cpp:1897.
static void sDispatchEventToScript(ScriptEventDefinition *ptScriptEventDef, ScriptEventListenerInstance &tListenerInst,
                                   ScriptEventListenerDefinition &tEventListenerDef, ScriptEventParamList *ptAppParams,
                                   ScriptEventReturnValue *ptReturnValue, ScriptAttachmentType eAttachType,
                                   DataID idAttachment)
{
    ScriptInstanceID idScriptInstance = LuaScriptNewInstanceWithScriptID(
        tListenerInst.idScript, tEventListenerDef.szFuncName, NULL, -2, idAttachment, eAttachType);
    lua_State *ptLuaState = LuaScriptGetInstanceState(idScriptInstance);
    LuaStackChecker tStackCheck(ptLuaState);
    if (LuaScriptGetFunction(ptLuaState, tEventListenerDef.szFuncName))
    {
        int32 nNumParams = 0;
        if (!(tEventListenerDef.dwFlags & (1 << SCRIPT_EVENT_LISTENER_DEF_NO_ATTACH_PARAMETERS)))
        {
            LuaScriptPushExecutingScript(idScriptInstance);
            lua_pushinteger(ptLuaState, idAttachment);
            lua_pushstring(ptLuaState, LuaScriptAttachmentTypeToString(eAttachType));
            LuaScriptPopExecutingScript();
            nNumParams = 2;
        }
        nNumParams += ptAppParams->PushScriptParams(ptLuaState);
        if (idScriptInstance != SCRIPTINSTANCEID_INVALID)
            ptAppParams->RefObjects(idScriptInstance);
        if (!ptReturnValue)
        {
            LuaScriptSetInstanceResumeTick(idScriptInstance, GameGetActiveGameCommonData().dwServerTime, nNumParams);
            LuaTickScriptInstanceId(idScriptInstance);
        }
        else
        {
            int32 nError = LuaCall(idScriptInstance, ptLuaState, nNumParams, ptReturnValue->m_nNumReturnValues,
                                   tEventListenerDef.szFuncName);
            LuaScriptCloseInstance(idScriptInstance);
            if (!nError)
            {
                ptReturnValue->HandleScriptReturnValues(ptLuaState);
                lua_settop(ptLuaState, -ptReturnValue->m_nNumReturnValues - 1);
            }
        }
    }
}

// ScriptEvent.cpp:1959.
static ScriptInstanceID sDispatchEventToConductorScript(ConductorInstance *ptConductor,
                                                        ScriptEventDefinition *ptScriptEventDef,
                                                        ScriptEventListenerInstance &tListenerInst,
                                                        ScriptEventListenerDefinition &tEventListenerDef,
                                                        ScriptEventParamList *ptAppParams,
                                                        ScriptEventReturnValue *ptReturnValue)
{
    ScriptInstanceID idScriptInstance = SCRIPTINSTANCEID_INVALID;
    int32 nPhase;
    int32 nSequence;
    ConductorGetPhaseAndSequenceFromStartFunctionName(tEventListenerDef.szFuncName, nPhase, nSequence);
    if (nPhase != 0 && (ptConductor->bFlaggedForPhaseSkip || ptConductor->bFlaggedForAdvancement))
        return idScriptInstance;
    const ConductorDefinition *ptConductorDef =
        (const ConductorDefinition *)gptGroupDefConductor->LockR(ptConductor->snoConductor, FALSE);
    const ConductorSequence &tSequence = ptConductorDef->arPhases[nPhase].arSequences[nSequence];
    // bRunAsScriptInstance and tSequence are present in DWARF but dead in release.
    gptGroupDefConductor->UnlockR((void *&)ptConductorDef);
    idScriptInstance = LuaScriptNewInstanceWithScriptID(tListenerInst.idScript, tEventListenerDef.szFuncName, NULL, -2,
                                                        -1, SCRIPT_ATTACH_NONE);
    if (idScriptInstance != SCRIPTINSTANCEID_INVALID)
    {
        lua_State *ptLuaState = LuaScriptGetInstanceState(idScriptInstance);
        LuaStackChecker tStackCheck(ptLuaState);
        if (ptLuaState && LuaScriptGetFunction(ptLuaState, tEventListenerDef.szFuncName))
        {
            LuaScriptPushExecutingScript(idScriptInstance);
            lua_pushinteger(ptLuaState, ptConductor->idAttachment);
            lua_pushstring(ptLuaState, LuaScriptAttachmentTypeToString(ptConductor->eAttachType));
            LuaScriptPopExecutingScript();
            int32 nNumParams = 2 + ptAppParams->PushScriptParams(ptLuaState);
            ptAppParams->RefObjects(idScriptInstance);
            int32 nNumReturnValues = ptReturnValue ? ptReturnValue->m_nNumReturnValues : 0;
            LuaScriptSetInstanceResumeTick(idScriptInstance, GameGetActiveGameCommonData().dwServerTime, nNumParams);
            LuaTickScriptInstanceId(idScriptInstance);
        }
    }
    return idScriptInstance;
}

// ScriptEvent.cpp:2065.
BOOL GameParamsPassFilter(ScriptEventFilters &tFilters, ScriptEventGameParams &tParamsGame, DataID idAttachment,
                          ScriptID idScript, ScriptInstanceID idScriptInstance)
{
    BOOL bPassesFilter = TRUE;
    for (int32 i = 0; i < tFilters.nNumParams; ++i)
    {
        ScriptEventVariantParam &tParamFilter = tFilters.arParams[i];
        ScriptEventVariantParam &tParamGame = tParamsGame.arGameParams[i][tParamFilter.eParamType];
        if (tParamFilter.eParamType == SCRIPT_EVENT_VARIANT_PARAM_NONE)
            continue;
        if (tParamGame.eParamType == SCRIPT_EVENT_VARIANT_PARAM_NONE)
        {
            CRefString sScriptName("Unknown");
            if (idScriptInstance != SCRIPTINSTANCEID_INVALID)
                idScript = LuaScriptGetInstanceParent(idScriptInstance);
            if (idScript != SCRIPTID_INVALID)
            {
                SNO snoScript = LuaScriptGetSNO(idScript);
                if (snoScript != -1)
                    sScriptName = SNOToString(SCRIPT_SNO, snoScript, FALSE);
            }
        }
        bPassesFilter = tParamFilter.FilterAgainst(tParamGame, idAttachment, idScript, idScriptInstance);
        if (!bPassesFilter)
            break;
    }
    return bPassesFilter;
}

// ScriptEvent.cpp:2122.
BOOL sScriptEventListenerPassFilters(POS posListener, ScriptEventListenerDefinition *ptListenerDef,
                                     WorldPlace *ptWorldPlace, float flRange, ScriptEventGameParams &tGameParams,
                                     DataID idAttachment, ScriptID idListener)
{
    if (!LuaScriptIsListenerEnabled(posListener, ptListenerDef->nIndex))
        return FALSE;
    if (ptWorldPlace && !LuaScriptIsListenerInRange(posListener, *ptWorldPlace, flRange))
        return FALSE;
    return GameParamsPassFilter(ptListenerDef->tFilters, tGameParams, idAttachment, idListener,
                                SCRIPTINSTANCEID_INVALID);
}

// ScriptEvent.cpp:2138.
BOOL sScriptInstancePassFilters(ScriptEventFilters &tFilters, ScriptEventGameParams &tGameParams, DataID idAttachment,
                                ScriptID idScript, ScriptInstanceID idScriptInstance)
{
    return GameParamsPassFilter(tFilters, tGameParams, idAttachment, idScript, idScriptInstance);
}

// ScriptEvent.cpp:2143.
// NON_MATCHING: 3668 vs 3592. On DWARF lines with the DWARF local set and blocks (x13): the flagged-for-delete
// test is a `continue` (locals at while-body depth); costs spills/regalloc; loop unswitching differs.
#line 2143
BOOL ScriptEventDispatchEvent(
    ScriptEventType eEventType,
    ScriptEventParamList *ptEventDefParams,
    ScriptEventReturnValue *ptReturnValue,
    WorldPlace *ptWorldPlace,
    float flRange,
    ACDID idACDTarget)
{



    SGameGlobals &tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_SHUTTING_DOWN))
        return FALSE;



    BOOL bTriggered = FALSE;

    ScriptEventAppGlobals *ptScriptEventAppGlobals = sgtAppGlobals.ptScriptEventAppGlobals;
    ScriptEventGameGlobals *ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;

    PooledScriptEventListenerInstanceList *listListeners = ptScriptEventGlobals->arActiveListenerLists[eEventType];

    ++ptScriptEventGlobals->nTotalEventsDispatched;

    if (!listListeners->Count())
    {
        ++ptScriptEventGlobals->nTotalEventsEasyOut;
        return FALSE;
    }

    ScriptEventDefinition *ptScriptEventDef = ptScriptEventAppGlobals->arScriptEventDefs[eEventType];
    ScriptEventGameParams tGameParams;

    ptEventDefParams->ToGameParams(tGameParams);

    POS pos = listListeners->GetHeadPosition();

    while (pos)
    {
        ScriptEventListenerInstance &tListenerInst = listListeners->GetNext(pos);
        // DWARF: idListenerDef / eListenerType / ptListenerDef are in the while body's scope, after this test.
        if (tListenerInst.dwFlags & (1 << SCRIPT_EVENT_LISTENER_FLAGGED_FOR_DELETE))
            continue;



        ScriptEventListenerDefinitionID idListenerDef = tListenerInst.idListenerDef;
        ListenerType eListenerType = tListenerInst.eListenerType;

        ++ptScriptEventGlobals->nTotalListenersConsidered;



        ScriptEventListenerDefinition *ptListenerDef;

        switch (eListenerType)
        {
        case LISTENER_TYPE_SCRIPT:

            ptListenerDef = ptScriptEventGlobals->arScriptEventListeners->ElementGet(idListenerDef);


            if (!(ptListenerDef->dwFlags & (1 << SCRIPT_EVENT_LISTENER_IS_CONDUCTOR)))
            {
                POS scopedPos = LuaScriptGetFirstAttachment(tListenerInst.idScript);

                while (scopedPos)
                {
                    ScriptAttachmentType eAttachType;
                    DataID idAttachment;
                    BOOL bFlaggedForDeletion;
                    POS posCur = scopedPos;

                    LuaScriptGetNextAttachment(scopedPos, eAttachType, idAttachment, bFlaggedForDeletion);

                    if (bFlaggedForDeletion)
                        continue;


                    // Original compares attachment id only; no attachment-type condition.
                    if (idACDTarget != ACDID_INVALID &&
                        idAttachment != idACDTarget)
                        continue;



                    if (sScriptEventListenerPassFilters(posCur, ptListenerDef, ptWorldPlace, flRange, tGameParams, idAttachment, tListenerInst.idScript))
                    {


                        if (ptListenerDef->eEventType == EVENT_USER_EVENT &&

                            !(SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_TRACE_SCRIPT_EVENTS)))
                            AppDrawFlagIsSet(APP_DRAW_DEBUG_QUESTS_BIT);









                        ++ptScriptEventGlobals->nTotalListenersCalled;

                        if (ptListenerDef->dwFlags & (1 << SCRIPT_EVENT_LISTENER_DEF_ONE_SHOT))

                            LuaScriptEnableListener(posCur, ptListenerDef->nIndex, FALSE);


                        if (eEventType == EVENT_ACTOR_SPAWNED_BY_SPAWNER && !ptListenerDef->szFuncName &&

                            OnlineService::IsPTR())
                        {

                            ACDID idACDSpawner = ((ACDIDParam *)ptEventDefParams->m_arParams[0])->idACD;
                            ACDID idACDActorSpawned = ((ACDIDParam *)ptEventDefParams->m_arParams[1])->idACD;
                            SNO snoSpawner = SNO_NONE; // DWARF: constant -1 over the whole block
                            SNO snoActorSpawned = SNO_NONE; // DWARF: constant -1 over the whole block
                            uint32 actorSpawnedFlags = 0; // DWARF: constant 0 over the whole block

                            ActorCommonData *ptACDSpawner = ACDTryToGet(idACDSpawner);
                            ActorCommonData *ptACDActorSpawned = ACDTryToGet(idACDActorSpawned);

                            if (ptACDActorSpawned)
                            {






                                ActorID idActorSpawned = (ActorID)ACDGetOwner(idACDActorSpawned);
                                Actor *ptActorSpawned = ActorTryToGet(idActorSpawned);
                            }





                            DisplayInternalError(NULL, "", 2285, ERROR_INTERNAL_FATAL);
                            ExitProgram(ERROR_INTERNAL_FATAL);
                        }





                        sDispatchEventToScript(ptScriptEventDef, tListenerInst, *ptListenerDef, ptEventDefParams, ptReturnValue, eAttachType, idAttachment);
                        bTriggered = TRUE;
                    }
                }
            }
            else
            {
                ConductorInstance *ptConductor = SConductorFindConductorByScriptID(tListenerInst.idScript);
                if (GameParamsPassFilter(ptListenerDef->tFilters, tGameParams,

                                ptConductor->idAttachment, SCRIPTID_INVALID, SCRIPTINSTANCEID_INVALID))
                {


                    ++ptListenerDef->nEventCount;

                    if (ptListenerDef->eEventType == EVENT_USER_EVENT &&

                        !(SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_TRACE_SCRIPT_EVENTS))) AppDrawFlagIsSet(APP_DRAW_DEBUG_QUESTS_BIT);








                    if (ptListenerDef->nEventCount >= ptListenerDef->nEventCountRequired)
                    {

                        ptListenerDef->nEventCount = 0;

                        ++ptScriptEventGlobals->nTotalListenersCalled;



                        int32 nPhase;
                        int32 nSequence;
                        ConductorGetPhaseAndSequenceFromStartFunctionName(ptListenerDef->szFuncName, nPhase, nSequence);

                        uint32 nPhasePriorToScript = ptConductor->nCurrentPhase;


                        const ConductorDefinition *ptConductorDef = (const ConductorDefinition *)gptGroupDefConductor->LockR(ptConductor->snoConductor, FALSE);
                        const ConductorSequence &tSequence = ptConductorDef->arPhases[nPhase].arSequences[nSequence];
                        gptGroupDefConductor->UnlockR((void *&)ptConductorDef);



                        if (!tSequence.bRepeatable)
                        {

                            if (nPhase == 0)
                            {
                                for (int32 i = 0; i < 20; ++i)

                                    if (ptConductor->arGlobalEventHandlers[i] == tListenerInst.idListenerDef)

                                        ptConductor->arGlobalEventHandlers[i] = SCRIPT_EVENT_LISTENER_DEFINITION_INVALID;


                            }
                            else
                            {
                                for (int32 i = 0; i < 20; ++i)

                                    if (ptConductor->arPhaseEventHandlers[i] == tListenerInst.idListenerDef)

                                        ptConductor->arPhaseEventHandlers[i] = SCRIPT_EVENT_LISTENER_DEFINITION_INVALID;


                            }

                            tListenerInst.dwFlags |= 1 << SCRIPT_EVENT_LISTENER_FLAGGED_FOR_DELETE;
                            ptScriptEventGlobals->dwFlags |= 1 << SCRIPT_EVENT_GAME_GLOBALS_LISTENER_FLAGGED_FOR_DELETE;
                        }

                        ScriptInstanceID idScriptInstance = sDispatchEventToConductorScript(ptConductor, ptScriptEventDef, tListenerInst, *ptListenerDef, ptEventDefParams, ptReturnValue);







                        if (!tSequence.bRepeatable)
                        {
                            if (nPhase == 0)

                                ptConductor->arGlobalScripts[nSequence] = idScriptInstance;




                            else if (ptConductor->nCurrentPhase == nPhasePriorToScript)

                                ptConductor->arPhaseScripts[nSequence] = idScriptInstance;

                        }




                        else if (idScriptInstance != SCRIPTINSTANCEID_INVALID)
                        {
                            RepeatableScriptInstanceData tData;
                            tData.nPhase = nPhase;
                            tData.nSequence = nSequence;
                            tData.idScriptInstance = idScriptInstance;
                            ptConductor->tRepeatableScriptInstanceDataList.AddTail_Tracked(tData);

                            // DWARF local, constant 20 over the rest of the block; no code uses it.
                            const int32 nRepeatableScriptInstanceSoftCap = 20;

                        }

                        bTriggered = TRUE;
                    }
                }
            }

            break;

        case LISTENER_TYPE_SCRIPT_INSTANCE:








            if (idACDTarget != ACDID_INVALID)
                break;
            ptListenerDef = ptScriptEventGlobals->arScriptEventWaitListeners->ElementTryToGet(idListenerDef);

            if (ptListenerDef)
            {




                if (sScriptInstancePassFilters(ptListenerDef->tFilters, tGameParams, -1, SCRIPTID_INVALID, tListenerInst.idScriptInstance))
                {

                    ++ptScriptEventGlobals->nTotalInstancesNotified;
                    LuaScriptInstanceNotifyEvent(tListenerInst.idScriptInstance, idListenerDef, eEventType, ptEventDefParams);
                    bTriggered = TRUE;
                }
            }
            else {
                lua_State *ptLuaState = LuaScriptTryGetLuaState(tListenerInst.idScript);
                SNOName snoNameSource;
                if (ptLuaState)
                {
                    snoNameSource = LuaScriptGetSNONameForLuaState(ptLuaState);
                }

            }

            break;

        default:
            break;
        }
    }



    return bTriggered;
}
#line 1096

// ScriptEvent.cpp:2477.
ScriptEventListenerDefinitionID ScriptEventAddListener(ScriptID idScript, LPCSTR szHandlerFuncName,
                                                       ScriptEventType eEventType, ScriptEventFilters &tFilters,
                                                       uint32 dwFlags, int32 nIndex, int32 nEventCount)
{
    ScriptEventGameGlobals *ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;
    ScriptEventListenerDefinitionID idListenerDef = SCRIPT_EVENT_LISTENER_DEFINITION_INVALID;
    lua_State *ptLuaState = LuaScriptGetLuaStateWithScriptID(idScript);
    lua_getfield(ptLuaState, LUA_GLOBALSINDEX, szHandlerFuncName);
    if (lua_type(ptLuaState, -1) == LUA_TFUNCTION)
    {
        idListenerDef = ptScriptEventGlobals->arScriptEventListeners->ElementNew();
        ScriptEventListenerDefinition *ptScriptEventListenerDef =
            ptScriptEventGlobals->arScriptEventListeners->ElementGet(idListenerDef);
        ptScriptEventListenerDef->eEventType = eEventType;
        ptScriptEventListenerDef->tFilters = tFilters;
        ptScriptEventListenerDef->dwFlags = dwFlags;
        ptScriptEventListenerDef->nIndex = nIndex;
        ptScriptEventListenerDef->nEventCountRequired = nEventCount;
        ptScriptEventListenerDef->nEventCount = 0;
        size_t nFuncNameLen = strlen(szHandlerFuncName);
        ptScriptEventListenerDef->szFuncName = (CHAR *)SigmaMemoryNew(nFuncNameLen + 1, 4, NULL, 0);
        strcpy(ptScriptEventListenerDef->szFuncName, szHandlerFuncName);
    }
    lua_pop(ptLuaState, 1);
    return idListenerDef;
}

// ScriptEvent.cpp:2521.
ScriptEventWaitListenerID ScriptEventAddGameWaitListener(ScriptInstanceID idScriptInstance, ScriptEventType eEventType,
                                                         ScriptEventFilters &tFilters)
{
    ScriptEventGameGlobals *ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;
    ScriptEventListenerDefinitionID idListenerDef = ptScriptEventGlobals->arScriptEventWaitListeners->ElementNew();
    ScriptEventListenerDefinition *ptScriptEventListenerDef =
        ptScriptEventGlobals->arScriptEventWaitListeners->ElementGet(idListenerDef);
    ptScriptEventListenerDef->eEventType = eEventType;
    ptScriptEventListenerDef->szFuncName = NULL;
    ptScriptEventListenerDef->dwFlags = 0;
    ptScriptEventListenerDef->nIndex = -1;
    ptScriptEventListenerDef->tFilters = tFilters;
    sActivateListenerForScriptInstance(ptScriptEventListenerDef, idScriptInstance);
    return idListenerDef;
}

// ScriptEvent.cpp:2546.
static void sActivateListenerForScriptInstance(ScriptEventListenerDefinition *ptListenerDef,
                                               ScriptInstanceID idScriptInstance)
{
    ScriptEventGameGlobals *ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;
    PooledScriptEventListenerInstanceList *listListenerInsts =
        ptScriptEventGlobals->arActiveListenerLists[ptListenerDef->eEventType];
    ScriptEventListenerInstance tListenerInst;
    tListenerInst.idListenerDef = ptListenerDef->id;
    tListenerInst.idScript = SCRIPTID_INVALID;
    tListenerInst.idScriptInstance = idScriptInstance;
    tListenerInst.eListenerType = LISTENER_TYPE_SCRIPT_INSTANCE;
    tListenerInst.dwFlags = 0;
    listListenerInsts->AddTail_Tracked(tListenerInst);
}

// ScriptEvent.cpp:2568.
static void sActivateListenerForScript(ScriptEventListenerDefinition *ptListenerDef, ScriptID idScript)
{
    ScriptEventGameGlobals *ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;
    PooledScriptEventListenerInstanceList *listListenerInsts =
        ptScriptEventGlobals->arActiveListenerLists[ptListenerDef->eEventType];
    ScriptEventListenerInstance tListenerInst;
    tListenerInst.idListenerDef = ptListenerDef->id;
    tListenerInst.idScript = idScript;
    tListenerInst.idScriptInstance = SCRIPTINSTANCEID_INVALID;
    tListenerInst.eListenerType = LISTENER_TYPE_SCRIPT;
    tListenerInst.dwFlags = 0;
    listListenerInsts->AddTail_Tracked(tListenerInst);
}

// ScriptEvent.cpp:2590.
void ScriptEventActivateListener(ScriptEventListenerDefinitionID idListenerDef, ScriptID idScript)
{
    ScriptEventGameGlobals *ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;
    sActivateListenerForScript(ptScriptEventGlobals->arScriptEventListeners->ElementGet(idListenerDef), idScript);
}

// ScriptEvent.cpp:2604.
static void sDeActivateListeners(ScriptEventListenerDefinition *ptListenerDef)
{
    ScriptEventGameGlobals *ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;
    if (!ptScriptEventGlobals)
        return;
    PooledScriptEventListenerInstanceList *listListenerInsts =
        ptScriptEventGlobals->arActiveListenerLists[ptListenerDef->eEventType];
    POS posListener = listListenerInsts->GetHeadPosition();
    while (posListener)
    {
        ScriptEventListenerInstance &tListenerInst = listListenerInsts->GetNext(posListener);
        if (tListenerInst.idListenerDef == ptListenerDef->id)
        {
            tListenerInst.dwFlags |= 1 << SCRIPT_EVENT_LISTENER_FLAGGED_FOR_DELETE;
            ptScriptEventGlobals->dwFlags |= 1 << SCRIPT_EVENT_GAME_GLOBALS_LISTENER_FLAGGED_FOR_DELETE;
            return;
        }
    }
}

// ScriptEvent.cpp:2635.
void ScriptEventDeActivateListener(ScriptEventListenerDefinitionID idListenerDef)
{
    ScriptEventGameGlobals *ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;
    if (!ptScriptEventGlobals)
        return;
    ScriptEventListenerDefinition *ptListenerDef =
        ptScriptEventGlobals->arScriptEventListeners->ElementGet(idListenerDef);
    sDeActivateListeners(ptListenerDef);
}

// ScriptEvent.cpp:2651.
void ScriptEventRemoveListener(ScriptEventListenerDefinitionID idListenerDef)
{
    ScriptEventGameGlobals *ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;
    if (!ptScriptEventGlobals)
        return;
    ScriptEventListenerDefinition *ptListenerDef =
        ptScriptEventGlobals->arScriptEventListeners->ElementGet(idListenerDef);
    SigmaMemoryFree(ptListenerDef->szFuncName, NULL);
    ptListenerDef->szFuncName = NULL;
    ptScriptEventGlobals->arScriptEventListeners->ElementDelete(ptListenerDef);
}

// ScriptEvent.cpp:2686.
void ScriptEventRemoveGameWaitListener(ScriptEventWaitListenerID idListenerDef)
{
    ScriptEventGameGlobals *ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;
    ScriptEventListenerDefinition *ptListenerDef =
        ptScriptEventGlobals->arScriptEventWaitListeners->ElementGet(idListenerDef);
    sDeActivateListeners(ptListenerDef);
    ptScriptEventGlobals->arScriptEventWaitListeners->ElementDeleteByID(idListenerDef);
}

// ScriptEvent.cpp:2703.
static void sScriptAddEventHandler(ScriptID idScript, LPCSTR szFuncName, ScriptEventType eEventType,
                                   ScriptEventFilters &tFilters, uint32 dwFlags,
                                   ScriptEventListenerDefinitionID *arEventHandlers, int32 &nNumEventHandlers,
                                   int32 nMaxEventHandlers, int32 nEventCount)
{
    if (nNumEventHandlers >= nMaxEventHandlers)
        return;
    ScriptEventListenerDefinitionID idListenerDef =
        ScriptEventAddListener(idScript, szFuncName, eEventType, tFilters, dwFlags, nNumEventHandlers, nEventCount);
    if (idListenerDef == SCRIPT_EVENT_LISTENER_DEFINITION_INVALID)
        return;
    arEventHandlers[nNumEventHandlers] = idListenerDef;
    ++nNumEventHandlers;
}

// ScriptEvent.cpp:2734.
int32 ScriptEventInitializeForConductorPhase(ConductorInstance &tConductor, int32 nPhase)
{
    int nNumEventHandlers = 0;
    LuaStackChecker tStackCheck(tConductor.ptConductorGlobalState);
    ScopedConductorDefinitionLockR ptConductorDef(tConductor.snoConductor);
    ConductorPhase &tPhase = ptConductorDef->arPhases[nPhase];
    for (uint32 i = 0; i < tPhase.nNumSequences; ++i)
    {
        ConductorSequence &tSequence = tPhase.arSequences[i];
        ScriptMessageMap *ptMessageMap = tSequence.arScriptMessageMap;
        if (ptMessageMap->eEventType == EVENT_INVALID)
            continue;
        ScriptEventFilters tFilters;
        CRefString sFuncName = ConductorGetSequenceStartFunctionName(nPhase, i);
        lua_getfield(tConductor.ptConductorGlobalState, LUA_GLOBALSINDEX, sFuncName.str());
        if (lua_type(tConductor.ptConductorGlobalState, -1) == LUA_TFUNCTION)
        {
            tFilters.nNumParams = 0;
            ScriptEventDescription *ptSED = ScriptEventGetDescription(ptMessageMap->eEventType);
            for (int32 j = 0; j < MAX_NUM_SCRIPT_EVENT_PARAMS && ptSED->arParamTypes[j] != INVALID_PARAM_TYPE; ++j)
            {
                tFilters.arParams[j] = ptMessageMap->arMessageFilters[j];
                ++tFilters.nNumParams;
            }
            ScriptEventListenerDefinitionID *arEventHandlers =
                nPhase == 0 ? tConductor.arGlobalEventHandlers : tConductor.arPhaseEventHandlers;
            uint32 dwFlags = (1 << SCRIPT_EVENT_LISTENER_DEF_ONE_SHOT) | (1 << SCRIPT_EVENT_LISTENER_IS_CONDUCTOR);
            int32 nMaxEventHandlers = 20;
            sScriptAddEventHandler(tConductor.idGlobalScript, sFuncName.str(), ptMessageMap->eEventType, tFilters,
                                   dwFlags, arEventHandlers, nNumEventHandlers, nMaxEventHandlers,
                                   ptMessageMap->nEventCount);
        }
        lua_pop(tConductor.ptConductorGlobalState, 1);
    }
    return nNumEventHandlers;
}

// ScriptEvent.cpp:2818.
int32 ScriptEventInitializeForState(SNO snoScript, ScriptID idScript, lua_State *ptLuaState,
                                    ScriptEventListenerDefinitionID *arEventHandlers, int32 nMaxEventHandlers)
{
    int nNumEventHandlers = 0;
    LuaStackChecker tStackCheck(ptLuaState);
    ScopedScriptDefinitionLockR ptScriptDef(snoScript);
    for (int32 i = 0; i < ptScriptDef->nMessageMapCount; ++i)
    {
        ScriptMessageMap *ptMessageMap = &ptScriptDef->arScriptMessageMap[i];
        if (ptMessageMap->eEventType == EVENT_INVALID)
            continue;
        ScriptEventFilters tFilters;
        LPCSTR szFuncName = ptMessageMap->szHandlerFunction;
        lua_getfield(ptLuaState, LUA_GLOBALSINDEX, szFuncName);
        if (lua_type(ptLuaState, -1) == LUA_TFUNCTION)
        {
            tFilters.nNumParams = 0;
            ScriptEventDescription *ptSED = ScriptEventGetDescription(ptMessageMap->eEventType);
            for (int32 j = 0; j < MAX_NUM_SCRIPT_EVENT_PARAMS && ptSED->arParamTypes[j] != INVALID_PARAM_TYPE; ++j)
            {
                tFilters.arParams[j] = ptMessageMap->arMessageFilters[j];
                ++tFilters.nNumParams;
            }
            sScriptAddEventHandler(idScript, szFuncName, ptMessageMap->eEventType, tFilters,
                                   ptMessageMap->dwListenerFlags, arEventHandlers, nNumEventHandlers, nMaxEventHandlers,
                                   ptMessageMap->nEventCount);
        }
        lua_pop(ptLuaState, 1);
    }
    return nNumEventHandlers;
}

// ScriptEvent.cpp:2911.
BOOL ScriptEventParseEventFilters(lua_State *ptLuaState, ScriptEventType eEventType, ScriptEventFilters &tFilters)
{
    return sgtAppGlobals.ptScriptEventAppGlobals->arScriptEventDefs[eEventType]->ParseFiltersFromScript(tFilters,
                                                                                                        ptLuaState);
}

// ScriptEvent.cpp:2934.
void ScriptEventOnGroupKilled(uint32 uGroup, ACDID idACDKiller)
{
    ActorGroupParam tParam1(uGroup);
    ACDIDParam tParam2(ACDGetControllingActor(idACDKiller));
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_GROUP_KILLED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:2947.
void ScriptEventOnActorKilled(ACDID idACDActor, ACDID idACDKiller)
{
    ACDIDParam tParam1(idACDActor);
    ACDIDParam tParam2(ACDGetControllingActor(idACDKiller));
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_ACTOR_KILLED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:2960.
void ScriptEventOnActorDamaged(ACDID idACDActor, ACDID idACDDamager)
{
    ACDIDParam tParam1(idACDActor);
    ACDIDParam tParam2(ACDGetControllingActor(idACDDamager));
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_ACTOR_DAMAGED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:2972.
void ScriptEventOnActorSighted(ACDID idACDActor, ACDID idACDTarget)
{
    ACDIDParam tParam1(idACDActor);
    ACDIDParam tParam2(idACDTarget);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_ACTOR_SIGHTED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:2985.
void ScriptEventOnActorScoredCriticalHit(ACDID idACDAttacker, ACDID idACDVictim)
{
    ACDIDParam tParam1(idACDAttacker);
    ACDIDParam tParam2(idACDVictim);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_ACTOR_SCORED_CRITICAL_HIT, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:2997.
void ScriptEventOnActorHealed(ACDID idACDActor, ACDID idACDSource, float flAmountHealed)
{
    ACDIDParam tParam1(idACDActor);
    ACDIDParam tParam2(idACDSource);
    FloatRangeInsideParam tParam3(flAmountHealed);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3);
    ScriptEventDispatchEvent(EVENT_ACTOR_HEALED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3011.
void ScriptEventOnActorResurrected(ACDID idACDActor)
{
    ACDIDParam tParam1(idACDActor);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_ACTOR_RESURRECTED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3021.
void ScriptEventOnActorGainedLevel(ACDID idACDActor)
{
    ACDIDParam tParam1(idACDActor);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_ACTOR_GAINED_LEVEL, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3031.
void ScriptEventOnActorUsedPower(ACDID idACDActor, SNO snoPower, ACDID idACDActorTarget)
{
    ACDIDParam tParam1(idACDActor);
    PowerParam tParam2(snoPower);
    ACDIDParam tParam3(idACDActorTarget);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3);
    ScriptEventDispatchEvent(EVENT_ACTOR_USED_POWER, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3046.
void ScriptEventOnActorSpawnedBySpawner(ACDID idACDSpawner, ACDID idACDActor)
{
    ACDIDParam tParam1(idACDSpawner);
    ACDIDParam tParam2(idACDActor);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_ACTOR_SPAWNED_BY_SPAWNER, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3059.
void ScriptEventOnActorHealthEnteredRange(ACDID idACDActor, float flPrevHitPoints, float flNewHitpoints,
                                          float flHitpointsMax)
{
    float flHitPointsPctPrev = flPrevHitPoints / flHitpointsMax * 100.0f;
    float flHitPointsPct = flNewHitpoints / flHitpointsMax * 100.0f;
    ACDIDParam tParam1(idACDActor);
    FloatRangeParam tParam2(flHitPointsPctPrev, flHitPointsPct);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_ACTOR_HEALTH_ENTERED_RANGE, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3077.
void ScriptEventOnActorReachedPathNode(ACDID idACDActor, LPCSTR szPathNodeName)
{
    ACDIDParam tParam1(idACDActor);
    StringParam tParam2(szPathNodeName);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_ACTOR_REACHED_PATH_NODE, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3090.
void ScriptEventOnActorActivated(ACDID idACDActor)
{
    ACDIDParam tParam1(idACDActor);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_ACTOR_ACTIVATED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3101.
void ScriptEventOnActorRerolledInventory(ACDID idACDActor)
{
    ACDIDParam tParam1(idACDActor);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_ACTOR_REROLLED_INVENTORY, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3116.
void ScriptEventOnConversationStarted(ACDID idACDPlayer, ACDID idACDNpc, SNO snoConv)
{
    ACDIDParam tParam1(idACDNpc);
    ACDIDParam tParam2(idACDPlayer);
    ConversationParam tParam3(snoConv);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3);
    ScriptEventDispatchEvent(EVENT_CONVERSATION_STARTED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3131.
void ScriptEventOnConversationFinished(ACDID idACDPlayer, ACDID idACDNpc, SNO snoConv)
{
    ACDIDParam tParam1(idACDNpc);
    ACDIDParam tParam2(idACDPlayer);
    ConversationParam tParam3(snoConv);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3);
    ScriptEventDispatchEvent(EVENT_CONVERSATION_FINISHED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3150.
void ScriptEventOnHeartBeat(uint32 dwGameTime)
{
    TimeParam tParam1(dwGameTime, ScriptEventVariantParam::LESS_THAN_THIS_VALUE);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_HEART_BEAT, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3160.
void ScriptEventOnTick(uint32 dwGameTime)
{
    TimeParam tParam1(dwGameTime, ScriptEventVariantParam::LESS_THAN_THIS_VALUE);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_GAME_TIME, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3174.
void ScriptEventOnPlayerEnteredGame(ACDID idACDPlayer)
{
    ACDIDParam tParam1(idACDPlayer);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_PLAYER_ENTERED_GAME, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3184.
void ScriptEventOnPlayerLeftGame(ACDID idACDPlayer)
{
    ACDIDParam tParam1(idACDPlayer);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_PLAYER_LEFT_GAME, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3194.
void ScriptEventOnPlayerEnteredWorld(ACDID idACDPlayer, SNO snoWorld)
{
    ACDIDParam tParam1(idACDPlayer);
    WorldParam tParam2(snoWorld);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_PLAYER_ENTERED_WORLD, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3205.
void ScriptEventOnPlayerEnteredLevelArea(ACDID idACDPlayer, SNO snoLevelArea)
{
    ACDIDParam tParam1(idACDPlayer);
    LevelAreaParam tParam2(snoLevelArea);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_PLAYER_ENTERED_LEVEL_AREA, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3216.
void ScriptEventOnPlayerEnteredHoradricQuestLevelArea(ACDID idACDPlayer, SNO snoLevelArea, SNO snoQuest)
{
    Act eQuestAct = QuestGetBountyAct(snoQuest);
    if (eQuestAct != -1)
    {
        CRefString sQuestName(QuestGetName(snoQuest).str());
        CRefString sActName(SymbolToName(eQuestAct, gActs));
        ACDIDParam tParam1(idACDPlayer);
        LevelAreaParam tParam2(snoLevelArea);
        QuestNameParam tParam3(sQuestName.str());
        StringParam tParam4(sActName.str());
        ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3, &tParam4);
        ScriptEventDispatchEvent(EVENT_PLAYER_ENTERED_HORADRIC_QUEST_LEVEL_AREA, &tEventDefParams, NULL, NULL, 0.0f,
                                 ACDID_INVALID);
    }
}

// ScriptEvent.cpp:3237.
void ScriptEventOnPlayerPickedUpItem(ACDID idACDPlayer, ACDID idACDItem)
{
    ACDIDParam tParam1(idACDPlayer);
    ACDIDParam tParam2(idACDItem);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_PLAYER_PICKED_UP_ITEM, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3248.
void ScriptEventOnPlayerDroppedItem(ACDID idACDPlayer, ACDID idACDItem)
{
    ACDIDParam tParam1(idACDPlayer);
    ACDIDParam tParam2(idACDItem);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_PLAYER_DROPPED_ITEM, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3259.
void ScriptEventOnPlayerKillCounter(ACDID idACDPlayer, KillCounterType eType)
{
    ACDIDParam tParam1(idACDPlayer);
    DWORDParam tParam2(eType);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_PLAYER_KILL_COUNTER, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3270.
void ScriptEventOnPlayerTownPortal(ACDID idACDPlayer)
{
    ACDIDParam tParam1(idACDPlayer);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_PLAYER_TOWN_PORTAL, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3280.
void ScriptEventOnHirelingEquippedItem(ACDID idACDHireling, ACDID idACDItem)
{
    ACDIDParam tParam1(idACDHireling);
    ACDIDParam tParam2(idACDItem);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_HIRELING_EQUIPPED_ITEM, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3291.
void ScriptEventOnLootDropped(ACDID idACDLootee, ACDID idACDLooter, ACDID idACDBestItem, int32 nNumItems)
{
    ACDIDParam tParam1(idACDLootee);
    ACDIDParam tParam2(idACDLooter);
    ACDIDParam tParam3(idACDBestItem);
    DWORDParam tParam4(nNumItems);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3, &tParam4);
    ScriptEventDispatchEvent(EVENT_LOOT_DROPPED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3304.
void ScriptEventOnTimedEventTick(SNO snoTimedEvent, uint32 dwTimeElapsed, uint32 dwTimeRemaining)
{
    TimedEventParam tParam1(snoTimedEvent);
    TimeParam tParamElapsed(dwTimeElapsed, ScriptEventVariantParam::LESS_THAN_THIS_VALUE);
    ScriptEventParamList tEventDefParamsElapsed(&tParam1, &tParamElapsed);
    ScriptEventDispatchEvent(EVENT_TIMED_EVENT_TIME_ELAPSED, &tEventDefParamsElapsed, NULL, NULL, 0.0f, ACDID_INVALID);
    TimeParam tParamRemaining(dwTimeRemaining, ScriptEventVariantParam::GREATER_THAN_THIS_VALUE);
    ScriptEventParamList tEventDefParamsRemaining(&tParam1, &tParamRemaining);
    ScriptEventDispatchEvent(EVENT_TIMED_EVENT_TIME_REMAINING, &tEventDefParamsRemaining, NULL, NULL, 0.0f,
                             ACDID_INVALID);
}

// ScriptEvent.cpp:3321.
void ScriptEventOnWorldDeleted(SNO snoWorld)
{
    WorldParam tParam1(snoWorld);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_WORLD_DELETED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3330.
void ScriptEventOnBossEncounterReset(SNO snoEncounter)
{
    BossEncounterParam tParam1(snoEncounter);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_BOSS_ENCOUNTER_RESET, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3339.
BOOL ScriptEventOnScriptedSequenceSkipped(ACDID idACDPlayer)
{
    ACDIDParam tParam1(idACDPlayer);
    ScriptEventParamList tEventDefParams(&tParam1);
    return ScriptEventDispatchEvent(EVENT_SCRIPTED_SEQUENCE_SKIPPED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3349.
void ScriptEventOnChangedAct(SNO snoAct)
{
    ScriptEventParamList tEventDefParams;
    ScriptEventDispatchEvent(EVENT_CHANGED_ACT, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3360.
void ScriptEventOnNPCVendorOperated(ACDID idACDNPC, ACDID idACDPlayer)
{
    ACDIDParam tParam1(idACDNPC);
    ACDIDParam tParam2(idACDPlayer);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_NPC_VENDOR_OPERATED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3396.
void sScriptEventTriggerForActors(const XPooledDataIDList &idACDActorsDetected, ACDID idACDTrigger,
                                  ScriptEventType eScriptEventType, BOOL &bTriggered)
{
    bTriggered = FALSE;
    ScriptEventGameGlobals *ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;
    PooledScriptEventListenerInstanceList *listListeners =
        ptScriptEventGlobals->arActiveListenerLists[eScriptEventType];
    ++ptScriptEventGlobals->nTotalEventsDispatched;
    if (!listListeners->Count())
    {
        ++ptScriptEventGlobals->nTotalEventsEasyOut;
        return;
    }
    ScriptEventAppGlobals *ptScriptEventAppGlobals = sgtAppGlobals.ptScriptEventAppGlobals;
    ScriptEventDefinition *ptScriptEventDef = ptScriptEventAppGlobals->arScriptEventDefs[eScriptEventType];
    POS posListener = listListeners->GetHeadPosition();
    while (posListener)
    {
        ScriptEventListenerInstance &tListenerInst = listListeners->GetNext(posListener);
        if (tListenerInst.dwFlags & (1 << SCRIPT_EVENT_LISTENER_FLAGGED_FOR_DELETE))
            continue;
        ScriptEventListenerDefinitionID idListenerDef = tListenerInst.idListenerDef;
        ListenerType eListenerType = tListenerInst.eListenerType;
        ++ptScriptEventGlobals->nTotalListenersConsidered;
        // Original uses this table for both listener types (86a698), unlike generic dispatch.
        ScriptEventListenerDefinition *ptListenerDef =
            ptScriptEventGlobals->arScriptEventListeners->ElementGet(idListenerDef);
        switch (eListenerType)
        {
        case LISTENER_TYPE_SCRIPT:
            if (!(ptListenerDef->dwFlags & (1 << SCRIPT_EVENT_LISTENER_IS_CONDUCTOR)))
            {
                POS posAttach = LuaScriptGetFirstAttachment(tListenerInst.idScript);
                while (posAttach)
                {
                    ScriptAttachmentType eAttachType;
                    DataID idAttachment;
                    BOOL bFlaggedForDeletion;
                    POS posCur = posAttach;
                    LuaScriptGetNextAttachment(posAttach, eAttachType, idAttachment, bFlaggedForDeletion);
                    if (bFlaggedForDeletion)
                        continue;
                    XPooledDataIDList idACDActorTriggersPassedFilter(MemoryPoolsGetDATAIDListPool());
                    POS posACDID = idACDActorsDetected.GetHeadPosition();
                    while (posACDID)
                    {
                        ACDID idACDActorDetected = (ACDID)idACDActorsDetected.GetNext(posACDID);
                        ACDIDParam tParam1(idACDTrigger);
                        ACDIDParam tParam2(idACDActorDetected);
                        ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
                        ScriptEventGameParams tGameParams;
                        tEventDefParams.ToGameParams(tGameParams);
                        if (sScriptEventListenerPassFilters(posCur, ptListenerDef, NULL, 0.0f, tGameParams,
                                                            idAttachment, tListenerInst.idScript))
                            idACDActorTriggersPassedFilter.AddTail_Tracked(idACDActorDetected);
                    }
                    if (idACDActorTriggersPassedFilter.Count() > 0)
                    {
                        bTriggered = TRUE;
                        ACDIDParam tParam1(idACDTrigger);
                        ACDIDListParam tParam2(idACDActorTriggersPassedFilter);
                        ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
                        ++ptScriptEventGlobals->nTotalListenersCalled;
                        sDispatchEventToScript(ptScriptEventDef, tListenerInst, *ptListenerDef, &tEventDefParams, NULL,
                                               eAttachType, idAttachment);
                        // In trigger path original one-shot disabling follows dispatch (3494).
                        if (ptListenerDef->dwFlags & (1 << SCRIPT_EVENT_LISTENER_DEF_ONE_SHOT))
                            LuaScriptEnableListener(posCur, ptListenerDef->nIndex, FALSE);
                    }
                }
            }
            else
            {
                ConductorInstance *ptConductor = SConductorFindConductorByScriptID(tListenerInst.idScript);
                XPooledDataIDList idACDActorTriggersPassedFilter(MemoryPoolsGetDATAIDListPool());
                POS posACDID = idACDActorsDetected.GetHeadPosition();
                while (posACDID)
                {
                    ACDID idACDActorDetected = (ACDID)idACDActorsDetected.GetNext(posACDID);
                    ACDIDParam tParam1(idACDTrigger);
                    ACDIDParam tParam2(idACDActorDetected);
                    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
                    ScriptEventGameParams tGameParams;
                    tEventDefParams.ToGameParams(tGameParams);
                    if (GameParamsPassFilter(ptListenerDef->tFilters, tGameParams, ptConductor->idAttachment,
                                             SCRIPTID_INVALID, SCRIPTINSTANCEID_INVALID))
                        idACDActorTriggersPassedFilter.AddTail_Tracked(idACDActorDetected);
                }
                if (idACDActorTriggersPassedFilter.Count() > 0)
                {
                    ++ptListenerDef->nEventCount;
                    if (ptListenerDef->nEventCount >= ptListenerDef->nEventCountRequired)
                    {
                        bTriggered = TRUE;
                        ptListenerDef->nEventCount = 0;
                        ACDIDParam tParam1(idACDTrigger);
                        ACDIDListParam tParam2(idACDActorTriggersPassedFilter);
                        ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
                        ++ptScriptEventGlobals->nTotalListenersCalled;
                        int32 nPhase;
                        int32 nSequence;
                        ConductorGetPhaseAndSequenceFromStartFunctionName(ptListenerDef->szFuncName, nPhase, nSequence);
                        uint32 nPhasePriorToScript = ptConductor->nCurrentPhase;
                        const ConductorDefinition *ptConductorDef =
                            (const ConductorDefinition *)gptGroupDefConductor->LockR(ptConductor->snoConductor, FALSE);
                        const ConductorSequence &tSequence = ptConductorDef->arPhases[nPhase].arSequences[nSequence];
                        gptGroupDefConductor->UnlockR((void *&)ptConductorDef);
                        if (!tSequence.bRepeatable)
                        {
                            if (nPhase == 0)
                            {
                                for (int32 i = 0; i < 20; ++i)
                                    if (ptConductor->arGlobalEventHandlers[i] == tListenerInst.idListenerDef)
                                        ptConductor->arGlobalEventHandlers[i] =
                                            SCRIPT_EVENT_LISTENER_DEFINITION_INVALID;
                            }
                            else
                            {
                                for (int32 i = 0; i < 20; ++i)
                                    if (ptConductor->arPhaseEventHandlers[i] == tListenerInst.idListenerDef)
                                        ptConductor->arPhaseEventHandlers[i] = SCRIPT_EVENT_LISTENER_DEFINITION_INVALID;
                            }
                            tListenerInst.dwFlags |= 1 << SCRIPT_EVENT_LISTENER_FLAGGED_FOR_DELETE;
                            ptScriptEventGlobals->dwFlags |= 1 << SCRIPT_EVENT_GAME_GLOBALS_LISTENER_FLAGGED_FOR_DELETE;
                        }
                        ScriptInstanceID idScriptInstance = sDispatchEventToConductorScript(
                            ptConductor, ptScriptEventDef, tListenerInst, *ptListenerDef, &tEventDefParams, NULL);
                        if (!tSequence.bRepeatable)
                        {
                            if (nPhase == 0)
                                ptConductor->arGlobalScripts[nSequence] = idScriptInstance;
                            else if (ptConductor->nCurrentPhase == nPhasePriorToScript)
                                ptConductor->arPhaseScripts[nSequence] = idScriptInstance;
                        }
                        else if (idScriptInstance != SCRIPTINSTANCEID_INVALID)
                        {
                            RepeatableScriptInstanceData tData;
                            tData.nPhase = nPhase;
                            tData.nSequence = nSequence;
                            tData.idScriptInstance = idScriptInstance;
                            ptConductor->tRepeatableScriptInstanceDataList.AddTail_Tracked(tData);
                        }
                    }
                }
            }
            break;
        case LISTENER_TYPE_SCRIPT_INSTANCE:
        {
            XPooledDataIDList idACDActorTriggersPassedFilter(MemoryPoolsGetDATAIDListPool());
            POS posACDID = idACDActorsDetected.GetHeadPosition();
            while (posACDID)
            {
                ACDID idACDActorDetected = (ACDID)idACDActorsDetected.GetNext(posACDID);
                ACDIDParam tParam1(idACDTrigger);
                ACDIDParam tParam2(idACDActorDetected);
                ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
                ScriptEventGameParams tGameParams;
                tEventDefParams.ToGameParams(tGameParams);
                if (sScriptInstancePassFilters(ptListenerDef->tFilters, tGameParams, -1, SCRIPTID_INVALID,
                                               tListenerInst.idScriptInstance))
                    idACDActorTriggersPassedFilter.AddTail_Tracked(idACDActorDetected);
            }
            if (idACDActorTriggersPassedFilter.Count() > 0)
            {
                bTriggered = TRUE;
                ACDIDParam tParam1(idACDTrigger);
                ACDIDListParam tParam2(idACDActorTriggersPassedFilter);
                ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
                ++ptScriptEventGlobals->nTotalInstancesNotified;
                LuaScriptInstanceNotifyEvent(tListenerInst.idScriptInstance, idListenerDef, eScriptEventType,
                                             &tEventDefParams);
            }
            break;
        }
        default:
            break;
        }
    }
}

// ScriptEvent.cpp:3685.
void ScriptEventOnActorEnteredTrigger(ACDID idACDActorDetected, ACDID idACDTrigger, BOOL &bTriggeredScriptEvent)
{
    ACDIDParam tParam1(idACDTrigger);
    ACDIDParam tParam2(idACDActorDetected);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    bTriggeredScriptEvent =
        ScriptEventDispatchEvent(EVENT_TRIGGER_ON_ACTOR_ENTERED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3697.
void ScriptEventOnActorExitedTrigger(ACDID idACDActorDetected, ACDID idACDTrigger, BOOL &bTriggeredScriptEvent)
{
    ACDIDParam tParam1(idACDTrigger);
    ACDIDParam tParam2(idACDActorDetected);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    bTriggeredScriptEvent =
        ScriptEventDispatchEvent(EVENT_TRIGGER_ON_ACTOR_EXITED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3709.
void ScriptEventOnTriggerPulseActorsInBounds(const XPooledDataIDList &idACDActorsDetected, ACDID idACDTrigger,
                                             BOOL &bTriggeredScriptEvent)
{
    sScriptEventTriggerForActors(idACDActorsDetected, idACDTrigger, EVENT_TRIGGER_PULSE_ACTORS_IN_BOUNDS,
                                 bTriggeredScriptEvent);
}

// ScriptEvent.cpp:3717.
void ScriptEventOnPlayerInRangeOfActor(ACDID idACDPlayer, ACDID idACDActor, float fDistance)
{
    ACDIDParam tParam1(idACDActor);
    ACDIDParam tParam2(idACDPlayer);
    FloatRangeInsideParam tParam3(fDistance);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3);
    ScriptEventDispatchEvent(EVENT_PLAYER_IN_RANGE_OF_ACTOR, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3735.
void ScriptEventOnGizmoOperated(ACDID idACDGizmo, ACDID idACDPlayer)
{
    ACDIDParam tParam1(idACDGizmo);
    ACDIDParam tParam2(idACDPlayer);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_GIZMO_OPERATED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3748.
BOOL ScriptEventOnGizmoCanOperate(ACDID idACDGizmo, ACDID idACDPlayer)
{
    ACDIDParam tParam1(idACDGizmo);
    ACDIDParam tParam2(idACDPlayer);
    ScriptEventReturnValueBool tReturnBoolParam(TRUE);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_GIZMO_CAN_OPERATE, &tEventDefParams, &tReturnBoolParam, NULL, 0.0f, ACDID_INVALID);
    return tReturnBoolParam.m_bValue;
}

// ScriptEvent.cpp:3764.
void ScriptEventOnGateGizmoOpened(ACDID idACDGizmo)
{
    ACDIDParam tParam1(idACDGizmo);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_GATE_GIZMO_OPENED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3775.
void ScriptEventOnDoorCompletelyOpened(ACDID idACDDoor)
{
    ACDIDParam tParam1(idACDDoor);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_DOOR_COMPLETELY_OPENED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3790.
void ScriptEventOnHelperAttached(ACDID idACDHelper, ACDID idACDPlayer)
{
    ACDIDParam tParam1(idACDHelper);
    ACDIDParam tParam2(idACDPlayer);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_HELPER_ATTACHED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3803.
void ScriptEventOnHelperDismissed(ACDID idACDHelper, ACDID idACDPlayer)
{
    ACDIDParam tParam1(idACDHelper);
    ACDIDParam tParam2(idACDPlayer);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_HELPER_DISMISSED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3816.
void ScriptEventOnHelperGuardObject(ACDID idACDHelper, ACDID idACDObject)
{
    ACDIDParam tParam1(idACDHelper);
    ACDIDParam tParam2(idACDObject);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_HELPER_GUARD_OBJECT, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3833.
void ScriptEventOnHirelingLearnedSkill(ACDID idACDHireling, SNO snoPower)
{
    ACDIDParam tParam1(idACDHireling);
    PowerParam tParam2(snoPower);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_ACTOR_HIRELING_LEARNED_SKILL, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3849.
void ScriptEventOnUserEvent(LPCSTR szUserEvent, ACDID idACD, int32 nUserParam)
{
    StringParam tParam1(szUserEvent);
    ACDIDParam tParam2(idACD);
    DWORDParam tParam3(nUserParam);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3);
    ActorCommonData *ptACD = ACDTryToGet(idACD);
    if (!(SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_TRACE_SCRIPT_EVENTS)))
        AppDrawFlagIsSet(APP_DRAW_DEBUG_QUESTS_BIT);
    ScriptEventDispatchEvent(EVENT_USER_EVENT, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
    SQuestsOnEventSend(szUserEvent, idACD);
}

// ScriptEvent.cpp:3875.
void ScriptEventOnUserEventInRange(LPCSTR szUserEvent, ACDID idACD, float flRange, int32 nUserParam)
{
    ActorCommonData &tACD = ACDGet(idACD);
    StringParam tParam1(szUserEvent);
    ACDIDParam tParam2(idACD);
    DWORDParam tParam3(nUserParam);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3);
    WorldPlace tPlace;
    tPlace.idSWorld = tACD.tLocation.tWorldLocation.idSWorld;
    tPlace.wp = tACD.tLocation.tWorldLocation.transform.wp;
    if (!(SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_TRACE_SCRIPT_EVENTS)))
        AppDrawFlagIsSet(APP_DRAW_DEBUG_QUESTS_BIT);
    ScriptEventDispatchEvent(EVENT_USER_EVENT, &tEventDefParams, NULL, &tPlace, flRange, ACDID_INVALID);
}

// ScriptEvent.cpp:3903.
void ScriptEventOnUserEventSentToActor(LPCSTR szUserEvent, ACDID idACDSource, ACDID idACDTarget, int32 nUserParam)
{
    StringParam tParam1(szUserEvent);
    ACDIDParam tParam2(idACDSource);
    DWORDParam tParam3(nUserParam);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3);
    ActorCommonData *ptACDSource = ACDTryToGet(idACDSource);
    ActorCommonData *ptACDTarget = ACDTryToGet(idACDTarget);
    if (!(SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_TRACE_SCRIPT_EVENTS)))
        AppDrawFlagIsSet(APP_DRAW_DEBUG_QUESTS_BIT);
    ScriptEventDispatchEvent(EVENT_USER_EVENT, &tEventDefParams, NULL, NULL, 0.0f, idACDTarget);
}

// ScriptEvent.cpp:3935.
void ScriptEventOnQuestStepAssigned(LPCSTR szQuest, LPCSTR szStep, ACDID idACDPlayer)
{
    QuestNameParam tParam1(szQuest);
    StringParam tParam2(szStep);
    ACDIDParam tParam3(idACDPlayer);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3);
    ScriptEventDispatchEvent(EVENT_QUEST_STEP_ASSIGNED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3950.
void ScriptEventOnQuestStepCompleted(LPCSTR szQuest, LPCSTR szStep, ACDID idACDPlayer)
{
    QuestNameParam tParam1(szQuest);
    StringParam tParam2(szStep);
    ACDIDParam tParam3(idACDPlayer);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3);
    ScriptEventDispatchEvent(EVENT_QUEST_STEP_COMPLETED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3965.
void ScriptEventOnQuestObjectiveProgressed(LPCSTR szQuest, LPCSTR szStep, ACDID idACDPlayer, int32 nCountLeft)
{
    QuestNameParam tParam1(szQuest);
    StringParam tParam2(szStep);
    ACDIDParam tParam3(idACDPlayer);
    DWORDParam tParam4(nCountLeft);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3, &tParam4);
    ScriptEventDispatchEvent(EVENT_QUEST_OBJECTIVE_PROGRESSED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3982.
void ScriptEventOnQuestObjectiveCompleted(LPCSTR szQuest, LPCSTR szStep, int32 nObjectiveIndex)
{
    QuestNameParam tParam1(szQuest);
    StringParam tParam2(szStep);
    DWORDParam tParam3(nObjectiveIndex + 1);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3);
    ScriptEventDispatchEvent(EVENT_QUEST_OBJECTIVE_COMPLETED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:3997.
void ScriptEventOnQuestRangeChanged(SNO snoQuest, int32 nPrevStepUID, int32 nNextStepUID)
{
    QuestTime tStart = {snoQuest, nPrevStepUID};
    QuestTime tEnd = {snoQuest, nNextStepUID};
    QuestRangeParam tParam1(tStart, tEnd);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_QUESTRANGE_CHANGED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:4015.
void ScriptEventOnQuestTimerRunningOut(LPCSTR szQuest, LPCSTR szStep, uint32 dwTimeRemaining)
{
    uint32 dwGameTime = GameGetActiveGameCommonData().dwServerTime;
    QuestNameParam tParam1(szQuest);
    StringParam tParam2(szStep);
    TimeParam tParam3(dwTimeRemaining, ScriptEventVariantParam::GREATER_THAN_THIS_VALUE);
    TimeParam tParam4(dwGameTime, ScriptEventVariantParam::LESS_THAN_THIS_VALUE);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2, &tParam3, &tParam4);
    ScriptEventDispatchEvent(EVENT_QUEST_TIMER_RUNNING_OUT, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:4033.
void ScriptEventOnQuestCompleted(LPCSTR szQuest)
{
    QuestNameParam tParam1(szQuest);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_QUEST_COMPLETED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:4045.
void ScriptEventOnAIQueueEmpty(ACDID idACDActor)
{
    ACDIDParam tParam1(idACDActor);
    ScriptEventParamList tEventDefParams(&tParam1);
    ScriptEventDispatchEvent(EVENT_AIQUEUE_EMPTY, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:4056.
void ScriptEventOnAIQueuedActionComplete(ACDID idACDActor, AIQueuedActionID idQActionID)
{
    ACDIDParam tParam1(idACDActor);
    DWORDParam tParam2(idQActionID);
    ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
    ScriptEventDispatchEvent(EVENT_AIQUEUE_ACTION_COMPLETE, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
}

// ScriptEvent.cpp:4069.
BOOL sIsScriptEventTypeTargetingActor(ACDID idActor, ScriptEventType eEventType, int32 nFilterIndex)
{
    ScriptEventGameGlobals *ptScriptEventGlobals = SGameManager::GameGet().ptScriptEventGameGlobals;
    PooledScriptEventListenerInstanceList *listListeners = ptScriptEventGlobals->arActiveListenerLists[eEventType];
    POS pos = listListeners->GetHeadPosition();
    while (pos)
    {
        ScriptEventListenerInstance &tListenerInst = listListeners->GetNext(pos);
        if (tListenerInst.dwFlags & (1 << SCRIPT_EVENT_LISTENER_FLAGGED_FOR_DELETE))
            continue;
        ListenerType eListenerType = tListenerInst.eListenerType;
        ScriptEventListenerDefinitionID idListenerDef = tListenerInst.idListenerDef;
        ++ptScriptEventGlobals->nTotalListenersConsidered;
        ScriptEventListenerDefinition *ptListenerDef;
        switch (eListenerType)
        {
        case LISTENER_TYPE_SCRIPT:
            ptListenerDef = ptScriptEventGlobals->arScriptEventListeners->ElementGet(idListenerDef);
            if (!(ptListenerDef->dwFlags & (1 << SCRIPT_EVENT_LISTENER_IS_CONDUCTOR)))
            {
                POS posAttachment = LuaScriptGetFirstAttachment(tListenerInst.idScript);
                while (posAttachment)
                {
                    ScriptEventVariantParam &tParamFilter = ptListenerDef->tFilters.arParams[nFilterIndex];
                    ScriptAttachmentType eAttachType;
                    DataID idAttachment;
                    BOOL bFlaggedForDeletion;
                    LuaScriptGetNextAttachment(posAttachment, eAttachType, idAttachment, bFlaggedForDeletion);
                    if (bFlaggedForDeletion)
                        continue;
                    ACDIDParam tParam1(idActor);
                    ScriptEventParamList tEventDefParams(&tParam1);
                    ScriptEventGameParams tGameParams;
                    tEventDefParams.ToGameParams(tGameParams);
                    ScriptEventVariantParam &tParamGame = tGameParams.arGameParams[0][tParamFilter.eParamType];
                    if (tParamFilter.FilterAgainst(tParamGame, idAttachment, tListenerInst.idScript,
                                                   SCRIPTINSTANCEID_INVALID))
                        return TRUE;
                }
            }
            else
            {
                ConductorInstance *ptConductor = SConductorFindConductorByScriptID(tListenerInst.idScript);
                ACDIDParam tParam1(idActor);
                ScriptEventParamList tEventDefParams(&tParam1);
                ScriptEventGameParams tGameParams;
                tEventDefParams.ToGameParams(tGameParams);
                ScriptEventVariantParam &tParamFilter = ptListenerDef->tFilters.arParams[nFilterIndex];
                ScriptEventVariantParam &tParamGame = tGameParams.arGameParams[0][tParamFilter.eParamType];
                if (tParamFilter.FilterAgainst(tParamGame, ptConductor->idAttachment, SCRIPTID_INVALID,
                                               SCRIPTINSTANCEID_INVALID))
                    return TRUE;
            }
            break;
        case LISTENER_TYPE_SCRIPT_INSTANCE:
            ptListenerDef = ptScriptEventGlobals->arScriptEventWaitListeners->ElementTryToGet(idListenerDef);
            if (!ptListenerDef)
            {
                SNO snoLuaScript = LuaScriptInstanceTryToGetSNO(tListenerInst.idScriptInstance);
                if (snoLuaScript != -1)
                {
                    ScopedScriptDefinitionLockR pDef(snoLuaScript);
                }
                continue;
            }
            {
                ACDIDParam tParam1(idActor);
                ScriptEventParamList tEventDefParams(&tParam1);
                ScriptEventGameParams tGameParams;
                tEventDefParams.ToGameParams(tGameParams);
                ScriptEventVariantParam &tParamFilter = ptListenerDef->tFilters.arParams[nFilterIndex];
                ScriptEventVariantParam &tParamGame = tGameParams.arGameParams[0][tParamFilter.eParamType];
                if (tParamFilter.FilterAgainst(tParamGame, -1, SCRIPTID_INVALID, tListenerInst.idScriptInstance))
                    return TRUE;
            }
            break;
        default:
            break;
        }
    }
    return FALSE;
}

// ScriptEvent.cpp:4203.
BOOL IsScriptTriggerEventTargetingActor(ACDID idActor)
{
    return sIsScriptEventTypeTargetingActor(idActor, EVENT_TRIGGER_ON_ACTOR_ENTERED, 1) ||
           sIsScriptEventTypeTargetingActor(idActor, EVENT_TRIGGER_ON_ACTOR_EXITED, 1) ||
           sIsScriptEventTypeTargetingActor(idActor, EVENT_TRIGGER_PULSE_ACTORS_IN_BOUNDS, 1) ||
           sIsScriptEventTypeTargetingActor(idActor, EVENT_PLAYER_IN_RANGE_OF_ACTOR, 0);
}
