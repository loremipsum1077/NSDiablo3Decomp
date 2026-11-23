// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSCookieEcho.h

#pragma once

#include "../../../bdCore/bdCrypto/bdCypherSuites.h"
#include "../../../bdCore/bdSocket/bdCommonAddr.h"
#include "../../../bdCore/bdMemory/bdMemory-std.h"
#include "../../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdDTLSHeader.h"
#include "bdDTLSInitAck.h"
#include <stddef.h>
#include <stdint.h>

class bdECCKey;

class bdDTLSCookieEcho : public bdDTLSHeader {  // size 0x1c0
public:
    BD_CLASS_ALLOCATOR(bdDTLSCookieEcho);
    bdDTLSCookieEcho();
    bdDTLSCookieEcho(bdUInt16, const bdDTLSInitAck &, bdCommonAddrRef, bdECCKey *const);
    virtual ~bdDTLSCookieEcho();  // vtable slot 0
    virtual bdBool serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const;  // vtable slot 2
    virtual bdBool deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset, const bdCypherSuite expectedCypherSuite);  // vtable slot 4
    const bdDTLSInitAck & getCookie() const;
    const bdUByte8 * getCa() const;
    const bdUByte8 * getSecID() const;
    const bdUByte8 * getECCKey() const;
protected:
    bdDTLSInitAck m_cookie;  // 0x10
    bdUByte8 m_ca[67];  // 0x110
    bdUByte8 m_secID[8];  // 0x153
    bdUByte8 m_ECCKey[100];  // 0x15b
};
static_assert(sizeof(bdDTLSCookieEcho) == 0x1c0, "bdDTLSCookieEcho");


// Functions defined in this file (bodies are not in the DWARF):
//   line    25  void bdDTLSCookieEcho::operator delete(void * p)  [inlined into 1 functions]
