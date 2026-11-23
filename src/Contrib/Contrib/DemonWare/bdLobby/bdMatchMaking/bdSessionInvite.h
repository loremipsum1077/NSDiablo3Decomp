// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMatchMaking/bdSessionInvite.h

#pragma once

#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "bdSessionID.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdSessionInvite : public bdTaskResult {  // size 0x490
public:
    BD_CLASS_ALLOCATOR(bdSessionInvite)
    bdSessionInvite();
    virtual ~bdSessionInvite();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    bdOnlineUserID m_senderID;  // 0x08
    bdNChar8 m_senderName[64];  // 0x10
    bdSessionID m_sessionID;  // 0x50
    bdUByte8 m_attachment[1024];  // 0x78
    bdUInt m_attachmentSize;  // 0x478
};
static_assert(sizeof(bdSessionInvite) == 0x490, "bdSessionInvite");
