// Original file: X/Common/World.cpp

#include "World.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer; it reaches them through include chains the DWARF does not record.
#include "SNO/SNOName.h"
#include "WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"
#include "D3TypeDescriptor/D3TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../Sigma/LanguageRules.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/Random.h"
#include "../Sigma/RefString.h"
#include "Definitions/SceneDefinition.h"
#include "Definitions/SceneGroupDefinition.h"
#include "Definitions/StringListDefinition.h"
#include "Definitions/WorldDefinition.h"
#include "Game.h"
#include "GameCommonData.h"
#include "GlobalSNO.h"
#include "SceneCommon.h"
#include "../Client/CGame.h"
#include "../Client/CWorld.h"
#include "../Server/SWorld.h"
#include <stddef.h>

// Declared here because their headers are not adopted yet (or do not declare them).
SNO SWorldGetDungeonFinderTileset(SNO snoWorld);  // X/Server/SWorld.cpp:2474
BOOL SWorldIsFlaggedForDeletion(SWorldID idSWorld);  // X/Server/SWorld.cpp:346
void DRLGGetWorldOrigin(SNO snoWorld, IVector2D & pointOriginInFeet);  // X/Common/DRLGWorld.cpp:527
void NavmeshStitchingInitializeForWorld(WorldCommonData & tWCD, int32 nStitchSizeInFeetX, int32 nStitchSizeInFeetY, int32 nWorldSizeInFeetX, int32 nWorldSizeInFeetY);  // NavMeshStitch.cpp:481
void NavmeshStitchingDisposeForWorld(WorldCommonData & tWCD);  // NavMeshStitch.cpp:599
void ActorCollisionInitializeForWorld(WorldCommonData & tWCD);  // ActorCollision.cpp:77
void ActorCollisionDisposeForWorld(WorldCommonData & tWCD);  // ActorCollision.cpp:106
void ACDCollInitializeForWorld(WorldCommonData & tWCD);  // ActorCommonCollision.cpp:92
void ACDCollDisposeForWorld(WorldCommonData & tWCD);  // ActorCommonCollision.cpp:109
void InactiveMarkerInitializeForWorld(SWorldID idSWorld);  // InactiveMarker.cpp:361
void InactiveMarkerDisposeForWorld(SWorldID idSWorld);  // InactiveMarker.cpp:373
BOOL SNOCheckSNOReference(SNOGroup snoGroupSrc, SNO snoHandleSrc, SNOGroup snoGroupDst, SNO snoHandleDst, LPCSTR szExtraMsg);  // SNOFiles.cpp:10239
void SceneCommonDataActivate(const SSceneID idSScene);  // SceneCommon.cpp:729
int32 WorldGetDungeonFinderLevel(const SNO snoWorld);
void WorldCommonDataInitializePhysics(WorldCommonData & tWCD);
BOOL WorldTreatAsOneSceneBySNO(const SNO snoWorld);





StartLocationOverride gStartLocationOverride = { SNO_NONE };
DungeonFinderNameOverride gDungeonFinderNameOverride = { -1, -1 };


















static const TypeDescriptorField WorldSyncedData_Fields[4] = {
    TypeDescriptorField("", DT_SNO, offsetof(WorldSyncedData, snoWeatherOverride), NULL, 1), TypeDescriptorField("", DT_FLOAT, offsetof(WorldSyncedData, flWeatherIntensityOverride), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WorldSyncedData, dwWeatherIntensityOverrideEnd), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(WorldSyncedData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_WorldSyncedData("WorldSyncedData", 0, WorldSyncedData_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_WORLD_SYNCED_DATA = &TypeDescriptor_WorldSyncedData;


static const TypeDescriptorField WorldSyncedDataMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(WorldSyncedDataMessage, tHeader), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(WorldSyncedDataMessage, idSWorld), NULL, 1),
    TypeDescriptorField("", DT_WORLD_SYNCED_DATA, offsetof(WorldSyncedDataMessage, tSyncedData), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(WorldSyncedDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_WorldSyncedDataMessage("WorldSyncedDataMessage", 0, WorldSyncedDataMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_WORLD_SYNCED_DATA_MESSAGE = &TypeDescriptor_WorldSyncedDataMessage;









void WorldCommonDataInitialize(WorldCommonData & tWCD,
                               SNO snoWorld,
                               SWorldID idSWorld,
                               int32 nOriginX,
                               int32 nOriginY,
                               int32 nStitchSizeInFeetX,
                               int32 nStitchSizeInFeetY,
                               int32 nWorldSizeInFeetX,
                               int32 nWorldSizeInFeetY,
                               SNO snoDungeonFinderSourceWorld)
{






    tWCD.snoWorld = snoWorld;
    tWCD.snoDungeonFinderSourceWorld = snoDungeonFinderSourceWorld;
    tWCD.idSWorld = idSWorld;
    tWCD.dwWorldStatusBits = 0;
    tWCD.ptACDCollSphereTree = NULL;
    tWCD.ptACDBlockerCollSphereTree = NULL;
    tWCD.ptActorSphereTreeFastMover = NULL;
    tWCD.ptActorSphereTreeItemsInWorld = NULL;
    tWCD.ptWorldStitching = NULL;
    tWCD.ptActorSphereTreeNormal = NULL;

    tWCD.fTreatAsOneScene = WorldTreatAsOneSceneBySNO(snoWorld);

    tWCD.tWorldSyncedData.snoWeatherOverride = SNO_NONE;
    tWCD.tWorldSyncedData.flWeatherIntensityOverride = 0.0f;
    tWCD.tWorldSyncedData.dwWeatherIntensityOverrideEnd = 0;


    if (WorldIsDungeonFinderPlaceHolder(snoWorld) && tWCD.snoDungeonFinderSourceWorld == SNO_NONE)
    {

        if (GameActiveGameCommonDataIsServer())
        {
            tWCD.snoDungeonFinderSourceWorld = SWorldGetDungeonFinderTileset(tWCD.snoWorld);
        }
    }




    if (nOriginX == 0 && nOriginY == 0)
    {
        DRLGGetWorldOrigin(snoWorld, tWCD.pointWorldOrigin);
    }
    else
    {
        tWCD.pointWorldOrigin.x = nOriginX; tWCD.pointWorldOrigin.y = nOriginY;
    }



    NavmeshStitchingInitializeForWorld(tWCD, nStitchSizeInFeetX, nStitchSizeInFeetY, nWorldSizeInFeetX, nWorldSizeInFeetY);

    WorldCommonDataInitializePhysics(tWCD);

    InactiveMarkerInitializeForWorld(idSWorld);
}




void WorldCommonDataInitializePhysics(WorldCommonData & tWCD)
{


    ActorCollisionInitializeForWorld(tWCD);

    ACDCollInitializeForWorld(tWCD);
}




void WorldCommonDataDisposePhysics(WorldCommonData & tWCD)
{

    ActorCollisionDisposeForWorld(tWCD);
    ACDCollDisposeForWorld(tWCD);
}




void WorldCommonDataDispose(WorldCommonData & tWCD)
{









    WorldCommonDataDisposePhysics(tWCD);

    NavmeshStitchingDisposeForWorld(tWCD);




    InactiveMarkerDisposeForWorld(tWCD.idSWorld);
}




void WorldCacheSceneValues(SNO snoScene, SceneCachedValues & tCachedValues)
{
    int32 nMarkerSets;
    ScopedSceneDefinitionLockR ptSceneDef(snoScene);


    tCachedValues.dwCachedValuesValid = (1 << SCENE_NAVMESH_SIZE_X_VALID_BIT) | (1 << SCENE_NAVMESH_SIZE_Y_VALID_BIT) | (1 << SCENE_AABB_BOUNDS_VALID_BIT) | (1 << SCENE_AABB_MARKERSET_BOUNDS_VALID_BIT) | (1 << SCENE_NUM_NODES_VALID_BIT) | (1 << SCENE_FLAGS_VALID_BIT);
    tCachedValues.aabbBounds = ptSceneDef->aabbBounds;
    tCachedValues.aabbMarkerSetBounds = ptSceneDef->aabbMarkerSetBounds;
    tCachedValues.nNavmeshSizeX = ptSceneDef->tNavMeshData.nSizeX;
    tCachedValues.nNavmeshSizeY = ptSceneDef->tNavMeshData.nSizeY;

    for (int i = 0; i < 4; i++)
    {
        tCachedValues.arNumNodesForDistByLevelArea[i] = ptSceneDef->tNavMeshData.arNumNodesForDistByLevelArea[i];
    }



    tCachedValues.dwFlags = ptSceneDef->dwFlags;
    nMarkerSets = ptSceneDef->arMarkerSets.GetArraySize();

    for (int32 i = 0; i < nMarkerSets; i++)
    {
        SNO snoMarkerSet = ptSceneDef->arMarkerSets[i];
        SNOCheckSNOReference(SCENE_SNO, snoScene, MARKERSET_SNO, snoMarkerSet, NULL);
    }
}




void WorldCacheSceneValues(const WorldDefinition * pDefWorld)
{




    if (pDefWorld->uFlags & (1 << WDF_SCENE_VALUES_CACHED))
    {
        return;
    }



    if (pDefWorld->ptServerData && pDefWorld->ptServerData->ptScene)
    {
        for (int32 i = 0; i < pDefWorld->ptServerData->ptScene->nSceneChunkCount; i++)
        {



            SceneChunk & tSceneChunk = pDefWorld->ptServerData->ptScene->ptSceneChunks[i]; SceneCachedValues & tCachedValues = tSceneChunk.tSceneSpec.tCachedValues; SNOName & snoname = tSceneChunk.snoname;
            SNOFileExists(snoname);







            if (snoname.snoGroup == SCENEGROUP_SNO)
            {
                ScopedSceneGroupDefinitionLockR ptSceneGroup(snoname.snoHandle);




                tCachedValues.dwCachedValuesValid = (1 << SCENE_NAVMESH_SIZE_X_VALID_BIT) | (1 << SCENE_NAVMESH_SIZE_Y_VALID_BIT);
                tCachedValues.nNavmeshSizeX = 4;
                tCachedValues.nNavmeshSizeY = 4;

                if (ptSceneGroup->nSceneCnt > 0)
                {
                    SNO snoScene = ptSceneGroup->arScenes[0].snoScene;
                    ScopedSceneDefinitionLockR ptScene(snoScene);

                    tCachedValues.nNavmeshSizeX = ptScene->tNavMeshData.nSizeX;
                    tCachedValues.nNavmeshSizeY = ptScene->tNavMeshData.nSizeY;
                }
            }
            else
            {







                WorldCacheSceneValues(snoname.snoHandle, tCachedValues);
            }
        }
    }

    const_cast<WorldDefinition *>(pDefWorld)->uFlags |= (1 << WDF_SCENE_VALUES_CACHED);

}


















WorldCommonData & WorldGetCommonData(SWorldID idSWorld)
{

    if (GameActiveGameCommonDataIsClient())
    {
        CWorld * ptCWorld = CWorldGetFromSWorldID(idSWorld);

        return ptCWorld->tWCD;
    }
    else
    {


        SWorld * ptSWorld = SWorldGet(idSWorld);

        return ptSWorld->tWCD;
    }
}







WorldCommonData * WorldTryGetCommonData(SWorldID idSWorld)
{
    if (GameActiveGameCommonDataIsClient())
    {
        CWorld * ptCWorld = CWorldGetFromSWorldID(idSWorld);

        if (ptCWorld)
        {


            return &ptCWorld->tWCD;
        }
    }
    else
    {


        SWorld * ptSWorld = SWorldTryToGet(idSWorld);

        if (ptSWorld)
        {

            return &ptSWorld->tWCD;
        }
    }


    return NULL;
}































































































































BOOL WorldIsDRLG(const SNO snoWorld)
{
    BOOL fRetVal; ScopedWorldDefinitionLockR pdefWorld(snoWorld);

    fRetVal = pdefWorld->bDRLG;


    return fRetVal;
}






























SNO WorldCommonGetWorldSNO(SWorldID idSWorld)
{
    WorldCommonData & tWCD = WorldGetCommonData(idSWorld);
    return tWCD.snoWorld;
}
















WorldCommonData * WorldCommonDataArrayIterate(WorldCommonDataIterator & tRawIterator)
{


    if (GameActiveGameCommonDataIsClient())
    {
        CGame & tCGame = CGameGet();
        TDataArrayIterator<CWorld> & tIterator = (TDataArrayIterator<CWorld> &)tRawIterator;

        return tCGame.ptCWorlds->Iterate(tIterator) ? &tIterator->tWCD : NULL;
    }
    else
    {









        SGame & tSGame = SGameManager::GameGet();
        TDataArrayIterator<SWorld> & tIterator = (TDataArrayIterator<SWorld> &)tRawIterator;

        return tSGame.ptSWorlds->Iterate(tIterator) ? &tIterator->tWCD : NULL;
    }
}












SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete)
{

    WorldCommonDataIterator tIterator;
    WorldCommonData * ptWCD;



    while ((ptWCD = WorldCommonDataArrayIterate(tIterator)) != NULL)
    {
        if (bIgnorePendingDelete && SWorldIsFlaggedForDeletion(ptWCD->idSWorld))
        {
            continue;
        }



        if (ptWCD->snoWorld != snoWorld)
        {
            continue;
        }


        return ptWCD->idSWorld;
    }
    return SWORLDID_INVALID;
}







































void WorldClearStartLocationOverride()
{

    gStartLocationOverride.snoWorld = SNO_NONE;
}




static SSceneID sWorldFindSceneForOverride(const WorldPlace & tPlace)
{
    SceneCommonData * ptSCD = NULL;
    SceneCommonDataIterator tIterator;
    NavMeshHandle tNavMeshHandle;

    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes))
    {
        const Rect2D & rect = ptSCD->rect2DSceneWorld;
        const WorldDistance wdFudge = 10.0f;

        if (tPlace.wp.x > rect.left - wdFudge &&
            tPlace.wp.x < rect.right + wdFudge &&
            tPlace.wp.y > rect.top - wdFudge &&
            tPlace.wp.y < rect.bottom + wdFudge)
        {
            SceneCommonDataActivate(ptSCD->idSScene);
        }
    }


    ScenesCommonNavMeshNodeFromPlace(tPlace, &tNavMeshHandle, FALSE, FALSE);
    return tNavMeshHandle.idSScene;
}




BOOL WorldGetStartLocationOverride(const SWorldID idSWorld,
                                   WorldPlace & placeEntrance,
                                   Quaternion & qEntrance)

{

    WorldCommonData & tWCD = WorldGetCommonData(idSWorld);
    SNO snoWorld = tWCD.snoWorld;

    if (gStartLocationOverride.snoWorld != SNO_NONE && gStartLocationOverride.snoWorld == snoWorld)
    {



        placeEntrance.wp = gStartLocationOverride.placeEntrance.wp;

        if (gStartLocationOverride.fSceneEditModeWorldOrigin)
        {
            placeEntrance.wp.x -= tWCD.pointWorldOrigin.x;
            placeEntrance.wp.y -= tWCD.pointWorldOrigin.y;
        }

        placeEntrance.idSWorld = idSWorld;
        qEntrance = gStartLocationOverride.qEntrance;



        SSceneID idSScene = sWorldFindSceneForOverride(placeEntrance);

        if (idSScene != SSCENEID_INVALID)
        {
            return TRUE;
        }
    }

    WorldClearStartLocationOverride();
    return FALSE;
}





























BOOL WorldIsRotated(const SNO snoWorld)
{
    BOOL fIsRotated = FALSE;

    if (snoWorld != SNO_NONE)
    {

        if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_PVP_CRYPT_ARENA_01)) return TRUE;
    }




    return fIsRotated;
}



BOOL WorldTreatAsOneSceneBySNO(const SNO snoWorld)
{

    ScopedWorldDefinitionLockR ptWorldDef(snoWorld);
    return (ptWorldDef->uFlags & (1 << WDF_TREAT_AS_ONE_SCENE)) != 0;
}



BOOL WorldTreatAsOneScene(SWorldID idSWorld)
{
    WorldCommonData * ptWCD = WorldTryGetCommonData(idSWorld);

    if (ptWCD == NULL)
    {
        return FALSE;
    }


    return ptWCD->fTreatAsOneScene;
}


void WorldCommonPlayerSees(SWorldID idSWorld)
{
    WorldCommonData * ptWCD = WorldTryGetCommonData(idSWorld);

    if (ptWCD)
    {
        ptWCD->dwPlayerLastSeenTime = GameGetServerTime();
    }
}



int32 WorldGetDungeonFinderLevel(const SNO snoWorld)
{
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_01)) return 1;
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_02)) return 2;
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_03)) return 3;
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_04)) return 4;
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_05)) return 5;
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_06)) return 6;
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_07)) return 7;
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_08)) return 8;
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_09)) return 9;
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_10)) return 10;

    return -1;
}




SNO WorldGetDungeonFinderWorldFromLevel(int32 nLevel)
{

    switch (nLevel)
    {
    case 1: return GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_01);
    case 2: return GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_02);
    case 3: return GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_03);
    case 4: return GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_04);
    case 5: return GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_05);
    case 6: return GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_06);
    case 7: return GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_07);
    case 8: return GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_08);
    case 9: return GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_09);
    case 10: return GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_10);
    }


    return SNO_NONE;
}


BOOL WorldIsDungeonFinderPlaceHolder(const SNO snoWorld)
{

    return WorldGetDungeonFinderLevel(snoWorld) != -1;
}



BOOL WorldIsDungeonFinderBossWorld(const SNO snoWorld)
{

    ScopedWorldDefinitionLockR ptWorldDef(snoWorld);
    return ptWorldDef->uFlags & (1 << WDF_DUNGEON_FINDER_BOSSWORLD);
}



BOOL WorldIsDungeonFinderWorld(const SNO snoWorld)
{
    if (WorldIsDungeonFinderPlaceHolder(snoWorld)) return TRUE;










    return WorldIsDungeonFinderBossWorld(snoWorld) ? TRUE : FALSE;
}



SNO WorldGetDungeonFinderLevelArea(const SNO snoWorld)
{
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_01)) return GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_01);
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_02)) return GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_02);
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_03)) return GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_03);
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_04)) return GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_04);
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_05)) return GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_05);
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_06)) return GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_06);
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_07)) return GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_07);
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_08)) return GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_08);
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_09)) return GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_09);
    if (snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_10)) return GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_10);

    return SNO_NONE;
}





static int32 sSelectRandomStringIndex(const SNO snoStringList, RNG & tRNG)
{

    ScopedStringListDefinitionLockR pdefStringList(snoStringList);

    if (pdefStringList->dwStringCount == 0)
    {
        return -1;
    }

    return Rand_Internal(tRNG, pdefStringList->dwStringCount - 1);
}






static LPCSTR sSelectIndexedString(const SNO snoStringList, int32 nIndex)
{

    ScopedStringListDefinitionLockR pdefStringList(snoStringList);


    int32 nClamped = Math::Min<int32>(pdefStringList->dwStringCount - 1, nIndex);

    if (nIndex < 0) nClamped = 0;
    return pdefStringList->arStrings[nClamped].szText;
}




CRefString WorldGetDungeonFinderName(const int32 nPrefixIndex,
                                     const int32 nNounIndex,
                                     const int32 nLevel)

{
    CRefString sName;
    CRefString sNoun(sSelectIndexedString(GlobalSNOGet(GLOBALSNO_STRINGLIST_DUNGEONFINDER_NOUNS), nNounIndex));
    CRefString sPrefix(sSelectIndexedString(GlobalSNOGet(GLOBALSNO_STRINGLIST_DUNGEONFINDER_PREFIXES), nPrefixIndex));

    CRefString sName1, sName2;
    uint32 dwGrammaticalForm;
    uint32 dwExtractionFlags;
    LanguageRules::ExtractGrammaticalForm(sNoun, sName1, sName2, dwGrammaticalForm);
    LanguageRules::ExtractMatchingGrammaticalForm(sPrefix, dwGrammaticalForm, dwExtractionFlags);
    if ((dwGrammaticalForm & 0x40) && (dwExtractionFlags & 1))

    {
        LanguageRules::HandleConcatenation(sPrefix, sName1);
    }

    sName = LanguageRules::BuildItemName(sPrefix, sName1, CRefString(""), INC_LOOT_RUN, FALSE);

    sName += CRefString().Format(" %d", nLevel);


    return sName;
}



CRefString WorldGetDungeonFinderName(const int32 nSeed,
                                     const int32 nLevel)

{


    if (nSeed == 0)
    {
        return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "LootRunPortal");
    }

    RNG tRNG;
    tRNG.dwSeed = nSeed;
    tRNG.dwCarry = 0;

    int32 nNounIndex = gDungeonFinderNameOverride.nNounIndex;
    if (nNounIndex == -1)
    {
        nNounIndex = sSelectRandomStringIndex(GlobalSNOGet(GLOBALSNO_STRINGLIST_DUNGEONFINDER_NOUNS), tRNG);
    }
    int32 nPrefixIndex = gDungeonFinderNameOverride.nPrefixIndex;
    if (nPrefixIndex == -1)
    {
        nPrefixIndex = sSelectRandomStringIndex(GlobalSNOGet(GLOBALSNO_STRINGLIST_DUNGEONFINDER_PREFIXES), tRNG);
    }

    return WorldGetDungeonFinderName(nPrefixIndex, nNounIndex, nLevel);
}
