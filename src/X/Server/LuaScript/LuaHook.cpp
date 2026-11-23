// Original file: X/Server/LuaScript/LuaHook.cpp -- the global LuaHook that
// tracks the lua_States running game scripts and installs the abort hook
// (sLuaHook) on them.

#include "LuaHook.h"

#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../Contrib/Contrib/lua/src/lua.h"
#include "../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../Sigma/Stopwatch.h"
#include "LuaAbortThreadWatcher.h"
#include "LuaScript.h"
#include "../SGame.h"
#include "../SGameConstants.h"
#include "../SGameManager.h"

// line 26
LuaHook *gpLuaHook;

// line 29
void LuaHook_Create()
{
    gpLuaHook = new LuaHook;
}

// line 35
void LuaHook_Destroy()
{
    if (gpLuaHook)
    {
        delete gpLuaHook;
    }
    gpLuaHook = nullptr;
}

// line 42
void sLuaHook(lua_State * L, lua_Debug * ar)
{
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals;

    // line 46
    ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    // lines 47-48
    if (ptLuaScriptThreadGlobals != NULL &&
        ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->bAbortLuaScript != 0)
    {
        // line 50
        SNOName snoNameScriptSource = LuaScriptGetSNONameForLuaState(L);

        // line 53
        int32 * pAbort = lua_abortaddress(L);

        // The hook runs on the script's own thread, so this is where the
        // watcher thread's abort request is honoured: write the flag through
        // the address lua hands us and the script unwinds at the next hook
        // opportunity.

        // line 59
        *pAbort = 1;

        // A script that wedged its thread cannot be resumed, so end the game
        // it was running instead of leaving a half-finished game alive for
        // its players; GetCurrentGame() yields the invalid id when this
        // thread is not running a game at all.

        // line 67
        SGameID idSGame = SGameManager::GetCurrentGame();
        // line 68
        if (idSGame != -1)
        {
            // line 70
            SGameManager::SignalKillGame(idSGame);
        }

        // Stamp the abort time so the watcher thread stops counting this game
        // thread as stalled, then clear the request.

        // line 75
        ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwLastObservedExecuteUIDTime = Blizzard::Time::Seconds();
        // line 76
        ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->bAbortLuaScript = 0;
    }
    // Installed with mask 7 (line, call and return hooks), so this runs
    // constantly while a script executes: the fast path is a single pointer
    // load and a test, and the abort path above is a handful of stores. The
    // hook itself is the last line of defence against a script that wedges
    // the main thread -- LuaAbortThreadWatcher sets bAbortLuaScript when a
    // game thread's script has not advanced for SCRIPT_ABORT_TIME_SECONDS,
    // and the next hook call on that thread runs the abort path above.
}  // line 88

// line 92
inline LuaHook::LuaHook() : aAttachedLuaStates() {}

// line 96
inline LuaHook::~LuaHook()
{
    size_t nSize = aAttachedLuaStates.size();
    for (uint32 i = 0; i < nSize; ++i)
    {
        lua_sethook(aAttachedLuaStates[i], sLuaHook, 0, 0);
    }
    aAttachedLuaStates.clear();
}

// line 108
void LuaHook::AttachToLuaState(lua_State * pLuaState)
{
    // Watch this state for the abort hook. Mask 7 is the line, call and
    // return hooks (LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE), so sLuaHook
    // runs constantly while the script executes and can honour the watcher
    // thread's abort request; count 0 disables the instruction-count hook.
    // line 110
    lua_sethook(pLuaState, sLuaHook, 7, 0);
    // line 111
    aAttachedLuaStates.push_back(pLuaState);
}  // line 112

// line 116
void LuaHook::DetachFromLuaState(lua_State * pLuaState)
{
    // line 118
    Stopwatch tStopwatch;
    // line 120
    float flTime = 0.0f;
    // line 121
    size_t nSize = aAttachedLuaStates.size();
    // line 122
    for (uint32 i = 0; i < nSize; ++i)
    {
        // line 124
        if (aAttachedLuaStates[i] == pLuaState)
        {
            // line 126
            if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
            {
                // line 128
                flTime = tStopwatch.Check();
                // line 130
                tStopwatch.Reset();
            }
            // Drop the hook installed by AttachToLuaState: mask 0 clears every
            // hook, so sLuaHook stops running on this state.
            // line 133
            lua_sethook(pLuaState, sLuaHook, 0, 0);
            // line 134
            if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
            {
                // line 136
                flTime = tStopwatch.Check();
                // line 138
                tStopwatch.Reset();
            }
            // line 141
            blzext::vector_erase_at(aAttachedLuaStates, i);
            // line 143
            if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
            {
                // line 145
                flTime = tStopwatch.Check();
                // line 147
                tStopwatch.Reset();
            }
            // line 150
            return;
        }
    }
}  // line 152
