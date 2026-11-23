// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTencent/bdTencentResponse.cpp
#include "bdTencentResponse.h"
#include "../bdLobbyCommon/bdStructBufferDeserializer.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"























bdTSSStructMessage::bdTSSStructMessage() {
    reset();
}


bdTSSStructMessage::~bdTSSStructMessage() {}



inline void bdTSSStructMessage::reset() {
    m_creditRating = 0;
    m_queryInfoSize = 0;
    bdMemset(m_queryInfo, 0, sizeof(m_queryInfo));
}


bdBool bdTSSStructMessage::deserialize(bdStructBufferDeserializer & deserializer) {
    m_queryInfoSize = 0;
    if (deserializer.readUInt32(1, m_creditRating)) {
        return deserializer.readBlob(2, m_queryInfo, sizeof(m_queryInfo), m_queryInfoSize);
    }
    return false;
}
