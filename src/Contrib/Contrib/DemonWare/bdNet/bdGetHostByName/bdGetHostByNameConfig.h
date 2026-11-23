// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdNet/bdGetHostByName/bdGetHostByNameConfig.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdGetHostByNameConfig {  // size 0x4
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdGetHostByNameConfig();
    void reset();
    void sanityCheckConfig();
    bdFloat32 m_timeout;  // 0x00
};
static_assert(sizeof(bdGetHostByNameConfig) == 0x4, "bdGetHostByNameConfig");

