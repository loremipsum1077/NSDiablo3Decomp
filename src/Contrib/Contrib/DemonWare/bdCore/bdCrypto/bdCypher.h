// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCypher.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

enum bdCypherAlgorithms {
    BD_CYPHER_AES128_CBC = 0,
    BD_CYPHER_AES192_CBC = 1,
    BD_CYPHER_AES256_CBC = 2,
    BD_CYPHER_3DES_CBC = 3,
    BD_CYPHER_AES128_GCM = 4,
    BD_CYPHER_AES192_GCM = 5,
    BD_CYPHER_AES256_GCM = 6,
    BD_MAX_CYPHER_ALGORITHMS = 7,
};

enum bdCypherModes {
    BD_CYPHER_MODE_NA = 0,
    BD_CYPHER_MODE_CBC = 1,
    BD_CYPHER_MODE_GCM = 2,
    BD_MAX_CYPHER_MODES = 3,
};

class bdCypher {  // size 0x10
    // vtable pointer
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    virtual ~bdCypher() = 0;  // vtable slot 0
    virtual bdBool init(const bdUByte8 *, bdUInt) = 0;  // vtable slot 2
    virtual bdBool encrypt(const bdUByte8 *, const bdUByte8 *, bdUByte8 *, bdUInt) = 0;  // vtable slot 3
    virtual bdBool decrypt(const bdUByte8 *, const bdUByte8 *, bdUByte8 *, bdUInt) = 0;  // vtable slot 4
    bdUInt32 getBlockSize() const;
    bdCypherModes getCypherMode() const;
protected:
    const bdUInt32 m_blockSize;  // 0x08
    const bdCypherModes m_cypherMode;  // 0x0c
    bdCypher(bdUInt32, bdCypherModes);
    bdCypher(const bdCypher &);
    bdCypher & operator=(const bdCypher &);
};
static_assert(sizeof(bdCypher) == 0x10, "bdCypher");

