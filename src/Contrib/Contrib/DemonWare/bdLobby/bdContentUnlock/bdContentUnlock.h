// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdContentUnlock/bdContentUnlock.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdContentStatus;
class bdNumContentUpdated;
class bdRemoteTaskManager;
class bdTransferrableUnlockedContent;
class bdUnlockContentInfo;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdContentUnlock {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdContentUnlock);
    ~bdContentUnlock();
    bdRemoteTaskRef unlockContent(const bdNChar8 *, bdUnlockContentInfo *, const bdUInt32, const bdUInt64);
    bdRemoteTaskRef unlockMultipleContentPieces(const bdNChar8 **, const bdUInt16, bdUnlockContentInfo *, const bdUInt32, const bdUInt64);
    bdRemoteTaskRef listUnlockedContentForUser(bdUnlockContentInfo *, bdUInt16);
    bdRemoteTaskRef listContentByLicenseCode(const bdNChar8 **, const bdUInt16, bdUnlockContentInfo *);
    bdRemoteTaskRef checkContentStatusByLicenseCodes(const bdNChar8 **, const bdUInt16, const bdUInt32, bdContentStatus *);
    bdRemoteTaskRef transferOwnershipOfContent(const bdUInt64, const bdOnlineUserID, const bdUInt32, bdNumContentUpdated *);
    bdRemoteTaskRef synchronizeUnlockedContent(const bdOnlineUserID, const bdOnlineUserID, const bdUInt32, const bdUInt32, const bdUInt64, const bdBool, bdTransferrableUnlockedContent *);
protected:
    bdContentUnlock(bdRemoteTaskManager *const);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdContentUnlock) == 0x8, "bdContentUnlock");


// Functions defined in this file (bodies are not in the DWARF):
//   line    45  void bdContentUnlock::operator delete(void * p)  [inlined into 1 functions]
//   line    45  void * bdContentUnlock::operator new(size_t nbytes)  [inline]
