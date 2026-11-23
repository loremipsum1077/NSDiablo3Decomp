// Original file: X/Client/UserInterface/Console/UIConsoleStartScreen.cpp
// Local declarations stand in for headers not adopted yet; each names its defining file.
#include "../../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../Battlenet/Subsystems/FrontendBackgroundCameraManager.h"
#include "../../Battlenet/Subsystems/FrontendBackgroundHeroManager.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/GamerProfileInternal.h"
#include "../../Console/Online.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../Sound/Sound.h"
#include "ConsoleModalNotificationManager.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/OnlineService/SubscriptionEvent.h"
#include "../../../Common/Player.h"
#include "../../../Common/Protobuf/Hero.pb.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Sigma/Random.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/buildver.h"
extern UIID UIDLG_CONSOLE_START_BUTTON, UIDLG_CONSOLE_START_TEXT, UITXT_CONSOLE_START_COPYRIGHT, UIDLG_CONSOLE_START_LOGO, UIDLG_CONSOLE_START_DEMO_LOGO, UITXT_CONSOLE_START_DEMO_LOGO_TEXT;  // UIIDs.h
namespace nn { namespace hid { void ShowControllerStrapGuide(); } } namespace Console { namespace Screens { BOOL OpenToScreen(const ScreenSets eScreenSet, const ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition); } }  // NintendoSDK nn/hid; ConsoleScreenManager.h
namespace Console { namespace GamerProfile { enum SignInResult { SIGNIN_CANCELLED, SIGNIN_FAILED, SIGNIN_FAILED_PROFILE_IN_USE, SIGNIN_FAILED_GUEST_NOT_ALLOWED, SIGNIN_FAILED_NO_ONLINE_PRIVILEGE, SIGNIN_FAILED_NO_SPACE, SIGNIN_SUCCESS, SIGNIN_SUCCESS_NO_PROFILE, SIGNIN_SUCCESS_NO_DEVICE, SIGNIN_SUCCESS_GUEST, SIGNIN_SUCCESS_UNOWNED_SAVE, SIGNIN_SUCCESS_CORRUPT_DATA, SIGNIN_SUCCESS_DEMO_SAVE_IMPORTED, SIGNIN_SUCCESS_NO_SAVE };
enum SignInMode { SIGNIN_MODE_ANY, SIGNIN_MODE_GUEST_ALLOWED, SIGNIN_MODE_ONLINE_ONLY, SIGNIN_MODE_VIA_INVITE }; typedef OnlineService::Callback<void (SignInResult)> SignInCallback;
void SignIn(int32 nUserIdx, SignInMode eMode, SignInCallback pfnCallback); void SignOut(int32 nUserIdx); uint32 GetNumAvailableHeroes(int32 nUserIdx); void SavePrefs(); void SetRandomBlizzconHero(int32 nUserIdx); void ClearPrimaryProfileUserIndex(); } }  // GamerProfile.h
namespace Console { namespace Online { void ClearOutstandingInvite(); void PartyCreate(int32 nControllerIndex, GameParams & tGameParams, BOOL bOfflineOnly); void PartySwitchOnlineState(int32 eState); void PartyLeave(); void SwitchToNetworkMode(NetworkMode eMode, BOOL bForce); BOOL HasOutstandingInvite(int32 & nUserIdx); PartyInviteAcceptedEvent & GetPartyInviteAcceptedEvent(); PlayTogetherEvent & GetPlayTogetherEvent(); } }  // Online.h
namespace Console { namespace UIOnlineActions { void SignInHandleError(int32 nControllerIndex, GamerProfile::SignInResult eResult); void SignInHandleSuccess(int32 nControllerIndex, GamerProfile::SignInResult eResult, NotificationCallback pfnSignInCompleteCallback, NotificationCallback pfnCancelSignInCallback); BOOL IsLocalJoinInProgress(); } }  // OnlineActions.h
namespace UIWhatsNew { namespace Console { BOOL IsVisible(); BOOL ShouldShow(); BOOL ShouldShowAnniversary(); BOOL ShouldShowNecromancer(); void OpenForController(int32 nControllerIndex, BOOL bAnniversary, BOOL bNecromancer); void Close(); } }  // UIConsoleWhatsNew.h (OpenForController's parameter names are not in the DWARF listing)
namespace UIWhatsNewNX64 { namespace Console { BOOL ShouldShow(); void OpenForController(int32 nControllerIndex); } } namespace UIEULA { namespace Console { BOOL IsVisible(); BOOL ShouldShow(); void OpenForController(int32 nControllerIndex); } }  // UIConsoleWhatsNewNX64.h, UIConsoleEULA.h
namespace UIFirstTimeFlow { namespace Console { BOOL IsActive(); void Cancel(); void PerformFirstTimeFlow(int32 nControllerIndex); } } namespace UIHeroCreate { namespace Console { BOOL CreateHero(int32 nControllerIndex, PlayerClass eClass, uint32 uFlags, CRefString sString); } }  // UIConsoleFirstTimeFlow.h, UIConsoleHeroCreate.h
namespace UIOptions { namespace Console { void ApplyTextLocale(); void OnPreferencesLoaded(); } } namespace UIGameSettings { namespace Console { void ClearGameSettingsViewed(); } } namespace UICredits { namespace Console { void Skip(const UIID &, const int32); } }
namespace ControllerDisconnectedHandler { namespace Console { void ControllerConnected(int32 nControllerIndex); } } namespace UIPartyDisplay { namespace Console { void HideFriendDisplay(); void HideLocalCommunicationsDisplay(); void HidePartyDisplay(); } } namespace UIMainMenu { namespace Console { void ResetMainMenuFlag(); } } namespace UIUtils { namespace Console { BOOL IsJapaneseVersion(); } }
typedef int32 CutsceneID; typedef void (*CutsceneCallback)(); CutsceneID CutsceneGetFullScreenCutscene(); void CutscenePlayActCutscene(const Act eAct, const BOOL fUseNoLogoVersion, CutsceneCallback pfnOnFinishedCallback, int32 nControllerIndex); void CutsceneStop(const CutsceneID idCutscene, const BOOL fStoppingAll); void CutsceneTryToCancel(const UIID &);  // X/Client/Cutscene.h
SNO GetActBackgroundSNO(Act eAct); void UIStartup_SetBackgroundWorld(SNO snoWorld); void UI_SetPrimaryUIController(int32 nPrimaryController); void GfxSetPrefsChanged();  // UIStartup.cpp, UI.cpp, Gfx

UI & UIGet();  // UI.cpp

namespace UIStartScreen {
namespace Console {

void OnGameInviteAccepted(int32 nUserIdx);
void sAdvanceIntoGame(const int32 nControllerIndex);
void sCancelUserSignIn(int32 nControllerIndex, BOOL bForInvite, const UIID &);
void sDoSignIn(BOOL bForInvite, const int32 nControllerIndex);
























static BOOL s_bSigningIn = FALSE;
static BOOL s_bCutscenePlaying = FALSE;
static BOOL s_bFirstTimeFlowComplete = FALSE;
static BOOL s_bForcePlayMovieForFirstTimeFlow = FALSE;
static BOOL s_bIsDisposing = FALSE;
static BOOL s_bShownWhatsNew = FALSE;
static BOOL s_bShownNecromancer = FALSE;

static BOOL s_bShownWristStrapMessage = FALSE;
static BOOL s_bShownWhatsNewNX64 = FALSE;

static Blizzard::Time::Millisecond s_nCutsceneTimerStart = 0;

static const Blizzard::Time::Millisecond CUTSCENE_INTERVAL = 30000;



void ShowStartText(BOOL bShow)

{
    UI_GetWindowFromID(UIDLG_CONSOLE_START_TEXT).SetActive(bShow);

    UIID idLocalizedText, idEnglishText;
    idLocalizedText.SetFromParent(UIDLG_CONSOLE_START_TEXT, "LocalizedPressStart");
    idEnglishText.SetFromParent(UIDLG_CONSOLE_START_TEXT, "EnglishPressStart");
    BOOL bIsRegionJapan = UIUtils::Console::IsJapaneseVersion();
    UI_GetWindowFromID(idLocalizedText).SetActive(!bIsRegionJapan);
    UI_GetWindowFromID(idEnglishText).SetActive(bIsRegionJapan);
}



void sProceedToNextScreen(int32 nControllerIndex)

{
    BOOL bCheckForPrefsSave = TRUE;

    if (::Console::GamerProfile::IsHeroLoaded(nControllerIndex)
        && !(::Console::GamerProfile::GetHero(nControllerIndex).digest().player_flags() & 8))
    {

        ::Console::Screens::OpenNextScreen(nControllerIndex, FALSE);
    }
    else if (::Console::GamerProfile::GetNumAvailableHeroes(nControllerIndex) != 0)
    {

        ::Console::Screens::OpenToScreen(::Console::Screens::HERO_SELECT_SCREEN_SET, 0, nControllerIndex, FALSE);
    }
    else
    {



        ::Console::Screens::OpenToScreen(::Console::Screens::HERO_CREATE_SCREEN_SET, 0, nControllerIndex, FALSE);
        bCheckForPrefsSave = FALSE;
    }





    if (bCheckForPrefsSave
        && !Prefs::GetFirstTimeFlowCompleted())
    {
        Prefs::GetFirstTimeFlowCompleted() = TRUE;
        ::Console::GamerProfile::SavePrefs();
    }


    if (UIWhatsNew::Console::ShouldShowNecromancer() && !s_bShownNecromancer)
    {
        UIWhatsNew::Console::OpenForController(nControllerIndex, FALSE, TRUE);
    }

    else if (UIWhatsNewNX64::Console::ShouldShow() && !s_bShownWhatsNewNX64)
    {
        UIWhatsNewNX64::Console::OpenForController(nControllerIndex);
    }

    else if (UIWhatsNew::Console::ShouldShow() && !s_bShownWhatsNew)
    {
        UIWhatsNew::Console::OpenForController(nControllerIndex, FALSE, FALSE);
    }
}




void OnWhatsNewCompleteNX64(int32 nControllerIndex)

{
    if (UIWhatsNew::Console::ShouldShow() && !s_bShownWhatsNew)
    {
        UIWhatsNew::Console::OpenForController(nControllerIndex, FALSE, FALSE);
    }


    s_bShownWhatsNewNX64 = TRUE;
}



void OnWhatsNewComplete(int32 nControllerIndex)

{


    if (!s_bShownNecromancer && UIWhatsNew::Console::ShouldShowNecromancer())
    {
        s_bShownNecromancer = TRUE;

        if (UIWhatsNew::Console::ShouldShow() && !s_bShownWhatsNew)
        {
            UIWhatsNew::Console::OpenForController(nControllerIndex, FALSE, FALSE);
            return;
        }
    }

    if (UIWhatsNew::Console::ShouldShowAnniversary())
    {
        UIWhatsNew::Console::OpenForController(nControllerIndex, TRUE, FALSE);
    }

    s_bShownWhatsNew = TRUE;
}




void OnEULAComplete(const int32 nControllerIndex, BOOL bAccepted)


{
    if (bAccepted)
    {
        if (!Prefs::GetFirstTimeFlowCompleted())
        {
            UIFirstTimeFlow::Console::PerformFirstTimeFlow(nControllerIndex);
            s_bForcePlayMovieForFirstTimeFlow = TRUE;
        }
        else
        {




            sProceedToNextScreen(nControllerIndex);
        }
    }
    else
    {
        s_bForcePlayMovieForFirstTimeFlow = FALSE;

        sCancelUserSignIn(nControllerIndex, FALSE, GetUIIDNone());
    }
}


void sAdvanceIntoGame(const int32 nControllerIndex)

{
    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        if (UIEULA::Console::ShouldShow())
        {
            UIEULA::Console::OpenForController(nControllerIndex);
        }
        else
        {
            OnEULAComplete(nControllerIndex, TRUE);
        }
    }
    else
    {




        RNG rng;

        if (!::Console::Online::PartyIsInParty())
        {

            ::Console::Online::GameParams tGameParams;
            tGameParams.m_bIsHardcore = FALSE;
            tGameParams.m_bResumeFromSave = FALSE;
            tGameParams.m_eAct = ACT_OPEN_WORLD;
            tGameParams.m_snoQuest = -1;
            tGameParams.m_nQuestStep = -1;
            tGameParams.m_nHandicap = 1;
            tGameParams.m_nTag = 0;
            tGameParams.m_uEntranceGUIDOverride = (uint32)-1;


            ::Console::Online::PartyCreate(nControllerIndex, tGameParams, TRUE);

            ::Console::Online::PartySwitchOnlineState(::Console::Online::PARTY_OFFLINE);
        }


        RandInit(rng);
        for (int i = 0; i < 7; i++)
        {
            BOOL male = RandGetDWord(rng) & 1;

            uint32 uFlags = male ? 0 : 2;
            UIHeroCreate::Console::CreateHero(nControllerIndex, i, uFlags, PlayerGetClassUIName(i, male));
        }


        ::Console::GamerProfile::SetRandomBlizzconHero(nControllerIndex);

        ::Console::Screens::OpenToScreen(::Console::Screens::HERO_SELECT_SCREEN_SET, 0, nControllerIndex, FALSE);
    }
}



void sCompleteUserSignIn(int32 nControllerIndex, const UIID & tId)


{

    s_bSigningIn = FALSE;

    UIOptions::Console::ApplyTextLocale();
    UIOptions::Console::OnPreferencesLoaded();

    if (!ShowSystemLocaleAOCOffer(nControllerIndex, OnlineService::internal::function_bind_0_1<void, int32, &sAdvanceIntoGame>()))
    {
        sAdvanceIntoGame(nControllerIndex);
    }
}





void sCancelUserSignIn(int32 nControllerIndex, BOOL bForInvite, const UIID &)



{
    ::Console::GamerProfile::SignOut(nControllerIndex);
    s_bSigningIn = FALSE;

    s_nCutsceneTimerStart = Blizzard::Time::Milliseconds();
    ShowStartText(TRUE);

    if (bForInvite)
    {

        ::Console::Online::ClearOutstandingInvite();
    }
}

void sOnUserSignedIn(int32 nControllerIndex, BOOL bForInvite, ModalNotificationId idNotification, ::Console::GamerProfile::SignInResult eResult)




{
    if (idNotification != INVALID_MODAL_NOTIFICATION_ID)
    {
        ::Console::Screens::GetModalNotificationManager().RemoveNotification(idNotification);
    }

    if (eResult < ::Console::GamerProfile::SIGNIN_SUCCESS || eResult == ::Console::GamerProfile::SIGNIN_SUCCESS_GUEST)
    {

        ::Console::UIOnlineActions::SignInHandleError(nControllerIndex, eResult);
        sCancelUserSignIn(nControllerIndex, bForInvite, GetUIIDNone());
    }
    else
    {



        ::Console::UIOnlineActions::SignInHandleSuccess(
            nControllerIndex,
            eResult,
            OnlineService::internal::function_binder_2<void, int32, const UIID &>::bind<&sCompleteUserSignIn>(nControllerIndex),
            OnlineService::internal::function_binder_3<void, int32, BOOL, const UIID &>::bind<&sCancelUserSignIn>(nControllerIndex, bForInvite));
    }
}

void sOnInitalCutsceneFinished()

{
    UI_GetControlFromID(UIDLG_CONSOLE_START_BUTTON).SetActive(TRUE);

    s_nCutsceneTimerStart = Blizzard::Time::Milliseconds();
    s_bCutscenePlaying = FALSE;
    int32 nPrimaryIndex = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    if (nPrimaryIndex != -1 && s_bFirstTimeFlowComplete && s_bIsDisposing == FALSE)
    {


        s_bFirstTimeFlowComplete = FALSE;


        sProceedToNextScreen(nPrimaryIndex);
    }
}

void sDoSignIn(BOOL bForInvite, const int32 nControllerIndex)


{
    ModalNotificationId idNotification = INVALID_MODAL_NOTIFICATION_ID;



    ::Console::GamerProfile::SignInMode eSignInMode = bForInvite ? ::Console::GamerProfile::SIGNIN_MODE_VIA_INVITE : ::Console::GamerProfile::SIGNIN_MODE_ANY;

    s_bSigningIn = TRUE;




    ShowStartText(FALSE);


    ::Console::GamerProfile::SignIn(nControllerIndex, eSignInMode,
        OnlineService::internal::function_binder_4<void, int32, BOOL, ModalNotificationId, ::Console::GamerProfile::SignInResult>::bind<&sOnUserSignedIn>(nControllerIndex, bForInvite, idNotification));
}

void OnButtonPress(const UIID &, const int32 nControllerIndex)

{
    if (s_bSigningIn
        || ::Console::Screens::IsScreenTransitioning()
        || ::Console::UIOnlineActions::IsLocalJoinInProgress())
    {
        return;
    }


    if (s_bCutscenePlaying == TRUE)
    {
        CutsceneID idCutscene = CutsceneGetFullScreenCutscene();
        CutsceneStop(idCutscene, FALSE);
    }
    else
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_START_BUTTON_PRESSED), cgtPSODefaults);
        sDoSignIn(FALSE, nControllerIndex);
    }
}
void OnGameInviteAccepted(int32 nUserIdx)

{
    int32 nPrimaryUserIndex = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    if (nPrimaryUserIndex != -1)
    {
        if (nPrimaryUserIndex != nUserIdx)
        {
            ::Console::GamerProfile::Internal::HandleUserSignedOut(nPrimaryUserIndex);
        }
        return;
    }

    CutsceneTryToCancel(GetUIIDNone());



















    sDoSignIn(TRUE, nUserIdx);
}
// NON_MATCHING: the original tests (bCutsceneTimerElapsed | s_bForcePlayMovieForFirstTimeFlow) before the UIEULA result (tbz then cbnz); ours tests them in the other order. The bitwise form below is the closest of 16 variants, not the original spelling.
void OnUpdate(
    const UIID &)
{
    BOOL bCutsceneTimerElapsed;  // DWARF decl 473; its value is computed on 476


    bCutsceneTimerElapsed = Blizzard::Time::GetTimeElapsed(s_nCutsceneTimerStart, Blizzard::Time::Milliseconds()) > CUTSCENE_INTERVAL;


    if (!UIFirstTimeFlow::Console::IsActive()
        && !UIWhatsNew::Console::IsVisible()
        && (!UIEULA::Console::IsVisible() & (bCutsceneTimerElapsed | s_bForcePlayMovieForFirstTimeFlow))
        && s_bCutscenePlaying == FALSE && s_bSigningIn == FALSE)
    {


        if (::Console::Screens::GetModalNotificationManager().IsNotificationShowing())
        {
            s_nCutsceneTimerStart = Blizzard::Time::Milliseconds();
        }
        else
        {


            if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
            {
                CutscenePlayActCutscene(ACT_5, FALSE, sOnInitalCutsceneFinished, 0);
            }
            else
            {
                CutscenePlayActCutscene(ACT_1, FALSE, sOnInitalCutsceneFinished, 0);
            }
            s_bCutscenePlaying = TRUE;
            s_bFirstTimeFlowComplete = s_bForcePlayMovieForFirstTimeFlow;
            s_bForcePlayMovieForFirstTimeFlow = FALSE;

            UI_GetControlFromID(UIDLG_CONSOLE_START_BUTTON).SetActive(FALSE);
        }
    }
}


static void sUpdateCopyrightAndVersionInfo()

{
    CRefString sCopyrightAndVersion = StringListFetch("Console:CopyrightLabel");
    sCopyrightAndVersion += "\n";

    CRefString sVersion;

    sVersion = Blizzard::String::QFormat("%s", BuildVersionFull());



    sCopyrightAndVersion += sVersion;

    UI_GetTextFromID(UITXT_CONSOLE_START_COPYRIGHT).SetText(sCopyrightAndVersion, FALSE);
}



void OpenForController(int32)

{



    if (!s_bShownWristStrapMessage && !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {

        nn::hid::ShowControllerStrapGuide();
        s_bShownWristStrapMessage = TRUE;
    }


    UI_GetControlFromID(UIDLG_CONSOLE_START_BUTTON).SetActive(TRUE);

    ::Console::Screens::GetActionManager().ClearAllActions();
    ::Console::Screens::UpdateButtonLegendText();


    s_bSigningIn = FALSE;

    SNO snoWorld = GetActBackgroundSNO(ACT_INVALID);
    UIStartup_SetBackgroundWorld(snoWorld);
    ::Console::Screens::GetFrontendBackgroundCameraManager().TransitionBackgroundCamera(CAM_POS_WIDE, 0.0f);
    ::Console::Screens::GetFrontendBackgroundHeroManager().ClearAllBackgroundHeros();
    ::Console::Screens::GetFrontendBackgroundHeroManager().ClearBackgroundBanner();

    sUpdateCopyrightAndVersionInfo();

    UI_SetPrimaryUIController(-1);
    ::Console::GamerProfile::ClearPrimaryProfileUserIndex();
    UIGameSettings::Console::ClearGameSettingsViewed();
    UICredits::Console::Skip(GetUIIDNone(), -1);

    if (::Console::Online::PartyIsInParty())
    {
        ::Console::Online::PartyLeave();
    }


    if (::Console::Online::GetNetworkMode() == ::Console::Online::NET_ADHOC)
    {

        ::Console::Online::SwitchToNetworkMode(::Console::Online::NET_OFFLINE, TRUE);
    }


    for (int32 i = 0; i < 4; i++)
    {
        ::Console::GamerProfile::SignOut(i);
        ControllerDisconnectedHandler::Console::ControllerConnected(i);
    }


    if (!CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT))
    {
        UIPartyDisplay::Console::HidePartyDisplay();
        UIPartyDisplay::Console::HideFriendDisplay();
        UIPartyDisplay::Console::HideLocalCommunicationsDisplay();
    }
    else
    {

        UI_GetWindowFromID(UIDLG_CONSOLE_START_LOGO).SetActive(FALSE);
        UI_GetWindowFromID(UIDLG_CONSOLE_START_DEMO_LOGO).SetActive(TRUE);
        UI_GetTextFromID(UITXT_CONSOLE_START_DEMO_LOGO_TEXT).SetText(StringListFetch("ConsoleDemo:Demo").str(), FALSE);
    }
    UIMainMenu::Console::ResetMainMenuFlag();

    if (UIWhatsNew::Console::IsVisible())
    {
        UIWhatsNew::Console::Close();
    }

    ::Console::Online::GetPartyInviteAcceptedEvent().Subscribe(OnlineService::internal::function_bind_0_1<void, int32, &OnGameInviteAccepted>());
    ::Console::Online::GetPlayTogetherEvent().Subscribe(OnlineService::internal::function_bind_0_1<void, int32, &OnGameInviteAccepted>());

    int32 nUserIndex = -1;
    if (::Console::Online::HasOutstandingInvite(nUserIndex))
    {
        ShowStartText(FALSE);
        OnGameInviteAccepted(nUserIndex);
    }
    else
    {






        ShowStartText(TRUE);
    }


    const VideoPrefs & tVideoPrefs = Prefs::GetVideoPrefs();
    float flSafeZoneLeft, flSafeZoneTop, flSafeZoneRight, flSafeZoneBottom;
    tVideoPrefs.GetSafeZoneDefualtValues(flSafeZoneLeft, flSafeZoneTop, flSafeZoneRight, flSafeZoneBottom);
    UIGet().tSafeZone.SetMargins(
        flSafeZoneLeft,
        flSafeZoneTop,
        1.0f - flSafeZoneRight,
        1.0f - flSafeZoneBottom);
    Prefs::GetMutableVideoPrefs().SetGamma(tVideoPrefs.GetGammaDefault());
    GfxSetPrefsChanged();

    s_nCutsceneTimerStart = Blizzard::Time::Milliseconds();
    s_bCutscenePlaying = FALSE;
}

void Close()

{
    ::Console::Online::GetPlayTogetherEvent().Unsubscribe(OnlineService::internal::function_bind_0_1<void, int32, &OnGameInviteAccepted>());
    ::Console::Online::GetPartyInviteAcceptedEvent().Unsubscribe(OnlineService::internal::function_bind_0_1<void, int32, &OnGameInviteAccepted>());
    UI_GetControlFromID(UIDLG_CONSOLE_START_BUTTON).SetActive(FALSE);

    if (UIFirstTimeFlow::Console::IsActive())
    {
        UIFirstTimeFlow::Console::Cancel();
    }
}

void StartScreenInitializeForGame()
{
    s_bForcePlayMovieForFirstTimeFlow = FALSE;
    s_bIsDisposing = FALSE;
}


void StartScreenDisposeForGame()
{
    s_bIsDisposing = TRUE;
}

}  // namespace Console
}  // namespace UIStartScreen
