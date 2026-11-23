// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleInventoryDyeView.h

#pragma once

#include "../../RActors/RActors.h"
#include "UIConsoleInventoryView.h"
#include "../../../Common/CommonTypes.h"

struct UIID;

namespace UIInventory {
namespace Console {
class UIInventoryDyeView : public UIInventory::Console::UIInventoryView {  // size 0x70
public:
    UIInventoryDyeView(const UIID &);
    void SetDye(const ACDID idDye);
    ACDID GetDye() const { return m_idDye; }  // line 33
    void Reset();
    void StartDyeing();
    BOOL IsFinishedDyeing() const;
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnUpdate();  // vtable slot 12
    virtual void OnSelectedItemChanged();  // vtable slot 10
    enum State {
        DEFAULT_STATE = 0,
        DYEING_STATE = 1,
        FINISHED_STATE = 2,
    };
private:
    ACDID m_idDye;  // 0x60
    DyeType m_eExpectedDyeType;  // 0x64
    UIInventory::Console::UIInventoryDyeView::State m_eState;  // 0x68
    void UpdateUI();
};
static_assert(sizeof(UIInventoryDyeView) == 0x70, "UIInventoryDyeView");

}  // namespace Console
}  // namespace UIInventory

// Functions defined in this file (bodies are not in the DWARF):
//   line    22  UIInventory::Console::UIInventoryDyeView::~UIInventoryDyeView()  [0x71003c9a20]
//   line    33  ACDID UIInventory::Console::UIInventoryDyeView::GetDye() const  [inlined into 1 functions]
