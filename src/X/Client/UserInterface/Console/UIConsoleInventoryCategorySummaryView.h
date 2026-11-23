// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleInventoryCategorySummaryView.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryView.h"
#include "UIConsoleItemCategories.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Sigma/SigmaTypes.h"

struct UIID;

namespace UIInventory {
namespace Console {
typedef int32 ItemCategory;

class UIInventoryCategorySummaryView : public UIInventory::Console::UIInventoryView {  // size 0x68
public:
    UIInventoryCategorySummaryView(const UIID &);
    void SetCategory(const UIInventory::Console::ItemCategory eCategory) { m_eCategory = eCategory; RequestUpdate(RESET_SELECTION); }
    virtual BOOL CanAcceptFocus() const { return FALSE; }  // vtable slot 2
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnRequestedUpdate(const UIInventory::Console::MaintainSelectionOptions);  // vtable slot 11
private:
    UIInventory::Console::ItemCategory m_eCategory;  // 0x60
    void UpdateUI();
};
static_assert(sizeof(UIInventoryCategorySummaryView) == 0x68, "UIInventoryCategorySummaryView");

class UIPortraitFrameSummaryView : public UIInventory::Console::UIInventoryView {  // size 0x68
public:
    UIPortraitFrameSummaryView(const UIID &);
    void SetCategory(const UIInventory::Console::ItemCategory);
    virtual BOOL CanAcceptFocus() const { return FALSE; }  // vtable slot 2
    void SetACDID(ACDID idACD) {  // DWARF line 78, inlined only
        if (m_idACD != idACD) {
            m_idACD = idACD;
            RequestUpdate(RESET_SELECTION);
        }
    }
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnRequestedUpdate(const UIInventory::Console::MaintainSelectionOptions);  // vtable slot 11
private:
    void UpdateUI();
    ACDID m_idACD;  // 0x60
};
static_assert(sizeof(UIPortraitFrameSummaryView) == 0x68, "UIPortraitFrameSummaryView");

}  // namespace Console
}  // namespace UIInventory

// Functions defined in this file (bodies are not in the DWARF):
//   line    24  UIInventory::Console::UIInventoryCategorySummaryView::~UIInventoryCategorySummaryView()  [0x71003c8990]
//   line    30  void UIInventory::Console::UIInventoryCategorySummaryView::SetCategory(const UIInventory::Console::ItemCategory eCategory)  [inlined into 6 functions]
//   line    37  BOOL UIInventory::Console::UIInventoryCategorySummaryView::CanAcceptFocus() const  [0x71003c89d0]
//   line    59  UIInventory::Console::UIPortraitFrameSummaryView::~UIPortraitFrameSummaryView()  [0x71003c89e0]
//   line    71  BOOL UIInventory::Console::UIPortraitFrameSummaryView::CanAcceptFocus() const  [0x71003c8a20]
//   line    78  void UIInventory::Console::UIPortraitFrameSummaryView::SetACDID(ACDID idACD)  [inlined into 1 functions]
