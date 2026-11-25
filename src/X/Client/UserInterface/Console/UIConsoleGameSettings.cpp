// Original file: X/Client/UserInterface/Console/UIConsoleGameSettings.cpp
// Partial adoption: quest selection and the reset confirmation's original callers.
#include "UIConsoleGameSettings.h"
#include "../../OnlineService/OnlineServiceClientBase.h"
#include "../../Console/GamerProfile.h"
#include "OnlineActions.h"
#include "ConsoleScreenManager.h"
#include "UIConsoleMainMenu.h"
#include "../UI.h"
#include "../UIIDs.h"
#include "../../../Common/Act.h"
#include "../../../Common/Definitions/ActDefinition.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../../Common/Protobuf/Hero.pb.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../../Sound/Sound.h"
#include "../../Gfx/GfxVB.h"
// GfxVB.h (its RGBAColor statics) must come before Online.h (the SDK statics)
// to keep the static-initializer order.
#include "../../Console/Online.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "ConsoleModalNotificationManager.h"
#include "../UIStoryboard.h"
#include "../../../Common/Quests/QuestsSwig.h"
#include "../../../Common/Quests/Quests.h"
#include "../Controls/UIWindowControl.h"
#include "../UIUtils.h"
#include "../../../Common/Leaderboard.h"
#include "../../../Common/Definitions/QuestDefinition.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIConsoleScrollBoxControl.h"
#include "../Controls/UIRActorControl.h"
#include "../../CPlayer.h"
#include "../../../Common/Player.h"
#include "../../../Common/Game.h"
#include "../../../Common/Definitions/HeroDefinition.h"
#include "ControllerActions.h"
#include "../Controls/UIRadioButtonGroupControl.h"
#include "../../../Common/Protobuf/Account.pb.h"
#include "../../../Common/Protobuf/ChallengeRifts.pb.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Common/Protobuf/Leaderboard.pb.h"
#include "../../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include <stddef.h>
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
extern UIID UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_MODE_GROUP;
extern UIID UISTORYBOARD_CONSOLE_GAME_SETTINGS_ACT_SELECT_HIGHLIGHT;
extern UIID UISTORYBOARD_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_HIGHLIGHT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_CURSOR;
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
const UIID & UI_GetControllerFocus();
extern LPCSTR gszQuestStepBnetTitleLabel;
extern LPCSTR gszUnassignedQuestStepBnetTitleLabel;
namespace UIUtils { namespace Console { BOOL GameTagIsValid(const OnlineService::GameTagId & tTag, BOOL bAdventureMode); } }
namespace UIMainMenu { namespace Console { int32 GetMinLocalHeroLevel(); } }
bool HeroCompletedAllQuests(const D3::Hero::QuestHistoryList & questHistoryList, BOOL bIncludeActV);
namespace UIUtils { namespace Console { void OpenInspectLeaderboardForChallengeRift(); } }
namespace UILeaderboards { namespace Console { void OpenForGameSettings(const int32 nControllerIndex, BOOL bOpenedFromGameSettings); } }
extern UIID UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_QUICK_MATCH;
namespace UIGameSettings { namespace Console {
enum { FILTER_BY_QUEST_COMPLETION = 0, DIFFICULTY_SELECT = 1, MATCHMAKING_MODE = 2, SHOW_TAGS = 3 };
enum PlayMode { CAMPAIGN_MODE = 0, ADVENTURE_MODE = 1, CHALLENGE_MODE = 2 };
enum GameSettingsMode {
    SELECT_MODE_PLAYMODE = 0, SELECT_MODE_ACT = 1, SELECT_MODE_QUEST = 2,
    SELECT_MODE_DIFFICULTY = 3, SELECT_MODE_CHALLENGE = 4, NUM_SELECT_MODES = 5
};

class QuestDataSource : public ConsoleListBoxDataSource {
public:
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual BOOL IsItemEnabled(const UIConsoleListBox & tListBox, const int32 nIndex);
    int32 GetQuestIndex(SNO snoQuest) {
        for (int32 i = 0; i < m_arQuestSnos.size(); ++i)
            if (m_arQuestSnos[i] == snoQuest)
                return i;
        return -1;
    }
    blz::vector<int, blz::allocator<int> > m_arQuestSnos;
};
static_assert(sizeof(QuestDataSource) == 0x20, "QuestDataSource");

// These remaining data-source declarations preserve their DWARF class owners.
class ActDataSource : public ConsoleListBoxDataSource {
public:
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32);
};
class QuestStepDataSource : public ConsoleListBoxDataSource {
public:
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32);
    int32 GetQuestStepIndex(int32 nQuestStep) {
        for (int32 i = 0; i < m_arQuestStepUIDs.size(); ++i)
            if (m_arQuestStepUIDs[i] == nQuestStep)
                return i;
        return -1;
    }
    blz::vector<int, blz::allocator<int> > m_arQuestStepUIDs;
};
class TagsDataSource : public ConsoleListBoxDataSource {
public:
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32);
    void Initialize(BOOL bAdventureMode);
    int32 GetTagIndex(int32 nTagId) {
        for (int32 i = 0; i < (int32)m_arGameTags.size(); ++i)
            if (m_arGameTags[i].m_tagId == nTagId)
                return i;
        return -1;
    }
    OnlineService::XGameTagArray m_arGameTags;
};

// DWARF GameSettingsGlobals; constructed by the file's static initializer.

struct GameSettingsGlobals {
    uint32 dwFlags;
    GameSettingsMode eCurrentMode;
    D3::Hero::SavedDefinition tHeroDefinition;
    D3::Account::SavedDefinition tAccountDefinition;
    OnlineService::DetailsDigest * ptDigest;
    uint32 dwActUnlockedProgress;
    uint32 dwActCompletionProgress;
    ActDataSource tActDataSource;
    QuestDataSource tQuestDataSource;
    QuestStepDataSource tQuestStepDataSource;
    TagsDataSource tMatchmakingTagsDataSource;
    ScreenClosedCallback cbScreenClosed;
    PlayMode eSelectedMode;
    int32 nSelectedDifficulty;
    Act eSelectedCampaignAct;
    Act eSelectedAdventureAct;
    SNO snoSelectedQuest;
    int32 nSelectedQuestStep;
    int32 nSelectedQuickMatchOption;
    int32 nSelectedQuickMatchTag;
    int32 nOriginalInGameDifficulty;
    Blizzard::Time::Millisecond nQuestResetOpenTime;
    BOOL bGameSettingsViewed;
    BOOL bOverwriteWarningAccepted;
    D3::ChallengeRifts::ChallengeData tChallengeData;
    D3::Leaderboard::WeeklyChallengeData tWeeklyChallengeData;
    blz::unordered_map<LeaderboardIds, D3::Leaderboard::Score> arPersonalBests;
    LeaderboardIds nCurrentChallengeLeaderboardDisplayed;
    BOOL bAskingForChallengeRiftData;
    BOOL bChallengeRiftDataAvailable;
    Blizzard::Time::Timestamp uCacheWeeklyChallengeUntilTimestamp;

    // Original lines 536 and 579-605.
    ~GameSettingsGlobals() {}
    GameSettingsGlobals()
        : dwFlags(0)
        , eCurrentMode(SELECT_MODE_PLAYMODE)
        , ptDigest(NULL)
        , dwActUnlockedProgress(0)
        , dwActCompletionProgress(0)
        , eSelectedMode(CAMPAIGN_MODE)
        , nSelectedDifficulty(0)
        , eSelectedCampaignAct(ACT_1)
        , eSelectedAdventureAct(ACT_1)
        , snoSelectedQuest(SNO_NONE)
        , nSelectedQuestStep(-1)
        , nSelectedQuickMatchOption(1)
        , nSelectedQuickMatchTag(OnlineService::GAME_TAG_ID_DEFAULT)  // GOT 0x71010e5e28 -> GAME_TAG_ID_DEFAULT (0x71003b132c)
        , nOriginalInGameDifficulty(0)
        , nQuestResetOpenTime(0)
        , bGameSettingsViewed(FALSE)
        , bOverwriteWarningAccepted(FALSE)
        , nCurrentChallengeLeaderboardDisplayed((LeaderboardIds)0)
        , bAskingForChallengeRiftData(FALSE)
        , bChallengeRiftDataAvailable(FALSE)
        , uCacheWeeklyChallengeUntilTimestamp(0)
    {
        dwFlags |= (1 << FILTER_BY_QUEST_COMPLETION);
    }
};
static_assert(sizeof(GameSettingsGlobals) == 0x2f0, "GameSettingsGlobals");
static_assert(offsetof(GameSettingsGlobals, ptDigest) == 0x188, "ptDigest");
static_assert(offsetof(GameSettingsGlobals, eSelectedCampaignAct) == 0x210, "eSelectedCampaignAct");
static_assert(offsetof(GameSettingsGlobals, nQuestResetOpenTime) == 0x22c, "nQuestResetOpenTime");
static_assert(offsetof(GameSettingsGlobals, tChallengeData) == 0x238, "tChallengeData");
static_assert(offsetof(GameSettingsGlobals, arPersonalBests) == 0x2b8, "arPersonalBests");
static_assert(offsetof(GameSettingsGlobals, bChallengeRiftDataAvailable) == 0x2e0, "bChallengeRiftDataAvailable");
static_assert(offsetof(GameSettingsGlobals, nCurrentChallengeLeaderboardDisplayed) == 0x2d8, "nCurrentChallengeLeaderboardDisplayed");
static_assert(offsetof(GameSettingsGlobals, uCacheWeeklyChallengeUntilTimestamp) == 0x2e8, "uCacheWeeklyChallengeUntilTimestamp");
GameSettingsGlobals sgtGameSettingsGlobals;  // original line 608

namespace {
enum { RESET_QUEST_COOLDOWN_MILLISECS = 3000 };
const Act arActList[5] = { ACT_1, ACT_2, ACT_3, ACT_4, ACT_5 };
void DetermineActCompletionProgress();
void SetActUnlocked(Act eAct, BOOL bComplete);
void SetActComplete(Act eAct, BOOL bComplete);
BOOL GetActComplete(Act eAct);
BOOL IsQuestSelectable(SNO snoQuest);
BOOL HasResetCooldownPassed();
void FallbackSelectedChoices();
void RefreshSelectionDisplay();
void ConfirmGameParams(BOOL, BOOL);
void ChangeSelectionMode(GameSettingsMode);
void ResetSelections();
void ResetQuestSelections();
void RefreshDisplay();
void UpdateStepListForSelectedQuest(SNO snoQuest);
void RefreshQuestDisplay();
void UpdateQuestStepDescriptions();
void RefreshResetWarning(const UIID & id);
void RefreshActDisplay();
void OnLeftShoulder(BOOL bPlaySound);
void RefreshPlayModeDisplay();
void RefreshDifficultyDisplay();
BOOL IsDifficultyLocked(int32 nIndex, int32 nPrimaryProfile);
void PopulateLeaderboard(LeaderboardIds id);
void UpdateActDescriptions();
}
void OnAccept(const UIID & id, const int32 nControllerIndex);

// Data-source virtuals (emitted at the end of the original file).
int32 ActDataSource::GetNumItems() const
{
    return 5;
}

UIControlSourceKey ActDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    return UIControlSourceKey::Parse("ConsoleGameSettings:ConsoleGameSettings_main.ListItemAct", SNO_NONE, NULL, NULL);
}

void ActDataSource::InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32)
{
}

int32 QuestStepDataSource::GetNumItems() const
{
    return m_arQuestStepUIDs.size();
}

UIControlSourceKey QuestStepDataSource::GetSourceKeyForItem(const int32) const
{
    return UIControlSourceKey::Parse("ConsoleGameSettings:ConsoleGameSettings_main.select", SNO_NONE, NULL, NULL);
}

void QuestStepDataSource::InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32)
{
}

UIControlSourceKey TagsDataSource::GetSourceKeyForItem(const int32) const
{
    return UIControlSourceKey::Parse("ConsoleGameSettings:ConsoleGameSettings_main.select", SNO_NONE, NULL, NULL);
}

void TagsDataSource::InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32)
{
}

int32 QuestDataSource::GetNumItems() const
{
    return m_arQuestSnos.size();
}

UIControlSourceKey QuestDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    return UIControlSourceKey::Parse("ConsoleGameSettings:ConsoleGameSettings_main.QuestSelectItem", SNO_NONE, NULL, NULL);
}

void QuestDataSource::InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32)
{
}

// Original line 342.
void QuestDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    BOOL bSelected = tListBox.GetSelectedIndex() == nIndex;
    BOOL bEnabled = IsQuestSelectable(m_arQuestSnos[nIndex]);
    UIID idSelected = UIIDCreateFromParent(idItemUI, ".DotCurrentSelected");
    UIID idUnlocked = UIIDCreateFromParent(idItemUI, ".DotUnlocked");
    UIID idDisabled = UIIDCreateFromParent(idItemUI, ".DotDisabled");
    UI_GetWindowFromID(idSelected).SetActive(bSelected);
    UI_GetWindowFromID(idUnlocked).SetActive(bEnabled);
    UI_GetWindowFromID(idDisabled).SetActive(!bEnabled);
}

// Original line 392. This real virtual method retains IsQuestSelectable.
BOOL QuestDataSource::IsItemEnabled(const UIConsoleListBox & tListBox, const int32 nIndex)
{
    return IsQuestSelectable(m_arQuestSnos[nIndex]);
}

// Original line 737.
void CommonOpen(int32 nControllerIndex, uint32 dwFlags, ScreenClosedCallback cb)
{
    sgtGameSettingsGlobals.cbScreenClosed = cb;
    sgtGameSettingsGlobals.dwFlags = dwFlags;
    sgtGameSettingsGlobals.bOverwriteWarningAccepted = FALSE;
// Inference: original cache increment is 60 billion nanoseconds, using DWARF CACHE_WEEKLY_CHALLENGE_SECONDS=60.
#line 3056
    sgtGameSettingsGlobals.uCacheWeeklyChallengeUntilTimestamp = 0;
    ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_GAME_SETTINGS, TRUE, GetUIIDNone());
    sgtGameSettingsGlobals.tHeroDefinition.CopyFrom(::Console::GamerProfile::GetHero(nControllerIndex));
    sgtGameSettingsGlobals.tAccountDefinition.CopyFrom(::Console::GamerProfile::GetAccount(nControllerIndex));
    ::delete sgtGameSettingsGlobals.ptDigest;
    sgtGameSettingsGlobals.ptDigest = NULL;
    sgtGameSettingsGlobals.ptDigest = new OnlineService::DetailsDigest;
    sgtGameSettingsGlobals.ptDigest->PopulateCompletedQuestList(sgtGameSettingsGlobals.tHeroDefinition.digest().quest_history());
    DetermineActCompletionProgress();
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_ACT_SELECT_LIST).SetDataSource(sgtGameSettingsGlobals.tActDataSource);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_LIST).SetDataSource(sgtGameSettingsGlobals.tQuestDataSource);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_STEP_LIST).SetDataSource(sgtGameSettingsGlobals.tQuestStepDataSource);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUICK_MATCH_TAGS).SetDataSource(sgtGameSettingsGlobals.tMatchmakingTagsDataSource);
    UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT).SetWrapSelection(FALSE);
    ResetSelections();
    sgtGameSettingsGlobals.tMatchmakingTagsDataSource.Initialize(sgtGameSettingsGlobals.eSelectedMode == ADVENTURE_MODE);
    RefreshDisplay();
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();
    if (dwFlags & (1 << DIFFICULTY_SELECT)) {
        ChangeSelectionMode(SELECT_MODE_DIFFICULTY);
    } else if (dwFlags & (1 << SHOW_TAGS)) {
        ::Console::Online::GameParams tParams;
        if (::Console::Online::PartyIsHost() && ::Console::Online::PartyGetGameParams(tParams)) {
            if (sgtGameSettingsGlobals.tMatchmakingTagsDataSource.GetNumItems() > 0 &&
                sgtGameSettingsGlobals.tMatchmakingTagsDataSource.GetTagIndex(tParams.m_nTag) == -1) {
                tParams.m_nTag = sgtGameSettingsGlobals.tMatchmakingTagsDataSource.m_arGameTags[0].m_tagId;
                ::Console::Online::PartySetGameParams(tParams);
            }
        }
        ChangeSelectionMode(SELECT_MODE_QUEST);
    } else {
        ChangeSelectionMode(SELECT_MODE_PLAYMODE);
    }
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("ConsoleUI:Close"));
    ::Console::Screens::UpdateButtonLegendText();
}

namespace {
SNO GetSelectedQuest();

#line 3419
CRefString GetQuestStepName(
    SNO snoQuest,
    int32 nQuestStep)
{
    CRefString sName;
    // The missing-quest branch skips the definition lock and returns the empty string.
    if (snoQuest == SNO_NONE)
    {
        return sName;
    }

    ScopedQuestDefinitionLockR ptDefQuest(snoQuest);
    if (ptDefQuest)
    {
        SNO snoStringList = QuestGetStringList(snoQuest);
        if (nQuestStep == -1)
        {
            sName = StringListFetch(snoStringList, gszUnassignedQuestStepBnetTitleLabel);
        }
        else
        {
            CRefString sNameLabel;
            sNameLabel.Format(gszQuestStepBnetTitleLabel, nQuestStep);
            sName = StringListFetch(snoStringList, sNameLabel);
        }
    }
    return sName;
}
}

#line 682
void QuestStepDataSource::UpdateItemUI(
    const UIConsoleListBox & tListBox,
    const UIID & idItemUI,
    const int32 nIndex)
{
    CRefString sText = GetQuestStepName(GetSelectedQuest(), m_arQuestStepUIDs[nIndex]);

    UIID idButton = UIIDCreateFromParent(idItemUI, ".select_highlight");
    BOOL bListFocused = UI_GetControllerFocus() == UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_STEP_LIST;
    UI_GetButtonFromID(idButton).SetButtonState(tListBox.GetSelectedIndex() == nIndex && bListFocused ? BS_PUSHED : BS_NORMAL, FALSE, FALSE);
    UI_GetButtonFromID(idButton).SetText(sText, FALSE);
}

#line 3118
// Original line 404.
void TagsDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    CRefString sStringKey = CRefString().Format("ConsoleGameSettings:GameTag_%s", m_arGameTags[nIndex].m_tag.str());
    CRefString sTagName = StringListFetch(sStringKey);
    UIID idButton = UIIDCreateFromParent(idItemUI, ".select_highlight");
    BOOL bListFocused = UI_GetControllerFocus() == UILIST_CONSOLE_GAME_SETTINGS_QUICK_MATCH_TAGS;
    UI_GetButtonFromID(idButton).SetButtonState(tListBox.GetSelectedIndex() == nIndex && bListFocused ? BS_PUSHED : BS_NORMAL, FALSE, FALSE);
    UI_GetButtonFromID(idButton).SetText(sTagName, FALSE);
}

// Original line 640.
void InitializeForGame()
{
    if (GameIsStartUp())
        return;
    sgtGameSettingsGlobals.nOriginalInGameDifficulty = GameGetHandicapLevel();
}

// Original line 650.
void DisposeForGame()
{
    sgtGameSettingsGlobals.tMatchmakingTagsDataSource.m_arGameTags.clear();
}

int32 TagsDataSource::GetNumItems() const
{
    return m_arGameTags.size();
}

// Original line 805.
void Open(int32 nControllerIndex, ScreenClosedCallback cb)
{
    uint32 dwFlags = sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION);
    CommonOpen(nControllerIndex, dwFlags, cb);
}

// Original line 816.
void OpenForDifficultySelect(int32 nControllerIndex, ScreenClosedCallback cb)
{
    uint32 dwFlags = (sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION)) | (1 << DIFFICULTY_SELECT);
    CommonOpen(nControllerIndex, dwFlags, cb);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_OPEN), cgtPSODefaults);
}

// Original line 828.
void OpenForMatchmaking(int32 nControllerIndex, ScreenClosedCallback cb)
{
    uint32 dwFlags = (sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION)) | (1 << MATCHMAKING_MODE);
    CommonOpen(nControllerIndex, dwFlags, cb);
}

// Original line 839.
void OpenForTags(int32 nControllerIndex, ScreenClosedCallback cb)
{
    ::Console::Online::GameParams tGameParams;
    if (::Console::Online::PartyGetGameParams(tGameParams) && tGameParams.m_eGameType == 6)
        return;
    uint32 dwFlags = (sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION)) | (1 << SHOW_TAGS);
    CommonOpen(nControllerIndex, dwFlags, cb);
}

// Original line 857.
void Close()
{
    if (!UI_GetControlFromID(UIWND_CONSOLE_GAME_SETTINGS).IsActive())
        return;
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_ACT_SELECT_LIST).SetSelectedIndex(-1);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_LIST).SetSelectedIndex(-1);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_STEP_LIST).SetSelectedIndex(-1);
    if (UI_TestID(UIWND_CONSOLE_GAME_SETTINGS_RESET_WARNING) &&
        UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_RESET_WARNING).IsActive())
        ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_GAME_SETTINGS_RESET_WARNING);
    UI_SetControllerFocus(GetUIIDNone());
    sgtGameSettingsGlobals.cbScreenClosed.Clear();
    ::delete sgtGameSettingsGlobals.ptDigest;
    sgtGameSettingsGlobals.ptDigest = NULL;
    ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_GAME_SETTINGS);
}

// Original line 900.
BOOL IsVisible()
{
    return UI_GetControlFromID(UIWND_CONSOLE_GAME_SETTINGS).IsActive();
}

void OnUpdate()
{
}

BOOL IsQuestFilteringEnabled()
{
    return sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION);
}

void ClearGameSettingsViewed()
{
    sgtGameSettingsGlobals.bGameSettingsViewed = FALSE;
}

// Original line 961.
BOOL GameSettingsViewed()
{
    return sgtGameSettingsGlobals.bGameSettingsViewed;
}

BOOL CanLowerGameDifficulty()
{
    Player * ptPlayer = LocalPlayerGetPrimary();
    if (!(ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE)) &&
        (GameIsStartUp() || !GameGetActiveGameCommonData().tGameSyncedData.bSetDungeonActive)) {
        int32 nCurrentDifficulty = GameGetHandicapLevel();
        if (nCurrentDifficulty > 0 && !IsDifficultyLocked(nCurrentDifficulty - 1, ptPlayer->nControllerIndex))
            return TRUE;
    }
    return FALSE;
}

void OnDifficultySelectionChanged(const UIID & id)
{
    int32 nSelected = UI_GetRadioButtonGroupFromID(id).GetSelectedValue();
    if (nSelected < 0)
        return;
    RefreshDifficultyDisplay();
    const UIID & idSelectedButton = UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT).GetSelection();
    if (UI_TestID(idSelectedButton)) {
        UIWindow & tButton = UI_GetWindowFromID(idSelectedButton);
        UIWindow & tCursor = UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_CURSOR);
        Rect2D rectButton;
        tButton.GetWindowRect(rectButton);
        Rect2D rectCursor;
        tCursor.GetWindowRect(rectCursor);
        Vector2D vecFinalTL;
        AlignRect(Vector2D(rectButton.left, rectButton.top), rectButton.Size(), ALIGN_CENTER_HORIZONTAL, rectCursor.top, rectCursor.Size(), vecFinalTL);
        tCursor.MoveTo(vecFinalTL);
        UIStoryboard & tHighlight = UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_HIGHLIGHT);
        tHighlight.Stop();
        tHighlight.Play(FALSE, 0.0f);
    }
    if (IsVisible())
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_GAMESETTINGS_DIFFICULTY_SCROLL), cgtPSODefaults);
}

void OnQuickMatchSelectionChanged(const UIID & id)
{
    int32 nSelected = UI_GetRadioButtonGroupFromID(id).GetSelectedValue();
    if (nSelected < 0)
        return;
    RefreshActDisplay();
    UIStoryboard & tHighlight = UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_GAME_SETTINGS_ACT_SELECT_HIGHLIGHT);
    tHighlight.Stop();
    tHighlight.Play(FALSE, 0.0f);
    if (IsVisible())
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_GAMESETTINGS_ACTQUEST_SCROLL), cgtPSODefaults);
}
}
}
float SNOPackChunkGetActLoadPercentComplete(Act eAct);  // X/Common/SNO/SNOFiles.cpp
BOOL SNOPackChunkLoadAct(Act eAct);  // X/Common/SNO/SNOFiles.cpp
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);  // X/Client/CGame.cpp
namespace UIGameSettings { namespace Console {

namespace {
BOOL SelectionsPreserveCampaignSave(GameSettingsMode eCurrentMode);
void ShowOverwriteWarning();

// Original line 1570.
BOOL GetActUnlocked(Act eAct)
{
    uint32 dwCompletion = 1 << (eAct / 100);
    return sgtGameSettingsGlobals.dwActUnlockedProgress & dwCompletion;
}

// Original line 3478.
PlayMode GetSelectedPlayMode()
{
    int32 nSelectedValue = UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_MODE_GROUP).GetSelectedValue();
    return (PlayMode)Math::Pin(nSelectedValue, CAMPAIGN_MODE, CHALLENGE_MODE);
}

// Original line 3486.
Act GetSelectedAct()
{
    int32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_ACT_SELECT_LIST).GetSelectedIndex();
    if (nSelectedIndex >= 0 && nSelectedIndex < 5)
        return arActList[nSelectedIndex];
    return ACT_1;
}

// Original line 3501.
SNO GetSelectedQuest()
{
    int32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_LIST).GetSelectedIndex();
    if (nSelectedIndex >= 0 && nSelectedIndex < sgtGameSettingsGlobals.tQuestDataSource.GetNumItems())
        return sgtGameSettingsGlobals.tQuestDataSource.m_arQuestSnos[nSelectedIndex];
    return SNO_NONE;
}

// Original line 3516.
int32 GetSelectedQuestStep()
{
    int32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_STEP_LIST).GetSelectedIndex();
    if (nSelectedIndex >= 0 && nSelectedIndex < sgtGameSettingsGlobals.tQuestStepDataSource.GetNumItems())
        return sgtGameSettingsGlobals.tQuestStepDataSource.m_arQuestStepUIDs[nSelectedIndex];
    return -1;
}

// Original line 3531.
int32 GetSelectedDifficulty()
{
    int32 nSelectedValue = UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT).GetSelectedValue();
    return Math::Pin(nSelectedValue, 0, 19);
}

// Original line 3539.
int32 GetSelectedQuickMatchOption()
{
    int32 nSelectedValue = UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_QUICK_MATCH).GetSelectedValue();
    return Math::Pin(nSelectedValue, 0, 2);
}

// Original line 3547.
OnlineService::GameTagId GetSelectedTag()
{
    int32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUICK_MATCH_TAGS).GetSelectedIndex();
    if (nSelectedIndex >= 0 && nSelectedIndex < sgtGameSettingsGlobals.tMatchmakingTagsDataSource.GetNumItems())
        return sgtGameSettingsGlobals.tMatchmakingTagsDataSource.m_arGameTags[nSelectedIndex].m_tagId;
    return OnlineService::GAME_TAG_ID_NONE;
}
}

// Original line 1037.
// Inference: the difficulty-lock return tests the flag first (original line 1171: flag load at column 26, bDifficultyLocked at column 95).
void OnAccept(const UIID & id, const int32 nControllerIndex)
{
    if (!::Console::Online::PartyIsInParty())
        return;
    BOOL bMatchmakingMode = (sgtGameSettingsGlobals.dwFlags >> MATCHMAKING_MODE) & 1;
    switch (sgtGameSettingsGlobals.eCurrentMode) {
    case SELECT_MODE_PLAYMODE:
        sgtGameSettingsGlobals.eSelectedMode = GetSelectedPlayMode();
        sgtGameSettingsGlobals.tMatchmakingTagsDataSource.Initialize(sgtGameSettingsGlobals.eSelectedMode == ADVENTURE_MODE);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_GAMESETTINGS_MODE_CONFIRM), cgtPSODefaults);
        if (bMatchmakingMode && sgtGameSettingsGlobals.eSelectedMode == ADVENTURE_MODE)
            ChangeSelectionMode(SELECT_MODE_QUEST);
        else if (sgtGameSettingsGlobals.eSelectedMode == CHALLENGE_MODE) {
            if (!sgtGameSettingsGlobals.bChallengeRiftDataAvailable)
                return;
            if (!SNOPackChunkLoadAct(ACT_OPEN_WORLD)) {
                NotificationCreationParams tParams;
                tParams.uszMessage = LocalizedSprintf(StringListFetch("ConsoleErrors:CannotProceedGenericContentNotReady").str(),
                    UnicodeText::FormatNumber(SNOPackChunkGetActLoadPercentComplete(ACT_OPEN_WORLD) * 100.0f, TRUE, FNS_NORMAL, 2, 2, 0).str(), NULL);
                ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
                return;
            }
            ChangeSelectionMode(SELECT_MODE_CHALLENGE);
        } else
            ChangeSelectionMode(SELECT_MODE_ACT);
        break;
    case SELECT_MODE_ACT:
        if (bMatchmakingMode) {
            if (sgtGameSettingsGlobals.eSelectedMode == CAMPAIGN_MODE)
                sgtGameSettingsGlobals.nSelectedQuickMatchOption = GetSelectedQuickMatchOption();
            else
                sgtGameSettingsGlobals.eSelectedAdventureAct = GetSelectedAct();
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_GAMESETTINGS_ACTQUEST_CONFIRM), cgtPSODefaults);
            ChangeSelectionMode(SELECT_MODE_QUEST);
        } else {
            if ((sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION)) &&
                sgtGameSettingsGlobals.eSelectedMode != ADVENTURE_MODE && !GetActUnlocked(GetSelectedAct()))
                return;
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_GAMESETTINGS_ACTQUEST_CONFIRM), cgtPSODefaults);
            if (sgtGameSettingsGlobals.eSelectedMode == CAMPAIGN_MODE) {
                sgtGameSettingsGlobals.eSelectedCampaignAct = GetSelectedAct();
                FallbackSelectedChoices();
                if (!sgtGameSettingsGlobals.bOverwriteWarningAccepted && !SelectionsPreserveCampaignSave(SELECT_MODE_ACT)) {
                    ShowOverwriteWarning();
                    return;
                }
                ChangeSelectionMode(SELECT_MODE_QUEST);
            } else {
                sgtGameSettingsGlobals.eSelectedAdventureAct = GetSelectedAct();
                ChangeSelectionMode(SELECT_MODE_DIFFICULTY);
            }
        }
        break;
    case SELECT_MODE_QUEST:
        if (UI_GetControllerFocus() == UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_LIST) {
            sgtGameSettingsGlobals.snoSelectedQuest = GetSelectedQuest();
            if (!sgtGameSettingsGlobals.bOverwriteWarningAccepted && !SelectionsPreserveCampaignSave(SELECT_MODE_QUEST))
                ShowOverwriteWarning();
            else {
                UI_SetControllerFocus(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_STEP_LIST);
                RefreshQuestDisplay();
                ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("ConsoleGameSettings:SelectQuestStep"));
                ::Console::Screens::UpdateButtonLegendText();
            }
        } else if (UI_GetControllerFocus() == UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_STEP_LIST) {
            sgtGameSettingsGlobals.nSelectedQuestStep = GetSelectedQuestStep();
            if (!sgtGameSettingsGlobals.bOverwriteWarningAccepted && !SelectionsPreserveCampaignSave(SELECT_MODE_QUEST))
                ShowOverwriteWarning();
            else
                ChangeSelectionMode(SELECT_MODE_DIFFICULTY);
        } else if (UI_GetControllerFocus() == UILIST_CONSOLE_GAME_SETTINGS_QUICK_MATCH_TAGS) {
            sgtGameSettingsGlobals.nSelectedQuickMatchTag = GetSelectedTag();
            if (bMatchmakingMode)
                ChangeSelectionMode(SELECT_MODE_DIFFICULTY);
            else
                ConfirmGameParams(TRUE, TRUE);
        }
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_GAMESETTINGS_ACTQUEST_CONFIRM), cgtPSODefaults);
        break;
    case SELECT_MODE_DIFFICULTY: {
        BOOL bDifficultyLocked = UI_GetPrimaryUIController() == -1 || IsDifficultyLocked(GetSelectedDifficulty(), -1);
        if ((sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION)) && bDifficultyLocked)
            return;
        sgtGameSettingsGlobals.nSelectedDifficulty = GetSelectedDifficulty();
        if (GameIsStartUp())
            ConfirmGameParams(bMatchmakingMode, TRUE);
        else {
            Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
            int32 nCurrentDifficulty = GameGetHandicapLevel();
            if (sgtGameSettingsGlobals.nSelectedDifficulty != nCurrentDifficulty) {
                IntDataMessage tMessage;
                tMessage.nData = sgtGameSettingsGlobals.nSelectedDifficulty;
                MessageSendToServer(CMSG_TRY_CHANGE_DIFFICULTY, &tMessage, sizeof(tMessage), ptPlayer);
            }
            ConfirmGameParams(TRUE, TRUE);
        }
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_GAMESETTINGS_DIFFICULTY_CONFIRM), cgtPSODefaults);
        break;
    }
    case SELECT_MODE_CHALLENGE:
        if (GameIsStartUp())
            ConfirmGameParams(bMatchmakingMode, TRUE);
        break;
    default:
        break;
    }
}

void OnActListSelectionChanged(const UIID &, int32, int32 nNewVal)
{
    if (nNewVal < 0)
        return;
    UIStoryboard & tHighlight = UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_GAME_SETTINGS_ACT_SELECT_HIGHLIGHT);
    tHighlight.Stop();
    tHighlight.Play(FALSE, 0.0f);
    UpdateActDescriptions();
    int32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_ACT_SELECT_LIST).GetSelectedIndex();
    Act eAct = (nSelectedIndex >= 0 && nSelectedIndex < 5) ? arActList[nSelectedIndex] : ACT_1;
    SNO snoAct = ActSNOFromActEnum(eAct);
    ScopedActDefinitionLockR ptActDef(snoAct);
    if (ptActDef) {
        sgtGameSettingsGlobals.tQuestDataSource.m_arQuestSnos.clear();
        int32 nNumQuest = ActGetNumQuests(ptActDef);
        for (int32 quest = 0; quest < nNumQuest; ++quest) {
            SNO snoQuest = ptActDef->arQuests[quest].snoQuest;
            if (snoQuest != SNO_NONE && QuestGetQuestType(snoQuest) == QUEST_TYPE_MAIN_QUEST)
                sgtGameSettingsGlobals.tQuestDataSource.m_arQuestSnos.push_back(snoQuest);
        }
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_LIST).RefreshVisibleItems();
    }
    int32 nSelectedQuest = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_LIST).GetSelectedIndex();
    SNO snoQuest = (nSelectedQuest >= 0 && nSelectedQuest < (int32)sgtGameSettingsGlobals.tQuestDataSource.m_arQuestSnos.size()) ?
        sgtGameSettingsGlobals.tQuestDataSource.m_arQuestSnos[nSelectedQuest] : SNO_NONE;
    UpdateStepListForSelectedQuest(snoQuest);
}

void OnQuestSelectionChanged(const UIID &, int32, int32 nNewVal)
{
    if (nNewVal < 0)
        return;
    int32 nSelected = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_LIST).GetSelectedIndex();
    SNO snoQuest = (nSelected >= 0 && nSelected < (int32)sgtGameSettingsGlobals.tQuestDataSource.m_arQuestSnos.size()) ?
        sgtGameSettingsGlobals.tQuestDataSource.m_arQuestSnos[nSelected] : SNO_NONE;
    UpdateStepListForSelectedQuest(snoQuest);
    RefreshQuestDisplay();
}

void OnQuestStepSelectionChanged(const UIID &, int32, int32 nNewVal)
{
    if (nNewVal >= 0)
        UpdateQuestStepDescriptions();
}

void OnViewLeaderboard(const UIID & id, const int32 nControllerIndex)
{
    if (sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_CHALLENGE &&
        sgtGameSettingsGlobals.eSelectedMode == CHALLENGE_MODE &&
        ::Console::GamerProfile::IsSignedInOnline(::Console::GamerProfile::GetPrimaryProfileUserIndex())) {
        ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_GAME_SETTINGS);
        UILeaderboards::Console::OpenForGameSettings(nControllerIndex, TRUE);
    }
}

void OnDPadLeft(const UIID & id, const int32 nControllerIndex)
{
    if (sgtGameSettingsGlobals.eSelectedMode != CHALLENGE_MODE || sgtGameSettingsGlobals.eCurrentMode != SELECT_MODE_CHALLENGE)
        return;
    int32 leaderboardToDisplay = sgtGameSettingsGlobals.nCurrentChallengeLeaderboardDisplayed <= LB_CHALLENGE_RIFT_FIRST ?
        LB_CHALLENGE_RIFT_LAST : sgtGameSettingsGlobals.nCurrentChallengeLeaderboardDisplayed - 1;
    if (leaderboardToDisplay > LB_CHALLENGE_RIFT_LAST)
        leaderboardToDisplay = LB_CHALLENGE_RIFT_LAST;
    if (leaderboardToDisplay < LB_CHALLENGE_RIFT_FIRST)
        leaderboardToDisplay = LB_CHALLENGE_RIFT_FIRST;
    PopulateLeaderboard((LeaderboardIds)leaderboardToDisplay);
}

void OnDPadRight(const UIID & id, const int32 nControllerIndex)
{
    if (sgtGameSettingsGlobals.eSelectedMode != CHALLENGE_MODE || sgtGameSettingsGlobals.eCurrentMode != SELECT_MODE_CHALLENGE)
        return;
    int32 leaderboardToDisplay = sgtGameSettingsGlobals.nCurrentChallengeLeaderboardDisplayed >= LB_CHALLENGE_RIFT_LAST ?
        LB_CHALLENGE_RIFT_FIRST : sgtGameSettingsGlobals.nCurrentChallengeLeaderboardDisplayed + 1;
    if (leaderboardToDisplay > LB_CHALLENGE_RIFT_LAST)
        leaderboardToDisplay = LB_CHALLENGE_RIFT_LAST;
    if (leaderboardToDisplay < LB_CHALLENGE_RIFT_FIRST)
        leaderboardToDisplay = LB_CHALLENGE_RIFT_FIRST;
    PopulateLeaderboard((LeaderboardIds)leaderboardToDisplay);
}

void OnReturnFromLeaderboard(int32 nControllerIndex)
{
    ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_GAME_SETTINGS, TRUE, GetUIIDNone());
    ChangeSelectionMode(SELECT_MODE_CHALLENGE);
}

void OnOpenResetWarning(const UIID & id)
{
    sgtGameSettingsGlobals.nQuestResetOpenTime = Blizzard::Time::Milliseconds();
    RefreshResetWarning(id);
}

// Original line 1326.
void OnUpdateResetWarning(const UIID & id)
{
    RefreshResetWarning(id);
}

void OnCancelResetWarning(const UIID & id, const int32 nControllerIndex)
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
    ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_GAME_SETTINGS_RESET_WARNING);
}

// NON_MATCHING: 600/600 bytes, 77 instructions differ in loop branch layout and register allocation after the flag load.
// Matchmaking and non-matchmaking paths reject a different list in adventure mode (original lines 1397 and 1401).
#line 1373
void OnRightShoulder(
    const UIID & id, const int32 nControllerIndex)
{
    int32 nSelected = UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_MODE_GROUP).GetSelectedValue();
    if (nSelected == CHALLENGE_MODE)
    {
        return;
    }

    sgtGameSettingsGlobals.eSelectedMode = GetSelectedPlayMode();

    if (sgtGameSettingsGlobals.dwFlags & ((1 << DIFFICULTY_SELECT) | (1 << SHOW_TAGS)))
    {
        return;
    }

    BOOL bMatchMakingMode = (sgtGameSettingsGlobals.dwFlags & (1 << MATCHMAKING_MODE)) != 0;
    BOOL bValidMode = FALSE;
    while (!bValidMode)
    {
        sgtGameSettingsGlobals.eCurrentMode = (GameSettingsMode)Math::PinWrap((int32)sgtGameSettingsGlobals.eCurrentMode + 1, 0, (int32)NUM_SELECT_MODES);
        BOOL bIsChallengeMode = sgtGameSettingsGlobals.eSelectedMode == CHALLENGE_MODE || sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_CHALLENGE;
        if (bMatchMakingMode)
        {
            bValidMode = !bIsChallengeMode && (sgtGameSettingsGlobals.eSelectedMode != ADVENTURE_MODE || sgtGameSettingsGlobals.eCurrentMode != SELECT_MODE_ACT);
        }
        else
        {
            bValidMode = !bIsChallengeMode && (sgtGameSettingsGlobals.eSelectedMode != ADVENTURE_MODE || sgtGameSettingsGlobals.eCurrentMode != SELECT_MODE_QUEST);
        }
    }
    ChangeSelectionMode(sgtGameSettingsGlobals.eCurrentMode);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_GAMESETTINGS_SHOULDER), cgtPSODefaults);
}
#line 3592

void OnLeftShoulder(const UIID & id, const int32 nControllerIndex)
{
    if (UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_MODE_GROUP).GetSelectedValue() == CHALLENGE_MODE)
        return;
    int32 nSelectedValue = UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_MODE_GROUP).GetSelectedValue();
    sgtGameSettingsGlobals.eSelectedMode = (PlayMode)nSelectedValue;
    if (nSelectedValue > CHALLENGE_MODE)
        sgtGameSettingsGlobals.eSelectedMode = CHALLENGE_MODE;
    if (nSelectedValue < CAMPAIGN_MODE)
        sgtGameSettingsGlobals.eSelectedMode = CAMPAIGN_MODE;
    OnLeftShoulder(TRUE);
}
// Line table: the DIFFICULTY_SELECT|MATCHMAKING_MODE test is on its own line (1273), the SHOW_TAGS and mode tests follow on 1275.
void OnResetQuest(const UIID & id, const int32 nControllerIndex)
{
    if (sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_CHALLENGE && sgtGameSettingsGlobals.eSelectedMode == CHALLENGE_MODE) {
        ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_GAME_SETTINGS);
        UIUtils::Console::OpenInspectLeaderboardForChallengeRift();
    } else if (!(sgtGameSettingsGlobals.dwFlags & ((1 << DIFFICULTY_SELECT) | (1 << MATCHMAKING_MODE)))) {
        if (!(sgtGameSettingsGlobals.dwFlags & (1 << SHOW_TAGS)) && sgtGameSettingsGlobals.eCurrentMode != SELECT_MODE_DIFFICULTY) {
            D3::Hero::QuestHistoryList tQuestHistory;
            tQuestHistory.mutable_quest_history()->MergeFrom(sgtGameSettingsGlobals.tHeroDefinition.digest().quest_history());
            if ((!(sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION)) || HeroCompletedAllQuests(tQuestHistory, TRUE)) &&
                !UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_RESET_WARNING).IsActive())
                ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_GAME_SETTINGS_RESET_WARNING, TRUE, GetUIIDNone());
        }
    }
}

#line 505
void TagsDataSource::Initialize(
    BOOL bAdventureMode)
{
    OnlineService::GetDefaultGameTags(m_arGameTags);

    for (int32 i = 0; i < (int32)m_arGameTags.size(); ++i)
    {
        if (!UIUtils::Console::GameTagIsValid(m_arGameTags[i].m_tagId, bAdventureMode))
        {
            blzext::vector_erase_at(m_arGameTags, i--);
        }
        else if (m_arGameTags[i].m_tagId == OnlineService::GAME_TAG_ID_GREATER_RIFT)
        {
            int32 nMinLocalHeroLevel = UIMainMenu::Console::GetMinLocalHeroLevel();
            if (nMinLocalHeroLevel < HERO_LEVEL_MAX)
            {
                blzext::vector_erase_at(m_arGameTags, i--);
            }
        }
    }
}
#line 3642

// Original line 1333. This real reset callback retains the act-history query.
void OnAcceptResetWarning(const UIID & id, const int32 nControllerIndex)
{
    if (!HasResetCooldownPassed())
        return;
    ::Console::UIOnlineActions::ResetQuests(sgtGameSettingsGlobals.tHeroDefinition);
    sgtGameSettingsGlobals.ptDigest->m_completedQuestMap->clear();
    sgtGameSettingsGlobals.ptDigest->PopulateCompletedQuestList(
        sgtGameSettingsGlobals.tHeroDefinition.digest().quest_history());
    ::Console::UIOnlineActions::ResetQuests(::Console::GamerProfile::GetHero(nControllerIndex));
    if (::Console::Online::PartyIsInParty())
        ::Console::Online::PartyUpdatePlayerHero(nControllerIndex);
    ::Console::GamerProfile::Save(nControllerIndex, OnlineService::Callback<void()>());
    DetermineActCompletionProgress();
    FallbackSelectedChoices();
    RefreshSelectionDisplay();
    UIMainMenu::Console::RefreshMainMenu(0);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
    ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_GAME_SETTINGS_RESET_WARNING);
    ConfirmGameParams(TRUE, FALSE);
    ChangeSelectionMode(SELECT_MODE_ACT);
}

namespace {
// Original line 1473. tQuest's initial -1 is observable before the loop.
void DetermineActCompletionProgress()
{
    if (sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION)) {
        OnlineService::QuestId tQuest;
        BOOL fIsPreviousActCompleted = TRUE;
        for (int32 j = 0; j < 5; ++j) {
            SNO snoAct = ActSNOFromActEnum(arActList[j]);
            if (snoAct == SNO_NONE || !SNOFileExists(ACT_SNO, snoAct))
                continue;
            ScopedActDefinitionLockR ptDefAct(snoAct);
            int32 nNumQuests = ActGetNumQuests(ptDefAct);
            BOOL bHasCompletedQuests = (j == 0);
            for (int32 quest = 0; quest < nNumQuests && !bHasCompletedQuests; ++quest) {
                SNO snoQuest = ptDefAct->arQuests[quest].snoQuest;
                if (snoQuest == SNO_NONE)
                    continue;
                tQuest.snoQuest = snoQuest;
                if (sgtGameSettingsGlobals.ptDigest->IsQuestCompleted(tQuest))
                    bHasCompletedQuests = TRUE;
            }
            BOOL bFinalQuestIsComplete = FALSE;
            if (nNumQuests > 0) {
                tQuest.snoQuest = ptDefAct->arQuests[nNumQuests - 1].snoQuest;
                if (sgtGameSettingsGlobals.ptDigest->IsQuestCompleted(tQuest))
                    bFinalQuestIsComplete = TRUE;
            }
            if (j == 3 && sgtGameSettingsGlobals.tHeroDefinition.digest().highest_unlocked_act() == ACT_5)
                bFinalQuestIsComplete = TRUE;
            SetActComplete(arActList[j], bFinalQuestIsComplete);
            SetActUnlocked(arActList[j], bHasCompletedQuests || fIsPreviousActCompleted);
            fIsPreviousActCompleted = bFinalQuestIsComplete;
        }
    } else {
        sgtGameSettingsGlobals.dwActUnlockedProgress = 0xffffffff;
        sgtGameSettingsGlobals.dwActCompletionProgress = 0xffffffff;
    }
}

// Original line 1553.
void SetActUnlocked(Act eAct, BOOL bComplete)
{
    uint32 dwCompletion = 1 << (eAct / 100);
    if (bComplete)
        sgtGameSettingsGlobals.dwActUnlockedProgress |= dwCompletion;
    else
        sgtGameSettingsGlobals.dwActUnlockedProgress &= ~dwCompletion;
}
// Original line 1579.
void SetActComplete(Act eAct, BOOL bComplete)
{
    uint32 dwCompletion = 1 << (eAct / 100);
    if (bComplete)
        sgtGameSettingsGlobals.dwActCompletionProgress |= dwCompletion;
    else
        sgtGameSettingsGlobals.dwActCompletionProgress &= ~dwCompletion;
}
// Original line 1596.
BOOL GetActComplete(Act eAct)
{
    uint32 dwCompletion = 1 << (eAct / 100);
    return sgtGameSettingsGlobals.dwActCompletionProgress & dwCompletion;
}

// Original line 2337; fully inlined into CommonOpen.
void ResetSelections()
{
    ::Console::Online::GameParams tGameParams;
    if (::Console::Online::PartyGetGameParams(tGameParams)) {
        sgtGameSettingsGlobals.eSelectedMode = tGameParams.m_eGameType == 6 ? CHALLENGE_MODE :
            (tGameParams.m_eAct == ACT_OPEN_WORLD ? ADVENTURE_MODE : CAMPAIGN_MODE);
        sgtGameSettingsGlobals.nSelectedDifficulty = GameIsStartUp() ? tGameParams.m_nHandicap : GameGetHandicapLevel();
        sgtGameSettingsGlobals.nSelectedQuickMatchOption = 1;
        sgtGameSettingsGlobals.nSelectedQuickMatchTag = tGameParams.m_nTag == OnlineService::GAME_TAG_ID_NONE ? OnlineService::GAME_TAG_ID_DEFAULT : tGameParams.m_nTag;
        ResetQuestSelections();
    }
}

// Original line 2899.
BOOL HasResetCooldownPassed()
{
    int32 nMillisecondsPassed = Blizzard::Time::Milliseconds() - sgtGameSettingsGlobals.nQuestResetOpenTime;
    return nMillisecondsPassed > RESET_QUEST_COOLDOWN_MILLISECS;
}

// Original line 3389.
BOOL IsQuestSelectable(SNO snoQuest)
{
    if (!(sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION)))
        return TRUE;
    if (!SNOFileExists(QUEST_SNO, snoQuest))
        return FALSE;
    SNO snoAct = ActSNOFromActEnum(sgtGameSettingsGlobals.eSelectedCampaignAct);
    ScopedActDefinitionLockR pDefAct(snoAct);
    OnlineService::QuestId tQuest;
    tQuest.snoQuest = snoQuest;
    BOOL bIsFirstQuestUnlocked = TRUE;
    if (sgtGameSettingsGlobals.eSelectedCampaignAct > ACT_1)
        bIsFirstQuestUnlocked = GetActComplete(sgtGameSettingsGlobals.eSelectedCampaignAct - 100) != 0;
    BOOL bIsFirstQuest = ActGetNumQuests(pDefAct) > 0 &&
        pDefAct->arQuests[0].snoQuest == snoQuest;
    return (bIsFirstQuest && bIsFirstQuestUnlocked) ||
        (sgtGameSettingsGlobals.ptDigest && sgtGameSettingsGlobals.ptDigest->GetHighestProgressedStep(tQuest) != -2);
}


void RefreshDisplay()
{
    RefreshSelectionDisplay();
    RefreshPlayModeDisplay();
    RefreshActDisplay();
    RefreshQuestDisplay();
    RefreshDifficultyDisplay();
    D3::Hero::QuestHistoryList tQuestHistory;
    tQuestHistory.mutable_quest_history()->MergeFrom(sgtGameSettingsGlobals.tHeroDefinition.digest().quest_history());
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_DIFFICULTY ||
        (sgtGameSettingsGlobals.dwFlags & ((1 << DIFFICULTY_SELECT) | (1 << MATCHMAKING_MODE) | (1 << SHOW_TAGS))) ||
        ((sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION)) && !HeroCompletedAllQuests(tQuestHistory, TRUE)))
        tActionManager.ClearAction(::Console::CONTROLLER_INPUT_BUTTON_4);
    else
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, 1, StringListFetch("ConsoleGameSettings:ResetCampaign"));
    ::Console::Screens::UpdateButtonLegendText();
}

void OnOverwriteWarningAccepted(const UIID &)
{
    sgtGameSettingsGlobals.bOverwriteWarningAccepted = TRUE;
    OnAccept(GetUIIDNone(), ::Console::GamerProfile::GetPrimaryProfileUserIndex());
}

void OnOverwriteWarningCanceled(const UIID &)
{
    ResetQuestSelections();
    RefreshDisplay();
}

void ShowOverwriteWarning()
{
    NotificationCreationParams tParams;
    tParams.nControllerIndex = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    tParams.uszMessage = StringListFetch("ConsoleQuestSelect:OverwriteWarning");
    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &OnOverwriteWarningAccepted>();
    tParams.tCallbacks.pfnOnClickCancelButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &OnOverwriteWarningCanceled>();
    ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
}

}
}
}
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_LEADERBOARD_TITLE;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_LEADERBOARD_TIER_NUMBER;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_LEADERBOARD_TIME_SPENT;
#include "../../Battlenet/UI/Util/UIUtil.h"












// X/Client/Battlenet/UI/Util/UIUtil.cpp
CRefString DisplayElapsedTime(const Blizzard::Time::Timestamp & tStart, const Blizzard::Time::Timestamp & tEnd, const ElapsedTimeDisplayOptions & tElapsedTimeDisplayOptions);
// X/Common/Leaderboard.cpp
uint32 LeaderboardGetRiftScoreLevel(uint64 score);
Blizzard::Time::Timestamp LeaderboardGetRiftScoreTimeToComplete(uint64 score);

namespace UIGameSettings { namespace Console { namespace {
// Original line 3262.
void PopulateLeaderboard(LeaderboardIds id)
{
    CRefString sLabel;
    switch (id) {
    case LB_CHALLENGE_RIFT_TEAM_1:
        sLabel = "Bnet_LeaderBoards:FilterOnePlayer";
        break;
    case LB_CHALLENGE_RIFT_TEAM_2:
        sLabel = "Bnet_LeaderBoards:FilterTwoPlayers";
        break;
    case LB_CHALLENGE_RIFT_TEAM_3:
        sLabel = "Bnet_LeaderBoards:FilterThreePlayers";
        break;
    default:
        sLabel = "Bnet_LeaderBoards:FilterFourPlayers";
        break;
    }
    UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_LEADERBOARD_TITLE).SetText(StringListFetch(sLabel).str(), FALSE);
    sgtGameSettingsGlobals.nCurrentChallengeLeaderboardDisplayed = id;
    UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_LEADERBOARD_TIER_NUMBER).SetText("", FALSE);
    UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_LEADERBOARD_TIME_SPENT).SetText("", FALSE);
    if (sgtGameSettingsGlobals.arPersonalBests.count(id)) {
        D3::Leaderboard::Score tMyScore = sgtGameSettingsGlobals.arPersonalBests[id];
        uint32 nScore = LeaderboardGetRiftScoreLevel(tMyScore.score());
        CRefString strFormattedScore = UnicodeText::FormatNumber((int32)nScore, TRUE, FNS_NORMAL, 0);
        CRefString sFormattedTierNumber = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHALLENGE_RIFT), "ChallengeMode_Tier"), strFormattedScore.str(), NULL);
        UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_LEADERBOARD_TIER_NUMBER).SetText(sFormattedTierNumber.str(), FALSE);
        Blizzard::Time::Timestamp nTimeToComplete = LeaderboardGetRiftScoreTimeToComplete(tMyScore.score());
        ElapsedTimeDisplayOptions tElapsedTimeDisplayOptions;
        tElapsedTimeDisplayOptions.fAlwaysShowSeconds = TRUE;
        tElapsedTimeDisplayOptions.fOnlyDisplayNumber = TRUE;
        tElapsedTimeDisplayOptions.fCountUp = TRUE;
        tElapsedTimeDisplayOptions.fDisplayMilliseconds = TRUE;
        CRefString strFormattedTime = DisplayElapsedTime(0, nTimeToComplete, tElapsedTimeDisplayOptions);
        CRefString sFormattedTimeSpent = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHALLENGE_RIFT), "ChallengeMode_TimeSpent"), strFormattedTime.str(), NULL);
        UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_LEADERBOARD_TIME_SPENT).SetText(sFormattedTimeSpent.str(), FALSE);
    }
}
} } }
extern UIID UISTORYBOARD_CONSOLE_GAME_SETTINGS_MODE_HIGHLIGHT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_CONTENT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_DISABLED;
extern UIID UIRACTOR_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_HERO;
UIRActor & UI_GetRActorFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
namespace Console { namespace Online {
// X/Client/Console/LobbyService.cpp
void GetChallengeRiftData(int32 nUserIndex, OnlineService::Callback<void (StorageResult, const D3::ChallengeRifts::ChallengeData &, const D3::Leaderboard::WeeklyChallengeData &)> pfnCallback);
}
}
namespace UIGameSettings { namespace Console { namespace {
// Original line 3000; definition follows below on its DWARF rows.
void sOnGetChallengeRiftData(int32 nControllerIndex, ::Console::Online::StorageResult eResult, const D3::ChallengeRifts::ChallengeData & tChallengeData, const D3::Leaderboard::WeeklyChallengeData & tWeeklyChallengeData);

// Original line 2969.
void sActivateChallengeRiftUIElements(BOOL bActive)
{
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_CONTENT).SetActive(bActive);
    if (!bActive)
        UI_GetRActorFromID(UIRACTOR_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_HERO).Cleanup();
}

// Original line 2202.
void OnPlayModeChanged(int32 nNewIndex)
{
    UIStoryboard & tHighlight = UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_GAME_SETTINGS_MODE_HIGHLIGHT);
    tHighlight.Stop();
    tHighlight.Play(FALSE, 0.0f);
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_CONTENT).SetActive(nNewIndex == CHALLENGE_MODE);
    UI_GetWindowFromID(UILIST_CONSOLE_GAME_SETTINGS_ACT_SELECT_LIST).SetActive(nNewIndex != CHALLENGE_MODE);
    if (nNewIndex == CAMPAIGN_MODE)
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_ACT_SELECT_LIST).SetSelectedIndex(sgtGameSettingsGlobals.eSelectedCampaignAct / 100);
    else if (nNewIndex == ADVENTURE_MODE)
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_ACT_SELECT_LIST).SetSelectedIndex(sgtGameSettingsGlobals.eSelectedAdventureAct / 100);
    RefreshPlayModeDisplay();
    RefreshActDisplay();
    if (IsVisible())
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_GAMESETTINGS_MODE_SCROLL), cgtPSODefaults);
    if (nNewIndex == CHALLENGE_MODE && ::Console::Online::PartyIsHost() && !sgtGameSettingsGlobals.bAskingForChallengeRiftData) {
        if (!sgtGameSettingsGlobals.tChallengeData.has_challenge_number() ||
            (sgtGameSettingsGlobals.tChallengeData.has_challenge_end_unix_time_console() &&
             Blizzard::Time::ToUnixTime64(Blizzard::Time::GetTimestamp()) >= sgtGameSettingsGlobals.tChallengeData.challenge_end_unix_time_console()) ||
            Blizzard::Time::GetTimestamp() >= sgtGameSettingsGlobals.uCacheWeeklyChallengeUntilTimestamp) {
            if (!sgtGameSettingsGlobals.tChallengeData.has_challenge_number())
                sActivateChallengeRiftUIElements(FALSE);
            sgtGameSettingsGlobals.bAskingForChallengeRiftData = TRUE;
            int32 nUserIndex = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
            ::Console::Online::GetChallengeRiftData(nUserIndex, OnlineService::internal::function_bind_1_3<void, int32, ::Console::Online::StorageResult, const D3::ChallengeRifts::ChallengeData &, const D3::Leaderboard::WeeklyChallengeData &, &sOnGetChallengeRiftData>(nUserIndex));
        }
    }
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_DISABLED).SetActive(nNewIndex == CHALLENGE_MODE && !sgtGameSettingsGlobals.bChallengeRiftDataAvailable && !sgtGameSettingsGlobals.bAskingForChallengeRiftData);
}
}

// Original line 968.
void OnPlayModeSelectionChanged(const UIID & id)
{
    int32 nSelected = UI_GetRadioButtonGroupFromID(id).GetSelectedValue();
    if (nSelected >= 0)
        OnPlayModeChanged(nSelected);
}
}
}
namespace UIGameSettings { namespace Console { namespace {
// Original line 3326.
BOOL IsResumeFromSave(Act eAct, SNO snoInitialQuest, int32 nInitialQuestStepUID)
{
    const D3::Hero::Digest & tDigest = sgtGameSettingsGlobals.tHeroDefinition.digest();
    return tDigest.last_played_act() == eAct &&
        tDigest.last_played_quest() == snoInitialQuest &&
        tDigest.last_played_quest_step() == nInitialQuestStepUID;
}

// Original line 2062.
extern "C" BOOL sub_710072e0b0(uint32 uSeasonCreated, uint32 uFlags);
// NON_MATCHING: Clang lays out the warning paths differently and splits a paired stack store.
void ConfirmGameParams(BOOL bSkipWarning, BOOL bCloseScreen)
{
    ::Console::Online::GameParams tGameParams;
    ::Console::Online::PartyGetGameParams(tGameParams);
    BOOL bMatchMakingMode = (sgtGameSettingsGlobals.dwFlags >> MATCHMAKING_MODE) & 1;
    int32 nHandicap = sgtGameSettingsGlobals.nSelectedDifficulty;
    BOOL bShowTags = sgtGameSettingsGlobals.dwFlags & (1 << SHOW_TAGS);
    ScreenClosedCallback cbCopy;
    if (sgtGameSettingsGlobals.eSelectedMode == CAMPAIGN_MODE) {
        SNO snoInitialQuest = sgtGameSettingsGlobals.snoSelectedQuest;
        int32 nInitialQuestStepUID = sgtGameSettingsGlobals.nSelectedQuestStep;
        Act eAct = sgtGameSettingsGlobals.eSelectedCampaignAct;
        if (bMatchMakingMode) {
            switch (sgtGameSettingsGlobals.nSelectedQuickMatchOption) {
            case 0:
                snoInitialQuest = SNO_NONE;
                nInitialQuestStepUID = -1;
                break;
            case 2:
                eAct = ACT_INVALID;
                snoInitialQuest = SNO_NONE;
                nInitialQuestStepUID = -1;
                break;
            }
        }
        if (!sgtGameSettingsGlobals.bOverwriteWarningAccepted && !bSkipWarning && !SelectionsPreserveCampaignSave(SELECT_MODE_DIFFICULTY)) {
            ShowOverwriteWarning();
            return;
        }
        const D3::Hero::Digest & tDigest = sgtGameSettingsGlobals.tHeroDefinition.digest();
        tGameParams.m_bIsHardcore = tDigest.player_flags() & (1 << PLAYERFLAG_HARDCORE);
        tGameParams.unk_14 = sub_710072e0b0(tDigest.season_created(), tDigest.player_flags2());
        tGameParams.m_eGameType = 1;
        tGameParams.m_bResumeFromSave = IsResumeFromSave(eAct, snoInitialQuest, nInitialQuestStepUID);
        tGameParams.m_eAct = eAct;
        tGameParams.m_snoQuest = snoInitialQuest;
        tGameParams.m_nQuestStep = nInitialQuestStepUID;
        tGameParams.m_nHandicap = nHandicap;
    } else if (sgtGameSettingsGlobals.eSelectedMode == CHALLENGE_MODE) {
        tGameParams.m_eGameType = 6;
        tGameParams.m_tWeeklyChallengeConfig = sgtGameSettingsGlobals.tChallengeData;
        tGameParams.m_tWeeklyChallengeData = sgtGameSettingsGlobals.tWeeklyChallengeData;
    } else {
        const D3::Hero::Digest & tDigest = sgtGameSettingsGlobals.tHeroDefinition.digest();
        tGameParams.m_bIsHardcore = tDigest.player_flags() & (1 << PLAYERFLAG_HARDCORE);
        tGameParams.unk_14 = sub_710072e0b0(tDigest.season_created(), tDigest.player_flags2());
        tGameParams.m_eGameType = 1;
        tGameParams.m_bResumeFromSave = FALSE;
        tGameParams.m_eAct = ACT_OPEN_WORLD;
        tGameParams.m_snoQuest = SNO_NONE;
        tGameParams.m_ePreferredAdventureAct = sgtGameSettingsGlobals.eSelectedAdventureAct;
        tGameParams.m_nQuestStep = -1;
        tGameParams.m_nHandicap = nHandicap;
    }
    if (bMatchMakingMode || bShowTags)
        tGameParams.m_nTag = sgtGameSettingsGlobals.nSelectedQuickMatchTag;
    else if (sgtGameSettingsGlobals.tMatchmakingTagsDataSource.GetTagIndex(tGameParams.m_nTag) == -1)
        tGameParams.m_nTag = sgtGameSettingsGlobals.tMatchmakingTagsDataSource.m_arGameTags[0].m_tagId;
    if (!bMatchMakingMode && !bShowTags)
        sgtGameSettingsGlobals.bGameSettingsViewed = TRUE;
    if (bCloseScreen) {
        cbCopy = sgtGameSettingsGlobals.cbScreenClosed;
        Close();
    }
    if (::Console::Online::PartyIsHost())
        ::Console::Online::PartySetGameParams(tGameParams);
    if (bCloseScreen && cbCopy)
        cbCopy(0);
}
} } }

namespace UIGameSettings { namespace Console { namespace {
// Original line 3338.
SNO GetFirstQuestForAct(Act eAct)
{
    SNO snoAct = ActSNOFromActEnum(eAct);
    ScopedActDefinitionLockR ptActDef(snoAct);
    if (ActGetNumQuests(ptActDef) > 0)
        return ptActDef->arQuests[0].snoQuest;
    return SNO_NONE;
}

// Original line 3351.
BOOL SelectionsPreserveCampaignSave(GameSettingsMode eCurrentMode)
{
    ::Console::Online::GameParams tGameParams;
    if (::Console::Online::PartyGetGameParams(tGameParams)) {
        BOOL bActCheck;
        BOOL bQuestCheck;
        if (tGameParams.m_eAct == ACT_OPEN_WORLD) {
            Act eLastPlayedAct = sgtGameSettingsGlobals.tHeroDefinition.digest().last_played_act();
            SNO snoLastPlayedQuest = sgtGameSettingsGlobals.tHeroDefinition.digest().last_played_quest();
            int32 nLastPlayedQuestStep = sgtGameSettingsGlobals.tHeroDefinition.digest().last_played_quest_step();
            if (eLastPlayedAct == ACT_INVALID)
                eLastPlayedAct = ACT_1;
            if (snoLastPlayedQuest == SNO_NONE)
                snoLastPlayedQuest = GetFirstQuestForAct(eLastPlayedAct);
            bActCheck = sgtGameSettingsGlobals.eSelectedCampaignAct == eLastPlayedAct;
            bQuestCheck = eCurrentMode < SELECT_MODE_QUEST ||
                (sgtGameSettingsGlobals.snoSelectedQuest == snoLastPlayedQuest && sgtGameSettingsGlobals.nSelectedQuestStep == nLastPlayedQuestStep);
        } else {
            bActCheck = sgtGameSettingsGlobals.eSelectedCampaignAct == tGameParams.m_eAct;
            bQuestCheck = eCurrentMode < SELECT_MODE_QUEST ||
                (sgtGameSettingsGlobals.snoSelectedQuest == tGameParams.m_snoQuest && sgtGameSettingsGlobals.nSelectedQuestStep == tGameParams.m_nQuestStep);
        }
        return bActCheck && bQuestCheck;
    }
    return FALSE;
}
} } }

namespace UIGameSettings { namespace Console { namespace {
// Original line 2165.
void FallbackSelectedChoices()
{
    if (sgtGameSettingsGlobals.eSelectedMode == CAMPAIGN_MODE) {
        BOOL bFilterByQuestCompletion = sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION);
        if (sgtGameSettingsGlobals.eSelectedCampaignAct != ACT_INVALID && bFilterByQuestCompletion &&
            !GetActUnlocked(sgtGameSettingsGlobals.eSelectedCampaignAct))
            sgtGameSettingsGlobals.eSelectedCampaignAct = ACT_1;
        if (sgtGameSettingsGlobals.eSelectedCampaignAct != ACT_INVALID && sgtGameSettingsGlobals.snoSelectedQuest != SNO_NONE) {
            OnlineService::QuestId tQuest;
            tQuest.snoQuest = sgtGameSettingsGlobals.snoSelectedQuest;
            ScopedQuestDefinitionLockR pQuest(sgtGameSettingsGlobals.snoSelectedQuest);
            int32 nHighestQuestStep = sgtGameSettingsGlobals.ptDigest->GetHighestProgressedStep(tQuest);
            if ((bFilterByQuestCompletion && nHighestQuestStep == -2) ||
                (pQuest && pQuest->eAct != sgtGameSettingsGlobals.eSelectedCampaignAct)) {
                SNO snoAct = ActSNOFromActEnum(sgtGameSettingsGlobals.eSelectedCampaignAct);
                ScopedActDefinitionLockR ptDefAct(snoAct);
                sgtGameSettingsGlobals.snoSelectedQuest = ptDefAct->arQuests[0].snoQuest;
                sgtGameSettingsGlobals.nSelectedQuestStep = -1;
            } else if (bFilterByQuestCompletion && nHighestQuestStep < sgtGameSettingsGlobals.nSelectedQuestStep)
                sgtGameSettingsGlobals.nSelectedQuestStep = -1;
        }
    }
}
} } }

namespace UIGameSettings { namespace Console { namespace {
// Original line 2907.
void RefreshResetWarning(const UIID & id)
{
    UIID idAcceptText;
    UIID idAcceptButton;
    idAcceptText = UIIDCreateFromParent(id, ".AcceptText");
    idAcceptButton = UIIDCreateFromParent(id, ".Button3");
    UIWindow & tAcceptButton = UI_GetWindowFromID(idAcceptButton);
    UIText & tAcceptText = UI_GetTextFromID(idAcceptText);
    int32 nMillisecondsPassed = Blizzard::Time::Milliseconds() - sgtGameSettingsGlobals.nQuestResetOpenTime;
    if (HasResetCooldownPassed()) {
        tAcceptButton.SetTransparency(255);
        tAcceptText.SetTransparency(255);
        tAcceptText.SetText(StringListFetch("Console:HardcoreAccept"), FALSE);
    } else {
        tAcceptButton.SetTransparency(64);
        tAcceptText.SetTransparency(64);
        CRefString sAcceptText;
        sAcceptText.Format("%s (%s)", StringListFetch("Console:HardcoreAccept").str(),
            UnicodeText::FormatNumber((RESET_QUEST_COOLDOWN_MILLISECS - nMillisecondsPassed) / 1000 + 1, TRUE, FNS_NORMAL, 0).str());
        tAcceptText.SetText(sAcceptText, FALSE);
    }
}
} } }

namespace UIGameSettings { namespace Console { namespace {
#line 2364
void ResetQuestSelections()

{
    ::Console::Online::GameParams tGameParams;
    if (::Console::Online::PartyGetGameParams(tGameParams))
    {
        Act eDefaultAct = ACT_1;

        if (tGameParams.m_eAct == ACT_OPEN_WORLD)
        {
            sgtGameSettingsGlobals.eSelectedCampaignAct = sgtGameSettingsGlobals.tHeroDefinition.digest().last_played_act();
            sgtGameSettingsGlobals.snoSelectedQuest = sgtGameSettingsGlobals.tHeroDefinition.digest().last_played_quest();
            sgtGameSettingsGlobals.nSelectedQuestStep = sgtGameSettingsGlobals.tHeroDefinition.digest().last_played_quest_step();
        }
        // The quest and act independently fall back to the saved values at original lines 2381 and 2382.
        else
        {
            sgtGameSettingsGlobals.eSelectedCampaignAct = tGameParams.m_eAct == ACT_INVALID ? sgtGameSettingsGlobals.tHeroDefinition.digest().last_played_act() : tGameParams.m_eAct;
            sgtGameSettingsGlobals.snoSelectedQuest = tGameParams.m_snoQuest == SNO_NONE ? sgtGameSettingsGlobals.tHeroDefinition.digest().last_played_quest() : tGameParams.m_snoQuest;
            sgtGameSettingsGlobals.nSelectedQuestStep = tGameParams.m_nQuestStep;
        }
        if (sgtGameSettingsGlobals.eSelectedCampaignAct < ACT_1 || sgtGameSettingsGlobals.eSelectedCampaignAct > ACT_MAX_CAMPAIGN)
        {
            sgtGameSettingsGlobals.eSelectedCampaignAct = eDefaultAct;
        }
        if (sgtGameSettingsGlobals.snoSelectedQuest == SNO_NONE)
        {
            sgtGameSettingsGlobals.snoSelectedQuest = GetFirstQuestForAct(sgtGameSettingsGlobals.eSelectedCampaignAct);
        }
        if (tGameParams.m_ePreferredAdventureAct != ACT_INVALID)
        {
            sgtGameSettingsGlobals.eSelectedAdventureAct = tGameParams.m_ePreferredAdventureAct;
        }
    }
}
#line 4145
} } }

namespace UIGameSettings { namespace Console { namespace {
// Original line 2876.
void RefreshSelectionDisplay()
{
    UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_MODE_GROUP).SetSelection(sgtGameSettingsGlobals.eSelectedMode, FALSE);
    Act eSelectedAct = sgtGameSettingsGlobals.eSelectedMode == CAMPAIGN_MODE ? sgtGameSettingsGlobals.eSelectedCampaignAct : sgtGameSettingsGlobals.eSelectedAdventureAct;
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_ACT_SELECT_LIST).SetSelectedIndex(eSelectedAct / 100);
    int32 nQuestIndex = sgtGameSettingsGlobals.tQuestDataSource.GetQuestIndex(sgtGameSettingsGlobals.snoSelectedQuest);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_LIST).SetSelectedIndex(nQuestIndex == -1 ? 0 : nQuestIndex);
    int32 nQuestStepIndex = sgtGameSettingsGlobals.tQuestStepDataSource.GetQuestStepIndex(sgtGameSettingsGlobals.nSelectedQuestStep);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_STEP_LIST).SetSelectedIndex(nQuestStepIndex == -1 ? 0 : nQuestStepIndex);
    UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT).SetSelection(sgtGameSettingsGlobals.nSelectedDifficulty, FALSE);
    UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_QUICK_MATCH).SetSelection(sgtGameSettingsGlobals.nSelectedQuickMatchOption, FALSE);
    int32 nTagIndex = sgtGameSettingsGlobals.tMatchmakingTagsDataSource.GetTagIndex(sgtGameSettingsGlobals.nSelectedQuickMatchTag);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUICK_MATCH_TAGS).SetSelectedIndex(nTagIndex == -1 ? 0 : nTagIndex);
}
} } }

namespace UIGameSettings { namespace Console {
// Original line 1213.
void OnBack(const UIID & id, const int32 nControllerIndex)
{
    BOOL bMatchMakingMode = sgtGameSettingsGlobals.dwFlags & (1 << MATCHMAKING_MODE);
    BOOL bShowTags = sgtGameSettingsGlobals.dwFlags & (1 << SHOW_TAGS);
    BOOL bDifficultySelect = sgtGameSettingsGlobals.dwFlags & (1 << DIFFICULTY_SELECT);
    if (UI_GetControllerFocus() == UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_STEP_LIST) {
        UI_SetControllerFocus(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_LIST);
        RefreshQuestDisplay();
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("ConsoleGameSettings:SelectQuest"));
        ::Console::Screens::UpdateButtonLegendText();
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_GAMESETTINGS_BACK), cgtPSODefaults);
    } else if (sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_CHALLENGE) {
        ChangeSelectionMode(SELECT_MODE_PLAYMODE);
    } else if (!bDifficultySelect && !bShowTags && sgtGameSettingsGlobals.eCurrentMode != SELECT_MODE_PLAYMODE) {
        GameSettingsMode ePreviousMode = sgtGameSettingsGlobals.eCurrentMode;
        OnLeftShoulder(FALSE);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_GAMESETTINGS_BACK), cgtPSODefaults);
        if (!bMatchMakingMode && !bShowTags && !bDifficultySelect && ePreviousMode == SELECT_MODE_DIFFICULTY &&
            sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_QUEST) {
            UI_SetControllerFocus(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_STEP_LIST);
            RefreshQuestDisplay();
            ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("ConsoleGameSettings:SelectQuestStep"));
            ::Console::Screens::UpdateButtonLegendText();
        }
    } else {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
        if (sgtGameSettingsGlobals.cbScreenClosed)
            sgtGameSettingsGlobals.cbScreenClosed(1);
        Close();
    }
}
}
}
namespace UIGameSettings { namespace Console { namespace {
// NON_MATCHING: 548/552 bytes, 93 instructions differ in loop branch layout and register allocation.
// The matchmaking and ordinary paths use the list and mode comparisons at original lines 3592 and 3596.
#line 3576
void OnLeftShoulder(
    BOOL bPlaySound)
{
    if (sgtGameSettingsGlobals.dwFlags & ((1 << DIFFICULTY_SELECT) | (1 << SHOW_TAGS)))
    {
        return;
    }

    BOOL bMatchMakingMode = (sgtGameSettingsGlobals.dwFlags & (1 << MATCHMAKING_MODE)) != 0;
    BOOL bValidMode = FALSE;
    while (!bValidMode)
    {
        sgtGameSettingsGlobals.eCurrentMode = (GameSettingsMode)Math::PinWrap((int32)sgtGameSettingsGlobals.eCurrentMode - 1, 0, (int32)NUM_SELECT_MODES);
        BOOL bIsChallengeMode = sgtGameSettingsGlobals.eSelectedMode == CHALLENGE_MODE || sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_CHALLENGE;
        if (bMatchMakingMode)
        {
            bValidMode = !bIsChallengeMode && (sgtGameSettingsGlobals.eSelectedMode != ADVENTURE_MODE || sgtGameSettingsGlobals.eCurrentMode != SELECT_MODE_ACT);
        }
        else
        {
            bValidMode = !bIsChallengeMode && (sgtGameSettingsGlobals.eSelectedMode != ADVENTURE_MODE || sgtGameSettingsGlobals.eCurrentMode != SELECT_MODE_QUEST);
        }
    }
    ChangeSelectionMode(sgtGameSettingsGlobals.eCurrentMode);
    if (bPlaySound)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_GAMESETTINGS_SHOULDER), cgtPSODefaults);
    }
}
#line 4223
} } }

extern UIID UITEXT_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_NAME;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ICON1;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ARROWL;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ARROWR;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_RING;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECTED_RING;
CRefString QuestsGetUIName(SNO snoQuest);  // X/Common/Quests/Quests.cpp
void QuestGetIcon(const Act eAct, const SNO snoQuest, const BOOL fCompleted, ImageTextureFrame & tFrame);  // X/Common/Definitions/QuestDefinition.cpp

namespace UIGameSettings { namespace Console { namespace {
// Original line 2691.
void UpdateQuestDescriptions()
{
    SNO snoQuest = GetSelectedQuest();
    CRefString sQuestName;
    ImageTextureFrame tQuestIcon;
    if (SNOFileExists(QUEST_SNO, snoQuest)) {
        sQuestName = QuestsGetUIName(snoQuest);
        QuestGetIcon(GetSelectedAct(), snoQuest, TRUE, tQuestIcon);
    }
    UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_NAME).SetText(sQuestName, FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ICON1).SetTextureFrame(tQuestIcon);
}

// Original line 2664.
void RefreshQuestDisplay()
{
    BOOL bMatchmakingMode = sgtGameSettingsGlobals.dwFlags & (1 << MATCHMAKING_MODE);
    BOOL bShowTags = sgtGameSettingsGlobals.dwFlags & (1 << SHOW_TAGS);
    if (!bMatchmakingMode && !bShowTags) {
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_STEP_LIST).RefreshVisibleItems();
        UpdateQuestDescriptions();
        UpdateQuestStepDescriptions();
        UIConsoleListBox & tQuestList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_LIST);
        tQuestList.RefreshVisibleItems();
        int32 nSelectedIndex = tQuestList.GetSelectedIndex();
        BOOL bQuestListFocused = UI_GetControllerFocus() == UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_LIST;
        int32 nPreviousIndex = tQuestList.GetPrevSelectableIndex();
        int32 nNextIndex = tQuestList.GetNextSelectableIndex();
        UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ARROWL).SetActive(bQuestListFocused && nSelectedIndex != nPreviousIndex);
        UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ARROWR).SetActive(bQuestListFocused && nSelectedIndex != nNextIndex &&
            sgtGameSettingsGlobals.tQuestDataSource.IsItemEnabled(tQuestList, nNextIndex));
        UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_RING).SetActive(bQuestListFocused);
        UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECTED_RING).SetActive(!bQuestListFocused);
    } else
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUICK_MATCH_TAGS).RefreshVisibleItems();
}
} } }

extern LPCSTR gszQuestStepBnetTextLabel;
extern LPCSTR gszUnassignedQuestStepBnetTextLabel;
extern LPCSTR gszQuestDescLabel;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_DESC;
extern UIID UISCROLL_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_DESC_SCROLL;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_WRAPPER;
UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp

namespace UIGameSettings { namespace Console { namespace {
// Original line 3449.
CRefString GetQuestStepDescription(SNO snoQuest, int32 nQuestStep)
{
    CRefString sFullDescription;
    ScopedQuestDefinitionLockR pQuest(snoQuest);
    if (pQuest) {
        SNO snoStringList = QuestGetStringList(snoQuest);
        CRefString sStepDescription;
        if (nQuestStep == -1)
            sStepDescription = StringListFetch(snoStringList, gszUnassignedQuestStepBnetTextLabel);
        else {
            CRefString sDescriptionLabel;
            sDescriptionLabel.Format(gszQuestStepBnetTextLabel, nQuestStep);
            sStepDescription = StringListFetch(snoStringList, sDescriptionLabel);
        }
        CRefString sQuestDescription = StringListFetch(snoStringList, gszQuestDescLabel);
        sFullDescription.Format("%s\n\n%s", sStepDescription.str(), sQuestDescription.str());
    }
    return sFullDescription;
}

// Original line 2707.
void UpdateQuestStepDescriptions()
{
    SNO snoQuest = GetSelectedQuest();
    int32 nQuestStep = GetSelectedQuestStep();
    if (SNOFileExists(QUEST_SNO, snoQuest)) {
        CRefString sFullDescription = GetQuestStepDescription(snoQuest, nQuestStep);
        UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_DESC).SetText(sFullDescription, FALSE);
        UIConsoleScrollBox & tScrollBox = UI_GetConsoleScrollBoxFromID(UISCROLL_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_DESC_SCROLL);
        Rect2D rectScrollBox = tScrollBox.GetWindowRect();
        Rect2D rectQuestContent = UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_WRAPPER).GetWindowRect();
        rectScrollBox.bottom = rectQuestContent.bottom;
        tScrollBox.SetWindowRect(rectScrollBox);
        tScrollBox.SetScrollPos(0.0f);
        ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
        if (tScrollBox.GetScrollLimit() > 0.0f && sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_QUEST) {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, 2, StringListFetch("ConsoleGameSettings:ScrollQuest"));
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, 2, StringListFetch("ConsoleGameSettings:ScrollQuest"));
        } else {
            tActionManager.ClearAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER);
            tActionManager.ClearAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER);
        }
    }
}
} } }

namespace UIGameSettings { namespace Console { namespace {
// Original line 1734.
void UpdateStepListForSelectedQuest(SNO snoQuest)
{
    int32 nSelectedIndex = 0;
    sgtGameSettingsGlobals.tQuestStepDataSource.m_arQuestStepUIDs.clear();
    if (snoQuest == SNO_NONE)
        return;
    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    OnlineService::QuestId tQuest;
    tQuest.snoQuest = snoQuest;
    if (pdefQuest->tQuestUnassignedStep.dwFlags & (1 << QSF_RESUMABLE_STEP))
        sgtGameSettingsGlobals.tQuestStepDataSource.m_arQuestStepUIDs.push_back(-1);
    int32 nHighestUnlockedStepUID = -3;
    if (sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION))
        nHighestUnlockedStepUID = sgtGameSettingsGlobals.ptDigest->GetHighestProgressedStep(tQuest);
    if (nHighestUnlockedStepUID != -2) {
        for (uint32 i = 0; i < pdefQuest->serQuestSteps.ElementCount(sizeof(QuestStep)); ++i) {
            const QuestStep & tStep = pdefQuest->arQuestSteps[i];
            if (tStep.dwFlags & (1 << QSF_SELECTABLE_STEP)) {
                if (!QuestStepIsReachableByStep(snoQuest, tStep.nUID, nHighestUnlockedStepUID))
                    break;
                if (sgtGameSettingsGlobals.snoSelectedQuest == snoQuest && tStep.nUID == sgtGameSettingsGlobals.nSelectedQuestStep)
                    nSelectedIndex = sgtGameSettingsGlobals.tQuestStepDataSource.m_arQuestStepUIDs.size();
                sgtGameSettingsGlobals.tQuestStepDataSource.m_arQuestStepUIDs.push_back(tStep.nUID);
            }
        }
    }
    UIConsoleListBox & tStepList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_STEP_LIST);
    tStepList.RefreshVisibleItems();
    tStepList.SetSelectedIndex(nSelectedIndex);
}
} } }

#include "../../Battlenet/UI/Util/UIUtil.h"
namespace UIGameSettings { namespace Console { namespace {
#line 1605 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
BOOL IsActSelectable(
    int32 nIndex)
{
    if (!(sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION)))
        return TRUE;
    return GetActUnlocked(arActList[nIndex]) != FALSE;
}
#line 1708 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
ImageTextureFrame GetActIcon(
    int32 nIndex)
{
    CRefString sIconKey;
    sIconKey.Format("UI:Console_GameSettings_Act_%dIcon", nIndex + 1);
    return ImageTextureFrame(sIconKey);
}


ImageTextureFrame GetActBossIcon(
    int32 nIndex,
    BOOL bLocked)
{
    CRefString sIconKey;
    if (bLocked)
    {
        sIconKey.Format("UI:Console_GameSettings_Act_%dBossHeadLocked", nIndex + 1);
    }
    else
    {
        sIconKey.Format("UI:Console_GameSettings_Act_%dBossHead", nIndex + 1);
    }
    return ImageTextureFrame(sIconKey);
}
}

#line 611 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
void ActDataSource::UpdateItemUI(
    const UIConsoleListBox & tListBox,
    const UIID & idItemUI,
    const int32 nIndex)
{
    BOOL bMatchMakingMode = sgtGameSettingsGlobals.dwFlags & (1 << MATCHMAKING_MODE);
    BOOL bShowTags = (sgtGameSettingsGlobals.dwFlags >> SHOW_TAGS) & 1;
    PlayMode eSelectedPlayMode = GetSelectedPlayMode();
    BOOL bSelected = tListBox.GetSelectedIndex() == nIndex;
    BOOL bActUnlocked = eSelectedPlayMode == ADVENTURE_MODE || IsActSelectable(nIndex);
    Act eAct = arActList[nIndex];

    UIID idCursor, idActLocked, idActBacklight, idActName, idActIcon, idActBossHeadActive, idActBossHeadLocked;
    idCursor = UIIDCreateFromParent(idItemUI, ".Cursor");
    idActLocked = UIIDCreateFromParent(idItemUI, ".ListItemActLocked");
    idActBacklight = UIIDCreateFromParent(idItemUI, ".ActPersistBacklight");
    idActName = UIIDCreateFromParent(idItemUI, ".Name");
    idActIcon = UIIDCreateFromParent(idItemUI, ".Icon");
    idActBossHeadActive = UIIDCreateFromParent(idItemUI, ".ActDemonHeadActive");
    idActBossHeadLocked = UIIDCreateFromParent(idItemUI, ".ActDemonHeadLocked");

    UI_GetWindowFromID(idActIcon).SetTextureFrame(GetActIcon(nIndex));
    UI_GetWindowFromID(idActLocked).SetActive(!bActUnlocked);
    UI_GetWindowFromID(idActBossHeadActive).SetTextureFrame(GetActBossIcon(nIndex, FALSE));
    UI_GetWindowFromID(idActBossHeadActive).SetActive(bActUnlocked);
    UI_GetWindowFromID(idActBossHeadLocked).SetTextureFrame(GetActBossIcon(nIndex, TRUE));
    UI_GetWindowFromID(idActBossHeadLocked).SetActive(!bActUnlocked);
    UI_GetWindowFromID(idCursor).SetActive(bSelected && sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_ACT);
    UI_GetWindowFromID(idActBacklight).SetActive(bSelected && sgtGameSettingsGlobals.eCurrentMode != SELECT_MODE_ACT && !bMatchMakingMode && !(bShowTags && eSelectedPlayMode == ADVENTURE_MODE));
    UI_GetTextFromID(idActName).SetText(DisplayActName(eAct), FALSE);

    UIWindow & tMainCursor = UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_CURSOR);

    if (bSelected)
    {
        UIWindow & tListItem = UI_GetWindowFromID(idItemUI);
        Rect2D rectItem, rectCursor;
        tListItem.GetWindowRect(rectItem);
        tMainCursor.GetWindowRect(rectCursor);
        Vector2D vecFinalTL;
        AlignRect(
            Vector2D(rectItem.left, rectItem.top),
            Vector2D(rectItem.Width(), rectItem.Height()),
            ALIGN_CENTER_VERTICAL,
            rectCursor.left,
            Vector2D(rectCursor.Width(), rectCursor.Height()), vecFinalTL);

        tMainCursor.MoveTo(vecFinalTL);

        UIWindow & tTopBlackout = UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_TOP_BLACKOUT);
        UIWindow & tBottomBlackout = UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_BOTTOM_BLACKOUT);
        Rect2D rectTopBlackout, rectBottomBlackout;
        tTopBlackout.GetWindowRect(rectTopBlackout);
        tBottomBlackout.GetWindowRect(rectBottomBlackout);
        if ((bMatchMakingMode || bShowTags) && eSelectedPlayMode == ADVENTURE_MODE)
        {
            rectTopBlackout.bottom = rectBottomBlackout.bottom;
            rectBottomBlackout.top = rectBottomBlackout.bottom;
        }
        else
        {
            rectTopBlackout.bottom = rectItem.top;
            rectBottomBlackout.top = rectItem.bottom;
        }
        tTopBlackout.SetWindowRect(rectTopBlackout);
        tBottomBlackout.SetWindowRect(rectBottomBlackout);
    }

}
}
}
#include "../../../Common/Attributes/FastAttribConstants.h"
#include <stdlib.h>
namespace Console { namespace UIOnlineActions { BOOL IsDifficultyLocked(const D3::Account::Digest & tAccountDigest, int32 nLevel, BOOL bIsHardcore, int32 nDifficulty); } } // Only OnlineActions.cpp:1560 recovered; no header declaration record.
namespace UIGameSettings { namespace Console { namespace {
#line 1616 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
static bool sDoesHostQualifyForHigherTorment(
    int32 nPrimaryProfile)
{
    ::Console::Online::PartyMember tPartyMembers[4];
    uint32 nNumPartyMembers = ::Console::Online::PartyGetMembers(tPartyMembers, TRUE, FALSE);
    for (uint32 i = 0; i < nNumPartyMembers; ++i)
    {
        if (tPartyMembers[i].nControllerIndex == nPrimaryProfile)
        {

            return tPartyMembers[i].tHeroDigest && tPartyMembers[i].tHeroDigest->level() >= HERO_LEVEL_MAX;
        }
    }

    return false;
}

#line 1634 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
BOOL IsDifficultyLocked(
    int32 nIndex,
    int32 nPrimaryProfile)
{

    if (sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION))
    {







        if (nPrimaryProfile == -1)
        {
            nPrimaryProfile = UI_GetPrimaryUIController();

            if (nPrimaryProfile == -1)
                return FALSE; }
        if (nIndex >= HC_TORMENT_7 && !sDoesHostQualifyForHigherTorment(nPrimaryProfile))
            return TRUE;



        BOOL bDifficultyIsLocked = ::Console::UIOnlineActions::IsDifficultyLocked(
            ::Console::GamerProfile::GetAccount(nPrimaryProfile).digest(),
            ::Console::GamerProfile::GetHero(nPrimaryProfile).digest().level(),
            sgtGameSettingsGlobals.tHeroDefinition.digest().player_flags() & (1 << PLAYERFLAG_HARDCORE), nIndex);

        if (!GameIsStartUp())
        {




            if (::Console::Online::PartyIsHost())
            {
                ::Console::Online::PartyMember tPartyMembers[4];
                uint32 nNumPartyMembers = ::Console::Online::PartyGetMembers(tPartyMembers, TRUE, FALSE);
                for (uint32 i = 0; i < nNumPartyMembers; ++i)
                {
                    if (tPartyMembers[i].tHeroDigest &&
                        tPartyMembers[i].tAccountDigest &&
                        ::Console::UIOnlineActions::IsDifficultyLocked(
                            *tPartyMembers[i].tAccountDigest,
                            tPartyMembers[i].tHeroDigest->level(),
                            tPartyMembers[i].tHeroDigest->player_flags() & (1 << PLAYERFLAG_HARDCORE), nIndex))
                    {
                        return TRUE;
                    }
                }

            }

            BOOL bIsSetDungeonActive = GameGetActiveGameCommonData().tGameSyncedData.bSetDungeonActive;
            if (bIsSetDungeonActive || GameGetActiveGameCommonData().tGameSyncedData.unk_4c)
                bDifficultyIsLocked = TRUE;



            else if (GameIsSeasonal() && nIndex > GameGetHandicapLevel())
                bDifficultyIsLocked = TRUE;


            else
                bDifficultyIsLocked = abs(sgtGameSettingsGlobals.nOriginalInGameDifficulty - nIndex) > MAX_RAISE_DIFFICULTY_HANDICAP || bDifficultyIsLocked;
        }
        return bDifficultyIsLocked;
    }
    return FALSE;
}
} } }

#include "../../../Sigma/LanguageRules.h"
// UIID declarations from X/Client/UserInterface/UIIDs.h.
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_ACT_SELECT_DIFFICULTY_NAME;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_ACT_SELECT_DESCRIPTION;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_QUEST;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_ADVENTURE;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_ACT_SELECT_QUEST_NAME;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_QUEST_ICON;
namespace UIGameSettings { namespace Console { namespace {
#line 2594 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
void UpdateActDescriptions()

{
    BOOL bMatchMakingMode = sgtGameSettingsGlobals.dwFlags & (1 << MATCHMAKING_MODE);
    BOOL bShowTags = sgtGameSettingsGlobals.dwFlags & (1 << SHOW_TAGS);
    CRefString sDifficulty = DisplayHandicap(sgtGameSettingsGlobals.nSelectedDifficulty);
    UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_ACT_SELECT_DIFFICULTY_NAME).SetText(sDifficulty, FALSE);
    int32 nSelectedPlayMode = GetSelectedPlayMode();
    Act eSelectedAct = GetSelectedAct();
    if ((bMatchMakingMode || bShowTags) && sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_QUEST)
    {
        UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_ACT_SELECT_DESCRIPTION).SetText("", FALSE);
    }
    else if (nSelectedPlayMode == CAMPAIGN_MODE)
    {
        CRefString sStringKey;
        if (GetActUnlocked(eSelectedAct) || !(sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION)))
        {

            sStringKey.Format("ConsoleGameSettings:Act%dDescription", eSelectedAct / 100 + 1);
        }
        else
        {
            sStringKey.Format("ConsoleGameSettings:Act%dLockedDescription", eSelectedAct / 100 + 1);
        }
        CRefString sDesc = StringListFetch(sStringKey);
        UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_ACT_SELECT_DESCRIPTION).SetText(
            LanguageRules::ApplyLanguageRules(sDesc.str()), FALSE);
    }
    else if (nSelectedPlayMode == ADVENTURE_MODE)
    {
        UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_ACT_SELECT_DESCRIPTION).SetText(StringListFetch("ConsoleGameSettings:AdventureDetailsDesc"), FALSE);
    }
#line 2626
// Inference: no default action; original branch exits for modes other than campaign/adventure.
    switch (nSelectedPlayMode)
    {
    case CAMPAIGN_MODE:
        {
            UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_QUEST).SetActive(TRUE);
            UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_ADVENTURE).SetActive(FALSE);
            CRefString sQuestTitle;
            ImageTextureFrame tQuestIcon;
            if (SNOFileExists(QUEST_SNO, sgtGameSettingsGlobals.snoSelectedQuest))
            {
                SNO snoDisplayQuest = sgtGameSettingsGlobals.snoSelectedQuest;
                ScopedQuestDefinitionLockR pDefQuest(snoDisplayQuest);
                if (eSelectedAct != pDefQuest->eAct)
                {
                    snoDisplayQuest = GetFirstQuestForAct(eSelectedAct);
                }
                sQuestTitle = QuestsGetUIName(snoDisplayQuest);
                QuestGetIcon(eSelectedAct, snoDisplayQuest, TRUE, tQuestIcon);
            }
            UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_ACT_SELECT_QUEST_NAME).SetText(sQuestTitle, FALSE);
            UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_QUEST_ICON).SetTextureFrame(tQuestIcon);
        }
        break;
    case ADVENTURE_MODE:
        {
            UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_QUEST).SetActive(FALSE);
            UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_ADVENTURE).SetActive(TRUE);
            CRefString sAdventureModeTitle = StringListFetch("Console:AdventureMode");
            UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_ACT_SELECT_QUEST_NAME).SetText(sAdventureModeTitle, FALSE);
        }
        break;


    }
}
} } }

// UIID declarations from X/Client/UserInterface/UIIDs.h.
extern UIID UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_QUICK_MATCH;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_BOTTOM_BLACKOUT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_TOP_BLACKOUT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_ANY_ACT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_ANY_QUEST;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_ANY_QUEST_BOSS;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_BOTTOM_BLACKOUT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_CURRENT_QUEST;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_TOP_BLACKOUT;
namespace UIGameSettings { namespace Console { namespace {
#line 2504 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
void RefreshActDisplay()

{
    UpdateActDescriptions();
    BOOL bMatchMakingMode = sgtGameSettingsGlobals.dwFlags & (1 << MATCHMAKING_MODE);
    UIConsoleListBox & tActList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SETTINGS_ACT_SELECT_LIST);
    BOOL bListActive = (!bMatchMakingMode || GetSelectedPlayMode() == ADVENTURE_MODE) && sgtGameSettingsGlobals.eCurrentMode != SELECT_MODE_DIFFICULTY;
    tActList.SetActive(bListActive);

    UIWindow & tActTopBlackout = UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_TOP_BLACKOUT);
    UIWindow & tActBottomBlackout = UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_BOTTOM_BLACKOUT);
    UIWindow & tQuickMatchTopBlackout = UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_TOP_BLACKOUT);
    UIWindow & tQuickMatchBottomBlackout = UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_BOTTOM_BLACKOUT);
    tActTopBlackout.SetActive(sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_QUEST && (!bMatchMakingMode || GetSelectedPlayMode() == ADVENTURE_MODE));
    tActBottomBlackout.SetActive(sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_QUEST && (!bMatchMakingMode || GetSelectedPlayMode() == ADVENTURE_MODE));
    tQuickMatchTopBlackout.SetActive(bMatchMakingMode && sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_QUEST && GetSelectedPlayMode() == CAMPAIGN_MODE);
    tQuickMatchBottomBlackout.SetActive(bMatchMakingMode && sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_QUEST && GetSelectedPlayMode() == CAMPAIGN_MODE);

    if (sgtGameSettingsGlobals.eCurrentMode != SELECT_MODE_DIFFICULTY)
    {
        UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH).SetActive(!bListActive && GetSelectedPlayMode() != CHALLENGE_MODE);
        if (bListActive)
        {
            tActList.RefreshVisibleItems();
        }
        else
        {


            ::Console::Online::GameParams tGameParams;
            if (::Console::Online::PartyGetGameParams(tGameParams))
            {
                int32 nSelectedQuickMatchOption = GetSelectedQuickMatchOption();

                {
                    UIID idTitle;
#line 2539
                    UIID idIcon;
#line 2539
                    UIID idCursor;
                    idTitle = UIIDCreateFromParent(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_ANY_QUEST, ".Text");
                    idIcon = UIIDCreateFromParent(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_ANY_QUEST, ".Icon");
                    idCursor = UIIDCreateFromParent(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_ANY_QUEST, ".Cursor");
                    CRefString sActName = DisplayActName(sgtGameSettingsGlobals.eSelectedCampaignAct);
                    ImageTextureFrame tActIcon = GetActIcon(sgtGameSettingsGlobals.eSelectedCampaignAct / 100);
                    ImageTextureFrame tBossHead = GetActBossIcon(sgtGameSettingsGlobals.eSelectedCampaignAct / 100, FALSE);
                    UI_GetTextFromID(idTitle).SetText(sActName, FALSE);
                    UI_GetWindowFromID(idIcon).SetTextureFrame(tActIcon);
                    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_ANY_QUEST_BOSS).SetTextureFrame(tBossHead);
#line 2548
// Inference: quick-match index 0 is Any Quest, from this control ID and original zero comparison.
                    UI_GetWindowFromID(idCursor).SetActive(nSelectedQuickMatchOption == 0 && sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_ACT);
                }


                {
                    UIID idTitle;
#line 2554
                    UIID idIcon;
#line 2554
                    UIID idCursor;
                    idTitle = UIIDCreateFromParent(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_CURRENT_QUEST, ".Text");
                    idIcon = UIIDCreateFromParent(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_CURRENT_QUEST, ".Icon");
                    idCursor = UIIDCreateFromParent(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_CURRENT_QUEST, ".Cursor");
                    CRefString sQuestName = QuestsGetUIName(sgtGameSettingsGlobals.snoSelectedQuest);
                    ImageTextureFrame tQuestIcon;
                    QuestGetIcon(sgtGameSettingsGlobals.eSelectedCampaignAct, sgtGameSettingsGlobals.snoSelectedQuest, TRUE, tQuestIcon);
                    UI_GetTextFromID(idTitle).SetText(sQuestName, FALSE);
                    UI_GetWindowFromID(idIcon).SetTextureFrame(tQuestIcon);
#line 2562
// Inference: quick-match index 1 is Current Quest, from this control ID and original comparison.
                    UI_GetWindowFromID(idCursor).SetActive(nSelectedQuickMatchOption == 1 && sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_ACT);
                }


                {
                    UIID idCursor;
                    idCursor = UIIDCreateFromParent(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_ANY_ACT, ".Cursor");
#line 2569
// Inference: quick-match index 2 is Any Act, from this control ID and original comparison.
                    UI_GetWindowFromID(idCursor).SetActive(nSelectedQuickMatchOption == 2 && sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_ACT);
                }

                const UIID & idSelected = UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_QUICK_MATCH).GetSelection();
                if (UI_TestID(idSelected))
                {
                    UIWindow & tSelectedItem = UI_GetWindowFromID(idSelected);
                    Rect2D rectTopBlackout;
#line 2577
                    Rect2D rectBottomBlackout;
#line 2577
                    Rect2D rectItem;
                    tSelectedItem.GetWindowRect(rectItem);
                    tQuickMatchTopBlackout.GetWindowRect(rectTopBlackout);
                    tQuickMatchBottomBlackout.GetWindowRect(rectBottomBlackout);
#line 2580
// Inference: edge assignment has no own line row; original GetWindowRect loads feed the SetWindowRect argument.
                    rectTopBlackout.bottom = rectItem.top;
#line 2581
// Inference: edge assignment has no own line row; original GetWindowRect loads feed the SetWindowRect argument.
                    rectBottomBlackout.top = rectItem.bottom;
                    tQuickMatchTopBlackout.SetWindowRect(rectTopBlackout);
                    tQuickMatchBottomBlackout.SetWindowRect(rectBottomBlackout);
                }
            }

        }
    }

}
} } }

#include "../../Battlenet/UI/Controls/ControlVisibilityManager.h"
#include "../Controls/UIRadioButtonControl.h"
// DWARF X/Common/Player.cpp:5543.
BOOL PlayerHasUnlockedChallengeMode(int32 nControllerIndex);
// DWARF X/Client/UserInterface/Console/OnlineActions.cpp:1591,1612.
namespace Console { namespace UIOnlineActions {
BOOL IsAdventureModeLocked(const D3::Account::Digest & tAccountDigest, BOOL bHardcore, BOOL bSeasonal);
bool IsActiveSeason(uint32 uSeasonNum, const D3::Account::Digest & tAccountDigest);
} }
// UIID declarations from X/Client/UserInterface/UIIDs.h.
extern UIID UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_MODE_GROUP;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_ADVENTURE;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_BOTTOM_BLACKOUT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_CAMPAIGN;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_CHALLENGE;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_TOP_BLACKOUT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV1;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV13;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV14;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV18;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV9;
namespace UIGameSettings { namespace Console { namespace {
#line 2941 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
void RefreshChallengeModeDisplay()

{
    int32 nProfileUserIdx = UI_GetPrimaryUIController();
    BOOL bChallengeLocked = nProfileUserIdx != -1 && !PlayerHasUnlockedChallengeMode(nProfileUserIdx);

    UIRadioButtonGroup & tPlayModes = UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_MODE_GROUP);

    UIRadioButton * ptButton = tPlayModes.GetButtonByValue(CHALLENGE_MODE);
    if (ptButton)
    {
        ptButton->SetEnabled(!bChallengeLocked);
    }

    UIID idChallengeDescription = UIIDCreateFromParent(UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_CHALLENGE, ".ModeDescription");
    CRefString sChallengeDescDesc;
    if (!bChallengeLocked)
    {
        sChallengeDescDesc = StringListFetch("ConsoleGameSettings:ChallengeRiftsDescString");
    }
    else
    {
        sChallengeDescDesc = StringListFetch("ConsoleGameSettings:ChallengeRiftUnlock");
    }
    UI_GetTextFromID(idChallengeDescription).SetText(sChallengeDescDesc, FALSE);
}
#line 2429 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
void RefreshPlayModeDisplay()

{
    BOOL bFilterByQuestCompletion = sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION);
    int32 nPrimaryController = UI_GetPrimaryUIController();
    BOOL bAdventureLocked = nPrimaryController != -1 &&
        ::Console::UIOnlineActions::IsAdventureModeLocked(
            ::Console::GamerProfile::GetAccount(nPrimaryController).digest(),
            sgtGameSettingsGlobals.tHeroDefinition.digest().player_flags() & (1 << PLAYERFLAG_HARDCORE),
            ::Console::UIOnlineActions::IsActiveSeason(sgtGameSettingsGlobals.tHeroDefinition.digest().season_created(), sgtGameSettingsGlobals.tAccountDefinition.digest()));

    UIRadioButtonGroup & tPlayModes = UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_MODE_GROUP);
    UIRadioButton * ptButton = tPlayModes.GetButtonByValue(ADVENTURE_MODE);
    if (ptButton)
    {
        ptButton->SetEnabled(!bFilterByQuestCompletion || !bAdventureLocked);
    }
    UIID idAdventureDescription = UIIDCreateFromParent(UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_ADVENTURE, ".ModeDescription");
    CRefString sAdventureDesc;
    if (!bFilterByQuestCompletion || !bAdventureLocked)
    {
        sAdventureDesc = StringListFetch("ConsoleGameSettings:AdventureDescString");
    }
    else
    {
        sAdventureDesc = StringListFetch("ConsoleGameSettings:AdventureModeUnlock");
    }
    UI_GetTextFromID(idAdventureDescription).SetText(sAdventureDesc, FALSE);

    UIID idCampaignCursor;
#line 2458
    UIID idAdventureCursor;
#line 2458
    UIID idChallengeCursor;
    idCampaignCursor = UIIDCreateFromParent(UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_CAMPAIGN, ".Cursor");
    idAdventureCursor = UIIDCreateFromParent(UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_ADVENTURE, ".Cursor");
    idChallengeCursor = UIIDCreateFromParent(UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_CHALLENGE, ".Cursor");

    ControlVisibilityManager tCursors;
    tCursors.AddControl(idCampaignCursor);
    tCursors.AddControl(idAdventureCursor);
    tCursors.AddControl(idChallengeCursor);
    int32 nSelectedValue = tPlayModes.GetSelectedValue();

    UIWindow & tTopBlackout = UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_TOP_BLACKOUT);
    UIWindow & tBottomBlackout = UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_BOTTOM_BLACKOUT);
    if (sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_PLAYMODE)
    {
        tTopBlackout.SetActive(FALSE);
        tBottomBlackout.SetActive(FALSE);
        tCursors.SetActive(nSelectedValue, false);
        if (!(sgtGameSettingsGlobals.dwFlags & ((1 << DIFFICULTY_SELECT) | (1 << SHOW_TAGS))))
        {

            if (sgtGameSettingsGlobals.dwFlags & (1 << MATCHMAKING_MODE))
            {
#line 2480
// Inference: original forwards the selected value unchanged to BOOL SetActive; no normalization.
                UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_NAV14).SetActive(nSelectedValue);
                UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_NAV9).SetActive(nSelectedValue == CAMPAIGN_MODE);
            }
            else
            {

#line 2486
// Inference: original forwards the selected value unchanged to BOOL SetActive; no normalization.
                UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_NAV13).SetActive(nSelectedValue);
                UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_NAV1).SetActive(nSelectedValue == CAMPAIGN_MODE);
            }
#line 2489
// Inference: merged line-0 tail activates NAV18 for the original comparison against CHALLENGE_MODE.
            UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_NAV18).SetActive(nSelectedValue == CHALLENGE_MODE);
        }

        RefreshChallengeModeDisplay();
    }
    else
    {
        tCursors.SetActive(-1, false);
        tTopBlackout.SetActive(nSelectedValue == ADVENTURE_MODE);
        tBottomBlackout.SetActive(nSelectedValue == CAMPAIGN_MODE);
    }
}
} } }

#include "../../ImageCollection.h"
// UIID declarations from X/Client/UserInterface/UIIDs.h.
extern UIID UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_DETAILS_DESC;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_DETAILS_HELP;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_DETAILS_LABEL;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_ARROWL;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_ARROWLOCKED;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_ARROWR;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_DETAILS_ICON;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_GROUP1;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_GROUP2;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_GROUP3;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_GROUP4;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_LOCK_1;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_LOCK_2;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_LOCK_3;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_LOCK_4;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_LOCK_5;
namespace UIGameSettings { namespace Console { namespace {
#line 2738 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
// NON_MATCHING: two page comparisons use signed lt instead of target unsigned lo.
void RefreshDifficultyDisplay()
{
    const int32 NUM_DIFFICULTIES_PER_PAGE = 5;

    int32 nSelectedDifficulty = GetSelectedDifficulty();


    CRefString sLabel = DisplayFormattedHandicap(nSelectedDifficulty);
    UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_DETAILS_LABEL).SetText(sLabel.str(), FALSE);


    CRefString sIcon;
    sIcon.Format("UI:ConsoleDifficulty_Icon_%d_Selected", nSelectedDifficulty);
    ImageTextureFrame tTexFrame;
    ImageCollectionGetImageInfo(sIcon, tTexFrame);
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_DETAILS_ICON).SetTextureFrame(tTexFrame);


    CRefString sHelpLabel;
#line 2757
// Inference: help-label index saturates at 5, matching the original Pin<int,int> immediates.
    sHelpLabel.Format("ConsoleGameSettings:DifficultySelector_Help_%d", Math::Pin(nSelectedDifficulty, 0, 5));
    CRefString sHelp = StringListFetch(sHelpLabel);
    UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_DETAILS_HELP).SetText(sHelp.str(), FALSE);


    CRefString sDescription;
    if (GameIsStartUp())
    {
        if (IsDifficultyLocked(nSelectedDifficulty, -1))
        {
            int32 nTempDifficulty;
            if (nSelectedDifficulty >= HC_TORMENT_7)
            {
                nTempDifficulty = HC_TORMENT_7;
            }
            else
            {
#line 2774
// Inference: locked-label index saturates at 4 (HC_TORMENT_1), matching original Pin<int,int>.
                nTempDifficulty = Math::Pin(nSelectedDifficulty, 0, 4);
            }
            sDescription.Format("ConsoleGameSettings:Difficulty_%d_Locked", nTempDifficulty);
            sDescription = StringListFetch(sDescription);
        }
        else
        {
            sDescription = DisplayHandicapDescription(nSelectedDifficulty);
        }
    }


    else if (GameIsSeasonal())
    {
        if (IsDifficultyLocked(nSelectedDifficulty, -1))
        {
            if (nSelectedDifficulty > GameGetHandicapLevel())
            {
                sDescription = StringListFetch("ConsoleGameSettings:InGameDifficultyLockedSeasonal");
            }
            else
            {
                sDescription = StringListFetch("ConsoleGameSettings:InGameDifficultyLocked");
            }
        }
        else
        {
            sDescription = DisplayHandicapDescription(nSelectedDifficulty);
        }
    }
    else
    {
#line 2806
// Inference: original2809 assignment/copy-constructor followed by2807 assignment proves the nested assignment in this conditional.
        sDescription = IsDifficultyLocked(nSelectedDifficulty, -1) ?
            StringListFetch("ConsoleGameSettings:InGameDifficultyLocked") :
            sDescription = DisplayHandicapDescription(nSelectedDifficulty);
    }

    UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_DETAILS_DESC).SetText(sDescription.str(), FALSE);

    int nCurrentPage = nSelectedDifficulty / NUM_DIFFICULTIES_PER_PAGE;
#line 2814
// 2.7.7 allows indices 0..19; the last page remains three with five entries per page.
    int nLastPage = (HC_NUM_HANDICAP_LEVELS - 1) / NUM_DIFFICULTIES_PER_PAGE;

#line 2816
// Inference: group numbers map to zero-based pages, as shown by the original division/range comparisons.
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_GROUP1).SetActive(nCurrentPage == 0);
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_GROUP2).SetActive(nCurrentPage == 1);
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_GROUP3).SetActive(nCurrentPage == 2);
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_GROUP4).SetActive(nCurrentPage == 3);

    int32 nBaseDifficulty = nCurrentPage * NUM_DIFFICULTIES_PER_PAGE;
    BOOL arLockActive[5];
    BOOL bFilterByQuestCompletion = sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION);
    int32 nPrimaryProfile = UI_GetPrimaryUIController();
    for (int32 i = 0; i < NUM_DIFFICULTIES_PER_PAGE; ++i)
    {
        int32 nDifficulty = nBaseDifficulty + i;
        arLockActive[i] = bFilterByQuestCompletion && nPrimaryProfile != -1 && nDifficulty < HC_NUM_HANDICAP_LEVELS && IsDifficultyLocked(nDifficulty, -1);
    }
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_LOCK_1).SetActive(arLockActive[0]);
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_LOCK_2).SetActive(arLockActive[1]);
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_LOCK_3).SetActive(arLockActive[2]);
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_LOCK_4).SetActive(arLockActive[3]);
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_LOCK_5).SetActive(arLockActive[4]);

    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_ARROWL).SetActive(nCurrentPage > 0);

    UIRadioButtonGroup & tDifficultyGroup = UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT);
    int32 nNumButtons = tDifficultyGroup.GetChildList().size();
    for (int32 i = 0; i < nNumButtons; ++i)
    {
        UIRadioButton * ptButton = tDifficultyGroup.GetButtonByValue(i);

        if (i < HC_NUM_HANDICAP_LEVELS)
        {
            BOOL bEnabled =
                !bFilterByQuestCompletion || nPrimaryProfile == -1 || !IsDifficultyLocked(i, -1);
            int32 nPage = i / NUM_DIFFICULTIES_PER_PAGE;

            if (i % NUM_DIFFICULTIES_PER_PAGE == 0)
            {
                BOOL bLocked = nCurrentPage == nPage - 1 && !bEnabled;
                UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_ARROWLOCKED).SetActive(static_cast<uint32>(nSelectedDifficulty) < nLastPage * NUM_DIFFICULTIES_PER_PAGE && bLocked);
                UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT_ARROWR).SetActive(static_cast<uint32>(nSelectedDifficulty) < nLastPage * NUM_DIFFICULTIES_PER_PAGE && !bLocked);
            }
            if (ptButton)
            {
                ptButton->SetShader(bEnabled ? SNO_NONE : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
                ptButton->SetEnabled(TRUE);
            }
        }
        else
        {

            if (ptButton)
            {
                ptButton->SetShader(SNO_NONE);
                ptButton->SetEnabled(FALSE);
            }
        }
    }
}
} } }

#include "../../../Sigma/ErrorManager/ErrorManager.h"
// UIID declarations from X/Client/UserInterface/UIIDs.h.
extern UIID UILIST_CONSOLE_GAME_SETTINGS_ACT_SELECT_LIST;
extern UIID UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_LIST;
extern UIID UILIST_CONSOLE_GAME_SETTINGS_QUICK_MATCH_TAGS;
extern UIID UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT;
extern UIID UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_MODE_GROUP;
extern UIID UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_QUICK_MATCH;
extern UIID UISCROLL_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_DESC_SCROLL;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_CURSOR;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_CONTENT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_LEATHER_BG;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_CHALLENGE_BLACKOUT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_FULL_BLACKOUT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_RIGHT_BLACKOUT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV1;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV10;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV11;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV12;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV13;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV14;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV15;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV16;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV18;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV2;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV3;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV4;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV5;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV6;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV7;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV8;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_NAV9;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ACT1;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ACT2;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ACT3;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ACT4;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ACT5;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ADVENTURE;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_BLACKOUT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_SELECTION_ANY_ACT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_SELECTION_CURRENT_ACT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_SELECTION_CURRENT_QUEST;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_TAGS;
namespace UIGameSettings { namespace Console { namespace {
#line 1815 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
// NON_MATCHING: 2720/2800 bytes; merged SetActive/RefreshSelectionDisplay call sites, boolean scheduling and 0x240/0x250 frame; tried mode if-chain versus switch, list operand order and reset-guard order.
void ChangeSelectionMode(
    GameSettingsMode eMode)
{
    sgtGameSettingsGlobals.eCurrentMode = eMode;
    BOOL bMatchmakingMode = (sgtGameSettingsGlobals.dwFlags & (1 << MATCHMAKING_MODE)) != 0;
    BOOL bShowTags = (sgtGameSettingsGlobals.dwFlags & (1 << SHOW_TAGS)) != 0;

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER);
    tActionManager.ClearAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER);
    tActionManager.ClearAction(::Console::CONTROLLER_INPUT_BUTTON_4);
    tActionManager.ClearAction(::Console::CONTROLLER_INPUT_BUTTON_3);
    tActionManager.ClearAction(::Console::CONTROLLER_INPUT_DPAD_HORIZONTAL);


    ControlVisibilityManager tNavHeaders;

    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV1);
    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV2);
    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV3);
    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV4);


    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV13);
    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV5);
    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV6);


    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV7);


    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV8);


    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV9);
    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV10);
    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV11);
    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV12);


    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV14);
    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV15);
    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV16);


    tNavHeaders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_NAV18);

    if (sgtGameSettingsGlobals.dwFlags & (1 << DIFFICULTY_SELECT))
    {
#line 1864
// Inference: navigation index7 is NAV7 in the ordered AddControl list; immediate7 is original.
        tNavHeaders.SetActive(7, false);
    }
    else if (bShowTags)
    {
#line 1868
// Inference: navigation index8 is NAV8 in the ordered AddControl list; immediate8 is original.
        tNavHeaders.SetActive(8, false);
    }
    else if (bMatchmakingMode)
    {
#line 1873
        switch (sgtGameSettingsGlobals.eSelectedMode)
        {
        case CAMPAIGN_MODE:
#line 1875
// Inference: campaign matchmaking headers start at index9, matching original mode-plus9.
            tNavHeaders.SetActive(eMode + 9, false);
            break;
        case CHALLENGE_MODE:
#line 1879
// Inference: index16 selects NAV18 in the original AddControl order.
            tNavHeaders.SetActive(16, false);
            break;
        default:
#line 1884
// Inference: adventure matchmaking selects index13 for play mode, otherwise mode-plus12.
            tNavHeaders.SetActive(eMode == SELECT_MODE_PLAYMODE ? 13 : eMode + 12, false);
        }
    }
    else if (sgtGameSettingsGlobals.eSelectedMode == ADVENTURE_MODE)
    {
#line 1889
// Inference: adventure headers use index6 for difficulty, otherwise mode-plus4.
        tNavHeaders.SetActive(eMode == SELECT_MODE_DIFFICULTY ? 6 : eMode + 4, false);
    }
    else if (sgtGameSettingsGlobals.eSelectedMode == CHALLENGE_MODE && sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_CHALLENGE)
    {
#line 1893
// Inference: index16 selects challenge NAV18 in the original AddControl order.
        tNavHeaders.SetActive(16, false);
    }
    else
    {
        tNavHeaders.SetActive(eMode, false);
    }

    ControlVisibilityManager tActSelectionBorders;
    tActSelectionBorders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ACT1);
    tActSelectionBorders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ACT2);
    tActSelectionBorders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ACT3);
    tActSelectionBorders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ACT4);
    tActSelectionBorders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ACT5);
    tActSelectionBorders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_ADVENTURE);
    tActSelectionBorders.SetActive(-1, false);

    ControlVisibilityManager tQuickMatchSelectionBorders;
    tQuickMatchSelectionBorders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_SELECTION_CURRENT_ACT);
    tQuickMatchSelectionBorders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_SELECTION_CURRENT_QUEST);
    tQuickMatchSelectionBorders.AddControl(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_SELECTION_ANY_ACT);
    tQuickMatchSelectionBorders.SetActive(-1, false);

    UI_GetWindowFromID(UILIST_CONSOLE_GAME_SETTINGS_ACT_SELECT_LIST).SetActive(!bMatchmakingMode && sgtGameSettingsGlobals.eCurrentMode != SELECT_MODE_DIFFICULTY);

    UI_GetControlFromID(UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT).SetActive(eMode != SELECT_MODE_DIFFICULTY);
#line 1918
// Inference: reconstructed boolean spelling follows the original masks/comparisons; no new conditions.
    UI_GetControlFromID(UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT).SetActive((eMode != SELECT_MODE_CHALLENGE && eMode != SELECT_MODE_DIFFICULTY && (bMatchmakingMode || bShowTags)) || (eMode != SELECT_MODE_QUEST && eMode != SELECT_MODE_DIFFICULTY));
    UI_GetControlFromID(UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT).SetActive(eMode == SELECT_MODE_QUEST && !bMatchmakingMode && !bShowTags);
    UI_GetControlFromID(UIWND_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT).SetActive(eMode == SELECT_MODE_DIFFICULTY);
    UI_GetControlFromID(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH).SetActive(eMode != SELECT_MODE_DIFFICULTY && eMode != SELECT_MODE_CHALLENGE && bMatchmakingMode);
    UI_GetControlFromID(UIWND_CONSOLE_GAME_SETTINGS_QUICK_MATCH_TAGS).SetActive((bMatchmakingMode || bShowTags) && eMode == SELECT_MODE_QUEST);
    UI_GetControlFromID(UIWND_CONSOLE_GAME_SETTINGS_LEATHER_BG).SetActive(eMode != SELECT_MODE_DIFFICULTY);
    UI_GetControlFromID(UIWND_CONSOLE_GAME_SETTINGS_ACT_SELECT_CURSOR).SetActive(sgtGameSettingsGlobals.eCurrentMode == SELECT_MODE_ACT && (!bMatchmakingMode || sgtGameSettingsGlobals.eSelectedMode == ADVENTURE_MODE));
    UI_GetControlFromID(UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_RIGHT_BLACKOUT).SetActive(eMode == SELECT_MODE_QUEST);
    UI_GetControlFromID(UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_FULL_BLACKOUT).SetActive(eMode == SELECT_MODE_PLAYMODE);
    UI_GetControlFromID(UIWND_CONSOLE_GAME_SETTINGS_QUEST_SELECT_BLACKOUT).SetActive(eMode == SELECT_MODE_QUEST);
    UI_GetControlFromID(UIWND_CONSOLE_GAME_SETTINGS_MODE_SELECT_CHALLENGE_BLACKOUT).SetActive(eMode == SELECT_MODE_CHALLENGE);

    UI_GetConsoleScrollBoxFromID(UISCROLL_CONSOLE_GAME_SETTINGS_QUEST_SELECT_QUEST_DESC_SCROLL).SetExternallyControlled(eMode != SELECT_MODE_QUEST);

    D3::Hero::QuestHistoryList tQuestHistory;
    tQuestHistory.mutable_quest_history()->MergeFrom(sgtGameSettingsGlobals.tHeroDefinition.digest().quest_history());
    if (eMode != SELECT_MODE_DIFFICULTY && !bMatchmakingMode && !bShowTags &&
        !(sgtGameSettingsGlobals.dwFlags & (1 << DIFFICULTY_SELECT)) &&
        (!(sgtGameSettingsGlobals.dwFlags & (1 << FILTER_BY_QUEST_COMPLETION)) || HeroCompletedAllQuests(tQuestHistory, TRUE)))
    {
#line 1938
// Inference: action slot1 is the original controller-legend position, as are the per-case literal slots below.
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, 1, StringListFetch("ConsoleGameSettings:ResetCampaign"));
    }

    RefreshSelectionDisplay();
    switch (eMode)
    {
    case SELECT_MODE_PLAYMODE:
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("ConsoleGameSettings:SelectMode"));
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("ConsoleUI:Close"));

            UI_SetControllerFocus(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_MODE_GROUP);
            RefreshPlayModeDisplay();
            RefreshActDisplay();
        }
        break;
    case SELECT_MODE_ACT:
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("ConsoleGameSettings:SelectAct"));
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("Console:Back"));
            if (bMatchmakingMode && sgtGameSettingsGlobals.eSelectedMode != ADVENTURE_MODE)
            {
                UI_SetControllerFocus(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_QUICK_MATCH);
            }
            else
            {
                UI_SetControllerFocus(UILIST_CONSOLE_GAME_SETTINGS_ACT_SELECT_LIST);
            }
            RefreshPlayModeDisplay();
            RefreshActDisplay();
            RefreshQuestDisplay();
        }
        break;
    case SELECT_MODE_QUEST:
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("Console:Back"));
            if (bMatchmakingMode)
            {
                UI_SetControllerFocus(UILIST_CONSOLE_GAME_SETTINGS_QUICK_MATCH_TAGS);
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("ConsoleGameSettings:SelectTag"));
                if (sgtGameSettingsGlobals.eSelectedMode == ADVENTURE_MODE)
                {
#line 1980
// Inference: border index5 is adventure after the five act borders in AddControl order.
                    tActSelectionBorders.SetActive(5, false);
                }
                else
                {
                    tQuickMatchSelectionBorders.SetActive(GetSelectedQuickMatchOption(), false);
                }
            }
            else
            {
                if (bShowTags)
                {
                    UI_SetControllerFocus(UILIST_CONSOLE_GAME_SETTINGS_QUICK_MATCH_TAGS);
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("ConsoleGameSettings:SelectTag"));
                }
                else
                {
                    UI_SetControllerFocus(UILIST_CONSOLE_GAME_SETTINGS_QUEST_SELECT_LIST);
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("ConsoleGameSettings:SelectQuest"));
                }
                if (sgtGameSettingsGlobals.eSelectedMode == CAMPAIGN_MODE)
                {
                    tActSelectionBorders.SetActive(sgtGameSettingsGlobals.eSelectedCampaignAct / 100, false);
                }
                else if (sgtGameSettingsGlobals.eSelectedMode == ADVENTURE_MODE)
                {





#line 2010
// Inference: showing tags selects adventure border5; otherwise original divides the adventure act by100.
                    tActSelectionBorders.SetActive(bShowTags ? 5 : sgtGameSettingsGlobals.eSelectedAdventureAct / 100, false);
                }
            }
            RefreshPlayModeDisplay();
            RefreshActDisplay();
            RefreshQuestDisplay();
        }
        break;
    case SELECT_MODE_DIFFICULTY:
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("ConsoleGameSettings:SelectDifficulty"));
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("Console:Back"));
            UI_SetControllerFocus(UIRADIOBUTTONGROUP_CONSOLE_GAME_SETTINGS_DIFFICULTY_SELECT);
            RefreshActDisplay();
            RefreshDifficultyDisplay();
        }
        break;
    case SELECT_MODE_CHALLENGE:
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_DPAD_HORIZONTAL, 0, StringListFetch("ConsoleGameSettings:ToggleLeaderboard"));
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 1, StringListFetch("ConsoleGameSettings:SelectChallengeMode"));
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 2, StringListFetch("Console:Back"));
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, 3, StringListFetch("ConsoleGameSettings:ViewLeaderboard"));
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, 4, StringListFetch("ConsoleGameSettings:InspectBuild"));
            UI_SetControllerFocus(UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_CONTENT);
        }
        break;
    default:
#line 2038
// Inference: explicit fatal-error expansion follows original2039 calls/arguments; no macro name invented.
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
    ::Console::Screens::UpdateButtonLegendText();
}
} } }

#include "../../Console/LobbyService.h"
#include "../../../Common/Items.h"
#include "../../../Server/System/SChallengeRift.h"
#include "../../../Common/ActorCommonData.h"
#include "../../RActors/RActors.h"
#include "../../../Sigma/Math/Transform.h"
// DWARF declarations from UIFunctions.cpp:3336, Lights.cpp:1104, and CActorInventory.cpp:121.
RActorID UI_CreateHeroUIRActor(SNO, SNO, const VisualEquipment &, VisualInventoryType, PlayerClass, BOOL, const PRTransform &, RenderContext, uint32, float, BOOL);
SNO UI_GetDefaultHeroLight();
LightID LightAdd(uint32, SNO, const PRTransform &, int32, RenderContext);
// DWARF cu_02db74d1 DIE0x02de78d3: X/Common/ActorInventory.h:178, typedef int32.
typedef int32 ItemDrawType;
void ACDInventoryGetItemDrawInfoWithGBID(GBID, ImageTextureFrame &, ItemDrawType, PlayerClass, BOOL);
// DWARF X/Common/Items.cpp:11319.
CRefString ItemGetPrimaryDescription(const ActorCommonData &);
// DWARF X/Client/Console/LobbyService.cpp:4447.
namespace Console { namespace Online {
void LeaderboardGetMultipleScoresForPlayer(int32, const blz::vector<LeaderboardKeyAndPartition> &, OnlineService::Callback<void (LeaderboardResult, const blz::vector<D3::Leaderboard::Score> &)>);
} }
extern UIID UIRACTOR_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_HERO;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_CHALLENGE_HERO_CLASS;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_CHALLENGE_NUMBER;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_DISABLED_TEXT;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_EXPIRATION_TIME;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_REWARD_DESCRIPTION;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_REWARD_TITLE;
extern UIID UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_TIME_TO_BEAT;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_DISABLED;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_LEADERBOARD_CONTAINER;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_REWARD;
extern UIID UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_REWARD_STATUS;
namespace UIGameSettings { namespace Console { namespace {
#line 90 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
enum { CACHE_WEEKLY_CHALLENGE_SECONDS = 60 };
void sLeaderboardGetScoresCallback(::Console::Online::LeaderboardResult, const blz::vector<D3::Leaderboard::Score> &);
#line 2981 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
void sOnGetChallengeRiftFailed(
    int32 nControllerIndex,
    const char * szReason)
{
#line 2987 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    CRefString sChallengeDescDesc = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHALLENGE_RIFT), szReason);
#line 2989 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_DISABLED).SetActive(TRUE);
    UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_DISABLED_TEXT).SetText(sChallengeDescDesc, FALSE);
#line 2992 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    sActivateChallengeRiftUIElements(FALSE);
#line 2994 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAction(::Console::CONTROLLER_INPUT_BUTTON_1);
    ::Console::Screens::UpdateButtonLegendText();
}
#line 2999 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
// NON_MATCHING: callback thunk 3304/3304 bytes; reward-check/virtual-call evaluation order differs (8 normalized instructions); tried failure switch and expiration comparison order.
void sOnGetChallengeRiftData(
    int32 nControllerIndex,
    ::Console::Online::StorageResult eResult,
    const D3::ChallengeRifts::ChallengeData & tChallengeData,
    const D3::Leaderboard::WeeklyChallengeData & tWeeklyChallengeData)
{
    sgtGameSettingsGlobals.bAskingForChallengeRiftData = FALSE;
    sgtGameSettingsGlobals.bChallengeRiftDataAvailable = FALSE;
    sgtGameSettingsGlobals.tChallengeData.Clear();
    sgtGameSettingsGlobals.tWeeklyChallengeData.Clear();
#line 3012 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    if (!IsVisible())
        return;
#line 3016 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    if (eResult != ::Console::Online::STORAGE_SUCCESS)
    {
#line 3017
        switch (eResult)
        {
        case ::Console::Online::STORAGE_FAILED_NOT_CONNECTED:
            sOnGetChallengeRiftFailed(nControllerIndex, ::Console::GamerProfile::IsSignedInOnline(nControllerIndex) ? "ErrorNotConnected" : "ErrorNotSignedInOnline");
            break;
        case ::Console::Online::STORAGE_FAILED_FILE_NOT_FOUND:
#line 3027
            sOnGetChallengeRiftFailed(nControllerIndex, "ErrorNoChallengeFound");
            break;
        default:
#line 3032
            sOnGetChallengeRiftFailed(nControllerIndex, "ErrorUnknown");
        }
        return;
    }
#line 3035 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    if (Blizzard::Time::ToUnixTime64(::Console::Online::GetNetworkTimestamp()) < tChallengeData.challenge_start_unix_time())
    {
        sOnGetChallengeRiftFailed(nControllerIndex, "ErrorChallengeNotYetStarted");
        return;
    }
#line 3040 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    // Inference: the original expiration failure falls through to display setup, unlike the preceding failures.
    if (Blizzard::Time::ToUnixTime64(::Console::Online::GetNetworkTimestamp()) >= tChallengeData.challenge_end_unix_time_console())
        sOnGetChallengeRiftFailed(nControllerIndex, "ErrorChallengeEnded");
#line 3048 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_DISABLED).SetActive(FALSE);
#line 3050 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    sActivateChallengeRiftUIElements(GetSelectedPlayMode() == CHALLENGE_MODE);
#line 3053 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    sgtGameSettingsGlobals.bChallengeRiftDataAvailable = TRUE;
    sgtGameSettingsGlobals.tChallengeData = tChallengeData;
    sgtGameSettingsGlobals.tWeeklyChallengeData = tWeeklyChallengeData;
// Inference: original cache increment is 60 billion nanoseconds, using DWARF CACHE_WEEKLY_CHALLENGE_SECONDS=60.
#line 3056
    sgtGameSettingsGlobals.uCacheWeeklyChallengeUntilTimestamp = Blizzard::Time::GetTimestamp() + int64(CACHE_WEEKLY_CHALLENGE_SECONDS) * 1000000000;
#line 3058 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    OnlineService::HeroDigest tHeroDigest;
    tHeroDigest.Import(
        OnlineService::GameAccountHandle::INVALID,
        CRefString(tWeeklyChallengeData.hero_snapshot().digest().hero_name().c_str()),
        tWeeklyChallengeData.hero_snapshot().digest(), -1);
// Inference: the original player-class validity test compares against -1.
#line 3064 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    if (tHeroDigest.m_details.m_playerClass != -1)
        UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_CHALLENGE_HERO_CLASS).SetText(tHeroDigest.m_details.GetClassName().str(), FALSE);
#line 3071 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    UIRActor & tRActor = UI_GetRActorFromID(UIRACTOR_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_HERO);
    RActor * ptRActor = RActorTryToGet(tRActor.GetRActor());
// Inference: the second player-class validity comparison also uses -1.
#line 3073
    if (!ptRActor && tHeroDigest.m_visual.IsEquipmentValid() && tHeroDigest.m_details.m_playerClass != -1)
    {
        tRActor.Cleanup();
#line 3077 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
        PlayerClass ePlayerClass = tHeroDigest.m_details.m_playerClass;
        BOOL bIsFemale = tHeroDigest.m_details.IsFemale();
#line 3080 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
        VisualEquipment tVisEquipment = tHeroDigest.m_visual.GetEquipment();
#line 3084 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
// Inference: the four SetDefaults effects cover all four VisualEquipment cosmetic slots.
#line 3084
        for (int32 i = 0; i < 4; ++i)
        {
            VisualCosmeticItem & tVisualCosmeticItem = tVisEquipment.tCosmeticItems[i];
            tVisualCosmeticItem.SetDefaults();
        }
#line 3090 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
        SNO snoActor = GetFrontendCharacterSNO(ePlayerClass, bIsFemale);
        if (snoActor == SNO_NONE)
            return;
#line 3096 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
        PRTransform tTransform = cgtPRTransformIdentity;
#line 3097 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
        // Inference: original creation arguments are visual inventory 0, animation tag 0x61000, scale 1 and final FALSE.
        RActorID idRActorNew = UI_CreateHeroUIRActor(snoActor, GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE), tVisEquipment, 0, ePlayerClass, bIsFemale, tTransform, tRActor.GetRenderContext(), 0x61000, 1.0f, FALSE);
#line 3108 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
        tRActor.SetRActor(idRActorNew);
#line 3113 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
        // Original relocation: LightAdd transform copies cgqIdentity and cgwpNull; flags and priority are zero.
        tRActor.SetLight(LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), 0, tRActor.GetRenderContext()));
    }
#line 3118 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    CRefString success = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHALLENGE_RIFT), "ChallengeNumber");
    uint32 uWeeklyChallengeNum = tChallengeData.challenge_number();
// Inference: the original FormatNumber relocation selects its int32 overload.
#line 3120
    CRefString szChallengeNumber = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHALLENGE_RIFT), "ChallengeNumber"), UnicodeText::FormatNumber(int32(uWeeklyChallengeNum), TRUE, FNS_NORMAL, FALSE).str(), NULL);
#line 3124 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_CHALLENGE_NUMBER).SetText(szChallengeNumber.str(), FALSE);
#line 3128 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    int32 nCompletionMilliseconds = tWeeklyChallengeData.rift_snapshot().completion_milliseconds();
#line 3130 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
// Inference: original sign-extends completion milliseconds before multiplying by one million nanoseconds.
#line 3130
    Blizzard::Time::Timestamp tTime = int64(nCompletionMilliseconds) * 1000000;
    ElapsedTimeDisplayOptions tTargetTimeDisplayOptions;
    tTargetTimeDisplayOptions.fAlwaysShowSeconds = TRUE;
    tTargetTimeDisplayOptions.fOnlyDisplayNumber = TRUE;
    CRefString sTargetTime = DisplayElapsedTime(Blizzard::Time::Timestamp(0), tTime, tTargetTimeDisplayOptions);
#line 3136 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    CRefString szTargetSeconds = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHALLENGE_RIFT), "ChallengeMode_TargetTime"), sTargetTime.str(), NULL);
#line 3140 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_TIME_TO_BEAT).SetText(szTargetSeconds.str(), FALSE);
#line 3144 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    UIWindow & tReward = UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_REWARD);
    tReward.SetShader(SNO_NONE);
    GBID gbidReward = ChallengeRift::GetChallengeReward().gbid;
    if (gbidReward != GBID_NONE)
    {
        D3::Items::Generator tGenerator;
        ItemGeneratorToDefaults(tGenerator);
        tGenerator.mutable_gb_handle()->set_gbid(gbidReward);
// Inference: the original CItemGenerate network-name argument is 0x201c.
#line 3152
        ACDID idACDItem = CItemGenerate(ACDNetworkName(0x201c), tGenerator, NULL);
#line 3154 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
        ImageTextureFrame tImageFrame;
// Inference: original draw-type argument is zero and gender argument negates IsFemale.
#line 3155
        ACDInventoryGetItemDrawInfoWithGBID(gbidReward, tImageFrame, 0, tHeroDigest.m_details.m_playerClass, !tHeroDigest.m_details.IsFemale());
#line 3162 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
        tReward.SetTextureFrame(tImageFrame);
#line 3164 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
        CRefString strName = ItemGetDisplayName(gbidReward);
        UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_REWARD_TITLE).SetText(strName.str(), FALSE);
#line 3167 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
        if (ptACDItem)
        {
            CRefString strDescription = ItemGetPrimaryDescription(*ptACDItem);
            UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_REWARD_DESCRIPTION).SetText(strDescription.str(), FALSE);
        }
        ACDDelete(idACDItem, FALSE);
    }
#line 3177 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    UIWindow & tCheckMarkActive = UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_REWARD_STATUS);
    tCheckMarkActive.SetActive(FALSE);
#line 3180 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(nControllerIndex);
    uint32 nLastRewardChallengeNumber = tAccountDefinition.digest().challenge_rift_account_data().last_challenge_reward_earned();
    // DWARF: challengeRiftRewardState has constant location 0 throughout its live range.
    ChallengeRiftRewardState challengeRiftRewardState = CRRS_REWARD_UNEARNED;


#line 3192 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    // Inference: only the unsigned last-earned >= current-number predicate survives in the original.
#line 3192
    tCheckMarkActive.SetActive(nLastRewardChallengeNumber >= tChallengeData.challenge_number());
#line 3195 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    Blizzard::Time::Timestamp tExpirationTime = 0;
    if (tChallengeData.has_challenge_end_unix_time_console())
    {
        tExpirationTime = Blizzard::Time::FromUnixTime64(tChallengeData.challenge_end_unix_time_console());
    }
    if (tExpirationTime > ::Console::Online::GetNetworkTimestamp())
    {
#line 3203 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
        ElapsedTimeDisplayOptions tElapsedTimeDisplayOptions;
        tElapsedTimeDisplayOptions.fAlwaysShowSeconds = TRUE;
        CRefString sExpirationTime = DisplayElapsedTime(::Console::Online::GetNetworkTimestamp(), tExpirationTime, tElapsedTimeDisplayOptions);
#line 3207 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
        CRefString sFormattedExpirationTime = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHALLENGE_RIFT), "ChallengeMode_Expiration"), sExpirationTime.str(), NULL);
        UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_EXPIRATION_TIME).SetText(sFormattedExpirationTime.str(), FALSE);
    }
    else
    {
        UI_GetTextFromID(UITEXT_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_EXPIRATION_TIME).SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHALLENGE_RIFT), "ChallengeMode_Expired").str(), FALSE);
    }
#line 3220 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    blz::vector<LeaderboardKeyAndPartition> idLeaderboards;
    LeaderboardKeyAndPartition tKey;
    tKey.m_scope = LP_CHALLENGE;
    tKey.m_partition = uWeeklyChallengeNum;
    bdOnlineUserID nUserId = ::Console::Online::LobbyService::Get(nControllerIndex)->GetOnlineUserID();
    for (int32 i = LB_CHALLENGE_RIFT_FIRST; i <= LB_CHALLENGE_RIFT_LAST; ++i)
    {
        tKey.m_id = i;
        idLeaderboards.push_back(tKey);
    }
#line 3231 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_LEADERBOARD_CONTAINER).SetActive(FALSE);
#line 3233 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
    ::Console::Online::LeaderboardGetMultipleScoresForPlayer(nControllerIndex, idLeaderboards, OnlineService::internal::function_bind_0_2<void, ::Console::Online::LeaderboardResult, const blz::vector<D3::Leaderboard::Score> &, &sLeaderboardGetScoresCallback>());
}
#line 3237 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
void sLeaderboardGetScoresCallback(
    ::Console::Online::LeaderboardResult eResult,
    const blz::vector<D3::Leaderboard::Score> & arScores)
{
    sgtGameSettingsGlobals.arPersonalBests.clear();
    if (eResult == ::Console::Online::LeaderboardResult::SUCCESS)
    {
#line 3245 "X/Client/UserInterface/Console/UIConsoleGameSettings.cpp"
        for (const D3::Leaderboard::Score * iter = arScores.begin(); iter != arScores.end(); ++iter)
        {
            const D3::Leaderboard::Score & tScore = *iter;
            // Inference: cast protobuf's uint64 leaderboard ID to the original map's LeaderboardIds key.
            if (!sgtGameSettingsGlobals.arPersonalBests.count(LeaderboardIds(tScore.leaderboard_id())))
                sgtGameSettingsGlobals.arPersonalBests[LeaderboardIds(tScore.leaderboard_id())] = tScore;
        }
    }
    UI_GetWindowFromID(UIWND_CONSOLE_GAME_SETTINGS_CHALLENGEMODE_LEADERBOARD_CONTAINER).SetActive(TRUE);
    PopulateLeaderboard(LB_CHALLENGE_RIFT_FIRST);
}
} } }
