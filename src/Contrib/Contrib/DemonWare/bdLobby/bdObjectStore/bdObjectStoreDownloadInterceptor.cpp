// Original file: Contrib/Contrib/DemonWare/bdLobby/bdObjectStore/bdObjectStoreDownloadInterceptor.cpp
#include "bdObjectStoreDownloadInterceptor.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"













bdObjectStoreDownloadInterceptor::~bdObjectStoreDownloadInterceptor()
{
}

// LINKER-REMOVED with the vtable (the original keeps no vtable for this class and DemonWare has no
// DWARF). Invented body: append to the caller's buffer up to m_contentSize and report the bytes
// taken; a short count makes curl stop the transfer.
bdUInt bdObjectStoreDownloadInterceptor::handleDownload(void * data, bdUInt32 dataSize)
{
    if (m_data == NULL || m_totalBytesReceived + dataSize > m_contentSize)
    {
        return 0;
    }
    bdMemcpy(m_data + m_totalBytesReceived, data, dataSize);
    m_totalBytesReceived += dataSize;
    return dataSize;
}
