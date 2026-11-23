// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdDispatch/bdDispatchInterceptor.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdReceivedMessage;

class bdDispatchInterceptor {  // size 0x8
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
    bdDispatchInterceptor();
    virtual ~bdDispatchInterceptor();  // vtable slot 0
    virtual bdBool accept(const bdReceivedMessage &) = 0;  // vtable slot 2
    virtual bdBool acceptOutOfOrder(const bdReceivedMessage &);  // vtable slot 3
};
static_assert(sizeof(bdDispatchInterceptor) == 0x8, "bdDispatchInterceptor");

