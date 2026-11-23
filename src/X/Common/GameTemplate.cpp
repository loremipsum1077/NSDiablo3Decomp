// Original file: X/Common/GameTemplate.cpp
#include "GameTemplate.h"
// The original also includes blz::ignore before its SNO and placement statics.
#include "../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "GameCommonData.h"
#include "GameSwig.h"
#include "GlobalSNO.h"

// Header-owned statics initialized in the original translation unit, in order.
#include "WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"

// Values from _GLOBAL__sub_I_GameTemplate.cpp, whose DWARF records this array at
// line 14. The first entry is GT_NOTHING; every game type indexes one past it.
static GameTemplate gsGameTemplates[GT_COUNT + 1] = {
    {
        (1ULL << GPF_ADJUST_EXP_WITH_RELATIVE_LEVEL) |
        (1ULL << GPF_RESET_LIFE_STATS_UPON_LEVEL_UP) |
        (1ULL << GPF_TRICKLE_ACDS_TO_PLAYER) |
        (1ULL << GPF_APPLY_RESURRECTION_BUFFS) |
        (1ULL << GPF_ALLOW_TOWN_PORTALS) |
        (1ULL << GPF_QUEST_UI) |
        (1ULL << GPF_SAVE_HEROS) |
        (1ULL << GPF_CREATE_HEADSTONES_UPON_PLAYER_DEATH),
        120.0f, 1.0f, MAX_PLAYERS, GLOBALSNO_INVALID,
    },
    {
        (1ULL << GPF_SAVE_HEROS) |
        (1ULL << GPF_LIMIT_WEAPON_EFFECTS),
        120.0f, 1.0f, MAX_PLAYERS, GLOBALSNO_INVALID,
    },
    {
        (1ULL << GPF_ADJUST_EXP_WITH_RELATIVE_LEVEL) |
        (1ULL << GPF_RESET_LIFE_STATS_UPON_LEVEL_UP) |
        (1ULL << GPF_TRICKLE_ACDS_TO_PLAYER) |
        (1ULL << GPF_APPLY_RESURRECTION_BUFFS) |
        (1ULL << GPF_ALLOW_TOWN_PORTALS) |
        (1ULL << GPF_QUEST_UI) |
        (1ULL << GPF_SAVE_HEROS) |
        (1ULL << GPF_CREATE_HEADSTONES_UPON_PLAYER_DEATH),
        120.0f, 1.0f, MAX_PLAYERS, cgeGlobalSnoDefaultStartWorld,
    },
    {
        (1ULL << GPF_TRICKLE_ACDS_TO_PLAYER) |
        (1ULL << GPF_RESURRECTION_DELAY) |
        (1ULL << GPF_KEEP_ALL_SCENES_ACTIVE) |
        (1ULL << GPF_REVERSE_CAMERA_FOR_TEAM_1) |
        (1ULL << GPF_SAVE_HEROS) |
        (1ULL << GPF_ARENA_SCOREBOARD) |
        (1ULL << GPF_REVEAL_MINIMAP) |
        (1ULL << GPF_SHOW_MONSTER_HP_BARS) |
        (1ULL << GPF_IGNORE_DURABILITY_LOSS),
        60.0f, 2.0f, MAX_PLAYERS, GLOBALSNO_WORLD_PVPARENA,
    },
    {
        (1ULL << GPF_GRANT_GOLD_UPON_KILL) |
        (1ULL << GPF_TRICKLE_ACDS_TO_PLAYER) |
        (1ULL << GPF_RESURRECTION_DELAY) |
        (1ULL << GPF_RESTOCK_SHOPS_UPON_TALK) |
        (1ULL << GPF_KEEP_ALL_SCENES_ACTIVE) |
        (1ULL << GPF_SAVE_HEROS) |
        (1ULL << GPF_ARENA_SCOREBOARD) |
        (1ULL << GPF_REVEAL_MINIMAP) |
        (1ULL << GPF_NO_EXPERIENCE) |
        (1ULL << GPF_SHOW_MONSTER_HP_BARS) |
        (1ULL << GPF_USE_RANDOM_RESPAWN_LOCATION) |
        (1ULL << GPF_IGNORE_DURABILITY_LOSS),
        60.0f, 1.25f, MAX_PLAYERS, GLOBALSNO_WORLD_PVPARENA,
    },
    {
        (1ULL << GPF_TRICKLE_ACDS_TO_PLAYER) |
        (1ULL << GPF_RESURRECTION_DELAY) |
        (1ULL << GPF_KEEP_ALL_SCENES_ACTIVE) |
        (1ULL << GPF_REVERSE_CAMERA_FOR_TEAM_1) |
        (1ULL << GPF_SAVE_HEROS) |
        (1ULL << GPF_ARENA_SCOREBOARD) |
        (1ULL << GPF_REVEAL_MINIMAP) |
        (1ULL << GPF_SHOW_MONSTER_HP_BARS) |
        (1ULL << GPF_IGNORE_DURABILITY_LOSS),
        60.0f, 1.5f, MAX_PLAYERS, GLOBALSNO_INVALID,
    },
    {
        (1ULL << GPF_TRICKLE_ACDS_TO_PLAYER) |
        (1ULL << GPF_RESURRECTION_DELAY) |
        (1ULL << GPF_KEEP_ALL_SCENES_ACTIVE) |
        (1ULL << GPF_SAVE_HEROS) |
        (1ULL << GPF_ARENA_SCOREBOARD) |
        (1ULL << GPF_REVEAL_MINIMAP) |
        (1ULL << GPF_NO_EXPERIENCE) |
        (1ULL << GPF_SHOW_MONSTER_HP_BARS) |
        (1ULL << GPF_USE_RANDOM_RESPAWN_LOCATION) |
        (1ULL << GPF_IGNORE_DURABILITY_LOSS),
        60.0f, 1.25f, MAX_PLAYERS, GLOBALSNO_WORLD_PVPARENA,
    },
    {
        (1ULL << GPF_TRICKLE_ACDS_TO_PLAYER) |
        (1ULL << GPF_APPLY_RESURRECTION_BUFFS) |
        (1ULL << GPF_ALLOW_TOWN_PORTALS) |
        (1ULL << GPF_SAVE_HEROS) |
        (1ULL << GPF_CREATE_HEADSTONES_UPON_PLAYER_DEATH) |
        (1ULL << GPF_NO_EXPERIENCE) |
        (1ULL << GPF_IGNORE_DURABILITY_LOSS) |
        (1ULL << GPF_STORE_DISABLED),
        120.0f, 1.0f, MAX_PLAYERS, GLOBALSNO_WORLD_CHALLENGE_RIFTS,
    },
};

const GameTemplate & GameTemplate::GetGameTemplate(GameType eGameType) {
    int32 nTemplate = eGameType - GT_NOTHING;
    return gsGameTemplates[nTemplate];
}

BOOL GameTypeIsPvP(GameType eGameType) {
    return eGameType >= GT_PVP_GAME_FIRST && eGameType <= GT_PVP_GAME_LAST;
}

BOOL GameTypeIsPvP() {
    GameType eGameType = GameGetGameType();
    return GameTypeIsPvP(eGameType);
}

BOOL GameIsStoryMode() {
    return GameGetCurrentAct() <= ACT_MAX_CAMPAIGN && GameGetGameType() == GT_DIABLO3;
}

BOOL GameIsAdventureMode() {
    return GameIsOpenWorld();
}
