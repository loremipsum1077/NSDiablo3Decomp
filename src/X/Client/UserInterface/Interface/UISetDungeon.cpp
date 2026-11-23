// Original file: X/Client/UserInterface/Interface/UISetDungeon.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UISetDungeon.h"
#include "UIBossElements.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "../Console/ConsoleModalNotificationManager.h"
#include "../Console/ConsoleScreenManager.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Definitions/QuestDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"

#include "../../Battlenet/UI/Util/UIUtil.h"
extern UIID UIDLG_SETDUNGEONJOINPARTY_MAIN, UITXT_SETDUNGEONJOINPARTY_LOCATION, UITXT_SETDUNGEONJOINPARTY_TIMER, UITXT_SETDUNGEONJOINPARTY_DESCRIPTION; extern UIID UIWND_SETDUNGEONCOMPLETION_MAIN, UIWND_SETDUNGEONCOMPLETION_CONTENT, UIWND_SETDUNGEONCOMPLETION_DESCRIPTION, UIWND_SETDUNGEONCOMPLETION_OBJECTIVE_1, UIWND_SETDUNGEONCOMPLETION_OBJECTIVE_2, UIWND_SETDUNGEONCOMPLETION_GENERAL_OBJECTIVE, UIWND_SETDUNGEONCOMPLETION_MASTERY_KILL_OBJECTIVE, UIWND_SETDUNGEONCOMPLETION_PLAYER_TIME_OBJECTIVE, UIWND_SETDUNGEONCOMPLETION_MASTERY_NOTIFICATION, UIWND_SETDUNGEONCOMPLETION_HELPERS;  void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer); namespace UIGame { void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer); }  CRefString DisplayElapsedTime(const Blizzard::Time::Timestamp & tStart, const Blizzard::Time::Timestamp & tEnd, const ElapsedTimeDisplayOptions & tElapsedTimeDisplayOptions);
namespace UISetDungeon {

class UISetEncounterJoinPartyWindow : public UISetDungeon::UISetEncounterWindowBase {  // size 0x10
public:















    UISetEncounterJoinPartyWindow() {}
    void Initialize(const SetDungeonDialogMessage * ptMessage, Player *, Player * ptPlayerInstigator);
    void SetTimer(uint32 dwSecondsRemaining);
    void Tick();
    void Close();
};
static_assert(sizeof(UISetEncounterJoinPartyWindow) == 0x10, "UISetEncounterJoinPartyWindow");









UISetEncounterJoinPartyWindow sJoinPartyWindow;


static int32 s_nOwningPlayerIndex = -1;
static uint8 s_nAcceptedFlags = 0;
static ModalNotificationId s_idModal = INVALID_MODAL_NOTIFICATION_ID;















void sAcceptedSetDungeonModal(int32 nControllerIndex, const UIID & id)
{
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);

    SimpleMessage tMessage;
    MessageSendToServer(CMSG_SET_DUNGEON_ACCEPT, &tMessage, sizeof(tMessage), ptPlayer);

    s_nOwningPlayerIndex = ptPlayer->nPlayerIndex;
}





void Show(StringLabelHandle hLabelDescription, StringLabelHandle hLabelTitle, Player * ptPlayer)



{
    if (!ptPlayer)
    {
        return;
    }

    s_nOwningPlayerIndex = -1;
    s_nAcceptedFlags = 0;

    SNO snoStringlistDescriptions = GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_DESCRIPTIONS);
    SNO snoStringlistTitles = GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_TITLES);

    CRefString sTextDescription;
    CRefString sTextTitle = StringListFetchByLabel(snoStringlistTitles, hLabelTitle);

    BOOL bFoundDescription = StringListFetchByLabel(snoStringlistDescriptions, hLabelDescription, sTextDescription);

    CRefString sHeader = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_MESSAGES), "SetDungeonTitle");

    if (bFoundDescription && sTextDescription.Length() > 0)
    {
        sTextDescription = LocalizedSprintf(StringListFetch(snoStringlistDescriptions, "Sets_Generic_All"), sTextDescription.str());

        if (PlayersGetCount() > 1)
        {
            sTextDescription = sTextDescription + " ";

            if (PlayerGetLocalPlayerCount() > 1)
            {
                CRefString sHeroName("Invalid Hero");


                sHeroName = CPlayerGetServiceName(*ptPlayer, FALSE);


                sTextDescription = sTextDescription + LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_MESSAGES), "Sets_Generic_OtherMessage"), sHeroName.str());
            }
            else
            {

                sTextDescription = sTextDescription + StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_MESSAGES), "Sets_Generic_OwnerMessage");
            }
        }


        NotificationCreationParams tParams;
        tParams.uszTitle = sHeader;
        tParams.uszMessageTitle = sTextTitle;
        tParams.uszMessage = sTextDescription;
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_2<void, int32, const UIID &>::bind<&sAcceptedSetDungeonModal>(ptPlayer->nControllerIndex);
        tParams.nControllerIndex = ptPlayer->nControllerIndex;
        tParams.bLeftJustify = TRUE;

        s_idModal = ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
    }
}














void ShowCannotStartMessage(Player * ptPlayer)

{

    CRefString sTextDescription = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_MESSAGES), "CannotStartError");


    NotificationCreationParams tParams;
    tParams.uszMessage = sTextDescription;
    tParams.nControllerIndex = ptPlayer->nControllerIndex;

    ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
}







CRefString sGetResultStringForQuest(SNO snoQuest)
{

    CRefString strResult("StringNotFound");

    ScopedQuestDefinitionLockR tLock(snoQuest);
    SNO snoStringList = QuestGetStringList(snoQuest);

    StringListFetch(snoStringList, "Result", strResult);

    return strResult;
}

void sSetDescriptionAndCheckBox(const UIID & idSection, const CRefString & sDescription, BOOL bSuccess)
{



    UIID idCheckBoxSuccess = UIIDCreateFromParent(idSection, ".CheckBoxSuccess");
    UI_GetWindowFromID(idCheckBoxSuccess).SetActive(bSuccess);

    UIID idCheckBoxFail = UIIDCreateFromParent(idSection, ".CheckBoxFailed");
    UI_GetWindowFromID(idCheckBoxFail).SetActive(!bSuccess);

    UIID idText = UIIDCreateFromParent(idSection, ".Description");
    UI_GetTextFromID(idText).SetText(sDescription, FALSE);
}





void ShowResults(const SetDungeonResultsMessage * ptResultsMsg)

{
    UIText & tTextDescription = UI_GetTextFromID(UIWND_SETDUNGEONCOMPLETION_DESCRIPTION);

    if (ptResultsMsg->bQuestMasterySuccess)
    {
        tTextDescription.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_COMPLETION), "Sets_Complete_3"), FALSE);
    }
    else if (ptResultsMsg->bQuestKillSuccess && (ptResultsMsg->bQuestBonus1Success || ptResultsMsg->bQuestBonus2Success))
    {
        tTextDescription.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_COMPLETION), "Sets_Complete_2"), FALSE);
    }
    else
    {
        tTextDescription.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_COMPLETION), "Sets_Complete_1"), FALSE);
    }

    UIID idBasicHeader = UIIDCreateFromParent(UIWND_SETDUNGEONCOMPLETION_CONTENT, ".BasicHeader");
    UI_GetWindowFromID(idBasicHeader).SetActive(!ptResultsMsg->bQuestMasterySuccess);

    UIID idMasteryHeader = UIIDCreateFromParent(UIWND_SETDUNGEONCOMPLETION_CONTENT, ".MasteryHeader");
    UI_GetWindowFromID(idMasteryHeader).SetActive(ptResultsMsg->bQuestMasterySuccess);









    UIID idResultsTitle = UIIDCreateFromParent(UIWND_SETDUNGEONCOMPLETION_CONTENT, ".List.ResultsTitle");













    sSetDescriptionAndCheckBox(UIWND_SETDUNGEONCOMPLETION_OBJECTIVE_1,
                               sGetResultStringForQuest(ptResultsMsg->snoQuestBonus1),
                               ptResultsMsg->bQuestBonus1Success);


    sSetDescriptionAndCheckBox(UIWND_SETDUNGEONCOMPLETION_OBJECTIVE_2,
                               sGetResultStringForQuest(ptResultsMsg->snoQuestBonus2),
                               ptResultsMsg->bQuestBonus2Success);


    sSetDescriptionAndCheckBox(UIWND_SETDUNGEONCOMPLETION_GENERAL_OBJECTIVE,
                               LocalizedSprintf(sGetResultStringForQuest(ptResultsMsg->snoQuestKill), UnicodeText::FormatNumber(ptResultsMsg->nQuestKillMonsterCount, TRUE, FNS_NORMAL, 0).str()),
                               ptResultsMsg->bQuestKillSuccess);


    sSetDescriptionAndCheckBox(UIWND_SETDUNGEONCOMPLETION_MASTERY_KILL_OBJECTIVE,
                               LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_COMPLETION), "KillAllMonsters"), UnicodeText::FormatNumber(ptResultsMsg->nQuestKillMonsterCount, TRUE, FNS_NORMAL, 0).str()),
                               ptResultsMsg->bQuestMasterySuccess);



    BOOL bTimeSuccess = ptResultsMsg->nTimeTaken <= ptResultsMsg->nTargetTime;

    UIID idCheckBoxSuccess3 = UIIDCreateFromParent(UIWND_SETDUNGEONCOMPLETION_PLAYER_TIME_OBJECTIVE, ".CheckBoxSuccess");
    UI_GetWindowFromID(idCheckBoxSuccess3).SetActive(bTimeSuccess);
    UIID idCheckBoxFail3 = UIIDCreateFromParent(UIWND_SETDUNGEONCOMPLETION_PLAYER_TIME_OBJECTIVE, ".CheckBoxFailed");
    UI_GetWindowFromID(idCheckBoxFail3).SetActive(!bTimeSuccess);

    ElapsedTimeDisplayOptions tElapsedTimeDisplayOptions;
    tElapsedTimeDisplayOptions.fAlwaysShowSeconds = TRUE;
    tElapsedTimeDisplayOptions.fOnlyDisplayNumber = TRUE;
    tElapsedTimeDisplayOptions.fCountUp = TRUE;
    tElapsedTimeDisplayOptions.fDisplayMilliseconds = FALSE;
    CRefString sFormattedTimeTaken = DisplayElapsedTime(0, (Blizzard::Time::Timestamp)ptResultsMsg->nTimeTaken * 1000000000, tElapsedTimeDisplayOptions);
    CRefString sFormattedTargetTime = DisplayElapsedTime(0, (Blizzard::Time::Timestamp)ptResultsMsg->nTargetTime * 1000000000, tElapsedTimeDisplayOptions);

    UIID idTimeTakenText = UIIDCreateFromParent(UIWND_SETDUNGEONCOMPLETION_PLAYER_TIME_OBJECTIVE, ".PlayerTime");
    UI_GetTextFromID(idTimeTakenText).SetText(LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_COMPLETION), "PlayerTime"), sFormattedTimeTaken.str()), FALSE);
    UI_GetWindowFromID(idTimeTakenText).SetActive(ptResultsMsg->bShowTotalTime);

    UIID idTargetTimeText = UIIDCreateFromParent(UIWND_SETDUNGEONCOMPLETION_PLAYER_TIME_OBJECTIVE, ".GoalTime");
    UI_GetTextFromID(idTargetTimeText).SetText(LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_COMPLETION), "TargetTime"), sFormattedTargetTime.str()), FALSE);




    CRefString sHelpers[4];


    int32 nOwnerIndex = LocalPlayerGet()->nPlayerIndex;
    int32 nHelpers = 0;
    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (ptPlayer->nPlayerIndex != nOwnerIndex && (s_nAcceptedFlags & (1 << ptPlayer->nPlayerIndex)))
        {
            sHelpers[nHelpers++] = CPlayerGetServiceName(*ptPlayer, FALSE);
        }
    }

    UIText & tHelperText = UI_GetTextFromID(UIWND_SETDUNGEONCOMPLETION_HELPERS);

    switch (nHelpers)
    {
    case 3:
        tHelperText.SetText(LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_COMPLETION), "ThreeHelpers"), sHelpers[0].str(), sHelpers[1].str(), sHelpers[2].str()), FALSE);
        break;
    case 2:
        tHelperText.SetText(LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_COMPLETION), "TwoHelpers"), sHelpers[0].str(), sHelpers[1].str()), FALSE);
        break;
    case 1:
        tHelperText.SetText(LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_COMPLETION), "OneHelper"), sHelpers[0].str()), FALSE);
        break;
    }




    tHelperText.SetActive(nHelpers > 0);


    UI_GetWindowFromID(UIWND_SETDUNGEONCOMPLETION_MASTERY_NOTIFICATION).SetActive(!ptResultsMsg->bQuestMasterySuccess);

    UI_GetWindowFromID(UIWND_SETDUNGEONCOMPLETION_MAIN).SetActive(TRUE);
}

void ResultsClose(const UIID &)
{
}






void Complete_OnButton1(const UIID & id, const int32 nControllerIndex)
{
    UI_GetWindowFromID(UIWND_SETDUNGEONCOMPLETION_MAIN).SetActive(FALSE);
}







UISetEncounterWindowBase::UISetEncounterWindowBase()
    : m_dwStartTime(0), m_dwStopTime(0), m_nControllerIndex(-1), m_dwOpenTime(0)
{
}























void UISetEncounterWindowBase::SetTimer(uint32 dwSecondsRemaining)
{

    m_dwStartTime = CGameGetClientTicks();
    m_dwStopTime = m_dwStartTime + dwSecondsRemaining * 60;
}





BOOL UISetEncounterWindowBase::PlayerHasControl(const int32 nControllerIndex) const
{

    return m_nControllerIndex == -1 || m_nControllerIndex == nControllerIndex;
}






































void UISetEncounterJoinPartyWindow::Initialize(const SetDungeonDialogMessage * ptMessage, Player *, Player * ptPlayerInstigator)



{

    SNO snoStringlistJoinMessages = GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_MESSAGES);
    SNO snoStringlistTitles = GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_TITLES);

    CRefString sEncounterDesc = StringListFetch(snoStringlistJoinMessages, "SetDungeonJoinDescription");

    CRefString sEncounterName;
    BOOL bFound = StringListFetchByLabel(snoStringlistTitles, ptMessage->nLabelTitle, sEncounterName);
    if (bFound)
    {
        UI_GetTextFromID(UITXT_SETDUNGEONJOINPARTY_LOCATION).SetText(sEncounterName, FALSE);
    }

    CRefString sHeroName("Invalid Hero");
    if (ptPlayerInstigator)
    {
        sHeroName = CPlayerGetServiceName(*ptPlayerInstigator, FALSE);
    }

    UI_GetTextFromID(UITXT_SETDUNGEONJOINPARTY_DESCRIPTION).SetText(LocalizedSprintf(sEncounterDesc.str(), sHeroName.str()), FALSE);












    SetTimer(30);

    UI_GetWindowFromID(UIDLG_SETDUNGEONJOINPARTY_MAIN).SetActive(TRUE);
}




void UISetEncounterJoinPartyWindow::SetTimer(uint32 dwSecondsRemaining)
{

    UIText & tTxtTimer = UI_GetTextFromID(UITXT_SETDUNGEONJOINPARTY_TIMER);
    tTxtTimer.SetText(UIText_GetTimeString(dwSecondsRemaining), FALSE);

    UISetEncounterWindowBase::SetTimer(dwSecondsRemaining);
}



void UISetEncounterJoinPartyWindow::Close()
{
    UI_GetWindowFromID(UIDLG_SETDUNGEONJOINPARTY_MAIN).SetActive(FALSE);
}

void UISetEncounterJoinPartyWindow::Tick()

{
    uint32 dwNow = CGameGetClientTicks();
    if (m_dwStopTime < dwNow)
    {
        JoinParty_Cancel(GetUIIDNone());
    }
    else
    {

        uint32 dwSecondsRemaining = (m_dwStopTime - dwNow) / 60;

        UIText & tTxtTimer = UI_GetTextFromID(UITXT_SETDUNGEONJOINPARTY_TIMER);
        tTxtTimer.SetText(UIText_GetTimeString(dwSecondsRemaining), FALSE);
    }
}



void InviteStatusUpdate(const EncounterInviteStateMessage * pState)
{
    s_nAcceptedFlags = pState->m_AcceptedPlayers;
}


































void Started()
{



    UIBossEncounter::Started();
}






















void Canceled(BOOL)

{



    CRefString sTextDescription = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_MESSAGES), "SetDungeonCancelled");

    UIGame::DisplayError(GameErrorData(GAMEERROR_SET_DUNGEON_SOMEONE_DECLINED), FALSE, NULL);
}




void JoinParty_Open(Player * ptPlayer, const SetDungeonDialogMessage * ptMsg)


{
    sJoinPartyWindow.Close();

    Player * ptPlayerInstigator = PlayerGetByPlayerIndex(ptMsg->nPlayerIndex);


    if (s_idModal != INVALID_MODAL_NOTIFICATION_ID && ::Console::Screens::GetModalNotificationManager().GetCurrentlyShowingNotification() == s_idModal)
    {
        ::Console::Screens::GetModalNotificationManager().ConsoleCancelCurrentlyShowingNotification(ptPlayerInstigator->nControllerIndex);
    }







    sJoinPartyWindow.Initialize(ptMsg, ptPlayer, ptPlayerInstigator);
}




void JoinParty_Update(const UIID &)
{
    sJoinPartyWindow.Tick();
}





void JoinParty_Cancel(const UIID &)

{
    SimpleMessage tMessage;
    MessageSendToServer(CMSG_SET_DUNGEON_JOIN_DECLINED, &tMessage, sizeof(tMessage), LocalPlayerGet());
    sJoinPartyWindow.Close();
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
            MessageSendToServer(CMSG_SET_DUNGEON_JOIN_ACCEPTED, &tMessage, sizeof(tMessage), LocalPlayerGet());
            sJoinPartyWindow.Close();
        }
    }
    else
    {
        UIGame::DisplayError(GameErrorData(GAMEERROR_CANNOT_DO_THIS_WHILE_DEAD), FALSE, NULL);
    }
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

        szDisplay = StringListFetch("SetDungeonJoinParty:Continue_Without_Player_0");
        break;

    case 1:
    {

        CRefString sFormat = StringListFetch("SetDungeonJoinParty:Continue_Without_Player_1");
        CRefString szPlayer1 = CPlayerGetServiceName(*arrPlayersDeclined[0], FALSE);
        szDisplay = LocalizedSprintf(sFormat.str(), szPlayer1.str());
        break;
    }

    case 2:
    {
        CRefString sFormat = StringListFetch("SetDungeonJoinParty:Continue_Without_Player_2");
        CRefString szPlayer1 = CPlayerGetServiceName(*arrPlayersDeclined[0], FALSE);
        CRefString szPlayer2 = CPlayerGetServiceName(*arrPlayersDeclined[1], FALSE);
        szDisplay = LocalizedSprintf(sFormat.str(), szPlayer1.str(), szPlayer2.str());
        break;
    }

    case 3:
    {
        CRefString sFormat = StringListFetch("SetDungeonJoinParty:Continue_Without_Player_3");
        CRefString szPlayer1 = CPlayerGetServiceName(*arrPlayersDeclined[0], FALSE);
        CRefString szPlayer2 = CPlayerGetServiceName(*arrPlayersDeclined[1], FALSE);
        CRefString szPlayer3 = CPlayerGetServiceName(*arrPlayersDeclined[2], FALSE);
        szDisplay = LocalizedSprintf(sFormat.str(), szPlayer1.str(), szPlayer2.str(), szPlayer3.str());
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








void VoteContinue(const UIID &)
{
    SimpleMessage tMessage;
    MessageSendToServer(CMSG_SET_DUNGEON_VOTE_CONTINUE, &tMessage, sizeof(tMessage), LocalPlayerGet());
}





void VoteCancel(const UIID &)
{
    SimpleMessage tMessage;
    MessageSendToServer(CMSG_SET_DUNGEON_VOTE_CANCEL, &tMessage, sizeof(tMessage), LocalPlayerGet());
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
    return UI_GetWindowFromID(UIDLG_SETDUNGEONJOINPARTY_MAIN).IsActive();
}



























BOOL IsSetDungeonOwner(const Player * ptPlayer)
{
    return ptPlayer->nPlayerIndex == s_nOwningPlayerIndex;
}

}  // namespace UISetDungeon
