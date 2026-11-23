#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryDetailsView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIProgressBarControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Common/ItemsSwig.h





// X/Common/Items.h
typedef int32 AncientRank;
// X/Common/Items.cpp
AncientRank ItemGetAncientRank(const ActorCommonData & tACDItem);
// X/Common/Definitions/StringList.cpp
CRefString StringListFetch(LPCSTR szLabel);
// X/Client/Input/InputNX64.cpp
bool InputIsSingleJoycon(int32_t nControllerIndex);
// X/Client/UserInterface/UI.cpp
UIControl & UI_GetControlFromID(const UIID & id);
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
UIConsoleListBox & UI_GetConsoleListBoxFromID(const UIID & id);
UIProgressBar & UI_GetProgressBarFromID(const UIID & id);
void UI_SetControllerFocus(const UIID & id);
int32 UI_GetPrimaryUIController();
#include "ControllerActions.h"
namespace Console {




}  // namespace Console
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console {
namespace Screens {
ControllerActionManager & GetActionManager();
}  // namespace Screens
}  // namespace Console
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils {
namespace Console {
BOOL CanScrollContentInScrollBox(const UIID & idScrollBox);
void ResetScrollBox(const UIID & idScrollBox);
}  // namespace Console
}  // namespace UIUtils
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.h
namespace UIInventory {
namespace Console {
enum DetailsViewMode {
    DETAILS_VIEW = 0,
};
}  // namespace Console
}  // namespace UIInventory
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
namespace UIInventory {
namespace Console {
void ShowDetailsMode(const UIID & idParent, const DetailsViewMode eViewMode);
void UpdateViewItemUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const ACDID idACDOwner, const UIFilterType eFilterType, const BOOL bIsSelected, const BOOL bIsDetailedView, const BOOL bIsNew, const BOOL bIdentifyAllowed, const Blizzard::Time::Timestamp nExpiration);
void UpdateViewBlacksmithRecipeUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const CRefString sItemName, const ACDID idACDPlayer, const ACDID idACDOwner, const BOOL bIsNew);
void UpdateViewJewelerRecipeUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const CRefString sItemName, const int32 nRequiredLevel, const ACDID idACDPlayer, const ACDID idACDOwner, const BOOL bIsNew);
void UpdateViewMysticTransmogUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const ACDID idACDOwner);
void UpdateViewBloodShardVendorUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const CRefString sItemName);
void UpdateViewNoPlayerItemUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const CRefString sItemName, const PlayerClass ePlayerClass, const BOOL bIsMale);
void ShowItemDetails(const UIID & idDetailsParent, const BOOL bShow);
}  // namespace Console
}  // namespace UIInventory

using namespace UIInventory::Console;

namespace {
void OnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal);
void PopulateItemDetails(const UIInventoryViewItemsProvider * ptItemsProvider, ACDID idItem, const UIID & idDetailsParent, const BOOL bShowStatRanges);
}

UIInventoryDetailsView::UIInventoryDetailsView(UIInventoryViewItemsProvider & tItemsProvider, const UIID & idRoot, const uint32 dwFlags)
    : UIInventoryView(idRoot)
    , m_tDataSource(tItemsProvider)
    , m_dwFlags(dwFlags)
    , m_bShowStatRanges(FALSE)
{
    {
        UIID idStatusLine = GetUIIDNone();
        idStatusLine.SetFromParent(GetRootUIID(), "LayoutRoot.ItemDetails.Item.SellValue.ValueAmount");
        SetStatusLineUIID(idStatusLine);
    }
}

void UIInventoryDetailsView::OnShow(const BOOL bShow)
{
    UIConsoleListBox & tListBox = GetListBox();

    tListBox.SetOnSelChangeFunction(bShow ? OnListSelectionChanged : NULL);

    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);

    if (!bShow)
    {
        UIID idAncientItem = GetUIIDNone();
        idAncientItem.SetFromParent(GetRootUIID(), "LayoutRoot.AncientEquipped");
        UI_GetWindowFromID(idAncientItem).SetActive(FALSE);

        UIID idPrimalAncientItem = GetUIIDNone();
        idPrimalAncientItem.SetFromParent(GetRootUIID(), "LayoutRoot.PrimalAncientEquipped");
        UI_GetWindowFromID(idPrimalAncientItem).SetActive(FALSE);
    }
}

void UIInventoryDetailsView::OnEnable(const BOOL bEnable)
{
    if (!bEnable)
    {
        UI_SetControllerFocus(GetUIIDNone());
    }
}

void UIInventoryDetailsView::OnGotFocus()
{
    UIConsoleListBox & tListBox = GetListBox();
    UI_SetControllerFocus(tListBox.GetID());
}

void UIInventoryDetailsView::OnLostFocus()
{
    UI_SetControllerFocus(GetUIIDNone());
}

inline UIConsoleListBox & UIInventoryDetailsView::GetListBox()
{
    UIID idListBox = GetUIIDNone();
    idListBox.SetFromParent(GetRootUIID(), "LayoutRoot.ItemDetails.Item.ItemList");
    return UI_GetConsoleListBoxFromID(idListBox);
}

void UIInventoryDetailsView::HandleListSelectionChanged(const int32 nOldVal, const int32 nNewVal)
{
    UIConsoleListBox & tListBox = GetListBox();

    int32 nSelectedIndex = tListBox.GetSelectedIndex();
    ACDID idSelectedItem = nSelectedIndex != -1 ? m_tDataSource.GetItems()[nSelectedIndex] : ACDID_INVALID;
    SelectItem(idSelectedItem);

    UIID idScrollbox;
    idScrollbox.SetFromParent(GetRootUIID(), "LayoutRoot.ItemDetails.Item.ScrollBox");
    UIUtils::Console::ResetScrollBox(idScrollbox);

    UpdateUI();

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

    if (GetParentScreen() != NULL && GetParentScreen()->GetFocusedView() == this)
    {
        if (m_tDataSource.GetNumItems() == 1 && idSelectedItem != ACDID_INVALID)
        {
            m_tDataSource.UnMarkAsNew(idSelectedItem);
        }
        else if (nOldVal != -1 && nNewVal != -1)
        {
            ACDID idOldSelectedItem = m_tDataSource.GetItems()[nOldVal];
            m_tDataSource.UnMarkAsNew(idOldSelectedItem);
        }
    }
}

void UIInventoryDetailsView::UpdateIdentifyingProgressBar()
{
    if (GetParentScreen() == NULL)
    {
        DoneIdentifying(FALSE);
        return;
    }

    if (GetIdentifyingItemACD() == ACDID_INVALID)
    {
        DoneIdentifying(FALSE);
    }
    else if (IsIdentifying())
    {
        if (GetIdentifyingProgress() >= 1.0f)
        {
            if (HasSendIdentifyingRequest())
            {
                if (IsFinishedIdentifying())
                {
                    DoneIdentifying(TRUE);
                }
            }
            else
            {
                SendIdentifyingRequest();
            }
        }
    }
    else
    {
        StartIdentifying();
    }

    UIID idActiveProgress = GetUIIDNone();
    UIConsoleListBox & tListBox = GetListBox();
    int32 nNumItems = m_tDataSource.GetNumItems();
    for (int32 i = 0; i < nNumItems; ++i)
    {
        const UIID & idItem = tListBox.GetItemUIID(i);
        if (idItem == GetUIIDNone())
        {
            continue;
        }

        UIID idProgress = GetUIIDNone();
        idProgress.SetFromParent(idItem, "LayoutRoot.IdentifyProgress");
        if (m_tDataSource.GetItems()[i] == GetIdentifyingItemACD())
        {
            idActiveProgress = idProgress;
        }
        else
        {
            UI_GetProgressBarFromID(idProgress).SetExtentsAndPosition(0.0f, 1.0f, 0.0f, -1.0f);
        }
    }

    if (idActiveProgress == GetUIIDNone())
    {
        return;
    }

    if (IsIdentifying())
    {
        UI_GetProgressBarFromID(idActiveProgress).SetExtentsAndPosition(0.0f, 1.0f, GetIdentifyingProgress(), -1.0f);
    }
}

void UIInventoryDetailsView::SetAnalogStickInputFilter(const ::Console::AnalogStickInputFilter & tStickFilter)
{
    GetListBox().SetAnalogStickInputFilter(tStickFilter);
}

BOOL UIInventoryDetailsView::CanScrollWithTriggers() const
{
    return UIUtils::Console::CanScrollContentInScrollBox(GetScrollBoxUIID());
}

UIID UIInventoryDetailsView::GetScrollBoxUIID() const
{
    UIID idItemDetails;
    idItemDetails.SetFromParent(GetRootUIID(), "LayoutRoot.ItemDetails.Item.ScrollBox");
    return idItemDetails;
}

void UIInventoryDetailsView::ToggleStatRangeDisplay()
{
    m_bShowStatRanges = !m_bShowStatRanges;
    RequestUpdate(KEEP_ITEM_SELECTED);
}

void UIInventoryDetailsView::OnRequestedUpdate(const MaintainSelectionOptions eSelectionType)
{
    UIConsoleListBox & tListBox = GetListBox();

    int32 nOldSelectedIndex = tListBox.GetSelectedIndex();
    ACDID idOldSelectedItem = GetSelectedItem();
    int32 nNewSelectedIndex = 0;
    int32 nOldFirstVisibleIndex = tListBox.GetFirstVisibleItemIndex();

    ClearSelectedItem();
    tListBox.ResetAllItems();

    int32 nNumItems = tListBox.GetNumItems();

    switch (eSelectionType)
    {
    case RESET_SELECTION:
        break;

    case KEEP_ITEM_SELECTED:
        nNewSelectedIndex = GetIndexOfItem(idOldSelectedItem);
        if (nNewSelectedIndex == -1 && nOldSelectedIndex < nNumItems)
        {
            nNewSelectedIndex = nOldSelectedIndex;
        }
        if (nNewSelectedIndex == -1 && nNumItems > 0)
        {
            nNewSelectedIndex = 0;
        }
        break;

    case KEEP_INDEX_SELECTED:
        nNewSelectedIndex = Math::Pin(nOldSelectedIndex, 0, nNumItems - 1);
        if (nOldFirstVisibleIndex >= 0 && nOldFirstVisibleIndex < nNumItems)
        {
            tListBox.SetFirstVisibleItemIndex(nOldFirstVisibleIndex);
        }
        break;
    }

    tListBox.SetSelectedIndex(nNewSelectedIndex);
    UpdateUI();
}

void UIInventoryDetailsView::OnUpdate()
{
    if (m_tDataSource.CanProvideUnidentifiedItems())
    {
        UpdateIdentifyingProgressBar();
    }
}

void UIInventoryDetailsView::OnParentScreenChanged()
{
    UIConsoleListBox & tListBox = GetListBox();
    if (GetParentScreen() != NULL)
    {
        tListBox.SetUserData(this);
        tListBox.SetDataSource(m_tDataSource);
    }
    else
    {
        tListBox.SetUserData(NULL);
        SelectItem(ACDID_INVALID);
    }
}

void UIInventoryDetailsView::OnSelectedItemChanged()
{
    int32 nIndex = GetIndexOfItem(GetSelectedItem());
    GetListBox().SetSelectedIndex(nIndex);
}

#line 405
// NON_MATCHING: the original branches on `if (fNoItems)` as two tests (selection part, then the
// item count) while keeping the combined value for SetActive; ours tests the combined value once,
// which frees x26 and shifts every stack slot by 8.

void UIInventoryDetailsView::UpdateUI()

{
    if (GetParentScreen() == NULL)
    {
        return;
    }


    ACDID idSelectedItem = GetSelectedItem();
    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    const UIID & idRoot = GetRootUIID();


    ShowDetailsMode(idRoot, DETAILS_VIEW);


    UIID idNoItems = GetUIIDNone();
    idNoItems.SetFromParent(idRoot, "LayoutRoot.ItemDetails.NoItem");

    BOOL fNoItems = (m_tDataSource.GetNumItems() == 0);

    if (!(m_dwFlags & (1 << IDV_CAN_ACCEPT_FOCUS_BIT)) && !ptACDItem)
    {
        fNoItems = TRUE;
    }

    UI_GetWindowFromID(idNoItems).SetActive(fNoItems);

    if (fNoItems)
    {
        UIID idNoItemsMessage;
        idNoItemsMessage.SetFromParent(idNoItems, "NoItemMessage");

        UIText & tNoItemText = UI_GetTextFromID(idNoItemsMessage);
        tNoItemText.SetActive(TRUE);

        CRefString sMsg;
        if (m_dwFlags & (1 << IDV_PROMPT_TO_EQUIP_ITEMS_BIT))
        {

            if (InputIsSingleJoycon(UI_GetPrimaryUIController()))
            {
                sMsg = StringListFetch("ConsoleInventory:NoItemEquippedNX64");
            }
            else
            {
                sMsg = StringListFetch("ConsoleInventory:NoItemEquipped");
            }



        }
        else
        {
            sMsg = StringListFetch("ConsoleInventory:NoItemSelected");
        }
        tNoItemText.SetText(sMsg, FALSE);
    }

    UIID idHaveItems = GetUIIDNone();
    idHaveItems.SetFromParent(idRoot, "LayoutRoot.ItemDetails.Item");
    m_tDataSource.UpdateDetailsUI(idHaveItems, idSelectedItem, m_bShowStatRanges);

    AncientRank eAncientRank = ptACDItem != NULL ? ItemGetAncientRank(*ptACDItem) : ANCIENT_RANK_NONE;


    UIID idAncientItem = GetUIIDNone();
    idAncientItem.SetFromParent(idRoot, "LayoutRoot.AncientEquipped");
    BOOL bSelectedItemIsAncient = ptACDItem != NULL && eAncientRank == ANCIENT_RANK_ANCIENT &&

        !ItemIsUnidentified(*ptACDItem);
    UI_GetWindowFromID(idAncientItem).SetActive(bSelectedItemIsAncient);


    UIID idPrimalAncientItem = GetUIIDNone();
    idPrimalAncientItem.SetFromParent(idRoot, "LayoutRoot.PrimalAncientEquipped");
    BOOL bSelectedItemIsPrimalAncient = ptACDItem != NULL && eAncientRank == ANCIENT_RANK_PRIMAL_ANCIENT &&

        !ItemIsUnidentified(*ptACDItem);
    UI_GetWindowFromID(idPrimalAncientItem).SetActive(bSelectedItemIsPrimalAncient);

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}
#line 409

inline int32 UIInventoryDetailsView::GetIndexOfItem(const ACDID idItem) const
{
    return m_tDataSource.GetIndexOfItem(idItem);
}

inline DetailsViewDataSource::DetailsViewDataSource(UIInventoryViewItemsProvider & tItemsProvider)
    : m_tItemsProvider(tItemsProvider)
{
}

inline int32 DetailsViewDataSource::GetIndexOfItem(const ACDID idItem) const
{
    const ACDIDArray & arItems = GetItems();
    int32 nNumItems = arItems.size();
    for (int32 i = 0; i < nNumItems; ++i)
    {
        if (arItems[i] == idItem)
        {
            return i;
        }
    }
    return -1;
}

int32 DetailsViewDataSource::GetNumItems() const
{
    return GetItems().size();
}

UIControlSourceKey DetailsViewDataSource::GetSourceKeyForItem(const int32) const
{
    const LPCSTR NORMAL_ITEM_TEMPLATE = "ConsoleInventoryListEquipmentTemplate:ConsoleInventoryListEquipmentTemplate_main";
    const UIControlSourceKey NORMAL_ITEM_SOURCE_KEY = UIControlSourceKey::Parse(NORMAL_ITEM_TEMPLATE, SNO_NONE, NULL, NULL);

    const LPCSTR BLACKSMITH_RECIPE_TEMPLATE = "ConsoleInventoryListBlacksmithTemplate:ConsoleInventoryListBlacksmithTemplate_main";
    const UIControlSourceKey BLACKSMITH_RECIPE_SOURCE_KEY = UIControlSourceKey::Parse(BLACKSMITH_RECIPE_TEMPLATE, SNO_NONE, NULL, NULL);

    const LPCSTR JEWELER_RECIPE_TEMPLATE = "ConsoleInventoryListJewelerTemplate:ConsoleInventoryListJewelerTemplate_main";
    const UIControlSourceKey JEWELER_RECIPE_SOURCE_KEY = UIControlSourceKey::Parse(JEWELER_RECIPE_TEMPLATE, SNO_NONE, NULL, NULL);

    const LPCSTR BLOODSHARD_VENDOR_TEMPLATE = "ConsoleInventoryListBloodshardVendorTemplate:ConsoleInventoryListBloodshardVendorTemplate_main";
    const UIControlSourceKey BLOODSHARD_VENDOR_SOURCE_KEY = UIControlSourceKey::Parse(BLOODSHARD_VENDOR_TEMPLATE, SNO_NONE, NULL, NULL);

    UIInventoryViewItemsProvider::ProviderType eProviderType = m_tItemsProvider.GetProviderType();
    if (eProviderType == UIInventoryViewItemsProvider::ITEM_PROVIDER_BLACKSMITH_RECIPES)
    {
        return BLACKSMITH_RECIPE_SOURCE_KEY;
    }
    else if (eProviderType == UIInventoryViewItemsProvider::ITEM_PROVIDER_JEWELER_RECIPES)
    {
        return JEWELER_RECIPE_SOURCE_KEY;
    }
    else if (eProviderType == UIInventoryViewItemsProvider::ITEM_PROVIDER_BLOODSHARD_VENDOR)
    {
        return BLOODSHARD_VENDOR_SOURCE_KEY;
    }
    return NORMAL_ITEM_SOURCE_KEY;
}

void DetailsViewDataSource::InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32)
{
}

void DetailsViewDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    ActorCommonData * ptACDItem = ACDTryToGet(m_tItemsProvider.GetItems()[nIndex]);
    if (ptACDItem == NULL)
    {
        return;
    }

    UIInventoryViewItemsProvider::ProviderType eProviderType = m_tItemsProvider.GetProviderType();
    const Player * ptPlayer = m_tItemsProvider.GetPlayer();
    if (eProviderType != UIInventoryViewItemsProvider::ITEM_PROVIDER_NO_PLAYER_ITEMS && ptPlayer == NULL)
    {
        return;
    }

    BOOL bIsNew = m_tItemsProvider.ItemIsNew(ptACDItem->id) || m_tItemsProvider.ItemIsNewElite(ptACDItem->id);
    BOOL bIdentifyAllowed = m_tItemsProvider.IdentifyIsAllowed();
    Blizzard::Time::Timestamp nExpiration = m_tItemsProvider.GetItemExpiration(nIndex);

    switch (eProviderType)
    {
    case UIInventoryViewItemsProvider::ITEM_PROVIDER_REAL_ITEMS:
        UpdateViewItemUI(idItemUI, *ptACDItem, ptPlayer->idPlayerACD, m_tItemsProvider.GetOwner(), m_tItemsProvider.GetFilter(), TRUE, TRUE, bIsNew, bIdentifyAllowed, nExpiration);
        break;
    case UIInventoryViewItemsProvider::ITEM_PROVIDER_BLACKSMITH_RECIPES:
        UpdateViewBlacksmithRecipeUI(idItemUI, *ptACDItem, m_tItemsProvider.GetItemName(nIndex), ptPlayer->idPlayerACD, m_tItemsProvider.GetOwner(), bIsNew);
        break;
    case UIInventoryViewItemsProvider::ITEM_PROVIDER_JEWELER_RECIPES:
        UpdateViewJewelerRecipeUI(idItemUI, *ptACDItem, m_tItemsProvider.GetItemName(nIndex), m_tItemsProvider.GetRequiredLevel(nIndex), ptPlayer->idPlayerACD, m_tItemsProvider.GetOwner(), bIsNew);
        break;
    case UIInventoryViewItemsProvider::ITEM_PROVIDER_MYSTIC_TRANSMOGS:
        UpdateViewMysticTransmogUI(idItemUI, *ptACDItem, ptPlayer->idPlayerACD, m_tItemsProvider.GetOwner());
        break;
    case UIInventoryViewItemsProvider::ITEM_PROVIDER_BLOODSHARD_VENDOR:
        UpdateViewBloodShardVendorUI(idItemUI, *ptACDItem, ptPlayer->idPlayerACD, m_tItemsProvider.GetItemName(nIndex));
        break;
    case UIInventoryViewItemsProvider::ITEM_PROVIDER_NO_PLAYER_ITEMS:
        {
            LeaderboardEquippedItem * ptNoPlayerProvider = static_cast<LeaderboardEquippedItem *>(&m_tItemsProvider);
            UpdateViewNoPlayerItemUI(idItemUI, *ptACDItem, ptNoPlayerProvider->GetItemName(nIndex), ptNoPlayerProvider->GetClass(), ptNoPlayerProvider->GetIsMale());
        }
        break;
    default:
        break;
    }
}

void DetailsViewDataSource::UpdateDetailsUI(const UIID & idDetailsUI, const ACDID idACDItem, const BOOL bShowStatRanges)
{
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    UI_GetWindowFromID(idDetailsUI).SetActive(ptACDItem != NULL);
    if (ptACDItem != NULL)
    {
        PopulateItemDetails(&m_tItemsProvider, idACDItem, idDetailsUI, bShowStatRanges);
    }
}

inline void DetailsViewDataSource::UnMarkAsNew(const ACDID idItem)
{
    m_tItemsProvider.UnMarkAsNew(idItem);
}

namespace {

void OnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal)
{
    UIInventoryDetailsView * ptDetailsView = (UIInventoryDetailsView *)UI_GetControlFromID(id).GetUserData();
    if (ptDetailsView != NULL)
    {
        ptDetailsView->HandleListSelectionChanged(nOldVal, nNewVal);
    }
}

void PopulateItemDetails(const UIInventoryViewItemsProvider * ptItemsProvider, ACDID idItem, const UIID & idDetailsParent, const BOOL bShowStatRanges)
{
    if (idItem == ACDID_INVALID)
    {
        return;
    }

    if (ptItemsProvider->GetPlayer() == NULL && ptItemsProvider->GetProviderType() != UIInventoryViewItemsProvider::ITEM_PROVIDER_NO_PLAYER_ITEMS)
    {
        return;
    }

    ActorCommonData * ptACDItem = ACDTryToGet(idItem);
    if (ptACDItem == NULL)
    {
        return;
    }

    BOOL bIsIdentified = !ItemIsUnidentified(*ptACDItem);
    ShowItemDetails(idDetailsParent, bIsIdentified);
    if (!bIsIdentified)
    {
        return;
    }

    CRefString sWarningMsg = ptItemsProvider->GetWarningMessage();
    UIID idWarningPanel = UIIDCreateFromParent(idDetailsParent, ".ScrollBox.ScrollingStack.LoadoutWarning");
    UI_GetWindowFromID(idWarningPanel).SetActive(sWarningMsg.Length() != 0);
    if (sWarningMsg.Length() != 0)
    {
        UIID idWarningText = UIIDCreateFromParent(idWarningPanel, ".LoadoutWarningMessage");
        UI_GetTextFromID(idWarningText).SetText(sWarningMsg, FALSE);
    }

    UIID idStats = UIIDCreateFromParent(idDetailsParent, ".ScrollBox.ScrollingStack.Affixes");
    CRefString sItemDescription = ptItemsProvider->GetItemDescription(idItem, bShowStatRanges);
    UI_GetTextFromID(idStats).SetText(sItemDescription, FALSE);
}

}
