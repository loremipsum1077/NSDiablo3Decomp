/*
** $Id: linit.c,v 1.14 2005/12/29 15:32:11 roberto Exp $
** Initialization of libraries for lua.c
** Copyright notice retained below.
*/


#define linit_c
#define LUA_LIB

#include "llimits.h"

/* Minimal C declarations from lua.h, lualib.h and lauxlib.h. The adopted
 * public headers currently contain C++-only declarations. */
typedef struct luaL_Reg {
  const char *name;
  lua_CFunction func;
} luaL_Reg;
LUA_LAYOUT_ASSERT(sizeof(luaL_Reg) == 0x10, "luaL_Reg");
extern void lua_pushcclosure(lua_State *L, lua_CFunction fn, int n);
extern void lua_pushstring(lua_State *L, const char *s);
extern void lua_call(lua_State *L, int nargs, int nresults);
extern int luaopen_base(lua_State *L);
extern int luaopen_package(lua_State *L);
extern int luaopen_table(lua_State *L);
extern int luaopen_io(lua_State *L);
extern int luaopen_os(lua_State *L);
extern int luaopen_string(lua_State *L);
extern int luaopen_math(lua_State *L);
extern int luaopen_debug(lua_State *L);

/* lua.h macro, expanded by the original caller. */
#define lua_pushcfunction(L,f) lua_pushcclosure(L, (f), 0)


static const luaL_Reg lualibs[] = {
  {"", luaopen_base},
  {"package", luaopen_package},
  {"table", luaopen_table},
  {"io", luaopen_io},
  {"os", luaopen_os},
  {"string", luaopen_string},
  {"math", luaopen_math},
  {"debug", luaopen_debug},
  {NULL, NULL}
};


// NON_MATCHING: 136 vs 144 bytes; original retains a parameter copy/spill.
void luaL_openlibs (lua_State *L) {
  const luaL_Reg *lib = lualibs;
  for (; lib->func; lib++) {
    lua_pushcfunction(L, lib->func);
    lua_pushstring(L, lib->name);
    lua_call(L, 1, 0);
  }
}


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


