// Reconstructed from original callers; owner: bdLobbyCommon/bdBufferParams.inl.
#pragma once
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
// line 19
inline bdBufferParams::bdBufferParams(bdUInt capacityIncreaseHint, bdUInt maxBufferSize, bdBool allocateBuffer)
    : m_hint(capacityIncreaseHint), m_maxBufferSize(maxBufferSize), m_serializeOk(true),
      m_buffer(static_cast<bdByteBuffer *>(0)),
      m_isWritingArray(false), m_arrayWriteCount(0)
{
    // Only allocateBuffer=false is observed/proven by the original TaskParams caller.
    // The true path delegates to the still-unimplemented allocation API; it is unproven.
    if (allocateBuffer) this->allocateBuffer();
}
// line 230
inline bdBool bdBufferParams::serializedOk() const
{
    if (m_isWritingArray || m_arrayWriteCount) {
        bdLogWarn("bdBufferParams", "Array serialization is incomplete", m_arrayWriteCount);
    }
    return !m_isWritingArray && !m_arrayWriteCount && m_serializeOk && m_buffer && m_buffer->getData();
}

// line 164. Capacity is checked even after an earlier serialization failure.
inline void bdBufferParams::addUInt32(const bdUInt32 & param)
{
    if (ensureCapacity(sizeof(param), true)) {
        if (m_isWritingArray) --m_arrayWriteCount;
        m_serializeOk = m_serializeOk && m_buffer->writeUInt32(param);
    }
}
// line 190
inline void bdBufferParams::addUInt64(const bdUInt64 & param)
{
    if (ensureCapacity(sizeof(param), true)) {
        if (m_isWritingArray) --m_arrayWriteCount;
        m_serializeOk = m_serializeOk && m_buffer->writeUInt64(param);
    }
}
































// Original bdBufferParams.inl:73, authenticated from bdStorage::uploadFile.
inline void bdBufferParams::addBool(const bdBool &param)
{
    if (ensureCapacity(sizeof(param), true))
    {
        if (m_isWritingArray)
        {
            --m_arrayWriteCount;
        }
        m_serializeOk = m_serializeOk && m_buffer->writeBool(param);
    }
}





















































// Original bdBufferParams.inl:138, authenticated from bdStorage::listFilesByOwner.
inline void bdBufferParams::addUInt16(const bdUInt16 &param)
{
    if (ensureCapacity(sizeof(param), true))
    {
        if (m_isWritingArray)
        {
            --m_arrayWriteCount;
        }
        m_serializeOk = m_serializeOk && m_buffer->writeUInt16(param);
    }
}
