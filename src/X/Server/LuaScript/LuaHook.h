// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/LuaScript/LuaHook.h

#pragma once

#include "../../../Contrib/Contrib/blz/include/blz/vector.h"

struct lua_State;

class LuaHook {  // size 0x18
public:
    LuaHook();
    ~LuaHook();
    void AttachToLuaState(lua_State * pLuaState);
    void DetachFromLuaState(lua_State * pLuaState);
private:
    blz::vector<lua_State *, blz::allocator<lua_State *> > aAttachedLuaStates;  // 0x00
};
static_assert(sizeof(LuaHook) == 0x18, "LuaHook");


// LuaHook.cpp:29 and :37 (X/Common/Game.cpp AppInitialize / AppDispose).
void LuaHook_Create();
void LuaHook_Destroy();
