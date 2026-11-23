// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTitleUtilities/bdUserInfo.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdUserInfo : public bdTaskResult {  // size 0x60
public:
    BD_CLASS_ALLOCATOR(bdUserInfo);
    bdUserInfo();
    virtual ~bdUserInfo();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    bdOnlineUserID m_userID;  // 0x08
    bdNChar8 m_userName[64];  // 0x10
};
static_assert(sizeof(bdUserInfo) == 0x60, "bdUserInfo");


// Functions defined in this file (bodies are not in the DWARF):
//   line    22  void bdUserInfo::operator delete(void * p)  [inlined into 1 functions]
