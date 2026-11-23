// Original file: X/Common/Nav/NavigationMesh.cpp
//
// Functions sit on their original lines; the assert in NavMeshGetAdjacentSquare bakes in 1606.

#include "NavigationMesh.h"

#include "NavCellHandle.h"
#include "NavMeshInternal.h"
#include "NavZone.h"
#include "../ActorCommonData.h"
#include "../Definitions/ActorDefinition.h"
#include "../Definitions/SceneDefinition.h"
#include "../Game.h"
#include "../GameCommonData.h"
#include "../SceneCommon.h"
#include "../Target.h"
#include "../WorldCommonCollision.h"
#include "../WorldPlace.h"
#include "../../Server/SScene.h"
#include "../../Server/SWorld.h"
// For the RGBAColor normals, which the original also initializes here; it reaches GfxVB.h
// through an include chain the DWARF does not record.
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/GamePerf.h"
#include "../../Sigma/Math/Intersect.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Ray.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Math/Triangle.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <float.h>
#include <stddef.h>

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// The field names are "" in the binary.







static const TypeDescriptorField NavMeshSquare_Fields[3] = { TypeDescriptorField("", DT_FLOAT, offsetof(NavMeshSquare, wdSquareZ), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(NavMeshSquare, dwSquareFlags), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(NavMeshSquare), NULL, 0) };


ComplexTypeDescriptor TypeDescriptor_NavMeshSquare("NavMeshSquare", 0, NavMeshSquare_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_NAVMESH_SQUARE = &TypeDescriptor_NavMeshSquare;

static const TypeDescriptorField CompNavMeshSquare_Fields[3] = { TypeDescriptorField("", DT_WORD, offsetof(CompNavMeshSquare, wdSquareZ), NULL, 1), TypeDescriptorField("", DT_WORD, offsetof(CompNavMeshSquare, dwSquareFlags), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(CompNavMeshSquare), NULL, 0) };


ComplexTypeDescriptor TypeDescriptor_CompNavMeshSquare("CompNavMeshSquare", 0, CompNavMeshSquare_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_COMP_NAVMESH_SQUARE = &TypeDescriptor_CompNavMeshSquare;

static const TypeDescriptorField NavMeshConnectedSquare_Fields[5] = {
    TypeDescriptorField("", DT_NAVMESH_SQUARE, offsetof(NavMeshConnectedSquare, tSquare), NULL, 1), FixedArrayTypeDescriptorField("", DT_INT, offsetof(NavMeshConnectedSquare, nSquareIndices), NULL, 1, 4, 0),
    TypeDescriptorField("", DT_INT, offsetof(NavMeshConnectedSquare, nSquareX), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(NavMeshConnectedSquare, nSquareY), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(NavMeshConnectedSquare), NULL, 0) };

ComplexTypeDescriptor TypeDescriptor_NavMeshConnectedSquare("NavMeshConnectedSquare", 0, NavMeshConnectedSquare_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_NAVMESH_CONNECTED_SQUARE = &TypeDescriptor_NavMeshConnectedSquare;






static const float FAT_MARBLE_SIZE = 0.1f;

// Defined in X/Common/Game.cpp; no adopted header declares them yet.
XMemoryAllocatorInterface * AppGetCurrentMemoryInterface();
BOOL GameActiveGameCommonDataIsServer();

// Defined in X/Sigma/GamePerf.cpp; no adopted header declares them yet.
POS GamePerfCtrAdd(LPCSTR szName, const BOOL fIsCounter, XMemoryAllocatorInterface * ptMemoryInterface);
void GamePerfCtrDelete(POS pos, XMemoryAllocatorInterface * ptMemoryInterface);
void GamePerfCtrIncrement(POS pos, int32 nAmount);

// Defined in X/Common/Nav/NavMeshStitch.cpp; no adopted header declares them yet.
void NavMeshStitchInitializeForGame();
void NavMeshStitchDisposeForGame();
BOOL NavMeshStitchLookup(SceneCommonData * ptSCD, const NavMeshHandle & tNavMeshHandle, int32 nAdjacentDirection, NavMeshHandle * ptAdjacent);

// Defined in X/Common/SceneCommon.cpp and X/Common/World.cpp; no adopted header declares them
// yet.
void SceneGetSceneToWorldTransform(SSceneID idSScene, PRTransform & transformSceneToWorld);
SWorldID SceneCommonGetSWorldID(SSceneID idSScene);
SNO SceneCommonGetSceneSNO(SSceneID idSScene);
BOOL ScenesCommonNavMeshNodeFromPlace(const WorldPlace & tPlace, NavMeshHandle * ptNavMeshHandle, BOOL bAllowInactive, BOOL bSpecial);
SNO WorldCommonGetWorldSNO(SWorldID idSWorld);
BOOL WorldIsDungeonFinderPlaceHolder(SNO snoWorld);
SNO WorldGetDungeonFinderLevelArea(SNO snoWorld);

// Defined in X/Server/System/LevelArea.cpp; no adopted header declares it yet.
LevelAreaID LevelAreaFind(SNO snoLevelArea, SWorldID idSWorld);

// Defined in X/Common/WorldCommonCollision.cpp; no adopted header declares them yet.
BOOL WorldCollSearchForLocation(const WorldPlace & placeStart, uint16 uOneRequiredStaticFlags, uint32 uObjectNoOverlapFlags, float flRadius, float flMinSearchRadius, float flMaxSearchRadius, ACDID idACDIgnore, Vector3D & wpResult, NavCellHandle & hNavCellResult, Team eTeam, uint16 uStaticPreventFlags, ACDID idACDIgnore2);
BOOL WorldCollSweptPoint(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Vector2D & wpStart, const Vector2D & wpEnd, const WorldCollParams & tParams, uint32 dwFlags, uint32 dwFlags2, ACDID idACDIgnore, WorldCollResult & tResult);

// Defined in X/Common/Powers/PowersCommon.cpp; no adopted header declares it yet.
BOOL PowerRollToTargetNavPlace(ACDID idACD, const Target & tTarget, int32 nFlags, BOOL bIgnoreDestinationHeight, NavPlace & tNavPlace);

struct NavMeshGlobals {  // size 0x98
    TPathPointPool tPathPointPool;  // 0x00
    TNavMeshHandlePool tNavMeshHandlePool;  // 0x30
    TNavMeshFloodHandlePool tNavMeshFloodHandlePool;  // 0x60
    POS gPathingRollingCounter;  // 0x90
};
static_assert(sizeof(NavMeshGlobals) == 0x98, "NavMeshGlobals");

struct NavMeshSquare3D {  // size 0xc
    Vector3D wp;  // 0x00
};
static_assert(sizeof(NavMeshSquare3D) == 0xc, "NavMeshSquare3D");

// NAVMESH_HANDLE_NONE is defined in its original owning CommonTypes.h.

void NavMeshGetPositionByIndex(const NavMesh * ptNavMesh, int32 nSquareIndex, int32 & nSquareX, int32 & nSquareY);
NavMeshSquare NavMeshIndexToSquare(const NavMeshDef * ptNavMeshDef, int32 nSquareIndex, BOOL fSpecialOnly);
uint32 NavMeshIndexToStaticFlags(const NavMeshInst * ptNavMeshInst, int32 nSquareIndex, BOOL fSpecialOnly);
uint32 NavMeshHandleToStaticFlags(const NavMeshHandle & tNavMeshHandle, BOOL fSpecialOnly);
NavMeshInst * NavMeshInstGetByScene(SSceneID idSScene);
BOOL NavMeshHandleIsNone(const NavMeshHandle & tNavMeshHandle);
uint32 NavMeshGetStaticFlagsForPassability(PassabilityType ePassability);
uint32 NavMeshGetDynamicFlagsForPassability(PassabilityType ePassability);
void sNavMeshGetAdjacentHandle(SceneCommonData * ptSCD, const NavMeshHandle & tNavMeshHandle, int32 nAdjacentDirection, NavMeshHandle & tAdjacent);
BOOL NavMeshHandleCanEnter(const NavMeshHandle & tNavMeshHandle, const PassabilityType ePassabilityType);
uint32 NavMeshHandleGetStaticFlags(const NavMeshHandle & tNavMeshHandle);






















void NavMeshInitializeForGame()
{
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    NavMeshGlobals *& ptNavMeshGlobals = GameGetActiveGameCommonData().ptNavMeshGlobals;
    ptNavMeshGlobals = (NavMeshGlobals *)SigmaMemoryNew(sizeof(NavMeshGlobals), 4, ptMemoryInterface, TRUE);
    ptNavMeshGlobals->tPathPointPool.Initialize();
    ptNavMeshGlobals->tPathPointPool.SetMemoryInterface(ptMemoryInterface);
    ptNavMeshGlobals->tPathPointPool.SetBlockCount_Tracked(1000);
    ptNavMeshGlobals->tNavMeshHandlePool.Initialize();
    ptNavMeshGlobals->tNavMeshHandlePool.SetMemoryInterface(ptMemoryInterface);
    ptNavMeshGlobals->tNavMeshHandlePool.SetBlockCount_Tracked(1000);
    ptNavMeshGlobals->tNavMeshFloodHandlePool.Initialize();
    ptNavMeshGlobals->tNavMeshFloodHandlePool.SetMemoryInterface(ptMemoryInterface);
    ptNavMeshGlobals->tNavMeshFloodHandlePool.SetBlockCount_Tracked(1000);

    ptNavMeshGlobals->gPathingRollingCounter = GamePerfCtrAdd("Pathing Rolling", TRUE, ptMemoryInterface);

    NavMeshStitchInitializeForGame();
}





void NavMeshDisposeForGame()
{
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    NavMeshStitchDisposeForGame();

    NavMeshGlobals *& ptNavMeshGlobals = GameGetActiveGameCommonData().ptNavMeshGlobals;
    if (ptNavMeshGlobals) {
        GamePerfCtrDelete(ptNavMeshGlobals->gPathingRollingCounter, ptMemoryInterface);
        ptNavMeshGlobals->tPathPointPool.Dispose();
        ptNavMeshGlobals->tNavMeshHandlePool.Dispose();
        ptNavMeshGlobals->tNavMeshFloodHandlePool.Dispose();
        SigmaMemoryFree(ptNavMeshGlobals, ptMemoryInterface);
        ptNavMeshGlobals = NULL;
    }
}




void NavMeshInitInst(SSceneID idSScene)
{
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    ScopedSceneDefinitionLockR pdefScene(tSCD.snoScene);
    if (pdefScene) {
        *(NavMesh *)tSCD.ptNavMeshInst = pdefScene->tNavMeshData;
        tSCD.ptNavMeshInst->idSScene = idSScene;
    }
}












void NavMeshFreeInst(SceneCommonData & tSCD)
{
}

























































void NavMeshSquare3DGetBounds(const NavMeshSquare3D & wsquare, WorldDistance (&wdSquareBounds)[4])
{
    wdSquareBounds[ADJ_NEG_X] = wsquare.wp.x;
    wdSquareBounds[ADJ_POS_Y] = wsquare.wp.y + 2.5f;
    wdSquareBounds[ADJ_POS_X] = wsquare.wp.x + 2.5f;
    wdSquareBounds[ADJ_NEG_Y] = wsquare.wp.y;
}






static WorldDistance sNavMeshGetHeightByIndex(const NavMeshDef * ptNavMeshDef, int32 nSquareIndex)
{
    return NavMeshIndexToSquare(ptNavMeshDef, nSquareIndex, FALSE).wdSquareZ;
}










static WorldDistance sNavMeshGetHeightByIndex(SceneCommonData * ptSCD, const SceneDefinition * pdefScene, int32 nSquareIndex)
{
    if (ptSCD == NULL || ptSCD->ptNavMeshInst == NULL) {
        return 0.0f;
    }

    if (ptSCD->tChildSceneIDs.Count() > 0) {
        int32 nParentSpaceX;
        int32 nParentSpaceY;
        NavMeshGetPositionByIndex(ptSCD->ptNavMeshInst, nSquareIndex, nParentSpaceX, nParentSpaceY);

        POS pos = ptSCD->tChildSceneIDs.GetHeadPosition();
        while (pos) {
            SSceneID idChildScene = ptSCD->tChildSceneIDs.GetNext(pos);
            SceneCommonData * ptSCDChild = SceneCommonDataTryToGet(idChildScene);
            if (ptSCDChild) {
                Vector3D wpOffset = ptSCDChild->transformWorld.wp - ptSCD->transformWorld.wp;

                ScopedSceneDefinitionLockR pdefChild(ptSCDChild->snoScene);
                int32 nChildSizeX = pdefChild->tNavMeshData.nSizeX;
                int32 nChildSizeY = pdefChild->tNavMeshData.nSizeY;

                IVector2D tOffset;
                tOffset.x = (int32)(wpOffset.x / 2.5f);
                tOffset.y = (int32)(wpOffset.y / 2.5f);

                if (nParentSpaceX >= tOffset.x && nParentSpaceX < tOffset.x + nChildSizeX &&
                    nParentSpaceY >= tOffset.y && nParentSpaceY < tOffset.y + nChildSizeY) {
                    nSquareIndex = (nParentSpaceY - tOffset.y) * nChildSizeX + (nParentSpaceX - tOffset.x);
                    return wpOffset.z + sNavMeshGetHeightByIndex(&pdefChild->tNavMeshData, nSquareIndex);
                }
            }
        }
    }

    return sNavMeshGetHeightByIndex(&pdefScene->tNavMeshData, nSquareIndex);
}





















static WorldDistance sNavMeshGetHeightByIndex(const NavMeshInst * ptNavMeshInst, int32 nSquareIndex)
{
    WorldDistance wdHeight = 0.0f;
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(ptNavMeshInst->idSScene);
    if (ptSCD && ptSCD->ptNavMeshInst) {
        ScopedSceneDefinitionLockR pdefScene(ptSCD->snoScene);
        wdHeight = sNavMeshGetHeightByIndex(ptSCD, pdefScene, nSquareIndex);
    }
    return wdHeight;
}













void NavMeshGetPositionByIndex(const NavMesh * ptNavMesh, int32 nSquareIndex, int32 & nSquareX, int32 & nSquareY)
{
    nSquareX = nSquareIndex % ptNavMesh->nSizeX;
    nSquareY = nSquareIndex / ptNavMesh->nSizeX;
}











int32 NavMeshGetIndexByPosition(const NavMesh * ptNavMesh, int32 nSquareX, int32 nSquareY)
{
    return nSquareY * ptNavMesh->nSizeX + nSquareX;
}































static void sNavMeshGetSquareSceneSpace(const NavMeshInst * ptNavMeshInst, int32 nSquareIndex, NavMeshSquare3D & wsquare)
{
    int32 nSquareX;
    int32 nSquareY;
    NavMeshGetPositionByIndex(ptNavMeshInst, nSquareIndex, nSquareX, nSquareY);

    wsquare.wp.x = nSquareX * 2.5f;
    wsquare.wp.y = nSquareY * 2.5f;
    wsquare.wp.z = sNavMeshGetHeightByIndex(ptNavMeshInst, nSquareIndex);
}










static void sNavMeshGetSquareSceneSpace(SceneCommonData * ptSCD, const SceneDefinition * pdefScene, int32 nSquareIndex, NavMeshSquare3D & wsquare)
{
    int32 nSquareX;
    int32 nSquareY;
    NavMeshGetPositionByIndex(ptSCD->ptNavMeshInst, nSquareIndex, nSquareX, nSquareY);

    wsquare.wp.x = nSquareX * 2.5f;
    wsquare.wp.y = nSquareY * 2.5f;
    wsquare.wp.z = sNavMeshGetHeightByIndex(ptSCD, pdefScene, nSquareIndex);
}










static void sNavMeshGetSquareWorldSpace(
    SceneCommonData * ptSCD,
    const SceneDefinition * pdefScene,
    const NavMeshHandle & tNavMeshHandle,
    NavMeshSquare3D & wsquare)
{

    NavMeshInst * ptNavMeshInst = ptSCD->ptNavMeshInst;


    int32 nSquareX;
    int32 nSquareY;
    NavMeshGetPositionByIndex(ptNavMeshInst, tNavMeshHandle.nSquareIndex, nSquareX, nSquareY);

    nSquareX += ptSCD->tNavMeshOffset.x;
    nSquareY += ptSCD->tNavMeshOffset.y;

    wsquare.wp.x = nSquareX * 2.5f;
    wsquare.wp.y = nSquareY * 2.5f;

    wsquare.wp.z = sNavMeshGetHeightByIndex(ptSCD, pdefScene, tNavMeshHandle.nSquareIndex) + ptSCD->flNavMeshOffsetZ;
}





static void sNavMeshGetSquareWorldSpace(const NavMeshHandle & tNavMeshHandle, NavMeshSquare3D & wsquare)
{
    SceneCommonData & tSCD = SceneCommonDataGet(tNavMeshHandle.idSScene);
    ScopedSceneDefinitionLockR pdefScene(tSCD.snoScene);
    sNavMeshGetSquareWorldSpace(&tSCD, pdefScene, tNavMeshHandle, wsquare);
}

WorldDistance sNavMeshGetHeightWorldSpace(SceneCommonData * ptSCD, const SceneDefinition * pdefScene, const NavMeshHandle & tNavMeshHandle);
WorldDistance NavMeshGetHeightWorldSpace(const NavMeshHandle & tNavMeshHandle);
















void sNavMeshSquare3DGetDrawingCorners(SceneCommonData * ptSCD, const NavMeshHandle & tNavMeshHandle, Vector3D (&wpCorners)[4])
{
    WorldDistance wdCornerZ[4];
    WorldDistance wdBounds[4];

    ScopedSceneDefinitionLockR pdefScene(ptSCD->snoScene);

    wdCornerZ[0] = sNavMeshGetHeightWorldSpace(ptSCD, pdefScene, tNavMeshHandle);

    NavMeshHandle tPosX;
    NavMeshHandle tPosY;
    sNavMeshGetAdjacentHandle(ptSCD, tNavMeshHandle, ADJ_POS_X, tPosX);
    sNavMeshGetAdjacentHandle(ptSCD, tNavMeshHandle, ADJ_POS_Y, tPosY);

    BOOL fPosXExists = !NavMeshHandleIsNone(tPosX);
    BOOL fPosYExists = !NavMeshHandleIsNone(tPosY);

    wdCornerZ[3] = fPosXExists ? (tPosX.idSScene == tNavMeshHandle.idSScene ? sNavMeshGetHeightWorldSpace(ptSCD, pdefScene, tPosX) : NavMeshGetHeightWorldSpace(tPosX)) : wdCornerZ[0];
    wdCornerZ[1] = fPosYExists ? (tPosY.idSScene == tNavMeshHandle.idSScene ? sNavMeshGetHeightWorldSpace(ptSCD, pdefScene, tPosY) : NavMeshGetHeightWorldSpace(tPosY)) : wdCornerZ[0];

    NavMeshHandle tPosXY;
    sNavMeshGetAdjacentHandle(tPosX.idSScene == tNavMeshHandle.idSScene ? ptSCD : SceneCommonDataTryToGet(tPosX.idSScene), tPosX, ADJ_POS_Y, tPosXY);
    if (NavMeshHandleIsNone(tPosXY)) {
        sNavMeshGetAdjacentHandle(tPosY.idSScene == tNavMeshHandle.idSScene ? ptSCD : SceneCommonDataTryToGet(tPosY.idSScene), tPosY, ADJ_POS_X, tPosXY);
    }

    if (!NavMeshHandleIsNone(tPosXY)) {
        wdCornerZ[2] = tPosXY.idSScene == tNavMeshHandle.idSScene ? sNavMeshGetHeightWorldSpace(ptSCD, pdefScene, tPosXY) : NavMeshGetHeightWorldSpace(tPosXY);
    } else if (fPosXExists) {
        wdCornerZ[2] = wdCornerZ[3];
    } else if (fPosYExists) {
        wdCornerZ[2] = wdCornerZ[1];
    } else {
        wdCornerZ[2] = wdCornerZ[0];
    }

    NavMeshSquare3D wsquare;
    sNavMeshGetSquareSceneSpace(ptSCD, pdefScene, tNavMeshHandle.nSquareIndex, wsquare);

    NavMeshSquare3DGetBounds(wsquare, wdBounds);

    WorldDistance wdXOffset = ptSCD->tNavMeshOffset.x * 2.5f;
    WorldDistance wdYOffset = ptSCD->tNavMeshOffset.y * 2.5f;

    wpCorners[CRNR_NEG_X_NEG_Y].x = wdBounds[ADJ_NEG_X] + wdXOffset;
    wpCorners[CRNR_NEG_X_POS_Y].x = wdBounds[ADJ_NEG_X] + wdXOffset;
    wpCorners[CRNR_POS_X_POS_Y].x = wdBounds[ADJ_POS_X] + wdXOffset;
    wpCorners[CRNR_POS_X_NEG_Y].x = wdBounds[ADJ_POS_X] + wdXOffset;

    wpCorners[CRNR_NEG_X_NEG_Y].y = wdBounds[ADJ_NEG_Y] + wdYOffset;
    wpCorners[CRNR_NEG_X_POS_Y].y = wdBounds[ADJ_POS_Y] + wdYOffset;
    wpCorners[CRNR_POS_X_POS_Y].y = wdBounds[ADJ_POS_Y] + wdYOffset;
    wpCorners[CRNR_POS_X_NEG_Y].y = wdBounds[ADJ_NEG_Y] + wdYOffset;

    const float cflZBias = 0.1f;
    wpCorners[0].z = wdCornerZ[0] + cflZBias;
    wpCorners[1].z = wdCornerZ[1] + cflZBias;
    wpCorners[2].z = wdCornerZ[2] + cflZBias;
    wpCorners[3].z = wdCornerZ[3] + cflZBias;
}



























void NavMeshSquare3DGetDrawingCorners(const NavMeshHandle & tNavMeshHandle, Vector3D (&wpCorners)[4])
{
    SceneCommonData & tSCD = SceneCommonDataGet(tNavMeshHandle.idSScene);
    sNavMeshSquare3DGetDrawingCorners(&tSCD, tNavMeshHandle, wpCorners);
}






BOOL NavMeshSquare3DContainsPoint(const NavMeshSquare3D & wsquare, const Vector3D & wp)
{
    const float SQUARE_EPSILON = 0.01f;

    WorldDistance wdSquareBounds[4];
    NavMeshSquare3DGetBounds(wsquare, wdSquareBounds);

    if (wp.x < wdSquareBounds[ADJ_NEG_X] - SQUARE_EPSILON || wp.y < wdSquareBounds[ADJ_NEG_Y] - SQUARE_EPSILON ||
        wp.x > wdSquareBounds[ADJ_POS_X] + SQUARE_EPSILON || wp.y > wdSquareBounds[ADJ_POS_Y] + SQUARE_EPSILON) {
        return FALSE;
    }
    return TRUE;
}
































BOOL NavMeshIsPointOnHandle(const NavMeshHandle & tNavMeshHandle, const Vector3D & wp)
{
    BOOL bResult = FALSE;

    if (!NavMeshHandleIsNone(tNavMeshHandle)) {
        SceneCommonData * ptSCD = SceneCommonDataTryToGet(tNavMeshHandle.idSScene);
        if (ptSCD == NULL) {
            return FALSE;
        }
        if (ptSCD->ptNavMeshInst) {
            NavMeshSquare3D wsquare;
            sNavMeshGetSquareWorldSpace(tNavMeshHandle, wsquare);
            bResult = NavMeshSquare3DContainsPoint(wsquare, wp);
        }
    }
    return bResult;
}


















static void sNavMeshPinPointToSquare(const NavMeshSquare3D & wsquare, Vector3D & wp)
{
    WorldDistance wdBounds[4];
    NavMeshSquare3DGetBounds(wsquare, wdBounds);

    wp.x = Math::Pin(wp.x, wdBounds[ADJ_NEG_X], wdBounds[ADJ_POS_X]);
    wp.y = Math::Pin(wp.y, wdBounds[ADJ_NEG_Y], wdBounds[ADJ_POS_Y]);
    wp.z = wsquare.wp.z;
}




































WorldDistance sNavMeshGetHeightWorldSpace(SceneCommonData * ptSCD, const SceneDefinition * pdefScene, const NavMeshHandle & tNavMeshHandle)
{
    NavMeshSquare3D wsquare;
    sNavMeshGetSquareWorldSpace(ptSCD, pdefScene, tNavMeshHandle, wsquare);
    return wsquare.wp.z;
}









WorldDistance NavMeshGetHeightWorldSpace(const NavMeshHandle & tNavMeshHandle)
{
    NavMeshSquare3D wsquare;
    sNavMeshGetSquareWorldSpace(tNavMeshHandle, wsquare);
    return wsquare.wp.z;
}






BOOL NavMeshGetHeightAndNormal(
    const NavMeshHandle & tNavMeshHandle,
    const Vector3D & wp,
    const BOOL fPinToSquareBounds,
    WorldDistance & flHeight,
    Vector3D & wvNormal)
{
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(tNavMeshHandle.idSScene);
    if (ptSCD == NULL) {

        wvNormal = cgwvZAxis;
        return FALSE;
    }

    if (tNavMeshHandle.nSquareIndex <= 0) {
        wvNormal = cgwvZAxis;
        return FALSE;
    }


    NavMeshInst * ptNavMeshInst = ptSCD->ptNavMeshInst;
    if (ptNavMeshInst == NULL) {
        wvNormal = cgwvZAxis;
        return FALSE;
    }





    int32 nSquareX;
    int32 nSquareY;
    NavMeshGetPositionByIndex(ptNavMeshInst, tNavMeshHandle.nSquareIndex, nSquareX, nSquareY);

    float flCellX = wp.x / 2.5f - nSquareX - ptSCD->tNavMeshOffset.x;
    float flCellY = wp.y / 2.5f - nSquareY - ptSCD->tNavMeshOffset.y;

    if (fPinToSquareBounds) {

        flCellX = Math::Pin(flCellX, 0.0f, 1.0f);
        flCellY = Math::Pin(flCellY, 0.0f, 1.0f);
    } else if (flCellX < 0.0f || flCellX > 1.0f || flCellY < 0.0f || flCellY > 1.0f) {
        wvNormal = cgwvZAxis;
        return FALSE;
    }








    Vector3D wpCorners[4];
    sNavMeshSquare3DGetDrawingCorners(ptSCD, tNavMeshHandle, wpCorners);







    Triangle3D tri;
    Vector3D wpTri;
    if (flCellX < flCellY) {


        tri.Set(wpCorners[1], wpCorners[0], wpCorners[2]);
        tri.PointFromBarycentric(1.0f - flCellY, flCellX, wpTri); // Inferred from the branch-specific barycentric weight at this call site.
    } else {



        tri.Set(wpCorners[3], wpCorners[2], wpCorners[0]);
        tri.PointFromBarycentric(flCellY, 1.0f - flCellX, wpTri); // Inferred from the branch-specific barycentric weight at this call site.
    }

    tri.GetFaceNormal(wvNormal);
    flHeight = wpTri.z;
    return TRUE;

}




static BOOL sNavMeshLineIntersectsSquare(const Vector3D * wpCorners, const LineSeg3D & wline, WorldDistance & wdT, Vector3D & wpIntersect)
{
    Triangle3D tri1;
    tri1.Set(wpCorners[0], wpCorners[1], wpCorners[2]);
    BOOL bIntersect1 = Intersect::LineSeg3DAndTriangle3D(wline, tri1, wdT, &wpIntersect);
    if (bIntersect1) {
        return TRUE;
    }

    Triangle3D tri2;
    tri2.Set(wpCorners[0], wpCorners[2], wpCorners[3]);
    BOOL bIntersect2 = Intersect::LineSeg3DAndTriangle3D(wline, tri2, wdT, &wpIntersect);
    return bIntersect2 ? TRUE : FALSE;
}


















BOOL NavMeshHandleIntersectsLine(const NavMeshHandle & tNavMeshHandle, const LineSeg3D & wline)
{
    Vector3D wpCorners[4];
    NavMeshSquare3DGetDrawingCorners(tNavMeshHandle, wpCorners);

    Vector3D wpIntersect;
    WorldDistance wdT;
    return sNavMeshLineIntersectsSquare(wpCorners, wline, wdT, wpIntersect);
}
































void NavMeshGetCenterSceneSpace(const NavMeshInst * ptNavMeshInst, int32 nSquareIndex, Vector3D & wpCenter)
{
    NavMeshSquare3D wsquare;
    sNavMeshGetSquareSceneSpace(ptNavMeshInst, nSquareIndex, wsquare);
    wpCenter.x = wsquare.wp.x + 1.25f;
    wpCenter.y = wsquare.wp.y + 1.25f;
    wpCenter.z = wsquare.wp.z;
}








void NavMeshGetCenterWorldSpace(const NavMeshHandle & tNavMeshHandle, Vector3D & wpCenter)
{
    NavMeshInst * ptNavMeshInst = NavMeshInstGetByScene(tNavMeshHandle.idSScene);
    Vector3D wpCenterSceneSpace;

    if (ptNavMeshInst == NULL) {
        SceneCommonData & tSCD = SceneCommonDataGet(tNavMeshHandle.idSScene);
        ScopedSceneDefinitionLockR pdefScene(tSCD.snoScene);

        NavMeshInst tTempInst;
        *(NavMesh *)&tTempInst = pdefScene->tNavMeshData;
        tTempInst.idSScene = tNavMeshHandle.idSScene;

        NavMeshGetCenterSceneSpace(&tTempInst, tNavMeshHandle.nSquareIndex, wpCenterSceneSpace);
    } else {
        NavMeshGetCenterSceneSpace(ptNavMeshInst, tNavMeshHandle.nSquareIndex, wpCenterSceneSpace);
    }

    PRTransform transformSceneToWorld;
    SceneGetSceneToWorldTransform(tNavMeshHandle.idSScene, transformSceneToWorld);
    wpCenter = transformSceneToWorld * wpCenterSceneSpace;
}












void NavMeshGetCenterPlace(const NavMeshHandle & tNavMeshHandle, WorldPlace & tPlaceCenter)
{
    NavMeshGetCenterWorldSpace(tNavMeshHandle, tPlaceCenter.wp);
    tPlaceCenter.idSWorld = SceneCommonGetSWorldID(tNavMeshHandle.idSScene);
}






int32 NavMeshLevelAreaIndexFromFlags(uint32 dwFlags)
{
    int32 nIndex = (dwFlags >> CELL_STATIC_LEVELAREA_BIT_0) & 3;
    return nIndex;
}
































int32 NavMeshAtPointSceneSpace(const NavMesh * ptNavMesh, const Vector3D & wpLocation)
{
    if (wpLocation.x < 0.0f || wpLocation.y < 0.0f) {
        return -1;
    }

    int32 nSquareX = Math::FloatToInt(wpLocation.x / 2.5f);
    int32 nSquareY = Math::FloatToInt(wpLocation.y / 2.5f);

    if (nSquareX >= ptNavMesh->nSizeX || nSquareY >= ptNavMesh->nSizeY) {
        return -1;
    }

    return nSquareY * ptNavMesh->nSizeX + nSquareX;
}
















int32 NavMeshAtSquareOffset(const NavMesh * ptNavMesh, int32 nSquareX, int32 nSquareY)
{
    if (nSquareX < 0 || nSquareY < 0 || nSquareX >= ptNavMesh->nSizeX || nSquareY >= ptNavMesh->nSizeY) {
        return -1;
    }
    return nSquareY * ptNavMesh->nSizeX + nSquareX;
}






















































































































static BOOL sTimeAlongLine(const WorldDistance wdStart, const WorldDistance wdEnd, const WorldDistance wdPoint, float & flTime)
{
    WorldDistance wdDiff = wdEnd - wdStart;
    if (Math::Abs(wdDiff) < 1e-06f) {
        return FALSE;
    }
    WorldDistance wdPointDiff = wdPoint - wdStart;
    flTime = wdPointDiff / wdDiff;
    if (flTime < 0.0f) {
        return FALSE;
    }
    return TRUE;
}















static BOOL sNavMeshIsOnDiffLevel(const WorldDistance wd1, const WorldDistance wd2)
{
    float fZDiff = Math::Abs(wd1 - wd2);
    return fZDiff > 8.0f;
}

void NavMeshGetAdjacentHandle(const NavMeshHandle & tNavMeshHandle, int32 nAdjacentDirection, NavMeshHandle & tAdjacent);
BOOL NavMeshHandlesEqual(const NavMeshHandle & tNavMeshHandle1, const NavMeshHandle & tNavMeshHandle2);
void NavMeshGetAdjacentHandles(const NavMeshHandle & tNavMeshHandleStart, NavMeshHandle * arCardinalNavMeshHandles, NavMeshHandle * arCornerNavMeshHandles);
void NavMeshGetAdjacentCornerHandle(const NavMeshHandle & tNavMeshHandle, int32 nAdjacentCorner, NavMeshHandle & tAdjacent);









static BOOL sNavMeshRollMarbleTowards(
    NavMeshRollMarbleCallBack pCallbackFunction,
    void * pCallbackData,
    const NavMeshHandle & tCurrentHandle,
    const Vector3D & wpOnNavMesh,
    const Vector3D & wpDest3D,
    uint32 dwMarbleRollFlags,
    NavMeshHandle & tResultHandle,
    Vector3D & wpResult)
{
    Vector2D wpStart;
    Vector2D wpDest;
    Vector2D wvDirection;

    wpStart.Set(wpOnNavMesh.x, wpOnNavMesh.y);
    wpDest.Set(wpDest3D.x, wpDest3D.y);
    wvDirection = wpDest - wpStart;

    NavMeshSquare3D wsquareCurrent;
    sNavMeshGetSquareWorldSpace(tCurrentHandle, wsquareCurrent);



    if (NavMeshSquare3DContainsPoint(wsquareCurrent, wpDest3D)) {
        wpResult = wpDest3D;



        sNavMeshPinPointToSquare(wsquareCurrent, wpResult);

        tResultHandle = tCurrentHandle;
        if (!(dwMarbleRollFlags & (1 << MARBLE_ROLL_IGNORE_HEIGHT_ON_DEST)) &&


            sNavMeshIsOnDiffLevel(wpResult.z, wpDest3D.z)) {
            return FALSE;
        }
        return TRUE;
    }




    BOOL bHaveXIntersect = FALSE;
    BOOL bHaveYIntersect = FALSE;

    BOOL bNegX = FALSE;
    BOOL bNegY = FALSE;

    float flTimeX = 0.0f;
    float flTimeY = 0.0f;

    WorldDistance wdSquareBounds[4];
    NavMeshSquare3DGetBounds(wsquareCurrent, wdSquareBounds);

    if (wvDirection.x < 0.0f) {
        bNegX = TRUE;





        bHaveXIntersect = sTimeAlongLine(wpOnNavMesh.x, wpDest3D.x, wdSquareBounds[ADJ_NEG_X], flTimeX);
    } else if (wvDirection.x > 0.0f) {
        bHaveXIntersect = sTimeAlongLine(wpOnNavMesh.x, wpDest3D.x, wdSquareBounds[ADJ_POS_X], flTimeX);
    }








    if (wvDirection.y < 0.0f) {
        bNegY = TRUE;





        bHaveYIntersect = sTimeAlongLine(wpOnNavMesh.y, wpDest3D.y, wdSquareBounds[ADJ_NEG_Y], flTimeY);
    } else if (wvDirection.y > 0.0f) {
        bHaveYIntersect = sTimeAlongLine(wpOnNavMesh.y, wpDest3D.y, wdSquareBounds[ADJ_POS_Y], flTimeY);
    }








    if (!bHaveXIntersect && !bHaveYIntersect) {
        wpResult = wpOnNavMesh;



        tResultHandle = tCurrentHandle;
        if (!(dwMarbleRollFlags & (1 << MARBLE_ROLL_IGNORE_HEIGHT_ON_DEST)) &&


            sNavMeshIsOnDiffLevel(wpOnNavMesh.z, wpDest3D.z)) {
            return FALSE;
        }
        return TRUE;
    }




    BOOL bUseXIntersect = FALSE;
    BOOL bUseXYIntersect = FALSE;

    if (bHaveXIntersect && bHaveYIntersect) {



        if (Math::Abs(flTimeX - flTimeY) < 1e-06f) {
            bUseXYIntersect = TRUE;
        } else if (flTimeX < flTimeY) {
            bUseXIntersect = TRUE;
        }
    } else if (bHaveXIntersect) {
        bUseXIntersect = TRUE;
    }






    int32 nAdjacentDirection = -1;
    int32 nAdjacentDirectionX = -1;
    int32 nAdjacentDirectionY = -1;
    int32 nAlsoCheckAdjacentDirectionX = -1;
    int32 nAlsoCheckAdjacentDirectionY = -1;
    int32 nAdjacentCorner = -1;
    if (bUseXYIntersect) {
        if (bNegX && bNegY) {
            nAdjacentCorner = CRNR_NEG_X_NEG_Y;
            nAdjacentDirectionX = ADJ_NEG_X;
            nAdjacentDirectionY = ADJ_NEG_Y;
        } else if (bNegX && !bNegY) {
            nAdjacentCorner = CRNR_NEG_X_POS_Y;
            nAdjacentDirectionX = ADJ_NEG_X;
            nAdjacentDirectionY = ADJ_POS_Y;
        } else if (!bNegX && bNegY) {
            nAdjacentCorner = CRNR_POS_X_NEG_Y;
            nAdjacentDirectionX = ADJ_POS_X;
            nAdjacentDirectionY = ADJ_NEG_Y;
        } else {
            nAdjacentCorner = CRNR_POS_X_POS_Y;
            nAdjacentDirectionX = ADJ_POS_X;
            nAdjacentDirectionY = ADJ_POS_Y;
        }













        wpResult.x = wdSquareBounds[nAdjacentDirectionX];
        wpResult.y = wdSquareBounds[nAdjacentDirectionY];

        if (dwMarbleRollFlags & (1 << MARBLE_ROLL_FAT)) {

            if (wpResult.x < wdSquareBounds[ADJ_NEG_X] + FAT_MARBLE_SIZE) {
                nAlsoCheckAdjacentDirectionX = ADJ_NEG_X;
            } else if (wpResult.x > wdSquareBounds[ADJ_POS_X] - FAT_MARBLE_SIZE) {
                nAlsoCheckAdjacentDirectionX = ADJ_POS_X;
            }




            if (wpResult.y < wdSquareBounds[ADJ_NEG_Y] + FAT_MARBLE_SIZE) {
                nAlsoCheckAdjacentDirectionY = ADJ_NEG_Y;
            } else if (wpResult.y > wdSquareBounds[ADJ_POS_Y] - FAT_MARBLE_SIZE) {
                nAlsoCheckAdjacentDirectionY = ADJ_POS_Y;
            }
        }
    } else if (bUseXIntersect) {





        Math::Lerp(wpOnNavMesh, wpDest3D, flTimeX, wpResult);
        nAdjacentDirection = bNegX ? ADJ_NEG_X : ADJ_POS_X;










        wpResult.x = wdSquareBounds[nAdjacentDirection];
        wpResult.y = Math::Pin(wpResult.y, wdSquareBounds[ADJ_NEG_Y], wdSquareBounds[ADJ_POS_Y]);

        if (dwMarbleRollFlags & (1 << MARBLE_ROLL_FAT)) {

            if (wpResult.y < wdSquareBounds[ADJ_NEG_Y] + FAT_MARBLE_SIZE) {
                nAlsoCheckAdjacentDirectionY = ADJ_NEG_Y;
            } else if (wpResult.y > wdSquareBounds[ADJ_POS_Y] - FAT_MARBLE_SIZE) {
                nAlsoCheckAdjacentDirectionY = ADJ_POS_Y;
            }
        }
    } else {





        Math::Lerp(wpOnNavMesh, wpDest3D, flTimeY, wpResult);
        nAdjacentDirection = bNegY ? ADJ_NEG_Y : ADJ_POS_Y;










        wpResult.x = Math::Pin(wpResult.x, wdSquareBounds[ADJ_NEG_X], wdSquareBounds[ADJ_POS_X]);
        wpResult.y = wdSquareBounds[nAdjacentDirection];
        if (dwMarbleRollFlags & (1 << MARBLE_ROLL_FAT)) {


            if (wpResult.x < wdSquareBounds[ADJ_NEG_X] + FAT_MARBLE_SIZE) {
                nAlsoCheckAdjacentDirectionX = ADJ_NEG_X;
            } else if (wpResult.x > wdSquareBounds[ADJ_POS_X] - FAT_MARBLE_SIZE) {
                nAlsoCheckAdjacentDirectionX = ADJ_POS_X;
            }
        }
    }






    NavMeshHandle tCheckHandle;
    if (!bUseXYIntersect) {


        NavMeshGetAdjacentHandle(tCurrentHandle, nAdjacentDirection, tCheckHandle);
    } else {



        NavMeshGetAdjacentCornerHandle(tCurrentHandle, nAdjacentCorner, tCheckHandle);
    }

    if (!pCallbackFunction(pCallbackData, tCheckHandle)) {
        wpResult.z = wsquareCurrent.wp.z; // A failed crossing keeps the current square height.
        tResultHandle = tCurrentHandle;
        return FALSE;
    }



    if (nAlsoCheckAdjacentDirectionX != -1) {

        NavMeshHandle tAlsoCheckHandle;
        NavMeshGetAdjacentHandle(tCurrentHandle, nAlsoCheckAdjacentDirectionX, tAlsoCheckHandle);

        if (!pCallbackFunction(pCallbackData, tAlsoCheckHandle)) {
            wpResult.z = wsquareCurrent.wp.z; // A failed crossing keeps the current square height.
            tResultHandle = tCurrentHandle;
            return FALSE;
        }
    }



    if (nAlsoCheckAdjacentDirectionY != -1) {

        NavMeshHandle tAlsoCheckHandle;
        NavMeshGetAdjacentHandle(tCurrentHandle, nAlsoCheckAdjacentDirectionY, tAlsoCheckHandle);

        if (!pCallbackFunction(pCallbackData, tAlsoCheckHandle)) {
            wpResult.z = wsquareCurrent.wp.z; // A failed crossing keeps the current square height.
            tResultHandle = tCurrentHandle;
            return FALSE;
        }
    }











    wpResult.z = NavMeshGetHeightWorldSpace(tCheckHandle);
    tResultHandle = tCheckHandle;
    return TRUE;
}





int32 NavMeshGetAdjacentSquare(const NavMesh * ptNavMesh, int32 nSquareIndex, int32 nAdjacentDirection)
{
    int32 nSquareX;
    int32 nSquareY;
    NavMeshGetPositionByIndex(ptNavMesh, nSquareIndex, nSquareX, nSquareY);

    switch (nAdjacentDirection) {
    case ADJ_NEG_Y:
        if (nSquareY > 0) {
            return nSquareIndex - ptNavMesh->nSizeX;
        }
        break;
    case ADJ_POS_Y:
        if (nSquareY < ptNavMesh->nSizeY - 1) {
            return nSquareIndex + ptNavMesh->nSizeX;
        }
        break;
    case ADJ_POS_X:
        if (nSquareX < ptNavMesh->nSizeX - 1) {
            return nSquareIndex + 1;
        }
        break;
    case ADJ_NEG_X:
        if (nSquareX > 0) {
            return nSquareIndex - 1;
        }
        break;
    default:
















        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    return -1;
}




void sNavMeshGetAdjacentHandle(SceneCommonData * ptSCD, const NavMeshHandle & tNavMeshHandle, int32 nAdjacentDirection, NavMeshHandle & tAdjacent)
{
    if (NavMeshHandleIsNone(tNavMeshHandle)) {
        tAdjacent = NAVMESH_HANDLE_NONE;
        return;
    }

    NavMeshInst * ptNavMeshInst = ptSCD ? ptSCD->ptNavMeshInst : NULL;
    if (ptNavMeshInst == NULL) {
        tAdjacent = NAVMESH_HANDLE_NONE;
        return;
    }

    int32 nAdjIndex = NavMeshGetAdjacentSquare(ptNavMeshInst, tNavMeshHandle.nSquareIndex, nAdjacentDirection);
    if (nAdjIndex != -1) {
        tAdjacent.idSScene = tNavMeshHandle.idSScene;
        tAdjacent.nSquareIndex = nAdjIndex;
        return;
    }

    {
        BOOL bResult = NavMeshStitchLookup(ptSCD, tNavMeshHandle, nAdjacentDirection, &tAdjacent);
        if (!bResult) {
            tAdjacent = NAVMESH_HANDLE_NONE;
        }
    }
}
















void NavMeshGetAdjacentHandle(const NavMeshHandle & tNavMeshHandle, int32 nAdjacentDirection, NavMeshHandle & tAdjacent)
{
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(tNavMeshHandle.idSScene);
    sNavMeshGetAdjacentHandle(ptSCD, tNavMeshHandle, nAdjacentDirection, tAdjacent);
}







void NavMeshGetAdjacentCornerHandle(const NavMeshHandle & tNavMeshHandle, int32 nAdjacentCorner, NavMeshHandle & tAdjacent)
{
    NavMeshHandle arCardinalNavMeshHandles[4];
    NavMeshHandle arCornerNavMeshHandles[4];

    if (NavMeshHandleIsNone(tNavMeshHandle)) {
        tAdjacent = NAVMESH_HANDLE_NONE;
        return;
    }

    NavMeshInst * ptNavMeshInst = NavMeshInstGetByScene(tNavMeshHandle.idSScene);
    if (ptNavMeshInst == NULL) {
        tAdjacent = NAVMESH_HANDLE_NONE;
        return;
    }

    NavMeshGetAdjacentHandles(tNavMeshHandle, arCardinalNavMeshHandles, arCornerNavMeshHandles);
    tAdjacent = arCornerNavMeshHandles[nAdjacentCorner];
}











































BOOL NavMeshHandleGetStaticFlag(const NavMeshHandle & tNavMeshHandle, int32 nFlag)
{
    const uint32 & dwSquareFlags = NavMeshHandleToStaticFlags(tNavMeshHandle, FALSE);
    BOOL bSet = (dwSquareFlags >> nFlag) & 1;
    return bSet;
}






















int32 NavMeshHandleGetLevelAreaIndex(const NavMeshHandle & tNavMeshHandle)
{
    const uint32 & dwSquareFlags = NavMeshHandleToStaticFlags(tNavMeshHandle, TRUE);
    int32 nIndex = NavMeshLevelAreaIndexFromFlags(dwSquareFlags);
    return nIndex;
}







SNO NavMeshHandleGetLevelArea(const SceneCommonData * ptSCD, const NavMeshHandle & tNavMeshHandle)
{
    SNO snoLevelArea = -1;
    if (ptSCD) {
        SNO snoWorld = WorldCommonGetWorldSNO(ptSCD->idSWorld);
        if (WorldIsDungeonFinderPlaceHolder(snoWorld)) {
            snoLevelArea = WorldGetDungeonFinderLevelArea(snoWorld);
        } else {
            int32 nIndex = NavMeshHandleGetLevelAreaIndex(tNavMeshHandle);
            snoLevelArea = ptSCD->tSceneSpec.arSnoLevelAreas[nIndex];
            if (snoLevelArea == -1) {
                snoLevelArea = ptSCD->tSceneSpec.arSnoLevelAreas[0];
            }
        }
    }
    return snoLevelArea;
}














SNO NavMeshHandleGetLevelArea(const NavMeshHandle & tNavMeshHandle)
{
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(tNavMeshHandle.idSScene);
    return NavMeshHandleGetLevelArea(ptSCD, tNavMeshHandle);
}






LevelAreaID NavMeshHandleGetLevelAreaID(const NavMeshHandle & tNavMeshHandle, BOOL bForDistribution)
{



    LevelAreaID idLevelArea = -1;



    if (GameActiveGameCommonDataIsServer()) {

        SScene * ptSScene = SSceneTryToGet(tNavMeshHandle.idSScene);


        if (ptSScene) {
            if (ptSScene->tSCD.idSWorld != SWORLDID_INVALID) {
                SWorld * ptSWorld = SWorldTryToGet(ptSScene->tSCD.idSWorld);
                if (!bForDistribution && ptSWorld &&
                    WorldIsDungeonFinderPlaceHolder(ptSWorld->tWCD.snoWorld)) {



                    SNO snoLevelArea = WorldGetDungeonFinderLevelArea(ptSWorld->tWCD.snoWorld);
                    idLevelArea = LevelAreaFind(snoLevelArea, ptSWorld->id);
                }
            }

            if (idLevelArea != -1) {
                return idLevelArea;
            }

            if (ptSScene->tSCD.ptNavMeshInst) {

                int32 nIndex = NavMeshHandleGetLevelAreaIndex(tNavMeshHandle);

                idLevelArea = ptSScene->arLevelAreaIds[nIndex];
            }
        }
    } else {



        SNO snoLevelArea = NavMeshHandleGetLevelArea(tNavMeshHandle);

        if (snoLevelArea != -1) {

            SceneCommonData * ptSCD = SceneCommonDataTryToGet(tNavMeshHandle.idSScene);

            idLevelArea = LevelAreaFind(snoLevelArea, ptSCD->idSWorld);
        }
    }
    return idLevelArea;
}



























uint32 NavMeshHandleGetStaticFlags(const NavMeshHandle & tNavMeshHandle)
{
    return NavMeshHandleToStaticFlags(tNavMeshHandle, FALSE);
}






































void NavMeshRollDataInitialize(NavMeshRollData & tRollData)
{
    SigmaMemoryClear(&tRollData, sizeof(NavMeshRollData));

    tRollData.tNavMeshHandleStartPathingSpace = NAVMESH_HANDLE_NONE;
    tRollData.tNavMeshHandleStartCenter = NAVMESH_HANDLE_NONE;
    tRollData.tNavMeshHandleEndPathingSpace = NAVMESH_HANDLE_NONE;
    tRollData.tNavMeshHandleEndCenter = NAVMESH_HANDLE_NONE;

    tRollData.tPassability.ePassabilityType = PASSABILITY_UNKNOWN_OBJECT;
    tRollData.tPassability.snoActor = -1;
    tRollData.tDestinationPassability.ePassabilityType = PASSABILITY_UNKNOWN_OBJECT;

    tRollData.idACD = ACDID_INVALID;
}


static BOOL sNavMeshRollMarbleCheckDistance(NavMeshRollData & tRollData, NavMeshHandle & tCurrentHandle, Vector3D & wpCurrent, NavMeshHandle & tNextHandle, Vector3D & wpNext)
{
    if (tRollData.wdMaxRollDistance < 1e-06f) {
        return TRUE;
    }

    WorldDistance wdRolled = Vector3D::Distance2D(wpCurrent, wpNext);
    if (wdRolled > 1e-06f) {
        WorldDistance wdRemaining = tRollData.wdMaxRollDistance - tRollData.wdResultRollDistance;
        if (wdRolled > wdRemaining) {
            float flFraction = wdRemaining / wdRolled;
            Math::Lerp(wpCurrent, wpNext, flFraction, wpNext);
            wpNext.z = wpCurrent.z;
            tNextHandle = tCurrentHandle;
            tRollData.wdResultRollDistance = tRollData.wdMaxRollDistance;
            return FALSE;
        }
        tRollData.wdResultRollDistance += wdRolled;
    }
    return TRUE;
}

























BOOL NavMeshRollMarble(
    NavMeshRollData & tRollData)
{


    NavMeshGlobals * ptNavMeshGlobals = GameGetActiveGameCommonData().ptNavMeshGlobals;
    GamePerfCtrIncrement(ptNavMeshGlobals->gPathingRollingCounter, 1);

    Vector3D wpStartPathingSpace(tRollData.wpStart);
    Vector3D wpDestPathingSpace(tRollData.wpDest);
    NavMeshHandle tCurrentHandle = tRollData.tNavMeshHandleStartPathingSpace;

    if (tCurrentHandle.nSquareIndex < 0) {
        tCurrentHandle = tRollData.tNavMeshHandleStartCenter;
    }






    NavMeshIsPointOnHandle(tCurrentHandle, wpStartPathingSpace);







































    BOOL bCanEnter = tRollData.pCallbackFunction(tRollData.pCallbackData, tCurrentHandle);

    if (!bCanEnter) {



        tRollData.wpEnd = tRollData.wpStart;
        tRollData.tNavMeshHandleEndPathingSpace = tRollData.tNavMeshHandleStartPathingSpace;
        tRollData.tNavMeshHandleEndCenter = tRollData.tNavMeshHandleStartCenter;
        return FALSE;
    }


    Vector3D wpCurrent = wpStartPathingSpace;


    wpCurrent.z = NavMeshGetHeightWorldSpace(tCurrentHandle);

    int32 nLoopCount = 0;
    Vector3D wpNext;
    Vector3D wpDest = wpDestPathingSpace;
    BOOL bResult = FALSE;


    BOOL bCheckDestination = tRollData.tDestinationPassability.ePassabilityType != PASSABILITY_UNKNOWN_OBJECT;






    Vector3D wpValidDest = wpStartPathingSpace;
    NavMeshHandle tValidDestHandle = tRollData.tNavMeshHandleStartPathingSpace;
    while (TRUE) {


        NavMeshHandle tNextHandle;

        BOOL bPassable = sNavMeshRollMarbleTowards(tRollData.pCallbackFunction, tRollData.pCallbackData, tCurrentHandle, wpCurrent, wpDest, tRollData.dwMarbleRollFlags, tNextHandle, wpNext);














        BOOL bRollMore = sNavMeshRollMarbleCheckDistance(tRollData, tCurrentHandle, wpCurrent, tNextHandle, wpNext);






        if (bCheckDestination && NavMeshHandleCanEnter(tNextHandle, tRollData.tDestinationPassability.ePassabilityType)) {
            wpValidDest = wpNext;
            tValidDestHandle = tNextHandle;
        }
        wpCurrent = wpNext;













        if (!bRollMore) {
            bResult = TRUE;
            break;
        }


        if (!bPassable) {
            break;
        }



        if (NavMeshHandlesEqual(tCurrentHandle, tNextHandle)) {
            bResult = TRUE;
            break;
        }



        tCurrentHandle = tNextHandle;


        if (tRollData.dwMarbleRollFlags & (1 << MARBLE_ROLL_ONE_SQUARE)) {
            bResult = TRUE;
            break;
        }




        if (++nLoopCount > 1000) // The original allows the iteration with a count of 1000.
        {
            break;
        }
    }






    if (!bCheckDestination) {

        tRollData.wpEnd = wpCurrent;
        tRollData.tNavMeshHandleEndPathingSpace = tCurrentHandle;
        tRollData.tNavMeshHandleEndCenter = tCurrentHandle;
    } else {



        tRollData.wpEnd = wpValidDest;
        tRollData.tNavMeshHandleEndPathingSpace = tValidDestHandle;
        tRollData.tNavMeshHandleEndCenter = tValidDestHandle;







    }
    return bResult;
}
NavMeshSquare NavMeshIndexToSquare(const NavMeshDef * ptNavMeshDef, int32 nSquareIndex, BOOL fSpecialOnly)
{








    int32 nY = nSquareIndex / ptNavMeshDef->nSizeX;
    int32 nX = nSquareIndex % ptNavMeshDef->nSizeX;
    int32 nNewSizeX = ptNavMeshDef->nSizeX - ptNavMeshDef->nEdgeLeftSize - ptNavMeshDef->nEdgeRightSize;
    int32 nNewSizeY = ptNavMeshDef->nSizeY - ptNavMeshDef->nEdgeTopSize - ptNavMeshDef->nEdgeBottomSize;

    nY -= ptNavMeshDef->nEdgeTopSize;
    nX -= ptNavMeshDef->nEdgeLeftSize;

    if (nX < 0 || nX >= nNewSizeX || nY < 0 || nY >= nNewSizeY) {
        return ptNavMeshDef->tEdge;
    }

    int32 nNewSquareIndex = nY * nNewSizeX + nX;

    if (fSpecialOnly) {
        uint32 dwSpecialFlags = ((unsigned char *)ptNavMeshDef->ptCompNavMeshArraySquaresSpecialFlags)[nNewSquareIndex >> 1];
        dwSpecialFlags >>= (nNewSquareIndex & 1) << 2;

        NavMeshSquare tSquare;
        tSquare.wdSquareZ = FLT_MAX;
        tSquare.dwSquareFlags = dwSpecialFlags & 1;
        int32 nIndex = (dwSpecialFlags >> 1) & 3;
        tSquare.dwSquareFlags |= nIndex << CELL_STATIC_LEVELAREA_BIT_0;
        return tSquare;
    }





    float flRange = ptNavMeshDef->flMaxZ - ptNavMeshDef->flMinZ;


    if ((CompNavMeshSquare *)ptNavMeshDef->ptCompNavMeshArraySquares) {
        NavMeshSquare tSquare;
        CompNavMeshSquare & tCompSquare = ((CompNavMeshSquare *)ptNavMeshDef->ptCompNavMeshArraySquares)[nNewSquareIndex];
        tSquare.dwSquareFlags = tCompSquare.dwSquareFlags;
        tSquare.wdSquareZ = ptNavMeshDef->flMinZ + flRange * (tCompSquare.wdSquareZ / 65535.0f);
        return tSquare;
    }

    return ptNavMeshDef->tEdge;



}





















uint32 NavMeshIndexToStaticFlags(const NavMeshInst * ptNavMeshInst, int32 nSquareIndex, BOOL fSpecialOnly)
{
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(ptNavMeshInst->idSScene);

    if (ptSCD->tChildSceneIDs.Count() > 0) {
        int32 nParentSpaceX;
        int32 nParentSpaceY;
        NavMeshGetPositionByIndex(ptNavMeshInst, nSquareIndex, nParentSpaceX, nParentSpaceY);

        POS pos = ptSCD->tChildSceneIDs.GetHeadPosition();
        while (pos) {
            SSceneID idChildScene = ptSCD->tChildSceneIDs.GetNext(pos);
            SceneCommonData * ptSCDChild = SceneCommonDataTryToGet(idChildScene);
            if (ptSCDChild) {
                Vector3D wpOffset = ptSCDChild->transformWorld.wp - ptSCD->transformWorld.wp;

                ScopedSceneDefinitionLockR pdefChild(ptSCDChild->snoScene);
                int32 nChildSizeX = pdefChild->tNavMeshData.nSizeX;
                int32 nChildSizeY = pdefChild->tNavMeshData.nSizeY;

                IVector2D tOffset;
                tOffset.x = (int32)(wpOffset.x / 2.5f);
                tOffset.y = (int32)(wpOffset.y / 2.5f);

                if (nParentSpaceX >= tOffset.x && nParentSpaceX < tOffset.x + nChildSizeX &&
                    nParentSpaceY >= tOffset.y && nParentSpaceY < tOffset.y + nChildSizeY) {
                    int32 nChildSquareIndex = (nParentSpaceY - tOffset.y) * nChildSizeX + (nParentSpaceX - tOffset.x);
                    return NavMeshIndexToSquare(&pdefChild->tNavMeshData, nChildSquareIndex, fSpecialOnly).dwSquareFlags;
                }
            }
        }
    }

    {
        ScopedSceneDefinitionLockR pdefScene(ptSCD->snoScene);
        if (pdefScene->tNavMeshData.nNumSquares != ptNavMeshInst->nNumSquares) {
            return 0;
        }
        return NavMeshIndexToSquare(&pdefScene->tNavMeshData, nSquareIndex, fSpecialOnly).dwSquareFlags;
    }
}




























uint32 NavMeshHandleToStaticFlags(const NavMeshHandle & tNavMeshHandle, BOOL fSpecialOnly)
{
    NavMeshInst * ptNavMeshInst = NavMeshInstGetByScene(tNavMeshHandle.idSScene);
    if (ptNavMeshInst == NULL) {
        SceneCommonData & tSCD = SceneCommonDataGet(tNavMeshHandle.idSScene);
        ScopedSceneDefinitionLockR pdefScene(tSCD.snoScene);
        NavMeshInst tTempInst;
        *(NavMesh *)&tTempInst = pdefScene->tNavMeshData;
        tTempInst.idSScene = tNavMeshHandle.idSScene;
        return NavMeshIndexToStaticFlags(&tTempInst, tNavMeshHandle.nSquareIndex, fSpecialOnly);
    }
    return NavMeshIndexToStaticFlags(ptNavMeshInst, tNavMeshHandle.nSquareIndex, fSpecialOnly);
}




































NavMeshInst * NavMeshInstGetByScene(SSceneID idSScene)
{
    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
    if (ptSCD) {
        return ptSCD->ptNavMeshInst;
    }
    return NULL;
}






BOOL NavMeshHandleIsNone(const NavMeshHandle & tNavMeshHandle)
{
    return tNavMeshHandle.nSquareIndex < 0;
}












BOOL NavMeshHandlesEqual(
    const NavMeshHandle & tNavMeshHandle1,
    const NavMeshHandle & tNavMeshHandle2)
{
    if (tNavMeshHandle1.nSquareIndex == -1 || tNavMeshHandle1.nSquareIndex != tNavMeshHandle2.nSquareIndex)
    {
        return FALSE;
    }

    return tNavMeshHandle1.idSScene == tNavMeshHandle2.idSScene;
}
struct PassableTableItem {  // size 0xc
    PassabilityType ePassabilityType;  // 0x00
    uint32 dwOneRequiredStaticFlags;  // 0x04
    uint32 dwOccupiedDenialDynFlags;  // 0x08
};
static_assert(sizeof(PassableTableItem) == 0xc, "PassableTableItem");

























































































































































































































































































































































































































































































































































































void NavMeshGetActorPassabilitySettings(SNO snoActor, float flScale, PassabilitySettings & tPassability);
PassabilityType NavMeshDeterminePassabilityType(SNO snoActor);
PassabilityType NavMeshDeterminePassabilityType(const ActorDefinition * pdefActor);

void NavMeshGetActorPassabilitySettings(SNO snoActor, float flScale, PassabilitySettings & tPassability)
{
    PassabilityType ePassabilityType = NavMeshDeterminePassabilityType(snoActor);
    tPassability.ePassabilityType = ePassabilityType;
    tPassability.snoActor = snoActor;
    tPassability.aYaw = 0.0f;
    tPassability.flScale = flScale;
}











































































































































static PassableTableItem csgPassableTable[18] = {
    { PASSABILITY_PLAYER, 0x1, 0x639 },
    { PASSABILITY_PLAYER_AI_OFF, 0x1, 0xe30 },
    { PASSABILITY_PROJECTILE, 0x407, 0x420 },
    { PASSABILITY_SPIDER, 0x5, 0x77b },
    { PASSABILITY_MONSTER, 0x1, 0x77b },
    { PASSABILITY_TOWNPORTAL, 0x1, 0x2bb },
    { PASSABILITY_FLIPPIES, 0x1, 0x2f0 },
    { PASSABILITY_UNKNOWN_OBJECT, 0xc07, 0x0 },
    { PASSABILITY_NONMOVER, 0x0, 0x63b },
    { PASSABILITY_CHARGE, 0x1, 0x30 },
    { PASSABILITY_NEW_ENCOUNTER, 0x1, 0x631 },
    { PASSABILITY_UNDEFINED, 0x0, 0x0 },
    { PASSABILITY_FLYER, 0x7, 0x77b },
    { PASSABILITY_LEAP, 0x7, 0x0 },
    { PASSABILITY_OWNED, 0x7, 0x0 },
    { PASSABILITY_GIZMO_OPERATE, 0x407, 0x0 },
    { PASSABILITY_GHOST, 0xc47, 0x42b },
    { PASSABILITY_ALLOW_PROJECTILES, 0x400, 0x420 },
};


uint32 NavMeshGetStaticFlagsForPassability(PassabilityType ePassability)
{
    uint32 uFlags = csgPassableTable[ePassability].dwOneRequiredStaticFlags;
    return uFlags;
}


uint32 NavMeshGetDynamicFlagsForPassability(PassabilityType ePassability)
{
    uint32 uFlags = csgPassableTable[ePassability].dwOccupiedDenialDynFlags;
    return uFlags;
}


PassabilityType NavMeshDeterminePassabilityType(SNO snoActor)
{
    ScopedActorDefinitionLockR pdefActor(snoActor);
    return NavMeshDeterminePassabilityType(pdefActor);
}


PassabilityType NavMeshDeterminePassabilityType(const ActorDefinition * pdefActor)
{
    const ActorCollisionFlags & tCollFlags = pdefActor->tCollData.tCollFlags;
    PassabilityType eType;
    if (tCollFlags.uDynamicMyFlags & 0x8) {
        eType = PASSABILITY_PLAYER;
    } else if (tCollFlags.uDynamicMyFlags & 0x80) {
        eType = PASSABILITY_FLIPPIES;
    } else if (tCollFlags.uStaticOneRequiredFlags & (1 << CELL_STATIC_ALLOW_GHOST)) {
        eType = PASSABILITY_GHOST;
    } else if (tCollFlags.uStaticOneRequiredFlags & (1 << CELL_STATIC_ALLOW_PROJECTILE)) {
        eType = PASSABILITY_PROJECTILE;
    } else if (tCollFlags.uStaticOneRequiredFlags & (1 << CELL_STATIC_ALLOW_FLIER)) {
        eType = PASSABILITY_FLYER;
    } else if (tCollFlags.uStaticOneRequiredFlags & (1 << CELL_STATIC_ALLOW_SPIDER)) {
        eType = PASSABILITY_SPIDER;
    } else {
        eType = PASSABILITY_MONSTER;
    }
    return eType;
}










































































BOOL NavMeshCanEnterForSpawning(const NavMeshDef * ptNavMeshDef, int32 nSquareIndex, const PassabilityType ePassabilityType)
{
    uint32 uStaticFlags = NavMeshIndexToSquare(ptNavMeshDef, nSquareIndex, ePassabilityType == PASSABILITY_NEW_ENCOUNTER).dwSquareFlags;
    if (uStaticFlags & (1 << CELL_STATIC_NO_SPAWN)) {
        return FALSE;
    }
    const PassableTableItem & tPassableItem = csgPassableTable[ePassabilityType];
    return (uStaticFlags & tPassableItem.dwOneRequiredStaticFlags) != 0;
}















BOOL NavMeshPlaceCanEnterForSpawning(const WorldPlace & placeSpawn, const PassabilityType ePassabilityType)
{
    NavMeshHandle tNavMeshHandle;
    BOOL bOnMesh = ScenesCommonNavMeshNodeFromPlace(placeSpawn, &tNavMeshHandle, FALSE, FALSE);
    if (!bOnMesh) {
        return FALSE;
    }

    const PassableTableItem & tPassableItem = csgPassableTable[ePassabilityType];
    uint32 uStaticFlags = tPassableItem.dwOneRequiredStaticFlags;

    if (uStaticFlags & (1 << CELL_STATIC_ALLOW_WALK)) {
        if (NavCellPlaceIsOnWalkableCorner(placeSpawn)) {
            return TRUE;
        }
    }

    uint32 dwSquareStaticFlags = NavMeshHandleGetStaticFlags(tNavMeshHandle);
    return (dwSquareStaticFlags & uStaticFlags) != 0;
}






















BOOL NavMeshHandleCanEnter(const NavMeshHandle & tNavMeshHandle, const PassabilityType ePassabilityType)
{
    if (NavMeshHandleIsNone(tNavMeshHandle)) {
        return FALSE;
    }

    const PassableTableItem & tPassableItem = csgPassableTable[ePassabilityType];
    uint32 dwNavMeshSquareFlags = NavMeshHandleGetStaticFlags(tNavMeshHandle);
    return dwNavMeshSquareFlags & tPassableItem.dwOneRequiredStaticFlags;
}











BOOL NavMeshCallBackPassabilityTypeCanEnter(void * pCallbackData, const NavMeshHandle & tNavMeshHandle)
{
    PassabilityType ePassabilityType = (PassabilityType)(intptr_t)pCallbackData;
    BOOL bResult = NavMeshHandleCanEnter(tNavMeshHandle, ePassabilityType);
    return bResult;
}
































BOOL NavMeshSearchForLocation(const WorldPlace & placeStart, const PassabilitySettings & tPassability, const float flMinSearchRadius, const float flMaxSearchRadius, const uint32 dwFindLocationFlags, NavMeshHandle & tNavMeshHandleLanding, WorldPlace & placeLanding, BOOL bSearchOverImpassable, Team eTeam, float flRadiusOverride)
{









    float flRadius;
    uint32 uDynamicFlagsNoOverlap;
    uint16 uStaticOneRequiredFlags;







    if (tPassability.snoActor != -1) {

        ScopedActorDefinitionLockR ptActorDef(tPassability.snoActor);
        const ActorCollisionData & tCollData = ptActorDef->tCollData;
        switch (tCollData.eCollisionShape) {
        default:
            flRadius = 0.0f;
            break;
        case COLLISION_SHAPE_NONE:
            flRadius = 1.25f;
            break;
        case COLLISION_SHAPE_BOX:
            flRadius = tPassability.flScale * Vector2D(tCollData.aabbCollision.wvExt.x, tCollData.aabbCollision.wvExt.y).Magnitude();
            break;
        case COLLISION_SHAPE_CYLINDER:
            flRadius = tPassability.flScale * ActorGetCollisionCylinderRadius(ptActorDef);
            break;
        }


        if (tPassability.ePassabilityType == PASSABILITY_PULL_FROM_ACTOR) {
            uStaticOneRequiredFlags = tCollData.tCollFlags.uStaticOneRequiredFlags;
            uDynamicFlagsNoOverlap = tCollData.tCollFlags.uDynamicFlagsNoOverlap;
        } else {
            uStaticOneRequiredFlags = NavMeshGetStaticFlagsForPassability(tPassability.ePassabilityType);
            uDynamicFlagsNoOverlap = NavMeshGetDynamicFlagsForPassability(tPassability.ePassabilityType);
        }
    } else {


        flRadius = 1.25f;
        uStaticOneRequiredFlags = NavMeshGetStaticFlagsForPassability(tPassability.ePassabilityType);
        uDynamicFlagsNoOverlap = csgPassableTable[tPassability.ePassabilityType].dwOccupiedDenialDynFlags;
    }

    if (flRadiusOverride > 1e-06f) {
        flRadius = flRadiusOverride;
    }
    flRadius = Math::Max(flRadius, 1.0f);

    NavCellHandle hNavCellSearch;
    Vector3D wpSearch;
    BOOL bFoundLoc = WorldCollSearchForLocation(placeStart, uStaticOneRequiredFlags, uDynamicFlagsNoOverlap, flRadius, flMinSearchRadius, flMaxSearchRadius, ACDID_INVALID, wpSearch, hNavCellSearch, eTeam, 0, ACDID_INVALID);
    if (!bFoundLoc) {

        WorldPlace placeModified = placeStart;
        NavCellHandle hNavCellTest;
        if (NavCellHandleFromWorldPlace(placeModified, hNavCellTest)) {

            const NavZoneInstance * ptNavZoneInstance = NavCellHandleToZoneInstance(hNavCellTest);
            NavCellModifyForRoundedCorners(ptNavZoneInstance, hNavCellTest, (Vector2D &)placeModified.wp, uStaticOneRequiredFlags);
            bFoundLoc = WorldCollSearchForLocation(placeModified, uStaticOneRequiredFlags, uDynamicFlagsNoOverlap, flRadius, flMinSearchRadius, flMaxSearchRadius, ACDID_INVALID, wpSearch, hNavCellSearch, eTeam, 0, ACDID_INVALID);
        }
    }

    if (bFoundLoc) {

        placeLanding.idSWorld = placeStart.idSWorld;
        placeLanding.wp = wpSearch;
    } else {


        placeLanding = placeStart;
    }

    ScenesCommonNavMeshNodeFromPlace(placeLanding, &tNavMeshHandleLanding, FALSE, FALSE);

    return bFoundLoc;
}

























BOOL NavMeshCheckLOSToACD(NavCellHandle hNavCellStart, const WorldPlace & placeStart, ACDID idACDDest, const PassabilityType ePassabilityType, BOOL bWantProjectileLOS)
{





    WorldCollResult tResult;
    WorldCollParams tCollParams;
    ActorCommonData * ptACD = ACDTryToGet(idACDDest);
    if (ptACD == NULL) {
        return FALSE;
    }

    ActorCommonData & tACD = *ptACD;


    tCollParams.uOneRequiredStaticFlags = (uint16)NavMeshGetStaticFlagsForPassability(ePassabilityType);
    if (bWantProjectileLOS) {
        tCollParams.uObjectNoOverlapFlags |= 0x420;
    } else {
        tCollParams.uObjectNoOverlapFlags |= 0x20;
    }

    BOOL bColl = WorldCollSweptPoint(placeStart.idSWorld, hNavCellStart, (const Vector2D &)placeStart.wp, (const Vector2D &)tACD.GetWorldLocation().transform.wp, tCollParams, 0, 0, idACDDest, tResult);

    return !bColl;
}









BOOL NavMeshShouldCheckLOS(const NavMeshHandle & tNavMeshHandle)
{
    BOOL fCheckLOS = FALSE;
    if (!NavMeshHandleIsNone(tNavMeshHandle)) {
        SNO snoScene = SceneCommonGetSceneSNO(tNavMeshHandle.idSScene);
        ScopedSceneDefinitionLockR pdefScene(snoScene);
        fCheckLOS = (pdefScene->dwFlags & (1 << SCENE_DISABLE_LOS_BIT)) == 0;
    }
    return fCheckLOS;
}









































BOOL NavMeshRollToTargetPlace(const ACDID idACDSource, const WorldPlace & tDesiredDestPlace, const BOOL & fIgnoreDestinationHeight, NavPlace & tNavPlaceResult)
{
    Target tTarget;
    tTarget.InitializeFromPlace(tDesiredDestPlace);
    return PowerRollToTargetNavPlace(idACDSource, tTarget, PASSABILITY_PROJECTILE, fIgnoreDestinationHeight, tNavPlaceResult);
}















BOOL NavMeshHandleIsActivated(const NavMeshHandle & tNavMeshHandle)
{
    if (NavMeshHandleIsNone(tNavMeshHandle)) {
        return FALSE;
    }
    NavMeshInst * ptNavMeshInst = NavMeshInstGetByScene(tNavMeshHandle.idSScene);
    return ptNavMeshInst != NULL;
}












BOOL NavMeshHandleIsInRegion(const NavMeshHandle & tNavMeshHandle, const IVector2D & tOffset, const IVector2D & tSize)
{
    BOOL bInRegion = FALSE;
    SceneCommonData & tSCD = SceneCommonDataGet(tNavMeshHandle.idSScene);

    int32 nSquareX;
    int32 nSquareY;
    NavMeshGetPositionByIndex(tSCD.ptNavMeshInst, tNavMeshHandle.nSquareIndex, nSquareX, nSquareY);

    if (nSquareX >= tOffset.x && nSquareY >= tOffset.y && nSquareX < tOffset.x + tSize.x && nSquareY < tOffset.y + tSize.y) {
        bInRegion = TRUE;
    }
    return bInRegion;
}


































































































































void NavMeshGetAdjacentHandles(const NavMeshHandle & tNavMeshHandleStart, NavMeshHandle * arCardinalNavMeshHandles, NavMeshHandle * arCornerNavMeshHandles)
{
    const int32 sDirTable[4][2] = {
        { ADJ_POS_Y, ADJ_NEG_Y },
        { ADJ_POS_X, ADJ_NEG_X },
        { ADJ_POS_Y, ADJ_NEG_Y },
        { ADJ_POS_X, ADJ_NEG_X },
    };
    const int32 sCornerTable[4][2] = {
        { CRNR_NEG_X_POS_Y, CRNR_NEG_X_NEG_Y },
        { CRNR_POS_X_POS_Y, CRNR_NEG_X_POS_Y },
        { CRNR_POS_X_POS_Y, CRNR_POS_X_NEG_Y },
        { CRNR_POS_X_NEG_Y, CRNR_NEG_X_NEG_Y },
    };

    int32 i;
    for (i = 0; i < 4; i++) {
        NavMeshHandle tNavMeshHandle;
        NavMeshGetAdjacentHandle(tNavMeshHandleStart, i, tNavMeshHandle);
        arCardinalNavMeshHandles[i] = tNavMeshHandle;
    }

    if (arCornerNavMeshHandles == NULL) {
        return;
    }

    for (i = 0; i < 4; i++) {
        arCornerNavMeshHandles[i] = NAVMESH_HANDLE_NONE;
    }

    uint32 dwCornerFlags = 0;
    for (int32 nDir = 0; nDir < 4; nDir++) {
        if (NavMeshHandleIsNone(arCardinalNavMeshHandles[nDir])) {
            continue;
        }
        for (i = 0; i < 2; i++) {
            int32 nCorner = sCornerTable[nDir][i];
            if (dwCornerFlags & (1 << nCorner)) {
                continue;
            }
            dwCornerFlags |= 1 << nCorner;
            int32 nAdjDirection = sDirTable[nDir][i];
            NavMeshHandle tNavMeshHandle;
            NavMeshGetAdjacentHandle(arCardinalNavMeshHandles[nDir], nAdjDirection, tNavMeshHandle);
            arCornerNavMeshHandles[nCorner] = tNavMeshHandle;
        }
    }
}



































SSceneID NavMeshHandleToSubSceneSceneID(const NavMeshHandle & tNavMeshHandle)
{
    SSceneID idActualScene = tNavMeshHandle.idSScene;

    NavMeshInst * ptNavMeshInst = NavMeshInstGetByScene(idActualScene);
    if (ptNavMeshInst == NULL) {
        return -1;
    }

    SceneCommonData * ptSCD = SceneCommonDataTryToGet(ptNavMeshInst->idSScene);
    if (ptSCD && ptSCD->ptNavMeshInst && ptSCD->tChildSceneIDs.Count() > 0) {
        int32 nParentSpaceX;
        int32 nParentSpaceY;
        NavMeshGetPositionByIndex(ptSCD->ptNavMeshInst, tNavMeshHandle.nSquareIndex, nParentSpaceX, nParentSpaceY);

        POS pos = ptSCD->tChildSceneIDs.GetTailPosition();
        while (pos) {
            SSceneID idChildScene = ptSCD->tChildSceneIDs.GetPrev(pos);
            SceneCommonData * ptSCDChild = SceneCommonDataTryToGet(idChildScene);
            if (ptSCDChild) {
                Vector3D wpOffset = ptSCDChild->transformWorld.wp - ptSCD->transformWorld.wp;

                ScopedSceneDefinitionLockR pdefChild(ptSCDChild->snoScene);
                int32 nChildSizeX = pdefChild->tNavMeshData.nSizeX;
                int32 nChildSizeY = pdefChild->tNavMeshData.nSizeY;

                IVector2D tOffset;
                tOffset.x = (int32)(wpOffset.x / 2.5f);
                tOffset.y = (int32)(wpOffset.y / 2.5f);

                if (nParentSpaceX >= tOffset.x && nParentSpaceX < tOffset.x + nChildSizeX &&
                    nParentSpaceY >= tOffset.y && nParentSpaceY < tOffset.y + nChildSizeY) {
                    idActualScene = idChildScene;
                    break;
                }
            }
        }
    }
    return idActualScene;
}






















SNO NavMeshGetLevelAreaAtScenePos(SSceneID idSScene, const Vector2D & vPos2D)
{



    SNO snoLevelArea = -1;

    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);

    if (ptSCD) {
        ScopedSceneDefinitionLockR pdefScene(ptSCD->snoScene);
        int32 nSizeX = pdefScene->tNavMeshData.nSizeX;
        int32 nSizeY = pdefScene->tNavMeshData.nSizeY;
        IVector2D tOffset;
        tOffset.x = (int32)(vPos2D.x / 2.5f);
        tOffset.y = (int32)(vPos2D.y / 2.5f);

        if (tOffset.x >= 0 && tOffset.y >= 0 && tOffset.x < nSizeX && tOffset.y < nSizeY) {
            int32 nSquareIndex = NavMeshGetIndexByPosition(&pdefScene->tNavMeshData, tOffset.x, tOffset.y);
            const NavMeshSquare & tSquare = NavMeshIndexToSquare(&pdefScene->tNavMeshData, nSquareIndex, TRUE);
            int32 nLevelAreaIndex = NavMeshLevelAreaIndexFromFlags(tSquare.dwSquareFlags);

            snoLevelArea = ptSCD->tSceneSpec.arSnoLevelAreas[nLevelAreaIndex];
            if (snoLevelArea == -1) {
                snoLevelArea = ptSCD->tSceneSpec.arSnoLevelAreas[0];
            }
        }
    }

    return snoLevelArea;
}
