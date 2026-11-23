// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMatchScheduler/bdMatchScheduler.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdMatchEventInfo;
class bdRemoteTaskManager;
class bdScheduledMatchInfo;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdMatchScheduler {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdMatchScheduler);
    bdMatchScheduler(bdRemoteTaskManager *const);
    ~bdMatchScheduler();
    bdRemoteTaskRef listScheduledMatches(bdOnlineUserID, bdScheduledMatchInfo *, bdUInt32);
    bdRemoteTaskRef getScheduledMatch(bdUInt64, bdScheduledMatchInfo *);
    bdRemoteTaskRef recordScheduledMatchEvent(bdUInt64, bdUByte8 *, bdUInt);
    bdRemoteTaskRef listMatchEvents(bdUInt64, bdMatchEventInfo *, bdUInt32);
    bdRemoteTaskRef incrementRoundCount(bdUInt64);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdMatchScheduler) == 0x8, "bdMatchScheduler");


// Functions defined in this file (bodies are not in the DWARF):
//   line    29  void bdMatchScheduler::operator delete(void * p)  [inlined into 1 functions]
//   line    29  void * bdMatchScheduler::operator new(size_t nbytes)  [inline]
