// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTencent/bdTencentAASRecord.cpp
#include "bdTencentAASRecord.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"







bdTencentAASRecord::bdTencentAASRecord() : bdTaskResult(), m_userID(0), m_ageStatus(PENDING), m_gameplaySeconds(0), m_recordAge(0) {}







bdTencentAASRecord::~bdTencentAASRecord() {}



bdBool bdTencentAASRecord::deserialize(bdByteBufferRef buffer) {
    bdUByte8 version = 0;
    if (!buffer->readUByte8(version) || version != 1) {
        return false;
    }
    bdBool ok = buffer->readUInt64(m_userID);
    bdUByte8 ageStatus = 0;
    ok = ok && buffer->readUByte8(ageStatus);
    if (ok) {
        m_ageStatus = static_cast<bdRecordStatus>(ageStatus);
        ok = buffer->readUInt32(m_gameplaySeconds);
        ok = ok && buffer->readUInt32(m_recordAge);
    }
    return ok;
}











bdUInt bdTencentAASRecord::sizeOf() { return sizeof(bdTencentAASRecord); }
