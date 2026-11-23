// Reconstructed from DWARF call sites and original control flow.
#include "bdRemoteTaskManager.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../bdLobbyCommon/bdTaskParams.h"

bdRemoteTaskManager::bdRemoteTaskManager(
    bdLobbyConnectionRef connection, const bdBool useEncryption)
    : m_asyncTasks(4, 0.75f)
    , m_asyncResults(4, 0.75f)
    , m_lobbyConnection(connection)
    , m_encryptedConnection(useEncryption)
    , m_connectionID(0)
{
}
bdRemoteTaskManager::~bdRemoteTaskManager()
{
    while (!m_tasks.isEmpty())
    {
        m_tasks.getHead()->m_errorCode = BD_START_TASK_FAILED;
        m_tasks.getHead()->m_status = bdTask::BD_FAILED;
        m_tasks.removeHead();
    }
}
void bdRemoteTaskManager::initTaskBuffer(
    bdTaskByteBufferRef& buffer, const bdUByte8 serviceID, const bdUByte8 taskID)
{
    buffer->write(&serviceID, 1);
    buffer->writeUByte8(taskID);
}
bdLobbyErrorCode bdRemoteTaskManager::startTask(
    bdRemoteTaskRef& newTask, bdTaskByteBufferRef& queryParams)
{
    newTask = new bdRemoteTask;
    return sendTask(newTask, queryParams);
}
bdLobbyErrorCode bdRemoteTaskManager::startTask(bdRemoteTaskRef& newTask, bdTaskParams& taskParams)
{
    newTask = new bdRemoteTask;
    return sendWithTaskParams(newTask, taskParams);
}
#line 159
bdLobbyErrorCode bdRemoteTaskManager::sendWithTaskParams(
    bdRemoteTaskRef newTask, bdTaskParams& taskParams)
{
    bdLobbyErrorCode startTaskResult = BD_START_TASK_FAILED;

    if (taskParams.serializedOk())
    {

        bdTaskByteBufferRef buffer = taskParams.getTaskBuffer();


        startTaskResult = sendTask(newTask, buffer);



        if (startTaskResult == BD_NO_ERROR)
        {

            bdTaskResult* taskResults;
            bdTaskResult** taskResultRefs;
            bdUInt maxNumResults;


            if (taskParams.getResultBindings(taskResults, taskResultRefs, maxNumResults))
            {

                if (taskResults)
                {
                    newTask->setTaskResult(taskResults, maxNumResults);
                }

                else
                {
                    newTask->setTaskResultList(taskResultRefs, maxNumResults);
                }
            }
        }


        else
        {
            bdUseVAArgs(startTaskResult);
        }
    }
    else
    {
        bdUseVAArgs(taskParams.m_serviceID, taskParams.m_taskID);
    }

    return startTaskResult;
}
#line 69
bdLobbyErrorCode bdRemoteTaskManager::sendTask(
    bdRemoteTaskRef newTask, bdTaskByteBufferRef& queryParams)
{
    bdBool ok = queryParams->writeNoType();
    if (ok)
        ok = m_lobbyConnection->sendTask(
            queryParams, queryParams->getDataSize(), m_encryptedConnection);
    if (ok)
    {
        m_tasks.addTail(newTask);
        newTask->start(0.0f);
        return BD_NO_ERROR;
    }
    newTask->m_status = bdTask::BD_FAILED;
    newTask->m_errorCode = BD_SEND_FAILED;
    return BD_SEND_FAILED;
}
void bdRemoteTaskManager::handleLSGTaskReply(const bdByteBufferRef buffer)
{
    bdRemoteTaskRef task = !m_tasks.isEmpty() ? m_tasks.getHead() : bdRemoteTaskRef();
    if (task.notNull() && task->getStatus() == bdTask::BD_PENDING)
    {
        task->handleTaskReply(buffer);
        if (task->getStatus() == bdTask::BD_PENDING)
        {
            bdByteBufferRef asyncResult;
            if (m_asyncResults.remove(task->getTransactionID(), asyncResult))
                task->handleAsyncTaskReply(asyncResult);
            else
                m_asyncTasks.put(task->getTransactionID(), task);
        }
    }
    m_tasks.removeHead();
}
void bdRemoteTaskManager::handleTaskReply(const bdByteBufferRef buffer)
{
    bdRemoteTaskRef task = m_tasks.getHead();
    if (task.notNull() && task->getStatus() == bdTask::BD_PENDING)
    {
        bdUInt64 transactionID;
        bdBool ok = buffer->read(transactionID);
        if (ok)
        {
            task->stop(buffer, transactionID);
            bdUseVAArgs(m_connectionID, transactionID);
        }
        else
            bdUseVAArgs();
    }
    m_tasks.removeHead();
}
void bdRemoteTaskManager::handleAsyncResult(const bdByteBufferRef buffer)
{
    bdUInt64 transactionID = 0;
    bdRemoteTaskRef task;
    bdBool ok = buffer->readUInt64(transactionID);
    if (ok)
    {
        if (m_asyncTasks.get(transactionID, task))
        {
            bdRemoteTask::bdStatus status = task->getStatus();
            if (status == bdTask::BD_PENDING)
            {
                task->handleAsyncTaskReply(buffer);
                if (task->getStatus() != bdTask::BD_PENDING)
                    m_asyncTasks.remove(transactionID);
            }
            else
                bdUseVAArgs();
        }
        else if (m_tasks.getSize())
        {
            bdUseVAArgs(transactionID);
            m_asyncResults.put(transactionID, buffer);
        }
        else
            bdUseVAArgs(transactionID);
    }
    else
        bdUseVAArgs();
}
#line 350
// NON_MATCHING: 896 vs 848 bytes (908 before the DWARF-line re-lay with the != test, 876 before h17's remove form); iterator advancement and removal share control
// flow differently: the iterator is kept in two registers (mov at the loop head), using an extra saved register.
void bdRemoteTaskManager::cleanUpAsyncState()
{
    bdHashMap<bdUInt64, bdRemoteTaskRef, bdHashingClass>::Iterator i = m_asyncTasks.getIterator();
    while (i)
    {
        if (m_asyncTasks.getValue(i)->getStatus() != bdTask::BD_PENDING)
        {
            bdUseVAArgs(m_asyncTasks.getKey(i));


            m_asyncTasks.remove(i);
        }
        else
        {
            m_asyncTasks.next(i);
        }
    }

    if (m_tasks.getSize() == 0 && m_asyncTasks.getSize() == 0 && m_asyncResults.getSize() > 0)
    {


        bdHashMap<bdUInt64, bdByteBufferRef, bdHashingClass>::Iterator j = m_asyncResults.getIterator();
        while (j)
        {
            bdUseVAArgs(m_asyncResults.getKey(j));
            m_asyncResults.next(j);
        }
        m_asyncResults.clear();
    }
}
#line 201

bdUInt bdRemoteTaskManager::getTaskParamsMaxBufferSize() const
{
    return m_lobbyConnection->getMaxSendMessageSizeWithoutOverhead();
}
