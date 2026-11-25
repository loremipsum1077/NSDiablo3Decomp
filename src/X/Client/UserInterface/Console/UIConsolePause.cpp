// Original file: X/Client/UserInterface/Console/UIConsolePause.cpp
//
// The in-game console pause menu: the option list, the hero model, the quest details and
// the party/friends/local-communication side panels.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../Console/Online.h"
#include "../../Console/LobbyService.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/ControllerUtils.h"
#include "ConsoleInputUtilities.h"
#include "ConsoleScreenManager.h"
#include "ConsoleModalNotificationManager.h"
#include "ControllerActions.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIUtils.h"
#include "../UIIDs.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Common/OnlineService/Callback/Callback.h"
#include "../../../Sigma/RefString.h"
#include "../../../Common/Game.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Player.h"
#include "../../CPlayer.h"
#include "../../CGame.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UITextControl.h"
#include "UIConsolePartyDisplay.h"
#include "../../RActors/RActors.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../Sound/Sound.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Common/Definitions/AttachmentDefinition.h"
#include "../../../Common/Protobuf/Account.pb.h"
#include "../../../Common/Protobuf/Hero.pb.h"

// Declared here because their headers are not adopted yet; each comment names the defining file.
const CmdLineParams * CmdLineGetParams();  // X/Common/Shell/CmdLine.cpp
CRefString StringListFetch(LPCSTR szLabel);  // X/Common/Definitions/StringListDefinition.cpp
namespace Console { namespace Online { BOOL PartyIsInGame(); void PartyForceMigration(); void RefreshAdHocList(); } }  // X/Client/Console/Online.cpp
namespace UIPartyDisplay { namespace Console { void ShowFriendDisplay(BOOL bShow); void HideFriendDisplay(); void ShowLocalCommunicationsDisplay(); void HideLocalCommunicationsDisplay(); BOOL CanDisplayLocalCommunicationDisplay(); } }  // UIConsolePartyDisplay.cpp
namespace UIGamePrivacyOptions { namespace Console { void ShowGamePrivacyOptions(int32 nControllerIndex); } }  // UIConsoleGamePrivacyOptions.cpp
const UIID & UI_GetControllerFocus();  // UI.cpp:674
void UI_SetControllerFocus(const UIID & id);  // UI.cpp
UIRActor & UI_GetRActorFromID(const UIID & id);  // UI.cpp
namespace Console { namespace GamerProfile { BOOL IsSignedInOnline(int32 nUserIdx); } }  // GamerProfileNX64.cpp
extern UIID UILIST_PAUSE_CONSOLE;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIRACTOR_PAUSE_CONSOLE_HIGHLIGHT;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UITEXT_CONSOLE_FRIENDS_NO_FRIENDS;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIWND_PAUSE_CONSOLE;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UITXT_CONSOLE_PLAYER_LABEL_CENTERED;  // X/Client/UserInterface/UIIDs.cpp
namespace UINetworkOptions { namespace Console { void CloseNetworkOptions(); } }  // UIConsoleNetworkOptions.cpp
namespace UIGamePrivacyOptions { namespace Console { void CloseGamePrivacyOptions(); } }  // UIConsoleGamePrivacyOptions.cpp
namespace UIAchievements { namespace Console { void Close(); } }  // UIConsoleAchievements.cpp
namespace UIBannerCustomization { namespace Console { void Close(); } }  // UIConsoleBannerCustomization.cpp
namespace UISeasonJourney { namespace Console { void Close(); } }  // UIConsoleSeasonJourney.cpp
namespace UILeaderboards { namespace Console { void Close(); } }  // UIConsoleLeaderboards.cpp
namespace UIProfileSummary { namespace Console { void Close(); } }  // UIConsoleProfileSummary.cpp
void UIDeathMenu_Console_Init(int32 nControllerIndex);  // UIConsoleDeathMenu.cpp
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);  // X/Client/Light.cpp
SNO UI_GetDefaultHeroLight();  // X/Client/UserInterface/UI.cpp
SNO GetFrontendCharacterSNO(int32 ePlayerClass, int32 bIsFemale);  // X/Client/Battlenet/UI
RActorID UI_CreateHeroUIRActor(SNO snoActor, SNO snoIrradianceTex, const VisualEquipment & tVisualEquipment, int32 nUnused, int32 ePlayerClass, int32 bIsFemale, const PRTransform & transform, RenderContext eContext, uint32 dwFlags, float flScale, int32 nUnused2);  // X/Client/UserInterface/UI.cpp
void RActorAttachExistingRActor(RActorID idRActor, RActorID idRActorAttachment, HardpointHash tHardpoint);  // X/Client/RActors/RActors.cpp
namespace Console { namespace UIOnlineActions { bool IsActiveSeason(uint32 uSeason, const D3::Account::Digest & tAccountDigest); } }  // OnlineActions.cpp
namespace UIConversation { void ConsoleCancel(const UIID & id, int32 nControllerIndex); }  // X/Client/UserInterface/Interface/UIConversations.cpp
void ClientSendOutgoingMessages();  // X/Client/CGame.cpp
namespace CHero { void Save(int32 nControllerIndex, uint32 uReason); }  // X/Client/CHero.cpp
namespace Console { namespace Online { void PartyLeave(); void PartyLeaveAndRemake(uint32 bOffline); } }  // X/Client/Console/Online.cpp
namespace UIWaypointMap { BOOL TutorialWasInterrupted(); void CancelTutorialWasInterrupted(); void Open(Player * ptPlayer, RActorID idWaypoint, int32 nWaypoint, BOOL bTutorial); }  // X/Client/UserInterface/Console/UIConsoleWaypointMap.cpp
namespace UIPause {
namespace Console {
enum ButtonActions {
    ACTION_CLOSE = 0,
    ACTION_SELECT = 1,
    ACTION_NETWORK_OPTIONS = 2,
    ACTION_VIEW_LEFT_SOCIAL = 3,
    ACTION_VIEW_RIGHT_SOCIAL = 4,
    ACTION_REFRESH_ADHOC_LIST = 5,
};




enum PauseOptionID {
    RESUME = 0,
    OPTIONS = 1,
    NETWORK_SETTINGS = 2,
    DIFFICULTY = 3,
    ACHIEVEMENTS = 4,
    LEADERBOARDS = 5,
    SEASON_JOURNEY = 6,
    BANNER = 7,
    HELP = 8,
    CONNECT_DEMONWARE = 9,
    QUIT = 10,
    PAUSEOPTIONS_COUNT = 11,
};







struct PauseOption {
    PauseOptionID eId;
    CRefString sDisplayText;
    BOOL bShowLeaf;
};


class PauseOptionsListDataSource : public ConsoleListBoxDataSource {
public:
    PauseOptionsListDataSource();
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
    virtual BOOL IsItemEnabled(const UIConsoleListBox & tListBox, const int32 nIndex);
    void UpdateOptionsAvailability();

    blz::vector<PauseOption> m_menuItems;
};
}
}
#include "../../../Sigma/UnicodeText.h"
#include "../../../Common/NephalemRiftCommon.h"
#include "ConsoleUIRActorManager.h"
#include "../Interface/UITutorials.h"
#include "UIConsoleGameSettings.h"
#include "../Interface/UIConfirmation.h"
// Original external declarations, recovered from the corresponding DWARF source owners.
CRefString DisplayActName(Act eAct); CRefString DisplayFormattedHandicap(int32 nHandicap); CRefString QuestsGetUIName(SNO snoQuest);
CRefString LocalizedSprintf(LPCSTR szFormat, ...); CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
int32 ACDGetTieredLootRunLevel(const ActorCommonData & tACD);
namespace UIUtils { namespace Console { CRefString GetTagName(OnlineService::GameTagId idTag); CRefString PlayerGetDecoratedPlayerNum(const int32 nControllerIndex); CRefString DisplayPartyOnlineStateIcon(::Console::Online::PartyOnlineState eState, BOOL bEnabled, float flScale); }
}
extern UIID UITXT_PAUSE_QUEST;
namespace UIPause { namespace Console {




enum UISocialDisplayWindow {
    PARTY_WINDOW = 0,
    FRIENDS_WINDOW = 1,
    ADHOC_WINDOW = 2,
};



class PauseMenuGlobals {
public:
    PauseOptionsListDataSource listDataSource;
    ::Console::AnalogStickInputFilter tStickFilter;
    BOOL bReactivateDeathMenu;
    int32 nOpeningControllerIndex;
    UISocialDisplayWindow eSocialWindowActive;
    BOOL bIsShowingConnectOption;









    PauseMenuGlobals()
    {
        tStickFilter = ::Console::GetDefaultStickInputFilter(::Console::FILTER_HORIZONTAL);
        tStickFilter.SetMinimumInputDelay(250);
        bReactivateDeathMenu = FALSE;
        nOpeningControllerIndex = 0;
        eSocialWindowActive = PARTY_WINDOW;
        bIsShowingConnectOption = FALSE;
    }
};
}
}
namespace UIPartyDisplay { namespace Console { void RefreshFriendsDisplay(); void RefreshLocalCommunicationsDisplay(); BOOL ShouldDirectFriendsForInvite(); void OnPartyListAcceptPressed(int32 nControllerIndex); void OnFriendsListAcceptPressed(int32 nControllerIndex); void OnLocalCommListAcceptPressed(int32 nControllerIndex); }
}
namespace UIPause { namespace Console {


PauseMenuGlobals * sgptPauseMenuGlobals;

inline PauseOptionsListDataSource::PauseOptionsListDataSource() {}





int32 PauseOptionsListDataSource::GetNumItems() const
{
    return m_menuItems.size();
}


UIControlSourceKey PauseOptionsListDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    const LPCSTR LARGE_ITEM_TEMPLATE = "ConsolePause:ConsolePause_main.LayoutRoot.List.TextItem";
    const UIControlSourceKey LARGE_ITEM_SOURCE_KEY = UIControlSourceKey::Parse(LARGE_ITEM_TEMPLATE, -1, NULL, NULL);
    return LARGE_ITEM_SOURCE_KEY;
}




void PauseOptionsListDataSource::InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32)
{
}




void PauseOptionsListDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    UIID idText;


    BOOL bSelected = tListBox.GetSelectedIndex() == nIndex;

    idText.SetFromParent(idItemUI, "Text");
    UIText & tText = UI_GetTextFromID(idText);
    tText.SetText(m_menuItems[nIndex].sDisplayText, FALSE);
    if (m_menuItems[nIndex].eId == QUIT && UI_GetPrimaryUIController() != ::Console::GamerProfile::GetPrimaryProfileUserIndex())
    {
        tText.SetText(StringListFetch("ConsolePause:DropOut"), FALSE);
    }

    RGBAColor rgbaTextColor;
    if (!IsItemEnabled(tListBox, nIndex))
    {
        rgbaTextColor = crgbaGray;
    }
    else if (bSelected)
    {
        rgbaTextColor = crgbaWhite;

        if (m_menuItems[nIndex].eId == QUIT)
        {
            CRefString sLeaveGameString;
            if (UI_GetPrimaryUIController() == ::Console::GamerProfile::GetPrimaryProfileUserIndex())
            {
                sLeaveGameString = StringListFetch("Bnet_EscapeMenu:LeaveGame");
            }
            else
            {
                sLeaveGameString = StringListFetch("Console:LeaveGameLocalPlayer");
            }
            ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT, sLeaveGameString);
        }
        else
        {
            ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT, StringListFetch("Console:Accept").str());
        }
        ::Console::Screens::UpdateButtonLegendText();
    }
    else
    {
        rgbaTextColor = crgbaGold;
    }
    tText.SetTextColor(rgbaTextColor);

    if (bSelected)
    {
        UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_PAUSE_CONSOLE_HIGHLIGHT);
        Vector2D vecFinalTL;
        Rect2D rectFilagree = tFilagree.GetWindowRect();


        UIWindow & tItem = UI_GetWindowFromID(idItemUI);
        Rect2D rectItem = tItem.GetWindowRect();

        AlignRect(Vector2D(rectItem.left, rectItem.top),
                  Vector2D(rectItem.Width(), rectItem.Height()),
                  ALIGN_CENTER_VERTICAL,
                  rectFilagree.left,
                  Vector2D(rectFilagree.Width(), rectFilagree.Height()), vecFinalTL);

        tFilagree.MoveTo(vecFinalTL);
    }

    UIID idLeaf = UIIDCreateFromParent(idItemUI, ".SeasonalIcon");
    UI_GetWindowFromID(idLeaf).SetActive(m_menuItems[nIndex].bShowLeaf);
}
void PauseOptionsListDataSource::UpdateOptionsAvailability()
{
    m_menuItems.clear();


    PauseOption pauseOption;
    pauseOption.eId = RESUME;
    pauseOption.sDisplayText = StringListFetch("ConsolePause:Resume");
    pauseOption.bShowLeaf = FALSE;
    m_menuItems.push_back(pauseOption);

    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {

        pauseOption.eId = NETWORK_SETTINGS;
        pauseOption.sDisplayText = StringListFetch("Console:NetworkOptions");
        pauseOption.bShowLeaf = FALSE;
        m_menuItems.push_back(pauseOption);


        pauseOption.eId = OPTIONS;
        pauseOption.sDisplayText = StringListFetch("ConsolePause:Options");
        pauseOption.bShowLeaf = FALSE;
        m_menuItems.push_back(pauseOption);

        pauseOption.eId = DIFFICULTY;
        pauseOption.sDisplayText = StringListFetch("ConsolePause:ChangeDifficulty");
        pauseOption.bShowLeaf = FALSE;
        m_menuItems.push_back(pauseOption);

        pauseOption.eId = ACHIEVEMENTS;
        pauseOption.sDisplayText = StringListFetch("Console:Challenges");
        pauseOption.bShowLeaf = FALSE;
        m_menuItems.push_back(pauseOption);

        if (GameIsSeasonal() || GameIsWeeklyChallenge())
        {
            pauseOption.eId = LEADERBOARDS;
            pauseOption.sDisplayText = StringListFetch("Console:Leaderboards");
            pauseOption.bShowLeaf = TRUE;
            m_menuItems.push_back(pauseOption);
        }
        if (GameIsSeasonal())
        {
            pauseOption.eId = SEASON_JOURNEY;
            pauseOption.sDisplayText = StringListFetch("ConsoleSeasonJourney:Title");
            pauseOption.bShowLeaf = TRUE;
            m_menuItems.push_back(pauseOption);
        }
    }





















    if (::Console::Online::LobbyService::ShouldShowReconnectOption(sgptPauseMenuGlobals->nOpeningControllerIndex))
    {
        pauseOption.eId = CONNECT_DEMONWARE;
        pauseOption.sDisplayText = StringListFetch("Console:ConnectToDemonwareSwitch");
        pauseOption.bShowLeaf = FALSE;
        sgptPauseMenuGlobals->bIsShowingConnectOption = TRUE;
        m_menuItems.push_back(pauseOption);
    }
    else
    {
        sgptPauseMenuGlobals->bIsShowingConnectOption = FALSE;
    }


    pauseOption.eId = QUIT;
    pauseOption.bShowLeaf = FALSE;
    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        pauseOption.sDisplayText = StringListFetch("ConsolePause:Quit");
    }
    else
    {
        pauseOption.sDisplayText = StringListFetch("ConsolePause:QuitDemo");
    }

    m_menuItems.push_back(pauseOption);
}

// NON_MATCHING: four instructions differ in the option checks.
BOOL PauseOptionsListDataSource::IsItemEnabled(const UIConsoleListBox & tListBox, const int32 nIndex)
{


    if (nIndex < 0 || nIndex >= m_menuItems.size())
        return FALSE;





    Player * ptPlayer = PlayerGetByControllerIndex(sgptPauseMenuGlobals->nOpeningControllerIndex);


    BOOL bIsPrimaryHost = ::Console::Online::PartyIsHost() &&
        sgptPauseMenuGlobals->nOpeningControllerIndex == ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    if (ptPlayer)
    {
        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        if (!tACDPlayer.IsAlive())
        {
            switch (m_menuItems[nIndex].eId)
            {
            case OPTIONS:
            case DIFFICULTY:
            case ACHIEVEMENTS:
            case BANNER:
                return FALSE;
            case NETWORK_SETTINGS:
                return bIsPrimaryHost;
            case LEADERBOARDS:
                return ::Console::GamerProfile::IsSignedInOnline(::Console::GamerProfile::GetPrimaryProfileUserIndex());
            default:
                break;
            }
        }
        else
        {
            switch (m_menuItems[nIndex].eId)
            {
            case DIFFICULTY:
            {
                BOOL bIsHardcore = ptPlayer->tHeroStateData.uPlayerFlags & 1;
                return bIsPrimaryHost && !bIsHardcore;
            }
            case NETWORK_SETTINGS:
                return bIsPrimaryHost;
            case LEADERBOARDS:
                return ::Console::GamerProfile::IsSignedInOnline(::Console::GamerProfile::GetPrimaryProfileUserIndex());
            default:
                break;
            }
        }
    }























    return TRUE;
}
void LoadHeroModel(UIID & idRActorWindow)
{

    int32 nControllerIndex = sgptPauseMenuGlobals->nOpeningControllerIndex;

    UIRActor * tHeroRActor = &UI_GetRActorFromID(idRActorWindow);
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    VisualInventory * ptVisualInventory = VisualInventoryGet(ptPlayer->idPlayerACD);
    PlayerClass ePlayerClass = PlayerGetPlayerClass(ptPlayer->idPlayerACD);
    BOOL bIsFemale = !PlayerIsMale(*ptPlayer);

    tHeroRActor->DeleteRActor();

    PRTransform transform(cgqIdentity, cgwpNull);

    SNO snoActor = GetFrontendCharacterSNO(ePlayerClass, bIsFemale);
    RActorID idRActorNew = UI_CreateHeroUIRActor(snoActor,
                                                 GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE),
                                                 ptVisualInventory->tEquipmentCurrent,
                                                 0,
                                                 ePlayerClass,
                                                 bIsFemale,
                                                 transform,
                                                 tHeroRActor->tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(),
                                                 0x68700, 1.0f, 0);
    tHeroRActor->SetRActor(idRActorNew);


    LightID idLight = LightAdd(0,
                               UI_GetDefaultHeroLight(),
                               PRTransform(cgqIdentity, cgwpNull),
                               FALSE,
                               tHeroRActor->tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
    tHeroRActor->SetLight(idLight);


    RActor * ptRActor = RActorGet(tHeroRActor->GetRActor());




    D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
    D3::Hero::SavedDefinition & tHeroDefinition = ::Console::GamerProfile::GetHero(ptPlayer->nControllerIndex);
    D3::Hero::Digest * tHeroDigest = tHeroDefinition.mutable_digest();

    BOOL bSeasonal = ::Console::UIOnlineActions::IsActiveSeason(tHeroDigest->season_created(), tAccountDefinition.digest());
    BOOL bHardcore = ptPlayer->tHeroStateData.uPlayerFlags & 1;
    BOOL bSolo = bSeasonal && (tHeroDigest->player_flags2() & 1);
    GlobalSNO gsnoWaypointActor;
    if (GameIsWeeklyChallenge())
    {
        gsnoWaypointActor = GLOBALSNO_ACTOR_FRONTEND_HERO_WAYPOINT_CHALLENGE;
    }
    else if (bHardcore && !bSeasonal)
    {
        gsnoWaypointActor = GLOBALSNO_ACTOR_CONSOLE_BNET_HERO_WAYPOINT_HC;
    }
    else if (!bHardcore && bSeasonal)
    {
        gsnoWaypointActor = bSolo ? GLOBALSNO_UNK_922 : GLOBALSNO_ACTOR_CONSOLE_BNET_HERO_WAYPOINT_SEASONAL;
    }
    else if (bSeasonal && bHardcore)
    {
        gsnoWaypointActor = bSolo ? GLOBALSNO_UNK_924 : GLOBALSNO_ACTOR_CONSOLE_BNET_HERO_WAYPOINT_SEASONAL_HC;
    }
    else
    {
        gsnoWaypointActor = GLOBALSNO_ACTOR_CONSOLE_BNET_HERO_WAYPOINT;
    }

    RActorID idWaypoint = RActorCreate(GlobalSNOGet(gsnoWaypointActor), 1.0f, -1.0f, PRTransform(cgqIdentity, cgwpNull), 0, ptRActor->eContext, -1);






    RActorAttachExistingRActor(ptRActor->id, idWaypoint, HardpointHash("Default"));
}






void RefreshHighlightRActor()
{
    if (UI_GetControllerFocus() == UILIST_PAUSE_CONSOLE)
    {
        UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_PAUSE_CONSOLE_HIGHLIGHT);
        PRTransform transform(cgqIdentity, cgwpNull);
        if (tFilagree.GetRActor() == RACTORID_INVALID)
        {
            RActorID idRActor = RActorCreate(GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_MENU_FILAGREE), 1.0f, -1.0f, transform, 0, tFilagree.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), -1);
            tFilagree.SetRActor(idRActor);
        }
    }
}









inline void ClearHighlightRActor()
{
    UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_PAUSE_CONSOLE_HIGHLIGHT);
    tFilagree.Cleanup();
}




void RefreshQuestDetails()
{

    ::Console::Online::GameParams tGameParams;
    if (!::Console::Online::PartyGetGameParams(tGameParams)) return;




    Act currentSelectedAct = GameGetCurrentAct();
    CRefString sAct = DisplayActName(currentSelectedAct);
    SNO currentSelectedQuest = tGameParams.m_snoQuest;
    CRefString sQuest;
    if (currentSelectedAct == -1)
    {
        sQuest = StringListFetch("ConsoleQuestSelect:AnyQuest");
    }
    else if (currentSelectedQuest == SNO_NONE)
    {
        sQuest = StringListFetch("ConsoleQuestSelect:CurrentAct");
    }
    else
    {
        sQuest = QuestsGetUIName(currentSelectedQuest);
    }

    CRefString szDifficulty = DisplayFormattedHandicap(GameGetHandicapLevel());
    CRefString sFinalString;
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        sFinalString = LocalizedSprintf(sAct.str(), NULL);
    }
    else
    {
        sFinalString = LocalizedSprintf(StringListFetch("Bnet_CallToArms:GameDetails").str(), sAct.str(), szDifficulty.str(), NULL);
    }






    Player * ptPlayer = PlayerGetByControllerIndex(sgptPauseMenuGlobals->nOpeningControllerIndex);
    if (ptPlayer)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer && ACDIsInTieredLootRun(*ptACDPlayer))
        {
            CRefString sVal = UnicodeText::FormatNumber(ACDGetTieredLootRunLevel(*ptACDPlayer) + 1, TRUE, FNS_NORMAL, XLOCALE_INVALID);
            sQuest = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_LEVEL_AREAS), "TieredRiftDifficulty"), sVal.str(), NULL);
        }
    }


    CRefString sQuestInfoString;
    if (::Console::Online::PartyIsInParty() && ::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ONLINE_PUBLIC)
    {

        CRefString sTagName = UIUtils::Console::GetTagName(tGameParams.m_nTag);
        sQuestInfoString.Format("%s\n%s\n{c:FF91641E}%s"
                               "{/c}", sFinalString.str(), sQuest.str(), sTagName.str());
    } else
    {
        sQuestInfoString.Format("%s\n%s", sFinalString.str(), sQuest.str());
    }
    if (GameIsWeeklyChallenge())
    {

        uint32 uChallengeNumber = tGameParams.m_tWeeklyChallengeConfig.challenge_number();
        // The original calls the signed-int FormatNumber overload for this uint32 challenge number.
        CRefString szChallengeNumber = LocalizedSprintf(StringListFetch("ConsoleChallengeRifts:ChallengeNumber"), UnicodeText::FormatNumber(int32(uChallengeNumber), TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);

        sQuestInfoString.Format("%s\n%s", StringListFetch("ConsoleChallengeRifts:ChallengeRift").str(), szChallengeNumber.str());
    }
    UI_GetTextFromID(UITXT_PAUSE_QUEST).SetText(sQuestInfoString, FALSE);
}








}
}
namespace UIAchievements { namespace Console { BOOL IsVisible(); void OpenForController(int32 nControllerIndex); }
}
namespace UILeaderboards { namespace Console { BOOL IsVisible(); void OpenForController(int32 nControllerIndex); }
}
namespace UISeasonJourney { namespace Console { BOOL IsActive(); void OpenForController(int32 nControllerIndex); }
}
namespace UIBannerCustomization { namespace Console { BOOL IsVisible(); void OpenForController(int32 nControllerIndex); }
}
namespace UIProfileSummary { namespace Console { BOOL IsVisible(); }
}
namespace UIFriendsList { namespace Console { BOOL IsActive(); void Close(); }
}
namespace UITrade { namespace Console { void ForceCancel(); }
}
namespace Tutorial { BOOL PlayerHasSeen(const Player & tPlayer, const SNO snoTutorial); }
extern UITutorialManager gtTutorialMgr;
BOOL UIDeathMenu_Console_IsADeathScreenActive(); UIIDList & UI_GetInGameDlgStack(); void UI_PopDialog(UIIDList & listMenuStack, const BOOL fActivatePrev);
extern UIID unk_71018606a8, unk_71018606b0;
extern "C" BOOL sub_710072e0b0(uint32 uSeasonCreated, uint32 uFlags);
extern UIID UIRACTOR_CONSOLE_CENTERED, UIWND_CONSOLE_UIRACTOR_ROOT, UITXT_PAUSE_TITLE, UIWND_PAUSE_ARROW_UP, UIWND_PAUSE_ARROW_DOWN, UIWND_CONSOLE_PAUSE_TITLE_HARDCORE, UIWND_CONSOLE_PAUSE_TITLE_SEASONAL, UIWND_CONSOLE_PAUSE_TITLE_SEASONAL_HARDCORE;
extern UIID UIWND_CONSOLE_PAUSE_ONLINE_STATE, UITEXT_CONSOLE_PAUSE_ONLINE_STATE_MESSAGE, UIWND_CONSOLE_PAUSE_ONLINE_STATE_OFFLINE_ICON, UIWND_CONSOLE_PAUSE_ONLINE_STATE_ONLINE_ICON;
namespace Console { namespace GamerProfile { void EnsureNetworkServiceAccountForUser(int32 nUserIndex); }
namespace Online { void sForceSubmitNetworkRequest(); PartyOnlineState PartyGetPreferredOnlineState(); }
}
namespace UINetworkOptions { namespace Console { void ShowNetworkOptions(int32 nControllerIndex); }
}
namespace UIPause { namespace Console {

void ChangeFocusToDisplay(UISocialDisplayWindow eNewWindowToFocus)
{
    switch (eNewWindowToFocus)
    {
    case PARTY_WINDOW:
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).SetSelectedIndex(-1);
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST).SetSelectedIndex(-1);
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_PARTY).SetSelectedIndex(0);
        UI_SetControllerFocus(UILIST_CONSOLE_MENU_PARTY);
        break;
    case FRIENDS_WINDOW:
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_PARTY).SetSelectedIndex(-1);
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST).SetSelectedIndex(-1);
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).SetSelectedIndex(0);
        UI_SetControllerFocus(UILIST_CONSOLE_FRIENDS_LIST);
        break;
    case ADHOC_WINDOW:
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_PARTY).SetSelectedIndex(-1);
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).SetSelectedIndex(-1);
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST).SetSelectedIndex(0);
        UI_SetControllerFocus(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST);
        break;
    }
    RefreshHighlightRActor();
}






inline void OnLeavePartyAccept(int nControllerIndex, const UIID & id)
{
    ::Console::Online::PartyForceMigration();
}



void SwitchSocialWindow(UISocialDisplayWindow eNewWindowToSwitch, BOOL bInitializing, int32 nControllerIndex)
{
    BOOL bSoloGame = FALSE;
    if (::Console::Online::PartyIsInGame())
    {
        ::Console::Online::GameParams tParams;
        ::Console::Online::PartyGetGameParams(tParams);
        bSoloGame = tParams.unk_14 && tParams.m_eGameType != GT_CHALLENGE_RIFTS;
    }
    if (bSoloGame)
    {
        UIPartyDisplay::Console::HidePartyDisplay();
        UIPartyDisplay::Console::HideFriendDisplay();
        UIPartyDisplay::Console::HideLocalCommunicationsDisplay();
        return;
    }
    if (bInitializing || sgptPauseMenuGlobals->eSocialWindowActive != eNewWindowToSwitch)
    {
        switch (eNewWindowToSwitch)
        {
        case PARTY_WINDOW:
            UIPartyDisplay::Console::ShowPartyDisplay();
            UIPartyDisplay::Console::HideFriendDisplay();
            UIPartyDisplay::Console::HideLocalCommunicationsDisplay();
            break;
        case FRIENDS_WINDOW:
            UIPartyDisplay::Console::HidePartyDisplay();
            UIPartyDisplay::Console::ShowFriendDisplay(TRUE);
            UIPartyDisplay::Console::HideLocalCommunicationsDisplay();
            break;
        case ADHOC_WINDOW:
            if (UIPartyDisplay::Console::CanDisplayLocalCommunicationDisplay())
            {
                ::Console::Online::RefreshAdHocList();
                UIPartyDisplay::Console::HidePartyDisplay();
                UIPartyDisplay::Console::HideFriendDisplay();
                UIPartyDisplay::Console::ShowLocalCommunicationsDisplay();
            }
            else
            {





                if (!::Console::Online::PartyIsHost())
                {
                    NotificationCreationParams tParams;
                    tParams.nControllerIndex = nControllerIndex;
                    tParams.uszMessage = StringListFetch("ConsoleErrors:MigrateIfSwitchingAdHocSessions");
                    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_1_1<void, int, const UIID &, &OnLeavePartyAccept>(nControllerIndex);

                    ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
                }
                return;
            }
            break;
        }

        if (!bInitializing && UI_GetControllerFocus() != UILIST_PAUSE_CONSOLE)
        {
            ChangeFocusToDisplay(eNewWindowToSwitch);
        }
    }

    sgptPauseMenuGlobals->eSocialWindowActive = eNewWindowToSwitch;
}

void OnEventPartyOnlineStateChanged(::Console::Online::PartySwitchOnlineResult eResult)
{
    if (::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ADHOC)
    {
        if (sgptPauseMenuGlobals && (sgptPauseMenuGlobals->eSocialWindowActive == FRIENDS_WINDOW || sgptPauseMenuGlobals->eSocialWindowActive == ADHOC_WINDOW))
        {
            SwitchSocialWindow(PARTY_WINDOW, FALSE, -1);
        }
    }
    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_PAUSE_CONSOLE);
    tList.RefreshVisibleItems();
}





void OnLobbyChangeConnectionEvent(int32 nUserIndex, ::Console::Online::LobbyConnectionEventType eStatus)
{
    UI_GetConsoleListBoxFromID(UILIST_PAUSE_CONSOLE).RefreshVisibleItems();
}






static void sHandleConnectToDemonware(const int32 nControllerIndex)
{
    if (::Console::GamerProfile::IsActive(nControllerIndex) && !::Console::GamerProfile::IsGuest(nControllerIndex))
    {
        if (!::Console::Online::IsPhysicalNetworkConnected())
            ::Console::Online::sForceSubmitNetworkRequest();
        else if (!::Console::GamerProfile::IsSignedInOnline(nControllerIndex))
            ::Console::GamerProfile::EnsureNetworkServiceAccountForUser(nControllerIndex);
        else
            ::Console::Online::LobbyService::AttemptConnectOnUser(nControllerIndex);
    }
}










void OpenForController(int32 nControllerIndex)
{
    sgptPauseMenuGlobals->nOpeningControllerIndex = nControllerIndex;
    sgptPauseMenuGlobals->listDataSource.UpdateOptionsAvailability();
    ::Console::Screens::GetActionManager().ClearAllActions();









    if (UIFriendsList::Console::IsActive())
    {
        UIFriendsList::Console::Close();
    }

    UITrade::Console::ForceCancel();

    ::Console::Screens::UpdateButtonLegendText();
    ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_PAUSE_CONSOLE, TRUE, UIWND_CONSOLE_UIRACTOR_ROOT);
    UIText & tText = UI_GetTextFromID(UITXT_PAUSE_TITLE);
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        GamePause(TRUE);
        tText.SetText(StringListFetch("ConsoleDemo:DemoTitle").str(), FALSE);
    }
    else if (GameIsWeeklyChallenge() && NephalemRift::IsTieredRiftActive())
        tText.SetText(StringListFetch("ConsolePause:WeeklyChallengeWarning").str(), FALSE);
    else if (PlayersAreAllLocal())
    {
        GamePause(TRUE);
        tText.SetText(StringListFetch("ConsolePause:Title").str(), FALSE);
    }
    else
        tText.SetText(StringListFetch("ConsolePause:TitleMultisystem").str(), FALSE);





    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_PAUSE_CONSOLE);
    tList.SetOnSelChangeFunction(NULL);
    tList.SetDataSource(sgptPauseMenuGlobals->listDataSource);
    tList.ResetAllItems();


    ::Console::Online::GetPartyOnlineStateChangedEvent().Subscribe(OnlineService::internal::function_bind_0_1<void, ::Console::Online::PartySwitchOnlineResult, &OnEventPartyOnlineStateChanged>());
    ::Console::Online::GetLobbyConnectionEvent().Subscribe(OnlineService::internal::function_bind_0_2<void, int32, ::Console::Online::LobbyConnectionEventType, &OnLobbyChangeConnectionEvent>());


    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {

        SwitchSocialWindow(PARTY_WINDOW, TRUE, -1);
    }
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_PARTY).SetSelectedIndex(-1);
    UI_SetControllerFocus(UILIST_PAUSE_CONSOLE);
    RefreshHighlightRActor();










    BOOL bShouldCreateRActor = ::Console::UIRActorManager::ShouldCreateRActor(::Console::UIRActorManager::RACTOR_CONTROL_CENTERED, ::Console::UIRActorManager::RACTOR_CHARACTER_PLAYER, FALSE, TRUE);
    if (bShouldCreateRActor)
    {
        LoadHeroModel(UIRACTOR_CONSOLE_CENTERED);
    }


    uint32 nNumMembers = ::Console::Online::PartyGetMemberCount();
    UIText & tPlayerNumber = UI_GetTextFromID(UITXT_CONSOLE_PLAYER_LABEL_CENTERED);
    if (nNumMembers > 1)
    {
        tPlayerNumber.SetText(UIUtils::Console::PlayerGetDecoratedPlayerNum(nControllerIndex), FALSE);
        tPlayerNumber.SetActive(TRUE);
    }
    else
        tPlayerNumber.SetActive(FALSE);
    UI_GetWindowFromID(UIWND_PAUSE_ARROW_UP).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_PAUSE_ARROW_DOWN).SetActive(FALSE);
    if (UIDeathMenu_Console_IsADeathScreenActive())
    {
        sgptPauseMenuGlobals->bReactivateDeathMenu = TRUE;
        UI_PopDialog(UI_GetInGameDlgStack(), FALSE);
    }







    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);

    D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
    D3::Hero::SavedDefinition & tHeroDefinition = ::Console::GamerProfile::GetHero(ptPlayer->nControllerIndex);
    D3::Hero::Digest & tHeroDigest = *tHeroDefinition.mutable_digest();

    BOOL bIsChallengeRift = GameIsWeeklyChallenge();
    BOOL bIsHardcore = !bIsChallengeRift && (ptPlayer->tHeroStateData.uPlayerFlags & 1);
    BOOL bIsSeasonal = !bIsChallengeRift && ::Console::UIOnlineActions::IsActiveSeason(tHeroDigest.season_created(), tAccountDefinition.digest());
    BOOL bSolo = !bIsChallengeRift && sub_710072e0b0(tHeroDigest.season_created(), tHeroDigest.player_flags2());
    UI_GetWindowFromID(UIWND_CONSOLE_PAUSE_TITLE_HARDCORE).SetActive(bIsHardcore && !bIsSeasonal);
    UI_GetWindowFromID(UIWND_CONSOLE_PAUSE_TITLE_SEASONAL).SetActive(!bIsHardcore && bIsSeasonal && !bSolo);
    UI_GetWindowFromID(UIWND_CONSOLE_PAUSE_TITLE_SEASONAL_HARDCORE).SetActive(bIsHardcore && bIsSeasonal && !bSolo);
    UI_GetWindowFromID(unk_71018606a8).SetActive(!bIsHardcore && bIsSeasonal && bSolo);
    UI_GetWindowFromID(unk_71018606b0).SetActive(bIsHardcore && bIsSeasonal && bSolo);
}











void Close()
{

    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        UIPartyDisplay::Console::HidePartyDisplay();
        UIPartyDisplay::Console::HideFriendDisplay();
        UIPartyDisplay::Console::HideLocalCommunicationsDisplay();
    }
    UINetworkOptions::Console::CloseNetworkOptions();

    UIGamePrivacyOptions::Console::CloseGamePrivacyOptions();

    ::Console::Online::GetPartyOnlineStateChangedEvent().Unsubscribe(OnlineService::internal::function_bind_0_1<void, ::Console::Online::PartySwitchOnlineResult, &OnEventPartyOnlineStateChanged>());
    ::Console::Online::GetLobbyConnectionEvent().Unsubscribe(OnlineService::internal::function_bind_0_2<void, int32, ::Console::Online::LobbyConnectionEventType, &OnLobbyChangeConnectionEvent>());

    UIAchievements::Console::Close();
    UIBannerCustomization::Console::Close();
    UISeasonJourney::Console::Close();
    UILeaderboards::Console::Close();
    UIProfileSummary::Console::Close();
    UI_SetControllerFocus(GetUIIDNone());
    ::Console::Screens::CloseModalDialog(UIWND_PAUSE_CONSOLE);
    ClearHighlightRActor();
    UIText & tPlayerNumber = UI_GetTextFromID(UITXT_CONSOLE_PLAYER_LABEL_CENTERED);
    tPlayerNumber.SetActive(FALSE);

    if (PlayersAreAllLocal())
    {
        GamePause(FALSE);
    }

    if (sgptPauseMenuGlobals->bReactivateDeathMenu)
    {
        sgptPauseMenuGlobals->bReactivateDeathMenu = FALSE;
        UIDeathMenu_Console_Init(sgptPauseMenuGlobals->nOpeningControllerIndex);
    }
}







// NON_MATCHING: the compiled update is 20 bytes shorter with different basic-block placement.
void OnUpdate(const UIID & id)
{
    UI_GetWindowFromID(UIRACTOR_CONSOLE_CENTERED).SetActive(!UISeasonJourney::Console::IsActive());
    if (UIAchievements::Console::IsVisible() || UILeaderboards::Console::IsVisible() || UISeasonJourney::Console::IsActive()) return;







    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        UIPartyDisplay::Console::RefreshPartyDisplay();
        UIPartyDisplay::Console::RefreshFriendsDisplay();
        UIPartyDisplay::Console::RefreshLocalCommunicationsDisplay();
    }
    RefreshQuestDetails();



    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && !UIBannerCustomization::Console::IsVisible() && !UIAchievements::Console::IsVisible() && !UIGameSettings::Console::IsVisible() && !UIProfileSummary::Console::IsVisible())
    {
        if (UIConfirmation::IsVisible()) UIConfirmation::Close();
        tActionManager.ClearAllActions();
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT, StringListFetch("Console:Accept").str());













        ::Console::ControllerInput eNetworkOptionsButton = ::Console::CONTROLLER_INPUT_BACK;











        if (GameIsSeasonal())
        {

            Player * ptPlayer = PlayerGetByControllerIndex(sgptPauseMenuGlobals->nOpeningControllerIndex);
            if (ptPlayer)
            {

                SNO snoLeaderboardTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_LEADERBOARDS_1);
                if (Tutorial::PlayerHasSeen(*ptPlayer, snoLeaderboardTutorial))
                {

                    SNO snoLeaderboardTutorial2 = GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_LEADERBOARDS_2);
                    if (!Tutorial::PlayerHasSeen(*ptPlayer, snoLeaderboardTutorial2) && !gtTutorialMgr.IsAnyShowing(0))
                        gtTutorialMgr.ShowTutorial(GetUIIDNone(), snoLeaderboardTutorial2, ptPlayer, TRUE);
                }
            }
        }




        GameCommonData & tGCD = CGameGetGameCommonData();
        BOOL bTieredRiftIsOpen = tGCD.tGameSyncedData.nOpenLootRunLevel >= 0;
        BOOL bSoloGame = FALSE;
        if (::Console::Online::PartyIsInGame())
        {
            ::Console::Online::GameParams tParams;
            ::Console::Online::PartyGetGameParams(tParams);
            bSoloGame = tParams.unk_14 && tParams.m_eGameType != GT_CHALLENGE_RIFTS;
        }
        if (::Console::Online::PartyIsHost() && ::Console::GamerProfile::GetPrimaryProfileUserIndex() == sgptPauseMenuGlobals->nOpeningControllerIndex && !bTieredRiftIsOpen && ::Console::Online::GetNetworkMode() != ::Console::Online::NET_ADHOC && ::Console::GamerProfile::IsSignedInOnline(sgptPauseMenuGlobals->nOpeningControllerIndex) && ::Console::Online::LobbyService::IsUserConnected(sgptPauseMenuGlobals->nOpeningControllerIndex) && !bSoloGame)
        {










            CRefString sNetworkOptions = StringListFetch("Console:GamePrivacyOptionsNX64");
            ::Console::Online::PartyOnlineState eState = ::Console::Online::PartyGetPreferredOnlineState();
            sNetworkOptions.Format("%s%s", sNetworkOptions.str(), UIUtils::Console::DisplayPartyOnlineStateIcon(eState, FALSE, 2.5f).str());
            tActionManager.SetAction(eNetworkOptionsButton, ACTION_NETWORK_OPTIONS, sNetworkOptions);
        }
        else
            tActionManager.ClearAction(eNetworkOptionsButton);
        if (!bSoloGame)
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_SHOULDER, ACTION_VIEW_LEFT_SOCIAL, StringListFetch("Console:ViewMyPartyNX64"));
            if (::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ADHOC)
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_SHOULDER, ACTION_VIEW_LEFT_SOCIAL, StringListFetch("Console:ViewPlayersNearbyNX64"));
            else
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_SHOULDER, ACTION_VIEW_LEFT_SOCIAL, StringListFetch("Console:ViewOnlineFriendsNX64"));














            if (::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ADHOC &&
                (UI_GetControllerFocus() == UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST ||
                 UI_GetControllerFocus() == UITEXT_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS))
            {

                CRefString sRefreshOptionString = StringListFetch("Console:AdHocRefreshListNX64");
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_VIEW_LEFT_SOCIAL, sRefreshOptionString);
            }
            else
                ::Console::Screens::GetActionManager().ClearAction(::Console::CONTROLLER_INPUT_BUTTON_3);
        }
    }










    UI_GetWindowFromID(UIWND_CONSOLE_PAUSE_ONLINE_STATE).SetActive(TRUE);
    UIText & tOnlineStateMessage = UI_GetTextFromID(UITEXT_CONSOLE_PAUSE_ONLINE_STATE_MESSAGE);

    int32 PrimaryUserIndex = sgptPauseMenuGlobals->nOpeningControllerIndex;
    if (::Console::Online::LobbyService::Get(PrimaryUserIndex)->IsConnected())
    {
        UI_GetWindowFromID(UIWND_CONSOLE_PAUSE_ONLINE_STATE_OFFLINE_ICON).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_PAUSE_ONLINE_STATE_ONLINE_ICON).SetActive(TRUE);
        tOnlineStateMessage.SetText(StringListFetch("Console:ConnectedDiabloNetworkNX64"), FALSE);
    }
    else
    {
        UI_GetWindowFromID(UIWND_CONSOLE_PAUSE_ONLINE_STATE_OFFLINE_ICON).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_CONSOLE_PAUSE_ONLINE_STATE_ONLINE_ICON).SetActive(FALSE);
        tOnlineStateMessage.SetText(StringListFetch("Console:DisconnectedDiabloNetworkNX64"), FALSE);
    }
    if (::Console::Online::LobbyService::ShouldShowReconnectOption(PrimaryUserIndex) != sgptPauseMenuGlobals->bIsShowingConnectOption)
    {
        sgptPauseMenuGlobals->listDataSource.UpdateOptionsAvailability();





        UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UILIST_PAUSE_CONSOLE);
        tList.RefreshVisibleItems();
    }
    ::Console::Screens::UpdateButtonLegendText();










    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UILIST_PAUSE_CONSOLE);
    UI_GetWindowFromID(UIWND_PAUSE_ARROW_UP).SetActive(tListBox.GetFirstVisibleItemIndex() > 0);
    UI_GetWindowFromID(UIWND_PAUSE_ARROW_DOWN).SetActive(tListBox.GetFirstVisibleItemIndex() + tListBox.GetNumVisibleItems() < tListBox.GetNumItems());
}

void sOnWarningAccepted(int32 nControllerIndex, const UIID &)
{
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer)
    {
        UIConversation::ConsoleCancel(GetUIIDNone(), nControllerIndex);
        CPlayerLocalPlayerLeave(ptPlayer);
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);
        if (::Console::GamerProfile::GetPrimaryProfileUserIndex() == nControllerIndex)
        {
            ClientSendOutgoingMessages();
            for (int8 i = 0; i < 4; i++)
            {
                if (PlayerGetByControllerIndex(i))
                    CHero::Save(i, PlayerSaveReason_PlayerQuit);
            }
            CGameLoadingScreenEnable(LSC_ALL);
            if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
                ::Console::Online::PartyLeave();
            else
                ::Console::Online::PartyLeaveAndRemake(TRUE);
            GameStartMenu((Act)400);
        }
    }
}



















void LeaveGame(const UIID & id, int32 nControllerIndex)
{
    NotificationCreationParams tParams;
    tParams.nControllerIndex = nControllerIndex;
    if (::Console::GamerProfile::GetPrimaryProfileUserIndex() == nControllerIndex)
        tParams.uszMessage = StringListFetch("Console:QuitMessage");
    else
        tParams.uszMessage = StringListFetch("H2OLayout:QuitConfirm");
    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_1_1<void, int, const UIID &, &sOnWarningAccepted>(nControllerIndex);
    ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
}








void Resume()
{
    ::Console::Screens::CloseScreenSet(TRUE, FALSE);
    Player * ptPlayer = PlayerGetByControllerIndex(sgptPauseMenuGlobals->nOpeningControllerIndex);
    if (GameIsOpenWorld() && UIWaypointMap::TutorialWasInterrupted())
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            if (ACDIsParticipatingInTieredLootRun(*ptACDPlayer))
                UIWaypointMap::CancelTutorialWasInterrupted();
            else
                UIWaypointMap::Open(ptPlayer, RACTORID_INVALID, -1, TRUE);
        }
    }
}











void Back(const UIID & id, const int32 nControllerIndex)
{
    Resume();
}





static void sHandleOpenNetworkOptions(const int32 nControllerIndex);


void Select(const UIID & id, const int32 nControllerIndex)
{


    if (UI_GetControllerFocus() == UILIST_CONSOLE_MENU_PARTY)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        if (UIPartyDisplay::Console::ShouldDirectFriendsForInvite())
        {


            SwitchSocialWindow(FRIENDS_WINDOW, FALSE, nControllerIndex);
        }
        else
            UIPartyDisplay::Console::OnPartyListAcceptPressed(nControllerIndex);
    }
    else if (UI_GetControllerFocus() == UILIST_CONSOLE_FRIENDS_LIST)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        UIPartyDisplay::Console::OnFriendsListAcceptPressed(nControllerIndex);
    }
    else if (UI_GetControllerFocus() == UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        UIPartyDisplay::Console::OnLocalCommListAcceptPressed(nControllerIndex);
    }
    else if (UI_GetControllerFocus() == UILIST_PAUSE_CONSOLE && UI_GetPrimaryUIController() == nControllerIndex)
    {










        UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UILIST_PAUSE_CONSOLE);
        switch (sgptPauseMenuGlobals->listDataSource.m_menuItems[tListBox.GetSelectedIndex()].eId)
        {
        case RESUME: Resume(); break;
        case OPTIONS:
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_OPEN), cgtPSODefaults);
            ::Console::Screens::OpenNextScreen(nControllerIndex, TRUE); break;
        case NETWORK_SETTINGS:
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_OPEN), cgtPSODefaults);
            sHandleOpenNetworkOptions(nControllerIndex); break;
        case CONNECT_DEMONWARE:
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_OPEN), cgtPSODefaults);
            sHandleConnectToDemonware(nControllerIndex); break;
        case DIFFICULTY:
        {








            GameCommonData & tGCD = CGameGetGameCommonData();
            BOOL bTieredRiftIsOpen = tGCD.tGameSyncedData.nOpenLootRunLevel >= 0;
            BOOL bSetDungeonIsActive = tGCD.tGameSyncedData.bSetDungeonActive;
            if (bTieredRiftIsOpen)
            {

                NotificationCreationParams tParams;
                if (GameIsWeeklyChallenge())
                    tParams.uszMessage = StringListFetch("ConsoleRifts:ErrorCannotChangeDifficultyInTieredRift_CR").str();
                else
                    tParams.uszMessage = StringListFetch("ConsoleRifts:ErrorCannotChangeDifficultyInTieredRift").str();
                tParams.nControllerIndex = nControllerIndex;
                ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
            }
            else if (GameIsWeeklyChallenge())
            {





                NotificationCreationParams tParams;
                tParams.uszMessage = StringListFetch("ConsoleRifts:ErrorCannotChangeDifficultyInTieredRift_CR").str();
                tParams.nControllerIndex = nControllerIndex;
                ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
            }
            else if (bSetDungeonIsActive)
            {

                NotificationCreationParams tParams;
                tParams.uszMessage = StringListFetch("ConsolePause:ErrorCannotChangeDifficultyInSetDungeon").str();
                tParams.nControllerIndex = nControllerIndex;
                ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
            }
            else
            {
                UIGameSettings::Console::OpenForDifficultySelect(nControllerIndex, UIGameSettings::Console::ScreenClosedCallback());
            }
            break;
        }
        case ACHIEVEMENTS:
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_OPEN), cgtPSODefaults);
            UIAchievements::Console::OpenForController(nControllerIndex); break;
        case LEADERBOARDS:
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_OPEN), cgtPSODefaults);
            UILeaderboards::Console::OpenForController(nControllerIndex); break;
        case SEASON_JOURNEY:
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_OPEN), cgtPSODefaults);
            UISeasonJourney::Console::OpenForController(nControllerIndex); break;
        case BANNER:
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_OPEN), cgtPSODefaults);
            UIBannerCustomization::Console::OpenForController(nControllerIndex); break;
        case QUIT: LeaveGame(id, nControllerIndex); break; // Inference: id is unused by LeaveGame; its original argument is optimized away.
        }
    }
}



























static void sHandleOpenGamePrivacyOptions(const int32 nControllerIndex)
{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        return;
    if (::Console::GamerProfile::GetPrimaryProfileUserIndex() != nControllerIndex)
        return;



    GameCommonData & tGCD = CGameGetGameCommonData();
    BOOL bInRift = tGCD.tGameSyncedData.nOpenLootRunLevel >= 0;

    if (::Console::Online::PartyIsInParty() &&
        ::Console::Online::PartyIsHost() &&
        ::Console::Online::PartyGetOnlineState() != ::Console::Online::PARTY_ADHOC &&
        !::Console::Screens::IsScreenTransitioning() &&
        ::Console::GamerProfile::IsSignedInOnline(nControllerIndex) &&
        ::Console::Online::LobbyService::IsUserConnected(nControllerIndex) &&
        !bInRift)
    {
        BOOL bSoloGame = FALSE;
        if (::Console::Online::PartyIsInGame())
        {
            ::Console::Online::GameParams tParams;
            ::Console::Online::PartyGetGameParams(tParams);
            bSoloGame = tParams.unk_14 && tParams.m_eGameType != GT_CHALLENGE_RIFTS;
        }
        if (bSoloGame)
            return;
        UIGamePrivacyOptions::Console::ShowGamePrivacyOptions(nControllerIndex);
    }
}




static void sHandleOpenNetworkOptions(const int32 nControllerIndex)
{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT)) return;
    if (::Console::GamerProfile::GetPrimaryProfileUserIndex() != nControllerIndex) return;





    GameCommonData & tGCD = CGameGetGameCommonData();
    BOOL bTieredRiftIsOpen = tGCD.tGameSyncedData.nOpenLootRunLevel >= 0;
    if (::Console::Online::PartyIsInParty() && ::Console::Online::PartyIsHost() && !::Console::Screens::IsScreenTransitioning() && !bTieredRiftIsOpen)
        UINetworkOptions::Console::ShowNetworkOptions(nControllerIndex);
}








void OpenNetworkOptions(const UIID &, const int32 nControllerIndex)
{



    sHandleOpenGamePrivacyOptions(nControllerIndex);
}





void OnR3(const UIID &, const int32 nControllerIndex)
{
}









enum Directions {
    DIRECTION_LEFT = 0,
    DIRECTION_RIGHT = 1,
};


void SwitchListFocus(Directions eDirection)
{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        return;
    if (UI_GetControllerFocus() == UILIST_PAUSE_CONSOLE && eDirection == DIRECTION_RIGHT)
    {
        BOOL bSoloGame = FALSE;
        if (::Console::Online::PartyIsInGame())
        {
            ::Console::Online::GameParams tParams;
            ::Console::Online::PartyGetGameParams(tParams);
            bSoloGame = tParams.unk_14 && tParams.m_eGameType != GT_CHALLENGE_RIFTS;
        }
        if (bSoloGame)
            return;
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_FOCUS_SWITCH), cgtPSODefaults);

        ClearHighlightRActor();
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_PARTY).SetSelectedIndex(0);

        ChangeFocusToDisplay(sgptPauseMenuGlobals->eSocialWindowActive);
    }




    else if ((UI_GetControllerFocus() == UILIST_CONSOLE_MENU_PARTY ||
              UI_GetControllerFocus() == UILIST_CONSOLE_FRIENDS_LIST ||
              UI_GetControllerFocus() == UITEXT_CONSOLE_FRIENDS_NO_FRIENDS ||
              UI_GetControllerFocus() == UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST ||
              UI_GetControllerFocus() == UITEXT_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS) && eDirection == DIRECTION_LEFT)
    {

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_FOCUS_SWITCH), cgtPSODefaults);

        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_PARTY).SetSelectedIndex(-1);
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).SetSelectedIndex(-1);
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST).SetSelectedIndex(-1);

        UI_SetControllerFocus(UILIST_PAUSE_CONSOLE);
        RefreshHighlightRActor();
    }
}



void OnLeftStick(const UIID &, const int32 nControllerIndex)
{
    ::Console::AnalogStickInputFilter::Rule::ID idRule = sgptPauseMenuGlobals->tStickFilter.HandleInput(::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE), FALSE);
    if (idRule == ::Console::RULE_STICK_LEFT)
        SwitchListFocus(DIRECTION_LEFT);
    else if (idRule == ::Console::RULE_STICK_RIGHT)
        SwitchListFocus(DIRECTION_RIGHT);
}








void OnLeftShoulder(const UIID & id, const int32 nControllerIndex)
{
    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        if (sgptPauseMenuGlobals->eSocialWindowActive == FRIENDS_WINDOW || sgptPauseMenuGlobals->eSocialWindowActive == ADHOC_WINDOW)
            SwitchSocialWindow(PARTY_WINDOW, FALSE, nControllerIndex);
    }
}











void OnRightShoulder(const UIID &, const int32 nControllerIndex)
{
    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        if (sgptPauseMenuGlobals->eSocialWindowActive == PARTY_WINDOW)
        {
            if (::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ADHOC)
                SwitchSocialWindow(ADHOC_WINDOW, FALSE, nControllerIndex);
            else
                SwitchSocialWindow(FRIENDS_WINDOW, FALSE, nControllerIndex);
        }
    }
}












void OnButton3(const UIID & id, const int32 nControllerIndex)
{
    if (::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ADHOC &&
        (UI_GetControllerFocus() == UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST || UI_GetControllerFocus() == UITEXT_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS))
    {
        ::Console::Online::RefreshAdHocList();
    }
}







void OnDPadLeft(const UIID &, const int32)
{
    SwitchListFocus(DIRECTION_LEFT);
}




void OnDPadRight(const UIID &, const int32)
{
    SwitchListFocus(DIRECTION_RIGHT);
}




void InitializeForGame()
{
    sgptPauseMenuGlobals = new PauseMenuGlobals;
}



void DisposeForGame()
{
    delete sgptPauseMenuGlobals;
    sgptPauseMenuGlobals = NULL;
}

}  // namespace Console
}  // namespace UIPause
