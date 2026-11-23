// Original owner: X/Common/NephalemRiftCommon.h. Query subset used by inventory filters.
#pragma once
#include "CommonTypes.h"
namespace NephalemRift { BOOL IsRiftKey(ACDID idItem); }

struct GBHandle;
struct Player;
typedef int32 GBID;  // X/Common/GameBalance.h

namespace NephalemRift {
enum RIFT_REWARD_CHOICE_STATE {
    RIFT_REWARD_CHOICE_INVALID = -1,
    RIFT_GEM_UPGRADE = 0,
    RIFT_FAILURE_NO_REWARD = 1,
    RIFT_CHOICE_COUNT = 2,
};

BOOL IsRiftKey(GBID gbidItem);
int32 GetKeyEntryCost(int32 nRiftTier);
ACDID GetKeyForGreaterRift(Player * ptPlayer, int32 & nCount);
BOOL HasKeyForGreaterRift(Player * ptPlayer);
BOOL IsTieredRiftActive();
int32 GetActiveTieredRiftLevel();
int32 GetMaxTieredRiftLevel();
BOOL PlayerHasNoRiftDeaths(const Player * ptPlayer);
int32 GetParticipatingPlayerCount();
uint32 GetStartRiftDelaySeconds();
uint64 GetEmpoweredRiftCost(int32 nRiftLevel);
}  // namespace NephalemRift
