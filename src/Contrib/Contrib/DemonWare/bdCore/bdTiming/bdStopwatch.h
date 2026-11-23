// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdTiming/bdStopwatch.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdStopwatch {  // size 0x8
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdStopwatch();
    void start();
    void reset();
    bdFloat32 getElapsedTimeInSeconds() const;
protected:
    bdUInt64 m_start;  // 0x00
};
static_assert(sizeof(bdStopwatch) == 0x8, "bdStopwatch");

