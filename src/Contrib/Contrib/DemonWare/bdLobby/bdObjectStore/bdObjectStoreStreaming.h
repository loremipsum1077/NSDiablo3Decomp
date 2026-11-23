// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdObjectStore/bdObjectStoreStreaming.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdHTTP/bdHTTP.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyErrorCodes.h"
#include "../bdRemoteTaskManager/bdStructBufferTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdDownloadInterceptor;
class bdObjectStoreDownloadInterceptor;
class bdObjectStoreMetadata;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdObjectStoreStreaming {  // size 0x50
    // vtable pointer
public:
    enum bdStatus {
        BD_READY = 0,
        BD_PRE_HTTP_OPERATION = 1,
        BD_HTTP_OPERATION = 2,
        BD_THROTTLED = 3,
        BD_POST_HTTP_OPERATION = 4,
        BD_DONE = 5,
        BD_FAILED = 6,
        BD_PAUSING = 7,
        BD_PAUSED = 8,
    };
public:
    BD_CLASS_ALLOCATOR(bdObjectStoreStreaming);
    bdObjectStoreStreaming(bdRemoteTaskManager *const, const bdNChar8 *);
    virtual ~bdObjectStoreStreaming();  // vtable slot 0
    void pump();
    bdRemoteTaskRef startDownload();
    bdBool initDownload(bdObjectStoreDownloadInterceptor *, bdStructBufferTaskRef &, const bdNChar8 *const, const bdObjectStoreMetadata *);
    void abortHTTPOperation();
    bdObjectStoreStreaming::bdStatus getState() const;
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x08
    bdRemoteTaskRef m_overallTask;  // 0x10
    bdStructBufferTaskRef m_remoteTask;  // 0x18
    bdHTTP * m_http;  // 0x20
    enum bdObjectStoreOperation {
        BD_IDLE = 0,
        BD_DOWNLOAD_OBJECT = 1,
    };
    bdUInt m_operation;  // 0x28
    bdObjectStoreStreaming::bdStatus m_state;  // 0x2c
    bdObjectStoreDownloadInterceptor * m_downloadHandler;  // 0x30
    const bdNChar8 * m_authorizationHeader;  // 0x38
    const bdObjectStoreMetadata * m_metadata;  // 0x40
    bdNChar8 * m_certificateAuthorityPath;  // 0x48
    bdRemoteTaskRef start(bdObjectStoreStreaming::bdObjectStoreOperation operation);
    void setState(bdObjectStoreStreaming::bdStatus newState, bdLobbyErrorCode errorCode);
    void pumpPreHTTP();
    void pumpHTTP();
    void exitHTTP();
    void exitHTTPFailed();
    virtual void initHTTP();  // vtable slot 2
    void finalizeHTTP();
    bdHTTP::bdStatus startContentDownload(const bdNChar8 *const downloadURL, bdDownloadInterceptor * downloadHandler, bdUInt64 transactionID, const bdNChar8 * authorizationHeader);
};
static_assert(sizeof(bdObjectStoreStreaming) == 0x50, "bdObjectStoreStreaming");


// Functions defined in this file (bodies are not in the DWARF):
//   line    47  void bdObjectStoreStreaming::operator delete(void * p)  [inlined into 1 functions]
