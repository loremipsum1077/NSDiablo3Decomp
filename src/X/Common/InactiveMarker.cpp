// Original file: X/Common/InactiveMarker.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "InactiveMarker.h"
#include "Game.h"
#include "GameCommonData.h"
#include "SceneCommon.h"
#include "World.h"
#include "../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "Definitions/MarkerSetDefinition.h"
#include "Marker/Marker.h"
#include "Definitions/ActorDefinition.h"
#include "Definitions/QuestRangeDefinition.h"
#include "GizmoSwig.h"
#include "Nav/NavigationMesh.h"
#include "TagMaps/Tags.h"
#include "TagMaps/ActorTags.h"
#include "TagMaps/MarkerTags.h"
#include "../Server/System/Adventure.h"
#include "../Sigma/RefString.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"

// InactiveMarker.h:205 and the typedefs before it (not in the adopted header yet).
typedef XGrowableMap<unsigned int, unsigned int, int, int> XInactiveMarkerActivateStatusMap;

struct InactiveMarkerGlobals {  // size 0x230
    ChainedFixedBlockAllocator tInactiveMarkerListAlloc;  // 0x00
    ChainedFixedBlockAllocator tTInactiveMarkerListAlloc;  // 0x30
    ChainedFixedBlockAllocator tTInactiveItemMarkerListAlloc;  // 0x60
    TInactiveMarkerListPool tInactiveMarkerListPool;  // 0x90
    TInactiveItemMarkerListPool tInactiveItemMarkerListPool;  // 0xc0
    XChainedFixedBlockAllocator<InactiveItemMarker> tInactiveItemMarkerAlloc;  // 0xf0
    ChainedFixedBlockAllocator tInactiveMarkerBaseAlloc;  // 0x120
    ChainedFixedBlockAllocator tInactiveGizmoMarkerAlloc;  // 0x150
    ChainedFixedBlockAllocator tInactiveMonsterMarkerAlloc;  // 0x180
    XInactiveMarkerActivateStatusMap tInactiveMarkerActivateStatusMap;  // 0x1b0
};

void InactiveMarkerFree(InactiveBaseMarker * ptInactiveMarker);  // line 605
void InactiveMarkerFree(InactiveItemMarker * ptInactiveMarker);  // line 634
static void sFreeInactiveMarkerList(InactiveMarkerList * ptMarkerList);  // line 734
static InactiveMarkerList * sAllocInactiveMarkerList();  // line 718
TInactiveItemMarkerList * sAllocTInactiveItemMarkerList();  // line 758
TInactiveMarkerList * sAllocTInactiveMarkerList();  // line 798
static void sFreeTInactiveItemMarkerList(TInactiveItemMarkerList * ptMarkerList);  // line 775
static void sFreeTInactiveMarkerList(TInactiveMarkerList * ptMarkerList);  // line 815
static void sInactiveMarkerSetStatus(uint32 tName, BOOL bValue);  // line 865

void ActivateMarkerNames(const blz::vector<unsigned int> & aMarkerNames, blz::vector<ACDID> & aActivatedACDs, SNO snoMarkerSet, uint16 nAdventureParentIndex, uint32 nMarkerSetInstanceIndex);  // line 1966

void ActorAIFreeInactiveAI(ActorSavedAI & tActorSavedAI);  // X/Server/AI/ActorAI.h (ActorAI.cpp:4493)
PassabilityType NavMeshDeterminePassabilityType(SNO snoActor);  // Nav/NavigationMesh.h
static BOOL sActorIsSceneMarker(SNO snoActor);  // line 1223
static uint16 sGetInactiveMarkerFlagsFromMarkerSet(SNO snoMarkerSet, int32 nMarkerIndex);  // line 1305
static void sAddInactiveMarkerToList(InactiveMarkerList * ptInactiveMarkerList, InactiveBaseMarker * ptInactiveMarker);  // line 1141
static InactiveBaseMarker * sAllocBaseMarker();  // line 1175












// X/Server/Network/Server.cpp:438.
void ServerQueueClientUpdate();









































































































static const float MAX_MARKER_VIEW_DISTANCE_SQR = 14400.0f;




































void InactiveMarkerInitializeForGame()

{
    GameCommonData & tGCD = GameGetActiveGameCommonData();

    XMemoryAllocatorInterface * ptMemInterface = AppGetCurrentMemoryInterface();

    InactiveMarkerGlobals * ptInactiveMarkerGlobals = (InactiveMarkerGlobals *)SigmaMemoryNew(sizeof(InactiveMarkerGlobals), 4, ptMemInterface, TRUE);

    int32 nNumInactiveMarkerLists;
    int32 nNumTInactiveItemMarkerLists;
    int32 nNumTInactiveMarkerLists;
    int32 nNumPooledInactiveMarkers;
    int32 nNumPooledInactiveItemMarkers;
    int32 nNumInactiveItemMarkers;
    int32 nNumInactiveBaseMarkers;
    int32 nNumInactiveGizmoMarkers;
    int32 nNumInactiveMonsterMarkers;
    int32 nActivateStatusMapHashSize;

    BOOL bServer = GameActiveGameCommonDataIsServer();
    // Inference: optimized conditional assignments recover the server/client counts from the original selects.
    if (bServer)
    {
        nNumInactiveMarkerLists = 256;
        nNumTInactiveItemMarkerLists = 32;
        nNumTInactiveMarkerLists = 64;
        nNumPooledInactiveMarkers = 512;
        nNumPooledInactiveItemMarkers = 512;
        nNumInactiveItemMarkers = 512;
        nNumInactiveBaseMarkers = 64;
        nNumInactiveGizmoMarkers = 512;
        nNumInactiveMonsterMarkers = 512;
        nActivateStatusMapHashSize = 256;

    }
    else
    {
        nNumInactiveMarkerLists = 32;
        nNumTInactiveItemMarkerLists = 1;
        nNumTInactiveMarkerLists = 32;
        nNumPooledInactiveMarkers = 512;
        nNumPooledInactiveItemMarkers = 1;
        nNumInactiveItemMarkers = 1;
        nNumInactiveBaseMarkers = 2048;
        nNumInactiveGizmoMarkers = 1;
        nNumInactiveMonsterMarkers = 1;
        nActivateStatusMapHashSize = 1;
    }
    ptInactiveMarkerGlobals->tInactiveMarkerListAlloc.Initialize(sizeof(InactiveMarkerList), nNumInactiveMarkerLists, ptMemInterface);

    ptInactiveMarkerGlobals->tTInactiveItemMarkerListAlloc.Initialize(sizeof(TInactiveItemMarkerList), nNumTInactiveItemMarkerLists, ptMemInterface);
    ptInactiveMarkerGlobals->tTInactiveMarkerListAlloc.Initialize(sizeof(TInactiveMarkerList), nNumTInactiveMarkerLists, ptMemInterface);
    ptInactiveMarkerGlobals->tInactiveMarkerListPool.Initialize();
    // Inference: line272 stores the interface at globals+0x50 (list allocator), leaving pool+0xb0 null.
    ptInactiveMarkerGlobals->tTInactiveMarkerListAlloc.SetMemoryInterface(ptMemInterface);
    // Inference: the final pool block count overwrites Initialize's default count.
    ptInactiveMarkerGlobals->tInactiveMarkerListPool.SetBlockCount_Tracked(nNumPooledInactiveMarkers);
    ptInactiveMarkerGlobals->tInactiveItemMarkerListPool.Initialize();
    ptInactiveMarkerGlobals->tInactiveItemMarkerListPool.SetMemoryInterface(ptMemInterface);
    // Inference: the final item-pool block count overwrites Initialize's default count.
    ptInactiveMarkerGlobals->tInactiveItemMarkerListPool.SetBlockCount_Tracked(nNumPooledInactiveItemMarkers);
    ptInactiveMarkerGlobals->tInactiveItemMarkerAlloc.Initialize(nNumInactiveItemMarkers, ptMemInterface);
    ptInactiveMarkerGlobals->tInactiveMarkerBaseAlloc.Initialize(sizeof(InactiveBaseMarker), nNumInactiveBaseMarkers, ptMemInterface);
    ptInactiveMarkerGlobals->tInactiveGizmoMarkerAlloc.Initialize(sizeof(InactiveGizmoMarker), nNumInactiveGizmoMarkers, ptMemInterface);
    ptInactiveMarkerGlobals->tInactiveMonsterMarkerAlloc.Initialize(sizeof(InactiveMonsterMarker), nNumInactiveMonsterMarkers, ptMemInterface);


    ptInactiveMarkerGlobals->tInactiveItemMarkerAlloc.SetGrow(XALLOC_GROW_LINEAR, nNumInactiveItemMarkers);
    ptInactiveMarkerGlobals->tInactiveItemMarkerAlloc.SetShrinkable(TRUE);

    ptInactiveMarkerGlobals->tTInactiveItemMarkerListAlloc.SetGrow(XALLOC_GROW_LINEAR, nNumTInactiveItemMarkerLists);
    ptInactiveMarkerGlobals->tTInactiveItemMarkerListAlloc.SetShrinkable(TRUE);

    ptInactiveMarkerGlobals->tTInactiveMarkerListAlloc.SetGrow(XALLOC_GROW_LINEAR, nNumTInactiveMarkerLists);
    ptInactiveMarkerGlobals->tTInactiveMarkerListAlloc.SetShrinkable(TRUE);

    ptInactiveMarkerGlobals->tInactiveMarkerBaseAlloc.SetGrow(XALLOC_GROW_LINEAR, nNumInactiveBaseMarkers);
    ptInactiveMarkerGlobals->tInactiveMarkerBaseAlloc.SetShrinkable(TRUE);


    ptInactiveMarkerGlobals->tInactiveMarkerActivateStatusMap.Initialize(16, FALSE, ptMemInterface);
    ptInactiveMarkerGlobals->tInactiveMarkerActivateStatusMap.SetHashTableSize_Tracked(nActivateStatusMapHashSize);

    tGCD.ptInactiveMarkerGlobals = ptInactiveMarkerGlobals;
}


void InactiveMarkerDisposeForGame()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    InactiveMarkerGlobals * ptInactiveMarkerGlobals;
    XMemoryAllocatorInterface * ptMemInterface = AppGetCurrentMemoryInterface();

    ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;




    ptInactiveMarkerGlobals->tInactiveMarkerListAlloc.Dispose();
    ptInactiveMarkerGlobals->tTInactiveItemMarkerListAlloc.Dispose();
    ptInactiveMarkerGlobals->tTInactiveMarkerListAlloc.Dispose();
    ptInactiveMarkerGlobals->tInactiveMarkerListPool.Dispose();
    ptInactiveMarkerGlobals->tInactiveItemMarkerListPool.Dispose();

    ptInactiveMarkerGlobals->tInactiveItemMarkerAlloc.Dispose();
    ptInactiveMarkerGlobals->tInactiveMarkerBaseAlloc.Dispose();
    ptInactiveMarkerGlobals->tInactiveGizmoMarkerAlloc.Dispose();
    ptInactiveMarkerGlobals->tInactiveMonsterMarkerAlloc.Dispose();

    ptInactiveMarkerGlobals->tInactiveMarkerActivateStatusMap.Dispose();
    SigmaMemoryFree(ptInactiveMarkerGlobals, ptMemInterface);
    tGCD.ptInactiveMarkerGlobals = NULL;
}



void InactiveMarkerInitializeForScene(SSceneID idSScene)
{
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);


    tSCD.ptMarkerScene = sAllocInactiveMarkerList();

    tSCD.ptMarkersToBeActivated = sAllocInactiveMarkerList();
    tSCD.ptMarkersToBeActivated->ptInactiveMarkerList = sAllocTInactiveMarkerList();
    tSCD.ptMarkersToBeActivated->ptInactiveItemMarkerList = sAllocTInactiveItemMarkerList();
}


void InactiveMarkerDisposeForScene(SSceneID idSScene)
{
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);


    sFreeInactiveMarkerList(tSCD.ptMarkersToBeActivated);
    sFreeInactiveMarkerList(tSCD.ptMarkerScene);
    tSCD.ptMarkerScene = NULL;
    tSCD.ptMarkersToBeActivated = NULL;
}



void InactiveMarkerInitializeForWorld(SWorldID idSWorld)
{
    WorldCommonData & tWCD = WorldGetCommonData(idSWorld);


    tWCD.ptMarkerWorld = sAllocInactiveMarkerList();
    tWCD.ptMarkersToActivate = sAllocInactiveMarkerList();
    tWCD.ptMarkersToActivate->ptInactiveMarkerList = sAllocTInactiveMarkerList();
    tWCD.ptMarkersToActivate->ptInactiveItemMarkerList = sAllocTInactiveItemMarkerList();
}


void InactiveMarkerDisposeForWorld(SWorldID idSWorld)
{
    WorldCommonData & tWCD = WorldGetCommonData(idSWorld);


    sFreeInactiveMarkerList(tWCD.ptMarkersToActivate);
    sFreeInactiveMarkerList(tWCD.ptMarkerWorld);
    tWCD.ptMarkerWorld = NULL;
    tWCD.ptMarkersToActivate = NULL;
}






void InactiveMarkerInitializeForAct()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    InactiveMarkerGlobals * ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;

    ptInactiveMarkerGlobals->tInactiveMarkerActivateStatusMap.RemoveAll();
}

// Raw DWARF records MarkerCreate only at Marker/Marker.cpp:1825; no recovered header declaration.
DataID MarkerCreate(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform &transform, const InactiveMarkerOwner &tMarkerOwner);

ACDID InactiveMonsterActivate(SWorldID idSWorld, SSceneID idSScene, InactiveMonsterMarker *ptInactiveMarker);
ACDID InactiveGizmoActivate(SWorldID idSWorld, SSceneID idSScene, InactiveGizmoMarker *ptInactiveMarker);
static ACDID sInactiveMarkerActivate(
    SWorldID idSWorld,
    SSceneID idSScene,
    InactiveBaseMarker *ptInactiveMarker)
{
    switch (ptInactiveMarker->eInactiveMarkerType)
    {
    case INACTIVE_MARKERSET:
        return (ACDID)MarkerCreate(idSWorld, idSScene, ptInactiveMarker->transform, ptInactiveMarker->tMarkerOwner);


    case INACTIVE_MONSTER:
        return InactiveMonsterActivate(idSWorld, idSScene, (InactiveMonsterMarker *)ptInactiveMarker);
    case INACTIVE_GIZMO:
        return InactiveGizmoActivate(idSWorld, idSScene, (InactiveGizmoMarker *)ptInactiveMarker);
    }

    return ACDID_INVALID;
}





































































































































void InactiveMarkerCreateFromMarkerSet(InactiveMarkerList * ptInactiveMarkerList, const InactiveMarkerOwner & tOwner, const PRTransform & transform, const Sphere & wsBounds)
{
    InactiveBaseMarker * ptInactiveMarker = InactiveMarkerCreateForMarkerSet(tOwner, wsBounds, transform);

    sAddInactiveMarkerToList(ptInactiveMarkerList, ptInactiveMarker);
}






void InactiveMarkerCreateFromMarkerSet(SceneCommonData & tSCD, const InactiveMarkerOwner & tOwner, const PRTransform & transform, const Sphere & wsBounds)
{
    InactiveMarkerCreateFromMarkerSet(tSCD.ptMarkerScene, tOwner, transform, wsBounds);
}






void InactiveMarkerCreateFromWorldMarkerSet(SWorldID idSWorld, const InactiveMarkerOwner & tOwner, const PRTransform & transform, const Sphere & wsBounds)
{
    WorldCommonData & tWCD = WorldGetCommonData(idSWorld);
    InactiveMarkerCreateFromMarkerSet(tWCD.ptMarkerWorld, tOwner, transform, wsBounds);
}







InactiveBaseMarker * InactiveMarkerCreateForMarkerSet(const InactiveMarkerOwner & tMarkerOwner, const Sphere & wsBounds, const PRTransform & transform)
{
    InactiveBaseMarker * ptInactiveMarker = sAllocBaseMarker();

    ptInactiveMarker->eInactiveMarkerType = INACTIVE_MARKERSET;
    ptInactiveMarker->nFlags = sGetInactiveMarkerFlagsFromMarkerSet(tMarkerOwner.snoMarkerSet, tMarkerOwner.nMarkerIndex);
    ptInactiveMarker->tMarkerOwner = tMarkerOwner;
    ptInactiveMarker->wsBounds = wsBounds;
    ptInactiveMarker->transform = transform;

    return ptInactiveMarker;
}





void InactiveMarkerFree(InactiveBaseMarker * ptInactiveMarker)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    InactiveMarkerGlobals * ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;

    switch (ptInactiveMarker->eInactiveMarkerType)
    {
        case INACTIVE_MARKERSET:
            ptInactiveMarkerGlobals->tInactiveMarkerBaseAlloc.Free(ptInactiveMarker);
            break;

        case INACTIVE_MONSTER:
            ActorAIFreeInactiveAI(((InactiveMonsterMarker *)ptInactiveMarker)->tSavedAI);
            ptInactiveMarkerGlobals->tInactiveMonsterMarkerAlloc.Free(ptInactiveMarker);
            break;

        case INACTIVE_GIZMO:
            ptInactiveMarkerGlobals->tInactiveGizmoMarkerAlloc.Free(ptInactiveMarker);
            break;

        default:

            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}



void InactiveMarkerFree(InactiveItemMarker * ptInactiveMarker)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    InactiveMarkerGlobals * ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;

    ptInactiveMarkerGlobals->tInactiveItemMarkerAlloc.Free(ptInactiveMarker);
}























void InactiveMarkerSetCanActivate(uint32 tMarkerName, BOOL bValue)
{


    sInactiveMarkerSetStatus(tMarkerName, bValue);
}


void InactiveMarkerGetMarkerName(InactiveBaseMarker * ptInactiveMarker, uint32 & tMarkerName)
{
    InactiveMarkerOwner & tMarkerOwner = ptInactiveMarker->tMarkerOwner;

    tMarkerName = 0;

    if (tMarkerOwner.snoMarkerSet != SNO_NONE) {



        ScopedMarkerSetDefinitionLockR ptMarkerSet(tMarkerOwner.snoMarkerSet);

        if (tMarkerOwner.nMarkerIndex < ptMarkerSet->tMarkerSet.GetArraySize()) {

            tMarkerName = ptMarkerSet->tMarkerSet[tMarkerOwner.nMarkerIndex].dwHash;
        }
    }
}



void InactiveMarkerGetFullName(InactiveBaseMarker * ptInactiveMarker, uint32 & tMarkerName)
{
    InactiveMarkerOwner & tMarkerOwner = ptInactiveMarker->tMarkerOwner;


    InactiveMarkerGetMarkerName(ptInactiveMarker, tMarkerName);

    if (tMarkerOwner.nAdventureParentIndex != 0xffff) {

        tMarkerName = MarkerStringHash(CRefString().Format("%s:%s", AdventureGetParentName(tMarkerOwner.nAdventureParentIndex).str(), MarkerNameGetString(tMarkerName)));
    }
}












static InactiveMarkerList * sAllocInactiveMarkerList()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    InactiveMarkerGlobals * ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;
    InactiveMarkerList * ptMarkerList = (InactiveMarkerList *)ptInactiveMarkerGlobals->tInactiveMarkerListAlloc.Alloc_Tracked();
    ptMarkerList->ptInactiveItemMarkerList = NULL;
    ptMarkerList->ptInactiveMarkerList = NULL;
    return ptMarkerList;
}






static void sFreeInactiveMarkerList(InactiveMarkerList * ptMarkerList)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    InactiveMarkerGlobals * ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;
    if (ptMarkerList->ptInactiveItemMarkerList) {






        sFreeTInactiveItemMarkerList(ptMarkerList->ptInactiveItemMarkerList);
    }

    if (ptMarkerList->ptInactiveMarkerList) {

        sFreeTInactiveMarkerList(ptMarkerList->ptInactiveMarkerList);
    }

    ptInactiveMarkerGlobals->tInactiveMarkerListAlloc.Free(ptMarkerList);
}


TInactiveItemMarkerList * sAllocTInactiveItemMarkerList()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    InactiveMarkerGlobals * ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;
    TInactiveItemMarkerList * ptMarkerList = (TInactiveItemMarkerList *)ptInactiveMarkerGlobals->tTInactiveItemMarkerListAlloc.Alloc_Tracked();
    SigmaMemoryClear(ptMarkerList, sizeof(TInactiveItemMarkerList));
    ptMarkerList->Initialize(&ptInactiveMarkerGlobals->tInactiveItemMarkerListPool);
    return ptMarkerList;
}







static void sFreeTInactiveItemMarkerList(TInactiveItemMarkerList * ptMarkerList)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    InactiveMarkerGlobals * ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;
    {
        POS pos = ptMarkerList->GetHeadPosition();
        while (pos) {

            InactiveItemMarker * ptInactiveMarker = ptMarkerList->GetNext(pos);
            InactiveMarkerFree(ptInactiveMarker);
        }
    }

    ptMarkerList->Dispose();

    ptInactiveMarkerGlobals->tTInactiveItemMarkerListAlloc.Free(ptMarkerList);
}





TInactiveMarkerList * sAllocTInactiveMarkerList()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    InactiveMarkerGlobals * ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;
    TInactiveMarkerList * ptMarkerList = (TInactiveMarkerList *)ptInactiveMarkerGlobals->tTInactiveMarkerListAlloc.Alloc_Tracked();
    SigmaMemoryClear(ptMarkerList, sizeof(TInactiveMarkerList));
    ptMarkerList->Initialize(&ptInactiveMarkerGlobals->tInactiveMarkerListPool);
    return ptMarkerList;
}







static void sFreeTInactiveMarkerList(TInactiveMarkerList * ptMarkerList)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    InactiveMarkerGlobals * ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;
    {
        POS pos = ptMarkerList->GetHeadPosition();
        while (pos) {

            InactiveBaseMarker * ptInactiveMarker = ptMarkerList->GetNext(pos);
            InactiveMarkerFree(ptInactiveMarker);
        }
    }

    ptMarkerList->Dispose();

    ptInactiveMarkerGlobals->tTInactiveMarkerListAlloc.Free(ptMarkerList);
}
































static void sInactiveMarkerSetStatus(uint32 tName, BOOL bValue)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    uint32 dwNameHash = tName;
    InactiveMarkerGlobals * ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;



    ptInactiveMarkerGlobals->tInactiveMarkerActivateStatusMap.SetAt_Tracked(dwNameHash, bValue);

    if (bValue) {
        ServerQueueClientUpdate();
    }
}












ACDPackID InactiveMarkerGetPack(
    InactiveBaseMarker *ptInactiveMarker)
{
    ACDPackID idACDPack = ACDPACKID_INVALID;

    if (ptInactiveMarker->eInactiveMarkerType == INACTIVE_MONSTER)
    {
        InactiveMonsterMarker *ptInactiveMonster = (InactiveMonsterMarker *)ptInactiveMarker;

        idACDPack = ptInactiveMonster->idACDPack;
    }

    return idACDPack;
}











































































































































































































































static void sAddInactiveMarkerToList(InactiveMarkerList * ptInactiveMarkerList, InactiveBaseMarker * ptInactiveMarker)
{

    if (!ptInactiveMarkerList->ptInactiveMarkerList)
    {
        ptInactiveMarkerList->ptInactiveMarkerList = sAllocTInactiveMarkerList();
    }

    ptInactiveMarkerList->ptInactiveMarkerList->AddTail_Tracked(ptInactiveMarker);
}




static void sAddInactiveMarkerToList(
    InactiveMarkerList *ptInactiveMarkerList,
    InactiveItemMarker *ptInactiveMarker)
{
    if (!ptInactiveMarkerList->ptInactiveItemMarkerList)
    {
        ptInactiveMarkerList->ptInactiveItemMarkerList = sAllocTInactiveItemMarkerList();
    }

    ptInactiveMarkerList->ptInactiveItemMarkerList->AddTail_Tracked(ptInactiveMarker);
}









static InactiveBaseMarker * sAllocBaseMarker()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    InactiveMarkerGlobals * ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;
    return (InactiveBaseMarker *)ptInactiveMarkerGlobals->tInactiveMarkerBaseAlloc.Alloc_Tracked();
}





static InactiveGizmoMarker *sAllocGizmoMarker()
{

    GameCommonData &tGCD = GameGetActiveGameCommonData();

    InactiveMarkerGlobals *ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;

    return (InactiveGizmoMarker *)ptInactiveMarkerGlobals->tInactiveGizmoMarkerAlloc.Alloc_Tracked();
}



static InactiveMonsterMarker *sAllocMonsterMarker()
{

    GameCommonData &tGCD = GameGetActiveGameCommonData();

    InactiveMarkerGlobals *ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;

    return (InactiveMonsterMarker *)ptInactiveMarkerGlobals->tInactiveMonsterMarkerAlloc.Alloc_Tracked();
}



static InactiveItemMarker *sAllocItemMarker()
{

    GameCommonData &tGCD = GameGetActiveGameCommonData();

    InactiveMarkerGlobals *ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;

    return ptInactiveMarkerGlobals->tInactiveItemMarkerAlloc.Alloc_Tracked();
}



static BOOL sActorIsSceneMarker(
    SNO snoActor)
{
    if (GameIsStartUp())
        return TRUE;




    ScopedActorDefinitionLockR pdefActor(snoActor);
    GizmoType nGizmoType = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TYPE);
    BOOL bKeepActiveWithScene = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_ACTIVATE_WITH_SCENE);
    PassabilityType ePassabilityType = NavMeshDeterminePassabilityType(snoActor);
    ActorType eType = pdefActor->eType;

    if (bKeepActiveWithScene)
        return TRUE;


    // DWARF 1243:50: the server-prop test is the second operand of this condition.
    if (ePassabilityType == PASSABILITY_NONMOVER || eType == ACTORTYPE_SERVER_PROP)
        return TRUE;




    switch (nGizmoType)
    {
        case ACTORGIZMO_PORTAL:
        case ACTORGIZMO_WAYPOINT:
        case ACTORGIZMO_CHECKPOINT:
        case ACTORGIZMO_TOWN_PORTAL:
        case ACTORGIZMO_HEARTH_PORTAL:
        case ACTORGIZMO_PORTAL_DESTINATION:
        case ACTORGIZMO_PAGE_OF_FATE_PORTAL:
        case ACTORGIZMO_BOSS_PORTAL:
        case ACTORGIZMO_SAVEPOINT:
        case ACTORGIZMO_SAVEPOINT_PORTAL:
        case ACTORGIZMO_DUNGEON_PORTAL:
        case ACTORGIZMO_RETURN_PORTAL:
            return TRUE;
        default:
            return FALSE;
    }
}


static void sSetFlagsForQuestRange(uint16 & nFlags, const Marker & tMarker)
{

    // flag markers whose quest range is not active yet
    SNO snoQuestRange = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_QUESTRANGE);
    QuestRangeResult result;
    if (!QuestRangeEvaluate(snoQuestRange, &result))
    {
        nFlags |= (1 << INACTIVE_MARKER_FLAG_WAITING_ON_QUEST); if (AppDrawFlagIsSet(APP_DRAW_DEBUG_QUESTS_BIT)) {}
    }









    else if (snoQuestRange != SNO_NONE)
    {
        if (AppDrawFlagIsSet(APP_DRAW_DEBUG_QUESTS_BIT)) {}
    }
}











static uint16 sGetInactiveMarkerFlagsFromMarkerSet(SNO snoMarkerSet, int32 nMarkerIndex)
{

    uint16 nFlags = 0;
    ScopedMarkerSetDefinitionLockR ptMarkerSet(snoMarkerSet);

    if (nMarkerIndex >= 0 && nMarkerIndex < ptMarkerSet->tMarkerSet.GetArraySize())
    {
        const Marker & tMarker = ptMarkerSet->tMarkerSet[nMarkerIndex];


        if (TagMapGetInt(tMarker.hTagMap, TAG_MARKER_START_INACTIVE))
            nFlags |= (1 << INACTIVE_MARKER_FLAG_CHECK_ACTIVATE_STATUS);


        if (tMarker.eType == MARKERTYPE_ACTOR)
        {
            if (sActorIsSceneMarker(tMarker.snoname.snoHandle))

                nFlags |= (1 << INACTIVE_MARKER_FLAG_IS_SCENE_MARKER);

            sSetFlagsForQuestRange(nFlags, tMarker);
        }
        else if (tMarker.eType == MARKERTYPE_AMBIENTSOUND)
        {
            sSetFlagsForQuestRange(nFlags, tMarker);
        }
    }

    return nFlags;
}












































































































































































































































































































































































































































































































































































































































ACDID ActivateMarkerName(LPCSTR szMarkerName)
{

    blz::vector<unsigned int> aMarkerNames;
    aMarkerNames.push_back(MarkerStringHash(szMarkerName));
    blz::vector<ACDID> aActivatedACDs;
    ActivateMarkerNames(aMarkerNames, aActivatedACDs, SNO_NONE, 0xffff, 0);
    return aActivatedACDs[0];
}

#include "../Server/Actors/Actors.h"
#include "../Server/AI/ActorAI.h"
#include "../Server/Powers/PowerAPI.h"
#include "Definitions/GlobalDefinition.h"
#include "GlobalSNO.h"
// Raw DWARF records only LuaScript.cpp:5241 and AnimPlayer.cpp:1323; no recovered header declarations.
void LuaScriptSaveForDeactivatingActor(ActorID idActor, SavedDataKey *ptSavedDataKey);
uint32 AnimPlayerGetIdleAnimation(ActorCommonData &tACD);

#line 2609
static uint16 sGetInactiveMarkerFlagsFromActor(
    ActorID idActor)
{
    uint16 nFlags = 0;
    Actor *ptActor = ActorGet(idActor);

    if (ptActor->tMarkerOwner.snoMarkerSet != SNO_NONE)
    {
        nFlags = sGetInactiveMarkerFlagsFromMarkerSet(ptActor->tMarkerOwner.snoMarkerSet, ptActor->tMarkerOwner.nMarkerIndex);
    }

    if (sActorIsSceneMarker(ptActor->snoActor))
    {
        nFlags |= 1 << INACTIVE_MARKER_FLAG_IS_SCENE_MARKER;
    }

    return nFlags;
}

#line 2634
static void sSetBaseInactiveMarkerForActor(
    ActorID idActor,
    InactiveBaseMarker *ptInactiveMarker)
{
    Actor *ptActor = ActorGet(idActor);
    ActorCommonData &tACD = ACDGet(ptActor->idACD);
    const ActorLocation &tLocation = tACD.GetLocation();

    ptInactiveMarker->nFlags = sGetInactiveMarkerFlagsFromActor(idActor);
    ptInactiveMarker->tMarkerOwner = ptActor->tMarkerOwner;
    ptInactiveMarker->wsBounds = tLocation.tWorldLocation.wsBounds;
    ptInactiveMarker->transform = tLocation.tWorldLocation.transform;


    ptInactiveMarker->nPrimaryGroupIndex = ActorGroupHashToIndex(tACD.uGroupPrimary);
    ptInactiveMarker->nSecondaryGroupIndex = ActorGroupHashToIndex(tACD.uGroupSecondary);
    ptInactiveMarker->nScriptState = tACD.nScriptState;

    if (!tACD.IsEnabled()) ptInactiveMarker->nFlags |= (1 << INACTIVE_MARKER_FLAG_DISABLED); else ptInactiveMarker->nFlags &= ~(1 << INACTIVE_MARKER_FLAG_DISABLED);
    if (tACD.AttributesGetInt(ATTRIBUTE_QUEST_MONSTER)) ptInactiveMarker->nFlags |= (1 << INACTIVE_MARKER_FLAG_QUEST_MONSTER); else ptInactiveMarker->nFlags &= ~(1 << INACTIVE_MARKER_FLAG_QUEST_MONSTER);
    if (tACD.AttributesGetInt(ATTRIBUTE_NPC_DISABLE_OPERATION)) ptInactiveMarker->nFlags |= (1 << INACTIVE_MARKER_FLAG_DISABLE_OPERATION); else ptInactiveMarker->nFlags &= ~(1 << INACTIVE_MARKER_FLAG_DISABLE_OPERATION);
    if (tACD.AttributesGetInt(ATTRIBUTE_MINIMAP_ICON_ACTIVE)) ptInactiveMarker->nFlags |= (1 << INACTIVE_MARKER_FLAG_MINIMAP_ENABLED); else ptInactiveMarker->nFlags &= ~(1 << INACTIVE_MARKER_FLAG_MINIMAP_ENABLED);
    if (tACD.AttributesGetInt(ATTRIBUTE_BOUNTY_OBJECTIVE)) ptInactiveMarker->nFlags |= (1 << INACTIVE_MARKER_FLAG_BOUNTY_OBJECTIVE); else ptInactiveMarker->nFlags &= ~(1 << INACTIVE_MARKER_FLAG_BOUNTY_OBJECTIVE);
}

#line 2331
InactiveGizmoMarker *InactiveGizmoMarkerCreate(
    ActorID idActor)
{
    InactiveGizmoMarker *ptInactiveMarker = sAllocGizmoMarker();

    ptInactiveMarker->eInactiveMarkerType = INACTIVE_GIZMO;
    sSetBaseInactiveMarkerForActor(idActor, ptInactiveMarker);
    LuaScriptSaveForDeactivatingActor(idActor, &ptInactiveMarker->tLuaSavedDataKey);

    Actor *ptActor = ActorGet(idActor);
    ActorCommonData &tACD = ACDGet(ptActor->idACD);
    const ActorLocation &tLocation = tACD.GetLocation();
    GizmoType eGizmoType = tACD.GetGizmoType();

    ptInactiveMarker->snoActor = tACD.snoActor;
    ptInactiveMarker->nGizmoState = tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE);
    ptInactiveMarker->nGizmoCharges = tACD.AttributesGetInt(ATTRIBUTE_GIZMO_CHARGES);
    ptInactiveMarker->nGizmoDisabledByScript = tACD.AttributesGetInt(ATTRIBUTE_GIZMO_DISABLED_BY_SCRIPT);
    ptInactiveMarker->dwCooldown = tACD.AttributesGetInt(ATTRIBUTE_GENERAL_COOLDOWN);
    ptInactiveMarker->nGizmoChestOpen = tACD.AttributesGetInt(ATTRIBUTE_CHEST_OPEN);
    ptInactiveMarker->idPing = tACD.AttributesGetInt(ATTRIBUTE_PING);
    if (eGizmoType == ACTORGIZMO_PAGE_OF_FATE_PORTAL)
    {
        ptInactiveMarker->uSpawnerCountID = tACD.AttributesGetInt(ATTRIBUTE_PAGE_OF_FATE_ITEM);
    }

    else
    {
        ptInactiveMarker->uSpawnerCountID = tACD.AttributesGetInt(ATTRIBUTE_SPAWNER_CONCURRENT_COUNT_ID);
    }
    ptInactiveMarker->uDynamicEntranceGUID = tACD.AttributesGetInt(ATTRIBUTE_DYNAMIC_ENTRANCE_GUID);
    ptInactiveMarker->nLevel = tACD.AttributesGetInt(ATTRIBUTE_LEVEL);
    ptInactiveMarker->nTeam = tACD.GetTeam();
    ptInactiveMarker->flScale = tLocation.tWorldLocation.flScale;
    ptInactiveMarker->nSourceSceneUID = ptActor->nSourceSceneUID;

    if (eGizmoType == ACTORGIZMO_MULTICLICK)
    {
        ptInactiveMarker->dwAnimIdleTag = AnimPlayerGetIdleAnimation(tACD);
    }

    ptInactiveMarker->dwFlags = 0;


    if (PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_INVULNERABLE_BUFF), &tACD, ACDID_INVALID) != BUFFID_INVALID)
    {
        ptInactiveMarker->dwFlags |= 1 << GSA_FLAG_INVULNERABLE;
    }

    if (PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_TAKES_NO_DAMAGE_BUFF), &tACD, ACDID_INVALID) != BUFFID_INVALID)
    {
        ptInactiveMarker->dwFlags |= 1 << GSA_FLAG_NO_DAMAGE;
    }

    if (PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_CANNOT_BE_ADDED_TO_AI_TARGET_LIST_BUFF), &tACD, ACDID_INVALID) != BUFFID_INVALID)
    {
        ptInactiveMarker->dwFlags |= 1 << GSA_FLAG_CANNOT_BE_ADDED_TO_AI_TARGET_LIST;
    }

    if (PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_DOES_FAKE_DAMAGE_BUFF), &tACD, ACDID_INVALID) != BUFFID_INVALID)
    {
        ptInactiveMarker->dwFlags |= 1 << GSA_FLAG_DOES_FAKE_DAMAGE;
    }

    if (tACD.AttributesGetInt(ATTRIBUTE_GRANTS_NO_XP))
    {
        ptInactiveMarker->dwFlags |= 1 << GSA_FLAG_GRANTS_NO_XP;
    }

    if (tACD.AttributesGetInt(ATTRIBUTE_DROPS_NO_LOOT))
    {
        ptInactiveMarker->dwFlags |= 1 << GSA_FLAG_DROPS_NO_LOOT;
    }

    ActorAI *ptAI = ActorTryToGetAI(idActor);
    if (ptAI)
    {
        if (ActorAIIsStatusFlagSet(idActor, ACTOR_AI_DISABLE_UPDATES_BIT)) ptInactiveMarker->dwFlags |= (1 << GSA_FLAG_DISABLE_BRAIN); else ptInactiveMarker->dwFlags &= ~(1 << GSA_FLAG_DISABLE_BRAIN);
    }

    return ptInactiveMarker;
}

#include "Player.h"
// Raw DWARF records only ActorAI.cpp:4165, ActorInventory.cpp:12887 and ActorCommonData.cpp:9307; no recovered header declarations.
void ActorAISaveForDeactivatingActor(ActorID idActor, ActorSavedAI &tActorSavedAI);
uint32 ActorInventoryGetSeed(const ActorInventory *pInv);
int32 ACDGetTieredLootRunLevel(const ActorCommonData &tACD);

#line 1347
static void sActorGetOccupiedScenes(
    ACDID idACD,
    SSceneID *parrayidSScenes,
    int32 &nCountOccupiedScenes)
{
    nCountOccupiedScenes = 0;
    ActorCommonData *ptACD = ACDTryToGet(idACD);

    if (ptACD)
    {
        const ActorLocation &tLocation = ptACD->GetLocation();
        if (tLocation.eActorLocation == AL_WORLD)
        {
            parrayidSScenes[nCountOccupiedScenes++] = tLocation.tWorldLocation.idSScene;
        }
    }
}

#line 2661
static void sSaveActorAttributes(
    ActorID idActor,
    MonsterSavedAttributes &tAttributes)
{
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData &tACD = ACDGet(idACD);

    tAttributes.uDeactivationTick = GameGetServerTime();
    tAttributes.dwSavedFlagAttributes = 0;
    tAttributes.flHitPoints = tACD.GetHitPointsCurrent();
    tAttributes.nPlayersWhenDeactivated = PlayerGetNumPlayersForDifficulty();
    tAttributes.nDifficultyWhenDeactivated = GameGetHandicapLevel();
    tAttributes.nLevel = tACD.AttributesGetInt(ATTRIBUTE_LEVEL);
    tAttributes.nDamageState = tACD.AttributesGetInt(ATTRIBUTE_DAMAGE_STATE_CURRENT);
    tAttributes.nTeam = tACD.GetTeam();
    tAttributes.dwAnimIdleTag = AnimPlayerGetIdleAnimation(tACD);
    tAttributes.dwInventorySeed = ActorInventoryGetSeed(tACD.ptInventory);


    if (PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_INVULNERABLE_BUFF), &tACD, ACDID_INVALID) != BUFFID_INVALID)
    {
        tAttributes.dwSavedFlagAttributes |= 1 << MSA_FLAG_INVULNERABLE;
    }

    if (PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_TAKES_NO_DAMAGE_BUFF), &tACD, ACDID_INVALID) != BUFFID_INVALID)
    {
        tAttributes.dwSavedFlagAttributes |= 1 << MSA_FLAG_NO_DAMAGE;
    }

    if (PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_CANNOT_BE_ADDED_TO_AI_TARGET_LIST_BUFF), &tACD, ACDID_INVALID) != BUFFID_INVALID)
    {
        tAttributes.dwSavedFlagAttributes |= 1 << MSA_FLAG_CANNOT_BE_ADDED_TO_AI_TARGET_LIST;
    }

    if (PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_DOES_FAKE_DAMAGE_BUFF), &tACD, ACDID_INVALID) != BUFFID_INVALID)
    {
        tAttributes.dwSavedFlagAttributes |= 1 << MSA_FLAG_DOES_FAKE_DAMAGE;
    }

    if (tACD.AttributesGetInt(ATTRIBUTE_GRANTS_NO_XP))
    {
        tAttributes.dwSavedFlagAttributes |= 1 << MSA_FLAG_GRANTS_NO_XP;
    }

    if (tACD.AttributesGetInt(ATTRIBUTE_RESURRECTED))
    {
        tAttributes.dwSavedFlagAttributes |= 1 << MSA_FLAG_RESURRECTED;
    }

    if (tACD.AttributesGetInt(ATTRIBUTE_DROPS_NO_LOOT))
    {
        tAttributes.dwSavedFlagAttributes |= 1 << MSA_FLAG_DROPS_NO_LOOT;
    }

    if (tACD.AttributesGetInt(ATTRIBUTE_HAS_DROPPED_SPECIAL_LOOT))
    {
        tAttributes.dwSavedFlagAttributes |= 1 << MSA_FLAG_HAS_DROPPED_SPECIAL_LOOT;
    }

    if (ActorTryToGetAI(idActor))
    {
        if (ActorAIIsStatusFlagSet(idActor, ACTOR_AI_DISABLE_UPDATES_BIT)) tAttributes.dwSavedFlagAttributes |= (1 << MSA_FLAG_DISABLE_BRAIN); else tAttributes.dwSavedFlagAttributes &= ~(1 << MSA_FLAG_DISABLE_BRAIN);
    }

    if (tACD.dwFlags & (1 << ACD_IS_FROM_DISTRIBUTION_BIT))
    {
        tAttributes.dwSavedFlagAttributes |= 1 << MSA_FLAG_IS_FROM_DISTRIBUTION;
    }

    if (ACDGetTieredLootRunLevel(tACD) >= 0)
    {
        tAttributes.dwSavedFlagAttributes |= 1 << MSA_FLAG_IN_TIERED_RIFT;
    }
}

#line 2415
InactiveMonsterMarker *InactiveMonsterMarkerCreate(
    ActorID idActor)
{
    InactiveMonsterMarker *ptInactiveMarker;

    Actor *ptActor = ActorGet(idActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData &tACD = ACDGet(idACD);

    ptInactiveMarker = sAllocMonsterMarker();
    ptInactiveMarker->eInactiveMarkerType = INACTIVE_MONSTER;
    sSetBaseInactiveMarkerForActor(idActor, ptInactiveMarker);
    sSaveActorAttributes(idActor, ptInactiveMarker->tSavedAttributes);

    PowerAPI_BuffFlagAllForDeletion(tACD);

    ActorAISaveForDeactivatingActor(idActor, ptInactiveMarker->tSavedAI);







    LuaScriptSaveForDeactivatingActor(idActor, &ptInactiveMarker->tLuaSavedDataKey);

    WorldPlace placeCurr = tACD.GetPlace();
    ScenesCommonNavMeshNodeFromPlace(placeCurr, &ptInactiveMarker->tNavMeshHandleCenter, FALSE, FALSE);

    int32 nSceneCount;
    sActorGetOccupiedScenes(idACD, ptInactiveMarker->arOccupiedScenes, nSceneCount);
    ptInactiveMarker->nOccupiedSceneCount = nSceneCount;

    ptInactiveMarker->idACDPack = tACD.idACDPack;
    ACDPack *ptACDPack = ACDPackTryToGet(tACD.idACDPack);
    if (ptACDPack)
    {
        ptACDPack->AddDeactivated();
    }

    ptInactiveMarker->eMonsterRarity = tACD.eMonsterRarity;
    ptInactiveMarker->nLookLinkIndex = tACD.nLookLinkIndex;
    ptInactiveMarker->flScale = tACD.tLocation.tWorldLocation.flScale;
    ptInactiveMarker->snoMonster = tACD.tSNOName.snoHandle;
    ptInactiveMarker->snoConvList = ptActor->snoConvList;

    SigmaMemoryCopy(ptInactiveMarker->arRareNameGBIDs, tACD.pRareNameGBIDs, sizeof(ptInactiveMarker->arRareNameGBIDs));
    SigmaMemoryCopy(ptInactiveMarker->arMonsterAffixGBIDs, tACD.pMonsterAffixGBIDs, sizeof(ptInactiveMarker->arMonsterAffixGBIDs));

    ptInactiveMarker->tRNGOriginal = ptActor->rndOriginal;
    ptInactiveMarker->nSourceSceneUID = ptActor->nSourceSceneUID;

    return ptInactiveMarker;
}

#include "Items.h"
#include "ActorInventory.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"
// Raw DWARF records only Items.cpp:5681 and ActorInventory.cpp:9771; no recovered header declarations.
ACDID ItemGetLockedToACD(const ActorCommonData &tACD);
void ACDInventoryGetSlotTypeInfo(InventoryType eType, int32 &nStart, int32 &nEnd, int32 &nSlots);

#line 2471
InactiveItemMarker *InactiveItemMarkerCreate(
    ActorID idActor)
{
    InactiveItemMarker *ptInactiveMarker = sAllocItemMarker();
    Actor *ptActor = ActorGet(idActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData &tACD = ACDGet(idACD);
    const ActorLocation &tLocation = tACD.GetLocation();

    ACDID idACDBoundTo = ItemGetBoundToACD(tACD, FALSE);
    if (idACDBoundTo != ACDID_INVALID)
    {
        ptInactiveMarker->bSoulbound = TRUE;
        ptInactiveMarker->idACDBoundTo = idACDBoundTo;
    }
    else
    {
        ptInactiveMarker->bSoulbound = FALSE;
        ptInactiveMarker->idACDBoundTo = ItemGetLockedToACD(tACD);

        uint64 uUnlockTime = tACD.GetUnlockTime();
        if (uUnlockTime)
        {
            ptInactiveMarker->uUnlockTime = uUnlockTime / 1000000ULL;
        }
    }

    if (ItemHasLabel(tACD.hGB.gbid, ITL_GLYPH))
    {
        ptInactiveMarker->dwCreateTime = tACD.dwHitGroundTime;
    }
    else
    {
        ptInactiveMarker->dwCreateTime = GameGetActiveGameCommonData().dwServerTime;
    }


    ptInactiveMarker->tItemId = ptActor->tItemId;
    ptInactiveMarker->wsBounds = tLocation.tWorldLocation.wsBounds;
    ptInactiveMarker->wp = tLocation.tWorldLocation.transform.wp;
    ptInactiveMarker->nSocketedItemCount = 0;
    ptInactiveMarker->nGoldAmount = -1;
    ptInactiveMarker->idAssignedHero = tACD.GetAssignedHeroID();
    for (int32 i = 0; i < 4; ++i)
    {
        ptInactiveMarker->idTradablePlayers[i] = tACD.GetItemTradePlayerID(i);
    }
    ptInactiveMarker->uTradeEndTime = tACD.AttributesGetInt(ATTRIBUTE_ITEM_TRADE_END_TIME);
    ptInactiveMarker->idSourcePlayer = tACD.GetItemSourcePlayerID();

    ptInactiveMarker->idAssignedGameConnectionID = tACD.GetAssignedGameConnectionID();

    if (tACD.ptFriendGiftData)
    {
        ptInactiveMarker->idPlayerGiftRecipient = tACD.ptFriendGiftData->idRecipient;
        Blizzard::String::Copy(ptInactiveMarker->szPlayerName, tACD.ptFriendGiftData->sNameRecipient.str(), sizeof(ptInactiveMarker->szPlayerName));
    }
    else
    {
        ptInactiveMarker->idPlayerGiftRecipient = 0;
        ptInactiveMarker->szPlayerName[0] = '\0';
    }

    ptInactiveMarker->nPlayerOwner = tACD.AttributesGetInt(ATTRIBUTE_ITEM_PLAYER_RECIPIENT);

    if (ptActor->dwActorFlags & (1 << ACTOR_DB_PERSISTED))
        ptInactiveMarker->bPersisted = TRUE;
    else
        ptInactiveMarker->bPersisted = FALSE;

    if (ptActor->dwActorFlags & (1 << ACTOR_CHEATED))
        ptInactiveMarker->bCheated = TRUE;
    else
        ptInactiveMarker->bCheated = FALSE;

    ptInactiveMarker->dwSeasonCreated = ptActor->dwSeasonCreated;

    ItemGeneratorPopulateFromACDID(tACD, ptInactiveMarker->tItemGenerator, TRUE);

    if (ItemIsGold(tACD))
    {


        ptInactiveMarker->nGoldAmount = tACD.AttributesGetInt(ATTRIBUTE_GOLD);
    }

    ptInactiveMarker->idPing = tACD.AttributesGetInt(ATTRIBUTE_PING);


    if (ItemHasSockets(tACD))
    {

        int32 nEnd = 0;
        int32 nStart = 0;
        int32 nSlots = 0;
        int32 nSocketedItemCount = 0;

        ACDInventoryGetSlotTypeInfo(ACDInventoryGetType(tACD), nStart, nEnd, nSlots);

        for (int32 nSlot = nStart; nSlot < nEnd; ++nSlot)
        {
            if (nSlot != SLOT_ITEM_SOCKETS)
                continue;



            IVector2D tSizeSlot;
            ACDInventorySlotGetSize(nSlot, tSizeSlot);

            for (int32 y = 0; y < tSizeSlot.y; ++y)
            {
                for (int32 x = 0; x < tSizeSlot.x; ++x)
                {
                    IVector2D tPoint(x, y);
                    ACDID idACDSquare = ACDInventoryGetSquareItem(tACD, nSlot, tPoint);

                    ActorCommonData *ptACDSquare = ACDTryToGet(idACDSquare);

                    if (ptACDSquare && ptACDSquare->IsItem())
                    {
                        ptInactiveMarker->arSocketedItems[nSocketedItemCount] = InactiveItemMarkerCreate((ActorID)ACDGetOwner(idACDSquare));
                        ++nSocketedItemCount;
                    }
                }
            }
        }



        ptInactiveMarker->nSocketedItemCount = nSocketedItemCount;
    }

    return ptInactiveMarker;
}
#include "Hyperlink.h"
#include "Definitions/GameBalanceDefinition.h"
#include "../Sigma/TypeDescriptor/SymbolicValue.h"
// Raw DWARF records gActorTypes only at Sno/SymbolicValue.cpp:5221; no recovered header declaration.
extern const SymbolicEntry gActorTypes[13];

#line 2275
void InactiveMarkerCreateFromActor(
    ActorID idActor)
{
    Actor *ptActor = ActorGet(idActor);
    ActorCommonData &tACD = ACDGet(ptActor->idACD);
    SSceneID idSScene = ACDGetCurrentSSceneID(tACD);
    SceneCommonData *ptSCD = SceneCommonDataTryToGet(idSScene);
    if (!ptSCD)
    {
        return;
    }


    InactiveMarkerList *ptInactiveMarkerList = ptSCD->ptMarkerScene;

    if (tACD.hGB.eType == GB_ITEMS)
    {
        InactiveItemMarker *ptInactiveMarker = InactiveItemMarkerCreate(idActor);

        sAddInactiveMarkerToList(ptInactiveMarkerList, ptInactiveMarker);
    }
    else
    {
        InactiveBaseMarker *ptInactiveMarker = NULL;

        ScopedActorDefinitionLockR pdefActor(ptActor->snoActor);
        ActorType eType = pdefActor->eType;
        if (tACD.IsNecroCorpse())
        {
            return;
        }
        if (tACD.IsMonster())
        {
            ptInactiveMarker = InactiveMonsterMarkerCreate(idActor);
        }
        else if (tACD.IsGizmo() || eType == ACTORTYPE_SERVER_PROP)
        {
            ptInactiveMarker = InactiveGizmoMarkerCreate(idActor);
        }
        else
        {
            CRefString sError = CRefString().Format("Error creating inactive marker for actor '%s' (Actor SNO '%s', Type '%s'); it isn't a monster, gizmo, or item.\n",
                MarkerNameGetString(ptActor->tName),
                HyperlinkCreateFromSNO(SNOName(ACTOR_SNO, ptActor->snoActor), NULL).str(),
                SymbolToName(eType, gActorTypes));

        }

        if (ptInactiveMarker)
        {
            sAddInactiveMarkerToList(ptInactiveMarkerList, ptInactiveMarker);
        }
    }
}

#include "../Server/SGameManager.h"
#include "../Server/SWorld.h"
#include "../Server/SScene.h"
#include "GameSwig.h"
// Raw DWARF records only World.cpp:605 and SceneCommon.cpp:729; no recovered header declarations.
SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete);
void SceneCommonDataActivate(const SSceneID idSScene);

// Shape from DWARF columns: scene tests nested, bFound set by ifs, second idSWorld test instead of else.
#line 2126
ACDID ActivateActorMarkerInWorld(
    SNO snoActor,
    SNO snoWorld,
    SNO snoLevelArea)
{
    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);


    if (idSWorld != SWORLDID_INVALID)
    {
        ActorIterator tIt;
        while (ActorDataArrayIterate(tIt))
        {
            const Actor *ptActor = tIt.ptElement;
            if (ptActor->snoActor == snoActor)
            {
                ActorCommonData &tACD = ACDGet(ACDFindByActor(ptActor->id));
                if (tACD.GetSWorldID() == idSWorld)
                {
                    if (ACDGetLevelArea(tACD) == snoLevelArea)
                    {

                        return tACD.id;
                    }
                }
            }
        }
    }

    if (idSWorld == SWORLDID_INVALID)
    {



        idSWorld = SWorldCreate(snoWorld, TRUE, FALSE, NULL, ACDID_INVALID, ACDID_INVALID, 0, WARP_UNKNOWN, FALSE, 0);
    }


    if (idSWorld != SWORLDID_INVALID)
    {
        SGame &tSGame = SGameManager::GameGet();
        TDataArrayIterator<SScene> tItSSceneForMarkers;
        while (tSGame.ptSScenes->Iterate(tItSSceneForMarkers))
        {
            SScene *ptSScene = tItSSceneForMarkers.ptElement;
            if (ptSScene->tSCD.idSWorld == idSWorld)
            {
                if (!(ptSScene->tSCD.dwSceneFlags & (1 << SCENE_WAS_ACTIVATED_BEFORE)))
                    continue;



                if (ptSScene->tSCD.ptMarkerScene)
                {
                    TInactiveMarkerList *ptInactiveMarkersList = ptSScene->tSCD.ptMarkerScene->ptInactiveMarkerList;

                    if (ptInactiveMarkersList)
                    {
                        POS pos = ptInactiveMarkersList->GetHeadPosition();
                        while (pos)
                        {
                            POS posCurrent = pos;
                            InactiveBaseMarker *ptInactiveMarker = ptInactiveMarkersList->GetNext(pos);
                            BOOL bFound = FALSE;
                            switch (ptInactiveMarker->eInactiveMarkerType)
                            { case INACTIVE_MONSTER: {
                                InactiveMonsterMarker *ptInactiveMonster = (InactiveMonsterMarker *)ptInactiveMarker;
                                if (ptInactiveMonster->snoMonster == snoActor)
                                    bFound = TRUE;
                                break;
                            }


                            case INACTIVE_GIZMO: {
                                InactiveGizmoMarker *ptInactiveGizmo = (InactiveGizmoMarker *)ptInactiveMarker;
                                if (ptInactiveGizmo->snoActor == snoActor)
                                    bFound = TRUE;
                                break;
                            }
                            }

                            if (ptInactiveMarker->tMarkerOwner.snoMarkerSet != SNO_NONE)
                            {
                                ScopedMarkerSetDefinitionLockR ptMarkerSetDef(ptInactiveMarker->tMarkerOwner.snoMarkerSet);
                                const Marker &tMarker = ptMarkerSetDef->tMarkerSet[ptInactiveMarker->tMarkerOwner.nMarkerIndex];
                                if (tMarker.snoname.snoGroup == ACTOR_SNO &&
                                    tMarker.snoname.snoHandle == snoActor)
                                {
                                    bFound = TRUE;
                                }
                            }

                            if (bFound)
                            {



                                ptInactiveMarkersList->RemoveAt(posCurrent);



                                SceneCommonDataActivate(ptSScene->id);

                                ACDID idACD = sInactiveMarkerActivate(ptSScene->tSCD.idSWorld, ptSScene->id, ptInactiveMarker);

                                InactiveMarkerFree(ptInactiveMarker);

                                if (idACD != ACDID_INVALID)
                                {
                                    if (ACDGetLevelArea(ACDGet(idACD)) == snoLevelArea)
                                    {
                                        return idACD;
                                    }
                                }

                                break;
                            }
                        }
                    }
                }
            }
        }
    }





















    return ACDID_INVALID;
}

#include "../Sigma/Math/Intersect.h"
#include "../Sigma/MemoryPools.h"

#line 844
static BOOL sInactiveMarkerGetStatus(
    uint32 tName)
{
    GameCommonData &tGCD = GameGetActiveGameCommonData();
    InactiveMarkerGlobals *ptInactiveMarkerGlobals = tGCD.ptInactiveMarkerGlobals;

    BOOL bValue = FALSE;
    uint32 dwNameHash = tName;

    ptInactiveMarkerGlobals->tInactiveMarkerActivateStatusMap.Lookup(dwNameHash, bValue);

    return bValue;
}





#line 909
static BOOL sCanActivateInactiveItem(
    InactiveItemMarker *ptInactiveMarker,
    const Frustum *ptFrustum,
    const Vector3D *ptCenterPos)
{
    if (ptFrustum)
    {
        if (!Intersect::FrustumAndSphere(*ptFrustum, ptInactiveMarker->wsBounds))
            return FALSE;
    }



    if (ptCenterPos)
    {
        if (Vector3D::DistanceSquared(*ptCenterPos, ptInactiveMarker->wsBounds.wpCenter) > MAX_MARKER_VIEW_DISTANCE_SQR)
            return FALSE;
    }


    return TRUE;
}


#line 935
static BOOL sCanActivateInactiveMarker(
    InactiveBaseMarker *ptInactiveMarker,
    const Frustum *ptFrustum,
    const Vector3D *ptCenterPos,
    BOOL bSceneMarkers)
{
    if (ptInactiveMarker->nFlags & (1 << INACTIVE_MARKER_FLAG_WAITING_ON_QUEST))
        return FALSE;



    if (ptInactiveMarker->nFlags & (1 << INACTIVE_MARKER_FLAG_CHECK_ACTIVATE_STATUS))
    {
        uint32 tMarkerName;
        InactiveMarkerGetFullName(ptInactiveMarker, tMarkerName);


        if (tMarkerName && !sInactiveMarkerGetStatus(tMarkerName))
            return FALSE;
    }


    // DWARF 958:11/10: the scene-flag test is the first operand; bSceneMarkers has no row (unswitched).
    if ((ptInactiveMarker->nFlags & (1 << INACTIVE_MARKER_FLAG_IS_SCENE_MARKER)) || bSceneMarkers)
        return (ptInactiveMarker->nFlags & (1 << INACTIVE_MARKER_FLAG_IS_SCENE_MARKER)) != 0;





    if (ptFrustum)
    {
        if (!Intersect::FrustumAndSphere(*ptFrustum, ptInactiveMarker->wsBounds))
            return FALSE;
    }



    if (ptCenterPos)
    {
        if (Vector3D::DistanceSquared(*ptCenterPos, ptInactiveMarker->wsBounds.wpCenter) > MAX_MARKER_VIEW_DISTANCE_SQR)
            return FALSE;
    }


    return TRUE;
}



#line 1095
static void sActivatePackInList(
    SWorldID idSWorld,
    SSceneID idSScene,
    TInactiveMarkerList *ptInactiveMarkersList,
    TInactiveMarkerList *ptActivateMarkersList,
    ACDPackID idACDPack)
{







    XPooledDataIDList listACDPacks(MemoryPoolsGetDATAIDListPool());
    POS pos = ptInactiveMarkersList->GetHeadPosition();

    while (pos)
    {
        POS posCurrent = pos;
        InactiveBaseMarker *ptInactiveMarker = ptInactiveMarkersList->GetNext(pos);

        ACDPackID idACDMarkerPack = InactiveMarkerGetPack(ptInactiveMarker);

        if (idACDMarkerPack != idACDPack)
            continue;






        ptActivateMarkersList->AddTail_Tracked(ptInactiveMarker);

        ptInactiveMarkersList->RemoveAt(posCurrent);
    }



}



// Loop body uses `if ( !sCanActivate... ) continue;`: the DWARF puts idACDPack (1013) in the loop-body block, and 1015:14 / 1031:36 put it at 8 spaces.
#line 987
static void sActivateInList(
    SWorldID idSWorld,
    SSceneID idSScene,
    TInactiveMarkerList *ptInactiveMarkersList,
    TInactiveMarkerList *ptActivateMarkersList,
    const Frustum *ptFrustum,
    const Vector3D *ptCenterPos,
    BOOL bSceneMarkers)
{
    if ( !ptInactiveMarkersList )
        return;



    XPooledDataIDList listACDPacks(MemoryPoolsGetDATAIDListPool());
    POS pos = ptInactiveMarkersList->GetHeadPosition();

    while (pos)
    {
        POS posCurrent = pos;
        InactiveBaseMarker *ptInactiveMarker = ptInactiveMarkersList->GetNext(pos);
        if ( !sCanActivateInactiveMarker(ptInactiveMarker, ptFrustum, ptCenterPos, bSceneMarkers) )
            continue;



        ACDPackID idACDPack = InactiveMarkerGetPack(ptInactiveMarker);

        if ( idACDPack != ACDPACKID_INVALID )
        {


            if (!listACDPacks.Find(idACDPack))
            {
                listACDPacks.AddTail_Tracked(idACDPack);
            }
        }
        else
        {
            ptActivateMarkersList->AddTail_Tracked(ptInactiveMarker);




            ptInactiveMarkersList->RemoveAt(posCurrent);
        }
    }


    {
    POS posACD = listACDPacks.GetHeadPosition();

    while (posACD)
    {
        ACDPackID idACDPack = listACDPacks.GetNext(posACD);

        sActivatePackInList(idSWorld, idSScene, ptInactiveMarkersList, ptActivateMarkersList, idACDPack);
    }
    }
}

#line 1050
static void sActivateItemsInList(
    SWorldID idSWorld,
    SSceneID idSScene,
    TInactiveItemMarkerList *ptInactiveItemMarkersList,
    TInactiveItemMarkerList *ptActivateItemMarkersList,
    const Frustum *ptFrustum,
    const Vector3D *ptCenterPos)
{







    if (!ptInactiveItemMarkersList)
        return;



    POS pos = ptInactiveItemMarkersList->GetHeadPosition();

    while (pos)
    {
        POS posCurrent = pos;

        InactiveItemMarker *ptInactiveMarker = ptInactiveItemMarkersList->GetNext(pos);

        if (!sCanActivateInactiveItem(ptInactiveMarker, ptFrustum, ptCenterPos))
            continue;






        ptActivateItemMarkersList->AddTail_Tracked(ptInactiveMarker);

        ptInactiveItemMarkersList->RemoveAt(posCurrent);
    }

}

#line 445
void InactiveMarkerActivateInFrustrum(
    SWorldID idSWorld,
    SSceneID idSScene,
    InactiveMarkerList *ptInactiveMarkerList,
    InactiveMarkerList *ptActivateMarkerList,
    const Frustum &tFrustum,
    const Vector3D &vCenterPos)
{
    sActivateInList(idSWorld, idSScene, ptInactiveMarkerList->ptInactiveMarkerList, ptActivateMarkerList->ptInactiveMarkerList, &tFrustum, &vCenterPos, FALSE);
    sActivateItemsInList(idSWorld, idSScene, ptInactiveMarkerList->ptInactiveItemMarkerList, ptActivateMarkerList->ptInactiveItemMarkerList, &tFrustum, &vCenterPos);
}

#line 482
void InactiveMarkerActivateInFrustrum(
    SSceneID idSScene,
    const Frustum &tFrustum,
    const Vector3D &vCenterPos)
{
    SceneCommonData &tSCD = SceneCommonDataGet(idSScene);

    InactiveMarkerActivateInFrustrum(tSCD.idSWorld, idSScene, tSCD.ptMarkerScene, tSCD.ptMarkersToBeActivated, tFrustum, vCenterPos);
}


#line 433
void InactiveMarkerActivateAll(
    SWorldID idSWorld,
    SSceneID idSScene,
    InactiveMarkerList *ptInactiveMarkerList,
    InactiveMarkerList *ptActivateMarkerList)
{
    sActivateInList(idSWorld, idSScene, ptInactiveMarkerList->ptInactiveMarkerList, ptActivateMarkerList->ptInactiveMarkerList, NULL, NULL, FALSE);
    sActivateItemsInList(idSWorld, idSScene, ptInactiveMarkerList->ptInactiveItemMarkerList, ptActivateMarkerList->ptInactiveItemMarkerList, NULL, NULL);
}

#line 459
void InactiveMarkerActivateSceneMarkers(
    SWorldID idSWorld,
    SSceneID idSScene,
    InactiveMarkerList *ptInactiveMarkerList,
    InactiveMarkerList *ptActivateMarkerList)
{
    sActivateInList(idSWorld, idSScene, ptInactiveMarkerList->ptInactiveMarkerList, ptActivateMarkerList->ptInactiveMarkerList, NULL, NULL, TRUE);
}

#line 494
void InactiveMarkerActivateInFrustrum(
    SSceneID idSScene,
    const Frustum &tFrustum)
{
    SceneCommonData &tSCD = SceneCommonDataGet(idSScene);

    sActivateInList(tSCD.idSWorld, idSScene, tSCD.ptMarkerScene->ptInactiveMarkerList, tSCD.ptMarkersToBeActivated->ptInactiveMarkerList, &tFrustum, NULL, FALSE);
    sActivateItemsInList(tSCD.idSWorld, idSScene, tSCD.ptMarkerScene->ptInactiveItemMarkerList, tSCD.ptMarkersToBeActivated->ptInactiveItemMarkerList, &tFrustum, NULL);
}

#line 506
void InactiveMarkerActivateInFrustrumForWorld(
    SWorldID idSWorld,
    const Frustum &tFrustum,
    const Vector3D &vCenterPos)
{
    WorldCommonData &tWCD = WorldGetCommonData(idSWorld);

    InactiveMarkerActivateInFrustrum(idSWorld, SSCENEID_INVALID, tWCD.ptMarkerWorld, tWCD.ptMarkersToActivate, tFrustum, vCenterPos);
}

#line 518
void InactiveMarkerActivateAllInScene(
    SSceneID idSScene)
{
    SceneCommonData &tSCD = SceneCommonDataGet(idSScene);

    InactiveMarkerActivateAll(tSCD.idSWorld, idSScene, tSCD.ptMarkerScene, tSCD.ptMarkersToBeActivated);
}

#line 538
void InactiveMarkerActivateSceneMarkers(
    SSceneID idSScene)
{
    SceneCommonData &tSCD = SceneCommonDataGet(idSScene);

    InactiveMarkerActivateSceneMarkers(tSCD.idSWorld, idSScene, tSCD.ptMarkerScene, tSCD.ptMarkersToBeActivated);
}

#include "../Server/Map/Minimap.h"
#include "Networking/Network.h"
#include "OnlineService/OnlineService.h"
#include "../Server/SItems.h"
// Raw DWARF records only Items.cpp:5565 and ActorInventory.cpp:7825; no recovered header declarations.
void ItemSetBoundToACD(ActorCommonData &tACD, const ACDID idACDOwner);
BOOL ActorInventorySocketItem(ActorCommonData &tACDItemToSocket, ActorCommonData &tACDItemSocketedInto, const IVector2D &tPoint, BOOL bMarkItemAsDirty);

#line 1368
static void sRestoreItemMarkerAttributes(
    ActorCommonData &tACDItem,
    InactiveItemMarker *ptInactiveItemMarker)
{
    if (ACDTryToGet(ptInactiveItemMarker->idACDBoundTo))
    {
        if (ptInactiveItemMarker->bSoulbound)
        {
            ItemSetBoundToACD(tACDItem, ptInactiveItemMarker->idACDBoundTo);
        }
        else
        {
            ItemSetLockedToACD(tACDItem, ptInactiveItemMarker->idACDBoundTo);


            tACDItem.SetUnlockTime((uint64)ptInactiveItemMarker->uUnlockTime * 1000000);
        }
    }

    if (ptInactiveItemMarker->nGoldAmount != -1)
    {
        tACDItem.AttributesSetInt(ATTRIBUTE_GOLD, ptInactiveItemMarker->nGoldAmount);
        tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_QUALITY_LEVEL, IQL_NORMAL);
    }

    if (ptInactiveItemMarker->idPing != PINGID_INVALID)
    {
        tACDItem.AttributesSetInt(ATTRIBUTE_PING, ptInactiveItemMarker->idPing);
    }

    if (ptInactiveItemMarker->idAssignedHero != OnlineService::INVALID_HERO_ID)
    {
        tACDItem.SetAssignedHeroID(ptInactiveItemMarker->idAssignedHero);
    }


    if (ptInactiveItemMarker->idAssignedGameConnectionID != GAMECONNECTIONID_INVALID)
    {
        tACDItem.SetAssignedGameConnectionID(ptInactiveItemMarker->idAssignedGameConnectionID);
    }

    if (ptInactiveItemMarker->idPlayerGiftRecipient)
    {
        tACDItem.ptFriendGiftData = new FriendGiftData;

        tACDItem.ptFriendGiftData->idRecipient = ptInactiveItemMarker->idPlayerGiftRecipient;
        tACDItem.ptFriendGiftData->sNameRecipient = CRefString(ptInactiveItemMarker->szPlayerName);
    }

    tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_PLAYER_RECIPIENT, ptInactiveItemMarker->nPlayerOwner);


    for (int32 i = 0; i < 4; ++i)
    {
        if (ptInactiveItemMarker->idTradablePlayers[i])
        {
            tACDItem.SetItemTradePlayerID(ptInactiveItemMarker->idTradablePlayers[i], i);
        }
    }
    tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_TRADE_END_TIME, ptInactiveItemMarker->uTradeEndTime);

    if (ptInactiveItemMarker->idSourcePlayer)
    {
        tACDItem.SetItemSourcePlayerID(ptInactiveItemMarker->idSourcePlayer);
    }

    if (ptInactiveItemMarker->bPersisted)
    {
        Actor *ptActor = ActorTryToGet((ActorID)tACDItem.idOwner);
        if (ptActor)
        {

            ptActor->dwActorFlags |= 1 << ACTOR_DB_PERSISTED;
        }
    }

    if (ptInactiveItemMarker->bCheated)
    {
        Actor *ptActor = ActorTryToGet((ActorID)tACDItem.idOwner);
        if (ptActor)
        {

            ptActor->dwActorFlags |= 1 << ACTOR_CHEATED;
        }
    }

    if (ptInactiveItemMarker->dwSeasonCreated)
    {
        Actor *ptActor = ActorTryToGet((ActorID)tACDItem.idOwner);
        if (ptActor)
        {

            ptActor->dwSeasonCreated = ptInactiveItemMarker->dwSeasonCreated;
            tACDItem.AttributesSetInt(ATTRIBUTE_SEASON, ptInactiveItemMarker->dwSeasonCreated);
        }
    }
}

#line 1469
static void sInactiveItemMarkerActivate(
    SWorldID idSWorld,
    SSceneID idSScene,
    InactiveItemMarker *ptInactiveItemMarker)
{





    if (GameIsSeasonal() && ptInactiveItemMarker->nPlayerOwner != -1)
    {
        Player *ptPlayer = PlayerGetByPlayerIndex(ptInactiveItemMarker->nPlayerOwner);
        if (!ptPlayer ||
            ptPlayer->dwJoinTime > ptInactiveItemMarker->dwCreateTime ||
            ptPlayer->idPlayerACD == ACDID_INVALID ||
            !PlayerIsInWorld(*ptPlayer))
            return;
    }






    BOOL bItemBound = ptInactiveItemMarker->idACDBoundTo != ACDID_INVALID;
    if (bItemBound && !ACDTryToGet(ptInactiveItemMarker->idACDBoundTo))
        return;



    int32 nSocketCount = ptInactiveItemMarker->nSocketedItemCount;
    for (int32 i = 0; i < nSocketCount; ++i)
    {
        InactiveItemMarker *ptInactiveSocketItem = ptInactiveItemMarker->arSocketedItems[i];
        if (ptInactiveSocketItem->idACDBoundTo != ACDID_INVALID && !ACDTryToGet(ptInactiveSocketItem->idACDBoundTo))
            return;
    }




    WorldPlace tWorldPlace;
    tWorldPlace.wp = ptInactiveItemMarker->wp;
    tWorldPlace.idSWorld = idSWorld;

    ACDID idACDItem = SItemGenerate(
        ptInactiveItemMarker->tItemId,
        ptInactiveItemMarker->tItemGenerator,
        tWorldPlace,
        0.0f, 0);


    if (idACDItem == ACDID_INVALID)
        return;



    ActorCommonData &tACDItem = ACDGet(idACDItem);

    sRestoreItemMarkerAttributes(tACDItem, ptInactiveItemMarker);

    if (nSocketCount)
    {
        for (int32 i = 0; i < nSocketCount; ++i)
        {
            InactiveItemMarker *ptInactiveSocketItem = ptInactiveItemMarker->arSocketedItems[i];
            ACDID idACDSocketedItem = SItemGenerate(
                ptInactiveSocketItem->tItemId,
                ptInactiveSocketItem->tItemGenerator,
                cgplaceNull, 0.0f, 0);


            ActorCommonData *ptACDSocketedItem = ACDTryToGet(idACDSocketedItem);
            if (!ptACDSocketedItem)
                continue;



            sRestoreItemMarkerAttributes(*ptACDSocketedItem, ptInactiveSocketItem);

            IVector2D tPoint(0, i);

            ActorInventorySocketItem(*ptACDSocketedItem, tACDItem, tPoint, FALSE);
        }
    }
}

#line 1559
static void sActivateMarkerList(
    SWorldID idSWorld,
    SSceneID idSScene,
    TInactiveMarkerList *ptInactiveMarkerList)
{



    while (ptInactiveMarkerList->Count() > 0)
    {
        InactiveBaseMarker *ptInactiveMarker = ptInactiveMarkerList->GetHead();
        ptInactiveMarkerList->RemoveHead();

        sInactiveMarkerActivate(idSWorld, idSScene, ptInactiveMarker);
        InactiveMarkerFree(ptInactiveMarker);
    }
}


#line 1578
void ActivateInactiveMarkerList(
    SWorldID idSWorld,
    SSceneID idSScene,
    InactiveMarkerList *ptActivateMarkerList)
{
    sActivateMarkerList(idSWorld, idSScene, ptActivateMarkerList->ptInactiveMarkerList);

    POS pos = ptActivateMarkerList->ptInactiveItemMarkerList->GetHeadPosition();
    while (pos)
    {
        InactiveItemMarker *ptInactiveMarker = ptActivateMarkerList->ptInactiveItemMarkerList->GetNext(pos);

        sInactiveItemMarkerActivate(idSWorld, idSScene, ptInactiveMarker);
        InactiveMarkerFree(ptInactiveMarker);
    }

    ptActivateMarkerList->ptInactiveItemMarkerList->RemoveAll();



}

#include "Actors/ComplexEffect.h"
// Raw DWARF records SComplexEffectAdd only at SComplexEffect.cpp:175; no recovered header declaration.
SComplexEffectID SComplexEffectAdd(ActorCommonData &tACD, const SNO snoEffectGroup, const ACDID idACDDestParam, ComplexEffectType eType, uint32 dwParam1, uint32 dwParam2, BOOL bIgnoreOwnerAlpha);

#line 1610
void sMarkerRestoreFromFlags(
    ActorCommonData &tACD,
    const InactiveBaseMarker *ptInactiveMarker)
{
    if (ptInactiveMarker->nFlags & (1 << INACTIVE_MARKER_FLAG_DISABLED))
    {
        tACD.SetEnabled(FALSE);
    }

    tACD.AttributesSetInt(ATTRIBUTE_NPC_DISABLE_OPERATION,
        (ptInactiveMarker->nFlags & (1 << INACTIVE_MARKER_FLAG_DISABLE_OPERATION)) != 0);

    if (ptInactiveMarker->nFlags & (1 << INACTIVE_MARKER_FLAG_QUEST_MONSTER))
    {
        tACD.AttributesSetInt(ATTRIBUTE_QUEST_MONSTER, TRUE);
        SNO snoEffect = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_QUEST_MOB);
        SComplexEffectID idEffect = SComplexEffectAdd(tACD, snoEffect, ACDID_INVALID, COMPLEXEFFECT_TYPE_NORMAL, 0, 0, FALSE);
        tACD.AttributesSetInt(ATTRIBUTE_QUEST_MONSTER_EFFECT, idEffect);
    }

    tACD.AttributesSetInt(ATTRIBUTE_MINIMAP_ICON_ACTIVE,
        (ptInactiveMarker->nFlags & (1 << INACTIVE_MARKER_FLAG_MINIMAP_ENABLED)) != 0);

    tACD.AttributesSetInt(ATTRIBUTE_BOUNTY_OBJECTIVE,
        (ptInactiveMarker->nFlags & (1 << INACTIVE_MARKER_FLAG_BOUNTY_OBJECTIVE)) != 0);
}

#line 1966
void ActivateMarkerNames(
    const blz::vector<unsigned int> & aMarkerNames,
    blz::vector<ACDID> & aActivatedACDs,
    SNO snoMarkerSet,
    uint16 nAdventureParentIndex,
    uint32 nMarkerSetInstanceIndex)
{
    aActivatedACDs.clear();
    size_t nMarkerNames = aMarkerNames.size();
    if (!nMarkerNames)
        return;


    for (size_t i = 0; i < nMarkerNames; ++i)
    {
        aActivatedACDs.push_back(ACDID_INVALID);
    }

    int32 nActivatedActors = 0;
    size_t nACD = 0;
    SGame & tSGame = SGameManager::GameGet();
    TDataArrayIterator<SScene> tItSSceneForMarkers;
    while (tSGame.ptSScenes->Iterate(tItSSceneForMarkers))
    {
        SScene * ptSScene = tItSSceneForMarkers.ptElement;
        if ((ptSScene->tSCD.dwSceneFlags & (1 << SCENE_ACTIVATED)) &&
            ptSScene->tSCD.ptMarkerScene)
        {
            TInactiveMarkerList * ptInactiveMarkersList = ptSScene->tSCD.ptMarkerScene->ptInactiveMarkerList;
            TInactiveMarkerList * ptActivateList = sAllocTInactiveMarkerList();
            if (ptInactiveMarkersList)
            {
                POS pos = ptInactiveMarkersList->GetHeadPosition();
                while (pos)
                {
                    POS posCurrent = pos;
                    InactiveBaseMarker * ptInactiveMarker = ptInactiveMarkersList->GetNext(pos);
                    BOOL bMarkerSetCheck;
                    BOOL bCheckInstanceIndex;
                    // Inference: these named booleans encode the original optional marker-set/instance tests.
                    bMarkerSetCheck = snoMarkerSet == SNO_NONE ? ptInactiveMarker->tMarkerOwner.snoMarkerSet != SNO_NONE : ptInactiveMarker->tMarkerOwner.snoMarkerSet == snoMarkerSet;
                    // Inference: a specified marker set also requires matching its instance index.
                    bCheckInstanceIndex = snoMarkerSet != SNO_NONE;






                    if (bMarkerSetCheck &&
                        ptInactiveMarker->tMarkerOwner.nMarkerIndex != -1 &&
                        ptInactiveMarker->tMarkerOwner.nAdventureParentIndex == nAdventureParentIndex &&
                        (!bCheckInstanceIndex || ptInactiveMarker->tMarkerOwner.nMarkerSetInstanceIndex == nMarkerSetInstanceIndex))
                    {
                        ScopedMarkerSetDefinitionLockR ptMarkerSetDef(ptInactiveMarker->tMarkerOwner.snoMarkerSet);
                        const Marker & tMarker = ptMarkerSetDef->tMarkerSet[ptInactiveMarker->tMarkerOwner.nMarkerIndex];
                        for (uint32 i = 0; i < aMarkerNames.size(); ++i)
                        {
                            if (tMarker.dwHash == aMarkerNames[i])
                            {



                                if (ptInactiveMarker->nFlags & (1 << INACTIVE_MARKER_FLAG_WAITING_ON_QUEST))
                                    continue;



                                ptActivateList->AddTail_Tracked(ptInactiveMarker);
                                ptInactiveMarkersList->RemoveAt(posCurrent);
                                break;
                            }
                        }



                    }
                }
            }

            if (ptSScene->tSCD.ptMarkersToBeActivated)
            {
                ptInactiveMarkersList = ptSScene->tSCD.ptMarkersToBeActivated->ptInactiveMarkerList;
                if (ptInactiveMarkersList)
                {
                    POS pos = ptInactiveMarkersList->GetHeadPosition();
                    while (pos)
                    {
                        POS posCurrent = pos;
                        InactiveBaseMarker * ptInactiveMarker = ptInactiveMarkersList->GetNext(pos);
                        BOOL bMarkerSetCheck;
                        BOOL bCheckInstanceIndex;
                        // Inference: the pending-list pass repeats the original optional marker-set/instance tests.
                        bMarkerSetCheck = snoMarkerSet == SNO_NONE ? ptInactiveMarker->tMarkerOwner.snoMarkerSet != SNO_NONE : ptInactiveMarker->tMarkerOwner.snoMarkerSet == snoMarkerSet;
                        // Inference: a specified marker set also requires matching its instance index.
                        bCheckInstanceIndex = snoMarkerSet != SNO_NONE;






                        if (bMarkerSetCheck &&
                            ptInactiveMarker->tMarkerOwner.nMarkerIndex != -1 &&
                            ptInactiveMarker->tMarkerOwner.nAdventureParentIndex == nAdventureParentIndex &&
                            (!bCheckInstanceIndex || ptInactiveMarker->tMarkerOwner.nMarkerSetInstanceIndex == nMarkerSetInstanceIndex))
                        {
                            ScopedMarkerSetDefinitionLockR ptMarkerSetDef(ptInactiveMarker->tMarkerOwner.snoMarkerSet);
                            const Marker & tMarker = ptMarkerSetDef->tMarkerSet[ptInactiveMarker->tMarkerOwner.nMarkerIndex];
                            for (uint32 i = 0; i < aMarkerNames.size(); ++i)
                            {
                                if (tMarker.dwHash == aMarkerNames[i])
                                {
                                    ptActivateList->AddTail_Tracked(ptInactiveMarker);
                                    ptInactiveMarkersList->RemoveAt(posCurrent);
                                    break;
                                }
                            }



                        }
                    }
                }
            }

            POS pos = ptActivateList->GetHeadPosition();
            while (pos)
            {
                POS posCurr = pos;
                InactiveBaseMarker * ptInactiveMarker = ptActivateList->GetNext(pos);
                ACDID idACD = sInactiveMarkerActivate(ptSScene->tSCD.idSWorld, ptSScene->id, ptInactiveMarker);
                if (idACD != ACDID_INVALID)
                {

                    if (nACD < nMarkerNames)
                    {





                        aActivatedACDs[nACD] = idACD;
                    }

                    ++nACD;
                }

                ptActivateList->RemoveAt(posCurr);


                InactiveMarkerFree(ptInactiveMarker);
            }

            sFreeTInactiveMarkerList(ptActivateList);
        }
    }
}

#include "Gizmo.h"
#include "AnimPlayer.h"
#include "Definitions/ActDefinition.h"
// DWARF owning definitions: GlobalDefinition.cpp:1318, SGizmo.cpp:2833, PortalCommon.cpp:154.
uint32 ActorGroupIndexToHash(int32 nIndex);
// DWARF SceneCommon.cpp:1891; no header declaration currently adopted.
SWorldID SceneCommonGetSWorldID(SSceneID idSScene);
void GizmoRestoreState(ActorCommonData & tACD);
void SPortalSpecifierInitialize(Actor * ptActor, const ResolvedPortalDestination & tDest);
#line 1779
ACDID InactiveGizmoActivate(
    SWorldID idSWorld,
    SSceneID idSScene,
    InactiveGizmoMarker * ptInactiveMarker)
{



    WorldPlace tPlace;
    tPlace.idSWorld = SceneCommonGetSWorldID(idSScene);
    tPlace.wp = ptInactiveMarker->transform.wp;

    uint32 dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION) | (1 << ACTORADD_IS_FROM_INACTIVE);



    ActorAddStruct tActorAdd;
    tActorAdd.snoActor = ptInactiveMarker->snoActor;
    tActorAdd.idMarkersSScene = idSScene;
    tActorAdd.eTeam = (Team)ptInactiveMarker->nTeam;
    tActorAdd.tPlace = tPlace;
    tActorAdd.qOrientation = ptInactiveMarker->transform.q;
    tActorAdd.flScale = ptInactiveMarker->flScale;
    tActorAdd.nLevel = ptInactiveMarker->nLevel;
    ScenesCommonNavMeshNodeFromPlace(tPlace, &tActorAdd.tNavMeshHandle, FALSE, FALSE);

    uint32 tMarkerName;
    InactiveMarkerGetMarkerName(ptInactiveMarker, tMarkerName);
    if (tMarkerName)
    {
        tActorAdd.tMarkersName = tMarkerName;
        dwAddFlags |= 1 << ACTORADD_KEEP_MARKER_NAME;
    }

    tActorAdd.dwActorAddFlags = dwAddFlags;
    tActorAdd.tMarkerOwner = ptInactiveMarker->tMarkerOwner;
    tActorAdd.uPrimaryGroup = ActorGroupIndexToHash(ptInactiveMarker->nPrimaryGroupIndex);
    tActorAdd.uSecondaryGroup = ActorGroupIndexToHash(ptInactiveMarker->nSecondaryGroupIndex);

    if (ptInactiveMarker->tMarkerOwner.snoMarkerSet != SNO_NONE && ptInactiveMarker->tMarkerOwner.nMarkerIndex != -1)
    {
        ScopedMarkerSetDefinitionLockR pdefMarkerSet(ptInactiveMarker->tMarkerOwner.snoMarkerSet);

        if (ptInactiveMarker->tMarkerOwner.nMarkerIndex >= 0 && ptInactiveMarker->tMarkerOwner.nMarkerIndex < pdefMarkerSet->tMarkerSet.GetArraySize())
        {
            const Marker & tMarker = pdefMarkerSet->tMarkerSet[ptInactiveMarker->tMarkerOwner.nMarkerIndex];

            SNO snoScript = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_SCRIPT);
            if (snoScript != SNO_NONE)
            {
                tActorAdd.listSNOScripts.AddTail_Tracked(snoScript);
            }
            for (uint32 i = TAG_MARKER_SCRIPT_2; i < TAG_MARKER_SCRIPT_MAX; ++i)
            {
                SNO snoScript2 = TagMapGetSNO(tMarker.hTagMap, i);
                if (snoScript2 != SNO_NONE)
                {
                    tActorAdd.listSNOScripts.AddTail_Tracked(snoScript2);
                }
            }

            tActorAdd.snoConvList = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_CONVERSATION_LIST);


            SNO snoQuestRangeOpen = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_GIZMO_DOOR_OPEN_ON_QUESTRANGE);
            if (snoQuestRangeOpen != SNO_NONE && QuestRangeEvaluate(snoQuestRangeOpen, NULL))
            {
                tActorAdd.dwActorAddFlags |= 1 << ACTORADD_GIZMO_DOOR_OPEN;
                ptInactiveMarker->nGizmoState = 1;
            }
        }
    }

    tActorAdd.ptLuaSavedDataKey = &ptInactiveMarker->tLuaSavedDataKey;

    ActorID idActor;
    ScopedActorDefinitionLockR pdefActor(ptInactiveMarker->snoActor);


    tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
    tActorAdd.tSNOName.snoHandle = ptInactiveMarker->snoActor;
    idActor = GBAdd(tActorAdd);

    if (idActor == ACTORID_INVALID)
        return ACDID_INVALID;




    Actor * ptActor = ActorGet(idActor);
    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    GizmoType eGizmoType = tACD.GetGizmoType();

    ptActor->nSourceSceneUID = ptInactiveMarker->nSourceSceneUID;

    tACD.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, ptInactiveMarker->nGizmoState);
    tACD.AttributesSetInt(ATTRIBUTE_GIZMO_CHARGES, ptInactiveMarker->nGizmoCharges);
    tACD.AttributesSetInt(ATTRIBUTE_GIZMO_DISABLED_BY_SCRIPT, ptInactiveMarker->nGizmoDisabledByScript);
    tACD.AttributesSetInt(ATTRIBUTE_GENERAL_COOLDOWN, ptInactiveMarker->dwCooldown);
    tACD.AttributesSetInt(ATTRIBUTE_PING, ptInactiveMarker->idPing);

    if (eGizmoType == ACTORGIZMO_LORE_CHEST)
    {
        LoreGizmoRestoreOpenState(tACD);
    }
    else
    {
        tACD.AttributesSetInt(ATTRIBUTE_CHEST_OPEN, ptInactiveMarker->nGizmoChestOpen);
    }

    if (eGizmoType == ACTORGIZMO_PAGE_OF_FATE_PORTAL)
    {
        tACD.AttributesSetInt(ATTRIBUTE_PAGE_OF_FATE_ITEM, ptInactiveMarker->uSpawnerCountID);

        ResolvedPortalDestination tResolvedDest;
        ActorResolvePortalDestination(ptActor, tResolvedDest);
        SPortalSpecifierInitialize(ptActor, tResolvedDest);
    }
    else
    {
        tACD.AttributesSetInt(ATTRIBUTE_SPAWNER_CONCURRENT_COUNT_ID, ptInactiveMarker->uSpawnerCountID);
    }

    tACD.AttributesSetInt(ATTRIBUTE_DYNAMIC_ENTRANCE_GUID, ptInactiveMarker->uDynamicEntranceGUID);

    if (eGizmoType == ACTORGIZMO_MULTICLICK)
    {
        AnimPlayerSetIdleAnimation(tACD, ptInactiveMarker->dwAnimIdleTag);
    }

    if (ptInactiveMarker->dwFlags & (1 << GSA_FLAG_NO_DAMAGE))
    {
        tACD.SetTakesNoDamage(TRUE);
    }

    if (ptInactiveMarker->dwFlags & (1 << GSA_FLAG_DOES_FAKE_DAMAGE))
    {
        tACD.SetDoesFakeDamage(TRUE);
    }

    if (ptInactiveMarker->dwFlags & (1 << GSA_FLAG_INVULNERABLE))
    {
        tACD.SetInvulnerable(TRUE);
    }

    if (ptInactiveMarker->dwFlags & (1 << GSA_FLAG_CANNOT_BE_ADDED_TO_AI_TARGET_LIST))
    {
        tACD.SetCannotBeAddedToAITargetList(TRUE);
    }

    if (ptInactiveMarker->dwFlags & (1 << GSA_FLAG_GRANTS_NO_XP))
    {
        tACD.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
    }

    if (ptInactiveMarker->dwFlags & (1 << GSA_FLAG_DROPS_NO_LOOT))
    {
        tACD.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
    }

    if (ptInactiveMarker->dwFlags & (1 << GSA_FLAG_DISABLE_BRAIN))
    {
        ActorAISetStatusFlag(idActor, ACTOR_AI_DISABLE_UPDATES_BIT, TRUE);
    }

    sMarkerRestoreFromFlags(tACD, ptInactiveMarker);


    GizmoRestoreState(tACD);


    tACD.nScriptState = ptInactiveMarker->nScriptState;
    return tACD.id;
}

#include "../Server/SGameConstants.h"
// DWARF owning definitions: SceneCommon.cpp:2689, GlobalDefinition.cpp:1543, Actors.cpp:6818; no header declarations adopted.
BOOL SceneCommonIsActivated(const SSceneID idSScene);
float AdjustHPForPlayerCount(const float flHP, const int32 nOldPlayerMult, const int32 nNewPlayerMult);
void ActorAdjustHPForDifficultyChanged(const int32 nOldDifficulty, const int32 nNewDifficulty, float & flHP);
#line 2740
static void sRestoreActorAttributes(
    ActorID idActor,
    MonsterSavedAttributes &tAttributes)
{
    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData &tACD = ACDGet(idACD);

    int32 nPlayers = PlayerGetNumPlayersForDifficulty();
    float flHP = tAttributes.flHitPoints;
    int32 nNewPlayerMult = Math::Max(0, nPlayers - 1);
    int32 nOldPlayerMult = Math::Max(0, tAttributes.nPlayersWhenDeactivated - 1);

    if (!(tAttributes.dwSavedFlagAttributes & (1 << MSA_FLAG_IN_TIERED_RIFT)))
    {
        if (nOldPlayerMult != nNewPlayerMult)  // Inference: no row of its own (Max<int> line-0 cmp); nesting from the 2756 column-18 indent.
        {
            flHP = AdjustHPForPlayerCount(flHP, nOldPlayerMult, nNewPlayerMult);
        }
    }

    int32 nOldDifficulty = tAttributes.nDifficultyWhenDeactivated;
    int32 nNewDifficulty = GameGetHandicapLevel();
    if (nOldDifficulty != nNewDifficulty)
    {
        ActorAdjustHPForDifficultyChanged(nOldDifficulty, nNewDifficulty, flHP);
    }

    tACD.SetHitPoints(flHP, FALSE, FALSE);
    tACD.AttributesSetInt(ATTRIBUTE_LEVEL, tAttributes.nLevel);
    tACD.AttributesSetInt(ATTRIBUTE_DAMAGE_STATE_CURRENT, tAttributes.nDamageState);
    tACD.SetTeam(tAttributes.nTeam);
    AnimPlayerSetIdleAnimation(tACD, tAttributes.dwAnimIdleTag);

    if (tAttributes.dwSavedFlagAttributes & (1 << MSA_FLAG_NO_DAMAGE))
    {
        tACD.SetTakesNoDamage(TRUE);
    }

    if (tAttributes.dwSavedFlagAttributes & (1 << MSA_FLAG_DOES_FAKE_DAMAGE))
    {
        tACD.SetDoesFakeDamage(TRUE);
    }

    if (tAttributes.dwSavedFlagAttributes & (1 << MSA_FLAG_INVULNERABLE))
    {
        tACD.SetInvulnerable(TRUE);
    }

    if (tAttributes.dwSavedFlagAttributes & (1 << MSA_FLAG_CANNOT_BE_ADDED_TO_AI_TARGET_LIST))
    {
        tACD.SetCannotBeAddedToAITargetList(TRUE);
    }

    if (tAttributes.dwSavedFlagAttributes & (1 << MSA_FLAG_GRANTS_NO_XP))
    {
        tACD.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
    }

    if (tAttributes.dwSavedFlagAttributes & (1 << MSA_FLAG_RESURRECTED))
    {
        tACD.AttributesSetInt(ATTRIBUTE_RESURRECTED, TRUE);
    }

    if (tAttributes.dwSavedFlagAttributes & (1 << MSA_FLAG_DROPS_NO_LOOT))
    {
        tACD.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
    }

    if (tAttributes.dwSavedFlagAttributes & (1 << MSA_FLAG_HAS_DROPPED_SPECIAL_LOOT))
    {
        tACD.AttributesSetInt(ATTRIBUTE_HAS_DROPPED_SPECIAL_LOOT, TRUE);
    }

    if (tAttributes.dwSavedFlagAttributes & (1 << MSA_FLAG_DISABLE_BRAIN))
    {
        ActorAISetStatusFlag(idActor, ACTOR_AI_DISABLE_UPDATES_BIT, TRUE);
    }

    if (tAttributes.dwSavedFlagAttributes & (1 << MSA_FLAG_IS_FROM_DISTRIBUTION))
    {
        tACD.dwFlags |= 1 << ACD_IS_FROM_DISTRIBUTION_BIT;
    }

}

#line 1642
ACDID InactiveMonsterActivate(
    SWorldID idSWorld,
    SSceneID idSScene,
    InactiveMonsterMarker * ptInactiveMarker)
{




    for (int32 i = 0; i < ptInactiveMarker->nOccupiedSceneCount; i++)
    {
        SSceneID idSSceneTemp = ptInactiveMarker->arOccupiedScenes[i];
        if (!SceneCommonIsActivated(idSSceneTemp))
        {
            SceneCommonDataActivate(idSSceneTemp);
        }
    }

    WorldPlace tPlace;
    tPlace.idSWorld = SceneCommonGetSWorldID(idSScene);
    tPlace.wp = ptInactiveMarker->transform.wp;

    uint32 dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION) | (1 << ACTORADD_IS_FROM_INACTIVE);


    // Monster spawns in a finished/active loot-run boss rift are suppressed (mirror Actors.cpp GBAdd check; dwFlags2 byte 0x19d mask 3).
    SGame & tSGame = SGameManager::GameGet();
    if (SWorldIsTieredDungeonFinder(tPlace.idSWorld) &&
        tSGame.ptSGameGlobals &&
        (tSGame.ptSGameGlobals->dwFlags2 & ((1 << SGAMEGLOBALS2_LOOT_RUN_BOSS_KILLED) | (1 << SGAMEGLOBALS2_LOOT_RUN_BOSS_ACTIVE))))
    {
        return ACDID_INVALID;
    }

    ActorAddStruct tActorAdd;
    tActorAdd.idMarkersSScene = idSScene;
    tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
    tActorAdd.tSNOName.snoHandle = ptInactiveMarker->snoMonster;
    tActorAdd.snoActor = ptInactiveMarker->snoMonster;  // Inference: no row of its own; store merged into the 1677 stp (offset 4).
    tActorAdd.eMonsterRarity = (MonsterRarity)ptInactiveMarker->eMonsterRarity;
    tActorAdd.eTeam = (Team)ptInactiveMarker->tSavedAttributes.nTeam;
    tActorAdd.nLevel = ptInactiveMarker->tSavedAttributes.nLevel;
    tActorAdd.tPlace = tPlace;
    tActorAdd.qOrientation = ptInactiveMarker->transform.q;
    tActorAdd.flScale = ptInactiveMarker->flScale;

    uint32 tMarkerName;
    InactiveMarkerGetMarkerName(ptInactiveMarker, tMarkerName);
    if (tMarkerName)
    {
        dwAddFlags |= 1 << ACTORADD_KEEP_MARKER_NAME;
        tActorAdd.tMarkersName = tMarkerName;
    }

    tActorAdd.dwActorAddFlags = dwAddFlags;
    tActorAdd.prngActorSeed = &ptInactiveMarker->tRNGOriginal;
    tActorAdd.tNavMeshHandle = ptInactiveMarker->tNavMeshHandleCenter;
    tActorAdd.tNavMeshHandlePathingSpace = ptInactiveMarker->tNavMeshHandleCenter;  // Inference: no row; store merged into the 1698 stp.
    tActorAdd.tMarkerOwner = ptInactiveMarker->tMarkerOwner;
    tActorAdd.uPrimaryGroup = ActorGroupIndexToHash(ptInactiveMarker->nPrimaryGroupIndex);
    tActorAdd.uSecondaryGroup = ActorGroupIndexToHash(ptInactiveMarker->nSecondaryGroupIndex);
    SigmaMemoryCopy(tActorAdd.pRareNameGBIDs, ptInactiveMarker->arRareNameGBIDs, sizeof(ptInactiveMarker->arRareNameGBIDs));
    SigmaMemoryCopy(tActorAdd.pMonsterAffixGBIDs, ptInactiveMarker->arMonsterAffixGBIDs, sizeof(ptInactiveMarker->arMonsterAffixGBIDs));

    if (ptInactiveMarker->tMarkerOwner.snoMarkerSet != SNO_NONE && ptInactiveMarker->tMarkerOwner.nMarkerIndex != -1)
    {
        ScopedMarkerSetDefinitionLockR pdefMarkerSet(ptInactiveMarker->tMarkerOwner.snoMarkerSet);

        if (ptInactiveMarker->tMarkerOwner.nMarkerIndex >= 0 && ptInactiveMarker->tMarkerOwner.nMarkerIndex < pdefMarkerSet->tMarkerSet.GetArraySize())
        {
            const Marker & tMarker = pdefMarkerSet->tMarkerSet[ptInactiveMarker->tMarkerOwner.nMarkerIndex];

            SNO snoScript = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_SCRIPT);
            if (snoScript != SNO_NONE)
            {
                tActorAdd.listSNOScripts.AddTail_Tracked(snoScript);
            }
            for (uint32 i = TAG_MARKER_SCRIPT_2; i < TAG_MARKER_SCRIPT_MAX; ++i)
            {
                SNO snoScriptLoc = TagMapGetSNO(tMarker.hTagMap, i);
                if (snoScriptLoc != SNO_NONE)
                {
                    tActorAdd.listSNOScripts.AddTail_Tracked(snoScriptLoc);
                }
            }
            tActorAdd.snoConvList = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_CONVERSATION_LIST);
        }
    }

    tActorAdd.ptSavedAI = &ptInactiveMarker->tSavedAI;
    tActorAdd.ptLuaSavedDataKey = &ptInactiveMarker->tLuaSavedDataKey;
    tActorAdd.dwInventorySeed = ptInactiveMarker->tSavedAttributes.dwInventorySeed;


    ActorID idActor;
    idActor = GBAdd(tActorAdd);

    if (idActor == ACTORID_INVALID)
    {
        return ACDID_INVALID;
    }


    sRestoreActorAttributes(idActor, ptInactiveMarker->tSavedAttributes);

    Actor * ptActor = ActorGet(idActor);

    ptActor->nSourceSceneUID = ptInactiveMarker->nSourceSceneUID;
    ptActor->snoConvList = ptInactiveMarker->snoConvList;

    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    ACDPack * ptACDPack = ACDPackTryToGet(ptInactiveMarker->idACDPack);
    if (ptACDPack)
    {

        ptACDPack->AddMember(tACD);
        ptACDPack->RemoveDeactivated();
    }

    tACD.nScriptState = ptInactiveMarker->nScriptState;

    sMarkerRestoreFromFlags(tACD, ptInactiveMarker);


    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld && ptSWorld->bChallengeMonstersCleared)
    {
        if (tACD.IsMonster() && tACD.IsMonsterTeam())
        {
            tACD.SetEnabled(FALSE);
        }
    }

    return tACD.id;
}
