#ifdef __cplusplus
#define LUA_C_API extern "C"
#else
#define LUA_C_API extern
#endif
// Adopted from the DWARF skeleton (build/dwarf/headers/Contrib/Contrib/lua/src/lua.h);
// hand-maintained from here on. Original file: Contrib/Contrib/lua/src/lua.h
// Subset: the debug record and the stack-inspection API LuaScriptDebug.cpp uses. The skeleton's
// lstate.h include is replaced by a forward declaration, as the adopted src/ headers already do
// for lua_State (src/X/Server/LuaScript/LuaScript.h:13, src/X/Server/SConductor.h:10).

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct lua_State lua_State;
typedef struct lua_Debug lua_Debug;

struct lua_Debug {  // size 0x78
    int event;  // 0x00
    const char * name;  // 0x08
    const char * namewhat;  // 0x10
    const char * what;  // 0x18
    const char * source;  // 0x20
    int currentline;  // 0x28
    int nups;  // 0x2c
    int linedefined;  // 0x30
    int lastlinedefined;  // 0x34
    char short_src[60];  // 0x38
    int i_ci;  // 0x74
};
#ifdef __cplusplus
static_assert(sizeof(lua_Debug) == 0x78, "lua_Debug");
#endif

// lua_Hook: DWARF-recorded in this header (build/dwarf/headers/Contrib/Contrib/lua/src/lua.h:26).
typedef void (*lua_Hook)(lua_State * L, lua_Debug * ar);

// lua_getstack / lua_getinfo: Contrib/Contrib/lua/src/ldebug.c. The original call targets are the
// unmangled symbols lua_getstack@0x7100a644c0 and lua_getinfo@0x7100a64910, so these declarations
// carry C linkage. lua_abortaddress: Contrib/Contrib/lua/src/lapi.c:1082 (DWARF: int *), called
// unmangled at 0x7100a625f0 from LuaHook.cpp's abort hook, so it carries C linkage as well.
// lua_sethook: Contrib/Contrib/lua/src/ldebug.c:56 (DWARF: int lua_sethook(lua_State * L,
// lua_Hook func, int mask, int count)), called unmangled at 0x7100a643c0 from LuaHook.cpp's hook
// install/teardown, so it carries C linkage as well.
#ifdef __cplusplus
extern "C" {
#endif
int lua_getstack(lua_State * L, int level, lua_Debug * ar);
int lua_getinfo(lua_State * L, const char * what, lua_Debug * ar);
int * lua_abortaddress(lua_State * L);
int lua_sethook(lua_State * L, lua_Hook func, int mask, int count);
#ifdef __cplusplus
}
#endif

// Lua 5.1 API used by the generated SWIG interface; declarations owned by lua.h.
typedef int (*lua_CFunction)(lua_State *L);
typedef double lua_Number;
typedef ptrdiff_t lua_Integer;
#ifdef __cplusplus
extern "C" {
#endif
int lua_gettop(lua_State *L);
void lua_settop(lua_State *L, int idx);
void lua_pushvalue(lua_State *L, int idx);
void lua_remove(lua_State *L, int idx);
int lua_type(lua_State *L, int idx);
int lua_iscfunction(lua_State *L, int idx);
int lua_isuserdata(lua_State *L, int idx);
void *lua_touserdata(lua_State *L, int idx);
void lua_pushnil(lua_State *L);
void lua_pushnumber(lua_State *L, lua_Number n);
void lua_pushinteger(lua_State *L, lua_Integer n);
void lua_pushstring(lua_State *L, const char *s);
const char *lua_pushfstring(lua_State *L, const char *fmt, ...);
void lua_pushcclosure(lua_State *L, lua_CFunction fn, int n);
void lua_pushboolean(lua_State *L, int b);
void lua_pushlightuserdata(lua_State *L, void *p);
void lua_rawget(lua_State *L, int idx);
void lua_rawset(lua_State *L, int idx);
void lua_createtable(lua_State *L, int narr, int nrec);
void *lua_newuserdata(lua_State *L, size_t sz);
int lua_getmetatable(lua_State *L, int objindex);
int lua_setmetatable(lua_State *L, int objindex);
void lua_call(lua_State *L, int nargs, int nresults);
int lua_error(lua_State *L);
#ifdef __cplusplus
}
#endif
#define LUA_REGISTRYINDEX (-10000)
#define LUA_TTABLE 5
#define lua_pop(L,n) lua_settop(L, -(n)-1)
#define lua_newtable(L) lua_createtable(L, 0, 0)
#define lua_pushcfunction(L,f) lua_pushcclosure(L, (f), 0)
#define lua_istable(L,n) (lua_type(L, (n)) == LUA_TTABLE)

#ifdef __cplusplus
extern "C" {
#endif
int lua_isnumber(lua_State *L, int idx);
int lua_isstring(lua_State *L, int idx);
lua_Number lua_tonumber(lua_State *L, int idx);
lua_Integer lua_tointeger(lua_State *L, int idx);
int lua_toboolean(lua_State *L, int idx);
const char *lua_tolstring(lua_State *L, int idx, size_t *len);
void lua_settable(lua_State *L, int idx);
#ifdef __cplusplus
}
#endif
#define lua_tostring(L,i) lua_tolstring(L, (i), NULL)
#define LUA_TNIL 0
#define LUA_TBOOLEAN 1
#define LUA_TLIGHTUSERDATA 2
#define LUA_TNUMBER 3
#define LUA_TSTRING 4
#define LUA_TFUNCTION 6
#define LUA_TUSERDATA 7
#define LUA_GLOBALSINDEX (-10002)

#define lua_islightuserdata(L,n) (lua_type(L, (n)) == LUA_TLIGHTUSERDATA)

// Lua 5.1 boolean type-test macro.
#define lua_isboolean(L, n) (lua_type((L), (n)) == LUA_TBOOLEAN)

// lua_gettable (lapi.c) -- used by ScriptEventParams.cpp FloatRangeParam::ParseParamFromScript
LUA_C_API void lua_gettable(lua_State *L, int idx);

LUA_C_API void lua_getfield(lua_State * L, int idx, const char * k);

#define LUA_API
/* luaconf.h (5.1.1) makes both hidden on GCC-compatible ELF compilers; the original reaches
   luaO_nilobject_ and luaT_typenames directly (adrp/add), not through the GOT. LUAI_FUNC
   stays plain extern: the checker link is -shared and unrecovered internal functions are
   still undefined there; call code is the same either way. */
#define LUAI_FUNC	extern
#define LUAI_DATA	__attribute__((visibility("hidden"))) extern
#define LUA_VERSION	"Lua 5.1"
#define LUA_RELEASE	"Lua 5.1.1"
/* mark for precompiled code (`<esc>Lua') */
#define LUA_SIGNATURE	"\033Lua"
#define LUA_VERSION_NUM	501
#define LUA_COPYRIGHT	"Copyright (C) 1994-2006 Lua.org, PUC-Rio"
#define LUA_AUTHORS 	"R. Ierusalimschy, L. H. de Figueiredo & W. Celes"
#define LUA_MULTRET	(-1)
#define LUA_ENVIRONINDEX	(-10001)
#define LUA_YIELD	1
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRERR	5
#define LUA_TNONE		(-1)
#define LUA_TTHREAD		8
#define LUA_MINSTACK	20
#define LUA_GCSTOP		0
#define LUA_GCRESTART		1
#define LUA_GCCOLLECT		2
#define LUA_GCCOUNT		3
#define LUA_GCCOUNTB		4
#define LUA_GCSTEP		5
#define LUA_GCSETPAUSE		6
#define LUA_GCSETSTEPMUL	7
#define LUA_HOOKCALL	0
#define LUA_HOOKRET	1
#define LUA_HOOKLINE	2
#define LUA_HOOKCOUNT	3
#define LUA_HOOKTAILRET 4
#define LUA_MASKCALL	(1 << LUA_HOOKCALL)
#define LUA_MASKRET	(1 << LUA_HOOKRET)
#define LUA_MASKLINE	(1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT	(1 << LUA_HOOKCOUNT)
typedef int (*lua_Writer)(lua_State *, const void *, size_t, void *);
typedef const char *(*lua_Reader)(lua_State *, void *, size_t *);

/* Lua 5.1.1 configuration and the rest of the 5.1.1 lua.h API, for the C owners
 * recovered from upstream text. Appended so the declarations above stay as they are;
 * luaconf.h replaces the empty LUA_API above (C linkage under C++, see luaconf.h). */
#include <stdarg.h>
#undef LUA_API
#undef LUAI_FUNC
#undef LUAI_DATA
#include "luaconf.h"
typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);
LUA_API lua_State *(lua_newstate) (lua_Alloc f, void *ud);
LUA_API void (lua_close) (lua_State *L);
LUA_API lua_State *(lua_newthread) (lua_State *L);
LUA_API lua_CFunction (lua_atpanic) (lua_State *L, lua_CFunction panicf);
LUA_API void (lua_insert) (lua_State *L, int idx);
LUA_API void (lua_replace) (lua_State *L, int idx);
LUA_API int (lua_checkstack) (lua_State *L, int sz);
LUA_API void (lua_xmove) (lua_State *from, lua_State *to, int n);
LUA_API const char *(lua_typename) (lua_State *L, int tp);
LUA_API int (lua_equal) (lua_State *L, int idx1, int idx2);
LUA_API int (lua_rawequal) (lua_State *L, int idx1, int idx2);
LUA_API int (lua_lessthan) (lua_State *L, int idx1, int idx2);
LUA_API lua_Number (lua_tonumber) (lua_State *L, int idx);
LUA_API lua_Integer (lua_tointeger) (lua_State *L, int idx);
LUA_API size_t (lua_objlen) (lua_State *L, int idx);
LUA_API lua_CFunction (lua_tocfunction) (lua_State *L, int idx);
LUA_API lua_State *(lua_tothread) (lua_State *L, int idx);
LUA_API const void *(lua_topointer) (lua_State *L, int idx);
LUA_API void (lua_pushlstring) (lua_State *L, const char *s, size_t l);
LUA_API const char *(lua_pushvfstring) (lua_State *L, const char *fmt, va_list argp);
LUA_API int (lua_pushthread) (lua_State *L);
LUA_API void (lua_rawgeti) (lua_State *L, int idx, int n);
LUA_API void (lua_getfenv) (lua_State *L, int idx);
LUA_API void (lua_setfield) (lua_State *L, int idx, const char *k);
LUA_API void (lua_rawseti) (lua_State *L, int idx, int n);
LUA_API int (lua_setfenv) (lua_State *L, int idx);
LUA_API int (lua_pcall) (lua_State *L, int nargs, int nresults, int errfunc);
LUA_API int (lua_cpcall) (lua_State *L, lua_CFunction func, void *ud);
LUA_API int (lua_load) (lua_State *L, lua_Reader reader, void *dt, const char *chunkname);
LUA_API int (lua_dump) (lua_State *L, lua_Writer writer, void *data);
LUA_API int (lua_yield) (lua_State *L, int nresults);
LUA_API int (lua_resume) (lua_State *L, int narg);
LUA_API int (lua_status) (lua_State *L);
LUA_API int (lua_gc) (lua_State *L, int what, int data);
LUA_API int (lua_next) (lua_State *L, int idx);
LUA_API void (lua_concat) (lua_State *L, int n);
LUA_API lua_Alloc (lua_getallocf) (lua_State *L, void **ud);
LUA_API void lua_setallocf (lua_State *L, lua_Alloc f, void *ud);
LUA_API const char *lua_getlocal (lua_State *L, const lua_Debug *ar, int n);
LUA_API const char *lua_setlocal (lua_State *L, const lua_Debug *ar, int n);
LUA_API const char *lua_getupvalue (lua_State *L, int funcindex, int n);
LUA_API const char *lua_setupvalue (lua_State *L, int funcindex, int n);
LUA_API lua_Hook lua_gethook (lua_State *L);
LUA_API int lua_gethookmask (lua_State *L);
LUA_API int lua_gethookcount (lua_State *L);
#define lua_upvalueindex(i)	(LUA_GLOBALSINDEX-(i))
#define lua_register(L,n,f) (lua_pushcfunction(L, (f)), lua_setglobal(L, (n)))
#define lua_strlen(L,i)		lua_objlen(L, (i))
#define lua_isfunction(L,n)	(lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_isnil(L,n)		(lua_type(L, (n)) == LUA_TNIL)
#define lua_isthread(L,n)	(lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L,n)		(lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n)	(lua_type(L, (n)) <= 0)
#define lua_pushliteral(L, s)	\
	lua_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)
#define lua_setglobal(L,s)	lua_setfield(L, LUA_GLOBALSINDEX, (s))
#define lua_getglobal(L,s)	lua_getfield(L, LUA_GLOBALSINDEX, (s))
#define lua_open()	luaL_newstate()
#define lua_getregistry(L)	lua_pushvalue(L, LUA_REGISTRYINDEX)
#define lua_getgccount(L)	lua_gc(L, LUA_GCCOUNT, 0)
#define lua_Chunkreader		lua_Reader
#define lua_Chunkwriter		lua_Writer

/******************************************************************************
* Copyright (C) 1994-2006 Lua.org, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/
