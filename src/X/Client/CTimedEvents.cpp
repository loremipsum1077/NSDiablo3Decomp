// Original file: X/Client/CTimedEvents.cpp

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/TimedEvents.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/Math/MathGeneral.h"
#include "CGame.h"










struct CTimedEventGlobals {  // size 0x58, line 18
    ActiveEventList tActiveEventList;  // 0x00
};
static_assert(sizeof(CTimedEventGlobals) == 0x58, "CTimedEventGlobals");




void CTimedEventsInitializeForGame()  // line 26

{
    CGame & tCGame = CGameGet();
    tCGame.ptCTimedEventGlobals = new CTimedEventGlobals;
}





void CTimedEventsDisposeForGame()  // line 36

{
    CGame & tCGame = CGameGet();

    delete tCGame.ptCTimedEventGlobals; tCGame.ptCTimedEventGlobals = NULL;
}




void CTimedEventStart(const ActiveEvent & tEvent)  // line 47

{
    CTimedEventGlobals * ptCTimedEventGlobals = CGameGet().ptCTimedEventGlobals;

    ActiveEvent tEventToAdd = tEvent;
    ptCTimedEventGlobals->tActiveEventList.AddTail_Tracked(tEventToAdd);
}






void CTimedEventModified(const ActiveEvent & tModifiedEvent)  // line 60

{
    CTimedEventGlobals * ptCTimedEventGlobals = CGameGet().ptCTimedEventGlobals;

    POS pos = ptCTimedEventGlobals->tActiveEventList.GetHeadPosition();
    while (pos)
    {
        ActiveEvent & tEvent = ptCTimedEventGlobals->tActiveEventList.GetNext(pos);
        if (tEvent.snoTimedEvent == tModifiedEvent.snoTimedEvent)
        {
            tEvent.dwArtificiallyElapsedTime = tModifiedEvent.dwArtificiallyElapsedTime;
        }
    }
}






void CTimedEventFinished(const SNO snoTimedEvent)  // line 80

{
    CTimedEventGlobals * ptCTimedEventGlobals = CGameGet().ptCTimedEventGlobals;

    POS pos = ptCTimedEventGlobals->tActiveEventList.GetHeadPosition();
    while (pos)
    {
        POS posPrev = pos;
        ActiveEvent & tEvent = ptCTimedEventGlobals->tActiveEventList.GetNext(pos);

        if (tEvent.snoTimedEvent == snoTimedEvent)
        {
            ptCTimedEventGlobals->tActiveEventList.RemoveAt(posPrev);
        }
    }
}






ActiveEvent * CTimedEventsGetActiveEvent(const SNO snoTimedEvent)  // line 101

{
    CTimedEventGlobals * ptCTimedEventGlobals = CGameGet().ptCTimedEventGlobals;
    POS pos = ptCTimedEventGlobals->tActiveEventList.GetHeadPosition();
    while (pos)
    {
        ActiveEvent & tEvent = ptCTimedEventGlobals->tActiveEventList.GetNext(pos);

        if (tEvent.snoTimedEvent == snoTimedEvent)
        {
            return &tEvent;
        }
    }

    return NULL;
}

















int32 CTimedEventGetRemainingTime(const SNO snoTimedEvent)  // line 137

{
    ActiveEvent * ptEvent = CTimedEventsGetActiveEvent(snoTimedEvent);
    if (ptEvent == NULL)
    {
        return 0;
    }



    uint32 dwNow = CGameGetPredictedServerTime();
    int32 nRemainingTicks = ptEvent->dwExpirationTime - dwNow - ptEvent->dwArtificiallyElapsedTime;


    return Math::Max(nRemainingTicks, 0);


}



















uint32 CTimedEventGetDurationTicks(const SNO snoTimedEvent)  // line 173

{
    ActiveEvent * ptEvent = CTimedEventsGetActiveEvent(snoTimedEvent);
    if (ptEvent)
    {


        if (ptEvent->dwStartTime > ptEvent->dwExpirationTime)
            return 0;
        return ptEvent->dwExpirationTime - ptEvent->dwStartTime;
    }



    return 0;
}
