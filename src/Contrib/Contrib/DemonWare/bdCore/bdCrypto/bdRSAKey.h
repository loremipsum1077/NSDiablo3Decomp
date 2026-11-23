// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdRSAKey.h

#pragma once

#include "../bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_pk.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_prng.h"
#include <stddef.h>
#include <stdint.h>

class bdRSAKey {  // size 0x58
public:
    BD_CLASS_ALLOCATOR(bdRSAKey);
    enum bdRSAKeyStatus {
        BD_RSA_KEY_UNINITIALIZED = 0,
        BD_RSA_KEY_INITIALIZED = 1,
    };
    enum bdRSAKeySize {
        BD_RSA_KEY_SIZE_1024 = 1024,
        BD_RSA_KEY_SIZE_2048 = 2048,
        BD_RSA_KEY_SIZE_3072 = 3072,
        BD_RSA_KEY_SIZE_4096 = 4096,
    };
    bdRSAKey();
    ~bdRSAKey();
    bdBool init(bdRSAKey::bdRSAKeySize);
    bdBool import(const bdNChar8 *const keyBase64);
    bdBool importKey(const bdUByte8 * key, const bdULong keySize);
    bdBool exportKey(bdUByte8 *, bdULong &);
    bdUInt32 getKeySize();
    bdBool encrypt(const bdUByte8 *, const bdULong &, bdUByte8 *, bdULong &, const bdUByte8 *, const bdUInt32);
    bdBool decrypt(const bdUByte8 *, const bdULong &, bdUByte8 *, bdULong &, const bdUByte8 *, const bdUInt32);
    enum bdRSAPKCS {
        BD_RSA_PKCS_1_v1_5 = 1,
        BD_RSA_PKCS_1_OAEP = 2,
        BD_RSA_PKCS_1_PSS = 3,
    };
    bdBool generateSignatureSHA256(const void *, const bdULong, bdUByte8 *, bdULong &, bdRSAKey::bdRSAPKCS);
    bdBool verifySignatureSHA256(const bdUByte8 * signature, const void * data, const bdULong dataSize, bdRSAKey::bdRSAPKCS padding);
    bdBool verifySignatureTIGER192(const bdUByte8 *, const void *, const bdULong);
    bdBool verifyHash(const bdNChar8 *, const void *, const bdULong);
protected:
    bdRSAKey::bdRSAKeyStatus m_status;  // 0x00
    rsa_key m_key;  // 0x08
    bdUInt32 m_keySize;  // 0x50
private:
    bdBool getStatePRNG(prng_state &);
};
static_assert(sizeof(bdRSAKey) == 0x58, "bdRSAKey");


// Variables defined in this file:
//   line    22  static const bdUByte8 OAEP_SYSTEM_TAG[10]
