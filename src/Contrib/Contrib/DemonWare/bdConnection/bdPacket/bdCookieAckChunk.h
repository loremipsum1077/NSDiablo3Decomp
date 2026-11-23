// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdCookieAckChunk.h

#pragma once

#include "bdChunk.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdCookieAckChunk;
typedef bdReference<bdCookieAckChunk> bdCookieAckChunkRef;

class bdCookieAckChunk : public bdChunk {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdCookieAckChunk);
    enum bdCookieAckFlags {
        BD_CA_NO_FLAGS = 0,
    };
    bdCookieAckChunk();
    virtual ~bdCookieAckChunk();  // vtable slot 0
    bdCookieAckChunk::bdCookieAckFlags getFlags() const;
protected:
    virtual bdUInt serialize(bdUByte8 * data, const bdUInt size) const;  // vtable slot 3
    virtual bdBool deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt & offset);  // vtable slot 4
    bdCookieAckChunk::bdCookieAckFlags m_flags;  // 0x14
};
static_assert(sizeof(bdCookieAckChunk) == 0x18, "bdCookieAckChunk");


// Functions defined in this file (bodies are not in the DWARF):
//   line    30  void bdCookieAckChunk::operator delete(void * p)  [inlined into 1 functions]
//   line    30  void * bdCookieAckChunk::operator new(size_t nbytes)  [inlined into 2 functions]
