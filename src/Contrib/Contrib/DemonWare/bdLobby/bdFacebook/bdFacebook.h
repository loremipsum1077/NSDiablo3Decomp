// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdFacebook/bdFacebook.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdBoolResult;
class bdFacebookAccount;
class bdFacebookAttachmentProperty;
class bdFacebookFriend;
class bdFacebookProfilePicture;
class bdFacebookTag;
class bdFacebookUserAttribute;
class bdRemoteTaskManager;
class bdStreamingTask;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

typedef bdReference<bdStreamingTask> bdStreamingTaskRef;

enum bdFacebookFriendSort {
    BD_FBSORT_UNSORTED = 0,
    BD_FBSORT_ACCOUNT_NAME_ASC = 1,
    BD_FBSORT_ACCOUNT_NAME_DESC = 2,
};

class bdFacebook {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdFacebook);
    bdFacebook(bdRemoteTaskManager *const);
    ~bdFacebook();
    bdRemoteTaskRef registerAccount(const bdNChar8 *const, const bdNChar8 *const, bdFacebookAccount *const);
    bdRemoteTaskRef registerAccount(const bdNChar8 *const, const bdNChar8 *const, const bdUByte8 *const, const bdUInt32, bdFacebookAccount *const);
    bdRemoteTaskRef registerToken(const bdNChar8 *const, const bdUInt32, const bdUByte8 *const, const bdUInt32, bdFacebookAccount *const);
    bdRemoteTaskRef post(const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const, const bdUInt32, const bdFacebookAttachmentProperty *const, const bdNChar8 *const, const bdNChar8 *const);
    bdRemoteTaskRef post(const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const);
    bdRemoteTaskRef post(const bdUInt64, const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const, const bdUInt32, const bdFacebookAttachmentProperty *const, const bdNChar8 *const, const bdNChar8 *const);
    bdRemoteTaskRef unregisterAccount();
    bdRemoteTaskRef unregisterAccount(const bdUInt64 facebookAccountID);
    bdRemoteTaskRef uploadPhoto(const bdUInt64, const bdNChar8 *const, const bdUInt32, const bdFacebookTag *const);
    bdRemoteTaskRef uploadVideo(const bdUInt64, const bdNChar8 *const);
    bdRemoteTaskRef isRegistered(bdBoolResult *const);
    bdRemoteTaskRef getInfo(const bdUInt32, const bdUInt32, bdFacebookUserAttribute *const);
    bdRemoteTaskRef getInfo(const bdUInt64, const bdUInt32, const bdUInt32, bdFacebookUserAttribute *const);
    bdRemoteTaskRef getRegisteredAccounts(const bdUInt32, bdFacebookAccount *const);
    bdRemoteTaskRef getFriends(const bdBool, const bdUInt32, const bdUInt32, bdFacebookFriend *const, bdFacebookFriendSort);
    bdRemoteTaskRef getFriendsByID(const bdOnlineUserID *, bdUInt16, bdFacebookFriend *);
    bdStreamingTaskRef getProfilePictures(const bdUInt64 *, const bdUInt16, bdFacebookProfilePicture *const);
    bdStreamingTaskRef setLikeStatus(const bdNChar8 *const, const bdBool);
    bdRemoteTaskRef compareFacebookTokenToCurrentUsersAccount(const bdNChar8 *const, const bdUInt32, bdBoolResult *const);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdFacebook) == 0x8, "bdFacebook");


// Functions defined in this file (bodies are not in the DWARF):
//   line    70  void bdFacebook::operator delete(void * p)  [inlined into 1 functions]
//   line    70  void * bdFacebook::operator new(size_t nbytes)  [inline]
