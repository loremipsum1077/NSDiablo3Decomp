#include "bdMatchMakingInfo.h"
#include "../../bdCore/bdSocket/bdCommonAddr.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"

bdMatchMakingInfo::bdMatchMakingInfo()
    : m_sessionID(), m_hostAddrSize(0), m_gameType(0), m_maxPlayers(0), m_numPlayers(0)
{
    bdMemset(m_hostAddr, 0, sizeof(m_hostAddr));
}
bdMatchMakingInfo::bdMatchMakingInfo(const bdCommonAddrRef hostAddr, const bdInt32 gameType, const bdUInt32 maxNumPlayers)
    : m_sessionID(), m_gameType(gameType), m_maxPlayers(maxNumPlayers)
{
    // The original leaves m_numPlayers and the unused blob tail untouched here.
    hostAddr->serialize(m_hostAddr);
    m_hostAddrSize = 67;
}
bdMatchMakingInfo::~bdMatchMakingInfo() {}
bdCommonAddrRef bdMatchMakingInfo::getHostAddrAsCommonAddr(bdCommonAddrRef localCommonAddr) const
{
    bdCommonAddrRef commonAddr;
    if (m_hostAddrSize == 67) {
        commonAddr = new bdCommonAddr();
        // The original returns the allocated object even if decoding fails.
        bdBool ok = commonAddr->deserialize(localCommonAddr, m_hostAddr);
        if (!localCommonAddr) bdLogWarn("bdMatchMakingInfo", "Local common address is NULL.");
    } else {
        bdLogWarn("bdMatchMakingInfo", "Invalid host address size.");
    }
    return commonAddr;
}
bdBool bdMatchMakingInfo::serialize(bdByteBuffer &buffer) const
{
    bdBool writeOk = buffer.writeBlob(m_hostAddr, m_hostAddrSize);
    writeOk = writeOk && buffer.writeUInt32(m_gameType);
    writeOk = writeOk && buffer.writeUInt32(m_maxPlayers);
    return writeOk;
}
bdBool bdMatchMakingInfo::deserialize(bdByteBufferRef buffer)
{
    bdUInt length = sizeof(m_hostAddr);
    bdBool ok = buffer->readBlob(m_hostAddr, length);
    // Publish the lengths and numeric outputs even after a failed read.
    m_hostAddrSize = length;
    length = sizeof(m_sessionID.ab);
    ok = ok && buffer->readBlob(m_sessionID.ab, length);
    bdUInt32 intValue = 0;
    ok = ok && buffer->readUInt32(intValue);
    m_gameType = intValue;
    bdUInt32 uintValue = 0;
    ok = ok && buffer->readUInt32(uintValue);
    m_maxPlayers = uintValue;
    ok = ok && buffer->readUInt32(uintValue);
    m_numPlayers = uintValue;
    if (!ok) bdLogWarn("bdMatchMakingInfo", "Failed to deserialize match making info.");
    return ok;
}
bdUInt bdMatchMakingInfo::sizeOf() { return sizeof(bdMatchMakingInfo); }
