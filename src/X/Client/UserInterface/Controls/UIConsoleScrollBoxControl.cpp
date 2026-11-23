// Original file: X/Client/UserInterface/Controls/UIConsoleScrollBoxControl.cpp
#include "UIConsoleScrollBoxControl.h"
#include "UIConsoleScrollIndicatorControl.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../Input/Input.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "../UICreation.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Rect.h"

// Declared here until their headers carry them (signatures from the DWARF).
const UIID & UI_GetControllerFocus();  // UI.cpp:429
UIConsoleScrollIndicator & UI_GetConsoleScrollIndicatorFromID( const UIID & id );  // UI.cpp














ConsoleScrollBoxCreationParams::ConsoleScrollBoxCreationParams( const UIType eType, const UIID & id, const UIID & idParent )
    : WindowCreationParams( eType, id, idParent )
{
    sInit();
}
















static const float SCROLLBOX_SCROLLDELTA = 4.0f;


void ConsoleScrollBoxCreationParams::Load( const SNO snoUI, const UIControlDataExt * pControlData )
{
    WindowCreationParams::Load( snoUI, pControlData );

    bRequiresControllerFocus = pControlData->GetBoolValue( ATT_REQUIRES_CONTROLLER_FOCUS );
    flScrollIndicatorPaddingTop = pControlData->GetFloat32Value( ATT_SCROLLINDICATOR_PADDING_TOP );
    flScrollIndicatorPaddingLeft = pControlData->GetFloat32Value( ATT_SCROLLINDICATOR_PADDING_LEFT );
    flScrollIndicatorPaddingRight = pControlData->GetFloat32Value( ATT_SCROLLINDICATOR_PADDING_RIGHT );
    flScrollIndicatorPaddingBottom = pControlData->GetFloat32Value( ATT_SCROLLINDICATOR_PADDING_BOTTOM );
    pControlData->GetImageAttribute( ATT_SCROLLDIMMER_TOP, tImageTexDimmerTop );
    pControlData->GetImageAttribute( ATT_SCROLLDIMMER_BOTTOM, tImageTexDimmerBottom );
}








inline void ConsoleScrollBoxCreationParams::sInit()
{
    bRequiresControllerFocus = TRUE;
    flScrollIndicatorPaddingTop = 0.0f;
    flScrollIndicatorPaddingLeft = 0.0f;
    flScrollIndicatorPaddingRight = 0.0f;
    flScrollIndicatorPaddingBottom = 0.0f;
}


















UIConsoleScrollBox::UIConsoleScrollBox( const ConsoleScrollBoxCreationParams & tParams, const Rect2D * pRect )
    : UIWindow( tParams, pRect )
{
    Vector2D vecTL, vecDim;
    sRelativeCoordsFromCreationParams( vecTL, vecDim, pRect, tParams );
    sInit( tParams, vecTL, vecDim );
}







UIConsoleScrollBox::~UIConsoleScrollBox()
{
}





void UIConsoleScrollBox::sInit( const ConsoleScrollBoxCreationParams & tParams, const Vector2D &, const Vector2D & vecDim )
{
    const BOOL bRequiresControllerFocus = tParams.bRequiresControllerFocus;
    m_bExternallyControlled = TRUE;
    m_bRequiresControllerFocus = bRequiresControllerFocus;
    m_flScrollIndicatorPaddingTop = tParams.flScrollIndicatorPaddingTop;
    m_flScrollIndicatorPaddingLeft = tParams.flScrollIndicatorPaddingLeft;
    m_flScrollIndicatorPaddingRight = tParams.flScrollIndicatorPaddingRight;
    m_flScrollIndicatorPaddingBottom = tParams.flScrollIndicatorPaddingBottom;
    m_flPos = 0.0f;
    m_nMouseWheelScaleFactor = 48.0f;

    const UIID & id = GetID();

    ConsoleScrollIndicatorCreationParams tScrollerParams( UIT_CONSOLESCROLLINDICATOR, "_scroller", id );
    tScrollerParams.eScrollbarVisibility = SBV_NEVER_SHOW;
    UIConsoleScrollIndicator * ptScroller = new UIConsoleScrollIndicator( tScrollerParams, Vector2D( 0.0f, 0.0f ), Vector2D( 0.0f, 0.0f ) );
    m_idScrollIndicator = ptScroller->GetID();

    if ( tParams.tImageTexDimmerTop != cgtImageTextureFrameNull )
    {
        WindowCreationParams tDimmerParams( UIT_WINDOW, "_scrollerTop", id );
        tDimmerParams.tImageTexBkgrnd = tParams.tImageTexDimmerTop;
        tDimmerParams.dwFlags = !( m_dwFlags & ( 1 << UIWINDOW_CLEAR_HIGHLIGHT_LAYER_BIT ) ) ? ( tDimmerParams.dwFlags & ~( 1 << UIWINDOW_CLEAR_HIGHLIGHT_LAYER_BIT ) ) : ( tDimmerParams.dwFlags | ( 1 << UIWINDOW_CLEAR_HIGHLIGHT_LAYER_BIT ) );
        m_idScrollIndicatorTop = CreateControl<UIWindow, WindowCreationParams>( tDimmerParams, Vector2D( 0.0f, 0.0f ), Vector2D( vecDim.x, 0.0f ) );
    }
    else
    {
        m_idScrollIndicatorTop = GetUIIDNone();
    }

    if ( tParams.tImageTexDimmerBottom != cgtImageTextureFrameNull )
    {
        WindowCreationParams tDimmerParams( UIT_WINDOW, "_scrollerBottom", id );
        tDimmerParams.tImageTexBkgrnd = tParams.tImageTexDimmerBottom;
        tDimmerParams.dwFlags = !( m_dwFlags & ( 1 << UIWINDOW_CLEAR_HIGHLIGHT_LAYER_BIT ) ) ? ( tDimmerParams.dwFlags & ~( 1 << UIWINDOW_CLEAR_HIGHLIGHT_LAYER_BIT ) ) : ( tDimmerParams.dwFlags | ( 1 << UIWINDOW_CLEAR_HIGHLIGHT_LAYER_BIT ) );
        m_idScrollIndicatorBottom = CreateControl<UIWindow, WindowCreationParams>( tDimmerParams, Vector2D( 0.0f, 0.0f ), Vector2D( vecDim.x, 0.0f ) );
    }
    else
    {
        m_idScrollIndicatorBottom = GetUIIDNone();
    }

    PositionChildControls();
}









void UIConsoleScrollBox::SetRelativePosition( const Vector2D & vecTL, const Vector2D & vecDim )
{
    Vector2D vecCurrentDim = m_vecDim;
    UIWindow::SetRelativePosition( vecTL, vecDim );

    if ( Math::Abs( vecCurrentDim.x - m_vecDim.x ) > EPSILON ||
         Math::Abs( vecCurrentDim.y - m_vecDim.y ) > EPSILON )
    {
        PositionChildControls();
    }
}






BOOL UIConsoleScrollBox::HandleMessage( const UIMsg & msg, const uintptr dwParam )
{
    switch ( msg )
    {
    case UI_INPUT:
        {
            const BufferedInput & tInput = *(const BufferedInput *)dwParam;
            if ( HandleInput( tInput ) )
            {
                return TRUE;
            }
        }
        break;
    case UI_UPDATE:
        UpdateScrollState();
        break;
    case UI_REGISTER_INPUT_LISTENER:
        UI_RegisterInputListener( this );
        break;
    }
    return UIWindow::HandleMessage( msg, dwParam );
}

















void UIConsoleScrollBox::PostCreate()
{
    if ( UI_TestID( m_idScrollIndicatorTop ) )
    {
        RemoveChild( m_idScrollIndicatorTop );
        AddChild( m_idScrollIndicatorTop );
    }
    if ( UI_TestID( m_idScrollIndicatorBottom ) )
    {
        RemoveChild( m_idScrollIndicatorBottom );
        AddChild( m_idScrollIndicatorBottom );
    }
}






inline BOOL UIConsoleScrollBox::HandleInput( const BufferedInput & tInput )
{
    if ( m_bExternallyControlled )
    {
        return FALSE;
    }

    BOOL bHasControllerFocus = UI_GetControllerFocus() == GetID();
    if ( bHasControllerFocus || !m_bRequiresControllerFocus )
    {
        if ( tInput.tBufControllerData.eInputButton == 6 )
        {
            Scroll( -SCROLLBOX_SCROLLDELTA );
            return TRUE;
        }
        else if ( tInput.tBufControllerData.eInputButton == 7 )
        {
            Scroll( SCROLLBOX_SCROLLDELTA );
            return TRUE;
        }
    }
    return FALSE;
}






















void UIConsoleScrollBox::Scroll( const float flDelta )
{
    float flLimit = GetScrollLimit();
    if ( flLimit > EPSILON )
    {
        SetScrollPos( Math::Pin( m_flPos + flDelta, 0.0f, flLimit ) );
    }
}






float UIConsoleScrollBox::GetScrollLimit() const
{

    Rect2D rectBounds;
    CalculateChildBounds( rectBounds );

    return Math::Max( rectBounds.Height() - m_vecDim.y, 0.0f );
}




void UIConsoleScrollBox::SetScrollPos( const float flPos )
{
    float flDelta = m_flPos - flPos;
    m_flPos = flPos;

    const UIControlItemArray & arChildren = GetChildList();
    for ( uint32 i = 0; i < arChildren.size(); i++ )
    {
        const UIID & idChild = arChildren[i].id;
        if ( idChild != m_idScrollIndicator && idChild != m_idScrollIndicatorTop && idChild != m_idScrollIndicatorBottom )
        {
            UIWindow & tChild = UI_GetWindowFromID( idChild );
            Vector2D vecDelta( 0.0f, flDelta );
            tChild.Move( vecDelta );
        }
    }
}










void UIConsoleScrollBox::UpdateScrollState()
{
    float flLimit = GetScrollLimit();

    UIConsoleScrollIndicator & tScroller = UI_GetConsoleScrollIndicatorFromID( m_idScrollIndicator );
    tScroller.SetExtentsAndPosition( (int32)flLimit, (int32)m_flPos );

    if ( UI_TestID( m_idScrollIndicatorTop ) )
    {
        UI_GetWindowFromID( m_idScrollIndicatorTop ).SetActive( m_flPos > EPSILON );
    }
    if ( UI_TestID( m_idScrollIndicatorBottom ) )
    {
        UI_GetWindowFromID( m_idScrollIndicatorBottom ).SetActive( flLimit - m_flPos > EPSILON );
    }
}


inline void UIConsoleScrollBox::CalculateChildBounds( Rect2D & rectBounds ) const
{
    rectBounds = Rect2D( 0.0f, 0.0f, 0.0f, 0.0f );



    const UIControlItemArray & arChildren = GetChildList();

    for ( uint32 i = 0; i < arChildren.size(); i++ )
    {
        const UIID & idChild = arChildren[i].id;
        if ( idChild != m_idScrollIndicatorTop && idChild != m_idScrollIndicatorBottom )
        {
            UIWindow & tChild = UI_GetWindowFromID( idChild );
            Vector2D vecTL, vecDim;
            tChild.GetRelativePosition( vecTL, vecDim );
            rectBounds.left = Math::Min( vecTL.x, rectBounds.left );
            rectBounds.top = Math::Min( vecTL.y, rectBounds.top );
            rectBounds.right = Math::Max( vecTL.x + vecDim.x, rectBounds.right );
            rectBounds.bottom = Math::Max( vecTL.y + vecDim.y, rectBounds.bottom );
        }
    }
}




void UIConsoleScrollBox::PositionChildControls()
{
    if ( UI_TestID( m_idScrollIndicator ) )
    {
        UIConsoleScrollIndicator & tScroller = UI_GetConsoleScrollIndicatorFromID( m_idScrollIndicator );
        Vector2D vecThumb;
        tScroller.GetThumbSize( vecThumb );
        Vector2D vecTL, vecDim;
        vecTL.x = m_vecDim.x - vecThumb.x - m_flScrollIndicatorPaddingRight;
        vecTL.y = m_flScrollIndicatorPaddingTop;
        vecDim.x = vecThumb.x;
        vecDim.y = m_vecDim.y - vecTL.y - m_flScrollIndicatorPaddingBottom;
        tScroller.SetRelativePosition( vecTL, vecDim );
    }

    if ( UI_TestID( m_idScrollIndicatorTop ) )
    {
        UIWindow & tDimmer = UI_GetWindowFromID( m_idScrollIndicatorTop );
        Vector2D vecSize;
        tDimmer.GetTextureFrameSize( vecSize );
        Vector2D vecTL, vecDim;
        vecTL.x = m_flScrollIndicatorPaddingLeft;
        vecTL.y = m_flScrollIndicatorPaddingTop;
        vecDim.x = m_vecDim.x - vecTL.x - m_flScrollIndicatorPaddingRight;
        vecDim.y = vecSize.y;
        tDimmer.SetRelativePosition( vecTL, vecDim );
    }

    if ( UI_TestID( m_idScrollIndicatorBottom ) )
    {
        UIWindow & tDimmer = UI_GetWindowFromID( m_idScrollIndicatorBottom );
        Vector2D vecSize;
        tDimmer.GetTextureFrameSize( vecSize );
        Vector2D vecTL, vecDim;
        vecTL.x = m_flScrollIndicatorPaddingLeft;
        vecTL.y = m_vecDim.y - m_flScrollIndicatorPaddingBottom - vecSize.y + 1.0f;
        vecDim.x = m_vecDim.x - vecTL.x - m_flScrollIndicatorPaddingRight;
        vecDim.y = vecSize.y;
        tDimmer.SetRelativePosition( vecTL, vecDim );
    }
}
