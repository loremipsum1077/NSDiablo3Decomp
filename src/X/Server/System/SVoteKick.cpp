// X/Server/System/SVoteKick.cpp -- the server side of the vote-kick vote. One VoteKickState
// hangs off the current SGame; the timer callbacks end the vote, and every function sits on its
// original line (the reference .cpp gives the lines).

#include "SVoteKick.h"

#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/Player.h"
#include "../../Common/PlayerAccount.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../PlayerList.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../SPlayer.h"
#include "../Timers.h"

// Declared locally until their headers are adopted; the comment names the defining file.
uint32 GameGetServerTime();  // X/Common/GameCommonData.cpp:292
namespace BossEncounterManager { uint32 GetLastCompleteTime(); }  // X/Server/System/BossEncounter.cpp:149
template <>
uint32 XHashKey<const OnlineService::GameAccountHandle &>(const OnlineService::GameAccountHandle & gameAccount);  // X/Common/OnlineService/OnlineService.cpp:1010

namespace VoteKick {




struct VoteKickState
{
    int32 nPlayerIndexStarted;
    int32 nPlayerIndexTarget;
    TimerID idVoteTimeOut;
    XGrowableList<OnlineService::GameAccountHandle, OnlineService::GameAccountHandle &> tKickedPlayers;


    void Clear()
    {
        nPlayerIndexStarted = -1;
        nPlayerIndexTarget = -1;
        idVoteTimeOut = TIMERID_INVALID;
    }

    VoteKickState() { Clear(); }
};

namespace Server {


inline BOOL IsPublicGame()
{

    return SGameManager::GameGet().tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PUBLIC_BIT);
}


inline VoteKickState * GetState()
{

    return SGameManager::GameGet().ptVoteKickState;
}


inline BOOL KickAlreadyInProgress()
{

    const VoteKickState * pState = GetState();
    return pState == NULL || pState->idVoteTimeOut != TIMERID_INVALID;
}


void OnVoteKickTimeOut(TimerID, DataID, void *)



{
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->bParticipatingInVoteKick)
        {
            VoteNo(ptPlayer);
        }
    }
}


void DoKick()

{
    VoteKickState * pState = GetState();


    Player * pTarget = PlayerGetByPlayerIndex(pState->nPlayerIndexTarget);
    if (pTarget != NULL)
    {
        pTarget->nTimesVoteKicked++;
        PlayerAccount tAccount = PlayerGetAccount(pTarget->idPlayerACD);
        pState->tKickedPlayers.AddTail_Tracked(tAccount.tHandle);

        SPlayerStartLogoutTimer(pTarget->nPlayerIndex, TRUE, LOGOUT_CONTEXT_SOLO);


        SGameManager::GameGet().tKickMap.SetAt_Tracked(pTarget->tAccount.tHandle, TRUE);
    }


    if (IsPublicGame())
    {

        Player * ptInitiatingPlayer = PlayerGetByPlayerIndex(pState->nPlayerIndexStarted);
        if (ptInitiatingPlayer != NULL)
        {
            ptInitiatingPlayer->dwLastSuccessfulKick = GameGetServerTime();
            ptInitiatingPlayer->nKicksInitiated++;

            SPlayerSendKickTimers(*ptInitiatingPlayer);
        }

        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllInGame(listDestPlayers, pState->nPlayerIndexTarget, FALSE, -1);


        PlayerIndexMessage tMessage;
        tMessage.nPlayerIndex = pState->nPlayerIndexTarget;
        MessageSendToClient(listDestPlayers, SMSG_VOTE_KICK_PASSED, &tMessage, sizeof(tMessage));
        POS pos = listDestPlayers.GetHeadPosition();
        while (pos != NULL)
        {
            Player * ptPlayer = listDestPlayers.GetNext(pos);
            if (ptPlayer != ptInitiatingPlayer)
            {
                ptPlayer->nKicksParticipatedIn++;
            }

            ptPlayer->bParticipatedInVoteKick = TRUE;
        }
    }

    pState->Clear();
}


void TryVoteKick(VoteKickMessage * ptMessage)

{
    Player * ptInitiatingPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndexInitiating);
    const Player * ptTarget = PlayerGetByPlayerIndex(ptMessage->nPlayerIndexToKick);

    if (!CanVoteKick(ptInitiatingPlayer, ptTarget))
    {
        return;
    }


    VoteKickState * pState = GetState();
    if (pState != NULL)
    {
        pState->nPlayerIndexStarted = ptMessage->nPlayerIndexInitiating;
        pState->nPlayerIndexTarget = ptMessage->nPlayerIndexToKick;
        pState->idVoteTimeOut = TimerRegister(-1, TIMER_OWNER_NONE, 7500, NULL, OnVoteKickTimeOut);
    }









    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, ptTarget->nPlayerIndex, FALSE, -1);
    POS pos = listDestPlayers.Find(ptInitiatingPlayer);
    if (pos != NULL)
    {
        listDestPlayers.RemoveAt(pos);
    }

    MessageSendToClient(listDestPlayers, SMSG_VOTE_KICK_VOTE, ptMessage, sizeof(VoteKickMessage));

    ptInitiatingPlayer->dwLastKickAttempt = GameGetServerTime();
    SPlayerSendKickTimers(*ptInitiatingPlayer);

    pos = listDestPlayers.GetHeadPosition();
    while (pos != NULL)
    {
        Player * ptPlayer = listDestPlayers.GetNext(pos);
        ptPlayer->bParticipatingInVoteKick = TRUE;
    }

}


BOOL CanVoteKick(const Player * ptInitiatingPlayer, const Player * ptTarget)


{
    if (PlayersGetCount() < 3)
    {
        return FALSE;
    }

    if (ptInitiatingPlayer == NULL || ptTarget == NULL || ptTarget == ptInitiatingPlayer)
    {
        return FALSE;
    }






    if (KickAlreadyInProgress())
    {
        return FALSE;
    }

    if (!PlayerCanAttemptVoteKick(*ptInitiatingPlayer))
    {
        return FALSE;
    }

    ScopedGlobalDefinitionLockR pdefGlobals;

    uint32 dwNow = GameGetServerTime();

    if (BossEncounterManager::GetLastCompleteTime() != 0)
    {
        uint32 dwTicksAfterBoss = (uint32)(pdefGlobals->ptServerData->flVoteKickAfterBossDelay * 60.0f);
        if (BossEncounterManager::GetLastCompleteTime() + dwTicksAfterBoss > dwNow)
        {
            return FALSE;
        }
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.dwLastChampRareUniqueKilled != 0)
    {
        uint32 dwTicksAfterChampRareUnique = (uint32)(pdefGlobals->ptServerData->flVoteKickAfterChampRareUniqueDelay * 60.0f);
        if (tSGameGlobals.dwLastChampRareUniqueKilled + dwTicksAfterChampRareUnique > dwNow)
        {
            return FALSE;
        }
    }

    return TRUE;
}


void KickRequested(const Player & tPlayerAsking, const Player * ptTarget)


{
    if (CanVoteKick(&tPlayerAsking, ptTarget))
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, tPlayerAsking);

        PlayerIndexMessage tMessage;
        tMessage.nPlayerIndex = ptTarget->nPlayerIndex;
        MessageSendToClient(listDestPlayers, SMSG_CAN_VOTE_KICK, &tMessage, sizeof(tMessage));
    }
    else
    {
        PlayerDisplayErrorMessage(tPlayerAsking.idPlayerACD, GAMEERROR_CANNOT_KICK_NOW);
    }
}


void Cancel()

{
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ptPlayer->bParticipatingInVoteKick = FALSE;
    }

    VoteKickState * pState = GetState();

    if (TimerExists(pState->idVoteTimeOut))
    {
        TimerUnregister(pState->idVoteTimeOut);
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, pState->nPlayerIndexTarget, FALSE, -1);

    PlayerIndexMessage tMessage;
    tMessage.nPlayerIndex = pState->nPlayerIndexTarget;
    MessageSendToClient(listDestPlayers, SMSG_VOTE_KICK_FAILED, &tMessage, sizeof(tMessage));

    pState->Clear();
}


void VoteNo(Player * ptPlayerVoting)

{
    if (ptPlayerVoting->bParticipatingInVoteKick && KickAlreadyInProgress())
    {



        Cancel();
    }
}

void OnVoteKickDoKick(TimerID, DataID, void *)

{


    DoKick();
}


void VoteYes(Player * ptPlayerVoting)

{
    if (!ptPlayerVoting->bParticipatingInVoteKick || !KickAlreadyInProgress())
    {
        return;
    }

    ptPlayerVoting->bParticipatingInVoteKick = FALSE;
    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->bParticipatingInVoteKick)
        {
            return;
        }
    }

    const VoteKickState * pState = GetState();
    TimerUnregister(pState->idVoteTimeOut);

    TimerRegister(-1, TIMER_OWNER_NONE, 300, NULL, OnVoteKickDoKick);
}


void OnRemovedPlayer(const Player & tPlayer)

{


    if (KickAlreadyInProgress())
    {
        Cancel();
    }
}


void StateDestroyForGame()

{
    if (SGameManager::GameGet().ptVoteKickState != NULL)
    {
        delete SGameManager::GameGet().ptVoteKickState; SGameManager::GameGet().ptVoteKickState = NULL;
    }
}


void StateCreateForGame()

{
    SGameManager::GameGet().ptVoteKickState = new VoteKickState;
}

}  // namespace Server

}  // namespace VoteKick
