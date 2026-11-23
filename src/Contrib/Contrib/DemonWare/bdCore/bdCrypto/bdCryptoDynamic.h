// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCryptoDynamic.h

#pragma once

#include "bdCypher.h"
#include "bdHash.h"
#include "../bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdHMac;

struct bdCryptoDummy {  // size 0x8
    void * m_dummy;  // 0x00
};
static_assert(sizeof(bdCryptoDummy) == 0x8, "bdCryptoDummy");

class bdCryptoDynamic {  // size 0x1
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    static bdUInt getCypherAlgorithmKeySize(bdCypherAlgorithms cypherAlgorithm);
    static bdCypher * allocateCypher(bdCypherAlgorithms cypherAlgorithm, void * alignedBuffer);
    static bdHash * allocateHash(bdHashAlgorithms hashAlgorithm, void * alignedBuffer);
    static bdHMac * allocateHMAC(bdHashAlgorithms hashAlgorithm, const bdUByte8 *const key, const bdUInt keySize, void * alignedBuffer);
};
static_assert(sizeof(bdCryptoDynamic) == 0x1, "bdCryptoDynamic");

class bdDynamicCypher {  // size 0x6c0
    // vtable pointer
public:
    static void * operator new(size_t);
    static void operator delete(void * p) { bdMemory::deallocate(p); }  // line 92
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdDynamicCypher(bdCypherAlgorithms);
    virtual ~bdDynamicCypher();  // vtable slot 0
    bdCypher & getCypher();
protected:
    bdCypher * m_cypher;  // 0x08
    union {  // size 0x6b0
        bdUByte8 m_cypherObjectBuffer[1712];  // 0x00
        bdCryptoDummy dummy;  // 0x00
    };  // 0x10
    bdDynamicCypher(const bdDynamicCypher &);
};
static_assert(sizeof(bdDynamicCypher) == 0x6c0, "bdDynamicCypher");

class bdDynamicHash {  // size 0x20
    // vtable pointer
public:
    static void * operator new(size_t);
    static void operator delete(void * p) { bdMemory::deallocate(p); }  // line 124
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdDynamicHash(bdHashAlgorithms);
    virtual ~bdDynamicHash();  // vtable slot 0
    bdHash & getHash();
protected:
    bdHash * m_hash;  // 0x08
    union {  // size 0x10
        bdUByte8 m_hashObjectBuffer[16];  // 0x00
        bdCryptoDummy dummy;  // 0x00
    };  // 0x10
    bdDynamicHash(const bdDynamicHash &);
};
static_assert(sizeof(bdDynamicHash) == 0x20, "bdDynamicHash");

class bdDynamicHMAC {  // size 0x1c8
    // vtable pointer
public:
    static void * operator new(size_t);
    static void operator delete(void * p) { bdMemory::deallocate(p); }  // line 156
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdDynamicHMAC(bdHashAlgorithms, const bdUByte8 *const, const bdUInt);
    virtual ~bdDynamicHMAC();  // vtable slot 0
    bdHMac & getHMAC();
protected:
    bdHMac * m_hmac;  // 0x08
    union {  // size 0x1b8
        bdUByte8 m_hmacObjectBuffer[440];  // 0x00
        bdCryptoDummy dummy;  // 0x00
    };  // 0x10
    bdDynamicHMAC(const bdDynamicHMAC &);
};
static_assert(sizeof(bdDynamicHMAC) == 0x1c8, "bdDynamicHMAC");


// Functions defined in this file (bodies are not in the DWARF):
//   line    92  void bdDynamicCypher::operator delete(void * p)  [inlined into 1 functions]
//   line   102  bdCypher & bdDynamicCypher::getCypher()  [inlined into 3 functions]
//   line   124  void bdDynamicHash::operator delete(void * p)  [inlined into 1 functions]
//   line   134  bdHash & bdDynamicHash::getHash()  [inlined into 3 functions]
//   line   156  void bdDynamicHMAC::operator delete(void * p)  [inlined into 1 functions]
//   line   168  bdHMac & bdDynamicHMAC::getHMAC()  [inlined into 3 functions]

// getHMAC sits on its DWARF line (168); its inlined load is attributed to line 171.


















































inline bdHMac& bdDynamicHMAC::getHMAC()
{

    return *m_hmac;
}

inline bdCypher& bdDynamicCypher::getCypher()
{
    return *m_cypher;
}

inline bdHash& bdDynamicHash::getHash()
{
    return *m_hash;
}
