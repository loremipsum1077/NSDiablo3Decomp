// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformSocket/bdPlatformSocket.h

#pragma once

#include "../bdPlatformConfig/bdPlatformConfig-switch.h"
#include "../bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdSockAddr.h"

enum bdSocketStatusCode {
    BD_NET_SUCCESS = 1,
    BD_NET_CONNECTION_CLOSED = 0,
    BD_NET_ERROR = -1,
    BD_NET_WOULD_BLOCK = -2,
    BD_NET_SUBSYTEM_ERROR = -3,
    BD_NET_ADDRESS_IN_USE = -4,
    BD_NET_CONNECTION_RESET = -5,
    BD_NET_MSG_SIZE = -6,
    BD_NET_ADDRESS_INVALID = -7,
    BD_NET_BLOCKING_CALL_CANCELED = -8,
    BD_NET_NOT_BOUND = -9,
    BD_NET_INVALID_HANDLE = -10,
    BD_NET_NOT_CONNECTED = -11,
    BD_NET_CONNECTION_REFUSED = -12,
    BD_NET_HOST_UNREACH = -13,
};

class bdPlatformSocket {  // size 0x1
public:
    static bdInt create(const bdBool blocking, const bdBool broadcast);
    static bdInt create(const bdUInt, const bdBool, const bdBool);
    static bdSocketStatusCode bind(bdInt & handle, const bdInAddr addr, const bdUInt16 port);
    static bdInt sendTo(const bdInt handle, const bdInAddr addr, const bdUInt16 port, const void * data, const bdUInt length);
    static bdInt receiveFrom(const bdInt handle, bdInAddr & addr, bdUInt16 & port, void * data, const bdUInt size);
    static bdBool close(bdInt & handle);
    static void unregisterThread();
    static void unregisterThread(bdInt);
    static bdUInt64 getBytesSent();
    static bdUInt64 getBytesReceived();
    static bdUInt64 getPacketsSent();
    static bdUInt64 getPacketsRecvd();
    static bdDNSHandle getDNSHandle(void *, bdInt);
    static bdUInt getHostByName(const bdNChar8 *const name, bdInAddr * addresses, bdUInt numAddresses, const bdDNSHandle);
    static void abortGetHostByName(bdDNSHandle);
    static void setExclusiveAccess(bdInt);
    static bdBool getAdHoc();
    static void setAdHoc(bdBool adHoc);
    static bdSocketStatusCode platformErrToBDErr(const bdInt error);
    static const bdNChar8 *const platformErrToString(const bdInt error);
private:
    bdPlatformSocket();
    static bdUInt64 m_totalBytesSent;
    static bdUInt64 m_totalPacketsSent;
    static bdUInt64 m_totalBytesRecvd;
    static bdUInt64 m_totalPacketsRecvd;
    static bdBool m_adHoc;
    static bdInt createWithHandle(const bdBool, const bdBool, const bdInt);
};
static_assert(sizeof(bdPlatformSocket) == 0x1, "bdPlatformSocket");

