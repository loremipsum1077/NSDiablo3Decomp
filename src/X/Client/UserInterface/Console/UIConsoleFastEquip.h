// X/Client/UserInterface/Console/UIConsoleFastEquip.h -- provisional owning header.
//
// The 2.6.2 DWARF attributes every UIFastEquip::Console function to
// X/Client/UserInterface/Console/UIConsoleFastEquip.cpp (skeleton
// build/dwarf/headers/X/Client/UserInterface/Console/UIConsoleFastEquip.cpp). Only the
// entry points UIConsoleFastAccessMenuHandler.cpp calls are declared here; the ItemLists /
// PlayerItemList layouts are not adopted yet.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../UIID.h"

namespace UIFastEquip {
namespace Console {

// X/Client/UserInterface/Console/UIConsoleFastEquip.cpp
void OpenFastEquip(const int32 nControllerIndex);                      // UIConsoleFastEquip.cpp:310
void OnUp(const UIID &, const int32 nControllerIndex);                 // UIConsoleFastEquip.cpp:341
void OnDown(const UIID &, const int32 nControllerIndex);               // UIConsoleFastEquip.cpp:365
void OnButton1(const UIID &, const int32 nControllerIndex);            // UIConsoleFastEquip.cpp:390
void OnButton3(const UIID &, const int32 nControllerIndex);            // UIConsoleFastEquip.cpp:557
void OnButton2(const UIID &, const int32 nControllerIndex);            // UIConsoleFastEquip.cpp:611
void OnButton4(const UIID &, const int32 nControllerIndex);            // UIConsoleFastEquip.cpp:635
void OnLStick(const int32 nControllerIndex);                           // UIConsoleFastEquip.cpp:688
BOOL IsActiveForController(const int32 nControllerIndex);              // UIConsoleFastEquip.cpp:1307

}  // namespace Console
}  // namespace UIFastEquip
