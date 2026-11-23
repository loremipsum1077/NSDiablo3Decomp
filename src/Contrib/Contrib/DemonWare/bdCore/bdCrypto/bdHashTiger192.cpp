#include "bdHashTiger192.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_hash.h"















bdInt bdHashTiger192::s_hash = -1;

bdHashTiger192::bdHashTiger192() : bdHash(BD_HASH_TIGER192, 24)
{
    if (register_hash(&tiger_desc) == -1)
    {
        bdLogError("hash", "Failed to register hash");
    }

    if (s_hash == -1)
    {
        s_hash = find_hash("tiger");
    }
}

bdHashTiger192::~bdHashTiger192()
{
}

bdBool bdHashTiger192::hash(const bdUByte8 * data, const bdUInt dataSize, bdUByte8 * result, bdUInt & resultSize)
{
    bdBool ok = false;
    if (data != NULL && result != NULL && resultSize >= 24 && s_hash >= 0)
    {
        unsigned long resultSz = resultSize;
        const bdInt error = hash_memory(s_hash, data, dataSize, result, &resultSz);
        if (error == CRYPT_OK)
        {
            resultSize = 24;
            ok = true;
        }
    }
    if (!ok)
    {
        bdLogError("hash", "Failed to hash");
    }
    return ok;
}
