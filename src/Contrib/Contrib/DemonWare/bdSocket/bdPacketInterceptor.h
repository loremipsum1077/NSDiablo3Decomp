// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdPacketInterceptor.h

#pragma once

#include "../bdCore/bdMemory/bdMemory-std.h"
#include "../bdCore/bdSocket/bdDefaultSocket.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdAddr;

class bdPacketInterceptor {  // size 0x8
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdPacketInterceptor);
    bdPacketInterceptor();
    virtual ~bdPacketInterceptor();  // vtable slot 0
    virtual bdBool acceptPacket(bdDefaultSocket *, bdAddr, void *, const bdUInt, const bdUByte8) = 0;  // vtable slot 2
};
static_assert(sizeof(bdPacketInterceptor) == 0x8, "bdPacketInterceptor");

