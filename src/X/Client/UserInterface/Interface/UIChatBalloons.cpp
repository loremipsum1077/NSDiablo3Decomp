// Original file: X/Client/UserInterface/Interface/UIChatBalloons.cpp
#include "UIGame.h"
#include "../UI.h"
#include "../UIInit.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIFloatBubbleControl.h"
#include "../Controls/UIWindowControl.h"
#include "../../CGame.h"
#include "../../RActors/RActors.h"
#include "../../RActors/RActorAttachments.h"
#include "../../Definitions/CAttachmentDefinition.h"
#include "../../../Common/Definitions/AttachmentDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Sigma/Math/Rect.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RefString.h"

// Declared here until their headers carry them.
extern UIID UIDLG_ITEMTAGS;  // X/Client/UserInterface/UIGlobals.cpp
void UI_DeleteControl( UIID & idControl, const BOOL fAssertIfDeletingRoot );  // X/Client/UserInterface/UI.cpp:2866
UIFloatBubble & UI_GetFloatBubbleFromID( const UIID & id );  // X/Client/UserInterface/UI.cpp:2598
BOOL RActorCanHaveChatBubble( const RActor * ptRActor );  // X/Client/RActors/RActors.cpp:12266
BOOL WorldPointToScreenPointUIC( const Vector3D & wp, Vector2D & wpUIC, const int32 nCameraIndex );  // X/Client/Camera/CameraManager.cpp:531

enum { MAX_CHAT_BALLOONS = 8 };








struct UIFloat
{
    RActorID idRActor;
    SNO snoStringList;
    StringLabelHandle hLabel;
    uint32 dwExpiration;
    BOOL bVisible;
    float flHeightOffset;

    UIID idFloatBubble;
    int32 controllerIndex;
};

struct UIChatBalloonGlobals
{
    UIFloat arUIFloats[MAX_CHAT_BALLOONS];
};





UIChatBalloonGlobals * sgptUIChatBalloonGlobals;

static void sAddRActorBalloon( UIFloat & tFloat, RActorID idRActor, SNO snoStringList, uint32 hLabel, uint32 dwTicksToDisplay, float flHeightOffset, int32 nControllerIndex );  // defined at line 289












namespace UIChatBalloons {

void InitializeForGame()

{
    sgptUIChatBalloonGlobals = (UIChatBalloonGlobals *)SigmaMemoryNew( sizeof( UIChatBalloonGlobals ), 4, NULL, TRUE );

    for ( int32 i = 0; i < MAX_CHAT_BALLOONS; i++ )
    {
        sAddRActorBalloon( sgptUIChatBalloonGlobals->arUIFloats[i], RACTORID_INVALID, SNO_NONE, 0, 0, 0.0f, -1 );
        sgptUIChatBalloonGlobals->arUIFloats[i].idFloatBubble = GetUIIDNone();
    }


    SNOName snoname( UI_SNO, "ConsoleConversationOptions" );
    UIControlSourceKey tTemplateSrcKey( snoname.snoHandle, "ConsoleConversationOptions_main.LayoutRoot.chat_balloon" );





    for ( int32 i = 0; i < MAX_CHAT_BALLOONS; i++ )
    {
        UIControl * ptControl = UI_CreateControlFromTemplate( tTemplateSrcKey, DynamicControlCreationParams( CRefString().Format( "ractor floatbubble %d", i ), UIDLG_ITEMTAGS ), NULL );

        sgptUIChatBalloonGlobals->arUIFloats[i].idFloatBubble = ptControl->GetID();
    }
}



void DisposeForGame()

{
    if ( !sgptUIChatBalloonGlobals ) return;






    for ( int32 i = 0; i < MAX_CHAT_BALLOONS; i++ )
    {
        UI_DeleteControl( sgptUIChatBalloonGlobals->arUIFloats[i].idFloatBubble, TRUE );
    }

    SigmaMemoryFree( sgptUIChatBalloonGlobals, NULL ); sgptUIChatBalloonGlobals = NULL;  // inference: free and null assignment share line 119 in the line table

}
}  // namespace UIChatBalloons



static BOOL sGetRActorRect( RActorID idRActor, float flHeightOffset, Vector2D & wpCenter, Rect2D & rectActor )




{
    const RActor * ptRActor = RActorTryToGet( idRActor );


    if ( ptRActor == NULL || !RActorCanHaveChatBubble( ptRActor ) ) return FALSE;  // inference: the NULL test merges into the RActorTryToGet id check in the inlined lookup at 206; only the RActorCanHaveChatBubble branch (line 135) is in the line table




    Vector3D wp;

    int32 nHardpointIndex = HardpointIndexFromName( ptRActor->snoAppearance, HardpointHash( "HP_aura" ) );
    if ( nHardpointIndex != -1 )
    {
        PRTransform transform;
        RActorGetHardpointTransformByIndex( *ptRActor, nHardpointIndex, transform );
        wp = transform.wp;
    }
    else
    {
        RActorGetTopPoint( ptRActor, wp );
    }

    wp.z += flHeightOffset;



    wp.z += 3.0f;


    Vector2D wpUIC;
    if ( !WorldPointToScreenPointUIC( wp, wpUIC, -1 ) ) return FALSE;




    wpCenter = wpUIC;  // inference: dead store (wpCenter is never read after the loop, so no instruction); DWARF keeps wpCenter as a parameter of sGetRActorRect and as Update local line 240







    float flPadTop = 50.0f;  // inference: pad values from the folded fadd constants (-50 / 100 / +50) at lines 179-183
    float flPadBottom = 100.0f;
    float flPadHorizontal = 50.0f;

    rectActor.top = wpUIC.y - flPadTop;
    rectActor.bottom = wpUIC.y + flPadBottom;

    rectActor.left = wpUIC.x - flPadHorizontal;
    rectActor.right = wpUIC.x + flPadHorizontal;

    return TRUE;
}



static void sUpdateFloatVisibility( UIFloat & tFloat, Vector2D & wpCenter, Rect2D & rectActor )



{
    if ( !UI_TestID( tFloat.idFloatBubble ) ) return;
    tFloat.bVisible = FALSE;

    UI_GetFloatBubbleFromID( tFloat.idFloatBubble ).SetActive( FALSE );

    if ( CGameGet().ptUIGlobals == NULL
        || UIGame::MenuIsActive()
        || UIGame::IsShowingNotification() ) return;



    RActor * ptRActor = RActorTryToGet( tFloat.idRActor );
    if ( ptRActor == NULL ) return;  // inference: the original skips the rest when the lookup fails (branch to the loop increment at line 206/207)












    if ( tFloat.dwExpiration < CGameGetRealTicks() )
    {
        sAddRActorBalloon( tFloat, RACTORID_INVALID, SNO_NONE, 0, 0, 0.0f, -1 );
        return;
    }

    if ( !sGetRActorRect( tFloat.idRActor, tFloat.flHeightOffset, wpCenter, rectActor ) ) return;




    tFloat.bVisible = TRUE;
}


namespace UIChatBalloons {

void Update()

{
    Vector2D wpCenter[MAX_CHAT_BALLOONS];
    Rect2D rectActor[MAX_CHAT_BALLOONS];

    for ( int32 i = 0; i < MAX_CHAT_BALLOONS; i++ )
    {
        sUpdateFloatVisibility( sgptUIChatBalloonGlobals->arUIFloats[i], wpCenter[i], rectActor[i] );
    }

    for ( int32 i = 0; i < MAX_CHAT_BALLOONS; i++ )
    {
        UIFloat * ptFloat = &sgptUIChatBalloonGlobals->arUIFloats[i];

        if ( !ptFloat->bVisible ) continue;  // inference: skip when not visible (cbz to the loop increment at line 252)




        if ( !UI_TestID( ptFloat->idFloatBubble ) ) return;  // inference: the branch lands on the function epilogue, same address for return or break
        UIFloatBubble & tText = UI_GetFloatBubbleFromID( ptFloat->idFloatBubble );

        tText.SetActive( TRUE );
        tText.SetEnabled( TRUE );

        CRefString sText = StringListFetchByLabel( ptFloat->snoStringList, ptFloat->hLabel );


        Rect2D rectTextUIC;
        rectTextUIC.Set( 0.0f, 0.0f, 300.0f, 720.0f );
        tText.SetWindowRect( rectTextUIC );


        if ( ptFloat->controllerIndex >= 0 )
        {
            tText.SetText( "", FALSE );
            tText.SetControllerIndexOverride( ptFloat->controllerIndex );
        }
        tText.SetText( sText, FALSE );
        tText.GetWindowRect( rectTextUIC );


        float flVerticalAdjustment = -40.0f;
        rectTextUIC.AlignToRect( rectActor[i], AP_HORIZONTAL_CENTER, AP_VERTICAL_ABOVE, flVerticalAdjustment );
        tText.SetWindowRect( rectTextUIC );
    }
}
}  // namespace UIChatBalloons



static void sAddRActorBalloon( UIFloat & tFloat, RActorID idRActor, SNO snoStringList, uint32 hLabel, uint32 dwTicksToDisplay, float flHeightOffset, int32 nControllerIndex )







{
    tFloat.idRActor = idRActor;
    tFloat.snoStringList = snoStringList;
    tFloat.hLabel = hLabel;
    tFloat.dwExpiration = CGameGetRealTicks() + dwTicksToDisplay;
    tFloat.flHeightOffset = flHeightOffset;
    tFloat.controllerIndex = nControllerIndex;


}
namespace UIChatBalloons {
void AddBalloon( RActorID idRActor, SNO snoStringList, uint32 hLabel, uint32 dwTicksToDisplay, float flHeightOffset, int32 nControllerIndex )






{











    int32 nIndex = -1;
    uint32 dwSoonestExpireTime = 0xFFFFFFFF;  // inference: 0xFFFFFFFF initial value with the nIndex == -1 test at line 340 reproduces the folded first iteration (no instruction of its own)

    for ( int32 i = 0; i < MAX_CHAT_BALLOONS; i++ )
    {
        UIFloat * ptFloat = &sgptUIChatBalloonGlobals->arUIFloats[i];
        if ( ptFloat->idRActor == RACTORID_INVALID )
        {
            nIndex = i;
            break;
        }


        if ( nIndex == -1 ||  // inference: see line 328
            ptFloat->dwExpiration < dwSoonestExpireTime )
        {
            nIndex = i;
            dwSoonestExpireTime = ptFloat->dwExpiration;
        }
    }





    sAddRActorBalloon( sgptUIChatBalloonGlobals->arUIFloats[nIndex], idRActor, snoStringList, hLabel, dwTicksToDisplay, flHeightOffset, nControllerIndex );
}



void CancelBalloonForRActor( RActorID idRActor )

{
    if ( !sgptUIChatBalloonGlobals ) return;




    for ( int32 i = 0; i < MAX_CHAT_BALLOONS; i++ )
    {
        UIFloat & tFloat = sgptUIChatBalloonGlobals->arUIFloats[i];
        if ( tFloat.idRActor == idRActor )
        {
            tFloat.bVisible = FALSE;
            tFloat.idRActor = RACTORID_INVALID;
        }
    }
}
}  // namespace UIChatBalloons
