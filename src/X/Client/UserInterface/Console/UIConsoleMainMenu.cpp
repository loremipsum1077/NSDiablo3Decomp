// Original file: X/Client/UserInterface/Console/UIConsoleMainMenu.cpp
//
// The console front-end main menu: the option list, the background heroes/banner, party
// events, matchmaking and the per-button handlers.
#include "../../../Common/Definitions/WorldDefinition.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../../Sigma/DataStructures/XGrowableMap.h"
#include "../../../Common/Definitions/ActDefinition.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../Console/Online.h"
#include "../../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h"
#include "../../Console/LobbyService.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/ControllerUtils.h"
#include "ConsoleInputUtilities.h"
#include "ConsoleScreenManager.h"
#include "ConsoleModalNotificationManager.h"
#include "ControllerActions.h"
#include "UIConsolePlayerOptions.h"
#include "UIConsoleGameSettings.h"
#include "UIConsoleMainMenu.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../../Sound/Sound.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/PlayerAccount.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Common/OnlineService/Callback/Callback.h"
#include "../../../Common/Protobuf/Account.pb.h"
#include "../../../Common/Protobuf/Hero.pb.h"
#include "../../../Sigma/RefString.h"
#include "../../../Common/Game.h"
#include "../../Battlenet/Subsystems/FrontendBackgroundHeroManager.h"
#include "../Controls/UIRActorControl.h"
#include "UIConsolePartyDisplay.h"
#include "../../Console/SaveMigration.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../Controls/UITextControl.h"
#include "../../OnlineService/OnlineServiceClientBase.h"
#include "../../../Common/Protobuf/Console.pb.h"
#include "../../../Common/Achievements/Console/ConsoleAchievements.h"
#include "../../../Common/Definitions/TreasureClassDefinition.h"
#include "../../SNOPreload.h"
#include "../../Battlenet/Subsystems/FrontendBackgroundCameraManager.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../CGame.h"
#include "../../RActors/RActors.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../../Common/Definitions/QuestRangeDefinition.h"
#include "../../../Common/GameCommonData.h"
#include "ConsoleToastManager.h"
#include "../UIUtils.h"
void SPlayerGetCommunityBuffInfo(BOOL & bActive, float & flXPBuff, float & flLegendaryBuff, float & flGoldBuff, BOOL & bDoubleTreasureGoblins, BOOL & bDoubleBloodShards, BOOL & bDoubleRiftKeystones, BOOL & bEasterEggWorld, BOOL & bDoubleBountyBags); void SceneCreatePlacePrefetchFrustum(WorldPlace tPlace, Frustum & tFrustum); namespace Intersect { BOOL FrustumAndAABB(const Frustum & wf, const AABB & aabb); }  // X/Server/SPlayer.cpp:11619, X/Common/SceneCommon.cpp:2178, X/Sigma/Math/Intersect.cpp:589
// Declared here because their headers are not adopted yet; each comment names the defining file.
const CmdLineParams * CmdLineGetParams();  // X/Common/Shell/CmdLine.cpp
CRefString StringListFetch(LPCSTR szLabel);  // X/Common/Definitions/StringListDefinition.cpp
namespace Console { namespace GamerProfile { BOOL IsSignedInOnline(int32 nUserIdx); void SignOut(int32 nUserIdx); } } namespace Console { namespace GamerProfile { void EnsureNetworkServiceAccountForUser(int32 nUserIndex); uint32 GetNumAvailableHeroes(int32 nUserIdx); } }  // GamerProfileNX64.cpp GamerProfileNX64.cpp ; EnsureNetworkServiceAccountForUser GamerProfileNX64.cpp:1098, GetNumAvailableHeroes :1214
namespace Console { namespace Screens { void OpenToScreen(int32 eScreen, BOOL bClearHistory, int32 nControllerIndex, BOOL bSkipTransition); } }  // ConsoleScreenManager.cpp
namespace Console { namespace Online { void PartyRemovePlayer(int32 nControllerIndex); BOOL PartyCanStartGame(); } } namespace Console { namespace Online { void sForceSubmitNetworkRequest(); void PartyLeaveAndRemake(uint32 uFlags); } }  // X/Client/Console/Online.cpp ; sForceSubmitNetworkRequest Online.cpp:1199, PartyLeaveAndRemake :2944
PlayerClass PlayerClassFromGBID(const GBID gbid);  // X/Common/Player.cpp
namespace Console { namespace UIOnlineActions { void LocalPlayerJoin(int32 nControllerIndex, BOOL bFromInvite); } }  // OnlineActions.cpp
namespace UIPartyDisplay { namespace Console { void ClearNamePlateSelection(); void HideFriendDisplay(); void ShowLocalCommunicationsDisplay(); void HideLocalCommunicationsDisplay(); void RefreshFriendsDisplay(); void RefreshLocalCommunicationsDisplay(); void RefreshPartyNameplates(); } }  // UIConsolePartyDisplay.cpp
namespace UIGamePrivacyOptions { namespace Console { void ShowGamePrivacyOptions(int32 nControllerIndex); void CloseGamePrivacyOptions(); } }  // UIConsoleGamePrivacyOptions.cpp
namespace UIPartyDisplay { namespace Console { BOOL IsLocalCommunicationDisplayVisible(); BOOL CanDisplayLocalCommunicationDisplay(); } }  // UIConsolePartyDisplay.cpp
namespace Console { namespace Screens { FrontendBackgroundHeroManager & GetFrontendBackgroundHeroManager(); void CloseAllModalDialogs(); } }  // ConsoleScreenManager.cpp
namespace Console { namespace Online { PartyOnlineState PartyGetPreferredOnlineState(); void RefreshAdHocList(); void PartyJoin(PartyConnectionInfo & tPartyInfo, JoinType eJoinType, PartyJoinCallback pfnCallback); void PartyJoinCancel(); void PartyMatchmakeCancel(); void PartyMatchmake(PartyInfo * ptParties, uint32 uMaxParties, OnlineService::Callback<void (MMSearchResult, unsigned int)> pfnCallback); BOOL PartyStartGame(); void PartySwitchOnlineState(PartyOnlineState eState); GameStartingEvent & GetGameStartingEvent(); GameEndingEvent & GetGameEndingEvent(); } }  // X/Client/Console/Online.cpp Online.cpp:3436 Online.cpp:3540 Online.cpp:4604
const UIID & UI_GetControllerFocus();  // UI.cpp:674
uint32 UI_GetXMLUint32Constant(const UIControlSourceKey & tSrcKey, uint32 nDefaultValue);  // UI.cpp
UIRActor & UI_GetRActorFromID(const UIID & id);  // UI.cpp
SNO GetActBackgroundSNO(Act eAct);  // UIStartup.cpp
void UIStartup_SetBackgroundWorld(SNO snoWorld);  // UIStartup.cpp
namespace Console { namespace Online { BOOL IsSeasonsInitialized(); } }  // LobbyService.cpp:661
namespace Console { namespace GamerProfile { D3::Console::Profile & GetProfile(int32 nUserIdx); } }  // GamerProfileNX64.cpp:1192
int32 CountSetBits(const uint8 * lpb, const int32 nLen);  // X/Sigma/BitManip.cpp
class UITutorial;
#include "../../../Common/Definitions/SceneDefinition.h"
#include "../../../Common/Definitions/MarkerSetDefinition.h"
#include "../../../Common/TagMaps/Tags.h"
#include "../Interface/UITutorials.h"
extern UITutorialManager gtTutorialMgr;
namespace Console { namespace Utilities { typedef OnlineService::Callback<void ()> ShowStoreCallback; void ShowStoreUI(int32 nControllerIndex, const char * szTarget, bool isCategory, ShowStoreCallback cb); } }  // UtilitiesNX64.cpp:239
namespace Console { namespace GamerProfile { void SavePrefs(); } } namespace Console { namespace SaveMigration { void ExportSave(int32 nUserIndex, SaveMigrationCallback pfnCallback); void ImportSave(int32 nUserIndex, SaveMigrationCallback pfnCallback); } }  // GamerProfileNX64.cpp:1694 ; ExportSave SaveMigration.cpp:191, ImportSave :358
namespace UINetworkOptions { namespace Console { void CloseNetworkOptions(); } } namespace UINetworkOptions { namespace Console { void ShowNetworkOptions(int32 nControllerIndex); } } namespace UIGameSettings { namespace Console { void Open(int32 nControllerIndex, ScreenClosedCallback cb); } }  // UIConsoleNetworkOptions.cpp ; ShowNetworkOptions UIConsoleNetworkOptionsNX64.cpp:152, UIGameSettings::Console::Open UIConsoleGameSettings.cpp:806
namespace UIWhatsNew { namespace Console { BOOL IsVisible(); void Close(); } } namespace UIWhatsNew { namespace Console { void OpenForController(int32 nControllerIndex, BOOL bAnniversary, BOOL bNecromancer); } }  // UIConsoleWhatsNewNX64.cpp ; OpenForController UIConsoleWhatsNew.cpp:92
namespace UISeasonJourney { namespace Console { BOOL IsActive(); void Close(); } } namespace UISeasonJourney { namespace Console { void OpenForController(const int32 nControllerIndex); } }  // UIConsoleSeasonJourney.cpp ; OpenForController :664
namespace UIBannerCustomization { namespace Console { BOOL IsVisible(); void Close(); } } namespace UIBannerCustomization { namespace Console { void OpenForController(int32 nControllerIndex); } }  // UIConsoleBannerCustomization.cpp, Close :497 ; OpenForController :449
namespace UIProfileSummary { namespace Console { BOOL IsVisible(); void Close(); } } namespace UIProfileSummary { namespace Console { void OpenForController(int32 nControllerIndex); } }  // UIConsoleProfileSummary.cpp, Close :108 ; OpenForController :86
namespace UIAchievements { namespace Console { BOOL IsVisible(); void Close(); } } namespace UIAchievements { namespace Console { void OpenForController(int32 nControllerIndex); } }  // UIConsoleAchievements.cpp, Close :535 ; OpenForController :486
namespace Console { namespace Online { PartyMemberAddedEvent & GetPartyMemberAddedEvent(); PartyMemberReceivedDigestEvent & GetPartyMemberReceivedDigestEvent(); PartyGameInfoChangedEvent & GetPartyGameInfoChangedEvent(); } }  // Online.cpp
CRefString DisplayActName(Act eAct); CRefString DisplayFormattedHandicap(int32 nHandicap);  // X/Client/Battlenet/UI/Util/UIUtil.cpp
CRefString QuestsGetUIName(SNO snoQuest);  // X/Client/UserInterface/Interface/UIQuests.cpp
namespace UIUtils { namespace Console { CRefString GetTagName(int32 nTag); CRefString DisplayPartyOnlineStateIcon(int32 eState, int32 nUnused, float flScale); } }  // UIConsoleUtils.cpp
BOOL SNOPackChunkLoadAct(Act eAct); float SNOPackChunkGetActLoadPercentComplete(Act eAct); uint32 GetConsoleMaxHeroesAllowed(int32 nUserIndex);  // X/Common/Sno/SNOFiles.cpp ; SNOPackChunkGetActLoadPercentComplete SNOFiles.cpp:11409; GetConsoleMaxHeroesAllowed X/Common/Console/ConsoleLicenses.cpp:482
namespace Console { namespace UIOnlineActions { enum GameStartValidationError { GAME_START_OK = 0, GAME_START_ERROR_BUSY = 1, GAME_START_ERROR_LOCAL_PLAYERS_OFFLINE = 15 }; BOOL IsLocalJoinInProgress(); bool IsActiveSeason(uint32 uSeason, const D3::Account::Digest & tAccountDigest); GameStartValidationError ValidateGameCanStart(); void ClearLocalJoinFlag(); void CreatePartyIfNeeded(int32 nControllerIndex); CRefString GetGameStartErrorMessage(int32 eError, BOOL bIsHost, BOOL bGameInProgress, BOOL bIsLocalPlayer); } }  // X/Client/UserInterface/Console/OnlineActions.h / OnlineActions.cpp
namespace Console { namespace DLCManager { uint32 GetLicenses(); } }  // X/Client/Console/DLCManager.cpp:385
namespace UIPartyDisplay { namespace Console { void ShowFriendDisplay(BOOL bShow); void EnablePartyNameplates(BOOL bEnable); } } namespace UIPartyDisplay { namespace Console { void OnPartyListAcceptPressed(int32 nUserIndex); void OnFriendsListAcceptPressed(int32 nUserIndex); void OnLocalCommListAcceptPressed(int32 nUserIndex); void OnPartyNamePlateAcceptPressed(int32 nUserIndex); } }  // UIConsolePartyDisplay.cpp ; On*AcceptPressed UIConsolePartyDisplay.cpp:1599/1625/1639/1655
namespace UIGamesList { namespace Console { void Close(); } } namespace UIHallOfFallenHeroes { namespace Console { void Close(); } } namespace UIGamesList { namespace Console { void OpenForController(int32 nControllerIndex); } } namespace UIHallOfFallenHeroes { namespace Console { void OpenForController(int32 nControllerIndex); } } namespace UILeaderboards { namespace Console { void OpenForController(const int32 nControllerIndex); } } namespace UICredits { namespace Console { void Show(const int32 nControllerIndex); } void ExpansionCredits(); void Credits2017(); void CreditsEternalCollection(); }  // UIConsoleGamesList.cpp:224, UIConsoleHallOfFallenHeroes.cpp:146 ; OpenForController UIConsoleGamesList.cpp:199, UIConsoleHallOfFallenHeroes.cpp:120, UIConsoleLeaderboards.cpp:897; UICredits.cpp:859/600/624/648
namespace UIMainMenu {
namespace Console {
enum {
    MAX_JOINABLE_GAMES = 20,
};

enum {
    OPTION_CONTINUE = 0, OPTION_UNLOCK_FULL_GAME = 1, OPTION_GAME_SETTINGS = 2, OPTION_NETWORK_SETTINGS = 3, OPTION_LEAVE_PARTY = 4,
    OPTION_MATCHMAKE = 5, OPTION_FIND_GAME = 6, OPTION_OPTIONS = 7, OPTION_SEASON_JOURNEY = 8, OPTION_CREATE_SEASONAL = 9,
    OPTION_NECRO_UPSELL = 10, OPTION_CONNECT_DEMONWARE = 11, OPTION_MORE = 12, OPTION_CUSTOMIZE_BANNER = 13, OPTION_VIEW_PROFILE = 14,
    OPTION_HALL_OF_FALLEN = 15, OPTION_CHALLENGES = 16, OPTION_LEADERBOARDS = 17, OPTION_CINEMATICS = 18, OPTION_CREDITS_MENU = 19,
    OPTION_IMPORT_SAVE = 20, OPTION_EXPORT_SAVE = 21, OPTION_QUIT_DEMO = 22, OPTION_HELP = 23, OPTION_INSTALL = 24,
    OPTION_VIEW_WHATS_NEW = 25, OPTION_CREDITS_2017 = 26, OPTION_CREDITS_ROS = 27, OPTION_CREDITS_ETERNAL = 28, OPTION_NUM_OPTIONS = 29,
};

typedef int32 MenuOption;
} }
#include "../../../Common/TagMaps/ActorTags.h"
enum class MenuLevel : int {
    MAIN = 0,
    MORE = 1,
    CREDITS = 2,
};
#include "../../../Common/TagMaps/MarkerTags.h"
namespace UIMainMenu {
namespace Console {

enum ScreenActions {
    ACTION_CONFIRM = 0,
    ACTION_BACK = 1,
    ACTION_VIEW_PROFILE = 2,
    ACTION_REFRESH = 3,
    ACTION_SWITCH_HERO = 4,
    ACTION_INVITE = 5,
    ACTION_NETWORK_OPTIONS = 6,
    ACTION_REFRESH_ADHOC_LIST = 7,
};

enum QuestPrefetchStep {
    QUEST_PREFETCH_NONE = 0,
    QUEST_PREFETCH_WORLD = 1,
    QUEST_PREFETCH_DONE = 2,
};

void OnDPadLeft(const UIID & id, const int32 nControllerIndex);
void OnDPadRight(const UIID & id, const int32 nControllerIndex);
BOOL IsAtStartOfGame();
void Refresh3DBackgroundHeroes();
void RefreshQuestInfo();
void PrefetchQuest();
void OnStartOrContinue(BOOL bFromEvent, BOOL bSkipSignInAccepted);
BOOL sVerifyAllLocalPlayersSignedInOnline(int32 nControllerIndex);
void RefreshHighlightRActor();
void FocusMainMenu();
void OnButton4(const UIID & id, const int32 nControllerIndex);
void OnEventGameStarting();
void OnEventGameEnding();
void OnDifficultyChosen(BOOL bCanceled);
void OnMatchmakingFinished(Act eLastAct, SNO snoLastQuest, int32 nLastQuestStep, ::Console::Online::MMSearchResult eResult, uint32 nNumPartiesFound);
CRefString GetOptionFromIndex(int32 nOption);














static BOOL sbIsDemoPurchaseOptionDirty;
static UIMainMenu::Console::QuestPrefetchStep seQuestPrefetchStep;
static XGrowableMap<int, int, int, int> stPrefetchedSNOs;
static ResolvedPortalDestination stCurrentDestination;
static BOOL sbHasSeasonalHero;
static BOOL sbHaveSeasonData;
static BOOL sbHeroIsSeasonal;
static BOOL sbHaveNecroLicense;


static BOOL sbIsShowingConnectOption;









BOOL IsAtStartOfGame()
{
    BOOL fShowStart = TRUE;
    int32 nPrimaryIndex = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    if (nPrimaryIndex != -1)
    {
        D3::Hero::SavedDefinition & tHero = ::Console::GamerProfile::GetHero(nPrimaryIndex);
        fShowStart = !tHero.has_digest();
        if (tHero.has_digest())
        {
            OnlineService::DetailsDigest tDigest;
            tDigest.PopulateCompletedQuestList(tHero.digest().quest_history());
            fShowStart = !tDigest.CanSelectQuests();
        }
    }
    return fShowStart;
}




CRefString GetOptionFromIndex(int32 nOption)
{
    switch (nOption)
    {
    case OPTION_CONTINUE:
        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
            return StringListFetch("ConsoleDemo:StartDemo");
        else if (::Console::Online::PartyIsHost())
        {
            if (IsAtStartOfGame())
                return StringListFetch("Console:StartNewGame");
            else
                return StringListFetch("Console:ContinueGame");
        }
        else if (::Console::Online::PartyCanStartGame())
            return StringListFetch("Console:EnterGame");
        else
            return StringListFetch("Console:WaitingHost");
    case OPTION_UNLOCK_FULL_GAME: return StringListFetch("ConsoleDemo:PurchaseGame");
    case OPTION_GAME_SETTINGS: return StringListFetch("Console:GameSettings");
    case OPTION_NETWORK_SETTINGS: return StringListFetch("Console:NetworkOptions");
    case OPTION_LEAVE_PARTY: return StringListFetch("Console:LeaveParty");
    case OPTION_MATCHMAKE: return StringListFetch("Console:MatchmakeGame");
    case OPTION_FIND_GAME: return StringListFetch("Console:FindGame");
    case OPTION_OPTIONS: return StringListFetch("Console:Options");
    case OPTION_SEASON_JOURNEY: return StringListFetch("ConsoleSeasonJourney:Title");
    case OPTION_CREATE_SEASONAL: return StringListFetch("Console:CreateSeasonalHero");
    case OPTION_NECRO_UPSELL: return StringListFetch("Console:NecroUpsell");
    case OPTION_CONNECT_DEMONWARE: return StringListFetch("Console:ConnectToDemonwareSwitch");
    case OPTION_MORE: return StringListFetch("ConsoleNavMenu:More");
    case OPTION_CUSTOMIZE_BANNER: return StringListFetch("ConsolePause:Banner");
    case OPTION_VIEW_PROFILE: return StringListFetch("Console:ViewProfile");
    case OPTION_HALL_OF_FALLEN: return StringListFetch("ConsoleNavMenu:HallOfFallen");
    case OPTION_CHALLENGES: return StringListFetch("Console:Challenges");
    case OPTION_LEADERBOARDS: return StringListFetch("Console:Leaderboards");
    case OPTION_CINEMATICS: return StringListFetch("H2OLayout:Cinematics");
    case OPTION_CREDITS_MENU: return StringListFetch("Console:Credits");
    case OPTION_IMPORT_SAVE: return StringListFetch("Console:ImportSave");
    case OPTION_EXPORT_SAVE: return StringListFetch("Console:ExportSave");
    case OPTION_QUIT_DEMO: return StringListFetch("ConsoleDemo:QuitDemo");
    case OPTION_HELP: return StringListFetch("Console:Help");
    case OPTION_INSTALL: return StringListFetch("Console:Install");
    case OPTION_VIEW_WHATS_NEW: return StringListFetch("Console:ViewWhatsNew");
    case OPTION_CREDITS_2017: return StringListFetch("Console:Credits2017");
    case OPTION_CREDITS_ROS: return StringListFetch("Console:CreditsX1");
    case OPTION_CREDITS_ETERNAL: return StringListFetch("Console:CreditsEternal");
    default:
        return CRefString("I wish I was a real string");
    }
}




















BOOL ItemShouldBeVisible(
    MenuLevel eMenuLevel,
    int32 nItem)
{
    int32 nProfileUserIdx = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    if (nProfileUserIdx == -1)
    {
        return FALSE;
    }

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return nItem == OPTION_CONTINUE;
    }





























    switch (nItem)
    {

    case OPTION_CONTINUE:
        return eMenuLevel == MenuLevel::MAIN && ::Console::GamerProfile::IsHeroLoaded(nProfileUserIdx) &&
            ::Console::Online::PartyIsInParty();


    case OPTION_GAME_SETTINGS:
        return eMenuLevel == MenuLevel::MAIN && ::Console::Online::PartyIsInParty() && ::Console::Online::PartyIsHost();
    case OPTION_NETWORK_SETTINGS:
        return eMenuLevel == MenuLevel::MAIN && ::Console::Online::PartyIsHost();
    case OPTION_MATCHMAKE:
        return eMenuLevel == MenuLevel::MAIN && ::Console::Online::PartyIsInParty() && ::Console::Online::PartyIsOnline() && ::Console::Online::PartyIsHost();



    case OPTION_FIND_GAME:
        if (eMenuLevel == MenuLevel::MAIN && ::Console::Online::PartyIsInParty() && ::Console::Online::PartyIsHost() &&

            ::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_LAN)
        {
            return TRUE;
        }
        return FALSE;







    case OPTION_LEAVE_PARTY:
        return eMenuLevel == MenuLevel::MAIN && ::Console::Online::PartyIsInParty() && !::Console::Online::PartyIsHost();

    case OPTION_CINEMATICS:
    {
        uint32 nDataLen;
        const uint8 * pbPlayedCutscene = Prefs::GetPlayedCutScenes(nDataLen);
        return eMenuLevel == MenuLevel::MORE && CountSetBits(pbPlayedCutscene, nDataLen) > 0;
    }

    case OPTION_HALL_OF_FALLEN:
        return eMenuLevel == MenuLevel::MORE && ::Console::GamerProfile::GetProfile(nProfileUserIdx).fallen_heroes_size() > 0;

    case OPTION_UNLOCK_FULL_GAME:
    case OPTION_IMPORT_SAVE:
    case OPTION_EXPORT_SAVE:
    case OPTION_QUIT_DEMO:
    case OPTION_HELP:
    case OPTION_INSTALL:
        break;



















    case OPTION_CREATE_SEASONAL:
        if (eMenuLevel == MenuLevel::MAIN)
        {
            return ::Console::Online::IsSeasonsInitialized() && OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE && sbHaveSeasonData &&

                !sbHasSeasonalHero;

        }
        return FALSE;



    case OPTION_SEASON_JOURNEY:
        if (eMenuLevel == MenuLevel::MAIN)
        {
            if (::Console::Online::IsSeasonsInitialized() && OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE && sbHaveSeasonData)
            {
                return TRUE;
            }
        }
        return FALSE;



    case OPTION_LEADERBOARDS:
        if (eMenuLevel == MenuLevel::MORE)
        {
            if (::Console::GamerProfile::IsSignedInOnline(::Console::GamerProfile::GetPrimaryProfileUserIndex()))
            {
                return TRUE;
            }
        }
        return FALSE;


    case OPTION_NECRO_UPSELL:
        return eMenuLevel == MenuLevel::MAIN && !sbHaveNecroLicense;




    case OPTION_CREDITS_2017:
    case OPTION_CREDITS_ROS:
    case OPTION_CREDITS_ETERNAL:
        return eMenuLevel == MenuLevel::CREDITS;











    case OPTION_CONNECT_DEMONWARE:
        if (eMenuLevel == MenuLevel::MAIN &&
            ::Console::Online::LobbyService::ShouldShowReconnectOption(nProfileUserIdx))
        {
            sbIsShowingConnectOption = TRUE;
            return TRUE;
        }
        sbIsShowingConnectOption = FALSE;
        return FALSE;




    default:
        return eMenuLevel == (nItem > OPTION_MORE ? MenuLevel::MORE : MenuLevel::MAIN);
    }
    return FALSE;

}


void UpdateCommunityEventsDisplay()

{
    BOOL bCommunityBuffIsActive, bDoubleTreasureGoblins, bDoubleBloodShards, bDoubleRiftKeystones, bEasterEggWorld, bDoubleBountyBags;  // DWARF lists these declarators in reverse order
    float flXPBuff, flLegendaryBuff, flGoldBuff;
    SPlayerGetCommunityBuffInfo(bCommunityBuffIsActive, flXPBuff, flLegendaryBuff, flGoldBuff, bDoubleTreasureGoblins, bDoubleBloodShards, bDoubleRiftKeystones, bEasterEggWorld, bDoubleBountyBags);
    if (bCommunityBuffIsActive)
    {
        CRefString sDescriptionText;
        if (flXPBuff > EPSILON)  // EPSILON (MathGeneral.h): the original CU DWARF carries it; 1e-6f constant
        {
            CRefString sBuffText = LocalizedSprintf(StringListFetch("ConsoleUI:CommunityBuff_XP").str(), UnicodeText::FormatNumber(flXPBuff * 100.0f, TRUE, FNS_NORMAL, 0, 0, 0).str(), NULL);
            sDescriptionText += sBuffText + "\n";
        }
        if (flLegendaryBuff > EPSILON)
        {
            CRefString sBuffText = LocalizedSprintf(StringListFetch("ConsoleUI:CommunityBuff_Legendary").str(), UnicodeText::FormatNumber(flLegendaryBuff * 100.0f, TRUE, FNS_NORMAL, 0, 0, 0).str(), NULL);
            sDescriptionText += sBuffText + "\n";
        }
        if (flGoldBuff > EPSILON)
        {
            CRefString sBuffText = LocalizedSprintf(StringListFetch("ConsoleUI:CommunityBuff_Gold").str(), UnicodeText::FormatNumber(flGoldBuff * 100.0f, TRUE, FNS_NORMAL, 0, 0, 0).str(), NULL);
            sDescriptionText += sBuffText + "\n";
        }
        if (bDoubleTreasureGoblins)
        {
            CRefString sBuffText = StringListFetch("ConsoleUI:CommunityBuff_DoubleGoblins").str();
            sDescriptionText += sBuffText + "\n";
        }
        if (bDoubleBloodShards)
        {
            CRefString sBuffText = StringListFetch("ConsoleUI:CommunityBuff_DoubleBloodShards").str();
            sDescriptionText += sBuffText + "\n";
        }
        if (bDoubleRiftKeystones)
        {
            CRefString sBuffText = StringListFetch("ConsoleUI:CommunityBuff_DoubleRiftKeystones").str();
            sDescriptionText += sBuffText + "\n";
        }
        if (bEasterEggWorld)
        {
            CRefString sBuffText = StringListFetch("ConsoleUI:CommunityBuff_EasterEggWorld").str();
            sDescriptionText += sBuffText + "\n";
        }
        if (bDoubleBountyBags)
        {
            CRefString sBuffText = StringListFetch("ConsoleUI:CommunityBuff_DoubleBountyCacheBags").str();
            sDescriptionText += sBuffText + "\n";
        }
        UI_GetWindowFromID(UIWND_CONSOLE_MAIN_MENU_COMMUNITY_EVENTS).SetActive(TRUE);
        UIText& tTitle = UI_GetTextFromID(UITEXT_CONSOLE_MAIN_MENU_COMMUNITY_EVENTS_TITLE);
        UIText& tDescription = UI_GetTextFromID(UITEXT_CONSOLE_MAIN_MENU_COMMUNITY_EVENTS_DESCRIPTION);
        tTitle.SetActive(TRUE);
        tTitle.SetText(StringListFetch("ConsoleUI:CommunityBuff_Header"), FALSE);
        tDescription.SetActive(TRUE);
        tDescription.SetText(sDescriptionText, FALSE);
    }
    else
    {
        UI_GetWindowFromID(UIWND_CONSOLE_MAIN_MENU_COMMUNITY_EVENTS).SetActive(FALSE);
    }
}


class MainMenuListSource : public ConsoleListBoxDataSource {
public:

    MainMenuListSource() : m_eMenuLevel(MenuLevel::MAIN), m_bColorsInitialized(FALSE) {}
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);
    virtual BOOL IsItemEnabled(const UIConsoleListBox & tListBox, const int32 nIndex);
    void Initialize(MenuLevel eMenuLevel);
    blz::vector<int> m_menuItems; MenuLevel m_eMenuLevel; BOOL m_bColorsInitialized;  // DWARF members at 682-690 (methods defined in-class in the original)
    RGBAColor m_rgbaNormalTextColor; RGBAColor m_rgbaDisabledTextColor; RGBAColor m_rgbaHighlightedTextColor;
    void InitializeColors(); };
int32 MainMenuListSource::GetNumItems() const
{
    return (int32)m_menuItems.size();
}



UIControlSourceKey MainMenuListSource::GetSourceKeyForItem(const int32 nIndex) const
{
    return UIControlSourceKey::Parse("ConsoleMainMenu:ConsoleMainMenu_main.menuItem", -1, NULL, NULL);
}




void MainMenuListSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}












void MainMenuListSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    if (!m_bColorsInitialized)
    {
        InitializeColors();
    }

    UIID idText = UIIDCreateFromParent(idItemUI, ".text");

    BOOL bSelected = tListBox.GetSelectedIndex() == nIndex;
    UIText & tText = UI_GetTextFromID(idText);
    tText.SetText(GetOptionFromIndex(m_menuItems[nIndex]), FALSE);
    RGBAColor rgbaTextColor(!bSelected ? m_rgbaNormalTextColor : m_rgbaHighlightedTextColor);
    if (!IsItemEnabled(tListBox, nIndex))
    {
        rgbaTextColor = m_rgbaDisabledTextColor;
    }

    tText.SetTextColor(rgbaTextColor);
    if (bSelected)
    {
        UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_CONSOLE_MAIN_MENU_HIGHLIGHT);
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

    UIID idSeasonalIcon = UIIDCreateFromParent(idItemUI, ".SeasonalIcon");
    UI_GetWindowFromID(idSeasonalIcon).SetActive(m_menuItems[nIndex] == OPTION_SEASON_JOURNEY || m_menuItems[nIndex] == OPTION_CREATE_SEASONAL);

    UIID idNewIcon = UIIDCreateFromParent(idItemUI, ".NotificationIcon");

    if (::Console::Online::IsSeasonsInitialized() && OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE && sbHaveSeasonData)
    {
        int32 & nSeenLeaderboardsSeason = Prefs::GetLeaderboardSeenSeason();
        UI_GetWindowFromID(idNewIcon).SetActive((uint32)nSeenLeaderboardsSeason < OnlineService::GetSeasonNum() && (m_menuItems[nIndex] == OPTION_MORE || m_menuItems[nIndex] == OPTION_LEADERBOARDS));
    }
}


BOOL MainMenuListSource::IsItemEnabled(const UIConsoleListBox & tListBox, const int32 nIndex)
{
    if (m_menuItems[nIndex] != OPTION_CONTINUE)
        return TRUE;

    if (!sbHaveNecroLicense)
    {
        int32 nPrimaryIndex = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
        if (nPrimaryIndex != -1)
        {
            D3::Hero::SavedDefinition & tHero = ::Console::GamerProfile::GetHero(nPrimaryIndex);
            if (tHero.has_digest() && PlayerClassFromGBID(tHero.digest().gbid_class()) == PLAYER_CLASS_NECROMANCER)
                return FALSE;
        }
        return ::Console::Online::PartyCanStartGame();
    }

    return ::Console::Online::PartyCanStartGame();
}







void MainMenuListSource::Initialize(MenuLevel eMenuLevel)
{
    m_menuItems.clear();
    m_eMenuLevel = eMenuLevel;
    for (int32 nItem = 0; nItem < OPTION_NUM_OPTIONS; nItem++)
    {
        if (ItemShouldBeVisible(eMenuLevel, nItem))
            m_menuItems.push_back((int)nItem);
    }
}















void MainMenuListSource::InitializeColors()
{
    m_rgbaNormalTextColor.Set(UI_GetXMLUint32Constant(UIControlSourceKey::Parse("ConsoleMainMenu:c_GoldTextColor", -1, NULL, NULL), crgbaGold.ToUIColor()));
    m_rgbaHighlightedTextColor.Set(UI_GetXMLUint32Constant(UIControlSourceKey::Parse("ConsoleMainMenu:c_WhiteTextColor", -1, NULL, NULL), crgbaWhite.ToUIColor()));
    m_rgbaDisabledTextColor.Set(UI_GetXMLUint32Constant(UIControlSourceKey::Parse("ConsoleMainMenu:c_GrayTextColor", -1, NULL, NULL), crgbaGray.ToUIColor()));
    m_bColorsInitialized = TRUE;
}

struct MainMenuGlobals {
    MainMenuGlobals();
    MainMenuListSource tListSource;
    ModalNotificationId idQuickmatchDialog;
    ModalNotificationId idStartGameDialog;
    ModalNotificationId idImportSaveDialog;
    ::Console::AnalogStickInputFilter tStickFilter;
    ::Console::Online::PartyInfo arMatchmakerParties[MAX_JOINABLE_GAMES];
    uint32 uNumMatchmakerParties;
    BOOL bQuickmatchJoining;
    BOOL bAdventureModeLockOverride;
};
















MainMenuGlobals::MainMenuGlobals()
    : idQuickmatchDialog(INVALID_MODAL_NOTIFICATION_ID)
    , idStartGameDialog(INVALID_MODAL_NOTIFICATION_ID)
    , idImportSaveDialog(INVALID_MODAL_NOTIFICATION_ID)
    , uNumMatchmakerParties(0)
    , bQuickmatchJoining(FALSE)
    , bAdventureModeLockOverride(FALSE)
{
    tStickFilter = ::Console::GetDefaultStickInputFilter(::Console::FILTER_HORIZONTAL);
    tStickFilter.SetMinimumInputDelay(250);
}




MainMenuGlobals sgtMainMenuGlobals;



void Refresh3DBackgroundHeroes()
{
    FrontendBackgroundHeroManager & tHeroManager = ::Console::Screens::GetFrontendBackgroundHeroManager();
    if (::Console::Online::PartyIsInParty())
    {



        ::Console::Online::PartyMember atMembers[4];
        uint32 uNumPartyMembers = ::Console::Online::PartyGetMembers(atMembers, 1, FALSE);

        ::Console::Online::GameParams tParams;
        ::Console::Online::PartyGetGameParams(tParams);

        int32 nHeroSlot = 1;

        for (uint32 i = 0; i < uNumPartyMembers; i++)
        {



            ::Console::Online::PartyMember & tMember = atMembers[i];
            BackgroundHeroSlot eSlot;
            if (tMember.bIsLocal && tMember.bIsPrimary) eSlot = (BackgroundHeroSlot)0;
            else eSlot = (BackgroundHeroSlot)nHeroSlot++;
            if (tMember.tHeroDigest)
            {
                BackgroundHeroDescription tHero;
                PlayerClass ePlayerClass = PlayerClassFromGBID(tMember.tHeroDigest->gbid_class());
                BOOL bIsFemale = (tMember.tHeroDigest->player_flags() >> 1) & 1;
                BOOL bIsHardcore = tMember.tHeroDigest->player_flags() & 1;
                sbHeroIsSeasonal = tMember.tAccountDigest && ::Console::UIOnlineActions::IsActiveSeason(tMember.tHeroDigest->season_created(), *tMember.tAccountDigest);

                VisualEquipment tEquipment; tEquipment.Init();
                tEquipment.Import(tMember.tHeroDigest->visual_equipment());
                tHero.SetPlayerHero(ePlayerClass, bIsFemale, bIsHardcore, FALSE, tEquipment);
                if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
                {
                    tHero.SetDemoHero(ePlayerClass, bIsFemale, bIsHardcore);
                }


                ::Console::Online::GameParams tGameParams;
                if (::Console::Online::PartyGetGameParams(tGameParams) && tGameParams.m_eGameType == GT_CHALLENGE_RIFTS &&
                    tGameParams.m_tWeeklyChallengeData.has_hero_snapshot())
                {

                    OnlineService::HeroDigest tHeroDigest;
                    tHeroDigest.Import(OnlineService::GameAccountHandle::INVALID,

                                       CRefString(tGameParams.m_tWeeklyChallengeData.hero_snapshot().digest().hero_name().c_str()),
                                       tGameParams.m_tWeeklyChallengeData.hero_snapshot().digest(), -1);

                    if (tHeroDigest.m_visual.IsEquipmentValid() && tHeroDigest.m_details.m_playerClass != PLAYER_CLASS_NONE)
                    {
                        tHero.SetPlayerHero(tHeroDigest.m_details.m_playerClass, tHeroDigest.m_details.IsFemale(), tHeroDigest.m_details.IsHardcore(), FALSE, tHeroDigest.m_visual.GetEquipment());
                    }
                }


                tHeroManager.ChangeBackgroundHero(eSlot, tHero);
                if (eSlot == 0)
                {
                    ::Console::Online::GameParams tGameParams;
                    BOOL bIsWeeklyChallenge = ::Console::Online::PartyGetGameParams(tGameParams) && tGameParams.m_eGameType == GT_CHALLENGE_RIFTS;

                    UI_GetWindowFromID(UIWND_CONSOLE_MAIN_MENU_HARDCORE).SetActive(!bIsWeeklyChallenge && bIsHardcore && !sbHeroIsSeasonal);
                    UI_GetWindowFromID(UIWND_CONSOLE_MAIN_MENU_SEASONAL).SetActive(!bIsWeeklyChallenge && !bIsHardcore && sbHeroIsSeasonal);
                    UI_GetWindowFromID(UIWND_CONSOLE_MAIN_MENU_SEASONAL_HARDCORE).SetActive(!bIsWeeklyChallenge && bIsHardcore && sbHeroIsSeasonal);
                }
            }
            else
            {


                tHeroManager.ClearBackgroundHero(eSlot);
            }
        }



        while (nHeroSlot < NUM_HERO_SLOTS)
        {
            tHeroManager.ClearBackgroundHero((BackgroundHeroSlot)nHeroSlot++);
        }
    }
    else
    {

        tHeroManager.ClearAllBackgroundHeros();
    }
}

int32 GetMaxLocalHeroLevel()
{
    ::Console::Online::PartyMember atMembers[4];
    int32 nMaxLocalHeroLevel = 0;
    uint32 uNumPartyMembers = ::Console::Online::PartyGetMembers(atMembers, 1, FALSE);
    for (uint32 i = 0; i < uNumPartyMembers; i++)
    {
        ::Console::Online::PartyMember & tMember = atMembers[i];
        if (tMember.tHeroDigest && tMember.bIsLocal)
        {
            if (nMaxLocalHeroLevel < tMember.tHeroDigest->level())
                nMaxLocalHeroLevel = tMember.tHeroDigest->level();
        }
    }
    return nMaxLocalHeroLevel;
}




int32 GetMinLocalHeroLevel()
{
    ::Console::Online::PartyMember atMembers[4];
    int32 nMinLocalHeroLevel = INT32_MAX;
    uint32 uNumPartyMembers = ::Console::Online::PartyGetMembers(atMembers, 1, FALSE);
    for (uint32 i = 0; i < uNumPartyMembers; i++)
    {
        ::Console::Online::PartyMember & tMember = atMembers[i];
        if (tMember.tHeroDigest && tMember.bIsLocal)
        {
            if (tMember.tHeroDigest->level() < nMinLocalHeroLevel)
                nMinLocalHeroLevel = tMember.tHeroDigest->level();
        }
    }
    return nMinLocalHeroLevel;
}





void UpdateQuestPrefetch()

{
    switch (seQuestPrefetchStep)
    {
        case QUEST_PREFETCH_NONE:
        case QUEST_PREFETCH_DONE:
            break;
        case QUEST_PREFETCH_WORLD:

            if (SNOPreload::IsServerWorldLoadingDone(stCurrentDestination.snoWorld, 0))
            {
                ScopedWorldDefinitionLockR pdefWorld(stCurrentDestination.snoWorld);


                BOOL fFound = FALSE;
                WorldPlace tPlace;

                int32 nSceneCount = pdefWorld->ptServerData->ptScene->nSceneChunkCount;
                for (int32 i = 0; i < nSceneCount && !fFound; i++)
                {
                    SceneChunk & tScene = pdefWorld->ptServerData->ptScene->ptSceneChunks[i];
                    if (tScene.snoname.snoGroup != SCENE_SNO)
                    {
                        continue;
                    }

                    ScopedSceneDefinitionLockR pdefScene(tScene.snoname.snoHandle);
                    int32 nMarkerCount = pdefScene->arMarkerSets.GetArraySize();
                    for (int32 nMarker = 0; nMarker < nMarkerCount && !fFound; nMarker++)
                    {
                        ScopedMarkerSetDefinitionLockR pdefMarkerSet(pdefScene->arMarkerSets[nMarker]);
                        int32 nCountMarkers = pdefMarkerSet->tMarkerSet.GetArraySize();

                        for (int32 nMarkerInner = 0; nMarkerInner < nCountMarkers && !fFound; nMarkerInner++)
                        {
                            const Marker & tMarker = pdefMarkerSet->tMarkerSet[nMarkerInner];
                            if (tMarker.eType != MARKERTYPE_ACTOR)
                            {
                                continue;
                            }

                            ScopedActorDefinitionLockR pdefActor(tMarker.snoname.snoHandle);
                            // 20 = ACTORGIZMO_PORTAL_DESTINATION, 69 = ACTORGIZMO_SAVEPOINT_PORTAL (X/Common/GizmoSwig.h, not included: no free include line)
                            switch (TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TYPE))
                            {
                                case 20:
                                case 69:
                                {
                                    uint32 uStartingLocationGUID = TagMapGetInheritedValueInt(pdefActor->hTagMap, tMarker.hTagMap, TAG_ACTOR_GIZMO_PORTAL_DEST_ENTRANCE_NUMBER, TAG_MARKER_PORTAL_DEST_ENTRANCE_NUMBER);


                                    if (uStartingLocationGUID == stCurrentDestination.uEntranceGUID)
                                    {
                                        PRSTransform transform;
                                        tPlace.wp = (tScene.transform * tMarker.transform).wp;
                                        tPlace.idSWorld = SWORLDID_INVALID;  // no row of its own: the -1 appears only in the WorldPlace copy at 953
                                        fFound = TRUE;
                                    }
                                }
                                break;
                                default:
                                    break;
                            }
                        }
                    }
                }


                if (fFound)
                {
                    Frustum tFrustum;
                    SceneCreatePlacePrefetchFrustum(tPlace, tFrustum);
                    for (int32 i = 0; i < nSceneCount; i++)
                    {
                        SceneChunk & tScene = pdefWorld->ptServerData->ptScene->ptSceneChunks[i];
                        if (tScene.snoname.snoGroup != SCENE_SNO)
                        {
                            continue;
                        }

                        ScopedSceneDefinitionLockR pdefScene(tScene.snoname.snoHandle);
                        AABB aabbWorldSpace;
                        aabbWorldSpace.wp = tScene.transform * pdefScene->aabbMarkerSetBounds.wp;
                        aabbWorldSpace.wvExt = tScene.transform.Rotate(pdefScene->aabbMarkerSetBounds.wvExt);

                        if (Intersect::FrustumAndAABB(tFrustum, aabbWorldSpace))
                        {
                            if (!stPrefetchedSNOs.LookupPtr(tScene.snoname.snoHandle))
                            {

                                SNOPreload::PrefetchClientScene(tScene.snoname.snoHandle, 0);
                                stPrefetchedSNOs.SetAt_Tracked(tScene.snoname.snoHandle, TRUE);
                            }

                            for (int32 j = 0; j < 4; j++)
                            {
                                SNO snoLevelArea = tScene.tSceneSpec.arSnoLevelAreas[j];
                                if (snoLevelArea != SNO_NONE)
                                {
                                    if (!stPrefetchedSNOs.LookupPtr(snoLevelArea))
                                    {

                                        SNOPreload::PrefetchClientLevelArea(snoLevelArea, 0);
                                        stPrefetchedSNOs.SetAt_Tracked(snoLevelArea, TRUE);
                                    }
                                }
                            }
                        }
                    }

                    stPrefetchedSNOs.RemoveAll();
                }

                seQuestPrefetchStep = QUEST_PREFETCH_DONE;
            }
            break;

    }
}


void PrefetchQuest()
{
    ::Console::Online::GameParams tGameParams;

    if (!::Console::Online::PartyGetGameParams(tGameParams))
        return;

    if (!SNOPackChunkLoadAct(tGameParams.m_eAct))
        return;

    SNO snoAct = ActSNOFromActEnum(tGameParams.m_eAct);
    if (snoAct == SNO_NONE)
        return;

    BOOL fFound = FALSE;
    ScopedActDefinitionLockR pdefAct(snoAct);
    ResolvedPortalDestination tStartLocation;

    for (int32 i = 0; i < MAX_START_LOCATION_OVERRIDES; i++)
    {
        const ActStartLocOverride & tOverride = pdefAct->aStartLocOverride[i];
        if (tOverride.snoQuestRange != SNO_NONE && tOverride.tStartLocation.snoWorld != SNO_NONE &&
            QuestRangeEvaluate(tOverride.snoQuestRange, tGameParams.m_snoQuest, tGameParams.m_nQuestStep, NULL))
        {
            tStartLocation = tOverride.tStartLocation;
            fFound = TRUE;
            break;
        }
    }

    if (!fFound)
    {
        tStartLocation = pdefAct->tStartLocDefault;
    }

    if (tStartLocation.snoWorld != stCurrentDestination.snoWorld ||
        tStartLocation.uEntranceGUID != stCurrentDestination.uEntranceGUID ||
        seQuestPrefetchStep == QUEST_PREFETCH_NONE)
    {
        seQuestPrefetchStep = QUEST_PREFETCH_WORLD;
        stCurrentDestination.snoWorld = tStartLocation.snoWorld;
        stCurrentDestination.uEntranceGUID = tStartLocation.uEntranceGUID;
        SNOPreload::PrefetchClientWorld(stCurrentDestination.snoWorld, 0);
        SNOPreload::PrefetchWorldLoading(stCurrentDestination.snoWorld, 0);
    }
}












void RefreshChallengeRiftInfo(uint32 uChallengeNumber)
{
    // "Challenge Rift" on the first line, the challenge number on the second.
    CRefString szChallengeNumber = LocalizedSprintf(
        StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHALLENGE_RIFT), "ChallengeNumber"),
        UnicodeText::FormatNumber((int32)uChallengeNumber, TRUE, FNS_NORMAL, 0).str(), NULL);

    CRefString sQuestInfoString;
    sQuestInfoString.Format("%s\n%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHALLENGE_RIFT), "ChallengeRift").str(), szChallengeNumber.str());
    UI_GetTextFromID(UITEXT_CONSOLE_MENU_QUEST).SetText(sQuestInfoString, FALSE);
}



void RefreshQuestInfo()
{
    ::Console::Online::GameParams tGameParams;

    if (!::Console::Online::PartyGetGameParams(tGameParams))
        return;

    if (tGameParams.m_eGameType == GT_CHALLENGE_RIFTS)
    {
        if (tGameParams.m_tWeeklyChallengeConfig.has_challenge_number())
        {
            RefreshChallengeRiftInfo(tGameParams.m_tWeeklyChallengeConfig.challenge_number());
        }
    }
    else
    {
        Act currentSelectedAct = tGameParams.m_eAct;
        CRefString sAct;
        if (currentSelectedAct == ACT_INVALID)
        {
            sAct = StringListFetch("ConsoleQuestSelect:AnyAct");
        }
        else
        {
            sAct = DisplayActName(currentSelectedAct);
        }

        SNO currentSelectedQuest = tGameParams.m_snoQuest;
        CRefString sQuest;
        if (currentSelectedAct == ACT_OPEN_WORLD)
        {
            sQuest = DisplayActName(tGameParams.m_ePreferredAdventureAct);
        }
        else if (currentSelectedAct == ACT_INVALID)
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

        CRefString szDifficulty = DisplayFormattedHandicap(tGameParams.m_nHandicap);

        CRefString sFinalString = LocalizedSprintf(
            StringListFetch("Bnet_CallToArms:GameDetails").str(),
            sAct.str(),
            szDifficulty.str(), NULL);
        CRefString sQuestInfoString;
        if (::Console::Online::PartyIsInParty() &&
            ::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ONLINE_PUBLIC)
        {
            CRefString sTagName = UIUtils::Console::GetTagName(tGameParams.m_nTag);
            sQuestInfoString.Format("%s\n%s\n{c:FF91641E}%s{/c}", sFinalString.str(), sQuest.str(), sTagName.str());
        }
        else
        {
            sQuestInfoString.Format("%s\n%s", sFinalString.str(), sQuest.str());
        }

        UI_GetTextFromID(UITEXT_CONSOLE_MENU_QUEST).SetText(sQuestInfoString, FALSE);
    }
}




void RefreshHighlightRActor()
{
    if (UI_GetControllerFocus() == UILIST_CONSOLE_MENU_GROUP)
    {
        UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_CONSOLE_MAIN_MENU_HIGHLIGHT);
        PRTransform transform(cgqIdentity, cgwpNull);

        if (tFilagree.GetRActor() == RACTORID_INVALID)
        {
            RActorID idRActor = RActorCreate(GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_MENU_FILAGREE), 1.0f, -1.0f, transform, 0,
                tFilagree.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), -1);
            tFilagree.SetRActor(idRActor);
        }
    }
}









inline void ClearHighlightRActor()
{

    UIRActor & tFilagree = UI_GetRActorFromID(UIRACTOR_CONSOLE_MAIN_MENU_HIGHLIGHT);
    tFilagree.Cleanup();
}


void RefreshBackgroundWorld()
{
    ::Console::Online::GameParams tParams;
    if (::Console::Online::PartyGetGameParams(tParams) && tParams.m_eAct != ACT_INVALID)
    {
        Act eAct = tParams.m_eAct;
        if (eAct == ACT_OPEN_WORLD)
            eAct = tParams.m_ePreferredAdventureAct;
        UIStartup_SetBackgroundWorld(GetActBackgroundSNO(eAct));
    }
    RefreshHighlightRActor();
}










void Refresh3DBanner()
{
    int32 nControllerIndex = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    if (nControllerIndex != -1)
    {
        const D3::Account::SavedDefinition & tAccount = ::Console::GamerProfile::GetAccount(nControllerIndex);
        PlayerAccountBanner tBanner;
        tBanner.Import(tAccount.digest().banner_configuration());
        ::Console::Screens::GetFrontendBackgroundHeroManager().ChangeBackgroundBanner(tBanner);
    }
}




void RefreshMainMenu(BOOL bResetToTop)
{
    sgtMainMenuGlobals.tListSource.Initialize(sgtMainMenuGlobals.tListSource.m_eMenuLevel);
    UIConsoleListBox & tMenu = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_GROUP);
    tMenu.SetDataSource(sgtMainMenuGlobals.tListSource);
    tMenu.RefreshVisibleItems();

    if (bResetToTop)
    {
        if (sgtMainMenuGlobals.tListSource.IsItemEnabled(tMenu, 0))
            tMenu.SetSelectedIndex(0);
        else
            tMenu.SetSelectedIndex(1);
    }
}






void FocusMainMenu()
{
    UIPartyDisplay::Console::ClearNamePlateSelection();
    UIPlayerOptions::Console::ClosePlayerOptions();

    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).SetSelectedIndex(-1);
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST).SetSelectedIndex(-1);
    }

    UI_SetControllerFocus(UILIST_CONSOLE_MENU_GROUP);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_GROUP).SetSelectedIndex(-1);
    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_GROUP).SelectNextItem(FALSE);
    RefreshHighlightRActor();
}

void OnEventPartyMemberAdded(int32)
{
    Refresh3DBackgroundHeroes();
    if (::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ADHOC)
    {
        RefreshMainMenu(FALSE);
    }
}







void OnEventPartyMemberRemoved(int32)
{
    if (::Console::GamerProfile::GetPrimaryProfileUserIndex() == -1)
        return;

    RefreshBackgroundWorld();
    Refresh3DBackgroundHeroes();
    Refresh3DBanner();
    RefreshMainMenu(FALSE);
    RefreshQuestInfo();
}




void OnEventPartyJoined()
{
    if (!::Console::Online::PartyIsHost())
    {
        UINetworkOptions::Console::CloseNetworkOptions();
        UIGamePrivacyOptions::Console::CloseGamePrivacyOptions();
        if (UIGameSettings::Console::IsVisible())
            UIGameSettings::Console::Close();
        if (sgtMainMenuGlobals.idImportSaveDialog != INVALID_MODAL_NOTIFICATION_ID)
        {
            ::Console::Screens::GetModalNotificationManager().RemoveNotification(sgtMainMenuGlobals.idImportSaveDialog);
            sgtMainMenuGlobals.idImportSaveDialog = INVALID_MODAL_NOTIFICATION_ID;
        }
        if (UIWhatsNew::Console::IsVisible())
            UIWhatsNew::Console::Close();
        if (UISeasonJourney::Console::IsActive())
        {
            UISeasonJourney::Console::Close();
            if (::Console::Screens::IsScreenSetOpen(17))
                ::Console::Screens::CloseScreenSet(17, FALSE);
            else
                ::Console::Screens::CloseAllModalDialogs();
        }
    }

    RefreshBackgroundWorld();
    Refresh3DBackgroundHeroes();
    Refresh3DBanner();
    RefreshMainMenu(FALSE);
    RefreshQuestInfo();

    if (!UIBannerCustomization::Console::IsVisible() && !UIProfileSummary::Console::IsVisible() && !UIAchievements::Console::IsVisible())
        FocusMainMenu();
}


















void OnEventPartyGameInfoChanged()
{
    RefreshBackgroundWorld();
    Refresh3DBackgroundHeroes();
    Refresh3DBanner();
    RefreshQuestInfo();
    PrefetchQuest();
}





void OnEventPartyOnlineStateChanged(::Console::Online::PartySwitchOnlineResult eResult)
{
    RefreshMainMenu(FALSE);
    if (::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ADHOC)
        UIPartyDisplay::Console::ShowLocalCommunicationsDisplay();
    else
        UIPartyDisplay::Console::HideLocalCommunicationsDisplay();
}











void OnEventPartyMigrated(::Console::Online::MigrationEvent eEvent)
{
    if (sgtMainMenuGlobals.idStartGameDialog != INVALID_MODAL_NOTIFICATION_ID)
    {
        ::Console::Screens::GetModalNotificationManager().RemoveNotification(sgtMainMenuGlobals.idStartGameDialog);
        sgtMainMenuGlobals.idStartGameDialog = INVALID_MODAL_NOTIFICATION_ID;
    }
}





void OnStartGameErrorAccepted(const UIID &)
{
    sgtMainMenuGlobals.idStartGameDialog = INVALID_MODAL_NOTIFICATION_ID;
}



void OnStartOrContinue(BOOL bFromEvent, BOOL bSkipSignInAccepted)
{
    int32 nProfileUserIdx = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    if (nProfileUserIdx == -1 || ::Console::UIOnlineActions::IsLocalJoinInProgress())
        return;

    if (::Console::Online::PartyIsInParty())
    {
        ::Console::UIOnlineActions::GameStartValidationError eReason = ::Console::UIOnlineActions::ValidateGameCanStart();

        if (::Console::Screens::GetModalNotificationManager().IsNotificationShowing())
            eReason = ::Console::UIOnlineActions::GAME_START_ERROR_BUSY;

        if (!bSkipSignInAccepted)
        {
            ::Console::Online::GameParams tGameParams;
            if (::Console::Online::PartyGetGameParams(tGameParams) &&
                tGameParams.m_bIsSeasonal &&
                !sVerifyAllLocalPlayersSignedInOnline(nProfileUserIdx))
            {
                eReason = ::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ADHOC ?
                    ::Console::UIOnlineActions::GAME_START_ERROR_LOCAL_PLAYERS_OFFLINE :
                    ::Console::UIOnlineActions::GAME_START_ERROR_BUSY;
            }
        }

        if (bFromEvent && eReason == ::Console::UIOnlineActions::GAME_START_OK)
        {
            if (::Console::Screens::GetModalNotificationManager().IsNotificationShowing())
                eReason = ::Console::UIOnlineActions::GAME_START_ERROR_BUSY;
        }

        if (eReason == ::Console::UIOnlineActions::GAME_START_OK)
        {
            int32 nMaxLocalHeroLevel = GetMaxLocalHeroLevel();
            CGameGlobals & tCGameGlobals = CGameGlobalsGet();
            LoadingScreenData & tLSD = tCGameGlobals.tLoadingScreenData;
            tLSD.nMaxLocalHeroLevel = nMaxLocalHeroLevel;

            if (::Console::Online::PartyStartGame())
            {
                // The host shows the loading screen; everyone else waits for the host.
                if (::Console::Online::PartyIsHost())
                {
                    ::Console::Online::GameParams tGameParams;
                    if (::Console::Online::PartyGetGameParams(tGameParams) && GameCommonDataExists())
                    {
                        GameGetActiveGameCommonData().tGameSyncedData.eAct = tGameParams.m_eAct;
                    }
                    CGameLoadingScreenEnable(LSC_ALL);
                }
                else
                {
                    NotificationCreationParams tParams;
                    tParams.nControllerIndex = nProfileUserIdx;
                    tParams.uszMessage = StringListFetch("Console:WaitingHost");
                    sgtMainMenuGlobals.idStartGameDialog =
                        ::Console::Screens::GetModalNotificationManager().SetMessageOnlyNotification(tParams);
                }
            }
        }
        else
        {
            CRefString sErrorMessage = ::Console::UIOnlineActions::GetGameStartErrorMessage(eReason,
                ::Console::Online::PartyIsHost(),
                bFromEvent ? FALSE : ::Console::Online::PartyCanStartGame(), FALSE);

            if (sErrorMessage.Length() > 0)
            {
                if (bFromEvent)
                {
                    ::Console::Screens::GetToastManager().SendToastConsole(sErrorMessage, 0);
                }
                else
                {
                    NotificationCreationParams tParams;
                    tParams.nControllerIndex = nProfileUserIdx;
                    tParams.uszMessage = sErrorMessage;
                    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &OnStartGameErrorAccepted>();
                    sgtMainMenuGlobals.idStartGameDialog =
                        ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
                }
            }
        }
    }
}






































void OnSeasonSignedInErrorAccepted(int32 nControllerIndex, const UIID &)
{
    // Try again, now that the players had a chance to sign in.
    OnStartOrContinue(FALSE, TRUE);
}



BOOL sVerifyAllLocalPlayersSignedInOnline(int32 nControllerIndex)
{
    ::Console::Online::PartyMember atMembers[4];
    BOOL bAllGood = TRUE;
    BOOL bSignedIn[4];
    uint32 uNumPartyMembers = ::Console::Online::PartyGetMembers(atMembers, 0, TRUE);

    for (uint32 i = 0; i < uNumPartyMembers; i++)
    {
        ::Console::Online::PartyMember & tMember = atMembers[i];
        bSignedIn[i] = ::Console::GamerProfile::IsSignedInOnline(tMember.nControllerIndex);
        bAllGood &= bSignedIn[i];
    }

    if (bAllGood)
        return TRUE;

    NotificationCreationParams tParams;
    tParams.tAcceptButtonDelay = 3000;
    tParams.nControllerIndex = nControllerIndex;
    tParams.uszMessage = StringListFetch("Console:PlayerNotSignedIn");
    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_1_1<void, int, const UIID &, &OnSeasonSignedInErrorAccepted>(nControllerIndex);
    ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);

    return FALSE;
}






void sOnQuitDemoWarningAccepted(int32 nControllerIndex, const UIID &)
{
    ::Console::Screens::OpenToScreen(29, FALSE, nControllerIndex, FALSE);
}







void OnQuitDemo(int32 nControllerIndex)
{
    NotificationCreationParams tParams;
    tParams.nControllerIndex = nControllerIndex;
    tParams.uszMessage = StringListFetch("H2OLayout:QuitConfirm");
    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_1_1<void, int, const UIID &, &sOnQuitDemoWarningAccepted>(nControllerIndex);
    ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
}




void OnEventGameStarting()
{
    RefreshMainMenu(FALSE);
    OnStartOrContinue(TRUE, FALSE);
}


void OnEventGameEnding()
{
    RefreshMainMenu(FALSE);
    if (sgtMainMenuGlobals.idStartGameDialog != INVALID_MODAL_NOTIFICATION_ID)
    {
        ::Console::Screens::GetModalNotificationManager().RemoveNotification(sgtMainMenuGlobals.idStartGameDialog);
        sgtMainMenuGlobals.idStartGameDialog = INVALID_MODAL_NOTIFICATION_ID;
    }
}

} }
namespace Console { namespace GamerProfile {  // X/Client/Console/GamerProfile.h (not adopted there yet)
struct HeroInfo {
    HeroInfo();
    OnlineService::HeroId uHeroId;
    Blizzard::Time::UnixTime uCreationTime;
    char szFilename[32];
    char szName[49];
    PlayerClass eClass;
    uint32 uLevel;
    uint32 uAltLevel;
    uint32 uPlayerFlags;
    Act eLastAct;
    Blizzard::Time::UnixTime uDeathTime;
    SNO snoKiller;
    MonsterRarity eKillerRarity;
    GBID arKillerRareNameGBIDs[2];
    CRefString usKillerName;
    uint64 uMonstersKilled;
    uint64 uElitesKilled;
    uint64 uGoldCollected;
    uint32 uTimePlayed;
    uint32 uSeasonCreated;
    VisualEquipment tEquipment;
};
typedef OnlineService::Callback<void (unsigned int)> FindHeroesCallback;
void FindHeroes(int32 nUserIdx, HeroInfo * ptHeroInfoList, uint32 uMaxHeroes, FindHeroesCallback pfnCallback);  // GamerProfileNX64.cpp
BOOL IsAccountLoaded(int32 nUserIdx);  // GamerProfileNX64.cpp
} }
namespace UIMainMenu { namespace Console {


::Console::GamerProfile::HeroInfo tHeroInfoList[17];



void sOnHeroesListed(int32 nControllerIndex, int32 nHeroCount)
{
    sbHasSeasonalHero = FALSE;

    for (int32 i = 0; i < nHeroCount; i++)
    {
        if (tHeroInfoList[i].uSeasonCreated && tHeroInfoList[i].uSeasonCreated == OnlineService::GetSeasonNum())
        {
            sbHasSeasonalHero = TRUE;
            break;
        }
    }

    RefreshMainMenu(sbHasSeasonalHero);
}




void sCheckForSeasonalHero(int32 nControllerIndex)
{
    // Look for a hero created in the current season.
    ::Console::GamerProfile::FindHeroes(nControllerIndex,
        tHeroInfoList,
        17,
        OnlineService::internal::function_bind_1_1<void, int, int, &sOnHeroesListed>(nControllerIndex));
}


// NON_MATCHING: 3372 bytes both, on DWARF lines; only the two zeroing stores of idCosmeticsList (vector _init, sp+0x30/sp+0x38) are scheduled in the opposite order; declaration order and line layout did not change it.
void OpenForController(
    int32 nControllerIndex)
{
    UI_GetControlFromID(UIWND_CONSOLE_MAIN_MENU).SetActive(TRUE);

    if (::Console::Online::IsSeasonsInitialized() && OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE)
    {
        sCheckForSeasonalHero(nControllerIndex);
    }

    sbHaveNecroLicense = (::Console::DLCManager::GetLicenses() & 0x80) != 0;

    {

        int32 nSeasonNum = OnlineService::GetSeasonNum();


        OnlineService::RewardIdList idItemsList;
        OnlineService::RewardIdList idCosmeticsList;

        ::Console::Achievements * ptAchievements = sgtAppGlobals.ptAchievements;

        idItemsList.clear();


        uint32 dwItemTypes = 1 << OnlineService::REWARD_TYPE_ITEM;

        ptAchievements->GetAllRewardsOfTypes(dwItemTypes, idItemsList, nSeasonNum);

        for (int32 n = 0; n < idItemsList.size(); n++)
        {
            ScopedItemDefinitionLockR pdefSetItem(idItemsList[n]);
            if (!pdefSetItem)
                continue;



            SNO snoTreasureClass = pdefSetItem->snoSalvageTreasureClassNormal;

            if (snoTreasureClass == SNO_NONE || snoTreasureClass == INVALID_SNO)
                continue;



            ScopedTreasureClassDefinitionLockR pdefTreasureClass(snoTreasureClass);

            for (int32 i = 0; i < pdefTreasureClass->nModifierCount; i++)
            {
                GBID gbidItem = pdefTreasureClass->ptModifierList[i].tSpecificItem.gbidItem;

                ScopedItemDefinitionLockR pdefItem(gbidItem);
                if (!pdefItem)
                    continue;


                ScopedItemTypeDefinitonLockR pItemTypeDef(pdefItem->gbidItemType);
                if (!pItemTypeDef)
                    continue;




                SNOPreload::PrefetchClientActor(pdefItem->snoActor, NULL, 2);
            }
        }

        uint32 dwCosmeticTypes = 1 << OnlineService::REWARD_TYPE_COSMETIC;

        ptAchievements->GetAllRewardsOfTypes(dwCosmeticTypes, idCosmeticsList, nSeasonNum);

        for (uint32 i = 0; i < idCosmeticsList.size(); i++)
        {
            GBID gbidItem = idCosmeticsList[i];

            ScopedItemDefinitionLockR pdefItem(gbidItem);
            if (!pdefItem)
                continue;



            SNOPreload::PrefetchClientActor(pdefItem->snoActor, NULL, 2);
        }
    }



    ::Console::Screens::ClearScreenSetHistory();
    ::Console::UIOnlineActions::ClearLocalJoinFlag();
    ::Console::UIOnlineActions::CreatePartyIfNeeded(nControllerIndex);

    ::Console::Online::GetPartyMemberAddedEvent().Subscribe(OnlineService::internal::function_bind_0_1<void, int, &OnEventPartyMemberAdded>());
    ::Console::Online::GetPartyMemberReceivedDigestEvent().Subscribe(OnlineService::internal::function_bind_0_1<void, int, &OnEventPartyMemberAdded>());
    ::Console::Online::GetPartyMemberRemovedEvent().Subscribe(OnlineService::internal::function_bind_0_1<void, int, &OnEventPartyMemberRemoved>());
    ::Console::Online::GetPartyJoinedEvent().Subscribe(OnlineService::internal::function_bind_0_0<void, &OnEventPartyJoined>());
    ::Console::Online::GetPartyGameInfoChangedEvent().Subscribe(OnlineService::internal::function_bind_0_0<void, &OnEventPartyGameInfoChanged>());
    ::Console::Online::GetPartyOnlineStateChangedEvent().Subscribe(OnlineService::internal::function_bind_0_1<void, ::Console::Online::PartySwitchOnlineResult, &OnEventPartyOnlineStateChanged>());
    ::Console::Online::GetPartyMigratingEvent().Subscribe(OnlineService::internal::function_bind_0_1<void, ::Console::Online::MigrationEvent, &OnEventPartyMigrated>());

    FrontendBackgroundCameraManager & tCameraManager = ::Console::Screens::GetFrontendBackgroundCameraManager();
    if (tCameraManager.GetBackgroundCameraPosition() != CAM_POS_WIDE ||
        (tCameraManager.GetTargetBackgroundCameraPosition() != CAM_POS_NONE && tCameraManager.GetTargetBackgroundCameraPosition() != CAM_POS_WIDE))
    {
        uint32 dwDuration = 30;  // DWARF 1759: const_value 30
        tCameraManager.TransitionBackgroundCamera(CAM_POS_WIDE, dwDuration);

        {
            SNO sound = GlobalSNOGet(GLOBALSNO_SOUND_CAMERA_ZOOMOUT);
            SoundManager::PlaySound(sound, cgtPSODefaults);
        }
    }


    if (::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ADHOC)
    {
        ::Console::Online::RefreshAdHocList();
    }


    RefreshMainMenu(FALSE);
    RefreshQuestInfo();
    UIConsoleListBox & tMenu = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_GROUP);
    if (tMenu.GetSelectedIndex() == -1)
    {
        tMenu.SelectNextItem(FALSE);
    }

    UI_SetControllerFocus(tMenu.GetID());

    RefreshBackgroundWorld();

    BOOL bBlizzcon = CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT);
    if (!bBlizzcon)
    {
        UIPartyDisplay::Console::ShowFriendDisplay(FALSE);

        if (::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ADHOC)
        {
            UIPartyDisplay::Console::ShowLocalCommunicationsDisplay();
            ::Console::Online::RefreshAdHocList();
        }
    }


    if (UI_TestID(UIWND_CONSOLE_MAIN_MENU_BLIZZCON_CONTAINER))
    {
        UI_GetWindowFromID(UIWND_CONSOLE_MAIN_MENU_BLIZZCON_CONTAINER).SetActive(bBlizzcon);
    }
    if (UI_TestID(UIWND_CONSOLE_MENU_LOGO))
    {
        UI_GetWindowFromID(UIWND_CONSOLE_MENU_LOGO).SetActive(!bBlizzcon);
    }
    if (UI_TestID(UITEXT_CONSOLE_MAIN_MENU_SEASON))
    {
        UI_GetWindowFromID(UITEXT_CONSOLE_MAIN_MENU_SEASON).SetActive(!bBlizzcon);
    }

    UIPartyDisplay::Console::EnablePartyNameplates(TRUE);
    Refresh3DBackgroundHeroes();
    Refresh3DBanner();





    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();










    if (::Console::GamerProfile::IsAccountLoaded(nControllerIndex))
    {
        if (gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_GAME_SETTINGS), ::Console::GamerProfile::GetAccount(nControllerIndex)))
        {
            ::Console::Online::PartyUpdatePlayerHero(nControllerIndex);
        }
    }
}


static void Cancel(int32 nControllerIndex)
{
    ::Console::Screens::GetActionManager().ClearAllActions();
    ::Console::Screens::UpdateButtonLegendText();
    ::Console::Screens::OpenPreviousScreen(nControllerIndex, FALSE);

    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        UIPartyDisplay::Console::HideFriendDisplay();
        UIPartyDisplay::Console::HideLocalCommunicationsDisplay();
    }
}










void InitializeForGame()
{
    if (GameIsStartUp())
    {
        ::Console::Online::GetGameStartingEvent().Subscribe(OnlineService::internal::function_bind_0_0<void, &OnEventGameStarting>());
        ::Console::Online::GetGameEndingEvent().Subscribe(OnlineService::internal::function_bind_0_0<void, &OnEventGameEnding>());
    }
}



void DisposeForGame()
{
    ::Console::Online::GetGameStartingEvent().Unsubscribe(OnlineService::internal::function_bind_0_0<void, &OnEventGameStarting>());
    ::Console::Online::GetGameEndingEvent().Unsubscribe(OnlineService::internal::function_bind_0_0<void, &OnEventGameEnding>());
}



void RestoreGameParamsAfterFailedMatch(Act eLastAct, SNO snoLastQuest, int32 nLastQuestStep)
{
    // Put back the act and quest the party had before matchmaking changed them.
    ::Console::Online::GameParams tGameParams;
    if (::Console::Online::PartyGetGameParams(tGameParams))
    {
        tGameParams.m_eAct = eLastAct;
        tGameParams.m_snoQuest = snoLastQuest;
        tGameParams.m_nQuestStep = nLastQuestStep;
        ::Console::Online::PartySetGameParams(tGameParams);
    }
}













void OnMatchmakingPartyJoinAttempt(uint32 uPartyIndex, Act eLastAct, SNO snoLastQuest, int32 nLastQuestStep, ::Console::Online::JoinResult eResult)
{
    if (eResult == ::Console::Online::JOIN_SUCCESS || sgtMainMenuGlobals.bQuickmatchJoining)
    {
        sgtMainMenuGlobals.bQuickmatchJoining = FALSE;
        if (eResult != ::Console::Online::JOIN_SUCCESS)
        {
            uint32 uNextParty = uPartyIndex + 1;
            if (uNextParty < sgtMainMenuGlobals.uNumMatchmakerParties)
            {
                sgtMainMenuGlobals.bQuickmatchJoining = TRUE;
                ::Console::Online::PartyJoin(sgtMainMenuGlobals.arMatchmakerParties[uNextParty], ::Console::Online::JOIN_TYPE_MATCHMAKE,
                    OnlineService::internal::function_binder_5<void, uint32, Act, SNO, int32, ::Console::Online::JoinResult>::bind<&OnMatchmakingPartyJoinAttempt>(uNextParty, eLastAct, snoLastQuest, nLastQuestStep));
            }
            else
            {
                OnMatchmakingFinished(eLastAct, snoLastQuest, nLastQuestStep, ::Console::Online::MMSEARCH_SUCCESS, 0);
            }
            return;
        }
    }

    if (sgtMainMenuGlobals.idQuickmatchDialog != INVALID_MODAL_NOTIFICATION_ID)
    {
        ::Console::Screens::GetModalNotificationManager().RemoveNotification(sgtMainMenuGlobals.idQuickmatchDialog);
        sgtMainMenuGlobals.idQuickmatchDialog = INVALID_MODAL_NOTIFICATION_ID;
    }
}























void OnMatchmakingFinished(Act eLastAct, SNO snoLastQuest, int32 nLastQuestStep, ::Console::Online::MMSearchResult eResult, uint32 nNumPartiesFound)
{
    if (::Console::Online::PartyIsHost() && eResult == ::Console::Online::MMSEARCH_SUCCESS && nNumPartiesFound > 0)
    {
        // Join the parties we found, one at a time, until one accepts us.
        sgtMainMenuGlobals.bQuickmatchJoining = TRUE;
        sgtMainMenuGlobals.uNumMatchmakerParties = nNumPartiesFound;
        ::Console::Online::PartyJoin(sgtMainMenuGlobals.arMatchmakerParties[0], ::Console::Online::JOIN_TYPE_MATCHMAKE,
            OnlineService::internal::function_binder_5<void, uint32, Act, SNO, int32, ::Console::Online::JoinResult>::bind<&OnMatchmakingPartyJoinAttempt>(0, eLastAct, snoLastQuest, nLastQuestStep));
        return;
    }

    if (sgtMainMenuGlobals.idQuickmatchDialog != INVALID_MODAL_NOTIFICATION_ID)
    {
        ::Console::Screens::GetModalNotificationManager().RemoveNotification(sgtMainMenuGlobals.idQuickmatchDialog);
        sgtMainMenuGlobals.idQuickmatchDialog = INVALID_MODAL_NOTIFICATION_ID;
    }

    if (!::Console::Online::PartyIsHost())
        return;

    LPCSTR szMessage = "ConsoleErrors:MatchmakeUnknownError";

    switch (eResult)
    {
    case ::Console::Online::MMSEARCH_SUCCESS:
        {
            ::Console::Online::GameParams tParams;
            if (::Console::Online::PartyGetGameParams(tParams))
            {
                BOOL bSuccess = tParams.m_eAct != ACT_INVALID && (tParams.m_eAct == ACT_OPEN_WORLD || tParams.m_snoQuest != SNO_NONE);
                if (bSuccess)
                {
                    // The party has somewhere to go: start a public game ourselves.
                    ::Console::Online::PartyStartGame();
                    ::Console::Online::PartySwitchOnlineState(::Console::Online::PARTY_ONLINE_PUBLIC);
                    return;
                }
            }
            szMessage = "ConsoleErrors:MatchmakeNoGamesFound";
        }
        break;
    case ::Console::Online::MMSEARCH_CANCELLED:
        return;
    case ::Console::Online::MMSEARCH_DISCONNECTED:
        szMessage = "ConsoleErrors:OnlineStateErrorNoInternet";
        break;
    default:
        break;
    }

    NotificationCreationParams tParams;
    tParams.nControllerIndex = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    tParams.uszMessage = StringListFetch(szMessage);

    ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
    RestoreGameParamsAfterFailedMatch(eLastAct, snoLastQuest, nLastQuestStep);
}


















void OnMatchmakingCancelled(Act eLastAct, SNO snoLastQuest, int32 nLastQuestStep, const UIID & id)
{
    if (sgtMainMenuGlobals.bQuickmatchJoining)
        ::Console::Online::PartyJoinCancel();
    else
        ::Console::Online::PartyMatchmakeCancel();
    sgtMainMenuGlobals.bQuickmatchJoining = FALSE;
    sgtMainMenuGlobals.idQuickmatchDialog = INVALID_MODAL_NOTIFICATION_ID;

    if (::Console::Online::PartyIsHost())
    {
        ::Console::Online::GameParams tParams;
        if (::Console::Online::PartyGetGameParams(tParams))
        {
            tParams.m_eAct = eLastAct;
            tParams.m_snoQuest = snoLastQuest;
            tParams.m_nQuestStep = nLastQuestStep;
            ::Console::Online::PartySetGameParams(tParams);
        }
    }
}




void OnMatchmakingQuestSelectClosed(
    Act eLastAct,
    SNO snoLastQuest,
    int32 nLastQuestStep,
    BOOL bCancelled)
{
    if (!bCancelled)
    {

        NotificationCreationParams tParams;
        tParams.nControllerIndex = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
        tParams.tCallbacks.pfnOnClickCancelButton = OnlineService::internal::function_binder_4<void, int, int, int, const UIID &>::bind<&OnMatchmakingCancelled>(eLastAct, snoLastQuest, nLastQuestStep);
        tParams.uszTitle = StringListFetch("Console:MatchmakeGame");
        tParams.uszMessage = StringListFetch("Console:SearchingForGame");
        tParams.bShowBlinker = TRUE;


        tParams.uszCancelButtonText = "";

        sgtMainMenuGlobals.idQuickmatchDialog = ::Console::Screens::GetModalNotificationManager().SetCancelNotification(tParams);


        ::Console::Online::PartyMatchmake(
            sgtMainMenuGlobals.arMatchmakerParties,
            MAX_JOINABLE_GAMES,
            OnlineService::internal::function_binder_5<void, int, int, int, ::Console::Online::MMSearchResult, uint32>::bind<&OnMatchmakingFinished>(eLastAct, snoLastQuest, nLastQuestStep));
    }
}


void OnMatchmakingErrorAccepted(const UIID &)
{

    sgtMainMenuGlobals.idQuickmatchDialog = INVALID_MODAL_NOTIFICATION_ID;
}



void OnExportSaveMigrationComplete(int32 nControllerIndex, ::Console::SaveMigration::Result eResult)
{
    LPCSTR szMessage = "ConsoleErrors:SaveTransferError";
    switch (eResult)
    {
    case ::Console::SaveMigration::SAVE_MIGRATION_SUCCESS:
        szMessage = "Console:SaveTransferExportSuccess";
        break;
    case ::Console::SaveMigration::SAVE_MIGRATION_SUCCESS_SKIPPED:
    case ::Console::SaveMigration::SAVE_MIGRATION_SUCCESS_NOT_FOUND:
    case ::Console::SaveMigration::SAVE_MIGRATION_FAILED:
    case ::Console::SaveMigration::SAVE_MIGRATION_FAILED_DATA_CORRUPT:
        szMessage = "ConsoleErrors:SaveTransferError";
        break;
    case ::Console::SaveMigration::SAVE_MIGRATION_FAILED_SAVE_CORRUPT:
        szMessage = "ConsoleErrors:SaveTransferExportSaveCorruptError";
        break;
    case ::Console::SaveMigration::SAVE_MIGRATION_FAILED_UNOWNED_SAVE:
        szMessage = "ConsoleErrors:SaveTransferUnownedSaveError";
        break;
    case ::Console::SaveMigration::SAVE_MIGRATION_FAILED_NOT_SIGNED_IN:
        szMessage = "ConsoleErrors:SaveTransferExportNotSignedInErrorPSN";
        break;
    case ::Console::SaveMigration::SAVE_MIGRATION_FAILED_NO_PERMISSION:
        szMessage = "ConsoleErrors:SaveTransferExportOnlinePrivilegeRequiredErrorPSN";
        break;
    case ::Console::SaveMigration::SAVE_MIGRATION_FAILED_UNABLE_TO_CONNECT:
        szMessage = "ConsoleErrors:SaveTransferNotConnectedError";
        break;
    }

    NotificationCreationParams tParams;
    tParams.nControllerIndex = nControllerIndex;
    tParams.uszMessage = StringListFetch(szMessage);
    ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
}







void OnImportSaveMigrationComplete(int32 nControllerIndex, ::Console::SaveMigration::Result eResult)
{
    LPCSTR szMessage = "ConsoleErrors:SaveTransferError";
    switch (eResult)
    {
    case ::Console::SaveMigration::SAVE_MIGRATION_SUCCESS:
        szMessage = "Console:SaveTransferImportSuccess";
        break;
    case ::Console::SaveMigration::SAVE_MIGRATION_SUCCESS_SKIPPED:
        return;
    case ::Console::SaveMigration::SAVE_MIGRATION_SUCCESS_NOT_FOUND:
        szMessage = "ConsoleErrors:SaveTransferNoSaveFoundError";
        break;
    case ::Console::SaveMigration::SAVE_MIGRATION_FAILED_DATA_CORRUPT:
        szMessage = "ConsoleErrors:SaveTransferImportDataCorruptError";
        break;
    case ::Console::SaveMigration::SAVE_MIGRATION_FAILED_SAVE_CORRUPT:
        szMessage = "ConsoleErrors:SaveTransferImportSaveCorruptError";
        break;
    case ::Console::SaveMigration::SAVE_MIGRATION_FAILED_UNOWNED_SAVE:
        szMessage = "ConsoleErrors:SaveTransferUnownedSaveError";
        break;
    case ::Console::SaveMigration::SAVE_MIGRATION_FAILED_NOT_SIGNED_IN:
        szMessage = "ConsoleErrors:SaveTransferImportNotSignedInErrorNX64";
        break;
    case ::Console::SaveMigration::SAVE_MIGRATION_FAILED_NO_PERMISSION:
        szMessage = "ConsoleErrors:SaveTransferImportOnlinePrivilegeRequiredErrorPSN";
        break;
    case ::Console::SaveMigration::SAVE_MIGRATION_FAILED_UNABLE_TO_CONNECT:
        szMessage = "ConsoleErrors:SaveTransferNotConnectedError";
        break;
    default:
        break;
    }

    NotificationCreationParams tParams;
    tParams.nControllerIndex = nControllerIndex;
    tParams.uszMessage = StringListFetch(szMessage);
    ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
}














void OnImportSaveAccepted(
    const int32 nControllerIndex,
    const UIID & )
{
    ::Console::SaveMigration::ImportSave(nControllerIndex, OnlineService::internal::function_bind_1_1<void, int, ::Console::SaveMigration::Result, &OnImportSaveMigrationComplete>(nControllerIndex));
}




void OnDifficultyChosen(BOOL bCanceled)
{
    if (!bCanceled)
        OnStartOrContinue(FALSE, FALSE);
}





void OnGameSettingsConfirmed(
    BOOL bCanceled)
{
    if (!bCanceled)
    {
        Refresh3DBackgroundHeroes();
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_GROUP).SetSelectedIndex(0);
    }
}


void sHandleStartOrContinueMenu(int32 nControllerIndex)
{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        OnButton4(UIID(0), nControllerIndex);
    }
    else if (IsAtStartOfGame() && ::Console::Online::PartyIsHost() && !UIGameSettings::Console::GameSettingsViewed())
    {
        UIGameSettings::Console::OpenForDifficultySelect(nControllerIndex, OnlineService::internal::function_bind_0_1<void, int, &OnDifficultyChosen>());
    }
    else
    {
        OnStartOrContinue(FALSE, FALSE);
    }
}



void OpenStoreToNecromancer(int32 nControllerIndex)
{
    NotificationCreationParams tParams;
    tParams.nControllerIndex = nControllerIndex;
    tParams.uszMessage = StringListFetch("Console:PleaseWait");
    ModalNotificationId idDialog = ::Console::Screens::GetModalNotificationManager().SetMessageOnlyNotification(tParams);
    ::Console::Utilities::ShowStoreUI(::Console::GamerProfile::GetPrimaryProfileUserIndex(), "TODO", false,
        [idDialog]() {
            ::Console::Screens::GetModalNotificationManager().RemoveNotification(idDialog);
        });
}








void sConfirmExportSave(
    const int32 nUserIndex,
    const UIID & )
{
    ::Console::SaveMigration::ExportSave(nUserIndex, OnlineService::internal::function_bind_1_1<void, int, ::Console::SaveMigration::Result, &OnExportSaveMigrationComplete>(nUserIndex));
}


















static void sHandleConnectToDemonware(const int32 nControllerIndex)
{

    if (::Console::GamerProfile::IsActive(nControllerIndex) &&
        !::Console::GamerProfile::IsGuest(nControllerIndex))
    {
        if (!::Console::Online::IsPhysicalNetworkConnected())
        {
            ::Console::Online::sForceSubmitNetworkRequest();
        }
        else if (::Console::GamerProfile::IsSignedInOnline(nControllerIndex))
        {
            ::Console::Online::LobbyService::AttemptConnectOnUser(nControllerIndex);
        }
        else
        {
            ::Console::GamerProfile::EnsureNetworkServiceAccountForUser(nControllerIndex);
        }
    }
}


static void sHandleNetworkOptions(const int32 nControllerIndex)
{

    if (::Console::Online::PartyIsInParty() &&
        ::Console::Online::PartyIsHost() &&
        !::Console::Screens::IsScreenTransitioning())
    {
        UINetworkOptions::Console::ShowNetworkOptions(nControllerIndex);
    }
}

void OnAccept(
    const UIID & ,
    const int32 nControllerIndex)
{
    if (::Console::Screens::IsScreenTransitioning())
        return;



    if (UI_GetControllerFocus() == UILIST_CONSOLE_MENU_GROUP)
    {
        UIConsoleListBox & tMenu = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_GROUP);
        if (tMenu.GetSelectedIndex() == -1)
            return;



        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_OPEN), cgtPSODefaults);
        switch (sgtMainMenuGlobals.tListSource.m_menuItems[tMenu.GetSelectedIndex()])
        {

        case OPTION_CONTINUE:
            sHandleStartOrContinueMenu(nControllerIndex);
            break;

        case OPTION_GAME_SETTINGS:
        {
            UIGameSettings::Console::Open(nControllerIndex, OnlineService::internal::function_bind_0_1<void, int, &OnGameSettingsConfirmed>());
            break;
        }

        case OPTION_MATCHMAKE:
        {
            // a local per DWARF (decl line 2355)
            ::Console::UIOnlineActions::GameStartValidationError eReason = ::Console::UIOnlineActions::ValidateGameCanStart();
            if (eReason == ::Console::UIOnlineActions::GAME_START_OK)
            {
                Act eLastAct = ACT_1;  // defaults: DWARF const_value 0 / -1 for eLastAct / snoLastQuest; the original materialises -1 for the step too
                SNO snoLastQuest = SNO_NONE;
                int32 nLastQuestStep = -1;
                ::Console::Online::GameParams tGameParams;
                if (::Console::Online::PartyGetGameParams(tGameParams))
                {
                    eLastAct = tGameParams.m_eAct;
                    snoLastQuest = tGameParams.m_snoQuest;
                    nLastQuestStep = tGameParams.m_nQuestStep;
                }
                UIGameSettings::Console::OpenForMatchmaking(nControllerIndex, OnlineService::internal::function_binder_4<void, int, int, int, int>::bind<&OnMatchmakingQuestSelectClosed>(eLastAct, snoLastQuest, nLastQuestStep));
            }
            else
            {
                CRefString sErrorMessage = ::Console::UIOnlineActions::GetGameStartErrorMessage(
                    eReason,
                    ::Console::Online::PartyIsHost(),
                    ::Console::Online::PartyCanStartGame(),
                    TRUE);
                NotificationCreationParams tParams;
                tParams.nControllerIndex = nControllerIndex;
                tParams.uszMessage = sErrorMessage;
                tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &OnMatchmakingErrorAccepted>();
                sgtMainMenuGlobals.idQuickmatchDialog = ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
            }
            break;
        }






        case OPTION_FIND_GAME:
        {
            UIGamesList::Console::OpenForController(nControllerIndex);
            break;
        }
        case OPTION_OPTIONS:
        {
            UIPartyDisplay::Console::HideFriendDisplay();
            UIPartyDisplay::Console::HideLocalCommunicationsDisplay();
            ::Console::Screens::GetActionManager().ClearAllActions();
            ::Console::Screens::UpdateButtonLegendText();
            ::Console::Screens::OpenToScreen(::Console::Screens::OPTIONS_SCREEN_SET, TRUE, nControllerIndex, FALSE);
            break;
        }



        case OPTION_CREATE_SEASONAL:
        {
            uint32 uNumHeroes = ::Console::GamerProfile::GetNumAvailableHeroes(nControllerIndex);
            uint32 uMaxHeroes = GetConsoleMaxHeroesAllowed(nControllerIndex);

            if (uNumHeroes >= uMaxHeroes)
            {
                NotificationCreationParams tParams;
                tParams.uszMessage = StringListFetch("Console:NoSlotsForSeasonal");
                tParams.nControllerIndex = nControllerIndex;
                ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
                return;
            }

            UIPartyDisplay::Console::HideFriendDisplay();
            UIPartyDisplay::Console::HideLocalCommunicationsDisplay();
            ::Console::Screens::GetActionManager().ClearAllActions();
            ::Console::Screens::UpdateButtonLegendText();
            ::Console::Screens::OpenToScreen(::Console::Screens::HERO_CREATE_SCREEN_SET, FALSE, nControllerIndex, FALSE);
            break;
        }





        case OPTION_SEASON_JOURNEY:
        {
            UISeasonJourney::Console::OpenForController(nControllerIndex);
            break;
        }
        case OPTION_VIEW_WHATS_NEW:
        {
            UIWhatsNew::Console::OpenForController(nControllerIndex, FALSE, FALSE);
            break;
        }
        case OPTION_CREDITS_MENU:
        {
            sgtMainMenuGlobals.tListSource.m_eMenuLevel = MenuLevel::CREDITS;
            RefreshMainMenu(FALSE);
            tMenu.SetSelectedIndex(0);
            break;
        }
        case OPTION_CREDITS_ROS:
        {
            UICredits::Console::Show(nControllerIndex);
            UICredits::ExpansionCredits();
            break;
        }
        case OPTION_CREDITS_2017:
        {
            UICredits::Console::Show(nControllerIndex);
            UICredits::Credits2017();
            break;
        }

        case OPTION_CREDITS_ETERNAL:
        {
            UICredits::Console::Show(nControllerIndex);
            UICredits::CreditsEternalCollection();
            break;
        }


        case OPTION_CINEMATICS:
        {
            if (!SNOPackChunkLoadAct(ACT_OPEN_WORLD))
            {
                NotificationCreationParams tParams;
                tParams.uszMessage = LocalizedSprintf(
                    StringListFetch("ConsoleErrors:CannotProceedGenericContentNotReady").str(),
                    UnicodeText::FormatNumber(SNOPackChunkGetActLoadPercentComplete(ACT_OPEN_WORLD) * 100.0f, TRUE, FNS_NORMAL, 2, 2, 0).str(),
                    NULL);
                ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
                return;
            }

            UIPartyDisplay::Console::HideFriendDisplay();
            UIPartyDisplay::Console::HideLocalCommunicationsDisplay();
            ::Console::Screens::GetActionManager().ClearAllActions();
            ::Console::Screens::UpdateButtonLegendText();
            ::Console::Screens::OpenToScreen(::Console::Screens::CINEMATICS_SCREEN_SET, FALSE, nControllerIndex, FALSE);
            break;
        }



        case OPTION_LEAVE_PARTY:
        {
            if (::Console::Online::PartyIsInParty() && !::Console::Online::PartyIsHost())
            {
                ::Console::Online::PartyLeaveAndRemake(1);
            }
            break;
        }
        case OPTION_CUSTOMIZE_BANNER:
        {
            UIBannerCustomization::Console::OpenForController(nControllerIndex);
            break;
        }
        case OPTION_VIEW_PROFILE:
        {
            UIProfileSummary::Console::OpenForController(nControllerIndex);
            break;
        }
        case OPTION_HALL_OF_FALLEN:
        {
            UIHallOfFallenHeroes::Console::OpenForController(nControllerIndex);
            break;
        }
        case OPTION_CHALLENGES:
        {
            UIAchievements::Console::OpenForController(nControllerIndex);
            break;
        }
        case OPTION_LEADERBOARDS:
        {
            if (::Console::GamerProfile::IsSignedInOnline(::Console::GamerProfile::GetPrimaryProfileUserIndex()))
            {
                UILeaderboards::Console::OpenForController(nControllerIndex);
            }
            break;
        }
        case OPTION_MORE:
        {
            sgtMainMenuGlobals.tListSource.m_eMenuLevel = MenuLevel::MORE;
            RefreshMainMenu(FALSE);
            tMenu.SetSelectedIndex(0);
            break;
        }
        case OPTION_EXPORT_SAVE:
        {
            NotificationCreationParams tParams;
            tParams.nControllerIndex = nControllerIndex;
            tParams.uszMessage = StringListFetch("Console:SaveTransferExportConfirm");

            tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_1_1<void, int, const UIID &, &sConfirmExportSave>(nControllerIndex);

            ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
            break;
        }
        case OPTION_IMPORT_SAVE:
        {
            NotificationCreationParams tParams;
            tParams.nControllerIndex = nControllerIndex;
            if (::Console::Online::PartyIsHost())
            {
                tParams.tAcceptButtonDelay = 3000;
                tParams.uszMessage = StringListFetch("Console:SaveTransferImportDestructiveWarning");
                tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_1_1<void, int, const UIID &, &OnImportSaveAccepted>(nControllerIndex);
                sgtMainMenuGlobals.idImportSaveDialog = ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
            }
            else
            {
                tParams.uszMessage = StringListFetch("ConsoleErrors:SaveTransferImportInvalidState");
                ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
            }
            break;
        }
        case OPTION_QUIT_DEMO:
        {
            OnQuitDemo(nControllerIndex);
            break;
        }



            // these options have no line rows; their jump-table entries go straight to the return
        case OPTION_UNLOCK_FULL_GAME:
        case OPTION_HELP:
        case OPTION_INSTALL:
            break;











        case OPTION_NECRO_UPSELL:
        {
            OpenStoreToNecromancer(nControllerIndex);
            break;
        }

        case OPTION_NETWORK_SETTINGS:
        {
            sHandleNetworkOptions(nControllerIndex);
            break;
        }


        case OPTION_CONNECT_DEMONWARE:
        {
            sHandleConnectToDemonware(nControllerIndex);
            break;
        }
        default:

            Cancel(nControllerIndex);
        }
    }
    else if (UI_GetControllerFocus() == UILIST_CONSOLE_MENU_PARTY)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
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
    else if (UIPartyDisplay::Console::IsNamePlateSelected())
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        UIPartyDisplay::Console::OnPartyNamePlateAcceptPressed(nControllerIndex);
    }
}


void OnLocalJoinButtonPressed(const UIID &, const int32 nControllerIndex)
{
    ::Console::UIOnlineActions::LocalPlayerJoin(nControllerIndex, FALSE);
}




void OnLocalLeaveButtonPressed(const UIID &, const int32 nControllerIndex)
{
    if (::Console::Screens::IsScreenTransitioning())
        return;

    if (::Console::GamerProfile::GetPrimaryProfileUserIndex() != nControllerIndex && ::Console::Online::PartyIsInParty(nControllerIndex))
    {
        ::Console::Online::PartyRemovePlayer(nControllerIndex);
        ::Console::GamerProfile::SignOut(nControllerIndex);
    }
}



























void UpdateLogoText()
{
    CRefString usText;
    if (sgtMainMenuGlobals.tListSource.m_eMenuLevel == MenuLevel::CREDITS)
        usText = StringListFetch("Console:CreditsMenuHeader");
    else
        usText = StringListFetch("Console:MainMenu");
    UI_GetTextFromID(UITEXT_CONSOLE_MAIN_MENU_LOGO_DESCRIPTION).SetText(usText, FALSE);
}

void UpdateQuestPrefetch();
void UpdateCommunityEventsDisplay();

// NON_MATCHING: global layout: original keeps sbIsShowingConnectOption/sbHaveSeasonData as separate bytes and sbHeroIsSeasonal in the merged block; ours merges differently. bSeasonDataStateKnown restored as the DWARF local (2815, const 0 then 1); size 2336 vs 2344; original has an extra bSeasonActive test near 2905 (adding it to the conditions gave 2388).





void OnUpdate(const UIID &)
{
    UpdateLogoText();

    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        UIPartyDisplay::Console::RefreshFriendsDisplay();
        UIPartyDisplay::Console::RefreshLocalCommunicationsDisplay();
    }
    UIPartyDisplay::Console::RefreshPartyNameplates();

    UpdateQuestPrefetch();










    if (((::Console::DLCManager::GetLicenses() & 0x80) != 0) != sbHaveNecroLicense)
    {
        sbHaveNecroLicense = (::Console::DLCManager::GetLicenses() & 0x80) != 0;
        RefreshMainMenu(FALSE);
    }


    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (!::Console::Screens::IsModalDialogVisible() && !::Console::Screens::IsScreenTransitioning())
    {
        tActionManager.ClearAllActions();
        D3::Hero::SavedDefinition & tHero = ::Console::GamerProfile::GetHero(::Console::GamerProfile::GetPrimaryProfileUserIndex());

        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_CONFIRM, StringListFetch("Console:Accept"));

        if (sgtMainMenuGlobals.tListSource.m_eMenuLevel != MenuLevel::MAIN &&
            !UIPartyDisplay::Console::IsNamePlateSelected() &&
            UI_GetControllerFocus() == UILIST_CONSOLE_MENU_GROUP)
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("Console:Back"));
        }
        else
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsolePause:Quit"));
        }

        if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_SWITCH_HERO, StringListFetch("Console:HeroList"));
        }

        if (::Console::Online::PartyIsHost() && !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {

            int32 nProfileUserIdx = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
            if (::Console::Online::GetNetworkMode() != ::Console::Online::NET_ADHOC &&
                ::Console::GamerProfile::IsSignedInOnline(nProfileUserIdx) &&
                ::Console::Online::LobbyService::IsUserConnected(nProfileUserIdx))
            {


                CRefString sNetworkOptions = StringListFetch("Console:GamePrivacyOptionsNX64");
                ::Console::Online::PartyOnlineState ePreferredState = ::Console::Online::PartyGetPreferredOnlineState();
                sNetworkOptions.Format("%s%s", sNetworkOptions.str(), UIUtils::Console::DisplayPartyOnlineStateIcon(ePreferredState, 0, 2.5f).str());





                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BACK,
                                         ACTION_NETWORK_OPTIONS,
                                         sNetworkOptions);
            }
        }

        if (::Console::Online::PartyIsInParty() &&
            ::Console::Online::PartyIsOnline() &&
            !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) &&

            UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).GetNumItems() > 0 &&
            UI_GetControllerFocus() != UILIST_CONSOLE_FRIENDS_LIST)
        {



            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_INVITE, StringListFetch("Console:InviteFriendAlt"));
        }

        else if (::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ADHOC &&
                 (UI_GetControllerFocus() == UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST ||
                  UI_GetControllerFocus() == UITEXT_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS))
        {

            CRefString sRefreshOptionString = StringListFetch("Console:AdHocRefreshListNX64");
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3,
                                     ACTION_REFRESH_ADHOC_LIST,
                                     sRefreshOptionString);
        }
        else
        {

            tActionManager.ClearAction(::Console::CONTROLLER_INPUT_BUTTON_3);
            tActionManager.ClearAction(::Console::CONTROLLER_INPUT_LEFT_SHOULDER);
            tActionManager.ClearAction(::Console::CONTROLLER_INPUT_RIGHT_SHOULDER);
        }
    }

    UpdateCommunityEventsDisplay();
    UIText & tSeasonStateText = UI_GetTextFromID(UITEXT_CONSOLE_MAIN_MENU_SEASON_STATE);
    BOOL bSeasonDataStateKnown = FALSE;  // DWARF 2815: constant 0, then 1 once the season state is known
    BOOL bSeasonActive = OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE;
    UI_GetWindowFromID(UIWND_CONSOLE_MAIN_MENU_ONLINE_STATE).SetActive(TRUE);
    UIText & tOnlineStateMessage = UI_GetTextFromID(UITEXT_CONSOLE_MAIN_MENU_ONLINE_STATE_MESSAGE);
    int32 PrimaryUserIndex = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    if (::Console::Online::LobbyService::IsUserConnected(PrimaryUserIndex))
    {
        UI_GetWindowFromID(UIWND_CONSOLE_MAIN_MENU_ONLINE_STATE_OFFLINE_ICON).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_MAIN_MENU_ONLINE_STATE_ONLINE_ICON).SetActive(TRUE);
        tOnlineStateMessage.SetText(StringListFetch("Console:ConnectedDiabloNetworkNX64"), FALSE);
    }
    else
    {
        UI_GetWindowFromID(UIWND_CONSOLE_MAIN_MENU_ONLINE_STATE_OFFLINE_ICON).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_CONSOLE_MAIN_MENU_ONLINE_STATE_ONLINE_ICON).SetActive(FALSE);
        tOnlineStateMessage.SetText(StringListFetch("Console:DisconnectedDiabloNetworkNX64"), FALSE);
    }












    if (sgtMainMenuGlobals.tListSource.m_eMenuLevel == MenuLevel::MAIN && ::Console::Online::LobbyService::ShouldShowReconnectOption(PrimaryUserIndex) != sbIsShowingConnectOption)
    {
        RefreshMainMenu(FALSE);
    }




    ::Console::Online::GameParams tGameParams;
    if (::Console::Online::PartyGetGameParams(tGameParams) &&
        tGameParams.m_eGameType == GT_CHALLENGE_RIFTS)
    {
        tSeasonStateText.SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CONSOLE_MAIN_MENU_SEASON_STATE_ICON).SetActive(FALSE);
        UI_GetWindowFromID(UITEXT_CONSOLE_MAIN_MENU_SEASON_HERO_REQUIRED).SetActive(TRUE);
    }
    else
    {





        UI_GetWindowFromID(UIWND_CONSOLE_MAIN_MENU_SEASON_STATE_ICON).SetActive(TRUE);
        tSeasonStateText.SetActive(TRUE);
        if (!::Console::Online::IsSeasonsInitialized())

        {
            if (!::Console::GamerProfile::IsSignedInOnline(::Console::GamerProfile::GetPrimaryProfileUserIndex()))
            {
                tSeasonStateText.SetText(StringListFetch("Console:SeasonStateNotSignedIn"), FALSE);
            }
            else
            {

                tSeasonStateText.SetText(StringListFetch("Console:SeasonStateUnknown"), FALSE);
            }
        }

        else if (bSeasonActive)
        {
            tSeasonStateText.SetText(LocalizedSprintf(StringListFetch("Console:SeasonStateInProgress"), UnicodeText::FormatNumber((int32)OnlineService::GetSeasonNum(), TRUE, FNS_NORMAL, 0).str()), FALSE);
            bSeasonDataStateKnown = TRUE;  // DWARF: constant 1 after this branch (no row of its own)
        }
        else
        {

            tSeasonStateText.SetText(StringListFetch("Console:SeasonStateEnded"), FALSE);

        }

        if (bSeasonDataStateKnown && !sbHaveSeasonData)
        {
            sbHaveSeasonData = TRUE;
            sCheckForSeasonalHero(::Console::GamerProfile::GetPrimaryProfileUserIndex());
            RefreshMainMenu(FALSE);
        }
        else if (!bSeasonDataStateKnown && sbHaveSeasonData)
        {
            sbHaveSeasonData = FALSE;
            RefreshMainMenu(FALSE);
        }



        if (bSeasonDataStateKnown && !sbHeroIsSeasonal)
        {


            if (!sbHasSeasonalHero)
            {
                UI_GetTextFromID(UITEXT_CONSOLE_MAIN_MENU_SEASON_HERO_REQUIRED).SetText(StringListFetch("Console:SeasonStateNonSeasonalHeroLoggedIn"), FALSE);
            }
            else
            {
                UI_GetTextFromID(UITEXT_CONSOLE_MAIN_MENU_SEASON_HERO_REQUIRED).SetText(StringListFetch("Console:SeasonStateNonSeasonalHeroSelected"), FALSE);
            }
            UI_GetWindowFromID(UITEXT_CONSOLE_MAIN_MENU_SEASON_HERO_REQUIRED).SetActive(TRUE);
        }

        UI_GetWindowFromID(UITEXT_CONSOLE_MAIN_MENU_SEASON_HERO_REQUIRED).SetActive(bSeasonActive && !sbHeroIsSeasonal);
    }

    ::Console::Screens::UpdateButtonLegendText();
}


void Close()

{
    ::Console::Online::GetPartyMemberAddedEvent().Unsubscribe(OnlineService::internal::function_bind_0_1<void, int, &OnEventPartyMemberAdded>());
    ::Console::Online::GetPartyMemberReceivedDigestEvent().Unsubscribe(OnlineService::internal::function_bind_0_1<void, int, &OnEventPartyMemberAdded>());
    ::Console::Online::GetPartyMemberRemovedEvent().Unsubscribe(OnlineService::internal::function_bind_0_1<void, int, &OnEventPartyMemberRemoved>());
    ::Console::Online::GetPartyJoinedEvent().Unsubscribe(OnlineService::internal::function_bind_0_0<void, &OnEventPartyJoined>());
    ::Console::Online::GetPartyGameInfoChangedEvent().Unsubscribe(OnlineService::internal::function_bind_0_0<void, &OnEventPartyGameInfoChanged>());
    ::Console::Online::GetPartyOnlineStateChangedEvent().Unsubscribe(OnlineService::internal::function_bind_0_1<void, ::Console::Online::PartySwitchOnlineResult, &OnEventPartyOnlineStateChanged>());
    ::Console::Online::GetPartyMigratingEvent().Unsubscribe(OnlineService::internal::function_bind_0_1<void, ::Console::Online::MigrationEvent, &OnEventPartyMigrated>());

    UIPartyDisplay::Console::EnablePartyNameplates(FALSE);
    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        UIPartyDisplay::Console::HideFriendDisplay();
        UIPartyDisplay::Console::HideLocalCommunicationsDisplay();
        UIGameSettings::Console::Close();
        UIGamesList::Console::Close();
        UIBannerCustomization::Console::Close();
        UIAchievements::Console::Close();
        UIProfileSummary::Console::Close();
        UIHallOfFallenHeroes::Console::Close();
        UINetworkOptions::Console::CloseNetworkOptions();

        UIGamePrivacyOptions::Console::CloseGamePrivacyOptions();
    }

    if (sgtMainMenuGlobals.idQuickmatchDialog != INVALID_MODAL_NOTIFICATION_ID)
    {
        ::Console::Screens::GetModalNotificationManager().RemoveNotification(sgtMainMenuGlobals.idQuickmatchDialog);
        sgtMainMenuGlobals.idQuickmatchDialog = INVALID_MODAL_NOTIFICATION_ID;
    }
    if (sgtMainMenuGlobals.idStartGameDialog != INVALID_MODAL_NOTIFICATION_ID)
    {
        ::Console::Screens::GetModalNotificationManager().RemoveNotification(sgtMainMenuGlobals.idStartGameDialog);
        sgtMainMenuGlobals.idStartGameDialog = INVALID_MODAL_NOTIFICATION_ID;
    }
    if (sgtMainMenuGlobals.idImportSaveDialog != INVALID_MODAL_NOTIFICATION_ID)
    {
        ::Console::Screens::GetModalNotificationManager().RemoveNotification(sgtMainMenuGlobals.idImportSaveDialog);
        sgtMainMenuGlobals.idImportSaveDialog = INVALID_MODAL_NOTIFICATION_ID;
    }
    UI_SetControllerFocus(GetUIIDNone());
    UI_GetControlFromID(UIWND_CONSOLE_MAIN_MENU).SetActive(FALSE);
    ClearHighlightRActor();




}


void ResetMainMenuFlag()
{
    sgtMainMenuGlobals.tListSource.m_eMenuLevel = MenuLevel::MAIN;
}



void OnCancelAccepted(const int32 nControllerIndex, const UIID &)
{
    Cancel(nControllerIndex);
}




void OnCancel(const UIID &, const int32 nControllerIndex)
{
    if (::Console::Screens::IsScreenTransitioning())
        return;

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);

    if (UI_GetControllerFocus() == UILIST_CONSOLE_MENU_GROUP && sgtMainMenuGlobals.tListSource.m_eMenuLevel != MenuLevel::MAIN)
    {
        MenuOption eSelectOption = OPTION_MORE;
        if (sgtMainMenuGlobals.tListSource.m_eMenuLevel == MenuLevel::CREDITS)
        {
            sgtMainMenuGlobals.tListSource.m_eMenuLevel = MenuLevel::MORE;
            eSelectOption = OPTION_CREDITS_MENU;
        }
        else if (sgtMainMenuGlobals.tListSource.m_eMenuLevel == MenuLevel::MORE)
        {
            if (::Console::Online::IsSeasonsInitialized() && OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE && sbHaveSeasonData)
            {
                int32 & nSeenSeason = Prefs::GetLeaderboardSeenSeason();
                if ((uint32)nSeenSeason < OnlineService::GetSeasonNum())
                {
                    nSeenSeason = OnlineService::GetSeasonNum();
                    ::Console::GamerProfile::SavePrefs();
                }
            }
            sgtMainMenuGlobals.tListSource.m_eMenuLevel = MenuLevel::MAIN;
        }

        RefreshMainMenu(FALSE);
        UIConsoleListBox & tMenu = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_GROUP);
        int32 nSelectedIndex = 0;
        for (int32 i = 0; i < (int32)sgtMainMenuGlobals.tListSource.m_menuItems.size(); i++)
        {
            if (sgtMainMenuGlobals.tListSource.m_menuItems[i] == eSelectOption)
            {
                nSelectedIndex = i;
                break;
            }
        }
        tMenu.SetSelectedIndex(nSelectedIndex);
        return;
    }

    gtTutorialMgr.CloseTutorialIfOpen(GlobalSNOGet(GLOBALSNO_TUTORIAL_GAME_SETTINGS));
    NotificationCreationParams tParams;
    tParams.nControllerIndex = nControllerIndex;
    tParams.uszMessage = StringListFetch("Console:BackToStartMessage");
    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_1_1<void, int, const UIID &, &OnCancelAccepted>(nControllerIndex);
    ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
}











void OnButton3(const UIID &, const int32 nControllerIndex)
{
    if (!::Console::Screens::IsScreenTransitioning() && ::Console::Online::PartyIsInParty() && ::Console::Online::PartyIsOnline() &&
        !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        if (UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).GetNumItems() > 0 && UI_GetControllerFocus() != UILIST_CONSOLE_FRIENDS_LIST)
        {
            UIPartyDisplay::Console::ClearNamePlateSelection();
            UI_GetRActorFromID(UIRACTOR_CONSOLE_MAIN_MENU_HIGHLIGHT).Cleanup();
            UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_GROUP).SetSelectedIndex(-1);
            UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).SetSelectedIndex(0);
            UI_SetControllerFocus(UILIST_CONSOLE_FRIENDS_LIST);
        }
    }
    else if (::Console::Online::PartyGetOnlineState() == ::Console::Online::PARTY_ADHOC &&
             (UI_GetControllerFocus() == UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST || UI_GetControllerFocus() == UITEXT_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS))
    {
        ::Console::Online::RefreshAdHocList();
    }
}



















void OnButton4(const UIID & id, const int32 nControllerIndex)
{
    if (::Console::Screens::IsScreenTransitioning())
        return;

    if (::Console::Online::PartyIsInParty() && !::Console::Online::PartyIsInParty(nControllerIndex))
        return;

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        UIPartyDisplay::Console::HideFriendDisplay();
        UIPartyDisplay::Console::HideLocalCommunicationsDisplay();
    }
    ::Console::Screens::GetActionManager().ClearAllActions();
    ::Console::Screens::UpdateButtonLegendText();
    ::Console::Screens::OpenToScreen(23, FALSE, nControllerIndex, FALSE);
}














void OnBackButton(const UIID & id, const int32 nControllerIndex)
{
    if (::Console::Online::PartyIsInParty() && ::Console::Online::PartyIsHost() && ::Console::Online::PartyGetOnlineState() != ::Console::Online::PARTY_ADHOC &&
        !::Console::Screens::IsScreenTransitioning() && ::Console::GamerProfile::IsSignedInOnline(nControllerIndex) &&
        ::Console::Online::LobbyService::IsUserConnected(nControllerIndex))
    {
        UIGamePrivacyOptions::Console::ShowGamePrivacyOptions(nControllerIndex);
    }
}



void OnR3(const UIID &, const int32 nControllerIndex)
{
}












void OnLeftShoulder(const UIID & id, const int32 nControllerIndex)
{
    if (::Console::Online::PartyIsInParty() && ::Console::Online::PartyIsOnline())
    {
        if (::Console::Screens::IsScreenTransitioning())
            return;
    }
}








void OnRightShoulder(const UIID &, const int32 nControllerIndex)
{
    if (::Console::Online::PartyIsInParty() && ::Console::Online::PartyIsOnline())
    {
        if (::Console::Screens::IsScreenTransitioning())
            return;
    }
}






void OnLeftStick(const UIID &, const int32 nControllerIndex)
{
    Vector2D vecStick = ::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE);
    switch (sgtMainMenuGlobals.tStickFilter.HandleInput(vecStick, FALSE))
    {
    case ::Console::RULE_STICK_LEFT:
        OnDPadLeft(GetUIIDNone(), nControllerIndex);
        break;
    case ::Console::RULE_STICK_RIGHT:
        OnDPadRight(GetUIIDNone(), nControllerIndex);
        break;
    }
}



void OnDPadLeft(const UIID & id, const int32 nControllerIndex)
{
    if (::Console::Screens::IsScreenTransitioning())
        return;
    if (UI_GetControllerFocus() == UILIST_CONSOLE_PLAYER_OPTIONS && !UIPartyDisplay::Console::IsNamePlateSelected())
        return;

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_FOCUS_SWITCH), cgtPSODefaults);

    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && UI_GetControllerFocus() == UILIST_CONSOLE_MENU_GROUP)
    {
        if (::Console::Online::GetNetworkMode() == ::Console::Online::NET_ADHOC)
        {
            if (UIPartyDisplay::Console::IsLocalCommunicationDisplayVisible() && UIPartyDisplay::Console::CanDisplayLocalCommunicationDisplay())
            {
                UI_GetRActorFromID(UIRACTOR_CONSOLE_MAIN_MENU_HIGHLIGHT).Cleanup();
                UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_GROUP).SetSelectedIndex(-1);
                if (UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST).GetNumItems() > 0)
                {
                    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST).SetSelectedIndex(0);
                    UI_SetControllerFocus(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST);
                }
                else
                {
                    UI_SetControllerFocus(UITEXT_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS);
                }
                return;
            }
        }
        else if (UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).GetNumItems() > 0)
        {
            UI_GetRActorFromID(UIRACTOR_CONSOLE_MAIN_MENU_HIGHLIGHT).Cleanup();
            UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_GROUP).SetSelectedIndex(-1);
            UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).SetSelectedIndex(0);
            UI_SetControllerFocus(UILIST_CONSOLE_FRIENDS_LIST);
            return;
        }
    }
    else if (UI_GetControllerFocus() == UILIST_CONSOLE_FRIENDS_LIST || UI_GetControllerFocus() == UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST ||
             UI_GetControllerFocus() == UITEXT_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS)
    {
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).SetSelectedIndex(-1);
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST).SetSelectedIndex(-1);
        if (!UIPartyDisplay::Console::SelectPreviousNameplate())
            FocusMainMenu();
        return;
    }

    if (UIPartyDisplay::Console::SelectPreviousNameplate())
    {
        UI_GetRActorFromID(UIRACTOR_CONSOLE_MAIN_MENU_HIGHLIGHT).Cleanup();
        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_GROUP).SetSelectedIndex(-1);
    }
    else
    {
        FocusMainMenu();
    }
}






























void OnDPadRight(const UIID & id, const int32 nControllerIndex)
{
    if (::Console::Screens::IsScreenTransitioning())
        return;
    if (UI_GetControllerFocus() == UILIST_CONSOLE_PLAYER_OPTIONS && !UIPartyDisplay::Console::IsNamePlateSelected())
        return;

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_FOCUS_SWITCH), cgtPSODefaults);

    if (UI_GetControllerFocus() != UILIST_CONSOLE_FRIENDS_LIST && UI_GetControllerFocus() != UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST &&
        UI_GetControllerFocus() != UITEXT_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS)
    {
        if (UIPartyDisplay::Console::SelectNextNameplate())
        {
            UI_GetRActorFromID(UIRACTOR_CONSOLE_MAIN_MENU_HIGHLIGHT).Cleanup();
            UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_GROUP).SetSelectedIndex(-1);
            return;
        }

        if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            if (::Console::Online::GetNetworkMode() == ::Console::Online::NET_ADHOC)
            {
                if (UIPartyDisplay::Console::IsLocalCommunicationDisplayVisible() && UIPartyDisplay::Console::CanDisplayLocalCommunicationDisplay())
                {
                    UIPartyDisplay::Console::ClearNamePlateSelection();
                    UI_GetRActorFromID(UIRACTOR_CONSOLE_MAIN_MENU_HIGHLIGHT).Cleanup();
                    UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_GROUP).SetSelectedIndex(-1);
                    if (UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST).GetNumItems() > 0)
                    {
                        UI_GetConsoleListBoxFromID(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST).SetSelectedIndex(0);
                        UI_SetControllerFocus(UILIST_CONSOLE_LOCAL_COMMUNICATION_LIST);
                    }
                    else
                    {
                        UI_SetControllerFocus(UITEXT_CONSOLE_LOCAL_COMMUNICATION_NO_PLAYERS);
                    }
                    return;
                }
            }
            else if (UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).GetNumItems() > 0)
            {
                UIPartyDisplay::Console::ClearNamePlateSelection();
                UI_GetRActorFromID(UIRACTOR_CONSOLE_MAIN_MENU_HIGHLIGHT).Cleanup();
                UI_GetConsoleListBoxFromID(UILIST_CONSOLE_MENU_GROUP).SetSelectedIndex(-1);
                UI_GetConsoleListBoxFromID(UILIST_CONSOLE_FRIENDS_LIST).SetSelectedIndex(0);
                UI_SetControllerFocus(UILIST_CONSOLE_FRIENDS_LIST);
                return;
            }
        }
    }

    FocusMainMenu();
}

} }
