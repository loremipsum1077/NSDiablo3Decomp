// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdCookieEchoChunk.cpp
#include "bdCookieEchoChunk.h"
#include "../../bdCore/bdUtilities/bdBytePacker.h"
extern void bdUseVAArgs(...);

bdCookieEchoChunk::bdCookieEchoChunk() : bdChunk(BD_CT_COOKIE_ECHO), m_flags(BD_CE_NO_FLAGS) {}
bdCookieEchoChunk::bdCookieEchoChunk(bdByteBufferRef cookieBuffer) : bdChunk(BD_CT_COOKIE_ECHO), m_flags(BD_CE_NO_FLAGS), m_cookieBuffer(cookieBuffer) {}
bdCookieEchoChunk::~bdCookieEchoChunk() {}
bdBool bdCookieEchoChunk::getCookie(bdCookieRef& cookie) {
    if ((bdCookie*)m_cookie) cookie = m_cookie;
    return m_cookie.notNull();
}
bdUInt bdCookieEchoChunk::serialize(bdUByte8* data, const bdUInt size) const {
    bdUInt offset = 0;
    bdBool ok;
    if (m_cookie.notNull() || m_cookieBuffer.notNull()) {
        offset = bdChunk::serialize(data, size);
        bdUByte8 flags = (bdUByte8)m_flags;
        ok = bdBytePacker::appendBasicType(data, size, offset, offset, flags);
        if (m_cookieBuffer.notNull()) {
            const bdUInt16 length = m_cookieBuffer->getSize();
            ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, length);
            ok = ok && bdBytePacker::appendBuffer(data, size, offset, offset, m_cookieBuffer->getData(), length);
        } else {
            bdUInt oldOffset = offset;
            offset += 2;
            bdUByte8* const cookieDest = data ? data + offset : 0;
            const bdUInt16 length = m_cookie->serialize(cookieDest, size - offset);
            offset += length;
            ok = ok && bdBytePacker::appendBasicType(data, size, oldOffset, oldOffset, length);
        }
    }
    return offset;
}
bdBool bdCookieEchoChunk::deserialize(const bdUByte8* const data, const bdUInt size, bdUInt& offset) {
    bdUInt bytesRead = offset;
    bdBool ok = bdChunk::deserialize(data, size, bytesRead);
    bdUByte8 flags = 0;
    bdUInt16 length = 0;
    ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, flags);
    m_flags = (bdCookieEchoFlags)flags;
    ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, length);
    m_cookie = new bdCookie();
    const bdUInt cookieOffset = bytesRead;
    if (ok) {
        ok = m_cookie->deserialize(data, size, bytesRead);
        if (ok && bytesRead - cookieOffset != length) {
            bdUseVAArgs();
            return false;
        }
        offset = bytesRead;
        return true;
    }
    return false;
}
