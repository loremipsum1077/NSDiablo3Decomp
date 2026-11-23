// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Network/XClientSocket.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/Console/Networking/Connection.h"
#include "../../Common/Networking/XSocket.h"

struct XClientSocket {  // size 0x30
    uint32 idConnection;  // 0x00
    Console::GameNetwork::ConnectionRef ptConnection;  // 0x08
    BOOL bInitialized;  // 0x10
    uint32 dwTimeConnectionTimeout;  // 0x14
    uint32 dwTimeSendKeepAlive;  // 0x18
    uint32 dwTimeReceiveKeepAlive;  // 0x1c
    intptr idUpcallHandle;  // 0x20
    SocketMessageHandlerProc pfnClientMsgHandler;  // 0x28
};
static_assert(sizeof(XClientSocket) == 0x30, "XClientSocket");

BOOL XClientSocketInit(XClientSocket & tClientSocket, intptr idUpcallHandle, SocketMessageHandlerProc pfnMsgHandler);
void XClientSocketDispose(XClientSocket & tClientSocket);
BOOL XClientSocketConnect(XClientSocket & tClientSocket, const XNetAddress & tAddress);
BOOL XClientSocketSendBuffer(XClientSocket & tClientSocket, void * pMessage, uint32 dwSize);
uint32 XClientSocketGetMaxSendBufferSize(XClientSocket & tClientSocket);
void XClientSocketWriteUpdate(XClientSocket & tClientSocket);
void XClientSocketUpdate(XClientSocket & tClientSocket);
BOOL XClientSocketIsConnected(XClientSocket & tClientSocket);
