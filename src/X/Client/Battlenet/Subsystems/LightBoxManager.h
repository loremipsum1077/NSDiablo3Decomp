// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Battlenet/Subsystems/LightBoxManager.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../UserInterface/Controls/UIControlControl.h"
#include "../../UserInterface/UIID.h"
#include "../../../Sigma/DataStructures/XGrowableList.h"

typedef uint32 LightBoxId;

class LightBoxManager {  // size 0x70
public:
    LightBoxManager();
    ~LightBoxManager();
    UIID CreateLightBoxForWindow(const UIID & idWindow, BOOL bBlockInput);
    LightBoxId PushLightBox(const UIID & idLightBoxWindow);
    UIID PopLightBox();
    void RemoveLightBox(LightBoxId idLightBox);
    void ClearAllLightBoxes();
    void InitializeForGame();
    void DisposeForGame();
    BOOL IsLightBoxVisible() { return !m_lightBoxStack.IsEmpty(); }  // line 55
private:
    LightBoxManager(const LightBoxManager &);
    LightBoxManager & operator=(const LightBoxManager &);
    void ShowTopLightBox(BOOL fShow);
    // Line 67. The body is not in the DWARF; written from the copy inlined into PushLightBox.
    LightBoxId GetNextLightBoxId() { return m_uNextId++; }
    struct LightBoxStackEntry {  // size 0x10
        UIID idLightBox;  // 0x00
        LightBoxId uId;  // 0x08
    };
    typedef XGrowableList<LightBoxManager::LightBoxStackEntry, const LightBoxManager::LightBoxStackEntry &> LightBoxStack;
    LightBoxManager::LightBoxStack m_lightBoxStack;  // 0x00
    UIControlSourceKey m_keyLightBox;  // 0x58
    LightBoxId m_uNextId;  // 0x68
};
static_assert(sizeof(LightBoxManager) == 0x70, "LightBoxManager");


// Functions defined in this file (bodies are not in the DWARF):
//   line    55  BOOL LightBoxManager::IsLightBoxVisible()  [inlined into 1 functions]
//   line    67  LightBoxId LightBoxManager::GetNextLightBoxId()  [inlined into 1 functions]
//   line    73  LightBoxManager::LightBoxStackEntry & LightBoxManager::LightBoxStackEntry::operator=(const LightBoxManager::LightBoxStackEntry &)  [inlined into 1 functions]
