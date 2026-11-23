// Declarations for X/Client/UserInterface/Interface/UIConversations.cpp. The original header
// carries no types of its own in the DWARF; signatures are from the definitions.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/SNO/SNOFiles.h"

struct UIID;
struct Player;
class ActorCommonData;
struct PlayLineParams;

namespace UIConversation {
BOOL RActorHasTalkBubble(const RActorID idRActor);
void InitializeForGame();
void DisposeForGame();
void PlayLine(const ACDID idACDSpeaker, const ACDID * pidACDParticipants, const PlayLineParams & tParams, const uint32 dwTimeToDisplay);
void StopLine(const int32 nLineGUID, const BOOL fStoppedByUser);
void EndGameConversation(const UIID &);
void ActTransitionConversation(const UIID &);
void EndConversation(const SNO snoConversation, const ACDNetworkName annSpeaker, const int32 nPlayerTalkingIndex);
void Advance(const UIID &, const Player * ptPlayer);
void ConsoleAdvance(const UIID & id, const int32 nControllerIndex);
void ConsoleCancel(const UIID & id, const int32 nControllerIndex);
void SendCancelRequest();
BOOL CanCancel();
void Cancel(const UIID &);
void Update();
BOOL IsImportantConversationPlaying();
BOOL IsUIBlockingConversationPlaying();
BOOL ACDIsInConversation(const ACDID idACD, BOOL bConvDialogOnly, BOOL bUnusableOnly);
void Pause(BOOL fPause);
BOOL IsCancelButtonOnCooldown();
BOOL IsSkipAllButtonOnCooldown();
void CancelLineForRActor(const RActorID idRActor);
void OnWorldLeave();
void ReattachConversationLinesForSpeaker(ActorCommonData & tACDSpeaker);
}  // namespace UIConversation
