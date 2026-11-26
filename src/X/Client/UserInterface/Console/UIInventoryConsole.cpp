// X/Client/UserInterface/Console/UIInventoryConsole.cpp
//
// The console (controller) player inventory screen: the ring of equipment slots, the category,
// list and details views, and the socketing / dyeing / add-sockets flows. One screen object is
// created per game (sgptInventoryGlobals); the free functions below forward UI input to it.

#include "UIInventoryConsole.h"

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Console/ControllerUtils.h"
#include "UIConsoleInventoryAddSocketsView.h"
#include "UIConsoleInventoryCategorySummaryView.h"
#include "UIConsoleInventoryCategoryView.h"
#include "UIConsoleInventoryDetailsView.h"
#include "UIConsoleInventoryDyeView.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryListView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventorySocketView.h"
#include "UIConsoleInventoryView.h"
#include "UIConsoleItemCategories.h"
#include "ConsoleUIRActorManager.h"
#include "ConsoleScreenManager.h"
#include "ConsoleModalNotificationManager.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../../Common/OnlineService/Callback/Callback.h"
#include "ControllerActions.h"
#include "Wrappers/RingWrapper.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Items.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/HotbarButton.h"
#include "../../../Common/Target.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Sigma/RefString.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Common/CosmeticItems.h"

// Declared here because their headers are not adopted yet; each comment names the defining file.
extern const UIID UIBTN_INVENTORY_CONSOLE_RING_ARM_LEFT, UIBTN_INVENTORY_CONSOLE_RING_ARM_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_BAG, UIBTN_INVENTORY_CONSOLE_RING_BRACERS, UIBTN_INVENTORY_CONSOLE_RING_FEET, UIBTN_INVENTORY_CONSOLE_RING_FINGER_LEFT, UIBTN_INVENTORY_CONSOLE_RING_FINGER_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_HANDS, UIBTN_INVENTORY_CONSOLE_RING_HEAD, UIBTN_INVENTORY_CONSOLE_RING_LEGS, UIBTN_INVENTORY_CONSOLE_RING_NECK, UIBTN_INVENTORY_CONSOLE_RING_SHOULDER, UIBTN_INVENTORY_CONSOLE_RING_TORSO, UIBTN_INVENTORY_CONSOLE_RING_WAIST;  // X/Client/UserInterface/UIIDs.h
extern const UIID UIWND_INVENTORY_CONSOLE_RING_GLOW, UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR,
    UIWND_INVENTORY_CONSOLE_ROOT;  // X/Client/UserInterface/UIIDs.h
extern const UIID UIDLG_INVENTORY_CONSOLE, UIDLG_INVENTORY_CONSOLE_LIST, UIDLG_INVENTORY_CONSOLE_DETAILS, UIDLG_INVENTORY_CONSOLE_EQUIPPED, UIDLG_INVENTORY_CONSOLE_SOCKETING, UIDLG_INVENTORY_CONSOLE_DYEING, UIDLG_INVENTORY_CONSOLE_ADD_SOCKETS;  // X/Client/UserInterface/UIIDs.h
extern const UIID UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED;  // X/Client/UserInterface/UIIDs.h
void InputClearRemappings();  // X/Client/Input/Input.cpp
void CTradeCancel(const Player * ptLocalPlayer, BOOL bForce, BOOL bSendMessage);  // X/Client/Input/Input.cpp
void InputClearRemappings();
// X/Client/CTrade.cpp
UIStoryboard * UI_GetStoryboardFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
int32 UI_GetPrimaryUIController();  // X/Client/UserInterface/UI.cpp
BOOL ItemIsDye(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
BOOL ItemIsAddSocketsItem(const ActorCommonData & tACDItem);
BOOL ItemCanBeMarkedAsJunk(const ActorCommonData & tACDItem);  // X/Common/Items.cpp  // X/Common/Items.cpp
struct PlaySoundOptions;  // X/Client/Sound/Sound.cpp
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
namespace Console {
namespace Screens {
void EnableScreen(const ScreenSets eScreenSet, const int32 idScreen, const BOOL bEnable);  // ConsoleScreenManager.cpp
BOOL OpenToScreen(const ScreenSets eScreenSet, const int32 idScreen, int32 nControllerOpening, BOOL fSkipTransition);  // ConsoleScreenManager.cpp:1104
}  // namespace Screens
}  // namespace Console
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
// X/Common/Networking/GameMessage.h

// X/Client/Input/InputNX64.cpp:1993, DWARF InputButton is int32.
void InputRemapSingleJoyconButton(int32 eFrom, int32 eTo);
UIItemOverlay ItemGetOverlay(const ActorCommonData & tACDItem, const ActorCommonData & tACDPlayer); // X/Client/CItems.cpp:117
BOOL ItemIsUsable(const ActorCommonData & tACDItem); // X/Common/Items.cpp:3499
// X/Client/CActorInventory.cpp
void CACDInventoryRequestSplitStack(const ActorCommonData * ptACDStack, const InventoryLocation & tInvLocDest, const uint64 uAmount);
void CACDInventoryDestroy(ActorCommonData & tACDItem);
void CACDInventoryRequestUse(ActorCommonData & tACDItem, ACDID idACDTarget, BOOL bFromHotbar, Player * ptPlayer);
// X/Common/ActorInventory.cpp
BOOL ACDInventoryCanHold(const ActorCommonData & tACD, const ActorCommonData & tACDItem, const InventorySlotType eSlot, IVector2D & tWhereCanHold, const int32 * pnSlotYMinMax);
ACDID ACDInventoryGetUltimateContainer(const ActorCommonData & tACDItem);
BOOL ItemIsLegendary(GBID gbidItem);  // X/Common/Items.cpp:5080
SocketReason ItemCanSocketInto(const ActorCommonData & tACDItemToSocket, ActorCommonData & tACDItemSocketedInto);  // X/Common/Items.cpp
GameError ItemCanBeUsed(ACDID idACDUser, const ActorCommonData & tACDItem, const Target * ptTarget);  // X/Common/Items.cpp
void CACDInventoryRequestToSocket(const ActorCommonData & tACDGem, const ActorCommonData & tACDItemToReceiveGem, Player * ptPlayer);  // X/Client/CActorInventory.cpp
void PlayerSendPotionSlotChangeToServer(const HotbarButtonData & tButtonData, Player * ptPlayer);  // X/Client/CPlayerInput.cpp
// X/Common/Player.h

GameError ItemCanDrop(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:9360


// X/Client/UserInterface/Interface/UIGame.cpp




Player * LocalPlayerGet();  // X/Client/CPlayer.cpp:1612
BOOL ItemIsGold(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
BOOL ItemIsHealthPotion(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
BOOL ItemIsCosmeticItem(const GBID gbidItem);  // X/Common/Items.cpp
BOOL ItemIsCraftingReagent(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
BOOL ItemIsGem(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
BOOL ItemIsSetItem(const GBID gbid);  // X/Common/Items.cpp
const CmdLineParams * CmdLineGetParams();
namespace UIEventQueue {
namespace Console {
void ReportItemPickup(const Player * ptPlayer, const ActorCommonData & tACDItem);  // X/Client/UserInterface/Console/UIConsoleEventQueue.cpp
void DisplayItemEquippedMessageForPlayer(const Player * ptPlayer, const ActorCommonData & tACDItem);  // X/Client/UserInterface/Console/UIConsoleEventQueue.cpp
}  // namespace Console
}  // namespace UIEventQueue
namespace UIFastEquip {
namespace Console {
void ReportItemPickup(const Player * ptPlayer, const ActorCommonData & tACDItem);  // X/Client/UserInterface/Console/UIConsoleFastEquip.cpp
}  // namespace Console
}  // namespace UIFastEquip
BOOL ItemUseIsTargettedInInventory(ItemUseType eItemUseType);  // X/Common/Items.cpp
BOOL ItemUseIsTargettedInWorld(ItemUseType eItemUseType);  // X/Common/Items.cpp
namespace UIMail {
namespace Console {
void ConfirmPlayerGiftSend(ActorCommonData & tACDItem, Player * ptPlayer);  // X/Client/UserInterface/Console/UIConsoleMail.cpp
}  // namespace Console
}  // namespace UIMail
#include "../Interface/UITutorials.h"



extern UITutorialManager gtTutorialMgr;
namespace Tutorial {
BOOL PlayerHasSeen(const Player & tPlayer, const SNO snoTutorial);  // X/Client/UserInterface/Interface/UITutorials.cpp
}  // namespace Tutorial
namespace UIStackSplit {
namespace Console {
enum StackSplitType {
    STACKSPLIT_GENERIC = 0,
};
typedef void (*UIStackSplitCallback)(const ACDID, const uint64);
void Open(const ACDID idACDStack, const uint64 uInitialVal, const uint64 uStackMax, UIStackSplitCallback pfnCallback, StackSplitType eType);  // X/Client/UserInterface/Console/UIConsoleStackSplit.cpp
}  // namespace Console
}  // namespace UIStackSplit
namespace UIGame {
void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer);
void DisplayErrorString(const CRefString & sError);
}  // namespace UIGame
// X/Common/Player.cpp
Player * PlayerGetByControllerIndex(int32 nControllerIndex);
BOOL PlayerCanOpenInventory(const ActorCommonData & tACDPlayer);
// X/Client/UserInterface/Interface/UIBossEncounter.cpp
namespace UIBossEncounter {
BOOL IsActive();
}  // namespace UIBossEncounter
namespace UIUtils {
namespace Console {
BOOL ShowTieredRiftMessage(const Player * ptPlayer);  // X/Client/UserInterface/Console/UIConsoleUtils.cpp
void DropItem(ACDID idSelectedItem, Player * ptPlayer);  // X/Client/UserInterface/Console/UIConsoleUtils.cpp
void YieldFocus(const UIID & idYielding);
void SetJunkFlag(ActorCommonData & ptACD, BOOL bJunk, Player * ptPlayer);  // X/Client/UserInterface/Console/UIConsoleUtils.cpp  // X/Client/UserInterface/Console/UIConsoleUtils.cpp
}  // namespace Console
}  // namespace UIUtils
namespace UICharacterDetails {
namespace Console {
void SetReturnScreen(int32 nScreen);  // X/Client/UserInterface/Console/UIConsoleCharacterDetails.cpp
}  // namespace Console
}  // namespace UICharacterDetails

namespace UIInventory {
namespace Console {
// X/Client/UserInterface/Console/UIConsoleInventory.cpp
void UpdateNavBarTabState(const Player * ptPlayer, const UIID & idParent, LPCSTR szNavBar, int32 nTab);
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
void UpdatePlayerInfoUI(const Player & tPlayer, const UIID & idParentControl, VisualInventory & tVisualInventory);
}  // namespace Console
// X/Client/UserInterface/Interface/UIInventoryCommon.cpp
void PlayItemInventorySound(const ActorCommonData & tACDItem);
void TryToQuickMoveToLocation(ActorCommonData & tACDOwner, ActorCommonData & tACDItem, const InventorySlotType eSlot);
void QuickMoveToEmptySlot(const ActorCommonData & tACD, ActorCommonData & tACDItem, int32 & nNumSlots, InventorySlotType * eValidSlots, BOOL bUseOffhand, Player * ptPlayer);
namespace Console {

enum ButtonActions {
    ACTION_CHOOSE_SLOT = 1,
    ACTION_OPEN_SLOT = 2,
    ACTION_CLOSE_SLOT = 3,
    ACTION_OPEN_CATEGORY = 4,
    ACTION_SCROLL_LIST = 5,
    ACTION_EQUIP = 6,
    ACTION_UNEQUIP = 7,
    ACTION_CLOSE = 8,
    ACTION_DETAILS = 9,
    ACTION_LIST = 10,
    ACTION_TOGGLE_STAT_RANGES = 11,
    ACTION_DROP = 12,
    ACTION_IDENTIFY = 13,
    ACTION_IDENTIFY_CANCEL = 14,
    ACTION_BEGIN_SOCKET = 15,
    ACTION_END_SOCKET = 16,
    ACTION_ACCEPT_SOCKETING = 17,
    ACTION_BEGIN_DYE = 18,
    ACTION_END_DYE = 19,
    ACTION_ACCEPT_DYEING = 20,
    ACTION_BEGIN_ADDING_SOCKETS = 21,
    ACTION_END_ADDING_SOCKETS = 22,
    ACTION_ACCEPT_ADDING_SOCKETS = 23,
    ACTION_BACK = 24,
    ACTION_MARK_AS_JUNK = 25,
    ACTION_USE = 26,
    ACTION_CHARACTER_DETAILS = 27,
    ACTION_SELECT_TRADE_ITEM = 28,
    ACTION_SPLIT_STACK = 29,
    ACTION_SELECT_ITEM_TO_MAIL = 30,
    ACTION_DESTROY = 31,
    ACTION_EQUIP_POTION = 32,
};

class PlayerInventoryScreen : public UIInventory::Console::UIInventoryScreen {  // size 0x13a8 (InitializeForGame: operator new(0x13a8))
public:
    // 2.7.7: a blz::function the constructor clears and OnOpen sets to a lambda capturing this
    // that calls OnButton1(0) (impl vtable 0x71010a5fc0, call slot 0x7100371f30).
    blz::function<void()> unk_200;  // 0x200
    UIInventory::Console::InventoryItems tItemsProvider;  // 0x228
    UIInventory::Console::EquippedItem tEquippedItemProvider;  // 0xb10
    UIInventory::Console::SingleItemProvider tSyncedDetailsProvider;  // 0xb48
    UIInventory::Console::CosmeticItemsProvider tCosmeticItemsProvider;  // 0xb80
    UIInventory::Console::CraftingMatItemsProvider tCraftingMatItemsProvider;  // 0xc68
    UIInventory::Console::UIInventoryListView tListView;  // 0xcd8
    UIInventory::Console::UIInventoryListView tCosmeticListView;  // 0xd60
    UIInventory::Console::UIInventoryListView tCraftingMatListView;  // 0xde8
    UIInventory::Console::UIInventoryDetailsView tDetailsView;  // 0xe70
    UIInventory::Console::UIInventoryDetailsView tEquippedView;  // 0xee8
    UIInventory::Console::UIInventoryDetailsView tBagDetailsView;  // 0xf60
    UIInventory::Console::UIInventorySocketView tSocketView;  // 0xfd8
    UIInventory::Console::UIInventoryDyeView tDyeView;  // 0x1068
    UIInventory::Console::UIInventoryAddSocketsView tAddSocketsView;  // 0xfb0 in 2.6.2. 2.7.7 has a 0x28-byte object at 0x10d8 instead
    // (constructor sub_71003739a0; it reaches the add-sockets view through the pointer at its +0x18).
    UIInventory::Console::UIInventoryCategoryView tCategoriesView;  // 0x1100
    UIInventory::Console::UIInventoryCategorySummaryView tCategorySummaryView;  // 0x1198
    UIInventory::Console::UIPortraitFrameSummaryView tPortraitFrameSummaryView;  // 0x1200
    VisualInventory tVisualInventory;  // 0x1268
    PlayerInventoryScreen();
    virtual ~PlayerInventoryScreen();
    virtual void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);  // vtable slot 9
    void OnButton1(int32 nControllerIndex);
    void OnButton2(int32 nControllerIndex);
    void OnButton3(int32 nControllerIndex);
    void OnButton4(int32 nControllerIndex);
    void OnButtonL3(int32 nControllerIndex);
    void OnButtonR3(int32 nControllerIndex);
    void OpenSelectedCategory();
    void OpenCategory(UIInventory::Console::ItemCategory eCategory);
    void CloseCategory();
    void ShowTopLevelCategories();
    void ShowGemCategories();
    void ShowCosmeticCategories();
    void ToggleSelectedItemAsJunk();
    BOOL CanToggleStatRangeDisplay();
    void ToggleStatRangeDisplay();
    void SetGemToSocket(const ACDID idGem);
    void BackUpSocketingFlow();
    void CancelSocketing();
    void SocketItem();
    void SetDyeToUse(const ACDID idDye);
    void BackUpDyeingFlow();
    void CancelDyeing();
    void DyeItem();
    void SetAddSocketsItemToUse(const ACDID idAddSocketsItem);
    void BackUpAddSocketsFlow();
    void CancelAddSockets();
    void AddSocketsToItem();
    void BackUpCosmeticsFlow();
    void SetSelectMode(UIInventory::Console::InventoryItems::ProviderMode eMode);
    void CacheRingSelection();
    BOOL RestoreCachedRingSelection();
    void UpdateCategoryUI(const UIID & idItemUI, const UIInventory::Console::ItemCategory eCategory);
    void ResetScreen();
protected:
    virtual BOOL CanOpenForController(int32 nControllerIndex) const;  // vtable slot 15
    virtual void OnOpen();  // vtable slot 10
    virtual void OnClose();  // vtable slot 11
    virtual void OnUpdate();  // vtable slot 13
    virtual void OnRequestedUpdate();  // vtable slot 12
    virtual void UpdateControllerActions(::Console::ControllerActionManager & tActionManager);  // vtable slot 14
    virtual void OnSetActive(BOOL bActive);  // vtable slot 16
    virtual void InitializeRingWrapper(::Console::RingWrapper & tRingWrapper);  // vtable slot 17
    virtual UIFilterType GetDefaultRingSelection() const;  // vtable slot 18
    virtual void OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType);  // vtable slot 22
    virtual void InitRingFilters(UIIDToFilterMap & tFilterMap);  // vtable slot 21
    virtual UIInventory::Console::UIInventoryView * GetPrimaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode eBrowsingMode);  // vtable slot 23
    virtual UIInventory::Console::UIInventoryView * GetSecondaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode);  // vtable slot 24
    virtual BOOL ShouldEnableRingIcon(const UIFilterType eFilterType);  // vtable slot 20
    enum InventoryState {
        BROWSE_CATEGORY_STATE = 0,
        BROWSE_SUBCATEGORY_STATE = 1,
        BROWSE_GEMS_STATE = 2,
        BROWSE_ITEMS_STATE = 3,
        BROWSE_ITEMS_TO_SOCKET_STATE = 4,
        BROWSE_ITEMS_TO_DYE_STATE = 5,
        BROWSE_ITEMS_TO_ADD_SOCKETS_TO_STATE = 6,
        BROWSE_COSMETICS_STATE = 7,
        BROWSE_PORTRAIT_FRAMES_STATE = 8,
        BROWSE_CRAFTING_MATS_STATE = 9,
        BUSY_SOCKETING_STATE = 10,
        FINISHED_SOCKETING_STATE = 11,
        READY_TO_SOCKET_AGAIN_STATE = 12,
        BUSY_DYEING_STATE = 13,
        FINISHED_DYEING_STATE = 14,
        BUSY_ADDING_SOCKETS_STATE = 15,
        FINISHED_ADDING_SOCKETS_STATE = 16,
    };
public:
    // Private in the original; the module's free functions (OpenForController and friends) are
    // its friends there. Kept public here until the friend declarations are known.
    BOOL m_fInTradeSelectMode;  // 0x1388
    BOOL m_fInMailSelectMode;  // 0x138c
    BOOL unk_1390;  // 0x1390; tested as a flag in OnUpdate and the button handlers, not set by the constructor
    InventoryState m_eInventoryState;  // 0x1394
    UIInventory::Console::ItemCategory m_eCurrentCategory;  // 0x1398
    UIFilterType m_eCachedRingSelection;  // 0x139c
    ACDID m_idLastSelectedItem;  // 0x13a0
    ACDID m_idLastIdentifyingACD;  // 0x13a4
private:
    BOOL CanOpenSelectedCategory();
    void UpdateSocketingActions(::Console::ControllerActionManager & tActionManager);
    void UpdateDyeingActions(::Console::ControllerActionManager & tActionManager);
    void UpdateAddSocketsActions(::Console::ControllerActionManager & tActionManager);
    void UpdateBagItemActions(::Console::ControllerActionManager & tActionManager);
    void UpdateNormalItemActions(::Console::ControllerActionManager & tActionManager);
    void UpdateTradeSelectActions(::Console::ControllerActionManager & tActionManager);
    void UpdateMailSelectActions(::Console::ControllerActionManager & tActionManager);
    void UpdateEquippedPrompt();
    void UpdateSocketingUI();
    void UpdateSocketingState();
    void UpdateDyeingUI();
    void UpdateDyeingState();
    void UpdateAddSocketsUI();
    void UpdateAddSocketsState();
    BOOL ShouldShowSocketingUI();
    BOOL ShouldShowCosmeticUI();
    BOOL ShouldShowDyeingUI();
    BOOL ShouldShowAddSocketsUI();
    BOOL ShowingBagUI();
    void SyncCategories();
    void SyncItems();
    void EquipItem();
    void EquipPotion();
    void UnMarkEquippedItemAsNew(const UIFilterType eFilterType);
};
// 2.7.7 size is 0x13a8. One part is still the 2.6.2 layout: tAddSocketsView is 0x70 where
// 2.7.7 has the 0x28-byte object above.
static_assert(sizeof(PlayerInventoryScreen) == 0x13a8 + (0x70 - 0x28), "PlayerInventoryScreen");

BOOL bExitViaTrade;
static BOOL sbTabNavigationEnabled;

extern PlayerInventoryScreen * sgptInventoryGlobals;

void UpdateRingNewItemIndicators();
BOOL RingIncludeControlPredicate(const UIID & idControl);
void RefreshViews(const MaintainSelectionOptions eSelectionType);
void EnableTabNavigation(const BOOL bEnable);
ACDID GetSelectedItem();
void AnimateEquippedItemIcon(const UIID & idSlot);
void UpdateInventoryItems();
void sDestroyGift(const UIID & id);
void sSplitStack(const ACDID idACDStack, const uint64 uAmount);
void sLearnCosmetic(const UIID & id);

void OnInventoryButtonLeftTrigger();
void OnInventoryButtonRightTrigger();
void OnInventoryDPadLeft();

PlayerInventoryScreen::~PlayerInventoryScreen()
{
}

void PlayerInventoryScreen::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)
{
    switch (eInputButton)
    {
    case BUTTON_1:
    case BUTTON_TOUCH_PAD:
        OnButton1(nControllerIndex);
        break;
    case BUTTON_2:
        OnButton2(nControllerIndex);
        break;
    case BUTTON_3:
        OnButton3(nControllerIndex);
        break;
    case BUTTON_4:
        OnButton4(nControllerIndex);
        break;
    case BUTTON_LEFT_STICK_BUTTON:
        OnButtonL3(nControllerIndex);
        break;
    case BUTTON_RIGHT_STICK_BUTTON:
        OnButtonR3(nControllerIndex);
        break;
    case BUTTON_LEFT_TRIGGER:
        OnInventoryButtonLeftTrigger();
        break;
    case BUTTON_RIGHT_TRIGGER:
        OnInventoryButtonRightTrigger();
        break;
    case BUTTON_DPAD_LEFT:
        OnInventoryDPadLeft();
        break;
    default:
        UIInventoryScreen::HandleButtonInput(nControllerIndex, eInputButton);
        break;
    }
}

BOOL PlayerInventoryScreen::CanOpenForController(int32 nControllerIndex) const
{
    return UIInventoryScreen::CanOpenForController(nControllerIndex);
}

void PlayerInventoryScreen::OnSetActive(BOOL bActive)
{
    if (bActive)
    {
        if (m_fInTradeSelectMode)
        {
            ShowNavBar(UIDLG_INVENTORY_CONSOLE, "TradeSelectNav");
        }
        else if (m_fInMailSelectMode)
        {
            ShowNavBar(UIDLG_INVENTORY_CONSOLE, "MailSelectNav");
        }
        else
        {
            ShowNavBar(UIDLG_INVENTORY_CONSOLE, "InventoryNav");
            UpdateNavBarTabState(GetPlayer(), UIDLG_INVENTORY_CONSOLE, "LayoutRoot.Navigation.InventoryNav", 1);
        }

        UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING_CURSOR).SetActive(TRUE);
        EnableRing(TRUE);
        RequestUpdate(RESET_SELECTION);
        ::Console::Screens::ShowModalDialog(UI_GetPrimaryUIController(), UIDLG_INVENTORY_CONSOLE, FALSE, GetUIIDNone());
    }
    else
    {
        ::Console::Screens::CloseModalDialog(UIDLG_INVENTORY_CONSOLE);
    }
}

void PlayerInventoryScreen::InitializeRingWrapper(::Console::RingWrapper & tRingWrapper)
{
    tRingWrapper.WrapUsingContainerControl(UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR, RingIncludeControlPredicate, FALSE);
}

UIFilterType PlayerInventoryScreen::GetDefaultRingSelection() const
{
    return 0;
}

void PlayerInventoryScreen::OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType)
{
    if (ShouldShowSocketingUI())
        CancelSocketing();

    if (ShouldShowDyeingUI())
        CancelDyeing();

    if (ShouldShowAddSocketsUI())
        CancelAddSockets();

    tItemsProvider.SetFilter(eNewFilterType);
    tEquippedItemProvider.SetFilter(eNewFilterType);
    tSyncedDetailsProvider.SetFilter(eNewFilterType);
    tCosmeticItemsProvider.SetFilter(eNewFilterType);
    UpdateEquippedPrompt();

    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));

    if (eNewFilterType == FILTER_BAG)
    {
        ShowTopLevelCategories();
    }
    else
    {
        RefreshViews(RESET_SELECTION);
    }

    UnMarkEquippedItemAsNew(eOldFilterType);
}

void PlayerInventoryScreen::InitRingFilters(UIIDToFilterMap & tFilterMap)
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

UIInventoryView * PlayerInventoryScreen::GetPrimaryViewForMode(const BrowsingMode eBrowsingMode)
{
    if (GetFilter() == FILTER_BAG && (m_eInventoryState == BROWSE_CATEGORY_STATE || m_eInventoryState == BROWSE_SUBCATEGORY_STATE))
    {
        return &tCategoriesView;
    }

    if (eBrowsingMode == BROWSE_LIST)
    {
        if (m_eInventoryState == BROWSE_COSMETICS_STATE || m_eInventoryState == BROWSE_PORTRAIT_FRAMES_STATE)
            return &tCosmeticListView;
        if (m_eInventoryState == BROWSE_CRAFTING_MATS_STATE)
            return &tCraftingMatListView;
        return &tListView;
    }
    else
    {
        return &tDetailsView;
    }
}

































BOOL PlayerInventoryScreen::ShouldEnableRingIcon(const UIFilterType eFilterType)
{
    size_t nItems = tItemsProvider.GetItemsWithFilter(eFilterType).size();
    if (eFilterType == FILTER_COSMETICS || eFilterType == FILTER_BAG)
        nItems += tCosmeticItemsProvider.GetItemsWithFilter(FILTER_COSMETICS).size();
    ACDID idEquippedItem = GetEquippedItem(GetPlayer(), eFilterType, FALSE);
    return nItems != 0 || idEquippedItem != ACDID_INVALID;
}

PlayerInventoryScreen * sgptInventoryGlobals;

static inline void sResetEquipAnimations()
{
    UIIDArray aridTargets;
    sgptInventoryGlobals->GetRingUI().GetTargetControlIDs(aridTargets);
    for (uint32 i = 0; i < aridTargets.size(); ++i)
    {
        UIID idStoryboard = UIIDCreateFromParent(aridTargets[i], ".LoadoutEquipItem");
        UIStoryboard & tStoryboard = *UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Stop();
        tStoryboard.Reset();
    }
}

void UpdateRingNewItemIndicators()
{
    Player * ptPlayer = sgptInventoryGlobals->GetPlayer();
    NewItemIndicatorManager<ACDID> * ptNewItemManager = ptPlayer->ptNewItemManager;
    NewItemIndicatorManager<ACDID> * ptNewEliteItemManager = ptPlayer->ptNewEliteItemManager;

    ptNewItemManager->RemoveItemsIf(RemovedItemPredicate(SLOT_PLAYER_BACKPACK, SLOT_PLAYER_BODYSLOT_END));
    ptNewEliteItemManager->RemoveItemsIf(RemovedItemPredicate(SLOT_PLAYER_BACKPACK, SLOT_PLAYER_BODYSLOT_END));

    UIFilterType eFilterType;
    UIID idKey;

    const UIIDToFilterMap & tFilterMap = sgptInventoryGlobals->GetFilterMap();
    POS pos = tFilterMap.GetStartAssoc();
    while (pos)
    {
        tFilterMap.GetNextAssoc(pos, idKey, eFilterType);
        UIID idNewIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.NewStar");
        UIID idNewEliteIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.NewStarElite");
        BOOL bRingEnabled = sgptInventoryGlobals->GetRingUI().IsEnabled();
        BOOL bNewElite = FilterHasNewItems(eFilterType, ptPlayer->idPlayerACD, *ptNewEliteItemManager, TRUE);
        BOOL bNewItem = !bNewElite && FilterHasNewItems(eFilterType, ptPlayer->idPlayerACD, *ptNewItemManager, TRUE);
        UI_GetWindowFromID(idNewIndicator).SetActive(bRingEnabled && bNewItem);
        UI_GetWindowFromID(idNewEliteIndicator).SetActive(bRingEnabled && bNewElite);

        if (eFilterType == FILTER_BAG && bNewItem)
        {
            SNO snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_GEMINVENTORY);
            if (!Tutorial::PlayerHasSeen(*ptPlayer, snoTutorial) && FilterHasNewItems(FILTER_GEMS, ptPlayer->idPlayerACD, *ptNewItemManager, TRUE))
                gtTutorialMgr.ShowTutorial(idKey, snoTutorial, ptPlayer, TRUE);
        }
    }
}

void PlayerInventoryScreen::OnClose()
{
    InputClearRemappings();
    ResetCurrencyAnimations(UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED);
    sResetEquipAnimations();
    ::Console::UIRActorManager::SetActive(FALSE);
    m_eInventoryState = BROWSE_CATEGORY_STATE;
    SetFilterType(FILTER_NONE);

    if (ShouldShowSocketingUI())
        CancelSocketing();

    if (ShouldShowDyeingUI())
        CancelDyeing();

    if (ShouldShowAddSocketsUI())
        CancelAddSockets();

    SetPrimaryView(NULL);
    SetSecondaryView(NULL);
    CloseSelectedSlot();
    tItemsProvider.SetPlayer(NULL);
    tEquippedItemProvider.SetPlayer(NULL);
    tSyncedDetailsProvider.SetPlayer(NULL);
    tCosmeticItemsProvider.SetPlayer(NULL);
    tCraftingMatItemsProvider.SetPlayer(NULL);
    m_idLastIdentifyingACD = ACDID_INVALID;
    UIUtils::Console::YieldFocus(UIDLG_INVENTORY_CONSOLE);
}

void PlayerInventoryScreen::OnRequestedUpdate()
{
    tItemsProvider.Refresh();
    tCosmeticItemsProvider.Refresh();
    tCraftingMatItemsProvider.Refresh();

    ACDID idEquippedItem = GetEquippedItem(GetPlayer(), GetFilter(), FALSE);
    tEquippedView.SelectItem(idEquippedItem);

    UpdateInventoryItems();
    UpdateEquippedPrompt();
}









































































void PlayerInventoryScreen::UpdateMailSelectActions(::Console::ControllerActionManager & tActionManager)
{
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());

    if (CanScrollDetails())
    {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
    }

    ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
    if (GetPlayer())
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(GetPlayer()->idPlayerACD);
        if (ptACDItem && ptACDPlayer)
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT_ITEM_TO_MAIL, StringListFetch("X1_ConsoleMail:MailItem").str());

        if (GetFilter() != FILTER_BAG)
        {
            if (GetBrowsingMode() == BROWSE_LIST)
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Compare").str());
            else
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List").str());
        }
    }
}

void PlayerInventoryScreen::UpdateTradeSelectActions(::Console::ControllerActionManager & tActionManager)
{
    BOOL bBack;
    if (GetFilter() == FILTER_BAG)
        bBack = m_eInventoryState == BROWSE_CATEGORY_STATE || m_eInventoryState == BROWSE_SUBCATEGORY_STATE || m_eInventoryState == BROWSE_GEMS_STATE ||
            m_eInventoryState == BROWSE_ITEMS_STATE || m_eInventoryState == BROWSE_COSMETICS_STATE || m_eInventoryState == BROWSE_PORTRAIT_FRAMES_STATE ||
            m_eInventoryState == BROWSE_CRAFTING_MATS_STATE;
    else
        bBack = m_eInventoryState == BROWSE_GEMS_STATE;

    if (bBack)
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
    else
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());

    if (CanScrollDetails())
    {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
    }

    ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
    if (GetPlayer())
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(GetPlayer()->idPlayerACD);
        if (ptACDItem && ptACDPlayer)
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT_TRADE_ITEM, StringListFetch("ConsoleTrade:OfferItem").str());

        if (GetFilter() != FILTER_BAG)
        {
            if (GetBrowsingMode() == BROWSE_LIST)
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Compare").str());
            else
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List").str());
        }
    }
}

void PlayerInventoryScreen::UpdateSocketingActions(::Console::ControllerActionManager & tActionManager)
{
    ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
    switch (m_eInventoryState)
    {
    case BROWSE_GEMS_STATE:
    {
        if (idSelectedItem != ACDID_INVALID)
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_DROP, StringListFetch("ConsoleInventory:Drop").str());
            ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
            if (ptACDItem)
            {
                if (ItemIsUnidentified(*ptACDItem))
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_IDENTIFY, StringListFetch("ConsoleInventory:Identify").str());
                else
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_BEGIN_SOCKET, StringListFetch("ConsoleUI:Open").str());
            }

            if (CanScrollDetails())
            {
                CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
            }
        }
        break;
    }
    case BROWSE_ITEMS_TO_SOCKET_STATE:
    case READY_TO_SOCKET_AGAIN_STATE:
        if (idSelectedItem != ACDID_INVALID)
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_END_SOCKET, StringListFetch("ConsoleSocketing:SocketButton").str());
        break;
    case FINISHED_SOCKETING_STATE:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT_SOCKETING, StringListFetch("H2OLayout:Continue").str());
        break;
    default:
        return;
    }

    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
}

void PlayerInventoryScreen::UpdateDyeingActions(::Console::ControllerActionManager & tActionManager)
{
    switch (m_eInventoryState)
    {
    case BROWSE_ITEMS_STATE:
        UpdateNormalItemActions(tActionManager);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_BEGIN_DYE, StringListFetch("ConsoleDyeing:DyeButton").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        break;
    case BROWSE_ITEMS_TO_DYE_STATE:
        if (tItemsProvider.GetItems().size() != 0)
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_END_DYE, StringListFetch("ConsoleDyeing:DyeButton").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        break;
    case FINISHED_DYEING_STATE:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT_DYEING, StringListFetch("H2OLayout:Continue").str());
        break;
    }
}

void PlayerInventoryScreen::UpdateAddSocketsActions(::Console::ControllerActionManager & tActionManager)
{
    switch (m_eInventoryState)
    {
    case BROWSE_ITEMS_STATE:
        UpdateNormalItemActions(tActionManager);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_BEGIN_ADDING_SOCKETS, StringListFetch("ConsoleInventory:AddSocketsButton").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        break;
    case BROWSE_ITEMS_TO_ADD_SOCKETS_TO_STATE:
        if (tItemsProvider.GetItems().size() != 0)
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_END_ADDING_SOCKETS, StringListFetch("ConsoleInventory:AddSocketsButton").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        break;
    case FINISHED_ADDING_SOCKETS_STATE:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT_ADDING_SOCKETS, StringListFetch("H2OLayout:Continue").str());
        break;
    }
}


































inline void PlayerInventoryScreen::UpdateEquippedPrompt()
{
    UIFilterType eFilter = GetFilter();
    if (eFilter != FILTER_BAG && tItemsProvider.GetItems().size() != 0)
    {
        tEquippedView.SetPromptToEquipItems(TRUE);
    }
    else
    {
        tEquippedView.SetPromptToEquipItems(FALSE);
    }
}

void PlayerInventoryScreen::OnButton3(int32 nControllerIndex)
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);

    switch (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3))
    {
    case ACTION_DETAILS:
    {
        ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
        SetBrowsingMode(BROWSE_DETAILS);
        GetPrimaryView()->SelectItem(idSelectedItem);
        break;
    }
    case ACTION_LIST:
    {
        ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
        SetBrowsingMode(BROWSE_LIST);
        GetPrimaryView()->SelectItem(idSelectedItem);
        break;
    }
    case ACTION_IDENTIFY:
        if (UIUtils::Console::ShowTieredRiftMessage(GetPlayer()))
            return;
    {
        ACDID idSelectedItem = GetSelectedItem();
        GetPrimaryView()->SetIdentifyingItemACD(idSelectedItem);
        EnableTabNavigation(FALSE);
        break;
    }
    case ACTION_CHARACTER_DETAILS:
        m_eCachedRingSelection = GetFilter();
        SetFilterType(FILTER_NONE);
        ::Console::Screens::OpenToScreen(::Console::Screens::CHARACTER_DETAILS_SCREEN_SET, 0, nControllerIndex, FALSE);
        UICharacterDetails::Console::SetReturnScreen(0);
        break;
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerInventoryScreen::OnButton4(int32 nControllerIndex)
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_4);
    if (idAction == ::Console::ACTION_NONE)
        return;

    if (UIUtils::Console::ShowTieredRiftMessage(GetPlayer()))
        return;

    if (idAction == ACTION_DROP)
    {
        ACDID idSelectedItem = GetSelectedItem();
        UIUtils::Console::DropItem(idSelectedItem, GetPlayer());
    }
    else if (idAction == ACTION_DESTROY)
    {
        NotificationCreationParams tParams;
        tParams.uszMessage = StringListFetch("ConsoleInventory:DestroyGiftWarning").str();
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sDestroyGift>();
        tParams.nControllerIndex = GetPlayer()->nControllerIndex;
        ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
    }
}

inline void PlayerInventoryScreen::OnButtonR3(int32 nControllerIndex)
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON) == ACTION_MARK_AS_JUNK)
    {
        if (!UIUtils::Console::ShowTieredRiftMessage(GetPlayer()))
            ToggleSelectedItemAsJunk();
    }
}

void PlayerInventoryScreen::ToggleSelectedItemAsJunk()
{
    ACDID idSelectedItem = GetSelectedItem();
    if (idSelectedItem == ACDID_INVALID)
        return;

    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (ptACDItem && ItemCanBeMarkedAsJunk(*ptACDItem))
    {
        BOOL bIsJunk = ptACDItem->AttributesGetInt(ATTRIBUTE_ITEM_FLAGGED_AS_JUNK);
        if (GetPlayer())
        {
            UIUtils::Console::SetJunkFlag(*ptACDItem, !bIsJunk, GetPlayer());
            GetPlayer()->ptNewItemManager->UnMarkAsNew(idSelectedItem);
            GetPlayer()->ptNewEliteItemManager->UnMarkAsNew(idSelectedItem);
        }
        SoundManager::PlaySound(GlobalSNOGet(bIsJunk ? GLOBALSNO_SOUND_UNMARK_AS_JUNK : GLOBALSNO_SOUND_MARK_AS_JUNK), cgtPSODefaults);
        tItemsProvider.Refresh();
        if (bIsJunk)
            GetPrimaryView()->ForceUpdate(KEEP_ITEM_SELECTED);
        else
            GetPrimaryView()->ForceUpdate(KEEP_INDEX_SELECTED);
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}






































void PlayerInventoryScreen::CloseCategory()
{
    if (m_eInventoryState == BROWSE_SUBCATEGORY_STATE || m_eInventoryState == BROWSE_ITEMS_STATE ||
        m_eInventoryState == BROWSE_COSMETICS_STATE || m_eInventoryState == BROWSE_CRAFTING_MATS_STATE)
    {
        SetBrowsingMode(BROWSE_LIST);
        ItemCategory eCurrentCategory = m_eCurrentCategory;
        ShowTopLevelCategories();
        SetFocusedView(GetPrimaryView());
        tCategoriesView.SelectItem((ACDID)eCurrentCategory);
    }
}

inline void PlayerInventoryScreen::ShowTopLevelCategories()
{
    m_eInventoryState = BROWSE_CATEGORY_STATE;
    m_eCurrentCategory = CATEGORY_NONE;
    PopulateTopLevelCategories(tCategoriesView);
    if (GetPrimaryView() != &tCategoriesView)
        SetPrimaryView(&tCategoriesView);
    else
        tCategoriesView.ForceUpdate(RESET_SELECTION);
}

inline void PlayerInventoryScreen::ShowGemCategories()
{
    m_eInventoryState = BROWSE_SUBCATEGORY_STATE;
    m_eCurrentCategory = CATEGORY_GEMS;
    PopulateGemCategories(tCategoriesView);
    if (GetPrimaryView() != &tCategoriesView)
        SetPrimaryView(&tCategoriesView);
    else
        tCategoriesView.ForceUpdate(RESET_SELECTION);
}

inline void PlayerInventoryScreen::ShowCosmeticCategories()
{
    m_eInventoryState = BROWSE_SUBCATEGORY_STATE;
    m_eCurrentCategory = CATEGORY_COSMETICS;
    PopulateCosmeticCategories(tCategoriesView);
    if (GetPrimaryView() != &tCategoriesView)
        SetPrimaryView(&tCategoriesView);
    else
        tCategoriesView.ForceUpdate(RESET_SELECTION);
}






inline void PlayerInventoryScreen::ToggleStatRangeDisplay()
{
    tDetailsView.ToggleStatRangeDisplay();
    tEquippedView.ToggleStatRangeDisplay();
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_TOGGLE_STATS), cgtPSODefaults);
}

void PlayerInventoryScreen::SetGemToSocket(const ACDID idGem)
{
    m_eInventoryState = BROWSE_ITEMS_TO_SOCKET_STATE;
    tSocketView.SetJewel(idGem);

    UIFilterType eFilter = FILTER_AVAILABLE_SOCKETS;
    if (idGem != ACDID_INVALID)
    {
        ActorCommonData * ptACDGem = ACDTryToGet(idGem);
        if (ptACDGem && ItemIsUpgradeableJewel(*ptACDGem))
            eFilter = FILTER_CAN_SOCKET_UPGRADEABLE_JEWEL;
    }

    tItemsProvider.SetFilter(eFilter);
    tItemsProvider.SetItemOptions((1 << InventoryItems::INCLUDE_EQUIPPED_ITEMS) | (1 << InventoryItems::EXCLUDE_UNIDENTIFIED_ITEMS));
    tItemsProvider.Refresh();
    GetPrimaryView()->ForceUpdate(RESET_SELECTION);
    if (tItemsProvider.GetItems().size() == 0)
    {
        SetBrowsingMode(BROWSE_DETAILS);
    }
    else
    {
        SetBrowsingMode(BROWSE_LIST);
    }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerInventoryScreen::CancelSocketing()
{
    m_eInventoryState = BROWSE_SUBCATEGORY_STATE;
    tSocketView.SetJewel(ACDID_INVALID);
    tSocketView.SelectItem(ACDID_INVALID);
    tSocketView.Reset();
    tItemsProvider.SetItemOptions(0);
    tItemsProvider.Refresh();
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    SetFocusedView(GetPrimaryView());
    GetPrimaryView()->ForceUpdate(RESET_SELECTION);
    SetFocusedView(GetPrimaryView());
    SetBrowsingMode(BROWSE_LIST);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerInventoryScreen::SocketItem()
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer == NULL)
        return;

    ACDID idGem = tSocketView.GetJewel();
    ACDID idItem = tSocketView.GetSelectedItem();
    ActorCommonData * ptACDGem = ACDTryToGet(idGem);
    ActorCommonData * ptACDItem = ACDTryToGet(idItem);
    if (ptACDGem == NULL || ptACDItem == NULL)
        return;

    if (ACDIsPlayer(ptPlayer->idPlayerACD) && ItemHasSockets(*ptACDItem))
    {
        SocketReason eReason = ItemCanSocketInto(*ptACDGem, *ptACDItem);
        switch (eReason)
        {
        case SOCKET_REASON_SUCCESS:
            tSocketView.SocketItem();
            SetFocusedView(GetSecondaryView());
            PlayItemInventorySound(*ptACDGem);
            CACDInventoryRequestToSocket(*ptACDGem, *ptACDItem, ptPlayer);
            m_eInventoryState = BUSY_SOCKETING_STATE;
            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
            break;
        case SOCKET_REASON_LEVEL_RESTRICTION_NOT_MET:
        {
            GameErrorData tError;
            tError.eError = GAMEERROR_IARESULT_PLAYER_DOESNT_MEET_ITEM_LEVEL;
            tError.dwVal = 0;
            UIGame::DisplayError(tError, FALSE, ptPlayer);
            break;
        }
        }
    }
}

inline BOOL PlayerInventoryScreen::ShouldShowSocketingUI()
{
    return m_eInventoryState == BROWSE_ITEMS_TO_SOCKET_STATE || m_eInventoryState == BUSY_SOCKETING_STATE ||
        m_eInventoryState == FINISHED_SOCKETING_STATE || m_eInventoryState == READY_TO_SOCKET_AGAIN_STATE ||
        m_eInventoryState == BROWSE_GEMS_STATE;
}



































































void PlayerInventoryScreen::BackUpCosmeticsFlow()
{
    if (m_eInventoryState == BROWSE_COSMETICS_STATE || m_eInventoryState == BROWSE_PORTRAIT_FRAMES_STATE)
    {
        m_eInventoryState = BROWSE_SUBCATEGORY_STATE;
        m_eCurrentCategory = CATEGORY_COSMETICS;
    }
    else
    {
        ShowTopLevelCategories();
    }

    tItemsProvider.SetItemOptions(0);
    tItemsProvider.Refresh();
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    GetPrimaryView()->ForceUpdate(RESET_SELECTION);
    SetFocusedView(GetPrimaryView());
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

inline BOOL PlayerInventoryScreen::ShowingBagUI()
{
    return GetFilter() == FILTER_BAG &&
        (m_eInventoryState == BROWSE_CATEGORY_STATE || m_eInventoryState == BROWSE_SUBCATEGORY_STATE || m_eInventoryState == BROWSE_ITEMS_STATE ||
         m_eInventoryState == BROWSE_COSMETICS_STATE || m_eInventoryState == BROWSE_PORTRAIT_FRAMES_STATE || m_eInventoryState == BROWSE_CRAFTING_MATS_STATE);
}

void PlayerInventoryScreen::SyncItems()
{
    ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
    if (idSelectedItem != m_idLastSelectedItem)
    {
        tSyncedDetailsProvider.SetItem(idSelectedItem);
        tBagDetailsView.RequestUpdate(RESET_SELECTION);
        m_idLastSelectedItem = idSelectedItem;
    }
}

BOOL PlayerInventoryScreen::ShouldShowDyeingUI()
{
    ACDID idSelectedItem = GetSelectedItem();
    if (idSelectedItem != ACDID_INVALID)
    {
        ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
        if (ptACDItem && !ItemIsJunk(*ptACDItem) && ItemIsDye(*ptACDItem))
            return TRUE;
    }

    return m_eInventoryState == BROWSE_ITEMS_TO_DYE_STATE || m_eInventoryState == BUSY_DYEING_STATE || m_eInventoryState == FINISHED_DYEING_STATE;
}

void PlayerInventoryScreen::BackUpSocketingFlow()
{
    switch (m_eInventoryState)
    {
    case BROWSE_GEMS_STATE:
        m_eInventoryState = BROWSE_SUBCATEGORY_STATE;
        tSocketView.SetJewel(ACDID_INVALID);
        m_eCurrentCategory = CATEGORY_GEMS;
        break;
    case BROWSE_ITEMS_TO_SOCKET_STATE:
    case FINISHED_SOCKETING_STATE:
    case READY_TO_SOCKET_AGAIN_STATE:
    {
        SetBrowsingMode(BROWSE_LIST);
        if (m_eInventoryState == FINISHED_SOCKETING_STATE || m_eInventoryState == READY_TO_SOCKET_AGAIN_STATE)
        {
            tSocketView.SetJewel(ACDID_INVALID);
            tSocketView.Reset();
        }
        tSocketView.SelectItem(ACDID_INVALID);
        ItemCategory eCategory = (ItemCategory)tCategoriesView.GetSelectedItem();
        if (CategoryHasItems(eCategory, tItemsProvider, FALSE))
        {
            tItemsProvider.SetFilter(GetFilterTypeForCategory(eCategory, FALSE));
            m_eInventoryState = BROWSE_GEMS_STATE;
            m_eCurrentCategory = CATEGORY_GEMS;
        }
        else if (CategoryHasItems(CATEGORY_GEMS, tItemsProvider, FALSE))
        {
            ShowGemCategories();
        }
        else
        {
            ShowTopLevelCategories();
        }
        tItemsProvider.SetItemOptions(0);
        tItemsProvider.Refresh();
        break;
    }
    default:
        return;
    }

    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    GetPrimaryView()->ForceUpdate(RESET_SELECTION);
    SetFocusedView(GetPrimaryView());
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerInventoryScreen::SetDyeToUse(const ACDID idDye)
{
    SetBrowsingMode(BROWSE_LIST);
    m_eInventoryState = BROWSE_ITEMS_TO_DYE_STATE;
    tDyeView.SetDye(idDye);
    tItemsProvider.SetFilter(FILTER_DYEABLE);
    tItemsProvider.SetItemOptions((1 << InventoryItems::INCLUDE_EQUIPPED_ITEMS) | (1 << InventoryItems::EXCLUDE_UNIDENTIFIED_ITEMS));
    tItemsProvider.Refresh();
    GetPrimaryView()->ForceUpdate(RESET_SELECTION);
    if (tItemsProvider.GetItems().size() == 0)
    {
        SetBrowsingMode(BROWSE_DETAILS);
    }
    else
    {
        SetBrowsingMode(BROWSE_LIST);
    }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerInventoryScreen::BackUpDyeingFlow()
{
    switch (m_eInventoryState)
    {
    case BROWSE_ITEMS_STATE:
        tDyeView.SetDye(ACDID_INVALID);
        CloseCategory();
        break;
    case BROWSE_ITEMS_TO_DYE_STATE:
    case FINISHED_DYEING_STATE:
        SetBrowsingMode(BROWSE_LIST);
        m_eInventoryState = BROWSE_ITEMS_STATE;
        tDyeView.Reset();
        tDyeView.SelectItem(ACDID_INVALID);
        if (m_eInventoryState == FINISHED_DYEING_STATE)
            tDyeView.SetDye(ACDID_INVALID);
        tItemsProvider.SetFilter(FILTER_MISCELLANEOUS_ITEMS);
        tItemsProvider.SetItemOptions(0);
        tItemsProvider.Refresh();
        break;
    default:
        return;
    }

    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    GetPrimaryView()->ForceUpdate(RESET_SELECTION);
    SetFocusedView(GetPrimaryView());
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerInventoryScreen::CancelDyeing()
{
    m_eInventoryState = BROWSE_ITEMS_STATE;
    tDyeView.SetDye(ACDID_INVALID);
    tDyeView.SelectItem(ACDID_INVALID);
    tItemsProvider.SetItemOptions(0);
    tItemsProvider.Refresh();
    SetFocusedView(GetPrimaryView());
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}


void PlayerInventoryScreen::DyeItem()
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer == NULL)
        return;

    ACDID idDye = tDyeView.GetDye();
    ACDID idItem = tDyeView.GetSelectedItem();
    ActorCommonData * ptACDDye = ACDTryToGet(idDye);
    ActorCommonData * ptACDItem = ACDTryToGet(idItem);
    if (ptACDDye == NULL || ptACDItem == NULL)
        return;

    if (ptACDItem->GetDyeType() == ptACDDye->GetDyeType())
    {
        UIGame::DisplayErrorString(StringListFetch("ConsoleInventory:CannotDyeItem"));
    }
    else if (ACDIsPlayer(ptPlayer->idPlayerACD))
    {
        Target tTarget;
        tTarget.eType = TT_ACTOR;
        tTarget.idACDTarget = ptACDItem->id;

        GameError eError = ItemCanBeUsed(ptPlayer->idPlayerACD, *ptACDDye, &tTarget);
        switch (eError)
        {
        case GAMEERROR_ITEMUSE_UNKNOWN:
            break;
        case GAMEERROR_NONE:
            tDyeView.StartDyeing();
            SetFocusedView(GetSecondaryView());
            CACDInventoryRequestUse(*ptACDDye, ptACDItem->id, TRUE, ptPlayer);
            m_eInventoryState = BUSY_DYEING_STATE;
            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
            break;
        default:

            UIGame::DisplayError(GameErrorData(eError), FALSE, ptPlayer);
            break;
        }
    }
}

BOOL PlayerInventoryScreen::ShouldShowAddSocketsUI()
{
    ACDID idSelectedItem = GetSelectedItem();
    if (idSelectedItem != ACDID_INVALID)
    {
        ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
        if (ptACDItem && !ItemIsJunk(*ptACDItem) && ItemIsAddSocketsItem(*ptACDItem) && !ItemIsUnidentified(*ptACDItem))
            return TRUE;
    }

    return m_eInventoryState == BROWSE_ITEMS_TO_ADD_SOCKETS_TO_STATE || m_eInventoryState == BUSY_ADDING_SOCKETS_STATE || m_eInventoryState == FINISHED_ADDING_SOCKETS_STATE;
}

void PlayerInventoryScreen::SetAddSocketsItemToUse(const ACDID idAddSocketsItem)
{
    m_eInventoryState = BROWSE_ITEMS_TO_ADD_SOCKETS_TO_STATE;
    tAddSocketsView.SetAddSocketsItem(idAddSocketsItem);
    tAddSocketsView.SetAddSocketsRecipient(ACDID_INVALID);
    tItemsProvider.SetFilter(FILTER_CAN_ADD_SOCKETS_WEAPONS);
    tItemsProvider.SetItemOptions((1 << InventoryItems::INCLUDE_EQUIPPED_ITEMS) | (1 << InventoryItems::EXCLUDE_UNIDENTIFIED_ITEMS));
    tItemsProvider.Refresh();
    GetPrimaryView()->ForceUpdate(RESET_SELECTION);
    if (tItemsProvider.GetItems().size() == 0)
    {
        SetBrowsingMode(BROWSE_DETAILS);
    }
    else
    {
        SetBrowsingMode(BROWSE_LIST);
    }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerInventoryScreen::BackUpAddSocketsFlow()
{
    switch (m_eInventoryState)
    {
    case BROWSE_ITEMS_STATE:
        tAddSocketsView.SetAddSocketsItem(ACDID_INVALID);
        tAddSocketsView.SetAddSocketsRecipient(ACDID_INVALID);
        CloseCategory();
        break;
    case BROWSE_ITEMS_TO_ADD_SOCKETS_TO_STATE:
    case FINISHED_ADDING_SOCKETS_STATE:
        SetBrowsingMode(BROWSE_LIST);
        m_eInventoryState = BROWSE_ITEMS_STATE;
        tAddSocketsView.Reset();
        tAddSocketsView.SelectItem(ACDID_INVALID);
        if (m_eInventoryState == FINISHED_ADDING_SOCKETS_STATE)
        {
            tAddSocketsView.SetAddSocketsItem(ACDID_INVALID);
            tAddSocketsView.SetAddSocketsRecipient(ACDID_INVALID);
        }
        tItemsProvider.SetFilter(FILTER_MISCELLANEOUS_ITEMS);
        tItemsProvider.SetItemOptions(0);
        tItemsProvider.Refresh();
        break;
    default:
        return;
    }

    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    GetPrimaryView()->ForceUpdate(RESET_SELECTION);
    SetFocusedView(GetPrimaryView());
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerInventoryScreen::CancelAddSockets()
{
    m_eInventoryState = BROWSE_ITEMS_STATE;
    tAddSocketsView.SetAddSocketsItem(ACDID_INVALID);
    tAddSocketsView.SelectItem(ACDID_INVALID);
    tItemsProvider.SetItemOptions(0);
    tItemsProvider.Refresh();
    SetFocusedView(GetPrimaryView());
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}


void PlayerInventoryScreen::AddSocketsToItem()
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer == NULL)
        return;

    ACDID idItemToUse = tAddSocketsView.GetAddSocketsItem();
    ACDID idItem = tAddSocketsView.GetSelectedItem();
    ActorCommonData * ptACDItemToUse = ACDTryToGet(idItemToUse);
    ActorCommonData * ptACDItem = ACDTryToGet(idItem);
    if (ptACDItemToUse == NULL || ptACDItem == NULL)
        return;

    if (ItemHasSockets(*ptACDItem))
    {
        UIGame::DisplayErrorString(StringListFetch("Errors:CannotSocket"));
    }
    else if (ACDIsPlayer(ptPlayer->idPlayerACD))
    {
        Target tTarget;
        tTarget.eType = TT_ACTOR;
        tTarget.idACDTarget = ptACDItem->id;

        GameError eError = ItemCanBeUsed(ptPlayer->idPlayerACD, *ptACDItemToUse, &tTarget);
        switch (eError)
        {
        case GAMEERROR_ITEMUSE_UNKNOWN:
            break;
        case GAMEERROR_NONE:
            tAddSocketsView.StartAddingSockets();
            SetFocusedView(GetSecondaryView());
            CACDInventoryRequestUse(*ptACDItemToUse, ptACDItem->id, TRUE, ptPlayer);
            m_eInventoryState = BUSY_ADDING_SOCKETS_STATE;
            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
            break;
        default:

            UIGame::DisplayError(GameErrorData(eError), FALSE, ptPlayer);
            break;
        }
    }
}























void InitializeForGame()
{
    if (GameIsStartUp())
        return;

    sgptInventoryGlobals = new PlayerInventoryScreen();
}

void DisposeForGame()
{
    if (GameIsStartUp())
        return;

    ::delete sgptInventoryGlobals;
    sgptInventoryGlobals = NULL;
}

void NotifyOfItemPickup(const ActorCommonData & tACDItem, BOOL bEquipped)
{
    if (bEquipped)
    {
        UIEventQueue::Console::DisplayItemEquippedMessageForPlayer(LocalPlayerGet(), tACDItem);
        return;
    }

    const InventoryLocation * ptInvLoc = tACDItem.TryToGetInventoryLocation();
    if (ptInvLoc == NULL || ptInvLoc->eSlot < SLOT_PLAYER_START || ptInvLoc->eSlot >= SLOT_PLAYER_END)
        return;

    if (ItemIsGold(tACDItem))
        return;

    if (ItemIsHealthPotion(tACDItem) && !ItemIsLegendary(tACDItem.hGB.gbid))
        return;

    Player * ptPlayer = PlayerGetByACD(tACDItem.GetInventoryContainerOwner());
    if (ptPlayer == NULL)
        return;

    if (ItemIsCosmeticItem(tACDItem.hGB.gbid))
        ptPlayer->ptNewCosmeticManager->MarkAsNew(tACDItem.hGB.gbid);

    if (ptInvLoc->eSlot >= SLOT_PLAYER_BODYSLOT_START && ptInvLoc->eSlot < SLOT_PLAYER_BODYSLOT_END)
    {
        UIEventQueue::Console::DisplayItemEquippedMessageForPlayer(ptPlayer, tACDItem);
    }
    else
    {
        UIEventQueue::Console::ReportItemPickup(ptPlayer, tACDItem);
        UIFastEquip::Console::ReportItemPickup(ptPlayer, tACDItem);
    }

    if (!ItemIsCraftingReagent(tACDItem) && !ItemIsGem(tACDItem) && !ItemIsDye(tACDItem) &&
        ptInvLoc->eSlot != SLOT_PLAYER_SHARED_STASH && !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        if (ItemIsLegendary(tACDItem.hGB.gbid) || ItemIsSetItem(tACDItem.hGB.gbid))
            ptPlayer->ptNewEliteItemManager->MarkAsNew(tACDItem.id);
        else
            ptPlayer->ptNewItemManager->MarkAsNew(tACDItem.id);
    }
}

void RefreshViews(const MaintainSelectionOptions eSelectionType)
{
    sgptInventoryGlobals->tItemsProvider.Refresh();
    sgptInventoryGlobals->tEquippedView.SelectItem(GetEquippedItem(sgptInventoryGlobals->GetPlayer(), sgptInventoryGlobals->GetFilter(), FALSE));

    if (sgptInventoryGlobals->GetPrimaryView())
        sgptInventoryGlobals->GetPrimaryView()->ForceUpdate(eSelectionType);

    if (sgptInventoryGlobals->GetSecondaryView())
        sgptInventoryGlobals->GetSecondaryView()->ForceUpdate(eSelectionType);

    UpdateRingNewItemIndicators();
}

BOOL RingIncludeControlPredicate(const UIID & idControl)
{
    return idControl != UIWND_INVENTORY_CONSOLE_RING_GLOW;
}

// 2.7.7 Console::Screens function that clears the vector at +0x1010 of its global (unnamed).
extern "C" void sub_710016ab00();

void Close()
{
    if (!sgptInventoryGlobals->IsActive())
        return;

    sub_710016ab00();
    if (!bExitViaTrade)
    {
        CTradeCancel(sgptInventoryGlobals->GetPlayer(), FALSE, TRUE);
    }
    sgptInventoryGlobals->UIInventoryScreen::Close();
}

void OpenForController(int32 nControllerOpening)
{
    PlayerInventoryScreen * ptScreen = sgptInventoryGlobals;
    ptScreen->m_fInTradeSelectMode = FALSE;
    ptScreen->m_fInMailSelectMode = FALSE;
    ptScreen->unk_1390 = FALSE;
    ptScreen->tItemsProvider.SetProviderMode(InventoryItems::PROVIDERMODE_BACKPACK);
    ptScreen->tCosmeticItemsProvider.SetProviderMode(InventoryItems::PROVIDERMODE_BACKPACK);
    sgptInventoryGlobals->UIInventoryScreen::OpenForController(nControllerOpening);
}

void OpenTradeSelectForController(int32 nControllerOpening)
{
    bExitViaTrade = FALSE;
    PlayerInventoryScreen * ptScreen = sgptInventoryGlobals;
    ptScreen->m_fInTradeSelectMode = TRUE;
    ptScreen->m_fInMailSelectMode = FALSE;
    ptScreen->tItemsProvider.SetProviderMode(InventoryItems::PROVIDERMODE_TRADE_SELECT);
    ptScreen->tCosmeticItemsProvider.SetProviderMode(InventoryItems::PROVIDERMODE_TRADE_SELECT);
    sgptInventoryGlobals->UIInventoryScreen::OpenForController(nControllerOpening);
}

// Probably: UIInventory::Console::Open<2.7.7 mode>ForController, the open callback of screen set 32
// (ConsoleScreenManager.cpp): same shape as OpenTradeSelectForController, but sets unk_1390 and the
// new 2.7.7 provider mode 8 (no enumerator yet) and leaves bExitViaTrade alone.
// NON_MATCHING: PlayerInventoryScreen fields 0x1388/0x1390 land at 0x13d0/0x13d8 in our layout (same 0x48 gap as OpenForController)
extern "C" void sub_7100370440(int32 nControllerOpening)
{
    PlayerInventoryScreen * ptScreen = sgptInventoryGlobals;
    ptScreen->unk_1390 = TRUE;
    ptScreen->m_fInTradeSelectMode = FALSE;
    ptScreen->m_fInMailSelectMode = FALSE;
    ptScreen->tItemsProvider.SetProviderMode((InventoryItems::ProviderMode)8);
    ptScreen->tCosmeticItemsProvider.SetProviderMode((InventoryItems::ProviderMode)8);
    sgptInventoryGlobals->UIInventoryScreen::OpenForController(nControllerOpening);
}

inline ACDID GetSelectedItem()
{
    UIInventoryView * ptView = sgptInventoryGlobals->GetPrimaryView();
    if (ptView)
        return ptView->GetSelectedItem();
    return ACDID_INVALID;
}

void PlayerInventoryScreen::EquipItem()
{
    ACDID idSelectedItem = GetSelectedItem();
    if (idSelectedItem == ACDID_INVALID)
        return;

    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (ptACDItem)
    {
        Player * ptPlayer = sgptInventoryGlobals->GetPlayer();
        InventorySlotType eValidSlots[4];
        eValidSlots[0] = GetSlotForFilterType(sgptInventoryGlobals->GetFilter());
        if (eValidSlots[0] != SLOT_NONE)
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer)
            {
                ptPlayer->ptNewItemManager->UnMarkAsNew(idSelectedItem);
                ptPlayer->ptNewEliteItemManager->UnMarkAsNew(idSelectedItem);
                UpdateRingNewItemIndicators();
                int32 nNumSlots = 1;
                QuickMoveToEmptySlot(*ptACDPlayer, *ptACDItem, nNumSlots, eValidSlots, FALSE, ptPlayer);
                AnimateEquippedItemIcon(GetRingUI().GetSelectedControlID());
                PlayItemInventorySound(*ptACDItem);
            }
            CloseSelectedSlot();
        }
    }
}

void UseItem()
{
    ACDID idSelectedItem = GetSelectedItem();
    if (idSelectedItem == ACDID_INVALID)
        return;

    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (!ptACDItem)
        return;

    Player * ptPlayer = sgptInventoryGlobals->GetPlayer();
    GBID gbidItem = ptACDItem->hGB.gbid;
    ItemUseType eItemUseType = ItemGetUseType(*ptACDItem);
    if (ItemUseIsTargettedInInventory(eItemUseType) || ItemUseIsTargettedInWorld(eItemUseType))
        return;

    GameError eError = ItemCanBeUsed(ptPlayer->idPlayerACD, *ptACDItem, NULL);
    if (eError == GAMEERROR_ITEMUSE_UNKNOWN)
        return;

    if (eError == GAMEERROR_NONE)
    {
        if (eItemUseType == ITEM_USE_PLAYER_GIFT)
        {
            UIMail::Console::ConfirmPlayerGiftSend(*ptACDItem, ptPlayer);
            return;
        }

        if (ItemIsCosmeticItem(gbidItem))
        {
            if (!CosmeticItems_KnownByPlayer(*ptPlayer, gbidItem) && !CosmeticItems_IsParagonFrame(ACDGet(ptPlayer->idPlayerACD), ptACDItem->hGB.gbid, COSMETIC_ITEM_FRAME))
            {
                NotificationCreationParams tParams;
                tParams.uszTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "WarningTitle");
                tParams.nControllerIndex = ptPlayer->nControllerIndex;
                tParams.uszMessage = StringListFetch("ConsoleInventory:LearnCosmetic");
                tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sLearnCosmetic>();
                ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
                return;
            }

            IntDataMessage tMessage;
            if (CosmeticItems_IsEquipped(*ptPlayer, gbidItem))
            {
                tMessage.nData = CosmeticItems_GetCosmeticItemTypeFromGBID(gbidItem);
                MessageSendToServer(CMSG_UNEQUIP_COSMETIC_ITEM, &tMessage, sizeof(tMessage), ptPlayer);
            }
            else
            {
                tMessage.nData = gbidItem;
                MessageSendToServer(CMSG_EQUIP_COSMETIC_ITEM, &tMessage, sizeof(tMessage), ptPlayer);
            }
        }
        else
        {
            CACDInventoryRequestUse(*ptACDItem, ACDID_INVALID, TRUE, ptPlayer);
            if (eItemUseType == ITEM_USE_TREASURE_BAG)
                ::Console::Screens::CloseScreenSet(TRUE, FALSE);
        }
    }
    else
    {
        GameErrorData tError;
        tError.eError = eError;
        tError.dwVal = 0;
        UIGame::DisplayError(tError, FALSE, ptPlayer);
    }
}

void PlayerInventoryScreen::EquipPotion()
{
    ACDID idSelectedItem = GetSelectedItem();
    if (idSelectedItem == ACDID_INVALID)
        return;

    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (ptACDItem && ItemGetUseType(*ptACDItem) == ITEM_USE_HEALTHPOTION)
    {
        Player * ptPlayer = sgptInventoryGlobals->GetPlayer();
        HotbarButtonData tButtonData;
        tButtonData.SetItemANN(ANNFromACD(ptACDItem->id));
        PlayerSendPotionSlotChangeToServer(tButtonData, ptPlayer);
        AnimateEquippedItemIcon(GetRingUI().GetSelectedControlID());
        RefreshViews(KEEP_ITEM_SELECTED);
        SoundManager::PlaySound(GlobalSNOGet(ItemIsLegendary(ptACDItem->hGB.gbid) ? GLOBALSNO_SOUND_CONSOLE_EQUIP_LEGENDARY_POTION : GLOBALSNO_SOUND_CONSOLE_EQUIP_POTION), cgtPSODefaults);
    }
}

inline void PlayerInventoryScreen::UnMarkEquippedItemAsNew(const UIFilterType eFilterType)
{
    const ACDIDArray & arItems = tEquippedItemProvider.GetItemsWithFilter(eFilterType);
    if (arItems.size() == 1)
    {
        tEquippedItemProvider.UnMarkAsNew(arItems[0]);
    }
}

void AnimateEquippedItemIcon(const UIID & idSlot)
{
    UIID idStoryboard = UIIDCreateFromParent(idSlot, ".LoadoutEquipItem");
    UIStoryboard * tStoryboard = UI_GetStoryboardFromID(idStoryboard);
    tStoryboard->Stop();
    tStoryboard->Play(FALSE, 0.0f);
}

void UpdateHeroModel()
{
    if (sgptInventoryGlobals && sgptInventoryGlobals->IsActive())
    {
        ::Console::UIRActorManager::ForceRActorRecreate();
    }
}

void sLearnCosmetic(const UIID & id)
{
    ACDID idSelectedItem = GetSelectedItem();
    if (idSelectedItem == ACDID_INVALID)
        return;

    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (!ptACDItem)
        return;

    ItemUseType eUseType = ItemGetUseType(*ptACDItem);
    if (eUseType == ITEM_USE_COLLECTORS_EDITION_BUFF || eUseType == ITEM_USE_LEARN_WINGS || eUseType == ITEM_USE_LEARN_PET ||
        eUseType == ITEM_USE_LEARN_PORTRAIT_FRAME || eUseType == ITEM_USE_LEARN_PENNANT)
    {
        CACDInventoryRequestUse(*ptACDItem, ACDID_INVALID, TRUE, sgptInventoryGlobals->GetPlayer());
        IntDataMessage tMessage;
        tMessage.nData = ptACDItem->hGB.gbid;
        MessageSendToServer(CMSG_EQUIP_COSMETIC_ITEM, &tMessage, sizeof(tMessage), sgptInventoryGlobals->GetPlayer());
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

void PlayerInventoryScreen::OnButtonL3(int32 nControllerIndex)
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_STICK_BUTTON);
    if (idAction == ::Console::ACTION_NONE)
        return;

    if (UIUtils::Console::ShowTieredRiftMessage(GetPlayer()))
        return;

    switch (idAction)
    {
    case ACTION_UNEQUIP:
    {
        ACDID idEquippedItem = GetEquippedItem(sgptInventoryGlobals->GetPlayer(), sgptInventoryGlobals->GetFilter(), FALSE);
        if (idEquippedItem == ACDID_INVALID)
            return;

        ActorCommonData * ptACDPlayer = ACDTryToGet(sgptInventoryGlobals->GetPlayer()->idPlayerACD);
        ActorCommonData * ptACDItem = ACDTryToGet(idEquippedItem);
        TryToQuickMoveToLocation(*ptACDPlayer, *ptACDItem, SLOT_PLAYER_BACKPACK);
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        break;
    }
    case ACTION_SPLIT_STACK:
    {
        ACDID idSelectedItem = GetSelectedItem();
        if (idSelectedItem == ACDID_INVALID)
            return;

        ActorCommonData * ptACDStack = ACDTryToGet(idSelectedItem);
        if (!ptACDStack)
            return;

        uint64 uStackCount = ptACDStack->GetStackCount();
        if (uStackCount < 2)
            return;

        ActorCommonData & tACDContainer = ACDGet(ACDInventoryGetUltimateContainer(*ptACDStack));
        InventoryLocation tInvLocDest = ptACDStack->GetInventoryLocation();
        tInvLocDest.pt.x = 0;
        tInvLocDest.pt.y = 0;
        if (ACDInventoryCanHold(tACDContainer, *ptACDStack, tInvLocDest.eSlot, tInvLocDest.pt, NULL))
        {
            UIStackSplit::Console::Open(ptACDStack->id, uStackCount - 1, uStackCount - 1, sSplitStack, UIStackSplit::Console::STACKSPLIT_GENERIC);
        }
        else
        {
            GameErrorData tError;
            tError.eError = GAMEERROR_IARESULT_NOT_ENOUGH_ROOM;
            tError.dwVal = 0;
            UIGame::DisplayError(tError, FALSE, sgptInventoryGlobals->GetPlayer());
        }
        break;
    }
    }
}

void sSplitStack(const ACDID idACDStack, const uint64 uAmount)
{
    if (idACDStack == ACDID_INVALID)
        return;

    ActorCommonData * ptACDStack = ACDTryToGet(idACDStack);
    if (!ptACDStack)
        return;

    ActorCommonData & tACDContainer = ACDGet(ACDInventoryGetUltimateContainer(*ptACDStack));
    InventoryLocation tInvLocDest = ptACDStack->GetInventoryLocation();
    tInvLocDest.pt.x = 0;
    tInvLocDest.pt.y = 0;
    if (ACDInventoryCanHold(tACDContainer, *ptACDStack, tInvLocDest.eSlot, tInvLocDest.pt, NULL))
    {
        CACDInventoryRequestSplitStack(ptACDStack, tInvLocDest, uAmount);
    }
}

void sDestroyGift(const UIID & id)
{
    ACDID idSelectedItem = GetSelectedItem();
    if (idSelectedItem == ACDID_INVALID)
        return;

    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (ptACDItem && ItemGetUseType(*ptACDItem) == ITEM_USE_PLAYER_GIFT)
    {
        ANNDataMessage tMessage;
        tMessage.ann = ANNFromACD(idSelectedItem);
        MessageSendToServer(CMSG_DESTROY_GIFT, &tMessage, sizeof(tMessage), sgptInventoryGlobals->GetPlayer());
        CACDInventoryDestroy(*ptACDItem);
        sgptInventoryGlobals->RequestUpdate(RESET_SELECTION);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_DESTROYED), cgtPSODefaults);
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

inline void OnInventoryButtonLeftTrigger()
{
    if (sgptInventoryGlobals && sgptInventoryGlobals->IsActive())
    {
        if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_TRIGGER) == ACTION_SCROLL_LIST &&
            !::Console::ControllerUtils::IsButtonDown(sgptInventoryGlobals->GetControllerIndex(), BUTTON_RIGHT_TRIGGER))
        {
            sgptInventoryGlobals->ScrollDetails(-4.0f);
        }
    }
}

inline void OnInventoryButtonRightTrigger()
{
    if (sgptInventoryGlobals && sgptInventoryGlobals->IsActive())
    {
        if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER) == ACTION_SCROLL_LIST &&
            !::Console::ControllerUtils::IsButtonDown(sgptInventoryGlobals->GetControllerIndex(), BUTTON_LEFT_TRIGGER))
        {
            sgptInventoryGlobals->ScrollDetails(4.0f);
        }
    }
}












void OnInventoryHotkey(const UIID &, const int32 nControllerIndex)
{
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (!ptPlayer)
        return;

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer && ptACDPlayer->IsAlive())
    {
        BOOL bCanOpen = !::Console::Screens::IsScreenSetOpen() && !UIBossEncounter::IsActive();
        int32 idScreen = PlayerCanOpenInventory(*ptACDPlayer) ? 0 : 1;
        if (bCanOpen)
            ::Console::Screens::OpenToScreen(::Console::Screens::GAMEPLAY_SCREEN_SET, idScreen, nControllerIndex, FALSE);
    }
}

void OnInventoryLeftStick(const UIID &, const int32 nPlayerIndex)
{
    if (sgptInventoryGlobals && sgptInventoryGlobals->IsActive())
    {
        sgptInventoryGlobals->HandleLeftStickInput(nPlayerIndex);
    }
}

void OnInventoryRightStick(const UIID &, const int32 nPlayerIndex)
{
    if (sgptInventoryGlobals && sgptInventoryGlobals->IsActive())
    {
        sgptInventoryGlobals->HandleRightStickInput(nPlayerIndex);
    }
}

void OnInventoryButton(const UIID &, const int32 nControllerIndex, const InputButton eInputButton)
{
    if (sgptInventoryGlobals && sgptInventoryGlobals->IsActive())
    {
        sgptInventoryGlobals->HandleButtonInput(nControllerIndex, eInputButton);
    }
}

inline void UpdateInventoryItems()
{
    UpdatePlayerInfoUI(*sgptInventoryGlobals->GetPlayer(), UIWND_INVENTORY_CONSOLE_ROOT, sgptInventoryGlobals->tVisualInventory);
    RefreshViews(KEEP_ITEM_SELECTED);
    if (sgptInventoryGlobals)
        sgptInventoryGlobals->UpdateRingIcons();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void OnInventoryUpdate()
{
    if (sgptInventoryGlobals && sgptInventoryGlobals->IsActive())
    {
        sgptInventoryGlobals->Update();
        UpdateRingNewItemIndicators();
    }
}

void OnInventoryChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bCosmeticChanged, BOOL bForce)
{
    if (sgptInventoryGlobals && sgptInventoryGlobals->IsActive())
    {
        if (bForce || tOldLoc.eActorLocation != tNewLoc.eActorLocation || tNewLoc.tInvLocation.eSlot != tOldLoc.tInvLocation.eSlot)
        {
            Player * ptPlayer = sgptInventoryGlobals->GetPlayer();
            if (!ptPlayer || tOldLoc.tInvLocation.idACDContainer == ptPlayer->idPlayerACD || tNewLoc.tInvLocation.idACDContainer == ptPlayer->idPlayerACD)
            {
                if (bCosmeticChanged ||
                    (tOldLoc.eActorLocation == AL_INVENTORY && VisualInventorySlotFromInventorySlot(tOldLoc.tInvLocation.eSlot) != VIS_INVALID) ||
                    (tNewLoc.eActorLocation == AL_INVENTORY && VisualInventorySlotFromInventorySlot(tNewLoc.tInvLocation.eSlot) != VIS_INVALID))
                {
                    ::Console::UIRActorManager::ForceRActorRecreate();
                }
                sgptInventoryGlobals->RequestUpdate(KEEP_ITEM_SELECTED);
            }
        }
    }
}

void OnCraftingMaterialsChanged()
{
    if (sgptInventoryGlobals && sgptInventoryGlobals->IsActive())
    {
        sgptInventoryGlobals->RequestUpdate(1);
    }
}

void EnableTabNavigation(const BOOL bEnable)
{
    if (sbTabNavigationEnabled == bEnable)
        return;

    sbTabNavigationEnabled = bEnable;
    ::Console::Screens::EnableScreen(::Console::Screens::GAMEPLAY_SCREEN_SET, 0, bEnable);
    ::Console::Screens::EnableScreen(::Console::Screens::GAMEPLAY_SCREEN_SET, 1, bEnable);
    ::Console::Screens::EnableScreen(::Console::Screens::GAMEPLAY_SCREEN_SET, 2, bEnable);
    ::Console::Screens::EnableScreen(::Console::Screens::GAMEPLAY_SCREEN_SET, 3, bEnable);
    ::Console::Screens::EnableScreen(::Console::Screens::GAMEPLAY_SCREEN_SET, 4, bEnable);
    ::Console::Screens::EnableScreen(::Console::Screens::GAMEPLAY_SCREEN_SET, 5, bEnable);
    UIID idNavBar = UIIDCreateFromParent(UIDLG_INVENTORY_CONSOLE, ".LayoutRoot.Navigation.InventoryNav");
    UI_GetWindowFromID(UIIDCreateFromParent(idNavBar, ".KeyBinding1")).SetActive(bEnable);
    UI_GetWindowFromID(UIIDCreateFromParent(idNavBar, ".KeyBinding2")).SetActive(bEnable);
}

}  // namespace Console
}  // namespace UIInventory

#include "UIConsoleInventoryViewUtilities.h"
// X/Client/UserInterface/UIIDs.h: original OnOpen data references.
extern const UIID UITXT_CONSOLE_BUTTON_LEGEND_TEXT, UITXT_INVENTORY_CONSOLE_GOLD;
namespace UIInventory { namespace Console {
// DWARF: UIConsoleInventoryViewUtilities.cpp:1949.
void SwitchToInfoPanel(const InfoPanelType eInfoPanel);
#line 2852 "X/Client/UserInterface/Console/UIInventoryConsole.cpp"
// Original standalone body was linker-removed (DWARF low_pc 0); not independently checkable. Needed by OnOpen.
BOOL PlayerInventoryScreen::RestoreCachedRingSelection()

{
    if (m_eCachedRingSelection != FILTER_NONE)
    {
        SetFilterType(m_eCachedRingSelection);
        m_eCachedRingSelection = FILTER_NONE;
        // Inference: the consumed cached selection takes the successful branch in OnOpen.
        return TRUE;
    }
    // Inference: an absent cached selection makes OnOpen choose its default filter.
    return FALSE;
}
#line 759 "X/Client/UserInterface/Console/UIInventoryConsole.cpp"
void PlayerInventoryScreen::OnOpen()

{
    Player * ptPlayer = GetPlayer();

    tItemsProvider.SetPlayer(ptPlayer);
    tEquippedItemProvider.SetPlayer(ptPlayer);
    tSyncedDetailsProvider.SetPlayer(ptPlayer);
    tCosmeticItemsProvider.SetPlayer(ptPlayer);
    tCraftingMatItemsProvider.SetPlayer(ptPlayer);

    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(&tEquippedView);


    SetFocusedView(NULL);

    SwitchToInfoPanel(CHARACTER_INFO_PANEL);

    UpdateRingIcons();
    UpdateRingNewItemIndicators();
    UpdateMainhandAndOffhandRingIcons(ptPlayer);
    // Inference: the helper success skips default selection; its return is folded into the cached-filter branch.
    if (!RestoreCachedRingSelection())
    {
        SetFilterType(GetInitialRingSelection(FILTER_PLAYER_START, FILTER_PLAYER_END));
    }

    UpdateEquippedPrompt();

    CloseSelectedSlot();

    ::Console::UIRActorManager::SetActive(TRUE);
    UpdatePlayerInfoUI(*ptPlayer, UIWND_INVENTORY_CONSOLE_ROOT, tVisualInventory);


    gtTutorialMgr.CloseTutorialIfOpen(GlobalSNOGet(GLOBALSNO_TUTORIAL_FIRST_MAGIC_ITEM));


    UITutorialManager::OnButtonClose(GetUIIDNone(), -1);


    UITutorial * ptTutorial = gtTutorialMgr.ShowTutorial(UITXT_CONSOLE_BUTTON_LEGEND_TEXT, GlobalSNOGet(GLOBALSNO_TUTORIAL_INVENTORY_NAVIGATION), ptPlayer, TRUE);


    if (!ptTutorial)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer && ptACDPlayer->GetCurrencyAmount(CURRENCY_BLOODSHARDS) > 0)
        {
            gtTutorialMgr.ShowTutorial(UITXT_INVENTORY_CONSOLE_GOLD, GlobalSNOGet(GLOBALSNO_TUTORIAL_CURRENCY_INVENTORY), ptPlayer, TRUE);
        }
    }
}
} // namespace Console
} // namespace UIInventory

// DWARF: UIConsoleTrade.cpp:1262.
namespace UITrade { namespace Console { BOOL OfferItem(const ACDID idACDItem); } }
namespace UIInventory { namespace Console {
#line 1987 "X/Client/UserInterface/Console/UIInventoryConsole.cpp"
// Standalone original linker-removed (DWARF low_pc 0); not independently checkable. Needed by OnButton1.
void PlayerInventoryScreen::OpenSelectedCategory()

{
    OpenCategory(tCategoriesView.GetSelectedItem());
}
#line 3501 "X/Client/UserInterface/Console/UIInventoryConsole.cpp"
// Standalone original linker-removed (DWARF low_pc 0); not independently checkable. Needed by OnButton1 callback.
void sConfirmAddingSockets(
    const UIID & id)
{


    if (sgptInventoryGlobals)
    {
        sgptInventoryGlobals->AddSocketsToItem();
    }
}
#line 1632 "X/Client/UserInterface/Console/UIInventoryConsole.cpp"
void PlayerInventoryScreen::OnButton1(
    int32 nControllerIndex)
{



    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_1);
    // Inference: the two navigation actions bypass the tiered-rift warning, as shown by their early branches.
    if (idAction == ::Console::ACTION_NONE) return;
    if (idAction != ACTION_OPEN_SLOT && idAction != ACTION_OPEN_CATEGORY)
        if (UIUtils::Console::ShowTieredRiftMessage(GetPlayer()))
            return;




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

    case ACTION_SELECT_TRADE_ITEM:
        if (UITrade::Console::OfferItem(GetSelectedItem()))
        {

            bExitViaTrade = TRUE;
            ::Console::Screens::OpenToScreen(::Console::Screens::TRADE_SCREEN_SET, 0, nControllerIndex, FALSE);
        }
        break;

    case ACTION_EQUIP:
        EquipItem();
        break;

    case ACTION_USE:
        UseItem();
        break;

    case ACTION_EQUIP_POTION:
        EquipPotion();
        break;

    case ACTION_BEGIN_SOCKET:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        SetGemToSocket(GetSelectedItem());
        break;

    case ACTION_END_SOCKET:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        SocketItem();
        break;

    case ACTION_ACCEPT_SOCKETING:
        BackUpSocketingFlow();
        break;

    case ACTION_BEGIN_DYE:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        SetDyeToUse(GetSelectedItem());
        break;

    case ACTION_END_DYE:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        DyeItem();
        break;

    case ACTION_ACCEPT_DYEING:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        BackUpDyeingFlow();
        break;

    case ACTION_BEGIN_ADDING_SOCKETS:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        SetAddSocketsItemToUse(GetSelectedItem());
        break;

    case ACTION_END_ADDING_SOCKETS:

        if (tAddSocketsView.GetAddSocketsRecipient() != ACDID_INVALID)
        {
            NotificationCreationParams tParams;
            tParams.uszTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "WarningTitle");
            tParams.uszMessage = StringListFetch("Inventory:AddSocketsConfirmation");
            tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sConfirmAddingSockets>();
            tParams.nControllerIndex = GetPlayer()->nControllerIndex;
            ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
        }
        break;


    case ACTION_ACCEPT_ADDING_SOCKETS:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        BackUpAddSocketsFlow();
        break;
    }
}
} // namespace Console
} // namespace UIInventory

namespace UIInventory { namespace Console {
#line 2178 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
// Original standalone body was discarded (DIE021d3260 low_pc0); body is not checkable independently.
BOOL PlayerInventoryScreen::CanToggleStatRangeDisplay()
{

    // Inference: the original inlined short circuit checks details before equipped-view activity.
    return tDetailsView.IsActive() || tEquippedView.IsActive();
}
#line 1304 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
void PlayerInventoryScreen::UpdateNormalItemActions(
    ::Console::ControllerActionManager & tActionManager)
{
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());

    if (CanScrollDetails())
    {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
    }

    ACDID idSelectedItem = GetSelectedItem();
    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (GetPlayer())
    {
        // Inference: the original passes zero as GetEquippedItem's third argument.
        ACDID idEquippedItem = GetEquippedItem(GetPlayer(), GetFilter(), 0);
        if (idEquippedItem != ACDID_INVALID)
        {
            if (!GameIsWeeklyChallenge())
            {

                InputRemapSingleJoyconButton(BUTTON_DPAD_DOWN, BUTTON_LEFT_STICK_BUTTON);

                tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_STICK_BUTTON, ACTION_UNEQUIP, StringListFetch("ConsoleInventory:Unequip").str());
            }


            if (sgptInventoryGlobals->CanToggleStatRangeDisplay())
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_DPAD_LEFT, ACTION_TOGGLE_STAT_RANGES, StringListFetch("ConsoleUI:ToggleStatRanges").str());
            }
        }


        ActorCommonData * ptACDPlayer = ACDTryToGet(GetPlayer()->idPlayerACD);
        if (ptACDItem && ptACDPlayer)
        {
            UIItemOverlay eOverlay = ItemGetOverlay(*ptACDItem, *ptACDPlayer);
            if (ItemIsEquippable(*ptACDItem) && eOverlay == ITEMOVERLAY_NONE)
            {

                InputRemapSingleJoyconButton(BUTTON_DPAD_UP, BUTTON_1);


                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_EQUIP, StringListFetch("ConsoleInventory:Equip").str());
            }
            else if (ItemIsUsable(*ptACDItem))
            {
                if (ItemGetUseType(*ptACDItem) == ITEM_USE_PLAYER_GIFT)
                {
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_USE, StringListFetch("X1_ConsoleMail:Send").str());
                }
                else if (ItemGetUseType(*ptACDItem) == ITEM_USE_HEALTHPOTION)
                {
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_EQUIP_POTION, StringListFetch("ConsoleInventory:Equip").str());
                }
                else
                {
                    if (ItemGetUseType(*ptACDItem) != ITEM_USE_ADD_SOCKETS || !ItemIsUnidentified(*ptACDItem))
                    {
                        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_USE, StringListFetch("ConsoleInventory:Use").str());
                    }
                }
            }

            if (!GameIsWeeklyChallenge())
            {
                if (ItemIsJunk(*ptACDItem))
                {

                    InputRemapSingleJoyconButton(BUTTON_DPAD_RIGHT, BUTTON_RIGHT_STICK_BUTTON);

                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON, ACTION_MARK_AS_JUNK, StringListFetch("ConsoleInventory:NotJunk").str());
                }
                else if (ItemCanBeMarkedAsJunk(*ptACDItem))
                {

                    InputRemapSingleJoyconButton(BUTTON_DPAD_RIGHT, BUTTON_RIGHT_STICK_BUTTON);

                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON, ACTION_MARK_AS_JUNK, StringListFetch("ConsoleInventory:Junk").str());
                }
            }
        }

        if (ptACDItem)
        {
            if (ItemIsUnidentified(*ptACDItem))
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_IDENTIFY, StringListFetch("ConsoleInventory:Identify").str());
            }
            else
            {
                if (GetFilter() != FILTER_BAG)
                {

                    if (GetBrowsingMode() == BROWSE_LIST)
                    {
                        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Compare").str());
                    }
                    else
                    {
                        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List").str());
                    }
                }


                if (sgptInventoryGlobals->CanToggleStatRangeDisplay())
                {
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_DPAD_LEFT, ACTION_TOGGLE_STAT_RANGES, StringListFetch("ConsoleUI:ToggleStatRanges").str());
                }
            }

            if (ItemGetUseType(*ptACDItem) == ITEM_USE_PLAYER_GIFT)
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_DESTROY, StringListFetch("ConsoleInventory:Destroy").str());
            }
            else if (!GameIsWeeklyChallenge())
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_DROP, StringListFetch("ConsoleInventory:Drop").str());
            }

            uint64 uStackSize = ptACDItem->GetStackCount();
            if (uStackSize > 1)
            {

                InputRemapSingleJoyconButton(BUTTON_DPAD_DOWN, BUTTON_LEFT_STICK_BUTTON);

                tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_STICK_BUTTON, ACTION_SPLIT_STACK, StringListFetch("ConsoleInventory:SplitStack").str());
            }
        }
    }
}
} // namespace Console
} // namespace UIInventory

namespace UIInventory { namespace Console {
#line 3547 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
// Original standalone body was discarded (DIE021a4c01 low_pc0); body is not checkable independently.
void OnInventoryDPadLeft()
{

    if (sgptInventoryGlobals && sgptInventoryGlobals->IsActive())
    {
        if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_DPAD_LEFT) == ACTION_TOGGLE_STAT_RANGES)
        {
            sgptInventoryGlobals->ToggleStatRangeDisplay();
        }
    }
}
} // namespace Console
} // namespace UIInventory

namespace UIInventory {
namespace Console {
#line 1233 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
void PlayerInventoryScreen::UpdateBagItemActions(
    ::Console::ControllerActionManager & tActionManager)
{
        switch (m_eInventoryState)
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
        case BROWSE_COSMETICS_STATE:
        case BROWSE_PORTRAIT_FRAMES_STATE:
            {
                Player * ptPlayer = GetPlayer();

                ACDID idSelectedItem = GetSelectedItem();
                ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);

                if (ptPlayer && ptACDItem)
                {
                    if (CosmeticItems_IsEquipped(*ptPlayer, ptACDItem->hGB.gbid))
                    {
                        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_USE, StringListFetch("ConsoleInventory:Unequip").str());
                    }
                    else
                    {
                        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_USE, StringListFetch("ConsoleInventory:Equip").str());
                    }

                    if (tCosmeticItemsProvider.VirtualItemIndex(idSelectedItem) == -1 && ItemCanDrop(*ptACDItem) == GAMEERROR_NONE)
                    {
                        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_DROP, StringListFetch("ConsoleInventory:Drop").str());
                    }
                }
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
            }
            break;

        case BROWSE_CRAFTING_MATS_STATE:
            {
                Player * ptPlayer = GetPlayer();

                ACDID idSelectedItem = GetSelectedItem();
                ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);

                if (ptPlayer && ptACDItem && !GameIsSeasonal())
                {
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_DROP, StringListFetch("ConsoleInventory:Drop").str());
                }
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
            }
            break;

        case BROWSE_ITEMS_STATE:

                UpdateNormalItemActions(tActionManager);
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
                break;
        }
}
#line 1529 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
// Original standalone body was discarded (concrete DIE 021d320e at low_pc 0); only its inlined copies are checkable.
BOOL PlayerInventoryScreen::CanOpenSelectedCategory()
{

    ItemCategory eCategory = (ItemCategory)tCategoriesView.GetSelectedItem();
    BOOL bHasItems = CategoryHasItems(eCategory, tItemsProvider, 0);


    bHasItems |= (CategoryHasCosmeticItems(eCategory, tItemsProvider, tCosmeticItemsProvider) && !GameIsWeeklyChallenge());
    bHasItems |= CategoryHasCraftingMatItems(eCategory, tItemsProvider, tCraftingMatItemsProvider);
    return bHasItems;
}
} // namespace Console
} // namespace UIInventory

namespace {
#line 3746 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
void OnUpdateCategoryUI(
    const UIID & idItemUI,
    const UIInventory::Console::ItemCategory eCategory)
{


    if (UIInventory::Console::sgptInventoryGlobals && UIInventory::Console::sgptInventoryGlobals->IsActive())
    {
        UIInventory::Console::sgptInventoryGlobals->UpdateCategoryUI(idItemUI, eCategory);
    }
}
} // namespace

namespace UIInventory {
namespace Console {
#line 206 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
PlayerInventoryScreen::PlayerInventoryScreen()

    : tItemsProvider(InventoryItems::PROVIDERMODE_BACKPACK)
    , tListView(tItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, FALSE)
    , tCosmeticListView(tCosmeticItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, FALSE)
    , tCraftingMatListView(tCraftingMatItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, FALSE)
    , tDetailsView(tItemsProvider, UIDLG_INVENTORY_CONSOLE_DETAILS, 1)
    , tEquippedView(tEquippedItemProvider, UIDLG_INVENTORY_CONSOLE_EQUIPPED, 0)
    , tBagDetailsView(tSyncedDetailsProvider, UIDLG_INVENTORY_CONSOLE_EQUIPPED, 0)
    , tSocketView(UIDLG_INVENTORY_CONSOLE_SOCKETING)
    , tDyeView(UIDLG_INVENTORY_CONSOLE_DYEING)
    , tAddSocketsView(UIDLG_INVENTORY_CONSOLE_ADD_SOCKETS)
    , tCategoriesView(UIDLG_INVENTORY_CONSOLE_LIST, OnUpdateCategoryUI)
    , tCategorySummaryView(UIDLG_INVENTORY_CONSOLE_EQUIPPED)
    , tPortraitFrameSummaryView(UIDLG_INVENTORY_CONSOLE_EQUIPPED)
    , m_fInTradeSelectMode(FALSE)
    , m_fInMailSelectMode(FALSE)
    , m_eInventoryState(BROWSE_CATEGORY_STATE)
    , m_eCurrentCategory(CATEGORY_NONE)
    , m_eCachedRingSelection(FILTER_NONE)
    , m_idLastSelectedItem(ACDID_INVALID)
    , m_idLastIdentifyingACD(ACDID_INVALID)

{
    tVisualInventory.Init();

    tListView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    tDetailsView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    tEquippedView.SetHeaderText(StringListFetch("ConsoleInventory:Equipped"));
    tBagDetailsView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    tSocketView.SetHeaderText(StringListFetch("ConsoleSocketing:Socketing"));
    tCategoriesView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    tCategorySummaryView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    tPortraitFrameSummaryView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    tCosmeticListView.SetHeaderText(StringListFetch("ConsoleInventory:CategoryCosmeticsTitle"));
    tCraftingMatListView.SetHeaderText(StringListFetch("ConsoleInventory:CategoryCraftingTitle"));
}
} // namespace Console
} // namespace UIInventory

#include "../Controls/UITextControl.h"
#include "../../../Common/UserInterface/UITextControl.h"
// X/Client/UserInterface/UIIDs.h (defined in X/Client/UserInterface/UIGlobals.cpp)
extern const UIID UITXT_INVENTORY_CONSOLE_HEADER, UITXT_INVENTORY_CONSOLE_GOLD_SPENT, UITXT_INVENTORY_CONSOLE_GOLD_RETURNED, UIWND_INVENTORY_CONSOLE_PANEL_FOCUS_HIGHLIGHT;
namespace UIInventory {
namespace Console {
// X/Client/UserInterface/Console/UIConsoleInventory.cpp
void ShowFilterName(const UIID & idText, const UIFilterType eFilter, const BOOL bHighlight);
#line 884 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
void PlayerInventoryScreen::OnUpdate()

{
    UIFilterType eFilterType = GetFilter();
    if (eFilterType != FILTER_NONE)
    {
        ShowFilterName(UITXT_INVENTORY_CONSOLE_HEADER, GetFilter(), !RingHasFocus());
    }
    else
    {
        UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(StringListFetch("ConsoleInventory:Empty"), FALSE);
    }


    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_PANEL_FOCUS_HIGHLIGHT).SetActive(!RingHasFocus());


    UpdateCurrencyDisplay(UITXT_INVENTORY_CONSOLE_GOLD, UITXT_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UITXT_INVENTORY_CONSOLE_GOLD_RETURNED, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED, -1);







    if (m_idLastIdentifyingACD != ACDID_INVALID && GetPrimaryView()->GetIdentifyingItemACD() == ACDID_INVALID)
    {
        ACDID idIdentifyingACD = m_idLastIdentifyingACD;
        m_idLastIdentifyingACD = ACDID_INVALID;



        if (eFilterType != FILTER_BAG)
        {
            if (idIdentifyingACD != GetPrimaryView()->GetSelectedItem())
            {
                GetPrimaryView()->SelectItem(idIdentifyingACD);
            }
            SetBrowsingMode(BROWSE_DETAILS);
        }
    }
    else if (m_idLastIdentifyingACD == ACDID_INVALID && GetPrimaryView()->GetIdentifyingItemACD() != ACDID_INVALID)
    {
        m_idLastIdentifyingACD = GetPrimaryView()->GetIdentifyingItemACD();
    }


    if (GetPrimaryView()->GetIdentifyingItemACD() == ACDID_INVALID)
    {
        EnableTabNavigation(TRUE);
    }



    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));

    if (m_fInTradeSelectMode || m_fInMailSelectMode)
    {

        if (GetFilter() == FILTER_BAG &&
            (m_eInventoryState == BROWSE_CATEGORY_STATE || m_eInventoryState == BROWSE_SUBCATEGORY_STATE))
        {
            SyncCategories();
        }
        else
        {
            SyncItems();
        }
    }
    else
    {


        if (ShouldShowSocketingUI())
        {
            UpdateSocketingState();
            UpdateSocketingUI();
        }
        else if (ShouldShowDyeingUI())
        {
            UpdateDyeingState();
            UpdateDyeingUI();
        }
        else if (ShouldShowAddSocketsUI())
        {
            UpdateAddSocketsState();
            UpdateAddSocketsUI();
        }
        else if (GetFilter() == FILTER_BAG)
        {

            if (m_eInventoryState == BROWSE_CATEGORY_STATE || m_eInventoryState == BROWSE_SUBCATEGORY_STATE)
            {
                SyncCategories();
            }
            else if (m_eInventoryState == BROWSE_PORTRAIT_FRAMES_STATE)
            {
                tPortraitFrameSummaryView.SetACDID(GetPrimaryView()->GetSelectedItem());
            }
            else
            {
                SyncItems();
            }
        }
    }




    UIID idStatusLine = GetSecondaryView() ? GetSecondaryView()->GetStatusLineUIID() : GetUIIDNone();
    if (idStatusLine != GetUIIDNone())
    {
        CRefString sItemCost;
        ACDID idItem = GetSecondaryView()->GetSelectedItem();
        if (idItem != ACDID_INVALID)
        {
            ActorCommonData * ptACDItem = ACDTryToGet(idItem);
            if (ptACDItem)
            {
                CurrencyType eCurrencyType = CURRENCY_GOLD;

                uint64 uGold = ItemGetCost(*ptACDItem, 0, eCurrencyType, NULL, FALSE, FALSE);
                CRefString sPrice = UIText_ValueToGameCurrency(uGold, eCurrencyType);

                Set_Text_Color(sPrice, FONTCOLOR_WHITE);

                sItemCost = LocalizedSprintf(
                    StringListFetch("ConsoleShop:SellValueFormat").str(),
                    sPrice.str(), NULL);
            }
        }

        UI_GetTextFromID(idStatusLine).SetText(sItemCost, FALSE);
    }



    if (GetPrimaryView())
    {
        GetPrimaryView()->ShowBagSpace(tItemsProvider, StringListFetch("ConsoleInventory:Inventory"));
    }
}
#line 1614 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
// Original standalone body was discarded (concrete DIE at low_pc 0); only its inlined copy is checkable.
void PlayerInventoryScreen::UpdateSocketingUI()
{

    ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();

    switch (m_eInventoryState)
    {
    case BROWSE_GEMS_STATE:
        tSocketView.SetJewel(idSelectedItem);
        break;

    case BROWSE_ITEMS_TO_SOCKET_STATE:
        tSocketView.SelectItem(idSelectedItem);
        break;
    }
}
#line 2396 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
// Original standalone body was discarded (concrete DIE at low_pc 0); only its inlined copy is checkable.
void PlayerInventoryScreen::UpdateDyeingState()
{

    if (m_eInventoryState == BUSY_DYEING_STATE && tDyeView.IsFinishedDyeing())
    {
        tItemsProvider.Refresh();
        GetPrimaryView()->RequestUpdate(RESET_SELECTION);
        m_eInventoryState = FINISHED_DYEING_STATE;
    }
}




void PlayerInventoryScreen::UpdateDyeingUI()
{

    if (!ShouldShowDyeingUI())
        return;



    ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();

    switch (m_eInventoryState)
    {
    case BROWSE_ITEMS_STATE:
        tDyeView.SetDye(idSelectedItem);
        break;

    case BROWSE_ITEMS_TO_DYE_STATE:
        tDyeView.SelectItem(idSelectedItem);
        break;
    }
}




void PlayerInventoryScreen::UpdateAddSocketsState()
{

    if (m_eInventoryState == BUSY_ADDING_SOCKETS_STATE && tAddSocketsView.IsFinishedAddingSockets())
    {
        tItemsProvider.Refresh();
        GetPrimaryView()->RequestUpdate(RESET_SELECTION);
        m_eInventoryState = FINISHED_ADDING_SOCKETS_STATE;
    }
}




void PlayerInventoryScreen::UpdateAddSocketsUI()
{

    if (!ShouldShowAddSocketsUI())
        return;



    ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();

    switch (m_eInventoryState)
    {
    case BROWSE_ITEMS_STATE:
        tAddSocketsView.SetAddSocketsItem(idSelectedItem);
        break;

    case BROWSE_ITEMS_TO_ADD_SOCKETS_TO_STATE:
        tAddSocketsView.SelectItem(idSelectedItem);
        tAddSocketsView.SetAddSocketsRecipient(idSelectedItem);
        break;
    }
}
#line 2509 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
// Original standalone body was discarded (concrete DIE at low_pc 0); only its inlined copy is checkable.
void PlayerInventoryScreen::SyncCategories()
{

    ItemCategory eCategory = (ItemCategory)tCategoriesView.GetSelectedItem();
    if (eCategory != CATEGORY_NONE)
    {
        tCategorySummaryView.SetCategory(eCategory);
    }
}
#line 1545 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
void PlayerInventoryScreen::UpdateSocketingState()
{

    if (m_eInventoryState == BUSY_SOCKETING_STATE && tSocketView.IsFinishedSocketing())
    {
        tItemsProvider.Refresh();
        GetPrimaryView()->RequestUpdate(RESET_SELECTION);




        ACDID idSelectedItem = tSocketView.GetSelectedItem();
        ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
        if (ptACDItem)
        {
            int32 nNumSocketsFilled = ItemGetNumFilledSockets(*ptACDItem);
            int32 nTotalSockets = ptACDItem->AttributesGetInt(FastAttribKey(ATTRIBUTE_SOCKETS));
            if (nTotalSockets > nNumSocketsFilled)
            {
                ACDID idJewel = tSocketView.GetJewel();
                ActorCommonData * ptACDJewel = ACDTryToGet(idJewel);
                if (ptACDJewel)
                {
                    const ACDIDArray & arGems = tItemsProvider.GetItemsWithFilter(FILTER_GEMS);
                    for (uint32 i = 0; i < arGems.size(); ++i)
                    {
                        ActorCommonData * ptACDOtherGem = ACDTryToGet(arGems[i]);
                        if (ptACDOtherGem)
                        {
                            // columns 57/84 on DWARF line 1574: jewel gbid on the left of ==, other gem on the right
                            if (ptACDJewel->hGB.gbid == ptACDOtherGem->hGB.gbid)
                            {

                                tSocketView.ResetToSocketAgain(arGems[i]);
                                m_eInventoryState = READY_TO_SOCKET_AGAIN_STATE;
                                ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
                                // the READY path jumps into the shared state-store/SetActionsDirtyFlag tail (line 0) and never reaches line 1588
                                return;
                            }
                        }
                    }
                }
            }
        }

        m_eInventoryState = FINISHED_SOCKETING_STATE;
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}
#line 1997 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
void PlayerInventoryScreen::OpenCategory(
    ItemCategory eCategory)
{
    UIFilterType eFilterType = FILTER_NONE;
    switch (eCategory)
    {
    case CATEGORY_GEMS: ShowGemCategories();
        return;



    case CATEGORY_COSMETICS: ShowCosmeticCategories();
        return;


    default:
    {
        eFilterType = GetFilterTypeForCategory(eCategory, FALSE);
        m_eCurrentCategory = eCategory;


        if (eFilterType != FILTER_NONE)
        {
            tItemsProvider.SetFilter(eFilterType);
            tSyncedDetailsProvider.SetFilter(eFilterType);
            tEquippedItemProvider.SetFilter(eFilterType);
            tCosmeticItemsProvider.SetFilter(eFilterType);
            tCraftingMatItemsProvider.SetFilter(eFilterType);
        }


        if (IsGemSubCategory(eCategory) || eCategory == CATEGORY_SHARD)
        {
            m_eInventoryState = BROWSE_GEMS_STATE;
        }
        else if (eFilterType == FILTER_COSMETICS || IsCosmeticSubCategory(eCategory))
        {
            if (eCategory == CATEGORY_PORTRAIT_FRAMES)
            {
                m_eInventoryState = BROWSE_PORTRAIT_FRAMES_STATE;
            }
            else
            {
                m_eInventoryState = BROWSE_COSMETICS_STATE;
            }
        }
        else if (eFilterType == FILTER_CRAFTING_MATERIALS)
        {
            m_eInventoryState = BROWSE_CRAFTING_MATS_STATE;
        }
        else
        {
            m_eInventoryState = BROWSE_ITEMS_STATE;
        }
    }
    }
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    SetFocusedView(GetPrimaryView());
}
#line 574 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
UIInventoryView * PlayerInventoryScreen::GetSecondaryViewForMode(
    const BrowsingMode)
{
    if (m_fInTradeSelectMode || m_fInMailSelectMode)
    {
        if (GetFilter() == FILTER_BAG &&
            (m_eInventoryState == BROWSE_CATEGORY_STATE || m_eInventoryState == BROWSE_SUBCATEGORY_STATE))
        {
            return &tCategorySummaryView;
        }
        else
        {
            return &tBagDetailsView;
        }
    }


    if (ShouldShowSocketingUI())
    {
        return &tSocketView;
    }
    else if (ShouldShowDyeingUI())
    {
        return &tDyeView;
    }
    else if (ShouldShowAddSocketsUI())
    {
        return &tAddSocketsView;
    }
    else if (GetFilter() == FILTER_BAG)
    {
        if (m_eInventoryState == BROWSE_CATEGORY_STATE || m_eInventoryState == BROWSE_SUBCATEGORY_STATE)
        {
            return &tCategorySummaryView;
        }
        else if (m_eInventoryState == BROWSE_PORTRAIT_FRAMES_STATE)
        {
            return &tPortraitFrameSummaryView;
        }
        else
        {
            return &tBagDetailsView;
        }
    }
    else
    {
        return &tEquippedView;
    }

}
#line 2869 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
void PlayerInventoryScreen::UpdateCategoryUI(
    const UIID & idItemUI,
    const ItemCategory eCategory)
{
    BOOL bIsCosmeticsCategory = FALSE;
    switch (eCategory)
    {
    case CATEGORY_COSMETICS: case CATEGORY_WINGS: case CATEGORY_PETS: case CATEGORY_PENNANTS: case CATEGORY_PORTRAIT_FRAMES:  // inferred switch: the original tests 12..15 then 1 (an || chain orders them differently); no row of its own on 2875-2878
        bIsCosmeticsCategory = TRUE; break;
    }

    BOOL bIsEnabled = FALSE;


    if (bIsCosmeticsCategory)
    {
        if (!GameIsWeeklyChallenge())
        {
            bIsEnabled = CategoryHasCosmeticItems(eCategory, tItemsProvider, tCosmeticItemsProvider);
        }
    }
    else if (eCategory == CATEGORY_CRAFTING && CategoryHasCraftingMatItems(eCategory, tItemsProvider, tCraftingMatItemsProvider))
    {
        bIsEnabled = TRUE;
    }
    else
    {
        bIsEnabled = CategoryHasItems(eCategory, tItemsProvider, FALSE);
    }

    BOOL bIsNew = CategoryHasNewItems(eCategory, GetPlayer());
    BOOL bIsNewElite = CategoryHasNewEliteItems(eCategory, GetPlayer());
    UpdateCommonCategoryUI(idItemUI, eCategory, bIsEnabled, bIsNew, bIsNewElite);
}
#line 1042 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
void PlayerInventoryScreen::UpdateControllerActions(
    ::Console::ControllerActionManager & tActionManager)
{


    InputClearRemappings();



    tActionManager.ClearAllActions();

    if (RingHasFocus())
    {

        if (ShouldEnableRingIcon(GetFilter()))
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_SLOT, StringListFetch("ConsoleUI:Open").str());
        }

        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
        if (!m_fInTradeSelectMode && !m_fInMailSelectMode)
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_CHARACTER_DETAILS, StringListFetch("ConsoleSkills:CharacterDetails").str());
        }
        return;
    }

    if (GetPrimaryView()->GetIdentifyingItemACD() != ACDID_INVALID)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_IDENTIFY_CANCEL, StringListFetch("H2OLayout:Cancel").str());
        return;
    }

    if (m_fInTradeSelectMode || m_fInMailSelectMode)
    {
        if (ShowingBagUI())
        {
            switch (m_eInventoryState)
            {
            case BROWSE_CATEGORY_STATE:
            case BROWSE_SUBCATEGORY_STATE:
            case BROWSE_COSMETICS_STATE: case BROWSE_PORTRAIT_FRAMES_STATE:
                if (m_eInventoryState == BROWSE_COSMETICS_STATE && m_fInTradeSelectMode)
                {
                    UpdateTradeSelectActions(tActionManager);
                }
                else
                {
                    UpdateBagItemActions(tActionManager);
                }
                return;
            case BROWSE_CRAFTING_MATS_STATE: return;
            }
        }
        if (m_fInMailSelectMode)
        {
            UpdateMailSelectActions(tActionManager);
        }
        else
        {
            UpdateTradeSelectActions(tActionManager);
        }
        return;
    }

    if (ShouldShowSocketingUI())
    {
        UpdateSocketingActions(tActionManager);
    }
    else if (ShouldShowDyeingUI())
    {
        UpdateDyeingActions(tActionManager);
    }
    else if (ShouldShowAddSocketsUI())
    {
        UpdateAddSocketsActions(tActionManager);
    }
    else if (ShowingBagUI())
    {
        UpdateBagItemActions(tActionManager);
    }
    else
    {
        UpdateNormalItemActions(tActionManager);
    }
}
#line 1739 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
void PlayerInventoryScreen::OnButton2(
    int32 nControllerIndex)
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2);
    switch (idAction) {
    case ACTION_IDENTIFY_CANCEL:
        m_idLastIdentifyingACD = ACDID_INVALID;
        {
            UIInventoryView * ptPrimaryView = GetPrimaryView();
            ACDID idACDIdentifyingItem = ptPrimaryView->GetIdentifyingItemACD();
            if (idACDIdentifyingItem != ACDID_INVALID)
            {
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);


                ptPrimaryView->DoneIdentifying(FALSE);
                EnableTabNavigation(TRUE);
                ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
            }
        }
        break;

    case ACTION_CLOSE:
        if (m_fInTradeSelectMode)
        {

            bExitViaTrade = TRUE;
            ::Console::Screens::OpenToScreen(::Console::Screens::TRADE_SCREEN_SET, 0, nControllerIndex, FALSE);
        }
        else if (m_fInMailSelectMode)
        {
            ::Console::Screens::OpenToScreen(::Console::Screens::MAIL_SCREEN_SET, 0, nControllerIndex, FALSE);
        }
        else
        {
            ::Console::Screens::CloseScreenSet(TRUE, FALSE);
        }
        break;

    case ACTION_CLOSE_SLOT:

        if (GetPrimaryView() && GetPlayer() && GetPlayer()->ptNewItemManager)
        {
            ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
            if (idSelectedItem != ACDID_INVALID)
            {
                GetPlayer()->ptNewItemManager->UnMarkAsNew(idSelectedItem);
                GetPlayer()->ptNewEliteItemManager->UnMarkAsNew(idSelectedItem);
            }
        }

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        CloseSelectedSlot();
        break;

    case ACTION_BACK:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);

        if (ShouldShowSocketingUI())
        {
            if (m_eInventoryState == FINISHED_SOCKETING_STATE)
            {
                BackUpSocketingFlow();
            }
            BackUpSocketingFlow();
        }
        else if (ShouldShowDyeingUI())
        {
            BackUpDyeingFlow();
        }
        else if (ShouldShowAddSocketsUI())
        {
            BackUpAddSocketsFlow();
        }
        else if (ShouldShowCosmeticUI())
        {
            BackUpCosmeticsFlow();
        }
        else if (m_eInventoryState == BROWSE_SUBCATEGORY_STATE || m_eInventoryState == BROWSE_ITEMS_STATE || m_eInventoryState == BROWSE_CRAFTING_MATS_STATE)
        {
            CloseCategory();
        }
    }
}
// Original standalone body was discarded (DW_INL_inlined only); only its inlined copy in OnButton2 is checkable.
#line 2486 "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/UserInterface/Console/UIInventoryConsole.cpp"
BOOL PlayerInventoryScreen::ShouldShowCosmeticUI()
{

    return (m_eInventoryState == BROWSE_COSMETICS_STATE || m_eInventoryState == BROWSE_PORTRAIT_FRAMES_STATE);
}
} // namespace Console
} // namespace UIInventory

// Header-defined globals the original _GLOBAL__sub_I_UIInventoryConsole.cpp initialises, in its order
// (DWARF: SNOName.h:142, WorldPlace.h:23, GfxVB.h:351-354, ldn_Ipv4Address.h:153/158, ldn_MacAddress.h).
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
