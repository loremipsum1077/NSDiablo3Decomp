// Original file: Contrib/Contrib/DemonWare/bdConnection/bdWindow/bdReliableSendWindow.cpp
#include "bdReliableSendWindow.h"
#include "../bdPacket/bdSAckChunk.h"
#include "../bdPacket/bdPacket.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
bdReliableSendWindow::bdReliableSendWindow() : m_lastAcked(-1), m_nextFree(0), m_remoteReceiveWindowCredit(15000), m_flightSize(0), m_partialBytesAcked(0), m_slowStartThresh(15000), m_congestionWindow(2576) { m_timeoutPeriod = 0.3f; m_retransmitCountThreshold = 3; m_lastSent.start(); }
bdReliableSendWindow::~bdReliableSendWindow() {}
void bdReliableSendWindow::setTimeoutPeriod(const bdFloat32 secs) { m_timeoutPeriod = secs; }
bdBool bdReliableSendWindow::add(bdDataChunkRef chunk) {
    bdBool messageAdded = false;
    const bdUInt index = m_nextFree.getValue() & 127;
    bdMessageFrame &frameSlot = m_frame[index];
    if (!frameSlot.m_chunk) {
        const bdInt nextFreeSeqNum = m_nextFree.getValue();
        chunk->setSequenceNumber(static_cast<bdSeqNumber>(nextFreeSeqNum));
        frameSlot = bdMessageFrame(chunk);
        m_nextFree++;
        messageAdded = true;
    } else bdLogWarn("", "");
    return messageAdded;
}
void bdReliableSendWindow::getDataToSend(bdPacket &packet) {
    bdBool retransmitTimerExpired = false;
    bdBool neededFastRetransmit = false;
    bdBool haveDataToSend = false;
    for (bdSequenceNumber i = m_lastAcked + bdSequenceNumber(1); i < m_nextFree; i++) {
        const bdUInt index = i.getValue() & 127;
        if (m_frame[index].m_chunk.notNull()) {
            const bdSequenceNumber frameSeqNum(m_lastAcked,m_frame[index].m_chunk->getSequenceNumber(),16);
            if (frameSeqNum.getValue() != i.getValue()) bdLogWarn("", "");
            bdMessageFrame &frameSlot = m_frame[index];
            if (frameSlot.m_chunk.notNull()) {
                const bdInt serializedSize = static_cast<bdInt>(frameSlot.m_chunk->getSerializedSize());
                if (frameSlot.m_sendCount == 0) break;
                if (frameSlot.m_missingCount >= m_retransmitCountThreshold) {
                    neededFastRetransmit = true;
                    m_flightSize -= serializedSize;
                } else if (frameSlot.m_timer.getElapsedTimeInSeconds() > m_timeoutPeriod) {
                    retransmitTimerExpired = true;
                    m_flightSize -= serializedSize;
                }
            }
        }
    }
    if (neededFastRetransmit) decreaseCongestionWindow(BD_CWDR_PACKET_LOSS_DETECTED);
    if (retransmitTimerExpired) {
        decreaseCongestionWindow(BD_CWDR_RESEND_TIMER_EXPIRED);
        m_timeoutPeriod *= 2.0f;
        if (m_timeoutPeriod > 2.0f) {
            m_timeoutPeriod = 2.0f;
        }
    }
    const bdUWord pmtu = 1288;
    bdBool full = false;
    for (bdSequenceNumber i = m_lastAcked + bdSequenceNumber(1); i < m_nextFree && !full; i++) {
        const bdUInt index = i.getValue() & 127;
        bdMessageFrame &frameSlot = m_frame[index];
        if (frameSlot.m_chunk.notNull()) {
            bdDataChunkRef chunk = frameSlot.m_chunk;
            if (frameSlot.m_sendCount == 0) {
                const bdUInt serializedSize = chunk->getSerializedSize();
                const bdBool enoughWindowCredit = m_remoteReceiveWindowCredit-m_flightSize > static_cast<bdWord>(chunk->getSerializedSize());
                const bdBool fitsInPMTUAndCwmdNotExceeded = m_flightSize < m_congestionWindow && serializedSize < pmtu;
                if (enoughWindowCredit) {
                    if (packet.addChunk(chunk)) {
                        ++frameSlot.m_sendCount;
                        frameSlot.m_timer.start();
                        m_flightSize += serializedSize;
                        haveDataToSend = true;
                    } else full = true;
                } else if (fitsInPMTUAndCwmdNotExceeded) {
                    if (packet.addChunk(chunk)) {
                        bdLogInfo("", "",chunk->getSequenceNumber());
                        ++frameSlot.m_sendCount;
                        frameSlot.m_timer.start();
                        m_flightSize += serializedSize;
                        haveDataToSend = true;
                    } else bdLogWarn("", "");
                    full = true;
                } else full = true;
            } else if (frameSlot.m_missingCount >= m_retransmitCountThreshold) {
                if (packet.addChunk(chunk)) {
                    bdLogInfo("", "",chunk->getSequenceNumber());
                    frameSlot.m_missingCount = 0;
                    ++frameSlot.m_sendCount;
                    frameSlot.m_timer.start();
                    m_flightSize += chunk->getSerializedSize();
                    haveDataToSend = true;
                } else { bdLogWarn("", ""); full = true; }
            } else if (frameSlot.m_timer.getElapsedTimeInSeconds() > m_timeoutPeriod) {
                const bdBool congestionWindowAllows = m_flightSize < m_congestionWindow;
                if (congestionWindowAllows) {
                    if (packet.addChunk(chunk)) {
                        bdLogInfo("", "",chunk->getSequenceNumber());
                        frameSlot.m_missingCount = 0;
                        ++frameSlot.m_sendCount;
                        frameSlot.m_timer.start();
                        m_flightSize += chunk->getSerializedSize();
                        haveDataToSend = true;
                    } else { bdLogWarn("", ""); full = true; }
                }
            }
        }
    }
    if (haveDataToSend) {
        m_lastSent.start();
    } else if (m_lastSent.getElapsedTimeInSeconds() > 1.0f) {
        decreaseCongestionWindow(BD_CWDR_INACTIVE);
        m_lastSent.start();
    }
}
bdBool bdReliableSendWindow::handleAck(bdSAckChunkRef chunk, bdFloat32 &rtt) {
    bdSequenceNumber ack(m_lastAcked,chunk->getCumulativeAck(),16);
    bdBool validAck = true;
    bdSequenceNumber lastSent = m_nextFree-bdSequenceNumber(1);
    if (ack > lastSent) {
        bdLogWarn("", "");
        return false;
    }
    if (ack.getValue() < m_lastAcked.getValue()) {
        return true;
    } else {
        const bdUInt ackIndex = ack.getValue()%128;
        rtt = m_frame[ackIndex].m_chunk.notNull() && m_frame[ackIndex].m_sendCount == 1 ? m_frame[ackIndex].m_timer.getElapsedTimeInSeconds() : 0.0f;
        m_remoteReceiveWindowCredit = chunk->getWindowCredit();
        m_flightSize = 0;
        bdUInt count = 0;
        for (bdSequenceNumber i = ack+bdSequenceNumber(1); count < 128; i++) {
            const bdUInt index = i.getValue() & 127;
            bdMessageFrame &frameSlot = m_frame[index];
            if (frameSlot.m_chunk.isNull()) break;
            m_flightSize += frameSlot.m_chunk->getSerializedSize()*frameSlot.m_sendCount;
            ++count;
        }
        bdUInt bytesAcked = 0;
        for (bdSequenceNumber i = m_lastAcked+bdSequenceNumber(1); i <= ack; i++) {
            const bdUInt index = i.getValue() & 127;
            if (m_frame[index].m_chunk.notNull()) {
                bdMessageFrame &frameSlot = m_frame[index];
                bytesAcked += frameSlot.m_chunk->getSerializedSize();
                frameSlot.m_chunk = 0;
                frameSlot.m_timer.reset();
            } else {
                bdLogWarn("", "");
                validAck = false;
            }
        }
        bdLinkedList<bdSAckChunk::bdGapAckBlock> &blocks = chunk->getGapList();
        bdSequenceNumber lastSeq = ack+bdSequenceNumber(1);
        while (validAck && !blocks.isEmpty()) {
            bdSAckChunk::bdGapAckBlock &block = blocks.getHead();
            const bdSequenceNumber startSeq = ack+bdSequenceNumber(block.m_start);
            const bdSequenceNumber endSeq = ack+bdSequenceNumber(block.m_end);
            validAck = true;
            for (bdSequenceNumber i = lastSeq; i <= endSeq; i++) {
                const bdUInt index = i.getValue() & 127;
                bdMessageFrame &frameSlot = m_frame[index];
                if (frameSlot.m_chunk.isNull()) { bdLogWarn("", ""); validAck = false; }
                if (frameSlot.m_sendCount == 0) { bdLogWarn("", ""); validAck = false; }
                if (i < startSeq) {
                    ++frameSlot.m_missingCount;
                    if (frameSlot.m_gapAcked) {
                        frameSlot.m_gapAcked = false;
                        if (frameSlot.m_timer.getElapsedTimeInSeconds() == 0.0f) frameSlot.m_timer.start();
                    }
                } else {
                    frameSlot.m_gapAcked = true;
                    frameSlot.m_timer.start();
                    bytesAcked += frameSlot.m_chunk->getSerializedSize();
                }
            }
            lastSeq = endSeq+bdSequenceNumber(1);
            blocks.removeHead();
        }
        increaseCongestionWindow(bytesAcked);
        m_lastAcked = ack;
    }
    return validAck;
}
bdBool bdReliableSendWindow::isEmpty() const {
    bdBool empty = true;
    for (bdSequenceNumber i = m_lastAcked; i < m_nextFree && empty; i++) {
        const bdUInt index = i.getValue() & 127;
        const bdMessageFrame &frameSlot = m_frame[index];
        empty = frameSlot.m_chunk.isNull();
    }
    return empty;
}
inline void bdReliableSendWindow::increaseCongestionWindow(const bdUWord bytesAcked) {
    const bdUWord pmtu = 1288;
    if (m_flightSize >= m_congestionWindow) {
        if (m_congestionWindow <= m_slowStartThresh) m_congestionWindow += bytesAcked < pmtu ? bytesAcked : pmtu;
        else {
            m_partialBytesAcked += bytesAcked;
            if (m_partialBytesAcked >= m_congestionWindow) {
                m_congestionWindow += pmtu;
                if (m_partialBytesAcked > m_congestionWindow) {
                    m_partialBytesAcked -= m_congestionWindow;
                } else {
                    m_partialBytesAcked = 0;
                }
            }
        }
    }
}
inline void bdReliableSendWindow::decreaseCongestionWindow(const bdCongestionWindowDecreaseReason reason) {
    const bdUWord pmtu = 1288;
    if (reason == BD_CWDR_INACTIVE) {
        m_congestionWindow = m_congestionWindow/2 > static_cast<bdWord>(4*pmtu) ? m_congestionWindow/2 : static_cast<bdWord>(4*pmtu);
        m_partialBytesAcked = 0;
        m_flightSize = 0;
    } else if (reason == BD_CWDR_PACKET_LOSS_DETECTED) {
        m_slowStartThresh = m_congestionWindow/2 > static_cast<bdWord>(2*pmtu) ? m_congestionWindow/2 : static_cast<bdWord>(2*pmtu);
        m_congestionWindow = m_slowStartThresh;
        m_partialBytesAcked = 0;
    } else if (reason == BD_CWDR_RESEND_TIMER_EXPIRED) {
        m_slowStartThresh = m_congestionWindow/2 > static_cast<bdWord>(2*pmtu) ? m_congestionWindow/2 : static_cast<bdWord>(2*pmtu);
        m_congestionWindow = pmtu;
        m_partialBytesAcked = 0;
    }
}
