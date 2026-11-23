// Adopted from the DWARF; inline bodies reconstructed from callers.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdBitBuffer.h

#pragma once

#include "bdBitBufferTypes.h"
#include "bdFastArray.h"
#include "../bdReference/bdReferencable.h"
#include "../bdReference/bdReference.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdString;

class bdBitBuffer;
typedef bdReference<bdBitBuffer> bdBitBufferRef;

class bdBitBuffer : public bdReferencable {  // size 0x30
public:
    BD_CLASS_ALLOCATOR(bdBitBuffer);
    bdBitBuffer(const bdUInt, const bdBool);
    bdBitBuffer(const bdUByte8 *, const bdUInt, const bdBool);
    virtual ~bdBitBuffer();  // vtable slot 0
    bdUInt getDataSize() const;
    bdUInt getNumBitsWritten() const;
    const bdUByte8 * getData() const;
    bdUByte8 * getData();
    bdBool failedRead() const;
    bdUInt getWritePosition() const;
    bdUInt getReadPosition() const;
    void setReadPosition(const bdUInt bitPosition);
    void resetReadPosition();
    void setWritePosition(const bdUInt);
    void resetWritePosition();
    bdBool writeBool(const bdBool b);
    void writeNChar8(const bdNChar8);
    void writeByte8(const bdByte8 b);
    void writeUByte8(const bdUByte8 b);
    void writeInt16(const bdInt16);
    void writeUInt16(const bdUInt16);
    void writeInt32(const bdInt32 i);
    void writeUInt32(const bdUInt32 u);
    void writeInt64(const bdInt64 i);
    void writeUInt64(const bdUInt64 u);
    void writeRangedUInt32(const bdUInt u, const bdUInt begin, const bdUInt end, const bdBool typeChecked);
    void writeRangedInt32(const bdInt i, const bdInt begin, const bdInt end);
    void writeFloat32(const bdFloat32 f);
    void writeFloat64(const bdFloat64 &);
    void writeRangedFloat32(const bdFloat32, const bdFloat32, const bdFloat32, const bdFloat32);
    void writeString(const bdNChar8 *const s, const bdUWord maxLen);
    void writeString(const bdString & s);
    void writeBlob(const void *const, const bdUInt32);
    void writeBits(const void * bits, const bdUInt numBits);
    // DWARF header lines 273 and 498. readFull consumes the tag even if it differs.
    template<typename T> void writeFull(const T& data) {
        writeDataType(BD_BB_FULL_TYPE);
        writeBits(&data, sizeof(T) * 8);
    }
    template<typename T> bdBool readFull(T& data) {
        readDataType(BD_BB_FULL_TYPE);
        return readBits(&data, sizeof(T) * 8);
    }
    bdBool readBool(bdBool & b);
    bdBool testBool();
    bdBool readNChar8(bdNChar8 &);
    bdBool readByte8(bdByte8 & b);
    bdBool readUByte8(bdUByte8 & b);
    bdBool readInt16(bdInt16 &);
    bdBool readUInt16(bdUInt16 &);
    bdBool readInt32(bdInt & i);
    bdBool readUInt32(bdUInt & u);
    bdBool readInt64(bdInt64 & i);
    bdBool readUInt64(bdUInt64 & u);
    bdBool readRangedUInt32(bdUInt & u, const bdUInt begin, const bdUInt end, const bdBool typeChecked);
    bdBool readRangedInt32(bdInt & i, const bdInt begin, const bdInt end);
    bdBool readFloat32(bdFloat32 &);
    bdBool readFloat64(bdFloat64 &);
    bdBool readRangedFloat32(bdFloat32 &, const bdFloat32, const bdFloat32, const bdFloat32);
    bdBool getStringLength(bdUWord &);
    bdBool readString(bdNChar8 *const s, const bdUWord maxLen);
    bdBool readString(bdNChar8 *const s, const bdUWord maxLen, bdUWord & length);
    bdBool readString(bdString &);
    bdBool getBlobLength(bdUInt32 &);
    bdBool readBlob(bdUByte8 *const, bdUInt32 &);
    bdBool readBits(void * bits, bdUInt numBits);
    bdBitBufferDataType readDataType();
    void setTypeCheck(const bdBool flag);
    bdBool getTypeCheck() const;
    static void typeToString(const bdBitBufferDataType type, bdNChar8 *const strBuffer, const bdUWord strLength);
    bdBool append(bdBitBuffer & other);
protected:
    void writeDataType(const bdBitBufferDataType dataType);
    bdBool readDataType(const bdBitBufferDataType expectedDataType);
    bdFastArray<unsigned char> m_data;  // 0x10
    bdUInt m_writePosition;  // 0x20
    bdUInt m_maxWritePosition;  // 0x24
    bdUInt m_readPosition;  // 0x28
    bdBool m_failedRead;  // 0x2c
    bdBool m_typeChecked;  // 0x2d
};
static_assert(sizeof(bdBitBuffer) == 0x30, "bdBitBuffer");



#include "bdBitBuffer.inl"
