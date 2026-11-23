// Original file: X/Client/UserInterface/Controls/UIRadioButtonGroupControl.cpp
#include "UIRadioButtonGroupControl.h"
#include "UIRadioButtonControl.h"
#include "../../Console/ControllerUtils.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../Input/Input.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "../../../Sigma/Math/MathGeneral.h"

// Declared here until their headers carry them (signatures from the DWARF).
const UIID & UI_GetControllerFocus();  // UI.cpp:429
BOOL UI_ValidateController( int32 nController );  // UI.cpp:556
UIRadioButton & UI_GetRadioButtonFromID( const UIID & id );  // UI.cpp:5703
BOOL InputIsController( const BufferedInput & tInput );  // Input.cpp:1015







RadioButtonGroupCreationParams::RadioButtonGroupCreationParams( const UIType eType, const UIID & id, const UIID & idParent )
    : ControlCreationParams( eType, id, idParent )
{
    sInit();
}

















void RadioButtonGroupCreationParams::Load( const SNO snoUI, const UIControlDataExt * pControlData )
{
    ControlCreationParams::Load( snoUI, pControlData );

    pfnOnSelectionChanged = UIFunction_Lookup( pControlData->GetAttributeValue( ATT_ONSELCHANGE ) );
    nStickSpeedLimitMS = pControlData->GetInt32Value( ATT_STICK_SPEED_LIMIT );
    nControlStyle = Math::Pin( pControlData->GetInt32Value( ATT_STYLE ), (int32)RADIO_BUTTON_CONTROLLER_VERTICAL, (int32)RADIO_BUTTON_CONTROLLER_HORIZONTAL );
}








inline void RadioButtonGroupCreationParams::sInit()
{
    pfnOnSelectionChanged = NULL;
    nStickSpeedLimitMS = 200;
    nControlStyle = RADIO_BUTTON_CONTROLLER_VERTICAL;
}








UIRadioButtonGroup::UIRadioButtonGroup( const RadioButtonGroupCreationParams & tParams )
    : UIControlGroup( tParams )
    , m_nInputSpeedLimitMS( 0 )
    , m_fShouldForceSelChange( FALSE )
{
    m_pfnOnSelectionChanged = tParams.pfnOnSelectionChanged;
    m_nInputSpeedLimitMS = tParams.nStickSpeedLimitMS;
    m_eInputStyle = tParams.nControlStyle;

    switch ( m_eInputStyle )
    {
    case RADIO_BUTTON_CONTROLLER_VERTICAL:
        m_tStickInputFilter = Console::GetDefaultStickInputFilter( Console::FILTER_VERTICAL );
        break;
    case RADIO_BUTTON_CONTROLLER_HORIZONTAL:
        m_tStickInputFilter = Console::GetDefaultStickInputFilter( Console::FILTER_HORIZONTAL );
        break;
    }

    m_tStickInputFilter.SetMinimumInputDelay( m_nInputSpeedLimitMS );
    SetWrapSelection( TRUE );
}









BOOL UIRadioButtonGroup::HandleMessage( const UIMsg & msg, const uintptr dwParam )
{
    if ( msg == UI_INPUT )
    {
        const BufferedInput & tInput = *(const BufferedInput *)dwParam;
        if ( !InputIsController( tInput ) || UI_ValidateController( tInput.tBufControllerData.nControllerIndex ) )
        {
            if ( HandleInput( tInput ) )
            {
                return TRUE;
            }
        }
    }
    return UIControlGroup::HandleMessage( msg, dwParam );
}









void UIRadioButtonGroup::SetSelection( int32 nValue, const BOOL fForceSelChange )
{
    int32 nOldValue = GetSelectedValue();

    for ( uint32 i = 0; i < GetChildList().size(); i++ )
    {
        UIID idChild = GetChildList()[i].id;
        UIRadioButton & tButton = UI_GetRadioButtonFromID( idChild );
        if ( tButton.GetValue() == nValue )
            tButton.SetSelected( TRUE );
        else
            tButton.SetSelected( FALSE );
    }

    if ( fForceSelChange || nOldValue != nValue )
    {
        if ( m_pfnOnSelectionChanged != NULL )
        {
            m_pfnOnSelectionChanged( GetID() );
        }
    }
}










void UIRadioButtonGroup::SetSelection( const UIID & idControl )
{
    UIRadioButton & tButton = UI_GetRadioButtonFromID( idControl );
    SetSelection( tButton.GetValue(), m_fShouldForceSelChange );
}






















inline void UIRadioButtonGroup::SelectFirstEnabledButton()
{
    for ( uint32 i = 0; i < GetChildList().size(); i++ )
    {
        UIID idChild = GetChildList()[i].id;
        UIRadioButton & tButton = UI_GetRadioButtonFromID( idChild );
        if ( tButton.IsEnabled() )
        {
            SetSelection( tButton.GetValue(), FALSE );
            return;
        }
    }
}




void UIRadioButtonGroup::SelectNextEnabledButton()
{
    const BOOL bWrap = m_bWrapSelection;
    BOOL bFoundSelected = FALSE;
    for ( uint32 i = 0; i < GetChildList().size(); i++ )
    {
        UIID idChild = GetChildList()[i].id;
        UIRadioButton & tButton = UI_GetRadioButtonFromID( idChild );
        if ( tButton.IsEnabled() && bFoundSelected )
        {
            SetSelection( tButton.GetValue(), FALSE );
            return;
        }
        if ( tButton.IsSelected() )
        {
            bFoundSelected = TRUE;
        }
    }

    if ( bWrap || GetSelectedValue() == -1 )
    {
        SelectFirstEnabledButton();
    }
}










void UIRadioButtonGroup::SelectPreviousEnabledButton()
{
    const BOOL bWrap = m_bWrapSelection;
    BOOL bFoundSelected = FALSE;
    int32 nLastEnabled = -1;
    for ( int32 i = (int32)GetChildList().size() - 1; i >= 0; i-- )
    {
        UIID idChild = GetChildList()[i].id;
        UIRadioButton & tButton = UI_GetRadioButtonFromID( idChild );
        if ( tButton.IsEnabled() )
        {
            if ( nLastEnabled == -1 )
            {
                nLastEnabled = i;
            }
            if ( bFoundSelected )
            {
                SetSelection( tButton.GetValue(), FALSE );
                return;
            }
        }
        if ( tButton.IsSelected() )
        {
            bFoundSelected = TRUE;
        }
    }

    if ( nLastEnabled != -1 )
    {
        if ( bWrap || GetSelectedValue() == -1 )
        {
            SetSelection( UI_GetRadioButtonFromID( GetChildList()[nLastEnabled].id ).GetValue(), m_fShouldForceSelChange );
        }
    }
}






UIRadioButton * UIRadioButtonGroup::GetButtonByValue( int32 nValue ) const
{
    for ( uint32 i = 0; i < GetChildList().size(); i++ )
    {
        UIID idChild = GetChildList()[i].id;
        UIRadioButton & tButton = UI_GetRadioButtonFromID( idChild );
        if ( tButton.GetValue() == nValue )
        {
            return &tButton;
        }
    }
    return NULL;
}





int32 UIRadioButtonGroup::GetSelectedValue() const
{
    for ( uint32 i = 0; i < GetChildList().size(); i++ )
    {
        UIID idChild = GetChildList()[i].id;
        UIRadioButton & tButton = UI_GetRadioButtonFromID( idChild );
        if ( tButton.IsSelected() )
        {
            return tButton.GetValue();
        }
    }
    return -1;
}





const UIID & UIRadioButtonGroup::GetSelection() const
{
    for ( uint32 i = 0; i < GetChildList().size(); i++ )
    {
        const UIID & idChild = GetChildList()[i].id;
        UIRadioButton & tButton = UI_GetRadioButtonFromID( idChild );
        if ( tButton.IsSelected() )
        {
            return idChild;
        }
    }
    return GetUIIDNone();
}



























void UIRadioButtonGroup::OnChildAdded( const UIID & idChild )
{
    if ( UI_TestID( idChild ) && UI_GetControlFromID( idChild ).IsType( UIT_RADIOBUTTON ) )
    {
        UIRadioButton & tButton = UI_GetRadioButtonFromID( idChild );
        tButton.m_idRadioButtonGroup = GetID();
    }
}






inline BOOL UIRadioButtonGroup::HandleInput( const BufferedInput & tInput )
{
    switch ( tInput.eInputMsg )
    {
    case INPUTMSG_LEFTSTICK:
        return HandleStickInput( tInput );
    case INPUTMSG_CONTROLLERBUTTONDOWN:
    case INPUTMSG_CONTROLLERBUTTONSTILLDOWN:
    case INPUTMSG_CONTROLLERBUTTONREPEAT:
    case INPUTMSG_CONTROLLERBUTTONUP:
        switch ( tInput.tBufControllerData.eInputButton )
        {
        case 8:
        case 9:
        case 10:
        case 11:
            return HandleDPadInput( tInput );
        }
        break;
    }
    return FALSE;
}













BOOL UIRadioButtonGroup::HandleStickInput( const BufferedInput & tInput )
{
    BOOL bHasControllerFocus = UI_GetControllerFocus() == GetID();
    BOOL bInDeadZone = Console::ControllerUtils::LStickIsInDeadZone( tInput.tBufControllerData.nControllerIndex, FALSE );
    if ( bHasControllerFocus && !bInDeadZone )
    {
        Vector2D vecStick = Console::ControllerUtils::GetLStickPosition( tInput.tBufControllerData.nControllerIndex, FALSE );
        Console::AnalogStickInputFilter::Rule::ID idRule = m_tStickInputFilter.HandleInput( vecStick, FALSE );
        if ( idRule == Console::RULE_STICK_DOWN || idRule == Console::RULE_STICK_RIGHT )
        {
            SelectNextEnabledButton();
            return TRUE;
        }
        else if ( idRule == Console::RULE_STICK_UP || idRule == Console::RULE_STICK_LEFT )
        {
            SelectPreviousEnabledButton();
            return TRUE;
        }
        else if ( idRule == Console::AnalogStickInputFilter::Rule::IGNORE_INPUT )
        {
            idRule = m_tStickInputFilter.HandleInput( vecStick, TRUE );
            if ( m_eInputStyle == RADIO_BUTTON_CONTROLLER_VERTICAL && ( idRule == Console::RULE_STICK_UP || idRule == Console::RULE_STICK_DOWN ) )
            {
                return TRUE;
            }
            if ( m_eInputStyle == RADIO_BUTTON_CONTROLLER_HORIZONTAL && ( idRule == Console::RULE_STICK_LEFT || idRule == Console::RULE_STICK_RIGHT ) )
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}












BOOL UIRadioButtonGroup::HandleDPadInput( const BufferedInput & tInput )
{
    static Console::InputSpeedLimiter tSpeedLimiter;
    static InputButton eLastButton = -1;

    if ( tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONUP )
    {
        tSpeedLimiter.ResetLastInputTime();
        return FALSE;
    }

    BOOL bIgnoreInput = !tSpeedLimiter.HasWaitTimeExpired( m_nInputSpeedLimitMS );
    BOOL bHasControllerFocus = UI_GetControllerFocus() == GetID();
    BOOL bValidButton = FALSE;
    if ( bHasControllerFocus )
    {
        switch ( m_eInputStyle )
        {
        case RADIO_BUTTON_CONTROLLER_VERTICAL:
            bValidButton = tInput.tBufControllerData.eInputButton >= 8 && tInput.tBufControllerData.eInputButton <= 9;
            break;
        case RADIO_BUTTON_CONTROLLER_HORIZONTAL:
            bValidButton = tInput.tBufControllerData.eInputButton == 10 || tInput.tBufControllerData.eInputButton == 11;
            break;
        }

        BOOL bButtonChanged = eLastButton != tInput.tBufControllerData.eInputButton;
        eLastButton = tInput.tBufControllerData.eInputButton;
        if ( !bIgnoreInput || bButtonChanged )
        {
            if ( bValidButton )
            {
                switch ( tInput.tBufControllerData.eInputButton )
                {
                case 8:
                case 10:
                    SelectPreviousEnabledButton();
                    break;
                case 9:
                case 11:
                    SelectNextEnabledButton();
                    break;
                }
            }
            tSpeedLimiter.UpdateLastInputTime();
            return TRUE;
        }
    }
    return bHasControllerFocus && bValidButton;
}
