// Reconstructed subset of the original NX SDK math.h, lines 47-50.
#pragma once
// Invented include guard shared with the existing compatibility definition.
#ifndef D3_NX_FLOAT_BITS_DEFINED
#define D3_NX_FLOAT_BITS_DEFINED
static inline unsigned int __FLOAT_BITS(float __f)
{
    union {
        float __f;
        unsigned int __i;
    } __u;
    __u.__f = __f;
    return __u.__i;
}
#endif
