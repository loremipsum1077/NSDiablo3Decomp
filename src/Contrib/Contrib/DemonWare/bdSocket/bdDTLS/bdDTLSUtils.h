// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdDTLSUtils.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdDTLSUtils {  // size 0x1
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    static const bdUByte8 BD_DTLS_LABEL_HEAD_LENGTH = 9;
    static const bdUByte8 BD_DTLS_LABEL_HEAD[];
    static const bdUByte8 BD_DTLS_LABEL_TAIL_MAX_LENGTH = 246;
    static const bdNChar8 BD_DTLS_LABEL_SERVER_APPLICATION_TRAFFIC_SECRET[];
    static const bdNChar8 BD_DTLS_LABEL_KEY[];
    static const bdNChar8 BD_DTLS_LABEL_IV[];
    static bdBool deriveSecret(const bdUByte8 (&inSecret)[32], const bdNChar8 * inLabel, const bdUByte8 * inMessages, const bdUInt inMessagesSize, bdUByte8 (&outBuffer)[32]);
    static bdBool hkdfExpandLabel(const bdUByte8 (&inSecret)[32], const bdNChar8 * inLabel, const bdUByte8 * inHashValue, const bdUInt16 hashLength, bdUByte8 * outBuffer, const bdUInt outBufferSize);
};
static_assert(sizeof(bdDTLSUtils) == 0x1, "bdDTLSUtils");

