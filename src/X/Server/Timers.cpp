// Original file: X/Server/Timers.cpp

#include "Timers.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/DataStructures/DataStructuresExternal.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "Actors/Actors.h"
#include "SGame.h"
#include "SGameManager.h"

struct Timer {  // size 0x20
    TimerID id;  // 0x00
    uint32 dwTime;  // 0x04
    DataID idOwner;  // 0x08
    TimerOwnerType eOwnerType;  // 0x0c
    void * lpData;  // 0x10
    TimerFunc pfunc;  // 0x18
};

struct TimersGlobals {  // size 0x10
    TDataArray<Timer, int> * ptTimers;  // 0x00
    BOOL bCurrentlyUpdating;  // 0x08
};

DataID TimerTryToGetOwner(const TimerID idTimer);
TimerOwnerType TimerGetOwnerType(const TimerID idTimer);





















void TimersInitializeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    TimersGlobals ** ptTimersGlobals = &tSGame.ptTimersGlobals;


    *ptTimersGlobals = (TimersGlobals *)SigmaMemoryNew(sizeof(TimersGlobals), 4, tSGame.ptMemoryInterface, TRUE);

    (*ptTimersGlobals)->ptTimers = TDataArrayCreateTracked<Timer, int>("Timers", tSGame.ptMemoryInterface, 0x2000);
    (*ptTimersGlobals)->bCurrentlyUpdating = FALSE;

}




void TimersDisposeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    TimersGlobals ** ptTimersGlobals = &tSGame.ptTimersGlobals;


    TDataArrayDispose((*ptTimersGlobals)->ptTimers);

    SigmaMemoryFree(*ptTimersGlobals, tSGame.ptMemoryInterface); *ptTimersGlobals = NULL;
}




static Timer * sTimerGet(const TimerID idTimer)
{
    SGame & tSGame = SGameManager::GameGet();

    TimersGlobals * ptTimersGlobals = tSGame.ptTimersGlobals;
    return ptTimersGlobals->ptTimers->ElementGet(idTimer);
}






















































void TimersDumpList()
{
    SGame & tSGame = SGameManager::GameGet();

    TimersGlobals * ptTimersGlobals = tSGame.ptTimersGlobals;
    int32 nCount = 0; uint32 dwNow = GameGetServerTime();


    TDataArrayIterator<Timer> tIt;



    while (ptTimersGlobals->ptTimers->Iterate(tIt))
    {
        nCount++;
    }
}










TimerID TimerRegister(DataID idOwner,
                      TimerOwnerType eOwnerType,
                      uint32 dwDelay,
                      void * lpData,
                      TimerFunc pfunc)

{
    SGame & tSGame = SGameManager::GameGet();
    TimersGlobals * ptTimersGlobals = tSGame.ptTimersGlobals;

    TimerID idTimer = ptTimersGlobals->ptTimers->ElementNew();
    if (idTimer == TIMERID_INVALID)
    {

        TimersDumpList();


        XASSERT(FALSE);
    }






    uint32 dwNow; Timer * ptTimer = sTimerGet(idTimer);

    dwNow = GameGetServerTime();

    ptTimer->dwTime = dwNow + dwDelay;
    ptTimer->idOwner = idOwner;
    ptTimer->eOwnerType = eOwnerType;
    ptTimer->lpData = lpData;
    ptTimer->pfunc = pfunc;




    if (ptTimer->dwTime < dwNow)
    {
        ptTimer->dwTime = dwNow;
    }

    if (idOwner != DATAID_INVALID)
    {
        ActorCommonData * ptACDOwner = NULL;

        if (eOwnerType == TIMER_OWNER_ACD)
        {
            ptACDOwner = &ACDGet((ACDID)idOwner);
        }
        else if (eOwnerType == TIMER_OWNER_ACTOR)
        {








            Actor * ptActor = ActorGet((ActorID)idOwner);
            ptACDOwner = &ACDGet(ptActor->idACD);
        }






        if (ptACDOwner)
        {
            ptACDOwner->dwTimerCount++;
        }
    }


    return idTimer;
}



BOOL TimerExists(TimerID idTimer)
{
    if (idTimer == TIMERID_INVALID)
        return FALSE;

    SGame & tSGame = SGameManager::GameGet();



    TimersGlobals * ptTimersGlobals = tSGame.ptTimersGlobals;

    if (ptTimersGlobals == NULL)
        return FALSE;



    Timer * ptTimer = ptTimersGlobals->ptTimers->ElementTryToGet(idTimer);


    return ptTimer != NULL;
}



void TimerUnregister(TimerID idTimer)
{
    if (!TimerExists(idTimer))
        return;

    SGame & tSGame = SGameManager::GameGet();



    TimersGlobals * ptTimersGlobals = tSGame.ptTimersGlobals;

    DataID idOwner = TimerTryToGetOwner(idTimer);
    ActorCommonData * ptACDOwner = NULL;





    switch (TimerGetOwnerType(idTimer))
    {
    case TIMER_OWNER_ACD:
        ptACDOwner = ACDTryToGet((ACDID)idOwner);
        break;

    case TIMER_OWNER_ACTOR:
        {


            Actor * ptActor = ActorTryToGet((ActorID)idOwner);
            if (ptActor)
            {
                ptACDOwner = ACDTryToGet(ptActor->idACD);
            }
        }
        break;
    }





    if (ptACDOwner)
    {






        if (ptACDOwner->dwTimerCount > 0)
        {
            ptACDOwner->dwTimerCount--;
        }
    }






    ptTimersGlobals->ptTimers->ElementDeleteByID(idTimer);

}




void GameTimersUpdate()
{
    SGame & tSGame = SGameManager::GameGet();
    TimersGlobals * ptTimersGlobals = tSGame.ptTimersGlobals;


    ptTimersGlobals->bCurrentlyUpdating = TRUE;
    uint32 dwTime = GameGetServerTime();

    TDataArrayIterator<Timer> tIt;


    while (ptTimersGlobals->ptTimers->Iterate(tIt))
    {
        Timer * ptTimer = tIt.ptElement;

        if (dwTime >= ptTimer->dwTime)
        {
            BOOL fCallFunc = TRUE;
            TimerID idTimer = ptTimer->id;


            if (ptTimer->eOwnerType == TIMER_OWNER_ACD)
            {
                fCallFunc = ACDTryToGet((ACDID)ptTimer->idOwner) != NULL;
            }





            if (fCallFunc)
            {
                ptTimer->pfunc(idTimer, ptTimer->idOwner, ptTimer->lpData);
            }

            TimerUnregister(idTimer);
        }
    }

    ptTimersGlobals->bCurrentlyUpdating = FALSE;
}



































DataID TimerTryToGetOwner(const TimerID idTimer)
{
    SGame & tSGame = SGameManager::GameGet();

    TimersGlobals * ptTimersGlobals = tSGame.ptTimersGlobals;
    Timer * ptTimer = ptTimersGlobals->ptTimers->ElementTryToGet(idTimer);

    return ptTimer ? ptTimer->idOwner : DATAID_INVALID;
}




TimerOwnerType TimerGetOwnerType(const TimerID idTimer)
{
    SGame & tSGame = SGameManager::GameGet();

    TimersGlobals * ptTimersGlobals = tSGame.ptTimersGlobals;
    Timer * ptTimer = ptTimersGlobals->ptTimers->ElementGet(idTimer);

    return ptTimer->eOwnerType;
}
















uint32 TimerGetTime(const TimerID idTimer)
{
    SGame & tSGame = SGameManager::GameGet();

    TimersGlobals * ptTimersGlobals = tSGame.ptTimersGlobals;
    Timer * ptTimer = ptTimersGlobals->ptTimers->ElementGet(idTimer);

    return ptTimer->dwTime;
}
