// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdNAT/bdPacket/bdNATTypeDiscoveryPacket.h

#pragma once

#include "../../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdNATTypeDiscoveryPacket {  // size 0x8
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    enum bdNATTypeDiscoveryPacketRequest {
        BD_NTDP_SAME_ADDR = 0,
        BD_NTDP_DIFF_PORT = 1,
        BD_NTDP_DIFF_IP = 2,
        BD_NTDP_DIFF_PORT_IP = 3,
        BD_NTDP_INVALID_TYPE = 4,
    };
    bdNATTypeDiscoveryPacket(bdNATTypeDiscoveryPacket::bdNATTypeDiscoveryPacketRequest);
    bdBool serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const;
    bdBool deserialize(const void *, const bdUInt, const bdUInt, bdUInt &);
    bdUByte8 getType() const;
    bdNATTypeDiscoveryPacket::bdNATTypeDiscoveryPacketRequest getRequest() const;
    bdUInt16 getProtocolVersion() const;
protected:
    bdUByte8 m_type;  // 0x00
    bdUInt16 m_protocolVersion;  // 0x02
    bdNATTypeDiscoveryPacket::bdNATTypeDiscoveryPacketRequest m_request;  // 0x04
};
static_assert(sizeof(bdNATTypeDiscoveryPacket) == 0x8, "bdNATTypeDiscoveryPacket");

