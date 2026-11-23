// Original file: X/Client/UserInterface/Controls/UIListItemControl.cpp
#include "UIListItemControl.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../Sound/Sound.h"
#include "../UI.h"
#include "../UIAttributes.h"

// Declared here until their headers carry them (signatures from the DWARF).
void AnimStateInitialize( AnimState & tAnimState, const SNO snoTexture, const FrameAnim * ptFrameAnim );  // AnimationCommon.cpp:139
void Animation2DReset( AnimState & tAnimState, const SNO snoAnim2D );  // Animation2D.cpp:72




























inline void ListItemCreationParams::sInit()
{
    m_tTexUnselected = cgtImageTextureFrameNull;
    m_tTexSelected = cgtImageTextureFrameNull;
    m_tTexHover = cgtImageTextureFrameNull;
}





ListItemCreationParams::ListItemCreationParams( const UIType eType, const UIID & id, const UIID & idParent )
    : TextCreationParams( eType, id, idParent )
{
    sInit();
}



















void ListItemCreationParams::Load( const SNO snoUI, const UIControlDataExt * pControlData )
{
    TextCreationParams::Load( snoUI, pControlData );

    pControlData->GetImageAttribute( ATT_IMAGEFRAME, m_tTexUnselected );
    pControlData->GetImageAttribute( ATT_IMAGEFRAME_MOUSEOVER, m_tTexHover );
    pControlData->GetImageAttribute( ATT_IMAGEFRAME_PRESSED, m_tTexSelected );

    if ( m_tTexSelected == cgtImageTextureFrameNull )
    {
        m_tTexSelected = m_tTexUnselected;
    }
    if ( m_tTexHover == cgtImageTextureFrameNull )
    {
        m_tTexHover = m_tTexUnselected;
    }
}







void UIListItem::sInit( const ListItemCreationParams & tParams, const Vector2D &, const Vector2D & )
{
    m_fIsBasicItem = FALSE;
    SetSelectable( FALSE );

    m_idOwnerListBox = GetUIIDNone();
    m_nItemIndex = -1;
    m_pItemData = NULL;
    m_eSelectedState = UNSELECTED;
    m_eHoverState = NORMAL;
    m_snoAnimationSelectedState = SNO_NONE;
    m_snoAnimationHoveredState = SNO_NONE;
    m_pfnCustomOnSetDataCallback = NULL;
    m_pfnCustomSetItemText = NULL;
    m_pfnCustomGetItemText = NULL;
    m_pfnCustomEnterSelectedState = NULL;
    m_pfnCustomEnterHoverState = NULL;
    m_snoMouseoverSound = SNO_NONE;
    m_snoAnimationUnselectedState = SNO_NONE;
    AnimStateInitialize( m_tAnimStateUnselectedState, SNO_NONE, NULL );
    AnimStateInitialize( m_tAnimStateSelectedState, SNO_NONE, NULL );
    AnimStateInitialize( m_tAnimStateHoveredState, SNO_NONE, NULL );

    m_tTexUnselected = tParams.m_tTexUnselected;
    m_tTexSelected = tParams.m_tTexSelected;
    m_tTexHover = tParams.m_tTexHover;
}




















UIListItem::UIListItem( const ListItemCreationParams & tParams, const Rect2D * pRect )
    : UIText( tParams, pRect )
{
    Vector2D vecTL, vecDim;
    sRelativeCoordsFromCreationParams( vecTL, vecDim, pRect, tParams );
    sInit( tParams, vecTL, vecDim );
}







UIListItem::~UIListItem()
{

}

























































































































































































































































































inline void UIListItem::EnterHoverState( UIListItem::HoverState eNewState )
{
    if ( !IsEnabled() )
    {
        return;
    }

    if ( m_pfnCustomEnterHoverState != NULL )
    {
        m_pfnCustomEnterHoverState( *this, eNewState );
        if ( eNewState == HOVER && m_eHoverState != HOVER && m_snoMouseoverSound != SNO_NONE ) SoundManager::PlaySound( m_snoMouseoverSound, cgtPSODefaults );
        m_eHoverState = eNewState;
        return;
    }

    if ( eNewState == HOVER )
    {
        SetTextColor( crgbaWhite );
        if ( m_snoAnimationHoveredState != SNO_NONE )
        {
            SetBackgroundAnimation( m_snoAnimationHoveredState );
        }
        SetTextureFrame( m_tTexHover );
        if ( m_snoMouseoverSound != SNO_NONE )
        {
            SoundManager::PlaySound( m_snoMouseoverSound, cgtPSODefaults );
        }
    }
    else if ( IsSelected() )
    {
        SetTextColor( crgbaWhite );
        SetBackgroundAnimation( m_snoAnimationSelectedState );
        SetTextureFrame( m_tTexSelected );
    }
    else
    {
        SetTextColor( crgbaGold );
        SetBackgroundAnimation( m_snoAnimationUnselectedState );
        SetTextureFrame( m_tTexUnselected );
    }

    m_eHoverState = eNewState;
}

























void UIListItem::SetActive( const BOOL fActive )
{



    UIText::SetActive( fActive );
}
















void UIListItem::SetRelativePosition( const Vector2D & vecTL, const Vector2D & vecDim )
{



    UIText::SetRelativePosition( vecTL, vecDim );
}










BOOL UIListItem::HandleMessage( const UIMsg & msg, const uintptr dwParam )
{
    if ( msg == UI_RESET )
    {
        Animation2DReset( m_tAnimStateUnselectedState, m_snoAnimationUnselectedState );
        Animation2DReset( m_tAnimStateSelectedState, m_snoAnimationSelectedState );
        Animation2DReset( m_tAnimStateHoveredState, m_snoAnimationHoveredState );
    }
    return UIText::HandleMessage( msg, dwParam );
}



























































































void UIListItem::EnteredHitRegion()
{
    EnterHoverState( HOVER );
}




void UIListItem::LeftHitRegion()
{
    EnterHoverState( NORMAL );
}




void UIListItem::Draw( const uint8 bLayerAlpha, float & flZSlice )
{











    UIText::Draw( bLayerAlpha, flZSlice );
}
