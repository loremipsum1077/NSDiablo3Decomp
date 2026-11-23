#include "UIConsoleInventoryCategoryView.h"
#include "UIConsoleInventoryScreen.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../UI.h"
#include "../UIStoryboard.h"
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../../Sigma/Math/MathGeneral.h"

// UI.h declarations not yet adopted.
const UIID & UI_GetControllerFocus();
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
namespace {
void UpdateSelectionUI(const UIID & idItem, const BOOL fShowSelection);
void OnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal);
}
namespace UIInventory { namespace Console {
UIInventoryCategoryView::UIInventoryCategoryView(const UIID & idRoot, CategoryItemUIUpdateCallback fnItemUIUpdate)
    : UIInventoryView(idRoot), m_tDataSource(fnItemUIUpdate),
      m_eLastSelectedCategory(CATEGORY_NONE), m_pfnOnSelectionChanged(NULL)
{
    UIID idStatusLine = GetUIIDNone();
    idStatusLine.SetFromParent(GetRootUIID(), "BagSpace.ValueAmount");
    SetStatusLineUIID(idStatusLine);
}
void UIInventoryCategoryView::OnShow(const BOOL bShow)
{
    UIConsoleListBox & tListBox = GetListBox();
    tListBox.SetOnSelChangeFunction(bShow ? OnListSelectionChanged : NULL);
    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);
}
void UIInventoryCategoryView::OnEnable(const BOOL bEnable)
{
    UIID idDimmer;
    idDimmer.SetFromParent(GetRootUIID(), "Dimmer");
    UI_GetWindowFromID(idDimmer).SetActive(!bEnable);
    if (!bEnable) UI_SetControllerFocus(GetUIIDNone());
}
void UIInventoryCategoryView::OnGotFocus()
{
    int32 nIndex = GetIndexOfCategory(m_eLastSelectedCategory);
    int32 nNumItems = m_tDataSource.GetCategories().size();
    if (nNumItems > 0 && nIndex == -1) nIndex = 0;
    UIConsoleListBox & tListBox = GetListBox();
    UI_SetControllerFocus(tListBox.GetID());
    tListBox.SetSelectedIndex(nIndex);
}
void UIInventoryCategoryView::OnLostFocus()
{
    m_eLastSelectedCategory = GetSelectedItem();
    UI_SetControllerFocus(GetUIIDNone());
}
void UIInventoryCategoryView::OnRequestedUpdate(const MaintainSelectionOptions eSelectionType)
{
    UIConsoleListBox & tListBox = GetListBox();
    int32 nOldSelectedIndex = tListBox.GetSelectedIndex();
    ItemCategory eOldSelectedCategory = GetSelectedItem();
    int32 nNewSelectedIndex = 0;
    int32 nOldFirstVisibleIndex = tListBox.GetFirstVisibleItemIndex();
    ClearSelectedItem();
    tListBox.ResetAllItems();
    int32 nNumItems = tListBox.GetNumItems();
    if (eSelectionType == KEEP_ITEM_SELECTED) {
        nNewSelectedIndex = GetIndexOfCategory(eOldSelectedCategory);
        if (nNewSelectedIndex == -1 && nOldSelectedIndex < nNumItems)
            nNewSelectedIndex = nOldSelectedIndex;
        if (nNewSelectedIndex == -1 && nNumItems > 0)
            nNewSelectedIndex = 0;
    } else if (eSelectionType == KEEP_INDEX_SELECTED) {
        nNewSelectedIndex = Math::Pin(nOldSelectedIndex, 0, nNumItems - 1);
        if (nOldFirstVisibleIndex >= 0 && nOldFirstVisibleIndex < nNumItems)
            tListBox.SetFirstVisibleItemIndex(nOldFirstVisibleIndex);
    }
    tListBox.SetSelectedIndex(nNewSelectedIndex);
}
void UIInventoryCategoryView::OnParentScreenChanged()
{
    UIConsoleListBox & tListBox = GetListBox();
    if (GetParentScreen()) {
        tListBox.SetUserData(this);
        tListBox.SetDataSource(m_tDataSource);
    } else tListBox.SetUserData(NULL);
}
void UIInventoryCategoryView::OnSelectedItemChanged()
{
    int32 nIndex = GetIndexOfCategory(GetSelectedItem());
    GetListBox().SetSelectedIndex(nIndex);
}
UIID UIInventoryCategoryView::GetHeaderUIID() const
{
    UIID idHeader = UIIDCreateFromParent(GetRootUIID(), ".WindowTitle");
    return idHeader;
}
inline UIConsoleListBox & UIInventoryCategoryView::GetListBox()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    UIID idListBox = GetUIIDNone();
    idListBox.SetFromParent(GetRootUIID(), "ItemList");
    return UI_GetConsoleListBoxFromID(idListBox);
}
void UIInventoryCategoryView::HandleListSelectionChanged(const int32 nOldVal, const int32 nNewVal)
{
    UIConsoleListBox & tListBox = GetListBox();
    int32 nSelectedIndex = tListBox.GetSelectedIndex();
    ItemCategory eSelectedCategory = nSelectedIndex == -1 ? CATEGORY_NONE : m_tDataSource.GetCategories()[nSelectedIndex];
    SelectItem(static_cast<ACDID>(eSelectedCategory));
    for (int32 i = 0; i < tListBox.GetNumVisibleItems(); ++i) {
        int32 nIndex = tListBox.GetFirstVisibleItemIndex() + i;
        const UIID & idItem = tListBox.GetItemUIID(nIndex);
        BOOL fShowSelection = nIndex == nSelectedIndex && IsFocused();
        UpdateSelectionUI(idItem, fShowSelection);
    }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    if (m_pfnOnSelectionChanged) m_pfnOnSelectionChanged(this);
}
int32 UIInventoryCategoryView::GetIndexOfCategory(const ItemCategory eCategory) const
{
    const CategoryArray & arCategories = m_tDataSource.GetCategories();
    return blzext::vector_find(arCategories, eCategory);
}
CategoryViewDataSource::CategoryViewDataSource(CategoryItemUIUpdateCallback fnItemUIUpdate)
    : m_fnItemUIUpdate(fnItemUIUpdate) {}
int32 CategoryViewDataSource::GetNumItems() const { return m_arCategories.size(); }
UIControlSourceKey CategoryViewDataSource::GetSourceKeyForItem(const int32) const
{
    const LPCSTR CATEGORY_ITEM_TEMPLATE = "ConsoleInventoryListCategoryTemplate:ConsoleInventoryListCategoryTemplate_main";
    const UIControlSourceKey CATEGORY_ITEM_SOURCE_KEY = UIControlSourceKey::Parse(CATEGORY_ITEM_TEMPLATE, -1, NULL, NULL);
    return CATEGORY_ITEM_SOURCE_KEY;
}
void CategoryViewDataSource::InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32) {}
void CategoryViewDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    if (nIndex != -1 && m_fnItemUIUpdate && !m_arCategories.empty())
        m_fnItemUIUpdate(idItemUI, m_arCategories[nIndex]);
    BOOL fShowSelection = tListBox.GetSelectedIndex() == nIndex && UI_GetControllerFocus() == tListBox.GetID();
    UpdateSelectionUI(idItemUI, fShowSelection);
}
} }
namespace {
void UpdateSelectionUI(const UIID & idItem, const BOOL fShowSelection)
{
    if (idItem == GetUIIDNone()) return;
    UIID idHighlight = GetUIIDNone();
    idHighlight.SetFromParent(idItem, "LayoutRoot.Selected");
    UI_GetWindowFromID(idHighlight).SetActive(fShowSelection);
    if (fShowSelection) {
        UIID idStoryboard;
        idStoryboard.SetFromParent(idItem, "ItemListHighlight");
        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Stop();
        tStoryboard.Play(FALSE, 0.0f);
    }
}
void OnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal)
{
    UIInventory::Console::UIInventoryCategoryView * ptListView = static_cast<UIInventory::Console::UIInventoryCategoryView *>(UI_GetWindowFromID(id).GetUserData());
    if (ptListView) ptListView->HandleListSelectionChanged(nOldVal, nNewVal);
}
}
