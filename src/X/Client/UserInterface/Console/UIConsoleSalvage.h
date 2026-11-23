// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleSalvage.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryView.h"
#include "../../../Common/GameBalance.h"
#include <stddef.h>
#include <stdint.h>

struct SalvageResultsMessage;
struct UIID;

namespace UIInventory {
namespace Console {
struct SalvagedItem {  // size 0x8
    GBID gbidItem;  // 0x00
    int32 nCount;  // 0x04
};
static_assert(sizeof(SalvagedItem) == 0x8, "SalvagedItem");

class SalvageResults {  // size 0x20
public:
    // Defined in UIConsoleSalvage.cpp (lines 258-334); inlined there, no out-of-line copies.
    inline SalvageResults();
    inline void SetFromMessage(const SalvageResultsMessage & tMsg);
    inline void Clear();
    inline size_t Count() const;
    inline const UIInventory::Console::SalvagedItem operator[](size_t nIndex) const;
private:
    static const int32 MAX_DISPLAYED_SALVAGE_RESULTS = 3;
    UIInventory::Console::SalvagedItem m_arSalvagedItems[3];  // 0x00
    size_t m_nNumItems;  // 0x18
};
static_assert(sizeof(SalvageResults) == 0x20, "SalvageResults");

class UIInventorySalvageView : public UIInventory::Console::UIInventoryView {  // size 0x90
public:
    UIInventorySalvageView(UIInventory::Console::SingleItemProvider &, const UIID &);
    void Reset();
    void WaitForConfirmation();
    void SetSalvageResultsFromMessage(const SalvageResultsMessage & tMsg);
    void ClearSalvageResults();
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnUpdate();  // vtable slot 12
    virtual void OnSelectedItemChanged();  // vtable slot 10
    enum State {
        START_STATE = 0,
        READY_STATE = 1,
        ANIMATING_STATE = 2,
        FINISHED_STATE = 3,
    };
private:
    UIInventory::Console::UIInventorySalvageView::State m_eState;  // 0x60
    UIInventory::Console::SingleItemProvider & m_tItemProvider;  // 0x68
    UIInventory::Console::SalvageResults m_tSalvageResults;  // 0x70
    void UpdateUI();
    void UpdateSalvageResults();
};
static_assert(sizeof(UIInventorySalvageView) == 0x90, "UIInventorySalvageView");

}  // namespace Console
}  // namespace UIInventory

// Functions defined in this file (bodies are not in the DWARF):
//   line    56  UIInventory::Console::UIInventorySalvageView::~UIInventorySalvageView()  [0x71003cee00]
