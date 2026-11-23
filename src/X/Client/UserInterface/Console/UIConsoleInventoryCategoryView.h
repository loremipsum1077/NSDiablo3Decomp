// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleInventoryCategoryView.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "UIConsoleInventoryView.h"
#include "UIConsoleItemCategories.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../../../Sigma/SigmaTypes.h"

class CRefString;
struct UIControlSourceKey;
struct UIID;

namespace UIInventory {
namespace Console {
typedef blz::vector<int, blz::allocator<int> > CategoryArray;

typedef void (*CategoryItemUIUpdateCallback)(const UIID &, const UIInventory::Console::ItemCategory);

class CategoryViewDataSource : public ConsoleListBoxDataSource {  // size 0x28
public:
    CategoryViewDataSource(UIInventory::Console::CategoryItemUIUpdateCallback);
    void AddCategory(const UIInventory::Console::ItemCategory eCategory) { m_arCategories.push_back(eCategory); }
    void ClearCategories() { m_arCategories.clear(); }
    const UIInventory::Console::CategoryArray & GetCategories() const { return m_arCategories; }
    virtual int32 GetNumItems() const;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
private:
    UIInventory::Console::CategoryArray m_arCategories;  // 0x08
    UIInventory::Console::CategoryItemUIUpdateCallback m_fnItemUIUpdate;  // 0x20
    UIInventory::Console::CategoryViewDataSource & operator=(const UIInventory::Console::CategoryViewDataSource &);
};
static_assert(sizeof(CategoryViewDataSource) == 0x28, "CategoryViewDataSource");

class UIInventoryCategoryView : public UIInventory::Console::UIInventoryView {  // size 0x98
    typedef void (*CategoryViewSelectionChangedCallback)(UIInventory::Console::UIInventoryCategoryView *);
public:
    UIInventoryCategoryView(const UIID &, UIInventory::Console::CategoryItemUIUpdateCallback);
    void AddCategory(const UIInventory::Console::ItemCategory eCategory) { m_tDataSource.AddCategory(eCategory); }
    void ClearCategories() { m_tDataSource.ClearCategories(); }
    void SetSelectionChangedCallback(UIInventory::Console::UIInventoryCategoryView::CategoryViewSelectionChangedCallback);
    void HandleListSelectionChanged(const int32 nOldVal, const int32 nNewVal);
    virtual BOOL CanAcceptFocus() const { return TRUE; }  // vtable slot 2
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnEnable(const BOOL bEnable);  // vtable slot 7
    virtual void OnGotFocus();  // vtable slot 8
    virtual void OnLostFocus();  // vtable slot 9
    virtual void OnRequestedUpdate(const UIInventory::Console::MaintainSelectionOptions eSelectionType);  // vtable slot 11
    virtual void OnParentScreenChanged();  // vtable slot 5
    virtual void OnSelectedItemChanged();  // vtable slot 10
    virtual UIID GetHeaderUIID() const;  // vtable slot 13
private:
    UIInventory::Console::CategoryViewDataSource m_tDataSource;  // 0x60
    UIInventory::Console::ItemCategory m_eLastSelectedCategory;  // 0x88
    UIInventory::Console::UIInventoryCategoryView::CategoryViewSelectionChangedCallback m_pfnOnSelectionChanged;  // 0x90
    UIInventoryCategoryView(const UIInventory::Console::UIInventoryCategoryView &);
    UIInventory::Console::UIInventoryCategoryView & operator=(const UIInventory::Console::UIInventoryCategoryView &);
    int32 GetIndexOfCategory(const UIInventory::Console::ItemCategory eCategory) const;
    UIConsoleListBox & GetListBox();
    void UpdateStatusLineUI(CRefString);
    void UpdateScrollArrows();
};
static_assert(sizeof(UIInventoryCategoryView) == 0x98, "UIInventoryCategoryView");

}  // namespace Console
}  // namespace UIInventory

// Functions defined in this file (bodies are not in the DWARF):
//   line    29  UIInventory::Console::CategoryViewDataSource::~CategoryViewDataSource()  [0x710016d3e0 (+1 more), inlined into 11 functions]
//   line    35  void UIInventory::Console::CategoryViewDataSource::AddCategory(const UIInventory::Console::ItemCategory eCategory)  [inlined into 4 functions]
//   line    41  void UIInventory::Console::CategoryViewDataSource::ClearCategories()  [inlined into 4 functions]
//   line    82  UIInventory::Console::UIInventoryCategoryView::~UIInventoryCategoryView()  [0x71003c92f0 (+1 more), inlined into 7 functions]
//   line    91  void UIInventory::Console::UIInventoryCategoryView::AddCategory(const UIInventory::Console::ItemCategory eCategory)  [inlined into 4 functions]
//   line    97  void UIInventory::Console::UIInventoryCategoryView::ClearCategories()  [inlined into 4 functions]
//   line   114  BOOL UIInventory::Console::UIInventoryCategoryView::CanAcceptFocus() const  [0x71003c93d0]
