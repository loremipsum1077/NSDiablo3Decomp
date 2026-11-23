// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSInitAck.cpp
#include "bdDTLSInitAck.h"
#include "../bdDTLSPackets.h"
#include "../../../bdCore/bdCrypto/bdCryptoUtils.h"
#include "../../../bdCore/bdCrypto/bdHMac.h"
#include "../../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"





bdDTLSInitAck::bdDTLSInitAck() : bdDTLSHeader(),
    m_cypherSuite(0),
    m_timestamp(0),
    m_initTag(0),
    m_localTag(0),
    m_peerTag(0),
    m_localTieTag(0),
    m_peerTieTag(0),
    m_localRandom(),
    m_peerRandom()
{
    bdMemset(m_signature, 0, sizeof(m_signature));
}

bdDTLSInitAck::bdDTLSInitAck(bdUInt16 vtag,
                             bdUInt16 initTag,
                             bdUInt16 localTag,
                             bdUInt16 peerTag,
                             bdUInt16 localTieTag,
                             bdUInt16 peerTieTag,
                             const bdCypherSuite cypherSuite,
                             const bdDTLSRandom & localRandom,
                             const bdDTLSRandom & peerRandom,
                             bdUInt32 timestamp,
                             const bdAddr & peerAddr,
                             bdSecurityID secID)
    : bdDTLSHeader(BD_DTLS_INIT_ACK, vtag, 0),
    m_cypherSuite(cypherSuite),
    m_timestamp(timestamp),
    m_initTag(initTag),
    m_localTag(localTag),
    m_peerTag(peerTag),
    m_localTieTag(localTieTag),
    m_peerTieTag(peerTieTag),
    m_localRandom(localRandom),
    m_peerRandom(peerRandom),
    m_peerAddr(peerAddr),
    m_secID(secID)
{
    bdCryptoUtils::zeroBuffer(m_signature, sizeof(m_signature));
}

bdDTLSInitAck::~bdDTLSInitAck()
{

    m_initTag = m_localTag = m_peerTag = m_localTieTag = m_peerTieTag = 0;



    bdCryptoUtils::zeroBuffer(m_signature, sizeof(m_signature));
}

bdBool bdDTLSInitAck::sign(bdHMac & hmac)
{
    bdBool ok = true;
    bdUInt signatureSize = sizeof(m_signature);
    bdMemset(m_signature, 0, sizeof(m_signature));

    ok = ok && hmac.process(reinterpret_cast<const bdUByte8 *>(&m_timestamp), sizeof(m_timestamp));
    ok = ok && hmac.process(reinterpret_cast<const bdUByte8 *>(&m_initTag), sizeof(m_initTag));
    ok = ok && hmac.process(reinterpret_cast<const bdUByte8 *>(&m_localTag), sizeof(m_localTag));
    ok = ok && hmac.process(reinterpret_cast<const bdUByte8 *>(&m_peerTag), sizeof(m_peerTag));
    ok = ok && hmac.process(reinterpret_cast<const bdUByte8 *>(&m_localTieTag), sizeof(m_localTieTag));
    ok = ok && hmac.process(reinterpret_cast<const bdUByte8 *>(&m_peerTieTag), sizeof(m_peerTieTag));


    if (BD_DTLS_VERSION >= 3)
    {
        ok = ok && hmac.process(reinterpret_cast<const bdUByte8 *>(m_secID.ab), sizeof(m_secID.ab));
        ok = ok && hmac.process(reinterpret_cast<const bdUByte8 *>(&m_cypherSuite), sizeof(m_cypherSuite));
        ok = ok && hmac.process(reinterpret_cast<const bdUByte8 *>(m_localRandom.getData()), sizeof(m_localRandom.getData()));
        ok = ok && hmac.process(reinterpret_cast<const bdUByte8 *>(m_peerRandom.getData()), sizeof(m_peerRandom.getData()));
    }

    bdUInt addrSize = 0;
    bdUByte8 addrBuffer[136];
    ok = ok && m_peerAddr.serialize(addrBuffer, sizeof(addrBuffer), 0, addrSize);
    ok = ok && hmac.process(addrBuffer, addrSize);

    ok = ok && hmac.getData(m_signature, signatureSize);

    return ok && signatureSize >= sizeof(m_signature);
}


bdBool bdDTLSInitAck::verify(bdHMac & hmac) const
{




    bdUByte8 signature[16] = {0};
    bdUInt signatureSize = sizeof(signature);

    bdBool ok = hmac.process(reinterpret_cast<const bdUByte8 *>(&m_timestamp), sizeof(m_timestamp));
    ok &= hmac.process(reinterpret_cast<const bdUByte8 *>(&m_initTag), sizeof(m_initTag));
    ok &= hmac.process(reinterpret_cast<const bdUByte8 *>(&m_localTag), sizeof(m_localTag));
    ok &= hmac.process(reinterpret_cast<const bdUByte8 *>(&m_peerTag), sizeof(m_peerTag));
    ok &= hmac.process(reinterpret_cast<const bdUByte8 *>(&m_localTieTag), sizeof(m_localTieTag));
    ok &= hmac.process(reinterpret_cast<const bdUByte8 *>(&m_peerTieTag), sizeof(m_peerTieTag));


    if (BD_DTLS_VERSION >= 3)
    {
        ok &= hmac.process(reinterpret_cast<const bdUByte8 *>(m_secID.ab), sizeof(m_secID.ab));
        ok &= hmac.process(reinterpret_cast<const bdUByte8 *>(&m_cypherSuite), sizeof(m_cypherSuite));
        ok &= hmac.process(reinterpret_cast<const bdUByte8 *>(m_localRandom.getData()), sizeof(m_localRandom.getData()));
        ok &= hmac.process(reinterpret_cast<const bdUByte8 *>(m_peerRandom.getData()), sizeof(m_peerRandom.getData()));
    }

    bdUInt addrSize = 0;
    bdUByte8 addrBuffer[136];
    ok &= m_peerAddr.serialize(addrBuffer, sizeof(addrBuffer), 0, addrSize);
    ok &= hmac.process(addrBuffer, addrSize);
    ok &= hmac.getData(signature, signatureSize);

    ok = bdCryptoUtils::constTimeCompare(m_signature, signature, sizeof(m_signature)) && ok;

    return ok;
}

bdBool bdDTLSInitAck::serialize(void * data,
                              const bdUInt size,
                              const bdUInt offset,
                              bdUInt & newOffset) const
{
    bdBool status = true;
    newOffset = offset;

    status = status && bdDTLSHeader::serialize(data, size, offset, newOffset);
    status = status && bdBytePacker::appendBasicType<bdUInt32>(data, size, newOffset, newOffset, m_timestamp);
    status = status && bdBytePacker::appendBuffer(data, size, newOffset, newOffset, m_signature, sizeof(m_signature));
    status = status && bdBytePacker::appendBasicType<bdUInt16>(data, size, newOffset, newOffset, m_initTag);
    status = status && bdBytePacker::appendBasicType<bdUInt16>(data, size, newOffset, newOffset, m_localTag);
    status = status && bdBytePacker::appendBasicType<bdUInt16>(data, size, newOffset, newOffset, m_peerTag);
    status = status && bdBytePacker::appendBasicType<bdUInt16>(data, size, newOffset, newOffset, m_localTieTag);
    status = status && bdBytePacker::appendBasicType<bdUInt16>(data, size, newOffset, newOffset, m_peerTieTag);
    status = status && m_peerAddr.serialize(data, size, newOffset, newOffset);
    status = status && bdBytePacker::appendBuffer(data, size, newOffset, newOffset, m_secID.ab, sizeof(m_secID.ab));


    if (BD_DTLS_VERSION >= 3)
    {
        status = status && bdBytePacker::appendBasicType<bdUInt16>(data, size, newOffset, newOffset, m_cypherSuite);
        status = status && bdBytePacker::appendBuffer(data, size, newOffset, newOffset, m_localRandom.getData(), sizeof(m_localRandom.getData()));
        status = status && bdBytePacker::appendBuffer(data, size, newOffset, newOffset, m_peerRandom.getData(), sizeof(m_peerRandom.getData()));
    }

    if (!status)
    {
        newOffset = offset;
    }
    return status;
}

bdBool bdDTLSInitAck::deserialize(const void * data,
                                const bdUInt size,
                                const bdUInt offset,
                                bdUInt & newOffset,
                                const bdCypherSuite expectedCypherSuite)
{
    bdBool status = true;
    newOffset = offset;

    status = status && bdDTLSHeader::deserialize(data, size, offset, newOffset);
    status = status && bdBytePacker::removeBasicType<bdUInt32>(data, size, newOffset, newOffset, m_timestamp);
    status = status && bdBytePacker::removeBuffer(data, size, newOffset, newOffset, m_signature, sizeof(m_signature));
    status = status && bdBytePacker::removeBasicType<bdUInt16>(data, size, newOffset, newOffset, m_initTag);
    status = status && bdBytePacker::removeBasicType<bdUInt16>(data, size, newOffset, newOffset, m_localTag);
    status = status && bdBytePacker::removeBasicType<bdUInt16>(data, size, newOffset, newOffset, m_peerTag);
    status = status && bdBytePacker::removeBasicType<bdUInt16>(data, size, newOffset, newOffset, m_localTieTag);
    status = status && bdBytePacker::removeBasicType<bdUInt16>(data, size, newOffset, newOffset, m_peerTieTag);
    status = status && m_peerAddr.deserialize(data, size, newOffset, newOffset);
    status = status && bdBytePacker::removeBuffer(data, size, newOffset, newOffset, m_secID.ab, sizeof(m_secID.ab));


    if (BD_DTLS_VERSION >= 3)
    {
        status = status && bdBytePacker::removeBasicType<bdUInt16>(data, size, newOffset, newOffset, m_cypherSuite);

        if (status && m_cypherSuite != expectedCypherSuite)
        {
            bdLogWarn("bdSocket/dtls", "Unexpected cypher suite");
            status = false;
        }

        bdUByte8 localRandom[32];
        bdUByte8 peerRandom[32];
        status = status && bdBytePacker::removeBuffer(data, size, newOffset, newOffset, localRandom, sizeof(localRandom));
        status = status && bdBytePacker::removeBuffer(data, size, newOffset, newOffset, peerRandom, sizeof(peerRandom));
        status = status && m_localRandom.setData(localRandom);
        status = status && m_peerRandom.setData(peerRandom);
    }

    if (!status)
    {
        newOffset = offset;
    }
    return status;
}

bdUInt32 bdDTLSInitAck::getTimestamp() const
{
    return m_timestamp;
}

bdUInt16 bdDTLSInitAck::getInitTag() const
{
    return m_initTag;
}

bdUInt16 bdDTLSInitAck::getLocalTag() const
{
    return m_localTag;
}

bdUInt16 bdDTLSInitAck::getPeerTag() const
{
    return m_peerTag;
}

bdUInt16 bdDTLSInitAck::getLocalTieTag() const
{
    return m_localTieTag;
}

bdUInt16 bdDTLSInitAck::getPeerTieTag() const
{
    return m_peerTieTag;
}

const bdAddr & bdDTLSInitAck::getPeerAddr() const
{
    return m_peerAddr;
}

void bdDTLSInitAck::getSecID(bdSecurityID & secID) const
{
    secID = m_secID;
}

const bdDTLSRandom & bdDTLSInitAck::getLocalRandom() const
{
    return m_localRandom;
}

const bdDTLSRandom & bdDTLSInitAck::getPeerRandom() const
{
    return m_peerRandom;
}
