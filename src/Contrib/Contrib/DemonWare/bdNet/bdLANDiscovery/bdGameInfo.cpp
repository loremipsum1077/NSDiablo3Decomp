// Original file: Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdGameInfo.cpp
// Reconstructed from DWARF function/inline trees and original instruction line tables.
// Release logging drops channel/format text; its descriptive strings below are inferred.
#include "bdGameInfo.h"
#include "../../bdCore/bdContainers/bdBitBuffer.h"
#include "../../bdCore/bdSocket/bdCommonAddr.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"

bdGameInfo::bdGameInfo()
    : m_titleId(0x29a), m_hostAddr(NULL)
{
}

bdGameInfo::~bdGameInfo()
{
}

bdUInt32 bdGameInfo::getTitleID() const
{
    if (m_titleId == 0x29a)
    {
        bdLogWarn("lan discovery", "Title ID has not been set.");
    }
    return m_titleId;
}

bdCommonAddrRef bdGameInfo::getHostAddr() const
{
    if (m_hostAddr.isNull())
    {
        bdLogWarn("lan discovery", "Host address has not been set.");
    }
    return m_hostAddr;
}

const bdSecurityID& bdGameInfo::getSecurityID() const
{
    return m_secID;
}

const bdSecurityKey& bdGameInfo::getSecurityKey() const
{
    return m_secKey;
}

void bdGameInfo::setTitleID(const bdUInt32 titleID)
{
    m_titleId = titleID;
}

void bdGameInfo::setHostAddr(const bdCommonAddrRef hostAddr)
{
    m_hostAddr = hostAddr;
}

void bdGameInfo::setSecurityID(const bdSecurityID& secID)
{
    m_secID = secID;
}

void bdGameInfo::setSecurityKey(const bdSecurityKey& secKey)
{
    m_secKey = secKey;
}

void bdGameInfo::serialize(bdBitBuffer& bitBuffer) const
{
    if (m_titleId == 0x29a)
    {
        bdLogWarn("lan discovery", "Title ID has not been set.");
    }
    if (m_hostAddr.isNull())
    {
        bdLogError("lan discovery", "Cannot serialize without a host address.");
        return;
    }
    bdUByte8 hostAddrBuffer[67];
    bitBuffer.writeUInt32(m_titleId);
    bitBuffer.writeFull(m_secID.ab);
    bitBuffer.writeFull(m_secKey.ab);
    m_hostAddr->serialize(hostAddrBuffer);
    bitBuffer.writeBits(hostAddrBuffer, sizeof(hostAddrBuffer) * 8);
}

bdBool bdGameInfo::deserialize(const bdCommonAddrRef localAddr, bdBitBuffer& bitBuffer)
{
    bdBool ok = true;
    bdUInt32 tempTitleID;
    bdSecurityID tempSecID;
    bdSecurityKey tempSecKey;
    bdCommonAddrRef tempCommonAddr;

    ok = bitBuffer.readUInt32(tempTitleID);
    ok = ok && bitBuffer.readFull(tempSecID.ab);
    ok = ok && bitBuffer.readFull(tempSecKey.ab);

    bdUByte8 tempAddrBuffer[67];
    ok = ok && bitBuffer.readBits(tempAddrBuffer, sizeof(tempAddrBuffer) * 8);
    if (ok)
    {
        tempCommonAddr = new bdCommonAddr();
        ok = tempCommonAddr->deserialize(localAddr, tempAddrBuffer);
    }

    if (!ok)
    {
        bdLogError("lan discovery", "Failed to deserialize game information.");
    }
    else
    {
        m_titleId = tempTitleID;
        bdMemcpy(m_secID.ab, tempSecID.ab, sizeof(m_secID.ab));
        bdMemcpy(m_secKey.ab, tempSecKey.ab, sizeof(m_secKey.ab));
        m_hostAddr = tempCommonAddr;
    }
    return ok;
}
