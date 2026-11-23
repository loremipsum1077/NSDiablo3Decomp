// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdNAT/bdNATTravListener.h

#pragma once

#include "../../bdCore/bdSocket/bdEndpoint.h"

class bdAddr;

class bdNATTravListener {  // size 0x8
    // vtable pointer
public:
    bdNATTravListener();
    virtual ~bdNATTravListener();  // vtable slot 0
    virtual void onNATAddrDiscovery(bdCommonAddrRef, const bdAddr &);  // vtable slot 2
    virtual void onNATAddrDiscoveryFailed(bdCommonAddrRef);  // vtable slot 3
};
static_assert(sizeof(bdNATTravListener) == 0x8, "bdNATTravListener");

