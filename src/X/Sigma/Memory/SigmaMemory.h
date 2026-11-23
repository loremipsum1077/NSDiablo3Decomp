#pragma once

#include <stddef.h>
#include <string.h>

#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../SigmaTypes.h"

class XMemoryAllocatorInterface;

struct SigmaMemoryTotals {  // size 0x20
    uintptr uCurrentMemUsed;  // 0x00
    uintptr uPeakMemUsed;  // 0x08
    uintptr uGlobalMemUsed;  // 0x10
    uintptr uSGameMemUsed;  // 0x18
};
static_assert(sizeof(SigmaMemoryTotals) == 0x20, "SigmaMemoryTotals");

typedef BOOL (*SigmaMemoryAllocationRetryFn)(XMemoryAllocatorInterface*);

void SigmaMemoryInit();
void SigmaMemoryDispose();
void SigmaMemoryOverflowCheck();
void SigmaMemoryGetTotals(SigmaMemoryTotals& tMemTotals);
void SigmaMemoryClear(void* lp, size_t size);
void SigmaMemoryDumpOSStats();
BOOL SigmaMemorySetTraceAllocations(BOOL bEnable);
void* SigmaMemoryNew(size_t size, size_t align, XMemoryAllocatorInterface* ptMemoryInterface, const BOOL fClear);
void SigmaMemoryFree(void* lp, XMemoryAllocatorInterface* ptMemoryInterface);
void* SigmaMemoryResize(void* lp, XMemoryAllocatorInterface* ptMemoryInterface, size_t size, size_t align);
void SigmaMemoryCopy(void* lpDest, const void* lpSrc, size_t size);
void SigmaMemoryMove(void* lpDest, void* lpSrc, size_t size);
void SigmaMemoryFill(void* lp, uint8 bFill, size_t size);

// SigmaMemory.h lines 226-229 in the original.
inline void CopyMemory(void* dest, const void* src, size_t size) { Blizzard::String::MemCopy(dest, src, size); }
// line 227. The original calls Blizzard::String::MemCopy here too (SigmaMemoryMove tail-calls it).
inline void MoveMemory(void* dest, const void* src, size_t size) { Blizzard::String::MemCopy(dest, src, size); }
inline void ZeroMemory(void* ptr, size_t size) { Blizzard::String::MemFill(ptr, size, 0); }
inline void FillMemory(void* ptr, size_t size, int32 fill) { Blizzard::String::MemFill(ptr, size, fill); }
