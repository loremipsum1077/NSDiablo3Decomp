// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdInitAckChunk.cpp
#include "bdInitAckChunk.h"
#include "../../bdCore/bdUtilities/bdBytePacker.h"
extern void bdUseVAArgs(...);

bdInitAckChunk::bdInitAckChunk() : bdChunk(BD_CT_INIT_ACK), m_initTag(0), m_flags(BD_IC_NO_FLAGS), m_windowCredit(0), m_peerTag(0) {}
bdInitAckChunk::bdInitAckChunk(bdUInt32 initTag, bdCookieRef cookie, const bdWord windowCredit, bdUInt32 peerTag) : bdChunk(BD_CT_INIT_ACK), m_initTag(initTag), m_flags(BD_IC_NO_FLAGS), m_cookie(cookie), m_windowCredit(windowCredit), m_peerTag(peerTag) {}
bdInitAckChunk::~bdInitAckChunk() {}
bdUInt32 bdInitAckChunk::getInitTag() const { return m_initTag; }
bdBool bdInitAckChunk::getCookie(bdByteBufferRef& cookie) {
    if ((bdByteBuffer*)m_cookieBuffer) cookie = m_cookieBuffer;
    return m_cookieBuffer.notNull();
}
bdUInt bdInitAckChunk::serialize(bdUByte8* data, const bdUInt size) const {
    bdUInt offset = 0;
    bdBool ok;
    if (m_cookie.notNull()) {
        offset = bdChunk::serialize(data, size);
        const bdUByte8 flags = (bdUByte8)m_flags;
        ok = bdBytePacker::appendBasicType(data, size, offset, offset, flags);
        bdUInt oldOffset = offset;
        offset += 2;
        ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, m_initTag);
        bdUByte8* const cookieDest = data ? data + offset : 0;
        const bdUInt cookieLength = m_cookie->serialize(cookieDest, size - offset);
        const bdUInt16 length16 = cookieLength;
        offset += length16;
        ok = ok && bdBytePacker::appendBasicType(data, size, oldOffset, oldOffset, length16);
    }
    return offset;
}
bdBool bdInitAckChunk::deserialize(const bdUByte8* const data, const bdUInt size, bdUInt& offset) {
    bdUInt bytesRead = offset;
    bdBool ok = true;
    if (size - bytesRead > 4) {
        bdUByte8 flags = 0;
        bdUInt16 length = 0;
        ok = bdChunk::deserialize(data, size, bytesRead);
        ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, flags);
        m_flags = (bdInitAckChunkFlags)flags;
        ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, length);
        ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, m_initTag);
        m_cookieBuffer = new bdByteBuffer(length, false);
        ok = ok && bdBytePacker::removeBuffer(data, size, bytesRead, bytesRead, m_cookieBuffer->getData(), length);
    }
    if (ok) offset = bytesRead;
    return ok;
}
bdUInt32 bdInitAckChunk::getPeerTag() const { return m_peerTag; }
