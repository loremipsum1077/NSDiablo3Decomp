#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
// blz/tuple.h for blz::ignore, which sits at the front of the merged statics block.
#include "../../../../Contrib/Contrib/blz/include/blz/tuple.h"
// SNOName.h, WorldPlace.h and GfxVB.h only for their statics: the original's static initialiser
// runs them ahead of sgtUIConfirmGlobals.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "UIConfirmation.h"
#include "../../CPlayerInput.h"
#include "UIFunctions.h"
#include "../UIID.h"
#include "../Controls/UIStackPanelControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/SigmaTypes.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
struct PlaySoundOptions;
typedef uint32 SoundID;
// X/Client/Sound/Sound.cpp
namespace SoundManager { SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions); }
extern const PlaySoundOptions cgtPSODefaults;

class UIConfirmationGlobals {
public:


    UIConfirmationGlobals()
    {


        pfnOnClickOK = NULL;
        pfnOnClickCancel = NULL;
        bCloseOnMovement = FALSE;
        eCursorState = CURSORSTATE_NORMAL;
        nControllerIndex = -1;
        idLastFocusedControl = GetUIIDNone();
    }

    UIFunctionPtr pfnOnClickOK;  // 0x00
    UIFunctionPtr pfnOnClickCancel;  // 0x08
    BOOL bCloseOnMovement;  // 0x10
    CursorState eCursorState;  // 0x14
    int32 nControllerIndex;  // 0x18
    UIID idLastFocusedControl;  // 0x20
};
// X/Client/Cursor.cpp
CursorMgr & PlayerGetCursorMgr();
namespace Console { namespace GamerProfile { int32 GetPrimaryProfileUserIndex(); } }  // GamerProfile.cpp
// X/Client/UserInterface/UI.cpp
BOOL UI_TestID(const UIID & id);
UIWindow & UI_GetWindowFromID(const UIID & id);
UIStackPanel & UI_GetStackPanelFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
void UI_SetControllerFocus(const UIID & id);
const UIID & UI_GetControllerFocus();
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_CONFIRMATION, UITXT_CONFIRMATION_MSG;
static UIConfirmationGlobals sgtUIConfirmGlobals;



// PlayerHasControl, OnOK, OnCancel and Reset are not inline: the CU keeps linker-stripped
// out-of-line copies of each (low_pc 0), so the original defined them as plain functions.
BOOL PlayerHasControl(int32 nControllerIndex)
{
    return sgtUIConfirmGlobals.nControllerIndex == -1 || sgtUIConfirmGlobals.nControllerIndex == nControllerIndex;
}

void ResizeWindowForContent(const UIID & id)

{
    UIID idContent = UIIDCreateFromParent(id, ".LayoutRoot.Notification.Content.List");
    UIID idContentFrame = UIIDCreateFromParent(id, ".LayoutRoot.Notification.Content");
    if (UI_TestID(idContentFrame) &&
        UI_TestID(idContent))
    {
        UIWindow & tDialogContentFrame = UI_GetWindowFromID(idContentFrame);
        UIStackPanel & tDialogContent = UI_GetStackPanelFromID(idContent);
        Vector2D vecFrameTL, vecFrameDim;
        Vector2D vecStackTL, vecStackDim;

        tDialogContent.GetRelativePosition(vecStackTL, vecStackDim);
        tDialogContentFrame.GetRelativePosition(vecFrameTL, vecFrameDim);
        vecFrameDim.y = vecStackDim.y;
        tDialogContentFrame.SetRelativePosition(vecFrameTL, vecFrameDim);
    }
}
void UIConfirmation::OnOK(const UIID & id)
{

    UIFunctionPtr pfnCallBack = sgtUIConfirmGlobals.pfnOnClickOK;
    CursorState eCursorState = sgtUIConfirmGlobals.eCursorState;
    Reset();


    Close();

    if (pfnCallBack)
    {
        pfnCallBack(id);
    }

    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    if (eCursorState == CURSORSTATE_SALVAGE_ITEM)
    {

        tCursorMgr.SalvageItem();
    }
}








void UIConfirmation::OnCancel(const UIID & id)
{

    UIFunctionPtr pfnCallBack = sgtUIConfirmGlobals.pfnOnClickCancel;
    Reset();


    Close();

    if (pfnCallBack)
    {
        pfnCallBack(id);
    }
}



void UIConfirmation::Reset()
{

    sgtUIConfirmGlobals.eCursorState = CURSORSTATE_NORMAL;
    sgtUIConfirmGlobals.pfnOnClickOK = NULL;
    sgtUIConfirmGlobals.pfnOnClickCancel = NULL;

    sgtUIConfirmGlobals.nControllerIndex = -1;
}





BOOL UIConfirmation::IsVisible()

{
    if (UI_TestID(UIDLG_CONFIRMATION))
        return UI_GetWindowFromID(UIDLG_CONFIRMATION).IsActive();
    return FALSE;
}

// UIConfirmation::IsOpenWithFunction(UIFunctionPtr pfnOnClickOK) was defined here (line 164); the
// linker stripped it and no caller survives, so its body cannot be checked and is left out.














void UIConfirmation::Close()

{







    UI_GetWindowFromID(UIDLG_CONFIRMATION).SetActive(FALSE);


    UI_SetControllerFocus(sgtUIConfirmGlobals.idLastFocusedControl);
}















void UIConfirmation::Show(LPCSTR szMessage,
                          UIFunctionPtr pfnOnClickOK,
                          UIFunctionPtr pfnOnClickCancel,
                          BOOL bCloseOnMovement,
                          BOOL bRestoreCursorStateOnOK,
                          BOOL bUseHotkeys,
                          int32 nControllerIndex)




{




    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    if (bRestoreCursorStateOnOK)
    {
        sgtUIConfirmGlobals.eCursorState = tCursorMgr.GetCursorState();
    }
    tCursorMgr.ResetCursorState(FALSE);

    UI_GetWindowFromID(UIDLG_CONFIRMATION).SetActive(TRUE);
    UI_GetTextFromID(UITXT_CONFIRMATION_MSG).SetText(szMessage, FALSE);


    if (nControllerIndex == -1)
    {
        nControllerIndex = Console::GamerProfile::GetPrimaryProfileUserIndex();
    }
    UIID idOk = UIIDCreateFromParent(UIDLG_CONFIRMATION, ".LayoutRoot.Notification.Buttons.ButtonList.OkButton");
    UIID idCancel = UIIDCreateFromParent(UIDLG_CONFIRMATION, ".LayoutRoot.Notification.Buttons.ButtonList.Cancel");
    UI_GetTextFromID(idOk).SetControllerIndexOverride(nControllerIndex);
    UI_GetTextFromID(idCancel).SetControllerIndexOverride(nControllerIndex);








    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONFIRMATION_OPEN), cgtPSODefaults);

    sgtUIConfirmGlobals.pfnOnClickOK = pfnOnClickOK;
    sgtUIConfirmGlobals.pfnOnClickCancel = pfnOnClickCancel;
    sgtUIConfirmGlobals.bCloseOnMovement = bCloseOnMovement;

    sgtUIConfirmGlobals.nControllerIndex = nControllerIndex;

    sgtUIConfirmGlobals.idLastFocusedControl = UI_GetControllerFocus();
    UI_SetControllerFocus(GetUIIDNone());

    ResizeWindowForContent(UIDLG_CONFIRMATION);
}






void UIConfirmation::PlayerMoved(int32 nControllerIndex)





{

    if (PlayerHasControl(nControllerIndex))
    {




        if (UI_GetWindowFromID(UIDLG_CONFIRMATION).IsActive() && sgtUIConfirmGlobals.bCloseOnMovement)
        {
            UI_GetWindowFromID(UIDLG_CONFIRMATION).SetActive(FALSE);
        }
    }
}





void UIConfirmation::OnButton1(const UIID & id,
                               const int32 nControllerIndex)
{

    if (PlayerHasControl(nControllerIndex))
    {
        OnOK(id);
    }
}




void UIConfirmation::OnButton2(const UIID & id,
                               const int32 nControllerIndex)
{

    if (PlayerHasControl(nControllerIndex))
    {
        OnCancel(id);
    }
}
