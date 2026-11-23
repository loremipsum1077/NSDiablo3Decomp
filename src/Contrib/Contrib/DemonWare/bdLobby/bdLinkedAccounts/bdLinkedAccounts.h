// Declarations recovered from the original DWARF; original line layout retained.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLinkedAccounts/bdLinkedAccounts.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdLinkedAccountsResult;
class bdRemoteTaskManager;
class bdString;
class bdUserAccountID;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;
























class bdLinkedAccounts {  // size 0x10
public:

    BD_CLASS_ALLOCATOR(bdLinkedAccounts);

    bdLinkedAccounts(bdRemoteTaskManager *const remoteTaskManager);
    virtual ~bdLinkedAccounts();  // vtable slot 0








    bdRemoteTaskRef getLinkedAccounts(const bdUserAccountID * userAccountIDs, bdLinkedAccountsResult * results, bdUInt32 count);







    bdRemoteTaskRef setLinkedAccounts(const bdString &);








    bdRemoteTaskRef switchContextData(const bdUserAccountID &, const bdUserAccountID &, const bdNChar8 *);

protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x08
};
static_assert(sizeof(bdLinkedAccounts) == 0x10, "bdLinkedAccounts");
