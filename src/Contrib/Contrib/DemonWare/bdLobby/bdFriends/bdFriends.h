// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdFriends/bdFriends.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdFriendAndRichPresence;
class bdFriendAssociation;
class bdFriendInfo;
class bdFriendProposal;
class bdGroupInfo;
class bdRemoteTaskManager;
class bdRichPresence;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdFriends {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdFriends);
    ~bdFriends();
    bdRemoteTaskRef proposeFriendship(const bdOnlineUserID, const void *const, const bdUInt);
    bdRemoteTaskRef proposeTypeFriendship(const bdOnlineUserID, bdInt32, const void *const, const bdUInt);
    bdRemoteTaskRef proposeFriendships(bdFriendProposal *, const bdUInt, bdInt32);
    bdRemoteTaskRef withdrawProposal(const bdOnlineUserID);
    bdRemoteTaskRef withdrawProposals(bdOnlineUserID *, const bdUInt);
    bdRemoteTaskRef getOutgoingProposals(bdFriendProposal *, const bdUInt, bdInt32, const bdUInt);
    bdRemoteTaskRef getIncomingProposals(bdFriendProposal *, const bdUInt, bdInt32, const bdUInt);
    bdRemoteTaskRef acceptProposal(const bdOnlineUserID);
    bdRemoteTaskRef acceptProposals(bdOnlineUserID *, const bdUInt);
    bdRemoteTaskRef rejectProposal(const bdOnlineUserID);
    bdRemoteTaskRef rejectProposals(bdOnlineUserID *, const bdUInt);
    bdRemoteTaskRef removeFriend(const bdOnlineUserID);
    bdRemoteTaskRef removeFriends(bdOnlineUserID *, const bdUInt);
    bdRemoteTaskRef removeAllFriends();
    bdRemoteTaskRef getFriends(bdFriendInfo *, const bdUInt, bdInt32, const bdUInt);
    bdRemoteTaskRef getFriendAssociations(const bdUInt32, const bdOnlineUserID *, const bdUInt, bdFriendAssociation *);
    bdRemoteTaskRef setFriendGroup(const bdOnlineUserID, const bdUByte8);
    bdRemoteTaskRef setGroupName(bdGroupInfo *);
    bdRemoteTaskRef getGroupNames(bdGroupInfo *, const bdUInt, const bdUInt);
    bdRemoteTaskRef deleteGroupName(const bdUByte8);
    bdRemoteTaskRef sendInstantMessage(const bdOnlineUserID, const void *const, const bdUInt);
    bdRemoteTaskRef sendInstantMessages(bdOnlineUserID *, const bdUInt, const void *const, const bdUInt);
    bdRemoteTaskRef setRichPresence(const void *const, const bdUInt);
    bdRemoteTaskRef getRichPresence(const bdOnlineUserID, bdRichPresence *);
    bdRemoteTaskRef setFriendType(bdOnlineUserID, bdInt32);
    bdRemoteTaskRef getFriendsAndRichPresence(bdFriendAndRichPresence *, const bdUInt, bdInt32, const bdUInt);
    bdRemoteTaskRef addFriendsToGroup(const bdUInt32, const bdOnlineUserID *, const bdUByte8);
protected:
    bdFriends(bdRemoteTaskManager *const);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdFriends) == 0x8, "bdFriends");


// Functions defined in this file (bodies are not in the DWARF):
//   line    78  void bdFriends::operator delete(void * p)  [inlined into 1 functions]
//   line    78  void * bdFriends::operator new(size_t nbytes)  [inline]
