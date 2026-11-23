// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSCookieEcho.cpp
#include "bdDTLSCookieEcho.h"
#include "../../../bdCore/bdCrypto/bdCryptoUtils.h"
#include "../../../bdCore/bdCrypto/bdECCKey.h"
#include "../../../bdCore/bdSocket/bdCommonAddr.h"
#include "../../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"




bdDTLSCookieEcho::bdDTLSCookieEcho() : bdDTLSHeader(),
    m_cookie()
{
    bdMemset(m_ca, 0, sizeof(m_ca));
    bdMemset(m_secID, 0, sizeof(m_secID));
    bdMemset(m_ECCKey, 0, sizeof(m_ECCKey));
}



bdDTLSCookieEcho::bdDTLSCookieEcho(bdUInt16 vtag,
                                   const bdDTLSInitAck & cookie,
                                   bdCommonAddrRef localCommonAddr,
                                   bdECCKey *const ECCKey)
    : bdDTLSHeader(BD_DTLS_COOKIE_ECHO, vtag, 0),
    m_cookie(cookie)
{
    bdSecurityID secID;
    cookie.getSecID(secID);

    bdUByte8 caBuffer[sizeof(m_ca)];
    localCommonAddr->serialize(caBuffer);

    bdUByte8 key[sizeof(m_ECCKey)];
    bdUInt keyLen = sizeof(key);
    const bdBool ok = ECCKey->exportKey(key, keyLen);
    if (keyLen != sizeof(key) || !ok)
    {
        bdLogError("bdSocket/dtls", "Failed to export ECC key");
    }

    bdMemcpy(m_ca, caBuffer, sizeof(m_ca));
    bdMemcpy(m_secID, &secID, sizeof(m_secID));
    bdMemcpy(m_ECCKey, key, sizeof(m_ECCKey));
}

bdDTLSCookieEcho::~bdDTLSCookieEcho()
{
    bdCryptoUtils::zeroBuffer(m_ca, sizeof(m_ca));
    bdCryptoUtils::zeroBuffer(m_secID, sizeof(m_secID));
    bdCryptoUtils::zeroBuffer(m_ECCKey, sizeof(m_ECCKey));
}

bdBool bdDTLSCookieEcho::serialize(void * data,
                                 const bdUInt size,
                                 const bdUInt offset,
                                 bdUInt & newOffset) const
{
    bdBool status = true;
    newOffset = offset;

    status = status && bdDTLSHeader::serialize(data, size, offset, newOffset);
    status = status && m_cookie.serialize(data, size, newOffset, newOffset);
    status = status && bdBytePacker::appendBuffer(data, size, newOffset, newOffset, m_ca, sizeof(m_ca));
    status = status && bdBytePacker::appendBuffer(data, size, newOffset, newOffset, m_secID, sizeof(m_secID));
    status = status && bdBytePacker::appendBuffer(data, size, newOffset, newOffset, m_ECCKey, sizeof(m_ECCKey));

    if (!status)
    {
        newOffset = offset;
    }

    return status;
}

bdBool bdDTLSCookieEcho::deserialize(const void * data,
                                   const bdUInt size,
                                   const bdUInt offset,
                                   bdUInt & newOffset,
                                   const bdCypherSuite expectedCypherSuite)
{
    bdBool status = true;
    newOffset = offset;

    status = status && bdDTLSHeader::deserialize(data, size, offset, newOffset);
    status = status && m_cookie.deserialize(data, size, newOffset, newOffset, expectedCypherSuite);
    status = status && bdBytePacker::removeBuffer(data, size, newOffset, newOffset, m_ca, sizeof(m_ca));
    status = status && bdBytePacker::removeBuffer(data, size, newOffset, newOffset, m_secID, sizeof(m_secID));
    status = status && bdBytePacker::removeBuffer(data, size, newOffset, newOffset, m_ECCKey, sizeof(m_ECCKey));

    if (!status)
    {
        newOffset = offset;
    }

    return status;
}

const bdDTLSInitAck & bdDTLSCookieEcho::getCookie() const
{
    return m_cookie;
}

const bdUByte8 * bdDTLSCookieEcho::getCa() const
{
    return m_ca;
}

const bdUByte8 * bdDTLSCookieEcho::getSecID() const
{
    return m_secID;
}

const bdUByte8 * bdDTLSCookieEcho::getECCKey() const
{
    return m_ECCKey;
}
