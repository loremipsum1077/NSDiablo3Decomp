// Original file: X/Client/Network/XClientSocket.cpp
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "XClientSocket.h"
#include "../../Common/Game.h"
#include "../../Common/ThreadGlobals.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Networking/XNetAddress.h"
#include "../Gfx/GfxVB.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"

namespace Console { namespace Online {
GameNetwork::ConnectionRef GameGetClientGameConnectionCheckID(uint32 idConnection);  // X/Client/Console/Online.cpp
BOOL PartyIsHost();  // X/Client/Console/Online.cpp:3857
void PartyLeaveAndRemake(uint32 bOffline);  // X/Client/Console/Online.cpp
} }

//@ 45
static const uint32 CONNECTION_TIMEOUT = 3600;
static const uint32 SEND_KEEPALIVE_TIME = 900;       // invented names: only CONNECTION_TIMEOUT is in the DWARF
static const uint32 RECEIVE_KEEPALIVE_TIME = 2700;

//@ 60
static BOOL sSocketMsgHandler(intptr idUpcallHandle, SocketEvent eEvent, void * pParam)



{
    XClientSocket * ptClientSocket = (XClientSocket *)idUpcallHandle;


    switch (eEvent)
    {
    case SOCKETEVENT_RECEIVED_DATA:
        {
            uint32 dwNow = AppGetTicks();
            ptClientSocket->dwTimeReceiveKeepAlive = dwNow + RECEIVE_KEEPALIVE_TIME;



            if (ptClientSocket->pfnClientMsgHandler)
            {
                SocketReceivedData * pSocketData = (SocketReceivedData *)pParam;
                pSocketData->dwConsumed = pSocketData->dwSize;
            }
        }
        break;
    case SOCKETEVENT_DISCONNECTED:
        {
            ThreadGlobals & tThreadGlobals = ThreadGlobals::Get();
            if (tThreadGlobals.m_ptInflateZstream)
            {
                inflateEnd(tThreadGlobals.m_ptInflateZstream);
                SigmaMemoryFree(tThreadGlobals.m_ptInflateZstream, NULL); tThreadGlobals.m_ptInflateZstream = NULL;
            }
        }
        break;
    }
    if (ptClientSocket->pfnClientMsgHandler)
    {
        return ptClientSocket->pfnClientMsgHandler(ptClientSocket->idUpcallHandle, eEvent, pParam);
    }


    return TRUE;
}

static void sClientSocketClose(XClientSocket & tClientSocket);

//@ 105
BOOL XClientSocketInit(XClientSocket & tClientSocket, intptr idUpcallHandle, SocketMessageHandlerProc pfnMsgHandler)



{
    SigmaMemoryClear(&tClientSocket, sizeof(tClientSocket));




    tClientSocket.ptConnection = NULL;


    tClientSocket.bInitialized = TRUE;
    tClientSocket.idUpcallHandle = idUpcallHandle;
    tClientSocket.pfnClientMsgHandler = pfnMsgHandler;

    tClientSocket.dwTimeSendKeepAlive = 0;
    tClientSocket.dwTimeReceiveKeepAlive = 0;




    return TRUE;
}

//@ 134
void XClientSocketDispose(XClientSocket & tClientSocket)

{
    sClientSocketClose(tClientSocket);
}

//@ 143
BOOL XClientSocketConnect(XClientSocket & tClientSocket, const XNetAddress & tAddress)


{
























    tClientSocket.idConnection = tAddress.Node();
    tClientSocket.ptConnection = NULL;

    uint32 dwNow = AppGetTicks();
    tClientSocket.dwTimeSendKeepAlive = dwNow + SEND_KEEPALIVE_TIME;
    tClientSocket.dwTimeReceiveKeepAlive = dwNow + RECEIVE_KEEPALIVE_TIME;


    tClientSocket.dwTimeConnectionTimeout = dwNow + CONNECTION_TIMEOUT;

    return TRUE;
}

//@ 195
BOOL XClientSocketSendBuffer(XClientSocket & tClientSocket, void * pMessage, uint32 dwSize)
{
    return tClientSocket.ptConnection->Send(pMessage, dwSize);
}

//@ 212
uint32 XClientSocketGetMaxSendBufferSize(XClientSocket & tClientSocket)
{
    return tClientSocket.ptConnection->GetMaxSendBufferSize();
}

//@ 294
void XClientSocketWriteUpdate(XClientSocket & tClientSocket)
{

    if (!tClientSocket.bInitialized)
    {
        return;
    }

//@ 363
    if (!tClientSocket.ptConnection)
    {
        uint32 dwNow = AppGetTicks();

        tClientSocket.ptConnection = Console::Online::GameGetClientGameConnectionCheckID(tClientSocket.idConnection);

        if (!tClientSocket.ptConnection)
        {
            if (dwNow >= tClientSocket.dwTimeConnectionTimeout)
            {

                sClientSocketClose(tClientSocket);
            }
            return;
        }


        tClientSocket.dwTimeSendKeepAlive = dwNow + SEND_KEEPALIVE_TIME;
        tClientSocket.dwTimeReceiveKeepAlive = dwNow + RECEIVE_KEEPALIVE_TIME;

        sSocketMsgHandler((intptr)&tClientSocket, SOCKETEVENT_CONNECTED, NULL);
    }

    BOOL bResult = tClientSocket.ptConnection->Flush();


    if (!bResult)
    {
        sClientSocketClose(tClientSocket);
    }
}

//@ 398
static void sClientSocketClose(XClientSocket & tClientSocket)
{

    if (!tClientSocket.bInitialized)
    {
        return;
    }




    tClientSocket.ptConnection = NULL;

    tClientSocket.bInitialized = FALSE;


    sSocketMsgHandler((intptr)&tClientSocket, SOCKETEVENT_DISCONNECTED, NULL);
}

//@ 421
void XClientSocketUpdate(XClientSocket & tClientSocket)
{

    if (!tClientSocket.bInitialized)
    {
        return;
    }

    uint32 dwNow = AppGetTicks();

//@ 456
    if (tClientSocket.ptConnection &&
        !tClientSocket.ptConnection->IsConnected() &&
        !tClientSocket.ptConnection->IsMigrated())
    {
        sClientSocketClose(tClientSocket);
        return;
    }

    if (!XClientSocketIsConnected(tClientSocket))
    {
        return;
    }


    if (!tClientSocket.ptConnection->Receive((intptr)&tClientSocket, sSocketMsgHandler))
    {






        if (!Console::Online::PartyIsHost())
        {
            tClientSocket.ptConnection->Close();
            tClientSocket.ptConnection = NULL;
        }
        else
        {
            sClientSocketClose(tClientSocket);
            Console::Online::PartyLeaveAndRemake(TRUE);
        }
        return;
    }


//@ 491
    BOOL bHasDataToSend = tClientSocket.ptConnection->HasDataToSend();


    if (bHasDataToSend)
    {
        tClientSocket.dwTimeSendKeepAlive = dwNow + SEND_KEEPALIVE_TIME;
    }
    else if (dwNow >= tClientSocket.dwTimeSendKeepAlive && XClientSocketIsConnected(tClientSocket))
    {
        sSocketMsgHandler((intptr)&tClientSocket, SOCKETEVENT_SEND_KEEPALIVE, NULL);
        tClientSocket.dwTimeSendKeepAlive = dwNow + SEND_KEEPALIVE_TIME;
    }
}

//@ 508
BOOL XClientSocketIsConnected(XClientSocket & tClientSocket)





{
    return tClientSocket.ptConnection && tClientSocket.ptConnection->IsConnected();
}
