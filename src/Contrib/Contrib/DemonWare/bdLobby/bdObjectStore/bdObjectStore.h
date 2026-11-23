// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdObjectStore/bdObjectStore.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdEnvironmentString.h"
#include "../bdLobbyErrorCodes.h"
#include "bdObjectStoreDownloadInterceptor.h"
#include "bdObjectStoreStreaming.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdObjectStoreDeleteUserObjectRequest;
class bdObjectStoreDeleteUserObjectResponse;
class bdObjectStoreGetPublisherObjectRequest;
class bdObjectStoreGetPublisherObjectResponse;
class bdObjectStoreGetPublisherObjectsRequest;
class bdObjectStoreGetPublisherObjectsResponse;
class bdObjectStoreGetUserObjectIfChangedRequest;
class bdObjectStoreGetUserObjectIfChangedResponse;
class bdObjectStoreGetUserObjectMetadataRequest;
class bdObjectStoreGetUserObjectMetadataResponse;
class bdObjectStoreGetUserObjectRequest;
class bdObjectStoreGetUserObjectResponse;
class bdObjectStoreGetUserObjectsVectorizedRequest;
class bdObjectStoreGetUserObjectsVectorizedResponse;
class bdObjectStoreListPublisherObjectsByCategoryRequest;
class bdObjectStoreListPublisherObjectsByCategoryResponse;
class bdObjectStoreListUserObjectsRequest;
class bdObjectStoreListUserObjectsResponse;
class bdObjectStoreUploadUserObjectRequest;
class bdObjectStoreUploadUserObjectResponse;
class bdObjectStoreUploadUserObjectsVectorizedRequest;
class bdObjectStoreUploadUserObjectsVectorizedResponse;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdObjectStore {  // size 0xa8
public:
    BD_CLASS_ALLOCATOR(bdObjectStore);
    bdObjectStore(bdRemoteTaskManager *const, bdEnvironment, const bdNChar8 *);
    ~bdObjectStore();
    void pump();
    void abortHTTPOperation();
    bdRemoteTaskRef getUserObject(const bdObjectStoreGetUserObjectRequest *, bdObjectStoreGetUserObjectResponse *);
    bdRemoteTaskRef getPublisherObject(const bdObjectStoreGetPublisherObjectRequest *, bdObjectStoreGetPublisherObjectResponse *);
    bdRemoteTaskRef getPublisherObjects(bdObjectStoreGetPublisherObjectsRequest *, bdObjectStoreGetPublisherObjectsResponse *);
    bdRemoteTaskRef listPublisherObjectsByCategory(const bdObjectStoreListPublisherObjectsByCategoryRequest *, bdObjectStoreListPublisherObjectsByCategoryResponse *);
    bdRemoteTaskRef getUserObjectIfChanged(const bdObjectStoreGetUserObjectIfChangedRequest *, bdObjectStoreGetUserObjectIfChangedResponse *);
    bdRemoteTaskRef uploadUserObject(const bdObjectStoreUploadUserObjectRequest *, bdObjectStoreUploadUserObjectResponse *);
    bdRemoteTaskRef listUserObjects(const bdObjectStoreListUserObjectsRequest *, bdObjectStoreListUserObjectsResponse *);
    bdRemoteTaskRef deleteUserObject(const bdObjectStoreDeleteUserObjectRequest *, bdObjectStoreDeleteUserObjectResponse *);
    bdRemoteTaskRef getUserObjectMetadata(const bdObjectStoreGetUserObjectMetadataRequest *, bdObjectStoreGetUserObjectMetadataResponse *);
    bdRemoteTaskRef getUserObjectsVectorized(const bdObjectStoreGetUserObjectsVectorizedRequest *, bdObjectStoreGetUserObjectsVectorizedResponse *);
    bdRemoteTaskRef uploadUserObjectsVectorized(const bdObjectStoreUploadUserObjectsVectorizedRequest *, bdObjectStoreUploadUserObjectsVectorizedResponse *);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
    bdEnvironment m_environment;  // 0x08
    bdObjectStoreStreaming m_streaming;  // 0x10
    bdObjectStoreDownloadInterceptor m_downloadHandler;  // 0x60
    bdObjectStoreGetPublisherObjectsRequest * m_batchRequest;  // 0x80
    bdObjectStoreGetPublisherObjectsResponse * m_batchResponse;  // 0x88
    bdUInt m_batchIndex;  // 0x90
    bdBool m_batchInProgress;  // 0x94
    bdRemoteTaskRef m_batchOverallRemoteTaskRef;  // 0x98
    bdRemoteTaskRef m_batchRemoteTaskRef;  // 0xa0
    bdRemoteTaskRef getPublisherObjectFromBatch();
    void resetAfterBatch();
private:
    void checkTaskResult(const bdLobbyErrorCode startTaskResult) const;
};
static_assert(sizeof(bdObjectStore) == 0xa8, "bdObjectStore");


// Functions defined in this file (bodies are not in the DWARF):
//   line    54  void bdObjectStore::operator delete(void * p)  [inlined into 1 functions]
//   line    54  void * bdObjectStore::operator new(size_t nbytes)  [inline]
