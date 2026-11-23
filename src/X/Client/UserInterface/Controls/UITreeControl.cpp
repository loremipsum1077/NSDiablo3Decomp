// Original file: X/Client/UserInterface/Controls/UITreeControl.cpp
#include "UITreeControl.h"
#include "UICheckBoxControl.h"
#include "UIScrollbarControl.h"
#include "UIStackPanelControl.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "../UICreation.h"

// Declared here until their headers carry them (signatures from the DWARF).
UICheckBox & UI_GetCheckBoxFromID( const UIID & id );  // UI.cpp
UIStackPanel & UI_GetStackPanelFromID( const UIID & id );  // UI.cpp
UITreeFolder & UI_GetTreeFolderFromID( const UIID & id );  // UI.cpp
UIID UIIDCreateFromControlAttribute( const UIControlSourceKey & tSrcKey, const UIID & id, const UIAttributeType eAttributeType );









inline void TreeFolderCreationParams::sInit()
{
    flFolderIndentUIC = 0.0f;
    flFolderSpacingAfterCheckbox = 0.0f;
    tTemplateKey.snoUI = SNO_NONE;
    tTemplateKey.nKey = (uint64)-1;
}






void TreeFolderCreationParams::Load( const SNO snoUI, const UIControlDataExt * pControlData )
{
    ButtonCreationParams::Load( snoUI, pControlData );

    flFolderIndentUIC = (float)pControlData->GetInt32Value( ATT_TREE_FOLDER_INDENT );
    flFolderSpacingAfterCheckbox = (float)pControlData->GetInt32Value( ATT_TREE_FOLDER_SPACING );
}





TreeFolderCreationParams::TreeFolderCreationParams( const UIType eType, const UIID & id, const UIID & idParent )
    : ButtonCreationParams( eType, id, idParent )
{
    sInit();
}


















void UITreeFolder::OnCheckboxClick( const UIID & id )
{
    UICheckBox & tCheckbox = UI_GetCheckBoxFromID( id );

    UITreeFolder & tTreeFolder = UI_GetTreeFolderFromID( tCheckbox.GetParent() );
    BOOL fCollapsed = tCheckbox.IsChecked();

    UI_GetStackPanelFromID( tTreeFolder.GetStackPanel() ).SetActive( !fCollapsed );
}



inline void UITreeFolder::sInit( const TreeFolderCreationParams & tParams )
{
    m_idCheckbox = GetUIIDNone();
    m_idStackPanel = GetUIIDNone();
    m_flFolderIndentUIC = tParams.flFolderIndentUIC;
    m_flFolderSpacingAfterCheckbox = tParams.flFolderSpacingAfterCheckbox;
}


















UITreeFolder::UITreeFolder( const TreeFolderCreationParams & tParams, const Rect2D * pRect )
    : UIWindow( tParams, pRect )
{
    sInit( tParams );
}






UITreeFolder::~UITreeFolder()
{
}




void UITreeFolder::PostCreate()
{
    UIWindow::PostCreate();



    const UIID & id = GetID();
    m_idCheckbox = UIIDCreateFromParent( id, ".checkbox" );
    UICheckBox & tCheckbox = UI_GetCheckBoxFromID( m_idCheckbox );

    Vector2D vecTLCheckbox, vecDimCheckbox;
    tCheckbox.GetRelativePosition( vecTLCheckbox, vecDimCheckbox );
    StackPanelCreationParams tStackPanelParams( UIT_STACKPANEL, "stackpanel", id );

    m_idStackPanel = CreateControl<UIStackPanel, StackPanelCreationParams>( tStackPanelParams, Vector2D( m_flFolderIndentUIC, vecDimCheckbox.y + m_flFolderSpacingAfterCheckbox ), Vector2D( 0.0f, 0.0f ) );

    RecalculateLayout( -1 );
}


void UITreeFolder::RecalculateLayout( int32 )
{

    ResizeToFitChildren();
}












void UITreeFolder::Draw( const uint8 bLayerAlpha, float & flZSlice )
{


    if ( IsClipped() )
    {
        return;
    }


    UIWindow::Draw( bLayerAlpha, flZSlice );
}













void TreeCreationParams::Load( const SNO snoUI, const UIControlDataExt * pControlData )
{
    WindowCreationParams::Load( snoUI, pControlData );
}






TreeCreationParams::TreeCreationParams( const UIType eType, const UIID & id, const UIID & idParent )
    : WindowCreationParams( eType, id, idParent )
{
    sInit();
}



















void UITree::sInit( const TreeCreationParams & tParams )
{




    const UIID & id = GetID();
    const UIControlSourceKey & tSrcKey = GetUISrcKey();
    m_idScrollbar = UIIDCreateFromControlAttribute( tSrcKey, id, ATT_ID_SCROLLBAR );
    if ( m_idScrollbar != GetUIIDNone() )
    {
        UIScrollbar & tScrollbar = UI_GetScrollbarFromID( m_idScrollbar );
        tScrollbar.SetOwner( id );
        tScrollbar.SetOnChangeFunction( UIScrollbar::OnScrollChangeDefault );
    }

    m_idStackPanel = GetUIIDNone();
    {
        StackPanelCreationParams tParamsScoped( UIT_STACKPANEL, "stackpanel", id );
        m_idStackPanel = CreateControl<UIStackPanel, StackPanelCreationParams>( tParamsScoped, Vector2D( 0.0f, 0.0f ), Vector2D( 0.0f, 0.0f ) );
    }

    m_nFolder = 0;
}















UITree::UITree( const TreeCreationParams & tParams, const Rect2D * pRect )
    : UIWindow( tParams, pRect )
{
    sInit( tParams );
}






UITree::~UITree()
{
}



































void UITree::RecalculateLayout( int32 nStartingChildIndex )
{
    UIWindow::RecalculateLayout( nStartingChildIndex );
    AdjustScrollbar();
}





void UITree::AdjustScrollbar()
{

    if ( m_idScrollbar == GetUIIDNone() )
        return;
    if ( m_idStackPanel == GetUIIDNone() )
        return;

    float flExtents;

    Vector2D vecRegionTL, vecRegionDim;
    GetRelativePosition( vecRegionTL, vecRegionDim );
    Vector2D vecStackTL, vecStackDim;
    UI_GetStackPanelFromID( m_idStackPanel ).GetRelativePosition( vecStackTL, vecStackDim );

    UIScrollbar & tScrollbar = UI_GetScrollbarFromID( m_idScrollbar );
    flExtents = vecStackDim.y - vecRegionDim.y;
    if ( flExtents < 0.000001f )
    {
        tScrollbar.SetExtentsAndPosition( 0, 0, 0, TRUE );
    }
    else
    {
        tScrollbar.SetExtents( 0, (int32)flExtents, TRUE );
        tScrollbar.SetIncrements( 10, (int32)vecRegionDim.y );
    }
}
