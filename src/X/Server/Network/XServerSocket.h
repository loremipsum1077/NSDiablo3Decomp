// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Network/XServerSocket.h
// Subset: the types and the functions SGameManager.cpp calls (SNetLounge.h include left out).

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"

enum {
    SERVERSOCKETID_INVALID = -1,
};

namespace OnlineService { struct GameAccountHandle; }
typedef int32 ServerSocketID;

enum {
    MSG_PERMISSIONS_ANYTIME_BIT = 0,
    MSG_PERMISSIONS_PREGAME_BIT = 1,
    MSG_PERMISSIONS_INGAME_BIT = 2,
};

enum SocketDisconnectReason {
    SDR_NONE = 0,
    SDR_GAME_CONN_DEL = 1,
    SDR_SEND_BUFFER_FULL = 2,
    SDR_MSG_QUEUE_FULL = 3,
    SDR_SEND_FAILED = 4,
    SDR_ACCOUNT_FAILED = 5,
    SDR_JOIN_FAILED = 6,
    SDR_GAME_FAILED = 7,
    SDR_INVALID_SOCKET_ID = 8,
    SDR_OUT_OF_GAME = 9,
    SDR_BNET = 10,
    SDR_MATCHMAKING = 11,
    SDR_GAME_NAME = 12,
    SDR_BAD_ACT = 13,
    SDR_SOCKET_OWNER = 14,
    SDR_PROTOCOL_VERSION = 15,
    SDR_PRODUCTION = 16,
    SDR_LAN = 17,
    SDR_AUTH = 18,
    SDR_PERMISSIONS = 19,
    SDR_UNKNOWN_MESSAGE = 20,
    SDR_DISPOSE = 21,
    SDR_UPDATE_FAILED = 22,
    SDR_KEEPALIVE_TIMEOUT = 23,
    SDR_GAMELESS_TIMEOUT = 24,
    SDR_GAME_TIMEOUT = 25,
    SDR_WRITE_FAILED = 26,
    SDR_INVALID_GAME = 27,
    SDR_CLEANUP = 28,
    SDR_BOOT = 29,
};

struct SocketAndReason {  // size 0x8
    ServerSocketID socketId;  // 0x00
    SocketDisconnectReason reason;  // 0x04
};
static_assert(sizeof(SocketAndReason) == 0x8, "SocketAndReason");

void XServerSocketDisconnect(ServerSocketID idServerSocket, SocketDisconnectReason eReason);  // XServerSocket.cpp:249
void XServerSocketFinishDisconnect(ServerSocketID idServerSocket, SocketDisconnectReason eReason, uint32 gameId);  // XServerSocket.cpp:337
void XServerSocketSetSGameID(ServerSocketID idServerSocket, SGameID idSGame);  // XServerSocket.cpp:1259
SGameID XServerSocketGetSGameID(ServerSocketID idServerSocket);  // XServerSocket.cpp:1278
uint32 XServerSocketClearMsgPermissions(ServerSocketID idServerSocket, uint32 dwPermissionsToClearMask);  // XServerSocket.cpp:1386
BOOL XServerSocketSetAccountId(ServerSocketID idServerSocket, const OnlineService::GameAccountHandle & tAccountId);  // XServerSocket.cpp:1418
