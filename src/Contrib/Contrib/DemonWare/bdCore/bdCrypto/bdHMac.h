// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHMac.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdHMac {  // size 0x8
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
    virtual ~bdHMac();  // vtable slot 0
    virtual bdBool process(const bdUByte8 *const, const bdUInt) = 0;  // vtable slot 2
    virtual bdBool getData(bdUByte8 *, bdUInt &) = 0;  // vtable slot 3
};
static_assert(sizeof(bdHMac) == 0x8, "bdHMac");

