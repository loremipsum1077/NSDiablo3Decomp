// Lua 5.1.1 macros and original lstring.c declaration.
#pragma once
#include "lobject.h"
#include "lgc.h"
#ifdef __cplusplus
extern "C" {
#endif
void luaS_resize(lua_State *L, int newsize);
Udata *luaS_newudata(lua_State *L, size_t s, Table *e);
TString *luaS_newlstr(lua_State *L, const char *str, size_t l);
#ifdef __cplusplus
}
#endif
#define luaS_new(L,s) (luaS_newlstr(L,s,strlen(s)))
#define luaS_newliteral(L, s)	(luaS_newlstr(L, "" s, \
                                 (sizeof(s)/sizeof(char))-1))
#define luaS_fix(s) l_setbit((s)->tsv.marked, FIXEDBIT)

/* Lua 5.1.1 allocation sizes for string and userdata objects. */
#define sizestring(s) (sizeof(union TString) + ((s)->len + 1) * sizeof(char))
#define sizeudata(u) (sizeof(union Udata) + (u)->len)
