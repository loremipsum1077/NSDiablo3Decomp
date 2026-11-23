// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformThread/bdPlatformMutex.h

#pragma once

#include "../bdPlatformConfig/bdPlatformConfig-switch.h"
#include "../bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdPlatformMutex {  // size 0x1
public:
    static bdMutexHandle createMutex();
    static bdBool lock(bdMutexHandle & handle);
    static bdBool unlock(bdMutexHandle & handle);
    static bdBool destroy(bdMutexHandle & handle);
private:
    bdPlatformMutex();
};
static_assert(sizeof(bdPlatformMutex) == 0x1, "bdPlatformMutex");

