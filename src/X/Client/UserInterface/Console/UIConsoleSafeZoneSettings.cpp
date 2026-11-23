// Original file: X/Client/UserInterface/Console/UIConsoleSafeZoneSettings.cpp
#include "ConsoleInputUtilities.h"
#include "ControllerActions.h"
#include "../Controls/UIWindowControl.h"
#include "../Interface/UIFunctions.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../UIMessage.h"
#include "../../Console/ControllerUtils.h"
#include "../../Input/Input.h"
#include "../../Gfx/GfxCommon.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/Math/MathGeneral.h"
// MathGeneral.inl comes in through MathGeneral.h
#include "../../../Sigma/Math/Rect.h"
#include "../../../Sigma/RGBAColor.h"
// RGBAColor.inl comes in through RGBAColor.h
#include "../../../Sigma/RefString.h"
#include "../../Sound/Sound.h"
#include <float.h>
// Declared here because their headers are not adopted yet; each comment names the defining file.


UI & UIGet();  // UI.cpp
int32 UI_GetPrimaryUIController();  // UI.cpp:538
const UIID & UI_GetControllerFocus();  // UI.cpp:674
CRefString StringListFetch(LPCSTR szLabel);  // StringList.cpp
void CGameGetLetterboxedViewportPixels(IRect2D & rectScreen);  // CGame.cpp
void GfxRectFrameDrawDeferred(const Rect2D & wrRect, const RGBAColor & rgbaColor, const float flZ, const RenderLayer eRenderLayerOverride);  // GfxPrimitives.cpp:205
namespace UIFirstTimeFlow { namespace Console { void OnSafeZoneSetupComplete(); BOOL IsActive(); } }  // UIConsoleFirstTimeFlow.cpp
namespace Console { namespace Screens { ControllerActionManager & GetActionManager(); void UpdateButtonLegendText(); } }  // ConsoleScreenManager.cpp
namespace UISafeZoneSettings {
void DrawSafeZoneRect();
class UISafeZoneControl : public UIWindow {
public:
    UISafeZoneControl(const WindowCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim)
        : UIWindow(tParams, vecTL, vecDim)
    {
    }

    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);
    virtual BOOL HandlesInput();
};

enum {
    ACTION_ACCEPT = 0,
    ACTION_CANCEL = 1,
};




void UISafeZoneControl::Draw(const uint8 bLayerAlpha, float & flZSlice)
{
    if (!IsActive())
    {
        return;
    }

    // Window alpha.
    uint8 bAlphaCascaded = MultiplyAlpha(bLayerAlpha, GetWindowColor().a);
    if (bAlphaCascaded == 0)
    {
        return;
    }

    flZSlice += 1.0f;

    DrawSafeZoneRect();
}







BOOL UISafeZoneControl::HandlesInput()
{
    return TRUE;
}






static const int32 SPEED_LIMIT_MS = 15;
static const float DPAD_SAFE_ZONE_DELTA = 0.0012f;
static const float STICK_SENSITIVITY = 0.3f;
static const float STICK_RANGE = 0.7f;
static const ::Console::AnalogStickInputFilter::Rule::ID RULE_STICK_UP = 1;
static const ::Console::AnalogStickInputFilter::Rule::ID RULE_STICK_DOWN = 2;
static const ::Console::AnalogStickInputFilter::Rule::ID RULE_STICK_LEFT = 3;
static const ::Console::AnalogStickInputFilter::Rule::ID RULE_STICK_RIGHT = 4;
static const float BOUNDARY_VERTICAL = 0.0595f;
static const float BOUNDARY_HORIZONTAL = 0.055f;









struct SafeZoneSettingsScreen
{
    SafeZoneSettingsScreen()
        : nControllerIndex(-1), idSafeZoneControl(GetUIIDNone()), idPreviousWindow(GetUIIDNone()), pfnOnClose(NULL)
    {
        tStickInputFilter.SetMinimumInputDelay(SPEED_LIMIT_MS);

        ::Console::AnalogStickInputFilter::Rule tStickUpRule;
        tStickUpRule.id = RULE_STICK_UP;
        tStickUpRule.vecMinStickPos.Set(-STICK_SENSITIVITY, STICK_SENSITIVITY);
        tStickUpRule.vecMaxStickPos.Set(STICK_SENSITIVITY, FLT_MAX);
        tStickInputFilter.AddRule(tStickUpRule);

        ::Console::AnalogStickInputFilter::Rule tStickDownRule;
        tStickDownRule.id = RULE_STICK_DOWN;
        tStickDownRule.vecMinStickPos.Set(-STICK_SENSITIVITY, -FLT_MAX);
        tStickDownRule.vecMaxStickPos.Set(STICK_SENSITIVITY, -STICK_SENSITIVITY);
        tStickInputFilter.AddRule(tStickDownRule);

        ::Console::AnalogStickInputFilter::Rule tStickLeftRule;
        tStickLeftRule.id = RULE_STICK_LEFT;
        tStickLeftRule.vecMinStickPos.Set(-FLT_MAX, -STICK_SENSITIVITY);
        tStickLeftRule.vecMaxStickPos.Set(-STICK_SENSITIVITY, STICK_SENSITIVITY);
        tStickInputFilter.AddRule(tStickLeftRule);

        ::Console::AnalogStickInputFilter::Rule tStickRightRule;
        tStickRightRule.id = RULE_STICK_RIGHT;
        tStickRightRule.vecMinStickPos.Set(STICK_SENSITIVITY, -STICK_SENSITIVITY);
        tStickRightRule.vecMaxStickPos.Set(FLT_MAX, STICK_SENSITIVITY);
        tStickInputFilter.AddRule(tStickRightRule);
    }

    int32 nControllerIndex;
    UIID idSafeZoneControl;
    UIID idPreviousWindow;
    Rect2D rectInitialSafeZone;
    UIFunctionPtr pfnOnClose;
    ::Console::AnalogStickInputFilter tStickInputFilter;
};











static SafeZoneSettingsScreen * sgptSafeZoneSettingsScreen = NULL;



static void CreateSafeRectWindow()
{
    // Create the safe zone rect window if needed.
    if (sgptSafeZoneSettingsScreen->idSafeZoneControl == GetUIIDNone())
    {
        Vector2D vecTL, vecDim;
        UI_GetWindowFromID(UIDLG_CONSOLE_SAFEZONE_SETTINGS).GetRelativePosition(vecTL, vecDim);
        vecTL.Set(0.0f, 0.0f);

        WindowCreationParams tWindowParams(UIT_WINDOW, "SafeZoneRect", UIDLG_CONSOLE_SAFEZONE_SETTINGS);
        tWindowParams.eType = UIT_SAFEZONESETTER;
        UISafeZoneControl * ptSafeZoneRect = new UISafeZoneControl(tWindowParams, vecTL, vecDim);


        sgptSafeZoneSettingsScreen->idSafeZoneControl = ptSafeZoneRect->GetID();
    }
}




void DrawSafeZoneRect()
{
    IRect2D rectViewPortPixels;
    CGameGetLetterboxedViewportPixels(rectViewPortPixels);

    // Shrink the viewport to the safe zone.
    Rect2D rectSafeZonePixels((float)rectViewPortPixels.left, (float)rectViewPortPixels.top, (float)rectViewPortPixels.right, (float)rectViewPortPixels.bottom);
    UIGet().tSafeZone.ApplyToRect(rectSafeZonePixels);

    // Draw the box.


    RGBAColor rgbaBoxColor(0xff, 0xff, 0xff);
    GfxRectFrameDrawDeferred(rectSafeZonePixels, rgbaBoxColor, 0.0f, RENDERLAYER_INVALID);
}













void Open(const int32 nControllerIndex)
{
    if (sgptSafeZoneSettingsScreen == NULL)
    {
        return;
    }

    sgptSafeZoneSettingsScreen->nControllerIndex = nControllerIndex;









    sgptSafeZoneSettingsScreen->idPreviousWindow = UI_GetControllerFocus();
    sgptSafeZoneSettingsScreen->rectInitialSafeZone = UIGet().tSafeZone.GetMargins();

    UI_GetWindowFromID(UIDLG_CONSOLE_SAFEZONE_SETTINGS).SetActive(TRUE);

    ::Console::Screens::GetActionManager().ClearAllActions();
    ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT, StringListFetch("Console:Accept").str());
    if (!UIFirstTimeFlow::Console::IsActive())
    {
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CANCEL, StringListFetch("Console:Cancel").str());
    }
    ::Console::Screens::UpdateButtonLegendText();

    CreateSafeRectWindow();

    // Restore the initial safe zone.
    UIGet().tSafeZone.SetMargins(sgptSafeZoneSettingsScreen->rectInitialSafeZone.left, sgptSafeZoneSettingsScreen->rectInitialSafeZone.top,
        sgptSafeZoneSettingsScreen->rectInitialSafeZone.right, sgptSafeZoneSettingsScreen->rectInitialSafeZone.bottom);
}




BOOL IsVisible()
{
    return UI_GetWindowFromID(UIDLG_CONSOLE_SAFEZONE_SETTINGS).IsActive();
}




void Close()
{
    if (!IsVisible())
    {
        return;
    }

    UI_GetWindowFromID(UIDLG_CONSOLE_SAFEZONE_SETTINGS).SetActive(FALSE);










    if (sgptSafeZoneSettingsScreen->idPreviousWindow != GetUIIDNone())
    {
        UI_SetControllerFocus(sgptSafeZoneSettingsScreen->idPreviousWindow);
    }

    if (sgptSafeZoneSettingsScreen->pfnOnClose)
    {
        sgptSafeZoneSettingsScreen->pfnOnClose(UIDLG_CONSOLE_SAFEZONE_SETTINGS);
    }
}

void InitializeForGame()
{
    sgptSafeZoneSettingsScreen = new SafeZoneSettingsScreen;
}



void DisposeForGame()
{
    delete sgptSafeZoneSettingsScreen;
    sgptSafeZoneSettingsScreen = NULL;
}




void OnButton1(const UIID &, const int32 nControllerIndex)
{
    if (UI_GetPrimaryUIController() != nControllerIndex)
    {
        return;
    }

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);

    if (UIFirstTimeFlow::Console::IsActive())
    {
        UIFirstTimeFlow::Console::OnSafeZoneSetupComplete();
    }
    else
    {
        Close();
    }
}





void OnButton2(const UIID &, const int32 nControllerIndex)
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);

    if (UIFirstTimeFlow::Console::IsActive())
    {
        UIFirstTimeFlow::Console::OnSafeZoneSetupComplete();
        return;
    }

    // Restore the initial safe zone.
    UIGet().tSafeZone.SetMargins(sgptSafeZoneSettingsScreen->rectInitialSafeZone.left, sgptSafeZoneSettingsScreen->rectInitialSafeZone.top,
        sgptSafeZoneSettingsScreen->rectInitialSafeZone.right, sgptSafeZoneSettingsScreen->rectInitialSafeZone.bottom);
    Close();
}









static int32 sConvertFloatToDirection(float x)
{
    if (x > 0.000001f)
    {
        return 1;
    }
    if (x < -0.000001f)
    {
        return -1;
    }
    return 0;
}







void AttemptMove(const Vector2D & vecDelta)
{
    // Is it moving?
    IVector2D ivecDirection(sConvertFloatToDirection(vecDelta.x), sConvertFloatToDirection(vecDelta.y));
    if (ivecDirection.x == 0 && ivecDirection.y == 0)
    {
        return;
    }

    Rect2D rectSafeZone = UIGet().tSafeZone.GetMargins();
    rectSafeZone.top = Math::Pin(rectSafeZone.top + vecDelta.y, 0.0f, BOUNDARY_VERTICAL);
    rectSafeZone.bottom = Math::Pin(rectSafeZone.bottom + vecDelta.y, 0.0f, BOUNDARY_VERTICAL);
    rectSafeZone.left = Math::Pin(rectSafeZone.left + vecDelta.x, 0.0f, BOUNDARY_HORIZONTAL);
    rectSafeZone.right = Math::Pin(rectSafeZone.right + vecDelta.x, 0.0f, BOUNDARY_HORIZONTAL);
    UIGet().tSafeZone.SetMargins(rectSafeZone.left, rectSafeZone.top, rectSafeZone.right, rectSafeZone.bottom);
}






void OnLeftStick(const UIID &, const int32 nControllerIndex)
{
    const float SAFE_ZONE_DELTA = 0.0015f;
    if (UI_GetPrimaryUIController() != nControllerIndex)
    {
        return;
    }





    Vector2D vLStickPosition = ::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE);
    ::Console::AnalogStickInputFilter::Rule::ID idRule = sgptSafeZoneSettingsScreen->tStickInputFilter.HandleInput(vLStickPosition, FALSE);
    BOOL fHandleMove = TRUE;
    Vector2D vecDelta(0.0f, 0.0f);

    switch (idRule)
    {
    case RULE_STICK_UP:

        vecDelta.y = (vLStickPosition.y - STICK_SENSITIVITY) * -SAFE_ZONE_DELTA / STICK_RANGE;
        break;
    case RULE_STICK_DOWN:

        vecDelta.y = (-vLStickPosition.y - STICK_SENSITIVITY) * SAFE_ZONE_DELTA / STICK_RANGE;
        break;
    case RULE_STICK_LEFT:

        vecDelta.x = (-vLStickPosition.x - STICK_SENSITIVITY) * -SAFE_ZONE_DELTA / STICK_RANGE;
        break;
    case RULE_STICK_RIGHT:

        vecDelta.x = (vLStickPosition.x - STICK_SENSITIVITY) * SAFE_ZONE_DELTA / STICK_RANGE;
        break;
    case ::Console::AnalogStickInputFilter::Rule::IN_DEAD_ZONE:
        break;
    default:
        fHandleMove = FALSE;
        break;
    }

    if (fHandleMove)
    {
        AttemptMove(vecDelta);
    }
}


BOOL HandleDPadInput(const BufferedInput & tInput)
{
    static ::Console::InputSpeedLimiter tSpeedLimiter;
    BOOL bIgnoreInput = !tSpeedLimiter.HasWaitTimeExpired(SPEED_LIMIT_MS);
    if (bIgnoreInput)
    {
        return FALSE;
    }

    Rect2D rectSafeZone = UIGet().tSafeZone.GetMargins();
    Vector2D vecDelta(0.0f, 0.0f);
    switch (tInput.tBufControllerData.eInputButton)
    {
    case BUTTON_DPAD_LEFT:
        vecDelta.x = -DPAD_SAFE_ZONE_DELTA;
        break;

    case BUTTON_DPAD_RIGHT:
        vecDelta.x = DPAD_SAFE_ZONE_DELTA;
        break;

    case BUTTON_DPAD_UP:
        vecDelta.y = -DPAD_SAFE_ZONE_DELTA;
        break;

    case BUTTON_DPAD_DOWN:
        vecDelta.y = DPAD_SAFE_ZONE_DELTA;
        break;
    }

    AttemptMove(vecDelta);
    tSpeedLimiter.UpdateLastInputTime();
    return TRUE;
}




BOOL UISafeZoneControl::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{

    switch (msg)
    {
    case UI_INPUT:
        {
            const BufferedInput & tInput = *(const BufferedInput *)dwParam;

            if (UI_GetPrimaryUIController() != tInput.tBufControllerData.nControllerIndex)
            {
                break;
            }
            if (tInput.eInputMsg != INPUTMSG_CONTROLLERBUTTONDOWN && tInput.eInputMsg != INPUTMSG_CONTROLLERBUTTONSTILLDOWN && tInput.eInputMsg != INPUTMSG_CONTROLLERBUTTONREPEAT)
            {
                break;
            }

            if (tInput.tBufControllerData.eInputButton != BUTTON_DPAD_UP && tInput.tBufControllerData.eInputButton != BUTTON_DPAD_DOWN && tInput.tBufControllerData.eInputButton != BUTTON_DPAD_LEFT && tInput.tBufControllerData.eInputButton != BUTTON_DPAD_RIGHT)
            {
                break;
            }

            return HandleDPadInput(tInput);
        }
        break;

    case UI_REGISTER_INPUT_LISTENER:
        {



            UI_RegisterInputListener(this);
        }
        break;
    }


    return FALSE;
}







void SetOnCloseFunction(const UIFunctionPtr pfnOnClose)
{
    sgptSafeZoneSettingsScreen->pfnOnClose = pfnOnClose;
}

}  // namespace UISafeZoneSettings
