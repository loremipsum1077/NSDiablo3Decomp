// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLinkCode/bdLinkCode.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdLink;
class bdLinkExtended;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdLinkCode {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdLinkCode);
    bdLinkCode(bdRemoteTaskManager *const);
    ~bdLinkCode();
    bdRemoteTaskRef getLinkCodes(const bdUInt64, const bdUInt32 *const, bdLink *const, const bdUInt);
    bdRemoteTaskRef getEntityIDsFromLinkCodes(const bdUInt32, const bdNChar8 *const *const, bdLink *const, const bdUInt);
    bdRemoteTaskRef getLinkCodesExtended(const bdUInt64, const bdUInt32 *const, bdLinkExtended *, const bdUInt, const void *const, const bdUInt32, const bdBool);
    bdRemoteTaskRef getEntityIDsFromLinkCodesExtended(const bdUInt32, const bdNChar8 *const *, bdLinkExtended *, const bdUInt);
protected:
    bdLinkCode(bdRemoteTaskManager *const, const bdNChar8 *);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
    bdNChar8 m_context[16];  // 0x08
};
static_assert(sizeof(bdLinkCode) == 0x18, "bdLinkCode");


// Functions defined in this file (bodies are not in the DWARF):
//   line    34  void bdLinkCode::operator delete(void * p)  [inlined into 1 functions]
//   line    34  void * bdLinkCode::operator new(size_t nbytes)  [inline]
