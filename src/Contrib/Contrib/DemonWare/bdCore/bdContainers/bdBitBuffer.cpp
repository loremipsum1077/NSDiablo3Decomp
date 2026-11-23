// Original bdBitBuffer.cpp. Reconstructed using DWARF names and inline trees.
#include "bdBitBuffer.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"

bdBitBuffer::bdBitBuffer(const bdUInt capacityBits, const bdBool typeChecked)
    : bdReferencable(), m_data((capacityBits >> 3) + ((capacityBits & 7) ? 1 : 0)),
      m_writePosition(0), m_maxWritePosition(0), m_readPosition(0),
      m_failedRead(false), m_typeChecked(typeChecked)
{
    if (typeChecked) {
        const bdUByte8 byte = 0xff;
        writeBits(&byte, 1);
    } else {
        const bdUByte8 byte = 0;
        writeBits(&byte, 1);
    }
    resetReadPosition();
}

bdBitBuffer::bdBitBuffer(const bdUByte8* bits, const bdUInt numBits, const bdBool dataHasTypeCheckedBit)
    : bdReferencable(), m_data(), m_writePosition(0), m_maxWritePosition(0),
      m_readPosition(0), m_failedRead(false), m_typeChecked(false)
{
    if (numBits == 0 || !dataHasTypeCheckedBit) {
        const bdUByte8 byte = 0;
        writeBits(&byte, 1);
        writeBits(bits, numBits);
        resetReadPosition();
    } else {
        m_data.pushBack(bits, (numBits >> 3) + ((numBits & 7) ? 1 : 0));
        m_writePosition = numBits;
        m_maxWritePosition = numBits;
        readBits(&m_typeChecked, 1);
    }
}

bdBitBuffer::~bdBitBuffer() {}

void bdBitBuffer::writeRangedUInt32(const bdUInt u, const bdUInt begin, const bdUInt end,
                                  const bdBool typeChecked)
{
    if (typeChecked) {
        writeDataType(BD_BB_RANGED_UNSIGNED_INTEGER32_TYPE);
        if (m_typeChecked) {
            writeUInt32(begin);
            writeUInt32(end);
        }
    }
    bdUInt rangeBits = 0;
    const bdUInt rangeSize = end - begin;
    if (rangeSize) rangeBits = bdBitOperations::highBitNumber(rangeSize) + 1;
    const bdUInt v = u > end ? end : (u < begin ? begin : u);
    bdUInt norm = v - begin;
    bdUInt nnorm;
    bdBitOperations::endianSwap(norm, nnorm);
    writeBits(&nnorm, rangeBits);
}

void bdBitBuffer::writeRangedInt32(const bdInt i, const bdInt begin, const bdInt end)
{
    writeDataType(BD_BB_RANGED_SIGNED_INTEGER32_TYPE);
    if (m_typeChecked) {
        writeInt32(begin);
        writeInt32(end);
    }
    bdUInt rangeBits = 0;
    const bdUInt rangeSize = end - begin;
    if (rangeSize) rangeBits = bdBitOperations::highBitNumber(rangeSize) + 1;
    const bdInt v = i > end ? end : (i < begin ? begin : i);
    bdUInt norm = v - begin;
    bdUInt nnorm;
    bdBitOperations::endianSwap(norm, nnorm);
    writeBits(&nnorm, rangeBits);
}

void bdBitBuffer::writeBits(const void* bits, const bdUInt numBits)
{
    bdUInt bitsToWrite = numBits;
    const bdUByte8* src = static_cast<const bdUByte8*>(bits);
    const bdUInt lastByteIndex = (m_writePosition + numBits - 1) >> 3;
    if (!m_data.rangeCheck(lastByteIndex)) m_data.setGrow(lastByteIndex, 0);
    while (bitsToWrite) {
        const bdUInt lastSrcByteIndex = (numBits - 1) >> 3;
        const bdUInt upShift = m_writePosition & 7;
        const bdUInt downShift = 8 - upShift;
        const bdUInt bitsWritten = bitsToWrite < downShift ? bitsToWrite : downShift;
        const bdUByte8 mask = (0xff << (bitsWritten + upShift)) | (0xff >> downShift);
        const bdUInt nextDestByteIndex = m_writePosition >> 3;
        const bdUByte8 maskedDest = m_data[nextDestByteIndex] & mask;
        const bdUInt srcBitsRead = numBits - bitsToWrite;
        const bdUInt srcDownShift = srcBitsRead & 7;
        const bdUInt srcUpShift = 8 - srcDownShift;
        const bdUInt currentSrcByteIndex = srcBitsRead >> 3;
        bdUByte8 currentSrcByte = src[currentSrcByteIndex];
        bdUByte8 nextSrcByte = 0;
        if (lastSrcByteIndex > currentSrcByteIndex)
            nextSrcByte = src[currentSrcByteIndex + 1];
        currentSrcByte >>= srcDownShift;
        nextSrcByte <<= srcUpShift;
        const bdUByte8 invMask = ~mask;
        const bdUByte8 maskedSrc = ((currentSrcByte | nextSrcByte) << upShift) & invMask;
        m_data[nextDestByteIndex] = maskedDest | maskedSrc;
        m_writePosition += bitsWritten;
        bitsToWrite -= bitsWritten;
        m_maxWritePosition = m_maxWritePosition > m_writePosition ? m_maxWritePosition : m_writePosition;
    }
}

bdBool bdBitBuffer::readRangedUInt32(bdUInt& u, const bdUInt begin, const bdUInt end,
                                  const bdBool typeChecked)
{
    bdBool ok = true;
    if (typeChecked) {
        ok = readDataType(BD_BB_RANGED_UNSIGNED_INTEGER32_TYPE);
        if (m_typeChecked) {
            bdUInt32 bufBegin;
            bdUInt32 bufEnd;
            ok = ok && readUInt32(bufBegin);
            ok = ok && readUInt32(bufEnd);
            if (ok && (bufBegin != begin || bufEnd != end))
                bdLogWarn("bdBitBuffer", "Range mismatch [%u, %u] vs [%u, %u].", begin, end, bufBegin, bufEnd);
        }
    }
    if (ok) {
        bdUInt rangeBits = 0;
        const bdUInt rangeSize = end - begin;
        if (rangeSize) rangeBits = bdBitOperations::highBitNumber(rangeSize) + 1;
        bdUInt nu = 0;
        ok = readBits(&nu, rangeBits);
        if (ok) {
            bdBitOperations::endianSwap(nu, u);
            u += begin;
            u = u > end ? end : (u < begin ? begin : u);
        }
    }
    return ok;
}

bdBool bdBitBuffer::readRangedInt32(bdInt& i, const bdInt begin, const bdInt end)
{
    bdBool ok = readDataType(BD_BB_RANGED_SIGNED_INTEGER32_TYPE);
    if (m_typeChecked) {
        bdInt32 bufBegin = 0;
        bdInt32 bufEnd = 0;
        ok = ok && readInt32(bufBegin);
        ok = ok && readInt32(bufEnd);
        if (ok && (bufBegin != begin || bufEnd != end))
            bdLogWarn("bdBitBuffer", "Range mismatch [%i, %i] vs [%i, %i].", begin, end, bufBegin, bufEnd);
    }
    if (ok) {
        bdUInt rangeBits = 0;
        const bdUInt rangeSize = end - begin;
        if (rangeSize) rangeBits = bdBitOperations::highBitNumber(rangeSize) + 1;
        bdInt ni = 0;
        ok = readBits(&ni, rangeBits);
        if (ok) {
            bdBitOperations::endianSwap(ni, i);
            i += begin;
            i = i > end ? end : (i < begin ? begin : i);
        }
    }
    return ok;
}

bdBool bdBitBuffer::readBits(void* bits, bdUInt numBits)
{
    if (!numBits) return true;
    if (m_readPosition + numBits > m_maxWritePosition) {
        m_failedRead = true;
        return false;
    }
    bdUInt nextByteIndex = m_readPosition >> 3;
    bdUByte8* dest = static_cast<bdUByte8*>(bits);
    while (numBits) {
        if (!m_data.rangeCheck(nextByteIndex)) {
            m_failedRead = true;
            return false;
        }
        const bdUInt numBitsToRead = numBits < 8 ? numBits : 8;
        const bdUByte8 byte0 = m_data[nextByteIndex++];
        const bdUInt downShift = m_readPosition & 7;
        if (downShift + numBitsToRead > 8) {
            if (!m_data.rangeCheck(nextByteIndex)) {
                m_failedRead = true;
                return false;
            }
            const bdUInt upShift = 8 - downShift;
            const bdUByte8 byte1 = m_data[nextByteIndex];
            *dest = ((byte0 >> downShift) | (byte1 << upShift)) & (0xff >> (8 - numBitsToRead));
        } else {
            *dest = (byte0 >> downShift) & (0xff >> (8 - numBitsToRead));
        }
        ++dest;
        m_readPosition += numBitsToRead;
        numBits -= numBitsToRead;
    }
    return true;
}

void bdBitBuffer::setTypeCheck(const bdBool flag) { m_typeChecked = flag; }
bdBool bdBitBuffer::getTypeCheck() const { return m_typeChecked; }

void bdBitBuffer::writeDataType(const bdBitBufferDataType dataType)
{
    if (m_typeChecked) writeRangedUInt32(dataType, BD_BB_NO_TYPE, BD_BB_MAX_TYPE - 1, false);
}

bdBool bdBitBuffer::readDataType(const bdBitBufferDataType expectedDataType)
{
    bdBool ok = true;
    if (m_typeChecked) {
        bdUInt32 dataType32 = BD_BB_NO_TYPE;
        ok = readRangedUInt32(dataType32, BD_BB_NO_TYPE, BD_BB_MAX_TYPE - 1, false);
        if (ok) {
            const bdBitBufferDataType type = static_cast<bdBitBufferDataType>(dataType32);
            if (type != expectedDataType) {
                bdNChar8 string1[40];
                bdNChar8 string2[40];
                typeToString(expectedDataType, string1, sizeof(string1));
                typeToString(type, string2, sizeof(string2));
                bdLogWarn("bdBitBuffer", "Expected data type %s but read %s.", string1, string2);
                ok = false;
            }
        }
    }
    return ok;
}

bdBool bdBitBuffer::append(bdBitBuffer& other)
{
    bdBool ok = m_typeChecked == other.m_typeChecked;
    if (ok) {
        const bdUInt oldReadPos = other.getReadPosition();
        other.resetReadPosition();
        void* const tempData = bdMemory::allocate(other.getDataSize());
        const bdUInt numBits = other.getNumBitsWritten();
        ok = tempData && other.readBits(tempData, numBits);
        if (ok) writeBits(tempData, numBits);
        bdMemory::deallocate(tempData);
        other.setReadPosition(oldReadPos);
    } else {
        const bdNChar8* thisBuffer = m_typeChecked ? "is not type checked" : "is type checked";
        const bdNChar8* otherBuffer = other.m_typeChecked ? "is not type checked" : "is type checked";
        bdLogWarn("bdBitBuffer", "Type check mismatch: this %s, other %s", thisBuffer, otherBuffer);
    }
    return ok;
}

void bdBitBuffer::typeToString(const bdBitBufferDataType type, bdNChar8* const strBuffer,
                               const bdUWord strLength)
{
    // Inferred table name; all 25 original pointers/strings verified at 0x71010de8e8.
    static const bdNChar8* typeStrings[] = {
        "NoType", "Bool", "Char8", "UChar8", "WChar16", "Int16", "UInt16",
        "Int32", "UInt32", "Int64", "UInt64", "RangedInt32", "RangeUInt32",
        "Float32", "Float64", "RangeFloat32", "String", "String",
        "MultiByteString", "Blob", "NaN", "FullType", "Blob", "StructData", "Unknown Type"
    };
    const bdUWord index = type > 24 ? 24 : (type < 0 ? 0 : type);
    bdStrlcpy(strBuffer, typeStrings[index], strLength);
}
