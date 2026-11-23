// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformString/bdPlatformString.inl


// Functions defined in this file (bodies are not in the DWARF):
//   line    19  bdInt bdToLower(const bdInt c)  [inline]
//   line    31  bdNChar8 * bdStrlwr(bdNChar8 *const s)  [inline]
//   line    37  bdNChar8 * bdStrnlwr(bdNChar8 *const s, const bdUWord count)  [inline]
//   line    74  bdFloat32 bdStrtof32(const bdNChar8 *const s, bdNChar8 ** endptr)  [inline]
//   line    81  bdFloat64 bdStrtof64(const bdNChar8 *const s, bdNChar8 ** endptr)  [inline]
//   line    87  bdUWord bdStrlcat(bdNChar8 *const dst, const bdNChar8 *const src, const bdUWord size)  [inline]
//   line   104  const bdNChar8 * bdStrchr(const bdNChar8 *const s, const bdInt c)  [inlined into 3 functions]
//   line   111  bdNChar8 * bdStrchr(bdNChar8 *const s, const bdInt c)  [inlined into 2 functions]
//   line   118  const bdNChar8 * bdStrrchr(const bdNChar8 *const s, const bdInt c)  [inlined into 1 functions]
//   line   132  bdInt bdStrcmp(const bdNChar8 *const s1, const bdNChar8 *const s2)  [inlined into 2 functions]
//   line   139  bdInt bdStrncmp(const bdNChar8 *const s1, const bdNChar8 * s2, const bdInt len)  [inlined into 6 functions]
//   line   148  bdUWord bdStrlcpy(bdNChar8 *const dst, const bdNChar8 *const src, const bdUWord size)  [inlined into 23 functions]
//   line   165  bdUWord bdStrlen(const bdNChar8 *const s)  [inlined into 33 functions]
//   line   174  const bdNChar8 * bdStrstr(const bdNChar8 *const str, const bdNChar8 *const searchStr)  [inline]
//   line   181  bdNChar8 * bdStrstr(bdNChar8 *const str, const bdNChar8 *const searchStr)  [inlined into 7 functions]
//   line   190  bdInt bdSnprintf(bdNChar8 * buf, const bdUWord maxlen, const bdNChar8 * format, ...)  [0x7100b226d0]
//   line   206  bdInt bdFprintf(bdFileHandle stream, const bdNChar8 * format, ...)  [0x7100b23d00]
//   line   244  bdBool bdStrIsASCII(const bdNChar8 *const str)  [inline]
//   line   265  bdFileHandle bdGetStderr()  [inlined into 1 functions]

#pragma once

#include <string.h>
#include "../bdPlatformCoreTypes/bdPlatformCoreTypes.h"

// Only the helpers bdString needs are written so far (bodies: minimal libc wrappers).

// line 132
inline bdInt bdStrcmp(const bdNChar8* const s1, const bdNChar8* const s2)
{
    return strcmp(s1, s2);
}

// line 165
inline bdUWord bdStrlen(const bdNChar8* const s)
{
    return strlen(s);
}

#include "../bdPlatformMemory/bdPlatformMemory.inl"

// line 148. Reconstructed from bdBitBuffer::typeToString's inlined copy.
inline bdUWord bdStrlcpy(bdNChar8* const dst, const bdNChar8* const src, const bdUWord size)
{
    const bdUWord slen = bdStrlen(src);
    if (dst && size) {
        const bdUWord n = slen < size - 1 ? slen : size - 1;
        bdMemcpy(dst, src, n);
        dst[n] = 0;
    }
    return slen;
}

// line 139. Minimal libc wrapper, evidenced by bdUserAccountID comparisons.
inline bdInt bdStrncmp(const bdNChar8 *const s1, const bdNChar8 * s2, const bdInt len)
{
    return strncmp(s1, s2, len);
}

// line 104: const strchr overload observed in bdAddr::set.
inline const bdNChar8 *bdStrchr(const bdNChar8 *const s, const bdInt c)
{
    return strchr(s, c);
}

// line 118: const strrchr wrapper observed in bdLogSubscriber::publish.
inline const bdNChar8 *bdStrrchr(const bdNChar8 *const s, const bdInt c)
{
    return strrchr(s, c);
}

#include "bdPlatformString-unix.inl"
// line 190: an emitted inline copy is assigned to bdAddr.cpp.
inline bdInt bdSnprintf(bdNChar8 *buf, const bdUWord maxlen, const bdNChar8 *format, ...)
{
    bdVaList argPtr;
    va_start(argPtr, format);
    const bdInt ret = bdVsnprintf(buf, maxlen, format, argPtr);
    va_end(argPtr);
    return ret;
}
// line 206: an emitted copy sits with bdMallocMemory-std.cpp.
inline bdInt bdFprintf(bdFileHandle stream, const bdNChar8 *format, ...)
{
    bdVaList argPtr;
    va_start(argPtr, format);
    const bdInt ret = vfprintf(static_cast<FILE *>(stream), format, argPtr);
    va_end(argPtr);
    return ret;
}
// line 265
inline bdFileHandle bdGetStderr()
{
    return stderr;
}
// lines 111 and 181: the non-const overloads, observed in bdUPnP::extractDeviceInfo.
inline bdNChar8 *bdStrchr(bdNChar8 *const s, const bdInt c)
{
    return strchr(s, c);
}
inline bdNChar8 *bdStrstr(bdNChar8 *const str, const bdNChar8 *const searchStr)
{
    return strstr(str, searchStr);
}
