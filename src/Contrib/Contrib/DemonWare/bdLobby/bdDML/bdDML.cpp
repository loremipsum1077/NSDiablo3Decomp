// Original file: Contrib/Contrib/DemonWare/bdLobby/bdDML/bdDML.cpp
// Diagnostic text is inferred; this release retains only variadic arguments.
#include "bdDML.h"
#include "bdDMLInfo.h"
#include "../bdLobbyCommon/bdTaskParams.h"
#include "../bdRemoteTaskManager/bdRemoteTaskManager.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"














bdDML::bdDML(bdRemoteTaskManager *const remoteTaskManager)
    : m_remoteTaskManager(remoteTaskManager)
{
}

bdDML::~bdDML()
{
}























bdRemoteTaskRef bdDML::getUserData(bdDMLInfo * info)
{
    bdRemoteTaskRef task;


    bdTaskParams params(27, 2, 1024, 0x1fffff);




    params.bindResults(info, 1);



    const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, params);

    if (startTaskResult != BD_NO_ERROR) {

        bdLogWarn("bdDML", "Failed to start task");
    }

    return task;
}
