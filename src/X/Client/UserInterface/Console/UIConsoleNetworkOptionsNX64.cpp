// Original file: X/Client/UserInterface/Console/UIConsoleNetworkOptionsNX64.cpp
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "ConsoleModalNotificationManager.h"
#include "UIConsoleMainMenu.h"
#include "UIConsolePartyDisplay.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/Online.h"
#include "ConsoleScreenManager.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/OnlineService/Callback/Callback.h"
#include "../../../Sigma/RefString.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Types.h"
// Declared here until their headers carry them (signatures from the DWARF).
void UI_SetControllerFocus( const UIID & idFocus );  // UI.cpp:599
namespace Console { namespace Online { BOOL PartySwitchWillForceDisconnect( PartyOnlineState eState ); void PartySwitchOnlineState( PartyOnlineState eState ); } }  // Online.cpp:4709, 4738
namespace UIUtils { namespace Console { CRefString DisplayPartyOnlineStateIcon( ::Console::Online::PartyOnlineState eState, BOOL bEnabled, float flScale ); } }  // UIConsoleUtils.cpp:246
namespace UIPartyDisplay { namespace Console { void RefreshFriendsDisplay(); } }  // UIConsolePartyDisplay.cpp:703
namespace UINetworkOptions {
namespace Console {
void CloseNetworkOptions();
void OnSwitchWarningAccept( int32 nControllerIndex, ::Console::Online::PartyOnlineState eNewState, const UIID & id );
enum NetworkState
{
    NetworkState_Online,
    NetworkState_LocalCommunication,
    NetworkState_Num,
};



class NetworkOptionsDataSource : public ConsoleListBoxDataSource
{
public:
    NetworkOptionsDataSource()
    {
    }




    virtual int32 GetNumItems() const
    {
        return NetworkState_Num;
    }



    virtual UIControlSourceKey GetSourceKeyForItem( const int32 nIndex ) const
    {
        return UIControlSourceKey::Parse( "ConsoleNetworkOptions:ConsoleNetworkOptions_main.menuItem", SNO_NONE, NULL, NULL );
    }




    virtual void InitializeItemUI( const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex )
    {
    }








    virtual void UpdateItemUI( const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex )
    {



        const UIID idText = UIIDCreateFromParent( idItemUI, ".text" );
        CRefString sIcon, sText;

        switch ( nIndex )
        {
        case NetworkState_Online:
            {
                sIcon = UIUtils::Console::DisplayPartyOnlineStateIcon( ::Console::Online::PARTY_ONLINE_PUBLIC, IsItemEnabled( tListBox, nIndex ), 2.0f );
                sText = StringListFetch( "Console:Online" );
            }
            break;
        case NetworkState_LocalCommunication:
            {
                sIcon = UIUtils::Console::DisplayPartyOnlineStateIcon( ::Console::Online::PARTY_ADHOC, IsItemEnabled( tListBox, nIndex ), 2.0f );
                sText = StringListFetch( "Console:PartyLocalCommunicationNX64" );
            }
            break;
        }





        CRefString sLabel( CRefString().Format( "%s %s", sIcon.str(), sText.str() ) );

        UIText & tText = UI_GetTextFromID( idText );
        tText.SetText( sLabel, FALSE );


        tText.SetTextColor( tListBox.GetSelectedIndex() == nIndex ? crgbaWhite : crgbaGold );
    }
};

struct UINetworkOptionsData
{
    NetworkOptionsDataSource tNetworkListSource;  // 0x00
    int32 nOpeningControllerIndex;  // 0x08
    ModalNotificationId idBlockingDialog;  // 0x0c









    UINetworkOptionsData() : nOpeningControllerIndex( -1 ), idBlockingDialog( INVALID_MODAL_NOTIFICATION_ID )
    {
    }
};

UINetworkOptionsData sgtNetworkOptionsData;















void ShowNetworkOptions( int32 nControllerIndex )
{
    sgtNetworkOptionsData.nOpeningControllerIndex = nControllerIndex;
    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID( UILIST_CONSOLE_NETWORK_OPTIONS );
    tListBox.SetDataSource( sgtNetworkOptionsData.tNetworkListSource );
    tListBox.RefreshVisibleItems();
    const ::Console::Online::PartyOnlineState eState = ::Console::Online::PartyGetOnlineState();

    int32 nSelectedIndex = NetworkState_Online;
    if ( eState == ::Console::Online::PARTY_ADHOC )
    {
        nSelectedIndex = NetworkState_LocalCommunication;
    }







    tListBox.SetSelectedIndex( nSelectedIndex );





    UI_GetTextFromID( UIIDCreateFromParent( UIWND_CONSOLE_NETWORK_OPTIONS, ".Buttons.ButtonList.OkButton" ) ).ResizeToFitText();
    UI_GetTextFromID( UIIDCreateFromParent( UIWND_CONSOLE_NETWORK_OPTIONS, ".Buttons.ButtonList.Cancel" ) ).ResizeToFitText();

    ::Console::Screens::ShowModalDialog( ::Console::GamerProfile::GetPrimaryProfileUserIndex(), UIWND_CONSOLE_NETWORK_OPTIONS, TRUE,
        GetUIIDNone() );

    UI_SetControllerFocus( UILIST_CONSOLE_NETWORK_OPTIONS );
}


static void sCloseDialog()
{

    if ( UI_GetWindowFromID( UIWND_CONSOLE_NETWORK_OPTIONS ).IsActive() )
    {
        ::Console::Screens::CloseModalDialog( UIWND_CONSOLE_NETWORK_OPTIONS );
    }
}



void CloseNetworkOptions()
{
    sgtNetworkOptionsData.nOpeningControllerIndex = -1;

    if ( sgtNetworkOptionsData.idBlockingDialog != INVALID_MODAL_NOTIFICATION_ID )
    {
        ::Console::Screens::GetModalNotificationManager().RemoveNotification( sgtNetworkOptionsData.idBlockingDialog );
        sgtNetworkOptionsData.idBlockingDialog = INVALID_MODAL_NOTIFICATION_ID;
    }

    sCloseDialog();
}




void OnNetworkOptionsCancel( const UIID & id, const int32 nControllerIndex )
{
    if ( sgtNetworkOptionsData.nOpeningControllerIndex != nControllerIndex )
    {
        return;
    }

    SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ), cgtPSODefaults );

    CloseNetworkOptions();
}




void OnSwitchWarningAccept( int32 nControllerIndex, ::Console::Online::PartyOnlineState eNewState, const UIID & id )
{
    if ( sgtNetworkOptionsData.nOpeningControllerIndex != nControllerIndex )
    {
        return;
    }

    sCloseDialog();

    ::Console::Online::PartySwitchOnlineState( eNewState );

    if ( GameIsStartUp() )
    {
        UIMainMenu::Console::RefreshMainMenu( FALSE );
    }

    UIPartyDisplay::Console::RefreshPartyDisplay();
    UIPartyDisplay::Console::RefreshFriendsDisplay();
}








void OnNetworkOptionsAccept( const UIID & id, const int32 nControllerIndex )
{

    if ( sgtNetworkOptionsData.nOpeningControllerIndex != nControllerIndex )
    {
        return;
    }


    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID( UILIST_CONSOLE_NETWORK_OPTIONS );
    if ( tListBox.GetSelectedIndex() != -1 && ::Console::Online::PartyIsInParty() )
    {























        const ::Console::Online::PartyOnlineState eNewState = tListBox.GetSelectedIndex();
        const BOOL bWasLocal = ::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ADHOC;

        if ( ( eNewState == ::Console::Online::PARTY_ADHOC ) == bWasLocal )
        {
            SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_SELECT_GENERIC ), cgtPSODefaults );
        }
        else
        {





            if ( ::Console::Online::PartySwitchWillForceDisconnect( eNewState ) )
            {
                NotificationCreationParams tParams;
                tParams.nControllerIndex = nControllerIndex;
                tParams.uszMessage = StringListFetch( "ConsoleErrors:OnlineStateErrorWillDisconnect" );
                tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_3<void, int32, ::Console::Online::PartyOnlineState, const UIID &>::bind<&OnSwitchWarningAccept>( nControllerIndex, eNewState );

                ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification( tParams );
                return;
            }

            SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_SELECT_GENERIC ), cgtPSODefaults );

            sCloseDialog();




            ::Console::Online::PartySwitchOnlineState( eNewState );

            if ( GameIsStartUp() )
            {
                UIMainMenu::Console::RefreshMainMenu( FALSE );
            }

            UIPartyDisplay::Console::RefreshPartyDisplay();
            UIPartyDisplay::Console::RefreshFriendsDisplay();
            return;
        }
    }





    sCloseDialog();
}

}  // namespace Console
}  // namespace UINetworkOptions
