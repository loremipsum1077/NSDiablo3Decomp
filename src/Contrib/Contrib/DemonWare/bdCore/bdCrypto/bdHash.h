// Declaration-only adoption from DWARF; no hash bodies reconstructed.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHash.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

enum bdHashAlgorithms {
    BD_HASH_SHA256 = 0,
    BD_HASH_TIGER192 = 1,
    BD_HASH_SHA1 = 2,
    BD_HASH_MD5 = 3,
    BD_MAX_HASH_ALGORITHMS = 4,
};

class bdHash {  // size 0x10
    // vtable pointer
public:
    static void * operator new(size_t);
    static void operator delete(void * p) { bdMemory::deallocate(p); }
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdHash();
    virtual ~bdHash();  // vtable slot 0
    virtual bdBool hash(const bdUByte8 *, const bdUInt, bdUByte8 *, bdUInt &) = 0;  // vtable slot 2
    const bdHashAlgorithms m_type;  // 0x08
    const bdUInt32 m_digestSize;  // 0x0c
protected:
    bdHash(bdHashAlgorithms, bdUInt32);
    bdHash(const bdHash &);
    bdHash & operator=(const bdHash &);
};
static_assert(sizeof(bdHash) == 0x10, "bdHash");

