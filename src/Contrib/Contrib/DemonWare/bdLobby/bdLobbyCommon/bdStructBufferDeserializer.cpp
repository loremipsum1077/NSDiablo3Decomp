#include "bdStructBufferDeserializer.h"
#include "bdStructBufferSerializable.h"
#include "bdProtobufHelper.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"

bdStructBufferDeserializer::bdStructBufferDeserializer(bdByteBufferRef buffer, bdUInt32 available,
                                                       bdUInt32 nestedDepth)
    : m_nextTag(0), m_nextType(0), m_nestedDepth(nestedDepth)
{
    prepare(buffer, available, nestedDepth);
}

bdStructBufferDeserializer::~bdStructBufferDeserializer()
{
    if (m_stream.getBufferRemaining() != 0)
    {
        m_stream.read(NULL, m_stream.getBufferRemaining());
    }
}

bdBool bdStructBufferDeserializer::prepare(bdByteBufferRef buffer, bdUInt32 available,
                                           bdUInt32 nestedDepth)
{
    m_nextTag = 0;
    m_nextType = 0;
    m_nestedDepth = nestedDepth;
    m_lastReadResult = BD_READ_SUCCESS;
    return m_stream.prepare(buffer, available);
}

bdBool bdStructBufferDeserializer::readNextTag()
{
    bdProtobufHelper::eWireType wireType;
    if (!bdProtobufHelper::decodeTag(m_stream, m_nextTag, wireType))
    {
        return false;
    }
    m_nextType = wireType;
    return true;
}

bdStructBufferDeserializer::bdReadResult
bdStructBufferDeserializer::readUntilTag(bdUInt32 tag, bdUInt32 tagType)
{
    bdReadResult result = BD_READ_SUCCESS_TAG_NOT_FOUND;
    while (m_nextTag < tag)
    {
        if (!skipCurrentTag())
        {
            result = BD_READ_FAILED_STREAM_ERROR;
            break;
        }
        if (m_stream.isEOF())
        {
            break;
        }
        if (!readNextTag())
        {
            result = BD_READ_FAILED_STREAM_ERROR;
            break;
        }
    }
    if (m_nextTag == tag)
    {
        if (m_nextType != tagType)
        {
            bdUseVAArgs(tag, m_nextType, tagType);
            result = BD_READ_FAILED_DATA_TYPE;
        }
        else
        {
            m_nextTag = 0;
            result = BD_READ_SUCCESS;
        }
    }
    return result;
}

bdBool bdStructBufferDeserializer::skipCurrentTag()
{
    if (m_nextTag != 0)
    {
        bdProtobufHelper::eWireType wireType =
            static_cast<bdProtobufHelper::eWireType>(m_nextType);
        if (!bdProtobufHelper::skip(m_stream, wireType))
        {
            return false;
        }
        m_nextTag = 0;
    }
    return true;
}

bdStructBufferDeserializer::bdReadResult bdStructBufferDeserializer::getLastReadResult()
{
    return m_lastReadResult;
}

bdBool bdStructBufferDeserializer::readUInt16(const bdUInt32 tag, bdUInt16 &value)
{
    bdUInt64 intVal;
    bdReadResult status = read(tag, intVal);
    if (status == BD_READ_SUCCESS)
    {
        if (intVal >= 0x10000)
        {
            bdUseVAArgs();
            status = BD_READ_FAILED_DATA_INTEGRITY;
        }
        else
        {
            status = BD_READ_SUCCESS;
            value = static_cast<bdUInt16>(intVal);
        }
    }
    return setLastReadResult(status);
}

bdBool bdStructBufferDeserializer::readUInt32(const bdUInt32 tag, bdUInt32 &value)
{
    bdUInt64 intVal;
    bdReadResult status = read(tag, intVal);
    if (status == BD_READ_SUCCESS)
    {
        if ((intVal >> 32) != 0)
        {
            bdUseVAArgs();
            status = BD_READ_FAILED_DATA_INTEGRITY;
        }
        else
        {
            status = BD_READ_SUCCESS;
            value = static_cast<bdUInt32>(intVal);
        }
    }
    return setLastReadResult(status);
}

bdBool bdStructBufferDeserializer::readUInt64(const bdUInt32 tag, bdUInt64 &value)
{
    bdReadResult status = read(tag, value);
    return setLastReadResult(status);
}

// Original line 301.
bdBool bdStructBufferDeserializer::readString(const bdUInt32 tag, bdNChar8 *const value,
                                              const bdUInt32 size)
{
    bdUInt32 stringLen = 0;
    bdBool ok = readBlob(tag, value, size, stringLen);
    if (ok && m_lastReadResult == BD_READ_SUCCESS && size != 0)
    {
        value[stringLen < size ? stringLen : size - 1] = '\0';
    }
    return ok;
}

bdBool bdStructBufferDeserializer::readBlob(const bdUInt32 tag, void *value,
                                            const bdUInt32 maxSize, bdUInt32 &readSize)
{
    bdReadResult status = readUntilTag(tag, bdProtobufHelper::WIRETYPE_STRING);
    if (status == BD_READ_SUCCESS)
    {
        bdUInt32 blobLen = maxSize;
        if (bdProtobufHelper::decodeString(m_stream, value, blobLen))
        {
            readSize = blobLen;
        }
        else
        {
            status = BD_READ_FAILED_STREAM_ERROR;
        }
    }
    return setLastReadResult(status);
}

bdBool bdStructBufferDeserializer::readObject(const bdUInt32 tag,
                                              bdStructBufferSerializable &object)
{
    bdReadResult status = readUntilTag(tag, bdProtobufHelper::WIRETYPE_STRING);
    if (status == BD_READ_SUCCESS)
    {
        bdUInt64 encSize = 0;
        if (bdProtobufHelper::decodeVarInt(m_stream, encSize))
        {
            if ((encSize >> 32) != 0)
            {
                bdProtobufHelper::skipBytes(m_stream, encSize);
                status = BD_READ_FAILED_DATA_INTEGRITY;
            }
            else
            {
                bdUInt32 bufferLen = static_cast<bdUInt32>(encSize);
                bdStructBufferDeserializer deserializer(m_stream.getBuffer(), bufferLen,
                                                         m_nestedDepth + 1);
                if (object.deserialize(deserializer))
                {
                    m_stream.read(bufferLen);
                }
                else
                {
                    status = deserializer.getLastReadResult();
                    // The original maps unsigned success values 0 and 1 to integrity failure.
                    if (static_cast<bdUInt32>(status) < BD_READ_FAILED_STREAM_ERROR)
                    {
                        status = BD_READ_FAILED_DATA_INTEGRITY;
                    }
                }
            }
        }
        else
        {
            status = BD_READ_FAILED_STREAM_ERROR;
        }
    }
    return setLastReadResult(status);
}

bdStructBufferDeserializer::bdReadResult bdStructBufferDeserializer::read(const bdUInt32 tag,
                                                                           bdUInt64 &value)
{
    bdReadResult status = readUntilTag(tag, bdProtobufHelper::WIRETYPE_VARINT);
    if (status == BD_READ_SUCCESS && !bdProtobufHelper::decodeVarInt(m_stream, value))
    {
        status = BD_READ_FAILED_STREAM_ERROR;
    }
    return status;
}
