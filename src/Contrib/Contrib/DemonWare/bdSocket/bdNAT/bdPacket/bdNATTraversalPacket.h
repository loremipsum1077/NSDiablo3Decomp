// Original declarations recovered from DWARF.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdNAT/bdPacket/bdNATTraversalPacket.h

#pragma once

#include "../../../bdCore/bdSocket/bdAddr.h"
#include "../../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdNATTraversalPacket {  // size 0x124
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdNATTraversalPacket();
    bdNATTraversalPacket(const bdUByte8 &, const bdUInt32, const bdAddr &, const bdAddr &);
    bdBool serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const;
    bdBool deserialize(const void * data, const bdUInt32 size, const bdUInt offset, bdUInt & newOffset);
    bdUInt getSize() const;
    bdUByte8 getType() const;
    void setType(bdUByte8 type);
    bdUInt32 getIdentifier() const;
    const bdAddr & getAddrDest() const;
    const bdAddr & getAddrSrc() const;
    const bdUByte8 * getHMAC() const;
    void setHMAC(const bdUByte8 * hmac, const bdUInt size);
    bdUInt16 getProtocolVersion() const;
protected:
    bdUByte8 m_type;  // 0x00
    bdUInt16 m_protocolVersion;  // 0x02
    bdUByte8 m_hmac[10];  // 0x04
    bdUInt32 m_ident;  // 0x10
    bdAddr m_addrSrc;  // 0x14
    bdAddr m_addrDest;  // 0x9c
    static bdUInt serializedSize;
};
static_assert(sizeof(bdNATTraversalPacket) == 0x124, "bdNATTraversalPacket");


// Functions defined in this file (bodies are not in the DWARF):
//   line    18  bdNATTraversalPacket::~bdNATTraversalPacket()  [inlined into 4 functions]
