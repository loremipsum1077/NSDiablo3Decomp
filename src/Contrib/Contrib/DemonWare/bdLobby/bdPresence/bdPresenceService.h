// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdPresence/bdPresenceService.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdPresenceInfo;
class bdRemoteTaskManager;
class bdUserAccountID;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdPresenceService {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdPresenceService);
    bdPresenceService(bdRemoteTaskManager *const);
    ~bdPresenceService();
    bdRemoteTaskRef setPresenceData(bdByte8 *const, bdUInt);
    bdRemoteTaskRef getPresenceData(bdUserAccountID *, bdPresenceInfo *, bdUInt);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdPresenceService) == 0x8, "bdPresenceService");


// Functions defined in this file (bodies are not in the DWARF):
//   line    31  void bdPresenceService::operator delete(void * p)  [inlined into 1 functions]
//   line    31  void * bdPresenceService::operator new(size_t nbytes)  [inline]
