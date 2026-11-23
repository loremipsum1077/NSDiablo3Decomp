// Original file: X/Client/CHero.h
#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"

typedef int32 CrafterType;

typedef int32 MonsterRarity;

typedef int32 SNO;  // X/Common/SNO/SNOFiles.h
typedef int32 GBID;  // X/Common/GameBalance.h

struct Player;
struct GenericBlobMessage;
struct PlayerSavedQuestUpdateMessage;
struct PlayerQuestHistoryUpdateMessage;
struct PlayerQuestRewardHistoryUpdateMessage;
struct PlayerSavedConversationsUpdateMessage;
struct ConsoleCounterIncrementMessage;
class CRefString;
namespace D3 { namespace Account { class SavedDefinition; class ConsoleChallengeRiftReward; } }
namespace D3 { namespace Hero { class SavedDefinition; } }
namespace D3 { namespace GameMessage { class AchievementsAwardedConsole; class AchievementsCriteriaUpdateConsole; } }

namespace CHero {
void InitializeForGame();
void DisposeForGame();
void Save(int32 nControllerIndex, uint32 uReason);
BOOL Validate(int32 nControllerIndex);
void Synchronize(int32 nControllerIndex);
void sComputeProgress(D3::Account::SavedDefinition & tAccountDefinition, D3::Hero::SavedDefinition & tHeroDefinition);
void OnSavedQuestUpdate(Player * ptPlayer, const PlayerSavedQuestUpdateMessage & tUpdateMsg);
void OnQuestHistoryUpdate(Player * ptPlayer, const PlayerQuestHistoryUpdateMessage & tUpdateMsg);
void OnQuestRewardHistoryUpdate(Player * ptPlayer, const PlayerQuestRewardHistoryUpdateMessage & tUpdateMsg);
void OnSavedConversationUpdate(Player * ptPlayer, const PlayerSavedConversationsUpdateMessage & tUpdateMsg);
void OnAchievementsAwarded(Player * ptPlayer, const D3::GameMessage::AchievementsAwardedConsole & tUpdateMsg);
void OnAchievementCriteriaUpdate(int32 nControllerIndex, const D3::GameMessage::AchievementsCriteriaUpdateConsole & tUpdateMsg);
void OnGameFinished(Player * ptPlayer);
void OnAdventureModeUnlocked(Player * ptPlayer);
void OnAccountFlagsSync(Player * ptPlayer, uint32 uAccountFlags);
void OnCounterIncrement(Player * ptPlayer, const ConsoleCounterIncrementMessage & tIncrementMsg);
void SyncRandomSeeds(Player * ptPlayer, GenericBlobMessage * ptMessage);
void SyncRandomCubeSeeds(Player * ptPlayer, GenericBlobMessage * ptMessage);
void OnCharacterKilled(Player * ptPlayer, SNO snoKiller, MonsterRarity eKillerRarity, const GBID * pRareNameGBIDs, const CRefString & usKiller);
void OnChallengeRiftComplete(int32 nUserIndex, uint64 idLeaderboard, uint32 uRiftNumber, uint64 uScore);
void OnChallengeRiftRewardEarned(int32 nUserIndex, D3::Account::ConsoleChallengeRiftReward tReward);
void OnChallengeRiftRewardClaimed(int32 nUserIndex, uint32 uRewardId);
}  // namespace CHero
