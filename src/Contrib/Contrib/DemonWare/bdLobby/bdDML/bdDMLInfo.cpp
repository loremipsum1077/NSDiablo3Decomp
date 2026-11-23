#include "bdDMLInfo.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"










bdDMLInfo::bdDMLInfo() : m_latitude(0.0f), m_longitude(0.0f), m_asn(0)
{
    bdMemset(m_countryCode, 0, sizeof(m_countryCode));
    bdMemset(m_country, 0, sizeof(m_country));
    bdMemset(m_region, 0, sizeof(m_region));
    bdMemset(m_city, 0, sizeof(m_city));
    bdMemset(m_timezone, 0, sizeof(m_timezone));
}








bdDMLInfo::~bdDMLInfo() {}




bdBool bdDMLInfo::deserialize(bdByteBufferRef buffer)
{

    bdBool ok = buffer->readString(m_countryCode, sizeof(m_countryCode))
        && buffer->readString(m_country, sizeof(m_country))
        && buffer->readString(m_region, sizeof(m_region))
        && buffer->readString(m_city, sizeof(m_city))
        && buffer->readFloat32(m_latitude)
        && buffer->readFloat32(m_longitude)


        && buffer->readUInt32(m_asn);

    if (!ok) {
        m_asn = 0;
        return false;
    }



    return buffer->readString(m_timezone, sizeof(m_timezone));


}


bdUInt bdDMLInfo::sizeOf() { return sizeof(bdDMLInfo); }
