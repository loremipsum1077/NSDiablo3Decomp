// Adopted from the DWARF reference; hand-maintained declarations.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdRemoteTaskManager/bdRemoteTaskManager.h

#pragma once

#include "../../bdCore/bdContainers/bdHashMap.h"
#include "../../bdCore/bdContainers/bdLinkedList.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../bdLobbyErrorCodes.h"
#include "bdRemoteTask.h"
#include "../bdLobbyConnection.h"
#include "bdTaskByteBuffer.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdStructBufferTask;
class bdStreamingTask;
class bdLobbyService;
class bdStructBufferSerializable;
typedef bdReference<bdStructBufferTask> bdStructBufferTaskRef;
class bdTaskParams;

typedef bdReference<bdLobbyConnection> bdLobbyConnectionRef;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

typedef bdReference<bdStreamingTask> bdStreamingTaskRef;

class bdRemoteTaskManager {  // size 0x78
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdRemoteTaskManager);

    bdRemoteTaskManager(bdLobbyConnectionRef, const bdBool);
    virtual ~bdRemoteTaskManager();  // vtable slot 0
    static void initTaskBuffer(bdTaskByteBufferRef & buffer, const bdUByte8 serviceID, const bdUByte8 taskID);
    bdLobbyErrorCode startTask(bdRemoteTaskRef & newTask, bdTaskByteBufferRef & queryParams);
    bdLobbyErrorCode startTask(bdRemoteTaskRef & newTask, bdTaskParams & taskParams);
    bdLobbyErrorCode startStreamingTask(bdStreamingTaskRef &, bdTaskByteBufferRef &);
    bdLobbyErrorCode startStreamingTask(bdStreamingTaskRef &, bdTaskParams &);
    bdLobbyErrorCode startStructTask(bdStructBufferTaskRef &, const bdUByte8, const bdUByte8, const bdStructBufferSerializable *, bdStructBufferSerializable *, bdLobbyService *);
    bdLobbyErrorCode startLSGTask(bdRemoteTaskRef &, const bdUByte8, const bdUByte8, const void *const, const bdUInt);
    bdUInt64 getConnectionID() const;
    bdUInt getTaskParamsMaxBufferSize() const;
    bdBool isConnectionMigrationSupported() const;
    friend class bdLobbyService;
protected:
    bdLobbyErrorCode sendWithTaskParams(bdRemoteTaskRef newTask, bdTaskParams & taskParams);
    bdLobbyErrorCode sendTask(bdRemoteTaskRef newTask, bdTaskByteBufferRef & queryParams);
    void handleTaskReply(const bdByteBufferRef buffer);
    void handleLSGTaskReply(const bdByteBufferRef buffer);
    void handleAsyncResult(const bdByteBufferRef buffer);
    void cleanUpAsyncState();
    bdLinkedList<bdReference<bdRemoteTask> > m_tasks;  // 0x08
    bdHashMap<unsigned long, bdReference<bdRemoteTask>, bdHashingClass> m_asyncTasks;  // 0x20
    bdHashMap<unsigned long, bdReference<bdByteBuffer>, bdHashingClass> m_asyncResults;  // 0x40
    bdLobbyConnectionRef m_lobbyConnection;  // 0x60
    bdBool m_encryptedConnection;  // 0x68
    bdUInt64 m_connectionID;  // 0x70
};
static_assert(sizeof(bdRemoteTaskManager) == 0x78, "bdRemoteTaskManager");


// Functions defined in this file (bodies are not in the DWARF):
//   line    32  void bdRemoteTaskManager::operator delete(void * p)  [inlined into 1 functions]
//   line    32  void * bdRemoteTaskManager::operator new(size_t nbytes)  [inlined into 1 functions]
