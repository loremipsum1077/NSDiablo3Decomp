// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdThread/bdMutex.h

#pragma once

#include "../../bdPlatform/bdPlatformConfig/bdPlatformConfig-switch.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdMutex {  // size 0x20
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdMutex();
    ~bdMutex();
    bdBool lock();
    bdBool unlock();
protected:
    bdMutexHandle m_handle;  // 0x00
};
static_assert(sizeof(bdMutex) == 0x20, "bdMutex");

