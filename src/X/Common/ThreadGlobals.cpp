// Original file: X/Common/ThreadGlobals.cpp

#include "../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "SNO/SNOName.h"
#include "ThreadGlobals.h"

#include "../Client/Gfx/GfxVB.h"
#include "../Sigma/Memory/SigmaMemory.h"

// Defined in X/Server/DRLGDungeon.cpp:191, X/Common/DRLG.cpp:106,
// X/Server/LuaScript/LuaAbortThreadWatcher.cpp:45 and X/Common/Nav/NavMeshStitch.cpp:637.
// The headers that declare them are not in src/ yet.
void DRLGDungeonDisposeGlobals();
void DRLGDisposeGlobals();
void LuaScriptDisposeThreadGlobals();
void NavmeshSitchingDisposeThreadGlobals();

#include "Game.h"
struct GameCommonData;




static_assert(sizeof(ActiveGameCommonDataGlobals) == 0x10, "ActiveGameCommonDataGlobals");

// The original include closure puts blz::ignore and SNONAME_NONE ahead of WorldPlace.h's
// cgplaceNull, and GfxVB.h's RGBAColor normals into this file's static initializer.
SigmaThreadLocalStorage ThreadGlobals::s_tThreadData;  // line 28

// line 33. Inlined into both Initialize functions; there is no out-of-line copy. The member
// order decides how the stores pair up: the line table puts m_ptLuaScriptThreadGlobals with the
// NavMesh/ACDColl stores (line 55), and m_ptSNONameThreadGlobals after the two ints.
ThreadGlobals::ThreadGlobals() {
    m_ptActiveGameCommonData = (ActiveGameCommonDataGlobals *)SigmaMemoryNew(sizeof(ActiveGameCommonDataGlobals), 4, NULL, TRUE);
    m_ptActiveGameCommonData->bClientActive = TRUE;

    m_ptDRLGDungeonGlobals = NULL;
    m_ptDRLGGlobals = NULL;

    m_nPowerFormulaStackCount = 0;
    m_idUtilityGame = -1;
    m_ptSNONameThreadGlobals = NULL;

    m_idSGameCurrent = -1;
    m_ptSGameCurrent = NULL;
    m_bSimulatingGame = FALSE;

    m_pauEncodeBuffer = NULL;
    m_pauDecodeBuffer = NULL;

    m_ptLuaScriptThreadGlobals = NULL;
    m_ptNavMeshStitchThreadGlobals = NULL;
    m_ptACDCollThreadGlobals = NULL;

    m_pSpriteThreadBuffer = NULL;
    m_pDataIDMemoryPool = NULL;

    m_nThreadIndex = THREADINDEX_COUNT;

    m_ptDeflateZstream = NULL;
    m_ptInflateZstream = NULL;
    m_pauCompressionBuffer = NULL;
}

// line 77
ThreadGlobals::~ThreadGlobals() {
    SigmaMemoryFree(m_ptActiveGameCommonData, NULL);
    m_ptActiveGameCommonData = NULL;

    DRLGDungeonDisposeGlobals();

    DRLGDisposeGlobals();

    LuaScriptDisposeThreadGlobals();

    NavmeshSitchingDisposeThreadGlobals();
    SigmaMemoryFree(m_pauEncodeBuffer, NULL);
    m_pauEncodeBuffer = NULL;
    SigmaMemoryFree(m_pauDecodeBuffer, NULL);
    m_pauDecodeBuffer = NULL;
    SigmaMemoryFree(m_ptACDCollThreadGlobals, NULL);
    m_ptACDCollThreadGlobals = NULL;
    SNONameDisposeThreadGlobals();

    if (m_ptDeflateZstream != NULL) {
        deflateEnd(m_ptDeflateZstream);
        SigmaMemoryFree(m_ptDeflateZstream, NULL);
        m_ptDeflateZstream = NULL;
    }
    if (m_ptInflateZstream != NULL) {
        inflateEnd(m_ptInflateZstream);
        SigmaMemoryFree(m_ptInflateZstream, NULL);
        m_ptInflateZstream = NULL;
    }
    SigmaMemoryFree(m_pauCompressionBuffer, NULL);
    m_pauCompressionBuffer = NULL;
}

// line 110
void ThreadGlobals::InitializeForApp() {
    s_tThreadData.Initialize();

    InitializeForThread();

    Get().m_nThreadIndex = THREADINDEX_MAIN;
}

// line 119
void ThreadGlobals::DisposeForApp() {
    DisposeForThread();

    s_tThreadData.Dispose();
}

// line 126
void ThreadGlobals::InitializeForThread() {
    ThreadGlobals * ptThreadGlobals = new ThreadGlobals;

    s_tThreadData.TLSSetValue(ptThreadGlobals);
}

// line 143
void ThreadGlobals::DisposeForThread() {
    ThreadGlobals * ptThreadGlobals = (ThreadGlobals *)s_tThreadData.TLSGetValue();

    delete ptThreadGlobals;

    s_tThreadData.TLSSetValue(NULL);
}
