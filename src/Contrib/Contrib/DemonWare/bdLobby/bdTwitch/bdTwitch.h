// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTwitch/bdTwitch.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdBoolResult;
class bdRemoteTaskManager;
class bdTwitchUserInfo;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdTwitch {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdTwitch);
    bdTwitch(bdRemoteTaskManager *const);
    ~bdTwitch();
    bdRemoteTaskRef linkAccount(const bdNChar8 *);
    bdRemoteTaskRef unlinkAccount();
    bdRemoteTaskRef isLinked(bdBoolResult *);
    bdRemoteTaskRef getUserInfo(bdTwitchUserInfo *);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdTwitch) == 0x8, "bdTwitch");


// Functions defined in this file (bodies are not in the DWARF):
//   line    34  void bdTwitch::operator delete(void * p)  [inlined into 1 functions]
//   line    34  void * bdTwitch::operator new(size_t nbytes)  [inline]
