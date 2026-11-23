// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/LuaScript/LuaDebugger.cpp

#include "LuaDebugger.h"

// X/Server/LuaScript/LuaDebugger.cpp:33  (external, declared in no header)
LuaDebugger * gpLuaDebugger;

// X/Server/LuaScript/LuaDebugger.cpp:344
//
// The released console build compiles this to a bare `ret` (size 4: just the
// return).  Whatever body the debugger had on other platforms is not part of
// this build, so the body here is empty.
void LuaDebugger::DebugBreak(lua_State * L, LuaDebugger::DebugBreakCallbackReason debugBreakReason, const CRefString & sMessage)
{
}
