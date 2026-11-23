// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Networking/XSocket.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../ThreadGlobals.h"
#include "../../Sigma/Networking/XSocketBase.h"

enum {
    SOCKET_RECEIVE_BUFFER_SIZE = 32768,
    SOCKET_SEND_BUFFER_SIZE = 32768,
    PORT_ADDRESS = 6112,
    LOUNGE_PORT_ADDRESS = 6112,
    LOCAL_PLAYER_PORT = 6120,
    TELNET_CLIENT_CONSOLE_PORT_ADDRESS = 1233,
    PROF_SERVER_PORT_ADDRESS = 1243,
};

enum SocketState {
    SOCKETSTATE_CONNECTED = 0,
    SOCKETSTATE_DISCONNECTED = 1,
    SOCKETSTATE_LISTENING = 2,
};

enum SocketEvent {
    SOCKETEVENT_CONNECTED = 0,
    SOCKETEVENT_DISCONNECTED = 1,
    SOCKETEVENT_RECEIVED_DATA = 2,
    SOCKETEVENT_SEND_KEEPALIVE = 3,
};

enum SocketHandleReadEventRes {
    SHRER_UNDEFINED = 0,
    SHRER_OK = 1,
    SHRER_RECV_FAILURE = 2,
    SHRER_ULEN_ZERO = 3,
    SHRER_ERROR_PARSING = 4,
};

typedef BOOL (*SocketMessageHandlerProc)(intptr, SocketEvent, void *);

struct SocketReceivedData {  // size 0x10
    uint8 * lpMsg;  // 0x00
    uint32 dwSize;  // 0x08
    uint32 dwConsumed;  // 0x0c
};
static_assert(sizeof(SocketReceivedData) == 0x10, "SocketReceivedData");

struct XSocket {  // size 0x50
    XSocketBase tBaseSocket;  // 0x00
    SocketState eState;  // 0x10
    uint8 * pbReceiveBuffer;  // 0x18
    uint32 dwReceiveBufferSize;  // 0x20
    uint8 * pbSendBuffer;  // 0x28
    int32 dwSendBufferSize;  // 0x30
    SocketMessageHandlerProc pfnMsgHandler;  // 0x38
    intptr idUpcallHandle;  // 0x40
    DataID idPollHandle;  // 0x48
};
static_assert(sizeof(XSocket) == 0x50, "XSocket");

void WinSockInit();
void WinSockDispose();
void XSocketClose(XSocket & tSocket);
BOOL XSocketGetMyIPAddress(XNetAddress & tAddress);

