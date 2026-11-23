// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdNAT/bdNATTypeDiscoveryConfig.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdNATTypeDiscoveryConfig {  // size 0x8
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdNATTypeDiscoveryConfig();
    void reset();
    void sanityCheckConfig();
    bdUInt m_NtdcsMaxResends;  // 0x00
    bdFloat32 m_NtdcsSendTimeout;  // 0x04
};
static_assert(sizeof(bdNATTypeDiscoveryConfig) == 0x8, "bdNATTypeDiscoveryConfig");

