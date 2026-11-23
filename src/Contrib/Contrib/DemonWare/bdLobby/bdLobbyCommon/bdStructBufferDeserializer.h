// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdStructBufferDeserializer.h

#pragma once

#include "bdStructBufferCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdStructBufferSerializable;
template <typename T> class bdFastArray;
template <typename T> class bdArray;
template <bdUWord MaxLength> class bdStructFixedSizeString;

class bdStructBufferDeserializer {  // size 0x20
public:
    template <typename T> bdBool readObjectArray(const bdUInt32 tag, bdArray<T> & objects);
    // template instantiation: bdBool readObjectArray<bdTencentClientDecidedReward>(const bdUInt32, bdFastArray<bdTencentClientDecidedReward> &);
    // template instantiation: bdBool readObjectArray<bdObjectStoreHTTPHeader, 20>(const bdUInt32, bdStructFixedSizeArray<bdObjectStoreHTTPHeader, 20> &);
    enum bdReadResult {
        BD_READ_SUCCESS = 0,
        BD_READ_SUCCESS_TAG_NOT_FOUND = 1,
        BD_READ_FAILED_STREAM_ERROR = 2,
        BD_READ_FAILED_DATA_TYPE = 3,
        BD_READ_FAILED_DATA_INTEGRITY = 4,
    };
private:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
public:
    bdStructBufferDeserializer(bdByteBufferRef, bdUInt32, bdUInt32);
    ~bdStructBufferDeserializer();
    bdUInt32 getRemainingReadSize();
    bdBool peekCurrentTag(bdUInt32 &);
    template <bdUWord MaxLength> bdBool readString(const bdUInt32 tag, bdStructFixedSizeString<MaxLength> & string);
    bdBool skipCurrentTag();
    bdStructBufferDeserializer::bdReadResult getLastReadResult();
    bdBool readBool(const bdUInt32, bdBool &);
    bdBool readByte8(const bdUInt32, bdByte8 &);
    bdBool readUByte8(const bdUInt32, bdUByte8 &);
    bdBool readInt16(const bdUInt32, bdInt16 &);
    bdBool readUInt16(const bdUInt32 tag, bdUInt16 & value);
    bdBool readInt32(const bdUInt32, bdInt32 &);
    bdBool readUInt32(const bdUInt32 tag, bdUInt32 & value);
    bdBool readInt64(const bdUInt32, bdInt64 &);
    bdBool readUInt64(const bdUInt32 tag, bdUInt64 & value);
    bdBool readFloat32(const bdUInt32, bdFloat32 &);
    bdBool readFloat64(const bdUInt32, bdFloat64 &);
    bdBool readString(const bdUInt32 tag, bdNChar8 *const value, const bdUInt32 size);
    bdBool readBlob(const bdUInt32 tag, void * value, const bdUInt32 maxSize, bdUInt32 & readSize);
    bdBool readObject(const bdUInt32 tag, bdStructBufferSerializable & object);
    bdBool readUInt64Array(const bdUInt32, bdFastArray<unsigned long> &);
    bdBool readUInt16Array(const bdUInt32, bdFastArray<unsigned short> &);
protected:
    bdStructBufferDeserializer::bdReadResult read(const bdUInt32, bdInt64 &);
    bdStructBufferDeserializer::bdReadResult read(const bdUInt32 tag, bdUInt64 & value);
    bdBool readNextTag();
    bdStructBufferDeserializer::bdReadResult readUntilTag(bdUInt32 tag, bdUInt32 tagType);
    bdBool prepare(bdByteBufferRef buffer, bdUInt32 available, bdUInt32 nestedDepth);
    bdBool setLastReadResult(bdStructBufferDeserializer::bdReadResult result);
    bdUInt32 m_nextTag;  // 0x00
    bdUInt32 m_nextType;  // 0x04
    bdUInt32 m_nestedDepth;  // 0x08
    bdStructBufferDeserializer::bdReadResult m_lastReadResult;  // 0x0c
    bdStructSerializationInputStream m_stream;  // 0x10
};
static_assert(sizeof(bdStructBufferDeserializer) == 0x20, "bdStructBufferDeserializer");

#include "bdStructBufferDeserializer.inl"

