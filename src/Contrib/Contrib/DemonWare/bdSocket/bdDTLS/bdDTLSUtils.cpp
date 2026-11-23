#include "bdDTLSUtils.h"
#include "../../bdCore/bdCrypto/bdCryptoUtils.h"
#include "../../bdCore/bdCrypto/bdHashSHA256.h"
#include "../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString-unix.inl"




const bdUByte8 bdDTLSUtils::BD_DTLS_LABEL_HEAD[] = "jzQ64jiNz";
const bdNChar8 bdDTLSUtils::BD_DTLS_LABEL_SERVER_APPLICATION_TRAFFIC_SECRET[] = "nN<0dtEg1xrlhp8OS18Uuu=0YxuM2zk2!";
const bdNChar8 bdDTLSUtils::BD_DTLS_LABEL_KEY[] = "VI)";
const bdNChar8 bdDTLSUtils::BD_DTLS_LABEL_IV[] = "ti";










bdBool bdDTLSUtils::deriveSecret(const bdUByte8 (&inSecret)[32], const bdNChar8 *inLabel, const bdUByte8 *inMessages, const bdUInt inMessagesSize, bdUByte8 (&outBuffer)[32]) {




    bdHashSHA256 hash;
    bdBool ok = false;
    bdUByte8 hashValue[32];

    bdUInt hashLength = 32;




    if (hash.hash(inMessages, inMessagesSize, hashValue, hashLength) && hashLength == 32) {

        ok = hkdfExpandLabel(inSecret, inLabel, hashValue, 32, outBuffer, 32);
    }



    return ok;
}
bdBool bdDTLSUtils::hkdfExpandLabel(const bdUByte8 (&inSecret)[32], const bdNChar8 *inLabel, const bdUByte8 *inHashValue, const bdUInt16 hashLength, bdUByte8 *outBuffer, const bdUInt outBufferSize) {





    bdUByte8 hkdfLabel[289];
    const bdUInt inLabelSize = bdStrnlen(inLabel, BD_DTLS_LABEL_TAIL_MAX_LENGTH);
    bdBool ok = false;




    bdUInt offset = 0;

    if (inLabelSize < BD_DTLS_LABEL_TAIL_MAX_LENGTH) {
        ok = bdBytePacker::appendBasicType(hkdfLabel, sizeof(hkdfLabel), 0, offset, hashLength) &&
             bdBytePacker::appendBuffer(hkdfLabel, sizeof(hkdfLabel), offset, offset, BD_DTLS_LABEL_HEAD, BD_DTLS_LABEL_HEAD_LENGTH) &&
             bdBytePacker::appendBuffer(hkdfLabel, sizeof(hkdfLabel), offset, offset, inLabel, inLabelSize) &&
             bdBytePacker::appendBuffer(hkdfLabel, sizeof(hkdfLabel), offset, offset, inHashValue, hashLength) &&
             bdCryptoUtils::hkdfExpand(inSecret, 32, hkdfLabel, offset, outBuffer, outBufferSize, BD_HASH_SHA256);
    }
    return ok;
}
