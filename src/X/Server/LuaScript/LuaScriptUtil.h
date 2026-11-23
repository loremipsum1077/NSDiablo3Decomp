// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/LuaScript/LuaScriptUtil.h
// Subset. The lua_* entry points are declared here until Contrib/lua is in src/.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

struct lua_State;

extern "C" {
void lua_pushnil(lua_State * L);  // Contrib/lua/src/lapi.c
void lua_pushnumber(lua_State * L, double n);  // Contrib/lua/src/lapi.c
void lua_pushinteger(lua_State * L, ptrdiff_t n);  // Contrib/lua/src/lapi.c
void lua_pushstring(lua_State * L, const char * s);  // Contrib/lua/src/lapi.c
}

typedef int32 ScriptID;

typedef int32 ScriptInstanceID;

// Release build: the checker compiles to nothing.
class LuaStackChecker {  // size 0x1
public:
    LuaStackChecker(lua_State *) {}
    LuaStackChecker(lua_State *, int32) {}
};
static_assert(sizeof(LuaStackChecker) == 0x1, "LuaStackChecker");

BOOL LuaScriptGetFunction(lua_State * L, LPCSTR szFuncName);  // LuaScriptUtil.cpp:1186
const char * LuaScriptStackItemToString(lua_State * luaState, int32 nIndex);  // LuaScriptUtil.cpp:739
int32 LuaScriptReportError(lua_State * L, int32 status, BOOL printStackTrace);  // LuaScriptUtil.cpp:636
void LuaDebugBreakError(lua_State * L, LPCSTR szErrorMsg);  // LuaScriptUtil.cpp:947
BOOL LuaScriptHasFunction(lua_State * L, LPCSTR szFuncName);  // LuaScriptUtil.cpp:1217
int32 LuaScriptGetTableSize(lua_State * L, int32 nTableIndex);  // LuaScriptUtil.cpp:1155

int32 LuaCall(ScriptInstanceID idScriptInstance, lua_State * L, int32 nArgumentsCnt, int32 nReturnValCnt, LPCSTR szFuncName);

void LuaMove(lua_State * lSrc, lua_State * lDst, int32 nItems);  // LuaScriptUtil.cpp:1319
