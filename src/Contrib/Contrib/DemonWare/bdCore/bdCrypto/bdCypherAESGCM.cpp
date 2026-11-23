// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCypherAESGCM.cpp

#include "bdCypherAESGCM.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_cipher.h"

// Invented name: an assert-style log macro whose DWARF leaves a block-scoped `exprValue`.
#define bdAssertMsg(expr, format, ...) \
    { \
        const bdBool exprValue = (expr); \
        if (!exprValue) \
        { \
            bdLogError("assert", format, ##__VA_ARGS__); \
        } \
    }


bdCypherAESGCM::bdCypherAESGCM()
    : bdCypher(16, BD_CYPHER_MODE_GCM),
      m_keySize(0),
      m_aad(NULL),
      m_aadSize(0)
{
    if (register_cipher(&aes_desc) == -1)
    {
        const bdInt error = CRYPT_ERROR;
        bdLogError("cypher", "Failed to register cypher: %s", error_to_string(error));
    }
}
bdCypherAESGCM::~bdCypherAESGCM()
{
}

bdBool bdCypherAESGCM::init(const bdUByte8 * key, bdUInt keySize)
{
    if (keySize != 16 && keySize != 24 && keySize != 32)
    {
        bdAssertMsg(false, "Invalid key size %u", keySize);
        return false;
    }

    bdBool result = false;
    bdInt error = CRYPT_OK;
    const bdInt cypherIndex = find_cipher_id(aes_desc.ID);
    if (cypherIndex == -1)
    {
        bdLogError("cypher", "Cypher not found");
    }
    else
    {
        bdAssertMsg(false, "GCM not supported");
        error = CRYPT_ERROR;
    }
    if (error != CRYPT_OK)
    {
        bdLogError("cypher", "init failed: %s", error_to_string(error));
    }
    return result;
}














bdBool bdCypherAESGCM::encrypt(const bdUByte8 * iv, const bdUByte8 * plainText, bdUByte8 * cypherText, bdUInt size)
{
    bdBool result = false;
    bdInt error = CRYPT_ERROR;
    bdAssertMsg(false, "GCM not supported");
    if (error != CRYPT_OK)
    {
        bdLogError("cypher", "encrypt failed: %s", error_to_string(error));
    }
    return result;
}



























bdBool bdCypherAESGCM::decrypt(const bdUByte8 * iv, const bdUByte8 * cypherText, bdUByte8 * plainText, bdUInt size)
{
    bdBool result = false;
    bdInt error = CRYPT_ERROR;
    bdAssertMsg(false, "GCM not supported");
    if (error != CRYPT_OK)
    {
        bdLogError("cypher", "decrypt failed: %s", error_to_string(error));
    }
    return result;
}

// Lines up to 466 held the libtomcrypt GCM implementation path, compiled out in this build.





















































































































































































































































































































































bdBool bdCypherAESGCM::setAAD(const bdUByte8 *const aad, const bdUInt aadSize)
{
    m_aad = aad;
    m_aadSize = aadSize;
    return true;
}




bdBool bdCypherAESGCM::setAuthenticationTag(const bdAuthenticationTag & authTag)
{
    m_authenticationTag = authTag;
    return true;
}

bdBool bdCypherAESGCM::getAuthenticationTag(bdAuthenticationTag & authTag, const bdUByte8 tagLength)
{
    bdBool ok = false;
    if (tagLength >= bdAuthenticationTag::BD_TAG_SIZE_MIN && tagLength <= bdAuthenticationTag::BD_TAG_SIZE_MAX)
    {
        authTag.setTag(m_authenticationTag.getTag(), tagLength);
        ok = true;
    }
    else
    {
        bdLogError("cypher", "Invalid tag length");
    }
    return ok;
}
