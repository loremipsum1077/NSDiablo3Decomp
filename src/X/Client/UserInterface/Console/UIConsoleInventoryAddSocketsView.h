// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleInventoryAddSocketsView.h

#pragma once

#include "UIConsoleInventoryView.h"
#include "../../../Common/CommonTypes.h"

struct UIID;

namespace UIInventory { namespace Console { class UIInventoryAddSocketsView; } }
// 2.7.7 out-of-line state setters (UIConsoleInventoryAddSocketsView.cpp), called by Reset and StartAddingSockets.
extern "C" void sub_7100374540(UIInventory::Console::UIInventoryAddSocketsView *);
extern "C" void sub_7100374590(UIInventory::Console::UIInventoryAddSocketsView *);

namespace UIInventory {
namespace Console {
class UIInventoryAddSocketsView : public UIInventory::Console::UIInventoryView {  // size 0x70
public:
    UIInventoryAddSocketsView(const UIID &);
    void SetAddSocketsItem(const ACDID idAddSocketsItem);
    ACDID GetAddSocketsItem() const { return m_idAddSocketsItem; }  // line 33
    void SetAddSocketsRecipient(const ACDID idAddSocketsRecipient);
    ACDID GetAddSocketsRecipient() const { return m_idAddSocketsRecipient; }  // line 44
    void Reset();
    void StartAddingSockets();
    BOOL IsFinishedAddingSockets() const;
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnUpdate();  // vtable slot 12
    virtual void OnSelectedItemChanged();  // vtable slot 10
    enum State {
        DEFAULT_STATE = 0,
        ADDING_SOCKETS_STATE = 1,
        FINISHED_STATE = 2,
    };
private:
    ACDID m_idAddSocketsItem;  // 0x60
    ACDID m_idAddSocketsRecipient;  // 0x64
    UIInventory::Console::UIInventoryAddSocketsView::State m_eState;  // 0x68
    void UpdateUI();
    friend void ::sub_7100374540(UIInventoryAddSocketsView *);
    friend void ::sub_7100374590(UIInventoryAddSocketsView *);
};
static_assert(sizeof(UIInventoryAddSocketsView) == 0x70, "UIInventoryAddSocketsView");

}  // namespace Console
}  // namespace UIInventory

// Functions defined in this file (bodies are not in the DWARF):
//   line    22  UIInventory::Console::UIInventoryAddSocketsView::~UIInventoryAddSocketsView()  [0x71003b1980]
//   line    33  ACDID UIInventory::Console::UIInventoryAddSocketsView::GetAddSocketsItem() const  [inlined into 1 functions]
//   line    44  ACDID UIInventory::Console::UIInventoryAddSocketsView::GetAddSocketsRecipient() const  [inlined into 2 functions]
