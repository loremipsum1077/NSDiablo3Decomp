// Not a file the DWARF records: SetEncounter.h holds no types, so the reference tree has no copy.
// It declares the SetEncounterManager functions other files call; their definitions and the
// SetEncounterState class are in SetEncounter.cpp.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/World.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Timers.h"

struct Player;

namespace SetEncounterManager {

ACDID GetOwner();
void SendEncounterInviteStateMessage();
BOOL IsInviteInProgress();
void SendCancelMessage(TimerID, DataID, void *);
void HandleConditionalStart();
void TryStartSetDungeon();
void PortPlayer(Player * ptPlayer, PlayerWarpReason eReason);
void InviteTimeOutCallback(TimerID, DataID, void *);
void PromptAllOtherPlayers(const Player * ptInstigator);
void StartSetDungeon();
void PlayerAcceptStart(Player * ptPlayer);
void PlayerVoteCancel(Player * ptPlayer);
void PlayerVoteContinue(Player * ptPlayer);
void PlayerRemoved(Player * ptPlayer, BOOL bPlayerWasDeleted);
BOOL IsAnyEncounterActive();
BOOL IsInProgress();
SWorldID GetActiveSetDungeonWorld();
void PlayerAcceptInvite(Player * ptPlayer);
void PlayerDeclineInvite(Player * ptPlayer);
BOOL CanEnterSetDungeon(Player * ptPlayer);
void SetDungeonClosing(BOOL bClosing);
BOOL IsDungeonClosing();
void EncounterStateDestroyForGame();
void EncounterStateCreateForGame();
void Cleanup();
int32 GetNumberOfParticipatingPlayers();
BOOL IsWaitingForPlayerToVote(Player * ptPlayer);

}  // namespace SetEncounterManager
