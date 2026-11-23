// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdSubscription/bdSubscription.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdRemoteTaskManager;
class bdSubscriptionInfo;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

enum bdSubscriptionFilter {
    BD_SUBSCRIPTION_FILTER_ALL = 0,
    BD_SUBSCRIPTION_FILTER_VALID = 1,
    BD_SUBSCRIPTION_FILTER_EXPIRED = 2,
};

class bdSubscription {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdSubscription);
    bdSubscription(bdRemoteTaskManager *const);
    ~bdSubscription();
    bdRemoteTaskRef getSubscriptionForUsers(const bdEntityID *, bdUInt32, bdUInt32, bdSubscriptionInfo *, bdSubscriptionFilter);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdSubscription) == 0x8, "bdSubscription");


// Functions defined in this file (bodies are not in the DWARF):
//   line    38  void bdSubscription::operator delete(void * p)  [inlined into 1 functions]
//   line    38  void * bdSubscription::operator new(size_t nbytes)  [inline]
