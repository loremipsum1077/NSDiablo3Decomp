// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleInventoryListView.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryView.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Sigma/SigmaTypes.h"

class CRefString;
namespace Console { class AnalogStickInputFilter; }

namespace UIInventory {
namespace Console {
class ListViewDataSource : public ConsoleListBoxDataSource {  // size 0x10
public:
    virtual ~ListViewDataSource() {}  // vtable slot 0; line 24
    ListViewDataSource(UIInventory::Console::UIInventoryViewItemsProvider &);
    const ACDIDArray & GetItems() const;
    virtual int32 GetNumItems() const;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
    void UnMarkAsNew(const ACDID idItem);
    UIInventory::Console::UIInventoryViewItemsProvider::ProviderType GetProviderType() const;
private:
    UIInventory::Console::UIInventoryViewItemsProvider & m_tItemsProvider;  // 0x08
    UIInventory::Console::ListViewDataSource & operator=(const UIInventory::Console::ListViewDataSource &);
};
static_assert(sizeof(ListViewDataSource) == 0x10, "ListViewDataSource");

class UIInventoryListView : public UIInventory::Console::UIInventoryView {  // size 0x88
public:
    typedef void (*ListViewSelectionChangedCallback)(UIInventory::Console::UIInventoryListView *);
public:
    UIInventoryListView(UIInventory::Console::UIInventoryViewItemsProvider &, const UIID &, const BOOL);
    virtual ~UIInventoryListView() {}  // vtable slot 0; line 72
    void SetSelectionChangedCallback(UIInventory::Console::UIInventoryListView::ListViewSelectionChangedCallback);
    void HandleListSelectionChanged(const int32 nOldVal, const int32 nNewVal);
    virtual BOOL CanAcceptFocus() const  // vtable slot 2; line 93
    {
        if (m_fRequiresItemsToAcceptFocus && m_tDataSource.GetNumItems() == 0)
        {
            return FALSE;
        }
        return UIInventoryView::CanAcceptFocus();
    }
    void UpdateIdentifyingProgressBar();
    void SetAnalogStickInputFilter(const ::Console::AnalogStickInputFilter & tStickFilter);
    int32 GetIndexOfSelectedItem();
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnEnable(const BOOL bEnable);  // vtable slot 7
    virtual void OnGotFocus();  // vtable slot 8
    virtual void OnLostFocus();  // vtable slot 9
    virtual void OnRequestedUpdate(const UIInventory::Console::MaintainSelectionOptions eSelectionType);  // vtable slot 11
    virtual void OnUpdate();  // vtable slot 12
    virtual UIID GetHeaderUIID() const;  // vtable slot 13
    virtual void OnParentScreenChanged();  // vtable slot 5
    virtual void OnSelectedItemChanged();  // vtable slot 10
private:
    UIInventory::Console::ListViewDataSource m_tDataSource;  // 0x60
    ACDID m_idLastSelectedItem;  // 0x70
    UIInventory::Console::UIInventoryListView::ListViewSelectionChangedCallback m_pfnOnSelectionChanged;  // 0x78
    BOOL m_fRequiresItemsToAcceptFocus;  // 0x80
    UIInventoryListView(const UIInventory::Console::UIInventoryListView &);
    UIInventory::Console::UIInventoryListView & operator=(const UIInventory::Console::UIInventoryListView &);
    int32 GetIndexOfItem(const ACDID idItem) const;
    UIConsoleListBox & GetListBox();
    void UpdateStatusLineUI(CRefString);
};
static_assert(sizeof(UIInventoryListView) == 0x88, "UIInventoryListView");

}  // namespace Console
}  // namespace UIInventory
