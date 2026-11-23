#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIFade.h"
#include "../UIID.h"
#include "../UIUtils.h"
#include "../Controls/UIWindowControl.h"

#include <float.h>

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_FADE;
// X/Common/Game.cpp
uint32 AppGetTicks();
// X/Client/Gfx/GfxCommon.cpp
BOOL ImageCollectionGetImageInfo(LPCSTR szDecoratedLabel, ImageTextureFrame & tImageTexFrame);

namespace UIFade {

struct FadeGlobals {  // size 0x10
    FadeRamp tFadeRamp;  // 0x00
    uint32 dwStartTick;  // 0x0c
};
static_assert(sizeof(FadeGlobals) == 0x10, "FadeGlobals");

FadeGlobals sgtFadeGlobals;  // line 28


void OnUpdate(const UIID &) {  // line 31
    UIWindow & tFade = UI_GetWindowFromID(UIDLG_FADE);
    uint32 dwElapsedTicks = AppGetTicks() - sgtFadeGlobals.dwStartTick;
    float flFadeLevel;
    sgtFadeGlobals.tFadeRamp.Evaluate(dwElapsedTicks, flFadeLevel);
    if (flFadeLevel < FLT_EPSILON) {
        tFade.SetActive(FALSE);
        return;
    }
    uint8 nFade = (uint8)(flFadeLevel * 255.0f);
    tFade.SetTransparency(nFade);
}

void Open(float flDuration, FadeColor eFadeColor) {  // line 54
    UIWindow & tFade = UI_GetWindowFromID(UIDLG_FADE);
    tFade.SetTransparency(0xff);
    tFade.SetActive(TRUE);

    ImageTextureFrame tBackground;  // line 62
    if (eFadeColor == FADE_COLOR_BLACK) {
        ImageCollectionGetImageInfo("UI:BlackCube", tBackground);
    } else if (eFadeColor == FADE_COLOR_WHITE) {
        ImageCollectionGetImageInfo("UI:WhiteCube", tBackground);
    } else {
        tBackground = cgtImageTextureFrameNull;
    }
    tFade.SetTextureFrame(tBackground);
    uint32 dwFadeTicks = (uint32)(flDuration * 60.0f);
    sgtFadeGlobals.dwStartTick = AppGetTicks();
    sgtFadeGlobals.tFadeRamp.Set(0, 0, dwFadeTicks);
}

}  // namespace UIFade
