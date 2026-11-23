// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdBytePacker.cpp

#include "bdBytePacker.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"

// line 14
bdBool bdBytePacker::appendBuffer(void* dest, const bdUInt32 destSize, const bdUInt offset, bdUInt& newOffset, const void* const src, const bdUInt writeSize)
{
    bdBool written = true;
    newOffset = offset + writeSize;
    if (dest && src)
    {
        const bdBool validStart = offset <= destSize;
        const bdBool validEnd = newOffset <= destSize;
        if (!validEnd)
        {
            bdLogError("bdBytePacker", "Not enough room to append %u bytes.", writeSize);
            written = false;
        }
        else if (!validStart)
        {
            written = false;
        }
        else
        {
            bdMemmove(static_cast<bdUByte8*>(dest) + offset, src, writeSize);
        }
    }
    return written;
}

// line 50
bdBool bdBytePacker::appendEncodedUInt16(void* buffer, const bdUInt bufferSize, const bdUInt offset, bdUInt& newOffset, const bdUInt16 value)
{
    if (value >= 0x80)
    {
        bdBool result;
        bdUByte8 firstByte = static_cast<bdUByte8>(value >> 8) | 0x80;
        bdUByte8 secondByte = static_cast<bdUByte8>(value);
        result = appendBasicType<bdUByte8>(buffer, bufferSize, offset, newOffset, firstByte);
        result = result && appendBasicType<bdUByte8>(buffer, bufferSize, newOffset, newOffset, secondByte);
        return result;
    }
    return appendBasicType<bdUByte8>(buffer, bufferSize, offset, newOffset, static_cast<bdUByte8>(value));
}

// line 83
bdBool bdBytePacker::removeBuffer(const void* src, const bdUInt srcSize, const bdUInt offset, bdUInt& newOffset, void* const dest, const bdUInt readSize)
{
    bdBool read = true;
    newOffset = offset + readSize;
    if (src && dest)
    {
        const bdBool validStart = offset <= srcSize;
        const bdBool validEnd = newOffset <= srcSize;
        if (!validEnd)
        {
            bdLogError("bdBytePacker", "Not enough room to remove %u bytes.", readSize);
            read = false;
        }
        else if (!validStart)
        {
            read = false;
        }
        else
        {
            bdMemmove(dest, static_cast<const bdUByte8*>(src) + offset, readSize);
        }
    }
    return read;
}

// line 120
bdBool bdBytePacker::removeEncodedUInt16(const bdUByte8* buffer, const bdUInt bufferSize, const bdUInt offset, bdUInt& newOffset, bdUInt16& value)
{
    bdUByte8 firstByte = 0;
    bdBool result = removeBasicType<bdUByte8>(buffer, bufferSize, offset, newOffset, firstByte);
    if (result && (firstByte & 0x80))
    {
        bdUByte8 secondByte = 0;
        // 'high' is not in the DWARF; the masked byte is held across the second read.
        const bdUByte8 high = firstByte & 0x7F;
        result = removeBasicType<bdUByte8>(buffer, bufferSize, newOffset, newOffset, secondByte);
        value = static_cast<bdUInt16>((high << 8) | secondByte);
    }
    else
    {
        value = firstByte;
    }
    return result;
}

// line 147
bdBool bdBytePacker::skipBytes(const void* const buffer, const bdUInt bufferSize, const bdUInt offset, bdUInt& newOffset, const bdUInt bytes)
{
    bdBool canSkip = false;
    newOffset = offset + bytes;
    if (buffer)
    {
        canSkip = newOffset <= bufferSize;
        if (!canSkip)
        {
            bdLogError("bdBytePacker", "Not enough room to skip bytes.");
        }
    }
    return canSkip;
}

// line 171
bdBool bdBytePacker::rewindBytes(const void* const buffer, const bdUInt, const bdUInt offset, bdUInt& newOffset, const bdUInt bytes)
{
    bdBool canRewind = false;
    newOffset = offset - bytes;
    if (buffer)
    {
        canRewind = offset >= bytes;
    }
    return canRewind;
}
