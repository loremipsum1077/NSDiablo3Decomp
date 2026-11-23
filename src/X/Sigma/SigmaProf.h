#pragma once

#include "../../BlizzardCore/Include/Blizzard/Thread.h"
#include "SigmaThreadLocalStorage.h"
#include "SigmaTypes.h"
#include "Unicode.h"

enum {
    MAX_SIGMA_PROF_STACK_SIZE = 32,
    MAX_SIGMA_PROF_CATEGORIES = 512,
    MAX_SIGMA_PROF_THREAD_NAME_LENGTH = 64,
    MAX_SIGMA_PROF_THREAD_COUNT = 64,
    SIGMA_PROF_NO_PARENT = 65535,
    MAX_SIGMA_PROF_SUB_CATEGORIES = 32,
};

class SigmaProfCategory {
public:
    SigmaProfCategory(LPCSTR, int32, LPCSTR);
    const int32& GetCategoryIndex();
    LPCSTR GetName();
    LPCSTR GetUserDataDesc();
    int32 GetUserDataType();

protected:
    int32 m_nIndex;
    int32 m_eDataType;
    LPCSTR m_szName;
    LPCSTR m_szUserDataDesc;
};

typedef uint32 SigmaProfEntry;

struct SigmaProfThread {
    Blizzard::Thread::Thread ptThread;
};

struct SigmaProfGlobals {
    uint8 m_bProfEnabled;
    SigmaThreadLocalStorage m_tThreadData;
    SigmaProfThread m_arProfThreads[MAX_SIGMA_PROF_THREAD_COUNT];
    int m_nProfThreadCount;
    SigmaProfCategory* m_arProfCategories[MAX_SIGMA_PROF_CATEGORIES];
    int m_nProfCategoryCount;
};

extern SigmaProfGlobals gtSigmaProfGlobals;

void SigmaProfInitializeForApp();
void SigmaProfDisposeForApp();
void SigmaProfNewFrame(Blizzard::Thread::Thread ptThread);
void SigmaProfInitializeForThread();
void SigmaProfDisposeForThread();
void SigmaProfRegisterThread(Blizzard::Thread::Thread ptThread, LPCSTR szName, uint16 nProfBufferSize);
void SigmaProfUnregisterThread(Blizzard::Thread::Thread ptThread);
SigmaProfThread* SigmaProfFindThread(Blizzard::Thread::Thread ptThread);

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(SigmaProfCategory) == 0x18, "SigmaProfCategory");
static_assert(sizeof(SigmaProfThread) == 0x8, "SigmaProfThread");
static_assert(offsetof(SigmaProfThread, ptThread) == 0x0, "SigmaProfThread");
static_assert(sizeof(SigmaProfGlobals) == 0x1228, "SigmaProfGlobals");
static_assert(offsetof(SigmaProfGlobals, m_bProfEnabled) == 0x0, "SigmaProfGlobals");
static_assert(offsetof(SigmaProfGlobals, m_tThreadData) == 0x8, "SigmaProfGlobals");
static_assert(offsetof(SigmaProfGlobals, m_arProfThreads) == 0x18, "SigmaProfGlobals");
static_assert(offsetof(SigmaProfGlobals, m_nProfThreadCount) == 0x218, "SigmaProfGlobals");
static_assert(offsetof(SigmaProfGlobals, m_arProfCategories) == 0x220, "SigmaProfGlobals");
static_assert(offsetof(SigmaProfGlobals, m_nProfCategoryCount) == 0x1220, "SigmaProfGlobals");
