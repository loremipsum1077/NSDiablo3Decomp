// Original file: X/Client/UserInterface/Controls/UIAutoScrollingTextControl.cpp
#include "UIAutoScrollingTextControl.h"
#include "../../CGame.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "../../../Sigma/Math/MathGeneral.h"















inline void AutoScrollingTextCreationParams::sInit()
{
    dwInitialDelay = 120;
    dwDelayAtEnd = 120;
    flScrollRate = 1.0f;
    bScrollHorizontal = FALSE;
    bRepeat = FALSE;
}





AutoScrollingTextCreationParams::AutoScrollingTextCreationParams( const UIType eType, const UIID & id, const UIID & idParent )
    : TextCreationParams( eType, id, idParent )
{
    sInit();
}



















void AutoScrollingTextCreationParams::Load( const SNO snoUI, const UIControlDataExt * pControlData )
{
    TextCreationParams::Load( snoUI, pControlData );

    dwInitialDelay = (uint32)( pControlData->GetFloat32Value( ATT_SCROLLER_INITIAL_DELAY ) * 60.0f );
    dwDelayAtEnd = (uint32)( pControlData->GetFloat32Value( ATT_SCROLLER_END_DELAY ) * 60.0f );
    flScrollRate = pControlData->GetFloat32Value( ATT_SCROLLER_RATE );
    bScrollHorizontal = pControlData->GetBoolValue( ATT_SCROLLER_HORIZONTAL );
    bRepeat = pControlData->GetBoolValue( ATT_SCROLLER_REPEAT );
}






void UIAutoScrollingText::sInit( const AutoScrollingTextCreationParams & tParams, const Vector2D &, const Vector2D & vecDim )
{
    TextCreationParams tTextParams( UIT_TEXT, "text", GetID() );
    ControlCreationParams tSavedParams( tTextParams );
    tTextParams = tParams;
    tTextParams.eType = tSavedParams.eType;
    tTextParams.id = tSavedParams.id;
    tTextParams.idParent = tSavedParams.idParent;
    tTextParams.fCodeConstructed = tSavedParams.fCodeConstructed;
    tTextParams.eEatsClicks = EC_IGNORE;

    UIText * ptText = new UIText( tTextParams, Vector2D( 0.0f, 0.0f ), vecDim );
    m_idTextChild = ptText->GetID();

    m_dwEndTime = 0;
    m_dwScrollStopTime = 0;
    m_dwScrollStartTime = 0;
    m_dwInitialDelay = tParams.dwInitialDelay;
    m_dwDelayAtEnd = tParams.dwDelayAtEnd;
    SetScrollRate( tParams.flScrollRate );
    SetHorizontalScrolling( tParams.bScrollHorizontal );
    m_bRepeat = tParams.bRepeat;
}
























UIAutoScrollingText::UIAutoScrollingText( const AutoScrollingTextCreationParams & tParams, const Rect2D * pRect )
    : UIWindow( tParams, pRect )
{
    Vector2D vecTL, vecDim;
    sRelativeCoordsFromCreationParams( vecTL, vecDim, pRect, tParams );
    sInit( tParams, vecTL, vecDim );
}







UIAutoScrollingText::~UIAutoScrollingText()
{
}




inline float UIAutoScrollingText::sGetScrollDistance()
{
    UIText & tText = UI_GetTextFromID( m_idTextChild );

    Vector2D vecTextSize;
    tText.GetTextRenderedSizeUIC( vecTextSize, FALSE );

    Vector2D vecTL, vecDim;
    GetRelativePosition( vecTL, vecDim );
    if ( m_bScrollHorizontal )
    {
        Vector2D vecNewDim( vecTextSize.x, vecDim.y );
        tText.SetRelativePosition( Vector2D( 0.0f, 0.0f ), vecNewDim );
        return vecNewDim.x - vecDim.x;
    }
    else
    {
        Vector2D vecNewDim( vecDim.x, vecTextSize.y );
        tText.SetRelativePosition( Vector2D( 0.0f, 0.0f ), vecNewDim );
        return vecNewDim.y - vecDim.y;
    }
}





void UIAutoScrollingText::SetText( LPCSTR uszText )
{
    UI_GetTextFromID( m_idTextChild ).SetText( uszText, FALSE );
}




void UIAutoScrollingText::SetScrollRateFromDuration(
    uint32 dwDuration )
{
    float flDistUIC = sGetScrollDistance();


    dwDuration = Math::Max(dwDuration, m_dwInitialDelay + m_dwDelayAtEnd + 1);
    float flScrollRate = 0.0f;

    if (flDistUIC > EPSILON)
    {
        flScrollRate = flDistUIC / (dwDuration - m_dwInitialDelay - m_dwDelayAtEnd);
    }

    SetScrollRate(flScrollRate);
}



inline void UIAutoScrollingText::SetHorizontalScrolling( const BOOL fVal )
{
    m_bScrollHorizontal = fVal;

    UIText & tText = UI_GetTextFromID( m_idTextChild );
    if ( m_bScrollHorizontal )
    {
        tText.m_eTextJustify = UIJH_LEFT;
    }
    else
    {
        tText.m_eTextJustifyVertical = UIJV_TOP;
    }
    tText.SetWordWrap( !m_bScrollHorizontal );
}





void UIAutoScrollingText::StartTimers()

{
    float flDistUIC = sGetScrollDistance();
    uint32 dwTime = CGameGetRealTicks();

    if (m_flScrollRate > EPSILON)
    {
        uint32 dwDuration = Math::FloatRoundToInt(flDistUIC / m_flScrollRate) + m_dwInitialDelay + m_dwDelayAtEnd;

        m_dwEndTime = dwTime + dwDuration;
        m_dwScrollStartTime = dwTime + m_dwInitialDelay;
        m_dwScrollStopTime = m_dwEndTime - m_dwDelayAtEnd;  // inferred: the original stores dwTime + rounded time + m_dwInitialDelay here; the operands are folded
    }
    else
    {
        m_dwEndTime = dwTime + m_dwInitialDelay + m_dwDelayAtEnd;
    }
}



BOOL UIAutoScrollingText::HandleMessage(
    const UIMsg & msg,
    const uintptr dwParam )
{
    if ( msg == UI_UPDATE )
    {


        if ( m_flScrollRate > EPSILON )
        {
            uint32 dwTime = CGameGetRealTicks();
            UIText & tText = UI_GetTextFromID( m_idTextChild );

            Vector2D vecTextDim, vecTL, vecDim;

            tText.GetRelativePosition( vecTL, vecDim );

            if ( dwTime > m_dwScrollStartTime &&
                 dwTime < m_dwScrollStopTime )
            {

                float flDist = (float)( dwTime - m_dwScrollStartTime ) * m_flScrollRate;
                if ( m_bScrollHorizontal )
                {
                    vecTL.Set( -flDist, 0.0f );  // inferred: the DWARF records one Set (line 268); both stores merge into one stp
                }
                else
                {
                    vecTL.Set( 0.0f, -flDist );
                }

                tText.SetRelativePosition( vecTL, vecDim );
            }
            else if ( dwTime >= m_dwEndTime )
            {
                if ( m_bRepeat )
                {
                    tText.SetRelativePosition( Vector2D( 0.0f, 0.0f ), vecDim );
                    StartTimers();
                }
                else
                {
                    m_flScrollRate = 0.0f;
                }
            }
        }
    }






    return UIWindow::HandleMessage( msg, dwParam );
}
