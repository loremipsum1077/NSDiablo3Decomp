// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdUserBlockList/bdUserBlockList.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdBoolResult;
class bdRemoteTaskManager;
class bdUserBlockListID;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdUserBlockList {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdUserBlockList);
    ~bdUserBlockList();
    bdRemoteTaskRef blockUsers(const bdOnlineUserID *, const bdUInt32);
    bdRemoteTaskRef unblockUsers(const bdOnlineUserID *, const bdUInt32);
    bdRemoteTaskRef replaceBlockList(const bdOnlineUserID *, const bdUInt32);
    bdRemoteTaskRef getBlockList(bdUserBlockListID *, const bdUInt32, const bdUInt32);
    bdRemoteTaskRef checkBlockExists(const bdOnlineUserID *, const bdUInt32, const bdOnlineUserID *, const bdUInt32, bdBoolResult *const);
protected:
    bdUserBlockList(bdRemoteTaskManager *const);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdUserBlockList) == 0x8, "bdUserBlockList");


// Functions defined in this file (bodies are not in the DWARF):
//   line    39  void bdUserBlockList::operator delete(void * p)  [inlined into 1 functions]
//   line    39  void * bdUserBlockList::operator new(size_t nbytes)  [inline]
