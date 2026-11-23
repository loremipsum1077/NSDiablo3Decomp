#include "bdDTLSError.h"
#include "../../../bdCore/bdUtilities/bdBytePacker.h"

// Blank lines preserve the original definition and inline call-site lines.






bdDTLSError::bdDTLSError()
    : bdDTLSHeader(), m_etype(BD_DTLS_INVALID_STATE), m_secID()
{
}

bdDTLSError::bdDTLSError(bdUInt16 vtag, bdDTLSErrorType etype, const bdSecurityID & secID)
    : bdDTLSHeader(BD_DTLS_ERROR, vtag, 0), m_etype(etype), m_secID(secID)
{
}





bdDTLSError::~bdDTLSError()
{
}

bdBool bdDTLSError::serialize(void * data,
                            const bdUInt size,
                            const bdUInt offset,
                            bdUInt & newOffset) const
{
    bdBool status = true;
    newOffset = offset;

    status = status && bdDTLSHeader::serialize(data, size, offset, newOffset);
    bdUByte8 shortEtype = static_cast<bdUByte8>(m_etype);
    status = status && bdBytePacker::appendBasicType(data, size, newOffset, newOffset, shortEtype);
    status = status && bdBytePacker::appendBuffer(data, size, newOffset, newOffset, m_secID.ab, sizeof(m_secID.ab));

    if (!status)
    {
        newOffset = offset;
    }

    return status;
}

bdBool bdDTLSError::deserialize(const void * data,
                              const bdUInt size,
                              const bdUInt offset,
                              bdUInt & newOffset)
{
    bdBool status = true;
    newOffset = offset;

    status = status && bdDTLSHeader::deserialize(data, size, offset, newOffset);
    bdUByte8 shortEtype = 0xff;
    status = status && bdBytePacker::removeBasicType(data, size, newOffset, newOffset, shortEtype);
    m_etype = static_cast<bdDTLSErrorType>(shortEtype);
    status = status && bdBytePacker::removeBuffer(data, size, newOffset, newOffset, m_secID.ab, sizeof(m_secID.ab));

    if (!status)
    {
        newOffset = offset;
    }

    return status;
}

bdDTLSError::bdDTLSErrorType bdDTLSError::getEtype() const
{
    return m_etype;
}

void bdDTLSError::getSecID(bdSecurityID & secID) const
{
    secID = m_secID;
}
