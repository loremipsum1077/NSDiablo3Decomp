// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdConnectionListener.h

#pragma once

#include "bdMessage/bdReceivedMessage.h"
#include "../bdCore/bdReference/bdReference.h"
#include "../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

typedef bdReference<bdConnection> bdConnectionRef;

class bdConnectionListener {  // size 0x8
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdConnectionListener);
    bdConnectionListener();
    virtual ~bdConnectionListener();  // vtable slot 0
    virtual void onConnect(bdConnectionRef);  // vtable slot 2
    virtual void onConnectFailed(bdConnectionRef);  // vtable slot 3
    virtual void onDisconnect(bdConnectionRef);  // vtable slot 4
    virtual void onReconnect(bdConnectionRef connection);  // vtable slot 5
};
static_assert(sizeof(bdConnectionListener) == 0x8, "bdConnectionListener");


// Functions defined in this file (bodies are not in the DWARF):
//   line    27  void bdConnectionListener::operator delete(void * p)  [inlined into 1 functions]
