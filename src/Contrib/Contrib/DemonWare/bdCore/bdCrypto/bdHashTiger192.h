// Declaration-only adoption from DWARF; no hash bodies reconstructed.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHashTiger192.h

#pragma once

#include "bdHash.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdHashTiger192 : public bdHash {  // size 0x10
public:
    static void * operator new(size_t nbytes) { return bdMemory::allocate(nbytes); }
    static void operator delete(void * p) { bdMemory::deallocate(p); }
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void * p) { return p; }
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdHashTiger192();
    virtual ~bdHashTiger192();  // vtable slot 0
    virtual bdBool hash(const bdUByte8 * data, const bdUInt dataSize, bdUByte8 * result, bdUInt & resultSize);  // vtable slot 2
protected:
    static bdInt s_hash;
};
static_assert(sizeof(bdHashTiger192) == 0x10, "bdHashTiger192");


// Functions defined in this file (bodies are not in the DWARF):
//   line    18  void bdHashTiger192::operator delete(void * p)  [inlined into 1 functions]
//   line    18  void * bdHashTiger192::operator new(size_t nbytes)  [inlined into 1 functions]
