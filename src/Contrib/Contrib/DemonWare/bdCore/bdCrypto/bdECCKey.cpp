// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdECCKey.cpp
#include "../bdMemory/bdMemory-std.h"
#include "bdECCKey.h"
#include "../bdContainers/bdSingleton.h"
#include "../bdUtilities/bdTrulyRandom.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include "../../external/libtomcrypt/src/headers/tomcrypt.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_prng.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_math.h"




bdECCKey::bdECCKey()
    : m_status(BD_ECC_KEY_UNINITIALIZED)
{
    ltc_mp = ltm_desc;
}

bdECCKey::~bdECCKey()
{
    if (m_status == BD_ECC_KEY_INITIALIZED)
    {
        ecc_free(&m_key);
    }
    m_status = BD_ECC_KEY_UNINITIALIZED;
}

bdBool bdECCKey::init()
{
    bdBool result = false;
    if (m_status == BD_ECC_KEY_UNINITIALIZED)
    {
        bdBool failed = false;
        bdUByte8 entropy[128];
        if (register_prng(&yarrow_desc) == -1)
        {
            bdLogError("bdCore/bdECCKey", "Failed to register PRNG"); failed = true;
        }


        prng_state prng;
        bdInt error = CRYPT_OK;
        bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUByte8(entropy, sizeof(entropy));


        if (!failed &&
            (error = yarrow_start(&prng)) != CRYPT_OK)
        {
            bdLogError("bdCore/bdECCKey", "%s", error_to_string(error));
            failed = true;
        }

        if (!failed &&
            (error = yarrow_add_entropy(entropy, sizeof(entropy), &prng)) != CRYPT_OK)
        {
            bdLogError("bdCore/bdECCKey", "%s", error_to_string(error));
            failed = true;
        }

        if (!failed &&
            (error = yarrow_ready(&prng)) != CRYPT_OK)
        {
            bdLogError("bdCore/bdECCKey", "%s", error_to_string(error));
            failed = true;
        }

        if (!failed &&
            (error = ecc_make_key(&prng, find_prng("yarrow"), 28, &m_key)) != CRYPT_OK)
        {
            bdLogError("bdCore/bdECCKey", "%s", error_to_string(error));
            failed = true;
        }
        if (!failed)
        {
            m_status = BD_ECC_KEY_INITIALIZED; result = true;
        }
    }
    else
    {
        bdLogWarn("bdCore/bdECCKey", "Key already initialized");
    }

    return result;
}

bdBool bdECCKey::exportKey(bdUByte8 *const pubKey,
                           bdUInt & keylen)
{
    bdBool result = false;
    if (m_status == BD_ECC_KEY_INITIALIZED)
    {
        unsigned long len = keylen;
        const bdInt error = ecc_export(pubKey, &len, PK_PUBLIC, &m_key);
        if (error != CRYPT_OK)
        {
            bdLogError("bdCore/bdECCKey", "%s", error_to_string(error));
        }
        else
        {

            keylen = static_cast<bdUInt>(len);
            if (keylen < 100)
            {
                bdMemset(pubKey + keylen, 0, 100 - keylen);
            }

            keylen = 100; result = true;
        }
    }
    else
    {

        bdLogWarn("bdCore/bdECCKey", "Key not initialized");
    }
    return result;
}

bdBool bdECCKey::generateSharedSecret(const bdUByte8 *const publicKey,
                                      const bdUInt publicKeyLen,
                                      bdUByte8 * secretBuffer,
                                      const bdUInt secretBufferLen)
{
    bdBool result = true;
    bdECCKey pubKey;

    bdInt error = ecc_import(publicKey, publicKeyLen, &pubKey.m_key);
    if (error != CRYPT_OK)
    {
        bdLogError("bdCore/bdECCKey", "%s", error_to_string(error));
        return false;
    }




    if (m_status == BD_ECC_KEY_INITIALIZED)
    {
        unsigned long secretLen = secretBufferLen;

        error = ecc_shared_secret(&m_key, &pubKey.m_key, secretBuffer, &secretLen);
        if (error != CRYPT_OK)
        {
            bdLogError("bdCore/bdECCKey", "%s", error_to_string(error)); result = false;
        }
        else
        {



            if (secretLen < secretBufferLen)
            {
                bdMemset(secretBuffer + secretLen, 0, secretBufferLen - secretLen);
            }
        }
    }
    else
    {

        bdLogWarn("bdCore/bdECCKey", "Key not initialized");
    }


    ecc_free(&pubKey.m_key);



    return result;
}
bdECCKey::bdECCKeyStatus bdECCKey::getStatus() const
{
    return m_status;
}
