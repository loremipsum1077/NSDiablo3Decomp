// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSInit.cpp
#include "bdDTLSInit.h"
#include "../bdDTLSPackets.h"
#include "../../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"






bdDTLSInit::bdDTLSInit() : bdDTLSHeader(),
    m_initTag(0),
    m_secID(),
    m_random(),
    m_cypherSuite(0)
{
}

bdDTLSInit::bdDTLSInit(bdUInt16 initTag,
                       const bdSecurityID & secID,
                       const bdDTLSRandom & randomData,
                       const bdCypherSuite & cypherSuite)
    : bdDTLSHeader(BD_DTLS_INIT, 0, 0),
    m_initTag(initTag),
    m_secID(secID),
    m_random(randomData),
    m_cypherSuite(cypherSuite)
{
}

bdDTLSInit::~bdDTLSInit()
{
}

bdBool bdDTLSInit::serialize(void * data,
                           const bdUInt size,
                           const bdUInt offset,
                           bdUInt & newOffset) const
{
    bdBool status = true;
    newOffset = offset;

    status = status && bdDTLSHeader::serialize(data, size, offset, newOffset);
    status = status && bdBytePacker::appendBasicType<bdUInt16>(data, size, newOffset, newOffset, m_initTag);
    status = status && bdBytePacker::appendBuffer(data, size, newOffset, newOffset, m_secID.ab, sizeof(m_secID.ab));


    if (BD_DTLS_VERSION >= 3)
    {
        status = status && bdBytePacker::appendBasicType<bdUInt16>(data, size, newOffset, newOffset, m_cypherSuite);
        status = status && bdBytePacker::appendBuffer(data, size, newOffset, newOffset, m_random.getData(), sizeof(m_random.getData()));
    }

    if (!status)
    {
        newOffset = offset;
    }
    return status;
}

bdBool bdDTLSInit::deserialize(const void * data,
                             const bdUInt size,
                             const bdUInt offset,
                             bdUInt & newOffset,
                             const bdCypherSuite expectedCypherSuite)
{
    bdBool status = true;
    newOffset = offset;

    status = status && bdDTLSHeader::deserialize(data, size, offset, newOffset);
    status = status && bdBytePacker::removeBasicType<bdUInt16>(data, size, newOffset, newOffset, m_initTag);
    status = status && bdBytePacker::removeBuffer(data, size, newOffset, newOffset, m_secID.ab, sizeof(m_secID.ab));


    if (BD_DTLS_VERSION >= 3)
    {

        status = status && bdBytePacker::removeBasicType<bdUInt16>(data, size, newOffset, newOffset, m_cypherSuite);
        bdUByte8 randomData[32];



        status = status && m_cypherSuite == expectedCypherSuite && bdBytePacker::removeBuffer(data, size, newOffset, newOffset, randomData, sizeof(randomData));
        status = status && m_random.setData(randomData);
    }

    if (!status)
    {
        newOffset = offset;
    }

    return status;
}

bdUInt16 bdDTLSInit::getInitTag() const
{
    return m_initTag;
}

void bdDTLSInit::getSecID(bdSecurityID & secID) const
{
    secID = m_secID;
}

const bdDTLSRandom & bdDTLSInit::getRandomData() const
{
    return m_random;
}
