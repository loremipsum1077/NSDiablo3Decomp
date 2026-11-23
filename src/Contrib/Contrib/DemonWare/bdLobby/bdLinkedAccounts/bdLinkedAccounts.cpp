// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLinkedAccounts/bdLinkedAccounts.cpp
// Release logging retains only the variadic argument calls shown below.
#include "bdLinkedAccounts.h"
#include "bdLinkedAccountsResult.h"
#include "../bdLobbyCommon/bdUserAccountID.h"
#include "../bdRemoteTaskManager/bdRemoteTaskManager.h"
#include "../bdRemoteTaskManager/bdTaskByteBuffer.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"














bdLinkedAccounts::bdLinkedAccounts(bdRemoteTaskManager *const remoteTaskManager)
    : m_remoteTaskManager(remoteTaskManager)
{
}

bdLinkedAccounts::~bdLinkedAccounts()
{

}

bdRemoteTaskRef bdLinkedAccounts::getLinkedAccounts(const bdUserAccountID * userAccountIDs,
                                                    bdLinkedAccountsResult * results,
                                                    bdUInt32 count)
{



    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(0, true);
    buffer->write(NULL, 72);
    buffer->writeUInt32(count);
    for (bdUInt32 i = 0; i < count; ++i)
    {
        ContextSerialization::writeUserID(buffer, userAccountIDs[i]);
    }


    buffer->allocateBuffer();

    bdRemoteTaskRef remoteTask(NULL);
    bdRemoteTaskManager::initTaskBuffer(buffer, 86, 3);

    bdBool wroteParam = buffer->writeUInt32(count);
    for (bdUInt32 i = 0; i < count && wroteParam; ++i)
    {
        wroteParam = ContextSerialization::writeUserID(buffer, userAccountIDs[i]);
    }

    if (wroteParam)
    {
        const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(remoteTask, buffer);
        if (startTaskResult != BD_NO_ERROR)
        {
            bdUseVAArgs(startTaskResult);
        }
        else
        {
            remoteTask->setTaskResult(results, count);
        }
    }
    else
    {
        bdUseVAArgs();
    }

    return remoteTask;
}
