// Original file: X/Client/Font/Font.cpp
// Partly written: functions sit on their original lines.
#include "X/Client/Font/Font.h"
#include "X/Sigma/RGBAColor.h"
#include "X/Common/WorldPlace.h"
#include "X/Client/Gfx/GfxVB.h"
#include "X/Common/GlobalSNO.h"
#include "X/Common/UserInterface/FontDefinition.h"
#include "X/Client/Definitions/FontDefinition.h"
#include "X/Sigma/DataStructures/XGrowableMap.h"
#include "X/Sigma/Math/MathGeneral.h"
#include "X/Common/Game.h"
#include "X/Common/ImageHandle.h"
#include "X/Client/Font/FormattedText.h"
#include "X/Common/SNO/SNOName.h"
#include "X/Client/Prefetch.h"
#include "X/Client/Definitions/ShaderDefinition.h"
// X/Client/UserInterface/UI.cpp
float UIGetBaseResolutionScaleFrac();
// X/Client/ImageCollection.cpp (ImageCollection.h does not build in this file's include set)
enum {
    IMAGE_COLLECTION_UI = 2,
};
ImageHandle StringToImageHandle( LPCSTR pszName );
BOOL ImageCollectionGetImageInfo( const ImageCollectionType eCollection, const ImageHandle hImage, ImageTextureFrame & tImageTexFrame );
// X/Client/Definitions/TextureDefinition.cpp:2697
void TextureGetFrameRectPixels( IRect2D & rectTexturePixels, const SNO snoTexture, const uint32 dwFrameIndex );

// X/Client/UserInterface/UIUtils.cpp
void ConvertPixelsDimensionsToUIC( const IVector2D & vecPixels, Vector2D & vecUIC );
static void sUpdateCursorPosition( const FontDefinition * pdefFont, FontF26Dot6 flPropScale, FontF26Dot6 flPropAdvanceScalar, FontF26Dot6 flSpaceScalar, IVector2D & vecSPBaseline, const FontChar * ptChar, uint32 uChar, IVector2D & vecSubPixelPos, FontPointSizeData * ptPointSize );  // defined at line 1958, used before

// X/Client/UserInterface/UIUtils.cpp:0x269cc0 / 0x269c30 (UIUtils.cpp is owned by another worker; declared here)
void ConvertUICToPixels( const Vector2D & vecUIC, Vector2D & vecPixels, const BOOL fSnapToPixelCenterX, const BOOL fSnapToPixelCenterY );
void ConvertUICToPixels( const Rect2D & rectUIC, Rect2D & rectPixels, const BOOL fSnapToPixelCenterX, const BOOL fSnapToPixelCenterY );
#include "X/Client/Sprite.h"
BOOL GfxStereo3DEnabled();  // X/Client/Gfx/Gfx.cpp:3898 (Gfx.h:74)
#include "BlizzardCore/Include/Blizzard/Util/Hash.h"
namespace Blizzard { namespace Thread { bool IsMainThread(); } }  // BlizzardCore/Source/System/Thread/System_Thread.cpp:395 (no header declares it)
#include "X/Sigma/ErrorManager/ErrorManager.h"
#include "X/Client/Definitions/TextureDefinition.h"
static const GlobalSNO scgeFontTable[] = { GLOBALSNO_FONT_SERIF };
// X/Client/Sprite.cpp (Sprite.h does not declare this overload)
void SpriteDrawDeferredGlyph( const TextureDefinition * pdefTexture, const float flTexWidth, const float flTexHeight, const uint32 dwFrameIndex, const ShaderDefinition * pdefShader, const Vector2D & pCoords, const float flScaleX, const float flScaleY, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const Rect2D * const pwrClipping, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw );

extern const RGBAColor crgbaTextShadow;  // DWARF: Font.cpp:47 definition is DW_AT_external (used by CGame.cpp, CPlayer.cpp)
const RGBAColor crgbaTextShadow( 0, 0, 0, 255 );
FontColorStack cgtColorStack;













































struct RenderedSizeCacheEntry
{
    Vector2D tSize;
    float flAscenderGlyphTopGapUIC;
    float flMaxAscenderGlyphTopGapUIC;
};

static XGrowableMap<unsigned long, unsigned long, RenderedSizeCacheEntry, RenderedSizeCacheEntry> s_tGetRenderedSizeCache;



void FontInitializeForApp()
{

    s_tGetRenderedSizeCache.SetHashTableSize_Tracked( 128 );
}




void FontDisposeForApp()
{
}





static inline BOOL sGetIconImage( const ImageHandle hImage, const ImageCollectionType eCollection, ImageTextureFrame & tImageTexFrame )
{
    BOOL fSuccess = ImageCollectionGetImageInfo( eCollection, hImage, tImageTexFrame );
    if ( !fSuccess )
        return FALSE;

    if ( AppDrawFlagIsSet( 105 ) && tImageTexFrame.snoTexture == GlobalSNOGet( GLOBALSNO_TEXTURE_FONTICONS_CONSOLE ) )
        return FALSE;

    return Prefetch::AddToQueue( SNOName( TEXTURE_SNO, tImageTexFrame.snoTexture ), 2 );
}





















static inline FontF26Dot6 sGetIconSize( FontPointSizeData * ptPointSize, const float flScale )
{
    return (FontF26Dot6)( flScale * ptPointSize->nIconHeight );
}





static inline float sGetIconAspectRatio( const ImageTextureFrame & tImageTexFrame )
{
    IRect2D rectTexturePixels;
    TextureGetFrameRectPixels( rectTexturePixels, tImageTexFrame.snoTexture, tImageTexFrame.nFrame );

    return (float)rectTexturePixels.Width() / (float)rectTexturePixels.Height();
}









static FontF26Dot6 sGetBulletIndentAdvance( const int32 nSpaces, const FontDefinition *& pdefFont, const TextRenderProperties & tProps, FontPointSizeData * ptPointSize )
{
    uint32 uCharSpace = ' ';
    FontF26Dot6 nAdvance = 0;
    const FontChar * ptChar = pdefFont->LookupCharacter( ptPointSize, uCharSpace );
    if ( ptChar )
    {
        float flScalar = tProps.flScale * tProps.flAdvanceScalar * pdefFont->flSpaceScalar;
        nAdvance = (FontF26Dot6)( flScalar * ptChar->nAdvanceComp * 64.0f * nSpaces );
    }

    ImageTextureFrame tImageTexFrameIcon;
    static const ImageHandle hBullet = StringToImageHandle( "bullet" );
    if ( sGetIconImage( hBullet, IMAGE_COLLECTION_UI, tImageTexFrameIcon ) )
    {
        FontF26Dot6 nIconSize = sGetIconSize( ptPointSize, tProps.flScale );
        FontF26Dot6 nIconAdvance = (FontF26Dot6)( nIconSize * sGetIconAspectRatio( tImageTexFrameIcon ) );
        nAdvance += nIconAdvance;
    }

    return nAdvance;
}













float FontGetBulletIndentAdvanceUIC( const TextRenderProperties & tProps, const int32 nSpaces )
{
    const FontDefinition * pdefFont = DefinitionLockR<FontDefinition>( tProps.snoFont );
    FontF26Dot6 nAdvance = 0;
    FontPointSizeData * ptPointSize = pdefFont->GetPointSizeData( tProps.nFontSize );
    if ( ptPointSize )
        nAdvance = sGetBulletIndentAdvance( nSpaces, pdefFont, tProps, ptPointSize );

    float flAdvanceInPixels = FontF26Dot6_To_Float( nAdvance );
    return flAdvanceInPixels / UIGetBaseResolutionScaleFrac();
}












// NON_MATCHING: 3056 B vs 3048 B. The drop-shadow test at 564 is folded into one flag hoisted out of the loop; the original keeps a per-character fcmp of flStereoDepth (s14 = 1.0) and saves d14/d15. Tried `== 1.0f` in the test (3044 B, reloads in the loop) and both icon-advance operand orders.
static void sFontStringDraw(
    const TextRenderProperties & tProps,
    const FormattedText & tFormattedText,
    IVector2D & vecSPBaseline,
    IVector2D & vecSubPixelPos,
    Rect2D * prectClipPixels,
    const FontDefinition *& pdefFont,
    const ShaderDefinition * pdefShader,
    const ShaderDefinition * pdefOutlineShader,
    const uint8 bAlpha,
    const RGBAColor * prgbaColorDropShadow,
    const BOOL fAllowColor,
    const BOOL bUseDropShadow,
    const float flZ,
    const RenderLayer eRenderLayerOverride,
    BOOL & fFirstChar,
    const FontChar *& ptCharPrev,
    BOOL fInRenderDraw,
    FontPointSizeData * ptPointSize,
    BOOL fIsPassword )
{

    FontF26Dot6 flPropScale = Float_To_FontF26Dot6( tProps.flScale );
    FontF26Dot6 flPropAdvanceScalar = Float_To_FontF26Dot6( tProps.flAdvanceScalar );
    FontF26Dot6 flSpaceScalar = Float_To_FontF26Dot6( pdefFont->flSpaceScalar );

    RGBAColor rgbaColor = cgtColorStack.GetTop();
    rgbaColor.a = MultiplyAlpha( rgbaColor.a, bAlpha );

    int32 nBytes = tFormattedText.Length();
    if ( !ptPointSize || nBytes <= 0 || rgbaColor.a == 0 )
        return;












    BOOL f3DText = tProps.flStereoDepth != 1.0f;
    LPCSTR uszOriginal, uszString;
    uszString = uszOriginal = tFormattedText.GetText();
    uint32 nCurrentDeltaIndex = 0;
    const FormattedText::FormattedTextDeltaArray & tStringDeltas = tFormattedText.m_arTextDeltas;

    const TextureDefinition * pdefTextures[32];
    int32 nRenderedTextureCount = 0;
    SNO snoTexturePrev = SNO_NONE;
    const TextureDefinition * pdefTexture = NULL;
    float flTexWidth = 0.0f;
    float flTexHeight = 0.0f;

    const float flZIncrement = 0.0002f;


    for ( LPCSTR puszEnd = uszString + nBytes; uszString <= puszEnd; )
    {
        if ( nCurrentDeltaIndex < tStringDeltas.size() )
        {
            FormattedTextDelta * ptDelta = tStringDeltas[nCurrentDeltaIndex];
            uint32 nCurOriginalTextIndex = (uint32)( uszString - uszOriginal );



            if ( ptDelta->nOriginalTextIndex <= nCurOriginalTextIndex &&
                 uszString + ptDelta->nOriginalBytesReplaced <= puszEnd )
            {

                switch ( ptDelta->eType )
                {
                case TEXTDELTA_FONTCOLOR_PUSH:

                    if ( fAllowColor )
                    {



                        FormattedColorPushDelta * pColorPushDelta = (FormattedColorPushDelta *)ptDelta;
                        rgbaColor = pColorPushDelta->rgbaColor;
                        rgbaColor.a = MultiplyAlpha( rgbaColor.a, bAlpha );

                        cgtColorStack.Push( rgbaColor );
                    }
                    break;
                case TEXTDELTA_FONTCOLOR_POP:

                    if ( fAllowColor )
                    {



                        cgtColorStack.Pop();
                        rgbaColor = cgtColorStack.GetTop();
                        rgbaColor.a = MultiplyAlpha( rgbaColor.a, bAlpha );
                    }
                    break;
                case TEXTDELTA_FONTFACE_PUSH:
                    {
                        FormattedFontFacePushDelta * pFontPushDelta = (FormattedFontFacePushDelta *)ptDelta;


                        pdefFont = (const FontDefinition *)gptGroupDefFont->LockRFast( pFontPushDelta->snoFont );
                    }
                    break;
                case TEXTDELTA_FONTICON:
                    {
                        FormattedIconDelta * pIconDelta = (FormattedIconDelta *)ptDelta;
                        ImageTextureFrame tImageTexFrameIcon;
                        if ( sGetIconImage( pIconDelta->hImage, pIconDelta->eCollection, tImageTexFrameIcon ) )
                        {

                            FontF26Dot6 nDefaultIconHeight = sGetIconSize( ptPointSize, tProps.flScale );
                            FontF26Dot6 nIconSize = sGetIconSize( ptPointSize, tProps.flScale * pIconDelta->flScale );
                            FontF26Dot6 nIconBearing = ( nIconSize + nDefaultIconHeight ) / 2;
                            FontF26Dot6 nIconAdvance = (FontF26Dot6)( nIconSize * sGetIconAspectRatio( tImageTexFrameIcon ) );

                            RGBAColor crgbaIcon( 255, 255, 255, bAlpha );

                            Rect2D rectIconPixels;
                            rectIconPixels.Set(
                                FontF26Dot6_To_Float( vecSubPixelPos.x ),
                                FontF26Dot6_To_Float( vecSubPixelPos.y - nIconBearing ),
                                FontF26Dot6_To_Float( vecSubPixelPos.x + nIconAdvance ),
                                FontF26Dot6_To_Float( vecSubPixelPos.y + ( nIconSize - nDefaultIconHeight ) / 2 ) );
                            if ( tProps.fSnapToPixelCenterX )
                            {
                                rectIconPixels.left = Math::FloatTrunc( rectIconPixels.left );
                                rectIconPixels.right = Math::FloatTrunc( rectIconPixels.right );
                            }
                            if ( tProps.fSnapToPixelCenterY )
                            {
                                rectIconPixels.top = Math::FloatTrunc( rectIconPixels.top );
                                rectIconPixels.bottom = Math::FloatTrunc( rectIconPixels.bottom );
                            }

                            SpriteDrawDeferred( tImageTexFrameIcon, SNO_NONE, rectIconPixels, crgbaIcon, crgbaBlack, flZ, prectClipPixels, SNO_NONE, NULL, FALSE, eRenderLayerOverride, FALSE );



                            vecSubPixelPos.x += nIconAdvance;
                        }
                    }
                    break;
                case TEXTDELTA_HYPERLINK:
                    {
                        FormattedHyperlinkDelta * pHyperlinkDelta = (FormattedHyperlinkDelta *)ptDelta;
                        sFontStringDraw(
                            tProps,
                            pHyperlinkDelta->tFormattedText,
                            vecSPBaseline,
                            vecSubPixelPos,
                            prectClipPixels,
                            pdefFont,
                            pdefShader,
                            pdefOutlineShader,
                            bAlpha,
                            prgbaColorDropShadow,
                            fAllowColor,
                            bUseDropShadow,
                            flZ,
                            eRenderLayerOverride,
                            fFirstChar,
                            ptCharPrev,
                            fInRenderDraw,
                            ptPointSize,
                            fIsPassword );
                    }
                    break;
                case TEXTDELTA_CHARACTER:
                    {
                        FormattedCharacterDelta * pCharacterDelta = (FormattedCharacterDelta *)ptDelta;
                        sFontStringDraw(
                            tProps,
                            FormattedText( Blizzard::Unicode::GetUTF8FromCodepoint( pCharacterDelta->uChar ), PARSE_DEFAULT, FALSE ),
                            vecSPBaseline,
                            vecSubPixelPos,
                            prectClipPixels,
                            pdefFont,
                            pdefShader,
                            pdefOutlineShader,
                            bAlpha,
                            prgbaColorDropShadow,
                            fAllowColor,
                            bUseDropShadow,
                            flZ,
                            eRenderLayerOverride,
                            fFirstChar,
                            ptCharPrev,
                            fInRenderDraw,
                            ptPointSize,
                            fIsPassword );
                    }
                    break;

                case TEXTDELTA_BULLET_INDENT:
                    {
                        FormattedBulletIndentDelta * pIndentDelta = (FormattedBulletIndentDelta *)ptDelta;

                        vecSubPixelPos.x += sGetBulletIndentAdvance( pIndentDelta->nSpaces, pdefFont, tProps, ptPointSize );
                    }
                    break;
                }





                uszString += ptDelta->nOriginalBytesReplaced;
                nCurrentDeltaIndex++;
                continue;
            }
        }


        if ( uszString == puszEnd )
            break;



        uint32 uChar = Blizzard::Unicode::GetCodepointFromUTF8( &uszString );
        if ( !uChar )
            break;

        // Inference: the asm selects the password character with one csel; the order of the two tests is not recoverable.
        if ( fIsPassword && uChar != '\n' )
            uChar = cguPasswordCharacter;

        // Inference (Font.cpp:473): fUpdateCursor leaves no instruction; it is never cleared, so the
        // sUpdateCursorPosition call at 637 is unconditional in the asm.

        BOOL fUpdateCursor = TRUE;
        const FontChar * ptChar = pdefFont->LookupCharacter( ptPointSize, uChar );

        if ( !ptChar )
        {
            uChar = cguInvalidGlyphCharacter;
            ptChar = pdefFont->LookupCharacter( ptPointSize, uChar );
        }



        if ( ptChar->snoTex != INVALID_SNO )
        {













            if ( ptChar->nGlyphFrame == -1 )
            {
                DisplayInternalError( NULL, "", __LINE__, ERROR_INTERNAL_FATAL ); ExitProgram( ERROR_INTERNAL_FATAL );
            }




            if ( ptChar->snoTex != snoTexturePrev )
            {
                BOOL fFound = FALSE;
                for ( int32 i = 0; i < nRenderedTextureCount; i++ )
                {
                    if ( pdefTextures[i]->tHeader.snoHandle == ptChar->snoTex )
                    {
                        pdefTexture = pdefTextures[i];
                        flTexWidth = (float)pdefTexture->dwWidth;
                        flTexHeight = (float)pdefTexture->dwHeight;
                        fFound = TRUE;
                        break;
                    }
                }

                if ( !fFound )
                {

                    pdefTexture = (const TextureDefinition *)gptGroupDefTexture->LockR( ptChar->snoTex, FALSE );
                    flTexWidth = (float)pdefTexture->dwWidth;
                    flTexHeight = (float)pdefTexture->dwHeight;
                    pdefTextures[nRenderedTextureCount++] = pdefTexture;
                }

                snoTexturePrev = ptChar->snoTex;
            }

            if ( !fFirstChar )
            {
                IVector2D vecKerning = ptPointSize->GetKerning( *ptCharPrev, *ptChar );
                vecSubPixelPos.x += FontF26Dot6Multiply( vecKerning.x, flPropScale );
                vecSubPixelPos.y += FontF26Dot6Multiply( vecKerning.y, flPropScale );
            }
            fFirstChar = FALSE;
            ptCharPrev = ptChar;


            FontF26Dot6 flX = vecSubPixelPos.x + FontF26Dot6Multiply( Int_ToFontF26Dot6( ptChar->nBearingXComp ), flPropScale );
            FontF26Dot6 flY = vecSubPixelPos.y - FontF26Dot6Multiply( Int_ToFontF26Dot6( ptChar->nBearingYComp ), flPropScale );




            if ( tProps.fSnapToPixelCenterX )
            {

                flX &= ~63;
            }
            if ( tProps.fSnapToPixelCenterY )
            {

                flY &= ~63;
            }

            Vector2D vecCurPos = Vector2D( FontF26Dot6_To_Float( flX ), FontF26Dot6_To_Float( flY ) );


            if ( prgbaColorDropShadow && !f3DText && bUseDropShadow && prgbaColorDropShadow->a )
            {
                static Vector2D vecOffset = Vector2D( 1.0f, 1.0f );

                SpriteDrawDeferredGlyph(
                    pdefTexture,
                    flTexWidth,
                    flTexHeight,
                    ptChar->nGlyphFrame,
                    pdefShader,
                    vecCurPos + vecOffset,
                    tProps.flScale,
                    tProps.flScale,
                    *prgbaColorDropShadow,
                    crgbaBlack,
                    Math::Pin( flZ + flZIncrement, 0.0f, 1.0f ),
                    tProps.flStereoDepth,
                    prectClipPixels,
                    eRenderLayerOverride,
                    fInRenderDraw );
            }


            SpriteDrawDeferredGlyph(
                pdefTexture,
                flTexWidth,
                flTexHeight,
                ptChar->nGlyphFrame,
                pdefShader,
                vecCurPos,
                tProps.flScale,
                tProps.flScale,
                rgbaColor,
                prgbaColorDropShadow ? *prgbaColorDropShadow : crgbaBlack,
                flZ,
                tProps.flStereoDepth,
                prectClipPixels,
                eRenderLayerOverride,
                fInRenderDraw );
        }
































        if ( fUpdateCursor )
            sUpdateCursorPosition( pdefFont, flPropScale, flPropAdvanceScalar, flSpaceScalar, vecSPBaseline, ptChar, uChar, vecSubPixelPos, ptPointSize );
    }


    for ( int32 i = 0; i < nRenderedTextureCount; i++ )
    {
        gptGroupDefTexture->UnlockR( (void *&)pdefTextures[i] );
    }
}




static void sGetTextShaders(
    const TextRenderProperties & tProps,
    SNO & snoShader,
    SNO & snoShaderHighlight )
{
    GlobalSNO eShader = ( tProps.fSnapToPixelCenterX &&
        tProps.fSnapToPixelCenterY ) ? GLOBALSNO_SHADER_TEXT_OUTLINE : GLOBALSNO_SHADER_TEXT_OUTLINE_NO_SNAP;















    snoShader = GlobalSNOGet( eShader );




    if ( tProps.flStereoDepth != 1.0f && GfxStereo3DEnabled() )

        snoShader = GlobalSNOGet( GLOBALSNO_SHADER_TEXT_3DV );

}





void FontStringDrawUIC(
    const TextRenderProperties & tProps,
    const FormattedText & tFormattedText,
    const Vector2D & vecPointUIC,
    const RGBAColor & rgbaColor,
    const RGBAColor * prgbaColorDropShadow,
    const float flZ,
    BOOL fAllowColor,
    const Rect2D * prectClipUIC,
    const RenderLayer eRenderLayerOverride,
    BOOL fInRenderDraw )
{
    cgtColorStack.Empty();
    cgtColorStack.Push( rgbaColor );


    if ( Math::Abs( vecPointUIC.x ) > 10000.0f ||
         Math::Abs( vecPointUIC.y ) > 10000.0f )
        return;
    {


        Vector2D vecPointPixels;
        ConvertUICToPixels( vecPointUIC, vecPointPixels, tProps.fSnapToPixelCenterX, tProps.fSnapToPixelCenterY );

        Rect2D * prectClipPixels = NULL;
        Rect2D rectClipPixels;
        if ( prectClipUIC )
        {
            prectClipPixels = &rectClipPixels;
            ConvertUICToPixels( *prectClipUIC, rectClipPixels, tProps.fSnapToPixelCenterX, tProps.fSnapToPixelCenterY );
        }










        const FontDefinition * pdefFont = (const FontDefinition *)gptGroupDefFont->LockRFast( tProps.snoFont );
        FontPointSizeData * ptPointSize = pdefFont->GetPointSizeData( tProps.nFontSize );



        if ( !ptPointSize )
            return;





        SNO snoShader, snoShaderHighlight;
        sGetTextShaders( tProps, snoShader, snoShaderHighlight );
        const ShaderDefinition * pdefShader = DefinitionLockR<ShaderDefinition>( snoShader );
        const ShaderDefinition * pdefOutlineShader = NULL;  // Inference: one shader LockRFast in the asm; dead-argument elimination removes this argument
        FontF26Dot6 nFontAscender = (FontF26Dot6)( tProps.flScale * ptPointSize->nFontAscender );

        IVector2D vecSPBaseline = IVector2D( Float_To_FontF26Dot6( vecPointPixels.x ), Float_To_FontF26Dot6( vecPointPixels.y ) + nFontAscender );
        IVector2D vecSubPixelPos = vecSPBaseline;
        BOOL fFirstChar = TRUE;
        const FontChar * ptCharPrev;

        sFontStringDraw( tProps, tFormattedText, vecSPBaseline, vecSubPixelPos, prectClipPixels, pdefFont, pdefShader, pdefOutlineShader, rgbaColor.a, prgbaColorDropShadow, fAllowColor, FALSE, flZ, eRenderLayerOverride, fFirstChar, ptCharPrev, fInRenderDraw, ptPointSize, tFormattedText.IsPassword() );































    }
}







































































































void FontStringDrawCenteredUIC( const TextRenderProperties & tProps, const FormattedText & tFormattedText, const Vector2D & vecUIC, const RGBAColor & rgbaColor, const RGBAColor * prgbaColorDropShadow, const float flZ, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw )
{
    Vector2D vecSizeUIC;
    FontStringGetRenderedSizeUIC( tProps, tFormattedText, vecSizeUIC );

    Vector2D vecTextUIC = Vector2D( vecUIC.x - vecSizeUIC.x * 0.5f, vecUIC.y - vecSizeUIC.y * 0.5f );
    FontStringDrawUIC( tProps, tFormattedText, vecTextUIC, rgbaColor, prgbaColorDropShadow, flZ, TRUE, NULL, eRenderLayerOverride, fInRenderDraw );
}

























void FontSubstringDrawUIC(
    const TextRenderProperties & tProps,
    FontPointSizeData * ptPointSize,
    const FontDefinition * pdefFont,
    const FormattedText & tFormattedText,
    const Vector2D & vecPointUIC,
    const uint8 bAlpha,
    const RGBAColor * prgbaColorDropShadow,
    const BOOL bUseDropShadow,
    const float flZ,
    BOOL fAllowColor,
    const Rect2D * prectClipUIC,
    const RenderLayer eRenderLayerOverride )
{

    if ( Math::Abs( vecPointUIC.x ) > 10000.0f ||
         Math::Abs( vecPointUIC.y ) > 10000.0f )
        return;



    Vector2D vecPointPixels;
    ConvertUICToPixels( vecPointUIC, vecPointPixels, tProps.fSnapToPixelCenterX, tProps.fSnapToPixelCenterY );

    Rect2D * prectClipPixels = NULL;
    Rect2D rectClipPixels;
    if ( prectClipUIC )
    {
        prectClipPixels = &rectClipPixels;
        ConvertUICToPixels( *prectClipUIC, rectClipPixels, tProps.fSnapToPixelCenterX, tProps.fSnapToPixelCenterY );
    }












    if ( !ptPointSize )
        return;




    SNO snoShader, snoShaderHighlight;
    sGetTextShaders( tProps, snoShader, snoShaderHighlight );
    const ShaderDefinition * pdefShader = DefinitionLockR<ShaderDefinition>( snoShader );
    const ShaderDefinition * pdefOutlineShader = NULL;  // Inference: dead argument of sFontStringDraw, see FontStringDrawUIC
    FontF26Dot6 nFontAscender = (FontF26Dot6)( tProps.flScale * ptPointSize->nFontAscender );

    IVector2D vecSPBaseline = IVector2D( Float_To_FontF26Dot6( vecPointPixels.x ), Float_To_FontF26Dot6( vecPointPixels.y ) + nFontAscender );
    IVector2D vecSubPixelPos = vecSPBaseline;
    BOOL fFirstChar = TRUE;
    const FontChar * ptCharPrev;

    sFontStringDraw(
        tProps,
        tFormattedText,
        vecSPBaseline,
        vecSubPixelPos,
        prectClipPixels,
        pdefFont,
        pdefShader,
        pdefOutlineShader,
        bAlpha,
        prgbaColorDropShadow,
        fAllowColor,
        bUseDropShadow,
        flZ,
        eRenderLayerOverride,
        fFirstChar,
        ptCharPrev,
        FALSE,
        ptPointSize,
        tFormattedText.IsPassword() );









}




static void sRecordSelectionPos(
    const int32 nPos,
    const int32 nSelectionBegin,
    const int32 nSelectionEnd,
    const IVector2D & vecSubPixelPos,
    IVector2D & vecSubPixelSelectionStart,
    IVector2D & vecSubPixelSelectionEnd )
{
    if ( nPos == nSelectionBegin )
    {
        vecSubPixelSelectionStart = vecSubPixelPos;
    }
    if ( nPos == nSelectionEnd )
    {
        vecSubPixelSelectionEnd = vecSubPixelPos;
    }
}



// NON_MATCHING: 3568 B both, opcodes identical; frame is 0x1b0 vs 0x1a0 (one extra spill slot), so stack offsets differ. Tried both icon-advance operand orders (the other one is 3572 B and keeps nIconSize in d9 like the original).
static BOOL sFontStringGetRenderedSize(
    const TextRenderProperties & tProps,
    const FormattedText & tFormattedText,
    const FontDefinition *& pdefFont,
    IVector2D & vecSPBaseline,
    IVector2D & vecSubPixelPos,
    IVector2D & vecSubPixelSelectionStart,
    IVector2D & vecSubPixelSelectionEnd,
    FontF26Dot6 & nSubPixelWidth,
    FontF26Dot6 & nSubPixelMinY,
    FontF26Dot6 & nSubPixelMaxY,
    FontF26Dot6 & nSPFirstLineBearing,
    BOOL & fFirstChar,
    BOOL & fOnFirstLine,
    const FontChar *& ptCharPrev,
    int32 & nFormattedIndex,
    const int32 nSelectionBegin,
    const int32 nSelectionEnd,
    IVector2D & tSize,
    IVector2D & vecSelectionBegin,
    IVector2D & vecSelectionEnd,
    FontPointSizeData * ptPointSize,
    BOOL fIsPassword,
    BOOL fFinalizeSelectionProcess )
{

    FontF26Dot6 flPropScale = Float_To_FontF26Dot6( tProps.flScale );
    FontF26Dot6 flPropAdvanceScalar = Float_To_FontF26Dot6( tProps.flAdvanceScalar );
    FontF26Dot6 flSpaceScalar = Float_To_FontF26Dot6( pdefFont->flSpaceScalar );

    int32 nBytes = tFormattedText.Length();

    if ( !ptPointSize || nBytes <= 0 )
        return TRUE;












    BOOL bFoundAllDependencies = TRUE;

    BOOL fProcessSelection = nSelectionBegin != -1 && nSelectionEnd != -1;
    LPCSTR uszString = tFormattedText.GetText();
    LPCSTR pszPos = uszString;
    uint32 nCurrentDeltaIndex = 0;
    const FormattedText::FormattedTextDeltaArray & tStringDeltas = tFormattedText.m_arTextDeltas;
    if ( fProcessSelection )
    {
        sRecordSelectionPos( nFormattedIndex, nSelectionBegin, nSelectionEnd, vecSubPixelPos, vecSubPixelSelectionStart, vecSubPixelSelectionEnd );
    }

    nSubPixelMinY = Math::Min( nSubPixelMinY, vecSubPixelPos.y );


    for ( LPCSTR puszEnd = uszString + nBytes; pszPos <= puszEnd; )
    {
        if ( nCurrentDeltaIndex < tStringDeltas.size() )
        {
            FormattedTextDelta * ptDelta = tStringDeltas[nCurrentDeltaIndex];
            uint32 nCurOriginalTextIndex = (uint32)( pszPos - uszString );



            if ( ptDelta->nOriginalTextIndex <= nCurOriginalTextIndex &&
                 pszPos + ptDelta->nOriginalBytesReplaced <= puszEnd )
            {
                switch ( ptDelta->eType )
                {
                case TEXTDELTA_FONTFACE_PUSH:
                    {
                        FormattedFontFacePushDelta * pFontPushDelta = (FormattedFontFacePushDelta *)ptDelta;


                        pdefFont = (const FontDefinition *)gptGroupDefFont->LockRFast( pFontPushDelta->snoFont );
                    }
                    break;
                case TEXTDELTA_FONTICON:
                    {
                        FormattedIconDelta * pIconDelta = (FormattedIconDelta *)ptDelta;
                        ImageTextureFrame tImageTexFrameIcon;
                        if ( sGetIconImage( pIconDelta->hImage, pIconDelta->eCollection, tImageTexFrameIcon ) )
                        {

                            FontF26Dot6 nDefaultIconHeight = sGetIconSize( ptPointSize, tProps.flScale );
                            FontF26Dot6 nIconSize = sGetIconSize( ptPointSize, tProps.flScale * pIconDelta->flScale );
                            FontF26Dot6 nIconBearing = ( nIconSize + nDefaultIconHeight ) / 2;
                            FontF26Dot6 nIconAdvance = (FontF26Dot6)( nIconSize * sGetIconAspectRatio( tImageTexFrameIcon ) );

                            vecSubPixelPos.x += nIconAdvance;


                            nSubPixelWidth = Math::Max( nSubPixelWidth, vecSubPixelPos.x );
                            nSubPixelMinY = Math::Min( nSubPixelMinY, vecSubPixelPos.y - nIconBearing );
                            nSubPixelMaxY = Math::Max( nSubPixelMaxY, vecSubPixelPos.y + ( nIconSize - nDefaultIconHeight ) / 2 );
                            if ( fOnFirstLine )
                            {
                                nSPFirstLineBearing = Math::Max( nSPFirstLineBearing, nIconBearing );
                            }
                            // Inference: this tail is merged with the bullet case's in the asm (no line or inline record of its own here).
                            nFormattedIndex += ptDelta->nFormattedTextLength;
                            if ( fProcessSelection )
                                sRecordSelectionPos( nFormattedIndex, nSelectionBegin, nSelectionEnd, vecSubPixelPos, vecSubPixelSelectionStart, vecSubPixelSelectionEnd );
                        }
                        else
                        {
                            bFoundAllDependencies = FALSE;
                        }
                    }
                    break;


                case TEXTDELTA_HYPERLINK:
                    {
                        FormattedHyperlinkDelta * pHyperlinkDelta = (FormattedHyperlinkDelta *)ptDelta;
                        sFontStringGetRenderedSize(
                            tProps,
                            pHyperlinkDelta->tFormattedText,
                            pdefFont,
                            vecSPBaseline,
                            vecSubPixelPos,
                            vecSubPixelSelectionStart,
                            vecSubPixelSelectionEnd,
                            nSubPixelWidth,
                            nSubPixelMinY,
                            nSubPixelMaxY,
                            nSPFirstLineBearing,
                            fFirstChar,
                            fOnFirstLine,
                            ptCharPrev,
                            nFormattedIndex,
                            nSelectionBegin,
                            nSelectionEnd,
                            tSize,
                            vecSelectionBegin,
                            vecSelectionEnd,
                            ptPointSize,
                            fIsPassword,
                            FALSE );
                    }
                    break;
                case TEXTDELTA_CHARACTER:
                    {
                        FormattedCharacterDelta * pCharacterDelta = (FormattedCharacterDelta *)ptDelta;
                        uint32 uDeltaChar = pCharacterDelta->uChar;
                        if ( pszPos + ptDelta->nOriginalBytesReplaced == puszEnd && uDeltaChar == '\n' )
                        {

                            if ( fProcessSelection )
                            {
                                nFormattedIndex++;
                                sRecordSelectionPos( nFormattedIndex, nSelectionBegin, nSelectionEnd, vecSubPixelPos, vecSubPixelSelectionStart, vecSubPixelSelectionEnd );
                            }
                        }

                        else if ( uDeltaChar == '\n' )
                        {



                            sFontStringGetRenderedSize(
                                tProps,
                                FormattedText( "\n\n", PARSE_DEFAULT, FALSE ),
                                pdefFont,
                                vecSPBaseline,
                                vecSubPixelPos,
                                vecSubPixelSelectionStart,
                                vecSubPixelSelectionEnd,
                                nSubPixelWidth,
                                nSubPixelMinY,
                                nSubPixelMaxY,
                                nSPFirstLineBearing,
                                fFirstChar,
                                fOnFirstLine,
                                ptCharPrev,
                                nFormattedIndex,
                                nSelectionBegin,
                                nSelectionEnd,
                                tSize,
                                vecSelectionBegin,
                                vecSelectionEnd,
                                ptPointSize,
                                FALSE,
                                FALSE );
                        }
                        else
                        {
                            sFontStringGetRenderedSize(
                                tProps,
                                FormattedText( Blizzard::Unicode::GetUTF8FromCodepoint( uDeltaChar ), PARSE_DEFAULT, FALSE ),
                                pdefFont,
                                vecSPBaseline,
                                vecSubPixelPos,
                                vecSubPixelSelectionStart,
                                vecSubPixelSelectionEnd,
                                nSubPixelWidth,
                                nSubPixelMinY,
                                nSubPixelMaxY,
                                nSPFirstLineBearing,
                                fFirstChar,
                                fOnFirstLine,
                                ptCharPrev,
                                nFormattedIndex,
                                nSelectionBegin,
                                nSelectionEnd,
                                tSize,
                                vecSelectionBegin,
                                vecSelectionEnd,
                                ptPointSize,
                                fIsPassword,
                                FALSE );
                        }
                    }
                    break;


                case TEXTDELTA_BULLET_INDENT:
                    {
                        FormattedBulletIndentDelta * pIndentDelta = (FormattedBulletIndentDelta *)ptDelta;

                        vecSubPixelPos.x += sGetBulletIndentAdvance( pIndentDelta->nSpaces, pdefFont, tProps, ptPointSize );
                        nFormattedIndex += ptDelta->nFormattedTextLength;

                        if ( fProcessSelection )
                        {
                            sRecordSelectionPos( nFormattedIndex, nSelectionBegin, nSelectionEnd, vecSubPixelPos, vecSubPixelSelectionStart, vecSubPixelSelectionEnd );
                        }
                    }
                    break;
                }



                pszPos += ptDelta->nOriginalBytesReplaced;
                nCurrentDeltaIndex++;
                continue;
            }
        }

        if ( pszPos == puszEnd )
            break;



        LPCSTR pszPosSaved = pszPos;
        uint32 uChar = Blizzard::Unicode::GetCodepointFromUTF8( &pszPos );
        nFormattedIndex += (int32)( pszPos - pszPosSaved );
        if ( !uChar )
        {
            if ( fProcessSelection )
            {
                sRecordSelectionPos( nFormattedIndex, nSelectionBegin, nSelectionEnd, vecSubPixelPos, vecSubPixelSelectionStart, vecSubPixelSelectionEnd );
            }
            break;
        }
        if ( fIsPassword && uChar != '\n' )
            uChar = cguPasswordCharacter;



        if ( uChar == '\n' )
        {
            fOnFirstLine = FALSE;
        }

        if ( pszPos == puszEnd )
        {
            if ( fProcessSelection )
            {
                sRecordSelectionPos( nFormattedIndex, nSelectionBegin, nSelectionEnd, vecSubPixelPos, vecSubPixelSelectionStart, vecSubPixelSelectionEnd );
            }
            if ( uChar == '\n' && !fFirstChar )
                break;
        }




        const FontChar * ptChar = pdefFont->LookupCharacter( ptPointSize, uChar );
        if ( !ptChar )
        {
            uChar = cguInvalidGlyphCharacter;
            ptChar = pdefFont->LookupCharacter( ptPointSize, uChar );
        }



        if ( !fFirstChar )
        {
            IVector2D vecKerning = ptPointSize->GetKerning( *ptCharPrev, *ptChar );
            vecSubPixelPos.x += FontF26Dot6Multiply( vecKerning.x, flPropScale );
            vecSubPixelPos.y += FontF26Dot6Multiply( vecKerning.y, flPropScale );
        }
        fFirstChar = FALSE;
        ptCharPrev = ptChar;

        sUpdateCursorPosition( pdefFont, flPropScale, flPropAdvanceScalar, flSpaceScalar, vecSPBaseline, ptChar, uChar, vecSubPixelPos, ptPointSize );


        FontF26Dot6 nCharBearingY = (FontF26Dot6)( tProps.flScale * ptChar->nBearingYComp * 64.0f );
        FontF26Dot6 nCharHeight = (FontF26Dot6)( tProps.flScale * ptChar->nHeightComp * 64.0f );





        nSubPixelWidth = Math::Max( nSubPixelWidth, vecSubPixelPos.x );
        nSubPixelMinY = Math::Min( nSubPixelMinY, vecSubPixelPos.y - nCharBearingY );
        nSubPixelMaxY = Math::Max( nSubPixelMaxY, vecSubPixelPos.y - nCharBearingY + nCharHeight );
        if ( fOnFirstLine )
        {
            nSPFirstLineBearing = Math::Max( nSPFirstLineBearing, nCharBearingY );
        }

        if ( fProcessSelection )
        {
            sRecordSelectionPos( nFormattedIndex, nSelectionBegin, nSelectionEnd, vecSubPixelPos, vecSubPixelSelectionStart, vecSubPixelSelectionEnd );
        }
    }

    tSize.x = FontF26Dot6_To_Int( nSubPixelWidth );
    tSize.y = FontF26Dot6_To_Int( nSubPixelMaxY - nSubPixelMinY );

    if ( fProcessSelection && fFinalizeSelectionProcess )
    {
        vecSelectionBegin.x = FontF26Dot6_To_Int( vecSubPixelSelectionStart.x );
        vecSelectionBegin.y = FontF26Dot6_To_Int( vecSubPixelSelectionStart.y );
        vecSelectionEnd.x = FontF26Dot6_To_Int( vecSubPixelSelectionEnd.x );
        vecSelectionEnd.y = FontF26Dot6_To_Int( vecSubPixelSelectionEnd.y );
    }

    return bFoundAllDependencies;
}




BOOL FontStringGetRenderedSizeUICUncached(
    const TextRenderProperties & tProps,
    const FormattedText & tFormattedText,
    Vector2D & vecSizeUIC,
    float * pflAscenderGlyphTopGapUIC,
    float * pflMaxAscenderGlyphTopGapUIC )
{
    IVector2D vecTextSize, vecSelectionBegin, vecSelectionEnd;
    vecTextSize.Set( 0, 0 );

    if ( pflAscenderGlyphTopGapUIC )

        *pflAscenderGlyphTopGapUIC = 0.0f;

    if ( pflMaxAscenderGlyphTopGapUIC )

        *pflMaxAscenderGlyphTopGapUIC = 0.0f;


    const FontDefinition * pdefFont = (const FontDefinition *)gptGroupDefFont->LockRFast( tProps.snoFont );
    FontPointSizeData * ptPointSize = pdefFont->GetPointSizeData( tProps.nFontSize );



    if ( !ptPointSize )
        return TRUE;





    FontF26Dot6 nFontAscender = (FontF26Dot6)( tProps.flScale * ptPointSize->nFontAscender );
    IVector2D vecSPBaseline = IVector2D( 0, nFontAscender );
    IVector2D vecSubPixelPos = vecSPBaseline;
    FontF26Dot6 nSubPixelWidth = 0;
    FontF26Dot6 nSubPixelMinY = 0x7fffffff;
    FontF26Dot6 nSubPixelMaxY = (FontF26Dot6)0x80000000;
    FontF26Dot6 nSPFirstLineBearing = 0;
    BOOL fFirstChar = TRUE;
    const FontChar * ptCharPrev;
    BOOL fOnFirstLine = TRUE;
    IVector2D vecSubPixelSelectionStart = cgptNull;
    IVector2D vecSubPixelSelectionEnd = cgptNull;
    int32 nFormattedIndex = 0;

    BOOL bFoundAllDependencies = sFontStringGetRenderedSize(
        tProps,
        tFormattedText,
        pdefFont,
        vecSPBaseline,
        vecSubPixelPos,
        vecSubPixelSelectionStart,
        vecSubPixelSelectionEnd,
        nSubPixelWidth,
        nSubPixelMinY,
        nSubPixelMaxY,
        nSPFirstLineBearing,
        fFirstChar,
        fOnFirstLine,
        ptCharPrev,
        nFormattedIndex,
        -1,
        -1,
        vecTextSize,
        vecSelectionBegin,
        vecSelectionEnd,
        ptPointSize,
        tFormattedText.IsPassword(),
        TRUE );





    ConvertPixelsDimensionsToUIC( vecTextSize, vecSizeUIC );

    if ( pflAscenderGlyphTopGapUIC || pflMaxAscenderGlyphTopGapUIC )
    {

        float flInvScaleFrac = 1.0f / UIGetBaseResolutionScaleFrac();

        if ( pflAscenderGlyphTopGapUIC )
        {
            FontF26Dot6 nFontAscenderScoped = (FontF26Dot6)( tProps.flScale * ptPointSize->nFontAscender );
            *pflAscenderGlyphTopGapUIC = flInvScaleFrac * FontF26Dot6_To_Int( nFontAscenderScoped - nSPFirstLineBearing );
        }
        if ( pflMaxAscenderGlyphTopGapUIC )
        {
            FontF26Dot6 nMaxAscender = (FontF26Dot6)( tProps.flScale * ptPointSize->nMaxAscender );
            *pflMaxAscenderGlyphTopGapUIC = flInvScaleFrac * FontF26Dot6_To_Int( nMaxAscender - nSPFirstLineBearing );
        }
    }

    return bFoundAllDependencies;
}




inline uint64 sGenerateKey(
    const TextRenderProperties & tProps,
    const FormattedText & tFormattedText )
{
    return Blizzard::Util::Hash64More( &tProps, sizeof( tProps ), Blizzard::Util::Hash64More( UIGetBaseResolutionScaleFrac(), tFormattedText.GetHash() ) );

}



void FontStringGetRenderedSizeUIC(
    const TextRenderProperties & tProps,
    const FormattedText & tFormattedText,
    Vector2D & vecSizeUIC,
    float * pflAscenderGlyphTopGapUIC,
    float * pflMaxAscenderGlyphTopGapUIC )
{
    if ( !Blizzard::Thread::IsMainThread() )
    {
        FontStringGetRenderedSizeUICUncached( tProps, tFormattedText, vecSizeUIC, pflAscenderGlyphTopGapUIC, pflMaxAscenderGlyphTopGapUIC );
        return;
    }

    uint64 nKey = sGenerateKey( tProps, tFormattedText );

    RenderedSizeCacheEntry tEntry;

    if ( s_tGetRenderedSizeCache.Lookup( nKey, tEntry ) )
    {
        if ( pflAscenderGlyphTopGapUIC )

            *pflAscenderGlyphTopGapUIC = tEntry.flAscenderGlyphTopGapUIC;


        if ( pflMaxAscenderGlyphTopGapUIC )

            *pflMaxAscenderGlyphTopGapUIC = tEntry.flMaxAscenderGlyphTopGapUIC;


        vecSizeUIC = tEntry.tSize;
        return;
    }

    BOOL bFoundAllDependencies = FontStringGetRenderedSizeUICUncached( tProps, tFormattedText, vecSizeUIC, pflAscenderGlyphTopGapUIC, pflMaxAscenderGlyphTopGapUIC );
    if ( !bFoundAllDependencies )
        return;







    if ( s_tGetRenderedSizeCache.Count() > 256 )

        s_tGetRenderedSizeCache.RemoveAll();


    tEntry.tSize = vecSizeUIC;
    tEntry.flAscenderGlyphTopGapUIC = 0.0f;
    if ( pflAscenderGlyphTopGapUIC )

        tEntry.flAscenderGlyphTopGapUIC = *pflAscenderGlyphTopGapUIC;

    tEntry.flMaxAscenderGlyphTopGapUIC = 0.0f;




    if ( pflMaxAscenderGlyphTopGapUIC )

        tEntry.flMaxAscenderGlyphTopGapUIC = *pflMaxAscenderGlyphTopGapUIC;






    s_tGetRenderedSizeCache.SetAt_Tracked( nKey, tEntry );
}



void FontStringGetRenderedSizeUIC(
    const TextRenderProperties & tProps,
    const FormattedText & tFormattedText,
    Vector2D & vecSizeUIC )
{
    float flAscenderGlyphTopGapUIC;
    float flMaxAscenderGlyphTopGapUIC;

    if ( !Blizzard::Thread::IsMainThread() )
    {
        FontStringGetRenderedSizeUICUncached( tProps, tFormattedText, vecSizeUIC, &flAscenderGlyphTopGapUIC, &flMaxAscenderGlyphTopGapUIC );
        return;
    }

    uint64 nKey = sGenerateKey( tProps, tFormattedText );

    RenderedSizeCacheEntry tEntry;

    if ( s_tGetRenderedSizeCache.Lookup( nKey, tEntry ) )
    {
        vecSizeUIC = tEntry.tSize;
        return;
    }

    BOOL bFoundAllDependencies = FontStringGetRenderedSizeUICUncached( tProps, tFormattedText, vecSizeUIC, &flAscenderGlyphTopGapUIC, &flMaxAscenderGlyphTopGapUIC );
    if ( !bFoundAllDependencies )
        return;




    if ( s_tGetRenderedSizeCache.Count() > 256 )

        s_tGetRenderedSizeCache.RemoveAll();


    tEntry.tSize = vecSizeUIC;
    tEntry.flAscenderGlyphTopGapUIC = flAscenderGlyphTopGapUIC;
    tEntry.flMaxAscenderGlyphTopGapUIC = flMaxAscenderGlyphTopGapUIC;
    s_tGetRenderedSizeCache.SetAt_Tracked( nKey, tEntry );
}





void FontStringGetSelectedRegionUIC(
    const TextRenderProperties & tProps,
    const FormattedText & tFormattedText,
    const int32 nSelectionBegin,
    const int32 nSelectionEnd,
    Vector2D & vecSizeUIC,
    Vector2D & vecSelectionBeginUIC,
    Vector2D & vecSelectionEndUIC )
{
    IVector2D vecTextSize, vecSelectionBegin, vecSelectionEnd;
    vecTextSize.Set( 0, 0 );
    const FontDefinition * pdefFont = (const FontDefinition *)gptGroupDefFont->LockRFast( tProps.snoFont );
    FontPointSizeData * ptPointSize = pdefFont->GetPointSizeData( tProps.nFontSize );



    if ( !ptPointSize )
        return;





    FontF26Dot6 nFontAscender = (FontF26Dot6)( tProps.flScale * ptPointSize->nFontAscender );
    IVector2D vecSPBaseline = IVector2D( 0, nFontAscender );
    IVector2D vecSubPixelPos = vecSPBaseline;
    FontF26Dot6 nSubPixelWidth = 0;
    FontF26Dot6 nSubPixelMinY = 0x7fffffff;
    FontF26Dot6 nSubPixelMaxY = (FontF26Dot6)0x80000000;
    FontF26Dot6 nSPFirstLineBearing = 0;
    BOOL fFirstChar = TRUE;
    const FontChar * ptCharPrev;
    BOOL fOnFirstLine = TRUE;
    IVector2D vecSubPixelSelectionStart = cgptNull;
    IVector2D vecSubPixelSelectionEnd = cgptNull;
    int32 nFormattedIndex = 0;

    sFontStringGetRenderedSize(
        tProps,
        tFormattedText,
        pdefFont,
        vecSPBaseline,
        vecSubPixelPos,
        vecSubPixelSelectionStart,
        vecSubPixelSelectionEnd,
        nSubPixelWidth,
        nSubPixelMinY,
        nSubPixelMaxY,
        nSPFirstLineBearing,
        fFirstChar,
        fOnFirstLine,
        ptCharPrev,
        nFormattedIndex,
        nSelectionBegin,
        nSelectionEnd,
        vecTextSize,
        vecSelectionBegin,
        vecSelectionEnd,
        ptPointSize,
        tFormattedText.IsPassword(),
        TRUE );





    ConvertPixelsDimensionsToUIC( vecTextSize, vecSizeUIC );
    ConvertPixelsDimensionsToUIC( vecSelectionBegin, vecSelectionBeginUIC );
    ConvertPixelsDimensionsToUIC( vecSelectionEnd, vecSelectionEndUIC );
}




// NON_MATCHING: 1868 B vs 1864 B. In the TEXTDELTA_CHARACTER case the inlined sUpdateCursorPosition tests ' ' before '\n' with a branch; the original tests '\n' first and selects the space scalar with csel. Rest matches. Also tried a ternary for the ' ' scalar (no change).
BOOL sFontStringGetPosWithinLine(
    const TextRenderProperties & tProps,
    const FormattedText & tFormattedText,
    const FontDefinition *& pdefFont,
    IVector2D & vecSPBaseline,
    IVector2D & vecSubPixelPos,
    BOOL & fFirstChar,
    const FontChar *& ptCharPrev,
    int32 & nFormattedIndex,
    const Vector2D & vecMousePixels,
    FontPointSizeData * ptPointSize )
{

    FontF26Dot6 flPropScale = Float_To_FontF26Dot6( tProps.flScale );
    FontF26Dot6 flPropAdvanceScalar = Float_To_FontF26Dot6( tProps.flAdvanceScalar );
    FontF26Dot6 flSpaceScalar = Float_To_FontF26Dot6( pdefFont->flSpaceScalar );

    int32 nBytes = tFormattedText.Length();


    if ( nBytes <= 0 || vecMousePixels.x <= 0.0f )
    {
        nFormattedIndex = 0;
        return FALSE;
    }
    // Inference: only the x component is converted (one Int_ToFontF26Dot6 inline at 1710; y is never read).
    IVector2D vecSPMouse;
    vecSPMouse.x = Int_ToFontF26Dot6( (int32)vecMousePixels.x );





    if ( !ptPointSize )
    {
        nFormattedIndex = 0;
        return FALSE;
    }


    LPCSTR uszString = tFormattedText.GetText();
    LPCSTR pszPos = uszString;
    uint32 nCurrentDeltaIndex = 0;
    const FormattedText::FormattedTextDeltaArray & tStringDeltas = tFormattedText.m_arTextDeltas;


    for ( LPCSTR puszEnd = uszString + nBytes; pszPos <= puszEnd; )
    {
        if ( nCurrentDeltaIndex < tStringDeltas.size() )
        {
            FormattedTextDelta * ptDelta = tStringDeltas[nCurrentDeltaIndex];
            uint32 nCurOriginalTextIndex = (uint32)( pszPos - uszString );



            if ( ptDelta->nOriginalTextIndex <= nCurOriginalTextIndex &&
                 pszPos + ptDelta->nOriginalBytesReplaced <= puszEnd )
            {
                switch ( ptDelta->eType )
                {
                case TEXTDELTA_FONTFACE_PUSH:
                    {
                        FormattedFontFacePushDelta * pFontPushDelta = (FormattedFontFacePushDelta *)ptDelta;


                        pdefFont = (const FontDefinition *)gptGroupDefFont->LockRFast( pFontPushDelta->snoFont );
                    }
                    break;
                case TEXTDELTA_FONTICON:
                    {
                        FormattedIconDelta * pIconDelta = (FormattedIconDelta *)ptDelta;
                        ImageTextureFrame tImageTexFrameIcon;
                        if ( sGetIconImage( pIconDelta->hImage, pIconDelta->eCollection, tImageTexFrameIcon ) )
                        {
                            FontF26Dot6 nIconSize = sGetIconSize( ptPointSize, tProps.flScale * pIconDelta->flScale );
                            FontF26Dot6 nIconAdvance = (FontF26Dot6)( nIconSize * sGetIconAspectRatio( tImageTexFrameIcon ) );

                            int32 nPrevX = vecSubPixelPos.x;
                            vecSubPixelPos.x += nIconAdvance;
                            if ( vecSPMouse.x < ( vecSubPixelPos.x + nPrevX ) / 2 )
                                return TRUE;

                            nFormattedIndex += ptDelta->nFormattedTextLength;
                        }
                    }
                    break;


                case TEXTDELTA_HYPERLINK:
                    {
                        FormattedHyperlinkDelta * pHyperlinkDelta = (FormattedHyperlinkDelta *)ptDelta;
                        int32 nHyperlinkFormattedIndex = 0;
                        BOOL fDone = sFontStringGetPosWithinLine(
                            tProps,
                            pHyperlinkDelta->tFormattedText,
                            pdefFont,
                            vecSPBaseline,
                            vecSubPixelPos,
                            fFirstChar,
                            ptCharPrev,
                            nHyperlinkFormattedIndex,
                            vecMousePixels,
                            ptPointSize );

                        nFormattedIndex += nHyperlinkFormattedIndex;
                        if ( fDone )
                            return TRUE;
                    }
                    break;


                case TEXTDELTA_CHARACTER:
                    {
                        FormattedCharacterDelta * pCharacterDelta = (FormattedCharacterDelta *)ptDelta;
                        const FontChar * ptChar = pdefFont->LookupCharacter( ptPointSize, pCharacterDelta->uChar );
                        if ( ptChar )
                        {
                            if ( !fFirstChar )
                            {
                                IVector2D vecKerning = ptPointSize->GetKerning( *ptCharPrev, *ptChar );
                                vecSubPixelPos.x += FontF26Dot6Multiply( vecKerning.x, flPropScale );
                                vecSubPixelPos.y += FontF26Dot6Multiply( vecKerning.y, flPropScale );
                            }
                            fFirstChar = FALSE;
                            ptCharPrev = ptChar;

                            int32 nPrevX = vecSubPixelPos.x;
                            sUpdateCursorPosition( pdefFont, flPropScale, flPropAdvanceScalar, flSpaceScalar, vecSPBaseline, ptChar, pCharacterDelta->uChar, vecSubPixelPos, ptPointSize );
                            if ( vecSPMouse.x < ( vecSubPixelPos.x + nPrevX ) / 2 )
                                return TRUE;



                            nFormattedIndex++;
                        }
                    }
                    break;

                case TEXTDELTA_BULLET_INDENT:
                    {
                        FormattedBulletIndentDelta * pIndentDelta = (FormattedBulletIndentDelta *)ptDelta;


                        int32 nPrevX = vecSubPixelPos.x;
                        vecSubPixelPos.x += sGetBulletIndentAdvance( pIndentDelta->nSpaces, pdefFont, tProps, ptPointSize );

                        if ( vecSPMouse.x < ( vecSubPixelPos.x + nPrevX ) / 2 )
                            return TRUE;

                        nFormattedIndex += ptDelta->nFormattedTextLength;
                    }
                    break;
                }







                pszPos += ptDelta->nOriginalBytesReplaced;
                nCurrentDeltaIndex++;
                continue;
            }
        }

        if ( pszPos == puszEnd )
            break;



        LPCSTR pszPosSaved = pszPos;
        uint32 uChar = Blizzard::Unicode::GetCodepointFromUTF8( &pszPos );
        if ( !uChar )
            break;


        if ( tFormattedText.IsPassword() )
            uChar = cguPasswordCharacter;



        const FontChar * ptChar = pdefFont->LookupCharacter( ptPointSize, uChar );
        if ( !ptChar )
        {
            uChar = cguInvalidGlyphCharacter;
            ptChar = pdefFont->LookupCharacter( ptPointSize, uChar );
        }



        if ( !fFirstChar )
        {
            IVector2D vecKerning = ptPointSize->GetKerning( *ptCharPrev, *ptChar );
            vecSubPixelPos.x += FontF26Dot6Multiply( vecKerning.x, flPropScale );
            vecSubPixelPos.y += FontF26Dot6Multiply( vecKerning.y, flPropScale );
        }
        fFirstChar = FALSE;
        ptCharPrev = ptChar;

        int32 nPrevX = vecSubPixelPos.x;
        sUpdateCursorPosition( pdefFont, flPropScale, flPropAdvanceScalar, flSpaceScalar, vecSPBaseline, ptChar, uChar, vecSubPixelPos, ptPointSize );
        if ( vecSPMouse.x < ( vecSubPixelPos.x + nPrevX ) / 2 )
            return TRUE;



        nFormattedIndex += (int32)( pszPos - pszPosSaved );
    }

    return FALSE;
}




int32 FontStringGetPosWithinLine(
    const TextRenderProperties & tProps,
    const FormattedText & tFormattedText,
    const Vector2D & vecMousePixels )
{
    int32 nBytes = tFormattedText.Length();


    if ( nBytes <= 0 )
        return 0;



    if ( vecMousePixels.x <= 0.0f )
        return 0;



    const FontDefinition * pdefFont = (const FontDefinition *)gptGroupDefFont->LockRFast( tProps.snoFont );  // no DefinitionLockR inline in the DWARF here
    FontPointSizeData * ptPointSize = pdefFont->GetPointSizeData( tProps.nFontSize );



    if ( !ptPointSize )
        return 0;





    FontF26Dot6 nFontAscender = (FontF26Dot6)( tProps.flScale * ptPointSize->nFontAscender );
    IVector2D vecSPBaseline = IVector2D( 0, nFontAscender );
    IVector2D vecSubPixelPos = vecSPBaseline;
    BOOL fFirstChar = TRUE;
    const FontChar * ptCharPrev;
    int32 nFormattedIndex = 0;

    sFontStringGetPosWithinLine( tProps, tFormattedText, pdefFont, vecSPBaseline, vecSubPixelPos, fFirstChar, ptCharPrev, nFormattedIndex, vecMousePixels, ptPointSize );















    return nFormattedIndex;
}




static void sUpdateCursorPosition(
    const FontDefinition * pdefFont,
    FontF26Dot6 flPropScale,
    FontF26Dot6 flPropAdvanceScalar,
    FontF26Dot6 flSpaceScalar,
    IVector2D & vecSPBaseline,
    const FontChar * ptChar,
    uint32 uChar,
    IVector2D & vecSubPixelPos,
    FontPointSizeData * ptPointSize )
{
    if ( uChar == '\n' )
    {



        vecSPBaseline.y += FontF26Dot6Multiply( ptPointSize->nAdvanceY, flPropScale );

        vecSubPixelPos = vecSPBaseline;
        return;
    }
    // Inference (Font.cpp:1981-2012): nAdvance and flScalar leave no line of their own (their
    // multiplies are hoisted out of every caller's loop); written as the 26.6 advance times the scalar.
    FontF26Dot6 nAdvance;
    if ( !ptChar )
    {








        const FontChar * ptMyChar = pdefFont->LookupCharacter( ptPointSize, uChar );
        if ( !ptMyChar )
            return;
        ptChar = ptMyChar;
    }









    FontF26Dot6 flScalar = FontF26Dot6Multiply( flPropAdvanceScalar, flPropScale );
    if ( uChar == ' ' )
        flScalar = FontF26Dot6Multiply( flScalar, flSpaceScalar );

    nAdvance = Int_ToFontF26Dot6( ptChar->nAdvanceComp );

    vecSubPixelPos.x += FontF26Dot6Multiply( nAdvance, flScalar );
}




BOOL FontSpecifierIsFont( FontSpecifier eFS )
{
    return eFS >= FONTTYPEFACE_FIRST && eFS < FONTTYPEFACE_FIRST + (int32)( sizeof( scgeFontTable ) / sizeof( scgeFontTable[0] ) );
}





SNO FontGetSnoForFontSpecifier( FontSpecifier eFS )
{
    return GlobalSNOGet( scgeFontTable[eFS - FONTTYPEFACE_FIRST] );
}





float FontGetLineAdvanceUIC( const TextRenderProperties & tProps )
{
    ScopedFontDefinitionLockR pdefFont( tProps.snoFont );
    return FontGetLineAdvanceUIC( tProps, pdefFont->GetPointSizeData( tProps.nFontSize ) );
}




float FontGetLineAdvanceUIC( const TextRenderProperties & tProps, FontPointSizeData * ptPointSize )
{
    if ( !ptPointSize )
        return 0.0f;

    uint32 nHeight = FontF26Dot6_To_Int( ptPointSize->nAdvanceY );
    return tProps.flScale * nHeight / UIGetBaseResolutionScaleFrac();
}















float FontGetAscenderUIC( const TextRenderProperties & tProps, FontPointSizeData * ptPointSize )
{
    if ( !ptPointSize )
        return 0.0f;

    return tProps.flScale * FontF26Dot6_To_Int( ptPointSize->nFontAscender ) / UIGetBaseResolutionScaleFrac();
}






float FontGetHeightUIC( const TextRenderProperties & tProps, const int32 nLines )
{
    FontPointSizeData * ptPointSize;
    float flHeight;
    float flLineGap;

    if ( nLines <= 0 )
        return 0.0f;

    flHeight = FontGetLineAdvanceUIC( tProps );
    ScopedFontDefinitionLockR pdefFont( tProps.snoFont );
    ptPointSize = pdefFont->GetPointSizeData( tProps.nFontSize );
    if ( !ptPointSize )
        return 0.0f;

    flHeight *= nLines;
    flLineGap = tProps.flScale * FontF26Dot6_To_Int( ptPointSize->nLineGap ) / UIGetBaseResolutionScaleFrac();
    return flHeight - flLineGap;
}












float FontGetCursorHeightUIC( const TextRenderProperties & tProps )
{
    FontPointSizeData * ptPointSize;
    uint32 nHeight;
    ScopedFontDefinitionLockR pdefFont( tProps.snoFont );
    ptPointSize = pdefFont->GetPointSizeData( tProps.nFontSize );
    if ( !ptPointSize )
        return 0.0f;

    nHeight = ptPointSize->nFontAscender - 2 * ptPointSize->nFontDescender;
    return tProps.flScale * FontF26Dot6_To_Int( nHeight ) / UIGetBaseResolutionScaleFrac();
}



void FontColorStack::Empty()
{
    m_nColorCount = 0;
}




void FontColorStack::Push( const RGBAColor & rgbaColor )
{
    m_rgbaColor[m_nColorCount] = rgbaColor;
    m_nColorCount++;
}





RGBAColor FontColorStack::Pop()
{
    if ( m_nColorCount == 0 )
        return crgbaWhite;

    m_nColorCount--;
    return m_rgbaColor[m_nColorCount];
}






const RGBAColor & FontColorStack::GetTop() const
{

    if ( m_nColorCount == 0 )
        return crgbaWhite;




    return m_rgbaColor[m_nColorCount - 1];
}





int32 FontGetNextSmallestFontSize( SNO snoFont, int32 nFontSize )
{
    const FontDefinition * pdefFont = DefinitionLockR<FontDefinition>( snoFont );
    pdefFont->GetNextSmallestFontSize( nFontSize );
    return nFontSize;
}
