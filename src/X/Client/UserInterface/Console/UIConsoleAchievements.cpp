// Original file: X/Client/UserInterface/Console/UIConsoleAchievements.cpp
//
// The console challenges (achievements) screen: the category ring, the challenge list and the
// detail view.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "ConsoleInputUtilities.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "Wrappers/RingWrapper.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Controls/UIRActorControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/Online.h"
#include "../../CGame.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Achievements/Console/ConsoleAchievements.h"
#include "../../../Common/OnlineService/Achievements.h"
#include "../../../Common/OnlineService/OnlineService.h"
#include "../../../Sigma/RefString.h"
#include "../../Sound/Sound.h"
#include "../Controls/UIButtonControl.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../Controls/UIConsoleScrollBoxControl.h"
#include "../../../Common/Player.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Common/Protobuf/Account.pb.h"
#include "../../../Common/Protobuf/Hero.pb.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../Battlenet/UI/Util/UIAchievementsUtil.h"
#include "../Controls/UIProgressBarControl.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"

// Declared here because their headers are not adopted yet. Each comment names the defining file.
CRefString StringListFetch(LPCSTR szLabel);  // X/Common/Definitions/StringList.cpp
const UIID & UI_GetControllerFocus();  // X/Client/UserInterface/UI.cpp
UIRActor & UI_GetRActorFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
namespace OnlineService { void ClearAchievementsCache(); }  // X/Common/OnlineService/Achievements.cpp
// X/Client/Console/GamerProfileInternal.cpp
namespace Console { namespace GamerProfile {
void OnChallengeViewed(int32 nUserIdx, OnlineService::AchievementId idAchievement);
BOOL IsChallengeNew(int32 nUserIdx, OnlineService::AchievementId idAchievement);
} }
SNO UI_GetDefaultHeroLight();  // X/Client/UserInterface/UI.cpp
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);  // X/Client/Lights.cpp
namespace UIUtils { namespace Console { SNO GetObserverForPlayerClass(PlayerClass ePlayerClass); } }  // UIConsoleUtils.cpp
namespace UIInventory { namespace Console {  // X/Client/UserInterface/Console/UIInventoryConsole.cpp
void UpdatePlayerActorUI(const UIID & idRActorControl, VisualInventory & tVisualInventory, PlayerClass eClass, BOOL bIsFemale, uint32 dwAnimTag);
} }
namespace Console { namespace UIOnlineActions {  // X/Client/UserInterface/Console/OnlineActions.cpp
bool IsActiveSeason(uint32 uSeasonNum, const D3::Account::Digest & tAccountDigest);
} }
namespace Console { namespace Online { BOOL IsSeasonsInitialized(); } }  // X/Client/Console/Online.cpp
// The screen's public entry points; their declarations live in a header nobody has adopted yet.
namespace UIAchievements { namespace Console {
void ActivateRing(BOOL bSeasonal);
void UpdateProgress(BOOL bSeasonal);
void OpenForController(int32 nControllerIndex);
void Close();
BOOL IsVisible();
} }

// X/Client/UserInterface/UIIDs.cpp
extern UIID UIWND_CONSOLE_ACHIEVEMENTS;
extern UIID UIWND_CONSOLE_ACHIEVEMENTS_RING;
extern UIID UIWND_CONSOLE_ACHIEVEMENTS_RING_CURSOR;
extern UIID UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING;
extern UIID UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING_CURSOR;
extern UIID UIWND_CONSOLE_ACHIEVEMENTS_SEASONAL_HEADER;
extern UIID UITEXT_CONSOLE_ACHIEVEMENTS_LIST_TITLE;
extern UIID UITEXT_CONSOLE_ACHIEVEMENTS_NAVIGATION_TITLE;
extern UIID UILIST_CONSOLE_ACHIEVEMENTS;
extern UIID UISCROLL_CONSOLE_ACHIEVEMENTS_DETAILVIEW;
extern UIID UIRACTOR_CONSOLE_ACHIEVEMENTS_HERO;
extern UIID UIPROGRESS_CONSOLE_ACHIEVEMENTS_OVERALL;
extern UIID UIPROGRESS_CONSOLE_ACHIEVEMENTS_CATEGORY;
extern UIID UITEXT_CONSOLE_ACHIEVEMENTS_CATEGORY;
extern UIID UITEXT_CONSOLE_ACHIEVEMENTS_CATEGORY_PERCENTAGE;
extern UIID UIWND_CONSOLE_ACHIEVEMENTS_MENU_GLOW;
extern UIID UIWND_CONSOLE_ACHIEVEMENTS_SEASON_MENU_GLOW;
extern UIID UIWND_CONSOLE_ACHIEVEMENTS_DIMMER;
extern UIID UIWND_CONSOLE_ACHIEVEMENTS_RING_GLOW;
UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
extern UIID UITEXT_CONSOLE_ACHIEVEMENTS_OVERALL_PERCENTAGE;
extern UIID UITEXT_CONSOLE_ACHIEVEMENTS_TOTAL_POINTS;

namespace UIAchievements {
namespace Console {
namespace {

enum {
    PROGRESS_BAR = 0,
    CHECKLIST = 1,
};

BOOL RingIncludeControlPredicate(const UIID & idControl);
void RefreshRingBlinkers();
void IconSelectionChanged(const ::Console::RingWrapper & tRingWrapper, const UIID & idIcon, const BOOL bSelected);
void FocusCategoryRing(BOOL bFocus);
void ShowDetailView(BOOL bShow);
void LoadHeroModel(int32 nControllerIndex);
BOOL CategoryHasNewAchievements(OnlineService::CategoryId idCategory);
int32 GetCategoryIndexForElementUIID(const UIID & id);
UIID GetElementUIIDForCategory(int32 nIndex);
UIID GetBlinkerUIIDForCategory(int32 nIndex);
void UpdateCategoryDisplay(OnlineService::CategoryId idCategory, BOOL bSeasonal);
BOOL IsAchievementNew(OnlineService::AchievementId idAchievement);
void MarkChallengeViewed(OnlineService::AchievementId idChallenge);
uint32 GetPointsAndResolveSeriesAchievement(OnlineService::AchievementDefinition & tDefinition, const OnlineService::GameAccountHandle & idPlayer, BOOL * pbIsSeries, OnlineService::AchievementIdList * parSeriesAchievements);
void PopulateCommonUIElements(const UIID & idWindow, const OnlineService::AchievementDefinition & tAchievementDefinition, const OnlineService::GameAccountHandle & idPlayer, uint32 nPointsValue, BOOL bShowEarnedStatus);
void FilterAndSortAchievements(const OnlineService::GameAccountHandle & idPlayer, OnlineService::AchievementIdList & arUnearnedAchievements, OnlineService::AchievementIdList & arEarnedAchievements);
BOOL sShouldShowAchievement(const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementDefinition & tDef);

}  // namespace

enum {
    NUM_CATEGORIES = 9,
};





















namespace {
// DWARF owner declarations for genuine helper calls, bodies below.
void PopulateDetailView(const OnlineService::AchievementDefinition &, const OnlineService::GameAccountHandle &);
uint32 GetCriteriaFlags(const OnlineService::AchievementDefinition &);
void PopulateProgressBar(OnlineService::CriteriaQuantity, OnlineService::CriteriaQuantity);
void PopulateSeries(const OnlineService::AchievementIdList &);
void PopulateRewards(const OnlineService::RewardIdList &);
void PopulateCriteriaDisplay(const OnlineService::AchievementDefinition &);
}





















class AchievementsDataSource : public ConsoleListBoxDataSource {
public:

    AchievementsDataSource() : m_idDisplayedCategory(0), m_bSortUnearnedFirst(FALSE), m_bShowingSeasonal(FALSE) {}


virtual int32 GetNumItems() const
{
    return (int32)m_arAchievements.size();
}




virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const
{
    return UIControlSourceKey::Parse("ConsoleAchievements:ConsoleAchievements_main.ListItem", -1, NULL, NULL);
}




virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32)
{
}






virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    UIID idHighlight = UIIDCreateFromParent(idItemUI, ".Highlight");
    UIID idBlinker = UIIDCreateFromParent(idItemUI, ".Star");

    BOOL bSelected = tListBox.GetSelectedIndex() == nIndex;
    BOOL bNew = IsAchievementNew(m_arAchievements[nIndex]);
    UI_GetWindowFromID(idHighlight).SetActive(bSelected);
    UI_GetWindowFromID(idBlinker).SetActive(bNew);

    if (bSelected && bNew && UI_GetPrimaryUIController() != -1)
    {
        MarkChallengeViewed(m_arAchievements[nIndex]);
        RefreshRingBlinkers();
    }

    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    const OnlineService::AchievementDefinition * ptAchievementDefinition = tAchievements.GetAchievementDefinition(m_arAchievements[nIndex]);
    if (ptAchievementDefinition)
    {
        OnlineService::AchievementDefinition tAchievementDefinition = *ptAchievementDefinition;
        uint32 nPoints = GetPointsAndResolveSeriesAchievement(tAchievementDefinition, m_idPlayer, NULL, NULL);
        PopulateCommonUIElements(idItemUI, tAchievementDefinition, m_idPlayer, nPoints, TRUE);
    }
}





    void Initialize(const OnlineService::GameAccountHandle & idPlayer, OnlineService::CategoryId idDisplayedCategory, BOOL bSeasonal)
    {
        if (m_idDisplayedCategory != idDisplayedCategory || m_bShowingSeasonal != bSeasonal)
        {
            m_idPlayer = idPlayer;
            m_idDisplayedCategory = idDisplayedCategory;
            m_arUnearnedAchievements.clear();
            m_arEarnedAchievements.clear();
            m_bShowingSeasonal = bSeasonal;

            sgtAppGlobals.ptAchievements->GetCategoryAchievements(idDisplayedCategory, m_arUnearnedAchievements);
            FilterAndSortAchievements(idPlayer, m_arUnearnedAchievements, m_arEarnedAchievements);

            OnlineService::CategoryIdList arChildCategories;
            if (sgtAppGlobals.ptAchievements->GetChildCategories(m_idDisplayedCategory, arChildCategories))
            {
                blz::sort(arChildCategories.begin(), arChildCategories.end(), OnlineService::CategorySortPredicate);
                for (uint32 * childIter = arChildCategories.begin(); childIter < arChildCategories.end(); ++childIter)
                {
                    OnlineService::CategoryIdList arGrandChildCategories;
                    if (sgtAppGlobals.ptAchievements->GetChildCategories(*childIter, arGrandChildCategories))
                    {
                        blz::sort(arGrandChildCategories.begin(), arGrandChildCategories.end(), OnlineService::CategorySortPredicate);
                        childIter = arChildCategories.insert(childIter + 1, arGrandChildCategories.begin(), arGrandChildCategories.end()) - 1;
                    }

                    OnlineService::AchievementIdList arUnearnedCategoryAchievements;
                    OnlineService::AchievementIdList arEarnedCategoryAchievements;
                    if (sgtAppGlobals.ptAchievements->GetCategoryAchievements(*childIter, arUnearnedCategoryAchievements))
                    {
                        FilterAndSortAchievements(idPlayer, arUnearnedCategoryAchievements, arEarnedCategoryAchievements);
                        blzext::vector_append(m_arUnearnedAchievements, arUnearnedCategoryAchievements);
                        blzext::vector_append(m_arEarnedAchievements, arEarnedCategoryAchievements);
                    }
                }
            }

            SortAchievements();
        }
    }










void SortAchievements()
{
    m_arAchievements.clear();
    if (m_bSortUnearnedFirst)
    {
        blzext::vector_append(m_arAchievements, m_arUnearnedAchievements);
        blzext::vector_append(m_arAchievements, m_arEarnedAchievements);
    }
    else
    {
        blzext::vector_append(m_arAchievements, m_arEarnedAchievements);
        blzext::vector_append(m_arAchievements, m_arUnearnedAchievements);
    }
}



void ToggleSortOrder()
{
    m_bSortUnearnedFirst = !m_bSortUnearnedFirst;
    SortAchievements();
}

    OnlineService::GameAccountHandle m_idPlayer;
    OnlineService::CategoryId m_idDisplayedCategory;
    OnlineService::AchievementIdList m_arUnearnedAchievements;
    OnlineService::AchievementIdList m_arEarnedAchievements;
    OnlineService::AchievementIdList m_arAchievements;
    BOOL m_bSortUnearnedFirst;
    BOOL m_bShowingSeasonal;
};


struct UIAchievementsData {
    OnlineService::GameAccountHandle idPlayer;
    UIAchievements::Console::AchievementsDataSource tAchievementsData;
    OnlineService::CategoryIdList arCategories;
    OnlineService::CategoryIdList arSeasonalCategories;
    RGBAColor rgbaTitleColor;
    ::Console::RingWrapper tRingUI;
    ::Console::AnalogStickInputFilter tRingStickInputFilter;
    BOOL bInitialized;
    BOOL bSeasonal;
    BOOL bSeasonAvailable;







    UIAchievementsData() : bInitialized(FALSE) {}






    void Initialize()
    {
        if (!bInitialized)
        {
            tRingStickInputFilter.AddRule(::Console::AnalogStickInputFilter::Rule::CatchAllRule);

            ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
            if (tAchievements.GetChildCategories(0, arCategories))
            {
                blz::sort(arCategories.begin(), arCategories.end(), OnlineService::CategorySortPredicate);

                for (uint32 * iter = arCategories.begin(); iter != arCategories.end(); ++iter)
                {
                    const OnlineService::CategoryDefinition * ptCategoryDefinition = tAchievements.GetCategoryDefinition(*iter);
                    if (ptCategoryDefinition &&
                        !(ptCategoryDefinition->m_flags & (1 << OnlineService::CATEGORY_FLAG_HIDDEN)) &&
                        !(ptCategoryDefinition->m_flags & (1 << OnlineService::CATEGORY_FLAG_SEASON_JOURNEY)) &&
                        OnlineService::CanShowCategory(*iter, TRUE))
                    {
                        arSeasonalCategories.push_back(*iter);
                    }
                }
            }

            bInitialized = TRUE;
        }
    }
};












UIAchievementsData sgtUIAchievementsData;












void ActivateRing(BOOL bSeasonal)
{
    FocusCategoryRing(TRUE);
    UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_RING).SetActive(!bSeasonal);
    UI_GetWindowFromID(UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING).SetActive(bSeasonal);
    UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_SEASONAL_HEADER).SetActive(bSeasonal);

    if (bSeasonal)
    {
        sgtUIAchievementsData.tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING, UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING_CURSOR, RingIncludeControlPredicate, FALSE);
        UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_NAVIGATION_TITLE).SetText(StringListFetch("Console:SeasonChallenges"), FALSE);
    }
    else
    {
        sgtUIAchievementsData.tRingUI.WrapUsingContainerControl(UIWND_CONSOLE_ACHIEVEMENTS_RING, UIWND_CONSOLE_ACHIEVEMENTS_RING_CURSOR, RingIncludeControlPredicate, FALSE);
        UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_NAVIGATION_TITLE).SetText(StringListFetch("Console:Challenges"), FALSE);
    }

    sgtUIAchievementsData.tRingUI.SelectControl(sgtUIAchievementsData.bSeasonal ? UIIDCreateFromParent(UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING, ".SeasonGeneral") : UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".General"), IconSelectionChanged);
    ShowDetailView(FALSE);
    RefreshRingBlinkers();
}

















void UpdateProgress(BOOL bSeasonal)
{
    if (bSeasonal)
    {
        uint32 nTotal = 0;
        uint32 nEarned = 0;
        OnlineService::CalculateCategoryPoints(sgtUIAchievementsData.idPlayer, 0, nTotal, nEarned, TRUE);
        UI_GetProgressBarFromID(UIPROGRESS_CONSOLE_ACHIEVEMENTS_OVERALL).SetExtentsAndPosition(0.0f, (float)nTotal, (float)nEarned, -1.0f);
        float flPercentage = 0.0f;
        if (nTotal)
            flPercentage = (float)nEarned / (float)nTotal;
        CRefString sPercentage = UnicodeText::FormatNumber(flPercentage, FALSE, FNS_PERCENT, 0, 0, XLOCALE_INVALID);
        UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_OVERALL_PERCENTAGE).SetText(sPercentage, FALSE);
        CRefString sScore = LocalizedSprintf(StringListFetch("ConsoleAchievements:AchievementScore").str(), UnicodeText::FormatNumber((int32)nEarned, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
        sScore += " {icon:BattleNetAchievements_ScoreIcon, 2.0}";
        UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_TOTAL_POINTS).SetText(sScore, FALSE);
    }
    else
    {
        uint32 nTotal = 0;
        uint32 nEarned = 0;
        OnlineService::CalculateCategoryPoints(sgtUIAchievementsData.idPlayer, 0, nTotal, nEarned, FALSE);
        UI_GetProgressBarFromID(UIPROGRESS_CONSOLE_ACHIEVEMENTS_OVERALL).SetExtentsAndPosition(0.0f, (float)nTotal, (float)nEarned, -1.0f);
        float flPercentage = 0.0f;
        if (nTotal)
            flPercentage = (float)nEarned / (float)nTotal;
        CRefString sPercentage = UnicodeText::FormatNumber(flPercentage, FALSE, FNS_PERCENT, 0, 0, XLOCALE_INVALID);
        UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_OVERALL_PERCENTAGE).SetText(sPercentage, FALSE);
        CRefString sScore = LocalizedSprintf(StringListFetch("ConsoleAchievements:AchievementScore").str(), UnicodeText::FormatNumber((int32)nEarned, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
        sScore += " {icon:BattleNetAchievements_ScoreIcon, 2.0}";
        UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_TOTAL_POINTS).SetText(sScore, FALSE);
    }
}






void OpenForController(int32 nControllerIndex)
{
    OnlineService::ClearAchievementsCache();
    ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_ACHIEVEMENTS, TRUE, GetUIIDNone());

    sgtUIAchievementsData.tAchievementsData.m_idDisplayedCategory = 0;
    sgtUIAchievementsData.Initialize();

    const D3::Account::SavedDefinition & tAccount = ::Console::GamerProfile::GetAccount(nControllerIndex);
    sgtUIAchievementsData.idPlayer = ::Console::ClientAchievementsInterface::UserIndexToGameAccount(nControllerIndex);
    sgtUIAchievementsData.rgbaTitleColor = UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_LIST_TITLE).GetTextColor();

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("ConsoleUI:Close"));

    if (GameIsStartUp())
    {
        const D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(nControllerIndex);
        D3::Hero::SavedDefinition & tHeroDef = ::Console::GamerProfile::GetHero(nControllerIndex);
        sgtUIAchievementsData.bSeasonal = ::Console::UIOnlineActions::IsActiveSeason(tHeroDef.digest().season_created(), tAccountDefinition.digest());
    }
    else
    {
        sgtUIAchievementsData.bSeasonal = GameIsSeasonal();
    }

    sgtUIAchievementsData.bSeasonAvailable = ::Console::Online::IsSeasonsInitialized() && OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE;

    ActivateRing(sgtUIAchievementsData.bSeasonal);
    UpdateProgress(sgtUIAchievementsData.bSeasonal);
    LoadHeroModel(nControllerIndex);

    UI_GetConsoleScrollBoxFromID(UISCROLL_CONSOLE_ACHIEVEMENTS_DETAILVIEW).SetExternallyControlled(FALSE);
}














void Close()
{
    if (!IsVisible())
        return;

    UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_LIST_TITLE).SetTextColor(sgtUIAchievementsData.rgbaTitleColor);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_ACHIEVEMENTS).SetSelectedIndex(-1);
    UI_SetControllerFocus(GetUIIDNone());
    UI_GetRActorFromID(UIRACTOR_CONSOLE_ACHIEVEMENTS_HERO).Cleanup();
    ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_ACHIEVEMENTS);
    OnlineService::ClearAchievementsCache();
}









BOOL IsVisible()
{
    return UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS).IsActive();
}




void OnAccept(const UIID & id, int32 nControllerIndex)
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);

    const UIID & idControllerFocus = UI_GetControllerFocus();
    if (idControllerFocus == GetUIIDNone())
    {
        FocusCategoryRing(FALSE);
    }
    else if (idControllerFocus == UILIST_CONSOLE_ACHIEVEMENTS)
    {
        ShowDetailView(TRUE);
        UI_SetControllerFocus(UISCROLL_CONSOLE_ACHIEVEMENTS_DETAILVIEW);
    }
}





void OnCancel(const UIID & id, int32 nControllerIndex)
{
    const UIID & idControllerFocus = UI_GetControllerFocus();
    if (idControllerFocus == GetUIIDNone())
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
        Close();
    }
    else if (idControllerFocus == UILIST_CONSOLE_ACHIEVEMENTS)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        FocusCategoryRing(TRUE);
    }
    else if (idControllerFocus == UISCROLL_CONSOLE_ACHIEVEMENTS_DETAILVIEW)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        ShowDetailView(FALSE);
        UI_SetControllerFocus(UILIST_CONSOLE_ACHIEVEMENTS);
    }
}





void OnSort(const UIID & id, int32 nControllerIndex)
{
    if (UI_GetPrimaryUIController() == nControllerIndex)
    {
        UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_ACHIEVEMENTS);
        int32 nIndex = tListBox.GetSelectedIndex();
        OnlineService::AchievementId idAchievement = 0;
        if (nIndex != -1)
            idAchievement = sgtUIAchievementsData.tAchievementsData.m_arAchievements[nIndex];

        sgtUIAchievementsData.tAchievementsData.ToggleSortOrder();
        tListBox.ResetAllItems();

        if (idAchievement == 0)
        {
            tListBox.SetSelectedIndex(-1);
        }
        else
        {
            for (int32 i = 0; i < sgtUIAchievementsData.tAchievementsData.m_arAchievements.size(); i++)
            {
                if (sgtUIAchievementsData.tAchievementsData.m_arAchievements[i] == idAchievement)
                {
                    tListBox.SetSelectedIndex(i);
                    break;
                }
            }
        }
    }
}









void OnToggleSeasonal(const UIID & id, int32 nControllerIndex)
{
    if (UI_GetPrimaryUIController() == nControllerIndex && sgtUIAchievementsData.bSeasonAvailable)
    {
        sgtUIAchievementsData.bSeasonal = !sgtUIAchievementsData.bSeasonal;
        ActivateRing(sgtUIAchievementsData.bSeasonal);
        UpdateProgress(sgtUIAchievementsData.bSeasonal);
    }
}











void OnUpdate(const UIID &)
{
    if (sgtUIAchievementsData.tRingUI.IsEnabled())
    {
        sgtUIAchievementsData.tRingUI.UpdateInput(UI_GetPrimaryUIController(), IconSelectionChanged);
    }
}

namespace {



BOOL sShouldShowAchievement(const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementDefinition & tDef)
{
    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;

    if (tDef.m_flags & ((1 << OnlineService::ACHIEVEMENT_FLAG_STATISTIC) | (1 << OnlineService::ACHIEVEMENT_FLAG_HIDDEN)))
        return FALSE;

    if (tAchievements.IsAchievementInSeries(tDef.m_id))
    {
        if (!(tDef.m_flags & (1 << OnlineService::ACHIEVEMENT_FLAG_HIDE_SERIES)) &&
            OnlineService::GetHighestUnearnedAchievementInSeries(idPlayer, tDef.m_id) != tDef.m_id &&
            OnlineService::GetHighestEarnedAchievementInSeries(idPlayer, tDef.m_id) != tDef.m_id)
            return FALSE;

        if ((tDef.m_flags & (1 << OnlineService::ACHIEVEMENT_FLAG_HIDE_SERIES)) &&
            ((tDef.m_supersedes == 0 && OnlineService::IsHighestAchievementUnearnedInSeries(idPlayer, tDef.m_id)) ||
             OnlineService::IsHighestAchievementEarnedInSeries(idPlayer, tDef.m_id)))
            return FALSE;
    }
    else if ((tDef.m_flags & (1 << OnlineService::ACHIEVEMENT_FLAG_HIDDEN_TILL_AWARDED)) &&
             OnlineService::GetAchievementStatus(idPlayer, tDef.m_id, NULL, NULL) != OnlineService::EARNED)
    {
        return FALSE;
    }

    if (sgtUIAchievementsData.bSeasonal && !tAchievements.IsAchievementValidForSeason(&tDef, OnlineService::GetSeasonNum()))
        return FALSE;
    if (!sgtUIAchievementsData.bSeasonal && !tAchievements.IsAchievementValidForSeason(&tDef, 0))
        return FALSE;

    return TRUE;
}


























void FilterAndSortAchievements(const OnlineService::GameAccountHandle & idPlayer, OnlineService::AchievementIdList & arUnearnedAchievements, OnlineService::AchievementIdList & arEarnedAchievements)
{
    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    for (int32 i = 0; i < arUnearnedAchievements.size(); i++)
    {
        const OnlineService::AchievementDefinition * ptDefinition = tAchievements.GetAchievementDefinition(arUnearnedAchievements[i]);
        if (ptDefinition == NULL || !sShouldShowAchievement(idPlayer, *ptDefinition))
        {
            blzext::vector_erase_reorder(arUnearnedAchievements, i--);
        }
        else if (OnlineService::GetAchievementStatus(idPlayer, arUnearnedAchievements[i], NULL, NULL) == OnlineService::EARNED)
        {
            arEarnedAchievements.push_back(arUnearnedAchievements[i]);
            blzext::vector_erase_reorder(arUnearnedAchievements, i--);
        }
    }

    OnlineService::SortAchievementsByOrder(arUnearnedAchievements);
    OnlineService::SortAchievementsByOrder(arEarnedAchievements);
}

































BOOL RingIncludeControlPredicate(const UIID & idControl)
{
    return UI_GetControlFromID(idControl).IsType(UIT_BUTTON);
}




void RefreshRingBlinkers()
{
    size_t nNumCategories = Math::Min(sgtUIAchievementsData.arCategories.size(), (size_t)NUM_CATEGORIES);
    for (size_t i = 0; i < nNumCategories; i++)
    {
        UIID idBlinker = GetBlinkerUIIDForCategory((int32)i);
        if (idBlinker != GetUIIDNone())
        {
            UI_GetWindowFromID(idBlinker).SetActive(CategoryHasNewAchievements(sgtUIAchievementsData.arCategories[i]));
        }
    }
}



BOOL IsAchievementNew(OnlineService::AchievementId idAchievement)
{
    return UI_GetPrimaryUIController() != -1 && ::Console::GamerProfile::IsChallengeNew(UI_GetPrimaryUIController(), idAchievement);
}




BOOL CategoryHasNewAchievements(OnlineService::CategoryId idCategory)
{
    if (UI_GetPrimaryUIController() == -1)
        return FALSE;

    OnlineService::AchievementIdList arAchievements;
    sgtAppGlobals.ptAchievements->GetCategoryAchievements(idCategory, arAchievements);

    OnlineService::CategoryIdList arChildCategories;
    if (sgtAppGlobals.ptAchievements->GetChildCategories(idCategory, arChildCategories))
    {
        for (uint i = 0; i < arChildCategories.size(); i++)
        {
            OnlineService::CategoryIdList arGrandChildCategories;
            if (sgtAppGlobals.ptAchievements->GetChildCategories(arChildCategories[i], arGrandChildCategories))
            {
                for (uint32 j = 0; j < arGrandChildCategories.size(); j++)
                {
                    arChildCategories.push_back(arGrandChildCategories[j]);
                }
            }

            OnlineService::AchievementIdList arCategoryAchievements;
            if (sgtAppGlobals.ptAchievements->GetCategoryAchievements(arChildCategories[i], arCategoryAchievements))
                blzext::vector_append(arAchievements, arCategoryAchievements);
        }
    }

    for (uint32 i = 0; i < arAchievements.size(); i++)
    {
        if (::Console::GamerProfile::IsChallengeNew(UI_GetPrimaryUIController(), arAchievements[i]))
            return TRUE;
    }

    return FALSE;
}









void MarkChallengeViewed(OnlineService::AchievementId idChallenge)
{
    if (UI_GetPrimaryUIController() != -1 && UI_GetControllerFocus() == UILIST_CONSOLE_ACHIEVEMENTS)
    {
        ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
        while (const OnlineService::AchievementDefinition * ptDef = tAchievements.GetAchievementDefinition(idChallenge))
        {
            ::Console::GamerProfile::OnChallengeViewed(UI_GetPrimaryUIController(), idChallenge);
            idChallenge = ptDef->m_supersedes;
        }
    }
}






void IconSelectionChanged(const ::Console::RingWrapper & tRingWrapper, const UIID & idIcon, const BOOL bSelected)
{
    if (idIcon == GetUIIDNone())
        return;

    UI_GetButtonFromID(idIcon).SetButtonState(bSelected ? BS_PUSHED : BS_NORMAL, FALSE, FALSE);

    int32 nSelectedCategory = GetCategoryIndexForElementUIID(idIcon);
    if (nSelectedCategory == -1)
        return;

    UI_GetButtonFromID(idIcon).SetButtonState(bSelected ? BS_MOUSEOVER : BS_NORMAL, FALSE, FALSE);
    if (bSelected)
    {
        OnlineService::CategoryId idCategory = (sgtUIAchievementsData.bSeasonal ? sgtUIAchievementsData.arSeasonalCategories : sgtUIAchievementsData.arCategories)[nSelectedCategory];
        sgtUIAchievementsData.tAchievementsData.Initialize(sgtUIAchievementsData.idPlayer, idCategory, sgtUIAchievementsData.bSeasonal);
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_ACHIEVEMENTS).SetDataSource(sgtUIAchievementsData.tAchievementsData);
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_ACHIEVEMENTS).ResetAllItems();
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_ACHIEVEMENTS).SetSelectedIndex(-1);
        UpdateCategoryDisplay(idCategory, sgtUIAchievementsData.bSeasonal);
    }
}











int32 GetCategoryIndexForElementUIID(const UIID & id)
{
    size_t nNumCategories = Math::Min(sgtUIAchievementsData.arCategories.size(), (size_t)NUM_CATEGORIES);
    for (size_t i = 0; i < nNumCategories; i++)
    {
        if (id == GetElementUIIDForCategory((int32)i))
            return (int32)i;
    }
    return -1;
}






UIID GetElementUIIDForCategory(int32 nIndex)
{
    if (sgtUIAchievementsData.bSeasonal)
    {
        switch (nIndex)
        {
        case 0:
            return UIIDCreateFromParent(UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING, ".SeasonGeneral");
        case 1:
            return UIIDCreateFromParent(UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING, ".SeasonItems");
        case 2:
            return UIIDCreateFromParent(UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING, ".SeasonGreaterRifts");
        case 3:
            return UIIDCreateFromParent(UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING, ".SeasonConquests");
        }
    }
    else
    {
        switch (nIndex)
        {
        case 0:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".General");
        case 1:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".Adventure");
        case 2:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".Campaign");
        case 3:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".Cooperative");
        case 4:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".Hardcore");
        case 5:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".Classes");
        case 6:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".SetDungeons");
        case 7:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".Challenges");
        case 8:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".Crafting");
        }
    }




























    return GetUIIDNone();
}


UIID GetBlinkerUIIDForCategory(int32 nIndex)
{
    if (sgtUIAchievementsData.bSeasonal)
    {
        switch (nIndex)
        {
        case 0:
            return UIIDCreateFromParent(UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING, ".SeasonGeneralNew");
        case 1:
            return UIIDCreateFromParent(UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING, ".SeasonItemsNew");
        case 2:
            return UIIDCreateFromParent(UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING, ".SeasonGreaterRiftsNew");
        case 3:
            return UIIDCreateFromParent(UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING, ".SeasonConquestsNew");
        }
    }
    else
    {
        switch (nIndex)
        {
        case 0:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".GeneralNew");
        case 1:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".AdventureNew");
        case 2:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".CampaignNew");
        case 3:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".CooperativeNew");
        case 4:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".HardcoreNew");
        case 5:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".ClassesNew");
        case 6:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".SetDungeonsNew");
        case 7:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".ChallengesNew");
        case 8:
            return UIIDCreateFromParent(UIWND_CONSOLE_ACHIEVEMENTS_RING, ".CraftingNew");
        }
    }




























    return GetUIIDNone();
}


void FocusCategoryRing(BOOL bFocus)
{
    const UIControlItemArray & arChildren = UI_GetWindowFromID(sgtUIAchievementsData.bSeasonal ? UIWND_CONSOLE_SEASONAL_ACHIEVEMENTS_RING : UIWND_CONSOLE_ACHIEVEMENTS_RING).GetChildList();
    for (uint32 i = 0; i < arChildren.size(); i++)
    {
        if (arChildren[i].ptControl->IsType(UIT_BUTTON) && sgtUIAchievementsData.tRingUI.GetSelectedControlID() != arChildren[i].id)
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

    sgtUIAchievementsData.tRingUI.SetEnabled(bFocus);

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (sgtUIAchievementsData.bSeasonal)
    {
        UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_MENU_GLOW).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_SEASON_MENU_GLOW).SetActive(!bFocus);
    }
    else
    {
        UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_SEASON_MENU_GLOW).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_MENU_GLOW).SetActive(!bFocus);
    }

    UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_LIST_TITLE).SetTextColor(bFocus ? sgtUIAchievementsData.rgbaTitleColor : crgbaWhite);

    if (bFocus)
    {
        UI_SetControllerFocus(GetUIIDNone());
        sgtUIAchievementsData.tRingUI.SelectControl(sgtUIAchievementsData.tRingUI.GetSelectedControlID(), IconSelectionChanged);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("ConsoleAchievements:ChooseCategory"));
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("ConsoleUI:Close"));
    }
    else
    {
        UI_SetControllerFocus(UILIST_CONSOLE_ACHIEVEMENTS);
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_ACHIEVEMENTS).SetSelectedIndex(0);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("ConsoleAchievements:Details"));
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 0, StringListFetch("Console:Back"));
    }

    if (sgtUIAchievementsData.bSeasonAvailable)
    {
        if (sgtUIAchievementsData.bSeasonal)
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, 0, StringListFetch("ConsoleAchievements:ViewCareer"));
        else
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, 0, StringListFetch("ConsoleAchievements:ViewSeasonal"));
    }
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, 0, StringListFetch("ConsoleAchievements:ChangeSort"));
    ::Console::Screens::UpdateButtonLegendText();

    UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_DIMMER).SetActive(bFocus);
    UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_RING_GLOW).SetActive(bFocus);
}










void UpdateCategoryDisplay(OnlineService::CategoryId idCategory, BOOL bSeasonal)
{
    const OnlineService::CategoryDefinition * ptCatDef = sgtAppGlobals.ptAchievements->GetCategoryDefinition(idCategory);
    if (ptCatDef)
    {
        uint32 nTotal = 0;
        uint32 nEarned = 0;
        OnlineService::CalculateCategoryPoints(sgtUIAchievementsData.idPlayer, ptCatDef->m_id, nTotal, nEarned, bSeasonal);

        CRefString sName = StringListFetch(ptCatDef->m_pszNameKey);
        UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_CATEGORY).SetText(sName, FALSE);
        UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_LIST_TITLE).SetActive(TRUE);
        UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_LIST_TITLE).SetText(sName, FALSE);

        UI_GetProgressBarFromID(UIPROGRESS_CONSOLE_ACHIEVEMENTS_CATEGORY).SetExtentsAndPosition(0.0f, (float)nTotal, (float)nEarned, -1.0f);

        float flPercentage = 0.0f;
        if (nTotal)
            flPercentage = (float)nEarned / (float)nTotal;
        CRefString sPercentage = UnicodeText::FormatNumber(flPercentage, FALSE, FNS_PERCENT, 0, 0, XLOCALE_INVALID);
        UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_CATEGORY_PERCENTAGE).SetText(sPercentage, FALSE);
    }
}






void LoadHeroModel(int32 nControllerIndex)
{
    UIRActor & tHero = UI_GetRActorFromID(UIRACTOR_CONSOLE_ACHIEVEMENTS_HERO);

    LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE,
                               tHero.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
    tHero.SetLight(idLight);
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    // 1197: test order and the two branches from the line table (GameIsStartUp at 1197:21, || at 37; hero-definition branch on 1199-1207).
    if (GameIsStartUp() || ptPlayer == NULL)
    {
        D3::Hero::SavedDefinition & tHeroDef = ::Console::GamerProfile::GetHero(nControllerIndex);
        PlayerClass eClass = PlayerClassFromGBID(tHeroDef.digest().gbid_class());
        BOOL bIsFemale = (tHeroDef.digest().player_flags() & (1 << PLAYERFLAG_FEMALE)) ? TRUE : FALSE;
        VisualInventory tInventory;
        tInventory.Init();
        tInventory.tEquipmentCurrent.Import(tHeroDef.digest().visual_equipment());
        tHero.SetObserver(UIUtils::Console::GetObserverForPlayerClass(eClass));
        UIInventory::Console::UpdatePlayerActorUI(UIRACTOR_CONSOLE_ACHIEVEMENTS_HERO, tInventory, eClass, bIsFemale, 0x11000);
    }
    else
    {
        PlayerClass eClass = ptPlayer->ePlayerClassPortrait;
        BOOL bIsFemale = (ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_FEMALE)) ? TRUE : FALSE;
        VisualInventory * ptInventory = VisualInventoryGet(ptPlayer->idPlayerACD);
        tHero.SetObserver(UIUtils::Console::GetObserverForPlayerClass(eClass));
        if (ptInventory)
            UIInventory::Console::UpdatePlayerActorUI(UIRACTOR_CONSOLE_ACHIEVEMENTS_HERO, *ptInventory, eClass, bIsFemale, 0x11000);
    }
}
}  // namespace

}  // namespace Console
}  // namespace UIAchievements


#include "../../Battlenet/UI/Controls/ControlVisibilityManager.h"
#include "../../../Common/CosmeticItems.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/Achievements/AchievementDefines.h"
#include "../../ImageCollection.h"
// DWARF CActorInventory.cpp:133 and CAchievementUtils.cpp:105.


void ACDInventoryGetItemDrawInfoWithGBID(const GBID, ImageTextureFrame &, const ItemDrawType, const PlayerClass, const BOOL);
namespace Achievement { CRefString DisplayStringFromEvent(EventID, uint64, CRefString &); }
// DWARF UI.cpp:6196: UI_DeleteControl signature; existing body in that owner.


void UI_DeleteControl(const UIID &, BOOL);
// DWARF UIIDs.cpp: globals referenced by the original detail view.
extern UIID UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW;
extern UIID UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW_TITLE;
extern UIID UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW_SERIES;
extern UIID UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW_PROGRESS;
extern UIID UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW_CHECKLIST;
extern UIID UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW_REWARDS;
extern UIID UIWRAP_CONSOLE_ACHIEVEMENTS_DETAILVIEW_SERIES_PANEL;
extern UIID UIWRAP_CONSOLE_ACHIEVEMENTS_DETAILVIEW_REWARDS_PANEL;


extern UIID UIWRAP_CONSOLE_ACHIEVEMENTS_DETAILVIEW_CHECKLIST_PANEL;
extern UIID UITEXT_CONSOLE_ACHIEVEMENTS_DETAILVIEW_PROGRESS_TEXT;
extern UIID UIPROGRESS_CONSOLE_ACHIEVEMENTS_DETAILVIEW_PROGRESS_BAR;
namespace UIAchievements { namespace Console { namespace {
// Logical lines preserve original DWARF declarations; shared-line locals below have separate physical declarations.
#line 1221
// Inference: selection and scroll guards follow original branches at 1226/1229/1244.
void ShowDetailView(
    BOOL bShow)
{
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
#line 1225
    // Inference: Show/hide branches follow the original bShow test.
    if (bShow)
    {
        int32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_ACHIEVEMENTS).GetSelectedIndex();
#line 1228
    // Inference: The original skips detail population for selection -1.
        if (nSelectedIndex != -1)
        {


            UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW).SetActive(TRUE);
            UI_GetWindowFromID(UILIST_CONSOLE_ACHIEVEMENTS).SetActive(FALSE);

            ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
            const OnlineService::AchievementDefinition * ptDefinition = tAchievements.GetAchievementDefinition(
                sgtUIAchievementsData.tAchievementsData.m_arAchievements[nSelectedIndex]);
#line 1238
    // Inference: Missing achievement definitions skip only population.
            if (ptDefinition)
            {
                PopulateDetailView(*ptDefinition, sgtUIAchievementsData.idPlayer);
            }

#line 1243
    // Inference: Original scroll controls are enabled above the observed 1e-6 threshold.
            if (UI_GetConsoleScrollBoxFromID(UISCROLL_CONSOLE_ACHIEVEMENTS_DETAILVIEW).GetScrollLimit() > 0.000001f)
            {
                CRefString sScrollPrompt = StringListFetch("ConsoleAchievements:Scroll");
#line 1246
    // Inference: Original scroll action ID is 2; no named action constant is present in this CU.
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, 2, sScrollPrompt);
#line 1247
    // Inference: Original scroll action ID is 2.
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, 2, sScrollPrompt);
            }
        }
        tActionManager.ClearAction(::Console::CONTROLLER_INPUT_BUTTON_1);
    }
    else
    {
        UI_GetConsoleScrollBoxFromID(UISCROLL_CONSOLE_ACHIEVEMENTS_DETAILVIEW).SetScrollPos(0.0f);
        UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW).SetActive(FALSE);
        UI_GetWindowFromID(UILIST_CONSOLE_ACHIEVEMENTS).SetActive(TRUE);
#line 1257
    // Inference: Original details action ID is zero.
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("ConsoleAchievements:Details"));
#line 1258
    // Inference: Original back action ID is zero.
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 0, StringListFetch("Console:Back"));
        tActionManager.ClearAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER);
        tActionManager.ClearAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER);
    }
    ::Console::Screens::UpdateButtonLegendText();
}

uint32 GetPointsAndResolveSeriesAchievement(
    OnlineService::AchievementDefinition & tDefinition,
    const OnlineService::GameAccountHandle & idPlayer,
    BOOL * pbIsSeries,
    OnlineService::AchievementIdList * parSeriesAchievements)
{

    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;
    // Inference: original series and HIDE_SERIES tests gate the supersedes/highest-earned expression.
    BOOL bIsSeries = tAchievements.IsAchievementInSeries(tDefinition.m_id) &&
        !(tDefinition.m_flags & (1 << OnlineService::ACHIEVEMENT_FLAG_HIDE_SERIES)) &&
        // Inference: zero supersedes short-circuits the original highest-earned lookup.
        (!tDefinition.m_supersedes || OnlineService::GetHighestEarnedAchievementInSeries(idPlayer, tDefinition.m_id) == tDefinition.m_id);
#line 1279
    // Inference: A nonnull output pointer receives the series decision.
    if (pbIsSeries)
    {
        *pbIsSeries = bIsSeries;
    }
    // Inference: the original accumulator starts at zero before traversing the series.
    uint32 nPointsValue = 0;
#line 1285
    // Inference: Only the series branch resolves and accumulates series points.
    if (bIsSeries)
    {
        OnlineService::AchievementIdList arSeriesAchievementIds;
        OnlineService::GetFilteredSupercededAchievements(
            idPlayer,
            tAchievements.GetSeriesTail(tDefinition.m_id), arSeriesAchievementIds);

#line 1292
    // Inference: A nonnull output vector receives the filtered series.
        if (parSeriesAchievements)
        {
            *parSeriesAchievements = arSeriesAchievementIds;
        }
#line 1296
    // Inference: The original definition-found flag initially is false.
        BOOL fDefFound = FALSE;
#line 1297
    // Inference: Original series traversal starts at zero and advances by one.
        for (uint32 i = 0; i < arSeriesAchievementIds.size(); ++i)
        {
            const OnlineService::AchievementDefinition * ptSeriesDef = tAchievements.GetAchievementDefinition(arSeriesAchievementIds[i]);
#line 1300
    // Inference: Absent series definitions are skipped.
            if (!ptSeriesDef)
                continue;


            OnlineService::AchievementRecord tRecord;
#line 1305
    // Inference: The original checks the interface before querying an award.
            if (ptAchievementsInterface && ptAchievementsInterface->FindAwardedAchievement(idPlayer, arSeriesAchievementIds[i], tRecord))
            {
                nPointsValue += ptSeriesDef->m_pointsValue;
#line 1308
    // Inference: The first earned series definition replaces the output definition.
                if (!fDefFound)
                {
                    tDefinition = *ptSeriesDef;
                    fDefFound = TRUE;
                }
            }
#line 1314
    // Inference: The last unearned definition is the fallback when none was earned.
            else if (!fDefFound && i == arSeriesAchievementIds.size() - 1)
            {
                tDefinition = *ptSeriesDef;
            }
        }
    }
#line 1320
    // Inference: Zero accumulated points fall back to the selected definition.
    if (!nPointsValue)
    {
        nPointsValue = tDefinition.m_pointsValue;
    }
    return nPointsValue;
}


void PopulateCommonUIElements(
    const UIID & idWindow,
    const OnlineService::AchievementDefinition & tAchievementDefinition,
    const OnlineService::GameAccountHandle & idPlayer,
    uint32 nPointsValue,
    BOOL bShowEarnedStatus)
{
    UIID idPortrait = UIIDCreateFromParent(idWindow, ".Portrait");
    UIID idBackground = UIIDCreateFromParent(idWindow, ".Background");
    UIID idInfo = UIIDCreateFromParent(idWindow, ".Info");
    UIID idStats = UIIDCreateFromParent(idWindow, ".Stats");
    UIID idDate = UIIDCreateFromParent(idStats, ".Date");
    UIID idPointWindow = UIIDCreateFromParent(idStats, ".TextImage");
    UIID idPointValue = UIIDCreateFromParent(idPointWindow, ".Value");
    UIID idShield = UIIDCreateFromParent(idPointWindow, ".Shield");
    UIID idGamerScore = UIIDCreateFromParent(idPointWindow, ".GamerScore");
    UIID idTrophies = UIIDCreateFromParent(idStats, ".PsnTrophies");
    UIID idPlatinumTrophy = UIIDCreateFromParent(idTrophies, ".PsnTrophyPlatinum");
    UIID idGoldTrophy = UIIDCreateFromParent(idTrophies, ".PsnTrophyGold");
    UIID idSilverTrophy = UIIDCreateFromParent(idTrophies, ".PsnTrophySilver");
    UIID idBronzeTrophy = UIIDCreateFromParent(idTrophies, ".PsnTrophyBronze");

    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;

    CRefString sAchievementTitle = StringListFetch(tAchievementDefinition.m_pszNameKey);
    CRefString sAchievementDesc = StringListFetch(tAchievementDefinition.m_pszDescriptionKey);
    CRefString sAchievementIcon;
    sAchievementIcon.Format("UI:%s", tAchievementDefinition.m_pszIconTexture);
    ImageTextureFrame tAchievementIcon(sAchievementIcon);

    CRefString sDescription = LocalizedSprintf(
        StringListFetch("ConsoleAchievements:AchievementDescription").str(),
        sAchievementTitle.str(),
        sAchievementDesc.str(), NULL);
    UI_GetTextFromID(idInfo).SetText(sDescription, FALSE);
    UI_GetWindowFromID(idPortrait).SetTextureFrame(tAchievementIcon);
    // Inference: original m_profileAwardId comparison uses the all-bits-set invalid value.
    if (tAchievementDefinition.m_profileAwardId != uint32(-1))
    {










        // Inference: the surviving platform branch uses profile award points and GamerScore.
        nPointsValue = tAchievementDefinition.m_profileAwardAmount;
        UI_GetWindowFromID(idTrophies).SetActive(FALSE);
        UI_GetWindowFromID(idPointWindow).SetActive(TRUE);
        UI_GetWindowFromID(idGamerScore).SetActive(TRUE);
        UI_GetWindowFromID(idShield).SetActive(FALSE);
    }
    else
    {

        UI_GetWindowFromID(idTrophies).SetActive(FALSE);
        UI_GetWindowFromID(idPointWindow).SetActive(TRUE);
        UI_GetWindowFromID(idGamerScore).SetActive(FALSE);
        UI_GetWindowFromID(idShield).SetActive(TRUE);
    }

#line 1393
    // Inference: The observed FormatNumber overload takes signed32 points; formatting defaults are explicit.
    UI_GetTextFromID(idPointValue).SetText(UnicodeText::FormatNumber(int32(nPointsValue), TRUE, FNS_NORMAL, 0), FALSE);

    OnlineService::AchievementRecord tRecord;
    SigmaMemoryClear(&tRecord, sizeof(tRecord));
#line 1397
    // Inference: The original guards the interface before querying earned state.
    BOOL bEarned = ptAchievementsInterface && ptAchievementsInterface->FindAwardedAchievement(idPlayer, tAchievementDefinition.m_id, tRecord);
#line 1398
    // Inference: An earned record shows its date; otherwise the date control is hidden.
    if (bEarned)
    {
        UIText & tUnlocked = UI_GetTextFromID(idDate);
        tUnlocked.SetText(
            UnicodeText::FormatDate(tRecord.m_timestampEarned, FDS_SHORT, 0), FALSE);
        tUnlocked.SetActive(TRUE);
    }
    else
    {
        UI_GetTextFromID(idDate).SetActive(FALSE);
    }
#line 1409
    // Inference: The original caller flag gates all four earned-state shaders.
    if (bShowEarnedStatus)
    {
#line 1411
    // Inference: Earned controls use SNO_NONE; unearned controls use the observed inactive shader.
        UI_GetWindowFromID(idWindow).SetShader(bEarned ? SNO_NONE : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
#line 1412
    // Inference: Earned portrait uses SNO_NONE; unearned portrait uses the inactive shader.
        UI_GetWindowFromID(idPortrait).SetShader(bEarned ? SNO_NONE : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
#line 1413
    // Inference: Earned stats use SNO_NONE; unearned stats use the inactive shader.
        UI_GetWindowFromID(idStats).SetShader(bEarned ? SNO_NONE : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
#line 1414
    // Inference: Earned background uses SNO_NONE; unearned background uses the inactive shader.
        UI_GetWindowFromID(idBackground).SetShader(bEarned ? SNO_NONE : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
    }
}

#line 1418
    // Inference: PopulateDetailView is recovered from its genuine ShowDetailView inline instance; a standalone body is not independently checkable in the linked original. DW_INL_inlined supplies no inline keyword.
void PopulateDetailView(
    const OnlineService::AchievementDefinition & tDefinition,
    const OnlineService::GameAccountHandle & idPlayer)
{
    OnlineService::AchievementDefinition tAchievement = tDefinition;
#line 1423
    // Inference: The original series output flag starts false.
    BOOL bIsSeries = FALSE;
    OnlineService::AchievementIdList aridSeriesAchievements;
#line 1425
    // Inference: The original passes the global current account, not the unused idPlayer parameter.
    uint32 nPoints = GetPointsAndResolveSeriesAchievement(tAchievement, sgtUIAchievementsData.idPlayer, &bIsSeries, &aridSeriesAchievements);
    Vector2D vecTitleTLBefore;
#line 1427
    Vector2D vecTitleDimBefore;
#line 1428
    UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW_TITLE).GetRelativePosition(vecTitleTLBefore, vecTitleDimBefore);
#line 1428
    // Inference: The original common-UI call uses tDefinition, while status and rewards use the resolved copy.
    PopulateCommonUIElements(UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW_TITLE, tDefinition, sgtUIAchievementsData.idPlayer, nPoints, FALSE);







    // Inference: original title-height delta adjusts the scroll rectangle after updating title content.
    Vector2D vecWinTL;
#line 1438
    Vector2D vecWinDim;
#line 1438
    Vector2D vecTitleTL;
#line 1438
    Vector2D vecTitleDim;
#line 1438
    Vector2D vecScrollTL;
#line 1438
    Vector2D vecScrollDim;
#line 1439
    UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW).GetRelativePosition(vecWinTL, vecWinDim);
    UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW_TITLE).GetRelativePosition(vecTitleTL, vecTitleDim);
    UI_GetWindowFromID(UISCROLL_CONSOLE_ACHIEVEMENTS_DETAILVIEW).GetRelativePosition(vecScrollTL, vecScrollDim);
    vecScrollTL.y = vecTitleTL.y + vecTitleDim.y;
    vecScrollDim.y += vecTitleDimBefore.y - vecTitleDim.y;
    UI_GetWindowFromID(UISCROLL_CONSOLE_ACHIEVEMENTS_DETAILVIEW).SetRelativePosition(vecScrollTL, vecScrollDim);


    // Inference: original branches activate and populate only the series section when bIsSeries.
    if (bIsSeries)
    {
        UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW_SERIES).SetActive(TRUE);
        PopulateSeries(aridSeriesAchievements);
    }
    else
    {
        UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW_SERIES).SetActive(FALSE);
    }
    // Inference: both output quantities are zero initialized in the original before the status lookup.
    OnlineService::CriteriaQuantity nEarned = 0;
#line 1458
    OnlineService::CriteriaQuantity nTotal = 0;
#line 1459
#line 1458
    // Inference: Display flags are derived from the resolved achievement.
    uint32 dwFlags = GetCriteriaFlags(tAchievement);
    OnlineService::AchievementStatus eStatus = OnlineService::GetAchievementStatus(sgtUIAchievementsData.idPlayer, tAchievement.m_id, &nEarned, &nTotal);

    // Inference: original flag tests gate the progress and checklist sections independently.
    UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW_PROGRESS).SetActive((dwFlags & (1 << PROGRESS_BAR)) != 0);
    if (dwFlags & (1 << PROGRESS_BAR))
    {
        PopulateProgressBar(nEarned, nTotal);
    }
#line 1467
    // Inference: The original explicitly shifts the checklist flag before masking one bit; this spelling preserves its reused shift.
    UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW_CHECKLIST).SetActive(((dwFlags >> CHECKLIST) & 1));
#line 1468
    // Inference: The same shifted checklist bit gates population.
    if ((dwFlags >> CHECKLIST) & 1)
    {
        PopulateCriteriaDisplay(tAchievement);
    }


    OnlineService::RewardIdList arRewards;

    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
#line 1477
    // Inference: The series flag selects aggregated rewards versus this achievement alone.
    if (bIsSeries)
    {

        // Inference: series rewards are appended from the last series achievement towards the first.
        for (int32 i = int32(aridSeriesAchievements.size()) - 1; i >= 0; --i)
        {
            OnlineService::RewardIdList arAchRewards;
#line 1484
    // Inference: Failed or empty reward lookups skip appending that series member.
            if (!tAchievements.GetAchievementRewards(aridSeriesAchievements[i], arAchRewards) ||
                !arAchRewards.size())
            {
                continue;
            }
            blzext::vector_append(arRewards, arAchRewards);
        }
    }
    else
    {
        tAchievements.GetAchievementRewards(tAchievement.m_id, arRewards);
    }
    // Inference: no reward controls are populated when the recovered reward list is empty.
    UI_GetWindowFromID(UIWND_CONSOLE_ACHIEVEMENTS_DETAILVIEW_REWARDS).SetActive(arRewards.size() != 0);
    if (arRewards.size())
    {
        PopulateRewards(arRewards);
    }

}



#line 1507
    // Inference: GetCriteriaFlags is recovered from its genuine ShowDetailView inline instance; a standalone body is not independently checkable in the linked original. DW_INL_inlined supplies no inline keyword.
uint32 GetCriteriaFlags(
    const OnlineService::AchievementDefinition & tDefinition)
{

    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;

    // Inference: initially no display flags; original achievement flag 16 enables progress.
    uint32 dwFlags = 0;

#line 1516
    // Inference: Achievement HAS_PROGRESS_BAR sets the progress display bit.
    if (tDefinition.m_flags & (1 << OnlineService::ACHIEVEMENT_FLAG_HAS_PROGRESS_BAR))
    {
        dwFlags |= (1 << PROGRESS_BAR);
    }

    OnlineService::CriteriaIdList arCriteriaIds;
#line 1522
    // Inference: The visible-criteria count starts at zero.
    int32 nCount = 0;
    if (tAchievements.GetAchievementCriteria(tDefinition.m_id, arCriteriaIds))
    {
#line 1525
    // Inference: Traverse criteria in original ascending index order.
        for (uint32 i = 0; i < arCriteriaIds.size(); ++i)
        {
            const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition(arCriteriaIds[i]);
#line 1528
    // Inference: A missing definition does not count.
            if (!ptCritDef)
            {
                continue;
            }

            // Inference: only visible criteria count towards enabling the checklist.
            if (ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_HIDDEN))
            {
                continue;
            }

#line 1539
    // Inference: The original increments the visible count and stops after the second visible criterion.
            if (++nCount > 1)
            {
                dwFlags |= (1 << CHECKLIST);
                break;
            }
        }
    }
    return dwFlags;
}




#line 1552
    // Inference: PopulateProgressBar is recovered from its genuine ShowDetailView inline instance; a standalone body is not independently checkable in the linked original. DW_INL_inlined supplies no inline keyword.
void PopulateProgressBar(
    OnlineService::CriteriaQuantity nEarned,
    OnlineService::CriteriaQuantity nTotal)
{
#line 1556
    // Inference: LocalizedSprintf receives a pointer-width NULL terminator, matching the original variadic argument.
    CRefString sProgress = LocalizedSprintf(
        StringListFetch("Bnet_Achievements:AchievementProgress").str(),
#line 1558
    // Inference: Original formatting converts the earned quantity to signed32 for this overload.
        UnicodeText::FormatNumber(int32(nEarned), TRUE, FNS_NORMAL, 0).str(),
#line 1559
    // Inference: Original formatting converts the total quantity to signed32 and passes NULL terminator.
        UnicodeText::FormatNumber(int32(nTotal), TRUE, FNS_NORMAL, 0).str(), NULL);
    UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_DETAILVIEW_PROGRESS_TEXT).SetText(sProgress, FALSE);
#line 1561
    // Inference: Original progress extents use zero, total, earned, and -1.0 transition default.
    UI_GetProgressBarFromID(UIPROGRESS_CONSOLE_ACHIEVEMENTS_DETAILVIEW_PROGRESS_BAR).SetExtentsAndPosition(0.0f, float(nTotal), float(nEarned), -1.0f);
}



void PopulateSeriesElement(
    const UIID & idElement,
    OnlineService::AchievementId idAchievement)
{
    UIID idIcon = UIIDCreateFromParent(idElement, ".SeriesPortrait");
    UIID idText = UIIDCreateFromParent(idElement, ".SeriesText");

    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;
    const OnlineService::AchievementDefinition * ptDef = tAchievements.GetAchievementDefinition(idAchievement);
#line 1576
    // Inference: Missing series achievement definitions leave the control unchanged.
    if (ptDef)
    {
        OnlineService::AchievementRecord tRecord;
        BOOL bEarned = ptAchievementsInterface->FindAwardedAchievement(sgtUIAchievementsData.idPlayer, idAchievement, tRecord);
        CRefString sAchievementIcon;
        sAchievementIcon.Format("UI:%s", ptDef->m_pszIconTexture);
        ImageTextureFrame tAchievementIcon(sAchievementIcon);
        UI_GetWindowFromID(idIcon).SetTextureFrame(tAchievementIcon);
#line 1584
    // Inference: Earned series icons use SNO_NONE; others use inactive shader.
        UI_GetWindowFromID(idIcon).SetShader(bEarned ? SNO_NONE : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
#line 1585
    // Inference: Original points format selects signed32 number overload with explicit defaults.
        UI_GetTextFromID(idText).SetText(UnicodeText::FormatNumber(int32(ptDef->m_pointsValue), TRUE, FNS_NORMAL, 0), FALSE);
    }
}



#line 1591
    // Inference: PopulateSeries is recovered from its genuine ShowDetailView inline instance; a standalone body is not independently checkable in the linked original. DW_INL_inlined supplies no inline keyword.
void PopulateSeries(
    const OnlineService::AchievementIdList & arSeriesAchievements)
{
    UIWindow & tWrapPanel = UI_GetWindowFromID(UIWRAP_CONSOLE_ACHIEVEMENTS_DETAILVIEW_SERIES_PANEL);
    const UIControlItemArray & arChildren = tWrapPanel.GetChildList();
#line 1596
    // Inference: The reused series-child index begins at zero.
    uint32 nSeriesControlIndex = 0;
    // Inference: reuse existing children, create missing controls, then delete surplus children.
    for (; nSeriesControlIndex < arChildren.size() && nSeriesControlIndex < arSeriesAchievements.size(); ++nSeriesControlIndex)
    {
        PopulateSeriesElement(arChildren[nSeriesControlIndex].id, arSeriesAchievements[arSeriesAchievements.size() - 1 - nSeriesControlIndex]);
    }

    UIControlSourceKey keySeriesItem = UIControlSourceKey::Parse("ConsoleAchievements:ConsoleAchievements_main.LayoutRoot.AchievementDetails.AchievementScrollBox.AchievementStack.SeriesPanelSpacer.SeriesPanel.SeriesTemplate", -1, NULL, NULL);

    for (; nSeriesControlIndex < arSeriesAchievements.size(); ++nSeriesControlIndex)
    {
        UIControl * ptControl = UI_CreateControlFromTemplate(keySeriesItem, DynamicControlCreationParams(CRefString().Format("series%d", nSeriesControlIndex), UIWRAP_CONSOLE_ACHIEVEMENTS_DETAILVIEW_SERIES_PANEL), NULL);

        PopulateSeriesElement(ptControl->GetID(), arSeriesAchievements[arSeriesAchievements.size() - 1 - nSeriesControlIndex]);
    }

    while (nSeriesControlIndex < arChildren.size())
    {
        UI_DeleteControl(arChildren[nSeriesControlIndex].id, TRUE);
    }
}


// Inference: reward-type branches and first earned parent follow the original calls and switch targets.
void PopulateRewardElement(
    const UIID & idElement,
    OnlineService::RewardId idReward)
{
    UIID idIcon = UIIDCreateFromParent(idElement, ".RewardPortrait");
    UIID idTransmogIcon = UIIDCreateFromParent(idElement, ".RewardItem");
    UIID idText = UIIDCreateFromParent(idElement, ".RewardText");

    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;
    OnlineService::RewardDefinition tDef;
#line 1631
    // Inference: Only a found reward definition is displayed.
    if (tAchievements.GetRewardDefinition(idReward, tDef))
    {
        OnlineService::AchievementRecord tRecord;
#line 1634
    // Inference: A reward begins unearned until any parent award is found.
        BOOL bEarned = FALSE;
        OnlineService::AchievementIdList listParentAchievements;
        tAchievements.GetRewardParentAchievements(idReward, listParentAchievements);
#line 1637
    // Inference: Traverse parent achievements in ascending index order.
        for (uint32 i = 0; i < listParentAchievements.size(); ++i)
        {
#line 1639
    // Inference: The first awarded parent marks the reward earned and ends traversal.
            if (ptAchievementsInterface->FindAwardedAchievement(sgtUIAchievementsData.idPlayer, listParentAchievements[i], tRecord))
            {
                bEarned = TRUE;
                break;
            }
        }
        CRefString sName = OnlineService::GetRewardTypeString(tDef);
        ImageTextureFrame tIcon;
#line 1647
    // Inference: Cosmetic rewards use their dedicated icon-type branch.
        if (tDef.m_type == OnlineService::REWARD_TYPE_COSMETIC)
        {
            GBID gbidCosmetic = tDef.m_id;
            CosmeticItemType eCosmeticType = CosmeticItems_GetCosmeticItemTypeFromGBID(gbidCosmetic);

            switch (eCosmeticType)
            {
            case COSMETIC_ITEM_PET:
                ImageCollectionGetImageInfo("UI:icon_cosmetic_pet", tIcon);
                break;

            case COSMETIC_ITEM_PENNANT:
                ImageCollectionGetImageInfo("UI:icon_cosmetic_pennant", tIcon);
                break;

            case COSMETIC_ITEM_WINGS:
                ImageCollectionGetImageInfo("UI:icon_cosmetic_wings", tIcon);
                break;

            case COSMETIC_ITEM_FRAME:
                ImageCollectionGetImageInfo("UI:icon_cosmetic_portrait", tIcon);
                break;

            default:
#line 1671
    // Inference: The original fallback draw uses normal item draw, Barbarian class, and male=true.
                ACDInventoryGetItemDrawInfoWithGBID(gbidCosmetic, tIcon, 0, PLAYER_CLASS_BARBARIAN, TRUE);
                break;
            }

            sName = OnlineService::GetRewardTypeString(tDef);
        }
#line 1677
    // Inference: Item and transmog rewards share the item-preview branch.
        else if (tDef.m_type == OnlineService::REWARD_TYPE_ITEM_TRANSMOG || tDef.m_type == OnlineService::REWARD_TYPE_ITEM)
        {

            UI_GetWindowFromID(idTransmogIcon).SetActive(TRUE);

            int32 nControllerIndex = ::Console::Screens::GetCurrentControllerIndex();
            Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
#line 1684
    // Inference: The original default preview class is Barbarian even if a live player exists.
            PlayerClass eClass = PLAYER_CLASS_BARBARIAN;
#line 1685
    // Inference: Startup or an absent player selects the saved-hero class.
            if (GameIsStartUp() || !ptPlayer)
            {
                D3::Hero::SavedDefinition & tHeroDef = ::Console::GamerProfile::GetHero(nControllerIndex);
                eClass = PlayerClassFromGBID(tHeroDef.digest().gbid_class());
            }

            GBID gbidItem = tDef.m_id;
#line 1692
    // Inference: The original preview uses normal item draw and male=true.
            ACDInventoryGetItemDrawInfoWithGBID(gbidItem, tIcon, 0, eClass, TRUE);
        }
        else
        {
            ImageCollectionGetImageInfo(
                IMAGE_COLLECTION_UI,
                tDef.m_hIcon,
                tIcon);
        }

        // Inference: the original uses item/transmog controls only for these two reward types.
        if (tDef.m_type == OnlineService::REWARD_TYPE_ITEM || tDef.m_type == OnlineService::REWARD_TYPE_ITEM_TRANSMOG)
        {

            UI_GetWindowFromID(idTransmogIcon).SetActive(TRUE);
            UI_GetWindowFromID(idTransmogIcon).SetTextureFrame(tIcon);
#line 1708
    // Inference: Earned item previews use SNO_NONE; otherwise inactive shader.
            UI_GetWindowFromID(idTransmogIcon).SetShader(bEarned ? SNO_NONE : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));

            ImageTextureFrame tIconBackground;
            ImageCollectionGetImageInfo("UI:BattleNetBannerCustomization_ListItemUp", tIconBackground);
            UI_GetWindowFromID(idIcon).SetTextureFrame(tIconBackground);
        }
        else
        {

            UI_GetWindowFromID(idTransmogIcon).SetActive(FALSE);
            UI_GetWindowFromID(idIcon).SetTextureFrame(tIcon);
#line 1719
    // Inference: Earned non-item icons use SNO_NONE; otherwise inactive shader.
            UI_GetWindowFromID(idIcon).SetShader(bEarned ? SNO_NONE : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
        }


        UI_GetTextFromID(idText).SetText(sName, FALSE);
    }
}



#line 1729
    // Inference: PopulateRewards is recovered from its genuine ShowDetailView inline instance; a standalone body is not independently checkable in the linked original. DW_INL_inlined supplies no inline keyword.
void PopulateRewards(
    const OnlineService::RewardIdList & arRewards)
{
    UIWindow & tWrapPanel = UI_GetWindowFromID(UIWRAP_CONSOLE_ACHIEVEMENTS_DETAILVIEW_REWARDS_PANEL);
    const UIControlItemArray & arChildren = tWrapPanel.GetChildList();
#line 1734
    // Inference: The reused reward-child index begins at zero.
    uint32 nRewardsControlIndex = 0;
    // Inference: reuse existing children, create missing controls, then delete surplus children.
    for (; nRewardsControlIndex < arChildren.size() && nRewardsControlIndex < arRewards.size(); ++nRewardsControlIndex)
    {
        PopulateRewardElement(arChildren[nRewardsControlIndex].id, arRewards[nRewardsControlIndex]);
    }

    UIControlSourceKey keySeriesItem = UIControlSourceKey::Parse("ConsoleAchievements:ConsoleAchievements_main.LayoutRoot.AchievementDetails.AchievementScrollBox.AchievementStack.RewardsSpacer.RewardsPanel.RewardTemplate", -1, NULL, NULL);

    for (; nRewardsControlIndex < arRewards.size(); ++nRewardsControlIndex)
    {
        UIControl * ptControl = UI_CreateControlFromTemplate(keySeriesItem, DynamicControlCreationParams(CRefString().Format("reward%d", nRewardsControlIndex), UIWRAP_CONSOLE_ACHIEVEMENTS_DETAILVIEW_REWARDS_PANEL), NULL);

        PopulateRewardElement(ptControl->GetID(), arRewards[nRewardsControlIndex]);
    }

    while (nRewardsControlIndex < arChildren.size())
    {
        UI_DeleteControl(arChildren[nRewardsControlIndex].id, TRUE);
    }
}


// Inference: require both definitions before deriving earned state and populating the criterion controls.
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
#line 1771
    // Inference: Criteria flag IS_ACHIEVEMENT_ID selects the meta-achievement visual state.
        BOOL fMetaAchievement = (ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_IS_ACHIEVEMENT_ID)) != 0;

#line 1773
    // Inference: Criteria start unearned until the original lookup proves otherwise.
        BOOL fEarned = FALSE;
        CRefString sMessage;
#line 1775
    // Inference: The original skips earned lookup when no interface exists.
        if (ptAchievementsInterface)
        {

            // Inference: absent criteria fall back to the parent award only when no minimum criteria is specified.
            if (!ptAchievementsInterface->FindCriteriaRecord(sgtUIAchievementsData.idPlayer, idCriteria, tCritRecord))
            {
                if (!ptAchievementDef->m_nMinRequiredCriteria)
                {
                    OnlineService::AchievementRecord tRecord;
                    fEarned = ptAchievementsInterface->FindAwardedAchievement(sgtUIAchievementsData.idPlayer, ptCritDef->m_parentAchievementId, tRecord);
                }
            }
            else
            {




#line 1793
    // Inference: A found criteria record is earned at or above its required quantity.
                fEarned = tCritRecord.m_quantity >= ptCritDef->m_necessaryQuantity;
            }
        }
#line 1796
    // Inference: RETRIEVE_NAME_FROM_STRINGLIST selects event-derived labeling.
        if (ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_RETRIEVE_NAME_FROM_STRINGLIST))
        {
            Achievement::EventID eEventId = ptCritDef->m_advanceEvent.m_id;
            CRefString sStringLabelKey(ptCritDef->m_pszStringlabelKey);
#line 1800
    // Inference: Original event operand is m_advanceEvent.m_comparand at offset0x50, not required quantity.
            sMessage = Achievement::DisplayStringFromEvent(eEventId, ptCritDef->m_advanceEvent.m_comparand, sStringLabelKey);
        }
        else
        {
            sMessage = StringListFetch(ptCritDef->m_pszNameKey);
        }

    UIID idTextEarned;
#line 1808
    UIID idTextUnearned;
#line 1809
        idTextEarned = UIIDCreateFromParent(idElement, ".CriteriaEarned");
        idTextUnearned = UIIDCreateFromParent(idElement, ".CriteriaUnearned");
        ControlVisibilityManager tText;
        tText.AddControl(idTextEarned);
        tText.AddControl(idTextUnearned);
#line 1813
    // Inference: Earned text is control0, unearned text control1; force refresh is false.
        tText.SetActive(fEarned ? 0 : 1, false);
#line 1814
    // Inference: The text target uses the same earned-state selection.
        UIID idTargetTextfield = fEarned ? idTextEarned : idTextUnearned;
        UI_GetTextFromID(idTargetTextfield).SetText(sMessage, FALSE);

    UIID idEarned;
#line 1818
    UIID idUnearned;
#line 1818
    UIID idChecked;
#line 1818
    UIID idUnchecked;
#line 1819
        idEarned = UIIDCreateFromParent(idElement, ".Earned");
        idUnearned = UIIDCreateFromParent(idElement, ".Unearned");
        idChecked = UIIDCreateFromParent(idElement, ".Checked");
        idUnchecked = UIIDCreateFromParent(idElement, ".UnChecked");
        // Inference: meta achievements select the earned pair, other criteria select the checked pair.
        int32 nTargetState = (fMetaAchievement ? 0 : 2) + (fEarned ? 0 : 1);
        ControlVisibilityManager tStates;
        tStates.AddControl(idEarned);
        tStates.AddControl(idUnearned);
        tStates.AddControl(idChecked);
        tStates.AddControl(idUnchecked);
        tStates.SetActive(nTargetState, false);
    }
}



#line 1835
    // Inference: PopulateCriteriaDisplay is recovered from its genuine ShowDetailView inline instance; a standalone body is not independently checkable in the linked original. DW_INL_inlined supplies no inline keyword.
void PopulateCriteriaDisplay(
    const OnlineService::AchievementDefinition & tDefinition)
{

    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::CriteriaIdList arCriteria;
    tAchievements.GetAchievementCriteria(tDefinition.m_id, arCriteria);
    for (uint32 i = 0; i < arCriteria.size(); ++i)
    {
        // Inference: missing and hidden criteria are removed before sorting; recheck the swapped entry.
        const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition(arCriteria[i]);
        if (ptCritDef && !(ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_HIDDEN)))
            continue;

        blzext::vector_erase_reorder(arCriteria, i--);
    }

    blz::sort(arCriteria.begin(), arCriteria.end(), OnlineService::CriteriaSortPredicate);
    UIWindow & tWrapPanel = UI_GetWindowFromID(UIWRAP_CONSOLE_ACHIEVEMENTS_DETAILVIEW_CHECKLIST_PANEL);
    const UIControlItemArray & arChildren = tWrapPanel.GetChildList();
#line 1855
    // Inference: The reused criterion-child index begins at zero.
    uint32 nCriteriaControlIndex = 0;
    // Inference: reuse children first, create remaining entries, then remove excess children.
    for (; nCriteriaControlIndex < arChildren.size() && nCriteriaControlIndex < arCriteria.size(); ++nCriteriaControlIndex)
    {
        PopulateCriteriaElement(arChildren[nCriteriaControlIndex].id, arCriteria[nCriteriaControlIndex]);
    }

    UIControlSourceKey keySeriesItem = UIControlSourceKey::Parse("ConsoleAchievements:ConsoleAchievements_main.LayoutRoot.AchievementDetails.AchievementScrollBox.AchievementStack.ChecklistSpacer.ChecklistPanel.ChecklistItem", -1, NULL, NULL);

    for (; nCriteriaControlIndex < arCriteria.size(); ++nCriteriaControlIndex)
    {
        UIControl * ptControl = UI_CreateControlFromTemplate(keySeriesItem, DynamicControlCreationParams(CRefString().Format("criteria%d", nCriteriaControlIndex), UIWRAP_CONSOLE_ACHIEVEMENTS_DETAILVIEW_CHECKLIST_PANEL), NULL);

        PopulateCriteriaElement(ptControl->GetID(), arCriteria[nCriteriaControlIndex]);
    }

    while (nCriteriaControlIndex < arChildren.size())
    {
        UI_DeleteControl(arChildren[nCriteriaControlIndex].id, TRUE);
    }
}



}
}
}
#line 1883


