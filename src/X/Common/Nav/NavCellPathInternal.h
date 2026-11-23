// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Nav/NavCellPathInternal.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "NavCellHandle.h"
#include "NavCellPath.h"
#include "../../Sigma/DataStructures/XBinaryHeap.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/Math/Vector.h"

struct NavZoneInstance;

struct NavCellPathLocation {  // size 0x10
    NavCellHandle hNavCell;  // 0x00
    Vector2D wpPos;  // 0x08
};
static_assert(sizeof(NavCellPathLocation) == 0x10, "NavCellPathLocation");

enum {
    PNF_ON_OPEN_LIST_BIT = 0,
    PNF_ON_CLOSED_LIST_BIT = 1,
};

struct NavCellPathNode {  // size 0x28
    NavCellPathLocation tLocation;  // 0x00
    const NavCellPathNode * ptParentNode;  // 0x10
    float flCost;  // 0x18
    float flHeuristicDistance;  // 0x1c
    float flEstimatedDistance;  // 0x20
    uint16 uFlags;  // 0x24
    int16 nOpenListHeapIndex;  // 0x26
};
static_assert(sizeof(NavCellPathNode) == 0x28, "NavCellPathNode");

typedef XGrowableMap<NavCellHandle, const NavCellHandle &, NavCellPathNode *, NavCellPathNode *> NavCellPathNodeMap;

struct NavCellPathHeapNode {  // size 0x10
    float flEstimatedDistance;  // 0x00
    NavCellPathNode * ptPathNode;  // 0x08
    BOOL HeapCompare(NavCellPathHeapNode & rhs) {  // line 61
        return flEstimatedDistance < rhs.flEstimatedDistance;  // line 63
    }
    BOOL HeapValidate(NavCellPathHeapNode &);
    void NodeIndexChanged(int nNewIndex) {  // line 73
        ptPathNode->nOpenListHeapIndex = nNewIndex;  // line 76
    }
    BOOL ValidateNode(XBinaryHeap<NavCellPathHeapNode, 1> &);
};
static_assert(sizeof(NavCellPathHeapNode) == 0x10, "NavCellPathHeapNode");

class NavCellPathNodePQueue {  // size 0x10
public:
    void Initialize(int32 nMaxNodeCount) {  // line 93
        m_tNodeHeap.Initialize(nMaxNodeCount);
    }
    void Dispose() {  // line 98
        m_tNodeHeap.Dispose();  // line 100
    }
    BOOL IsEmpty() { return m_tNodeHeap.IsEmpty(); }  // line 103
    BOOL IsFull() { return m_tNodeHeap.IsFull(); }  // line 104
    NavCellPathNode * PopFront();
    void Push(NavCellPathNode * ptPathNode);
    NavCellPathHeapNode * FindNodeAtLocation(NavCellPathLocation & tLocation, NavCellPathNodeMap & tMasterNodeList);
    void UpdateCost(NavCellPathHeapNode * ptPathHeapNode);
    void RemoveAll();
    typedef XBinaryHeap<NavCellPathHeapNode, 1> XBinaryMinPathHeap;
protected:
    NavCellPathNodePQueue::XBinaryMinPathHeap m_tNodeHeap;  // 0x00
};
static_assert(sizeof(NavCellPathNodePQueue) == 0x10, "NavCellPathNodePQueue");

struct NavCellPathGlobals {  // size 0x20
    XChainedFixedBlockAllocator<NavCellPathNode> * pPathNodeAllocator;  // 0x00
    NavCellPathNodePQueue * ptPathOpenList;  // 0x08
    NavCellPathNodeMap * ptMasterNodeList;  // 0x10
    TPathPoint2DPool * ptPathPointPool;  // 0x18
};
static_assert(sizeof(NavCellPathGlobals) == 0x20, "NavCellPathGlobals");

enum NavCellEdgeType {
    NCET_X_AXIS = 0,
    NCET_Y_AXIS = 1,
    NCET_POINT = 2,
};

struct NavCellEdge {  // size 0x10
    uint16 eType;  // 0x00
    uint16 uBorder;  // 0x02
    float flAxis;  // 0x04
    float flMin;  // 0x08
    float flMax;  // 0x0c
};
static_assert(sizeof(NavCellEdge) == 0x10, "NavCellEdge");

class NavCellPathFinder {  // size 0x440
public:
    NavCellPathFinder(NavCellPathFindData &, XChainedFixedBlockAllocator<NavCellPathNode> &, NavCellPathNodePQueue &, NavCellPathNodeMap &);
    ~NavCellPathFinder();
    BOOL FindPath();
protected:
    NavCellPathNode * CreateNewPathNode(NavCellPathLocation & tPathLoc);
    NavCellPathNode * CreateNewPathNode(NavCellPathNode & tCopy);
    BOOL IsNodeAtDest(NavCellPathNode * ptNode);
    float GetHeuristicDistance(NavCellPathNode & tNodeCur);
    void ClosestPointToNavCell(const NavCellHandle & hNavCell, const Vector2D & wpGiven, Vector2D & wpClosest);
    void InitializeNode(NavCellPathNode & tNode, const NavCellPathNode * ptParentNode, const NavCellHandle & hNavCell);
    void ExploreNode(NavCellPathNode * ptParentNode, const NavCellHandle & hNavCell);
    void ExploreNeighbors(NavCellPathNode * ptNode);
    void MakeSolutionPath();
    const NavZoneInstance * GetNavZoneInst(const NavCellHandle & hNavCell);
    BOOL GetClosestPointOnEdge(const Vector2D & wpA, const Vector2D & wpB, const NavCellEdge & tEdge, Vector2D & wpClosest);
    void GetSharedEdge(const NavCellHandle & hNavCell1, const NavCellHandle & hNavCell2, NavCellEdge & tEdge, float flEdgeOffset);
    void ModifyStartLocationForRoundedCorners(NavCellPathLocation & tStartLoc);
    NavCellPathFindData & m_tPathFindData;  // 0x00
    XChainedFixedBlockAllocator<NavCellPathNode> & m_tPathNodeAllocator;  // 0x08
    NavCellPathNodePQueue & m_tOpenList;  // 0x10
    NavCellPathNodeMap & m_tMasterNodeList;  // 0x18
    NavCellPathNode * m_ptBestNode;  // 0x20
    float m_flMaxRangeFromDstSqr;  // 0x28
    NavCellPathLocation m_tEndLocation;  // 0x2c
    struct SceneNavZoneInstance {  // size 0x10
        SSceneID idSScene;  // 0x00
        const NavZoneInstance * ptNavZoneInstance;  // 0x08
    };
    int m_nNavZoneInstanceCount;  // 0x3c
    NavCellPathFinder::SceneNavZoneInstance m_arNavZoneInstances[64];  // 0x40
private:
    NavCellPathFinder & operator=(const NavCellPathFinder &);
};
static_assert(sizeof(NavCellPathFinder) == 0x440, "NavCellPathFinder");


// Functions defined in this file (bodies are not in the DWARF):
//   line    21  NavCellPathLocation & NavCellPathLocation::operator=(const NavCellPathLocation &)  [inlined into 3 functions]
//   line    38  NavCellPathNode & NavCellPathNode::operator=(const NavCellPathNode &)  [inlined into 1 functions]
//   line    61  BOOL NavCellPathHeapNode::HeapCompare(NavCellPathHeapNode & rhs)  [inlined into 2 functions]
//   line    73  void NavCellPathHeapNode::NodeIndexChanged(int nNewIndex)  [inlined into 4 functions]
//   line    93  void NavCellPathNodePQueue::Initialize(int32 nMaxNodeCount)  [inlined into 1 functions]
//   line    98  void NavCellPathNodePQueue::Dispose()  [inlined into 1 functions]
//   line   103  BOOL NavCellPathNodePQueue::IsEmpty()  [inlined into 1 functions]
//   line   104  BOOL NavCellPathNodePQueue::IsFull()  [inlined into 1 functions]
//   line   106  NavCellPathNode * NavCellPathNodePQueue::PopFront()  [inlined into 1 functions]
//   line   118  void NavCellPathNodePQueue::Push(NavCellPathNode * ptPathNode)  [inlined into 2 functions]
//   line   131  NavCellPathHeapNode * NavCellPathNodePQueue::FindNodeAtLocation(NavCellPathLocation & tLocation, NavCellPathNodeMap & tMasterNodeList)  [inlined into 1 functions]
//   line   148  void NavCellPathNodePQueue::UpdateCost(NavCellPathHeapNode * ptPathHeapNode)  [inlined into 1 functions]
//   line   167  void NavCellPathNodePQueue::RemoveAll()  [inlined into 1 functions]
