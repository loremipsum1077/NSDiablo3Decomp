// `X/Client/UserInterface/Interface/UIShop.cpp` -- the PC vendor screen.  The two
// functions below are all that either original build emits for this TU, and both
// are empty: the bodies were gutted when the console shop screen
// (`UIShopConsole.cpp`) took the work over, but the entry points stayed so the
// authored callbacks still resolve.  Each is a single `ret` in 2.6.2
// (0x003031c0 / 0x003031d0, size 4 each).
#include "UIShop.h"

namespace UIShop {

void BuyItem(const ActorCommonData & tACDItem)
{
}

void SellItem(ActorCommonData & tACDItem)
{
}

}  // namespace UIShop
