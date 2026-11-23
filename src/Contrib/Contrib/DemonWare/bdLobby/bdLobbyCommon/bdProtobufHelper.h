// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdProtobufHelper.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdStructSerializationInputStream;
class bdStructSerializationOutputStream;

class bdProtobufHelper {  // size 0x1
public:
    enum eWireType {
        WIRETYPE_INVALID = -1,
        WIRETYPE_VARINT = 0,
        WIRETYPE_64BIT = 1,
        WIRETYPE_STRING = 2,
        WIRETYPE_32BIT = 5,
    };
public:
    static bdBool encode32bits(bdStructSerializationOutputStream &, bdFloat32);
    static bdBool encode64bits(bdStructSerializationOutputStream &, bdFloat64);
    static bdBool encodeVarInt(bdStructSerializationOutputStream &, bdInt64);
    static bdBool encodeVarInt(bdStructSerializationOutputStream & stream, bdUInt64 value);
    static bdBool encodeString(bdStructSerializationOutputStream &, const void *, bdUInt32);
    static bdBool encodeTag(bdStructSerializationOutputStream &, bdUInt32, bdProtobufHelper::eWireType);
    static bdBool decode32bits(bdStructSerializationInputStream &, bdFloat32 &);
    static bdBool decode64bits(bdStructSerializationInputStream &, bdFloat64 &);
    static bdBool decodeVarInt(bdStructSerializationInputStream &, bdInt64 &);
    static bdBool decodeVarInt(bdStructSerializationInputStream & stream, bdUInt64 & value);
    static bdBool decodeString(bdStructSerializationInputStream & stream, void * value, bdUInt32 & len);
    static bdBool decodeTag(bdStructSerializationInputStream & stream, bdUInt32 & tagId, bdProtobufHelper::eWireType & wireType);
    static bdBool skip(bdStructSerializationInputStream & stream, bdProtobufHelper::eWireType wireType);
    static bdBool skipBytes(bdStructSerializationInputStream & stream, bdUInt64 byteCount);
};
static_assert(sizeof(bdProtobufHelper) == 0x1, "bdProtobufHelper");

