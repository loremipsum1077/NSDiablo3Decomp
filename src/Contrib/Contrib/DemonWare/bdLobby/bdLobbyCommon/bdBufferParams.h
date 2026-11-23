// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdBufferParams.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdSerializable;

class bdBufferParams {  // size 0x28
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdBufferParams);
    bdBufferParams(bdUInt, bdUInt, bdBool);
    virtual ~bdBufferParams();  // vtable slot 0
    void setHint(bdUInt);
    void setMaxBufferSize(bdUInt);
    bdBool allocateBuffer();
    bdByteBufferRef getBuffer() const;
    void addBool(const bdBool & param);
    void addNChar8(const bdNChar8 &);
    void addByte8(const bdByte8 &);
    void addUByte8(const bdUByte8 & param);
    void addInt16(const bdInt16 & param);
    void addUInt16(const bdUInt16 & param);
    void addInt32(const bdInt32 & param);
    void addUInt32(const bdUInt32 & param);
    void addInt64(const bdInt64 & param);
    void addUInt64(const bdUInt64 & param);
    void addFloat32(const bdFloat32 &);
    void addFloat64(const bdFloat64 &);
    void addSerializable(const bdSerializable & param);
    void arrayStart(bdUByte8, bdUInt32, bdUInt32);
    void arrayEnd();
    void addString(const bdNChar8 * str, const bdUWord maxLen);
    void addBlob(const void * data, const bdUInt32 length);
    void addNAN();
    void addNoType();
    bdBool serializedOk() const;
protected:
    virtual bdBool ensureCapacity(bdUInt paramSize, bdBool typeChecked);  // vtable slot 2
    bdUInt m_hint;  // 0x08
    bdUInt m_maxBufferSize;  // 0x0c
    bdBool m_serializeOk;  // 0x10
    bdByteBufferRef m_buffer;  // 0x18
    bdBool m_isWritingArray;  // 0x20
    bdInt32 m_arrayWriteCount;  // 0x24
private:
    bdBufferParams(const bdBufferParams &);
};
static_assert(sizeof(bdBufferParams) == 0x28, "bdBufferParams");


// Functions defined in this file (bodies are not in the DWARF):
//   line    49  void bdBufferParams::operator delete(void * p)  [inlined into 1 functions]

#include "bdBufferParams.inl"
