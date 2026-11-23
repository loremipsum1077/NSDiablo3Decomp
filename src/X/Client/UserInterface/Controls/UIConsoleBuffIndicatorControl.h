// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIConsoleBuffIndicatorControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIWindowControl.h"

struct UIControlDataExt;
struct Rect2D;

class ConsoleBuffIndicatorCreationParams : public WindowCreationParams {  // size 0x738
public:
    ConsoleBuffIndicatorCreationParams(const UIType, const UIID &, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    ImageTextureFrame tTextureFrame;  // 0x730
private:
    void sInit();
};
static_assert(sizeof(ConsoleBuffIndicatorCreationParams) == 0x738, "ConsoleBuffIndicatorCreationParams");

class UIConsoleBuffIndicator : public UIWindow {  // size 0x1e8
public:
    UIConsoleBuffIndicator(const ConsoleBuffIndicatorCreationParams &, const Vector2D &, const Vector2D &);
    UIConsoleBuffIndicator(const ConsoleBuffIndicatorCreationParams &, const Rect2D *);
    virtual ~UIConsoleBuffIndicator() {}  // vtable slot 0
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17
        return eType == UIT_CONSOLEBUFFINDICATOR || UIWindow::IsType(eType);
    } void SetTimerId(  // line 69
        const UIID & idTimer) { m_idTimer = idTimer; } void SetBuffData(SNO snoPower, int32 nStackCount);  // idTimer line 70
    SNO m_snoPower;  // 0x1cc
    int32 m_nStackCount;  // 0x1d0
    UIID m_idTimer;  // 0x1d8
    UIID m_idText;  // 0x1e0
private:
    void sInit(const ConsoleBuffIndicatorCreationParams & tParams, const Vector2D &, const Vector2D &);
};
static_assert(sizeof(UIConsoleBuffIndicator) == 0x1e8, "UIConsoleBuffIndicator");
