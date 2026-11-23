// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdRandom.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdRandom {  // size 0x4
public:
    static void operator delete(void *);
    static void operator delete(void *, void *);
    static void operator delete[](void *);
    static void operator delete[](void *, void *);
    static void * operator new(size_t);
    static void * operator new(size_t, void *);
    static void * operator new[](size_t);
    static void * operator new[](size_t, void *);
    bdRandom();
    bdRandom(const bdUInt);
    ~bdRandom();
    bdUInt nextUInt();
    void nextUBytes(bdUByte8 *, const bdInt);
    void setSeed(const bdUInt seed);
    bdUInt getSeed() const;
protected:
    bdUInt m_val;  // 0x00
    static const bdInt a = 16807;  // DWARF: defined out of class (stripped); in-class values avoid emitting unused data
    static const bdInt m = 2147483647;
    static const bdInt q = 127773;
    static const bdInt r = 2836;
};
static_assert(sizeof(bdRandom) == 0x4, "bdRandom");

