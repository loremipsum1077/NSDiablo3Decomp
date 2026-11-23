#include "bdSecurityInfo.h"
#include "bdSecurityID.h"
#include "bdSecurityKey.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"
// Blank lines keep the original line numbers









bdUInt bdSecurityInfo::toString(const bdSecurityID& id, bdNChar8* buffer, bdUInt len)
{
    return bufferToString(id.ab, sizeof(id.ab), buffer, len);
}

bdUInt bdSecurityInfo::toString(const bdSecurityKey& key, bdNChar8* buffer, bdUInt len)
{
    return bufferToString(key.ab, sizeof(key.ab), buffer, len);
}

bdUInt bdSecurityInfo::bufferToString(const bdUByte8* buffer, const bdUInt bufferLen, bdNChar8* outBuffer, const bdUInt outBufferLen)
{
    bdNChar8* cur = outBuffer;



    for (bdUInt i = 0; i < bufferLen; ++i)
    {
        if (i && !(i % 4))
        {
            cur += bdSnprintf(cur, outBufferLen - (cur - outBuffer), " ");
        }

        if (bdSnprintf(cur, outBufferLen - (cur - outBuffer), "%02hhx", buffer[i]) != 2)
            return 0;
        cur += 2;
    }








    return cur - outBuffer;
}
