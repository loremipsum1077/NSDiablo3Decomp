// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIStackPanelControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIWindowControl.h"

struct UIControlDataExt;
struct Rect2D;

class StackPanelCreationParams : public WindowCreationParams {  // size 0x750
public:
    StackPanelCreationParams(const UIType, const UIID &, const UIID &);
    StackPanelCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    BOOL fStackHorizontally;  // 0x730
    BOOL fReverseOrder;  // 0x734
    float flSpacing;  // 0x738
    float flMarginTop;  // 0x73c
    float flMarginRight;  // 0x740
    float flMarginBottom;  // 0x744
    float flMarginLeft;  // 0x748
private:
    void sInit();
};
static_assert(sizeof(StackPanelCreationParams) == 0x750, "StackPanelCreationParams");

class UIStackPanel : public UIWindow {  // size 0x1f8
public:
    UIStackPanel(const StackPanelCreationParams &, const Vector2D &, const Vector2D &);
    UIStackPanel(const StackPanelCreationParams &, const Rect2D *);
    virtual ~UIStackPanel();  // vtable slot 0
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 65
        return eType == UIT_STACKPANEL || UIWindow::IsType(eType);
    }
    virtual void RecalculateLayout(int32 nStartingChildIndex);  // vtable slot 24
    float GetSpacing() const;
    void DeleteAllChildren();
    BOOL m_fStackHorizontally;  // 0x1cc
    BOOL m_fReverseOrder;  // 0x1d0
    float m_flSpacing;  // 0x1d4
    float m_flMarginTop;  // 0x1d8
    float m_flMarginRight;  // 0x1dc
    float m_flMarginBottom;  // 0x1e0
    float m_flMarginLeft;  // 0x1e4
    Vector2D m_vecInitialSize;  // 0x1e8
    float m_flLargestWidth;  // 0x1f0
private:
    void sInit(const StackPanelCreationParams & tParams);
    void sRelayoutChild(UIWindow * ptWindow, Vector2D & vecOffset);
    int32 FindPrevWindowChild(int32 nStartingIndex);
    void GetStartingLayout(int32 nStartingChildIndex, Vector2D & vecOffset);
    float GetLargestWidth(BOOL & hasCenteredChild) const;
    void RecalculateForwardLayout(int32 nStartingChildIndex);
    void RecalculateReverseLayout(int32 nStartingChildIndex);
};
static_assert(sizeof(UIStackPanel) == 0x1f8, "UIStackPanel");
