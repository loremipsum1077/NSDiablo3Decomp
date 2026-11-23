// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/LuaScript/ScriptEvent.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 ScriptID;

typedef int32 ScriptEventType;

typedef int32 ScriptInstanceID;

typedef int32 LevelAreaID;

typedef int32 AIQueuedActionID;

typedef int32 KillCounterType;

enum {
    SCRIPT_EVENT_LISTENER_DEFINITION_INVALID = -1,
};

typedef int32 ScriptEventListenerDefinitionID;

enum {
    SCRIPT_EVENT_WAIT_LISTENER_INVALID = -1,
};

typedef int32 ScriptEventWaitListenerID;


#include "ScriptEventSwig.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/Unicode.h"
struct ScriptEventParamList;
struct ScriptEventReturnValue;
struct WorldPlace;
struct ScriptEventFilters;
struct lua_State;
void ScriptEventInitializeForApp();
void ScriptEventDisposeForApp();
void ScriptEventInitializeForGame();
void ScriptEventDisposeForGame();
void ScriptEventUpdate();
BOOL ScriptEventDispatchEvent(ScriptEventType eEventType, ScriptEventParamList * ptEventDefParams, ScriptEventReturnValue * ptReturnValue, WorldPlace * ptWorldPlace, float flRange, ACDID idACDTarget);
ScriptEventListenerDefinitionID ScriptEventAddListener(ScriptID idScript, LPCSTR szHandlerFuncName, ScriptEventType eEventType, ScriptEventFilters & tFilters, uint32 dwFlags, int32 nIndex, int32 nEventCount);
ScriptEventWaitListenerID ScriptEventAddGameWaitListener(ScriptInstanceID idScriptInstance, ScriptEventType eEventType, ScriptEventFilters & tFilters);
BOOL ScriptEventParseEventFilters(lua_State * ptLuaState, ScriptEventType eEventType, ScriptEventFilters & tFilters);  // ScriptEvent.cpp:2911
void ScriptEventActivateListener(ScriptEventListenerDefinitionID idListenerDef, ScriptID idScript);
void ScriptEventDeActivateListener(ScriptEventListenerDefinitionID idListenerDef);
void ScriptEventRemoveListener(ScriptEventListenerDefinitionID idListenerDef);
void ScriptEventRemoveGameWaitListener(ScriptEventWaitListenerID idListenerDef);
void ScriptEventOnGroupKilled(uint32 uGroup, ACDID idACDKiller);
void ScriptEventOnActorKilled(ACDID idACDActor, ACDID idACDKiller);
void ScriptEventOnActorDamaged(ACDID idACDActor, ACDID idACDDamager);
void ScriptEventOnActorSighted(ACDID idACDActor, ACDID idACDTarget);
void ScriptEventOnActorScoredCriticalHit(ACDID idACDAttacker, ACDID idACDVictim);
void ScriptEventOnActorHealed(ACDID idACDActor, ACDID idACDSource, float flAmountHealed);
void ScriptEventOnActorResurrected(ACDID idACDActor);
void ScriptEventOnActorGainedLevel(ACDID idACDActor);
void ScriptEventOnActorUsedPower(ACDID idACDActor, SNO snoPower, ACDID idACDActorTarget);
void ScriptEventOnActorSpawnedBySpawner(ACDID idACDSpawner, ACDID idACDActor);
void ScriptEventOnActorHealthEnteredRange(ACDID idACDActor, float flPrevHitPoints, float flNewHitpoints, float flHitpointsMax);
void ScriptEventOnActorReachedPathNode(ACDID idACDActor, LPCSTR szPathNodeName);
void ScriptEventOnActorActivated(ACDID idACDActor);
void ScriptEventOnActorRerolledInventory(ACDID idACDActor);
void ScriptEventOnConversationStarted(ACDID idACDPlayer, ACDID idACDNpc, SNO snoConv);
void ScriptEventOnConversationFinished(ACDID idACDPlayer, ACDID idACDNpc, SNO snoConv);
void ScriptEventOnHeartBeat(uint32 dwGameTime);
void ScriptEventOnTick(uint32 dwGameTime);
void ScriptEventOnPlayerEnteredGame(ACDID idACDPlayer);
void ScriptEventOnPlayerLeftGame(ACDID idACDPlayer);
void ScriptEventOnPlayerEnteredWorld(ACDID idACDPlayer, SNO snoWorld);
void ScriptEventOnPlayerEnteredLevelArea(ACDID idACDPlayer, SNO snoLevelArea);
void ScriptEventOnPlayerEnteredHoradricQuestLevelArea(ACDID idACDPlayer, SNO snoLevelArea, SNO snoQuest);
void ScriptEventOnPlayerPickedUpItem(ACDID idACDPlayer, ACDID idACDItem);
void ScriptEventOnPlayerDroppedItem(ACDID idACDPlayer, ACDID idACDItem);
void ScriptEventOnPlayerKillCounter(ACDID idACDPlayer, KillCounterType eType);
void ScriptEventOnPlayerTownPortal(ACDID idACDPlayer);
void ScriptEventOnHirelingEquippedItem(ACDID idACDHireling, ACDID idACDItem);
void ScriptEventOnLootDropped(ACDID idACDLootee, ACDID idACDLooter, ACDID idACDBestItem, int32 nNumItems);
void ScriptEventOnTimedEventTick(SNO snoTimedEvent, uint32 dwTimeElapsed, uint32 dwTimeRemaining);
void ScriptEventOnWorldDeleted(SNO snoWorld);
void ScriptEventOnBossEncounterReset(SNO snoEncounter);
BOOL ScriptEventOnScriptedSequenceSkipped(ACDID idACDPlayer);
void ScriptEventOnChangedAct(SNO snoAct);
void ScriptEventOnNPCVendorOperated(ACDID idACDNPC, ACDID idACDPlayer);
void ScriptEventOnActorEnteredTrigger(ACDID idACDActorDetected, ACDID idACDTrigger, BOOL & bTriggeredScriptEvent);
void ScriptEventOnActorExitedTrigger(ACDID idACDActorDetected, ACDID idACDTrigger, BOOL & bTriggeredScriptEvent);
void ScriptEventOnPlayerInRangeOfActor(ACDID idACDPlayer, ACDID idACDActor, float fDistance);
void ScriptEventOnGizmoOperated(ACDID idACDGizmo, ACDID idACDPlayer);
BOOL ScriptEventOnGizmoCanOperate(ACDID idACDGizmo, ACDID idACDPlayer);
void ScriptEventOnGateGizmoOpened(ACDID idACDGizmo);
void ScriptEventOnDoorCompletelyOpened(ACDID idACDDoor);
void ScriptEventOnHelperAttached(ACDID idACDHelper, ACDID idACDPlayer);
void ScriptEventOnHelperDismissed(ACDID idACDHelper, ACDID idACDPlayer);
void ScriptEventOnHelperGuardObject(ACDID idACDHelper, ACDID idACDObject);
void ScriptEventOnHirelingLearnedSkill(ACDID idACDHireling, SNO snoPower);
void ScriptEventOnUserEvent(LPCSTR szUserEvent, ACDID idACD, int32 nUserParam);
void ScriptEventOnUserEventInRange(LPCSTR szUserEvent, ACDID idACD, float flRange, int32 nUserParam);
void ScriptEventOnUserEventSentToActor(LPCSTR szUserEvent, ACDID idACDSource, ACDID idACDTarget, int32 nUserParam);
void ScriptEventOnQuestStepAssigned(LPCSTR szQuest, LPCSTR szStep, ACDID idACDPlayer);
void ScriptEventOnQuestStepCompleted(LPCSTR szQuest, LPCSTR szStep, ACDID idACDPlayer);
void ScriptEventOnQuestObjectiveProgressed(LPCSTR szQuest, LPCSTR szStep, ACDID idACDPlayer, int32 nCountLeft);
void ScriptEventOnQuestObjectiveCompleted(LPCSTR szQuest, LPCSTR szStep, int32 nObjectiveIndex);
void ScriptEventOnQuestRangeChanged(SNO snoQuest, int32 nPrevStepUID, int32 nNextStepUID);
void ScriptEventOnQuestTimerRunningOut(LPCSTR szQuest, LPCSTR szStep, uint32 dwTimeRemaining);
void ScriptEventOnQuestCompleted(LPCSTR szQuest);
void ScriptEventOnAIQueueEmpty(ACDID idACDActor);
void ScriptEventOnAIQueuedActionComplete(ACDID idACDActor, AIQueuedActionID idQActionID);
