// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdSequenceNumber.cpp
#include "bdSequenceNumber.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
bdSequenceNumber::bdSequenceNumber(const bdInt seqNum) : m_seqNum(seqNum) {}
bdSequenceNumber::bdSequenceNumber(const bdSequenceNumber &last, const bdUInt seqNumber, const bdUInt bits) : m_seqNum(-1) { set(last,seqNumber,bits); }
inline void bdSequenceNumber::set(const bdSequenceNumber &last, const bdUInt seqNumber, const bdUInt bits) {
    const bdInt range = 1 << bits;
    const bdInt lastRangeBase = (last.m_seqNum / range) * range;
    const bdInt lastSeq = last.m_seqNum - lastRangeBase;
    const bdInt seq = static_cast<bdInt>(seqNumber) % range;
    if (lastSeq < 0) m_seqNum = seqNumber;
    else if (lastSeq == seq) m_seqNum = last.m_seqNum;
    else {
        // The two directions deliberately retain the half-range ambiguity.
        const bdBool forward = (lastSeq < seq && seq-lastSeq < range/2) || (lastSeq > seq && lastSeq-seq > range/2);
        const bdBool backward = (lastSeq < seq && seq-lastSeq > range/2) || (lastSeq > seq && lastSeq-seq < range/2);
        if (!(forward || backward)) bdLogWarn("", "");
        bdInt rangeChange = 0;
        if (lastSeq > seq && backward) rangeChange = 0;
        else if (lastSeq < seq && backward) rangeChange = -1;
        else if (forward && lastSeq > seq) rangeChange = 1;
        m_seqNum = lastRangeBase + (rangeChange << bits) + seq;
    }
}
bdInt32 bdSequenceNumber::getValue() const { return m_seqNum; }
bdSequenceNumber bdSequenceNumber::operator+(const bdSequenceNumber &other) const { bdSequenceNumber seqNum(m_seqNum + other.m_seqNum); return seqNum; }
bdSequenceNumber bdSequenceNumber::operator++(bdInt) { bdSequenceNumber other(*this); ++m_seqNum; return other; }
bdSequenceNumber bdSequenceNumber::operator-(const bdSequenceNumber &other) const { bdSequenceNumber seqNum(m_seqNum - other.m_seqNum); return seqNum; }
bdBool bdSequenceNumber::operator>(const bdSequenceNumber &other) const { return m_seqNum > other.m_seqNum; }
bdBool bdSequenceNumber::operator<(const bdSequenceNumber &other) const { return m_seqNum < other.m_seqNum; }
bdBool bdSequenceNumber::operator<=(const bdSequenceNumber &other) const { return m_seqNum <= other.m_seqNum; }
bdBool bdSequenceNumber::operator==(const bdSequenceNumber &other) const { return m_seqNum == other.m_seqNum; }
bdBool bdSequenceNumber::operator!=(const bdSequenceNumber &other) const { return m_seqNum != other.m_seqNum; }
