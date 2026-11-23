// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/Networking/XSocketBasePrivate.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../NintendoSDK/Include/nn/socket/socket_Api.h"

static uint32 WSAGetLastError() {
    return nn::socket::GetLastErrno();
}

typedef int32 SOCKET;

enum {
    WSAEWOULDBLOCK = 11,
    WSAEAFNOSUPPORT = 97,
    WSAEPROTONOSUPPORT = 93,
    WSAEPROTOTYPE = 91,
    WSAESOCKTNOSUPPORT = 94,
    WSAECONNREFUSED = 111,
    WSAENETUNREACH = 101,
    WSAEFAULT = 14,
    WSAEINVAL = 22,
    WSAEMFILE = 24,
    WSAENOBUFS = 105,
    WSAEISCONN = 106,
    WSAENOTCONN = 107,
    WSAETIMEDOUT = 110,
    WSAEINPROGRESS = 115,
    WSAENETDOWN = 100,
    WSAEADDRINUSE = 98,
    WSAEADDRNOTAVAIL = 99,
    WSAEINTR = 4,
    WSAEALREADY = 114,
    WSAENOTSOCK = 88,
    WSAEHOSTDOWN = 112,
    WSAEHOSTUNREACH = 113,
    WSAECONNRESET = 104,
    WSAECONNABORTED = 103,
};

struct XSocketBaseInternal {  // size 0x4
    SOCKET m_sock;  // 0x00
};
static_assert(sizeof(XSocketBaseInternal) == 0x4, "XSocketBaseInternal");
