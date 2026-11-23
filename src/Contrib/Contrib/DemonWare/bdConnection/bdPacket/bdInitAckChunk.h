// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdInitAckChunk.h

#pragma once
#include "bdCookie.h"

#include "bdChunk.h"
#include "bdCookieEchoChunk.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdCookie;
typedef bdReference<bdCookie> bdCookieRef;

class bdInitAckChunk;
typedef bdReference<bdInitAckChunk> bdInitAckChunkRef;

class bdInitAckChunk : public bdChunk {  // size 0x40
public:
    BD_CLASS_ALLOCATOR(bdInitAckChunk);
    enum bdInitAckChunkFlags {
        BD_IC_NO_FLAGS = 0,
    };
    bdInitAckChunk();
    bdInitAckChunk(bdUInt32, bdCookieRef, const bdWord, bdUInt32);
    virtual ~bdInitAckChunk();  // vtable slot 0
    bdUInt32 getInitTag() const;
    bdBool getCookie(bdByteBufferRef & cookie);
    bdWord getWindowCredit() const;
    virtual bdUInt serialize(bdUByte8 * data, const bdUInt size) const;  // vtable slot 3
    virtual bdBool deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt & offset);  // vtable slot 4
    bdUInt32 getPeerTag() const;
protected:
    bdUInt32 m_initTag;  // 0x14
    bdInitAckChunk::bdInitAckChunkFlags m_flags;  // 0x18
    bdByteBufferRef m_cookieBuffer;  // 0x20
    bdCookieRef m_cookie;  // 0x28
    bdWord m_windowCredit;  // 0x30
    bdUInt32 m_peerTag;  // 0x38
};
static_assert(sizeof(bdInitAckChunk) == 0x40, "bdInitAckChunk");


// Functions defined in this file (bodies are not in the DWARF):
//   line    60  void bdInitAckChunk::operator delete(void * p)  [inlined into 1 functions]
//   line    60  void * bdInitAckChunk::operator new(size_t nbytes)  [inlined into 2 functions]
