// Adopted from original DWARF declarations.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdRemoteTaskManager/bdRemoteTask.h

#pragma once

#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdCore/bdTiming/bdStopwatch.h"
#include "../../bdCore/bdUtilities/bdTask.h"
#include "../bdLobbyErrorCodes.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdTaskResult;
class bdTaskResultProcessor;

class bdRemoteTask : public bdTask {  // size 0x60
    friend class bdRemoteTaskManager;
    friend class bdObjectStoreStreaming;  // not in the DWARF; it reads m_errorCode and writes m_status directly
public:
    BD_CLASS_ALLOCATOR(bdRemoteTask);
    typedef bdTask::bdStatus bdStatus;
    virtual bdRemoteTask::bdStatus getStatus();  // vtable slot 2
    bdUInt getNumResults() const;
    bdUInt getTotalNumResults() const;
    void cancelTask();
    bdByteBufferRef getByteResults() const;
    bdStopwatch getTimer() const;
    bdFloat32 getTimeout() const;
    void setTimeout(const bdFloat32);
    bdUInt64 getTransactionID() const;
    bdLobbyErrorCode getErrorCode() const;
    void setTaskResult(bdTaskResult * result, const bdUInt numResults);
    void setTaskResultList(bdTaskResult ** result, const bdUInt numResults);
    void setTaskResultProcessor(bdTaskResultProcessor *);
    bdTaskResult * getTaskResult();
    void handleTaskReply(const bdByteBufferRef buffer);
    virtual void handleAsyncTaskReply(const bdByteBufferRef buffer);  // vtable slot 3
    bdRemoteTask();
    virtual ~bdRemoteTask();  // vtable slot 0
protected:
    explicit bdRemoteTask(bdRemoteTask::bdStatus);
    virtual void start(const bdFloat32 timeout);  // vtable slot 4
    void stop(const bdByteBufferRef results, const bdUInt64 id);
    virtual bdBool deserializeTaskReply(const bdByteBufferRef buffer);  // vtable slot 5
    bdStopwatch m_timer;  // 0x10
    bdFloat32 m_timeout;  // 0x18
    bdRemoteTask::bdStatus m_status;  // 0x1c
    bdByteBufferRef m_byteResults;  // 0x20
    bdTaskResult * m_taskResult;  // 0x28
    bdTaskResult ** m_taskResultList;  // 0x30
    bdUInt m_numResults;  // 0x38
    bdUInt m_maxNumResults;  // 0x3c
    bdUInt m_totalNumResults;  // 0x40
    bdUInt64 m_transactionID;  // 0x48
    bdLobbyErrorCode m_errorCode;  // 0x50
    bdTaskResultProcessor * m_taskResultProcessor;  // 0x58
};
static_assert(sizeof(bdRemoteTask) == 0x60, "bdRemoteTask");


// Functions defined in this file (bodies are not in the DWARF):
//   line    57  void bdRemoteTask::operator delete(void * p)  [inlined into 1 functions]
//   line    57  void * bdRemoteTask::operator new(size_t nbytes)  [inlined into 2 functions]
