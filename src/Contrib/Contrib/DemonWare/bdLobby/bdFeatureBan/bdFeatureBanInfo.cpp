#include "bdFeatureBanInfo.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"










bdFeatureBanInfo::bdFeatureBanInfo() : m_category(0), m_secondsRemaining(0) {}





bdFeatureBanInfo::~bdFeatureBanInfo() {}



bdBool bdFeatureBanInfo::deserialize(bdByteBufferRef buffer)
{
    bdBool ok = buffer->readUInt64(m_category);
    ok = ok && buffer->readUInt32(m_secondsRemaining);
    return ok;
}

bdUInt bdFeatureBanInfo::sizeOf() { return sizeof(bdFeatureBanInfo); }
