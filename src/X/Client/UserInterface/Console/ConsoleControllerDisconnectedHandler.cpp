// Original file: X/Client/UserInterface/Console/ConsoleControllerDisconnectedHandler.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
// SNOName.h, WorldPlace.h and GfxVB.h only for their statics: the original's static initialiser
// runs them ahead of Online.h's nn::ldn constants.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../Console/Online.h"
#include "../../Console/GamerProfile.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../Sound/Sound.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"

// Declared here because their headers are not adopted yet; each comment names the defining file.
void GameRefPause(BOOL bPause);  // CGame.cpp:4113
BOOL CutsceneIsPlaying();  // Cutscene.cpp:2129
int32 UI_GetPrimaryUIController();  // UI.cpp:538
const UIID & UI_GetControllerFocus();  // UI.cpp:674

namespace ControllerDisconnectedHandler {

enum {
    MAX_CONTROLLERS = 4,
};

















struct ControllerConnectedGlobals
{
    BOOL bShowing;
    BOOL bPauseApplied;
    BOOL bControllersNeeded[MAX_CONTROLLERS];
    int32 nControllerNeedingReconnect;
    UIID idLastFocusedControl;


    ControllerConnectedGlobals()
    {
        bShowing = FALSE;

        for (uint32 i = 0; i < MAX_CONTROLLERS; ++i) { bControllersNeeded[i] = FALSE; }

        nControllerNeedingReconnect = -1;
        bPauseApplied = FALSE;

        idLastFocusedControl = GetUIIDNone();
    }
};





static ControllerConnectedGlobals sgtControllerConnectedGlobals;

namespace Console {

static void sCheckForMoreToShow();



void InitializeForGame()
{
    sgtControllerConnectedGlobals.bShowing = FALSE;
    sCheckForMoreToShow();
}

void ShowForController(int32 nControllerIndex)
{

    if (!sgtControllerConnectedGlobals.bShowing)
    {



        sgtControllerConnectedGlobals.nControllerNeedingReconnect = nControllerIndex;

        CRefString sController = UnicodeText::FormatNumber(nControllerIndex + 1, TRUE, FNS_NORMAL, 0);
        CRefString sText = LocalizedSprintf(StringListFetch("ConsoleUI:ControllerDisconnected").str(),
            sController.str(),
            ::Console::GamerProfile::GetGamerTag(nControllerIndex), NULL);

        UI_GetTextFromID(UITXT_CONSOLE_CONTROLLER_DISCONNECT_MESSAGE).SetText(sText.str(), FALSE);

        // Fit the frame to the list.
        UIID idContentFrame = UIIDCreateFromParent(UIDLG_CONSOLE_CONTROLLER_DISCONNECT, ".LayoutRoot.Notification.Content");
        UIID idContent = UIIDCreateFromParent(idContentFrame, ".List");
        Vector2D vecStackTL, vecStackDim;
        UI_GetWindowFromID(idContent).GetRelativePosition(vecStackTL, vecStackDim);
        UIWindow & tFrameWnd = UI_GetWindowFromID(idContentFrame);
        Vector2D vecFrameTL, vecFrameDim;
        tFrameWnd.GetRelativePosition(vecFrameTL, vecFrameDim);
        vecFrameDim.y = vecStackDim.y;
        tFrameWnd.SetRelativePosition(vecFrameTL, vecFrameDim);

        UI_GetWindowFromID(UIDLG_CONSOLE_CONTROLLER_DISCONNECT).SetActive(TRUE);

        CRefString sButtonLegendDummyText("");
        UI_GetTextFromID(UITXT_CONSOLE_CONTROLLER_DISCONNECT_BUTTON_LEGEND).SetText(sButtonLegendDummyText.str(), FALSE);

        sgtControllerConnectedGlobals.idLastFocusedControl = UI_GetControllerFocus();

        UI_SetControllerFocus(GetUIIDNone());

        sgtControllerConnectedGlobals.bShowing = TRUE;

        GameRefPause(TRUE);
        if (!CutsceneIsPlaying())
        {
            SoundManager::SetGlobalMute(TRUE);
        }
    }
}




void Hide()
{
    UI_GetWindowFromID(UIDLG_CONSOLE_CONTROLLER_DISCONNECT).SetActive(FALSE);







    if (sgtControllerConnectedGlobals.idLastFocusedControl != GetUIIDNone())
    {
        UI_SetControllerFocus(sgtControllerConnectedGlobals.idLastFocusedControl);
    }

    sgtControllerConnectedGlobals.bShowing = FALSE;

    GameRefPause(FALSE);
    SoundManager::SetGlobalMute(FALSE);
}



static void sCheckForMoreToShow()
{
    for (uint32 i = 0; i < MAX_CONTROLLERS; ++i)
    {
        if (sgtControllerConnectedGlobals.bControllersNeeded[i] == TRUE)
        {
            ShowForController(i);
            break;
        }
    }
}


void ControllerConnected(int32 nControllerIndex)
{
    sgtControllerConnectedGlobals.bControllersNeeded[nControllerIndex] = FALSE;

    if (sgtControllerConnectedGlobals.nControllerNeedingReconnect == nControllerIndex)
    {
        sgtControllerConnectedGlobals.nControllerNeedingReconnect = -1;
        if (sgtControllerConnectedGlobals.bShowing)
        {
            Hide();
            sCheckForMoreToShow();
        }
    }
}

void ControllerDisconnected(int32 nControllerIndex)
{
    BOOL bNeedReconnect = ::Console::GamerProfile::GetPrimaryProfileUserIndex() != -1 &&
        (UI_GetPrimaryUIController() == nControllerIndex || ::Console::Online::PartyIsInParty(nControllerIndex));
    if (bNeedReconnect)
    {
        sgtControllerConnectedGlobals.bControllersNeeded[nControllerIndex] = TRUE;
        ShowForController(nControllerIndex);
    }
}

}  // namespace Console
}  // namespace ControllerDisconnectedHandler
