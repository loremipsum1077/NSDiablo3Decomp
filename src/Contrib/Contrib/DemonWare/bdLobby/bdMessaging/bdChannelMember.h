// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMessaging/bdChannelMember.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdChannelMember : public bdTaskResult {  // size 0x68
public:
    BD_CLASS_ALLOCATOR(bdChannelMember);
    bdChannelMember();
    virtual ~bdChannelMember();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    bdUInt64 m_userID;  // 0x08
    bdNChar8 m_userName[64];  // 0x10
    bdUInt32 m_subscribedTimestamp;  // 0x50
};
static_assert(sizeof(bdChannelMember) == 0x68, "bdChannelMember");


// Functions defined in this file (bodies are not in the DWARF):
//   line    22  void bdChannelMember::operator delete(void * p)  [inlined into 1 functions]
