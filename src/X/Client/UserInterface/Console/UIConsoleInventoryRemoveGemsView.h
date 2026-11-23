// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleInventoryRemoveGemsView.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryView.h"
#include "../../../Sigma/SigmaTypes.h"

struct UIID;

namespace UIInventory {
namespace Console {
// Blank lines keep the original line numbers.


class UIInventoryRemoveGemsView : public UIInventory::Console::UIInventoryView {  // size 0x68; line 21
public:

    // The constructor is defined inline in the original header (line 24); its body is not
    // in the DWARF, so the initializers are inferred from the state fields.
    UIInventoryRemoveGemsView(const UIID & idRoot) : UIInventoryView(idRoot), m_eState(START_STATE), m_nItemNumFilledSockets(0) {}
    void Reset();
    void StartRemoving();
    BOOL IsFinishedRemoving() const;
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnUpdate();  // vtable slot 12
    enum State {
        START_STATE = 0,
        REMOVING_STATE = 1,
        FINISHED_STATE = 2,
    };
private:
    UIInventory::Console::UIInventoryRemoveGemsView::State m_eState;  // 0x60
    int32 m_nItemNumFilledSockets;  // 0x64
    void UpdateUI();
};
static_assert(sizeof(UIInventoryRemoveGemsView) == 0x68, "UIInventoryRemoveGemsView");

}  // namespace Console
}  // namespace UIInventory
