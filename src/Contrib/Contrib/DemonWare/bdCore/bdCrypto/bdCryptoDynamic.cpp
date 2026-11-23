#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCryptoDynamic.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCypher3Des.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCypherAES.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCypherAESGCM.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHMacSHA1.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHMacSHA256.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHashMD5.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHashSHA1.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHashSHA256.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHashTiger192.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"

// line 18
bdUInt bdCryptoDynamic::getCypherAlgorithmKeySize(bdCypherAlgorithms cypherAlgorithm)
{
    bdUInt keySize = 0;
    switch (cypherAlgorithm)
    {
    case BD_CYPHER_AES128_CBC:
        keySize = 16;
        break;
    case BD_CYPHER_AES192_CBC:
        keySize = 24;
        break;
    case BD_CYPHER_AES256_CBC:
        keySize = 32;
        break;
    case BD_CYPHER_3DES_CBC:
        keySize = 24;
        break;
    case BD_CYPHER_AES128_GCM:
        keySize = 16;
        break;
    case BD_CYPHER_AES192_GCM:
        keySize = 24;
        break;
    case BD_CYPHER_AES256_GCM:
        keySize = 32;
        break;
    default:
        bdLogError("crypto", "Unknown cypher algorithm");
        break;
    }
    return keySize;
}

// line 52
bdCypher* bdCryptoDynamic::allocateCypher(bdCypherAlgorithms cypherAlgorithm, void* alignedBuffer)
{
    bdCypher* cypherObject;
    switch (cypherAlgorithm)
    {
    case BD_CYPHER_AES128_CBC:
    case BD_CYPHER_AES192_CBC:
    case BD_CYPHER_AES256_CBC:
        cypherObject = alignedBuffer ? new (alignedBuffer) bdCypherAES() : new bdCypherAES();
        break;
    case BD_CYPHER_3DES_CBC:
        cypherObject = alignedBuffer ? new (alignedBuffer) bdCypher3Des() : new bdCypher3Des();
        break;
    case BD_CYPHER_AES128_GCM:
    case BD_CYPHER_AES192_GCM:
    case BD_CYPHER_AES256_GCM:
        cypherObject = alignedBuffer ? new (alignedBuffer) bdCypherAESGCM() : new bdCypherAESGCM();
        break;
    default:
        bdLogError("crypto", "Unknown cypher algorithm");
        cypherObject = NULL;
        break;
    }
    return cypherObject;
}

// line 79
bdHash* bdCryptoDynamic::allocateHash(bdHashAlgorithms hashAlgorithm, void* alignedBuffer)
{
    bdHash* hashObject;
    switch (hashAlgorithm)
    {
    case BD_HASH_SHA256:
        hashObject = alignedBuffer ? new (alignedBuffer) bdHashSHA256() : new bdHashSHA256();
        break;
    case BD_HASH_SHA1:
        hashObject = alignedBuffer ? new (alignedBuffer) bdHashSHA1() : new bdHashSHA1();
        break;
    case BD_HASH_TIGER192:
        hashObject = alignedBuffer ? new (alignedBuffer) bdHashTiger192() : new bdHashTiger192();
        break;
    case BD_HASH_MD5:
        hashObject = alignedBuffer ? new (alignedBuffer) bdHashMD5() : new bdHashMD5();
        break;
    default:
        bdLogError("crypto", "Unknown hash algorithm");
        hashObject = NULL;
        break;
    }
    return hashObject;
}

// line 105
bdHMac* bdCryptoDynamic::allocateHMAC(bdHashAlgorithms hashAlgorithm, const bdUByte8* const key, const bdUInt keySize, void* alignedBuffer)
{
    bdHMac* hmacHashObject;
    switch (hashAlgorithm)
    {
    case BD_HASH_SHA256:
        hmacHashObject = alignedBuffer ? new (alignedBuffer) bdHMacSHA256(key, keySize) : new bdHMacSHA256(key, keySize);
        break;
    case BD_HASH_SHA1:
        hmacHashObject = alignedBuffer ? new (alignedBuffer) bdHMacSHA1(key, keySize) : new bdHMacSHA1(key, keySize);
        break;
    default:
        bdLogError("crypto", "Unknown hash algorithm");
        hmacHashObject = NULL;
        break;
    }
    return hmacHashObject;
}

// line 128
bdDynamicCypher::bdDynamicCypher(bdCypherAlgorithms algorithm)
    : m_cypher(bdCryptoDynamic::allocateCypher(algorithm, m_cypherObjectBuffer))
{
}

// line 133
bdDynamicCypher::~bdDynamicCypher()
{
    if (m_cypher)
    {
        m_cypher->~bdCypher();
        m_cypher = NULL;
    }
}

// line 143
bdDynamicHash::bdDynamicHash(bdHashAlgorithms algorithm)
    : m_hash(bdCryptoDynamic::allocateHash(algorithm, m_hashObjectBuffer))
{
}

// line 148
bdDynamicHash::~bdDynamicHash()
{
    if (m_hash)
    {
        m_hash->~bdHash();
        m_hash = NULL;
    }
}

// line 158
bdDynamicHMAC::bdDynamicHMAC(bdHashAlgorithms algorithm, const bdUByte8* const key, const bdUInt keySize)
    : m_hmac(bdCryptoDynamic::allocateHMAC(algorithm, key, keySize, m_hmacObjectBuffer))
{
}

// line 165
bdDynamicHMAC::~bdDynamicHMAC()
{
    if (m_hmac)
    {
        m_hmac->~bdHMac();
        m_hmac = NULL;
    }
}
