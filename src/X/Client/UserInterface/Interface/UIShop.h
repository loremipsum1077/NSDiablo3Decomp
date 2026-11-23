// Adopted by hand; the DWARF records no definitions for this header (it only
// declares functions), so tools/gen_headers.py does not emit a reference copy.
// Original file: X/Client/UserInterface/Interface/UIShop.h

#pragma once

class ActorCommonData;

namespace UIShop {

// UIShop.cpp:1538 (2.6.2 0x003031c0).
void BuyItem(const ActorCommonData & tACDItem);

// UIShop.cpp:1656 (2.6.2 0x003031d0).
void SellItem(ActorCommonData & tACDItem);

}  // namespace UIShop
