// X/Server/STimedEvents.cpp
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/GameCommonData.h"
#include "../Common/TimedEvents.h"
#include "../Common/Definitions/TimedEventDefinition.h"
#include "../Common/Networking/GameMessage.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/DataStructures/XGrowableArray.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/Math/MathGeneral.h"
#include "LuaScript/ScriptEvent.h"
#include "PlayerList.h"
#include "Quests/SQuests.h"
#include "SGame.h"
#include "SGameManager.h"














struct STimedEventGlobals {  // size 0xf0
    ActiveEventList tActiveEventList;  // 0x00
    SNOArray tExpiredEventList;  // 0x58
    SNOArray tQueuedDeleteList;  // 0x90
    BOOL bListLockedForUpdate;  // 0xc8
    UpdateData tUpdateData;  // 0xd0
};
static_assert(sizeof(STimedEventGlobals) == 0xf0, "STimedEventGlobals");
static_assert(__builtin_offsetof(STimedEventGlobals, tExpiredEventList) == 0x58, "STimedEventGlobals::tExpiredEventList");
static_assert(__builtin_offsetof(STimedEventGlobals, tQueuedDeleteList) == 0x90, "STimedEventGlobals::tQueuedDeleteList");
static_assert(__builtin_offsetof(STimedEventGlobals, bListLockedForUpdate) == 0xc8, "STimedEventGlobals::bListLockedForUpdate");
static_assert(__builtin_offsetof(STimedEventGlobals, tUpdateData) == 0xd0, "STimedEventGlobals::tUpdateData");

void STimedEventsInitializeForGame()

{
    SGame & tSGame = SGameManager::GameGet();
    STimedEventGlobals *& ptTimedEventGlobals = tSGame.ptSTimedEventGlobals;
    ptTimedEventGlobals = new STimedEventGlobals;
    ptTimedEventGlobals->bListLockedForUpdate = FALSE;

    UpdateFunctionInitialize(UPDATE_TIMEDEVENTS, -1, ptTimedEventGlobals->tUpdateData);
    ServerQueueUpdateFunction(1, ptTimedEventGlobals->tUpdateData);
}




void STimedEventsDisposeForGame()

{
    SGame & tSGame = SGameManager::GameGet();
    STimedEventGlobals *& ptTimedEventGlobals = tSGame.ptSTimedEventGlobals;

    delete ptTimedEventGlobals; ptTimedEventGlobals = NULL;
}




void STimedEventReset(const SNO snoTimedEvent)

{
    SGame & tSGame = SGameManager::GameGet();
    STimedEventGlobals * ptEventGlobals = tSGame.ptSTimedEventGlobals;

    if (ptEventGlobals == NULL)
        return;


    POS posActive = ptEventGlobals->tActiveEventList.GetHeadPosition();

    while (posActive)
    {
        POS pos = posActive;
        ActiveEvent & tEvent = ptEventGlobals->tActiveEventList.GetNext(posActive);
        if (tEvent.snoTimedEvent == snoTimedEvent)
        {
            if (ptEventGlobals->bListLockedForUpdate)
            {
                ptEventGlobals->tQueuedDeleteList.Add_Tracked(snoTimedEvent);
            }
            else
            {
                ptEventGlobals->tActiveEventList.RemoveAt(pos);
            }
            break;
        }
    }

    for (int32 i = 0; i < ptEventGlobals->tExpiredEventList.GetArraySize(); i++)
    {
        if (ptEventGlobals->tExpiredEventList[i] == snoTimedEvent)
        {
            ptEventGlobals->tExpiredEventList.RemoveAt(i);
            break;
        }
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

    SNODataMessage tMessage;
    tMessage.snoData = snoTimedEvent;

    MessageSendToClient(listDestPlayers, SMSG_TIMED_EVENT_RESET, &tMessage, sizeof(tMessage));
}



void STimedEventsUpdate()

{

    SGame & tSGame = SGameManager::GameGet();
    STimedEventGlobals * ptEventGlobals = tSGame.ptSTimedEventGlobals;

    ServerUpdateWasCalled(ptEventGlobals->tUpdateData);
    ServerQueueUpdateFunction(1, ptEventGlobals->tUpdateData);

    ptEventGlobals->bListLockedForUpdate = TRUE;
    uint32 dwNow = GameGetServerTime();
    POS pos = ptEventGlobals->tActiveEventList.GetHeadPosition();
    while (pos)
    {
        POS posPrev = pos;
        ActiveEvent & tEvent = ptEventGlobals->tActiveEventList.GetNext(pos);

        uint32 dwTimeElapsed = dwNow - tEvent.dwStartTime + tEvent.dwArtificiallyElapsedTime;
        uint32 dwTimeRemaining = (tEvent.dwExpirationTime - tEvent.dwArtificiallyElapsedTime > dwNow) ? tEvent.dwExpirationTime - tEvent.dwArtificiallyElapsedTime - dwNow : 0;
        ScriptEventOnTimedEventTick(tEvent.snoTimedEvent, dwTimeElapsed, dwTimeRemaining);

        if (tEvent.dwExpirationTime - tEvent.dwArtificiallyElapsedTime <= dwNow)
        {
            SQuestsOnTimedEventExpired(tEvent.snoTimedEvent);
            {

                PlayerList listDestPlayers(PlayerListGetElementPool());
                PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

                SNODataMessage tMessage;
                tMessage.snoData = tEvent.snoTimedEvent;

                MessageSendToClient(listDestPlayers, SMSG_TIMED_EVENT_FINISHED, &tMessage, sizeof(tMessage));

                ptEventGlobals->tExpiredEventList.Add_Tracked(tEvent.snoTimedEvent);
            }

            ptEventGlobals->tActiveEventList.RemoveAt(posPrev);
        }
        else if ((dwNow % 15) == 0)
        {
            SQuestsOnTimedEventAboutToExpire(tEvent.snoTimedEvent, tEvent.dwExpirationTime - tEvent.dwArtificiallyElapsedTime - dwNow);
        }
    }

    ptEventGlobals->bListLockedForUpdate = FALSE;


    if (ptEventGlobals->tQueuedDeleteList.GetArraySize() > 0)
    {
        POS posEvent = ptEventGlobals->tActiveEventList.GetHeadPosition();
        while (posEvent)
        {
            POS posPrev = posEvent;
            ActiveEvent & tEvent = ptEventGlobals->tActiveEventList.GetNext(posEvent);


            for (int32 i = 0; i < ptEventGlobals->tQueuedDeleteList.GetArraySize(); i++)
            {
                if (ptEventGlobals->tQueuedDeleteList[i] == tEvent.snoTimedEvent)
                {
                    ptEventGlobals->tActiveEventList.RemoveAt(posPrev);
                    break;
                }
            }
        }

        ptEventGlobals->tQueuedDeleteList.EmptyArray();
    }
}




void STimedEventOnPlayerEnterWorld(Player & tPlayer)

{
    SGame & tSGame = SGameManager::GameGet();
    STimedEventGlobals * ptEventGlobals = tSGame.ptSTimedEventGlobals;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(&tPlayer);

    POS pos = ptEventGlobals->tActiveEventList.GetHeadPosition();
    while (pos)
    {
        ActiveEvent & tEvent = ptEventGlobals->tActiveEventList.GetNext(pos);

        TimedEventStartedMessage tMessage;
        tMessage.tEvent = tEvent;

        MessageSendToClient(listDestPlayers, SMSG_TIMED_EVENT_STARTED, &tMessage, sizeof(tMessage));
    }
}




void STimedEventStart(const SNO snoTimedEvent,
                      const int32 nOverrideDuration)

{
    SGame & tSGame = SGameManager::GameGet();
    STimedEventGlobals * ptEventGlobals = tSGame.ptSTimedEventGlobals;

    ScopedTimedEventDefinitionLockR pdefEvent(snoTimedEvent);

    uint32 dwNow = GameGetServerTime();
    ActiveEvent tNewEvent;
    tNewEvent.snoTimedEvent = snoTimedEvent;
    tNewEvent.dwStartTime = dwNow;
    tNewEvent.dwExpirationTime = dwNow + (nOverrideDuration ? nOverrideDuration : pdefEvent->dwDuration);
    tNewEvent.dwArtificiallyElapsedTime = 0;

    ptEventGlobals->tActiveEventList.AddTail_Tracked(tNewEvent);

    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

        TimedEventStartedMessage tMessage;
        tMessage.tEvent = tNewEvent;

        MessageSendToClient(listDestPlayers, SMSG_TIMED_EVENT_STARTED, &tMessage, sizeof(tMessage));
    }
}
























BOOL STimedEventHasExpired(const SNO snoTimedEvent)

{
    SGame & tSGame = SGameManager::GameGet();
    STimedEventGlobals * ptEventGlobals = tSGame.ptSTimedEventGlobals;

    if (ptEventGlobals->tExpiredEventList.Find(snoTimedEvent) != -1)
        return TRUE;

    return FALSE;
}





void STimedEventModifyExpirationTime(const SNO snoTimedEvent,
                                     int32 nTicks)

{
    SGame & tSGame = SGameManager::GameGet();
    STimedEventGlobals * ptEventGlobals = tSGame.ptSTimedEventGlobals;

    POS pos = ptEventGlobals->tActiveEventList.GetHeadPosition();
    while (pos)
    {
        ActiveEvent & tEvent = ptEventGlobals->tActiveEventList.GetNext(pos);
        if (tEvent.snoTimedEvent == snoTimedEvent)
        {
            tEvent.dwExpirationTime = Math::Max<uint32>(tEvent.dwStartTime, tEvent.dwExpirationTime + nTicks);
        }
    }
}



void STimedEventAddArtificiallyElapsedTime(const SNO snoTimedEvent,
                                           int32 nTicks)

{
    SGame & tSGame = SGameManager::GameGet();
    STimedEventGlobals * ptEventGlobals = tSGame.ptSTimedEventGlobals;

    POS pos = ptEventGlobals->tActiveEventList.GetHeadPosition();
    while (pos)
    {
        ActiveEvent & tEvent = ptEventGlobals->tActiveEventList.GetNext(pos);
        if (tEvent.snoTimedEvent == snoTimedEvent)
        {
            tEvent.dwArtificiallyElapsedTime += nTicks;

            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

            TimedEventStartedMessage tMessage;
            tMessage.tEvent = tEvent;
            MessageSendToClient(listDestPlayers, SMSG_TIMED_EVENT_MODIFIED, &tMessage, sizeof(tMessage));
        }
    }
}
