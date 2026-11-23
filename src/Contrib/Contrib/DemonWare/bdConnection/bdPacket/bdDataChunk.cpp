// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdDataChunk.cpp
#include "bdDataChunk.h"
#include "../../bdCore/bdUtilities/bdBytePacker.h"
extern void bdUseVAArgs(...);
bdDataChunk::bdDataChunk() : bdChunk(BD_CT_DATA), m_flags(BD_DC_NO_FLAGS), m_seqNum(0) {}
bdDataChunk::bdDataChunk(bdMessageRef data, const bdDataFlags flags)
    : bdChunk(BD_CT_DATA), m_message(data), m_flags(flags), m_seqNum(0)
{
    if (m_message.notNull()) {
        if (m_message->hasPayload() && m_message->getPayload()->getNumBitsWritten())
            m_flags |= BD_DC_ENC_DATA;
        if (m_message->hasUnencryptedPayload() && m_message->getUnencryptedPayload()->getMaxReadSize())
            m_flags |= BD_DC_UNENC_DATA;
    }
}
bdDataChunk::~bdDataChunk() {}
void bdDataChunk::setSequenceNumber(bdSeqNumber seqNum) { m_seqNum = seqNum; }
bdSeqNumber bdDataChunk::getSequenceNumber() const { return m_seqNum; }
bdMessageRef bdDataChunk::getMessage() const { return m_message; }
bdUByte8 bdDataChunk::getFlags() const { return m_flags; }
bdUInt bdDataChunk::serializeUnencrypted(bdUByte8* data, const bdUInt32 size) const
{
    bdUInt bytesWritten = 0;
    if (m_flags & BD_DC_UNENC_DATA) {
        const bdByteBufferRef unEncPayload = m_message->getUnencryptedPayload();
        if (unEncPayload.notNull()) {
            const bdUByte8* const unEncData = unEncPayload->getData();
            const bdUInt unEncDataSize = unEncPayload->getDataSize();
            bdUInt offset = 0;
            if (bdBytePacker::appendBuffer(data, size, offset, offset, unEncData, unEncDataSize))
                bytesWritten = offset;
        }
    }
    return bytesWritten;
}
bdUInt bdDataChunk::getSerializedSize() const
{
    const bdUInt encSerializedSize = serialize(0, 0);
    const bdUInt unencSerializedSize = serializeUnencrypted(0, 0);
    const bdUInt serializedSize = encSerializedSize + unencSerializedSize;
    return serializedSize;
}
// `payloadSize > 0` (not a plain truth test) in both tests: only that spelling gives the original code.
bdUInt bdDataChunk::serialize(bdUByte8* data, const bdUInt32 size) const
{
    bdUInt offset = bdChunk::serialize(data, size);
    bdBool ok = true;
    if (m_message.notNull()) {
        bdUInt16 payloadSize = 0;
        ok = bdBytePacker::appendBasicType(data, size, offset, offset, m_flags);
        const bdUByte8 type = m_message->getType();
        ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, type);
        const bdUInt16 seqNum = m_seqNum;
        ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, seqNum);
        if (m_flags & BD_DC_ENC_DATA) {
            const bdBitBufferRef payload = m_message->getPayload();
            if (payload.notNull()) {
                payloadSize = payload->getNumBitsWritten();
                if (payloadSize > 0) payloadSize = payload->getDataSize();
            }
            ok = ok && bdBytePacker::appendEncodedUInt16(data, size, offset, offset, payloadSize);
        }
        if (m_flags & BD_DC_UNENC_DATA) {
            const bdByteBufferRef unencPayload = m_message->getUnencryptedPayload();
            bdUInt16 unencPlayloadSize = 0;
            if (unencPayload.notNull()) unencPlayloadSize = unencPayload->getDataSize();
            ok = ok && bdBytePacker::appendEncodedUInt16(data, size, offset, offset, unencPlayloadSize);
        }
        if (payloadSize > 0) {
            const bdUByte8* const payloadData = m_message->getPayload()->getData();
            ok = ok && bdBytePacker::appendBuffer(data, size, offset, offset, payloadData, payloadSize);
        }
    }
    const bdUInt bytesWritten = ok ? offset : 0;
    return bytesWritten;
}
bdBool bdDataChunk::deserialize(const bdUByte8* const data, const bdUInt size, bdUInt& offset)
{
    bdUInt encOffset = 0;
    const bdBool ok = deserialize(data, size, offset, 0, 0, encOffset);
    return ok;
}
bdBool bdDataChunk::deserialize(const bdUByte8* const data, const bdUInt size, bdUInt& offset,
    const bdUByte8* const unencData, const bdUInt, bdUInt& unencOffset)
{
    bdUInt bytesRead = offset;
    bdUByte8 type8 = 0;
    bdUInt16 seqNum = 0;
    bdUInt unencBytesRead = unencOffset;
    bdBool ok = bdChunk::deserialize(data, size, bytesRead);
    ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, m_flags);
    ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, type8);
    ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, seqNum);
    m_seqNum = seqNum;
    bdUInt16 payloadSize = 0;
    if (m_flags & BD_DC_ENC_DATA) {
        ok = ok && bdBytePacker::removeEncodedUInt16(data, size, bytesRead, bytesRead, payloadSize);
        // The original diagnostic keeps the available count in its second argument register.
        if (size - bytesRead < payloadSize) { bdUseVAArgs(payloadSize, size - bytesRead); return false; }
    }
    bdUInt16 unencPayloadSize = 0;
    if (m_flags & BD_DC_UNENC_DATA) {
        ok = ok && bdBytePacker::removeEncodedUInt16(data, size, bytesRead, bytesRead, unencPayloadSize);
        if (size - bytesRead < unencPayloadSize) { bdUseVAArgs(unencPayloadSize, size - bytesRead); return false; }
    }
    if (ok) {
        const bdMessageType type = (bdMessageType)type8;
        m_message = new bdMessage(type, data + bytesRead, payloadSize, true, unencData + unencBytesRead, unencPayloadSize);
        ok = m_message.notNull();
        bytesRead += payloadSize;
        unencBytesRead += unencPayloadSize;
        offset = bytesRead;
        unencOffset = unencBytesRead;
    }
    return ok;
}
