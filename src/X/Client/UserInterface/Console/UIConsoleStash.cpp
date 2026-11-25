// Original file: X/Client/UserInterface/Console/UIConsoleStash.cpp
// The console shared-stash screen: player inventory on one side, stash on the other.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Console/ControllerUtils.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "ConsoleInputUtilities.h"
#include "Wrappers/RingWrapper.h"
#include "UIConsoleInventoryCategorySummaryView.h"
#include "UIConsoleInventoryCategoryView.h"
#include "UIConsoleInventoryDetailsView.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryListView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "UIConsoleItemCategories.h"
#include "UIConsoleNewItemIndicators.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../Interface/UIConfirmation.h"
#include "../UIID.h"
#include "../../CPlayer.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Items.h"
#include "../../../Common/ItemsSwig.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Sigma/RefString.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Common/Player.h









BOOL PlayerCanManipulateInventory(const ActorCommonData & tACDPlayer);
// X/Client/UserInterface/Interface/UIGame.cpp
namespace UIGame { void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer); }
// X/Common/Networking/GameMessage.h



// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
// X/Common/ActorInventory.cpp
BOOL ACDInventoryCanHold(const ActorCommonData & tACD, const ActorCommonData & tACDItem, const InventorySlotType eSlot, IVector2D & tWhereCanHold, const int32 * pnSlotYMinMax);
int32 ACDInventoryGetMaxStashSlots(ActorCommonData & tACDPlayer);
int32 ACDInventorySlotGetUpgradeIncrement(const InventorySlotType eSlot);
BOOL ACDInventoryFindNonFullStackInSlot(const ActorCommonData & tACDItem, const ActorCommonData & tACDContainer, const InventorySlotType eCheckSlot, InventoryLocation & tNewLocation, const int32 * pnSlotYMinMax);
void ACDInventoryLocationSet(InventoryLocation & tInvLoc, const ACDID idACDContainer, const InventorySlotType eSlot, const int32 nSlotX, const int32 nSlotY);
int32 ACDInventoryGetStashUpgradeCost(ActorCommonData & tACDPlayer);
// X/Client/CActorInventory.cpp
void CACDInventoryRequestSplitStack(const ActorCommonData * ptACDStack, const InventoryLocation & tInvLocDest, const uint64 uAmount);
void CACDInventoryRequestStackTransfer(const ACDID idACDItemSrc, const ACDID idACDItemDest, const uint64 uAmount);
// X/Client/Input/InputNX64.cpp
void InputClearRemappings();
void InputRemapSingleJoyconButton(InputButton eFrom, InputButton eTo);
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Sigma/Unicode.cpp
CRefString LocalizedSprintf(LPCSTR szFormat, ...);
// X/Client/UserInterface/UIText.cpp
CRefString UIText_ValueToGameCurrency(const uint64 uAmount, CurrencyType eCurrencyType);
// X/Client/UserInterface/UI.cpp
int32 UI_GetPrimaryUIController();
// X/Client/UserInterface/Console/UIConsoleStackSplit.cpp
namespace UIStackSplit {
namespace Console {
enum StackSplitType {
    STACKSPLIT_GENERIC = 0,
};
typedef void (*UIStackSplitCallback)(const ACDID, const uint64);
void Open(const ACDID idACDStack, const uint64 uInitialVal, const uint64 uStackMax, const UIStackSplitCallback pfnCallback, const StackSplitType eStackSplitType);
}  // namespace Console
}  // namespace UIStackSplit
// X/Client/UserInterface/Interface/UIInventoryCommon.cpp
namespace UIInventory {
void UpdateGoldAmount(const UIID & idText, const ACDID idPlayerACD);
void PlayItemInventorySound(const ActorCommonData & tACDItem);
void TryQuickMoveToLocation(ActorCommonData & tACDOwner, ActorCommonData & tACDItem, const InventorySlotType eSlot);
}  // namespace UIInventory
namespace UIInventory {
namespace Console {
// X/Client/UserInterface/Console/UIConsoleInventory.cpp
void ShowFilterName(const UIID & idText, const UIFilterType eFilter, const BOOL bHighlight);
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
enum InfoPanelType {
    STASH_UPGRADE_INFO_PANEL = 1,
};
void SwitchToInfoPanel(const InfoPanelType eInfoPanel);
}  // namespace Console
}  // namespace UIInventory
// X/Client/UserInterface/UIGlobals.cpp
extern const UIID UIDLG_INVENTORY_CONSOLE, UIDLG_INVENTORY_CONSOLE_LIST, UIDLG_INVENTORY_CONSOLE_DETAILS,
    UIDLG_INVENTORY_CONSOLE_EQUIPPED, UIDLG_STASH_CONSOLE_LIST, UIDLG_STASH_CONSOLE_DETAILS;
extern const UIID UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR, UIWND_INVENTORY_CONSOLE_RING_GLOW,
    UIWND_INVENTORY_CONSOLE_ROOT, UIWND_INVENTORY_CONSOLE_STASH_UPGRADE, UIWND_CONSOLE_STASH_MENU_GLOW;
extern UIID UITXT_INVENTORY_CONSOLE_HEADER, UITXT_INVENTORY_CONSOLE_GOLD;  // DWARF: UIGlobals.cpp:3144 (HEADER), :3139 (GOLD); no header declarations recorded.
extern const UIID UIBTN_INVENTORY_CONSOLE_RING_ARM_LEFT, UIBTN_INVENTORY_CONSOLE_RING_ARM_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_BAG,
    UIBTN_INVENTORY_CONSOLE_RING_BRACERS, UIBTN_INVENTORY_CONSOLE_RING_FEET, UIBTN_INVENTORY_CONSOLE_RING_FINGER_LEFT,
    UIBTN_INVENTORY_CONSOLE_RING_FINGER_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_HANDS, UIBTN_INVENTORY_CONSOLE_RING_HEAD,
    UIBTN_INVENTORY_CONSOLE_RING_LEGS, UIBTN_INVENTORY_CONSOLE_RING_NECK, UIBTN_INVENTORY_CONSOLE_RING_SHOULDER,
    UIBTN_INVENTORY_CONSOLE_RING_TORSO, UIBTN_INVENTORY_CONSOLE_RING_WAIST;
// X/Client/UserInterface/Console/ConsoleScreenManager.h
namespace Console { namespace Screens { void CloseScreenSet(BOOL fClearScreenSetHistory, BOOL fSkipTransition); } }

namespace {
BOOL RingIncludeControlPredicate(const UIID & idControl);
void OnUpdateCategoryUI(const UIID & idItemUI, const UIInventory::Console::ItemCategory eCategory);
void ShowDetailsFocusHighlight(const UIInventory::Console::UIInventoryView * ptView, const BOOL bShow);
}  // namespace

namespace UIInventory {
namespace Console {
enum ButtonActions {
    ACTION_CHOOSE_SLOT = 1,
    ACTION_OPEN_SLOT = 2,
    ACTION_CLOSE_SLOT = 3,
    ACTION_OPEN_CATEGORY = 4,
    ACTION_BACK = 5,
    ACTION_SCROLL_LIST = 6,
    ACTION_MOVE_TO_STASH = 7,
    ACTION_MOVE_TO_INVENTORY = 8,
    ACTION_CLOSE = 9,
    ACTION_DETAILS = 10,
    ACTION_LIST = 11,
    ACTION_UPGRADE_STASH = 12,
    ACTION_IDENTIFY = 13,
    ACTION_IDENTIFY_CANCEL = 14,
    ACTION_MOVE_ALL_GEMS_TO_STASH = 15,
};

// Folded into the original HandleButtonInput as +/-4.0.
static const float SCROLLBOX_SCROLLDELTA = 4.0f;

void UpdateStashInfo(const Player & tPlayer);
void UpdateStashUpgradeInfo(const Player & tPlayer);
inline BOOL StashCanBeUpgraded(const Player & tPlayer);
inline BOOL CanAffordUpgrade(const Player & tPlayer);
void OnStashButton3();
void OnStashButtonR3();

class PlayerStashScreen : public UIInventory::Console::UIInventoryScreen {  // size 0x16e0 (UIStash InitializeForGame: new(0x16e0))
public:
    uint32 m_nStashSlots;  // 0x200

    virtual ~PlayerStashScreen() {}

    PlayerStashScreen()
        : m_tStashItemsProvider(InventoryItems::PROVIDERMODE_STASH)
        , m_tPlayerItemsProvider(InventoryItems::PROVIDERMODE_BACKPACK)
        , m_tStashListView(m_tStashItemsProvider, UIDLG_STASH_CONSOLE_LIST, TRUE)
        , m_tPlayerListView(m_tPlayerItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
        , m_tStashDetailsView(m_tStashItemsProvider, UIDLG_STASH_CONSOLE_DETAILS, 1)
        , m_tPlayerDetailsView(m_tPlayerItemsProvider, UIDLG_INVENTORY_CONSOLE_DETAILS, 1)
        , m_tCategoriesView(UIDLG_INVENTORY_CONSOLE_LIST, OnUpdateCategoryUI)
        , m_tCategorySummaryView(UIDLG_INVENTORY_CONSOLE_EQUIPPED)
        , m_eStashState(BROWSE_CATEGORY_STATE)
        , m_eCurrentCategory(CATEGORY_NONE)
    {
        m_tPlayerListView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
        m_tPlayerDetailsView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
        m_tStashListView.SetHeaderText(StringListFetch("ConsoleInventory:Stash"));
        m_tStashDetailsView.SetHeaderText(StringListFetch("ConsoleInventory:Stash"));
        m_tCategoriesView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
        m_tCategorySummaryView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
        m_nStashSlots = 0;
        InitAnalogStickFilters();
    }

    InventoryItems & GetPlayerItemsProvider()
    {
        return m_tPlayerItemsProvider;
    }

    InventoryItems & GetStashItemsProvider()
    {
        return m_tStashItemsProvider;
    }

    void RefreshPlayerItems();
    void RefreshStashItems();
    void BuySlots();

    virtual void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)
    {
        if (GetControllerIndex() != nControllerIndex)
            return;

        switch (eInputButton)
        {
        case BUTTON_1:
            OnButton1();
            break;
        case BUTTON_2:
            OnButton2();
            break;
        case BUTTON_3:
            OnStashButton3();
            break;
        case BUTTON_4:
            OnButton4();
            break;
        case BUTTON_RIGHT_STICK_BUTTON:
            OnStashButtonR3();
            break;
        case BUTTON_LEFT_TRIGGER:
            OnLeftTrigger();
            break;
        case BUTTON_RIGHT_TRIGGER:
            OnRightTrigger();
            break;
        case BUTTON_DPAD_RIGHT:
            OnDPadRight();
            break;
        case BUTTON_DPAD_LEFT:
            OnDPadLeft();
            break;
        default:
            UIInventoryScreen::HandleButtonInput(nControllerIndex, eInputButton);
            break;
        }
    }

    void SwitchFocusIfCurrentFocusIsInvalid();
    void UpdateCategoryUI(const UIID & idItemUI, const ItemCategory eCategory);

protected:
    virtual BOOL CanOpenForController(int32 nControllerIndex) const
    {
        return UIInventoryScreen::CanOpenForController(nControllerIndex);
    }

    virtual void OnOpen();
    virtual void OnClose();
    virtual void UpdateControllerActions(::Console::ControllerActionManager & tActionManager);

    virtual void OnSetActive(BOOL bActive)
    {
        if (bActive)
        {
            ShowNavBar(UIDLG_INVENTORY_CONSOLE, "Stash");
            UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING).SetActive(TRUE);
            UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING_CURSOR).SetActive(TRUE);
            EnableRing(TRUE);
            SwitchToInfoPanel(STASH_UPGRADE_INFO_PANEL);
            RequestUpdate(RESET_SELECTION);
            ::Console::Screens::ShowModalDialog(UI_GetPrimaryUIController(), UIDLG_INVENTORY_CONSOLE, FALSE, GetUIIDNone());
        }
        else
        {
            ::Console::Screens::CloseModalDialog(UIDLG_INVENTORY_CONSOLE);
            UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_STASH_UPGRADE).SetActive(FALSE);
        }

        UIID idStats;
        idStats.SetFromParent(UIWND_INVENTORY_CONSOLE_ROOT, "CharacterStats");
        UI_GetWindowFromID(idStats).SetActive(!bActive);
    }

    virtual void OnUpdate();
    virtual void OnRequestedUpdate();

    virtual void InitializeRingWrapper(::Console::RingWrapper & tRingWrapper)
    {
        tRingWrapper.WrapUsingContainerControl(UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR, RingIncludeControlPredicate, FALSE);
    }

    virtual UIFilterType GetDefaultRingSelection() const
    {
        return FILTER_MAIN_HAND;
    }

    virtual void OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType)
    {
        SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
        SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));

        if (GetFilter() == FILTER_BAG)
            ShowTopLevelCategories();

        m_tPlayerItemsProvider.SetFilter(eNewFilterType);
        m_tStashItemsProvider.SetFilter(eNewFilterType);
        RefreshProviders(RESET_SELECTION);
    }

    virtual void InitRingFilters(UIIDToFilterMap & tFilterMap)
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

    virtual UIInventoryView * GetPrimaryViewForMode(const BrowsingMode eBrowsingMode)
    {
        if (GetFilter() == FILTER_BAG && (m_eStashState == BROWSE_CATEGORY_STATE || m_eStashState == BROWSE_SUBCATEGORY_STATE))
        {
            return &m_tCategoriesView;
        }

        if (eBrowsingMode == BROWSE_LIST)
        {
            return &m_tPlayerListView;
        }
        else
        {
            return &m_tPlayerDetailsView;
        }
    }

    virtual UIInventoryView * GetSecondaryViewForMode(const BrowsingMode eBrowsingMode)
    {
        if (GetFilter() == FILTER_BAG && m_eStashState != BROWSE_GEMS_STATE && m_eStashState != BROWSE_ITEMS_STATE)
        {
            return &m_tCategorySummaryView;
        }

        if (eBrowsingMode == BROWSE_LIST)
        {
            return &m_tStashListView;
        }
        else
        {
            return &m_tStashDetailsView;
        }
    }

    virtual BOOL ShouldEnableRingIcon(const UIFilterType eFilterType)
    {
        if (GetStashItemsProvider().GetItemsWithFilter(eFilterType).size() != 0)
        {
            return TRUE;
        }

        return GetPlayerItemsProvider().GetItemsWithFilter(eFilterType).size() != 0;
    }

    enum StashState {
        BROWSE_CATEGORY_STATE = 0,
        BROWSE_SUBCATEGORY_STATE = 1,
        BROWSE_GEMS_STATE = 2,
        BROWSE_ITEMS_STATE = 3,
    };

private:
    InventoryItems m_tStashItemsProvider;  // 0x208
    InventoryItems m_tPlayerItemsProvider;  // 0xaf0
    UIInventoryListView m_tStashListView;  // 0x13d8
    UIInventoryListView m_tPlayerListView;  // 0x1460
    UIInventoryDetailsView m_tStashDetailsView;  // 0x14e8
    UIInventoryDetailsView m_tPlayerDetailsView;  // 0x1560
    UIInventoryCategoryView m_tCategoriesView;  // 0x15d8
    UIInventoryCategorySummaryView m_tCategorySummaryView;  // 0x1670
    StashState m_eStashState;  // 0x16d8
    ItemCategory m_eCurrentCategory;  // 0x16dc

    void UpdateRingNewItemIndicators();
    void UpdateBagItemActions(::Console::ControllerActionManager & tActionManager);
    void UpdateNormalItemActions(::Console::ControllerActionManager & tActionManager);
    BOOL CanOpenSelectedCategory();
    BOOL CanOpenCategory(const ItemCategory eCategory);
    void OpenSelectedCategory();
    void CloseCategory();
    void ShowTopLevelCategories();
    void ShowGemCategories();
    void SyncCategories();
    void OnButton1();
    void OnButton2();
    void OnButton4();
    void OnLeftTrigger();
    void OnRightTrigger();
    void OnDPadLeft();
    void OnDPadRight();

    void RefreshProviders(const MaintainSelectionOptions eSelectionType)
    {
        m_tStashItemsProvider.Refresh();
        m_tPlayerItemsProvider.Refresh();

        UIInventoryView * ptPrimaryView = GetPrimaryView();
        if (ptPrimaryView)
        {
            ptPrimaryView->ForceUpdate(eSelectionType);
        }

        UIInventoryView * ptSecondaryView = GetSecondaryView();
        if (ptSecondaryView)
        {
            ptSecondaryView->ForceUpdate(eSelectionType);
        }
    }

    void InitAnalogStickFilters()
    {
        ::Console::AnalogStickInputFilter tStickInputFilter = ::Console::GetDefaultStickInputFilter(0);
        m_tPlayerListView.SetAnalogStickInputFilter(tStickInputFilter);
        m_tPlayerDetailsView.SetAnalogStickInputFilter(tStickInputFilter);
        m_tStashListView.SetAnalogStickInputFilter(tStickInputFilter);
        m_tStashDetailsView.SetAnalogStickInputFilter(tStickInputFilter);
    }
};
static_assert(sizeof(PlayerStashScreen) == 0x16e0, "PlayerStashScreen");

PlayerStashScreen * sgptStashGlobals;

void PlayerStashScreen::OnOpen()
{
    Player * ptPlayer = GetPlayer();
    m_tStashItemsProvider.SetPlayer(ptPlayer);
    m_tPlayerItemsProvider.SetPlayer(ptPlayer);

    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    SetFocusedView(NULL);
    m_eStashState = BROWSE_CATEGORY_STATE;

    UpdateRingIcons();
    UpdateMainhandAndOffhandRingIcons(ptPlayer);
    UpdateStashInfo(*ptPlayer);
    SetFilterType(GetInitialRingSelection(FILTER_PLAYER_START, FILTER_PLAYER_END));
}

void PlayerStashScreen::OnClose()
{
    InputClearRemappings();
    ShowDetailsFocusHighlight(&m_tPlayerDetailsView, FALSE);
    ShowDetailsFocusHighlight(&m_tStashDetailsView, FALSE);
    SetPrimaryView(NULL);
    SetSecondaryView(NULL);
    CloseSelectedSlot();
    m_tStashItemsProvider.SetPlayer(NULL);
    m_tPlayerItemsProvider.SetPlayer(NULL);

    UIID idPlayerNumber = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".PlayerLabel");
    UI_GetTextFromID(idPlayerNumber).SetText("", FALSE);
}

void PlayerStashScreen::UpdateRingNewItemIndicators()
{
    Player * ptPlayer = m_tPlayerItemsProvider.GetPlayer();
    NewItemIndicatorManager<ACDID> * ptNewItemManager = ptPlayer->ptNewItemManager;
    NewItemIndicatorManager<ACDID> * ptNewEliteItemManager = ptPlayer->ptNewEliteItemManager;
    RemovedItemPredicate tPredicate;

    ptNewItemManager->RemoveItemsIf(tPredicate);
    ptNewEliteItemManager->RemoveItemsIf(tPredicate);

    UIFilterType eFilterType;
    UIID idKey;

    const UIIDToFilterMap & tFilterMap = m_tFilterMap;
    POS pos = tFilterMap.GetStartAssoc();
    while (pos)
    {
        tFilterMap.GetNextAssoc(pos, idKey, eFilterType);
        UIID idNewIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.NewStar");
        UIID idNewEliteIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.NewStarElite");
        BOOL bRingEnabled = sgptStashGlobals->GetRingUI().IsEnabled();
        BOOL bNewElite = FilterHasNewItems(eFilterType, ptPlayer->idPlayerACD, *ptNewEliteItemManager, TRUE);
        BOOL bNewItem = !bNewElite && FilterHasNewItems(eFilterType, ptPlayer->idPlayerACD, *ptNewItemManager, TRUE);
        UI_GetWindowFromID(idNewIndicator).SetActive(bRingEnabled && bNewItem);
        UI_GetWindowFromID(idNewEliteIndicator).SetActive(bRingEnabled && bNewElite);
    }
}

void PlayerStashScreen::UpdateControllerActions(::Console::ControllerActionManager & tActionManager)
{
    tActionManager.ClearAllActions();
    InputClearRemappings();

    if (RingHasFocus())
    {
        if (ShouldEnableRingIcon(GetFilter()))
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_SLOT, StringListFetch("ConsoleUI:Open").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());

        if (GetPlayer() && StashCanBeUpgraded(*GetPlayer()) && CanAffordUpgrade(*GetPlayer()))
        {
            InputRemapSingleJoyconButton(BUTTON_DPAD_UP, BUTTON_RIGHT_STICK_BUTTON);
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON, ACTION_UPGRADE_STASH, StringListFetch("ConsoleInventory:UpgradeStash").str());
        }
    }
    else
    {
        UIInventoryView * ptFocusedView = GetFocusedView();
        if (ptFocusedView && ptFocusedView->GetIdentifyingItemACD() != ACDID_INVALID)
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_IDENTIFY_CANCEL, StringListFetch("H2OLayout:Cancel").str());
        }
        else if (GetFilter() == FILTER_BAG)
        {
            UpdateBagItemActions(tActionManager);
        }
        else
        {
            UpdateNormalItemActions(tActionManager);
        }
    }
}

void PlayerStashScreen::UpdateBagItemActions(::Console::ControllerActionManager & tActionManager)
{
    switch (m_eStashState)
    {
    case BROWSE_CATEGORY_STATE:
        if (CanOpenSelectedCategory())
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_CATEGORY, StringListFetch("ConsoleUI:Open").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
        break;
    case BROWSE_SUBCATEGORY_STATE:
        if (CanOpenSelectedCategory())
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_CATEGORY, StringListFetch("ConsoleUI:Open").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        if (m_eCurrentCategory == CATEGORY_GEMS && m_tPlayerItemsProvider.GetItemsWithFilter(GetFilterTypeForCategory(CATEGORY_GEMS, FALSE)).size() != 0)
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_MOVE_ALL_GEMS_TO_STASH, StringListFetch("ConsoleInventory:MoveAllGemsToStash").str());
        break;
    case BROWSE_GEMS_STATE:
    case BROWSE_ITEMS_STATE:
        UpdateNormalItemActions(tActionManager);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        break;
    }
}

void PlayerStashScreen::UpdateNormalItemActions(::Console::ControllerActionManager & tActionManager)
{
    if (CanScrollDetails())
    {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
    }

    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());

    BOOL bIsIdentifyingAnItem = (GetPrimaryView() && GetPrimaryView()->IsIdentifyingItem()) ||
        (GetSecondaryView() && GetSecondaryView()->IsIdentifyingItem());
    if (!bIsIdentifyingAnItem && GetFocusedView())
    {
        ACDID idSelectedItem = GetFocusedView()->GetSelectedItem();
        if (idSelectedItem != ACDID_INVALID)
        {
            if (GetFocusedView() == GetSecondaryView())
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_MOVE_TO_INVENTORY, StringListFetch("ConsoleInventory:MoveToInventory").str());
            else
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_MOVE_TO_STASH, StringListFetch("ConsoleInventory:MoveToStash").str());

            ActorCommonData & tItemACD = ACDGet(idSelectedItem);
            if (ItemIsUnidentified(tItemACD))
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_IDENTIFY, StringListFetch("ConsoleInventory:Identify").str());
            else if (sgptStashGlobals->GetBrowsingMode() == BROWSE_LIST)
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Compare").str());
            else
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List").str());
        }
    }
}

inline BOOL PlayerStashScreen::CanOpenSelectedCategory()
{
    ItemCategory eCategory = m_tCategoriesView.GetSelectedItem();
    return CanOpenCategory(eCategory);
}

inline BOOL PlayerStashScreen::CanOpenCategory(const ItemCategory eCategory)
{
    return CategoryHasItems(eCategory, m_tPlayerItemsProvider, FALSE) || CategoryHasItems(eCategory, m_tStashItemsProvider, FALSE);
}

void PlayerStashScreen::SwitchFocusIfCurrentFocusIsInvalid()
{
    UIInventoryView * ptCurrentView = GetFocusedView();
    if (ptCurrentView && !ptCurrentView->CanAcceptFocus())
    {
        UIInventoryView * ptPrimaryView = GetPrimaryView();
        UIInventoryView * ptSecondaryView = GetSecondaryView();
        if (ptCurrentView == ptPrimaryView)
        {
            if (CanSwitchFocusToView(ptSecondaryView))
                SetFocusedView(ptSecondaryView);
        }
        else if (ptCurrentView == ptSecondaryView)
        {
            if (CanSwitchFocusToView(ptPrimaryView))
                SetFocusedView(ptPrimaryView);
        }
    }
}

void PlayerStashScreen::OnUpdate()
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer)
    {
        BOOL fRingHasFocus = RingHasFocus();
        ShowFilterName(UITXT_INVENTORY_CONSOLE_HEADER, GetFilter(), !fRingHasFocus);
        UI_GetWindowFromID(UIWND_CONSOLE_STASH_MENU_GLOW).SetActive(!fRingHasFocus);
        UpdateStashUpgradeInfo(*ptPlayer);
        UpdateGoldAmount(UITXT_INVENTORY_CONSOLE_GOLD, ptPlayer->idPlayerACD);
    }

    UIInventoryView * ptSecondaryView = GetSecondaryView();
    if (ptSecondaryView)
        ptSecondaryView->ShowBagSpace(m_tStashItemsProvider, StringListFetch("ConsoleInventory:Stash"));

    UIInventoryView * ptPrimaryView = GetPrimaryView();
    if (ptPrimaryView)
        ptPrimaryView->ShowBagSpace(m_tPlayerItemsProvider, StringListFetch("ConsoleInventory:Inventory"));

    if (GetFilter() == FILTER_BAG && (m_eStashState == BROWSE_CATEGORY_STATE || m_eStashState == BROWSE_SUBCATEGORY_STATE))
        SyncCategories();

    if (GetBrowsingMode() == BROWSE_DETAILS && m_eStashState != BROWSE_CATEGORY_STATE && m_eStashState != BROWSE_SUBCATEGORY_STATE)
    {
        ShowDetailsFocusHighlight(GetPrimaryView(), GetFocusedView() == GetPrimaryView());
        ShowDetailsFocusHighlight(GetSecondaryView(), GetFocusedView() == GetSecondaryView());
    }

    UpdateRingNewItemIndicators();
}

void PlayerStashScreen::OnRequestedUpdate()
{
    m_tPlayerItemsProvider.Refresh();
    m_tStashItemsProvider.Refresh();
    UpdateRingIcons();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerStashScreen::OpenSelectedCategory()
{
    UIFilterType eFilterType;
    ItemCategory eCategory = m_tCategoriesView.GetSelectedItem();

    if (eCategory == CATEGORY_GEMS)
    {
        ShowGemCategories();
    }
    else
    {
        eFilterType = GetFilterTypeForCategory(eCategory, FALSE);
        m_eCurrentCategory = eCategory;
        if (eFilterType != FILTER_NONE)
        {
            m_tPlayerItemsProvider.SetFilter(eFilterType);
            m_tStashItemsProvider.SetFilter(eFilterType);
        }

        m_eStashState = IsGemSubCategory(eCategory) ? BROWSE_GEMS_STATE : BROWSE_ITEMS_STATE;
        SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
        SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
        GetPrimaryView()->Enable(TRUE);
        GetSecondaryView()->Enable(TRUE);
        SetFocusedView(GetPrimaryView());
    }
}

void PlayerStashScreen::CloseCategory()
{
    if (m_eStashState == BROWSE_SUBCATEGORY_STATE || m_eStashState == BROWSE_ITEMS_STATE)
    {
        ItemCategory eCurrentCategory = m_eCurrentCategory;
        ShowTopLevelCategories();
        SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
        SetFocusedView(GetPrimaryView());
        m_tCategoriesView.SelectItem((ACDID)eCurrentCategory);
    }
}

inline void PlayerStashScreen::ShowTopLevelCategories()
{
    m_eStashState = BROWSE_CATEGORY_STATE;
    m_eCurrentCategory = CATEGORY_NONE;
    PopulateTopLevelCategories(m_tCategoriesView);
    if (GetPrimaryView() != &m_tCategoriesView)
        SetPrimaryView(&m_tCategoriesView);
    else
        m_tCategoriesView.ForceUpdate(RESET_SELECTION);
}

inline void PlayerStashScreen::ShowGemCategories()
{
    m_eStashState = BROWSE_SUBCATEGORY_STATE;
    m_eCurrentCategory = CATEGORY_GEMS;
    PopulateGemCategories(m_tCategoriesView);
    if (GetPrimaryView() != &m_tCategoriesView)
        SetPrimaryView(&m_tCategoriesView);
    else
        m_tCategoriesView.ForceUpdate(RESET_SELECTION);
}

void PlayerStashScreen::UpdateCategoryUI(const UIID & idItemUI, const ItemCategory eCategory)
{
    BOOL bIsEnabled = CanOpenCategory(eCategory);
    BOOL bIsNew = CategoryHasNewItems(eCategory, GetPlayer());
    BOOL bIsNewElite = CategoryHasNewEliteItems(eCategory, GetPlayer());
    UpdateCommonCategoryUI(idItemUI, eCategory, bIsEnabled, bIsNew, bIsNewElite);
}

void PlayerStashScreen::BuySlots()
{
    Player * ptPlayer = GetPlayer();
    if (!ptPlayer)
        return;

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return;

    int32 nUpgradeCost = ACDInventoryGetStashUpgradeCost(*ptACDPlayer);
    if (ptACDPlayer->CanAffordCurrencyCost(nUpgradeCost, CURRENCY_GOLD))
    {
        SimpleMessage tMessage;
        MessageSendToServer(CMSG_SHARED_STASH_TRY_BUY, &tMessage, sizeof(tMessage), ptPlayer);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_STASH_BUY_SLOTS), cgtPSODefaults);
    }
}

inline void PlayerStashScreen::SyncCategories()
{
    ItemCategory eCategory = m_tCategoriesView.GetSelectedItem();
    if (eCategory != CATEGORY_NONE)
        m_tCategorySummaryView.SetCategory(eCategory);
}

static InventorySlotType sgeCachedStackDest;

void sMoveStack(const ACDID idACDStack, const uint64 uAmount)
{
    ActorCommonData * ptACDStack = ACDTryToGet(idACDStack);
    if (!ptACDStack)
        return;

    Player * ptPlayer = sgptStashGlobals->GetPlayer();
    if (!ptPlayer)
        return;

    ActorCommonData & tACDPlayer = ACDGet(sgptStashGlobals->GetPlayer()->idPlayerACD);
    InventorySlotType eSlot = sgeCachedStackDest;
    uint64 uStackSize = ptACDStack->GetStackCount();
    if (uStackSize == uAmount)
    {
        TryQuickMoveToLocation(tACDPlayer, *ptACDStack, eSlot);
        return;
    }

    InventoryLocation tInvLoc;
    if (ACDInventoryFindNonFullStackInSlot(*ptACDStack, tACDPlayer, eSlot, tInvLoc, NULL))
    {
        ACDID idACDItemInSlot = ACDInventoryGetItemInSlot(tInvLoc);
        ActorCommonData * ptACDItemInSlot = ACDTryToGet(idACDItemInSlot);
        if (ptACDItemInSlot)
        {
            CACDInventoryRequestStackTransfer(ptACDStack->id, idACDItemInSlot, uAmount);
            PlayItemInventorySound(*ptACDStack);
            return;
        }
    }

    IVector2D tPoint(0, 0);
    if (!ACDInventoryCanHold(tACDPlayer, *ptACDStack, eSlot, tPoint, NULL))
    {
        UIGame::DisplayError(GameErrorData(GAMEERROR_IARESULT_NOT_ENOUGH_ROOM), FALSE, NULL);
        return;
    }

    ACDInventoryLocationSet(tInvLoc, tACDPlayer.id, eSlot, tPoint.x, tPoint.y);
    CACDInventoryRequestSplitStack(ptACDStack, tInvLoc, uAmount);
    PlayItemInventorySound(*ptACDStack);
}

void PlayerStashScreen::OnButton1()
{
    InventorySlotType eSlot;
    ACDID idSelectedItem;
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_1);

    switch (idAction)
    {
    case ACTION_OPEN_SLOT:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        OpenSelectedSlot();
        return;
    case ACTION_OPEN_CATEGORY:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        OpenSelectedCategory();
        return;
    case ACTION_MOVE_TO_STASH:
    {
        UIInventoryView * ptPlayerView = GetPrimaryView();
        if (!ptPlayerView)
            return;
        eSlot = SLOT_PLAYER_SHARED_STASH;
        idSelectedItem = ptPlayerView->GetSelectedItem();
        break;
    }
    case ACTION_MOVE_TO_INVENTORY:
    {
        UIInventoryView * ptStashView = GetSecondaryView();
        if (!ptStashView)
            return;
        eSlot = SLOT_PLAYER_BACKPACK;
        idSelectedItem = ptStashView->GetSelectedItem();
        break;
    }
    default:
        return;
    }

    if (idSelectedItem != ACDID_INVALID)
    {
        ActorCommonData * ptACDSelectedItem = ACDTryToGet(idSelectedItem);
        if (ptACDSelectedItem)
        {
            ActorCommonData & tACDPlayer = ACDGet(sgptStashGlobals->GetPlayer()->idPlayerACD);
            if (!PlayerCanManipulateInventory(tACDPlayer))
            {
                UIGame::DisplayError(GameErrorData(GAMEERROR_ITEMS_CANNOT_BE_MOVED_AT_THIS_TIME), FALSE, NULL);
                return;
            }

            uint64 uStackSize = ptACDSelectedItem->GetStackCount();
            if (uStackSize > 1)
            {
                sgeCachedStackDest = eSlot;
                UIStackSplit::Console::Open(ptACDSelectedItem->id, uStackSize, uStackSize, sMoveStack, UIStackSplit::Console::STACKSPLIT_GENERIC);
            }
            else
            {
                TryQuickMoveToLocation(tACDPlayer, *ptACDSelectedItem, eSlot);
                if (GetPlayer())
                    GetPlayer()->ptNewItemManager->UnMarkAsNew(idSelectedItem);
            }
        }
    }
}

void PlayerStashScreen::OnButton2()
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2);

    switch (idAction)
    {
    case ACTION_CLOSE_SLOT:
        if (GetFocusedView() && GetPlayer() && GetPlayer()->ptNewItemManager)
        {
            ACDID idSelectedItem = GetFocusedView()->GetSelectedItem();
            if (idSelectedItem != ACDID_INVALID)
                GetPlayer()->ptNewItemManager->UnMarkAsNew(idSelectedItem);
        }
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        CloseSelectedSlot();
        break;
    case ACTION_BACK:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        if (m_eStashState == BROWSE_SUBCATEGORY_STATE || m_eStashState == BROWSE_ITEMS_STATE)
        {
            CloseCategory();
        }
        else
        {
            m_eStashState = BROWSE_SUBCATEGORY_STATE;
            m_eCurrentCategory = CATEGORY_GEMS;
            SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
            SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
        }
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        break;
    case ACTION_CLOSE:
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);
        break;
    case ACTION_IDENTIFY_CANCEL:
    {
        UIInventoryView * ptFocusedView = GetFocusedView();
        if (ptFocusedView && ptFocusedView->IsIdentifyingItem())
        {
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
            ptFocusedView->DoneIdentifying(FALSE);
            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        }
        break;
    }
    }
}

void PlayerStashScreen::OnButton4()
{
    Player * ptPlayer = GetPlayer();
    if (!ptPlayer)
        return;

    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_4);
    if (idAction == ACTION_MOVE_ALL_GEMS_TO_STASH)
    {
        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        if (!PlayerCanManipulateInventory(tACDPlayer))
        {
            UIGame::DisplayError(GameErrorData(GAMEERROR_ITEMS_CANNOT_BE_MOVED_AT_THIS_TIME), FALSE, NULL);
            return;
        }

        const ACDIDArray & arItems = m_tPlayerItemsProvider.GetItemsWithFilter(FILTER_GEMS);
        for (ACDIDArray::const_iterator iter = arItems.begin(); iter != arItems.end(); ++iter)
        {
            ActorCommonData * ptACDItem = ACDTryToGet(*iter);
            if (ptACDItem && !ItemIsUpgradeableJewel(ptACDItem->hGB.gbid))
            {
                TryQuickMoveToLocation(tACDPlayer, *ptACDItem, SLOT_PLAYER_SHARED_STASH);
                ptPlayer->ptNewItemManager->UnMarkAsNew(*iter);
            }
        }
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

inline void PlayerStashScreen::OnLeftTrigger()
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_TRIGGER) == ACTION_SCROLL_LIST &&
        !::Console::ControllerUtils::IsButtonDown(GetControllerIndex(), BUTTON_RIGHT_TRIGGER))
    {
        ScrollDetails(-SCROLLBOX_SCROLLDELTA);
    }
}

inline void PlayerStashScreen::OnRightTrigger()
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER) == ACTION_SCROLL_LIST &&
        !::Console::ControllerUtils::IsButtonDown(GetControllerIndex(), BUTTON_LEFT_TRIGGER))
    {
        ScrollDetails(SCROLLBOX_SCROLLDELTA);
    }
}

inline void PlayerStashScreen::OnDPadLeft()
{
    if (CanSwitchFocusToView(GetPrimaryView()))
        SetFocusedView(GetPrimaryView());
}

inline void PlayerStashScreen::OnDPadRight()
{
    if (CanSwitchFocusToView(GetSecondaryView()))
        SetFocusedView(GetSecondaryView());
}

void UpdateStashInfo(const Player & tPlayer)
{
    UIID idPlayerNumber;
    idPlayerNumber.SetFromParent(UIWND_INVENTORY_CONSOLE_ROOT, "PlayerLabel");
    CRefString sNumber;
    if (PlayerGetLocalPlayerCount() > 1 && PlayerIsLocal(&tPlayer))
        sNumber = CPlayerGetIndicatorString(tPlayer);
    UI_GetTextFromID(idPlayerNumber).SetText(sNumber.str(), FALSE);

    UpdateStashUpgradeInfo(tPlayer);

    UIID idGold;
    idGold.SetFromParent(UIWND_INVENTORY_CONSOLE_ROOT, "GoldBalance");
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer)
    {
        CurrencyType eCurrencyType = CURRENCY_GOLD;
        uint64 uGold = ptACDPlayer->GetCurrencyAmount(eCurrencyType);
        CRefString usAmount = UIText_ValueToGameCurrency(uGold, eCurrencyType);
        UI_GetTextFromID(idGold).SetText(usAmount, FALSE);
    }
}

inline BOOL StashCanBeUpgraded(const Player & tPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;

    int32 nSlotCount = ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_SHARED_STASH_SLOTS));
    int32 nMaxSlots = ACDInventoryGetMaxStashSlots(*ptACDPlayer);
    return nSlotCount < nMaxSlots;
}

inline BOOL CanAffordUpgrade(const Player & tPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;

    int32 nUpgradeCost = ACDInventoryGetStashUpgradeCost(*ptACDPlayer);
    return ptACDPlayer->CanAffordCurrencyCost(nUpgradeCost, CURRENCY_GOLD);
}

void UpdateStashUpgradeInfo(const Player & tPlayer)
{
    BOOL bCanBuy = StashCanBeUpgraded(tPlayer);

    UIID idUpgradeTitle = GetUIIDNone();
    idUpgradeTitle.SetFromParent(UIWND_INVENTORY_CONSOLE_STASH_UPGRADE, "UpgradeSlotTitle");
    UI_GetWindowFromID(idUpgradeTitle).SetActive(bCanBuy);

    UIID idSlotCount = GetUIIDNone();
    idSlotCount.SetFromParent(UIWND_INVENTORY_CONSOLE_STASH_UPGRADE, "UpgradeSlotCount");
    CRefString sSlotText;
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (bCanBuy && ptACDPlayer)
    {
        int32 nNumSlotsToBuy = ACDInventorySlotGetUpgradeIncrement(SLOT_PLAYER_SHARED_STASH);
        int32 nSlotCount = ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_SHARED_STASH_SLOTS));
        int32 nMaxSlots = ACDInventoryGetMaxStashSlots(*ptACDPlayer);
        if (nSlotCount + nNumSlotsToBuy * 2 > nMaxSlots)
            nNumSlotsToBuy = nMaxSlots - nSlotCount;
        sSlotText = LocalizedSprintf(StringListFetch("ConsoleInventory:UpgradeStashSlots").str(), UnicodeText::FormatNumber(nNumSlotsToBuy, TRUE, FNS_NORMAL, FALSE).str(), NULL);
    }
    UI_GetTextFromID(idSlotCount).SetText(sSlotText.str(), FALSE);

    UIID idGoldCost = GetUIIDNone();
    idGoldCost.SetFromParent(UIWND_INVENTORY_CONSOLE_STASH_UPGRADE, "UpgradeSlotCost");
    CRefString sUpgradeCost;
    if (bCanBuy && ptACDPlayer)
    {
        CurrencyType eCurrencyType = CURRENCY_GOLD;
        int32 nUpgradeCost = ACDInventoryGetStashUpgradeCost(*ptACDPlayer);
        CRefString sCost = UIText_ValueToGameCurrency(nUpgradeCost, eCurrencyType);
        Set_Text_Color(sCost, ptACDPlayer->CanAffordCurrencyCost(nUpgradeCost, eCurrencyType) ? FONTCOLOR_WHITE : FONTCOLOR_RED);
        sUpgradeCost = LocalizedSprintf(StringListFetch("ConsoleInventory:UpgradeStashGoldCost").str(), sCost.str(), NULL);
    }

    if (ptACDPlayer)
    {
        uint32 nStashSlots = ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_SHARED_STASH_SLOTS));
        if (sgptStashGlobals->m_nStashSlots != nStashSlots)
        {
            sgptStashGlobals->m_nStashSlots = nStashSlots;
            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        }
    }
    UI_GetTextFromID(idGoldCost).SetText(sUpgradeCost.str(), FALSE);
}

void OnStashUpdate()
{
    if (sgptStashGlobals && sgptStashGlobals->IsActive())
    {
        sgptStashGlobals->Update();
        sgptStashGlobals->SwitchFocusIfCurrentFocusIsInvalid();
    }
}

void OnStashChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce)
{
    if (sgptStashGlobals && sgptStashGlobals->IsActive() &&
        (bForce || tOldLoc.eActorLocation != tNewLoc.eActorLocation || tNewLoc.tInvLocation.eSlot != tOldLoc.tInvLocation.eSlot))
    {
        sgptStashGlobals->RequestUpdate(KEEP_ITEM_SELECTED);
    }
}

void OnStashLeftStick(const UIID &, const int32 nControllerIndex)
{
    if (sgptStashGlobals && sgptStashGlobals->IsActive())
    {
        if (sgptStashGlobals->RingHasFocus() || !sgptStashGlobals->HandleLeftStickViewSwitching(nControllerIndex))
            sgptStashGlobals->HandleLeftStickInput(nControllerIndex);
    }
}

void OnStashRightStick(const UIID &, const int32 nControllerIndex)
{
    if (sgptStashGlobals && sgptStashGlobals->IsActive())
        sgptStashGlobals->HandleRightStickInput(nControllerIndex);
}

void OnStashButton(const UIID &, const int32 nControllerIndex, const InputButton eInputButton)
{
    if (sgptStashGlobals && sgptStashGlobals->IsActive())
        sgptStashGlobals->HandleButtonInput(nControllerIndex, eInputButton);
}

void OnStashButton3()
{
    ACDID idPrimarySelectedItem = sgptStashGlobals->GetPrimaryView()->GetSelectedItem();
    ACDID idRightSelectedItem = sgptStashGlobals->GetSecondaryView()->GetSelectedItem();
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);

    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3) == ACTION_DETAILS)
    {
        sgptStashGlobals->SetBrowsingMode(UIInventoryScreen::BROWSE_DETAILS);
        sgptStashGlobals->GetPrimaryView()->SelectItem(idPrimarySelectedItem);
        sgptStashGlobals->GetSecondaryView()->SelectItem(idRightSelectedItem);
    }
    else if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3) == ACTION_LIST)
    {
        sgptStashGlobals->SetBrowsingMode(UIInventoryScreen::BROWSE_LIST);
        sgptStashGlobals->GetPrimaryView()->SelectItem(idPrimarySelectedItem);
        sgptStashGlobals->GetSecondaryView()->SelectItem(idRightSelectedItem);
    }
    else if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3) == ACTION_IDENTIFY)
    {
        UIInventoryView * ptFocusedView = sgptStashGlobals->GetFocusedView();
        if (ptFocusedView)
        {
            ACDID idSelected = ptFocusedView->GetSelectedItem();
            ptFocusedView->SetIdentifyingItemACD(idSelected);
        }
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

static void sDoPurchase(const UIID & id)
{
    sgptStashGlobals->BuySlots();
}

void OnStashButtonR3()
{
    if (sgptStashGlobals && ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON) == ACTION_UPGRADE_STASH)
    {
        Player * ptPlayer = sgptStashGlobals->GetPlayer();
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            int32 nUpgradeCost = ACDInventoryGetStashUpgradeCost(*ptACDPlayer);
            CRefString sUpgradeCost = UIText_ValueToGameCurrency(nUpgradeCost, CURRENCY_GOLD);
            Set_Text_Color(sUpgradeCost, FONTCOLOR_WHITE);
            CRefString sMsg = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_STASH), "Warning"), sUpgradeCost.str(), NULL);
            UIConfirmation::Show(sMsg.str(), sDoPurchase, NULL, FALSE, TRUE, ptPlayer->nControllerIndex, -1);
        }
    }
}

}  // namespace Console
}  // namespace UIInventory

namespace UIStash {
namespace Console {

void InitializeForGame()
{
    if (GameIsStartUp())
        return;

    UIInventory::Console::sgptStashGlobals = new UIInventory::Console::PlayerStashScreen;
}

void DisposeForGame()
{
    if (GameIsStartUp())
        return;

    ::delete UIInventory::Console::sgptStashGlobals;
    UIInventory::Console::sgptStashGlobals = NULL;
}

void OpenForController(int32 nControllerIndex)
{
    UIInventory::Console::sgptStashGlobals->OpenForController(nControllerIndex);
}

void Close()
{
    if (UIInventory::Console::sgptStashGlobals->IsActive())
        UIInventory::Console::sgptStashGlobals->Close();
}

}  // namespace Console
}  // namespace UIStash

namespace {

BOOL RingIncludeControlPredicate(const UIID & idControl)
{
    return idControl != UIWND_INVENTORY_CONSOLE_RING_GLOW;
}

void OnUpdateCategoryUI(const UIID & idItemUI, const UIInventory::Console::ItemCategory eCategory)
{
    if (UIInventory::Console::sgptStashGlobals && UIInventory::Console::sgptStashGlobals->IsActive())
        UIInventory::Console::sgptStashGlobals->UpdateCategoryUI(idItemUI, eCategory);
}

inline void ShowDetailsFocusHighlight(const UIInventory::Console::UIInventoryView * ptView, const BOOL bShow)
{
    if (ptView)
    {
        UIID idHighlight = UIIDCreateFromParent(ptView->GetRootUIID(), ".LayoutRoot.ItemDetails.Item.Focused");
        UI_GetWindowFromID(idHighlight).SetActive(bShow);
    }
}

}  // namespace
