// Original file: X/Client/UserInterface/Console/UIConsolePartyDisplay.h.
// Public party-display APIs; signatures follow the original source DWARF.
#pragma once
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
struct UIID;
namespace UIPartyDisplay { namespace Console {
void InitializeForGame();
void DisposeForGame();
void SwitchFocusedList(BOOL bPressedUp);
BOOL SelectNextNameplate();
BOOL SelectPreviousNameplate();
void ShowPartyDisplay();
void HidePartyDisplay();
void RefreshPartyDisplay();
void OnScrolledOffListTop(const UIID & id);
void OnScrolledOffListBottom(const UIID & id);
void OnEventPartyMemberRemoved(int32 nPartyIndex);
BOOL IsNamePlateSelected();  // UIConsolePartyDisplay.cpp:1526
} }
