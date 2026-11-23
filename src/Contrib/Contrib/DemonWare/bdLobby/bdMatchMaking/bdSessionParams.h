// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMatchMaking/bdSessionParams.h

#pragma once

#include "../bdLobbyCommon/bdSerializable.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdByteBuffer;

class bdSessionParams : public bdSerializable {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdSessionParams);
    bdSessionParams();
    virtual ~bdSessionParams();  // vtable slot 0
    virtual bdBool serialize(bdByteBuffer &) const;  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 4
};
static_assert(sizeof(bdSessionParams) == 0x18, "bdSessionParams");

