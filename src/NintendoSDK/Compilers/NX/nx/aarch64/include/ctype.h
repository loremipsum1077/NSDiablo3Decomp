// Recovered subset of the Nintendo SDK ctype.h from original DWARF/assembly.
// Only the whitespace predicate used by the Lua object parser is adopted.
#pragma once

// NON_MATCHING: the original O0 compiler retains additional argument copies/spills.
static inline int __isspace(int _c) {
  return _c == ' ' || (unsigned int)(_c - '\t') < 5;
}
#define isspace(_c) __isspace(_c)

// The rest of the SDK ctype.h interface, as in musl 1.1.x (which the SDK libc follows,
// see include/stdio.h): the original Lua string library tests isalpha/isdigit/islower/
// isupper arithmetically and calls the remaining classifiers.
#ifdef __cplusplus
extern "C" {
#endif
int   isalnum(int);
int   isalpha(int);
int   isblank(int);
int   iscntrl(int);
int   isdigit(int);
int   isgraph(int);
int   islower(int);
int   isprint(int);
int   ispunct(int);
int   isupper(int);
int   isxdigit(int);
int   tolower(int);
int   toupper(int);
#ifdef __cplusplus
}
#endif

#ifndef __cplusplus
#define isalpha(a) (0 ? isalpha(a) : (((unsigned)(a)|32)-'a') < 26)
#define isdigit(a) (0 ? isdigit(a) : ((unsigned)(a)-'0') < 10)
#define islower(a) (0 ? islower(a) : ((unsigned)(a)-'a') < 26)
#define isupper(a) (0 ? isupper(a) : ((unsigned)(a)-'A') < 26)
#define isprint(a) (0 ? isprint(a) : ((unsigned)(a)-0x20) < 0x5f)
#define isgraph(a) (0 ? isgraph(a) : ((unsigned)(a)-0x21) < 0x5e)
#endif
