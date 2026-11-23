// Original file: X/Client/UserInterface/Controls/UIButtonControl.cpp
// Generated from build/try/p2-uic/btn.cpp by gen.py (functions padded to their original lines).

#include "UIButtonControl.h"
#include <string.h>
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Random.h"
#include "../../../Sigma/SigmaTiming.h"
#include "../../CGame.h"
#include "../../Definitions/ControllerSchemeDefinition.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../ImageCollection.h"
#include "../../Input/Input.h"
#include "../../Sound/Sound.h"
#include "../Interface/UIFunctions.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "../UIUtils.h"

// Declared here until their headers carry them (signatures from the DWARF).
UIMsgFunctionPtr UIMsgFunction_Lookup( LPCSTR szFunction );  // UIFunctions.cpp:3137
const UIID & UI_GetCapture();  // UI.cpp
void UI_SetCapture( const UIID & idCapture );  // UI.cpp
extern UIAttributeType sgtConsoleButtonAttributes[18];  // UIWindowControl.cpp:45
void TextureGetFrameRectPixels( IRect2D & rectTexturePixels, const SNO snoTexture, const uint32 dwFrameIndex );  // TextureDefinition.cpp:2697
void SpriteDrawSimple( const SNO snoTexture, const SNO snoShader, const Rect2D & wrScreen, const Vector2D * wpUVs, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const RenderLayer eRenderLayerOverride );  // Sprite.cpp



static const uint32 KEY_BUTTON_DOWN_TIME = 6;




void ButtonCreationParams::sInit()
{




    vecTextButtonPressOffsetUIC.Set( 3.0f, 3.0f );



    for ( int32 i = 0; i < BS_MAX; i++ )
    {
        tTextureFrame[i] = cgtImageTextureFrameNull;
        tIconTextureFrame[i] = cgtImageTextureFrameNull;
    }

    fHasDisabledTextureFrame = FALSE;
    tTextureFrameDisabled = cgtImageTextureFrameNull;
    tIconTextureFrameDisabled = cgtImageTextureFrameNull;

    rgbaTextHighlight = crgbaWhite;
    rgbaTextHighlightDropShadow = crgbaUIOrange;
    rgbaTextPushed = crgbaWhite;
    rgbaTextPushedDropShadow = crgbaUIOrange;
    rgbaDisabled = crgbaGray;

    pfnOnHitRegionEnter = NULL;
    pfnOnHitRegionExit = NULL;
    pfnOnLeftMouseUp = NULL;
    pfnOnLeftMouseDown = NULL;
    pfnClickPredicate = NULL;
    nRepeatRate = 0;
    fDoActionOnMouseDown = FALSE;
    fIsDragTarget = FALSE;
    fDelayRepeat = FALSE;
    eButtonStyle = BUTTONSTYLE_NORMAL;
    eJustifyHorizontal = UIJH_CENTER;
    eJustifyVertical = UIJV_MIDDLE;




    nUIAction = -1;
    nUIActionAlt = -1;
    snoMouseoverSound = SNO_NONE;
    snoClickSound = SNO_NONE;
    snoRightClickSound = SNO_NONE;

    vecIconDim.Set( 0.0f, 0.0f );
    eButtonIconAlignment = BUTTON_ICON_LEFT;
    nIconTextPaddingX = 0;
}
#line 88
ButtonCreationParams::ButtonCreationParams( const UIType eType, const UIID & id, const UIID & idParent )

    : TextCreationParams( eType, id, idParent )
{

    sInit();
}

#line 100
ButtonCreationParams::ButtonCreationParams( const UIType eType, LPCSTR szName, const UIID & idParent )

    : TextCreationParams( eType, szName, idParent )
{

    sInit();
}





void ButtonCreationParams::Load( const SNO snoUI, const UIControlDataExt * pControlData )
{
    if ( snoUI == SNO_NONE )
        return;

    TextCreationParams::Load( snoUI, pControlData );

    for ( int32 i = 0; i < BS_MAX; i++ )
    {
        pControlData->GetImageAttribute( ATT_IMAGEFRAME + i, tTextureFrame[i] );
        pControlData->GetImageAttribute( ATT_IMAGEFRAME_ICON + i, tIconTextureFrame[i] );
    }

    if ( pControlData->GetImageAttribute( ATT_IMAGEFRAME_DISABLED, tTextureFrameDisabled ) )
        fHasDisabledTextureFrame = TRUE;

    pControlData->GetImageAttribute( ATT_IMAGEFRAME_ICON_DISABLED, tIconTextureFrameDisabled );

    UIColor uicolor = pControlData->GetUint32Value( ATT_FOREGROUND_HIGHLIGHT );
    rgbaTextHighlight.Set( uicolor );
    uicolor = pControlData->GetUint32Value( ATT_DROPSHADOW_HIGHLIGHT );
    rgbaTextHighlightDropShadow.Set( uicolor );
    uicolor = pControlData->GetUint32Value( ATT_FOREGROUND_PUSHED );
    rgbaTextPushed.Set( uicolor );
    uicolor = pControlData->GetUint32Value( ATT_DROPSHADOW_PUSHED );
    rgbaTextPushedDropShadow.Set( uicolor );
    uicolor = pControlData->GetUint32Value( ATT_BUTTON_DISABLED_COLOR );
    rgbaDisabled.Set( uicolor );

    float flClicksPerSec = pControlData->GetFloat32Value( ATT_BUTTON_REPEAT );
    if ( flClicksPerSec < 0.000001f )
        nRepeatRate = 0;
    else
        nRepeatRate = (int32)( 60.0f / flClicksPerSec );

    fDoActionOnMouseDown = pControlData->GetBoolValue( ATT_BUTTON_DO_ACTION_ON_MOUSEDOWN );
    fIsDragTarget = pControlData->GetBoolValue( ATT_IS_DRAG_TARGET );

    vecTextButtonPressOffsetUIC.Set( (float)pControlData->GetInt32Value( ATT_BUTTON_PRESS_HORIZONTAL ), (float)pControlData->GetInt32Value( ATT_BUTTON_PRESS_VERTICAL ) );

    eButtonStyle = (ButtonStyle)pControlData->GetInt32Value( ATT_BUTTON_STYLE );

    if ( !pControlData->AttributeExists( ATT_TEXTJUSTIFY ) )
        eJustifyHorizontal = UIJH_CENTER;
    if ( !pControlData->AttributeExists( ATT_TEXTJUSTIFY_VERTICAL ) )
        eJustifyVertical = UIJV_MIDDLE;

    nUIAction = pControlData->GetInt32Value( ATT_HOTKEY );
    nUIActionAlt = pControlData->GetInt32Value( ATT_HOTKEY_ALT );

    pfnClickPredicate = UIMsgFunction_Lookup( pControlData->GetAttributeValue( ATT_CLICKPREDICATE ) );

    snoMouseoverSound = SNOFileLookupGUID( SOUND_SNO, pControlData->GetAttributeValue( ATT_MOUSEOVER_SOUND ) );
    snoClickSound = SNOFileLookupGUID( SOUND_SNO, pControlData->GetAttributeValue( ATT_CLICK_SOUND ) );
    snoRightClickSound = SNOFileLookupGUID( SOUND_SNO, pControlData->GetAttributeValue( ATT_RIGHT_CLICK_SOUND ) );

    int32 nIconW = Math::Min( pControlData->GetInt32Value( ATT_BUTTON_ICON_W ), (int32)vecDim.x );
    int32 nIconH = Math::Min( pControlData->GetInt32Value( ATT_BUTTON_ICON_H ), (int32)vecDim.y );
    vecIconDim.Set( (float)nIconW, (float)nIconH );
    eButtonIconAlignment = (ButtonIconAlignment)pControlData->GetInt32Value( ATT_BUTTON_ICON_ALIGNMENT );
    nIconTextPaddingX = pControlData->GetInt32Value( ATT_BUTTON_ICON_TEXT_PADDING_X );
}






























void UIButton::sInit( const ButtonCreationParams & tParams )
{

    m_eButtonState = BS_NORMAL;

    m_idIcon = GetUIIDNone();
    m_dwButtonDownOverrideTime = 0;
    m_fDrawIcon = FALSE;
    m_fAllowClickedTextOffset = TRUE;

    BOOL fHaveIconTexture = FALSE;
    for ( int32 i = 0; i < BS_MAX; i++ )
    {
        m_tTextureFrame[i] = tParams.tTextureFrame[i];
        m_tIconTextureFrame[i] = tParams.tIconTextureFrame[i];
        if ( m_tIconTextureFrame[i] != cgtImageTextureFrameNull )
            fHaveIconTexture = TRUE;
    }

    m_fHasDisabledTextureFrame = tParams.fHasDisabledTextureFrame;
    m_tTextureFrameDisabled = tParams.tTextureFrameDisabled;
    m_eButtonIconAlignment = tParams.eButtonIconAlignment;
    m_nIconTextPaddingX = tParams.nIconTextPaddingX;

    m_tIconTextureFrameDisabled = tParams.tIconTextureFrameDisabled;
    if ( m_tIconTextureFrameDisabled != cgtImageTextureFrameNull )
        fHaveIconTexture = TRUE;

    if ( tParams.vecIconDim.x > 0.0f && tParams.vecIconDim.y > 0.0f )
    {
        UIID idIcon;



        idIcon.SetFromParent( GetID(), "_icon_" );
        WindowCreationParams tWinParams( UIT_WINDOW, idIcon, GetID() );
        Rect2D rectPadding = GetTextPadding();




        tWinParams.vecTL.Set( rectPadding.left + tParams.vecTextInsetUIC.x, rectPadding.top + tParams.vecTextInsetUIC.y );
        tWinParams.vecDim = tParams.vecIconDim;
        tWinParams.fActive = fHaveIconTexture;
        tWinParams.fCodeConstructed = TRUE;

        UIWindow * ptIconWindow = new UIWindow( tWinParams, NULL );
        m_idIcon = ptIconWindow->GetID();
        m_fDrawIcon = fHaveIconTexture;

        if ( tParams.eResizeToFit )
        {
            ResizeToFitText();
        }

    }

    m_rgbaTextOriginal = GetTextColor();
    m_rgbaTextOriginalDropShadow = GetTextDropShadowColor();

    m_snoMouseoverSound = tParams.snoMouseoverSound;
    m_snoClickSound = tParams.snoClickSound;
    m_snoRightClickSound = tParams.snoRightClickSound;

    SetButtonState( BS_NORMAL, TRUE, FALSE );

    SetJustification( tParams.eJustifyHorizontal, tParams.eJustifyVertical );
    SetEnabled( TRUE );

    m_rgbaTextHighlight = tParams.rgbaTextHighlight;
    m_rgbaTextHighlightDropShadow = tParams.rgbaTextHighlightDropShadow;

    m_rgbaTextPushed = tParams.rgbaTextPushed;
    m_rgbaTextPushedDropShadow = tParams.rgbaTextPushedDropShadow;

    m_pfnOnHitRegionEnter = tParams.pfnOnHitRegionEnter;
    m_pfnOnHitRegionExit = tParams.pfnOnHitRegionExit;
    m_pfnClickPredicate = tParams.pfnClickPredicate;
    m_pfnOnLeftMouseUp = tParams.pfnOnLeftMouseUp;
    m_pfnOnLeftMouseDown = tParams.pfnOnLeftMouseDown;
    m_nRepeatRate = tParams.nRepeatRate;
    m_nRepeatTicks = 0;
    m_fDelayRepeat = tParams.fDelayRepeat;
    m_fDoActionOnMouseDown = tParams.fDoActionOnMouseDown;
    m_fIsDragTarget = tParams.fIsDragTarget;
    m_fIsDragSource = tParams.fIsDragTarget && tParams.pfnOnDrag != NULL;
    if ( m_fIsDragSource )
    {
        m_dwFlags |= 0x38;
    }
    m_eButtonStyle = tParams.eButtonStyle;
    m_dwSeed = GetMachineTicks();
    m_rgbaDisabled = tParams.rgbaDisabled;
    m_nUIAction = Math::Pin( tParams.nUIAction, -1, 97 );
    m_nUIActionAlt = Math::Pin( tParams.nUIActionAlt, -1, 97 );
}
















UIButton::UIButton( const ButtonCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim )

    : UIText( tParams, vecTL, vecDim )
{

    sInit( tParams );
}





UIButton::UIButton( const ButtonCreationParams & tParams, const Rect2D * pRect )
    : UIText( tParams, pRect )
{
    sInit( tParams );
}





void UIButton::SetEnabled( const BOOL bEnabled )
{
    const UIID & id = GetID();

    UIText::SetEnabled( bEnabled );

    if ( !bEnabled )
    {
        if ( UI_GetCapture() == id )
        {
            UI_SetCapture( GetUIIDNone() );
        }
        else
        {
            SetButtonState( BS_NORMAL, TRUE, FALSE );
        }









        SetWindowColor( m_rgbaDisabled );
        if ( UI_TestID( m_idIcon ) && m_fDrawIcon )
        {
            UIWindow & tIconWindow = UI_GetWindowFromID( m_idIcon );
            tIconWindow.SetWindowColor( m_rgbaDisabled );
            tIconWindow.SetTextureFrame( m_tIconTextureFrameDisabled );
        }
        if ( m_fHasDisabledTextureFrame )
        {
            UIWindow::SetTextureFrame( m_tTextureFrameDisabled );
        }
        else
        {
            UIWindow::SetTextureFrame( m_tTextureFrame[BS_NORMAL] );
        }
    }
    else
    {
        if ( UI_TestID( m_idIcon ) && m_fDrawIcon )
        {
            UIWindow & tIconWindow = UI_GetWindowFromID( m_idIcon );
            tIconWindow.SetWindowColor( crgbaWhite );
        }
        SetWindowColor( crgbaWhite );
        SetButtonState( m_eButtonState, TRUE, FALSE );
    }
}




Vector2D UIButton::CalculateTextSize() const
{
    Vector2D newDim = UIText::CalculateTextSize();
    if ( UI_TestID( m_idIcon ) && m_fDrawIcon )
    {
        Vector2D vecTL;
        Vector2D vecDim;
        UI_GetWindowFromID( m_idIcon ).GetRelativePosition( vecTL, vecDim );
        newDim.x += vecDim.x;
    }
    return newDim;
}





void UIButton::GetTextRegionRectUIC( Rect2D & rectTextExtentsUIC ) const
{
    UIText::GetTextRegionRectUIC( rectTextExtentsUIC );
    if ( UI_TestID( m_idIcon ) && m_fDrawIcon )
    {
        Vector2D vecIconTL;
        Vector2D vecIconDim;
        UI_GetWindowFromID( m_idIcon ).GetRelativePosition( vecIconTL, vecIconDim );
        switch ( m_eButtonIconAlignment )
        {
        case BUTTON_ICON_RIGHT:
            rectTextExtentsUIC.right -= vecIconDim.x + (float)m_nIconTextPaddingX;
            break;

        case BUTTON_ICON_LEFT:
            rectTextExtentsUIC.left += vecIconDim.x + (float)m_nIconTextPaddingX;
            break;
        }
    }
}





void UIButton::sRunOnClickFunction( UIAttributeType eAttribute, const BOOL fRepeated )
{



    switch ( eAttribute )
    {
    case ATT_ONCLICK:
        if ( m_pFunctionsPtr && m_pFunctionsPtr->pfnOnClick )
        {
            if ( !fRepeated )
            {
                SoundManager::PlaySound( m_snoClickSound, cgtPSODefaults );
            }
            m_pFunctionsPtr->pfnOnClick( GetID() );
        }
        break;



    case ATT_ONRIGHTCLICK:
        if ( m_pFunctionsPtr && m_pFunctionsPtr->pfnOnRightClick )
        {
            if ( !fRepeated )
            {
                SoundManager::PlaySound( m_snoRightClickSound != SNO_NONE ? m_snoRightClickSound : m_snoClickSound, cgtPSODefaults );
            }
            m_pFunctionsPtr->pfnOnRightClick( GetID() );
        }
        break;



    case ATT_ONLBUTTONSTILLDOWN:
        if ( m_pFunctionsPtr && m_pFunctionsPtr->pfnOnLButtonStillDown )
        {
            if ( !fRepeated )
            {
                SoundManager::PlaySound( m_snoClickSound, cgtPSODefaults );
            }
            m_pFunctionsPtr->pfnOnLButtonStillDown( GetID() );
        }
        break;
    }
}
void UIButton::SetButtonState( ButtonState eButtonState, const BOOL bForceUpdate, const BOOL bSuppressSounds )
{
    ButtonState eOldState = m_eButtonState;
    m_eButtonState = eButtonState;






    ButtonState eNewButtonState = GetButtonState();

    if ( !bSuppressSounds && eOldState != eNewButtonState && eNewButtonState == BS_MOUSEOVER && m_snoMouseoverSound != SNO_NONE )
    {
        SoundManager::PlaySound( m_snoMouseoverSound, cgtPSODefaults );
    }

    if ( !bForceUpdate && eOldState == eNewButtonState )
        return;









    if ( !IsEnabled() && m_fHasDisabledTextureFrame )
    {
        UIWindow::SetTextureFrame( m_tTextureFrameDisabled );
    }
    else
    {
        UIWindow::SetTextureFrame( m_tTextureFrame[eNewButtonState] );
    }

    if ( UI_TestID( m_idIcon ) && m_fDrawIcon )
    {
        UI_GetWindowFromID( m_idIcon ).SetTextureFrame( m_tIconTextureFrame[eNewButtonState] );
    }

    switch ( eNewButtonState )
    {
    case BS_MOUSEOVER:
        UIText::SetTextColor( m_rgbaTextHighlight );
        SetDropShadowColor( m_rgbaTextHighlightDropShadow );
        break;
    case BS_PUSHED:
        UIText::SetTextColor( m_rgbaTextPushed );
        SetDropShadowColor( m_rgbaTextPushedDropShadow );
        break;
    default:
        UIText::SetTextColor( m_rgbaTextOriginal );
        SetDropShadowColor( m_rgbaTextOriginalDropShadow );
        break;
    }
}













ButtonState UIButton::GetButtonState()
{
    if ( m_dwButtonDownOverrideTime )
    {
        if ( CGameGetRealTicks() < m_dwButtonDownOverrideTime )
            return BS_PUSHED;
        m_dwButtonDownOverrideTime = 0;
        SetButtonState( m_eButtonState, TRUE, FALSE );
    }
    return m_eButtonState;
}














































































void UIButton::SetUIAction( int32 nAction, int32 nAltAction )
{
    m_nUIAction = nAction;
    m_nUIActionAlt = nAltAction;
}





void UIButton::SetTextColor( const RGBAColor & rgbaColor )
{
    m_rgbaTextOriginal = rgbaColor;
    UIText::SetTextColor( rgbaColor );
    SetButtonState( m_eButtonState, TRUE, FALSE );
}













































void UIButton::SetAllTextureFrames( const ImageTextureFrame & tTextureFrame )
{
    for ( int32 i = 0; i < BS_MAX; i++ )
        SetTextureFrame( (ButtonState)i, tTextureFrame );
}






void UIButton::SetTextureFrame( const ButtonState eState, const ImageTextureFrame & tTextureFrame )
{
    if ( m_tTextureFrame[eState] != tTextureFrame )
    {
        m_tTextureFrame[eState] = tTextureFrame;
        if ( GetButtonState() == eState )
        {
            UIWindow::SetTextureFrame( m_tTextureFrame[eState] );
        }
    }
}














void UIButton::SetTextureFrameDisabled( const ImageTextureFrame & tTextureFrameDisabled )
{
    m_tTextureFrameDisabled = tTextureFrameDisabled;
    m_fHasDisabledTextureFrame = TRUE;
}

















void UIButton::SetIconTextureFrame( const ButtonState eState, const ImageTextureFrame & tTextureFrame )
{
    if ( m_tIconTextureFrame[eState] != tTextureFrame )
    {
        m_tIconTextureFrame[eState] = tTextureFrame;
        if ( GetButtonState() == eState )
        {
            SetButtonState( eState, TRUE, FALSE );
        }
    }
}


























void UIButton::SetTextureFrame( const ButtonState eState, LPCSTR szDecoratedLabel )
{
    ImageTextureFrame tTexFrame;
    ImageCollectionGetImageInfo( szDecoratedLabel, tTexFrame );
    SetTextureFrame( eState, tTexFrame );
}






void UIButton::EnteredHitRegion()
{
    if ( !IsEnabled() )
        return;

    const UIID & id = GetID();
    UIID idCapture = UI_GetCapture();
    if ( idCapture == GetUIIDNone() || id == idCapture )
    {
        if ( m_pfnOnHitRegionEnter )
            m_pfnOnHitRegionEnter( id );
        SetButtonState( BS_MOUSEOVER, FALSE, FALSE );
    }
}











void UIButton::LeftHitRegion()
{
    if ( m_pfnOnHitRegionExit )
        m_pfnOnHitRegionExit( GetID() );
    SetButtonState( BS_NORMAL, FALSE, FALSE );
}






// NON_MATCHING: register allocation swaps x21/x22 (fPassToBaseClass pointer vs. return value).
BOOL UIButton::sInput(
    const BufferedInput & tInput,
    BOOL & fPassToBaseClass )
{
    fPassToBaseClass = TRUE;
    switch ( tInput.eInputMsg )
    {
    case INPUTMSG_KEYDOWNONCE:
        fPassToBaseClass = FALSE;
        if ( !IsEnabled() )
            return FALSE;
        if ( ( m_nUIAction != -1 && Prefs::ActionKeyMatch( m_nUIAction, tInput.tBufKeyData.eKey, tInput.tBufKeyData.bExactMatch ) ) || ( m_nUIActionAlt != -1 && Prefs::ActionKeyMatch( m_nUIActionAlt, tInput.tBufKeyData.eKey, tInput.tBufKeyData.bExactMatch ) ) )
        {
            if ( m_pfnClickPredicate == NULL || m_pfnClickPredicate( GetID() ) )
            {
                m_dwButtonDownOverrideTime = CGameGetRealTicks() + KEY_BUTTON_DOWN_TIME;
                SetButtonState( m_eButtonState, TRUE, FALSE );
                sRunOnClickFunction( ATT_ONCLICK, FALSE );
            }
            return TRUE;
        }
        break;
    case INPUTMSG_LEFTTRIGGER:
    case INPUTMSG_RIGHTTRIGGER:
    case INPUTMSG_CONTROLLERBUTTONDOWN:
    case INPUTMSG_CONTROLLERBUTTONREPEAT:
        if ( IsEnabled() && ( ( m_nUIAction != -1 && Controller::ActionButtonMatch( m_nUIAction, tInput ) ) || ( m_nUIActionAlt != -1 && Controller::ActionButtonMatch( m_nUIActionAlt, tInput ) ) ) )
        {
            fPassToBaseClass = FALSE;
            if ( m_pfnClickPredicate == NULL || m_pfnClickPredicate( GetID() ) )
            {
                m_dwButtonDownOverrideTime = CGameGetRealTicks() + KEY_BUTTON_DOWN_TIME;
                SetButtonState( m_eButtonState, TRUE, FALSE );
                sRunOnClickFunction( ATT_ONCLICK, FALSE );
                sRunOnClickFunction( sgtConsoleButtonAttributes[tInput.tBufControllerData.eInputButton], FALSE );
            }
            return TRUE;
        }
        break;
    }
    return FALSE;
}



































































































































































































































































































































static inline float sGetCapWidth( const SNO snoTexture, const float flWindowHeight )
{

    IRect2D rectTexturePixels;

    TextureGetFrameRectPixels( rectTexturePixels, snoTexture, (uint32)-1 );

    float flAspectRatio = (float)rectTexturePixels.Width() / (float)rectTexturePixels.Height();

    return flWindowHeight * flAspectRatio;
}




static void sDrawPiece( const SNO snoTexturePiece, const SNO snoShader, const Rect2D & rectUIC, const Rect2D & wrUVs, const RGBAColor & rgbaColor, const float flZ, const int32 nRotations, const Rect2D & rectClippingPixels )
{
    Rect2D rectScreenPixels;
    ConvertUICToPixels( rectUIC, rectScreenPixels );
    Rect2D wrUVsClipped;
    Rect2D rectScreenClipped;
    BOOL bVisible = Rect2D::Intersect( rectScreenPixels, rectClippingPixels, &rectScreenClipped );
    if ( bVisible )
    {
        Rect2D rectFrameSpace;
        Vector2D wpUVs[4];
        rectScreenClipped.ToUnitCoords( rectScreenPixels, rectFrameSpace );
        rectFrameSpace.FromUnitCoords( wrUVs, wrUVsClipped );
        wrUVsClipped.ToPoints( wpUVs, nRotations );
        SpriteDrawSimple( snoTexturePiece, snoShader, rectScreenClipped, wpUVs, rgbaColor, crgbaBlack, flZ, 1.0f, RENDERLAYER_INVALID );
    }
}




















void UIButton::sDrawStretchWithCaps( const Rect2D & rectWindowUIC, const Rect2D & rectClippingUIC, uint8, const RGBAColor & rgbaColor, float flZ )
{
    Rect2D rectClippingPixels;
    SNO snoTexCenter[4];




    ConvertUICToPixels( rectClippingUIC, rectClippingPixels );

    int32 eRightCap, eLeftCap, eCenter4, eCenter3, eCenter2, eCenter;
    ButtonState eState = GetButtonState();
    if ( eState == BS_PUSHED )
        { eRightCap = GLOBALSNO_TEXTURE_TEXTBTN_PUSHED_RIGHTCAP; eLeftCap = GLOBALSNO_TEXTURE_TEXTBTN_PUSHED_LEFTCAP; eCenter4 = GLOBALSNO_TEXTURE_TEXTBTN_PUSHED_CENTER4; eCenter3 = GLOBALSNO_TEXTURE_TEXTBTN_PUSHED_CENTER3; eCenter2 = GLOBALSNO_TEXTURE_TEXTBTN_PUSHED_CENTER2; eCenter = GLOBALSNO_TEXTURE_TEXTBTN_PUSHED_CENTER; }
    else if ( eState == BS_MOUSEOVER )
        { eRightCap = GLOBALSNO_TEXTURE_TEXTBTN_MOUSEOVER_RIGHTCAP; eLeftCap = GLOBALSNO_TEXTURE_TEXTBTN_MOUSEOVER_LEFTCAP; eCenter4 = GLOBALSNO_TEXTURE_TEXTBTN_MOUSEOVER_CENTER4; eCenter3 = GLOBALSNO_TEXTURE_TEXTBTN_MOUSEOVER_CENTER3; eCenter2 = GLOBALSNO_TEXTURE_TEXTBTN_MOUSEOVER_CENTER2; eCenter = GLOBALSNO_TEXTURE_TEXTBTN_MOUSEOVER_CENTER; }
    else
        { eRightCap = GLOBALSNO_TEXTURE_TEXTBTN_NORMAL_RIGHTCAP; eLeftCap = GLOBALSNO_TEXTURE_TEXTBTN_NORMAL_LEFTCAP; eCenter4 = GLOBALSNO_TEXTURE_TEXTBTN_NORMAL_CENTER4; eCenter3 = GLOBALSNO_TEXTURE_TEXTBTN_NORMAL_CENTER3; eCenter2 = GLOBALSNO_TEXTURE_TEXTBTN_NORMAL_CENTER2; eCenter = GLOBALSNO_TEXTURE_TEXTBTN_NORMAL_CENTER; }














    snoTexCenter[0] = GlobalSNOGet( eCenter );
    snoTexCenter[1] = GlobalSNOGet( eCenter2 );
    snoTexCenter[2] = GlobalSNOGet( eCenter3 );
    snoTexCenter[3] = GlobalSNOGet( eCenter4 );
    SNO snoTexLeftCap = GlobalSNOGet( eLeftCap );
    SNO snoTexRightCap = GlobalSNOGet( eRightCap );

    RNG rng;


    RandSetSeed_Internal( rng, m_dwSeed );

    float flWindowHeight = rectWindowUIC.Height();

    float flLeftCapWidth = sGetCapWidth( snoTexLeftCap, flWindowHeight );
    float flRightCapWidth = sGetCapWidth( snoTexRightCap, flWindowHeight );
    SNO snoShader = GlobalSNOGet( GLOBALSNO_SHADER_SPRITE_TRANSPARENT_CLAMP );

    Rect2D rectTextureUVs( 0.0f, 0.0f, 1.0f, 1.0f );


    Rect2D rectPieceUIC( rectWindowUIC.left, rectWindowUIC.top, rectWindowUIC.left + flLeftCapWidth, rectWindowUIC.bottom );
    sDrawPiece( snoTexLeftCap, snoShader, rectPieceUIC, rectTextureUVs, rgbaColor, flZ, 0, rectClippingPixels );

    float flX = rectWindowUIC.left + flLeftCapWidth;
    float flXMax = rectWindowUIC.right - flRightCapWidth;
    while ( flX < flXMax - 0.000001f )
    {

        int32 nCenterPiece = Rand_Internal( rng, 3 );
        float flPieceWidth = sGetCapWidth( snoTexCenter[nCenterPiece], flWindowHeight );
        float flRight = Math::Min( flX + flPieceWidth, flXMax );
        rectPieceUIC = Rect2D( flX, rectWindowUIC.top, flRight, rectWindowUIC.bottom );
        sDrawPiece( snoTexCenter[nCenterPiece], snoShader, rectPieceUIC, rectTextureUVs, rgbaColor, flZ, 0, rectClippingPixels );
        flX = flRight;
    }


    rectPieceUIC = Rect2D( rectWindowUIC.right - flRightCapWidth, rectWindowUIC.top, rectWindowUIC.right, rectWindowUIC.bottom );
    sDrawPiece( snoTexRightCap, snoShader, rectPieceUIC, rectTextureUVs, rgbaColor, flZ, 0, rectClippingPixels );
}


void UIButton::sCustomDraw( const uint8 bLayerAlpha, float & flZSlice )
{


    if ( m_eButtonStyle == BUTTONSTYLE_NORMAL )
        return;



    uint32 dwC = MultiplyAlpha( bLayerAlpha, GetWindowColor().a );
    if ( !dwC )
        return;




    float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
    flZSlice += 1.0f;


    Rect2D rectWindowUIC;
    GetWindowRect( rectWindowUIC );
    const Rect2D & rectClippingUIC = GetCascadedClippingRect();

    switch ( m_eButtonStyle )
    {
    case BUTTONSTYLE_STRETCH_WITH_CAPS:


        sDrawStretchWithCaps( rectWindowUIC, rectClippingUIC, bLayerAlpha, GetWindowColor(), flZ );
        break;
    default:
        break;
    }
}
// NON_MATCHING: rectPadding left/right land in swapped float registers, and the no-icon UIText::Draw is merged with the other call instead of a tail call.
void UIButton::sDraw(
    const uint8 bLayerAlpha,
    float & flZSlice,
    BOOL fTextButtonOffset )
{
    if ( UI_TestID( m_idIcon ) && m_fDrawIcon )
    {
        Rect2D rectPadding = GetTextPadding();
        Vector2D vecIconTL; Vector2D vecIconDim;
        UIWindow & tIconWindow = UI_GetWindowFromID( m_idIcon );
        tIconWindow.GetRelativePosition( vecIconTL, vecIconDim );
        switch ( m_eTextJustify )
        {
        default:
            {

                vecIconTL.x = rectPadding.left + m_vecTextInsetUIC.x;

                if ( m_eButtonIconAlignment == BUTTON_ICON_RIGHT )
                {
                    Vector2D vecTextDim;
                    GetTextRenderedSizeUIC( vecTextDim, TRUE );
                    vecIconTL.x += vecTextDim.x;
                }
            }
            break;
        case UIJH_CENTER:
            {
                Vector2D vecTextDim;
                GetTextRenderedSizeUIC( vecTextDim, TRUE );
                float flTotalContentWidth = vecIconDim.x + vecTextDim.x;
                float flAvailableSpace = m_vecDim.x - rectPadding.left - rectPadding.right;
                float flDistFromLeft = Math::Max( ( flAvailableSpace - flTotalContentWidth ) * 0.5f, 0.0f );
                vecIconTL.x = rectPadding.left + flDistFromLeft;

                if ( m_eButtonIconAlignment == BUTTON_ICON_RIGHT )
                {
                    vecIconTL.x += vecTextDim.x;
                }
            }
            break;
        case UIJH_RIGHT:
            {
                Vector2D vecTextDim;
                GetTextRenderedSizeUIC( vecTextDim, TRUE ); vecIconTL.x = m_vecDim.x - rectPadding.right - m_vecTextInsetUIC.x;

                if ( m_eButtonIconAlignment == BUTTON_ICON_RIGHT )
                {
                    vecIconTL.x = Math::Max( 0.0f, vecIconTL.x - vecIconDim.x );
                }
                else
                {
                    vecIconTL.x = Math::Max( 0.0f, vecIconTL.x - vecTextDim.x - vecIconDim.x );
                }
            }
            break;
        }

        switch ( m_eTextJustifyVertical )
        {
        default:
            vecIconTL.y = rectPadding.top;
            break;
        case UIJV_MIDDLE:
        case UIJV_LINE_MIDDLE:
        case UIJV_ASCENDER_MIDDLE:





            vecIconTL.y = ( m_vecDim.y - vecIconDim.y ) * 0.5f;
            break;

        case UIJV_BOTTOM:

            vecIconTL.y = m_vecDim.y - rectPadding.bottom - vecIconDim.y;
            break;
        }

        if ( fTextButtonOffset )
        {
            vecIconTL += m_vecTextButtonPressOffsetUIC;
        }

        tIconWindow.SetRelativePosition( vecIconTL, vecIconDim );

        UIText::Draw( bLayerAlpha, flZSlice );
    }
    else
    {
        UIText::Draw( bLayerAlpha, flZSlice );
    }
}



void UIButton::Draw( const uint8 bLayerAlpha, float & flZSlice )
{







    sCustomDraw( bLayerAlpha, flZSlice );

    BOOL fPushed = m_fAllowClickedTextOffset && GetButtonState() == BS_PUSHED;
    EnableTextButtonPressOffset( fPushed );
    sDraw( bLayerAlpha, flZSlice, fPushed );
}














void UIButton::SetActive( const BOOL fActive )
{
    if ( !fActive )
        SetButtonState( BS_NORMAL, FALSE, FALSE );
    UIText::SetActive( fActive );
}








BOOL UIButton::HandleMessage( const UIMsg & msg, const uintptr dwParam )
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
        if ( HandlesInput() )
            UI_RegisterInputListener( this );
        break;
    }
    return UIText::HandleMessage( msg, dwParam );
}






















BOOL UIButton::HandlesInput()
{
    return m_nUIAction != -1 || m_nUIActionAlt != -1 || UIText::HandlesInput();
}





UIID UIButton::HitTest( const IVector2D & pt )
{
    if ( IsActive() && EatsClicks() != EC_IGNORE && IsInside( pt ) && ClickIsEaten( pt ) )
        return GetID();
    return GetUIIDNone();
}


















UIID UIButton::HitTestForTooltips( const IVector2D & pt, BOOL & bContinueTesting )
{
    if ( IsActive() && IsInside( pt ) && ClickIsEaten( pt ) )
    {
        LPCSTR pszTooltipText = GetTooltipText();
        if ( pszTooltipText && pszTooltipText[0] )
            return GetID();
        bContinueTesting = FALSE;
    }
    return GetUIIDNone();
}
