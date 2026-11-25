// Original file: X/Client/UserInterface/Console/UIConsoleRiftEndScreen.cpp
// The console greater/challenge rift end screen: party miniatures, rewards and the friends
// leaderboard.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../../Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdLobbyServicesCommon.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "../../RActors/RActors.h"
#include "../../Console/GamerProfile.h"
// GfxVB.h only for its statics: the original's static initialiser runs them ahead of Online.h's
// nn::ldn constants.
#include "../../Gfx/GfxVB.h"
#include "../../Console/Online.h"
#include "../../Console/LobbyService.h"
#include "../../../Common/Definitions/GlobalDefinition.h"
#include "../../../Common/PlayerAccount.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/Protobuf/GameMessage.pb.h"
#include "../../../Common/Protobuf/Leaderboard.pb.h"
#include "../../../Common/Leaderboard.h"
#include "../../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Sigma/Math/Quaternion.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Common/Definitions/StringList.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
CRefString StringListFetch(LPCSTR szLabel);
CRefString LocalizedSprintf(LPCSTR szFormat, ...);
// X/Client/UserInterface/UI.cpp
UIRActor & UI_GetRActorFromID(const UIID & id);
RActorID UI_CreateHeroUIRActor(SNO snoActor, SNO snoIrradianceTex, const VisualEquipment & tEquipment, VisualInventoryType eVIT, PlayerClass ePlayerClass, BOOL fFemale, const PRTransform & tTransform, RenderContext eContext, uint32 dwAnimTag, float flScale, BOOL bUnknown);
SNO UI_GetDefaultHeroLight();
// X/Client/UserInterface/Console/UIConsoleCharacterSelect.cpp
SNO GetFrontendCharacterSNO(PlayerClass eClass, BOOL bIsFemale);
// X/Client/Lights.cpp
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);
// X/Client/Battlenet/UI/Util/UIUtil.cpp
#include "../../Battlenet/UI/Util/UIUtil.h"











CRefString DisplayElapsedTime(const Blizzard::Time::Timestamp & tStart, const Blizzard::Time::Timestamp & tEnd, const ElapsedTimeDisplayOptions & tElapsedTimeDisplayOptions);
CRefString GetPortraitImageName(const PlayerClass ePlayerClass, const BOOL fIsMale);
// X/Client/UserInterface/UIText.cpp
CRefString UIText_ValueToGameCurrency(const uint64 uAmount, CurrencyType eCurrencyType);
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils { namespace Console {
void DisplayPortrait(PlayerClass eClass, BOOL bFemale, int32 nAltLevel, const UIID & idPortrait, GBID gbidPortrait);
} }
// X/Client/UserInterface/Console/UIConsoleLeaderboards.cpp
namespace UILeaderboards { namespace Console {
void SetRiftCompletionOpenContext(int32 nParticipants, BOOL bWeeklyChallenge);
} }
// X/Common/Player.cpp
GBID PlayerGetPortraitGBIDFromParagonLevel(int32 nLevel);
BOOL PlayerIsMale(const Player & tPlayer);
// X/Common/Leaderboard.cpp
uint32 LeaderboardGetRiftScoreLevel(uint64 score);
Blizzard::Time::Timestamp LeaderboardGetRiftScoreTimeToComplete(uint64 score);
// X/Common/ActorCommonData.cpp
int32 ACDGetBloodShardCap(const ActorCommonData & tACD);
// X/Common/Items.cpp
void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const int32 eDrawType);
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console { namespace Screens {
void ShowButtonLegend(const BOOL bShow);
void OpenToScreen(int32 nScreenSet, int32 nScreen, int32 nControllerIndex, BOOL bUnused);
} }

// X/Client/Console/PresenceManager.cpp, X/Client/Console/Online.cpp
namespace Console { namespace Online {
blz::vector<unsigned long, blz::allocator<unsigned long> > PresenceGetFriendIDs(int32 nUserIndex);
typedef OnlineService::Callback<void (Console::Online::LeaderboardResult, const blz::vector<D3::Leaderboard::Score, blz::allocator<D3::Leaderboard::Score> > &)> LeaderboardGetScoresCallback;
void LeaderboardGetScoresForUsers(int32 nUserIndex, const LeaderboardKeyAndPartition & idLeaderboard, const blz::vector<unsigned long, blz::allocator<unsigned long> > & idUsers, LeaderboardGetScoresCallback pfnCallback, bool bIncludeSelf);
} }
// X/Common/OnlineService/OnlineService.cpp
namespace OnlineService {
uint32 GetSeasonNum();
}

extern UIID UIDLG_CONSOLE_RIFT_END_SCREEN;
extern UIID UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_FRIENDS_LIST;
extern UIID UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_PERSONAL_LIST;
extern UIID UIRACTOR_CONSOLE_RIFT_END_SCREEN_PLAYER1;
extern UIID UIRACTOR_CONSOLE_RIFT_END_SCREEN_PLAYER2;
extern UIID UIRACTOR_CONSOLE_RIFT_END_SCREEN_PLAYER3;
extern UIID UIRACTOR_CONSOLE_RIFT_END_SCREEN_PLAYER4;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_BLOODSHARD_CAP_INCREASE;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_ALREADY_EARNED;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_COMPLETED_TIME;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_CURRENT_TARGET_TIME;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_ORIGINAL_RUNNER_TIME;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_TIME;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_COMPLETED_LEVEL;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_COMPLETED_TEXT;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_COMPLETED_TIME;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_GOLD_REWARD;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER1_BANNER_BASE;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER1_BANNER_TRIM;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER1_NAME;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER2_BANNER_BASE;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER2_BANNER_TRIM;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER2_NAME;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER3_BANNER_BASE;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER3_BANNER_TRIM;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER3_NAME;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER4_BANNER_BASE;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER4_BANNER_TRIM;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER4_NAME;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_REWARD_CONTAINER;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_TITLE;
extern UIID UITEXT_CONSOLE_RIFT_END_SCREEN_XP_REWARD;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_BACKGROUND;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_BACKGROUND;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_REWARDCONTAINER;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_REWARD_TEXTURE;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_ITEM_REWARD_TEXTURE;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_LEADERBOARD;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_COVER;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER1;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER1_PORTRAIT;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER2;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER2_PORTRAIT;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER3;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER3_PORTRAIT;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER4;
extern UIID UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER4_PORTRAIT;

namespace UIConsoleRiftEndScreen {
UIID GetPlayerWindowUIIDByIndex(int32 nIndex);
UIID GetPlayerRActorUIIDBySlot(int32 nIndex);
UIID GetPlayerPortraitUIIDBySlot(int32 nIndex);
UIID GetPlayerNameUIIDBySlot(int32 nIndex);
UIID GetPlayerBannerBaseBySlot(int32 nIndex);
UIID GetPlayerBannerTrimBySlot(int32 nIndex);
void SetupRewardsPane();
void SetupCenterPane();
void SetupLeaderboardPane();
void Close();

class LeaderboardRiftEndDataSource : public ConsoleListBoxDataSource {  // size 0x20
public:
    LeaderboardRiftEndDataSource();
    virtual ~LeaderboardRiftEndDataSource() {}
    virtual int32 GetNumItems() const;  // vtable slot 2
    const blz::vector<D3::Leaderboard::Score, blz::allocator<D3::Leaderboard::Score> > & GetLeaderboard();
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID & idItemUI, const int32);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
    blz::vector<D3::Leaderboard::Score, blz::allocator<D3::Leaderboard::Score> > tLeaderboardScores;  // 0x08
};

inline LeaderboardRiftEndDataSource::LeaderboardRiftEndDataSource()
{
}

int32 LeaderboardRiftEndDataSource::GetNumItems() const
{
    return (int32)tLeaderboardScores.size();
}

UIControlSourceKey LeaderboardRiftEndDataSource::GetSourceKeyForItem(const int32) const
{
    return UIControlSourceKey::Parse("Console_RiftsVictoryScreen:Console_RiftsVictoryScreen.LayoutRoot.LeaderboardListListItem", -1, NULL, NULL);
}

void LeaderboardRiftEndDataSource::InitializeItemUI(const UIConsoleListBox &, const UIID & idItemUI, const int32)
{
}

void LeaderboardRiftEndDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    if ((size_t)nIndex >= tLeaderboardScores.size())
        return;

    UIID idName = UIIDCreateFromParent(idItemUI, ".EntryName");
    UIID idTier = UIIDCreateFromParent(idItemUI, ".TierNumber");
    UIID idTime = UIIDCreateFromParent(idItemUI, ".TierTime");

    D3::Leaderboard::Score tScore = tLeaderboardScores[nIndex];
    PlayerClass ePlayerClass = PlayerClassFromGBID(tScore.metadata().team_member(0).hero_gbid_class());
    BOOL fIsMale = !(tScore.metadata().team_member(0).hero_flags() & 2);

    CRefString sIcon;
    sIcon.Format("{icon:%s, 4}", GetPortraitImageName(ePlayerClass, fIsMale).str());
    CRefString sName;
    sName.Format("%s %s", sIcon.str(), tScore.metadata().team_member(0).battle_tag().c_str());
    UI_GetTextFromID(idName).SetText(sName.str(), FALSE);

    uint32 nTier = LeaderboardGetRiftScoreLevel(tScore.score());
    CRefString sTier;
    sTier = UnicodeText::FormatNumber((int32)nTier, TRUE, FNS_NORMAL, 0);
    UI_GetTextFromID(idTier).SetText(sTier.str(), FALSE);

    Blizzard::Time::Timestamp tTime = LeaderboardGetRiftScoreTimeToComplete(tScore.score());
    ElapsedTimeDisplayOptions tElapsedTimeDisplayOptions;
    tElapsedTimeDisplayOptions.fAlwaysShowSeconds = TRUE;
    tElapsedTimeDisplayOptions.fOnlyDisplayNumber = TRUE;
    tElapsedTimeDisplayOptions.fCountUp = TRUE;
    tElapsedTimeDisplayOptions.fDisplayMilliseconds = TRUE;
    CRefString sTime;
    sTime = DisplayElapsedTime(0, tTime, tElapsedTimeDisplayOptions);
    UI_GetTextFromID(idTime).SetText(sTime.str(), FALSE);
}

}  // namespace UIConsoleRiftEndScreen

static const int32 NUM_CONSOLE_PLAYERS_WITH_LOWER_SCORES_TO_SHOW = 3;
static const int32 NUM_CONSOLE_TOTAL_PLAYERS_TO_SHOW = 5;

struct ConsoleRiftEndScreenGlobals {  // size 0x158
    ConsoleRiftEndScreenGlobals();
    BOOL m_bIsActive;  // 0x00
    D3::GameMessage::RiftEndScreenInfo m_tRiftEndScreenInfo;  // 0x08
    int32 m_nControllerIndex;  // 0x88
    UIConsoleRiftEndScreen::LeaderboardRiftEndDataSource m_tLeaderboardDataSource;  // 0x90
    UIConsoleRiftEndScreen::LeaderboardRiftEndDataSource m_tPersonalBestDataSource;  // 0xb0
    blz::vector<D3::Leaderboard::Score, blz::allocator<D3::Leaderboard::Score> > m_arScores;  // 0xd0
    blz::vector<D3::Leaderboard::Score, blz::allocator<D3::Leaderboard::Score> > m_arScoresToShow;  // 0xe8
    bdOnlineUserID m_nUserId;  // 0x100
    D3::Leaderboard::Score m_tMyScore;  // 0x108
};
static_assert(sizeof(ConsoleRiftEndScreenGlobals) == 0x158, "ConsoleRiftEndScreenGlobals");

inline ConsoleRiftEndScreenGlobals::ConsoleRiftEndScreenGlobals()
: m_bIsActive(FALSE)
, m_nControllerIndex(-1)
{
}

static ConsoleRiftEndScreenGlobals * sgptConsoleRiftEndScreenGlobals;

void SetupRActorForPlayerInUISlot(const D3::GameMessage::RiftEndScreenConsolePlayerInfo & tRiftEndScreenConsolePlayerInfo, int32 nUISlot);
void sSetupCheatMiniatures();
void SetupTagsAndPortraitForPlayerInUISlot(const D3::GameMessage::RiftEndScreenConsolePlayerInfo & tRiftEndScreenConsolePlayerInfo, int32 nUISlot);
void sConsoleRiftEndScreen_PickFriendScoresToShow();
void sConsoleLeaderboardRiftEndRequestCallback(Console::Online::LeaderboardResult eResult, const blz::vector<D3::Leaderboard::Score, blz::allocator<D3::Leaderboard::Score> > & arScores);

namespace UIConsoleRiftEndScreen {

void InitializeForGame()
{
    sgptConsoleRiftEndScreenGlobals = new ConsoleRiftEndScreenGlobals;
    sgptConsoleRiftEndScreenGlobals->m_bIsActive = FALSE;
}

void DisposeForGame()
{
    delete sgptConsoleRiftEndScreenGlobals;
    sgptConsoleRiftEndScreenGlobals = NULL;
}

inline BOOL IsActive()
{
    return sgptConsoleRiftEndScreenGlobals && sgptConsoleRiftEndScreenGlobals->m_bIsActive;
}

inline void SetupMiniaturesPane();
inline void SetupTagsAndPortraitsPane();

void InitializeForWorld()
{
    if (IsActive())
        SetupMiniaturesPane();
}

void Open(const int32 nControllerIndex, D3::GameMessage::RiftEndScreenInfo & tRiftEndScreenInfo)
{
    if (UI_GetWindowFromID(UIDLG_CONSOLE_RIFT_END_SCREEN).IsActive())
        return;

    sgptConsoleRiftEndScreenGlobals->m_nControllerIndex = nControllerIndex;
    CancelAllChannelingAbilities();
    ::Console::Screens::ShowModalDialog(nControllerIndex, UIDLG_CONSOLE_RIFT_END_SCREEN, TRUE, GetUIIDNone());

    ::Console::ControllerActionManager & tActions = ::Console::Screens::GetActionManager();
    tActions.SetActionsDirtyFlag(TRUE);
    tActions.ClearAllActions();
    tActions.SetAction(7, 0, StringListFetch("Console:Close"));
    if ((GameIsSeasonal() || GameIsWeeklyChallenge()) && ::Console::GamerProfile::IsSignedInOnline(nControllerIndex))
        tActions.SetAction(5, 1, StringListFetch("ConsoleGameSettings:ViewLeaderboard"));
    ::Console::Screens::UpdateButtonLegendText();
    ::Console::Screens::ShowButtonLegend(TRUE);

    if (UI_TestID(UIWND_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_BACKGROUND))
        UI_GetWindowFromID(UIWND_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_BACKGROUND).SetActive(GameIsWeeklyChallenge());
    if (UI_TestID(UIWND_CONSOLE_RIFT_END_SCREEN_BACKGROUND))
        UI_GetWindowFromID(UIWND_CONSOLE_RIFT_END_SCREEN_BACKGROUND).SetActive(!GameIsWeeklyChallenge());

    if (sgptConsoleRiftEndScreenGlobals)
    {
        sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.CopyFrom(tRiftEndScreenInfo);
        sgptConsoleRiftEndScreenGlobals->m_bIsActive = TRUE;
        sgptConsoleRiftEndScreenGlobals->m_nUserId = ::Console::Online::LobbyService::Get(nControllerIndex)->GetOnlineUserID();
    }

    if (UI_TestID(UITEXT_CONSOLE_RIFT_END_SCREEN_TITLE))
    {
        CRefString sTieredRift = StringListFetch(GlobalSNOGet(GLOBALSNO_NEPHALEM_RIFT_STRINGLIST), "TieredRiftComplete");
        CRefString sChallengeRift = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CHALLENGE_RIFT), "ChallengeRiftComplete");
        UI_GetTextFromID(UITEXT_CONSOLE_RIFT_END_SCREEN_TITLE).SetText((GameIsWeeklyChallenge() ? sChallengeRift : sTieredRift).str(), FALSE);
    }

    for (int32 i = 0; i < 4; ++i)
    {
        UI_GetWindowFromID(GetPlayerWindowUIIDByIndex(i)).SetActive(FALSE);
        UI_GetTextFromID(GetPlayerNameUIIDBySlot(i)).SetActive(FALSE);
        UI_GetWindowFromID(GetPlayerBannerBaseBySlot(i)).SetActive(FALSE);
        UI_GetWindowFromID(GetPlayerBannerTrimBySlot(i)).SetActive(FALSE);
        UI_GetWindowFromID(GetPlayerPortraitUIIDBySlot(i)).SetActive(FALSE);
    }
    for (int32 i = 0; i < tRiftEndScreenInfo.console_player_info_size(); ++i)
        UI_GetWindowFromID(GetPlayerWindowUIIDByIndex(i)).SetActive(TRUE);

    UI_GetTextFromID(UITEXT_CONSOLE_RIFT_END_SCREEN_BLOODSHARD_CAP_INCREASE).SetActive(FALSE);

    SetupMiniaturesPane();
    SetupTagsAndPortraitsPane();
    SetupRewardsPane();
    SetupCenterPane();

    UI_GetWindowFromID(UIWND_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_COVER).SetActive(TRUE);
    if (GameIsSeasonal() || GameIsWeeklyChallenge())
    {
        if (UI_TestID(UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_FRIENDS_LIST))
            UI_GetConsoleListBoxFromID(UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_FRIENDS_LIST).SetDataSource(sgptConsoleRiftEndScreenGlobals->m_tLeaderboardDataSource);
        if (UI_TestID(UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_PERSONAL_LIST))
            UI_GetConsoleListBoxFromID(UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_PERSONAL_LIST).SetDataSource(sgptConsoleRiftEndScreenGlobals->m_tPersonalBestDataSource);
        SetupLeaderboardPane();
    }
}

}  // namespace UIConsoleRiftEndScreen

void SetupRActorForPlayerInUISlot(const D3::GameMessage::RiftEndScreenConsolePlayerInfo & tRiftEndScreenConsolePlayerInfo, int32 nUISlot)
{
    UIID idRActor = UIConsoleRiftEndScreen::GetPlayerRActorUIIDBySlot(nUISlot);
    UIRActor & tRActor = UI_GetRActorFromID(idRActor);
    if (RActorTryToGet(tRActor.GetRActor()))
        return;

    tRActor.Cleanup();
    PlayerClass eClass = PlayerClassFromGBID(tRiftEndScreenConsolePlayerInfo.hero_gbid_class());
    BOOL fIsFemale = (tRiftEndScreenConsolePlayerInfo.hero_flags() >> 1) & 1;
    VisualEquipment tVisualEquipment;
    memset(&tVisualEquipment, 0, sizeof(tVisualEquipment));
    if (tRiftEndScreenConsolePlayerInfo.has_hero_visual_equipment())
        tVisualEquipment.Import(tRiftEndScreenConsolePlayerInfo.hero_visual_equipment());

    SNO snoActor = GetFrontendCharacterSNO(eClass, fIsFemale);
    if (snoActor != -1)
    {
        PRTransform tTransform = cgtPRTransformIdentity;
        tRActor.SetRActor(UI_CreateHeroUIRActor(snoActor, GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE), tVisualEquipment, 0, eClass, fIsFemale, tTransform, tRActor.GetRenderContext(), 0x61000, 1.0f, FALSE));
        tRActor.SetLight(LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tRActor.GetRenderContext()));
    }
}

void sSetupCheatMiniatures()
{
    Player * ptPlayer = LocalPlayerGet();
    const ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    for (int32 i = 0; i < sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.console_player_info_size(); ++i)
    {
        UIID idRActor = UIConsoleRiftEndScreen::GetPlayerRActorUIIDBySlot(i);
        UIRActor & tRActor = UI_GetRActorFromID(idRActor);
        if (RActorTryToGet(tRActor.GetRActor()))
            continue;

        SNO snoActor = GetFrontendCharacterSNO(PlayerGetPlayerClass(tACDPlayer.id), !PlayerIsMale(*ptPlayer));
        PRTransform tTransform = cgtPRTransformIdentity;
        tRActor.SetRActor(UI_CreateHeroUIRActor(snoActor, GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE), tACDPlayer.ptVisualInventory->tEquipmentCurrent, 0, PlayerGetPlayerClass(tACDPlayer.id), !PlayerIsMale(*ptPlayer), tTransform, tRActor.GetRenderContext(), 0x61000, 1.0f, FALSE));
        tRActor.SetLight(LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tRActor.GetRenderContext()));
    }
}

namespace UIConsoleRiftEndScreen {

inline void SetupMiniaturesPane()
{
    if (!sgptConsoleRiftEndScreenGlobals)
        return;
    if (sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.is_from_cheat())
    {
        sSetupCheatMiniatures();
        return;
    }
    for (int32 i = 0; i < sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.console_player_info_size(); ++i)
        SetupRActorForPlayerInUISlot(sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.console_player_info(i), i);
}

}  // namespace UIConsoleRiftEndScreen

static inline RGBAColor sConvertBannerColorIndexToColor(int32 nColorIndex, int32 eBannerColorType)
{
    const GlobalDefinition * ptGlobals = DefinitionLockR<GlobalDefinition>();
    if (nColorIndex < 0 || nColorIndex > (int32)(ptGlobals->tBannerParams.serColorSets.dwSize / sizeof(BannerColorSet)))
        return crgbaWhite;
    return ptGlobals->tBannerParams.arColorSets[nColorIndex].rgbaTint[eBannerColorType];
}

void SetupTagsAndPortraitForPlayerInUISlot(const D3::GameMessage::RiftEndScreenConsolePlayerInfo & tRiftEndScreenConsolePlayerInfo, int32 nUISlot)
{
    UIID idName = UIConsoleRiftEndScreen::GetPlayerNameUIIDBySlot(nUISlot);
    UIText & tName = UI_GetTextFromID(idName);
    tName.SetActive(TRUE);
    if (tRiftEndScreenConsolePlayerInfo.has_player_name())
        tName.SetText(tRiftEndScreenConsolePlayerInfo.player_name().c_str(), FALSE);

    PlayerAccountBanner tBanner;
    tBanner.Import(sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.banner_configuration());

    UIWindow & tBannerBase = UI_GetWindowFromID(UIConsoleRiftEndScreen::GetPlayerBannerBaseBySlot(nUISlot));
    tBannerBase.SetActive(TRUE);
    tBannerBase.SetWindowColor(sConvertBannerColorIndexToColor(tBanner.nPatternColorIndex, 0));
    UIWindow & tBannerTrim = UI_GetWindowFromID(UIConsoleRiftEndScreen::GetPlayerBannerTrimBySlot(nUISlot));
    tBannerTrim.SetActive(TRUE);
    tBannerTrim.SetWindowColor(sConvertBannerColorIndexToColor(tBanner.nBackgroundColorIndex, 0));

    PlayerClass eClass = PlayerClassFromGBID(tRiftEndScreenConsolePlayerInfo.hero_gbid_class());
    BOOL fIsFemale = (tRiftEndScreenConsolePlayerInfo.hero_flags() >> 1) & 1;
    VisualEquipment tVisualEquipment;
    memset(&tVisualEquipment, 0, sizeof(tVisualEquipment));
    GBID gbidPortrait = GBID_NONE;
    if (tRiftEndScreenConsolePlayerInfo.has_hero_visual_equipment())
    {
        tVisualEquipment.Import(tRiftEndScreenConsolePlayerInfo.hero_visual_equipment());
        gbidPortrait = tVisualEquipment.tCosmeticItems[3].gbidItem;
    }
    int32 nLevel = tRiftEndScreenConsolePlayerInfo.has_alt_level() ? tRiftEndScreenConsolePlayerInfo.alt_level() : 0;
    if (gbidPortrait == GBID_NONE)
        gbidPortrait = PlayerGetPortraitGBIDFromParagonLevel(nLevel);
    UIID idPortrait = UIConsoleRiftEndScreen::GetPlayerPortraitUIIDBySlot(nUISlot);
    UIUtils::Console::DisplayPortrait(eClass, fIsFemale, nLevel, idPortrait, gbidPortrait);
    UI_GetWindowFromID(idPortrait).SetActive(TRUE);
}

namespace UIConsoleRiftEndScreen {

inline void SetupTagsAndPortraitsPane()
{
    if (!sgptConsoleRiftEndScreenGlobals)
        return;
    for (int32 i = 0; i < sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.console_player_info_size(); ++i)
        SetupTagsAndPortraitForPlayerInUISlot(sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.console_player_info(i), i);
}

void SetupRewardsPane()
{
    if (!sgptConsoleRiftEndScreenGlobals)
        return;
    if (!UI_TestID(UITEXT_CONSOLE_RIFT_END_SCREEN_REWARD_CONTAINER) || !UI_TestID(UITEXT_CONSOLE_RIFT_END_SCREEN_GOLD_REWARD) || !UI_TestID(UITEXT_CONSOLE_RIFT_END_SCREEN_XP_REWARD) || !UI_TestID(UITEXT_CONSOLE_RIFT_END_SCREEN_BLOODSHARD_CAP_INCREASE) || !UI_TestID(UIWND_CONSOLE_RIFT_END_SCREEN_ITEM_REWARD_TEXTURE))
        return;

    UI_GetWindowFromID(UITEXT_CONSOLE_RIFT_END_SCREEN_REWARD_CONTAINER).SetActive(!GameIsWeeklyChallenge());

    int32 nGoldReward = sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.gold_reward();
    int64 nXPReward = sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.xp_reward();
    if (nGoldReward > 0)
    {
        CRefString sGold = UIText_ValueToGameCurrency(nGoldReward, (CurrencyType)0);
        UI_GetTextFromID(UITEXT_CONSOLE_RIFT_END_SCREEN_GOLD_REWARD).SetText(sGold, FALSE);
    }
    if (nXPReward > 0)
    {
        CRefString sXP = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_QUESTLOG), "BountiesRewardXP"), UnicodeText::FormatNumber(nXPReward, TRUE, FNS_NORMAL, 0).str(), NULL);
        UI_GetTextFromID(UITEXT_CONSOLE_RIFT_END_SCREEN_XP_REWARD).SetText(sXP, FALSE);
    }

    if (sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.new_personal_best())
    {
        UI_GetWindowFromID(UIWND_CONSOLE_RIFT_END_SCREEN_ITEM_REWARD_TEXTURE).SetTextureFrame(ImageTextureFrame("UI:GreaterRifts_Reward_MaxBloodshardIcon_p2"));
        UIText & tCapIncrease = UI_GetTextFromID(UITEXT_CONSOLE_RIFT_END_SCREEN_BLOODSHARD_CAP_INCREASE);
        tCapIncrease.SetActive(TRUE);
        const ActorCommonData & tACDPlayer = ACDGet(LocalPlayerGet()->idPlayerACD);
        CRefString sCap = LocalizedSprintf(StringListFetch("NephalemRift:BloodShardCapIncreased").str(), UnicodeText::FormatNumber(ACDGetBloodShardCap(tACDPlayer), TRUE, FNS_NORMAL, 0).str());
        tCapIncrease.SetText(sCap.str(), FALSE);
    }

    if (!UI_TestID(UIWND_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_REWARDCONTAINER) || !UI_TestID(UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_ORIGINAL_RUNNER_TIME) || !UI_TestID(UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_CURRENT_TARGET_TIME) || !UI_TestID(UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_ALREADY_EARNED) || !UI_TestID(UIWND_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_REWARD_TEXTURE))
        return;

    UI_GetWindowFromID(UIWND_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_REWARDCONTAINER).SetActive(GameIsWeeklyChallenge());
    if (!GameIsWeeklyChallenge())
        return;

    if (sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.has_challenge_rift_time_to_beat_ms())
    {
        UIText & tText = UI_GetTextFromID(UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_ORIGINAL_RUNNER_TIME);
        ElapsedTimeDisplayOptions tElapsedTimeDisplayOptions;
        tElapsedTimeDisplayOptions.fAlwaysShowSeconds = TRUE;
        tElapsedTimeDisplayOptions.fOnlyDisplayNumber = TRUE;
        tElapsedTimeDisplayOptions.fDisplayMilliseconds = TRUE;
        CRefString sTime = DisplayElapsedTime(0, (Blizzard::Time::Timestamp)sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.challenge_rift_time_to_beat_ms() * 1000000, tElapsedTimeDisplayOptions);
        tText.SetText(sTime.str(), FALSE);
    }

    UIText & tTargetTime = UI_GetTextFromID(UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_CURRENT_TARGET_TIME);
    const CGameGlobals & tGameGlobals = CGameGlobalsGet();
    if (tGameGlobals.nChallengeRiftCompletionMilliseconds > 0)
    {
        ElapsedTimeDisplayOptions tElapsedTimeDisplayOptions;
        tElapsedTimeDisplayOptions.fAlwaysShowSeconds = TRUE;
        tElapsedTimeDisplayOptions.fOnlyDisplayNumber = TRUE;
        tElapsedTimeDisplayOptions.fDisplayMilliseconds = TRUE;
        CRefString sTime = DisplayElapsedTime(0, (Blizzard::Time::Timestamp)tGameGlobals.nChallengeRiftCompletionMilliseconds * 1000000, tElapsedTimeDisplayOptions);
        tTargetTime.SetText(sTime.str(), FALSE);
    }

    UIText & tAlreadyEarned = UI_GetTextFromID(UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_ALREADY_EARNED);
    tAlreadyEarned.SetActive(FALSE);
    UIWindow & tRewardTexture = UI_GetWindowFromID(UIWND_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_REWARD_TEXTURE);
    tRewardTexture.SetActive(FALSE);
    if (sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.has_challenge_rift_reward_gbid())
    {
        GBID gbidReward = sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.challenge_rift_reward_gbid();
        if (gbidReward != GBID_NONE)
        {
            ImageTextureFrame tTexFrame;
            ACDInventoryGetItemDrawInfoWithGBID(gbidReward, tTexFrame, 0);
            tRewardTexture.SetTextureFrame(tTexFrame);
        }
    }

    switch (sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.challenge_rift_reward_state())
    {
    case 0:
        tRewardTexture.SetActive(TRUE);
        tRewardTexture.SetShader(GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
        break;
    case 1:
        tRewardTexture.SetActive(TRUE);
        tRewardTexture.SetShader(-1);
        break;
    case 2:
        tRewardTexture.SetActive(FALSE);
        tAlreadyEarned.SetActive(TRUE);
        break;
    }
}

void SetupCenterPane()
{
    if (!sgptConsoleRiftEndScreenGlobals)
        return;

    CRefString sLevel = LocalizedSprintf(StringListFetch("NephalemRift:Victory_Tier").str(), UnicodeText::FormatNumber(sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.rift_tier(), TRUE, FNS_NORMAL, 0).str());
    if (UI_TestID(UITEXT_CONSOLE_RIFT_END_SCREEN_COMPLETED_LEVEL))
    {
        UIText & tText = UI_GetTextFromID(UITEXT_CONSOLE_RIFT_END_SCREEN_COMPLETED_LEVEL);
        tText.SetText(sLevel.str(), FALSE);
        tText.SetActive(!GameIsWeeklyChallenge());
    }
    if (UI_TestID(UITEXT_CONSOLE_RIFT_END_SCREEN_COMPLETED_TEXT))
        UI_GetTextFromID(UITEXT_CONSOLE_RIFT_END_SCREEN_COMPLETED_TEXT).SetActive(!GameIsWeeklyChallenge());
    if (UI_TestID(UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_COMPLETED_TIME))
        UI_GetTextFromID(UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_COMPLETED_TIME).SetActive(GameIsWeeklyChallenge());

    ElapsedTimeDisplayOptions tElapsedTimeDisplayOptions;
    tElapsedTimeDisplayOptions.fAlwaysShowSeconds = TRUE;
    tElapsedTimeDisplayOptions.fOnlyDisplayNumber = TRUE;
    tElapsedTimeDisplayOptions.fCountUp = TRUE;
    tElapsedTimeDisplayOptions.fDisplayMilliseconds = TRUE;
    CRefString sTime = DisplayElapsedTime(0, (Blizzard::Time::Timestamp)sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.completion_time_ms() * 1000000, tElapsedTimeDisplayOptions);
    if (UI_TestID(UITEXT_CONSOLE_RIFT_END_SCREEN_COMPLETED_TIME))
    {
        UIText & tText = UI_GetTextFromID(UITEXT_CONSOLE_RIFT_END_SCREEN_COMPLETED_TIME);
        tText.SetText(sTime.str(), FALSE);
        tText.SetActive(!GameIsWeeklyChallenge());
    }
    if (UI_TestID(UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_TIME))
    {
        UIText & tText = UI_GetTextFromID(UITEXT_CONSOLE_RIFT_END_SCREEN_CHALLENGE_RIFT_TIME);
        tText.SetText(sTime.str(), FALSE);
        tText.SetActive(GameIsWeeklyChallenge());
        if (GameIsWeeklyChallenge())
        {
            int32 nTargetTime = CGameGlobalsGet().nChallengeRiftCompletionMilliseconds;
            if (sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.completion_time_ms() < nTargetTime)
                tText.SetTextColor((FontSpecifier)3);
            else
                tText.SetTextColor((FontSpecifier)1);
        }
    }
}

inline uint64 sGetLeaderboardId()
{
    if (!sgptConsoleRiftEndScreenGlobals)
        return LB_NONE;
    if (!GameIsWeeklyChallenge())
    {
        switch (sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.console_num_participants())
        {
        case 1:
            switch (PlayerClassFromGBID(sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.console_player_info(0).hero_gbid_class()))
            {
            case PLAYER_CLASS_DEMONHUNTER: return LB_RIFT_SOLO_DEMONHUNTER;
            case PLAYER_CLASS_BARBARIAN: return LB_RIFT_SOLO_BARBARIAN;
            case PLAYER_CLASS_WIZARD: return LB_RIFT_SOLO_WIZARD;
            case PLAYER_CLASS_WITCHDOCTOR: return LB_RIFT_SOLO_WITCHDOCTOR;
            case PLAYER_CLASS_MONK: return LB_RIFT_SOLO_MONK;
            case PLAYER_CLASS_CRUSADER: return LB_RIFT_SOLO_CRUSADER;
            case PLAYER_CLASS_NECROMANCER: return LB_RIFT_SOLO_NECROMANCER;
            }
            break;
        case 2: return LB_RIFT_TEAM_2;
        case 3: return LB_RIFT_TEAM_3;
        case 4: return LB_RIFT_TEAM_4;
        }
        return LB_NONE;
    }
    switch (sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.console_num_participants())
    {
    case 2: return LB_CHALLENGE_RIFT_TEAM_2;
    case 3: return LB_CHALLENGE_RIFT_TEAM_3;
    case 4: return LB_CHALLENGE_RIFT_TEAM_4;
    }
    return LB_CHALLENGE_RIFT_TEAM_1;
}

// The 2.7.7 CPlayer predicate compares the mode word at 0x710179a200 with 1.
extern "C" BOOL sub_71000b4bf0();

void SetupLeaderboardPane()
{
    if (!UI_TestID(UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_FRIENDS_LIST))
        return;
    if (!sgptConsoleRiftEndScreenGlobals)
    {
        UI_GetWindowFromID(UIWND_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_COVER).SetActive(TRUE);
        return;
    }

    sgptConsoleRiftEndScreenGlobals->m_tLeaderboardDataSource.tLeaderboardScores.clear();
    sgptConsoleRiftEndScreenGlobals->m_tPersonalBestDataSource.tLeaderboardScores.clear();
    sgptConsoleRiftEndScreenGlobals->m_tMyScore.Clear();
    UI_GetWindowFromID(UIWND_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_COVER).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_RIFT_END_SCREEN_LEADERBOARD).SetActive(TRUE);

    BOOL bHardcore = sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.console_player_info(0).hero_flags() & 1;
    LeaderboardKeyAndPartition tLeaderboardKey;
    if (GameIsWeeklyChallenge())
        tLeaderboardKey.m_scope = LP_CHALLENGE;
    else
    {
        BOOL bAlternateScope = sub_71000b4bf0();
        if (bHardcore)
            tLeaderboardKey.m_scope = bAlternateScope ? static_cast<LeaderboardScope>(8) : LP_SEASONAL_HARDCORE;
        else
            tLeaderboardKey.m_scope = bAlternateScope ? static_cast<LeaderboardScope>(9) : LP_SEASONAL_SOFTCORE;
    }
    tLeaderboardKey.m_id = sGetLeaderboardId();
    if (tLeaderboardKey.m_id == LB_NONE)
        return;
    tLeaderboardKey.m_partition = GameIsWeeklyChallenge() ? sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.challenge_rift_number() : OnlineService::GetSeasonNum();

    int32 nUserIndex = sgptConsoleRiftEndScreenGlobals->m_nControllerIndex;
    Console::Online::LeaderboardGetScoresForUsers(nUserIndex, tLeaderboardKey, Console::Online::PresenceGetFriendIDs(nUserIndex),
        OnlineService::internal::function_bind_0_2<void, Console::Online::LeaderboardResult, const blz::vector<D3::Leaderboard::Score, blz::allocator<D3::Leaderboard::Score> > &, &sConsoleLeaderboardRiftEndRequestCallback>(), true);
}

}  // namespace UIConsoleRiftEndScreen

void sConsoleLeaderboardRiftEndRequestCallback(Console::Online::LeaderboardResult eResult, const blz::vector<D3::Leaderboard::Score, blz::allocator<D3::Leaderboard::Score> > & arScores)
{
    if (!sgptConsoleRiftEndScreenGlobals)
        return;

    UIConsoleListBox & tFriendsList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_FRIENDS_LIST);
    UIConsoleListBox & tPersonalList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_PERSONAL_LIST);
    if (eResult == Console::Online::LeaderboardResult::SUCCESS)
    {
        const D3::Leaderboard::Score * ptMyScore = blz::find_if(arScores.begin(), arScores.end(), [](const D3::Leaderboard::Score & tScore) {
            return tScore.has_game_account_id() && tScore.game_account_id() == sgptConsoleRiftEndScreenGlobals->m_nUserId;
        });
        if (ptMyScore != arScores.end())
            sgptConsoleRiftEndScreenGlobals->m_tMyScore.CopyFrom(*ptMyScore);

        if (sgptConsoleRiftEndScreenGlobals->m_tMyScore.has_metadata() && sgptConsoleRiftEndScreenGlobals->m_tMyScore.metadata().team_member_size() > 0)
            sgptConsoleRiftEndScreenGlobals->m_tPersonalBestDataSource.tLeaderboardScores.push_back(sgptConsoleRiftEndScreenGlobals->m_tMyScore);
        tPersonalList.ResetAllItems();
        tPersonalList.RefreshVisibleItems();

        for (const D3::Leaderboard::Score * ptScore = arScores.begin(); ptScore != arScores.end(); ++ptScore)
        {
            D3::Leaderboard::Score tScore = *ptScore;
            if (tScore.game_account_id() != sgptConsoleRiftEndScreenGlobals->m_nUserId)
                sgptConsoleRiftEndScreenGlobals->m_arScores.push_back(tScore);
        }

        sConsoleRiftEndScreen_PickFriendScoresToShow();
        if (sgptConsoleRiftEndScreenGlobals->m_arScoresToShow.size() != 0)
        {
            sgptConsoleRiftEndScreenGlobals->m_tLeaderboardDataSource.tLeaderboardScores = sgptConsoleRiftEndScreenGlobals->m_arScoresToShow;
            tFriendsList.ResetAllItems();
            tFriendsList.SetSelectedIndex(0);
            tFriendsList.RefreshVisibleItems();
            tFriendsList.SetActive(TRUE);
        }
        UI_GetWindowFromID(UIWND_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_COVER).SetActive(sgptConsoleRiftEndScreenGlobals->m_arScoresToShow.size() == 0 && sgptConsoleRiftEndScreenGlobals->m_tPersonalBestDataSource.tLeaderboardScores.size() == 0);
    }
    else
    {
        UI_GetWindowFromID(UIWND_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_COVER).SetActive(TRUE);
        tFriendsList.SetActive(FALSE);
        sgptConsoleRiftEndScreenGlobals->m_tLeaderboardDataSource.tLeaderboardScores.clear();
    }
}

static bool sIsSlotABetterThanSlotB(const D3::Leaderboard::Score & score1, const D3::Leaderboard::Score & score2)
{
    uint32 nLevel1 = LeaderboardGetRiftScoreLevel(score1.score());
    uint32 nLevel2 = LeaderboardGetRiftScoreLevel(score2.score());
    Blizzard::Time::Timestamp tTime1 = LeaderboardGetRiftScoreTimeToComplete(score1.score());
    Blizzard::Time::Timestamp tTime2 = LeaderboardGetRiftScoreTimeToComplete(score2.score());
    if (nLevel1 != nLevel2)
        return nLevel1 > nLevel2;
    return tTime1 < tTime2;
}

static inline BOOL sIsConsoleSlotAlreadyAdded(D3::Leaderboard::Score & tScore, blz::vector<D3::Leaderboard::Score, blz::allocator<D3::Leaderboard::Score> > & arScores)
{
    for (int32 i = 0; i < (int32)arScores.size(); ++i)
    {
        if (arScores[i].game_account_id() == tScore.game_account_id())
            return TRUE;
    }
    return FALSE;
}

void sConsoleRiftEndScreen_PickFriendScoresToShow()
{
    if (!sgptConsoleRiftEndScreenGlobals)
        return;

    blz::vector<D3::Leaderboard::Score, blz::allocator<D3::Leaderboard::Score> > & arScores = sgptConsoleRiftEndScreenGlobals->m_arScores;
    blz::sort(arScores.begin(), arScores.end(), sIsSlotABetterThanSlotB);

    for (uint32 i = 0; i < arScores.size(); ++i)
    {
        if (!sgptConsoleRiftEndScreenGlobals->m_tMyScore.has_score() || sIsSlotABetterThanSlotB(arScores[i], sgptConsoleRiftEndScreenGlobals->m_tMyScore))
        {
            if (!sIsConsoleSlotAlreadyAdded(arScores[i], sgptConsoleRiftEndScreenGlobals->m_arScoresToShow))
                sgptConsoleRiftEndScreenGlobals->m_arScoresToShow.push_back(arScores[i]);
            break;
        }
    }

    if (arScores.size() < NUM_CONSOLE_TOTAL_PLAYERS_TO_SHOW)
    {
        for (uint32 i = 0; i < arScores.size(); ++i)
        {
            if (!sIsConsoleSlotAlreadyAdded(arScores[i], sgptConsoleRiftEndScreenGlobals->m_arScoresToShow))
                sgptConsoleRiftEndScreenGlobals->m_arScoresToShow.push_back(arScores[i]);
        }
        blz::sort(sgptConsoleRiftEndScreenGlobals->m_arScoresToShow.begin(), sgptConsoleRiftEndScreenGlobals->m_arScoresToShow.end(), sIsSlotABetterThanSlotB);
    }
    else
    {
        D3::Leaderboard::Score * ptFirst = arScores.begin();
        D3::Leaderboard::Score * ptLast = arScores.end();
        for (ptrdiff_t nCount = ptLast - ptFirst; nCount > 1; --nCount)
            blz::swap(ptFirst[nCount - 1], ptFirst[rand() % nCount]);
        int32 nNumLowerScores = NUM_CONSOLE_PLAYERS_WITH_LOWER_SCORES_TO_SHOW;
        for (D3::Leaderboard::Score * ptScore = arScores.begin(); ptScore != arScores.end(); ++ptScore)
        {
            if (sIsSlotABetterThanSlotB(sgptConsoleRiftEndScreenGlobals->m_tMyScore, *ptScore))
            {
                if (!sIsConsoleSlotAlreadyAdded(*ptScore, sgptConsoleRiftEndScreenGlobals->m_arScoresToShow))
                    sgptConsoleRiftEndScreenGlobals->m_arScoresToShow.push_back(*ptScore);
                if (nNumLowerScores < 2)
                    break;
                --nNumLowerScores;
            }
        }
        blz::sort(sgptConsoleRiftEndScreenGlobals->m_arScoresToShow.begin(), sgptConsoleRiftEndScreenGlobals->m_arScoresToShow.end(), sIsSlotABetterThanSlotB);
    }
}

namespace UIConsoleRiftEndScreen {

void OnButton2(const UIID &, const int32)
{
    Close();
}

void OnButton3(const UIID & id, const int32 nControllerIndex)
{
    if ((GameIsSeasonal() || GameIsWeeklyChallenge()) && ::Console::GamerProfile::IsSignedInOnline(nControllerIndex))
    {
        Close();
        int32 nParticipants = sgptConsoleRiftEndScreenGlobals->m_tRiftEndScreenInfo.console_player_info_size();
        UILeaderboards::Console::SetRiftCompletionOpenContext(nParticipants, GameIsWeeklyChallenge());
        ::Console::Screens::OpenToScreen(::Console::Screens::LEADERBOARD_SCREEN_SET, 0, nControllerIndex, FALSE);
    }
}

void Close()
{
    ::Console::Screens::CloseModalDialog(UIDLG_CONSOLE_RIFT_END_SCREEN);
    ::Console::Screens::ShowButtonLegend(FALSE);
    for (int32 i = 0; i < 4; ++i)
        UI_GetRActorFromID(GetPlayerRActorUIIDBySlot(i)).Cleanup();

    if (sgptConsoleRiftEndScreenGlobals)
    {
        sgptConsoleRiftEndScreenGlobals->m_bIsActive = FALSE;
        if (UI_TestID(UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_PERSONAL_LIST))
            UI_GetConsoleListBoxFromID(UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_PERSONAL_LIST).SetSelectedIndex(-1);
        if (UI_TestID(UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_FRIENDS_LIST))
        {
            UI_GetConsoleListBoxFromID(UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_FRIENDS_LIST).SetSelectedIndex(-1);
            sgptConsoleRiftEndScreenGlobals->m_tLeaderboardDataSource.tLeaderboardScores.clear();
        }
        if (UI_TestID(UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_PERSONAL_LIST))
        {
            UI_GetConsoleListBoxFromID(UILIST_CONSOLE_RIFT_END_SCREEN_LEADERBOARD_PERSONAL_LIST).SetSelectedIndex(-1);
            sgptConsoleRiftEndScreenGlobals->m_tPersonalBestDataSource.tLeaderboardScores.clear();
        }
    }
}

UIID GetPlayerWindowUIIDByIndex(int32 nIndex)
{
    switch (nIndex)
    {
    case 0: return UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER1;
    case 1: return UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER2;
    case 2: return UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER3;
    case 3: return UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER4;
    }
    return GetUIIDNone();
}

UIID GetPlayerRActorUIIDBySlot(int32 nIndex)
{
    switch (nIndex)
    {
    case 0: return UIRACTOR_CONSOLE_RIFT_END_SCREEN_PLAYER1;
    case 1: return UIRACTOR_CONSOLE_RIFT_END_SCREEN_PLAYER2;
    case 2: return UIRACTOR_CONSOLE_RIFT_END_SCREEN_PLAYER3;
    case 3: return UIRACTOR_CONSOLE_RIFT_END_SCREEN_PLAYER4;
    }
    return GetUIIDNone();
}

UIID GetPlayerPortraitUIIDBySlot(int32 nIndex)
{
    switch (nIndex)
    {
    case 0: return UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER1_PORTRAIT;
    case 1: return UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER2_PORTRAIT;
    case 2: return UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER3_PORTRAIT;
    case 3: return UIWND_CONSOLE_RIFT_END_SCREEN_PLAYER4_PORTRAIT;
    }
    return GetUIIDNone();
}

UIID GetPlayerNameUIIDBySlot(int32 nIndex)
{
    switch (nIndex)
    {
    case 0: return UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER1_NAME;
    case 1: return UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER2_NAME;
    case 2: return UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER3_NAME;
    case 3: return UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER4_NAME;
    }
    return GetUIIDNone();
}

UIID GetPlayerBannerBaseBySlot(int32 nIndex)
{
    switch (nIndex)
    {
    case 0: return UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER1_BANNER_BASE;
    case 1: return UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER2_BANNER_BASE;
    case 2: return UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER3_BANNER_BASE;
    case 3: return UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER4_BANNER_BASE;
    }
    return GetUIIDNone();
}

UIID GetPlayerBannerTrimBySlot(int32 nIndex)
{
    switch (nIndex)
    {
    case 0: return UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER1_BANNER_TRIM;
    case 1: return UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER2_BANNER_TRIM;
    case 2: return UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER3_BANNER_TRIM;
    case 3: return UITEXT_CONSOLE_RIFT_END_SCREEN_PLAYER4_BANNER_TRIM;
    }
    return GetUIIDNone();
}

}  // namespace UIConsoleRiftEndScreen
