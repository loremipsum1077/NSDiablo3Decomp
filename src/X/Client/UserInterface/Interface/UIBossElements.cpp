#include "UIBossElements.h"

#include "../../CGame.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../Console/ConsoleModalNotificationManager.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttributeStructs.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Definitions/Definitions.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/RefString.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
#include "../../../Common/Player.h"




// X/Common/Player.h:280
typedef int32 GameError;










#include "../../../Common/Networking/GameMessage.h"
















// X/Common/Definitions/StringListDefinition.h:19
typedef uint32 StringLabelHandle;
// X/Common/Definitions/BossEncounterDefinition.h
struct BossEncounterDefinition {  // size 0x68
    RequiredDefinitionHeader tHeader;  // 0x00
    StringLabelHandle hEncounterNameLabel;  // 0x0c
    StringLabelHandle hInformationTextLabel;  // 0x10
    StringLabelHandle hInstructionJoinLabel;  // 0x14
    StringLabelHandle hTauntTextLabel;  // 0x18
    uint8 _unadopted0[0x68 - 0x1c];  // 0x1c (invented name: hMural .. snoActor)
};
class BossEncounterGroupDef : public GroupDefinition {};
extern BossEncounterGroupDef * gptGroupDefBossEncounter;
typedef ScopedDefinitionLockR<BossEncounterDefinition> ScopedBossEncounterDefinitionLockR;
// Line 101. Its callers call LockRFast and have nothing from an unlock.
template <> inline const BossEncounterDefinition * DefinitionLockR<BossEncounterDefinition>(SNO sno) {
    return (const BossEncounterDefinition *)gptGroupDefBossEncounter->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<BossEncounterDefinition>(const BossEncounterDefinition *) {}
// X/Common/Definitions/StringList.cpp
CRefString StringListFetchByLabel(const SNO snoStringList, const StringLabelHandle hLabel);
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
CRefString LocalizedSprintf(LPCSTR szFormat, ...);
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UITXT_CONSOLE_BUTTON_LEGEND_TEXT;
extern UIID UIDLG_BOSSJOINPARTY_MAIN;
extern UIID UITXT_BOSSJOINPARTY_LOCATION;
extern UIID UITXT_BOSSJOINPARTY_INSTRUCTION;
extern UIID UITXT_BOSSJOINPARTY_TIMER;
extern UIID UITXT_BOSSJOINPARTY_DESCRIPTION;
extern UIID UIWND_BOSSJOINPARTY_BUTTON_WRAPPER;
extern UIID UIDLG_BOSS_START_MAIN;
extern UIID UITXT_BOSS_LOCATION;
extern UIID UITXT_BOSS_TAUNT;
extern UIID UITXT_BOSS_INFORMATION;
extern UIID UITXT_BOSS_TIMER;
extern UIID UIWND_BOSS_BUTTON_WRAPPER;
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
// X/Client/UserInterface/Controls/UITextControl.cpp
CRefString UIText_GetTimeString(const uint32 uSeconds);
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);
// X/Client/CPlayer.cpp
CRefString CPlayerGetServiceName(const Player & tPlayer, BOOL fDisplayPound);
Player * LocalPlayerGet();
ACDID LocalPlayerACDGet();
// X/Common/Player.cpp
Player * PlayerGetByPlayerIndex(int32 nPlayerIndex);
int32 PlayerGetNumPlayers();
// X/Client/UserInterface/Interface/UIGame.cpp
namespace UIGame {
void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer);
}  // namespace UIGame
// X/Client/UserInterface/Interface/UIConfirmation.cpp
namespace UIConfirmation {
void Close();
}  // namespace UIConfirmation
// X/Client/UserInterface/Console/UIConsoleJoinRift.cpp
namespace UIJoinRift {
BOOL IsActive();
}  // namespace UIJoinRift
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console {
namespace Screens {
ConsoleModalNotificationManager & GetModalNotificationManager();
}  // namespace Screens
}  // namespace Console

namespace UIBossEncounter {

static const int32 DEFAULT_TIMER_DURATION_SECONDS = 60;  // line 45; value from Enter_Open's 60-second timer

namespace {

class UIBossJoinPartyWindow : public UIBossEncounter::UIBossWindowBase {  // size 0x10
public:
    UIBossJoinPartyWindow() {}
    void Initialize(SNO snoEncounter, Player * ptPlayer, Player * ptPlayerInstigator);
    void SetTimer(uint32 dwSecondsRemaining);
    void Tick();
    void Close();
};
static_assert(sizeof(UIBossJoinPartyWindow) == 0x10, "UIBossJoinPartyWindow");

UIBossJoinPartyWindow sJoinPartyWindow;

class UIBossEnterEncounterWindow : public UIBossEncounter::UIBossWindowBase {  // size 0x14
public:
    UIBossEnterEncounterWindow() : m_bPendingInvite(FALSE) {}
    void Initialize(SNO snoEncounter, Player * ptPlayer);
    void SetTimer(uint32 dwSecondsRemaining);
    void Tick();
    void Close();
    // Compared with TRUE: every reader is ldrb + cmp #1, not a zero test.
    BOOL HasPendingInvite() const { return m_bPendingInvite == TRUE; }
private:
    BOOL m_bPendingInvite;  // 0x10
};
static_assert(sizeof(UIBossEnterEncounterWindow) == 0x14, "UIBossEnterEncounterWindow");

UIBossEnterEncounterWindow sStartEncounterWindow;

}  // namespace

void UIBossWindowBase::OnOpen(Player * ptPlayer)
{
    m_nControllerIndex = ptPlayer->nControllerIndex;
    m_dwOpenTime = CGameGetRealTicks();
}

void UIBossWindowBase::SetTimer(uint32 dwSecondsRemaining)
{
    m_dwStartTime = CGameGetClientTicks();
    m_dwStopTime = m_dwStartTime + dwSecondsRemaining * 60;
}

BOOL UIBossWindowBase::PlayerHasControl(const int32 nControllerIndex) const
{
    return m_nControllerIndex == -1 || m_nControllerIndex == nControllerIndex;
}

BOOL UIBossWindowBase::DelayHasElapsed() const
{
    uint32 dwCurTime = CGameGetRealTicks();
    return dwCurTime - m_dwOpenTime >= 60;
}





void Enter_Close()
{
    sStartEncounterWindow.Close();

    UI_GetTextFromID(UITXT_CONSOLE_BUTTON_LEGEND_TEXT).SetActive(TRUE);
}


void Enter_Update(const UIID &)
{
    sStartEncounterWindow.Tick();
}

void Enter_Cancel(const UIID &)
{
    if (sStartEncounterWindow.HasPendingInvite())
    {
        SimpleMessage tMessage;
        MessageSendToServer(CMSG_BOSS_START_DECLINED, &tMessage, sizeof(tMessage), LocalPlayerGet());
    }

    sStartEncounterWindow.Close();

    UI_GetTextFromID(UITXT_CONSOLE_BUTTON_LEGEND_TEXT).SetActive(TRUE);
}

void Enter_Accept(const UIID &)
{
    if (sStartEncounterWindow.HasPendingInvite())
    {
        SimpleMessage tMessage;
        MessageSendToServer(CMSG_BOSS_START_ACCEPTED, &tMessage, sizeof(tMessage), LocalPlayerGet());
    }

    sStartEncounterWindow.Close();

    UI_GetTextFromID(UITXT_CONSOLE_BUTTON_LEGEND_TEXT).SetActive(TRUE);
}


void Enter_Open(Player * ptPlayer, const BossEncounterMessage * ptMsg)
{
    sStartEncounterWindow.Close();
    sJoinPartyWindow.Close();

    sStartEncounterWindow.Initialize(ptMsg->snoEncounter, ptPlayer);
    sStartEncounterWindow.SetTimer(DEFAULT_TIMER_DURATION_SECONDS);

    UI_GetTextFromID(UITXT_CONSOLE_BUTTON_LEGEND_TEXT).SetActive(FALSE);
}


void Enter_OnButton1(const UIID & id, const int32 nControllerIndex)
{
    if (sStartEncounterWindow.PlayerHasControl(nControllerIndex) &&
        sStartEncounterWindow.DelayHasElapsed())
    {
        Enter_Accept(id);
    }
}


void Enter_OnButton2(const UIID & id, const int32 nControllerIndex)
{
    if (sStartEncounterWindow.PlayerHasControl(nControllerIndex) &&
        sStartEncounterWindow.DelayHasElapsed())
    {
        Enter_Cancel(id);
    }
}


void JoinParty_Update(const UIID &)
{
    sJoinPartyWindow.Tick();
}

void JoinParty_Cancel(const UIID &)
{
    SimpleMessage tMessage;
    MessageSendToServer(CMSG_BOSS_JOIN_DECLINED, &tMessage, sizeof(tMessage), LocalPlayerGet());
    sJoinPartyWindow.Close();

    UI_GetTextFromID(UITXT_CONSOLE_BUTTON_LEGEND_TEXT).SetActive(TRUE);
}

void JoinParty_Accept(const UIID &)
{
    const ActorCommonData * ptACD = ACDTryToGet(LocalPlayerACDGet());

    if (ptACD && ptACD->IsAlive())
    {
        if (ptACD->AttributesGetInt(ATTRIBUTE_CANT_START_DISPLAYED_POWERS))
        {
            UIGame::DisplayError(GameErrorData(GAMEERROR_CANNOT_DO_THAT_RIGHT_NOW), FALSE, NULL);
        }
        else
        {
            SimpleMessage tMessage;
            MessageSendToServer(CMSG_BOSS_JOIN_ACCEPTED, &tMessage, sizeof(tMessage), LocalPlayerGet());
            sJoinPartyWindow.Close();
        }
    }
    else
    {
        UIGame::DisplayError(GameErrorData(GAMEERROR_CANNOT_DO_THIS_WHILE_DEAD), FALSE, NULL);
    }

    UI_GetTextFromID(UITXT_CONSOLE_BUTTON_LEGEND_TEXT).SetActive(TRUE);
}

static BOOL sSafeToJoin()
{
    return !UIJoinRift::IsActive();
}


void JoinParty_Open(Player * ptPlayer, const BossEncounterMessage * ptMsg)
{
    sStartEncounterWindow.Close();
    sJoinPartyWindow.Close();

    if (!sSafeToJoin())
    {
        JoinParty_Cancel(GetUIIDNone());
        return;
    }

    UI_GetTextFromID(UITXT_CONSOLE_BUTTON_LEGEND_TEXT).SetActive(FALSE);

    Player * ptPlayerInstigator = PlayerGetByPlayerIndex(ptMsg->nPlayerIndex);
    sJoinPartyWindow.Initialize(ptMsg->snoEncounter, ptPlayer, ptPlayerInstigator);
    sJoinPartyWindow.SetTimer(DEFAULT_TIMER_DURATION_SECONDS);
}


void JoinParty_OnButton1(const UIID & id, const int32 nControllerIndex)
{
    if (sJoinPartyWindow.PlayerHasControl(nControllerIndex))
    {
        JoinParty_Accept(id);
    }
}


void JoinParty_OnButton2(const UIID & id, const int32 nControllerIndex)
{
    if (sJoinPartyWindow.PlayerHasControl(nControllerIndex))
    {
        JoinParty_Cancel(id);
    }
}

BOOL IsActive()
{
    return UI_GetWindowFromID(UIDLG_BOSS_START_MAIN).IsActive() ||
           UI_GetWindowFromID(UIDLG_BOSSJOINPARTY_MAIN).IsActive();
}

namespace {

void UIBossJoinPartyWindow::Initialize(SNO snoEncounter, Player * ptPlayer, Player * ptPlayerInstigator)
{
    OnOpen(ptPlayer);

    SNO snoStringList = GlobalSNOGet(GLOBALSNO_BOSS_STRINGLIST);

    ScopedBossEncounterDefinitionLockR pdefEncounter(snoEncounter);

    CRefString sEncounterName = StringListFetchByLabel(snoStringList, pdefEncounter->hEncounterNameLabel);
    UI_GetTextFromID(UITXT_BOSSJOINPARTY_LOCATION).SetText(sEncounterName, FALSE);

    CRefString sHeroName("Invalid Hero");
    if (ptPlayerInstigator)
    {
        sHeroName = CPlayerGetServiceName(*ptPlayerInstigator, FALSE);
    }

    CRefString sInstructionJoinFormatStr = StringListFetchByLabel(snoStringList, pdefEncounter->hInstructionJoinLabel);
    CRefString sInstructionJoin = LocalizedSprintf(sInstructionJoinFormatStr.str(), sHeroName.str(), NULL);
    UI_GetTextFromID(UITXT_BOSSJOINPARTY_INSTRUCTION).SetText(sInstructionJoin, FALSE);

    CRefString sJoinDescription = StringListFetchByLabel(snoStringList, pdefEncounter->hInformationTextLabel);
    UI_GetTextFromID(UITXT_BOSSJOINPARTY_DESCRIPTION).SetText(sJoinDescription, FALSE);

    SetTimer(DEFAULT_TIMER_DURATION_SECONDS / 2);

    UI_GetWindowFromID(UIDLG_BOSSJOINPARTY_MAIN).SetActive(TRUE);
}

void UIBossJoinPartyWindow::Close()
{
    UI_GetWindowFromID(UIDLG_BOSSJOINPARTY_MAIN).SetActive(FALSE);
}

void UIBossJoinPartyWindow::Tick()
{
    UI_GetWindowFromID(UIWND_BOSSJOINPARTY_BUTTON_WRAPPER).SetActive(DelayHasElapsed());

    uint32 dwNow = CGameGetClientTicks();
    if (m_dwStopTime < dwNow)
    {
        JoinParty_Cancel(GetUIIDNone());
    }
    else
    {
        uint32 dwSecondsRemaining = (m_dwStopTime - dwNow) / 60;
        UIText & tTxtTimer = UI_GetTextFromID(UITXT_BOSSJOINPARTY_TIMER);
        tTxtTimer.SetText(UIText_GetTimeString(dwSecondsRemaining), FALSE);
    }
}

void UIBossJoinPartyWindow::SetTimer(uint32 dwSecondsRemaining)
{
    UIText & tTxtTimer = UI_GetTextFromID(UITXT_BOSSJOINPARTY_TIMER);
    tTxtTimer.SetText(UIText_GetTimeString(dwSecondsRemaining), FALSE);

    UIBossWindowBase::SetTimer(dwSecondsRemaining);
}

void UIBossEnterEncounterWindow::Initialize(SNO snoEncounter, Player * ptPlayer)
{
    OnOpen(ptPlayer);

    SNO snoStringList = GlobalSNOGet(GLOBALSNO_BOSS_STRINGLIST);

    ScopedBossEncounterDefinitionLockR pdefEncounter(snoEncounter);

    CRefString sLocationText = StringListFetchByLabel(snoStringList, pdefEncounter->hEncounterNameLabel);
    UI_GetTextFromID(UITXT_BOSS_LOCATION).SetText(sLocationText, FALSE);

    if (PlayerGetNumPlayers() > 1)
    {
        CRefString sInformationText = StringListFetchByLabel(snoStringList, pdefEncounter->hInformationTextLabel);
        UI_GetTextFromID(UITXT_BOSS_INFORMATION).SetText(sInformationText, FALSE);
    }
    else
    {
        UI_GetTextFromID(UITXT_BOSS_INFORMATION).SetText("", FALSE);
    }

    CRefString sTauntText = StringListFetchByLabel(snoStringList, pdefEncounter->hTauntTextLabel);
    UI_GetTextFromID(UITXT_BOSS_TAUNT).SetText(sTauntText, FALSE);

    int32 nTimerSeconds = DEFAULT_TIMER_DURATION_SECONDS;
    SetTimer(nTimerSeconds);

    UI_GetWindowFromID(UIDLG_BOSS_START_MAIN).SetActive(TRUE);
    m_bPendingInvite = TRUE;
}

void UIBossEnterEncounterWindow::Close()
{
    m_bPendingInvite = FALSE;
    UI_GetWindowFromID(UIDLG_BOSS_START_MAIN).SetActive(FALSE);
}

void UIBossEnterEncounterWindow::Tick()
{
    UI_GetWindowFromID(UIWND_BOSS_BUTTON_WRAPPER).SetActive(DelayHasElapsed());

    uint32 dwNow = CGameGetClientTicks();
    if (m_dwStopTime < dwNow)
    {
        Enter_Cancel(GetUIIDNone());
    }
    else
    {
        uint32 dwSecondsRemaining = (m_dwStopTime - dwNow) / 60;
        UIText & tTxtTimer = UI_GetTextFromID(UITXT_BOSS_TIMER);
        tTxtTimer.SetText(UIText_GetTimeString(dwSecondsRemaining), FALSE);
    }
}

void UIBossEnterEncounterWindow::SetTimer(uint32 dwSecondsRemaining)
{
    UIText & tTxtTimer = UI_GetTextFromID(UITXT_BOSS_TIMER);
    tTxtTimer.SetText(UIText_GetTimeString(dwSecondsRemaining), FALSE);

    UIBossWindowBase::SetTimer(dwSecondsRemaining);
}

}  // namespace

void InviteStatusUpdate(const EncounterInviteStateMessage * pState)
{
}

void Started()
{
    ::Console::Screens::GetModalNotificationManager().RemoveAllNotifications();
}

void Canceled(BOOL bShowError)
{
    UIConfirmation::Close();

    if (bShowError)
    {
        UIGame::DisplayError(GameErrorData(GAMEERROR_BOSS_ENCOUNTER_SOMEONE_DECLINED), FALSE, NULL);
    }
}

void VoteContinue(const UIID &)
{
    SimpleMessage tMessage;
    MessageSendToServer(CMSG_BOSS_VOTE_CONTINUE, &tMessage, sizeof(tMessage), LocalPlayerGet());
}

void VoteCancel(const UIID &)
{
    SimpleMessage tMessage;
    MessageSendToServer(CMSG_BOSS_VOTE_CANCEL, &tMessage, sizeof(tMessage), LocalPlayerGet());
}

void OneDeclined(Player * ptPlayer, int32 nDeclinedPlayerIndex)
{
    CRefString szDisplay;

    const Player * arrPlayersDeclined[4] = { NULL, NULL, NULL, NULL };
    int32 nIndex = 0;
    int32 nDeclined = 0;

    while (nDeclinedPlayerIndex != 0)
    {
        if ((nDeclinedPlayerIndex & (1 << nIndex)) == 0)
        {
            nIndex++;
            continue;
        }

        nDeclinedPlayerIndex &= ~(1 << nIndex);

        const Player * ptDeclined = PlayerGetByPlayerIndex(nIndex);
        if (ptDeclined)
        {
            arrPlayersDeclined[nDeclined++] = ptDeclined;
        }

        nIndex++;
    }

    switch (nDeclined)
    {
    case 0:
        szDisplay = StringListFetch(GlobalSNOGet(GLOBALSNO_BOSS_STRINGLIST), "Continue_Without_Player_0");
        break;

    case 1:
    {
        CRefString sFormat = StringListFetch(GlobalSNOGet(GLOBALSNO_BOSS_STRINGLIST), "Continue_Without_Player_1");
        CRefString szPlayer1 = CPlayerGetServiceName(*arrPlayersDeclined[0], FALSE);
        szDisplay = LocalizedSprintf(sFormat.str(), szPlayer1.str(), NULL);
        break;
    }

    case 2:
    {
        CRefString sFormat = StringListFetch(GlobalSNOGet(GLOBALSNO_BOSS_STRINGLIST), "Continue_Without_Player_2");
        CRefString szPlayer1 = CPlayerGetServiceName(*arrPlayersDeclined[0], FALSE);
        CRefString szPlayer2 = CPlayerGetServiceName(*arrPlayersDeclined[1], FALSE);
        szDisplay = LocalizedSprintf(sFormat.str(), szPlayer1.str(), szPlayer2.str(), NULL);
        break;
    }

    case 3:
    {
        CRefString sFormat = StringListFetch(GlobalSNOGet(GLOBALSNO_BOSS_STRINGLIST), "Continue_Without_Player_3");
        CRefString szPlayer1 = CPlayerGetServiceName(*arrPlayersDeclined[0], FALSE);
        CRefString szPlayer2 = CPlayerGetServiceName(*arrPlayersDeclined[1], FALSE);
        CRefString szPlayer3 = CPlayerGetServiceName(*arrPlayersDeclined[2], FALSE);
        szDisplay = LocalizedSprintf(sFormat.str(), szPlayer1.str(), szPlayer2.str(), szPlayer3.str(), NULL);
        break;
    }
    }

    NotificationCreationParams tParams;
    tParams.uszMessage = szDisplay;
    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &VoteContinue>();
    tParams.tCallbacks.pfnOnClickCancelButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &VoteCancel>();
    tParams.nControllerIndex = ptPlayer->nControllerIndex;

    ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
}

}  // namespace UIBossEncounter
