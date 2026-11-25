// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIBlinkerControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UITextControl.h"

struct UIControlDataExt;
struct Rect2D;

extern const uint32 BLINKER_DURATION_INFINITE;  // UIBlinkerControl.cpp:18

class BlinkerCreationParams : public TextCreationParams {  // size 0x7e8
public:
    BlinkerCreationParams(const UIType, const UIID &, const UIID &);
    BlinkerCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    uint32 dwDuration;  // 0x7d8
    float flMinAlpha;  // 0x7dc
    Angle flRotSpeed;  // 0x7e0
    Angle flAlphaSpeed;  // 0x7e4
private:
    void sInit();
};
static_assert(sizeof(BlinkerCreationParams) == 0x7e8, "BlinkerCreationParams");

class UIBlinker : public UIText {  // size 0x400
public:
    UIBlinker(const BlinkerCreationParams &, const Vector2D &, const Vector2D &);
    UIBlinker(const BlinkerCreationParams &, const Rect2D *);
    virtual ~UIBlinker();  // vtable slot 0
    virtual UIID HitTest(const IVector2D &) {  // vtable slot 15, line 57
        return GetUIIDNone();
    }
    virtual UIID HitTestForTooltips(const IVector2D &, BOOL &) {  // vtable slot 16, line 63
        return GetUIIDNone();
    }
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17, line 74
        return eType == UIT_BLINKER || UIText::IsType(eType);
    }
    void SetAlphaSpeed(const float flBlinksPerSecond);
    void SetRotationSpeed(const Angle flRotSpeed);
    void SetDuration(const uint32 dwDuration);
    BOOL IsBlinking();
private:
    void sInit(const BlinkerCreationParams & tParams);
    void sUpdate();
public:
    uint32 m_dwStartTime;  // 0x3e8
    uint32 m_dwDuration;  // 0x3ec
    float m_flMinAlpha;  // 0x3f0
    Angle m_flRotSpeed;  // 0x3f4
    uint32 m_dwRotationFullCycleTimeInTicks;  // 0x3f8
    uint32 m_dwAlphaFullCycleTimeInTicks;  // 0x3fc
};
static_assert(sizeof(UIBlinker) == 0x400, "UIBlinker");
