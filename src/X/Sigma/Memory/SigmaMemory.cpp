#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../BlizzardCore/Include/Blizzard/Using.h"
#include "../DataStructures/XGrowableMap.h"
#include "../DataStructures/XLocalHeap.h"
#include "../DataStructures/XRemoteHeap.h"
#include "../DataStructures/XVirtualHeap.h"
#include "../ErrorManager/ErrorManager.h"
#include "SigmaMemory.h"
#include "../SigmaTypes.h"
#include "../Unicode.h"
#include <new>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

class SigmaCriticalSection;

// BlizzardCore/Include/Blizzard/Memory.h; defined in X/Sigma/CoreGlue/SigmaMemoryGlue.cpp.
namespace Blizzard {
namespace Memory {
void Shutdown();
}  // namespace Memory
}  // namespace Blizzard

#include "../../../NintendoSDK/Include/nn/nn_Result.h"
namespace nn {









namespace os {
Result AllocateAddressRegion(uintptr_t* pOutAddress, size_t size);
Result AllocateMemoryPages(uintptr_t address, size_t size);
}  // namespace os
}  // namespace nn

struct MemPoolEntry {
    LPCSTR szFile;
    int32 nLine;
    LPCSTR szDebugString;
    int32 nAllocNumber;
    XMemoryAllocatorInterface* ptMemoryInterface;
    size_t uSize;
    uint16 uFlags;
    uint16 uAlignment;
};

typedef XGrowableMap<void*, void*, MemPoolEntry, const MemPoolEntry> MemMap;

struct MemPool {
    MemMap* ptMap;
    uintptr uMinAddress;
    uintptr uMaxAddress;
    SigmaMemoryTotals tMemoryTotals;
    int32 nAllocNumber;
    int32 nSnapshotEntries;
    MemPoolEntry* ptSnapshot;
    SigmaCriticalSection* cs;
};

struct MemoryGlobals {
    MemPool tPool;
    BOOL fAfterMain;
    XMemoryAllocatorInterface* ptOSMemoryAllocator;
    SigmaMemoryAllocationRetryFn pRetryFn;
};

// The names of these sizes are not in the DWARF.
static const size_t MAIN_MEMORY_HEAP_SIZE = 0x20000000;
static const size_t SMALL_SUB_HEAP_SIZE = 0x2000000;
static const size_t SMALL_SUB_HEAP_MAX_ALLOC_SIZE = 0x800;
static const uint32 FMOD_MEMORY_BUFFER_SIZE = 0x8000000;
static const size_t DEFINITIONS_MEMORY_HEAP_SIZE = 0x58000000;
static const size_t GRAPHICS_PERSISTENT_MEMORY_SIZE = 0x17400000;
static const size_t SHADERS_PERSISTENT_MEMORY_SIZE = 0x1000000;
static const size_t REMOTE_HEAP_MIN_ALLOC_SIZE = 0x1000;

off_t stMainMemoryHeapPhysOffset;

void* gpFMODMemoryBuffer;
uint32 gdwFMODMemoryBufferSize;

static void* spDefinitionsMemoryHeapMem;
XVirtualHeap* gptMainDefinitionsMemoryHeap;

XBaseHeap* gptSecondaryDefinitionsMemoryHeap;

static void* spGraphicsPersistentMemoryVirtAddr;
static void* spGraphicsPersistentMemoryHeapMem;
XRemoteHeap* gptGraphicsPersistentMemoryHeap;

static void* spShadersPersistentMemoryVirtualAddress;
static void* spShadersPersistentMemoryHeapAddress;
XRemoteHeap* gptShaderHeap;

XLocalHeap* gptMainMemoryHeap;

int32 BREAK_ON_ALLOCATION_NUMBER;
int32 BREAK_ON_DEALLOCATION_NUMBER;

BOOL TRACE_ALLOCATIONS;

static LPCSTR scszUnknownFilename;

static MemoryGlobals* sgptMemoryGlobals;

static void sSigmaMemoryReallyDispose();
// LINKER-REMOVED: DWARF size 0x10; address-0 rows: a 2-instruction store at 410:16 (`dwHeapSize =`) before the malloc at 411:20; the value 0x20000000 is the one the inlined copy in sSigmaMemoryInit passes.
uint8* sAllocateLocalHeapBuffer(size_t& dwHeapSize) {
    dwHeapSize = MAIN_MEMORY_HEAP_SIZE;
    return (uint8*)malloc(dwHeapSize);
}
static void sSigmaMemoryInit() {
    size_t dwSize = MAIN_MEMORY_HEAP_SIZE;
    uint8* pBuffer = sAllocateLocalHeapBuffer(dwSize);
    void* pMainMemoryHeapMem = pBuffer;
    pBuffer += sizeof(XLocalHeap);
    dwSize -= sizeof(XLocalHeap);
    gptMainMemoryHeap = new (pMainMemoryHeapMem) XLocalHeap(pBuffer, dwSize, 32, 0);
    gptMainMemoryHeap->CreateSmallSubHeap(SMALL_SUB_HEAP_SIZE, SMALL_SUB_HEAP_MAX_ALLOC_SIZE);

    sgptMemoryGlobals = (MemoryGlobals*)gptMainMemoryHeap->MemAlloc(sizeof(MemoryGlobals), 32, FALSE);
    SigmaMemoryClear(sgptMemoryGlobals, sizeof(MemoryGlobals));
    sgptMemoryGlobals->ptOSMemoryAllocator = gptMainMemoryHeap;

    gdwFMODMemoryBufferSize = FMOD_MEMORY_BUFFER_SIZE;
    gpFMODMemoryBuffer = malloc(gdwFMODMemoryBufferSize);

    spDefinitionsMemoryHeapMem = malloc(sizeof(XVirtualHeap));
    gptMainDefinitionsMemoryHeap = new (spDefinitionsMemoryHeapMem)
        XVirtualHeap(gptMainMemoryHeap, DEFINITIONS_MEMORY_HEAP_SIZE, XVIRTUAL_HEAP_TYPE_CPU_RW_GPU_R);
    gptSecondaryDefinitionsMemoryHeap = NULL;

    {
        uintptr_t address = 0;
        {
            nn::Result nn_result = nn::os::AllocateAddressRegion(&address, GRAPHICS_PERSISTENT_MEMORY_SIZE);
        }
        {
            nn::Result nn_result = nn::os::AllocateMemoryPages(address, GRAPHICS_PERSISTENT_MEMORY_SIZE);
        }
        spGraphicsPersistentMemoryVirtAddr = (void*)address;
    }
    spGraphicsPersistentMemoryHeapMem = malloc(sizeof(XRemoteHeap));
    gptGraphicsPersistentMemoryHeap = new (spGraphicsPersistentMemoryHeapMem) XRemoteHeap(
        NULL, spGraphicsPersistentMemoryVirtAddr, GRAPHICS_PERSISTENT_MEMORY_SIZE, REMOTE_HEAP_MIN_ALLOC_SIZE, 0);

    {
        uintptr_t address = 0;
        {
            nn::Result nn_result = nn::os::AllocateAddressRegion(&address, SHADERS_PERSISTENT_MEMORY_SIZE);
        }
        {
            nn::Result nn_result = nn::os::AllocateMemoryPages(address, SHADERS_PERSISTENT_MEMORY_SIZE);
        }
        spShadersPersistentMemoryVirtualAddress = (void*)address;
    }
    spShadersPersistentMemoryHeapAddress = malloc(sizeof(XRemoteHeap));
    gptShaderHeap = new (spShadersPersistentMemoryHeapAddress) XRemoteHeap(
        NULL, spShadersPersistentMemoryVirtualAddress, SHADERS_PERSISTENT_MEMORY_SIZE, REMOTE_HEAP_MIN_ALLOC_SIZE, 0);
}

void SigmaMemoryInit() {
    if (sgptMemoryGlobals == NULL) {
        sSigmaMemoryInit();
    }
    sgptMemoryGlobals->fAfterMain = TRUE;

    Blizzard::Using::StartUsing("Blizzard::Memory");
    Blizzard::Using::Register("Blizzard::Memory", sSigmaMemoryReallyDispose);
    Blizzard::Using::StartUsing("ErrorManager::Logging");
}

void SigmaMemoryDispose() {
    Blizzard::Using::StopUsing("Blizzard::Memory");
}

static void sSigmaMemoryReallyDispose() {
    if (!SigmaCurrentlyHandlingError()) {
        if (sgptMemoryGlobals == NULL) {
            return;
        }
        Blizzard::Memory::Shutdown();
    }
    Blizzard::Using::StopUsing("ErrorManager::Logging");
}

void SigmaMemoryOverflowCheck() {
}

void SigmaMemoryGetTotals(SigmaMemoryTotals& tMemTotals) {
    SigmaMemoryClear(&tMemTotals, sizeof(tMemTotals));
}

void SigmaMemoryDumpOSStats() {
    gptMainMemoryHeap->PrintUsage();
    gptMainDefinitionsMemoryHeap->PrintUsage();
    gptSecondaryDefinitionsMemoryHeap->PrintUsage();
}

BOOL SigmaMemorySetTraceAllocations(BOOL bEnable) {
    BOOL bPrev = TRACE_ALLOCATIONS;
    TRACE_ALLOCATIONS = bEnable;
    return bPrev;
}

static void sReportOutOfMemoryError(const char* szMessage, size_t size) {
    CHAR szTemp[256];
    SigmaMemoryDumpOSStats();
    sprintf(szTemp, szMessage, (int32)size);
    DisplayErrorMessage(ERROR_OUT_OF_MEMORY, szTemp);
}

void* SigmaMemoryNew(size_t size, size_t align, XMemoryAllocatorInterface* ptMemoryInterface, const BOOL fClear) {
    if (sgptMemoryGlobals == NULL) {
        sSigmaMemoryInit();
    }

    if (align <= 32) {
        align = 32;
    }
    if (ptMemoryInterface == NULL) {
        ptMemoryInterface = sgptMemoryGlobals->ptOSMemoryAllocator;
    }

    void* ptResult = ptMemoryInterface->MemAlloc(size, align, fClear);
    if (ptResult == NULL) {
        if (sgptMemoryGlobals->pRetryFn != NULL) {
            while (sgptMemoryGlobals->pRetryFn(ptMemoryInterface)) {
                ptResult = ptMemoryInterface->MemAlloc(size, align, fClear);
                if (ptResult != NULL) {
                    return ptResult;
                }
            }
        }
        sReportOutOfMemoryError("Unable to allocate %d bytes", size);
    }
    return ptResult;
}

void SigmaMemoryFree(void* lp, XMemoryAllocatorInterface* ptMemoryInterface) {
    if (lp == NULL || sgptMemoryGlobals == NULL) {
        return;
    }

    if (ptMemoryInterface == NULL) {
        ptMemoryInterface = sgptMemoryGlobals->ptOSMemoryAllocator;
    }
    ptMemoryInterface->MemFree(lp);
}

void* SigmaMemoryResize(void* lp, XMemoryAllocatorInterface* ptMemoryInterface, size_t size, size_t align) {
    if (lp == NULL) {
        return SigmaMemoryNew(size, align, ptMemoryInterface, FALSE);
    }

    if (align <= 32) {
        align = 32;
    }
    if (ptMemoryInterface == NULL) {
        ptMemoryInterface = sgptMemoryGlobals->ptOSMemoryAllocator;
    }

    void* ptNewBlock = ptMemoryInterface->MemResize(lp, size, align);
    if (ptNewBlock == NULL) {
        if (sgptMemoryGlobals->pRetryFn != NULL) {
            while (sgptMemoryGlobals->pRetryFn(ptMemoryInterface)) {
                ptNewBlock = ptMemoryInterface->MemResize(lp, size, align);
                if (ptNewBlock != NULL) {
                    return ptNewBlock;
                }
            }
        }
        sReportOutOfMemoryError("Unable to re-allocate to %d bytes", size);
    }
    return ptNewBlock;
}

void SigmaMemoryCopy(void* lpDest, const void* lpSrc, size_t size) {
    CopyMemory(lpDest, lpSrc, size);
}

void SigmaMemoryMove(void* lpDest, void* lpSrc, size_t size) {
    MoveMemory(lpDest, lpSrc, size);
}

void SigmaMemoryClear(void* lp, size_t size) {
    ZeroMemory(lp, size);
}

void SigmaMemoryFill(void* lp, uint8 bFill, size_t size) {
    FillMemory(lp, size, bFill);
}

void* operator new(size_t size) {
    LPCSTR pFilename = scszUnknownFilename;
    return SigmaMemoryNew(size, 32, NULL, FALSE);
}

void* operator new[](size_t size) {
    return SigmaMemoryNew(size, 32, NULL, FALSE);
}

void* operator new(size_t size, const std::nothrow_t&) {
    return SigmaMemoryNew(size, 32, NULL, FALSE);
}

void* operator new[](size_t size, const std::nothrow_t&) {
    return SigmaMemoryNew(size, 32, NULL, FALSE);
}

void operator delete(void* pData) {
    SigmaMemoryFree(pData, NULL);
}

void operator delete[](void* pData) {
    SigmaMemoryFree(pData, NULL);
}

void operator delete(void* pData, const std::nothrow_t&) {
    SigmaMemoryFree(pData, NULL);
}

void operator delete[](void* pData, const std::nothrow_t&) {
    SigmaMemoryFree(pData, NULL);
}
