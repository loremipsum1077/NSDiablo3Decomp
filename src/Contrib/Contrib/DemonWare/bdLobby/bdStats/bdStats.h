// Partial declaration from bdStats.h; enum values verified in cu_079dacd6.
#pragma once
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
class bdRemoteTaskManager;
class bdStatsInfo;
class bdStatsInfoEx;
typedef bdReference<bdRemoteTask> bdRemoteTaskRef;
class bdStats {
public:
    enum bdWriteType {
        STAT_WRITE_REPLACE = 0,
        STAT_WRITE_ADD = 1,
        STAT_WRITE_MAX = 2,
        STAT_WRITE_MIN = 3,
        STAT_WRITE_REPLACE_WHEN_RATING_INCREASE = 4,
        STAT_WRITE_ADD_WHEN_RATING_INCREASE = 5,
        STAT_WRITE_MAX_WHEN_RATING_INCREASE = 6,
        STAT_WRITE_MIN_WHEN_RATING_INCREASE = 7
    };
    BD_CLASS_ALLOCATOR(bdStats);
    ~bdStats();
    bdRemoteTaskRef writeStats(bdStatsInfo ** stats, const bdUInt numStats);
    bdRemoteTaskRef readStatsByRank(bdLeaderBoardID leaderBoardID, bdUInt64 firstRank, bdStatsInfo ** stats, bdUInt32 maxResults, const bdNChar8 ** excludedColumns, bdUInt32 numExclColumns);
    bdRemoteTaskRef readStatsByPivot(bdLeaderBoardID leaderBoardID, bdEntityID entityID, bdStatsInfo ** stats, bdUInt32 maxResults, const bdNChar8 ** excludedColumns, bdUInt32 numExclColumns);
    bdRemoteTaskRef readStatsByEntityID(bdLeaderBoardID leaderBoardID, const bdEntityID * entityIDs, bdUInt numEntityIDs, bdStatsInfo ** stats, const bdNChar8 ** excludedColumns, bdUInt32 numExclColumns);
    bdRemoteTaskRef readStatsByLeaderboardIDsAndEntityIDs(const bdEntityID * entityIDs, const bdLeaderBoardID * leaderboardIDs, bdStatsInfoEx ** statsResults, bdUInt32 numLeaderBoards, bdUInt32 numEntityIDs);
protected:
    friend class bdLobbyService;
    bdStats(bdRemoteTaskManager *const remoteTaskManager);
    bdRemoteTaskManager *m_remoteTaskManager;
private:
    bdRemoteTaskRef startTaskStatReadLeaderboardByRank(bdLeaderBoardID leaderBoardID, bdUInt64 firstRank, bdStatsInfo * stats, bdStatsInfo ** statPtrs, bdUInt32 maxResults, const bdNChar8 ** excludedColumns, bdUInt32 numExclColumns);
    bdRemoteTaskRef startTaskStatReadByEntityIDV2(bdLeaderBoardID leaderBoardID, const bdEntityID * entityIDs, bdUInt numEntityIDs, bdStatsInfo * stats, bdStatsInfo ** statPtrs, const bdNChar8 ** excludedColumns, bdUInt32 numExclColumns);
};
static_assert(sizeof(bdStats) == 8, "bdStats");
