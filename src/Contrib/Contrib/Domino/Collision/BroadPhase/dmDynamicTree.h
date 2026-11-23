// Adopted from DWARF; supported original interfaces.
// Original file: Contrib/Contrib/Domino/Collision/BroadPhase/dmDynamicTree.h

#pragma once


#include "../../Include/dmCollision.h"
#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"

class dmDynamicTree {  // size 0x28
public:
    dmDynamicTree();
    ~dmDynamicTree();
    void Create();
    void Destroy();
    dm_int32 CreateProxy(const dmAABB & aabb, dm_uint64 userData);
    void DestroyProxy(dm_int32 proxyId);
    dm_uint64 GetUserData(dm_int32 proxyId) const;
    const dmAABB & GetFatAABB(dm_int32 proxyId) const;
    bool CheckMove(dm_int32, const dmAABB &) const;
    bool MoveProxy(dm_int32 proxyId, const dmAABB & aabb, dm_vec4Arg displacement);
    void Optimize(dm_int32 iterations);
    dm_int32 GetProxyCount() const;
    dm_int32 GetHeight() const;
    dm_int32 GetMaxBalance() const;
    dm_float32 GetAreaRatio() const;
    void Validate() const;
    void RebuildBottomUp();
    void ShiftOrigin(dm_vec4Arg);
    dm_int32 GetBytes() const;
    struct Node {  // size 0x40
        bool IsLeaf() const { return child1 == -1; }
        dmAABB aabb;  // 0x00
        dm_uint64 userData;  // 0x20
        union {  // size 0x4
            dm_int32 parent;  // 0x00
            dm_int32 next;  // 0x00
        };  // 0x28
        dm_int32 child1;  // 0x2c
        dm_int32 child2;  // 0x30
        dm_int32 height;  // 0x34
    };
private:
    dm_int32 AllocateNode();
    void FreeNode(dm_int32 nodeId);
    void InsertLeaf(dm_int32 leaf);
    void RemoveLeaf(dm_int32 leaf);
    dm_int32 Balance(dm_int32 iA);
    void Shuffle(dm_int32 index);
    dm_int32 ComputeHeight() const;
    dm_int32 ComputeHeight(dm_int32) const;
    void ValidateStructure(dm_int32) const;
    void ValidateMetrics(dm_int32) const;
    dm_float32 ComputeArea(dm_int32) const;
    dmDynamicTree::Node * m_aNodes;  // 0x00
    dm_int32 m_nodeCapacity;  // 0x08
    dm_int32 m_nodeCount;  // 0x0c
    dm_int32 m_freeList;  // 0x10
    dm_int32 m_root;  // 0x14
    dm_int32 m_proxyCount;  // 0x18
    dm_int32 m_path;  // 0x1c
    volatile bool m_threadCheck;  // 0x20
public:
    template <typename T> void QueryAABB(T * pCallback, const dmAABB & aabb) const; // DWARF dmDynamicTree.h:184
    template <typename T> void RayCast(T * pCallback, const dmRayCastInput * pInput) const; // DWARF dmDynamicTree.h:228
    // template instantiation: void QueryAABB<dmBroadPhaseQueryWrapper<dmBroadPhaseCallback> >(dmBroadPhaseQueryWrapper<dmBroadPhaseCallback> *, const dmAABB &) const;
    // template instantiation: void RayCast<dmBroadPhaseRayCastWrapper<dmRayCastClosestCallback> >(dmBroadPhaseRayCastWrapper<dmRayCastClosestCallback> * pCallback, const dmRayCastInput * pInput) const;
    // template instantiation: void RayCast<dmBroadPhaseRayCastWrapper<dmRayCastAnyCallback> >(dmBroadPhaseRayCastWrapper<dmRayCastAnyCallback> *, const dmRayCastInput *) const;
    // template instantiation: void RayCast<dmBroadPhaseRayCastWrapper<dmBroadPhaseCallback> >(dmBroadPhaseRayCastWrapper<dmBroadPhaseCallback> *, const dmRayCastInput *) const;
};
static_assert(sizeof(dmDynamicTree) == 0x28, "dmDynamicTree");


// Functions defined in this file (bodies are not in the DWARF):
//   line   104  bool dmDynamicTree::Node::IsLeaf() const  [inlined into 8 functions]
//   line   158  dm_uint64 dmDynamicTree::GetUserData(dm_int32 proxyId) const  [inlined into 3 functions]
//   line   170  const dmAABB & dmDynamicTree::GetFatAABB(dm_int32 proxyId) const  [inlined into 2 functions]
//   line   184  void dmDynamicTree::QueryAABB<dmBroadPhase>(dmBroadPhase * pCallback, const dmAABB & aabb) const  [0x7100bc67a0, inlined into 1 functions]
//   line   228  void dmDynamicTree::RayCast<dmBroadPhaseRayCastWrapper<dmRayCastClosestCallback> >(dmBroadPhaseRayCastWrapper<dmRayCastClosestCallback> * pCallback, const dmRayCastInput * pInput) const  [0x7100bccef0]

// Bodies. The #line directives put each one on its original line; the assert
// in QueryAABB records the original header path and line 220.
#include "../../Include/dmSettings.h"
#include "../../Common/dmGrowableStack.h"
#include <string.h>

// Defined at dmDynamicTree.cpp:17; the DWARF does not say which header declares it.
extern dmAABB dm_defaultAABB;

#line 158 "../../..\\Domino/Collision/BroadPhase/dmDynamicTree.h"
inline dm_uint64 dmDynamicTree::GetUserData(dm_int32 proxyId) const
{
    if (proxyId < 0 || proxyId >= m_nodeCapacity)
    {
        return 0;
    }

    return m_aNodes[proxyId].userData;
}



inline const dmAABB & dmDynamicTree::GetFatAABB(dm_int32 proxyId) const
{
    if (proxyId < 0 || proxyId >= m_nodeCapacity)
    {


        memset(&dm_defaultAABB, 0, sizeof(dm_defaultAABB)); return dm_defaultAABB;
    }

    return m_aNodes[proxyId].aabb;
}



template <typename T> void dmDynamicTree::QueryAABB(T * pCallback, const dmAABB & aabb) const
{
    if (m_root == -1)
    {
        return;
    }



    dm_int32 aStack[128];
    aStack[0] = m_root;
    dm_int32 count = 1;

    const dmAABB box = aabb;
    const Node * aNodes = m_aNodes;

    while (count > 0)
    {
        --count;
        dm_int32 nodeId = aStack[count];
        const Node * pNode = aNodes + nodeId;

        if (dmTestOverlap(pNode->aabb, box))
        {
            if (pNode->IsLeaf())
            {
                pCallback->QueryCallback(nodeId);
            }
            else if (count + 2 <= 128)
            {
                aStack[count++] = pNode->child1;
                aStack[count++] = pNode->child2;
            }
            else
            {

                dmAssert(false);
            }
        }
    }
}

// Does not match yet (see dmWorld.cpp). The two rejection tests are written as nested ifs because that is the only form tried that gives the
// original single loop; the original closing braces (lines 319-321) leave room for only three, so its spelling differs. Lines 293-298 are unknown.
template <typename T> void dmDynamicTree::RayCast(T * pCallback, const dmRayCastInput * pInput) const
{
    if (m_root == -1)
    {
        return;
    }

    if (dmIsFinite3(pInput->p1) == false || dmIsFinite3(pInput->p2) == false)
    {

        dmLog("Bad input to dmDynamicTree::RayCast\n");
        return;
    }

    const dm_vec4 point1 = pInput->p1;
    const dm_vec4 point2 = pInput->p2;
    dm_vec4 maxLambda = pInput->maxLambda;


    dm_vec4 d = dmSub(point2, point1);
    dm_vec4 m = dmMul(dmAdd(point1, point2), dm_vec4_oneHalf);


    dm_vec4 absD = dmAdd(dmAbs(d), dm_vec4_epsilon);

    dmAABB rayAABB;
    rayAABB.m_lowerBound = dmMin(point1, point2);
    rayAABB.m_upperBound = dmMax(point1, point2);

    dmGrowableStack<dm_int32, 128> stack;
    stack.Push(m_root);

    while (stack.GetCount() > 0)
    {
        dm_int32 nodeId = stack.Pop();
        const Node * pNode = m_aNodes + nodeId;


        if (dmTestOverlap(pNode->aabb, rayAABB))
        {




        dm_vec4 c = pNode->aabb.GetCenter();
        dm_vec4 h = pNode->aabb.GetExtents();
        dm_vec4 s = dmSub(m, c);
        dm_vec4 separation = dmSub(dmAbs(dmCross(s, d)), dmModifiedCross(h, absD));
        if (dmAll_GT3(separation, vdupq_n_f32(0.0f)) == false)
        {



        if (pNode->IsLeaf())
        {
            dmRayCastInput input;

            input.p1 = point1;
            input.p2 = point2;
            input.maxLambda = maxLambda;
            input.triangleMask = pInput->triangleMask;
            input.slop = pInput->slop;

            dm_vec4 lambda;
            dmCollisionCode code = pCallback->RayCastCallback(&lambda, input, nodeId);






            if (code == dm_collideClip && dmLT(lambda, maxLambda))
            {

                maxLambda = lambda;
                d = dmMul(dmSub(point2, point1), lambda);

                dm_vec4 p = dmAdd(d, point1);
                absD = dmAdd(dm_vec4_epsilon, dmAbs(d));
                m = dmMul(dm_vec4_oneHalf, dmAdd(p, point1));
                rayAABB.m_lowerBound = dmMin(point1, p);
                rayAABB.m_upperBound = dmMax(point1, p);
            }
        }
        else
        {



            stack.Push(pNode->child1);
            stack.Push(pNode->child2);
        } } }
    }
}
