// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleInventoryGemCombineView.h

#pragma once

#include "UIConsoleInventoryView.h"
#include "../../../Sigma/SigmaTypes.h"

struct UIID;

namespace UIInventory {
namespace Console {
// Blank lines keep the original line numbers.



class UIInventoryGemCombineView : public UIInventory::Console::UIInventoryView {  // size 0x70; line 22
public:

    // The constructor is defined inline in the original header (line 25); its body is not
    // in the DWARF, so the initializers are inferred from the member list.
    UIInventoryGemCombineView(UIInventory::Console::CrafterRecipes & tItemsProvider, const UIID & idRoot) : UIInventoryView(idRoot), m_tItemsProvider(tItemsProvider), m_eState(START_STATE) {}
    void Reset();
    void StartCombining();
    BOOL IsFinishedCombining() const;
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnUpdate();  // vtable slot 12
    enum State {
        START_STATE = 0,
        COMBINING_STATE = 1,
        FINISHED_STATE = 2,
    };
private:
    UIInventory::Console::CrafterRecipes & m_tItemsProvider;  // 0x60
    UIInventory::Console::UIInventoryGemCombineView::State m_eState;  // 0x68
    void UpdateUI();
};
static_assert(sizeof(UIInventoryGemCombineView) == 0x70, "UIInventoryGemCombineView");

}  // namespace Console
}  // namespace UIInventory
