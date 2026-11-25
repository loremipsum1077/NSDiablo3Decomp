// Original file: X/Client/UserInterface/UIUtils.cpp

#include "UIUtils.h"
#include "../RWindow.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/UnicodeText.h"

#include <stdlib.h>
#include <string.h>

// Declared here until their headers carry them (signatures from the DWARF).
float CGameGetLetterboxedViewportNDC( Rect2D & rectScreenNDC );  // X/Client/CGame.cpp:3647
void CGameGetLetterboxedViewportPixels( IRect2D & rectScreen );  // X/Client/CGame.cpp:3677
void CGameGetSafeViewportUIC( Rect2D & rectUIC );  // X/Client/CGame.cpp:3710
BOOL ShellShouldSelectFullDisplayMode();  // X/Common/Shell/ShellNX64.cpp:337
BOOL GfxIsLetterboxed();  // X/Client/Gfx/Gfx.cpp:1937
RWindowMgr * GetRWindowMgr();  // X/Client/RWindow.cpp:1355

static void sGetUIBasisRectPixels( Rect2D & rectBasis );
float UIGetBaseResolutionScaleFrac();
CRefString FormatTruncatedNumber( const XLocale eLocale, const float flValue, const BOOL bUseMillions, const BOOL bIncludeSpecialChar );
#include "../../Common/Definitions/StringListDefinition.h"








void AlignRect( const Vector2D & vecSourceTL, const Vector2D & vecSourceDim, const AlignmentType eAlignmentType, const float flStart, const Vector2D & vecToAlignDim, Vector2D & vecTL )
{
    switch ( eAlignmentType )
    {
    case ALIGN_CENTER_VERTICAL:
        vecTL.x = flStart;
        vecTL.y = vecSourceTL.y + ( vecSourceDim.y - vecToAlignDim.y ) * 0.5f;
        break;
    case ALIGN_CENTER_HORIZONTAL:
        vecTL.x = vecSourceTL.x + ( vecSourceDim.x - vecToAlignDim.x ) * 0.5f;
        vecTL.y = flStart;
        break;
    case ALIGN_CENTER_HORIZONTAL_VERTICAL:
        vecTL.x = vecSourceTL.x + ( vecSourceDim.x - vecToAlignDim.x ) * 0.5f;
        vecTL.y = vecSourceTL.y + ( vecSourceDim.y - vecToAlignDim.y ) * 0.5f;
        break;
    default:







        XASSERT( FALSE );
        break;
    }
}



void ConvertNDCToPixels( const Rect2D & rectNDC, IRect2D & rectPixels )
{


    Vector2D vecDim = GetRWindowMgr()->GetCurrentRWindow().GetPixelDimensionsFloat();
    Rect2D rectBasis( 0.0f, 0.0f, vecDim.x, vecDim.y );
    rectNDC.FromUnitCoords( rectBasis, rectPixels );
}





void ConvertNDCToPixels( const Rect2D & rectNDC, Rect2D & rectPixels )
{


    Vector2D vecDim = GetRWindowMgr()->GetCurrentRWindow().GetPixelDimensionsFloat();
    Rect2D rectBasis( 0.0f, 0.0f, vecDim.x, vecDim.y );
    rectNDC.FromUnitCoords( rectBasis, rectPixels );
}





void ConvertPixelsToNDC( const Rect2D & rectPixels, Rect2D & rectNDC )
{
    Vector2D vecDim = GetRWindowMgr()->GetCurrentRWindow().GetPixelDimensionsFloat();
    Rect2D rectBasis( 0.0f, 0.0f, vecDim.x, vecDim.y );
    rectPixels.ToUnitCoords( rectBasis, rectNDC );
}







void ConvertPixelsToUIC( const IRect2D & rectPixels, Rect2D & rectUIC )
{
    Rect2D rectBasis;
    sGetUIBasisRectPixels( rectBasis );
    rectPixels.ToUnitCoords( rectBasis, rectUIC );
    rectUIC.left *= 960.0f;
    rectUIC.top *= 720.0f;
    rectUIC.right *= 960.0f;
    rectUIC.bottom *= 720.0f;
}







void ConvertPixelsToUIC( const IVector2D & vecPixels, Vector2D & vecUIC )
{
    Rect2D rectBasis;
    sGetUIBasisRectPixels( rectBasis );
    vecUIC.x = ( (float)vecPixels.x - rectBasis.left ) * 960.0f / rectBasis.Width();
    vecUIC.y = ( (float)vecPixels.y - rectBasis.top ) * 720.0f / rectBasis.Height();
}






void ConvertPixelsToUIC( const Vector2D & vecPixels, Vector2D & vecUIC )
{
    Rect2D rectBasis;
    sGetUIBasisRectPixels( rectBasis );
    vecUIC.x = ( vecPixels.x - rectBasis.left ) * 960.0f / rectBasis.Width();
    vecUIC.y = ( vecPixels.y - rectBasis.top ) * 720.0f / rectBasis.Height();
}







void ConvertPixelsDimensionsToUIC( const IVector2D & vecPixels, Vector2D & vecUIC )
{




    float flInvFrac = 1.0f / UIGetBaseResolutionScaleFrac();
    vecUIC.x = flInvFrac * (float)vecPixels.x;
    vecUIC.y = flInvFrac * (float)vecPixels.y;
}



void ConvertUICDimensionsToPixels( const Vector2D & vecUIC, IVector2D & vecPixels )
{



    float flFrac = UIGetBaseResolutionScaleFrac();
    vecPixels.x = Math::FloatToInt( vecUIC.x * flFrac );
    vecPixels.y = Math::FloatToInt( vecUIC.y * flFrac );
}


void ConvertUICToPixels( const Rect2D & rectUIC, IRect2D & rectPixels )
{
    Rect2D rectBasis;
    Rect2D rectNUIC;
    sGetUIBasisRectPixels( rectBasis );



    rectNUIC.Set( rectUIC.left / 960.0f, rectUIC.top / 720.0f, rectUIC.right / 960.0f, rectUIC.bottom / 720.0f );
    rectNUIC.FromUnitCoords( rectBasis, rectPixels );
}

































































































static float s_fUICToPixelsH;
static float s_fUICToPixelsV;
static float s_fUICToPixelsLeft;
static float s_fUICToPixelsTop;



void SetUICConversionConstansts()
{
    Rect2D rectBasis;
    sGetUIBasisRectPixels( rectBasis );
    s_fUICToPixelsH = rectBasis.Width() / 960.0f;
    s_fUICToPixelsV = rectBasis.Height() / 720.0f;
    s_fUICToPixelsLeft = rectBasis.left;
    s_fUICToPixelsTop = rectBasis.top;
}






void ConvertUICToPixels( const Rect2D & rectUIC, Rect2D & rectPixels )
{
    rectPixels.right = rectUIC.right * s_fUICToPixelsH + s_fUICToPixelsLeft;
    rectPixels.bottom = rectUIC.bottom * s_fUICToPixelsV;
    rectPixels.left = s_fUICToPixelsLeft + s_fUICToPixelsH * rectUIC.left;
    rectPixels.top = s_fUICToPixelsV * rectUIC.top;



    rectPixels.left = Math::FloatTrunc( rectPixels.left );
    rectPixels.right = Math::FloatTrunc( rectPixels.right );

    rectPixels.top = Math::FloatTrunc( rectPixels.top );
    rectPixels.bottom = Math::FloatTrunc( rectPixels.bottom );
}



void ConvertUICToPixels( const Vector2D & vecUIC, Vector2D & vecPixels )
{
    vecPixels.x = vecUIC.x * s_fUICToPixelsH + s_fUICToPixelsLeft;
    vecPixels.y = vecUIC.y * s_fUICToPixelsV;
    vecPixels.x = Math::FloatTrunc( vecPixels.x );
    vecPixels.y = Math::FloatTrunc( vecPixels.y );
}






void ConvertUICToPixelsDoNotSnap( const Rect2D & rectUIC, Rect2D & rectPixels )
{
    rectPixels.right = rectUIC.right * s_fUICToPixelsH + s_fUICToPixelsLeft;
    rectPixels.bottom = rectUIC.bottom * s_fUICToPixelsV;
    rectPixels.left = s_fUICToPixelsLeft + s_fUICToPixelsH * rectUIC.left;
    rectPixels.top = s_fUICToPixelsV * rectUIC.top;
}





void ConvertUICToPixelsDoNotSnap( const Vector2D & vecUIC, Vector2D & vecPixels )
{
    vecPixels.x = vecUIC.x * s_fUICToPixelsH + s_fUICToPixelsLeft;
    vecPixels.y = vecUIC.y * s_fUICToPixelsV;
}





void ConvertUICToPixels( const Rect2D & rectUIC, Rect2D & rectPixels, const BOOL fSnapToIntegerCoordsX, const BOOL fSnapToIntegerCoordsY )
{
    rectPixels.right = rectUIC.right * s_fUICToPixelsH + s_fUICToPixelsLeft;
    rectPixels.bottom = rectUIC.bottom * s_fUICToPixelsV;
    rectPixels.left = s_fUICToPixelsLeft + s_fUICToPixelsH * rectUIC.left;
    rectPixels.top = s_fUICToPixelsV * rectUIC.top;
    if ( fSnapToIntegerCoordsX )
    {
        rectPixels.left = Math::FloatTrunc( rectPixels.left );
        rectPixels.right = Math::FloatTrunc( rectPixels.right );
    }
    if ( fSnapToIntegerCoordsY )
    {
        rectPixels.top = Math::FloatTrunc( rectPixels.top );
        rectPixels.bottom = Math::FloatTrunc( rectPixels.bottom );
    }
}








void ConvertUICToPixels( const Vector2D & vecUIC, Vector2D & vecPixels, const BOOL fSnapToIntegerCoordsX, const BOOL fSnapToIntegerCoordsY )
{
    vecPixels.x = vecUIC.x * s_fUICToPixelsH + s_fUICToPixelsLeft;
    vecPixels.y = vecUIC.y * s_fUICToPixelsV;
    if ( fSnapToIntegerCoordsX )
    {
        vecPixels.x = Math::FloatTrunc( vecPixels.x );
    }
    if ( fSnapToIntegerCoordsY )
    {
        vecPixels.y = Math::FloatTrunc( vecPixels.y );
    }
}
























float UIGetBaseResolutionScaleFrac()
{
    static BOOL sfWasLetterboxed;
    static BOOL sfInitialized;
    static float flLastScaleFrac;
    static BOOL sfDidSelectFullDisplayMode;
    float flScaleFrac;
    BOOL fIsInFullDisplayMode = ShellShouldSelectFullDisplayMode();
    BOOL fIsLetterboxed = GfxIsLetterboxed();
    {
        IRect2D rectViewportPixels;
        CGameGetLetterboxedViewportPixels( rectViewportPixels );





















        int32 nHeight = Math::Max( (int32)rectViewportPixels.Height(), 1 );
        flScaleFrac = (float)nHeight / 720.0f;
    }
    return flScaleFrac;
}









































void UI_ConstrainRectToViewport( Rect2D & rect, const float flIndentUIC )
{
    Rect2D rectViewport;
    CGameGetSafeViewportUIC( rectViewport );
    rect.ConstrainToRect( rectViewport, flIndentUIC );
}






static void sGetUIBasisRectPixels( Rect2D & rectBasis )
{
    static BOOL sfWasLetterboxed;
    static BOOL sfInitialized;
    static Rect2D sLastRectBasis;
    static BOOL sfDidSelectFullDisplayMode;
    BOOL fIsInFullDisplayMode = ShellShouldSelectFullDisplayMode();
    BOOL fIsLetterboxed = GfxIsLetterboxed();
    {
        Rect2D rectViewportNDC;
        float flAspectRatio = CGameGetLetterboxedViewportNDC( rectViewportNDC );


















        ConvertNDCToPixels( rectViewportNDC, rectBasis );



        if ( Math::Abs( flAspectRatio - 4.0f / 3.0f ) > 0.01f )
        {

            float flWidth = Math::FloatTrunc( rectBasis.Height() * ( 4.0f / 3.0f ) );
            rectBasis.left = ( rectBasis.right - flWidth ) * 0.5f;
            rectBasis.right = rectBasis.left + flWidth;
        }
    }
}














void UIParseCsvElement( float & dest, LPCSTR & szCsv )
{
    int32 flMarginsLength = (int32)strlen( szCsv );
    if ( flMarginsLength != 0 )
    {
        LPCSTR marginValue = szCsv;


        LPCSTR comma = strchr( marginValue, ',' );
        if ( comma != NULL )
        {
            CHAR szValue[16];
            Blizzard::String::Copy( szValue, marginValue, comma - marginValue + 1 );
            dest = (float)atof( szValue );
            szCsv = comma + 1;
        }
        else
        {
            dest = (float)atof( marginValue );
            szCsv = "";
        }
    }
}




void FadeRamp::Set( uint32 dwFadeInTime, uint32 dwFadeOutStartTime, uint32 dwFadeOutEndTime )
{
    m_dwFadeInTime = dwFadeInTime;
    m_dwFadeOutStartTime = dwFadeOutStartTime;
    m_dwFadeOutEndTime = dwFadeOutEndTime;
}









FadeState FadeRamp::Evaluate( const uint32 dwElapsedTicks, float & flFrac ) const
{
    if ( dwElapsedTicks < m_dwFadeInTime )
    {
        flFrac = (float)dwElapsedTicks / (float)m_dwFadeInTime;
        return FADESTATE_SHOWING;
    }
    if ( dwElapsedTicks < m_dwFadeOutStartTime )
    {
        flFrac = 1.0f;
        return FADESTATE_SHOWN;
    }
    if ( dwElapsedTicks < m_dwFadeOutEndTime )
    {
        flFrac = (float)( m_dwFadeOutEndTime - dwElapsedTicks ) / (float)( m_dwFadeOutEndTime - m_dwFadeOutStartTime );
        return FADESTATE_HIDING;
    }
    flFrac = 0.0f;
    return FADESTATE_HIDDEN;
}








static CRefString sFormatTruncatedNumber_Asia( const float flValue, const BOOL bUseLargeUnitGlyphs, const BOOL bIncludeSpecialChar )



{























    CRefString sStr;
    float flAbs = Math::Abs( flValue );

    if ( flAbs >= 1e16f && bUseLargeUnitGlyphs )
    {

        float flTruncatedValue = flValue / 1e16f;
        sStr = UnicodeText::FormatNumber( flTruncatedValue, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbrTrillions" ).str(), sStr.str(), NULL );
        }
    }
    else if ( flAbs >= 1e15f && bUseLargeUnitGlyphs )
    {
        int32 nValue = Math::FloatRoundToInt( flValue / 1e15f );
        sStr = UnicodeText::FormatNumber( nValue, TRUE, FNS_NORMAL, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbrBillions" ).str(), sStr.str(), NULL );
        }
    }
    else if ( flAbs >= 1e12f && bUseLargeUnitGlyphs )
    {

        float flTruncatedValue = flValue / 1e12f;
        sStr = UnicodeText::FormatNumber( flTruncatedValue, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbrBillions" ).str(), sStr.str(), NULL );
        }
    }
    else if ( flAbs >= 1e8f && bUseLargeUnitGlyphs )
    {

        float flTruncatedValue = flValue / 1e8f;
        sStr = UnicodeText::FormatNumber( flTruncatedValue, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbrMillions" ).str(), sStr.str(), NULL );
        }
    }
    else if ( flAbs >= 1e7f )
    {

        int32 nValue = Math::FloatRoundToInt( flValue / 10000.0f );
        sStr = UnicodeText::FormatNumber( nValue, TRUE, FNS_NORMAL, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbr" ).str(), sStr.str(), NULL );
        }
    }
    else if ( flAbs >= 100000.0f )
    {

        float flTruncatedValue = flValue / 10000.0f;
        sStr = UnicodeText::FormatNumber( flTruncatedValue, TRUE, FNS_NORMAL, 1, 1, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbr" ).str(), sStr.str(), NULL );
        }
    }
    else if ( flAbs >= 10000.0f )
    {

        float flTruncatedValue = flValue / 10000.0f;
        sStr = UnicodeText::FormatNumber( flTruncatedValue, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbr" ).str(), sStr.str(), NULL );
        }
    }
    else
    {
        int32 nValue = Math::FloatRoundToInt( flValue );
        sStr = UnicodeText::FormatNumber( nValue, TRUE, FNS_NORMAL, XLOCALE_INVALID );
    }


    return sStr;
}


CRefString sFormatTruncatedNumber_generic( const float flValue, const BOOL bUseLargeUnitGlyphs, const BOOL bIncludeSpecialChar )



{
    CRefString sStr;
    float flAbs = Math::Abs( flValue );

    if ( flAbs >= 1e14f && bUseLargeUnitGlyphs )
    {

        int32 nValue = Math::FloatRoundToInt( flValue / 1e12f );
        sStr = UnicodeText::FormatNumber( nValue, TRUE, FNS_NORMAL, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbrTrillions" ).str(), sStr.str(), NULL );
        }
    }
    else if ( flAbs >= 1e13f && bUseLargeUnitGlyphs )
    {

        float flTruncatedValue = flValue / 1e12f;
        sStr = UnicodeText::FormatNumber( flTruncatedValue, TRUE, FNS_NORMAL, 1, 1, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbrTrillions" ).str(), sStr.str(), NULL );
        }
    }
    else if ( flAbs >= 1e11f && bUseLargeUnitGlyphs )
    {

        int32 nValue = Math::FloatRoundToInt( flValue / 1e9f );
        sStr = UnicodeText::FormatNumber( nValue, TRUE, FNS_NORMAL, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbrBillions" ).str(), sStr.str(), NULL );
        }
    }
    else if ( flAbs >= 1e10f && bUseLargeUnitGlyphs )
    {

        float flTruncatedValue = flValue / 1e9f;
        sStr = UnicodeText::FormatNumber( flTruncatedValue, TRUE, FNS_NORMAL, 1, 1, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbrBillions" ).str(), sStr.str(), NULL );
        }
    }
    else if ( flAbs >= 1e8f && bUseLargeUnitGlyphs )
    {

        int32 nValue = Math::FloatRoundToInt( flValue / 1e6f );
        sStr = UnicodeText::FormatNumber( nValue, TRUE, FNS_NORMAL, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbrMillions" ).str(), sStr.str(), NULL );
        }
    }
    else if ( flAbs >= 1e7f && bUseLargeUnitGlyphs )
    {

        float flTruncatedValue = flValue / 1e6f;
        sStr = UnicodeText::FormatNumber( flTruncatedValue, TRUE, FNS_NORMAL, 1, 1, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbrMillions" ).str(), sStr.str(), NULL );
        }
    }
    else if ( flAbs > 100000.0f )
    {

        int32 nValue = Math::FloatRoundToInt( flValue / 1000.0f );
        sStr = UnicodeText::FormatNumber( nValue, TRUE, FNS_NORMAL, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbr" ).str(), sStr.str(), NULL );
        }
    }
    else if ( flAbs > 10000.0f )
    {
        float flTruncatedValue = flValue / 1000.0f;
        sStr = UnicodeText::FormatNumber( flTruncatedValue, TRUE, FNS_NORMAL, 1, 1, XLOCALE_INVALID );
        if ( bIncludeSpecialChar )
        {
            sStr = LocalizedSprintf( StringListFetch( "General:StatAbbr" ).str(), sStr.str(), NULL );
        }
    }
    else
    {
        int32 nValue = Math::FloatRoundToInt( flValue );
        sStr = UnicodeText::FormatNumber( nValue, TRUE, FNS_NORMAL, XLOCALE_INVALID );
    }

    return sStr;
}



CRefString FormatTruncatedNumber( const float flValue, const BOOL bUseMillions, const BOOL bIncludeSpecialChar )
{
    return FormatTruncatedNumber( UnicodeText::GetCurrentLocale(), flValue, bUseMillions, bIncludeSpecialChar );
}







CRefString FormatTruncatedNumber( const XLocale eLocale, const float flValue, const BOOL bUseMillions, const BOOL bIncludeSpecialChar )




{
    if ( eLocale == XLOCALE_KOREAN || eLocale == XLOCALE_CHINESE_CHINA || eLocale == XLOCALE_CHINESE_TAIWAN || eLocale == 19 /* 2.7.7 XLocale 19, enumerator missing from UnicodeText.h */ )
    {
        return sFormatTruncatedNumber_Asia( flValue, bUseMillions, bIncludeSpecialChar );
    }


    return sFormatTruncatedNumber_generic( flValue, bUseMillions, bIncludeSpecialChar );

}