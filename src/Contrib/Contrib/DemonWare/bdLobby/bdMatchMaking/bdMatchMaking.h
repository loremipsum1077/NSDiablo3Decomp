// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMatchMaking/bdMatchMaking.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdMatchMakingInfo;
class bdPagingToken;
class bdPerformanceValue;
class bdRemoteTaskManager;
class bdSessionID;
class bdSessionInvite;
class bdSessionParams;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdMatchMaking {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdMatchMaking);
    ~bdMatchMaking();
    bdRemoteTaskRef requestSessionID(bdSessionID *const);
    bdRemoteTaskRef initializeSession(const bdSessionID &, bdMatchMakingInfo *const);
    bdRemoteTaskRef createSession(bdMatchMakingInfo * sessionInfo, bdSessionID * sessionCredentials);
    bdRemoteTaskRef updateSession(const bdSessionID & sessionID, bdMatchMakingInfo *const sessionInfo);
    bdRemoteTaskRef updateSessionPlayers(const bdSessionID & sessionID, bdMatchMakingInfo *const sessionInfo, bdUInt32 numPlayers);
    bdRemoteTaskRef deleteSession(const bdSessionID & sessionID);
    bdRemoteTaskRef findSessionFromID(const bdSessionID &, bdMatchMakingInfo *const);
    bdRemoteTaskRef findSessionsFromIDs(const bdSessionID *const, const bdUInt, bdMatchMakingInfo *);
    bdRemoteTaskRef findSessions(const bdUInt queryID, const bdUInt startIndexDEPRECATED, const bdUInt maxNumResults, bdSessionParams * sessionParams, bdMatchMakingInfo * results);
    bdRemoteTaskRef findSessionsPaged(const bdUInt, bdSessionParams *, bdPagingToken *, bdMatchMakingInfo *);
    bdRemoteTaskRef findSessionsByEntityIDs(const bdEntityID *const, const bdUInt, bdMatchMakingInfo *, const bdUInt);
    bdRemoteTaskRef findSessionsTwoPass(const bdUInt, const bdUInt, const bdUInt, bdSessionParams *, bdMatchMakingInfo *);
    bdRemoteTaskRef inviteToSession(const bdSessionID &, const bdOnlineUserID *, const bdUInt, const void *const, const bdUInt);
    bdRemoteTaskRef notifyJoin(const bdSessionID &, const bdOnlineUserID *const, const bdUInt);
    bdRemoteTaskRef notifyLeave(const bdSessionID &, const bdOnlineUserID *const, const bdUInt);
    bdRemoteTaskRef submitPerformance(const bdUInt, bdPerformanceValue *, const bdUInt);
    bdRemoteTaskRef submitPerformance(const bdUInt, bdPerformanceValue **, const bdUInt);
    bdRemoteTaskRef getPerformanceValues(const bdEntityID *const, const bdUInt, const bdUInt, bdPerformanceValue *);
    bdRemoteTaskRef getSessionInvites(const bdUInt32, const bdUInt32, bdSessionInvite *);
protected:
    friend class bdLobbyService;
    bdMatchMaking(bdRemoteTaskManager *const, const bdNChar8 *);
    bdRemoteTaskRef startNotifyTask(const bdSessionID &, const bdOnlineUserID *, const bdUInt, const bdUByte8);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
    bdNChar8 m_context[16];  // 0x08
};
static_assert(sizeof(bdMatchMaking) == 0x18, "bdMatchMaking");

