// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCryptoUtils.h

#pragma once

#include "bdHash.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdCryptoUtils {  // size 0x1
public:
    static void getNewEncryptionKey(bdUByte8 *);
    static bdUInt32 getNewIVSeed();
    static void calculateInitialVector(const bdUInt32, bdUByte8 *);
    static bdBool encrypt(const void *, const void *, const void *, void *, const bdUInt);
    static bdBool decrypt(const void *, const void *, const void *, void *, const bdUInt);
    static bdBool encryptAES(const void *, const void *, const void *, void *, bdUInt, bdUInt32);
    static bdBool decryptAES(const void *, const void *, const void *, void *, bdUInt, bdUInt);
    static bdBool constTimeCompare(const volatile bdUByte8 *volatile buffer1, const volatile bdUByte8 *volatile buffer2, const bdUWord size);
    static void zeroBuffer(void *const buffer, const bdUWord size);
    static bdBool hkdfExtractSHA1(const bdUByte8 (&inSalt)[20], const bdUByte8 * inIKM, const bdUInt inIKMSize, bdUByte8 (&outPRK)[20]);
    static bdBool hkdfExtractSHA256(const bdUByte8 (&inSalt)[32], const bdUByte8 * inIKM, const bdUInt inIKMSize, bdUByte8 (&outPRK)[32]);
    static bdBool hkdfExpand(const void * inPRK, const bdUInt inPRKSize, const void * inInfo, const bdUInt inInfoSize, void * outBuffer, const bdUInt outBufferSize, const bdHashAlgorithms hmacAlg);
};
static_assert(sizeof(bdCryptoUtils) == 0x1, "bdCryptoUtils");

