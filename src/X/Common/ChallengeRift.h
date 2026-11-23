// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/ChallengeRift.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions/AttachmentDefinition.h"
#include "Leaderboard.h"
#include "SNO/SNOFiles.h"
#include "../Sigma/SigmaTypes.h"

struct Player;

enum ChallengeRiftRewardState {
    CRRS_REWARD_UNEARNED = 0,
    CRRS_REWARD_JUST_EARNED = 1,
    CRRS_REWARD_PREVIOUSLY_EARNED = 2,
    CRRS_COUNT = 3,
};

namespace ChallengeRift {
enum ChallengeRiftPersonalBest {
    CRPB_TEAM_1 = 0,
    CRPB_TEAM_2 = 1,
    CRPB_TEAM_3 = 2,
    CRPB_TEAM_4 = 3,
    CRPB_COUNT = 4,
};

void InitializeForGame();
void DisposeForGame();
ChallengeRiftPersonalBest LeaderboardIdToChallengeRiftPersonalBest(LeaderboardId leaderboardId);
ChallengeRiftPersonalBest NumPlayersToChallengeRiftPersonalBest(uint32 numPlayers);
void SnapshotPlayerSkills(const Player & tPlayer);
BOOL IsValidChallengeRiftSkill(SNO snoSkill);
RuneType GetRuneTypeForSkill(SNO snoSkill);

}  // namespace ChallengeRift
