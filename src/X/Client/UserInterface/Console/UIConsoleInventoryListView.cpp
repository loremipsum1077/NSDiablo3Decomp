#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryListView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIProgressBarControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIConsoleListBox & UI_GetConsoleListBoxFromID(const UIID & id);
UIProgressBar & UI_GetProgressBarFromID(const UIID & id);
void UI_SetControllerFocus(const UIID & id);
const UIID & UI_GetControllerFocus();
#include "ControllerActions.h"
namespace Console {




}  // namespace Console
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console {
namespace Screens {
ControllerActionManager & GetActionManager();
}  // namespace Screens
}  // namespace Console
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
namespace UIInventory {
namespace Console {
void UpdateViewItemUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const ACDID idACDOwner, const UIFilterType eFilterType, const BOOL bIsSelected, const BOOL bIsDetailedView, const BOOL bIsNew, const BOOL bIdentifyAllowed, const Blizzard::Time::Timestamp nExpiration);
void UpdateViewBlacksmithRecipeUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const CRefString sItemName, const ACDID idACDPlayer, const ACDID idACDOwner, const BOOL bIsNew);
void UpdateViewJewelerRecipeUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const CRefString sItemName, const int32 nRequiredLevel, const ACDID idACDPlayer, const ACDID idACDOwner, const BOOL bIsNew);
void UpdateViewMysticTransmogUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const ACDID idACDOwner);
void UpdateViewCosmeticItemUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const ACDID idACDOwner, const BOOL bIsNew);
void UpdateViewCraftingMaterialUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const ACDID idACDOwner, const BOOL bIsNew);
void UpdateViewBloodShardVendorUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const CRefString sItemName);
void UpdateViewItemSelectionUI(const UIID & idItemUI, const BOOL bIsSelected);
void UpdateViewArtisanSelectionUI(const UIID & idItemUI, const BOOL bIsSelected);
}  // namespace Console
}  // namespace UIInventory

using namespace UIInventory::Console;

namespace {
void UpdateSelectionUI(const UIInventoryViewItemsProvider::ProviderType eProviderType, const UIID & idItem, const BOOL fShowSelection);
void OnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal);
}

UIInventoryListView::UIInventoryListView(UIInventoryViewItemsProvider & tItemsProvider, const UIID & idRoot, const BOOL fRequiresItemsToAcceptFocus)
    : UIInventoryView(idRoot)
    , m_tDataSource(tItemsProvider)
    , m_idLastSelectedItem(ACDID_INVALID)
    , m_pfnOnSelectionChanged(NULL)
    , m_fRequiresItemsToAcceptFocus(fRequiresItemsToAcceptFocus)
{
    {
        UIID idStatusLine = GetUIIDNone();
        idStatusLine.SetFromParent(GetRootUIID(), "BagSpace.ValueAmount");
        SetStatusLineUIID(idStatusLine);
    }
}

void UIInventoryListView::OnShow(const BOOL bShow)
{
    UIConsoleListBox & tListBox = GetListBox();

    if (bShow)
    {
        tListBox.SetOnSelChangeFunction(OnListSelectionChanged);
    }
    else
    {
        tListBox.SetOnSelChangeFunction(NULL);
        m_idLastSelectedItem = ACDID_INVALID;
    }

    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);
}

void UIInventoryListView::OnEnable(const BOOL bEnable)
{
    UIID idDimmer;
    idDimmer.SetFromParent(GetRootUIID(), "Dimmer");
    UI_GetWindowFromID(idDimmer).SetActive(!bEnable);

    if (!bEnable)
    {
        UI_SetControllerFocus(GetUIIDNone());
    }
}

void UIInventoryListView::OnGotFocus()
{
    int32 nIndex = GetIndexOfItem(m_idLastSelectedItem);
    if (m_tDataSource.GetNumItems() > 0 && nIndex == -1)
    {
        nIndex = 0;
    }

    UIConsoleListBox & tListBox = GetListBox();
    UI_SetControllerFocus(tListBox.GetID());
    tListBox.SetSelectedIndex(nIndex);

    int32 nNumItems = m_tDataSource.GetNumItems();
    if (nNumItems == 1)
    {
        ACDID idSelectedItem = GetSelectedItem();
        if (idSelectedItem != ACDID_INVALID)
        {
            m_tDataSource.UnMarkAsNew(idSelectedItem);
        }
    }
}

void UIInventoryListView::OnLostFocus()
{
    m_idLastSelectedItem = GetSelectedItem();
    UI_SetControllerFocus(GetUIIDNone());
}

void UIInventoryListView::OnRequestedUpdate(const MaintainSelectionOptions eSelectionType)
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
    case KEEP_ITEM_SELECTED:
        if (nNumItems > 0)
        {
            nNewSelectedIndex = GetIndexOfItem(idOldSelectedItem);
            if (nNewSelectedIndex == -1)
            {
                nNewSelectedIndex = Math::Pin(nOldSelectedIndex, 0, nNumItems - 1);
            }
            tListBox.SetFirstVisibleItemIndex(Math::Pin(nOldFirstVisibleIndex, 0, Math::Max(Math::Min(nNewSelectedIndex, nNumItems - tListBox.GetNumVisibleItems()), 0)));
        }
        break;

    case KEEP_INDEX_SELECTED:
        nNewSelectedIndex = Math::Pin(nOldSelectedIndex, 0, nNumItems - 1);
        tListBox.SetFirstVisibleItemIndex(Math::Pin(nOldFirstVisibleIndex, 0, Math::Max(nNumItems - tListBox.GetNumVisibleItems(), 0)));
        break;

    case RESET_SELECTION:
        m_idLastSelectedItem = ACDID_INVALID;
        break;
    }

    tListBox.SetSelectedIndex(nNewSelectedIndex);

    if (nNewSelectedIndex > 0)
    {
        m_idLastSelectedItem = GetSelectedItem();
    }
}

void UIInventoryListView::OnUpdate()
{
    if (m_tDataSource.GetProviderType() == UIInventoryViewItemsProvider::ITEM_PROVIDER_REAL_ITEMS)
    {
        UpdateIdentifyingProgressBar();
    }
}

UIID UIInventoryListView::GetHeaderUIID() const
{
    UIID idHeader = UIIDCreateFromParent(GetRootUIID(), ".WindowTitle");
    return idHeader;
}

void UIInventoryListView::OnParentScreenChanged()
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
    }
}

void UIInventoryListView::OnSelectedItemChanged()
{
    int32 nIndex = GetIndexOfItem(GetSelectedItem());
    GetListBox().SetSelectedIndex(nIndex);
}

inline UIConsoleListBox & UIInventoryListView::GetListBox()
{
    UIID idListBox = GetUIIDNone();
    idListBox.SetFromParent(GetRootUIID(), "ItemList");
    return UI_GetConsoleListBoxFromID(idListBox);
}

int32 UIInventoryListView::GetIndexOfSelectedItem()
{
    return GetListBox().GetSelectedIndex();
}

void UIInventoryListView::HandleListSelectionChanged(const int32 nOldVal, const int32 nNewVal)
{
    UIConsoleListBox & tListBox = GetListBox();

    int32 nSelectedIndex = tListBox.GetSelectedIndex();
    ACDID idSelectedItem = nSelectedIndex != -1 ? m_tDataSource.GetItems()[nSelectedIndex] : ACDID_INVALID;
    SelectItem(idSelectedItem);

    for (int32 i = 0; i < tListBox.GetNumVisibleItems(); ++i)
    {
        int32 nIndex = tListBox.GetFirstVisibleItemIndex() + i;
        const UIID & idItem = tListBox.GetItemUIID(nIndex);
        if (idItem != GetUIIDNone())
        {
            BOOL fShowSelection = nIndex == nSelectedIndex && IsFocused();
            UpdateSelectionUI(m_tDataSource.GetProviderType(), idItem, fShowSelection);
        }
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

    if (m_pfnOnSelectionChanged != NULL)
    {
        m_pfnOnSelectionChanged(this);
    }

    if (GetParentScreen() != NULL && GetParentScreen()->GetFocusedView() == this)
    {
        if (m_tDataSource.GetNumItems() == 1 && idSelectedItem != ACDID_INVALID)
        {
            m_tDataSource.UnMarkAsNew(idSelectedItem);
        }
        else if (nOldVal >= 0 && nNewVal != -1 && nOldVal < m_tDataSource.GetNumItems())
        {
            ACDID idOldSelectedItem = m_tDataSource.GetItems()[nOldVal];
            m_tDataSource.UnMarkAsNew(idOldSelectedItem);
        }
    }
}

inline int32 UIInventoryListView::GetIndexOfItem(const ACDID idItem) const
{
    const ACDIDArray & arItems = m_tDataSource.GetItems();
    return blzext::vector_find(arItems, idItem);
}

void UIInventoryListView::UpdateIdentifyingProgressBar()
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

        UIID idProgress = UIIDCreateFromParent(idItem, ".LayoutRoot.IdentifyProgress");
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

void UIInventoryListView::SetAnalogStickInputFilter(const ::Console::AnalogStickInputFilter & tStickFilter)
{
    GetListBox().SetAnalogStickInputFilter(tStickFilter);
}

inline ListViewDataSource::ListViewDataSource(UIInventoryViewItemsProvider & tItemsProvider)
    : m_tItemsProvider(tItemsProvider)
{
}

inline const ACDIDArray & ListViewDataSource::GetItems() const
{
    return m_tItemsProvider.GetItems();
}

int32 ListViewDataSource::GetNumItems() const
{
    return m_tItemsProvider.GetItems().size();
}

UIControlSourceKey ListViewDataSource::GetSourceKeyForItem(const int32) const
{
    const LPCSTR NORMAL_ITEM_TEMPLATE = "ConsoleInventoryListEquipmentTemplate:ConsoleInventoryListEquipmentTemplate_main";
    const UIControlSourceKey NORMAL_ITEM_SOURCE_KEY = UIControlSourceKey::Parse(NORMAL_ITEM_TEMPLATE, SNO_NONE, NULL, NULL);

    const LPCSTR BLACKSMITH_RECIPE_TEMPLATE = "ConsoleInventoryListBlacksmithTemplate:ConsoleInventoryListBlacksmithTemplate_main";
    const UIControlSourceKey BLACKSMITH_RECIPE_SOURCE_KEY = UIControlSourceKey::Parse(BLACKSMITH_RECIPE_TEMPLATE, SNO_NONE, NULL, NULL);

    const LPCSTR JEWELER_RECIPE_TEMPLATE = "ConsoleInventoryListJewelerTemplate:ConsoleInventoryListJewelerTemplate_main";
    const UIControlSourceKey JEWELER_RECIPE_SOURCE_KEY = UIControlSourceKey::Parse(JEWELER_RECIPE_TEMPLATE, SNO_NONE, NULL, NULL);

    const LPCSTR MYSTIC_TRANSMOG_TEMPLATE = "ConsoleInventoryListMysticTemplate:ConsoleInventoryListMysticTemplate_main";
    const UIControlSourceKey MYSTIC_TRANSMOG_SOURCE_KEY = UIControlSourceKey::Parse(MYSTIC_TRANSMOG_TEMPLATE, SNO_NONE, NULL, NULL);

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
    else if (eProviderType == UIInventoryViewItemsProvider::ITEM_PROVIDER_MYSTIC_TRANSMOGS)
    {
        return MYSTIC_TRANSMOG_SOURCE_KEY;
    }
    else if (eProviderType == UIInventoryViewItemsProvider::ITEM_PROVIDER_BLOODSHARD_VENDOR)
    {
        return BLOODSHARD_VENDOR_SOURCE_KEY;
    }
    return NORMAL_ITEM_SOURCE_KEY;
}

void ListViewDataSource::InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32)
{
}

void ListViewDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    const ACDIDArray & arItems = m_tItemsProvider.GetItems();
    if (nIndex < 0 || nIndex >= (int32)arItems.size())
    {
        return;
    }

    ActorCommonData * ptACDItem = ACDTryToGet(arItems[nIndex]);
    if (ptACDItem == NULL)
    {
        return;
    }

    const Player * ptPlayer = m_tItemsProvider.GetPlayer();
    if (ptPlayer == NULL)
    {
        return;
    }

    BOOL fShowSelection = tListBox.GetSelectedIndex() == nIndex && UI_GetControllerFocus() == tListBox.GetID();
    BOOL bIsNew = m_tItemsProvider.ItemIsNew(ptACDItem->id) || m_tItemsProvider.ItemIsNewElite(ptACDItem->id);
    BOOL bIdentifyAllowed = m_tItemsProvider.IdentifyIsAllowed();
    Blizzard::Time::Timestamp nExpiration = m_tItemsProvider.GetItemExpiration(nIndex);

    UIInventoryViewItemsProvider::ProviderType eProviderType = m_tItemsProvider.GetProviderType();
    switch (eProviderType)
    {
    case UIInventoryViewItemsProvider::ITEM_PROVIDER_REAL_ITEMS:
        UpdateViewItemUI(idItemUI, *ptACDItem, ptPlayer->idPlayerACD, m_tItemsProvider.GetOwner(), m_tItemsProvider.GetFilter(), fShowSelection, FALSE, bIsNew, bIdentifyAllowed, nExpiration);
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
    case UIInventoryViewItemsProvider::ITEM_PROVIDER_COSMETIC_ITEMS:
        UpdateViewCosmeticItemUI(idItemUI, *ptACDItem, ptPlayer->idPlayerACD, m_tItemsProvider.GetOwner(), bIsNew);
        break;
    case UIInventoryViewItemsProvider::ITEM_PROVIDER_CRAFTING_MATERIALS:
        UpdateViewCraftingMaterialUI(idItemUI, *ptACDItem, ptPlayer->idPlayerACD, m_tItemsProvider.GetOwner(), bIsNew);
        break;
    default:
        break;
    }

    UpdateSelectionUI(m_tItemsProvider.GetProviderType(), idItemUI, fShowSelection);
}

inline void ListViewDataSource::UnMarkAsNew(const ACDID idItem)
{
    m_tItemsProvider.UnMarkAsNew(idItem);
}

inline UIInventoryViewItemsProvider::ProviderType ListViewDataSource::GetProviderType() const
{
    return m_tItemsProvider.GetProviderType();
}

namespace {

void UpdateSelectionUI(const UIInventoryViewItemsProvider::ProviderType eProviderType, const UIID & idItem, const BOOL fShowSelection)
{
    if (eProviderType == UIInventoryViewItemsProvider::ITEM_PROVIDER_REAL_ITEMS)
    {
        UpdateViewItemSelectionUI(idItem, fShowSelection);
    }
    else
    {
        UpdateViewArtisanSelectionUI(idItem, fShowSelection);
    }
}

void OnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal)
{
    UIInventoryListView * ptListView = (UIInventoryListView *)UI_GetWindowFromID(id).GetUserData();
    if (ptListView != NULL)
    {
        ptListView->HandleListSelectionChanged(nOldVal, nNewVal);
    }
}

}
