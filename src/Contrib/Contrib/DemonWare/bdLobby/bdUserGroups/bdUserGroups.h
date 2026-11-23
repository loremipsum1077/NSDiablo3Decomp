// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdUserGroups/bdUserGroups.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdRemoteTaskManager;
class bdStatsInfo;
class bdUserGroupInfo;
class bdUserGroupList;
class bdUserGroupMembershipInfo;
class bdUserGroupNumMembers;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

typedef bdUInt64 bdUserGroupID;

class bdUserGroups {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdUserGroups);
    ~bdUserGroups();
    bdRemoteTaskRef createGroup(const bdNChar8 *const, const bdOnlineUserID, const bdInt16, bdUserGroupInfo *const);
    bdRemoteTaskRef deleteGroup(const bdUserGroupID);
    bdRemoteTaskRef joinGroup(const bdUserGroupID, const bdOnlineUserID, const bdInt16);
    bdRemoteTaskRef leaveGroup(const bdUserGroupID, const bdOnlineUserID);
    bdRemoteTaskRef getMembershipInfo(const bdUserGroupID, const bdOnlineUserID, bdUserGroupMembershipInfo *const);
    bdRemoteTaskRef changeMemberType(const bdUserGroupID, const bdOnlineUserID, const bdInt16);
    bdRemoteTaskRef getNumMembers(const bdUserGroupID, const bdInt16, bdUserGroupNumMembers *);
    bdRemoteTaskRef getMembers(const bdUserGroupID, const bdInt16, const bdUInt64, const bdUInt16, bdUserGroupMembershipInfo *);
    bdRemoteTaskRef getMemberships(const bdOnlineUserID *, const bdUInt, bdUserGroupMembershipInfo *, const bdUInt);
    bdRemoteTaskRef getGroupLists(const bdOnlineUserID *, const bdUInt, const bdUInt64, bdUserGroupList *);
    bdRemoteTaskRef readStatsByRank(const bdUserGroupID, const bdLeaderBoardID, const bdUInt64, bdStatsInfo *, const bdUInt32);
protected:
    bdUserGroups(bdRemoteTaskManager *const);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdUserGroups) == 0x8, "bdUserGroups");


// Functions defined in this file (bodies are not in the DWARF):
//   line    60  void bdUserGroups::operator delete(void * p)  [inlined into 1 functions]
//   line    60  void * bdUserGroups::operator new(size_t nbytes)  [inline]
