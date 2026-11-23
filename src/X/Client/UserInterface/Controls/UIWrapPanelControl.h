// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIWrapPanelControl.h

#pragma once

#include "UIWindowControl.h"

struct UIControlDataExt;
struct Rect2D;

class WrapPanelCreationParams : public WindowCreationParams {  // size 0x750
public:
    WrapPanelCreationParams(const UIType, const UIID &, const UIID &);
    WrapPanelCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    BOOL fStackVertically;  // 0x730
    BOOL fReverseOrder;  // 0x734
    float flHSpacing;  // 0x738
    float flVSpacing;  // 0x73c
    float flMarginTop;  // 0x740
    float flMarginRight;  // 0x744
    float flMarginBottom;  // 0x748
    float flMarginLeft;  // 0x74c
private:
    void sInit();
};
static_assert(sizeof(WrapPanelCreationParams) == 0x750, "WrapPanelCreationParams");

class UIWrapPanel : public UIWindow {  // size 0x1f0
    // sForEachChild (UIWrapPanelControl.cpp:25) reads the protected members directly.
    template <typename T>
    friend void sForEachChild(const UIWrapPanel & tWrapPanel, const int32 nStartIndex, const int32 nEndIndex, T operation);
public:
    UIWrapPanel(const WrapPanelCreationParams &, const Vector2D &, const Vector2D &);
    UIWrapPanel(const WrapPanelCreationParams &, const Rect2D *);
    virtual ~UIWrapPanel();  // vtable slot 0
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 68
        return eType == UIT_WRAPPANEL || UIWindow::IsType(eType);
    }
    virtual void RecalculateLayout(int32 nStartingChildIndex);  // vtable slot 24
    void DeleteAllChildren();
    BOOL IsVertical() const;
    float GetHorizontalSpacing() const;
    float GetVerticalSpacing() const;
    float GetFrontMargin() const;
    float GetRearMargin() const;
    int32 GetNumStrips() const;
    int32 GetStripContainingChild(const int32 nChildIndex) const;
    int32 GetNumChildrenInStrip(const int32 nStrip) const;
    BOOL GetGridPosOfChild(const int32 nChildIndex, IVector2D & vecGridPos) const;
    int32 GetChildIndexAtGridPos(const IVector2D & vecGridPos) const;
protected:
    BOOL m_fStackVertically;  // 0x1cc
    BOOL m_fReverseOrder;  // 0x1d0
    float m_flHSpacing;  // 0x1d4
    float m_flVSpacing;  // 0x1d8
    Vector2D m_vecMarginTopLeft;  // 0x1dc
    Vector2D m_vecMarginBottomRight;  // 0x1e4
    float m_flContentStripSize;  // 0x1ec
private:
    void sInit(const WrapPanelCreationParams & tParams);
    void RelayoutChild(UIWindow * ptWindow, Vector2D & vecOffset);
    void FixupContentStripSize(UIWindow * ptWindow);
    int32 FindPrevWindowChild(int32 nStartingIndex);
    void GetStartingLayout(int32 nStartingChildIndex, Vector2D & vecOffset);
    void RecalculateForwardLayout(int32 nStartingChildIndex);
    void RecalculateReverseLayout(int32 nStartingChildIndex);
};
static_assert(sizeof(UIWrapPanel) == 0x1f0, "UIWrapPanel");
