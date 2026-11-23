// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdSocket/bdSocket.h

#pragma once

#include "bdPort.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdPlatform/bdPlatformSocket/bdPlatformSocket.h"
#include <stddef.h>
#include <stdint.h>
#include "../bdMemory/bdMemory-std.h"
class bdAddr;

class bdSocket {  // size 0x10
    // vtable pointer
public:
    // bdSocket.h:24 holds both operator new and operator delete (DWARF): the shared allocator macro.






    BD_CLASS_ALLOCATOR(bdSocket);
    bdSocket();
    virtual ~bdSocket();  // vtable slot 0
    virtual bdBool create(const bdBool blocking, const bdBool broadcast, const bdBool);  // vtable slot 2
    virtual bdBool create(const bdUInt16 af_family, const bdBool blocking, const bdBool broadcast, const bdBool);  // vtable slot 3
    virtual bdSocketStatusCode bind(const bdPort port);  // vtable slot 4
    virtual bdSocketStatusCode bind(const bdUInt16 af_family, const bdPort port);  // vtable slot 5
    virtual bdSocketStatusCode bind(const bdAddr & addr);  // vtable slot 6
    virtual bdInt sendTo(const bdAddr & addr, const void * data, const bdUInt length);  // vtable slot 7
    virtual bdInt receiveFrom(bdAddr & addr, void * data, const bdUInt size);  // vtable slot 8
    virtual bdBool close();  // vtable slot 9
    bdInt getHandle() const;
    void setExclusiveAccess();
protected:
    bdInt m_handle;  // 0x08
};
static_assert(sizeof(bdSocket) == 0x10, "bdSocket");


// Functions defined in this file (bodies are not in the DWARF):
//   line    24  void bdSocket::operator delete(void * p)  [inlined into 1 functions]
//   line    24  void * bdSocket::operator new(size_t nbytes)  [inlined into 1 functions]
