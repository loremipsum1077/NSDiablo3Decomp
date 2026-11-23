// X/Client/UserInterface/Console/UIConsoleMultiplayerDeathMenu.h -- provisional owning header.
//
// The 2.6.2 DWARF attributes every UIMultiplayerDeathMenu::Console function to
// X/Client/UserInterface/Console/UIConsoleMultiplayerDeathMenu.cpp (skeleton
// build/dwarf/headers/X/Client/UserInterface/Console/UIConsoleMultiplayerDeathMenu.cpp).
// Only the entry points UIConsoleFastAccessMenuHandler.cpp calls are declared here.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"

namespace UIMultiplayerDeathMenu {
namespace Console {

// X/Client/UserInterface/Console/UIConsoleMultiplayerDeathMenu.cpp
void sUpdateResurrectionsInProgress(int32 nControllerIndex);           // UIConsoleMultiplayerDeathMenu.cpp:36
void OpenForPlayer(const int32 nPlayerIndex, BOOL bForced);            // UIConsoleMultiplayerDeathMenu.cpp:64
void CloseForController(const int32 nControllerIndex);                 // UIConsoleMultiplayerDeathMenu.cpp:137
BOOL AllLocalPlayersDead();                                            // UIConsoleMultiplayerDeathMenu.cpp:161
BOOL CanReviveAtCorpse(const int32 nControllerIndex);                  // UIConsoleMultiplayerDeathMenu.cpp:184
BOOL PlayerHasResTimer(int32 nControllerIndex);                        // UIConsoleMultiplayerDeathMenu.cpp:238
void Update();                                                         // UIConsoleMultiplayerDeathMenu.cpp:270
void OnButton2(const int32 nControllerIndex);                          // UIConsoleMultiplayerDeathMenu.cpp:449
void OnButton3(const int32 nControllerIndex);                          // UIConsoleMultiplayerDeathMenu.cpp:532
void OnButton4(const int32 nControllerIndex);                          // UIConsoleMultiplayerDeathMenu.cpp:576
void OnBackButton(const int32 nControllerIndex);                       // UIConsoleMultiplayerDeathMenu.cpp:590
BOOL IsActiveForController(const int32 nControllerIndex);              // UIConsoleMultiplayerDeathMenu.cpp:598

void PlayerLeft(const int32 nControllerIndex);                         // UIConsoleMultiplayerDeathMenu.cpp:604
void RefreshForAllPlayers();                                           // UIConsoleMultiplayerDeathMenu.cpp:613

}  // namespace Console
}  // namespace UIMultiplayerDeathMenu
