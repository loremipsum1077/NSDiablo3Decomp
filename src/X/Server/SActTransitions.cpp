// X/Server/SActTransitions.cpp -- the server side of an act transition: a player starts it, the
// others opt in, and once everyone has (or the timer runs out) the game advances acts.
#include "SActTransitions.h"

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/ActTransitions.h"
#include "../Common/Definitions/GlobalDefinition.h"
#include "../Common/GameCommonData.h"
#include "../Common/GameConstants.h"
#include "../Common/Networking/GameMessage.h"
#include "../Common/Player.h"
#include "PlayerList.h"
#include "SGame.h"
#include "SGameConstants.h"
#include "SGameManager.h"

struct SActTransitionGlobals {  // size 0x28
    uint32 dwActTransitionTime;  // 0x00
    BOOL bRecreateGameWithParty;  // 0x04
    UpdateData tUpdateData;  // 0x08
};
static_assert(sizeof(SActTransitionGlobals) == 0x28, "SActTransitionGlobals");
static_assert(__builtin_offsetof(SActTransitionGlobals, bRecreateGameWithParty) == 0x04, "SActTransitionGlobals::bRecreateGameWithParty");
static_assert(__builtin_offsetof(SActTransitionGlobals, tUpdateData) == 0x08, "SActTransitionGlobals::tUpdateData");










static void sDoActTransition()
{

    if (SGameIsShuttingDown())
    {
        return;
    }

    SGame & tSGame = SGameManager::GameGet();
    SActTransitionGlobals * ptActTransitionGlobals = tSGame.ptSActTransitionGlobals;

    if (!ptActTransitionGlobals->bRecreateGameWithParty)
    {





        SGameAdvanceActs();
    }

    SActTransitionEnd();
}




void SActTransitionsInitializeForGame()

{
    SGame & tSGame = SGameManager::GameGet();
    tSGame.ptSActTransitionGlobals = new SActTransitionGlobals;
    tSGame.ptSActTransitionGlobals->dwActTransitionTime = NO_ACT_TRANSITION_TIME;
    tSGame.ptSActTransitionGlobals->bRecreateGameWithParty = FALSE;

    UpdateFunctionInitialize(UPDATE_ACTTRANSITIONS, -1, tSGame.ptSActTransitionGlobals->tUpdateData);
    ServerQueueUpdateFunction(1, tSGame.ptSActTransitionGlobals->tUpdateData);
}




void SActTransitionsDisposeForGame()

{
    SGame & tSGame = SGameManager::GameGet();
    delete tSGame.ptSActTransitionGlobals; tSGame.ptSActTransitionGlobals = NULL;
}




void SActTransitionsUpdate()

{

    SGame & tSGame = SGameManager::GameGet();
    SActTransitionGlobals * ptActTransitionGlobals = tSGame.ptSActTransitionGlobals;

    ServerUpdateWasCalled(ptActTransitionGlobals->tUpdateData);
    ServerQueueUpdateFunction(1, ptActTransitionGlobals->tUpdateData);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_PREVENT_ACT_TRANSITION_BIT))
    {
        ptActTransitionGlobals->dwActTransitionTime = NO_ACT_TRANSITION_TIME;
        return;
    }

    uint32 dwNow = GameGetServerTime();
    if (dwNow >= ptActTransitionGlobals->dwActTransitionTime && ptActTransitionGlobals->dwActTransitionTime != NO_ACT_TRANSITION_TIME)
    {

        sDoActTransition();
    }
}























void SActTransitionStart(Player * ptPlayer, BOOL bRecreateGameWithParty)


{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_PREVENT_ACT_TRANSITION_BIT))
    {
        return;
    }

    SGame & tSGame = SGameManager::GameGet();
    SActTransitionGlobals * ptActTransitionGlobals = tSGame.ptSActTransitionGlobals;
    if (ptActTransitionGlobals->dwActTransitionTime != NO_ACT_TRANSITION_TIME &&
        ptActTransitionGlobals->bRecreateGameWithParty != bRecreateGameWithParty)
    {
        return;
    }

    if (ptActTransitionGlobals->dwActTransitionTime == NO_ACT_TRANSITION_TIME)
    {

        ScopedGlobalDefinitionLockR pdefGlobal;

        int32 nTime = pdefGlobal->ptServerData->nActTransitionTime;
        ptActTransitionGlobals->dwActTransitionTime = GameGetServerTime() + nTime * TICKS_PER_SECOND;
        tSGame.ptSActTransitionGlobals->bRecreateGameWithParty = bRecreateGameWithParty;
    }

    ptPlayer->fOptedIntoActTransition = TRUE;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
    ActTransitionStartedMessage tMessage;
    tMessage.dwActTransitionTime = ptActTransitionGlobals->dwActTransitionTime;
    tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;

    Act eCurrentAct = GameGetCurrentAct();
    Act eMaxAct = SGameGetCurrentMaxGameAct();
    if (eCurrentAct == eMaxAct)
    {
        MessageSendToClient(listDestPlayers, SMSG_START_END_COUNTDOWN, &tMessage, sizeof(tMessage));
    }
    else
    {
        MessageSendToClient(listDestPlayers, SMSG_ACT_TRANSITION_STARTED, &tMessage, sizeof(tMessage));
    }

    SActTransitionOptInsUpdated();
}



void SActTransitionEnd()

{
    SGame & tSGame = SGameManager::GameGet();
    if (tSGame.ptSActTransitionGlobals == NULL)
    {
        return;
    }

    tSGame.ptSActTransitionGlobals->dwActTransitionTime = NO_ACT_TRANSITION_TIME;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
    SimpleMessage tMessage;
    MessageSendToClient(listDestPlayers, SMSG_ACT_TRANSITION_ENDED, &tMessage, sizeof(tMessage));
    Player * ptPlayer = PlayerGetFirstInGame();
    while (ptPlayer != NULL)
    {
        ptPlayer->fOptedIntoActTransition = FALSE;
        ptPlayer = PlayerGetNextInGame(ptPlayer);
    }
}



void SActTransitionOptInsUpdated()
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_PREVENT_ACT_TRANSITION_BIT))
    {
        return;
    }

    Player * ptPlayer = PlayerGetFirstInGame();
    if (ptPlayer == NULL)
    {
        return;
    }

    while (ptPlayer != NULL)
    {
        if (!ptPlayer->fOptedIntoActTransition)
        {
            return;
        }

        ptPlayer = PlayerGetNextInGame(ptPlayer);
    }

    sDoActTransition();
}


void SActTransitionSetPreventActTransition(BOOL bPreventActTransition)

{
    if (bPreventActTransition) SGameGlobalsGet().dwFlags |= (1 << SGAMEGLOBALS_PREVENT_ACT_TRANSITION_BIT); else SGameGlobalsGet().dwFlags &= ~(1 << SGAMEGLOBALS_PREVENT_ACT_TRANSITION_BIT);

    if (bPreventActTransition)
    {

        SGame & tSGame = SGameManager::GameGet();
        SActTransitionGlobals * ptActTransitionGlobals = tSGame.ptSActTransitionGlobals;

        ptActTransitionGlobals->dwActTransitionTime = NO_ACT_TRANSITION_TIME;

        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
        SimpleMessage tMessage;
        MessageSendToClient(listDestPlayers, SMSG_ACT_TRANSITION_ENDED, &tMessage, sizeof(tMessage));
    }
}
