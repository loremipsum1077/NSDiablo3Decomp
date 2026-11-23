// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdDispatch/bdDispatcher.h

#pragma once

#include "../bdMessage/bdReceivedMessage.h"
#include "../../bdCore/bdContainers/bdFastArray.h"
#include "../../bdCore/bdReference/bdReference.h"
#include <stddef.h>
#include <stdint.h>

class bdDispatchInterceptor;

typedef bdReference<bdConnection> bdConnectionRef;

class bdDispatcher {  // size 0x10
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdDispatcher();
    void process(bdConnectionRef connection);
    void registerInterceptor(bdDispatchInterceptor *const interceptor);
    void unregisterInterceptor(bdDispatchInterceptor *const interceptor);
    void reset();
protected:
    bdFastArray<bdDispatchInterceptor *> m_interceptors;  // 0x00
};
static_assert(sizeof(bdDispatcher) == 0x10, "bdDispatcher");


// Functions defined in this file (bodies are not in the DWARF):
//   line    24  bdDispatcher::~bdDispatcher()  [inlined into 1 functions]
