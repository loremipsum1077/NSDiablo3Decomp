// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleInventoryDetailsView.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryView.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Sigma/SigmaTypes.h"

namespace Console { class AnalogStickInputFilter; }
struct UIControlSourceKey;
struct UIID;

namespace UIInventory {
namespace Console {
class DetailsViewDataSource : public ConsoleListBoxDataSource {  // size 0x10
public:
    virtual ~DetailsViewDataSource() {}  // vtable slot 0; line 25
    DetailsViewDataSource(UIInventoryViewItemsProvider &);

    const ACDIDArray & GetItems() const  // line 32
    {
        return m_tItemsProvider.GetItems();
    }

    UIFilterType GetFilter() const;
    UIInventoryViewItemsProvider::ProviderType GetProviderType() const;

    BOOL CanProvideUnidentifiedItems() const  // line 53
    {
        return m_tItemsProvider.CanProvideUnidentifiedItems();
    }

    int32 GetIndexOfItem(const ACDID idItem) const;
    virtual int32 GetNumItems() const;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
    void UpdateDetailsUI(const UIID & idDetailsUI, const ACDID idACDItem, const BOOL bShowStatRanges);
    void UnMarkAsNew(const ACDID idItem);
private:
    UIInventoryViewItemsProvider & m_tItemsProvider;  // 0x08
    DetailsViewDataSource & operator=(const DetailsViewDataSource &);
};
static_assert(sizeof(DetailsViewDataSource) == 0x10, "DetailsViewDataSource");

class UIInventoryDetailsView : public UIInventoryView {  // size 0x78
    enum {
        IDV_CAN_ACCEPT_FOCUS_BIT = 0,
        IDV_PROMPT_TO_EQUIP_ITEMS_BIT = 1,
    };
public:
    UIInventoryDetailsView(UIInventoryViewItemsProvider &, const UIID &, const uint32);
    virtual ~UIInventoryDetailsView() {}  // line 103
    void HandleListSelectionChanged(const int32 nOldVal, const int32 nNewVal);
    void UpdateIdentifyingProgressBar();

    virtual BOOL CanAcceptFocus() const  // vtable slot 2; line 125
    {
        if ((m_dwFlags & (1 << IDV_CAN_ACCEPT_FOCUS_BIT)) && m_tDataSource.GetNumItems() != 0)
        {
            return UIInventoryView::CanAcceptFocus();
        }
        return FALSE;
    }

    void SetPromptToEquipItems(const BOOL fVal)  // line 132
    {
        if (fVal)
            m_dwFlags |= (1 << IDV_PROMPT_TO_EQUIP_ITEMS_BIT);
        else
            m_dwFlags &= ~(1 << IDV_PROMPT_TO_EQUIP_ITEMS_BIT);
        UpdateUI();
    }

    void SetAnalogStickInputFilter(const ::Console::AnalogStickInputFilter & tStickFilter);
    virtual BOOL CanScrollWithTriggers() const;  // vtable slot 3
    virtual UIID GetScrollBoxUIID() const;  // vtable slot 4
    void ToggleStatRangeDisplay();
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnEnable(const BOOL bEnable);  // vtable slot 7
    virtual void OnGotFocus();  // vtable slot 8
    virtual void OnLostFocus();  // vtable slot 9
    virtual void OnRequestedUpdate(const MaintainSelectionOptions eSelectionType);  // vtable slot 11
    virtual void OnUpdate();  // vtable slot 12
    virtual void OnParentScreenChanged();  // vtable slot 5
    virtual void OnSelectedItemChanged();  // vtable slot 10
private:
    DetailsViewDataSource m_tDataSource;  // 0x60
    uint32 m_dwFlags;  // 0x70
    BOOL m_bShowStatRanges;  // 0x74
    int32 GetIndexOfItem(const ACDID idItem) const;
    UIConsoleListBox & GetListBox();
    void UpdateUI();
};
static_assert(sizeof(UIInventoryDetailsView) == 0x78, "UIInventoryDetailsView");

}  // namespace Console
}  // namespace UIInventory
