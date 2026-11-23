// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdSocket/bdStreamSocket.h

#pragma once

#include "bdPort.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdPlatform/bdPlatformSocket/bdPlatformSocket.h"
#include <stddef.h>
#include <stdint.h>

class bdAddr;

class bdStreamSocket {  // size 0x4
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdStreamSocket();
    ~bdStreamSocket();
    bdBool create(const bdBool blocking);
    bdSocketStatusCode bind(const bdPort) const;
    bdSocketStatusCode bind(const bdAddr addr) const;
    bdSocketStatusCode connect(const bdAddr addr) const;
    bdBool isConnected() const;
    bdBool isWritable(bdSocketStatusCode & error) const;
    bdInt send(const void * data, const bdUInt length) const;
    bdInt recv(void * data, const bdUInt size) const;
    bdBool accept(bdStreamSocket &, bdAddr &);
    void close();
    bdInt getHandle();
protected:
    bdInt m_handle;  // 0x00
};
static_assert(sizeof(bdStreamSocket) == 0x4, "bdStreamSocket");

