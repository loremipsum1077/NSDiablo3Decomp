// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/UIUtils.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

enum AlignmentType {
    ALIGN_CENTER_VERTICAL = 0,
    ALIGN_CENTER_HORIZONTAL = 1,
    ALIGN_CENTER_HORIZONTAL_VERTICAL = 2,
};

enum FadeState {
    FADESTATE_SHOWING = 0,
    FADESTATE_SHOWN = 1,
    FADESTATE_HIDING = 2,
    FADESTATE_HIDDEN = 3,
};

class FadeRamp {  // size 0xc
public:
    FadeRamp() = default;  // trivial: a static FadeGlobals needs no dynamic initialiser
    FadeRamp(uint32, uint32, uint32);
    void Set(uint32 dwFadeInTime, uint32 dwFadeOutStartTime, uint32 dwFadeOutEndTime);
    FadeState Evaluate(const uint32 dwElapsedTicks, float & flFrac) const;
private:
    uint32 m_dwFadeInTime;  // 0x00
    uint32 m_dwFadeOutStartTime;  // 0x04
    uint32 m_dwFadeOutEndTime;  // 0x08
};
static_assert(sizeof(FadeRamp) == 0xc, "FadeRamp");

struct Vector2D;

// Defined in X/Client/UserInterface/UIUtils.cpp.
void ConvertPixelsToUIC(const Vector2D & vecPixels, Vector2D & vecUIC);






















































































































































inline FadeRamp::FadeRamp(uint32 dwFadeInTime, uint32 dwFadeOutStartTime,  // line 183
                          uint32 dwFadeOutEndTime)
{
    // The original body is spread over lines 184-189; only the call at line 188 has code.

    Set(dwFadeInTime, dwFadeOutStartTime, dwFadeOutEndTime);
}

struct Rect2D;

void ConvertUICToPixels(const Rect2D & rectUIC, Rect2D & rectPixels);  // UIUtils.cpp:304

// Functions defined in this file (bodies are not in the DWARF):
//   line   183  FadeRamp::FadeRamp(uint32 dwFadeInTime, uint32 dwFadeOutStartTime, uint32 dwFadeOutEndTime)  [inline]
void AlignRect(const Vector2D & vecSourceTL, const Vector2D & vecSourceDim, const AlignmentType eAlignmentType, const float flStart, const Vector2D & vecToAlignDim, Vector2D & vecTL);
