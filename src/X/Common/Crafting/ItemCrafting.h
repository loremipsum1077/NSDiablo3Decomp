// Original file: X/Common/Crafting/ItemCrafting.h
// Subset: the currency type carried in achievement rewards.
#pragma once
#include "BlizzardCore/Include/Blizzard/Types.h"
typedef int32 CurrencyType;

// ItemCrafting.h:108 and :132. The crafter save-data records (CrafterSavedData names every DWARF
// member) that HealTransmogs (X/Server/OnlineService/DataHealUtils.cpp) needs.
typedef int32 GBID;  // X/Common/GameBalance.h
#include "../GameBalance.h"

namespace D3 { namespace Account { class SavedDefinition; } }
namespace D3 { namespace GameBalance { class BitPackedGbidArray; } }
namespace D3 { namespace ItemCrafting { class CrafterData; } }
namespace D3 { namespace ItemCrafting { class CrafterSavedData; } }

enum {
    CRAFTER_RANKS = 13,
    CRAFTER_DATA_VERSION = 26,
    MAX_RECIPES_PER_CRAFTER = 640,
    MAX_UNLOCKED_PARTITION_TRANSMOGS_PER_ACCOUNT = 1536,
    MAX_UNLOCKED_ACCOUNT_TRANSMOGS_PER_ACCOUNT = 64,
    MAX_UNLOCKED_TRANSMOGS = 1600,
    MAX_RECIPES_PER_CRAFTER_BYTE_ARRAY_SIZE = 80,
};

struct CrafterData {  // size 0xa08
    void Export(D3::ItemCrafting::CrafterData & tData) const;
    void Import(const D3::ItemCrafting::CrafterData & tData);
    GBID gbidRecipes[640];  // 0x00
    int32 nNumRecipes;  // 0xa00
    int32 nLevel;  // 0xa04
};
static_assert(sizeof(CrafterData) == 0xa08, "CrafterData");
static_assert(__builtin_offsetof(CrafterData, nNumRecipes) == 0xa00, "CrafterData");
static_assert(offsetof(CrafterData, nNumRecipes) == 0xa00, "CrafterData.nNumRecipes");
static_assert(offsetof(CrafterData, nLevel) == 0xa04, "CrafterData.nLevel");

struct CrafterTransmogData {  // size 0x1a08
    void SetToDefaults();
    void Export(D3::GameBalance::BitPackedGbidArray & tPartitionData, D3::GameBalance::BitPackedGbidArray * ptAccountData) const;
    void Import(const D3::GameBalance::BitPackedGbidArray & tPartitionData, const D3::GameBalance::BitPackedGbidArray * ptAccountData);
    void ImportNoLogs(const D3::GameBalance::BitPackedGbidArray & tPartitionData, const D3::GameBalance::BitPackedGbidArray * ptAccountData);
private:
    void ImportInternal(const D3::GameBalance::BitPackedGbidArray & tPartitionData, const D3::GameBalance::BitPackedGbidArray * ptAccountData, bool bSilent);
public:
    GBID gbidUnlockedTransmogs[1600];  // 0x00
    int32 nUnlockedTransmogs;  // 0x1900
    GBID gbidUnlockedAccountTransmogs[64];  // 0x1904
    int32 nUnlockedAccountTransmogs;  // 0x1a04
};
static_assert(sizeof(CrafterTransmogData) == 0x1a08, "CrafterTransmogData");
static_assert(offsetof(CrafterTransmogData, nUnlockedTransmogs) == 0x1900, "CrafterTransmogData.nUnlockedTransmogs");
static_assert(offsetof(CrafterTransmogData, gbidUnlockedAccountTransmogs) == 0x1904, "CrafterTransmogData.gbidUnlockedAccountTransmogs");
static_assert(offsetof(CrafterTransmogData, nUnlockedAccountTransmogs) == 0x1a04, "CrafterTransmogData.nUnlockedAccountTransmogs");

// X/Common/Items.h:632. Defined here rather than in Items.h: the original ItemCrafting.h includes
// Items.h, but this repo's Items.h pulls in ItemsSwig.h, whose AncientRank enumerators are still
// redeclared locally by consumers that reach ItemCrafting.h through Player.h
// (X/Client/UserInterface/Console/UIConsoleInventoryDetailsView.cpp). The constructor (line 634)
// is inline; its body is the SetToDefaults call inlined into HealTransmogs
// (X/Server/OnlineService/DataHealUtils.cpp).
struct ExtractedLegendariesData {  // size 0x804
    ExtractedLegendariesData()  // line 634
    {
        SetToDefaults();
    }
    void SetToDefaults();
    void Export(D3::GameBalance::BitPackedGbidArray & tPartitionData) const;
    void Import(const D3::GameBalance::BitPackedGbidArray & tPartitionData);
    BOOL Contains(GBID gbidItem) const;
    GBID gbidExtractedLegendaries[512];  // 0x00
    int32 nExtractedLegendariesCount;  // 0x800
};
static_assert(sizeof(ExtractedLegendariesData) == 0x804, "ExtractedLegendariesData");
static_assert(offsetof(ExtractedLegendariesData, nExtractedLegendariesCount) == 0x800, "ExtractedLegendariesData.nExtractedLegendariesCount");

// The default constructor (line 214) is implicit.
struct CrafterSavedData {  // size 0x5434
    void Export(D3::ItemCrafting::CrafterSavedData & tData, D3::Account::SavedDefinition * ptAccountData) const;
    void Import(const D3::ItemCrafting::CrafterSavedData & tData, const D3::Account::SavedDefinition * ptAccountData);
    void ImportNoLogs(const D3::ItemCrafting::CrafterSavedData & tData, const D3::Account::SavedDefinition * ptAccountData);
private:
    void ImportInternal(const D3::ItemCrafting::CrafterSavedData & tData, const D3::Account::SavedDefinition * ptAccountData, bool bSilent);
public:
    CrafterData tCrafterData[5];  // 0x00
    CrafterTransmogData tTransmogData;  // 0x3228
    ExtractedLegendariesData tExtractedLegendariesData;  // 0x4c30
};
static_assert(sizeof(CrafterSavedData) == 0x5434, "CrafterSavedData");
static_assert(offsetof(CrafterSavedData, tTransmogData) == 0x3228, "CrafterSavedData.tTransmogData");
static_assert(offsetof(CrafterSavedData, tExtractedLegendariesData) == 0x4c30, "CrafterSavedData.tExtractedLegendariesData");

// X/Common/Crafting/ItemCrafting.cpp:1792, 1816, 2062.
#include "../CommonTypes.h"
class ActorCommonData;
struct RecipeIngredient;
int32 ItemCrafting_GetReforgeCost(ACDID idACDItem);
int32 ItemCrafting_GetReforgeReagents(ActorCommonData & tACDItem, RecipeIngredient * arOutIngredients);
int32 ItemCrafting_GetTransmogCost(const ActorCommonData & tACDItem, GBID gbidTransmog);

// X/Common/Crafting/ItemCrafting.cpp: the rest of the file's public functions.
typedef int32 CrafterType;  // ItemCraftingSwig.h
struct Player;
struct RecipeEntry;
struct InventoryLocation;
typedef int32 InventorySlotType;
typedef int32 GameError;
void ItemCrafting_InitializeForApp();
void ItemCrafting_DisposeForApp();
int32 ItemCrafting_GetUnidentifiedItemCount(const ActorCommonData & tACDContainer);
float ItemCrafting_GetJewelUpgradeChance(const ActorCommonData & tACDJewel, int32 nRiftLevel);
int32 ItemCrafting_GetDynamicRecipeMaxTargetLevel(int32 nCrafterLevel);
BOOL ItemCrafting_ArtisanCanCraftItem(GBID gbidRecipe, CrafterType eType, const Player * ptPlayer);
BOOL ItemCrafting_CanCraftItem(GBID gbidRecipe, CrafterType eType, const Player * ptPlayer);
GameError ItemCrafting_CanLearnRecipe(const RecipeEntry * pdefRecipeEntry, const Player * ptPlayer);
BOOL ItemCrafting_PlayerKnowsRecipe(const Player & tPlayer, GBID gbidRecipe);
BOOL ItemCrafting_PlayerPossessesRecipe(const Player & tPlayer, GBID gbidRecipe);
BOOL ItemCrafting_PlayerKnowsAllRecipes(const Player & tPlayer, CrafterType eCrafterType);
GBID ItemCrafting_GetTrainingRecipe(CrafterType eType, const Player * ptPlayer);
BOOL ItemCrafting_CanTrain(Player & tPlayer);
int32 GetCraftLevel(const CrafterType eType, const Player & tPlayer);
void SetCraftLevel(const CrafterType eType, Player & tPlayer, const int32 nLevel);
int32 ItemCrafting_GetRecipeCost(GBID gbidRecipe);
int32 ItemCrafting_GetRepairAllCost(const Player * ptPlayer, const BOOL fEquippedOnly);
int32 ItemCrafting_GetRepairCost(ACDID idACDItem, ACDID idACDPlayer);
int32 ItemCrafting_GetEnchantAffixCost(ActorCommonData & tACDItem);
int32 ItemCrafting_GetEnchantAffixReagents(ActorCommonData & tACDItem, RecipeIngredient * arOutIngredients, int32 nOutIngredientsMax);
InventorySlotType ItemCrafting_GetTransmogSlot(GBID gbidItem);
BOOL ItemCrafting_IsValidTransmogSlot(InventorySlotType eSlot);
int32 ItemCrafting_CompareMatchingTransmogs(GBID gbidTransmog1, GBID gbidTransmog2);
BOOL ItemCrafting_IsTransmogUnlocked(GBID gbidTransmog, const Player * ptPlayer, GBID * gbidOutUnlockedTransmog);
BOOL ItemCrafting_CanUnlockTransmog(GBID gbidTransmog, const Player * ptPlayerRequiredOwner, BOOL bManuallyLearnedOnly);
BOOL ItemCrafting_ItemTransmogsMatch(GBID gbidItem1, GBID gbidItem2);
BOOL ItemCrafting_ItemTransmogsMatch(const ActorCommonData & tACDItem, GBID gbidItem2);
BOOL ItemCrafting_CanTransmogItem(const ActorCommonData & tACDItem, GBID gbidTransmog, const Player * ptPlayer, BOOL bCheckUnlocked, BOOL bCheckCannotTransmog);
int32 ItemCrafting_GetGemRemovalCost(const ActorCommonData & tACDItem);
int32 ItemCrafting_GetGemRemovalCostForNewSocketCount(const ActorCommonData & tACDItem, int32 nNewSocketCount);
int32 ItemCrafting_GetGemQuality(const GBID gbidItem);
int32 ItemCrafting_GetGemTier(const ActorCommonData & tACDGem);
int32 CrafterGetMaxLevel(const CrafterType eType);
uint32 CrafterGetRank(const CrafterType eType, const uint32 nLevel);
uint32 CrafterGetLevelUpAnim(uint32 nRank);
uint32 CrafterGetIdleAnim(uint32 nRank);
extern const int32 cgnCrafterLevelsPerRank[5];

namespace ItemCrafting_BackEnd {
GameMessageType MessageTypeFromCrafterType(CrafterType eType, BOOL fInitialInfo);
CrafterType CrafterTypeFromMessageType(GameMessageType eType, BOOL & fInitialInfo);
void RepairCostVisitItem(ACDID idACDSquare, InventorySlotType, void * pUserData);
void GetIdentifyCount(ACDID idACDSquare, InventorySlotType, void * pUserData);
extern uint32 sArrLevelUpAnimTags[CRAFTER_RANKS];
extern uint32 sArrIdleAnimTags[CRAFTER_RANKS];
}  // namespace ItemCrafting_BackEnd
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
void ItemCrafting_GetLearnableRecipes(blz::vector<int> & arLearnableRecipes, CrafterType eCrafterType);
namespace ItemCrafting_BackEnd {
GBID GetJewelerTrainingRecipe(int32 nLevel);
GBID GetBlackSmithTrainingRecipe(int32 nLevel);
GBID GetMysticTrainingRecipe(int32 nLevel);
// line 164
struct UnsocketCallbackData {  // size 0x10
    ActorCommonData * ptACDPlayer;  // 0x00
    BOOL bTriggerAchievementEvent;  // 0x08
};
static_assert(sizeof(UnsocketCallbackData) == 0x10, "UnsocketCallbackData");
}  // namespace ItemCrafting_BackEnd

// The transmute message and inventory types (DWARF lines 140, 152, 247 and 256). The two enum
// typedefs repeat GameBalanceDefinition.h's (the reference header has them at lines 23 and 145).
typedef int32 TransmuteRecipeAction;
typedef int32 TransmuteRecipeIngredientType;

struct TransmuteItemsMessage {  // size 0x5c, line 140
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItems[9];  // 0x08
    int32 nItemsCount;  // 0x2c
    int32 nStackedItemsBitField;  // 0x30
    GBID gbidCurrencies[9];  // 0x34
    int32 nCurrenciesCount;  // 0x58
};

struct TransmuteResultsMessage {  // size 0x28, line 152
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    GBID gbidPower;  // 0x0c
    GBID gbidFakeItem;  // 0x10
    uint64 uFakeItemStackCount;  // 0x18
    TransmuteRecipeAction eType;  // 0x20
};

struct TransmuteInput {  // size 0x20, line 247
    GBID gbidCurrency;  // 0x00
    ActorCommonData * ptACDItem;  // 0x08
    int64 nStackCount;  // 0x10
    BOOL bUsedInRecipe;  // 0x18
    TransmuteRecipeIngredientType eIngredientType;  // 0x1c
};

// Line 256. The copy constructor, copy assignment and destructor the original inlines (line 256)
// are the implicit ones.
struct TransmuteInventory {  // size 0x18
    blz::vector<TransmuteInput, blz::allocator<TransmuteInput> > arInputs;  // 0x00
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(TransmuteItemsMessage) == 0x5c, "TransmuteItemsMessage");
static_assert(offsetof(TransmuteItemsMessage, tHeader) == 0x0, "TransmuteItemsMessage");
static_assert(offsetof(TransmuteItemsMessage, annItems) == 0x8, "TransmuteItemsMessage");
static_assert(offsetof(TransmuteItemsMessage, nItemsCount) == 0x2c, "TransmuteItemsMessage");
static_assert(offsetof(TransmuteItemsMessage, nStackedItemsBitField) == 0x30, "TransmuteItemsMessage");
static_assert(offsetof(TransmuteItemsMessage, gbidCurrencies) == 0x34, "TransmuteItemsMessage");
static_assert(offsetof(TransmuteItemsMessage, nCurrenciesCount) == 0x58, "TransmuteItemsMessage");
static_assert(sizeof(TransmuteResultsMessage) == 0x28, "TransmuteResultsMessage");
static_assert(offsetof(TransmuteResultsMessage, tHeader) == 0x0, "TransmuteResultsMessage");
static_assert(offsetof(TransmuteResultsMessage, annItem) == 0x8, "TransmuteResultsMessage");
static_assert(offsetof(TransmuteResultsMessage, gbidPower) == 0xc, "TransmuteResultsMessage");
static_assert(offsetof(TransmuteResultsMessage, gbidFakeItem) == 0x10, "TransmuteResultsMessage");
static_assert(offsetof(TransmuteResultsMessage, uFakeItemStackCount) == 0x18, "TransmuteResultsMessage");
static_assert(offsetof(TransmuteResultsMessage, eType) == 0x20, "TransmuteResultsMessage");
static_assert(sizeof(CrafterData) == 0xa08, "CrafterData");
static_assert(offsetof(CrafterData, gbidRecipes) == 0x0, "CrafterData");
static_assert(offsetof(CrafterData, nNumRecipes) == 0xa00, "CrafterData");
static_assert(offsetof(CrafterData, nLevel) == 0xa04, "CrafterData");
static_assert(sizeof(CrafterTransmogData) == 0x1a08, "CrafterTransmogData");
static_assert(offsetof(CrafterTransmogData, gbidUnlockedTransmogs) == 0x0, "CrafterTransmogData");
static_assert(offsetof(CrafterTransmogData, nUnlockedTransmogs) == 0x1900, "CrafterTransmogData");
static_assert(offsetof(CrafterTransmogData, gbidUnlockedAccountTransmogs) == 0x1904, "CrafterTransmogData");
static_assert(offsetof(CrafterTransmogData, nUnlockedAccountTransmogs) == 0x1a04, "CrafterTransmogData");
static_assert(sizeof(CrafterSavedData) == 0x5434, "CrafterSavedData");
static_assert(offsetof(CrafterSavedData, tCrafterData) == 0x0, "CrafterSavedData");
static_assert(offsetof(CrafterSavedData, tTransmogData) == 0x3228, "CrafterSavedData");
static_assert(offsetof(CrafterSavedData, tExtractedLegendariesData) == 0x4c30, "CrafterSavedData");
static_assert(sizeof(TransmuteInput) == 0x20, "TransmuteInput");
static_assert(offsetof(TransmuteInput, gbidCurrency) == 0x0, "TransmuteInput");
static_assert(offsetof(TransmuteInput, ptACDItem) == 0x8, "TransmuteInput");
static_assert(offsetof(TransmuteInput, nStackCount) == 0x10, "TransmuteInput");
static_assert(offsetof(TransmuteInput, bUsedInRecipe) == 0x18, "TransmuteInput");
static_assert(offsetof(TransmuteInput, eIngredientType) == 0x1c, "TransmuteInput");
static_assert(sizeof(TransmuteInventory) == 0x18, "TransmuteInventory");
static_assert(offsetof(TransmuteInventory, arInputs) == 0x0, "TransmuteInventory");
static_assert(sizeof(ItemCrafting_BackEnd::UnsocketCallbackData) == 0x10, "ItemCrafting_BackEnd::UnsocketCallbackData");
static_assert(offsetof(ItemCrafting_BackEnd::UnsocketCallbackData, ptACDPlayer) == 0x0, "ItemCrafting_BackEnd::UnsocketCallbackData");
static_assert(offsetof(ItemCrafting_BackEnd::UnsocketCallbackData, bTriggerAchievementEvent) == 0x8, "ItemCrafting_BackEnd::UnsocketCallbackData");
static_assert(sizeof(ExtractedLegendariesData) == 0x804, "ExtractedLegendariesData");
static_assert(offsetof(ExtractedLegendariesData, gbidExtractedLegendaries) == 0x0, "ExtractedLegendariesData");
static_assert(offsetof(ExtractedLegendariesData, nExtractedLegendariesCount) == 0x800, "ExtractedLegendariesData");
