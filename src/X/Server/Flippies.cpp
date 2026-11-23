// Original file: X/Server/Flippies.cpp
#include "./Flippies.h"
#include "../Common/ActorCommonData.h"
#include "../Common/ActorCommonDataSwig.h"
#include "../Common/CommonTypes.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/Nav/NavZone.h"
#include "../Common/Nav/NavigationMesh.h"
#include "../Common/Player.h"
#include "../Common/SceneCommon.h"
#include "../Common/TagMaps/ActorTags.h"
#include "../Common/TagMaps/Tags.h"
#include "../Common/WorldCommonCollision.h"
#include "../Common/WorldPlace.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/DataStructures/ChainedDataArrayTemplated.h"
// Declared here until their headers carry them: WorldCommonCollision.cpp:147, NavigationMesh.cpp:3180, ActorCommonData.cpp:2997.
BOOL WorldCollSweptCircle(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Circle & wcStart, const Vector2D & wpDest, const WorldCollParams & tWorldCollParams, uint32 uCollTestFlags, uint32 uACDCollNarrowFlags, ACDID idACDIgnore, WorldCollResult & tResult); uint32 NavMeshGetStaticFlagsForPassability(PassabilityType ePassability); Angle ACDGetYaw(ACDID idACD);
#include "../Sigma/Memory/SigmaMemory.h"
#include "Actors/Actors.h"
#include "Loot/Loot.h"
#include "SGame.h"
#include "SGameManager.h"
#include "../Common/Marker/Marker.h"
#include "../Common/Definitions/GameBalanceDefinition.h"
#include "../Sigma/RefString.h"
#include "../Common/Networking/Network.h"
#include "../Common/Items.h"
#include "../Common/NephalemRiftCommon.h"
#include "../Common/Game.h"
#include "../Common/World.h"
// Declared here until their headers carry them: Player.cpp:5556, Items.cpp:5631, Items.cpp:5621.
void PlayerAddOnGroundCraftingPlan(Player & tPlayer, GBID gbidPlan); int32 ItemGetPlayerRecipient(const ActorCommonData & tACD); void ItemSetPlayerRecipient(ActorCommonData & tACDItem, int32 nPlayerIndex);
#include "SGameConstants.h"
#include "PlayerList.h"
#include "SWorld.h"
#include "Network/KnownList.h"
#include "Actors/ActorEffect.h"
#include "../Common/Actors/ActorEffectCommon.h"
#include "../Common/Actors/ActorCollision.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Networking/GameMessage.h"
#include "../Sigma/Math/Quaternion.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
// Declared here until their headers carry them: SItems.cpp:324, SScene.cpp:513, ActorCommonData.cpp:4326, PlayerList.cpp:479, ActorInventory.cpp:13117.
void SItemLanded(ActorCommonData & tACDItem); void SScenePutACDInScene(SSceneID idSScene, ACDID idACD, BOOL bAdd); void LocationSetBounds(SNO snoActor, WorldLocation & tLocation, const ActorDefinition * pdefActor); void PlayerListGetInKnowRegionByID(PlayerList & listPlayers, OnlineService::HeroId idPlayer, const WorldPlace & tPlace); BOOL ActorInventoryAllowedToSeeItem(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem);

#include "Map/Minimap.h"
#include "LuaScript/ScriptEvent.h"
#include "Powers/PowerAPI.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/DataStructures/XPooledList.h"
// Declared here until their headers carry them: SGame.cpp:7255, SItems.cpp:1581, SGameStats.cpp:786, ActorInventory.cpp:3273, Items.cpp:2208, Items.cpp:4290, Items.cpp:3003.
void TraceStat(LPCSTR sEventType, ACDID idACD, LPCSTR cpFmtStr, ...); CRefString SItemGetDisplayName(const ActorCommonData & tACD, const BOOL, const BOOL fForceIdentified, const BOOL, const BOOL); void SGameStatsRecordLoot(ActorCommonData & tACDItem, LootContext eLootContext, const LooteeInfo * ptLootee); void ACDRemoveToLimbo(ActorCommonData & tACDItem); ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData & tACD); BOOL ItemIsHealthPotion(const ActorCommonData & tACDItem); void ItemGetItemTypes(const ItemDefinition * ptItem, XGrowableList<int, int> & listItemTypes);
enum { GB_ITEMTYPES = 1 }; enum { AT_INHERENT = 2 };  // GameBalanceDefinition.h:34 and :294 (the adopted subsets do not carry them yet)

enum
{
    FLIPPY_SEARCH_RANGE = 5,
    FLIPPY_EXTRA_SEARCH_RANGE = 12,
    FLIPPY_RANDOM_SPLASH_RANGE = 5,
};




static const int32 FLIPPY_HANG_TIME = 33;
static const int32 FLIPPY_LAUNCH_DELAY = 6;







struct Flippy
{
    FlippyID id;

    LootSpecifier tLootSpecifier;
    ACDID idACDSpawningActor;
    ACDID idACDExistingLoot;
    uint32 dwNextEventTime;
    WorldPlace tPlace;
    uint32 dwFlippyFlags;
    uint32 dwGameEventNumber;

    UpdateData tUpdateData;

    OnlineService::HeroId idAssignedHero;

    GameConnectionID idAssignedGameConnection;
};





struct FlippyGlobals
{
    TChainedDataArray<Flippy, int> * paFlippies;
    BOOL bReachedFlippySoftCap;
};




static Flippy * sFlippyTryToGet(
    FlippyID idFlippy)
{
    SGame & tSGame = SGameManager::GameGet();

    return tSGame.ptFlippyGlobals->paFlippies->ElementTryToGet(idFlippy);
}




static Flippy & sFlippyGet(
    FlippyID idFlippy)
{
    SGame & tSGame = SGameManager::GameGet();

    return *tSGame.ptFlippyGlobals->paFlippies->ElementGet(idFlippy);
}




void FlippiesInitializeForGame(
    void)
{
    SGame & tSGame = SGameManager::GameGet();

    tSGame.ptFlippyGlobals = (FlippyGlobals *)SigmaMemoryNew(sizeof(FlippyGlobals), 4, tSGame.ptMemoryInterface, TRUE);
    tSGame.ptFlippyGlobals->paFlippies = TChainedDataArrayCreateTracked<Flippy, int>("Flippy", tSGame.ptMemoryInterface, INITIAL_FLIPPY_COUNT);
    tSGame.ptFlippyGlobals->bReachedFlippySoftCap = FALSE;
}




static void sFlippyDispose(
    Flippy & tFlippy)
{
    if (tFlippy.idACDExistingLoot != ACDID_INVALID)
    {
        ActorCommonData * ptACDExistingLoot = ACDTryToGet(tFlippy.idACDExistingLoot);
        // No instruction of its own: the null test folds into ACDTryToGet's (ActorCommonData.h:1454/1451 branch past line 157).
        if (ptACDExistingLoot)
        {


            ActorID idActorExistingLoot = (ActorID)ACDGetOwner(tFlippy.idACDExistingLoot);
            ActorFlagForDeletion(idActorExistingLoot);
        }
    }

    SGame & tSGame = SGameManager::GameGet();
    tSGame.ptFlippyGlobals->paFlippies->ElementDeleteByID(tFlippy.id);
}




static FlippyID sFlippyCreate(
    void)
{
    SGame & tSGame = SGameManager::GameGet();


    FlippyID idFlippy = tSGame.ptFlippyGlobals->paFlippies->ElementNew();
    Flippy & tFlippy = sFlippyGet(idFlippy);

    tFlippy.idACDExistingLoot = ACDID_INVALID;
    tFlippy.dwNextEventTime = 0;
    tFlippy.dwFlippyFlags = 0;
    tFlippy.dwGameEventNumber = 0;
    tFlippy.idAssignedHero = OnlineService::INVALID_HERO_ID;

    tFlippy.idAssignedGameConnection = GAMECONNECTIONID_INVALID;


    UpdateFunctionInitialize(UPDATE_FLIPPIES, idFlippy, tFlippy.tUpdateData);

    return idFlippy;
}





void FlippiesDisposeForGame(
    void)
{
    SGame & tSGame = SGameManager::GameGet();

    if (!tSGame.ptFlippyGlobals)
    {
        return;
    }

    TDataArrayIterator<Flippy> tIt;
    while (Flippy * ptFlippy = tSGame.ptFlippyGlobals->paFlippies->Iterate(tIt))
    {



        if (ptFlippy->idACDExistingLoot != ACDID_INVALID)
        {

            ActorID idActor = (ActorID)ACDGetOwner(ptFlippy->idACDExistingLoot);
            ActorDelete(idActor);

            ptFlippy->idACDExistingLoot = ACDID_INVALID;
        }

        sFlippyDispose(*ptFlippy);
    }

    TChainedDataArrayDispose(tSGame.ptFlippyGlobals->paFlippies);
    SigmaMemoryFree(tSGame.ptFlippyGlobals, tSGame.ptMemoryInterface); tSGame.ptFlippyGlobals = NULL;
}



void FlippiesDumpForGame(
    SGame & tSGame)
{
    if (!tSGame.ptFlippyGlobals)
    {
        return;
    }




    TDataArrayIterator<Flippy> tIt;
    while (tSGame.ptFlippyGlobals->paFlippies->Iterate(tIt))
    {
        Flippy * ptFlippy = tIt.ptElement;


        WorldCommonData * ptWCD = WorldTryGetCommonData(ptFlippy->tPlace.idSWorld);
    }
}











static void sFlippyCheckAndHandleSoftCap(
    void)
{





    SGame & tSGame = SGameManager::GameGet();
    if (!tSGame.ptFlippyGlobals->bReachedFlippySoftCap)
    {

        uint32 dwNextMaxCount = tSGame.ptFlippyGlobals->paFlippies->dwMaximumCount + tSGame.ptFlippyGlobals->paFlippies->dwGrowSize;
        if (dwNextMaxCount >= MAX_DATA_ARRAY_ELEMENTS)
        {

            tSGame.ptSGameGlobals->dwFlags |= (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN);

            tSGame.ptFlippyGlobals->bReachedFlippySoftCap = TRUE;

            FlippiesDumpForGame(tSGame);
        }
    }
}




static void sFlippySetNextUpdate(
    Flippy & tFlippy)
{
    uint32 dwNow = GameGetServerTime();
    int32 nNextFlippyTime = -1;

    if (!(tFlippy.dwFlippyFlags & (1 << FF_LAUNCHED)) || !(tFlippy.dwFlippyFlags & (1 << FF_LANDED)))
    {






        nNextFlippyTime = Math::Max(tFlippy.dwNextEventTime, dwNow);
    }


    if (nNextFlippyTime != -1)
    {


        int32 nTicksDelay = Math::Min<int32>(nNextFlippyTime - dwNow, MAX_TICKS_UPDATE_DELAY);
        ServerQueueUpdateFunction(nTicksDelay, tFlippy.tUpdateData);
    }
}

















static void FlippySetItemOnGround(
    ActorCommonData & tACDItem,
    Flippy & tFlippy)
{




    WorldPlace & tPlace = tFlippy.tPlace;

    ActorID idActor = (ActorID)ACDGetOwner(tACDItem.id);
    Actor * ptActor = ActorGet(idActor);


    if (SWorldTryToGet(tPlace.idSWorld))
    {




        tACDItem.AttributesSetInt(ATTRIBUTE_FLIPPY_ID, FLIPPYID_INVALID);
        tFlippy.idACDExistingLoot = ACDID_INVALID;


        tACDItem.ChangeLocation(AL_WORLD);
        KnownListItemAdded(KET_ACD, tACDItem.id);


        Quaternion qOrientation;
        Angle aYaw = 0.0f;

        ActorLocation & tActorLocation = tACDItem.GetLocation();
        qOrientation.FromYaw(aYaw);




        ActorLocationInit(tActorLocation, tPlace, qOrientation, tACDItem.GetScale());



        ptActor->aYaw = aYaw;

        LocationSetBounds(tACDItem.snoActor, tActorLocation.tWorldLocation, NULL);


        ActorCollisionInitialize(tACDItem);

        ActorUpdateOrientation(idActor, FALSE);

        tACDItem.SetTimeStamp(GameGetServerTime());
        tACDItem.SetHitGroundTime(GameGetServerTime());



        SSceneID idSSceneNew = ACDGetCurrentSSceneID(tACDItem);


        if (idSSceneNew != SSCENEID_INVALID)
        {



            // The branch after this call carries line 395 too, so the return shares its line.
            SScenePutACDInScene(idSSceneNew, tACDItem.id, TRUE); return;
        }
    }
    ActorFlagForDeletion(idActor);
}

static void sFlippyTraceSpecStat(
    const LootSpecifier & tLootSpecifier,
    uint32 dwGameEventNumber,
    uint32 dwLootEventNumber)
{
    const int32 MAX_IT_LEVELS = 5;
    const int32 MAX_AFFIXES = 5;

    CRefString sItemName;
    CRefString sLine;

    const GameBalanceDefinition * pdefGB;
    ItemDefinition * ptItem = (ItemDefinition *)GBLockR(tLootSpecifier.hGBItem, pdefGB, TRUE);
    if (!ptItem)
    {
        return;
    }

    int32 nItemLevel = ptItem->nItemLevel;

    sItemName = ptItem->tHeader.szName;
    sLine.Format("%s\t%u\t%d\t%d\t%d",
        sItemName.str(),
        nItemLevel,
        tLootSpecifier.eIQL,
        tLootSpecifier.nAffixLevel,
        tLootSpecifier.nGoldAmount);

    int32 nItemLevelCount = 0;
    XGrowableList<int32, int32> listItemTypes;
    ItemGetItemTypes(ptItem, listItemTypes);
    GBUnlockR(pdefGB);

    POS pos = listItemTypes.GetHeadPosition();
    while (pos && nItemLevelCount < MAX_IT_LEVELS)
    {
        GBHandle hGB(GB_ITEMTYPES, listItemTypes.GetNext(pos));
        const ItemTypeDefinition * ptItemTypeDef = (const ItemTypeDefinition *)GBLockR(hGB, pdefGB, TRUE);
        sLine += "\t";
        sLine += ptItemTypeDef->tHeader.szName;
        GBUnlockR(pdefGB);
        nItemLevelCount++;
    }


    CRefString sAffixName[MAX_AFFIXES];
    for (int32 nAffix = 0; nAffix < MAX_AFFIXES; nAffix++)
    {
        if (nAffix >= tLootSpecifier.nNumAffixes)
        {
            break;
        }

        GBID gbidAffix = tLootSpecifier.arAffixGBIDs[nAffix];
        ScopedItemAffixLockR ptAffix(gbidAffix);
        if (ptAffix)
        {



            if (ptAffix->eAffixType != AT_INHERENT)

                sAffixName[nAffix] = ptAffix->tHeader.szName;
        }
    }

    for (int32 i = 0; i < MAX_AFFIXES; i++)
    {
        sLine += "\t";
        sLine += sAffixName[i];
    }

    TraceStat("lootspec", ACDID_INVALID, "%u\t%u\t%s", dwGameEventNumber, dwLootEventNumber, sLine.str());
}




static void sFlippyTraceItemStat(
    ActorCommonData & tACDItem,
    const LootSpecifier & tLootSpecifier,
    uint32 dwGameEventNumber)
{
    if (!TraceStatEnabled())
    {
        return;
    }

    uint32 dwLootEventNumber = SGameGetEventNumber();

    sFlippyTraceSpecStat(tLootSpecifier, dwGameEventNumber, dwLootEventNumber);

    CRefString usDisplayName = SItemGetDisplayName(tACDItem, TRUE, TRUE, FALSE, TRUE);

    CRefString sItemDescription("none");

    TraceStat(
        "lootname",
        ACDID_INVALID,
        "%u\t%u\t%s\t%s",
        dwGameEventNumber,
        dwLootEventNumber,
        usDisplayName.str(),
        sItemDescription.str());
}




static void sServerMessageFlippy(
    const ACDID idACDLauncher,
    const ACDID idACDExistingLoot,
    OnlineService::HeroId idAssignedHero,
    const SNO snoParticle,
    const SNO snoActorLoot,
    const WorldPlace & tPlace)
{
    FlippyMessage tMessage;
    tMessage.annLauncher = ANNFromACD(idACDLauncher);
    tMessage.snoFlippyParticle = snoParticle;
    tMessage.snoActorLoot = snoActorLoot;
    tMessage.wpDest = tPlace.wp;

    PlayerList listDestPlayers(PlayerListGetElementPool());

    PlayerListGetInKnowRegionByID(listDestPlayers, idAssignedHero, tPlace);

    ActorCommonData * ptACDExistingLoot = ACDTryToGet(idACDExistingLoot);

    if (ptACDExistingLoot && ptACDExistingLoot->IsItem())
    {
        POS pos = listDestPlayers.GetHeadPosition();
        while (pos)
        {
            POS posCurrent = pos;
            Player * ptPlayer = listDestPlayers.GetNext(pos);

            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer && !ActorInventoryAllowedToSeeItem(*ptACDPlayer, *ptACDExistingLoot))
            {
                listDestPlayers.RemoveAt(posCurrent);
            }
        }
    }

    MessageSendToClient(listDestPlayers, SMSG_FLIPPY, &tMessage, sizeof(tMessage));
}



void FlippyDestroy(
    FlippyID idFlippy)
{
    Flippy * ptFlippy = sFlippyTryToGet(idFlippy);
    if (ptFlippy)
    {
        sFlippyDispose(*ptFlippy);
    }
}




static void sLaunchFlippy(
    Flippy & tFlippy)
{
    SNO snoActorSpawned = tFlippy.tLootSpecifier.snoActorForFlippy;

    ScopedActorDefinitionLockR pdefActorLoot(snoActorSpawned);
    SNO snoFlippyParticle = TagMapGetSNO(pdefActorLoot->hTagMap, TAG_ACTOR_FLIPPY_PARTICLE_SYSTEM);
    SNO snoFlippyActor = TagMapGetSNO(pdefActorLoot->hTagMap, TAG_ACTOR_FLIPPY_SPECIAL_ACTOR);

    if (snoFlippyParticle == SNO_NONE)
    {
        snoFlippyParticle = GlobalSNOGet(GLOBALSNO_PARTICLE_FLIPPY_DEFAULT);
    }


    if (snoFlippyActor != SNO_NONE)
    {
        snoActorSpawned = snoFlippyActor;
    }

    if (TagMapGetInt(pdefActorLoot->hTagMap, TAG_ACTOR_FLIPPY_DONT_OVERRIDE_PARTICLE_ACTOR))
    {
        snoActorSpawned = SNO_NONE;
    }

    uint32 dwNow = GameGetServerTime();
    tFlippy.dwNextEventTime = dwNow + FLIPPY_HANG_TIME;
    tFlippy.dwFlippyFlags &= ~(1 << FF_NO_HALO_SOUND);

    sServerMessageFlippy(tFlippy.idACDSpawningActor, tFlippy.idACDExistingLoot, tFlippy.idAssignedHero, snoFlippyParticle, snoActorSpawned, tFlippy.tPlace);


    tFlippy.dwFlippyFlags |= (1 << FF_LAUNCHED);
    sFlippySetNextUpdate(tFlippy);
}




static void sLandFlippy(
    Flippy & tFlippy)
{
    ActorCommonData * ptACDItem = ACDTryToGet(tFlippy.idACDExistingLoot);
    if (ptACDItem)
    {
        FlippySetItemOnGround(*ptACDItem, tFlippy);

        ptACDItem->SetAssignedHeroID(tFlippy.idAssignedHero);

        ptACDItem->SetAssignedGameConnectionID(tFlippy.idAssignedGameConnection);



        if (!(tFlippy.dwFlippyFlags & (1 << FF_NO_HALO_SOUND)))
        {
            ACDPlayEffect(ptACDItem->id, EFFECT_PLAY_HALO_SOUND, 0, ACDID_INVALID);
        }
        ACDPlayEffect(ptACDItem->id, EFFECT_PLAY_LANDING_SOUND, 0, ACDID_INVALID);

        SItemLanded(*ptACDItem);
    }
    else
    {
        DisplayInternalError(NULL, "", 628, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);  // the binary passes line 628 (0x274); the call is on DWARF line 627
    }


    tFlippy.dwFlippyFlags |= (1 << FF_LANDED);
}




void FlippyUpdate(
    FlippyID idFlippy)
{

    Flippy * ptFlippy = sFlippyTryToGet(idFlippy);
    // No instruction of its own: the null test folds into ElementTryToGet's (ChainedDataArrayTemplated.h:451 branch to line 711).
    if (!ptFlippy)
    {
        return;
    }
    ServerUpdateWasCalled(ptFlippy->tUpdateData);

    uint32 dwNow = GameGetServerTime();


























    if (!(ptFlippy->dwFlippyFlags & (1 << FF_LAUNCHED)))
    {
        if (ptFlippy->dwNextEventTime <= dwNow)
        {
            sLaunchFlippy(*ptFlippy);
        }
        else
        {



            sFlippySetNextUpdate(*ptFlippy);
        }
    }


    if (!(ptFlippy->dwFlippyFlags & (1 << FF_LANDED)))
    {
        if (ptFlippy->dwNextEventTime <= dwNow)
        {
            sLandFlippy(*ptFlippy);
        }
        else
        {



            sFlippySetNextUpdate(*ptFlippy);
        }
    }

    if (ptFlippy->dwFlippyFlags & (1 << FF_LANDED))
    {
        sFlippyDispose(*ptFlippy);
    }
}





static BOOL sFlippyGetLandingNodeRandom(
    const WorldPlace & placeStartOrig,
    WorldPlace & placeLanding,
    ACDID idACDKiller,
    float flSpawnerRadius)
{
    NavCellHandle hNavCellStart, hNavCellSearch;
    Vector3D wpSearch;
    float flMaxSearchRadius = flSpawnerRadius;

    flMaxSearchRadius = Math::Max(flMaxSearchRadius, 12.5f);




    uint16 uOneRequiredFlags = 1 << CELL_STATIC_ALLOW_WALK;
    WorldPlace placeStart = placeStartOrig;
    if (NavCellHandleFromWorldPlace(placeStart, hNavCellStart))
    {
        const NavZoneInstance * ptNavZoneInstance = NavCellHandleToZoneInstance(hNavCellStart);
        NavCellModifyForRoundedCorners(ptNavZoneInstance, hNavCellStart, placeStart.wp.As2D(), uOneRequiredFlags);

        uint32 uObjectNoOverlapFlags = NavMeshGetDynamicFlagsForPassability(PASSABILITY_FLIPPIES);
        uint16 uObjectNoPassthroughFlags = (1 << CELL_STATIC_SUBSCENE) | (1 << CELL_STATIC_NO_SPAWN) | (1 << CELL_STATIC_ALLOW_GHOST);
        if (WorldCollSearchForLocation(placeStart, uOneRequiredFlags, uObjectNoOverlapFlags, 1.25f, 0.0f, flMaxSearchRadius, ACDID_INVALID, wpSearch, hNavCellSearch, TEAM_NEUTRAL, uObjectNoPassthroughFlags, idACDKiller))
        {












            placeLanding.idSWorld = placeStart.idSWorld;
            placeLanding.wp = wpSearch;
            return TRUE;
        }
    }



    NavMeshHandle tNavMeshHandle;
    if (ScenesCommonNavMeshNodeFromPlace(placeStart, &tNavMeshHandle, FALSE, FALSE))
    {
        uint32 dwSquareStaticFlags = NavMeshHandleGetStaticFlags(tNavMeshHandle);
        if (dwSquareStaticFlags & (1 << CELL_STATIC_ALLOW_WALK))
        {
            NavMeshGetCenterPlace(tNavMeshHandle, placeLanding);
            return TRUE;
        }
    }



    ActorCommonData * ptACDKiller = ACDTryToGet(idACDKiller);
    if (ptACDKiller)
    {
        WorldPlace placePlayer = ptACDKiller->GetPlace();
        if (placePlayer.idSWorld == placeStart.idSWorld)
        {
            if (Vector2D::DistanceSquared(placePlayer.wp.As2D(), placeStart.wp.As2D()) <= 3600.0f)
            {
                Circle circleItem;
                circleItem.Set(placePlayer.wp.As2D(), 1.25f);

                WorldCollParams tCollParams;
                tCollParams.uOneRequiredStaticFlags = NavMeshGetStaticFlagsForPassability(PASSABILITY_FLIPPIES);

                NavCellHandle hNavCellStart2;
                NavCellHandleFromWorldPlace(placePlayer, hNavCellStart2);
                WorldCollResult tResult;
                WorldCollSweptCircle(placePlayer.idSWorld, hNavCellStart2, circleItem, placeStart.wp.As2D(), tCollParams, 3, 1, idACDKiller, tResult);









                const NavZoneInstance * ptNavZoneInstance = NavCellHandleToZoneInstance(tResult.hNavCellEnd);
                NavCellModifyForRoundedCorners(ptNavZoneInstance, tResult.hNavCellEnd, tResult.wpEnd.As2D(), tCollParams.uOneRequiredStaticFlags);

                WorldPlace placeDest;
                placeDest.idSWorld = placePlayer.idSWorld;
                placeDest.wp = tResult.wpEnd;
                NavMeshHandle tNavMeshHandle2;
                if (ScenesCommonNavMeshNodeFromPlace(placeDest, &tNavMeshHandle2, FALSE, FALSE))
                {
                    uint32 dwSquareStaticFlags = NavMeshHandleGetStaticFlags(tNavMeshHandle2);
                    if (dwSquareStaticFlags & (1 << CELL_STATIC_ALLOW_WALK))
                    {
                        NavMeshGetCenterPlace(tNavMeshHandle2, placeLanding);
                        return TRUE;
                    }
                }
            }
        }
    }

    return FALSE;
}





static const float FLIPPY_MAXIMUM_PLAYER_DISTANCE_SQR = 10000.0f;
BOOL FlippyFindLandingLocation(
    const ActorCommonData & tACDLaunching,
    const SNO snoActorFlippy,
    WorldPlace & tPlaceLanding,
    const Player * ptAssignedPlayer,
    const Vector3D * pvBias,
    ACDID idACDKiller)
{

    WorldPlace tPlace = tACDLaunching.GetPlace();

    ScopedActorDefinitionLockR pdefActorLaunching(tACDLaunching.snoActor);
    BOOL bLaunchForward = TagMapGetDWORD(pdefActorLaunching->hTagMap, TAG_ACTOR_FLIPPY_LAUNCH_FORWARD);

    ScopedActorDefinitionLockR pdefActorFlippy(snoActorFlippy);
    float flLandingHeight = TagMapGetFloat(pdefActorFlippy->hTagMap, TAG_ACTOR_FLIPPY_LANDING_HEIGHT);

    NavCellHandle hNavCellCenter;
    NavCellHandleFromWorldPlace(tPlace, hNavCellCenter);

    WorldCollParams tCollParams;
    tCollParams.uOneRequiredStaticFlags = NavMeshGetStaticFlagsForPassability(PASSABILITY_FLIPPIES);

    Circle circleFlippy;
    circleFlippy.Set(tPlace.wp.As2D(), 2.0f);
    if (bLaunchForward)
    {
        Angle aYaw = ACDGetYaw(tACDLaunching.id);
        Vector3D wvForward(cgwvXAxis);
        Vector3D::Rotate(cgwvZAxis, aYaw, wvForward);

        wvForward *= tACDLaunching.GetRadius();

        WorldPlace tPlaceDest;
        WorldPlaceOffset(tPlace, wvForward, tPlaceDest);

        WorldCollResult tResult;
        WorldCollSweptCircle(
            tPlace.idSWorld,
            hNavCellCenter,
            circleFlippy,
            tPlaceDest.wp.As2D(),
            tCollParams,
            0,
            0,
            tACDLaunching.id, tResult);


        tPlace.wp.x = tResult.wpEnd.x;
        tPlace.wp.y = tResult.wpEnd.y;
    }

    if (pvBias)
    {
        WorldPlace tPlaceDest;
        WorldPlaceOffset(tPlace, *pvBias, tPlaceDest);

        WorldCollResult tResult;
        WorldCollSweptCircle(
            tPlace.idSWorld,
            hNavCellCenter,
            circleFlippy,
            tPlaceDest.wp.As2D(),
            tCollParams,
            0,
            0,
            tACDLaunching.id, tResult);


        tPlace.wp.x = tResult.wpEnd.x;
        tPlace.wp.y = tResult.wpEnd.y;
    }


    BOOL bOnMesh = sFlippyGetLandingNodeRandom(tPlace, tPlaceLanding, idACDKiller, tACDLaunching.GetRadius());
    if (!bOnMesh)
    {
        ACDID idAssignedPlayerACD = ptAssignedPlayer ? ptAssignedPlayer->idPlayerACD : ACDID_INVALID;
        ActorCommonData * ptACDAssignedPlayer = ACDTryToGet(idAssignedPlayerACD);
        if (ptACDAssignedPlayer)
        {
            WorldPlace placePlayer = ptACDAssignedPlayer->GetPlace();
            WorldPlace placeGizmo = tACDLaunching.GetPlace();
            BOOL bCloseEnough = FALSE;
            if (placePlayer.idSWorld == placeGizmo.idSWorld)
            {
                Vector2D vDelta = placeGizmo.wp.As2D() - placePlayer.wp.As2D();
                bCloseEnough = vDelta.MagnitudeSquared() <= FLIPPY_MAXIMUM_PLAYER_DISTANCE_SQR;
            }






            if (bCloseEnough) { tPlace = placePlayer; } else { tPlace = placeGizmo; }






            bOnMesh = sFlippyGetLandingNodeRandom(tPlace, tPlaceLanding, idACDKiller, tACDLaunching.GetRadius());
        }


        if (!bOnMesh) { return FALSE; }
    }









    tPlaceLanding.wp.z += flLandingHeight;
    return TRUE;

}




static const int32 LEGENDARY_MAP_PING_TICKS = 0;
// NON_MATCHING: 4108 of 4116 bytes; the only remaining difference is block placement in the inlined XGrowableList destructor loop of sFlippyTraceSpecStat (line 474; original: tbnz/b/ldr/cbnz/b around +0x818, ours one branch fewer). Taking tActorLocation out of its invented bare scope (DWARF: it sits in the if (ptACDItem) block with the 1077 inline and the eIQL block) removed the register/spill and tail-merge differences.
void FlippiesCreateOnActor(
    const ActorCommonData & tACDSpawningActor,
    const ACDID idACDAssigned,
    const LootSpecifierArray & arLootSpecifiersToSpawn,
    const uint32 dwFlippyFlags,
    uint32 dwGameEventNumber,
    float flCameraBiasDist,
    const ACDID idACDKiller,
    const LooteeInfo * ptLootee,
    const Vector3D * pvForcedPositionBias,
    const LootContext eLootContext,
    XPooledDataIDList * plistSpawnedItemACDs)
{
    uint32 dwNow = GameGetServerTime();

    int32 nNumFlippies = arLootSpecifiersToSpawn.GetArraySize();

    OnlineService::HeroId idAssignedHero = OnlineService::INVALID_HERO_ID;
    GameConnectionID idAssignedGameConnection = GAMECONNECTIONID_INVALID;
    Player * ptAssignedPlayer = NULL;
    Vector3D vBias = cgwvNull;
    ActorCommonData * ptACDAssigned = ACDTryToGet(idACDAssigned);
    if (ptACDAssigned)  // No instruction of its own: the null test folds into ACDTryToGet's (ActorCommonData.h:1448/1454).
    {
        ptAssignedPlayer = PlayerGetByACD(idACDAssigned);
        if (ptAssignedPlayer)
        {
            idAssignedHero = ptAssignedPlayer->uHeroId;
            idAssignedGameConnection = ptAssignedPlayer->idGameConnection;

            if (flCameraBiasDist > EPSILON)
            {

                Vector3D vBiasDir(0.70710678f, 0.70710678f, 0.0f);
                vBias = vBiasDir * flCameraBiasDist;
            }
        }
    }

    if (pvForcedPositionBias)
    {
        vBias += *pvForcedPositionBias;
    }

    uint32 dwCreatedCount = 0;
    ActorCommonData * ptACDBestItem = NULL;

    for (int32 i = 0; i < nNumFlippies; i++)
    {
        GBID gbidNewItem = arLootSpecifiersToSpawn[i].hGBItem.gbid;
        if (gbidNewItem == GBID_NONE)
        {
            continue;
        }


        FlippyID idFlippy = sFlippyCreate();
        Flippy & tFlippy = sFlippyGet(idFlippy);

        tFlippy.idACDSpawningActor = tACDSpawningActor.id;
        tFlippy.dwGameEventNumber = dwGameEventNumber;
        tFlippy.dwFlippyFlags = dwFlippyFlags;
        tFlippy.tLootSpecifier = arLootSpecifiersToSpawn[i];
        tFlippy.dwNextEventTime = dwNow + (dwCreatedCount * FLIPPY_LAUNCH_DELAY);


        tFlippy.dwFlippyFlags &= ~((1 << FF_NO_HALO_SOUND) | (1 << FF_LAUNCHED) | (1 << FF_LANDED));

        tFlippy.idAssignedHero = idAssignedHero;

        tFlippy.idAssignedGameConnection = idAssignedGameConnection;



        ActorCommonData * ptACDItem = NULL;
        BOOL bFoundSpot = FlippyFindLandingLocation(tACDSpawningActor, tFlippy.tLootSpecifier.snoActorForFlippy, tFlippy.tPlace, ptAssignedPlayer, &vBias, idACDKiller);
        if (bFoundSpot)
        {


            ACDID idACDItem = LootCreateItem(tFlippy.tLootSpecifier, tFlippy.tPlace, NULL);
            ptACDItem = ACDTryToGet(idACDItem);
            if (ptACDItem)  // Folds into ACDTryToGet's null test (ActorCommonData.h:1448/1454 branch to line 1146).
            {

                if (ItemHasLabel(gbidNewItem, ITL_QUEST) && !ItemHasLabel(gbidNewItem, ITL_BOOK))
                {


                    tFlippy.idAssignedHero = OnlineService::INVALID_HERO_ID;

                    tFlippy.idAssignedGameConnection = GAMECONNECTIONID_INVALID;

                    ptACDItem->ClearAssignedHero();
                }


                if (ptAssignedPlayer && !ItemHasLabel(gbidNewItem, ITL_BOOK) && !ItemIsGold(*ptACDItem))
                {
                    ItemSetPlayerRecipient(*ptACDItem, ptAssignedPlayer->nPlayerIndex);
                }


                if (!ptAssignedPlayer && !ItemHasLabel(gbidNewItem, ITL_BOOK) && !ItemIsGold(*ptACDItem) && !ItemIsHealthPotion(*ptACDItem) && !ItemHasLabel(gbidNewItem, ITL_HEALTH_GLYPH))
                {
                    // The original body (lines 1062-1066) leaves no instruction; only the condition's calls remain at line 1061.
                }

                // No instruction of its own: ptAssignedPlayer is known on both paths here, so this test is threaded away.
                if (ptAssignedPlayer && ItemHasLabel(ptACDItem->hGB.gbid, ITL_CRAFTING_PLAN))
                {
                    PlayerAddOnGroundCraftingPlan(*ptAssignedPlayer, ptACDItem->hGB.gbid);
                }




                ptACDItem->AttributesSetInt(ATTRIBUTE_FLIPPY_ID, idFlippy);
                SGameStatsRecordLoot(*ptACDItem, eLootContext, ptLootee);
                sFlippyTraceItemStat(*ptACDItem, tFlippy.tLootSpecifier, tFlippy.dwGameEventNumber);
                tFlippy.idACDExistingLoot = ptACDItem->id;

                if (ptAssignedPlayer)
                {
                    PowerAPI_BuffApplyOnLootDropEffect(ptAssignedPlayer->idPlayerACD, *ptACDItem, eLootContext);
                }

                ACDRemoveToLimbo(*ptACDItem);




                // DWARF: tActorLocation (1093) belongs to the if (ptACDItem) block, which also holds the 1077 inline and the eIQL block.


                ActorLocation tActorLocation;
                ActorLocationInit(tActorLocation, tFlippy.tPlace, cgqIdentity, ptACDItem->GetScale());
                ptACDItem->UpdateCollision(tActorLocation.tWorldLocation);

                if (ItemGetUseType(*ptACDItem) == ITEM_USE_PLAYER_GIFT)
                {
                    ptACDItem->AttributesSetInt(ATTRIBUTE_ITEM_QUALITY_LEVEL, arLootSpecifiersToSpawn[i].eIQL);
                }
                else
                {
                    ItemQualityLevel eIQL = ItemGetDisplayedQualityLevel(*ptACDItem);
                    SNO snoTexturePing = SNO_NONE;
                    switch (eIQL)
                    {
                    case IQL_LEGENDARY:
                        snoTexturePing = GlobalSNOGet(GLOBALSNO_TEXTURE_MARKER_LEGENDARY_PING);
                        break;
                    case IQL_DISPLAY_AS_SET_ITEM:
                        snoTexturePing = GlobalSNOGet(GLOBALSNO_TEXTURE_MARKER_SET_ITEM_PING);
                        break;
                    }
                    if (snoTexturePing != SNO_NONE &&
                        !ItemHasLabel(ptACDItem->hGB.gbid, ITL_CRAFTING) &&
                        !ItemHasLabel(ptACDItem->hGB.gbid, ITL_ORNAMENT) &&
                        !ItemHasLabel(ptACDItem->hGB.gbid, ITL_TIERED_RIFT_KEY) &&
                        !ItemHasLabel(ptACDItem->hGB.gbid, ITL_CRAFTING_PLAN_LEGENDARY))
                    {
                        PingID idPing = Minimap::PingLocation(tFlippy.tPlace, LEGENDARY_MAP_PING_TICKS, idAssignedHero, snoTexturePing, FALSE, (uint32)-1, SNO_NONE, SNO_NONE, 0.0f, SNO_NONE);
                        ptACDItem->AttributesSetInt(ATTRIBUTE_PING, idPing);
                    }
                }
            }
        }


















        if (!ptACDItem)
        {
            tFlippy.dwFlippyFlags |= (1 << FF_LAUNCHED);
            tFlippy.dwFlippyFlags |= (1 << FF_LANDED);
        }
        else
        {

            if (!ptACDBestItem || ItemGetQualityLevel(*ptACDBestItem) < ItemGetQualityLevel(*ptACDItem))
            {
                ptACDBestItem = ptACDItem;
            }

            if (plistSpawnedItemACDs)
            {
                plistSpawnedItemACDs->AddTail_Tracked(ptACDItem->id);
            }
        }

        sFlippySetNextUpdate(tFlippy);

        dwCreatedCount++;
    }


    if (dwCreatedCount > 0)
    {
        ScriptEventOnLootDropped(tACDSpawningActor.id, idACDAssigned, ptACDBestItem ? ptACDBestItem->id : ACDID_INVALID, dwCreatedCount);
    }

    sFlippyCheckAndHandleSoftCap();
}




void FlippyDropCreateOnActor(
    const ACDID idACDDropper,
    const ACDID idACDItem,
    const WorldPlace & tPlace)
{
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    // No instruction of its own: the null test folds into ACDTryToGet's (ActorCommonData.h:1448/1454 branch to line 1248).
    if (ptACDItem)
    {






        uint32 dwNow = GameGetServerTime();

        FlippyID idFlippy = sFlippyCreate();
        Flippy & tFlippy = sFlippyGet(idFlippy);
        ptACDItem->AttributesSetInt(ATTRIBUTE_FLIPPY_ID, idFlippy);

        tFlippy.idACDSpawningActor = idACDDropper;
        tFlippy.idACDExistingLoot = idACDItem;
        tFlippy.tLootSpecifier.snoActorForFlippy = ACDGet(idACDItem).snoActor;
        tFlippy.tLootSpecifier.hGBItem = ACDGet(idACDItem).hGB;
        tFlippy.tPlace = tPlace;
        tFlippy.dwNextEventTime = dwNow;
        tFlippy.dwFlippyFlags |= (1 << FF_NO_HALO_SOUND);
        tFlippy.dwFlippyFlags &= ~(1 << FF_LAUNCHED);
        tFlippy.dwFlippyFlags &= ~(1 << FF_LANDED);
        tFlippy.idAssignedHero = ptACDItem->GetAssignedHeroID();

        tFlippy.idAssignedGameConnection = ptACDItem->GetAssignedGameConnectionID();


        Player * ptAssignedPlayer = PlayerGetByACD(idACDDropper);

        if (ItemHasLabel(ptACDItem->hGB.gbid, ITL_CRAFTING_PLAN))
        {
            PlayerAddOnGroundCraftingPlan(*ptAssignedPlayer, ptACDItem->hGB.gbid);
        }





        if (ptAssignedPlayer)
        {

            if (NephalemRift::IsRiftKey(idACDItem) ||
                GameIsSeasonal())
            {
                if (ItemGetPlayerRecipient(*ptACDItem) == -1)
                {
                    ItemSetPlayerRecipient(*ptACDItem, ptAssignedPlayer->nPlayerIndex);
                }
            }
        }



        ActorLocation tActorLocation;
        ActorLocationInit(tActorLocation, tPlace, cgqIdentity, ptACDItem->GetScale());
        ptACDItem->UpdateCollision(tActorLocation.tWorldLocation);

        sFlippySetNextUpdate(tFlippy);
    }

    sFlippyCheckAndHandleSoftCap();
}














































CRefString FlippyGetDebugString(
    FlippyID idFlippy)
{
    Flippy * ptFlippy = sFlippyTryToGet(idFlippy);
    if (ptFlippy)
    {
        ActorCommonData * ptACDSpawningActor = ACDTryToGet(ptFlippy->idACDSpawningActor);
        ActorCommonData * ptACDExistingLoot = ACDTryToGet(ptFlippy->idACDExistingLoot);

        return CRefString().Format("Spawning Actor:%s Existing Loot:%s Actor:%s Item:%s Flags:%d ",
            ptACDSpawningActor ? MarkerNameGetString(ptACDSpawningActor->tName) : "Invalid ACD",
            ptACDExistingLoot ? MarkerNameGetString(ptACDExistingLoot->tName) : "Invalid ACD",
            SNOToString(ACTOR_SNO, ptFlippy->tLootSpecifier.snoActorForFlippy, FALSE).str(),
            GBIDToString(ptFlippy->tLootSpecifier.hGBItem, FALSE).str(),
            ptFlippy->dwFlippyFlags);
    }


    return CRefString("Invalid FlippyID");

}
