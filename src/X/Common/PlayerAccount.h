// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/PlayerAccount.h
// Subset: PlayerAccountBanner and PlayerAccount (the layout Player::tAccount needs).

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "OnlineService/OnlineService.h"

namespace D3 { namespace Account { class BannerConfiguration; } }

enum {  // line 28
    PARTITION_FLAG_TIERED_LOOT_RUN_BOSS_KILLED = 0,
    PARTITION_FLAG_GREED_REALM_VISITED = 1,
    PARTITION_FLAG_SAW_PLATINUM_GOBLIN = 2,
    PARTITION_FLAG_SET_DUNGEON_LORE_1 = 3,
    PARTITION_FLAG_SET_DUNGEON_LORE_2 = 4,
    PARTITION_FLAG_SET_DUNGEON_LORE_3 = 5,
    PARTITION_FLAG_SET_DUNGEON_LORE_4 = 6,
    PARTITION_FLAG_SET_DUNGEON_LORE_5 = 7,
};

struct PlayerAccountBanner {  // size 0x28
    PlayerAccountBanner();
    PlayerAccountBanner(const D3::Account::BannerConfiguration &);
    void Export(D3::Account::BannerConfiguration & tBanner) const;
    void Import(const D3::Account::BannerConfiguration & tBanner);
    BOOL operator==(const PlayerAccountBanner & tRHS) const;
    BOOL operator!=(const PlayerAccountBanner & tRHS) const { return !(*this == tRHS); }  // PlayerAccount.h:73
    uint32 uBannerIndex;  // 0x00
    int32 nSigilMain;  // 0x04
    int32 nSigilAccent;  // 0x08
    int32 nPatternColorIndex;  // 0x0c
    int32 nBackgroundColorIndex;  // 0x10
    int32 nSigilColorIndex;  // 0x14
    int32 nPlacementIndex;  // 0x18
    int32 nPattern;  // 0x1c
    int32 nEpicBanner;  // 0x20
    BOOL bUseSigilVariant;  // 0x24
};
static_assert(sizeof(PlayerAccountBanner) == 0x28, "PlayerAccountBanner");

struct PlayerAccount {  // size 0xc0
    PlayerAccount();
    OnlineService::GameAccountHandle tHandle;  // 0x00
    OnlineService::AccountId uBnetAccountId;  // 0x0c
    CHAR usName[128];  // 0x10
    uint32 uDigestFlags;  // 0x90
    uint32 uPartitionFlags;  // 0x94
    PlayerAccountBanner tBanner;  // 0x98
};
static_assert(sizeof(PlayerAccount) == 0xc0, "PlayerAccount");

