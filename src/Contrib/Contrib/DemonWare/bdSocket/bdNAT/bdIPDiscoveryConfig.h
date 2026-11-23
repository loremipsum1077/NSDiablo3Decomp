// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdNAT/bdIPDiscoveryConfig.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdIPDiscoveryConfig {  // size 0xc
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdIPDiscoveryConfig();
    void reset();
    bdUByte8 m_retries;  // 0x00
    bdFloat32 m_requestTimeout;  // 0x04
    bdUInt16 m_version;  // 0x08
    void sanityCheckConfig();
};
static_assert(sizeof(bdIPDiscoveryConfig) == 0xc, "bdIPDiscoveryConfig");

