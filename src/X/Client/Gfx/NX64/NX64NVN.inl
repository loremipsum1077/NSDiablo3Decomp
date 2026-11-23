// Original file: X/Client/Gfx/NX64/NX64NVN.inl
//
// Trimmed to the resource-age helpers. The bodies are not in the DWARF; they are inferred from
// the code inlined into NX64NVNShaders.cpp. MAX_NVN_EVENT_LATENCY is NX64NVN.h:42, its value
// is the 4 the binary compares against.

#pragma once

#include "../../CGame.h"

static const uint32 MAX_NVN_EVENT_LATENCY = 4;

// line 33
template <class T>
inline void NVNMarkResourceReadyForDelete(T * ptResource) {
    ptResource->dwLastFrameUsed = CGameAsyncGetDrawFrameCount() - MAX_NVN_EVENT_LATENCY;
}

// line 39
template <class T>
inline void NVNMarkResourceUsedThisFrame(T * ptResource) {
    ptResource->dwLastFrameUsed = CGameAsyncGetDrawFrameCount();
}

// line 45
template <class T>
inline BOOL NVNIsResourceBusy(const T * ptResource) {
    return (CGameAsyncGetDrawFrameCount() - ptResource->dwLastFrameUsed) < MAX_NVN_EVENT_LATENCY;
}
