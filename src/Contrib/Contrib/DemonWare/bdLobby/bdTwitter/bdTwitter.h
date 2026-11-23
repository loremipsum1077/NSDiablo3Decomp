// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTwitter/bdTwitter.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdBoolResult;
class bdRemoteTaskManager;
class bdTweet;
class bdTwitterAccount;
class bdTwitterAuthorizationResult;
class bdTwitterFriendsIdsResult;
class bdTwitterSearchResult;
class bdTwitterUserAttribute;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

enum bdTwitterResultType {
    BD_TWITTER_DEFAULT_RESULT = 0,
    BD_TWITTER_MIXED_RESULT = 1,
    BD_TWITTER_RECENT_RESULT = 2,
    BD_TWITTER_POPULAR_RESULT = 3,
};

class bdTwitter {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdTwitter);
    bdTwitter(bdRemoteTaskManager *const);
    ~bdTwitter();
    bdRemoteTaskRef searchAsApp(const bdNChar8 *, const bdUInt32, bdTwitterSearchResult *const, const bdTwitterResultType, const bdUInt64, const bdUInt64);
    bdRemoteTaskRef search(const bdNChar8 *, const bdUInt32, bdTwitterSearchResult *const, const bdTwitterResultType, const bdUInt64, const bdUInt64);
    bdRemoteTaskRef getInfoAsApp(const bdNChar8 *, const bdUInt32, const bdUInt32, bdTwitterUserAttribute *const);
    bdRemoteTaskRef getInfoAsApp(const bdUInt64, const bdUInt32, const bdUInt32, bdTwitterUserAttribute *const);
    bdRemoteTaskRef getInfo(const bdNChar8 *, const bdUInt32, const bdUInt32, bdTwitterUserAttribute *const);
    bdRemoteTaskRef getInfo(const bdUInt64, const bdUInt32, const bdUInt32, bdTwitterUserAttribute *const);
    bdRemoteTaskRef getInfo(const bdUInt32, const bdUInt32, bdTwitterUserAttribute *const);
    bdRemoteTaskRef getInfosAsApp(const bdUInt64 *, const bdUInt32, bdTwitterAccount *);
    bdRemoteTaskRef getInfosAsApp(const bdNChar8 **, const bdUInt32, bdTwitterAccount *);
    bdRemoteTaskRef getInfos(const bdUInt64 *, const bdUInt32, bdTwitterAccount *);
    bdRemoteTaskRef getInfos(const bdNChar8 **, const bdUInt32, bdTwitterAccount *);
    bdRemoteTaskRef getFriendsIdsAsApp(const bdNChar8 *, const bdUInt32, bdTwitterFriendsIdsResult *, const bdUInt64);
    bdRemoteTaskRef getFriendsIdsAsApp(const bdUInt64, const bdUInt32, bdTwitterFriendsIdsResult *, const bdUInt64);
    bdRemoteTaskRef getFriendsIds(const bdNChar8 *, const bdUInt32, bdTwitterFriendsIdsResult *, const bdUInt64);
    bdRemoteTaskRef getFriendsIds(const bdUInt64, const bdUInt32, bdTwitterFriendsIdsResult *, const bdUInt64);
    bdRemoteTaskRef getFriendsIds(const bdUInt32, bdTwitterFriendsIdsResult *, const bdUInt64);
    bdRemoteTaskRef getUserTimelineAsApp(const bdNChar8 *, bdTweet *const, const bdUInt32, const bdUInt64, const bdUInt64, const bdBool);
    bdRemoteTaskRef getUserTimelineAsApp(const bdUInt64, bdTweet *const, const bdUInt32, const bdUInt64, const bdUInt64, const bdBool);
    bdRemoteTaskRef getUserTimeline(const bdNChar8 *, bdTweet *const, const bdUInt32, const bdUInt64, const bdUInt64, const bdBool);
    bdRemoteTaskRef getUserTimeline(const bdUInt64, bdTweet *const, const bdUInt32, const bdUInt64, const bdUInt64, const bdBool);
    bdRemoteTaskRef getUserTimeline(bdTweet *const, const bdUInt32, const bdUInt64, const bdUInt64, const bdBool);
    bdRemoteTaskRef requestAuthorization(bdTwitterAuthorizationResult *const);
    bdRemoteTaskRef confirmAuthorization(const bdNChar8 *const, const bdNChar8 *const, bdTwitterAccount *const);
    bdRemoteTaskRef registerToken(const bdNChar8 *const, const bdNChar8 *const, bdTwitterAccount *const);
    bdRemoteTaskRef isRegistered(bdBoolResult *const);
    bdRemoteTaskRef post(const bdNChar8 *const);
    bdRemoteTaskRef follow(const bdNChar8 *);
    bdRemoteTaskRef follow(const bdUInt64);
    bdRemoteTaskRef unfollow(const bdNChar8 *);
    bdRemoteTaskRef unfollow(const bdUInt64);
    bdRemoteTaskRef unregisterAccount();
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
    bdRemoteTaskRef searchGeneral(const bdUByte8, const bdNChar8 *, const bdUInt32, bdTwitterSearchResult *const, const bdTwitterResultType, const bdUInt64, const bdUInt64);
    bdRemoteTaskRef getInfoGeneral(const bdUByte8, const bdNChar8 *, const bdUInt64, const bdUInt32, const bdUInt32, bdTwitterUserAttribute *const);
    bdRemoteTaskRef getInfosGeneral(const bdUByte8, const bdUInt64 *, const bdUInt32, const bdNChar8 **, const bdUInt32, bdTwitterAccount *);
    bdRemoteTaskRef getFriendsIdsGeneral(const bdUByte8, const bdNChar8 *, const bdUInt64, const bdUInt32, bdTwitterFriendsIdsResult *, const bdUInt64);
    bdRemoteTaskRef getUserTimelineGeneral(const bdUByte8, const bdNChar8 *, const bdUInt64, bdTweet *const, const bdUInt32, const bdUInt64, const bdUInt64, const bdBool);
    bdRemoteTaskRef followGeneral(const bdUInt64, const bdNChar8 *);
    bdRemoteTaskRef unfollowGeneral(const bdUInt64, const bdNChar8 *);
};
static_assert(sizeof(bdTwitter) == 0x8, "bdTwitter");


// Functions defined in this file (bodies are not in the DWARF):
//   line    73  void bdTwitter::operator delete(void * p)  [inlined into 1 functions]
//   line    73  void * bdTwitter::operator new(size_t nbytes)  [inline]
