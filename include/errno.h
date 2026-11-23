#pragma once

// Not an original file. The Nintendo SDK's libc (musl-derived) declares
// `__errno_location` without __attribute__((const)); the toolchain image's glibc
// declares it const, so clang folds repeated errno reads and writes into one call
// where the original calls the accessor at every use. Rebind errno to the same
// symbol through a declaration without the attribute.
#include_next <errno.h>

#ifdef __cplusplus
extern "C" int* __errno_location_nx(void) __asm__("__errno_location");
#else
extern int* __errno_location_nx(void) __asm__("__errno_location");
#endif
#undef errno
#define errno (*__errno_location_nx())
