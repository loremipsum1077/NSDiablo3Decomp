// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/LuaScript/LuaAbortThreadWatcher.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "LuaScript.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"

struct LuaDeadlockGameThreadData {  // size 0x14
    LuaDeadlockGameThreadData();
    uint32 dwExecuteUID;  // 0x00
    uint32 dwExecuteCount;  // 0x04
    uint32 dwLastObservedExecuteUID;  // 0x08
    uint32 dwLastObservedExecuteUIDTime;  // 0x0c
    BOOL bAbortLuaScript;  // 0x10
};
static_assert(sizeof(LuaDeadlockGameThreadData) == 0x14, "LuaDeadlockGameThreadData");

struct LuaDeadlockThreadData {  // size 0x30
    LuaDeadlockThreadData();  // line 41
    ~LuaDeadlockThreadData();  // line 41
    Blizzard::Lock::REvent hExitThreadEvent;  // 0x00
    blz::vector<LuaDeadlockGameThreadData *, blz::allocator<LuaDeadlockGameThreadData *> > taGameThreadData;  // 0x10
    volatile BOOL bCheckForMainThreadStall;  // 0x28
    uint32 uMainLoopUpdateCount;  // 0x2c
};
static_assert(sizeof(LuaDeadlockThreadData) == 0x30, "LuaDeadlockThreadData");

struct LuaScriptThreadGlobals {  // size 0x218
    LuaDeadlockGameThreadData * ptDeadlockGameThreadData;  // 0x00
    ActorIDArray tActorIdArray;  // 0x08
    CHAR sInitializingScriptStateName[128];  // 0x110
    ScriptInstanceID arExecutingScriptStack[10];  // 0x190
    int32 nExecutingScriptDepth;  // 0x1b8
    LPCSTR arExecutingFunctionStack[10];  // 0x1c0
    int32 nExecutingFunctionDepth;  // 0x210
};
static_assert(sizeof(LuaScriptThreadGlobals) == 0x218, "LuaScriptThreadGlobals");

// Defined in X/Server/LuaScript/LuaAbortThreadWatcher.cpp:32 (DWARF: LuaScriptThreadGlobals *
// LuaScriptThreadGlobalsGet(), call target 0x71007e64d0). Declared here because this header owns
// the LuaScriptThreadGlobals type it returns; LuaHook.cpp's abort hook calls it.
LuaScriptThreadGlobals * LuaScriptThreadGlobalsGet();

// The two special members below are what LuaDeadlockThreadData's members make
// up: they are recorded (the DWARF footer lists them as defined in this
// header) but their bodies are not, so they are written from their inlined
// copies in SGameManager's constructor and destructor.
inline LuaDeadlockThreadData::LuaDeadlockThreadData() {
    // No member initialisers: SGameManager's constructor sets bCheckForMainThreadStall and
    // uMainLoopUpdateCount itself (SGameManager.cpp:272-273), after the threads are created.
}

inline LuaDeadlockThreadData::~LuaDeadlockThreadData() {
}


// Functions defined in this file (bodies are not in the DWARF):
//   line    41  LuaDeadlockThreadData::LuaDeadlockThreadData()  [inlined into 1 functions]
//   line    41  LuaDeadlockThreadData::~LuaDeadlockThreadData()  [inlined into 1 functions]
