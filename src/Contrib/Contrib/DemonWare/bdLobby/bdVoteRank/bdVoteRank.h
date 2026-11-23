// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdVoteRank/bdVoteRank.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdCategorizedRatingInfo;
class bdRatingInfo;
class bdRemoteTaskManager;
class bdVoteRankStatsInfo;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdVoteRank {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdVoteRank);
    ~bdVoteRank();
    bdRemoteTaskRef submitRating(bdRatingInfo *, const bdUInt32);
    bdRemoteTaskRef submitCategorizedRating(bdCategorizedRatingInfo *, const bdUInt32);
    bdRemoteTaskRef getVoteHistory(const bdUInt16, bdCategorizedRatingInfo *, const bdUInt32, const bdUInt32);
    bdBool getLikeDislikeRatioFromRating(const bdVoteRankStatsInfo *, bdUInt32 &, bdUInt32 &) const;
protected:
    bdVoteRank(bdRemoteTaskManager *const);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdVoteRank) == 0x8, "bdVoteRank");


// Functions defined in this file (bodies are not in the DWARF):
//   line    49  void bdVoteRank::operator delete(void * p)  [inlined into 1 functions]
//   line    49  void * bdVoteRank::operator new(size_t nbytes)  [inline]
