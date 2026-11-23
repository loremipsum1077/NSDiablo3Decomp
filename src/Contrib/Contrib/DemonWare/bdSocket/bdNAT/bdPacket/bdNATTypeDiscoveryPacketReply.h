// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdNAT/bdPacket/bdNATTypeDiscoveryPacketReply.h

#pragma once

#include "../../../bdCore/bdSocket/bdAddr.h"
#include "../../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdNATTypeDiscoveryPacketReply {  // size 0x114
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdNATTypeDiscoveryPacketReply();
    bdNATTypeDiscoveryPacketReply(const bdAddr &, const bdAddr &, const bdUInt16);
    bdBool serialize(void *, const bdUInt, const bdUInt, bdUInt &) const;
    bdBool deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset);
    bdUByte8 getType() const;
    const bdAddr & getMappedAddr() const;
    const bdAddr & getSecAddr() const;
protected:
    bdUByte8 m_type;  // 0x00
    bdUInt16 m_protocolVersion;  // 0x02
    bdAddr m_mappedAddr;  // 0x04
    bdAddr m_secAddr;  // 0x8c
};
static_assert(sizeof(bdNATTypeDiscoveryPacketReply) == 0x114, "bdNATTypeDiscoveryPacketReply");


// Functions defined in this file (bodies are not in the DWARF):
//   line    16  bdNATTypeDiscoveryPacketReply::~bdNATTypeDiscoveryPacketReply()  [inlined into 1 functions]
