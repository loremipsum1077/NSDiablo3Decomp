// Original file: X/Client/UserInterface/Interface/UIMap.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Battlenet/Hub.h"
#include "../../Battlenet/Subsystems/ScreenManager.h"
#include "../../Console/ControllerUtils.h"
#include "../../CPlayer.h"
#include "../../CPlayerInput.h"
#include "../../Sound/Sound.h"
#include "../Console/ConsoleScreenManager.h"
#include "../Console/ControllerActions.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "UIGame.h"
#include "UIMinimap.h"
#include "UIWaypointMapCommon.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Definitions/LevelAreaDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/NephalemRiftCommon.h"
#include "../../../Common/Player.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Sigma/DataStructures/XBitArray.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
// Declared here because their headers are not adopted yet (UIGlobals.cpp, UI.cpp, UIMasterIdList.cpp, UIMinimap.cpp, UIWaypointMap.cpp, CPlayerInput.cpp, RActors.cpp, BitManip.cpp, Tutorials.h, UITutorials.h, ConsoleScreenManager.cpp).
extern UIID UITXT_MAP_AREA_NAME; extern UIID UITXT_MAP_INSTRUCTIONS; extern UIID UIDLG_WAYPOINTMAP_PAGE; extern UIID UIBTN_MAP_ZOOM_IN; extern UIID UIBTN_MAP_ZOOM_OUT; extern UIID UIDLG_MAP_LOCALMAP;
extern UIID UISTACK_OBJHUD_MAINQUEST_REGION; extern UIID UISTACK_OBJHUD_SIDEQUEST_REGION; extern UIID UIWND_MAP_NAV_CONSOLE; extern UIID UIWND_MAP_NAV_CONSOLE_CAMPAIGN;
void UISideScreenSetVisible(const UIID & id, const BOOL fVisible); UIMinimap & UI_GetMinimapFromID(const UIID & id); namespace UIWaypointMap { void Close(); }
CursorMgr & PlayerGetCursorMgr(); void CPlayerInputForceHighlightUpdate(Player * ptPlayer); SNO RActorGetLevelArea(RActorID idRActor); int32 CountSetBits(const uint8 * lpb, const int32 nLen); namespace Tutorial { BOOL PlayerHasSeen(const Player & tPlayer, const SNO snoTutorial); } class UITutorial;
#include "UITutorials.h"
extern UITutorialManager gtTutorialMgr; Hub & HubGet(); namespace Console { namespace Screens { BOOL OpenToScreen(const ScreenSets eScreenSet, const ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition); } }

namespace UIMap {

inline void OnSideScreenShowHide(const UIID & id, BOOL fWindowOpen);












void Update()
{

    if (UI_GetWindowFromID(UIDLG_MAP_PAGE).IsActive())
    {



        CRefString sInstructions;


        if (UI_TestID(UITXT_MAP_AREA_NAME))
        {
            const ACDID idACDLocalPlayer = LocalPlayerACDGet();
            if (idACDLocalPlayer != ACDID_INVALID)
            {
                CRefString usAreaName;

                const RActorID idPlayerRActor = LocalPlayerRActorGet();
                const SNO snoLevelArea = RActorGetLevelArea(idPlayerRActor);
                if (snoLevelArea != SNO_NONE)
                {
                    usAreaName = LevelAreaGetLocalizedName(snoLevelArea);
                }

                if (NephalemRift::IsTieredRiftActive())
                {
                    const int32 nDungeonFinderLevel = LevelAreaGetDungeonFinderLevel(snoLevelArea);
                    if (nDungeonFinderLevel > 0)
                    {
                        CRefString sVal = UnicodeText::FormatNumber(nDungeonFinderLevel, TRUE, FNS_NORMAL, 0);
                        usAreaName = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_LEVEL_AREAS), "TieredRift").str(), sVal.str(), NULL);
                    }
                }

                UIText & tAreaNameText = UI_GetTextFromID(UITXT_MAP_AREA_NAME);
                tAreaNameText.SetText(usAreaName, FALSE);
                tAreaNameText.SetActive(TRUE);
            }
        }

        ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
        tActionManager.ClearAllActions();
        tActionManager.SetAction(0, 0, StringListFetch("ConsoleMap:MoveMap").str());


        tActionManager.SetAction(12, 0, StringListFetch("ConsoleMap:ZoomIn").str());
        tActionManager.SetAction(13, 0, StringListFetch("ConsoleMap:ZoomOut").str());
        tActionManager.SetAction(14, 0, StringListFetch("ConsoleMap:RecenterMap").str());
        tActionManager.SetAction(15, 0, StringListFetch("ConsoleMap:ResetZoom").str());






        tActionManager.SetAction(7, 0, StringListFetch("ConsoleUI:Close").str());
        ::Console::Screens::UpdateButtonLegendText();


        ::Console::Screens::EnableScreen(::Console::Screens::MAP_SCREEN_SET, 2, GameIsOpenWorld() && !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT));














        UI_GetTextFromID(UITXT_MAP_INSTRUCTIONS).SetText(sInstructions, FALSE);
    }
}



void Toggle(const UIID &)
{


    if (UIGame::MenuIsActive()) { return; }





    if (UIWaypointMap::IsOpenForTutorial() && !UIWaypointMap::TutorialCanBeClosed(LocalPlayerGet()))
    {
        return;
    }

    if (UI_TestID(UIDLG_MAP_PAGE)) {
        BOOL bNewActiveState = !UI_GetWindowFromID(UIDLG_MAP_PAGE).IsActive() && !UI_GetWindowFromID(UIDLG_WAYPOINTMAP_PAGE).IsActive();

        if (HubGet().ScreenManagerGet().GetCurrentOverlay() == -1)
        {



            if (!bNewActiveState)
            {
                UIWaypointMap::Close();
            }





            UISideScreenSetVisible(UIDLG_MAP_PAGE, bNewActiveState);

            UI_GetButtonFromID(UIBTN_MAP_ZOOM_IN).SetEnabled(FALSE);
            if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
            {
                UI_GetButtonFromID(UIBTN_MAP_ZOOM_OUT).SetEnabled(FALSE);
            }
        }
    }
}













void OpenForController(int32 nControllerIndex)
{


    if (UI_TestID(UIDLG_MAP_PAGE)) {
        UIControl & tControlSideScreen = UI_GetControlFromID(UIDLG_MAP_PAGE);
        if (!tControlSideScreen.IsActive()) {

            tControlSideScreen.SetActive(TRUE);

            OnSideScreenShowHide(GetUIIDNone(), TRUE);
        }


        if (UI_TestID(UISTACK_OBJHUD_MAINQUEST_REGION))
        {
            UI_GetWindowFromID(UISTACK_OBJHUD_MAINQUEST_REGION).SetActive(TRUE);
        }

        if (UI_TestID(UISTACK_OBJHUD_SIDEQUEST_REGION))
        {
            UI_GetWindowFromID(UISTACK_OBJHUD_SIDEQUEST_REGION).SetActive(TRUE);
        }

        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            UI_GetWindowFromID(UIWND_MAP_NAV_CONSOLE).SetActive(FALSE);
            UI_GetWindowFromID(UIWND_MAP_NAV_CONSOLE_CAMPAIGN).SetActive(FALSE);
            ::Console::Screens::EnableScreen(::Console::Screens::MAP_SCREEN_SET, 1, FALSE);
            ::Console::Screens::EnableScreen(::Console::Screens::MAP_SCREEN_SET, 2, FALSE);
        }
        else
        {
            UI_GetWindowFromID(UIWND_MAP_NAV_CONSOLE).SetActive(GameIsOpenWorld());
            UI_GetWindowFromID(UIWND_MAP_NAV_CONSOLE_CAMPAIGN).SetActive(!GameIsOpenWorld());
        }


        const Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
        const SNO snoMapTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_X1_CONSOLE_MAPS);

        if (ptPlayer != NULL && !Tutorial::PlayerHasSeen(*ptPlayer, snoMapTutorial))
        {
            const int32 nActivatedWaypoints = CountSetBits((const uint8 *)&ptPlayer->tHeroStateData.tSavedData.m_ActivatedWaypoints, sizeof(ptPlayer->tHeroStateData.tSavedData.m_ActivatedWaypoints));
            if (nActivatedWaypoints > 1)
            {
                gtTutorialMgr.ShowTutorial(GetUIIDNone(), snoMapTutorial, ptPlayer, TRUE);
            }
        }

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_OPEN), cgtPSODefaults);
    }
}









void Close()
{





    if (UI_TestID(UIDLG_MAP_PAGE)) {
        UIControl & tControlSideScreen = UI_GetControlFromID(UIDLG_MAP_PAGE);
        if (tControlSideScreen.IsActive()) {

            tControlSideScreen.SetActive(FALSE);

            OnSideScreenShowHide(GetUIIDNone(), FALSE);
        }
    }
}





void OnButtonClose(const UIID & id, const int32 nControllerIndex)
{




    if (UI_GetPrimaryUIController() == nControllerIndex)
    {





        if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 9) || ::Console::ControllerUtils::IsButtonDown(nControllerIndex, 1))
        {




            ::Console::Screens::CloseScreenSet(TRUE, FALSE);
        }
    }
}

void OnControllerHotkey(const UIID &, int32 nControllerIndex)
{


    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer != NULL)
    {



        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL && !ptACDPlayer->IsAlive())
        {
            return;
        }

        if (!::Console::Screens::IsScreenSetOpen())
        {
            ::Console::Screens::OpenToScreen(::Console::Screens::MAP_SCREEN_SET, 0, nControllerIndex, FALSE);
        }
    }
}







inline void OnSideScreenShowHide(const UIID & id, BOOL fWindowOpen)
{




    PlayerGetCursorMgr().ResetCursorState(FALSE);
    if (fWindowOpen) { UI_GetMinimapFromID(UIDLG_MAP_LOCALMAP).Initialize(TRUE, GetUIIDNone()); }

















    CPlayerInputForceHighlightUpdate(LocalPlayerGetPrimary());
}


void ToolTip(const UIID & uId)
{
    UIGame::Panel_ToolTip(uId);


    gtTutorialMgr.ShowTutorial(uId, GlobalSNOGet(GLOBALSNO_TUTORIAL_MAP), NULL, TRUE);
}


BOOL IsOpen()
{

    if (UI_TestID(UIDLG_MAP_PAGE))
    {
        return UI_GetWindowFromID(UIDLG_MAP_PAGE).IsActive();
    }
    return FALSE;
}

}  // namespace UIMap
