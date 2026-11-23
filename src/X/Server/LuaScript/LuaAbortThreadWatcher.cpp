// Original file: X/Server/LuaScript/LuaAbortThreadWatcher.cpp -- the watcher
// thread that aborts runaway Lua scripts, and the per-thread block it keeps in
// `ThreadGlobals::m_ptLuaScriptThreadGlobals`.

#include "LuaAbortThreadWatcher.h"

#include "../../../BlizzardCore/Include/Blizzard/Debug.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../SGameManager.h"

// line 22
LuaDeadlockGameThreadData::LuaDeadlockGameThreadData() {
    bAbortLuaScript = 0;
    dwExecuteUID = 0;
    dwExecuteCount = 0;
    dwLastObservedExecuteUID = 0;
    dwLastObservedExecuteUIDTime = 0;
}

// line 32
LuaScriptThreadGlobals * LuaScriptThreadGlobalsGet() {
    LuaScriptThreadGlobals *& ptLuaScriptThreadGlobals = ThreadGlobals::Get().m_ptLuaScriptThreadGlobals;
    if (ptLuaScriptThreadGlobals == NULL)
        ptLuaScriptThreadGlobals = (LuaScriptThreadGlobals *)SigmaMemoryNew(0x218, 4, NULL, TRUE);
    return ptLuaScriptThreadGlobals;
}

// line 45
void LuaScriptDisposeThreadGlobals() {
    LuaScriptThreadGlobals *& ptLuaScriptThreadGlobals = ThreadGlobals::Get().m_ptLuaScriptThreadGlobals;
    if (ptLuaScriptThreadGlobals != NULL) {
        if (ptLuaScriptThreadGlobals->ptDeadlockGameThreadData != NULL)
            ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwExecuteUID = 0;
        SigmaMemoryFree(ptLuaScriptThreadGlobals, NULL);
        ptLuaScriptThreadGlobals = NULL;
    }
}

// line 63
uint32 LuaDeadlockThreadProc(void * lpData) {
    const uint32 HALF_MAX_UINT = 0x7FFFFFFF;
    const int32 SCRIPT_ABORT_TIME_SECONDS = 5;
    LuaDeadlockThreadData * ptDeadlockThreadData = (LuaDeadlockThreadData *)lpData;
    uint32 uCurrentSeconds = Blizzard::Time::Seconds();
    for (uint32 i = 0; i < ptDeadlockThreadData->taGameThreadData.size(); ++i) {
        ptDeadlockThreadData->taGameThreadData[i]->dwLastObservedExecuteUID = 0;
        ptDeadlockThreadData->taGameThreadData[i]->dwLastObservedExecuteUIDTime = uCurrentSeconds;
    }
    uint32 uLastMainThreadFrameCount = 0;
    uint32 uLastMainThreadFrameCountUpdateTime = uCurrentSeconds;
    while (ptDeadlockThreadData->hExitThreadEvent.Wait(1000) != &ptDeadlockThreadData->hExitThreadEvent) {
        uCurrentSeconds = Blizzard::Time::Seconds();
        for (uint32 i = 0; i < ptDeadlockThreadData->taGameThreadData.size(); ++i) {
            LuaDeadlockGameThreadData & tGameThreadData = *ptDeadlockThreadData->taGameThreadData[i];
            if (tGameThreadData.dwExecuteUID == tGameThreadData.dwLastObservedExecuteUID) {
                if (tGameThreadData.dwExecuteUID != 0
                    && uCurrentSeconds - tGameThreadData.dwLastObservedExecuteUIDTime > SCRIPT_ABORT_TIME_SECONDS
                    && !tGameThreadData.bAbortLuaScript) {
                    Blizzard::Debug::AmIBeingDebugged();
                }
            } else {
                tGameThreadData.dwLastObservedExecuteUID = tGameThreadData.dwExecuteUID;
                tGameThreadData.dwLastObservedExecuteUIDTime = uCurrentSeconds;
            }
        }
        if (SGameManager::GetGameCount() != 0 && ptDeadlockThreadData->bCheckForMainThreadStall) {
            if (ptDeadlockThreadData->uMainLoopUpdateCount == uLastMainThreadFrameCount) {
                uint32 uStalledSeconds = uCurrentSeconds - uLastMainThreadFrameCountUpdateTime;
                if (uStalledSeconds > s_varStalledMainThreadThresholdSeconds.Get()
                    && uStalledSeconds < HALF_MAX_UINT) {
                    Blizzard::Debug::AmIBeingDebugged();
                }
            } else {
                uLastMainThreadFrameCount = ptDeadlockThreadData->uMainLoopUpdateCount;
                uLastMainThreadFrameCountUpdateTime = uCurrentSeconds;
            }
        }
    }
    return 0;
}
