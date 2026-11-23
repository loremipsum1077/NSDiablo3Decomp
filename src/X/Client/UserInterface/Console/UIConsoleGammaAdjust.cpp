// Original file: X/Client/UserInterface/Console/UIConsoleGammaAdjust.cpp
#include "ConsoleInputUtilities.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../Controls/UIWindowControl.h"
#include "../Interface/UIFunctions.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../../Console/ControllerUtils.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/Math/MathGeneral.h"

// Declared here until their headers carry them (signatures from the DWARF).
const UIID & UI_GetControllerFocus();  // UI.cpp:429
int32 UI_GetPrimaryUIController();  // UI.cpp:538
void UI_SetControllerFocus( const UIID & idFocus );  // UI.cpp:599
void GfxSetPrefsChanged();  // Gfx.cpp:1778

namespace UIGammaAdjust {
namespace Console {










static const int32 SPEED_LIMIT_MS = 250;

struct ConsoleGammaAdjustData
{
    ConsoleGammaAdjustData()
        : nControllerIndex( -1 )
        , idPreviousWindow( GetUIIDNone() )
        , fInitialGamma( 0.0f )
        , fCurrentGamma( 0.0f )
        , fGammaStepSize( 0.0f )
        , pfnOnClose( NULL )
        , bIsFirstTimeFlow( FALSE )
    {
        tStickInputFilter = ::Console::GetDefaultStickInputFilter( ::Console::FILTER_HORIZONTAL );
        tStickInputFilter.SetMinimumInputDelay( SPEED_LIMIT_MS );
    }

    int32 nControllerIndex;  // 0x00
    UIID idPreviousWindow;  // 0x08
    ::Console::AnalogStickInputFilter tStickInputFilter;  // 0x10
    float fInitialGamma;  // 0x38
    float fCurrentGamma;  // 0x3c
    float fGammaStepSize;  // 0x40
    UIFunctionPtr pfnOnClose;  // 0x48
    BOOL bIsFirstTimeFlow;  // 0x50
};















static ConsoleGammaAdjustData * sgptConsoleGammaAdjustData = NULL;
void AdjustSliderForCurrentGamma();
enum
{
    ACTION_NEXT,
    ACTION_ADJUST,
};

void InitializeForGame()
{
    sgptConsoleGammaAdjustData = new ConsoleGammaAdjustData();
}


void DisposeForGame()
{
    delete sgptConsoleGammaAdjustData;
    sgptConsoleGammaAdjustData = NULL;
}


void AdjustSliderForCurrentGamma()
{
    Vector2D vecSliderTL, vecSliderDim;
    UI_GetWindowFromID( UIWND_CONSOLE_GAMMAADJUSTSCREEN_SLIDER ).GetRelativePosition( vecSliderTL, vecSliderDim );

    Vector2D vecRangeTL, vecRangeDim;
    UI_GetWindowFromID( UIWND_CONSOLE_GAMMAADJUSTSCREEN_RANGE ).GetRelativePosition( vecRangeTL, vecRangeDim );

    float flPercent = ( sgptConsoleGammaAdjustData->fCurrentGamma - Prefs::GetVideoPrefs().GetGammaMin() ) / ( Prefs::GetVideoPrefs().GetGammaMax() - Prefs::GetVideoPrefs().GetGammaMin() );
    vecSliderTL.x = vecRangeTL.x + vecRangeDim.x * flPercent + vecSliderDim.x * -0.5f;

    UI_GetWindowFromID( UIWND_CONSOLE_GAMMAADJUSTSCREEN_SLIDER ).SetRelativePosition( vecSliderTL, vecSliderDim );
}






void OpenForController( int32 nControllerIndex, BOOL bInFirstTimeFlow )
{
    sgptConsoleGammaAdjustData->bIsFirstTimeFlow = bInFirstTimeFlow;
    sgptConsoleGammaAdjustData->nControllerIndex = nControllerIndex;
    sgptConsoleGammaAdjustData->fInitialGamma = sgptConsoleGammaAdjustData->fCurrentGamma = Prefs::GetVideoPrefs().GetGamma();
    sgptConsoleGammaAdjustData->fGammaStepSize = ( Prefs::GetVideoPrefs().GetGammaMax() - Prefs::GetVideoPrefs().GetGammaMin() ) / 10.0f;
    sgptConsoleGammaAdjustData->idPreviousWindow = UI_GetControllerFocus();

    AdjustSliderForCurrentGamma();

    UI_GetWindowFromID( UIWND_CONSOLE_GAMMAADJUSTSCREEN ).SetActive( TRUE );

    ::Console::Screens::GetActionManager().ClearAllActions();
    ::Console::Screens::GetActionManager().SetAction( ::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_NEXT, StringListFetch( "Console:Accept" ).str() );
    if ( !bInFirstTimeFlow )
    {
        ::Console::Screens::GetActionManager().SetAction( ::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_NEXT, StringListFetch( "Console:Cancel" ).str() );
    }
    ::Console::Screens::GetActionManager().SetAction( ::Console::CONTROLLER_INPUT_LEFT_STICK, ACTION_ADJUST, StringListFetch( "ConsoleUI:AdjustGamma" ).str() );
    ::Console::Screens::UpdateButtonLegendText();
}















BOOL IsVisible()
{
    return UI_GetWindowFromID( UIWND_CONSOLE_GAMMAADJUSTSCREEN ).IsActive();
}


void Close()
{
    if ( IsVisible() )
    {
        UI_GetWindowFromID( UIWND_CONSOLE_GAMMAADJUSTSCREEN ).SetActive( FALSE );

        if ( sgptConsoleGammaAdjustData->idPreviousWindow != GetUIIDNone() )
        {
            UI_SetControllerFocus( sgptConsoleGammaAdjustData->idPreviousWindow );
        }

        if ( sgptConsoleGammaAdjustData->pfnOnClose != NULL )
        {
            sgptConsoleGammaAdjustData->pfnOnClose( UIWND_CONSOLE_GAMMAADJUSTSCREEN );
        }
    }
}












void AdjustGamma( const float nAmount )
{
    float fNewGamma = Math::Pin( sgptConsoleGammaAdjustData->fCurrentGamma + nAmount, Prefs::GetVideoPrefs().GetGammaMin(), Prefs::GetVideoPrefs().GetGammaMax() );
    if ( Math::Abs( fNewGamma - sgptConsoleGammaAdjustData->fCurrentGamma ) > EPSILON )
    {
        sgptConsoleGammaAdjustData->fCurrentGamma = fNewGamma;
        AdjustSliderForCurrentGamma();
        Prefs::GetMutableVideoPrefs().SetGamma( sgptConsoleGammaAdjustData->fCurrentGamma );
        GfxSetPrefsChanged();
    }
}




void OnLeftStick( const UIID &, const int32 nControllerIndex )
{
    if ( UI_GetPrimaryUIController() != nControllerIndex )
    {
        return;
    }

    ::Console::AnalogStickInputFilter::Rule::ID idRule = sgptConsoleGammaAdjustData->tStickInputFilter.HandleInput( ::Console::ControllerUtils::GetLStickPosition( nControllerIndex, FALSE ), FALSE );
    switch ( idRule )
    {
    case ::Console::RULE_STICK_RIGHT:
        AdjustGamma( sgptConsoleGammaAdjustData->fGammaStepSize );
        break;
    case ::Console::RULE_STICK_LEFT:
        AdjustGamma( -sgptConsoleGammaAdjustData->fGammaStepSize );
        break;
    case ::Console::AnalogStickInputFilter::Rule::IN_DEAD_ZONE:
        AdjustGamma( 0.0f );
        break;
    }
}











void OnDPadLeft( const UIID & id, const int32 nControllerIndex )
{
    AdjustGamma( -sgptConsoleGammaAdjustData->fGammaStepSize );
}



void OnDPadRight( const UIID & id, const int32 nControllerIndex )
{
    AdjustGamma( sgptConsoleGammaAdjustData->fGammaStepSize );
}



void OnButton1( const UIID &, const int32 nControllerIndex )
{
    if ( UI_GetPrimaryUIController() != nControllerIndex )
    {
        return;
    }

    SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ), cgtPSODefaults );
    Close();
}



void OnButton2( const UIID &, const int32 nControllerIndex )
{
    if ( UI_GetPrimaryUIController() != nControllerIndex )
    {
        return;
    }

    if ( sgptConsoleGammaAdjustData->bIsFirstTimeFlow )
    {
        return;
    }

    SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ), cgtPSODefaults );
    Prefs::GetMutableVideoPrefs().SetGamma( sgptConsoleGammaAdjustData->fInitialGamma );
    GfxSetPrefsChanged();
    Close();
}










void SetOnCloseFunction( const UIFunctionPtr pfnOnClose )
{
    sgptConsoleGammaAdjustData->pfnOnClose = pfnOnClose;
}

}  // namespace Console
}  // namespace UIGammaAdjust
