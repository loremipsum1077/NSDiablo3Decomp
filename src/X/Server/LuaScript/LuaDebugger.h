// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/LuaScript/LuaDebugger.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"

struct lua_Debug;
struct lua_State;

struct LuaDebuggerBreakpoint {  // size 0x10
    LuaDebuggerBreakpoint();
    LuaDebuggerBreakpoint(SNOName, int32);
    SNOName mSnoName;  // 0x00
    int32 nLineNumber;  // 0x08
    BOOL bEnabled;  // 0x0c
};
static_assert(sizeof(LuaDebuggerBreakpoint) == 0x10, "LuaDebuggerBreakpoint");

typedef blz::vector<LuaDebuggerBreakpoint, blz::allocator<LuaDebuggerBreakpoint> > LuaDebuggerBreakpointArray;

struct LuaDebuggerLocal {  // size 0x30
    CRefString sName;  // 0x00
    CRefString sValue;  // 0x18
};
static_assert(sizeof(LuaDebuggerLocal) == 0x30, "LuaDebuggerLocal");

typedef blz::vector<LuaDebuggerLocal, blz::allocator<LuaDebuggerLocal> > LuaDebuggerLocalArray;

class LuaDebugger {  // size 0x30
    enum LuaDebuggerState {
        LUA_DEBUGGER_RUN_PROGRAM = 0,
        LUA_DEBUGGER_HALT_PROGRAM = 1,
        LUA_DEBUGGER_STEP_INTO = 2,
        LUA_DEBUGGER_STEP_OVER = 3,
    };
public:
    enum DebugBreakCallbackReason {
        DEBUG_BREAK_USER_STOP = 0,
        DEBUG_BREAK_SCRIPT_ERROR = 1,
    };
public:
    LuaDebugger();
    ~LuaDebugger();
    void AddBreakpoint(SNOName, int32);
    void RemoveBreakpoint(SNOName, int32);
    void EnableBreakpoint(SNOName, int32);
    void DisableBreakpoint(SNOName, int32);
    BOOL HasBreakpoint(SNOName, int32);
    void EnableAllBreakpoints();
    void DisableAllBreakpoints();
    void ClearBreakpoints();
    void Continue();
    void ContinueAndSetToGameMode();
    void StepOver();
    void StepInto();
    void GetLocals(lua_State *, lua_Debug *, LuaDebuggerLocalArray &);
    LuaDebugger::LuaDebuggerState GetState() const;
    void HookCallback(lua_State *, lua_Debug *);
    int32 GetBreakpointIndex(SNOName snoName, int32 nLineNumber);
    void DebugBreak(lua_State * L, LuaDebugger::DebugBreakCallbackReason debugBreakReason, const CRefString & sMessage);
    void DebugBreakAtBreakpoint(lua_State * L, lua_Debug * ar, const char * pszFileName);
    LuaDebugger::LuaDebuggerState eState;  // 0x00
    int32 nStackLevel;  // 0x04
    int32 nStepToTargetStackLevel;  // 0x08
    LuaDebuggerBreakpointArray aBreakpoints;  // 0x10
    BOOL mbSetGameModePlayingAtEndOfFrame;  // 0x28
private:
    void EnterDebuggerCallback(lua_State *, LuaDebugger::DebugBreakCallbackReason, LPCSTR);
    void SetState(LuaDebugger::LuaDebuggerState newState);
};
static_assert(sizeof(LuaDebugger) == 0x30, "LuaDebugger");


extern LuaDebugger * gpLuaDebugger;  // LuaDebugger.cpp:7

// Functions defined in this file (bodies are not in the DWARF):
//   line    27  LuaDebuggerBreakpoint::LuaDebuggerBreakpoint(LuaDebuggerBreakpoint &&)  [inline]
//   line    37  LuaDebuggerBreakpoint::LuaDebuggerBreakpoint(SNOName snoName, int32 nLineNumber)  [inline]
//   line    53  LuaDebuggerLocal::LuaDebuggerLocal()  [inline]
//   line    53  LuaDebuggerLocal::~LuaDebuggerLocal()  [inline]
