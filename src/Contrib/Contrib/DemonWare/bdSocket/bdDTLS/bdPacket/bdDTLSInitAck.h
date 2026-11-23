// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSInitAck.h

#pragma once

#include "../../../bdCore/bdCrypto/bdCypherSuites.h"
#include "../../../bdCore/bdSocket/bdAddr.h"
#include "../../../bdCore/bdSocket/bdSecurityID.h"
#include "../../../bdCore/bdMemory/bdMemory-std.h"
#include "../../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdDTLSRandom.h"
#include "bdDTLSHeader.h"
#include <stddef.h>
#include <stdint.h>

class bdHMac;

class bdDTLSInitAck : public bdDTLSHeader {  // size 0x100
public:
    BD_CLASS_ALLOCATOR(bdDTLSInitAck);
    bdDTLSInitAck();
    bdDTLSInitAck(bdUInt16, bdUInt16, bdUInt16, bdUInt16, bdUInt16, bdUInt16, const bdCypherSuite, const bdDTLSRandom &, const bdDTLSRandom &, bdUInt32, const bdAddr &, bdSecurityID);
    virtual ~bdDTLSInitAck();  // vtable slot 0
    bdBool sign(bdHMac & hmac);
    bdBool verify(bdHMac & hmac) const;
    virtual bdBool serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const;  // vtable slot 2
    virtual bdBool deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset, const bdCypherSuite expectedCypherSuite);  // vtable slot 4
    bdUInt32 getTimestamp() const;
    bdUInt16 getInitTag() const;
    bdUInt16 getLocalTag() const;
    bdUInt16 getPeerTag() const;
    bdUInt16 getLocalTieTag() const;
    bdUInt16 getPeerTieTag() const;
    const bdAddr & getPeerAddr() const;
    const bdDTLSRandom & getLocalRandom() const;
    const bdDTLSRandom & getPeerRandom() const;
    void getSecID(bdSecurityID & secID) const;
protected:
    bdCypherSuite m_cypherSuite;  // 0x0e
    bdUInt32 m_timestamp;  // 0x10
    bdUByte8 m_signature[16];  // 0x14
    bdUInt16 m_initTag;  // 0x24
    bdUInt16 m_localTag;  // 0x26
    bdUInt16 m_peerTag;  // 0x28
    bdUInt16 m_localTieTag;  // 0x2a
    bdUInt16 m_peerTieTag;  // 0x2c
    bdDTLSRandom m_localRandom;  // 0x2e
    bdDTLSRandom m_peerRandom;  // 0x4f
    bdAddr m_peerAddr;  // 0x70
    bdSecurityID m_secID;  // 0xf8
};
static_assert(sizeof(bdDTLSInitAck) == 0x100, "bdDTLSInitAck");


// Functions defined in this file (bodies are not in the DWARF):
//   line    22  bdDTLSInitAck::bdDTLSInitAck(const bdDTLSInitAck &)  [inlined into 1 functions]
//   line    22  bdDTLSInitAck & bdDTLSInitAck::operator=(const bdDTLSInitAck &)  [inlined into 1 functions]
//   line    26  void bdDTLSInitAck::operator delete(void * p)  [inlined into 1 functions]

// Variables defined in this file:
//   line    19  static const bdUInt BD_DTLS_INIT_ACK_SIGNATURE_SIZE
