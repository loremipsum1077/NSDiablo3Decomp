// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleInventoryFilters.h
// Filter bands and their inventory item storage; layout and names follow DWARF.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleNewItemIndicators.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Sigma/SigmaTypes.h"

struct Player;

typedef int32 InventorySlotType;

enum {
    FILTER_ALL = -2,
    FILTER_NONE = -1,
    FILTER_PLAYER_START = 0,
    FILTER_MAIN_HAND = 0,
    FILTER_OFF_HAND = 1,
    FILTER_RING_RIGHT = 2,
    FILTER_NECK = 3,
    FILTER_RING_LEFT = 4,
    FILTER_HANDS = 5,
    FILTER_SHOULDERS = 6,
    FILTER_CHEST = 7,
    FILTER_HEAD = 8,
    FILTER_BELT = 9,
    FILTER_LEGS = 10,
    FILTER_FEET = 11,
    FILTER_WRIST = 12,
    FILTER_BAG = 13,
    FILTER_PLAYER_END = 13,
    FILTER_DAMAGED = 14,
    FILTER_AVAILABLE_SOCKETS = 15,
    FILTER_SOCKETED = 16,
    FILTER_CAN_SOCKET_UPGRADEABLE_JEWEL = 17,
    FILTER_DYEABLE = 18,
    FILTER_CAN_ADD_SOCKETS_WEAPONS = 19,
    FILTER_GEMS = 20,
    FILTER_RUBY = 21,
    FILTER_TOPAZ = 22,
    FILTER_EMERALD = 23,
    FILTER_AMETHYST = 24,
    FILTER_DIAMOND = 25,
    FILTER_UPGRADEABLE_JEWEL = 26,
    FILTER_UNK_27 = 27,  // 2.7.7: icon "LegendaryGems" (table 0x710109efc0)
    FILTER_UNK_28 = 28,  // 2.7.7: icon "Bag"
    FILTER_SHARD = 29,
    FILTER_JUNK = 30,
    FILTER_SALVAGEABLE = 31,
    FILTER_MASS_SALVAGEABLE = 32,
    FILTER_CRAFTING_MATERIALS = 33,
    FILTER_MISCELLANEOUS_ITEMS = 34,
    FILTER_TREASURE = 35,
    FILTER_RIFT_KEYS = 36,
    FILTER_COSMETICS = 37,
    FILTER_WINGS = 38,
    FILTER_PETS = 39,
    FILTER_PENNANTS = 40,
    FILTER_FRAMES = 41,
    FILTER_FOLLOWER_START = 42,
    FILTER_FOLLOWER_MAIN_HAND = 42,
    FILTER_FOLLOWER_OFF_HAND = 43,
    FILTER_FOLLOWER_RING_LEFT = 44,
    FILTER_FOLLOWER_RING_RIGHT = 45,
    FILTER_FOLLOWER_NECK = 46,
    FILTER_FOLLOWER_HEAD = 47,  // 2.7.7 follower armour (SLOT_HIRELING_* order)
    FILTER_FOLLOWER_CHEST = 48,
    FILTER_FOLLOWER_HANDS = 49,
    FILTER_FOLLOWER_BELT = 50,
    FILTER_FOLLOWER_FEET = 51,
    FILTER_FOLLOWER_SHOULDERS = 52,
    FILTER_FOLLOWER_LEGS = 53,
    FILTER_FOLLOWER_WRIST = 54,
    FILTER_FOLLOWER_SPECIAL = 55,
    FILTER_FOLLOWER_END = 55,
    FILTER_FOLLOWER_SKILL_START = 56,
    FILTER_FOLLOWER_SKILL_1 = 56,
    FILTER_FOLLOWER_SKILL_2 = 57,
    FILTER_FOLLOWER_SKILL_3 = 58,
    FILTER_FOLLOWER_SKILL_4 = 59,
    FILTER_FOLLOWER_SKILL_END = 59,
    FILTER_RECIPES_START = 60,
    FILTER_JEWELER_RECIPES_START = 60,
    FILTER_JEWELER_RECIPES_RUBY = 60,
    FILTER_JEWELER_RECIPES_TOPAZ = 61,
    FILTER_JEWELER_RECIPES_EMERALD = 62,
    FILTER_JEWELER_RECIPES_AMETHYST = 63,
    FILTER_JEWELER_RECIPES_DIAMOND = 64,
    FILTER_JEWELER_RECIPES_END = 64,
    FILTER_BLACKSMITH_RECIPES_WEAPONS_START = 65,
    FILTER_BLACKSMITH_RECIPES_SWORD = 65,
    FILTER_BLACKSMITH_RECIPES_CROSSBOW = 66,
    FILTER_BLACKSMITH_RECIPES_DAGGER = 67,
    FILTER_BLACKSMITH_RECIPES_WAND = 68,
    FILTER_BLACKSMITH_RECIPES_MACE = 69,
    FILTER_BLACKSMITH_RECIPES_OFF_HAND = 70,
    FILTER_BLACKSMITH_RECIPES_MIGHTY = 71,
    FILTER_BLACKSMITH_RECIPES_STAFF = 72,
    FILTER_BLACKSMITH_RECIPES_AXE = 73,
    FILTER_BLACKSMITH_RECIPES_FIST = 74,
    FILTER_BLACKSMITH_RECIPES_BOW = 75,
    FILTER_BLACKSMITH_RECIPES_FLAIL = 76,
    FILTER_BLACKSMITH_RECIPES_SCYTHE = 77,
    FILTER_BLACKSMITH_RECIPES_WEAPONS_END = 77,
    FILTER_BLACKSMITH_RECIPES_ARMOR_START = 78,
    FILTER_BLACKSMITH_RECIPES_RING_LEFT = 78,
    FILTER_BLACKSMITH_RECIPES_RING_RIGHT = 79,
    FILTER_BLACKSMITH_RECIPES_NECK = 80,
    FILTER_BLACKSMITH_RECIPES_HANDS = 81,
    FILTER_BLACKSMITH_RECIPES_SHOULDERS = 82,
    FILTER_BLACKSMITH_RECIPES_CHEST = 83,
    FILTER_BLACKSMITH_RECIPES_HEAD = 84,
    FILTER_BLACKSMITH_RECIPES_BELT = 85,
    FILTER_BLACKSMITH_RECIPES_LEGS = 86,
    FILTER_BLACKSMITH_RECIPES_FEET = 87,
    FILTER_BLACKSMITH_RECIPES_WRIST = 88,
    FILTER_BLACKSMITH_RECIPES_OTHER = 89,
    FILTER_BLACKSMITH_RECIPES_ARMOR_END = 89,
    FILTER_RECIPES_END = 89,
    FILTER_RECIPES_COUNT = 30,
    FILTER_COUNT = 90,
};

typedef int32 UIFilterType;

namespace UIInventory {
namespace Console {
typedef BOOL (*ItemFilterPredicate)(const ActorCommonData &, ACDID, const UIFilterType);

typedef void (*ItemPartitioner)(ACDIDArray &, UIInventory::Console::NewItemIndicatorManager<ACDID> *);

class FilteredInventoryView {  // size 0x7c8
public:
    enum {
        FIV_INCLUDE_EQUIPPED_BIT = 0,
        FIV_INCLUDE_EQUIPPED_FOLLOWER_BIT = 1,
        FIV_INCLUDE_UNIDENTIFIED_BIT = 2,
        FIV_ONLY_REPAIRABLE_BIT = 3,
        FIV_SHOW_EQUIPPED_LAST_BIT = 4,
        FIV_EXCLUDE_OFFERED_ITEMS_BIT = 5,
        FIV_INCLUDE_JUNK_BIT = 6,
        FIV_INCLUDE_PLAYER_GIFTS_BIT = 7,
        FIV_EXCLUDE_NON_ENCHANTABLE_BIT = 8,
        FIV_EXCLUDE_ACCOUNT_BOUND_BIT = 9,
        FIV_EXCLUDE_TIERED_RIFT_KEYS_BIT = 10,
    };
public:
    FilteredInventoryView() : m_ptNewItemMgr(NULL), m_ptNewEliteItemMgr(NULL), m_dwFlags(0) {}
    ~FilteredInventoryView() {}
    void SetNewItemIndicators(NewItemIndicatorManager<ACDID> * ptNewItemMgr, NewItemIndicatorManager<ACDID> * ptNewEliteItemMgr)
    { m_ptNewItemMgr = ptNewItemMgr; m_ptNewEliteItemMgr = ptNewEliteItemMgr; }
    void ClearItems();
    void AddItems(const ACDID idACDInvSource, const ACDID idACDFilterTarget, const InventorySlotType eSlotType, const uint32 dwFlags, const ItemQualityLevel eMinimumItemQuality);
    const ACDIDArray & GetItems(const UIFilterType eFilterType) const;
    int32 GetIndexOfItem(const ACDID, const UIFilterType) const;
    static UIInventory::Console::ItemFilterPredicate GetPredicateForType(const UIFilterType eFilterType);
private:
    ACDIDArray m_arNoItems;  // 0x00
    ACDIDArray m_arAllItems;  // 0x18
    ACDIDArray m_arFilteredItems[FILTER_COUNT];  // 0x30
    UIInventory::Console::NewItemIndicatorManager<ACDID> * m_ptNewItemMgr;  // 0x7b0
    UIInventory::Console::NewItemIndicatorManager<ACDID> * m_ptNewEliteItemMgr;  // 0x7b8
    uint32 m_dwFlags;  // 0x7c0
    static UIInventory::Console::ItemFilterPredicate m_fnSlotFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnFollowerSlotFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnLabelFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnBagFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnDamagedFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnAvailableSocketsFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnSocketedFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnCanSocketUpgradeableJewelFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnDyeableFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnCanAddSocketsFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnGemsFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnShardFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnJunkItemFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnSalvageableFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnMassSalvageableFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnCraftingMaterialsFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnMiscellaneousItemsFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnTreasureFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnRiftKeysFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnUpgradeableJewelFilter;
    static UIInventory::Console::ItemFilterPredicate m_fnCosmeticsFilter;
    static UIInventory::Console::ItemPartitioner m_fnItemPartitioner;
    static UIInventory::Console::ItemPartitioner m_fnBagPartitioner;
    static UIInventory::Console::ItemPartitioner m_fnGemsPartitioner;
    static UIInventory::Console::ItemPartitioner m_fnAvailableSocketsPartitioner;
    static UIInventory::Console::ItemPartitioner m_fnEquippedLastItemPartitioner;
    static UIInventory::Console::ItemPartitioner GetPartitionerForType(const UIFilterType eFilterType, uint32 dwFlags);
    void UpdateFilteredItems(const ActorCommonData & tACDOwner);
    void sUpdateItemsForFilter(const UIFilterType eFilterType, ACDIDArray & arFilteredItems, const ActorCommonData & tACDOwner);
};
static_assert(sizeof(FilteredInventoryView) == 0x8b8, "FilteredInventoryView");

class RemovedItemPredicate {  // size 0x8
public:
    RemovedItemPredicate();
    RemovedItemPredicate(InventorySlotType);
    RemovedItemPredicate(InventorySlotType, InventorySlotType);
    BOOL operator()(const ACDID idItemACD);
private:
    InventorySlotType m_eBeginSlot;  // 0x00
    InventorySlotType m_eEndSlot;  // 0x04
};
static_assert(sizeof(RemovedItemPredicate) == 0x8, "RemovedItemPredicate");
// The default constructor is inlined into its callers (no out-of-line copy exists).
inline RemovedItemPredicate::RemovedItemPredicate()
    : m_eBeginSlot(0)
    , m_eEndSlot(0)
{
}

// Inlined into the follower screen's UpdateRingNewItemIndicators (no out-of-line copy exists).
inline RemovedItemPredicate::RemovedItemPredicate(InventorySlotType eBeginSlot, InventorySlotType eEndSlot)
    : m_eBeginSlot(eBeginSlot)
    , m_eEndSlot(eEndSlot)
{
}


LPCSTR GetIconNameFromFilterType(const UIFilterType eFilterType);
// X/Client/UserInterface/Console/UIConsoleInventoryFilters.cpp:279
InventorySlotType GetSlotForFilterType(const UIFilterType eFilterType);
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
ACDID GetEquippedItem(const Player * ptPlayer, const UIFilterType eFilterType, const BOOL fFallbackTo2HMainHand);

BOOL FilterUsesAltSlotForComparisons(const UIFilterType eFilter);
BOOL ItemCanBeWornByThisPlayer(const ActorCommonData & tACDOwner, const ActorCommonData & tACDItem);
void SetFilterJewel(const ACDID idJewel);
BOOL FilterHasNewItems(const UIFilterType eFilterType, const ACDID idPlayerACD,
    const NewItemIndicatorManager<ACDID> & tNewItemsMgr, const BOOL bIncludeEquipped);
}  // namespace Console
}  // namespace UIInventory
