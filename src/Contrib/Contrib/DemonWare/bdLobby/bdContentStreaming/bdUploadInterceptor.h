// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdContentStreaming/bdUploadInterceptor.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdUploadInterceptor {  // size 0x8
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdUploadInterceptor);
    bdUploadInterceptor();
    virtual ~bdUploadInterceptor();  // vtable slot 0
    virtual bdUInt handleUpload(void *, bdUInt32, bdUInt32) = 0;  // vtable slot 2
};
static_assert(sizeof(bdUploadInterceptor) == 0x8, "bdUploadInterceptor");

