// Original file: Contrib/Contrib/DemonWare/bdLobby/bdCounter/bdCounter.cpp
// Diagnostic text is inferred; this release retains only variadic arguments.
// getCounterTotals was stripped by the linker (low_pc 0 in the DWARF, no bytes to
// verify) and is not reconstructed here.
#include "bdCounter.h"
#include "bdCounterValue.h"
#include "../bdRemoteTaskManager/bdRemoteTaskManager.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"








bdCounter::bdCounter(bdRemoteTaskManager *const remoteTaskManager)
    : m_remoteTaskManager(remoteTaskManager)
{
}

bdCounter::~bdCounter()
{
}

bdRemoteTaskRef bdCounter::incrementCounters(bdCounterValue *const counterIncrements,
                                             const bdUInt numEntries)
{
    const bdUInt numParameters = numEntries;
    bdUInt taskSize = 72 + numParameters;
    for (bdUInt i = 0; i < numEntries; ++i)
    {
        taskSize += counterIncrements[i].sizeOf();
    }

    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(taskSize, true);
    bdRemoteTaskRef remoteTask(NULL);
    bdRemoteTaskManager::initTaskBuffer(buffer, 23, 1);

    bdBool wroteParam = true;

    for (bdUInt i = 0; i < numEntries; ++i)
    {
        wroteParam = wroteParam && counterIncrements[i].serialize(*buffer);
    }

    if (wroteParam)
    {
        const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(remoteTask, buffer);
        if (startTaskResult != BD_NO_ERROR)
        {
            bdLogWarn("bdCounter", "Failed to start task: %u", startTaskResult);
        }
    }
    else
    {
        bdLogWarn("bdCounter", "Failed to serialize task parameters");
    }

    return remoteTask;
}
