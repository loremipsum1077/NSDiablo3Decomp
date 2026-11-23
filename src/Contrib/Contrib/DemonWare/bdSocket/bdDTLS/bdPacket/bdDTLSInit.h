// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSInit.h

#pragma once

#include "../../../bdCore/bdCrypto/bdCypherSuites.h"
#include "../../../bdCore/bdSocket/bdSecurityID.h"
#include "../../../bdCore/bdMemory/bdMemory-std.h"
#include "../../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdDTLSRandom.h"
#include "bdDTLSHeader.h"
#include <stddef.h>
#include <stdint.h>

class bdDTLSInit : public bdDTLSHeader {  // size 0x40
public:
    BD_CLASS_ALLOCATOR(bdDTLSInit);
    bdDTLSInit();
    bdDTLSInit(bdUInt16, const bdSecurityID &, const bdDTLSRandom &, const bdCypherSuite &);
    virtual ~bdDTLSInit();  // vtable slot 0
    virtual bdBool serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const;  // vtable slot 2
    virtual bdBool deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset, const bdCypherSuite expectedCypherSuite);  // vtable slot 4
    bdUInt16 getInitTag() const;
    void getSecID(bdSecurityID & secID) const;
    const bdDTLSRandom & getRandomData() const;
protected:
    bdUInt16 m_initTag;  // 0x0e
    bdSecurityID m_secID;  // 0x10
    bdDTLSRandom m_random;  // 0x18
    bdCypherSuite m_cypherSuite;  // 0x3a
};
static_assert(sizeof(bdDTLSInit) == 0x40, "bdDTLSInit");


// Functions defined in this file (bodies are not in the DWARF):
//   line    21  void bdDTLSInit::operator delete(void * p)  [inlined into 1 functions]
