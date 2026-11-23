// Adopted from the DWARF reference; hand-maintained declarations.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMail/bdMail.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdMailBody;
class bdMailCategoryCount;
class bdMailInfo;
class bdMailInfoAndBody;
class bdRemoteTaskManager;
class bdUserAccountID;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdMail {  // size 0x20
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdMail);
    virtual ~bdMail();  // vtable slot 0
    bdRemoteTaskRef getMailCountByCategory(bdMailCategoryCount *, const bdUInt32, const bdUserAccountID *);
    bdRemoteTaskRef getMailInfo(bdMailInfo *, const bdUInt32, const bdUInt32, const bdBool, const bdUserAccountID *);
    bdRemoteTaskRef getMailInfoForCategories(bdMailInfo * mailInfos, const bdUInt32 startIndex, const bdUInt32 numMails, const bdUInt16 * mailCategories, const bdUInt32 numCategories, const bdBool newestFirst, const bdUserAccountID * userID);
    bdRemoteTaskRef getMailInfoAndBodiesForCategories(bdMailInfoAndBody *, const bdUInt32, const bdUInt32, const bdUInt16 *, const bdUInt32, const bdBool, const bdUserAccountID *);
    bdRemoteTaskRef getMail(const bdUInt64 * mailIDs, const bdUInt32 numMailIDs, bdMailBody * mails, const bdBool deleteOnRead, const bdUserAccountID * userID);
    bdRemoteTaskRef deleteMail(const bdUInt64 * mailIDs, const bdUInt32 numMailIDs, const bdUserAccountID * userID);
    bdRemoteTaskRef sendMail(const void *const, const bdUInt32, const bdUInt16, const bdUserAccountID *, const bdUInt32, const bdUserAccountID *);
    bdRemoteTaskRef sendMailWithDuration(const void *const message, const bdUInt32 messageSize, const bdUInt16 mailCategory, const bdUInt32 messageDuration, const bdBool returnOnExpiration, const bdUserAccountID * userIDs, const bdUInt32 numUserIDs, const bdUserAccountID * senderID);
    bdRemoteTaskRef setMessageReadStatus(const bdUInt64 mailID, const bdBool status, const bdUserAccountID * userID);
protected:
    friend class bdLobbyService;
    bdMail(bdRemoteTaskManager *const, const bdNChar8 *);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x08
    bdNChar8 m_context[16];  // 0x10
};
static_assert(sizeof(bdMail) == 0x20, "bdMail");


// Functions defined in this file (bodies are not in the DWARF):
//   line    35  void bdMail::operator delete(void * p)  [inlined into 1 functions]
//   line    35  void * bdMail::operator new(size_t nbytes)  [inlined into 1 functions]
