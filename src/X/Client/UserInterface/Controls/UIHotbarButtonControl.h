// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIHotbarButtonControl.h
// Class layouts and method lines from the DWARF.

#pragma once

#include "UIButtonControl.h"
#include "../../../Common/HotbarButton.h"

struct UIControlDataExt;
struct Rect2D;
struct HotbarButtonData;
struct Player;

class HotbarButtonCreationParams : public ButtonCreationParams {  // size 0x8a0; line 16
public:
    HotbarButtonCreationParams(const UIType, const UIID &, const UIID &);  // line 19
    HotbarButtonCreationParams(const UIType, LPCSTR, const UIID &);  // line 24
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2; line 29
    ImageTextureFrame tTexFrameFlash;  // 0x890
    BOOL bHideStackCount;  // 0x898
private:
    void sInit();  // line 37
};
static_assert(sizeof(HotbarButtonCreationParams) == 0x8a0, "HotbarButtonCreationParams");

class UIHotbarButton : public UIButton {  // size 0x510; line 45
public:
    UIHotbarButton(const HotbarButtonCreationParams &, const Vector2D &, const Vector2D &);  // line 48
    UIHotbarButton(const HotbarButtonCreationParams & tParams, const Rect2D * pRect);  // line 52
    virtual ~UIHotbarButton();  // vtable slot 0; line 56
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 58
        return eType == UIT_HOTBARBUTTON || UIButton::IsType(eType);
    }
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19; line 71
    const HotbarButtonData & GetData() const { return m_tData; }  // line 76
    void DisableTimer();  // line 83
    void DisableText();  // line 86
    void UpdateSkillButtonState(const HotbarButtonData & tButtonData, const BOOL bUnusable, const BOOL bNoMouseOver, const BOOL bRoundIcon, const BOOL bDisplayCharges, const BOOL bDisplayBuffStackCount);  // line 90
    void UpdateTimer(const BOOL fSkillChanged, const BOOL bDisplayCharges);  // line 98
    int32 GetDurationRemaining() const;  // line 102
    BOOL ClearCooldown();  // line 105
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12; line 108
    const Player * GetPlayer() const { return m_ptPlayer; }  // line 113
    void SetPlayer(const Player * ptPlayer);  // line 117
    BOOL IsHidingStackCount() const { return m_bHideStackCount; }  // line 121
    void HideStackCount(const BOOL bHide);  // line 125
    UIID GetTimerID() const { return m_idTimer; }  // line 128
private:
    void sInit(const HotbarButtonCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim);  // line 135
    void DrawItem(const uint8 bLayerAlpha, float & flZSlice);  // line 140
    void sUpdate();  // line 144
public:  // DWARF has these private; UIQuests.cpp and UIConsoleHUD.cpp read m_tData / m_idTimer directly in this tree.
    UIID m_idTimer;  // 0x4c8
    UIID m_idChargeText;  // 0x4d0
    UIID m_idBuffStackCountText;  // 0x4d8
    UIID m_idRechargeTimer;  // 0x4e0
    const Player * m_ptPlayer;  // 0x4e8
    BOOL m_bHideStackCount;  // 0x4f0
    BOOL m_bUnusable;  // 0x4f4
    BOOL m_bOnCooldown;  // 0x4f8
    HotbarButtonData m_tData;  // 0x4fc
};
static_assert(sizeof(UIHotbarButton) == 0x510, "UIHotbarButton");
