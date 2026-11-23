// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSHeader.cpp

#include "bdDTLSHeader.h"
#include "../../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../../bdPlatform/bdPlatformLog/bdPlatformLog.h"

bdUInt bdDTLSHeader::s_serializedSize = 0;






bdDTLSHeader::bdDTLSHeader()
    : m_type(0), m_version(0), m_vtag(0), m_counter(0)
{
}




bdDTLSHeader::~bdDTLSHeader()
{
}

bdDTLSHeader::bdDTLSHeader(bdDTLSPacketTypes type, bdUInt16 vtag, bdUInt16 counter)
{
    m_type = type;
    m_version = BD_DTLS_VERSION;
    m_vtag = vtag;
    m_counter = counter;
}



bdBool bdDTLSHeader::serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const
{
    bdBool status = true;

    newOffset = offset;

    status = status && bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_type);
    status = status && bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_version);
    status = status && bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_vtag);
    status = status && bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_counter);

    if (!status)
    {
        newOffset = offset;
    }

    return status;
}



bdBool bdDTLSHeader::deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset)
{
    bdBool status = true;

    newOffset = offset;

    status = status && bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_type);
    status = status && bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_version);
    status = status && m_version == BD_DTLS_VERSION;
    status = status && bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_vtag);
    status = status && bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_counter);

    if (!status)
    {
        if (m_version != BD_DTLS_VERSION)
        {
            bdLogWarn("dtls", "Bad DTLS version.");
        }
        newOffset = 0;
    }

    return status;
}



bdUByte8 bdDTLSHeader::getType() const
{
    return m_type;
}

bdUInt16 bdDTLSHeader::getVtag() const
{
    return m_vtag;
}

bdUInt16 bdDTLSHeader::getCounter() const
{
    return m_counter;
}

bdUInt bdDTLSHeader::getSerializedSize() const
{
    if (s_serializedSize == 0)
    {
        bdDTLSHeader::serialize(NULL, 0xffff, 0, s_serializedSize);
    }
    return s_serializedSize;
}
