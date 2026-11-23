// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyConnectionListener.h

#pragma once

#include "../bdCore/bdReference/bdReference.h"
#include "../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdLobbyConnection;

typedef bdReference<bdLobbyConnection> bdLobbyConnectionRef;

class bdLobbyConnectionListener {  // size 0x8
    // vtable pointer
public:
    // bdLobbyConnectionListener.h:25 holds operator delete (DWARF): the shared allocator macro.






    BD_CLASS_ALLOCATOR(bdLobbyConnectionListener);
    bdLobbyConnectionListener();
    virtual ~bdLobbyConnectionListener();  // vtable slot 0
    virtual void onConnect(bdLobbyConnectionRef);  // vtable slot 2
    virtual void onConnectFailed(bdLobbyConnectionRef);  // vtable slot 3
    virtual void onDisconnect(bdLobbyConnectionRef);  // vtable slot 4
};
static_assert(sizeof(bdLobbyConnectionListener) == 0x8, "bdLobbyConnectionListener");
