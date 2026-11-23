// Original file: X/Server/SMemoryPools.cpp

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/CommonTypes.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/SigmaTypes.h"
#include "SGame.h"
#include "SGameManager.h"
#include "SWorld.h"

void SMemoryPoolsInitializeForGame()
{
    SGame & tSGame = SGameManager::GameGet();

    tSGame.ptMemoryInterface = NULL;

    tSGame.ptDATAIDMemPool =
        new (SigmaMemoryNew(sizeof(XListMemoryPool<int>), 4, tSGame.ptMemoryInterface, FALSE)) XListMemoryPool<int>;
    tSGame.ptDATAIDMemPool->SetMemoryInterface(tSGame.ptMemoryInterface);

    tSGame.ptSphereNodeMemPool =
        new (SigmaMemoryNew(sizeof(XListMemoryPool<const XSphereNode *>), 4, tSGame.ptMemoryInterface, FALSE))
            XListMemoryPool<const XSphereNode *>;
    tSGame.ptSphereNodeMemPool->SetMemoryInterface(tSGame.ptMemoryInterface);

    tSGame.ptGBIDMemPool =
        new (SigmaMemoryNew(sizeof(XListMemoryPool<int>), 4, tSGame.ptMemoryInterface, FALSE)) XListMemoryPool<int>;
    tSGame.ptGBIDMemPool->SetMemoryInterface(tSGame.ptMemoryInterface);
    tSGame.ptGBIDMemPool->SetBlockCount_Tracked(1024);

    tSGame.ptSpawnGroupExactPool =
        new (SigmaMemoryNew(sizeof(XListMemoryPool<SpawnGroupExactInstance>), 4, tSGame.ptMemoryInterface, FALSE))
            XListMemoryPool<SpawnGroupExactInstance>;
    tSGame.ptSpawnGroupExactPool->SetMemoryInterface(tSGame.ptMemoryInterface);
    tSGame.ptSpawnGroupExactPool->SetBlockCount_Tracked(128);
}

void SMemoryPoolsDisposeForGame()
{
    SGame & tSGame = SGameManager::GameGet();

    tSGame.ptSpawnGroupExactPool->~XListMemoryPool<SpawnGroupExactInstance>();
    SigmaMemoryFree(tSGame.ptSpawnGroupExactPool, tSGame.ptMemoryInterface);
    tSGame.ptSpawnGroupExactPool = NULL;

    tSGame.ptGBIDMemPool->~XListMemoryPool<int>();
    SigmaMemoryFree(tSGame.ptGBIDMemPool, tSGame.ptMemoryInterface);
    tSGame.ptGBIDMemPool = NULL;

    tSGame.ptSphereNodeMemPool->~XListMemoryPool<const XSphereNode *>();
    SigmaMemoryFree(tSGame.ptSphereNodeMemPool, tSGame.ptMemoryInterface);
    tSGame.ptSphereNodeMemPool = NULL;

    tSGame.ptDATAIDMemPool->~XListMemoryPool<int>();
    SigmaMemoryFree(tSGame.ptDATAIDMemPool, tSGame.ptMemoryInterface);
    tSGame.ptDATAIDMemPool = NULL;
}
