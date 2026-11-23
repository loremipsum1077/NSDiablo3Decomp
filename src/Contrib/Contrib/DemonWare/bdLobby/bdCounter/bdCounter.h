// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdCounter/bdCounter.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdCounterValue;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdCounter {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdCounter);
    bdCounter(bdRemoteTaskManager *const);
    ~bdCounter();
    bdRemoteTaskRef incrementCounters(bdCounterValue *const counterIncrements, const bdUInt numEntries);
    bdRemoteTaskRef getCounterTotals(bdCounterValue *, const bdUInt);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdCounter) == 0x8, "bdCounter");
