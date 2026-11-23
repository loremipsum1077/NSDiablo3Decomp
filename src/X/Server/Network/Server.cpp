// Original file: X/Server/Network/Server.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Networking/Network.h"
#include "../../Common/Player.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Networking/XNetAddress.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "Server.h"

typedef int32 ServerSocketID;

#include "XServerSocket.h"
#include "../../Sigma/Networking/XEncoder.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../Actors/Actors.h"
#include "../../Common/Items.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
// Declared here from their definitions elsewhere (headers not adopted yet).
void ReplaySendServerMessages();
BOOL XServerSocketGetPeerAddress(ServerSocketID idServerSocket, XNetAddress & tAddress);
void sServerReceiveIncomingQueue();
void ServerPutMessageOnOutgoingQueue(GameConnectionID idGameConnection, void * pMessage);
BOOL ReplayIsPlaying();
void ReplayServerClientDisconnect(int nPlayerIndex);
void SGamePause(int bPause);
void PacketBufferListFreeAll(PacketPool * ptPool, TPacketBufferList & tList);
int32 SGamePlayerConnected(GameConnectionID idGameConnection, int32 nControllerIndex, int32 nConsolePartyIndex);
PacketBuffer * PacketBufferAlloc(PacketPool * ptPool, uint32 dwSize);
BOOL MessageCanBeDropped(GameMessageType eType);
BOOL ReplayIsPlayingMultiServerGame();
BOOL ReplayIsPlayingServerCPUTest();
uint32 XServerGetMaxSendBufferSize(ServerSocketID idServerSocket);
uint8 * NetworkGetEncodeBuffer();
BOOL EncodeMessage(void * pMessage, XEncoder & tEncoder);
BOOL XServerSendBuffer(ServerSocketID idServerSocket, void * pBuffer, uint32 dwSize);
void XServerWriteUpdateForASocket(ServerSocketID idServerSocket);
void PacketBufferFree(PacketPool * ptPool, PacketBuffer *& ptPacketBuffer);
BOOL ServerMessageReceiveOutsideGame(ServerSocketID idServerSocket, void * pMessage);  // X/Server/Network/ServerMessageReceive.cpp:206
void ServerMessageReceive(int32 nPlayerIndex, void * pMessage);
void SPlayerOffline(Player & tPlayer, BOOL bDisconnected);
// PacketPoolCreate is declared by X/Common/Networking/Network.cpp:595 (DWARF).
PacketPool * PacketPoolCreate(XMemoryAllocatorInterface * ptMemoryInterface);
// PacketPoolDispose is declared by X/Common/Networking/Network.cpp:646 (DWARF).
void PacketPoolDispose(PacketPool *& ptPacketPool, XMemoryAllocatorInterface * ptMemoryInterface);
#include "../PlayerList.h"
#include "KnownList.h"
// DWARF definitions: X/Server/SPlayer.cpp:1601 and :1620.
KnownListID PlayerKnownListGetCurrent(const Player & tPlayer);
KnownListID PlayerKnownListGetPrevious(const Player & tPlayer);
// DWARF definition: X/Server/Network/ServerMessageSend.cpp:1272.
void ServerMessageSendExitMessages(const PlayerList & listDestPlayers, const KnownEntryListID idExitList, const Player & tPlayer);
// DWARF definition: X/Server/SGame.cpp:7719.
void SGameBootPendingPlayer(OnlineService::HeroId uHeroId, QuitReason eQuitReason, const char * pszBootLogReason, ServerSocketID idServerSocket);
// DWARF definition: X/Server/Network/Server.cpp:772.
void ServerClientDisconnectGC(GameConnectionID idGameConnection);
#include "../../../Contrib/Contrib/Zlib/zlib.h"
#include "../../Sigma/Stopwatch.h"
#include "../../Common/Game.h"
static XVarScope s_scopeNet("Net", NULL);
XVarUint32 s_varNetPacketLimit(s_scopeNet, "PacketQueueLimit", 300000);
XVarUint32 s_varLimitedPacketByteThreshold(s_scopeNet, "LimitedPacketByteThreshold", 1024);
XVarFloat s_varOutputBufferDisconnectThreshold(s_scopeNet, "OutputBufferDisconnectThreshold", 0.9f);
XVarInt32 s_varNetPacketsToCleanupImmediately(s_scopeNet, "PacketsToCleanupImmediately", 1000);
XVarInt32 s_varNetOrphanedPacketsToCleanupPerFrame(s_scopeNet, "OrphanedPacketsToCleanupPerFrame", 1000);


static XVarScope s_scopeStopwatch("Stopwatch", NULL);
XVarFloat s_varStopwatchFreePacketsWarnThreshold(s_scopeStopwatch, "FreeAllWarningThreshold", 0.02f);

static const int32 DEFAULT_PACKET_DROPPING_QUEUE_SIZE_THRESHOLD = 150000;
XVarInt32 s_varPacketDroppingQueueSizeThreshold(s_scopeNet, "PacketDroppingQueueSizeThreshold", DEFAULT_PACKET_DROPPING_QUEUE_SIZE_THRESHOLD);

static const uint32 DEFAULT_PACKET_DROPPING_GAME_UPDATE_THRESHOLD = 60;
XVarUint32 s_varPacketDroppingGameUpdateThreshold(s_scopeNet, "PacketDroppingGameUpdateThreshold", DEFAULT_PACKET_DROPPING_GAME_UPDATE_THRESHOLD);

static const float NUM_FRAMES_FOR_FULL_BUFFER_TREND = 9000.0f;

static XVarScope s_scopeCompression(s_scopeNet, "Compression");
XVarBool s_varCompressionEnabled(s_scopeCompression, "Enabled", false);
XVarBool s_varCompressionDedicatedStreams(s_scopeCompression, "DedicatedStreams", true);
XVarInt32 s_varCompressionLevel(s_scopeCompression, "Level", -1);
XVarBool s_varCompressionMetrics(s_scopeCompression, "Metrics", false);

enum NetworkCompressionMode {
    NETWORK_COMPRESSION_MODE_NONE = 0,
    NETWORK_COMPRESSION_MODE_SHARED_STREAM = 1,
    NETWORK_COMPRESSION_MODE_CONNECTION_STREAM = 2,
    NUM_NETWORK_COMPRESSION_MODES = 3,
};








enum {
    MAX_SERVER_PLAYERS = 16,
};

struct GameConnection {
    GameConnectionID id;
    ServerSocketID idServerSocket;
    int32 nPlayerIndex[4];
    uint8 uPlayerCount;
    TPacketBufferList tOutgoingPacketBufferList;
    BOOL bInGame;
    BOOL bDisposing;
    BOOL bTerminateConnectionWhenOutgoingQueueEmpty;
    BOOL bNeedHeartBeat;
    uint32 dwLastHeartBeatTime;
    uint32 uHighestCount;
    time_t tHighTime;
    uint32 dwConsecutiveLimitedPacketRoomFrameCount;
    uint32 dwTotalLimitedPacketRoomFrameCount;
    uint32 dwTotalDroppedPacketCount;
    float fOutputBufferFullRuntimeTrend;
    z_streamp ptZstream;
    NetworkCompressionMode m_eLastCompressionMode;
    uint32 dwConnectTime;
    struct Metrics {
        uint32 dwNumCompressions;
        uint32 dwBytesBeforeCompression;
        uint32 dwBytesAfterCompression;
        uint32 dwInefficientCompressions;
        uint32 dwBytesWastedByCompression;
        int64 iTotalDuration;
        struct WorstDurations {
            int64 iDuration;
            uint32 dwCompressionNum;
            uint32 dwNumBytesCompressed;
        };
        WorstDurations atWorstDuration[5];
    };
    Metrics tMetrics;
};
static_assert(sizeof(GameConnection) == 0xf8, "GameConnection");

struct ServerGlobals {
    ServerGlobals();
    ~ServerGlobals();
    TDataArray<GameConnection, int> * ptGameConnections;
    XGrowableMap<int, int, int, int> tSocketToConnectionMap;
    BOOL bRequestedUpdate;
    uint32 uActivePacketBufferIndex;
    PacketPool * aptIncomingPacketPool[2];
    TPacketListPool atIncomingPacketListPool[2];
    TPacketBufferList atIncomingPacketBufferList[2];
    PacketPool * ptOutgoingPacketPool;
    TPacketListPool tOutgoingPacketListPool;
    int32 nPacketDroppingQueueSizeThreshold;
    uint32 uPacketDroppingGameUpdateThreshold;
    TPacketHistoryListPool tPacketHistoryListPool;
    TPacketHistoryList tPacketHistory;
    void * pCurrentMessage;
    int32 nCurrentMessagePlayerIndex;
    float fOONumFramesForFullBufferTrend;
};
static_assert(sizeof(ServerGlobals) == 0x1f8, "ServerGlobals");


static const int32 MAX_PACKETS_IN_HISTORY = 20;
































ServerGlobals::ServerGlobals()
  : ptGameConnections(TDataArrayCreateTracked<GameConnection, int>("GameConnection", SGameManager::GameGet().ptMemoryInterface, MAX_SERVER_PLAYERS)),
    tSocketToConnectionMap(MAX_SERVER_PLAYERS, SGameManager::GameGet().ptMemoryInterface),
    bRequestedUpdate(FALSE), // Inferred next initializer: the original 64-bit zero store clears both adjacent members.
    uActivePacketBufferIndex(0),
    nPacketDroppingQueueSizeThreshold(DEFAULT_PACKET_DROPPING_QUEUE_SIZE_THRESHOLD),
    uPacketDroppingGameUpdateThreshold(DEFAULT_PACKET_DROPPING_GAME_UPDATE_THRESHOLD), // Inferred value-initialization below: original zero stores precede the pool constructor stores.
    tPacketHistoryListPool(),
    tPacketHistory(&tPacketHistoryListPool),
    pCurrentMessage(NULL),
    nCurrentMessagePlayerIndex(-1)
{
    for (uint32 i = 0; i < 2; i++)
    {
        aptIncomingPacketPool[i] = PacketPoolCreate(SGameManager::GameGet().ptMemoryInterface);
        atIncomingPacketListPool[i].SetMemoryInterface(SGameManager::GameGet().ptMemoryInterface);
        atIncomingPacketBufferList[i].SetPool(&atIncomingPacketListPool[i]);
    }

    ptOutgoingPacketPool = PacketPoolCreate(SGameManager::GameGet().ptMemoryInterface);
    tOutgoingPacketListPool.SetMemoryInterface(SGameManager::GameGet().ptMemoryInterface);

    SGameManager::GameGet().ptSGameGlobals->tOrphanedPacketBufferList.Initialize(&tOutgoingPacketListPool);

    tPacketHistoryListPool.SetMemoryInterface(SGameManager::GameGet().ptMemoryInterface); // Inferred reciprocal below: original float bits 0x38e90453 equal 1 / NUM_FRAMES_FOR_FULL_BUFFER_TREND.
    fOONumFramesForFullBufferTrend = 1.0f / NUM_FRAMES_FOR_FULL_BUFFER_TREND;
}



ServerGlobals::~ServerGlobals()
{
    TDataArrayDispose(ptGameConnections);
    PacketPoolDispose(aptIncomingPacketPool[0], SGameManager::GameGet().ptMemoryInterface);
    PacketPoolDispose(aptIncomingPacketPool[1], SGameManager::GameGet().ptMemoryInterface);
    PacketPoolDispose(ptOutgoingPacketPool, SGameManager::GameGet().ptMemoryInterface);
}




inline GameConnection * GameConnectionGet(GameConnectionID idGameConnection)
{
    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;

    return ptServerGlobals->ptGameConnections->ElementGet(idGameConnection);
}




inline GameConnection * GameConnectionTryToGet(GameConnectionID idGameConnection)
{
    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;

    return ptServerGlobals->ptGameConnections->ElementTryToGet(idGameConnection);
}




inline GameConnection * GameConnectionDataArrayIterate(TDataArrayIterator<GameConnection> & tIterator)
{
    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;

    return ptServerGlobals->ptGameConnections->Iterate(tIterator);
}




static void sGameConnectionDelete(GameConnection * ptGameConnection, BOOL bFreePendingOutgoingPackets)
{
    const GameConnection::Metrics & tMetrics = ptGameConnection->tMetrics;

    SGame & tSGame = SGameManager::GameGet();
    if (ptGameConnection->bDisposing)
        return;

    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;
    ptGameConnection->bDisposing = TRUE;

    Blizzard::Time::Timestamp tHigh = Blizzard::Time::FromUnixTime64(ptGameConnection->tHighTime);
    Blizzard::Time::TimeRec tRec;
    Blizzard::Time::BreakTime(tHigh, tRec);
    s_varNetPacketLimit.Get();

    SGameManager::SocketMarkForDelete(ptGameConnection->idServerSocket, SDR_GAME_CONN_DEL);

    if (bFreePendingOutgoingPackets)
    {
        Stopwatch duration;

        int32 packetsToCleanup = Math::Min(ptGameConnection->tOutgoingPacketBufferList.Count(), s_varNetPacketsToCleanupImmediately.Get());
        for (int32 i = 0; i < packetsToCleanup; ++i)
        {
            PacketBuffer * ptPacketBuffer = ptGameConnection->tOutgoingPacketBufferList.GetHead();
            ptGameConnection->tOutgoingPacketBufferList.RemoveHead();
            PacketBufferFree(ptServerGlobals->ptOutgoingPacketPool, ptPacketBuffer);
        }

        int32 orphanedPackets = ptGameConnection->tOutgoingPacketBufferList.Count();
        tSGame.ptSGameGlobals->tOrphanedPacketBufferList.Splice(ptGameConnection->tOutgoingPacketBufferList);
        (void)orphanedPackets;

        float freeDuration = duration.Check();
        if (ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_MAIN && freeDuration > s_varStopwatchFreePacketsWarnThreshold.Get())
        {
        }
    }

    if (ptGameConnection->ptZstream)
    {
        deflateEnd(ptGameConnection->ptZstream);
        SigmaMemoryFree(ptGameConnection->ptZstream, NULL);
        ptGameConnection->ptZstream = NULL;
    }

    if (tMetrics.dwBytesBeforeCompression != 0 && s_varCompressionMetrics.Get())
    {
        uint32 dwNow = GameGetServerTime();
        uint32 dwTimeConnected = dwNow - ptGameConnection->dwConnectTime;
        float fSecondsConnected = (float)dwTimeConnected;
        float fSpaceSavings = 0.0f;
        float fAvgBytesWasted = 0.0f;
        float fTotalDuration = Stopwatch::ToSeconds(tMetrics.iTotalDuration);
        (void)fSecondsConnected; (void)fSpaceSavings; (void)fAvgBytesWasted; (void)fTotalDuration;
    }

    ptServerGlobals->tSocketToConnectionMap.RemoveAt(ptGameConnection->idServerSocket);
    ptServerGlobals->ptGameConnections->ElementDelete(ptGameConnection);
}



















































void ServerInitializeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals *& ptServerGlobals = tSGame.ptServerGlobals;


    ptServerGlobals = new (SigmaMemoryNew(sizeof(ServerGlobals), 4, tSGame.ptMemoryInterface, FALSE)) ServerGlobals;
}




void ServerDisposeForGame()
{

    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;

    TDataArrayIterator<GameConnection> tIt;
    while (GameConnectionDataArrayIterate(tIt))
    {
        GameConnection * ptGameConnection = tIt.ptElement;
        sGameConnectionDelete(ptGameConnection, FALSE);
    }

    PacketBufferListFreeAll(ptServerGlobals->aptIncomingPacketPool[0], ptServerGlobals->atIncomingPacketBufferList[0]);
    PacketBufferListFreeAll(ptServerGlobals->aptIncomingPacketPool[1], ptServerGlobals->atIncomingPacketBufferList[1]);



    POS pos = ptServerGlobals->tPacketHistory.GetHeadPosition();
    while (pos)
    {
        POS oldPos = pos;
        PacketHistoryEntry & tPacketHistoryEntry = ptServerGlobals->tPacketHistory.GetNext(pos);
        PacketBufferFree(tPacketHistoryEntry.ptPacketPoolAllocator, tPacketHistoryEntry.ptPacketBuffer);
        ptServerGlobals->tPacketHistory.RemoveAt(oldPos);
    }

    ptServerGlobals->~ServerGlobals();
    SigmaMemoryFree(ptServerGlobals, tSGame.ptMemoryInterface);
}





void ServerQueueClientUpdate()
{
    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;
    if (ptServerGlobals)
        ptServerGlobals->bRequestedUpdate = TRUE;
}










void ServerEventReceive(ServerSocketID idServerSocket, uint8 * pMessage)
{

    RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pMessage;
    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;

    if (ReplayIsPlaying())
        return;







    uint32 uBackupBuffer = !ptServerGlobals->uActivePacketBufferIndex;
    PacketBuffer * ptPacketBuffer = PacketBufferAlloc(ptServerGlobals->aptIncomingPacketPool[uBackupBuffer], ptHeader->dwSize);
    ptPacketBuffer->tHeader.idSocket = idServerSocket;
    SigmaMemoryCopy(ptPacketBuffer->byteData, pMessage, ptHeader->dwSize);
    ptServerGlobals->atIncomingPacketBufferList[uBackupBuffer].AddTail_Tracked(ptPacketBuffer);
}

































void ServerEventReceiveFromReplay(int32 nPlayerIndex, void * pMessage)
{

    if (nPlayerIndex == -1)
    {
        ServerMessageReceiveOutsideGame(-1, pMessage);
        return;
    }

    GameConnection * ptGameConnection = NULL;
    TDataArrayIterator<GameConnection> tIt;





    while ((ptGameConnection = GameConnectionDataArrayIterate(tIt)) != NULL)
    {
        GameConnection * ptCurGameConnection = tIt.ptElement;

        for (uint32 i = 0; i < ptCurGameConnection->uPlayerCount; i++)
        {
            if (ptCurGameConnection->nPlayerIndex[i] == nPlayerIndex)
            {
                ServerMessageReceive(nPlayerIndex, pMessage);
                return;
            }
        }
    }





















    ServerMessageReceive(nPlayerIndex, pMessage);
}
GameConnectionID ServerClientConnect(ServerSocketID idServerSocket, int32 nControllerIndex, int32 nConsolePartyIndex)
{
    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;
    GameConnectionID existingGameConnection;
    if (ptServerGlobals->tSocketToConnectionMap.Lookup(idServerSocket, existingGameConnection))
    {
        GameConnection * ptGameConnection = GameConnectionGet(existingGameConnection);
        ptGameConnection->nPlayerIndex[nControllerIndex] = SGamePlayerConnected(existingGameConnection, nControllerIndex, nConsolePartyIndex);
        ptGameConnection->uPlayerCount++;
        return existingGameConnection;
    }

    GameConnectionID idGameConnection = ptServerGlobals->ptGameConnections->ElementNew();
    if (idGameConnection == DATAID_INVALID)
        return DATAID_INVALID;


    ptServerGlobals->tSocketToConnectionMap.SetAt_Tracked(idServerSocket, idGameConnection);
    GameConnection * ptGameConnection = GameConnectionGet(idGameConnection);
    ptGameConnection->tOutgoingPacketBufferList.SetPool(&ptServerGlobals->tOutgoingPacketListPool);
    for (int32 i = 0; i < 4; i++)
    {
        ptGameConnection->nPlayerIndex[i] = -1;
    }
    ptGameConnection->idServerSocket = idServerSocket;
    ptGameConnection->bInGame = FALSE;
    ptGameConnection->bDisposing = FALSE;
    ptGameConnection->bNeedHeartBeat = FALSE;
    ptGameConnection->dwLastHeartBeatTime = 0;
    ptGameConnection->nPlayerIndex[nControllerIndex] = SGamePlayerConnected(idGameConnection, nControllerIndex, nConsolePartyIndex);
    ptGameConnection->uPlayerCount = 1;
    ptGameConnection->bTerminateConnectionWhenOutgoingQueueEmpty = FALSE;
    ptGameConnection->uHighestCount = 0;
    ptGameConnection->tHighTime = 0;
    ptGameConnection->dwConsecutiveLimitedPacketRoomFrameCount = 0;
    ptGameConnection->dwTotalLimitedPacketRoomFrameCount = 0;
    ptGameConnection->dwTotalDroppedPacketCount = 0;
    ptGameConnection->fOutputBufferFullRuntimeTrend = 0.0f;
    ptGameConnection->dwConnectTime = GameGetServerTime();
    ptGameConnection->ptZstream = NULL;
    ptGameConnection->m_eLastCompressionMode = (NetworkCompressionMode)0;
    SigmaMemoryClear(&ptGameConnection->tMetrics, sizeof(ptGameConnection->tMetrics));
    return idGameConnection;
}






















































void ServerDisconnectWhenOutgoingQueueEmpty(Player & tPlayer)
{
    GameConnection * ptGameConnection = GameConnectionTryToGet(tPlayer.idGameConnection);
    if (ptGameConnection)
        ptGameConnection->bTerminateConnectionWhenOutgoingQueueEmpty = TRUE;

    tPlayer.idGameConnection = -1;
}










void ServerCleanUpGameConnections(
    ExternalGameSignals & tExternalSignals)
{
    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;


    XGrowableMapTS<unsigned long, OnlineService::PlayerResolveData>::MapContentsArray tPendingPlayers;
    if (tExternalSignals.tDisconnectedSocketList.Count())
    {

        tSGame.tPendingJoinMap.Copy(tPendingPlayers);
    }

    while (tExternalSignals.tDisconnectedSocketList.Count())
    {
        ServerSocketID idServerSocket = tExternalSignals.tDisconnectedSocketList.GetHead();
        tExternalSignals.tDisconnectedSocketList.RemoveHead();



        GameConnectionID idGameConnection;
        if (ptServerGlobals->tSocketToConnectionMap.Lookup(idServerSocket, idGameConnection))
        {
            ServerClientDisconnectGC(idGameConnection);
        }
        else
        {

            for (int32 i = 0; i < tPendingPlayers.GetArraySize(); i++)
            {
                OnlineService::PlayerResolveData & tPlayerInArray = tPendingPlayers[i];
                if (tPlayerInArray.idServerSocket == idServerSocket)
                {

                    SGameBootPendingPlayer(tPlayerInArray.uHeroId, QUIT_REASON_COUNT, "Socket Disconnect", SERVERSOCKETID_INVALID);
                }
            }






            SGameManager::SocketMarkForDelete(idServerSocket, SDR_CLEANUP);
        }
    }
}




void ServerClientDisconnect(ServerSocketID idServerSocket)
{

    SGame & tSGame = SGameManager::GameGet();
    ExternalGameSignals & tSignals = *tSGame.ptSGameGlobals->ptInactiveSignals;

    tSignals.tDisconnectedSocketList.AddTail_Tracked(idServerSocket);
}





void sClearPlayerGameConnection(GameConnection * ptGameConnection)
{

    if (SGameIsShuttingDown())
        return;






    for (uint32 i = 0; i < 4; i++)
    {

        if (ptGameConnection->nPlayerIndex[i] == -1)
            continue;

        Player * ptPlayer = PlayerGetByPlayerIndex(ptGameConnection->nPlayerIndex[i]);




        if (ptPlayer && ptPlayer->idGameConnection == ptGameConnection->id)


            SPlayerOffline(*ptPlayer, TRUE);
    }
}
void ServerClientDisconnectGC(GameConnectionID idGameConnection)
{

    GameConnection * ptGameConnection = GameConnectionGet(idGameConnection);

    for (uint32 i = 0; i < ptGameConnection->uPlayerCount; i++)
    {
        ReplayServerClientDisconnect(ptGameConnection->nPlayerIndex[i]);
    }

    sClearPlayerGameConnection(ptGameConnection);
    sGameConnectionDelete(ptGameConnection, TRUE);
    SGamePause(FALSE);
}










void ServerPlayerLeftOnConnection(
    GameConnectionID idGameConnection,
    const Player & tPlayer)
{



    int32 nLocalIndex = tPlayer.nControllerIndex;

    GameConnection * ptGameConnection = GameConnectionGet(idGameConnection);

    ptGameConnection->uPlayerCount--;

    ptGameConnection->nPlayerIndex[nLocalIndex] = -1;


    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);

    if (ptACDPlayer && ptACDPlayer->IsInWorld() && ptGameConnection->uPlayerCount)
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());


        for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (ptPlayer->nPlayerIndex != tPlayer.nPlayerIndex &&
                PlayersAreLocalToEachOther(*ptPlayer, tPlayer))
            {
                PlayerListGetSingle(listDestPlayers, *ptPlayer);
                KnownEntryListID idExitList = KnownEntryListAlloc();
                KnownEntryListID idUpdateList = KnownEntryListAlloc();
                KnownEntryListID idEnterList = KnownEntryListAlloc();

                KnownListID idKnownListPrevious = PlayerKnownListGetCurrent(tPlayer);
                KnownListID idKnownListEmpty = PlayerKnownListGetPrevious(tPlayer);
                KnownListClear(idKnownListEmpty, FALSE, NULL);
                KnownListBuildChangedLists(idKnownListPrevious, idKnownListEmpty, idEnterList, idExitList, idUpdateList);







                ServerMessageSendExitMessages(listDestPlayers, idExitList, tPlayer);

                KnownEntryListFree(idExitList);
                KnownEntryListFree(idUpdateList);
                KnownEntryListFree(idEnterList); // Inferred break below: original next enters list destruction.
                break;
            }
        }
    }

}




void ServerConnectionEnteredGame(GameConnectionID idGameConnection)
{
    GameConnection * ptGameConnection = GameConnectionGet(idGameConnection);
    ptGameConnection->bInGame = TRUE;
}





// Inferred: two separate drop tests (894, 899); their shared increment carries row 899 and has no row of its own.
void ServerPutMessageOnOutgoingQueue(
    GameConnectionID idGameConnection,
    void * pMessage)
{
    SGame & tSGame = SGameManager::GameGet();
    if (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_UTILITY))
    {
        return;
    }
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;
    GameConnection * ptGameConnection = GameConnectionGet(idGameConnection);


    if (ptGameConnection->bTerminateConnectionWhenOutgoingQueueEmpty)
    {
        return;
    }



    RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pMessage;




    if (MessageCanBeDropped(ptHeader->eType))
    {
        int32 nCount = ptGameConnection->tOutgoingPacketBufferList.Count();
        if (nCount > ptServerGlobals->nPacketDroppingQueueSizeThreshold)
        {
            ptGameConnection->dwTotalDroppedPacketCount++;
            return;
        }
        if (tSGame.uScheduledGameUpdateCount > ptServerGlobals->uPacketDroppingGameUpdateThreshold)
        {
            ptGameConnection->dwTotalDroppedPacketCount++;
            return;
        }
    }

    PacketBuffer * ptPacketBuffer = PacketBufferAlloc(ptServerGlobals->ptOutgoingPacketPool, ptHeader->dwSize);
    SigmaMemoryCopy(ptPacketBuffer->byteData, pMessage, ptHeader->dwSize);
    ptGameConnection->tOutgoingPacketBufferList.AddTail_Tracked(ptPacketBuffer);
    ptGameConnection->bNeedHeartBeat = TRUE;


}




void ServerSendOutOfGameMessage(ServerSocketID idServerSocket, void * pMessage, GameMessageType eType, uint32 dwSize)
{

    if (ReplayIsPlayingMultiServerGame() || ReplayIsPlayingServerCPUTest())
    {
        return;
    }

    SGameID idSGame = XServerSocketGetSGameID(idServerSocket);



    uint32 dwSocketRoom = XServerGetMaxSendBufferSize(idServerSocket);










    XEncoder tEncoder;
    if (dwSocketRoom < 5)
    {
        XServerSocketDisconnect(idServerSocket, SDR_SEND_BUFFER_FULL);
        return;
    }


    dwSocketRoom = Math::Min(dwSocketRoom, (uint32)0x8000);
    tEncoder.SetData(NetworkGetEncodeBuffer(), dwSocketRoom);
    tEncoder.SetUsed(32);

    RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pMessage;
    ptHeader->eType = eType;
    ptHeader->dwSize = (uint16)dwSize;
    BOOL bResult = EncodeMessage(pMessage, tEncoder);
    if (!bResult)
    {
        XServerSocketDisconnect(idServerSocket, SDR_MSG_QUEUE_FULL);
        return;
    }

    uint32 dwEncodedBytes = (uint32)tEncoder.UsedBytes();
    tEncoder.SetUsed(0);
    tEncoder.Put(dwEncodedBytes, 32);

    bResult = XServerSendBuffer(idServerSocket, tEncoder.Data(), dwEncodedBytes);
    if (!bResult)
    {
        XServerSocketDisconnect(idServerSocket, SDR_SEND_FAILED);
        return;
    }

    XServerWriteUpdateForASocket(idServerSocket);
}
// DWARF definition: X/Server/SScene.cpp:1143.
void SActivationUpdate(BOOL bUpdateLists);
// DWARF definitions: X/Server/SPlayer.cpp:13042, :1610, :1629, :13352.
void SPlayerConsoleSync(Player & tPlayer);
void PlayerKnownListSetCurrent(const Player & tPlayer, KnownListID idKnownList);
void PlayerKnownListSetPrevious(const Player & tPlayer, KnownListID idKnownList);
void SPlayerPrefetchDestinations(Player & tPlayer);
// DWARF definitions: X/Server/Network/ServerMessageSend.cpp:1114, :1365, :1466.
void ServerMessageSendEnterMessages(const PlayerList & listDestPlayers, const KnownEntryListID idEnterList, const Player & tPlayer);
void ServerMessageSendPlayerState(const ACDID idPlayerACD);
void ServerMessageSendUpdateMessagesFromKnownEntryList(const PlayerList & listDestPlayers, const KnownEntryListID idUpdateList);
// DWARF definitions: X/Server/SScene.cpp:948, :975, :298.
void SSceneSendSceneSwapMessages(const PlayerList & listDestPlayers);
void SSceneFinalizeSubSceneSwaps();
void SSceneDeleteAllFlagged();
// DWARF definitions: X/Common/ActorCommonData.cpp:2540, :2602.
void ACDSendServerUpdates(const PlayerList & listDestPlayers);
void ACDModifiedClearAll();
// DWARF definition: X/Server/Network/STrickle.cpp:138.
void STrickleUpdate();
// DWARF definition: X/Client/CGame.cpp:6147.
BOOL CGameLoadingScreenIsEnabled();










static BOOL sServerEncodeMessage(
    GameConnection * ptGameConnection,
    uint8 * pMessage,
    XEncoder & tEncoder)
{
    // DWARF: Server.cpp:1007, inlined at 1374; only the 1017:12 return leaves a row (ptGameConnection unused here).




    return EncodeMessage(pMessage, tEncoder);
}
























uint32 sServerCompressBuffer(
    GameConnection * ptGameConnection,
    NetworkCompressionMode eCompressionMode,
    void * pMessage,
    uint32 dwSize,
    void * pDest,
    uint32 dwAvailable)
{
    ThreadGlobals & tThreadGlobals = ThreadGlobals::Get();

    int32 nFlushMode;
    z_streamp ptStream;

    switch (eCompressionMode)
    {
        case NETWORK_COMPRESSION_MODE_CONNECTION_STREAM:

            if (!ptGameConnection->ptZstream)
            {
                ptGameConnection->ptZstream = (z_streamp)SigmaMemoryNew(sizeof(z_stream), 4, NULL, TRUE);
                deflateInit(ptGameConnection->ptZstream, s_varCompressionLevel.Get());
            } // Flush constant inferred from original immediate 2 (Z_SYNC_FLUSH).
            nFlushMode = Z_SYNC_FLUSH;
            ptStream = ptGameConnection->ptZstream;
            break;

        case NETWORK_COMPRESSION_MODE_SHARED_STREAM:

            if (!tThreadGlobals.m_ptDeflateZstream)
            {
                tThreadGlobals.m_ptDeflateZstream = (z_streamp)SigmaMemoryNew(sizeof(z_stream), 4, NULL, TRUE);
                deflateInit(tThreadGlobals.m_ptDeflateZstream, s_varCompressionLevel.Get());
            } // Flush constant inferred from original immediate 3 (Z_FULL_FLUSH).
            nFlushMode = Z_FULL_FLUSH;
            ptStream = tThreadGlobals.m_ptDeflateZstream;
            break;
        // Inferred default: modes other than shared/connection branch directly to the zero return.
        default:
            return 0;
    }





    ptStream->next_in = (Bytef *)pMessage;
    ptStream->avail_in = dwSize;

    ptStream->next_out = (Bytef *)pDest;
    ptStream->avail_out = dwAvailable;


    Stopwatch tStopwatch;
    int nResult = deflate(ptStream, nFlushMode);
    int64 iStopwatchDuration = tStopwatch.CheckLong();

    if (nResult != Z_OK)
    {
        return 0;
    }


    uint32 dwCompressedOut = dwAvailable - ptStream->avail_out;

    if (ptStream->avail_in)
    {
        return 0;
    }



    if (s_varCompressionMetrics.Get())
    {
        GameConnection::Metrics & tMetrics = ptGameConnection->tMetrics;

        tMetrics.dwNumCompressions++;
        tMetrics.dwBytesBeforeCompression += dwSize;
        tMetrics.dwBytesAfterCompression += dwCompressedOut;

        if (dwCompressedOut > dwSize)
        {
            tMetrics.dwInefficientCompressions++;
            tMetrics.dwBytesWastedByCompression += dwCompressedOut - dwSize;
        }

        tMetrics.iTotalDuration += iStopwatchDuration;



        if (iStopwatchDuration > tMetrics.atWorstDuration[4].iDuration)
        {
            for (uint32 i = 0; i < 5; i++)
            {
                if (iStopwatchDuration > tMetrics.atWorstDuration[i].iDuration)
                {



                    for (uint32 n = 4; n > i; n--)
                    {
                        tMetrics.atWorstDuration[n] = tMetrics.atWorstDuration[n - 1];
                    }

                    tMetrics.atWorstDuration[i].iDuration = iStopwatchDuration;
                    tMetrics.atWorstDuration[i].dwCompressionNum = tMetrics.dwNumCompressions;
                    tMetrics.atWorstDuration[i].dwNumBytesCompressed = dwSize; // Inferred break below: the original next branches to the function return.
                    break;
                }
            }
        }
    }
    // Inferred return: the original epilogue returns the computed output-byte count.
    return dwCompressedOut;
}




// Inferred statement shape: the original selects with cset ne under the 1166 greater-than test (row 1167).
static int GameMessagePairComparator(const void * left, const void * right)
{
    blz::pair<int, unsigned int> l = *(const blz::pair<int, unsigned int> *)left;
    blz::pair<int, unsigned int> r = *(const blz::pair<int, unsigned int> *)right;
    if (l.second > r.second) return -1;
    if (l.second == r.second) return 0;
    return 1;

}














































































































































































































































































































































































void ServerCleanupOrphanedPackets()
{

    SGame & tSGame = SGameManager::GameGet();


    int32 packetsToCleanup = Math::Min(tSGame.ptSGameGlobals->tOrphanedPacketBufferList.Count(), (s_varNetOrphanedPacketsToCleanupPerFrame.Get() < 1) ? 1 : s_varNetOrphanedPacketsToCleanupPerFrame.Get());
    ServerGlobals *& ptServerGlobals = tSGame.ptServerGlobals;





    for (int i = 0; i < packetsToCleanup; i++)
    {

        PacketBuffer * ptPacketBuffer = tSGame.ptSGameGlobals->tOrphanedPacketBufferList.GetHead();
        tSGame.ptSGameGlobals->tOrphanedPacketBufferList.RemoveHead();
        PacketBufferFree(ptServerGlobals->ptOutgoingPacketPool, ptPacketBuffer);
    }
}


void ServerTogglePacketBuffers(SGame * ptSGame)
{
    ServerGlobals * ptServerGlobals = ptSGame->ptServerGlobals;
    ptServerGlobals->uActivePacketBufferIndex = !ptServerGlobals->uActivePacketBufferIndex;
}





void ServerLimitPacketHistorySize(SGame * ptSGame)
{

    ServerGlobals * ptServerGlobals = ptSGame->ptServerGlobals;
    TPacketHistoryList & tPacketHistory = ptServerGlobals->tPacketHistory;

    while (tPacketHistory.Count() > MAX_PACKETS_IN_HISTORY)
    {
        PacketHistoryEntry & tEntryToFree = tPacketHistory.GetHead();
        PacketBufferFree(tEntryToFree.ptPacketPoolAllocator, tEntryToFree.ptPacketBuffer);
        tPacketHistory.RemoveHead();
    }
}



int32 PacketGetPlayerIndex(
    const PacketBuffer * ptPacket)
{
    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;


    GameConnectionID idGameConnection;
    if (!ptServerGlobals->tSocketToConnectionMap.Lookup(ptPacket->tHeader.idSocket, idGameConnection))
        return -1;




    GameConnection * ptGameConnection = GameConnectionGet(idGameConnection);



    const RequiredMessageHeader * ptHeader = (const RequiredMessageHeader *)ptPacket->byteData;
    int32 nControllerIndex = ptHeader->nControllerIndex;
    if (nControllerIndex < 0 || nControllerIndex >= 4)
        return -1;



    int32 nPlayerIndex = ptGameConnection->nPlayerIndex[nControllerIndex];
    if (nPlayerIndex == -1)
        return -1;







    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    if (ptPlayer->idGameConnection != idGameConnection)
        return -1;




    return ptPlayer->nPlayerIndex;
}



void sServerReceiveIncomingQueue()

{
    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;

    TPacketBufferList & tIncoming = ptServerGlobals->atIncomingPacketBufferList[ptServerGlobals->uActivePacketBufferIndex];
    TPacketHistoryList & tPacketHistory = ptServerGlobals->tPacketHistory;
    PacketPool * ptIncomingPacketPool = ptServerGlobals->aptIncomingPacketPool[ptServerGlobals->uActivePacketBufferIndex];

    while (tIncoming.Count())
    {
        PacketBuffer * ptPacketBuffer = tIncoming.GetHead();
        tIncoming.RemoveHead();


        int32 nPlayerIndex = PacketGetPlayerIndex(ptPacketBuffer);
        if (nPlayerIndex == -1)
        {

            PacketBufferFree(ptIncomingPacketPool, ptPacketBuffer); // Inferred continue below: the invalid-player path returns to the queue-count test.
            continue;
        }


        PacketHistoryEntry tPacketHistoryEntry;
        tPacketHistoryEntry.dwTickReceived = GameGetServerTime();
        tPacketHistoryEntry.ptPacketBuffer = ptPacketBuffer;
        tPacketHistoryEntry.ptPacketPoolAllocator = ptServerGlobals->aptIncomingPacketPool[ptServerGlobals->uActivePacketBufferIndex]; // Inferred next assignment: AddTail stores nPlayerIndex in the copied history entry.
        tPacketHistoryEntry.nPlayerIndex = nPlayerIndex;
        tPacketHistory.AddTail_Tracked(tPacketHistoryEntry);


        ptServerGlobals->pCurrentMessage = ptPacketBuffer->byteData;
        ptServerGlobals->nCurrentMessagePlayerIndex = nPlayerIndex;

        ServerMessageReceive(nPlayerIndex, ptServerGlobals->pCurrentMessage);


        ptServerGlobals->pCurrentMessage = NULL;
        ptServerGlobals->nCurrentMessagePlayerIndex = -1;
    }
}




void ServerProcessIncomingMessages()
{
    ReplaySendServerMessages();
    sServerReceiveIncomingQueue();
}








void ServerSendHeartBeats()
{

    uint32 dwNow = GameGetServerTime();

    TDataArrayIterator<GameConnection> tIt;


    while (GameConnectionDataArrayIterate(tIt))
    {


        GameConnection * ptGameConnection = tIt.ptElement;




        if (!ptGameConnection->bInGame)
            continue;

        int32 nTimeSinceHeartBeat = dwNow - ptGameConnection->dwLastHeartBeatTime;





        if (ptGameConnection->bNeedHeartBeat || nTimeSinceHeartBeat >= 900)
        {
            ptGameConnection->dwLastHeartBeatTime = dwNow;

            DWordDataMessage tMessage;
            tMessage.tHeader.eType = SMSG_HEARTBEAT;
            tMessage.tHeader.dwSize = sizeof(tMessage);
            tMessage.tHeader.nControllerIndex = -1;
            tMessage.dwData = dwNow;
            ServerPutMessageOnOutgoingQueue(ptGameConnection->id, &tMessage);
            ptGameConnection->bNeedHeartBeat = FALSE;
        }
    }
}





void ServerUpdateClients(
    BOOL bUpdateKnownLists)
{

    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;

    if (!ptServerGlobals->bRequestedUpdate)
    {
        return;
    }



    SActivationUpdate(bUpdateKnownLists);


    if (bUpdateKnownLists)
    {
        ptServerGlobals->bRequestedUpdate = FALSE;
    }

    PlayerList listUpdateOnlyPlayers(PlayerListGetElementPool());






    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {

        Player & tPlayer = *ptPlayer;

        if (!tPlayer.bIsPrimaryPlayerServerOnly &&
            tPlayer.bForceKnownListUpdate)
        {
            Player * ptPrimaryPlayer = PlayerGetPrimaryPlayerForGameConnection(tPlayer.idGameConnection);
            if (ptPrimaryPlayer)
            {
                ptPrimaryPlayer->bForceKnownListUpdate = TRUE;
            }

            tPlayer.bForceKnownListUpdate = FALSE;
        }
    }



    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {

        Player & tPlayer = *ptPlayer;



        SPlayerConsoleSync(tPlayer);

        if (!tPlayer.bIsPrimaryPlayerServerOnly)
            continue;



        KnownEntryListID idExitList = KnownEntryListAlloc();

        KnownListProcessDeletedEntries(ptPlayer, idExitList);


        ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);

        BOOL fSkipPlayer = !ptACDPlayer || !PlayerIsInWorld(tPlayer) ||
            tPlayer.nPlayerIndex < 0 || tPlayer.nPlayerIndex >= MAX_PLAYERS;

        if (!fSkipPlayer)
        {
            KnownListID idKnownListCurrent = PlayerKnownListGetCurrent(tPlayer);
            if (bUpdateKnownLists || tPlayer.bForceKnownListUpdate)
            {

                KnownListID idKnownListPrevious = PlayerKnownListGetPrevious(tPlayer);



                KnownListClear(idKnownListPrevious, FALSE, NULL);


                // Inferred swap: original later passes previous as current and current as previous.
                KnownListID idKnownListTemp = idKnownListCurrent;
                idKnownListCurrent = idKnownListPrevious;
                idKnownListPrevious = idKnownListTemp;


                PlayerKnownListSetCurrent(tPlayer, idKnownListCurrent);
                PlayerKnownListSetPrevious(tPlayer, idKnownListPrevious);




                KnownListBuild(tPlayer, idKnownListCurrent);
                KnownListProcessAddedEntries(ptPlayer, KNOWNENTRYLIST_INVALID);


                KnownListBuildChangedLists(idKnownListPrevious, idKnownListCurrent, KNOWNENTRYLIST_INVALID, idExitList, KNOWNENTRYLIST_INVALID);
            }
        }






        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, tPlayer);


        ServerMessageSendExitMessages(listDestPlayers, idExitList, tPlayer);

        KnownEntryListFree(idExitList);
    }
    // Inferred initial flag: original starts false and preserves true after any warp reason.
    BOOL fAnyPlayerWarping = FALSE;



    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {

        Player & tPlayer = *ptPlayer;


        if (!tPlayer.bIsPrimaryPlayerServerOnly)
        {
            if (GameGetParts() != GAME_PARTS_HIDDEN_SERVER)
            {

                ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);

                BOOL fSkipPlayer = !ptACDPlayer || !PlayerIsInWorld(tPlayer) ||
                    tPlayer.nPlayerIndex < 0 || tPlayer.nPlayerIndex >= MAX_PLAYERS;

                if (!fSkipPlayer)
                {

                    ServerMessageSendPlayerState(tPlayer.idPlayerACD);
                }
            }
            continue;
        }


        // Inferred zero warp-reason spelling: original compares eWarpReason against zero.
        fAnyPlayerWarping = fAnyPlayerWarping || (tPlayer.eWarpReason != 0);


        KnownEntryListID idUpdateList = KnownEntryListAlloc();
        KnownEntryListID idEnterList = KnownEntryListAlloc();
        // Inferred defaults: only a full known-list update clears add/remove-only; valid player sets state.
        BOOL fSendOnlyAddRemove = TRUE; // Inferred next flag default: non-valid-player paths do not send player state.
        BOOL fSendPlayerState = FALSE;

        if (GameGetParts() != GAME_PARTS_HIDDEN_SERVER)
        {

            ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);

            BOOL fSkipPlayer = !ptACDPlayer || !PlayerIsInWorld(tPlayer) ||
                tPlayer.nPlayerIndex < 0 || tPlayer.nPlayerIndex >= MAX_PLAYERS;

            if (!fSkipPlayer)
            {
                // Inferred flag assignment: every valid-player path subsequently sends player state.
                fSendPlayerState = TRUE;
                KnownListID idKnownListCurrent = PlayerKnownListGetCurrent(tPlayer);
                if (bUpdateKnownLists || tPlayer.bForceKnownListUpdate)
                {



                    KnownListID idKnownListPrevious = PlayerKnownListGetPrevious(tPlayer);




                    KnownListBuildChangedLists(idKnownListPrevious, idKnownListCurrent, idEnterList, KNOWNENTRYLIST_INVALID, idUpdateList);



                    // Inferred flag assignment: this path skips add-only processing and sends updates.
                    fSendOnlyAddRemove = FALSE;

                    tPlayer.bForceKnownListUpdate = FALSE;
                }
                else
                {
                    listUpdateOnlyPlayers.AddTail_Tracked(ptPlayer);
                }
            }
        }






        if (fSendOnlyAddRemove)
        {
            KnownListProcessAddedEntries(ptPlayer, idEnterList);
        }


        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, tPlayer);





        ServerMessageSendEnterMessages(listDestPlayers, idEnterList, tPlayer);

        if (fSendPlayerState)
        {

            ServerMessageSendPlayerState(tPlayer.idPlayerACD);
        }

        if (!fSendOnlyAddRemove)
        {



            SSceneSendSceneSwapMessages(listDestPlayers);



            ServerMessageSendUpdateMessagesFromKnownEntryList(listDestPlayers, idEnterList);
            ServerMessageSendUpdateMessagesFromKnownEntryList(listDestPlayers, idUpdateList);
        }


        KnownEntryListFree(idUpdateList);
        KnownEntryListFree(idEnterList);
    }



    if (!fAnyPlayerWarping &&
        !CGameLoadingScreenIsEnabled())
    {

        for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
        {
            SPlayerPrefetchDestinations(*ptPlayer);
        }
    }



    if (listUpdateOnlyPlayers.Count() > 0)
    {

        SSceneSendSceneSwapMessages(listUpdateOnlyPlayers);
        ACDSendServerUpdates(listUpdateOnlyPlayers);
    }




    KnownListResetDeletedItems();
    KnownListResetAddedItems();
    KnownListResetChangedItems();



    ACDModifiedClearAll();


    STrickleUpdate();




    SSceneFinalizeSubSceneSwaps();

    SSceneDeleteAllFlagged();

}





void ServerACDGarbageCollectOldestItemOnGround()
{

    ActorID idOldestActor = ACTORID_INVALID;
    uint32 dwOldestTime = (uint32)-1;



    ActorIterator tIt;
    while (ActorDataArrayIterate(tIt))
    {
        Actor * ptActor = tIt.ptElement;



        ActorCommonData & tACD = ACDGet(ptActor->idACD);
        const ActorLocation & tLocation = tACD.GetLocation();
        if (tLocation.eActorLocation == AL_WORLD && tACD.IsItem())
        {
            uint32 dwActorTimestamp = tACD.GetTimeStamp();
            ItemQualityLevel eIQL = ItemGetQualityLevel(tACD);
            if (eIQL == IQL_MAGIC_1_AFFIX || eIQL == IQL_MAGIC_2_AFFIX || eIQL == IQL_MAGIC_3_AFFIX || eIQL == IQL_SPECIAL)
                dwActorTimestamp *= 2;
            else if (eIQL >= IQL_RARE_4_AFFIX && eIQL <= IQL_RARE_6_AFFIX)
                dwActorTimestamp *= 3;
            else if (eIQL == IQL_AFFIX_COUNT_END)
                dwActorTimestamp *= 4;

            if (dwOldestTime == (uint32)-1 || dwActorTimestamp < dwOldestTime)
            {
                dwOldestTime = dwActorTimestamp;
                idOldestActor = ptActor->id;
            }
        }
    }

    if (idOldestActor != ACTORID_INVALID)
    {
        ActorDelete(idOldestActor);
    }
}


















GameConnectionID ServerGetOnlyGameConnection()
{
    GameConnection * ptGameConnection;
    TDataArrayIterator<GameConnection> tIt;
    ptGameConnection = GameConnectionDataArrayIterate(tIt);
    if (ptGameConnection)
        return ptGameConnection->id;
    return -1;
}










BOOL ServerGetPlayerAddress(const Player & tPlayer, XNetAddress & tPlayerAddress)
{
    GameConnection * ptGameConnection = GameConnectionTryToGet(tPlayer.idGameConnection);
    if (ptGameConnection)
        return XServerSocketGetPeerAddress(ptGameConnection->idServerSocket, tPlayerAddress);
    return FALSE;
}




















int32 ServerGetPlayerIndexFromGameConnectionID(GameConnectionID idGameConnection, int32 nControllerIndex)
{
    GameConnection * ptGameConnection = GameConnectionGet(idGameConnection);
    return ptGameConnection->nPlayerIndex[nControllerIndex];
}










int32 ServerGetPlayerCountForGameConnectionID(GameConnectionID idGameConnection)
{
    GameConnection * ptGameConnection = GameConnectionGet(idGameConnection);
    return ptGameConnection->uPlayerCount;
}






uint32 ServerGameConnectionGetTotalCount()
{
    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;
    return ptServerGlobals->ptGameConnections->GetTotalCount();
}





ServerSocketID ServerGetSocketIDFromGameConnectionID(GameConnectionID idGameConnection)
{
    GameConnection * ptGameConnection = GameConnectionGet(idGameConnection);
    return ptGameConnection->idServerSocket;
}










































int32 ServerGetUnsentPacketCountTotal()
{
    int32 nPacketCount = 0;
    TDataArrayIterator<GameConnection> tIt;
    while (GameConnectionDataArrayIterate(tIt))
    {
        GameConnection * ptGameConnection = tIt.ptElement;
        nPacketCount += ptGameConnection->tOutgoingPacketBufferList.Count();
    }
    return nPacketCount;
}







void ServerUpdateServerVars()
{
    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;
    ptServerGlobals->nPacketDroppingQueueSizeThreshold = s_varPacketDroppingQueueSizeThreshold.Get();
    ptServerGlobals->uPacketDroppingGameUpdateThreshold = s_varPacketDroppingGameUpdateThreshold.Get();
}

#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include <string.h>
#include <stdlib.h>
void ActorsDumpList();
void TimersDumpList();
CRefString GameMessageTypeToName(GameMessageType eMessageType);
uint8 * NetworkGetCompressionBuffer();
BOOL XServerSocketHasDataToSend(ServerSocketID idServerSocket);

// Original Server.cpp:1023 helper; inverse of the compression size bound.
uint32 sMaxCompressibleBytes(uint32 dwBufferSize)
{
    if (dwBufferSize < 11)
        return 0;
    dwBufferSize -= 11;
    return dwBufferSize - (dwBufferSize >> 12) - (dwBufferSize >> 14);
}

#line 1171  // DWARF: sServerSendOutgoingQueue is declared at Server.cpp:1171 (its body below is not re-laid)
static BOOL sServerSendOutgoingQueue(GameConnection * ptGameConnection)
{
    SGame & tSGame = SGameManager::GameGet();
    ServerGlobals * ptServerGlobals = tSGame.ptServerGlobals;
    TPacketBufferList & tOutgoing = ptGameConnection->tOutgoingPacketBufferList;
    BOOL bDoCompress = FALSE;
    uint32 dwSocketRoom = uint32(-4);
    NetworkCompressionMode eCompressionMode = NETWORK_COMPRESSION_MODE_NONE;
    if (GameIsNetworked() && s_varCompressionEnabled.Get())
    {
        bDoCompress = TRUE;
        eCompressionMode = s_varCompressionDedicatedStreams.Get() ? NETWORK_COMPRESSION_MODE_CONNECTION_STREAM : NETWORK_COMPRESSION_MODE_SHARED_STREAM;
    }
    XEncoder tEncoder;
    if (GameIsNetworked() && ptGameConnection->idServerSocket != -1)
    {
        uint32 dwEncodeRoom = XServerGetMaxSendBufferSize(ptGameConnection->idServerSocket);
        float fOutputBufferFull = 0.0f;
        if (dwEncodeRoom <= s_varLimitedPacketByteThreshold.Get())
        {
            ++ptGameConnection->dwConsecutiveLimitedPacketRoomFrameCount;
            ++ptGameConnection->dwTotalLimitedPacketRoomFrameCount;
            fOutputBufferFull = 1.0f;
        }
        else
            ptGameConnection->dwConsecutiveLimitedPacketRoomFrameCount = 0;
        ptGameConnection->fOutputBufferFullRuntimeTrend = (1.0f - ptServerGlobals->fOONumFramesForFullBufferTrend) * ptGameConnection->fOutputBufferFullRuntimeTrend + ptServerGlobals->fOONumFramesForFullBufferTrend * fOutputBufferFull;
        if (dwEncodeRoom < 5)
            return TRUE;
        dwSocketRoom = dwEncodeRoom - 4;
        if (bDoCompress)
        {
            uint32 dwSocketRoomForCompression = sMaxCompressibleBytes(dwSocketRoom);
            if (dwSocketRoomForCompression)
                dwEncodeRoom = dwSocketRoomForCompression + 4;
            else
                bDoCompress = FALSE;
        }
        dwEncodeRoom = Math::Min(dwEncodeRoom, uint32(32768));
        tEncoder.SetData(NetworkGetEncodeBuffer(), dwEncodeRoom);
        tEncoder.SetUsed(32);
    }
    uint32 uNumMessages = tOutgoing.Count();
    BOOL bDoDisconnect = ptGameConnection->fOutputBufferFullRuntimeTrend > s_varOutputBufferDisconnectThreshold.Get() && uNumMessages != 0;
    if (uNumMessages > ptGameConnection->uHighestCount)
    {
        ptGameConnection->uHighestCount = uNumMessages;
        ptGameConnection->tHighTime = Blizzard::Time::ToUnixTime64(Blizzard::Time::GetTimestamp());
        if (uNumMessages >= s_varNetPacketLimit.Get())
            bDoDisconnect = TRUE;
    }
    if (bDoDisconnect)
    {
        Blizzard::Time::Timestamp tHigh = Blizzard::Time::FromUnixTime64(ptGameConnection->tHighTime);
        Blizzard::Time::TimeRec tRec;
        Blizzard::Time::BreakTime(tHigh, tRec);
        blz::unordered_map<int, unsigned int> packetCounts;
        POS posOutgoingList = tOutgoing.GetHeadPosition();
        while (posOutgoingList)
        {
            void * pCurrentMessage = tOutgoing.GetNext(posOutgoingList)->byteData;
            RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pCurrentMessage;
            blz::unordered_map<int, unsigned int>::iterator it = packetCounts.insert(blz::make_pair(ptHeader->eType, 0)).first;
            ++it->second;
        }
        blz::vector<blz::pair<int, unsigned int> > sortedPacketCounts(packetCounts.size());
        unsigned int i = 0;
        for (blz::unordered_map<int, unsigned int>::iterator it = packetCounts.begin(); it != packetCounts.end(); it++)
        {
            sortedPacketCounts[i] = *it;
            i++;
        }
        qsort(sortedPacketCounts.data(), sortedPacketCounts.size(), sizeof(blz::pair<int, unsigned int>), GameMessagePairComparator);
        const uint32 nBufferSize = 512;
        const uint32 nMsgsForError = 5;
        CHAR szBuffer[nBufferSize];
        szBuffer[0] = 0;
        int32 nMsgCount = 0;
        for (blz::vector<blz::pair<int, unsigned int> >::iterator it = sortedPacketCounts.begin(); it != sortedPacketCounts.end(); ++it)
        {
            Blizzard::String::Format(szBuffer + strlen(szBuffer), nBufferSize - strlen(szBuffer), "%s : %d", GameMessageTypeToName(it->first).str(), it->second);
            if (++nMsgCount >= nMsgsForError)
                break;
            Blizzard::String::Format(szBuffer + strlen(szBuffer), nBufferSize - strlen(szBuffer), ", ");
        }
        s_varNetPacketLimit.Get();
        // DWARF iterator at original line1323 has no surviving executable traversal.
        ActorsDumpList();
        TimersDumpList();
        ServerClientDisconnectGC(ptGameConnection->id);
        return FALSE;
    }
    for (uint32 i = 0; i < uNumMessages; ++i)
    {
        PacketBuffer * ptPacketBuffer = tOutgoing.GetHead();
        // DWARF local ptHeader (1341); the encode call below takes byteData directly (1374:58)
        RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)ptPacketBuffer->byteData;
        (void)ptHeader;
        if (!GameIsNetworked())
        {
            DisplayInternalError(NULL, "", 1347, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
        if (ptGameConnection->idServerSocket != -1)
        {
            if (!sServerEncodeMessage(ptGameConnection, ptPacketBuffer->byteData, tEncoder))
                break;
        }
        tOutgoing.RemoveHead();
        PacketBufferFree(ptServerGlobals->ptOutgoingPacketPool, ptPacketBuffer);
    }
    if (GameIsNetworked() && tEncoder.UsedBytes() > 4)
    {
        uint32 dwUsedBytes = tEncoder.UsedBytes() & 0xffffff;
        tEncoder.SetUsed(0);
        tEncoder.Put(dwUsedBytes, 32);
        if (ptGameConnection->idServerSocket != -1)
        {
            void * pData = tEncoder.Data();
            if (bDoCompress)
            {
                uint8 * pCompressionBuffer = NetworkGetCompressionBuffer();
                uint8 * pSource = (uint8 *)tEncoder.Data() + 4;
                uint32 dwSourceSize = dwUsedBytes - 4;
                uint8 * pDest = pCompressionBuffer + 4;
                uint32 dwDestSize = dwSocketRoom;
                uint32 uCompressedSize = sServerCompressBuffer(ptGameConnection, eCompressionMode, pSource, dwSourceSize, pDest, dwDestSize);
                if (uCompressedSize)
                {
                    dwUsedBytes = uCompressedSize + 4;
                    uint32 uHeader = (dwUsedBytes & 0xffffff) | (eCompressionMode == NETWORK_COMPRESSION_MODE_CONNECTION_STREAM ? 0xc0000000 : 0x80000000);
                    if (ptGameConnection->m_eLastCompressionMode != eCompressionMode)
                        uHeader |= 0x20000000;
                    XEncoder tCompressedSizeEncoder(pCompressionBuffer, 4);
                    tCompressedSizeEncoder.Put(uHeader, 32);
                    ptGameConnection->m_eLastCompressionMode = eCompressionMode;
                    pData = pCompressionBuffer;
                }
                else if (eCompressionMode == NETWORK_COMPRESSION_MODE_CONNECTION_STREAM)
                {
                    deflateReset(ptGameConnection->ptZstream);
                    ptGameConnection->m_eLastCompressionMode = NETWORK_COMPRESSION_MODE_NONE;
                }
            }
            else if (eCompressionMode == NETWORK_COMPRESSION_MODE_NONE)
                ptGameConnection->m_eLastCompressionMode = NETWORK_COMPRESSION_MODE_NONE;
            if (eCompressionMode != NETWORK_COMPRESSION_MODE_CONNECTION_STREAM && ptGameConnection->ptZstream)
            {
                deflateEnd(ptGameConnection->ptZstream);
                SigmaMemoryFree(ptGameConnection->ptZstream, NULL);
                ptGameConnection->ptZstream = NULL;
            }
            BOOL bResult = XServerSendBuffer(ptGameConnection->idServerSocket, pData, dwUsedBytes);
            (void)bResult;
        }
    }
    return TRUE;
}
#line 1505  // DWARF: ServerSendOutgoingMessages is declared at Server.cpp:1505
void ServerSendOutgoingMessages()

{


    TDataArrayIterator<GameConnection> tIt;
    while (GameConnectionDataArrayIterate(tIt))
    {
        GameConnection * ptGameConnection = tIt.ptElement;

        BOOL bConnectionStillExists = sServerSendOutgoingQueue(ptGameConnection);

        if (bConnectionStillExists && ptGameConnection->idServerSocket != -1)
        {
            XServerWriteUpdateForASocket(ptGameConnection->idServerSocket);




            if (ptGameConnection->bTerminateConnectionWhenOutgoingQueueEmpty &&
                !ptGameConnection->tOutgoingPacketBufferList.Count() &&
                !XServerSocketHasDataToSend(ptGameConnection->idServerSocket))
            {
                ServerClientDisconnectGC(ptGameConnection->id);
            }
        }
    }


}
