
/* Original DWARF declaration consumed by Domino's dmThreading_Linux.h (dmMutex::m_mutex),
 * dmWorld.cpp CU, DIE 0x07d3dc14. Layout from the DWARF; nothing else of this header is recovered.
 *
 * The original type is the global `pthread_mutex_t` (a union, typedef'd under the same name).
 * It is declared here as nnmusl_pthread_mutex_t because the build compiles against the host
 * glibc, whose <pthread.h> declares a 48-byte pthread_mutex_t under the original name.
 */
#ifndef D3_RECOVERED_NINTENDOSDK_COMPILERS_NX_NX_AARCH64_INCLUDE_NNMUSL_ALLTYPES_H
#define D3_RECOVERED_NINTENDOSDK_COMPILERS_NX_NX_AARCH64_INCLUDE_NNMUSL_ALLTYPES_H


/* C:/Nintendo/DiabloIII/NintendoSDK/Compilers/NX/nx/aarch64/include/nnmusl_alltypes.h:35; DIE 0x7d3dc14 (size 0x20) */
typedef union nnmusl_pthread_mutex_t {
  unsigned char buf[32]; /* 0x0 */
  void * alignment_holder; /* 0x0 */
} nnmusl_pthread_mutex_t;

_Static_assert(sizeof(nnmusl_pthread_mutex_t) == 32, "nnmusl pthread_mutex_t size");
_Static_assert(_Alignof(nnmusl_pthread_mutex_t) == 8, "nnmusl pthread_mutex_t alignment");
#endif /* D3_RECOVERED_NINTENDOSDK_COMPILERS_NX_NX_AARCH64_INCLUDE_NNMUSL_ALLTYPES_H */
