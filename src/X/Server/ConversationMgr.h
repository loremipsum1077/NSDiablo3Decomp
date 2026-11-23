// Original file: X/Server/ConversationMgr.h
// Subset written for X/Server/ConversationMgr.cpp.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/CommonTypes.h"
#include "../Common/SNO/SNOFiles.h"

struct Player;
struct ActorCommonData;
struct ActiveConversation;

namespace ConversationMgr {
void InitializeForGame();
void DisposeForGame();
BOOL StartConversation(const int32 nPlayerTalkingIndex, const SNO snoConversation, const ACDID idACDTalkingTo, const ACDID idACDAlt1, const ACDID idACDAlt2, const ACDID idACDAlt3, const ACDID idACDAlt4, const int32 nPriority, const BOOL bForceInterrupt, const BOOL bPlayerInitiated);
BOOL ConversationHasAllParticipants(const SNO snoConversation, const ActorCommonData * ptACDPlayer, const ActorCommonData * ptACDNpc);
void Update();
void CancelConversation(const SNO snoConv);
BOOL ConversationIsActive(const SNO snoConv);
BOOL ACDIsUnusableDuringConversation(const ACDID idACD, const Player * ptPlayer);
BOOL ACDIsInActiveConversation(const ACDID idACD, const Player * ptPlayer);
BOOL PlayerOptOutOfAllConversations(const Player & tPlayer, ActiveConversation * ptConv);
void UpdateIdleForActor(ACDID idACD, const uint32 dwAnimTag);
void AdvanceConversationForPlayer(const Player & tPlayer, SNO snoConv, int32 nLineUID);
void UpdateConversationAutoAdvanceForPlayer(const Player & tPlayer, SNO snoConv, int32 nLineUID, uint32 dwNextAdvanceTime);
void CancelConversationForPlayer(const Player & tPlayer);
BOOL PlayerIsInQuestConversation(const Player & tPlayer);
void EvaluateConversationConditions(const ACDID idACDTalkingTo, const ACDID idACDPlayer, const ACDID * idACDParticipants, const SNO snoConversation, uint32 & dwConditions, uint32 & dwCheckConditions);
void MarkSavedConversationsAsHeard(Player & tPlayer);
void CancelConversationsForDeadParticipant(const ACDID idACD);
}  // namespace ConversationMgr
