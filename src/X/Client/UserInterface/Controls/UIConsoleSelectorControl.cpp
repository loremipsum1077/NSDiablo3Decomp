// Original file: X/Client/UserInterface/Controls/UIConsoleSelectorControl.cpp
#include "UIConsoleSelectorControl.h"
#include "../../Console/ControllerUtils.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../Input/Input.h"
#include "../../Sound/Sound.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "../../../Sigma/Math/MathGeneral.h"

const UIID & UI_GetControllerFocus();  // UI.cpp:429


















ConsoleSelectorCreationParams::ConsoleSelectorCreationParams( const UIType eType, const UIID & id, const UIID & idParent )
    : WindowCreationParams( eType, id, idParent )
{
    sInit();
}













void ConsoleSelectorCreationParams::Load( const SNO snoUI, const UIControlDataExt * pControlData )
{
    WindowCreationParams::Load( snoUI, pControlData );

    pfnOnSelChange = UIFunction_Lookup( pControlData->GetAttributeValue( ATT_ONSELCHANGE ) );
    bRequiresControllerFocus = pControlData->GetBoolValue( ATT_REQUIRES_CONTROLLER_FOCUS );
    bScrollWithRightStick = pControlData->GetBoolValue( ATT_SCROLL_WITH_RSTICK );
    nStickSpeedLimitMS = pControlData->GetInt32Value( ATT_STICK_SPEED_LIMIT );
    snoClickSound = SNOFileLookupGUID( SOUND_SNO, pControlData->GetAttributeValue( ATT_CLICK_SOUND ) );
}



inline void ConsoleSelectorCreationParams::sInit()
{
    pfnOnSelChange = NULL;
    bRequiresControllerFocus = TRUE;
    bScrollWithRightStick = FALSE;
    bWrap = FALSE;
    nStickSpeedLimitMS = 200;
    snoClickSound = SNO_NONE;
}































UIConsoleSelector::UIConsoleSelector( const ConsoleSelectorCreationParams & tParams, const Rect2D * pRect )
    : UIWindow( tParams, pRect )
    , m_bRequiresControllerFocus( FALSE )
    , m_bScrollWithRightStick( FALSE )
    , m_bWrap( FALSE )
    , m_nInputSpeedLimitMS( 200 )
    , m_nSelectedIndex( 0 )
    , m_snoClickSound( SNO_NONE )
    , m_idArrowLeft( GetUIIDNone() )
    , m_idArrowRight( GetUIIDNone() )
    , m_pfnOnSelChange( NULL )
    , m_ptDataSource( NULL )
    , m_idVisibleItem( GetUIIDNone() )
{
    Vector2D vecTL;
    Vector2D vecDim;
    sRelativeCoordsFromCreationParams( vecTL, vecDim, pRect, tParams );
    sInit( tParams, vecTL, vecDim );
}






UIConsoleSelector::~UIConsoleSelector()
{
    m_arSelectorItems.clear();
}





void UIConsoleSelector::sInit( const ConsoleSelectorCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim )
{
    m_nSelectedIndex = -1;
    m_ptDataSource = NULL;
    m_bScrollWithRightStick = tParams.bScrollWithRightStick;
    const UIID & id = GetID();
    m_bRequiresControllerFocus = tParams.bRequiresControllerFocus;
    m_pfnOnSelChange = tParams.pfnOnSelChange;
    m_bWrap = tParams.bWrap;
    m_snoClickSound = tParams.snoClickSound;
    m_nInputSpeedLimitMS = tParams.nStickSpeedLimitMS;








    m_tStickInputFilter = Console::GetDefaultStickInputFilter( Console::FILTER_HORIZONTAL );
    m_tStickInputFilter.SetMinimumInputDelay( m_nInputSpeedLimitMS );
    {
        WindowCreationParams tWindowParams( UIT_WINDOW, "left", id );
        tWindowParams.eRenderLayerOverride = tParams.eRenderLayerOverride;
        tWindowParams.aRot = PI / 2.0f;




        UIWindow & tWindow = *new UIWindow( tWindowParams, Vector2D( 0.0f, 0.0f ), Vector2D( vecDim.y, vecDim.y ) );
        m_idArrowLeft = tWindow.GetID();
        tWindow.SetTextureFrame( "UI:SkillsPane_Arrow" );
    }
    {
        WindowCreationParams tWindowParams( UIT_WINDOW, "right", id );
        tWindowParams.eRenderLayerOverride = tParams.eRenderLayerOverride;
        tWindowParams.aRot = PI * 1.5f;


        UIWindow & tWindow = *new UIWindow( tWindowParams, Vector2D( vecDim.x - vecDim.y, 0.0f ), Vector2D( vecDim.y, vecDim.y ) );
        m_idArrowRight = tWindow.GetID();
        tWindow.SetTextureFrame( "UI:SkillsPane_Arrow" );
    }
}






BOOL UIConsoleSelector::HandleMessage( const UIMsg & msg, const uintptr dwParam )
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
    case UI_REGISTER_INPUT_LISTENER:
        UI_RegisterInputListener( this );
        break;
    }
    return UIWindow::HandleMessage( msg, dwParam );
}













inline BOOL UIConsoleSelector::HandleInput( const BufferedInput & tInput )
{
    switch ( tInput.eInputMsg )
    {
    case INPUTMSG_LEFTSTICK:
        if ( !m_bScrollWithRightStick )
            return HandleStickInput( tInput );
        break;
    case INPUTMSG_RIGHTSTICK:
        if ( m_bScrollWithRightStick )
            return HandleStickInput( tInput );
        break;
    case INPUTMSG_CONTROLLERBUTTONDOWN:
    case INPUTMSG_CONTROLLERBUTTONSTILLDOWN:
    case INPUTMSG_CONTROLLERBUTTONREPEAT:
        if ( tInput.tBufControllerData.eInputButton == 10 || tInput.tBufControllerData.eInputButton == 11 )
            return HandleDPadInput( tInput );
        break;
    }
    return FALSE;
}




BOOL UIConsoleSelector::HandleStickInput( const BufferedInput & tInput )
{
    BOOL bHasControllerFocus = UI_GetControllerFocus() == GetID();
    BOOL bRightStick = tInput.eInputMsg == INPUTMSG_RIGHTSTICK && m_bScrollWithRightStick;
    Vector2D vecStick = bRightStick ? Console::ControllerUtils::GetRStickPosition( tInput.tBufControllerData.nControllerIndex ) : Console::ControllerUtils::GetLStickPosition( tInput.tBufControllerData.nControllerIndex, FALSE );
    BOOL bInDeadZone = bRightStick ? Console::ControllerUtils::RStickIsInDeadZone( tInput.tBufControllerData.nControllerIndex ) : Console::ControllerUtils::LStickIsInDeadZone( tInput.tBufControllerData.nControllerIndex, FALSE );
    if ( !bInDeadZone && ( bHasControllerFocus || !m_bRequiresControllerFocus ) )
    {
        Console::AnalogStickInputFilter::Rule::ID idRule = m_tStickInputFilter.HandleInput( vecStick, FALSE );
        switch ( idRule )
        {
        case Console::RULE_STICK_LEFT:
            SelectPreviousItem( TRUE );
            return TRUE;
        case Console::RULE_STICK_RIGHT:
            SelectNextItem( TRUE );
            return TRUE;
        }
    }
    return FALSE;
}












BOOL UIConsoleSelector::HandleDPadInput( const BufferedInput & tInput )
{
    static Console::InputSpeedLimiter tSpeedLimiter;
    static InputButton eLastButton = -1;
    BOOL bHasControllerFocus;
    BOOL bIgnoreInput = !tSpeedLimiter.HasWaitTimeExpired( m_nInputSpeedLimitMS );


    bHasControllerFocus = UI_GetControllerFocus() == GetID();
    if ( bHasControllerFocus || !m_bRequiresControllerFocus )
    {
        BOOL bButtonChanged = eLastButton != tInput.tBufControllerData.eInputButton;
        eLastButton = tInput.tBufControllerData.eInputButton;
        if ( !bIgnoreInput || bButtonChanged )
        {
            switch ( tInput.tBufControllerData.eInputButton )
            {
            case 10:
                SelectPreviousItem( TRUE );
                break;
            case 11:
                SelectNextItem( TRUE );
                break;
            }
            tSpeedLimiter.UpdateLastInputTime();
            return TRUE;
        }
    }
    return bHasControllerFocus;
}















int32 UIConsoleSelector::GetNumItems() const
{
    if ( m_ptDataSource != NULL )
    {
        return m_ptDataSource->GetNumItems();
    }
    return 0;
}

void UIConsoleSelector::SetSelectedIndex( const int32 nIndex, const BOOL fRun )
{
    if ( nIndex < 0 || nIndex >= GetNumItems() )
    {
        return;
    }
    if ( m_ptDataSource == NULL )
    {
        return;
    }



    m_nSelectedIndex = nIndex;






    if ( fRun )
    {
        if ( m_snoClickSound != SNO_NONE )
        {
            SoundManager::PlaySound( m_snoClickSound, cgtPSODefaults );
        }
        if ( m_pfnOnSelChange != NULL )
        {
            m_pfnOnSelChange( GetID() );
        }
    }
    ShowItem( nIndex );
    UpdateArrows();
}


void UIConsoleSelector::ShowItem( int32 nItemIndex )
{
    if ( nItemIndex < 0 || nItemIndex >= GetNumItems() )
    {
        return;
    }
    if ( m_ptDataSource == NULL )
    {
        return;
    }

    UIControlSourceKey tItemTemplate = m_ptDataSource->GetSourceKeyForItem( nItemIndex );








    UIID idNewItem( GetUIIDNone() );
    for ( uint32 nIndex = 0; nIndex < m_arSelectorItems.size(); nIndex++ )
    {
        SelectorItem & tItem = m_arSelectorItems[nIndex];
        if ( tItem.nKey == tItemTemplate.nKey )
        {

            idNewItem = tItem.idItem;
            break;
        }
    }

    if ( idNewItem == GetUIIDNone() )
    {
        CRefString sName;
        sName.Format( "item %d list", nItemIndex );




        UIWindow * ptWindow = (UIWindow *)UI_CreateControlFromTemplate( tItemTemplate, DynamicControlCreationParams( sName,
            GetID() ), NULL );


        idNewItem = ptWindow->GetID();

        if ( idNewItem == GetUIIDNone() || !UI_TestID( idNewItem ) )
        {
            return;
        }



        SelectorItem tItem;
        tItem.nKey = tItemTemplate.nKey;
        tItem.idItem = ptWindow->GetID();
        m_arSelectorItems.push_back( tItem );
        m_ptDataSource->InitializeItemUI( *this, tItem.idItem, nItemIndex );
    }


    if ( m_idVisibleItem != GetUIIDNone() && UI_TestID( m_idVisibleItem ) )
    {
        UI_GetWindowFromID( m_idVisibleItem ).SetActive( FALSE );
    }


    m_idVisibleItem = idNewItem;
    UI_GetWindowFromID( m_idVisibleItem ).SetActive( TRUE );
    m_nSelectedIndex = nItemIndex;
    m_ptDataSource->UpdateItemUI( *this, m_idVisibleItem, nItemIndex );
}








void UIConsoleSelector::SelectNextItem( const BOOL fRun )
{
    int32 nNumItems = GetNumItems();
    if ( nNumItems <= 0 )
    {
        return;
    }



    int32 nSelectedIndex = GetSelectedIndex() + 1;
    if ( m_bWrap )
    {
        nSelectedIndex = nSelectedIndex % nNumItems;
    }
    else
    {
        nSelectedIndex = Math::Pin( nSelectedIndex, 0, nNumItems - 1 );
    }
    if ( nSelectedIndex != GetSelectedIndex() )
    {
        SetSelectedIndex( nSelectedIndex, fRun );
    }
}



void UIConsoleSelector::SelectPreviousItem( const BOOL fRun )
{
    int32 nNumItems = GetNumItems();
    if ( nNumItems <= 0 )
    {
        return;
    }



    int32 nSelectedIndex = GetSelectedIndex() - 1;
    if ( m_bWrap )
    {
        nSelectedIndex = ( nSelectedIndex + nNumItems ) % nNumItems;
    }
    else
    {
        nSelectedIndex = Math::Pin( nSelectedIndex, 0, nNumItems - 1 );
    }
    if ( nSelectedIndex != GetSelectedIndex() )
    {
        SetSelectedIndex( nSelectedIndex, fRun );
    }
}



void UIConsoleSelector::SetDataSource( ConsoleSelectorDataSource & tDataSource )
{
    if ( m_ptDataSource == &tDataSource )
    {
        return;
    }
    m_ptDataSource = &tDataSource;



    ResetAllItems();
}




inline void UIConsoleSelector::ResetAllItems()
{
    m_nSelectedIndex = -1;
    if ( m_ptDataSource->GetNumItems() > 0 )
    {
        m_nSelectedIndex = 0;
        ShowItem( 0 );
    }
}






















void UIConsoleSelector::LostControllerFocus()
{
    if ( UI_TestID( m_idVisibleItem ) )
    {
        m_ptDataSource->UpdateItemUI( *this, m_idVisibleItem, m_nSelectedIndex );
    }
}






void UIConsoleSelector::GainedControllerFocus()
{
    if ( UI_TestID( m_idVisibleItem ) )
    {
        m_ptDataSource->UpdateItemUI( *this, m_idVisibleItem, m_nSelectedIndex );
    }
}







void UIConsoleSelector::SetWrapping( BOOL bWrap )
{
    m_bWrap = bWrap;
    UpdateArrows();
}





void UIConsoleSelector::UpdateArrows()
{
    if ( m_bWrap )
    {
        BOOL bShowArrows = GetNumItems() > 1;
        UI_GetWindowFromID( m_idArrowLeft ).SetActive( bShowArrows );
        UI_GetWindowFromID( m_idArrowRight ).SetActive( bShowArrows );
    }
    else
    {

        UI_GetWindowFromID( m_idArrowLeft ).SetActive( GetSelectedIndex() > 0 );
        UI_GetWindowFromID( m_idArrowRight ).SetActive( GetSelectedIndex() < GetNumItems() - 1 );
    }
}
