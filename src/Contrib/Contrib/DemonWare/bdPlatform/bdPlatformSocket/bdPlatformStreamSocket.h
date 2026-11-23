// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformSocket/bdPlatformStreamSocket.h

#pragma once

#include "../bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdPlatformSocket.h"
#include "bdSockAddr.h"

class bdPlatformStreamSocket {  // size 0x1
public:
    static bdInt create(const bdBool blocking);
    static bdInt create(const bdUInt, const bdBool);
    static bdSocketStatusCode bind(const bdInt, const bdInAddr, const bdUInt16);
    static bdSocketStatusCode connect(const bdInt handle, const bdInAddr addr, const bdUInt16 port);
    static bdBool accept(const bdInt, bdInt &, bdInAddr &, bdUInt16 &);
    static bdInt send(const bdInt handle, const void * data, const bdUInt length);
    static bdInt receive(const bdInt handle, void *const data, const bdUInt size);
    static bdBool close(const bdInt handle);
    static void unregisterThread();
    static bdUInt64 getBytesSent();
    static bdUInt64 getBytesReceived();
    static bdBool checkSocketException(const bdInt);
    static bdBool isWritable(const bdInt handle);
    static bdBool isWritable(const bdInt handle, bdSocketStatusCode & error);
private:
    bdPlatformStreamSocket();
    static bdUInt64 m_totalBytesSent;
    static bdUInt64 m_totalBytesRecvd;
};
static_assert(sizeof(bdPlatformStreamSocket) == 0x1, "bdPlatformStreamSocket");
