// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIThumbButtonControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIButtonControl.h"

struct BufferedInput;
struct UIControlDataExt;
struct Rect2D;

class UIThumbButton : public UIButton {  // size 0x4c8
public:
    UIThumbButton(const ButtonCreationParams &, const Vector2D &, const Vector2D &);
    UIThumbButton(const ButtonCreationParams &, const Rect2D *);
    virtual ~UIThumbButton() {}  // vtable slot 0; line 33
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 35
        return eType == UIT_THUMBBUTTON || UIButton::IsType(eType);
    }
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    static UIID Create(ButtonCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim, const BOOL fForVerticalScrollbar);
    static UIID Create(ButtonCreationParams &, const Rect2D &, const BOOL);
private:
    BOOL sInput(const BufferedInput &, BOOL &);
    static UIResult sOnDrag(const UIID & id);
};
static_assert(sizeof(UIThumbButton) == 0x4c8, "UIThumbButton");
