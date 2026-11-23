// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCypherAES.cpp
#include "bdCypherAES.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_cipher.h"












bdCypherAES::bdCypherAES()
    : bdCypher(16, BD_CYPHER_MODE_CBC),
      m_keySize(0)
{
    if (register_cipher(&aes_desc) == -1)
    {
        bdLogError("cypher", "Failed to register cypher: %s", error_to_string(CRYPT_ERROR));
    }
}

bdCypherAES::~bdCypherAES()
{
}

bdBool bdCypherAES::init(const bdUByte8 * key,
                         bdUInt keySize)
{
    if (keySize != 16 && keySize != 24 && keySize != 32)
    {
        bdLogError("cypher", "Invalid key size %u", keySize);
        return false;
    }

    bdBool result = false;

    const bdInt cypherIndex = find_cipher_id(aes_desc.ID);
    if (cypherIndex == -1)
    {
        bdLogError("cypher", "Cypher not found");
    }
    else
    {
        bdUByte8 iv[16];

        const bdInt error = cbc_start(cypherIndex,
                                      iv,
                                      key,
                                      keySize,
                                      0,
                                      &m_cbc);


        if (error == CRYPT_OK)
        {
            m_keySize = keySize; result = true;
        }
        else
        {

            bdLogError("cypher", "init failed: %s", error_to_string(error));
        }
    }

    return result;
}
bdBool bdCypherAES::encrypt(const bdUByte8 * iv,
                            const bdUByte8 * plainText,
                            bdUByte8 * cypherText,
                            bdUInt size)
{
    bdBool result = false;
    bdInt error = CRYPT_ERROR;

    if (m_keySize != 0)
    {
        error = cbc_setiv(iv, 16, &m_cbc);
    }
    else
    {
        bdLogError("cypher", "Cypher not initialized");
    }

    if (error == CRYPT_OK)
    {
        error = cbc_encrypt(plainText, cypherText, size, &m_cbc);
    }
    else
    {




        bdLogError("cypher", "setiv failed: %s", error_to_string(error));
    }

    if (error == CRYPT_OK)
    {
        result = true;
    }
    else
    {
        bdLogError("cypher", "encrypt failed: %s", error_to_string(error));
    }

    return result;
}

bdBool bdCypherAES::decrypt(const bdUByte8 * iv,
                            const bdUByte8 * cypherText,
                            bdUByte8 * plainText,
                            bdUInt size)
{
    bdBool result = false;
    bdInt error = CRYPT_ERROR;

    if (m_keySize != 0)
    {
        error = cbc_setiv(iv, 16, &m_cbc);
    }
    else
    {
        bdLogError("cypher", "Cypher not initialized");
    }

    if (error == CRYPT_OK)
    {
        error = cbc_decrypt(cypherText, plainText, size, &m_cbc);
    }
    else
    {




        bdLogError("cypher", "setiv failed: %s", error_to_string(error));
    }

    if (error == CRYPT_OK)
    {
        result = true;
    }
    else
    {
        bdLogError("cypher", "decrypt failed: %s", error_to_string(error));
    }

    return result;
}
