// Original file: X/Client/Network/NetStat.cpp

#include "NetStat.h"
#include "../../Common/Game.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/SigmaTiming.h"
#include "../CGame.h"
























void NetStatInit(NetStat & tNetStat)
{
    tNetStat.bTrackingEnabled = FALSE;

    tNetStat.dwLastUpdateTick = GetMachineTicks();
    tNetStat.dwTrackingStartTimeTick = 0;
    tNetStat.dwTrackingEndTimeTick = 0;

    SigmaMemoryClear(&tNetStat.tTransmit, sizeof(tNetStat.tTransmit));
    SigmaMemoryClear(&tNetStat.tReceived, sizeof(tNetStat.tReceived));

    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    // The DWARF records the no-argument Initialize() here, but the binary stores a block count of 256
    // where that overload's default is 10 everywhere else; the two-argument overload reproduces it.



    tNetStat.poolMsgHistoryData.Initialize(256, NULL);
    tNetStat.poolMsgHistoryData.SetMemoryInterface(ptMemoryInterface);

    tNetStat.tkbpsClient.listMsgHistory.SetPool(&tNetStat.poolMsgHistoryData);
    tNetStat.tkbpsServer.listMsgHistory.SetPool(&tNetStat.poolMsgHistoryData);
}



void NetStatDispose(NetStat & tNetStat)
{

    tNetStat.tkbpsServer.listMsgHistory.Dispose();
    tNetStat.tkbpsClient.listMsgHistory.Dispose();
    tNetStat.poolMsgHistoryData.Dispose();
}



void NetStatAddData(NetStat & tNetStat,
                    uint32 uBytesToAdd,
                    NetStatData eType,
                    uint8 * pMessage)
{
    GameMessageType eMsgType = 0;

    if (pMessage)
    {
        RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pMessage;
        eMsgType = ptHeader->eType;
    }

    NetStatAddData(tNetStat, uBytesToAdd, eType, eMsgType);
}



void NetStatAddData(NetStat & tNetStat,
                    uint32 uBytesToAdd,
                    NetStatData eType,
                    GameMessageType eMsgType)
{
    TMsgHistoryDataList * ptlistMessages = NULL;




    switch (eType)
    {
    case NS_TRANSMIT:
        tNetStat.tTransmit.nShortTermBytes += uBytesToAdd;
        tNetStat.tTransmit.nShortTermNumMessages++;
        tNetStat.tTransmit.dwTotalBytes += uBytesToAdd;
        tNetStat.tTransmit.dwTotalPackets++;
        ptlistMessages = &tNetStat.tkbpsClient.listMsgHistory;
        break;

    case NS_RECEIVED:
        tNetStat.tReceived.nShortTermBytes += uBytesToAdd;
        tNetStat.tReceived.nShortTermNumMessages++;
        tNetStat.tReceived.dwTotalBytes += uBytesToAdd;
        tNetStat.tReceived.dwTotalPackets++;
        ptlistMessages = &tNetStat.tkbpsServer.listMsgHistory;
        break;
    }



    if (ptlistMessages && ptlistMessages->Count() < 10240)
    {
        MsgHistoryData tHistoryData;
        tHistoryData.eMsgType = eMsgType;




        tHistoryData.uSize = uBytesToAdd;
        tHistoryData.uTime = CGameGetClientTicks();
        ptlistMessages->AddTail_Tracked(tHistoryData);
    }
}




































































































































NetworkData * sGetData(NetStat & tNetStat, NetStatData eType)
{


    switch (eType)
    {
    case NS_TRANSMIT:
        return &tNetStat.tTransmit;

    case NS_RECEIVED:
        return &tNetStat.tReceived;
    }
    return NULL;
}

























































void NetStatAddGameMsg(NetStat & tNetStat,
                       NetStatData eType,
                       GameMessageType eMsgType,
                       uint32 dwMsgBitCount)
{
    NetworkData * ptData = sGetData(tNetStat, eType);






    XASSERT(ptData);


    ptData->atMsgData[eMsgType].dwCount++;
    ptData->atMsgData[eMsgType].dwBitCount += dwMsgBitCount;
}











































































































































































































































































void kbpsData::Clear()
{

    listMsgHistory.RemoveAll();
    flkbpsTotal = 0.0f;
    ukbpsEntries = 0;
    uNextClientTime = 0;
}


































void NetStatClearList()
{

    NetStat * ptNetStat = CGameGetNetStat();
    if (ptNetStat)
    {
        ptNetStat->tkbpsClient.Clear();
        ptNetStat->tkbpsServer.Clear();
    }
}
