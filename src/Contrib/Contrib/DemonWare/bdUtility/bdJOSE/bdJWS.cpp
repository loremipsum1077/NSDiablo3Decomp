#include "bdJWS.h"
#include "../../bdCore/bdUtilities/bdBase64.h"
#include "../../bdCore/bdCrypto/bdHMacSHA256.h"
#include "../../bdCore/bdCrypto/bdRSAKey.h"
#include "../../bdCore/bdCrypto/bdCryptoUtils.h"
#include "../../bdJSON/bdJSONDeserializer.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"

#line 19
static const bdUInt32 BD_JWS_HEADER_MAX_SIZE = 512;

#line 408
bdBool bdJWS::verifyJWSFormat(const bdNChar8* const jwsData,
    const bdNChar8*& jwsHeaderB64, bdUInt32& jwsHeaderB64Len,
    const bdNChar8*& jwsPayloadB64, bdUInt32& jwsPayloadB64Len,
    const bdNChar8*& jwsSignatureB64, bdUInt32& jwsSignatureB64Len)
{
#line 418
    bdBool ok = true;
    if (!jwsData) {
        ok = false;
        bdUseVAArgs();
    }
    if (ok) {
        jwsHeaderB64 = jwsData;
#line 432
        const bdNChar8* endHeader = bdStrchr(jwsHeaderB64, '.');
        ok = endHeader != NULL;
        if (ok) jwsHeaderB64Len = endHeader - jwsHeaderB64;
    }
    if (ok) {
#line 444
        jwsPayloadB64 = jwsHeaderB64 + jwsHeaderB64Len + 1;
#line 447
        const bdNChar8* endPayload = bdStrchr(jwsPayloadB64, '.');
        ok = endPayload != NULL;
        if (ok) jwsPayloadB64Len = endPayload - jwsPayloadB64;
    }
    if (ok) {
#line 459
        jwsSignatureB64 = jwsPayloadB64 + jwsPayloadB64Len + 1;
#line 463
        const bdNChar8* endSignature = bdStrchr(jwsSignatureB64, '.');
        ok = endSignature == NULL;
        if (ok) jwsSignatureB64Len = bdStrlen(jwsSignatureB64);
    }
    if (!ok) bdUseVAArgs();
    return ok;
}


#line 182
bdBool bdJWS::verify(const bdNChar8* jwsData, const bdUInt32 jwsDataLen,
    bdUByte8* payload, bdUInt32& payloadLen,
    const bdJOSE::bdJWSSigAlgorithm* allowedSigAlgorithms,
    const bdUInt32 allowedSigAlgorithmsLen,
    const bdUByte8* const key, const bdUInt32 keyLen)
{
    const bdNChar8* headerB64 = NULL;
    const bdNChar8* payloadB64 = NULL;
    const bdNChar8* signatureB64 = NULL;
#line 201
    bdUInt32 headerB64Len = 0;
    bdUInt32 payloadB64Len = 0;
    bdUInt32 signatureB64Len = 0;
    bdBool ok = verifyJWSFormat(jwsData, headerB64, headerB64Len,
        payloadB64, payloadB64Len, signatureB64, signatureB64Len);
    bdUByte8 headerBuffer[BD_JWS_HEADER_MAX_SIZE];
#line 212
    bdUInt32 headerLen = BD_JWS_HEADER_MAX_SIZE - 1;
    ok = ok && bdBase64::base64URLPadAndDecode(headerB64, headerB64Len, headerBuffer, headerLen);
    if (ok) headerBuffer[headerLen] = 0;
    else bdUseVAArgs();
#line 231
    bdJSONDeserializer jwsProtectedHeader;
    ok = ok && jwsProtectedHeader.parse(reinterpret_cast<const bdNChar8*>(headerBuffer));
    if (!ok) bdUseVAArgs();
#line 240
    bdNChar8 sigAlgorithmHeader[16] = {0};
    bdJOSE::bdJWSSigAlgorithm sigAlgorithm = bdJOSE::none;
    ok = ok && jwsProtectedHeader.getString(bdJOSE::bdJWTRegisteredHeaders[bdJOSE::ALGORITHM], sigAlgorithmHeader, sizeof(sigAlgorithmHeader));
    if (ok) {
#line 246
        bdBool matched = false;
        for (bdUInt i = 0; i < allowedSigAlgorithmsLen; ++i) {
            if (bdStrcmp(bdJOSE::bdJWSSigAlgorithms[allowedSigAlgorithms[i]], sigAlgorithmHeader) == 0) {
                sigAlgorithm = allowedSigAlgorithms[i];
                matched = true;
                break;
            }
        }
        if (!matched) {
            bdUseVAArgs(sigAlgorithm);
            ok = false;
        }
    } else {
        bdUseVAArgs();
    }
    // Original 272:7 (two tabs): the crit test is an if inside if (ok), not an assignment.
    if (ok) {
        if (jwsProtectedHeader.hasKey(bdJOSE::bdJWTRegisteredHeaders[bdJOSE::CRIT])) {
            bdUseVAArgs();
            ok = false;
        }
    }
    if (ok && payload && payloadLen) {
        bdUseVAArgs();
        ok = bdBase64::base64URLPadAndDecode(payloadB64, payloadB64Len, payload, payloadLen);
    }
    if (ok) {
#line 290
        const bdUByte8* concatenation = reinterpret_cast<const bdUByte8*>(jwsData);
        bdUInt32 concatenationLen = headerB64Len + payloadB64Len + 1;
        switch (sigAlgorithm) {
        case bdJOSE::HS256: {
            bdUByte8 signature[33];
            bdUInt32 signatureLen = 32;
            ok = bdBase64::base64URLPadAndDecode(signatureB64, signatureB64Len, signature, signatureLen);
            bdUByte8 newSignature[32];
            bdUInt signatureIntLen = 32;
            bdHMacSHA256 hmacSha256(key, keyLen);
            ok = ok && hmacSha256.process(concatenation, concatenationLen);
            ok = ok && hmacSha256.getData(newSignature, signatureIntLen);
            ok = ok && bdCryptoUtils::constTimeCompare(signature, newSignature, 32);
            break;
        }
        case bdJOSE::RS256:
        case bdJOSE::PS256: {
            bdUByte8 signature[256];
            bdUInt32 signatureLen = 256;
            ok = bdBase64::base64URLPadAndDecode(signatureB64, signatureB64Len, signature, signatureLen);
            const bdRSAKey::bdRSAPKCS pkcsPadding = sigAlgorithm == bdJOSE::RS256 ? bdRSAKey::BD_RSA_PKCS_1_v1_5 : bdRSAKey::BD_RSA_PKCS_1_PSS;
            bdRSAKey rsaVerificationKey;
            if (keyLen) ok = ok && rsaVerificationKey.importKey(key, keyLen);
            else ok = ok && rsaVerificationKey.import(reinterpret_cast<const bdNChar8*>(key));
            ok = ok && rsaVerificationKey.verifySignatureSHA256(signature, concatenation, concatenationLen, pkcsPadding);
            break;
        }
        case bdJOSE::none:
            ok = signatureB64Len == 0 && *signatureB64 == 0;
            if (!ok) bdUseVAArgs();
            break;
        default:
            bdUseVAArgs(bdJOSE::bdJWSSigAlgorithms[sigAlgorithm]);
            ok = false;
        }
        bdUseVAArgs();
    }
    if (!ok) bdUseVAArgs();
    return ok;
}
