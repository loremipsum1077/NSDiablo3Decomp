// Original file: X/Client/UserInterface/Console/UIConsoleRiftComplete.cpp
#include "ConsoleInputUtilities.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "Wrappers/RadioSelectorWrapper.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../UIStoryboard.h"
#include "../../CGame.h"
#include "../../Console/ControllerUtils.h"
#include "../../Sound/Sound.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/NephalemRiftCommon.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/RefString.h"

// Declared here until their headers carry them (signatures from the DWARF).
UIStoryboard & UI_GetStoryboardFromID( const UIID & id );  // UI.cpp
namespace Console { namespace Screens { BOOL OpenToScreen( const ScreenSets eScreenSet, const ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition ); } }  // ConsoleScreenManager.cpp:1104
namespace UIRiftComplete {
void InitializeForGame(); void DisposeForGame(); void OpenForController( int32 nControllerIndex ); void Close(); void OnUpdate( const UIID & id );
void OnLStick( const UIID & id, const int32 nControllerIndex ); void OnDPadLeft( const UIID & id, const int32 nControllerIndex ); void OnDPadRight( const UIID & id, const int32 nControllerIndex );
void OnButton1( const UIID & id, const int32 nControllerIndex ); void OnButton2( const UIID & id, const int32 nControllerIndex );
}
static void sUpdateControllerActions(); static void sUpdateButtonTitles(); static void sUpdateButtonState(); static void sUpdateButtonDescription();
static void sOnSelectionChanged( const UIID & idChoice, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput );






static const int32 KEY_UPGRADE_INDEX = 0;
static const int32 GEM_UPGRADE_INDEX = 1;

enum ButtonActions
{
    ACTION_SELECT = 1,
    ACTION_CLOSE = 2,
};






























struct RiftCompleteGlobals
{
    RiftCompleteGlobals();

    int32 nControllerIndex;  // 0x00
    NephalemRift::RIFT_REWARD_CHOICE_STATE ePlayerChoiceState;  // 0x04
    Console::AnalogStickInputFilter tStickInputFilter;  // 0x08
    Console::RadioSelectorWrapper tRewardSelector;  // 0x30
};






RiftCompleteGlobals * sgptRiftCompleteGlobals = NULL;




RiftCompleteGlobals::RiftCompleteGlobals()
    : nControllerIndex( -1 )
    , ePlayerChoiceState( NephalemRift::RIFT_REWARD_CHOICE_INVALID )
{
    tStickInputFilter = Console::GetDefaultStickInputFilter( Console::FILTER_DIRECTIONAL );
}






static BOOL sIsActive()
{

    return UI_GetWindowFromID( UIDLG_RIFT_COMPLETE ).IsActive();
}




void UIRiftComplete::InitializeForGame()
{
    sgptRiftCompleteGlobals = new RiftCompleteGlobals();
}






void UIRiftComplete::DisposeForGame()
{
    delete sgptRiftCompleteGlobals;
    sgptRiftCompleteGlobals = NULL;
}
















void UIRiftComplete::OpenForController( int32 nControllerIndex )
{
    if ( sIsActive() )
    {
        return;
    }

    CancelAllChannelingAbilities();

    sgptRiftCompleteGlobals->nControllerIndex = nControllerIndex;

    sgptRiftCompleteGlobals->tRewardSelector.WrapUsingContainerControl( UIWND_RIFT_COMPLETE_CHOICES );
    sgptRiftCompleteGlobals->tRewardSelector.SetCallbacks( NULL, sOnSelectionChanged );
    sgptRiftCompleteGlobals->tRewardSelector.SetSelectedIndex( GEM_UPGRADE_INDEX, TRUE );

    Console::Screens::ShowModalDialog( nControllerIndex, UIDLG_RIFT_COMPLETE, FALSE, GetUIIDNone() );

    Console::Screens::GetActionManager().SetActionsDirtyFlag( TRUE );
}












void UIRiftComplete::Close()
{
    Console::Screens::CloseModalDialog( UIDLG_RIFT_COMPLETE );

    if ( sgptRiftCompleteGlobals->ePlayerChoiceState != NephalemRift::RIFT_GEM_UPGRADE )
    {
        Player * ptPlayer = PlayerGetByControllerIndex( sgptRiftCompleteGlobals->nControllerIndex );
        if ( ptPlayer != NULL )
        {
            ptPlayer->idACDActiveVendor = ACDID_INVALID;
        }
    }

    sgptRiftCompleteGlobals->ePlayerChoiceState = NephalemRift::RIFT_REWARD_CHOICE_INVALID;
}










void UIRiftComplete::OnUpdate( const UIID & id )
{
    if ( !sIsActive() )
        return;

    sUpdateButtonTitles();
    sUpdateButtonState();
    sUpdateButtonDescription();





    if ( Console::Screens::GetActionManager().AreActionsDirty() )
    {
        sUpdateControllerActions();
        Console::Screens::UpdateButtonLegendText();
        Console::Screens::GetActionManager().SetActionsDirtyFlag( FALSE );
    }
}




void UIRiftComplete::OnLStick( const UIID & id, const int32 nControllerIndex )
{
    if ( sgptRiftCompleteGlobals->nControllerIndex != nControllerIndex )
    {
        return;
    }

    const Vector2D vecStick = Console::ControllerUtils::GetLStickPosition( nControllerIndex, FALSE );
    const Console::AnalogStickInputFilter::Rule::ID idRule = sgptRiftCompleteGlobals->tStickInputFilter.HandleInput( vecStick, FALSE );

    switch ( idRule )
    {
    case Console::RULE_STICK_LEFT:
        sgptRiftCompleteGlobals->tRewardSelector.SelectPrevious( TRUE );
        break;
    case Console::RULE_STICK_RIGHT:
        sgptRiftCompleteGlobals->tRewardSelector.SelectNext( TRUE );
        break;
    }
}







void UIRiftComplete::OnDPadLeft( const UIID & id, const int32 nControllerIndex )
{
    if ( sgptRiftCompleteGlobals->nControllerIndex != nControllerIndex )
    {
        return;
    }

    sgptRiftCompleteGlobals->tRewardSelector.SelectPrevious( TRUE );
}






void UIRiftComplete::OnDPadRight( const UIID & id, const int32 nControllerIndex )
{
    if ( sgptRiftCompleteGlobals->nControllerIndex != nControllerIndex )
    {
        return;
    }

    sgptRiftCompleteGlobals->tRewardSelector.SelectNext( TRUE );
}






void UIRiftComplete::OnButton1( const UIID & id, const int32 nControllerIndex )
{
    if ( sgptRiftCompleteGlobals->nControllerIndex != nControllerIndex )
    {
        return;
    }

    if ( Console::Screens::GetActionManager().GetActionForInput( Console::CONTROLLER_INPUT_BUTTON_1 ) == ACTION_SELECT )
    {
        sgptRiftCompleteGlobals->ePlayerChoiceState = NephalemRift::RIFT_REWARD_CHOICE_INVALID;
        Console::Screens::OpenToScreen( Console::Screens::JEWEL_UPGRADE_SCREEN_SET, 0, nControllerIndex, TRUE );
    }
}












void UIRiftComplete::OnButton2( const UIID & id, const int32 nControllerIndex )
{
    if ( sgptRiftCompleteGlobals->nControllerIndex != nControllerIndex )
    {
        return;
    }

    Console::Screens::CloseScreenSet( TRUE, TRUE );
}






static void sUpdateControllerActions()
{
    Console::ControllerActionManager & tActionManager = Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();
    const int32 nSelectedIndex = sgptRiftCompleteGlobals->tRewardSelector.GetSelectedIndex();





    tActionManager.SetAction( Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch( "ConsoleUI:Close" ) );
}




static void sUpdateButtonTitles()
{

    const RGBAColor rgbaDefault = UI_GetTextFromID( UITEXT_RIFT_COMPLETE_CHOICE_DESCRIPTION ).GetTextColor();

    const int32 nSelectedIndex = sgptRiftCompleteGlobals->tRewardSelector.GetSelectedIndex();
    UI_GetTextFromID( UITEXT_RIFT_COMPLETE_KEY_TEXT ).SetTextColor( nSelectedIndex == KEY_UPGRADE_INDEX ? crgbaWhite : rgbaDefault );

    UI_GetTextFromID( UITEXT_RIFT_COMPLETE_GEM_TEXT ).SetTextColor( nSelectedIndex == GEM_UPGRADE_INDEX ? crgbaWhite : rgbaDefault );
}





static void sUpdateButtonState()
{



    UIWindow & tKey = UI_GetWindowFromID( UIWND_RIFT_COMPLETE_KEY );
    tKey.SetShader( GlobalSNOGet( GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE ) );


    UIWindow & tGem = UI_GetWindowFromID( UIWND_RIFT_COMPLETE_GEM );
    tGem.SetShader( GlobalSNOGet( GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE ) );
}




static void sUpdateButtonDescription()
{

    CRefString sDescription;
    const int32 nSelectedIndex = sgptRiftCompleteGlobals->tRewardSelector.GetSelectedIndex();
    switch ( nSelectedIndex )
    {
    case -1:
        sDescription = StringListFetch( "NephalemRift:TieredRiftRewardChoiceNone" );
        break;

    case KEY_UPGRADE_INDEX:
        sDescription = StringListFetch( "NephalemRift:TieredRiftRewardRewardChoiceNoKey" );
        break;
    case GEM_UPGRADE_INDEX:
    {
        Player * ptPlayer = PlayerGetByControllerIndex( sgptRiftCompleteGlobals->nControllerIndex );
        ActorCommonData * ptACD = ( ptPlayer != NULL ) ? ACDTryToGet( ptPlayer->idPlayerACD ) : NULL;
        if ( ptACD != NULL )
        {
            sDescription = StringListFetch( "NephalemRift:TieredRiftRewardChoiceGem" );
        }
        break;
    }
    }
















    UI_GetTextFromID( UITEXT_RIFT_COMPLETE_CHOICE_DESCRIPTION ).SetText( sDescription, FALSE );
}













static void sOnSelectionChanged( const UIID & idChoice, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput )
{
    if ( bTriggeredByInput )
    {
        SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_GENERIC_FOCUS_SWITCH ), cgtPSODefaults );
    }

    UI_GetWindowFromID( UIWND_RIFT_COMPLETE_KEY_HIGHLIGHT ).SetActive( bSelected && nIndex == KEY_UPGRADE_INDEX );
    UI_GetWindowFromID( UIWND_RIFT_COMPLETE_GEM_HIGHLIGHT ).SetActive( bSelected && nIndex == GEM_UPGRADE_INDEX );

    UIStoryboard & tStoryboard = UI_GetStoryboardFromID( UISTORYBOARD_RIFT_COMPLETE_HIGHLIGHT );
    if ( tStoryboard.IsPlaying() )
    {
        tStoryboard.Stop();
    }
    tStoryboard.Play( FALSE, 0.0f );

    Console::Screens::GetActionManager().SetActionsDirtyFlag( TRUE );
}
