// Original file: X/Client/UserInterface/Console/UIShopConsole.cpp
// The console vendor screen: buy, sell, repair and buyback tabs.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Console/ControllerUtils.h"
#include "ConsoleUIRActorManager.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "ConsoleModalNotificationManager.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../Controls/UIRActorControl.h"
#include "../../../Sigma/Math/Quaternion.h"
#include "../../../Sigma/Math/Transform.h"
#include "UIConsoleInventoryCategorySummaryView.h"
#include "UIConsoleInventoryCategoryView.h"
#include "UIConsoleInventoryDetailsView.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryListView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "UIConsoleItemCategories.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../../RActors/RActors.h"
#include "../../../Common/Monsters.h"
#include "../../../Common/TagMaps/AnimTagsSwig.h"
#include "../Interface/UIConfirmation.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../CGame.h"
#include "../../Input/Input.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Items.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Sigma/RefString.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Common/Crafting/ItemCraftingSwig.h
enum {
    CRAFTER_INVALID = -1,
};
// X/Common/Networking/GameMessage.h
struct ANNDataMessage;
enum {
    CMSG_CLOSE_INTERACTION = 22,



};





// X/Common/Player.h





// X/Client/UserInterface/Interface/UIGame.cpp




namespace UIGame { void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer); }
// X/Common/Items.cpp
ItemSellError ItemCanBeSold(const ActorCommonData & tACDItem, const ActorCommonData * ptACDVendor);
BOOL ItemIsIrreplaceable(GBID gbidItem);
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
// X/Common/VisualInventory.cpp
VisualInventorySlot VisualInventorySlotFromInventorySlot(const InventorySlotType eInventorySlot);
// X/Client/Input/Input.cpp
void InputClearRemappings();
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/UserInterface/Console/UIConsoleStackSplit.cpp
namespace UIStackSplit {
namespace Console {
enum StackSplitType {
    STACKSPLIT_GENERIC = 0,
};
typedef void (*UIStackSplitCallback)(const ACDID, const uint64);
BOOL IsVisible();
void Close();
void Open(const ACDID idACDStack, const uint64 uInitialVal, const uint64 uStackMax, UIStackSplitCallback pfnCallback, StackSplitType eType);
}  // namespace Console
}  // namespace UIStackSplit
// X/Client/UserInterface/Console/UIConsoleRepair.cpp
namespace UIRepair {
namespace Console {
void RepairItem(Player * ptPlayer, const ACDID idItem);
void RepairAllItems(Player * ptPlayer, const BOOL bEquippedItemsOnly);
}  // namespace Console
}  // namespace UIRepair
// X/Common/ActorInventory.cpp
BuyResponse ACDInventoryCanBuy(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem);
// X/Client/UserInterface/Console/ConsoleScreenManager.h
namespace Console {
namespace Screens {
enum {
    SHOP_BUY_SCREEN = 0,
    SHOP_SELL_SCREEN = 1,
    SHOP_REPAIR_SCREEN = 2,
    SHOP_BUYBACK_SCREEN = 3,
};
}  // namespace Screens
}  // namespace Console
// X/Client/UserInterface/Console/UIConsoleRepair.cpp
namespace UIRepair { namespace Console { void UpdateRepairUI(Player * ptPlayer); } }
// X/Client/UserInterface/Console/UIConsoleInventory.cpp
namespace UIInventory { namespace Console { void ShowFilterName(const UIID & idText, const UIFilterType eFilter, const BOOL bHighlight); } }
// X/Client/Input/Input.cpp
void InputRemapSingleJoyconButton(int32 eButton, int32 eRemappedButton);
// X/Client/UserInterface/UI.cpp
UIRActor & UI_GetRActorFromID(const UIID & id);
SNO UI_GetDefaultHeroLight();
// X/Client/Render/Light.cpp
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);
// X/Common/Definitions/ActorDefinition.cpp
SNO ActorGetAnimSetSNO(const SNO snoActor);
// X/Client/RActors/RActorAnimation.cpp
BOOL RActorPlayAnimation(RActor * ptRActor, const uint32 dwTag, BOOL bDisableBlending, BOOL fOnlyPlayIfDifferent);
// X/Client/UserInterface/UIText.cpp
CRefString UIText_ValueToGameCurrency(const uint64 uAmount, CurrencyType eCurrencyType);
// X/Client/UserInterface/Interface/UIInventoryCommon.cpp
namespace UIInventory {
void UpdateGoldAmount(const UIID & idText, const ACDID idPlayerACD);
}  // namespace UIInventory
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
namespace UIInventory {
namespace Console {
enum InfoPanelType {
    NO_INFO_PANEL = -1,
    CHARACTER_INFO_PANEL = 0,
    REPAIR_SUMMARY_INFO_PANEL = 2,
    VENDOR_INFO_PANEL = 9,
};
void ClearStatusLine(UIInventoryView * ptView);
void SwitchToInfoPanel(const InfoPanelType eInfoPanel);
void UpdatePlayerInfoUI(const Player & tPlayer, const UIID & idParentControl, VisualInventory & tVisualInventory);
}  // namespace Console
}  // namespace UIInventory
// X/Client/UserInterface/UIGlobals.cpp
extern const UIID UIDLG_INVENTORY_CONSOLE, UIDLG_INVENTORY_CONSOLE_LIST, UIDLG_INVENTORY_CONSOLE_DETAILS,
    UIDLG_INVENTORY_CONSOLE_EQUIPPED;
extern const UIID UIRACTOR_CONSOLE_RING, UITXT_CONSOLE_PLAYER_LABEL_RING; extern const UIID UIBTN_INVENTORY_CONSOLE_RING_ARM_LEFT, UIBTN_INVENTORY_CONSOLE_RING_ARM_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_BAG, UIBTN_INVENTORY_CONSOLE_RING_BRACERS, UIBTN_INVENTORY_CONSOLE_RING_FEET, UIBTN_INVENTORY_CONSOLE_RING_FINGER_LEFT, UIBTN_INVENTORY_CONSOLE_RING_FINGER_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_HANDS, UIBTN_INVENTORY_CONSOLE_RING_HEAD, UIBTN_INVENTORY_CONSOLE_RING_LEGS, UIBTN_INVENTORY_CONSOLE_RING_NECK, UIBTN_INVENTORY_CONSOLE_RING_SHOULDER, UIBTN_INVENTORY_CONSOLE_RING_TORSO, UIBTN_INVENTORY_CONSOLE_RING_WAIST;
extern const UIID UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR, UIWND_INVENTORY_CONSOLE_RING_GLOW,
    UIWND_INVENTORY_CONSOLE_ROOT, UIWND_INVENTORY_CONSOLE_PANEL_FOCUS_HIGHLIGHT, UIWND_INVENTORY_CONSOLE_VENDOR_NAME;
extern UIID UITXT_INVENTORY_CONSOLE_HEADER, UITXT_INVENTORY_CONSOLE_GOLD; extern const UIID UITXT_INVENTORY_CONSOLE_GOLD_SPENT,  // DWARF: UIGlobals.cpp:3144 (HEADER), :3139 (GOLD); no header declarations recorded.
    UITXT_INVENTORY_CONSOLE_GOLD_RETURNED, UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED;

namespace {
void OnUpdateCategoryUI(const UIID & idItemUI, const UIInventory::Console::ItemCategory eCategory);
BOOL RingIncludeControlPredicate(const UIID & idControl);
void UpdatePrice(UIInventory::Console::UIInventoryView * ptView);
void FormatBuyPriceString(const Player * ptPlayer, const ACDID idItem, const BOOL bIsBuybackMode, CRefString & sText);
void FormatSellPriceString(const Player * ptPlayer, const ACDID idItem, CRefString & sText);
void ShowVendorModel(const Player & tPlayer);
void ShowPlayerModel(const Player & tPlayer);
void DoSell(const UIID &);
void sSellStack(const ACDID idACDStack, const uint64 uAmount);
}  // namespace

namespace UIInventory {
namespace Console {
enum ButtonActions {
    ACTION_CHOOSE_SLOT = 1,
    ACTION_OPEN_SLOT = 2,
    ACTION_CLOSE_SLOT = 3,
    ACTION_SCROLL_LIST = 4,
    ACTION_OPEN_CATEGORY = 5,
    ACTION_BACK = 6,
    ACTION_BUY = 7,
    ACTION_SELL = 8,
    ACTION_REPAIR = 9,
    ACTION_REPAIR_EQUIPPED = 10,
    ACTION_REPAIR_ALL = 11,
    ACTION_CLOSE = 12,
    ACTION_SELL_ALL_JUNK = 13,
    ACTION_IDENTIFY = 14,
    ACTION_IDENTIFY_CANCEL = 15,
    ACTION_DETAILS = 16,
    ACTION_LIST = 17,
    ACTION_TOGGLE_STAT_RANGES = 18,
};

class VendorItems : public UIInventory::Console::UIInventoryViewItemsProvider {  // size 0x8e0
public:
    enum ProviderMode {
        PROVIDERMODE_NORMAL = 0,
        PROVIDERMODE_BUYBACK = 1,
    };

    virtual ~VendorItems() {}  // line 130

    VendorItems(const ProviderMode eMode)  // line 138
        : m_eProviderMode(eMode)
    {
    }

    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType eFilterType) const { return m_tView.GetItems(eFilterType); }  // vtable slot 6
    void Refresh();
    void SetProviderMode(ProviderMode);
private:
    virtual void OnPlayerChanged();  // vtable slot 14
    UIInventory::Console::FilteredInventoryView m_tView;  // 0x20
    ProviderMode m_eProviderMode;  // 0x8d8
};
static_assert(sizeof(VendorItems) == 0x8e0, "VendorItems");


inline void VendorItems::Refresh()  // line 150
{
    ACDID idACDFilter = GetPlayer()->idPlayerACD;
    m_tView.ClearItems();
    switch (m_eProviderMode)
    {
    case PROVIDERMODE_NORMAL:
        m_tView.AddItems(GetPlayer()->idACDActiveVendor, idACDFilter, SLOT_MERCHANT_SHOPLIST, 0, 0);
        break;
    case PROVIDERMODE_BUYBACK:
        m_tView.AddItems(GetPlayer()->idPlayerACD, idACDFilter, SLOT_PLAYER_BUYBACK, 0, 0);
        break;
    }
}

void VendorItems::OnPlayerChanged()
{
    Refresh();
}

class PlayerShopScreen : public UIInventory::Console::UIInventoryScreen {  // size 0x2b20
public:
    enum ShopMode {
        SHOP_MODE_INVALID = -1,
        SHOP_MODE_SELLING = 0,
        SHOP_MODE_BUYING = 1,
        SHOP_MODE_REPAIR = 2,
        SHOP_MODE_BUYBACK = 3,
        NUM_SHOP_MODES = 4,
    };

    virtual ~PlayerShopScreen() {}  // line 192

    ShopMode GetShopMode() const  // line 224
    {
        return m_eShopMode;
    }

    PlayerShopScreen();
    void BuyItem();
    void SellItem();
    void RepairItem();
    void RepairAllItems(const BOOL bEquippedOnly);
    void SellAllJunk();
    void SetShopMode(const ShopMode eShopMode);
    virtual void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);  // vtable slot 9
    void UpdateNavBar();
    void OnRepair();
    void UpdateCategoryUI(const UIID & idItemUI, const ItemCategory eCategory);
    void ToggleStatRangeDisplay();
protected:
    virtual BOOL CanOpenForController(int32 nControllerIndex) const;  // vtable slot 15
    virtual void OnOpen();  // vtable slot 10
    virtual void OnClose();  // vtable slot 11
    virtual void UpdateControllerActions(::Console::ControllerActionManager & tActionManager);  // vtable slot 14
    virtual void OnSetActive(BOOL bActive);  // vtable slot 16
    virtual void OnUpdate();  // vtable slot 13
    virtual void OnRequestedUpdate();  // vtable slot 12
    virtual void InitializeRingWrapper(::Console::RingWrapper & tRingWrapper);  // vtable slot 17
    virtual UIFilterType GetDefaultRingSelection() const;  // vtable slot 18
    virtual void DoSetFilterType(const UIFilterType eFilterType);  // vtable slot 25
    virtual void OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType);  // vtable slot 22
    virtual void InitRingFilters(UIIDToFilterMap & tFilterMap);  // vtable slot 21
    virtual UIInventoryView * GetPrimaryViewForMode(const BrowsingMode eBrowsingMode);  // vtable slot 23
    virtual UIInventoryView * GetSecondaryViewForMode(const BrowsingMode eBrowsingMode);  // vtable slot 24
    virtual BOOL ShouldEnableRingIcon(const UIFilterType eFilterType);  // vtable slot 20
    enum ShopState {
        BROWSE_CATEGORY_STATE = 0,
        BROWSE_SUBCATEGORY_STATE = 1,
        BROWSE_ITEMS_STATE = 2,
    };
private:
    ShopMode m_eShopMode;  // 0x200
    UIFilterType m_eLastFilter[4];  // 0x204
    VendorItems m_tShopItemsProvider;  // 0x218
    VendorItems m_tBuybackItemsProvider;  // 0xaf8
    InventoryItems m_tPlayerItemsProvider;  // 0x13d8
    InventoryItems m_tRepairItemsProvider;  // 0x1cc0
    SingleItemProvider m_tSynchedItemProvider;  // 0x25a8
    EquippedItem m_tEquippedItemProvider;  // 0x25e0
    UIInventoryListView m_tShopListView;  // 0x2618
    UIInventoryListView m_tBuybackListView;  // 0x26a0
    UIInventoryListView m_tPlayerListView;  // 0x2728
    UIInventoryListView m_tRepairListView;  // 0x27b0
    UIInventoryDetailsView m_tShopDetailedItemView;  // 0x2838
    UIInventoryDetailsView m_tPlayerDetailedItemView;  // 0x28b0
    UIInventoryDetailsView m_tBuybackDetailedItemView;  // 0x2928
    UIInventoryDetailsView m_tEquippedItemView;  // 0x29a0
    UIInventoryCategoryView m_tCategoriesView;  // 0x2a18
    UIInventoryCategorySummaryView m_tCategorySummaryView;  // 0x2ab0
    ACDID m_idLastSelectedItem;  // 0x2b18
    ShopState m_eShopState;  // 0x2b1c
    void UpdateNormalItemActions(::Console::ControllerActionManager & tActionManager);
    void UpdateBuyingActions(::Console::ControllerActionManager &);
    void UpdateBuyBackActions(::Console::ControllerActionManager & tActionManager);
    void UpdateSellingActions(::Console::ControllerActionManager & tActionManager);
    void UpdateRepairActions(::Console::ControllerActionManager & tActionManager);
    void UpdateBagItemActions(::Console::ControllerActionManager & tActionManager);
    void RefreshProviders(const MaintainSelectionOptions eSelectionType);
    void SwitchToBuybackMode();
    void SwitchToRepairMode();
    void SwitchToOtherShopMode(const ShopMode eOtherShopMode);
    void UpdateRingNewItemIndicators();
    UIInventoryViewItemsProvider * GetProviderForView(const UIInventoryView * ptView, const ShopMode eShopMode);
    BOOL sHasValidItemsForFilterAndMode(const ShopMode eMode, const UIFilterType eFilterType);
    BOOL HasValidItemsForMode(const ShopMode eMode);
    void SetPlayerProviderFlagsForMode();
    BOOL CanOpenSelectedCategory();
    BOOL CanOpenCategory(const ItemCategory eCategory);
    BOOL ShouldShowBagUI();
    void OpenSelectedCategory();
    void CloseCategory();
    void ShowTopLevelCategories();
    void ShowGemCategories();
    void SyncCategories();
    void OnButton1();
    void OnButton2();
};
static_assert(sizeof(PlayerShopScreen) == 0x2b20, "PlayerShopScreen");

PlayerShopScreen * sgptShopGlobals;  // line 411
static const float SCROLLBOX_SCROLLDELTA = 4.0f;

void OnShopButton3();
void OnShopButton4();
void OnShopButtonR3();
void OnShopDPadLeft();

PlayerShopScreen::PlayerShopScreen()
    : m_eShopMode(SHOP_MODE_INVALID)
    , m_tShopItemsProvider(VendorItems::PROVIDERMODE_NORMAL)
    , m_tBuybackItemsProvider(VendorItems::PROVIDERMODE_BUYBACK)
    , m_tPlayerItemsProvider(InventoryItems::PROVIDERMODE_BACKPACK)
    , m_tRepairItemsProvider(InventoryItems::PROVIDERMODE_REPAIR)
    , m_tShopListView(m_tShopItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
    , m_tBuybackListView(m_tBuybackItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
    , m_tPlayerListView(m_tPlayerItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
    , m_tRepairListView(m_tRepairItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
    , m_tShopDetailedItemView(m_tShopItemsProvider, UIDLG_INVENTORY_CONSOLE_DETAILS, FALSE)
    , m_tPlayerDetailedItemView(m_tPlayerItemsProvider, UIDLG_INVENTORY_CONSOLE_DETAILS, FALSE)
    , m_tBuybackDetailedItemView(m_tBuybackItemsProvider, UIDLG_INVENTORY_CONSOLE_DETAILS, FALSE)
    , m_tEquippedItemView(m_tEquippedItemProvider, UIDLG_INVENTORY_CONSOLE_EQUIPPED, FALSE)
    , m_tCategoriesView(UIDLG_INVENTORY_CONSOLE_LIST, OnUpdateCategoryUI)
    , m_tCategorySummaryView(UIDLG_INVENTORY_CONSOLE_EQUIPPED)
    , m_idLastSelectedItem(ACDID_INVALID)
    , m_eShopState(BROWSE_CATEGORY_STATE)
{
    for (int32 i = 0; i < NUM_SHOP_MODES; ++i)
    {
        m_eLastFilter[i] = FILTER_PLAYER_START;
    }

    m_tPlayerListView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tRepairListView.SetHeaderText(StringListFetch("ConsoleInventory:Damaged"));
    m_tShopListView.SetHeaderText(StringListFetch("ConsoleShop:Vendor"));
    m_tBuybackListView.SetHeaderText(StringListFetch("ConsoleShop:Vendor"));
    m_tShopDetailedItemView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    m_tPlayerDetailedItemView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    m_tBuybackDetailedItemView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    m_tEquippedItemView.SetHeaderText(StringListFetch("ConsoleInventory:Equipped"));
    m_tCategorySummaryView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
}

void PlayerShopScreen::UpdateRingNewItemIndicators()
{
    const UIIDToFilterMap & tFilterMap = m_tFilterMap;
    UIFilterType eFilterType;
    UIID idKey;
    POS pos = tFilterMap.GetStartAssoc();
    while (pos)
    {
        tFilterMap.GetNextAssoc(pos, idKey, eFilterType);
        UIID idNewIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.NewStar");
        UIID idNewEliteIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.NewStarElite");
        UIWindow & tWndIndicator = UI_GetWindowFromID(idNewIndicator);
        UIWindow & tWndEliteIndicator = UI_GetWindowFromID(idNewEliteIndicator);
        if (!RingHasFocus() || m_eShopMode == SHOP_MODE_BUYING || m_eShopMode == SHOP_MODE_REPAIR || m_eShopMode == SHOP_MODE_BUYBACK)
        {
            tWndIndicator.SetActive(FALSE);
            tWndEliteIndicator.SetActive(FALSE);
        }
        else
        {
            Player * ptPlayer = m_tPlayerItemsProvider.GetPlayer();
            NewItemIndicatorManager<ACDID> * ptNewItemManager = ptPlayer->ptNewItemManager;
            NewItemIndicatorManager<ACDID> * ptNewEliteItemManager = ptPlayer->ptNewEliteItemManager;
            RemovedItemPredicate tPredicate;
            ptNewItemManager->RemoveItemsIf(tPredicate);
            ptNewEliteItemManager->RemoveItemsIf(tPredicate);
            tWndIndicator.SetActive(FilterHasNewItems(eFilterType, ptPlayer->idPlayerACD, *ptNewItemManager, FALSE));
            tWndEliteIndicator.SetActive(FilterHasNewItems(eFilterType, ptPlayer->idPlayerACD, *ptNewEliteItemManager, FALSE));
        }
    }
}

void PlayerShopScreen::SetShopMode(const ShopMode eShopMode)
{
    if (m_eShopMode == eShopMode)
    {
        return;
    }

    m_fCurrentlyOpening = TRUE;
    m_eShopMode = eShopMode;
    SetBrowsingMode(BROWSE_LIST);
    SetPlayerProviderFlagsForMode();

    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING).SetActive(m_eShopMode != SHOP_MODE_REPAIR);
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING_CURSOR).SetActive(m_eShopMode != SHOP_MODE_REPAIR);

    switch (m_eShopMode)
    {
    case SHOP_MODE_BUYBACK:
        m_tCategoriesView.SetHeaderText(StringListFetch("ConsoleShop:Vendor"));
        SwitchToBuybackMode();
        break;
    case SHOP_MODE_REPAIR:
        SwitchToRepairMode();
        break;
    case SHOP_MODE_BUYING:
        m_tCategoriesView.SetHeaderText(StringListFetch("ConsoleShop:Vendor"));
        SwitchToOtherShopMode(m_eShopMode);
        break;
    case SHOP_MODE_SELLING:
        m_tCategoriesView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
        SwitchToOtherShopMode(m_eShopMode);
        break;
    default:
        SwitchToOtherShopMode(m_eShopMode);
        break;
    }

    UIFilterType eFilterType = GetInitialRingSelection(FILTER_PLAYER_START, FILTER_BAG);
    if (GetFilter() == eFilterType && eFilterType == FILTER_BAG && (m_eShopMode == SHOP_MODE_SELLING || m_eShopMode == SHOP_MODE_BUYING))
    {
        SetFilterType(FILTER_BAG);
        ShowTopLevelCategories();
    }
    else
    {
        SetFilterType(eFilterType);
    }

    UpdateNavBar();
    UpdateRingNewItemIndicators();
    UpdateRingIcons();
    UpdateMainhandAndOffhandRingIcons(m_eShopMode == SHOP_MODE_SELLING ? GetPlayer() : NULL);
    m_fCurrentlyOpening = FALSE;

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

inline void PlayerShopScreen::SwitchToBuybackMode()  // line 582
{
    ShowVendorModel(*GetPlayer());

    UIInventoryView * ptBrowsingView = GetPrimaryViewForMode(GetBrowsingMode());
    SetPrimaryView(ptBrowsingView);
    if (ptBrowsingView != NULL)
    {
        ptBrowsingView->ForceUpdate(RESET_SELECTION);
    }
    CloseSelectedSlot();
}

void PlayerShopScreen::SwitchToRepairMode()
{
    UIInventoryView * ptBrowsingView = GetPrimaryViewForMode(GetBrowsingMode());
    SetPrimaryView(ptBrowsingView);
    ::Console::UIRActorManager::SetActive(FALSE);
    SwitchToInfoPanel(REPAIR_SUMMARY_INFO_PANEL);
    UI_GetTextFromID(UITXT_CONSOLE_PLAYER_LABEL_RING).SetText("", 0);
    if (ptBrowsingView != NULL)
    {
        ptBrowsingView->ForceUpdate(RESET_SELECTION);
    }
    SetFilterType(FILTER_DAMAGED);
    OpenSelectedSlot();
}

void PlayerShopScreen::SwitchToOtherShopMode(const ShopMode eOtherShopMode)
{
    if (eOtherShopMode == SHOP_MODE_BUYING)
    {
        ShowVendorModel(*GetPlayer());
    }
    else if (eOtherShopMode == SHOP_MODE_SELLING)
    {
        ShowPlayerModel(*GetPlayer());
    }

    UIInventoryView * ptBrowsingView = GetPrimaryViewForMode(GetBrowsingMode());
    if (ptBrowsingView != NULL && GetPrimaryView() == ptBrowsingView)
    {
        ptBrowsingView->ForceUpdate(RESET_SELECTION);
    }
    else
    {
        SetPrimaryView(ptBrowsingView);
    }

    if (eOtherShopMode == SHOP_MODE_BUYING)
    {
        if (GetSecondaryView() != &m_tEquippedItemView)
        {
            SetSecondaryView(&m_tEquippedItemView);
        }
    }

    if (ptBrowsingView != NULL)
    {
        if (eOtherShopMode == SHOP_MODE_SELLING)
        {
            ClearStatusLine(ptBrowsingView);
        }
        else
        {
            UpdatePrice(ptBrowsingView);
        }
    }
    CloseSelectedSlot();
}

void PlayerShopScreen::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)
{
    if (GetControllerIndex() != nControllerIndex)
    {
        return;
    }

    switch (eInputButton)
    {
    case BUTTON_1:
    case BUTTON_TOUCH_PAD:
        OnButton1();
        break;
    case BUTTON_2:
        OnButton2();
        break;
    case BUTTON_3:
        OnShopButton3();
        break;
    case BUTTON_4:
        OnShopButton4();
        break;
    case BUTTON_LEFT_TRIGGER:
        sgptShopGlobals->ScrollDetails(-SCROLLBOX_SCROLLDELTA);
        break;
    case BUTTON_RIGHT_TRIGGER:
        sgptShopGlobals->ScrollDetails(SCROLLBOX_SCROLLDELTA);
        break;
    case BUTTON_DPAD_LEFT:
        OnShopDPadLeft();
        break;
    case BUTTON_RIGHT_STICK_BUTTON:
        OnShopButtonR3();
        break;
    default:
        UIInventoryScreen::HandleButtonInput(nControllerIndex, eInputButton);
        break;
    }
}

void PlayerShopScreen::OnButton1()
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_1);
    switch (idAction)
    {
    case ACTION_OPEN_SLOT:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        OpenSelectedSlot();
        break;
    case ACTION_OPEN_CATEGORY:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        OpenSelectedCategory();
        break;
    case ACTION_BUY:
        BuyItem();
        break;
    case ACTION_SELL:
        SellItem();
        break;
    case ACTION_REPAIR:
        RepairItem();
        break;
    }
}

void PlayerShopScreen::OnButton2()
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2);
    switch (idAction)
    {
    case ACTION_IDENTIFY_CANCEL:
        if (GetPrimaryView() != NULL)
        {
            ACDID idACDIdentifyingItem = GetPrimaryView()->GetIdentifyingItemACD();
            if (idACDIdentifyingItem != ACDID_INVALID)
            {
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
                GetPrimaryView()->DoneIdentifying(FALSE);
                ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
            }
        }
        break;
    case ACTION_CLOSE:
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);
        break;
    case ACTION_CLOSE_SLOT:
        if (GetShopMode() == SHOP_MODE_SELLING && GetPrimaryView() != NULL && GetPlayer() != NULL && GetPlayer()->ptNewItemManager != NULL)
        {
            ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
            if (idSelectedItem != ACDID_INVALID)
            {
                GetPlayer()->ptNewItemManager->UnMarkAsNew(idSelectedItem);
            }
        }
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        CloseSelectedSlot();
        break;
    case ACTION_BACK:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        CloseCategory();
        break;
    }
}

void PlayerShopScreen::OpenSelectedCategory()
{
    UIFilterType eFilterType;
    ItemCategory eCategory = m_tCategoriesView.GetSelectedItem();
    if (eCategory == CATEGORY_GEMS)
    {
        ShowGemCategories();
        return;
    }

    eFilterType = GetFilterTypeForCategory(eCategory, FALSE);
    if (eFilterType != FILTER_NONE)
    {
        UIInventoryViewItemsProvider * ptItemsProvider = GetShopMode() == SHOP_MODE_BUYING ? static_cast<UIInventoryViewItemsProvider *>(&m_tShopItemsProvider) : &m_tPlayerItemsProvider;
        ptItemsProvider->SetFilter(eFilterType);
        if (GetShopMode() == SHOP_MODE_BUYING || GetShopMode() == SHOP_MODE_SELLING)
        {
            m_tEquippedItemProvider.SetFilter(eFilterType);
        }
    }

    m_eShopState = BROWSE_ITEMS_STATE;
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    SetFocusedView(GetPrimaryView());
}

void PlayerShopScreen::CloseCategory()
{
    if (m_eShopState == BROWSE_ITEMS_STATE)
    {
        ItemCategory eCategory = m_tCategoriesView.GetSelectedItem();
        if (eCategory >= CATEGORY_RUBY && eCategory <= CATEGORY_UPGRADEABLE_JEWEL)
        {
            ShowGemCategories();
            SetFocusedView(GetPrimaryView());
            return;
        }
    }

    if (m_eShopState == BROWSE_SUBCATEGORY_STATE || m_eShopState == BROWSE_ITEMS_STATE)
    {
        ShowTopLevelCategories();
        SetFocusedView(GetPrimaryView());
    }
}

inline void PlayerShopScreen::ShowTopLevelCategories()  // line 1333
{
    m_eShopState = BROWSE_CATEGORY_STATE;
    PopulateTopLevelCategories(m_tCategoriesView);
    if (GetPrimaryView() != &m_tCategoriesView)
    {
        SetPrimaryView(&m_tCategoriesView);
    }
    else
    {
        m_tCategoriesView.ForceUpdate(RESET_SELECTION);
    }
}

inline void PlayerShopScreen::ShowGemCategories()  // line 1352
{
    m_eShopState = BROWSE_SUBCATEGORY_STATE;
    PopulateGemCategories(m_tCategoriesView);
    if (GetPrimaryView() != &m_tCategoriesView)
    {
        SetPrimaryView(&m_tCategoriesView);
    }
    else
    {
        m_tCategoriesView.ForceUpdate(RESET_SELECTION);
    }
}

inline UIInventoryViewItemsProvider * PlayerShopScreen::GetProviderForView(const UIInventoryView * ptView, const ShopMode eShopMode)  // line 1544
{
    if (ptView == NULL)
    {
        return NULL;
    }

    if (ptView == GetSecondaryView())
    {
        return eShopMode == SHOP_MODE_REPAIR ? static_cast<UIInventoryViewItemsProvider *>(&m_tSynchedItemProvider) : &m_tEquippedItemProvider;
    }

    switch (eShopMode)
    {
    case SHOP_MODE_SELLING:
        return &m_tPlayerItemsProvider;
    case SHOP_MODE_BUYING:
        return &m_tShopItemsProvider;
    case SHOP_MODE_REPAIR:
        return &m_tRepairItemsProvider;
    case SHOP_MODE_BUYBACK:
        return &m_tBuybackItemsProvider;
    }
    return NULL;
}

UIInventoryView * PlayerShopScreen::GetPrimaryViewForMode(const BrowsingMode eBrowsingMode)
{
    if (ShouldShowBagUI())
    {
        return &m_tCategoriesView;
    }

    UIInventoryView * ptView = NULL;
    switch (GetShopMode())
    {
    case SHOP_MODE_BUYING:
        if (eBrowsingMode == BROWSE_LIST)
        {
            ptView = &m_tShopListView;
        }
        else
        {
            ptView = &m_tShopDetailedItemView;
        }
        break;
    case SHOP_MODE_BUYBACK:
        if (eBrowsingMode == BROWSE_LIST)
        {
            ptView = &m_tBuybackListView;
        }
        else
        {
            ptView = &m_tBuybackDetailedItemView;
        }
        break;
    case SHOP_MODE_REPAIR:
        ptView = &m_tRepairListView;
        break;
    case SHOP_MODE_SELLING:
        if (eBrowsingMode == BROWSE_LIST)
        {
            ptView = &m_tPlayerListView;
        }
        else
        {
            ptView = &m_tPlayerDetailedItemView;
        }
        break;
    }
    return ptView;
}

inline BOOL PlayerShopScreen::sHasValidItemsForFilterAndMode(const ShopMode eMode, const UIFilterType eFilterType)  // line 1703
{
    UIInventoryViewItemsProvider * ptProvider = GetProviderForView(GetPrimaryView(), eMode);
    if (ptProvider == NULL)
    {
        return FALSE;
    }
    size_t nNumItems = ptProvider->GetItemsWithFilter(eFilterType).size();
    return nNumItems > 0;
}

void PlayerShopScreen::UpdateNavBar()
{
    LPCSTR arTabs[] = { "SellTab", "BuyTab", "RepairTab", "BuybackTab" };

    BOOL fSomethingToRepair = HasValidItemsForMode(SHOP_MODE_REPAIR);
    ::Console::Screens::EnableScreen(::Console::Screens::SHOP_SCREEN_SET, ::Console::Screens::SHOP_REPAIR_SCREEN, fSomethingToRepair);

    BOOL fHasItemsToBuyback = HasValidItemsForMode(SHOP_MODE_BUYBACK);
    ::Console::Screens::EnableScreen(::Console::Screens::SHOP_SCREEN_SET, ::Console::Screens::SHOP_BUYBACK_SCREEN, fHasItemsToBuyback);

    UIID idNavBar = GetUIIDNone();
    idNavBar.SetFromParent(UIDLG_INVENTORY_CONSOLE, "LayoutRoot.Navigation.Vendor");

    int32 nTabIndex = GetShopMode();
    for (int32 i = 0; i < NUM_SHOP_MODES; ++i)
    {
        UIID idTab;
        idTab.SetFromParent(idNavBar, arTabs[i]);
        UI_GetWindowFromID(idTab).SetActive(i == nTabIndex);
    }

    if (nTabIndex != SHOP_MODE_INVALID)
    {
        UIID idCurrentTabPage;
        idCurrentTabPage.SetFromParent(idNavBar, arTabs[nTabIndex]);

        UIID idText;
        idText.SetFromParent(idCurrentTabPage, "Text3");
        UI_GetButtonFromID(idText).SetEnabled(fSomethingToRepair);

        idText.SetFromParent(idCurrentTabPage, "Text4");
        UI_GetButtonFromID(idText).SetEnabled(fHasItemsToBuyback);
    }
}

inline BOOL PlayerShopScreen::HasValidItemsForMode(const ShopMode eMode)  // line 1728
{
    return sHasValidItemsForFilterAndMode(eMode, FILTER_ALL);
}

BOOL PlayerShopScreen::ShouldEnableRingIcon(const UIFilterType eFilterType)
{
    return sHasValidItemsForFilterAndMode(GetShopMode(), eFilterType);
}

inline void PlayerShopScreen::SetPlayerProviderFlagsForMode()  // line 500
{
    uint32 dwProviderFlags = GetShopMode() == SHOP_MODE_REPAIR ? (1 << InventoryItems::INCLUDE_EQUIPPED_ITEMS) : 0;
    m_tPlayerItemsProvider.SetItemOptions(dwProviderFlags);
}

void PlayerShopScreen::OnOpen()
{
    Player * ptPlayer = GetPlayer();
    m_tShopItemsProvider.SetPlayer(ptPlayer);
    m_tBuybackItemsProvider.SetPlayer(ptPlayer);
    m_tPlayerItemsProvider.SetPlayer(ptPlayer);
    m_tRepairItemsProvider.SetPlayer(ptPlayer);
    m_tEquippedItemProvider.SetPlayer(ptPlayer);
    if (m_eShopMode == SHOP_MODE_INVALID)
    {
        SetShopMode(SHOP_MODE_BUYING);
    }
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    SetFocusedView(NULL);
}

void PlayerShopScreen::OnSetActive(BOOL bActive)
{
    if (bActive)
    {
        ShowNavBar(UIDLG_INVENTORY_CONSOLE, "Vendor");
        UpdateNavBar();
        SetPlayerProviderFlagsForMode();
        UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING_CURSOR).SetActive(TRUE);
        RequestUpdate(RESET_SELECTION);
        ::Console::Screens::ShowModalDialog(UI_GetPrimaryUIController(), UIDLG_INVENTORY_CONSOLE, FALSE, GetUIIDNone());
    }
    else
    {
        ::Console::Screens::CloseModalDialog(UIDLG_INVENTORY_CONSOLE);
    }
}

void PlayerShopScreen::OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType)
{
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    if (eNewFilterType == FILTER_BAG && (GetShopMode() == SHOP_MODE_SELLING || GetShopMode() == SHOP_MODE_BUYING))
    {
        ShowTopLevelCategories();
    }
    m_tShopItemsProvider.SetFilter(eNewFilterType);
    m_tBuybackItemsProvider.SetFilter(eNewFilterType);
    m_tPlayerItemsProvider.SetFilter(eNewFilterType);
    m_tRepairItemsProvider.SetFilter(eNewFilterType);
    m_tEquippedItemProvider.SetFilter(eNewFilterType);
    RefreshProviders(RESET_SELECTION);
}

void PlayerShopScreen::RefreshProviders(const MaintainSelectionOptions eSelectionType)
{
    m_tShopItemsProvider.Refresh();
    m_tBuybackItemsProvider.Refresh();
    m_tPlayerItemsProvider.Refresh();
    m_tRepairItemsProvider.Refresh();
    UIInventoryView * ptPrimaryView = GetPrimaryView();
    if (ptPrimaryView != NULL)
    {
        ptPrimaryView->ForceUpdate(eSelectionType);
    }
    UIInventoryView * ptSecondaryView = GetSecondaryView();
    if (ptSecondaryView != NULL)
    {
        ptSecondaryView->ForceUpdate(eSelectionType);
    }
}

void PlayerShopScreen::InitRingFilters(UIIDToFilterMap &tFilterMap)
{
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_ARM_LEFT, FILTER_MAIN_HAND);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_ARM_RIGHT, FILTER_OFF_HAND);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_BAG, FILTER_BAG);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_BRACERS, FILTER_WRIST);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_FEET, FILTER_FEET);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_FINGER_LEFT, FILTER_RING_LEFT);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_FINGER_RIGHT, FILTER_RING_RIGHT);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_HANDS, FILTER_HANDS);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_HEAD, FILTER_HEAD);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_LEGS, FILTER_LEGS);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_NECK, FILTER_NECK);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_SHOULDER, FILTER_SHOULDERS);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_TORSO, FILTER_CHEST);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_WAIST, FILTER_BELT);
}

inline void PlayerShopScreen::ToggleStatRangeDisplay()  // line 1199
{
    m_tShopDetailedItemView.ToggleStatRangeDisplay();
    m_tPlayerDetailedItemView.ToggleStatRangeDisplay();
    m_tEquippedItemView.ToggleStatRangeDisplay();
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_TOGGLE_STATS), cgtPSODefaults);
}

inline void PlayerShopScreen::RepairItem()  // line 1822
{
    Player * ptPlayer = GetPlayer();
    UIInventoryView * ptView = GetPrimaryView();
    if (ptPlayer != NULL && ptView != NULL)
    {
        UIRepair::Console::RepairItem(ptPlayer, ptView->GetSelectedItem());
    }
}

inline void PlayerShopScreen::RepairAllItems(const BOOL bEquippedOnly)  // line 1836
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer != NULL)
    {
        UIRepair::Console::RepairAllItems(ptPlayer, bEquippedOnly);
    }
}

inline void PlayerShopScreen::SellAllJunk()  // line 1849
{
    const ACDIDArray & aJunk = m_tPlayerItemsProvider.GetItemsWithFilter(FILTER_JUNK);
    for (int32 i = aJunk.size(); i > 0; --i)
    {
        ACDID id = aJunk[i - 1];
        if (id)
        {
            ANNDataMessage tMessage;
            tMessage.ann = ANNFromACD(id);
            MessageSendToServer(CMSG_SHOP_TRY_SELL, &tMessage, sizeof(tMessage), GetPlayer());
        }
    }
}

BOOL PlayerShopScreen::CanOpenForController(int32 nControllerIndex) const
{
    return UIInventoryScreen::CanOpenForController(nControllerIndex);
}

void PlayerShopScreen::OnClose()
{
    if (UIStackSplit::Console::IsVisible())
    {
        UIStackSplit::Console::Close();
    }
    InputClearRemappings();
    ResetCurrencyAnimations(UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED);
    SetPrimaryView(NULL);
    SetSecondaryView(NULL);
    m_eShopMode = SHOP_MODE_INVALID;
    Player * ptPlayer = GetPlayer();
    ptPlayer->idACDActiveVendor = ACDID_INVALID;
    ptPlayer->eActiveCrafterType = CRAFTER_INVALID;
    SimpleMessage tMessage;
    MessageSendToServer(CMSG_CLOSE_INTERACTION, &tMessage, sizeof(tMessage), ptPlayer);
}

inline void PlayerShopScreen::UpdateCategoryUI(const UIID & idItemUI, const ItemCategory eCategory)  // line 1188
{
    BOOL bIsEnabled = CanOpenCategory(eCategory);
    UpdateCommonCategoryUI(idItemUI, eCategory, bIsEnabled, FALSE, FALSE);
}

inline BOOL PlayerShopScreen::CanOpenCategory(const ItemCategory eCategory)  // line 1222
{
    UIInventoryViewItemsProvider * ptItemsProvider = NULL;
    switch (m_eShopMode)
    {
    case SHOP_MODE_BUYING:
        ptItemsProvider = &m_tShopItemsProvider;
        break;
    case SHOP_MODE_SELLING:
        ptItemsProvider = &m_tPlayerItemsProvider;
        break;
    }
    if (ptItemsProvider == NULL)
    {
        return FALSE;
    }
    return CategoryHasItems(eCategory, *ptItemsProvider, FALSE);
}

void PlayerShopScreen::InitializeRingWrapper(::Console::RingWrapper & tRingWrapper)
{
    tRingWrapper.WrapUsingContainerControl(UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR, RingIncludeControlPredicate, NULL);
}

UIFilterType PlayerShopScreen::GetDefaultRingSelection() const
{
    return m_eLastFilter[m_eShopMode];
}

void PlayerShopScreen::DoSetFilterType(const UIFilterType eFilterType)
{
    UIInventoryScreen::DoSetFilterType(eFilterType);
    m_eLastFilter[m_eShopMode] = eFilterType;
}

inline BOOL PlayerShopScreen::ShouldShowBagUI()  // line 1246
{
    return GetFilter() == FILTER_BAG &&
        (GetShopMode() == SHOP_MODE_SELLING || GetShopMode() == SHOP_MODE_BUYING) &&
        (m_eShopState == BROWSE_CATEGORY_STATE || m_eShopState == BROWSE_SUBCATEGORY_STATE);
}

UIInventoryView * PlayerShopScreen::GetSecondaryViewForMode(const BrowsingMode eBrowsingMode)
{
    UIInventoryView * ptView;
    if (ShouldShowBagUI())
    {
        ptView = &m_tCategorySummaryView;
    }
    else
    {
        ptView = &m_tEquippedItemView;
    }
    return ptView;
}

inline void PlayerShopScreen::SyncCategories()  // line 1371
{
    ItemCategory eCategory = m_tCategoriesView.GetSelectedItem();
    if (eCategory != CATEGORY_NONE)
    {
        m_tCategorySummaryView.SetCategory(eCategory);
    }
}

void PlayerShopScreen::OnUpdate()
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer != NULL)
    {
        BOOL fRingHasFocus = RingHasFocus();

        UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_PANEL_FOCUS_HIGHLIGHT).SetActive(!fRingHasFocus);
        UpdateNavBar();

        ShowFilterName(UITXT_INVENTORY_CONSOLE_HEADER, GetFilter(), !fRingHasFocus);

        UpdateCurrencyDisplay(UITXT_INVENTORY_CONSOLE_GOLD, UITXT_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UITXT_INVENTORY_CONSOLE_GOLD_RETURNED, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED, CURRENCY_GOLD);

        SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));

        if (GetShopMode() == SHOP_MODE_REPAIR)
        {
            UIRepair::Console::UpdateRepairUI(ptPlayer);
        }

        UIInventoryView * ptBrowsingView = GetPrimaryView();
        if (ptBrowsingView != NULL)
        {
            if (ptBrowsingView->GetSelectedItem() != ACDID_INVALID)
            {
                if (UIID(ptBrowsingView->GetStatusLineUIID()) != GetUIIDNone())
                {
                    UpdatePrice(ptBrowsingView);
                }
            }
            else
            {
                ClearStatusLine(ptBrowsingView);
            }
        }

        if (ShouldShowBagUI())
        {
            SyncCategories();
            m_idLastSelectedItem = ACDID_INVALID;
        }

        UIInventoryView * ptDetailedView = GetSecondaryView();
        if (ptDetailedView != NULL && ptDetailedView->GetSelectedItem() != ACDID_INVALID)
        {
            if (GetShopMode() == SHOP_MODE_BUYING)
            {
                ptDetailedView->ShowBagSpace(m_tPlayerItemsProvider, StringListFetch("ConsoleInventory:Inventory"));
            }
            else
            {
                UpdatePrice(ptDetailedView);
            }
        }
    }
    UpdateRingNewItemIndicators();
}

void PlayerShopScreen::OnRequestedUpdate()
{
    m_tShopItemsProvider.Refresh();
    m_tBuybackItemsProvider.Refresh();
    m_tPlayerItemsProvider.Refresh();
    m_tRepairItemsProvider.Refresh();
    UpdateRingNewItemIndicators();
    UpdateRingIcons();
    if (GetPlayer() != NULL && GetShopMode() == SHOP_MODE_SELLING)
    {
        VisualInventory tVisualInventory;
        tVisualInventory.Init();
        UpdatePlayerInfoUI(*GetPlayer(), UIWND_INVENTORY_CONSOLE_ROOT, tVisualInventory);
    }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerShopScreen::UpdateControllerActions(::Console::ControllerActionManager & tActionManager)
{
    tActionManager.ClearAllActions();
    InputClearRemappings();

    if (GetShopMode() == SHOP_MODE_SELLING && m_tPlayerItemsProvider.GetItemsWithFilter(FILTER_JUNK).size() > 0)
    {
        InputRemapSingleJoyconButton(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ::Console::CONTROLLER_INPUT_DPAD_RIGHT);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON, ACTION_SELL_ALL_JUNK, StringListFetch("ConsoleShop:SellAllJunk").str());
    }

    if (RingHasFocus())
    {
        if (ShouldEnableRingIcon(GetFilter()))
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_SLOT, StringListFetch("ConsoleUI:Open").str());
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
    }
    else if (GetPrimaryView() != NULL && GetPrimaryView()->GetIdentifyingItemACD() != ACDID_INVALID)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_IDENTIFY_CANCEL, StringListFetch("H2OLayout:Cancel").str());
    }
    else
    {
        if (CanScrollDetails())
        {
            CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
        }

        if (GetFilter() == FILTER_BAG && GetShopMode() != SHOP_MODE_BUYBACK)
        {
            UpdateBagItemActions(tActionManager);
        }
        else
        {
            UpdateNormalItemActions(tActionManager);
        }
    }
}

void PlayerShopScreen::UpdateNormalItemActions(::Console::ControllerActionManager & tActionManager)
{
    switch (m_eShopMode)
    {
    case SHOP_MODE_BUYING:
        UpdateBuyBackActions(tActionManager);
        break;
    case SHOP_MODE_BUYBACK:
        UpdateBuyBackActions(tActionManager);
        break;
    case SHOP_MODE_SELLING:
        UpdateSellingActions(tActionManager);
        break;
    case SHOP_MODE_REPAIR:
        UpdateRepairActions(tActionManager);
        break;
    default:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
        break;
    }

    ACDID idEquippedItem = GetEquippedItem(GetPlayer(), GetFilter(), FALSE);
    if (idEquippedItem != ACDID_INVALID || GetBrowsingMode() == BROWSE_DETAILS)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_DPAD_LEFT, ACTION_TOGGLE_STAT_RANGES, StringListFetch("ConsoleUI:ToggleStatRanges").str());
    }
}

void PlayerShopScreen::UpdateBuyBackActions(::Console::ControllerActionManager & tActionManager)
{
    if (GetPrimaryView()->GetSelectedItem() != ACDID_INVALID)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_BUY, StringListFetch("ConsoleShop:Buy").str());
        if (GetBrowsingMode() == BROWSE_LIST)
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Compare").str());
        }
        else
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List").str());
        }
    }
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
}

void PlayerShopScreen::UpdateSellingActions(::Console::ControllerActionManager & tActionManager)
{
    ACDID idACDSelectedItem = GetPrimaryView()->GetSelectedItem();
    if (idACDSelectedItem != ACDID_INVALID)
    {
        if (GetSecondaryView()->GetIdentifyingItemACD() == ACDID_INVALID)
        {
            ActorCommonData * ptACDItem = ACDTryToGet(idACDSelectedItem);
            if (ItemIsUnidentified(*ptACDItem))
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_IDENTIFY, StringListFetch("ConsoleInventory:Identify").str());
            }
            else if (GetBrowsingMode() == BROWSE_LIST)
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Compare").str());
            }
            else
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List").str());
            }
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELL, StringListFetch("ConsoleShop:Sell").str());
    }
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
}

void PlayerShopScreen::UpdateRepairActions(::Console::ControllerActionManager & tActionManager)
{
    if (GetPrimaryView()->GetSelectedItem() != ACDID_INVALID)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_REPAIR, StringListFetch("ConsoleShop:Repair").str());
    }
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_REPAIR_EQUIPPED, StringListFetch("ConsoleShop:RepairEquipped").str());
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_REPAIR_ALL, StringListFetch("ConsoleShop:RepairAll").str());
}

void PlayerShopScreen::UpdateBagItemActions(::Console::ControllerActionManager & tActionManager)
{
    switch (m_eShopState)
    {
    case BROWSE_CATEGORY_STATE:
        if (CanOpenSelectedCategory())
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_CATEGORY, StringListFetch("ConsoleUI:Open").str());
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
        break;
    case BROWSE_SUBCATEGORY_STATE:
        if (CanOpenSelectedCategory())
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_CATEGORY, StringListFetch("ConsoleUI:Open").str());
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        break;
    case BROWSE_ITEMS_STATE:
        UpdateNormalItemActions(tActionManager);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        break;
    }
}

inline BOOL PlayerShopScreen::CanOpenSelectedCategory()  // line 1212
{
    ItemCategory eCategory = m_tCategoriesView.GetSelectedItem();
    return CanOpenCategory(eCategory);
}

void PlayerShopScreen::BuyItem()
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer == NULL)
    {
        return;
    }

    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    ActorCommonData * ptACDItem = ACDTryToGet(GetPrimaryView()->GetSelectedItem());
    if (ptACDItem != NULL)
    {
        BuyResponse eBuyResponse = ACDInventoryCanBuy(tACDPlayer, *ptACDItem);
        switch (eBuyResponse)
        {
        case BUY_RESPONSE_CANT_AFFORD:
            {
                GameErrorData tError;
                tError.eError = GAMEERROR_INSUFFICIENT_FUNDS;
                tError.dwVal = 0;
                UIGame::DisplayError(tError, FALSE, ptPlayer);
            }
            return;
        case BUY_RESPONSE_CANT_PICKUP:
        case BUY_RESPONSE_ITEM_NOT_AT_VENDOR:
            {
                GameErrorData tError;
                tError.eError = GAMEERROR_PICKUP_FAILED;
                tError.dwVal = 0;
                UIGame::DisplayError(tError, FALSE, ptPlayer);
            }
            return;
        }

        ANNDataMessage tMessage;
        tMessage.ann = ANNFromACD(ptACDItem->id);
        MessageSendToServer(CMSG_SHOP_TRY_BUY, &tMessage, sizeof(tMessage), PlayerGetByACD(ptPlayer->idPlayerACD));
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_UI_BUY_ITEM), cgtPSODefaults);
    }
}

void PlayerShopScreen::SellItem()
{
    Player * ptPlayer = GetPlayer();
    UIInventoryView * ptView = GetPrimaryView();
    if (ptPlayer == NULL || ptView == NULL)
    {
        return;
    }

    ActorCommonData * ptACDItem = ACDTryToGet(ptView->GetSelectedItem());
    if (ptACDItem != NULL)
    {
        GetPlayer()->ptNewItemManager->UnMarkAsNew(ptACDItem->id);

        ActorCommonData & tACDVendor = ACDGet(ptPlayer->idACDActiveVendor);
        ItemSellError eSellError = ItemCanBeSold(*ptACDItem, &tACDVendor);
        if (eSellError != ISE_NONE)
        {
            GameErrorData tError;
            tError.eError = GAMEERROR_ITEM_CANNOT_BE_SOLD;
            tError.dwVal = 0;
            UIGame::DisplayError(tError, FALSE, NULL);
            return;
        }

        CRefString sPrompt;
        if (ItemIsIrreplaceable(ptACDItem->hGB.gbid))
        {
            sPrompt = StringListFetch("Shop:Warning");
        }
        else if (ItemIsEquipped(*ptACDItem))
        {
            sPrompt = StringListFetch("ConsoleShop:SellEquippedWarning");
        }

        if (sPrompt.Length() == 0)
        {
            DoSell(GetUIIDNone());
        }
        else
        {
            UIConfirmation::Show(sPrompt.str(), DoSell, NULL, FALSE, TRUE, ptPlayer->nControllerIndex, -1);
        }
    }
}

inline void PlayerShopScreen::OnRepair()  // line 1169
{
    UIInventoryView * ptBrowsingView = GetPrimaryView();
    if (ptBrowsingView != NULL && m_eShopMode == SHOP_MODE_REPAIR)
    {
        m_tRepairItemsProvider.Refresh();
        RequestUpdate(RESET_SELECTION);
        UpdateRingIcons();
    }
}

void OnShopButton3()
{
    if (sgptShopGlobals == NULL)
    {
        return;
    }
    if (!sgptShopGlobals->IsActive())
    {
        return;
    }

    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3);
    switch (idAction)
    {
    case ACTION_REPAIR_EQUIPPED:
        sgptShopGlobals->RepairAllItems(TRUE);
        return;
    case ACTION_IDENTIFY:
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        ACDID idACDSelectedItem = sgptShopGlobals->GetPrimaryView()->GetSelectedItem();
        sgptShopGlobals->GetPrimaryView()->SetIdentifyingItemACD(idACDSelectedItem);
        break;
    }
    case ACTION_DETAILS:
    {
        ACDID idSelectedItem = sgptShopGlobals->GetPrimaryView()->GetSelectedItem();
        sgptShopGlobals->SetBrowsingMode(UIInventoryScreen::BROWSE_DETAILS);
        sgptShopGlobals->GetPrimaryView()->SelectItem(idSelectedItem);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        break;
    }
    case ACTION_LIST:
    {
        ACDID idSelectedItem = sgptShopGlobals->GetPrimaryView()->GetSelectedItem();
        sgptShopGlobals->SetBrowsingMode(UIInventoryScreen::BROWSE_LIST);
        sgptShopGlobals->GetPrimaryView()->SelectItem(idSelectedItem);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        break;
    }
    default:
        return;
    }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

inline void OnShopButton4()  // line 2008
{
    if (sgptShopGlobals != NULL && sgptShopGlobals->IsActive())
    {
        if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_4) == ACTION_REPAIR_ALL)
        {
            sgptShopGlobals->RepairAllItems(FALSE);
        }
    }
}

void OnShopButtonR3()
{
    if (sgptShopGlobals != NULL && sgptShopGlobals->IsActive())
    {
        ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON);
        if (idAction == ACTION_SELL_ALL_JUNK)
        {
            sgptShopGlobals->SellAllJunk();
        }
    }
}

void OnShopDPadLeft()
{
    if (sgptShopGlobals != NULL && sgptShopGlobals->IsActive())
    {
        if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_DPAD_LEFT) == ACTION_TOGGLE_STAT_RANGES)
        {
            sgptShopGlobals->ToggleStatRangeDisplay();
        }
    }
}

void OnShopUpdate()
{
    if (sgptShopGlobals != NULL && sgptShopGlobals->IsActive())
    {
        sgptShopGlobals->Update();
    }
}

void OnShopChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bCosmeticChanged, BOOL bForce)
{
    if (sgptShopGlobals == NULL || !sgptShopGlobals->IsActive())
    {
        return;
    }
    if (!bForce && tOldLoc.eActorLocation == tNewLoc.eActorLocation && tNewLoc.tInvLocation.eSlot == tOldLoc.tInvLocation.eSlot)
    {
        return;
    }
    Player * ptPlayer = sgptShopGlobals->GetPlayer();
    if (ptPlayer == NULL || tOldLoc.tInvLocation.idACDContainer == ptPlayer->idPlayerACD || tNewLoc.tInvLocation.idACDContainer == ptPlayer->idPlayerACD)
    {
        if (bCosmeticChanged ||
            (tOldLoc.eActorLocation == AL_INVENTORY && VisualInventorySlotFromInventorySlot(tOldLoc.tInvLocation.eSlot) != VIS_INVALID) ||
            (tNewLoc.eActorLocation == AL_INVENTORY && VisualInventorySlotFromInventorySlot(tNewLoc.tInvLocation.eSlot) != VIS_INVALID))
        {
            ::Console::UIRActorManager::ForceRActorRecreate();
        }
        sgptShopGlobals->RequestUpdate(KEEP_ITEM_SELECTED);
    }
}

void OnShopLeftStick(const UIID &, const int32 nControllerIndex)
{
    if (sgptShopGlobals != NULL && sgptShopGlobals->IsActive())
    {
        sgptShopGlobals->HandleLeftStickInput(nControllerIndex);
    }
}

void OnShopRightStick(const UIID &, const int32 nControllerIndex)
{
    if (sgptShopGlobals != NULL && sgptShopGlobals->IsActive())
    {
        sgptShopGlobals->HandleRightStickInput(nControllerIndex);
    }
}

void OnShopButton(const UIID &, const int32 nControllerIndex, const InputButton eInputButton)
{
    if (sgptShopGlobals != NULL && sgptShopGlobals->IsActive())
    {
        sgptShopGlobals->HandleButtonInput(nControllerIndex, eInputButton);
    }
}

}  // namespace Console
}  // namespace UIInventory

namespace UIShop {
namespace Console {
using namespace UIInventory::Console;

void InitializeForGame()
{
    if (!GameIsStartUp())
    {
        sgptShopGlobals = new PlayerShopScreen();
    }
}

void DisposeForGame()
{
    if (!GameIsStartUp())
    {
        ::delete sgptShopGlobals;
        sgptShopGlobals = NULL;
    }
}

void OpenForController(int32 nControllerIndex)
{
    if (PlayerGetByControllerIndex(nControllerIndex) != NULL)
    {
        sgptShopGlobals->UpdateNavBar();
    }
    sgptShopGlobals->OpenForController(nControllerIndex);
}

void Close()
{
    if (sgptShopGlobals->IsActive())
    {
        sgptShopGlobals->Close();
    }
}

void OpenBuyScreen(int32)
{
    if (sgptShopGlobals != NULL && sgptShopGlobals->IsActive())
    {
        sgptShopGlobals->SetShopMode(PlayerShopScreen::SHOP_MODE_BUYING);
    }
}

void OpenSellScreen(int32)
{
    if (sgptShopGlobals != NULL && sgptShopGlobals->IsActive())
    {
        sgptShopGlobals->SetShopMode(PlayerShopScreen::SHOP_MODE_SELLING);
    }
}

void OpenRepairScreen(int32)
{
    if (sgptShopGlobals != NULL && sgptShopGlobals->IsActive())
    {
        sgptShopGlobals->SetShopMode(PlayerShopScreen::SHOP_MODE_REPAIR);
    }
}

void OpenBuybackScreen(int32)
{
    if (sgptShopGlobals != NULL && sgptShopGlobals->IsActive())
    {
        sgptShopGlobals->SetShopMode(PlayerShopScreen::SHOP_MODE_BUYBACK);
    }
}

void OnRepair()
{
    if (sgptShopGlobals != NULL && sgptShopGlobals->IsActive())
    {
        sgptShopGlobals->OnRepair();
    }
}

void Refresh(const Player * ptPlayer)
{
    if (sgptShopGlobals != NULL && sgptShopGlobals->IsActive() && sgptShopGlobals->GetPlayer() == ptPlayer &&
        sgptShopGlobals->GetShopMode() == PlayerShopScreen::SHOP_MODE_SELLING)
    {
        ShowPlayerModel(*ptPlayer);
    }
}

}  // namespace Console
}  // namespace UIShop

namespace {

void OnUpdateCategoryUI(const UIID & idItemUI, const UIInventory::Console::ItemCategory eCategory)
{
    using namespace UIInventory::Console;
    if (sgptShopGlobals != NULL && sgptShopGlobals->IsActive())
    {
        sgptShopGlobals->UpdateCategoryUI(idItemUI, eCategory);
    }
}

BOOL RingIncludeControlPredicate(const UIID & idControl)
{
    return idControl != UIWND_INVENTORY_CONSOLE_RING_GLOW;
}

void sSellStack(const ACDID idACDStack, const uint64 uAmount)  // line 2222
{
    using namespace UIInventory::Console;
    ActorCommonData * ptACDStack = ACDTryToGet(idACDStack);
    if (ptACDStack == NULL)
    {
        return;
    }

    Player * ptPlayer = sgptShopGlobals->GetPlayer();
    if (ptPlayer == NULL)
    {
        return;
    }

    StackPortionMessage tMessage;
    tMessage.annItem = ANNFromACD(ptACDStack->id);
    tMessage.uAmount = uAmount;
    MessageSendToServer(CMSG_SHOP_TRY_SELL_STACK_PORTION, &tMessage, sizeof(tMessage), ptPlayer);
}

void DoSell(const UIID &)  // line 2246
{
    using namespace UIInventory::Console;
    if (sgptShopGlobals != NULL)
    {
        Player * ptPlayer = sgptShopGlobals->GetPlayer();
        UIInventoryView * ptView = sgptShopGlobals->GetPrimaryView();
        if (ptPlayer != NULL && ptView != NULL && ptView->GetSelectedItem() != ACDID_INVALID)
        {
            ActorCommonData * ptACDItem = ACDTryToGet(ptView->GetSelectedItem());
            if (ptACDItem != NULL)
            {
                uint64 uStackCount = ptACDItem->GetStackCount();
                if (uStackCount > 1)
                {
                    UIStackSplit::Console::Open(ptACDItem->id, uStackCount, uStackCount, sSellStack, UIStackSplit::Console::STACKSPLIT_GENERIC);
                    return;
                }
                ANNDataMessage tMessage;
                tMessage.ann = ANNFromACD(ptACDItem->id);
                MessageSendToServer(CMSG_SHOP_TRY_SELL, &tMessage, sizeof(tMessage), PlayerGetByACD(ptPlayer->idPlayerACD));
            }
        }
    }
}

void UpdatePrice(UIInventory::Console::UIInventoryView * ptView)  // line 2278
{
    using namespace UIInventory::Console;
    if (sgptShopGlobals == NULL)
    {
        return;
    }
    if (!sgptShopGlobals->IsActive())
    {
        return;
    }

    Player * ptPlayer = sgptShopGlobals->GetPlayer();
    if (ptPlayer == NULL)
    {
        return;
    }

    const UIID & idStatusLine = ptView->GetStatusLineUIID();
    if (idStatusLine == GetUIIDNone())
    {
        return;
    }

    ACDID idItem = ptView->GetSelectedItem();
    CRefString sPriceText;
    PlayerShopScreen::ShopMode eShopMode = sgptShopGlobals->GetShopMode();
    switch (eShopMode)
    {
    case PlayerShopScreen::SHOP_MODE_BUYING:
        FormatBuyPriceString(ptPlayer, idItem, FALSE, sPriceText);
        break;
    case PlayerShopScreen::SHOP_MODE_BUYBACK:
        FormatBuyPriceString(ptPlayer, idItem, TRUE, sPriceText);
        break;
    case PlayerShopScreen::SHOP_MODE_SELLING:
        FormatSellPriceString(ptPlayer, idItem, sPriceText);
        break;
    }
    UI_GetTextFromID(idStatusLine).SetText(sPriceText.str(), 0);
}

void FormatBuyPriceString(const Player * ptPlayer, const ACDID idItem, const BOOL bIsBuybackMode, CRefString & sText)  // line 2328
{
    ActorCommonData * ptACDItem = ACDTryToGet(idItem);
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    ActorCommonData * ptACDVendor = ACDTryToGet(bIsBuybackMode ? ptPlayer->idPlayerACD : ptPlayer->idACDActiveVendor);
    if (ptACDItem != NULL && ptACDPlayer != NULL && ptACDVendor != NULL)
    {
        BuyResponse eBuyResponse = ACDInventoryCanBuy(*ptACDPlayer, *ptACDItem);
        BOOL bCanBuy = eBuyResponse == BUY_RESPONSE_OKAY;
        CurrencyType eCurrencyType = CURRENCY_GOLD;
        uint64 uGold = ItemGetCost(*ptACDItem, bIsBuybackMode ? 2 : 1, eCurrencyType, ptACDVendor, bIsBuybackMode, FALSE);
        CRefString sPrice = UIText_ValueToGameCurrency(uGold, eCurrencyType);
        Set_Text_Color(sPrice, bCanBuy ? FONTCOLOR_WHITE : FONTCOLOR_RED);
        sText = LocalizedSprintf(StringListFetch("ConsoleShop:CostFormat").str(), sPrice.str(), NULL);
    }
}

inline void FormatSellPriceString(const Player * ptPlayer, const ACDID idItem, CRefString & sText)  // line 2361
{
    ActorCommonData * ptACDItem = ACDTryToGet(idItem);
    ActorCommonData * ptACDVendor = ACDTryToGet(ptPlayer->idACDActiveVendor);
    if (ptACDItem != NULL && ptACDVendor != NULL)
    {
        CurrencyType eCurrencyType = CURRENCY_GOLD;
        uint64 uGold = ItemGetCost(*ptACDItem, 0, eCurrencyType, ptACDVendor, TRUE, FALSE);
        CRefString sPrice = UIText_ValueToGameCurrency(uGold, eCurrencyType);
        Set_Text_Color(sPrice, FONTCOLOR_WHITE);
        sText = LocalizedSprintf(StringListFetch("ConsoleShop:SellValueFormat").str(),
                                 sPrice.str(), NULL);
    }
}

void ShowVendorModel(const Player & tPlayer)  // line 2402
{
    using namespace UIInventory::Console;
    ActorCommonData & tACDVendor = ACDGet(tPlayer.idACDActiveVendor);

    ::Console::UIRActorManager::SetActive(TRUE);
    BOOL bShouldCreateRActor = ::Console::UIRActorManager::ShouldCreateRActor(::Console::UIRActorManager::RACTOR_CONTROL_RING, ::Console::UIRActorManager::RACTOR_CHARACTER_NPC, FALSE, TRUE);
    if (bShouldCreateRActor)
    {
        UIRActor & tModel = UI_GetRActorFromID(UIRACTOR_CONSOLE_RING);
        RActorID idRActor = (RActorID)tACDVendor.idOwner;
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor != NULL)
        {
            tModel.CreateRActor(RENDERCONTEXT_HUD_0, ptRActor->snoActor);

            LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tModel.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
            tModel.SetLight(idLight);

            SNO snoAnimSet = ActorGetAnimSetSNO(ptRActor->snoActor);
            ptRActor = RActorTryToGet(tModel.GetRActor());
            if (snoAnimSet == GlobalSNOGet(GLOBALSNO_ANIMSET_OMNINPC_MALE))
            {
                RActorPlayAnimation(ptRActor, TAG_ACTOR_ANIMATION_GIZMO_STATE_10, TRUE, FALSE);
                tModel.SetObserver(GlobalSNOGet(GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_CRAFTER));
            }
            else
            {
                tModel.SetObserver(GlobalSNOGet(GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_FOLLOWER));
            }
        }
    }

    SwitchToInfoPanel(VENDOR_INFO_PANEL);

    CRefString sVendorName;
    MonsterGetBaseName(tACDVendor.tSNOName, sVendorName);
    UI_GetTextFromID(UIWND_INVENTORY_CONSOLE_VENDOR_NAME).SetText(sVendorName.str(), 0);
    UI_GetTextFromID(UITXT_CONSOLE_PLAYER_LABEL_RING).SetText("", 0);

    UIID idGold;
    idGold.SetFromParent(UIWND_INVENTORY_CONSOLE_ROOT, "GoldBalance");
    UIInventory::UpdateGoldAmount(idGold, tPlayer.idPlayerACD);
}

inline void ShowPlayerModel(const Player & tPlayer)  // line 2463
{
    using namespace UIInventory::Console;
    SwitchToInfoPanel(CHARACTER_INFO_PANEL);
    ::Console::UIRActorManager::SetActive(TRUE);
    VisualInventory tVisualInventory;
    tVisualInventory.Init();
    UpdatePlayerInfoUI(tPlayer, UIWND_INVENTORY_CONSOLE_ROOT, tVisualInventory);
}

}  // namespace
