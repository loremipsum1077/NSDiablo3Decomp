// Adopted from the DWARF reference; hand-maintained.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdSocket/bdSecurityID.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdSecurityID {  // size 0x8
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdSecurityID();
    bdSecurityID(const bdSecurityID &);
    ~bdSecurityID();
    bdBool operator==(const bdSecurityID & other) const;
    bdBool operator!=(const bdSecurityID &) const;
    bdUByte8 ab[8];  // 0x00
};
static_assert(sizeof(bdSecurityID) == 0x8, "bdSecurityID");

