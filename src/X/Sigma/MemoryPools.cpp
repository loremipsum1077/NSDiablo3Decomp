#include "MemoryPools.h"

#include "Sigma.h"

enum {
    POOL_DATAID = 0,
    POOL_SPHERENODE = 1,
    NUM_POOLS = 2,
};

struct MemoryPools {
    DataIDMemoryPool* ptDATAIDListPool;
    SphereNodeMemoryPool* ptSphereNodeListPool;
    GetMemoryPoolFunc pfnGetMemoryPool[NUM_POOLS];
};

// NOTE: the DWARF inline tree records only SetShrinkable() here, but both SetGrow() calls are
// required to produce the linear-grow words the original stores. The pfnGetMemoryPool loop is
// the last statement (DWARF: `i` is declared at line 58, after the SetShrinkable calls at
// lines 52/55); placing it before the pool setup changes the tail instruction scheduling.
void MemoryPoolsInitialize() {
    SigmaGlobals& tSigmaGlobals = gtSigmaGlobals;

    tSigmaGlobals.ptMemoryPools = new MemoryPools;

    tSigmaGlobals.ptMemoryPools->ptDATAIDListPool = new DataIDMemoryPool;
    tSigmaGlobals.ptMemoryPools->ptSphereNodeListPool = new SphereNodeMemoryPool;

    tSigmaGlobals.ptMemoryPools->ptDATAIDListPool->SetBlockCount_Tracked(4096);
    tSigmaGlobals.ptMemoryPools->ptSphereNodeListPool->SetBlockCount_Tracked(1024);

    tSigmaGlobals.ptMemoryPools->ptDATAIDListPool->SetShrinkable(TRUE);
    tSigmaGlobals.ptMemoryPools->ptDATAIDListPool->SetGrow(XALLOC_GROW_LINEAR, 4096);

    tSigmaGlobals.ptMemoryPools->ptSphereNodeListPool->SetShrinkable(TRUE);
    tSigmaGlobals.ptMemoryPools->ptSphereNodeListPool->SetGrow(XALLOC_GROW_LINEAR, 1024);

    for (int32 i = 0; i < NUM_POOLS; i++) {
        tSigmaGlobals.ptMemoryPools->pfnGetMemoryPool[i] = NULL;
    }
}

void MemoryPoolsDispose() {
    SigmaGlobals& tSigmaGlobals = gtSigmaGlobals;

    delete tSigmaGlobals.ptMemoryPools->ptSphereNodeListPool;
    delete tSigmaGlobals.ptMemoryPools->ptDATAIDListPool;
    delete tSigmaGlobals.ptMemoryPools;
}

DataIDMemoryPool* MemoryPoolsGetDATAIDListPool() {
    MemoryPools* ptMemoryPools = gtSigmaGlobals.ptMemoryPools;

    if (ptMemoryPools->pfnGetMemoryPool[POOL_DATAID] != NULL) {
        DataIDMemoryPool* ptPool = (DataIDMemoryPool*)ptMemoryPools->pfnGetMemoryPool[POOL_DATAID]();
        if (ptPool != NULL) {
            return ptPool;
        }
    }

    return ptMemoryPools->ptDATAIDListPool;
}

SphereNodeMemoryPool* MemoryPoolsGetSphereNodeListPool() {
    MemoryPools* ptMemoryPools = gtSigmaGlobals.ptMemoryPools;

    if (ptMemoryPools->pfnGetMemoryPool[POOL_SPHERENODE] != NULL) {
        SphereNodeMemoryPool* ptPool = (SphereNodeMemoryPool*)ptMemoryPools->pfnGetMemoryPool[POOL_SPHERENODE]();
        if (ptPool != NULL) {
            return ptPool;
        }
    }

    return ptMemoryPools->ptSphereNodeListPool;
}

DWordMemoryPool* MemoryPoolsGetDWORDListPool() {
    return (DWordMemoryPool*)MemoryPoolsGetDATAIDListPool();
}

void MemoryPoolsSetMemoryPoolUpCalls(GetMemoryPoolFunc pfnGetDATAIDMemoryPool,
                                     GetMemoryPoolFunc pfnGetSphereNodeMemoryPool) {
    MemoryPools* ptMemoryPools = gtSigmaGlobals.ptMemoryPools;
    ptMemoryPools->pfnGetMemoryPool[POOL_DATAID] = pfnGetDATAIDMemoryPool;
    ptMemoryPools->pfnGetMemoryPool[POOL_SPHERENODE] = pfnGetSphereNodeMemoryPool;
}
