// Original file: X/Client/UserInterface/Console/UIConsoleJoinRift.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "../../Console/GamerProfile.h"
#include "../../Sound/Sound.h"
#include "ConsoleModalNotificationManager.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../Controls/UIItemButtonControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Interface/UIBossElements.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GameBalance.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/NephalemRiftCommon.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
#include "../Interface/UITutorials.h"
// Declared here because their headers are not adopted yet: UIID globals (X/Client/UserInterface/UIGlobals.cpp); BoolDataMessage (X/Common/Networking/GameMessage.h); MessageSendToServer (X/Client/Network/CGameMessage.cpp); UITutorialManager (X/Client/UserInterface/Interface/UITutorials.h, only the member this file calls); UI_GetItemButtonFromID, UI_GetStoryboardFromID (X/Client/UserInterface/UI.cpp); ACDInventoryGetItemDrawInfoWithGBID (X/Client/CActorInventory.cpp); the CanCloseSafely/IsActive/IsVisible entry points of other screens; Console::Screens::OpenToScreen (ConsoleScreenManager.cpp).
extern UIID UIDLG_JOIN_RIFT_MAIN, UITEXT_JOIN_RIFT_TITLE, UITEXT_JOIN_RIFT_LOCAL_PLAYER_DECLINED_WARNING, UITEXT_JOIN_RIFT_ENTRY_COUNTDOWN, UITEXT_JOIN_RIFT_USE_KEY_MSG, UITEXT_JOIN_RIFT_ELIGIBILITY_MSG, UIWND_JOIN_RIFT_PLAYER_PANEL_1, UIWND_JOIN_RIFT_PLAYER_PANEL_2, UIWND_JOIN_RIFT_PLAYER_PANEL_3, UIWND_JOIN_RIFT_PLAYER_PANEL_4, UIWND_JOIN_RIFT_BUTTON_LEGEND, UISTORYBOARD_JOIN_RIFT_LOCAL_PLAYER_DECLINED_WARNING, UISTORYBOARD_JOIN_RIFT_ENTER_RIFT_COUNTDOWN, UISTORYBOARD_JOIN_RIFT_REMOTE_PLAYER_DECLINED, UISTORYBOARD_JOIN_RIFT_HOST_CANCELED, UISTORYBOARD_JOIN_RIFT_INITIATOR_PROMPT;
 void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
class UITutorial; extern UITutorialManager gtTutorialMgr;
UIItemButton & UI_GetItemButtonFromID(const UIID & id); UIStoryboard & UI_GetStoryboardFromID(const UIID & id); void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const int32 eDrawType);
namespace UIInventory { namespace Console { BOOL UIMysticCanCloseSafely(); } } namespace UIHeroCreate { namespace Console { BOOL CanCloseSafely(); } } namespace UIHeroSelect { namespace Console { BOOL CanCloseSafely(); } } namespace UIWaypointMap { BOOL CanCloseSafely(); } namespace UISeasonJourney { namespace Console { BOOL IsActive(); } } namespace UIConfirmation { BOOL IsVisible(); } namespace UIConsoleRiftEntry { BOOL PlayerInstigatorEmpoweredRift(int32 nPlayerIndex); }
namespace Console { namespace Screens { BOOL OpenToScreen(const ScreenSets eScreenSet, const ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition); } }

namespace UIJoinRift {
void InitializeForGame();
void DisposeForGame();
void Open(const RiftJoinMessage * ptRiftJoinMsg);
BOOL IsActive();
void Close();
void InviteStatusUpdate(const EncounterInviteStateMessage * ptStateMsg);
void Started();
void Canceled();
void RemotePlayersDeclined(int32 nDeclinedPlayerMap);
void OnUpdate(const UIID & id);
void OnButton1(const UIID & id, const int32 nControllerIndex);
void OnButton2(const UIID & id, const int32 nControllerIndex);
void OnButton4(const UIID & id, const int32 nControllerIndex);
}  // namespace UIJoinRift





























UIID aridPlayerPanels[4] = { UIID(), UIID(), UIID(), UIID() };  // The DWARF gives only `UIID aridPlayerPanels[4]` (line 84). The initialiser list is invented: the original has no static initialiser for this file and keeps sgtJoinRiftGlobals constant-initialised in .data, which a default-constructed array (a constructor loop) prevents.

enum class ButtonAction : int {
    None = 0,
    Close = 1,
    Yes = 2,
    No = 3,
};

enum class PlayerStatus : int {
    NoAnswer = 0,
    Ineligible = 1,
    Accepted = 2,
    Declined = 3,
};

enum class JoinState : int {
    WaitingOnPlayers = 0,
    AllDeclined = 1,
    RemoteDeclined = 2,
    StartingRift = 3,
    DidNotJoinRift = 4,
    RiftCanceled = 5,
};

struct JoinRiftGlobals {  // size 0x4c
    int32 nInstigatorPlayerIndex;  // 0x00
    int32 nRiftTier;  // 0x04
    uint32 dwStopTime;  // 0x08
    PlayerStatus arPlayerStatus[4];  // 0x0c
    JoinState eState;  // 0x1c
    int32 nPlayerKeys[4];  // 0x20
    BOOL bPlayerEmpowering[4];  // 0x30
    ButtonAction eActionBtn1;  // 0x40
    ButtonAction eActionBtn2;  // 0x44
    BOOL bManuallyClosed;  // 0x48
    JoinRiftGlobals()  // body invented from the image's initial bytes (tools/data.py sgtJoinRiftGlobals); the object is constant-initialised in .data
        : nInstigatorPlayerIndex(-1), nRiftTier(-1), dwStopTime(0), eState(JoinState::WaitingOnPlayers), eActionBtn1(ButtonAction::None), eActionBtn2(ButtonAction::None), bManuallyClosed(FALSE)
    {
        arPlayerStatus[0] = PlayerStatus::NoAnswer; nPlayerKeys[0] = -1; bPlayerEmpowering[0] = FALSE;
        arPlayerStatus[1] = PlayerStatus::NoAnswer; nPlayerKeys[1] = -1; bPlayerEmpowering[1] = FALSE;
        arPlayerStatus[2] = PlayerStatus::NoAnswer; nPlayerKeys[2] = -1; bPlayerEmpowering[2] = FALSE;
        arPlayerStatus[3] = PlayerStatus::NoAnswer; nPlayerKeys[3] = -1; bPlayerEmpowering[3] = FALSE;
    }
};
static_assert(sizeof(JoinRiftGlobals) == 0x4c, "JoinRiftGlobals");







JoinRiftGlobals sgtJoinRiftGlobals;

static void sPlaySelectionAnimation(int32 nPlayerIndex, PlayerStatus ePlayerStatus);
static void sSetItemButtons();
static void sShowAutoStatus();
static BOOL sHasALocalPlayerDeclined();
static void sUpdateControllerActions();
static void sResetStoryboard(const UIID & id);
static void sCloseSelf();
static void sResetUI();
static void sUpdatePlayerStatus(int32 nPlayerIndex);
static UIID sGetStatusSelectedIconUIID(int32 nPlayerIndex);
static UIID sGetStatusSelectStoryboardUIID(int32 nPlayerIndex);
static UIID sGetStatusOverrideIconUIID(int32 nPlayerIndex);
static UIID sGetStatusOverrideStoryboardUIID(int32 nPlayerIndex);
















void UIJoinRift::InitializeForGame()
{

    aridPlayerPanels[0] = UIWND_JOIN_RIFT_PLAYER_PANEL_1;
    aridPlayerPanels[1] = UIWND_JOIN_RIFT_PLAYER_PANEL_2;
    aridPlayerPanels[2] = UIWND_JOIN_RIFT_PLAYER_PANEL_3;
    aridPlayerPanels[3] = UIWND_JOIN_RIFT_PLAYER_PANEL_4;
}




void UIJoinRift::DisposeForGame()
{
}





static void sSetTimer(uint32 dwTicksRemaining)
{

    uint32 dwStartTime = CGameGetClientTicks();
    sgtJoinRiftGlobals.dwStopTime = dwStartTime + dwTicksRemaining;
}





BOOL sSafeToJoin()
{

    if (!UIInventory::Console::UIMysticCanCloseSafely())
    {
        return FALSE;
    }

    if (!UIHeroCreate::Console::CanCloseSafely())
    {
        return FALSE;
    }

    if (!UIHeroSelect::Console::CanCloseSafely())
    {
        return FALSE;
    }

    if (!UIWaypointMap::CanCloseSafely())
    {
        return FALSE;
    }

    if (UIBossEncounter::IsActive())
    {
        return FALSE;
    }

    if (UISeasonJourney::Console::IsActive())
    {
        return FALSE;
    }

    ConsoleModalNotificationManager & tManager = ::Console::Screens::GetModalNotificationManager();
    if (tManager.IsNotificationShowing())
    {
        return FALSE;
    }

    if (UIConfirmation::IsVisible())
    {
        return FALSE;
    }

    return TRUE;
}











void UIJoinRift::Open(const RiftJoinMessage * ptRiftJoinMsg)
{

    if (IsActive())
    {
        Close();
    }

    Player * ptPrimaryPlayer = PlayerGetByControllerIndex(::Console::GamerProfile::GetPrimaryProfileUserIndex());
    ActorCommonData * ptACDPrimaryPlayer = ACDTryToGet(ptPrimaryPlayer->idPlayerACD);
    if (!sSafeToJoin() ||
        (ptACDPrimaryPlayer && ptACDPrimaryPlayer->AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER)))
    {
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (PlayersAreLocalToEachOther(*ptPrimaryPlayer, *ptPlayer))
            {
                SimpleMessage tMessage;
                MessageSendToServer(CMSG_RIFT_JOIN_DECLINED, &tMessage, sizeof(tMessage), ptPlayer);
            }
        }
        return;
    }


    sgtJoinRiftGlobals.nInstigatorPlayerIndex = ptRiftJoinMsg->nPlayerIndex;
    sgtJoinRiftGlobals.nRiftTier = ptRiftJoinMsg->nRiftTier;

    sgtJoinRiftGlobals.eState = JoinState::WaitingOnPlayers;
    sgtJoinRiftGlobals.bManuallyClosed = FALSE;


    int32 nControllerIndex = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    if (PlayerIsLocal((int32)ptRiftJoinMsg->nPlayerIndex))
    {


        Player * ptInstigatingPlayer = PlayerGetByPlayerIndex(ptRiftJoinMsg->nPlayerIndex);
        if (ptInstigatingPlayer)
        {
            nControllerIndex = ptInstigatingPlayer->nControllerIndex;
        }
    }

    int32 nNumLocalIneligiblePlayers = 0;


    for (int32 i = 0; i < 4; i++)
    {
        int32 nKeyTier = (ptRiftJoinMsg->nPlayerKeyTiers >> (i * 8)) & 0xff;
        if (nKeyTier == 0xff)
        {
            nKeyTier = -1;
            sgtJoinRiftGlobals.arPlayerStatus[i] = PlayerStatus::Ineligible;





            if (PlayerIsLocal(i))
            {
                nNumLocalIneligiblePlayers++;
            }
        }
        else
        {

            if (ptRiftJoinMsg->nPlayerIndex == i)
            {
                sgtJoinRiftGlobals.arPlayerStatus[i] = PlayerStatus::Accepted;
                if (PlayerIsLocal(i))
                {
                    sgtJoinRiftGlobals.bPlayerEmpowering[i] = UIConsoleRiftEntry::PlayerInstigatorEmpoweredRift(i);
                }
                else
                {
                    sgtJoinRiftGlobals.bPlayerEmpowering[i] = FALSE;
                }
            }
            else
            {
                sgtJoinRiftGlobals.arPlayerStatus[i] = PlayerStatus::NoAnswer;
                sgtJoinRiftGlobals.bPlayerEmpowering[i] = FALSE;
                gtTutorialMgr.ShowTutorial(aridPlayerPanels[i], GlobalSNOGet(GLOBALSNO_TUTORIAL_TIERED_LOOT_RUN_EMPOWER), NULL, TRUE);
            }

            UIID idEmpoweredFrame = UIIDCreateFromParent(aridPlayerPanels[i], ".LayoutRoot.Empowered");
            UI_GetWindowFromID(idEmpoweredFrame).SetTransparency(sgtJoinRiftGlobals.bPlayerEmpowering[i] ? 255 : 0);
            UIID idEmpoweredLightning = UIIDCreateFromParent(aridPlayerPanels[i], ".LayoutRoot.Empowered.EmpoweredLightning");
            UI_GetWindowFromID(idEmpoweredLightning).SetTransparency(sgtJoinRiftGlobals.bPlayerEmpowering[i] ? 255 : 0);
        }


        sgtJoinRiftGlobals.nPlayerKeys[i] = nKeyTier;
    }


    sShowAutoStatus();


    UI_GetTextFromID(UITEXT_JOIN_RIFT_ELIGIBILITY_MSG).SetActive(nNumLocalIneligiblePlayers > 0);
    UI_GetTextFromID(UITEXT_JOIN_RIFT_USE_KEY_MSG).SetActive(nNumLocalIneligiblePlayers == 0);


    CRefString sTitle = LocalizedSprintf(StringListFetch("ConsoleRifts:JoinTitle").str(),
                                         CPlayerGetIndicatorString((int32)ptRiftJoinMsg->nPlayerIndex).str(),
                                         UnicodeText::FormatNumber(sgtJoinRiftGlobals.nRiftTier + 1, TRUE, FNS_NORMAL, 0).str(), NULL);
    UI_GetTextFromID(UITEXT_JOIN_RIFT_TITLE).SetText(sTitle, FALSE);


    uint32 dwTimerTicks = 0;
    if (ptRiftJoinMsg->uRiftStartServerTime > GameGetServerTime())
    {
        dwTimerTicks = ptRiftJoinMsg->uRiftStartServerTime - GameGetServerTime();
    }
    sSetTimer(dwTimerTicks);


    sSetItemButtons();



    ::Console::Screens::OpenToScreen(::Console::Screens::RIFT_JOIN_SCREEN_SET, 0, nControllerIndex, TRUE);






    if (::Console::Screens::IsScreenOpen(::Console::Screens::RIFT_JOIN_SCREEN_SET, 0))
    {




        ::Console::Screens::ShowModalDialog(nControllerIndex, UIDLG_JOIN_RIFT_MAIN, FALSE, GetUIIDNone());

        ::Console::Screens::GetActionManager().ClearAllActions();
        ::Console::Screens::UpdateButtonLegendText();

        sUpdateControllerActions();
    }
}



BOOL UIJoinRift::IsActive()
{

    return UI_GetWindowFromID(UIDLG_JOIN_RIFT_MAIN).IsActive();
}




void UIJoinRift::Close()
{

    if (IsActive())
    {



        for (int32 i = 0; i < 4; i++)
        {
            sgtJoinRiftGlobals.arPlayerStatus[i] = PlayerStatus::NoAnswer;
            sgtJoinRiftGlobals.nPlayerKeys[i] = -1;
        }

        sResetUI();

        ::Console::Screens::CloseModalDialog(UIDLG_JOIN_RIFT_MAIN);
    }
}



static PlayerStatus sGetStatusForPlayer(const EncounterInviteStateMessage * ptStateMsg, int32 nPlayerIndex)
{


    if (ptStateMsg->m_AcceptedPlayers & (1 << nPlayerIndex))
    {
        return PlayerStatus::Accepted;
    }

    if (ptStateMsg->m_DeclinedPlayers & (1 << nPlayerIndex))
    {
        return PlayerStatus::Declined;
    }

    if (ptStateMsg->m_PendingPlayers & (1 << nPlayerIndex))
    {
        return PlayerStatus::NoAnswer;
    }

    return PlayerStatus::Ineligible;
}




static LPCTSTR sGetStatusTextureName(PlayerStatus eStatus)
{

    if (eStatus == PlayerStatus::Accepted)
    {
        return "UI:Console_TieredRifts_CheckMark_p1";
    }
    else if (eStatus == PlayerStatus::Declined || eStatus == PlayerStatus::Ineligible)
    {
        return "UI:Console_TieredRifts_XMark_p1";
    }

    return "";
}






static void sStartRift()
{

    sgtJoinRiftGlobals.eState = JoinState::StartingRift;


    sSetTimer(NephalemRift::GetStartRiftDelaySeconds() * 60);

    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_JOIN_RIFT_ENTER_RIFT_COUNTDOWN);
    tStoryboard.Reset();
    tStoryboard.Play(0, 0.0f);
}




static void sPlaySelectionAnimation(int32 nPlayerIndex, PlayerStatus ePlayerStatus)


{

    UIID idStatusIcon = sGetStatusSelectedIconUIID(nPlayerIndex);
    UI_GetWindowFromID(idStatusIcon).SetTextureFrame(sGetStatusTextureName(ePlayerStatus));

    UIID idStoryboard = sGetStatusSelectStoryboardUIID(nPlayerIndex);
    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
    tStoryboard.Reset();
    tStoryboard.Play(0, 0.0f);
}




static void sPlayOverrideAnimation(int32 nPlayerIndex, PlayerStatus ePlayerStatus)


{

    UIID idStatusIcon = sGetStatusOverrideIconUIID(nPlayerIndex);
    UI_GetWindowFromID(idStatusIcon).SetTextureFrame(sGetStatusTextureName(ePlayerStatus));

    UIID idStoryboard = sGetStatusOverrideStoryboardUIID(nPlayerIndex);
    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
    tStoryboard.Reset();
    tStoryboard.Play(0, 0.0f);
}




void UIJoinRift::InviteStatusUpdate(const EncounterInviteStateMessage * ptStateMsg)
{


    if (!::Console::Screens::IsScreenOpen(::Console::Screens::RIFT_JOIN_SCREEN_SET, 0))
    {
        if (!sgtJoinRiftGlobals.bManuallyClosed)
        {
            SimpleMessage tMessage;
            MessageSendToServer(CMSG_RIFT_JOIN_INFO_REQUEST, &tMessage, sizeof(tMessage), LocalPlayerGetPrimary());
        }

        return;
    }

    int32 nNumAccepted = 0;
    int32 nNumDeclined = 0;
    int32 nNumUnanswered = 0;

    BOOL bPlayLocalDeclineWarningAnim = FALSE;


    for (int32 i = 0; i < 4; i++)
    {

        PlayerStatus ePrevStatus = sgtJoinRiftGlobals.arPlayerStatus[i];
        PlayerStatus eNewStatus = sGetStatusForPlayer(ptStateMsg, i);


        if (eNewStatus == PlayerStatus::Accepted)
        {
            nNumAccepted++;
        }
        else if (eNewStatus == PlayerStatus::Declined || eNewStatus == PlayerStatus::Ineligible)
        {
            nNumDeclined++;
        }
        else if (eNewStatus == PlayerStatus::NoAnswer)
        {
            nNumUnanswered++;
        }


        if (ePrevStatus != eNewStatus)
        {
            sgtJoinRiftGlobals.arPlayerStatus[i] = eNewStatus;

            if (eNewStatus == PlayerStatus::Declined)
            {
                if (PlayerIsLocal(i))
                {





                    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_CANCEL_RIFT_ENTRY), cgtPSODefaults);
                    bPlayLocalDeclineWarningAnim = TRUE;
                }
                else
                {
                    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_REMOTE_CANCEL_RIFT_ENTRY), cgtPSODefaults);
                }
            }
            else if (eNewStatus == PlayerStatus::Accepted)
            {

                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_CONFIRM_RIFT_ENTRY), cgtPSODefaults);
            }

            if (ePrevStatus == PlayerStatus::NoAnswer || ePrevStatus == PlayerStatus::Ineligible)
            {
                sPlaySelectionAnimation(i, eNewStatus);
            }
            else
            {
                sPlayOverrideAnimation(i, eNewStatus);


                UIID idStoryboardSelect = sGetStatusSelectStoryboardUIID(i);
                UIStoryboard & tStoryboardSelect = UI_GetStoryboardFromID(idStoryboardSelect);
                tStoryboardSelect.Stop();
                tStoryboardSelect.Reset();
            }
        }
    }

    if (bPlayLocalDeclineWarningAnim)
    {

        if (UI_GetWindowFromID(UITEXT_JOIN_RIFT_LOCAL_PLAYER_DECLINED_WARNING).GetTransparency() == 0)
        {
            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_JOIN_RIFT_LOCAL_PLAYER_DECLINED_WARNING);
            if (!tStoryboard.IsPlaying())
            {
                tStoryboard.Reset();
                tStoryboard.Play(0, 0.0f);
            }
        }

        sUpdateControllerActions();
    }

    if (sgtJoinRiftGlobals.eState == JoinState::WaitingOnPlayers)
    {

        int32 nNumPlayers = PlayerGetNumPlayers();
        if (nNumAccepted == nNumPlayers)
        {
            sStartRift();
        }
        else if (nNumAccepted == 0 && nNumUnanswered == 0)
        {
            sgtJoinRiftGlobals.eState = JoinState::AllDeclined;
        }
        else if (nNumUnanswered == 0 && nNumDeclined > 0)
        {
            uint32 dwTimerTicks = 900;
            if (!PlayerIsLocal(sgtJoinRiftGlobals.nInstigatorPlayerIndex) && !sHasALocalPlayerDeclined())
            {

                UIStoryboard & tStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_JOIN_RIFT_ENTER_RIFT_COUNTDOWN);
                tStoryboard.Reset();
                tStoryboard.Play(0, 0.0f);
            }

            sSetTimer(dwTimerTicks);
        }
    }
}



void UIJoinRift::Started()
{

    if (IsActive())
    {

        BOOL bGoingIntoRift = TRUE;



        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (PlayerIsLocal(ptPlayer->nPlayerIndex))
            {
                if (sgtJoinRiftGlobals.arPlayerStatus[ptPlayer->nPlayerIndex] != PlayerStatus::Accepted)
                {
                    bGoingIntoRift = FALSE;
                    break;
                }
            }
        }

        if (bGoingIntoRift)
        {
            sCloseSelf();
        }
        else
        {
            sgtJoinRiftGlobals.eState = JoinState::DidNotJoinRift;
            sUpdateControllerActions();
        }
    }
}



void UIJoinRift::Canceled()
{

    if (IsActive())
    {



        if (!PlayerIsLocal(sgtJoinRiftGlobals.nInstigatorPlayerIndex))
        {
            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_JOIN_RIFT_HOST_CANCELED);
            tStoryboard.Reset();
            tStoryboard.Play(0, 0.0f);
        }

        for (int32 i = 0; i < 4; i++)
        {
            if (sgtJoinRiftGlobals.arPlayerStatus[i] != PlayerStatus::Declined)
            {
                sgtJoinRiftGlobals.arPlayerStatus[i] = PlayerStatus::Declined;
                sPlayOverrideAnimation(i, PlayerStatus::Declined);
            }
        }

        sgtJoinRiftGlobals.eState = JoinState::RiftCanceled;
        sUpdateControllerActions();
    }
}



void UIJoinRift::RemotePlayersDeclined(int32 nDeclinedPlayerMap)
{



    if (PlayerIsLocal(sgtJoinRiftGlobals.nInstigatorPlayerIndex))
    {
        UIStoryboard & tStoryboardPlayerDeclined = UI_GetStoryboardFromID(UISTORYBOARD_JOIN_RIFT_REMOTE_PLAYER_DECLINED);
        tStoryboardPlayerDeclined.Reset();
        tStoryboardPlayerDeclined.Play(0, 0.0f);

        UIStoryboard & tStoryboardInitiatorPrompt = UI_GetStoryboardFromID(UISTORYBOARD_JOIN_RIFT_INITIATOR_PROMPT);
        tStoryboardInitiatorPrompt.Reset();
        tStoryboardInitiatorPrompt.Play(0, 0.0f);

        sgtJoinRiftGlobals.eState = JoinState::RemoteDeclined;
        sUpdateControllerActions();
    }
}




static void sUpdateEntryTimer()
{

    CRefString sText;
    CRefString sTimeRemaining;
    uint32 dwNow = CGameGetClientTicks();
    if (dwNow > sgtJoinRiftGlobals.dwStopTime)
    {
        return;
    }

    uint32 dwSecondsRemaining = (sgtJoinRiftGlobals.dwStopTime - dwNow) / 60;

    switch (sgtJoinRiftGlobals.eState)
    {
    case JoinState::StartingRift:
        sTimeRemaining = UnicodeText::FormatNumber((int32)(dwSecondsRemaining + 1), TRUE, FNS_NORMAL, 0);
        sText = LocalizedSprintf(StringListFetch("ConsoleRifts:p_ConsoleTieredRiftJoinEnteringRift").str(),
                                 sTimeRemaining.str(), NULL);
        break;

    case JoinState::WaitingOnPlayers:

        sTimeRemaining = UnicodeText::FormatNumber((int32)dwSecondsRemaining, TRUE, FNS_NORMAL, 0);
        sText = StringListFetch("Console:WaitingHost").str();
        sText.Format("%s %s", sText.str(), sTimeRemaining.str());
        break;

    case JoinState::RemoteDeclined:
        sTimeRemaining = UnicodeText::FormatNumber((int32)dwSecondsRemaining, TRUE, FNS_NORMAL, 0);
        sText = sTimeRemaining.str();
        break;
    }
    UI_GetTextFromID(UITEXT_JOIN_RIFT_ENTRY_COUNTDOWN).SetText(sText, FALSE);
}
void sCheckInviteCompletionStatus()
{

    if (sgtJoinRiftGlobals.eState == JoinState::WaitingOnPlayers)
    {
        int32 nNumAccepted = 0;
        int32 nNumDeclined = 0;
        int32 nNumUnanswered = 0;
        int32 nNumPlayers;


        for (int32 i = 0; i < 4; i++)
        {
            PlayerStatus eStatus = sgtJoinRiftGlobals.arPlayerStatus[i];



            if (eStatus == PlayerStatus::Accepted)
            {
                nNumAccepted++;
            }
            else if (eStatus == PlayerStatus::Declined || eStatus == PlayerStatus::Ineligible)
            {
                nNumDeclined++;
            }
            else if (eStatus == PlayerStatus::NoAnswer)
            {
                nNumUnanswered++;
            }
        }



        nNumPlayers = PlayerGetNumPlayers();
        if (nNumAccepted == nNumPlayers)
        {
            sStartRift();
        }
        else if (nNumAccepted == 0 && nNumUnanswered == 0)
        {
            sgtJoinRiftGlobals.eState = JoinState::AllDeclined;
        }
    }
}




void UIJoinRift::OnUpdate(const UIID & id)
{

    if (IsActive())
    {




        for (int32 i = 0; i < 4; i++)
        {
            sUpdatePlayerStatus(i);
        }

        sUpdateEntryTimer();

        if (sgtJoinRiftGlobals.eState == JoinState::WaitingOnPlayers)
        {
            sCheckInviteCompletionStatus();
        }
    }
}



void UIJoinRift::OnButton1(const UIID & id, const int32 nControllerIndex)



{


    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (!ptPlayer)
    {
        return;
    }

    if (sgtJoinRiftGlobals.eActionBtn1 == ButtonAction::Yes)
    {
        if (ptPlayer->nPlayerIndex == sgtJoinRiftGlobals.nInstigatorPlayerIndex)
        {

            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_CONFIRM_RIFT_ENTRY), cgtPSODefaults);

            sStartRift();

            BoolDataMessage tMessage;
            tMessage.bVal = sgtJoinRiftGlobals.bPlayerEmpowering[ptPlayer->nPlayerIndex];
            MessageSendToServer(CMSG_RIFT_VOTE_CONTINUE, &tMessage, sizeof(tMessage), ptPlayer);
        }
    }
    else
    {

        if (sgtJoinRiftGlobals.arPlayerStatus[ptPlayer->nPlayerIndex] == PlayerStatus::NoAnswer)
        {



            BoolDataMessage tMessage;
            tMessage.bVal = sgtJoinRiftGlobals.bPlayerEmpowering[ptPlayer->nPlayerIndex];
            MessageSendToServer(CMSG_RIFT_JOIN_ACCEPTED, &tMessage, sizeof(tMessage), ptPlayer);
        }
    }
}




void UIJoinRift::OnButton2(const UIID & id, const int32 nControllerIndex)



{


    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (!ptPlayer)
    {
        return;
    }

    switch (sgtJoinRiftGlobals.eActionBtn2)
    {
    case ButtonAction::No:
        if (ptPlayer->nPlayerIndex == sgtJoinRiftGlobals.nInstigatorPlayerIndex)
        {

            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_CANCEL_RIFT_ENTRY), cgtPSODefaults);

            SimpleMessage tMessage;
            MessageSendToServer(CMSG_RIFT_VOTE_CANCEL, &tMessage, sizeof(tMessage), ptPlayer);

            sgtJoinRiftGlobals.bManuallyClosed = TRUE;
            sCloseSelf();
        }
        break;

    case ButtonAction::Close:
    {
        Player * ptPrimaryPlayer = PlayerGetByControllerIndex(::Console::GamerProfile::GetPrimaryProfileUserIndex());
        int32 nUIOwnerPlayerIndex = ptPrimaryPlayer ? ptPrimaryPlayer->nPlayerIndex : -1;
        if (PlayerIsLocal(sgtJoinRiftGlobals.nInstigatorPlayerIndex))
        {
            nUIOwnerPlayerIndex = sgtJoinRiftGlobals.nInstigatorPlayerIndex;
        }

        if (nUIOwnerPlayerIndex == ptPlayer->nPlayerIndex)
        {
            sgtJoinRiftGlobals.bManuallyClosed = TRUE;
            sCloseSelf();
        }
        break;
    }

    default:
        if (sgtJoinRiftGlobals.arPlayerStatus[ptPlayer->nPlayerIndex] == PlayerStatus::NoAnswer)
        {




            SimpleMessage tMessage;
            MessageSendToServer(CMSG_RIFT_JOIN_DECLINED, &tMessage, sizeof(tMessage), ptPlayer);
        }
        break;
    }
}
void UIJoinRift::OnButton4(const UIID & id, const int32 nControllerIndex)



{

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    int32 nPlayerIndex = ptPlayer->nPlayerIndex;

    if (sgtJoinRiftGlobals.nRiftTier >= 0 && sgtJoinRiftGlobals.arPlayerStatus[nPlayerIndex] == PlayerStatus::NoAnswer)
    {
        UIID idStoryboard = GetUIIDNone();
        if (sgtJoinRiftGlobals.bPlayerEmpowering[nPlayerIndex])
        {
            idStoryboard = UIIDCreateFromParent(aridPlayerPanels[nPlayerIndex], ".PlayRemoveEmpoweredRift");
            sgtJoinRiftGlobals.bPlayerEmpowering[nPlayerIndex] = FALSE;
        }
        else
        {
            uint64 nEmpowerCost;
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

            nEmpowerCost = NephalemRift::GetEmpoweredRiftCost(sgtJoinRiftGlobals.nRiftTier - 1);
            if ((uint64)ptACDPlayer->GetCurrencyAmount(0) < nEmpowerCost)
            {
                idStoryboard = UIIDCreateFromParent(aridPlayerPanels[nPlayerIndex], ".PlayNotGoldNotify");
                sgtJoinRiftGlobals.bPlayerEmpowering[nPlayerIndex] = FALSE;
            }
            else
            {
                idStoryboard = UIIDCreateFromParent(aridPlayerPanels[nPlayerIndex], ".PlayEmpoweredRift");
                sgtJoinRiftGlobals.bPlayerEmpowering[nPlayerIndex] = TRUE;
            }
        }

        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Reset();
        tStoryboard.Play(0, 0.0f);
    }
}



static void sSetItemButtons()
{

    for (int32 i = 0; i < 4; i++)
    {
        Player * ptPlayer = PlayerGetByPlayerIndex(i);
        if (!ptPlayer)
        {
            continue;
        }

        if (ptPlayer->eTeam != -1)
        {




            UI_GetWindowFromID(UIIDCreateFromParent(aridPlayerPanels[i], ".LayoutRoot.KeystoneNotEligible")).SetActive(
                sgtJoinRiftGlobals.arPlayerStatus[i] == PlayerStatus::Ineligible);

            UI_GetWindowFromID(UIIDCreateFromParent(aridPlayerPanels[i], ".LayoutRoot.RiftIcon")).SetActive(
                sgtJoinRiftGlobals.arPlayerStatus[i] != PlayerStatus::Ineligible);

            if (sgtJoinRiftGlobals.arPlayerStatus[i] != PlayerStatus::Ineligible)
            {
                GBID gbidLootRunKey = StringToGBID(CRefString("GreaterLootRunKey"));
                GBHandle hGBItem;
                hGBItem.Set(2, gbidLootRunKey);

                if (GBIDExists(hGBItem))
                {

                    UIID idItemButton = UIIDCreateFromParent(aridPlayerPanels[i], ".LayoutRoot.RiftIcon");
                    UIItemButton & tItemButton = UI_GetItemButtonFromID(idItemButton);

                    ImageTextureFrame tImageTexFrame;
                    ACDInventoryGetItemDrawInfoWithGBID(gbidLootRunKey, tImageTexFrame, 0);

                    tItemButton.SetAllTextureFrames(tImageTexFrame);

                    tItemButton.SetQualityLevel(9);
                    tItemButton.SetGBID(gbidLootRunKey);
                    tItemButton.SetOverlay(0);
                }
            }
        }
    }
}



static void sUpdatePlayerStatus(int32 nPlayerIndex)
{

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    BOOL bIsPlayerValid = FALSE;
    if (ptPlayer)
    {



        bIsPlayerValid = ptPlayer->eTeam != -1;
        UI_GetWindowFromID(aridPlayerPanels[nPlayerIndex]).SetActive(bIsPlayerValid);
    }
    if (!bIsPlayerValid)
    {
        sgtJoinRiftGlobals.arPlayerStatus[nPlayerIndex] = PlayerStatus::Declined;
        return;
    }

    UIID idPlayerLabel = UIIDCreateFromParent(aridPlayerPanels[nPlayerIndex], ".LayoutRoot.Player");
    UI_GetTextFromID(idPlayerLabel).SetText(CPlayerGetIndicatorString(*ptPlayer), FALSE);


    if (!PlayerIsLocal(nPlayerIndex))
    {
        UIID idVotingButtons = UIIDCreateFromParent(aridPlayerPanels[nPlayerIndex], ".LayoutRoot.YesNoControls");
        UI_GetWindowFromID(idVotingButtons).SetTransparency(0);
    }



    if (PlayerIsLocal(nPlayerIndex))
    {
        UIID idVotingButtons = UIIDCreateFromParent(aridPlayerPanels[nPlayerIndex], ".LayoutRoot.YesNoControls");
        UIID idYes = UIIDCreateFromParent(idVotingButtons, ".Yes");
        UIText & yesButton = UI_GetTextFromID(idYes);
        yesButton.SetControllerIndexOverride(ptPlayer->nControllerIndex);

        UIID idNo = UIIDCreateFromParent(idVotingButtons, ".No");
        UIText & noButton = UI_GetTextFromID(idNo);
        noButton.SetControllerIndexOverride(ptPlayer->nControllerIndex);

        UIID idEmpower = UIIDCreateFromParent(idVotingButtons, ".Empower");
        UIText & empowerButton = UI_GetTextFromID(idEmpower);
        empowerButton.SetControllerIndexOverride(ptPlayer->nControllerIndex);
    }



    UIID idCountdown = UIIDCreateFromParent(aridPlayerPanels[nPlayerIndex], ".LayoutRoot.Countdown");
    UIText & tCountdownText = UI_GetTextFromID(idCountdown);

    uint32 dwNow = CGameGetClientTicks();
    if (dwNow > sgtJoinRiftGlobals.dwStopTime)
    {
        tCountdownText.SetText("", FALSE);
    }
    else
    {
        uint32 dwSecondsRemaining = (sgtJoinRiftGlobals.dwStopTime - dwNow) / 60;
        tCountdownText.SetText(UnicodeText::FormatNumber((int32)dwSecondsRemaining, TRUE, FNS_NORMAL, 0), FALSE);
    }
}




static void sShowAutoStatus()
{


    for (int32 i = 0; i < 4; i++)
    {
        if (sgtJoinRiftGlobals.arPlayerStatus[i] != PlayerStatus::NoAnswer)
        {
            sPlaySelectionAnimation(i, sgtJoinRiftGlobals.arPlayerStatus[i]);
        }
    }
}



static BOOL sHasALocalPlayerDeclined()
{

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayerIsLocal(ptPlayer->nPlayerIndex))
        {
            if (sgtJoinRiftGlobals.arPlayerStatus[ptPlayer->nPlayerIndex] == PlayerStatus::Declined)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}



static void sUpdateControllerActions()
{

    sgtJoinRiftGlobals.eActionBtn1 = ButtonAction::None;
    sgtJoinRiftGlobals.eActionBtn2 = ButtonAction::None;

    if (sgtJoinRiftGlobals.eState == JoinState::DidNotJoinRift || sgtJoinRiftGlobals.eState == JoinState::RiftCanceled)
    {
        sgtJoinRiftGlobals.eActionBtn2 = ButtonAction::Close;
    }
    else if (PlayerIsLocal(sgtJoinRiftGlobals.nInstigatorPlayerIndex) &&
             
             sgtJoinRiftGlobals.eState == JoinState::RemoteDeclined)
    {
        sgtJoinRiftGlobals.eActionBtn1 = ButtonAction::Yes;
        sgtJoinRiftGlobals.eActionBtn2 = ButtonAction::No;
    }



    if (sHasALocalPlayerDeclined())
    {
        sgtJoinRiftGlobals.eActionBtn2 = ButtonAction::Close;
    }

    CRefString sButtonLabels;
    if (sgtJoinRiftGlobals.eActionBtn1 == ButtonAction::Yes)
    {
        CRefString sBtn1;
        sBtn1.Format("%s %s",
                     ::Console::Screens::GetActionManager().InputToString(::Console::CONTROLLER_INPUT_BUTTON_1).str(),
                     StringListFetch("ConsoleRifts:Yes").str());

        sButtonLabels += sBtn1;
    }

    if (sgtJoinRiftGlobals.eActionBtn2 != ButtonAction::None)
    {
        CRefString sLabel;
        switch (sgtJoinRiftGlobals.eActionBtn2)
        {
        case ButtonAction::No:
            sLabel = StringListFetch("ConsoleRifts:No");
            break;

        case ButtonAction::Close:
            sLabel = StringListFetch("ConsoleUI:Close");
            break;
        }
        CRefString sBtn2;
        sBtn2.Format("%s %s",
                     ::Console::Screens::GetActionManager().InputToString(::Console::CONTROLLER_INPUT_BUTTON_2).str(),
                     sLabel.str());

        if (sButtonLabels.Length() > 0)
        {
            sButtonLabels += "   ";
        }
        sButtonLabels += sBtn2;
    }

    UI_GetTextFromID(UIWND_JOIN_RIFT_BUTTON_LEGEND).SetText(sButtonLabels, FALSE);
}




static void sResetStoryboard(const UIID & id)
{

    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(id);
    tStoryboard.Stop();
    tStoryboard.Reset();
}




static void sCloseSelf()
{

    ::Console::Screens::CloseScreenSet(TRUE, TRUE);
}




static void sResetUI()
{

    for (int32 i = 0; i < 4; i++)
    {
        UIID idSelectStoryboard = UIIDCreateFromParent(aridPlayerPanels[i], ".PlayStatusSelect");
        sResetStoryboard(idSelectStoryboard);

        UIID idOverrideStoryboard = UIIDCreateFromParent(aridPlayerPanels[i], ".PlayStatusSelectDeclined");
        sResetStoryboard(idOverrideStoryboard);

        UIID idStatusSelected = UIIDCreateFromParent(aridPlayerPanels[i], ".LayoutRoot.Status.StatusSelected");
        UI_GetWindowFromID(idStatusSelected).SetTransparency(0);

        UIID idStatusOverride = UIIDCreateFromParent(aridPlayerPanels[i], ".LayoutRoot.Status.StatusLocalOverride");
        UI_GetWindowFromID(idStatusOverride).SetTransparency(0);
    }

    sResetStoryboard(UISTORYBOARD_JOIN_RIFT_LOCAL_PLAYER_DECLINED_WARNING);
    sResetStoryboard(UISTORYBOARD_JOIN_RIFT_ENTER_RIFT_COUNTDOWN);
    sResetStoryboard(UISTORYBOARD_JOIN_RIFT_REMOTE_PLAYER_DECLINED);
    sResetStoryboard(UISTORYBOARD_JOIN_RIFT_HOST_CANCELED);
    sResetStoryboard(UISTORYBOARD_JOIN_RIFT_INITIATOR_PROMPT);

    UI_GetWindowFromID(UIWND_JOIN_RIFT_BUTTON_LEGEND).SetTransparency(255);
}




static UIID sGetStatusSelectedIconUIID(int32 nPlayerIndex)
{



    UIID idStoryboard = UIIDCreateFromParent(aridPlayerPanels[nPlayerIndex], ".LayoutRoot.Status.StatusSelected");
    return idStoryboard;
}




static UIID sGetStatusSelectStoryboardUIID(int32 nPlayerIndex)
{



    UIID idStoryboard = UIIDCreateFromParent(aridPlayerPanels[nPlayerIndex], ".PlayStatusSelect");
    return idStoryboard;
}




static UIID sGetStatusOverrideIconUIID(int32 nPlayerIndex)
{



    UIID idStoryboard = UIIDCreateFromParent(aridPlayerPanels[nPlayerIndex], ".LayoutRoot.Status.StatusLocalOverride");
    return idStoryboard;
}




static UIID sGetStatusOverrideStoryboardUIID(int32 nPlayerIndex)
{



    UIID idStoryboard = UIIDCreateFromParent(aridPlayerPanels[nPlayerIndex], ".PlayStatusSelectDeclined");
    return idStoryboard;
}
