// Original file: Contrib/Contrib/DemonWare/bdCore/bdThread/bdSemaphore.h
// Blank lines keep the original line numbers.

#pragma once

#include "../../bdPlatform/bdPlatformConfig/bdPlatformConfig-switch.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory-std.h"

class bdSemaphore {  // size 0x10
public:







    BD_CLASS_ALLOCATOR(bdSemaphore);
    bdSemaphore(bdUInt initialCount, bdUInt maxCount);
    void release();
    bdBool wait();
    void destroy();
protected:
    virtual ~bdSemaphore();  // vtable slot 0
    bdSemaphoreHandle m_handle;  // 0x08
};
static_assert(sizeof(bdSemaphore) == 0x10, "bdSemaphore");
