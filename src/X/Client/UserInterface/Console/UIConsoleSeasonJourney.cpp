// Original file: X/Client/UserInterface/Console/UIConsoleSeasonJourney.cpp
//
// The console season journey screen: the chapter ring, the objective list and the rewards view.
#include "../../../Common/VisualInventory.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "ConsoleInputUtilities.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "Wrappers/RingWrapper.h"
#include "UIConsoleUtils.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Controls/UIButtonControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/Online.h"
#include "../../Console/ControllerUtils.h"
#include "../../CGame.h"
#include "../../../Common/Game.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/Achievements/Console/ConsoleAchievements.h"
#include "../../../Common/OnlineService/Achievements.h"
#include "../../../Common/OnlineService/OnlineService.h"
#include "../../../Common/Protobuf/Account.pb.h"
#include "../../../Common/Protobuf/Hero.pb.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Sigma/RefString.h"
#include "../../Sound/Sound.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "ConsoleModalNotificationManager.h"
#include "../../Battlenet/UI/Util/UIAchievementsUtil.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../Controls/UIWrapPanelControl.h"
#include "../UIInit.h"
#include "../../../Common/Partition.h"
#include "../../CPlayer.h"
#include "../Interface/UISeasonJourneyUtil.h"
#include "../../SubObjectGfx.h"
// Declared here because their headers are not adopted yet. Each comment names the defining file.
CRefString StringListFetch(LPCSTR szLabel);  // X/Common/Definitions/StringList.cpp
const UIID & UI_GetControllerFocus();  // X/Client/UserInterface/UI.cpp
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
namespace OnlineService { void ClearAchievementsCache(); }  // X/Common/OnlineService/Achievements.cpp
namespace Console { namespace Online { BOOL IsSeasonsInitialized(); } }  // X/Client/Console/Online.cpp
// X/Client/UserInterface/Interface/UISeasonJourneyUtil.cpp
BOOL SeasonJourney_IsCasualAchievement(const OnlineService::AchievementIdList & arCasualAchievementIds, const OnlineService::AchievementId & idAchievement);
BOOL SeasonJourney_IsAchievementCompleted(const OnlineService::GameAccountHandle & tAccountId, const OnlineService::AchievementId & idAchievement);
void SeasonJourney_GetAchievementsToDisplay(OnlineService::AchievementId & idAchDefinitionToDisplay, SeasonJourneyProgressionView & eSeasonJourneyProgressionView, OnlineService::AchievementIdList & arCasualAchievementIds, OnlineService::AchievementIdList & arAdvancedAchievementIds, const OnlineService::GameAccountHandle & tAccountId);
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp
BOOL PlayerIsHardcore(const Player * ptPlayer);  // X/Common/Player.cpp
void UI_DeleteControl(const UIID & idControl, const BOOL fAssertIfDeletingRoot);  // X/Client/UserInterface/UI.cpp
UIWrapPanel & UI_GetWrapPanelFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
namespace Console { namespace Screens { BOOL OpenToScreen(const ScreenSets eScreenSet, const int32 idScreen, int32 nControllerOpening, BOOL fSkipTransition); } }
// X/Common/Networking/GameMessage.h (kept local: other files define their own subsets)
#include "../Controls/UIRActorControl.h"
#include "../../Battlenet/UI/Util/UIUtil.h"
#include "../Controls/UIItemButtonControl.h"
// X/Client/UserInterface/UIIDs.cpp
extern UIID UIWND_CONSOLE_SEASON_JOURNEY;
extern UIID UIWND_CONSOLE_SEASON_JOURNEY_RING;
extern UIID UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_WRAPPER;
extern UIID UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_RIGHT_PANEL;
extern UIID UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_LEFT_PANEL_COMPLETE;
extern UIID UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS;
extern UIID UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS_REDEEMED;
extern UIID UIWRAP_CONSOLE_SEASON_JOURNEY_WRAPPER;
extern UIID UIWND_CONSOLE_SEASON_JOURNEY_DETAILS;
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_OBJECTIVES_MESSAGE;
extern UIID UIWND_CONSOLE_SEASON_JOURNEY_TAB_LEFT;
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_TITLE;
extern UIID UIWND_CONSOLE_SEASON_JOURNEY_TITLE_BLINKER;
extern UIID UISTORYBOARD_CONSOLE_SEASON_JOURNEY_GIFT_REDEEMED;
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_DESCRIPTION_TITLE;
extern UIID UIWND_CONSOLE_SEASON_JOURNEY_MENU_GLOW;
extern UIID UIWND_CONSOLE_SEASON_JOURNEY_RING_CURSOR;
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_PLAYER_LABEL;
extern UIID UILIST_CONSOLE_SEASON_JOURNEY_CHAPTER_ICONS;
extern UIID UIWND_SEASON_JOURNEY_ADVANCED_CHAPTER_ICON;
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_LIST_TITLE;
extern UIID UIWND_CONSOLE_SEASON_JOURNEY_DIMMER;
extern UIID UIWND_CONSOLE_SEASON_JOURNEY_RING_GLOW;
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_DESCRIPTION_DETAILS;

namespace UISeasonJourney {
namespace Console {

void OpenForController(const int32 nControllerIndex);
void Close();
BOOL IsActive();
void OnClose(const UIID & id, const int32 nControllerIndex);
void FocusCategoryRing(BOOL bFocus);
void RefreshChapters();
void SetTitleText(BOOL bShowRewards);
CRefString GetChapterTitle(int32 nIndex);
UIID GetElementUIIDForCategory(int32 nIndex);
UIID GetBlinkerUIIDForCategory(int32 nIndex);
void UpdateChapterIcons(int32 nSelectedChapter);
GBID GetItemRewardForAchievement(OnlineService::AchievementId idAchievement);
BOOL IsGiftRedeemedByPlayer(int32 nControllerIndex, GBID gbidGift);
void DisplayRewardsForChapter(int32 nSelectedChapter);
void IconSelectionChanged(const ::Console::RingWrapper & tRingWrapper, const UIID & idIcon, const BOOL bSelected);
void PopulateCommonUIElements(const UIID & idWindow, const OnlineService::AchievementDefinition & tAchievementDefinition, const OnlineService::CriteriaDefinition & tCriteriaDefinition, const OnlineService::GameAccountHandle & idPlayer, BOOL bSelected);
void TryToRedeemGift();
static int32 GetCategoryIndexForElementUIID(const UIID & id);
BOOL RingIncludeControlPredicate(const UIID & idControl);
static void RefreshRingBlinkers();
enum {
    NUM_CASUAL_CHAPTERS = 4,
    NUM_ADVANCED_CHAPTERS = 5,
    NUM_CHAPTERS = 9,
};

enum ButtonActions { ACTION_CHOOSE_CATEGORY = 0, ACTION_VIEW_REWARDS = 1, ACTION_VIEW_OBJECTIVES = 2, ACTION_CLOSE = 3,
                     ACTION_SCROLL_GIFT_DETAILS = 4, ACTION_TOGGLE_SORT_ORDER = 5, ACTION_REDEEM = 6, ACTION_BACK = 7 };




class SeasonJourneyUIChapter {
public:


    SeasonJourneyUIChapter() : m_idAchievement(0), m_nCriteria(0), m_bComplete(FALSE) {}
    OnlineService::AchievementId m_idAchievement;
    uint32 m_nCriteria;
    BOOL m_bComplete;
};

typedef blz::vector<SeasonJourneyUIChapter> SeasonJourneyChapterList;
typedef blz::vector<unsigned long> SeasonJourneyCriteriaList;



class SeasonJourneyDataSource : public ConsoleListBoxDataSource {
public:

    virtual ~SeasonJourneyDataSource() {}


    SeasonJourneyDataSource() {}


virtual int32 GetNumItems() const
{
    return (int32)m_idCriteria.size();
}




virtual UIControlSourceKey GetSourceKeyForItem(const int32) const
{
    return UIControlSourceKey::Parse("ConsoleSeasonJourneyListCategoryTemplate:ConsoleSeasonJourneyListCategoryTemplate_main", -1, NULL, NULL);
}




virtual void InitializeItemUI(const UIConsoleListBox &, const UIID & idItemUI, const int32)
{
    static const char * BACKGROUND_RELATIVE_PATH_ROOT = ".LayoutRoot.Backgrounds";
    UIID idBackgrounds = UIIDCreateFromParent(idItemUI, BACKGROUND_RELATIVE_PATH_ROOT);
    UIWindow & tBackgrounds = UI_GetWindowFromID(idBackgrounds);
    for (uint32 i = 0; i < tBackgrounds.GetChildList().size(); i++)
    {
        UIID idChild = tBackgrounds.GetChildList()[i].id;
        if (UI_TestID(idChild))
        {
            UI_GetWindowFromID(idChild).SetActive(FALSE);
        }
    }

    UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".LayoutRoot.Backgrounds.SeasonJourneyObjective")).SetActive(TRUE);
}













virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    UIID idSelected = UIIDCreateFromParent(idItemUI, ".LayoutRoot.SelectedSeasonJourney");
    BOOL bSelected = tListBox.GetSelectedIndex() == nIndex;
    UI_GetWindowFromID(idSelected).SetActive(bSelected);

    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    const OnlineService::CriteriaDefinition * ptCriteriaDefinition = tAchievements.GetCriteriaDefinition(m_idCriteria[nIndex]);
    const OnlineService::AchievementDefinition * ptAchievementDefinition = tAchievements.GetAchievementDefinition(ptCriteriaDefinition->m_advanceEvent.m_comparand);
    if (ptAchievementDefinition)
    {
        if (bSelected)
        {
            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(UIIDCreateFromParent(idItemUI, ".ItemListHighlightSeasonJourney"));
            tStoryboard.Stop();
            tStoryboard.Play(FALSE, 0.0f);
        }
        PopulateCommonUIElements(idItemUI, *ptAchievementDefinition, *ptCriteriaDefinition, m_idPlayer, bSelected);
    }
}






















void Initialize(const OnlineService::GameAccountHandle & idPlayer, SeasonJourneyUIChapter tChapter)
{
    if (tChapter.m_idAchievement != m_idDisplayedAchievement || !(m_idPlayer == idPlayer))
    {
        m_idPlayer = idPlayer;
        m_idDisplayedAchievement = tChapter.m_idAchievement;
        m_bSortedByComplete = FALSE;
        OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
        const OnlineService::AchievementDefinition * ptDefinition = tAchievements.GetAchievementDefinition(m_idDisplayedAchievement);
        if (ptDefinition)
            PopulateCriteriaDisplay(*ptDefinition);
    }
}

















































































































void PopulateCriteriaDisplay(const OnlineService::AchievementDefinition & tDefinition)
{
    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::CriteriaIdList arCriteria;
    tAchievements.GetAchievementCriteria(tDefinition.m_id, arCriteria);
    for (uint32 i = 0; i < arCriteria.size(); i++)
    {
        const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition(arCriteria[i]);
        if (ptCritDef == NULL || (ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_HIDDEN)))
        {
            blzext::vector_erase_reorder(arCriteria, i--);
        }
    }
    blz::sort(arCriteria.begin(), arCriteria.end(), m_bSortedByComplete ? OnlineService::CriteriaSortByCompletedPredicate : OnlineService::CriteriaSortPredicate);

    m_idCriteria.clear();
    UIWindow & tWrapPanel = UI_GetWindowFromID(UIWRAP_CONSOLE_SEASON_JOURNEY_WRAPPER);
    const UIControlItemArray & arChildren = tWrapPanel.GetChildList();
    uint32 nCriteriaControlIndex = 0;
    for (; nCriteriaControlIndex < arChildren.size() && nCriteriaControlIndex < arCriteria.size(); nCriteriaControlIndex++)
    {
        m_idCriteria.push_back(arCriteria[nCriteriaControlIndex]);
    }

    UIControlSourceKey keySeriesItem = UIControlSourceKey::Parse("ConsoleSeasonJourneyListCategoryTemplate:ConsoleSeasonJourneyListCategoryTemplate_main", -1, NULL, NULL);
    for (; nCriteriaControlIndex < arCriteria.size(); nCriteriaControlIndex++)
    {
        m_idCriteria.push_back(arCriteria[nCriteriaControlIndex]);
    }

    while (nCriteriaControlIndex < arChildren.size())
    {
        UI_DeleteControl(arChildren[nCriteriaControlIndex].id, TRUE);
    }
}

    OnlineService::GameAccountHandle m_idPlayer;
    OnlineService::AchievementId m_idDisplayedAchievement;
    SeasonJourneyCriteriaList m_idCriteria;
    BOOL m_bSortedByComplete;
};

struct UISeasonJourneyData {









    UISeasonJourneyData() : bInitialized(FALSE), m_gbidGiftBeingRedeemed(GBID_NONE), m_bCurrentSelectionActiveOrComplete(FALSE) {}









void Initialize()
{
    if (!bInitialized)
    {
        tRingStickInputFilter.AddRule(::Console::AnalogStickInputFilter::Rule::CatchAllRule);
        bInitialized = TRUE;
    }
}






BOOL IsChapterActive(int32 nChapterIndex)
{
    if (nChapterIndex < 0 || nChapterIndex > m_arChapters.size())
        return FALSE;
    return nChapterIndex == 0 || m_arChapters[nChapterIndex - 1].m_bComplete;
}





BOOL IsChapterComplete(int32 nChapterIndex)
{
    if (nChapterIndex < 0 || nChapterIndex > m_arChapters.size())
        return FALSE;
    return m_arChapters[nChapterIndex].m_bComplete;
}





void ToggleRewardsView(BOOL bShow)
{
    m_bShowingRewards = bShow;
    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_WRAPPER).SetActive(m_bShowingRewards);
    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_RIGHT_PANEL).SetActive(m_bShowingRewards);
    UI_GetWindowFromID(UIWRAP_CONSOLE_SEASON_JOURNEY_WRAPPER).SetActive(!m_bShowingRewards);
    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_DETAILS).SetActive(!m_bShowingRewards && m_bCurrentSelectionActiveOrComplete);
    UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_OBJECTIVES_MESSAGE).SetActive(!m_bShowingRewards);

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    SetTitleText(m_bShowingRewards);
    if (!m_bShowingRewards)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_TOGGLE_SORT_ORDER, StringListFetch("ConsoleAchievements:ChangeSort"));
        tActionManager.ClearAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER);
        tActionManager.ClearAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER);
        UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_TAB_LEFT).SetActive(TRUE);
    }
    else
    {
        tActionManager.ClearAction(::Console::CONTROLLER_INPUT_BUTTON_4);
        DisplayRewardsForChapter(m_nSelectedCategory);
    }

    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_VIEW_OBJECTIVES, bShow ? StringListFetch("ConsoleSeasonJourney:ViewObjectives") : StringListFetch("ConsoleSeasonJourney:ViewRewards"));
    ::Console::Screens::UpdateButtonLegendText();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(FALSE);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CLICK_GENERIC), cgtPSODefaults);
}








int32 GetCompletedObjectivesForChapter(int32 nChapter)
{
    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::CriteriaIdList arCriteria;
    tAchievements.GetAchievementCriteria(m_arChapters[nChapter].m_idAchievement, arCriteria);
    int32 nComplete = 0;
    for (int32 n = 0; n < arCriteria.size(); n++)
    {
        const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition(arCriteria[n]);
        const OnlineService::AchievementDefinition * ptAchievementDefinition = tAchievements.GetAchievementDefinition(ptCritDef->m_advanceEvent.m_comparand);
        if (!(ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_HIDDEN)) && SeasonJourney_IsAchievementCompleted(idPlayer, ptAchievementDefinition->m_id))
        {
            nComplete++;
        }
    }
    return nComplete;
}


















int32 GetTotalObjectivesForChapter(int32 nChapter)
{
    return m_arChapters[nChapter].m_nCriteria;
}

    OnlineService::GameAccountHandle idPlayer;
    SeasonJourneyChapterList m_arChapters;
    SeasonJourneyDataSource tSeasonJourneyData;
    OnlineService::AchievementIdList m_arCasualAchievementIds;
    OnlineService::AchievementIdList m_arAdvancedAchievementIds;
    ::Console::RingWrapper tRingUI;
    ::Console::AnalogStickInputFilter tRingStickInputFilter;
    BOOL bInitialized;
    int32 m_nHighestCompletedChapter;
    BOOL m_bShowingRewards;
    int32 m_nSelectedCategory;
    int32 m_nControllerIndex;
    GBID m_gbidGiftBeingRedeemed;
    BOOL m_bCurrentSelectionActiveOrComplete;
};
}  // namespace Console
}  // namespace UISeasonJourney
// Declared for DisplayRewardsForChapter; each comment names the DWARF definition.
UIRActor & UI_GetRActorFromID(const UIID & id);  /* UI.cpp:5757 */  SNO UI_GetDefaultHeroLight();  /* UI.cpp:6354 */  UIItemButton & UI_GetItemButtonFromID(const UIID & id);  /* UI.cpp:5384 */
RActorID UI_CreateHeroUIRActor(const SNO snoActor, const SNO snoIrradianceTex, const VisualEquipment & tVisualEquipment, const VisualInventoryType eVIT, const PlayerClass ePlayerClass, const BOOL fFemale, const PRTransform & transform, const RenderContext eContext, const uint32 dwAnimTag, const float flScale, BOOL bDisableBlending);  // X/Client/UserInterface/Interface/UIFunctions.cpp:3324
LightID LightAdd(const uint32 dwFlags, const SNO sno, const PRTransform & transform, const int32 nPriorityBoost, const RenderContext eContext);  /* Lights.cpp:1104 */  void RActorSetIrradianceMap(RActor * ptRActor, const SNO snoIrradianceTex);  /* RActors.cpp:12340 */
namespace UISeasonJourney {
namespace Console {
UISeasonJourneyData sgtUISeasonJourneyData;
}  // namespace Console
}  // namespace UISeasonJourney
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_SEASON_PROGRESSION_TITLE;
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_SEASON_PROGRESSION_INCOMPLETE;
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_SEASON_BANNER;
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_CHAPTER_COMPLETE_SEASON_BANNER;
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_CHAPTER_COMPLETE_SEASON_NAME;
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_CHAPTER_COMPLETE_SEASON_STEP_NAME;
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_SEASON_NAME;
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_SEASON_STEP_NAME;
extern UIID UITEXT_CONSOLE_SEASON_JOURNEY_TITLE_LEFT;
namespace UISeasonJourney {
namespace Console {






void OnControllerHotkey(const UIID & id, int32 nControllerIndex)
{
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer)
    {
        if (ptPlayer->idPlayerACD != ACDID_INVALID)
        {
            ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACD && !ptACD->IsAlive())
                return;
        }

        if (::Console::Online::IsSeasonsInitialized() && OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE && !GameIsWeeklyChallenge())
        {
            ::Console::Online::GameParams tGameParams;
            if (::Console::Online::PartyGetGameParams(tGameParams) && tGameParams.m_bIsSeasonal && !::Console::Screens::IsScreenSetOpen())
            {
                ::Console::Screens::OpenToScreen(::Console::Screens::SEASON_JOURNEY_SCREEN_SET, 0, nControllerIndex, FALSE);
            }
        }
    }
}


















































void OpenForController(const int32 nControllerIndex)
{
    if (GameIsWeeklyChallenge())
        return;

    GameSoftPause(TRUE);
    OnlineService::ClearAchievementsCache();
    ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_SEASON_JOURNEY, TRUE, GetUIIDNone());

    sgtUISeasonJourneyData.idPlayer = ::Console::ClientAchievementsInterface::UserIndexToGameAccount(nControllerIndex);
    sgtUISeasonJourneyData.Initialize();
    sgtUISeasonJourneyData.m_bShowingRewards = FALSE;
    sgtUISeasonJourneyData.m_nControllerIndex = nControllerIndex;

    SeasonJourneyProgressionView eSeasonJourneyProgressionView;
    OnlineService::AchievementId idAchDefinitionToDisplay = 0;
    sgtUISeasonJourneyData.m_arAdvancedAchievementIds.clear();
    sgtUISeasonJourneyData.m_arCasualAchievementIds.clear();
    SeasonJourney_GetAchievementsToDisplay(idAchDefinitionToDisplay, eSeasonJourneyProgressionView, sgtUISeasonJourneyData.m_arCasualAchievementIds, sgtUISeasonJourneyData.m_arAdvancedAchievementIds, sgtUISeasonJourneyData.idPlayer);

    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::AchievementsInterface & tAchievementsInterface = *sgtAppGlobals.ptAchievementsInterface;
    sgtUISeasonJourneyData.m_nHighestCompletedChapter = 0;
    for (uint32 n = 0; n < NUM_CASUAL_CHAPTERS; n++)
    {
        SeasonJourneyUIChapter tChapter;
        tChapter.m_idAchievement = sgtUISeasonJourneyData.m_arCasualAchievementIds[NUM_CASUAL_CHAPTERS - 1 - n];
        OnlineService::CriteriaIdList arCriteria;
        tAchievements.GetAchievementCriteria(tChapter.m_idAchievement, arCriteria);
        int32 nSize = arCriteria.size();
        for (int32 n = 0; n < (int32)arCriteria.size(); n++)
        {
            const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition(arCriteria[n]);
            if (ptCritDef == NULL || (ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_HIDDEN)))
                nSize--;
        }
        tChapter.m_nCriteria = nSize;
        OnlineService::AchievementRecord tRecord;
        tChapter.m_bComplete = tAchievementsInterface.FindAwardedAchievement(sgtUISeasonJourneyData.idPlayer, sub_71000b4be0(), tChapter.m_idAchievement, tRecord);
        if (tChapter.m_bComplete)
            sgtUISeasonJourneyData.m_nHighestCompletedChapter = n + 1;
        sgtUISeasonJourneyData.m_arChapters.push_back(tChapter);
    }
    for (uint32 n = 0; n < NUM_ADVANCED_CHAPTERS; n++)
    {
        SeasonJourneyUIChapter tChapter;
        tChapter.m_idAchievement = sgtUISeasonJourneyData.m_arAdvancedAchievementIds[NUM_ADVANCED_CHAPTERS - 1 - n];
        OnlineService::CriteriaIdList arCriteria;
        tAchievements.GetAchievementCriteria(tChapter.m_idAchievement, arCriteria);
        int32 nSize = arCriteria.size();
        for (int32 n = 0; n < (int32)arCriteria.size(); n++)
        {
            const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition(arCriteria[n]);
            if (ptCritDef == NULL || (ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_HIDDEN)))
                nSize--;
        }
        tChapter.m_nCriteria = nSize;
        OnlineService::AchievementRecord tRecord;
        tChapter.m_bComplete = tAchievementsInterface.FindAwardedAchievement(sgtUISeasonJourneyData.idPlayer, sub_71000b4be0(), tChapter.m_idAchievement, tRecord);
        if (tChapter.m_bComplete)
            sgtUISeasonJourneyData.m_nHighestCompletedChapter = NUM_CASUAL_CHAPTERS + n + 1;
        sgtUISeasonJourneyData.m_arChapters.push_back(tChapter);
    }
    if (sgtUISeasonJourneyData.m_nHighestCompletedChapter > NUM_CHAPTERS)
        sgtUISeasonJourneyData.m_nHighestCompletedChapter = NUM_CHAPTERS;

    sgtUISeasonJourneyData.tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_SEASON_JOURNEY_RING, UIWND_CONSOLE_SEASON_JOURNEY_RING_CURSOR, RingIncludeControlPredicate, FALSE);
    sgtUISeasonJourneyData.tRingUI.SelectControl(GetElementUIIDForCategory(Math::Min(sgtUISeasonJourneyData.m_nHighestCompletedChapter, (int32)NUM_CHAPTERS - 1)), NULL);
    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_WRAPPER).SetActive(FALSE);
    FocusCategoryRing(TRUE);
    sgtUISeasonJourneyData.ToggleRewardsView(FALSE);
    RefreshRingBlinkers();
    RefreshChapters();
    SetTitleText(FALSE);

    UIText & tPlayerLabel = UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_PLAYER_LABEL);
    if (GameIsStartUp())
    {
        tPlayerLabel.SetText("", FALSE);
    }
    else
    {
        Player * ptPlayer = PlayerGetByControllerIndex(sgtUISeasonJourneyData.m_nControllerIndex);
        if (ptPlayer)
            tPlayerLabel.SetText(CPlayerGetIndicatorString(*ptPlayer).str(), FALSE);
    }
}



































































void OnClose(const UIID & id, const int32 nControllerIndex)
{
    GameSoftPause(FALSE);
    OnlineService::ClearAchievementsCache();
    if (::Console::Screens::IsScreenSetOpen(::Console::Screens::SEASON_JOURNEY_SCREEN_SET))
    {
        ::Console::Screens::CloseScreenSet(::Console::Screens::SEASON_JOURNEY_SCREEN_SET, FALSE);
    }
    ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_SEASON_JOURNEY);
}







void OnButton1(const UIID & id, const int32 nControllerIndex)
{
    if (UI_GetControllerFocus() == GetUIIDNone())
    {
        if (sgtUISeasonJourneyData.m_bShowingRewards)
        {
            sgtUISeasonJourneyData.ToggleRewardsView(FALSE);
        }
        FocusCategoryRing(FALSE);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
    }
}








void OnButton2(const UIID & id, const int32 nControllerIndex)
{
    const UIID & idControllerFocus = UI_GetControllerFocus();
    if (idControllerFocus == GetUIIDNone())
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
        OnClose(id, nControllerIndex);
    }
    else if (idControllerFocus == UIWRAP_CONSOLE_SEASON_JOURNEY_WRAPPER)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        FocusCategoryRing(TRUE);
        RefreshChapters();
        sgtUISeasonJourneyData.ToggleRewardsView(FALSE);
    }
}









void OnButton3(const UIID & id, const int32 nControllerIndex)
{
    sgtUISeasonJourneyData.ToggleRewardsView(!sgtUISeasonJourneyData.m_bShowingRewards);
}






void OnButton4(const UIID & id, const int32 nControllerIndex)
{
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (tActionManager.GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_4) == ACTION_REDEEM && !GameIsStartUp())
    {
        TryToRedeemGift();
    }
    else if (tActionManager.GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_4) == ACTION_TOGGLE_SORT_ORDER) {
        BOOL bRingEnabled = sgtUISeasonJourneyData.tRingUI.IsEnabled();
        sgtUISeasonJourneyData.tSeasonJourneyData.m_bSortedByComplete = !sgtUISeasonJourneyData.tSeasonJourneyData.m_bSortedByComplete;
        const OnlineService::AchievementDefinition * ptDefinition = sgtAppGlobals.ptAchievements->GetAchievementDefinition(sgtUISeasonJourneyData.tSeasonJourneyData.m_idDisplayedAchievement);
        if (ptDefinition) {
            sgtUISeasonJourneyData.tSeasonJourneyData.PopulateCriteriaDisplay(*ptDefinition);
            UI_GetConsoleListBoxFromID(UIWRAP_CONSOLE_SEASON_JOURNEY_WRAPPER).ResetAllItems();
            if (bRingEnabled)
                UI_GetConsoleListBoxFromID(UIWRAP_CONSOLE_SEASON_JOURNEY_WRAPPER).SetSelectedIndex(-1);
        }
    }
}


void OnL2(const UIID &, const int32 nControllerIndex)
{
    if (!::Console::ControllerUtils::IsButtonDown(sgtUISeasonJourneyData.m_nControllerIndex, 7))
    {
        UIUtils::Console::ScrollContentInScrollBox(UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS, ".LoadHaedrigsGift.ScrollBox"), -4.0f);
    }
}








void OnR2(const UIID &, const int32 nControllerIndex)
{
    if (!::Console::ControllerUtils::IsButtonDown(sgtUISeasonJourneyData.m_nControllerIndex, 6))
    {
        UIUtils::Console::ScrollContentInScrollBox(UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS, ".LoadHaedrigsGift.ScrollBox"), 4.0f);
    }
}








void Close()
{
    if (IsActive())
    {
        UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY).SetActive(FALSE);
        sgtUISeasonJourneyData.m_arChapters.clear();
        sgtUISeasonJourneyData.m_arCasualAchievementIds.clear();
        sgtUISeasonJourneyData.m_arAdvancedAchievementIds.clear();
        GameSoftPause(FALSE);
    }
}




void OnUpdate(const UIID &)
{
    if (sgtUISeasonJourneyData.tRingUI.IsEnabled())
    {
        sgtUISeasonJourneyData.tRingUI.UpdateInput(sgtUISeasonJourneyData.m_nControllerIndex, IconSelectionChanged, NULL);
    }
}



BOOL IsActive()
{
    return UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY).IsActive();
}



BOOL RingIncludeControlPredicate(const UIID & idControl)
{
    return UI_GetControlFromID(idControl).IsType(UIT_BUTTON);
}




static void RefreshRingBlinkers()
{
    uint32 nNumCategories = sgtUISeasonJourneyData.m_arChapters.size();
    for (uint32 i = 0; i < nNumCategories; i++)
    {
        UIID idBlinker = GetBlinkerUIIDForCategory(i);
        if (idBlinker != GetUIIDNone())
        {
            UI_GetWindowFromID(idBlinker).SetActive(FALSE);
        }
    }
}



void RefreshChapters()
{
    uint32 nNumChapters = sgtUISeasonJourneyData.m_arChapters.size();
    for (uint32 i = 0; i < nNumChapters; i++)
    {
        UIID idElement = GetElementUIIDForCategory(i);
        if (idElement != GetUIIDNone())
        {
            UI_GetButtonFromID(idElement).SetEnabled(sgtUISeasonJourneyData.IsChapterActive(i) || sgtUISeasonJourneyData.IsChapterComplete(i));
        }
    }
}



UIID GetElementUIIDForCategory(int32 nIndex)
{
    switch (nIndex)
    {
    case 0:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".Chapter1");
    case 1:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".Chapter2");
    case 2:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".Chapter3");
    case 3:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".Chapter4");
    case 4:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".Slayer");
    case 5:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".Champion");
    case 6:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".Destroyer");
    case 7:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".Conqueror");
    case 8:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".Guardian");
    }
    return GetUIIDNone();
}





















UIID GetBlinkerUIIDForCategory(int32 nIndex)
{
    switch (nIndex)
    {
    case 0:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".Chapter1New");
    case 1:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".Chapter2New");
    case 2:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".Chapter3New");
    case 3:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".Chapter4New");
    case 4:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".SlayerNew");
    case 5:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".ChampionNew");
    case 6:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".DestroyerNew");
    case 7:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".ConquerorNew");
    case 8:
        return UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_RING, ".GuardianNew");
    }
    return GetUIIDNone();
}






















CRefString GetChapterTitle(int32 nIndex)
{
    CRefString sLabel("ConsoleSeasonJourney:");
    switch (nIndex)
    {
    case 0: sLabel += "SeasonJourneyChapter1Title"; break;
    case 1: sLabel += "SeasonJourneyChapter2Title"; break;
    case 2: sLabel += "SeasonJourneyChapter3Title"; break;
    case 3: sLabel += "SeasonJourneyChapter4Title"; break;
    case 4: sLabel += "SeasonJourneyAdvancedTitle1Slayer"; break;
    case 5: sLabel += "SeasonJourneyAdvancedTitle2Champion"; break;
    case 6: sLabel += "SeasonJourneyAdvancedTitle3Destroyer"; break;
    case 7: sLabel += "SeasonJourneyAdvancedTitle4Conqueror"; break;
    case 8: sLabel += "SeasonJourneyAdvancedTitle5Guardian"; break;
    }
    return StringListFetch(sLabel);
}














































void SetTitleText(BOOL bShowRewards)
{
    UIText & tText = UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_TITLE);
    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_TITLE_BLINKER).SetActive(bShowRewards);
    if (bShowRewards)
    {
        tText.SetText(StringListFetch("ConsoleSeasonJourney:RewardsListTitle"), FALSE);
        tText.SetTextColor(0);
    }
    else
    {
        tText.SetText(GetChapterTitle(sgtUISeasonJourneyData.m_nSelectedCategory), FALSE);
        tText.SetTextColor(5);
    }
}







void FocusCategoryRing(BOOL bFocus)
{
    const UIControlItemArray & arChildren = UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_RING).GetChildList();
    for (uint32 i = 0; i < arChildren.size(); i++)
    {
        if (arChildren[i].ptControl->IsType(UIT_BUTTON) && sgtUISeasonJourneyData.tRingUI.GetSelectedControlID() != arChildren[i].id)
        {
            UIButton * ptButton = (UIButton *)arChildren[i].ptControl;
            if (bFocus)
            {
                ptButton->SetButtonState(BS_NORMAL, FALSE, FALSE);
                ptButton->SetEnabled(TRUE);
                ptButton->SetShader(SNO_NONE);
            }
            else
            {
                ptButton->SetButtonState(BS_NORMAL, FALSE, FALSE);
                ptButton->SetEnabled(FALSE);
                ptButton->SetShader(GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
            }
        }
    }

    sgtUISeasonJourneyData.tRingUI.SetEnabled(bFocus);

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();
    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_MENU_GLOW).SetActive(!bFocus);
    UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_LIST_TITLE).SetTextColor(bFocus ? crgbaYellow : crgbaWhite);

    if (bFocus)
    {
        UI_SetControllerFocus(GetUIIDNone());
        sgtUISeasonJourneyData.tRingUI.SelectControl(sgtUISeasonJourneyData.tRingUI.GetSelectedControlID(), IconSelectionChanged);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_CHOOSE_CATEGORY, StringListFetch("ConsoleAchievements:ChooseCategory"));
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close"));
    }
    else
    {
        UI_SetControllerFocus(UIWRAP_CONSOLE_SEASON_JOURNEY_WRAPPER);
        UI_GetConsoleListBoxFromID(UIWRAP_CONSOLE_SEASON_JOURNEY_WRAPPER).SetSelectedIndex(0);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("Console:Back"));
    }

    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_VIEW_OBJECTIVES, StringListFetch("ConsoleSeasonJourney:ViewRewards"));
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_TOGGLE_SORT_ORDER, StringListFetch("ConsoleAchievements:ChangeSort"));
    ::Console::Screens::UpdateButtonLegendText();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(FALSE);

    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_DIMMER).SetActive(bFocus);
    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_RING_GLOW).SetActive(bFocus);
}







void UpdateChapterIcons(int32 nSelectedChapter)
{
    UIWrapPanel & tChapterIcons = UI_GetWrapPanelFromID(UILIST_CONSOLE_SEASON_JOURNEY_CHAPTER_ICONS);
    UIWindow & tAdvancedChapterIcon = UI_GetWindowFromID(UIWND_SEASON_JOURNEY_ADVANCED_CHAPTER_ICON);
    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    UIControlSourceKey tTemplateSrcKey = UIControlSourceKey::Parse("ConsoleSeasonJourney:ConsoleSeasonJourney_main.LayoutRoot.Progression.IconsTemplate", -1, NULL, NULL);
    if (nSelectedChapter < NUM_CASUAL_CHAPTERS)
    {
        tChapterIcons.DeleteAllChildren();
        tChapterIcons.SetActive(TRUE);
        tAdvancedChapterIcon.SetActive(FALSE);
        for (int32 n = 0; n < NUM_CASUAL_CHAPTERS; n++)
        {
            const OnlineService::AchievementDefinition * ptDefinition = tAchievements.GetAchievementDefinition(sgtUISeasonJourneyData.m_arChapters[n].m_idAchievement);
            if (!ptDefinition)
                return;
            UIWindow * ptWindow = (UIWindow *)UI_CreateControlFromTemplate(tTemplateSrcKey, DynamicControlCreationParams(CRefString().Format("season_journey_progression %d", n), UILIST_CONSOLE_SEASON_JOURNEY_CHAPTER_ICONS), NULL);
            if (ptWindow)
            {
                UIID idComplete;
                idComplete.SetFromParent(ptWindow->GetID(), "Complete");
                UIID idIncomplete;
                idIncomplete.SetFromParent(ptWindow->GetID(), "Incomplete");
                UIID idCurrent;
                idCurrent.SetFromParent(ptWindow->GetID(), "Current");
                BOOL bIsCurrentAchievementCompleted = SeasonJourney_IsAchievementCompleted(sgtUISeasonJourneyData.idPlayer, ptDefinition->m_id);
                BOOL bIsCurrentAchievementActive = sgtUISeasonJourneyData.IsChapterActive(n);
                ImageTextureFrame tIconTexture;
                tIconTexture = ImageTextureFrame(CRefString().Format("UI:%s", ptDefinition->m_pszIconTexture));
                UI_GetWindowFromID(ptWindow->GetID()).SetTextureFrame(tIconTexture);
                ptWindow->SetShader(SNO_NONE);
                if (UI_TestID(idComplete))
                    UI_GetWindowFromID(idComplete).SetActive(bIsCurrentAchievementCompleted);
                if (!bIsCurrentAchievementCompleted)
                {
                    UI_GetWindowFromID(idIncomplete).SetActive(!bIsCurrentAchievementActive);
                    UI_GetWindowFromID(idCurrent).SetActive(bIsCurrentAchievementActive);
                }
                if (!bIsCurrentAchievementActive && !bIsCurrentAchievementCompleted)
                    ptWindow->SetShader(GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
            }
        }
    }
    else
    {
        tChapterIcons.SetActive(FALSE);
        tAdvancedChapterIcon.SetActive(TRUE);
        const OnlineService::AchievementDefinition * ptDefinition = tAchievements.GetAchievementDefinition(sgtUISeasonJourneyData.m_arChapters[nSelectedChapter].m_idAchievement);
        if (ptDefinition)
        {
            UIID idComplete;
            idComplete.SetFromParent(UIWND_SEASON_JOURNEY_ADVANCED_CHAPTER_ICON, "Complete");
            UIID idIncomplete;
            idIncomplete.SetFromParent(UIWND_SEASON_JOURNEY_ADVANCED_CHAPTER_ICON, "Incomplete");
            UIID idCurrent;
            idCurrent.SetFromParent(UIWND_SEASON_JOURNEY_ADVANCED_CHAPTER_ICON, "Current");
            BOOL bIsCurrentAchievementCompleted = SeasonJourney_IsAchievementCompleted(sgtUISeasonJourneyData.idPlayer, ptDefinition->m_id);
            BOOL bIsCurrentAchievementActive = sgtUISeasonJourneyData.IsChapterActive(nSelectedChapter);
            ImageTextureFrame tIconTexture;
                tIconTexture = ImageTextureFrame(CRefString().Format("UI:%s", ptDefinition->m_pszIconTexture));
            tAdvancedChapterIcon.SetTextureFrame(tIconTexture);
            tAdvancedChapterIcon.SetShader(SNO_NONE);
            if (UI_TestID(idComplete))
                UI_GetWindowFromID(idComplete).SetActive(bIsCurrentAchievementCompleted);
            if (!bIsCurrentAchievementCompleted)
            {
                UI_GetWindowFromID(idIncomplete).SetActive(!bIsCurrentAchievementActive);
                UI_GetWindowFromID(idCurrent).SetActive(bIsCurrentAchievementActive);
            }
            if (!bIsCurrentAchievementActive && !bIsCurrentAchievementCompleted)
                tAdvancedChapterIcon.SetShader(GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
        }
    }
}






























GBID GetItemRewardForAchievement(OnlineService::AchievementId idAchievement)
{
    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    if (SeasonJourney_IsCasualAchievement(sgtUISeasonJourneyData.m_arCasualAchievementIds, idAchievement))
    {
        OnlineService::RewardIdList arRewards;
        tAchievements.GetAchievementRewards(idAchievement, arRewards);
        for (uint32 i = 0; i < arRewards.size(); i++)
        {
            OnlineService::RewardDefinition tReward;
            if (tAchievements.GetRewardDefinition(arRewards[i], tReward) && tReward.m_type == OnlineService::REWARD_TYPE_ITEM)
            {
                OnlineService::RewardDefinition tItemReward;
                if (tAchievements.GetRewardDefinition(arRewards[i], tItemReward))
                {
                    return (GBID)tItemReward.m_id;
                }
            }
        }
    }
    return GBID_NONE;
}













BOOL IsGiftRedeemedByPlayer(int32 nControllerIndex, GBID gbidGift)
{
    D3::Hero::SavedDefinition tHeroDefinition = ::Console::GamerProfile::GetHero(nControllerIndex);
    if (tHeroDefinition.has_digest())
    {
        const D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(nControllerIndex);
        const D3::Account::AccountPartition & tPartition = tAccountDefinition.partitions(ACCOUNT_PARTITION_SEASON_NORMAL);
        if (tPartition.has_console_partition_data())
        {
            for (int32 n = 0; n < tPartition.console_partition_data().seasonal_gift_gbids_redeemed_size(); n++)
            {
                if (tPartition.console_partition_data().seasonal_gift_gbids_redeemed(n) == gbidGift)
                    return TRUE;
            }
        }
        const D3::Account::AccountPartition & tHardcorePartition = tAccountDefinition.partitions(ACCOUNT_PARTITION_SEASON_HARDCORE);
        if (tHardcorePartition.has_console_partition_data())
        {
            for (int32 n = 0; n < tHardcorePartition.console_partition_data().seasonal_gift_gbids_redeemed_size(); n++)
            {
                if (tHardcorePartition.console_partition_data().seasonal_gift_gbids_redeemed(n) == gbidGift)
                    return TRUE;
            }
        }
    }
    return FALSE;
}
}  // namespace Console
}  // namespace UISeasonJourney
namespace UIUtils {
namespace Console {
void DisplayPortrait(PlayerClass eClass, BOOL bFemale, int32 nAltLevel, const UIID & idPortrait, GBID gbidPortrait);  // X/Client/UserInterface/Console/UIConsoleUtils.cpp:1043
}  // namespace Console
}  // namespace UIUtils
SNO VisualInventoryGetPennantSnoFromGbid(GBID gbidItem);  /* VisualInventory.cpp:1330 */  SNO VisualInventoryGetPetSnoFromGbid(GBID gbidItem);  /* VisualInventory.cpp:1302 */  void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType, const PlayerClass eClass, const BOOL fIsMale);  /* CActorInventory.cpp:133 */
ItemQualityLevel ItemGetDisplayedQualityLevel(const D3::Items::Generator & tItem);  /* Items.cpp:2229 */  CRefString ItemGetPrimaryDescription(const ActorCommonData & tACD);  /* Items.cpp:11319 */
extern UIID UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_PET, UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_PENNANT, UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_PORTRAIT, UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_WINGS, UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_RIGHT_PANEL_COMPLETE, UITEXT_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS_TITLE, UITEXT_CONSOLE_SEASON_JOURNEY_REWARDS_RIGHT_PANEL_TITLE, UIRACTOR_CONSOLE_SEASONJOURNEY_REWARDS_WINGS, UIRACTOR_CONSOLE_SEASONJOURNEY_REWARDS_PENNANT, UIRACTOR_CONSOLE_SEASONJOURNEY_REWARDS_PET, UITEXT_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS_PROGRESS, UITEXT_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS_COMPLETE;  // X/Client/UserInterface/UIGlobals.cpp:989-1006
namespace UISeasonJourney {
namespace Console {
void DisplayRewardsForChapter(
    int32 nSelectedChapter)
{

    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_PET).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_PENNANT).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_PORTRAIT).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_WINGS).SetActive(FALSE);

    // Inference: the tail achievement is element 0 of the casual list (original loads [x8] at line 1439).
    OnlineService::AchievementId idAchievement = sgtUISeasonJourneyData.m_arChapters[nSelectedChapter].m_idAchievement;
    OnlineService::AchievementId idTailAchievement = sgtUISeasonJourneyData.m_arCasualAchievementIds[0];

    BOOL bIsCasualAch = SeasonJourney_IsCasualAchievement(sgtUISeasonJourneyData.m_arCasualAchievementIds, idAchievement);
    BOOL bTailComplete = bIsCasualAch && SeasonJourney_IsAchievementCompleted(sgtUISeasonJourneyData.idPlayer, idTailAchievement);
    BOOL bCurrentComplete = SeasonJourney_IsAchievementCompleted(sgtUISeasonJourneyData.idPlayer, idAchievement);

    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_RIGHT_PANEL_COMPLETE).SetActive(bTailComplete || (!bIsCasualAch && bCurrentComplete));

    UIID idLeftProgress = UIIDCreateFromParent(UITEXT_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS_TITLE, ".RewardsHaedigsGiftTitleLeftProgress");
    UIID idLeftComplete = UIIDCreateFromParent(UITEXT_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS_TITLE, ".RewardsHaedigsGiftTitleLeftComplete");

    UIID idRightProgress = UIIDCreateFromParent(UITEXT_CONSOLE_SEASON_JOURNEY_REWARDS_RIGHT_PANEL_TITLE, ".RewardsCosmeticsTitleRightProgress");
    UIID idRightComplete = UIIDCreateFromParent(UITEXT_CONSOLE_SEASON_JOURNEY_REWARDS_RIGHT_PANEL_TITLE, ".RewardsCosmeticsTitleRightComplete");

    UIText & tLeftProgress = UI_GetTextFromID(idLeftProgress);
    UIText & tLeftComplete = UI_GetTextFromID(idLeftComplete);

    UIText & tRightProgress = UI_GetTextFromID(idRightProgress);
    UIText & tRightComplete = UI_GetTextFromID(idRightComplete);

    if (bCurrentComplete)
    {
        tLeftProgress.SetActive(FALSE);
        tLeftComplete.SetActive(TRUE);
    }
    else
    {
        tLeftProgress.SetActive(TRUE);
        tRightProgress.SetActive(TRUE);

        tLeftComplete.SetActive(FALSE);
        tRightComplete.SetActive(FALSE);
    }

    CRefString strProgress = LocalizedSprintf(StringListFetch("ConsoleSeasonJourney:SeasonJourneyRewards"), GetChapterTitle(nSelectedChapter).str());
    tLeftProgress.SetText(strProgress.str(), FALSE);

    if (nSelectedChapter < NUM_CASUAL_CHAPTERS - 1)
    {
        strProgress = LocalizedSprintf(StringListFetch("ConsoleSeasonJourney:SeasonJourneyRewards"), GetChapterTitle(NUM_CASUAL_CHAPTERS - 1).str());
    }

    tRightProgress.SetText(strProgress.str(), FALSE);

    if (bTailComplete || (!bIsCasualAch && bCurrentComplete))
    {
        tRightProgress.SetActive(FALSE);
        tRightComplete.SetActive(TRUE);
    }
    else
    {
        tRightProgress.SetActive(TRUE);
        tRightComplete.SetActive(FALSE);
    }


    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;

    OnlineService::RewardIdList arAchRewards;
    if (bIsCasualAch)
    {
        tAchievements.GetAchievementRewards(idTailAchievement, arAchRewards);
    }
    else
    {
        tAchievements.GetAchievementRewards(idAchievement, arAchRewards);
    }

    for (uint32 i = 0; i < arAchRewards.size(); i++)
    {
        OnlineService::RewardDefinition tRewardDef;
        if (tAchievements.GetRewardDefinition(arAchRewards[i], tRewardDef) &&




            tRewardDef.m_type == OnlineService::REWARD_TYPE_COSMETIC)
        {
            GBID gbidReward = tRewardDef.m_id;
            CosmeticItemType eCosmeticType = CosmeticItems_GetCosmeticItemTypeFromGBID(gbidReward);

            switch (eCosmeticType) {
            case COSMETIC_ITEM_FRAME: {
                D3::Hero::SavedDefinition & tHeroDefinition = ::Console::GamerProfile::GetHero(UI_GetPrimaryUIController());

                uint32 uPlayerFlags = tHeroDefinition.digest().player_flags();

                PlayerClass eClass = PlayerClassFromGBID(tHeroDefinition.digest().gbid_class());
                BOOL bIsFemale = (uPlayerFlags & (1 << PLAYERFLAG_FEMALE)) != 0;

                const UIID & idPortraitFrame = UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_PORTRAIT, ".PortraitFrame");

                UIUtils::Console::DisplayPortrait(eClass, bIsFemale, 0, idPortraitFrame, gbidReward);

                UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_PORTRAIT).SetActive(TRUE);
                break;
            }
            case COSMETIC_ITEM_PENNANT: {
                UIRActor & tRactor = UI_GetRActorFromID(UIRACTOR_CONSOLE_SEASONJOURNEY_REWARDS_PENNANT);
                tRactor.Cleanup();
                tRactor.DeleteRActor();
                SNO snoPennant = VisualInventoryGetPennantSnoFromGbid(gbidReward);
                if (snoPennant != SNO_NONE)
                {
                    PRTransform transform(cgqIdentity, cgwpNull);
                    RActorID ractorID = RActorCreate(
                        snoPennant,
                        1.0f,
                        -1.0f,
                        transform,
                        0,
                        tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), -1);
                    tRactor.SetRActor(ractorID);
                    if (RActor * ptPennantRActor = RActorTryToGet(ractorID))
                    {
                        RActorSetIrradianceMap(ptPennantRActor, GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE));
                    }

                    LightID idLight = LightAdd(
                        0,
                        UI_GetDefaultHeroLight(),
                        PRTransform(cgqIdentity, cgwpNull),
                        FALSE,
                        tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
                    tRactor.SetLight(idLight);
                }

                UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_PENNANT).SetActive(TRUE);
                break;
            }
            case COSMETIC_ITEM_WINGS: {
                D3::Hero::SavedDefinition & tHeroDefinition = ::Console::GamerProfile::GetHero(UI_GetPrimaryUIController());
                uint32 uPlayerFlags = tHeroDefinition.digest().player_flags();

                PlayerClass eClass = PlayerClassFromGBID(tHeroDefinition.digest().gbid_class());
                BOOL bIsFemale = (uPlayerFlags & (1 << PLAYERFLAG_FEMALE)) != 0;

                const D3::Hero::Digest & tHeroDigest = tHeroDefinition.digest();

                VisualEquipment tVisualEquipment;
                tVisualEquipment.Init();
                tVisualEquipment.Import(tHeroDigest.visual_equipment());

                for (int32 i = VCIS_FIRST; i < VCIS_NUM_SLOTS; i++)
                {
                    tVisualEquipment.tCosmeticItems[i].gbidItem = GBID_NONE;
                }

                tVisualEquipment.tCosmeticItems[VCIS_WINGS].gbidItem = gbidReward;

                PRTransform transform(cgqIdentity, cgwpNull);
                SNO snoActor = GetFrontendCharacterSNO(eClass, bIsFemale);
                UIRActor & tRactor = UI_GetRActorFromID(UIRACTOR_CONSOLE_SEASONJOURNEY_REWARDS_WINGS);
                tRactor.Cleanup();
                tRactor.DeleteRActor();
                if (snoActor != SNO_NONE)
                {
                    RActorID ractorID = UI_CreateHeroUIRActor(
                        snoActor,
                        GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE),
                        tVisualEquipment,
                        0,
                        eClass,
                        bIsFemale,
                        transform,
                        tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(),
                        0x68700,
                        1.0f,
                        TRUE);
                    tRactor.SetRActor(ractorID);

                    LightID idLight = LightAdd(
                        0,
                        UI_GetDefaultHeroLight(),
                        PRTransform(cgqIdentity, cgwpNull),
                        FALSE,
                        tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
                    tRactor.SetLight(idLight);
                }

                UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_WINGS).SetActive(TRUE);
                break;
            }
            case COSMETIC_ITEM_PET: {
                UIRActor & tRactor = UI_GetRActorFromID(UIRACTOR_CONSOLE_SEASONJOURNEY_REWARDS_PET);
                tRactor.Cleanup();
                tRactor.DeleteRActor();
                SNO snoPet = VisualInventoryGetPetSnoFromGbid(gbidReward);
                if (snoPet != SNO_NONE)
                {
                    PRTransform transform(cgqIdentity, cgwpNull);
                    RActorID ractorID = RActorCreate(
                        snoPet,
                        1.0f,
                        -1.0f,
                        transform,
                        0,
                        tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), -1);
                    tRactor.SetRActor(ractorID);
                    if (RActor * ptPetRActor = RActorTryToGet(ractorID))
                    {
                        RActorSetIrradianceMap(ptPetRActor, GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE));
                    }

                    LightID idLight = LightAdd(
                        0,
                        UI_GetDefaultHeroLight(),
                        PRTransform(cgqIdentity, cgwpNull),
                        FALSE,
                        tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
                    tRactor.SetLight(idLight);
                }
                UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_PET).SetActive(TRUE);
            }
            }
        }
    }
    GBID gbidReward = GetItemRewardForAchievement(sgtUISeasonJourneyData.m_arChapters[nSelectedChapter].m_idAchievement);

    BOOL bShowArmorSet = bIsCasualAch && gbidReward != GBID_NONE;

    BOOL bRedeemed = !GameIsStartUp() && IsGiftRedeemedByPlayer(sgtUISeasonJourneyData.m_nControllerIndex, gbidReward);

    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS).SetActive(bShowArmorSet);

    UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_TAB_LEFT).SetActive(bShowArmorSet);

    if (bShowArmorSet)
    {
        UIID idIcon = UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS, ".LoadHaedrigsGift.Icon");
        UIID idTitle = UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS, ".LoadHaedrigsGift.ItemName");
        UIID idRedeem = UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS, ".LoadHaedrigsGift.Redeem");
        UIID idDetails = UIIDCreateFromParent(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS, ".LoadHaedrigsGift.ScrollBox.Affixes");

        // Inference: DWARF has an inner block (lines 1675-1700) with no branch of its own; a bare scope.
        {
            ImageTextureFrame tIcon;
            ACDInventoryGetItemDrawInfoWithGBID(gbidReward, tIcon, ITEMDRAW_NORMAL, 1, TRUE);  // 1: class argument as in the original (w3 = 1)

            UIItemButton & tButton = UI_GetItemButtonFromID(idIcon);


            D3::Items::Generator tGenerator;
            ItemGeneratorToDefaults(tGenerator);
            tGenerator.mutable_gb_handle()->set_gbid(gbidReward);
            ACDID idACDItem = CItemGenerate((ACDNetworkName)0x201c, tGenerator, NULL);

            tButton.SetQualityLevel(ItemGetDisplayedQualityLevel(tGenerator));
            tButton.SetAllTextureFrames(tIcon);

            CRefString strName = ItemGetDisplayName(gbidReward);
            UI_GetTextFromID(idTitle).SetText(strName.str(), FALSE);
            UI_GetTextFromID(idTitle).SetTextColor(3);  // FontSpecifier 3 (original immediate)


            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            if (ptACDItem)
            {
                CRefString strDescription = ItemGetPrimaryDescription(*ptACDItem);
                UI_GetTextFromID(idDetails).SetText(strDescription.str(), FALSE);
            }
            ACDDelete(idACDItem, FALSE);
        }

        UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS_PROGRESS).SetActive(!bCurrentComplete && !bRedeemed);
        UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS_COMPLETE).SetActive(bCurrentComplete && !bRedeemed);
        UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS_REDEEMED).SetActive(bRedeemed);
        UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_LEFT_PANEL_COMPLETE).SetActive(bCurrentComplete && !bRedeemed);

        ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_GIFT_DETAILS, StringListFetch("ConsoleSeasonJourney:ScrollGiftText"));
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_GIFT_DETAILS, StringListFetch("ConsoleSeasonJourney:ScrollGiftText"));

        if (bCurrentComplete && !bRedeemed && !GameIsStartUp())
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_REDEEM, StringListFetch("ConsoleSeasonJourney:ClaimReward"));
        }
    }
    else
    {
        ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
        tActionManager.ClearAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER);
        tActionManager.ClearAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER);
    }

    ::Console::Screens::UpdateButtonLegendText();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(FALSE);
}


void IconSelectionChanged(const ::Console::RingWrapper & tRingWrapper, const UIID & idIcon, const BOOL bSelected)
{
    if (idIcon == GetUIIDNone())
        return;

    UI_GetButtonFromID(idIcon).SetButtonState(bSelected ? BS_PUSHED : BS_NORMAL, FALSE, FALSE);

    int32 nSelectedCategory = GetCategoryIndexForElementUIID(idIcon);
    sgtUISeasonJourneyData.m_nSelectedCategory = nSelectedCategory;
    if (nSelectedCategory == -1)
        return;

    UI_GetButtonFromID(idIcon).SetButtonState(bSelected ? BS_MOUSEOVER : BS_NORMAL, FALSE, FALSE);
    if (bSelected)
    {
        if (sgtUISeasonJourneyData.m_bShowingRewards)
            DisplayRewardsForChapter(nSelectedCategory);
        sgtUISeasonJourneyData.tSeasonJourneyData.Initialize(sgtUISeasonJourneyData.idPlayer, sgtUISeasonJourneyData.m_arChapters[nSelectedCategory]);
        UI_GetConsoleListBoxFromID(UIWRAP_CONSOLE_SEASON_JOURNEY_WRAPPER).SetDataSource(sgtUISeasonJourneyData.tSeasonJourneyData);
        UI_GetConsoleListBoxFromID(UIWRAP_CONSOLE_SEASON_JOURNEY_WRAPPER).ResetAllItems();
        UI_GetConsoleListBoxFromID(UIWRAP_CONSOLE_SEASON_JOURNEY_WRAPPER).SetSelectedIndex(-1);
    }

    CRefString strSeason = LocalizedSprintf(StringListFetch("ConsoleSeasonJourney:Season"), UnicodeText::FormatNumber((int32)OnlineService::GetSeasonNum(), TRUE, FNS_NORMAL, XLOCALE_INVALID).str());
    CRefString strChapter = GetChapterTitle(nSelectedCategory);
    if (sgtUISeasonJourneyData.IsChapterComplete(nSelectedCategory) ||
        sgtUISeasonJourneyData.IsChapterActive(nSelectedCategory))
    {
        UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_SEASON_PROGRESSION_TITLE).SetActive(nSelectedCategory < NUM_CASUAL_CHAPTERS);
        UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_SEASON_PROGRESSION_INCOMPLETE).SetActive(FALSE);
    }
    else
    {
        UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_SEASON_PROGRESSION_TITLE).SetActive(FALSE);
        UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_SEASON_PROGRESSION_INCOMPLETE).SetActive(TRUE);
    }

    if (sgtUISeasonJourneyData.IsChapterComplete(nSelectedCategory))
    {
        UI_GetWindowFromID(UITEXT_CONSOLE_SEASON_JOURNEY_SEASON_BANNER).SetActive(FALSE);
        UI_GetWindowFromID(UITEXT_CONSOLE_SEASON_JOURNEY_CHAPTER_COMPLETE_SEASON_BANNER).SetActive(TRUE);
        UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_CHAPTER_COMPLETE_SEASON_NAME).SetText(strSeason.str(), FALSE);
        UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_CHAPTER_COMPLETE_SEASON_STEP_NAME).SetText(strChapter.str(), FALSE);
    }
    else
    {
        UI_GetWindowFromID(UITEXT_CONSOLE_SEASON_JOURNEY_CHAPTER_COMPLETE_SEASON_BANNER).SetActive(FALSE);
        UI_GetWindowFromID(UITEXT_CONSOLE_SEASON_JOURNEY_SEASON_BANNER).SetActive(TRUE);
        UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_SEASON_NAME).SetText(strSeason.str(), FALSE);
        UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_SEASON_STEP_NAME).SetText(strChapter.str(), FALSE);
    }

    UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_TITLE_LEFT).SetText(LocalizedSprintf(StringListFetch("ConsoleSeasonJourney:SeasonJourneyObjectivesProgress"),
                                                                                        UnicodeText::FormatNumber(sgtUISeasonJourneyData.GetCompletedObjectivesForChapter(nSelectedCategory), TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                                                                                        UnicodeText::FormatNumber(sgtUISeasonJourneyData.GetTotalObjectivesForChapter(nSelectedCategory), TRUE, FNS_NORMAL, XLOCALE_INVALID).str()), FALSE);
    if (nSelectedCategory < NUM_CASUAL_CHAPTERS)
    {
        int32 nHighestCasualChapterCompleted = Math::Min(sgtUISeasonJourneyData.m_nHighestCompletedChapter, (int32)NUM_CASUAL_CHAPTERS);
        UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_SEASON_PROGRESSION_TITLE).SetText(LocalizedSprintf(StringListFetch("ConsoleSeasonJourney:CompletedChapters"),
                                                                                                          UnicodeText::FormatNumber(nHighestCasualChapterCompleted, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                                                                                                          UnicodeText::FormatNumber((int32)NUM_CASUAL_CHAPTERS, TRUE, FNS_NORMAL, XLOCALE_INVALID).str()), FALSE);
    }
    SetTitleText(sgtUISeasonJourneyData.m_bShowingRewards);
    UpdateChapterIcons(nSelectedCategory);
}


















static int32 GetCategoryIndexForElementUIID(const UIID & id)
{
    uint32 nNumCategories = Math::Min((uint32)sgtUISeasonJourneyData.m_arChapters.size(), (uint32)NUM_CHAPTERS);
    for (uint32 i = 0; i < nNumCategories; i++)
    {
        if (id == GetElementUIIDForCategory(i))
            return i;
    }
    return -1;
}





void PopulateCommonUIElements(const UIID & idWindow, const OnlineService::AchievementDefinition & tAchievementDefinition, const OnlineService::CriteriaDefinition & tCriteriaDefinition, const OnlineService::GameAccountHandle & idPlayer, BOOL bSelected)
{
    UIID idActual = UIIDCreateFromParent(idWindow, ".LayoutRoot.Backgrounds.SeasonJourneyObjective");
    UIID idTitle = UIIDCreateFromParent(idActual, ".ObjectiveTitle");
    UIID idGiftIcon = UIIDCreateFromParent(idActual, ".GiftIcon");
    UIID idCheckbox = UIIDCreateFromParent(idActual, ".Checkbox.CompleteCheck");
    UIID idCover = UIIDCreateFromParent(idActual, ".SeasonJourneyObjectiveListItemLocked");

    BOOL bComplete = SeasonJourney_IsAchievementCompleted(idPlayer, tAchievementDefinition.m_id);
    CRefString sAchievementTitle = StringListFetch(tAchievementDefinition.m_pszNameKey);
    CRefString sAchievementDesc = StringListFetch(tAchievementDefinition.m_pszDescriptionKey);

    if (bSelected)
    {
        sgtUISeasonJourneyData.m_bCurrentSelectionActiveOrComplete = bComplete || sgtUISeasonJourneyData.m_nSelectedCategory <= sgtUISeasonJourneyData.m_nHighestCompletedChapter;
        if (!sgtUISeasonJourneyData.m_bShowingRewards)
        {
            UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_DESCRIPTION_TITLE).SetText(sAchievementTitle.str(), FALSE);
            UI_GetTextFromID(UITEXT_CONSOLE_SEASON_JOURNEY_DESCRIPTION_DETAILS).SetText(sAchievementDesc.str(), FALSE);
            UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_DETAILS).SetActive(sgtUISeasonJourneyData.m_bCurrentSelectionActiveOrComplete);
        }
    }

    UI_GetWindowFromID(idCover).SetActive(!bComplete && sgtUISeasonJourneyData.m_nSelectedCategory > sgtUISeasonJourneyData.m_nHighestCompletedChapter);
    UI_GetWindowFromID(idCheckbox).SetActive(bComplete);

    CRefString strName = sAchievementTitle;
    if (!bComplete && (tAchievementDefinition.m_flags & (1 << OnlineService::ACHIEVEMENT_FLAG_HAS_PROGRESS_BAR)) && !OnlineService::IsCriteriaEarned(tCriteriaDefinition.m_id, sub_71000b4be0()))
    {
        OnlineService::CriteriaQuantity nEarnedCriteria = 0;
        OnlineService::CriteriaQuantity nTotalCriteria = 0;
        OnlineService::GetAchievementStatus(idPlayer, tAchievementDefinition.m_id, sub_71000b4be0(), &nEarnedCriteria, &nTotalCriteria);
        strName = LocalizedSprintf(StringListFetch("ConsoleSeasonJourney:CriteriaWithProgress"), sAchievementTitle.str(),
                                   UnicodeText::FormatNumber(nEarnedCriteria, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                                   UnicodeText::FormatNumber(nTotalCriteria, TRUE, FNS_NORMAL, XLOCALE_INVALID).str());
    }
    UI_GetTextFromID(idTitle).SetText(strName.str(), FALSE);
}




















void TryToRedeemGift()
{
    int32 nControllerIndex = sgtUISeasonJourneyData.m_nControllerIndex;
    OnlineService::AchievementId idAchievement = sgtUISeasonJourneyData.m_arChapters[sgtUISeasonJourneyData.m_nSelectedCategory].m_idAchievement;
    if (!SeasonJourney_IsCasualAchievement(sgtUISeasonJourneyData.m_arCasualAchievementIds, idAchievement))
        return;
    if (!SeasonJourney_IsAchievementCompleted(sgtUISeasonJourneyData.idPlayer, idAchievement))
        return;
    GBID gbidGift = GetItemRewardForAchievement(idAchievement);
    if (gbidGift == GBID_NONE)
        return;
    if (IsGiftRedeemedByPlayer(nControllerIndex, gbidGift))
        return;

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ACDInventoryGetNumEmptySlots(*ACDTryToGet(ptPlayer->idPlayerACD), 0) > 0)
    {
        GBIDDataMessage tMessage;
        tMessage.gbidData = gbidGift;
        MessageSendToServer(CMSG_REDEEM_SEASON_REWARD_CONSOLE, &tMessage, sizeof(tMessage), PlayerGetByControllerIndex(nControllerIndex));
        sgtUISeasonJourneyData.m_gbidGiftBeingRedeemed = gbidGift;
    }
    else
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszMessage = StringListFetch("ConsoleSeasonJourney:InventoryFull");
        ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
    }
}




void GiftRedeemedResult(int32 nControllerIndex, BOOL bResult)
{
    if (bResult)
    {
        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_SEASON_JOURNEY_GIFT_REDEEMED);
        tStoryboard.Stop();
        tStoryboard.Play(FALSE, 0.0f);

        BOOL bHardcore = PlayerIsHardcore(PlayerGetByControllerIndex(nControllerIndex));
        D3::Account::SavedDefinition & tAccount = ::Console::GamerProfile::GetAccount(nControllerIndex);
        tAccount.mutable_partitions(bHardcore ? ACCOUNT_PARTITION_SEASON_HARDCORE : ACCOUNT_PARTITION_SEASON_NORMAL)->mutable_console_partition_data()->add_seasonal_gift_gbids_redeemed(sgtUISeasonJourneyData.m_gbidGiftBeingRedeemed);

        UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_GIFT_HAEDRIGS_REDEEMED).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_CONSOLE_SEASON_JOURNEY_REWARDS_LEFT_PANEL_COMPLETE).SetActive(FALSE);
        ::Console::Screens::GetActionManager().ClearAction(::Console::CONTROLLER_INPUT_BUTTON_4);
        ::Console::Screens::UpdateButtonLegendText();
    }
    else
    {
        sgtUISeasonJourneyData.m_gbidGiftBeingRedeemed = GBID_NONE;
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszMessage = StringListFetch("ConsoleSeasonJourney:DeliveryError");
        ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
    }
}

}  // namespace Console
}  // namespace UISeasonJourney
