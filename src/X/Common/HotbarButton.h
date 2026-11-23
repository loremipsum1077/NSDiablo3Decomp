// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/HotbarButton.h

#pragma once

#include "CommonTypes.h"
#include "Definitions/AttachmentDefinition.h"
#include "GameBalance.h"
#include "SNO/SNOFiles.h"
#include "../Sigma/SigmaTypes.h"

struct HotbarButtonData {  // size 0x10
    HotbarButtonData() { Empty(); }  // line 17
    BOOL operator==(const HotbarButtonData & rhs) const;
    void Empty() {  // line 31
        m_snoPower = -1;
        m_eRuneType = -1;
        m_gbidItem = -1;
        m_annItem = ANN_INVALID;
    }
    BOOL IsEmpty() const;
    // line 46. Body written from the copy inlined into UISkills::Console::PlayerSkillsGlobals::UpdateSkillItemUI.
    void SetSkill(const SNO snoPower, const RuneType eRuneType) {
        m_snoPower = snoPower;
        m_eRuneType = eRuneType;
        m_gbidItem = -1;
        m_annItem = ANN_INVALID;
    }
    void SetItemGBID(const GBID gbidItem);
    // line 65. Body written from the copy inlined into PlayerAssignBaseHealthPotion (X/Common/Player.cpp).
    void SetItemANN(const ACDNetworkName annItem) {
        m_snoPower = -1;
        m_eRuneType = -1;
        m_gbidItem = -1;
        m_annItem = annItem;
    }
    SNO m_snoPower;  // 0x00
    RuneType m_eRuneType;  // 0x04
    GBID m_gbidItem;  // 0x08
    ACDNetworkName m_annItem;  // 0x0c
};
static_assert(sizeof(HotbarButtonData) == 0x10, "HotbarButtonData");

// line 22. Body written from the copy inlined into PlayerSendSkillChangeToServer (X/Client/CPlayerInput.cpp).
inline BOOL HotbarButtonData::operator==(const HotbarButtonData & rhs) const {
    return m_snoPower == rhs.m_snoPower &&
           m_eRuneType == rhs.m_eRuneType &&
           m_gbidItem == rhs.m_gbidItem &&
           m_annItem == rhs.m_annItem;
}

// line 56. Body written from the copy inlined into PlayerAssignItemToPotionSlot (X/Server/LuaScript/ScriptUtil.cpp).
inline void HotbarButtonData::SetItemGBID(const GBID gbidItem) {
    m_snoPower = -1;
    m_eRuneType = -1;
    m_gbidItem = gbidItem;
    m_annItem = ANN_INVALID;
}


// Functions defined in this file (bodies are not in the DWARF):
//   line    17  HotbarButtonData::HotbarButtonData()  [inlined into 8 functions]
//   line    22  BOOL HotbarButtonData::operator==(const HotbarButtonData & rhs) const  [inlined into 2 functions]
//   line    31  void HotbarButtonData::Empty()  [inlined into 7 functions]
//   line    46  void HotbarButtonData::SetSkill(const SNO snoPower, const RuneType eRuneType)  [inlined into 19 functions]
//   line    56  void HotbarButtonData::SetItemGBID(const GBID gbidItem)  [inlined into 3 functions]
//   line    65  void HotbarButtonData::SetItemANN(const ACDNetworkName annItem)  [inlined into 6 functions]
