// Declarations recovered from original DWARF; original line layout retained.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTitleUtilities/bdTitleUtilities.h

#pragma once

#include "../bdLobbyCommon/bdService.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"

class bdRemoteTaskManager;
class bdTimeStamp;
class bdUserInfo;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;
































class bdTitleUtilities : public bdService {  // size 0x10
public:

    static void operator delete(void *p) { bdMemory::deallocate(p); } static void* operator new(size_t nbytes) { return bdMemory::allocate(nbytes); }


    bdTitleUtilities(bdRemoteTaskManager *const remoteTaskManager);


    virtual ~bdTitleUtilities();



















































    bdRemoteTaskRef getServerTime(bdTimeStamp *timeStamp);










































    bdRemoteTaskRef getUserNames(bdOnlineUserID *userIDs, const bdUInt numUsers, bdUserInfo *userInfos);












































protected:
    bdRemoteTaskManager *m_remoteTaskManager;  // 0x08
};
static_assert(sizeof(bdTitleUtilities) == 0x10, "bdTitleUtilities");
