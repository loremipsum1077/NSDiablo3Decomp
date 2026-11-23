// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdRSAKey.cpp
#include "bdRSAKey.h"
#include "bdHashSHA256.h"
#include "../bdUtilities/bdBase64.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include "../../bdPlatform/bdPlatformString/bdPlatformString-unix.inl"
#include "../../external/libtomcrypt/src/headers/tomcrypt.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_hash.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_math.h"

// C++ linkage in the binary (_Z18rsa_verify_hash_exPKhmS0_miimPiP7Rsa_key); defined in another file.
int rsa_verify_hash_ex(const unsigned char * sig, unsigned long siglen, const unsigned char * hash, unsigned long hashlen, int padding, int hash_idx, unsigned long saltlen, int * stat, rsa_key * key);







bdRSAKey::bdRSAKey()
    : m_status(BD_RSA_KEY_UNINITIALIZED),
      m_keySize(0)
{
    ltc_mp = ltm_desc;
    bdMemset(&m_key, 0, sizeof(m_key));


    if (register_hash(&tiger_desc) == -1)
    {
        bdLogError("bdCore/bdRSAKey", "Failed to register tiger hash");
    }

    if (register_hash(&sha256_desc) == -1)
    {
        bdLogError("bdCore/bdRSAKey", "Failed to register sha256 hash");
    }
}

bdRSAKey::~bdRSAKey()
{

    if (m_status == BD_RSA_KEY_INITIALIZED)
    {
        rsa_free(&m_key);
    }
}


























































bdBool bdRSAKey::import(const bdNChar8 *const keyBase64)
{
    bdBool result = false;
    bdByte8 key[2348] = {0};

    const bdUInt length = bdStrnlen(keyBase64, 3133);

    const bdInt keySize = bdBase64::decode(keyBase64, length, key, sizeof(key));




    ltc_mp = ltm_desc;
    const bdInt error = rsa_import(reinterpret_cast<const bdUByte8 *>(key), keySize, &m_key);
    if (error == CRYPT_OK)
    {

        const bdInt bits = ltc_mp.count_bits(m_key.N);
        m_keySize = bits / 8;
        m_status = BD_RSA_KEY_INITIALIZED; result = true;
    }
    else
    {

        bdLogError("bdCore/bdRSAKey", "%s", error_to_string(error));
    }
    return result;
}

bdBool bdRSAKey::importKey(const bdUByte8 * key,
                           const bdULong keySize)
{
    bdBool result = false;

    ltc_mp = ltm_desc;
    if (m_status == BD_RSA_KEY_UNINITIALIZED)
    {
        const bdInt error = rsa_import(key, keySize, &m_key);
        if (error == CRYPT_OK)
        {

            const bdInt bits = ltc_mp.count_bits(m_key.N);
            m_keySize = bits / 8;
            m_status = BD_RSA_KEY_INITIALIZED; result = true;
        }
        else
        {

            bdLogError("bdCore/bdRSAKey", "%s", error_to_string(error));
        }
    }
    else
    {
        bdLogWarn("bdCore/bdRSAKey", "Key already initialized");
    }

    return result;
}































































































































































































bdBool bdRSAKey::verifySignatureSHA256(const bdUByte8 * signature,
                                       const void * data,
                                       const bdULong dataSize,
                                       bdRSAKey::bdRSAPKCS padding)
{
    bdBool result = false;

    ltc_mp = ltm_desc;

    bdHashSHA256 sha256;
    bdUByte8 hash[32];
    bdUInt hashSize = sizeof(hash);
    const bdBool hashed = sha256.hash(reinterpret_cast<const bdUByte8 *>(data), dataSize, hash, hashSize);




    if (!hashed)
    {

        bdLogError("bdCore/bdRSAKey", "Failed to hash data");
    }
    else
    {

        const bdInt hashIdx = find_hash("sha256");
        bdInt stat = 0;
        const bdInt signatureSize = m_keySize;
        bdBool ok = true;
        if (m_keySize == 128 || m_keySize == 256 || m_keySize == 384 || m_keySize == 512)
        {



            const bdInt error = rsa_verify_hash_ex(signature, signatureSize, hash,

                                                   hashSize, padding, hashIdx, 0, &stat,
                                                   &m_key);


            if (error != CRYPT_OK)
            {
                bdLogError("bdCore/bdRSAKey", "%s", error_to_string(error)); ok = false;
            }
        }
        else
        {


            bdLogWarn("bdCore/bdRSAKey", "Unexpected key size %u", m_keySize);
        }

        result = ok && stat == 1;
    }


    return result;
}
