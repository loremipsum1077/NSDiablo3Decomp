#include "bdHashMD5.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_hash.h"



















bdInt bdHashMD5::s_hash = -1;

bdHashMD5::bdHashMD5() : bdHash(BD_HASH_MD5, 16)
{
    if (register_hash(&md5_desc) == -1)
    {
        bdLogError("hash", "Failed to register hash");
    }

    if (s_hash == -1)
    {
        s_hash = find_hash("md5");
    }
}


bdHashMD5::~bdHashMD5()
{
}

bdBool bdHashMD5::hash(const bdUByte8 * data, const bdUInt dataSize, bdUByte8 * result, bdUInt & resultSize)
{
    bdBool ok = false;
    if (data != NULL && result != NULL && resultSize >= 16 && s_hash >= 0)
    {
        unsigned long resultSz = resultSize;
        const bdInt error = hash_memory(s_hash, data, dataSize, result, &resultSz);
        if (error == CRYPT_OK)
        {
            resultSize = 16;
            ok = true;
        }
    }
    if (!ok)
    {
        bdLogError("hash", "Failed to hash");
    }
    return ok;
}
