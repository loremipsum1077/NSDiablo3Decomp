// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdUtility/bdJOSE/bdJWS.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdJOSE.h"
#include <stddef.h>
#include <stdint.h>

class bdJWT;

class bdJWS {  // size 0x70
public:
    BD_CLASS_ALLOCATOR(bdJWS);
    bdJWS(const bdUByte8 *, bdUInt32);
    bdJWS(bdJWT &);
    ~bdJWS();
    bdBool setRegisteredHeader(bdJOSE::bdJWTRegisteredHeader, const bdNChar8 *);
    bdBool encode(bdNChar8 *, const bdUInt32, bdJOSE::bdJWSSigAlgorithm, const bdUByte8 *const, const bdUInt32);
    static bdBool verify(const bdNChar8 * jwsData, const bdUInt32 jwsDataLen, bdUByte8 * payload, bdUInt32 & payloadLen, const bdJOSE::bdJWSSigAlgorithm * allowedSigAlgorithms, const bdUInt32 allowedSigAlgorithmsLen, const bdUByte8 *const key, const bdUInt32 keyLen);
protected:
    static bdBool verifyJWSFormat(const bdNChar8 *const jwsData, const bdNChar8 *& jwsHeaderB64, bdUInt32 & jwsHeaderB64Len, const bdNChar8 *& jwsPayloadB64, bdUInt32 & jwsPayloadB64Len, const bdNChar8 *& jwsSignatureB64, bdUInt32 & jwsSignatureB64Len);
    bdBool encodeHeader(bdNChar8 *const, bdUInt32 &, bdJOSE::bdJWSSigAlgorithm);
    bdBool encodePayload(bdNChar8 **, bdUInt32 &);
    bdBool encodeSignature(bdNChar8 *const, bdUInt32 &);
    bdJWT * m_jwtPayload;  // 0x00
    const bdUByte8 * m_payload;  // 0x08
    bdUInt32 m_payloadLength;  // 0x10
    bdJOSERestrictedHeaders m_jwsHeaders;  // 0x18
};
static_assert(sizeof(bdJWS) == 0x70, "bdJWS");

