// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdSAckChunk.cpp
#include "bdSAckChunk.h"
#include "../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
bdSAckChunk::bdSAckChunk() : bdChunk(BD_CT_SACK), m_flags(BD_SC_NO_FLAGS), m_cumulativeAck(0), m_windowCredit(0) {}
bdSAckChunk::bdSAckChunk(const bdWord windowCredit, bdSAckFlags flags) : bdChunk(BD_CT_SACK), m_flags(flags), m_cumulativeAck(0), m_windowCredit(windowCredit) {}
bdSAckChunk::~bdSAckChunk() {}
bdSeqNumber bdSAckChunk::getCumulativeAck() const { return m_cumulativeAck; }
void bdSAckChunk::setCumulativeAck(const bdSeqNumber seqNumAck) { m_cumulativeAck = seqNumAck; }
void bdSAckChunk::addGap(const bdGapAckBlock &gap) { m_gaps.addTail(gap); }
bdLinkedList<bdSAckChunk::bdGapAckBlock> &bdSAckChunk::getGapList() { return m_gaps; }
void bdSAckChunk::setWindowCredit(const bdWord credit) { m_windowCredit = credit; }
bdWord bdSAckChunk::getWindowCredit() const { return m_windowCredit; }
bdUInt bdSAckChunk::serialize(bdUByte8 *data, const bdUInt size) const {
    bdUInt offset = bdChunk::serialize(data, size);
    const bdUByte8 flags = static_cast<bdUByte8>(m_flags);
    bdBool ok = bdBytePacker::appendBasicType(data, size, offset, offset, flags);
    bdUInt16 length = 0;
    ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, length);
    ++offset;
    const bdUInt16 seqNum = m_cumulativeAck;
    ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, seqNum);
    const bdUInt32 recvWndCredit = static_cast<bdUInt32>(m_windowCredit);
    ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, recvWndCredit);
    const bdUInt16 numGaps = static_cast<bdUInt16>(m_gaps.getSize());
    ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, numGaps);
    const bdUInt16 tmpZero = 0;
    ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, tmpZero);
    bdLinkedList<bdGapAckBlock>::Position pos = m_gaps.getHeadPosition();
    while (pos) {
        const bdGapAckBlock &gap = m_gaps.getAt(pos);
        const bdUInt16 start = static_cast<bdUInt16>(gap.m_start);
        ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, start);
        const bdUInt16 end = static_cast<bdUInt16>(gap.m_end);
        ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, end);
        m_gaps.forward(pos);
    }
    return offset;
}
bdBool bdSAckChunk::deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt &offset) {
    bdUInt bytesRead = offset;
    bdUByte8 flags = 0;
    bdUInt16 length;
    bdUInt16 seqNum = 0;
    bdUInt32 recvWndCredit = 0;
    bdUInt16 numGaps = 0;
    bdBool ok = bdChunk::deserialize(data, size, bytesRead) && bdBytePacker::removeBasicType(data,size,bytesRead,bytesRead,flags);
    m_flags = static_cast<bdSAckFlags>(flags);
    ok = ok && bdBytePacker::removeBasicType(data,size,bytesRead,bytesRead,length);
    ++bytesRead;
    ok = ok && bdBytePacker::removeBasicType(data,size,bytesRead,bytesRead,seqNum);
    m_cumulativeAck = seqNum;
    ok = ok && bdBytePacker::removeBasicType(data,size,bytesRead,bytesRead,recvWndCredit);
    m_windowCredit = recvWndCredit;
    ok = ok && bdBytePacker::removeBasicType(data,size,bytesRead,bytesRead,numGaps);
    bytesRead += 2;
    bdUInt16 numGapsRead = 0;
    while (ok & (numGapsRead < numGaps)) {
        bdUInt16 start = 0, end = 0;
        ok = bdBytePacker::removeBasicType(data,size,bytesRead,bytesRead,start);
        ok = ok && bdBytePacker::removeBasicType(data,size,bytesRead,bytesRead,end);
        m_gaps.addTail(bdGapAckBlock(start,end));
        bdLogInfo("", "", start, end);
        ++numGapsRead;
    }
    if (ok) offset = bytesRead;
    return ok;
}
