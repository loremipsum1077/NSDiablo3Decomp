// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIFloatBubbleControl.h

#pragma once

#include "UITextControl.h"

struct UIControlDataExt;
struct IVector2D;
struct Rect2D;
struct UIID;
struct Vector2D;

class UIFloatBubble : public UIText {  // size 0x3e8
public:
    UIFloatBubble(const TextCreationParams &, const Vector2D &, const Vector2D &);
    UIFloatBubble(const TextCreationParams &, const Rect2D *);
    virtual ~UIFloatBubble() {}  // vtable slot 0
    virtual UIID HitTest(const IVector2D &) {  // vtable slot 15; line 29
        return GetUIIDNone();
    }
    virtual UIID HitTestForTooltips(const IVector2D &, BOOL &) {  // vtable slot 16; line 35
        return GetUIIDNone();
    }
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 42
        return eType == UIT_FLOATBUBBLE || UIText::IsType(eType);
    }
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
private:
    void sInit();
};
static_assert(sizeof(UIFloatBubble) == 0x3e8, "UIFloatBubble");
