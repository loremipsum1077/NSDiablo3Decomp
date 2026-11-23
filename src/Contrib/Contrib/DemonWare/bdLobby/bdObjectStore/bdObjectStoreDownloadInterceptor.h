// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdObjectStore/bdObjectStoreDownloadInterceptor.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../bdContentStreaming/bdDownloadInterceptor.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdObjectStoreDownloadInterceptor : public bdDownloadInterceptor {  // size 0x20
public:
    BD_CLASS_ALLOCATOR(bdObjectStoreDownloadInterceptor);
    bdObjectStoreDownloadInterceptor();
    virtual ~bdObjectStoreDownloadInterceptor();  // vtable slot 0
    bdUInt handleDownload(bdUByte8 * data, bdUInt32 dataSize);
    virtual bdUInt handleDownload(void *, bdUInt32);  // vtable slot 3
    void setData(bdUByte8 *, bdUInt64);
protected:
    bdUInt32 m_totalBytesReceived;  // 0x08
    bdUByte8 * m_data;  // 0x10
    bdUInt64 m_contentSize;  // 0x18
};
static_assert(sizeof(bdObjectStoreDownloadInterceptor) == 0x20, "bdObjectStoreDownloadInterceptor");


// Functions defined in this file (bodies are not in the DWARF):
//   line    23  void bdObjectStoreDownloadInterceptor::operator delete(void * p)  [inline]
