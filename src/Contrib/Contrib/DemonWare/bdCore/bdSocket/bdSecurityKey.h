// Adopted from the DWARF reference; hand-maintained.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdSocket/bdSecurityKey.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdSecurityKey {  // size 0x10
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdSecurityKey();
    bdSecurityKey(const bdSecurityKey &);
    ~bdSecurityKey();
    bdBool operator==(const bdSecurityKey & other) const;
    bdUByte8 ab[16];  // 0x00
};
static_assert(sizeof(bdSecurityKey) == 0x10, "bdSecurityKey");

