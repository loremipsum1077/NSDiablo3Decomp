// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdStorage/bdFileData.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdFileData : public bdTaskResult {  // size 0x28
public:
    BD_CLASS_ALLOCATOR(bdFileData);
    bdFileData();
    virtual ~bdFileData();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    void * m_fileData;  // 0x08
    bdUInt m_fileSize;  // 0x10
};
static_assert(sizeof(bdFileData) == 0x28, "bdFileData");

