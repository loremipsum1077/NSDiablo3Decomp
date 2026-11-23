// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdNAT/bdPacket/bdIPDiscoveryPacket.h

#pragma once

#include "../../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdIPDiscoveryPacket {  // size 0x4
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdIPDiscoveryPacket();
    bdIPDiscoveryPacket(const bdUInt16);
    bdBool serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const;
    bdBool deserialize(const void *, const bdUInt, const bdUInt, bdUInt &);
    bdUByte8 getType() const;
    bdUInt16 getProtocolVersion() const;
protected:
    bdUByte8 m_type;  // 0x00
    bdUInt16 m_protocolVersion;  // 0x02
};
static_assert(sizeof(bdIPDiscoveryPacket) == 0x4, "bdIPDiscoveryPacket");

