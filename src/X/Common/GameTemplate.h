// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/GameTemplate.h
// The data and game-type queries implemented in GameTemplate.cpp.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"

typedef int32 GameType;

typedef int32 GlobalSNO;

enum GameParamFlags {
    GPF_ADJUST_EXP_WITH_RELATIVE_LEVEL = 0,
    GPF_RESET_LIFE_STATS_UPON_LEVEL_UP = 1,
    GPF_GRANT_GOLD_UPON_KILL = 2,
    GPF_TRICKLE_ACDS_TO_PLAYER = 3,
    GPF_RESURRECTION_DELAY = 4,
    GPF_APPLY_RESURRECTION_BUFFS = 5,
    GPF_ALLOW_TOWN_PORTALS = 6,
    GPF_RESTOCK_SHOPS_UPON_TALK = 7,
    GPF_KEEP_ALL_SCENES_ACTIVE = 8,
    GPF_QUEST_UI = 9,
    GPF_REVERSE_CAMERA_FOR_TEAM_1 = 10,
    GPF_SAVE_HEROS = 11,
    GPF_ARENA_SCOREBOARD = 12,
    GPF_REVEAL_MINIMAP = 13,
    GPF_FORCE_NEW_HEROS = 14,
    GPF_CREATE_HEADSTONES_UPON_PLAYER_DEATH = 15,
    GPF_NO_EXPERIENCE = 16,
    GPF_RESPAWN_IN_WAVES = 17,
    GPF_SHOW_MONSTER_HP_BARS = 18,
    GPF_LIMIT_WEAPON_EFFECTS = 19,
    GPF_USE_RANDOM_RESPAWN_LOCATION = 20,
    GPF_IGNORE_DURABILITY_LOSS = 21,
    GPF_STORE_DISABLED = 22,
    GPF_COUNT = 23,
    GPF_MAX = 64,
};

struct GameTemplate {  // size 0x18
    uint64 uFlags;  // 0x00
    float flMaxExperienceDist;  // 0x08
    float flMinimapFeetPerPixel;  // 0x0c
    uint32 uMaxPlayerCount;  // 0x10
    GlobalSNO eStartWorld;  // 0x14
    static const GameTemplate & GetGameTemplate(GameType eGameType);
};
static_assert(sizeof(GameTemplate) == 0x18, "GameTemplate");

// Declared here from its definition in GameTemplate.cpp (:0x7100438680); the
// game-type queries the UI screens call. _Z13GameTypeIsPvPv.
BOOL GameTypeIsPvP();

BOOL GameTypeIsPvP(GameType eGameType);
BOOL GameIsStoryMode();
BOOL GameIsAdventureMode();
