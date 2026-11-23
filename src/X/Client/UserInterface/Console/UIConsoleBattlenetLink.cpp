// Original file: X/Client/UserInterface/Console/UIConsoleBattlenetLink.cpp
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Interface/UIFunctions.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../../Gfx/GfxVB.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/RefString.h"

// Declared here until their headers carry them (signatures from the DWARF).
int32 UI_GetPrimaryUIController();  // UI.cpp:538
void UI_SetControllerFocus( const UIID & idFocus );  // UI.cpp:599
namespace Console { namespace WebBrowser {
BOOL NavigateTo( const CRefString & sUri, int32 nControllerIndex );  // WebBrowserNX64.cpp:26
} }

namespace UIBattlenetLink {
void OpenForController( int32 nControllerIndex, BOOL bIsFirstTimeFlow );
BOOL IsVisible();
void Close();
void OnButton1( const UIID &, const int32 nControllerIndex );
void OnButton2( const UIID &, const int32 nControllerIndex );
void OnButton4( const UIID &, const int32 nControllerIndex );
void SetOnCloseFunction( const UIFunctionPtr pfnOnClose );
}

struct UIBattlenetLinkData
{
    UIID idPreviousWindow;  // 0x00
    UIFunctionPtr pfnOnClose;  // 0x08
    BOOL bIsFirstTimeFlow;  // 0x10


    UIBattlenetLinkData()
        : idPreviousWindow( GetUIIDNone() )
        , pfnOnClose( NULL )
        , bIsFirstTimeFlow( FALSE )
    {
    }
};
static UIBattlenetLinkData sgtBattlenetLinkData;




static void sCloseScreen()
{
    SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ), cgtPSODefaults );
    UIBattlenetLink::Close();
}
















void UIBattlenetLink::OpenForController( int32 nControllerIndex, BOOL bIsFirstTimeFlow )
{
    sgtBattlenetLinkData.bIsFirstTimeFlow = bIsFirstTimeFlow;

    UI_GetWindowFromID( UIWND_CONSOLE_BATTLENETLINK_SCREEN ).SetActive( TRUE );

    CRefString sUrl = StringListFetch( "ConsoleUI:AccountLinkBnetURL" );
    UI_GetTextFromID( UITEXT_CONSOLE_BATTLENETLINK ).SetText( LocalizedSprintf( StringListFetch( "ConsoleUI:AccountLinkBnetSubTitle" ).str(), sUrl.str(), NULL ), FALSE );

    ::Console::Screens::GetActionManager().ClearAllActions();
    ::Console::Screens::GetActionManager().SetAction( ::Console::CONTROLLER_INPUT_BUTTON_4, 0, StringListFetch( "ConsoleUI:LinkNow" ).str() );
    if ( bIsFirstTimeFlow )
    {
        ::Console::Screens::GetActionManager().SetAction( ::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch( "H2OLayout:Continue" ).str() );
    }
    else
    {
        ::Console::Screens::GetActionManager().SetAction( ::Console::CONTROLLER_INPUT_BUTTON_2, 0, StringListFetch( "ConsoleUI:Close" ).str() );
    }
    ::Console::Screens::UpdateButtonLegendText();
}





















BOOL UIBattlenetLink::IsVisible()
{
    return UI_GetWindowFromID( UIWND_CONSOLE_BATTLENETLINK_SCREEN ).IsActive();
}





void UIBattlenetLink::Close()
{
    if ( IsVisible() )
    {
        UI_GetWindowFromID( UIWND_CONSOLE_BATTLENETLINK_SCREEN ).SetActive( FALSE );

        if ( sgtBattlenetLinkData.idPreviousWindow != GetUIIDNone() )
        {
            UI_SetControllerFocus( sgtBattlenetLinkData.idPreviousWindow );
        }

        if ( sgtBattlenetLinkData.pfnOnClose != NULL )
        {
            sgtBattlenetLinkData.pfnOnClose( UIWND_CONSOLE_BATTLENETLINK_SCREEN );
        }
    }
}

















static void sGoToLinkSite( int32 nControllerIndex )
{
    CRefString sUri;
    sUri.Format( "https://%s", StringListFetch( "ConsoleUI:AccountLinkBnetURL" ).str() );
    ::Console::WebBrowser::NavigateTo( sUri, nControllerIndex );
}






void UIBattlenetLink::OnButton1( const UIID &, const int32 nControllerIndex )
{
    if ( UI_GetPrimaryUIController() != nControllerIndex )
    {
        return;
    }

    if ( sgtBattlenetLinkData.bIsFirstTimeFlow )
    {
        sCloseScreen();
    }
}








void UIBattlenetLink::OnButton2( const UIID &, const int32 nControllerIndex )
{
    if ( UI_GetPrimaryUIController() != nControllerIndex )
    {
        return;
    }

    if ( !sgtBattlenetLinkData.bIsFirstTimeFlow )
    {
        sCloseScreen();
    }
}








void UIBattlenetLink::OnButton4( const UIID &, const int32 nControllerIndex )
{
    if ( UI_GetPrimaryUIController() != nControllerIndex )
    {
        return;
    }

    sGoToLinkSite( nControllerIndex );
}

















void UIBattlenetLink::SetOnCloseFunction( const UIFunctionPtr pfnOnClose )
{
    sgtBattlenetLinkData.pfnOnClose = pfnOnClose;
}

