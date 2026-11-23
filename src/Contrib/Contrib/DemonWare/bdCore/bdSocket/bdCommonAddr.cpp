// Original file: Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.cpp
#include "bdCommonAddr.h"
#include "../bdContainers/bdBitBuffer.h"
#include "../bdCrypto/bdHashTiger192.h"
#include "../bdUtilities/bdBytePacker.h"

// line 16
bdCommonAddr::bdCommonAddr()
    : m_localAddrs(0), m_publicAddr(), m_natType(BD_NAT_OPEN), m_hash(0), m_isLoopback(false)
{
}

// line 23
bdCommonAddr::~bdCommonAddr() {}

// line 26
bdCommonAddr::bdCommonAddr(const bdArray<bdAddr> & localAddrs, const bdAddr & publicAddr, const bdNATType natType)
    : m_localAddrs(localAddrs), m_publicAddr(publicAddr), m_natType(natType), m_isLoopback(true)
{
    calculateHash();
}

// line 60
bdCommonAddr::bdCommonAddr(const bdAddr & publicAddr)
    : m_localAddrs(0), m_publicAddr(publicAddr), m_natType(BD_NAT_OPEN), m_isLoopback(false)
{
    m_localAddrs.pushBack(publicAddr);
    calculateHash();
}

// line 72
bdUInt bdCommonAddr::getHash() const { return m_hash; }
// line 77
bdBool bdCommonAddr::isLoopback() const { return m_isLoopback; }
// line 82
bdNATType bdCommonAddr::getNATType() const { return m_natType; }

// line 88
void bdCommonAddr::serialize(bdUByte8 * buffer) const
{
    bdUInt offset = 0;
    const bdUInt bufferSize = 67;
    bdBool status = true;
    const bdAddr invalidAddr;
    for (bdUInt i = 0; i < 10; ++i) {
        if (i < m_localAddrs.getSize()) status = status && m_localAddrs[i].serialize(buffer, bufferSize, offset, offset);
        else status = status && invalidAddr.serialize(buffer, bufferSize, offset, offset);
    }
    status = status && m_publicAddr.serialize(buffer, bufferSize, offset, offset);
    status = status && bdBytePacker::appendBasicType<bdByte8>(buffer, bufferSize, offset, offset, static_cast<bdByte8>(m_natType));
}

// line 119
void bdCommonAddr::serialize(bdBitBufferRef buffer) const
{
    bdUByte8 tmpBuffer[67];
    const bdUInt tmpBufferSize = 67;
    serialize(tmpBuffer);
    buffer->writeBits(tmpBuffer, tmpBufferSize * 8);
}

// line 129
bdBool bdCommonAddr::deserialize(bdCommonAddrRef me, const bdUByte8 * buffer)
{
    bdUInt offset = 0;
    const bdUInt bufferSize = 67;
    bdBool status = true;
    m_localAddrs.clear();
    for (bdUInt i = 0; i < 10; ++i) {
        bdAddr localAddr;
        status = status && localAddr.deserialize(buffer, bufferSize, offset, offset);
        if (status && localAddr.getAddress().isValid()) m_localAddrs.pushBack(localAddr);
    }
    bdByte8 tmpByte;
    status = status && m_publicAddr.deserialize(buffer, bufferSize, offset, offset);
    status = status && bdBytePacker::removeBasicType<bdByte8>(buffer, bufferSize, offset, offset, tmpByte);
    if (status) {
        m_natType = static_cast<bdNATType>(tmpByte);
        calculateHash();
        m_isLoopback = static_cast<bdCommonAddr *>(me) && *me == *this;
    }
    return status;
}

// line 171
bdBool bdCommonAddr::deserialize(bdCommonAddrRef me, bdBitBufferRef buffer)
{
    bdUByte8 tmpBuffer[67];
    const bdUInt tmpBufferSize = 67;
    return buffer->readBits(tmpBuffer, tmpBufferSize * 8) && deserialize(me, tmpBuffer);
}

// line 187
bdBool bdCommonAddr::operator==(const bdCommonAddr & other) const
{
    bdAddr localAddr = m_localAddrs.getSize() ? m_localAddrs[0] : bdAddr();
    bdAddr otherLocalAddr = other.m_localAddrs.getSize() ? other.m_localAddrs[0] : bdAddr();
    if (m_hash != other.m_hash) return false;
    const bdAddr * myAddr = m_publicAddr.getAddress().isValid() ? &m_publicAddr : &localAddr;
    const bdAddr * otherAddr = other.m_publicAddr.getAddress().isValid() ? &other.m_publicAddr : &otherLocalAddr;
    return *myAddr == *otherAddr;
}

// line 223
bdBool bdCommonAddr::operator!=(const bdCommonAddr & other) const { return m_hash != other.m_hash; }
const bdAddr & bdCommonAddr::getPublicAddr() const { return m_publicAddr; }
const bdArray<bdAddr> & bdCommonAddr::getLocalAddrs() const { return m_localAddrs; }
const bdAddr & bdCommonAddr::getLocalAddrByIndex(const bdUInt index) const { return m_localAddrs[index]; }

// line 265
void bdCommonAddr::calculateHash()
{
    bdAddr addr;
    const bdAddr localAddr = m_localAddrs.getSize() ? m_localAddrs[0] : bdAddr();
    if (m_publicAddr.getAddress().isValid()) addr = m_publicAddr;
    else addr = localAddr;
    bdUByte8 data[6];
    const bdUWord dataSize = 6;
    bdUInt offset = 0;
    bdBool status = addr.serialize(data, dataSize, offset, offset);
    bdUByte8 hash[24];
    const bdUInt hashSize = 24;
    bdUInt tmpHashSize = hashSize;
    bdHashTiger192 tiger;
    status = status && tiger.hash(data, dataSize, hash, tmpHashSize);
    status = status && bdBytePacker::removeBasicType<bdUInt>(hash, hashSize, 0, offset, m_hash);
}
