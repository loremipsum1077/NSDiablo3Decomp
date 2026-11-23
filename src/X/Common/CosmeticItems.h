// Original owner: X/Common/CosmeticItems.h. Query subset used by inventory filters.
#pragma once
#include "CommonTypes.h"
typedef int32 GBID;  // X/Common/GameBalance.h
enum { COSMETIC_ITEM_INVALID = 0, COSMETIC_ITEM_WINGS = 1, COSMETIC_ITEM_PENNANT = 2, COSMETIC_ITEM_PET = 3, COSMETIC_ITEM_FRAME = 4, COSMETIC_ITEM_COUNT = 5 };
typedef int32 CosmeticItemType;
CosmeticItemType CosmeticItems_GetCosmeticItemTypeFromGBID(GBID gbidItem);

// The rest of the reference header (build/dwarf/headers/X/Common/CosmeticItems.h), added for
// X/Common/CosmeticItems.cpp. Layouts from the DWARF.
#include "../../BlizzardCore/Include/Blizzard/Time.h"

class ActorCommonData;
struct Player;
namespace D3 { namespace Account { class SavedDefinition; } }
namespace D3 { namespace CosmeticItems { class CosmeticItemSavedData; } }
namespace D3 { namespace CosmeticItems { class CosmeticItems; } }

typedef int32 VisualCosmeticItemSlot;

enum {
    MAX_COSMETIC_ITEMS = 512,
    MAX_COSMETIC_PETS = 32,
};

struct LimitedDurationPet {  // size 0x10
    GBID gbidPet;  // 0x00
    Blizzard::Time::Timestamp nExpirationTime;  // 0x08
};
static_assert(sizeof(LimitedDurationPet) == 0x10, "LimitedDurationPet");
static_assert(offsetof(LimitedDurationPet, nExpirationTime) == 0x08, "LimitedDurationPet");

struct CosmeticItemData {  // size 0x1210
    void Export(D3::CosmeticItems::CosmeticItemSavedData & tData) const;
    void Import(const D3::CosmeticItems::CosmeticItemSavedData & tData);
    void Import(const D3::CosmeticItems::CosmeticItems & tData);
    void Export(D3::CosmeticItems::CosmeticItems & tData) const;
    enum HasItemFlags {
        HI_LEARNED_ITEMS = 0,
        HI_PURCHASED_ITEMS = 1,
        HI_RENTED_ITEMS = 2,
        HI_ALL_ITEMS = 7,
    };
    void SetToDefaults();
    Blizzard::Time::Timestamp GetPetExpiration(GBID gbidPet);
    BOOL UpdateLimitedDurationPets(const ActorCommonData & tACDPlayer);
    bool HasItem(GBID gbidItem, uint32 dwFlags, Blizzard::Time::Timestamp nNowBlizzardTimestamp) const;
    void AddCosmeticItem(GBID gbidItem);
    void AddPurchasedCosmeticItem(GBID gbidItem);
    void AddLimitedDurationPet(GBID gbidPet, Blizzard::Time::Timestamp nExpirationTime);
    GBID gbidCosmeticItems[512];  // 0x00
    int32 nNumCosmeticItems;  // 0x800
    GBID gbidPurchasedCosmeticItems[512];  // 0x804
    int32 nNumPurchasedCosmeticItems;  // 0x1004
    LimitedDurationPet tLimitedDurationPets[32];  // 0x1008
    int32 nNumLimitedDurationPets;  // 0x1208
};
static_assert(sizeof(CosmeticItemData) == 0x1210, "CosmeticItemData");
static_assert(offsetof(CosmeticItemData, nNumCosmeticItems) == 0x800, "CosmeticItemData");
static_assert(offsetof(CosmeticItemData, gbidPurchasedCosmeticItems) == 0x804, "CosmeticItemData");
static_assert(offsetof(CosmeticItemData, nNumPurchasedCosmeticItems) == 0x1004, "CosmeticItemData");
static_assert(offsetof(CosmeticItemData, tLimitedDurationPets) == 0x1008, "CosmeticItemData");
static_assert(offsetof(CosmeticItemData, nNumLimitedDurationPets) == 0x1208, "CosmeticItemData");

struct CosmeticItemSavedData {  // size 0x1210
    void Export(D3::Account::SavedDefinition * ptAccountData) const;
    void Import(const D3::Account::SavedDefinition * ptAccountData);
    CosmeticItemData tCosmeticItemData;  // 0x00
};
static_assert(sizeof(CosmeticItemSavedData) == 0x1210, "CosmeticItemSavedData");

BOOL CosmeticItems_IsParagonFrame(ActorCommonData & tACDPlayer, GBID gbidItem, CosmeticItemType eType);
BOOL CosmeticItems_ValidateItem(ActorCommonData & tACDPlayer, GBID gbidItem, CosmeticItemType eType);
ACDID CosmeticItems_GenerateItemFromGBID(ACDNetworkName ann, GBID gbidCosmetic);
BOOL CosmeticItems_KnownByPlayer(const Player & tPlayer, GBID gbidCosmetic);
BOOL CosmeticItems_IsEquipped(const Player & tPlayer, GBID gbidItem);
