// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSCookieAck.h

#pragma once

#include "../../../bdCore/bdSocket/bdSecurityID.h"
#include "../../../bdCore/bdMemory/bdMemory-std.h"
#include "../../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdDTLSHeader.h"
#include <stddef.h>
#include <stdint.h>

class bdDTLSCookieAck : public bdDTLSHeader {  // size 0x80
public:
    static void operator delete(void * p) { bdMemory::deallocate(p); }
    static void operator delete(void *, void *);
    static void operator delete[](void *);
    static void operator delete[](void *, void *);
    static void * operator new(size_t);
    static void * operator new(size_t, void *);
    static void * operator new[](size_t);
    static void * operator new[](size_t, void *);
    bdDTLSCookieAck();
    bdDTLSCookieAck(bdUInt16, const bdUByte8 *, const bdSecurityID);
    virtual ~bdDTLSCookieAck();  // vtable slot 0
    virtual bdBool serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const;  // vtable slot 2
    virtual bdBool deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset);  // vtable slot 3
    const bdUByte8 * getECCKey() const;
    void getSecID(bdSecurityID & secID) const;
protected:
    bdUByte8 m_ECCKey[100];  // 0x0e
    bdSecurityID m_secID;  // 0x72
};
static_assert(sizeof(bdDTLSCookieAck) == 0x80, "bdDTLSCookieAck");


// Functions defined in this file (bodies are not in the DWARF):
//   line    23  void bdDTLSCookieAck::operator delete(void * p)  [inlined into 1 functions]
