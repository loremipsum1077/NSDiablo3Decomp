// Original file: X/Client/UserInterface/Controls/UIScrollableTextControl.cpp
#include "UIScrollableTextControl.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "../../../Sigma/Math/MathGeneral.h"

// Declared here until its header carries it (signature from the DWARF).
UIID UIIDCreateFromControlAttribute( const UIControlSourceKey & tSrcKey, const UIID & id, const UIAttributeType eAttributeType );
















static const int32 SCROLLBAR_BUTTON_INCREMENT = 1;
static const int32 SCROLLBAR_PAGE_INCREMENT = 4;




ScrollableTextCreationParams::ScrollableTextCreationParams( const UIType eType, const UIID & id, const UIID & idParent )
    : TextCreationParams( eType, id, idParent )
    , eScrollbarStyle( SVS_DEFAULT )
    , eScrollbarVisibility( SBV_HIDE_WHEN_NOT_NEEDED )
{
}







void ScrollableTextCreationParams::Load( const SNO snoUI, const UIControlDataExt * pControlData )
{
    TextCreationParams::Load( snoUI, pControlData );

    int32 nStyle = pControlData->GetInt32Value( ATT_STYLE );
    if ( nStyle < 0 || nStyle >= SVS_COUNT )
        nStyle = SVS_DEFAULT;
    eScrollbarStyle = (ScrollbarVisualStyle)nStyle;
    eScrollbarVisibility = (ScrollbarVisibility)pControlData->GetInt32Value( ATT_SCROLLBAR_VISIBILITY );
}























void UIScrollableText::sInit( const ScrollableTextCreationParams & tParams, const Vector2D &, const Vector2D & vecDim )
{
    const UIID & id = GetID();

    Vector2D vecScrollbarTL, vecScrollbarDim;
    UIScrollbar::GetScrollbarRect( vecScrollbarTL, vecScrollbarDim, tParams.eScrollbarStyle, vecDim );

    m_idScrollbarChild = UIIDCreateFromControlAttribute( GetUISrcKey(), id, ATT_ID_SCROLLBAR );
    BOOL bHasScrollbar = m_idScrollbarChild != GetUIIDNone();
    if ( bHasScrollbar )
    {
        UI_GetScrollbarFromID( m_idScrollbarChild ).SetOwner( id );
    }
    else
    {
        ScrollbarCreationParams tScrollbarParams( UIT_SCROLLBAR, "scrollbar", id );
        tScrollbarParams.eRenderLayerOverride = tParams.eRenderLayerOverride;
        tScrollbarParams.eScrollbarVisibility = tParams.eScrollbarVisibility;
        UIScrollbar * ptScrollbar = new UIScrollbar( tScrollbarParams, vecScrollbarTL, vecScrollbarDim, tParams.eScrollbarStyle );
        m_idScrollbarChild = ptScrollbar->GetID();
    }

    Vector2D vecTextTL( 10.0f, 10.0f );
    Vector2D vecTextDim( vecDim.x - 20.0f, vecDim.y - 20.0f );
    if ( !bHasScrollbar )
    {
        vecTextDim.x -= vecScrollbarDim.x + 12.0f;
        vecTextTL.x += vecScrollbarDim.x * 0.5f;
    }

    TextCreationParams tTextParams( UIT_TEXT, "textArea", id );
    {
        ControlCreationParams tSavedParams( tTextParams );
        tTextParams = tParams;
        tTextParams.eType = tSavedParams.eType;
        tTextParams.id = tSavedParams.id;
        tTextParams.idParent = tSavedParams.idParent;
        tTextParams.fCodeConstructed = tSavedParams.fCodeConstructed;
        tTextParams.eEatsClicks = EC_IGNORE;
        UIText * ptText = new UIText( tTextParams, vecTextTL, vecTextDim );
        m_idTextChild = ptText->GetID();
    }
}

































void UIScrollableText::PostCreate()
{
    UIWindow::PostCreate();

    UI_GetScrollbarFromID( m_idScrollbarChild ).SetExtentsAndPosition( 0, 0, 0, TRUE );
}







UIScrollableText::UIScrollableText( const ScrollableTextCreationParams & tParams, const Rect2D * pRect )
    : UIWindow( tParams, pRect )
{
    Vector2D vecTL, vecDim;
    sRelativeCoordsFromCreationParams( vecTL, vecDim, pRect, tParams );
    sInit( tParams, vecTL, vecDim );
}







UIScrollableText::~UIScrollableText()
{
}




void UIScrollableText::SetText( LPCSTR uszText )
{
    UI_GetTextFromID( m_idTextChild ).SetText( uszText, FALSE );
    UpdateScrollbarExtents();
}






inline void UIScrollableText::UpdateScrollbarExtents()
{
    UIText & tText = UI_GetTextFromID( m_idTextChild );
    UIScrollbar & tScrollbar = UI_GetScrollbarFromID( m_idScrollbarChild );

    int32 nMax = Math::Max( tText.GetLineCount() - tText.GetNumVisibleLines(), 0 );
    tScrollbar.SetExtentsAndPosition( 0, nMax, tText.GetTopLine(), TRUE );
    tScrollbar.SetIncrements( SCROLLBAR_BUTTON_INCREMENT, SCROLLBAR_PAGE_INCREMENT );
}








































































void UIScrollableText::SetActive( const BOOL fActive )
{
    UIWindow::SetActive( fActive );

    if ( fActive )
    {
        UIText & tText = UI_GetTextFromID( m_idTextChild );
        if ( tText.GetNumVisibleLines() <= tText.GetLineCount() )
        {
            UI_GetScrollbarFromID( m_idScrollbarChild ).SetActive( FALSE );
        }
    }
}





BOOL UIScrollableText::HandleMessage( const UIMsg & msg, const uintptr dwParam )
{
    BOOL bResult = UIWindow::HandleMessage( msg, dwParam );

    if ( msg == UI_GFX_INIT )
    {
        if ( UI_GetTextFromID( m_idTextChild ).GetText().Length() != 0 )
        {
            UpdateScrollbarExtents();
        }
    }
    return bResult;
}











void UIScrollableText::OnScrollbarPositionChanged( const UIID & idScrollbar )
{
    UIScrollbar & tScrollbar = UI_GetScrollbarFromID( idScrollbar );
    int32 nPosition = tScrollbar.GetPosition();
    UI_GetTextFromID( m_idTextChild ).SetTopLine( nPosition );
}
