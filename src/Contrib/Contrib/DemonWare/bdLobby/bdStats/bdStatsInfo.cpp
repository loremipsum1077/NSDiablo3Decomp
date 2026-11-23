// bdLobby/bdStats/bdStatsInfo.cpp -- one leaderboard/entity stats record and
// its wire (de)serialisation.
#include "bdStatsInfo.h"

#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"

#include <string.h>

bdStatsInfo::bdStatsInfo()
	: m_leaderboardID(0U),
	  m_entityID(0U),
	  m_writeType(bdStats::STAT_WRITE_ADD),
	  m_rating(0),
	  m_rank(0U),
	  m_secondsSinceUpdate(0U)
{
	memset(m_entityName, 0, sizeof(m_entityName));
}

bdStatsInfo::~bdStatsInfo()
{
}

bdBool bdStatsInfo::serialize(bdByteBuffer &buffer) const
{
	bdBool ok = buffer.writeUInt32(m_leaderboardID);
	ok = ok && buffer.writeUInt64(m_entityID);
	ok = ok && buffer.writeUByte8(static_cast<bdUByte8>(m_writeType));
	ok = ok && buffer.writeInt64(m_rating);
	if (ok)
	{
		return true;
	}
	bdLogWarn("stats", "Failed to serialize stats info.");
	return false;
}

bdBool bdStatsInfo::deserialize(bdByteBufferRef buffer)
{
	bdBool ok = buffer->readUInt64(m_entityID);
	ok = ok && buffer->readInt64(m_rating);
	ok = ok && buffer->readUInt64(m_rank);
	ok = ok && buffer->readString(m_entityName, sizeof(m_entityName) - 1);
	ok = ok && buffer->readUInt32(m_secondsSinceUpdate);
	if (ok)
	{
		return true;
	}
	bdLogWarn("stats", "Failed to deserialize stats info.");
	return false;
}

bdUInt bdStatsInfo::sizeOf()
{
	return sizeof(bdStatsInfo);
}

bdStatsInfoEx::bdStatsInfoEx()
	: bdTaskResult(),
	  m_leaderboardID(0U),
	  m_error(0U),
	  m_entityID(0U),
	  m_statsInfo(NULL)
{
}

bdStatsInfoEx::~bdStatsInfoEx()
{
}

bdBool bdStatsInfoEx::deserialize(bdByteBufferRef buffer)
{
	bdBool ok = m_statsInfo != NULL;
	ok = ok && buffer->readUInt32(m_leaderboardID);
	ok = ok && buffer->readUInt32(m_error);
	if (ok)
	{
		if (m_error == 0U)
		{
			ok = m_statsInfo->deserialize(buffer);
			if (ok)
			{
				m_entityID = m_statsInfo->m_entityID;
				m_statsInfo->m_leaderboardID = m_leaderboardID;
			}
		}
		else
		{
			ok = buffer->readUInt64(m_entityID);
		}
	}
	return ok;
}

bdUInt bdStatsInfoEx::sizeOf()
{
	return sizeof(bdStatsInfoEx);
}
