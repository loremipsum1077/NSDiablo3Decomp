// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCypher3Des.cpp
#include "bdCypher3Des.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_cipher.h"












bdCypher3Des::bdCypher3Des()
    : bdCypher(8, BD_CYPHER_MODE_CBC)
{
    if (register_cipher(&des3_desc) == -1)
    {
        bdLogError("cypher", "Failed to register cypher: %s", error_to_string(CRYPT_ERROR));
    }
}

bdCypher3Des::~bdCypher3Des()
{
}

bdBool bdCypher3Des::init(const bdUByte8 * key,
                          bdUInt keySize)
{
    bdBool result = false;


    if (keySize == 24)
    {

        const bdInt cypherIndex = find_cipher_id(des3_desc.ID);
        if (cypherIndex != -1)
        {
            bdUByte8 iv[8];
            const bdInt error = cbc_start(cypherIndex,
                                          iv,
                                          key,
                                          keySize,
                                          0,
                                          &m_cbc);
            if (error == CRYPT_OK)
            {
                result = true;
            }
            else
            {
                bdLogError("cypher", "init failed: %s", error_to_string(error));
            }
        }
        else
        {
            bdLogError("cypher", "Cypher not found");
        }
    }
    else
    {
        bdLogError("cypher", "Invalid key size %u", keySize);
    }

    return result;
}

bdBool bdCypher3Des::encrypt(const bdUByte8 * iv,
                             const bdUByte8 * plainText,
                             bdUByte8 * cypherText,
                             bdUInt size)
{
    bdBool result = true;

    bdInt error = cbc_setiv(iv, 8, &m_cbc);
    if (error == CRYPT_OK)
    {
        error = cbc_encrypt(plainText, cypherText, size, &m_cbc);
    }



    if (error != CRYPT_OK) { bdLogError("cypher", "encrypt failed: %s", error_to_string(error)); result = false; }













    return result;
}

bdBool bdCypher3Des::decrypt(const bdUByte8 * iv,
                             const bdUByte8 * cypherText,
                             bdUByte8 * plainText,
                             bdUInt size)
{
    bdBool result = true;

    bdInt error = cbc_setiv(iv, 8, &m_cbc);
    if (error == CRYPT_OK)
    {
        error = cbc_decrypt(cypherText, plainText, size, &m_cbc);
    }



    if (error != CRYPT_OK) { bdLogError("cypher", "decrypt failed: %s", error_to_string(error)); result = false; }













    return result;
}
