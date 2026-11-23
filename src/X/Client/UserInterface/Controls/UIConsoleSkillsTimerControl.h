// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIConsoleSKillsTimerControl.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "UITimerControl.h"
#include "../../../Sigma/Math/Vector.h"

class UIConsoleSkillsTimer : public UITimer {  // size 0x1f8
public:
    UIConsoleSkillsTimer(const TimerCreationParams &, const Vector2D &, const Vector2D &);
    UIConsoleSkillsTimer(const TimerCreationParams &, const Rect2D *);
    virtual ~UIConsoleSkillsTimer();  // vtable slot 0
    virtual UIID HitTest(const IVector2D &) {  // vtable slot 15; line 29
        return GetUIIDNone();
    }
    virtual UIID HitTestForTooltips(const IVector2D &, BOOL &) {  // vtable slot 16; line 35
        return GetUIIDNone();
    }
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 42
        return UITimer::IsType(eType);
    }
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
    BOOL IsFinished();
    void Flash();
    const Vector2D & GetLeadingEdgePos() const { return m_LeadingEdgePos; }  // line 65
    enum FlashState {
        FLASH_STATE_OFF = 0,
        FLASH_STATE_RAMP_UP = 1,
        FLASH_STATE_RAMP_MAX = 2,
        FLASH_STATE_RAMP_DOWN = 3,
    };
private:
    UIConsoleSkillsTimer::FlashState m_eFlashState;  // 0x1e0
    float m_flFlashScale;  // 0x1e4
    uint32 m_dwFlashStartTime;  // 0x1e8
    Vector2D m_LeadingEdgePos;  // 0x1ec
    void Init();
    friend void ConsoleTimerSpriteSubmitTimerControlSprite(const UIID & id, float flZ, float flBrighten, Vector2D * pLeadingEdgePos);
};
static_assert(sizeof(UIConsoleSkillsTimer) == 0x1f8, "UIConsoleSkillsTimer");


// Functions defined in this file (bodies are not in the DWARF):
//   line    29  UIID UIConsoleSkillsTimer::HitTest(const IVector2D &)  [0x71003d2080]
//   line    35  UIID UIConsoleSkillsTimer::HitTestForTooltips(const IVector2D &, BOOL &)  [0x71003d20b0]
//   line    42  BOOL UIConsoleSkillsTimer::IsType(const UIType eType) const  [0x71003d20e0]
//   line    65  const Vector2D & UIConsoleSkillsTimer::GetLeadingEdgePos() const  [inlined into 2 functions]
