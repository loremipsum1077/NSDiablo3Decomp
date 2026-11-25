#include "NavCellPath.h"
#include "NavCellPathInternal.h"
#include "NavZone.h"
#include "../Definitions/SceneDefinition.h"
#include "../SceneCommon.h"
#include <alloca.h>
#include "../Game.h"
#include "../GameCommonData.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include <new>









































template <> inline uint32 XHashKey<const NavCellHandle &>(const NavCellHandle & tNavMeshHandle)
{

    uint32 dwKey = (tNavMeshHandle.idSScene << 16) | tNavMeshHandle.uCellIndex;




    uint32 hash = 0x811C9DC5;
    hash ^= dwKey & 0xFF;
    hash *= 0x01000193;
    hash ^= (dwKey >> 8) & 0xFF;
    hash *= 0x01000193;
    hash ^= (dwKey >> 16) & 0xFF;
    hash *= 0x01000193;
    hash ^= (dwKey >> 24) & 0xFF;
    hash *= 0x01000193;

    return hash;
}


void NavCellPathFindDataInitialize(NavCellPathFindData & tPathFindData)
{

    SigmaMemoryClear(&tPathFindData, sizeof(tPathFindData));

    tPathFindData.hNavCellStart.idSScene = -1;
    tPathFindData.hNavCellStart.uCellIndex = (uint32)-1;

    tPathFindData.hNavCellEnd.idSScene = -1;
    tPathFindData.hNavCellEnd.uCellIndex = (uint32)-1;
}


void NavCellPathInitializeForGame()
{
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    NavCellPathGlobals *& ptNavCellPathGlobals = GameGetActiveGameCommonData().ptNavCellPathGlobals;
    ptNavCellPathGlobals = (NavCellPathGlobals *)SigmaMemoryNew(sizeof(NavCellPathGlobals), 4, ptMemoryInterface, TRUE);

    void * pMem = SigmaMemoryNew(sizeof(XChainedFixedBlockAllocator<NavCellPathNode>), 4, ptMemoryInterface, FALSE);
    ptNavCellPathGlobals->pPathNodeAllocator = new (pMem) XChainedFixedBlockAllocator<NavCellPathNode>;
    ptNavCellPathGlobals->pPathNodeAllocator->SetMemoryInterface(ptMemoryInterface);
    ptNavCellPathGlobals->pPathNodeAllocator->SetBlockCount_Tracked(500);

    ptNavCellPathGlobals->ptPathOpenList = (NavCellPathNodePQueue *)SigmaMemoryNew(sizeof(NavCellPathNodePQueue), 4, ptMemoryInterface, TRUE);
    ptNavCellPathGlobals->ptPathOpenList->Initialize(256);

    ptNavCellPathGlobals->ptMasterNodeList = (NavCellPathNodeMap *)SigmaMemoryNew(sizeof(NavCellPathNodeMap), 4, ptMemoryInterface, TRUE);
    ptNavCellPathGlobals->ptMasterNodeList->Initialize(16, FALSE, ptMemoryInterface);
    ptNavCellPathGlobals->ptMasterNodeList->SetHashTableSize_Tracked(256);

    ptNavCellPathGlobals->ptPathPointPool = (TPathPoint2DPool *)SigmaMemoryNew(sizeof(TPathPoint2DPool), 4, ptMemoryInterface, TRUE);
    ptNavCellPathGlobals->ptPathPointPool->Initialize();
    ptNavCellPathGlobals->ptPathPointPool->SetBlockCount_Tracked(256);
}



void NavCellPathDisposeForGame()
{

    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    NavCellPathGlobals *& ptNavCellPathGlobals = GameGetActiveGameCommonData().ptNavCellPathGlobals;
    if (ptNavCellPathGlobals)
    {
        ptNavCellPathGlobals->pPathNodeAllocator->~XChainedFixedBlockAllocator();
        SigmaMemoryFree(ptNavCellPathGlobals->pPathNodeAllocator, ptMemoryInterface);
        ptNavCellPathGlobals->ptPathOpenList->Dispose();
        SigmaMemoryFree(ptNavCellPathGlobals->ptPathOpenList, ptMemoryInterface);
        ptNavCellPathGlobals->ptMasterNodeList->Dispose();
        SigmaMemoryFree(ptNavCellPathGlobals->ptMasterNodeList, ptMemoryInterface);
        ptNavCellPathGlobals->ptPathPointPool->Dispose();
        SigmaMemoryFree(ptNavCellPathGlobals->ptPathPointPool, ptMemoryInterface);
        SigmaMemoryFree(ptNavCellPathGlobals, ptMemoryInterface); ptNavCellPathGlobals = NULL;
    }
}


TPathPoint2DPool * NavCellPathGetPointPool()
{

    NavCellPathGlobals * ptNavCellPathGlobals = GameGetActiveGameCommonData().ptNavCellPathGlobals;

    return ptNavCellPathGlobals->ptPathPointPool;
}


BOOL NavCellFindPath(NavCellPathFindData & tPathFindData)
{

    NavCellPathGlobals * ptNavCellPathGlobals = GameGetActiveGameCommonData().ptNavCellPathGlobals;
    NavCellPathFinder tPathFinder(tPathFindData,
        *ptNavCellPathGlobals->pPathNodeAllocator,
        *ptNavCellPathGlobals->ptPathOpenList,
        *ptNavCellPathGlobals->ptMasterNodeList);

    BOOL bFoundPath = tPathFinder.FindPath();

    return bFoundPath;
}






NavCellPathFinder::NavCellPathFinder(NavCellPathFindData & tPathFindData, XChainedFixedBlockAllocator<NavCellPathNode> & tPathNodeAllocator, NavCellPathNodePQueue & tOpenList, NavCellPathNodeMap & tMasterNodeList)




    : m_tPathFindData(tPathFindData), m_tPathNodeAllocator(tPathNodeAllocator)

    , m_tOpenList(tOpenList), m_tMasterNodeList(tMasterNodeList)

    , m_ptBestNode(NULL)
    , m_flMaxRangeFromDstSqr(tPathFindData.flMaxRangeFromDest * tPathFindData.flMaxRangeFromDest)

    , m_nNavZoneInstanceCount(0)
{
    m_tEndLocation.hNavCell = tPathFindData.hNavCellEnd;
    m_tEndLocation.wpPos = *(const Vector2D *)&tPathFindData.wpNavCellEnd;
}






NavCellPathFinder::~NavCellPathFinder()
{
    for (int i = 0; i < m_nNavZoneInstanceCount; i++)
    {
        m_arNavZoneInstances[i].ptNavZoneInstance->UnlockZoneDef();
    }
}





















































































































































float NavCellPathFinder::GetHeuristicDistance(NavCellPathNode & tNodeCur)

{

    Vector2D wpClosestEnd;

    ClosestPointToNavCell(tNodeCur.tLocation.hNavCell, m_tEndLocation.wpPos, wpClosestEnd);

    float flHeuristicDist = Vector2D::Distance(tNodeCur.tLocation.wpPos, wpClosestEnd);




    flHeuristicDist += Vector2D::Distance(wpClosestEnd, m_tEndLocation.wpPos);
    return flHeuristicDist;
}



void NavCellPathFinder::ClosestPointToNavCell(const NavCellHandle & hNavCell, const Vector2D & wpGiven, Vector2D & wpClosest)

{


    NavCellGetClosestPointToNavCell(hNavCell, GetNavZoneInst(hNavCell), wpGiven, wpClosest);
}


void NavCellPathFinder::InitializeNode(NavCellPathNode & tNode, const NavCellPathNode * ptParentNode, const NavCellHandle & hNavCell)



{
    tNode.uFlags = 0; tNode.nOpenListHeapIndex = -1;
    tNode.ptParentNode = ptParentNode;

    tNode.tLocation.hNavCell = hNavCell;
    if (hNavCell != m_tEndLocation.hNavCell)
    {
        ClosestPointToNavCell(hNavCell, ptParentNode->tLocation.wpPos, tNode.tLocation.wpPos);
        tNode.flCost = ptParentNode->flCost + Vector2D::Distance(ptParentNode->tLocation.wpPos, tNode.tLocation.wpPos);
        tNode.flHeuristicDistance = GetHeuristicDistance(tNode);
    }
    else
    {

        ClosestPointToNavCell(ptParentNode->tLocation.hNavCell, m_tEndLocation.wpPos, tNode.tLocation.wpPos);
        tNode.flCost = ptParentNode->flCost + Vector2D::Distance(ptParentNode->tLocation.wpPos, tNode.tLocation.wpPos) + Vector2D::Distance(tNode.tLocation.wpPos, m_tEndLocation.wpPos); tNode.flHeuristicDistance = 0.0f;
    }

    tNode.flEstimatedDistance = tNode.flCost + tNode.flHeuristicDistance;
}


































































































































































































































































































































































































































































const NavZoneInstance * NavCellPathFinder::GetNavZoneInst(const NavCellHandle & hNavCell)

{


    BOOL bFound = FALSE;
    const NavZoneInstance * ptNavZoneInstance = NULL;



    for (int i = 0; i < m_nNavZoneInstanceCount; i++)
    {
        if (m_arNavZoneInstances[i].idSScene == hNavCell.idSScene)
        {
            ptNavZoneInstance = m_arNavZoneInstances[i].ptNavZoneInstance;
            bFound = TRUE;
            break;
        }
    }

    if (!bFound)
    {
        ptNavZoneInstance = NavCellHandleToZoneInstance(hNavCell);
        if (ptNavZoneInstance)
        {







            m_arNavZoneInstances[m_nNavZoneInstanceCount].idSScene = hNavCell.idSScene;
            m_arNavZoneInstances[m_nNavZoneInstanceCount].ptNavZoneInstance = ptNavZoneInstance;

            ptNavZoneInstance->LockZoneDef();
            m_nNavZoneInstanceCount++;
        }
    }

    return ptNavZoneInstance;
}


























































































































Vector2D sgarEdgeOffsets[4] = {
    Vector2D(-2.5f, 0.0f),
    Vector2D(2.5f, 0.0f),
    Vector2D(0.0f, -2.5f),
    Vector2D(0.0f, 2.5f)
};

inline NavCellPathNode* NavCellPathNodePQueue::PopFront()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    NavCellPathHeapNode* ptFront = m_tNodeHeap.GetFront();
    NavCellPathNode* ptPathNode = ptFront->ptPathNode;
    m_tNodeHeap.PopFront();
    ptPathNode->uFlags &= ~(1 << PNF_ON_OPEN_LIST_BIT);
    ptPathNode->nOpenListHeapIndex = -1;
    return ptPathNode;
}
inline void NavCellPathNodePQueue::Push(NavCellPathNode* ptPathNode)  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    NavCellPathHeapNode tNode;
    tNode.flEstimatedDistance = ptPathNode->flEstimatedDistance;
    tNode.ptPathNode = ptPathNode;
    m_tNodeHeap.InsertNode(tNode);
    ptPathNode->uFlags |= 1 << PNF_ON_OPEN_LIST_BIT;
}
inline NavCellPathHeapNode* NavCellPathNodePQueue::FindNodeAtLocation(NavCellPathLocation& tLocation, NavCellPathNodeMap& tMasterNodeList)  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    NavCellPathNode* ptPathNode;
    if (tMasterNodeList.Lookup(tLocation.hNavCell, ptPathNode))
        return m_tNodeHeap.GetNode(ptPathNode->nOpenListHeapIndex);
    return NULL;
}
inline void NavCellPathNodePQueue::UpdateCost(NavCellPathHeapNode* ptPathHeapNode)  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    int nIndex = m_tNodeHeap.GetNodeIndex(ptPathHeapNode);
    float flEstimatedDistance = ptPathHeapNode->flEstimatedDistance;
    ptPathHeapNode->flEstimatedDistance = ptPathHeapNode->ptPathNode->flEstimatedDistance;
    if (ptPathHeapNode->flEstimatedDistance < flEstimatedDistance)
        m_tNodeHeap.UpHeap(nIndex);
    else if (ptPathHeapNode->flEstimatedDistance > flEstimatedDistance)
        m_tNodeHeap.DownHeap(nIndex);
}
inline void NavCellPathNodePQueue::RemoveAll()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    m_tNodeHeap.RemoveAll();
}
NavCellPathNode* NavCellPathFinder::CreateNewPathNode(NavCellPathLocation& tPathLoc)
{
    NavCellPathNode* ptNewNode = m_tPathNodeAllocator.Alloc_Tracked();
    ptNewNode->tLocation = tPathLoc;
    ptNewNode->ptParentNode = NULL;
    ptNewNode->flCost = 0.0f;
    ptNewNode->flHeuristicDistance = GetHeuristicDistance(*ptNewNode);
    ptNewNode->flEstimatedDistance = ptNewNode->flHeuristicDistance;
    ptNewNode->uFlags = 0;
    ptNewNode->nOpenListHeapIndex = -1;
    return ptNewNode;
}
NavCellPathNode* NavCellPathFinder::CreateNewPathNode(NavCellPathNode& tCopy)
{
    NavCellPathNode* ptNewNode = m_tPathNodeAllocator.Alloc_Tracked();
    *ptNewNode = tCopy;
    return ptNewNode;
}
BOOL NavCellPathFinder::IsNodeAtDest(NavCellPathNode* ptNode)
{
    if (ptNode->tLocation.hNavCell == m_tEndLocation.hNavCell) return TRUE;
    if (m_tPathFindData.flMaxRangeFromDest > 1e-6f)
    {
        float flSeparationSquared = Vector2D::DistanceSquared(ptNode->tLocation.wpPos, m_tEndLocation.wpPos);
        return flSeparationSquared < m_flMaxRangeFromDstSqr;
    }
    return FALSE;
}
void NavCellPathFinder::ModifyStartLocationForRoundedCorners(NavCellPathLocation& tStartLoc)
{
    NavCellModifyForRoundedCorners(GetNavZoneInst(tStartLoc.hNavCell), tStartLoc.hNavCell, tStartLoc.wpPos, m_tPathFindData.uOneRequiredFlags);
}
BOOL NavCellPathFinder::FindPath()
{
    NavCellPathLocation tStartLocation;
    tStartLocation.hNavCell = m_tPathFindData.hNavCellStart;
    tStartLocation.wpPos = *(const Vector2D*)&m_tPathFindData.wpPosStart;
    ModifyStartLocationForRoundedCorners(tStartLocation);
    m_tPathFindData.uNodesExplored = 0;
    NavCellPathNode* ptStartNode = CreateNewPathNode(tStartLocation);
    m_tOpenList.Push(ptStartNode);
    m_tMasterNodeList.SetAt_Tracked(ptStartNode->tLocation.hNavCell, ptStartNode);
    m_ptBestNode = ptStartNode;
    BOOL bFoundPath = FALSE;
    while (!m_tOpenList.IsEmpty())
    {
        NavCellPathNode* ptCurrentNode = m_tOpenList.PopFront();
        BOOL bAtDest = IsNodeAtDest(ptCurrentNode);
        if (bAtDest)
        {
            m_ptBestNode = ptCurrentNode;
            bFoundPath = TRUE;
            break;
        }
        ExploreNeighbors(ptCurrentNode);
        ptCurrentNode->uFlags &= ~(1 << PNF_ON_OPEN_LIST_BIT);
        ptCurrentNode->uFlags |= 1 << PNF_ON_CLOSED_LIST_BIT;
        ptCurrentNode->nOpenListHeapIndex = -1;
        ++m_tPathFindData.uNodesExplored;
        if (m_tPathFindData.uNodesExplored > m_tPathFindData.uMaxNodesToExplore) break;
    }
    if (m_tPathFindData.ptListPathPoints && (bFoundPath || m_ptBestNode != ptStartNode)) MakeSolutionPath();
    m_tOpenList.RemoveAll();
    m_tMasterNodeList.RemoveAll();
    m_tPathNodeAllocator.Reset();
    return bFoundPath;
}
// NON_MATCHING: 816 vs 808 after the re-lay on DWARF 390-453 (cost test at 409 per rows 409:10/32/56, null test at 415:9; bNewNode = TRUE at 403 inferred, it leaves no row): only the flHeuristicDistance/flEstimatedDistance copy at 423/424 (original ldp/stp, ours two ldr/str pairs; the second load is not moved above the first store).
#line 390
void NavCellPathFinder::ExploreNode(
    NavCellPathNode* ptParentNode,
    const NavCellHandle& hNavCell)
{
    NavCellPathNode* ptActualNode = NULL;
    BOOL bNewNode = FALSE;
    NavCellPathNode tNewNode;

    InitializeNode(tNewNode, ptParentNode, hNavCell);

    if ( !m_tMasterNodeList.Lookup( hNavCell, ptActualNode ) )
    {
        ptActualNode = CreateNewPathNode( tNewNode );
        bNewNode = TRUE;
        m_tMasterNodeList.SetAt_Tracked( hNavCell, ptActualNode );
    }



    if ( !bNewNode && tNewNode.flCost >= ptActualNode->flCost )
    {
        return;
    }


    if (!ptActualNode)
    {
        return;
    }

    ptActualNode->uFlags &= ~(1 << PNF_ON_CLOSED_LIST_BIT);

    ptActualNode->flCost = tNewNode.flCost;
    ptActualNode->flHeuristicDistance = tNewNode.flHeuristicDistance;
    ptActualNode->flEstimatedDistance = tNewNode.flEstimatedDistance;
    ptActualNode->ptParentNode = tNewNode.ptParentNode;
    ptActualNode->tLocation.wpPos = tNewNode.tLocation.wpPos;

    BOOL bOnOpen = (ptActualNode->uFlags & (1 << PNF_ON_OPEN_LIST_BIT)) != 0;
    if ( bOnOpen )
    {
        NavCellPathHeapNode* ptPathHeapNode = m_tOpenList.FindNodeAtLocation( tNewNode.tLocation, m_tMasterNodeList );




        m_tOpenList.UpdateCost( ptPathHeapNode );
    }
    else
    {

        if ( m_tOpenList.IsFull() )
        {
            return;
        }



        m_tOpenList.Push( ptActualNode );
    }

    if ( ptActualNode->flHeuristicDistance < m_ptBestNode->flHeuristicDistance ) m_ptBestNode = ptActualNode;

}
#line 1150
void NavCellPathFinder::ExploreNeighbors(NavCellPathNode* ptNode)
{
    const NavCellHandle& hNavCellCur = ptNode->tLocation.hNavCell;
    const NavZoneInstance* ptNavZoneInst = GetNavZoneInst(hNavCellCur);
    const NavZoneDefinition* ptNavZoneDef = ptNavZoneInst->GetZoneDef();
    const NavCell& tNavCell = ptNavZoneDef->arNavCells[hNavCellCur.uCellIndex];
    const NavCellLookup* arNeighbors = ptNavZoneDef->arNavCellNeighbors + tNavCell.uNeighborOffset;
    for (int i = 0; i < tNavCell.uNeighborCount; ++i)
    {
        const NavCellLookup& tLookup = arNeighbors[i];
        if ((int16)tLookup.uCellIndex < 0)
        {
            uint16 uIndex = tLookup.uCellIndex & 0x7fff;
            const NavCellBorderDataInst& tBorderDataInst = ptNavZoneInst->GetBorderDataInst(uIndex);
            for (int j = 0; j < tBorderDataInst.uSceneCount; ++j)
            {
                const NavCellBorderDataSceneInst& tSceneInst = ptNavZoneInst->GetBorderDataSceneInst(tBorderDataInst, j);
                const SceneCommonData* ptSCD = SceneCommonDataTryToGet(tSceneInst.idSScene);
                if (!ptSCD || !(ptSCD->dwSceneFlags & 1) || !ptSCD->ptNavZoneInst) continue;
                const NavCellLookup* arBorderNeighbors = ptNavZoneInst->arBorderLookups + tSceneInst.uNeighborOffset;
                for (int k = 0; k < tSceneInst.uNeighborCount; ++k)
                {
                    const NavCellLookup& tBorderLookup = arBorderNeighbors[k];
                    if (tBorderLookup.uCellFlags & m_tPathFindData.uOneRequiredFlags)
                    {
                        NavCellHandle hNavCellNeighbor;
                        hNavCellNeighbor.idSScene = tSceneInst.idSScene;
                        hNavCellNeighbor.uCellIndex = tBorderLookup.uCellIndex;
                        ExploreNode(ptNode, hNavCellNeighbor);
                    }
                }
            }
        }
        else if (tLookup.uCellFlags & m_tPathFindData.uOneRequiredFlags)
        {
            NavCellHandle hNavCellNeighbor;
            hNavCellNeighbor.idSScene = ptNavZoneInst->idSScene;
            hNavCellNeighbor.uCellIndex = tLookup.uCellIndex;
            ExploreNode(ptNode, hNavCellNeighbor);
        }
    }
}
void NavCellPathFinder::GetSharedEdge(const NavCellHandle& hNavCell1, const NavCellHandle& hNavCell2, NavCellEdge& tEdge, float flEdgeOffset)
{
    const NavZoneInstance* ptInst1 = GetNavZoneInst(hNavCell1);
    const NavZoneInstance* ptInst2 = GetNavZoneInst(hNavCell2);
    const NavCell* ptCell1 = NavCellHandleToNavCell(ptInst1->GetZoneDef(), hNavCell1);
    const NavCell* ptCell2 = NavCellHandleToNavCell(ptInst2->GetZoneDef(), hNavCell2);
    Vector2D wpMin1 = ptInst1->wpWorldMin + *(const Vector2D*)&ptCell1->tMin;
    Vector2D wpMax1 = ptInst1->wpWorldMin + *(const Vector2D*)&ptCell1->tMax;
    Vector2D wpMin2 = ptInst2->wpWorldMin + *(const Vector2D*)&ptCell2->tMin;
    Vector2D wpMax2 = ptInst2->wpWorldMin + *(const Vector2D*)&ptCell2->tMax;
    if (Math::Abs(wpMax1.x - wpMin2.x) < 0.1f)
    {
        tEdge.eType = NCET_X_AXIS;
        tEdge.uBorder = NCBD_MAX_X;
        tEdge.flAxis = wpMax1.x - flEdgeOffset;
        tEdge.flMin = Math::Max(wpMin1.y, wpMin2.y) + flEdgeOffset;
        tEdge.flMax = Math::Min(wpMax1.y, wpMax2.y) - flEdgeOffset;
    }
    else if (Math::Abs(wpMax2.x - wpMin1.x) < 0.1f)
    {
        tEdge.eType = NCET_X_AXIS;
        tEdge.uBorder = NCBD_MIN_X;
        tEdge.flAxis = wpMin1.x + flEdgeOffset;
        tEdge.flMin = Math::Max(wpMin1.y, wpMin2.y) + flEdgeOffset;
        tEdge.flMax = Math::Min(wpMax1.y, wpMax2.y) - flEdgeOffset;
    }
    else if (Math::Abs(wpMax1.y - wpMin2.y) < 0.1f)
    {
        tEdge.eType = NCET_Y_AXIS;
        tEdge.uBorder = NCBD_MAX_Y;
        tEdge.flAxis = wpMax1.y - flEdgeOffset;
        tEdge.flMin = Math::Max(wpMin1.x, wpMin2.x) + flEdgeOffset;
        tEdge.flMax = Math::Min(wpMax1.x, wpMax2.x) - flEdgeOffset;
    }
    else if (Math::Abs(wpMax2.y - wpMin1.y) < 0.1f)
    {
        tEdge.eType = NCET_Y_AXIS;
        tEdge.uBorder = NCBD_MIN_Y;
        tEdge.flAxis = wpMin1.y + flEdgeOffset;
        tEdge.flMin = Math::Max(wpMin1.x, wpMin2.x) + flEdgeOffset;
        tEdge.flMax = Math::Min(wpMax1.x, wpMax2.x) - flEdgeOffset;
    }
    else
    {
        tEdge.eType = NCET_X_AXIS;
        tEdge.flAxis = Math::Pin(wpMin1.x, wpMin2.x, wpMax2.x);
        tEdge.flMin = Math::Pin(wpMin1.y, wpMin2.y, wpMax2.y) + flEdgeOffset;
        tEdge.flMax = Math::Pin(wpMax1.y, wpMin2.y, wpMax2.y) - flEdgeOffset;
    }
    if (tEdge.flMax - tEdge.flMin < 0.1f)
    {
        if (tEdge.eType == NCET_Y_AXIS)
        {
            tEdge.flMax = tEdge.flAxis;
            tEdge.flAxis = tEdge.flMin;
            tEdge.flMin = tEdge.flMax;
        }
        tEdge.eType = NCET_POINT;
    }
}
BOOL NavCellPathFinder::GetClosestPointOnEdge(const Vector2D& wpA, const Vector2D& wpB, const NavCellEdge& tEdge, Vector2D& wpClosest)
{
    if (tEdge.eType == NCET_POINT) return FALSE;
    float dline = wpA.AsFloatArray()[tEdge.eType] - wpB.AsFloatArray()[tEdge.eType];
    if (Math::Abs(dline) < 1e-6f) return FALSE;
    float dedge = wpA.AsFloatArray()[tEdge.eType] - tEdge.flAxis;
    float t = dedge / dline;
    float daxis = wpB.AsFloatArray()[tEdge.eType ^ 1] - wpA.AsFloatArray()[tEdge.eType ^ 1];
    float flAxisIsect = wpA.AsFloatArray()[tEdge.eType ^ 1] + t * daxis;
    flAxisIsect = Math::Pin(flAxisIsect, tEdge.flMin, tEdge.flMax);
    if (Math::Abs(wpClosest.AsFloatArray()[tEdge.eType ^ 1] - flAxisIsect) <= 0.1f) return FALSE;
    (&wpClosest.x)[tEdge.eType ^ 1] = flAxisIsect;
    return TRUE;
}
#line 522
// NON_MATCHING: 1440 vs 1424 after the re-lay on DWARF 523-660 (was 1408 compact). x/y stores at 593/594 (DWARF has no inlined Vector2D::Set; Set gives 1408) put the unsmoothed AddHead blocks between the count loop and the alloca; the original places them last. Also 2 frame/spill differences around &arPathLocs[nCount - 1].
void NavCellPathFinder::MakeSolutionPath()

{


    PathPoint2DList& listPathPoints = *m_tPathFindData.ptListPathPoints;

    if ( m_tPathFindData.uPathSmoothingIterations == 0 )
    {
        const NavCellPathNode* ptNodeCur = m_ptBestNode;

        listPathPoints.AddHead_Tracked(m_tEndLocation.wpPos);

        while (ptNodeCur)
        {
            listPathPoints.AddHead_Tracked(ptNodeCur->tLocation.wpPos);

            ptNodeCur = ptNodeCur->ptParentNode;
        }
    }
    else
    {
        BOOL bUseSafeBorder = (m_tPathFindData.uPathSmoothingFlags & (1 << PSF_USE_SAFE_BORDER)) != 0;
        float flEdgeOffset = 0.0f;
        const NavCellPathNode* ptNodeCur = m_ptBestNode;
        int nCount = 1;

        if ( bUseSafeBorder )
        {
            flEdgeOffset = 1.25f;
        }

        while (ptNodeCur)
        {
            nCount++;
            ptNodeCur = ptNodeCur->ptParentNode;
        }



        NavCellPathLocation* arPathLocs = (NavCellPathLocation*)alloca(nCount * sizeof(NavCellPathLocation));
        NavCellEdge* arEdges = (NavCellEdge*)alloca(nCount * sizeof(NavCellEdge));


        {
            int nIndex = nCount - 1;

            arPathLocs[nIndex--] = m_tEndLocation;

            ptNodeCur = m_ptBestNode;
            while (ptNodeCur)
            {
                arPathLocs[nIndex--] = ptNodeCur->tLocation;


                ptNodeCur = ptNodeCur->ptParentNode;
            }
        }


        for (int i = 1; i < nCount - 1; ++i)
        {
            NavCellPathLocation& tLocPrev = arPathLocs[i - 1];
            NavCellPathLocation& tLocCur = arPathLocs[i];
            NavCellEdge& tEdge = arEdges[i];

            GetSharedEdge(tLocPrev.hNavCell, tLocCur.hNavCell, tEdge, flEdgeOffset);

            if ( tEdge.eType == NCET_POINT )
            {
                tLocCur.wpPos.x = tEdge.flAxis;
                tLocCur.wpPos.y = tEdge.flMin;
            }
            else
            {


                float* flPos = (float*)tLocCur.wpPos.AsFloatArray();  // DWARF: inlined const AsFloatArray() here

                flPos[tEdge.eType] = tEdge.flAxis;
            }
        }

        for (uint32 j = 0; j < m_tPathFindData.uPathSmoothingIterations; ++j)
        {
            int nMoves = 0;



            for (int i = 1; i < nCount - 1; ++i)
            {
                NavCellPathLocation& tLocPrev = arPathLocs[i - 1];
                NavCellPathLocation& tLocCur = arPathLocs[i];
                NavCellPathLocation& tLocNext = arPathLocs[i + 1];

                if (GetClosestPointOnEdge(tLocPrev.wpPos, tLocNext.wpPos, arEdges[i], tLocCur.wpPos))
                {
                    nMoves++;
                }
            }

            if (nMoves == 0)
            {
                break;
            }
        }



        Vector2D wpLastPos = arPathLocs[0].wpPos;
        listPathPoints.AddTail_Tracked(arPathLocs[0].wpPos);




        for (int i = 1; i < nCount - 1; ++i)
        {
            NavCellPathLocation& tLoc = arPathLocs[i];
            NavCellEdge& tEdge = arEdges[i];

            if ( Vector2D::DistanceSquared(tLoc.wpPos, wpLastPos) > 1.0f )
            {
                listPathPoints.AddTail_Tracked(tLoc.wpPos);
            }

            if ( bUseSafeBorder )
            {
                const Vector2D& vOffset = sgarEdgeOffsets[tEdge.uBorder];
                Vector2D wpPos = tLoc.wpPos + vOffset;

                listPathPoints.AddTail_Tracked(wpPos);
                wpLastPos = wpPos;
            }
        }

        listPathPoints.AddTail_Tracked(arPathLocs[nCount - 1].wpPos);
    }
}
