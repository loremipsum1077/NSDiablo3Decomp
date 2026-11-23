// Original file: Contrib/Contrib/DemonWare/bdLobby/bdStorage/bdStorage.h
// Minimal declarations and consumed inline body authenticated from original DWARF.
#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../bdLobbyCommon/bdUserAccountID.h"
#include "bdFileInfo.h"

class bdRemoteTask;
class bdRemoteTaskManager;
class bdFileData;
class bdContextUserStorageFileInfo;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;
typedef bdContextUserStorageFileInfo bdUserStorageFileInfo;

// Preserve the original class and inline-delete declaration lines.
































class bdStorage {  // size 0x20
public:
    // Original header inline body at line 54.
    static void operator delete(void *p) { bdMemory::deallocate(p); } static void* operator new(size_t nbytes) { return bdMemory::allocate(nbytes); }

    virtual ~bdStorage();
    bdRemoteTaskRef uploadFile(const bdNChar8 *const fileName,
                              const void *const fileData,
                              const bdUInt fileSize,
                              const bdFileInfo::bdVisibility fileVisibility,
                              const bdUserID &ownerID,
                              bdUserStorageFileInfo *const fileInfo);
    bdRemoteTaskRef getFile(const bdNChar8 *fileName, bdFileData *const fileData,
                           const bdUserID &ownerUID);
    bdRemoteTaskRef listFilesByOwner(const bdUserID &ownerID, const bdUInt32 startDate,
                                    bdUserStorageFileInfo *fileList,
                                    const bdUInt16 maxNumResults, const bdUInt16 offset,
                                    const bdNChar8 *const fileName);
    bdRemoteTaskRef getPublisherFile(const bdNChar8 *const fileName,
                                    bdFileData *const fileData);
protected:
    friend class bdLobbyService;
    bdStorage(bdRemoteTaskManager *const remoteTaskManager, const bdNChar8 *context);
    bdRemoteTaskManager *m_remoteTaskManager;  // 0x08
    bdNChar8 m_context[16];  // 0x10
};
static_assert(sizeof(bdStorage) == 0x20, "bdStorage");
