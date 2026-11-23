
/* Original DWARF declarations consumed by bundles.c, CU DIE 0x07c8ece6.
 * Complete definitions for every by-value type; pointer-only types are forward
 * declarations. No synthetic padding, hidden implementation body or API stub.
 */
#ifndef D3_RECOVERED_PROGRAMS_ERIS_INCLUDE_NNC_SSL_SSL_CONNECTION_H
#define D3_RECOVERED_PROGRAMS_ERIS_INCLUDE_NNC_SSL_SSL_CONNECTION_H

#include <stddef.h>
#include <stdint.h>

union nnsslSslConnectionType;

/* D:/home/teamcity2/work/sdk/Programs/Eris/Include/nnc/ssl/ssl_Connection.h:20; DIE 0x7c91c9d */
union nnsslSslConnectionType {
  uint8_t buf[128]; /* 0x0 */
  void *alignment_holder; /* 0x0 */
};

_Static_assert(sizeof(union nnsslSslConnectionType) == 128, "nnsslSslConnectionType size");
_Static_assert(offsetof(union nnsslSslConnectionType, buf) == 0, "nnsslSslConnectionType.buf");
_Static_assert(offsetof(union nnsslSslConnectionType, alignment_holder) == 0, "nnsslSslConnectionType.alignment_holder");

/* D:/home/teamcity2/work/sdk/Programs/Eris/Include/nnc/ssl/ssl_Connection.h:20; DIE 0x7c91c92 */
typedef union nnsslSslConnectionType nnsslSslConnectionType;

_Static_assert(sizeof(nnsslSslConnectionType) == 128, "nnsslSslConnectionType size");

#endif /* D3_RECOVERED_PROGRAMS_ERIS_INCLUDE_NNC_SSL_SSL_CONNECTION_H */
