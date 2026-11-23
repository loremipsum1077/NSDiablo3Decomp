// Original file: X/Client/UserInterface/Interface/UICountDown.cpp
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "UIFunctions.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../../CGame.h"
#include "../../Gfx/GfxVB.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RefString.h"

// Declared here until their headers carry them (signatures from the DWARF).
struct ActiveEvent;
ActiveEvent * CTimedEventsGetActiveEvent( const SNO snoTimedEvent );  // CTimedEvents.cpp:101
int32 CTimedEventGetRemainingTime( const SNO snoTimedEvent );  // CTimedEvents.cpp:137

namespace UICountDown {
void Close();
void OnExpired();

namespace {
struct CountDownUIGlobals
{
    UIFunctionPtr pfnOnExpired;  // 0x00
    UIFunctionPtr pfnOnCanceled;  // 0x08
    BOOL bResetOnMovement;  // 0x10
    uint32 dwStartTick;  // 0x14
    uint32 dwEndTick;  // 0x18
    SNO snoTimedEvent;  // 0x1c



    CountDownUIGlobals()
    {
        SigmaMemoryClear( this, sizeof( *this ) );
    }
};
static CountDownUIGlobals sgtUICountDownGlobals;
}  // namespace


void InitializeForGame()
{
    Close();
}



void Close()
{

    if ( UI_TestID( UIDLG_COUNT_DOWN ) )
    {
        UI_GetWindowFromID( UIDLG_COUNT_DOWN ).SetActive( FALSE );
    }

    if ( UI_TestID( UITXT_COUNTDOWN_TIMER ) )
    {
        UI_GetTextFromID( UITXT_COUNTDOWN_TIMER ).SetActive( FALSE );
    }

    sgtUICountDownGlobals.dwEndTick = 0;
    sgtUICountDownGlobals.bResetOnMovement = FALSE;
    sgtUICountDownGlobals.pfnOnExpired = NULL;
    sgtUICountDownGlobals.pfnOnCanceled = NULL;
    sgtUICountDownGlobals.snoTimedEvent = SNO_NONE;
}


void OnExpired()
{
    UIFunctionPtr pfnOnExpired = sgtUICountDownGlobals.pfnOnExpired;

    Close();

    if ( pfnOnExpired != NULL )
    {
        pfnOnExpired( GetUIIDNone() );
    }
}



BOOL IsVisible()
{
    return UI_GetWindowFromID( UIDLG_COUNT_DOWN ).IsActive();
}



void OnUpdate( const UIID & )
{
    const uint32 dwCurrentTick = CGameGetPredictedServerTime();
    if ( dwCurrentTick > sgtUICountDownGlobals.dwEndTick )
    {
        OnExpired();
        return;
    }

    uint32 dwRemainingTicks = sgtUICountDownGlobals.dwEndTick - dwCurrentTick;


    if ( sgtUICountDownGlobals.snoTimedEvent != SNO_NONE )
    {
        if ( CTimedEventsGetActiveEvent( sgtUICountDownGlobals.snoTimedEvent ) == NULL )
        {
            OnExpired();
            return;
        }



        dwRemainingTicks = CTimedEventGetRemainingTime( sgtUICountDownGlobals.snoTimedEvent );
    }


    UI_GetTextFromID( UITXT_COUNTDOWN_TIMER ).SetText(
        CRefString().Format( "%02d", dwRemainingTicks / 60 ),
        FALSE );
}

void Show( uint32 dwDurationTicks, LPCSTR szMessage, UIFunctionPtr pfnOnExpire, UIFunctionPtr pfnOnCanceled, BOOL bCancelOnMovement, BOOL bPositionHigh, SNO snoTimedEvent )
{






    Close();




    sgtUICountDownGlobals.dwStartTick = CGameGetPredictedServerTime();
    sgtUICountDownGlobals.dwEndTick = CGameGetPredictedServerTime() + dwDurationTicks;

    sgtUICountDownGlobals.pfnOnExpired = pfnOnExpire;
    sgtUICountDownGlobals.pfnOnCanceled = pfnOnCanceled;
    sgtUICountDownGlobals.bResetOnMovement = bCancelOnMovement;
    sgtUICountDownGlobals.snoTimedEvent = snoTimedEvent;
    UI_GetWindowFromID( UIDLG_COUNT_DOWN ).SetActive( TRUE );
    if ( UI_TestID( UITXT_COUNTDOWN_TIMER ) )
    {
        UI_GetTextFromID( UITXT_COUNTDOWN_TIMER ).SetActive( TRUE );
    }



    UIText & tTitle = UI_GetTextFromID( UITXT_COUNTDOWN_TIMER_TITLE );
    if ( szMessage[0] != '\0' )
    {
        tTitle.SetText( szMessage, FALSE );
        tTitle.SetActive( TRUE );
    }
    else
    {
        tTitle.SetActive( FALSE );
    }

    UIWindow & tDialog = UI_GetWindowFromID( UIDLG_COUNT_DOWN );
    Vector2D vecTL, vecDim;
    tDialog.GetRelativePosition( vecTL, vecDim );

    UIWindow & tAnchor = UI_GetWindowFromID( bPositionHigh ? UIWND_COUNT_DOWN_ANCHOR_HIGH : UIWND_COUNT_DOWN_ANCHOR_LOW );
    Vector2D vecAnchorTL, vecAnchorDim;
    tAnchor.GetRelativePosition( vecAnchorTL, vecAnchorDim );







    tDialog.SetRelativePosition( vecAnchorTL, vecDim );
}


void Show( float flDuration, LPCSTR szMessage, UIFunctionPtr pfnOnExpire, UIFunctionPtr pfnOnCanceled, BOOL bCancelOnMovement, BOOL bPositionHigh, SNO snoTimedEvent )
{
    Show( (uint32)( flDuration * 60.0f ), szMessage, pfnOnExpire, pfnOnCanceled, bCancelOnMovement, bPositionHigh, snoTimedEvent );
}










void PlayerMoved()
{
    if ( sgtUICountDownGlobals.bResetOnMovement )
    {
        UIFunctionPtr pfnOnCanceled = sgtUICountDownGlobals.pfnOnCanceled;

        Close();

        if ( pfnOnCanceled != NULL )
        {
            pfnOnCanceled( GetUIIDNone() );
        }
    }
}

}  // namespace UICountDown
