// Adopted from the DWARF skeleton written by tools/gen_headers.py; hand-maintained from here on.
// Original file: X/Client/Prefetch.h
//
// NOTE: the DWARF skeleton also carried
//     typedef blz::vector<SNOName, blz::allocator<SNOName> > ChildSNOContainer;
//     typedef BOOL (*FilterFunc)(const CompressedSNOName &, const CompressedSNOName &,
//                                const CompressedSNOName &, BOOL &);
// with #include "../../Contrib/Contrib/blz/include/blz/vector.h".  Those two typedefs are not
// added here because their dependencies (the blz::allocator include path and the translation unit
// that owns CompressedSNOName) cannot be verified from this packet; nothing in
// X/Client/AnimationCommon.cpp uses them.  Only the declaration this file needs is added.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/SNO/SNOName.h"

namespace Prefetch {

enum PrefetchType {
    PT_BASE = 0,
    PT_COUNT = 1,
};

// X/Client/Prefetch.cpp: _ZN8Prefetch10AddToQueueERK7SNONamei.  Queues the SNO and reports
// whether it was added to the queue.  X/Client/AnimationCommon.cpp:150 calls it with priority 2.
BOOL AddToQueue( const SNOName & tSNOName, const int nPriority );

}  // namespace Prefetch

// Prefetch.cpp: the app subsystem entry points (X/Common/Game.cpp sgtAppSubsystems).
namespace Prefetch {
void InitForApp();
void DisposeForApp();
}  // namespace Prefetch

// Prefetch.h: the two typedefs X/Client/SNOPreload.cpp needs (see the note at the top).
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
namespace Prefetch {
typedef blz::vector<SNOName, blz::allocator<SNOName> > ChildSNOContainer;
typedef BOOL (*FilterFunc)(const CompressedSNOName &, const CompressedSNOName &, const CompressedSNOName &, BOOL &);
}  // namespace Prefetch

// Prefetch.cpp: the entry points X/Client/SNOManager.cpp calls.
namespace Prefetch {
void BeginAdd();
void EndAdd();
void BeginActiveWait();
void EndActiveWait();
void Update(BOOL bForce);
BOOL IsPending(const SNOName & tSNOName);
BOOL IsLoaded(const SNOName & tSNOName);
BOOL HasPriorityPending(BOOL bPriority);
BOOL GetChildSNOs(const SNOName & snoName, ChildSNOContainer & snoContainer, FilterFunc ptFilterFunc, BOOL fAddBase);
BOOL MultiGetChildSNOs(ChildSNOContainer & tSNOsToProcess, ChildSNOContainer & tChildren, FilterFunc ptFilterFunc);
}  // namespace Prefetch

// Prefetch.cpp: the rest of the namespace's entry points.
struct SNOElement;
namespace Prefetch {
BOOL ApplyHotfix(const char * szHotfixName);
void CancelAllActivePrefetches();
BOOL TakeSNOElement(const SNOName & snoname, SNOElement *& ptSNOElement);
void Cancel(const SNOName & snoname);
void WasCancelled(const SNOName & snoname);
void BlockUntilPriorityIdle(int32 ePriority);
BOOL IsLockingDefinition();
void EnableHighPriority(BOOL fEnabled);
int32 GetPriorityPendingCount(int32 ePriority);
}  // namespace Prefetch
