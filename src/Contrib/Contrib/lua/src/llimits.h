// Adopted DWARF types; macros recovered from Lua 5.1.1 headers.
// Original file: Contrib/Contrib/lua/src/llimits.h

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef unsigned int lu_int32;

typedef size_t lu_mem;

typedef ptrdiff_t l_mem;

typedef unsigned char lu_byte;

typedef union {  // size 0x8
    double u;  // 0x00
    void * s;  // 0x00
    long int l;  // 0x00
} L_Umaxalign;

typedef lu_int32 Instruction;


/* Minimal lua.h declarations needed by the internal layouts. The existing
 * public header is a C++ API subset; keep this C owner independent of it. */
typedef struct lua_State lua_State;
struct lua_Debug;
typedef double lua_Number;
typedef int (*lua_CFunction)(lua_State *L);
typedef void (*lua_Hook)(lua_State *L, struct lua_Debug *ar);
typedef void *(*lua_Alloc)(void *ud, void *ptr, size_t osize, size_t nsize);
#define cast(t, exp) ((t)(exp))
#define cast_byte(i) cast(lu_byte, (i))
#define lua_assert(c) ((void)0)
/* Invented compatibility name for the DWARF layout checks. */
#ifdef __cplusplus
#define LUA_LAYOUT_ASSERT static_assert
#else
#define LUA_LAYOUT_ASSERT _Static_assert
#endif

#define check_exp(c,e) (e)
#define cast_int(i) cast(int, (i))
#define cast_num(i) cast(lua_Number, (i))
/* As luaconf.h (with its D3 deviations), for owners that do not include lua.h. */
#ifndef LUAI_FUNC
#define LUAI_FUNC	extern
#endif
#ifndef LUAI_DATA
#define LUAI_DATA	__attribute__((visibility("hidden"))) extern
#endif
/* luaconf.h (5.1.1) luai_apicheck without LUA_USE_APICHECK: evaluates L at O0. */
#define luai_apicheck(L,o)	{ (void)L; }
#define api_check		luai_apicheck
#define lua_lock(L) ((void)0)
#define lua_unlock(L) ((void)0)
#define lua_number2integer(i,d)	((i)=(lua_Integer)(d))
#define LUAI_MAXCSTACK 2048
#define MAX_SIZET ((size_t)(~(size_t)0)-2)

#define condhardstacktests(x) ((void)0)
#define UNUSED(x) ((void)(x))

#include <limits.h>
#define MAX_INT (INT_MAX-2)
#define MAX_LUMEM ((lu_mem)(~(lu_mem)0)-2)
#define luai_userstatethread(L,L1)	((void)L)
#define l_uacNumber double
#define luai_numeq(a,b) ((a)==(b))
#define luai_numisnan(a) (!luai_numeq((a), (a)))
#define lua_str2number(s,p) strtod((s), (p))
#define lua_number2int(i,d)	((i)=(int)(d))
#define LUAI_BITSINT 32
#define IntPoint(p) ((unsigned int)(lu_mem)(p))
#define LUA_QL(x) "'" x "'"

/* maximum stack for a Lua function (5.1.1 llimits.h) */
#define MAXSTACK	250

// Layout checks generated from the DWARF by tools/layout_asserts.py.
_Static_assert(sizeof(L_Umaxalign) == 0x8, "L_Umaxalign");

/* Lua 5.1.1 limits used by state and collection. */
#ifndef MINSTRTABSIZE
#define MINSTRTABSIZE 32
#endif
#ifndef LUA_MINBUFFER
#define LUA_MINBUFFER 32
#endif
/* Lua 5.1.1 llimits.h: yield point used by the VM's jumps */
#ifndef luai_threadyield
#define luai_threadyield(L)     {lua_unlock(L); lua_lock(L);}
#endif
