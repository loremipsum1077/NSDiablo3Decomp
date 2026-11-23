// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTitleUtilities/bdTimeStamp.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdTimeStamp : public bdTaskResult {  // size 0x20
public:
    BD_CLASS_ALLOCATOR(bdTimeStamp);
    bdTimeStamp();
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    bdUInt32 m_timeStamp;  // 0x08
};
static_assert(sizeof(bdTimeStamp) == 0x20, "bdTimeStamp");


// Functions defined in this file (bodies are not in the DWARF):
//   line    16  bdTimeStamp::~bdTimeStamp()  [0x7100b26a70 (+1 more), inlined into 4 functions]
//   line    19  void bdTimeStamp::operator delete(void * p)  [inlined into 1 functions]
