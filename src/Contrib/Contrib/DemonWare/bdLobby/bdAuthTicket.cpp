#include "bdAuthTicket.h"
#include "../bdCore/bdUtilities/bdBytePacker.h"
#include "../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include <string.h>









static const bdUByte8 BD_AUTH_TICKET_MAGIC_BYTES[3] = {0x55, 0x33, 0x22};

bdAuthTicket::bdAuthTicket()
{
    m_magicNumber = 0;
    m_type = 0;
    m_titleID = 0;
    m_timeIssued = 0;
    m_timeExpires = 0;
    memset(&m_licenseID, 0, 0x68);



    m_usingHashMagicNumber[0] = BD_AUTH_TICKET_MAGIC_BYTES[0];
    m_usingHashMagicNumber[1] = BD_AUTH_TICKET_MAGIC_BYTES[1];
    m_usingHashMagicNumber[2] = BD_AUTH_TICKET_MAGIC_BYTES[2];
    bdMemset(m_hash, 0, sizeof(m_hash));
}









































bdAuthTicket::~bdAuthTicket() {}





































void bdAuthTicket::deserialize(const void * buffer)
{

    const bdUInt bufferSize = 128;
    bdUInt offset = 0;


    bdBool result = bdBytePacker::removeBasicType(buffer, bufferSize, 0, offset, m_magicNumber);
    result = result && bdBytePacker::removeBasicType(buffer, bufferSize, offset, offset, m_type);
    result = result && bdBytePacker::removeBasicType(buffer, bufferSize, offset, offset, m_titleID);
    result = result && bdBytePacker::removeBasicType(buffer, bufferSize, offset, offset, m_timeIssued);
    result = result && bdBytePacker::removeBasicType(buffer, bufferSize, offset, offset, m_timeExpires);
    result = result && bdBytePacker::removeBasicType(buffer, bufferSize, offset, offset, m_licenseID);
    result = result && bdBytePacker::removeBasicType(buffer, bufferSize, offset, offset, m_userID);
    result = result && bdBytePacker::removeBuffer(buffer, bufferSize, offset, offset, m_username, sizeof(m_username));
    result = result && bdBytePacker::removeBuffer(buffer, bufferSize, offset, offset, m_sessionKey, sizeof(m_sessionKey));
    result = result && bdBytePacker::removeBuffer(buffer, bufferSize, offset, offset, m_usingHashMagicNumber, sizeof(m_usingHashMagicNumber));
    result = result && bdBytePacker::removeBuffer(buffer, bufferSize, offset, offset, m_hash, sizeof(m_hash));



}
