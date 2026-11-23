// Original file: Contrib/Contrib/Domino/Collision/BroadPhase/dmDynamicTree.cpp
#include "dmDynamicTree.h"
#include "../../Include/dmSettings.h"
#include <string.h>

dmAABB dm_defaultAABB;  // line 17

void dmDynamicTree::Create() {
    m_root = -1; m_nodeCapacity = 32; m_nodeCount = 0;
    m_aNodes = static_cast<Node *>(dmAllocFunction(sizeof(Node) * m_nodeCapacity, 16,
        "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/BroadPhase/dmDynamicTree.cpp", 38, "Create"));
    memset(m_aNodes, 0, sizeof(Node) * m_nodeCapacity);
    for (dm_int32 i = 0; i < m_nodeCapacity; ++i) {
        m_aNodes[i].next = i + 1;
        m_aNodes[i].child1 = 0x69f6bc7;
        m_aNodes[i].child2 = 0x69f6bc7;
        m_aNodes[i].height = -1;
        m_aNodes[i].userData = 0;
    }
    m_aNodes[m_nodeCapacity - 1].next = -1;
    m_freeList = 0; m_proxyCount = 0; m_path = 0; m_threadCheck = false;
}
void dmDynamicTree::Destroy() { dmFree(m_aNodes); m_aNodes = 0; }
dm_int32 dmDynamicTree::AllocateNode() {
    if (m_freeList == -1) {
        Node * pOldNodes = m_aNodes;
        m_nodeCapacity *= 2;
        m_aNodes = static_cast<Node *>(dmAllocFunction(sizeof(Node) * m_nodeCapacity, 16,
            "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/BroadPhase/dmDynamicTree.cpp", 78, "AllocateNode"));
        memcpy(m_aNodes, pOldNodes, sizeof(Node) * m_nodeCount);
        dmFree(pOldNodes);
        for (dm_int32 i = m_nodeCount; i < m_nodeCapacity; ++i) {
            m_aNodes[i].next = i + 1;
            // The original initializes child1 twice in the growth path.
            m_aNodes[i].child1 = 0x69f6bc7;
            m_aNodes[i].child1 = 0x69f6bc7;
            m_aNodes[i].height = -1;
            m_aNodes[i].userData = 0;
        }
        m_aNodes[m_nodeCapacity - 1].next = -1;
        m_freeList = m_nodeCount;
    }
    dm_int32 nodeId = m_freeList;
    m_freeList = m_aNodes[nodeId].next;
    m_aNodes[nodeId].parent = -1;
    m_aNodes[nodeId].child1 = -1;
    m_aNodes[nodeId].child2 = -1;
    m_aNodes[nodeId].height = 0;
    m_aNodes[nodeId].userData = 0;
    ++m_nodeCount;
    return nodeId;
}
inline void dmDynamicTree::FreeNode(dm_int32 nodeId) {
    if (nodeId < 0 || nodeId >= m_nodeCapacity || m_nodeCount == 0) return;
    m_aNodes[nodeId].next = m_freeList;
    m_aNodes[nodeId].child1 = 0x69f6bc7;
    m_aNodes[nodeId].child2 = 0x69f6bc7;
    m_aNodes[nodeId].height = -1;
    --m_nodeCount;
    m_freeList = nodeId;
}
dm_int32 dmDynamicTree::CreateProxy(const dmAABB & aabb, dm_uint64 userData) {
    if (!aabb.IsValid()) return -1;
    m_threadCheck = true;
    dm_int32 proxyId = AllocateNode();
    dm_vec4 r = dmVecSet(0.328084f);
    m_aNodes[proxyId].aabb.m_lowerBound = dmSub(aabb.m_lowerBound, r);
    m_aNodes[proxyId].aabb.m_upperBound = dmAdd(aabb.m_upperBound, r);
    m_aNodes[proxyId].userData = userData;
    m_aNodes[proxyId].height = 0;
    InsertLeaf(proxyId);
    ++m_proxyCount;
    m_threadCheck = false;
    return proxyId;
}
void dmDynamicTree::DestroyProxy(dm_int32 proxyId) {
    if (proxyId < 0 || proxyId >= m_nodeCapacity || !m_aNodes[proxyId].IsLeaf()) return;
    m_threadCheck = true;
    RemoveLeaf(proxyId);
    FreeNode(proxyId);
    --m_proxyCount;
    m_threadCheck = false;
}
bool dmDynamicTree::MoveProxy(dm_int32 proxyId, const dmAABB & aabb, dm_vec4Arg displacement) {
    if (!dmIsFinite3(displacement)) return false;
    if (!aabb.IsValid()) return false;
    if (proxyId < 0 || proxyId >= m_nodeCapacity || !m_aNodes[proxyId].IsLeaf()) return false;
    m_threadCheck = true;
    if (m_aNodes[proxyId].aabb.Contains(aabb)) { m_threadCheck = false; return false; }
    RemoveLeaf(proxyId);
    dm_vec4 r = dmVecSet(0.328084f);
    dmAABB b;
    b.m_lowerBound = dmSub(aabb.m_lowerBound, r);
    b.m_upperBound = dmAdd(aabb.m_upperBound, r);
    b.m_lowerBound = dmMin(b.m_lowerBound, dmAdd(b.m_lowerBound, displacement));
    b.m_upperBound = dmMax(b.m_upperBound, dmAdd(b.m_upperBound, displacement));
    m_aNodes[proxyId].aabb = b;
    InsertLeaf(proxyId);
    m_threadCheck = false;
    return true;
}
void dmDynamicTree::InsertLeaf(dm_int32 leaf) {
    if (leaf < 0 || leaf >= m_nodeCapacity || !m_aNodes[leaf].IsLeaf()) return;
    if (m_root == -1) { m_root = leaf; m_aNodes[m_root].parent = -1; return; }
    Node * pLeaf = m_aNodes + leaf;
    dmAABB leafAABB = pLeaf->aabb;
    dm_int32 index = m_root;
    while (!m_aNodes[index].IsLeaf()) {
        Node * pNode = m_aNodes + index;
        dm_int32 child1 = pNode->child1, child2 = pNode->child2;
        dm_vec4 area = pNode->aabb.GetSurfaceArea();
        dmAABB combinedAABB;
        combinedAABB.Merge(pNode->aabb, leafAABB);
        dm_vec4 combinedArea = combinedAABB.GetSurfaceArea();
        dm_vec4 cost = dmMul(dmVecSet(2.0f), combinedArea);
        dm_vec4 inheritanceCost = dmMul(dmVecSet(2.0f), dmSub(combinedArea, area));
        dm_vec4 cost1;
        if (m_aNodes[child1].IsLeaf()) {
            dmAABB aabb; aabb.Merge(leafAABB, m_aNodes[child1].aabb);
            cost1 = dmAdd(aabb.GetSurfaceArea(), inheritanceCost);
        } else {
            dmAABB aabb; aabb.Merge(leafAABB, m_aNodes[child1].aabb);
            dm_vec4 oldArea = m_aNodes[child1].aabb.GetSurfaceArea();
            dm_vec4 newArea = aabb.GetSurfaceArea();
            cost1 = dmAdd(dmSub(newArea, oldArea), inheritanceCost);
        }
        dm_vec4 cost2;
        if (m_aNodes[child2].IsLeaf()) {
            dmAABB aabb; aabb.Merge(leafAABB, m_aNodes[child2].aabb);
            cost2 = dmAdd(aabb.GetSurfaceArea(), inheritanceCost);
        } else {
            dmAABB aabb; aabb.Merge(leafAABB, m_aNodes[child2].aabb);
            dm_vec4 oldArea = m_aNodes[child2].aabb.GetSurfaceArea();
            dm_vec4 newArea = aabb.GetSurfaceArea();
            cost2 = dmAdd(dmSub(newArea, oldArea), inheritanceCost);
        }
        if (dmLT(cost, dmMin(cost1, cost2))) break;
        index = dmLT(cost1, cost2) ? child1 : child2;
    }
    dm_int32 sibling = index;
    dm_int32 oldParent = m_aNodes[sibling].parent;
    dm_int32 newParent = AllocateNode();
    pLeaf = m_aNodes + leaf;
    Node * pSibling = m_aNodes + sibling;
    Node * pNewParent = m_aNodes + newParent;
    pNewParent->parent = oldParent;
    pNewParent->userData = 0;
    pNewParent->aabb.Merge(leafAABB, pSibling->aabb);
    pNewParent->height = pSibling->height + 1;
    if (oldParent != -1) {
        if (m_aNodes[oldParent].child1 == sibling) m_aNodes[oldParent].child1 = newParent;
        else {
            if (m_aNodes[oldParent].child2 != sibling && !dm_assertOnce) {
                dmAssertFunction("m_aNodes[oldParent].child2 == sibling",
                    "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/BroadPhase/dmDynamicTree.cpp", 383, 0, 0);
                __builtin_trap();
            }
            m_aNodes[oldParent].child2 = newParent;
        }
        pNewParent->child1 = sibling;
        pNewParent->child2 = leaf;
        pSibling->parent = newParent;
        pLeaf->parent = newParent;
    } else {
        pNewParent->child1 = sibling;
        pNewParent->child2 = leaf;
        pSibling->parent = newParent;
        pLeaf->parent = newParent;
        m_root = newParent;
    }
    index = newParent;
    while (index != -1) {
        index = Balance(index);
        dm_int32 child1 = m_aNodes[index].child1, child2 = m_aNodes[index].child2;
        m_aNodes[index].height = 1 + dmMax(m_aNodes[child1].height, m_aNodes[child2].height);
        m_aNodes[index].aabb.Merge(m_aNodes[child1].aabb, m_aNodes[child2].aabb);
        index = m_aNodes[index].parent;
    }
}
void dmDynamicTree::RemoveLeaf(dm_int32 leaf) {
    if (leaf < 0 || leaf >= m_nodeCapacity || !m_aNodes[leaf].IsLeaf()) return;
    if (leaf == m_root) { m_root = -1; return; }
    dm_int32 parent = m_aNodes[leaf].parent;
    dm_int32 grandParent = m_aNodes[parent].parent;
    dm_int32 sibling = m_aNodes[parent].child1 == leaf ? m_aNodes[parent].child2 : m_aNodes[parent].child1;
    if (grandParent != -1) {
        if (m_aNodes[grandParent].child1 == parent) m_aNodes[grandParent].child1 = sibling;
        else m_aNodes[grandParent].child2 = sibling;
        m_aNodes[sibling].parent = grandParent;
        FreeNode(parent);
        dm_int32 index = grandParent;
        while (index != -1) {
            index = Balance(index);
            dm_int32 child1 = m_aNodes[index].child1, child2 = m_aNodes[index].child2;
            m_aNodes[index].aabb.Merge(m_aNodes[child1].aabb, m_aNodes[child2].aabb);
            m_aNodes[index].height = 1 + dmMax(m_aNodes[child1].height, m_aNodes[child2].height);
            index = m_aNodes[index].parent;
        }
    } else {
        m_root = sibling;
        m_aNodes[sibling].parent = -1;
        FreeNode(parent);
    }
}
dm_int32 dmDynamicTree::Balance(dm_int32 iA) {
    Node * A = m_aNodes + iA;
    if (A->IsLeaf() || A->height < 2) return iA;
    dm_int32 iB = A->child1, iC = A->child2;
    Node * B = m_aNodes + iB, * C = m_aNodes + iC;
    dm_int32 balance = C->height - B->height;
    if (balance > 1) {
        dm_int32 iF = C->child1, iG = C->child2;
        Node * F = m_aNodes + iF, * G = m_aNodes + iG;
        C->child1 = iA; C->parent = A->parent; A->parent = iC;
        if (C->parent != -1) {
            if (m_aNodes[C->parent].child1 == iA) m_aNodes[C->parent].child1 = iC;
            else m_aNodes[C->parent].child2 = iC;
        } else m_root = iC;
        if (F->height > G->height) {
            C->child2 = iF; A->child2 = iG; G->parent = iA;
            A->aabb.Merge(B->aabb, G->aabb); C->aabb.Merge(A->aabb, F->aabb);
            A->height = 1 + dmMax(B->height, G->height);
            C->height = 1 + dmMax(A->height, F->height);
        } else {
            C->child2 = iG; A->child2 = iF; F->parent = iA;
            A->aabb.Merge(B->aabb, F->aabb); C->aabb.Merge(A->aabb, G->aabb);
            A->height = 1 + dmMax(B->height, F->height);
            C->height = 1 + dmMax(A->height, G->height);
        }
        return iC;
    }
    if (balance < -1) {
        dm_int32 iD = B->child1, iE = B->child2;
        Node * D = m_aNodes + iD, * E = m_aNodes + iE;
        B->child1 = iA; B->parent = A->parent; A->parent = iB;
        if (B->parent != -1) {
            if (m_aNodes[B->parent].child1 == iA) m_aNodes[B->parent].child1 = iB;
            else m_aNodes[B->parent].child2 = iB;
        } else m_root = iB;
        if (D->height > E->height) {
            B->child2 = iD; A->child1 = iE; E->parent = iA;
            A->aabb.Merge(C->aabb, E->aabb); B->aabb.Merge(A->aabb, D->aabb);
            A->height = 1 + dmMax(C->height, E->height);
            B->height = 1 + dmMax(A->height, D->height);
        } else {
            B->child2 = iE; A->child1 = iD; D->parent = iA;
            A->aabb.Merge(C->aabb, D->aabb); B->aabb.Merge(A->aabb, E->aabb);
            A->height = 1 + dmMax(C->height, D->height);
            B->height = 1 + dmMax(A->height, E->height);
        }
        return iB;
    }
    return iA;
}
void dmDynamicTree::Optimize(dm_int32 iterations) {
    if (m_root == -1) return;
    for (dm_int32 i = 0; i < iterations; ++i) {
        while (m_aNodes[m_path].height == -1) {
            if (++m_path == m_nodeCapacity) m_path = 0;
        }
        Shuffle(m_path);
        if (++m_path == m_nodeCapacity) m_path = 0;
    }
}
void dmDynamicTree::Shuffle(dm_int32 index) {
    if (index == -1)
        return;
    Node * node = m_aNodes + index;
    if (node->height < 2)
        return;
    dm_int32 i1 = node->child1;
    dm_int32 i2 = node->child2;
    Node * node1 = m_aNodes + i1;
    Node * node2 = m_aNodes + i2;
    if (node1->height < 1 || node2->height < 1)
        return;
    dm_int32 i11 = node1->child1;
    dm_int32 i12 = node1->child2;
    dm_int32 i21 = node2->child1;
    dm_int32 i22 = node2->child2;
    Node * node11 = m_aNodes + i11;
    Node * node12 = m_aNodes + i12;
    Node * node21 = m_aNodes + i21;
    Node * node22 = m_aNodes + i22;
    dmAABB b11 = node11->aabb;
    dmAABB b12 = node12->aabb;
    dmAABB b21 = node21->aabb;
    dmAABB b22 = node22->aabb;
    dm_vec4 m1, m2, m3;
    {
        dmAABB b1, b2;
        b1.Merge(b11, b12);
        b2.Merge(b21, b22);
        m1 = dmAdd(b1.GetSurfaceArea(), b2.GetSurfaceArea());
    }
    {
        dmAABB b1, b2;
        b1.Merge(b11, b22);
        b2.Merge(b12, b21);
        m2 = dmAdd(b1.GetSurfaceArea(), b2.GetSurfaceArea());
    }
    {
        dmAABB b1, b2;
        b1.Merge(b11, b21);
        b2.Merge(b12, b22);
        m3 = dmAdd(b1.GetSurfaceArea(), b2.GetSurfaceArea());
    }
    if (dmLE(m1, dmMin(m2, m3)))
        return;
    // Each branch updates node2 itself: the original sinks the common node2 Merge/height into the
    // join (only the else-side Merge DIE at 777 survives, its code at line 0 after both branches).
    if (dmLE(m2, m3)) {
        node1->child2 = i22;
        node22->parent = i1;
        node1->aabb.Merge(node11->aabb, node22->aabb);
        node1->height = 1 + dmMax(node11->height, node22->height);
        node2->child2 = i12;
        node12->parent = i2;
        node2->aabb.Merge(node21->aabb, node12->aabb);
        node2->height = 1 + dmMax(node21->height, node12->height);
    } else {
        node1->child2 = i21;
        node21->parent = i1;
        node1->aabb.Merge(node11->aabb, node21->aabb);
        node1->height = 1 + dmMax(node11->height, node21->height);
        node2->child1 = i12;
        node12->parent = i2;
        node2->aabb.Merge(node12->aabb, node22->aabb);
        node2->height = 1 + dmMax(node12->height, node22->height);
    }
    node->aabb.Merge(node1->aabb, node2->aabb);
    node->height = 1 + dmMax(node1->height, node2->height);
    dm_int32 i = node->parent;
    while (i != -1) {
        Node * n = m_aNodes + i;
        dm_int32 cI1 = n->child1;
        dm_int32 cI2 = n->child2;
        Node * n1 = m_aNodes + cI1;
        Node * n2 = m_aNodes + cI2;
        n->aabb.Merge(n1->aabb, n2->aabb);
        n->height = 1 + dmMax(n1->height, n2->height);
        i = n->parent;
    }
}
