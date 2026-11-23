// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdThread/bdRunnable.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdRunnable {  // size 0x10
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
    bdRunnable();
    virtual ~bdRunnable();  // vtable slot 0
    virtual bdUInt run(void *) = 0;  // vtable slot 2
    virtual void stop();  // vtable slot 3
    void start();
protected:
    bdBool m_stop;  // 0x08
};
static_assert(sizeof(bdRunnable) == 0x10, "bdRunnable");

