// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMail/bdMailBody.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdMailBody : public bdTaskResult {  // size 0x30
public:
    BD_CLASS_ALLOCATOR(bdMailBody);
    bdMailBody();
    bdMailBody(void *, const bdUInt32);
    virtual ~bdMailBody();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt32 sizeOf();  // vtable slot 3
    void * m_mailBody;  // 0x08
    bdUInt32 m_size;  // 0x10
    bdUInt64 m_mailID;  // 0x18
};
static_assert(sizeof(bdMailBody) == 0x30, "bdMailBody");


// Functions defined in this file (bodies are not in the DWARF):
//   line    19  void bdMailBody::operator delete(void * p)  [inlined into 1 functions]
