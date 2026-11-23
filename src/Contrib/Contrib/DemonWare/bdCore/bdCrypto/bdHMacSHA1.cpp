#include "bdHMacSHA1.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_hash.h"
#include "../../external/libtomcrypt/src/headers/tomcrypt_mac.h"









bdHMacSHA1::bdHMacSHA1(const bdUByte8 *const key, const bdUInt keySize)
{

    if (register_hash(&sha1_desc) == -1)
    {
        bdLogError("hmac", "Failed to register hash");
    }

    const bdInt idx = find_hash("sha1");
    const bdInt status = hmac_init(&m_state, idx, key, keySize);

    if (status != CRYPT_OK)
    {
        bdLogError("hmac", "Failed to init: %s", error_to_string(status));
    }
}

bdHMacSHA1::~bdHMacSHA1()
{
}

bdBool bdHMacSHA1::process(const bdUByte8 *const data, const bdUInt length)
{
    bdBool ok = false;
    const bdInt status = hmac_process(&m_state, data, length);

    if (status != CRYPT_OK)
    {
        bdLogError("hmac", "Failed to process: %s", error_to_string(status));
    }
    else
    {
        ok = true;
    }
    return ok;
}
bdBool bdHMacSHA1::getData(bdUByte8 * dst, bdUInt & length)
{
    bdBool ok = false;
    if (length > 20)
    {
        bdLogError("hmac", "Buffer too small");
    }
    else
    {
        bdULong tmpLength = length;
        const bdInt status = hmac_done(&m_state, dst, &tmpLength);
        if (status != CRYPT_OK)
        {
            bdLogError("hmac", "Failed to get data: %s", error_to_string(status));
        }
        else
        {
            length = static_cast<bdUInt>(tmpLength);
            ok = true;
        }
    }
    return ok;
}
