// Original file: Contrib/Contrib/DemonWare/bdConnection/bdWindow/bdUnreliableReceiveWindow.cpp
#include "bdUnreliableReceiveWindow.h"
#include "../bdPacket/bdDataChunk.h"
bdUnreliableReceiveWindow::bdUnreliableReceiveWindow() : m_seqNumber(-1) {}
bdUnreliableReceiveWindow::~bdUnreliableReceiveWindow() {}
bdBool bdUnreliableReceiveWindow::add(bdDataChunkRef chunk, bdBool acceptOutOfOrder) {
    bdSequenceNumber newSeqNum(m_seqNumber,chunk->getSequenceNumber(),16);
    if (newSeqNum > m_seqNumber) { m_recvQueue.enqueue(chunk); m_seqNumber = newSeqNum; }
    else if (acceptOutOfOrder) m_recvOutOfOrderQueue.enqueue(chunk);
    return true;
}
bdDataChunkRef bdUnreliableReceiveWindow::getNextToRead() {
    bdDataChunkRef chunk;
    if (!m_recvQueue.isEmpty()) { chunk = m_recvQueue.peek(); m_recvQueue.dequeue(); }
    return chunk;
}
bdDataChunkRef bdUnreliableReceiveWindow::getNextOutOfOrderToRead() {
    bdDataChunkRef chunk;
    if (!m_recvOutOfOrderQueue.isEmpty()) { chunk = m_recvOutOfOrderQueue.peek(); m_recvOutOfOrderQueue.dequeue(); }
    return chunk;
}
void bdUnreliableReceiveWindow::reset() {
    m_seqNumber = bdSequenceNumber(-1);
    while (!m_recvQueue.isEmpty()) m_recvQueue.dequeue();
    while (!m_recvOutOfOrderQueue.isEmpty()) m_recvOutOfOrderQueue.dequeue();
}
