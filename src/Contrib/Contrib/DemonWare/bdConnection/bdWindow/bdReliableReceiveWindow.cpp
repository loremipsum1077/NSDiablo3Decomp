// Original file: Contrib/Contrib/DemonWare/bdConnection/bdWindow/bdReliableReceiveWindow.cpp
#include "bdReliableReceiveWindow.h"
#include "../bdPacket/bdDataChunk.h"
#include "../bdPacket/bdSAckChunk.h"
#include "../bdPacket/bdPacket.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
bdReliableReceiveWindow::bdReliableReceiveWindow() : m_newest(-1), m_lastCumulative(-1), m_lastDispatched(-1), m_shouldAck(false), m_recvWindowCredit(15000), m_recvWindowUsedCredit(0) {}
bdReliableReceiveWindow::~bdReliableReceiveWindow() {}
bdBool bdReliableReceiveWindow::add(bdDataChunkRef chunk) {
    bdBool result = false;
    bdSequenceNumber seqNumber(m_newest,chunk->getSequenceNumber(),16);
    const bdInt serializedSize = static_cast<bdInt>(chunk->getSerializedSize());
    if (seqNumber < m_newest || m_recvWindowUsedCredit + serializedSize <= m_recvWindowCredit) {
        if (!(seqNumber > m_lastDispatched + bdSequenceNumber(128))) {
            result = true;
            if (seqNumber > m_lastDispatched) {
                const bdUInt index = seqNumber.getValue() & 127;
                if (m_frame[index].isNull()) {
                    m_recvWindowUsedCredit += chunk->getSerializedSize();
                    m_frame[index] = chunk;
                    if (seqNumber > m_newest) m_newest = seqNumber;
                } else if (m_frame[index]->getSequenceNumber() != chunk->getSequenceNumber()) result = false;
            }
        }
    }
    if (!result) bdLogWarn("", "");
    calculateAck();
    return result;
}
void bdReliableReceiveWindow::calculateAck() {
    bdSequenceNumber i = (m_lastCumulative > m_lastDispatched ? m_lastCumulative + bdSequenceNumber(1) : m_lastDispatched + bdSequenceNumber(1));
    while (i <= m_newest && m_frame[i.getValue() & 127].notNull()) { m_lastCumulative = i; i++; }
    if (m_lastCumulative != bdSequenceNumber(-1)) {
        m_sack = new bdSAckChunk(m_recvWindowCredit-m_recvWindowUsedCredit,bdSAckChunk::BD_SC_NO_FLAGS);
        bdSAckChunk::bdGapAckBlock block;
        i = m_lastCumulative + bdSequenceNumber(1);
        for (; i <= m_newest; i++) {
            const bdUInt frameIndex = i.getValue() & 127;
            if (m_frame[frameIndex].isNull()) {
                if (block.m_start) {
                    block.m_end = (i-bdSequenceNumber(1)-m_lastCumulative).getValue();
                    m_sack->addGap(block);
                    block.m_start = block.m_end = 0;
                }
            } else if (!block.m_start) block.m_start = (i-m_lastCumulative).getValue();
        }
        if (block.m_start) {
            block.m_end = (i-bdSequenceNumber(1)-m_lastCumulative).getValue();
            m_sack->addGap(block);
        }
        m_sack->setCumulativeAck(static_cast<bdSeqNumber>(m_lastCumulative.getValue()));
    }
}
void bdReliableReceiveWindow::getDataToSend(bdPacket &packet) {
    if (m_sack) {
        if (packet.addChunk(m_sack)) m_sack = 0;
        else bdLogWarn("", "");
    }
}
bdDataChunkRef bdReliableReceiveWindow::getNextToRead() {
    bdDataChunkRef chunk;
    const bdUInt32 index = (m_lastDispatched.getValue()+1) & 127;
    bdSequenceNumber toDispatch = m_lastDispatched + bdSequenceNumber(1);
    if (m_frame[index].notNull()) {
        bdSequenceNumber frameSeqNumber(m_lastDispatched,m_frame[index]->getSequenceNumber(),16);
        if (toDispatch == frameSeqNumber) {
            chunk = m_frame[index];
            m_frame[index] = 0;
            m_lastDispatched = frameSeqNumber;
            m_recvWindowUsedCredit -= chunk->getSerializedSize();
            if (m_sack) m_sack->setWindowCredit(m_recvWindowCredit-m_recvWindowUsedCredit);
            else bdLogWarn("", "");
        } else bdLogWarn("", "", toDispatch.getValue(),frameSeqNumber.getValue());
    }
    return chunk;
}
