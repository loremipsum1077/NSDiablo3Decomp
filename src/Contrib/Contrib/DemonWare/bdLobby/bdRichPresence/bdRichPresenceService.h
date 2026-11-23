// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdRichPresence/bdRichPresenceService.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdRemoteTaskManager;
class bdRichPresenceData;
class bdRichPresenceInfo;
class bdTaskParams;
class bdUserAccountID;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdRichPresenceService {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdRichPresenceService);
    bdRichPresenceService(bdRemoteTaskManager *const, const bdNChar8 *);
    ~bdRichPresenceService();
    bdRemoteTaskRef setRichPresence(const bdRichPresenceData & richPresenceData);
    bdRemoteTaskRef getRichPresence(const bdUserAccountID *, bdRichPresenceData *, const bdUInt);
    bdRemoteTaskRef getRichPresenceAndSubscribe(const bdUserAccountID * userAccountIDs, bdRichPresenceData * results, const bdUInt maxNumResults);
    bdRemoteTaskRef subscribeToUsers(const bdUserAccountID *, const bdUInt);
    bdRemoteTaskRef unsubscribeFromUsers(const bdUserAccountID * userAccountIDs, bdUInt numUsers);
    bdRemoteTaskRef unsubscribeAll();
    bdRemoteTaskRef getSubscriptions(bdUserAccountID *, const bdUInt, const bdUInt);
    bdRemoteTaskRef getSubscribers(bdUserAccountID *, const bdUInt, const bdUInt);
    bdRemoteTaskRef blockUsers(const bdUserAccountID *, const bdUInt);
    bdRemoteTaskRef unblockUsers(const bdUserAccountID *, const bdUInt);
    bdRemoteTaskRef getBlockedUsers(bdUserAccountID *, const bdUInt, const bdUInt);
    bdRemoteTaskRef setInfo(bdRichPresenceInfo *const);
    bdRemoteTaskRef getInfo(bdOnlineUserID *, bdRichPresenceInfo *, bdUInt);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
    bdNChar8 m_context[16];  // 0x08
    void startTask(bdRemoteTaskRef & remoteTask, bdTaskParams & params);
    void addUserAccountIDArray(bdTaskParams & params, const bdUserAccountID * userAccountIDs, const bdUInt32 numUsers);
};
static_assert(sizeof(bdRichPresenceService) == 0x18, "bdRichPresenceService");

