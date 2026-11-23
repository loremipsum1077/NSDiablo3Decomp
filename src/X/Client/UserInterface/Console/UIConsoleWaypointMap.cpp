// Original file: X/Client/UserInterface/Console/UIConsoleWaypointMap.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../Gfx/GfxVB.h"
#include "../../Battlenet/UI/Util/UIUtil.h"
#include "../Interface/UIWaypointMapCommon.h"
#include "../Interface/UIBossElements.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "../../Console/ControllerUtils.h"
#include "../../ImageCollection.h"
#include "../../Input/Input.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Act.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Definitions/ActDefinition.h"
#include "../../../Common/Definitions/BossEncounterDefinition.h"
#include "../../../Common/Definitions/LevelAreaDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Monsters.h"
#include "../../../Common/Player.h"
#include "../../../Common/Quests/Quests.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/RefString.h"
#include <float.h>
struct CachedWaypoint { Act eSourceAct; WaypointInfo tInfo; }; typedef blz::vector<CachedWaypoint, blz::allocator<CachedWaypoint> > CachedWaypointArray; const CachedWaypointArray & ActGetCachedWaypointArray(); SNO ActGetLevelAreaByWaypointIndex(int32 nIndex);  /* X/Common/Act.h:13, :19; Act.cpp:146, :198 */ SNO QuestGetActiveBountyForLevelArea(SNO snoLevelArea, BOOL bCheckAdditionalLevelAreas); BOOL QuestIsComplete(const QuestInstance & tQuest); CRefString QuestsGetUIName(SNO snoQuest); BOOL QuestsOpenWorldTrackedSNOIsComplete(SNO sno);  // X/Common/Quests/Quests.cpp
extern UIID UIWND_WAYPOINTMAP_OVERLAY_WORLD, UIBTN_WAYPOINTMAP_OVERLAY_WORLD_ACT, UITXT_WAYPOINTMAP_OVERLAY_WORLD_TEXT, UIWND_WAYPOINTMAP_TUTORIAL_TEXT_WRAPPER, UITEXT_WAYPOINTMAP_TUTORIAL, UIWND_WAYPOINTMAP_CONSOLE_NAV_ACT, UIWND_WAYPOINTMAP_CONSOLE_NAV_WORLD, UIWND_WAYPOINTMAP_CONSOLE_NAV_ACT_CAMPAIGN, UIDLG_WAYPOINTMAP_PAGE, UIWND_WAYPOINTMAP_OVERLAY_POI, UIWND_WAYPOINTMAP_WORLDMAP; /* X/Client/UserInterface/UIGlobals.cpp */ class UITutorial;
#include "../Interface/UITutorials.h"
extern UITutorialManager gtTutorialMgr; /* X/Client/UserInterface/Interface/UITutorials.h */
namespace Console { namespace Screens { BOOL OpenToScreen(const ScreenSets eScreenSet, const ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition); } }  // ConsoleScreenManager.cpp
namespace UIWaypointMap { Act GetWaypointActForPlayer(const Player * ptPlayer); void Open(Player * ptPlayer, RActorID idRActor, Act eSelectedAct, BOOL bForTutorial); void OpenWaypointMap(int32 nControllerOpening); BOOL CanCloseSafely(); void CloseWaypointMap(int32 nControllerIndex); void Close(); void Update(); void SetAct3HubEntryPointAct(Act eAct); void OpenActMap(int32 nControllerOpening); void CloseActMap(); void OpenWorldMap(int32 nControllerOpening); void CloseWorldMap(); BOOL TutorialWasInterrupted(); void CancelTutorialWasInterrupted(); void SetupControlsForAct(Act eAct); void SetupControlsForWorldView(); void GetBountyCountForAct(Act eAct, int32 & nBountyCount, int32 & nCompletedBountyCount); void OnControllerHotkey(const UIID & id, int32 nControllerIndex); void OnButtonAccept(const UIID & id, const int32 nControllerIndex); void OnButtonClose(const UIID & id, const int32 nControllerIndex); void ShowTutorialText(const CRefString & sText); void SetZKConversationComplete(); } enum { ACTION_ACCEPT = 0, ACTION_CLOSE = 1, ACTION_SHOW_AREA_MAP = 2, ACTION_SHOW_ACT_MAP = 3, ACTION_SHOW_WORLD_MAP = 4, ACTION_SHOW_ALL_BOUNTIES = 5, ACTION_SHOW_ALL_BOSSES = 6, };
struct MapTarget {  // size 0x18
    Vector2D vecPosition;  // 0x00
    int32 nIndex;  // 0x08
    UIID id;  // 0x10
};
typedef blz::vector<MapTarget, blz::allocator<MapTarget> > MapTargetArray;

enum ViewMode {
    VIEWMODE_NORMAL = 0,
    VIEWMODE_BOUNTIES = 1,
    VIEWMODE_BOSSES = 2,
};




struct UIWaypointMapGlobals {  // size 0x58
    UIWaypointMap::TutorialState tTutorialState;  // 0x00
    BOOL bTutorialWasInterrupted;  // 0x10
    Act eAct3HubEntryPointAct;  // 0x14
    Act eAct3HubEntryPointQueuedOverride;  // 0x18
    Act eCurrentDisplayedAct;  // 0x1c
    int32 nControllerIndex;  // 0x20
    MapTargetArray arMapTargets;  // 0x28
    int32 nCurrentWaypointIndex;  // 0x40
    int32 nTargetWaypointIndex;  // 0x44
    BOOL bTargetChanged;  // 0x48
    BOOL bZKConversationComplete;  // 0x4c
    Blizzard::Time::Millisecond nTargetChangedTime;  // 0x50
    ViewMode eViewMode;  // 0x54











    UIWaypointMapGlobals()
        : bTutorialWasInterrupted(FALSE)
        , eAct3HubEntryPointAct(-1)
        , eAct3HubEntryPointQueuedOverride(-1)
        , eCurrentDisplayedAct(-1)
        , nControllerIndex(-1)
        , nCurrentWaypointIndex(-1)
        , nTargetWaypointIndex(-1)
        , bTargetChanged(FALSE)
        , bZKConversationComplete(FALSE)
        , nTargetChangedTime(0)
        , eViewMode(VIEWMODE_NORMAL)
    {
    }
};





UIWaypointMapGlobals sgtWaypointMapGlobals;

static void sUpdateBossTextFlyout(int32 nIndex);
static void sUpdateBountyTextFlyout(int32 nIndex);
static void sUpdateTextFlyout(int32 nIndex);
static void sShowActMap(Player * ptPlayer, Act eAct);
static void sShowWorldMap();










Act UIWaypointMap::GetWaypointActForPlayer(const Player * ptPlayer)
{



    Act eResult = -1;

    if (!GameIsOpenWorld())
    {
        eResult = GameGetCurrentAct();
    }

    if (eResult == -1)
    {
        SNO snoLastWaypointLevelArea = PlayerGetLastWaypointLevelArea(ptPlayer);
        eResult = LevelAreaGetAct(snoLastWaypointLevelArea);


        if (eResult == 200 &&
            snoLastWaypointLevelArea == GlobalSNOGet(GLOBALSNO_LEVELAREA_ACT_3_HUB) &&
            sgtWaypointMapGlobals.eAct3HubEntryPointAct != -1)
        {
            eResult = sgtWaypointMapGlobals.eAct3HubEntryPointAct;
        }
    }

    return eResult;
}





















BOOL UIWaypointMap::IsOpenForTutorial()
{

    return IsOpen() && sgtWaypointMapGlobals.tTutorialState.bOpenForTutorial;
}









static BOOL sIsWorldMapActive()
{

    return UI_GetWindowFromID(UIWND_WAYPOINTMAP_WORLDMAP).IsActive();
}




static BOOL sWaypointHasBounty(int32 nIndex)
{

    SNO snoLevelArea = (SNO)(intptr)UI_GetControlFromID(sgtWaypointMapGlobals.arMapTargets[nIndex].id).GetUserData();
    SNO snoBounty = QuestGetActiveBountyForLevelArea(snoLevelArea, FALSE);
    return snoBounty != SNO_NONE;
}




static BOOL sWaypointHasBoss(int32 nIndex)
{

    return UI_GetControlFromID(UIIDCreateFromParent(sgtWaypointMapGlobals.arMapTargets[nIndex].id, ".LayoutRoot.Interest.Boss")).GetUserData() != NULL;
}




static BOOL sWaypointHasEventPOI(int32 nIndex)
{

    return UI_GetControlFromID(UIIDCreateFromParent(sgtWaypointMapGlobals.arMapTargets[nIndex].id, ".LayoutRoot.Interest.AnniversaryDungeon")).IsActive();
}




static void sUpdateBossTextFlyout(int32 nIndex)
{

    CRefString usTip;

    UIID idBoss = UIIDCreateFromParent(sgtWaypointMapGlobals.arMapTargets[nIndex].id, ".LayoutRoot.Interest.Boss");
    uintptr pData = (uintptr)UI_GetControlFromID(idBoss).GetUserData();
    if (pData)
    {
        SNO snoBossEncounter = (SNO)pData;
        ScopedBossEncounterDefinitionLockR pdefBossEncounter(snoBossEncounter);

        CRefString sEncounterName;
        if (pdefBossEncounter->snoActor == SNO_NONE || !MonsterGetBaseName(SNOName(ACTOR_SNO, pdefBossEncounter->snoActor), sEncounterName))
        {
            sEncounterName = StringListFetchByLabel(GlobalSNOGet(GLOBALSNO_BOSS_STRINGLIST), pdefBossEncounter->hEncounterNameLabel);
        }

        if (QuestsOpenWorldTrackedSNOIsComplete(snoBossEncounter))
        {
            usTip += StringListFetch("WaypointMap:Completed");
            usTip += " ";
        }

        usTip += sEncounterName;
    }

    UI_GetTextFromID(UIIDCreateFromParent(sgtWaypointMapGlobals.arMapTargets[nIndex].id, ".LayoutRoot.FlyoutWrapper.Flyout")).SetText(usTip, FALSE);
}




static void sUpdateBountyTextFlyout(int32 nIndex)
{

    SNO snoLevelArea = (SNO)(intptr)UI_GetControlFromID(sgtWaypointMapGlobals.arMapTargets[nIndex].id).GetUserData();
    SNO snoBountyQuest = QuestGetActiveBountyForLevelArea(snoLevelArea, FALSE);
    CRefString usTip;

    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoBountyQuest, ACDID_INVALID);
    if (ptQuestInstance)
    {
        if (QuestIsComplete(*ptQuestInstance))
        {
            usTip = StringListFetch("ConsoleWaypoint:BountyComplete");
        }
        else
        {
            usTip = QuestsGetUIName(snoBountyQuest);
        }
    }

    UI_GetTextFromID(UIIDCreateFromParent(sgtWaypointMapGlobals.arMapTargets[nIndex].id, ".LayoutRoot.FlyoutWrapper.Flyout")).SetText(usTip, FALSE);
}




static void sUpdateEventPOITextFlyout(int32 nIndex)
{

    UIWindow & tWindow = UI_GetWindowFromID(UIIDCreateFromParent(sgtWaypointMapGlobals.arMapTargets[nIndex].id, ".LayoutRoot.Interest.AnniversaryDungeon"));
    CRefString szTooltip = StringListFetch(tWindow.GetTooltipTextLabel());

    UI_GetTextFromID(UIIDCreateFromParent(sgtWaypointMapGlobals.arMapTargets[nIndex].id, ".LayoutRoot.FlyoutWrapper.Flyout")).SetText(szTooltip, FALSE);
}




static void sUpdateTextFlyout(int32 nIndex)
{

    BOOL bHasBounty = sWaypointHasBounty(nIndex);
    BOOL bHasBoss = sWaypointHasBoss(nIndex);
    BOOL bHasEventPOI = sWaypointHasEventPOI(nIndex);


    if (bHasBounty && sgtWaypointMapGlobals.eViewMode != VIEWMODE_BOSSES)
    {
        sUpdateBountyTextFlyout(nIndex);
    }
    else if (bHasBoss)
    {
        sUpdateBossTextFlyout(nIndex);
    }
    else if (bHasEventPOI)
    {
        sUpdateEventPOITextFlyout(nIndex);
    }
}




static void sSetTargetWaypoint(int32 nTargetIndex)
{

    sgtWaypointMapGlobals.nTargetWaypointIndex = nTargetIndex;
    sgtWaypointMapGlobals.nTargetChangedTime = Blizzard::Time::Milliseconds();
    sgtWaypointMapGlobals.bTargetChanged = TRUE;

    if (!sIsWorldMapActive())
    {
        if (nTargetIndex != -1 && GameIsOpenWorld())
        {
            sUpdateTextFlyout(nTargetIndex);
        }
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}




static void sClearMapTargets()
{

    sgtWaypointMapGlobals.nCurrentWaypointIndex = -1;
    sgtWaypointMapGlobals.nTargetWaypointIndex = -1;
    sgtWaypointMapGlobals.bTargetChanged = FALSE;
    sgtWaypointMapGlobals.arMapTargets.clear();
}




static void sCacheActMapTargets()
{

    Vector2D vecTL, vecDim, vecCenterPoint;
    UIWindow & tParentWindow = UI_GetWindowFromID(UIWND_WAYPOINTMAP_OVERLAY_POI);

    int32 nCurrentLocationIndex = -1;
    int32 nCurrentQuestLocIndex = -1;

    SNO snoQuestWaypoint = SNO_NONE;
    const Player * ptPlayer = PlayerGetByControllerIndex(sgtWaypointMapGlobals.nControllerIndex);
    if (ptPlayer)
    {
        snoQuestWaypoint = UIWaypointMap::GetQuestWaypointLevelArea(ptPlayer);
    }

    const UIControlItemArray & arChildren = tParentWindow.GetChildList();
    for (uint32 i = 0; i < arChildren.size(); i++)
    {

        UIID idName = UIIDCreateFromParent(arChildren[i].id, ".LayoutRoot.Name");
        UIButton & tNameButton = UI_GetButtonFromID(idName);

        const Rect2D rect = tNameButton.GetWindowRect();

        vecCenterPoint.Set(rect.left + rect.Width() * 0.5f, rect.top + rect.Height() * 0.5f);
        int32 nIndex = (int32)(intptr)tNameButton.GetUserData();

        MapTarget tMapTarget = { vecCenterPoint, nIndex, arChildren[i].id };
        sgtWaypointMapGlobals.arMapTargets.push_back(tMapTarget);


        if (UI_GetWindowFromID(UIIDCreateFromParent(arChildren[i].id, ".LayoutRoot.Selected")).IsItselfActive())
        {
            nCurrentLocationIndex = i;
        }


        if (snoQuestWaypoint != SNO_NONE)
        {
            SNO snoLevelArea = (SNO)(intptr)UI_GetControlFromID(arChildren[i].id).GetUserData();
            if (snoLevelArea == snoQuestWaypoint)
            {
                nCurrentQuestLocIndex = i;
            }
        }
    }

    int32 nTargetIndex = -1;
    if (GameIsOpenWorld() || nCurrentQuestLocIndex == -1)
    {
        nTargetIndex = nCurrentLocationIndex;
    }
    else
    {
        nTargetIndex = nCurrentQuestLocIndex;
    }






    if (nTargetIndex != -1)
    {
        sSetTargetWaypoint(nTargetIndex);
    }
}



static void sCacheWorldMapTargets()
{

    Vector2D vecTL, vecDim, vecCenterPoint;
    UIWindow & tParentWindow = UI_GetWindowFromID(UIWND_WAYPOINTMAP_WORLDMAP);

    const UIControlItemArray & arChildren = tParentWindow.GetChildList();
    for (uint32 i = 0; i < arChildren.size(); i++)
    {

        UIID idName = UIIDCreateFromParent(arChildren[i].id, ".LayoutRoot.Name");
        UIButton & tNameButton = UI_GetButtonFromID(idName);

        const Rect2D rect = tNameButton.GetWindowRect();

        vecCenterPoint.Set(rect.left + rect.Width() * 0.5f, rect.top + rect.Height() * 0.5f);
        int32 nIndex = (int32)(intptr)UI_GetWindowFromID(arChildren[i].id).GetUserData();

        MapTarget tMapTarget = { vecCenterPoint, nIndex };
        sgtWaypointMapGlobals.arMapTargets.push_back(tMapTarget);


        UIID idSelected = UIIDCreateFromParent(arChildren[i].id, ".LayoutRoot.Selected");
        if (UI_GetWindowFromID(idSelected).IsItselfActive())
        {
            sgtWaypointMapGlobals.nCurrentWaypointIndex = sgtWaypointMapGlobals.nTargetWaypointIndex = i;

            sgtWaypointMapGlobals.nTargetChangedTime = 0;
        }
    }
}




static int32 sGetTownWaypointIndex()
{

    const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
    for (int32 i = 0; i < (int32)tWaypoints.size(); i++)
    {
        const CachedWaypoint & tWaypoint = tWaypoints[i];
        if (LevelAreaIsTown(tWaypoint.tInfo.snoLevelArea))
        {
            return i;
        }
    }

    return -1;
}




static void sShowActMap(Player * ptPlayer, Act eAct)

{

    sgtWaypointMapGlobals.eCurrentDisplayedAct = eAct;

    UIWaypointMap::ShowActMap(eAct);
    UIWaypointMap::SetupWaypointControls(ptPlayer, eAct);
    UIWaypointMap::SetupBountyOverlay(ptPlayer, eAct);

    UI_GetWindowFromID(UIWND_WAYPOINTMAP_CONSOLE_NAV_ACT).SetActive(GameIsOpenWorld());
    UI_GetWindowFromID(UIWND_WAYPOINTMAP_CONSOLE_NAV_ACT_CAMPAIGN).SetActive(!GameIsOpenWorld());
    UI_GetWindowFromID(UIWND_WAYPOINTMAP_CONSOLE_NAV_WORLD).SetActive(FALSE);


    UI_GetWindowFromID(UIWND_WAYPOINTMAP_OVERLAY_WORLD).SetActive(FALSE);

    sClearMapTargets();
    sCacheActMapTargets();


    if (sgtWaypointMapGlobals.nTargetWaypointIndex == -1 && sgtWaypointMapGlobals.arMapTargets.size() > 0)
    {
        sSetTargetWaypoint(sGetTownWaypointIndex());
    }

    if (eAct == 200 && GameIsOpenWorld() && sgtWaypointMapGlobals.bZKConversationComplete)
    {
        gtTutorialMgr.ShowTutorial(UIWND_WAYPOINTMAP_OVERLAY_POI, GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_CUBE_SESCHERON), NULL, TRUE);
        sgtWaypointMapGlobals.bZKConversationComplete = FALSE;
    }
}















static void sShowWorldMap()
{

    sgtWaypointMapGlobals.eCurrentDisplayedAct = -1;

    Player * ptPlayer = PlayerGetByControllerIndex(sgtWaypointMapGlobals.nControllerIndex);
    UIWaypointMap::ShowWorldMap();
    UIWaypointMap::SetupWorldControls(ptPlayer);

    UI_GetWindowFromID(UIWND_WAYPOINTMAP_CONSOLE_NAV_ACT).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_WAYPOINTMAP_CONSOLE_NAV_ACT_CAMPAIGN).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_WAYPOINTMAP_CONSOLE_NAV_WORLD).SetActive(TRUE);

    sClearMapTargets();
    sCacheWorldMapTargets();
}




static void sUpdateNavButtons(const UIID & idNav, const BOOL bEnable)
{


    UI_GetWindowFromID(UIIDCreateFromParent(idNav, ".KeyBinding1")).SetActive(bEnable);
    UI_GetWindowFromID(UIIDCreateFromParent(idNav, ".KeyBinding2")).SetActive(bEnable);
}




static void sEnableTabNavigation(const BOOL bEnable)
{



    ::Console::Screens::EnableScreen(::Console::Screens::MAP_SCREEN_SET, 0, bEnable);
    ::Console::Screens::EnableScreen(::Console::Screens::MAP_SCREEN_SET, 1, bEnable);


    ::Console::Screens::EnableScreen(::Console::Screens::MAP_SCREEN_SET, 2, bEnable && GameIsOpenWorld());


    sUpdateNavButtons(UIWND_WAYPOINTMAP_CONSOLE_NAV_ACT, bEnable);
    sUpdateNavButtons(UIWND_WAYPOINTMAP_CONSOLE_NAV_ACT_CAMPAIGN, bEnable);
    sUpdateNavButtons(UIWND_WAYPOINTMAP_CONSOLE_NAV_WORLD, bEnable);
}




void UIWaypointMap::Open(Player * ptPlayer, RActorID idRActor, Act eSelectedAct, BOOL bForTutorial)




{
    if (!ptPlayer)
    {
        return;
    }

    if (::Console::Screens::IsModalDialogVisible())
    {
        return;
    }

    if (!::Console::Screens::OpenToScreen(::Console::Screens::MAP_SCREEN_SET, 1, ptPlayer->nControllerIndex, FALSE))
    {
        return;
    }




    sgtWaypointMapGlobals.bTutorialWasInterrupted = FALSE;
    sgtWaypointMapGlobals.tTutorialState.bOpenForTutorial = bForTutorial;
    sgtWaypointMapGlobals.tTutorialState.eMapTutorialState = UIWaypointMap::MAP_TUTORIAL_STATE_INVALID;

    CPlayerSetLastUsedWaypointRActor(idRActor);

    if (bForTutorial)
    {
        sEnableTabNavigation(FALSE);


        TutorialAdvance(ptPlayer, sgtWaypointMapGlobals.tTutorialState);
    }
    else
    {

        sEnableTabNavigation(TRUE);
    }
}














void UIWaypointMap::OpenWaypointMap(int32 nControllerOpening)
{



    GameSoftPause(TRUE);

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAP_OPEN), cgtPSODefaults);
}







BOOL UIWaypointMap::CanCloseSafely()
{

    return !sgtWaypointMapGlobals.tTutorialState.bOpenForTutorial;
}









void UIWaypointMap::CloseWaypointMap(int32 nControllerIndex)
{







    if (sgtWaypointMapGlobals.tTutorialState.bOpenForTutorial)
    {

        SoundDelete(sgtWaypointMapGlobals.tTutorialState.idTutorialSound);
        sgtWaypointMapGlobals.tTutorialState.idTutorialSound = SOUNDID_INVALID;


        ShowTutorialText("");


        sgtWaypointMapGlobals.bTutorialWasInterrupted = TRUE;
    }


    sEnableTabNavigation(TRUE);

    sgtWaypointMapGlobals.nCurrentWaypointIndex = -1;
    sgtWaypointMapGlobals.nTargetWaypointIndex = -1;
    sgtWaypointMapGlobals.bTargetChanged = FALSE;
    sgtWaypointMapGlobals.tTutorialState.bOpenForTutorial = FALSE;

    GameSoftPause(FALSE);

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAP_CLOSE), cgtPSODefaults);


    CPlayerSetLastUsedWaypointRActor(RACTORID_INVALID);
}




void UIWaypointMap::Close()
{

    ::Console::Screens::CloseScreenSet(TRUE, FALSE);
}




static LPCSTR sGetTextureFrameForAct(Act eAct)
{

    switch (eAct)
    {
    case 0:
        return "WaypointMap_ButtonAct1Up";
    case 100:
        return "WaypointMap_ButtonAct2Up";
    case 200:
        return "WaypointMap_ButtonAct3Up";
    case 300:
        return "WaypointMap_ButtonAct4Up";
    case 400:
        return "WaypointMap_ButtonAct5Up";
    }

    return "";
}









static void sUpdateWorldMap()
{

    Player * ptPlayer = PlayerGetByControllerIndex(sgtWaypointMapGlobals.nControllerIndex);
    UIWaypointMap::UpdateWorldControls(ptPlayer, sgtWaypointMapGlobals.tTutorialState.bOpenForTutorial &&
                                                 sgtWaypointMapGlobals.tTutorialState.eMapTutorialState == UIWaypointMap::MAP_TUTORIAL_STATE_WORLD_VIEW_HIGHLIGHT_ACTS);

    Act eAct = -1;

    UIWindow & tParentWindow = UI_GetWindowFromID(UIWND_WAYPOINTMAP_WORLDMAP);
    const UIControlItemArray & arChildren = tParentWindow.GetChildList();
    for (int32 i = 0; i < (int32)arChildren.size(); i++)
    {
        const UIID & idWaypoint = arChildren[i].id;


        if (i == sgtWaypointMapGlobals.nTargetWaypointIndex)
        {
            eAct = (Act)(intptr)UI_GetWindowFromID(idWaypoint).GetUserData();
        }


        BOOL bIsSelected = (i == sgtWaypointMapGlobals.nTargetWaypointIndex);
        ButtonState eButtonState = bIsSelected ? BS_MOUSEOVER : BS_NORMAL;

        UIID idName = UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Name");
        UIID idPOI = UIIDCreateFromParent(idWaypoint, ".LayoutRoot.POI");
        UIID idTown = UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Town");
        UI_GetButtonFromID(idName).SetButtonState(eButtonState, FALSE, FALSE);
        UI_GetButtonFromID(idPOI).SetButtonState(eButtonState, FALSE, FALSE);
        UI_GetButtonFromID(idTown).SetButtonState(eButtonState, FALSE, FALSE);

        UIID idSelected = UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Selected");
        UI_GetWindowFromID(idSelected).SetActive(bIsSelected);
    }


    UI_GetWindowFromID(UIWND_WAYPOINTMAP_OVERLAY_WORLD).SetActive(GameIsOpenWorld() && !sgtWaypointMapGlobals.tTutorialState.bOpenForTutorial);
    if (GameIsOpenWorld() && eAct != -1)
    {
        ImageTextureFrame tTexFrame;
        ImageCollectionGetImageInfo(2, sGetTextureFrameForAct(eAct), tTexFrame);
        UI_GetButtonFromID(UIBTN_WAYPOINTMAP_OVERLAY_WORLD_ACT).SetAllTextureFrames(tTexFrame);


        CRefString sText = UIWaypointMap::GetBountyTooltipTextForAct(eAct);
        UI_GetTextFromID(UITXT_WAYPOINTMAP_OVERLAY_WORLD_TEXT).SetText(sText, FALSE);
    }
}




static void sUpdateActMap()
{

    Player * ptPlayer = PlayerGetByControllerIndex(sgtWaypointMapGlobals.nControllerIndex);

    UIWaypointMap::UpdateBountyOverlay(ptPlayer, sgtWaypointMapGlobals.eCurrentDisplayedAct,
                                       sgtWaypointMapGlobals.tTutorialState.bOpenForTutorial &&
                                       sgtWaypointMapGlobals.tTutorialState.eMapTutorialState == UIWaypointMap::MAP_TUTORIAL_STATE_ACT_VIEW_HIGHLIGHT_BOUNTY_REWARDS);

    UIWaypointMap::UpdateWaypointControls(ptPlayer, sgtWaypointMapGlobals.eCurrentDisplayedAct,
                                          sgtWaypointMapGlobals.tTutorialState.bOpenForTutorial &&
                                          (sgtWaypointMapGlobals.tTutorialState.eMapTutorialState == UIWaypointMap::MAP_TUTORIAL_STATE_ACT_VIEW_HIGHLIGHT_BOUNTIES || sgtWaypointMapGlobals.tTutorialState.eMapTutorialState == UIWaypointMap::MAP_TUTORIAL_STATE_ACT_VIEW_HIGHLIGHT_BOUNTIES_INDICATOR));



    SNO snoQuestWaypointLevelArea = UIWaypointMap::GetQuestWaypointLevelArea(ptPlayer);

    for (int32 i = 0; i < sgtWaypointMapGlobals.arMapTargets.size(); i++)
    {
        const UIID & idWaypoint = sgtWaypointMapGlobals.arMapTargets[i].id;


        UIWaypointMap::UpdateQuestWaypointCallout(idWaypoint, snoQuestWaypointLevelArea);


        BOOL bIsSelected = (i == sgtWaypointMapGlobals.nTargetWaypointIndex);
        ButtonState eButtonState = bIsSelected ? BS_MOUSEOVER : BS_NORMAL;

        UIID idName = UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Name");
        UIID idPOI = UIIDCreateFromParent(idWaypoint, ".LayoutRoot.POI");
        UIID idTown = UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Town");
        UI_GetButtonFromID(idName).SetButtonState(eButtonState, FALSE, FALSE);
        UI_GetButtonFromID(idPOI).SetButtonState(eButtonState, FALSE, FALSE);
        UI_GetButtonFromID(idTown).SetButtonState(eButtonState, FALSE, FALSE);

        UIID idSelected = UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Selected");
        UI_GetWindowFromID(idSelected).SetActive(bIsSelected);

        if (GameIsOpenWorld())
        {

            BOOL bShowFlyout = FALSE;
            BOOL bHasBounty = sWaypointHasBounty(i);
            BOOL bHasBoss = sWaypointHasBoss(i);
            BOOL bHasEventPOI = sWaypointHasEventPOI(i);

            BOOL bIsPlayerPresent = FALSE;
            const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
            for (const Player * ptPlayerScoped = PlayerGetFirstAll(); ptPlayerScoped; ptPlayerScoped = PlayerGetNextAll(ptPlayerScoped))
            {
                if (PlayerGetLastWaypointLevelArea(ptPlayerScoped) == tWaypoints[sgtWaypointMapGlobals.arMapTargets[i].nIndex].tInfo.snoLevelArea)
                {
                    bIsPlayerPresent = TRUE;
                    break;
                }
            }



            if (bIsSelected || bHasBounty || bHasEventPOI || bIsPlayerPresent)
            {
                UIButton & tWaypointButton = UI_GetButtonFromID(UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Name"));
                if (tWaypointButton.GetText().Length() == 0)
                {
                    CRefString sName = LevelAreaGetLocalizedName(tWaypoints[sgtWaypointMapGlobals.arMapTargets[i].nIndex].tInfo.snoLevelArea);
                    tWaypointButton.SetText(sName, FALSE);
                }
            }
            else
            {

                UI_GetButtonFromID(UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Name")).SetText("", FALSE);
            }

            if (sgtWaypointMapGlobals.eViewMode == VIEWMODE_BOUNTIES)
            {
                bShowFlyout = bHasBounty;
            }
            else if (sgtWaypointMapGlobals.eViewMode == VIEWMODE_BOSSES)
            {
                bShowFlyout = bHasBoss;
            }
            else
            {

                bShowFlyout = bIsSelected && (bHasBounty || bHasBoss || bHasEventPOI);
            }

            UI_GetWindowFromID(UIIDCreateFromParent(idWaypoint, ".LayoutRoot.FlyoutWrapper")).SetActive(bShowFlyout);


            if (bShowFlyout)
            {
                sUpdateTextFlyout(i);
            }




            BOOL bFadeOut = (sgtWaypointMapGlobals.eViewMode != VIEWMODE_NORMAL) && !bShowFlyout;
            UI_GetWindowFromID(idWaypoint).SetTransparency(bFadeOut ? 150 : 255);
        }
    }
}
static void sUpdateViewMode()
{

    const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);
    const ControllerState & tContollerState = tInputState.tControllerState[sgtWaypointMapGlobals.nControllerIndex];

    BOOL bLeftTriggerDown = tContollerState.tCurrentFrame.IsButtonDown(6);
    BOOL bRightTriggerDown = tContollerState.tCurrentFrame.IsButtonDown(7);

    ViewMode eOldViewMode = sgtWaypointMapGlobals.eViewMode;
    if (sgtWaypointMapGlobals.eViewMode == VIEWMODE_NORMAL)
    {
        if (bLeftTriggerDown)
        {
            sgtWaypointMapGlobals.eViewMode = VIEWMODE_BOUNTIES;
        }
        else if (bRightTriggerDown)
        {
            sgtWaypointMapGlobals.eViewMode = VIEWMODE_BOSSES;
        }
    }
    else if ((!bLeftTriggerDown && sgtWaypointMapGlobals.eViewMode == VIEWMODE_BOUNTIES) ||
             (sgtWaypointMapGlobals.eViewMode == VIEWMODE_BOSSES && !bRightTriggerDown))
    {
        sgtWaypointMapGlobals.eViewMode = VIEWMODE_NORMAL;
    }

    if (eOldViewMode != sgtWaypointMapGlobals.eViewMode)
    {
        for (int32 i = 0; i < sgtWaypointMapGlobals.arMapTargets.size(); i++)
        {
            if (sgtWaypointMapGlobals.eViewMode == VIEWMODE_BOUNTIES)
            {
                sUpdateBountyTextFlyout(i);
            }
            else if (sgtWaypointMapGlobals.eViewMode == VIEWMODE_BOSSES)
            {
                sUpdateBossTextFlyout(i);
            }
            else if (i == sgtWaypointMapGlobals.nTargetWaypointIndex)
            {
                sUpdateTextFlyout(i);
            }
        }
    }
}




static void sUpdateLeftStickInput()
{

    static const Vector2D VECTOR_X_AXIS(1.0f, 0.0f);
    static const float TARGET_ANGLE = 0.7f;
    static const Blizzard::Time::Millisecond TARGET_CHANGE_DELAY_MS = 300;

    if (::Console::ControllerUtils::LStickIsInDeadZone(sgtWaypointMapGlobals.nControllerIndex, FALSE))
    {
        sgtWaypointMapGlobals.bTargetChanged = FALSE;
        return;
    }

    if (Blizzard::Time::Milliseconds() - sgtWaypointMapGlobals.nTargetChangedTime < TARGET_CHANGE_DELAY_MS)
    {
        return;
    }

    if (sgtWaypointMapGlobals.nTargetWaypointIndex == -1)
    {
        return;
    }



    const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);
    Vector2D vecLeftStick = tInputState.tControllerState[sgtWaypointMapGlobals.nControllerIndex].tCurrentFrame.vecLeftStick;
    vecLeftStick.y = -vecLeftStick.y;
    Angle aLeftStickAngle = Vector2D::AngleBetween(vecLeftStick, VECTOR_X_AXIS);



    int32 nSelectedTarget = -1;
    float flDistanceToTarget = FLT_MAX;

    const Vector2D vecSourcePos = sgtWaypointMapGlobals.arMapTargets[sgtWaypointMapGlobals.nTargetWaypointIndex].vecPosition;
    for (int32 i = 0; i < sgtWaypointMapGlobals.arMapTargets.size(); i++)
    {
        if (i == sgtWaypointMapGlobals.nTargetWaypointIndex)
        {
            continue;
        }


        if (sgtWaypointMapGlobals.eViewMode == VIEWMODE_BOUNTIES && !sWaypointHasBounty(i))
        {
            continue;
        }


        if (sgtWaypointMapGlobals.eViewMode == VIEWMODE_BOSSES && !sWaypointHasBoss(i))
        {
            continue;
        }

        const Vector2D & vecTargetPos = sgtWaypointMapGlobals.arMapTargets[i].vecPosition;
        Vector2D vecDirection = vecTargetPos - vecSourcePos;
        Angle aTargetAngle = Vector2D::AngleBetween(vecDirection, VECTOR_X_AXIS);

        if (Math::AngularDifference(aLeftStickAngle, aTargetAngle) < TARGET_ANGLE)
        {
            float flDistance = Vector2D::DistanceSquared(vecSourcePos, vecTargetPos);
            if (nSelectedTarget == -1 || flDistance < flDistanceToTarget)
            {
                nSelectedTarget = i;
                flDistanceToTarget = flDistance;
            }
        }
    }



    if (nSelectedTarget != -1)
    {
        sSetTargetWaypoint(nSelectedTarget);
    }
}

















































static void sUpdateTutorialButtonLegend()
{

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (tActionManager.AreActionsDirty())
    {
        tActionManager.SetActionsDirtyFlag(FALSE);
        tActionManager.ClearAllActions();
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());

        ::Console::Screens::UpdateButtonLegendText();
    }
}



static void sUpdateWorldMapButtonLegend()
{

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (tActionManager.AreActionsDirty())
    {
        tActionManager.SetActionsDirtyFlag(FALSE);
        tActionManager.ClearAllActions();

        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT, StringListFetch("Console:Select").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());

        ::Console::Screens::UpdateButtonLegendText();
    }
}



static void sUpdateActMapButtonLegend()
{

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (tActionManager.AreActionsDirty())
    {
        tActionManager.SetActionsDirtyFlag(FALSE);
        tActionManager.ClearAllActions();

        if (sgtWaypointMapGlobals.nTargetWaypointIndex != sgtWaypointMapGlobals.nCurrentWaypointIndex)
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT, StringListFetch("ConsoleWaypoint:SelectWaypoint").str());
        }

        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());

        if (GameIsOpenWorld())
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SHOW_ALL_BOUNTIES, StringListFetch("ConsoleWaypoint:DisplayBounties").str());
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SHOW_ALL_BOSSES, StringListFetch("ConsoleWaypoint:DisplayBosses").str());
        }

        ::Console::Screens::UpdateButtonLegendText();
    }
}




static void sUpdateButtonLegend()
{

    if (UIWaypointMap::IsOpenForTutorial())
    {
        sUpdateTutorialButtonLegend();
    }


    else if (sIsWorldMapActive())
    {
        sUpdateWorldMapButtonLegend();
    }
    else
    {
        sUpdateActMapButtonLegend();
    }
}





void UIWaypointMap::Update()
{

    if (!IsOpen())
    {
        return;
    }

    if (IsOpenForTutorial())
    {
        if (GameIsSoftPaused())
        {

            GameSoftPause(FALSE);
        }

        if (sgtWaypointMapGlobals.tTutorialState.idTutorialSound != SOUNDID_INVALID &&

            !SoundTryToGet(sgtWaypointMapGlobals.tTutorialState.idTutorialSound))

        {

            const uint32 dwDelay = 60;
            if (sgtWaypointMapGlobals.tTutorialState.dwTickNextTutorialAdvance == 0xffffffff)
            {
                sgtWaypointMapGlobals.tTutorialState.dwTickNextTutorialAdvance = CGameGetClientTicks() + dwDelay;
            }
            else if (sgtWaypointMapGlobals.tTutorialState.dwTickNextTutorialAdvance < CGameGetClientTicks())
            {
                TutorialAdvance(PlayerGetByControllerIndex(sgtWaypointMapGlobals.nControllerIndex), sgtWaypointMapGlobals.tTutorialState);
                if (sgtWaypointMapGlobals.tTutorialState.eMapTutorialState == MAP_TUTORIAL_STATE_DONE)
                {
                    sEnableTabNavigation(TRUE);
                    GameSoftPause(TRUE);
                }
            }
        }
    }

    if (sIsWorldMapActive())
    {
        sUpdateWorldMap();
    }
    else
    {
        if (!IsOpenForTutorial())
        {
            sUpdateViewMode();
        }
        sUpdateActMap();
    }

    if (!IsOpenForTutorial())
    {
        sUpdateLeftStickInput();
    }

    sUpdateButtonLegend();
}




void UIWaypointMap::SetAct3HubEntryPointAct(Act eAct)
{

    if (sgtWaypointMapGlobals.eAct3HubEntryPointQueuedOverride != -1)
    {
        sgtWaypointMapGlobals.eAct3HubEntryPointAct = sgtWaypointMapGlobals.eAct3HubEntryPointQueuedOverride;
        sgtWaypointMapGlobals.eAct3HubEntryPointQueuedOverride = -1;
    }
    else
    {
        sgtWaypointMapGlobals.eAct3HubEntryPointAct = eAct;
    }
}




void UIWaypointMap::OpenActMap(int32 nControllerOpening)
{

    sgtWaypointMapGlobals.nControllerIndex = nControllerOpening;

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerOpening);
    if (ptPlayer)
    {
        Act eSelectedAct = GetWaypointActForPlayer(ptPlayer);
        sShowActMap(ptPlayer, eSelectedAct);

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_OPEN), cgtPSODefaults);
    }
}




void UIWaypointMap::CloseActMap()
{

    sgtWaypointMapGlobals.eViewMode = VIEWMODE_NORMAL;
    UI_GetWindowFromID(UIDLG_WAYPOINTMAP_PAGE).SetActive(FALSE);
}




void UIWaypointMap::OpenWorldMap(int32 nControllerOpening)
{

    sgtWaypointMapGlobals.nControllerIndex = nControllerOpening;

    UI_GetWindowFromID(UIDLG_WAYPOINTMAP_PAGE).SetActive(TRUE);

    sShowWorldMap();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_OPEN), cgtPSODefaults);
}




void UIWaypointMap::CloseWorldMap()
{

    UI_GetWindowFromID(UIDLG_WAYPOINTMAP_PAGE).SetActive(FALSE);
}




BOOL UIWaypointMap::TutorialWasInterrupted()
{

    return sgtWaypointMapGlobals.bTutorialWasInterrupted;
}



void UIWaypointMap::CancelTutorialWasInterrupted()
{

    sgtWaypointMapGlobals.bTutorialWasInterrupted = FALSE;
}




void UIWaypointMap::SetupControlsForAct(Act eAct)
{

    Player * ptPlayer = PlayerGetByControllerIndex(sgtWaypointMapGlobals.nControllerIndex);
    sShowActMap(ptPlayer, eAct);
}





void UIWaypointMap::SetupControlsForWorldView()
{

    sShowWorldMap();
}




void UIWaypointMap::GetBountyCountForAct(Act eAct, int32 & nBountyCount, int32 & nCompletedBountyCount)
{

    CountBountiesAtWaypoints(eAct, nBountyCount, nCompletedBountyCount);
}






void UIWaypointMap::OnControllerHotkey(const UIID & id, int32 nControllerIndex)


{


    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (!ptPlayer)
    {
        return;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer && !ptACDPlayer->IsAlive())
    {
        return;
    }

    if (UIBossEncounter::IsActive())
    {
        return;
    }

    if (!::Console::Screens::IsScreenSetOpen())
    {
        ::Console::Screens::OpenToScreen(::Console::Screens::MAP_SCREEN_SET, 0, nControllerIndex, FALSE);
    }
}







void UIWaypointMap::OnButtonAccept(const UIID & id, const int32 nControllerIndex)


{


    if (!IsOpen() || IsOpenForTutorial())
    {
        return;
    }

    if (sgtWaypointMapGlobals.nTargetWaypointIndex == -1)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    int32 nIndex = sgtWaypointMapGlobals.arMapTargets[sgtWaypointMapGlobals.nTargetWaypointIndex].nIndex;

    if (sIsWorldMapActive())
    {

        ::Console::Screens::OpenToScreen(::Console::Screens::MAP_SCREEN_SET, 1, nControllerIndex, TRUE);


        Act eAct = (Act)nIndex;
        if (sgtWaypointMapGlobals.eCurrentDisplayedAct != eAct)
        {
            sShowActMap(ptPlayer, eAct);
        }
    }
    else
    {
        if (sgtWaypointMapGlobals.nTargetWaypointIndex != sgtWaypointMapGlobals.nCurrentWaypointIndex)
        {



            if (ActGetLevelAreaByWaypointIndex(nIndex) == GlobalSNOGet(GLOBALSNO_LEVELAREA_ACT_3_HUB))
            {
                sgtWaypointMapGlobals.eAct3HubEntryPointAct = sgtWaypointMapGlobals.eCurrentDisplayedAct;
            }
            else
            {
                sgtWaypointMapGlobals.eAct3HubEntryPointAct = -1;
            }
            sgtWaypointMapGlobals.eAct3HubEntryPointQueuedOverride = sgtWaypointMapGlobals.eAct3HubEntryPointAct;

            ActivateWaypoint(ptPlayer, nIndex);
        }

        Close();
    }
}






void UIWaypointMap::OnButtonClose(const UIID & id, const int32 nControllerIndex)


{



    if (IsOpen())
    {



        Close();


        sgtWaypointMapGlobals.bTutorialWasInterrupted = FALSE;
    }
}



void UIWaypointMap::ShowTutorialText(const CRefString & sText)
{

    UI_GetWindowFromID(UIWND_WAYPOINTMAP_TUTORIAL_TEXT_WRAPPER).SetActive(sText.Length() > 0);
    UI_GetTextFromID(UITEXT_WAYPOINTMAP_TUTORIAL).SetText(sText, FALSE);
}



void UIWaypointMap::SetZKConversationComplete()
{
    sgtWaypointMapGlobals.bZKConversationComplete = TRUE;
}
