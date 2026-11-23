// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdByteBuffer.h

#pragma once

#include "bdBitBufferTypes.h"
#include "../bdReference/bdReferencable.h"
#include "../bdReference/bdReference.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdUtilities/bdBitOperations.h"
#include "../bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdString;
class bdByteBuffer;

typedef bdReference<bdByteBuffer> bdByteBufferRef;

class bdByteBuffer : public bdReferencable {  // size 0x30
public:
    static void * operator new(size_t nbytes) { return bdMemory::allocate(nbytes); }
    static void operator delete(void * p) { bdMemory::deallocate(p); }
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdByteBuffer();
    bdByteBuffer(const bdUInt, bdBool);
    bdByteBuffer(void *, const bdUInt, bdBool);
    virtual ~bdByteBuffer();  // vtable slot 0
    void init(const bdUInt size);
    void init(void * bytesToCopy, const bdUInt size);
    virtual void allocateBuffer();  // vtable slot 2
    bdUByte8 & operator[](const bdUInt);
    bdUByte8 operator[](const bdUInt) const;
    bdUInt getSize() const;
    bdUInt getDataSize() const;
    bdUByte8 * getData();
    const bdUByte8 * getData() const;
    bdUInt getMaxWriteSize() const;
    bdUInt getMaxReadSize() const;
    bdBool writeArrayStart(bdUByte8 type, bdUInt32 numElements, bdUInt32 elementSize);
    void writeArrayEnd();
    bdBool readArrayStart(bdUByte8 expectedType, bdUInt32 & numElements);
    void readArrayEnd();
    bdBool writeBool(const bdBool b);
    bdBool writeNChar8(const bdNChar8);
    bdBool writeByte8(const bdByte8);
    bdBool writeUByte8(const bdUByte8 b);
    bdBool writeInt16(const bdInt16);
    bdBool writeUInt16(const bdUInt16 u);
    bdBool writeInt32(const bdInt32 i);
    bdBool writeUInt32(const bdUInt32 u);
    bdBool writeInt64(const bdInt64 i);
    bdBool writeUInt64(const bdUInt64 u);
    bdBool writeFloat32(const bdFloat32);
    bdBool writeFloat64(const bdFloat64);
    bdBool writeString(const bdNChar8 *const s, const bdUWord maxLen);
    bdBool writeString(const bdString & s);
    bdBool writeBlob(const void *const blob, const bdUInt32 length);
    bdBool writeStructStart(const bdUInt32);
    bdBool writeStructEnd();
    bdBool writeNAN();
    bdBool writeNoType();
    // template instantiation: bdBool write<unsigned char>(const unsigned char &);
    // template instantiation: bdBool write<unsigned int>(const unsigned int & var);
    // template instantiation: bdBool write<bool>(const bool &);
    // template instantiation: bdBool write<char>(const char &);
    // template instantiation: bdBool write<signed char>(const signed char &);
    // template instantiation: bdBool write<short>(const short &);
    // template instantiation: bdBool write<unsigned short>(const unsigned short &);
    // template instantiation: bdBool write<int>(const int &);
    // template instantiation: bdBool write<long>(const long int &);
    // template instantiation: bdBool write<unsigned long>(const long unsigned int &);
    // template instantiation: bdBool write<float>(const float &);
    // template instantiation: bdBool write<double>(const double &);
    template <typename T> bdBool write(const T & var)
    {
        T temp;
        bdBitOperations::endianSwap(var, temp);
        return write(&temp, sizeof(temp));
    }
    template <typename T> bdBool read(T & var)
    {
        T temp;
        bdBool ok = read(&temp, sizeof(temp));
        if (ok) bdBitOperations::endianSwap(temp, var);
        return ok;
    }
    bdBool write(const void * data, const bdUInt size);
    // template instantiation: bdBool read<unsigned char>(unsigned char &);
    // template instantiation: bdBool read<unsigned int>(unsigned int &);
    // template instantiation: bdBool read<char>(char &);
    // template instantiation: bdBool read<signed char>(signed char &);
    // template instantiation: bdBool read<short>(short &);
    // template instantiation: bdBool read<unsigned short>(unsigned short &);
    // template instantiation: bdBool read<int>(int &);
    // template instantiation: bdBool read<long>(long int &);
    // template instantiation: bdBool read<unsigned long>(long unsigned int & var);
    // template instantiation: bdBool read<float>(float &);
    // template instantiation: bdBool read<double>(double &);
    bdBool read(void * data, bdUInt size);
    bdBool readBool(bdBool & b);
    bdBool readNChar8(bdNChar8 &);
    bdBool readByte8(bdByte8 &);
    bdBool readUByte8(bdUByte8 & b);
    bdBool readInt16(bdInt16 &);
    bdBool readUInt16(bdUInt16 & u);
    bdBool readInt32(bdInt & i);
    bdBool readUInt32(bdUInt & u);
    bdBool readInt64(bdInt64 & i);
    bdBool readUInt64(bdUInt64 & u);
    bdBool readFloat32(bdFloat32 & f);
    bdBool readFloat64(bdFloat64 &);
    bdBool getStringLength(bdUInt &);
    bdBool readString(bdNChar8 *const s, const bdUWord maxLen);
    bdBool readString(bdString & s);
    bdBool readBlob(bdUByte8 *const blob, bdUInt32 & length);
    bdBool readAndAllocateBlob(bdUByte8 *& blob, bdUInt32 & length);
    bdBool readStructStart(bdUInt32 & dataLen);
    bdBool readStructEnd();
    bdBool readNAN();
    void setTypeCheck(const bdBool);
    bdBitBufferDataType readDataType();
    bdBitBufferDataType inspectDataType() const;
    void reset();
    virtual bdBool expand(bdUInt32 increaseSize);  // vtable slot 3
    virtual void resizeToFit(bdUInt32 newDataSize);  // vtable slot 4
    const bdUByte8 * getReadPtr() const;
protected:
    bdBool writeDataType(const bdBitBufferDataType dataType);
    bdBool readDataType(const bdBitBufferDataType expectedDataType);
    bdBool readAndAllocateCompressedMsg(bdUByte8 *& msg, bdUInt32 & length, bdUInt32 & compressedLength);
    bdBool zlibUncompress(bdUByte8 *& destBuffer, const bdUInt32 destLength, bdUByte8 *& srcBuffer, const bdUInt32 srcLength);
    bdUInt m_size;  // 0x0c
    bdUByte8 * m_data;  // 0x10
    bdUByte8 * m_readPtr;  // 0x18
    bdUByte8 * m_writePtr;  // 0x20
    bdBool m_typeChecked;  // 0x28
    bdBool m_typeCheckedCopy;  // 0x29
private:
    bdBool m_allocatedData;  // 0x2a
};
static_assert(sizeof(bdByteBuffer) == 0x30, "bdByteBuffer");


// Functions defined in this file (bodies are not in the DWARF):
//   line    58  void bdByteBuffer::operator delete(void * p)  [inlined into 1 functions]
//   line    58  void * bdByteBuffer::operator new(size_t nbytes)  [inlined into 5 functions]
//   line   270  bdBool bdByteBuffer::write<unsigned int>(const unsigned int & var)  [inlined into 9 functions]
//   line   285  bdBool bdByteBuffer::read<unsigned long>(long unsigned int & var)  [inlined into 13 functions]

#include "bdByteBuffer.inl"
