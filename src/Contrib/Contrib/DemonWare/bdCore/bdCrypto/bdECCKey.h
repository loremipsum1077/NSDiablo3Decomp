// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdECCKey.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_pk.h"
#include <stddef.h>
#include <stdint.h>

class bdECCKey {  // size 0x30
public:
    BD_CLASS_ALLOCATOR(bdECCKey);
    enum bdECCKeyStatus {
        BD_ECC_KEY_UNINITIALIZED = 0,
        BD_ECC_KEY_INITIALIZED = 1,
    };
    bdECCKey();
    ~bdECCKey();
    bdBool init();
    bdBool exportKey(bdUByte8 *const pubKey, bdUInt & keylen);
    bdBool generateSharedSecret(const bdUByte8 *const publicKey, const bdUInt publicKeyLen, bdUByte8 * secretBuffer, const bdUInt secretBufferLen);
    bdECCKey::bdECCKeyStatus getStatus() const;
protected:
    bdECCKey::bdECCKeyStatus m_status;  // 0x00
    ecc_key m_key;  // 0x08
};
static_assert(sizeof(bdECCKey) == 0x30, "bdECCKey");

