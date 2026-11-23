// Original file: Contrib/Contrib/DemonWare/bdLobby/bdRichPresence/bdRichPresenceService.cpp
// Diagnostic text is inferred; this release retains only variadic arguments.
// The other task methods (getRichPresence, subscribeToUsers, unsubscribeAll, ...) were
// stripped by the linker (low_pc 0 in the DWARF, no bytes) and are not reconstructed.
#include "bdRichPresenceService.h"
#include "bdRichPresenceData.h"
#include "../bdLobbyCommon/bdTaskParams.h"
#include "../bdLobbyCommon/bdUserAccountID.h"
#include "../bdRemoteTaskManager/bdRemoteTaskManager.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"




bdRichPresenceService::bdRichPresenceService(bdRemoteTaskManager *const remoteTaskManager,
                                             const bdNChar8 * context)
    : m_remoteTaskManager(remoteTaskManager)
{
    bdStrlcpy(m_context, context, sizeof(m_context));
}

bdRichPresenceService::~bdRichPresenceService()
{
}

bdRemoteTaskRef bdRichPresenceService::setRichPresence(const bdRichPresenceData & richPresenceData)
{
    bdRemoteTaskRef remoteTask(NULL);
    bdTaskParams params(68, 3, 1024, 0x1fffff);
    params.addContext(m_context);

    params.addSerializable(richPresenceData);
    startTask(remoteTask, params);

    return remoteTask;
}
























bdRemoteTaskRef bdRichPresenceService::getRichPresenceAndSubscribe(const bdUserAccountID * userAccountIDs,
                                                                   bdRichPresenceData * results,
                                                                   const bdUInt maxNumResults)
{
    bdRemoteTaskRef remoteTask(NULL);




    if ((userAccountIDs != NULL) && (results != NULL) && (maxNumResults > 0))
    {
        bdTaskParams params(68, 5, 1024, 0x1fffff);
        params.addContext(m_context);
        params.addUInt32(maxNumResults);
        addUserAccountIDArray(params, userAccountIDs, maxNumResults);
        params.bindResults(results, maxNumResults);

        startTask(remoteTask, params);
    }

    return remoteTask;
}





















bdRemoteTaskRef bdRichPresenceService::unsubscribeFromUsers(const bdUserAccountID * userAccountIDs,
                                                            bdUInt numUsers)
{
    bdRemoteTaskRef remoteTask(NULL);



    if ((userAccountIDs != NULL) && (numUsers > 0))
    {
        bdTaskParams params(68, 7, 1024, 0x1fffff);
        params.addContext(m_context);
        params.addUInt32(numUsers);
        addUserAccountIDArray(params, userAccountIDs, numUsers);

        startTask(remoteTask, params);
    }

    return remoteTask;
}




























































































































void bdRichPresenceService::startTask(bdRemoteTaskRef & remoteTask, bdTaskParams & params)
{

    if (params.serializedOk())
    {
        const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(remoteTask, params);
        if (startTaskResult != BD_NO_ERROR)
        {
            bdLogWarn("bdRichPresenceService", "Failed to start task: %u", startTaskResult);
        }
    }
    else
    {
        bdLogWarn("bdRichPresenceService", "Failed to serialize task parameters");
    }
}

void bdRichPresenceService::addUserAccountIDArray(bdTaskParams & params,
                                                  const bdUserAccountID * userAccountIDs,
                                                  const bdUInt32 numUsers)
{

    for (bdUInt32 i = 0; i < numUsers; ++i)
    {
        params.addSerializable(userAccountIDs[i]);
    }
}
