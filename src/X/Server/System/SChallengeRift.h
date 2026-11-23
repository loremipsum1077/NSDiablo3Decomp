// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/System/SChallengeRift.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ChallengeRift.h"
#include "../../Common/GameBalance.h"

struct Player;

namespace ChallengeRift {
struct RewardScreenInfo { RewardScreenInfo();  // size 0x10
    uint32 challengeRiftNum;  // 0x00
    GBID challengeRiftRewardGbid;  // 0x04
    int32 challengeRiftTimeToBeatMs;  // 0x08
    ChallengeRiftRewardState challengeRiftRewardState;  // 0x0c
};
static_assert(sizeof(RewardScreenInfo) == 0x10, "RewardScreenInfo");

void HandleChallengeRewards(Player * ptPlayer, int32 nTotalTimeMS, RewardScreenInfo & tInfo);  // SChallengeRift.cpp:47
GBHandle GetChallengeReward();  // SChallengeRift.cpp:110
void OpenChallengeRift();  // SChallengeRift.cpp:237
void GrantRewardToPlayerIfEligible(Player * ptPlayer);  // SChallengeRift.cpp:288

}  // namespace ChallengeRift

// Functions defined in this file (bodies are not in the DWARF):
//   line    28  ChallengeRift::RewardScreenInfo::RewardScreenInfo()  [inlined into 1 functions]

#line 28 "X/Server/System/SChallengeRift.h"
inline ChallengeRift::RewardScreenInfo::RewardScreenInfo()
    : challengeRiftNum(0), challengeRiftRewardGbid(GBID_NONE),
      challengeRiftTimeToBeatMs(900000), challengeRiftRewardState(CRRS_REWARD_UNEARNED)
{
}
