// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMessaging/bdChannelInfo.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdChannelInfo : public bdTaskResult {  // size 0x38
public:
    BD_CLASS_ALLOCATOR(bdChannelInfo);
    bdChannelInfo();
    virtual ~bdChannelInfo();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    bdUInt64 m_channelID;  // 0x08
    bdUInt32 m_numSubscribers;  // 0x10
    bdUInt32 m_maxSubscribers;  // 0x14
    bdOnlineUserID m_adminID;  // 0x18
    bdBool m_isPasswordRequired;  // 0x20
    bdBool m_isPublicChannel;  // 0x21
    bdUInt32 m_dataVersion;  // 0x24
};
static_assert(sizeof(bdChannelInfo) == 0x38, "bdChannelInfo");


// Functions defined in this file (bodies are not in the DWARF):
//   line    23  void bdChannelInfo::operator delete(void * p)  [inlined into 1 functions]
