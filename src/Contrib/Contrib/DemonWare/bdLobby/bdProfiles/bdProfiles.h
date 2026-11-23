// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdProfiles/bdProfiles.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdProfileInfo;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdProfiles {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdProfiles);
    ~bdProfiles();
    bdRemoteTaskRef setPublicInfo(bdProfileInfo *);
    bdRemoteTaskRef setPrivateInfo(bdProfileInfo *);
    bdRemoteTaskRef setPrivateInfoByUserID(bdUInt64, bdProfileInfo *const);
    bdRemoteTaskRef setPublicInfoByUserID(bdUInt64, bdProfileInfo *const);
    bdRemoteTaskRef getPrivateInfoByUserID(bdUInt64, bdProfileInfo *const);
    bdRemoteTaskRef getPublicInfos(const bdOnlineUserID *, bdProfileInfo *, const bdUInt);
    bdRemoteTaskRef getPrivateInfo(bdProfileInfo *const);
    bdRemoteTaskRef deleteProfile();
protected:
    bdProfiles(bdRemoteTaskManager *const);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdProfiles) == 0x8, "bdProfiles");


// Functions defined in this file (bodies are not in the DWARF):
//   line    42  void bdProfiles::operator delete(void * p)  [inlined into 1 functions]
//   line    42  void * bdProfiles::operator new(size_t nbytes)  [inline]
