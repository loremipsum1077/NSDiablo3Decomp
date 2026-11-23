// Original file: X/Sigma/DebugChannelInterface/XDebugChannelMemory.cpp
//
// functions.csv assigns four out-of-line functions (Init, Update, ~MutexWrapper and the static
// initialiser). The original file also defined XDCMemory::PostBasicMemStats (line 26),
// XDebugChannelMemoryDispose (line 63), XDebugChannelMemoryLogGrowth (line 114),
// XDebugChannelMemoryLogAlloc (line 140), XDebugChannelMemorySuspend (line 162) and
// XDebugChannelMemoryResume (line 172); the linker stripped all of them (their DWARF in
// build/dwarf/cu_07563a76.txt has low_pc 0, so they have no bytes in main.elf to compare against).
//
// In cu_07563a76.txt, MutexWrapper::Enter (line 97) inlines Blizzard::Lock::Mutex::Enter at line 99
// and MutexWrapper::Leave (line 102) inlines Blizzard::Lock::Mutex::Leave at line 104, placing the
// s_bMutexActive guard inside Enter/Leave. Suspend (line 162, size 0x60, Enter inlined at line 166
// [0x30..0x44), Leave at line 168 [0x50..0x60)) and Resume (line 172, size 0x60, Enter inlined at
// line 176 [0x30..0x44), Leave at line 178 [0x50..0x60)) are written below so that Enter/Leave are
// instantiated and s_bMutexActive is read in this translation unit (otherwise GlobalOpt deletes the
// stores to s_bMutexActive in MutexWrapper() and ~MutexWrapper()), and s_bXDebugMemorySuspended is
// preserved at _MergedGlobals+0x4 (matching DW_AT_location 0x195b620+0x4 in cu_07563a76.txt).

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
// blz/tuple.h for blz::ignore, which sits at the front of the merged statics block and shifts
// this file's statics (the original compile unit reaches it through the blz include chain).
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "XDebugChannelInterface.h"

class XDCMemory {  // size 0x1
public:
    void PostBasicMemStats();
};
static_assert(sizeof(XDCMemory) == 0x1, "XDCMemory");

static XDCMemory s_sXDCMemory;

static BOOL s_bMemoryLogEnabled = FALSE;

void XDebugChannelMemoryInit()
{
    if (!s_bMemoryLogEnabled) {
        if (g_pDebugChannel != NULL) {
            XDebugMessage tMsg(XDM_MEMORY_ENABLE);
        }
        s_bMemoryLogEnabled = TRUE;
    }
}

void XDebugChannelMemoryUpdate()
{
}

static uint32 s_bXDebugMemorySuspended;

static BOOL s_bMutexActive;

struct MutexWrapper {  // size 0x20
    MutexWrapper()
    {
        s_bMutexActive = TRUE;
    }

    ~MutexWrapper()
    {
        s_bMutexActive = FALSE;
    }

    void Enter()
    {
        if (s_bMutexActive) {
            m_tMemoryLogGrowthMutex.Enter();
        }
    }

    void Leave()
    {
        if (s_bMutexActive) {
            m_tMemoryLogGrowthMutex.Leave();
        }
    }

private:
    Blizzard::Lock::RMutex m_tMemoryLogGrowthMutex;  // 0x00
};
static_assert(sizeof(MutexWrapper) == 0x20, "MutexWrapper");

static MutexWrapper s_tMemoryLogGrowthMutex;

// LINKER-REMOVED: line 162. Stripped by the linker (DWARF: low_pc 0, size 0x60); the body cannot be checked, so
// only the shape its DWARF records is written (MutexWrapper::Enter inlined at line 166 and Leave
// at line 168, no locals). It is what keeps s_bMutexActive read in this translation unit.
void XDebugChannelMemorySuspend()
{
    if (!s_bMemoryLogEnabled || g_pDebugChannel == NULL) {
        return;
    }
    s_tMemoryLogGrowthMutex.Enter();
    s_bXDebugMemorySuspended++;
    s_tMemoryLogGrowthMutex.Leave();
}

// LINKER-REMOVED: line 172. Also stripped by the linker (size 0x60); Enter inlined at line 176, Leave at line 178.
void XDebugChannelMemoryResume()
{
    if (!s_bMemoryLogEnabled || g_pDebugChannel == NULL) {
        return;
    }
    s_tMemoryLogGrowthMutex.Enter();
    s_bXDebugMemorySuspended--;
    s_tMemoryLogGrowthMutex.Leave();
}
