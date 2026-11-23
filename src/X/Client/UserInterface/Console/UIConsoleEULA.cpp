// Original file: X/Client/UserInterface/Console/UIConsoleEULA.cpp
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../Gfx/GfxVB.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Sigma/RefString.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Types.h"

// Declared here until their headers carry them (signatures from the DWARF).
int32 UI_GetPrimaryUIController();  // UI.cpp:538
namespace Console { namespace WebBrowser {
BOOL NavigateTo( const CRefString & sUri, int32 nControllerIndex );  // WebBrowserNX64.cpp:26
} }
namespace Console { namespace GamerProfile {
void SavePrefs();
} }
namespace UIStartScreen { namespace Console {
void OnEULAComplete( int32 nControllerIndex, BOOL bAccepted );
} }

namespace UIEULA {
namespace Console {

void AcceptEULA( const UIID & id, const int32 nControllerIndex );
void ViewLink( const UIID & id, const int32 nControllerIndex );
void Close();

struct LegalLink
{
    const char * TextLabel;
    const char * UrlLabel;
};
static const LegalLink arLinks[] =
{
    { "ConsoleUI:EULA_LinkText_EULA", "ConsoleUI:EULA_URL_EULA" },
    { "ConsoleUI:EULA_LinkText_Privacy", "ConsoleUI:EULA_URL_Privacy" },
};

class LegalLinksDataSource : public ConsoleListBoxDataSource
{
public:
    LegalLinksDataSource()
    {
    }



    virtual int32 GetNumItems() const
    {
        return sizeof( arLinks ) / sizeof( arLinks[0] );
    }



    virtual UIControlSourceKey GetSourceKeyForItem( const int32 nIndex ) const
    {
        return UIControlSourceKey::Parse( "ConsoleEULA:Console_EULA.menuItem", SNO_NONE, NULL, NULL );
    }




    virtual void InitializeItemUI( const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex )
    {
    }








    virtual void UpdateItemUI( const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex )
    {


        const UIID idText = UIIDCreateFromParent( idItemUI, ".text" );
        CRefString sText = StringListFetch( arLinks[nIndex].TextLabel );


        UIText & tText = UI_GetTextFromID( idText );

        tText.SetText( sText, FALSE );



        const UIID idHighlight = UIIDCreateFromParent( idItemUI, ".highlight" );
        UIWindow & tHighlight = UI_GetWindowFromID( idHighlight );
        RGBAColor rgbaText;
        if ( tListBox.GetSelectedIndex() == nIndex )
        {
            rgbaText = crgbaGold;
            tHighlight.SetActive( TRUE );
        }
        else
        {
            rgbaText = crgbaWhite;
            tHighlight.SetActive( FALSE );
        }

        tText.SetTextColor( rgbaText );
    }
};

struct UIEULAData
{
    LegalLinksDataSource tDataSource;  // 0x00
    BOOL bConnected;  // 0x08

    UIEULAData()
        : bConnected( FALSE )
    {
    }
};


UIEULAData sgtEULAData;

enum
{
    ACTION_OK,
    ACTION_CANCEL,
    ACTION_VIEW,
};


void OpenForController( int32 nControllerIndex )
{
    ::Console::Screens::ShowModalDialog( nControllerIndex, UIWND_CONSOLE_EULA, TRUE, GetUIIDNone() );
    sgtEULAData.bConnected = FALSE;

    ::Console::Screens::GetActionManager().ClearAllActions();





    if ( sgtEULAData.bConnected )
    {
        UI_GetWindowFromID( UIWND_CONSOLE_EULA_ONLINE ).SetActive( TRUE );

        UI_GetWindowFromID( UIWND_CONSOLE_EULA_OFFLINE ).SetActive( FALSE );
        UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID( UILIST_CONSOLE_EULA_LINKS );

        tListBox.SetDataSource( sgtEULAData.tDataSource );
        tListBox.RefreshVisibleItems();
        tListBox.SetSelectedIndex( nControllerIndex );

        ::Console::Screens::GetActionManager().SetAction( ::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_OK, StringListFetch( "ConsoleUI:EULA_Continue" ).str() );
        ::Console::Screens::GetActionManager().SetAction( ::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_VIEW, StringListFetch( "ConsoleUI:EULA_ViewLink" ).str() );
    }
    else
    {
        UI_GetWindowFromID( UIWND_CONSOLE_EULA_ONLINE ).SetActive( FALSE );
        UI_GetWindowFromID( UIWND_CONSOLE_EULA_OFFLINE ).SetActive( TRUE );
        ::Console::Screens::GetActionManager().SetAction( ::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OK, StringListFetch( "ConsoleUI:EULA_Continue" ).str() );
    }

    ::Console::Screens::GetActionManager().SetAction( ::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CANCEL, StringListFetch( "ConsoleUI:EULA_NoContinue" ).str() );


    ::Console::Screens::UpdateButtonLegendText();
}


BOOL IsVisible()
{
    return UI_GetWindowFromID( UIWND_CONSOLE_EULA ).IsActive();
}


void OnButton1( const UIID & id, const int32 nControllerIndex )
{

    if ( UI_GetPrimaryUIController() != nControllerIndex )
    {
        return;
    }


    if ( ::Console::Screens::GetActionManager().IsActionDelayed( ::Console::CONTROLLER_INPUT_BUTTON_1 ) )
    {
        return;
    }

    if ( sgtEULAData.bConnected )
    {
        ViewLink( id, nControllerIndex );
    }
    else
    {
        AcceptEULA( id, nControllerIndex );
    }

}

void OnButton2( const UIID & id, const int32 nControllerIndex )
{
    if ( UI_GetPrimaryUIController() != nControllerIndex )
    {
        return;
    }

    if ( ::Console::Screens::GetActionManager().IsActionDelayed( ::Console::CONTROLLER_INPUT_BUTTON_2 ) )
    {
        return;
    }

    Close();
    UIStartScreen::Console::OnEULAComplete( nControllerIndex, FALSE );
}



void OnButton4( const UIID & id, const int32 nControllerIndex )
{
    if ( UI_GetPrimaryUIController() != nControllerIndex )
    {
        return;
    }

    if ( ::Console::Screens::GetActionManager().IsActionDelayed( ::Console::CONTROLLER_INPUT_BUTTON_4 ) )
    {
        return;
    }

    if ( sgtEULAData.bConnected )
    {
        AcceptEULA( id, nControllerIndex );
    }
}

void AcceptEULA( const UIID & id, const int32 nControllerIndex )
{




    Prefs::SetEulaAccepted( 0 );
    ::Console::GamerProfile::SavePrefs();

    Close();
    UIStartScreen::Console::OnEULAComplete( nControllerIndex, TRUE );
}

void ViewLink( const UIID & id, const int32 nControllerIndex )
{


    const int32 nIndex = UI_GetConsoleListBoxFromID( UILIST_CONSOLE_EULA_LINKS ).GetSelectedIndex();


    if ( nIndex != -1 )
    {
        SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_SELECT_GENERIC ), cgtPSODefaults );

        ::Console::WebBrowser::NavigateTo(
            StringListFetch( arLinks[nIndex].UrlLabel ),
            nControllerIndex );
    }
}
void Close()
{
    SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ), cgtPSODefaults );

    ::Console::Screens::GetActionManager().ClearAllActions();
    ::Console::Screens::UpdateButtonLegendText();

    ::Console::Screens::CloseModalDialog( UIWND_CONSOLE_EULA );
}

BOOL ShouldShow()
{
    if ( CmdLineGetParams()->tBitArray.Test( CMDLINE_BLIZZCON_BIT ) )
    {
        return FALSE;
    }

    const int32 nAccepted = Prefs::GetEulaAccepted();



    return nAccepted != 0;
}

}  // namespace Console
}  // namespace UIEULA
