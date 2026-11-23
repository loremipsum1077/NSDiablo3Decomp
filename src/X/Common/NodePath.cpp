// Original file: X/Common/NodePath.cpp

#include "NodePath.h"
#include "ActorCommonData.h"
#include "Game.h"
#include "GameCommonData.h"
#include "TeamCommon.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Ray.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/RefString.h"
#include "../Sigma/TypeDescriptor/SymbolicValue.h"

enum {
    MAX_PATHS_PER_GAME = 32,
};

struct PathGameGlobals {  // size 0x8
    TDataArray<NodePath, int> * ptPaths;  // 0x00
};

// Belongs in X/Common/SNO/SymbolicValue.h (defined at SymbolicValue.cpp:4115).
extern const SymbolicEntry gPathClasses[16];














void NodePathInitializeForGame()
{

    GameCommonData & tCGD = GameGetActiveGameCommonData();

    tCGD.ptPathGlobals = (PathGameGlobals *)SigmaMemoryNew(sizeof(PathGameGlobals), 4, NULL, TRUE);
    tCGD.ptPathGlobals->ptPaths = TDataArrayCreateTracked<NodePath, int>("Paths", NULL, MAX_PATHS_PER_GAME);
}




static void sPathDispose(NodePath * ptPath)
{

    SigmaMemoryFree(ptPath->arPathNodes, NULL); ptPath->arPathNodes = NULL;
}




void NodePathDisposeForGame()
{

    GameCommonData & tCGD = GameGetActiveGameCommonData();

    PathGameGlobals * ptPathGameGlobals = tCGD.ptPathGlobals;

    TDataArrayIterator<NodePath> tIter;
    while (ptPathGameGlobals->ptPaths->Iterate(tIter))
    {
        NodePath * ptPath = tIter.ptElement;

        sPathDispose(ptPath);
    }

    TDataArrayDispose(tCGD.ptPathGlobals->ptPaths);

    SigmaMemoryFree(tCGD.ptPathGlobals, NULL); tCGD.ptPathGlobals = NULL;
}




static PathGameGlobals * sGetPathGameGlobals()
{

    GameCommonData & tCGD = GameGetActiveGameCommonData();



    return tCGD.ptPathGlobals;
}




static NodePath * sPathGetPath(PathID idPath)
{

    PathGameGlobals * ptPathGameGlobals = sGetPathGameGlobals();

    return ptPathGameGlobals->ptPaths->ElementGet(idPath);
}




static void sComputePath(NodePath * ptPath)
{

    ptPath->dwFlags &= ~(1 << PATH_FLAG_DIRTY);

    int32 nStartNodeIdx = -1;
    int32 nEndNodeIdx = -1;

    for (int32 i = 0; i < ptPath->nPathNodeCount; i++)
    {
        NodePathNode * ptPathNode = &ptPath->arPathNodes[i];
        if (ptPathNode->dwFlags & (1 << PATH_NODE_FLAG_PATH_END))
        {
            if (nStartNodeIdx == -1)
            {
                nStartNodeIdx = i;
            }
            else if (nEndNodeIdx == -1)
            {
                nEndNodeIdx = i;
            }
        }
    }






    if (nStartNodeIdx == -1)
    {
        return;
    }



    int32 nNodeIndexLast = ptPath->nPathNodeCount - 1;

    if (nStartNodeIdx != 0)
    {
        NodePathNode tNodeTemp = ptPath->arPathNodes[0];
        ptPath->arPathNodes[0] = ptPath->arPathNodes[nStartNodeIdx];
        ptPath->arPathNodes[nStartNodeIdx] = tNodeTemp;
    }

    if (nEndNodeIdx != -1)
    {
        NodePathNode tNodeTemp = ptPath->arPathNodes[nNodeIndexLast];
        ptPath->arPathNodes[nNodeIndexLast] = ptPath->arPathNodes[nEndNodeIdx];
        ptPath->arPathNodes[nEndNodeIdx] = tNodeTemp;

        nNodeIndexLast--;
    }



    for (int32 i = 0; i < nNodeIndexLast - 1; i++)
    {
        NodePathNode * ptNodeCmp = &ptPath->arPathNodes[i];
        WorldDistance wdBestSqr = 1e10f;
        int32 nNextIndex = -1;

        for (int32 j = i + 1; j <= nNodeIndexLast; j++)
        {
            NodePathNode * ptNode = &ptPath->arPathNodes[j];

            WorldDistance wdDistSqr = Vector3D::DistanceSquared(ptNodeCmp->wpPos, ptNode->wpPos);

            if (wdDistSqr < wdBestSqr)
            {
                wdBestSqr = wdDistSqr;
                nNextIndex = j;
            }
        }



        if (nNextIndex != i + 1)
        {

            NodePathNode tNodeTemp = ptPath->arPathNodes[i + 1];
            ptPath->arPathNodes[i + 1] = ptPath->arPathNodes[nNextIndex];
            ptPath->arPathNodes[nNextIndex] = tNodeTemp;
        }
    }
}




static NodePath * sPathGetComputedPath(PathID idPath)
{

    NodePath * ptPath = sPathGetPath(idPath);

    if (ptPath->dwFlags & (1 << PATH_FLAG_DIRTY))
    {
        sComputePath(ptPath);
    }

    return ptPath;
}




PathID NodePathCreate(int32 nMaxPathNodeCount, PathClass ePathClass, SWorldID idSWorld)
{



    PathGameGlobals * ptPathGameGlobals = sGetPathGameGlobals();

    PathID idPath = ptPathGameGlobals->ptPaths->ElementNew();

    if (idPath != PATHID_INVALID)
    {
        NodePath * ptPath = ptPathGameGlobals->ptPaths->ElementGet(idPath);
        ptPath->dwFlags = 0;
        ptPath->nPathNodeMaxCount = nMaxPathNodeCount;
        ptPath->nPathNodeCount = 0;

        ptPath->arPathNodes = (NodePathNode *)SigmaMemoryNew(nMaxPathNodeCount * sizeof(NodePathNode), 4, NULL, TRUE);
        ptPath->ePathClass = ePathClass; ptPath->idSWorld = idSWorld;
    }
    else
    {
        int32 nCount = 0;
        TDataArrayIterator<NodePath> tIter;



        while (ptPathGameGlobals->ptPaths->Iterate(tIter))
        {
            nCount++;
        }








        DisplayInternalError(NULL, "", 252, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    return idPath;
}




PathID NodePathFind(PathClass ePathClass, SWorldID idSWorld)
{
    PathID idPath = PATHID_INVALID;

    PathGameGlobals * ptPathGameGlobals = sGetPathGameGlobals();

    TDataArrayIterator<NodePath> tIter;


    while (ptPathGameGlobals->ptPaths->Iterate(tIter))
    {
        NodePath * ptPath = tIter.ptElement;

        if (ptPath->ePathClass == ePathClass &&
            ptPath->idSWorld == idSWorld)
        {
            idPath = ptPath->id;
            break;
        }
    }

    return idPath;
}




void NodePathDispose(PathID idPath)
{

    PathGameGlobals * ptPathGameGlobals = sGetPathGameGlobals();

    NodePath * ptPath = ptPathGameGlobals->ptPaths->ElementGet(idPath);

    sPathDispose(ptPath);

    ptPathGameGlobals->ptPaths->ElementDelete(ptPath);
}




















void NodePathsDeleteForWorld(SWorldID idSWorld)
{

    PathGameGlobals * ptPathGameGlobals = sGetPathGameGlobals();

    TDataArrayIterator<NodePath> tIter;
    while (ptPathGameGlobals->ptPaths->Iterate(tIter))
    {
        NodePath * ptPath = ptPathGameGlobals->ptPaths->ElementGet(tIter->id);
        if (ptPath->idSWorld == idSWorld)
        {
            NodePathDispose(tIter->id);
        }
    }
}



























































































BOOL NodePathAddNode(PathID idPath, uint32 tName, const Vector3D & wpPos, float flRadius, uint32 dwFlags, Team eTeam)
{
    BOOL bAdded = FALSE;






    NodePath * ptPath = sPathGetPath(idPath);

    if (ptPath->nPathNodeCount < ptPath->nPathNodeMaxCount)
    {
        NodePathNode * ptPathNode = &ptPath->arPathNodes[ptPath->nPathNodeCount];

        ptPathNode->tName = tName;



        ptPathNode->wpPos = wpPos;
        ptPathNode->dwFlags = dwFlags;
        ptPathNode->flRadius = flRadius;
        ptPathNode->eTeam = eTeam;

        ptPath->nPathNodeCount++;

        ptPath->dwFlags |= (1 << PATH_FLAG_DIRTY); bAdded = TRUE;
    }
    else
    {
        CRefString sPathType = SymbolToName(ptPath->ePathClass, gPathClasses);




    }

    return bAdded;
}











































PathNodeID sGetIDFromIndex(PathID idPath, int32 nIndex)
{


    if (nIndex >= 0)
    {
        NodePath * ptPath = sPathGetPath(idPath);
        if (ptPath)
        {
            if (nIndex < ptPath->nPathNodeCount)
            {

                return nIndex | 0x8000;
            }
        }
    }

    return PATHNODEID_INVALID;
}




PathNodeID NodePathGetNodeByName(PathID idPath, uint32 tName)
{
    int32 nWinningIdx = -1;


    NodePath * ptPath = sPathGetComputedPath(idPath);





    for (int32 i = 0; i < ptPath->nPathNodeCount; i++)
    {
        NodePathNode * ptPathNodeCur = &ptPath->arPathNodes[i];


        if (ptPathNodeCur->tName == tName)
        {
            nWinningIdx = i;
            break;
        }
    }




    return sGetIDFromIndex(idPath, nWinningIdx);
}







































































NodePathNode * NodePathGetNode(PathID idPath, PathNodeID idPathNode)
{


    NodePath * ptPath = sPathGetPath(idPath);
    if (!ptPath)
        return NULL;

    int32 nIndex = idPathNode & ~0x8000;
    if (nIndex < 0 || nIndex >= ptPath->nPathNodeCount)
        return NULL;
    return &ptPath->arPathNodes[nIndex];
}























PathNodeID NodePathGetNextNodeAndPosition(PathID idPath, const Vector3D & wpPos, const NodePathNode * ptPathNodeDest, Vector3D & wpPosClosest)
{
    int32 nPathIndex = -1;




    NodePath * ptPath = sPathGetComputedPath(idPath);

    if (ptPath->nPathNodeCount >= 2)
    {
        WorldDistance wdBestDist = 100000.0f;

        for (int32 i = 0; i < ptPath->nPathNodeCount - 1; i++)
        {
            NodePathNode * ptPathNode = &ptPath->arPathNodes[i];
            NodePathNode * ptPathNodeNext = ptPathNode + 1;

            LineSeg3D tLineSeg;

            tLineSeg.Set(ptPathNode->wpPos, ptPathNodeNext->wpPos);

            Vector3D wpClosestPt;

            WorldDistance wdDist = tLineSeg.ClosestPoint(wpPos, wpClosestPt);

            if (wdDist < wdBestDist)
            {
                wdBestDist = wdDist;
                wpPosClosest = wpClosestPt;


                int32 nOffset0 = ptPathNodeDest - ptPathNode;
                int32 nOffset1 = ptPathNodeDest - ptPathNodeNext;
                int32 nNodeDist0 = nOffset0 < 0 ? -nOffset0 : nOffset0;
                int32 nNodeDist1 = nOffset1 < 0 ? -nOffset1 : nOffset1;

                nPathIndex = (nNodeDist0 < nNodeDist1) ? i : i + 1;
            }
        }
    }








    else if (ptPath->nPathNodeCount == 1)
    {
        wpPosClosest = ptPath->arPathNodes[0].wpPos;
        nPathIndex = 0;
    }
    else
    {
        wpPosClosest = wpPos;
    }



    return sGetIDFromIndex(idPath, nPathIndex);
}





PathNodeID NodePathGetNextClosestNode(PathID idPath, PathNodeID idCurrNode, PathNodeID idDestNode)
{



    NodePath * ptPath = sPathGetComputedPath(idPath);
    if (!ptPath || idCurrNode == PATHNODEID_INVALID || idDestNode == PATHNODEID_INVALID)
    {
        return PATHNODEID_INVALID;
    }
    {

        int32 nCurrIndex = idCurrNode & ~0x8000;
        int32 nDestIndex = idDestNode & ~0x8000;
        int32 nWinningIndex;


        if (nCurrIndex == nDestIndex)
        {
            nWinningIndex = nCurrIndex;
        }
        else if (nDestIndex < nCurrIndex)
        {
            nWinningIndex = (nCurrIndex - 1 < 0) ? 0 : nCurrIndex - 1;
        }
        else
        {
            nWinningIndex = Math::Min(ptPath->nPathNodeCount - 1, nCurrIndex + 1);
        }

        return sGetIDFromIndex(idPath, nWinningIndex);
    }

    return PATHNODEID_INVALID;
}





PathNodeID NodePathGetPrevNode(PathID idPath, PathNodeID idCurrNode, PathNodeID idDestNode)
{



    NodePath * ptPath = sPathGetComputedPath(idPath);
    if (!ptPath || idCurrNode == PATHNODEID_INVALID || idDestNode == PATHNODEID_INVALID)
    {
        return PATHNODEID_INVALID;
    }
    {

        int32 nCurrIndex = idCurrNode & ~0x8000;
        int32 nDestIndex = idDestNode & ~0x8000;
        int32 nWinningIndex;


        if (nDestIndex >= nCurrIndex)
        {
            nWinningIndex = (nCurrIndex - 1 < 0) ? 0 : nCurrIndex - 1;
        }
        else
        {
            nWinningIndex = Math::Min(ptPath->nPathNodeCount - 1, nCurrIndex + 1);
        }

        return sGetIDFromIndex(idPath, nWinningIndex);
    }

    return PATHNODEID_INVALID;
}




BOOL NodePathIsOnPath(PathID idPath, PathNodeID idNode)
{


    NodePath * ptPath = sPathGetPath(idPath);
    if (ptPath)
    {
        for (int32 i = 0; i < ptPath->nPathNodeCount; i++)
        {
            PathNodeID idCurrNode = sGetIDFromIndex(idPath, i);
            if (idCurrNode == idNode)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}




CRefString NodePathGetEndNodeForActor(ActorCommonData & tACD, PathClass ePathClass)
{


    Team eTeam = tACD.GetTeam();
    SWorldID idSWorld = tACD.GetSWorldID();
    WorldDistance wdLargestDistanceSquared = 0.0f;
    PathGameGlobals * ptPathGameGlobals = sGetPathGameGlobals();
    NodePathNode * ptFarthestNode = NULL;

    TDataArrayIterator<NodePath> tIter;





    while (ptPathGameGlobals->ptPaths->Iterate(tIter))
    {
        NodePath * ptPath = tIter.ptElement;

        if (ptPath->idSWorld != idSWorld || ptPath->ePathClass != ePathClass)
        {
            continue;
        }

        for (int32 i = 0; i < ptPath->nPathNodeCount; i++)
        {
            NodePathNode * ptPathNode = &ptPath->arPathNodes[i];

            if (!(ptPathNode->dwFlags & (1 << PATH_NODE_FLAG_PATH_END)))
            {
                continue;
            }

            if (!TeamsAreFriendly(ptPathNode->eTeam, eTeam))
            {
                continue;
            }

            WorldDistance wdDistanceSquared = Vector3D::DistanceSquared(tACD.GetPlace().wp, ptPathNode->wpPos);
            if (wdDistanceSquared > wdLargestDistanceSquared)
            {
                wdLargestDistanceSquared = wdDistanceSquared;
                ptFarthestNode = ptPathNode;
            }
        }
    }

    if (ptFarthestNode)
    {
        return CRefString(ptFarthestNode->tName);
    }

    return CRefString("");
}
