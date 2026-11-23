// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCypher3Des.h

#pragma once

#include "bdCypher.h"
#include "../bdContainers/bdAligned.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_cipher.h"
#include "../bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdCypher3Des : public bdCypher {  // size 0x6a8
public:
    BD_CLASS_ALLOCATOR(bdCypher3Des)  // line 23
    bdCypher3Des();
    virtual ~bdCypher3Des();  // vtable slot 0
    virtual bdBool init(const bdUByte8 * key, bdUInt keySize);  // vtable slot 2
    virtual bdBool encrypt(const bdUByte8 * iv, const bdUByte8 * plainText, bdUByte8 * cypherText, bdUInt size);  // vtable slot 3
    virtual bdBool decrypt(const bdUByte8 * iv, const bdUByte8 * cypherText, bdUByte8 * plainText, bdUInt size);  // vtable slot 4
protected:
    bdAligned<symmetric_CBC, 16> m_cbc;  // 0x10
};
static_assert(sizeof(bdCypher3Des) == 0x6a8, "bdCypher3Des");


// Functions defined in this file (bodies are not in the DWARF):
//   line    23  void bdCypher3Des::operator delete(void * p)  [inlined into 1 functions]
//   line    23  void * bdCypher3Des::operator new(size_t nbytes)  [inlined into 1 functions]
