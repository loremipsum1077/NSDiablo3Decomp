#include "Contrib/Contrib/blz/include/blz/float.h"
#include "X/Client/UserInterface/Controls/UIStackPanelControl.h"

#include "../../../Sigma/Math/MathGeneral.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../UI.h"
#include "../UIAttributes.h"

// Declarations of the functions this file calls whose headers are not adopted yet;
// the DWARF gives their signatures but not where they were declared.
// X/Client/UserInterface/UI.cpp
void UI_DeleteControl(const UIID & idControl, const BOOL fAssertIfDeletingRoot);
// X/Client/UserInterface/UIUtils.cpp
void UIParseCsvElement(float & dest, LPCSTR & szCsv);





inline void StackPanelCreationParams::sInit()
{
    fStackHorizontally = FALSE;
    fReverseOrder = FALSE;
    flSpacing = 0.0f;
    flMarginTop = 0.0f;
    flMarginRight = 0.0f;
    flMarginBottom = 0.0f;
    flMarginLeft = 0.0f;
}





void StackPanelCreationParams::Load(
    const SNO snoUI,
    const UIControlDataExt * pControlData )
{
    WindowCreationParams::Load( snoUI, pControlData );

    fStackHorizontally = pControlData->GetBoolValue( ATT_HORIZONTAL );
    fReverseOrder = pControlData->GetBoolValue( ATT_REVERSE_ORDER );
    flSpacing = (float)pControlData->GetInt32Value( ATT_STACK_SPACING );

    LPCSTR szMargins = pControlData->GetAttributeValue( ATT_MARGINS_TRBL );
    UIParseCsvElement( flMarginTop, szMargins );
    UIParseCsvElement( flMarginRight, szMargins );
    UIParseCsvElement( flMarginBottom, szMargins );
    UIParseCsvElement( flMarginLeft, szMargins );
}



StackPanelCreationParams::StackPanelCreationParams(
    const UIType eType,
    const UIID & id,
    const UIID & idParent )
    : WindowCreationParams( eType, id, idParent )
{
    sInit();
}




StackPanelCreationParams::StackPanelCreationParams(
    const UIType eType,
    LPCSTR szName,
    const UIID & idParent )
    : WindowCreationParams( eType, szName, idParent )
{
    sInit();
}




inline void UIStackPanel::sInit( const StackPanelCreationParams & tParams )
{

    m_flLargestWidth = 0.0f;
    m_fStackHorizontally = tParams.fStackHorizontally;
    m_fReverseOrder = tParams.fReverseOrder;
    m_flSpacing = tParams.flSpacing;
    m_flMarginTop = tParams.flMarginTop;
    m_flMarginRight = tParams.flMarginRight;
    m_flMarginBottom = tParams.flMarginBottom;
    m_flMarginLeft = tParams.flMarginLeft;

    m_vecInitialSize = m_vecDim;
}




UIStackPanel::UIStackPanel(
    const StackPanelCreationParams & tParams,
    const Vector2D & vecTL,
    const Vector2D & vecDim )
    : UIWindow( tParams, vecTL, vecDim )
{
    sInit( tParams );
}




UIStackPanel::UIStackPanel(
    const StackPanelCreationParams & tParams,
    const Rect2D * pRect )
    : UIWindow( tParams, pRect )
{
    sInit( tParams );
}




UIStackPanel::~UIStackPanel()
{
}




inline void UIStackPanel::sRelayoutChild(
    UIWindow * ptWindow,
    Vector2D & vecOffset)
{

    Vector2D vecTL, vecDim;
    ptWindow->GetRelativePosition(vecTL, vecDim);

    Vector2D vecNewLoc = vecOffset;
    if (m_fStackHorizontally)
    {
        if (ptWindow->IsCenteredInStack())
        {


            vecNewLoc.y = (m_flLargestWidth + m_flMarginTop + m_flMarginBottom - vecDim.y) * 0.5f;
        }
        else
        {
            vecNewLoc.y = vecTL.y;
        }
    }
    else
    {
        if (ptWindow->IsCenteredInStack())
        {


            vecNewLoc.x = (m_flLargestWidth + m_flMarginLeft + m_flMarginRight - vecDim.x) * 0.5f;
        }
        else
        {
            vecNewLoc.x = vecTL.x;
        }
    }


    ptWindow->Move(vecNewLoc - vecTL);
    // NOTE: flChildSize is not in the DWARF (which has only vecTL/vecDim 131, vecNewLoc 134; row 165:9 is an if-condition);
    const float flChildSize = m_fStackHorizontally ? vecDim.x : vecDim.y; if (m_fStackHorizontally == FALSE) vecOffset.y += m_flSpacing + flChildSize; else vecOffset.x += m_flSpacing + flChildSize;
    // the plain if/else without it gives 7/223 and 8/215 instead of 3/223 and 4/215 (y01, Nov 22), so the old form stays.






}




inline int32 UIStackPanel::FindPrevWindowChild(
    int32 nStartingIndex)
{
    int32 nStart = nStartingIndex - 1;
    int32 nEnd = -1;
    int8 nIncrement = -1;

    if (m_fReverseOrder)
    {
        nEnd = GetChildList().size();
        nIncrement = 1;
        nStart = nStartingIndex + 1;
    }
    // DWARF: loop variable i (192, own block) with tControl (194) inside; columns 192:30, 194:31, 195:13/22/41/44
    for (int32 i = nStart; i != nEnd; i += nIncrement)
    {
        UIControl& tControl = UI_GetControlFromID(GetChildList()[i].id);
        if (tControl.IsType(UIT_WINDOW) && tControl.IsItselfActive())
        {
            return i;
        }
    }

    return -1;
}




inline float UIStackPanel::GetLargestWidth( BOOL & hasCenteredChild ) const
{

    const uint32 nNumChildren = GetChildList().size();
    float flLargestWidth = 0.0f;

    for ( uint32 i = 0; i < nNumChildren; ++i )
    {
        UIControl & tControl = UI_GetControlFromID( GetChildList()[i].id );
        if ( tControl.IsType( UIT_WINDOW ) && tControl.IsItselfActive() )
        {
            UIWindow & tWindow = static_cast<UIWindow &>( tControl );
            Vector2D vecTL, vecDim;
            tWindow.GetRelativePosition( vecTL, vecDim );
            const float flWidth = m_fStackHorizontally ? vecDim.y : vecDim.x;





            flLargestWidth = Math::Max( flLargestWidth, flWidth );

            if ( tWindow.IsCenteredInStack() )
            {
                hasCenteredChild = TRUE;
            }
        }
    }

    return flLargestWidth;
}



inline void UIStackPanel::GetStartingLayout( int32 nStartingChildIndex, Vector2D & vecOffset )
{


    const int32 nNumChildren = GetChildList().size();



    if ( nStartingChildIndex == nNumChildren )
    {
        if ( nStartingChildIndex < 0 || m_fReverseOrder )
        {
            return;
        }
    }
    else
    {

        if ( nStartingChildIndex < 0 )
        {
            return;
        }
    }

    const int32 nPrevChild = FindPrevWindowChild( nStartingChildIndex );
    if ( nPrevChild >= 0 )
    {
        Vector2D vecTL, vecDim;
        UI_GetWindowFromID( GetChildList()[nPrevChild].id ).GetRelativePosition( vecTL, vecDim );
        if ( m_fStackHorizontally )
        {
            vecOffset.x = vecDim.x + vecTL.x + m_flSpacing;
        }
        else
        {
            vecOffset.y = vecDim.y + vecTL.y + m_flSpacing;
        }
    }
}




void UIStackPanel::RecalculateLayout(
    int32 nStartingChildIndex )
{
    if ( !UI_TestID( GetID() ) ||
         !UI_TestID( GetParent() ) ||
         nStartingChildIndex < -1 ||
         nStartingChildIndex > (int32)GetChildList().size() )
    {
        return;
    }
















    BOOL hasCenteredChild = FALSE;

    const float flLargestWidth = GetLargestWidth( hasCenteredChild );

    if ( flLargestWidth != m_flLargestWidth )
    {
        if ( hasCenteredChild )
        {
            nStartingChildIndex = -1;
        }


        m_flLargestWidth = flLargestWidth;
    }

    if ( m_fReverseOrder )
    {
        RecalculateReverseLayout( nStartingChildIndex );
    }
    else
    {
        RecalculateForwardLayout( nStartingChildIndex );
    }
}




void UIStackPanel::DeleteAllChildren(
    void )
{
    const UIControlItemArray & arStackChildren = GetChildList();
    while ( arStackChildren.size() != 0 )
    {

        const UIID & idChild = arStackChildren[arStackChildren.size() - 1].id;
        if ( UI_TestID( idChild ) )
        {
            UI_DeleteControl( idChild, TRUE );
        }
    }
}



// NON_MATCHING: register allocation only; the csel/ldr/str of the child-size pointer use x8, the original x9.
void UIStackPanel::RecalculateForwardLayout(
    int32 nStartingChildIndex )
{

    const int32 nNumChildren = GetChildList().size();
    Vector2D vecOffset( m_flMarginLeft, m_flMarginTop );

    GetStartingLayout( nStartingChildIndex, vecOffset );

    const int32 nIndex = nStartingChildIndex == -1 ? 0 : nStartingChildIndex;

    Vector2D vecEncapsulatingDim( 0.0f, 0.0f );
    for ( int32 i = 0; i < nNumChildren; ++i )
    {
        UIControl & tControl = UI_GetControlFromID( GetChildList()[i].id );
        if ( tControl.IsType( UIT_WINDOW ) && tControl.IsItselfActive() )
        {
            UIWindow * ptWindow = static_cast<UIWindow *>( &tControl );

            if ( i >= nIndex )
            {
                sRelayoutChild( ptWindow, vecOffset );
            }

            Vector2D vecChildOffset, vecChildDim;
            ptWindow->GetRelativePosition( vecChildOffset, vecChildDim );

            vecEncapsulatingDim.x = Math::Max( vecEncapsulatingDim.x, vecChildOffset.x + vecChildDim.x );
            vecEncapsulatingDim.y = Math::Max( vecEncapsulatingDim.y, vecChildOffset.y + vecChildDim.y );
        }
    }

    vecEncapsulatingDim.x += m_flMarginRight;
    vecEncapsulatingDim.y += m_flMarginBottom;

    SetRelativePosition( m_vecTL, vecEncapsulatingDim );
}



// NON_MATCHING: register allocation only; the csel/ldr/str of the child-size pointer use x8, the original x9.
void UIStackPanel::RecalculateReverseLayout(
    int32 nStartingChildIndex )
{

    const int32 nNumChildren = GetChildList().size();
    Vector2D vecOffset( m_flMarginLeft, m_flMarginTop );

    GetStartingLayout( nStartingChildIndex, vecOffset );

    int32 nIndex = nStartingChildIndex;
    if ( nStartingChildIndex == -1 || nStartingChildIndex >= nNumChildren )
    {
        nIndex = nNumChildren - 1;
    }

    Vector2D vecEncapsulatingDim( 0.0f, 0.0f );
    for ( int32 i = nNumChildren - 1; i >= 0; --i )
    {
        UIControl & tControl = UI_GetControlFromID( GetChildList()[i].id );
        if ( tControl.IsType( UIT_WINDOW ) && tControl.IsItselfActive() )
        {
            UIWindow * ptWindow = static_cast<UIWindow *>( &tControl );

            if ( i <= nIndex )
            {
                sRelayoutChild( ptWindow, vecOffset );
            }

            Vector2D vecChildOffset, vecChildDim;
            ptWindow->GetRelativePosition( vecChildOffset, vecChildDim );

            vecEncapsulatingDim.x = Math::Max( vecEncapsulatingDim.x, vecChildOffset.x + vecChildDim.x );
            vecEncapsulatingDim.y = Math::Max( vecEncapsulatingDim.y, vecChildOffset.y + vecChildDim.y );
        }
    }

    vecEncapsulatingDim.x += m_flMarginRight;
    vecEncapsulatingDim.y += m_flMarginBottom;

    SetRelativePosition( m_vecTL, vecEncapsulatingDim );
}




float UIStackPanel::GetSpacing() const {
    return m_flSpacing;
}
