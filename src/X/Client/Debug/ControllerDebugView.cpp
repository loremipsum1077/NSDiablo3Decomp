// Original file: X/Client/Debug/ControllerDebugView.cpp
#include "../Sprite.h"
#include "../ImageCollection.h"
#include "../Input/Input.h"
#include "../Console/ControllerUtils.h"
#include "../UserInterface/UI.h"
#include "../UserInterface/Controls/UITextControl.h"
#include "../../Common/Definitions/GlobalDefinition.h"
// Original initializer includes these header-owned constants.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
// Declarations from UIGlobals.cpp, UIUtils.cpp:292, InputNX64.cpp:1098 and GfxPrimitives.cpp:154.
extern UIID UIWND_CONTROLLER_DEBUG;
extern UIID UIWND_LSTICK_CONTAINER;
extern UIID UIWND_LSTICK_POS;
extern UIID UIWND_LSTICK_DEADZONE;
extern UIID UITXT_INPUT_DEBUG;
void ConvertUICToPixels(const Vector2D & vecUIC, Vector2D & vecPixels);
IVector2D InputPlatformGetTouchPadResolution(int32 nControllerIndex);
void GfxLine2DDrawDeferred(const float flX0, const float flY0, const float flX1, const float flY1, const RGBAColor & rgbaColor, const float flZ, const RenderContext eContext);
static void sUpdateLeftAnalogStick(const Player * ptPlayer);
namespace {
#line 29
struct ControllerDebugGlobals
{
    BOOL bDebugView;
    int32 nControllerIndex;

    IVector2D vecTouchRefPoint;
    BOOL bShowRefPoint;

    // Inference: original initializer stores 8 zero bytes across bDebugView and nControllerIndex.
#line 37
    ControllerDebugGlobals() : bDebugView(FALSE), nControllerIndex(0), bShowRefPoint(FALSE)
    {
    }
};
#line 44
static ControllerDebugGlobals tControllerGlobals;
}
#line 50
static ImageTextureFrame sgtTouchRefPointTexture;
static ImageTextureFrame sgtTouchTextures[2];
static LPCSTR sgtTouchTextureNames[2] = { "UI:Console_DebugControllerPosition", "UI:Console_DebugControllerPosition2" };
// Original externally linked helper copies also have DWARF low_pc=0 (DIEs 0x581ccb, 0x581dcd, 0x5826d6).
// LINKER-REMOVED: four setters were removed by the linker (DWARF low_pc 0; lines 58, 64, 223, 231; 28, 16, 28
// and 16 bytes). Their stores keep tControllerGlobals' fields alive through the late GlobalOpt
// (docs/late_globalopt.md). The bodies are reconstructed from the names and fields; each
// compiles to exactly its DWARF size.
namespace Console {
#line 58
void ToggleControllerDebugView()
{
    tControllerGlobals.bDebugView = !tControllerGlobals.bDebugView;
}
#line 64
void SetControllerDebugView(int32 nControllerIndex)
{
    tControllerGlobals.nControllerIndex = nControllerIndex;
}
void TouchPadDebugSetReferencePoint(const IVector2D & vecPoint);
void TouchPadDebugRemoveReferencePoint();
// NON_MATCHING: 592 vs 592 bytes; 6 instructions differ: scheduling of the 280-282 dead-zone vector math (the original multiplies vLStickIndicatorDim by 0.5 before vDim).
#line 71
void UpdateControllerDebugView()
{
    if (!UI_TestID(UIWND_CONTROLLER_DEBUG))
        return;
    UI_GetControlFromID(UIWND_CONTROLLER_DEBUG).SetActive(tControllerGlobals.bDebugView);
    if (!tControllerGlobals.bDebugView)
        return;
#line 84
    const Player * ptPlayer = PlayerGetByControllerIndex(tControllerGlobals.nControllerIndex);
    if (!ptPlayer)
    {
        tControllerGlobals.bDebugView = FALSE;
        return;
    }
#line 91
    sUpdateLeftAnalogStick(ptPlayer);
}
}
#line 95
void sGetTouchPadDebugViewLocation(Vector2D & vecTL, Vector2D & vecDim)
{
#line 99
    IVector2D vecTouchPadRes = InputPlatformGetTouchPadResolution(tControllerGlobals.nControllerIndex);
// One declaration line (DWARF: all four on 101); declarator order from the original's stack slots (the DWARF lists
// TLPixels, TLUIC, DimPixels, DimUIC, but that order puts the four slots where the original does not).
#line 101
    Vector2D vecTLWindowUIC, vecDimWindowUIC, vecTLWindowPixels, vecDimWindowPixels;
    UI_GetWindowFromID(UIWND_LSTICK_CONTAINER).GetRelativePosition(vecTLWindowUIC, vecDimWindowUIC);
    ConvertUICToPixels(vecTLWindowUIC, vecTLWindowPixels);
    ConvertUICToPixels(vecDimWindowUIC, vecDimWindowPixels);

    vecTL.Set(vecTLWindowPixels.x + vecDimWindowPixels.x + 10.0f, vecTLWindowPixels.y);
    vecDim.Set(float(vecTouchPadRes.x >> 2), float(vecTouchPadRes.y >> 2));
}
#line 111
void sDrawTouchPadBorder(const Vector2D & vecTL, const Vector2D & vecDim, float flZ)
{
#line 116
    GfxLine2DDrawDeferred(vecTL.x, vecTL.y, vecTL.x + vecDim.x, vecTL.y, crgbaWhite, flZ, RENDERCONTEXT_DEFAULT);
    GfxLine2DDrawDeferred(vecTL.x, vecTL.y, vecTL.x, vecTL.y + vecDim.y, crgbaWhite, flZ, RENDERCONTEXT_DEFAULT);
    GfxLine2DDrawDeferred(vecTL.x + vecDim.x, vecTL.y, vecTL.x + vecDim.x, vecTL.y + vecDim.y, crgbaWhite, flZ, RENDERCONTEXT_DEFAULT);
    GfxLine2DDrawDeferred(vecTL.x, vecTL.y + vecDim.y, vecTL.x + vecDim.x, vecTL.y + vecDim.y, crgbaWhite, flZ, RENDERCONTEXT_DEFAULT);
}
#line 123
void sDrawTouchRefPoint(const Vector2D & vecTL, const Vector2D & vecDim)
{
#line 127
    if (!tControllerGlobals.bShowRefPoint)
        return;
#line 133
    if (sgtTouchRefPointTexture == cgtImageTextureFrameNull)
    {
        ImageCollectionGetImageInfo("UI:Console_DebugControllerPosition3", sgtTouchRefPointTexture);
        if (sgtTouchRefPointTexture == cgtImageTextureFrameNull)
            return;
    }
#line 142
    float flX = vecTL.x + float(tControllerGlobals.vecTouchRefPoint.x >> 2);
    float flY = vecTL.y + float(tControllerGlobals.vecTouchRefPoint.y >> 2);
    Rect2D rectRefPoint(flX - 6.5f, flY - 6.5f, flX + 13.0f, flY + 13.0f);
    SpriteDrawDeferred(sgtTouchRefPointTexture, SNO_NONE, rectRefPoint, crgbaWhite, crgbaBlack, 0.0f, NULL, SNO_NONE, NULL, FALSE, RENDERLAYER_UI_OVERLAY, FALSE);
}
#line 160
void sDrawTouch(const Vector2D & vecTL, const Vector2D & vecDim, int32 nIndex)
{
#line 165
    TouchState tTouch = InputGetCurrentState(INPUTCONTEXT_DEFAULT).tControllerState[tControllerGlobals.nControllerIndex].tCurrentFrame.arTouches[nIndex];
#line 168
    if (sgtTouchTextures[nIndex] == cgtImageTextureFrameNull)
    {
        ImageCollectionGetImageInfo(sgtTouchTextureNames[nIndex], sgtTouchTextures[nIndex]);
        if (sgtTouchTextures[nIndex] == cgtImageTextureFrameNull)
            return;
    }
#line 178
    float flX = vecTL.x + float(tTouch.uX >> 2);
    float flY = vecTL.y + float(tTouch.uY >> 2);
    Rect2D rectTouch(flX - 6.5f, flY - 6.5f, flX + 13.0f, flY + 13.0f);
    SpriteDrawDeferred(sgtTouchTextures[nIndex], SNO_NONE, rectTouch, crgbaWhite, crgbaBlack, 0.0f, NULL, SNO_NONE, NULL, FALSE, RENDERLAYER_UI_OVERLAY, FALSE);
}
#line 196
void sDrawTouches(const Vector2D & vecTL, const Vector2D & vecDim)
{
#line 200
    int32 nNumTouches = InputGetCurrentState(INPUTCONTEXT_DEFAULT).tControllerState[tControllerGlobals.nControllerIndex].tCurrentFrame.nNumTouches;
    for (int32 i = 0; i < nNumTouches; ++i)
    {
        sDrawTouch(vecTL, vecDim, i);
    }
}
namespace Console {

#line 208
void TouchPadDebugDraw(const uint8 bLayerAlpha, float & flZSlice)
{
#line 212
    float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
    ++flZSlice;
#line 215
    Vector2D vecTL, vecDim;
    sGetTouchPadDebugViewLocation(vecTL, vecDim);
    sDrawTouchPadBorder(vecTL, vecDim, flZ);
    sDrawTouchRefPoint(vecTL, vecDim);
    sDrawTouches(vecTL, vecDim);
}
}
namespace Console {
#line 223
void TouchPadDebugSetReferencePoint(const IVector2D & vecPoint)
{
    tControllerGlobals.vecTouchRefPoint = vecPoint;
    tControllerGlobals.bShowRefPoint = TRUE;
}
#line 231
void TouchPadDebugRemoveReferencePoint()
{
    tControllerGlobals.bShowRefPoint = FALSE;
}
}
#line 238
static void sUpdateLeftAnalogStick(const Player * ptPlayer)
{
#line 241
    const UIWindow & tLStickIndicator = UI_GetWindowFromID(UIWND_LSTICK_CONTAINER);
    Vector2D vLStickIndicatorDim;
    {
        Vector2D vTL;
        tLStickIndicator.GetRelativePosition(vTL, vLStickIndicatorDim);
    }
#line 248
    Vector2D vDelta;
#line 251
    UIWindow & tLStickLoc = UI_GetWindowFromID(UIWND_LSTICK_POS);
#line 253
    Vector2D vTL, vDim;
    tLStickLoc.GetRelativePosition(vTL, vDim);
#line 257
    vDelta = vDim * 0.5f;
#line 260
    Vector2D vLStickPos = Console::ControllerUtils::GetRawLStickPosition(ptPlayer->nControllerIndex, TRUE);
#line 264
    vTL.x = vLStickIndicatorDim.x * ((vLStickPos.x + 1.0f) * 0.5f);
    vTL.y = vLStickIndicatorDim.y * ((1.0f - vLStickPos.y) * 0.5f);
    vTL = vTL - vDelta;
    tLStickLoc.SetRelativePosition(vTL, vDim);
    {
#line 272
        UIWindow & tLStickDeadZone = UI_GetWindowFromID(UIWND_LSTICK_DEADZONE);
        ScopedGlobalDefinitionLockR pdefGlobal;
        float flDeadZone = pdefGlobal->flControllerDeadZone;
        if (Console::ControllerUtils::IsStickRebounding(ptPlayer->nControllerIndex, FALSE))
        {
            flDeadZone = Math::Max(flDeadZone, pdefGlobal->flControllerReboundDeadZone);
        }
#line 280
        vDim = vLStickIndicatorDim * flDeadZone;
        vDelta = vDim * 0.5f;
        vTL = vLStickIndicatorDim * 0.5f - vDelta;
        tLStickDeadZone.SetRelativePosition(vTL, vDim);
    }
#line 287
    UIText & tInputText = UI_GetTextFromID(UITXT_INPUT_DEBUG);
    CRefString szStickText;
    szStickText.Format("<%.03f, %.03f> %.03f", vLStickPos.x, vLStickPos.y, vLStickPos.Magnitude());
    tInputText.SetFontSize(12);
    tInputText.SetText(szStickText.str(), FALSE);
}
