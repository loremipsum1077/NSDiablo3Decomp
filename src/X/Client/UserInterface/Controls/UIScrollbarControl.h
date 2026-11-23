// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIScrollbarControl.h

#pragma once

#include "UIWindowControl.h"
#include "../Interface/UIFunctions.h"

struct UIControlDataExt;
struct Rect2D;

enum ScrollbarVisualStyle {
    SVS_DEFAULT = 0,
    SVS_POINTER_SLIDER = 1,
    SVS_TICK_SLIDER = 2,
    SVS_HUE_SLIDER = 3,
    SVS_COUNT = 4,
};

enum ScrollbarVisibility {
    SBV_HIDE_WHEN_NOT_NEEDED = 0,
    SBV_NEVER_SHOW = 1,
    SBV_ALWAYS_SHOW = 2,
    SBV_COUNT = 3,
};

class ScrollbarCreationParams : public WindowCreationParams {  // size 0x750
public:
    ScrollbarCreationParams(const UIType, const UIID &, const UIID &);
    ScrollbarCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    ScrollbarVisualStyle eScrollbarStyle;  // 0x730
    BOOL fIsVertical;  // 0x734
    ScrollbarVisibility eScrollbarVisibility;  // 0x738
    UIScrollbarChangeFunctionPtr pfnOnChange;  // 0x740
    SNO snoMouseoverSound;  // 0x748
private:
    void sInit();
};
static_assert(sizeof(ScrollbarCreationParams) == 0x750, "ScrollbarCreationParams");

class UIScrollbar : public UIWindow {  // size 0x230
public:
    UIScrollbar(const ScrollbarCreationParams &, const Vector2D &, const Vector2D &, const ScrollbarVisualStyle);
    UIScrollbar(const ScrollbarCreationParams &, const Rect2D *, const ScrollbarVisualStyle);
    virtual ~UIScrollbar();  // vtable slot 0
    const UIID & GetOwner() const { return m_idOwner; }  // line 83
    void SetOwner(const UIID & idOwner) { m_idOwner = idOwner; }  // line 90
    int32 GetPosition() const { return m_nPosition; }  // line 97
    BOOL AtScrollTop();
    BOOL AtScrollEnd();
    void SetPosition(int32 nPosition, const BOOL bSendUpdate);
    void SetExtentsAndPosition(const int32 nMin, const int32 nMax, const int32 nPos, const BOOL bSendUpdate);
    void SetExtents(const int32 nMin, const int32 nMax, const BOOL bSendUpdate) {  // line 144
        SetExtentsAndPosition(nMin, nMax, m_nPosition, bSendUpdate);
    }
    void SetIncrements(const int32 nButtonIncrement, const int32 nPageIncrement);
    void SetVisualStyle(ScrollbarVisualStyle eStyle);
    BOOL IsEnabled() const;
    virtual void SetEnabled(BOOL fEnabled);  // vtable slot 34
    ScrollbarVisualStyle GetVisualStyle() const;
    void SetOnChangeFunction(UIScrollbarChangeFunctionPtr pfnOnChange) { m_pfnOnChange = pfnOnChange; }  // line 180
    void SetHideWhenNotNeeded(const ScrollbarVisibility);
    void OnDrag();
    void HandleChildClick(const UIID & idChild);
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 200
        return eType == UIT_SCROLLBAR || UIWindow::IsType(eType);
    }
    virtual BOOL CanAcceptKeyboardFocus() const { return FALSE; }  // vtable slot 2; line 213
    static void GetScrollbarRect(Vector2D & vecScrollbarTL, Vector2D & vecScrollbarDim, const ScrollbarVisualStyle, const Vector2D & vecParentDim);
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    static void OnScrollChangeDefault(const UIID & id, const int32 nOldPos, const int32 nNewPos);
    void SetIsVertical(BOOL);
    virtual void SetRelativePosition(const Vector2D & vecTL, const Vector2D & vecDim);  // vtable slot 30
    void SetMouseWheelScaleFactor(float);
private:
    void sInit(const ScrollbarCreationParams & tParams, const Vector2D &, const Vector2D & vecDim, const ScrollbarVisualStyle eStyleOverride);
    BOOL sIsSlider() const;
    void sClickUp();
    void sClickDown();
    void sPageUp();
    void sPageDown();
    void sReposition();
    float sGetThumbDeviation();
    void sRepositionThumb(float flDeviation);
    void sRepositionButtons();
    void sRepositionTrack();
    void sClipTrack(float flDeviation);
    void sMove(int32 nDistance);
    void sHandleDrag(BOOL);
    void sDragExtents(Rect2D *);
    BOOL sIsVertical() { return m_fIsVertical; }  // line 299
    int32 m_nMin;  // 0x1cc
    int32 m_nMax;  // 0x1d0
    int32 m_nPosition;  // 0x1d4
    int32 m_nThumbDistance;  // 0x1d8
    int32 m_nPageDistance;  // 0x1dc
    float m_nMouseWheelScaleFactor;  // 0x1e0
    ScrollbarVisualStyle m_eVisualStyle;  // 0x1e4
    UIID m_idOwner;  // 0x1e8
    UIID m_idThumb;  // 0x1f0
    UIID m_idTrackUp;  // 0x1f8
    UIID m_idTrackDown;  // 0x200
    UIID m_idButtonUp;  // 0x208
    UIID m_idButtonDown;  // 0x210
    ScrollbarVisibility m_eScrollbarVisibility;  // 0x218
    BOOL m_fIsVertical;  // 0x21c
    UIScrollbarChangeFunctionPtr m_pfnOnChange;  // 0x220
    SNO m_snoMouseoverSound;  // 0x228
};
static_assert(sizeof(UIScrollbar) == 0x230, "UIScrollbar");
