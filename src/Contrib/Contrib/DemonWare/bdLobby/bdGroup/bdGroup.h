// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdGroup/bdGroup.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdGroupCount;
class bdGroupID;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdGroup {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdGroup);
    bdGroup(bdRemoteTaskManager *const);
    ~bdGroup();
    bdRemoteTaskRef setGroups(const bdUInt32 *const, const bdUInt);
    bdRemoteTaskRef setGroupsForEntity(const bdEntityID, const bdUInt32 *const, const bdUInt);
    bdRemoteTaskRef getEntityGroups(const bdEntityID, bdGroupID *, const bdUInt);
    bdRemoteTaskRef getGroupCounts(const bdUInt32 *const, const bdUInt, bdGroupCount *, const bdUInt);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdGroup) == 0x8, "bdGroup");


// Functions defined in this file (bodies are not in the DWARF):
//   line    33  void bdGroup::operator delete(void * p)  [inlined into 1 functions]
//   line    33  void * bdGroup::operator new(size_t nbytes)  [inline]
