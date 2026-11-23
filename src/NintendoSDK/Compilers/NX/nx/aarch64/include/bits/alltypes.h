
/* Original DWARF declarations consumed by bundles.c, CU DIE 0x07c8ece6.
 * Complete definitions for every by-value type; pointer-only types are forward
 * declarations. No synthetic padding, hidden implementation body or API stub.
 */
#ifndef D3_RECOVERED_NINTENDOSDK_COMPILERS_NX_NX_AARCH64_INCLUDE_BITS_ALLTYPES_H
#define D3_RECOVERED_NINTENDOSDK_COMPILERS_NX_NX_AARCH64_INCLUDE_BITS_ALLTYPES_H

#include <stddef.h>
#include <stdint.h>


/* D:/Tools/clang-for-nx.1.4.9/nx/aarch64/include/bits/alltypes.h:77; DIE 0x7c8fd50 */
typedef long time_t;


/* D:/Tools/clang-for-nx.1.4.9/nx/aarch64/include/bits/alltypes.h:82; DIE 0x7c8fd5b */
typedef long suseconds_t;


/* D:/Tools/clang-for-nx.1.4.9/nx/aarch64/include/bits/alltypes.h:138; DIE 0x7c9007e */
typedef long ssize_t;


/* D:/Tools/clang-for-nx.1.4.9/nx/aarch64/include/bits/alltypes.h:287; DIE 0x7c8fd2c */
struct timeval {
  time_t tv_sec; /* 0x0 */
  suseconds_t tv_usec; /* 0x8 */
};

_Static_assert(sizeof(struct timeval) == 16, "timeval size");
_Static_assert(offsetof(struct timeval, tv_sec) == 0, "timeval.tv_sec");
_Static_assert(offsetof(struct timeval, tv_usec) == 8, "timeval.tv_usec");

_Static_assert(sizeof(size_t) == 8, "original size_t");
_Static_assert(sizeof(uint8_t) == 1, "original uint8_t");
#endif /* D3_RECOVERED_NINTENDOSDK_COMPILERS_NX_NX_AARCH64_INCLUDE_BITS_ALLTYPES_H */
