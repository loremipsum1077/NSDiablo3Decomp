#pragma once

// Not an original file. The Nintendo SDK's libc exports plain `sscanf`; the
// toolchain image's glibc redirects it to `__isoc99_sscanf` under strict
// -std=c++11, a different callee. Bind the name to the symbol the original calls.
#include_next <stdio.h>

#ifdef __cplusplus
extern "C" int sscanf_nx(const char*, const char*, ...) __asm__("sscanf");
#else
extern int sscanf_nx(const char*, const char*, ...) __asm__("sscanf");
#endif
#define sscanf sscanf_nx

// The SDK libc (musl-derived, see the alltypes.h/time.h DWARF lines) has BUFSIZ 1024,
// not glibc's 8192. Lua sizes luaL_Buffer with LUAL_BUFFERSIZE = BUFSIZ, and the
// original auxiliary/table/string/io library frames and immediates use 0x400.
#undef BUFSIZ
#define BUFSIZ 1024

// Same for fscanf (Lua's io library read_number): the original calls plain `fscanf`.
#ifdef __cplusplus
extern "C" int fscanf_nx(FILE*, const char*, ...) __asm__("fscanf");
#else
extern int fscanf_nx(FILE*, const char*, ...) __asm__("fscanf");
#endif
#define fscanf fscanf_nx

// The SDK libc is musl-derived: musl declares the standard streams `FILE *const`
// (extern FILE *const stdin;), so loads of them are invariant. Evidence: the original
// Curl_init_userdefined (url.c:508-510, linker-removed copy rows 508:14/509:12/510:13) loads
// stdout/stdin/stderr ahead of the stores and writes out/in with one paired store.
#ifdef __cplusplus
extern "C" FILE *const stdin_nx __asm__("stdin");
extern "C" FILE *const stdout_nx __asm__("stdout");
extern "C" FILE *const stderr_nx __asm__("stderr");
#else
extern FILE *const stdin_nx __asm__("stdin");
extern FILE *const stdout_nx __asm__("stdout");
extern FILE *const stderr_nx __asm__("stderr");
#endif
#undef stdin
#undef stdout
#undef stderr
#define stdin (stdin_nx)
#define stdout (stdout_nx)
#define stderr (stderr_nx)
