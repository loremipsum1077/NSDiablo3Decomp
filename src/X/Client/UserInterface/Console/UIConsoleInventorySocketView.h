// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleInventorySocketView.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryView.h"
#include "../UIID.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Sigma/RefString.h"

struct Player;

namespace UIInventory {
namespace Console {
class UIInventorySocketView : public UIInventory::Console::UIInventoryView {  // size 0x90
public:
    UIInventorySocketView(const UIID &);
    void SetJewel(const ACDID idJewel);
    ACDID GetJewel() const { return m_idJewel; }  // line 33
    void Reset();
    void ResetToSocketAgain(const ACDID idJewel);
    void SocketItem();
    BOOL IsFinishedSocketing() const;
    virtual UIID GetScrollBoxUIID() const;  // vtable slot 4
    virtual BOOL CanScrollWithTriggers() const;  // vtable slot 3
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnUpdate();  // vtable slot 12
    virtual void OnSelectedItemChanged();  // vtable slot 10
private:
    ACDID m_idJewel;  // 0x60
    int32 m_nItemNumFilledSockets;  // 0x64
    UIID m_idStoryboard;  // 0x68
    CRefString m_sJewelStats;  // 0x70
    BOOL m_bJewelIsUnidentified;  // 0x88
    static CRefString GetJewelStatsString(const ACDID idJewelACD, const ACDID idItemACD, const Player * ptPlayer);
    void UpdateUI();
};
static_assert(sizeof(UIInventorySocketView) == 0x90, "UIInventorySocketView");

}  // namespace Console
}  // namespace UIInventory

// Functions defined in this file (bodies are not in the DWARF):
//   line    22  UIInventory::Console::UIInventorySocketView::~UIInventorySocketView()  [0x71003cb080 (+1 more), inlined into 3 functions]
//   line    33  ACDID UIInventory::Console::UIInventorySocketView::GetJewel() const  [inlined into 4 functions]
