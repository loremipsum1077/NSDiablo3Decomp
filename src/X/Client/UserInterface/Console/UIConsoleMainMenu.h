// Original file: X/Client/UserInterface/Console/UIConsoleMainMenu.h
// Subset: the entry point implemented at UIConsoleMainMenu.cpp:1215.
#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/SigmaTypes.h"
struct UIID;
namespace UIMainMenu { namespace Console {
void RefreshMainMenu(BOOL bResetToTop);
void OnLocalLeaveButtonPressed(const UIID &, const int32 nControllerIndex);  // UIConsoleMainMenu.cpp:2645
void OnCancel(const UIID &, const int32 nControllerIndex);  // UIConsoleMainMenu.cpp:2994
} }
