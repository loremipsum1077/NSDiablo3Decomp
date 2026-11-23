// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdFeatureBan/bdFeatureBan.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdFeatureBanInfo;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdFeatureBan {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdFeatureBan);
    bdFeatureBan(bdRemoteTaskManager *const);
    ~bdFeatureBan();
    bdRemoteTaskRef getFeatureBans(bdEntityID, bdFeatureBanInfo *, bdUInt32, bdUInt32);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdFeatureBan) == 0x8, "bdFeatureBan");


// Functions defined in this file (bodies are not in the DWARF):
//   line    45  void bdFeatureBan::operator delete(void * p)  [inlined into 1 functions]
//   line    45  void * bdFeatureBan::operator new(size_t nbytes)  [inline]
