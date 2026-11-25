// Original file: X/Client/UserInterface/Console/OnlineActions.cpp
// The console front end's online actions: joining parties, local-player sign-in and join,
// hero/game validation and their error messages.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../Console/Online.h"
#include "../../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h"
#include "../../Console/GamerProfile.h"
#include "ConsoleScreenManager.h"
#include "ConsoleModalNotificationManager.h"
#include "OnlineActions.h"
#include "../../../Common/Game.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Common/OnlineService/OnlineService.h"
#include "../../../Common/OnlineService/Callback/Callback.h"
#include "../../../Common/Protobuf/Account.pb.h"
#include "../../../Common/Protobuf/Hero.pb.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../../Common/Player.h"
#include "../../CPlayer.h"
#include "../../../Common/Protobuf/Console.pb.h"
#include "../../../Common/Achievements/Console/ConsoleAchievements.h"
#include "../../CGame.h"
#include "../../Cutscene.h"
#include "../../../Common/ActorCommonData.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UIWindowControl.h"
#include "../../Battlenet/Subsystems/LightBoxManager.h"
#include "../../../Common/Definitions/HeroDefinition.h"
#include "../../../Common/Definitions/ActDefinition.h"
#include "../../../Common/Act.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../Console/LobbyService.h"
namespace Console { namespace GamerProfile {  // GamerProfile.h (GamerProfileNX64.cpp defines these locally too)
enum SignInResult { SIGNIN_CANCELLED, SIGNIN_FAILED, SIGNIN_FAILED_PROFILE_IN_USE, SIGNIN_FAILED_GUEST_NOT_ALLOWED, SIGNIN_FAILED_NO_ONLINE_PRIVILEGE, SIGNIN_FAILED_NO_SPACE, SIGNIN_SUCCESS, SIGNIN_SUCCESS_NO_PROFILE, SIGNIN_SUCCESS_NO_DEVICE, SIGNIN_SUCCESS_GUEST, SIGNIN_SUCCESS_UNOWNED_SAVE, SIGNIN_SUCCESS_CORRUPT_DATA, SIGNIN_SUCCESS_DEMO_SAVE_IMPORTED, SIGNIN_SUCCESS_NO_SAVE };
enum SignInMode { SIGNIN_MODE_ANY, SIGNIN_MODE_GUEST_ALLOWED, SIGNIN_MODE_ONLINE_ONLY, SIGNIN_MODE_VIA_INVITE };
enum ExportSaveResult { EXPORT_SAVE_FAILED, EXPORT_SAVE_FAILED_NOT_FOUND, EXPORT_SAVE_FAILED_NOT_OWNER, EXPORT_SAVE_FAILED_CORRUPT, EXPORT_SAVE_SUCCESS };
enum ImportSaveResult { IMPORT_SAVE_FAILED, IMPORT_SAVE_FAILED_NO_STORAGE, IMPORT_SAVE_FAILED_CORRUPT, IMPORT_SAVE_SUCCESS };
typedef OnlineService::Callback<void (SignInResult)> SignInCallback; typedef OnlineService::Callback<void (ImportSaveResult)> ImportSaveCallback; void SignIn(int32 nUserIdx, SignInMode eMode, SignInCallback pfnCallback);
} }
namespace Console { namespace Online { BOOL IsSeasonsInitialized(); void ClearOutstandingInvite(); void PartyCreate(int32 nControllerIndex, GameParams & tGameParams, BOOL bOfflineOnly); } }
namespace Console { namespace Online { void PartyJoin(PartyConnectionInfo & tPartyInfo, JoinType eJoinType, PartyJoinCallback pfnCallback); BOOL GetInviteConnectionInfo(PartyConnectionInfo & tPartyInfo, BOOL & bIsInvite); } }
namespace Console { namespace Online { BOOL PartyIsFull(); BOOL PartyIsGamePaused(); uint32 GetSecondsUntilSeasonEnd(); void PartyAddPlayer(int32 nControllerIndex, PartyAddPlayerCallback pfnCallback); uint32 PartyGetLocalMemberCount(); void PartySwitchOnlineState(PartyOnlineState eState); } }
namespace Console { namespace GamerProfile { void SignOut(int32 nUserIdx); uint32 GetNumAvailableHeroes(int32 nUserIdx);  OnlinePlayPrivilege GetOnlinePlayPrivilege(int32 nUserIdx); nn::account::Uid GetUserId(int32 nUserIndex); } }  // GetUserId: GamerProfileNX64.cpp:734
namespace Console { namespace Screens { enum { MAIN_MENU_SCREEN_SET = 20 }; BOOL OpenToScreen(const ScreenSets eScreenSet, const int32 idScreen, int32 nControllerOpening, BOOL fSkipTransition); } }
namespace Console { namespace DLCManager { uint32 GetLicenses(); } }  // DLCManager.cpp:385
namespace Console { namespace UISaveIcon { void ForceDraw(const Vector2D * pvPos); } }  // UISaveIcon.cpp
CutsceneID CutsceneGetFullScreenCutscene();  // X/Client/Cutscene.cpp:2108
namespace Console { namespace Utilities { void ShowSystemOnlineErrorDialog(); void ShowNX64Upsell(int32 nControllerIndex, OnlineService::Callback<void ()> successCB, OnlineService::Callback<void ()> cancelCB); } }  // UtilitiesNX64.cpp:198
namespace UIHeroCreate { namespace Console { BOOL CreateHero(int32 nControllerIndex, PlayerClass eClass, uint32 uFlags, CRefString sString); } }  // UIConsoleHeroCreate.cpp:1496
int32 UI_GetPrimaryUIController(); void UI_SetPrimaryUIController(int32 nPrimaryController);
namespace Console { namespace SaveMigration { void PerformSeasonMigration(); BOOL HealMigrationData(int32 nUserIndex, D3::Console::MigrationData & tMigrationData); } }
namespace Console { enum { TITLE_VERSION_CURRENT = 277 }; namespace GamerProfile { BOOL IsActive(int32 nUserIdx); void ImportSave(int32 nUserIdx, D3::Console::MigrationData * ptMigrationData, BOOL bDestructive, ImportSaveCallback pfnCallback); } }  // TitleVersion.h, GamerProfileNX64.cpp:1736
float SNOPackChunkGetActLoadPercentComplete(Act eAct); void SNOPackChunkRequestLoad(Act eAct); BOOL SNOPackChunkLoadAct(Act eAct);  // SNOFiles.cpp
typedef int32 X1HandicapLevel; bool PlayerCanPlayHandicap(int32 nLevel, int32 nHighestRiftLevel, X1HandicapLevel eHandicap, bool bPublicGame);
namespace Console { namespace UIOnlineActions {
void SetGameParamsForHero(const D3::Account::SavedDefinition & tAccount, const D3::Hero::SavedDefinition & tHero, Online::GameParams & tParams);
void HandleJoinParty(BOOL bIsInvite, const Online::PartyConnectionInfo & tConnectionInfo, Online::PartyJoinCallback pfnCallback);
void FinalizeLocalPlayerJoin(int32 nControllerIndex, Online::PartyAddPlayerCallback pfnCallback); BOOL CanLocalPlayerJoin();
void OnLocalPlayerSignedIn(int32 nControllerIndex, BOOL bForceChooseHero, const UIID & idLightBox, GamerProfile::SignInResult eResult);
} }

namespace Console {
namespace UIOnlineActions {
struct UIOnlineActionsData { ModalNotificationId idPartyJoinDialog; BOOL bPartyJoinInProgress; BOOL bAdHocJoinNetworkInProgress; BOOL bLocalPlayerJoinInProgress;
    UIOnlineActionsData() : idPartyJoinDialog(INVALID_MODAL_NOTIFICATION_ID), bPartyJoinInProgress(FALSE), bAdHocJoinNetworkInProgress(FALSE), bLocalPlayerJoinInProgress(FALSE) {} };

UIOnlineActionsData s_tOnlineActionsData;




void OnPartyJoined(Online::PartyJoinCallback pfnCallback, BOOL bSuppressErrorMessage, Online::JoinResult eResult)
{
    const char * szMessage = NULL;

    s_tOnlineActionsData.bPartyJoinInProgress = FALSE;
    Screens::GetModalNotificationManager().RemoveNotification(s_tOnlineActionsData.idPartyJoinDialog);
    s_tOnlineActionsData.idPartyJoinDialog = INVALID_MODAL_NOTIFICATION_ID;

    if (!GameIsStartUp() && Screens::IsScreenSetOpen())
    {
        Screens::CloseScreenSet(TRUE, TRUE);
    }

    switch (eResult)
    {
    case Online::JOIN_SUCCESS:
        break;
    case Online::JOIN_FAILED_GAME_FULL:
        szMessage = "ConsoleErrors:PartyTooFullFailure";
        break;
    case Online::JOIN_FAILED_REQUIRES_UPDATE:
        szMessage = "ConsoleErrors:PartyJoinFailureRequiresUpdateNX64";
        break;
    case Online::JOIN_FAILED_HOST_VERSION_TOO_OLD:
        szMessage = "ConsoleErrors:PartyJoinFailureHostVersionTooOldNX64";
        break;
    case Online::JOIN_FAILED_INVITE_ONLY:
        szMessage = "ConsoleErrors:PartyJoinFailureInviteOnly";
        break;
    default:
        szMessage = "ConsoleErrors:PartyJoinFailure";
        break;
    }




























    if (szMessage && !bSuppressErrorMessage && !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = GamerProfile::GetPrimaryProfileUserIndex();
        tParams.uszTitle = StringListFetch("Bnet_Notifications:ErrorTitle");
        tParams.uszMessage = StringListFetch(szMessage);
        Screens::GetModalNotificationManager().SetOkNotification(tParams);
    }

    if (pfnCallback)
    {
        pfnCallback(eResult);
    }
}




void ShowJoinPartyDialog()
{
    s_tOnlineActionsData.bPartyJoinInProgress = TRUE;

    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        NotificationCreationParams tParams;
        tParams.uszMessage = StringListFetch("Console:JoiningParty");
        s_tOnlineActionsData.idPartyJoinDialog =
            Screens::GetModalNotificationManager().SetMessageOnlyNotification(tParams);
    }
}






void sAcceptJoinParty(BOOL bIsInvite, Online::PartyConnectionInfo tConnectionInfo, Online::PartyJoinCallback pfnCallback, const UIID &)
{
    ShowJoinPartyDialog();

    Online::JoinType eJoinType = Online::JOIN_TYPE_NORMAL;
    if (bIsInvite)
        eJoinType = Online::JOIN_TYPE_INVITE;

    Online::PartyJoin(tConnectionInfo, eJoinType, OnlineService::internal::function_binder_3<void, Online::PartyJoinCallback, BOOL, Online::JoinResult>::bind<&OnPartyJoined>(pfnCallback, FALSE));
}



void sCancelJoinParty(
    BOOL bIsInvite,
    Online::PartyJoinCallback pfnCallback,
    const UIID &)
{
    if (bIsInvite)
    {
        Online::ClearOutstandingInvite();
    }

    if (pfnCallback)
    {
        pfnCallback(Online::JOIN_CANCELLED);
    }
}

BOOL IsJoinPartyInProgress(); void ShowPartySwitchOnlineResultMessage(Online::PartySwitchOnlineResult eResult, int32 nControllerIndex, const char * szMessage);  // defined below at OnlineActions.cpp:419 and :2301

// NON_MATCHING: 2940 bytes vs 2692; the original calls Callback<void (const UIID &)>::operator=<function_bind_3_1<...sAcceptJoinParty>> (line 277) out of line, ours inlines it; the rest lines up
void HandleJoinParty(
    BOOL bIsInvite,
    const Online::PartyConnectionInfo & tConnectionInfo,
    Online::PartyJoinCallback pfnCallback)
{

    if (!SNOPackChunkLoadAct(ACT_OPEN_WORLD))
    {
        NotificationCreationParams tParams;
        tParams.uszMessage = LocalizedSprintf(
            StringListFetch("ConsoleErrors:CannotProceedGenericContentNotReady").str(),
            UnicodeText::FormatNumber(SNOPackChunkGetActLoadPercentComplete(ACT_OPEN_WORLD) * 100.0f, TRUE, FNS_NORMAL, 2, 2, XLOCALE_INVALID).str(),
            NULL);
        Screens::GetModalNotificationManager().SetOkNotification(tParams);

        return;
    }

    if (IsJoinPartyInProgress())
    {
        if (bIsInvite)
        {
            Online::ClearOutstandingInvite();
        }

        if (pfnCallback)
        {
            pfnCallback(Online::JOIN_FAILED);
        }
        return;
    }

    if (!tConnectionInfo.m_bIsAdHocGame && tConnectionInfo.m_ptAddress.isNull())
    {



        Online::ClearOutstandingInvite();
        OnPartyJoined(pfnCallback, FALSE, Online::JOIN_FAILED_UNABLE_TO_CONNECT);
        return;
    }



    Online::PartyOnlineState possibleOfflinePartyState =

        (Online::GetNetworkMode() == Online::NET_ADHOC) ? Online::PARTY_ADHOC : Online::PARTY_LAN;




    Online::PartySwitchOnlineResult eResult = Online::PartyCanSwitchOnlineState((bIsInvite || tConnectionInfo.m_bIsOnline) ? Online::PARTY_ONLINE : possibleOfflinePartyState);




    if (eResult == Online::PARTY_SWITCH_ONLINE_OK && GameIsStartUp())
    {
        sAcceptJoinParty(bIsInvite, tConnectionInfo, pfnCallback, GetUIIDNone());
    }
    else if (eResult == Online::PARTY_SWITCH_ONLINE_OK || eResult == Online::PARTY_SWITCH_ONLINE_WILL_DISCONNECT)
    {

        NotificationCreationParams tParams;
        tParams.nControllerIndex = GamerProfile::GetPrimaryProfileUserIndex();
        tParams.uszMessage = StringListFetch("ConsoleErrors:PartyJoinLeavingCurrentGame");
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_4<void, int32, Online::PartyConnectionInfo, Online::PartyJoinCallback, const UIID &>::bind<&sAcceptJoinParty>(bIsInvite, tConnectionInfo, pfnCallback);
        tParams.tCallbacks.pfnOnClickCancelButton = OnlineService::internal::function_binder_3<void, int32, Online::PartyJoinCallback, const UIID &>::bind<&sCancelJoinParty>(bIsInvite, pfnCallback);
        Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
    }
    else
    {































































        if (eResult == Online::PARTY_SWITCH_ONLINE_PREMIUM_SERVICE_REQUIRED)
        {
            int32 nPrimaryUserIndex = GamerProfile::GetPrimaryProfileUserIndex();
            for (int32 i = -1; i < MAX_LOCAL_PLAYERS; ++i)
            {


                int32 nControllerIndex = (i == -1) ? nPrimaryUserIndex : i;






                if (i != -1 && i == nPrimaryUserIndex)
                {
                    continue;
                }


                if (nControllerIndex == -1)
                {
                    break;
                }


                if (Online::PartyIsInParty(nControllerIndex))
                {
                    Utilities::ShowNX64Upsell(nControllerIndex,
                        OnlineService::internal::function_binder_3<void, int32, const Online::PartyConnectionInfo &, Online::PartyJoinCallback>::bind<&HandleJoinParty>(bIsInvite, tConnectionInfo, pfnCallback),
                        OnlineService::internal::function_binder_3<void, int32, Online::PartyJoinCallback, const UIID &>::bind<&sCancelJoinParty>(bIsInvite, pfnCallback, GetUIIDNone()));
                    return;
                }
            }
        }


        ShowPartySwitchOnlineResultMessage(eResult, -1, "ConsoleErrors:PartyJoinFailure");

        if (bIsInvite)
        {
            Online::ClearOutstandingInvite();
        }

        OnPartyJoined(pfnCallback, TRUE, Online::JOIN_FAILED_NO_PRIVILEGE);
    }
}




void JoinParty(Online::PartyConnectionInfo & tConnectionInfo, Online::PartyJoinCallback pfnCallback)
{
    HandleJoinParty(FALSE, tConnectionInfo, pfnCallback);
}





void JoinPartyByInvite(
    Online::PartyJoinCallback pfnCallback)
{
    Online::PartyConnectionInfo tConnectionInfo;
    BOOL bIsInvite = TRUE;
    if (Online::GetInviteConnectionInfo(tConnectionInfo, bIsInvite))
    {
        HandleJoinParty(bIsInvite, tConnectionInfo, pfnCallback);
    }
}



BOOL IsJoinPartyInProgress()
{
    return s_tOnlineActionsData.bPartyJoinInProgress;
}




BOOL IsJoinAdHocNetworkInProgress()
{
    return s_tOnlineActionsData.bAdHocJoinNetworkInProgress;
}


void SetJoinAdHocNetworkInProgress()
{
    s_tOnlineActionsData.bAdHocJoinNetworkInProgress = TRUE;
}


void ClearJoinAdHocNetworkInProgress()
{
    s_tOnlineActionsData.bAdHocJoinNetworkInProgress = FALSE;
}




BOOL CreatePartyIfNeeded(int32 nControllerIndex)
{
    if (GameIsStartUp() && !GameIsAGameStartQueued())
    {
        SaveMigration::PerformSeasonMigration();
    }

    if (!Online::PartyIsInParty())
    {
        const D3::Account::SavedDefinition & tAccountDefinition = GamerProfile::GetAccount(nControllerIndex);
        const D3::Hero::SavedDefinition & tHeroDefinition = GamerProfile::GetHero(nControllerIndex);
        Online::GameParams tGameParams;
        SetGameParamsForHero(tAccountDefinition, tHeroDefinition, tGameParams);
        Online::PartyCreate(nControllerIndex, tGameParams, FALSE);
        return TRUE;
    }
    return FALSE;
}







}  // namespace UIOnlineActions
}  // namespace Console
namespace UIGameSettings { namespace Console { BOOL IsQuestFilteringEnabled(); } }  // UIConsoleGameSettings.cpp:919
namespace UIUtils { namespace Console { BOOL GameTagIsValid(const OnlineService::GameTagId & tTag, BOOL bAdventureMode); } }  // UIConsoleUtils.cpp:977
namespace Console {
namespace UIOnlineActions {
BOOL IsDifficultyLocked(const D3::Account::Digest & tAccountDigest, int32 nLevel, BOOL bIsHardcore, int32 nDifficulty); BOOL IsAdventureModeLocked(const D3::Account::Digest & tAccountDigest, BOOL bHardcore, BOOL bSeasonal); bool IsActiveSeason(uint32 uSeasonNum, const D3::Account::Digest & tAccountDigest);

void SetGameParamsForHero(
    const D3::Account::SavedDefinition & tAccount,
    const D3::Hero::SavedDefinition & tHero,
    Online::GameParams & tParams)
{




    tParams.m_eGameType = 1;
    tParams.m_nLevel = tHero.digest().level();
    tParams.m_bIsHardcore = tHero.digest().player_flags() & (1 << PLAYERFLAG_HARDCORE);
    tParams.m_bIsSeasonal = IsActiveSeason(tHero.digest().season_created(), tAccount.digest());
    tParams.unk_14 = tParams.m_bIsSeasonal ? (tHero.digest().player_flags2() & 1) : 0;
    tParams.m_uSeason = tParams.m_bIsSeasonal ? tHero.digest().season_created() : 0;
    tParams.m_tWeeklyChallengeConfig.Clear();
    tParams.m_tWeeklyChallengeData.Clear();


    // 2.7.7: the adventure-mode-locked / quest-filtering condition is gone.

    if (tHero.console_data().has_last_played_adventure_mode_act())
    {
        tParams.m_ePreferredAdventureAct = tHero.console_data().last_played_adventure_mode_act();
        tParams.m_eAct = ACT_OPEN_WORLD;
        tParams.m_snoQuest = SNO_NONE;
        tParams.m_nQuestStep = -1;
        tParams.m_bResumeFromSave = FALSE;
        if (!UIUtils::Console::GameTagIsValid(tParams.m_nTag, TRUE))
        {
            tParams.m_nTag = OnlineService::GAME_TAG_ID_BOUNTIES;
        }
    }
    else
    {
        Act eAct = tHero.digest().last_played_act();
        if (eAct < ACT_1 || eAct > ACT_MAX_CAMPAIGN)
        {
            eAct = ACT_1;
        }
        SNO snoQuest = tHero.digest().last_played_quest();
        if (snoQuest == SNO_NONE)
        {
            SNO snoAct = ActSNOFromActEnum(eAct);
            ScopedActDefinitionLockR pAct(snoAct);
            if (pAct && ActGetNumQuests(pAct) > 0)
            {
                snoQuest = ((ActQuestInfo *)pAct->arQuests)[0].snoQuest;
            }
        }
        tParams.m_eAct = eAct;
        tParams.m_snoQuest = snoQuest;
        tParams.m_nQuestStep = tHero.digest().last_played_quest_step();
        tParams.m_bResumeFromSave = TRUE;
        if (!UIUtils::Console::GameTagIsValid(tParams.m_nTag, FALSE))
        {
            tParams.m_nTag = OnlineService::GAME_TAG_ID_DEFAULT;
        }
    }
    tParams.m_nHandicap = 0;
    if (tHero.has_console_data())
    {
        tParams.m_nHandicap = tHero.console_data().last_played_handicap();
    }
    if (tParams.m_nTag == OnlineService::GAME_TAG_ID_NONE)
    {
        tParams.m_nTag = OnlineService::GAME_TAG_ID_DEFAULT;
    }
}




inline void MigrateSaveImportComplete(int32 nControllerIndex, NotificationCallback pfnSignInCompleteCallback, NotificationCallback pfnCancelSignInCallback, GamerProfile::ImportSaveResult eResult)
{
    if (eResult == GamerProfile::IMPORT_SAVE_SUCCESS && GamerProfile::IsActive(nControllerIndex))
    {
        sgtAppGlobals.ptAchievementsInterface->OnUserSignedIn(nControllerIndex);
        pfnSignInCompleteCallback(GetUIIDNone());
    }
    else
    {
        pfnCancelSignInCallback(GetUIIDNone());
    }
}








void MigrateSaveExportComplete(
    int32 nControllerIndex,
    D3::Console::MigrationData * ptMigrationData,
    NotificationCallback pfnSignInCompleteCallback,
    NotificationCallback pfnCancelSignInCallback,
    GamerProfile::ExportSaveResult eResult)
{
    if (eResult != GamerProfile::EXPORT_SAVE_FAILED_CORRUPT && eResult != GamerProfile::EXPORT_SAVE_SUCCESS)
    {
        ::delete ptMigrationData;
        pfnCancelSignInCallback(GetUIIDNone());
        return;
    }


    if (!GamerProfile::IsActive(nControllerIndex))
    {
        ::delete ptMigrationData;
        pfnCancelSignInCallback(GetUIIDNone());
        return;
    }

    SaveMigration::HealMigrationData(nControllerIndex, *ptMigrationData);

    ptMigrationData->mutable_account()->mutable_console_data()->set_version_required(TITLE_VERSION_CURRENT);

    GamerProfile::ImportSave(nControllerIndex, ptMigrationData, FALSE,
        OnlineService::internal::function_binder_4<void, int32, NotificationCallback, NotificationCallback, GamerProfile::ImportSaveResult>::bind<&MigrateSaveImportComplete>(nControllerIndex, pfnSignInCompleteCallback, pfnCancelSignInCallback));
}




void SignInHandleError(int32 nControllerIndex, GamerProfile::SignInResult eResult)
{
    const char * szMessage = NULL;
    switch (eResult)
    {
    case GamerProfile::SIGNIN_FAILED_PROFILE_IN_USE:
        szMessage = "ConsoleErrors:SignInProfileInUseError";
        break;
    case GamerProfile::SIGNIN_FAILED_GUEST_NOT_ALLOWED:
    case GamerProfile::SIGNIN_SUCCESS_GUEST:
        szMessage = "ConsoleErrors:SignInNoGuestError";
        break;
    default:
        return;
    }

    NotificationCreationParams tParams;
    tParams.uszMessage = StringListFetch(szMessage);
    Screens::GetModalNotificationManager().SetOkNotification(tParams);
}



}  // namespace UIOnlineActions
}  // namespace Console
namespace Console {
namespace SaveMigration {
BOOL IsMigrationRequired(const D3::Account::SavedDefinition & tDefinition);  // SaveMigration.cpp:757
}
namespace GamerProfile {
void ExportSave(int32 nUserIdx, D3::Console::MigrationData * ptMigrationData, OnlineService::Callback<void (ExportSaveResult)> pfnCallback);  // GamerProfileNX64.cpp:1804
}
}  // namespace Console
namespace Console {
namespace UIOnlineActions {





enum
{
    CONFIRM_TITLEUPDATE_MISSING,
    CONFIRM_DLC_MISSING,
    CONFIRM_TITLEUPDATE_DEPENDENCY,
    CONFIRM_DLC_DEPENDENCY,
};


void VerifyNSOComplete(NotificationCallback pfnSignInCompleteCallback, ModalNotificationId ModId)
{
    Screens::GetModalNotificationManager().RemoveNotification(ModId);
    pfnSignInCompleteCallback(GetUIIDNone());
}


void SignInConfirmWarnings(
    int32 nControllerIndex,
    NotificationCallback pfnSignInCompleteCallback,
    NotificationCallback pfnCancelSignInCallback,
    uint32 uConfirmationFlags,
    const UIID &)
{
    const char * szMessage = NULL;
    BOOL bFailSignIn = FALSE;

    if (uConfirmationFlags & (1 << CONFIRM_TITLEUPDATE_MISSING))
    {
        szMessage = "ConsoleErrors:SignInSaveRequiresTitleUpdateError";
        bFailSignIn = TRUE;
    }
    else if ((uConfirmationFlags & (1 << CONFIRM_TITLEUPDATE_DEPENDENCY)) && !(uConfirmationFlags & (1 << CONFIRM_DLC_MISSING)))
    {









        szMessage = "ConsoleErrors:SignInSaveWillRequireTitleUpdateWarning";
        uConfirmationFlags &= ~(1 << CONFIRM_TITLEUPDATE_DEPENDENCY);
    }










    if (szMessage)
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszMessage = StringListFetch(szMessage);

        if (bFailSignIn)
        {
            Screens::GetModalNotificationManager().SetOkNotification(tParams);
            pfnCancelSignInCallback(GetUIIDNone());
        }
        else
        {
            tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_5<void, int32, NotificationCallback, NotificationCallback, uint32, const UIID &>::bind<&SignInConfirmWarnings>(nControllerIndex, pfnSignInCompleteCallback, pfnCancelSignInCallback, uConfirmationFlags);
            tParams.tCallbacks.pfnOnClickCancelButton = pfnCancelSignInCallback;

            Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
        }

        return;
    }





    const D3::Account::SavedDefinition & tAccount = GamerProfile::GetAccount(nControllerIndex);

    if (tAccount.console_data().has_version_required() &&
        SaveMigration::IsMigrationRequired(tAccount))
    {
        D3::Console::MigrationData * ptMigrationData = new D3::Console::MigrationData;
        ptMigrationData->set_title_version(tAccount.console_data().version_required());
        GamerProfile::ExportSave(nControllerIndex, ptMigrationData,
            OnlineService::internal::function_binder_5<void, int32, D3::Console::MigrationData *, NotificationCallback, NotificationCallback, GamerProfile::ExportSaveResult>::bind<&MigrateSaveExportComplete>(nControllerIndex, ptMigrationData, pfnSignInCompleteCallback, pfnCancelSignInCallback));
        return;
    }

    sgtAppGlobals.ptAchievementsInterface->OnUserSignedIn(nControllerIndex);

    GamerProfile::GetAccount(nControllerIndex).mutable_console_data()->set_version_required(TITLE_VERSION_CURRENT);



    if (Online::PartyGetOnlineState() >= Online::PARTY_ONLINE)
    {

        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszMessage = StringListFetch("ConsoleErrors:ValidatingNSO");

        ModalNotificationId ModId = Screens::GetModalNotificationManager().SetMessageOnlyNotification(tParams);

        Online::LobbyService::VerifyNSOSubscriptionAsync(nControllerIndex, OnlineService::internal::function_binder_2<void, NotificationCallback, ModalNotificationId>::bind<&VerifyNSOComplete>(pfnSignInCompleteCallback, ModId));
    }
    else
    {

        pfnSignInCompleteCallback(GetUIIDNone());
    }
}


































































































void SignInHandleSuccess(int32 nControllerIndex, GamerProfile::SignInResult eResult, NotificationCallback pfnSignInCompleteCallback, NotificationCallback pfnCancelSignInCallback)
{
    const char * szWarning = NULL;
    BOOL bHasSave = TRUE;

    switch (eResult)
    {
    case GamerProfile::SIGNIN_SUCCESS_NO_PROFILE:
        if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
            szWarning = "ConsoleErrors:SignInNoProfileWarning";
        bHasSave = FALSE;
        break;
    case GamerProfile::SIGNIN_SUCCESS_NO_DEVICE:
        szWarning = "ConsoleErrors:SignInNoDeviceWarning";
        bHasSave = FALSE;
        break;
    case GamerProfile::SIGNIN_SUCCESS_GUEST:
        if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
            szWarning = "ConsoleErrors:SignInGuestWarning";
        bHasSave = FALSE;
        break;
    case GamerProfile::SIGNIN_SUCCESS_UNOWNED_SAVE:
        szWarning = "ConsoleErrors:SignInNotYourSaveWarning";
        break;
    case GamerProfile::SIGNIN_SUCCESS_CORRUPT_DATA:
        szWarning = "ConsoleErrors:SignInSaveCorruptedWarning";
        break;
    default:
        break;
    }

    const D3::Account::SavedDefinition & tAccount = GamerProfile::GetAccount(nControllerIndex);
    uint32 uConfirmationFlags = 0;
    if (tAccount.console_data().has_version_required() && tAccount.console_data().version_required() > TITLE_VERSION_CURRENT)
    {
        uConfirmationFlags |= (1 << CONFIRM_TITLEUPDATE_MISSING);
    }
    if (bHasSave && tAccount.console_data().has_version_required() && tAccount.console_data().version_required() < TITLE_VERSION_CURRENT)
    {
        uConfirmationFlags |= (1 << CONFIRM_TITLEUPDATE_DEPENDENCY);
    }

    if (szWarning)
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszMessage = StringListFetch(szWarning);
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_5<void, int32, NotificationCallback, NotificationCallback, uint32, const UIID &>::bind<&SignInConfirmWarnings>(nControllerIndex, pfnSignInCompleteCallback, pfnCancelSignInCallback, uConfirmationFlags);
        tParams.tCallbacks.pfnOnClickCancelButton = pfnCancelSignInCallback;
        Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
    }
    else
    {
        SignInConfirmWarnings(nControllerIndex, pfnSignInCompleteCallback, pfnCancelSignInCallback, uConfirmationFlags, GetUIIDNone());
    }
}
























































inline void OnLocalPlayerAddedErrorAccepted(int32 nPlayerThatFailedToBeAdded, const UIID & id)
{
    if (UI_GetPrimaryUIController() == nPlayerThatFailedToBeAdded)
    {
        if (GameIsStartUp() || Screens::IsScreenSetOpen())
        {
            UI_SetPrimaryUIController(GamerProfile::GetPrimaryProfileUserIndex());
        }
        else
        {
            UI_SetPrimaryUIController(-1);
        }
    }
}











void OnLocalPlayerAdded(int32 nControllerIndex, Online::PartyAddPlayerCallback pfnCallback, BOOL bSuccess)
{
    s_tOnlineActionsData.bLocalPlayerJoinInProgress = FALSE;

    if (!bSuccess)
    {
        const char * szMessage = "ConsoleErrors:PartyJoinFailure";
        if (Online::PartyIsFull())
            szMessage = "H2OLayout:BattleNetJoinGameErrorGameFull";

        NotificationCreationParams tParams;
        tParams.nControllerIndex = -1;
        tParams.uszMessage = StringListFetch(szMessage);
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_2<void, int32, const UIID &>::bind<&OnLocalPlayerAddedErrorAccepted>(nControllerIndex);
        Screens::GetModalNotificationManager().SetOkNotification(tParams);

        GamerProfile::SignOut(nControllerIndex);
    }
    else if (!GameIsStartUp())
    {
        CPlayerLocalPlayerJoin(nControllerIndex);
    }

    if (pfnCallback)
    {
        pfnCallback(bSuccess);
    }
}
























void FinalizeLocalPlayerJoin(int32 nControllerIndex, Online::PartyAddPlayerCallback pfnCallback)
{
    s_tOnlineActionsData.bLocalPlayerJoinInProgress = TRUE;
    Online::PartyAddPlayer(nControllerIndex, OnlineService::internal::function_binder_3<void, int32, Online::PartyAddPlayerCallback, BOOL>::bind<&OnLocalPlayerAdded>(nControllerIndex, pfnCallback));
}








void OnAddLocalPlayerToGame(int32 nControllerIndex, BOOL bForceChooseHero, const UIID &)
{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        UIHeroCreate::Console::CreateHero(nControllerIndex, PLAYER_CLASS_NECROMANCER, 0, PlayerGetClassUIName(PLAYER_CLASS_NECROMANCER, TRUE));
        return;
    }

    BOOL bHeroLoaded = GamerProfile::IsHeroLoaded(nControllerIndex);
    if (bForceChooseHero || !bHeroLoaded)
    {
        if (GamerProfile::GetNumAvailableHeroes(nControllerIndex) > 0)
            Screens::OpenToScreen(Screens::HERO_SELECT_SCREEN_SET, 0, nControllerIndex, FALSE);
        else
            Screens::OpenToScreen(Screens::HERO_CREATE_SCREEN_SET, 0, nControllerIndex, FALSE);
        return;
    }

    FinalizeLocalPlayerJoin(nControllerIndex, Online::PartyAddPlayerCallback());
}
















inline void OnConfirmOnlineModeSwitch(int32 nControllerIndex, BOOL bForceChooseHero, Online::PartyOnlineState eOnlineState, const UIID &)
{
    s_tOnlineActionsData.bLocalPlayerJoinInProgress = FALSE;

    // The party may have changed state while the warning was up; never raise it.
    if (Online::PartyGetOnlineState() < eOnlineState)
        eOnlineState = Online::PartyGetOnlineState();

    if (Online::PartyCanSwitchOnlineState(eOnlineState) == Online::PARTY_SWITCH_ONLINE_WILL_DISCONNECT)
        return;

    Online::PartySwitchOnlineState(eOnlineState);
    OnAddLocalPlayerToGame(nControllerIndex, bForceChooseHero, GetUIIDNone());
}





}  // namespace UIOnlineActions
}  // namespace Console
void UI_DeleteControl(const UIID & idControl, const BOOL fAssertIfDeletingRoot);  // UI.cpp:6196
namespace Console {
namespace UISaveIcon {
void StopForceDraw();  // UISaveIcon.cpp:268
}  // namespace UISaveIcon
namespace UIOnlineActions {



inline void OnCancelLocalPlayerSignedIn(int32 nControllerIndex, const UIID &)
{
    // The player backed out of sign-in; drop the half-joined profile.
    GamerProfile::SignOut(nControllerIndex);
    s_tOnlineActionsData.bLocalPlayerJoinInProgress = FALSE;
}





// NON_MATCHING: block placement only; the "SignInLANProfileRequiredError" block is placed after the
// switch-warning notification instead of before it.
void OnCompleteLocalPlayerSignedIn(
    int32 nControllerIndex,
    BOOL bForceChooseHero,
    const UIID &)
{
    s_tOnlineActionsData.bLocalPlayerJoinInProgress = FALSE;

    if (!Online::PartyIsInParty() ||
        Online::PartyIsInParty(nControllerIndex))
    {
        return;
    }


    if (!CanLocalPlayerJoin())
    {
        GamerProfile::SignOut(nControllerIndex);
        return;
    }

    if (Online::PartyIsFull())
    {
        OnLocalPlayerAdded(nControllerIndex, Online::PartyAddPlayerCallback(), FALSE);
        return;
    }


    Online::PartyOnlineState eBestOnlineState;

    GamerProfile::OnlinePlayPrivilege ePrivilege = GamerProfile::GetOnlinePlayPrivilege(nControllerIndex);
    switch (ePrivilege)
    {
    case GamerProfile::ONLINE_PLAY_ALLOWED:
        eBestOnlineState = Online::PARTY_ONLINE_PUBLIC;
        break;
    case GamerProfile::ONLINE_PLAY_BLOCKED_NOT_SIGNED_IN_LOCALLY:
        eBestOnlineState = Online::PARTY_OFFLINE;
        break;
    default:




        eBestOnlineState = Online::PartyGetOnlineState() == Online::PARTY_ADHOC ? Online::PARTY_ADHOC : Online::PARTY_OFFLINE;
        break;
    }











    if (eBestOnlineState < Online::PartyGetOnlineState())
    {


        const char * szMessage = NULL;

        if (!Online::PartyIsHost() ||
            Online::PartyCanSwitchOnlineState(eBestOnlineState) != Online::PARTY_SWITCH_ONLINE_OK)
        {





















            if (ePrivilege == GamerProfile::ONLINE_PLAY_BLOCKED_REQUIRES_PREMIUM)
            {
                Utilities::ShowNX64Upsell(
                    nControllerIndex,
                    OnlineService::internal::function_binder_3<void, int32, BOOL, const UIID &>::bind<&OnCompleteLocalPlayerSignedIn>(nControllerIndex, bForceChooseHero, GetUIIDNone()),
                    OnlineService::internal::function_binder_2<void, int32, const UIID &>::bind<&OnCancelLocalPlayerSignedIn>(nControllerIndex, GetUIIDNone()));
            }
            else
            {

                if (!Online::PartyIsOnline())
                    szMessage = "ConsoleErrors:SignInLANProfileRequiredError";
                else if (ePrivilege == GamerProfile::ONLINE_PLAY_BLOCKED_PARENTAL_CONTROLS)
                    szMessage = "ConsoleErrors:SignInOnlineParentalBlockErrorPSN";
                else if (ePrivilege == GamerProfile::ONLINE_PLAY_BLOCKED_NOT_SIGNED_IN_ONLINE)
                    szMessage = "ConsoleErrors:SignInNotSignedOnlineInErrorNX64";











































                if (szMessage != NULL)
                {
                    NotificationCreationParams tParams;
                    tParams.nControllerIndex = nControllerIndex;
                    tParams.uszMessage = StringListFetch(szMessage);
                    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_2<void, int32, const UIID &>::bind<&OnCancelLocalPlayerSignedIn>(nControllerIndex);
                    Screens::GetModalNotificationManager().SetOkNotification(tParams);
                }
                else
                {
                    OnCancelLocalPlayerSignedIn(nControllerIndex, GetUIIDNone());
                }
            }
        } else
        {
            s_tOnlineActionsData.bLocalPlayerJoinInProgress = TRUE;

            // 2.7.7: a premium-blocked user with a valid user id gets the upsell instead of the switch warning.
            nn::account::Uid tUserId = GamerProfile::GetUserId(nControllerIndex);
            if (ePrivilege == GamerProfile::ONLINE_PLAY_BLOCKED_REQUIRES_PREMIUM && (tUserId._data[1] != 0 || tUserId._data[0] != 0))
            {
                Utilities::ShowNX64Upsell(
                    nControllerIndex,
                    OnlineService::internal::function_binder_3<void, int32, BOOL, const UIID &>::bind<&OnCompleteLocalPlayerSignedIn>(nControllerIndex, bForceChooseHero, GetUIIDNone()),
                    OnlineService::internal::function_binder_2<void, int32, const UIID &>::bind<&OnCancelLocalPlayerSignedIn>(nControllerIndex, GetUIIDNone()));
            }
            else
            {
                NotificationCreationParams tParams;
                tParams.nControllerIndex = nControllerIndex;
                tParams.uszMessage = StringListFetch("ConsoleErrors:SignInSwitchWarningNX64");
                tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_4<void, int32, BOOL, Online::PartyOnlineState, const UIID &>::bind<&OnConfirmOnlineModeSwitch>(nControllerIndex, bForceChooseHero, eBestOnlineState);
                tParams.tCallbacks.pfnOnClickCancelButton = OnlineService::internal::function_binder_2<void, int32, const UIID &>::bind<&OnCancelLocalPlayerSignedIn>(nControllerIndex);
                Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
            }
        }
        return;
    }







    OnAddLocalPlayerToGame(
        nControllerIndex,
        bForceChooseHero,
        GetUIIDNone());
}




void OnLocalPlayerSignedIn(int32 nControllerIndex, BOOL bForceChooseHero, const UIID & idLightBox, GamerProfile::SignInResult eResult)
{

    GamePause(FALSE);

    if (idLightBox != GetUIIDNone())
        UI_DeleteControl(idLightBox, TRUE);

    UISaveIcon::StopForceDraw();


    if (eResult < GamerProfile::SIGNIN_SUCCESS ||
        !Online::PartyIsInParty())
    {
        SignInHandleError(nControllerIndex, eResult);
        OnCancelLocalPlayerSignedIn(nControllerIndex, GetUIIDNone());
        return;
    }

    if (!GameIsStartUp())
    {

        const D3::Account::SavedDefinition & tAccount = GamerProfile::GetAccount(nControllerIndex);

        int32 nLevel = 0;
        if (GamerProfile::IsHeroLoaded(nControllerIndex))
        {
            D3::Hero::SavedDefinition & tHero = GamerProfile::GetHero(nControllerIndex);
            nLevel = tHero.digest().level();
        }


        Online::GameParams tGameParams;
        Online::PartyGetGameParams(tGameParams);


        if (IsDifficultyLocked(tAccount.digest(), nLevel, tGameParams.m_bIsHardcore, GameGetHandicapLevel()))
        {
            NotificationCreationParams tParams;
            tParams.nControllerIndex = nControllerIndex;
            tParams.uszMessage = StringListFetch("ConsoleErrors:LocalGameJoinFailureDifficultyNotUnlocked");
            Screens::GetModalNotificationManager().SetOkNotification(tParams);

            OnCancelLocalPlayerSignedIn(nControllerIndex, GetUIIDNone());
            return;
        }


        if (tGameParams.m_bIsSeasonal && GamerProfile::IsGuest(nControllerIndex))
        {
            NotificationCreationParams tParams;
            tParams.nControllerIndex = nControllerIndex;
            tParams.uszMessage = StringListFetch("ConsoleHeroCreate:SeasonalGuestWarning");
            Screens::GetModalNotificationManager().SetOkNotification(tParams);

            OnCancelLocalPlayerSignedIn(nControllerIndex, GetUIIDNone());
            return;
        }
    }


    SignInHandleSuccess(nControllerIndex, eResult,
        OnlineService::internal::function_binder_3<void, int32, BOOL, const UIID &>::bind<&OnCompleteLocalPlayerSignedIn>(nControllerIndex, bForceChooseHero),
        OnlineService::internal::function_binder_2<void, int32, const UIID &>::bind<&OnCancelLocalPlayerSignedIn>(nControllerIndex));
}





void LocalPlayerJoin(int32 nControllerIndex, BOOL bForceChooseHero)
{
    // Ignore the request while the screens are busy or another join is under way.
    if (Screens::IsScreenTransitioning() ||
        Screens::IsModalDialogVisible())
        return;
    if (IsJoinPartyInProgress() || s_tOnlineActionsData.bLocalPlayerJoinInProgress)
    {
        return;
    }

    // Only a second local player can join an existing party.
    if (GamerProfile::GetPrimaryProfileUserIndex() == nControllerIndex ||
        !Online::PartyIsInParty() ||
        Online::PartyIsInParty(nControllerIndex))
    {
        return;
    }

    if (Online::PartyIsFull())
    {
        OnLocalPlayerAdded(nControllerIndex, Online::PartyAddPlayerCallback(), FALSE);
        return;
    }

    GamerProfile::SignInMode eSignInMode = GamerProfile::SIGNIN_MODE_ANY;
    if (Online::PartyIsOnline())
    {
        eSignInMode = (Online::PartyIsHost() && Online::PartyCanSwitchOnlineState(Online::PARTY_OFFLINE) != Online::PARTY_SWITCH_ONLINE_WILL_DISCONNECT) ?
            GamerProfile::SIGNIN_MODE_GUEST_ALLOWED : GamerProfile::SIGNIN_MODE_ONLINE_ONLY;
    }









    if (PlayersAreAllLocal())
    {
        GamePause(TRUE);
    }

    s_tOnlineActionsData.bLocalPlayerJoinInProgress = TRUE;
    UIID idLightBox = Screens::GetLightBoxManager().CreateLightBoxForWindow(cgUIIDTopLayer, TRUE);
    if (idLightBox != GetUIIDNone())
    {
        UI_GetWindowFromID(idLightBox).SetActive(TRUE);
    }
    UISaveIcon::ForceDraw(NULL);
    GamerProfile::SignIn(nControllerIndex, eSignInMode,
        OnlineService::internal::function_binder_4<void, int32, BOOL, const UIID &, GamerProfile::SignInResult>::bind<&OnLocalPlayerSignedIn>(nControllerIndex, bForceChooseHero, idLightBox));
}











void ClearLocalJoinFlag()
{
    s_tOnlineActionsData.bLocalPlayerJoinInProgress = FALSE;
}




BOOL IsLocalJoinInProgress()
{
    return s_tOnlineActionsData.bLocalPlayerJoinInProgress;
}





BOOL CanLocalPlayerJoin(
    void)
{
    if (CGameLoadingScreenIsEnabled())
    {
        return FALSE;
    }

    if (!GameIsStartUp())
    {
        if (Screens::IsScreenSetOpen())
        {
            return FALSE;
        }

        if (CGameInScriptedSequence())
        {
            return FALSE;
        }

        if (CutsceneGetFullScreenCutscene() != CUTSCENEID_INVALID)
        {
            return FALSE;
        }

        ACDID idPrimaryPlayer = LocalPlayerACDGet();
        if (idPrimaryPlayer != ACDID_INVALID)
        {
            const ActorCommonData & tACDPrimary = ACDGet(idPrimaryPlayer);

            if (tACDPrimary.AttributesGetInt(FastAttribKey(ATTRIBUTE_IN_BOSS_ENCOUNTER)))
            {
                return FALSE;
            }

            if (tACDPrimary.AttributesGetInt(FastAttribKey(ATTRIBUTE_PARTICIPATING_IN_SET_DUNGEON)))
            {
                return FALSE;
            }

            if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) &&
                ACDIsParticipatingInTieredLootRun(tACDPrimary))
            {
                return FALSE;
            }

            if (PlayerGetLocalPlayerCount() == 1 && !tACDPrimary.IsAlive())
            {
                return FALSE;
            }
        }
    }
    else if (!Screens::IsScreenSetOpen(Screens::MAIN_MENU_SCREEN_SET))
    {
        return FALSE;
    }

    return TRUE;
}





BOOL IsDifficultyLocked(const D3::Account::Digest & tAccountDigest, int32 nLevel, BOOL bIsHardcore, int32 nDifficulty)
{
    // 2.7.7: the per-difficulty account-flag checks are gone.
    uint32 nHighestHeroRiftLevel = 0;
    return !PlayerCanPlayHandicap(nLevel, nHighestHeroRiftLevel, nDifficulty, false);
}

























BOOL IsAdventureModeLocked(const D3::Account::Digest & tAccountDigest, BOOL bHardcore, BOOL bSeasonal)
{
    if (bSeasonal)
        return FALSE;
    if (bHardcore)
        return (tAccountDigest.flags() & 0x200) == 0;
    return (tAccountDigest.flags() & 0x4) == 0;
}













bool IsActiveSeason(uint32 uSeasonNum, const D3::Account::Digest & tAccountDigest)
{
    if (uSeasonNum == 0)
        return false;

    if (!Online::IsSeasonsInitialized())
        return uSeasonNum >= tAccountDigest.season_id();

    return OnlineService::GetSeasonNum() == uSeasonNum && OnlineService::GetSeasonState() < OnlineService::SEASON_STATE_ENDED;
}











void ResetQuests(D3::Hero::SavedDefinition & tSavedDefinition)
{
    // Clear the quest progress and rewards.
    tSavedDefinition.clear_quest_reward_history();
    tSavedDefinition.clear_saved_quest();

    if (tSavedDefinition.has_digest())
    {
        D3::Hero::Digest & tDigest = *tSavedDefinition.mutable_digest();
        tDigest.set_last_played_act(0);
        tDigest.set_highest_unlocked_act(0);

        SNO snoQuest = SNO_NONE;
        SNO snoAct = ActSNOFromActEnum(0);
        if (snoAct != SNO_NONE)
        {
            ScopedActDefinitionLockR ptDefAct(snoAct);
            if (ptDefAct && ActGetNumQuests(ptDefAct) > 0)
            {
                snoQuest = ((ActQuestInfo *)ptDefAct->arQuests)[0].snoQuest;
            }
        }

        tDigest.set_last_played_quest(snoQuest);
        tDigest.set_last_played_quest_step(-1);
        tDigest.clear_quest_history();
    }

    if (tSavedDefinition.has_saved_data())
    {
        D3::Hero::SavedData & modifiedSaveData = *tSavedDefinition.mutable_saved_data();
        modifiedSaveData.clear_boss_kill_flags();
        modifiedSaveData.set_activated_waypoints(D3::Hero::SavedData::default_instance().activated_waypoints());

        SavePointData tSavePoint = { SNO_NONE, -1, FALSE, ACDID_INVALID };
        tSavePoint.Export(*modifiedSaveData.mutable_save_point());
    }
}





HeroValidationError ValidateHeroForPartyMember(const Online::PartyMember & tPartyMember)
{
    const D3::Hero::Digest & tHeroDigest = *tPartyMember.tHeroDigest;

    const D3::Account::Digest * ptAccountDigest = NULL;
    if (tPartyMember.tAccountDigest)
    {
        ptAccountDigest = &*tPartyMember.tAccountDigest;
    }

    Online::GameParams tGameParams;
    if (!Online::PartyGetGameParams(tGameParams))
    {
        return HERO_VALIDATION_OK;
    }

    // 2.7.7: new first check (player_flags2 bit 0 hero created in the current season).
    if (tGameParams.unk_14)
    {
        if (tPartyMember.idPlayer)
            return HERO_VALIDATION_UNK_10;
    }
    else if (tPartyMember.idPlayer && (tHeroDigest.player_flags2() & 1) &&
             OnlineService::GetSeasonState() <= OnlineService::SEASON_STATE_ACTIVE && OnlineService::GetSeasonNum() != 0)
    {
        uint32 uSeason = OnlineService::GetSeasonState() <= OnlineService::SEASON_STATE_ACTIVE ? OnlineService::GetSeasonNum() : 0;
        if (uSeason == tHeroDigest.season_created())
            return HERO_VALIDATION_UNK_10;
    }

    if (tGameParams.m_eGameType == 6)
    {
        return HERO_VALIDATION_OK;
    }

    if (tPartyMember.bIsLocal)
    {
        if (PlayerClassFromGBID(tHeroDigest.gbid_class()) == PLAYER_CLASS_NECROMANCER &&
            !(DLCManager::GetLicenses() & 0x80))
        {
            return HERO_VALIDATION_MISSING_NECROMANCER_LICENSE;
        }
    }

    if (tHeroDigest.player_flags() & (1 << PLAYERFLAG_HARDCORE))
    {
        if (!tGameParams.m_bIsHardcore)
            return HERO_VALIDATION_MUST_NOT_BE_HARDCORE;
    }
    else if (tGameParams.m_bIsHardcore)
    {
        return HERO_VALIDATION_MUST_BE_HARDCORE;
    }

    if (tHeroDigest.player_flags() & (1 << PLAYERFLAG_DECEASED))
    {
        return HERO_VALIDATION_DEAD;
    }

    bool bHeroIsSeasonal;
    if (ptAccountDigest)
    {
        bHeroIsSeasonal = IsActiveSeason(tHeroDigest.season_created(), *ptAccountDigest);
    }
    else if (tGameParams.m_bIsSeasonal)
    {
        bHeroIsSeasonal = tHeroDigest.season_created() == tGameParams.m_uSeason;
    }
    else
    {
        bHeroIsSeasonal = IsActiveSeason(tHeroDigest.season_created(), GamerProfile::GetAccount(GamerProfile::GetPrimaryProfileUserIndex()).digest());
    }

    if (tGameParams.m_bIsSeasonal)
    {
        if (!Online::IsSeasonsInitialized())
            return HERO_VALIDATION_SEASONS_NOT_READY;

        if (OnlineService::GetSeasonState() != OnlineService::SEASON_STATE_ACTIVE)
            return HERO_VALIDATION_SEASON_OVER;

        if (!bHeroIsSeasonal)
            return HERO_VALIDATION_MUST_BE_SEASONAL;

        uint32 uSeason = OnlineService::GetSeasonNum();
        if (uSeason != tGameParams.m_uSeason || tHeroDigest.season_created() != uSeason)
            return HERO_VALIDATION_MUST_BE_SEASONAL;

        if (ptAccountDigest && ptAccountDigest->season_id() > uSeason)
            return HERO_VALIDATION_MUST_BE_SEASONAL;
    }
    else
    {
        if (bHeroIsSeasonal)
        {
            return Online::IsSeasonsInitialized() ? HERO_VALIDATION_MUST_NOT_BE_SEASONAL : HERO_VALIDATION_SEASONS_NOT_READY;
        }
    }

    if (ptAccountDigest)
    {
        if (IsDifficultyLocked(*ptAccountDigest, tHeroDigest.level(), tGameParams.m_bIsHardcore, tGameParams.m_nHandicap))
            return HERO_VALIDATION_DIFFICULTY_LOCKED;
    }

    return HERO_VALIDATION_OK;
}



























// NON_MATCHING: the CONTENT_NOT_READY_YET case's last GameParams destructor call is tail-merged with the
// CHALLENGE_RIFT_ENDED case's (one bl short).
CRefString GetGameStartErrorMessage(
    GameStartValidationError eReason,
    BOOL bForHost,
    BOOL bGameInProgress,
    BOOL bForMatchmaking)
{
    CRefString sMessage;
    switch (eReason)
    {
    case GAME_START_OK:
        {
            if (!bForHost && bGameInProgress)
            {
                sMessage = StringListFetch("ConsoleErrors:GameStartingAlreadyInProgressAlt");
            }
            break;
        }

    case GAME_START_ERROR_BUSY:
        {
            if (!bForHost)
            {
                sMessage = StringListFetch("ConsoleErrors:GameStartingPlayerBusy");
            }
            break;
        }
    case GAME_START_ERROR_HOST_PAUSED:
        {
            sMessage = StringListFetch("ConsoleErrors:GameEnterInProgressFailureGamePaused");
            break;
        }
    case GAME_START_ERROR_HERO_DEAD:
        {
            if (bForHost)
            {
                if (bForMatchmaking)
                {
                    sMessage = StringListFetch("ConsoleErrors:MatchmakeFailureDeadHardcore");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:LocalGameStartFailureDeadHardcore");
                }
            }
            else
            {

                if (bGameInProgress)
                {
                    sMessage = StringListFetch("ConsoleErrors:GameEnterInProgressFailureDeadHardcore");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:GameStartingDeadHardcoreWarning");
                }
            }
            break;
        }

    case GAME_START_ERROR_HERO_DIFFICULTY_LOCKED:
        {
            if (bForHost)
            {
                if (bForMatchmaking)
                {
                    sMessage = StringListFetch("ConsoleErrors:MatchmakeFailureDifficultyNotUnlocked");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:LocalGameStartFailureDifficultyLockedAlt");
                }
            }
            else
            {

                if (bGameInProgress)
                {
                    sMessage = StringListFetch("ConsoleErrors:GameEnterInProgressFailureDifficultyLockedAlt");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:GameStartingDifficultyNotUnlocked");
                }
            }
            break;
        }

    case GAME_START_ERROR_HERO_MUST_BE_HARDCORE:
        {
            if (bForHost)
            {
                if (bForMatchmaking)
                {
                    sMessage = StringListFetch("ConsoleErrors:MatchmakeFailureHardcore");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:LocalGameStartFailureHardcore");
                }
            }
            else
            {

                if (bGameInProgress)
                {
                    sMessage = StringListFetch("ConsoleErrors:GameEnterInProgressFailureHardcore");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:GameStartingHardcoreWarning");
                }
            }
            break;
        }
    case GAME_START_ERROR_HERO_MUST_NOT_BE_HARDCORE:
        {
            if (bForHost)
            {
                if (bForMatchmaking)
                {
                    sMessage = StringListFetch("ConsoleErrors:MatchmakeFailureNonHardcore");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:LocalGameStartFailureNonHardcore");
                }
            }
            else
            {

                if (bGameInProgress)
                {
                    sMessage = StringListFetch("ConsoleErrors:GameEnterInProgressFailureNonHardcore");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:GameStartingNonHardcoreWarning");
                }
            }
            break;
        }

    case GAME_START_ERROR_HERO_MUST_BE_SEASONAL:
        {
            if (bForHost)
            {
                if (bForMatchmaking)
                {
                    sMessage = StringListFetch("ConsoleErrors:MatchmakeFailureSeasonal");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:LocalGameStartFailureSeasonal");
                }
            }
            else
            {

                if (bGameInProgress)
                {
                    sMessage = StringListFetch("ConsoleErrors:GameEnterInProgressFailureSeasonal");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:GameStartingSeasonalWarning");
                }
            }
            break;
        }
    case GAME_START_ERROR_HERO_MUST_NOT_BE_SEASONAL:
        {
            if (bForHost)
            {
                if (bForMatchmaking)
                {
                    sMessage = StringListFetch("ConsoleErrors:MatchmakeFailureNonSeasonal");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:LocalGameStartFailureNonSeasonal");
                }
            }
            else
            {

                if (bGameInProgress)
                {
                    sMessage = StringListFetch("ConsoleErrors:GameEnterInProgressFailureNonSeasonal");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:GameStartingNonSeasonalWarning");
                }
            }
            break;
        }
    case GAME_START_ERROR_HERO_SEASON_OVER:
        {
            if (bForHost)
            {
                if (bForMatchmaking)
                {
                    sMessage = StringListFetch("ConsoleErrors:MatchmakeFailureSeasonOver");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:LocalGameStartFailureSeasonOver");
                }
            }
            else
            {

                if (bGameInProgress)
                {
                    sMessage = StringListFetch("ConsoleErrors:GameEnterInProgressFailureSeasonOver");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:GameStartingSeasonOverWarning");
                }
            }
            break;
        }
    case GAME_START_ERROR_HERO_SEASONS_NOT_READY:
        {
            if (bForHost)
            {
                if (bForMatchmaking)
                {
                    sMessage = StringListFetch("ConsoleErrors:MatchmakeFailureSeasonsNotReady");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:LocalGameStartFailureSeasonsNotReady");
                }
            }
            else
            {

                if (bGameInProgress)
                {
                    sMessage = StringListFetch("ConsoleErrors:GameEnterInProgressFailureSeasonsNotReady");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:GameStartingSeasonsNotReadyWarning");
                }
            }
            break;
        }
    case GAME_START_ERROR_SEASON_ENDING_SOON:
        {
            sMessage = LocalizedSprintf(StringListFetch("Console:SeasonEndingWarning"), UnicodeText::FormatNumber(1, TRUE, FNS_NORMAL, 0).str());
            break;
        }
    case GAME_START_ERROR_CHALLENGE_RIFT_ENDED:
        {
            Online::GameParams tGameParams;
            Online::PartyGetGameParams(tGameParams);

            sMessage = LocalizedSprintf(StringListFetch("ConsoleErrors:ChallengeRiftEnded"),
                UnicodeText::FormatNumber((uint64)tGameParams.m_tWeeklyChallengeConfig.challenge_number(), TRUE, FNS_NORMAL, 0).str());
            break;
        }
    case GAME_START_ERROR_MISSING_NECROMANCER_LICENSE:
        {
            if (bForHost)
            {
                if (bForMatchmaking)
                {
                    sMessage = StringListFetch("ConsoleErrors:MatchmakeFailureMissingNecromancerDLC");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:LocalGameStartFailureMissingNecromancerDLC");
                }
            }
            else
            {

                if (bGameInProgress)
                {
                    sMessage = StringListFetch("ConsoleErrors:GameEnterInProgressFailureMissingNecromancerDLC");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:GameStartingMissingNecromancerDLCWarning");
                }
            }
            break;
        }
    case GAME_START_ERROR_UNK_16:
        {
            if (bForHost)
            {
                if (bForMatchmaking)
                {
                    sMessage = StringListFetch("ConsoleErrors:MatchmakeFailureSolo");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:LocalGameStartFailureSolo");
                }
            }
            else
            {
                if (bGameInProgress)
                {
                    sMessage = StringListFetch("ConsoleErrors:GameEnterInProgressFailureSolo");
                }
                else
                {
                    sMessage = StringListFetch("ConsoleErrors:GameStartingSolo");
                }
            }
            break;
        }

    case GAME_START_ERROR_CONTENT_NOT_READY_YET:
        {
            Online::GameParams tGameParams;
            if (Online::PartyGetGameParams(tGameParams))
            {
                sMessage = LocalizedSprintf(
                    StringListFetch("ConsoleErrors:CannotProceedGenericContentNotReady").str(),
                    UnicodeText::FormatNumber(SNOPackChunkGetActLoadPercentComplete(tGameParams.m_eAct) * 100.0f, TRUE, FNS_NORMAL, 2, 2, 0).str(),
                    NULL);


                SNOPackChunkRequestLoad(tGameParams.m_eAct);
            }
            break;
        }
    default:
        break;
    }

    return sMessage;
}



CRefString GetHeroValidationErrorMessage(HeroValidationError eValidationError, BOOL bDetail)
{
    const char * szMessage = NULL;
    switch (eValidationError)
    {
    case HERO_VALIDATION_DIFFICULTY_LOCKED:
        {
            Online::GameParams tParams;
            if (Online::PartyGetGameParams(tParams))
            {
                if (tParams.m_nHandicap == 3)
                    szMessage = bDetail ? "ConsoleErrors:HeroValidationDifficultyLockedMasterDetail" : "ConsoleErrors:HeroValidationDifficultyLockedMaster";
                else
                    szMessage = bDetail ? "ConsoleErrors:HeroValidationDifficultyLockedTormentDetail" : "ConsoleErrors:HeroValidationDifficultyLockedTorment";
            }
        }
        break;
    case HERO_VALIDATION_DEAD:
        szMessage = bDetail ? "ConsoleErrors:HeroValidationDeadDetail" : "ConsoleErrors:HeroValidationDead";
        break;
    case HERO_VALIDATION_MUST_BE_HARDCORE:
        szMessage = bDetail ? "ConsoleErrors:HeroValidationMustBeHardcoreDetail" : "ConsoleErrors:HeroValidationMustBeHardcore";
        break;
    case HERO_VALIDATION_MUST_NOT_BE_HARDCORE:
        szMessage = bDetail ? "ConsoleErrors:HeroValidationMustNotBeHardcoreDetail" : "ConsoleErrors:HeroValidationMustNotBeHardcore";
        break;
    case HERO_VALIDATION_MUST_BE_SEASONAL:
        szMessage = bDetail ? "ConsoleErrors:HeroValidationMustBeSeasonalDetail" : "ConsoleErrors:HeroValidationMustBeSeasonal";
        break;
    case HERO_VALIDATION_MUST_NOT_BE_SEASONAL:
        szMessage = bDetail ? "ConsoleErrors:HeroValidationMustNotBeSeasonalDetail" : "ConsoleErrors:HeroValidationMustNotBeSeasonal";
        break;
    case HERO_VALIDATION_SEASON_OVER:
        szMessage = bDetail ? "ConsoleErrors:HeroValidationSeasonOverDetail" : "ConsoleErrors:HeroValidationSeasonOver";
        break;
    case HERO_VALIDATION_SEASONS_NOT_READY:
        szMessage = bDetail ? "ConsoleErrors:HeroValidationSeasonsNotReadyDetail" : "ConsoleErrors:HeroValidationSeasonsNotReady";
        break;
    case HERO_VALIDATION_MISSING_NECROMANCER_LICENSE:
    case HERO_VALIDATION_UNK_10:
        szMessage = bDetail ? "ConsoleErrors:HeroValidationSoloHeroMustBeSoloDetail" : "ConsoleErrors:HeroValidationSoloHeroMustBeSolo";
        break;
    }

    if (szMessage)
        return StringListFetch(szMessage);
    return CRefString();
}





























// 2.7.7: new function between GetHeroValidationErrorMessage and ValidateGameCanStart, called from
// UIMainMenu::Console::OnStartOrContinue (0x710035ef18): TRUE if a party member's hero has player_flags2
// bit 0 set and was created in the current season.
// NON_MATCHING: loop registers swapped (x19/x21), player_flags2 loaded as a byte instead of a word, and the
// empty-party branch goes to a separate return block.
extern "C" BOOL sub_71001bfef0()
{
    Online::PartyMember arPartyMembers[4];
    int32 nNumPartyMembers = Online::PartyGetMembers(arPartyMembers, 1, FALSE);
    BOOL bResult = FALSE;
    for (const Online::PartyMember * ptMember = arPartyMembers; ptMember != arPartyMembers + nNumPartyMembers; ++ptMember)
    {
        if (ptMember->tHeroDigest &&
            OnlineService::GetSeasonState() <= OnlineService::SEASON_STATE_ACTIVE && OnlineService::GetSeasonNum() != 0 &&
            (OnlineService::GetSeasonState() <= OnlineService::SEASON_STATE_ACTIVE ? OnlineService::GetSeasonNum() : 0) == ptMember->tHeroDigest->season_created() &&
            (ptMember->tHeroDigest->player_flags2() & 1) != 0)
        {
            bResult = TRUE;
            break;
        }
    }
    return bResult;
}
}  // namespace UIOnlineActions
}  // namespace Console
namespace UIGameSettings { namespace Console { BOOL IsQuestFilteringEnabled(); BOOL IsVisible(); } } namespace UIGamesList { namespace Console { BOOL IsVisible(); } } namespace UIBannerCustomization { namespace Console { BOOL IsVisible(); } }
namespace UIAchievements { namespace Console { BOOL IsVisible(); } } namespace UIProfileSummary { namespace Console { BOOL IsVisible(); } } namespace UIHallOfFallenHeroes { namespace Console { BOOL IsVisible(); } }
namespace Console {
namespace UIOnlineActions {
// NON_MATCHING: 2.7.7 is at 0x71001c0130 (functions.csv maps this name to 0x71001bfef0, which is the new
// sub_71001bfef0). Differences: the bSoloHero update is evaluated before the season comparison, and the
// challenge-rift-ended tail block is placed earlier.
GameStartValidationError ValidateGameCanStart()
{
    Online::GameParams tGameParams;
    if (!Online::PartyGetGameParams(tGameParams))
        return GAME_START_ERROR_UNKNOWN;




    if (Online::PartyIsGamePaused() && !Online::PartyIsHost())
        return GAME_START_ERROR_HOST_PAUSED;





    if (!SNOPackChunkLoadAct(tGameParams.m_eAct))
        return GAME_START_ERROR_CONTENT_NOT_READY_YET;







    // 2.7.7: the party is validated only outside challenge rifts, a member without a hero digest is
    // GAME_START_ERROR_UNKNOWN, and a party of 2+ with a current-season solo hero is GAME_START_ERROR_UNK_16.
    if (tGameParams.m_eGameType != 6)
    {
        uint32 nNumPartyMembers;
        HeroValidationError eWorstValidationError = HERO_VALIDATION_OK;
        bool bSoloHero = false;
        BOOL bHaveDigests = TRUE;
        {
            Online::PartyMember arPartyMembers[4];
            nNumPartyMembers = Online::PartyGetMembers(arPartyMembers, 1, FALSE);
            for (uint32 i = 0; i < nNumPartyMembers; ++i)
            {
                if (!arPartyMembers[i].tHeroDigest)
                {
                    bHaveDigests = FALSE;
                    break;
                }

                const D3::Hero::Digest & tHeroDigest = *arPartyMembers[i].tHeroDigest;
                bool bCurrentSeason = OnlineService::GetSeasonState() <= OnlineService::SEASON_STATE_ACTIVE && OnlineService::GetSeasonNum() != 0 &&
                    (OnlineService::GetSeasonState() <= OnlineService::SEASON_STATE_ACTIVE ? OnlineService::GetSeasonNum() : 0) == tHeroDigest.season_created();
                bSoloHero = bSoloHero || (bCurrentSeason && (tHeroDigest.player_flags2() & 1));

                HeroValidationError eResult = ValidateHeroForPartyMember(arPartyMembers[i]);

                if (eResult > eWorstValidationError)
                    eWorstValidationError = eResult;
            }
        }

        if (!bHaveDigests)
            return GAME_START_ERROR_UNKNOWN;

        if (nNumPartyMembers >= 2 && bSoloHero)
            return GAME_START_ERROR_UNK_16;

        switch (eWorstValidationError)
        {
        case HERO_VALIDATION_DIFFICULTY_LOCKED:
            return GAME_START_ERROR_HERO_DIFFICULTY_LOCKED;
        case HERO_VALIDATION_DEAD:
            return GAME_START_ERROR_HERO_DEAD;
        case HERO_VALIDATION_MUST_BE_HARDCORE:
            return GAME_START_ERROR_HERO_MUST_BE_HARDCORE;
        case HERO_VALIDATION_MUST_NOT_BE_HARDCORE:
            return GAME_START_ERROR_HERO_MUST_NOT_BE_HARDCORE;
        case HERO_VALIDATION_MUST_BE_SEASONAL:
            return GAME_START_ERROR_HERO_MUST_BE_SEASONAL;
        case HERO_VALIDATION_MUST_NOT_BE_SEASONAL:
            return GAME_START_ERROR_HERO_MUST_NOT_BE_SEASONAL;
        case HERO_VALIDATION_SEASON_OVER:
            return GAME_START_ERROR_HERO_SEASON_OVER;
        case HERO_VALIDATION_SEASONS_NOT_READY:
            return GAME_START_ERROR_HERO_SEASONS_NOT_READY;
        case HERO_VALIDATION_MISSING_NECROMANCER_LICENSE:
            return GAME_START_ERROR_MISSING_NECROMANCER_LICENSE;
        default:
            break;
        }
    }











    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && !Screens::IsScreenOpen(Screens::MAIN_MENU_SCREEN_SET, 1))
        return GAME_START_ERROR_BUSY;







    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) &&
        (!Screens::IsScreenOpen(Screens::MAIN_MENU_SCREEN_SET, 1) ||
         UIGameSettings::Console::IsVisible() ||
         UIGamesList::Console::IsVisible() ||
         UIBannerCustomization::Console::IsVisible() ||
         UIAchievements::Console::IsVisible() ||
         UIProfileSummary::Console::IsVisible() ||
         UIHallOfFallenHeroes::Console::IsVisible()))
        return GAME_START_ERROR_BUSY;


    if (tGameParams.m_bIsSeasonal && Online::GetSecondsUntilSeasonEnd() <= 60)
        return GAME_START_ERROR_SEASON_ENDING_SOON;



    if (tGameParams.m_eGameType == 6 && Online::GetNetworkTimestamp() >= Blizzard::Time::FromUnixTime64(tGameParams.m_tWeeklyChallengeConfig.challenge_end_unix_time_console()))
        return GAME_START_ERROR_CHALLENGE_RIFT_ENDED;





    return GAME_START_OK;
}


void ShowPartySwitchOnlineResultMessage(Online::PartySwitchOnlineResult eResult, int32 nControllerIndex, const char * szMessage)
{
    switch (eResult)
    {
    case Online::PARTY_SWITCH_ONLINE_OK:
        return;
    case Online::PARTY_SWITCH_ONLINE_NOT_SIGNED_IN_ONLINE:
        szMessage = Online::PartyGetLocalMemberCount() == 1 ? "ConsoleErrors:OnlineStateErrorNotSignedInNX64" : "ConsoleErrors:OnlineStateErrorNotAllSignedInNX64";
        break;
    case Online::PARTY_SWITCH_ONLINE_PARENTAL_BLOCK:
        szMessage = Online::PartyGetLocalMemberCount() == 1 ? "ConsoleErrors:OnlineStateErrorParentalBlockSwitch" : "ConsoleErrors:OnlineStateErrorSomeoneParentalBlockSwitch";
        break;
    case Online::PARTY_SWITCH_NO_NETWORK_CONNECTION:
        Utilities::ShowSystemOnlineErrorDialog();
        break;
    case Online::PARTY_SWITCH_NO_INTERNET_CONNECTION:
        szMessage = "ConsoleErrors:OnlineStateErrorNoInternet";
        break;
    case Online::PARTY_SWITCH_UNKNOWN_ERROR:
        szMessage = "ConsoleErrors:OnlineStateErrorUnknown";
        break;
    default:
        break;
    }

    if (nControllerIndex == -1)
        nControllerIndex = GamerProfile::GetPrimaryProfileUserIndex();

    if (szMessage)
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszMessage = StringListFetch(szMessage);
        Screens::GetModalNotificationManager().SetOkNotification(tParams);
    }
}











































}  // namespace UIOnlineActions
}  // namespace Console
