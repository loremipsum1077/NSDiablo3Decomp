// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMatchMaking/bdMatchMaking.cpp
// Diagnostic text is inferred; this release retains only variadic arguments.
// The other task methods (requestSessionID, findSessionFromID, notifyJoin, ...) were
// stripped by the linker (low_pc 0 in the DWARF, no bytes) and are not reconstructed.
#include "bdMatchMaking.h"
#include "bdMatchMakingInfo.h"
#include "bdSessionID.h"
#include "bdSessionParams.h"
#include "../bdRemoteTaskManager/bdRemoteTaskManager.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"














bdMatchMaking::bdMatchMaking(bdRemoteTaskManager *const remoteTaskManager,
                             const bdNChar8 * context)
    : m_remoteTaskManager(remoteTaskManager)
{
    if (context == NULL)
    {
        bdStrlcpy(m_context, "", sizeof(m_context));
    }
    else
    {
        bdStrlcpy(m_context, context, sizeof(m_context));
    }
}

bdMatchMaking::~bdMatchMaking()
{
    m_remoteTaskManager = NULL;
}

bdRemoteTaskRef bdMatchMaking::createSession(bdMatchMakingInfo * sessionInfo,
                                             bdSessionID * sessionCredentials)
{
    bdRemoteTaskRef task(NULL);
    const bdUInt numParameters = 1;
    const bdUInt taskSize = 72 + numParameters + sessionInfo->sizeOf();
    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(taskSize, true);
    bdRemoteTaskManager::initTaskBuffer(buffer, 21, 1);

    const bdBool wroteParam = sessionInfo->serialize(*buffer);
    if (wroteParam)
    {
        const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, buffer);
        if (startTaskResult != BD_NO_ERROR)
        {
            bdLogWarn("bdMatchMaking", "Failed to start task: %u", startTaskResult);
        }
        else
        {

            const bdUInt numResults = 1;
            task->setTaskResult(sessionCredentials, numResults);
        }
    }
    else
    {
        bdLogWarn("bdMatchMaking", "Failed to serialize task parameters");
    }

    return task;
}

bdRemoteTaskRef bdMatchMaking::updateSession(const bdSessionID & sessionID,
                                             bdMatchMakingInfo *const sessionInfo)
{
    bdRemoteTaskRef task(NULL);
    const bdUInt numParameters = 2;
    const bdUInt taskSize = 72 + numParameters + sizeof(bdSessionID) + sessionInfo->sizeOf();
    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(taskSize, true);
    bdRemoteTaskManager::initTaskBuffer(buffer, 21, 2);

    bdBool wroteParam = sessionID.serialize(*buffer);
    wroteParam = wroteParam && sessionInfo->serialize(*buffer);

    if (wroteParam)
    {
        const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, buffer);
        if (startTaskResult != BD_NO_ERROR)
        {
            bdLogWarn("bdMatchMaking", "Failed to start task: %u", startTaskResult);
        }
    }
    else
    {
        bdLogWarn("bdMatchMaking", "Failed to serialize task parameters");
    }

    return task;
}

bdRemoteTaskRef bdMatchMaking::updateSessionPlayers(const bdSessionID & sessionID,
                                                    bdMatchMakingInfo *const sessionInfo,
                                                    bdUInt32 numPlayers)
{
    bdRemoteTaskRef task(NULL);
    const bdUInt numParameters = 3;
    const bdUInt taskSize = 72
                            + numParameters
                            + sessionInfo->sizeOf()
                            + sizeof(bdSessionID)
                            + sizeof(numPlayers);
    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(taskSize, true);
    bdRemoteTaskManager::initTaskBuffer(buffer, 21, 12);

    bdBool wroteParam = sessionID.serialize(*buffer);
    wroteParam = wroteParam && buffer->writeUInt32(numPlayers);
    wroteParam = wroteParam && sessionInfo->serialize(*buffer);

    if (wroteParam)
    {
        const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, buffer);
        if (startTaskResult != BD_NO_ERROR)
        {
            bdLogWarn("bdMatchMaking", "Failed to start task: %u", startTaskResult);
        }
    }
    else
    {
        bdLogWarn("bdMatchMaking", "Failed to serialize task parameters");
    }

    return task;
}


bdRemoteTaskRef bdMatchMaking::deleteSession(const bdSessionID & sessionID)
{
    bdRemoteTaskRef task(NULL);
    const bdUInt numParameters = 1;
    const bdUInt taskSize = 72 + numParameters + sizeof(bdSessionID);
    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(taskSize, true);
    bdRemoteTaskManager::initTaskBuffer(buffer, 21, 3);

    const bdBool wroteParam = sessionID.serialize(*buffer);

    if (wroteParam)
    {
        const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, buffer);
        if (startTaskResult != BD_NO_ERROR)
        {
            bdLogWarn("bdMatchMaking", "Failed to start task: %u", startTaskResult);
        }
    }
    else
    {
        bdLogWarn("bdMatchMaking", "Failed to serialize task parameters");
    }

    return task;
}




































































bdRemoteTaskRef bdMatchMaking::findSessions(const bdUInt queryID,
                                            const bdUInt startIndexDEPRECATED,
                                            const bdUInt maxNumResults,
                                            bdSessionParams * sessionParams,
                                            bdMatchMakingInfo * results)
{
    bdRemoteTaskRef task(NULL);
    const bdUInt taskSize = 72 + 4
                            + sizeof(queryID)
                            + sizeof(startIndexDEPRECATED)
                            + sizeof(maxNumResults)
                            + sessionParams->sizeOf();
    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(taskSize, true);
    bdRemoteTaskManager::initTaskBuffer(buffer, 21, 5);

    bdBool wroteParam = buffer->writeUInt32(queryID);
    wroteParam = wroteParam && buffer->writeUInt32(startIndexDEPRECATED);
    wroteParam = wroteParam && buffer->writeUInt32(maxNumResults);
    sessionParams->serialize(*buffer);

    if (wroteParam)
    {
        const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, buffer);
        if (startTaskResult != BD_NO_ERROR)
        {
            bdLogWarn("bdMatchMaking", "Failed to start task: %u", startTaskResult);
        }
        else
        {
            task->setTaskResult(results, maxNumResults);
        }
    }
    else
    {
        bdLogWarn("bdMatchMaking", "Failed to serialize task parameters");
    }

    return task;
}
