// Original file: X/Server/Network/KnownList.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "KnownList.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/DataStructures/XFixedBlockAllocator.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Game.h"
#include "../../Common/Player.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../SScene.h"
#include "../SWorld.h"
#include "../../Common/Definitions/SceneDefinition.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/DataStructures/XSphereNode.h"
#include "../../Sigma/Math/Intersect.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Common/HirelingSwig.h"
#include "../../Common/ItemsSwig.h"

#include "../../Common/ActorInventory.h"















typedef XListMemoryPool<KnownEntry> KnownEntryPool;
#include "../Actors/Actors.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/MarkerTags.h"
#include "../../Common/WorldPlace.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/Hireling.h"
#include "../System/EncounterTypes.h"








typedef XPooledList<KnownEntry, const KnownEntry &> KnownList;

typedef XGrowableMap<KnownEntry, const KnownEntry &, int, const int &> KnownEntryMap;







template <>
inline uint32 XHashKey<const KnownEntry &>(const KnownEntry & tEntry)
{
    return tEntry.eType * tEntry.id;
}



enum {
    KNOWNENTRY_MAP_TYPE_OLD = 0,
    KNOWNENTRY_MAP_TYPE_NEW = 1,
    KNOWNENTRY_MAP_TYPE_COUNT = 2,
};






enum {
    KNOWN_LIST_ACD_BIT_ARRAY_SIZE = 1472,
    KNOWN_LIST_ACD_BIT_ARRAY_SIZE_DWORDS = 368,
    KNOWN_LIST_SCENE_BIT_ARRAY_SIZE = 640,
    KNOWN_LIST_SCENE_BIT_ARRAY_SIZE_DWORDS = 160,
    KNOWN_LIST_WORLD_BIT_ARRAY_SIZE = 32,
    KNOWN_LIST_WORLD_BIT_ARRAY_SIZE_DWORDS = 8,
    KNOWN_LIST_TEAMS_BIT_ARRAY_SIZE = 3,
    KNOWN_LIST_TEAMS_BIT_ARRAY_SIZE_DWORDS = 1,
};

typedef uint32 BitArrayBlockType;







enum {
    BIT_ARRAY_BLOCK_TYPE_BITS = 32,
    BIT_ARRAY_BLOCK_SHIFT = 5,
    BIT_ARRAY_BLOCK_LOW_MASK = 31,
};

struct KnownListBitArray {  // size 0x1344
    BitArrayBlockType arACD[KNOWN_LIST_ACD_BIT_ARRAY_SIZE_DWORDS];  // 0x00
    BitArrayBlockType arScenes[KNOWN_LIST_SCENE_BIT_ARRAY_SIZE_DWORDS];  // 0x5c0
    BitArrayBlockType arSubScenes[KNOWN_LIST_SCENE_BIT_ARRAY_SIZE_DWORDS];  // 0x840
    BitArrayBlockType arWorldEnter[KNOWN_LIST_WORLD_BIT_ARRAY_SIZE_DWORDS];  // 0xac0
    BitArrayBlockType arWorldReveal[KNOWN_LIST_WORLD_BIT_ARRAY_SIZE_DWORDS];  // 0xae0
    BitArrayBlockType arTeams[KNOWN_LIST_TEAMS_BIT_ARRAY_SIZE_DWORDS];  // 0xb00
    BitArrayBlockType arPrefetchScenes[KNOWN_LIST_SCENE_BIT_ARRAY_SIZE_DWORDS];  // 0xb04
    BitArrayBlockType arFarACD[KNOWN_LIST_ACD_BIT_ARRAY_SIZE_DWORDS];  // 0xd84
};
static_assert(sizeof(KnownListBitArray) == 0x1344, "KnownListBitArray");

struct KnownListWrapper {  // size 0x18
    KnownListID id;  // 0x00
    KnownList * ptKnownList;  // 0x08
    KnownListBitArray * ptKnownListBitArrays;  // 0x10
};
static_assert(sizeof(KnownListWrapper) == 0x18, "KnownListWrapper");

struct KnownEntryListWrapper {  // size 0x10
    KnownEntryListID id;  // 0x00
    KnownList * ptKnownList;  // 0x08
};
static_assert(sizeof(KnownEntryListWrapper) == 0x10, "KnownEntryListWrapper");

typedef TDataArray<KnownListWrapper, int> KnownListDataArray;
typedef TDataArray<KnownEntryListWrapper, int> KnownEntryListDataArray;
typedef XFixedBlockAllocator<XPooledList<KnownEntry, const KnownEntry &>, 19> XKnownListAllocator;
typedef XFixedBlockAllocator<KnownListBitArray, 13> XKnownListBitArrayAllocator;

struct KnownEntryGlobals {  // size 0x118a0
    XKnownListAllocator tKnownListAllocator;  // 0x00
    XKnownListBitArrayAllocator tKnownListBitArrayAllocator;  // 0x330
    KnownListDataArray * ptKnownListArray;  // 0xfde0
    KnownEntryListDataArray * ptKnownEntryListArray;  // 0xfde8
    KnownEntryPool tKnownEntryPool;  // 0xfdf0
    KnownEntryMap tKnownEntryMap[KNOWNENTRY_MAP_TYPE_COUNT];  // 0xfe20
    KnownList tAddedList;  // 0xff20
    KnownList tChangedList;  // 0xff48
    KnownList tDeletedList;  // 0xff70
    KnownListBitArray tDeletedBitArray;  // 0xff98
    BitArrayBlockType arACDAddedBits[KNOWN_LIST_ACD_BIT_ARRAY_SIZE_DWORDS];  // 0x112dc
};
static_assert(sizeof(KnownEntryGlobals) == 0x118a0, "KnownEntryGlobals");

KnownEntryPool * KnownListEntryPoolGet();
void sKnownListItemDeleted(KnownEntryType eType, DataID idObject);

// Declarations of functions whose headers are not under src/ yet (or do not declare them).
void STeamAddTeamDataToKnownList(const Player & tPlayer, KnownListID idKnownList);  // X/Server/STeam.cpp:518
BOOL WorldTreatAsOneScene(SWorldID idSWorld);  // X/Common/World.cpp:802
BOOL TradeIsTrading(const ACDID idACDPlayer);  // X/Common/Trade.cpp:270
Player * TradeGetTradingTarget(const Player & tPlayer);  // X/Common/Trade.cpp:98
BOOL SPlayerTestAllowedToInspect(ACDID idACDPlayer, ACDID idACDToInspect);  // X/Server/SPlayer.cpp:4172
void SPlayerInspectEnd(ACDID idACDPlayer);  // X/Server/SPlayer.cpp:4266
#include "../SPlayer.h"  // ServerTrickleList, SPlayerGetPossibleTrickleList

int32 ACDInventoryGetType(const ActorCommonData & tACD);  // X/Common/ActorInventory.h
ACDID ACDInventoryGetSquareItem(const ActorCommonData & tACD, const InventorySlotType eSlot, const IVector2D & pt);  // X/Common/ActorInventory.cpp:2403
BOOL NPCShouldBeHiddenForClient(const ActorCommonData & tACD, const Player * ptPlayer);  // X/Server/SNPC.cpp
BOOL ActorInventoryAllowedToSeeItem(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem);  // X/Common/ActorInventory.cpp
BOOL SceneCommonIsExcluded(SSceneID idSScene, const SceneDefinition * ptDefScene);  // X/Common/SceneCommon.cpp:2784
void SComplexEffectACDsExpand(ACDIDListPooled & listACDs, SWorldID idSWorld);  // X/Server/SComplexEffect.cpp
static void sAddACDToKnownList(ActorCommonData & tACD, KnownListID idKnownList, const Player & tPlayer);

enum KnownSceneAdd {
    KSA_USE_KNOWN_REGION = 0,
    KSA_USE_ACD_LOCATION_SCENE = 1,
    KSA_NUM_TYPES = 2,
};




static const int32 KNOWN_ENTRY_POOL_SIZE = 512;






void KnownListInitializeForGame()
{

    SGame & tSGame = SGameManager::GameGet();
    KnownEntryGlobals *& ptKnownEntryGlobals = tSGame.ptKnownEntryGlobals;





    ptKnownEntryGlobals = (KnownEntryGlobals *)SigmaMemoryNew(sizeof(KnownEntryGlobals), 4, tSGame.ptMemoryInterface, TRUE);

    ptKnownEntryGlobals->ptKnownListArray = TDataArrayCreateTracked<KnownListWrapper, int>("KnownList", tSGame.ptMemoryInterface, 13);
    ptKnownEntryGlobals->ptKnownEntryListArray = TDataArrayCreateTracked<KnownEntryListWrapper, int>("KnownEntryList", tSGame.ptMemoryInterface, 6);

    ptKnownEntryGlobals->tKnownListAllocator.Initialize();
    ptKnownEntryGlobals->tKnownListBitArrayAllocator.Initialize();


    ptKnownEntryGlobals->tKnownEntryPool.Initialize();
    ptKnownEntryGlobals->tKnownEntryPool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptKnownEntryGlobals->tKnownEntryPool.SetBlockCount_Tracked(KNOWN_ENTRY_POOL_SIZE);


    for (int32 i = 0; i < KNOWNENTRY_MAP_TYPE_COUNT; i++)
    {
        ptKnownEntryGlobals->tKnownEntryMap[i].Initialize(16, FALSE, tSGame.ptMemoryInterface);
        ptKnownEntryGlobals->tKnownEntryMap[i].SetHashTableSize_Tracked(KNOWN_ENTRY_POOL_SIZE - 1);
    }


    ptKnownEntryGlobals->tAddedList.Initialize(&ptKnownEntryGlobals->tKnownEntryPool);
    ptKnownEntryGlobals->tChangedList.Initialize(&ptKnownEntryGlobals->tKnownEntryPool);
    ptKnownEntryGlobals->tDeletedList.Initialize(&ptKnownEntryGlobals->tKnownEntryPool);

    SigmaMemoryFill(&ptKnownEntryGlobals->tDeletedBitArray, 0, sizeof(ptKnownEntryGlobals->tDeletedBitArray));
}
void KnownListDisposeForGame()
{

    SGame & tSGame = SGameManager::GameGet();
    KnownEntryGlobals *& ptKnownEntryGlobals = tSGame.ptKnownEntryGlobals;

    TDataArrayDispose(ptKnownEntryGlobals->ptKnownListArray);
    TDataArrayDispose(ptKnownEntryGlobals->ptKnownEntryListArray);

    ptKnownEntryGlobals->tKnownListAllocator.Dispose();
    ptKnownEntryGlobals->tKnownListBitArrayAllocator.Dispose();

    for (int32 i = 0; i < KNOWNENTRY_MAP_TYPE_COUNT; i++)
    {
        ptKnownEntryGlobals->tKnownEntryMap[i].Dispose();
    }


    ptKnownEntryGlobals->tDeletedList.Dispose();
    ptKnownEntryGlobals->tChangedList.Dispose();
    ptKnownEntryGlobals->tAddedList.Dispose();

    ptKnownEntryGlobals->tKnownEntryPool.Dispose();


    XFREE(ptKnownEntryGlobals, tSGame.ptMemoryInterface);
    ptKnownEntryGlobals = NULL;
}

KnownListID KnownListAlloc()
{
    SGame & tSGame = SGameManager::GameGet();
    KnownEntryGlobals * ptKnownEntryGlobals = tSGame.ptKnownEntryGlobals;

    KnownListID idKnownList = ptKnownEntryGlobals->ptKnownListArray->ElementNew();

    KnownListWrapper * ptKnownListWrapper = ptKnownEntryGlobals->ptKnownListArray->ElementGet(idKnownList);

    ptKnownListWrapper->ptKnownList = ptKnownEntryGlobals->tKnownListAllocator.Alloc_Tracked();
    ptKnownListWrapper->ptKnownListBitArrays = ptKnownEntryGlobals->tKnownListBitArrayAllocator.Alloc_Tracked();

    ptKnownListWrapper->ptKnownList->SetPool(KnownListEntryPoolGet());

    SigmaMemoryClear(ptKnownListWrapper->ptKnownListBitArrays, sizeof(KnownListBitArray));

    return idKnownList;
}






void KnownListFree(KnownListID idKnownList)
{

    SGame & tSGame = SGameManager::GameGet();
    KnownEntryGlobals * ptKnownEntryGlobals = tSGame.ptKnownEntryGlobals;

    KnownListWrapper * ptKnownListWrapper = ptKnownEntryGlobals->ptKnownListArray->ElementGet(idKnownList);

    ptKnownEntryGlobals->tKnownListAllocator.Free(ptKnownListWrapper->ptKnownList);
    ptKnownEntryGlobals->tKnownListBitArrayAllocator.Free(ptKnownListWrapper->ptKnownListBitArrays);

    ptKnownEntryGlobals->ptKnownListArray->ElementDeleteByID(idKnownList);
}



KnownEntryListID KnownEntryListAlloc()
{

    SGame & tSGame = SGameManager::GameGet();
    KnownEntryGlobals * ptKnownEntryGlobals = tSGame.ptKnownEntryGlobals;

    KnownEntryListID idKnownEntryList = ptKnownEntryGlobals->ptKnownEntryListArray->ElementNew();

    KnownEntryListWrapper * ptKnownListWrapper = ptKnownEntryGlobals->ptKnownEntryListArray->ElementGet(idKnownEntryList);

    ptKnownListWrapper->ptKnownList = ptKnownEntryGlobals->tKnownListAllocator.Alloc_Tracked();
    ptKnownListWrapper->ptKnownList->SetPool(KnownListEntryPoolGet());

    return idKnownEntryList;
}





void KnownEntryListFree(KnownEntryListID idKnownEntryList)
{

    SGame & tSGame = SGameManager::GameGet();
    KnownEntryGlobals * ptKnownEntryGlobals = tSGame.ptKnownEntryGlobals;

    KnownEntryListWrapper * ptKnownListWrapper = ptKnownEntryGlobals->ptKnownEntryListArray->ElementGet(idKnownEntryList);

    ptKnownEntryGlobals->tKnownListAllocator.Free(ptKnownListWrapper->ptKnownList);

    ptKnownEntryGlobals->ptKnownEntryListArray->ElementDeleteByID(idKnownEntryList);
}
















static KnownList & sKnownEntryListGet(KnownEntryListID idKnownEntryList)
{

    SGame & tSGame = SGameManager::GameGet();
    KnownEntryGlobals * ptKnownEntryGlobals = tSGame.ptKnownEntryGlobals;

    return *ptKnownEntryGlobals->ptKnownEntryListArray->ElementGet(idKnownEntryList)->ptKnownList;
}





static KnownList * sKnownEntryListTryToGet(KnownEntryListID idKnownEntryList)
{

    SGame & tSGame = SGameManager::GameGet();
    KnownEntryGlobals * ptKnownEntryGlobals = tSGame.ptKnownEntryGlobals;

    KnownEntryListWrapper * ptKnownListWrapper = ptKnownEntryGlobals->ptKnownEntryListArray->ElementTryToGet(idKnownEntryList);
    return ptKnownListWrapper != NULL ? ptKnownListWrapper->ptKnownList : NULL;
}




static KnownListBitArray & sKnownListGetBitArray(KnownListID idKnownList)
{

    SGame & tSGame = SGameManager::GameGet();
    KnownEntryGlobals * ptKnownEntryGlobals = tSGame.ptKnownEntryGlobals;

    KnownListWrapper * ptKnownListWrapper = ptKnownEntryGlobals->ptKnownListArray->ElementGet(idKnownList);

    return *ptKnownListWrapper->ptKnownListBitArrays;
}



static int32 sKnownListGetBitArraySize(
    KnownEntryType eType)
{


    static const int32 arSizes[KET_NUM_TYPES] =
    {
        0,                                          // KET_INVALID
        KNOWN_LIST_TEAMS_BIT_ARRAY_SIZE_DWORDS,     // KET_TEAM
        KNOWN_LIST_WORLD_BIT_ARRAY_SIZE_DWORDS,     // KET_WORLD_REVEAL
        KNOWN_LIST_WORLD_BIT_ARRAY_SIZE_DWORDS,     // KET_WORLD_ENTER
        KNOWN_LIST_SCENE_BIT_ARRAY_SIZE_DWORDS,     // KET_PREFETCH_SCENE
        KNOWN_LIST_ACD_BIT_ARRAY_SIZE_DWORDS,       // KET_PREFETCH_ACD
        KNOWN_LIST_SCENE_BIT_ARRAY_SIZE_DWORDS,     // KET_SCENE
        KNOWN_LIST_SCENE_BIT_ARRAY_SIZE_DWORDS,     // KET_SUBSCENE
        KNOWN_LIST_ACD_BIT_ARRAY_SIZE_DWORDS,       // KET_ACD
    };


    return arSizes[eType];
}



static BitArrayBlockType * sKnownListGetBitArrayBytes(KnownListBitArray & tKnownListBitArray, KnownEntryType eType)
{


    switch (eType)
    {
    case KET_ACD:               return tKnownListBitArray.arACD;
    case KET_SCENE:             return tKnownListBitArray.arScenes;
    case KET_SUBSCENE:          return tKnownListBitArray.arSubScenes;
    case KET_WORLD_ENTER:       return tKnownListBitArray.arWorldEnter;
    case KET_WORLD_REVEAL:      return tKnownListBitArray.arWorldReveal;
    case KET_TEAM:              return tKnownListBitArray.arTeams;
    case KET_PREFETCH_SCENE:    return tKnownListBitArray.arPrefetchScenes;
    case KET_PREFETCH_ACD:      return tKnownListBitArray.arFarACD;
    default:                    return NULL;
    }
}





static BitArrayBlockType * sKnownListGetBitArrayBytes(KnownListID idKnownList, KnownEntryType eType)
{

    KnownListBitArray & tKnownListBitArray = sKnownListGetBitArray(idKnownList);

    return sKnownListGetBitArrayBytes(tKnownListBitArray, eType);
}




static void sGetBitIndex(uint16 dwObjectIndex, int32 & nBlockIndex, int32 & nBitIndex)
{

    nBlockIndex = dwObjectIndex >> BIT_ARRAY_BLOCK_SHIFT;
    nBitIndex = dwObjectIndex & BIT_ARRAY_BLOCK_LOW_MASK;
}





static int32 sGetObjectIndex(int32 nBlockIndex, int32 nBitIndex)
{

    int32 nIndex = (nBlockIndex << BIT_ARRAY_BLOCK_SHIFT) | nBitIndex;
    return nIndex;
}





static void sKnownListAdd(KnownListID idKnownList,
                          KnownEntry & tKnownEntry)
{




    uint16 nIndex = (uint16)tKnownEntry.id;
    BitArrayBlockType * arBits = sKnownListGetBitArrayBytes(idKnownList, tKnownEntry.eType);

    int32 nBlockIndex, nBitIndex;

    sGetBitIndex(nIndex, nBlockIndex, nBitIndex);

    arBits[nBlockIndex] |= 1 << nBitIndex;

    if (tKnownEntry.eType == KET_WORLD_REVEAL)
    {
        // the DWARF records these three locals; whatever used ptSWorld was compiled away
        int32 nObjectIndex = nIndex;
        SGame & tSGame = SGameManager::GameGet();
        SWorld * ptSWorld = &tSGame.ptSWorlds->ptData[nObjectIndex];
    }
}











static BOOL sKnownListContains(KnownListID idKnownList,
                               KnownEntryType eType,
                               DataID idObject)
{


    uint16 nIndex = (uint16)idObject;
    BitArrayBlockType * arBits = sKnownListGetBitArrayBytes(idKnownList, eType);

    int32 nBlockIndex, nBitIndex;

    sGetBitIndex(nIndex, nBlockIndex, nBitIndex);

    BOOL bFound = (arBits[nBlockIndex] & (1 << nBitIndex)) != 0;
    return bFound;
}




















static DataID sKnownListGetObjectId(KnownEntryType eType, int32 nObjectIndex)
{

    switch (eType)
    {
    case KET_ACD:
    case KET_PREFETCH_ACD:
        {
            GameCommonData & tGCD = GameGetActiveGameCommonData();
            ActorCommonData * ptACD = tGCD.ptACDGlobals->ptACDs->ElementGetFromBaseIndex(nObjectIndex);
            return ptACD->id;
        }

    case KET_SCENE:
    case KET_SUBSCENE:
    case KET_PREFETCH_SCENE:
        {
            SGame & tSGame = SGameManager::GameGet();
            SScene * ptSScene = &tSGame.ptSScenes->ptData[nObjectIndex];
            return ptSScene->id;
        }

    case KET_WORLD_ENTER:
    case KET_WORLD_REVEAL:
        {
            SGame & tSGame = SGameManager::GameGet();
            SWorld * ptSWorld = &tSGame.ptSWorlds->ptData[nObjectIndex];
            return ptSWorld->id;
        }

    case KET_TEAM:
        return nObjectIndex;

    default:
        return DATAID_INVALID;
    }
}















static void sKnownListClear(KnownListID idKnownList, BOOL bClearAcdKnownByPlayer, const Player * ptPlayer)
{





    if (bClearAcdKnownByPlayer)
    {
        BitArrayBlockType * arBits = sKnownListGetBitArrayBytes(idKnownList, KET_ACD);
        BitArrayBlockType * pCurBlock = arBits;

        for (int32 nBlockIndex = 0; nBlockIndex < KNOWN_LIST_ACD_BIT_ARRAY_SIZE_DWORDS; nBlockIndex++)
        {
            BitArrayBlockType dwMask = 1;
            BitArrayBlockType dwCur = *pCurBlock;

            for (int32 nBitIndex = 0; nBitIndex < BIT_ARRAY_BLOCK_TYPE_BITS; nBitIndex++)
            {
                if (dwCur & dwMask)
                {
                    int32 nIndex = sGetObjectIndex(nBlockIndex, nBitIndex);

                    ACDID idACD = (ACDID)sKnownListGetObjectId(KET_ACD, nIndex);
                    ActorCommonData * ptACD = idACD != ACDID_INVALID ? ACDTryToGet(idACD) : NULL;

                    if (ptACD != NULL)
                    {
                        ptACD->SetKnownByPlayer(*ptPlayer, FALSE);
                    }
                }

                dwMask <<= 1;
            }

            pCurBlock++;
        }
    }

    KnownListBitArray & tKnownListBitArray = sKnownListGetBitArray(idKnownList);

    SigmaMemoryClear(&tKnownListBitArray, sizeof(tKnownListBitArray));
}





















static KnownEntryType sGetPrefetchType(KnownEntryType eType)
{

    if (eType == KET_ACD)
    {
        return KET_PREFETCH_ACD;
    }
    if (eType == KET_SCENE || eType == KET_SUBSCENE)
    {
        return KET_PREFETCH_SCENE;
    }
    return KET_INVALID;
}




void sKnownListItemDeleted(KnownEntryType eType,
                           DataID idObject)
{


    SGame & tSGame = SGameManager::GameGet();
    KnownEntryGlobals * ptKnownEntryGlobals = tSGame.ptKnownEntryGlobals;

    KnownEntry tKnownEntry;
    tKnownEntry.eType = eType;
    tKnownEntry.id = idObject;


    POS posExistingChanged = ptKnownEntryGlobals->tChangedList.Find(tKnownEntry);
    if (posExistingChanged != NULL)
    {


        ptKnownEntryGlobals->tChangedList.RemoveAt(posExistingChanged);
    }

    POS posExistingAdd = ptKnownEntryGlobals->tAddedList.Find(tKnownEntry);
    if (posExistingAdd != NULL)
    {



        ptKnownEntryGlobals->tAddedList.RemoveAt(posExistingAdd);
    }
    else
    {
        ptKnownEntryGlobals->tDeletedList.AddTail_Tracked(tKnownEntry);

        uint16 nIndex = (uint16)idObject;
        int32 nBlockIndex, nBitIndex;

        sGetBitIndex(nIndex, nBlockIndex, nBitIndex);

        BitArrayBlockType * arBits = sKnownListGetBitArrayBytes(ptKnownEntryGlobals->tDeletedBitArray, eType);
        arBits[nBlockIndex] |= 1 << nBitIndex;


        eType = sGetPrefetchType(eType);
        if (eType != KET_INVALID)
        {
            arBits = sKnownListGetBitArrayBytes(ptKnownEntryGlobals->tDeletedBitArray, eType);
            arBits[nBlockIndex] |= 1 << nBitIndex;
        }
    }
}







void KnownListItemAdded(KnownEntryType eType, DataID idObject)
{

    SGame & tSGame = SGameManager::GameGet();
    KnownEntryGlobals * ptKnownEntryGlobals = tSGame.ptKnownEntryGlobals;

    if (ptKnownEntryGlobals != NULL)
    {
        KnownEntry tKnownEntry;
        tKnownEntry.eType = eType;
        tKnownEntry.id = idObject;

        ptKnownEntryGlobals->tAddedList.AddTail_Tracked(tKnownEntry);
    }
}



























































































































































static void sRecursiveAddACDToAddedList(KnownEntry & tKnownEntry,
                                        KnownList & tListAdded,
                                        BitArrayBlockType * arBitsOld,
                                        BitArrayBlockType * arBitsNew,
                                        BitArrayBlockType * arBitsDel,
                                        BitArrayBlockType * arBitsAdded)
{




    uint16 nIndex = (uint16)tKnownEntry.id;
    int32 nBlockIndex, nBitIndex;
    sGetBitIndex(nIndex, nBlockIndex, nBitIndex);
    if (arBitsAdded[nBlockIndex] & (1 << nBitIndex))
    {
        return;
    }




    ActorCommonData & tACD = ACDGet((ACDID)tKnownEntry.id);
    ActorLocationType eLocType = tACD.GetLocationType();

    if (eLocType == AL_INVENTORY)
    {

        InventoryLocation & tInvLoc = tACD.GetInventoryLocation();
        uint16 nIndexContainer = (uint16)tInvLoc.idACDContainer;
        int32 nContainerBlockIndex, nContainerBitIndex;
        sGetBitIndex(nIndexContainer, nContainerBlockIndex, nContainerBitIndex);

        BitArrayBlockType dwBitsOld = arBitsOld[nContainerBlockIndex] & ~arBitsDel[nContainerBlockIndex];
        BitArrayBlockType dwBitsNew = arBitsNew[nContainerBlockIndex];
        uint32 dwMask = 1 << nContainerBitIndex;
        BitArrayBlockType dwBitOld = dwBitsOld & dwMask;
        BitArrayBlockType dwBitNew = dwBitsNew & dwMask;
        if (!dwBitOld && dwBitNew)
        {

            KnownEntry tContainerKnownEntry;
            tContainerKnownEntry.eType = tKnownEntry.eType;
            tContainerKnownEntry.id = tInvLoc.idACDContainer;
            sRecursiveAddACDToAddedList(tContainerKnownEntry, tListAdded, arBitsOld, arBitsNew, arBitsDel, arBitsAdded);
        }
    }


    tListAdded.AddTail_Tracked(tKnownEntry);
    arBitsAdded[nBlockIndex] |= 1 << nBitIndex;
}




static void sKnownListBuildChangedLists(KnownListID idListOld,
                                        KnownListID idListNew,
                                        KnownEntryListID idListAdded,
                                        KnownEntryListID idListRemoved,
                                        KnownEntryListID idListIntersected)
{

    KnownEntryGlobals & tKnownEntryGlobals = *SGameManager::GameGet().ptKnownEntryGlobals;
    BitArrayBlockType * patACDAddedBits = tKnownEntryGlobals.arACDAddedBits;
    SigmaMemoryClear(patACDAddedBits, sizeof(tKnownEntryGlobals.arACDAddedBits));



    KnownList * ptListAdded = idListAdded != -1 ? &sKnownEntryListGet(idListAdded) : NULL;
    KnownList * ptListRemoved = idListRemoved != -1 ? &sKnownEntryListGet(idListRemoved) : NULL;
    KnownList * ptListIntersected = idListIntersected != -1 ? &sKnownEntryListGet(idListIntersected) : NULL;

    KnownEntry tKnownEntry;

    for (int32 i = 0; i < KET_NUM_TYPES; i++)
    {
        KnownEntryType eType = (KnownEntryType)i;

        tKnownEntry.eType = eType;

        int32 nObjectIndex = 0;
        int32 nCount = sKnownListGetBitArraySize(eType);
        BitArrayBlockType * arBitsOld = sKnownListGetBitArrayBytes(idListOld, eType);
        BitArrayBlockType * arBitsNew = sKnownListGetBitArrayBytes(idListNew, eType);
        BitArrayBlockType * arBitsDel = sKnownListGetBitArrayBytes(tKnownEntryGlobals.tDeletedBitArray, eType);

        for (int32 j = 0; j < nCount; j++)
        {
            BitArrayBlockType dwBitsOld = arBitsOld[j];
            BitArrayBlockType dwBitsDel = arBitsDel[j];
            BitArrayBlockType dwBitsNew = arBitsNew[j];

            if (dwBitsOld || dwBitsNew)
            {
                BitArrayBlockType dwMask = 1;
                for (int32 k = 0; k < BIT_ARRAY_BLOCK_TYPE_BITS; k++)
                {
                    BitArrayBlockType dwBitOld = dwBitsOld & dwMask;
                    BitArrayBlockType dwBitDel = dwBitsDel & dwMask;
                    BitArrayBlockType dwBitNew = dwBitsNew & dwMask;

                    if (dwBitOld)
                    {
                        if (dwBitNew)
                        {

                            if (dwBitDel)
                            {
                                if (ptListAdded)
                                {


                                    tKnownEntry.id = sKnownListGetObjectId(eType, nObjectIndex);





                                    if (eType == KET_ACD)
                                    {
                                        sRecursiveAddACDToAddedList(tKnownEntry, *ptListAdded, arBitsOld, arBitsNew, arBitsDel, patACDAddedBits);
                                    }
                                    else
                                    {
                                        ptListAdded->AddTail_Tracked(tKnownEntry);
                                    }
                                }
                            }
                            else
                            {

                                if (ptListIntersected)
                                {

                                    tKnownEntry.id = sKnownListGetObjectId(eType, nObjectIndex);




                                    ptListIntersected->AddTail_Tracked(tKnownEntry);
                                }
                            }
                        }
                        else
                        {

                            if (ptListRemoved && !dwBitDel)
                            {



                                tKnownEntry.id = sKnownListGetObjectId(eType, nObjectIndex);






                                ptListRemoved->AddTail_Tracked(tKnownEntry);
                            }
                        }
                    }
                    else
                    {
                        if (ptListAdded && dwBitNew)
                        {

                            tKnownEntry.id = sKnownListGetObjectId(eType, nObjectIndex);




                            if (eType == KET_ACD)
                            {
                                sRecursiveAddACDToAddedList(tKnownEntry, *ptListAdded, arBitsOld, arBitsNew, arBitsDel, patACDAddedBits);
                            }
                            else
                            {
                                ptListAdded->AddTail_Tracked(tKnownEntry);
                            }
                        }
                    }
                    nObjectIndex++;
                    dwMask <<= 1;
                }
            }
            else
            {
                nObjectIndex += BIT_ARRAY_BLOCK_TYPE_BITS;
            }
        }
    }
}

































































KnownEntryPool * KnownListEntryPoolGet()
{

    KnownEntryGlobals & tKnownEntryGlobals = *SGameManager::GameGet().ptKnownEntryGlobals;
    return &tKnownEntryGlobals.tKnownEntryPool;
}



void KnownListItemDeleted(KnownEntryType eType, DataID idObject)
{

    if (SGameManager::GameGet().ptKnownEntryGlobals != NULL)
    {
        sKnownListItemDeleted(eType, idObject);
    }
}






void KnownListItemChanged(KnownEntryType eType, DataID idObject, BOOL bCheckForDuplicate)
{

    SGame & tSGame = SGameManager::GameGet();
    KnownEntryGlobals * ptKnownEntryGlobals = tSGame.ptKnownEntryGlobals;

    if (ptKnownEntryGlobals != NULL)
    {
        KnownEntry tKnownEntry;
        tKnownEntry.eType = eType;
        tKnownEntry.id = idObject;

        if (bCheckForDuplicate)
        {
            if (ptKnownEntryGlobals->tChangedList.Find(tKnownEntry) != NULL)
            {
                return;
            }
        }

        ptKnownEntryGlobals->tChangedList.AddTail_Tracked(tKnownEntry);

        if (eType == KET_ACD && idObject != DATAID_INVALID)
        {
            ActorCommonData * ptACD = ACDTryToGet((ACDID)idObject);
            if (ptACD != NULL && ptACD->IsItem())
            {
                InventoryIterator tIt(ptACD);
                while (ACDInventoryIterate(tIt))
                {
                    KnownListItemChanged(KET_ACD, tIt.m_idItem, FALSE);
                }
            }
        }
    }
}







BOOL AddToKnownList(const KnownEntryType eType,
                    const DataID idObject,
                    KnownListID idKnownList,
                    const Player & tPlayerKnownListOwner,
                    BOOL bCheckDuplicates,
                    BOOL bKnowInventory)
{






    ActorCommonData * ptACDObject = NULL;
    if (eType == KET_ACD)
    {
        ptACDObject = &ACDGet((ACDID)idObject);
    }


































    if (bCheckDuplicates)
    {

        if (eType == KET_ACD)
        {
            if (ptACDObject->IsKnownByPlayer(tPlayerKnownListOwner))
            {
                return FALSE;
            }
        }
        else
        {

            if (sKnownListContains(idKnownList, eType, idObject))
            {
                return FALSE;
            }
        }
    }

    KnownEntry tKnownEntry;
    tKnownEntry.eType = eType;
    tKnownEntry.id = idObject;

    sKnownListAdd(idKnownList, tKnownEntry);


    if (eType == KET_ACD)
    {
        ptACDObject->SetTimeStamp(GameGetServerTime());


        if (bKnowInventory)
        {
            InventoryIterator tIt(ptACDObject);
            while (ACDInventoryIterate(tIt))
            {
                if (ACDTryToGet(tIt.m_idItem) != NULL)
                {
                    AddToKnownList(KET_ACD, tIt.m_idItem, idKnownList, tPlayerKnownListOwner, FALSE, TRUE);
                }
            }
        }
    }

    return TRUE;
}



static BOOL sSceneIsPrefetchSceneAroundPlayer(const SWorldID idSWorld,
                                              const int32 nPlayerIndex,
                                              const SceneCommonData & tSCD,
                                              BOOL bTreatAsOneScene)
{

    if (tSCD.idSWorld != idSWorld)
    {
        return FALSE;
    }



    if (tSCD.dwSceneFlags & (1 << SCENE_FLAGGED_FOR_DELETE))
    {
        return FALSE;
    }

    if (bTreatAsOneScene)
    {
        return TRUE;
    }
    return (tSCD.dwPrefetchScenePlayerSeenBits & (1 << nPlayerIndex)) != 0;
}





static BOOL sSceneIsSceneAroundPlayer(const SWorldID idSWorld,
                                      const int32 nPlayerIndex,
                                      const SceneCommonData & tSCD,
                                      BOOL bTreatAsOneScene)
{


    if (tSCD.idSWorld != idSWorld)
    {
        return FALSE;
    }
    if (bTreatAsOneScene)
    {
        return TRUE;
    }
    return (tSCD.dwScenePlayerSeenBits & (1 << nPlayerIndex)) != 0;
}







static void sAddSceneToKnownList(SceneCommonData & tSCD,
                                 KnownListID idKnownList,
                                 const Player & tPlayerKnownListOwner,
                                 BOOL bCheckDuplicates)
{
    if (tSCD.dwSceneFlags & (1 << SCENE_FLAGGED_FOR_DELETE))
    {
        return;
    }



    if (tSCD.idSSceneParent == SSCENEID_INVALID)
    {
        AddToKnownList(KET_SCENE, tSCD.idSScene, idKnownList, tPlayerKnownListOwner, bCheckDuplicates, FALSE);


        SScene * ptSScene = SSceneGet(tSCD.idSScene);

        WorldPlace tPlacePlayer = ACDGet(tPlayerKnownListOwner.idPlayerACD).GetPlace();
        if (tPlacePlayer.idSWorld == tSCD.idSWorld)
        {
            POS pos = ptSScene->listACDInScene.GetHeadPosition();
            while (pos)
            {
                ACDID idACDInScene = (ACDID)ptSScene->listACDInScene.GetNext(pos);
                const ActorCommonData * ptACD = ACDTryToGet(idACDInScene);
                if (ptACD &&
                    (ptACD->dwFlags & (1 << ACD_KNOWN_WITH_SCENE)) &&
                    !NPCShouldBeHiddenForClient(*ptACD, &tPlayerKnownListOwner))
                {
                    sAddACDToKnownList(ACDGet(idACDInScene), idKnownList, tPlayerKnownListOwner);
                }
            }
        }


        POS pos = tSCD.tChildSceneIDs.GetHeadPosition();
        while (pos)
        {
            SSceneID idSSceneChild = tSCD.tChildSceneIDs.GetNext(pos);
            SceneCommonData * ptSCDChild = SceneCommonDataTryToGet(idSSceneChild);
            if (ptSCDChild)
            {
                AddToKnownList(KET_SUBSCENE, ptSCDChild->idSScene, idKnownList, tPlayerKnownListOwner, bCheckDuplicates, FALSE);
            }
        }
    }
    else
    {

        SceneCommonData & tSCDParent = SceneCommonDataGet(tSCD.idSSceneParent);

        sAddSceneToKnownList(tSCDParent, idKnownList, tPlayerKnownListOwner, bCheckDuplicates);
    }
}












static void sAddScenesAroundPlayer(Player & tPlayerToAddAround,
                                   ActorCommonData & tACDPlayer,
                                   const KnownSceneAdd eAddType,
                                   KnownListID idKnownList,
                                   const Player & tPlayerKnownListOwner,
                                   BOOL bCheckDuplicates)
{




    if (eAddType == KSA_USE_ACD_LOCATION_SCENE)
    {
        NavMeshHandle tNavMeshHandle;
        ACDGetNavMeshHandle(tACDPlayer, tNavMeshHandle);
        if (!NavMeshHandleIsNone(tNavMeshHandle))
        {
            SceneCommonData & tSCD = SceneCommonDataGet(tNavMeshHandle.idSScene);
            sAddSceneToKnownList(tSCD, idKnownList, tPlayerKnownListOwner, TRUE);
        }
        return;
    }


    SWorldID idSWorld = tACDPlayer.GetSWorldID();
    BOOL bTreatAsOneScene = WorldTreatAsOneScene(idSWorld);

    SceneCommonData * ptSCD = NULL;
    SceneCommonDataIterator tIterator;

    SSceneID idSScenePlayer = ACDGetCurrentSSceneID(tACDPlayer);
    if (idSScenePlayer == SSCENEID_INVALID)
    {
        return;
    }

    SceneCommonData & tSCDPlayer = SceneCommonDataGet(idSScenePlayer);
    ScopedSceneDefinitionLockR ptDefScene(tSCDPlayer.snoScene);

    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        if (sSceneIsPrefetchSceneAroundPlayer(idSWorld, tPlayerToAddAround.nPlayerIndex, *ptSCD, bTreatAsOneScene) &&

            (!SceneCommonIsExcluded(ptSCD->idSScene, ptDefScene) || bTreatAsOneScene))
        {





            if (sSceneIsSceneAroundPlayer(idSWorld, tPlayerToAddAround.nPlayerIndex, *ptSCD, bTreatAsOneScene))
            {






                sAddSceneToKnownList(*ptSCD, idKnownList, tPlayerKnownListOwner, bCheckDuplicates);
            }


            AddToKnownList(KET_PREFETCH_SCENE, ptSCD->idSScene, idKnownList, tPlayerKnownListOwner, bCheckDuplicates, FALSE);
        }
    }




    for (int32 i = 0; i <= tPlayerToAddAround.nMaxUsedPetIndex; i++)
    {
        const ACDID idACDPet = tPlayerToAddAround.tPets[i].idACD;

        ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
        if (ptACDPet)
        {
            NavMeshHandle tNavMeshHandle;
            ACDGetNavMeshHandle(*ptACDPet, tNavMeshHandle);
            if (!NavMeshHandleIsNone(tNavMeshHandle))
            {
                SceneCommonData & tSCD = SceneCommonDataGet(tNavMeshHandle.idSScene);

                sAddSceneToKnownList(tSCD, idKnownList, tPlayerKnownListOwner, TRUE);
            }
        }
    }
}



static void sAddWorldsFromTrickleEntries(Player & tPlayer,
                                         KnownListID idKnownList)
{


    const ServerTrickleList & listTrickle = *SPlayerGetPossibleTrickleList(tPlayer);

    POS pos = listTrickle.GetHeadPosition();
    while (pos)
    {
        ACDID idACD = (ACDID)listTrickle.GetNext(pos);





        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (!ptACD || !ptACD->IsInWorld())
        {
            continue;
        }





        WorldPlace tPlace = ptACD->GetPlace();
        if (SWorldTryToGet(tPlace.idSWorld))
        {
            AddToKnownList(KET_WORLD_REVEAL, tPlace.idSWorld, idKnownList, tPlayer, TRUE, FALSE);
        }
    }
}



static void sAddTradeItemsToKnownList(const Player & tPlayer,
                                      KnownListID idKnownList,
                                      const Player * ptOther)
{
    if (ptOther == NULL)
    {
        return;
    }












    for (int32 i = 0; i < (int32)(sizeof(ptOther->tTradeData.arrItems) / sizeof(ptOther->tTradeData.arrItems[0])); i++)
    {
        ACDID idACDItem = ACDFromANN(ptOther->tTradeData.arrItems[i]);
        if (ACDTryToGet(idACDItem))
        {
            AddToKnownList(KET_ACD, idACDItem, idKnownList, tPlayer, FALSE, TRUE);
        }
    }
}



static void sAddSlotItemToKnownList(const Player & tPlayer,
                                    KnownListID idKnownList,
                                    ACDID idACDTarget,
                                    InventorySlotType eSlot)
{


    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    ACDID idACDItem = ACDInventoryGetSquareItem(tACDTarget, eSlot, cgptNull);
    if (idACDItem != ACDID_INVALID)
    {
        AddToKnownList(KET_ACD, idACDItem, idKnownList, tPlayer, FALSE, TRUE);
    }
}





static void sAddInspectItemsToKnownList(Player & tPlayer,
                                        KnownListID idKnownList,
                                        ACDID idACDPlayerToInspect)
{



    if (!SPlayerTestAllowedToInspect(tPlayer.idPlayerACD, idACDPlayerToInspect))
    {

        SPlayerInspectEnd(tPlayer.idPlayerACD);
        return;
    }


    for (int32 i = SLOT_PLAYER_BODYSLOT_START; i < SLOT_PLAYER_BODYSLOT_END; i++)
    {
        sAddSlotItemToKnownList(tPlayer, idKnownList, idACDPlayerToInspect, (InventorySlotType)i);
    }
}




static BOOL sShouldKnowInventory(const ActorCommonData & tACD,
                                 const Player & tPlayer)
{
    if (tACD.IsMerchant())
    {
        return TRUE;
    }












    if (tPlayer.bIsPrimaryPlayerServerOnly)
    {
        if (tACD.id == tPlayer.idPlayerACD)
        {
            return TRUE;
        }

        Player * ptOtherPlayer = PlayerGetByACD(tACD.id);
        if (ptOtherPlayer && PlayersAreLocalToEachOther(tPlayer, *ptOtherPlayer))
        {
            return TRUE;
        }
    }



    if (tACD.IsPet())
    {
        if (ACDInventoryGetType(tACD) != 0)
        {
            return TRUE;
        }
    }

    if (tACD.IsItem() &&
        tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_SOCKETS_FILLED)) > 0)
    {
        return TRUE;
    }

    return FALSE;
}



static void sAddACDToKnownList(ActorCommonData & tACD,
                               KnownListID idKnownList,
                               const Player & tPlayer)
{

    if (!tACD.DoesPlayerMatchAssignedHeroID(&tPlayer, TRUE))
    {
        return;
    }


    if (tACD.IsItem())
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (ptACDPlayer && !ActorInventoryAllowedToSeeItem(*ptACDPlayer, tACD))
        {
            return;
        }
    }
    else if (tACD.IsNPC())
    {
        Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(tACD.id));
        if (ptActor)
        {
            PlayerClass eKnownOnlyToClass = (PlayerClass)ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_KNOWN_ONLY_TO_CLASS, TAG_MARKER_KNOWN_ONLY_TO_CLASS);
            if (eKnownOnlyToClass != PLAYER_CLASS_NONE && eKnownOnlyToClass != PlayerGetPlayerClass(&tPlayer))
            {
                return;
            }

            ActorGender eGender = (ActorGender)ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_KNOWN_ONLY_TO_GENDER, TAG_MARKER_KNOWN_ONLY_TO_GENDER);
            if (eGender != ACTOR_GENDER_INVALID)
            {
                BOOL bIsMale = PlayerIsMale(tPlayer);
                if (eGender == ACTOR_GENDER_FEMALE && bIsMale)
                {
                    return;
                }
                if (eGender == ACTOR_GENDER_MALE && !bIsMale)
                {
                    return;
                }
            }
        }
    }

    BOOL bKnowInventory = sShouldKnowInventory(tACD, tPlayer);

    AddToKnownList(KET_ACD, tACD.id, idKnownList, tPlayer, FALSE, bKnowInventory);
}



void KnownListBuild(Player & tPlayer,
                    KnownListID idKnownList)
{




    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    WorldPlace tPlace = tACDPlayer.GetPlace();


    STeamAddTeamDataToKnownList(tPlayer, idKnownList);




    AddToKnownList(KET_WORLD_REVEAL, tPlace.idSWorld, idKnownList, tPlayer, FALSE, FALSE);


    AddToKnownList(KET_WORLD_ENTER, tPlace.idSWorld, idKnownList, tPlayer, FALSE, FALSE);




    sAddScenesAroundPlayer(tPlayer, tACDPlayer, KSA_USE_KNOWN_REGION, idKnownList, tPlayer, FALSE);




    sAddScenesAroundPlayer(tPlayer, tACDPlayer, KSA_USE_ACD_LOCATION_SCENE, idKnownList, tPlayer, TRUE);



    sAddWorldsFromTrickleEntries(tPlayer, idKnownList);


    ACDIDListPooled listACDsWithConnectedComplexEffects(MemoryPoolsGetDATAIDListPool());


    sAddACDToKnownList(tACDPlayer, idKnownList, tPlayer);



    if (tPlayer.bIsPrimaryPlayerServerOnly)
    {
        for (Player * ptOtherPlayer = PlayerGetFirstAll(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextAll(ptOtherPlayer))
        {
            if (!ptOtherPlayer->bIsPrimaryPlayerServerOnly &&

                PlayersAreLocalToEachOther(tPlayer, *ptOtherPlayer))
            {
                ActorCommonData * ptACDOther = ACDTryToGet(ptOtherPlayer->idPlayerACD);
                if (ptACDOther)
                {
                    sAddACDToKnownList(*ptACDOther, idKnownList, tPlayer);
                }
            }


            if ((tPlayer.dwBossEncounterFlags & (1 << ESF_ACTIVE)) &&
                (ptOtherPlayer->dwBossEncounterFlags & (1 << ESF_ACTIVE)))
            {
                ActorCommonData * ptACDOther = ACDTryToGet(ptOtherPlayer->idPlayerACD);
                if (ptACDOther)
                {
                    WorldPlace tPlacePlayer = tACDPlayer.GetPlace();
                    WorldPlace tPlaceOther = ptACDOther->GetPlace();
                    if (WorldPlaceInSameWorld(tPlacePlayer, tPlaceOther))
                    {
                        sAddACDToKnownList(*ptACDOther, idKnownList, tPlayer);
                    }
                }
            }
        }
    }




    {
        POS pos = tPlayer.listVisibleACDs.GetHeadPosition();
        while (pos)
        {
            ACDID idACD = (ACDID)tPlayer.listVisibleACDs.GetNext(pos);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (!ptACD)
            {
                continue;
            }




            if (idACD == tPlayer.idPlayerACD)
            {
                if (ptACD->HasComplexEffects())
                {
                    listACDsWithConnectedComplexEffects.AddTail_Tracked(idACD);
                }
                continue;
            }


            if (NPCShouldBeHiddenForClient(*ptACD, &tPlayer))
            {
                continue;
            }



            BOOL bIsAlivePet = ptACD->IsMyPet(tPlayer.nPlayerIndex) &&
                (ptACD->IsAlive() || !ptACD->HasResurrectionPower());
            if (bIsAlivePet)
            {
                continue;
            }

            if (ptACD->HasComplexEffects() == TRUE)
            {
                listACDsWithConnectedComplexEffects.AddTail_Tracked(idACD);
            }
            else
            {
                sAddACDToKnownList(*ptACD, idKnownList, tPlayer);
            }
        }
    }



    if (TradeIsTrading(tPlayer.idPlayerACD))
    {
        sAddTradeItemsToKnownList(tPlayer,
                                  idKnownList,

                                  TradeGetTradingTarget(tPlayer));
    }




    else if (tPlayer.bIsPrimaryPlayerServerOnly)
    {
        for (Player * ptOtherPlayer = PlayerGetFirstAll(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextAll(ptOtherPlayer))
        {
            if (!ptOtherPlayer->bIsPrimaryPlayerServerOnly &&

                TradeIsTrading(ptOtherPlayer->idPlayerACD))
            {
                sAddTradeItemsToKnownList(tPlayer,
                                          idKnownList,

                                          TradeGetTradingTarget(*ptOtherPlayer));
            }
        }
    }







    if (tPlayer.idACDPlayerToInspect != ACDID_INVALID)
    {
        ActorCommonData * ptACDPlayerToInspect = ACDTryToGet(tPlayer.idACDPlayerToInspect);
        if (ptACDPlayerToInspect && ptACDPlayerToInspect->IsKnownByPlayer(tPlayer))
        {
            sAddInspectItemsToKnownList(tPlayer, idKnownList, tPlayer.idACDPlayerToInspect);
        }
    }





    for (int32 i = 0; i <= tPlayer.nMaxUsedPetIndex; i++)
    {
        const ACDID idACDPet = tPlayer.tPets[i].idACD;

        ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
        if (ptACDPet)
        {

            WorldPlace tPlacePet = ptACDPet->GetPlace();
            if (tPlacePet.idSWorld == tPlace.idSWorld)
            {





                if (ptACDPet->HasComplexEffects() == TRUE)
                {
                    listACDsWithConnectedComplexEffects.AddTail_Tracked(idACDPet);
                }
                else
                {
                    sAddACDToKnownList(*ptACDPet, idKnownList, tPlayer);
                }
            }
        }
    }







    SComplexEffectACDsExpand(listACDsWithConnectedComplexEffects, tACDPlayer.GetPlace().idSWorld);



    {
        POS posACD = listACDsWithConnectedComplexEffects.GetHeadPosition();
        while (posACD)
        {
            const ACDID idACD = (ACDID)listACDsWithConnectedComplexEffects.GetNext(posACD);




            if (idACD != tPlayer.idPlayerACD)
            {
                ActorCommonData & tACD = ACDGet(idACD);
                sAddACDToKnownList(tACD, idKnownList, tPlayer);



                NavMeshHandle tNavMeshACD;
                ACDGetNavMeshHandle(tACD, tNavMeshACD);
                if (tNavMeshACD.idSScene != SSCENEID_INVALID)
                {
                    SceneCommonData & tSCD = SceneCommonDataGet(tNavMeshACD.idSScene);

                    sAddSceneToKnownList(tSCD, idKnownList, tPlayer, TRUE);
                }
            }
        }
    }



    {
        POS pos = tPlayer.listPrefetchableACDs.GetHeadPosition();
        while (pos)
        {
            ACDID idACD = (ACDID)tPlayer.listPrefetchableACDs.GetNext(pos);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD && ptACD->snoActor != SNO_NONE)
            {
                AddToKnownList(KET_PREFETCH_ACD, idACD, idKnownList, tPlayer, TRUE, FALSE);
            }
        }
    }


    if (tPlayer.bIsPrimaryPlayerServerOnly)
    {
        for (Player * ptOtherPlayer = PlayerGetFirstAll(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextAll(ptOtherPlayer))
        {
            if (PlayersAreLocalToEachOther(tPlayer, *ptOtherPlayer))
            {

                for (int32 nHireling = 0; nHireling < NUM_HIRELING_CLASSES; nHireling++)
                {
                    HirelingItemStore * ptItems = &ptOtherPlayer->tHirelingStable.tHireling[nHireling].tItems;


                    for (int32 nSlot = 0; nSlot < NUM_HIRELING_SLOTS; nSlot++)
                    {
                        ACDID idACDItem = ptItems->idACDItems[nSlot];
                        if (idACDItem != ACDID_INVALID)
                        {
                            ActorCommonData & tACDItem = ACDGet(idACDItem);
                            sAddACDToKnownList(tACDItem, idKnownList, tPlayer);
                        }
                    }
                }
            }
        }
    }
}





void KnownListBuildChangedLists(KnownListID idListOld,
                                KnownListID idListNew,
                                KnownEntryListID idListAdded,
                                KnownEntryListID idListRemoved,
                                KnownEntryListID idListIntersected)
{


    sKnownListBuildChangedLists(idListOld, idListNew, idListAdded, idListRemoved, idListIntersected);
}




void KnownListResetDeletedItems()
{

    KnownEntryGlobals & tKnownEntryGlobals = *SGameManager::GameGet().ptKnownEntryGlobals;

    SigmaMemoryFill(&tKnownEntryGlobals.tDeletedBitArray, 0, sizeof(tKnownEntryGlobals.tDeletedBitArray));

    tKnownEntryGlobals.tDeletedList.RemoveAll();
}






void KnownListResetAddedItems()
{
    KnownEntryGlobals & tKnownEntryGlobals = *SGameManager::GameGet().ptKnownEntryGlobals;
    tKnownEntryGlobals.tAddedList.RemoveAll();
}





void KnownListResetChangedItems()
{
    KnownEntryGlobals & tKnownEntryGlobals = *SGameManager::GameGet().ptKnownEntryGlobals;
    tKnownEntryGlobals.tChangedList.RemoveAll();
}





void KnownListClear(KnownListID idKnownList, BOOL bClearAcdKnownByPlayer, const Player * ptPlayer)
{

    sKnownListClear(idKnownList, bClearAcdKnownByPlayer, ptPlayer);
}






















POS KnownEntryListGetFirst(KnownEntryListID idKnownEntryList)
{

    KnownList & tKnownList = sKnownEntryListGet(idKnownEntryList);

    return tKnownList.GetHeadPosition();
}



KnownEntry & KnownEntryListGetNext(KnownEntryListID idKnownEntryList, POS & pos)
{


    KnownList & tKnownList = sKnownEntryListGet(idKnownEntryList);

    return tKnownList.GetNext(pos);
}

BOOL SceneIntersectsFrustum(SceneCommonData & tSCD, const Frustum & tFrustum);  // X/Common/SceneCommon.cpp:2280
BOOL SceneCommonIsExcluded(SSceneID idSScene, const SceneDefinition * ptDefScene);  // X/Common/SceneCommon.cpp:2784
static BOOL sCanSeeACD(const ActorCommonData & tACDPlayer, const Frustum & tFrustum, ACDID idACD, BOOL bCheckForComplexEffects);
void SceneCreatePlayerFrustum(const Player * ptPlayer, Frustum & tFrustum, Vector3D & vCenterPos);  // X/Common/SceneCommon.cpp
void SceneCreatePlayerPrefetchFrustum(Player * ptPlayer, Frustum & tFrustum);  // X/Common/SceneCommon.cpp
KnownListID PlayerKnownListGetCurrent(const Player & tPlayer);  // X/Server/SPlayer.cpp
BOOL NPCShouldBeHiddenForClient(const ActorCommonData & tACD, const Player * ptPlayer);  // X/Server/SNPC.cpp
BOOL ActorInventoryAllowedToSeeItem(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem);  // X/Common/ActorInventory.cpp
void SComplexEffectACDsExpand(ACDIDListPooled & listACDs, SWorldID idSWorld);  // X/Server/SComplexEffect.cpp











































































































































































































































static BOOL sCanSeeScene(const ActorCommonData & tACDPlayer, const Frustum & tFrustum, SSceneID idSScene)
{



    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
    if (ptSCD)
    {

        SWorldID idSWorld = tACDPlayer.GetSWorldID();
        if (idSWorld == SWORLDID_INVALID || idSWorld != ptSCD->idSWorld)
        {
            return FALSE;
        }


        if (ptSCD->idSSceneParent != SSCENEID_INVALID)
        {

            return sCanSeeScene(tACDPlayer, tFrustum, ptSCD->idSSceneParent);
        }

        SSceneID idSScenePlayer = ACDGetCurrentSSceneID(tACDPlayer);
        if (idSScenePlayer == SSCENEID_INVALID)
        {
            return FALSE;
        }

        SceneCommonData & tSCDPlayer = SceneCommonDataGet(idSScenePlayer);
        ScopedSceneDefinitionLockR ptDefScene(tSCDPlayer.snoScene);

        if (!SceneCommonIsExcluded(ptSCD->idSScene, ptDefScene))
        {
            return SceneIntersectsFrustum(*ptSCD, tFrustum);
        }
    }


    return FALSE;
}

static BOOL sCanSeeACD(const ActorCommonData & tACDPlayer, const Frustum & tFrustum, ACDID idACD, BOOL bCheckForComplexEffects)
{




    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return FALSE;
    }


    if (tACDPlayer.id == idACD)
    {
        return TRUE;
    }


    Player * ptPlayerOwner = ptACD->GetPetOwnerPlayer();
    if (ptPlayerOwner && ptPlayerOwner->idPlayerACD == tACDPlayer.id)
    {
        return TRUE;
    }

    if (ptACD->GetInventoryContainerOwner() == tACDPlayer.id)
    {
        return TRUE;
    }

    const Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);


    if (ptPlayer)
    {
        const Player * ptOtherPlayer = PlayerGetByACD(ptACD->id);
        if (ptOtherPlayer && PlayersAreLocalToEachOther(*ptPlayer, *ptOtherPlayer))
        {
            return TRUE;
        }

        if (ptACD->GetLocationType() == AL_INVENTORY)
        {
            const Player * ptOwner = PlayerGetByACD(ptACD->GetInventoryContainerOwner());
            if (ptOwner && PlayersAreLocalToEachOther(*ptPlayer, *ptOwner))
            {
                return TRUE;
            }
        }
    }


    if (NPCShouldBeHiddenForClient(*ptACD, ptPlayer))
    {
        return FALSE;
    }


    if (ptACD->GetLocationType() == AL_NONE && ptPlayer)
    {

        if (ptPlayer->bIsPrimaryPlayerServerOnly)
        {
            for (Player * ptOtherPlayer = PlayerGetFirstAll(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextAll(ptOtherPlayer))
            {
                if (ptPlayer == ptOtherPlayer ||
                    PlayersAreLocalToEachOther(*ptPlayer, *ptOtherPlayer))
                {
                    for (int32 nHireling = 0; nHireling < NUM_HIRELING_CLASSES; nHireling++)
                    {
                        const HirelingItemStore * ptItems = &ptOtherPlayer->tHirelingStable.tHireling[nHireling].tItems;
                        for (int32 i = 0; i < NUM_HIRELING_SLOTS; i++)
                        {
                            if (ptACD->id == ptItems->idACDItems[i])
                            {
                                return TRUE;
                            }
                        }
                    }
                }
            }
        }
    }

    if (ptACD->GetLocationType() != AL_WORLD)
    {
        return FALSE;
    }

    if (ptACD->GetSWorldID() != tACDPlayer.GetSWorldID())
    {
        return FALSE;
    }



    if (!ptACD->DoesPlayerMatchAssignedHeroID(ptPlayer, TRUE))
    {
        return FALSE;
    }

    if (ptACD->IsItem())
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(tACDPlayer.id);
        if (ptACDPlayer && !ActorInventoryAllowedToSeeItem(tACDPlayer, *ptACD))
        {
            return FALSE;
        }
    }

    WorldLocation & loc = ptACD->GetWorldLocation();
    if (loc.ptSphereNode)
    {

        Sphere ws;
        loc.ptSphereNode->GetSphere(ws);
        if (Intersect::FrustumAndSphere(tFrustum, ws))
        {
            return TRUE;
        }
    }

    if (bCheckForComplexEffects && ptACD->HasComplexEffects())
    {



        ACDIDListPooled listACDsWithConnectedComplexEffects(MemoryPoolsGetDATAIDListPool());
        listACDsWithConnectedComplexEffects.AddTail_Tracked(ptACD->id);
        SComplexEffectACDsExpand(listACDsWithConnectedComplexEffects, ptACD->GetSWorldID());

        POS pos = listACDsWithConnectedComplexEffects.GetHeadPosition();
        while (pos)
        {
            const ACDID idACDConnected = (ACDID)listACDsWithConnectedComplexEffects.GetNext(pos);
            if (idACDConnected != ptACD->id)
            {
                BOOL bCanSee = sCanSeeACD(tACDPlayer, tFrustum, idACDConnected, FALSE);
                if (bCanSee)
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}






static BOOL sCanSeeEntry(const ActorCommonData & tACDPlayer, const Frustum & tFrustum, const KnownEntry & tKnownEntry)
{



    WorldPlace placePlayer = tACDPlayer.GetPlace();
    switch (tKnownEntry.eType)
    {
    case KET_WORLD_REVEAL:
    case KET_WORLD_ENTER:
        return placePlayer.idSWorld == tKnownEntry.id;

    case KET_SCENE:
    case KET_SUBSCENE:
        return sCanSeeScene(tACDPlayer, tFrustum, tKnownEntry.id);

    case KET_ACD:
        return sCanSeeACD(tACDPlayer, tFrustum, (ACDID)tKnownEntry.id, TRUE);

    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        return FALSE;
    }
}






















































































































































































































static void sDeleteKnownEntry(KnownEntry & tEntry, KnownList * ptDeleteList, KnownListID idKnownList)
{




    BitArrayBlockType * arBitsCurr = sKnownListGetBitArrayBytes(idKnownList, tEntry.eType);
    int32 nBlockIndex;
    int32 nBitIndex;
    sGetBitIndex(tEntry.id, nBlockIndex, nBitIndex);

    BitArrayBlockType dwMask = 1 << nBitIndex;
    if (arBitsCurr[nBlockIndex] & dwMask)
    {











        arBitsCurr[nBlockIndex] &= ~dwMask;
        if (ptDeleteList)
        {
            ptDeleteList->AddTail_Tracked(tEntry);
        }
    }
}


static void sDeleteEntryByType(KnownEntry & tKnownEntry, KnownList * ptListRemoved, KnownListID idKnownListCurrent)
{



    switch (tKnownEntry.eType)
    {
    case KET_WORLD_REVEAL:
        sDeleteKnownEntry(tKnownEntry, ptListRemoved, idKnownListCurrent);
        {
            KnownEntry tWorldEnter;
            tWorldEnter.eType = KET_WORLD_ENTER; tWorldEnter.id = tKnownEntry.id;
            sDeleteKnownEntry(tWorldEnter, ptListRemoved, idKnownListCurrent);
        }
        break;
    case KET_SCENE: case KET_SUBSCENE:
        sDeleteKnownEntry(tKnownEntry, ptListRemoved, idKnownListCurrent);
        break;
    case KET_PREFETCH_SCENE:
        sDeleteKnownEntry(tKnownEntry, ptListRemoved, idKnownListCurrent);
        break;
    case KET_PREFETCH_ACD:
        sDeleteKnownEntry(tKnownEntry, ptListRemoved, idKnownListCurrent);
        break;
    case KET_ACD:
        sDeleteKnownEntry(tKnownEntry, ptListRemoved, idKnownListCurrent);
        break;
    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    }
}



void KnownListProcessDeletedEntries(Player * ptPlayer, KnownEntryListID idList)
{


    KnownEntryGlobals & tKnownEntryGlobals = *SGameManager::GameGet().ptKnownEntryGlobals;

    KnownList * ptListRemoved = sKnownEntryListTryToGet(idList);

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer || !PlayerIsInWorld(*ptPlayer))
    {
        return;
    }

    KnownListID idKnownListCurrent = PlayerKnownListGetCurrent(*ptPlayer);
    POS pos = tKnownEntryGlobals.tDeletedList.GetHeadPosition();
    while (pos)
    {
        KnownEntry & tKnownEntry = tKnownEntryGlobals.tDeletedList.GetNext(pos);

        KnownEntry tPrefetchEntry = tKnownEntry;
        tPrefetchEntry.eType = sGetPrefetchType(tKnownEntry.eType);
        if (tPrefetchEntry.eType != KET_INVALID)
        {
            sDeleteEntryByType(tPrefetchEntry, ptListRemoved, idKnownListCurrent);
        }

        sDeleteEntryByType(tKnownEntry, ptListRemoved, idKnownListCurrent);
    }

    Frustum tFrustum;
    Frustum tPrefetchFrustum;
    Vector3D vCenterPos;
    SceneCreatePlayerFrustum(ptPlayer, tFrustum, vCenterPos);
    SceneCreatePlayerPrefetchFrustum(ptPlayer, tPrefetchFrustum);
    pos = tKnownEntryGlobals.tChangedList.GetHeadPosition();
    while (pos)
    {
        KnownEntry & tKnownEntry = tKnownEntryGlobals.tChangedList.GetNext(pos);

        KnownEntry tPrefetchEntry = tKnownEntry;
        tPrefetchEntry.eType = sGetPrefetchType(tKnownEntry.eType);
        if (tPrefetchEntry.eType != KET_INVALID && !sCanSeeEntry(*ptACDPlayer, tPrefetchFrustum, tKnownEntry))
        {
            sDeleteEntryByType(tPrefetchEntry, ptListRemoved, idKnownListCurrent);
        }

        if (!sCanSeeEntry(*ptACDPlayer, tFrustum, tKnownEntry))
        {
            sDeleteEntryByType(tKnownEntry, ptListRemoved, idKnownListCurrent);
        }
    }
}



void KnownListForceUpdateForAllPlayers()
{
    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer != NULL; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        KnownListForceUpdate(*ptPlayer);
    }
}




void KnownListForceUpdate(Player & tPlayer)
{
    tPlayer.bForceKnownListUpdate = TRUE;
}

#include "../../Common/Movement/ActorMovement.h"
void SComplexEffectACDsExpand(ACDIDListPooled & listACDsWithConnectedComplexEffects, SWorldID idSWorld);
BOOL SPlayerIsWaitingForSavePointPort(const Player & tPlayer);

static void sAddKnownEntry(KnownEntry & tEntry, KnownList * ptAddList, KnownListID idKnownListCurrent)
{
    BitArrayBlockType * arBitsCurr = sKnownListGetBitArrayBytes(idKnownListCurrent, tEntry.eType);
    int32 nBlockIndex;
    int32 nBitIndex;
    sGetBitIndex((uint16)tEntry.id, nBlockIndex, nBitIndex);
    BitArrayBlockType dwMask = 1 << nBitIndex;
    if (!(arBitsCurr[nBlockIndex] & dwMask))
    {
        arBitsCurr[nBlockIndex] |= dwMask;
        if (ptAddList)
            ptAddList->AddTail_Tracked(tEntry);
    }
}

static void sAddKnownPrefetchScene(KnownEntry & tEntry, KnownList * ptAddList, KnownListID idKnownListCurrent)
{
    sAddKnownEntry(tEntry, ptAddList, idKnownListCurrent);
    SSceneID idSScene = tEntry.id;
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
    if (ptSCD)
    {
        POS pos = ptSCD->tChildSceneIDs.GetHeadPosition();
        while (pos)
        {
            SSceneID idSSceneChild = ptSCD->tChildSceneIDs.GetNext(pos);
            SceneCommonData * ptSCDChild = SceneCommonDataTryToGet(idSSceneChild);
            if (ptSCDChild)
            {
                KnownEntry tChildScene;
                tChildScene.eType = KET_PREFETCH_SCENE;
                tChildScene.id = idSSceneChild;
                sAddKnownEntry(tChildScene, ptAddList, idKnownListCurrent);
            }
        }
    }
}

static void sAddKnownScene(KnownEntry & tKnownEntry, KnownList * ptListAddedScenes, KnownListID idKnownListCurrent)
{
    sAddKnownEntry(tKnownEntry, ptListAddedScenes, idKnownListCurrent);
    SSceneID idSScene = tKnownEntry.id;
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
    if (ptSCD)
    {
        POS pos = ptSCD->tChildSceneIDs.GetHeadPosition();
        while (pos)
        {
            SSceneID idSSceneChild = ptSCD->tChildSceneIDs.GetNext(pos);
            SceneCommonData * ptSCDChild = SceneCommonDataTryToGet(idSSceneChild);
            if (ptSCDChild)
            {
                KnownEntry tChildScene;
                tChildScene.eType = tKnownEntry.eType == KET_PREFETCH_SCENE ? KET_PREFETCH_SCENE : KET_SUBSCENE;
                tChildScene.id = idSSceneChild;
                sAddKnownEntry(tChildScene, ptListAddedScenes, idKnownListCurrent);
            }
        }
    }
}

static void sAddKnownACD(KnownEntry & tEntry, KnownList * ptAddList, KnownListID idKnownListCurrent, BOOL bCheckForComplexEffects)
{
    ACDID idACD = (ACDID)tEntry.id;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        ActorLocationType eLocType = ptACD->GetLocationType();
        if (eLocType == AL_INVENTORY)
        {
            InventoryLocation & tInvLoc = ptACD->GetInventoryLocation();
            if (tInvLoc.idACDContainer != ACDID_INVALID)
            {
                KnownEntry tContainerEntry;
                tContainerEntry.eType = tEntry.eType;
                tContainerEntry.id = tInvLoc.idACDContainer;
                sAddKnownACD(tContainerEntry, ptAddList, idKnownListCurrent, TRUE);
            }
        }
        ActorMovement * ptActorMovement = ptACD->GetActorMovement();
        if (ptActorMovement)
            ptActorMovement->ResetCache();
        if (bCheckForComplexEffects && ptACD->HasComplexEffects())
        {
            ACDIDListPooled listACDsWithConnectedComplexEffects(MemoryPoolsGetDATAIDListPool());
            listACDsWithConnectedComplexEffects.AddTail_Tracked(ptACD->id);
            SComplexEffectACDsExpand(listACDsWithConnectedComplexEffects, ptACD->GetSWorldID());
            POS pos = listACDsWithConnectedComplexEffects.GetHeadPosition();
            while (pos)
            {
                const ACDID idACDConnected = (ACDID)listACDsWithConnectedComplexEffects.GetNext(pos);
                if (idACDConnected != ptACD->id)
                {
                    KnownEntry tConnectedEntry;
                    tConnectedEntry.eType = tEntry.eType;
                    tConnectedEntry.id = idACDConnected;
                    sAddKnownACD(tConnectedEntry, ptAddList, idKnownListCurrent, FALSE);
                }
            }
        }
    }
    sAddKnownEntry(tEntry, ptAddList, idKnownListCurrent);
}

static void sProcessGuaranteedEntries(const ActorCommonData & tACDPlayer, KnownListID idKnownListCurrent, KnownList * ptListAddedWorlds, KnownList * ptListAddedScenes, KnownList * ptListAddedACDs)
{
    if (!tACDPlayer.IsInWorld())
        return;
    SWorldID idSWorld = tACDPlayer.GetSWorldID();
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
    {
        KnownEntry tWorldEntry;
        tWorldEntry.eType = KET_WORLD_REVEAL;
        tWorldEntry.id = idSWorld;
        sAddKnownEntry(tWorldEntry, ptListAddedWorlds, idKnownListCurrent);
        tWorldEntry.eType = KET_WORLD_ENTER;
        sAddKnownEntry(tWorldEntry, ptListAddedWorlds, idKnownListCurrent);
    }
    NavMeshHandle tNavMeshHandle;
    ACDGetNavMeshHandle(tACDPlayer, tNavMeshHandle);
    if (!NavMeshHandleIsNone(tNavMeshHandle))
    {
        SceneCommonData * ptSCD = SceneCommonDataTryToGet(tNavMeshHandle.idSScene);
        if (ptSCD)
        {
            KnownEntry tSceneEntry;
            tSceneEntry.id = ptSCD->idSScene;
            tSceneEntry.eType = KET_PREFETCH_SCENE;
            sAddKnownScene(tSceneEntry, ptListAddedScenes, idKnownListCurrent);
            tSceneEntry.eType = ptSCD->idSSceneParent == -1 ? KET_SCENE : KET_SUBSCENE;
            sAddKnownScene(tSceneEntry, ptListAddedScenes, idKnownListCurrent);
        }
    }
    KnownEntry tACDEntry;
    tACDEntry.eType = KET_PREFETCH_ACD;
    tACDEntry.id = tACDPlayer.id;
    sAddKnownEntry(tACDEntry, ptListAddedACDs, idKnownListCurrent);
    tACDEntry.eType = KET_ACD;
    sAddKnownEntry(tACDEntry, ptListAddedACDs, idKnownListCurrent);
}

    // NON_MATCHING: 1256 bytes both; only block placement differs: the original places the SUBSCENE block before
    // the PREFETCH_SCENE block. Case order from the original line table (2675/2679/2682/2685/2691/2697); tWorldEnter
    // is in the switch scope (its DWARF lexical block spans every case). Tried 12 case orders and explicit WORLD_ENTER labels.
#line 2670
static void sAddEntryByType(KnownEntry & tKnownEntry, KnownList * ptListAddedWorlds, KnownList * ptListAddedScenes, KnownList * ptListAddedACDs, KnownListID idKnownListCurrent)
{
    switch (tKnownEntry.eType)
    {
    case KET_WORLD_REVEAL:
        sAddKnownEntry(tKnownEntry, ptListAddedWorlds, idKnownListCurrent);
        KnownEntry tWorldEnter;
        tWorldEnter.eType = KET_WORLD_ENTER;
        tWorldEnter.id = tKnownEntry.id;
        sAddKnownEntry(tWorldEnter, ptListAddedWorlds, idKnownListCurrent);
        break;
    case KET_SCENE:
        sAddKnownScene(tKnownEntry, ptListAddedScenes, idKnownListCurrent);
        break;
    case KET_SUBSCENE:
        sAddKnownEntry(tKnownEntry, ptListAddedScenes, idKnownListCurrent);
        break;
    case KET_PREFETCH_SCENE:
        sAddKnownPrefetchScene(tKnownEntry, ptListAddedScenes, idKnownListCurrent);
        break;
    case KET_PREFETCH_ACD: case KET_ACD:
        sAddKnownACD(tKnownEntry, ptListAddedACDs, idKnownListCurrent, TRUE);
        break;



    default:
        DisplayInternalError(NULL, "", 2697, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);  // one row 2697 (a macro in the original)
        break;
    }
}
#line 3136

void KnownListProcessAddedEntries(Player * ptPlayer, KnownEntryListID idList)
{

    KnownEntryGlobals & tKnownEntryGlobals = *SGameManager::GameGet().ptKnownEntryGlobals;
    if (tKnownEntryGlobals.tAddedList.Count() <= 0 && tKnownEntryGlobals.tChangedList.Count() <= 0)
        return;
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer || !PlayerIsInWorld(*ptPlayer) || SPlayerIsWaitingForSavePointPort(*ptPlayer))
        return;
    KnownEntryListID idListAddedWorlds = KNOWNENTRYLIST_INVALID;
    KnownEntryListID idListAddedScenes = KNOWNENTRYLIST_INVALID;
    KnownEntryListID idListAddedACDs = KNOWNENTRYLIST_INVALID;
    if (idList != KNOWNENTRYLIST_INVALID)
    {
        idListAddedWorlds = KnownEntryListAlloc();
        idListAddedScenes = KnownEntryListAlloc();
        idListAddedACDs = KnownEntryListAlloc();
    }
    KnownList * ptListAddedWorlds = sKnownEntryListTryToGet(idListAddedWorlds);
    KnownList * ptListAddedScenes = sKnownEntryListTryToGet(idListAddedScenes);
    KnownList * ptListAddedACDs = sKnownEntryListTryToGet(idListAddedACDs);
    KnownListID idKnownListCurrent = PlayerKnownListGetCurrent(*ptPlayer);
    sProcessGuaranteedEntries(*ptACDPlayer, idKnownListCurrent, ptListAddedWorlds, ptListAddedScenes, ptListAddedACDs);
    Frustum tFrustum;
    Frustum tPrefetchFrustum;
    Vector3D vCenterPos;
    SceneCreatePlayerFrustum(ptPlayer, tFrustum, vCenterPos);
    SceneCreatePlayerPrefetchFrustum(ptPlayer, tPrefetchFrustum);
    POS pos = tKnownEntryGlobals.tAddedList.GetHeadPosition();
    while (pos)
    {
        KnownEntry & tKnownEntry = tKnownEntryGlobals.tAddedList.GetNext(pos);
        KnownEntry tPrefetchEntry = tKnownEntry;
        tPrefetchEntry.eType = sGetPrefetchType(tKnownEntry.eType);
        if (tPrefetchEntry.eType != KET_INVALID && sCanSeeEntry(*ptACDPlayer, tPrefetchFrustum, tKnownEntry))
            sAddEntryByType(tPrefetchEntry, ptListAddedWorlds, ptListAddedScenes, ptListAddedACDs, idKnownListCurrent);
        if (sCanSeeEntry(*ptACDPlayer, tFrustum, tKnownEntry))
            sAddEntryByType(tKnownEntry, ptListAddedWorlds, ptListAddedScenes, ptListAddedACDs, idKnownListCurrent);
    }
    pos = tKnownEntryGlobals.tChangedList.GetHeadPosition();
    while (pos)
    {
        KnownEntry & tKnownEntry = tKnownEntryGlobals.tChangedList.GetNext(pos);
        KnownEntry tPrefetchEntry = tKnownEntry;
        tPrefetchEntry.eType = sGetPrefetchType(tKnownEntry.eType);
        if (tPrefetchEntry.eType != KET_INVALID && sCanSeeEntry(*ptACDPlayer, tPrefetchFrustum, tKnownEntry))
            sAddEntryByType(tPrefetchEntry, ptListAddedWorlds, ptListAddedScenes, ptListAddedACDs, idKnownListCurrent);
        if (sCanSeeEntry(*ptACDPlayer, tFrustum, tKnownEntry))
            sAddEntryByType(tKnownEntry, ptListAddedWorlds, ptListAddedScenes, ptListAddedACDs, idKnownListCurrent);
    }
    if (idList != KNOWNENTRYLIST_INVALID)
    {
        KnownList & tListAdded = sKnownEntryListGet(idList);
        pos = KnownEntryListGetFirst(idListAddedWorlds);
        while (pos)
        {
            const KnownEntry tKnownEntry = KnownEntryListGetNext(idListAddedWorlds, pos);
            tListAdded.AddTail_Tracked(tKnownEntry);
        }
        pos = KnownEntryListGetFirst(idListAddedScenes);
        while (pos)
        {
            const KnownEntry tKnownEntry = KnownEntryListGetNext(idListAddedScenes, pos);
            tListAdded.AddTail_Tracked(tKnownEntry);
        }
        pos = KnownEntryListGetFirst(idListAddedACDs);
        while (pos)
        {
            const KnownEntry tKnownEntry = KnownEntryListGetNext(idListAddedACDs, pos);
            tListAdded.AddTail_Tracked(tKnownEntry);
        }
        KnownEntryListFree(idListAddedACDs);
        KnownEntryListFree(idListAddedScenes);
        KnownEntryListFree(idListAddedWorlds);
    }
}
