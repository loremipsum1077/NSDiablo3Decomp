// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdCODO/bdCODOTeamMarketplace.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include <stddef.h>
#include <stdint.h>

class bdCODOAwardCCPRequest;
class bdCODOAwardCCPResponse;
class bdCODOReadCCPLeaderboardRequest;
class bdCODOReadCCPLeaderboardResponse;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdCODOTeamMarketplace {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdCODOTeamMarketplace);
    bdRemoteTaskRef awardCCP(bdCODOAwardCCPRequest *, bdCODOAwardCCPResponse *);
    bdRemoteTaskRef readTeamMembersCCPLeaderboard(bdCODOReadCCPLeaderboardRequest *, bdCODOReadCCPLeaderboardResponse *);
protected:
    bdCODOTeamMarketplace(bdRemoteTaskManager *const);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdCODOTeamMarketplace) == 0x8, "bdCODOTeamMarketplace");


// Functions defined in this file (bodies are not in the DWARF):
//   line    27  void bdCODOTeamMarketplace::operator delete(void * p)  [inlined into 1 functions]
//   line    27  void * bdCODOTeamMarketplace::operator new(size_t nbytes)  [inline]
