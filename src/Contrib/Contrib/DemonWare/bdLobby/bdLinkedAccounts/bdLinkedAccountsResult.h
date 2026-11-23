// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLinkedAccounts/bdLinkedAccountsResult.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../bdLobbyCommon/bdUserAccountID.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdSetLinkedAccountsResult : public bdTaskResult {  // size 0x3a0
public:
    // The class operators new/delete are inline in the original (operator delete is
    // inlined into the deleting destructor), as BD_CLASS_ALLOCATOR defines them.
    BD_CLASS_ALLOCATOR(bdSetLinkedAccountsResult);





    bdSetLinkedAccountsResult();
    virtual ~bdSetLinkedAccountsResult();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    bdUserAccountID m_linkedAccountsResult[16];  // 0x08
    bdUInt32 m_numLinkedAccounts;  // 0x388
};
static_assert(sizeof(bdSetLinkedAccountsResult) == 0x3a0, "bdSetLinkedAccountsResult");

class bdLinkedAccountsResult : public bdTaskResult {  // size 0x3d8
public:
    bdLinkedAccountsResult();
    virtual ~bdLinkedAccountsResult();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    bdUserAccountID m_userAccount;  // 0x08
    bdUserAccountID m_linkedAccountsResult[16];  // 0x40
    bdUInt32 m_numLinkedAccounts;  // 0x3c0
};
static_assert(sizeof(bdLinkedAccountsResult) == 0x3d8, "bdLinkedAccountsResult");


// Functions defined in this file (bodies are not in the DWARF):
//   line    25  void bdSetLinkedAccountsResult::operator delete(void * p)  [inline]
//   line    42  bdLinkedAccountsResult::bdLinkedAccountsResult(const bdLinkedAccountsResult &)  [0x710003f140]
