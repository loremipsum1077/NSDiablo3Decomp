// Original file: X/Client/UserInterface/Console/UIConsoleStackSplit.cpp
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "ConsoleInputUtilities.h"
#include "UIConsoleStackSplit.h"
#include "ConsoleScreenManager.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../../Console/ControllerUtils.h"
#include "../../Gfx/GfxVB.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Items.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"

// Declared here until their headers carry them (signatures from the DWARF).
int32 UI_GetPrimaryUIController();  // UI.cpp:538
BOOL UI_ValidateController( int32 nController );  // UI.cpp:556
void UI_SetControllerFocus( const UIID & idFocus );  // UI.cpp:599
namespace UIStackSplit {
namespace Console {
void Close();
void sUpdateForChangedAmounts();
void sChangeAmount( const int32 nDelta );


class UIStackSplitGlobals
{
public:
    UIStackSplitGlobals()
    {
        m_idACDStack = ACDID_INVALID;
        m_uStackSplitMin = 0;
        m_uStackSplitMax = 0;
        m_uCurrentVal = 0;
        m_nSpeed = 0;
        m_uStartValThisDirection = 0;
        m_pfnCallback = NULL;
        m_eStackSplitType = STACKSPLIT_GENERIC;
    }

    ACDID m_idACDStack;  // 0x00
    uint64 m_uStackSplitMin;  // 0x08
    uint64 m_uStackSplitMax;  // 0x10
    uint64 m_uCurrentVal;  // 0x18
    int32 m_nSpeed;  // 0x20
    uint64 m_uStartValThisDirection;  // 0x28
    UIStackSplitCallback m_pfnCallback;  // 0x30
    StackSplitType m_eStackSplitType;  // 0x38
    ::Console::AnalogStickInputFilter m_tStickInputFilter;  // 0x40
};












static UIStackSplitGlobals sgtUIStackSplitGlobals;







BOOL IsVisible()
{
    return UI_TestID( UIDLG_STACKSPLIT ) && UI_GetWindowFromID( UIDLG_STACKSPLIT ).IsActive();
}


void Close()
{
    ::Console::Screens::CloseModalDialog( UIDLG_STACKSPLIT );
}


















void Open( const ACDID idACDStack, const uint64 uInitialVal, const uint64 uStackMax, const UIStackSplitCallback pfnCallback, const StackSplitType eStackSplitType )
{




    if ( uStackMax == 0 )
    {
        return;
    }


    ::Console::Screens::ShowModalDialog( UI_GetPrimaryUIController(), UIDLG_STACKSPLIT, FALSE, GetUIIDNone() );

    SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_STACKSPLIT_OPEN ), cgtPSODefaults );

    sgtUIStackSplitGlobals.m_idACDStack = idACDStack;

    sgtUIStackSplitGlobals.m_uStackSplitMin = ( eStackSplitType == STACKSPLIT_GOLD_MAIL ) ? 0 : 1;
    sgtUIStackSplitGlobals.m_uStackSplitMax = uStackMax;
    sgtUIStackSplitGlobals.m_uCurrentVal = Math::Pin( uInitialVal, sgtUIStackSplitGlobals.m_uStackSplitMin, sgtUIStackSplitGlobals.m_uStackSplitMax );

    sgtUIStackSplitGlobals.m_pfnCallback = pfnCallback;
    sgtUIStackSplitGlobals.m_eStackSplitType = eStackSplitType;

    sgtUIStackSplitGlobals.m_tStickInputFilter = ::Console::GetDefaultStickInputFilter( ::Console::FILTER_HORIZONTAL );
    sgtUIStackSplitGlobals.m_tStickInputFilter.SetMinimumInputDelay( 250 );
    sgtUIStackSplitGlobals.m_tStickInputFilter.SetDeadZone( 0.9f );

    sgtUIStackSplitGlobals.m_nSpeed = 0;
    sgtUIStackSplitGlobals.m_uStartValThisDirection = 0;

    UI_SetControllerFocus( GetUIIDNone() );

    switch ( eStackSplitType )
    {
    case STACKSPLIT_GOLD_TRADE:
        UI_GetTextFromID( UITEXT_STACKSPLIT_TITLE ).SetText( StringListFetch( "ConsoleTrade:TradeGold" ), FALSE );
        break;
    case STACKSPLIT_GOLD_MAIL:
        UI_GetTextFromID( UITEXT_STACKSPLIT_TITLE ).SetText( StringListFetch( "ConsoleTrade:HowMuch" ), FALSE );
        break;




    default:
        UI_GetTextFromID( UITEXT_STACKSPLIT_TITLE ).SetText( StringListFetch( "ConsoleTrade:HowMany" ), FALSE );
        break;
    }

    sUpdateForChangedAmounts();
}






void OnAccept( const UIID & id, const int32 nControllerIndex )
{
    if ( !UI_ValidateController( nControllerIndex ) )
    {
        return;
    }

    Close();

    if ( sgtUIStackSplitGlobals.m_pfnCallback != NULL )
    {
        sgtUIStackSplitGlobals.m_pfnCallback( sgtUIStackSplitGlobals.m_idACDStack, sgtUIStackSplitGlobals.m_uCurrentVal );
    }
}






void OnCancel( const UIID & id, const int32 nControllerIndex )
{
    if ( !UI_ValidateController( nControllerIndex ) )
    {
        return;
    }

    SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_STACKSPLIT_CANCEL ), cgtPSODefaults );

    Close();
}






void sUpdateForChangedAmounts()
{
    CRefString sAmount;
    if ( sgtUIStackSplitGlobals.m_eStackSplitType == STACKSPLIT_GOLD_TRADE || sgtUIStackSplitGlobals.m_eStackSplitType == STACKSPLIT_GOLD_MAIL )
        sAmount = UIText_ValueToGameCurrency( sgtUIStackSplitGlobals.m_uCurrentVal, 0 );
    else
        sAmount = UnicodeText::FormatNumber( (int64)sgtUIStackSplitGlobals.m_uCurrentVal, TRUE, FNS_NORMAL, 0 );



    UI_GetTextFromID( UITEXT_STACKSPLIT_AMOUNT ).SetText( sAmount, FALSE );


    const BOOL bCanIncrease = sgtUIStackSplitGlobals.m_uCurrentVal < sgtUIStackSplitGlobals.m_uStackSplitMax;
    const BOOL bCanDecrease = sgtUIStackSplitGlobals.m_uCurrentVal > sgtUIStackSplitGlobals.m_uStackSplitMin;


    UI_GetWindowFromID( UIWND_STACKSPLIT_ARROW_UP ).SetActive( bCanIncrease );
    UI_GetWindowFromID( UIWND_STACKSPLIT_ARROW_DOWN ).SetActive( bCanDecrease );
}






void sChangeAmount( const int32 nDelta )
{
    uint64 uNewVal;

    if ( nDelta < 0 )
    {
        if ( sgtUIStackSplitGlobals.m_uCurrentVal < sgtUIStackSplitGlobals.m_uStackSplitMin - nDelta )
        {
            uNewVal = sgtUIStackSplitGlobals.m_uStackSplitMin;
        }
        else
        {
            uNewVal = sgtUIStackSplitGlobals.m_uCurrentVal + nDelta;
        }
    }
    else
    {
        uNewVal = Math::Min( sgtUIStackSplitGlobals.m_uCurrentVal + nDelta, sgtUIStackSplitGlobals.m_uStackSplitMax );
    }
    if ( uNewVal != sgtUIStackSplitGlobals.m_uCurrentVal )
    {
        sgtUIStackSplitGlobals.m_uCurrentVal = uNewVal;

        sUpdateForChangedAmounts();
    }
}


static void sAdjustSpeed( const BOOL fUp, const uint32 dwCurTime )
{

    if ( !fUp && sgtUIStackSplitGlobals.m_nSpeed >= 0 )
    {
        SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_STACKSPLIT_ADJUST ), cgtPSODefaults );


        sgtUIStackSplitGlobals.m_nSpeed = -1;
        sgtUIStackSplitGlobals.m_uStartValThisDirection = sgtUIStackSplitGlobals.m_uCurrentVal;
    }


    else if ( fUp && sgtUIStackSplitGlobals.m_nSpeed <= 0 )
    {
        SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_STACKSPLIT_ADJUST ), cgtPSODefaults );

        sgtUIStackSplitGlobals.m_nSpeed = 1;
        sgtUIStackSplitGlobals.m_uStartValThisDirection = sgtUIStackSplitGlobals.m_uCurrentVal;
    }
    else
    {









        const uint64 uTravelled = ( sgtUIStackSplitGlobals.m_nSpeed > 0 ) ? sgtUIStackSplitGlobals.m_uCurrentVal - sgtUIStackSplitGlobals.m_uStartValThisDirection : sgtUIStackSplitGlobals.m_uStartValThisDirection - sgtUIStackSplitGlobals.m_uCurrentVal;
        if ( sgtUIStackSplitGlobals.m_uCurrentVal % ( ( sgtUIStackSplitGlobals.m_nSpeed < 0 ? -sgtUIStackSplitGlobals.m_nSpeed : sgtUIStackSplitGlobals.m_nSpeed ) * 10 ) == 0 && uTravelled >= 5 )
        {

            sgtUIStackSplitGlobals.m_nSpeed *= 10;
        }
    }
}


void OnLeftStick( const UIID & id, const int32 nControllerIndex )
{

    if ( !UI_ValidateController( nControllerIndex ) )
    {
        return;
    }
    if ( ::Console::ControllerUtils::IsButtonDown( nControllerIndex, 10 ) || ::Console::ControllerUtils::IsButtonDown( nControllerIndex, 11 ) )
    {
        return;
    }

    const ::Console::AnalogStickInputFilter::Rule::ID idRule = sgtUIStackSplitGlobals.m_tStickInputFilter.HandleInput(
        ::Console::ControllerUtils::GetLStickPosition( nControllerIndex, FALSE ), FALSE );


    // dwCurTime: the argument is unused by sAdjustSpeed and was dropped from the call; 0 is a placeholder.
    const uint32 dwCurTime = 0;

    switch ( idRule )
    {
    case ::Console::RULE_STICK_LEFT:
        sAdjustSpeed( FALSE, dwCurTime );
        sChangeAmount( sgtUIStackSplitGlobals.m_nSpeed );
        break;

    case ::Console::RULE_STICK_RIGHT:
        sAdjustSpeed( TRUE, dwCurTime );
        sChangeAmount( sgtUIStackSplitGlobals.m_nSpeed );
        break;


    case ::Console::AnalogStickInputFilter::Rule::IN_DEAD_ZONE:
        sgtUIStackSplitGlobals.m_nSpeed = 0;
        sgtUIStackSplitGlobals.m_uStartValThisDirection = 0;
        break;
    }
}




void OnLeftDPad( const UIID & id, const int32 nControllerIndex )
{

    sAdjustSpeed( FALSE, 0 );


    sChangeAmount( sgtUIStackSplitGlobals.m_nSpeed );
}


void OnRightDPad( const UIID & id, const int32 nControllerIndex )
{

    sAdjustSpeed( TRUE, 0 );


    sChangeAmount( sgtUIStackSplitGlobals.m_nSpeed );
}

}  // namespace Console
}  // namespace UIStackSplit
