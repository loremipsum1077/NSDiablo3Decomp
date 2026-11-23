// Adopted from authenticated original DWARF; declaration-only dependency closure.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCypherAESGCM.h

#pragma once

#include "bdAuthenticationTag.h"
#include "bdCypher.h"
#include "../bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdCypherAESGCM : public bdCypher {  // size 0x48
public:
    BD_CLASS_ALLOCATOR(bdCypherAESGCM)  // line 26
    bdCypherAESGCM();
    virtual ~bdCypherAESGCM();  // vtable slot 0
    virtual bdBool init(const bdUByte8 * key, bdUInt keySize);  // vtable slot 2
    virtual bdBool encrypt(const bdUByte8 * iv, const bdUByte8 * plainText, bdUByte8 * cypherText, bdUInt size);  // vtable slot 3
    virtual bdBool decrypt(const bdUByte8 * iv, const bdUByte8 * cypherText, bdUByte8 * plainText, bdUInt size);  // vtable slot 4
    bdBool setAuthenticationTag(const bdAuthenticationTag & authTag);
    bdBool getAuthenticationTag(bdAuthenticationTag & authTag, const bdUByte8 tagLength);
    bdBool setAAD(const bdUByte8 *const aad, const bdUInt aadSize);
protected:
    bdUInt32 m_keySize;  // 0x10
    bdAuthenticationTag m_authenticationTag;  // 0x18
    const bdUByte8 * m_aad;  // 0x38
    bdUInt m_aadSize;  // 0x40
};
static_assert(sizeof(bdCypherAESGCM) == 0x48, "bdCypherAESGCM");


// Functions defined in this file (bodies are not in the DWARF):
//   line    26  void bdCypherAESGCM::operator delete(void * p)  [inlined into 1 functions]
//   line    26  void * bdCypherAESGCM::operator new(size_t nbytes)  [inlined into 1 functions]
