// Local classes recovered from the original DWARF; methods below use original evidence.
// Original file: X/Client/UserInterface/Console/UIConsoleSeasonStash.cpp

#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"

#include "../../../Common/GlobalSNO.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/ItemsSwig.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Items.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Sigma/UnicodeText.h"
#include "../Controls/UITextControl.h"
#include "../../Sound/Sound.h"
#include "UIConsoleInventoryCategorySummaryView.h"
#include "ConsoleUIRActorManager.h"
#include "ConsoleScreenManager.h"
#include "ConsoleModalNotificationManager.h"
#include "ControllerActions.h"
#include "../../Console/ControllerUtils.h"
#include "../UI.h"
#include "../Controls/UIWindowControl.h"
#include "../UIIDs.h"
#include "../../Console/GamerProfile.h"
#include "../../../Common/Protobuf/Account.pb.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "UIConsoleInventoryCategoryView.h"
#include "UIConsoleInventoryDetailsView.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryListView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "UIConsoleItemCategories.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/Protobuf/Items.pb.h"
#include "../../../Sigma/SigmaTypes.h"

namespace Console { class ControllerActionManager; }
namespace Console { class RingWrapper; }
namespace D3 { namespace Account { class ConsolePartitionData; } }
struct UIID;

namespace UISeasonStash {
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
};

class SeasonStashItemProvider : public UIInventory::Console::UIInventoryViewItemsProvider {  // size 0x8c8
public:
    SeasonStashItemProvider();
    virtual ~SeasonStashItemProvider() {}
    virtual BOOL IdentifyIsAllowed();  // vtable slot 12
    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType eFilterType) const;  // vtable slot 6
    virtual const ACDIDArray & GetItems() { return GetItemsWithFilter(GetFilter()); }  // vtable slot 15; original line 97
    void RemoveAllItems();
    virtual Blizzard::Time::Timestamp GetItemExpiration(const int32 nIndex) const;  // vtable slot 5
    void CleanupACDs();
    virtual void Refresh();  // vtable slot 16
    void SetFilter(const UIFilterType eFilterType);
    int32 GetCount();
    struct StashItemData {  // size 0xe8
        ACDID idItem;  // 0x00
        int32 nIndex;  // 0x04
        D3::Items::Generator tGenerator;  // 0x08
        Blizzard::Time::Timestamp nExpiration;  // 0xe0
        StashItemData();
    };
    BOOL GetItemData(ACDID idItem, UISeasonStash::Console::SeasonStashItemProvider::StashItemData & tData);
    void DeleteItem(int32 nIndex);
private:
    D3::Account::ConsolePartitionData * m_ptSeasonRolloverItems;  // 0x20
    blz::vector<UISeasonStash::Console::SeasonStashItemProvider::StashItemData, blz::allocator<UISeasonStash::Console::SeasonStashItemProvider::StashItemData> > m_arrAllItems;  // 0x28
    ACDIDArray m_arrFilteredItems[FILTER_COUNT];  // 0x40
    ACDIDArray m_arrEmpty;  // 0x8b0
};
static_assert(sizeof(SeasonStashItemProvider) == 0x8c8, "SeasonStashItemProvider");

class SeasonStashScreen : public UIInventory::Console::UIInventoryScreen {  // size 0xcf0 (UISeasonStash InitializeForGame: new(0xcf0))
    enum StashState {
        BROWSE_CATEGORY_STATE = 0,
        BROWSE_SUBCATEGORY_STATE = 1,
        BROWSE_GEMS_STATE = 2,
        BROWSE_ITEMS_STATE = 3,
    };
public:
    UISeasonStash::Console::SeasonStashItemProvider m_tItemsProvider;  // 0x200
    UIInventory::Console::UIInventoryListView m_tListView;  // 0xac8
    UIInventory::Console::UIInventoryDetailsView m_tRightDetailsView;  // 0xb50
    UIInventory::Console::UIInventoryCategoryView m_tCategoriesView;  // 0xbc8
    UIInventory::Console::UIInventoryCategorySummaryView m_tCategorySummaryView;  // 0xc60
    UISeasonStash::Console::SeasonStashScreen::StashState m_eStashState;  // 0xcc8
    UIInventory::Console::ItemCategory m_eCurrentCategory;  // 0xccc
    int32 m_nSelectedIndex;  // 0xcd0
    ACDID m_idSelectedItem;  // 0xcd4
    BOOL m_bWaitingOnItem;  // 0xcd8
    int32 m_nIndexOfItemBeingRetrieved;  // 0xcdc
    int32 m_nStashSpaceUsed;  // 0xce0
    int32 m_nInventorySpaceUsed;  // 0xce4
    BOOL m_bWaitingOnSpaceUpdate;  // 0xce8
    virtual void OnSetActive(BOOL bActive);  // vtable slot 16
    virtual void InitializeRingWrapper(::Console::RingWrapper & tRingWrapper);  // vtable slot 17
    virtual void OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType);  // vtable slot 22
    virtual UIFilterType GetDefaultRingSelection() const;  // vtable slot 18
    virtual void InitRingFilters(UIIDToFilterMap & tFilterMap);  // vtable slot 21
    virtual UIInventory::Console::UIInventoryView * GetPrimaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode);  // vtable slot 23
    virtual UIInventory::Console::UIInventoryView * GetSecondaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode eBrowsingMode);  // vtable slot 24
    void UpdateStashSpace();
    virtual void OnOpen();  // vtable slot 10
    virtual void OnClose();  // vtable slot 11
    virtual void OnUpdate();  // vtable slot 13
    virtual void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);  // vtable slot 9
    void ShowTopLevelCategories();
    void ShowGemCategories();
    void OpenSelectedCategory();
    void CloseCategory();
    void RetrieveItemFromSeasonStash(InventorySlotType eSlotType);
    void OnButton1();
    void OnButton2();
    void OnButton3();
    void OnButton4();
    void OnLeftTrigger();
    void OnRightTrigger();
    virtual BOOL ShouldEnableRingIcon(const UIFilterType eFilterType);  // vtable slot 20
    BOOL CanOpenSelectedCategory();
    BOOL CanOpenCategory(const UIInventory::Console::ItemCategory eCategory);
    void UpdateBagItemActions(::Console::ControllerActionManager & tActionManager);
    void UpdateNormalItemActions(::Console::ControllerActionManager & tActionManager);
    virtual void UpdateControllerActions(::Console::ControllerActionManager & tActionManager);  // vtable slot 14
    void EnableStashUI(BOOL bActive);
    void UpdateCategoryUI(const UIID & idItemUI, const UIInventory::Console::ItemCategory eCategory);
    void ItemRetrievedResult(BOOL bResult);
    SeasonStashScreen();
    virtual ~SeasonStashScreen() {}
};
static_assert(sizeof(SeasonStashScreen) == 0xcf0, "SeasonStashScreen");

}  // namespace Console
}  // namespace UISeasonStash

// UI.h globals not yet adopted in UIIDs.h.
extern const UIID UIBTN_INVENTORY_CONSOLE_RING_ARM_LEFT, UIBTN_INVENTORY_CONSOLE_RING_ARM_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_BAG, UIBTN_INVENTORY_CONSOLE_RING_BRACERS, UIBTN_INVENTORY_CONSOLE_RING_FEET, UIBTN_INVENTORY_CONSOLE_RING_FINGER_LEFT, UIBTN_INVENTORY_CONSOLE_RING_FINGER_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_HANDS, UIBTN_INVENTORY_CONSOLE_RING_HEAD, UIBTN_INVENTORY_CONSOLE_RING_LEGS, UIBTN_INVENTORY_CONSOLE_RING_NECK, UIBTN_INVENTORY_CONSOLE_RING_SHOULDER, UIBTN_INVENTORY_CONSOLE_RING_TORSO, UIBTN_INVENTORY_CONSOLE_RING_WAIST;
extern UIID UITXT_INVENTORY_CONSOLE_HEADER; extern UIID UITEXT_SEASON_STASH_STASH_SPACE, UITEXT_SEASON_STASH_INVENTORY_SPACE;  // DWARF: UIGlobals.cpp:3144, :2666, :2667 respectively; no header declarations recorded.
CRefString LocalizedSprintf(LPCSTR szFormat, ...); // X/Sigma/Unicode.cpp
extern const UIID UIWND_INVENTORY_CONSOLE_RING_GLOW, UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR;
extern const UIID UIDLG_INVENTORY_CONSOLE_LIST, UIDLG_INVENTORY_CONSOLE_EQUIPPED, UIDLG_INVENTORY_CONSOLE;
extern const UIID UIWND_MAIL_WRAPPER, UIWND_SEASON_STASH_WRAPPER, UIWND_SEASON_STASH_RING_WRAPPER;
// X/Common/ActorCommonData.h and X/Client/Console/GamerProfile.h: unadopted declarations.
void ACDDelete(ACDID idACD, BOOL bFreeID);
// X/Common/Items.h, original Items.cpp:7494.
ACDID CItemGenerate(ACDNetworkName ann, const D3::Items::Generator & tGenerator, XGrowableMap<int32, int32, int32, int32> * arAffixSpecificAttributes);
// X/Common/Player.h: original error record and inventory manipulation declarations.







BOOL PlayerCanManipulateInventory(const ActorCommonData & tACDPlayer);
namespace UIGame { void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer); }
// X/Client/Network/CGameMessage.cpp and Common/Networking/GameMessage.h.
void MessageSendToServer(GameMessageType eType, const google::protobuf::MessageLite & tMessage, const Player * ptPlayer);
#include "../../../Common/Networking/GameMessage.h"
// X/Client/UserInterface/Console/ConsoleScreenManager.h.
namespace Console { namespace Screens { void CloseScreenSet(BOOL fClearScreenSetHistory, BOOL fSkipTransition); } }
int32 UI_GetPrimaryUIController(); // X/Client/UserInterface/UI.h
namespace Console { namespace GamerProfile { BOOL IsActive(int32 nUserIdx); } }
// UIConsoleInventoryViewUtilities.h: unadopted info-panel interface.
namespace UIInventory { namespace Console {
enum InfoPanelType { SEASON_STASH_INFO_PANEL = 12 };
void SwitchToInfoPanel(InfoPanelType eInfoPanel);
} }
namespace UISeasonStash { namespace Console {
using namespace UIInventory::Console;

// Folded into the original HandleButtonInput as +/-4.0.
static const float SCROLLBOX_SCROLLDELTA = 4.0f;
void OnUpdateCategoryUI(const UIID & idItemUI, const ItemCategory eCategory);

BOOL SeasonStashRingIncludeControlPredicate(const UIID & idControl)
{
    return idControl != UIWND_INVENTORY_CONSOLE_RING_GLOW;
}

inline SeasonStashItemProvider::SeasonStashItemProvider() : m_ptSeasonRolloverItems(NULL)
{
    RemoveAllItems();
}

BOOL SeasonStashItemProvider::IdentifyIsAllowed() { return FALSE; }

const ACDIDArray & SeasonStashItemProvider::GetItemsWithFilter(const UIFilterType eFilterType) const
{
    if (eFilterType >= 0 && eFilterType < FILTER_COUNT) return m_arrFilteredItems[eFilterType];
    return m_arrEmpty;
}

inline void SeasonStashItemProvider::RemoveAllItems() { CleanupACDs(); }

Blizzard::Time::Timestamp SeasonStashItemProvider::GetItemExpiration(const int32 nIndex) const
{
    if (nIndex >= 0 && nIndex < m_arrAllItems.size()) return m_arrAllItems[nIndex].nExpiration;
    return 0;
}

void SeasonStashItemProvider::CleanupACDs()
{
    for (int32 i = 0; i < m_arrAllItems.size(); ++i) ACDDelete(m_arrAllItems[i].idItem, FALSE);
    m_arrAllItems.clear();
    for (int32 i = 0; i < FILTER_COUNT; ++i) m_arrFilteredItems[i].clear();
}

void SeasonStashItemProvider::Refresh()
{
    CleanupACDs();
    Player * ptPlayer = GetPlayer();
    if (!ptPlayer) return;
    for (int32 n = 0; n < FILTER_COUNT; ++n) m_arrFilteredItems[n].clear();
    D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
    BOOL bIsHardcore = (ptPlayer->tHeroStateData.uPlayerFlags & 1) != 0;
    int32 nPartition = bIsHardcore ? 1 : 0;
    D3::Account::AccountPartition * ptPartition = tAccountDefinition.mutable_partitions(nPartition);
    if (!ptPartition->console_partition_data().seasonal_rollover_item_size()) return;
    m_ptSeasonRolloverItems = ptPartition->mutable_console_partition_data();
    // Original line 174 uses network-name base 0x2b84; line 171 caps rollover generation at 512.
    for (int32 n = 0; n < m_ptSeasonRolloverItems->seasonal_rollover_item_size() && n < 512; ++n) {
        const D3::Account::ConsolePartitionData::SeasonalRolloverItem * ptSeasonalRolloverItemData = &m_ptSeasonRolloverItems->seasonal_rollover_item(n);
        const D3::Items::SavedItem & tSeasonalRolloverItem = ptSeasonalRolloverItemData->item();
        ACDID idItem = CItemGenerate((ACDNetworkName)(0x2b84 + n), tSeasonalRolloverItem.generator(), NULL);
        if (idItem != ACDID_INVALID) {
            ActorCommonData * ptACDItem = ACDTryToGet(idItem);
            if (ptACDItem && ItemIsGift(ptACDItem->hGB.gbid)) {
                if (!ptACDItem->ptFriendGiftData) ptACDItem->ptFriendGiftData = new FriendGiftData;
                ptACDItem->ptFriendGiftData->idRecipient = tSeasonalRolloverItem.friend_gift_info().id_friend_recipient();
                ptACDItem->ptFriendGiftData->sNameRecipient = tSeasonalRolloverItem.friend_gift_info().recipient_name().c_str();
            }
            StashItemData tItemData;
            tItemData.nIndex = n;
            tItemData.idItem = idItem;
            tItemData.tGenerator = *ptPartition->mutable_console_partition_data()->mutable_seasonal_rollover_item(n)->mutable_item()->mutable_generator();
            // Original 0x278d00 is 30 days of rollover retention, expressed in Unix seconds.
            tItemData.nExpiration = ptSeasonalRolloverItemData->has_create_time() ? Blizzard::Time::FromUnixTime(ptSeasonalRolloverItemData->create_time() + 30 * 24 * 60 * 60) : 0;
            m_arrAllItems.push_back(tItemData);
        }
    }
    ActorCommonData * ptACDOwner = ACDTryToGet(ptPlayer->idPlayerACD);
    for (int32 i = 0; i < FILTER_FOLLOWER_START; ++i) {
        UIFilterType eFilterType = i;
        ItemFilterPredicate fnFilterPredicate = FilteredInventoryView::GetPredicateForType(eFilterType);
        for (int32 n = 0; n < m_arrAllItems.size(); ++n) {
            ActorCommonData * ptACDItem = ACDTryToGet(m_arrAllItems[n].idItem);
            if (ptACDItem && fnFilterPredicate) {
                BOOL bIncludeItem = fnFilterPredicate(*ptACDOwner, m_arrAllItems[n].idItem, eFilterType);
                if (bIncludeItem) m_arrFilteredItems[i].push_back(m_arrAllItems[n].idItem);
            }
        }
    }
}

inline void SeasonStashItemProvider::SetFilter(const UIFilterType eFilterType)
{
    UIInventoryViewItemsProvider::SetFilter(eFilterType);
    Refresh();
}

inline SeasonStashItemProvider::StashItemData::StashItemData() : idItem(ACDID_INVALID), nIndex(-1), nExpiration(0) {}

inline BOOL SeasonStashItemProvider::GetItemData(ACDID idItem, StashItemData & tData)
{
    for (int32 n = 0; n < m_arrAllItems.size(); ++n) {
        if (m_arrAllItems[n].idItem == idItem) {
            tData = m_arrAllItems[n];
            return TRUE;
        }
    }
    return FALSE;
}

void SeasonStashItemProvider::DeleteItem(int32 nIndex)
{
    if (nIndex != -1) {
        D3::Account::ConsolePartitionData tNewData;
        for (int32 i = 0; i < m_ptSeasonRolloverItems->seasonal_rollover_item_size(); ++i) {
            if (i != nIndex) {
                D3::Account::ConsolePartitionData::SeasonalRolloverItem * ptEntry = tNewData.add_seasonal_rollover_item();
                *ptEntry = m_ptSeasonRolloverItems->seasonal_rollover_item(i);
            }
        }
        m_ptSeasonRolloverItems->clear_seasonal_rollover_item();
        *m_ptSeasonRolloverItems = tNewData;
    }
    Refresh();
}

void SeasonStashScreen::OnSetActive(BOOL bActive)
{
    if (bActive) {
        ShowNavBar(UIDLG_INVENTORY_CONSOLE, "SeasonStashNavigation");
        EnableStashUI(TRUE);
        RequestUpdate(RESET_SELECTION);
        ::Console::Screens::ShowModalDialog(UI_GetPrimaryUIController(), UIDLG_INVENTORY_CONSOLE, FALSE, GetUIIDNone());
        SetFocusedView(NULL);
    } else {
        EnableStashUI(FALSE);
        ::Console::Screens::CloseModalDialog(UIDLG_INVENTORY_CONSOLE);
    }
}

void SeasonStashScreen::InitializeRingWrapper(::Console::RingWrapper & tRingWrapper)
{
    tRingWrapper.WrapUsingContainerControl(UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR, SeasonStashRingIncludeControlPredicate, FALSE);
}

void SeasonStashScreen::OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType)
{
    m_tItemsProvider.SetFilter(eNewFilterType);
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    m_tRightDetailsView.RequestUpdate(KEEP_INDEX_SELECTED);
    if (GetFilter() == FILTER_BAG) ShowTopLevelCategories();
    else m_tListView.ForceUpdate(RESET_SELECTION);
}

UIFilterType SeasonStashScreen::GetDefaultRingSelection() const { return FILTER_MAIN_HAND; }

void SeasonStashScreen::InitRingFilters(UIIDToFilterMap &tFilterMap)
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

UIInventoryView * SeasonStashScreen::GetPrimaryViewForMode(const BrowsingMode)
{
    if (GetFilter() == FILTER_BAG && (m_eStashState == BROWSE_CATEGORY_STATE || m_eStashState == BROWSE_SUBCATEGORY_STATE)) return &m_tCategoriesView;
    return &m_tListView;
}

UIInventoryView * SeasonStashScreen::GetSecondaryViewForMode(const BrowsingMode eBrowsingMode)
{
    if (GetFilter() == FILTER_BAG && m_eStashState != BROWSE_GEMS_STATE && m_eStashState != BROWSE_ITEMS_STATE) return &m_tCategorySummaryView;
    return &m_tRightDetailsView;
}

void SeasonStashScreen::UpdateStashSpace()
{
    Player * ptPlayer = PlayerGetByControllerIndex(GetControllerIndex());
    if (!ptPlayer) return;
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer) return;
    int32 nInventoryUsed = ACDInventoryGetNumFilledSlots(*ptACDPlayer, SLOT_PLAYER_BACKPACK);
    IVector2D tSlotSize;
    ACDInventorySlotGetSize(SLOT_PLAYER_BACKPACK, tSlotSize);
    int32 nInventoryTotal = tSlotSize.x * tSlotSize.y;
    int32 nStashUsed = ACDInventoryGetNumFilledSlots(*ptACDPlayer, SLOT_PLAYER_SHARED_STASH);
    int32 nStashTotal = ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_SHARED_STASH_SLOTS));
    UI_GetTextFromID(UITEXT_SEASON_STASH_STASH_SPACE).SetText(LocalizedSprintf(StringListFetch("ConsoleInventory:SeasonRollStashSpace"), UnicodeText::FormatNumber(nStashUsed, TRUE, FNS_NORMAL, FALSE).str(), UnicodeText::FormatNumber(nStashTotal, TRUE, FNS_NORMAL, FALSE).str()), FALSE);
    UI_GetTextFromID(UITEXT_SEASON_STASH_INVENTORY_SPACE).SetText(LocalizedSprintf(StringListFetch("ConsoleInventory:SeasonRollInventorySpace"), UnicodeText::FormatNumber(nInventoryUsed, TRUE, FNS_NORMAL, FALSE).str(), UnicodeText::FormatNumber(nInventoryTotal, TRUE, FNS_NORMAL, FALSE).str()), FALSE);
    m_nStashSpaceUsed = nStashUsed;
    m_nInventorySpaceUsed = nInventoryUsed;
}

void SeasonStashScreen::OnOpen()
{
    Player * ptPlayer = PlayerGetByControllerIndex(GetControllerIndex());
    if (ptPlayer) m_tItemsProvider.SetPlayer(ptPlayer);
    UIID idKey;
    const UIIDToFilterMap & tFilterMap = m_tFilterMap;
    UIFilterType eFilterType;
    POS pos = tFilterMap.GetStartAssoc();
    while (pos) {
        tFilterMap.GetNextAssoc(pos, idKey, eFilterType);
        UIID idNewIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.NewStar");
        UI_GetWindowFromID(idNewIndicator).SetActive(FALSE);
        UIID idNewEliteIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.NewStarElite");
        UI_GetWindowFromID(idNewEliteIndicator).SetActive(FALSE);
    }
    SwitchToInfoPanel(SEASON_STASH_INFO_PANEL);
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING_CURSOR).SetActive(TRUE);
    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(LocalizedSprintf(StringListFetch("ConsoleInventory:SeasonCache")), FALSE);
    m_tItemsProvider.Refresh();
    EnableRing(TRUE);
    SetFilterType(GetInitialRingSelection(FILTER_PLAYER_START, FILTER_PLAYER_END));
    UpdateRingIcons();
    UpdateMainhandAndOffhandRingIcons(ptPlayer);
    SetFocusedView(NULL);
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    UpdateStashSpace();
    m_bWaitingOnSpaceUpdate = FALSE;
    ::Console::UIRActorManager::SetActive(FALSE);
}

void SeasonStashScreen::OnClose()
{
    SetPrimaryView(NULL);
    SetSecondaryView(NULL);
    ::Console::UIRActorManager::SetActive(FALSE);
    m_tItemsProvider.RemoveAllItems();
    m_tItemsProvider.SetPlayer(NULL);
}

void SeasonStashScreen::OnUpdate()
{
    ACDID idSelectedItem = m_tListView.GetSelectedItem();
    if (idSelectedItem != m_idSelectedItem) {
        m_tRightDetailsView.SelectItem(idSelectedItem);
        m_idSelectedItem = idSelectedItem;
    }
    if (GetFilter() == FILTER_BAG && (m_eStashState == BROWSE_CATEGORY_STATE || m_eStashState == BROWSE_SUBCATEGORY_STATE)) {
        ItemCategory eCategory = m_tCategoriesView.GetSelectedItem();
        if (eCategory != -1) m_tCategorySummaryView.SetCategory(eCategory);
    }
    if (m_bWaitingOnSpaceUpdate) {
        int32 nOldInventoryUsed = m_nInventorySpaceUsed;
        int32 m_nOldStashUsed = m_nStashSpaceUsed;
        UpdateStashSpace();
        if (m_nInventorySpaceUsed != nOldInventoryUsed || m_nStashSpaceUsed != m_nOldStashUsed) m_bWaitingOnSpaceUpdate = FALSE;
    }
}

void SeasonStashScreen::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)
{
    if (GetControllerIndex() != nControllerIndex) return;
    switch (eInputButton) {
    case BUTTON_1: OnButton1(); break;
    case BUTTON_2: OnButton2(); break;
    case BUTTON_3: OnButton3(); break;
    case BUTTON_4: OnButton4(); break;
    case BUTTON_LEFT_TRIGGER: OnLeftTrigger(); break;
    case BUTTON_RIGHT_TRIGGER: OnRightTrigger(); break;
    default: UIInventoryScreen::HandleButtonInput(nControllerIndex, eInputButton); break;
    }
}

inline void SeasonStashScreen::ShowTopLevelCategories()
{
    m_eStashState = BROWSE_CATEGORY_STATE;
    m_eCurrentCategory = CATEGORY_NONE;
    PopulateTopLevelCategories(m_tCategoriesView);
    if (GetPrimaryView() != &m_tCategoriesView) SetPrimaryView(&m_tCategoriesView);
    else m_tCategoriesView.ForceUpdate(RESET_SELECTION);
}

inline void SeasonStashScreen::ShowGemCategories()
{
    m_eStashState = BROWSE_SUBCATEGORY_STATE;
    m_eCurrentCategory = CATEGORY_GEMS;
    PopulateGemCategories(m_tCategoriesView);
    if (GetPrimaryView() != &m_tCategoriesView) SetPrimaryView(&m_tCategoriesView);
    else m_tCategoriesView.ForceUpdate(RESET_SELECTION);
}

void SeasonStashScreen::OpenSelectedCategory()
{
    ItemCategory eCategory = m_tCategoriesView.GetSelectedItem();
    if (eCategory == CATEGORY_GEMS) ShowGemCategories();
    else {
        UIFilterType eFilterType = GetFilterTypeForCategory(eCategory, FALSE);
        m_eCurrentCategory = eCategory;
        if (eFilterType != FILTER_NONE) m_tItemsProvider.SetFilter(eFilterType);
        m_eStashState = IsGemSubCategory(eCategory) ? BROWSE_GEMS_STATE : BROWSE_ITEMS_STATE;
        SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
        SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
        GetPrimaryView()->Enable(TRUE);
        GetSecondaryView()->Enable(TRUE);
        SetFocusedView(GetPrimaryView());
    }
}

void SeasonStashScreen::CloseCategory()
{
    if (m_eStashState == BROWSE_SUBCATEGORY_STATE || m_eStashState == BROWSE_ITEMS_STATE) {
        ItemCategory eCurrentCategory = m_eCurrentCategory;
        ShowTopLevelCategories();
        SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
        SetFocusedView(GetPrimaryView());
        m_tCategoriesView.SelectItem((ACDID)eCurrentCategory);
    }
}

void SeasonStashScreen::RetrieveItemFromSeasonStash(InventorySlotType eSlotType)
{
    if (m_bWaitingOnItem) return;
    int32 nIndex = m_tListView.GetIndexOfSelectedItem();
    const ACDIDArray & arrItems = m_tItemsProvider.GetItems();
    if (nIndex >= 0 && nIndex < arrItems.size()) {
        ACDID idItem = arrItems[nIndex];
        SeasonStashItemProvider::StashItemData tData;
        if (m_tItemsProvider.GetItemData(idItem, tData)) {
            MessageSendToServer(eSlotType == SLOT_PLAYER_SHARED_STASH ? CMSG_SEND_SEASONAL_ITEM_TO_STASH : CMSG_SEND_SEASONAL_ITEM_TO_INVENTORY, tData.tGenerator, GetPlayer());
            m_bWaitingOnItem = TRUE;
            m_nIndexOfItemBeingRetrieved = tData.nIndex;
            m_bWaitingOnSpaceUpdate = TRUE;
        }
    }
}

void SeasonStashScreen::OnButton1()
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_1);
    switch (idAction) {
    case ACTION_OPEN_SLOT:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        OpenSelectedSlot();
        break;
    case ACTION_OPEN_CATEGORY:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        OpenSelectedCategory();
        break;
    case ACTION_MOVE_TO_INVENTORY: {
        UIInventoryView * ptPlayerView = GetPrimaryView();
        ACDID idSelectedItem = ptPlayerView ? ptPlayerView->GetSelectedItem() : ACDID_INVALID;
        IVector2D tSlotSize;
        ACDInventorySlotGetSize(SLOT_PLAYER_BACKPACK, tSlotSize);
        int32 nInventoryTotal = tSlotSize.x * tSlotSize.y;
        if (m_nInventorySpaceUsed >= nInventoryTotal) {
            NotificationCreationParams tParams;
            tParams.nControllerIndex = GetControllerIndex();
            tParams.uszMessage = StringListFetch("ConsoleInventory:InventoryFull");
            ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
            return;
        }
        ActorCommonData * ptACDSelectedItem = ACDTryToGet(idSelectedItem);
        if (ptACDSelectedItem) {
            const ActorCommonData & tACDPlayer = ACDGet(GetPlayer()->idPlayerACD);
            if (!PlayerCanManipulateInventory(tACDPlayer)) {
                UIGame::DisplayError(GameErrorData(GAMEERROR_ITEMS_CANNOT_BE_MOVED_AT_THIS_TIME), FALSE, NULL);
                return;
            }
            RetrieveItemFromSeasonStash(SLOT_PLAYER_BACKPACK);
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_RETRIEVE_ITEM_FROM_SEASON_STASH), cgtPSODefaults);
        }
        break;
    }
    }
}

void SeasonStashScreen::OnButton2()
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2);
    if (idAction == ACTION_CLOSE) { ::Console::Screens::CloseScreenSet(TRUE, FALSE); return; }
    switch (idAction) {
    case ACTION_CLOSE_SLOT: {
        if (GetFocusedView() && GetPlayer() && GetPlayer()->ptNewItemManager) {
            ACDID idSelectedItem = GetFocusedView()->GetSelectedItem();
            if (idSelectedItem != ACDID_INVALID) GetPlayer()->ptNewItemManager->UnMarkAsNew(idSelectedItem);
        }
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        CloseSelectedSlot();
    break;
    }
    case ACTION_BACK: {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        if (m_eStashState == BROWSE_SUBCATEGORY_STATE || m_eStashState == BROWSE_ITEMS_STATE) CloseCategory();
        else {
            m_eStashState = BROWSE_SUBCATEGORY_STATE;
            m_eCurrentCategory = CATEGORY_GEMS;
            SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
            SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
        }
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    break;
    }
    }
}

void SeasonStashScreen::OnButton3()
{
    ACDID idPrimarySelectedItem = GetPrimaryView()->GetSelectedItem();
    ACDID idRightSelectedItem = GetSecondaryView()->GetSelectedItem();
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3) == ACTION_DETAILS) {
        SetBrowsingMode(BROWSE_DETAILS);
        GetPrimaryView()->SelectItem(idPrimarySelectedItem);
        GetSecondaryView()->SelectItem(idRightSelectedItem);
    } else if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3) == ACTION_LIST) {
        SetBrowsingMode(BROWSE_LIST);
        GetPrimaryView()->SelectItem(idPrimarySelectedItem);
        GetSecondaryView()->SelectItem(idRightSelectedItem);
    }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void SeasonStashScreen::OnButton4()
{
    Player * ptPlayer = GetPlayer();
    if (!ptPlayer) return;
    const ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_4);
    if (idAction == ACTION_MOVE_TO_STASH) {
        if (m_nStashSpaceUsed >= tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_SHARED_STASH_SLOTS))) {
            NotificationCreationParams tParams;
            tParams.nControllerIndex = GetControllerIndex();
            tParams.uszMessage = StringListFetch("ConsoleInventory:StashFull");
            ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
            return;
        }
        UIInventoryView * ptPlayerView = GetPrimaryView();
        ACDID idSelectedItem = ptPlayerView ? ptPlayerView->GetSelectedItem() : ACDID_INVALID;
        ActorCommonData * ptACDSelectedItem = ACDTryToGet(idSelectedItem);
        if (ptACDSelectedItem) {
            const ActorCommonData & tACDPlayer = ACDGet(GetPlayer()->idPlayerACD);
            if (!PlayerCanManipulateInventory(tACDPlayer)) {
                UIGame::DisplayError(GameErrorData(GAMEERROR_ITEMS_CANNOT_BE_MOVED_AT_THIS_TIME), FALSE, NULL);
                return;
            }
            RetrieveItemFromSeasonStash(SLOT_PLAYER_SHARED_STASH);
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_RETRIEVE_ITEM_FROM_SEASON_STASH), cgtPSODefaults);
        }
    }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

inline void SeasonStashScreen::OnLeftTrigger()
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_TRIGGER) == ACTION_SCROLL_LIST &&
        !::Console::ControllerUtils::IsButtonDown(GetControllerIndex(), BUTTON_RIGHT_TRIGGER)) ScrollDetails(-SCROLLBOX_SCROLLDELTA);
}

inline void SeasonStashScreen::OnRightTrigger()
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER) == ACTION_SCROLL_LIST &&
        !::Console::ControllerUtils::IsButtonDown(GetControllerIndex(), BUTTON_LEFT_TRIGGER)) ScrollDetails(SCROLLBOX_SCROLLDELTA);
}

BOOL SeasonStashScreen::ShouldEnableRingIcon(const UIFilterType eFilterType)
{
    return m_tItemsProvider.GetItemsWithFilter(eFilterType).size() != 0;
}

inline BOOL SeasonStashScreen::CanOpenSelectedCategory()
{
    ItemCategory eCategory = m_tCategoriesView.GetSelectedItem();
    return CanOpenCategory(eCategory);
}

inline BOOL SeasonStashScreen::CanOpenCategory(const ItemCategory eCategory)
{
    return CategoryHasItems(eCategory, m_tItemsProvider, FALSE);
}

void SeasonStashScreen::UpdateBagItemActions(::Console::ControllerActionManager & tActionManager)
{
    switch (m_eStashState) {
    case BROWSE_CATEGORY_STATE:
        if (CanOpenSelectedCategory()) tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_CATEGORY, StringListFetch("ConsoleUI:Open").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
        break;
    case BROWSE_SUBCATEGORY_STATE:
        if (CanOpenSelectedCategory()) tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_CATEGORY, StringListFetch("ConsoleUI:Open").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        break;
    case BROWSE_GEMS_STATE:
    case BROWSE_ITEMS_STATE:
        UpdateNormalItemActions(tActionManager);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        break;
    }
}

void SeasonStashScreen::UpdateNormalItemActions(::Console::ControllerActionManager & tActionManager)
{
    if (CanScrollDetails()) {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
    }
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
    BOOL bIsIdentifyingAnItem = (GetPrimaryView() && GetPrimaryView()->IsIdentifyingItem()) ||
        (GetSecondaryView() && GetSecondaryView()->IsIdentifyingItem());
    if (!bIsIdentifyingAnItem && GetFocusedView()) {
        ACDID idSelectedItem = GetFocusedView()->GetSelectedItem();
        if (idSelectedItem != ACDID_INVALID && GetFocusedView() == GetPrimaryView()) {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_MOVE_TO_INVENTORY, StringListFetch("ConsoleInventory:MoveToInventory").str());
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_MOVE_TO_STASH, StringListFetch("ConsoleInventory:MoveToStash").str());
        }
    }
}

void SeasonStashScreen::UpdateControllerActions(::Console::ControllerActionManager & tActionManager)
{
    tActionManager.ClearAllActions();
    if (RingHasFocus()) {
        if (ShouldEnableRingIcon(GetFilter())) tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_SLOT, StringListFetch("ConsoleUI:Open").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
    } else if (GetFilter() == FILTER_BAG) UpdateBagItemActions(tActionManager);
    else UpdateNormalItemActions(tActionManager);
}

inline void SeasonStashScreen::EnableStashUI(BOOL bActive)
{
    UI_GetWindowFromID(UIWND_MAIL_WRAPPER).SetActive(!bActive);
    UI_GetWindowFromID(UIWND_SEASON_STASH_WRAPPER).SetActive(bActive);
    UI_GetWindowFromID(UIWND_SEASON_STASH_RING_WRAPPER).SetActive(bActive);
}

inline void SeasonStashScreen::UpdateCategoryUI(const UIID & idItemUI, const ItemCategory eCategory)
{
    BOOL bIsEnabled = CanOpenCategory(eCategory);
    UpdateCommonCategoryUI(idItemUI, eCategory, bIsEnabled, FALSE, FALSE);
}

inline void SeasonStashScreen::ItemRetrievedResult(BOOL bResult)
{
    if (m_bWaitingOnItem) {
        if (bResult) {
            m_tItemsProvider.DeleteItem(m_nIndexOfItemBeingRetrieved);
            UpdateStashSpace();
            m_tListView.RequestUpdate(KEEP_INDEX_SELECTED);
            m_tRightDetailsView.RequestUpdate(KEEP_INDEX_SELECTED);
        }
        m_bWaitingOnItem = FALSE;
    }
}

SeasonStashScreen::SeasonStashScreen()
    : m_tListView(m_tItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE),
      m_tRightDetailsView(m_tItemsProvider, UIDLG_INVENTORY_CONSOLE_EQUIPPED, 0),
      m_tCategoriesView(UIDLG_INVENTORY_CONSOLE_LIST, OnUpdateCategoryUI),
      m_tCategorySummaryView(UIDLG_INVENTORY_CONSOLE_EQUIPPED),
      m_nSelectedIndex(-1), m_idSelectedItem(ACDID_INVALID), m_bWaitingOnItem(FALSE), m_nIndexOfItemBeingRetrieved(-1)
{
    m_tListView.SetHeaderText(StringListFetch("ConsoleInventory:SeasonCache"));
    m_tCategoriesView.SetHeaderText(StringListFetch("ConsoleInventory:SeasonCache"));
}

SeasonStashScreen * sgptSeasonStashGlobals;  // original line 1180

void InitializeForGame() { sgptSeasonStashGlobals = new SeasonStashScreen; }

void DisposeForGame() { ::delete sgptSeasonStashGlobals; sgptSeasonStashGlobals = NULL; }

void OpenForController(const int32 nControllerIndex) { sgptSeasonStashGlobals->OpenForController(nControllerIndex); }

void Close() { sgptSeasonStashGlobals->Close(); }

void OnSeasonStashButton(const UIID &, const int32 nControllerIndex, const InputButton eInputButton)
{
    if (sgptSeasonStashGlobals && sgptSeasonStashGlobals->IsActive()) sgptSeasonStashGlobals->HandleButtonInput(nControllerIndex, eInputButton);
}

void OnUpdate()
{
    if (sgptSeasonStashGlobals && sgptSeasonStashGlobals->IsActive()) sgptSeasonStashGlobals->Update();
}

void OnUpdateCategoryUI(const UIID & idItemUI, const ItemCategory eCategory)
{
    if (sgptSeasonStashGlobals && sgptSeasonStashGlobals->IsActive()) sgptSeasonStashGlobals->UpdateCategoryUI(idItemUI, eCategory);
}

void ItemRetrievedResult(int32 nControllerIndex, BOOL bResult)
{
    if (sgptSeasonStashGlobals && sgptSeasonStashGlobals->IsActive()) sgptSeasonStashGlobals->ItemRetrievedResult(bResult);
}

BOOL HasSeasonStashItem(Player * ptPlayer)
{
    if (!::Console::GamerProfile::IsActive(ptPlayer->nControllerIndex)) return FALSE;
    D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
    // Original HasSeasonStashItem tests HeroStateData::uPlayerFlags bit 0.
    BOOL bIsHardcore = (ptPlayer->tHeroStateData.uPlayerFlags & 1) != 0;
    int32 nPartition = bIsHardcore ? 1 : 0;
    D3::Account::AccountPartition * ptPartition = tAccountDefinition.mutable_partitions(nPartition);
    return ptPartition->console_partition_data().seasonal_rollover_item_size() != 0;
}

} }
