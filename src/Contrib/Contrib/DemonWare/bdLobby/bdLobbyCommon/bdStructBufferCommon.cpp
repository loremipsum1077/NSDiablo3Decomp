#include "bdStructBufferCommon.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"

bdStructSerializationInputStream::bdStructSerializationInputStream()
{
}

bdStructSerializationInputStream::~bdStructSerializationInputStream()
{
}

bdBool bdStructSerializationInputStream::prepare(bdByteBufferRef buffer, bdUInt32 bufferSize)
{
    if (buffer == NULL)
    {
        bufferSize = 0;
    }
    bdStructSerializationStream::prepare(buffer, bufferSize);
    return true;
}

bdUInt32 bdStructSerializationInputStream::read(void *value, bdUInt32 byteCount)
{
    if (byteCount <= m_bufferSize)
    {
        if (m_buffer->read(value, byteCount))
        {
            return read(byteCount);
        }
        bdUseVAArgs(byteCount);
    }
    return 0;
}

bdUInt32 bdStructSerializationInputStream::read(bdUInt32 byteCount)
{
    m_bufferSize -= byteCount;
    return byteCount;
}
