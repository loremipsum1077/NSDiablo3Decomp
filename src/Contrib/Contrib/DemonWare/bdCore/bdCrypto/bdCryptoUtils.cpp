#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCryptoUtils.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCryptoDynamic.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHMac.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHMacSHA1.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdHMacSHA256.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdContainers/bdSingleton.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdBytePacker.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdTrulyRandom.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include <stdlib.h>

















bdUInt32 bdCryptoUtils::getNewIVSeed()
{
    return bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUInt();
}

// Lines 33-160 held code with no trace in this 2.6.2 CU (the class's other
// members, e.g. getNewEncryptionKey and encrypt/decrypt, are not defined here).
// Blank lines keep each definition on its DWARF line.




























































































































bdBool bdCryptoUtils::constTimeCompare(const volatile bdUByte8 *volatile buffer1,
                                       const volatile bdUByte8 *volatile buffer2,
                                       const bdUWord size)
{

    bdUByte8 result = 0;
    for (bdUWord i = 0; i < size; ++i)
    {
        result |= buffer1[i] ^ buffer2[i];
    }

    return result == 0;
}

void bdCryptoUtils::zeroBuffer(void *const buffer,
                               const bdUWord size)
{

    void *(*volatile bdMemsetVolatile)(void *const, const bdInt, const bdUWord) = bdMemset;
    bdMemsetVolatile(buffer, 0, size);
}

bdBool bdCryptoUtils::hkdfExtractSHA1(const bdUByte8 (&inSalt)[20],
                                      const bdUByte8 *inIKM,
                                      const bdUInt inIKMSize,
                                      bdUByte8 (&outPRK)[20])
{



    bdHMacSHA1 hmac(inSalt, 20);
    bdBool status = hmac.process(inIKM, inIKMSize);
    bdUInt outSize = 20;
    status = status && hmac.getData(outPRK, outSize);


    return status;
}

bdBool bdCryptoUtils::hkdfExtractSHA256(const bdUByte8 (&inSalt)[32],
                                        const bdUByte8 *inIKM,
                                        const bdUInt inIKMSize,
                                        bdUByte8 (&outPRK)[32])
{



    bdHMacSHA256 hmac(inSalt, 32);
    bdBool status = hmac.process(inIKM, inIKMSize);
    bdUInt outSize = 32;
    status = status && hmac.getData(outPRK, outSize);


    return status;
}

bdBool bdCryptoUtils::hkdfExpand(const void *inPRK,
                                 const bdUInt inPRKSize,
                                 const void *inInfo,
                                 const bdUInt inInfoSize,
                                 void *outBuffer,
                                 const bdUInt outBufferSize,
                                 const bdHashAlgorithms hmacAlg)
{



    bdBool status = hmacAlg == BD_HASH_SHA256 || hmacAlg == BD_HASH_SHA1;
    bdUInt outBufferOffset = 0;




    const bdUInt hmacResultSize = hmacAlg == BD_HASH_SHA1 ? 20 : 32;

    status = inPRKSize > 8 && status && outBufferSize < 255 * hmacResultSize;

    bdUByte8 hmacResult[32];

    const bdUInt hmacInputMaxSize = 400;
    bdUByte8 hmacInput[hmacInputMaxSize];


    bdUInt hmacInputOffset = 0;


    status = status && inInfoSize + hmacResultSize + 1 < hmacInputMaxSize;


    bdUByte8 counter = 1;


    hmacInputOffset = 0;
    status = status && bdBytePacker::appendBuffer(hmacInput, hmacInputMaxSize,
                                                  hmacInputOffset, hmacInputOffset, inInfo, inInfoSize);
    status = status && bdBytePacker::appendBasicType<bdUByte8>(hmacInput, hmacInputMaxSize,
                                                               hmacInputOffset, hmacInputOffset, counter);

    {

        bdUInt tmp = hmacResultSize;
        bdDynamicHMAC hmac(hmacAlg, static_cast<const bdUByte8 *>(inPRK), inPRKSize);
        status = status && hmac.getHMAC().process(hmacInput, hmacInputOffset);
        status = status && hmac.getHMAC().getData(hmacResult, tmp);

        status = status && tmp == hmacResultSize;
        status = status && bdBytePacker::appendBuffer(outBuffer, outBufferSize,
                                                      outBufferOffset, outBufferOffset,
                                                      hmacResult,
                                                      hmacResultSize < outBufferSize - outBufferOffset ? hmacResultSize : outBufferSize - outBufferOffset);
    }


    while (status && outBufferOffset < outBufferSize)
    {
        counter += 1;
        hmacInputOffset = 0;
        status = bdBytePacker::appendBuffer(hmacInput, hmacInputMaxSize,
                                            hmacInputOffset, hmacInputOffset, hmacResult, hmacResultSize);

        status = status && bdBytePacker::appendBuffer(hmacInput, hmacInputMaxSize,
                                                      hmacInputOffset, hmacInputOffset, inInfo, inInfoSize);

        status = status && bdBytePacker::appendBasicType<bdUByte8>(hmacInput, hmacInputMaxSize,
                                                                   hmacInputOffset, hmacInputOffset, counter);


        bdUInt tmp = hmacResultSize;
        bdDynamicHMAC hmac(hmacAlg, static_cast<const bdUByte8 *>(inPRK), inPRKSize);
        status = status && hmac.getHMAC().process(hmacInput, hmacInputOffset);
        status = status && hmac.getHMAC().getData(hmacResult, tmp);

        status = status && tmp == hmacResultSize;

        status = status && bdBytePacker::appendBuffer(outBuffer, outBufferSize,
                                                      outBufferOffset, outBufferOffset, hmacResult,
                                                      hmacResultSize < outBufferSize - outBufferOffset ? hmacResultSize : outBufferSize - outBufferOffset);
    }

    return status;
}

extern "C" void libTomCryptQsort(void *base,
                                 size_t nmemb,
                                 size_t size,
                                 int (*compar)(const void *, const void *))
{
    qsort(base, nmemb, size, compar);
}
