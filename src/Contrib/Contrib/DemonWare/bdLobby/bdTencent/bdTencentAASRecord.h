// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTencent/bdTencentAASRecord.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdTencentAASRecord : public bdTaskResult {  // size 0x30
public:
    BD_CLASS_ALLOCATOR(bdTencentAASRecord);
    bdTencentAASRecord();
    virtual ~bdTencentAASRecord();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    enum bdRecordStatus {
        PENDING = 0,
        NOINFO = 1,
        MINOR = 2,
        ADULT = 3,
    };
    bdOnlineUserID m_userID;  // 0x08
    bdTencentAASRecord::bdRecordStatus m_ageStatus;  // 0x10
    bdUInt32 m_gameplaySeconds;  // 0x14
    bdUInt32 m_recordAge;  // 0x18
};
static_assert(sizeof(bdTencentAASRecord) == 0x30, "bdTencentAASRecord");


// Functions defined in this file (bodies are not in the DWARF):
//   line    22  void bdTencentAASRecord::operator delete(void * p)  [inlined into 1 functions]
