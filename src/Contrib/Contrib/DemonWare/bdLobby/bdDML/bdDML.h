// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdDML/bdDML.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdDMLHierarchicalInfo;
class bdDMLInfo;
class bdDMLUserLogonInfo;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdDML {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdDML);
    bdDML(bdRemoteTaskManager *const);
    ~bdDML();
    bdRemoteTaskRef recordIP(const bdUInt32);
    bdRemoteTaskRef getUserData(bdDMLInfo * info);
    bdRemoteTaskRef getUserHierarchicalData(bdDMLHierarchicalInfo *);
    bdRemoteTaskRef getUsersLastLogonData(const bdOnlineUserID *, const bdUInt32, bdDMLUserLogonInfo *);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdDML) == 0x8, "bdDML");


// Functions defined in this file (bodies are not in the DWARF):
//   line    37  void bdDML::operator delete(void * p)  [inlined into 1 functions]
//   line    37  void * bdDML::operator new(size_t nbytes)  [inlined into 1 functions]
