// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdLANDiscoveryListener.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "bdGameInfo.h"
#include <stddef.h>
#include <stdint.h>

class bdLANDiscoveryListener {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdLANDiscoveryListener)
    bdLANDiscoveryListener();
    virtual ~bdLANDiscoveryListener();  // vtable slot 0
    virtual void onDiscovery(bdGameInfoRef);  // vtable slot 2
    virtual void onDiscoveryFinished();  // vtable slot 3
    virtual void onRequest();  // vtable slot 4
};
static_assert(sizeof(bdLANDiscoveryListener) == 0x8, "bdLANDiscoveryListener");
