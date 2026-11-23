// Adopted from original DWARF declarations.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdTaskResultProcessor.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdTaskResult;

class bdTaskResultProcessor {  // size 0x8
    // vtable pointer
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdTaskResultProcessor();
    virtual ~bdTaskResultProcessor();  // vtable slot 0
    virtual bdBool processResult(bdTaskResult *, bdUInt) = 0;  // vtable slot 2
};
static_assert(sizeof(bdTaskResultProcessor) == 0x8, "bdTaskResultProcessor");

