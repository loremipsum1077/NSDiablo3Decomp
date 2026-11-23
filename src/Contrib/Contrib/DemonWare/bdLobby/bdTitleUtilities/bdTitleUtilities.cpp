// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTitleUtilities/bdTitleUtilities.cpp
// Recovered from original instructions, DWARF and existing authenticated helpers.
#include "bdTitleUtilities.h"
#include "bdTimeStamp.h"
#include "bdUserInfo.h"
#include "../bdRemoteTaskManager/bdRemoteTaskManager.h"
#include "../bdRemoteTaskManager/bdTaskByteBuffer.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"













bdTitleUtilities::bdTitleUtilities(bdRemoteTaskManager *const remoteTaskManager)
    : m_remoteTaskManager(remoteTaskManager)
{
}

bdTitleUtilities::~bdTitleUtilities()
{
}
























































































bdRemoteTaskRef bdTitleUtilities::getServerTime(bdTimeStamp *timeStamp)
{
    bdRemoteTaskRef remoteTask(NULL);
    const bdUInt taskSize = 72;
    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(taskSize, true);
    bdRemoteTaskManager::initTaskBuffer(buffer, 12, 6);

    const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(remoteTask, buffer);

    if (startTaskResult != BD_NO_ERROR)
    {
        bdUseVAArgs(startTaskResult);
    }
    else
    {
        remoteTask->setTaskResult(timeStamp, 1);
    }

    return remoteTask;
}







































































































bdRemoteTaskRef bdTitleUtilities::getUserNames(bdOnlineUserID *userIDs,
                                             const bdUInt numUsers,
                                             bdUserInfo *userInfos)
{
    bdUInt taskSize = 72;
    taskSize += numUsers * 9;
    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(taskSize, true);
    bdRemoteTaskRef remoteTask(NULL);
    bdRemoteTaskManager::initTaskBuffer(buffer, 12, 9);

    bdBool wroteParam = true;


    for (bdUInt i = 0; i < numUsers; ++i)
    {
        wroteParam = wroteParam && buffer->writeUInt64(userIDs[i]);
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
            remoteTask->setTaskResult(userInfos, numUsers);
        }
    }
    else
    {
        bdUseVAArgs();
    }

    return remoteTask;
}
