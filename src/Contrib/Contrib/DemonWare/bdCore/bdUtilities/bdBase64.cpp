// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdBase64.cpp
#include "bdBase64.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"













static const bdNChar8 B64_PADDING = '=';




static const bdNChar8 * standardBase64Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";





static const bdNChar8 * urlsafeBase64Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";









































































bdBool bdBase64::base64URLPadAndDecode(const bdNChar8 *const src,
                                       const bdUInt32 srcLen,
                                       bdUByte8 *const dest,
                                       bdUInt32 & destLen)
{
    bdBool result = false;






    bdUInt32 length = srcLen;
    const bdNChar8 * end = src + (srcLen - 1);
    while (*end == B64_PADDING)
    {
        --length; --end;
    }


    const bdUInt32 padding = (length % 4 != 0) ? 4 - (length % 4) : 0;





    if (destLen >= (length * 3) / 4)
    {

        const bdUInt32 alignedLength = length & ~3u;
        bdUInt32 decoded = decodeURLSafe(src, alignedLength, reinterpret_cast<bdByte8 *>(dest), destLen);


        if (padding != 0)
        {
            const bdUInt32 remaining = destLen - decoded;
            const bdUInt32 tailLength = 4 - padding;
            bdNChar8 tail[4] = { B64_PADDING, B64_PADDING, B64_PADDING, B64_PADDING };
            bdMemcpy(tail, src + (length - tailLength), tailLength);
            decoded += decodeURLSafe(tail, 4, reinterpret_cast<bdByte8 *>(dest + decoded), remaining);
        }

        if (decoded <= destLen)
        {
            destLen = decoded; result = true;
        }
    }
    else
    {

        bdLogWarn("base64", "Destination buffer too small: %u, need %u", destLen, (srcLen * 3) / 4);
    }


    return result;
}

















































































































bdUInt bdBase64::decode(const bdNChar8 * src,
                        const bdUInt srcLen,
                        bdByte8 * dest,
                        const bdUInt destLen)
{
    return decode(src, srcLen, dest, destLen, standardBase64Alphabet);
}









inline bdUInt bdBase64::decodeURLSafe(const bdNChar8 * src,
                               const bdUInt srcLen,
                               bdByte8 * dest,
                               const bdUInt destLen)
{
    return decode(src, srcLen, dest, destLen, urlsafeBase64Alphabet);
}

static bdUInt findIndex(const bdNChar8 c, const bdNChar8 * cvt);
static const bdNChar8 * skipWhitespace(const bdNChar8 * ptr);






bdUInt bdBase64::decode(const bdNChar8 * src,
                        const bdUInt srcLen,
                        bdByte8 * dest,
                        const bdUInt destLen,
                        const bdNChar8 * alphabet)
{




















    bdUInt destIndex = 0;
    for (const bdNChar8 * ptr = skipWhitespace(src); ptr != src + srcLen; ptr = skipWhitespace(ptr + 1))  // INFERENCE: DWARF puts both skipWhitespace calls on line 331 (init and step); the end test is src + srcLen
    {
        bdNChar8 c = *ptr;
        if (c == '\0' || c == B64_PADDING)
        {
            break;
        }
        c = findIndex(c, alphabet);  // INFERENCE: c and c1 hold the 6-bit indices then the output bytes (only two char locals in the DWARF)

        ptr = skipWhitespace(ptr + 1);
        bdNChar8 c1 = findIndex(*ptr, alphabet);
        c = (c << 2) | ((c1 & 0x30) >> 4);


        if (destIndex >= destLen)
        {
            break;
        }
        dest[destIndex] = c;
        ++destIndex;

        ptr = skipWhitespace(ptr + 1);
        if (static_cast<bdUInt>(ptr - src) < srcLen)  // INFERENCE: 32-bit ptr - src compare (sub w8,w19,w23; cmp w8,w22 at 0xc02418)
        {
            c = *ptr;
            if (c == '\0' || c == B64_PADDING)
            {
                break;
            }
            c = findIndex(c, alphabet);
            c1 = (c1 << 4) | ((c & 0x3c) >> 2);

            if (destIndex >= destLen)
            {
                break;
            }
            dest[destIndex] = c1;
            ++destIndex;
        }

        ptr = skipWhitespace(ptr + 1);
        if (static_cast<bdUInt>(ptr - src) < srcLen)
        {
            c1 = *ptr;
            if (c1 == '\0' || c1 == B64_PADDING)
            {
                break;
            }
            c1 = findIndex(c1, alphabet);
            c = (c << 6) | c1;

            if (destIndex >= destLen)
            {
                break;
            }
            dest[destIndex] = c;
            ++destIndex;
        }
    }

    if (destIndex > destLen)
    {

        bdLogWarn("bdBase64", "Decode overran the destination: %u > %u", destLen, destIndex);
    }
    return destIndex;
}






static bdUInt findIndex(const bdNChar8 c, const bdNChar8 * cvt)
{
    bdUInt i = 0;


    if (c >= 'A' && c <= 'Z')
    {
        i = c - 'A';
    }
    else if (c >= 'a' && c <= 'z')
    {
        i = c - 'a' + 26;
    }
    else if (c >= '0' && c <= '9')
    {
        i = c - '0' + 52;
    }
    else if (cvt[62] == c)
    {
        i = 62;
    }
    else if (cvt[63] == c)
    {
        i = 63;
    }
    else
    {
        bdLogWarn("bdBase64", "Invalid base64 character: %c", c);  // INFERENCE: c is the log argument (the byte is already in w0 at the call, 0xc02378)
    }
    return i;
}














static const bdNChar8 * skipWhitespace(const bdNChar8 * ptr)
{
    if (ptr == NULL)
    {
        return NULL;
    }
    while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\v' || *ptr == '\f' || *ptr == '\r')
    {
        ++ptr;
    }
    return ptr;
}
