// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdUtility/bdJOSE/bdJOSE.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdJOSE {  // size 0x1
public:
    enum bdJWTRegisteredClaim {
        ISSUER = 0,
        SUBJECT = 1,
        AUDIENCE = 2,
        EXPIRATION = 3,
        NOT_BEFORE = 4,
        ISSUED_AT = 5,
        JWT_ID = 6,
    };
public:
    static const bdNChar8 * bdJWTRegisteredClaims[];
    enum bdJWTRegisteredHeader {
        ALGORITHM = 0,
        ENCRYPTION_ALGORITHM = 1,
        COMPRESSION_ALGORITHM = 2,
        JWK_SET_URL = 3,
        JSON_WEB_KEY = 4,
        KEY_ID = 5,
        X509_URL = 6,
        X509_CERT_CHAIN = 7,
        X509_CERT_SHA1_THUMBPRINT = 8,
        X509_CERT_SHA256_THUMBPRINT = 9,
        TYPE = 10,
        CONTENT_TYPE = 11,
        CRIT = 12,
    };
    BD_CLASS_ALLOCATOR(bdJOSE);
    static const bdNChar8 * bdJWTRegisteredHeaders[];
    enum bdJWSSigAlgorithm {
        HS256 = 0,
        HS384 = 1,
        HS512 = 2,
        RS256 = 3,
        RS384 = 4,
        RS512 = 5,
        ES256 = 6,
        ES384 = 7,
        ES512 = 8,
        PS256 = 9,
        PS384 = 10,
        PS512 = 11,
        none = 12,
    };
    static const bdNChar8 * bdJWSSigAlgorithms[];
    static const bdNChar8 * bdJWEKeyAlgorithms[];
    static const bdNChar8 * bdJWEEncAlgorithms[];
};
static_assert(sizeof(bdJOSE) == 0x1, "bdJOSE");

class bdJOSERestrictedHeaders {  // size 0x58
public:
    BD_CLASS_ALLOCATOR(bdJOSERestrictedHeaders);
    bdJOSERestrictedHeaders();
    void reset();
    const bdNChar8 * m_compression;  // 0x00
    const bdNChar8 * m_type;  // 0x08
    const bdNChar8 * m_contentType;  // 0x10
    const bdNChar8 * m_jwkSetURL;  // 0x18
    const bdNChar8 * m_jsonWebKey;  // 0x20
    const bdNChar8 * m_keyId;  // 0x28
    const bdNChar8 * m_x509URL;  // 0x30
    const bdNChar8 * m_x509CertificateChain;  // 0x38
    const bdNChar8 * m_x509Sha1Thumbprint;  // 0x40
    const bdNChar8 * m_x509Sha256Thumbprint;  // 0x48
    const bdNChar8 * m_crit;  // 0x50
};
static_assert(sizeof(bdJOSERestrictedHeaders) == 0x58, "bdJOSERestrictedHeaders");

