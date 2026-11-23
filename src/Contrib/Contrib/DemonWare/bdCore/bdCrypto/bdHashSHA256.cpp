#include "bdHashSHA256.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_hash.h"



















bdInt bdHashSHA256::s_hash = -1;

bdHashSHA256::bdHashSHA256() : bdHash(BD_HASH_SHA256, 32)
{
    if (register_hash(&sha256_desc) == -1)
    {
        bdLogError("hash", "Failed to register hash");
    }

    if (s_hash == -1)
    {
        s_hash = find_hash("sha256");
    }
}


bdHashSHA256::~bdHashSHA256()
{
}

bdBool bdHashSHA256::hash(const bdUByte8 * data, const bdUInt dataSize, bdUByte8 * result, bdUInt & resultSize)
{
    bdBool ok = false;
    if (data != NULL && result != NULL && resultSize >= 32 && s_hash >= 0)
    {
        unsigned long resultSz = resultSize;
        const bdInt error = hash_memory(s_hash, data, dataSize, result, &resultSz);
        if (error == CRYPT_OK)
        {
            resultSize = 32;
            ok = true;
        }
    }
    if (!ok)
    {
        bdLogError("hash", "Failed to hash");
    }
    return ok;
}
