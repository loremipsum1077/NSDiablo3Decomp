// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/SHero.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/string.h"
#include "../Common/OnlineService/OnlineService.h"
#include "../Common/Protobuf/Account.pb.h"
#include "../Common/Protobuf/ChallengeRifts.pb.h"
#include "../Common/Protobuf/Hero.pb.h"
#include "../Common/Protobuf/Items.pb.h"

struct Player;

typedef int32 ServerSocketID;

typedef int32 DbErrorCode;

namespace SHero {
enum HeroSaveOptions {
    HSO_INVALID_BIT = 0,
    HSO_QUIT_AFTER_SAVE_BIT = 1,
    HSO_PLAYER_DELETED_FROM_GAME_BIT = 2,
    HSO_SKIP_ACCOUNT_DEFINITION_SAVE_BIT = 3,
    HSO_ENCODE_ITEM_DATA = 4,
    HSO_RESET_ACCOUNT_DATA = 5,
    HSO_RELOAD_MAIL = 6,
    HSO_WEEKLY_CHALLENGE_DATA = 7,
    HSO_READ_ONLY = 8,
    HSO_COUNT = 9,
};

struct PlayerSaveData {  // size 0x1e8
    PlayerSaveData() : uHeroSaveOptions(0) {}  // line 66
    OnlineService::GameAccountHandle idGameAccount;  // 0x00
    blz::string tHeroFilename;  // 0x10
    blz::string tAccountFilename;  // 0x38
    D3::Account::SavedDefinition tAccountDefinition;  // 0x60
    D3::Hero::SavedDefinition tHeroDefinition;  // 0x158
    uint32 uHeroSaveOptions;  // 0x1e0
};
static_assert(sizeof(PlayerSaveData) == 0x1e8, "PlayerSaveData");

struct ChallengeRiftPlayerSaveData {  // size 0xc8
    ChallengeRiftPlayerSaveData() {}  // line 84
    OnlineService::GameAccountHandle idGameAccount;  // 0x00
    D3::Items::Mail tRewardMail;  // 0x10
    blz::string tSeenTutorials;  // 0x78
    D3::ChallengeRifts::AccountData tAccountData;  // 0xa0
};
static_assert(sizeof(ChallengeRiftPlayerSaveData) == 0xc8, "ChallengeRiftPlayerSaveData");

BOOL Save(Player & tPlayer, SHero::PlayerSaveData & tSaveData, uint32 dwHeroSaveOptions);  // SHero.cpp:141

}  // namespace SHero

// Functions defined in this file (bodies are not in the DWARF):
//   line    64  SHero::PlayerSaveData::PlayerSaveData(SHero::PlayerSaveData &&)  [inlined into 2 functions]
//   line    64  SHero::PlayerSaveData::~PlayerSaveData()  [inlined into 3 functions]
//   line    66  SHero::PlayerSaveData::PlayerSaveData()  [inlined into 1 functions]
//   line    82  SHero::ChallengeRiftPlayerSaveData::ChallengeRiftPlayerSaveData(SHero::ChallengeRiftPlayerSaveData &&)  [inlined into 2 functions]
//   line    82  SHero::ChallengeRiftPlayerSaveData::~ChallengeRiftPlayerSaveData()  [inlined into 4 functions]
//   line    84  SHero::ChallengeRiftPlayerSaveData::ChallengeRiftPlayerSaveData()  [inlined into 1 functions]
