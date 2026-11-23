// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdPlayerVote/bdPlayerVote.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdPlayerVoteCountResponse;
class bdPlayerVoteStatusResponse;
class bdRemoteTaskManager;
class bdUserAccountID;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdPlayerVote {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdPlayerVote);
    bdPlayerVote(bdRemoteTaskManager *const, const bdNChar8 *);
    ~bdPlayerVote();
    bdRemoteTaskRef voteForPlayer(const bdUserAccountID &);
    bdRemoteTaskRef getVoteStatusForUsers(const bdUserAccountID *, const bdUInt32, bdPlayerVoteStatusResponse *);
    bdRemoteTaskRef getVoteCountsForUsers(const bdUserAccountID *, const bdUInt32, bdPlayerVoteCountResponse *);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
    bdNChar8 m_context[16];  // 0x08
};
static_assert(sizeof(bdPlayerVote) == 0x18, "bdPlayerVote");


// Functions defined in this file (bodies are not in the DWARF):
//   line    32  void bdPlayerVote::operator delete(void * p)  [inlined into 1 functions]
//   line    32  void * bdPlayerVote::operator new(size_t nbytes)  [inline]
