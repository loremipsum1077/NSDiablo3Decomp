// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdCounter/bdCounterValue.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdSerializable.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdByteBuffer;

class bdCounterValue : public bdTaskResult, public bdSerializable {  // size 0x30
public:
    BD_CLASS_ALLOCATOR(bdCounterValue);
    bdCounterValue();
    // destructor implicit: DWARF has ~bdCounterValue and the move constructor bdCounterValue(bdCounterValue &&) both at the class line (19) like implicit members; a user-declared destructor would suppress the move constructor the original inlines
    virtual bdBool serialize(bdByteBuffer & buffer) const;  // vtable slot 4
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    bdUInt32 m_counterID;  // 0x10
    bdInt64 m_counterValue;  // 0x18
};
static_assert(sizeof(bdCounterValue) == 0x30, "bdCounterValue");
