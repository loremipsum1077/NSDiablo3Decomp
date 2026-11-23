#include "bdDTLSCookieAck.h"
#include "../../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"

// Blank lines preserve the original definition and inline call-site lines.






bdDTLSCookieAck::bdDTLSCookieAck()
    : bdDTLSHeader(), m_secID()
{
}
bdDTLSCookieAck::~bdDTLSCookieAck()
{
}

bdDTLSCookieAck::bdDTLSCookieAck(bdUInt16 vtag,
                                  const bdUByte8 * key,
                                  const bdSecurityID secID)
    : bdDTLSHeader(BD_DTLS_COOKIE_ACK, vtag, 0), m_secID(secID)
{

    bdMemcpy(m_ECCKey, key, sizeof(m_ECCKey));
}

bdBool bdDTLSCookieAck::serialize(void * data,
                                const bdUInt size,
                                const bdUInt offset,
                                bdUInt & newOffset) const
{
    bdBool status = true;
    newOffset = offset;

    status = status && bdDTLSHeader::serialize(data, size, offset, newOffset);
    status = status && bdBytePacker::appendBuffer(data, size, newOffset, newOffset, m_ECCKey, sizeof(m_ECCKey));
    status = status && bdBytePacker::appendBuffer(data, size, newOffset, newOffset, m_secID.ab, sizeof(m_secID.ab));

    if (!status)
    {
        newOffset = offset;
    }

    return status;
}

bdBool bdDTLSCookieAck::deserialize(const void * data,
                                  const bdUInt size,
                                  const bdUInt offset,
                                  bdUInt & newOffset)
{
    bdBool status = true;
    newOffset = offset;

    status = status && bdDTLSHeader::deserialize(data, size, offset, newOffset);
    status = status && bdBytePacker::removeBuffer(data, size, newOffset, newOffset, m_ECCKey, sizeof(m_ECCKey));
    status = status && bdBytePacker::removeBuffer(data, size, newOffset, newOffset, m_secID.ab, sizeof(m_secID.ab));

    if (!status)
    {
        newOffset = offset;
    }

    return status;
}
const bdUByte8 * bdDTLSCookieAck::getECCKey() const
{
    return m_ECCKey;
}

void bdDTLSCookieAck::getSecID(bdSecurityID & secID) const
{
    secID = m_secID;
}
