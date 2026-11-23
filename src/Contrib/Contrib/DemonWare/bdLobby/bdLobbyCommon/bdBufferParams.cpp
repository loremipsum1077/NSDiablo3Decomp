// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdBufferParams.cpp
#include "bdBufferParams.h"
#include "bdSerializable.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString-unix.inl"

// Diagnostic text is inferred; this release retains only variadic arguments.
// line 27
bdBufferParams::~bdBufferParams() {}

// line 34
void bdBufferParams::addSerializable(const bdSerializable & param)
{
    if (!m_isWritingArray) {
        bdInt objectSize = param.serializedSizeOf();
        bdBool capacityOk = ensureCapacity(objectSize, false);
        if (capacityOk) m_serializeOk = m_serializeOk && param.serialize(*m_buffer);
    } else {
        bdLogError("bdBufferParams", "Cannot serialize an object inside an array");
        m_serializeOk = false;
    }
}

// line 126
void bdBufferParams::addString(const bdNChar8 * str, const bdUWord maxLen)
{
    if (!str || m_isWritingArray) {
        bdLogError("bdBufferParams", "Invalid string or array state");
        m_serializeOk = false;
    } else {
        const bdUWord strLength = bdStrnlen(str, maxLen);
        if (strLength >= maxLen) bdLogWarn("bdBufferParams", "String exceeds limit %lu", maxLen);
        bdUInt strSizeWithNull = strLength + 1;
        bdBool capacityOk = ensureCapacity(strSizeWithNull, true);
        if (capacityOk) m_serializeOk = m_serializeOk && m_buffer->writeString(str, maxLen);
    }
}

// line 169
void bdBufferParams::addBlob(const void * data, const bdUInt32 length)
{
    if ((!data && length) || m_isWritingArray) {
        bdLogError("bdBufferParams", "Invalid blob or array state");
        m_serializeOk = false;
    } else {
        bdUInt blobLength = length + sizeof(bdUInt32) + 1;
        bdBool capacityOk = ensureCapacity(blobLength, true);
        if (capacityOk) m_serializeOk = m_serializeOk && m_buffer->writeBlob(data, length);
    }
}

// line 248
bdBool bdBufferParams::ensureCapacity(bdUInt paramSize, bdBool typeChecked)
{
    bdUInt increaseSize = 0;
    bdBool expandOk = true;
    if (typeChecked) ++paramSize;
    bdInt32 remainBytes = m_buffer->getMaxWriteSize() - paramSize;
    if (remainBytes < 0) {
        bdUInt currentSize = m_buffer->getSize();
        bdUInt maxIncrease = m_maxBufferSize - currentSize;
        bdUInt needed = -remainBytes;
        if (maxIncrease < needed) {
            bdLogError("bdBufferParams", "Limit %u, size %u, needed %u", m_maxBufferSize, currentSize, needed);
            expandOk = false;
        } else {
            bdUInt lowerBound = m_hint < maxIncrease ? m_hint : maxIncrease;
            increaseSize = lowerBound > needed ? lowerBound : needed;
            bdLogInfo("bdBufferParams", "Expanding by %u", increaseSize);
            expandOk = m_buffer->expand(increaseSize);
        }
    }
    if (!expandOk) {
        bdLogError("bdBufferParams", "Failed to expand by %u", increaseSize);
        m_serializeOk = false;
    }
    return expandOk;
}
