// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdNAT/bdPacket/bdIPDiscoveryPacketReply.h

#pragma once

#include "../../../bdCore/bdSocket/bdAddr.h"
#include "../../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdIPDiscoveryPacketReply {  // size 0x114
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdIPDiscoveryPacketReply(const bdAddr &, bdUInt16);
    bdIPDiscoveryPacketReply();
    bdBool serialize(void *, const bdUInt, const bdUInt, bdUInt &) const;
    bdBool deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset);
    bdUByte8 getType() const;
    const bdAddr & getAddr() const;
    const bdAddr & getProtectedAddr() const;
    bdUInt16 getProtocolVersion() const;
    void applyXorToAddr(bdAddr & addr) const;
protected:
    bdUByte8 m_type;  // 0x00
    bdUInt16 m_protocolVersion;  // 0x02
    bdAddr m_addr;  // 0x04
    bdAddr m_protectedAddr;  // 0x8c
};
static_assert(sizeof(bdIPDiscoveryPacketReply) == 0x114, "bdIPDiscoveryPacketReply");


// Functions defined in this file (bodies are not in the DWARF):
//   line    16  bdIPDiscoveryPacketReply::~bdIPDiscoveryPacketReply()  [inlined into 1 functions]
