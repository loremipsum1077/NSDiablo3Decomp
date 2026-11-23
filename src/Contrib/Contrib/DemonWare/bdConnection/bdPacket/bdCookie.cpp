// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdCookie.cpp
#include "bdCookie.h"
#include "../../bdCore/bdCrypto/bdHMacSHA1.h"
#include "../../bdCore/bdContainers/bdSingleton.h"
#include "../../bdCore/bdUtilities/bdTrulyRandom.h"
#include "../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
extern void bdUseVAArgs(...);
bdUByte8 bdCookie::m_secret[20];
bdBool bdCookie::m_secretInitialized = false;
bdCookie::bdCookie() : m_localTag(0), m_peerTag(0), m_localTieTag(0), m_peerTieTag(0) {}
bdCookie::bdCookie(bdUInt32 localTag, bdUInt32 peerTag, bdUInt32 localTieTag, bdUInt32 peerTieTag)
    : m_localTag(localTag), m_peerTag(peerTag), m_localTieTag(localTieTag), m_peerTieTag(peerTieTag)
{
    if (!m_secretInitialized) {
        bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUByte8(m_secret, sizeof(m_secret));
        m_secretInitialized = true;
    }
}
bdCookie::~bdCookie() {}
bdUInt32 bdCookie::getLocalTag() const { return m_localTag; }
bdUInt32 bdCookie::getPeerTag() const { return m_peerTag; }
bdUInt32 bdCookie::getLocalTieTag() const { return m_localTieTag; }
bdUInt32 bdCookie::getPeerTieTag() const { return m_peerTieTag; }
bdUInt bdCookie::serialize(bdUByte8* data, const bdUInt size) const
{
    bdUInt offset = 20;
    bdUInt32 timeTag = 0;
    bdBool ok = bdBytePacker::appendBasicType(data, size, offset, offset, timeTag);
    ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, m_localTag);
    ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, m_peerTag);
    ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, m_peerTieTag);
    ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, m_localTieTag);
    if (data) {
        bdHMacSHA1 hmac(m_secret, sizeof(m_secret));
        hmac.process(data + 20, offset - 20);
        bdUInt hmacSize = 20;
        hmac.getData(data, hmacSize);
    }
    return offset;
}
bdBool bdCookie::deserialize(const bdUByte8* const data, const bdUInt size, bdUInt& offset)
{
    bdBool ok = false;
    if (size - offset > 20) {
        bdUByte8 tmpHMac[20];
        bdUInt bytesRead = offset;
        const bdUByte8* const cookieData = data + bytesRead;
        bdHMacSHA1 hmac(m_secret, sizeof(m_secret));
        hmac.process(cookieData + 20, getSerializedSize() - 20);
        bdUInt hmacSize = 20;
        hmac.getData(tmpHMac, hmacSize);
        if (!bdMemcmp(cookieData, tmpHMac, 20)) {
            bytesRead += 20;
            bdUInt32 timeTag;
            ok = bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, timeTag);
            ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, m_localTag);
            ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, m_peerTag);
            ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, m_peerTieTag);
            ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, m_localTieTag);
            if (ok) offset = bytesRead;
        } else bdUseVAArgs();
    }
    return ok;
}
inline bdUInt bdCookie::getSerializedSize() const { const bdUInt serializedSize = serialize(0, 0); return serializedSize; }
