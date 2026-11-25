// X/Client/UserInterface/Console/UIConsoleMail.cpp
//
// The console mail screen: the inbox list, sending items/gold/crafting materials to a friend,
// and redeeming received gifts. One screen object is created per game (sgptMailGlobals); the
// free functions below forward UI input to it.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
// SNOName.h, WorldPlace.h and GfxVB.h only for their statics, which the original's static
// initialiser runs ahead of this file's own.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReferencable.h"
#include "../../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReference.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../Console/ControllerUtils.h"
#include "../../Console/Online.h"
#include "UIConsoleInventoryCategorySummaryView.h"
#include "UIConsoleInventoryCategoryView.h"
#include "UIConsoleInventoryDetailsView.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryListView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "UIConsoleItemCategories.h"
#include "ConsoleScreenManager.h"
#include "ConsoleModalNotificationManager.h"
#include "../../../Common/OnlineService/Callback/Callback.h"
#include "Wrappers/RingWrapper.h"
#include "ConsoleUIRActorManager.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Common/Items.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Protobuf/ConsoleMail.pb.h"
#include "UIConsoleStackSplit.h"
// Declared here because their headers are not adopted yet; each comment names the defining file.
extern const UIID UIBTN_INVENTORY_CONSOLE_RING_ARM_LEFT, UIBTN_INVENTORY_CONSOLE_RING_ARM_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_BAG, UIBTN_INVENTORY_CONSOLE_RING_BRACERS, UIBTN_INVENTORY_CONSOLE_RING_FEET, UIBTN_INVENTORY_CONSOLE_RING_FINGER_LEFT, UIBTN_INVENTORY_CONSOLE_RING_FINGER_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_HANDS, UIBTN_INVENTORY_CONSOLE_RING_HEAD, UIBTN_INVENTORY_CONSOLE_RING_LEGS, UIBTN_INVENTORY_CONSOLE_RING_NECK, UIBTN_INVENTORY_CONSOLE_RING_SHOULDER, UIBTN_INVENTORY_CONSOLE_RING_TORSO, UIBTN_INVENTORY_CONSOLE_RING_WAIST;  // X/Client/UserInterface/UIIDs.h
extern const UIID UIWND_INVENTORY_CONSOLE_RING_GLOW, UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR;  // X/Client/UserInterface/UIIDs.h
extern const UIID UIDLG_INVENTORY_CONSOLE, UIWND_INVENTORY_CONSOLE_PANEL_FOCUS_HIGHLIGHT;  // X/Client/UserInterface/UIIDs.h
extern const UIID UIDLG_INVENTORY_CONSOLE_LIST, UIDLG_INVENTORY_MAIL_LIST, UIDLG_SEND_MAIL_CONSOLE_LIST, UIDLG_INVENTORY_CONSOLE_DETAILS, UIDLG_INVENTORY_CONSOLE_EQUIPPED;  // X/Client/UserInterface/UIIDs.h
extern UIID UITXT_INVENTORY_CONSOLE_GOLD;  // DWARF: UIGlobals.cpp:3139; no header declaration recorded.
extern const UIID UIWND_INVENTORY_CONSOLE_ROOT;  // X/Client/UserInterface/UIIDs.h  // X/Client/UserInterface/UIIDs.h
int32 UI_GetPrimaryUIController();  // X/Client/UserInterface/UI.cpp
CRefString UIText_ValueToGameCurrency(const uint64 uAmount, CurrencyType eCurrencyType);  // X/Client/UserInterface/UIText.cpp
CRefString LocalizedSprintf(LPCSTR szFormat, ...);  // X/Sigma/Unicode.cpp
// X/Client/Input/Input.cpp
void InputClearRemappings();
void InputRemapSingleJoyconButton(int32 eButton, int32 eRemappedButton);
CurrencyType CurrencyFromGBID(GBID gbid);  // X/Common/Items.cpp
namespace Console { namespace Screens { void CloseScreenSet(BOOL fClearScreenSetHistory, BOOL fSkipTransition); } }  // ConsoleScreenManager.cpp
namespace Console { namespace Online { BOOL LobbyIsConnected(int32 nControllerIndex); } }  // X/Client/Console/Online.cpp
namespace UIPlayerOptions { namespace Console { void ShowGamerCard(int32 nControllerIndex, OnlineUserId idUser, char * szName, int32 nNameLength); } }  // UIConsolePlayerOptions.cpp
CRefString StringListFetch(LPCSTR szLabel);  // X/Common/Definitions/StringListDefinition.cpp
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
namespace SoundManager { SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions); }
extern const PlaySoundOptions cgtPSODefaults;
#include "ControllerActions.h"
namespace Console {
typedef int32 ControllerInput;
typedef int32 ControllerActionId;








}  // namespace Console

// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);
void MessageSendToServer(int32 nMessageID, const google::protobuf::MessageLite & tMessage, const Player * ptPlayer);
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Protobuf/Items.pb.h"
#include "../../../../Contrib/Contrib/blz/include/blz/func_poly.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_sort.h"
GBID CurrencyGetGBID(const CurrencyType eCurrencyType);  // DWARF Items.cpp:10429.

#include "../../../Common/ActorInventory.h"
namespace Console { namespace Online { void MailVerify(int32 nUserIndex, uint64 idMail, VerifyMailCallback pfnCallback); } }  // DWARF MailService.cpp:1324.
namespace UIInventory { void PlayItemInventorySound(const ActorCommonData & tACDItem); }  // DWARF UIInventoryCommon.cpp:195.




// The mail types (Mail, MailType, MailRef, MailArray, MailResult, MailGetAllOfType) are in
// X/Client/Console/Online.h.
namespace D3 { namespace Items { class Generator; } }
typedef uint64 bdOnlineUserID;



#include "../UIIDs.h"































namespace UIFastEquip { namespace Console { void CloseForPlayer(const int32 nPlayerIndex); } }  // UIConsoleFastEquip.cpp
namespace UIInventory { namespace Console { void UpdateNavBarTabState(const Player * ptPlayer, const UIID & idRoot, LPCSTR szNavigationContainerID, BOOL bUpdatePartyFollowerTab); } }  // UIConsoleInventoryViewUtilities.cpp
namespace Console { namespace GamerProfile { BOOL IsSignedInOnline(int32 nUserIndex); } }  // X/Client/Console/GamerProfileNX64.cpp
BOOL PlayerIsHardcore(const Player * ptPlayer);  // X/Common/Player.cpp
namespace Console { namespace Online {
void MailSend(int32 nUserIndex, MailType eType, const D3::Console::MailItem & tItem, uint64 idDestinationUser, OnlineService::Callback<void(MailResult)> fnCallback);  // X/Client/Console/MailService.cpp:1245
} }
namespace Console { namespace GamerProfile { BOOL IsGuest(int32 nUserIndex); } }  // X/Client/Console/GamerProfileNX64.cpp
void ACDDelete(const ACDID idACD, BOOL bWorldDeleted);  // X/Common/ActorCommonData.cpp:996
void ACDInventoryWhereCanThisGo(const ActorCommonData & tACDPlayer, const GBID gbidItem, int32 & nNumSlots, InventorySlotType * eValidSlots);  // X/Common/ActorInventory.cpp
namespace UIInventory { namespace Console { UIFilterType GetFilterForSlot(InventorySlotType eSlotType); } }  // UIConsoleInventoryViewUtilities.cpp
// Raw UIConsoleStackSplit.cpp:107 only; no recovered header declaration for Open.
namespace UIStackSplit { namespace Console {


void Open(ACDID idACD, uint64 uMin, uint64 uMax, UIStackSplitCallback pfnCallback, StackSplitType eType);
} }
// UIConsoleInventoryViewUtilities.h: unadopted info-panel interface.
namespace UIInventory { namespace Console {
enum InfoPanelType { MAIL_INBOX_PANEL = 10 };
void SwitchToInfoPanel(InfoPanelType eInfoPanel);
} }

namespace UIInventory {
namespace Console {
enum ButtonActions {
    ACTION_TAKE = 0,
    ACTION_CLOSE = 1,
    ACTION_CREATE = 2,
    ACTION_COMPARE = 3,
    ACTION_EQUIP = 4,
    ACTION_BACK = 5,
    ACTION_SELECT = 6,
    ACTION_ADDGOLD = 7,
    ACTION_SEND = 8,
    ACTION_CHANGE = 9,
    ACTION_OPEN_CATEGORY = 10,
    ACTION_VIEW_GAMERCARD = 11,
};

enum {
    MAX_MAIL_ITEMS = 30,
    MAX_FRIENDLIST_FRIENDS = 1000,
};

class MailUIEntry {  // size 0x38
public:
    CRefString sDisplayText;  // 0x00
    CRefString sExpirationText;  // 0x18
    ::Console::Online::MailRef ptMailRef;  // 0x30
};
static_assert(sizeof(MailUIEntry) == 0x38, "MailUIEntry");

class MailViewDataSource : public ConsoleListBoxDataSource {  // size 0x6a0
public:
    MailViewDataSource();
    virtual ~MailViewDataSource() {}
    virtual int32 GetNumItems() const;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
    void ClearAll();
    MailUIEntry tEntryUI[MAX_MAIL_ITEMS];  // 0x08
    int32 nMailItems;  // 0x698
};
static_assert(sizeof(MailViewDataSource) == 0x6a0, "MailViewDataSource");

class UIMailListView : public UIInventoryView {  // size 0x708
public:
    UIMailListView(const UIID & idRoot)  // line 488
        : UIInventoryView(idRoot)
    {
        UIID idStatusLine = GetUIIDNone();
        idStatusLine.SetFromParent(GetRootUIID(), "MailCount.ValueAmount");
        SetStatusLineUIID(idStatusLine);
        m_eLastSelectedCategory = CATEGORY_NONE;
    }
    virtual ~UIMailListView() {}
    virtual BOOL CanAcceptFocus() const;  // vtable slot 2
    BOOL AddItem(const ::Console::Online::MailRef & ptMailRef);
    void ClearItems();
    MailViewDataSource & GetDataSource();
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnEnable(const BOOL bEnable);  // vtable slot 7
    virtual void OnGotFocus();  // vtable slot 8
    virtual void OnLostFocus();  // vtable slot 9
    virtual void OnRequestedUpdate(const MaintainSelectionOptions eSelectionType);  // vtable slot 11
    virtual void OnParentScreenChanged();  // vtable slot 5
    virtual UIID GetHeaderUIID() const;  // vtable slot 13
private:
    MailViewDataSource m_tDataSource;  // 0x60
    ItemCategory m_eLastSelectedCategory;  // 0x700
    UIMailListView(const UIMailListView &);
    UIMailListView & operator=(const UIMailListView &);
    int32 GetIndexOfCategory(const ItemCategory) const;
    UIConsoleListBox & GetListBox();
    void UpdateStatusLineUI(CRefString);
    void UpdateScrollArrows();
};
static_assert(sizeof(UIMailListView) == 0x708, "UIMailListView");

class FriendListUIEntry {  // size 0x20
public:
    FriendListUIEntry() : bIsFemale(FALSE), ePlayerClass(-1) {}  // line 341
    CRefString sDisplayName;  // 0x00
    BOOL bIsFemale;  // 0x18
    PlayerClass ePlayerClass;  // 0x1c
};
static_assert(sizeof(FriendListUIEntry) == 0x20, "FriendListUIEntry");

class FriendListViewDataSource : public ConsoleListBoxDataSource {  // size 0x7d10
public:
    FriendListViewDataSource() : nCount(0) {}  // line 379
    virtual ~FriendListViewDataSource() {}
    virtual int32 GetNumItems() const;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
    FriendListUIEntry m_tEntryUI[MAX_FRIENDLIST_FRIENDS];  // 0x08
    uint32 nCount;  // 0x7d08
};
static_assert(sizeof(FriendListViewDataSource) == 0x7d10, "FriendListViewDataSource");

class MailItemProvider : public UIInventoryViewItemsProvider {  // size 0x38
public:
    virtual ~MailItemProvider() {}
    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType) const;  // vtable slot 6
    ACDIDArray & GetItems();
    void AddItem(const ACDID idItem);
    void RemoveItem(const ACDID idItem);
    void RemoveAllItems();
    int32 GetCount();
private:
    ACDIDArray m_arMailItems;  // 0x20
};
static_assert(sizeof(MailItemProvider) == 0x38, "MailItemProvider");

class UnmailedItemProvider : public UIInventoryViewItemsProvider {  // size 0x1150
public:
    virtual ~UnmailedItemProvider() {}
    UnmailedItemProvider() : m_ptMailItemProvider(NULL) {}  // line 824
    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType eFilterType) const;  // vtable slot 6
    virtual const ACDIDArray & GetItems() { return GetItemsWithFilter(GetFilter()); }  // vtable slot 15; line 840
    void RemoveAllItems();
    void Update();
    void SetMailProvider(MailItemProvider * ptProvider);
    void SetFilter(const UIFilterType eFilterType);
    int32 GetCount();
protected:
    void RefreshView();
    virtual void OnPlayerChanged();  // vtable slot 14
private:
    FilteredInventoryView m_tView;  // 0x20
    MailItemProvider * m_ptMailItemProvider;  // 0x8d8
    ACDIDArray m_arDisplayItems[FILTER_COUNT];  // 0x8e0
    friend class PlayerMailScreen;
};
static_assert(sizeof(UnmailedItemProvider) == 0x1150, "UnmailedItemProvider");

class UnmailedCraftingMatItemsProvider : public CraftingMatItemsProvider {  // size 0x70
public:
    UnmailedCraftingMatItemsProvider() {}  // line 931
    virtual ~UnmailedCraftingMatItemsProvider() { CleanupACDs(); }  // vtable slot 0; line 933
    int32 GetCount();
    virtual void Refresh();  // vtable slot 17
    friend class PlayerMailScreen;
};
static_assert(sizeof(UnmailedCraftingMatItemsProvider) == 0x70, "UnmailedCraftingMatItemsProvider");

class PlayerMailScreen : public UIInventoryScreen {  // size 0xa6a0
public:
    InventoryItems m_tItemsProvider;  // 0x200
    EquippedItem m_tEquippedItemProvider;  // 0xae8
    UnmailedCraftingMatItemsProvider m_tCraftingMatItemsProvider;  // 0xb20
    SingleItemProvider m_tRightSingleItemProvider;  // 0xb90
    MailItemProvider m_tMailItemProvider;  // 0xbc8
    UnmailedItemProvider m_tUnmailedItemProvider;  // 0xc00
    UIInventoryListView m_tListView;  // 0x1d50
    UIInventoryListView m_tCraftingMatListView;  // 0x1dd8
    UIMailListView m_tMailListView;  // 0x1e60
    UIInventoryListView m_tMailItemsView;  // 0x2568
    UIInventoryDetailsView m_tLeftDetailsView;  // 0x25f0
    UIInventoryDetailsView m_tRightDetailsView;  // 0x2668
    UIInventoryDetailsView m_tEquippedView;  // 0x26e0
    UIInventoryCategoryView m_tCategoriesView;  // 0x2758
    FriendListViewDataSource m_tFriendsListDataSource;  // 0x27f0
    VisualInventory m_tVisualInventory;  // 0xa500
    int32 m_nMailItemCount;  // 0xa620
    int32 m_nCurrentMailIndex;  // 0xa624
    int32 m_nFriendCount;  // 0xa628
    bdOnlineUserID m_idBDUser;  // 0xa630
    CRefString m_szFriendName;  // 0xa638
    BOOL m_bHaveFriend;  // 0xa650
    BOOL m_bShowingGift;  // 0xa654
    BOOL m_bShowingGold;  // 0xa658
    BOOL m_bShowingCraftingMaterial;  // 0xa65c
    uint64 m_nGoldToSend;  // 0xa660
    PlayerMailScreen();
    virtual ~PlayerMailScreen();
    virtual void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);  // vtable slot 9
    void OnButton1(int32 nControllerIndex);
    void OnButton2(int32 nControllerIndex);
    void OnButton3(int32 nControllerIndex);
    void OnButton4(int32 nControllerIndex);
    void OnButtonL3(int32 nControllerIndex);
    void OnDPadLeft(int32 nControllerIndex);
    void OnDPadRight(int32 nControllerIndex);
    void OpenSelectedCategory();
    void OpenCategory(ItemCategory eCategory);
    void CloseCategory();
    void ShowTopLevelCategories();
    void ShowGemCategories();
    void SetSelectMode(InventoryItems::ProviderMode);
    void CacheRingSelection();
    BOOL RestoreCachedRingSelection();
    void UpdateCategoryUI(const UIID & idItemUI, const ItemCategory eCategory);
    void UpdateNavBar();
    void SetCurrentSelectedIndex(int32 nNewVal);
    void SendMailAndConfirm();
    void OnMailSendConfirmed(::Console::Online::MailResult eResult);
    void OnGiftRedeemed(Player * ptReceivingPlayer, D3::Items::Generator & tItemData);
    void ShowReceivedGift(D3::Items::Generator & tItemData);
    void RedeemCurrentGift();
    void ShowMailGiftError();
    void SetGoldToSend(uint64 uAmount);
    void UpdateGoldDisplay();
    BOOL GameIsHardcore();
    void ConfirmGiveItemToPlayer(::Console::Online::MailRef ptMail, int32 nControllerIndex, BOOL bEquip);
    void ReturnToInboxMode();
    void UnsubscribeFromNewMail();
    uint64 GetAmountOfCraftingMaterialInMailItemProvider(CurrencyType eCurrency);
protected:
    void OnNewMail(int32 nControllerIndex);
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
    virtual UIInventoryView * GetPrimaryViewForMode(const UIInventoryScreen::BrowsingMode eBrowsingMode);  // vtable slot 23
    virtual UIInventoryView * GetSecondaryViewForMode(const UIInventoryScreen::BrowsingMode eBrowsingMode);  // vtable slot 24
    virtual BOOL ShouldEnableRingIcon(const UIFilterType eFilterType);  // vtable slot 20
public:
    enum MailState {
        MAILSTATE_INVALID = -1,
        MAILSTATE_BROWSE_MAIL = 0,
        MAILSTATE_COMPARING_ITEMS = 1,
        MAILSTATE_SENDING_MAIL = 2,
        MAILSTATE_FRIEND_SELECT = 3,
        MAILSTATE_FRIEND_UPDATE = 4,
        MAILSTATE_VIEWING_RECEIVED_GIFT = 5,
        MAILSTATE_INVALID_GIFT_RECIPIENT = 6,
        MAILSTATE_WAIT_FOR_GIFT_RECEIPT_CONFIRM = 7,
        MAILSTATE_WAITING_FOR_SEND_COMPLETE = 8,
    };
    enum NoMailReason {
        NOMAILREASON_NO_MAIL = 0,
        NOMAILREASON_NO_CONNECTION = 1,
        NOMAILREASON_CLOSE = 2,
    };
    enum CategoryState {
        BROWSE_CATEGORY_STATE = 0,
        BROWSE_SUBCATEGORY_STATE = 1,
        BROWSE_GEMS_STATE = 2,
        BROWSE_ITEMS_STATE = 3,
        BROWSE_CRAFTING_MATS_STATE = 4,
    };
public:
    // Private in the original; the module's free functions are its friends there.
    MailState m_eMailState;  // 0xa668
    CategoryState m_eCategoryState;  // 0xa66c
    ItemCategory m_eCurrentCategory;  // 0xa670
    int32 m_nLastGiftMailIndex;  // 0xa674
    UIFilterType m_eCachedRingSelection;  // 0xa678
    ACDID m_idLastSelectedItem;  // 0xa67c
    ACDID m_idACDCurrentItem;  // 0xa680
    ACDID m_idACDSocketed[3];  // 0xa684
    int32 m_nSocketsFilled;  // 0xa690
    BOOL m_bCurrentItemIsEquippable;  // 0xa694
    BOOL m_bNewMailAvailable;  // 0xa698
    BOOL CanOpenSelectedCategory();
    void UpdateInboxActions(::Console::ControllerActionManager & tActionManager);
    void UpdateCompareActions(::Console::ControllerActionManager & tActionManager);
    void UpdateFriendSelectActions(::Console::ControllerActionManager & tActionManager);
    void UpdateSendMailActions(::Console::ControllerActionManager & tActionManager);
    void UpdateBagItemActions(::Console::ControllerActionManager & tActionManager);
    void UpdateReceivedGiftViewingActions(::Console::ControllerActionManager & tActionManager);
    void UpdateEquippedPrompt();
    BOOL ShowingBagUI();
    void SyncCategories();
    void SyncItems();
    void EquipItem();
    void UnMarkEquippedItemAsNew(const UIFilterType);
    void SwitchToInboxMode();
    void SwitchToCompareMode();
    void SwitchToSendMailMode(BOOL bResetMail);
    void SwitchToFriendSelectMode(BOOL bChangeRecipient);
    void GetMail();
    void UpdateDisplayedMail();
    void UpdateInboxDisplay();
    void UpdateFriendList();
    void UpdateCompareDisplay(BOOL);
    void SetInboxActive(BOOL bActive);
    void SetMailboxActive(BOOL bActive);
    void SetNoMailDisplay(BOOL bActive, NoMailReason eReason);
    void SetFriendListActive(BOOL bActive);
    void ShowReceivedGiftDetails(BOOL bShow);
    BOOL TakeCurrentItem(BOOL bEquip);
    void SetMailRecipient();
    void ConfirmMailSend();
    void PlayerMailConfirmSend(const UIID &);
    ACDID GenerateFakeItem(const D3::Items::Generator & tGenerator);
    void DeleteFakeItem();
    void UpdateCompareItem();
    BOOL CanEquipCurrent();
    BOOL CheckLobbyConnection();
    BOOL PlayerIsOnline();
    void UpdatePlayerGold();
};
static_assert(sizeof(PlayerMailScreen) == 0xa6a0, "PlayerMailScreen");

static ModalNotificationId s_idModal = -1;
}  // namespace Console
}  // namespace UIInventory
namespace UIMail {
namespace Console {
struct GiftToSend {  // size 0x8 (invented name)
    ACDID idACDGift;
    int32 nControllerIndex;
};
static GiftToSend sidGiftToSend = { ACDID_INVALID, -1 };
}  // namespace Console
}  // namespace UIMail
namespace UIInventory {
namespace Console {

static ::Console::Online::MailArray * sgpMailArray;
static ::Console::Online::Friend * sgptFriendsArray;

PlayerMailScreen * sgptMailGlobals;

MailViewDataSource::MailViewDataSource()
{
    ClearAll();
}

inline void MailViewDataSource::ClearAll()
{
    nMailItems = 0;
    for (int32 i = 0; i < MAX_MAIL_ITEMS; ++i)
    {
        tEntryUI[i].ptMailRef = NULL;
    }
}

int32 MailViewDataSource::GetNumItems() const
{
    return nMailItems;
}

UIControlSourceKey MailViewDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    return UIControlSourceKey::Parse("ConsoleMail_ListTemplate:ConsoleMail_ListTemplate_main", SNO_NONE, NULL, NULL);
}

void MailViewDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}

BOOL UIMailListView::CanAcceptFocus() const
{
    if (m_tDataSource.GetNumItems() == 0)
        return FALSE;
    return UIInventoryView::CanAcceptFocus();
}

int32 FriendListViewDataSource::GetNumItems() const
{
    return nCount;
}

UIControlSourceKey FriendListViewDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    return UIControlSourceKey::Parse("ConsoleMail_FriendListTemplate:ConsoleMail_FriendListTemplate_main", SNO_NONE, NULL, NULL);
}

void FriendListViewDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}

void sOnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal);

void UIMailListView::OnShow(const BOOL bShow)
{
    GetListBox().SetOnSelChangeFunction(bShow ? sOnListSelectionChanged : NULL);
    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);
}

void UIMailListView::OnEnable(const BOOL bEnable)
{
    UIID idDimmer;
    idDimmer.SetFromParent(GetRootUIID(), "Dimmer");
    UI_GetWindowFromID(idDimmer).SetActive(!bEnable);
    if (!bEnable)
    {
        UI_SetControllerFocus(GetUIIDNone());
    }
}

void UIMailListView::OnGotFocus()
{
    int32 nIndex = m_tDataSource.GetNumItems() > 0 ? 0 : -1;
    UIConsoleListBox & tListBox = GetListBox();
    UI_SetControllerFocus(tListBox.GetID());
    tListBox.SetSelectedIndex(nIndex);
}

void UIMailListView::OnLostFocus()
{
    UI_SetControllerFocus(GetUIIDNone());
}

void UIMailListView::OnRequestedUpdate(const MaintainSelectionOptions eSelectionType)
{
    UIConsoleListBox & tListBox = GetListBox();
    int32 nOldSelectedIndex = tListBox.GetSelectedIndex();
    int32 nOldFirstVisibleIndex = tListBox.GetFirstVisibleItemIndex();
    int32 nNewSelectedIndex;
    ClearSelectedItem();
    tListBox.ResetAllItems();
    int32 nNumItems = tListBox.GetNumItems();
    switch (eSelectionType)
    {
    case KEEP_ITEM_SELECTED:
        if (nOldSelectedIndex >= nNumItems)
            nOldSelectedIndex = -1;
        nNewSelectedIndex = (nOldSelectedIndex == -1 && nNumItems > 0) ? 0 : nOldSelectedIndex;
        break;
    case KEEP_INDEX_SELECTED:
        nNewSelectedIndex = Math::Pin(nOldSelectedIndex, 0, nNumItems - 1);
        if (nOldFirstVisibleIndex >= 0 && nOldFirstVisibleIndex < nNumItems)
            tListBox.SetFirstVisibleItemIndex(nOldFirstVisibleIndex);
        break;
    default:
        nNewSelectedIndex = 0;
        break;
    }
    tListBox.SetSelectedIndex(nNewSelectedIndex);
}

UIID UIMailListView::GetHeaderUIID() const
{
    return UIIDCreateFromParent(GetRootUIID(), ".WindowTitle");
}

void UIMailListView::OnParentScreenChanged()
{
    UIConsoleListBox & tListBox = GetListBox();
    if (GetParentScreen() != NULL)
    {
        tListBox.SetUserData(this);
        tListBox.SetDataSource(m_tDataSource);
        tListBox.ResetAllItems();
    }
    else
    {
        tListBox.SetUserData(NULL);
    }
}

inline UIConsoleListBox & UIMailListView::GetListBox()
{
    UIID idListBox = GetUIIDNone();
    idListBox.SetFromParent(GetRootUIID(), "ItemList");
    return UI_GetConsoleListBoxFromID(idListBox);
}

const ACDIDArray & MailItemProvider::GetItemsWithFilter(const UIFilterType) const
{
    return m_arMailItems;
}

const ACDIDArray & UnmailedItemProvider::GetItemsWithFilter(const UIFilterType eFilterType) const
{
    if (eFilterType != FILTER_NONE)
        return m_arDisplayItems[eFilterType];
    return m_tView.GetItems(FILTER_NONE);
}


void UnmailedItemProvider::OnPlayerChanged()
{
    if (GetPlayer())
    {
        ACDID idACDPlayer = GetPlayer()->idPlayerACD;
        m_tView.ClearItems();
        m_tView.AddItems(idACDPlayer, idACDPlayer, 0, 0x600, 0);
    }
}


CRefString sGetExpirationfromTimestamp(Blizzard::Time::Timestamp nExpiration);  // defined below on its DWARF line 655




















BOOL MailRingIncludeControlPredicate(const UIID & idControl)
{
    return idControl != UIWND_INVENTORY_CONSOLE_RING_GLOW;
}

PlayerMailScreen::~PlayerMailScreen()
{
}

}  // namespace Console
}  // namespace UIInventory
static void sOnUpdateCategoryUI(const UIID & idItemUI, const UIInventory::Console::ItemCategory eCategory);
namespace UIInventory {
namespace Console {

// NON_MATCHING: register allocation only (the original keeps &m_tListView in a callee-saved register across the provider memsets; 4 bytes larger frame)
PlayerMailScreen::PlayerMailScreen()
    : m_tItemsProvider(InventoryItems::PROVIDERMODE_BACKPACK)
    , m_tListView(m_tUnmailedItemProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
    , m_tCraftingMatListView(m_tCraftingMatItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
    , m_tMailListView(UIDLG_INVENTORY_MAIL_LIST)
    , m_tMailItemsView(m_tMailItemProvider, UIDLG_SEND_MAIL_CONSOLE_LIST, TRUE)
    , m_tLeftDetailsView(m_tRightSingleItemProvider, UIDLG_INVENTORY_CONSOLE_DETAILS, 0)
    , m_tRightDetailsView(m_tRightSingleItemProvider, UIDLG_INVENTORY_CONSOLE_EQUIPPED, 0)
    , m_tEquippedView(m_tEquippedItemProvider, UIDLG_INVENTORY_CONSOLE_EQUIPPED, 0)
    , m_tCategoriesView(UIDLG_INVENTORY_CONSOLE_LIST, sOnUpdateCategoryUI)
    , m_nMailItemCount(0)
    , m_nCurrentMailIndex(-1)
    , m_nFriendCount(0)
    , m_idBDUser(0)
    , m_bHaveFriend(FALSE)
    , m_bShowingGift(FALSE)
    , m_bShowingGold(FALSE)
    , m_bShowingCraftingMaterial(FALSE)
    , m_nGoldToSend(0)
    , m_eMailState(MAILSTATE_INVALID)
    , m_eCategoryState(BROWSE_CATEGORY_STATE)
    , m_eCurrentCategory(CATEGORY_NONE)
    , m_nLastGiftMailIndex(-1)
    , m_eCachedRingSelection(FILTER_NONE)
    , m_idLastSelectedItem(ACDID_INVALID)
    , m_idACDCurrentItem(ACDID_INVALID)
    , m_nSocketsFilled(0)
    , m_bCurrentItemIsEquippable(FALSE)
    , m_bNewMailAvailable(FALSE)
{
    m_tVisualInventory.Init();
    m_tListView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tCraftingMatListView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tLeftDetailsView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tRightDetailsView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    m_tEquippedView.SetHeaderText(StringListFetch("ConsoleInventory:Equipped"));
    m_tCategoriesView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tUnmailedItemProvider.SetMailProvider(&m_tMailItemProvider);
    for (int32 n = 0; n < 3; ++n)
    {
        m_idACDSocketed[n] = ACDID_INVALID;
    }
}

inline void UnmailedItemProvider::SetMailProvider(MailItemProvider * ptProvider)
{
    m_ptMailItemProvider = ptProvider;
    Update();
}

void PlayerMailScreen::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)
{
    switch (eInputButton)
    {
    case BUTTON_1: OnButton1(nControllerIndex); break;
    case BUTTON_2: OnButton2(nControllerIndex); break;
    case BUTTON_3: OnButton3(nControllerIndex); break;
    case BUTTON_4: OnButton4(nControllerIndex); break;
    case 6:
    case 7:
    case 15:
        break;
    case 10: OnDPadLeft(nControllerIndex); break;
    case 11: OnDPadRight(nControllerIndex); break;
    case 14: OnButtonL3(nControllerIndex); break;
    default: UIInventoryScreen::HandleButtonInput(nControllerIndex, eInputButton); break;
    }
}

BOOL PlayerMailScreen::CanOpenForController(int32 nControllerIndex) const
{
    return UIInventoryScreen::CanOpenForController(nControllerIndex);
}

void PlayerMailScreen::OnSetActive(BOOL bActive)
{
    if (bActive)
    {
        ShowNavBar(UIDLG_INVENTORY_CONSOLE, "MailNavigation");
        RequestUpdate(RESET_SELECTION);
        ::Console::Screens::ShowModalDialog(UI_GetPrimaryUIController(), UIDLG_INVENTORY_CONSOLE, FALSE, GetUIIDNone());
        SetFocusedView(GetPrimaryView());
    }
    else
    {
        ::Console::Screens::CloseModalDialog(UIDLG_INVENTORY_CONSOLE);
    }
}

void PlayerMailScreen::InitializeRingWrapper(::Console::RingWrapper & tRingWrapper)
{
    tRingWrapper.WrapUsingContainerControl(UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR, MailRingIncludeControlPredicate, FALSE);
}

UIFilterType PlayerMailScreen::GetDefaultRingSelection() const
{
    return 0;
}

void PlayerMailScreen::InitRingFilters(UIIDToFilterMap & tFilterMap)
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

UIInventoryView * PlayerMailScreen::GetPrimaryViewForMode(const UIInventoryScreen::BrowsingMode eBrowsingMode)
{
    switch (m_eMailState)
    {
    case MAILSTATE_BROWSE_MAIL:
        return &m_tMailListView;
    case MAILSTATE_SENDING_MAIL:
        if (m_eCategoryState == BROWSE_CRAFTING_MATS_STATE)
            return &m_tCraftingMatListView;
        if (ShowingBagUI())
            return &m_tCategoriesView;
        return &m_tListView;
    case MAILSTATE_COMPARING_ITEMS:
        return &m_tLeftDetailsView;
    }
    return NULL;
}

UIInventoryView * PlayerMailScreen::GetSecondaryViewForMode(const UIInventoryScreen::BrowsingMode eBrowsingMode)
{
    switch (m_eMailState)
    {
    case MAILSTATE_BROWSE_MAIL:
        return &m_tRightDetailsView;
    case MAILSTATE_COMPARING_ITEMS:
        return &m_tEquippedView;
    case MAILSTATE_SENDING_MAIL:
        return &m_tMailItemsView;
    }
    return NULL;
}

BOOL PlayerMailScreen::ShouldEnableRingIcon(const UIFilterType eFilterType)
{
    if (eFilterType != FILTER_NONE)
    {
        size_t nCount = m_tUnmailedItemProvider.m_arDisplayItems[eFilterType].size();
        if (eFilterType == FILTER_CRAFTING_MATERIALS || eFilterType == FILTER_BAG)
            nCount += m_tCraftingMatItemsProvider.m_arrVirtualItems.size();
        return nCount != 0;
    }
    return m_tUnmailedItemProvider.m_tView.GetItems(FILTER_NONE).size() != 0;
}

void sOnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal)
{
    if (sgptMailGlobals)
    {
        sgptMailGlobals->SetCurrentSelectedIndex(nNewVal);
    }
}

inline void sMailConnectionErrorAccept(const UIID &)
{
    ::Console::Screens::CloseScreenSet(TRUE, FALSE);
}

BOOL PlayerMailScreen::CheckLobbyConnection()
{
    if (!::Console::Online::LobbyIsConnected(GetControllerIndex()))
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = GetControllerIndex();
        tParams.uszMessage = StringListFetch("X1_ConsoleMail:MailConnectionError");
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sMailConnectionErrorAccept>();
        s_idModal = ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        return FALSE;
    }
    return TRUE;
}

BOOL PlayerMailScreen::PlayerIsOnline()
{
    if (!::Console::GamerProfile::IsSignedInOnline(GetControllerIndex()))
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = GetControllerIndex();
        tParams.uszMessage = StringListFetch("X1_ConsoleMail:ErrorNotOnline");
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sMailConnectionErrorAccept>();
        s_idModal = ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        return FALSE;
    }
    if (::Console::GamerProfile::IsGuest(GetControllerIndex()))
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = GetControllerIndex();
        tParams.uszMessage = StringListFetch("X1_ConsoleMail:ErrorNoGuestAccess");
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sMailConnectionErrorAccept>();
        s_idModal = ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        return FALSE;
    }
    return TRUE;
}

uint64 PlayerMailScreen::GetAmountOfCraftingMaterialInMailItemProvider(CurrencyType eCurrency)
{
    PlayerMailScreen * ptScreen = sgptMailGlobals;
    int32 nCount = ptScreen->m_tMailItemProvider.GetCount();
    for (int32 i = 0; i < nCount; ++i)
    {
        ACDID idItem = ptScreen->m_tMailItemProvider.GetItems()[i];
        if (idItem != ACDID_INVALID)
        {
            ActorCommonData * ptACDItem = ACDTryToGet(idItem);
            if (ptACDItem && CurrencyFromGBID(ptACDItem->hGB.gbid) == eCurrency)
            {
                return ptACDItem->GetStackCount();
            }
        }
    }
    return 0;
}

inline int32 MailItemProvider::GetCount()
{
    return m_arMailItems.size();
}

inline ACDIDArray & MailItemProvider::GetItems()
{
    return m_arMailItems;
}

void PlayerMailScreen::SwitchToInboxMode()
{
    SwitchToInfoPanel(MAIL_INBOX_PANEL);
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING_CURSOR).SetActive(FALSE);
    EnableRing(FALSE);
    ::Console::UIRActorManager::SetActive(FALSE);
    m_eMailState = MAILSTATE_BROWSE_MAIL;
    m_eCategoryState = BROWSE_CATEGORY_STATE;
    if (m_bNewMailAvailable)
    {
        GetMail();
        m_bNewMailAvailable = FALSE;
    }
    m_nGoldToSend = 0;
    UpdateInboxDisplay();
}

inline void PlayerMailScreen::GetMail()
{
    if (CheckLobbyConnection())
    {
        sgpMailArray->clear();
        ::Console::Online::MailGetAllOfType(GetControllerIndex(), ::Console::Online::MAIL_TYPE_MESSAGE, *sgpMailArray, 0, 10000);
        UpdateDisplayedMail();
    }
}

void PlayerMailScreen::OnNewMail(int32 nControllerIndex)
{
    if (GetControllerIndex() == nControllerIndex && m_eMailState == MAILSTATE_BROWSE_MAIL)
    {
        GetMail();
        RequestUpdate(KEEP_INDEX_SELECTED);
    }
    else
    {
        m_bNewMailAvailable = TRUE;
    }
}

void PlayerMailScreen::UpdateInboxActions(::Console::ControllerActionManager & tActionManager)
{
    if (m_nMailItemCount > 0)
    {
        if (m_bShowingGift)
        {
            tActionManager.SetAction(4, ACTION_TAKE, StringListFetch("X1_ConsoleMail:RedeemGift").str());
        }
        else if (m_bShowingGold)
        {
            tActionManager.SetAction(4, ACTION_TAKE, StringListFetch("X1_ConsoleMail:TakeGold").str());
        }
        else
        {
            tActionManager.SetAction(4, ACTION_TAKE, StringListFetch("X1_ConsoleMail:TakeItem").str());
            if (m_bCurrentItemIsEquippable)
            {
                tActionManager.SetAction(5, ACTION_COMPARE, StringListFetch("ConsoleUI:Compare").str());
            }
        }
    }
    tActionManager.SetAction(7, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
    tActionManager.SetAction(6, ACTION_CREATE, StringListFetch("X1_ConsoleMail:CreateMail").str());
}

void PlayerMailScreen::OpenCategory(ItemCategory eCategory)
{
    if (eCategory == CATEGORY_GEMS)
    {
        ShowGemCategories();
    }
    else
    {
        UIFilterType eFilterType = GetFilterTypeForCategory(eCategory, FALSE);
        m_eCurrentCategory = eCategory;
        if (eFilterType != FILTER_NONE)
            m_tUnmailedItemProvider.SetFilter(eFilterType);
        if (IsGemSubCategory(eCategory))
        {
            m_eCategoryState = BROWSE_GEMS_STATE;
        }
        else if (eFilterType == FILTER_CRAFTING_MATERIALS)
        {
            m_eCategoryState = BROWSE_CRAFTING_MATS_STATE;
            m_tCraftingMatItemsProvider.SetFilter(FILTER_CRAFTING_MATERIALS);
        }
        else
        {
            m_eCategoryState = BROWSE_ITEMS_STATE;
        }
        SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
        SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
        GetPrimaryView()->Enable(TRUE);
        SetFocusedView(GetPrimaryView());
    }
}

void PlayerMailScreen::CloseCategory()
{
    ItemCategory eCurrentCategory = m_eCurrentCategory;
    if (m_eCategoryState == BROWSE_GEMS_STATE)
    {
        m_tUnmailedItemProvider.SetFilter(FILTER_GEMS);
        ShowGemCategories();
    }
    else
    {
        m_tUnmailedItemProvider.SetFilter(FILTER_BAG);
        ShowTopLevelCategories();
    }
    m_tListView.Enable(FALSE);
    SetFocusedView(GetPrimaryView());
    m_tCategoriesView.SelectItem((ACDID)eCurrentCategory);
}

inline void PlayerMailScreen::ShowGemCategories()
{
    m_eCategoryState = BROWSE_SUBCATEGORY_STATE;
    m_eCurrentCategory = CATEGORY_GEMS;
    PopulateGemCategories(m_tCategoriesView);
    if (GetPrimaryView() != &m_tCategoriesView)
        SetPrimaryView(&m_tCategoriesView);
    else
        m_tCategoriesView.ForceUpdate(RESET_SELECTION);
}























void PlayerMailScreen::UnsubscribeFromNewMail()
{
    ::Console::Online::GetNewMailEvent().Unsubscribe(OnlineService::internal::method_bind_0_1<void, PlayerMailScreen, PlayerMailScreen *, int32, &PlayerMailScreen::OnNewMail>(this));
}

void PlayerMailScreen::OnUpdate()
{
    BOOL bShowHighlight = !RingHasFocus();
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_PANEL_FOCUS_HIGHLIGHT).SetActive(bShowHighlight);
}

void PlayerMailScreen::OnRequestedUpdate()
{
}

void PlayerMailScreen::UpdateControllerActions(::Console::ControllerActionManager & tActionManager)
{
    tActionManager.ClearAllActions();
    switch (m_eMailState)
    {
    case MAILSTATE_BROWSE_MAIL:
        UpdateInboxActions(tActionManager);
        break;
    case MAILSTATE_COMPARING_ITEMS:
        UpdateCompareActions(tActionManager);
        break;
    case MAILSTATE_SENDING_MAIL:
        UpdateSendMailActions(tActionManager);
        break;
    case MAILSTATE_FRIEND_SELECT:
    case MAILSTATE_FRIEND_UPDATE:
        UpdateFriendSelectActions(tActionManager);
        break;
    case MAILSTATE_VIEWING_RECEIVED_GIFT:
        UpdateReceivedGiftViewingActions(tActionManager);
        break;
    default:
        break;
    }
}

inline void PlayerMailScreen::UpdateReceivedGiftViewingActions(::Console::ControllerActionManager & tActionManager)
{
    tActionManager.SetAction(4, ACTION_TAKE, StringListFetch("X1_ConsoleMail:TakeItem").str());
}

void PlayerMailScreen::UpdateFriendSelectActions(::Console::ControllerActionManager & tActionManager)
{
    InputClearRemappings();
    tActionManager.SetAction(4, ACTION_SELECT, StringListFetch("ConsoleUI:Select").str());
    tActionManager.SetAction(7, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
    InputRemapSingleJoyconButton(8, 14);
    tActionManager.SetAction(21, ACTION_VIEW_GAMERCARD, StringListFetch("Console:PlayerOptionViewUserPageNX64").str());
}

void PlayerMailScreen::OnButtonL3(int32 nControllerIndex)
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(21);
    switch (idAction)
    {
    case ACTION_CHANGE:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_BACK), cgtPSODefaults);
        m_eCategoryState = BROWSE_CATEGORY_STATE;
        SwitchToFriendSelectMode(TRUE);
        break;
    case ACTION_VIEW_GAMERCARD:
        if (m_eMailState == MAILSTATE_FRIEND_SELECT || m_eMailState == MAILSTATE_FRIEND_UPDATE)
        {
            int32 nIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDSLIST).GetSelectedIndex();
            if (nIndex >= 0 && nIndex < m_nFriendCount)
            {
                OnlineUserId & tID = sgptFriendsArray[nIndex].m_idFriend;
                UIPlayerOptions::Console::ShowGamerCard(nControllerIndex, tID, sgptFriendsArray[nIndex].m_szUsername, 32);
            }
        }
        break;
    }
}

void PlayerMailScreen::UpdatePlayerGold()
{
    Player * ptPlayer = PlayerGetByControllerIndex(GetControllerIndex());
    if (ptPlayer && ptPlayer->idPlayerACD != ACDID_INVALID)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_GOLD).SetText(UIText_ValueToGameCurrency(ptACDPlayer->GetCurrencyAmount(0) - m_nGoldToSend, 0), FALSE);
        }
    }
}

void PlayerMailScreen::UpdateGoldDisplay()
{
    const UIID & idGoldText = m_tMailItemsView.GetStatusLineUIID();
    if (m_nGoldToSend == 0)
    {
        UI_GetTextFromID(idGoldText).SetText("", FALSE);
    }
    else
    {
        CRefString sGold = UIText_ValueToGameCurrency(m_nGoldToSend, 0);
        CRefString sText = LocalizedSprintf(StringListFetch("X1_ConsoleMail:SendGoldFormat").str(), sGold.str(), NULL);
        UI_GetTextFromID(idGoldText).SetText(sText.str(), FALSE);
    }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerMailScreen::SetMailRecipient()
{
    int32 nIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDSLIST).GetSelectedIndex();
    if (nIndex >= 0 && nIndex < m_nFriendCount)
    {
        m_idBDUser = sgptFriendsArray[nIndex].m_idBDUser;
        m_szFriendName = sgptFriendsArray[nIndex].m_szUsername;
        m_bHaveFriend = TRUE;
        m_tMailItemsView.SetHeaderText(LocalizedSprintf(StringListFetch("X1_ConsoleMail:SendTo"), m_szFriendName.str()));
    }
    else
    {
        m_bHaveFriend = FALSE;
    }
}

void PlayerMailScreen::OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType)
{
    m_tItemsProvider.SetFilter(eNewFilterType);
    m_tCraftingMatItemsProvider.SetFilter(eNewFilterType);
    m_tEquippedItemProvider.SetFilter(eNewFilterType);
    m_tUnmailedItemProvider.SetFilter(eNewFilterType);
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    if (eNewFilterType == FILTER_BAG)
        ShowTopLevelCategories();
    else
        m_tListView.ForceUpdate(RESET_SELECTION);
}

inline void PlayerMailScreen::ShowTopLevelCategories()
{
    m_eCategoryState = BROWSE_CATEGORY_STATE;
    m_eCurrentCategory = CATEGORY_NONE;
    PopulateTopLevelCategories(m_tCategoriesView);
    if (GetPrimaryView() != &m_tCategoriesView)
        SetPrimaryView(&m_tCategoriesView);
    else
        m_tCategoriesView.ForceUpdate(RESET_SELECTION);
}

inline void UnmailedItemProvider::SetFilter(const UIFilterType eFilterType)
{
    UIInventoryViewItemsProvider::SetFilter(eFilterType);
    Update();
}

void PlayerMailScreen::UpdateCompareActions(::Console::ControllerActionManager & tActionManager)
{
    if (m_bCurrentItemIsEquippable)
    {
        tActionManager.SetAction(4, ACTION_EQUIP, StringListFetch("ConsoleInventory:Equip").str());
    }
    tActionManager.SetAction(7, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
}

inline BOOL PlayerMailScreen::ShowingBagUI()
{
    return GetFilter() == FILTER_BAG && m_eCategoryState != BROWSE_GEMS_STATE && m_eCategoryState != BROWSE_ITEMS_STATE && m_eCategoryState != BROWSE_CRAFTING_MATS_STATE;
}

inline BOOL PlayerMailScreen::CanOpenSelectedCategory()
{
    ItemCategory eCategory = m_tCategoriesView.GetSelectedItem();
    BOOL bHasItems = CategoryHasItems(eCategory, m_tUnmailedItemProvider, FALSE);
    return CategoryHasCraftingMatItems(eCategory, m_tUnmailedItemProvider, m_tCraftingMatItemsProvider) || bHasItems;
}










void sGiveGold(const ACDID idACDPlayerDst, const uint64 uAmount)
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_GOLD_SEND), cgtPSODefaults);
    PlayerMailScreen * ptScreen = sgptMailGlobals;
    if (ptScreen->m_eMailState == PlayerMailScreen::MAILSTATE_SENDING_MAIL)
    {
        ptScreen->m_nGoldToSend = uAmount;
        ptScreen->UpdatePlayerGold();
        ptScreen->UpdateGoldDisplay();
    }
}

void MailItemProvider::RemoveItem(const ACDID idItem)
{
    if (idItem == ACDID_INVALID)
        return;

    for (uint32 i = 0; i < m_arMailItems.size(); i++)
    {
        if (m_arMailItems[i] == idItem)
        {
            ActorCommonData * ptACD = ACDTryToGet(idItem);
            if (ptACD && CurrencyFromGBID(ptACD->hGB.gbid) != CURRENCY_INVALID)
                ACDDelete(idItem, FALSE);
            m_arMailItems.erase(m_arMailItems.begin() + i);
            return;
        }
    }
}

void PlayerMailScreen::UpdateCompareItem()
{
    Player * ptPlayer = PlayerGetByControllerIndex(GetControllerIndex());
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    ActorCommonData * ptACDItem = ACDTryToGet(m_tRightSingleItemProvider.GetItems()[0]);
    int32 nNumSlots = 0;
    InventorySlotType eValidSlots[4];
    ACDInventoryWhereCanThisGo(*ptACDPlayer, ptACDItem->hGB.gbid, nNumSlots, eValidSlots);
    if (nNumSlots > 0)
        m_tEquippedItemProvider.SetFilter(GetFilterForSlot(eValidSlots[0]));
    m_tEquippedView.ForceUpdate(RESET_SELECTION);
}

void PlayerMailScreen::SwitchToCompareMode()
{
    m_eMailState = MAILSTATE_COMPARING_ITEMS;
    UIInventoryView * ptView = GetPrimaryViewForMode(GetBrowsingMode());
    SetPrimaryView(ptView);
    SetFocusedView(ptView);
    UI_GetTextFromID(ptView->GetStatusLineUIID()).SetActive(FALSE);
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    UIID idInbox = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MainInbox");
    UI_GetWindowFromID(idInbox).SetActive(FALSE);
    UIID idGiftPane = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackGrounds.GiftPane");
    UI_GetWindowFromID(idGiftPane).SetActive(FALSE);
    UIID idGoldPane = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackGrounds.GoldPane");
    UI_GetWindowFromID(idGoldPane).SetActive(FALSE);
    m_tLeftDetailsView.Show(TRUE);
}

void PlayerMailScreen::OnButton3(int32 nControllerIndex)
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(5);
    switch (idAction)
    {
    case ACTION_COMPARE:
        if (!m_bShowingGift && !m_bShowingGold && !m_bShowingCraftingMaterial)
        {
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
            UpdateCompareItem();
            SwitchToCompareMode();
        }
        break;
    case ACTION_ADDGOLD:
    {
        Player * ptPlayer = PlayerGetByControllerIndex(GetControllerIndex());
        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        uint64 uAmount = tACDPlayer.GetCurrencyAmount(0);
        uint64 uAmountToSend = uAmount < 1000 ? uAmount : 1000;
        UIStackSplit::Console::Open(ptPlayer->idPlayerACD, uAmountToSend, uAmount, sGiveGold, UIStackSplit::Console::STACKSPLIT_GOLD_MAIL);
        break;
    }
    }
}

void PlayerMailScreen::OnDPadLeft(int32 nControllerIndex)
{
    if (m_eMailState == MAILSTATE_SENDING_MAIL && GetSecondaryView()->IsFocused() && CanSwitchFocusToView(GetPrimaryView()))
    {
        SetFocusedView(GetPrimaryView());
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

void PlayerMailScreen::OnDPadRight(int32 nControllerIndex)
{
    if (m_eMailState == MAILSTATE_SENDING_MAIL && GetPrimaryView()->IsFocused() && CanSwitchFocusToView(GetSecondaryView()))
    {
        SetFocusedView(GetSecondaryView());
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

void PlayerMailScreen::ConfirmGiveItemToPlayer(::Console::Online::MailRef ptMail, int32 nControllerIndex, BOOL bEquip)
{
    const D3::Console::MailItem & tContents = ptMail->GetContents();
    if (tContents.has_item_mail())
    {
        const D3::Items::Generator & tGenerator = tContents.item_mail().item_generator();
        Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
        if (bEquip)
            MessageSendToServer(CMSG_MAIL_ITEM_RECEIVED_EQUIP, tGenerator, ptPlayer);
        else
            MessageSendToServer(CMSG_MAIL_ITEM_RECEIVED, tGenerator, ptPlayer);
        return;
    }

    if (tContents.has_gold_amount())
    {
        CurrencyMessage tMessage;
        tMessage.eCurrency = CURRENCY_GOLD;
        tMessage.uAmount = tContents.gold_amount();
        MessageSendToServer(CMSG_MAIL_CURRENCY_RECEIVED, &tMessage, sizeof(tMessage), PlayerGetByControllerIndex(nControllerIndex));
    }
    else if (tContents.has_crafting_material_type() && tContents.has_crafting_material_amount())
    {
        CurrencyMessage tMessage;
        tMessage.eCurrency = tContents.crafting_material_type();
        tMessage.uAmount = tContents.crafting_material_amount();
        MessageSendToServer(CMSG_MAIL_CURRENCY_RECEIVED, &tMessage, sizeof(tMessage), PlayerGetByControllerIndex(nControllerIndex));
    }
}

void sGiftReceivable(::Console::Online::MailResult eResult, ::Console::Online::MailRef)
{
    if (!sgptMailGlobals)
        return;
    if (eResult == ::Console::Online::MAIL_SUCCESS)
        sgptMailGlobals->RedeemCurrentGift();
    else
        sgptMailGlobals->ShowMailGiftError();
}





















void sHandleMailError(::Console::Online::MailResult eResult, int32 nControllerIndex)
{
    switch (eResult)
    {
    case ::Console::Online::MAIL_FAILURE_NOT_CONNECTED:
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszMessage = StringListFetch("X1_ConsoleMail:NoConnection");
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sMailConnectionErrorAccept>();
        s_idModal = ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        break;
    }
    case ::Console::Online::MAIL_FAILURE:
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszMessage = StringListFetch("X1_ConsoleMail:MailConnectionError");
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sMailConnectionErrorAccept>();
        s_idModal = ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        break;
    }
    default:
        break;
    }
}


void sMailItemDeleteConfirmed_Equip(::Console::Online::MailResult eResult, ::Console::Online::MailRef ptMail)
{
    if (sgptMailGlobals && sgptMailGlobals->IsActive())
    {
        int32 nControllerIndex = sgptMailGlobals->GetControllerIndex();
        if (eResult != ::Console::Online::MAIL_SUCCESS || nControllerIndex == -1)
        {
            sHandleMailError(eResult, nControllerIndex);
            return;
        }
        sgptMailGlobals->ConfirmGiveItemToPlayer(ptMail, nControllerIndex, TRUE);
    }
}


void sMailDeleteConfirmed(::Console::Online::MailResult eResult, ::Console::Online::MailRef ptMail)
{
    if (sgptMailGlobals && sgptMailGlobals->IsActive())
    {
        int32 nControllerIndex = sgptMailGlobals->GetControllerIndex();
        if (eResult != ::Console::Online::MAIL_SUCCESS || nControllerIndex == -1)
        {
            sHandleMailError(eResult, nControllerIndex);
            return;
        }
        sgptMailGlobals->ConfirmGiveItemToPlayer(ptMail, nControllerIndex, FALSE);
    }
}

inline void sMailSendErrorAccept(const UIID &)
{
    ::Console::Screens::CloseScreenSet(TRUE, FALSE);
}

void PlayerMailScreen::ShowMailGiftError()
{
    NotificationCreationParams tParams;
    tParams.nControllerIndex = GetControllerIndex();
    tParams.uszMessage = StringListFetch("X1_ConsoleMail:MaiGiftRetrievalError");
    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sMailSendErrorAccept>();
    s_idModal = ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
}

void sMailSendConfirmed(::Console::Online::MailResult eResult)
{
    if (sgptMailGlobals && sgptMailGlobals->IsActive())
    {
        sgptMailGlobals->OnMailSendConfirmed(eResult);
    }
}

void OnMailUpdate()
{
    if (sgptMailGlobals && sgptMailGlobals->IsActive())
    {
        sgptMailGlobals->Update();
    }
}

void OnMailButton(const UIID &, const int32 nControllerIndex, const InputButton eInputButton)
{
    if (sgptMailGlobals && sgptMailGlobals->IsActive())
    {
        sgptMailGlobals->HandleButtonInput(nControllerIndex, eInputButton);
    }
}

void OnMailLeftStick(const UIID &, const int32 nControllerIndex)
{
    if (sgptMailGlobals && sgptMailGlobals->IsActive())
    {
        if (sgptMailGlobals->RingHasFocus() || !sgptMailGlobals->HandleLeftStickViewSwitching(nControllerIndex))
        {
            sgptMailGlobals->HandleLeftStickInput(nControllerIndex);
        }
    }
}

}  // namespace Console
}  // namespace UIInventory

namespace UIMail {
namespace Console {
using namespace UIInventory::Console;

void InitializeForGame()
{
    if (GameIsStartUp())
        return;

    sgptMailGlobals = new PlayerMailScreen();
    sgpMailArray = new ::Console::Online::MailArray;
    sgptFriendsArray = new ::Console::Online::Friend[MAX_FRIENDLIST_FRIENDS];
}

void OpenForController(int32 nControllerIndex)
{
    sgptMailGlobals->UIInventoryScreen::OpenForController(nControllerIndex);
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer)
    {
        UIFastEquip::Console::CloseForPlayer(ptPlayer->nPlayerIndex);
        ::Console::Screens::EnableScreen(::Console::Screens::MAIL_SCREEN_SET, 1, !(sgtAppGlobals.dwGameCreateParamFlags & (1 << GAMECREATEPARAM_SEASON)));
        Player * ptPrimaryPlayer = PlayerGetByControllerIndex(UI_GetPrimaryUIController());
        UpdateNavBarTabState(ptPrimaryPlayer, UIDLG_INVENTORY_CONSOLE, "LayoutRoot.Navigation.MailNavigation", FALSE);
        UpdateNavBarTabState(ptPrimaryPlayer, UIDLG_INVENTORY_CONSOLE, "LayoutRoot.Navigation.SeasonStashNavigation", FALSE);
    }
}

void Close()
{
    if (sgptMailGlobals->IsActive())
    {
        sgptMailGlobals->UIInventoryScreen::Close();
    }
}

void DisposeForGame()
{
    if (GameIsStartUp())
        return;

    if (s_idModal != -1)
    {
        if (::Console::Screens::GetModalNotificationManager().IsNotificationShowing(s_idModal))
            ::Console::Screens::GetModalNotificationManager().RemoveNotification(s_idModal);
        s_idModal = -1;
    }
    sgptMailGlobals->UnsubscribeFromNewMail();
    delete sgpMailArray;
    sgpMailArray = NULL;
    ::delete sgptMailGlobals;
    sgptMailGlobals = NULL;
    delete[] sgptFriendsArray;
}

void PlayerGiftRedeemed(Player * ptReceivingPlayer, D3::Items::Generator & tItemData)
{
    if (sgptMailGlobals && sgptMailGlobals->IsActive())
    {
        sgptMailGlobals->OnGiftRedeemed(ptReceivingPlayer, tItemData);
    }
}

void PlayerGiftFailed(Player * ptReceivingPlayer)
{
}

static void sPlayerGiftSendSuccess(::Console::Online::MailResult eResult);


void SendGift(const ActorCommonData & tACD, int32 nControllerIndex);  // defined below on its DWARF line 3976









static void sPlayerGiftSendSuccess(::Console::Online::MailResult eResult)
{
    if (eResult != ::Console::Online::MAIL_SUCCESS)
    {
        sHandleMailError(eResult, sidGiftToSend.nControllerIndex);
        return;
    }

    ANNDataMessage tMessage;
    tMessage.ann = ANNFromACD(sidGiftToSend.idACDGift);
    MessageSendToServer(CMSG_PLAYER_GIFT_SENT, &tMessage, sizeof(tMessage), PlayerGetByControllerIndex(sidGiftToSend.nControllerIndex));
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_SENT), cgtPSODefaults);
}

inline void sPlayerGiftConfirmSend(const UIID &)
{
    if (sidGiftToSend.idACDGift == ACDID_INVALID)
        return;

    ActorCommonData * ptACD = ACDTryToGet(sidGiftToSend.idACDGift);
    if (ptACD)
        SendGift(*ptACD, sidGiftToSend.nControllerIndex);
}

inline void sPlayerGiftCancelSend(const UIID &)
{
    sidGiftToSend.idACDGift = ACDID_INVALID;
    sidGiftToSend.nControllerIndex = -1;
}

void ConfirmPlayerGiftSend(ActorCommonData & tACDItem, Player * ptPlayer)
{
    if (::Console::GamerProfile::IsSignedInOnline(ptPlayer->nControllerIndex))
    {
        sidGiftToSend.idACDGift = tACDItem.id;
        sidGiftToSend.nControllerIndex = ptPlayer->nControllerIndex;
        NotificationCreationParams tParams;
        tParams.nControllerIndex = sidGiftToSend.nControllerIndex;
        tParams.uszMessage = LocalizedSprintf(StringListFetch("X1_ConsoleMail:SendPlayerGiftWarning"), tACDItem.ptFriendGiftData->sNameRecipient.str());
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sPlayerGiftConfirmSend>();
        tParams.tCallbacks.pfnOnClickCancelButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sPlayerGiftCancelSend>();
        s_idModal = ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
    }
    else
    {
        sidGiftToSend.idACDGift = ACDID_INVALID;
        sidGiftToSend.nControllerIndex = -1;
        NotificationCreationParams tParams;
        tParams.nControllerIndex = ptPlayer->nControllerIndex;
        tParams.uszMessage = StringListFetch("X1_ConsoleMail:ErrorNotOnline");
        s_idModal = ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
    }
}

}  // namespace Console
}  // namespace UIMail

static void sOnUpdateCategoryUI(const UIID & idItemUI, const UIInventory::Console::ItemCategory eCategory)
{
    using namespace UIInventory::Console;
    if (sgptMailGlobals && sgptMailGlobals->IsActive())
    {
        sgptMailGlobals->UpdateCategoryUI(idItemUI, eCategory);
    }
}

namespace UIInventory {
namespace Console {
inline void PlayerMailScreen::UpdateCategoryUI(const UIID & idItemUI, const ItemCategory eCategory)
{
    BOOL bEnabled = CategoryHasItems(eCategory, m_tUnmailedItemProvider, FALSE);
    if (eCategory == CATEGORY_CRAFTING && CategoryHasCraftingMatItems(eCategory, m_tUnmailedItemProvider, m_tCraftingMatItemsProvider))
        bEnabled = TRUE;
    UpdateCommonCategoryUI(idItemUI, eCategory, bEnabled, FALSE, FALSE);
}
}  // namespace Console
}  // namespace UIInventory

// Raw DWARF only records PresenceManager.cpp:4105; no recovered header declaration.
namespace Console { namespace Online { uint32 PresenceGetFriends(int32 nUserIndex, Friend * ptFriends, uint32 uMaxFriends); } }
namespace UIInventory { namespace Console {
#line 2960 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::UpdateFriendList()
{

    if (!CheckLobbyConnection())
        return;



    m_tFriendsListDataSource.nCount = 0;

    m_nFriendCount = ::Console::Online::PresenceGetFriends(m_nControllerIndex, sgptFriendsArray, MAX_FRIENDLIST_FRIENDS);

    if (m_nFriendCount > 0)
    {
        for (int32 i = 0; i < m_nFriendCount; ++i)
        {
            FriendListUIEntry & tUIEntry = m_tFriendsListDataSource.m_tEntryUI[i];
            tUIEntry.sDisplayName = sgptFriendsArray[i].m_szUsername;

            tUIEntry.bIsFemale = sgptFriendsArray[i].m_tUserPresence.m_bFemale;
            tUIEntry.ePlayerClass = sgptFriendsArray[i].m_tUserPresence.m_eClass;

            ++m_tFriendsListDataSource.nCount;
        }


        UIConsoleListBox & tFriendsList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDSLIST);
        tFriendsList.ResetAllItems();
        tFriendsList.SetDataSource(m_tFriendsListDataSource);
        tFriendsList.SetActive(TRUE);
        tFriendsList.RefreshVisibleItems();
        tFriendsList.SetSelectedIndex(0);
    }
    else
    {
        UIConsoleListBox & tFriendsList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDSLIST);
        tFriendsList.SetActive(FALSE);
        tFriendsList.RefreshVisibleItems();
        tFriendsList.SetSelectedIndex(-1);
    }
}
} }

// Raw DWARF only records MailService.cpp:1311; no recovered header declaration.
namespace Console { namespace Online { void MailDelete(int32 nUserIndex, uint64 idMail, DeleteMailCallback pfnCallback); } }
namespace UIInventory { namespace Console {
#line 647 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void UIMailListView::ClearItems()
{

    m_tDataSource.nMailItems = 0;
    m_tDataSource.ClearAll();

}
#line 3487 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::DeleteFakeItem()
{

    for (int32 n = 0; n < m_nSocketsFilled; ++n)
    {
        if (m_idACDSocketed[n] != ACDID_INVALID)
        {
            ACDDelete(m_idACDSocketed[n], FALSE);

            m_idACDSocketed[n] = ACDID_INVALID;
        }
    }

    if (m_idACDCurrentItem != ACDID_INVALID)
    {
        ACDDelete(m_idACDCurrentItem, FALSE);

        m_idACDCurrentItem = ACDID_INVALID;
    }
}
#line 2854 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::UpdateDisplayedMail()
{

    m_tMailListView.ClearItems();

    m_nMailItemCount = sgpMailArray->size();

    if (m_nMailItemCount > MAX_MAIL_ITEMS)
        m_nMailItemCount = MAX_MAIL_ITEMS;



    DeleteFakeItem();

    for (const ::Console::Online::MailRef & ptMailRef : *sgpMailArray)
    {
        BOOL bValid = m_tMailListView.AddItem(ptMailRef);
        if (!bValid)
        {
            ::Console::Online::MailDelete(m_nControllerIndex, ptMailRef->GetID(), ::Console::Online::DeleteMailCallback());
        }
    }

    UpdateInboxDisplay();
}
} }

namespace UIInventory { namespace Console {
#line 2738 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::UpdateSendMailActions(::Console::ControllerActionManager & tActionManager)
{

    if (GetPrimaryView()->IsFocused() && ShowingBagUI())
    {
        UpdateBagItemActions(tActionManager);
    }
    else
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT, StringListFetch("ConsoleUI:Select").str());
    }
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
    if (m_nGoldToSend > 0)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_ADDGOLD, StringListFetch("X1_ConsoleMail:ModifyGold").str());
    }
    else
    {
        Player * ptPlayer = PlayerGetByControllerIndex(m_nControllerIndex);
        if (ptPlayer)
        {
            ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
            uint64 uAmount = tACDPlayer.GetCurrencyAmount(CURRENCY_GOLD);
            if (uAmount > 0)
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_ADDGOLD, StringListFetch("X1_ConsoleMail:AddGold").str());
            }
        }
    }

    if (m_tMailItemProvider.GetCount() > 0 || m_nGoldToSend > 0)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_SEND, StringListFetch("X1_ConsoleMail:Send").str());
    }
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_STICK_BUTTON, ACTION_CHANGE, StringListFetch("X1_ConsoleMail:ChangeRecipient").str());
}
} }

namespace UIInventory { namespace Console {
#line 681 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
BOOL UIMailListView::AddItem(
    const ::Console::Online::MailRef & ptMailRef)
{
    const D3::Console::MailItem & tMailItem = ptMailRef->GetContents();

    Blizzard::Time::Timestamp nExpiration = ptMailRef->GetExpiration();

    if (nExpiration <= 0)
        return FALSE;



    if (tMailItem.has_item_mail())
    {
        const D3::Console::ItemMail & tItemDetails = tMailItem.item_mail();

        const D3::Items::Generator & tGenerator = tItemDetails.item_generator();

        ACDID idACDItem = CItemGenerate(static_cast<ACDNetworkName>(9588), tGenerator, NULL);

        if (idACDItem == ACDID_INVALID)
            return FALSE;



        MailUIEntry & tUIEntry = m_tDataSource.tEntryUI[m_tDataSource.nMailItems++];



        tUIEntry.sDisplayText = CRefString(ptMailRef->GetSender());

        tUIEntry.sExpirationText = sGetExpirationfromTimestamp(nExpiration);

        tUIEntry.ptMailRef = ptMailRef;

        ACDDelete(idACDItem, FALSE);
    }
    else if (tMailItem.has_gift_mail() || tMailItem.has_gold_amount() || tMailItem.has_crafting_material_type())
    {
        MailUIEntry & tUIEntry = m_tDataSource.tEntryUI[m_tDataSource.nMailItems++];



        tUIEntry.sDisplayText = CRefString(ptMailRef->GetSender());

        tUIEntry.ptMailRef = ptMailRef;

        tUIEntry.sExpirationText = sGetExpirationfromTimestamp(nExpiration);
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}
} }

namespace UIInventory { namespace Console {
#line 1645 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::SetInboxActive(
    BOOL bActive)
{
    UIID idMail = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MainInbox");
    UI_GetWindowFromID(idMail).SetActive(bActive);


    if (!bActive)
    {
        UIID idGiftPane = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackGrounds.GiftPane");
        UI_GetWindowFromID(idGiftPane).SetActive(FALSE);

        UIID idGoldPane = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackGrounds.GoldPane");
        UI_GetWindowFromID(idGoldPane).SetActive(FALSE);
    }
}
#line 1662 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::SetMailboxActive(
    BOOL bActive)
{
    UIID idMailBox = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailRing");
    UI_GetWindowFromID(idMailBox).SetActive(bActive);
}
#line 1702 "X/Client/UserInterface/Console/UIConsoleMail.cpp"






#line 1714 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::ShowReceivedGiftDetails(
    BOOL bShow)
{
    UIID idItem = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackgrounds.GiftPaneFocus");
    UI_GetWindowFromID(idItem).SetActive(bShow);
}
#line 2893 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::UpdateInboxDisplay()
{

    ShowReceivedGiftDetails(FALSE);
    SetFriendListActive(FALSE);

    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(LocalizedSprintf(StringListFetch("X1_ConsoleMail:Inbox")), FALSE);

    if (m_nMailItemCount > 0)
    {
        SetInboxActive(TRUE);
        SetNoMailDisplay(FALSE, NOMAILREASON_NO_MAIL);
    }
    else
    {
        SetInboxActive(FALSE);
        if (m_eMailState != MAILSTATE_VIEWING_RECEIVED_GIFT && m_eMailState != MAILSTATE_WAIT_FOR_GIFT_RECEIPT_CONFIRM)
        {
            SetNoMailDisplay(TRUE, NOMAILREASON_NO_MAIL);
        }
    }

    SetMailboxActive(TRUE);





    UIInventoryView * ptItemBrowsingView = GetSecondaryViewForMode(GetBrowsingMode());
    SetSecondaryView(ptItemBrowsingView);

    UIInventoryView * ptMailBrowsingView = GetPrimaryViewForMode(GetBrowsingMode());
    SetPrimaryView(ptMailBrowsingView);

    if (GetFocusedView() != ptMailBrowsingView)
    {
        SetFocusedView(ptMailBrowsingView);
    }

    m_tLeftDetailsView.Show(FALSE);

    if (m_bShowingGift || m_bShowingGold || !m_nMailItemCount)
    {
        m_tRightDetailsView.Show(FALSE);
    }
    else
    {
        m_tRightDetailsView.Show(TRUE);
    }

    const UIID & idStatusLine = m_tRightDetailsView.GetStatusLineUIID();
    UI_GetTextFromID(idStatusLine).SetText("", FALSE);

    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_GOLD).SetText("", FALSE);

    m_tMailListView.Show(m_nMailItemCount > 0);

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}
} }

namespace UIInventory { namespace Console {
#line 790 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
inline void MailItemProvider::RemoveAllItems()
{

    for (int32 n = 0; n < m_arMailItems.size(); ++n)
    {
        ACDID idItemToRemove = m_arMailItems[n];
        ActorCommonData * ptACDItem = ACDTryToGet(idItemToRemove);

        if (ptACDItem && CurrencyFromGBID(ptACDItem->hGB.gbid) != CURRENCY_INVALID)
        {
            ACDDelete(idItemToRemove, FALSE);
        }
    }

    m_arMailItems.clear();

}
#line 2198 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::OnButton2(
    int32 nControllerIndex)
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2);
    if (idAction == ACTION_CLOSE)
    {
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);
        m_eMailState = MAILSTATE_INVALID;
    }
    else if (idAction == ACTION_BACK)
    {
        switch (m_eMailState)
        {
        case MAILSTATE_SENDING_MAIL:
            if (RingHasFocus())
            {
                m_tMailItemProvider.RemoveAllItems();
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_BACK), cgtPSODefaults); SwitchToInboxMode();
            }
            else
            {

                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);

                if (m_eCategoryState != BROWSE_CATEGORY_STATE)
                {
                    CloseCategory();
                }
                else
                {
                    CloseSelectedSlot();
                }
                ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
            }
            break;
        case MAILSTATE_COMPARING_ITEMS:
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults); SwitchToInboxMode();
            break;

        case MAILSTATE_FRIEND_SELECT:
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_SELECT), cgtPSODefaults); SwitchToInboxMode();
            break;

        case MAILSTATE_FRIEND_UPDATE:
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_SELECT), cgtPSODefaults);
            SwitchToSendMailMode(FALSE);
            break;
        default:
            break;
        }
    }
}
} }

namespace UIInventory { namespace Console {
// Raw definition only at UIConsoleInventoryViewUtilities.cpp:1867; no recovered header record.
void UpdatePlayerInfoUI(const Player & tPlayer, const UIID & idParentControl, VisualInventory & tVisualInventory);
#line 3077 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::SwitchToSendMailMode(
    BOOL bResetMail)
{
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING_CURSOR).SetActive(TRUE);
    EnableRing(TRUE);

    ::Console::UIRActorManager::SetActive(TRUE);
    VisualInventory tVisualInventory;
    Player * ptPlayer = PlayerGetByControllerIndex(m_nControllerIndex);
    if (ptPlayer)
    {
        UpdatePlayerInfoUI(*ptPlayer, UIWND_INVENTORY_CONSOLE_ROOT, tVisualInventory);
    }

    SetInboxActive(FALSE);
    SetNoMailDisplay(FALSE, NOMAILREASON_NO_MAIL);
    SetFriendListActive(FALSE);
    SetMailboxActive(FALSE);

    UpdatePlayerGold();
    UpdateGoldDisplay();

    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(LocalizedSprintf(StringListFetch("X1_ConsoleMail:SendMail")), FALSE);

    m_eMailState = MAILSTATE_SENDING_MAIL;

    UIInventoryView * ptInventoryView = GetPrimaryViewForMode(GetBrowsingMode());
    SetPrimaryView(ptInventoryView);

    UIInventoryView * ptMailItemsView = GetSecondaryViewForMode(GetBrowsingMode());
    SetSecondaryView(ptMailItemsView);

    if (bResetMail)
    {
        m_tMailItemProvider.RemoveAllItems();
        m_tUnmailedItemProvider.Update();
        m_tCraftingMatItemsProvider.Refresh();
    }

    ResetRing();
    UpdateRingIcons();

    const UIID & idStatusLine = ptInventoryView->GetStatusLineUIID();

    UI_GetTextFromID(idStatusLine).SetText("", FALSE);

    SetFocusedView(NULL);

    UIInventoryView * ptMailItemView = GetSecondaryViewForMode(GetBrowsingMode());
    SetSecondaryView(ptMailItemView);

    SetFilterType(GetInitialRingSelection(FILTER_PLAYER_START, FILTER_PLAYER_END));

    RequestUpdate(RESET_SELECTION);

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

}
} }

// Raw API records name ActorInventory.cpp:3969 only; no recovered header declaration.
void ACDAddToInventory(ActorCommonData & tACDItem, const InventoryLocation & tLocation, const BOOL bMarkItemAsDirty, InventorySlotType eCameFrom);
namespace UIInventory { namespace Console {
#line 3451 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
ACDID PlayerMailScreen::GenerateFakeItem(
    const D3::Items::Generator & tGenerator)
{
    DeleteFakeItem();

    m_idACDCurrentItem = CItemGenerate(static_cast<ACDNetworkName>(9588), tGenerator, NULL);

    if (m_idACDCurrentItem != ACDID_INVALID)
    {
        ActorCommonData & tACDItem = ACDGet(m_idACDCurrentItem);

        m_nSocketsFilled = 0;
        for (int32 i = 0; i < tGenerator.contents_size(); ++i)
        {
            const D3::Items::EmbeddedGenerator & tEmbedded = tGenerator.contents(i);

            m_idACDSocketed[i] = CItemGenerate(static_cast<ACDNetworkName>(9589 + i), tEmbedded.generator(), NULL);
            ActorCommonData * ptACDGem = ACDTryToGet(m_idACDSocketed[i]);
            if (ptACDGem)
            {
                InventoryLocation tLocation;
                tLocation.eSlot = SLOT_ITEM_SOCKETS;
                tLocation.pt.Set(0, m_nSocketsFilled);
                tLocation.idACDContainer = tACDItem.id;
                ACDAddToInventory(*ptACDGem, tLocation, FALSE, SLOT_NONE);
                ItemAddSocketedItemAttributes(*ptACDGem, tACDItem);
                FastAttribMerge(tACDItem.idFastAttrib, ptACDGem->idFastAttrib);
                ++m_nSocketsFilled;
            }
        }
        tACDItem.AttributesSetInt(ATTRIBUTE_SOCKETS_FILLED, m_nSocketsFilled);
    }

    return m_idACDCurrentItem;
}
} }

#include "../Controls/UIItemButtonControl.h"
#include "../../../Common/ActorInventory.h"
// Raw API records name UI.cpp:5384 and CActorInventory.cpp:169 only; no recovered header declarations.
UIItemButton & UI_GetItemButtonFromID(const UIID & id);
void ACDInventoryGetItemDrawInfoWithGBID(const ACDID idPlayer, const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType);
namespace UIInventory { namespace Console {
#line 3714 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::ShowReceivedGift(
    D3::Items::Generator & tItemData)
{

    m_eMailState = MAILSTATE_VIEWING_RECEIVED_GIFT;

    UIID idGiftPane = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackgrounds.GiftPane");
    UIID idGiftPaneFocus = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackgrounds.GiftPaneFocus");

    UI_GetWindowFromID(idGiftPane).SetActive(FALSE);
    UI_GetWindowFromID(idGiftPaneFocus).SetActive(TRUE);

    SetFocusedView(NULL);

    m_bShowingGift = FALSE;

    ACDID idACDItem = GenerateFakeItem(tItemData);

    if (idACDItem != ACDID_INVALID)
    {
        ActorCommonData & tACDItem = ACDGet(idACDItem);

        UIID idItemName = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackgrounds.GiftPaneFocus.Item.ItemName");
        UIID idItemButton = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackGrounds.GiftPaneFocus.Item.ItemVisual");

        UIItemButton & tItemButton = UI_GetItemButtonFromID(idItemButton);

        tItemButton.SetACDID(idACDItem);
        tItemButton.SetActive(TRUE);

        ImageTextureFrame tImageTexFrame;

        Player * ptPlayer = PlayerGetByControllerIndex(m_nControllerIndex);
        if (ptPlayer)
        {
            ACDInventoryGetItemDrawInfoWithGBID(ptPlayer->idPlayerACD, tACDItem.hGB.gbid, tImageTexFrame, ITEMDRAW_NORMAL);
            tItemButton.SetAllTextureFrames(tImageTexFrame);
        }

        UI_GetTextFromID(idItemName).SetText(ItemGetDisplayName(tACDItem, TRUE, FALSE, FALSE, TRUE), FALSE);

        if (ItemGetQualityLevel(tACDItem) == IQL_LEGENDARY)
        {
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_LEGENDARY_GIFT_RECEIVED), cgtPSODefaults);
        }
        else
        {
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_GIFT_RECEIVED), cgtPSODefaults);
        }

        ShowReceivedGiftDetails(TRUE);

        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
    else
    {
        SwitchToInboxMode();
    }

}
} }

namespace UIInventory { namespace Console {
#line 3691 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::OnGiftRedeemed(
    Player * ptReceivingPlayer,
    D3::Items::Generator & tItemData)
{

    if (m_nControllerIndex != ptReceivingPlayer->nControllerIndex ||
        m_eMailState != MAILSTATE_WAIT_FOR_GIFT_RECEIPT_CONFIRM)
    {
        return;
    }



    ::Console::Online::MailRef & tMailRef = (*sgpMailArray)[m_nLastGiftMailIndex];
    ::Console::Online::MailDelete(m_nControllerIndex, tMailRef->GetID(), sMailDeleteConfirmed);
    sgpMailArray->erase(sgpMailArray->begin() + m_nLastGiftMailIndex);

    UIID idGiftText = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackgrounds.GiftPane.GiftText");
    UI_GetTextFromID(idGiftText).SetActive(FALSE);

    ShowReceivedGift(tItemData);
}
} }

#include "../UIStoryboard.h"
// Raw API records name UI.cpp:674 and UI.cpp:5793 only; no recovered header declarations.
const UIID & UI_GetControllerFocus();
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
namespace UIInventory { namespace Console {
#line 175 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void MailViewDataSource::UpdateItemUI(
    const UIConsoleListBox & tListBox,
    const UIID & idItemUI,
    const int32 nIndex)
{
    if (!UI_TestID(idItemUI))
    {
        return;
    }

    if (tEntryUI[nIndex].ptMailRef.isNull())
    {
        return;
    }

    if (!tEntryUI[nIndex].ptMailRef->AreContentsValid())
    {
        return;
    }

    UIID idSenderName = UIIDCreateFromParent(idItemUI, ".LayoutRoot.SenderName");
    UIID idExpiration = UIIDCreateFromParent(idItemUI, ".LayoutRoot.ExpirationDate");
    UIID idSelected = UIIDCreateFromParent(idItemUI, ".LayoutRoot.Selected");

    if (UI_TestID(idSelected))
    {
        BOOL bSelected = tListBox.GetSelectedIndex() == nIndex && UI_GetControllerFocus() == tListBox.GetID();
        UI_GetWindowFromID(idSelected).SetActive(bSelected);
    }

    if (UI_TestID(idSenderName))
    {
        UIText & tText = UI_GetTextFromID(idSenderName);
        tText.SetText(tEntryUI[nIndex].sDisplayText, FALSE);
    }

    if (UI_TestID(idExpiration))
    {
        UIText & tText = UI_GetTextFromID(idExpiration);
        tText.SetText(tEntryUI[nIndex].sExpirationText, FALSE);
    }

    UIID idItemEnvelope = UIIDCreateFromParent(idItemUI, ".LayoutRoot.Backgrounds.ItemBackground");
    UIID idGiftEnvelope = UIIDCreateFromParent(idItemUI, ".LayoutRoot.Backgrounds.GoldBackground");
    UIID idGoldEnvelope = UIIDCreateFromParent(idItemUI, ".LayoutRoot.Backgrounds.GiftBackground");

    if (UI_TestID(idItemEnvelope) && UI_TestID(idGiftEnvelope) && UI_TestID(idGoldEnvelope))
    {
        UIWindow & tItemEnvelope = UI_GetWindowFromID(idItemEnvelope);
        UIWindow & tGiftEnvelope = UI_GetWindowFromID(idGiftEnvelope);
        UIWindow & tGoldEnvelope = UI_GetWindowFromID(idGoldEnvelope);

        tItemEnvelope.SetActive(FALSE);
        tGiftEnvelope.SetActive(FALSE);
        tGoldEnvelope.SetActive(FALSE);

        MailUIEntry & tUIEntry = tEntryUI[nIndex];

        if (tUIEntry.ptMailRef.notNull() && tUIEntry.ptMailRef->AreContentsValid())
        {
            const D3::Console::MailItem & tMailItem = tUIEntry.ptMailRef->GetContents();

            if (tMailItem.has_item_mail() || tMailItem.has_crafting_material_type())
            {
                tItemEnvelope.SetActive(TRUE);
            }
            else if (tMailItem.has_gift_mail())
            {
                tGiftEnvelope.SetActive(TRUE);
            }
            else if (tMailItem.has_gold_amount())
            {
                tGoldEnvelope.SetActive(TRUE);
            }
        }
    }


    UIID idStoryboard = UIIDCreateFromParent(idItemUI, ".ItemListSelection");
    if (UI_TestID(idStoryboard))
    {
        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Stop();
        tStoryboard.Play(FALSE, 0.0f);
    }
}
} }

namespace UIInventory { namespace Console {
// Modal sentinel -1 is inferred literal spelling from original immediate instructions; use of the external CPP constant object is not established.
#line 1721 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::OnClose()

{

    InputClearRemappings();


    if (s_idModal != -1)
    {
        if (::Console::Screens::GetModalNotificationManager().IsNotificationShowing(s_idModal))
        {
            ::Console::Screens::GetModalNotificationManager().RemoveNotification(s_idModal);
        }

        s_idModal = -1;
    }

    SetPrimaryView(NULL);
    SetSecondaryView(NULL);

    DeleteFakeItem();

    SetInboxActive(FALSE);
    SetMailboxActive(FALSE);
    SetNoMailDisplay(FALSE, NOMAILREASON_CLOSE);
    SetFriendListActive(FALSE);
    ShowReceivedGiftDetails(FALSE);

    m_eMailState = MAILSTATE_INVALID;

    m_nMailItemCount = 0;
    m_tMailListView.ClearItems();

    ::Console::UIRActorManager::SetActive(FALSE);

    m_tItemsProvider.SetPlayer(NULL);
    m_tCraftingMatItemsProvider.SetPlayer(NULL);
    m_tEquippedItemProvider.SetPlayer(NULL);
    m_tRightSingleItemProvider.SetPlayer(NULL);
    m_tMailItemProvider.SetPlayer(NULL);
    m_tUnmailedItemProvider.SetPlayer(NULL);

    UnsubscribeFromNewMail();
}
} }

namespace UIInventory { namespace Console {
#line 413 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
static LPCSTR szClassIcons[15] =
{
    ".OfflineIcon",
    ".DemonHunter_Female",
    ".DemonHunter_Male",
    ".Barbarian_Female",
    ".Barbarian_Male",
    ".Wizard_Female",
    ".Wizard_Male",
    ".WitchDoctor_Female",
    ".WitchDoctor_Male",
    ".Monk_Female",
    ".Monk_Male",
    ".Crusader_Female",
    ".Crusader_Male",
    ".Necromancer_Female",
    ".Necromancer_Male",
};
void ShowOnly(
    const UIID & idParent,
    const int32 nIndexToShow)
{
    for (int32 i = 0; i < sizeof(szClassIcons) / sizeof(szClassIcons[0]); ++i)
    {
        UIID idEntry = UIIDCreateFromParent(idParent, szClassIcons[i]);

        if (UI_TestID(idEntry))
        {
            UI_GetWindowFromID(idEntry).SetActive(nIndexToShow != -1 && i == nIndexToShow);
        }
    }
}

void FriendListViewDataSource::UpdateItemUI(
    const UIConsoleListBox & tListBox,
    const UIID & idItemUI,
    const int32 nIndex)
{
    UIID idName, idSelected, idPortrait;
    idName.SetFromParent(idItemUI, "LayoutRoot.FriendName");
    idSelected.SetFromParent(idItemUI, "LayoutRoot.Selected");
    idPortrait.SetFromParent(idItemUI, "LayoutRoot.Portraits");

    BOOL bSelected = tListBox.GetSelectedIndex() == nIndex && UI_GetControllerFocus() == tListBox.GetID();

    UI_GetWindowFromID(idSelected).SetActive(bSelected);
    UI_GetWindowFromID(idName).SetActive(TRUE);

    CRefString sPortraitKey;
    if (m_tEntryUI[nIndex].ePlayerClass != PLAYER_CLASS_NONE)
    {
        int32 nClassGenderIndex = m_tEntryUI[nIndex].ePlayerClass * 2 + 1 + !m_tEntryUI[nIndex].bIsFemale;
        ShowOnly(idPortrait, nClassGenderIndex);
    }
    else
    {
        ShowOnly(idPortrait, 0);
    }

    CRefString sListText = m_tEntryUI[nIndex].sDisplayName;
    UIText & tText = UI_GetTextFromID(idName);
    tText.SetText(sListText, FALSE);


    UIID idStoryboard = UIIDCreateFromParent(idItemUI, ".ItemListSelection");
    if (UI_TestID(idStoryboard))
    {
        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Stop();
        tStoryboard.Play(FALSE, 0.0f);
    }
}
} }

namespace UIInventory { namespace Console {
#line 1702 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::SetFriendListActive(
    BOOL bActive)
{
    UIID idFriendList = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackGrounds.FriendList");
    UI_GetWindowFromID(idFriendList).SetActive(bActive);

    if (bActive)
    {
        UI_SetControllerFocus(UILIST_CONSOLE_FRIENDSLIST);
    }
}
#line 3032 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::SwitchToFriendSelectMode(
    BOOL bChangeRecipient)
{
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING_CURSOR).SetActive(FALSE);
    EnableRing(FALSE);
    ::Console::UIRActorManager::SetActive(FALSE);

    SetInboxActive(FALSE);
    SetNoMailDisplay(FALSE, NOMAILREASON_NO_MAIL);
    SetMailboxActive(TRUE);

    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(LocalizedSprintf(StringListFetch("X1_ConsoleMail:SelectFriend")), FALSE);

    m_eMailState = bChangeRecipient ? MAILSTATE_FRIEND_UPDATE : MAILSTATE_FRIEND_SELECT;

    UIInventoryView * ptPrimaryView = GetPrimaryViewForMode(GetBrowsingMode());
    SetPrimaryView(ptPrimaryView);

    UIInventoryView * ptSecondaryView = GetSecondaryViewForMode(GetBrowsingMode());
    SetSecondaryView(ptSecondaryView);

    SetFocusedView(NULL);

    SetFriendListActive(TRUE);
    UpdateFriendList();

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}
} }

namespace UIInventory { namespace Console {
#line 1607 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::OnOpen()

{
    if (PlayerIsOnline())
    {
        Player * ptPlayer = PlayerGetByControllerIndex(m_nControllerIndex);

        if (ptPlayer)
        {
            m_tItemsProvider.SetPlayer(ptPlayer);
            m_tCraftingMatItemsProvider.SetPlayer(ptPlayer);
            m_tEquippedItemProvider.SetPlayer(ptPlayer);
            m_tRightSingleItemProvider.SetPlayer(ptPlayer);
            m_tMailItemProvider.SetPlayer(ptPlayer);
            m_tUnmailedItemProvider.SetPlayer(ptPlayer);
        }

        const UIIDToFilterMap & tFilterMap = sgptMailGlobals->m_tFilterMap;
        UIID idKey;
        UIFilterType eFilterType;

        POS pos = tFilterMap.GetStartAssoc();
        while (pos)
        {
            tFilterMap.GetNextAssoc(pos, idKey, eFilterType);

            UIID idNewIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.NewStar");

            UI_GetWindowFromID(idNewIndicator).SetActive(FALSE);
        }

        SwitchToInboxMode();
        GetMail();

        ::Console::Online::GetNewMailEvent().Subscribe(OnlineService::internal::method_bind_0_1<void, PlayerMailScreen, PlayerMailScreen *, int32, &PlayerMailScreen::OnNewMail>(this));
    }
}
} }

namespace UIInventory { namespace Console {
#line 2283 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void sPlayerMailCancelSend(
    const UIID &)
{
}


void sPlayerMailConfirmSend(
    const UIID &)
{
    if (sgptMailGlobals && sgptMailGlobals->IsActive())
    {
        sgptMailGlobals->SendMailAndConfirm();
    }
}

void PlayerMailScreen::OnButton4(
    int32 nControllerIndex)
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_4);
    if (idAction == ACTION_CREATE)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_CREATE), cgtPSODefaults);
        SwitchToFriendSelectMode(FALSE);
    }
    else if (idAction == ACTION_SEND)
    {
        if (!CheckLobbyConnection())
            return;

        CRefString szMessageBody = LocalizedSprintf(StringListFetch("X1_ConsoleMail:ConfirmMailSend"), m_szFriendName.str());
        if (m_tMailItemProvider.GetCount() > 0)
        {
            szMessageBody += LocalizedSprintf(StringListFetch("X1_ConsoleMail:ConfirmMailSendItemCount"), UnicodeText::FormatNumber(m_tMailItemProvider.GetCount(), TRUE, FNS_NORMAL, XLOCALE_INVALID).str());
        }

        if (m_nGoldToSend != 0)
        {
            CRefString szColorizedString = CRefString("{c_green}") + UIText_ValueToGameCurrency(m_nGoldToSend, CURRENCY_GOLD) + CRefString("{/c_green}");
            szMessageBody += szColorizedString;
        }

        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszTitle = LocalizedSprintf(StringListFetch("X1_ConsoleMail:ConfirmMailSendTitle"));
        tParams.uszMessage = szMessageBody;
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sPlayerMailConfirmSend>();
        tParams.tCallbacks.pfnOnClickCancelButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sPlayerMailCancelSend>();
        s_idModal = ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
    }

}
} }

namespace UIInventory { namespace Console {
#line 3517 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void sMailSendFailureConfirm(
    const UIID &)
{
    ::Console::Screens::CloseScreenSet(TRUE, FALSE);
}

BOOL PlayerMailScreen::GameIsHardcore()

{
    return PlayerIsHardcore(PlayerGetByControllerIndex(m_nControllerIndex));
}

void PlayerMailScreen::SendMailAndConfirm()

{
    SetFocusedView(NULL);

    m_eMailState = MAILSTATE_WAITING_FOR_SEND_COMPLETE;

    D3::Console::MailItem tMailItem;

    tMailItem.set_hardcore(GameIsHardcore());





    if (m_nGoldToSend != 0)
    {
        tMailItem.set_gold_amount(m_nGoldToSend);
    }
    else
    {
        const ACDIDArray & arItems = m_tMailItemProvider.GetItems();
        ActorCommonData * ptACDItem = ACDTryToGet(arItems[0]);

        if (ptACDItem)
        {
            CurrencyType eCurrency = CurrencyFromGBID(ptACDItem->hGB.gbid);
            if (eCurrency != CURRENCY_INVALID)
            {
                tMailItem.set_crafting_material_type(eCurrency);
                tMailItem.set_crafting_material_amount(ptACDItem->GetStackCount());
            }
            else
            {
                D3::Console::ItemMail * ptItemDetails = tMailItem.mutable_item_mail();
                D3::Items::Generator * tGenerator = ptItemDetails->mutable_item_generator();
                ItemGeneratorPopulateFromACDID(*ptACDItem, *tGenerator, TRUE);
            }
        }
        else
        {
            NotificationCreationParams tParams;
            tParams.nControllerIndex = m_nControllerIndex;
            tParams.uszMessage = LocalizedSprintf(StringListFetch("X1_ConsoleMail:ConfirmMailSendFailure"), m_szFriendName.str());
            tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sMailSendFailureConfirm>();
            s_idModal = ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
            return;
        }
    }


    ::Console::Online::MailSend(m_nControllerIndex,
        ::Console::Online::MAIL_TYPE_MESSAGE,
        tMailItem,
        sgptMailGlobals->m_idBDUser,
        &sMailSendConfirmed);

}
} }

#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
namespace UIInventory { namespace Console {
#line 3002 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::ReturnToInboxMode()

{
    SwitchToInboxMode();
}
#line 3589 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void sMailSendSucessConfirm(
    const UIID &)
{
    if (sgptMailGlobals && sgptMailGlobals->IsActive())
    {
        sgptMailGlobals->ReturnToInboxMode();
    }
}

void PlayerMailScreen::OnMailSendConfirmed(
    ::Console::Online::MailResult eResult)
{
    if (m_eMailState == MAILSTATE_INVALID)
        return;



    if (eResult == ::Console::Online::MAIL_SUCCESS)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_SENT), cgtPSODefaults);

        if (m_nGoldToSend != 0)
        {
            CurrencyMessage tMessage;
            tMessage.eCurrency = CURRENCY_GOLD;
            tMessage.uAmount = m_nGoldToSend;

            MessageSendToServer(CMSG_MAIL_CURRENCY_SENT, &tMessage, sizeof(tMessage), PlayerGetByControllerIndex(m_nControllerIndex));

            m_nGoldToSend = 0;
            UpdateGoldDisplay();
        }
        else
        {
            ACDIDArray & arItems = m_tMailItemProvider.GetItems();

            if (arItems.size() > 0)
            {
                ActorCommonData * ptACDItem = ACDTryToGet(arItems[0]);



                CurrencyType eCurrency = CurrencyFromGBID(ptACDItem->hGB.gbid);
                if (eCurrency != CURRENCY_INVALID)
                {
                    CurrencyMessage tMessage;
                    tMessage.eCurrency = eCurrency;
                    tMessage.uAmount = ptACDItem->GetStackCount();

                    ACDDelete(ptACDItem->id, FALSE);

                    MessageSendToServer(CMSG_MAIL_CURRENCY_SENT, &tMessage, sizeof(tMessage), PlayerGetByControllerIndex(m_nControllerIndex));
                }
                else
                {

                    ACDNetworkName annItem = ANNFromACD(arItems[0]);

                    StackPortionMessage tMessage;
                    tMessage.annItem = annItem;
                    tMessage.uAmount = (int32)ptACDItem->GetStackCount();

                    MessageSendToServer(CMSG_MAIL_ITEM_SENT, &tMessage, sizeof(tMessage), PlayerGetByControllerIndex(m_nControllerIndex));
                }


                blzext::vector_erase_at(arItems, 0);

                UIInventoryView * ptSecondary = GetSecondaryView();
                if (ptSecondary)
                {
                    ptSecondary->ForceUpdate(RESET_SELECTION);
                }
            }
        }

        ACDIDArray & arItems = m_tMailItemProvider.GetItems();

        if (arItems.size() > 0)
        {
            SendMailAndConfirm();
        }
        else
        {
            NotificationCreationParams tParams;
            tParams.nControllerIndex = m_nControllerIndex;
            tParams.uszMessage = LocalizedSprintf(StringListFetch("X1_ConsoleMail:ConfirmMailSendSuccess"), m_szFriendName.str());
            tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sMailSendSucessConfirm>();
            s_idModal = ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        }
    }
    else
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = m_nControllerIndex;
        tParams.uszMessage = LocalizedSprintf(StringListFetch("X1_ConsoleMail:ConfirmMailSendFailure"), m_szFriendName.str());
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sMailSendFailureConfirm>();
        s_idModal = ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
    }
}
} }

namespace UIInventory { namespace Console {
#line 897 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
inline void UnmailedItemProvider::RefreshView()

{
    if (!GetPlayer())
    {
        return;
    }

    ACDID idACDPlayer = GetPlayer()->idPlayerACD;



    m_tView.ClearItems();
    m_tView.AddItems(idACDPlayer, idACDPlayer, SLOT_PLAYER_BACKPACK, (1 << FilteredInventoryView::FIV_EXCLUDE_ACCOUNT_BOUND_BIT) | (1 << FilteredInventoryView::FIV_EXCLUDE_TIERED_RIFT_KEYS_BIT), IQL_INFERIOR);
}
#line 846 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
inline void UnmailedItemProvider::RemoveAllItems()

{
    for (int32 i = 0; i < FILTER_COUNT; ++i)
    {
        m_arDisplayItems[i].clear();
    }
}

inline void UnmailedItemProvider::Update()

{
    RefreshView();
    RemoveAllItems();

    for (int32 i = 0; i < FILTER_COUNT; ++i)
    {
        const ACDIDArray & tItems = m_tView.GetItems(i);
        const ACDIDArray & tMailItems = m_ptMailItemProvider->GetItemsWithFilter(i);

        for (uint32 j = 0; j < tItems.size(); ++j)
        {
            if (blz::find(tMailItems.begin(), tMailItems.end(), tItems[j]) == tMailItems.end())
            {
                m_arDisplayItems[i].push_back(tItems[j]);
            }
        }
    }
}
} }

// CurrencyGetGBID: raw Items.cpp:10429 only; exhaustive cached records have no header declaration.
GBID CurrencyGetGBID(const CurrencyType eCurrencyType);
namespace UIInventory { namespace Console {
#line 889 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
inline int32 UnmailedItemProvider::GetCount()

{
    return m_arDisplayItems[GetFilter()].size();
}
#line 938 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
inline int32 UnmailedCraftingMatItemsProvider::GetCount()

{
    return GetItemsWithFilter(GetFilter()).size();
}
#line 1970 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
ActorCommonData * GetCraftingMaterialItemInMailItemProvider(
    CurrencyType eCurrency)
{
    const ACDIDArray & arItems = sgptMailGlobals->m_tMailItemProvider.GetItems();
    int32 nNumItems = arItems.size();
    for (int32 i = 0; i < nNumItems; ++i)
    {
        ActorCommonData * ptACDItem = ACDTryToGet(arItems[i]);
        if (ptACDItem)
        {
            if (CurrencyFromGBID(ptACDItem->hGB.gbid) == eCurrency)
            {
                return ptACDItem;
            }
        }
    }

    return NULL;
}

void sGiveCraftingMaterials(
    const ACDID idACDPlayerDst,
    const uint64 uAmount)
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_GOLD_SEND), cgtPSODefaults);
    ActorCommonData * ptACDItem = ACDTryToGet(idACDPlayerDst);
    if (ptACDItem)
    {
        CurrencyType eCurrency = CurrencyFromGBID(ptACDItem->hGB.gbid);







        ActorCommonData * ptACDCraftingMatItem = GetCraftingMaterialItemInMailItemProvider(eCurrency);


        if (ptACDCraftingMatItem)
        {
            ptACDCraftingMatItem->AddToStackCount(uAmount, 0x7000000000000000ULL);
        }
        else
        {


            GBID gbidItem = CurrencyGetGBID(eCurrency);
            if (gbidItem == GBID_NONE)
            {
                return;
            }

            GBHandle hgbItem(GB_ITEMS, gbidItem);


            D3::Items::Generator tItem;
            ItemGeneratorToDefaults(tItem);
            hgbItem.Export(*tItem.mutable_gb_handle());

            tItem.set_seed(0);
            uint32 dwFlags = tItem.flags() | (1 << IGF_IDENTIFIED_BIT);
            tItem.set_flags(dwFlags);

            tItem.set_stack_size(uAmount);

            ACDNetworkName ann = static_cast<ACDNetworkName>(10632 + eCurrency);
            ACDID idACDItem = CItemGenerate(ann, tItem, NULL);

            sgptMailGlobals->m_tMailItemProvider.AddItem(idACDItem);
        }



        sgptMailGlobals->m_tUnmailedItemProvider.Update();
        sgptMailGlobals->m_tCraftingMatItemsProvider.Refresh();

        if (sgptMailGlobals->m_tUnmailedItemProvider.GetCount() == 0 &&
            sgptMailGlobals->m_tCraftingMatItemsProvider.GetCount() == 0)
        {
            if (sgptMailGlobals->m_tMailItemProvider.GetCount() > 0)
            {
                sgptMailGlobals->SetFocusedView(sgptMailGlobals->GetSecondaryView());
            }
            else
            {
                sgptMailGlobals->CloseSelectedSlot();
            }
        }
        sgptMailGlobals->GetPrimaryView()->ForceUpdate(KEEP_INDEX_SELECTED);
        sgptMailGlobals->GetSecondaryView()->ForceUpdate(KEEP_INDEX_SELECTED);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
    }
}
} }

namespace UIInventory { namespace Console {
#line 757 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
inline void MailItemProvider::AddItem(
    const ACDID idItem)
{
    if (idItem != ACDID_INVALID)
    {
        m_arMailItems.push_back(idItem);
    }
}
} }

namespace UIInventory { namespace Console {
#line 2403 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::OpenSelectedCategory()

{
    OpenCategory(m_tCategoriesView.GetSelectedItem());
}
#line 2065 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::OnButton1(
    int32 nControllerIndex)
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_1);
    switch (idAction)
    { case ACTION_TAKE:
        if (m_eMailState == MAILSTATE_BROWSE_MAIL)
        {
            TakeCurrentItem(FALSE);
        }
        else
        {
            UpdateDisplayedMail();
            SwitchToInboxMode();
        }
        break;



    case ACTION_SELECT:
        if (m_eMailState == MAILSTATE_FRIEND_SELECT || m_eMailState == MAILSTATE_FRIEND_UPDATE)
        {
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_SELECT), cgtPSODefaults);
            SetMailRecipient();
            if (m_bHaveFriend)
            {
                SwitchToSendMailMode(m_eMailState == MAILSTATE_FRIEND_SELECT);
            }
            else
            {
                SwitchToInboxMode();
            }
        }
        else if (m_eMailState == MAILSTATE_SENDING_MAIL)
        {
            UIInventoryView * ptView = GetFocusedView();
            if (RingHasFocus())
            {
                int32 nUnmailedItemsCount = m_tUnmailedItemProvider.GetCount();
                if (GetFilter() == FILTER_CRAFTING_MATERIALS || GetFilter() == FILTER_BAG)
                {
                    nUnmailedItemsCount += m_tCraftingMatItemsProvider.m_arrVirtualItems.size();
                }

                if (nUnmailedItemsCount > 0)
                {
                    OpenSelectedSlot();
                }
                else if (m_tMailItemProvider.GetCount() > 0)
                {
                    OpenSelectedSlot();
                    SetFocusedView(GetSecondaryView());
                }
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_SELECT), cgtPSODefaults);
            }
            else if (ptView == GetPrimaryView())
            {
                if (m_eCategoryState == BROWSE_CRAFTING_MATS_STATE)
                {
                    Player * ptPlayer = PlayerGetByControllerIndex(m_nControllerIndex);
                    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

                    ACDID idACDItem = ptView->GetSelectedItem();
                    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
                    if (ptACDItem)
                    {
                        CurrencyType eCurrency = CurrencyFromGBID(ptACDItem->hGB.gbid);
                        uint64 uAmount = tACDPlayer.GetCurrencyAmount(eCurrency) - GetAmountOfCraftingMaterialInMailItemProvider(eCurrency);
                        uint64 uAmountToSend = 0;
                        UIStackSplit::Console::Open(ptACDItem->id, uAmountToSend, uAmount, sGiveCraftingMaterials, UIStackSplit::Console::STACKSPLIT_CRAFTING_MAT_MAIL);
                    }
                }
                else
                {
                    m_tMailItemProvider.AddItem(ptView->GetSelectedItem());
                    m_tUnmailedItemProvider.Update();
                    if (m_tUnmailedItemProvider.GetCount() == 0 && m_tCraftingMatItemsProvider.GetCount() == 0)
                    {
                        if (m_tMailItemProvider.GetCount() > 0)
                        {
                            SetFocusedView(GetSecondaryView());
                        }
                        else
                        {
                            CloseSelectedSlot();
                        }
                    }
                    ptView->ForceUpdate(KEEP_INDEX_SELECTED);
                    GetSecondaryView()->ForceUpdate(KEEP_INDEX_SELECTED);
                    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
                }
            }
            else if (ptView == GetSecondaryView())
            {
                ACDID idACDItemToRemove = ptView->GetSelectedItem();

                m_tMailItemProvider.RemoveItem(idACDItemToRemove);
                ActorCommonData * ptACDItemToRemove = ACDTryToGet(idACDItemToRemove);
                if (ptACDItemToRemove && CurrencyFromGBID(ptACDItemToRemove->hGB.gbid) != CURRENCY_INVALID)
                {
                    ACDDelete(idACDItemToRemove, FALSE);
                }

                m_tUnmailedItemProvider.Update();
                m_tCraftingMatItemsProvider.Refresh();
                ptView->ForceUpdate(KEEP_INDEX_SELECTED);
                if (m_tMailItemProvider.GetCount() == 0)
                {
                    GetPrimaryView()->ForceUpdate(RESET_SELECTION);
                    SetFocusedView(GetPrimaryView());
                }
                else
                {
                    GetPrimaryView()->ForceUpdate(KEEP_INDEX_SELECTED);
                }
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
            }
        }
        break;

    case ACTION_EQUIP:
        if (!TakeCurrentItem(TRUE))
        {
            SwitchToInboxMode();
        }
        break;

    case ACTION_OPEN_CATEGORY:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        OpenSelectedCategory();
        break; }
}
} }

namespace UIInventory { namespace Console {
#line 1851 "X/Client/UserInterface/Console/UIConsoleMail.cpp"

void UnmailedCraftingMatItemsProvider::Refresh()

{
    CleanupACDs();

    Player * ptPlayer = GetPlayer();
    if (ptPlayer)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            m_arrVirtualItems.clear();
            // DWARF gives the initial ann as 0x2974; the loop emits 0x2975 first and advances each iteration.
            ACDNetworkName ann = (ACDNetworkName)10612;


            GBHandleList listGBHandles(GBGetHandlePool());
            GBEnumerate(GB_CURRENCY_CONVERSIONS, listGBHandles);

            // The original calls AttributesGetInt even though this DWARF local has no surviving use.
            int32 nCurrenciesDiscovered = ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_CURRENCIES_DISCOVERED));


            struct CurrencyItemInfo
            {
                CurrencyType eCurrency;
                int32 nSortOrder;
            };

            blz::vector<CurrencyItemInfo> arCurrencyInfos;

            POS pos = listGBHandles.GetHeadPosition();
            while (pos)
            {
                GBHandle hGB = listGBHandles.GetNext(pos);
                ScopedCurrencyConversionEntryLockR pdefCurrencyConversion(hGB.gbid);

                if (pdefCurrencyConversion && !pdefCurrencyConversion->bHidden)
                {
                    if (ptACDPlayer->GetCurrencyAmount(pdefCurrencyConversion->eCurrency) > 0)
                    {
                        CurrencyItemInfo tCurrencyItemInfo;
                        tCurrencyItemInfo.eCurrency = pdefCurrencyConversion->eCurrency;
                        tCurrencyItemInfo.nSortOrder = pdefCurrencyConversion->nSortOrder;
                        arCurrencyInfos.push_back(tCurrencyItemInfo);
                    }
                }
            }

            blz::function<bool(const CurrencyItemInfo &, const CurrencyItemInfo &)> funcSortEntries = [](const CurrencyItemInfo & tCurrencyA, const CurrencyItemInfo & tCurrencyB)
            {
                return tCurrencyA.nSortOrder < tCurrencyB.nSortOrder;
            };
            blz::sort(arCurrencyInfos.begin(), arCurrencyInfos.end(), funcSortEntries);

            for (int32 i = 0; i < arCurrencyInfos.size(); ++i)
            {
                // Inference: increment before each item, as in the base provider and original loop induction.
                ann = (ACDNetworkName)(ann + 1);
                CurrencyType eCurrency = arCurrencyInfos[i].eCurrency;

                GBID gbidItem = CurrencyGetGBID(eCurrency);
                if (gbidItem == GBID_NONE)
                {
                    continue;
                }

                int64 nCurrencyCount = ptACDPlayer->GetCurrencyAmount(eCurrency);

                // Inference: the line-0 zero branch skips exhausted stacks after subtracting already-mailed currency.
                uint64 nStackSize = nCurrencyCount - sgptMailGlobals->GetAmountOfCraftingMaterialInMailItemProvider(eCurrency);
                if (nStackSize == 0)
                {
                    continue;
                }


                GBHandle hgbItem(GB_ITEMS, gbidItem);


                D3::Items::Generator tItem;
                ItemGeneratorToDefaults(tItem);
                hgbItem.Export(*tItem.mutable_gb_handle());

                tItem.set_seed(0);
                uint32 dwFlags = tItem.flags() | (1 << IGF_IDENTIFIED_BIT);
                tItem.set_flags(dwFlags);

                tItem.set_stack_size(nStackSize);

                ACDID idACDItem = CItemGenerate(ann, tItem, NULL);

                m_arrVirtualItems.push_back(idACDItem);
            }
        }
    }
}
} }

namespace UIInventory { namespace Console {
#line 284 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
// DWARF has only an inlined body; this standalone emitted body is not checkable against the original.
inline MailViewDataSource & UIMailListView::GetDataSource()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{

    return m_tDataSource;
}
#line 2545 "X/Client/UserInterface/Console/UIConsoleMail.cpp"

void PlayerMailScreen::SetCurrentSelectedIndex(
    int32 nNewVal)
{
    m_bCurrentItemIsEquippable = FALSE;

    DeleteFakeItem();

    m_nCurrentMailIndex = nNewVal;

    MailViewDataSource & tDataSource = m_tMailListView.GetDataSource();

    if (nNewVal < 0 || nNewVal >= tDataSource.GetNumItems())
    {
        return;
    }

    MailUIEntry & tUIEntry = tDataSource.tEntryUI[nNewVal];

    const D3::Console::MailItem & tMailItem = tUIEntry.ptMailRef->GetContents();

    m_bShowingGift = FALSE;
    m_bShowingGold = FALSE;
    m_bShowingCraftingMaterial = FALSE;

    if (tMailItem.has_item_mail())
    {
        const D3::Console::ItemMail & tItemDetails = tMailItem.item_mail();

        const D3::Items::Generator & tGenerator = tItemDetails.item_generator();

        ACDID idItem = GenerateFakeItem(tGenerator);

        ActorCommonData * ptACDItem = ACDTryToGet(idItem);

        if (ptACDItem)
        {
            Player * ptPlayer = PlayerGetByControllerIndex(GetControllerIndex());

            ptACDItem->SetOwner(ptPlayer->idPlayerACD);

            m_bCurrentItemIsEquippable = ItemIsEquippable(*ptACDItem) && !ItemIsUnidentified(*ptACDItem);

            if (m_bCurrentItemIsEquippable)
            {
                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

                m_bCurrentItemIsEquippable = ItemOwnerMeetsRequirements(*ptACDItem, *ptACDPlayer, NULL);
            }
        }

        m_tRightSingleItemProvider.SetItem(idItem);
        m_tRightDetailsView.ForceUpdate(RESET_SELECTION);

        m_tRightDetailsView.Show(TRUE);
    }
    else if (tMailItem.has_gift_mail())
    {
        m_bShowingGift = TRUE;

        UIID idGiftText = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackgrounds.GiftPane.GiftText");
        UIText & tText = UI_GetTextFromID(idGiftText);
        tText.SetText(LocalizedSprintf(StringListFetch("X1_ConsoleMail:GiftText")), FALSE);
        // Inference: gift and gold branches share the FALSE Show tail attributed to line 2599.
        m_tRightDetailsView.Show(FALSE);
    }

    else if (tMailItem.has_gold_amount())
    {
        m_bShowingGold = TRUE;

        UIID idGoldText = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackgrounds.GoldPane.GoldText");
        UIText & tText = UI_GetTextFromID(idGoldText);
        tText.SetText(LocalizedSprintf(StringListFetch("X1_ConsoleMail:GoldText"), UIText_ValueToGameCurrency(tMailItem.gold_amount(), 0).str()), FALSE);
        m_tRightDetailsView.Show(FALSE);
    }
    else if (tMailItem.has_crafting_material_type())
    {
        m_bShowingCraftingMaterial = TRUE;

        CurrencyType eCurrencyType = tMailItem.crafting_material_type();
        GBID gbidCraftingMatItem = CurrencyGetGBID(eCurrencyType);

        GBHandle hGBItem(GB_ITEMS, gbidCraftingMatItem);

        D3::Items::Generator tGenerator;
        ItemGeneratorToDefaults(tGenerator);
        hGBItem.Export(*tGenerator.mutable_gb_handle());
        tGenerator.set_flags(tGenerator.flags() | (1 << IGF_IDENTIFIED_BIT));

        ACDID idItem = GenerateFakeItem(tGenerator);

        ActorCommonData * ptACDItem = ACDTryToGet(idItem);

        if (ptACDItem)
        {
            Player * ptPlayer = PlayerGetByControllerIndex(GetControllerIndex());
            ptACDItem->SetOwner(ptPlayer->idPlayerACD);
            ptACDItem->SetStackCount(tMailItem.crafting_material_amount());
        }

        m_tRightSingleItemProvider.SetItem(idItem);
        m_tRightDetailsView.ForceUpdate(RESET_SELECTION);

        m_tRightDetailsView.Show(TRUE);
    }

    UIID idGiftPane = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackGrounds.GiftPane");
    UI_GetWindowFromID(idGiftPane).SetActive(m_bShowingGift);

    UIID idGoldPane = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackGrounds.GoldPane");
    UI_GetWindowFromID(idGoldPane).SetActive(m_bShowingGold);

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}
} }

namespace UIInventory { namespace Console {
#line 3333 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
// Inference: line-0 tails return TRUE on no-op/success, FALSE after the full-inventory notification.
BOOL PlayerMailScreen::TakeCurrentItem(
    BOOL bEquip)
{
    if (!sgptMailGlobals || !sgptMailGlobals->IsActive())
    {
        return TRUE;
    }

    // Inference: the original compares the vector size as signed int32 against the selected index.
    if (m_nCurrentMailIndex >= 0 && m_nCurrentMailIndex < (int32)sgpMailArray->size())
    {
        m_eMailState = MAILSTATE_BROWSE_MAIL;

        ::Console::Online::MailRef & tMailRef = (*sgpMailArray)[m_nCurrentMailIndex];

        const D3::Console::MailItem & tMailItem = tMailRef->GetContents();

        Player * ptPlayer = PlayerGetByControllerIndex(GetControllerIndex());

        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

        if (tMailItem.has_item_mail())
        {
            int32 nFreeSlots = ACDInventoryGetNumEmptySlots(tACDPlayer, SLOT_PLAYER_BACKPACK);

            if (nFreeSlots > 0)
            {
                if (bEquip)
                {
                    ::Console::Online::MailDelete(GetControllerIndex(), tMailRef->GetID(), sMailItemDeleteConfirmed_Equip);
                }
                else
                {
                    ::Console::Online::MailDelete(GetControllerIndex(), tMailRef->GetID(), sMailDeleteConfirmed);
                }

                sgpMailArray->erase(sgpMailArray->begin() + m_nCurrentMailIndex);

                if (m_idACDCurrentItem != ACDID_INVALID)
                {
                    ActorCommonData * ptACDItem = ACDTryToGet(m_idACDCurrentItem);
                    if (ptACDItem)
                    {
                        UIInventory::PlayItemInventorySound(*ptACDItem);
                    }
                }
                // Inference: original inlined cleanup and shared update/TRUE tail follow the optional sound.
                DeleteFakeItem();
                UpdateDisplayedMail();
                sgptMailGlobals->RequestUpdate(KEEP_INDEX_SELECTED);
                return TRUE;
            }
        }

        else if (tMailItem.has_gift_mail())
        {
            int32 nFreeSlots = ACDInventoryGetNumEmptySlots(tACDPlayer, SLOT_PLAYER_BACKPACK);

            if (nFreeSlots > 0)
            {
                ::Console::Online::MailVerify(GetControllerIndex(), tMailRef->GetID(), sGiftReceivable);
                return TRUE;
            }
        }
        else if (tMailItem.has_gold_amount())
        {
            ::Console::Online::MailDelete(GetControllerIndex(), tMailRef->GetID(), sMailDeleteConfirmed);

            sgpMailArray->erase(sgpMailArray->begin() + m_nCurrentMailIndex);
            UpdateDisplayedMail();
            sgptMailGlobals->RequestUpdate(KEEP_INDEX_SELECTED);
            return TRUE;
        }

        else if (tMailItem.has_crafting_material_type())
        {
            ::Console::Online::MailDelete(GetControllerIndex(), tMailRef->GetID(), sMailDeleteConfirmed);

            sgpMailArray->erase(sgpMailArray->begin() + m_nCurrentMailIndex);
            UpdateDisplayedMail();
            sgptMailGlobals->RequestUpdate(KEEP_INDEX_SELECTED);
            return TRUE;
        }


        // Inference: all unsuccessful content branches share this notification and FALSE return.
        NotificationCreationParams tParams;
        tParams.nControllerIndex = GetControllerIndex();
        tParams.uszMessage = StringListFetch("X1_ConsoleMail:InventoryFull");
        s_idModal = ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);

        return FALSE;

    }

    return TRUE;
}
} }
namespace UIInventory { namespace Console {
// Inference: switch with two identical cases (tail-merged in the original: cmp #1/b.eq/cbnz dispatch, SetAction block at line 0), mirroring PlayerStashScreen::UpdateBagItemActions in UIConsoleStash.cpp; surviving CanOpenSelectedCategory inline is DWARF call line 2787. Blank lines 2784/2785 not recovered.
#line 2775 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::UpdateBagItemActions(
    ::Console::ControllerActionManager & tActionManager)
{
    switch (m_eCategoryState)
    {
    case BROWSE_CATEGORY_STATE:
        if (CanOpenSelectedCategory())
            tActionManager.SetAction(4, ACTION_OPEN_CATEGORY, StringListFetch("ConsoleUI:Open").str());
        break;


    case BROWSE_SUBCATEGORY_STATE:
        if (CanOpenSelectedCategory())
            tActionManager.SetAction(4, ACTION_OPEN_CATEGORY, StringListFetch("ConsoleUI:Open").str());
        break;
    }

}
} }
namespace UIMail { namespace Console {
#line 3976 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void SendGift(
    const ActorCommonData & tACD,
    int32 nControllerIndex)
{
    D3::Console::MailItem tMailItem;
    D3::Console::GiftMail * ptItemDetails = tMailItem.mutable_gift_mail();

    if (tACD.hGB.gbid == GlobalGBIDGet(GLOBALGBID_CONSOLE_FRIEND_GIFT))
    {
        ptItemDetails->set_item_quality_level(9);
    }
    else
    {
        ptItemDetails->set_item_quality_level(8);
    }

    tMailItem.set_hardcore(PlayerIsHardcore(PlayerGetByControllerIndex(nControllerIndex)) != FALSE);





    sidGiftToSend.idACDGift = tACD.id;
    sidGiftToSend.nControllerIndex = nControllerIndex;

    ::Console::Online::MailSend(nControllerIndex, ::Console::Online::MAIL_TYPE_MESSAGE, tMailItem, tACD.ptFriendGiftData->idRecipient, &sPlayerGiftSendSuccess);




}
} }
namespace UIInventory { namespace Console {
#line 655 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
CRefString sGetExpirationfromTimestamp(
    Blizzard::Time::Timestamp nExpiration)
{
    nExpiration -= ::Console::Online::GetNetworkTimestamp();
    uint64 nHoursToExpiration = nExpiration / 3600000000000LL;

    CRefString sReturn;

    if (nHoursToExpiration < 24)
    {
        sReturn = StringListFetch("x1_ConsoleMail:ExpirationToday").str();
    }
    else if (nHoursToExpiration < 744)
    {
        sReturn = LocalizedSprintf(
            StringListFetch("x1_ConsoleMail:ExpirationDays").str(), UnicodeText::FormatNumber(nHoursToExpiration / 24, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
    }
    else
    {
        sReturn = LocalizedSprintf(
            StringListFetch("x1_ConsoleMail:ExpirationMonths").str(), UnicodeText::FormatNumber(nHoursToExpiration / 744, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
    }
    return sReturn;

}
} }
namespace UIInventory { namespace Console {
#line 1669 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::SetNoMailDisplay(
    BOOL bActive,
    NoMailReason eReason)
{
    UIID idNoMail = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackGrounds.NoMail");
    UI_GetWindowFromID(idNoMail).SetActive(bActive);

    UIID idNoMailText = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailBackGrounds.NoMail.NoMailText");
    UIText & tText = UI_GetTextFromID(idNoMailText);

    UIID idMailBox = UIIDCreateFromParent(UIWND_INVENTORY_CONSOLE_ROOT, ".Mail.MailRing");
    UI_GetWindowFromID(idMailBox).SetActive(bActive);

    if (eReason == NOMAILREASON_CLOSE)
    {
        UI_GetWindowFromID(idMailBox).SetActive(FALSE);
    }

    tText.SetActive(bActive);
    if (bActive)
    {
        UI_GetWindowFromID(idMailBox).SetActive(TRUE);
        if (eReason == NOMAILREASON_NO_MAIL)
        {
            tText.SetText(LocalizedSprintf(StringListFetch("X1_ConsoleMail:NoMail")), FALSE);
        }
        else
        {
            tText.SetText(LocalizedSprintf(StringListFetch("X1_ConsoleMail:NoConnection")), FALSE);
        }
    }
}
} }
namespace UIInventory { namespace Console {
// NON_MATCHING: 288 vs 296; the original tests m_eMailState == MAILSTATE_BROWSE_MAIL and sgpMailArray != NULL together (cmp/ccmp, line 3304) on the path where sgpMailArray is NULL; ours threads that path straight to CloseScreenSet. bValid (DWARF 3287) shape inferred: lines 3289/3294/3299 hold the array load, the index tests and the state load; combined and assignment forms of 3294-3304 tried (280).
#line 3284 "X/Client/UserInterface/Console/UIConsoleMail.cpp"
void PlayerMailScreen::RedeemCurrentGift()

{
    BOOL bValid = TRUE;

    if (sgpMailArray != NULL)
    {



        if (m_nCurrentMailIndex < 0 || m_nCurrentMailIndex >= (int32)sgpMailArray->size())
        {
            bValid = FALSE;
        }
    }
    if (m_eMailState != MAILSTATE_BROWSE_MAIL)
    {
        bValid = FALSE;
    }

    if (bValid && sgpMailArray != NULL)
    {
        ::Console::Online::MailRef & tMailRef = (*sgpMailArray)[m_nCurrentMailIndex];

        if (tMailRef->AreContentsValid())
        {
            const D3::Console::MailItem & tMailItem = tMailRef->GetContents();

            IntDataMessage tMessage;
            tMessage.nData = tMailItem.gift_mail().item_quality_level();

            m_nLastGiftMailIndex = m_nCurrentMailIndex;

            MessageSendToServer(CMSG_GIFT_ITEM_RECEIVED, &tMessage, sizeof(tMessage), PlayerGetByControllerIndex(GetControllerIndex()));
            m_eMailState = MAILSTATE_WAIT_FOR_GIFT_RECEIPT_CONFIRM;

            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

            return;
        }
    }





    ::Console::Screens::CloseScreenSet(TRUE, FALSE);

}
} }
