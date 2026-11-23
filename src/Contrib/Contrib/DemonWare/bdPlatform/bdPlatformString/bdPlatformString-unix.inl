// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformString/bdPlatformString-unix.inl


// Functions defined in this file (bodies are not in the DWARF):
//   line    12  bdByte8 bdStrtoi8(const bdNChar8 *const s, bdNChar8 ** endptr, const bdUInt radix)  [inline]
//   line    31  bdUByte8 bdStrtoui8(const bdNChar8 *const s, bdNChar8 ** endptr, const bdUInt radix)  [inline]
//   line    45  bdInt32 bdStrtoi32(const bdNChar8 *const s, bdNChar8 ** endptr, const bdUInt radix)  [inlined into 1 functions]
//   line    67  bdUInt32 bdStrtoui32(const bdNChar8 *const s, bdNChar8 ** endptr, const bdUInt radix)  [inlined into 2 functions]
//   line    84  bdInt64 bdStrtoi64(const bdNChar8 *const s, bdNChar8 ** endptr, const bdUInt32 radix)  [inline]
//   line    92  bdUInt64 bdStrtoui64(const bdNChar8 *const s, bdNChar8 ** endptr, const bdUInt32 radix)  [inlined into 1 functions]
//   line   100  bdUWord bdStrnlen(const bdNChar8 *const s, const bdUWord maxLen)  [inlined into 17 functions]
//   line   109  bdInt32 bdStrcasecmp(const bdNChar8 *const s1, const bdNChar8 *const s2)  [inline]
//   line   124  bdInt bdVsnprintf(bdNChar8 * buf, const bdUWord maxlen, const bdNChar8 * format, bdVaList argPtr)  [inlined into 1 functions]

#pragma once

#include <string.h>
#include "../bdPlatformCoreTypes/bdPlatformCoreTypes.h"

// line 100
inline bdUWord bdStrnlen(const bdNChar8* const s, const bdUWord maxLen)
{
    const bdNChar8* const pos = static_cast<const bdNChar8*>(memchr(s, '\0', maxLen));
    const bdUWord slen = pos ? static_cast<bdUWord>(pos - s) : maxLen;
    return slen;
}

#include <stdlib.h>
#include <errno.h>
// line 67: preserve the original 32-bit saturation and errno on LP64.
inline bdUInt32 bdStrtoui32(const bdNChar8 *const s, bdNChar8 **endptr, const bdUInt radix)
{
    unsigned long result = strtoul(s, endptr, radix);
    if (result > 0xffffffffUL) {
        errno = ERANGE;
        result = 0xffffffffUL;
    }
    return static_cast<bdUInt32>(result);
}

#include "../bdPlatformVarArgs/bdPlatformVarArgs.h"
#include <stdio.h>
// line 124: libc formatting wrapper observed in bdSnprintf.
inline bdInt bdVsnprintf(bdNChar8 *buf, const bdUWord maxlen, const bdNChar8 *format, bdVaList argPtr)
{
    const bdInt ret = vsnprintf(buf, maxlen, format, argPtr);
    return ret;
}
