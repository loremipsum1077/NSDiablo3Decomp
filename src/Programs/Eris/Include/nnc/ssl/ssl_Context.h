
/* Original DWARF declarations consumed by bundles.c, CU DIE 0x07c8ece6.
 * Complete definitions for every by-value type; pointer-only types are forward
 * declarations. No synthetic padding, hidden implementation body or API stub.
 */
#ifndef D3_RECOVERED_PROGRAMS_ERIS_INCLUDE_NNC_SSL_SSL_CONTEXT_H
#define D3_RECOVERED_PROGRAMS_ERIS_INCLUDE_NNC_SSL_SSL_CONTEXT_H

#include <stddef.h>
#include <stdint.h>

union nnsslSslContextType;

/* D:/home/teamcity2/work/sdk/Programs/Eris/Include/nnc/ssl/ssl_Context.h:27; DIE 0x7c91cd5 */
union nnsslSslContextType {
  uint8_t buf[128]; /* 0x0 */
  void *alignment_holder; /* 0x0 */
};

_Static_assert(sizeof(union nnsslSslContextType) == 128, "nnsslSslContextType size");
_Static_assert(offsetof(union nnsslSslContextType, buf) == 0, "nnsslSslContextType.buf");
_Static_assert(offsetof(union nnsslSslContextType, alignment_holder) == 0, "nnsslSslContextType.alignment_holder");

/* D:/home/teamcity2/work/sdk/Programs/Eris/Include/nnc/ssl/ssl_Context.h:27; DIE 0x7c91cca */
typedef union nnsslSslContextType nnsslSslContextType;

_Static_assert(sizeof(nnsslSslContextType) == 128, "nnsslSslContextType size");

#endif /* D3_RECOVERED_PROGRAMS_ERIS_INCLUDE_NNC_SSL_SSL_CONTEXT_H */
