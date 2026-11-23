// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformThread/bdPlatformSemaphore.h

#pragma once

#include "../bdPlatformConfig/bdPlatformConfig-switch.h"
#include "../bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdPlatformSemaphore {  // size 0x1
public:
    static bdSemaphoreHandle createSemaphore(bdUInt32 initialCount, bdUInt32 maxCount);
    static void destroy(bdSemaphoreHandle & handle);
    static void release(bdSemaphoreHandle & handle);
    static bdBool wait(bdSemaphoreHandle & handle);
private:
    bdPlatformSemaphore();
};
static_assert(sizeof(bdPlatformSemaphore) == 0x1, "bdPlatformSemaphore");

