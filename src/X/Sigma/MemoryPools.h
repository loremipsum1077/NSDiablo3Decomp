#pragma once

#include "DataStructures/XPooledList.h"
#include "DataStructures/XSphereNode.h"

typedef void* (*GetMemoryPoolFunc)();

typedef XListMemoryPool<unsigned int> DWordMemoryPool;

typedef XListMemoryPool<int> DataIDMemoryPool;

typedef XListMemoryPool<const XSphereNode*> SphereNodeMemoryPool;

void MemoryPoolsInitialize();
void MemoryPoolsDispose();
DataIDMemoryPool* MemoryPoolsGetDATAIDListPool();
SphereNodeMemoryPool* MemoryPoolsGetSphereNodeListPool();
DWordMemoryPool* MemoryPoolsGetDWORDListPool();
void MemoryPoolsSetMemoryPoolUpCalls(GetMemoryPoolFunc pfnGetDATAIDMemoryPool,
                                     GetMemoryPoolFunc pfnGetSphereNodeMemoryPool);
