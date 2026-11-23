// Original file: X/Common/Nav/NavMeshStitch.cpp

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "NavMeshStitch.h"
#include "NavigationMesh.h"
#include "../Definitions/SceneDefinition.h"
#include "../Definitions/WorldDefinition.h"
#include "../Game.h"
#include "../GameCommonData.h"
#include "../SceneCommon.h"
#include "../TagMaps/Tags.h"
#include "../ThreadGlobals.h"
#include "../World.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include <math.h>

void NavMeshGetPositionByIndex(const NavMesh * ptNavMesh, int32 nSquareIndex, int32 & nSquareX, int32 & nSquareY);  // NavigationMesh.cpp:390
void DRLGGetDungeonStitchSize(const WorldCommonData & tWCD, int32 & nStitchSizeInFeetX, int32 & nStitchSizeInFeetY);  // X/Server/DRLGDungeon.cpp:10678
void DRLGGetWorldSize(SNO snoWorld, int32 & nMinTileSizeInFeetX, int32 & nMinTileSizeInFeetY, int32 & nWorldSizeInFeetX, int32 & nWorldSizeInFeetY);  // X/Common/DRLGWorld.cpp:404
int32 NavMeshAtSquareOffset(const NavMesh * ptNavMesh, int32 nSquareX, int32 nSquareY);  // NavigationMesh.cpp:1071

struct NavMeshWorldStitching {  // size 0x20
    int32 nTileSizeX;  // 0x00
    int32 nTileSizeY;  // 0x04
    int32 nGridTilesX;  // 0x08
    int32 nGridTilesY;  // 0x0c
    int32 nOffsetX;  // 0x10
    int32 nOffsetY;  // 0x14
    SSceneID * pidSceneArray;  // 0x18
};





static const int32 MAX_APPROX_WORLDS = 20;

typedef XChainedFixedBlockAllocator<NavMeshWorldStitching> TStitchingPool;

struct NavMeshStitchGlobals {  // size 0x30
    TStitchingPool tStitchingPool;  // 0x00
};

enum {
    NAV_MESH_STITCH_MAX_AABB_SCENES = 4096,
};

struct NavMeshStitchThreadGlobals {  // size 0x4020
    NavMeshStitchAABBLookup tNavMeshStitchAABBLookupResult;  // 0x00
    SSceneID arNavMeshStitchAABBLookupScenes[4096];  // 0x20
};



















void NavMeshStitchInitializeForGame()
{

    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    NavMeshStitchGlobals *& ptNavMeshStitchGlobals = tGCD.ptNavMeshStitchGlobals;
    ptNavMeshStitchGlobals = (NavMeshStitchGlobals *)SigmaMemoryNew(sizeof(NavMeshStitchGlobals), 4, ptMemoryInterface, TRUE);

    ptNavMeshStitchGlobals->tStitchingPool.Initialize();
    ptNavMeshStitchGlobals->tStitchingPool.SetMemoryInterface(ptMemoryInterface);
    ptNavMeshStitchGlobals->tStitchingPool.SetBlockCount_Tracked(MAX_APPROX_WORLDS);
}




void NavMeshStitchDisposeForGame()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    NavMeshStitchGlobals *& ptNavMeshStitchGlobals = tGCD.ptNavMeshStitchGlobals;

    ptNavMeshStitchGlobals->tStitchingPool.Dispose();

    SigmaMemoryFree(ptNavMeshStitchGlobals, AppGetCurrentMemoryInterface()); ptNavMeshStitchGlobals = NULL;
}




static SSceneID sNavMeshStitchArray(NavMeshWorldStitching * ptWStitch, int32 nArrayX, int32 nArrayY, BOOL bIncludeInactive)
{









    if (ptWStitch->pidSceneArray == NULL)
    {
        return SSCENEID_INVALID;
    }

    int32 nOffset = nArrayX + nArrayY * ptWStitch->nGridTilesX;

    SSceneID idSScene = ptWStitch->pidSceneArray[nOffset];

    if (!bIncludeInactive && idSScene != SSCENEID_INVALID)
    {

        SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
        if (ptSCD == NULL || ptSCD->ptNavMeshInst == NULL)
        {
            return SSCENEID_INVALID;
        }
    }

    return idSScene;
}




static void sNavMeshStitchPutSceneInWorldArray(uint32 worldFlags, NavMeshWorldStitching * ptWStitch, int32 x, int32 y, SSceneID idSScene, BOOL bSet)
{










    int32 nOffset = x + y * ptWStitch->nGridTilesX;

    if (ptWStitch->pidSceneArray[nOffset] != SSCENEID_INVALID)
    {
        if ((bSet || ptWStitch->pidSceneArray[nOffset] != idSScene) &&

            !(worldFlags & (1 << WDF_WORLD_USES_NON_UNIFORM_SCENE_SIZES)))
        {
            SceneCommonData & tSCD1 = SceneCommonDataGet(idSScene);
            SceneCommonData & tSCD2 = SceneCommonDataGet(ptWStitch->pidSceneArray[nOffset]);
        }
    }








    ptWStitch->pidSceneArray[nOffset] = bSet ? idSScene : SSCENEID_INVALID;
}




static void sNavMeshStitchSetWorldArray(SceneCommonData & tSCD, WorldCommonData & tWCD, BOOL bSet)
{

    NavMeshWorldStitching * ptWStitch = tWCD.ptWorldStitching;

    if (!ptWStitch)
    {
        return;
    }





    ScopedSceneDefinitionLockR ptDefScene(tSCD.snoScene);

    int32 nMinXGrid = ptWStitch->nOffsetX + tSCD.tNavMeshOffset.x;
    int32 nMinYGrid = ptWStitch->nOffsetY + tSCD.tNavMeshOffset.y;







    int32 nSceneSizeX = ptDefScene->tNavMeshData.nSizeX;
    int32 nSceneSizeY = ptDefScene->tNavMeshData.nSizeY;



    int32 nMod = nSceneSizeX % ptWStitch->nTileSizeX;

    if (nMod > 0)
    {
        nSceneSizeX += ptWStitch->nTileSizeX - nMod;
    }

    nMod = nSceneSizeY % ptWStitch->nTileSizeY;

    if (nMod > 0)
    {
        nSceneSizeY += ptWStitch->nTileSizeY - nMod;
    }




    int32 nMinX = nMinXGrid / ptWStitch->nTileSizeX;
    int32 nMinY = nMinYGrid / ptWStitch->nTileSizeY;
    int32 nMaxX = (nMinXGrid + nSceneSizeX) / ptWStitch->nTileSizeX;
    int32 nMaxY = (nMinYGrid + nSceneSizeY) / ptWStitch->nTileSizeY;

    nMinX = Math::Pin(nMinX, 0, ptWStitch->nGridTilesX);
    nMinY = Math::Pin(nMinY, 0, ptWStitch->nGridTilesY);
    nMaxX = Math::Pin(nMaxX, nMinX, ptWStitch->nGridTilesX);
    nMaxY = Math::Pin(nMaxY, nMinY, ptWStitch->nGridTilesY);


    ScopedWorldDefinitionLockR pdefWorld(tWCD.snoWorld);

    for (int32 x = nMinX; x < nMaxX; x++)
    {
        for (int32 y = nMinY; y < nMaxY; y++)
        {
            sNavMeshStitchPutSceneInWorldArray(pdefWorld->uFlags, ptWStitch, x, y, tSCD.idSScene, bSet);
        }
    }
}




void NavMeshStitchScene(SSceneID idSScene)
{

    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    WorldCommonData & tWCD = WorldGetCommonData(tSCD.idSWorld);

    sNavMeshStitchSetWorldArray(tSCD, tWCD, TRUE);
}




void NavMeshUnstitchScene(SSceneID idSScene)
{

    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    WorldCommonData & tWCD = WorldGetCommonData(tSCD.idSWorld);

    sNavMeshStitchSetWorldArray(tSCD, tWCD, FALSE);
}




BOOL NavMeshStitchLookup(SceneCommonData * ptSCD, const NavMeshHandle & tNavMeshHandle, int32 nAdjacentDirection, NavMeshHandle * pAdjacent)
{





    int32 nSquareX;
    int32 nSquareY;
    NavMeshGetPositionByIndex(ptSCD->ptNavMeshInst, tNavMeshHandle.nSquareIndex, nSquareX, nSquareY);



    WorldCommonData & tWCD = WorldGetCommonData(ptSCD->idSWorld);
    NavMeshWorldStitching * ptWStitch = tWCD.ptWorldStitching;

    if (!ptWStitch)
    {
        return FALSE;
    }

    int32 nTileSizeX = ptWStitch->nTileSizeX;
    int32 nTileSizeY = ptWStitch->nTileSizeY;

    switch (nAdjacentDirection)
    {
    case 0:
        if (nSquareX % nTileSizeX != 0)
            return FALSE;


        nSquareX--;
        break;
    case 2:

        if (nSquareX % nTileSizeX != nTileSizeX - 1)
            return FALSE;


        nSquareX++;
        break;
    case 1:

        if (nSquareY % nTileSizeY != nTileSizeY - 1)
            return FALSE;


        nSquareY++;
        break;
    case 3:

        if (nSquareY % nTileSizeY != 0)
            return FALSE;


        nSquareY--;
        break;
    default:

        DisplayInternalError(NULL, "", 334, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    int32 nWorldSpaceSquareX = ptSCD->tNavMeshOffset.x + nSquareX;
    int32 nWorldSpaceSquareY = ptSCD->tNavMeshOffset.y + nSquareY;



    int32 nArrayX = (nWorldSpaceSquareX + ptWStitch->nOffsetX) / ptWStitch->nTileSizeX;
    int32 nArrayY = (nWorldSpaceSquareY + ptWStitch->nOffsetY) / ptWStitch->nTileSizeY;

    if (nArrayX < 0 || nArrayY < 0 ||

        nArrayX >= ptWStitch->nGridTilesX ||
        nArrayY >= ptWStitch->nGridTilesY)
    {
        return FALSE;
    }


    SSceneID idSScene = sNavMeshStitchArray(ptWStitch, nArrayX, nArrayY, TRUE);

    if (idSScene == SSCENEID_INVALID)
    {
        return FALSE;
    }


    if (idSScene == tNavMeshHandle.idSScene)
    {
        return FALSE;
    }




    SceneCommonData & tAdjSCD = SceneCommonDataGet(idSScene);

    if (tAdjSCD.ptNavMeshInst == NULL)
    {
        return FALSE;
    }


    int32 nAdjSquareX = nWorldSpaceSquareX - tAdjSCD.tNavMeshOffset.x;
    int32 nAdjSquareY = nWorldSpaceSquareY - tAdjSCD.tNavMeshOffset.y;

    int32 nSquareIndex = NavMeshAtSquareOffset(tAdjSCD.ptNavMeshInst, nAdjSquareX, nAdjSquareY);




    if (nSquareIndex == -1)
    {
        return FALSE;
    }


    pAdjacent->nSquareIndex = nSquareIndex; pAdjacent->idSScene = idSScene;

    return TRUE;
}















static void sAllocStitchingArray(NavMeshWorldStitching * ptWStitch, int nNumTiles, SNO snoWorld)
{





    ptWStitch->pidSceneArray = (SSceneID *)SigmaMemoryNew(nNumTiles * sizeof(SSceneID), 4, NULL, FALSE);

    for (int32 i = 0; i < nNumTiles; i++)
    {
        ptWStitch->pidSceneArray[i] = SSCENEID_INVALID;
    }
}




void NavmeshStitchingSetSizeForDRLGWorld(WorldCommonData & tWCD, int nTilesX, int nTilesY)
{





    ScopedWorldDefinitionLockR ptWorldDef(tWCD.snoDungeonFinderSourceWorld != SNO_NONE ? tWCD.snoDungeonFinderSourceWorld : tWCD.snoWorld);


    if (ptWorldDef->ptServerData->ptDRLG)
    {
        NavMeshWorldStitching * ptWStitch = tWCD.ptWorldStitching;


        int nXTileSizeFeet = TagMapGetInt(ptWorldDef->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_TILE_SIZE_X);
        int nYTileSizeFeet = TagMapGetInt(ptWorldDef->ptServerData->ptDRLG->hDRLGTagMap, TAG_WORLD_TILE_SIZE_Y);


        int nGridTilesX = Math::FloatRoundToInt(nXTileSizeFeet / cflGridCellWidth);
        int nGridTilesY = Math::FloatRoundToInt(nYTileSizeFeet / cflGridCellWidth);


        int nGridX = nGridTilesX * nTilesX;
        int nGridY = nGridTilesY * nTilesY;


        int nCellsX = nGridX / ptWStitch->nTileSizeX;
        int nCellsY = nGridY / ptWStitch->nTileSizeY;





        nCellsX = Math::Max(nCellsX, 1);
        nCellsY = Math::Max(nCellsY, 1);




        int nTotalCells = nCellsX * nCellsY;

        ptWStitch->nGridTilesX = nCellsX;
        ptWStitch->nGridTilesY = nCellsY;

        sAllocStitchingArray(ptWStitch, nTotalCells, tWCD.snoWorld);
    }
}


// NON_MATCHING: 4 bytes short. The original keeps nNumTiles in two registers (mul w21 then mov w20, w21) before the
// inlined sAllocStitchingArray; ours uses one. The flag polarity (set from IsServer, tested with !) is what the code shows.
void NavmeshStitchingInitializeForWorld(WorldCommonData & tWCD, int32 nStitchSizeInFeetX, int32 nStitchSizeInFeetY, int32 nWorldSizeInFeetX, int32 nWorldSizeInFeetY)
{





    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (tGCD.GameModeGet() != GAMEMODE_PLAYING)
    {
        return;
    }

    NavMeshStitchGlobals * ptNavMeshStitchGlobals = tGCD.ptNavMeshStitchGlobals;

    tWCD.ptWorldStitching = ptNavMeshStitchGlobals->tStitchingPool.Alloc_Tracked();

    SigmaMemoryClear(tWCD.ptWorldStitching, sizeof(NavMeshWorldStitching));

    NavMeshWorldStitching * ptWStitch = tWCD.ptWorldStitching;

    ScopedWorldDefinitionLockR pdefWorld(tWCD.snoWorld);
    BOOL bAllocStitchingArray = FALSE;

    if (pdefWorld->bDRLG)
    {



        DRLGGetDungeonStitchSize(tWCD, nStitchSizeInFeetX, nStitchSizeInFeetY);





        ptWStitch->nTileSizeX = Math::FloatRoundToInt(nStitchSizeInFeetX / cflGridCellWidth);
        ptWStitch->nTileSizeY = Math::FloatRoundToInt(nStitchSizeInFeetY / cflGridCellWidth);








        ptWStitch->nGridTilesX = 50; ptWStitch->nGridTilesY = 50;


        ptWStitch->nOffsetX = 0; ptWStitch->nOffsetY = 0;


        bAllocStitchingArray = GameActiveGameCommonDataIsServer();
    }
    else
    {







        if (pdefWorld->ptServerData && pdefWorld->ptServerData->ptScene)
        {
            DRLGGetWorldSize(tWCD.snoWorld, nStitchSizeInFeetX, nStitchSizeInFeetY, nWorldSizeInFeetX,
                             nWorldSizeInFeetY);
        }






        ptWStitch->nTileSizeX = Math::FloatRoundToInt(nStitchSizeInFeetX / cflGridCellWidth);
        ptWStitch->nTileSizeY = Math::FloatRoundToInt(nStitchSizeInFeetY / cflGridCellWidth);

        if (ptWStitch->nTileSizeX <= 0 || ptWStitch->nTileSizeY <= 0)
        {
            return;
        }






        int32 nWorldSizeInNodesX = Math::FloatRoundToInt(nWorldSizeInFeetX / cflGridCellWidth);
        int32 nWorldSizeInNodesY = Math::FloatRoundToInt(nWorldSizeInFeetY / cflGridCellWidth);

        ptWStitch->nOffsetX = 0; ptWStitch->nOffsetY = 0;


        ptWStitch->nGridTilesX = nWorldSizeInNodesX / ptWStitch->nTileSizeX + 1;
        ptWStitch->nGridTilesY = nWorldSizeInNodesY / ptWStitch->nTileSizeY + 1;
    }

    ptWStitch->nTileSizeX = Math::Max(ptWStitch->nTileSizeX, 1);
    ptWStitch->nTileSizeY = Math::Max(ptWStitch->nTileSizeY, 1);
    ptWStitch->nGridTilesX = Math::Max(ptWStitch->nGridTilesX, 1);
    ptWStitch->nGridTilesY = Math::Max(ptWStitch->nGridTilesY, 1);

    int32 nNumTiles = ptWStitch->nGridTilesX * ptWStitch->nGridTilesY;







    if (!bAllocStitchingArray)
    {
        sAllocStitchingArray(ptWStitch, nNumTiles, tWCD.snoWorld);
    }
}




void NavmeshStitchingDisposeForWorld(WorldCommonData & tWCD)
{

    NavMeshWorldStitching * ptWStitch = tWCD.ptWorldStitching;

    if (ptWStitch)
    {



        GameCommonData & tGCD = GameGetActiveGameCommonData();
        NavMeshStitchGlobals * ptNavMeshStitchGlobals = tGCD.ptNavMeshStitchGlobals;

        SigmaMemoryFree(tWCD.ptWorldStitching->pidSceneArray, NULL); tWCD.ptWorldStitching->pidSceneArray = NULL;

        ptNavMeshStitchGlobals->tStitchingPool.Free(tWCD.ptWorldStitching);
    }
}



static NavMeshStitchThreadGlobals * sNavMeshStitchGetThreadGlobals()
{

    NavMeshStitchThreadGlobals *& ptNavMeshStitchThreadGlobals = ThreadGlobals::Get().m_ptNavMeshStitchThreadGlobals;


    if (ptNavMeshStitchThreadGlobals == NULL)
    {
        ptNavMeshStitchThreadGlobals = (NavMeshStitchThreadGlobals *)SigmaMemoryNew(sizeof(NavMeshStitchThreadGlobals), 4, NULL, TRUE);
    }

    return ptNavMeshStitchThreadGlobals;
}




void NavmeshSitchingDisposeThreadGlobals()
{

    NavMeshStitchThreadGlobals *& ptNavMeshStitchThreadGlobals = ThreadGlobals::Get().m_ptNavMeshStitchThreadGlobals;

    if (ptNavMeshStitchThreadGlobals)
    {
        SigmaMemoryFree(ptNavMeshStitchThreadGlobals, NULL); ptNavMeshStitchThreadGlobals = NULL;
    }
}









































































































































































static void sPlaceToArrayXY(NavMeshWorldStitching * ptWStitch, Vector2D & wpPos, int32 & nArrayX, int32 & nArrayY)
{




    int32 nWorldCellX = Math::FloatFloorToInt(wpPos.x / cflGridCellWidth);
    int32 nWorldCellY = Math::FloatFloorToInt(wpPos.y / cflGridCellWidth);



    if (nWorldCellX >= 0)
    {
        nArrayX = nWorldCellX / ptWStitch->nTileSizeX;
        nArrayX -= ptWStitch->nOffsetX;

        if (nArrayX > ptWStitch->nGridTilesX) nArrayX = ptWStitch->nGridTilesX;
    }
    else
    {
        nArrayX = -1;
    }



    if (nWorldCellY >= 0)
    {
        nArrayY = nWorldCellY / ptWStitch->nTileSizeY;
        nArrayY -= ptWStitch->nOffsetY;

        if (nArrayY > ptWStitch->nGridTilesY) nArrayY = ptWStitch->nGridTilesY;
    }
    else
    {
        nArrayY = -1;
    }
}






SSceneID NavMeshStitchWorldPlaceToScene(WorldCommonData & tWCD, const WorldPlace & tWorldPlace, BOOL bIncludeInactive)
{




    SSceneID idSScene = SSCENEID_INVALID;
    NavMeshWorldStitching * ptWStitch = tWCD.ptWorldStitching;

    if (ptWStitch)
    {
        if (ptWStitch->nTileSizeX > 0 &&
            ptWStitch->nTileSizeY > 0)
        {







            int32 nArrayX, nArrayY;

            sPlaceToArrayXY(ptWStitch, (Vector2D &)tWorldPlace.wp, nArrayX, nArrayY);

            if (nArrayX >= 0 && nArrayY >= 0 &&
                nArrayX < ptWStitch->nGridTilesX &&
                nArrayY < ptWStitch->nGridTilesY)
            {
                idSScene = sNavMeshStitchArray(ptWStitch, nArrayX, nArrayY, bIncludeInactive);
            }
        }
    }

    return idSScene;
}




NavMeshStitchAABBLookup * NavMeshStitchScenesInAABB(SWorldID idSWorld, Vector2D wpAABBMin, Vector2D wpAABBMax)
{



    WorldCommonData & tWCD = WorldGetCommonData(idSWorld);
    NavMeshWorldStitching * ptWStitch = tWCD.ptWorldStitching;

    NavMeshStitchThreadGlobals * ptThreadGlobals = sNavMeshStitchGetThreadGlobals();

    NavMeshStitchAABBLookup & tLookupResult = ptThreadGlobals->tNavMeshStitchAABBLookupResult;

    tLookupResult.tSize.Set(0, 0);

    if (ptWStitch)
    {

        int32 nArrayMinX, nArrayMinY, nArrayMaxX, nArrayMaxY;

        sPlaceToArrayXY(ptWStitch, wpAABBMin, nArrayMinX, nArrayMinY);
        sPlaceToArrayXY(ptWStitch, wpAABBMax, nArrayMaxX, nArrayMaxY);

        tLookupResult.wpGridOffset.Set(nArrayMinX * ptWStitch->nTileSizeX * cflGridCellWidth,
                                       nArrayMinY * ptWStitch->nTileSizeY * cflGridCellWidth);
        tLookupResult.vGridSize.Set(ptWStitch->nTileSizeX * cflGridCellWidth, ptWStitch->nTileSizeY * cflGridCellWidth);
        tLookupResult.tSize.Set(nArrayMaxX - nArrayMinX + 1, nArrayMaxY - nArrayMinY + 1);

        tLookupResult.arScenes = ptThreadGlobals->arNavMeshStitchAABBLookupScenes;

        if (tLookupResult.tSize.x * tLookupResult.tSize.y >= NAV_MESH_STITCH_MAX_AABB_SCENES)
        {
            return NULL;
        }












        uint32 uCount = 0;

        for (int y = nArrayMinY; y <= nArrayMaxY; y++)
        {
            for (int x = nArrayMinX; x <= nArrayMaxX; x++)
            {
                SSceneID idSScene = SSCENEID_INVALID;



                if (x >= 0 && y >= 0 &&
                    x < ptWStitch->nGridTilesX &&
                    y < ptWStitch->nGridTilesY)
                {
                    idSScene = sNavMeshStitchArray(ptWStitch, x, y, FALSE);
                }




















                tLookupResult.arScenes[uCount] = idSScene;
                uCount++;
            }
        }
    }










    return &tLookupResult;
}



int32 NavMeshStitchScenesInAABB(SWorldID idSWorld, Vector2D wpAABBMin, Vector2D wpAABBMax, SSceneID * arSceneIDs, int32 nMaxSceneIDCount)
{





    WorldCommonData & tWCD = WorldGetCommonData(idSWorld);
    NavMeshWorldStitching * ptWStitch = tWCD.ptWorldStitching;
    int nSceneIntersectCount = 0;

    if (ptWStitch)
    {
        int32 nArrayMinX, nArrayMaxX, nArrayMinY, nArrayMaxY;

        sPlaceToArrayXY(ptWStitch, wpAABBMin, nArrayMinX, nArrayMinY);
        sPlaceToArrayXY(ptWStitch, wpAABBMax, nArrayMaxX, nArrayMaxY);



        nArrayMinX = Math::Pin(nArrayMinX, 0, ptWStitch->nGridTilesX - 1);
        nArrayMaxX = Math::Pin(nArrayMaxX, 0, ptWStitch->nGridTilesX - 1);
        nArrayMinY = Math::Pin(nArrayMinY, 0, ptWStitch->nGridTilesY - 1);
        nArrayMaxY = Math::Pin(nArrayMaxY, 0, ptWStitch->nGridTilesY - 1);

        for (int y = nArrayMinY; y <= nArrayMaxY; y++)
        {
            for (int x = nArrayMinX; x <= nArrayMaxX; x++)
            {

                SSceneID idSScene = sNavMeshStitchArray(ptWStitch, x, y, TRUE);

                if (idSScene != SSCENEID_INVALID)
                {
                    BOOL bFound = FALSE;

                    for (int i = 0; i < nSceneIntersectCount; i++)
                    {
                        if (arSceneIDs[i] == idSScene)
                        {
                            bFound = TRUE;
                            break;
                        }
                    }

                    if (!bFound)
                    {
                        arSceneIDs[nSceneIntersectCount] = idSScene;
                        nSceneIntersectCount++;

                        if (nSceneIntersectCount >= nMaxSceneIDCount)
                        {
                            break;
                        }
                    }
                }
            }
        }
    }

    return nSceneIntersectCount;
}



int32 NavMeshStitchScenesInLineSegment(SWorldID idSWorld, Vector2D wpStart, Vector2D wpEnd, SSceneID * arSceneIDs, int32 nMaxSceneIDCount)
{





    WorldCommonData & tWCD = WorldGetCommonData(idSWorld);
    NavMeshWorldStitching * ptWStitch = tWCD.ptWorldStitching;
    int nSceneIntersectCount = 0;

    if (ptWStitch)
    {




        float flTileSizeX = ptWStitch->nTileSizeX * cflGridCellWidth;
        float flTileSizeY = ptWStitch->nTileSizeY * cflGridCellWidth;


        int i = Math::FloatFloorToInt(wpStart.x / flTileSizeX);
        int j = Math::FloatFloorToInt(wpStart.y / flTileSizeY);


        int iend = Math::FloatFloorToInt(wpEnd.x / flTileSizeX);
        int jend = Math::FloatFloorToInt(wpEnd.y / flTileSizeY);


        int di = ((wpStart.x < wpEnd.x) ? 1 : ((wpStart.x > wpEnd.x) ? -1 : 0));
        int dj = ((wpStart.y < wpEnd.y) ? 1 : ((wpStart.y > wpEnd.y) ? -1 : 0));

        float adx = Math::Abs(wpEnd.x - wpStart.x);
        float ady = Math::Abs(wpEnd.y - wpStart.y);





        float minx = flTileSizeX * floorf(wpStart.x / flTileSizeX);
        float maxx = minx + flTileSizeX;
        float tx;
        float deltatx;
        if (adx > EPSILON)
        {
            tx = ((wpStart.x > wpEnd.x) ? (wpStart.x - minx) : (maxx - wpStart.x)) / adx;



            deltatx = flTileSizeX / adx;
        }
        else
        {
            tx = 0.0f;
            deltatx = 0.0f;
        }

        float miny = flTileSizeY * floorf(wpStart.y / flTileSizeY);
        float maxy = miny + flTileSizeY;
        float ty;
        float deltaty;
        if (ady > EPSILON)
        {
            ty = ((wpStart.y > wpEnd.y) ? (wpStart.y - miny) : (maxy - wpStart.y)) / ady;



            deltaty = flTileSizeY / ady;
        }
        else
        {
            ty = 0.0f;
            deltaty = 0.0f;
        }
        SSceneID idSSceneLastIsect = SSCENEID_INVALID;
        int nTilesTotal = ptWStitch->nGridTilesY * ptWStitch->nGridTilesX;

        for (;;)
        {
            SSceneID idSSceneCur = SSCENEID_INVALID;



















            if (j >= 0 && j < ptWStitch->nGridTilesY &&
                i >= 0 && i < ptWStitch->nGridTilesX)
            {
                int nOffset = i + j * ptWStitch->nGridTilesX;
                if (nOffset >= 0 && nOffset < nTilesTotal)
                {
                    idSSceneCur = sNavMeshStitchArray(ptWStitch, i, j, FALSE);
                }
            }




            if (idSSceneCur != idSSceneLastIsect)
            {
                arSceneIDs[nSceneIntersectCount] = idSSceneCur;
                idSSceneLastIsect = idSSceneCur;
                nSceneIntersectCount++;

                if (nSceneIntersectCount >= nMaxSceneIDCount)
                {
                    break;
                }
            }

            if (tx <= ty)
            {
                if (i == iend)
                    break;
                tx += deltatx;
                i += di;
            }
            else
            {

                if (j == jend)
                    break;
                ty += deltaty;
                j += dj;
            }
        }
    }
    return nSceneIntersectCount;
}



BOOL NavMeshStitchVerifyTileSize(const SceneCommonData & tSCD)
{

    WorldCommonData & tWCD = WorldGetCommonData(tSCD.idSWorld);
    NavMeshWorldStitching * ptWStitch = tWCD.ptWorldStitching;
    if (ptWStitch)
    {
        if (ptWStitch->nTileSizeX <= 0 ||
            ptWStitch->nTileSizeY <= 0)
        {
            return FALSE;
        }
    }








    return TRUE;
}
