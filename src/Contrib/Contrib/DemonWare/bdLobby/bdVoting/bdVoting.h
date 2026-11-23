// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdVoting/bdVoting.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdRemoteTaskManager;
class bdVoteBallot;
class bdWinningVoteCandidate;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdVoting {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdVoting);
    bdRemoteTaskRef castVotes(const bdNChar8 *const, const bdVoteBallot *const, const bdUInt32);
    bdRemoteTaskRef getVoteTotals(const bdNChar8 *const, bdVoteBallot *, const bdUInt32);
    bdRemoteTaskRef getWinningCandidate(const bdNChar8 *const, bdWinningVoteCandidate *const);
protected:
    friend class bdLobbyService;  // not in the DWARF: bdLobbyService::cleanup deletes this through the protected destructor
    ~bdVoting();
    bdVoting(bdRemoteTaskManager *const);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdVoting) == 0x8, "bdVoting");


// Functions defined in this file (bodies are not in the DWARF):
//   line    55  void bdVoting::operator delete(void * p)  [inlined into 1 functions]
//   line    55  void * bdVoting::operator new(size_t nbytes)  [inline]
