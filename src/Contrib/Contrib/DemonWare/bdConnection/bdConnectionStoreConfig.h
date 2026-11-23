// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdConnectionStoreConfig.h

#pragma once

#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdConnectionStoreConfig {  // size 0x10
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdConnectionStoreConfig();
    void reset();
    bdUInt m_maxReceiveIterations;  // 0x00
    bdFloat32 m_maxShutdownDuration;  // 0x04
    bdUInt m_maxFlushIterations;  // 0x08
    bdBool m_acceptOutOfOrder;  // 0x0c
};
static_assert(sizeof(bdConnectionStoreConfig) == 0x10, "bdConnectionStoreConfig");

