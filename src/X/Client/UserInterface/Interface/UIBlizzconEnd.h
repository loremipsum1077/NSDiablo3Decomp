// Owning header for the functions defined in X/Client/UserInterface/Interface/UIBlizzconEnd.cpp.
// The DWARF records no header of its own for that translation unit, so this file holds the
// declarations its callers need, taken from the definitions' signatures.
//   X/Client/UserInterface/Interface/UIBlizzconEnd.cpp:60   void UIBlizzconEnd::InitializeForGame()     [0x71003d4470]
//   X/Client/UserInterface/Interface/UIBlizzconEnd.cpp:69   void UIBlizzconEnd::DisposeForGame()        [0x71003d44c0]
//   X/Client/UserInterface/Interface/UIBlizzconEnd.cpp:78   void UIBlizzconEnd::Show(int32)             [0x71003d44f0]
//   X/Client/UserInterface/Interface/UIBlizzconEnd.cpp:133  BOOL UIBlizzconEnd::IsActive()              [0x71003d45b0]
//   X/Client/UserInterface/Interface/UIBlizzconEnd.cpp:149  void UIBlizzconEnd::Update()                [0x71003d4600]
//   X/Client/UserInterface/Interface/UIBlizzconEnd.cpp:179  void UIBlizzconEnd::CheatClosed()           [DWARF DIE 0x0381e922, stripped]
#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"

namespace UIBlizzconEnd {

void InitializeForGame();
void DisposeForGame();
void Show(int32 nScreenToShow);
BOOL IsActive();
void Update();
void CheatClosed();

}  // namespace UIBlizzconEnd
