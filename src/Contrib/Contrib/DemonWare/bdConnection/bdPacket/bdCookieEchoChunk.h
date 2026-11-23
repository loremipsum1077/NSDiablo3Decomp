// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdCookieEchoChunk.h

#pragma once
#include "bdCookie.h"

#include "bdChunk.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdInitChunk;
typedef bdReference<bdInitChunk> bdInitChunkRef;

class bdCookie;
typedef bdReference<bdCookie> bdCookieRef;

class bdCookieEchoChunk;
typedef bdReference<bdCookieEchoChunk> bdCookieEchoChunkRef;

class bdCookieEchoChunk : public bdChunk {  // size 0x28
public:
    BD_CLASS_ALLOCATOR(bdCookieEchoChunk);
    enum bdCookieEchoFlags {
        BD_CE_NO_FLAGS = 0,
    };
    bdCookieEchoChunk();
    bdCookieEchoChunk(bdByteBufferRef);
    virtual ~bdCookieEchoChunk();  // vtable slot 0
    bdBool getCookie(bdCookieRef & cookie);
    bdCookieEchoChunk::bdCookieEchoFlags getFlags() const;
protected:
    virtual bdUInt serialize(bdUByte8 * data, const bdUInt size) const;  // vtable slot 3
    virtual bdBool deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt & offset);  // vtable slot 4
    bdCookieEchoChunk::bdCookieEchoFlags m_flags;  // 0x14
    bdCookieRef m_cookie;  // 0x18
    bdByteBufferRef m_cookieBuffer;  // 0x20
};
static_assert(sizeof(bdCookieEchoChunk) == 0x28, "bdCookieEchoChunk");


// Functions defined in this file (bodies are not in the DWARF):
//   line    36  void bdCookieEchoChunk::operator delete(void * p)  [inlined into 1 functions]
//   line    36  void * bdCookieEchoChunk::operator new(size_t nbytes)  [inlined into 2 functions]
