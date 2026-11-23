// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/ThreadGlobals.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/Zlib/zlib.h"
#include "CommonTypes.h"
#include "WorldPlace.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/SigmaThreadLocalStorage.h"
#include "../Sigma/SigmaTypes.h"

struct ACDCollThreadGlobals;
struct ActiveGameCommonDataGlobals;
struct DRLGDungeonGlobals;
struct DRLGGlobals;
struct LuaScriptThreadGlobals;
struct NavMeshStitchThreadGlobals;
struct SGame;
struct SNONameThreadGlobals;
class XCommandBuffer;

typedef int32 DataID;

typedef XListMemoryPool<int> DataIDMemoryPool;

enum ThreadIndex {
    THREADINDEX_MAIN = 0,
    THREADINDEX_SERVERWORKER = 1,
    THREADINDEX_ASYNCRENDER = 2,
    THREADINDEX_ASYNCLOAD = 3,
    THREADINDEX_ASYNCPRELOAD = 4,
    THREADINDEX_ASYNCAUDIO = 5,
    THREADINDEX_JOBWORKER = 6,
    THREADINDEX_DEFRAG = 7,
    THREADINDEX_SERVERIDLE = 8,
    THREADINDEX_CUTSCENE_ASYNC_LOAD = 9,
    THREADINDEX_CUTSCENE_ASYNC_DECODE = 10,
    THREADINDEX_SAVELOADTASK = 11,
    THREADINDEX_NETWORK = 12,
    THREADINDEX_TROPHY = 13,
    THREADINDEX_COUNT = 14,
};

class ThreadGlobals {  // size 0xa8
public:
    ThreadGlobals();
    ~ThreadGlobals();
    static void InitializeForApp();
    static void DisposeForApp();
    static void InitializeForThread();
    static void DisposeForThread();
    static ThreadGlobals * TryToGet();
    static ThreadGlobals & Get();
    ActiveGameCommonDataGlobals * m_ptActiveGameCommonData;  // 0x00
    DRLGDungeonGlobals * m_ptDRLGDungeonGlobals;  // 0x08
    DRLGGlobals * m_ptDRLGGlobals;  // 0x10
    LuaScriptThreadGlobals * m_ptLuaScriptThreadGlobals;  // 0x18
    NavMeshStitchThreadGlobals * m_ptNavMeshStitchThreadGlobals;  // 0x20
    ACDCollThreadGlobals * m_ptACDCollThreadGlobals;  // 0x28
    SNONameThreadGlobals * m_ptSNONameThreadGlobals;  // 0x30
    int m_nPowerFormulaStackCount;  // 0x38
    SGameID m_idUtilityGame;  // 0x3c
    WorldPlace m_tUtilityPlace;  // 0x40
    SGameID m_idSGameCurrent;  // 0x50
    SGame * m_ptSGameCurrent;  // 0x58
    BOOL m_bSimulatingGame;  // 0x60
    uint8 * m_pauEncodeBuffer;  // 0x68
    uint8 * m_pauDecodeBuffer;  // 0x70
    z_streamp m_ptDeflateZstream;  // 0x78
    z_streamp m_ptInflateZstream;  // 0x80
    uint8 * m_pauCompressionBuffer;  // 0x88
    XCommandBuffer * m_pSpriteThreadBuffer;  // 0x90
    DataIDMemoryPool * m_pDataIDMemoryPool;  // 0x98
    ThreadIndex m_nThreadIndex;  // 0xa0
private:
    static SigmaThreadLocalStorage s_tThreadData;
};
static_assert(sizeof(ThreadGlobals) == 0xa8, "ThreadGlobals");

inline ThreadGlobals * ThreadGlobals::TryToGet() {  // line 78
    if (!s_tThreadData.IsInitialized())
        return NULL;
    return (ThreadGlobals *)s_tThreadData.TLSGetValue();
}

inline ThreadGlobals & ThreadGlobals::Get() {  // line 84
    return *(ThreadGlobals *)s_tThreadData.TLSGetValue();
}


// Functions defined in this file (bodies are not in the DWARF):
//   line    78  ThreadGlobals * ThreadGlobals::TryToGet()  [inlined into 13 functions]
//   line    84  ThreadGlobals & ThreadGlobals::Get()  [inlined into 4417 functions]
