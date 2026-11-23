// Original file: Contrib/Contrib/DemonWare/bdLobby/bdRemoteTaskManager/bdRemoteTask.cpp
#include "bdRemoteTask.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../bdLobbyCommon/bdTaskResultProcessor.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"

// Release-build logging retains only the bdUseVAArgs calls and arguments below;
// the original message strings are not present in this binary.

bdRemoteTask::bdRemoteTask()
    : m_timeout(0.0f), m_status(BD_EMPTY), m_taskResult(NULL), m_taskResultList(NULL),
      m_numResults(0), m_maxNumResults(0), m_totalNumResults(0), m_transactionID(0),
      m_errorCode(BD_NO_ERROR), m_taskResultProcessor(NULL) {
    m_timer.reset();
}
bdRemoteTask::~bdRemoteTask() {}
bdRemoteTask::bdStatus bdRemoteTask::getStatus() {
    if (m_status == BD_PENDING && m_timeout > 0.0f) {
        bdFloat32 elapsedTime = m_timer.getElapsedTimeInSeconds();
        if (elapsedTime >= m_timeout) {
            m_status = BD_TIMED_OUT;
            m_byteResults = (bdByteBuffer *)NULL;
            bdUseVAArgs((double)m_timeout);
        }
    }
    return m_status;
}
bdUInt bdRemoteTask::getNumResults() const { return m_numResults; }
void bdRemoteTask::cancelTask() { m_status = BD_CANCELLED; }
bdUInt64 bdRemoteTask::getTransactionID() const { return m_transactionID; }
bdLobbyErrorCode bdRemoteTask::getErrorCode() const { return m_errorCode; }
void bdRemoteTask::setTaskResult(bdTaskResult * result, const bdUInt numResults) {
    m_taskResult = result;
    m_maxNumResults = numResults;
}
void bdRemoteTask::setTaskResultList(bdTaskResult ** result, const bdUInt numResults) {
    m_taskResultList = result;
    m_maxNumResults = numResults;
}
void bdRemoteTask::handleTaskReply(const bdByteBufferRef buffer) {
    bdUInt32 error = BD_HANDLE_TASK_FAILED;
    bdBool ok = buffer->readUInt64(m_transactionID);
    if (ok) {
        bdUseVAArgs(m_transactionID);
        ok = buffer->readUInt32(error);
        if (ok) {
            if (error == BD_ASYNCHRONOUS_ERROR) return;
            if (error == BD_NO_ERROR) {
                m_errorCode = BD_NO_ERROR;
                bdUByte8 taskId;
                ok = buffer->readUByte8(taskId) && deserializeTaskReply(buffer);
                m_status = BD_DONE;
            } else {
                m_errorCode = (bdLobbyErrorCode)error;
                m_status = BD_FAILED;
            }
        }
    }
    if (!ok) {
        m_errorCode = BD_HANDLE_TASK_FAILED;
        m_status = BD_FAILED;
        bdUseVAArgs();
    }
}
void bdRemoteTask::handleAsyncTaskReply(const bdByteBufferRef buffer) {
    bdUInt32 error = BD_HANDLE_TASK_FAILED;
    bdUseVAArgs(m_transactionID);
    bdBool ok = buffer->readUInt32(error);
    if (ok) {
        if (error == BD_NO_ERROR) {
            m_errorCode = BD_NO_ERROR;
            ok = deserializeTaskReply(buffer);
            m_status = BD_DONE;
        } else if (error == BD_ASYNCHRONOUS_ERROR) {
            m_errorCode = BD_HANDLE_TASK_FAILED;
            m_status = BD_FAILED;
            bdUseVAArgs(m_transactionID);
        } else {
            m_errorCode = (bdLobbyErrorCode)error;
            m_status = BD_FAILED;
        }
    }
    if (!ok) {
        m_errorCode = BD_HANDLE_TASK_FAILED;
        m_status = BD_FAILED;
        bdUseVAArgs(m_transactionID);
    }
}

bdBool bdRemoteTask::deserializeTaskReply(const bdByteBufferRef buffer) {
    bdBool ok = buffer->readUInt32(m_numResults);
    if (ok) {
        if (m_numResults) {
            ok = buffer->readUInt32(m_totalNumResults);
            if (ok) {
                if (m_taskResult) {
                    bdUInt taskResultSize = m_taskResult->sizeOf();
                    bdUInt numResultsToRead = m_numResults < m_maxNumResults ? m_numResults : m_maxNumResults;
                    bdByte8 * taskResult = (bdByte8 *)m_taskResult;
                    for (bdUInt i = 0; i < numResultsToRead && ok; ++i) {
                        ok = ((bdTaskResult *)taskResult)->deserialize(buffer);
                        taskResult += taskResultSize;
                    }
                    if (m_taskResultProcessor) ok = ok && m_taskResultProcessor->processResult(m_taskResult, numResultsToRead);
                } else if (m_taskResultList) {
                    bdUInt numResultsToRead = m_numResults < m_maxNumResults ? m_numResults : m_maxNumResults;
                    for (bdUInt i = 0; i < numResultsToRead && ok; ++i)
                        ok = m_taskResultList[i]->deserialize(buffer);
                } else bdUseVAArgs(m_transactionID);
            }
        } else if (m_taskResultProcessor) ok = m_taskResultProcessor->processResult(m_taskResult, 0);
    }
    if (!ok) bdUseVAArgs(m_transactionID);
    return ok;
}
void bdRemoteTask::start(const bdFloat32 timeout) {
    m_timer.start();
    m_status = BD_PENDING;
    m_timeout = timeout;
    m_byteResults = (bdByteBuffer *)NULL;
}
void bdRemoteTask::stop(const bdByteBufferRef results, const bdUInt64 id) {
    m_status = BD_DONE;
    m_byteResults = results;
    m_transactionID = id;
}
