// 0x7100374540 and 0x7100374590 sit between the UIInventoryAddSocketsView destructor and SetAddSocketsItem.
// In 2.7.7 they are the base-class state setters that the overriding Reset (0x71003742c0) and StartAddingSockets
// (0x7100374320) call directly before their storyboard work; they also fill vtable slots 14/15 of the other
// add-sockets views (0x10a62a8, 0x10a6348, 0x10a63e8). Kept in their own unit so those callers do not inline them.
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryAddSocketsView.h"

// Probably: UIInventoryAddSocketsView base Reset (virtual slot 14): m_eState = DEFAULT_STATE.
extern "C" void sub_7100374540(UIInventory::Console::UIInventoryAddSocketsView * ptView)
{
    ptView->m_eState = UIInventory::Console::UIInventoryAddSocketsView::DEFAULT_STATE;
}

// Probably: UIInventoryAddSocketsView base StartAddingSockets (virtual slot 15): m_eState = ADDING_SOCKETS_STATE.
extern "C" void sub_7100374590(UIInventory::Console::UIInventoryAddSocketsView * ptView)
{
    ptView->m_eState = UIInventory::Console::UIInventoryAddSocketsView::ADDING_SOCKETS_STATE;
}
