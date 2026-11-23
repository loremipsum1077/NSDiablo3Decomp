#include "UIInit.h"

#include "../Definitions/UIDialogDefinition.h"
#include "Controls/UIControlControl.h"
#include "Controls/UIControlGroup.h"
#include "Controls/UIWindowControl.h"
#include "UI.h"
#include "UIAttributes.h"
#include "UIID.h"
#include "UIMasterIdList.h"
#include "UITypes.h"











void UIInit_CreateRoot()
{
	UIUIIDInit();

	WindowCreationParams tParams( UIT_WINDOW, cgUIIDRoot, GetUIIDNone() );
	tParams.eHorizontalAnchor = UI_ANCHOR_STRETCH_LEFT_AND_RIGHT;
	tParams.eVerticalAnchor = UI_VERT_ANCHOR_CENTER;
	UIWindow * ptDialog = new UIWindow( tParams, Vector2D( 0.0f, 0.0f ), Vector2D( 0.0f, 0.0f ) );

	WindowCreationParams tNormalLayerParams( UIT_WINDOW, cgUIIDNormalLayer, cgUIIDRoot );
	tNormalLayerParams.eHorizontalAnchor = UI_ANCHOR_STRETCH_LEFT_AND_RIGHT;
	tNormalLayerParams.eVerticalAnchor = UI_VERT_ANCHOR_CENTER;
	UIWindow * ptNormalLayerDialog = new UIWindow( tNormalLayerParams, Vector2D( 0.0f, 0.0f ), Vector2D( 0.0f, 0.0f ) );
	ptDialog->AddChild( cgUIIDNormalLayer );

	WindowCreationParams tWindowLayerParams( UIT_WINDOW, cgUIIDWindowLayer, cgUIIDRoot );
	tWindowLayerParams.eHorizontalAnchor = UI_ANCHOR_STRETCH_LEFT_AND_RIGHT;
	tWindowLayerParams.eVerticalAnchor = UI_VERT_ANCHOR_CENTER;
	UIWindow * ptWindowLayerDialog = new UIWindow( tWindowLayerParams, Vector2D( 0.0f, 0.0f ), Vector2D( 0.0f, 0.0f ) );
	ptDialog->AddChild( cgUIIDWindowLayer );

	WindowCreationParams tTopLayerParams( UIT_WINDOW, cgUIIDTopLayer, cgUIIDRoot );
	tTopLayerParams.eHorizontalAnchor = UI_ANCHOR_STRETCH_LEFT_AND_RIGHT;
	tTopLayerParams.eVerticalAnchor = UI_VERT_ANCHOR_CENTER;
	UIWindow * ptTopLayerDialog = new UIWindow( tTopLayerParams, Vector2D( 0.0f, 0.0f ), Vector2D( 0.0f, 0.0f ) );
	ptDialog->AddChild( cgUIIDTopLayer );








}




static UIControl * sCreateControlFromData( const UIControlDataExt * pControlData, const DynamicControlCreationParams & tCreationParams, const SNO snoUI, const BOOL fInstantiatingTemplate )
{
	UIID idControl;



	idControl.SetFromParent( tCreationParams.m_idDlgParent, tCreationParams.m_szNewControlName );



	UIControl * ptControl = UIControl::CreateFromXMLData( snoUI, pControlData, idControl, tCreationParams.m_idDlgParent, fInstantiatingTemplate );
	if ( ptControl )
	{
		UIControlGroup & tControlGroupParent = UI_GetControlGroupFromID( tCreationParams.m_idDlgParent );
		tControlGroupParent.AddChild( idControl );
	}

	return ptControl;
}






UIControl * sCreateChildControls( UIControlData * pControlData, const DynamicControlCreationParams & tCreationParams, const SNO snoUI, const BOOL fInstantiatingTemplate )
{



	UIControlDataExt * pControlDataExt = new UIControlDataExt( *pControlData );
	UIControlDataExt * pCurControlData = pControlDataExt;


	while ( pCurControlData )
	{

		if ( pCurControlData->m_control.AttributeExists( ATT_XML_TEMPLATE ) )
		{
			LPCSTR szTemplate = pCurControlData->m_control.GetAttributeValue( ATT_XML_TEMPLATE );

			if ( szTemplate[ 0 ] != '\0' )
			{
				BOOL fSuccess;
				UIControlSourceKey srcKey = UIControlSourceKey::Parse( szTemplate, snoUI, NULL, NULL );
				if ( srcKey.snoUI != SNO_NONE )
				{
					ScopedUIDialogDefinitionLockR pdefUI( srcKey.snoUI );
					UIControlData * pTemplate;

					fSuccess = UI_GetControlData( srcKey, pdefUI, pTemplate );
					if ( fSuccess )
					{
						pCurControlData->SetTemplate( pTemplate, srcKey.snoUI );
					}
				}
			}
		}












		pCurControlData = pCurControlData->GetTemplate();
	}

	UIControl * ptControl = sCreateControlFromData( pControlDataExt, tCreationParams, snoUI, fInstantiatingTemplate );
	if ( ptControl )
	{

		if ( pControlDataExt->GetTemplate() )
		{
			UIControlDataArray children;
			pControlDataExt->CloneTemplateChildren( &children );




			UI_SetupStyles( *pControlData, &children, pControlDataExt->GetTemplateSno(), NULL );

			for ( uint32 i = 0; i < children.size(); i++ )
			{
				UIControlData * pChild = children[ i ];
				sCreateChildControls( pChild, DynamicControlCreationParams( pChild->szName, ptControl->GetID() ), pControlDataExt->GetTemplateSno(), FALSE );
				UIControlData::Delete( children[ i ], FALSE );
			}
		}


		for ( uint32 i = 0; i < pControlData->plistChildren->size(); i++ )
		{
			UIControlData * pChild = ( *pControlData->plistChildren )[ i ];

			if ( !pChild->AttributeExists( ATT_PC_ONLY ) || !pChild->GetBoolValue( ATT_PC_ONLY ) )
			{



				sCreateChildControls( pChild, DynamicControlCreationParams( pChild->szName, ptControl->GetID() ), snoUI, FALSE );
			}
		}



		if ( ptControl->IsType( UIT_CONTROLGROUP ) )
		{
			UIControlGroup * ptControlGroupParent = (UIControlGroup *)ptControl;
			ptControlGroupParent->OnFinishAddingXmlChildren();
		}
	}

	::delete pControlDataExt;

	return ptControl;
}





inline BOOL sControlDoesDynamicLayout( const UIID & id )
{

	UIControl & tControl = UI_GetControlFromID( id );
	return tControl.IsType( UIT_STACKPANEL ) || tControl.IsType( UIT_WRAPPANEL );
}





UIControl * UI_CreateControlFromTemplate( const UIControlSourceKey & tSrcKey, const DynamicControlCreationParams & tCreationParams, const Vector2D * pvecTLNewControl )
{



	if ( tSrcKey.snoUI == SNO_NONE )
		return NULL;

	ScopedUIDialogDefinitionLockR pdefUI( tSrcKey.snoUI );
	UIControlData * pControlData;
	BOOL fSuccess;
	UIControl * ptControl;

	fSuccess = UI_GetControlData( tSrcKey, pdefUI, pControlData );
	if ( !fSuccess )
		return NULL;




	ptControl = sCreateChildControls( pControlData, tCreationParams, tSrcKey.snoUI, TRUE );


	if ( ptControl->IsType( UIT_WINDOW ) )
	{
		UIWindow * ptWindow = (UIWindow *)ptControl;


		ptWindow->PostCreate();


		if ( !sControlDoesDynamicLayout( tCreationParams.m_idDlgParent ) && pvecTLNewControl )
		{
			Vector2D vecTLIgnored, vecDim;
			ptWindow->GetRelativePosition( vecTLIgnored, vecDim );
			ptWindow->SetRelativePosition( *pvecTLNewControl, vecDim );
		}
	}




	return ptControl;
}




























UIWindow * UI_CreateDlg( const UIID & idDlgParent, const SNO snoUI )
{
	UIWindow * ptDialogToReturn = NULL;

	if ( snoUI != SNO_NONE )
	{







		UIControl * ptControl;
		ScopedUIDialogDefinitionLockR pdefUI( snoUI );

		ptControl = sCreateChildControls( pdefUI->pControlDataRoot, DynamicControlCreationParams( pdefUI->pControlDataRoot->szName, idDlgParent ), snoUI, FALSE );
		if ( ptControl )
		{






			ptControl->PostCreate();
			ptControl->UpdateAbsolutePositions();
			ptDialogToReturn = (UIWindow *)ptControl;
		}
	}

	return ptDialogToReturn;
}
