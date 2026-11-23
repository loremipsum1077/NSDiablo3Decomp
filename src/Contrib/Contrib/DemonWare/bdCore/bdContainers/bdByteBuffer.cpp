#include "bdByteBuffer.h"
#include "bdBitBuffer.h"
#include "../bdMemory/bdMemory.inl"
#include "../bdString/bdString.h"
#include "../bdUtilities/bdBytePacker.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString-unix.inl"

// zlib.h lives under the private zlib include root; only its C ABI is needed here.
extern "C" int uncompress(unsigned char *, unsigned long *, const unsigned char *, unsigned long);
// Diagnostic text is inferred: this release retains only variadic arguments.

bdByteBuffer::~bdByteBuffer()
{
    if (m_data && m_allocatedData) bdDeallocate(m_data);
    m_readPtr = NULL;
    m_writePtr = NULL;
    m_data = NULL;
}
bdUInt bdByteBuffer::getDataSize() const { return bdUInt(m_writePtr - m_data); }
void bdByteBuffer::allocateBuffer()
{
    if (m_data) { bdLogWarn("bdByteBuffer", "Already allocated"); return; }
    m_data = bdAllocate<bdUByte8>(m_size);
    m_allocatedData = true;
    m_writePtr = m_data;
    m_readPtr = m_data;
}
bdBool bdByteBuffer::write(const void * data, const bdUInt size)
{
    bdBool ok = false;
    if (!m_data) {
        m_size += size;
        ok = true;
    } else {
        bdUInt maxWriteSize = m_size - bdUInt(m_writePtr - m_data);
        if (maxWriteSize >= size) {
            bdUInt temp;
            ok = bdBytePacker::appendBuffer(m_writePtr, maxWriteSize, 0, temp, data, size);
            if (ok) m_writePtr += temp;
        } else bdLogError("bdByteBuffer", "Not enough write space");
    }
    return ok;
}

bdBool bdByteBuffer::writeArrayStart(bdUByte8 type, bdUInt32 numElements, bdUInt32 elementSize)
{
    type += 100;
    m_typeCheckedCopy = m_typeChecked;
    m_typeChecked = false;
    bdBool ok = writeUByte8(type);
    m_typeChecked = true;
    ok = ok && writeUInt32(numElements * elementSize);
    m_typeChecked = false;
    return ok && writeUInt32(numElements);
}
void bdByteBuffer::writeArrayEnd() { m_typeChecked = m_typeCheckedCopy; }
bdBool bdByteBuffer::readArrayStart(bdUByte8 expectedType, bdUInt32 & numElements)
{
    m_typeCheckedCopy = m_typeChecked;
    m_typeChecked = false;
    bdUByte8 type;
    bdBool ok = readUByte8(type);
    if (!ok) {
        bdLogError("bdByteBuffer", "Array type missing");
        numElements = 0;
    } else if (type - 100 != expectedType) {
        bdLogError("bdByteBuffer", "Unexpected array type", expectedType, type);
        ok = false;
    } else {
        bdUByte8 ignore;
        bdUInt32 ignoredArrayLength;
        m_typeChecked = false;
        ok = readUByte8(ignore);
        ok = ok && readUInt32(ignoredArrayLength);
        ok = ok && readUInt32(numElements);
    }
    return ok;
}
void bdByteBuffer::readArrayEnd() { m_typeChecked = m_typeCheckedCopy; }
bdBool bdByteBuffer::writeBool(const bdBool b)
{
    bdBool ok = writeDataType(BD_BB_BOOL_TYPE);
    return ok && write(b);
}
bdBool bdByteBuffer::writeUByte8(const bdUByte8 b)
{
    bdBool ok = writeDataType(BD_BB_UNSIGNED_CHAR8_TYPE);
    return ok && write(b);
}
bdBool bdByteBuffer::writeUInt16(const bdUInt16 u)
{
    bdBool ok = writeDataType(BD_BB_UNSIGNED_INTEGER16_TYPE);
    return ok && write(u);
}
bdBool bdByteBuffer::writeInt32(const bdInt32 i)
{
    if (i == 0x7fffffff) return writeNAN();
    bdBool ok = writeDataType(BD_BB_SIGNED_INTEGER32_TYPE);
    return ok && write(i);
}
bdBool bdByteBuffer::writeUInt32(const bdUInt32 u)
{
    if (u == 0xffffffff) return writeNAN();
    bdBool ok = writeDataType(BD_BB_UNSIGNED_INTEGER32_TYPE);
    return ok && write(u);
}
inline bdBool bdByteBuffer::writeNAN()
{
    if (!m_typeChecked) { bdLogError("bdByteBuffer", "NAN needs type checking"); return false; }
    return writeDataType(BD_BB_NAN_TYPE);
}
bdBool bdByteBuffer::writeNoType()
{
    if (!m_typeChecked) { bdLogError("bdByteBuffer", "No type needs type checking"); return false; }
    return writeDataType(BD_BB_NO_TYPE);
}
bdBool bdByteBuffer::writeInt64(const bdInt64 i)
{
    if (i == 0x7fffffffffffffffL) return writeNAN();
    bdBool ok = writeDataType(BD_BB_SIGNED_INTEGER64_TYPE);
    return ok && write(i);
}
bdBool bdByteBuffer::writeUInt64(const bdUInt64 u)
{
    if (u == 0xffffffffffffffffUL) return writeNAN();
    bdBool ok = writeDataType(BD_BB_UNSIGNED_INTEGER64_TYPE);
    return ok && write(u);
}
bdBool bdByteBuffer::writeString(const bdNChar8 *const s, const bdUWord maxLen)
{
    bdBool ok = false;
    if (!s) ok = writeNAN();
    else {
        bdUInt lengthToWrite;
        bdBool addNullTerminator = false;
        ok = writeDataType(BD_BB_SIGNED_CHAR8_STRING_TYPE);
        bdUWord strLength = bdStrnlen(s, maxLen);
        if (strLength < maxLen) lengthToWrite = strLength + 1;
        else {
            lengthToWrite = maxLen - 1;
            addNullTerminator = true;
            bdLogError("bdByteBuffer", "String truncated");
        }
        ok = ok && write(s, lengthToWrite);
        if (addNullTerminator) {
            bdUByte8 null = 0;
            ok = ok && write(null);
        }
    }
    return ok;
}
bdBool bdByteBuffer::writeString(const bdString & s) { return writeString(s.getBuffer(), s.getLength() + 1); }
bdBool bdByteBuffer::writeBlob(const void *const blob, const bdUInt32 length)
{
    bdBool ok = writeDataType(BD_BB_BLOB_TYPE);
    ok = ok && writeUInt32(length);
    return ok && write(blob, length);
}
bdBool bdByteBuffer::read(void * data, bdUInt size)
{
    bdBool ok = false;
    bdUInt maxReadSize = m_size - bdUInt(m_readPtr - m_data);
    if (size <= maxReadSize) {
        bdUInt temp;
        ok = bdBytePacker::removeBuffer(m_readPtr, maxReadSize, 0, temp, data, size);
        m_readPtr += size;
    } else bdLogError("bdByteBuffer", "Not enough read space");
    return ok;
}
bdBool bdByteBuffer::readBool(bdBool & b)
{
    bdBool ok = readDataType(BD_BB_BOOL_TYPE);
    if (ok) {
        bdUByte8 byte = 0;
        ok = read(&byte, sizeof(byte));
        if (ok) b = byte != 0;
    }
    return ok;
}
bdBool bdByteBuffer::readUByte8(bdUByte8 & b)
{
    bdBool ok = readDataType(BD_BB_UNSIGNED_CHAR8_TYPE);
    return ok && read(b);
}
bdBool bdByteBuffer::readUInt16(bdUInt16 & u)
{
    bdBool ok = readDataType(BD_BB_UNSIGNED_INTEGER16_TYPE);
    return ok && read(u);
}
bdBool bdByteBuffer::readInt32(bdInt & i)
{
    bdBool ok = readDataType(BD_BB_SIGNED_INTEGER32_TYPE);
    return ok && read(i);
}
bdBool bdByteBuffer::readUInt32(bdUInt & u)
{
    bdBool ok = readDataType(BD_BB_UNSIGNED_INTEGER32_TYPE);
    return ok && read(u);
}
bdBool bdByteBuffer::readInt64(bdInt64 & i)
{
    bdBool ok = readDataType(BD_BB_SIGNED_INTEGER64_TYPE);
    return ok && read(i);
}
bdBool bdByteBuffer::readUInt64(bdUInt64 & u)
{
    bdBool ok = readDataType(BD_BB_UNSIGNED_INTEGER64_TYPE);
    return ok && read(u);
}
bdBool bdByteBuffer::readFloat32(bdFloat32 & f)
{
    bdBool ok = readDataType(BD_BB_FLOAT32_TYPE);
    return ok && read(f);
}
bdBool bdByteBuffer::readString(bdNChar8 *const s, const bdUWord maxLen)
{
    bdBool ok = readDataType(BD_BB_SIGNED_CHAR8_STRING_TYPE);
    // Consume through the terminator even when the destination is truncated.
    if (s && ok) {
        bdUInt i = 0;
        bdNChar8 c;
        do {
            c = 0;
            ok = read(&c, sizeof(c));
            if (i < maxLen) s[i] = c;
            i += ok && c != 0;
        } while (ok && c != 0);
        if (maxLen) s[maxLen - 1] = 0;
    }
    return ok;
}
bdBool bdByteBuffer::readString(bdString & s)
{
    bdBool ok = readDataType(BD_BB_SIGNED_CHAR8_STRING_TYPE);
    if (ok) {
        // Original assigns before checking the remaining buffer length.
        s = reinterpret_cast<const bdNChar8 *>(m_readPtr);
        const bdUInt maxReadSize = m_size - bdUInt(m_readPtr - m_data);
        ok = s.getLength() < maxReadSize;
        if (ok) m_readPtr += s.getLength() + 1;
    }
    return ok;
}
bdBool bdByteBuffer::readBlob(bdUByte8 *const blob, bdUInt32 & length)
{
    bdBool ok = readDataType(BD_BB_BLOB_TYPE);
    if (ok) {
        bdUInt32 tempLength = 0;
        ok = readUInt32(tempLength);
        if (ok && blob) {
            // Consume only the copied prefix, but report the full encoded length below.
            ok = read(blob, length < tempLength ? length : tempLength);
            if (tempLength > length) bdLogWarn("bdByteBuffer", "Blob truncated: %u %u", tempLength, length);
        }
        length = tempLength;
    }
    return ok;
}
bdBool bdByteBuffer::readAndAllocateBlob(bdUByte8 *& blob, bdUInt32 & length)
{
    bdUInt32 tmpLength = 0;
    bdUByte8 * tmpBlob = NULL;
    bdBool ok = readDataType(BD_BB_BLOB_TYPE);
    ok = ok && readUInt32(tmpLength);
    if (ok) {
        tmpBlob = bdAllocate<bdUByte8>(tmpLength);
        // Original treats null allocation as success without consuming payload bytes.
        if (tmpBlob) ok = read(tmpBlob, tmpLength);
    }
    if (ok) { blob = tmpBlob; length = tmpLength; }
    else bdDeallocate(tmpBlob);
    return ok;
}
bdBool bdByteBuffer::readStructStart(bdUInt32 & dataLen)
{
    bdBool ok = readDataType(BD_BB_STRUCTURED_DATA_TYPE);
    if (ok) {
        bdUInt32 structDataLength;
        ok = readUInt32(structDataLength);
        if (ok) dataLen = structDataLength;
    }
    return ok;
}
bdBool bdByteBuffer::readStructEnd() { return true; }
// NON_MATCHING: size matches (572); only this/tmpCompressedMsg swap x22/x23. Re-laid on DWARF lines 674-782 (locals, rows and
// columns now match the original; `ok && ptr` read from the && at column 9); `if (ok) ok = readUInt32` and dropping `inline` tried, no gain.
#line 674
bdBool bdByteBuffer::readAndAllocateCompressedMsg(bdUByte8 *& msg, bdUInt32 & length, bdUInt32 & compressedLength)
{
	// Inferred lines (no rows of their own): if (ok) at 700, case at 715, zlibUncompress switch body 762-770, return at 781.
	// Locals start at 0 / NULL: their DWARF location lists hold constant 0 before each read.
	bdUByte8 * tmpCompressedMsg = NULL;
	bdUByte8 * tmpMsg = NULL;
	bdUInt32 tmpLength = 0;
	bdUInt32 tmpCompressedLength = 0;


	bdUInt32 compressionType = 0;
	bdBool ok = readUInt32(compressionType);




	ok = ok && readUInt32(tmpLength);





	ok = ok && readUInt32(tmpCompressedLength);



	if (ok)
	{
		tmpCompressedMsg = bdAllocate<bdUByte8>(tmpCompressedLength);
		tmpMsg = bdAllocate<bdUByte8>(tmpLength);
	}

	if (ok && tmpCompressedMsg)
	{
		ok = read(tmpCompressedMsg, tmpCompressedLength);
	}

	if (ok && tmpMsg)
	{
		switch (compressionType)
		{
		case 50:




			ok = zlibUncompress(tmpMsg, tmpLength, tmpCompressedMsg, tmpCompressedLength);
			break;
		default:
			bdLogError("bdByteBuffer", "Unknown compression method");
			ok = false;
			break;
		}
	}

	if (ok)
	{
		msg = tmpMsg;
		length = tmpLength;
		compressedLength = tmpCompressedLength;
	}
	else
	{
		bdDeallocate(tmpMsg);
	}


	bdDeallocate(tmpCompressedMsg);

	return ok;
}


inline bdBool bdByteBuffer::zlibUncompress(bdUByte8 *& destBuffer,
	const bdUInt32 destLength,
	bdUByte8 *& srcBuffer,
	const bdUInt32 srcLength)
{

	bdULong msgLength = destLength;




	int result = uncompress(destBuffer, &msgLength, srcBuffer, srcLength);
	bdBool ok = result == 0;  // DWARF local ok (759); keeps result live past the switch
	// Jump table covers -5 through 0; release entries call bdUseVAArgs with no retained arguments. Text and case lines inferred.
	switch (result)
	{
	case -5: bdLogError("bdByteBuffer", "Buffer error"); break;
	case -4: bdLogError("bdByteBuffer", "Memory error"); break;
	case -3: bdLogError("bdByteBuffer", "Data error"); break;
	case -2: bdLogError("bdByteBuffer", "Stream error"); break;
	case -1: bdLogError("bdByteBuffer", "IO error"); break;
	case 0: bdLogInfo("bdByteBuffer", "Decompressed payload"); break;
	default: bdLogError("bdByteBuffer", "Unknown error"); break;
	}










	return ok;
}
#line 331
inline bdBool bdByteBuffer::writeDataType(const bdBitBufferDataType dataType)
{
    bdUByte8 dataType8 = dataType;
    bdBool ok = true;
    if (m_typeChecked) ok = write(&dataType8, sizeof(dataType8));
    return ok;
}
bdBool bdByteBuffer::readDataType(const bdBitBufferDataType expectedDataType)
{
    bdBool ok = true;
    if (m_typeChecked) {
        bdUByte8 dataType8;
        bdUByte8 * oldReadPtr = m_readPtr;
        dataType8 = 0;
        ok = read(&dataType8, sizeof(dataType8));
        if (ok) {
            const bdBitBufferDataType type = static_cast<bdBitBufferDataType>(dataType8);
            if (type == BD_COMPRESSED_BB_PAYLOAD_TYPE) {
                bdUByte8 * msg = NULL;
                bdUInt32 msgLength = 0;
                bdUInt32 compressedMsgLength = 0;
                bdBool hasReadCompressedMsg = readAndAllocateCompressedMsg(msg, msgLength, compressedMsgLength);
                (void)hasReadCompressedMsg; // Original continues reconstruction even on decode failure.
                bdUInt32 bufferHeaderOffset = oldReadPtr - m_data;
                bdUInt32 bufferFooterOffset = m_readPtr - m_data;
                bdUInt32 bufferFooterSize = m_size - bufferFooterOffset;
                bdUByte8 * oldData = bdAllocate<bdUByte8>(m_size);
                bdMemcpy(oldData, m_data, m_size);
                bdUInt32 newDataSize = m_size - compressedMsgLength + msgLength;
                resizeToFit(newDataSize);
                bdMemcpy(m_data, oldData, bufferHeaderOffset);
                bdMemcpy(m_data + bufferHeaderOffset, msg, msgLength);
                bdMemcpy(m_data + bufferHeaderOffset + msgLength, oldData + bufferFooterOffset, bufferFooterSize);
                bdDeallocate(oldData);
                bdDeallocate(msg);
                m_readPtr = m_data + bufferHeaderOffset;
                return readDataType(expectedDataType);
            } else {
                ok = type == expectedDataType;
                if (!ok) {
                    bdNChar8 string1[40];
                    bdNChar8 string2[40];
                    bdBitBuffer::typeToString(expectedDataType, string1, sizeof(string1));
                    bdBitBuffer::typeToString(type, string2, sizeof(string2));
                    bdLogError("bdByteBuffer", "Expected %s received %s", string1, string2);
                }
            }
        }
    }
    return ok;
}
void bdByteBuffer::resizeToFit(bdUInt32 newDataSize)
{
    // Original always frees the old pointer and leaves m_allocatedData unchanged.
    bdDeallocate(m_data);
    m_data = bdAllocate<bdUByte8>(newDataSize);
    m_readPtr = m_data;
    m_writePtr = m_data;
    m_size = newDataSize;
}
bdBool bdByteBuffer::expand(bdUInt32 increaseSize)
{
    bdBool expandOk = false;
    if (m_data) {
        bdUInt oldSize = m_size;
        bdUByte8 * oldData = m_data;
        bdUByte8 * oldReadPtr = m_readPtr;
        bdUByte8 * oldWritePtr = m_writePtr;
        if (oldReadPtr >= oldData && oldWritePtr >= oldData) {
            bdUInt64 writeOffset = oldWritePtr - oldData;
            bdUInt64 readOffset = oldReadPtr - oldData;
            m_size += increaseSize;
            m_data = NULL;
            m_allocatedData = false;
            allocateBuffer();
            if (m_data) {
                bdMemcpy(m_data, oldData, oldSize);
                m_readPtr = m_data + readOffset;
                m_writePtr = m_data + writeOffset;
                bdDeallocate(oldData);
                expandOk = true;
            } else {
                m_data = oldData;
                m_size = oldSize;
                m_readPtr = oldReadPtr;
                m_writePtr = oldWritePtr;
                // Original rollback assumes ownership, regardless of the former flag.
                m_allocatedData = true;
                bdLogError("bdByteBuffer", "Allocation failed %u", oldSize + increaseSize);
            }
        }
    } else {
        bdLogWarn("bdByteBuffer", "Allocate %u", increaseSize);
        m_size = increaseSize;
        allocateBuffer();
        expandOk = m_data != NULL;
        if (!expandOk) bdLogError("bdByteBuffer", "Allocation failed %u", m_size);
    }
    return expandOk;
}
const bdUByte8 * bdByteBuffer::getReadPtr() const { return m_readPtr; }
