// From the DWARF (tools/gen_headers.py output for bdCODO.h); hand-maintained.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdCODO/bdCODO.h
#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../bdLobbyCommon/bdUserAccountID.h"

class bdCODOAutoJoinLevelInfo;
class bdCODOMinimumApplicationLevelInfo;
class bdCODOResettableStatsRequest;
class bdMessagingGroup;
class bdProfileItem;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

enum bdCODOLoudSpeakerType {
    BIG_SPEAKER = 0,
    SMALL_SPEAKER = 1,
};

class bdCODO {  // size 0x10
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdCODO);
    virtual ~bdCODO();  // vtable slot 0
    bdRemoteTaskRef setTeamAutoJoinLevel(const bdTeamID, const bdUInt32);
    bdRemoteTaskRef getTeamAutoJoinLevel(const bdTeamID, bdCODOAutoJoinLevelInfo *const);
    bdRemoteTaskRef autoJoinTeam(const bdTeamID, const bdUserID);
    bdRemoteTaskRef setTeamMinimumApplicationLevel(const bdTeamID, const bdUInt32);
    bdRemoteTaskRef getTeamMinimumApplicationLevel(const bdTeamID, bdCODOMinimumApplicationLevelInfo *const);
    bdRemoteTaskRef addApplication(const bdTeamID, const bdTeamID, const bdUserID, const void *const, const bdUInt);
    bdRemoteTaskRef sendLoudSpeakerMessage(const bdNChar8 *, const bdUInt32, const bdMessagingGroup &, const void *const, const bdUInt);
    bdRemoteTaskRef setTeamPublicProfilePartial(const bdTeamID, bdProfileItem *, const bdUInt32, bdUInt32);
    bdRemoteTaskRef setTeamActiveTimestamp(const bdTeamID);
    bdRemoteTaskRef resetStats(bdCODOResettableStatsRequest *);
protected:
    bdCODO(bdRemoteTaskManager *const);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x08
    static const bdUInt32 MAX_PARTIAL_PROFILE_UPDATES = 20;
};
static_assert(sizeof(bdCODO) == 0x10, "bdCODO");
