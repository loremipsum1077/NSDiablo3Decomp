// Original file: X/Client/UserInterface/Console/OnlineActions.h
// Subset: the campaign-reset entry point implemented at OnlineActions.cpp:1633.
#pragma once

namespace D3 { namespace Hero { class SavedDefinition; } }
namespace Console { namespace UIOnlineActions {
void ResetQuests(D3::Hero::SavedDefinition & tSavedDefinition);
} }

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"

namespace Console {
namespace UIOnlineActions {
enum {
    HERO_VALIDATION_OK = 0,
    HERO_VALIDATION_DIFFICULTY_LOCKED = 1,
    HERO_VALIDATION_DEAD = 2,
    HERO_VALIDATION_MUST_BE_HARDCORE = 3,
    HERO_VALIDATION_MUST_NOT_BE_HARDCORE = 4,
    HERO_VALIDATION_MUST_BE_SEASONAL = 5,
    HERO_VALIDATION_MUST_NOT_BE_SEASONAL = 6,
    HERO_VALIDATION_SEASON_OVER = 7,
    HERO_VALIDATION_SEASONS_NOT_READY = 8,
    HERO_VALIDATION_MISSING_NECROMANCER_LICENSE = 9,
    HERO_VALIDATION_UNK_10 = 10,  // 2.7.7: returned first by ValidateHeroForPartyMember (solo-hero check); message "ConsoleErrors:HeroValidationSoloHeroMustBeSolo[Detail]"
};

typedef int32 HeroValidationError;

enum {
    GAME_START_OK = 0,
    GAME_START_ERROR_BUSY = 1,
    GAME_START_ERROR_HOST_PAUSED = 2,
    GAME_START_ERROR_HERO_DEAD = 3,
    GAME_START_ERROR_HERO_DIFFICULTY_LOCKED = 4,
    GAME_START_ERROR_HERO_MUST_BE_HARDCORE = 5,
    GAME_START_ERROR_HERO_MUST_NOT_BE_HARDCORE = 6,
    GAME_START_ERROR_HERO_MUST_BE_SEASONAL = 7,
    GAME_START_ERROR_HERO_MUST_NOT_BE_SEASONAL = 8,
    GAME_START_ERROR_HERO_SEASON_OVER = 9,
    GAME_START_ERROR_HERO_SEASONS_NOT_READY = 10,
    GAME_START_ERROR_SEASON_ENDING_SOON = 11,
    GAME_START_ERROR_CONTENT_NOT_READY_YET = 12,
    GAME_START_ERROR_CHALLENGE_RIFT_ENDED = 13,
    GAME_START_ERROR_MISSING_NECROMANCER_LICENSE = 14,
    GAME_START_ERROR_LOCAL_PLAYERS_OFFLINE = 15,
    GAME_START_ERROR_UNKNOWN = 16,
};

typedef int32 GameStartValidationError;

}  // namespace UIOnlineActions
}  // namespace Console
