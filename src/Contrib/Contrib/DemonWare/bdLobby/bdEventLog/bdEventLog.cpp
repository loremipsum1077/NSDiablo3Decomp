// Original file: Contrib/Contrib/DemonWare/bdLobby/bdEventLog/bdEventLog.cpp
// Diagnostic text is inferred; this release retains only variadic arguments.
// recordEventsMixed, readyToSend, getNumFiltered and isFiltered were stripped by the
// linker (low_pc 0 in the DWARF, no bytes to verify) and are not reconstructed here.
#include "bdEventLog.h"
#include "../bdLobbyCommon/bdTaskParams.h"
#include "../bdRemoteTaskManager/bdRemoteTaskManager.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"









bdEventLog::bdEventLog(bdRemoteTaskManager *const remoteTaskManager, bdBool isConnected)
    : m_remoteTaskManager(remoteTaskManager),
      m_receivedCategoryData(false),
      m_numFilteredCategories(0),
      m_numFilteredLogs(0)
{
    if (isConnected) {
        initializeFiltering();
    }
}

bdEventLog::~bdEventLog()
{
}



































































bdRemoteTaskRef bdEventLog::initializeFiltering()
{
    bdRemoteTaskRef remoteTask(NULL);

    m_numFilteredLogs = 0;
    resetFilteredCategories();
    bdTaskParams params(67, 6, 1024, 0x1fffff);

    const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(remoteTask, params);
    if (startTaskResult != BD_NO_ERROR)
    {
        bdLogWarn("bdEventLog", "Failed to start task: %u", startTaskResult);
    }

    return remoteTask;
}





void bdEventLog::setReceivedCategoryData(bdBool received)
{
    m_receivedCategoryData = received;
}






























void bdEventLog::resetFilteredCategories()
{
    m_receivedCategoryData = false;
    m_numFilteredCategories = 0;
}

bdBool bdEventLog::addFilteredCategory(bdUInt32 category)
{
    bdBool result = false;
    if (m_numFilteredCategories < 128)
    {
        m_filteredCategories[m_numFilteredCategories] = category;
        ++m_numFilteredCategories;
        result = true;
    }
    return result;
}
