// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdJSON/bdJSONDeserializer.h

#pragma once

#include "bdJSONValue.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdJSONDeserializer {  // size 0x20
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdJSONDeserializer();
    bdJSONDeserializer(const bdNChar8 *);
    ~bdJSONDeserializer();
    bdBool parse(const bdNChar8 * buffer);
    bdBool getBoolean(const bdNChar8 *const, bdBool &) const;
    bdBool getByte8(const bdNChar8 *const, bdByte8 &) const;
    bdBool getUByte8(const bdNChar8 *const, bdUByte8 &) const;
    bdBool getInt16(const bdNChar8 *const, bdInt16 &) const;
    bdBool getUInt16(const bdNChar8 *const key, bdUInt16 & value) const;
    bdBool getInt32(const bdNChar8 *const key, bdInt32 & value) const;
    bdBool getUInt32(const bdNChar8 *const, bdUInt32 &) const;
    bdBool getInt64(const bdNChar8 *const, bdInt64 &) const;
    bdBool getUInt64(const bdNChar8 *const key, bdUInt64 & value) const;
    bdBool getFloat32(const bdNChar8 *const, bdFloat32 &) const;
    bdBool getFloat64(const bdNChar8 *const, bdFloat64 &) const;
    bdBool getString(const bdNChar8 *const key, bdNChar8 *const value, const bdUInt32 size) const;
    bdBool getNull(const bdNChar8 *const) const;
    bdBool getObject(const bdNChar8 *const, bdJSONDeserializer &) const;
    bdBool getArray(const bdNChar8 *const, bdJSONDeserializer &) const;
    bdBool getBoolean(const bdUInt32, bdBool &) const;
    bdBool getByte8(const bdUInt32, bdByte8 &) const;
    bdBool getUByte8(const bdUInt32, bdUByte8 &) const;
    bdBool getInt16(const bdUInt32, bdInt16 &) const;
    bdBool getUInt16(const bdUInt32, bdUInt16 &) const;
    bdBool getInt32(const bdUInt32, bdInt32 &) const;
    bdBool getUInt32(const bdUInt32, bdUInt32 &) const;
protected:
    template <typename T> bdBool getNumeric(T & value) const;  // DWARF bdJSONDeserializer.cpp:294
    // template instantiation: bdBool getNumeric<unsigned char>(unsigned char &) const;
    // template instantiation: bdBool getNumeric<short>(short &) const;
    // template instantiation: bdBool getNumeric<unsigned short>(unsigned short & value) const;
    // template instantiation: bdBool getNumeric<int>(int &) const;
    // template instantiation: bdBool getNumeric<unsigned int>(unsigned int &) const;
    // template instantiation: bdBool getNumeric<long>(long int &) const;
    // template instantiation: bdBool getNumeric<unsigned long>(long unsigned int &) const;
    // template instantiation: bdBool getNumeric<float>(float &) const;
    // template instantiation: bdBool getNumeric<double>(double &) const;
public:
    bdBool getInt64(const bdUInt32, bdInt64 &) const;
    bdBool getUInt64(const bdUInt32, bdUInt64 &) const;
    bdBool getFloat32(const bdUInt32, bdFloat32 &) const;
    bdBool getFloat64(const bdUInt32, bdFloat64 &) const;
    bdBool getString(const bdUInt32, bdNChar8 *const, const bdUInt32) const;
    bdBool getNull(const bdUInt32) const;
    bdBool getObject(const bdUInt32, bdJSONDeserializer &) const;
    bdBool getArray(const bdUInt32, bdJSONDeserializer &) const;
    bdBool getBoolean(bdBool & value) const;
    bdBool getByte8(bdByte8 & value) const;
    bdBool getUByte8(bdUByte8 & value) const;
    bdBool getInt16(bdInt16 & value) const;
    bdBool getUInt16(bdUInt16 & value) const;
    bdBool getInt32(bdInt32 & value) const;
    bdBool getUInt32(bdUInt32 & value) const;
    bdBool getInt64(bdInt64 & value) const;
    bdBool getUInt64(bdUInt64 & value) const;
    bdBool getFloat32(bdFloat32 & value) const;
    bdBool getFloat64(bdFloat64 & value) const;
    bdBool getString(bdNChar8 *const value, const bdUInt32 size) const;
    bdBool getNull() const;
    bdBool getArray(bdJSONDeserializer & value) const;
    bdBool getObject(bdJSONDeserializer & value) const;
    bdBool isNull() const;
    bdBool isBoolean() const;
    bdBool isNumber() const;
    bdBool isFloatingPoint();
    bdBool isString() const;
    bdBool isObject() const;
    bdBool isArray() const;
    bdBool hasKey(const bdNChar8 * key) const;
    bdBool getFieldByKey(const bdNChar8 *const key, bdJSONDeserializer & value, bdBool logErrors) const;
    bdBool getFieldByIndex(const bdUInt32, bdNChar8 *const, const bdUInt32, bdJSONDeserializer &) const;
    bdBool getElementByIndex(bdUInt32, bdJSONDeserializer &) const;
    bdUInt32 getCount() const;
    bdJSONType getType() const;
    const bdNChar8 * getStart() const;
    const bdNChar8 * getEnd() const;
    bdBool getChild(bdJSONDeserializer &, bdNChar8 *const, const bdUInt32) const;
    bdBool getNext(bdJSONDeserializer & value, bdNChar8 *const key, const bdUInt32 size) const;
    bdBool validated() const;
    static bdBool parse(const bdNChar8 * pos, bdJSONDeserializer & value);
protected:
    bdBool checkStringToNumericValueError(const bdNChar8 * ptr, bdNChar8 * endptr, const bdNChar8 * typeName) const;
    bdJSONType m_type;  // 0x00
    const bdNChar8 * m_ptr;  // 0x08
    const bdNChar8 * m_end;  // 0x10
    bdBool m_isFloatingPoint;  // 0x18
    bdBool m_parsed;  // 0x19
    bdUInt32 m_count;  // 0x1c
    template <typename T> T strToNumber(const bdNChar8 *const ptr, bdNChar8 ** endptr) const;  // DWARF cpp:849..954
    template <typename T> const bdNChar8 * getTypename() const;
    // template instantiation: unsigned char strToNumber<unsigned char>(const bdNChar8 *const ptr, bdNChar8 ** endptr) const;
    // template instantiation: const bdNChar8 * getTypename<unsigned char>() const;
    // template instantiation: short strToNumber<short>(const bdNChar8 *const ptr, bdNChar8 ** endptr) const;
    // template instantiation: const bdNChar8 * getTypename<short>() const;
    // template instantiation: unsigned short strToNumber<unsigned short>(const bdNChar8 *const ptr, bdNChar8 ** endptr) const;
    // template instantiation: const bdNChar8 * getTypename<unsigned short>() const;
    // template instantiation: int strToNumber<int>(const bdNChar8 *const ptr, bdNChar8 ** endptr) const;
    // template instantiation: const bdNChar8 * getTypename<int>() const;
    // template instantiation: unsigned int strToNumber<unsigned int>(const bdNChar8 *const ptr, bdNChar8 ** endptr) const;
    // template instantiation: const bdNChar8 * getTypename<unsigned int>() const;
    // template instantiation: long int strToNumber<long>(const bdNChar8 *const ptr, bdNChar8 ** endptr) const;
    // template instantiation: const bdNChar8 * getTypename<long>() const;
    // template instantiation: long unsigned int strToNumber<unsigned long>(const bdNChar8 *const ptr, bdNChar8 ** endptr) const;
    // template instantiation: const bdNChar8 * getTypename<unsigned long>() const;
    // template instantiation: float strToNumber<float>(const bdNChar8 *const ptr, bdNChar8 ** endptr) const;
    // template instantiation: const bdNChar8 * getTypename<float>() const;
    // template instantiation: double strToNumber<double>(const bdNChar8 *const ptr, bdNChar8 ** endptr) const;
    // template instantiation: const bdNChar8 * getTypename<double>() const;
};
static_assert(sizeof(bdJSONDeserializer) == 0x20, "bdJSONDeserializer");


// Functions defined in this file (bodies are not in the DWARF):
//   line   531  bdUInt32 bdJSONDeserializer::getCount() const  [inline]
//   line   538  bdJSONType bdJSONDeserializer::getType() const  [inline]
//   line   579  bdBool bdJSONDeserializer::validated() const  [inline]
