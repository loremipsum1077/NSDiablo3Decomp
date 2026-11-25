// Original file: X/Client/UserInterface/Controls/UICheckBoxControl.cpp
// Generated from build/try/p2-uic/cb.cpp by gen.py (functions padded to their original lines).

#include "UICheckBoxControl.h"
#include "UITextControl.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../Input/Input.h"
#include "../../Sound/Sound.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "../UICreation.h"

// Declared here until their headers carry them (signatures from the DWARF).
UIMsgFunctionPtr UIMsgFunction_Lookup( LPCSTR szFunction );  // UIFunctions.cpp:3137
const UIID & UI_GetCapture();  // UI.cpp
void UI_SetCapture( const UIID & idCapture );  // UI.cpp
const UIID & UI_GetKeyboardFocus();  // UI.cpp:414
void UI_IncrementKeyboardFocus( const BOOL fBackwards );  // UI.cpp
BOOL InputShiftKeyIsDown( InputContext eContext );  // Input.cpp








void CheckBoxCreationParams::sInit()
{

    pfnClickPredicate = NULL;

    for ( int32 i = 0; i < CS_MAX; i++ )
    {
        tTextureFrames[i] = cgtImageTextureFrameNull;
        rgbaTextColors[i] = crgbaUIGold;
        rgbaDropShadowColors[i] = crgbaBlack;
    }

    snoFont = GlobalSNOGet( GLOBALSNO_FONT_EXOCETLIGHT );
    nFontSize = 14;
    sDecoratedLabel = "";
    eStyle = CS_TOGGLE_BUTTON;
    sTooltipLabel = "";

    rgbaTextDisabled = crgbaGray;
    rgbaDisabled = crgbaGray;

    rectTextPaddingUIC = Rect2D( 0.0f, 0.0f, 0.0f, 0.0f );
    nLinesBeforeTruncation = 0; fAcceptsFocus = TRUE;
    snoClickSound = SNO_NONE; snoMouseoverSound = SNO_NONE;
    eResizeToFit = RESIZETOFIT_DISABLED; fAllCaps = FALSE;
}





CheckBoxCreationParams::CheckBoxCreationParams( const UIType eType, const UIID & id, const UIID & idParent )

    : WindowCreationParams( eType, id, idParent )
{
    sInit();
}


















void CheckBoxCreationParams::Load( const SNO snoUI, const UIControlDataExt * pControlData )
{

    WindowCreationParams::Load( snoUI, pControlData );
    for ( int32 i = 0; i < CS_MAX; i++ )
    {
        pControlData->GetImageAttribute( ATT_IMAGEFRAME_CHECKED_NORMAL + i, tTextureFrames[i] );
    }

    snoFont = SNOFileLookupGUID( FONT_SNO, pControlData->GetAttributeValue( ATT_FONT ) );
    nFontSize = pControlData->GetInt32Value( ATT_FONT_SIZE );
    fAllCaps = pControlData->GetBoolValue( ATT_TEXT_ALL_CAPS );
    nLinesBeforeTruncation = pControlData->GetInt32Value( ATT_TEXT_LINES_BEFORE_TRUNCATION );

    fAcceptsFocus = pControlData->GetBoolValue( ATT_ACCEPTSFOCUS );

    sDecoratedLabel = pControlData->GetAttributeValue( ATT_TEXT );

    int32 nStyle = Math::Pin( pControlData->GetInt32Value( ATT_STYLE ), (int32)CS_STANDARD_BOX, (int32)CS_TOGGLE_BUTTON );
    if ( sDecoratedLabel.Length() == 0 )
        nStyle = CS_TOGGLE_BUTTON;
    eStyle = (CheckBoxStyle)nStyle;

    int32 resizeToFit = pControlData->GetInt32Value( ATT_RESIZE_TO_FIT );
    eResizeToFit = ( resizeToFit != -1 ) ? resizeToFit : RESIZETOFIT_DISABLED;
    if ( eResizeToFit > RESIZETOFIT_DISABLED )
        dwFlags |= 0x200;

    if ( eStyle == CS_STANDARD_BOX )
    {
        UIColor uicolor = pControlData->GetUint32Value( ATT_FOREGROUND );
        UIColor dropShadowColor = pControlData->GetUint32Value( ATT_DROPSHADOW );
        for ( int32 i = 0; i < CS_MAX; i++ )
        {
            rgbaTextColors[i].Set( uicolor );
            rgbaDropShadowColors[i].Set( dropShadowColor );
        }
    }
    else
    {
        UIColor uicolor = pControlData->GetUint32Value( ATT_FOREGROUND );
        rgbaTextColors[CS_UNCHECKED_NORMAL].Set( uicolor );
        uicolor = pControlData->GetUint32Value( ATT_DROPSHADOW );
        rgbaDropShadowColors[CS_UNCHECKED_NORMAL].Set( uicolor );
        uicolor = pControlData->GetUint32Value( ATT_FOREGROUND_HIGHLIGHT );
        rgbaTextColors[CS_UNCHECKED_MOUSEOVER].Set( uicolor );
        uicolor = pControlData->GetUint32Value( ATT_DROPSHADOW_HIGHLIGHT );
        rgbaDropShadowColors[CS_UNCHECKED_MOUSEOVER].Set( uicolor );
        uicolor = pControlData->GetUint32Value( ATT_FOREGROUND_PUSHED );
        rgbaTextColors[CS_CHECKED_NORMAL].Set( uicolor );
        rgbaTextColors[CS_CHECKED_MOUSEOVER].Set( uicolor );
        uicolor = pControlData->GetUint32Value( ATT_DROPSHADOW_PUSHED );
        rgbaDropShadowColors[CS_CHECKED_NORMAL].Set( uicolor );
        rgbaDropShadowColors[CS_CHECKED_MOUSEOVER].Set( uicolor );
    }

    float flPaddingL = pControlData->GetFloat32Value( ATT_TEXT_PADDING_L );
    float flPaddingR = pControlData->GetFloat32Value( ATT_TEXT_PADDING_R );
    float flPaddingT = pControlData->GetFloat32Value( ATT_TEXT_PADDING_T );
    float flPaddingB = pControlData->GetFloat32Value( ATT_TEXT_PADDING_B );
    rectTextPaddingUIC = Rect2D( flPaddingL, flPaddingT, flPaddingR, flPaddingB );

    UIColor uicolor = pControlData->GetUint32Value( ATT_FOREGROUND_DISABLED );
    rgbaTextDisabled.Set( uicolor );
    uicolor = pControlData->GetUint32Value( ATT_BUTTON_DISABLED_COLOR );
    rgbaDisabled.Set( uicolor );

    sTooltipLabel = pControlData->GetAttributeValue( ATT_TOOLTIP_TEXT );
    pfnClickPredicate = UIMsgFunction_Lookup( pControlData->GetAttributeValue( ATT_CLICKPREDICATE ) );
    snoClickSound = SNOFileLookupGUID( SOUND_SNO, pControlData->GetAttributeValue( ATT_CLICK_SOUND ) );
    snoMouseoverSound = SNOFileLookupGUID( SOUND_SNO, pControlData->GetAttributeValue( ATT_MOUSEOVER_SOUND ) );
}














void UICheckBox::sInit( const CheckBoxCreationParams & tParams, const Vector2D &, const Vector2D & vecDim )
{

    const UIID & id = GetID();
    SetEatsClicks( tParams.eEatsClicks );


    Vector2D vecBoxDim = ( tParams.eStyle == CS_STANDARD_BOX ) ? Vector2D( vecDim.y, vecDim.y ) : vecDim;














    m_bAcceptsFocus = tParams.fAcceptsFocus;
    m_rgbaDisabled = tParams.rgbaDisabled;
    for ( int32 i = 0; i < CS_MAX; i++ )
    {
        m_tTextureFrames[i] = tParams.tTextureFrames[i];
        m_rgbaTextColors[i] = tParams.rgbaTextColors[i];
        m_rgbaDropShadowColors[i] = tParams.rgbaDropShadowColors[i];
    }


    m_ptWndBox = NULL;
    if ( tParams.eStyle == CS_STANDARD_BOX )
    {
        WindowCreationParams tWndParams( UIT_WINDOW, "box", id );
        m_ptWndBox = &UI_GetWindowFromID( CreateControl<UIWindow, WindowCreationParams>( tWndParams, Vector2D( 0.0f, 0.0f ), vecBoxDim ) );
    }

    m_pfnClickPredicate = tParams.pfnClickPredicate;
    m_snoClickSound = tParams.snoClickSound;
    m_snoMouseoverSound = tParams.snoMouseoverSound;

    m_ptTextLabel = NULL;
    if ( tParams.sDecoratedLabel.Length() )
    {
        Vector2D vecLabelTL;
        Vector2D vecLabelDim;
        if ( tParams.eStyle == CS_STANDARD_BOX )
        {
            vecLabelTL = Vector2D( vecBoxDim.x, 0.0f );
            vecLabelDim = Vector2D( vecDim.x - vecBoxDim.x, vecDim.y );
        }
        else
        {
            vecLabelTL = Vector2D( 0.0f, 0.0f );
            vecLabelDim = vecBoxDim;
        }



        AlignPosition eTooltipAlignmentHorizontal = tParams.eToolTipAlignHorizontal;
        AlignPosition eTooltipAlignmentVertical = tParams.eToolTipAlignVertical;
        if ( eTooltipAlignmentHorizontal == AP_NONE )
            eTooltipAlignmentHorizontal = AP_HORIZONTAL_CENTER;



        if ( eTooltipAlignmentVertical == AP_NONE )
            eTooltipAlignmentVertical = AP_VERTICAL_ABOVE;


        TextCreationParams tTextParams( UIT_TEXT, "textlabel", id );
        tTextParams.eJustifyHorizontal = ( tParams.eStyle == CS_STANDARD_BOX ) ? UIJH_LEFT : UIJH_CENTER;
        tTextParams.eJustifyVertical = UIJV_ASCENDER_MIDDLE;
        tTextParams.eResizeToFit = tParams.eResizeToFit;
        tTextParams.snoFont = tParams.snoFont;
        tTextParams.nFontSize = tParams.nFontSize;
        tTextParams.rgbaText = tParams.rgbaTextColors[CS_UNCHECKED_NORMAL];
        tTextParams.rgbaDropShadow = tParams.rgbaDropShadowColors[CS_UNCHECKED_NORMAL];
        tTextParams.rgbaTextDisabled = tParams.rgbaTextDisabled;
        tTextParams.nToolTipWidth = tParams.nToolTipWidth;
        tTextParams.eToolTipAlignHorizontal = eTooltipAlignmentHorizontal;
        tTextParams.eToolTipAlignVertical = eTooltipAlignmentVertical;
        tTextParams.rectTextPaddingUIC = tParams.rectTextPaddingUIC;
        tTextParams.fAllCaps = tParams.fAllCaps;
        tTextParams.nLinesBeforeTruncation = tParams.nLinesBeforeTruncation;

        m_ptTextLabel = &UI_GetTextFromID( CreateControl<UIText, TextCreationParams>( tTextParams, vecLabelTL, vecLabelDim ) );
        if ( tParams.sDecoratedLabel.Length() )
        {
            CRefString sLabelText = StringListFetch( tParams.sDecoratedLabel );
            m_ptTextLabel->SetText( sLabelText, FALSE );
        }
    }

    sSetCheckBoxVisualState( CS_UNCHECKED_NORMAL, FALSE );
    SetEnabled( tParams.fActive );
}















UICheckBox::UICheckBox( const CheckBoxCreationParams & tParams, const Rect2D * pRect )
    : UIWindow( tParams, pRect )
{
    Vector2D vecTL;
    Vector2D vecDim;
    sRelativeCoordsFromCreationParams( vecTL, vecDim, pRect, tParams );
    sInit( tParams, vecTL, vecDim );
}





UICheckBox::~UICheckBox()
{
}



void UICheckBox::SetEnabled( const BOOL bEnabled )
{
    m_bCheckBoxEnabled = bEnabled;
    if ( !bEnabled )
    {
        if ( UI_GetCapture() == GetID() )
        {
            UI_SetCapture( GetUIIDNone() );
        }
        SetWindowColor( m_rgbaDisabled );
        if ( m_ptWndBox )
            m_ptWndBox->SetWindowColor( m_rgbaDisabled );
        if ( IsChecked() )
            sSetCheckBoxVisualState( CS_CHECKED_DISABLED, TRUE );
        else
            sSetCheckBoxVisualState( CS_UNCHECKED_DISABLED, TRUE );
    }
    else
    {
        SetWindowColor( crgbaWhite );
        if ( m_ptWndBox )
            m_ptWndBox->SetWindowColor( crgbaWhite );
        if ( IsChecked() )
            sSetCheckBoxVisualState( CS_CHECKED_NORMAL, TRUE );
        else
            sSetCheckBoxVisualState( CS_UNCHECKED_NORMAL, TRUE );
    }
    if ( m_ptTextLabel )
        m_ptTextLabel->SetEnabled( bEnabled );
}






















inline void UICheckBox::sSetCheckBoxVisualState( CheckBoxState eCheckBoxState, BOOL fRunOnClick )
{
    BOOL fWasChecked = IsChecked();




    m_eCheckBoxState = eCheckBoxState;
    CheckBoxState eVisualState = eCheckBoxState;
    if ( UI_GetKeyboardFocus() == GetID() && ( eVisualState == CS_UNCHECKED_NORMAL || eVisualState == CS_CHECKED_NORMAL ) )
        eVisualState = (CheckBoxState)( eVisualState + 1 );



    if ( m_tTextureFrames[eVisualState] != cgtImageTextureFrameNull )
    {
        if ( m_ptWndBox )
            m_ptWndBox->SetTextureFrame( m_tTextureFrames[eVisualState] );
        else
            SetTextureFrame( m_tTextureFrames[eVisualState] );
    }






    if ( m_ptTextLabel )
    {

        m_ptTextLabel->SetTextColor( m_rgbaTextColors[eVisualState] );
        m_ptTextLabel->SetDropShadowColor( m_rgbaDropShadowColors[eVisualState] );
    }
    BOOL fIsChecked = IsChecked();

    if ( fRunOnClick && fWasChecked != fIsChecked )
    {
        if ( m_pFunctionsPtr && m_pFunctionsPtr->pfnOnClick )
        {
            m_pFunctionsPtr->pfnOnClick( GetID() );
        }
    }
}




void UICheckBox::SetCheckBoxState( const BOOL fCheckBoxState, const BOOL fRunOnClick )
{
    if ( fCheckBoxState )
        sSetCheckBoxVisualState( CS_CHECKED_NORMAL, fRunOnClick );
    else
        sSetCheckBoxVisualState( CS_UNCHECKED_NORMAL, fRunOnClick );
}










void UICheckBox::SetCheckBoxHoverState( const BOOL fHover )
{
    CheckBoxState eNewCheckboxState;

    if ( IsChecked() )
    {
        eNewCheckboxState = fHover ? CS_CHECKED_MOUSEOVER : CS_CHECKED_NORMAL;
    }
    else
    {
        eNewCheckboxState = fHover ? CS_UNCHECKED_MOUSEOVER : CS_UNCHECKED_NORMAL;
    }
    sSetCheckBoxVisualState( eNewCheckboxState, TRUE );
}




















BOOL UICheckBox::sToggleState()
{
    CheckBoxState eNewState;

    switch ( m_eCheckBoxState )
    {
    case CS_CHECKED_NORMAL:
        eNewState = CS_UNCHECKED_NORMAL;
        break;
    case CS_CHECKED_MOUSEOVER:
        eNewState = CS_UNCHECKED_MOUSEOVER;
        break;
    case CS_UNCHECKED_NORMAL:
        eNewState = CS_CHECKED_NORMAL;
        break;
    case CS_UNCHECKED_MOUSEOVER:
        eNewState = CS_CHECKED_MOUSEOVER;
        break;
    default:
        return FALSE;
    }

    sSetCheckBoxVisualState( eNewState, TRUE );
    return eNewState;
}





void UICheckBox::SetText( LPCSTR uszNewText )
{
    if ( m_ptTextLabel )
        m_ptTextLabel->SetText( uszNewText, FALSE );
}




















void UICheckBox::EnteredHitRegion()
{
    const UIID & id = GetID();
    UIID idCapture = UI_GetCapture();
    if ( idCapture != GetUIIDNone() && id != idCapture )
        return;

    if ( IsChecked() )
        sSetCheckBoxVisualState( CS_CHECKED_MOUSEOVER, TRUE );
    else
        sSetCheckBoxVisualState( CS_UNCHECKED_MOUSEOVER, TRUE );

    if ( m_snoMouseoverSound != SNO_NONE )
        SoundManager::PlaySound( m_snoMouseoverSound, cgtPSODefaults );
}











void UICheckBox::LeftHitRegion()
{
    if ( IsChecked() )
        sSetCheckBoxVisualState( CS_CHECKED_NORMAL, TRUE );
    else
        sSetCheckBoxVisualState( CS_UNCHECKED_NORMAL, TRUE );
}








void UICheckBox::LostKeyboardFocus()
{
    sSetCheckBoxVisualState( m_eCheckBoxState, FALSE );
}




void UICheckBox::GainedKeyboardFocus()
{
    sSetCheckBoxVisualState( m_eCheckBoxState, FALSE );
}





BOOL UICheckBox::sOnMouseMove()
{
    const UIID & id = GetID();
    if ( UI_GetCapture() != id )
        return FALSE;

    BOOL fIsChecked = IsChecked();
    if ( !m_bCheckBoxEnabled )
    {
        if ( fIsChecked )
            sSetCheckBoxVisualState( CS_CHECKED_DISABLED, TRUE );
        else
            sSetCheckBoxVisualState( CS_UNCHECKED_DISABLED, TRUE );
    }
    else if ( UI_GetMousedOverControl() == id )
    {
        if ( fIsChecked )
            sSetCheckBoxVisualState( CS_CHECKED_MOUSEOVER, TRUE );
        else
            sSetCheckBoxVisualState( CS_UNCHECKED_MOUSEOVER, TRUE );
    }
    else
    {
        if ( fIsChecked )
            sSetCheckBoxVisualState( CS_CHECKED_NORMAL, TRUE );
        else
            sSetCheckBoxVisualState( CS_UNCHECKED_NORMAL, TRUE );
    }
    return TRUE;
}






















BOOL UICheckBox::sInput( const BufferedInput & tInput, BOOL & fPassToBaseClass )
{
    const UIID & id = GetID();
    fPassToBaseClass = TRUE;

    switch ( tInput.eInputMsg )
    {
    case INPUTMSG_CONTROLLERBUTTONDOWN:
    case INPUTMSG_CONTROLLERBUTTONREPEAT:


        if ( tInput.tBufControllerData.eInputButton == 0 )
        {
            if ( m_snoClickSound != SNO_NONE )
            {
                SoundManager::PlaySound( m_snoClickSound, cgtPSODefaults );
            }
            sToggleState();

            fPassToBaseClass = FALSE;
            return TRUE;
        }
        break;

    case INPUTMSG_MOUSEMOVE:
        fPassToBaseClass = FALSE;
        return sOnMouseMove();

    case INPUTMSG_LEFTMOUSEDOWN:
    case INPUTMSG_LEFTMOUSEDOUBLECLICK:
        {

            fPassToBaseClass = FALSE;
            UIID idHitTest = UI_GetMousedOverControl();
            if ( IsEnabled() )
            {
                if ( idHitTest != id && !( UI_TestID( idHitTest ) && UI_GetControlFromID( idHitTest ).HasParent( id ) ) )
                    return FALSE;


                UI_SetCapture( id );
                if ( tInput.eInputMsg == INPUTMSG_LEFTMOUSEDOUBLECLICK )
                {
                    if ( m_pFunctionsPtr && m_pFunctionsPtr->pfnOnDoubleClick )
                    {
                        m_pFunctionsPtr->pfnOnDoubleClick( id );
                    }
                }
                return TRUE;
            }
        }
        break;

    case INPUTMSG_LEFTMOUSEUP:
        {
            fPassToBaseClass = FALSE;
            if ( UI_GetCapture() != id )
            {
                return FALSE;
            }

            UI_SetCapture( GetUIIDNone() );

            UIID idHitTest = UI_GetMousedOverControl();
            if ( idHitTest == id ||
                 ( idHitTest != GetUIIDNone() && UI_GetControlFromID( idHitTest ).HasParent( id ) ) )
            {
                if ( m_pfnClickPredicate == NULL ||
                     m_pfnClickPredicate( id ) )
                {








                    SoundManager::PlaySound( m_snoClickSound, cgtPSODefaults );
                    sToggleState();
                    HandleContextMenu( tInput );
                }
            }
            return TRUE;
        }

    case INPUTMSG_LEFTMOUSESTILLDOWN_NO_DELAY:
    case INPUTMSG_LEFTMOUSESTILLDOWN:
    case INPUTMSG_RIGHTMOUSESTILLDOWN_NO_DELAY:
    case INPUTMSG_RIGHTMOUSESTILLDOWN:
        return TRUE;

    case INPUTMSG_KEYDOWN:



        if ( UI_GetKeyboardFocus() == id )
        {
            switch ( tInput.tBufKeyData.eKey )
            {
            case KEY_SPACE:

                if ( IsActive() && IsEnabled() )
                {
                    fPassToBaseClass = FALSE;
                    SoundManager::PlaySound( m_snoClickSound, cgtPSODefaults );
                    sToggleState();
                    return TRUE;
                }
                break;
            case KEY_TAB:



                fPassToBaseClass = FALSE;
                UI_IncrementKeyboardFocus( InputShiftKeyIsDown( INPUTCONTEXT_DEFAULT ) );
                return TRUE;
            }
        }
        break;
    }
    return FALSE;
}












void UICheckBox::SetActive( const BOOL fActive )
{
    if ( !fActive )
    {
        if ( IsChecked() )
            sSetCheckBoxVisualState( CS_CHECKED_NORMAL, TRUE );
        else
            sSetCheckBoxVisualState( CS_UNCHECKED_NORMAL, TRUE );
    }
    UIWindow::SetActive( fActive );
}










BOOL UICheckBox::HandleMessage( const UIMsg & msg, const uintptr dwParam )
{
    switch ( msg )
    {
    case UI_INPUT:
        {
            BOOL fPassToBaseClass = TRUE;
            BOOL bRetval = sInput( *(const BufferedInput *)dwParam, fPassToBaseClass );
            if ( !fPassToBaseClass )
                return bRetval;
        }
        break;
    case UI_REGISTER_INPUT_LISTENER:
        UI_RegisterInputListener( this );
        break;
    }
    return UIWindow::HandleMessage( msg, dwParam );
}

















UIID UICheckBox::HitTestForTooltips( const IVector2D & pt, BOOL & bContinueTesting )
{
    if ( IsActive() && IsInside( pt ) && ClickIsEaten( pt ) )
    {
        LPCSTR pszTooltipText = GetTooltipText();
        if ( ( pszTooltipText && pszTooltipText[0] ) || ( m_ptTextLabel && m_ptTextLabel->IsTruncated() ) )
            return GetID();
        bContinueTesting = FALSE;
    }
    return GetUIIDNone();
}
