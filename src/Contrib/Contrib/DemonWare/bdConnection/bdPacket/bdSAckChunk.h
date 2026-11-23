// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdSAckChunk.h

#pragma once

#include "bdChunk.h"
#include "../../bdCore/bdContainers/bdLinkedList.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdSAckChunk;
typedef bdReference<bdSAckChunk> bdSAckChunkRef;

class bdSAckChunk : public bdChunk {  // size 0x40
public:
    BD_CLASS_ALLOCATOR(bdSAckChunk);
    enum bdSAckFlags {
        BD_SC_NO_FLAGS = 0,
    };
    class bdGapAckBlock {  // size 0x8
    public:
        BD_CLASS_ALLOCATOR(bdGapAckBlock);
        bdGapAckBlock() : m_start(0), m_end(0) {}
        bdGapAckBlock(bdUInt start, bdUInt end) : m_start(start), m_end(end) {}
        bdUInt m_start;  // 0x00
        bdUInt m_end;  // 0x04
    };
    bdSAckChunk();
    bdSAckChunk(const bdWord, bdSAckChunk::bdSAckFlags);
    virtual ~bdSAckChunk();  // vtable slot 0
    bdSeqNumber getCumulativeAck() const;
    void setCumulativeAck(const bdSeqNumber seqNumAck);
    void addGap(const bdSAckChunk::bdGapAckBlock & gap);
    bdLinkedList<bdSAckChunk::bdGapAckBlock> & getGapList();
    void setWindowCredit(const bdWord credit);
    bdWord getWindowCredit() const;
    bdSAckChunk::bdSAckFlags getFlags() const;
protected:
    virtual bdUInt serialize(bdUByte8 * data, const bdUInt size) const;  // vtable slot 3
    virtual bdBool deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt & offset);  // vtable slot 4
    bdSAckChunk::bdSAckFlags m_flags;  // 0x14
    bdSeqNumber m_cumulativeAck;  // 0x18
    bdLinkedList<bdSAckChunk::bdGapAckBlock> m_gaps;  // 0x20
    bdWord m_windowCredit;  // 0x38
};
static_assert(sizeof(bdSAckChunk) == 0x40, "bdSAckChunk");


// Functions defined in this file (bodies are not in the DWARF):
//   line   149  void bdSAckChunk::operator delete(void * p)  [inlined into 1 functions]
//   line   149  void * bdSAckChunk::operator new(size_t nbytes)  [inlined into 2 functions]
//   line   161  bdSAckChunk::bdGapAckBlock::bdGapAckBlock()  [inlined into 1 functions]
