// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSHeader.h

#pragma once

#include "../../../bdCore/bdMemory/bdMemory-std.h"
#include "../../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdDTLSPackets.h"
#include <stddef.h>
#include <stdint.h>

class bdDTLSHeader {  // size 0x10
    // vtable pointer
public:
    static void operator delete(void * p) { bdMemory::deallocate(p); }
    static void operator delete(void *, void *);
    static void operator delete[](void *);
    static void operator delete[](void *, void *);
    static void * operator new(size_t);
    static void * operator new(size_t, void *);
    static void * operator new[](size_t);
    static void * operator new[](size_t, void *);
    bdDTLSHeader();
    virtual ~bdDTLSHeader();  // vtable slot 0
    bdDTLSHeader(bdDTLSPacketTypes, bdUInt16, bdUInt16);
    virtual bdBool serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const;  // vtable slot 2
    virtual bdBool deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset);  // vtable slot 3
    bdUByte8 getType() const;
    bdUInt16 getVtag() const;
    bdUInt16 getCounter() const;
    bdUInt getSerializedSize() const;
protected:
    bdUByte8 m_type;  // 0x08
    bdDTLSVersion m_version;  // 0x09
    bdUInt16 m_vtag;  // 0x0a
    bdUInt16 m_counter;  // 0x0c
    static bdUInt s_serializedSize;
};
static_assert(sizeof(bdDTLSHeader) == 0x10, "bdDTLSHeader");


// Functions defined in this file (bodies are not in the DWARF):
//   line    19  bdDTLSHeader::bdDTLSHeader(const bdDTLSHeader &)  [inlined into 1 functions]
//   line    19  bdDTLSHeader & bdDTLSHeader::operator=(const bdDTLSHeader &)  [inlined into 1 functions]
//   line    23  void bdDTLSHeader::operator delete(void * p)  [inlined into 1 functions]
