// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Network/NetStat.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/SigmaTypes.h"

enum {
    NETSTAT_HISTORY = 20,
};

enum NetStatData {
    NS_TRANSMIT = 0,
    NS_RECEIVED = 1,
};

struct MsgTrackingData {  // size 0x8
    uint32 dwCount;  // 0x00
    uint32 dwBitCount;  // 0x04
};
static_assert(sizeof(MsgTrackingData) == 0x8, "MsgTrackingData");

struct NetworkData {  // size 0x1354
    int32 nShortTermBytes;  // 0x00
    int32 nShortTermNumMessages;  // 0x04
    float flTransferRate[20];  // 0x08
    int32 nMessageSize[20];  // 0x58
    int32 nCurrent;  // 0xa8
    uint32 dwTotalBytes;  // 0xac
    uint32 dwTotalPackets;  // 0xb0
    MsgTrackingData atMsgData[596];  // 0xb4 (MSG_NUM_MESSAGES)
};
static_assert(sizeof(NetworkData) == 0x1354, "NetworkData");

struct MsgHistoryData {  // size 0xc
    GameMessageType eMsgType;  // 0x00
    uint32 uTime;  // 0x04
    uint32 uSize;  // 0x08
};
static_assert(sizeof(MsgHistoryData) == 0xc, "MsgHistoryData");

typedef XListMemoryPool<MsgHistoryData> TMsgHistoryDataPool;

typedef XPooledList<MsgHistoryData, const MsgHistoryData &> TMsgHistoryDataList;

struct kbpsData {  // size 0x38
    TMsgHistoryDataList listMsgHistory;  // 0x00
    float flkbpsTotal;  // 0x28
    uint32 ukbpsEntries;  // 0x2c
    uint32 uNextClientTime;  // 0x30
    void Clear();
    float GetAvg();
    void Addkbps(float kbps);
};
static_assert(sizeof(kbpsData) == 0x38, "kbpsData");

struct NetStat {  // size 0x2758
    TMsgHistoryDataPool poolMsgHistoryData;  // 0x00
    kbpsData tkbpsClient;  // 0x30
    kbpsData tkbpsServer;  // 0x68
    BOOL bTrackingEnabled;  // 0xa0
    uint32 dwLastUpdateTick;  // 0xa4
    uint32 dwTrackingStartTimeTick;  // 0xa8
    uint32 dwTrackingEndTimeTick;  // 0xac
    NetworkData tTransmit;  // 0xb0
    NetworkData tReceived;  // 0x1404
};
static_assert(sizeof(NetStat) == 0x2758, "NetStat");

// NetStat.cpp:33 and :59 (X/Common/Game.cpp sCGameInit / sCGameDispose).
void NetStatInit(NetStat & tNetStat);
void NetStatDispose(NetStat & tNetStat);
// NetStat.cpp:69, :87, :334 and :661.
void NetStatAddData(NetStat & tNetStat, uint32 uBytesToAdd, NetStatData eType, uint8 * pMessage);
void NetStatAddData(NetStat & tNetStat, uint32 uBytesToAdd, NetStatData eType, GameMessageType eMsgType);
void NetStatAddGameMsg(NetStat & tNetStat, NetStatData eType, GameMessageType eMsgType, uint32 dwMsgBitCount);
void NetStatClearList();
