// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdDataChunk.h

#pragma once

#include "../bdMessage/bdMessage.h"
#include "bdChunk.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdDataChunk;
typedef bdReference<bdDataChunk> bdDataChunkRef;

typedef bdReference<bdMessage> bdMessageRef;

class bdDataChunk : public bdChunk {  // size 0x28
    friend class bdPacket; // Packet calls the protected split-payload codec.
public:
    BD_CLASS_ALLOCATOR(bdDataChunk);
    enum bdDataFlags {
        BD_DC_NO_FLAGS = 0,
        BD_DC_UNRELIABLE = 1,
        BD_DC_ENC_DATA = 2,
        BD_DC_UNENC_DATA = 4,
    };
    bdDataChunk();
    bdDataChunk(bdMessageRef, const bdDataChunk::bdDataFlags);
    virtual ~bdDataChunk();  // vtable slot 0
    void setSequenceNumber(bdSeqNumber seqNum);
    bdSeqNumber getSequenceNumber() const;
    bdMessageRef getMessage() const;
    bdUByte8 getFlags() const;
    virtual bdUInt getSerializedSize() const;  // vtable slot 5
protected:
    virtual bdUInt serialize(bdUByte8 * data, const bdUInt32 size) const;  // vtable slot 3
    bdUInt serializeUnencrypted(bdUByte8 * data, const bdUInt32 size) const;
    virtual bdBool deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt & offset);  // vtable slot 4
    bdBool deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt & offset, const bdUByte8 *const unencData, const bdUInt, bdUInt & unencOffset);
    bdMessageRef m_message;  // 0x18
    bdUByte8 m_flags;  // 0x20
    bdSeqNumber m_seqNum;  // 0x22
};
static_assert(sizeof(bdDataChunk) == 0x28, "bdDataChunk");


// Functions defined in this file (bodies are not in the DWARF):
//   line    39  void bdDataChunk::operator delete(void * p)  [inlined into 1 functions]
//   line    39  void * bdDataChunk::operator new(size_t nbytes)  [inlined into 2 functions]
