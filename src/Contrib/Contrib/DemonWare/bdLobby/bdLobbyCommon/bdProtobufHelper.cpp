#include "bdProtobufHelper.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "bdStructBufferCommon.h"

bdBool bdProtobufHelper::decodeVarInt(bdStructSerializationInputStream &stream, bdUInt64 &value)
{
    bdUInt64 result = 0;
    bdUByte8 data = 0;
    bdUInt32 bitPos = 0;
    do
    {
        if (bitPos >= 64)
        {
            return false;
        }
        if (stream.read(&data, 1) != 1)
        {
            return false;
        }
        result |= static_cast<bdUInt64>(data & 0x7f) << bitPos;
        bitPos += 7;
    } while (data & 0x80);
    value = result;
    return true;
}

bdBool bdProtobufHelper::decodeString(bdStructSerializationInputStream &stream, void *value, bdUInt32 &len)
{
    bdUInt64 encodedSize;
    if (!decodeVarInt(stream, encodedSize))
    {
        return false;
    }
    bdUInt64 remainder = encodedSize > len ? encodedSize - len : 0;
    bdUInt32 decodeSize = encodedSize > len ? len : static_cast<bdUInt32>(encodedSize);
    bdBool status = stream.read(value, decodeSize) == decodeSize;
    if (status)
    {
        if (remainder > 0)
        {
            status = skipBytes(stream, remainder);
        }
        if (status)
        {
            len = decodeSize;
        }
    }
    // The original reports success after a valid length prefix even if data read or skip failed.
    return true;
}

bdBool bdProtobufHelper::skip(bdStructSerializationInputStream &stream, eWireType wireType)
{
    switch (wireType)
    {
    case WIRETYPE_VARINT:
    {
        bdUByte8 data;
        do
        {
            if (!stream.read(&data, 1))
            {
                return false;
            }
        } while (data & 0x80);
        return true;
    }
    case WIRETYPE_64BIT:
        return skipBytes(stream, 8);
    case WIRETYPE_STRING:
    {
        bdUInt64 stringLen;
        if (!decodeVarInt(stream, stringLen))
        {
            return false;
        }
        return skipBytes(stream, stringLen);
    }
    case WIRETYPE_32BIT:
        return skipBytes(stream, 4);
    default:
        return true;
    }
}

bdBool bdProtobufHelper::skipBytes(bdStructSerializationInputStream &stream, bdUInt64 byteCount)
{
    while (byteCount > 0)
    {
        bdUInt32 readLen = byteCount < 0xffffffff ? static_cast<bdUInt32>(byteCount) : 0xffffffff;
        if (!stream.read(nullptr, readLen))
        {
            return false;
        }
        byteCount -= readLen;
    }
    return true;
}

bdBool bdProtobufHelper::decodeTag(bdStructSerializationInputStream &stream, bdUInt32 &tagId, eWireType &wireType)
{
    bdUInt64 key;
    if (!decodeVarInt(stream, key))
    {
        return false;
    }
    tagId = static_cast<bdUInt32>(key >> 3);
    wireType = static_cast<eWireType>(key & 7);
    return true;
}
