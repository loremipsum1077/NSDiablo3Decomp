// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCypherAES.h

#pragma once

#include "bdCypher.h"
#include "../bdContainers/bdAligned.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_cipher.h"
#include "../bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdCypherAES : public bdCypher {  // size 0x6b0
public:
    BD_CLASS_ALLOCATOR(bdCypherAES)  // line 25
    bdCypherAES();
    virtual ~bdCypherAES();  // vtable slot 0
    virtual bdBool init(const bdUByte8 * key, bdUInt keySize);  // vtable slot 2
    virtual bdBool encrypt(const bdUByte8 * iv, const bdUByte8 * plainText, bdUByte8 * cypherText, bdUInt size);  // vtable slot 3
    virtual bdBool decrypt(const bdUByte8 * iv, const bdUByte8 * cypherText, bdUByte8 * plainText, bdUInt size);  // vtable slot 4
protected:
    bdUInt32 m_keySize;  // 0x10
    bdAligned<symmetric_CBC, 16> m_cbc;  // 0x18
};
static_assert(sizeof(bdCypherAES) == 0x6b0, "bdCypherAES");


// Functions defined in this file (bodies are not in the DWARF):
//   line    25  void bdCypherAES::operator delete(void * p)  [inlined into 1 functions]
//   line    25  void * bdCypherAES::operator new(size_t nbytes)  [inlined into 2 functions]
