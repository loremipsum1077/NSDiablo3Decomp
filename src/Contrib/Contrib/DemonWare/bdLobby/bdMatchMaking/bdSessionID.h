// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMatchMaking/bdSessionID.h

#pragma once

#include "../../bdCore/bdSocket/bdSecurityID.h"
#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdSerializable.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdByteBuffer;

class bdSessionID : public bdTaskResult, public bdSerializable {  // size 0x28
public:
    BD_CLASS_ALLOCATOR(bdSessionID);
    bdSessionID();
    bdSessionID(const bdSessionID &);
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdBool serialize(bdByteBuffer & buffer) const;  // vtable slot 4
    virtual bdUInt sizeOf();  // vtable slot 3
    bdSecurityID m_sessionID;  // 0x10
};
static_assert(sizeof(bdSessionID) == 0x28, "bdSessionID");

