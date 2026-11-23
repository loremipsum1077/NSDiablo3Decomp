#pragma once

// Not an original file. The Nintendo SDK's libc (musl-derived, see stdio.h) declares
// setjmp as a function; the toolchain image's glibc <setjmp.h> turns setjmp into
// _setjmp. The original ldo.c (luaD_rawrunprotected, through luaconf.h LUAI_TRY)
// calls setjmp.
#include_next <setjmp.h>
#undef setjmp
