// Original file: X/Server/Network/XServerSocket.cpp
// The trace output of the bandwidth statistics (sServerSocketPrintBandwidthStats, XServerDisposeForApp)
// and the keep-alive/timeout code that read the XVars below are not in this build.
#include "XServerSocket.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Console/Networking/Connection.h"
#include "../../Common/Game.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/Networking/XNetAddress.h"

enum {
    LISTENER_BACKLOG = 3,
    INITIAL_SERVER_SOCKET_COUNT = 8,
    MAX_SERVER_SOCKETS = 8,
};

struct ServerSocket {  // size 0x50
    Console::GameNetwork::ConnectionRef ptConnection;  // 0x00
    XNetAddress tAddress;  // 0x08
    uint32 uTimeReceiveKeepAlive;  // 0x18
    uint32 uLastMeaningfulMessageReceivedTime;  // 0x1c
    SGameID idSGame;  // 0x20
    BOOL bDisconnecting;  // 0x24
    uint32 dwMsgPermissions;  // 0x28
    uint32 dwConnectTime;  // 0x2c
    uint32 dwBytesReceived;  // 0x30
    uint32 dwBytesSent;  // 0x34
    uint32 dwFrameBytesReceived;  // 0x38
    uint32 dwFrameBytesSent;  // 0x3c
    OnlineService::GameAccountHandle tAccountId;  // 0x40
};
static_assert(sizeof(ServerSocket) == 0x50, "ServerSocket");

struct ServerSocketContainer {  // size 0x10
    ServerSocketContainer() { id = SERVERSOCKETID_INVALID; ptServerSocket = NULL; }  // line 107; body from TDataArray<ServerSocketContainer, int>::ElementNew
    ServerSocketID id;  // 0x00
    ServerSocket * ptServerSocket;  // 0x08
};
static_assert(sizeof(ServerSocketContainer) == 0x10, "ServerSocketContainer");

struct XServerAppGlobals {  // size 0x30
    BOOL bListening;  // 0x00
    TDataArray<ServerSocketContainer, int> * ptServerSockets;  // 0x08
    XChainedFixedBlockAllocator<ServerSocket> * ptSocketPool;  // 0x10
    uint32 dwStartTime;  // 0x18
    uint32 dwTotalBytesReceived;  // 0x1c
    uint32 dwTotalBytesSent;  // 0x20
    uint32 dwFrameBytesReceived;  // 0x24
    uint32 dwFrameBytesSent;  // 0x28
};
static_assert(sizeof(XServerAppGlobals) == 0x30, "XServerAppGlobals");

void XServerSocketDisconnect( ServerSocketID idServerSocket, SocketDisconnectReason eReason );
#include "../SockAddr.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../SGameManager.h"
#include "../../Common/Networking/XSocket.h"
#include "../../Common/Networking/Network.h"
#include "../../Sigma/Networking/XDecoder.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/DataStructures/XVar.h"
namespace Console { namespace Online { GameNetwork::ConnectionRef GameGetServerPendingConnection(); } }  // X/Client/Console/ConsoleGameConnection.cpp:156
void ServerClientDisconnect( ServerSocketID idServerSocket );  // X/Server/Network/Server.cpp:731
void ServerEventReceive( ServerSocketID idServerSocket, uint8 * pMessage );  // X/Server/Network/Server.cpp:455
BOOL ServerMessageReceiveOutsideGame( ServerSocketID idServerSocket, void * pMessage );  // X/Server/Network/ServerMessageReceive.cpp:206
BOOL NetworkParseGameDataBlock( intptr idUpcallHandle, SocketReceivedData & tSocketData, NetworkGameDataBlockHandlerProc pHandlerFcn );  // X/Common/Networking/Network.cpp:386
uint8 * NetworkGetDecodeBuffer();  // X/Common/Networking/Network.cpp:518
XDecoder::Result DecodeMessage( GameMessageType eMessageType, void * pDecodeToBuffer, uint32 dwBufferSize, XDecoder & tDecoder );  // X/Common/Networking/GameMessage.cpp:837
void XServerSocketFinishDisconnect( ServerSocketID idServerSocket, SocketDisconnectReason eReason, uint32 gameId ); void XServerStopListener();

static XVarScope s_scopeXServerSocket( "XServerSocket", NULL );

static XVarBool s_varKeepAliveEnabled( s_scopeXServerSocket, "KeepAliveTimeoutEnabled", true );



static XVarUint32 s_varGamelessSocketTimeout( s_scopeXServerSocket, "GamelessSocketTimeout", 10 );
static XVarUint32 s_varInactivityTimeout( s_scopeXServerSocket, "InactivityTimeout", 1800 );
static XVarUint32 s_varFetchPublicIPTimeout( s_scopeXServerSocket, "FetchPublicIPTimeout", 5 );
static XVarBool s_varAllowPublicIPFetch( s_scopeXServerSocket, "AllowPublicIPFetch", true );























































static ServerSocketContainer & sServerSocketContainerGet( ServerSocketID idServerSocket )
{
    return *sgtAppGlobals.ptXServerAppGlobals->ptServerSockets->ElementGet( idServerSocket );
}





static ServerSocketContainer * sServerSocketContainerTryToGet( ServerSocketID idServerSocket )
{
    if ( idServerSocket == SERVERSOCKETID_INVALID )
        return NULL;
    return sgtAppGlobals.ptXServerAppGlobals->ptServerSockets->ElementTryToGet( idServerSocket );
}



static ServerSocket & sServerSocketGet( ServerSocketID idServerSocket )
{

    return *sServerSocketContainerGet( idServerSocket ).ptServerSocket;
}

static ServerSocket * sServerSocketTryToGet( ServerSocketID idServerSocket )
{

    ServerSocketContainer * ptSocketContainer = sServerSocketContainerTryToGet( idServerSocket );
    if ( ptSocketContainer != NULL )
    {
        return ptSocketContainer->ptServerSocket;
    }
    return NULL;
}




static void sServerSocketPrintBandwidthStats( ServerSocket * ptServerSocket )
{


    uint32 dwNow = AppGetTicks();
    // dwTimeConnected, fSecondsConnected, fSentkbps and fReceivedkbps (lines 183-186) fed a trace.
}













BOOL XServerSocketIDValid( ServerSocketID idServerSocket )
{
    return sServerSocketContainerTryToGet( idServerSocket ) != NULL;
}





ServerSocketContainer * ServerSocketContainerDataArrayIterate( TDataArrayIterator<ServerSocketContainer> & tIterator )
{


    XServerAppGlobals * ptXServerAppGlobals = sgtAppGlobals.ptXServerAppGlobals;

    return ptXServerAppGlobals->ptServerSockets->Iterate( tIterator );
}




void XServerInitializeForApp()
{

    AppGlobals & tAppGlobals = sgtAppGlobals;
    XServerAppGlobals *& ptXServerAppGlobals = tAppGlobals.ptXServerAppGlobals;

    ptXServerAppGlobals = (XServerAppGlobals *)SigmaMemoryNew( sizeof( XServerAppGlobals ), 4, NULL, TRUE );





    ptXServerAppGlobals->ptServerSockets = TDataArrayCreateTracked<ServerSocketContainer, ServerSocketID>( "ServerSocket", NULL, INITIAL_SERVER_SOCKET_COUNT );

    ptXServerAppGlobals->ptSocketPool = new XChainedFixedBlockAllocator<ServerSocket>;
    ptXServerAppGlobals->ptSocketPool->SetBlockCount_Tracked( INITIAL_SERVER_SOCKET_COUNT );
    ptXServerAppGlobals->ptSocketPool->SetGrow( XALLOC_GROW_LINEAR, MAX_SERVER_SOCKETS );





    ptXServerAppGlobals->bListening = FALSE;

    ptXServerAppGlobals->dwStartTime = AppGetTicks();
}




void XServerSocketDisconnect( ServerSocketID idServerSocket, SocketDisconnectReason eReason )
{


    ServerSocket * ptServerSocket = sServerSocketTryToGet( idServerSocket );

    if ( ptServerSocket == NULL )
    {
        return;
    }

    if ( ptServerSocket->bDisconnecting )
    {
        return;
    }

    ptServerSocket->bDisconnecting = TRUE;

    if ( SGameManager::GameIsValid( ptServerSocket->idSGame ) )
    {

        BOOL bReturnToClient = FALSE;
        if ( GameActiveGameCommonDataIsClient() )
        {
            bReturnToClient = GameServerCodeEnter_Track() || _GameServerCodeEnter( ptServerSocket->idSGame );
        }

        ServerClientDisconnect( idServerSocket );

        GameServerCodeLeave_Tracked( bReturnToClient );
    }
    else
    {

        XServerSocketFinishDisconnect( idServerSocket, eReason, ptServerSocket->idSGame );
    }
}



















































void XServerSocketFinishDisconnect( ServerSocketID idServerSocket, SocketDisconnectReason eReason, uint32 gameId )
{




    ServerSocketContainer * ptServerSocketContainer = sServerSocketContainerTryToGet( idServerSocket );
    if ( ptServerSocketContainer == NULL )
    {
        return;
    }

    ServerSocket * ptServerSocket = ptServerSocketContainer->ptServerSocket;



    sServerSocketPrintBandwidthStats( ptServerSocket );

    AppGlobals & tAppGlobals = sgtAppGlobals;
    XServerAppGlobals *& ptXServerAppGlobals = tAppGlobals.ptXServerAppGlobals;

    ptServerSocket->ptConnection->Close();

    ptXServerAppGlobals->ptSocketPool->Free( ptServerSocket );

    ptXServerAppGlobals->ptServerSockets->ElementDelete( ptServerSocketContainer );
}




















void XServerDisposeForApp()
{

    AppGlobals & tAppGlobals = sgtAppGlobals;
    XServerAppGlobals *& ptXServerAppGlobals = tAppGlobals.ptXServerAppGlobals;

    XServerStopListener();

    TDataArrayIterator<ServerSocketContainer> tIt;
    while ( ServerSocketContainerDataArrayIterate( tIt ) )
    {
        ServerSocketContainer * ptServerSocketContainer = tIt.ptElement;

        XServerSocketDisconnect( ptServerSocketContainer->id, SDR_DISPOSE );
    }

    TDataArrayDispose( ptXServerAppGlobals->ptServerSockets );
    if ( ptXServerAppGlobals->ptSocketPool ) { delete ptXServerAppGlobals->ptSocketPool; } ptXServerAppGlobals->ptSocketPool = NULL;










    uint32 dwNow = AppGetTicks();
    uint32 dwTimeRunning = dwNow - ptXServerAppGlobals->dwStartTime;
    // fSecondsRan, fSentkbps and fReceivedkbps (lines 414-416) fed a trace.





    SigmaMemoryFree( ptXServerAppGlobals, NULL ); ptXServerAppGlobals = NULL;
}






BOOL XServerStartListener()
{
    sgtAppGlobals.ptXServerAppGlobals->bListening = TRUE;
    return TRUE;
}




































































































































































































































































void XServerStopListener()
{
    sgtAppGlobals.ptXServerAppGlobals->bListening = FALSE;
}

















BOOL XServerIsListening()
{
    return sgtAppGlobals.ptXServerAppGlobals->bListening;
}








BOOL XServerSendBuffer( ServerSocketID idServerSocket, void * pMessage, uint32 dwSize )
{






    ServerSocket & tServerSocket = sServerSocketGet( idServerSocket );




    BOOL bResult = tServerSocket.ptConnection->Send( pMessage, dwSize );
    if ( bResult )
    {
        tServerSocket.dwBytesSent += dwSize;
        tServerSocket.dwFrameBytesSent += dwSize;
    }
    return bResult;
}





uint32 XServerGetMaxSendBufferSize( ServerSocketID idServerSocket )
{
    ServerSocket & tServerSocket = sServerSocketGet( idServerSocket );
    return tServerSocket.ptConnection->GetMaxSendBufferSize();
}












static BOOL sXServerReceive( intptr idUpcallHandle, uint8 * pMessage, uint32 dwSize, BOOL & bContinueParsing )
{




    uint8 * pauDecodedMessage = NetworkGetDecodeBuffer();
    ServerSocket * ptServerSocket = sServerSocketTryToGet( (ServerSocketID)idUpcallHandle );

    if ( ptServerSocket == NULL )
    {
        return FALSE;
    }



    uint32 dwNow = AppGetTicks();
    ptServerSocket->uTimeReceiveKeepAlive = dwNow + 2700;




    ptServerSocket->dwBytesReceived += dwSize;
    ptServerSocket->dwFrameBytesReceived += dwSize;

    XDecoder tDecoder( pMessage, dwSize );

    while ( tDecoder.Used() < tDecoder.Size() )
    {
        uint32 dwUsedBits = tDecoder.Used();




        RequiredMessageHeader tHeader;
        TypeDescriptorField field( DT_REQUIRED_MESSAGE_HEADER, NULL );
        XDecoder::Result eResult = DT_REQUIRED_MESSAGE_HEADER->Decode( &tHeader, field, tDecoder );
        if ( eResult == XDecoder::RESULT_CORRUPTED )
        {
            return FALSE;
        }




        if ( eResult == XDecoder::RESULT_TRUNCATED ||
             tHeader.eType == 0 )  // MSG_INVALID
        {



            return ( tDecoder.Size() - dwUsedBits ) < 8;
        }

        if ( tHeader.eType == 217 )  // CMSG_KEEPALIVE
        {
            continue;
        }












        if ( tHeader.eType >= 585 )  // MSG_RANGE_INGAME_END
        {
            return FALSE;
        }







        tDecoder.SetUsed( dwUsedBits );
        eResult = DecodeMessage( tHeader.eType, pauDecodedMessage, 0x8000, tDecoder );
        if ( eResult != XDecoder::RESULT_VALID )
        {
            return FALSE;
        }









        BOOL bProcessed = ServerMessageReceiveOutsideGame( (ServerSocketID)idUpcallHandle, pauDecodedMessage );

        if ( bProcessed )
        {





            if ( !XServerSocketIDValid( (ServerSocketID)idUpcallHandle ) || !ptServerSocket->ptConnection->IsConnected() )
            {


                bContinueParsing = FALSE;
                return TRUE;
            }
        }
        else
        {


            BOOL bValidSGame = SGameManager::GameIsValid( ptServerSocket->idSGame );

            if ( !bValidSGame )
                return FALSE;
            BOOL bReturnToClient = GameServerCodeEnter_Track() || _GameServerCodeEnter( ptServerSocket->idSGame );
            ServerEventReceive( (ServerSocketID)idUpcallHandle, pauDecodedMessage );

            GameServerCodeLeave_Tracked( bReturnToClient );
        }
    }



    return TRUE;
}



static BOOL sServerSocketMsgHandler( intptr idUpcallHandle, SocketEvent eEvent, void * pParam )
{



    switch ( eEvent )
    {
    case SOCKETEVENT_CONNECTED:
        break;

    case SOCKETEVENT_DISCONNECTED:
        break;

    case SOCKETEVENT_RECEIVED_DATA:
        {
            SocketReceivedData * ptSocketData = (SocketReceivedData *)pParam;
            if ( !NetworkParseGameDataBlock( idUpcallHandle, *ptSocketData, sXServerReceive ) )
            {
                return FALSE;
            }
        }
        break;

    case SOCKETEVENT_SEND_KEEPALIVE:
        break;

    default:
        DisplayInternalError( NULL, "", 933, ERROR_INTERNAL_FATAL ); ExitProgram( ERROR_INTERNAL_FATAL );
    }

    return TRUE;
}






































































































static void XServerSocketUpdate()
{

    AppGlobals & tAppGlobals = sgtAppGlobals;
    XServerAppGlobals * ptXServerAppGlobals = tAppGlobals.ptXServerAppGlobals;

    if ( !ptXServerAppGlobals->bListening )
    {
        return;
    }


















































    while ( TRUE )
    {
        Console::GameNetwork::ConnectionRef ptConnection = Console::Online::GameGetServerPendingConnection();
        if ( !ptConnection )
        {
            break;
        }

        ServerSocketID idServerSocket = ptXServerAppGlobals->ptServerSockets->ElementNew();

        if ( idServerSocket == SERVERSOCKETID_INVALID )
        {

            ptConnection->Close();
            return;
        }

        ServerSocketContainer & tServerSocketContainer = sServerSocketContainerGet( idServerSocket );
        tServerSocketContainer.ptServerSocket = ptXServerAppGlobals->ptSocketPool->Alloc_Tracked();
        ServerSocket & tServerSocket = *tServerSocketContainer.ptServerSocket;

        tServerSocket.ptConnection = ptConnection;
        ptConnection->GetAddress( tServerSocket.tAddress );

        uint32 dwNow = AppGetTicks();

        tServerSocket.uTimeReceiveKeepAlive = dwNow + 2700;
        tServerSocket.uLastMeaningfulMessageReceivedTime = dwNow;
        tServerSocket.idSGame = -1;
        tServerSocket.bDisconnecting = FALSE;


        tServerSocket.dwMsgPermissions = ( 1 << MSG_PERMISSIONS_ANYTIME_BIT ) | ( 1 << MSG_PERMISSIONS_PREGAME_BIT );
        tServerSocket.dwConnectTime = dwNow;



        tServerSocket.dwBytesReceived = 0; tServerSocket.dwBytesSent = 0; tServerSocket.dwFrameBytesReceived = 0; tServerSocket.dwFrameBytesSent = 0;




    }


    TDataArrayIterator<ServerSocketContainer> tIt;
    while ( ServerSocketContainerDataArrayIterate( tIt ) )
    {
        ServerSocketContainer * ptServerSocketContainer = tIt.ptElement;
        Console::GameNetwork::ConnectionRef ptConnection = ptServerSocketContainer->ptServerSocket->ptConnection;
        if ( !ptConnection->IsConnected() ||
             !ptConnection->Receive( ptServerSocketContainer->id, sServerSocketMsgHandler ) )
        {
            XServerSocketDisconnect( ptServerSocketContainer->id, SDR_UPDATE_FAILED );
        }
    }
}


void XServerReadUpdate()
{









































































    XServerSocketUpdate();

}




void XServerWriteUpdateForASocket( ServerSocketID idServerSocket )
{
    ServerSocket & tServerSocket = sServerSocketGet( idServerSocket );
    if ( !tServerSocket.ptConnection->Flush() )
    {
        XServerSocketDisconnect( idServerSocket, SDR_WRITE_FAILED );
    }
}












void XServerSocketSetSGameID( ServerSocketID idServerSocket, SGameID idSGame )
{
    ServerSocket * ptServerSocket = sServerSocketTryToGet( idServerSocket );
    if ( ptServerSocket != NULL )
    {
        ptServerSocket->idSGame = idSGame;
    }
}











SGameID XServerSocketGetSGameID( ServerSocketID idServerSocket )
{
    ServerSocket * ptServerSocket = sServerSocketTryToGet( idServerSocket );
    if ( ptServerSocket != NULL )
    {
        return ptServerSocket->idSGame;
    }
    return -1;
}





BOOL XServerSocketGetPeerAddress( ServerSocketID idServerSocket, XNetAddress & tPeerAddress )
{




    ServerSocket * ptServerSocket = sServerSocketTryToGet( idServerSocket );
    if ( ptServerSocket != NULL )
    {
        tPeerAddress = ptServerSocket->tAddress;
        return TRUE;
    }
    return FALSE;
}


























BOOL XServerSocketHasDataToSend( ServerSocketID idServerSocket )
{
    ServerSocket * ptServerSocket = sServerSocketTryToGet( idServerSocket );
    if ( ptServerSocket != NULL )
    {
        return ptServerSocket->ptConnection->HasDataToSend();
    }
    return FALSE;
}










uint32 XServerSocketGetMsgPermissions( ServerSocketID idServerSocket )
{
    ServerSocket * ptServerSocket = sServerSocketTryToGet( idServerSocket );
    if ( ptServerSocket != NULL )
    {
        return ptServerSocket->dwMsgPermissions;
    }
    return 0;
}







uint32 XServerSocketSetMsgPermissions( ServerSocketID idServerSocket, uint32 dwNewPermissionsMask )
{
    ServerSocket * ptServerSocket = sServerSocketTryToGet( idServerSocket );
    if ( ptServerSocket != NULL )
    {
        ptServerSocket->dwMsgPermissions |= dwNewPermissionsMask;
        return ptServerSocket->dwMsgPermissions;
    }
    return 0;
}









uint32 XServerSocketClearMsgPermissions( ServerSocketID idServerSocket, uint32 dwPermissionsToClearMask )
{
    ServerSocket * ptServerSocket = sServerSocketTryToGet( idServerSocket );
    if ( ptServerSocket != NULL )
    {
        ptServerSocket->dwMsgPermissions &= ~dwPermissionsToClearMask;
        return ptServerSocket->dwMsgPermissions;
    }
    return 0;
}






















BOOL XServerSocketSetAccountId( ServerSocketID idServerSocket, const OnlineService::GameAccountHandle & tAccountId )
{
    ServerSocket * ptServerSocket = sServerSocketTryToGet( idServerSocket );
    if ( ptServerSocket != NULL )
    {
        ptServerSocket->tAccountId = tAccountId;
        return TRUE;
    }
    return FALSE;
}


























BOOL XServerSocketMarkMeaningfulMessage( ServerSocketID idServerSocket )
{



    ServerSocket * ptServerSocket = sServerSocketTryToGet( idServerSocket );
    if ( ptServerSocket != NULL )
    {
        ptServerSocket->uLastMeaningfulMessageReceivedTime = AppGetTicks();
        return TRUE;
    }
    return FALSE;
}






Console::GameNetwork::ConnectionRef XServerSocketGetGameConnection( ServerSocketID idServerSocket )
{



    ServerSocket * ptServerSocket = sServerSocketTryToGet( idServerSocket );
    if ( ptServerSocket == NULL )
    {
        return Console::GameNetwork::ConnectionRef( NULL );
    }

    return ptServerSocket->ptConnection;
}
