// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/TeamCommon.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"

typedef int32 Team;

typedef int32 KnownListID;

enum {
    TEAM_COLOR_NONE = -1,
    TEAM_COLOR_0 = 0,
    TEAM_COLOR_1 = 1,
    TEAM_COLOR_NEUTRAL = 2,
    TEAM_COLOR_COUNT = 3,
};

typedef int32 TeamColor;

enum TeamFlags {
    TEAM_AFFECTED_BY_PLAYER_CHAOS_EFFECTS = 0,
    TEAM_AFFECTED_BY_MONSTER_CHAOS_EFFECTS = 1,
    TEAM_FLAGS_COUNT = 2,
};


BOOL TeamsAreFriendly(Team eTeam0, Team eTeam1);  // TeamCommon.cpp:358
BOOL TeamIsMonsterTeam(Team eTeam);  // TeamCommon.cpp:405
struct Player;
void STeamRemovePlayer(Player & tPlayer);  // TeamCommon.cpp:534
