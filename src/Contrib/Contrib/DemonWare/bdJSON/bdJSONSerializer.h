// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdJSON/bdJSONSerializer.h

#pragma once

#include "bdJSONValue.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdJSONSerializer {  // size 0x118
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdJSONSerializer();
    bdJSONSerializer(bdNChar8 *, bdUInt32);
    ~bdJSONSerializer();
    void setBuffer(bdNChar8 *, bdUInt32);
    bdBool validate() const;
    bdUInt32 length();
    bdBool writeBeginObject();
    bdBool writeEndObject();
    bdBool writeBeginObject(const bdNChar8 *);
    bdBool writeBeginArray(const bdNChar8 *);
    bdBool writeEndArray();
    bdBool writeBoolean(const bdNChar8 * key, const bdBool value);
    bdBool writeString(const bdNChar8 * key, const bdNChar8 * value);
    bdBool writeByte8(const bdNChar8 *, const bdByte8, bdBool);
    bdBool writeUByte8(const bdNChar8 *, const bdUByte8, bdBool);
    bdBool writeInt16(const bdNChar8 *, const bdInt16, bdBool);
    bdBool writeUInt16(const bdNChar8 *, const bdUInt16, bdBool);
    bdBool writeInt32(const bdNChar8 *, const bdInt32, bdBool);
    bdBool writeUInt32(const bdNChar8 *, const bdUInt32, bdBool);
    bdBool writeUInt64(const bdNChar8 * key, const bdUInt64 value, bdBool asString);
    bdBool writeInt64(const bdNChar8 *, const bdInt64, bdBool);
    bdBool writeFloat32(const bdNChar8 *, const bdFloat32, bdBool);
    bdBool writeFloat64(const bdNChar8 *, const bdFloat64, bdBool);
    bdBool writeNull(const bdNChar8 *);
    bdBool writeBoolean(const bdBool);
    bdBool writeString(const bdNChar8 *);
    bdBool writeByte8(const bdByte8, bdBool);
    bdBool writeUByte8(const bdUByte8, bdBool);
    bdBool writeInt16(const bdInt16, bdBool);
    bdBool writeUInt16(const bdUInt16, bdBool);
    bdBool writeInt32(const bdInt32, bdBool);
    bdBool writeUInt32(const bdUInt32, bdBool);
    bdBool writeInt64(const bdInt64, bdBool);
    bdBool writeUInt64(const bdUInt64, bdBool);
    bdBool writeFloat32(const bdFloat32, bdBool);
    bdBool writeFloat64(const bdFloat64, bdBool);
    bdBool writeNull();
protected:
    bdNChar8 * m_buffer;  // 0x00
    bdUInt32 m_bufferSize;  // 0x08
    bdUInt32 m_bufferIndex;  // 0x0c
    struct bdJSONTypeFirst {  // size 0x8
        bdJSONType m_type;  // 0x00
        bdBool m_first;  // 0x04
    };
    bdJSONSerializer::bdJSONTypeFirst m_stack[32];  // 0x10
    bdUInt32 m_stackDepth;  // 0x110
    bdBool checkBuffer(bdUInt32 size) const;
    bdBool checkObject() const;
    bdBool checkArray() const;
    bdBool checkRoot() const;
    void writeObjectKey(const bdNChar8 * key);
    bdUInt32 getSeparatorSize(bdBool inArrayOrObject);
    void writeKeySeparator();
    void writeArraySeparator();
    bdBool writeValue(const bdNChar8 * value, bdBool isArray, bdBool asString);
    bdBool writeObjectKeyValue(const bdNChar8 * key, const bdNChar8 * value, bdBool asString);
    // template instantiation: bdBool writeNumeric<const signed char>(const signed char, bdBool);
    // template instantiation: bdBool writeNumeric<const unsigned char>(const unsigned char, bdBool);
    // template instantiation: bdBool writeNumeric<const short>(const short, bdBool);
    // template instantiation: bdBool writeNumeric<const unsigned short>(const unsigned short, bdBool);
    // template instantiation: bdBool writeNumeric<const unsigned int>(const unsigned int, bdBool);
    // template instantiation: bdBool writeNumeric<const int>(const int, bdBool);
    // template instantiation: bdBool writeNumeric<const long>(const long int, bdBool);
    // template instantiation: bdBool writeNumeric<const unsigned long>(const long unsigned int, bdBool);
    // template instantiation: bdBool writeNumeric<const float>(const float, bdBool);
    // template instantiation: bdBool writeNumeric<const double>(const double, bdBool);
    // template instantiation: bdBool writeNumeric<const signed char>(const bdNChar8 *, const signed char, bdBool);
    // template instantiation: bdBool writeNumeric<const unsigned char>(const bdNChar8 *, const unsigned char, bdBool);
    // template instantiation: bdBool writeNumeric<const short>(const bdNChar8 *, const short, bdBool);
    // template instantiation: bdBool writeNumeric<const unsigned short>(const bdNChar8 *, const unsigned short, bdBool);
    // template instantiation: bdBool writeNumeric<const unsigned int>(const bdNChar8 *, const unsigned int, bdBool);
    // template instantiation: bdBool writeNumeric<const int>(const bdNChar8 *, const int, bdBool);
    // template instantiation: bdBool writeNumeric<const long>(const bdNChar8 *, const long int, bdBool);
    // template instantiation: bdBool writeNumeric<const unsigned long>(const bdNChar8 * key, const long unsigned int value, bdBool asString);
    // template instantiation: bdBool writeNumeric<const float>(const bdNChar8 *, const float, bdBool);
    // template instantiation: bdBool writeNumeric<const double>(const bdNChar8 *, const double, bdBool);
    // template instantiation: const bdNChar8 * getFormat<const signed char>() const;
    // template instantiation: const bdNChar8 * getFormat<const unsigned char>() const;
    // template instantiation: const bdNChar8 * getFormat<const short>() const;
    // template instantiation: const bdNChar8 * getFormat<const unsigned short>() const;
    // template instantiation: const bdNChar8 * getFormat<const int>() const;
    // template instantiation: const bdNChar8 * getFormat<const unsigned int>() const;
    // template instantiation: const bdNChar8 * getFormat<const long>() const;
    // template instantiation: const bdNChar8 * getFormat<const unsigned long>() const;
    // template instantiation: const bdNChar8 * getFormat<const float>() const;
    // template instantiation: const bdNChar8 * getFormat<const double>() const;
    // Member templates defined in bdJSONSerializer.cpp (writeNumeric at line 490,
    // getFormat specializations from line 715).
    template <typename T>
    bdBool writeNumeric(const bdNChar8 * key, T value, bdBool asString);
    template <typename T>
    const bdNChar8 * getFormat() const;
};
static_assert(sizeof(bdJSONSerializer) == 0x118, "bdJSONSerializer");

