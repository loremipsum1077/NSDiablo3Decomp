// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIRechargeTimerControl.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "UITimerControl.h"

class UIRechargeTimer : public UITimer {  // size 0x1e0
public:
    UIRechargeTimer(const TimerCreationParams &, const Vector2D &, const Vector2D &);
    UIRechargeTimer(const TimerCreationParams &, const Rect2D *);
    virtual ~UIRechargeTimer();  // vtable slot 0
    virtual UIID HitTest(const IVector2D &) {  // vtable slot 15; line 28
        return GetUIIDNone();
    }
    virtual UIID HitTestForTooltips(const IVector2D &, BOOL &) {  // vtable slot 16; line 34
        return GetUIIDNone();
    }
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 41
        return UITimer::IsType(eType);
    }
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
private:
    void Init();
    friend void RechargeTimerSpriteSubmitTimerControlSprite(const UIID & id, float flZ);
};
static_assert(sizeof(UIRechargeTimer) == 0x1e0, "UIRechargeTimer");


// Functions defined in this file (bodies are not in the DWARF):
//   line    28  UIID UIRechargeTimer::HitTest(const IVector2D &)  [0x71003d2c90]
//   line    34  UIID UIRechargeTimer::HitTestForTooltips(const IVector2D &, BOOL &)  [0x71003d2cc0]
//   line    41  BOOL UIRechargeTimer::IsType(const UIType eType) const  [0x71003d2cf0]
