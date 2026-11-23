// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Leaderboard.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include <stddef.h>
#include <stdint.h>

namespace D3 { namespace Leaderboard { class Slot; } }

enum LeaderboardIds {
    LB_NONE = 0,
    LB_ACHIEVEMENT_POINTS = 1,
    LB_RIFT_SOLO_BARBARIAN = 2,
    LB_RIFT_SOLO_CRUSADER = 3,
    LB_RIFT_SOLO_DEMONHUNTER = 4,
    LB_RIFT_SOLO_MONK = 5,
    LB_RIFT_SOLO_WITCHDOCTOR = 6,
    LB_RIFT_SOLO_WIZARD = 7,
    LB_RIFT_SOLO_NECROMANCER = 8,
    LB_RIFT_PLACEHOLDER_9 = 9,
    LB_RIFT_TEAM_2 = 10,
    LB_RIFT_TEAM_3 = 11,
    LB_RIFT_TEAM_4 = 12,
    LB_RIFT_TEAM_1 = 13,
    LB_RIFT_FIRST = 2,
    LB_RIFT_LAST_PC = 12,
    LB_RIFT_LAST_CONSOLE = 13,
    LB_RIFT_LAST = 13,
    LB_SPEED_RUN_ALL = 20,
    LB_SPEED_RUN_ACT1 = 21,
    LB_SPEED_RUN_ACT2 = 22,
    LB_SPEED_RUN_ACT3 = 23,
    LB_SPEED_RUN_ACT4 = 24,
    LB_SPEED_RUN_ACT5 = 25,
    LB_SPEED_RUN_FIRST = 20,
    LB_SPEED_RUN_LAST = 25,
    LB_CHALLENGE_RIFT_TEAM_1 = 30,
    LB_CHALLENGE_RIFT_TEAM_2 = 31,
    LB_CHALLENGE_RIFT_TEAM_3 = 32,
    LB_CHALLENGE_RIFT_TEAM_4 = 33,
    LB_CHALLENGE_RIFT_FIRST = 30,
    LB_CHALLENGE_RIFT_LAST = 33,
    LB_CHALLENGE_SCORE = 40,
    LB_LAST_STATIC = 99,
    LB_CONSOLE_CONQUEST_MAPPED_FIRST = 101,
    LB_CONSOLE_CONQUEST_MAPPED_LAST = 120,
};

typedef uint64 LeaderboardId;

enum LeaderboardScope {
    LP_REGIONAL = 0,
    LP_SEASONAL = 1,
    LP_HARDCORE = 2,
    LP_SOFTCORE = 3,
    LP_SEASONAL_HARDCORE = 4,
    LP_SEASONAL_SOFTCORE = 5,
    LP_CHALLENGE = 6,
    LP_CHALLENGE_SCORE = 7,
    LP_COUNT = 8,
};

enum LeaderboardType {
    LT_ERA = 0,
    LT_SEASON = 1,
    LT_CHALLENGE_SCORE = 2,
    LT_CHALLENGE = 3,
};

struct LeaderboardKey {  // size 0x10
    LeaderboardKey();
    LeaderboardKey(uint64_t, LeaderboardScope);
    uint64_t m_id;  // 0x00
    LeaderboardScope m_scope;  // 0x08
};
static_assert(sizeof(LeaderboardKey) == 0x10, "LeaderboardKey");

struct LeaderboardKeyAndPartition {  // size 0x10
    LeaderboardKeyAndPartition();
    LeaderboardKeyAndPartition(uint64, LeaderboardScope, uint32);
    uint64_t m_id;  // 0x00
    LeaderboardScope m_scope;  // 0x08
    uint32 m_partition;  // 0x0c
};
static_assert(sizeof(LeaderboardKeyAndPartition) == 0x10, "LeaderboardKeyAndPartition");

// Inlined everywhere in the original (no out-of-line copy); default ctor zeroes all fields
// (LobbyService.cpp: vector<LeaderboardStatsInfo>::resize stores 16 zero bytes at m_lbKey).
inline LeaderboardKeyAndPartition::LeaderboardKeyAndPartition()
    : m_id(0), m_scope((LeaderboardScope)0), m_partition(0)
{
}

struct LeaderboardSlotSorter {  // size 0x1
    bool operator()(const D3::Leaderboard::Slot &, const D3::Leaderboard::Slot &) const;
    bool operator()(const D3::Leaderboard::Slot *, const D3::Leaderboard::Slot *) const;
};
static_assert(sizeof(LeaderboardSlotSorter) == 0x1, "LeaderboardSlotSorter");
