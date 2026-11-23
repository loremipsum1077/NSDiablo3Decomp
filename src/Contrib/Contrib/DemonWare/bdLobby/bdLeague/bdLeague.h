// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLeague/bdLeague.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdLeagueAndSubdivisionResult;
class bdLeagueStatsInfo;
class bdLeagueSubdivisionHistoryResult;
class bdLeagueSubdivisionInfo;
class bdLeagueSubdivisionResult;
class bdLeagueTeamInfo;
class bdLeagueTeamMemberInfo;
class bdLeagueUserTeamResult;
class bdGenericLeagueID;  // bdGenericLeagueID.h, not adopted yet
typedef bdGenericLeagueID bdLeagueTeamID;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdLeague {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdLeague);
    bdLeague(bdRemoteTaskManager *const);
    ~bdLeague();
    bdRemoteTaskRef getTeamID(const bdEntityID *, bdUInt32, bdLeagueTeamID *);
    bdRemoteTaskRef setTeamName(bdUInt64, const bdNChar8 *);
    bdRemoteTaskRef setTeamIcon(bdUInt64, const bdUByte8 *, bdUInt32);
    bdRemoteTaskRef getTeamInfos(const bdUInt64 *, bdLeagueTeamInfo *, bdUInt32);
    bdRemoteTaskRef getTeamMemberInfos(const bdUInt64 *, bdLeagueTeamMemberInfo *, bdUInt32);
    bdRemoteTaskRef getTeamIDsForUser(bdEntityID, bdUByte8, bdLeagueUserTeamResult *, bdUInt32, bdUInt32);
    bdRemoteTaskRef getTeamSubdivisions(bdUInt64, const bdUInt64 *, bdUInt32, bdLeagueSubdivisionResult *);
    bdRemoteTaskRef getTeamLeaguesAndSubdivisions(bdUInt64, bdLeagueAndSubdivisionResult *, bdUInt32, bdUInt32);
    bdRemoteTaskRef incrementGamesPlayedCount(const bdUInt64 *, const bdUInt64 *, bdUInt32);
    bdRemoteTaskRef getSubdivisionInfos(const bdUInt64 *, bdLeagueSubdivisionInfo *, bdUInt32);
    bdRemoteTaskRef getTeamSubdivisionHistory(bdUInt64, bdUInt64, const bdUInt64 *, bdLeagueSubdivisionHistoryResult *, bdUInt32);
    bdRemoteTaskRef writeStats(const bdLeagueStatsInfo &);
    bdRemoteTaskRef readStatsByTeamID(const bdUInt64 *, bdUInt64, bdLeagueStatsInfo *, bdUInt32);
    bdRemoteTaskRef readStatsByRank(bdUInt64, bdUInt32, bdLeagueStatsInfo *, bdUInt32);
    bdRemoteTaskRef readStatsByPivot(const bdUInt64, const bdUInt64, bdLeagueStatsInfo *, bdUInt32);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdLeague) == 0x8, "bdLeague");


// Functions defined in this file (bodies are not in the DWARF):
//   line    92  void bdLeague::operator delete(void * p)  [inlined into 1 functions]
//   line    92  void * bdLeague::operator new(size_t nbytes)  [inline]
