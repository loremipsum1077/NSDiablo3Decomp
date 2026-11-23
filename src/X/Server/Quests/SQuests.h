// Adopted from the DWARF reference; hand maintained.
// Original file: X/Server/Quests/SQuests.h

#pragma once

#include "../../Common/CommonTypes.h"
#include "../../Common/GameSwig.h"

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 InventorySlotType;


#include "../../Sigma/Unicode.h"
void QuestMeterTrackActorHitpoints(LPCSTR szQuestName, ACDID idActor);  // SQuests.cpp:4868
void HoradricQuestSetColorOrbs(Act eAct, BOOL bRed, BOOL bGreen, BOOL bBlue);  // SQuests.cpp:4904
void QuestMeterSetAmount(LPCSTR szQuestName, float flPercent); // SQuests.cpp:4886

void SQuestsOnEventSend(LPCSTR szEventName, ACDID idACD);  // SQuests.cpp:4825
void SQuestsOnTimedEventExpired(SNO snoTimedEvent);  // SQuests.cpp:4801
void SQuestsOnTimedEventAboutToExpire(SNO snoTimedEvent, uint32 dwTicksRemaining);  // SQuests.cpp:4812
struct QuestInstance;
void SQuestsOnQuestActivated(const QuestInstance & tQuestInstance);  // SQuests.cpp:4838
struct Player;
void SQuestsPlayerLeaveGame(Player & tPlayer);  // SQuests.cpp:521
BOOL SQuestStateIsExploitable();  // SQuests.cpp:5090

void SQuestsOnPlayerAcquireItem(ACDID idACDPlayer, ACDID idACDItem);

void SQuestsOnHQCursedRealmReagentCollected(ACDID idACDPlayer, int32 nActIndex);  // SQuests.cpp:4773

SNO SQuestGetBountyQuestSNOWithActorObjective(SNO snoActor);  // SQuests.cpp:5583
void SQuestsRegisterBountyActor(SNO snoActor, SNO snoWorld, SNO snoLevelArea, SNO snoQuest);  // SQuests.cpp:5739

void SQuestEvaluateGoldRewardedAchievementEvent(const Player & tPlayer, uint32 nGoldGranted);  // SQuests.cpp:2485

namespace D3 { namespace Quests { class QuestReward; } }
void SQuestsSendTieredRiftReward(const Player & tDestPlayer, const BOOL bFinishedInTime, const int32 nRiftDepth, const D3::Quests::QuestReward & tReward);  // SQuests.cpp:798

void SQuestsSendQuestReward(const Player & tDestPlayer, const BOOL bIsComplete, const D3::Quests::QuestReward & tReward, const BOOL bEquipped);  // SQuests.cpp:772

struct ActorCommonData;
void SQuestsOnActorRemoved(ActorCommonData & tACDActor);  // SQuests.cpp:4606
