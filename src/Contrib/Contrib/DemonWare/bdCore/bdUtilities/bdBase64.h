// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdBase64.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdBase64 {  // size 0x1
public:
    static void encode(const bdByte8 *, const bdUInt, bdNChar8 *, const bdUInt);
    static void encode(const bdUByte8 *, const bdUInt, bdNChar8 *, const bdUInt);
    static void encodeURLSafe(const bdByte8 * src, const bdUInt srcLen, bdNChar8 * dest, const bdUInt destLen);
    static void encodeURLSafe(const bdUByte8 *, const bdUInt, bdNChar8 *, const bdUInt);
    static bdUInt decode(const bdNChar8 * src, const bdUInt srcLen, bdByte8 * dest, const bdUInt destLen);
    static bdUInt decode(const bdNChar8 *, const bdUInt, bdUByte8 *, const bdUInt);
    static bdUInt decodeURLSafe(const bdNChar8 * src, const bdUInt srcLen, bdByte8 * dest, const bdUInt destLen);
    static bdUInt decodeURLSafe(const bdNChar8 *, const bdUInt, bdUByte8 *, const bdUInt);
    static bdBool base64URLEncodeAndStrip(const bdUByte8 *const, const bdUInt, bdNChar8 *const, bdUInt &);
    static bdBool base64URLPadAndDecode(const bdNChar8 *const src, const bdUInt32 srcLen, bdUByte8 *const dest, bdUInt32 & destLen);
protected:
    static void encode(const bdByte8 *, const bdUInt, bdNChar8 *, const bdUInt, const bdNChar8 *);
    static bdUInt decode(const bdNChar8 * src, const bdUInt srcLen, bdByte8 * dest, const bdUInt destLen, const bdNChar8 * alphabet);
};
static_assert(sizeof(bdBase64) == 0x1, "bdBase64");

