// Original file: Contrib/Contrib/DemonWare/bdLobby/bdStats/bdStats.cpp
// Diagnostic text is inferred; this release retains only variadic arguments.
#include "bdStats.h"
#include "bdStatsInfo.h"
#include "../bdLobbyCommon/bdTaskParams.h"
#include "../bdRemoteTaskManager/bdRemoteTaskManager.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"

bdStats::bdStats(bdRemoteTaskManager *const remoteTaskManager) : m_remoteTaskManager(remoteTaskManager) {}
bdStats::~bdStats() { m_remoteTaskManager = 0; }

bdRemoteTaskRef bdStats::writeStats(bdStatsInfo ** stats, const bdUInt numStats)
{
    bdRemoteTaskRef task;
    bdTaskParams params(4, 1, 1024, 0x1fffff);
    for (bdUInt i = 0; i < numStats; ++i) params.addSerializable(*stats[i]);
    const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, params);
    if (startTaskResult != BD_NO_ERROR) bdLogWarn("bdStats", "Failed to start task");
    return task;
}

bdRemoteTaskRef bdStats::readStatsByRank(bdLeaderBoardID leaderBoardID, bdUInt64 firstRank, bdStatsInfo ** stats, bdUInt32 maxResults, const bdNChar8 ** excludedColumns, bdUInt32 numExclColumns)
{
    return startTaskStatReadLeaderboardByRank(leaderBoardID, firstRank, 0, stats, maxResults, excludedColumns, numExclColumns);
}

bdRemoteTaskRef bdStats::startTaskStatReadLeaderboardByRank(bdLeaderBoardID leaderBoardID, bdUInt64 firstRank, bdStatsInfo * stats, bdStatsInfo ** statPtrs, bdUInt32 maxResults, const bdNChar8 ** excludedColumns, bdUInt32 numExclColumns)
{
    bdRemoteTaskRef task;
    bdTaskParams params(4, 4, 1024, 0x1fffff);
    params.addUInt32(leaderBoardID);
    params.addUInt64(firstRank);
    params.addUInt32(maxResults);
    params.addUInt32(numExclColumns);
    for (bdUInt32 i = 0; i < numExclColumns; ++i) params.addString(excludedColumns[i], 65);
    if (stats) params.bindResults(stats, maxResults);
    else if (statPtrs) params.bindResultRefs(reinterpret_cast<bdTaskResult **>(statPtrs), maxResults);
    const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, params);
    if (startTaskResult != BD_NO_ERROR) bdLogWarn("bdStats", "Failed to start task");
    return task;
}

bdRemoteTaskRef bdStats::readStatsByPivot(bdLeaderBoardID leaderBoardID, bdEntityID entityID, bdStatsInfo ** stats, bdUInt32 maxResults, const bdNChar8 ** excludedColumns, bdUInt32 numExclColumns)
{
    bdRemoteTaskRef task;
    bdTaskParams params(4, 5, 1024, 0x1fffff);
    params.addUInt32(leaderBoardID);
    params.addUInt64(entityID);
    params.addUInt32(maxResults);
    params.addUInt32(numExclColumns);
    for (bdUInt32 i = 0; i < numExclColumns; ++i) params.addString(excludedColumns[i], 65);
    params.bindResultRefs(reinterpret_cast<bdTaskResult **>(stats), maxResults);
    const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, params);
    if (startTaskResult != BD_NO_ERROR) bdLogWarn("bdStats", "Failed to start task");
    return task;
}

bdRemoteTaskRef bdStats::readStatsByEntityID(bdLeaderBoardID leaderBoardID, const bdEntityID * entityIDs, bdUInt numEntityIDs, bdStatsInfo ** stats, const bdNChar8 ** excludedColumns, bdUInt32 numExclColumns)
{
    return startTaskStatReadByEntityIDV2(leaderBoardID, entityIDs, numEntityIDs, 0, stats, excludedColumns, numExclColumns);
}

bdRemoteTaskRef bdStats::startTaskStatReadByEntityIDV2(bdLeaderBoardID leaderBoardID, const bdEntityID * entityIDs, bdUInt numEntityIDs, bdStatsInfo * stats, bdStatsInfo ** statPtrs, const bdNChar8 ** excludedColumns, bdUInt32 numExclColumns)
{
    bdRemoteTaskRef task;
    bdTaskParams params(4, 13, 1024, 0x1fffff);
    params.addUInt32(leaderBoardID);
    params.addUInt32(numEntityIDs);
    for (bdUInt i = 0; i < numEntityIDs; ++i) params.addUInt64(entityIDs[i]);
    params.addUInt32(numExclColumns);
    for (bdUInt32 i = 0; i < numExclColumns; ++i) params.addString(excludedColumns[i], 65);
    if (stats) params.bindResults(stats, numEntityIDs);
    else if (statPtrs) params.bindResultRefs(reinterpret_cast<bdTaskResult **>(statPtrs), numEntityIDs);
    const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, params);
    if (startTaskResult != BD_NO_ERROR) bdLogWarn("bdStats", "Failed to start task");
    return task;
}

// The 1303 log passes startTaskResult (the original calls bdUseVAArgs with it still in w0, so the call is not merged with 1308).
#line 1250
bdRemoteTaskRef bdStats::readStatsByLeaderboardIDsAndEntityIDs(const bdEntityID * entityIDs,
	const bdLeaderBoardID * leaderboardIDs,
	bdStatsInfoEx ** statsResults,
	const bdUInt32 numLeaderBoards,
	const bdUInt32 numEntityIDs)
{
	bdRemoteTaskRef task;


	bdTaskParams params(4, 11, 1024, 0x1fffff);

	bdBool statsInfoSizesMatch = true;
	bdUInt lastStatsInfoSize = 0;


	params.addUInt32(numEntityIDs);
	for (bdUInt i = 0; i < numEntityIDs; ++i)
	{
		params.addUInt64(entityIDs[i]);
	}


	params.addUInt32(numLeaderBoards);
	for(bdUInt i = 0; (i < numLeaderBoards) && (statsInfoSizesMatch); ++i) // 1273:68 is the ++i of this longer condition
	{
		params.addUInt32(leaderboardIDs[i]);

		if ((lastStatsInfoSize != 0) && (statsResults[i]->m_statsInfo->sizeOf() != lastStatsInfoSize))
		{
			statsInfoSizesMatch = false;

			bdLogWarn("bdStats", "Stats sizes differ");
		}
		else
		{


			lastStatsInfoSize = statsResults[i]->m_statsInfo->sizeOf();
		}
	}




	if (statsInfoSizesMatch)
	{
		params.bindResultRefs((bdTaskResult **)statsResults, numLeaderBoards * numEntityIDs);


		const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, params);

		if (startTaskResult != BD_NO_ERROR)
		{
			bdLogWarn("bdStats", "Failed to start task: %u", startTaskResult);
		}
	}
	else
	{
		bdLogError("bdStats", "Cannot start task");
	}

	return task;
}

