// The console *hardcore* death screen (X/Client/UserInterface/Console/
// UIConsoleDeathMenuHardcore.cpp): the "your hardcore hero is dead" dialog,
// raised over the in-game dialog stack with the dead hero's portrait and one of
// seven per-class window frames shown.
//
// The whole translation unit is two free functions in
// `UIDeathMenuHardcore::Console`: the 2.6.2 DWARF carries no
// `UIDeathMenuHardcore` type at all, so the namespace spelling comes from the
// mangled names (`_ZN19UIDeathMenuHardcore7Console...`).
#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"

struct UIID;

namespace UIDeathMenuHardcore {
namespace Console {

// UIConsoleDeathMenuHardcore.cpp:35 (0x71003c5010) -- raise the hardcore death
// dialog. The parameter is the controller index the `.ui` binding passes, and
// the body never reads it: the hero it dresses the dialog with is
// `LocalPlayerGet()`'s.
void OpenForPlayer(int32 nPlayerIndex);

// UIConsoleDeathMenuHardcore.cpp:92 (0x71003c5260) -- the dialog's accept
// button; a bare tail branch to `UIDeathMenu_Console_HandleHardcoreDeath`.
void OnAccept(const UIID & id, const int32 nControllerIndex);

}  // namespace Console
}  // namespace UIDeathMenuHardcore
