// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMail/bdMailInfo.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../bdLobbyCommon/bdUserAccountID.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdMailInfo : public bdTaskResult {  // size 0xb0
public:
    BD_CLASS_ALLOCATOR(bdMailInfo);
    bdMailInfo();
    virtual ~bdMailInfo();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt32 sizeOf();  // vtable slot 3
    bdUserAccountID m_senderID;  // 0x08
    bdNChar8 m_senderName[64];  // 0x40
    bdUInt64 m_mailID;  // 0x80
    bdUInt32 m_timeStamp;  // 0x88
    bdUInt32 m_size;  // 0x8c
    bdUInt16 m_mailCategory;  // 0x90
    bdBool m_isRead;  // 0x92
    bdBool m_isUndelivered;  // 0x93
    bdUInt32 m_lastUpdated;  // 0x94
    bdUInt32 m_messageDuration;  // 0x98
    bdUInt32 m_secondsToExpiration;  // 0x9c
};
static_assert(sizeof(bdMailInfo) == 0xb0, "bdMailInfo");


// Functions defined in this file (bodies are not in the DWARF):
//   line    18  bdMailInfo::bdMailInfo(const bdMailInfo &)  [inlined into 1 functions]
//   line    22  void bdMailInfo::operator delete(void * p)  [inlined into 1 functions]
