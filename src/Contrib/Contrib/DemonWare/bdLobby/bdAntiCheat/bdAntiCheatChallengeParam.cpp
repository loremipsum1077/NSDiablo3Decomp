// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAntiCheat/bdAntiCheatChallengeParam.cpp
#include "bdAntiCheatChallengeParam.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdCore/bdMemory/bdMemory.inl"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"





bdAntiCheatChallengeParam::bdAntiCheatChallengeParam() : m_data(0), m_size(0) {}













bdAntiCheatChallengeParam::bdAntiCheatChallengeParam(const bdAntiCheatChallengeParam & other) {
    m_size = other.m_size;
    m_data = bdAllocate<bdUByte8>(m_size);
    bdMemcpy(m_data, other.m_data, m_size);
}


bdAntiCheatChallengeParam::~bdAntiCheatChallengeParam() {
    if (m_data) {
        bdDeallocate<bdUByte8>(m_data);
    }
}


















bdAntiCheatChallengeParam & bdAntiCheatChallengeParam::operator=(const bdAntiCheatChallengeParam & other) {
    if (m_data) {
        bdDeallocate<bdUByte8>(m_data);
    }
    m_size = other.m_size;
    if (other.m_data) {
        m_data = bdAllocate<bdUByte8>(m_size);
        bdMemcpy(m_data, other.m_data, m_size);
    } else {
        m_data = 0;
    }
    return *this;
}








bdBool bdAntiCheatChallengeParam::deserialize(bdByteBufferRef buffer) {
    if (m_data) {
        bdDeallocate<bdUByte8>(m_data);
        m_data = 0;
        m_size = 0;
    }
    return buffer->readAndAllocateBlob(m_data, m_size);
}




bdUInt bdAntiCheatChallengeParam::sizeOf() { return m_size; }
