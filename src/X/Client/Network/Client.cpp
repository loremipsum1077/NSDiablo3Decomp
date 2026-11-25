// X/Client/Network/Client.cpp
// The client end of the game connection: the socket, the outgoing/incoming packet queues and
// the latency simulator.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/SNO/SNOName.h"  // original line table: SNOName.h, WorldPlace.h, GfxVB.h precede Client.cpp
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Networking/Network.h"
#include "../../Common/Networking/XSocket.h"
#include "../../Sigma/DataStructures/XRingBuffer.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Networking/XNetAddress.h"

#include "XClientSocket.h"
namespace Console { namespace GameNetwork { class Connection; } }







BOOL XClientSocketIsConnected(XClientSocket & tSocket);
void XClientSocketWriteUpdate(XClientSocket & tSocket);
void XClientSocketDispose(XClientSocket & tSocket);
typedef void (*ClientConnectedCallback)();
struct PacketPool;
void PacketBufferListFreeAll(PacketPool * ptPool, TPacketBufferList & tList);
PacketBuffer * PacketBufferAlloc(PacketPool * ptPool, uint32 dwSize);
void ClientMessageDisposeForGame();
PacketPool * PacketPoolCreate(XMemoryAllocatorInterface * ptMemoryInterface);
void CTrickleInitializeForApp();
BOOL XClientSocketConnect(XClientSocket & tSocket, const XNetAddress & tAddress);
void CNetworkSeasonEnded();
void XClientSocketUpdate(XClientSocket & tSocket);
void CNetworkChallengeRiftsDisabled();
void CNetworkClientDisconnected();
void ClientMessageInitializeForGame();
void ClientDisconnectUpcall();

struct EndOfTickMessage {  // size 0x10 (DWARF: X/Client/Network/Client.h:21, header not in src yet)
    RequiredMessageHeader tHeader;  // 0x00
    uint32 dwTickEnding;  // 0x08
    uint32 dwTickNext;  // 0x0c
};
void ClientMessageReceiveAllMessage(void * pMessage);
struct LatencyMessageHeader { uint32 dwAppTimeToForward; };  // DWARF: Client.cpp:50, member at :52
BOOL ClientMessageReceiveGameMessage(void * pMessage);
void CTrickleDisposeForApp();
void PacketPoolDispose(PacketPool *& ptPool, XMemoryAllocatorInterface * ptMemoryInterface);

class LatencySimulator {  // size 0x40
public:
    void DisposeForGame();
    void UpdateDirection();
    void ForwardPackets(BOOL bOutgoing);
    void QueueMessage(void * pMessage);
    void Dispose();


















    TPacketBufferList m_tPacketBufferList;  // 0x00

    int32 m_nCurrentLatency;  // 0x28

    float m_flNormalLatencyMean;  // 0x2c
    float m_flNormalLatencyDistribution;  // 0x30

    float m_flSpikeLatencyChance;  // 0x34
    float m_flSpikeLatencyMean;  // 0x38
    float m_flSpikeLatencyDistribution;  // 0x3c
};



struct ClientGlobals {  // size 0x1b8

    XClientSocket tClientSocket;  // 0x00
    BOOL bInitialized;  // 0x30
    BOOL bDisposing;  // 0x34
    BOOL bConnecting;  // 0x38
    BOOL bSeasonEnded;  // 0x3c
    BOOL bChallengeRiftsDisabled;  // 0x40

    ClientConnectedCallback pfnClientConnectedCB;  // 0x48


    TPacketBufferList tOutgoingPacketBufferList;  // 0x50
    TPacketBufferList tIncomingPacketBufferList;  // 0x78

    PacketPool * ptPacketPool;  // 0xa0
    TPacketListPool tPacketListPool;  // 0xa8
    XRingBuffer<int, int, 10> tPreviousMessageIds;  // 0xd8

    POS posLastHeartBeat;  // 0x128

    BOOL bLatencySimulatorActivated;  // 0x130
    LatencySimulator tOutgoingLatencySimulator;  // 0x138
    LatencySimulator tIncomingLatencySimulator;  // 0x178
};

#include "../../Sigma/Math/MathGeneral.h"
static ClientGlobals * sgptClientGlobals;
#include "../../Sigma/Networking/XEncoder.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../CGame.h"
#include "NetStat.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/Networking/XDecoder.h"
// Declared in headers not adopted yet.
extern const XTypeDescriptor * DT_REQUIRED_MESSAGE_HEADER;
uint8 * NetworkGetDecodeBuffer();
uint8 * NetworkGetEncodeBuffer();
uint32 NetworkGetProtocolHash();
uint32 ClientMessageReceiveGetServerProtocolHash();
XDecoder::Result DecodeMessage(GameMessageType eMessageType, void * pDecodeToBuffer, uint32 dwBufferSize, XDecoder & tDecoder);
BOOL GetGameMessageTypeAndSize(GameMessageType eMessageType, const XTypeDescriptor *& pTypeDescriptor, uint32 & dwSize, const char *& pszMessageLabel);
void NetStatAddGameMsg(NetStat & tNetStat, NetStatData eData, GameMessageType eMessageType, uint32 dwBitCount);
void NetStatAddData(NetStat & tNetStat, uint32 dwBytes, NetStatData eData, GameMessageType eMessageType);
void ClientMessageReceiveUpcall(uint8 * pMessage);
BOOL XClientSocketInit(XClientSocket & tSocket, intptr idUpcallHandle, SocketMessageHandlerProc pfnMsgHandler);
BOOL XClientSocketSendBuffer(XClientSocket & tSocket, void * pBuffer, uint32 dwSize);
BOOL EncodeMessage(void * pMessage, XEncoder & tEncoder);
BOOL NetworkParseGameDataBlock(intptr idUpcallHandle, SocketReceivedData & tData, NetworkGameDataBlockHandlerProc pfnHandler);
void ClientConnectUpcall();
void NetStatAddData(NetStat & tNetStat, uint32 dwBytes, NetStatData eData, uint8 * pMessage);
uint32 XClientSocketGetMaxSendBufferSize(XClientSocket & tSocket);
void CNetworkWriteUpdate();
void PacketBufferFree(PacketPool * ptPool, PacketBuffer *& ptPacketBuffer);
// DWARF: EndOfTickMessage_Fields at Client.cpp:125, TypeDescriptor_EndOfTickMessage and DT_END_OF_TICK_MESSAGE at :129
static const TypeDescriptorField EndOfTickMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(EndOfTickMessage, tHeader), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(EndOfTickMessage, dwTickEnding), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(EndOfTickMessage, dwTickNext), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(EndOfTickMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_EndOfTickMessage("EndOfTickMessage", 0, EndOfTickMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_END_OF_TICK_MESSAGE = &TypeDescriptor_EndOfTickMessage;
void LatencySimulator::Dispose()
{

    PacketBufferListFreeAll(sgptClientGlobals->ptPacketPool, m_tPacketBufferList);
    m_tPacketBufferList.Dispose();
}






inline void LatencySimulator::DisposeForGame()
{

    PacketBufferListFreeAll(sgptClientGlobals->ptPacketPool, m_tPacketBufferList);

    m_nCurrentLatency = 0; m_flNormalLatencyMean = 0.0f; m_flNormalLatencyDistribution = 0.0f; m_flSpikeLatencyChance = 0.0f; m_flSpikeLatencyMean = 0.0f; m_flSpikeLatencyDistribution = 0.0f;
}
























static BOOL sClientSendAnOutgoingMessage(uint8 * pMessage, XEncoder & tEncoder);


void LatencySimulator::UpdateDirection()
{
    RNG & rng = CGameGlobalsGet().rngLocal;
    float flLatency;



    float flSpikeRand = RandFloat_Internal(rng);
    if (m_flSpikeLatencyChance > 0.000001f && m_flSpikeLatencyChance > flSpikeRand)
    {
        float flMinSpike = m_flSpikeLatencyMean - m_flSpikeLatencyDistribution;
        float flRangeSpike = m_flSpikeLatencyDistribution * 2.0f;
        flLatency = RandRangeFloat_Internal(rng, flMinSpike, flRangeSpike);
    }
    else
    {
        float flMinNormal = m_flNormalLatencyMean - m_flNormalLatencyDistribution;
        float flRangeNormal = m_flNormalLatencyDistribution * 2.0f;


        flLatency = RandRangeFloat_Internal(rng, flMinNormal, flRangeNormal);
    }

    m_nCurrentLatency = Math::FloatRoundToInt(flLatency);
}




void LatencySimulator::ForwardPackets(const BOOL bOutgoing)
{
    uint32 dwAppTime = AppGetTicks();


    XEncoder tEncoder;
    uint32 dwEncodedBytes;
    if (GameIsNetworked())
    {
        uint32 dwSocketRoom = XClientSocketGetMaxSendBufferSize(sgptClientGlobals->tClientSocket);
        if (dwSocketRoom < 5)
            return;


        dwSocketRoom = Math::Min(dwSocketRoom, (uint32)0x8000);
        tEncoder.SetData(NetworkGetEncodeBuffer(), dwSocketRoom);
        tEncoder.SetUsed(32);
    }

    while (!m_tPacketBufferList.IsEmpty())
    {
        PacketBuffer * ptPacketBuffer = m_tPacketBufferList.GetHead();
        RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)ptPacketBuffer->byteData;
        LatencyMessageHeader & tLatencyHeader = *(LatencyMessageHeader *)(ptPacketBuffer->byteData + ptHeader->dwSize);
        if (tLatencyHeader.dwAppTimeToForward > dwAppTime)
            break;
        if (bOutgoing)
        {
            if (!sClientSendAnOutgoingMessage(ptPacketBuffer->byteData, tEncoder))
                break;
        }
        else
        {
            ClientMessageReceiveAllMessage(ptPacketBuffer->byteData);
        }











        m_tPacketBufferList.RemoveHead();
        PacketBufferFree(sgptClientGlobals->ptPacketPool, ptPacketBuffer);
    }

    dwEncodedBytes = tEncoder.UsedBytes();
    if (dwEncodedBytes > sizeof(uint32))
    {
        tEncoder.SetUsed(0);
        tEncoder.Put(dwEncodedBytes, 32);

        BOOL bResult = XClientSocketSendBuffer(sgptClientGlobals->tClientSocket, tEncoder.Data(), dwEncodedBytes);
    }
}





inline void LatencySimulator::QueueMessage(
    void * pMessage)
{
    uint32 dwAppTime = AppGetTicks();

    RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pMessage;



    uint32 dwTotalSize = ptHeader->dwSize + sizeof(uint32);

    PacketBuffer * ptPacketBuffer = PacketBufferAlloc(sgptClientGlobals->ptPacketPool, dwTotalSize);

    LatencyMessageHeader * ptLatencyHeader = (LatencyMessageHeader *)(ptPacketBuffer->byteData + ptHeader->dwSize);
    uint32 dwAppTimeToForward = dwAppTime + m_nCurrentLatency;
    ptLatencyHeader->dwAppTimeToForward = dwAppTimeToForward;

    SigmaMemoryCopy(ptPacketBuffer->byteData, pMessage, ptHeader->dwSize);

    m_tPacketBufferList.AddTail_Tracked(ptPacketBuffer);
}











static void sTracePreviousMessageIds()
{
    for (size_t i = 0; i < sgptClientGlobals->tPreviousMessageIds.Count(); i++)
    {
        const XTypeDescriptor * pTypeDescriptor = NULL;
        uint32 dwSize = 0;
        const char * pszMessageLabel = "MSG_INVALID -- FRAME BREAK";
        GameMessageType eType = sgptClientGlobals->tPreviousMessageIds[i];
        if (eType != 0)
            GetGameMessageTypeAndSize(eType, pTypeDescriptor, dwSize, pszMessageLabel);
    }
}












static BOOL sClientSocketParseMsgBlock(intptr idUpcallHandle, uint8 * pMessage, uint32 dwMessageSize, BOOL & bContinueParsing)
{
    uint8 * pauDecodedMessage = NetworkGetDecodeBuffer();
    XDecoder tDecoder(pMessage, dwMessageSize);
    sgptClientGlobals->tPreviousMessageIds.Enqueue(0);
    while (tDecoder.Used() < tDecoder.Size())
    {
        uint32 dwUsedBits = tDecoder.Used();
        RequiredMessageHeader tHeader;
        TypeDescriptorField field(DT_REQUIRED_MESSAGE_HEADER, NULL);
        XDecoder::Result eResult = DT_REQUIRED_MESSAGE_HEADER->Decode(&tHeader, field, tDecoder);
        if (eResult == XDecoder::RESULT_TRUNCATED || (eResult != XDecoder::RESULT_CORRUPTED && tHeader.eType == 0))
        {
            if (ClientMessageReceiveGetServerProtocolHash() == NetworkGetProtocolHash() && tDecoder.Size() - dwUsedBits >= 8)
                sTracePreviousMessageIds();
            return TRUE;
        }
        if (eResult == XDecoder::RESULT_CORRUPTED)
        {
            sTracePreviousMessageIds();
            return FALSE;
        }
        tDecoder.SetUsed(dwUsedBits);
        if (DecodeMessage(tHeader.eType, pauDecodedMessage, 0x8000, tDecoder) != XDecoder::RESULT_VALID)
        {
            sTracePreviousMessageIds();
            return FALSE;
        }
        if (sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_NET_COLLECT_DATA_BIT))
        {
            NetStat * ptNetStat = CGameGetNetStat();
            uint32 uUsedBits = tDecoder.Used() - dwUsedBits;
            NetStatAddGameMsg(*ptNetStat, NS_RECEIVED, tHeader.eType, uUsedBits);
            NetStatAddData(*ptNetStat, uUsedBits >> 3, NS_RECEIVED, tHeader.eType);
        }
        ClientMessageReceiveUpcall(pauDecodedMessage);
        XClientSocket * ptClientSocket = (XClientSocket *)idUpcallHandle;
        if (!XClientSocketIsConnected(*ptClientSocket))
        {
            bContinueParsing = FALSE;
            return TRUE;
        }
        sgptClientGlobals->tPreviousMessageIds.Enqueue(tHeader.eType);
    }
    return TRUE;
}


























































static void sSendKeepAlive(XClientSocket & tSocket)
{
    RequiredMessageHeader tHeader;
    tHeader.eType = 0xd9;
    tHeader.dwSize = sizeof(tHeader);
    tHeader.nControllerIndex = -1;

    XEncoder tEncoder(NetworkGetEncodeBuffer(), 0x8000);
    tEncoder.SetUsed(32);
    EncodeMessage(&tHeader, tEncoder);
    uint32 dwBytesUsed = tEncoder.UsedBytes();
    tEncoder.SetUsed(0);
    tEncoder.Put(dwBytesUsed, 32);

    XClientSocketSendBuffer(tSocket, tEncoder.Data(), dwBytesUsed);
}





static BOOL sClientSocketMsgHandler(intptr idUpcallHandle, SocketEvent eEvent, void * pParam)
{
    XClientSocket * ptClientSocket = (XClientSocket *)idUpcallHandle;

    switch (eEvent)
    {
    case SOCKETEVENT_CONNECTED:
        {

            ClientConnectUpcall();
        }
        break;
    case SOCKETEVENT_DISCONNECTED:
        ClientDisconnectUpcall();
        break;
    case SOCKETEVENT_RECEIVED_DATA:
        {
            SocketReceivedData * ptSocketData = (SocketReceivedData *)pParam;
            if (!NetworkParseGameDataBlock(idUpcallHandle, *ptSocketData, sClientSocketParseMsgBlock))
                return FALSE;
        }
        break;
    case SOCKETEVENT_SEND_KEEPALIVE:
        {







            sSendKeepAlive(*ptClientSocket);
        }
        break;
    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);

    }
    return TRUE;
}



static BOOL sClientEncodeMessage(void * pMessage, XEncoder & tEncoder)
{
    if (sgptClientGlobals->bInitialized)
    {
        return EncodeMessage(pMessage, tEncoder);
    }
    else
    {


        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        return FALSE;
    }
}









static BOOL sClientMessageForceRouteToSocket(uint8 * pMessage)
{
    RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pMessage;
    return ptHeader->eType == 0xe;
}












static BOOL sClientSendAnOutgoingMessage(uint8 * pMessage, XEncoder & tEncoder)
{
    uint32 uBytesEncoded;






    if (GameIsNetworked() || sClientMessageForceRouteToSocket(pMessage))
    {
        if (!sClientEncodeMessage(pMessage, tEncoder))
            return FALSE;




        uBytesEncoded = tEncoder.UsedBytes();
    }
    else
    {


        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }



















    if ((sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_NET_COLLECT_DATA_BIT)) && uBytesEncoded)
    {
        NetStat * ptNetStat = CGameGetNetStat();
        NetStatAddData(*ptNetStat, uBytesEncoded, NS_TRANSMIT, pMessage);
    }
    return TRUE;

}



































static BOOL sLatencySimulatorIsActive()
{

    return sgptClientGlobals->bLatencySimulatorActivated;
}




static void sLatencySimulatorUpdate()
{

    sgptClientGlobals->tOutgoingLatencySimulator.UpdateDirection();
    sgptClientGlobals->tIncomingLatencySimulator.UpdateDirection();

    sgptClientGlobals->tOutgoingLatencySimulator.ForwardPackets(TRUE);
    sgptClientGlobals->tIncomingLatencySimulator.ForwardPackets(FALSE);
}




inline void ClientConnectUpcall()
{
    sgptClientGlobals->bConnecting = FALSE;
    sgptClientGlobals->pfnClientConnectedCB();
}








void ClientInitializeForApp()

{
    sgptClientGlobals = (ClientGlobals *)SigmaMemoryNew(sizeof(ClientGlobals), 4, NULL, TRUE);

    sgptClientGlobals->ptPacketPool = PacketPoolCreate(NULL);
    sgptClientGlobals->tPacketListPool.Initialize();

    sgptClientGlobals->tOutgoingPacketBufferList.Initialize(&sgptClientGlobals->tPacketListPool);
    sgptClientGlobals->tIncomingPacketBufferList.Initialize(&sgptClientGlobals->tPacketListPool);

    new (&sgptClientGlobals->tPreviousMessageIds) XRingBuffer<int, int, 10>(TRUE);

    CTrickleInitializeForApp();
}




void ClientDisposeForApp()
{
    CTrickleDisposeForApp();

    PacketBufferListFreeAll(sgptClientGlobals->ptPacketPool, sgptClientGlobals->tIncomingPacketBufferList);
    PacketBufferListFreeAll(sgptClientGlobals->ptPacketPool, sgptClientGlobals->tOutgoingPacketBufferList);



    sgptClientGlobals->tIncomingPacketBufferList.Dispose();
    sgptClientGlobals->tOutgoingPacketBufferList.Dispose();

    if (sgptClientGlobals->bLatencySimulatorActivated)
    {
        sgptClientGlobals->tOutgoingLatencySimulator.Dispose();
        sgptClientGlobals->tIncomingLatencySimulator.Dispose();
    }

    PacketPoolDispose(sgptClientGlobals->ptPacketPool, NULL);
    sgptClientGlobals->tPacketListPool.Dispose();

    SigmaMemoryFree(sgptClientGlobals, NULL);
    sgptClientGlobals = NULL;
}



void ClientInitializeForGame()
{
    if (sgptClientGlobals->bInitialized)
        return;

    if (!XClientSocketInit(sgptClientGlobals->tClientSocket, (intptr)sgptClientGlobals, sClientSocketMsgHandler))
    {
        DisplayInternalError(NULL, "", 738, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        return;
    }

    sgptClientGlobals->bInitialized = TRUE;
    sgptClientGlobals->bDisposing = FALSE;
    sgptClientGlobals->bConnecting = FALSE;
    sgptClientGlobals->bSeasonEnded = FALSE;
}









void ClientDisposeForGame()
{
    if (!sgptClientGlobals || !sgptClientGlobals->bInitialized)
    {
        return;
    }





    ClientMessageDisposeForGame();
    ClientMessageInitializeForGame();

    sgptClientGlobals->bDisposing = TRUE;

    XClientSocketDispose(sgptClientGlobals->tClientSocket);

    sgptClientGlobals->bInitialized = FALSE;
    sgptClientGlobals->bSeasonEnded = FALSE;
}




void ClientConnect(const XNetAddress & tAddress, ClientConnectedCallback pfnClientConnectedCB)


{



    sgptClientGlobals->bConnecting = TRUE;
    sgptClientGlobals->pfnClientConnectedCB = pfnClientConnectedCB;

    if (!XClientSocketConnect(sgptClientGlobals->tClientSocket, tAddress))
    {


        ClientDisconnectUpcall();
    }
}




inline void ClientDisconnectUpcall()
{

    if (!sgptClientGlobals->bInitialized ||
        sgptClientGlobals->bDisposing)
    {
        return;
    }

    if (sgptClientGlobals->bSeasonEnded)
    {
        CNetworkSeasonEnded();
    }

    else if (sgptClientGlobals->bChallengeRiftsDisabled)
    {
        CNetworkChallengeRiftsDisabled();
    }
    else
    {
        sgptClientGlobals->bConnecting = FALSE;


        CNetworkClientDisconnected();
    }
}

























inline BOOL ClientIsConnected()
{

    if (!sgptClientGlobals->bInitialized)
    {
        return FALSE;
    }

    return XClientSocketIsConnected(sgptClientGlobals->tClientSocket);
}











inline BOOL ClientIsConnecting()
{

    if (!sgptClientGlobals->bInitialized)
    {
        return FALSE;
    }

    return sgptClientGlobals->bConnecting;
}




void ClientMessageReceiveUpcall(uint8 * pMessage)
{

    if (sLatencySimulatorIsActive())
    {
        sgptClientGlobals->tIncomingLatencySimulator.QueueMessage(pMessage);
    }
    else
    {
        ClientMessageReceiveAllMessage(pMessage);
    }
}



void ClientMessageInitializeForGame()
{









    sgptClientGlobals->tPreviousMessageIds.Empty();

    sgptClientGlobals->posLastHeartBeat = NULL;
}




void ClientMessageDisposeForGame()
{

    TPacketBufferList & tOutgoingQueue = sgptClientGlobals->tOutgoingPacketBufferList;
    PacketBufferListFreeAll(sgptClientGlobals->ptPacketPool, tOutgoingQueue);

    TPacketBufferList & tIncomingQueue = sgptClientGlobals->tIncomingPacketBufferList;
    PacketBufferListFreeAll(sgptClientGlobals->ptPacketPool, tIncomingQueue);

    sgptClientGlobals->tOutgoingLatencySimulator.DisposeForGame();
    sgptClientGlobals->tIncomingLatencySimulator.DisposeForGame();
    sgptClientGlobals->tPreviousMessageIds.Empty();
}




void ClientSendOutgoingMessages()
{



    XEncoder tEncoder;
    if ( sgptClientGlobals->bInitialized && XClientSocketIsConnected(sgptClientGlobals->tClientSocket) )
    {
        uint32 dwSocketRoom = XClientSocketGetMaxSendBufferSize(sgptClientGlobals->tClientSocket);



        if ( dwSocketRoom < 5 )
        {
            CNetworkWriteUpdate();
            dwSocketRoom = XClientSocketGetMaxSendBufferSize(sgptClientGlobals->tClientSocket);
        }



        if ( dwSocketRoom < 5 )
            return;



        dwSocketRoom = Math::Min(dwSocketRoom, (uint32)0x8000);
        tEncoder.SetData(NetworkGetEncodeBuffer(), dwSocketRoom);
        tEncoder.SetUsed(32);
    }

    TPacketBufferList & tOutgoing = sgptClientGlobals->tOutgoingPacketBufferList;

    int32 nNumMessages = tOutgoing.Count();
    for (int32 i = 0; i < nNumMessages; i++)
    {
        PacketBuffer * ptPacketBuffer = tOutgoing.GetHead();

        if (sLatencySimulatorIsActive())
        {
            sgptClientGlobals->tOutgoingLatencySimulator.QueueMessage(ptPacketBuffer->byteData);
        }
        else
        {
            uint32 dwBitsUsedBeforeEncoding = tEncoder.Used();
            if (!sClientSendAnOutgoingMessage(ptPacketBuffer->byteData, tEncoder))
                break;




            if (sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_NET_COLLECT_DATA_BIT))
            {
                RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)ptPacketBuffer->byteData;
                NetStat * ptNetStat = CGameGetNetStat();
                NetStatAddGameMsg(*ptNetStat, NS_TRANSMIT, ptHeader->eType, (int32)tEncoder.Used() - dwBitsUsedBeforeEncoding);  // (int32) inferred: line-table column 987:100 for the subtraction
            }
        }

        tOutgoing.RemoveHead();
        PacketBufferFree(sgptClientGlobals->ptPacketPool, ptPacketBuffer);
    }

    uint32 dwEncodedBytes = tEncoder.UsedBytes();
    if (dwEncodedBytes > sizeof(uint32))
    {

        tEncoder.SetUsed(0);
        tEncoder.Put(dwEncodedBytes, 32);
        BOOL bResult = XClientSocketSendBuffer(sgptClientGlobals->tClientSocket, tEncoder.Data(), dwEncodedBytes);
    }






    CNetworkWriteUpdate();
}




void ClientReceiveHeartbeat(uint32 dwHeartBeatTime)
{
    TPacketBufferList & tIncoming = sgptClientGlobals->tIncomingPacketBufferList;
    POS posLastHeartBeat = sgptClientGlobals->posLastHeartBeat;
    if (posLastHeartBeat)
    {
        PacketBuffer * ptOldPacketBuffer = tIncoming.GetAt(posLastHeartBeat);
        EndOfTickMessage & tLastEndOfTick = *(EndOfTickMessage *)ptOldPacketBuffer->byteData;
        tLastEndOfTick.dwTickNext = dwHeartBeatTime;
        sgptClientGlobals->posLastHeartBeat = NULL;
    }

    PacketBuffer * ptPacketBuffer = PacketBufferAlloc(sgptClientGlobals->ptPacketPool, sizeof(EndOfTickMessage));
    EndOfTickMessage & tEndOfTickMessage = *(EndOfTickMessage *)ptPacketBuffer->byteData;
    tEndOfTickMessage.tHeader.eType = 0xa1;
    tEndOfTickMessage.tHeader.dwSize = sizeof(EndOfTickMessage);
    tEndOfTickMessage.tHeader.nControllerIndex = -1;
    tEndOfTickMessage.dwTickEnding = dwHeartBeatTime;
    tEndOfTickMessage.dwTickNext = 0xffffffff;






















    sgptClientGlobals->posLastHeartBeat = tIncoming.AddTail_Tracked(ptPacketBuffer);
}




void ClientProcessIncomingMessagesForCurrentTick()
{
    GameCommonData & tGCD = CGameGetGameCommonData();
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    TPacketBufferList & tIncoming = sgptClientGlobals->tIncomingPacketBufferList;
    uint32 dwMaxServerTime = tCGameGlobals.dwMaxGameTime;
    uint32 dwMaxServerTimeNow;
    uint32 dwCurServerTime;
    uint32 dwClientTime;

    int32 nNumMessages = tIncoming.Count();
    uint32 dwWantedServerTime = CGameGetPredictedServerTime() + tCGameGlobals.nClientLatencySmoothBufferTime;
    BOOL fSoftPaused = GameIsSoftPaused() || GameIsPaused();









    dwWantedServerTime = Math::Min(dwWantedServerTime, dwMaxServerTime);
    for (int32 i = 0; i < nNumMessages; i++)
    {

        PacketBuffer * ptPacketBuffer = tIncoming.GetHead();
        RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)ptPacketBuffer->byteData;
        if (ptHeader->eType == 0xa1)
        {
            EndOfTickMessage * ptEndOfTickMessage = (EndOfTickMessage *)ptHeader;
            int32 nTimeDiff = (int32)(dwWantedServerTime - ptEndOfTickMessage->dwTickEnding);
            if (nTimeDiff <= 0)
            {
                if (!fSoftPaused) break;
                sgptClientGlobals->posLastHeartBeat = NULL;





                tIncoming.RemoveHead();
                PacketBufferFree(sgptClientGlobals->ptPacketPool, ptPacketBuffer);
                continue;
            }
            if (fSoftPaused && sgptClientGlobals->posLastHeartBeat == tIncoming.GetHeadPosition()) sgptClientGlobals->posLastHeartBeat = NULL;
        }
















        tIncoming.RemoveHead();
        if (!ClientMessageReceiveGameMessage(ptPacketBuffer->byteData))
            return;
        PacketBufferFree(sgptClientGlobals->ptPacketPool, ptPacketBuffer);
    }
    tGCD.GameModeGet();
}


















void ClientPutMessageOnOutgoingQueue(uint8 * pMessage)
{



    RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pMessage;

    PacketBuffer * ptPacketBuffer = PacketBufferAlloc(sgptClientGlobals->ptPacketPool, ptHeader->dwSize);
    SigmaMemoryCopy(ptPacketBuffer->byteData, pMessage, ptHeader->dwSize);
    sgptClientGlobals->tOutgoingPacketBufferList.AddTail_Tracked(ptPacketBuffer);
}




void ClientPutMessageOnIncomingQueue(uint8 * pMessage)
{



    RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pMessage;

    PacketBuffer * ptPacketBuffer = PacketBufferAlloc(sgptClientGlobals->ptPacketPool, ptHeader->dwSize);
    SigmaMemoryCopy(ptPacketBuffer->byteData, pMessage, ptHeader->dwSize);
    sgptClientGlobals->tIncomingPacketBufferList.AddTail_Tracked(ptPacketBuffer);
}




void ClientReadUpdate()

{
    XClientSocketUpdate(sgptClientGlobals->tClientSocket);

    if (sLatencySimulatorIsActive())
    {
        sLatencySimulatorUpdate();
    }
}




void ClientWriteUpdate()
{

    if (ClientIsConnecting() ||
        ClientIsConnected())
    {
        XClientSocketWriteUpdate(sgptClientGlobals->tClientSocket);
    }
}




void ClientMarkSeasonEnded()
{
    sgptClientGlobals->bSeasonEnded = TRUE;
}




void ClientMarkChallengeRiftsDisabled()
{
    sgptClientGlobals->bChallengeRiftsDisabled = TRUE;
}
