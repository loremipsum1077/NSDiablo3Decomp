// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTencent/bdTencentResponse.h

#pragma once

#include "../bdLobbyCommon/bdStructBufferSerializable.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdStructBufferDeserializer;

// bdTencentClientDecidedRewardResponse (size 0x28, fast array of rewards) lives in this header
// originally; trimmed here because its element type is not adopted and no function uses it.

class bdTSSStructMessage : public bdStructBufferSerializable {  // size 0x420
public:
    BD_CLASS_ALLOCATOR(bdTSSStructMessage);
    bdTSSStructMessage();
    virtual ~bdTSSStructMessage();  // vtable slot 0
    void reset();
    virtual bdBool deserialize(bdStructBufferDeserializer & deserializer);  // vtable slot 3
    bdUInt32 m_creditRating;  // 0x08
    bdUInt32 m_queryInfoSize;  // 0x0c
    bdUByte8 m_queryInfo[1024];  // 0x10
};
static_assert(sizeof(bdTSSStructMessage) == 0x420, "bdTSSStructMessage");


// Functions defined in this file (bodies are not in the DWARF):
//   line    21  bdTencentClientDecidedRewardResponse::~bdTencentClientDecidedRewardResponse()  [inline]
//   line    24  void bdTencentClientDecidedRewardResponse::operator delete(void * p)  [inline]
//   line    43  void bdTSSStructMessage::operator delete(void * p)  [inlined into 1 functions]
