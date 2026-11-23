// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHMacSHA256.h

#pragma once

#include "bdHMac.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory-std.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_mac.h"
#include <stddef.h>
#include <stdint.h>

class bdHMacSHA256 : public bdHMac {  // size 0x1b8
public:
    static void * operator new(size_t nbytes) { return bdMemory::allocate(nbytes); }
    static void operator delete(void * p) { bdMemory::deallocate(p); }
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void * p) { return p; }
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdHMacSHA256(const bdUByte8 *const, const bdUInt);
    virtual ~bdHMacSHA256();  // vtable slot 0
    virtual bdBool process(const bdUByte8 *const data, const bdUInt length);  // vtable slot 2
    virtual bdBool getData(bdUByte8 * dst, bdUInt & length);  // vtable slot 3
protected:
    hmac_state m_state;  // 0x08
};
static_assert(sizeof(bdHMacSHA256) == 0x1b8, "bdHMacSHA256");


// Functions defined in this file (bodies are not in the DWARF):
//   line    20  void bdHMacSHA256::operator delete(void * p)  [inlined into 1 functions]
//   line    20  void * bdHMacSHA256::operator new(size_t nbytes)  [inlined into 1 functions]
