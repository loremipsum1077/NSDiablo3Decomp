// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UITimerControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIWindowControl.h"

struct UIControlDataExt;
struct Rect2D;

class TimerCreationParams : public WindowCreationParams {  // size 0x738
public:
    TimerCreationParams(const UIType, const UIID &, const UIID &);
    TimerCreationParams(const UIType, LPCSTR, const UIID &);
    ImageTextureFrame tImageTexFlash;  // 0x730
private:
    void sInit();
};
static_assert(sizeof(TimerCreationParams) == 0x738, "TimerCreationParams");

class UITimer : public UIWindow {  // size 0x1e0
public:
    UITimer(const TimerCreationParams &, const Vector2D &, const Vector2D &);
    UITimer(const TimerCreationParams &, const Rect2D *);
    virtual ~UITimer();  // vtable slot 0
    virtual UIID HitTest(const IVector2D &) {  // vtable slot 15; line 51
        return GetUIIDNone();
    }
    virtual UIID HitTestForTooltips(const IVector2D &, BOOL &) {  // vtable slot 16; line 57
        return GetUIIDNone();
    }
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 68
        return eType == UIT_TIMER || UIWindow::IsType(eType);
    }
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
    void SetReverseColor(const BOOL fReverse) { m_fReverseColor = fReverse; }  // line 86
    void SetTimer(const uint32 dwStartTime, const uint32 dwEndTime);
    BOOL IsFinished();
    int32 GetTimeRemaining() const { return m_dwEndTime - m_dwStartTime; }  // line 104; body from the copy inlined into UIHotbarButton::GetDurationRemaining
protected:
    void sInit(const TimerCreationParams & tParams);
    RGBAColor sGetColor(const float) const;
    friend void SpriteSubmitTimerControlSprite(const UIID & id, float flZ);
public:
    uint32 m_dwStartTime;  // 0x1cc
    uint32 m_dwEndTime;  // 0x1d0
    BOOL m_fReverseColor;  // 0x1d4
    UIID m_idBlinker;  // 0x1d8
};
static_assert(sizeof(UITimer) == 0x1e0, "UITimer");
