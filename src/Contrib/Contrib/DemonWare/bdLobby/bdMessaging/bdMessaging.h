// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMessaging/bdMessaging.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdChannelInfo;
class bdChannelMember;
class bdMessagingGroup;
class bdMessagingGroupInfo;
class bdMsgGroupSubscriptionFailure;
class bdNotification;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdMessaging {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdMessaging);
    ~bdMessaging();
    bdRemoteTaskRef getNotifications(bdNotification *, const bdUInt, const bdUInt, const bdBool);
    bdRemoteTaskRef deleteNotifications(const bdUInt64 *, const bdUInt);
    bdRemoteTaskRef createChatChannel(bdChannelInfo *const);
    bdRemoteTaskRef createChatChannelWithID(bdChannelInfo *const, const bdUInt64);
    bdRemoteTaskRef subscribeToChannel(const bdUInt64);
    bdRemoteTaskRef subscribeToChannel(const bdUInt64, const bdUInt64);
    bdRemoteTaskRef unsubscribeFromChannel(const bdUInt64);
    bdRemoteTaskRef getChannelList(bdChannelInfo *, const bdUInt);
    bdRemoteTaskRef getChannelsInfo(const bdUInt, const bdUInt64 *, bdChannelInfo *);
    bdRemoteTaskRef promoteToChannelAdmin(const bdUInt64, const bdOnlineUserID);
    bdRemoteTaskRef kickFromChannel(const bdUInt64, const bdOnlineUserID, bdUInt32, bdBool);
    bdRemoteTaskRef setChannelPassword(const bdUInt64, const bdUInt64);
    bdRemoteTaskRef clearChannelPassword(const bdUInt64);
    bdRemoteTaskRef sendToChannel(const bdUInt64, const void *const, const bdUInt, const bdUInt32);
    bdRemoteTaskRef whisperToChannelMember(const bdUInt64, const bdOnlineUserID, const void *const, const bdUInt, const bdUInt32);
    bdRemoteTaskRef getChannelMembers(const bdUInt64, bdChannelMember *, const bdUInt);
    bdRemoteTaskRef sendGlobalInstantMessage(const bdOnlineUserID recipientUID, const void *const message, const bdUInt msgSize);
    bdRemoteTaskRef sendGlobalInstantMessages(const bdOnlineUserID *, bdUInt, const void *, bdUInt);
    bdRemoteTaskRef manageMsgGroupsSubscriptions(const bdMessagingGroup *, bdUInt, const bdMessagingGroup *, bdUInt, bdMsgGroupSubscriptionFailure *, const bdUInt);
    bdRemoteTaskRef unsubscribeMsgGroupsByCategory(const bdUByte8 *, bdUInt);
    bdRemoteTaskRef unsubscribeAllMsgGroups();
    bdRemoteTaskRef listMsgGroupSubscriptions(bdMessagingGroup *, const bdUInt);
    bdRemoteTaskRef queryMsgGroupInfo(const bdMessagingGroup *, const bdUInt, bdMessagingGroupInfo *);
    bdRemoteTaskRef postToMsgGroup(const bdMessagingGroup, const void *, bdUInt);
    bdRemoteTaskRef broadcastMessage(const void *, bdUInt);
protected:
    friend class bdLobbyService;
    bdMessaging(bdRemoteTaskManager *const);
    void setRemoteTaskManager(bdRemoteTaskManager *const);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdMessaging) == 0x8, "bdMessaging");


// Functions defined in this file (bodies are not in the DWARF):
//   line    96  void bdMessaging::operator delete(void * p)  [inlined into 1 functions]
//   line    96  void * bdMessaging::operator new(size_t nbytes)  [inlined into 1 functions]
