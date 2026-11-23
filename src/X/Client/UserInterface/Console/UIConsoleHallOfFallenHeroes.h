// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleHallOfFallenHeroes.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../../Common/OnlineService/OnlineService.h"

class UIID;

namespace UIHallOfFallenHeroes {
namespace Console {
enum {
    MAX_FALLEN_HEROES = 5,
};

typedef OnlineService::Callback<void ()> HeroArchivingComplete;

// The DWARF records these functions only in UIConsoleHallOfFallenHeroes.cpp; declaring the
// externally called ones here is an assumption.
void OpenForController(int32 nControllerIndex);  // UIConsoleHallOfFallenHeroes.cpp:120
void OpenForController(int32 nControllerIndex, OnlineService::HeroId idHeroToArchive, HeroArchivingComplete cb);  // :130
void Close();  // :146
BOOL IsVisible();  // :161
void InitializeForWorld();  // :169
void DisposeForWorld();  // :180
void OnLeftStick(const UIID &, const int32 nControllerIndex);  // :193
void OnDpadLeft(const UIID &, const int32);  // :210
void OnDpadRight(const UIID &, const int32);  // :227
void OnAccept(const UIID &, const int32 nControllerIndex);  // :301
void OnCancel(const UIID &, const int32 nControllerIndex);  // :320
void OnLeftBumper(const UIID &, int32);  // :336
void OnRightBumper(const UIID &, int32);  // :350

}  // namespace Console
}  // namespace UIHallOfFallenHeroes
