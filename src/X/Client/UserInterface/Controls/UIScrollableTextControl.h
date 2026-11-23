// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIScrollableTextControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UITextControl.h"
#include "UIScrollbarControl.h"
#include "UIWindowControl.h"

struct UIControlDataExt;
struct Rect2D;

class ScrollableTextCreationParams : public TextCreationParams {  // size 0x7e0; line 19
public:
    ScrollableTextCreationParams(const UIType, const UIID &, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    ScrollbarVisualStyle eScrollbarStyle;  // 0x7d8
    ScrollbarVisibility eScrollbarVisibility;  // 0x7dc
};
static_assert(sizeof(ScrollableTextCreationParams) == 0x7e0, "ScrollableTextCreationParams");

class UIScrollableText : public UIWindow {  // size 0x1e0
public:
    UIScrollableText(const ScrollableTextCreationParams &, const Vector2D &, const Vector2D &);
    UIScrollableText(const ScrollableTextCreationParams &, const Rect2D *);
    virtual ~UIScrollableText();  // vtable slot 0
    virtual void PostCreate();  // vtable slot 18
    virtual void SetActive(const BOOL fActive);  // vtable slot 10
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual void OnScrollbarPositionChanged(const UIID & idScrollbar);  // vtable slot 32
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 67
        return eType == UIT_SCROLLABLETEXT || UIWindow::IsType(eType);
    }
    void ScrollToTop() const;
    void ScrollToEnd() const;
    void SetText(LPCSTR uszText);
    void AppendText(LPCSTR);
    const UIID & GetScrollaBarId() const;
private:
    void sInit(const ScrollableTextCreationParams & tParams, const Vector2D &, const Vector2D & vecDim);
    void UpdateScrollbarExtents();
    UIID m_idTextChild;  // 0x1d0
    UIID m_idScrollbarChild;  // 0x1d8
};
static_assert(sizeof(UIScrollableText) == 0x1e0, "UIScrollableText");
