// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSData.cpp
// Reconstructed from original DWARF, source-line and instruction evidence.

#include "bdDTLSData.h"
#include "../../../bdCore/bdContainers/bdSequenceNumber.h"
#include "../../../bdCore/bdCrypto/bdAuthenticationTag.h"
#include "../../../bdCore/bdCrypto/bdCypherAESGCM.h"
#include "../../../bdCore/bdCrypto/bdCryptoUtils.h"
#include "../../../bdCore/bdCrypto/bdHMacSHA256.h"
#include "../../../bdCore/bdCrypto/bdHash.h"
#include "../../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"

namespace {
// Original bdCypherSuites enumerator values; no runtime storage.
enum {
    BD_DTLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256 = 48385,
    BD_DTLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256 = 48386,
};
}

bdDTLSData::bdDTLSData()
    : bdDTLSHeader(), m_macSize(0)
{
    bdMemset(m_mac, 0, sizeof(m_mac));
}

bdDTLSData::bdDTLSData(bdUInt16 vtag, bdUInt16 counter)
    : bdDTLSHeader(BD_DTLS_DATA, vtag, counter), m_macSize(0)
{
    bdMemset(m_mac, 0, sizeof(m_mac));
}

bdDTLSData::~bdDTLSData()
{
}

bdBool bdDTLSData::serialize(void * data, const bdUInt size,
                            const bdUInt offset, bdUInt & newOffset) const
{
    newOffset = offset;
    bdBool status = bdDTLSHeader::serialize(data, size, offset, newOffset);
    status = status && bdBytePacker::appendBuffer(data, size, newOffset,
                                                 newOffset, m_mac, m_macSize);
    if (!status)
        newOffset = offset;
    return status;
}

bdBool bdDTLSData::deserialize(const void * data, const bdUInt size,
                              const bdUInt offset, bdUInt & newOffset)
{
    newOffset = offset;
    bdBool status = bdDTLSHeader::deserialize(data, size, offset, newOffset);
    status = status && bdBytePacker::removeBuffer(data, size, newOffset,
                                                 newOffset, m_mac, m_macSize);
    if (!status)
        newOffset = offset;
    return status;
}
// NON_MATCHING: 1012 vs 1016; only the removeBasicType-failure exit differs: ours copies encryptedDataSize=0 first and tail-merges `status=0` into the setMACSize log block, the original copies status first (mov w27,wzr; mov w26,wzr; b).
#line 73
bdBool bdDTLSData::serialize(void * outData,
    const bdUInt outDataSize,
    const bdUInt outDataOffset,
    bdUInt & outDataNewOffset,
    const bdSequenceNumber & lastSequenceNumber,
    const bdUByte8 (&keyingMaterials)[32],
    const bdUByte8 *const inData,
    const bdUInt inDataSize,
    bdCypher *const cypher,
    bdHash *const hash,
    const bdCypherSuite cypherSuite)
{
    bdBool status = true;
    bdUByte8 iv[16];
    bdUInt inDataOffset = 0;


    bdUInt16 encryptedDataSize = 0;
    status = status && bdBytePacker::removeBasicType(
        inData, inDataSize, inDataOffset, inDataOffset, encryptedDataSize);

    status = status && setMACSize(cypherSuite);
    status = status && bdDTLSData::serialize(
        outData, outDataSize, outDataOffset, outDataNewOffset);

    status = status && bdBytePacker::appendBasicType(
        outData, outDataSize, outDataNewOffset, outDataNewOffset, encryptedDataSize);


    const bdUByte8 *const inPayloadStart = inData + inDataOffset;
    const bdUByte8 *const inPlainPayloadStart = inPayloadStart + encryptedDataSize;
    const bdUInt plainDataSize = inDataSize - sizeof(encryptedDataSize) - encryptedDataSize;


    const bdUInt encryptedPaddedDataSize = getPaddedDataSize(cypherSuite, encryptedDataSize);
    const bdUInt encryptedPaddingSize = encryptedPaddedDataSize - encryptedDataSize;
    const bdUInt outFinalOffset = outDataNewOffset + encryptedPaddedDataSize + plainDataSize;

    bdUByte8 *const outPayloadStart = static_cast<bdUByte8 *>(outData) + outDataNewOffset;
    bdUByte8 *const outPlainPayloadStart = outPayloadStart + encryptedPaddedDataSize;


    if (static_cast<bdUInt>(inPayloadStart - inData) > inDataSize ||
        static_cast<bdUInt>(inPlainPayloadStart - inData) > inDataSize ||
        encryptedDataSize + sizeof(encryptedDataSize) > inDataSize ||
        encryptedDataSize + sizeof(encryptedDataSize) + plainDataSize > inDataSize)
    {
        // Inferred: own failure exit (original's single failure block is a line-0 merge of two identical exits).
        bdUseVAArgs();
        outDataNewOffset = outDataOffset;
        return false;
    }
    if (!status ||
        outDataNewOffset - outDataOffset + encryptedPaddedDataSize + plainDataSize > outDataSize)
    {
        bdUseVAArgs();
        outDataNewOffset = outDataOffset;
        return false;
    }


    bdUInt ivNonceSize = 0;
    status = status && generateIVNonce(cypherSuite, lastSequenceNumber, keyingMaterials,
                             iv, ivNonceSize, hash);



    bdMemcpy(outPayloadStart, inPayloadStart, encryptedDataSize);


    bdMemset(outPayloadStart + encryptedDataSize, encryptedPaddingSize, encryptedPaddingSize);
    bdMemcpy(outPlainPayloadStart, inPlainPayloadStart, plainDataSize);


    bdUByte8 aadBuffer[1288];
    status = status && setAdditionalAuthData(cypherSuite, cypher, aadBuffer,
                                             encryptedDataSize, plainDataSize,
                                             inPlainPayloadStart);


    bdUInt encryptBlockSize = encryptedPaddedDataSize;
    status = status && cypher->encrypt(iv, outPayloadStart, outPayloadStart,
                                       encryptBlockSize);




    status = status && generateMessageAuthCode(
        cypherSuite, cypher, keyingMaterials, plainDataSize,
        encryptedPaddedDataSize, outDataOffset, outDataNewOffset,
        outData, outPayloadStart);


    status = status && bdDTLSData::serialize(
        outData, outDataSize, outDataOffset, outDataNewOffset);
    outDataNewOffset = status ? outFinalOffset : outDataOffset;
    return status;















}
#line 135

bdBool bdDTLSData::deserialize(
    const void * inData, const bdUInt inDataSize, const bdUInt inDataOffset,
    bdUInt & inDataNewOffset, const bdSequenceNumber & lastSequenceNumber,
    const bdUByte8 (&keyingMaterials)[32], bdUByte8 * outData,
    const bdUInt outDataMaxSize, bdUInt & outDataSize,
    bdCypher *const cypher, bdHash *const hash, const bdCypherSuite cypherSuite)
{
    bdBool status = true;
    const bdUInt cypherBlockSize = 16;
    bdUByte8 iv[16];
    bdUInt16 encryptedDataSize = 0;
    status = status && setMACSize(cypherSuite);
    status = status && bdDTLSData::deserialize(
        inData, inDataSize, inDataOffset, inDataNewOffset);
    status = status && bdBytePacker::removeBasicType(
        inData, inDataSize, inDataNewOffset, inDataNewOffset, encryptedDataSize);
    if (!status)
    {
        bdUseVAArgs();
        inDataNewOffset = inDataOffset;
        outDataSize = 0;
        return false;
    }

    const bdUInt paddedEncryptedDataSize = getPaddedDataSize(cypherSuite, encryptedDataSize);
    if (inDataNewOffset + paddedEncryptedDataSize > inDataSize)
    {
        bdUseVAArgs();
        inDataNewOffset = inDataOffset;
        outDataSize = 0;
        return false;
    }

    const bdUInt plainDataSize = inDataSize - (paddedEncryptedDataSize + inDataNewOffset);
    const bdUByte8 *const inPayloadStart = static_cast<const bdUByte8 *>(inData) + inDataNewOffset;
    const bdUByte8 *const inPlainPayloadStart = inPayloadStart + paddedEncryptedDataSize;
    bdUByte8 *const outPayloadStart = outData + sizeof(encryptedDataSize);
    bdUByte8 *const outPlainPayloadStart = outPayloadStart + encryptedDataSize;
    outDataSize = sizeof(encryptedDataSize) + encryptedDataSize + plainDataSize;

    if (outDataSize + cypherBlockSize > outDataMaxSize)
    {
        bdUseVAArgs();
        inDataNewOffset = inDataOffset;
        outDataSize = 0;
        return false;
    }

    bdUInt ivNonceSize = 0;
    status = generateIVNonce(cypherSuite, lastSequenceNumber, keyingMaterials,
                             iv, ivNonceSize, hash);
    status = status && processMessageAuthCode(
        cypherSuite, cypher, keyingMaterials, plainDataSize,
        paddedEncryptedDataSize, inDataOffset, inDataNewOffset,
        outDataSize, inData, inPayloadStart);
    bdUByte8 aadBuffer[1288];
    status = status && setAdditionalAuthData(cypherSuite, cypher, aadBuffer,
                                             encryptedDataSize, plainDataSize,
                                             inPlainPayloadStart);
    bdUInt encryptedBlockSize = paddedEncryptedDataSize;
    status = status && cypher->decrypt(iv, inPayloadStart, outPayloadStart,
                                       encryptedBlockSize);
    if (status)
        bdMemcpy(outPlainPayloadStart, inPlainPayloadStart, plainDataSize);

    bdUInt tmpOffset = 0;
    status = status && bdBytePacker::appendBasicType(
        outData, outDataMaxSize, tmpOffset, tmpOffset, encryptedDataSize);
    if (!status)
    {
        bdUseVAArgs();
        inDataNewOffset = inDataOffset;
        outDataSize = 0;
    }
    return status;
}

inline bdBool bdDTLSData::setMACSize(const bdCypherSuite cypherSuite)
{
    bdBool ok = true;
    bdMemset(m_mac, 0, sizeof(m_mac));
    switch (cypherSuite)
    {
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256:
        m_macSize = 12;
        break;
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256:
        m_macSize = 8;
        break;
    default:
        bdUseVAArgs(cypherSuite);
        ok = false;
        break;
    }
    return ok;
}

inline bdUInt bdDTLSData::getPaddedDataSize(const bdCypherSuite cypherSuite,
                                          const bdUInt encryptedDataSize)
{
    bdUInt result = encryptedDataSize;
    switch (cypherSuite)
    {
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256:
        break;
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256:
        result = (encryptedDataSize + 15) & ~15u;
        break;
    default:
        bdUseVAArgs(cypherSuite);
        break;
    }
    return result;
}

bdBool bdDTLSData::generateIVNonce(
    const bdCypherSuite cypherSuite, const bdSequenceNumber & lastSequenceNumber,
    const bdUByte8 (&keyingMaterials)[32], bdUByte8 (&iv)[16],
    bdUInt & ivNonceSize, bdHash *const hash)
{
    bdBool ok = false;
    ivNonceSize = 0;
    switch (cypherSuite)
    {
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256:
        {
            ok = hash->m_type == BD_HASH_SHA256;
            const bdInt32 seqNum = bdSequenceNumber(lastSequenceNumber, m_counter, 16).getValue();
            bdUInt bytesWritten = 0;
            ok = ok && bdBytePacker::appendBasicType(iv, 12, bytesWritten,
                                                     bytesWritten, seqNum);
            ok = ok && bdBytePacker::appendBasicType(iv, 12, bytesWritten,
                                                     bytesWritten, m_vtag);
            ok = ok && bytesWritten < 12;
            if (ok)
            {
                bdMemcpy(iv + bytesWritten, keyingMaterials, 12 - bytesWritten);
                ivNonceSize = 12;
            }
        }
        break;
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256:
        {
            ok = hash->m_type == BD_HASH_SHA256;
            const bdInt32 seqNum = bdSequenceNumber(lastSequenceNumber, m_counter, 16).getValue();
            const bdUInt BD_DTLS_IV_SEED_SIZE = 6;
            bdUByte8 ivSeed[BD_DTLS_IV_SEED_SIZE];
            bdUInt bytesWritten = 0;
            ok = ok && bdBytePacker::appendBasicType(ivSeed, BD_DTLS_IV_SEED_SIZE,
                                                     bytesWritten, bytesWritten, seqNum);
            ok = ok && bdBytePacker::appendBasicType(ivSeed, BD_DTLS_IV_SEED_SIZE,
                                                     bytesWritten, bytesWritten, m_vtag);
            bdUByte8 hashResult[32];
            bdUInt hashResultSize = sizeof(hashResult);
            ok = ok && hash->hash(ivSeed, BD_DTLS_IV_SEED_SIZE, hashResult, hashResultSize);
            ok = ok && hashResultSize >= sizeof(iv);
            if (ok)
            {
                bdMemcpy(iv, hashResult, sizeof(iv));
                ivNonceSize = sizeof(iv);
            }
        }
        break;
    default:
        bdUseVAArgs(cypherSuite);
        break;
    }
    if (!ok)
        bdUseVAArgs();
    return ok;
}

bdBool bdDTLSData::generateMessageAuthCode(
    const bdCypherSuite cypherSuite, bdCypher *const cypher,
    const bdUByte8 (&keyingMaterials)[32], const bdUInt plainDataSize,
    const bdUInt encryptedPaddedDataSize, const bdUInt outDataOffset,
    bdUInt & outDataNewOffset, void * outData, bdUByte8 *const outPayloadStart)
{
    bdBool ok = false;
    switch (cypherSuite)
    {
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256:
        {
            bdAuthenticationTag authTag;
            ok = static_cast<bdCypherAESGCM *>(cypher)->getAuthenticationTag(authTag, 12);
            m_macSize = authTag.getTagLength();
            bdMemcpy(m_mac, authTag.getTag(), m_macSize);
        }
        break;
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256:
        {
            bdHMacSHA256 hmac(keyingMaterials, 24);
            ok = hmac.process(static_cast<const bdUByte8 *>(outData), getSerializedSize());
            ok = ok && hmac.process(outPayloadStart - sizeof(bdUInt16),
                                    plainDataSize + encryptedPaddedDataSize + sizeof(bdUInt16));
            m_macSize = 8;
            bdBool getDataSuccess = hmac.getData(m_mac, m_macSize);
            if (!getDataSuccess)
            {
                bdUseVAArgs();
                outDataNewOffset = outDataOffset;
                return false;
            }
        }
        break;
    default:
        bdUseVAArgs();
        break;
    }
    return ok;
}

bdBool bdDTLSData::processMessageAuthCode(
    const bdCypherSuite cypherSuite, bdCypher *const cypher,
    const bdUByte8 (&keyingMaterials)[32], const bdUInt plainDataSize,
    const bdUInt paddedEncryptedDataSize, const bdUInt inDataOffset,
    bdUInt & inDataNewOffset, bdUInt & outDataSize, const void * inData,
    const bdUByte8 *const inPayloadStart)
{
    bdBool ok = false;
    switch (cypherSuite)
    {
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256:
        {
            bdAuthenticationTag authTag(m_mac, static_cast<bdUByte8>(m_macSize));
            ok = static_cast<bdCypherAESGCM *>(cypher)->setAuthenticationTag(authTag);
        }
        break;
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256:
        {
            bdHMacSHA256 hmac(keyingMaterials, 24);
            ok = hmac.process(static_cast<const bdUByte8 *>(inData), getSerializedSize());
            ok = ok && hmac.process(inPayloadStart - sizeof(bdUInt16),
                                    plainDataSize + paddedEncryptedDataSize + sizeof(bdUInt16));
            bdUInt tmpHmacResultSize = 8;
            bdUByte8 tmpHmac[8];
            ok = ok && hmac.getData(tmpHmac, tmpHmacResultSize);
            ok = ok && bdCryptoUtils::constTimeCompare(tmpHmac, m_mac, tmpHmacResultSize);
            if (!ok)
            {
                inDataNewOffset = inDataOffset;
                outDataSize = 0;
            }
        }
        break;
    default:
        bdUseVAArgs();
        break;
    }
    return ok;
}

inline bdBool bdDTLSData::setAdditionalAuthData(
    const bdCypherSuite cypherSuite, bdCypher *const cypher,
    bdUByte8 (&aadBuffer)[1288], const bdUInt16 encryptedDataSize,
    const bdUInt plainDataSize, const bdUByte8 *const plainData)
{
    bdBool ok = true;
    switch (cypherSuite)
    {
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256:
        {
            bdUInt aadOffset = 0;
            ok = bdDTLSHeader::serialize(aadBuffer, sizeof(aadBuffer), 0, aadOffset);
            ok = ok && bdBytePacker::appendBasicType(aadBuffer, sizeof(aadBuffer),
                                                    aadOffset, aadOffset, encryptedDataSize);
            ok = ok && bdBytePacker::appendBuffer(aadBuffer, sizeof(aadBuffer),
                                                 aadOffset, aadOffset, plainData, plainDataSize);
            ok = ok && static_cast<bdCypherAESGCM *>(cypher)->setAAD(aadBuffer, aadOffset);
        }
        break;
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256:
        break;
    default:
        bdUseVAArgs();
        ok = false;
        break;
    }
    return ok;
}
