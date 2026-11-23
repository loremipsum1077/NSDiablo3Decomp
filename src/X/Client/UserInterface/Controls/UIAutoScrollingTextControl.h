// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIAutoScrollingTextControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UITextControl.h"
#include "UIWindowControl.h"

struct UIControlDataExt;
struct Rect2D;

class AutoScrollingTextCreationParams : public TextCreationParams {  // size 0x7f0; line 17
public:
    AutoScrollingTextCreationParams(const UIType, const UIID &, const UIID &);
    AutoScrollingTextCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    uint32 dwInitialDelay;  // 0x7d8
    uint32 dwDelayAtEnd;  // 0x7dc
    float flScrollRate;  // 0x7e0
    BOOL bScrollHorizontal;  // 0x7e4
    BOOL bRepeat;  // 0x7e8
private:
    void sInit();
};
static_assert(sizeof(AutoScrollingTextCreationParams) == 0x7f0, "AutoScrollingTextCreationParams");

class UIAutoScrollingText : public UIWindow {  // size 0x1f8
public:
    UIAutoScrollingText(const AutoScrollingTextCreationParams &, const Vector2D &, const Vector2D &);
    UIAutoScrollingText(const AutoScrollingTextCreationParams &, const Rect2D *);
    virtual ~UIAutoScrollingText();  // vtable slot 0
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 68
        return eType == UIT_AUTOSCROLLINGTEXT || UIWindow::IsType(eType);
    }
    void SetScrollRateFromDuration(uint32 dwDuration);
    void SetHorizontalScrolling(const BOOL fVal);
    void SetScrollRate(const float flRate) { m_flScrollRate = flRate; }  // line 86
    void StartTimers();
    BOOL IsFinishedScrolling();
    void SetText(LPCSTR uszText);
private:
    float sGetScrollDistance();
    void sInit(const AutoScrollingTextCreationParams & tParams, const Vector2D &, const Vector2D & vecDim);
    float sSetText(LPCSTR, BOOL, BOOL);
    uint32 m_dwScrollStartTime;  // 0x1cc
    uint32 m_dwScrollStopTime;  // 0x1d0
    uint32 m_dwEndTime;  // 0x1d4
    uint32 m_dwInitialDelay;  // 0x1d8
    uint32 m_dwDelayAtEnd;  // 0x1dc
    float m_flScrollRate;  // 0x1e0
    BOOL m_bScrollHorizontal;  // 0x1e4
    BOOL m_bRepeat;  // 0x1e8
    UIID m_idTextChild;  // 0x1f0
};
static_assert(sizeof(UIAutoScrollingText) == 0x1f8, "UIAutoScrollingText");
