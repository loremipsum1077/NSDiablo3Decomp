// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UIEquipLegendaryPowersCommon.h
// The function declarations are not in the DWARF; they are the definitions in
// UIEquipLegendaryPowersCommon.cpp and Console/UIConsoleEquipLegendaryPowers.cpp.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/GameBalance.h"

struct UIID;
class CRefString;

typedef int32 LegendaryPowerSlotType;
typedef int32 PlayerClass;  // X/Common/GameSwig.h

namespace UIEquipLegendaryPowers {
// UIEquipLegendaryPowersCommon.cpp
void GetAllPowers(GBIDArray & arrPowers);
void ShowPowerIcon(const UIID & idItemUI, GBID gbidPower, BOOL bUnlocked);
CRefString GetPowerTooltipText(GBID gbidPower, bool bUseBullets);
void ShowPowerIcon(const UIID & idItemUI, GBID gbidPower, PlayerClass ePlayerClass, BOOL bIsMale);
// Console/UIConsoleEquipLegendaryPowers.cpp
void InitializeForGame();
void DisposeForGame();
void OpenForController(int32 nControllerIndex);
void Close();
void MarkPowerAsNew(GBID gbidPower);
void OnUpdate(const UIID & id);
void OnButton1(const UIID & id, const int32 nControllerIndex);
void OnButton2(const UIID & id, const int32 nControllerIndex);
void OnButtonL2(const UIID & id, const int32 nControllerIndex);
void OnButtonR2(const UIID & id, const int32 nControllerIndex);
}  // namespace UIEquipLegendaryPowers
