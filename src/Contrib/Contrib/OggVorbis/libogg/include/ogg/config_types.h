/* config_types.h for the Diablo III (NX64) build of libogg 1.2.2.
 *
 * Hand-written, not generated: the build image has no autotools, and this is
 * what configure writes for an LP64 aarch64 target (the 2.6.2 DWARF types
 * ogg_int64_t as a 64-bit signed integer and ogg_int32_t/ogg_uint32_t as
 * 32-bit, which is all the width that reaches code generation). See
 * docs/batches/2026-10-19-codec-pilot/framing/README.md.
 */
#ifndef __CONFIG_TYPES_H__
#define __CONFIG_TYPES_H__

#define INCLUDE_INTTYPES_H 1
#define INCLUDE_STDINT_H 1
#define INCLUDE_SYS_TYPES_H 1

#if INCLUDE_INTTYPES_H
#  include <inttypes.h>
#endif
#if INCLUDE_STDINT_H
#  include <stdint.h>
#endif
#if INCLUDE_SYS_TYPES_H
#  include <sys/types.h>
#endif

typedef int16_t ogg_int16_t;
typedef uint16_t ogg_uint16_t;
typedef int32_t ogg_int32_t;
typedef uint32_t ogg_uint32_t;
typedef int64_t ogg_int64_t;

#endif
