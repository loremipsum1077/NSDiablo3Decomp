// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdContentStreaming/bdDownloadInterceptor.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdDownloadInterceptor {  // size 0x8
    // vtable pointer
public:
    bdDownloadInterceptor();
    virtual ~bdDownloadInterceptor();  // vtable slot 0
    virtual bdBool handleContentLength(bdUInt32, bdBool);  // vtable slot 2
    virtual bdUInt handleDownload(void *, bdUInt32) = 0;  // vtable slot 3
};
static_assert(sizeof(bdDownloadInterceptor) == 0x8, "bdDownloadInterceptor");

