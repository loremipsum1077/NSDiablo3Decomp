// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleInventoryView.h
// Inventory view lifecycle and provider subsets, recovered from original callers.
// Inline bodies are hand-written from DWARF and matching original instructions.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleNewItemIndicators.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"

namespace D3 { namespace Leaderboard { class HeroSnapshot; } }
namespace UIInventory { namespace Console { class UIInventoryScreen; } }
namespace UILeaderboardInspect { namespace Console { class DetailsInvScreen; } }
BOOL ItemCrafting_IsValidTransmogSlot(InventorySlotType eSlot);  // X/Common/Crafting/ItemCrafting.h

typedef int32 SoundID;
typedef int32 PlayerClass;

// X/Common/Items.cpp
BOOL ItemIsUnidentified(const ActorCommonData & tACDItem);

namespace UIInventory {
namespace Console {
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
float GetIdentifyingProgress(uint32 dwEndTime, uint32 dwDuration);
BOOL SendIdentifyRequest(const Player * ptPlayer, ACDID idACDItem);
}  // namespace Console
}  // namespace UIInventory

namespace UIInventory {
namespace Console {
enum {
    RESET_SELECTION = 0,
    KEEP_ITEM_SELECTED = 1,
    KEEP_INDEX_SELECTED = 2,
};

typedef int32 MaintainSelectionOptions;

class UIInventoryViewItemsProvider {  // size 0x20
public:
    enum ProviderType {
        ITEM_PROVIDER_REAL_ITEMS = 0,
        ITEM_PROVIDER_BLACKSMITH_RECIPES = 1,
        ITEM_PROVIDER_JEWELER_RECIPES = 2,
        ITEM_PROVIDER_MYSTIC_TRANSMOGS = 3,
        ITEM_PROVIDER_BLOODSHARD_VENDOR = 4,
        ITEM_PROVIDER_COSMETIC_ITEMS = 5,
        ITEM_PROVIDER_CRAFTING_MATERIALS = 6,
        ITEM_PROVIDER_NO_PLAYER_ITEMS = 7,
        ITEM_PROVIDER_LOADOUT_ITEMS = 8,
    };

    UIInventoryViewItemsProvider()  // line 65
        : m_eFilterType(FILTER_NONE)
        , m_ptPlayer(NULL)
        , m_idACDOwner(ACDID_INVALID)
    {
    }
    virtual ~UIInventoryViewItemsProvider() {}  // vtable slot 0

    Player * GetPlayer() const  // line 73
    {
        return m_ptPlayer;
    }

    void SetPlayer(Player * ptPlayer)  // line 79
    {
        if (ptPlayer == NULL)
        {
            m_ptPlayer = NULL;
            SetOwner(ACDID_INVALID);
            // DWARF: OnPlayerChanged vcall is on SetPlayer line 86, not inside SetOwner (UIConsoleFollower OnOpen 0x131478)
            OnPlayerChanged();
        }
        else if (GetOwner() != ptPlayer->idPlayerACD)
        {
            m_ptPlayer = ptPlayer;
            SetOwner(ptPlayer->idPlayerACD);
            // DWARF: OnPlayerChanged vcall is on SetPlayer line 99 (UIConsoleFollower OnOpen 0x131448)
            OnPlayerChanged();
        }
    }

    virtual CRefString GetItemName(const int32 nIndex) const  // vtable slot 2, line 104
    {
        return CRefString("");
    }
    virtual int32 GetRequiredLevel(const int32 nIndex) const  // vtable slot 3, line 112
    {
        return 0;
    }
    virtual CRefString GetItemDescription(const ACDID idACDItem, const BOOL bShowStatRanges) const;  // vtable slot 4
    virtual Blizzard::Time::Timestamp GetItemExpiration(const int32 nIndex) const  // vtable slot 5, line 124
    {
        return 0;
    }

    ACDID GetOwner()  // line 131
    {
        return m_idACDOwner;
    }

    void SetOwner(ACDID idACDOwner)  // line 137
    {
        // DWARF: SetOwner has only the line-140 store; inlined alone at UIConsoleFollower.cpp:629/633/636 it emits no call
        m_idACDOwner = idACDOwner;
    }

    UIFilterType GetFilter() const  // line 143
    {
        return m_eFilterType;
    }

    void SetFilter(const UIFilterType eFilterType)  // line 149
    {
        m_eFilterType = eFilterType;
    }

    const ACDIDArray & GetItems() const  // line 155
    {
        return GetItemsWithFilter(m_eFilterType);
    }

    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType) const = 0;  // vtable slot 6
    virtual ProviderType GetProviderType() const  // vtable slot 7, line 167
    {
        return ITEM_PROVIDER_REAL_ITEMS;
    }
    virtual BOOL CanProvideUnidentifiedItems() const  // vtable slot 8, line 173
    {
        return TRUE;
    }
    virtual BOOL ItemIsNew(const ACDID idItem) const  // vtable slot 9, line 179
    {
        if (m_ptPlayer != NULL)
        {
            return m_ptPlayer->ptNewItemManager->ItemIsNew(idItem);
        }
        return FALSE;
    }
    virtual BOOL ItemIsNewElite(const ACDID idItem) const  // vtable slot 10, line 190
    {
        if (m_ptPlayer != NULL)
        {
            return m_ptPlayer->ptNewEliteItemManager->ItemIsNew(idItem);
        }
        return FALSE;
    }
    virtual void UnMarkAsNew(const ACDID idItem)  // vtable slot 11, line 201
    {
        if (m_ptPlayer != NULL)
        {
            m_ptPlayer->ptNewItemManager->UnMarkAsNew(idItem);
            m_ptPlayer->ptNewEliteItemManager->UnMarkAsNew(idItem);
        }
    }
    virtual BOOL IdentifyIsAllowed()  // vtable slot 12, line 211
    {
        return TRUE;
    }
    virtual CRefString GetWarningMessage() const  // vtable slot 13, line 217
    {
        return CRefString("");
    }
protected:
    virtual void OnPlayerChanged() {}  // vtable slot 14, line 224
private:
    UIFilterType m_eFilterType;  // 0x08
    Player * m_ptPlayer;  // 0x10
    ACDID m_idACDOwner;  // 0x18
};
static_assert(sizeof(UIInventoryViewItemsProvider) == 0x20, "UIInventoryViewItemsProvider");

class InventoryItems : public UIInventory::Console::UIInventoryViewItemsProvider {  // size 0x7f8
public:
    enum ProviderMode {
        PROVIDERMODE_BACKPACK = 0,
        PROVIDERMODE_STASH = 1,
        PROVIDERMODE_BACKPACK_AND_STASH = 2,
        PROVIDERMODE_FOLLOWER = 3,
        PROVIDERMODE_REPAIR = 4,
        PROVIDERMODE_TRADE_SELECT = 5,
        PROVIDERMODE_MAIL_SELECT = 6,
        PROVIDERMODE_MYSTIC_ENCHANT = 7,
    };
    enum ItemOptionsFlags {
        INCLUDE_EQUIPPED_ITEMS = 0,
        EXCLUDE_UNIDENTIFIED_ITEMS = 1,
        EXCLUDE_JUNK_ITEMS = 2,
        EXCLUDE_PLAYER_GIFTS = 3,
        EXCLUDE_NON_ENCHANTABLE_ITEMS = 4,
        SORT_EQUIPPED_ITEMS_LAST = 5,
        EXCLUDE_TIERED_RIFT_KEYS = 6,
    };
    typedef uint32 ItemOptions;
public:
    InventoryItems(const ProviderMode eMode) : m_dwItemOptions(0), m_eProviderMode(eMode), m_eMinimumItemQuality(0) {}
    void SetItemOptions(const ItemOptions dwItemOptions);
    void SetProviderMode(ProviderMode eProviderMode);
    void SetMinimumItemQuality(ItemQualityLevel eMinimumItemQuality);
    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType eFilterType) const { return m_tView.GetItems(eFilterType); }  // vtable slot 6
    void GetBagSpace(int32 & nUsed, int32 & nTotal);
    void Refresh();
protected:
    virtual void OnPlayerChanged()
    {
        NewItemIndicatorManager<ACDID> * ptNewItemMgr = GetPlayer() ? GetPlayer()->ptNewItemManager : NULL;
        NewItemIndicatorManager<ACDID> * ptNewEliteItemMgr = GetPlayer() ? GetPlayer()->ptNewEliteItemManager : NULL;
        m_tView.SetNewItemIndicators(ptNewItemMgr, ptNewEliteItemMgr);
        Refresh();
    }  // vtable slot 14
private:
    UIInventory::Console::FilteredInventoryView m_tView;  // 0x20
    UIInventory::Console::InventoryItems::ItemOptions m_dwItemOptions;  // 0x7e8
    UIInventory::Console::InventoryItems::ProviderMode m_eProviderMode;  // 0x7ec
    ItemQualityLevel m_eMinimumItemQuality;  // 0x7f0
};
static_assert(sizeof(InventoryItems) == 0x8e8, "InventoryItems");

class UIInventoryView {  // size 0x60
public:
    UIInventoryView(const UIID &);
    friend class UIInventoryScreen;
    void Show(const BOOL bShow);
    void ShowBagSpace(InventoryItems & tItemsProvider, CRefString sBagLabel);
    void Enable(const BOOL bEnable) { if (m_bIsEnabled != bEnable) { m_bIsEnabled = bEnable; OnEnable(bEnable); } }
    BOOL IsActive() const { return m_bIsActive; }
    BOOL IsIdentifyingItem() const { return GetIdentifyingItemACD() != ACDID_INVALID; }
    void SetIdentifyingItemACD(const ACDID idItem) { if (m_idIdentifyingItemACD != idItem) m_idIdentifyingItemACD = idItem; }
    const UIID & GetStatusLineUIID() const { return m_idStatusLine; }
    void SetParentScreen(UIInventoryScreen * ptParentScreen) { m_ptParentScreen = ptParentScreen; OnParentScreenChanged(); }
    virtual ~UIInventoryView() {}  // vtable slot 0; line 247
    BOOL IsFocused() const;  // line 268; body in UIConsoleInventoryScreen.h, which completes UIInventoryScreen
    virtual BOOL CanAcceptFocus() const  // vtable slot 2; line 281
    {
        return m_bIsActive && m_bIsEnabled;
    }

    ACDID GetSelectedItem() const  // line 287
    {
        return m_idSelectedItem;
    }

    void SelectItem(const ACDID idItem)  // line 293
    {
        if (m_idSelectedItem != idItem)
        {
            m_idSelectedItem = idItem;
            OnSelectedItemChanged();
        }
    }

    void ClearSelectedItem()  // line 303
    {
        m_idSelectedItem = ACDID_INVALID;
    }

    void DoneIdentifying(const BOOL fSuccess);

    ACDID GetIdentifyingItemACD() const  // line 321
    {
        return m_idIdentifyingItemACD;
    }

    void SetHeaderText(CRefString sText)  // line 327
    {
        m_sHeaderText = sText;
    }

    const UIID & GetRootUIID() const  // line 335
    {
        return m_idRoot;
    }

    UIInventoryScreen * GetParentScreen() const  // line 342
    {
        return m_ptParentScreen;
    }

    void Update();

    void RequestUpdate(const MaintainSelectionOptions eMaintainSelectionOptions)  // line 361
    {
        m_fUpdateRequestedFlag = TRUE;
        m_eMaintainSelectionOptions = eMaintainSelectionOptions;
    }

    void ForceUpdate(const MaintainSelectionOptions eMaintainSelectionOptions)  // line 369
    {
        RequestUpdate(eMaintainSelectionOptions);
        Update();
    }

    virtual BOOL CanScrollWithTriggers() const;  // vtable slot 3
    virtual UIID GetScrollBoxUIID() const;  // vtable slot 4
protected:
    void SetStatusLineUIID(const UIID & idStatusLine)  // line 402
    {
        m_idStatusLine = idStatusLine;
    }

    virtual void OnParentScreenChanged() {}  // vtable slot 5
    virtual void OnShow(const BOOL) = 0;  // vtable slot 6
    virtual void OnEnable(const BOOL bEnable) {}  // vtable slot 7
    virtual void OnGotFocus() {}  // vtable slot 8
    virtual void OnLostFocus() {}  // vtable slot 9
    virtual void OnSelectedItemChanged() {}  // vtable slot 10
    virtual void OnRequestedUpdate(const MaintainSelectionOptions) {}  // vtable slot 11
    virtual void OnUpdate() {}  // vtable slot 12
    virtual UIID GetHeaderUIID() const;  // vtable slot 13
    void StartIdentifying();
    void SetIsIdentifying(const BOOL bIsIdentifying) { m_bIsIdentifying = bIsIdentifying; m_bIdentifyingRequestSent = FALSE; }

    float GetIdentifyingProgress()  // line 475
    {
        return UIInventory::Console::GetIdentifyingProgress(m_dwIdentifyingEndTime, IDENTIFYING_DURATION);
    }

    BOOL HasSendIdentifyingRequest()  // line 481
    {
        return m_bIdentifyingRequestSent;
    }

    void SendIdentifyingRequest();  // line 487; body in UIConsoleInventoryScreen.h, which completes UIInventoryScreen

    BOOL IsIdentifying() const  // line 509
    {
        return m_bIsIdentifying;
    }

    BOOL IsFinishedIdentifying() const  // line 516
    {
        if (IsIdentifying())
        {
            ACDID idACDItem = GetIdentifyingItemACD();
            if (idACDItem != ACDID_INVALID)
            {
                ActorCommonData * ptItem = ACDTryToGet(idACDItem);
                if (ptItem != NULL && ItemIsUnidentified(*ptItem))
                {
                    return FALSE;
                }
            }
        }
        return TRUE;
    }
private:
    UIInventoryScreen * m_ptParentScreen;  // 0x08
    const UIID & m_idRoot;  // 0x10
    BOOL m_bIsActive;  // 0x18
    BOOL m_bIsEnabled;  // 0x1c
    UIID m_idStatusLine;  // 0x20
    BOOL m_fUpdateRequestedFlag;  // 0x28
    MaintainSelectionOptions m_eMaintainSelectionOptions;  // 0x2c
    CRefString m_sHeaderText;  // 0x30
    ACDID m_idSelectedItem;  // 0x48
    ACDID m_idIdentifyingItemACD;  // 0x4c
    BOOL m_bIsIdentifying;  // 0x50
    BOOL m_bIdentifyingRequestSent;  // 0x54
    uint32 m_dwIdentifyingEndTime;  // 0x58
    SoundID m_idSoundLoop;  // 0x5c
    static const uint32 IDENTIFYING_DURATION;
};
static_assert(sizeof(UIInventoryView) == 0x60, "UIInventoryView");

class RecipeItemsProvider : public UIInventory::Console::UIInventoryViewItemsProvider {  // size 0x20
public:
    virtual ~RecipeItemsProvider() {}  // vtable slot 0
    virtual const GBIDArray & GetRecipes() const = 0;  // vtable slot 15
    virtual CRefString GetItemName(const int32 nIndex) const;  // vtable slot 2
    virtual CRefString GetItemDescription(const ACDID idACDItem, const BOOL bShowStatRanges) const;  // vtable slot 4
    virtual int32 GetIndexOfItem(const ACDID) const;  // vtable slot 16
    virtual UIInventory::Console::UIInventoryViewItemsProvider::ProviderType GetProviderType() const;  // vtable slot 7
    virtual BOOL CanProvideUnidentifiedItems() const;  // vtable slot 8
    virtual void Refresh() = 0;  // vtable slot 17
protected:
    virtual void OnPlayerChanged();  // vtable slot 14
};
static_assert(sizeof(RecipeItemsProvider) == 0x20, "RecipeItemsProvider");


class CrafterRecipes : public UIInventory::Console::RecipeItemsProvider {  // size 0x5e0
    enum RecipeOptionslags {
        EXCLUDE_RECIPES_BY_CLASS = 0,
        EXCLUDE_RECIPES_BY_MATERIALS = 1,
    };
    typedef uint32 RecipeOptions;
public:
    CrafterRecipes();
    virtual ~CrafterRecipes() { CleanupACDs(); }  // vtable slot 0
    void SetRecipeOptions(const UIInventory::Console::CrafterRecipes::RecipeOptions dwRecipeOptions);
    virtual const GBIDArray & GetRecipes() const { return GetFilter() < FILTER_RECIPES_START ? m_arrRecipes[0] : m_arrRecipes[GetFilter() - FILTER_RECIPES_START]; }  // vtable slot 15
    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType eFilterType) const { return eFilterType < FILTER_RECIPES_START ? m_arrEmpty : m_arrItems[eFilterType - FILTER_RECIPES_START]; }  // vtable slot 6
    virtual int32 GetIndexOfItem(const ACDID idItem) const {
        int32 nIndex = GetFilter() - FILTER_RECIPES_START;
        for (int32 i = 0; i < (int32)m_arrItems[nIndex].size(); ++i)
            if (m_arrItems[nIndex][i] == idItem) return i;
        return -1;
    }  // vtable slot 16
    virtual UIInventory::Console::UIInventoryViewItemsProvider::ProviderType GetProviderType() const;  // vtable slot 7
    virtual BOOL CanProvideUnidentifiedItems() const;  // vtable slot 8
    virtual BOOL ItemIsNew(const ACDID idItem) const {
        if (GetPlayer()) {
            int32 nIndex = GetIndexOfItem(idItem);
            if (nIndex != -1) {
                GBID gbidRecipe = GetRecipes()[nIndex];
                return GetPlayer()->ptNewRecipeManager->ItemIsNew(gbidRecipe);
            }
        }
        return FALSE;
    }  // vtable slot 9
    virtual void UnMarkAsNew(const ACDID idItem) {
        if (GetPlayer()) {
            int32 nIndex = GetIndexOfItem(idItem);
            if (nIndex != -1) {
                GBID gbidRecipe = GetRecipes()[nIndex];
                GetPlayer()->ptNewRecipeManager->UnMarkAsNew(gbidRecipe);
            }
        }
    }  // vtable slot 11
    virtual void Refresh();  // vtable slot 17
    virtual int32 GetRequiredLevel(const int32 nIndex) const {
        ScopedRecipeEntryLockR pdefRecipeEntry(GetRecipes()[nIndex]);
        return pdefRecipeEntry->nLevel;
    }  // vtable slot 3
    static BOOL FilterIncludesItem(const GBID gbidItem, const UIFilterType eFilter);
private:
    ACDIDArray m_arrEmpty;  // 0x20
    ACDIDArray m_arrItems[30];  // 0x38
    GBIDArray m_arrRecipes[30];  // 0x308
    UIInventory::Console::CrafterRecipes::RecipeOptions m_dwRecipeOptions;  // 0x5d8
    void CleanupACDs();
    static BOOL OptionsApplyToItem(const Player & tPlayer, const GBID gbidRecipe, const GBID gbidItem, const UIInventory::Console::CrafterRecipes::RecipeOptions dwRecipeOptions);
    static PFN_COMPARE GetSortFuncForFilter(const UIFilterType eFilter);
};
static_assert(sizeof(CrafterRecipes) == 0x5e0, "CrafterRecipes");


class CosmeticItemsProvider : public UIInventory::Console::RecipeItemsProvider {  // size 0xe8
public:
    CosmeticItemsProvider() {}  // DWARF UIConsoleInventoryView.h:1000, inlined only
    virtual ~CosmeticItemsProvider() { CleanupACDs(); }  // vtable slot 0
    virtual const GBIDArray & GetRecipes() const { return m_arrRecipes; }  // vtable slot 15
    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType eFilterType) const {
        if (eFilterType == FILTER_COSMETICS) return m_arrItems;
        if (eFilterType >= FILTER_WINGS && eFilterType <= FILTER_FRAMES) return m_arrFilteredItems[eFilterType - FILTER_WINGS];
        return m_arrEmpty;
    }  // vtable slot 6
    virtual int32 GetIndexOfItem(const ACDID idItem) const {
        for (int32 i = 0; i < (int32)m_arrItems.size(); ++i) if (m_arrItems[i] == idItem) return i;
        return -1;
    }  // vtable slot 16
    virtual UIInventory::Console::UIInventoryViewItemsProvider::ProviderType GetProviderType() const;  // vtable slot 7
    virtual BOOL CanProvideUnidentifiedItems() const;  // vtable slot 8
    int32 VirtualItemIndex(const ACDID idItem) const { for (int32 i = 0; i < (int32)m_arrVirtualItems.size(); ++i) if (m_arrVirtualItems[i] == idItem) return i; return -1; }  // DWARF UIConsoleInventoryView.h:1048, inlined only
    // NON_MATCHING: 392 bytes versus 364; equivalent guards produce extra return blocks.
    virtual BOOL ItemIsNew(const ACDID idItem) const {
        if (idItem == ACDID_INVALID || !GetPlayer()) return FALSE;
        ActorCommonData * ptACD = ACDTryToGet(idItem);
        return ptACD && GetPlayer()->ptNewCosmeticManager->ItemIsNew(ptACD->hGB.gbid);
    }  // vtable slot 9
    virtual void UnMarkAsNew(const ACDID idItem) {
        if (idItem != ACDID_INVALID && GetPlayer()) {
            ActorCommonData * ptACD = ACDTryToGet(idItem);
            if (ptACD) GetPlayer()->ptNewCosmeticManager->UnMarkAsNew(ptACD->hGB.gbid);
        }
    }  // vtable slot 11
    virtual CRefString GetItemName(const int32 nIndex) const;  // vtable slot 2
    virtual void Refresh();  // vtable slot 17
    virtual int32 GetRequiredLevel(const int32 nIndex) const { return 1; }  // vtable slot 3
    static BOOL FilterIncludesItem(const GBID gbidItem, const UIFilterType eFilter);
    void SetProviderMode(UIInventory::Console::InventoryItems::ProviderMode eProviderMode);
private:
    ACDIDArray m_arrEmpty;  // 0x20
    ACDIDArray m_arrItems;  // 0x38
    ACDIDArray m_arrFilteredItems[4];  // 0x50
    ACDIDArray m_arrVirtualItems;  // 0xb0
    GBIDArray m_arrRecipes;  // 0xc8
    UIInventory::Console::InventoryItems::ProviderMode m_eProviderMode;  // 0xe0
    void CleanupACDs();
    static PFN_COMPARE GetSortFuncForFilter(const UIFilterType);
};
static_assert(sizeof(CosmeticItemsProvider) == 0xe8, "CosmeticItemsProvider");

class CraftingItem : public UIInventory::Console::UIInventoryViewItemsProvider {  // size 0x48
public:
    CraftingItem();
    virtual ~CraftingItem() {}
    void OnCraftingStarted(const GBID gbidRecipe, const ACDID idRecipeItem);
    void OnCraftingFinished(const ACDID idCraftedItem);
    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType eFilterType) const;  // vtable slot 6
    virtual UIInventory::Console::UIInventoryViewItemsProvider::ProviderType GetProviderType() const;  // vtable slot 7
    virtual BOOL CanProvideUnidentifiedItems() const { return FALSE; }  // vtable slot 8
    GBID GetRecipe() const;
    ACDID GetRecipeItem() const;
    ACDID GetCraftedItem() const;
    virtual CRefString GetItemName(const int32 nIndex) const;  // vtable slot 2
    enum CraftingStage {
        BEFORE_CRAFTING = 0,
        STARTED_CRAFTING = 1,
        FINISHED_CRAFTING = 2,
    };
private:
    mutable ACDIDArray m_aridItem;  // 0x20
    ACDID m_idRecipeItem;  // 0x38
    GBID m_gbidRecipe;  // 0x3c
    ACDID m_idCraftedItem;  // 0x40
    UIInventory::Console::CraftingItem::CraftingStage m_eCraftingStage;  // 0x44
};
static_assert(sizeof(CraftingItem) == 0x48, "CraftingItem");


class TransmogableItems : public UIInventory::Console::UIInventoryViewItemsProvider {  // size 0x8d8
public:
    // line 1208
    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType eFilterType) const  // vtable slot 6
    {
        UIFilterType eRealFilterType = eFilterType;
        if (!ItemCrafting_IsValidTransmogSlot(GetSlotForFilterType(eFilterType)))
        {
            eRealFilterType = FILTER_NONE;
        }
        return m_tView.GetItems(eRealFilterType);
    }
    void Refresh();
private:
    UIInventory::Console::FilteredInventoryView m_tView;  // 0x20
};
static_assert(sizeof(TransmogableItems) == 0x8d8, "TransmogableItems");

class DyeableItems : public UIInventory::Console::UIInventoryViewItemsProvider {  // size 0x8d8
public:
#line 1235 "X/Client/UserInterface/Console/UIConsoleInventoryView.h"
    virtual const ACDIDArray & GetItemsWithFilter(
        const UIFilterType eFilterType) const
    {
        UIFilterType arAllowableFilters[] = {
            FILTER_HANDS,
            FILTER_SHOULDERS,
            FILTER_CHEST,
            FILTER_HEAD,
            FILTER_LEGS,
            FILTER_FEET
        };

        UIFilterType eRealFilterType = eFilterType;



        if (blz::find(arAllowableFilters, arAllowableFilters + 6, eRealFilterType) == arAllowableFilters + 6)
        {
            eRealFilterType = FILTER_NONE;
        }

        return m_tView.GetItems(eRealFilterType);
    }
#line 554 "X/Client/UserInterface/Console/UIConsoleInventoryView.h"
    void Refresh();
private:
    UIInventory::Console::FilteredInventoryView m_tView;  // 0x20
};
static_assert(sizeof(DyeableItems) == 0x8d8, "DyeableItems");


class EquippedItem : public UIInventoryViewItemsProvider {  // size 0x38
public:
    virtual ~EquippedItem() {}  // line 676

    EquippedItem()  // line 679
    {
        m_arEquippedItems.push_back(ACDID_INVALID);
    }

    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType eFilterType) const  // vtable slot 6; line 685
    {
        ACDID idEquippedItem = GetEquippedItem(GetPlayer(), eFilterType, TRUE);
        m_arEquippedItems[0] = idEquippedItem;
        return m_arEquippedItems;
    }
private:
    mutable ACDIDArray m_arEquippedItems;  // 0x20
};
static_assert(sizeof(EquippedItem) == 0x38, "EquippedItem");

class LeaderboardEquippedItem : public UIInventoryViewItemsProvider {  // size 0x190
    // DetailsInvScreen::Reset (UIConsoleLeaderboardInspect.cpp) calls the private CleanupACDs.
    friend class UILeaderboardInspect::Console::DetailsInvScreen;
public:
    LeaderboardEquippedItem();
    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType eFilterType) const;  // vtable slot 6
    virtual BOOL CanProvideUnidentifiedItems() const;  // vtable slot 8
    void OnClose();
    virtual ProviderType GetProviderType() const;  // vtable slot 7
    virtual CRefString GetItemDescription(const ACDID idACDItem, const BOOL bShowStatRanges) const;  // vtable slot 4

    PlayerClass GetClass()  // line 754
    {
        return m_ePlayerClass;
    }

    BOOL GetIsMale()  // line 760
    {
        return m_bIsMale;
    }
    void SetData(const D3::Leaderboard::HeroSnapshot & tHeroSnapshot, PlayerClass ePlayerClass);
private:
    void CleanupACDs();
    ACDIDArray m_arrItems[13];  // 0x20
    blz::vector<ACDID, blz::allocator<ACDID> > m_vecSocketACDs;  // 0x158
    ACDIDArray m_emptyArray;  // 0x170
    PlayerClass m_ePlayerClass;  // 0x188
    BOOL m_bIsMale;  // 0x18c
};
static_assert(sizeof(LeaderboardEquippedItem) == 0x190, "LeaderboardEquippedItem");

class CraftingMatItemsProvider : public UIInventory::Console::RecipeItemsProvider {  // size 0x70
public:
    CraftingMatItemsProvider();
    virtual ~CraftingMatItemsProvider() { CleanupACDs(); }  // vtable slot 0
    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType eFilterType) const { return eFilterType == FILTER_CRAFTING_MATERIALS ? m_arrVirtualItems : m_arrEmpty; }  // vtable slot 6
    virtual int32 GetIndexOfItem(const ACDID idItem) const {
        for (int32 i = 0; i < (int32)m_arrVirtualItems.size(); ++i) if (m_arrVirtualItems[i] == idItem) return i;
        return -1;
    }  // vtable slot 16
    virtual UIInventory::Console::UIInventoryViewItemsProvider::ProviderType GetProviderType() const;  // vtable slot 7
    virtual BOOL CanProvideUnidentifiedItems() const;  // vtable slot 8
    int32 VirtualItemIndex(const ACDID) const;
    virtual BOOL ItemIsNew(const ACDID idItem) const {
        // The original retains the ACD lookup's TLS access but discards its result.
        if (idItem != ACDID_INVALID && GetPlayer()) {
            ActorCommonData * ptACD = ACDTryToGet(idItem);
            (void)ptACD;
        }
        return FALSE;
    }  // vtable slot 9
    virtual void UnMarkAsNew(const ACDID idItem) {
        // The original retains the ACD lookup's TLS access but discards its result.
        if (idItem != ACDID_INVALID && GetPlayer()) {
            ActorCommonData * ptACD = ACDTryToGet(idItem);
            (void)ptACD;
        }
    }  // vtable slot 11
    virtual const GBIDArray & GetRecipes() const { return m_arrRecipes; }  // vtable slot 15
    virtual CRefString GetItemName(const int32 nIndex) const;  // vtable slot 2
    virtual void Refresh();  // vtable slot 17
    virtual int32 GetRequiredLevel(const int32 nIndex) const { return 1; }  // vtable slot 3
    static BOOL FilterIncludesItem(const GBID, const UIFilterType);
    void SetProviderMode(UIInventory::Console::InventoryItems::ProviderMode);
protected:
    ACDIDArray m_arrEmpty;  // 0x20
    ACDIDArray m_arrVirtualItems;  // 0x38
    GBIDArray m_arrRecipes;  // 0x50
    UIInventory::Console::InventoryItems::ProviderMode m_eProviderMode;  // 0x68
    void CleanupACDs();
};
static_assert(sizeof(CraftingMatItemsProvider) == 0x70, "CraftingMatItemsProvider");

// UIConsoleInventoryView.h:783; the out-of-line destructors and GetItemsWithFilter land in
// X/Client/UserInterface/Console/UIInventoryConsole.cpp.
class SingleItemProvider : public UIInventory::Console::UIInventoryViewItemsProvider {  // size 0x38
public:
    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType) const { return m_arSingleItem; }  // vtable slot 6; line 786
    void SetItem(const ACDID idItem);  // line 793
private:
    ACDIDArray m_arSingleItem;  // 0x20
};
static_assert(sizeof(SingleItemProvider) == 0x38, "SingleItemProvider");

// UIConsoleInventoryView.h:1271; no out-of-line copy exists (inlined into the mail screen's ctor).
inline CraftingMatItemsProvider::CraftingMatItemsProvider()
{
}

// UIConsoleInventoryView.h:623
inline void InventoryItems::SetProviderMode(ProviderMode eProviderMode)
{
    if (m_eProviderMode != eProviderMode)
    {
        m_eProviderMode = eProviderMode;
        Refresh();
    }
}

// UIConsoleInventoryView.h:613
inline void InventoryItems::SetItemOptions(const ItemOptions dwItemOptions)
{
    if (m_dwItemOptions != dwItemOptions)
    {
        m_dwItemOptions = dwItemOptions;
        Refresh();
    }
}

// UIConsoleInventoryView.h:793
inline void SingleItemProvider::SetItem(const ACDID idItem)
{
    if (idItem == ACDID_INVALID)
    {
        m_arSingleItem.clear();
    }
    else if (m_arSingleItem.size() == 0)
    {
        m_arSingleItem.push_back(idItem);
    }
    else
    {
        m_arSingleItem[0] = idItem;
    }
}

// UIConsoleInventoryView.h:869; no out-of-line copy exists.
inline CrafterRecipes::CrafterRecipes()
    : m_dwRecipeOptions(0)
{
}

// UIConsoleInventoryView.h:876
inline void CrafterRecipes::SetRecipeOptions(const RecipeOptions dwRecipeOptions)
{
    if (m_dwRecipeOptions != dwRecipeOptions)
    {
        m_dwRecipeOptions = dwRecipeOptions;
        Refresh();
    }
}

// UIConsoleInventoryView.h:1168
inline GBID CraftingItem::GetRecipe() const
{
    return m_gbidRecipe;
}

// UIConsoleInventoryView.h:1174
inline ACDID CraftingItem::GetRecipeItem() const
{
    return m_idRecipeItem;
}

// UIConsoleInventoryView.h:1180
inline ACDID CraftingItem::GetCraftedItem() const
{
    return m_idCraftedItem;
}

// UIConsoleInventoryView.h:1110
inline void CosmeticItemsProvider::SetProviderMode(UIInventory::Console::InventoryItems::ProviderMode eProviderMode)
{
    if (m_eProviderMode != eProviderMode)
    {
        m_eProviderMode = eProviderMode;
        Refresh();
    }
}

}  // namespace Console
}  // namespace UIInventory
