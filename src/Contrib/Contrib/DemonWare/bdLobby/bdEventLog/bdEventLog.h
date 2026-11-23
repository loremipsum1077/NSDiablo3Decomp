// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdEventLog/bdEventLog.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdEventInfo;
class bdEventLogID;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdEventLog {  // size 0x218
public:
    BD_CLASS_ALLOCATOR(bdEventLog);
    bdEventLog(bdRemoteTaskManager *const, bdBool);
    ~bdEventLog();
    bdRemoteTaskRef recordEventsMixed(const bdEventInfo *, bdUInt, bdEventLogID *);
    bdRemoteTaskRef initializeFiltering();
    bdBool readyToSend();
    bdUInt64 getNumFiltered();
    bdBool isFiltered(bdUInt category);
protected:
    friend class bdLobbyService;
    void setReceivedCategoryData(bdBool received);
    void resetFilteredCategories();
    bdBool addFilteredCategory(bdUInt32 category);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
    bdBool m_receivedCategoryData;  // 0x08
    bdUInt m_numFilteredCategories;  // 0x0c
    bdUInt m_filteredCategories[128];  // 0x10
    bdUInt64 m_numFilteredLogs;  // 0x210
};
static_assert(sizeof(bdEventLog) == 0x218, "bdEventLog");
