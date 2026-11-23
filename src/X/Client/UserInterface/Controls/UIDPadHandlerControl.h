// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIDPadHandlerControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIWindowControl.h"

struct UIControlDataExt;
struct Rect2D;

class UIDPadHandlerControl : public UIWindow {  // size 0x1d0
public:
    UIDPadHandlerControl(const WindowCreationParams &, const Vector2D &, const Vector2D &);
    UIDPadHandlerControl(const WindowCreationParams & tParams, const Rect2D * pRect) : UIWindow(tParams, pRect) {}  // line 27
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 35
        return eType == UIT_DPADHANDLER || UIWindow::IsType(eType);
    }
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL HandlesInput() { return TRUE; }  // vtable slot 13; line 53
};
static_assert(sizeof(UIDPadHandlerControl) == 0x1d0, "UIDPadHandlerControl");
