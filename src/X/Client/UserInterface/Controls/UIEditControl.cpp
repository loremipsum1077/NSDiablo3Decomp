// Original file: X/Client/UserInterface/Controls/UIEditControl.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "UIEditControl.h"
#include <string.h>
#include <ctype.h>
#include "../../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../Common/Definitions/Definitions.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../CGame.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../Interface/UIFunctions.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "../UIUtils.h"
#include "../../Font/Font.h"
#include "../../../Common/Hyperlink.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Sigma/StringManip.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/StringListDefinition.h"

// Declared here until X/Client/Animation2D.h is adopted (signatures from the DWARF).
void Animation2DReset( AnimState & tAnimState, const SNO snoAnim2D );  // Animation2D.cpp:72
void Animation2DUpdate( AnimState & tAnimState, const SNO snoAnim2D, const float flDeltaTime );  // Animation2D.cpp:99
void Animation2DSet( AnimState & tAnimState, SNO & snoAnim2D, const SNO snoAnim2DNew, const BOOL fForceReset );  // Animation2D.cpp:55
// X/Client/UserInterface/UI.cpp:414
const UIID & UI_GetKeyboardFocus();
// X/Client/Sprite.cpp
void SpriteDrawSimple( const SNO snoTexture, const SNO snoShader, const Rect2D & wrScreen, const Vector2D * wpUVs, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const RenderLayer eRenderLayerOverride );
void Animation2DDraw( const SNO snoAnim2D, const AnimState & tAnimState, const Rect2D & wrRect, const Rect2D & wrRectClipping, const uint8 bAlpha, const float flZ, const RGBAColor & rgbaColor, const SNO snoShader, const RenderLayer eRenderLayerOverride );  // Animation2D.cpp:115
#include "../../Input/Input.h"

// FontDefinition.h (in the original CU: ScopedFontDefinitionLockR is declared at FontDefinition.h:382)
#include "../../Definitions/FontDefinition.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../../Common/Player.h"
BOOL InputShiftKeyIsDown( InputContext eContext );  // X/Client/Input/Input.cpp:771
BOOL InputAltKeyIsDown( InputContext eContext );  // X/Client/Input/Input.cpp:799
BOOL InputPlatformGetClipboard( CHAR * puzString, int32 nMaxLength );  // X/Client/Input/InputNX64.cpp:1066
const char * InputIMEGetCompositionString( int32 * nInsertionIndex );  // X/Client/Input/InputNX64.cpp:1079
const char * InputIMEGetResultString();  // X/Client/Input/InputNX64.cpp:1090
void UI_IncrementKeyboardFocus( const BOOL fReverseTabOrder );  // X/Client/UserInterface/UI.cpp:1226
// X/Client/UserInterface/UIUtils.cpp; UIUtils.h has only the Vector2D overload.
void ConvertPixelsToUIC( const IVector2D & vecPixels, Vector2D & vecUIC );
void ConvertPixelsDimensionsToUIC( const IVector2D & vecPixels, Vector2D & vecUIC );
// Declared here until X/Client/Definitions/TextureDefinition.h carries it.
void TextureGetFrameRectPixels( IRect2D & rectTexturePixels, const SNO snoTexture, const uint32 dwFrameIndex );  // TextureDefinition.cpp:2697
void UI_SetCapture( const UIID & idCapture );  // X/Client/UserInterface/UI.cpp:501
void UI_UpdateIMELocaleIndicator( const UIID & idControl, const Rect2D & rectEdit, AlignPosition eHorizontalAlign, bool bHasIMEFocus );  // X/Client/UserInterface/UIIMEHelpers.cpp:153
void CPlayerInputOtherListenerReceivedAction( Player * ptPlayer, UIAction eAction, uint32 nDelay );  // X/Client/CPlayerInput.cpp:7007




static const float cflVerticalBuffer = 5.0f;
static const float TOP_BOTTOM_INDENT_FRAC = 0.1f;
static inline float sGetCapWidth( const SNO snoTexture, const float flWindowHeight );












EditBoxCreationParams::EditBoxCreationParams( const UIType eType,
                                              const UIID & id,
                                              const UIID & idParent )

    : TextCreationParams( eType, id, idParent )
{
    snoCursorAnim = SNO_NONE;
    eEditBoxStyle = EDITSTYLE_DEFAULT;
    pfnOnReturnEndEdit = NULL;
    pfnOnChange = NULL;
    pfnOnGainedFocus = NULL;
    pfnOnLostFocus = NULL;
    fNumbersOnly = FALSE;
    fAutoFormatNumbers = FALSE;
    rgbaText = crgbaWhite;
    vecTextInsetUIC.y = cflVerticalBuffer;
    eJustifyHorizontal = UIJH_LEFT;
    eJustifyVertical = UIJV_ASCENDER_MIDDLE;
    fIMEDisabled = FALSE;
    nMaxAllowedCharacters = 1024;
    nMaxAllowedBytes = -1;
    nNumDecimalPlaces = 0;
    fAcceptsReturn = FALSE;
}




void EditBoxCreationParams::Load( const SNO snoUI, const UIControlDataExt * pControlData )
{
    TextCreationParams::Load( snoUI, pControlData );

    snoCursorAnim = SNOFileLookupGUID( ANIMATION2D_SNO, pControlData->GetAttributeValue( ATT_TEXTCURSOR_ANIMSNO ) );
    eEditBoxStyle = (EditBoxStyle)pControlData->GetInt32Value( ATT_STYLE );
    pfnOnReturnEndEdit = UIFunction_Lookup( pControlData->GetAttributeValue( ATT_ONRETURNENDEDIT ) );
    pfnOnChange = UIFunction_Lookup( pControlData->GetAttributeValue( ATT_ONCHANGE ) );
    pfnOnGainedFocus = UIFunction_Lookup( pControlData->GetAttributeValue( ATT_ONGAINEDFOCUS ) );
    pfnOnLostFocus = UIFunction_Lookup( pControlData->GetAttributeValue( ATT_ONLOSTFOCUS ) );
    fNumbersOnly = pControlData->GetBoolValue( ATT_NUMBERS_ONLY );
    fAutoFormatNumbers = pControlData->GetBoolValue( ATT_NUMBER_FORMATTING_ENABLED );
    fIMEDisabled = pControlData->GetBoolValue( ATT_IME_DISABLED );
    nMaxAllowedCharacters = pControlData->GetInt32Value( ATT_MAX_EDIT_CHARS );
    nMaxAllowedBytes = pControlData->GetInt32Value( ATT_MAX_EDIT_BYTES );
    nNumDecimalPlaces = pControlData->GetInt32Value( ATT_NUM_DECIMAL_PLACES );
    fAcceptsReturn = pControlData->GetBoolValue( ATT_ACCEPTS_RETURN );

    sPlaceholderTextLabel = pControlData->GetAttributeValue( ATT_EDIT_PLACEHOLDER_TEXT_LABEL );
    UIColor uicolor = pControlData->GetUint32Value( ATT_EDIT_PLACEHOLDER_TEXT_FOREGROUND );
    rgbaPlaceholderText.Set( uicolor );
    uicolor = pControlData->GetUint32Value( ATT_EDIT_PLACEHOLDER_TEXT_DROPSHADOW );
    rgbaPlaceholderTextDropShadow.Set( uicolor );

    if ( eEditBoxStyle != EDITSTYLE_NO_BACKGROUND )
        vecTextInsetUIC.y = cflVerticalBuffer;

    if ( !pControlData->AttributeExists( ATT_FOREGROUND ) )
        rgbaText = crgbaWhite;

    if ( !pControlData->AttributeExists( ATT_TEXTJUSTIFY ) )
        eJustifyHorizontal = UIJH_LEFT;

    if ( !pControlData->AttributeExists( ATT_TEXTJUSTIFY_VERTICAL ) )
    {
        if ( fWordWrap )
            eJustifyVertical = UIJV_LINE_TOP;
        else if ( fIsPassword )
            eJustifyVertical = UIJV_MIDDLE;
        else
            eJustifyVertical = UIJV_ASCENDER_MIDDLE;
    }
}


















void UIEdit::sInit( const EditBoxCreationParams & tParams )
{
    SetSelectable( TRUE );
    SetSelectsRawText( TRUE );
    Animation2DSet( m_tCursorAnimState, m_snoCursor, tParams.snoCursorAnim, TRUE );

    m_pfnOnReturnEndEdit = tParams.pfnOnReturnEndEdit;
    m_pfnOnChange = tParams.pfnOnChange;
    m_pfnOnGainedFocus = tParams.pfnOnGainedFocus;
    m_pfnOnLostFocus = tParams.pfnOnLostFocus;
    m_nUTF8Length = 0;
    m_fIgnoreNextCharMessage = FALSE;
    m_eLocaleOverride = 0;
    m_pfnInputWouldBeValid = NULL;
    m_pfnStringWouldBeValid = NULL;
    m_pfnMaximumAllowedCharactersReached = NULL;
    SetMaximumAllowedCharacters( tParams.nMaxAllowedCharacters, TRUE );
    SetMaximumAllowedBytes( tParams.nMaxAllowedBytes );
    m_nNumDecimalDigits = tParams.nNumDecimalPlaces;

    if ( tParams.sPlaceholderTextLabel.Length() )
        m_tPlaceholderText = FormattedText( StringListFetch( tParams.sPlaceholderTextLabel ), PARSE_DEFAULT );

    m_rgbaPlaceholderText = tParams.rgbaPlaceholderText;
    m_rgbaPlaceholderTextDropShadow = tParams.rgbaPlaceholderTextDropShadow;

    m_dwFlags = 0;
    m_dwFlags = !tParams.fNumbersOnly ? ( m_dwFlags & ~( 1 << EDIT_FLAG_NUMBERS_ONLY_BIT ) ) : ( m_dwFlags | ( 1 << EDIT_FLAG_NUMBERS_ONLY_BIT ) );
    m_dwFlags = !tParams.fAutoFormatNumbers ? ( m_dwFlags & ~( 1 << EDIT_FLAG_NUMBER_FORMATTING_ENABLED ) ) : ( m_dwFlags | ( 1 << EDIT_FLAG_NUMBER_FORMATTING_ENABLED ) );
    m_dwFlags = !( tParams.fIMEDisabled || tParams.fNumbersOnly ) ? ( m_dwFlags & ~( 1 << EDIT_FLAG_IME_DISABLED ) ) : ( m_dwFlags | ( 1 << EDIT_FLAG_IME_DISABLED ) );
    m_dwFlags = !tParams.fAcceptsReturn ? ( m_dwFlags & ~( 1 << EDIT_FLAG_ACCEPTS_RETURN ) ) : ( m_dwFlags | ( 1 << EDIT_FLAG_ACCEPTS_RETURN ) );

    m_nCursorPosition = 0;
    SetWordWrap( tParams.fWordWrap );

    m_eEditBoxStyle = tParams.eEditBoxStyle;
    if ( m_eEditBoxStyle == EDITSTYLE_NO_BACKGROUND )
    {
        m_flIndentLeft = 0.0f;
        m_flIndentRight = 0.0f;
    }
    else
    {
        Vector2D vecTL;
        Vector2D vecDim;
        GetRelativePosition( vecTL, vecDim );

        float flWindowHeight = vecDim.y;
        float flTopBottomIndent = flWindowHeight * TOP_BOTTOM_INDENT_FRAC;
        SNO snoTexLeftCap = GlobalSNOGet( GLOBALSNO_TEXTURE_EDITTEXT_LEFTCAP );
        SNO snoTexRightCap = GlobalSNOGet( GLOBALSNO_TEXTURE_EDITTEXT_RIGHTCAP );
        m_flIndentLeft = sGetCapWidth( snoTexLeftCap, flWindowHeight );
        m_flIndentRight = sGetCapWidth( snoTexRightCap, flWindowHeight );

        vecTL.x += m_flIndentLeft;
        vecTL.y += flTopBottomIndent;
        vecDim.x -= m_flIndentLeft + m_flIndentRight;
        vecDim.y -= flTopBottomIndent + flTopBottomIndent;
        SetRelativePosition( vecTL, vecDim );
    }

    m_bIsIMEComposing = FALSE;
}


































UIEdit::UIEdit( const EditBoxCreationParams & tParams, const Rect2D * pRect )
    : UIText( tParams, pRect )
{
    sInit( tParams );
}









































void UIEdit::SetMaximumAllowedCharacters( const int32 nNewMaximum, BOOL fSetCursor )
{
    m_nMaximumAllowedCharacters = nNewMaximum < 0 ? -1 : nNewMaximum;

    if ( nNewMaximum >= 0 && fSetCursor && m_nUTF8Length > m_nMaximumAllowedCharacters )
    {
        CRefString sTemp( m_tText.GetText() );
        Clear();
        InsertAtCursor( sTemp.str(), FALSE );
    }
}







void UIEdit::SetMaximumAllowedBytes( const int32 nNewMaximum )
{
    m_nMaximumAllowedBytes = nNewMaximum >= MAX_UI_TEXT_LEN ? MAX_UI_TEXT_LEN : nNewMaximum;

    if ( nNewMaximum != -1 )
    {
        if ( m_nMaximumAllowedCharacters != -1 && nNewMaximum < m_nMaximumAllowedCharacters )
            m_nMaximumAllowedCharacters = nNewMaximum;

        CRefString sTemp( m_tText.GetText() );
        Clear();
        InsertAtCursor( sTemp.str(), FALSE );
    }
}
























































inline void UIEdit::sUpdate()
{
    Animation2DUpdate( m_tCursorAnimState, m_snoCursor, CGameGetRealDeltaTime() );
}
















void UIEdit::Clear()
{
    SetText( "", FALSE );
    SetScrollOffsetUIC( UITEXT_OFFSET_HORIZONTAL, 0.0f );
    SetCursorPosition( 0 );
}






BOOL UIEdit::DeleteSelection()
{
    if ( m_nSelectionBegin != -1 && m_nSelectionEnd != -1 )
    {
        int32 nConvertedSelectionBegin;
        int32 nConvertedSelectionEnd;
        ConvertFormattedSelectionToOriginalTextSelection( nConvertedSelectionBegin, nConvertedSelectionEnd );

        CHAR uszTemp[MAX_UI_TEXT_LEN];
        memset( uszTemp, 0, sizeof( uszTemp ) );

        LPCSTR pszCurrentText = m_tText.GetText();
        strncpy( uszTemp, pszCurrentText, nConvertedSelectionBegin );
        Blizzard::String::Copy( uszTemp + nConvertedSelectionBegin, pszCurrentText + nConvertedSelectionEnd, MAX_UI_TEXT_LEN - nConvertedSelectionBegin );

        int32 nSelectionBeginTemp = nConvertedSelectionBegin;
        int32 nHyperlinkIndex = IsInHyperlink( m_nSelectionBegin, TRUE, FALSE );
        if ( nHyperlinkIndex != -1 )
            nSelectionBeginTemp = m_tText.m_arTextDeltas[nHyperlinkIndex]->nOriginalTextIndex;

        if ( m_dwFlags & ( 1 << EDIT_FLAG_NUMBERS_ONLY_BIT ) )
            PerformNumberFormatting( uszTemp, (uint32)strlen( uszTemp ), nSelectionBeginTemp );

        if ( m_pfnStringWouldBeValid && !m_pfnStringWouldBeValid( uszTemp ) )
            return FALSE;

        m_nCursorPosition = 0;
        SetText( uszTemp, FALSE );
        int32 nNewCursorPosition = OriginalTextPosToTextPos( nSelectionBeginTemp );
        SetCursorPosition( nNewCursorPosition );
        ClearSelection();
    }

    return TRUE;
}



























inline void UIEdit::SetCursorPosition( const int32 nFormattedCursorPosition )
{
    int32 nFormattedLength = m_tText.ToFormattedIndex( m_tText.Length() );
    m_nCursorPosition = Math::Pin( nFormattedCursorPosition, 0, nFormattedLength );

    int32 nHyperlinkIndex = IsInHyperlink( m_nCursorPosition, FALSE, FALSE );
    if ( nHyperlinkIndex != -1 )
        m_nCursorPosition = GetText().m_arTextDeltas[nHyperlinkIndex]->nFormattedTextIndex;
}












static inline float sGetCursorWidthUIC()
{

    const int32 CURSOR_WIDTH_PIXELS = 1;

    Vector2D vecCursorDimUIC;
    ConvertPixelsDimensionsToUIC( IVector2D( CURSOR_WIDTH_PIXELS, 0 ), vecCursorDimUIC );
    return vecCursorDimUIC.x;
}





void UIEdit::KeepCursorVisible()

{
    int32 nIMECursor; // DWARF local (UIEditControl.cpp:525, no location); its uses left no line rows.
















    int32 nCursorPosition = m_nCursorPosition;
    if ( !nCursorPosition )
    {

        SetScrollOffsetUIC( UITEXT_OFFSET_HORIZONTAL, 0.0f );
        if ( m_uTCFlags & ( 1 << UITCF_WORDWRAP_BIT ) )
        {
            SetTopLine( 0 );
        }
        return;
    }


    Rect2D rectTextUIC;
    GetTextRegionRectUIC( rectTextUIC );
    int32 nFormattedLength = m_tText.ToFormattedIndex( m_tText.Length() );

    TextRenderProperties tProps( GetFont(), GetFontSize(), 1.0f, GetAdvanceScalar() );

    if ( m_uTCFlags & ( 1 << UITCF_WORDWRAP_BIT ) )
    {
        int32 nLineIndex, nLineCursorOriginalIndex;
        FormattedText tCurrentLine;
        sGetCursorLineInfo( nCursorPosition, nLineIndex, nLineCursorOriginalIndex, tCurrentLine );
        int32 nTopOffscreenLines = GetTopLine();
        int32 nNumVisibleLines = GetNumVisibleLines();
        // line 573 col 37 is the '+': the sum is the left operand of the compare.
        if ( nLineIndex < nTopOffscreenLines )
        {
            SetTopLine( nLineIndex );
        }
        else if ( nTopOffscreenLines + nNumVisibleLines <= nLineIndex )
        {
            SetTopLine( nLineIndex - nNumVisibleLines + 1 );
        }
    }
    else
    {


        float flFullTextWidthUIC = rectTextUIC.Width();


        Vector2D tFullTextSizeUIC;
        FontStringGetRenderedSizeUIC( tProps, m_tText, tFullTextSizeUIC );

        if ( tFullTextSizeUIC.x < flFullTextWidthUIC )
        {

            SetScrollOffsetUIC( UITEXT_OFFSET_HORIZONTAL, 0.0f );
            return;
        }

        float flCursorWidthUIC = sGetCursorWidthUIC();


        Vector2D tTextSizeUIC;
        FormattedText tSizeTest( m_tText, TextPosToOriginalTextPos( nCursorPosition ) );
        FontStringGetRenderedSizeUIC( tProps, tSizeTest, tTextSizeUIC );
        float flCursorPositionUIC = tTextSizeUIC.x;

        if ( nCursorPosition == nFormattedLength )
        {
            SetScrollOffsetUIC( UITEXT_OFFSET_HORIZONTAL, flFullTextWidthUIC - flCursorPositionUIC - flCursorWidthUIC );
            return;
        }



        float flViewAreaRelCursorPosUIC = flCursorPositionUIC + GetScrollOffsetUIC( UITEXT_OFFSET_HORIZONTAL );

        if ( flViewAreaRelCursorPosUIC <= 0.0f )
        {
            SetScrollOffsetUIC( UITEXT_OFFSET_HORIZONTAL, -flCursorPositionUIC );
        }
        else if ( !( flViewAreaRelCursorPosUIC < flFullTextWidthUIC ) )
        {
            SetScrollOffsetUIC( UITEXT_OFFSET_HORIZONTAL, flFullTextWidthUIC - flCursorPositionUIC - flCursorWidthUIC );
        }
    }





}




static inline float sGetCapWidth( const SNO snoTexture, const float flWindowHeight )
{
    IRect2D rectTexturePixels;
    TextureGetFrameRectPixels( rectTexturePixels, snoTexture, (uint32)-1 );
    float flAspectRatio = (float)rectTexturePixels.Width() / (float)rectTexturePixels.Height();
    return flAspectRatio * flWindowHeight;
}








static inline void sDrawPiece( const SNO snoTexture, const SNO snoShader, const Rect2D & wrPiece, const Rect2D & rectTextureUVs, const RGBAColor & rgbaColor, float flZ, const int32 nRotations )
{
    Vector2D wpUVs[4];
    Rect2D wrScreen;
    ConvertUICToPixels( wrPiece, wrScreen );
    rectTextureUVs.ToPoints( wpUVs, nRotations );
    SpriteDrawSimple( snoTexture, snoShader, wrScreen, wpUVs, rgbaColor, crgbaBlack, flZ, 1.0f, RENDERLAYER_INVALID );
}













void UIEdit::sDrawBackground( const uint8, const RGBAColor & rgbaColor, float flZ )
{
    Rect2D wrWindow;
    GetWindowRect( wrWindow );
    SNO snoTexCenter = GlobalSNOGet( GLOBALSNO_TEXTURE_EDITTEXT_CENTER );
    SNO snoTexLeftCap = GlobalSNOGet( GLOBALSNO_TEXTURE_EDITTEXT_LEFTCAP );
    SNO snoTexRightCap = GlobalSNOGet( GLOBALSNO_TEXTURE_EDITTEXT_RIGHTCAP );
    const SNO snoShader = GlobalSNOGet( GLOBALSNO_SHADER_SPRITE_TRANSPARENT_CLAMP );
    Rect2D rectTextureUVs( 0.0f, 0.0f, 1.0f, 1.0f );
    float flTopBottomIndent = wrWindow.Height() * TOP_BOTTOM_INDENT_FRAC;
    wrWindow.top -= flTopBottomIndent;
    wrWindow.bottom += flTopBottomIndent;

    Rect2D wrPiece;

    wrPiece.Set( wrWindow.left - m_flIndentLeft, wrWindow.top, wrWindow.left, wrWindow.bottom );
    sDrawPiece( snoTexLeftCap, snoShader, wrPiece, rectTextureUVs, rgbaColor, flZ, 0 );

    wrPiece.Set( wrWindow.left, wrWindow.top, wrWindow.right, wrWindow.bottom );
    sDrawPiece( snoTexCenter, snoShader, wrPiece, rectTextureUVs, rgbaColor, flZ, 0 );

    wrPiece.Set( wrWindow.right, wrWindow.top, wrWindow.right + m_flIndentRight, wrWindow.bottom );
    sDrawPiece( snoTexRightCap, snoShader, wrPiece, rectTextureUVs, rgbaColor, flZ, 0 );
}










void UIEdit::GetCursorRectUIC(
    Rect2D & rectCursorUIC )
{

    Rect2D rectTextRegionUIC;
    GetTextRegionRectUIC( rectTextRegionUIC );

    float flTextOffsetUIC = GetScrollOffsetUIC( UITEXT_OFFSET_HORIZONTAL );

    int32 nLineIndex, nLineCursorOriginalIndex;
    FormattedText tCurrentLine;
    sGetCursorLineInfo( m_nCursorPosition, nLineIndex, nLineCursorOriginalIndex, tCurrentLine );

    TextRenderProperties tProps( GetFont(), GetFontSize(), 1.0f, GetAdvanceScalar() );
    Vector2D tTextSizeUIC;
    FormattedText tSizeTest( tCurrentLine, nLineCursorOriginalIndex );
    FontStringGetRenderedSizeUIC( tProps, tSizeTest, tTextSizeUIC );


    float flCursorLeft;
    float flCursorRight;
    float flCursorWidthUIC = sGetCursorWidthUIC();
    switch ( m_eTextJustify )
    {
    default:
        // line 729: columns 46/51/66 give the operand order; the offset add is tail-merged (line 0).
        {
            flCursorLeft = rectTextRegionUIC.left + tTextSizeUIC.x + flTextOffsetUIC;
            flCursorRight = flCursorLeft + flCursorWidthUIC;
        }
        break;
    case UIJH_CENTER:
        {
            Vector2D tFullTextSizeUIC;
            FontStringGetRenderedSizeUIC( tProps, tCurrentLine, tFullTextSizeUIC );
            // line 738: columns 46/51/69/84/107/115/134/137 give the operand order.
            flCursorLeft = rectTextRegionUIC.left + flTextOffsetUIC + tTextSizeUIC.x + ( rectTextRegionUIC.Width() - tFullTextSizeUIC.x ) * 0.5f;
            flCursorRight = flCursorLeft + flCursorWidthUIC;
        }
        break;

    case UIJH_RIGHT:
        {
            Vector2D tFullTextSizeUIC;
            FontStringGetRenderedSizeUIC( tProps, tCurrentLine, tFullTextSizeUIC );

            flCursorRight = rectTextRegionUIC.right - flTextOffsetUIC - tFullTextSizeUIC.x + tTextSizeUIC.x;
            flCursorLeft = flCursorRight - flCursorWidthUIC;
        }
        break;
    }

    float flCursorHeightUIC = FontGetCursorHeightUIC( tProps );
    float flCursorTop = rectTextRegionUIC.top;
    if ( m_uTCFlags & ( 1 << UITCF_WORDWRAP_BIT ) )
    {
        float flLineHeightUIC = FontGetLineAdvanceUIC( tProps );
        flCursorTop += ( ( nLineIndex - GetTopLine() ) * flLineHeightUIC ) - GetScrollOffsetUIC( UITEXT_OFFSET_VERTICAL );
    }
    else
    {
        if ( m_eTextJustifyVertical == UIJV_LINE_MIDDLE || // line 763 col 56 is this first '||': UIJV_LINE_MIDDLE comes first.
             m_eTextJustifyVertical == UIJV_MIDDLE ||
             m_eTextJustifyVertical == UIJV_ASCENDER_MIDDLE )
        {

            flCursorTop += ( rectTextRegionUIC.Height() - flCursorHeightUIC ) * 0.5f;
        }
    }
    float flCursorBottom = flCursorTop + flCursorHeightUIC;

    rectCursorUIC.Set( flCursorLeft, flCursorTop, flCursorRight, flCursorBottom );
}




inline void UIEdit::sDrawCursor( const uint8 bAlpha, const Rect2D & rectClippingUIC, float flZ )
{
    Rect2D rectCursorUIC;
    GetCursorRectUIC( rectCursorUIC );

    Rect2D rectClippingPixels;
    ConvertUICToPixels( rectClippingUIC, rectClippingPixels );
    Rect2D rectCursorPixels;
    ConvertUICToPixels( rectCursorUIC, rectCursorPixels );
    if ( rectCursorPixels.right <= rectCursorPixels.left )
        rectCursorPixels.right = rectCursorPixels.left + 1.0f;

    Animation2DDraw( m_snoCursor, m_tCursorAnimState, rectCursorPixels, rectClippingPixels, bAlpha, flZ, crgbaWhite, SNO_NONE, m_eRenderLayerOverride );
}
































































































void UIEdit::sDraw( const uint8 bLayerAlpha, float & flZSlice )
{
    Rect2D rectTextRegionUIC;
    uint8 bAlphaCascaded = MultiplyAlpha( bLayerAlpha, GetWindowColor().a );
    if ( bAlphaCascaded == 0 )
        return;

    if ( m_eEditBoxStyle != EDITSTYLE_NO_BACKGROUND )
    {
        float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
        flZSlice += 1.0f;
        sDrawBackground( bLayerAlpha, GetWindowColor(), flZ );
    }
    else
    {
        UIWindow::Draw( bLayerAlpha, flZSlice );
    }

    const Rect2D & rectClippingUIC = GetCascadedClippingRect();
    GetTextRegionRectUIC( rectTextRegionUIC );

    if ( UI_GetKeyboardFocus() == GetID() && IsEnabled() )
    {
        uint8 bSelectionAlphaScale = 255;
        Rect2D rectSelectionUIC;
        DrawSelection( MultiplyAlpha( bLayerAlpha, bSelectionAlphaScale ), flZSlice, rectClippingUIC, rectTextRegionUIC, &rectSelectionUIC );

        float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice++;
        sDrawCursor( bAlphaCascaded, rectClippingUIC, flZ );
    }

    DrawText( bLayerAlpha, flZSlice );
}









































inline void UIEdit::sAdjustSelection( const int32 nOldCursorPos, const int32 nNewCursorPos, const BOOL fShiftDown, const BOOL fExpandingLeft )
{
    if ( !fShiftDown )
    {
        ClearSelection();
        return;
    }

    int32 nSelectionBegin;
    int32 nSelectionEnd;
    GetSelection( nSelectionBegin, nSelectionEnd );

    if ( nSelectionBegin == -1 )
    {
        SetSelection( nNewCursorPos, nOldCursorPos );
        return;
    }

    if ( fExpandingLeft )
    {
        if ( nSelectionBegin <= nNewCursorPos )
            nSelectionEnd = nNewCursorPos;
        nSelectionBegin = Math::Min( nSelectionBegin, nNewCursorPos );
    }
    else
    {
        if ( nSelectionEnd >= nNewCursorPos )
            nSelectionBegin = nNewCursorPos;
        nSelectionEnd = Math::Max( nSelectionEnd, nNewCursorPos );
    }

    SetSelection( nSelectionBegin, nSelectionEnd );
}












// NON_MATCHING: 688 vs 684 bytes. Only nNewPosition differs: the original keeps the three `nNewPosition +=` adds (1060, 1081, 1087) in their blocks; ours folds them into one add of a phi in the loop latch. Inferred from cols 1074:17/48/52/98 and 1086/1087:17, 1090:13: line 1074 is `if (!valid || !CountUTF8Chars(...)) continue;` and 1079-1091 sit one level up (this restored the hyperlink-only loop copy; it was 568). Tried avail/remaining order at 1089-1091.
int32 UIEdit::sValidateString(
    LPSTR lpuszString,
    int32 nAvailableSpace,
    int32 nRemainingCharacters )
{
    CHAR uszBuffer[MAX_UI_TEXT_LEN];
    int32 nNewPosition = 0;
    int32 nOldLength = (int32)strlen( lpuszString );

    // inferred: the -1 -> MAX_UI_TEXT_LEN select has no line row of its own (line 0, sunk into the loop preheader).
    if ( nRemainingCharacters == -1 )
    {
        nRemainingCharacters = MAX_UI_TEXT_LEN;
    }






    LPCSTR pszPos = lpuszString;
    for ( LPCSTR puszEnd = lpuszString + nOldLength; pszPos < puszEnd; )
    {
        LPCSTR pszPrevPos = pszPos;
        if ( HyperlinkAdvancePastHyperlink( pszPos ) )
        {
            uint32 dwSize = (uint32)( pszPos - pszPrevPos );

            if ( (uint32)nAvailableSpace < dwSize )
            {
                break;
            }

            strncpy( uszBuffer + nNewPosition, pszPrevPos, dwSize + 1 );
            uint32 dwPrettyNameSize = UIText_RemoveFormattingCharacters( uszBuffer + nNewPosition, FALSE, TRUE );
            size_t nNumUTF8Chars = 0;
            if ( !Blizzard::Unicode::CountUTF8Chars( uszBuffer + nNewPosition, dwPrettyNameSize, nNumUTF8Chars ) )
            {
                uszBuffer[nNewPosition] = 0;
                continue;
            }


            if ( nRemainingCharacters < (int32)nNumUTF8Chars )
            {
                break;
            }



            strncpy( uszBuffer + nNewPosition, pszPrevPos, dwSize + 1 );
            nNewPosition += dwSize;
            nAvailableSpace -= dwSize;
            nRemainingCharacters -= (int32)nNumUTF8Chars;
        }
        else
        {
            uint32 nCodepoint = Blizzard::Unicode::GetCodepointFromUTF8( &pszPos );
            uint32 dwSize = (uint32)( pszPos - pszPrevPos );
            if ( nCodepoint == 0 || nRemainingCharacters == 0 || (uint32)nAvailableSpace < dwSize )
            {
                break;
            }

            size_t nNumUTF8Chars = 0;
            if (!sCodepointIsValid(nCodepoint) || !Blizzard::Unicode::CountUTF8Chars(pszPrevPos, dwSize, nNumUTF8Chars))
            {
                continue;
            }

            if ( nCodepoint == '\n' && !( m_dwFlags & ( 1 << EDIT_FLAG_ACCEPTS_RETURN ) ) )
            {
                uszBuffer[nNewPosition++] = ' ';
            }
            else
            {

                SigmaMemoryCopy( uszBuffer + nNewPosition, pszPrevPos, dwSize );
                nNewPosition += dwSize;
            }

            --nRemainingCharacters;
            nAvailableSpace -= dwSize;

        }
    }
    uszBuffer[nNewPosition] = 0;


    strcpy( lpuszString, uszBuffer );
    return nNewPosition;
}



inline BOOL UIEdit::InsertAtCursor( LPCSTR lpuszNewCharacters, BOOL fPasteHyperlink )
{
    return InsertAtCursorInternal( lpuszNewCharacters, FALSE, fPasteHyperlink );
}




// X/Common/Hyperlink.cpp:402 (not declared by src/X/Common/Hyperlink.h; signature from the DWARF)
BOOL EscapeHyperlink( char * pszEscapedBuffer, LPCSTR szOriginalStr, size_t nEscapedBufferSize );


BOOL UIEdit::InsertAtCursorInternal(
    LPCSTR lpuszNewCharacters,
    BOOL fShowingComposition,
    BOOL fPasteHyperlink)
{
    if ( !lpuszNewCharacters )
        return FALSE;



    int32 nOriginalInsertLength = (int32)strlen( lpuszNewCharacters );
    if ( nOriginalInsertLength == 0 )
        return FALSE;





    if ( m_nUTF8Length == m_nMaximumAllowedCharacters )
    {
        if ( m_pfnMaximumAllowedCharactersReached )
        {
            m_pfnMaximumAllowedCharactersReached( GetID() );
        }
        return FALSE;
    }



    int32 nMaxSpace = ( m_nMaximumAllowedBytes == -1 ) ? MAX_UI_TEXT_LEN - 1 : m_nMaximumAllowedBytes - 1;
    if ( !fShowingComposition )
    {
        // Min operand order from the csel (nMaxSpace first); line 1149
        nMaxSpace = ( m_nMaximumAllowedCharacters == -1 ) ? nMaxSpace : Math::Min( nMaxSpace, m_nMaximumAllowedCharacters * 4 );
    }


    int32 nFullLength = m_tText.Length();

    int32 nAvailableSpace = nMaxSpace - nFullLength;
    if ( nAvailableSpace <= 0 )
        return FALSE;




    CHAR uszBuffer[MAX_UI_TEXT_LEN];




    if ( fPasteHyperlink || ( m_uTCFlags & ( 1 << UITCF_ISPASSWORD_BIT ) ) )
    {
        Blizzard::String::Copy( uszBuffer, lpuszNewCharacters, sizeof( uszBuffer ) );
    }
    else
    {
        EscapeHyperlink( uszBuffer, lpuszNewCharacters, sizeof( uszBuffer ) );
    }


    const char * pszPotentialHyperlink = uszBuffer;
    if ( uszBuffer[0] == ' ' )
    {
        pszPotentialHyperlink = uszBuffer + 1;
    }
    HyperlinkType eHLType = HyperlinkParseType( pszPotentialHyperlink, FALSE );

    uszBuffer[nAvailableSpace] = 0;
    int32 nInsertLength = sValidateString( uszBuffer, nAvailableSpace, ( !fShowingComposition && m_nMaximumAllowedCharacters != -1 ) ? m_nMaximumAllowedCharacters - m_nUTF8Length : -1 );
    if ( nInsertLength == 0 )
        return FALSE;


    nInsertLength = Math::Min( nInsertLength, nAvailableSpace );

    if ( eHLType != HLT_NONE && nInsertLength != nOriginalInsertLength )
        return FALSE;











    CHAR uszTempBuffer[MAX_UI_TEXT_LEN] = { 0 };


    if ( !fShowingComposition && m_nMaximumAllowedCharacters != -1 )
    {

        int32 nOriginalInsertCharCount;
        {
            size_t nUTF8Count = 0;
            Blizzard::String::Copy( uszTempBuffer, uszBuffer, sizeof( uszTempBuffer ) );
            UIText_RemoveFormattingCharacters( uszTempBuffer, FALSE, TRUE );
            nOriginalInsertCharCount = Blizzard::Unicode::CountUTF8Chars( uszTempBuffer, nInsertLength, nUTF8Count )
                ?
                (int32)nUTF8Count : 0;
        }

        if ( m_nUTF8Length + nOriginalInsertCharCount > m_nMaximumAllowedCharacters )
            return FALSE;
    }





    LPCSTR pszCurrentText = m_tText.GetText();
    Blizzard::String::Copy( uszTempBuffer, pszCurrentText, sizeof( uszTempBuffer ) );



    if ( IsInHyperlink( m_nCursorPosition, FALSE, FALSE ) != -1 )
        return FALSE;




    int32 nConvertedCursorPos = TextPosToOriginalTextPos( m_nCursorPosition );




    if ( nConvertedCursorPos == nFullLength )
    {
        strncat_safe( uszTempBuffer, sizeof( uszTempBuffer ), uszBuffer, nInsertLength );
    }
    else if ( m_nCursorPosition == 0 )
    {
        strncpy( uszTempBuffer, uszBuffer, nInsertLength );
        Blizzard::String::Copy( uszTempBuffer + nInsertLength, pszCurrentText, sizeof( uszTempBuffer ) - nInsertLength );
    }
    else
    {
        strncpy( uszTempBuffer, pszCurrentText, nConvertedCursorPos );
        Blizzard::String::Copy( uszTempBuffer + nConvertedCursorPos, uszBuffer, sizeof( uszTempBuffer ) - nConvertedCursorPos );
        int32 nTempLen = (int32)strlen( uszTempBuffer );
        Blizzard::String::Copy( uszTempBuffer + nTempLen, pszCurrentText + nConvertedCursorPos, sizeof( uszTempBuffer ) - nTempLen );
    }




    BOOL fAtEnd = ( nConvertedCursorPos == nFullLength );
    nConvertedCursorPos += nInsertLength;

    if ( m_dwFlags & ( 1 << EDIT_FLAG_NUMBERS_ONLY_BIT ) )
    {
        PerformNumberFormatting( uszTempBuffer, (uint32)strlen( uszTempBuffer ), nConvertedCursorPos );
    }


    BOOL fAllowInsert = TRUE;
    if ( m_pfnStringWouldBeValid && !fShowingComposition )
    {
        fAllowInsert = m_pfnStringWouldBeValid( uszTempBuffer );
    }
    if ( !fAllowInsert )
        return FALSE;



    m_nCursorPosition = 0;

    SetText( uszTempBuffer, FALSE );




    if ( uszTempBuffer[0] != '/' || ( (LPCSTR)GetText() )[0] == uszTempBuffer[0] )
    {



        int32 nNewCursorPosition = OriginalTextPosToTextPos( nConvertedCursorPos );
        SetCursorPosition( nNewCursorPosition );
    }



    if ( fAtEnd )
    {
        m_nCursorPosition = OriginalTextPosToTextPos( m_tText.Length() );
    }



    return fAllowInsert;
}



// `inline` here and on SetCursorPosition/InsertAtCursor/helpers below: the DWARF has only inlined instances, no concrete out-of-line DIE
inline void UIEdit::sPasteFromClipboard()
{

    CHAR uzText[MAX_UI_TEXT_LEN];
    SigmaMemoryClear( uzText, sizeof( uzText ) );
    if ( InputPlatformGetClipboard( uzText, sizeof( uzText ) ) )
    {
        // m_pfnInputWouldBeValid test first, then the call (line 1323 columns 13 and 40)
        if ( !m_pfnInputWouldBeValid || m_pfnInputWouldBeValid( uzText ) )
        {




            BOOL fDeleteOkay = DeleteSelection();
            if ( fDeleteOkay )
            {


                InsertAtCursor( uzText, FALSE );
            }

            if ( m_pfnOnChange )
                m_pfnOnChange( GetID() );
        }
    }
}







BOOL UIEdit::sHandleMouseMove( const BufferedInput & tInput )
{
    if ( !m_bDragging )
        return FALSE;

    Vector2D vecMouseUIC;
    ConvertPixelsToUIC( tInput.tBufMouseData.p, vecMouseUIC );

    Rect2D rectTextExtentsUIC;
    GetTextRegionRectUIC( rectTextExtentsUIC );

    if ( ( m_uTCFlags & ( 1 << UITCF_WORDWRAP_BIT ) ) && vecMouseUIC.y < rectTextExtentsUIC.top )
    {
        int32 nTopLine = GetTopLine();
        if ( nTopLine > 0 )
            SetTopLine( nTopLine - 1 );
    }

    int32 nNewCursorPos = ScreenPtToCursorPos( vecMouseUIC );
    SetSelection( m_nDragStartCursorPos, nNewCursorPos );
    SetCursorPosition( nNewCursorPos );
    return TRUE;
}







inline BOOL UIEdit::sHandleLeftMouseDown(
    const BufferedInput & tInput )
{
    return HandleLeftMouseDown( tInput );
}













void UIEdit::sHandleKeyBackspace( const BOOL fCtrlDown )
{
    LPCSTR pszCurrentText = m_tText.GetText();
    int32 nLen = m_tText.Length();
    const UIID & id = GetID();

    if ( nLen > 0 )
    {
        if ( HasSelection() )
        {
            DeleteSelection();
        }
        else if ( m_nCursorPosition > 0 )
        {
            CHAR uszTemp[MAX_UI_TEXT_LEN];
            int32 nCursorPosition = m_nCursorPosition;
            int32 nHyperlinkIndex = IsInHyperlink( nCursorPosition, FALSE, FALSE );
            if ( nHyperlinkIndex != -1 )
            {
                const FormattedTextDelta * ptDelta = GetText().m_arTextDeltas[nHyperlinkIndex];
                nCursorPosition = ptDelta->nFormattedTextIndex + ptDelta->nFormattedTextLength;
            }

            int32 nNewPos = fCtrlDown ? m_tText.GetPrevFormattedWord( nCursorPosition ) : m_tText.GetPrevFormattedChar( nCursorPosition );
            int32 nConvertedOldPos = m_tText.ToOriginalIndex( nCursorPosition );
            int32 nConvertedNewPos = m_tText.ToOriginalIndex( nNewPos );

            memset( uszTemp, 0, sizeof( uszTemp ) );
            Blizzard::String::Copy( uszTemp, pszCurrentText, nConvertedNewPos + 1 );
            strcat_safe( uszTemp, sizeof( uszTemp ), pszCurrentText + nConvertedOldPos );

            if ( m_dwFlags & ( 1 << EDIT_FLAG_NUMBERS_ONLY_BIT ) )
                PerformNumberFormatting( uszTemp, (uint32)strlen( uszTemp ), nConvertedNewPos );

            if ( !m_pfnStringWouldBeValid || m_pfnStringWouldBeValid( uszTemp ) )
            {
                m_nCursorPosition = 0;
                SetText( uszTemp, FALSE );
                int32 nNewPosScoped = OriginalTextPosToTextPos( nConvertedNewPos );
                SetCursorPosition( nNewPosScoped );
            }
        }
    }

    if ( m_pfnOnChange )
        m_pfnOnChange( id );
}























void UIEdit::sHandleKeyDelete( const BOOL fCtrlDown )
{
    const UIID & id = GetID();
    if ( HasSelection() )
    {
        DeleteSelection();
        if ( m_pfnOnChange )
            m_pfnOnChange( id );
        return;
    }

    LPCSTR pszCurrentText = m_tText.GetText();
    int32 nLen = m_tText.Length();
    int32 nCursorPosition = m_nCursorPosition;
    int32 nHyperlinkIndex = IsInHyperlink( nCursorPosition, FALSE, FALSE );
    if ( nHyperlinkIndex != -1 )
    {
        const FormattedTextDelta * ptDelta = GetText().m_arTextDeltas[nHyperlinkIndex];
        nCursorPosition = ptDelta->nFormattedTextIndex;
    }

    int32 nConvertedOldPos = m_tText.ToOriginalIndex( nCursorPosition );
    if ( nLen > 0 && nConvertedOldPos < nLen )
    {
        CHAR uszTemp[MAX_UI_TEXT_LEN];
        int32 nNewPos = fCtrlDown ? m_tText.GetNextFormattedWord( nCursorPosition ) : m_tText.GetNextFormattedChar( nCursorPosition );
        int32 nConvertedNewPos = m_tText.ToOriginalIndex( nNewPos );

        memset( uszTemp, 0, sizeof( uszTemp ) );
        Blizzard::String::Copy( uszTemp, pszCurrentText, nConvertedOldPos + 1 );
        strcat_safe( uszTemp, sizeof( uszTemp ), pszCurrentText + nConvertedNewPos );

        if ( m_dwFlags & ( 1 << EDIT_FLAG_NUMBERS_ONLY_BIT ) )
            PerformNumberFormatting( uszTemp, (uint32)strlen( uszTemp ), nConvertedOldPos );

        if ( m_pfnStringWouldBeValid && !m_pfnStringWouldBeValid( uszTemp ) )
            return;

        m_nCursorPosition = 0;
        SetText( uszTemp, FALSE );
        {
            int32 nNewCursorPosition = OriginalTextPosToTextPos( nConvertedOldPos );
            SetCursorPosition( nNewCursorPosition );
        }
        ClearSelection();

        if ( m_pfnOnChange )
            m_pfnOnChange( id );
    }
}
























void UIEdit::sHandleKeyLeft( const BOOL fShiftDown, const BOOL fCtrlDown )
{
    int32 nOldPos = m_nCursorPosition;
    if ( nOldPos <= 0 )
    {
        if ( !fShiftDown )
            ClearSelection();
        return;
    }

    int32 nNewPos;
    if ( fCtrlDown )
        nNewPos = m_tText.GetPrevFormattedWord( nOldPos );
    else if ( !fShiftDown && HasSelection() )
        nNewPos = m_nSelectionBegin;
    else
        nNewPos = m_tText.GetPrevFormattedChar( nOldPos );

    int32 nHyperlinkIndex = IsInHyperlink( nNewPos, TRUE, FALSE );
    if ( nHyperlinkIndex != -1 )
        nNewPos = m_tText.m_arTextDeltas[nHyperlinkIndex]->nFormattedTextIndex;

    SetCursorPosition( nNewPos );
    sAdjustSelection( nOldPos, m_nCursorPosition, fShiftDown, TRUE );
}























void UIEdit::sHandleKeyRight( const BOOL fShiftDown, const BOOL fCtrlDown )
{
    int32 nFormattedLen = m_tText.ToFormattedIndex( m_tText.Length() );
    int32 nOldPos = m_nCursorPosition;
    if ( nOldPos >= nFormattedLen )
    {
        if ( !fShiftDown )
            ClearSelection();
        return;
    }

    int32 nNewPos;
    if ( fCtrlDown )
        nNewPos = m_tText.GetNextFormattedWord( nOldPos );
    else if ( !fShiftDown && HasSelection() )
        nNewPos = m_nSelectionEnd;
    else
        nNewPos = m_tText.GetNextFormattedChar( nOldPos );

    int32 nHyperlinkIndex = IsInHyperlink( nNewPos, FALSE, FALSE );
    if ( nHyperlinkIndex != -1 )
    {
        const FormattedTextDelta * pDelta = m_tText.m_arTextDeltas[nHyperlinkIndex];
        nNewPos = pDelta->nFormattedTextIndex + pDelta->nFormattedTextLength;
    }

    SetCursorPosition( nNewPos );
    sAdjustSelection( nOldPos, m_nCursorPosition, fShiftDown, FALSE );
}























BOOL UIEdit::sHandleKeyUp(
    const BOOL fShiftDown,
    const BOOL fCtrlDown )
{
    if ( fCtrlDown || !( m_uTCFlags & ( 1 << UITCF_WORDWRAP_BIT ) ) )
    {
        return FALSE;
    }

    if ( m_nCursorPosition <= 0 )
    {
        if ( !fShiftDown )
        {
            ClearSelection();
        }
        return TRUE;
    }

    int32 nLineIndex, nLineCursorOriginalIndex;
    FormattedText tCursorLine;
    sGetCursorLineInfo( m_nCursorPosition, nLineIndex, nLineCursorOriginalIndex, tCursorLine );
    int32 nOldPos = m_nCursorPosition;
    int32 nNewPos = 0;
    if ( nLineIndex != 0 )
    {






        TextRenderProperties tProps( GetFont(), GetFontSize(), 1.0f, GetAdvanceScalar() );
        float flLineHeight = FontGetLineAdvanceUIC( tProps );
        int32 nTopLine = GetTopLine();
        if ( nTopLine == nLineIndex )
        {
            SetTopLine( nLineIndex - 1 );
        }
        Rect2D rectCursorUIC;
        GetCursorRectUIC( rectCursorUIC );

        Vector2D vecNewCursorPos( rectCursorUIC.left + rectCursorUIC.Width() * 0.5f, rectCursorUIC.top - 0.5f * flLineHeight );
        nNewPos = ScreenPtToCursorPos( vecNewCursorPos );

        int32 nNewLineIndex, nNewLineCursorOriginalIndex;
        FormattedText tLineAtNewPos;
        sGetCursorLineInfo( nNewPos, nNewLineIndex, nNewLineCursorOriginalIndex, tLineAtNewPos );

        LPCSTR szOriginalText = m_tText.GetText();
        intptr nTemp = tLineAtNewPos.GetText() - szOriginalText;
        int32 nPositionAtStartOfLine = OriginalTextPosToTextPos( (int32)nTemp );







        if ( nLineIndex == nNewLineIndex &&
             nNewPos == nPositionAtStartOfLine && nNewLineIndex > 0 )
        {


            const FormattedText & prevLine = m_artTextLines[nLineIndex - 1];
            LPCSTR szStartOfLine = prevLine.GetText();
            int32 nFormattedIndexEndOfLine = prevLine.ToFormattedIndex( prevLine.Length() );
            int32 prevCharFormattedIndexFromStartOfLine = prevLine.GetPrevFormattedChar( nFormattedIndexEndOfLine );
            int32 prevCharOriginalIndexFromStartOfLine = prevLine.ToOriginalIndex( prevCharFormattedIndexFromStartOfLine );
            size_t width = Blizzard::Unicode::GetUTF8CharWidth( szStartOfLine + prevCharOriginalIndexFromStartOfLine );
            if ( width == 1 && isspace( szStartOfLine[prevCharOriginalIndexFromStartOfLine] ) )
            {



                nTemp = ( szStartOfLine - m_tText.GetText() ) + prevLine.Length() - 1;
                nNewPos = OriginalTextPosToTextPos( (int32)nTemp );
                int32 nNewPosOriginal = TextPosToOriginalTextPos( nNewPos );
                if ( szOriginalText[nNewPosOriginal] == '\r' && szOriginalText[nNewPosOriginal + 1] == '\n' ) nNewPos--;
            }
        }




    }

    if ( !fShiftDown )
    {
        ClearSelection();
    }

    int32 nHyperlinkIndex = IsInHyperlink( nNewPos, TRUE, FALSE );
    if ( nHyperlinkIndex != -1 )
    {

        const FormattedTextDelta * pDelta = m_tText.m_arTextDeltas[nHyperlinkIndex];
        nNewPos = pDelta->nFormattedTextIndex + pDelta->nFormattedTextLength;
    }

    SetCursorPosition( nNewPos );
    sAdjustSelection( nOldPos, m_nCursorPosition, fShiftDown, FALSE );
    return TRUE;
}




BOOL UIEdit::sHandleKeyDown(
    const BOOL fShiftDown,
    const BOOL fCtrlDown)
{
    if ( fCtrlDown || !( m_uTCFlags & ( 1 << UITCF_WORDWRAP_BIT ) ) )
        return FALSE;



    int32 nFormattedTextLength = OriginalTextPosToTextPos( m_tText.Length() );

    if ( m_nCursorPosition >= nFormattedTextLength )
    {
        if ( !fShiftDown )
        {
            ClearSelection();
        }
        return TRUE;
    }

    int32 nLineIndex, nLineCursorOriginalIndex;
    FormattedText tCursorLine;
    sGetCursorLineInfo( m_nCursorPosition, nLineIndex, nLineCursorOriginalIndex, tCursorLine );
    int32 nOldPos = m_nCursorPosition;
    int32 nNewPos = nFormattedTextLength;

    if ( nLineIndex != GetLineCount() - 1 )
    {



        Rect2D rectTextExtentsUIC;
        GetTextRegionRectUIC( rectTextExtentsUIC );
        TextRenderProperties tProps( GetFont(), GetFontSize(), 1.0f, GetAdvanceScalar() );

        ScopedFontDefinitionLockR pdefFont( tProps.snoFont );
        FontPointSizeData * ptPointSize = pdefFont->GetPointSizeData( tProps.nFontSize );

        float flLineHeight = FontGetLineAdvanceUIC( tProps, ptPointSize );

        Rect2D rectCursorUIC;
        GetCursorRectUIC( rectCursorUIC );

        float flVerticalOffset = sJustifyVertical( ptPointSize, rectTextExtentsUIC, IsTruncated() ? m_tTruncatedText : m_tText );
        Vector2D vecNewCursorPos( rectCursorUIC.left + rectCursorUIC.Width() * 0.5f, rectCursorUIC.top + flLineHeight + 0.5f + flVerticalOffset );
        nNewPos = ScreenPtToCursorPos( vecNewCursorPos );

        int32 nNewLineIndex, nNewLineCursorOriginalIndex;
        FormattedText tLineAtNewPos;
        sGetCursorLineInfo( nNewPos, nNewLineIndex, nNewLineCursorOriginalIndex, tLineAtNewPos );

        LPCSTR szOriginalText = m_tText.GetText();
        intptr nTemp = tLineAtNewPos.GetText() - szOriginalText;
        int32 nPositionAtStartOfLine = OriginalTextPosToTextPos( (int32)nTemp );






        if ( nNewLineIndex - nLineIndex > 1 &&
             nNewPos == nPositionAtStartOfLine && nNewLineIndex > 0 )
        {


            const FormattedText & prevLine = m_artTextLines[nNewLineIndex - 1];
            LPCSTR szStartOfLine = prevLine.GetText();
            int32 nFormattedIndexEndOfLine = prevLine.ToFormattedIndex( prevLine.Length() );
            int32 prevCharFormattedIndexFromStartOfLine = prevLine.GetPrevFormattedChar( nFormattedIndexEndOfLine );
            int32 prevCharOriginalIndexFromStartOfLine = prevLine.ToOriginalIndex( prevCharFormattedIndexFromStartOfLine );
            size_t width = Blizzard::Unicode::GetUTF8CharWidth( szStartOfLine + prevCharOriginalIndexFromStartOfLine );
            if ( width == 1 && isspace( szStartOfLine[prevCharOriginalIndexFromStartOfLine] ) )
            {



                nTemp = ( szStartOfLine - m_tText.GetText() ) + prevLine.Length() - 1;
                nNewPos = OriginalTextPosToTextPos( (int32)nTemp );
                int32 nNewPosOriginal = TextPosToOriginalTextPos( nNewPos );
                if ( szOriginalText[nNewPosOriginal] == '\r' ) nNewPos -= szOriginalText[nNewPosOriginal + 1] == '\n';
            }
        }




    }

    if ( !fShiftDown )
    {
        ClearSelection();
    }

    int32 nHyperlinkIndex = IsInHyperlink( nNewPos, FALSE, FALSE );
    if ( nHyperlinkIndex != -1 )
    {

        const FormattedTextDelta * pDelta = m_tText.m_arTextDeltas[nHyperlinkIndex];
        nNewPos = pDelta->nFormattedTextIndex + pDelta->nFormattedTextLength;
    }

    SetCursorPosition( nNewPos );
    sAdjustSelection( nOldPos, m_nCursorPosition, fShiftDown, FALSE );
    return TRUE;
}




void UIEdit::sHandleKeyHome( const BOOL fShiftDown, const BOOL fCtrlDown )
{
    int32 nTargetCursorPosition = 0;
    if ( !fCtrlDown )
    {
        FormattedText tLineAtCursor;
        int32 nLineIndex;
        int32 nLineCursorOriginalIndex;
        sGetCursorLineInfo( m_nCursorPosition, nLineIndex, nLineCursorOriginalIndex, tLineAtCursor );
        intptr nTemp = tLineAtCursor.GetText() - m_tText.GetText();
        nTargetCursorPosition = OriginalTextPosToTextPos( (int32)nTemp );
    }

    if ( fShiftDown )
    {
        if ( HasSelection() )
        {
            int32 nOldStartSelection;
            int32 nOldEndSelection;
            GetSelection( nOldStartSelection, nOldEndSelection );
            int32 nNewEndSelection = m_nCursorPosition < nOldEndSelection ? nOldEndSelection : nOldStartSelection;
            SetSelection( nTargetCursorPosition, nNewEndSelection );
        }
        else
        {
            SetSelection( nTargetCursorPosition, m_nCursorPosition );
        }
    }
    else
    {
        ClearSelection();
    }

    m_nCursorPosition = nTargetCursorPosition;
}











void UIEdit::sHandleKeyEnd( const BOOL fShiftDown, const BOOL fCtrlDown )
{
    int32 nLen = m_tText.Length();
    int32 nTargetCursorPosition = OriginalTextPosToTextPos( nLen );
    if ( !fCtrlDown )
    {
        FormattedText tLineAtCursor;
        int32 nLineIndex;
        int32 nLineCursorOriginalIndex;
        sGetCursorLineInfo( m_nCursorPosition, nLineIndex, nLineCursorOriginalIndex, tLineAtCursor );

        LPCSTR szOriginalText = m_tText.GetText();
        intptr nPositionAtStartOfLine = tLineAtCursor.GetText() - szOriginalText;
        int32 nPosEndOfLine = (int32)nPositionAtStartOfLine + tLineAtCursor.Length();

        LPCSTR szStartOfLine = tLineAtCursor.GetText();
        int32 nFormattedIndexEndOfLine = tLineAtCursor.ToFormattedIndex( tLineAtCursor.Length() );
        int32 prevCharFormattedIndexFromStartOfLine = tLineAtCursor.GetPrevFormattedChar( nFormattedIndexEndOfLine );
        int32 prevCharOriginalIndexFromStartOfLine = tLineAtCursor.ToOriginalIndex( prevCharFormattedIndexFromStartOfLine );
        size_t width = Blizzard::Unicode::GetUTF8CharWidth( szStartOfLine + prevCharOriginalIndexFromStartOfLine );
        if ( width == 1 && isspace( szStartOfLine[prevCharOriginalIndexFromStartOfLine] ) )
        {
            nPosEndOfLine--;
            if ( szOriginalText[nPosEndOfLine] == '\r' && szOriginalText[nPosEndOfLine + 1] == '\n' )
                nPosEndOfLine--;
        }

        nTargetCursorPosition = OriginalTextPosToTextPos( nPosEndOfLine );
    }

    if ( fShiftDown )
    {
        if ( HasSelection() )
        {
            int32 nOldStartSelection;
            int32 nOldEndSelection;
            GetSelection( nOldStartSelection, nOldEndSelection );
            int32 nNewStartLocation = m_nCursorPosition > nOldStartSelection ? nOldStartSelection : nOldEndSelection;
            SetSelection( nNewStartLocation, nTargetCursorPosition );
        }
        else
        {
            SetSelection( m_nCursorPosition, nTargetCursorPosition );
        }
    }
    else
    {
        ClearSelection();
    }

    m_nCursorPosition = nTargetCursorPosition;
}



















inline BOOL UIEdit::sCodepointIsValid( const uint32 nCodepoint ) const
{
    if ( nCodepoint < 0x20 || nCodepoint == 0x7f )
    {
        if ( nCodepoint == '\n' && ( m_dwFlags & ( 1 << EDIT_FLAG_ACCEPTS_RETURN ) ) )
            return TRUE;
        return FALSE;
    }

    return !( m_dwFlags & ( 1 << EDIT_FLAG_NUMBERS_ONLY_BIT ) ) || ( nCodepoint >= '0' && nCodepoint <= '9' );
}


















void UIEdit::sHandleCharacter( LPCSTR szUTF8 )
{
    LPCSTR pszTemp = szUTF8;
    uint32 nCodepoint = Blizzard::Unicode::GetCodepointFromUTF8( &pszTemp );
    if ( !sCodepointIsValid( nCodepoint ) )
        return;

    uint32 dwSizeToAdd = (uint32)( pszTemp - szUTF8 );
    int32 nLen = m_tText.Length();
    if ( nLen + dwSizeToAdd >= MAX_UI_TEXT_LEN )
        return;

    if ( m_pfnInputWouldBeValid && !m_pfnInputWouldBeValid( szUTF8 ) )
        return;

    if ( nLen > 0 && HasSelection() )
        DeleteSelection();

    BOOL fInsertAllowed = InsertAtCursor( szUTF8, FALSE );
    if ( !fInsertAllowed && m_pfnOnChange )
        m_pfnOnChange( GetID() );
}





















inline BOOL UIEdit::sIsUnhandledKey(
    const Key eKey ) const
{

    switch ( eKey )
    {
    case KEY_RETURN:
    case KEY_NUMPADENTER:
        return m_pfnOnReturnEndEdit == NULL && !AcceptsReturn();
    case KEY_ESCAPE:
    case KEY_MOUSE1:
    case KEY_MOUSE2:
    case KEY_MOUSE3:
    case KEY_MOUSE4:
    case KEY_MOUSE5:
    case KEY_MOUSEWHEELUP:
    case KEY_MOUSEWHEELDOWN:
        return TRUE;
    case KEY_TAB:
        return InputCtrlKeyIsDown( INPUTCONTEXT_DEFAULT ) || InputAltKeyIsDown( INPUTCONTEXT_DEFAULT );
    }
    return FALSE;
}






inline BOOL UIEdit::HasKeyboardFocus() const
{

    return UI_GetKeyboardFocus() == GetID();
}




BOOL UIEdit::sInput(
    const BufferedInput & tInput)
{

    KeepCursorVisible();

    Player * ptControllingPlayer = PlayerGetByControllerIndex( tInput.tBufControllerData.nControllerIndex );




    UIAction eAction = Prefs::GetUIActionFromInput( tInput );
    if ( ptControllingPlayer && eAction != ACTION_INVALID && UI_GetKeyboardFocus() == GetID() )
    {
        // 30: literal delay (no named constant found in the CU)
        CPlayerInputOtherListenerReceivedAction( ptControllingPlayer, eAction, 30 );
    }
    switch ( tInput.eInputMsg )
    {



    case INPUTMSG_KEYDOWNONCE:
        if ( ( tInput.tBufKeyData.eKey == KEY_RETURN || tInput.tBufKeyData.eKey == KEY_NUMPADENTER ) &&
             !AcceptsReturn() &&
             m_pfnOnReturnEndEdit )
        {

            m_pfnOnReturnEndEdit( GetID() );
            return TRUE;
        }
        return !sIsUnhandledKey( tInput.tBufKeyData.eKey ) && UI_GetKeyboardFocus() == GetID();
    case INPUTMSG_KEYSTILLDOWN:
    case INPUTMSG_KEYUP:
        return !sIsUnhandledKey( tInput.tBufKeyData.eKey ) && UI_GetKeyboardFocus() == GetID();

    case INPUTMSG_KEYDOWN:
    {
        if ( sIsUnhandledKey( tInput.tBufKeyData.eKey ) || UI_GetKeyboardFocus() != GetID() )
            return FALSE;



        BOOL fShiftDown = InputShiftKeyIsDown( INPUTCONTEXT_DEFAULT );
        BOOL fCtrlDown = InputCtrlKeyIsDown( INPUTCONTEXT_DEFAULT );
        BOOL fAltDown = InputAltKeyIsDown( INPUTCONTEXT_DEFAULT );

        switch ( tInput.tBufKeyData.eKey )
        {
        case KEY_BACK:
            if ( m_uTCFlags & ( 1 << UITCF_ENABLED_BIT ) )
            {
                sHandleKeyBackspace( fCtrlDown );
            }
            break;

        case KEY_DELETE:
            if ( m_uTCFlags & ( 1 << UITCF_ENABLED_BIT ) )
            {
                sHandleKeyDelete( fCtrlDown );
            }
            break;

        case KEY_LEFT:
            sHandleKeyLeft( fShiftDown, fCtrlDown );
            break;

        case KEY_RIGHT:
            sHandleKeyRight( fShiftDown, fCtrlDown );
            break;

        case KEY_UP:
            return sHandleKeyUp( fShiftDown, fCtrlDown );

        case KEY_DOWN:
            return sHandleKeyDown( fShiftDown, fCtrlDown );


        case KEY_HOME:
            sHandleKeyHome( fShiftDown, fCtrlDown );
            break;



        case KEY_END:
            sHandleKeyEnd( fShiftDown, fCtrlDown );
            break;

        case KEY_C:
            if ( fCtrlDown && !fAltDown && !fShiftDown )
            {
                if ( m_uTCFlags & ( 1 << UITCF_ENABLED_BIT ) )
                {
                    CopySelectionToClipboard();
                }
            }
            break;

        case KEY_INSERT:
            if ( fCtrlDown )
            {
                CopySelectionToClipboard();
            }
            else if ( fShiftDown )
            {
                if ( m_uTCFlags & ( 1 << UITCF_ENABLED_BIT ) )
                {
                    sPasteFromClipboard();
                }
            }
            break;


        case KEY_V:
            if ( fCtrlDown && !fAltDown && !fShiftDown )
            {
                if ( m_uTCFlags & ( 1 << UITCF_ENABLED_BIT ) )
                {
                    sPasteFromClipboard();
                }
            }
            break;

        case KEY_X:
            if ( fCtrlDown && !fAltDown && !fShiftDown )
            {
                CopySelectionToClipboard();
                if ( m_uTCFlags & ( 1 << UITCF_ENABLED_BIT ) )
                {
                    DeleteSelection();
                    if ( m_pfnOnChange )
                        m_pfnOnChange( GetID() );
                }
            }
            break;



        case KEY_A:
            if ( fCtrlDown && !fAltDown && !fShiftDown )
            {
                SetSelection( 0, 9999 );
                SetCursorPosition( 9999 );
            }
            break;


        case KEY_TAB:
        {
            BOOL fBackwards = fShiftDown;
            UI_IncrementKeyboardFocus( fBackwards );
            break;
        }


        case KEY_RETURN:
        case KEY_NUMPADENTER:
            if ( m_dwFlags & ( 1 << EDIT_FLAG_ACCEPTS_RETURN ) )
            {
                sHandleCharacter( "\n" );
            }
            break;
        }
        return TRUE;
    }




    case INPUTMSG_CHARACTER:
        if ( !m_fIgnoreNextCharMessage && ( m_uTCFlags & ( 1 << UITCF_ENABLED_BIT ) ) && UI_GetKeyboardFocus() == GetID() )
        {
            sHandleCharacter( tInput.tBufCharData.szUTF8 );
        }
        m_fIgnoreNextCharMessage = FALSE;
        return TRUE;

    case INPUTMSG_MOUSEMOVE:
        if ( sHandleMouseMove( tInput ) )
            return TRUE;
        break;



    case INPUTMSG_LEFTMOUSEDOWN:
    case INPUTMSG_LEFTMOUSEDOUBLECLICK:
        return sHandleLeftMouseDown( tInput );

    case INPUTMSG_LEFTMOUSEUP:
        if ( m_bDragging )
        {
            m_bDragging = FALSE;
            UI_SetCapture( GetUIIDNone() );
            return TRUE;
        }
        break;






    case INPUTMSG_IME_START_COMPOSITION:
    {
        if ( ( m_uTCFlags & ( 1 << UITCF_ENABLED_BIT ) ) && HasKeyboardFocus() && !( m_dwFlags & ( ( 1 << EDIT_FLAG_NUMBERS_ONLY_BIT ) | ( 1 << EDIT_FLAG_IME_DISABLED ) ) ) )
        {



            m_bIsIMEComposing = TRUE;
        }
        break;
    }

    case INPUTMSG_IME_END_COMPOSITION:
    {
        if ( ( m_uTCFlags & ( 1 << UITCF_ENABLED_BIT ) ) && HasKeyboardFocus() && !( m_dwFlags & ( ( 1 << EDIT_FLAG_NUMBERS_ONLY_BIT ) | ( 1 << EDIT_FLAG_IME_DISABLED ) ) ) )
        {



            m_bIsIMEComposing = FALSE;

            DeleteSelection();
            KeepCursorVisible();

            if ( m_pfnOnChange )
            {
                m_pfnOnChange( GetID() );
            }
        }
        break;
    }

    case INPUTMSG_IME_SHOW_COMPOSITION:
    {
        if ( ( m_uTCFlags & ( 1 << UITCF_ENABLED_BIT ) ) && HasKeyboardFocus() && !( m_dwFlags & ( ( 1 << EDIT_FLAG_NUMBERS_ONLY_BIT ) | ( 1 << EDIT_FLAG_IME_DISABLED ) ) ) )
        {



            const char * pszText = InputIMEGetCompositionString( NULL );
            if ( pszText )
            {






                BOOL fDeleteOkay = DeleteSelection();
                if ( fDeleteOkay )
                {
                    int32 nOldCursorPosition = m_nCursorPosition;
                    if ( InsertAtCursorInternal( pszText, TRUE, FALSE ) )
                    {


                        SetSelection( nOldCursorPosition, m_nCursorPosition );
                        SetCursorPosition( nOldCursorPosition );
                    }
                }
            }
        }
        break;
    }







    case INPUTMSG_IME_INSERT_RESULT:
    {
        if ( ( m_uTCFlags & ( 1 << UITCF_ENABLED_BIT ) ) && HasKeyboardFocus() && !( m_dwFlags & ( ( 1 << EDIT_FLAG_NUMBERS_ONLY_BIT ) | ( 1 << EDIT_FLAG_IME_DISABLED ) ) ) )
        {



            BOOL fDeleteOkay = DeleteSelection();
            if ( fDeleteOkay )
            {
                const char * pszText = InputIMEGetResultString();
                InsertAtCursor( pszText, FALSE );
            }

            if ( m_pfnOnChange )
            {
                m_pfnOnChange( GetID() );
            }
        }
        break;
    }









































    case INPUTMSG_IME_LANGUAGE_CHANGE:
    {
        if ( ( m_uTCFlags & ( 1 << UITCF_ENABLED_BIT ) ) && HasKeyboardFocus() && !( m_dwFlags & ( ( 1 << EDIT_FLAG_NUMBERS_ONLY_BIT ) | ( 1 << EDIT_FLAG_IME_DISABLED ) ) ) )
        {



            UpdateIMELocaleIndicator();
        }
        break;
    }
    }


    return FALSE;
}




void UIEdit::LostKeyboardFocus()
{
    if ( m_pfnOnLostFocus )
        m_pfnOnLostFocus( GetID() );
}




































void UIEdit::GainedKeyboardFocus()
{
    if ( m_pfnOnGainedFocus )
        m_pfnOnGainedFocus( GetID() );
}




















void UIEdit::SetText_Internal(
    LPCSTR,
    const BOOL )
{






    SetCursorPosition( m_nCursorPosition );
    SetSelection( m_nSelectionBegin, m_nSelectionEnd );
    if ( GetText().Length() )
    {
        size_t nNumChars = 0;
        CRefString sString = static_cast< const CRefString & >( GetText() ); // inferred spelling: the original converts into a temporary and copy-constructs sString from it (line 2520 CRefString(const&) + ~CRefString).
        UIText_RemoveFormattingCharacters( sString, FALSE, ( GetTextParseFlags() >> 4 ) & 1 );
        Blizzard::Unicode::CountUTF8Chars( sString, sString.Length(), nNumChars );
        m_nUTF8Length = (int32)nNumChars;
        KeepCursorVisible();
    }
    else
    {
        m_nUTF8Length = 0;
    }
}




void UIEdit::SetTextUpcall()
{
    if ( m_pfnOnChange )
        m_pfnOnChange( GetID() );
}







void UIEdit::PerformNumberFormatting(
    LPSTR uszBuffer,
    uint32 nLength,
    int32 & nCursorPosition )
{
    if ( nLength &&




         ( m_dwFlags & ( 1 << EDIT_FLAG_NUMBERS_ONLY_BIT ) ) &&
         ( ( m_dwFlags & ( 1 << EDIT_FLAG_NUMBER_FORMATTING_ENABLED ) ) || m_nNumDecimalDigits > 0 ) )
    {
        XLocale eCurrentLocale = ( m_eLocaleOverride ) ? m_eLocaleOverride : UnicodeText::GetStringDisplayLocale();
        CHAR uDecimalCharacter = UnicodeText::GetDecimalCharacter( eCurrentLocale );
        CHAR uszNumBuffer[MAX_UI_TEXT_LEN] = { 0 };
        uint32 nBufferPos = 0;


        while ( (int32)nBufferPos <= m_nNumDecimalDigits - (int32)nLength )
        {
            uszNumBuffer[nBufferPos++] = '0';
            ++nCursorPosition;
        }
        int32 nCursorDelta = 0;
        for ( uint32 i = 0; i < nLength && i < MAX_UI_TEXT_LEN - 1; ++i )
        {
            if ( uszBuffer[i] >= '0' && uszBuffer[i] <= '9' )
            {
                uszNumBuffer[nBufferPos++] = uszBuffer[i];
            }
            else if ( (int32)i < nCursorPosition )
            {
                ++nCursorDelta;
            }
        }
        nCursorPosition -= nCursorDelta;
        uszNumBuffer[nBufferPos] = 0;
        int32 nNewLength = nBufferPos;

        if ( (int32)nBufferPos >= m_nNumDecimalDigits && m_nNumDecimalDigits > 0 )
        {
            int32 nSize = m_nNumDecimalDigits + ( nBufferPos < MAX_UI_TEXT_LEN - 1 ? 1 : -1 );
            if ( nSize > 0 )
            {
                SigmaMemoryMove( uszNumBuffer + ( nBufferPos - m_nNumDecimalDigits + 1 ), uszNumBuffer + ( nBufferPos - m_nNumDecimalDigits ), nSize );
                nNewLength = nBufferPos + 1;
            }
            uszNumBuffer[nBufferPos - m_nNumDecimalDigits] = uDecimalCharacter;
        }
        int32 nFinalLength = nNewLength;
        if ( m_dwFlags & ( 1 << EDIT_FLAG_NUMBER_FORMATTING_ENABLED ) )
        {
            CRefString sFinal = UnicodeText::FormatNumber( uszNumBuffer, FNS_NORMAL, m_nNumDecimalDigits, m_nNumDecimalDigits, eCurrentLocale );
            nFinalLength = (int32)sFinal.Length();
            Blizzard::String::Copy( uszBuffer, sFinal.str(), MAX_UI_TEXT_LEN );
        }
        else
        {
            Blizzard::String::Copy( uszBuffer, uszNumBuffer, MAX_UI_TEXT_LEN );
        }
        int32 nNonNumericChars = 0;
        for ( int32 i = 0; i < nFinalLength; ++i )
        {
            if ( uszBuffer[i] < '0' || uszBuffer[i] > '9' )
            {
                if ( i < nCursorPosition )
                {
                    ++nCursorPosition;
                }
                ++nNonNumericChars;
            }
        }

        nCursorPosition -= nBufferPos - nFinalLength + nNonNumericChars; // line 2621 cols 25 (-=) and 39 (first -) give this spelling
    }
}












inline void UIEdit::UpdateIMELocaleIndicator()
{

    UI_UpdateIMELocaleIndicator( GetID(), GetWindowRect(), AP_HORIZONTAL_RIGHT, false );
}





























































inline BOOL UIEdit::AcceptsReturn() const
{
    return ( m_dwFlags >> EDIT_FLAG_ACCEPTS_RETURN ) & 1;
}





































BOOL UIEdit::UpdateCachedActiveState()
{
    return UIText::UpdateCachedActiveState();
}




























void UIEdit::Draw( const uint8 bLayerAlpha, float & flZSlice )
{
    sDraw( bLayerAlpha, flZSlice );
}






BOOL UIEdit::HandleMessage( const UIMsg & msg, const uintptr dwParam )
{
    switch ( msg )
    {
        case UI_UPDATE:
            sUpdate();
            break;
        case UI_INPUT:
        {
            BOOL bRetval = sInput( *(const BufferedInput *)dwParam );
            if ( bRetval )
                return bRetval;
            break;
        }
        case UI_REGISTER_INPUT_LISTENER:
            UI_RegisterInputListener( this );
            break;
        case UI_RESET:
            Animation2DReset( m_tCursorAnimState, m_snoCursor );
            break;
    }

    return UIText::HandleMessage( msg, dwParam );
}



























































void UIEdit::sGetCursorLineInfo( int32 nCursorPosition, int32 & nLineIndex, int32 & nLineCursorOriginalIndex, FormattedText & tLineAtCursor ) const
{
    if ( m_tText.Length() == 0 )
    {
        nLineIndex = 0;
        nLineCursorOriginalIndex = 0;
        tLineAtCursor = m_tText;
        return;
    }

    int32 nFormattedLength = m_tText.ToFormattedIndex( m_tText.Length() );
    nCursorPosition = Math::Pin( nCursorPosition, 0, nFormattedLength );
    int32 nOriginalCursorPosition = m_tText.ToOriginalIndex( nCursorPosition );
    LPCSTR pszCursorPosition = m_tText.GetText() + nOriginalCursorPosition;

    uint32 nCurrentLine;
    for ( nCurrentLine = 0; nCurrentLine < m_artTextLines.size(); nCurrentLine++ )
    {
        LPCSTR uszLineBegin = m_artTextLines[nCurrentLine].GetText();
        LPCSTR uszLineEnd;
        if ( nCurrentLine == m_artTextLines.size() - 1 )
            uszLineEnd = m_artTextLines[nCurrentLine].GetText() + m_artTextLines[nCurrentLine].Length();
        else
            uszLineEnd = m_artTextLines[nCurrentLine + 1].GetText();

        BOOL fInStart = pszCursorPosition >= uszLineBegin;
        BOOL fInEnd = pszCursorPosition < uszLineEnd;
        if ( fInStart && fInEnd )
        {
            nLineIndex = nCurrentLine;
            nLineCursorOriginalIndex = (int32)( pszCursorPosition - uszLineBegin );
            tLineAtCursor = m_artTextLines[nCurrentLine];
            return;
        }
    }

    nLineIndex = nCurrentLine - 1;
    nLineCursorOriginalIndex = m_artTextLines[nLineIndex].Length();
    tLineAtCursor = m_artTextLines[nLineIndex];

    if ( m_nCursorPosition >= (int32)m_tText.ToFormattedIndex( m_tText.Length() ) )
    {
        LPCSTR szStartOfLine = tLineAtCursor.GetText();
        int32 nFormattedIndexEndOfLine = tLineAtCursor.ToFormattedIndex( tLineAtCursor.Length() );
        int32 lastCharFormattedIndexFromStartOfLine = tLineAtCursor.GetPrevFormattedChar( nFormattedIndexEndOfLine );
        int32 lastCharOriginalIndexFromStartOfLine = tLineAtCursor.ToFormattedIndex( lastCharFormattedIndexFromStartOfLine );
        if ( lastCharOriginalIndexFromStartOfLine >= 0 && szStartOfLine[lastCharOriginalIndexFromStartOfLine] == '\n' )
        {
            LPCSTR szNewLine = szStartOfLine + tLineAtCursor.Length();
            nLineIndex++;
            nLineCursorOriginalIndex = 0;
            tLineAtCursor = FormattedText( szNewLine, 0, FALSE );
        }
    }
}
