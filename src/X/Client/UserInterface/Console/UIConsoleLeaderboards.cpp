// Original file: X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "../../../../Contrib/Contrib/blz/include/blz/map.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../../Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdLobbyServicesCommon.h"
#include "ConsoleInputUtilities.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "Wrappers/RadioSelectorWrapper.h"
#include "Wrappers/RingWrapper.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Controls/UIProgressBarControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../UIStoryboard.h"
#include "../UITypes.h"
#include "../../CGame.h"
#include "../../Gfx/GfxVB.h"
#include "../../Console/ControllerUtils.h"
#include "../../Console/Online.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Game.h"
#include "../../../Common/Leaderboard.h"
#include "../../../Common/Player.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "ConsoleModalNotificationManager.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Achievements/Console/ConsoleAchievements.h"
#include "../../../Common/OnlineService/Achievements.h"
#include "../../../Common/OnlineService/OnlineService.h"

UIStoryboard & UI_GetStoryboardFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp

namespace UILeaderboardInspect { namespace Console {
BOOL InspectActive();
void OnL1(const UIID & id, const int32 nControllerIndex);
void OnButton1(const UIID & id, const int32 nControllerIndex);
void OnButton2(const UIID & id, const int32 nControllerIndex);
void OnUpdate(const UIID & id);
void OnR1(const UIID & id, const int32 nControllerIndex);
void OnL2(const UIID & id, const int32 nControllerIndex);
void OnR2(const UIID & id, const int32 nControllerIndex);
void ForceClose();
} }
namespace Console { namespace Utilities {
XLocale GetSystemLanguage();  // X/Client/Console/UtilitiesNX64.cpp
} }
namespace UIGameSettings { namespace Console {
void OnReturnFromLeaderboard(const int32 nControllerIndex);
} }

namespace UILeaderboards { namespace Console {
enum CurrentTab {
    CURRENT_TAB_INVALID = -1,
    CURRENT_TAB_SEASON_CHALLENGE = 0,
    CURRENT_TAB_GREATER_RIFTS = 1,
    CURRENT_TAB_CONQUESTS = 2,
};

enum ButtonActions {
    ACTION_SELECT = 1,
    ACTION_CLOSE = 2,
    ACTION_BACK = 3,
    ACTION_SELECT_FILTER = 4,
    ACTION_INSPECT = 5,
    ACTION_REFRESH = 6,
    ACTION_TOGGLE_HARDCORE = 7,
    ACTION_VIEW_GAMERCARD = 8,
    ACTION_TOGGLE_LAST_WEEK = 9,
    ACTION_CONQUEST_DETAILS = 10,
};

enum LeaderboardFilters {
    LEADERBOARD_FILTER_FRIENDS = 0,
    LEADERBOARD_FILTER_GLOBAL_POSITION = 1,
    LEADERBOARD_FILTER_TOP_GLOBAL = 2,
    LEADERBOARD_FILTER_COUNT = 3,
};

enum UIState {
    UISTATE_RING_NAVIGATION = 0,
    UISTATE_FILTERS_NAVIGATION = 1,
    UISTATE_INSPECT = 2,
    UISTATE_CONQUEST_DETAILS = 3,
};

enum {
    PROGRESS_BAR = 0,
    CHECKLIST = 1,
};

typedef blz::vector<D3::Leaderboard::Score> ScoreList;

class LeaderboardDataSource : public ConsoleListBoxDataSource {
public:
    LeaderboardDataSource();
    virtual int32 GetNumItems() const;
    const ScoreList & GetCurrentLeaderboard();
    ScoreList & GetLeaderboard(LeaderboardFilters eFilter);
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID & idItemUI, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
    void InitializeData();
    void SetFilter(LeaderboardFilters eFilter);
    int32 nItems;
    int32 nPlayerPositionRank[3];
    int32 nCurrentIndex[3];
    LeaderboardFilters eCurrentFilter;
    ScoreList tFriendsLeaderboardScores;
    ScoreList tGlobalPositionLeaderboardScores;
    ScoreList tTopGlobalLeaderboardScores;
};
static_assert(sizeof(LeaderboardDataSource) == 0x70, "LeaderboardDataSource");

struct UILeaderboardData {
    UILeaderboardData();
    void Initialize();
    void OnFilterSelectionChanged(int32 nIndex, BOOL bSelected);
    static bool ConquestSortOrderPredicate(const OnlineService::AchievementId & idA, const OnlineService::AchievementId & idB);
    void GetConquestsForSeason();
    bdOnlineUserID nUserId;
    ::Console::RingWrapper tRingUI;
    ::Console::AnalogStickInputFilter tRingStickInputFilter;
    BOOL bInitialized;
    int32 nControllerIndex;
    int32 nChallengeNumber;
    CurrentTab eCurrentTab;
    LeaderboardFilters eLeaderboardFilter;
    UIState eUINavigationState;
    int32 nSelectedCategory;
    BOOL bHardcoreGreaterRifts;
    BOOL bHardcoreConquests;
    BOOL bLeaderboardRequestIssued[3];
    BOOL bPlayerPositionRequestIssued;
    BOOL bGlobalLeaderboardsEnabled;
    BOOL bInspectEnabled;
    BOOL bChallengeRifts;
    BOOL bLastWeeklyChallenge;
    BOOL bOpenedFromGameSettings;
    BOOL bInspectOpenedFromGameSettings;
    BOOL bOpenedFromRiftComplete;
    BOOL bWeeklyChallenge;
    int32 nParticipants;
    OnlineService::GameAccountHandle idPlayer;
    OnlineService::AchievementIdList arConquestAchievementIds;
    ::Console::AnalogStickInputFilter tStickInputFilter;
    ::Console::RadioSelectorWrapper tFilterSelector;
    blz::map<int, D3::Leaderboard::Score> mapPlayerScores;
    blz::map<int, D3::Leaderboard::Score> mapPlayerScoresPreviousSeason;
    LeaderboardDataSource tLeaderboardData;
    uint32 tLastUpdatedTicks;
};
static_assert(sizeof(UILeaderboardData) == 0x240, "UILeaderboardData");

void OpenForController(const int32 nControllerIndex);
void IssueLeaderboardRequest();
void UpdateNavigationPrompt();
void UpdateControllerActions(BOOL bDelayRefresh);
void EnableSeasonChallengeScreen(BOOL bEnable, BOOL bWasActiveAlready);
void EnableGreaterRiftsScreen(BOOL bEnable, BOOL bWasActiveAlready);
void EnableConquestsScreen(BOOL bEnable, BOOL bWasActiveAlready);
void UpdateFilterButtonState(BOOL bLocked);
void SetShoulderButtonVisibility();
void EnableInspectMode(BOOL bEnable);
void UpdateRingIcons(const UIID & idRing, BOOL bFocus);
void LoadLeaderboards();
BOOL RingIncludeControlPredicate(const UIID & idControl);
void OnClose(const UIID & id, const int32 nControllerIndex);
void ActivateScreen(CurrentTab eTab);
void IconSelectionChanged(const ::Console::RingWrapper & tRingWrapper, const UIID & idIcon, const BOOL bSelected);
void UpdateLeaderboardDisplay(LeaderboardFilters eFilter, BOOL bForce, int32 nIndexOffset);

static BOOL sFilterSelectionPredicate(const int32 nIndex)
{
    return TRUE;
}

inline LeaderboardDataSource::LeaderboardDataSource() : nItems(0), eCurrentFilter(LEADERBOARD_FILTER_FRIENDS)
{
    for (int32 n = 0; n < 3; ++n)
    {
        nCurrentIndex[n] = -1;
        nPlayerPositionRank[n] = -1;
    }
}

int32 LeaderboardDataSource::GetNumItems() const
{
    switch (eCurrentFilter)
    {
    case LEADERBOARD_FILTER_FRIENDS:
        return (int32)tFriendsLeaderboardScores.size();
    case LEADERBOARD_FILTER_GLOBAL_POSITION:
        return (int32)tGlobalPositionLeaderboardScores.size();
    case LEADERBOARD_FILTER_TOP_GLOBAL:
        return (int32)tTopGlobalLeaderboardScores.size();
    default:
        return 0;
    }
}

UIControlSourceKey LeaderboardDataSource::GetSourceKeyForItem(const int32) const
{
    return UIControlSourceKey::Parse("ConsoleLeaderboardsListItemTemplate:ConsoleLeaderboardsListItemTemplate_main", -1, NULL, NULL);
}

void LeaderboardDataSource::InitializeItemUI(const UIConsoleListBox &, const UIID & idItemUI, const int32)
{
}

inline void LeaderboardDataSource::InitializeData()
{
    for (int32 n = 0; n < 3; ++n)
    {
        nCurrentIndex[n] = -1;
        nPlayerPositionRank[n] = -1;
    }
}

inline void LeaderboardDataSource::SetFilter(LeaderboardFilters eFilter)
{
    eCurrentFilter = eFilter;
}

UILeaderboardData::UILeaderboardData()
    : bInitialized(FALSE)
    , nControllerIndex(-1)
    , nChallengeNumber(-1)
    , eCurrentTab(CURRENT_TAB_INVALID)
    , eLeaderboardFilter(LEADERBOARD_FILTER_FRIENDS)
    , eUINavigationState(UISTATE_RING_NAVIGATION)
    , nSelectedCategory(0)
    , bHardcoreGreaterRifts(FALSE)
    , bHardcoreConquests(FALSE)
    , bPlayerPositionRequestIssued(FALSE)
    , bGlobalLeaderboardsEnabled(TRUE)
    , bInspectEnabled(FALSE)
    , bChallengeRifts(FALSE)
    , bLastWeeklyChallenge(FALSE)
    , bOpenedFromGameSettings(FALSE)
    , bInspectOpenedFromGameSettings(FALSE)
    , bOpenedFromRiftComplete(FALSE)
    , bWeeklyChallenge(FALSE)
    , nParticipants(0)
    , tLastUpdatedTicks(0)
{
    tStickInputFilter = ::Console::GetDefaultStickInputFilter(::Console::FILTER_HORIZONTAL);
    for (int32 n = 0; n < LEADERBOARD_FILTER_COUNT; ++n)
    {
        bLeaderboardRequestIssued[n] = FALSE;
    }
}

void UILeaderboardData::OnFilterSelectionChanged(int32 nIndex, BOOL bSelected)
{
    if (nIndex != -1)
    {
        const UIID & ptidItem = tFilterSelector.GetUIIDAtIndex(nIndex);
        UIID idSelector;
        idSelector.SetFromParent(ptidItem, "LayoutRoot.Filter.FilterButton");
        UI_GetButtonFromID(idSelector).SetButtonState(bSelected ? BS_PUSHED : BS_NORMAL, FALSE, FALSE);
        if (bSelected)
        {
            UIID idStoryboard;
            idStoryboard.SetFromParent(ptidItem, "FilterListHighlight");
            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
            tStoryboard.Stop();
            tStoryboard.Play(FALSE, 0.0f);
        }
    }
}

bool UILeaderboardData::ConquestSortOrderPredicate(const OnlineService::AchievementId & idA, const OnlineService::AchievementId & idB)
{
    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    const OnlineService::AchievementDefinition * ptDefinitionA = tAchievements.GetAchievementDefinition(idA);
    const OnlineService::AchievementDefinition * ptDefinitionB = tAchievements.GetAchievementDefinition(idB);
    BOOL bAIsHardcore = tAchievements.IsAchievementHardcoreByCriteria(idA);
    BOOL bBIsHardcore = tAchievements.IsAchievementHardcoreByCriteria(idB);
    if (bAIsHardcore != bBIsHardcore)
        return bBIsHardcore;
    if (ptDefinitionA && ptDefinitionB)
        return ptDefinitionA->m_uiOrder < ptDefinitionB->m_uiOrder;
    return ptDefinitionA != NULL;
}

void UILeaderboardData::GetConquestsForSeason()
{
    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::CategoryIdList tTopCategories;
    arConquestAchievementIds.clear();
    if (tAchievements.GetChildCategories(0, tTopCategories))
    {
        for (uint32 i = 0; i < tTopCategories.size(); i++)
        {
            const OnlineService::CategoryDefinition * ptConquestCategoryDefinition = tAchievements.GetCategoryDefinition(tTopCategories[i]);
            if (ptConquestCategoryDefinition != NULL && strcmp(ptConquestCategoryDefinition->m_pszNameKey, "Achievements:Conquests") == 0)
            {
                OnlineService::AchievementIdList arAchievementIdsForCurrentSeason;
                tAchievements.GetCategoryAchievements(ptConquestCategoryDefinition->m_id, arAchievementIdsForCurrentSeason);
                for (uint32 i = 0; i < arAchievementIdsForCurrentSeason.size(); i++)
                {
                    const OnlineService::AchievementDefinition * ptDefinition = tAchievements.GetAchievementDefinition(arAchievementIdsForCurrentSeason[i]);
                    if (ptDefinition != NULL && tAchievements.IsAchievementValidForSeason(ptDefinition, OnlineService::GetSeasonNum()))
                        arConquestAchievementIds.push_back(arAchievementIdsForCurrentSeason[i]);
                }
                blz::sort(arConquestAchievementIds.begin(), arConquestAchievementIds.end(), ConquestSortOrderPredicate);
                break;
            }
        }
    }
}

UILeaderboardData sgtUILeaderboardData;

static void sFilterSelectionChanged(const UIID & idSkill, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput)
{
    if (sgtUILeaderboardData.bInitialized)
        sgtUILeaderboardData.OnFilterSelectionChanged(nIndex, bSelected);
}

void OpenForGameSettings(const int32 nControllerIndex, BOOL bOpenedFromGameSettings)
{
    OpenForController(nControllerIndex);
    sgtUILeaderboardData.bOpenedFromGameSettings = bOpenedFromGameSettings;
}

void OnClose(const UIID & id, const int32 nControllerIndex)
{
    GameSoftPause(FALSE);
    ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_LEADERBOARDS);
    sgtUILeaderboardData.bOpenedFromRiftComplete = FALSE;
    if (::Console::Screens::IsScreenSetOpen(::Console::Screens::LEADERBOARD_SCREEN_SET))
        ::Console::Screens::CloseScreenSet(::Console::Screens::LEADERBOARD_SCREEN_SET, FALSE);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
    sgtUILeaderboardData.bInspectOpenedFromGameSettings = FALSE;
    if (sgtUILeaderboardData.bOpenedFromGameSettings)
    {
        sgtUILeaderboardData.bOpenedFromGameSettings = FALSE;
        UIGameSettings::Console::OnReturnFromLeaderboard(nControllerIndex);
    }
}

void OnButton3(const UIID & id, const int32 nControllerIndex)
{
    if (sgtUILeaderboardData.eUINavigationState == UISTATE_FILTERS_NAVIGATION)
    {
        ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
        if (!tActionManager.IsActionDelayed((::Console::ControllerInput)5))
        {
            IssueLeaderboardRequest();
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        }
    }
}

void OnL2(const UIID & id, const int32 nControllerIndex)
{
    if (UILeaderboardInspect::Console::InspectActive())
        UILeaderboardInspect::Console::OnL2(id, nControllerIndex);
}

void OnR2(const UIID & id, const int32 nControllerIndex)
{
    if (UILeaderboardInspect::Console::InspectActive())
        UILeaderboardInspect::Console::OnR2(id, nControllerIndex);
}

BOOL IsVisible()
{
    return UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS).IsActive();
}

void Close()
{
    if (IsVisible())
    {
        sgtUILeaderboardData.bOpenedFromRiftComplete = FALSE;
        GameSoftPause(FALSE);
        ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_LEADERBOARDS);
        if (sgtUILeaderboardData.bInspectEnabled)
        {
            UILeaderboardInspect::Console::ForceClose();
            sgtUILeaderboardData.bInspectEnabled = FALSE;
        }
    }
}

BOOL RingIncludeControlPredicate(const UIID & idControl)
{
    return UI_GetControlFromID(idControl).IsType(UIT_BUTTON);
}

void SetRiftCompletionOpenContext(int32 nParticipants, BOOL bWeeklyChallenge)
{
    sgtUILeaderboardData.bOpenedFromRiftComplete = TRUE;
    sgtUILeaderboardData.bWeeklyChallenge = bWeeklyChallenge;
    sgtUILeaderboardData.nParticipants = nParticipants;
}

inline UIConsoleListBox & GetCurrentList()
{
    return UI_GetConsoleListBoxFromID(sgtUILeaderboardData.eLeaderboardFilter == LEADERBOARD_FILTER_FRIENDS ? UILIST_CONSOLE_LEADERBOARDS_ENTRY_LIST_FRIENDS
        : sgtUILeaderboardData.eLeaderboardFilter == LEADERBOARD_FILTER_GLOBAL_POSITION ? UILIST_CONSOLE_LEADERBOARDS_ENTRY_LIST_GLOBAL
        : UILIST_CONSOLE_LEADERBOARDS_ENTRY_LIST_TOP_GLOBAL);
}

void ActivateScreen(CurrentTab eTab)
{
    GetCurrentList().SetActive(FALSE);
    BOOL bWasActiveAlready = sgtUILeaderboardData.eCurrentTab == eTab;
    sgtUILeaderboardData.eCurrentTab = eTab;
    switch (eTab)
    {
    case CURRENT_TAB_SEASON_CHALLENGE:
        EnableSeasonChallengeScreen(TRUE, bWasActiveAlready);
        EnableGreaterRiftsScreen(FALSE, FALSE);
        EnableConquestsScreen(FALSE, FALSE);
        break;
    case CURRENT_TAB_GREATER_RIFTS:
        EnableSeasonChallengeScreen(FALSE, FALSE);
        EnableGreaterRiftsScreen(TRUE, bWasActiveAlready);
        EnableConquestsScreen(FALSE, FALSE);
        break;
    case CURRENT_TAB_CONQUESTS:
        EnableSeasonChallengeScreen(FALSE, FALSE);
        EnableGreaterRiftsScreen(FALSE, FALSE);
        EnableConquestsScreen(TRUE, bWasActiveAlready);
        break;
    default:
        break;
    }
    UpdateNavigationPrompt();
    UpdateControllerActions(FALSE);
}

void LoadLeaderboards()
{
    sgtUILeaderboardData.tLeaderboardData.InitializeData();
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LEADERBOARDS_ENTRY_LIST_FRIENDS).SetDataSource(sgtUILeaderboardData.tLeaderboardData);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LEADERBOARDS_ENTRY_LIST_GLOBAL).SetDataSource(sgtUILeaderboardData.tLeaderboardData);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LEADERBOARDS_ENTRY_LIST_TOP_GLOBAL).SetDataSource(sgtUILeaderboardData.tLeaderboardData);
    UI_GetWindowFromID(UITEXT_CONSOLE_LEADERBOARDS_MESSAGE).SetActive(TRUE);
    UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_MESSAGE).SetText(StringListFetch("ConsoleLeaderboards:RetrievingData"), FALSE);
    IssueLeaderboardRequest();
}

void OnL1(const UIID & id, const int32 nControllerIndex)
{
    if (UILeaderboardInspect::Console::InspectActive())
    {
        UILeaderboardInspect::Console::OnL1(id, nControllerIndex);
        return;
    }
    if (sgtUILeaderboardData.eUINavigationState == UISTATE_RING_NAVIGATION)
    {
        int32 nTab = sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_SEASON_CHALLENGE ? CURRENT_TAB_CONQUESTS : sgtUILeaderboardData.eCurrentTab - 1;
        UpdateFilterButtonState(FALSE);
        ActivateScreen((CurrentTab)nTab);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_COLUMN_HEADERS).SetActive(FALSE);
    }
    else if (sgtUILeaderboardData.eUINavigationState == UISTATE_FILTERS_NAVIGATION && sgtUILeaderboardData.bGlobalLeaderboardsEnabled)
    {
        int32 nCurrentIndex = sgtUILeaderboardData.tFilterSelector.GetSelectedIndex();
        LeaderboardFilters eFilter = (LeaderboardFilters)((nCurrentIndex + LEADERBOARD_FILTER_COUNT - 1) % LEADERBOARD_FILTER_COUNT);
        sgtUILeaderboardData.tFilterSelector.SetSelectedIndex(eFilter, TRUE);
        sgtUILeaderboardData.eLeaderboardFilter = eFilter;
        sgtUILeaderboardData.tLeaderboardData.SetFilter(eFilter);
        UpdateLeaderboardDisplay(eFilter, FALSE, 0);
        UpdateControllerActions(FALSE);
        SoundManager::PlaySound(GlobalSNOGet((GlobalSNO)0x38f), cgtPSODefaults);
    }
}

void OnR1(const UIID & id, const int32 nControllerIndex)
{
    if (UILeaderboardInspect::Console::InspectActive())
    {
        UILeaderboardInspect::Console::OnR1(id, nControllerIndex);
        return;
    }
    if (sgtUILeaderboardData.eUINavigationState == UISTATE_RING_NAVIGATION)
    {
        int32 nTab = sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_CONQUESTS ? CURRENT_TAB_SEASON_CHALLENGE : sgtUILeaderboardData.eCurrentTab + 1;
        ActivateScreen((CurrentTab)nTab);
        UpdateFilterButtonState(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_COLUMN_HEADERS).SetActive(FALSE);
    }
    else if (sgtUILeaderboardData.eUINavigationState == UISTATE_FILTERS_NAVIGATION && sgtUILeaderboardData.bGlobalLeaderboardsEnabled)
    {
        int32 nCurrentIndex = sgtUILeaderboardData.tFilterSelector.GetSelectedIndex();
        LeaderboardFilters eFilter = (LeaderboardFilters)((nCurrentIndex + 1) % LEADERBOARD_FILTER_COUNT);
        sgtUILeaderboardData.tFilterSelector.SetSelectedIndex(eFilter, TRUE);
        sgtUILeaderboardData.eLeaderboardFilter = eFilter;
        sgtUILeaderboardData.tLeaderboardData.SetFilter(eFilter);
        UpdateLeaderboardDisplay(eFilter, FALSE, 0);
        UpdateControllerActions(FALSE);
        SoundManager::PlaySound(GlobalSNOGet((GlobalSNO)0x38f), cgtPSODefaults);
    }
}

void OnLStick(const UIID & id, const int32 nControllerIndex)
{
    if (::Console::Screens::IsScreenTransitioning())
        return;
    if (sgtUILeaderboardData.eUINavigationState == UISTATE_FILTERS_NAVIGATION)
    {
        if (::Console::ControllerUtils::IsButtonDown(nControllerIndex, 10) || ::Console::ControllerUtils::IsButtonDown(nControllerIndex, 11))
            return;
        if (::Console::ControllerUtils::IsButtonDown(nControllerIndex, 8) || ::Console::ControllerUtils::IsButtonDown(nControllerIndex, 9))
            return;
    }
}

CRefString GetChallengeLeaderboardName(int32 nIndex)
{
    if (nIndex == 0)
        return StringListFetch("ConsoleLeaderboards:SeasonChallengesTitle");
    if (nIndex >= 1 && nIndex <= 4)
    {
        CRefString strElement = FormatRefString("ConsoleLeaderboards:GreaterRiftsLeaderboard%d", nIndex);
        return CRefString(StringListFetch(strElement).str());
    }
    return CRefString("Missing Achievement");
}

CRefString GetGreaterRiftLeaderboardName(int32 nIndex)
{
    if (nIndex >= 0)
    {
        if (nIndex < 3)
        {
            CRefString strElement = FormatRefString("ConsoleLeaderboards:GreaterRiftsLeaderboard%d", nIndex + 2);
            return CRefString(StringListFetch(strElement).str());
        }
        else if (nIndex <= 9)
        {
            switch (nIndex)
            {
            case 3: return PlayerGetClassUIName(PLAYER_CLASS_BARBARIAN, TRUE);
            case 4: return PlayerGetClassUIName(PLAYER_CLASS_CRUSADER, TRUE);
            case 5: return PlayerGetClassUIName(PLAYER_CLASS_DEMONHUNTER, TRUE);
            case 6: return PlayerGetClassUIName(PLAYER_CLASS_MONK, TRUE);
            case 7: return PlayerGetClassUIName(PLAYER_CLASS_NECROMANCER, TRUE);
            case 8: return PlayerGetClassUIName(PLAYER_CLASS_WITCHDOCTOR, TRUE);
            case 9: return PlayerGetClassUIName(PLAYER_CLASS_WIZARD, TRUE);
            }
        }
    }
    return CRefString("Invalid");
}

UIID GetElementUIIDForGreaterRiftsCategory(int32 nIndex)
{
    switch (nIndex)
    {
    case 0: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, ".TwoPlayers");
    case 1: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, ".ThreePlayers");
    case 2: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, ".FourPlayers");
    case 3: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, ".Barbarian");
    case 4: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, ".Crusader");
    case 5: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, ".DemonHunter");
    case 6: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, ".Monk");
    case 7: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, ".Necromancer");
    case 8: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, ".WitchDoctor");
    case 9: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, ".Wizard");
    default: return GetUIIDNone();
    }
}

UIID GetElementUIIDForChallengesCategory(int32 nIndex)
{
    switch (nIndex)
    {
    case 0: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING, ".LeaderboardSeasonChallenges");
    case 1: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING, ".LeaderboardChallengeRiftsSingle");
    case 2: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING, ".LeaderboardChallengeRiftsTwoPlayers");
    case 3: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING, ".LeaderboardChallengeRiftsThreePlayers");
    case 4: return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING, ".LeaderboardChallengeRiftsFourPlayers");
    default: return GetUIIDNone();
    }
}

void PopupLeaderboardError(::Console::Online::LeaderboardResult eResult)
{
    NotificationCreationParams tParams;
    tParams.nControllerIndex = sgtUILeaderboardData.nControllerIndex;
    if (eResult != ::Console::Online::LeaderboardResult::FAILED_NOT_CONNECTED)
    {
        tParams.uszMessage = StringListFetch("ConsoleErrors:LeaderboardUnknownError");
        ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
    }
}

void ReturnFromInspect()
{
    UI_SetControllerFocus(GetCurrentList().GetID());
    sgtUILeaderboardData.eUINavigationState = UISTATE_FILTERS_NAVIGATION;
    sgtUILeaderboardData.bInspectEnabled = FALSE;
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_FILTERS_WRAPPER).SetActive(TRUE);
    GetCurrentList().SetActive(TRUE);
    UpdateControllerActions(FALSE);
}

void UpdateNavigationPrompt()
{
    UIText & tNavigationText = UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_MESSAGE);
    UIText & tHeaderText = UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_TITLE);
    UIWindow & tGlow = UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_TITLE_BLINKER);
    switch (sgtUILeaderboardData.eUINavigationState)
    {
    case UISTATE_RING_NAVIGATION:
        tNavigationText.SetText(StringListFetch("ConsoleLeaderboards:InstructionSelectLeaderboard"), FALSE);
        tNavigationText.SetActive(TRUE);
        tHeaderText.SetTextColor((FontSpecifier)5);
        tGlow.SetActive(FALSE);
        break;
    case UISTATE_FILTERS_NAVIGATION:
        tHeaderText.SetTextColor((FontSpecifier)0);
        tGlow.SetActive(TRUE);
        break;
    case UISTATE_CONQUEST_DETAILS:
        tNavigationText.SetActive(FALSE);
        break;
    default:
        break;
    }
}

void OnButton2(const UIID & id, const int32 nControllerIndex)
{
    if (UILeaderboardInspect::Console::InspectActive())
    {
        UILeaderboardInspect::Console::OnButton2(id, nControllerIndex);
        return;
    }
    switch (sgtUILeaderboardData.eUINavigationState)
    {
    case UISTATE_RING_NAVIGATION:
        OnClose(*(const UIID *)NULL, nControllerIndex);
        return;
    case UISTATE_INSPECT:
        UILeaderboardInspect::Console::OnButton2(id, nControllerIndex);
        break;
    case UISTATE_CONQUEST_DETAILS:
        UI_GetWindowFromID(UISCROLL_CONSOLE_LEADERBOARDS_CONQUESTS_DETAILVIEW).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_DESCRIPTION).SetActive(TRUE);
        UpdateNavigationPrompt();
    case UISTATE_FILTERS_NAVIGATION:
        sgtUILeaderboardData.eUINavigationState = UISTATE_RING_NAVIGATION;
        sgtUILeaderboardData.tFilterSelector.SetSelectedIndex(-1, FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_COLUMN_HEADERS).SetActive(FALSE);
        ActivateScreen(sgtUILeaderboardData.eCurrentTab);
        SetShoulderButtonVisibility();
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
        break;
    }
    if (sgtUILeaderboardData.eUINavigationState != UISTATE_INSPECT)
    {
        UpdateNavigationPrompt();
        UpdateControllerActions(FALSE);
    }
}

void UpdateFilterButtonState(BOOL bLocked)
{
    for (int32 n = 0; n < sgtUILeaderboardData.tFilterSelector.GetNumberOfOptions(); ++n)
    {
        const UIID & idFilter = sgtUILeaderboardData.tFilterSelector.GetUIIDAtIndex(n);
        UIID idButton = UIIDCreateFromParent(idFilter, ".LayoutRoot.Filter.FilterButton");
        UIID idText = UIIDCreateFromParent(idFilter, ".LayoutRoot.Filter.FilterTitle");
        if (bLocked)
        {
            UI_GetButtonFromID(idButton).SetButtonState(n == sgtUILeaderboardData.eLeaderboardFilter ? BS_PUSHED : BS_NORMAL, FALSE, FALSE);
            UI_GetTextFromID(idText).SetTextColor(n == sgtUILeaderboardData.eLeaderboardFilter ? (FontSpecifier)0 : (FontSpecifier)5);
        }
        else
        {
            UI_GetButtonFromID(idButton).SetButtonState(BS_NORMAL, FALSE, FALSE);
            UI_GetTextFromID(idText).SetTextColor((FontSpecifier)5);
        }
    }
}

void OnUpdate(const UIID & id)
{
    if (UILeaderboardInspect::Console::InspectActive())
    {
        UILeaderboardInspect::Console::OnUpdate(id);
        return;
    }
    if (sgtUILeaderboardData.tRingUI.IsEnabled() && UI_GetPrimaryUIController() != -1)
        sgtUILeaderboardData.tRingUI.UpdateInput(UI_GetPrimaryUIController(), IconSelectionChanged);
    if (::Console::Screens::GetActionManager().AreActionsDirty())
        UpdateControllerActions(FALSE);
    if (sgtUILeaderboardData.tLastUpdatedTicks)
    {
        UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_UPDATED).SetActive(TRUE);
        uint32 nTicksElapsed = AppGetTicks() - sgtUILeaderboardData.tLastUpdatedTicks;
        if (nTicksElapsed <= 7200)
            UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_UPDATED).SetText(StringListFetch("ConsoleLeaderboards:LastUpdateRecent"), FALSE);
        else
            UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_UPDATED).SetText(LocalizedSprintf(StringListFetch("ConsoleLeaderboards:LastUpdateMinutesAgo"), UnicodeText::FormatNumber((int32)(nTicksElapsed / 3600), TRUE, FNS_NORMAL, (XLocale)0).str()), FALSE);
    }
    else
    {
        UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_UPDATED).SetActive(FALSE);
    }
}

void SetShoulderButtonVisibility()
{
    UIID idFilterBarL = UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_FILTERS, ".LeftTrigger");
    UIID idFilterBarR = UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_FILTERS, ".RightTrigger");
    UI_GetWindowFromID(idFilterBarL).SetActive(sgtUILeaderboardData.eUINavigationState != UISTATE_RING_NAVIGATION);
    UI_GetWindowFromID(idFilterBarR).SetActive(sgtUILeaderboardData.eUINavigationState != UISTATE_RING_NAVIGATION);
    UIID idNavSeasonChallengesL1 = UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_NAVIGATION, ".SeasonChallenges.KeyBinding1");
    UIID idNavSeasonChallengesR1 = UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_NAVIGATION, ".SeasonChallenges.KeyBinding2");
    UI_GetWindowFromID(idNavSeasonChallengesL1).SetActive(sgtUILeaderboardData.eUINavigationState == UISTATE_RING_NAVIGATION);
    UI_GetWindowFromID(idNavSeasonChallengesR1).SetActive(sgtUILeaderboardData.eUINavigationState == UISTATE_RING_NAVIGATION);
    UIID idNavGreaterRiftsR1 = UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_NAVIGATION, ".GreaterRifts.KeyBinding2");
    UIID idNavGreaterRiftsL1 = UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_NAVIGATION, ".GreaterRifts.KeyBinding1");
    UI_GetWindowFromID(idNavGreaterRiftsL1).SetActive(sgtUILeaderboardData.eUINavigationState == UISTATE_RING_NAVIGATION);
    UI_GetWindowFromID(idNavGreaterRiftsR1).SetActive(sgtUILeaderboardData.eUINavigationState == UISTATE_RING_NAVIGATION);
    UIID idNavConquestsR1 = UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_NAVIGATION, ".Conquests.KeyBinding2");
    UIID idNavConquestsL1 = UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_NAVIGATION, ".Conquests.KeyBinding1");
    UI_GetWindowFromID(idNavConquestsL1).SetActive(sgtUILeaderboardData.eUINavigationState == UISTATE_RING_NAVIGATION);
    UI_GetWindowFromID(idNavConquestsR1).SetActive(sgtUILeaderboardData.eUINavigationState == UISTATE_RING_NAVIGATION);
}

void OnButton1(const UIID & id, const int32 nControllerIndex)
{
    if (sgtUILeaderboardData.bInspectOpenedFromGameSettings)
        return;
    switch (sgtUILeaderboardData.eUINavigationState)
    {
    case UISTATE_RING_NAVIGATION:
        sgtUILeaderboardData.tRingUI.SetEnabled(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_GLOW).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_GLOW_HARDCORE).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING_GLOW).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING_GLOW).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_SEASON_RING_GLOW).SetActive(FALSE);
        UpdateRingIcons(sgtUILeaderboardData.tRingUI.GetContainerControlID(), FALSE);
        if (sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_CONQUESTS)
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_DESCRIPTION).SetActive(FALSE);
        sgtUILeaderboardData.tFilterSelector.SetSelectedIndex(sgtUILeaderboardData.eLeaderboardFilter, FALSE);
        sgtUILeaderboardData.eUINavigationState = UISTATE_FILTERS_NAVIGATION;
        SetShoulderButtonVisibility();
        LoadLeaderboards();
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        break;
    case UISTATE_FILTERS_NAVIGATION:
        if (sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_GREATER_RIFTS)
        {
            if (!sgtUILeaderboardData.bLeaderboardRequestIssued[LEADERBOARD_FILTER_FRIENDS] && !sgtUILeaderboardData.bLeaderboardRequestIssued[LEADERBOARD_FILTER_GLOBAL_POSITION] && !sgtUILeaderboardData.bLeaderboardRequestIssued[LEADERBOARD_FILTER_TOP_GLOBAL])
                EnableInspectMode(TRUE);
        }
        break;
    case UISTATE_INSPECT:
        UILeaderboardInspect::Console::OnButton1(id, nControllerIndex);
        break;
    default:
        break;
    }
    if (sgtUILeaderboardData.eUINavigationState != UISTATE_INSPECT)
    {
        UpdateNavigationPrompt();
        UpdateControllerActions(FALSE);
    }
}

void ToggleHardcoreGreaterRifts()
{
    sgtUILeaderboardData.bHardcoreGreaterRifts = !sgtUILeaderboardData.bHardcoreGreaterRifts;
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_HARDCORE).SetActive(sgtUILeaderboardData.bHardcoreGreaterRifts);
    if (sgtUILeaderboardData.bHardcoreGreaterRifts)
    {
        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_LEADERBOARDS_HARDCORE_STORYBOARD);
        tStoryboard.Stop();
        tStoryboard.Play(FALSE, 0.0f);
    }
    if (sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_GREATER_RIFTS)
    {
        const UIID & idControl = sgtUILeaderboardData.tRingUI.GetSelectedControlID();
        if (sgtUILeaderboardData.bHardcoreGreaterRifts)
            sgtUILeaderboardData.tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_CURSOR_HARDCORE, RingIncludeControlPredicate, TRUE);
        else
            sgtUILeaderboardData.tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_CURSOR, RingIncludeControlPredicate, TRUE);
        sgtUILeaderboardData.tRingUI.SelectControl(idControl, IconSelectionChanged);
        if (sgtUILeaderboardData.eUINavigationState == UISTATE_RING_NAVIGATION)
        {
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_GLOW).SetActive(!sgtUILeaderboardData.bHardcoreGreaterRifts);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_GLOW_HARDCORE).SetActive(sgtUILeaderboardData.bHardcoreGreaterRifts);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_CURSOR).SetActive(!sgtUILeaderboardData.bHardcoreGreaterRifts);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_CURSOR_HARDCORE).SetActive(sgtUILeaderboardData.bHardcoreGreaterRifts);
        }
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_LEAF_HARDCORE).SetActive(sgtUILeaderboardData.bHardcoreGreaterRifts);
    }
}

void UpdateChallengesRingDisplay(int32 nSelectedCategory)
{
    if (sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_SEASON_CHALLENGE)
    {
        const UIID & idControl = sgtUILeaderboardData.tRingUI.GetSelectedControlID();
        BOOL bChallengeRifts = nSelectedCategory != 0;
        if (sgtUILeaderboardData.bChallengeRifts != bChallengeRifts)
        {
            sgtUILeaderboardData.bChallengeRifts = bChallengeRifts;
            if (bChallengeRifts)
                sgtUILeaderboardData.tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING, UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING_CURSOR, RingIncludeControlPredicate, TRUE);
            else
                sgtUILeaderboardData.tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING, UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_SEASON_RING_CURSOR, RingIncludeControlPredicate, TRUE);
            sgtUILeaderboardData.tRingUI.SelectControl(idControl, IconSelectionChanged);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_SEASON_RING_GLOW).SetActive(!bChallengeRifts);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_SEASON_RING_CURSOR).SetActive(!bChallengeRifts);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING_SEASON_ICON).SetActive(!bChallengeRifts);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING_GLOW).SetActive(bChallengeRifts);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING_CURSOR).SetActive(bChallengeRifts);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING_ICON).SetActive(bChallengeRifts);
        }
    }
}

void UpdateConquestHardcoreDisplay(int32 nSelectedCategory)
{
    if (sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_CONQUESTS)
    {
        BOOL bHardcore = nSelectedCategory >= 5;
        if (sgtUILeaderboardData.bHardcoreConquests != bHardcore)
        {
            sgtUILeaderboardData.bHardcoreConquests = bHardcore;
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_HARDCORE).SetActive(bHardcore);
            const UIID & idControl = sgtUILeaderboardData.tRingUI.GetSelectedControlID();
            if (bHardcore)
                sgtUILeaderboardData.tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING, UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING_CURSOR_HARDCORE, RingIncludeControlPredicate, TRUE);
            else
                sgtUILeaderboardData.tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING, UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING_CURSOR, RingIncludeControlPredicate, TRUE);
            sgtUILeaderboardData.tRingUI.SelectControl(idControl, IconSelectionChanged);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING_GLOW).SetActive(!bHardcore);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING_GLOW_HARDCORE).SetActive(bHardcore);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING_CURSOR).SetActive(!bHardcore);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING_CURSOR_HARDCORE).SetActive(bHardcore);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING_LEAF_HARDCORE).SetActive(sgtUILeaderboardData.bHardcoreConquests);
        }
    }
}

void UpdateRingIcons(const UIID & idRing, BOOL bFocus)
{
    const UIControlItemArray & arChildren = UI_GetWindowFromID(idRing).GetChildList();
    for (uint32 i = 0; i < arChildren.size(); ++i)
    {
        if (!arChildren[i].ptControl->IsType(UIT_BUTTON))
            continue;
        UIButton * ptButton = (UIButton *)arChildren[i].ptControl;
        if (bFocus || sgtUILeaderboardData.tRingUI.GetSelectedControlID() == arChildren[i].id)
        {
            ptButton->SetEnabled(TRUE);
            ptButton->SetShader(SNO_NONE);
            if (sgtUILeaderboardData.tRingUI.GetSelectedControlID() != arChildren[i].id)
                ptButton->SetButtonState(BS_NORMAL, FALSE, FALSE);
        }
        else
        {
            ptButton->SetEnabled(FALSE);
            ptButton->SetShader(GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
        }
    }
}

void sFixFontSize(const UIID & idNavBar)
{
    int32 nPointSize = 18;
    XLocale eCurrentLocale = ::Console::Utilities::GetSystemLanguage();
    switch (eCurrentLocale)
    {
    case XLOCALE_GERMAN:
    case XLOCALE_PORTUGUESE_BRAZIL:
        nPointSize = 14;
        break;
    }
    UIID id1 = UIIDCreateFromParent(idNavBar, ".Text1");
    UIID id2 = UIIDCreateFromParent(idNavBar, ".Text2");
    UIID id3 = UIIDCreateFromParent(idNavBar, ".Text3");
    UI_GetTextFromID(id1).SetFontSize(nPointSize);
    UI_GetTextFromID(id2).SetFontSize(nPointSize);
    UI_GetTextFromID(id3).SetFontSize(nPointSize);
    UI_GetTextFromID(id1).SetText(StringListFetch("ConsoleNavMenu:SeasonChallenges"), FALSE);
    UI_GetTextFromID(id2).SetText(StringListFetch("ConsoleNavMenu:GreaterRifts"), FALSE);
    UI_GetTextFromID(id3).SetText(StringListFetch("ConsoleNavMenu:Conquests"), FALSE);
}

void UpdateLeaderboardColumnHeaders(BOOL bEnable)
{
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_COLUMN_HEADERS).SetActive(bEnable);
    if (bEnable)
    {
        UIID idScore = UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_COLUMN_HEADERS, ".Score");
        UIID idScoreLabel = UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_COLUMN_HEADERS, ".Score.ScoreLabelText");
        UIID idTimeLabel = UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_COLUMN_HEADERS, ".Time.TimeLabelText");
        if (sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_CONQUESTS)
        {
            UI_GetWindowFromID(idScore).SetActive(FALSE);
            UI_GetWindowFromID(idScoreLabel).SetActive(TRUE);
            UI_GetTextFromID(idTimeLabel).SetText(StringListFetch("ConsoleLeaderboards:DateColumnHeader"), FALSE);
        }
        else if (sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_SEASON_CHALLENGE && sgtUILeaderboardData.nSelectedCategory == 0)
        {
            UI_GetWindowFromID(idScore).SetActive(TRUE);
            UI_GetWindowFromID(idScoreLabel).SetActive(TRUE);
            UI_GetTextFromID(idScoreLabel).SetText(StringListFetch("ConsoleLeaderboards:ScoreColumnHeader"), FALSE);
            UI_GetTextFromID(idTimeLabel).SetText(StringListFetch("ConsoleLeaderboards:DateColumnHeader"), FALSE);
        }
        else
        {
            UI_GetWindowFromID(idScore).SetActive(TRUE);
            UI_GetWindowFromID(idScoreLabel).SetActive(sgtUILeaderboardData.eCurrentTab != CURRENT_TAB_SEASON_CHALLENGE);
            UI_GetTextFromID(idScoreLabel).SetText(StringListFetch("ConsoleLeaderboards:TierColumnHeader"), FALSE);
            UI_GetTextFromID(idTimeLabel).SetText(StringListFetch("ConsoleLeaderboards:TimeSpentColumnHeader"), FALSE);
        }
    }
}

void NotifyInspectOpenedFromGameSettings(BOOL bOpened)
{
    sgtUILeaderboardData.bInspectOpenedFromGameSettings = bOpened;
}


} }
extern UIID UITEXT_CONSOLE_ACHIEVEMENTS_DETAILVIEW_PROGRESS_TEXT;
extern UIID UITEXT_CONSOLE_LEADERBOARDS_RING_TITLE;
uint64 LeaderboardGetLeaderboardIdForConsoleMetadata(uint64 leaderboardId, const D3::Leaderboard::Score & tScore);  // X/Common/Leaderboard.cpp
namespace UILeaderboardInspect { namespace Console {
void Open(int32 nControllerIndex, const LeaderboardKeyAndPartition & tKey, int64 idGameAccount, PlayerClass ePlayerClass);
} }
extern UIID UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_NAVIGATION;
extern UIID UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_NAVIGATION;
extern UIID UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_DETAIL;
extern UIID UIWND_CONSOLE_LEADERBOARDS_SEASON_CHALLENGES_NAVIGATION;
extern UIID UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING_DETAIL;
extern UIID UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING_DETAIL;
extern UIID UITEXT_CONSOLE_LEADERBOARDS_CONQUESTS_DESCRIPTION_TEXT;
extern UIID UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_DESCRIPTION_ICON;
extern UIID UITEXT_CONSOLE_LEADERBOARDS_CONQUESTS_DESCRIPTION_VALUE;
extern UIID UIPROGRESS_CONSOLE_LEADERBOARDS_CONQUESTS_DETAILVIEW_PROGRESS_BAR;
namespace UILeaderboards { namespace Console {

void UpdateConquestDescription(int32 nSelectedCategory)
{
    const OnlineService::AchievementDefinition * ptAchievementDefinition = sgtAppGlobals.ptAchievements->GetAchievementDefinition(sgtUILeaderboardData.arConquestAchievementIds[nSelectedCategory]);
    CRefString sAchievementTitle = StringListFetch(ptAchievementDefinition->m_pszNameKey);
    CRefString sAchievementDesc = StringListFetch(ptAchievementDefinition->m_pszDescriptionKey);
    CRefString sAchievementIcon;
    sAchievementIcon.Format("UI:%s", ptAchievementDefinition->m_pszIconTexture);
    ImageTextureFrame tAchievementIcon(sAchievementIcon);
    CRefString sDescription = LocalizedSprintf(StringListFetch("ConsoleAchievements:AchievementDescription").str(), sAchievementTitle.str(), sAchievementDesc.str(), NULL);
    UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_CONQUESTS_DESCRIPTION_TEXT).SetText(sDescription, FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_DESCRIPTION_ICON).SetTextureFrame(tAchievementIcon);
    uint32 nPoints = ptAchievementDefinition->m_pointsValue;
    UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_CONQUESTS_DESCRIPTION_VALUE).SetText(UnicodeText::FormatNumber((int32)nPoints, TRUE, FNS_NORMAL, (XLocale)0), FALSE);
}
UIID GetElementUIIDForConquestsCategory(int32 nIndex);  // defined at the end of this file under #line 1952 (DWARF UIConsoleLeaderboards.cpp:1952)
// Switch on eTab (2243 col 16 = the condition of `switch(`); case bodies in DWARF line order: rifts 2247, conquests 2257,
// challenges 2267 (the original default path). Conquests use the GetElementUIIDForConquestsCategory inline (DWARF call line 2257).
int32 GetCategoryIndexForElementUIID(const UIID & id, CurrentTab eTab)
{
    switch (eTab)
    {
    case CURRENT_TAB_GREATER_RIFTS:
        for (uint32 i = 0; i < 10; i++)
        {
            if (id == GetElementUIIDForGreaterRiftsCategory(i))
                return i;
        }
        break;
    case CURRENT_TAB_CONQUESTS:
        for (uint32 i = 0; i < 10; i++)
        {
            if (id == GetElementUIIDForConquestsCategory(i))
                return i;
        }
        break;
    default:
        for (uint32 i = 0; i < 5; i++)
        {
            if (id == GetElementUIIDForChallengesCategory(i))
                return i;
        }
        break;
    }
    return -1;

}

void UpdatePlayerScore();
int32 MakeGameKey(CurrentTab eScopeKey, int32 nLeaderboardIndex, BOOL bHardcore); // Declaration only; defined at DWARF line 2993 below.
inline CRefString GetConquestLeaderboardName(int32 nIndex)
{
    if ((size_t)nIndex < sgtUILeaderboardData.arConquestAchievementIds.size())
    {
        const OnlineService::AchievementDefinition * ptAchievementDefinition = sgtAppGlobals.ptAchievements->GetAchievementDefinition(sgtUILeaderboardData.arConquestAchievementIds[nIndex]);
        return StringListFetch(ptAchievementDefinition->m_pszNameKey);
    }
    return CRefString("Missing Achievement");
}

void IconSelectionChanged(const ::Console::RingWrapper & tRingWrapper, const UIID & idIcon, const BOOL bSelected)
{
    int32 nSelectedCategory = GetCategoryIndexForElementUIID(idIcon, sgtUILeaderboardData.eCurrentTab);
    sgtUILeaderboardData.nSelectedCategory = nSelectedCategory;
    if (nSelectedCategory != -1)
        UI_GetButtonFromID(idIcon).SetButtonState(bSelected ? BS_PUSHED : BS_NORMAL, FALSE, FALSE);

    if (sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_GREATER_RIFTS)
    {
        UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_RING_TITLE).SetText(GetGreaterRiftLeaderboardName(nSelectedCategory), FALSE);
        UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_TITLE).SetText(GetGreaterRiftLeaderboardName(nSelectedCategory), FALSE);
    }
    else if (sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_CONQUESTS)
    {
        UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_RING_TITLE).SetText(GetConquestLeaderboardName(nSelectedCategory), FALSE);
        UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_TITLE).SetText(GetConquestLeaderboardName(nSelectedCategory), FALSE);
        if (bSelected)
        {
            UpdateConquestHardcoreDisplay(nSelectedCategory);
            UpdateConquestDescription(nSelectedCategory);
        }
    }
    else
    {
        UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_RING_TITLE).SetText(GetChallengeLeaderboardName(nSelectedCategory), FALSE);
        UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_TITLE).SetText(GetChallengeLeaderboardName(nSelectedCategory), FALSE);
        if (bSelected)
        {
            UpdateChallengesRingDisplay(nSelectedCategory);
            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        }
    }
    UpdatePlayerScore();
    SoundManager::PlaySound(GlobalSNOGet((GlobalSNO)0x395), cgtPSODefaults);
}

BOOL CreateLeaderboardKeyForSelection(LeaderboardKeyAndPartition & tKey, CurrentTab eCurrentTab, int32 nIndex, BOOL bHardcore, BOOL bPreviousResults); // DWARF owner:3503.






































































void sFixFontSize(const UIID & idWindow);

void EnableSeasonChallengeScreen(BOOL bEnable, BOOL bWasActiveAlready)
{
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_SEASON_CHALLENGES_NAVIGATION).SetActive(bEnable);
    if (bEnable)
        sFixFontSize(UIWND_CONSOLE_LEADERBOARDS_SEASON_CHALLENGES_NAVIGATION);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING).SetActive(bEnable);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING_GLOW).SetActive(bEnable && sgtUILeaderboardData.bChallengeRifts);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING_CURSOR).SetActive(bEnable && sgtUILeaderboardData.bChallengeRifts);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING_ICON).SetActive(bEnable && sgtUILeaderboardData.bChallengeRifts);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_SEASON_RING_GLOW).SetActive(bEnable && !sgtUILeaderboardData.bChallengeRifts);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_SEASON_RING_CURSOR).SetActive(bEnable && !sgtUILeaderboardData.bChallengeRifts);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING_SEASON_ICON).SetActive(bEnable && !sgtUILeaderboardData.bChallengeRifts);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING_DETAIL).SetActive(bEnable);

    if (bEnable)
    {
        sgtUILeaderboardData.tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING, UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_SEASON_RING_CURSOR, RingIncludeControlPredicate, FALSE);
        sgtUILeaderboardData.tRingUI.SetEnabled(TRUE);
        if (!bWasActiveAlready)
        {
            int32 nIndex = sgtUILeaderboardData.bOpenedFromRiftComplete ? sgtUILeaderboardData.nParticipants : 0;
            sgtUILeaderboardData.bOpenedFromRiftComplete = FALSE;
            UIID idControl = GetElementUIIDForChallengesCategory(nIndex);
            sgtUILeaderboardData.tRingUI.SelectControl(idControl, IconSelectionChanged);
        }
        else
        {
            sgtUILeaderboardData.tRingUI.SelectControl(sgtUILeaderboardData.tRingUI.GetSelectedControlID(), IconSelectionChanged);
        }
        sgtUILeaderboardData.tFilterSelector.SetSelectedIndex(-1, FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_COLUMN_HEADERS).SetActive(FALSE);
        sgtUILeaderboardData.eUINavigationState = UISTATE_RING_NAVIGATION;
    }
    UpdateRingIcons(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING, sgtUILeaderboardData.eUINavigationState == UISTATE_RING_NAVIGATION);
}

void EnableGreaterRiftsScreen(BOOL bEnable, BOOL bWasActiveAlready)
{
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_NAVIGATION).SetActive(bEnable);
    if (bEnable)
        sFixFontSize(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_NAVIGATION);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING).SetActive(bEnable);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_DETAIL).SetActive(bEnable);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_GLOW).SetActive(bEnable && !sgtUILeaderboardData.bHardcoreGreaterRifts);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_GLOW_HARDCORE).SetActive(bEnable && sgtUILeaderboardData.bHardcoreGreaterRifts);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_HARDCORE).SetActive(bEnable && sgtUILeaderboardData.bHardcoreGreaterRifts);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_LEAF_HARDCORE).SetActive(bEnable && sgtUILeaderboardData.bHardcoreGreaterRifts);

    if (bEnable)
    {
        if (sgtUILeaderboardData.bHardcoreGreaterRifts)
            sgtUILeaderboardData.tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_CURSOR_HARDCORE, RingIncludeControlPredicate, FALSE);
        else
            sgtUILeaderboardData.tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_CURSOR, RingIncludeControlPredicate, FALSE);
        sgtUILeaderboardData.tRingUI.SetEnabled(TRUE);
        if (!bWasActiveAlready)
        {
            int32 defaultCategory = 0;
            Player * ptPlayer = PlayerGetByControllerIndex(sgtUILeaderboardData.nControllerIndex);
            if (ptPlayer)
            {
                PlayerClass playerClass = PlayerGetPlayerClass(ptPlayer);
                switch (playerClass)
                {
                case PLAYER_CLASS_DEMONHUNTER: defaultCategory = 5; break;
                case PLAYER_CLASS_BARBARIAN: defaultCategory = 3; break;
                case PLAYER_CLASS_WIZARD: defaultCategory = 8; break;
                case PLAYER_CLASS_WITCHDOCTOR: defaultCategory = 7; break;
                case PLAYER_CLASS_MONK: defaultCategory = 6; break;
                case PLAYER_CLASS_CRUSADER: defaultCategory = 4; break;
                default: break;
                }
            }
            if (sgtUILeaderboardData.bOpenedFromRiftComplete && sgtUILeaderboardData.nParticipants > 1)
            {
                uint32 nTeamCategory = sgtUILeaderboardData.nParticipants - 2;
                if (nTeamCategory <= 2)
                    defaultCategory = nTeamCategory;
            }
            sgtUILeaderboardData.bOpenedFromRiftComplete = FALSE;
            UIID idControl = GetElementUIIDForGreaterRiftsCategory(defaultCategory);
            sgtUILeaderboardData.tRingUI.SelectControl(idControl, IconSelectionChanged);
        }
        else
        {
            sgtUILeaderboardData.tRingUI.SelectControl(sgtUILeaderboardData.tRingUI.GetSelectedControlID(), IconSelectionChanged);
        }
        sgtUILeaderboardData.tFilterSelector.SetSelectedIndex(-1, FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_COLUMN_HEADERS).SetActive(FALSE);
        sgtUILeaderboardData.eUINavigationState = UISTATE_RING_NAVIGATION;
    }
    UpdateRingIcons(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING, sgtUILeaderboardData.eUINavigationState == UISTATE_RING_NAVIGATION);
}

// Inference: no idControl local in the DWARF; SelectControl takes the GetElementUIIDForConquestsCategory(0) temporary (inline call line 2224, SelectControl at 2223 col 50).
void EnableConquestsScreen(BOOL bEnable, BOOL bWasActiveAlready)
{
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_NAVIGATION).SetActive(bEnable);
    if (bEnable)
        sFixFontSize(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_NAVIGATION);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING).SetActive(bEnable);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING_DETAIL).SetActive(bEnable);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING_GLOW).SetActive(bEnable);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_DESCRIPTION).SetActive(bEnable);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING_LEAF_HARDCORE).SetActive(bEnable && sgtUILeaderboardData.bHardcoreConquests);

    if (bEnable)
    {
        sgtUILeaderboardData.tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING, UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING_CURSOR, RingIncludeControlPredicate, FALSE);
        sgtUILeaderboardData.tRingUI.SetEnabled(TRUE);
        if (!bWasActiveAlready)
        {
            sgtUILeaderboardData.tRingUI.SelectControl(
                GetElementUIIDForConquestsCategory(0), IconSelectionChanged);
        }
        else
        {
            sgtUILeaderboardData.tRingUI.SelectControl(sgtUILeaderboardData.tRingUI.GetSelectedControlID(), IconSelectionChanged);
        }
        sgtUILeaderboardData.tFilterSelector.SetSelectedIndex(-1, FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_COLUMN_HEADERS).SetActive(FALSE);
        sgtUILeaderboardData.eUINavigationState = UISTATE_RING_NAVIGATION;
    }
    UpdateRingIcons(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING, sgtUILeaderboardData.eUINavigationState == UISTATE_RING_NAVIGATION);
}

static inline const ScoreList & sGetCurrentScores()
{
    LeaderboardDataSource & tData = sgtUILeaderboardData.tLeaderboardData;
    return tData.eCurrentFilter == LEADERBOARD_FILTER_TOP_GLOBAL ? tData.tTopGlobalLeaderboardScores
        : tData.eCurrentFilter == LEADERBOARD_FILTER_GLOBAL_POSITION ? tData.tGlobalPositionLeaderboardScores
        : tData.tFriendsLeaderboardScores;
}

void UpdateControllerActions(BOOL bDelayRefresh)
{
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();
    switch (sgtUILeaderboardData.eUINavigationState)
    {
    case UISTATE_RING_NAVIGATION:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT, StringListFetch("ConsoleUI:Select").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
        if (sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_GREATER_RIFTS)
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_TOGGLE_HARDCORE, StringListFetch("ConsoleLeaderboards:ToggleHardcore").str());
        else if (sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_SEASON_CHALLENGE && sgtUILeaderboardData.nSelectedCategory != 0)
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_TOGGLE_LAST_WEEK, sgtUILeaderboardData.bLastWeeklyChallenge ? StringListFetch("ConsoleLeaderboards:ToggleCurrentWeek").str() : StringListFetch("ConsoleLeaderboards:ToggleLastWeek").str());
        else if (sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_CONQUESTS)
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_CONQUEST_DETAILS, StringListFetch("ConsoleLeaderboards:Details").str());
        break;
    case UISTATE_FILTERS_NAVIGATION:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        if (sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_GREATER_RIFTS && sgtUILeaderboardData.tLeaderboardData.GetNumItems() > 0 && !sgtUILeaderboardData.bLeaderboardRequestIssued[sgtUILeaderboardData.eLeaderboardFilter])
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_INSPECT, StringListFetch("ConsoleLeaderboards:Inspect").str());
        if (bDelayRefresh)
            tActionManager.SetDelayedAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_REFRESH, StringListFetch("ConsoleLeaderboards:Refresh").str(), 5);
        else
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_REFRESH, StringListFetch("ConsoleLeaderboards:Refresh").str());
        if (sgtUILeaderboardData.tLeaderboardData.GetNumItems() > 0 && !sgtUILeaderboardData.bLeaderboardRequestIssued[sgtUILeaderboardData.eLeaderboardFilter])
        {
            int32 nIndex = sgtUILeaderboardData.tLeaderboardData.nCurrentIndex[sgtUILeaderboardData.tLeaderboardData.eCurrentFilter];
            if (nIndex >= 0 && (size_t)nIndex < sGetCurrentScores().size())
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_VIEW_GAMERCARD, StringListFetch("Console:PlayerOptionViewUserPageNX64").str());
        }
        break;
    case UISTATE_INSPECT:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_INSPECT, StringListFetch("ConsoleLeaderboards:Inspect").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
    case UISTATE_CONQUEST_DETAILS:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        break;
    default:
        break;
    }
    ::Console::Screens::UpdateButtonLegendText();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(FALSE);
}

#line 3577 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void EnableInspectMode(
    BOOL bEnable)
{
    if (bEnable)
    {
        if (sgtUILeaderboardData.tLeaderboardData.nCurrentIndex[sgtUILeaderboardData.tLeaderboardData.eCurrentFilter] < 0 || sgtUILeaderboardData.tLeaderboardData.nCurrentIndex[sgtUILeaderboardData.tLeaderboardData.eCurrentFilter] >= sgtUILeaderboardData.tLeaderboardData.GetCurrentLeaderboard().size())
        {
            return;
        }
        sgtUILeaderboardData.eUINavigationState = UISTATE_INSPECT;
        UI_SetControllerFocus(GetUIIDNone());

        BOOL bHardcore = sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_GREATER_RIFTS && sgtUILeaderboardData.bHardcoreGreaterRifts;
        LeaderboardKeyAndPartition tKey;
        if (!CreateLeaderboardKeyForSelection(tKey, sgtUILeaderboardData.eCurrentTab, sgtUILeaderboardData.nSelectedCategory, bHardcore, FALSE))
        {
            return;
        }

        size_t leaderBoardIndex = sgtUILeaderboardData.tLeaderboardData.nCurrentIndex[sgtUILeaderboardData.tLeaderboardData.eCurrentFilter];
        const D3::Leaderboard::Score & tScore = sgtUILeaderboardData.tLeaderboardData.GetCurrentLeaderboard()[leaderBoardIndex];

        tKey.m_id = LeaderboardGetLeaderboardIdForConsoleMetadata(tKey.m_id, tScore);

        PlayerClass ePlayerClass = PLAYER_CLASS_NONE;

        if (tScore.has_metadata() &&
            tScore.metadata().team_member_size() > 0 &&
            tScore.metadata().team_member(0).has_hero_gbid_class())
        {
            ePlayerClass = PlayerClassFromGBID(tScore.metadata().team_member(0).hero_gbid_class());
        }

        UILeaderboardInspect::Console::Open(sgtUILeaderboardData.nControllerIndex, tKey, tScore.game_account_id(), ePlayerClass);
    }

    else
    {
        UI_SetControllerFocus(GetCurrentList().GetID());
        sgtUILeaderboardData.eUINavigationState = UISTATE_FILTERS_NAVIGATION;
    }

    sgtUILeaderboardData.bInspectEnabled = bEnable;

    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_FILTERS_WRAPPER).SetActive(!bEnable);
    GetCurrentList().SetActive(!bEnable);
}
#line 1339 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
BOOL GetPlayerScoreForCurrentLeaderboard(D3::Leaderboard::Score & tScore, BOOL bPreviousSeason)
{
    const CurrentTab eCurrentTab = sgtUILeaderboardData.eCurrentTab;
    int32 nCategory = sgtUILeaderboardData.nSelectedCategory;
    BOOL bHardcore = eCurrentTab == CURRENT_TAB_GREATER_RIFTS && sgtUILeaderboardData.bHardcoreGreaterRifts;
    int32 nGameKey = MakeGameKey(eCurrentTab, nCategory, bHardcore);
    if (bPreviousSeason)
    {
        blz::map<int, D3::Leaderboard::Score>::iterator iter = sgtUILeaderboardData.mapPlayerScoresPreviousSeason.find(nGameKey);
        if (iter == sgtUILeaderboardData.mapPlayerScoresPreviousSeason.end())
            return FALSE;
        tScore = iter->second;
    }
    else
    {
        blz::map<int, D3::Leaderboard::Score>::iterator iter = sgtUILeaderboardData.mapPlayerScores.find(nGameKey);
        if (iter == sgtUILeaderboardData.mapPlayerScores.end())
            return FALSE;
        tScore = iter->second;
    }
    return TRUE;
}
void PopulateProgressBar(OnlineService::CriteriaQuantity nEarned, OnlineService::CriteriaQuantity nTotal)
{
    CRefString strProgress = LocalizedSprintf(StringListFetch("Bnet_Achievements:AchievementProgress").str(), UnicodeText::FormatNumber((int32)nEarned, TRUE, FNS_NORMAL, (XLocale)0).str(), UnicodeText::FormatNumber((int32)nTotal, TRUE, FNS_NORMAL, (XLocale)0).str(), NULL);
    UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_DETAILVIEW_PROGRESS_TEXT).SetText(strProgress, FALSE);
    UI_GetProgressBarFromID(UIPROGRESS_CONSOLE_LEADERBOARDS_CONQUESTS_DETAILVIEW_PROGRESS_BAR).SetExtentsAndPosition(0.0f, (float)nTotal, (float)nEarned, -1.0f);
}

} }

namespace UILeaderboards { namespace Console {
#line 96 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
static const int32 MAX_LEADERBOARD_SIZE = 50;
#line 2715 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
int32 TrimLeaderboard(
    ScoreList & tLeaderboard,
    LeaderboardFilters eFilter,
    BOOL bUp)
{
    if (tLeaderboard.size() > MAX_LEADERBOARD_SIZE)
    {
        int32 nAmountToTrim = tLeaderboard.size() - MAX_LEADERBOARD_SIZE;

        if (bUp)
        {
            tLeaderboard.erase(tLeaderboard.end() - nAmountToTrim, tLeaderboard.end());
        }

        else
        {
            tLeaderboard.erase(tLeaderboard.begin(), tLeaderboard.begin() + nAmountToTrim);
            return -nAmountToTrim;
        }
    }

    return 0;
}
} }

// Raw LobbyService.cpp:4551 only; no recovered header declaration.
namespace Console { namespace Online { void LeaderboardGetScoresByRange(int32 nUserIndex, const LeaderboardKeyAndPartition & idLeaderboard, uint32 uFirstRank, uint32 uMaxResults, LeaderboardGetScoresCallback pfnCallback); } }
namespace UILeaderboards { namespace Console {
#line 98 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
static const int32 NUM_ADDITIONAL_ENTRIES_TO_RETRIEVE = 25;
#line 365 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
inline ScoreList & LeaderboardDataSource::GetLeaderboard(
    LeaderboardFilters eFilter)
{
    return eFilter == LEADERBOARD_FILTER_TOP_GLOBAL ? tTopGlobalLeaderboardScores : eFilter == LEADERBOARD_FILTER_GLOBAL_POSITION ? tGlobalPositionLeaderboardScores : tFriendsLeaderboardScores;
}
#line 2740 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void sAdditionalLeaderboardRequestCallback(
    LeaderboardFilters eFilter,
    BOOL bUp,
    ::Console::Online::LeaderboardResult eResult,
    const ScoreList & atScores)
{
    sgtUILeaderboardData.bLeaderboardRequestIssued[eFilter] = FALSE;

    if (!sgtUILeaderboardData.bGlobalLeaderboardsEnabled && eFilter != LEADERBOARD_FILTER_FRIENDS)
        return;



    if (sgtUILeaderboardData.bInspectEnabled || !IsVisible())
        return;



    if (sgtUILeaderboardData.eUINavigationState != UISTATE_FILTERS_NAVIGATION)
        return;



    if (eResult != ::Console::Online::LeaderboardResult::SUCCESS)
    {
        PopupLeaderboardError(eResult);
    }


    ScoreList & tLeaderboard = sgtUILeaderboardData.tLeaderboardData.GetLeaderboard(eFilter);

    int32 nIndexOffset = 0;

    if (bUp)
    {
        tLeaderboard.insert(tLeaderboard.begin(), atScores.begin(), atScores.end());
        nIndexOffset = atScores.size();
    }
    else
    {
        tLeaderboard.insert(tLeaderboard.end(), atScores.begin(), atScores.end());
    }

    nIndexOffset += TrimLeaderboard(tLeaderboard, eFilter, bUp);

    sgtUILeaderboardData.tLastUpdatedTicks = AppGetTicks();
    UpdateLeaderboardDisplay(eFilter, TRUE, nIndexOffset);
}
#line 2790 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void IssueAdditionalLeaderboardRequest(
    LeaderboardFilters eFilter,
    int32 nCurrentEndIndex,
    BOOL bUp)
{
    if (eFilter != LEADERBOARD_FILTER_FRIENDS && !sgtUILeaderboardData.bGlobalLeaderboardsEnabled)
        return;



    if (sgtUILeaderboardData.bLeaderboardRequestIssued[eFilter])
        return;



    BOOL bHardcore = sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_GREATER_RIFTS && sgtUILeaderboardData.bHardcoreGreaterRifts;
    LeaderboardKeyAndPartition tKey;

    BOOL bLastWeeksResults = sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_SEASON_CHALLENGE && sgtUILeaderboardData.nSelectedCategory > 0 && sgtUILeaderboardData.bLastWeeklyChallenge;

    if (!CreateLeaderboardKeyForSelection(tKey, sgtUILeaderboardData.eCurrentTab, sgtUILeaderboardData.nSelectedCategory, bHardcore, bLastWeeksResults))
        return;



    int32 nNumToRequest = NUM_ADDITIONAL_ENTRIES_TO_RETRIEVE;
    int32 nStartIndex;
    if (bUp)
    {
        nStartIndex = nCurrentEndIndex - NUM_ADDITIONAL_ENTRIES_TO_RETRIEVE - 1;
        if (nStartIndex < 0)
        {
            nStartIndex = 0;

            nNumToRequest = nCurrentEndIndex - 1;
        }
    }

    else
    {
        nStartIndex = nCurrentEndIndex + 1;
    }

    switch (eFilter)
    {
    case LEADERBOARD_FILTER_GLOBAL_POSITION:
    case LEADERBOARD_FILTER_TOP_GLOBAL:
        ::Console::Online::LeaderboardGetScoresByRange(sgtUILeaderboardData.nControllerIndex, tKey, nStartIndex, nNumToRequest, OnlineService::internal::function_binder_4<void, LeaderboardFilters, BOOL, ::Console::Online::LeaderboardResult, const ScoreList &>::bind<&sAdditionalLeaderboardRequestCallback>(eFilter, bUp));
        sgtUILeaderboardData.bLeaderboardRequestIssued[eFilter] = TRUE;
        break;
    default:
        break;
    }

}
} }

namespace UILeaderboards { namespace Console {
#line 1000 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
UIConsoleListBox & GetListForFilter(
    LeaderboardFilters eFilter)
{
    switch (eFilter)
    {
    case LEADERBOARD_FILTER_FRIENDS:
        return UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LEADERBOARDS_ENTRY_LIST_FRIENDS);

    case LEADERBOARD_FILTER_GLOBAL_POSITION:
        return UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LEADERBOARDS_ENTRY_LIST_GLOBAL);


    default:
        return UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LEADERBOARDS_ENTRY_LIST_TOP_GLOBAL);
    }
}
#line 2517 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void UpdateLeaderboardDisplay(
    LeaderboardFilters eFilter,
    BOOL bForce,
    int32 nIndexOffset)
{
    int32 nIndexToSelect = sgtUILeaderboardData.tLeaderboardData.nCurrentIndex[eFilter];

    nIndexToSelect += nIndexOffset;

    if (nIndexToSelect == -1)
    {
        int32 nPlayerPositionIndex = sgtUILeaderboardData.tLeaderboardData.nPlayerPositionRank[eFilter];

        nIndexToSelect = nPlayerPositionIndex > 0 ? nPlayerPositionIndex - 1 : -1;
    }




    if (nIndexToSelect == -1 || nIndexToSelect > sgtUILeaderboardData.tLeaderboardData.GetNumItems())
        nIndexToSelect = 0;



    int32 nFirstVisible = GetListForFilter(eFilter).GetFirstVisibleItemIndex() + nIndexOffset;

    if (GetListForFilter(eFilter).GetSelectedIndex() != nIndexToSelect || bForce)
    {
        GetListForFilter(eFilter).ResetAllItems();
        GetListForFilter(eFilter).SetFirstVisibleItemIndex(nFirstVisible);
        GetListForFilter(eFilter).SetSelectedIndex(nIndexToSelect);
    }

    GetListForFilter(eFilter).RefreshVisibleItems();

    sgtUILeaderboardData.tLeaderboardData.nCurrentIndex[eFilter] = nIndexToSelect;

    if (eFilter != sgtUILeaderboardData.eLeaderboardFilter)
        return;



    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LEADERBOARDS_ENTRY_LIST_FRIENDS).SetActive(FALSE);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LEADERBOARDS_ENTRY_LIST_GLOBAL).SetActive(FALSE);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LEADERBOARDS_ENTRY_LIST_TOP_GLOBAL).SetActive(FALSE);


    if (sgtUILeaderboardData.tLeaderboardData.GetNumItems() == 0)
    {
        UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_MESSAGE).SetText(StringListFetch("ConsoleLeaderboards:NoEntries"), FALSE);
        UI_GetWindowFromID(UITEXT_CONSOLE_LEADERBOARDS_MESSAGE).SetActive(TRUE);

        UI_SetControllerFocus(GetUIIDNone());
        UpdateLeaderboardColumnHeaders(FALSE);
    }

    else
    {
        GetCurrentList().SetActive(TRUE);
        UI_SetControllerFocus(GetCurrentList().GetID());

        UI_GetWindowFromID(UITEXT_CONSOLE_LEADERBOARDS_MESSAGE).SetActive(FALSE);

        UpdateLeaderboardColumnHeaders(TRUE);
    }
}
} }

namespace UILeaderboards { namespace Console {
#line 2684 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
int32 GetPlayerIndexForLeaderboard(
    bdOnlineUserID idUser,
    const ScoreList & atScores)
{
    for (int32 n = 0; n < atScores.size(); ++n)
    {
        const D3::Leaderboard::Score & tStat = atScores[n];
        if (tStat.game_account_id() == idUser)
            return n;
    }

    return 0;


}
#line 2701 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
int32 GetPlayerRankForLeaderboard(
    bdOnlineUserID idUser,
    const ScoreList & atScores)
{
    BOOL bPrevious = sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_SEASON_CHALLENGE && sgtUILeaderboardData.nSelectedCategory != 0 && sgtUILeaderboardData.bLastWeeklyChallenge;
    D3::Leaderboard::Score tScore;
    if (GetPlayerScoreForCurrentLeaderboard(tScore, bPrevious))
    {
        return tScore.score_band();
    }

    return 0;
}
#line 2847 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void sLeaderboardRequestCallback(
    LeaderboardFilters eFilter,
    ::Console::Online::LeaderboardResult eResult,
    const ScoreList & atScores)
{
    sgtUILeaderboardData.bLeaderboardRequestIssued[eFilter] = FALSE;

    if (!sgtUILeaderboardData.bGlobalLeaderboardsEnabled && eFilter != LEADERBOARD_FILTER_FRIENDS)
        return;



    if (sgtUILeaderboardData.bInspectEnabled || !IsVisible())
        return;



    if (sgtUILeaderboardData.eUINavigationState != UISTATE_FILTERS_NAVIGATION)
        return;



    if (eResult == ::Console::Online::LeaderboardResult::SUCCESS &&
        atScores.size() > 0)
    {
        switch (eFilter)
        {
        case LEADERBOARD_FILTER_FRIENDS:
            sgtUILeaderboardData.tLeaderboardData.tFriendsLeaderboardScores = atScores;
            break;
        case LEADERBOARD_FILTER_GLOBAL_POSITION:
            sgtUILeaderboardData.tLeaderboardData.tGlobalPositionLeaderboardScores = atScores;
            break;
        case LEADERBOARD_FILTER_TOP_GLOBAL:
            sgtUILeaderboardData.tLeaderboardData.tTopGlobalLeaderboardScores = atScores;
            break;
        default: break;
        }

        sgtUILeaderboardData.tLeaderboardData.nPlayerPositionRank[eFilter] = GetPlayerRankForLeaderboard(sgtUILeaderboardData.nUserId, atScores);
        sgtUILeaderboardData.tLastUpdatedTicks = AppGetTicks();


        int32 nIndex = GetPlayerIndexForLeaderboard(sgtUILeaderboardData.nUserId, atScores);

        sgtUILeaderboardData.tLeaderboardData.nCurrentIndex[eFilter] = nIndex;


        UpdateLeaderboardDisplay(eFilter, TRUE, 0);
        UpdateControllerActions(TRUE);
    }
    else
    {
        switch (eFilter)
        {
        case LEADERBOARD_FILTER_FRIENDS:
            sgtUILeaderboardData.tLeaderboardData.tFriendsLeaderboardScores.clear();
            break;
        case LEADERBOARD_FILTER_GLOBAL_POSITION:
            sgtUILeaderboardData.tLeaderboardData.tGlobalPositionLeaderboardScores.clear();
            break;
        case LEADERBOARD_FILTER_TOP_GLOBAL:
            sgtUILeaderboardData.tLeaderboardData.tTopGlobalLeaderboardScores.clear();
            break;
        default: break;
        }


        sgtUILeaderboardData.tLeaderboardData.nPlayerPositionRank[eFilter] = -1;
        sgtUILeaderboardData.tLastUpdatedTicks = AppGetTicks();
        UpdateLeaderboardDisplay(eFilter, TRUE, 0);
        UpdateControllerActions(FALSE);

        if (eFilter == LEADERBOARD_FILTER_TOP_GLOBAL && eResult != ::Console::Online::LeaderboardResult::SUCCESS)
        {

            PopupLeaderboardError(eResult);
        }
    }
}
} }

#include "../../Console/LobbyService.h"
namespace Console { namespace Online {
// Raw LobbyService.cpp:4521 and4537 only; no recovered header declarations.
void LeaderboardGetScoresForUsers(int32 nUserIndex, const LeaderboardKeyAndPartition & idLeaderboard, const blz::vector<bdOnlineUserID> & idUsers, LeaderboardGetScoresCallback pfnCallback, bool bIncludeSelf);
void LeaderboardGetScoresAroundPlayer(int32 nUserIndex, const LeaderboardKeyAndPartition & idLeaderboard, uint32 uMaxResults, LeaderboardGetScoresCallback pfnCallback);
// Raw PresenceManager.cpp:4146 only for this free function; no recovered header declaration.
blz::vector<bdOnlineUserID> PresenceGetFriendIDs(int32 nUserIndex);
} }
namespace UILeaderboards { namespace Console {
#line 97 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
static const int32 NUM_ENTRIES_TO_RETRIEVE = 35;
#line 2929 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void IssueLeaderboardRequest()

{

    for (int32 n = 0; n < LEADERBOARD_FILTER_COUNT; ++n)
    {
        if (sgtUILeaderboardData.bLeaderboardRequestIssued[n])
            return;
    }



    BOOL bHardcore = sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_GREATER_RIFTS && sgtUILeaderboardData.bHardcoreGreaterRifts;
    BOOL bPrevious = sgtUILeaderboardData.eCurrentTab == CURRENT_TAB_SEASON_CHALLENGE && sgtUILeaderboardData.nSelectedCategory != 0 && sgtUILeaderboardData.bLastWeeklyChallenge;

    LeaderboardKeyAndPartition tKey;

    if (!CreateLeaderboardKeyForSelection(tKey, sgtUILeaderboardData.eCurrentTab, sgtUILeaderboardData.nSelectedCategory, bHardcore, bPrevious))
        return;




    BOOL bIssued = FALSE;


    for (int32 n = 0; n < LEADERBOARD_FILTER_COUNT; ++n)
    {
        if (n != LEADERBOARD_FILTER_FRIENDS && !sgtUILeaderboardData.bGlobalLeaderboardsEnabled)
            continue;



        sgtUILeaderboardData.bLeaderboardRequestIssued[n] = TRUE;

        bIssued = TRUE;



        switch (n)
        {
        case LEADERBOARD_FILTER_TOP_GLOBAL:
            ::Console::Online::LeaderboardGetScoresByRange(sgtUILeaderboardData.nControllerIndex, tKey, 1, NUM_ENTRIES_TO_RETRIEVE, OnlineService::internal::function_bind_1_2<void, LeaderboardFilters, ::Console::Online::LeaderboardResult, const ScoreList &, &sLeaderboardRequestCallback>((LeaderboardFilters)n));
            break;
        case LEADERBOARD_FILTER_GLOBAL_POSITION:
            ::Console::Online::LeaderboardGetScoresAroundPlayer(sgtUILeaderboardData.nControllerIndex, tKey, NUM_ENTRIES_TO_RETRIEVE, OnlineService::internal::function_bind_1_2<void, LeaderboardFilters, ::Console::Online::LeaderboardResult, const ScoreList &, &sLeaderboardRequestCallback>((LeaderboardFilters)n));
            break;
        case LEADERBOARD_FILTER_FRIENDS:
            ::Console::Online::LeaderboardGetScoresForUsers(sgtUILeaderboardData.nControllerIndex, tKey, ::Console::Online::PresenceGetFriendIDs(sgtUILeaderboardData.nControllerIndex), OnlineService::internal::function_bind_1_2<void, LeaderboardFilters, ::Console::Online::LeaderboardResult, const ScoreList &, &sLeaderboardRequestCallback>((LeaderboardFilters)n), true);
            break;
        default: break;
        }
    }




    if (bIssued && !::Console::Online::LobbyService::IsUserConnected(sgtUILeaderboardData.nControllerIndex))
    {
        ::Console::Online::LobbyService::AttemptConnectOnUser(sgtUILeaderboardData.nControllerIndex);
    }

}
} }

// These APIs have only CPP definition records in recovered DWARF; no header declaration records were recovered.
BOOL PlayerIsHardcore(const Player * ptPlayer); // Player.cpp:5360.
namespace Console { namespace Online {
void GetChallengeRiftConfig(int32, OnlineService::Callback<void (StorageResult, const D3::ChallengeRifts::ChallengeData &)>); // LobbyService.cpp:4609.
BOOL AreSeasonalGlobalLeaderboardsEnabled(); // LobbyService.cpp:4583.
} }
namespace UILeaderboards { namespace Console {
void IssuePlayerPositionRequest(); // Same owning CPP:3105.

#line 598 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
inline void UILeaderboardData::Initialize()
{

    if (bInitialized) return;

    tRingStickInputFilter.AddRule(::Console::AnalogStickInputFilter::Rule::CatchAllRule);

    tFilterSelector.WrapUsingContainerControl(UIWND_CONSOLE_LEADERBOARDS_FILTERS_WRAPPER);
    tFilterSelector.SetCallbacks(sFilterSelectionPredicate, sFilterSelectionChanged);

    bInitialized = TRUE;
}

#line 871 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void sChallengeLeaderboardRequestCallback(
    ::Console::Online::StorageResult eResult,
    const D3::ChallengeRifts::ChallengeData & tChallengeConfig)
{
    if (!IsVisible())
    {
        return;
    }

    if (eResult == ::Console::Online::STORAGE_SUCCESS && tChallengeConfig.has_challenge_number())
    {
        sgtUILeaderboardData.nChallengeNumber = tChallengeConfig.challenge_number();
    }

    IssuePlayerPositionRequest();
}

#line 897 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void OpenForController(
    const int32 nControllerIndex)
{
    GameSoftPause(TRUE);

    sgtUILeaderboardData.tLeaderboardData.InitializeData();

    ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_LEADERBOARDS, TRUE, GetUIIDNone());

    sgtUILeaderboardData.nUserId = ::Console::Online::LobbyService::Get(nControllerIndex)->GetOnlineUserID();

    sgtUILeaderboardData.Initialize();

    sgtUILeaderboardData.GetConquestsForSeason();

    sgtUILeaderboardData.nControllerIndex = nControllerIndex;
    sgtUILeaderboardData.idPlayer = ::Console::ClientAchievementsInterface::UserIndexToGameAccount(nControllerIndex);
    sgtUILeaderboardData.nChallengeNumber = -1;
    sgtUILeaderboardData.bLastWeeklyChallenge = FALSE;

    ::Console::Online::GetChallengeRiftConfig(nControllerIndex, OnlineService::internal::function_bind_0_2<void, ::Console::Online::StorageResult, const D3::ChallengeRifts::ChallengeData &, &sChallengeLeaderboardRequestCallback>());




    if (!::Console::Online::LobbyService::IsUserConnected(sgtUILeaderboardData.nControllerIndex))
    {
        ::Console::Online::LobbyService::AttemptConnectOnUser(sgtUILeaderboardData.nControllerIndex);
    }


    if (GameIsStartUp())
    {
        ::Console::Online::GameParams tParams;
        if (::Console::Online::PartyGetGameParams(tParams))
        {
            sgtUILeaderboardData.bHardcoreGreaterRifts = tParams.m_bIsHardcore;
        }
        else
        {
            sgtUILeaderboardData.bHardcoreGreaterRifts = FALSE;
        }
    }
    else
    {
        sgtUILeaderboardData.bHardcoreGreaterRifts = PlayerIsHardcore(PlayerGetByControllerIndex(nControllerIndex));
    }

    sgtUILeaderboardData.bGlobalLeaderboardsEnabled = ::Console::Online::AreSeasonalGlobalLeaderboardsEnabled();

    if (UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_INVENTORY).IsActive() || UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_INSPECT_SKILLS).IsActive())
    {
        UILeaderboardInspect::Console::ForceClose();
        sgtUILeaderboardData.bInspectEnabled = FALSE;
    }

    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_FILTERS).SetActive(sgtUILeaderboardData.bGlobalLeaderboardsEnabled);

    if (sgtUILeaderboardData.bOpenedFromRiftComplete && !sgtUILeaderboardData.bWeeklyChallenge)
    {
        ActivateScreen(CURRENT_TAB_GREATER_RIFTS);
    }
    else
    {
        ActivateScreen(CURRENT_TAB_SEASON_CHALLENGE);
    }

    sgtUILeaderboardData.mapPlayerScoresPreviousSeason.clear();

    UpdatePlayerScore();

    UIID idFriendsTitle = UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_FILTER_FRIENDS, ".LayoutRoot.Filter.FilterTitle");
    UIID idGlobalPositionTitle = UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_FILTER_GLOBAL_POSITION, ".LayoutRoot.Filter.FilterTitle");
    UIID idTopGlobalTitle = UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_FILTER_TOP_GLOBAL, ".LayoutRoot.Filter.FilterTitle");

    UI_GetTextFromID(idFriendsTitle).SetText(StringListFetch("ConsoleLeaderboards:FriendsFilter"), FALSE);
    UI_GetTextFromID(idGlobalPositionTitle).SetText(StringListFetch("ConsoleLeaderboards:GlobalPositionFilter"), FALSE);
    UI_GetTextFromID(idTopGlobalTitle).SetText(StringListFetch("ConsoleLeaderboards:TopGlobalFilter"), FALSE);

    SetShoulderButtonVisibility();

    UpdateNavigationPrompt();
    UpdateControllerActions(FALSE);
}
} }

#include "../../Console/Utilities.h"
namespace UILeaderboards { namespace Console {
void sUpdateConquestDetails(const OnlineService::AchievementId & idAchievement); // Same owning CPP:1262.

#line 349 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"






#line 1335 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void ToggleConquestDetails(
    BOOL bActive)
{
    UI_GetWindowFromID(UISCROLL_CONSOLE_LEADERBOARDS_CONQUESTS_DETAILVIEW).SetActive(bActive);
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_DESCRIPTION).SetActive(!bActive);

    if (bActive)
    {
        ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
        int32 nIndex = sgtUILeaderboardData.nSelectedCategory;
        const OnlineService::AchievementDefinition * ptAchievementDefinition = tAchievements.GetAchievementDefinition(sgtUILeaderboardData.arConquestAchievementIds[nIndex]);
        sUpdateConquestDetails(ptAchievementDefinition->m_id);
    }

    UpdateNavigationPrompt();
}

#line 1475 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void OnButton4(
    const UIID & id,
    const int32 nControllerIndex)
{
    switch (sgtUILeaderboardData.eUINavigationState)
    {
    case UISTATE_RING_NAVIGATION:
        switch (sgtUILeaderboardData.eCurrentTab)
        { case CURRENT_TAB_GREATER_RIFTS:
            ToggleHardcoreGreaterRifts();
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CLICK_GENERIC), cgtPSODefaults);
            break;

        case CURRENT_TAB_SEASON_CHALLENGE:
            sgtUILeaderboardData.bLastWeeklyChallenge = !sgtUILeaderboardData.bLastWeeklyChallenge;
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CLICK_GENERIC), cgtPSODefaults);
            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
            break;

        case CURRENT_TAB_CONQUESTS:
            sgtUILeaderboardData.tRingUI.SetEnabled(FALSE);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_GLOW).SetActive(FALSE);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_GREATER_RIFTS_RING_GLOW_HARDCORE).SetActive(FALSE);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING_GLOW).SetActive(FALSE);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_RING_GLOW).SetActive(FALSE);
            UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CHALLENGES_SEASON_RING_GLOW).SetActive(FALSE);

            UpdateRingIcons(sgtUILeaderboardData.tRingUI.GetContainerControlID(), FALSE);

            sgtUILeaderboardData.eUINavigationState = UISTATE_CONQUEST_DETAILS;
            ToggleConquestDetails(TRUE);
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CLICK_GENERIC), cgtPSODefaults);
            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
            break;
        } break;
    case UISTATE_FILTERS_NAVIGATION:
    {
        int32 nFilterIndex = sgtUILeaderboardData.tLeaderboardData.nCurrentIndex[sgtUILeaderboardData.tLeaderboardData.eCurrentFilter];
        if (nFilterIndex < 0 ||
            nFilterIndex >= sgtUILeaderboardData.tLeaderboardData.GetCurrentLeaderboard().size())
        {
            return;
        }

        const D3::Leaderboard::Score & tScore = sgtUILeaderboardData.tLeaderboardData.GetCurrentLeaderboard()[nFilterIndex];


        blz::string tNameTouse = tScore.metadata().team_member_size() > 0 ? tScore.metadata().team_member(0).battle_tag() : "";

        const char * pNameToUse = tNameTouse.c_str();
        int32 nSizeToUse = tNameTouse.size();

        ::Console::Utilities::ShowProfile(nControllerIndex, tScore.game_account_id(), pNameToUse, nSizeToUse);
        break;
    }
    }
}
} }

namespace UILeaderboards { namespace Console {
int32 ConvertLeaderboardAndScopeToGameKey(int64 nLeaderboardId, int32 nScopeId); // Same owning CPP:3001, actual unresolved callee.


#line 3038 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void sPlayerPositionCallback(
    BOOL bPreviousSeason,
    ::Console::Online::LeaderboardResult eResult,
    const ScoreList & atScores)
{
    sgtUILeaderboardData.bPlayerPositionRequestIssued = FALSE;

    if (bPreviousSeason)
    {
        sgtUILeaderboardData.mapPlayerScoresPreviousSeason.clear();
    }

    if (eResult == ::Console::Online::LeaderboardResult::SUCCESS &&
        atScores.size() > 0)
    {
        if (!bPreviousSeason)
        {
            sgtUILeaderboardData.mapPlayerScores.clear();

            for (int32 n = 0; n < atScores.size(); ++n)
            {
                if (atScores[n].score_band() == 0 || atScores[n].score() == 0)
                {
                    continue;
                }

                int64 nLeaderboardId = atScores[n].has_leaderboard_id() ? atScores[n].leaderboard_id() : -1;
                int32 nScopeId = atScores[n].has_scope_id() ? atScores[n].scope_id() : -1;

                if (nLeaderboardId == -1 || nScopeId == -1)
                {
                    continue;
                }

                int32 nGameLeaderboardKey = ConvertLeaderboardAndScopeToGameKey(nLeaderboardId, nScopeId);

                sgtUILeaderboardData.mapPlayerScores[nGameLeaderboardKey] = atScores[n];
            }
        }
        else
        {
            for (int32 n = 0; n < atScores.size(); ++n)
            {
                if (atScores[n].score_band() == 0 || atScores[n].score() == 0)
                {
                    continue;
                }

                int64 nLeaderboardId = atScores[n].has_leaderboard_id() ? atScores[n].leaderboard_id() : -1;
                int32 nScopeId = atScores[n].has_scope_id() ? atScores[n].scope_id() : -1;

                if (nLeaderboardId == -1 || nScopeId == -1)
                {
                    continue;
                }

                int32 nGameLeaderboardKey = ConvertLeaderboardAndScopeToGameKey(nLeaderboardId, nScopeId);

                sgtUILeaderboardData.mapPlayerScoresPreviousSeason[nGameLeaderboardKey] = atScores[n];
            }
        }

        UpdatePlayerScore();
    }
}
} }

#include "../../Battlenet/UI/Controls/ControlVisibilityManager.h"
#include "../../../Common/Achievements/AchievementDefines.h"
namespace Achievement { CRefString DisplayStringFromEvent(EventID eID, uint64 operand, CRefString & sStringLabel); } // Only CAchievementUtils.cpp:105 recovered for this API; no header declaration record.
namespace UILeaderboards { namespace Console {
#line 1140 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void PopulateCriteriaElement(
    const UIID & idElement,
    OnlineService::CriteriaId idCriteria)
{

    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;
    const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition(idCriteria);
    const OnlineService::AchievementDefinition * ptAchievementDef;
    if (ptCritDef &&
        (ptAchievementDef = tAchievements.GetAchievementDefinition(ptCritDef->m_parentAchievementId)))
    {
        OnlineService::CriteriaRecord tCritRecord;
        BOOL fMetaAchievement = (ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_IS_ACHIEVEMENT_ID)) ? TRUE : FALSE;

        BOOL fEarned = FALSE;
        CRefString sMessage;
        if (ptAchievementsInterface)
        {


            if (!ptAchievementsInterface->FindCriteriaRecord(sgtUILeaderboardData.idPlayer, idCriteria, tCritRecord))
            {
                if (ptAchievementDef->m_nMinRequiredCriteria == 0)
                {
                    OnlineService::AchievementRecord tRecord;
                    fEarned = ptAchievementsInterface->FindAwardedAchievement(sgtUILeaderboardData.idPlayer, ptCritDef->m_parentAchievementId, tRecord);
                }
            }




            else
            {
                fEarned = tCritRecord.m_quantity >= ptCritDef->m_necessaryQuantity;
            }
        }
        if (ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_RETRIEVE_NAME_FROM_STRINGLIST))
        {
            Achievement::EventID eEventId = ptCritDef->m_advanceEvent.m_id;
            CRefString sStringLabelKey(ptCritDef->m_pszStringlabelKey);
            sMessage = Achievement::DisplayStringFromEvent(eEventId, ptCritDef->m_advanceEvent.m_comparand, sStringLabelKey);
        }
        else
        {
            sMessage = StringListFetch(ptCritDef->m_pszNameKey);
        }

        UIID idTextEarned, idTextUnearned;
        idTextEarned = UIIDCreateFromParent(idElement, ".CriteriaEarned");
        idTextUnearned = UIIDCreateFromParent(idElement, ".CriteriaUnearned");
        ControlVisibilityManager tText;
        tText.AddControl(idTextEarned);
        tText.AddControl(idTextUnearned);
        tText.SetActive(fEarned ? 0 : 1, false);
        UIID idTargetTextfield = fEarned ? idTextEarned : idTextUnearned;
        UI_GetTextFromID(idTargetTextfield).SetText(sMessage, FALSE);

        UIID idEarned, idUnearned, idChecked, idUnchecked;
        idEarned = UIIDCreateFromParent(idElement, ".Earned");
        idUnearned = UIIDCreateFromParent(idElement, ".Unearned");
        idChecked = UIIDCreateFromParent(idElement, ".Checked");
        idUnchecked = UIIDCreateFromParent(idElement, ".UnChecked");

        int32 nTargetState = (fEarned ? 0 : 1) | (fMetaAchievement ? 0 : 2);
        ControlVisibilityManager tStates;
        tStates.AddControl(idEarned);
        tStates.AddControl(idUnearned);
        tStates.AddControl(idChecked);
        tStates.AddControl(idUnchecked);
        tStates.SetActive(nTargetState, false);
    }
}
} }

#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../Battlenet/UI/Util/UIAchievementsUtil.h"
#include "../UIInit.h"
void UI_DeleteControl(const UIID & idControl, const BOOL fAssertIfDeletingRoot); // Only UI.cpp:6196 recovered for this overload; no header declaration record.
namespace UILeaderboards { namespace Console {
#line 1217 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void PopulateCriteriaDisplay(
    const OnlineService::AchievementDefinition & tDefinition)
{

    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::CriteriaIdList arCriteria;
    tAchievements.GetAchievementCriteria(tDefinition.m_id, arCriteria);
    for (uint32 i = 0; i < arCriteria.size(); ++i)
    {

        const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition(arCriteria[i]);
        if (ptCritDef == NULL || (ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_HIDDEN)))
        {

            blzext::vector_erase_reorder(arCriteria, i--);
        }
    }
    blz::sort(arCriteria.begin(), arCriteria.end(), OnlineService::CriteriaSortPredicate);
    UIWindow & tWrapPanel = UI_GetWindowFromID(UIWRAP_CONSOLE_LEADERBOARDS_CONQUESTS_DETAILVIEW_CHECKLIST_PANEL);
    const UIControlItemArray & arChildren = tWrapPanel.GetChildList();
    uint32 nCriteriaControlIndex = 0;

    for (; nCriteriaControlIndex < arChildren.size() && nCriteriaControlIndex < arCriteria.size(); ++nCriteriaControlIndex)
    {
        PopulateCriteriaElement(arChildren[nCriteriaControlIndex].id, arCriteria[nCriteriaControlIndex]);
    }

    UIControlSourceKey keySeriesItem = UIControlSourceKey::Parse("ConsoleAchievements:ConsoleAchievements_main.LayoutRoot.AchievementDetails.AchievementScrollBox.AchievementStack.ChecklistSpacer.ChecklistPanel.ChecklistItem", SNO_NONE, NULL, NULL);

    for (; nCriteriaControlIndex < arCriteria.size(); ++nCriteriaControlIndex)
    {
        UIControl * ptControl = UI_CreateControlFromTemplate(keySeriesItem, DynamicControlCreationParams(CRefString().Format("criteria%d", nCriteriaControlIndex), UIWRAP_CONSOLE_LEADERBOARDS_CONQUESTS_DETAILVIEW_CHECKLIST_PANEL), NULL);

        PopulateCriteriaElement(ptControl->GetID(), arCriteria[nCriteriaControlIndex]);
    }

    while (nCriteriaControlIndex < arChildren.size())
    {
        UI_DeleteControl(arChildren[nCriteriaControlIndex].id, TRUE);
    }
}
} }

namespace UILeaderboards { namespace Console {
#line 1080 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
uint32 GetCriteriaFlags(
    const OnlineService::AchievementDefinition & tDefinition)
{

    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;

    uint32 dwFlags =

        (tDefinition.m_flags & (1 << OnlineService::ACHIEVEMENT_FLAG_HAS_PROGRESS_BAR)) ? (1 << PROGRESS_BAR) : 0;




    OnlineService::CriteriaIdList arCriteriaIds;
    int32 nCount = 0;
    if (tAchievements.GetAchievementCriteria(tDefinition.m_id, arCriteriaIds))
    {
        for (uint32 i = 0; i < arCriteriaIds.size(); ++i)
        {
            const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition(arCriteriaIds[i]);
            if (!ptCritDef)
            {
                continue;
            }


            if (ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_HIDDEN))
            {
                continue;
            }

            if (++nCount > 1)
            {
                dwFlags |= (1 << CHECKLIST);
                break;
            }
        }
    }

    return dwFlags;
}
#line 1262 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void sUpdateConquestDetails(
    const OnlineService::AchievementId & idAchievement)
{

    const OnlineService::AchievementDefinition * ptDefinition = sgtAppGlobals.ptAchievements->GetAchievementDefinition(idAchievement);

    if (!ptDefinition)
        return;




    CRefString sName = StringListFetch(ptDefinition->m_pszNameKey);



    CRefString sDescription = StringListFetch(ptDefinition->m_pszDescriptionKey);
    UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_CONQUESTS_DETAILVIEW_TEXT).SetText(sDescription.str(), FALSE);


    OnlineService::AchievementRecord tRecord;
    CRefString sEarnedText;
    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;
    if (ptAchievementsInterface && ptAchievementsInterface->FindAwardedAchievement(sgtUILeaderboardData.idPlayer, idAchievement, tRecord))
    {
        sEarnedText = LocalizedSprintf(
            StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_TOOLTIPS), "AchievementTooltipEarned").str(),
            UnicodeText::FormatDate(tRecord.m_timestampEarned, FDS_SHORT, XLOCALE_INVALID).str(), NULL);
    }
    else
    {

        sEarnedText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_TOOLTIPS), "AchievementTooltipUnearned");
    }

    UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_CONQUESTS_DETAILVIEW_EARNED).SetText(sEarnedText.str(), FALSE);


    UIWindow & tWnd = UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_DETAILVIEW_ICON);
    ImageTextureFrame tIconTexture(CRefString().Format("UI:%s", ptDefinition->m_pszIconTexture));
    tWnd.SetTextureFrame(tIconTexture);


    UIText & tPointsText = UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_CONQUESTS_DETAILVIEW_VALUE);
    if (ptDefinition->m_pointsValue)
    {
        tPointsText.SetText(UnicodeText::FormatNumber((int32)ptDefinition->m_pointsValue, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), FALSE);
        tPointsText.SetActive(TRUE);
    }
    else
    {
        tPointsText.SetActive(FALSE);
    }

    OnlineService::CriteriaQuantity nEarned = 0, nTotal = 0;
    uint32 dwFlags = GetCriteriaFlags(*ptDefinition);

    OnlineService::AchievementStatus eStatus = OnlineService::GetAchievementStatus(::Console::ClientAchievementsInterface::UserIndexToGameAccount(sgtUILeaderboardData.nControllerIndex), idAchievement, &nEarned, &nTotal);


    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_DETAILVIEW_PROGRESS).SetActive(dwFlags & (1 << PROGRESS_BAR));
    if (dwFlags & (1 << PROGRESS_BAR))
    {
        PopulateProgressBar(nEarned, nTotal);
    }
    UI_GetWindowFromID(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_DETAILVIEW_CHECKLIST).SetActive((dwFlags >> CHECKLIST) & 1);
    if ((dwFlags >> CHECKLIST) & 1)
    {
        PopulateCriteriaDisplay(*ptDefinition);
    }

}
} }

// Inference: this CPP-local declaration uses the exact LobbyService.cpp:4444 DWARF signature; declaring header unavailable.
namespace Console { namespace Online {
void LeaderboardGetMultipleScoresForPlayer(int32 nUserIndex, const blz::vector<LeaderboardKeyAndPartition> & idLeaderboards, LeaderboardGetScoresCallback pfnCallback);
} }
namespace UILeaderboards { namespace Console {
#line 101 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
const int32 MAX_GREATER_RIFT_LEADERBOARDS = 10;

const int32 FIRST_WEEKLY_CHALLENGE_INDEX = 1;
const int32 MAX_WEEKLY_CHALLENGE_LEADERBOARDS = 4;
#line 3104 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Inference: the original flag branch skips both scoped requests when a request is outstanding.
void IssuePlayerPositionRequest()

{

    if (sgtUILeaderboardData.bPlayerPositionRequestIssued)
    {
        return;
    }

    {
        blz::vector<LeaderboardKeyAndPartition> idLeaderboards;

        LeaderboardKeyAndPartition tKey;

        sgtUILeaderboardData.mapPlayerScores.clear();

        // Inference: the key-success guard is optimized away for this constant tab/index.
        if (CreateLeaderboardKeyForSelection(tKey, CURRENT_TAB_SEASON_CHALLENGE, 0, FALSE, FALSE))
        {
            idLeaderboards.push_back(tKey);
        }

        // Inference: zero-based loops and key-success guards reconstruct the original inline tree and branches.
        for (int32 n = 0; n < sgtUILeaderboardData.arConquestAchievementIds.size(); ++n)
        {
            if (CreateLeaderboardKeyForSelection(tKey, CURRENT_TAB_CONQUESTS, n, FALSE, FALSE))
            {
                idLeaderboards.push_back(tKey);
            }
        }

        // Inference: h enumerates softcore then hardcore; bounds follow the two original rift loops.
        for (int32 h = 0; h < 2; ++h)
        {
            for (int32 n = 0; n < MAX_GREATER_RIFT_LEADERBOARDS; ++n)
            {
                if (CreateLeaderboardKeyForSelection(tKey, CURRENT_TAB_GREATER_RIFTS, n, h, FALSE))
                {
                    idLeaderboards.push_back(tKey);
                }
            }
        }

        // Inference: weekly indices 1 through 4 follow the unrolled original key-generation sequence.
        for (int32 n = FIRST_WEEKLY_CHALLENGE_INDEX; n <= MAX_WEEKLY_CHALLENGE_LEADERBOARDS; ++n)
        {
            if (CreateLeaderboardKeyForSelection(tKey, CURRENT_TAB_SEASON_CHALLENGE, n, FALSE, FALSE))
            {
                idLeaderboards.push_back(tKey);
            }
        }

        // Inference: construct the exact DWARF functor directly; the unavailable binder wrapper leaves no instructions. Capture is FALSE.
        ::Console::Online::LeaderboardGetMultipleScoresForPlayer(sgtUILeaderboardData.nControllerIndex, idLeaderboards, OnlineService::internal::function_bind_1_2<void, BOOL, ::Console::Online::LeaderboardResult, const ScoreList &, &sPlayerPositionCallback>(FALSE));
    }


    {
        blz::vector<LeaderboardKeyAndPartition> idLeaderboards;

        LeaderboardKeyAndPartition tKey;

        sgtUILeaderboardData.mapPlayerScoresPreviousSeason.clear();

        // Inference: the mirrored request uses previous-results TRUE in the original inline arguments.
        if (CreateLeaderboardKeyForSelection(tKey, CURRENT_TAB_SEASON_CHALLENGE, 0, FALSE, TRUE))
        {
            idLeaderboards.push_back(tKey);
        }

        // Inference: the previous-season loop uses the same zero-based conquest range.
        for (int32 n = 0; n < sgtUILeaderboardData.arConquestAchievementIds.size(); ++n)
        {
            if (CreateLeaderboardKeyForSelection(tKey, CURRENT_TAB_CONQUESTS, n, FALSE, TRUE))
            {
                idLeaderboards.push_back(tKey);
            }
        }

        // Inference: mirror the two hardcore states and ten rift indices observed in the original.
        for (int32 h = 0; h < 2; ++h)
        {
            for (int32 n = 0; n < MAX_GREATER_RIFT_LEADERBOARDS; ++n)
            {
                if (CreateLeaderboardKeyForSelection(tKey, CURRENT_TAB_GREATER_RIFTS, n, h, TRUE))
                {
                    idLeaderboards.push_back(tKey);
                }
            }
        }

        // Inference: previous weekly requests retain indices 1 through 4.
        for (int32 n = FIRST_WEEKLY_CHALLENGE_INDEX; n <= MAX_WEEKLY_CHALLENGE_LEADERBOARDS; ++n)
        {
            if (CreateLeaderboardKeyForSelection(tKey, CURRENT_TAB_SEASON_CHALLENGE, n, FALSE, TRUE))
            {
                idLeaderboards.push_back(tKey);
            }
        }
        // Inference: the same DWARF functor captures TRUE directly; the final original flag store is TRUE.
        ::Console::Online::LeaderboardGetMultipleScoresForPlayer(sgtUILeaderboardData.nControllerIndex, idLeaderboards, OnlineService::internal::function_bind_1_2<void, BOOL, ::Console::Online::LeaderboardResult, const ScoreList &, &sPlayerPositionCallback>(TRUE));
    }

    sgtUILeaderboardData.bPlayerPositionRequestIssued = TRUE;
}
} }

namespace UILeaderboards { namespace Console {
#line 3412 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Body not directly checkable: original copy is linker-removed (DIE032e15c2); required by the real key conversion. Inference: switch returns follow inline original case values.
int32 GetWeeklyChallengeIdFromIndex(
    int32 nIndex)
{
    switch (nIndex)
    {
    case 1: return LB_CHALLENGE_RIFT_TEAM_1;
    case 2: return LB_CHALLENGE_RIFT_TEAM_2;
    case 3: return LB_CHALLENGE_RIFT_TEAM_3;
    case 4: return LB_CHALLENGE_RIFT_TEAM_4;
    default: return -1;
    }
}
#line 3430 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Body not directly checkable: original copy is linker-removed (DIE032e15dd); real key callers need it. Inference: switch/default follow the inlined original and existing exact key function.
int32 GetGreaterRiftLeaderboardIdFromIndex(
    int32 nIndex)
{
    switch (nIndex)
    {
    case 0: return LB_RIFT_TEAM_2;
    case 1: return LB_RIFT_TEAM_3;
    case 2: return LB_RIFT_TEAM_4;
    case 3: return LB_RIFT_SOLO_BARBARIAN;
    case 4: return LB_RIFT_SOLO_CRUSADER;
    case 5: return LB_RIFT_SOLO_DEMONHUNTER;
    case 6: return LB_RIFT_SOLO_MONK;
    case 7: return LB_RIFT_SOLO_NECROMANCER;
    case 8: return LB_RIFT_SOLO_WITCHDOCTOR;
    case 9: return LB_RIFT_SOLO_WIZARD;
    default: return 0;
    }
}
#line 3502 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Inference: restore the original helper calls and field assignments from the inline tree/line table, without the inherited non-DWARF locals.
BOOL CreateLeaderboardKeyForSelection(
    LeaderboardKeyAndPartition & tKey,
    CurrentTab eCurrentTab,
    int32 nIndex,
    BOOL bHardcore,
    BOOL bPreviousResults)
{

    switch (eCurrentTab)
    { case CURRENT_TAB_SEASON_CHALLENGE:
        if (nIndex == 0)
        {
            tKey.m_partition = OnlineService::GetSeasonNum();
            tKey.m_scope = LP_CHALLENGE; tKey.m_id = LB_ACHIEVEMENT_POINTS;
        }
        else
        {
            // Inference: the original invalid challenge-number branch returns FALSE before writing the key.
            if (sgtUILeaderboardData.nChallengeNumber == -1)
            {
                return FALSE;
            }

            tKey.m_partition = sgtUILeaderboardData.nChallengeNumber;
            tKey.m_scope = LP_CHALLENGE;
            tKey.m_id = GetWeeklyChallengeIdFromIndex(nIndex);
            if (tKey.m_id == static_cast<uint64>(-1))
            {
                return FALSE;
            }
        }
        break;


    case CURRENT_TAB_CONQUESTS:
    {
        tKey.m_partition = OnlineService::GetSeasonNum();
        tKey.m_scope = LP_SEASONAL;
        tKey.m_id = sgtUILeaderboardData.arConquestAchievementIds[nIndex];
        break;
    }
    case CURRENT_TAB_GREATER_RIFTS:
        tKey.m_partition = OnlineService::GetSeasonNum();
        // Inference: original condition selects hardcore scope4 versus softcore5.
        tKey.m_scope = bHardcore ? LP_SEASONAL_HARDCORE : LP_SEASONAL_SOFTCORE;







        // Inference: original zero identifier exits FALSE; the explicit getter is authenticated at this call line.
        tKey.m_id = GetGreaterRiftLeaderboardIdFromIndex(nIndex);
        if (tKey.m_id == 0)
        {
            return FALSE;
        }
        break;
    default:
        return FALSE;
    }

    // Inference: original previous-results guard decrements the key partition by one.
    if (bPreviousResults)
    {
        --tKey.m_partition;
    }

    return TRUE;

}
} }

namespace UILeaderboards { namespace Console {
#line 2992 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Body not directly checkable: original copy linker-removed(DIE032e030b), required by real conversion. Inference: original packing shifts scope16/index8 and ORs the hardcore flag.
int32 MakeGameKey(
    CurrentTab eScopeKey,
    int32 nLeaderboardIndex,
    BOOL bHardcore)
{
    return (eScopeKey << 16) | (nLeaderboardIndex << 8) | bHardcore;
}
#line 3460 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Body not directly checkable: original copy linker-removed(DIE032e10d4), needed by real conversion. Inference: scan ten original rift keys from index0 and return -1 if absent.
int32 GetIndexFromGreaterRiftLeaderboardId(
    int32 nLeaderboardId)
{
    for (int32 n = 0; n < MAX_GREATER_RIFT_LEADERBOARDS; ++n)
    {
        if (GetGreaterRiftLeaderboardIdFromIndex(n) == nLeaderboardId)
        {
            return n;
        }
    }

    return -1;
}
#line 3474 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Body not directly checkable: original copy linker-removed(DIE032e10ab), needed by real conversion. Inference: original scans exactly ten entries, using the shared ten-entry limit.
int32 GetIndexFromConquestLeaderboardId(
    int64 nLeaderboardId)
{
    for (int32 n = 0; n < MAX_GREATER_RIFT_LEADERBOARDS; ++n)
    {
        if (sgtUILeaderboardData.arConquestAchievementIds[n] == nLeaderboardId)
        {
            return n;
        }
    }

    return -1;
}
#line 3488 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Body not directly checkable: original copy linker-removed(DIE032e1079), needed by real conversion. Inference: scan indices1..4 using original weekly getter and return -1 if absent.
int32 GetIndexFromWeeklyChallengeLeaderboardId(
    int32 nLeaderboardId)
{
    for (int32 n = FIRST_WEEKLY_CHALLENGE_INDEX; n <= MAX_WEEKLY_CHALLENGE_LEADERBOARDS; ++n)
    {
        if (GetWeeklyChallengeIdFromIndex(n) == nLeaderboardId)
        {
            return n;
        }
    }

    return -1;
}
#line 3000 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Inference: DWARF initial values are scope/index -1 and hardcore FALSE; decoded cases reconstruct the assignments and final validity guard.
int32 ConvertLeaderboardAndScopeToGameKey(
    int64 nLeaderboardId,
    int32 nScopeId)
{
    int32 nScopeKey = -1;
    int32 nLeaderboardKey = -1;
    BOOL bHardcore = FALSE;
    switch (nScopeId)
    {
    case LP_CHALLENGE:
        nScopeKey = CURRENT_TAB_SEASON_CHALLENGE;
        nLeaderboardKey = nLeaderboardId == LB_ACHIEVEMENT_POINTS ? 0 : GetIndexFromWeeklyChallengeLeaderboardId(nLeaderboardId);
        break;
    case LP_SEASONAL:
        nScopeKey = CURRENT_TAB_CONQUESTS;
        nLeaderboardKey = GetIndexFromConquestLeaderboardId(nLeaderboardId);
        break;
    case LP_SEASONAL_HARDCORE:
        nScopeKey = CURRENT_TAB_GREATER_RIFTS;
        nLeaderboardKey = GetIndexFromGreaterRiftLeaderboardId(nLeaderboardId);
        bHardcore = TRUE;
        break;
    case LP_SEASONAL_SOFTCORE:
        nScopeKey = CURRENT_TAB_GREATER_RIFTS;
        nLeaderboardKey = GetIndexFromGreaterRiftLeaderboardId(nLeaderboardId);
        break;
    }

    if (nLeaderboardKey == -1)
    {
        return -1;
    }
    // Inference: DWARF local is int32 while the helper accepts CurrentTab; value comes only from the original enumerators above.
    return MakeGameKey(static_cast<CurrentTab>(nScopeKey), nLeaderboardKey, bHardcore);
}
} }

#include "../../Battlenet/UI/Util/UIUtil.h"
// DWARF X/Client/UserInterface/UI.cpp:674; the public UI header does not yet expose this declaration.
const UIID & UI_GetControllerFocus();
// DWARF X/Common/Leaderboard.cpp:113,118,142,147,163; declarations only.
uint32 LeaderboardGetRiftScoreLevel(uint64 score);
Blizzard::Time::Timestamp LeaderboardGetRiftScoreTimeToComplete(uint64 score);
uint32 LeaderboardGetAchievementPoints(uint64 score);
Blizzard::Time::Timestamp LeaderboardGetLastAchievementTime(uint64 score);
Blizzard::Time::Timestamp LeaderboardGetConquestTime(uint64 score);
namespace UILeaderboards { namespace Console {
#line 348 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Inference: original line 352 dispatches the filter switch; default uses friends, return rows 356/358/360. Standalone body is not checkable: DWARF records only its inline instances.
inline const ScoreList & LeaderboardDataSource::GetCurrentLeaderboard()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{

    switch (eCurrentFilter)
    {
    default:
    case LEADERBOARD_FILTER_FRIENDS:
        return tFriendsLeaderboardScores;
    case LEADERBOARD_FILTER_GLOBAL_POSITION:
        return tGlobalPositionLeaderboardScores;
    case LEADERBOARD_FILTER_TOP_GLOBAL:
        return tTopGlobalLeaderboardScores;
    }
}
#line 99 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
const int32 NUM_BEFORE_END_TO_TRIGGER_FETCH = 10;
#line 774 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Original low_pc 0: standalone body is not checkable; inferred getter is required by UpdateItemUI and its surviving inline loads.
CurrentTab GetCurrentTab()
{

    return sgtUILeaderboardData.eCurrentTab;
}
#line 780 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Original low_pc 0: standalone body is not checkable; inferred getter is required by UpdateItemUI and its surviving inline loads.
int32 GetSelectedCategory()
{

    return sgtUILeaderboardData.nSelectedCategory;
}
#line 786 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Original low_pc 0: standalone body is not checkable; inferred getter is required by UpdateItemUI and its surviving inline loads.
LeaderboardFilters GetCurrentFilter()
{

    return sgtUILeaderboardData.eLeaderboardFilter;
}
#line 792 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Original low_pc 0: standalone body is not checkable; inferred getter is required by UpdateItemUI and its surviving inline loads.
BOOL UsePreviousWeek()
{

    return sgtUILeaderboardData.bLastWeeklyChallenge;
}
#line 399 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
// Inference: branch spellings below follow original predicates and DWARF lexical scopes.
void LeaderboardDataSource::UpdateItemUI(
    const UIConsoleListBox & tListBox,
    const UIID & idItemUI,
    const int32 nIndex)
{
    if (nIndex >= GetCurrentLeaderboard().size())
    {
        return;
    }

    UIID idHighlight = UIIDCreateFromParent(idItemUI, ".LayoutRoot.Selected");
    BOOL bSelected = tListBox.GetSelectedIndex() == nIndex;
    UI_GetWindowFromID(idHighlight).SetActive(bSelected && UI_GetControllerFocus() == tListBox.GetID());
    // Inference: DWARF records FALSE initially; only the visible-edge branches set TRUE.
    BOOL bShowPlayerHere = FALSE;

    UIID idNormalBackground = UIIDCreateFromParent(idItemUI, ".LayoutRoot.Entry.BGStandard");
    UIID idPlayerBackground = UIIDCreateFromParent(idItemUI, ".LayoutRoot.Entry.BGSelf");

    int32 nRank = GetCurrentLeaderboard()[nIndex].score_band();

    if (nPlayerPositionRank[eCurrentFilter] > 0)
    {
        if (tListBox.GetFirstVisibleItemIndex() == nIndex && nRank > nPlayerPositionRank[eCurrentFilter])
        {
            bShowPlayerHere = TRUE;
        }

        // Inference: last-visible row follows the original fall-through from the first-row test.
        else if (tListBox.GetFirstVisibleItemIndex() + tListBox.GetNumVisibleItems() - 1 == nIndex && nRank < nPlayerPositionRank[eCurrentFilter])
        {
            bShowPlayerHere = TRUE;
        }
    }


    UIID idRank = UIIDCreateFromParent(idItemUI, ".LayoutRoot.Entry.Rank");
    UIID idName = UIIDCreateFromParent(idItemUI, ".LayoutRoot.Entry.Name");
    UIID idScore = UIIDCreateFromParent(idItemUI, ".LayoutRoot.Entry.Score");
    UIID idTime = UIIDCreateFromParent(idItemUI, ".LayoutRoot.Entry.Time");

    if (bSelected)
    {

        UIID idStoryboard = UIIDCreateFromParent(idItemUI, ".ItemListHighlight");
        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Stop();
        tStoryboard.Play(0, 0.0f);

        if (UI_GetControllerFocus() == tListBox.GetID())
        {
            nCurrentIndex[eCurrentFilter] = nIndex;
        }
    }

    D3::Leaderboard::Score tScoreToUse;
    BOOL bPrevious = GetCurrentTab() == CURRENT_TAB_SEASON_CHALLENGE && GetSelectedCategory() != 0 && UsePreviousWeek();
    if (bShowPlayerHere && GetPlayerScoreForCurrentLeaderboard(tScoreToUse, bPrevious))
    {
        nRank = tScoreToUse.score_band();
    }
    else
    {
        tScoreToUse = GetCurrentLeaderboard()[nIndex];
        bShowPlayerHere = FALSE;
    }

    UI_GetWindowFromID(idNormalBackground).SetActive(nRank != nPlayerPositionRank[eCurrentFilter] && !bShowPlayerHere);
    UI_GetWindowFromID(idPlayerBackground).SetActive(nRank == nPlayerPositionRank[eCurrentFilter] || bShowPlayerHere);

    UI_GetTextFromID(idRank).SetText(UnicodeText::FormatNumber(nRank, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), FALSE);
    // Inference: absent metadata defaults to the original Barbarian/male values (1/1).
    PlayerClass eClass = PLAYER_CLASS_BARBARIAN; // Inference: DWARF records initial bIsMale TRUE below.
    BOOL bIsMale = TRUE;

    if (tScoreToUse.metadata().team_member_size() > 0)
    {
        eClass = PlayerClassFromGBID(tScoreToUse.metadata().team_member(0).hero_gbid_class()); // Inference: original tests hero flag bit 1; no named bit is present in this CU.
        bIsMale = !(tScoreToUse.metadata().team_member(0).hero_flags() & 2);
    }

    CRefString sPortraitName;
    sPortraitName.Format("{icon:%s, 4}",

        GetPortraitImageName(eClass, bIsMale).str());

    CRefString szFormattedName;
    szFormattedName.Format("%s %s",
        sPortraitName.str(),
        tScoreToUse.metadata().team_member_size() > 0 ? tScoreToUse.metadata().team_member(0).battle_tag().c_str() : "");

    UI_GetTextFromID(idName).SetText(szFormattedName.str(), FALSE);

    CRefString strFormattedScore;
    CRefString strFormattedTime;

    if (GetCurrentTab() == CURRENT_TAB_CONQUESTS)
    {
        Blizzard::Time::Timestamp uTime = LeaderboardGetConquestTime(tScoreToUse.score());
        strFormattedTime = UnicodeText::FormatDate(uTime, FDS_SHORT, XLOCALE_INVALID);
    }
    else if (GetCurrentTab() == CURRENT_TAB_SEASON_CHALLENGE && GetSelectedCategory() == 0)
    {
        int32 nScore = LeaderboardGetAchievementPoints(tScoreToUse.score());
        strFormattedScore = UnicodeText::FormatNumber(nScore, TRUE, FNS_NORMAL, XLOCALE_INVALID);
        Blizzard::Time::Timestamp uTime = LeaderboardGetLastAchievementTime(tScoreToUse.score());
        strFormattedTime = UnicodeText::FormatDate(uTime, FDS_SHORT, XLOCALE_INVALID);
    }
    else
    {
        int32 nRiftTier = LeaderboardGetRiftScoreLevel(tScoreToUse.score());

        strFormattedScore = UnicodeText::FormatNumber(nRiftTier, TRUE, FNS_NORMAL, XLOCALE_INVALID);

        Blizzard::Time::Timestamp tRiftTime = LeaderboardGetRiftScoreTimeToComplete(tScoreToUse.score());

        ElapsedTimeDisplayOptions tElapsedTimeDisplayOptions; // Inference: paired store sets always-seconds and number-only TRUE below.
        tElapsedTimeDisplayOptions.fAlwaysShowSeconds = tElapsedTimeDisplayOptions.fOnlyDisplayNumber = TRUE;
        // Inference: paired stores set count-up and milliseconds; zero defaults remain from the constructor.
        tElapsedTimeDisplayOptions.fCountUp = tElapsedTimeDisplayOptions.fDisplayMilliseconds = TRUE;

        strFormattedTime = DisplayElapsedTime(0L, tRiftTime, tElapsedTimeDisplayOptions);
    }

    UI_GetWindowFromID(idScore).SetActive(GetCurrentTab() != CURRENT_TAB_SEASON_CHALLENGE || GetSelectedCategory() == 0);

    UI_GetTextFromID(idScore).SetText(strFormattedScore.str(), FALSE);
    UI_GetTextFromID(idTime).SetText(strFormattedTime.str(), FALSE);

    if (GetCurrentFilter() != LEADERBOARD_FILTER_FRIENDS)
    {
        if (nIndex < NUM_BEFORE_END_TO_TRIGGER_FETCH && tListBox.GetScrollDirection() < 0 && GetCurrentLeaderboard()[0].score_band() > 1)
        {

            IssueAdditionalLeaderboardRequest(GetCurrentFilter(), GetCurrentLeaderboard()[0].score_band(), TRUE);
        }
        // Inference: the opposite scroll direction fetches after the final entry inside the original ten-row threshold.
        else if (tListBox.GetScrollDirection() > 0 && GetCurrentLeaderboard().size() - nIndex < NUM_BEFORE_END_TO_TRIGGER_FETCH)
        {

            IssueAdditionalLeaderboardRequest(GetCurrentFilter(), GetCurrentLeaderboard()[GetCurrentLeaderboard().size() - 1].score_band(), FALSE);
        }
    }
}
}
}
// DWARF X/Client/UserInterface/UIGlobals.cpp:1012-1015 (UIIDs defined there); src/X/Client/UserInterface/UIIDs.h does not yet declare them.
extern UIID UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TITLE;
extern UIID UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT1;
extern UIID UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT2;
extern UIID UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT3;
namespace UILeaderboards {
namespace Console {
#line 2618 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
void UpdatePlayerScore()

{
    const CurrentTab eCurrentTab = sgtUILeaderboardData.eCurrentTab;

    D3::Leaderboard::Score tScore;
    if (GetPlayerScoreForCurrentLeaderboard(tScore, TRUE))
    {
        switch (eCurrentTab)
        {
        case CURRENT_TAB_SEASON_CHALLENGE:
            if (sgtUILeaderboardData.bChallengeRifts)
            {
                UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TITLE).SetText(StringListFetch("ConsoleLeaderboards:LastWeekTitle"), FALSE);
                UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT1).SetText(LocalizedSprintf(StringListFetch("ConsoleLeaderboards:LastSeasonRank"), UnicodeText::FormatNumber((int32)tScore.score_band(), TRUE, FNS_NORMAL, XLOCALE_INVALID).str()), FALSE);
                UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT2).SetText("", FALSE);
                UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT3).SetText("", FALSE);
            }
            else
            {
                UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TITLE).SetText(StringListFetch("ConsoleLeaderboards:LastSeasonTitle"), FALSE);
                UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT1).SetText(LocalizedSprintf(StringListFetch("ConsoleLeaderboards:LastSeasonRank"), UnicodeText::FormatNumber((int32)tScore.score_band(), TRUE, FNS_NORMAL, XLOCALE_INVALID).str()), FALSE);
                UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT2).SetText(LocalizedSprintf(StringListFetch("ConsoleLeaderboards:LastSeasonPoints"), UnicodeText::FormatNumber((int32)LeaderboardGetAchievementPoints(tScore.score()), TRUE, FNS_NORMAL, XLOCALE_INVALID).str()), FALSE);
                UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT3).SetText("", FALSE);
            }
            break;
        case CURRENT_TAB_GREATER_RIFTS:
        {

            UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TITLE).SetText(StringListFetch("ConsoleLeaderboards:LastSeasonTitle"), FALSE);
            UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT1).SetText(LocalizedSprintf(StringListFetch("ConsoleLeaderboards:LastSeasonRank"), UnicodeText::FormatNumber((int32)tScore.score_band(), TRUE, FNS_NORMAL, XLOCALE_INVALID).str()), FALSE);
            UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT2).SetText(LocalizedSprintf(StringListFetch("ConsoleLeaderboards:LastSeasonTier"), UnicodeText::FormatNumber((int32)LeaderboardGetRiftScoreLevel(tScore.score()), TRUE, FNS_NORMAL, XLOCALE_INVALID).str()), FALSE);

            Blizzard::Time::Timestamp tRiftTime = LeaderboardGetRiftScoreTimeToComplete(tScore.score());
            ElapsedTimeDisplayOptions tElapsedTimeDisplayOptions;
            tElapsedTimeDisplayOptions.fAlwaysShowSeconds = TRUE;
            tElapsedTimeDisplayOptions.fOnlyDisplayNumber = TRUE;
            tElapsedTimeDisplayOptions.fCountUp = TRUE;
            tElapsedTimeDisplayOptions.fDisplayMilliseconds = TRUE;
            CRefString strFormattedTime = DisplayElapsedTime(0L, tRiftTime, tElapsedTimeDisplayOptions);
            UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT3).SetText(LocalizedSprintf(StringListFetch("ConsoleLeaderboards:LastSeasonTime"), strFormattedTime.str()), FALSE);
        }
            break;
        case CURRENT_TAB_CONQUESTS:
        {
            Blizzard::Time::Timestamp uTime = LeaderboardGetConquestTime(tScore.score());
            CRefString strFormattedTime = UnicodeText::FormatDateTime(uTime, FDS_SHORT, FTS_SHORT, XLOCALE_INVALID);

            UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TITLE).SetText(StringListFetch("ConsoleLeaderboards:LastSeasonTitle"), FALSE);
            UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT1).SetText(LocalizedSprintf(StringListFetch("ConsoleLeaderboards:LastSeasonRank"), UnicodeText::FormatNumber((int32)tScore.score_band(), TRUE, FNS_NORMAL, XLOCALE_INVALID).str()), FALSE);
            UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT2).SetText(LocalizedSprintf(StringListFetch("ConsoleLeaderboards:LastSeasonDate"), strFormattedTime.str()), FALSE);
            UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT3).SetText("", FALSE);
        }
            break;
        }
    }
    else
    {

        UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TITLE).SetText(sgtUILeaderboardData.bChallengeRifts ? StringListFetch("ConsoleLeaderboards:LastWeekTitle") : StringListFetch("ConsoleLeaderboards:LastSeasonTitle"), FALSE);
        UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT1).SetText(StringListFetch("ConsoleLeaderboards:SeasonDataUnavailable"), FALSE);
        UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT2).SetText("", FALSE);
        UI_GetTextFromID(UITEXT_CONSOLE_LEADERBOARDS_LAST_SEASON_TEXT3).SetText("", FALSE);
    }
}
}
}
namespace UILeaderboards { namespace Console {
// LINKER-REMOVED: DWARF concrete DIE (low_pc 0, 0x84 bytes, abstract origin decl line 1952, param nIndex, block local strElement)
// and inlined copies in EnableConquestsScreen (call line 2224) and GetCategoryIndexForElementUIID (2257): rows 1954:0, 1955:28 (range test,
// folded away in both callers; the bound 10 is inferred from the ten-iteration caller loop), 1957:41/79, 1958:24/99, 1959:13, 1961:20, 1962:9.
#line 1952 "X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp"
UIID GetElementUIIDForConquestsCategory(
    int32 nIndex)
{
    if (nIndex >= 0 && nIndex < 10)
    {
        CRefString strElement = FormatRefString(".Conquest%d", nIndex + 1);
        return UIIDCreateFromParent(UIWND_CONSOLE_LEADERBOARDS_CONQUESTS_RING, strElement.str());
    }

    return GetUIIDNone();
}
} }
