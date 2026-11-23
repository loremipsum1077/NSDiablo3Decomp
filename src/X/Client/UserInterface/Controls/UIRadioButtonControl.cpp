// Original file: X/Client/UserInterface/Controls/UIRadioButtonControl.cpp
#include "UIRadioButtonControl.h"
#include "UIRadioButtonGroupControl.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../Input/Input.h"
#include "../UI.h"
#include "../UIAttributes.h"

// Declared here until their headers carry them (signatures from the DWARF).
UIMsgFunctionPtr UIMsgFunction_Lookup( LPCSTR szFunction );  // UIFunctions.cpp:3137
const UIID & UI_GetCapture();  // UI.cpp
UIID UIIDCreateFromControlAttribute( const UIControlSourceKey & tSrcKey, const UIID & id, const UIAttributeType eAttributeType );












inline void RadioButtonCreationParams::sInit()
{
    fDoActionOnMouseDown = TRUE;
    pfnClickPredicate = NULL;
    nValue = 0;
    bDefault = FALSE;
    for ( int32 i = 0; i < BS_MAX; i++ )
    {
        tUnselectedTextureFrames[i] = cgtImageTextureFrameNull;
        tSelectedTextureFrames[i] = cgtImageTextureFrameNull;
    }
    idGroup = GetUIIDNone();
}





RadioButtonCreationParams::RadioButtonCreationParams( const UIType eType, const UIID & id, const UIID & idParent )
    : ButtonCreationParams( eType, id, idParent )
{
    sInit();
}



















void RadioButtonCreationParams::Load( const SNO snoUI, const UIControlDataExt * pControlData )
{
    ButtonCreationParams::Load( snoUI, pControlData );

    nValue = pControlData->GetInt32Value( ATT_RADIOBUTTON_VALUE );
    bDefault = pControlData->GetBoolValue( ATT_RADIOBUTTON_DEFAULT );
    pfnClickPredicate = UIMsgFunction_Lookup( pControlData->GetAttributeValue( ATT_CLICKPREDICATE ) );

    pControlData->GetImageAttribute( ATT_IMAGEFRAME, tUnselectedTextureFrames[BS_NORMAL] );
    pControlData->GetImageAttribute( ATT_RADIOBUTTON_IMAGEFRAME, tSelectedTextureFrames[BS_NORMAL] );
    pControlData->GetImageAttribute( ATT_IMAGEFRAME_MOUSEOVER, tUnselectedTextureFrames[BS_MOUSEOVER] );
    pControlData->GetImageAttribute( ATT_RADIOBUTTON_IMAGEFRAME_MOUSEOVER, tSelectedTextureFrames[BS_MOUSEOVER] );
    pControlData->GetImageAttribute( ATT_IMAGEFRAME_PRESSED, tUnselectedTextureFrames[BS_PUSHED] );
    pControlData->GetImageAttribute( ATT_RADIOBUTTON_IMAGEFRAME_PRESSED, tSelectedTextureFrames[BS_PUSHED] );

    if ( !pControlData->AttributeExists( ATT_BUTTON_DO_ACTION_ON_MOUSEDOWN ) )
    {
        fDoActionOnMouseDown = TRUE;
    }
}




void UIRadioButton::sInit( const RadioButtonCreationParams & tParams )
{
    for ( int32 i = 0; i < BS_MAX; i++ )
    {
        m_tUnselectedTextureFrames[i] = tParams.tUnselectedTextureFrames[i];
        m_tSelectedTextureFrames[i] = tParams.tSelectedTextureFrames[i];
    }

    if ( tParams.fDoActionOnMouseDown )
    {
        m_tUnselectedTextureFrames[BS_PUSHED] = m_tUnselectedTextureFrames[BS_MOUSEOVER];
        m_tSelectedTextureFrames[BS_PUSHED] = m_tSelectedTextureFrames[BS_NORMAL];
        m_tSelectedTextureFrames[BS_MOUSEOVER] = m_tSelectedTextureFrames[BS_NORMAL];
    }

    m_tMouseOverIconTextureFrame = tParams.tIconTextureFrame[BS_MOUSEOVER];
    m_nValue = tParams.nValue;
    m_bDefault = tParams.bDefault;
    m_pfnClickPredicate = tParams.pfnClickPredicate;
    m_idRadioButtonGroup = tParams.idGroup;
    if ( m_idRadioButtonGroup == GetUIIDNone() )
    {
        m_idRadioButtonGroup = UIIDCreateFromControlAttribute( GetUISrcKey(), GetID(), ATT_ID_GENERIC_01 );
    }

    m_bSelected = FALSE;
    sUpdateButtonTextureFrames();
}


















UIRadioButton::UIRadioButton( const RadioButtonCreationParams & tParams, const Rect2D * pRect )
    : UIButton( tParams, pRect )
{
    sInit( tParams );
}


















void UIRadioButton::SetTextureFrames( const ImageTextureFrame & tTextureFrameNormal, const ImageTextureFrame & tTextureFrameMouseover, const ImageTextureFrame & tTextureFramePushed )
{
    m_tUnselectedTextureFrames[BS_NORMAL] = tTextureFrameNormal;
    m_tUnselectedTextureFrames[BS_MOUSEOVER] = tTextureFrameMouseover;
    m_tUnselectedTextureFrames[BS_PUSHED] = tTextureFramePushed;
    m_tSelectedTextureFrames[BS_NORMAL] = tTextureFramePushed;
    m_tSelectedTextureFrames[BS_MOUSEOVER] = tTextureFramePushed;
    m_tSelectedTextureFrames[BS_PUSHED] = tTextureFramePushed;
    sUpdateButtonTextureFrames();
}
































int32 UIRadioButton::GetValue() const
{
    return m_nValue;
}




BOOL UIRadioButton::IsSelected() const
{
    return m_bSelected;
}





void UIRadioButton::sUpdateButtonTextureFrames()
{
    if ( m_bSelected )
    {
        SetTextureFrame( BS_NORMAL, m_tSelectedTextureFrames[BS_NORMAL] );
        SetTextureFrame( BS_MOUSEOVER, m_tSelectedTextureFrames[BS_MOUSEOVER] );
        SetTextureFrame( BS_PUSHED, m_tSelectedTextureFrames[BS_PUSHED] );
        SetIconTextureFrame( BS_MOUSEOVER, m_tIconTextureFrame[BS_PUSHED] );
        if ( GetButtonState() == BS_NORMAL )
        {
            SetButtonState( BS_PUSHED, TRUE, FALSE );
        }
    }
    else
    {
        SetTextureFrame( BS_NORMAL, m_tUnselectedTextureFrames[BS_NORMAL] );
        SetTextureFrame( BS_MOUSEOVER, m_tUnselectedTextureFrames[BS_MOUSEOVER] );
        SetTextureFrame( BS_PUSHED, m_tUnselectedTextureFrames[BS_PUSHED] );
        SetIconTextureFrame( BS_MOUSEOVER, m_tMouseOverIconTextureFrame );
        if ( GetButtonState() == BS_PUSHED )
        {
            SetButtonState( BS_NORMAL, TRUE, FALSE );
        }
    }
}
















void UIRadioButton::SetSelected( const BOOL bSelected )
{
    if ( m_bSelected != bSelected )
    {
        m_bSelected = bSelected;
        sUpdateButtonTextureFrames();
    }
}








void UIRadioButton::SetButtonState( ButtonState eButtonState, const BOOL bForceUpdate, const BOOL bSuppressSounds )
{
    if ( m_bSelected && eButtonState == BS_NORMAL )
    {
        eButtonState = BS_PUSHED;
    }
    UIButton::SetButtonState( eButtonState, bForceUpdate, bSuppressSounds );
}









void UIRadioButton::PostCreate()
{
    UIButton::PostCreate();

    if ( UI_TestID( m_idRadioButtonGroup ) )
    {
        UIRadioButtonGroup & tGroup = UI_GetRadioButtonGroupFromID( m_idRadioButtonGroup );
        tGroup.AddChild( GetID() );
        if ( m_bDefault )
        {
            tGroup.SetSelection( GetID() );
        }
    }
    else if ( m_bDefault )
    {
        SetSelected( TRUE );
    }
}









void UIRadioButton::Draw( const uint8 bLayerAlpha, float & flZSlice )
{
    BOOL bSelected = IsSelected();
    BOOL fTextButtonOffset = GetButtonState() == BS_PUSHED || bSelected;

    EnableTextButtonPressOffset( fTextButtonOffset );
    sDraw( bLayerAlpha, flZSlice, fTextButtonOffset );
}





BOOL UIRadioButton::HandleMessage( const UIMsg & msg, const uintptr dwParam )
{
    if ( msg == UI_INPUT )
    {
        const BufferedInput & tInput = *(const BufferedInput *)dwParam;
        const UIID & id = GetID();
        if ( tInput.eInputMsg == INPUTMSG_LEFTMOUSEDOWN || tInput.eInputMsg == INPUTMSG_LEFTMOUSEDOUBLECLICK )
        {
            if ( UI_GetMousedOverControl() != id )
            {
                return FALSE;
            }
            if ( IsEnabled() )
            {
                if ( m_pfnClickPredicate != NULL && !m_pfnClickPredicate( id ) )
                {
                    return TRUE;
                }
                if ( m_fDoActionOnMouseDown )
                {
                    if ( UI_TestID( m_idRadioButtonGroup ) )
                    {
                        UI_GetRadioButtonGroupFromID( m_idRadioButtonGroup ).SetSelection( id );
                    }
                }
                if ( tInput.eInputMsg == INPUTMSG_LEFTMOUSEDOUBLECLICK )
                {
                    if ( m_pFunctionsPtr && m_pFunctionsPtr->pfnOnDoubleClick )
                    {
                        m_pFunctionsPtr->pfnOnDoubleClick( id );
                    }
                }
            }
        }
        else if ( !m_fDoActionOnMouseDown && tInput.eInputMsg == INPUTMSG_LEFTMOUSEUP )
        {
            if ( UI_GetCapture() != id )
            {
                return FALSE;
            }
            if ( IsEnabled() && UI_GetMousedOverControl() == id )
            {
                if ( m_pfnClickPredicate == NULL || m_pfnClickPredicate( id ) )
                {
                    if ( UI_TestID( m_idRadioButtonGroup ) )
                    {
                        UI_GetRadioButtonGroupFromID( m_idRadioButtonGroup ).SetSelection( id );
                    }
                }
            }
        }
    }
    return UIButton::HandleMessage( msg, dwParam );
}
