// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdServerInventory/bdServerInventory.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdContainers/bdArray.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdService.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdAllocateDedicatedServerResponse;
class bdBoolResult;
class bdDataCenterPreferences;
class bdRemoteTaskManager;
class bdSecurityID;
class bdSecurityKey;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdServerInventory : public bdService {  // size 0x10
public:
    BD_CLASS_ALLOCATOR(bdServerInventory);
    bdRemoteTaskRef registerDedicatedServer(const bdNChar8 *const, const bdNChar8 *const, const bdUByte8 *, bdSecurityID, bdSecurityKey, const bdUInt32, const bdNChar8 *const);
    bdRemoteTaskRef registerDedicatedServerInContext(const bdNChar8 *const, const bdNChar8 *const, const bdUByte8 *, bdSecurityID, bdSecurityKey, const bdUInt32, const bdNChar8 *const, const bdNChar8 *const);
    bdRemoteTaskRef registerDedicatedServer(const bdNChar8 *const, const bdNChar8 *const, const bdUInt32, const bdNChar8 *const);
    bdRemoteTaskRef deregisterDedicatedServer();
    bdRemoteTaskRef healthCheck(bdBoolResult *const);
    bdRemoteTaskRef allocateDedicatedServer(const bdNChar8 *, const bdNChar8 **, bdUInt, bdAllocateDedicatedServerResponse *, const bdNChar8 *);
    bdRemoteTaskRef allocateDedicatedServer(const bdNChar8 *, bdArray<bdString>, bdAllocateDedicatedServerResponse *, const bdNChar8 *);
    bdRemoteTaskRef allocateDedicatedServer(const bdNChar8 *, const bdDataCenterPreferences *, bdAllocateDedicatedServerResponse *, const bdNChar8 *);
protected:
    friend class bdLobbyService;  // not in the DWARF: bdLobbyService::cleanup deletes this through the protected destructor
    virtual ~bdServerInventory();  // vtable slot 0
    bdServerInventory(bdRemoteTaskManager *const);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x08
};
static_assert(sizeof(bdServerInventory) == 0x10, "bdServerInventory");


// Functions defined in this file (bodies are not in the DWARF):
//   line    45  void bdServerInventory::operator delete(void * p)  [inline]
//   line    45  void * bdServerInventory::operator new(size_t nbytes)  [inline]
