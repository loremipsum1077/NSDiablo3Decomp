// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/System/SNephalemRift.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/NephalemRiftCommon.h"
#include "../../Common/OnlineService/OnlineService.h"

struct Player;

namespace NephalemRiftManager {

static const LPCSTR ALL_PLAYERS_CONSUMED_RIFT_REWARDS_EVENT = "AllPlayersConsumedRiftRewards";  // line 28

static const LPCSTR LOOT_RUN_ORPHANED_EVENT = "LootRunIsOrphaned";  // line 32

struct GreaterRiftRewardState {  // size 0x14
    NephalemRift::RIFT_REWARD_CHOICE_STATE eChoiceState;  // 0x00
    BOOL bSuccess;  // 0x04
    int32 nJewelUpgradesMax;  // 0x08
    int32 nJewelUpgradesUsed;  // 0x0c
    int32 nJewelUpgradesBonus;  // 0x10
};
static_assert(sizeof(GreaterRiftRewardState) == 0x14, "GreaterRiftRewardState");

struct GreaterRiftPlayerState {  // size 0x28
    int32 nDeathCount;  // 0x00
    uint64 uLeaverGearAndSkillHash;  // 0x08
    OnlineService::GameAccountHandle tGameAccount;  // 0x10
    BOOL bHasNewSoloRecord;  // 0x1c
    BOOL bEmpoweredRift;  // 0x20
};
static_assert(sizeof(GreaterRiftPlayerState) == 0x28, "GreaterRiftPlayerState");

void InitiateRift(ACDID idObeliskGizmo, Player * ptPlayer);
void TriggerRift(int32 nRiftTier, Player * ptPlayer);
BOOL EnterRift(ACDID idPortalGizmo, Player * ptPlayer);
BOOL PortToPlayerInStandardRiftPossible(Player * ptTargetPlayer, Player * ptPlayer);
void StartWeeklyChallengeRift(ACDID idObeliskGizmo, int32 nRiftTier, Player * ptPlayer);
void TryStartBlizzonRift(ACDID idObeliskGizmo, int32 nRiftTier, ACDID idACDPlayer);
BOOL PlayerPaidForRift(const Player * ptPlayer);
GreaterRiftPlayerState * GetGreaterRiftPlayerState(const Player * ptPlayer);
BOOL IsTieredRiftOrphaned(Player * ptPlayerToIgnore);
void SendTieredRiftOrphanedEvent(ACDID idACDPlayer);
int32 RiftPlayerCount();
BOOL PlayerGetEmpoweredRift(const Player * ptPlayer);
void PlayerSetLeaverHash(Player * ptPlayer);
void PlayerSetPaidForRift(Player * ptPlayer, BOOL bPaid, BOOL bCopyToLocalPlayers, BOOL bEmpowered);
void UnlockRiftLevelsForParticipants(int32 nRiftTierCompleted);
void Cleanup();
BOOL IsAnyRiftActive();
BOOL IsTieredRiftActive();
BOOL IsNonTieredRiftActive();
void SetRiftActive(BOOL bIsActive);
int32 GetActiveRiftLevel();
void PlayerAcceptInvite(Player * ptPlayer, BOOL bEmpowered);
void PlayerDeclineInvite(Player * ptPlayer);
void PlayerAcceptStart(int32 nRiftTier, Player * ptPlayer, BOOL bEmpowered);
void PlayerDeclineStart(Player * ptPlayer);
void PlayerRequestedJoinInfo(Player * ptPlayer);
void PlayerRemoved(Player * ptPlayer, BOOL bPlayerWasDeleted);
void PlayerVoteCancel(Player * ptPlayer);
void PlayerVoteContinue(Player * ptPlayer, BOOL bEmpowered);
void PlayerPaymentWarningAccepted(Player * ptPlayer, BOOL bEnterPortal);
void PlayerSetJewelUpgradesUsed(Player * ptPlayer, int32 nUsed);
void PlayerSetJewelBonusUpgradesLeft(Player * ptPlayer, int32 nRemaining);
void NephalemRiftStateCreateForGame();
void NephalemRiftStateDestroyForGame();
void PlayersGiveAllRiftRewardChoices(BOOL bSuccess);
BOOL PlayersAreStillChoosingRewards(const Player * ptPlayerToIgnore);
void PlayerStateSetNewSoloRecord(Player * ptPlayer, BOOL bHasNewSoloRecord);
void AddArtificiallyElapsedTime(int32 nTicks);
int32 GetArtificiallyElapsedTimeMS();
void SendPlayersDoneChoosingRewardsEvent(ACDID idACDPlayer);
GreaterRiftRewardState * PlayerGetGreaterRiftRewardState(Player * ptPlayer);
ACDID GetJewelUpgradeVendor();
void SetJewelUpgradeVendor(ACDID idACDJewelUpgradeVendor);
void PlayerManuallyPortIntoRift(Player * ptPlayer);

}  // namespace NephalemRiftManager

namespace D3 { namespace Quests { class QuestReward; } }
namespace ChallengeRift { struct RewardScreenInfo; }
namespace NephalemRiftManager {
void PlayerSendRiftEndScreenInfo(const Player & tDestPlayer, BOOL bSuccess, int32 nCompletionTimeMS, int32 nRiftTier, const D3::Quests::QuestReward & tReward, ChallengeRift::RewardScreenInfo tChallengeRiftInfo);
}
