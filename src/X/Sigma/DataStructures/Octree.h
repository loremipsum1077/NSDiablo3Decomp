#pragma once

#include "../Math/MathGeneral.h"
#include "../Math/Rect.h"
#include "../Math/Vector.h"
#include "../SigmaTypes.h"
#include "XPtr64.h"

#include "../Files/XSerializeData.h"









class Frustum;
struct LineSeg3D;
struct Sphere;

struct OctreeCube {
    Vector3D wpOrigin;
    float wdSize;

    void Init(const Vector3D& _wpOrigin, const float _wdSize);
};

struct OctreeNode {
    OctreeCube tCube;
    uint32 pdwChildren[8];
};

struct OctreeLeaf {
    OctreeCube tCube;
    int32 nPrimitiveIndex;
    int32 nPrimitiveCount;
};

struct OctreePrimitive {
    uint32 dwUserData;
    uint32 dwIndexListIndex;
};

typedef BOOL (*OctreeNodeCallback)(const OctreeNode* const, void*, BOOL&);

typedef BOOL (*OctreeLeafCallback)(const OctreeLeaf* const, void*, BOOL&);

class Octree {
public:
    int32 nMaxNodes;
    int32 nMaxLeaves;
    int32 nMaxPrimitives;
    int32 nNodeCount;
    int32 nLeafCount;
    int32 nPrimitiveCount;
    SerializeData serNodes;
    XPtr64<OctreeNode> ptNodes;
    SerializeData serLeaves;
    XPtr64<OctreeLeaf> ptLeaves;
    SerializeData serPrimitives;
    XPtr64<OctreePrimitive> ptPrimitives;

    void WalkLinear(const LineSeg3D& wline, void* lpData, OctreeLeafCallback pfnLeafCallback) const;

private:
    OctreeNode& sGetNode(const int32 nNodeIndex) const;
    OctreeLeaf& sGetLeaf(const int32 nLeafIndex) const;
    void sWalkLinearRecursive(const LineSeg3D& wline, const OctreeNode* const ptNode, void* lpData, BOOL& fEndWalk,
                              OctreeLeafCallback pfnLeafCallback) const;
};









































































































inline OctreeNode& Octree::sGetNode(const int32 nNodeIndex) const {




    return ptNodes[nNodeIndex];
}

inline OctreeLeaf& Octree::sGetLeaf(const int32 nLeafIndex) const {
    int32 nIndex = nLeafIndex ^ 0x80000000;





    return ptLeaves[nIndex];
}

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(OctreeCube) == 0x10, "OctreeCube");
static_assert(offsetof(OctreeCube, wpOrigin) == 0x0, "OctreeCube");
static_assert(offsetof(OctreeCube, wdSize) == 0xc, "OctreeCube");
static_assert(sizeof(OctreeNode) == 0x30, "OctreeNode");
static_assert(offsetof(OctreeNode, tCube) == 0x0, "OctreeNode");
static_assert(offsetof(OctreeNode, pdwChildren) == 0x10, "OctreeNode");
static_assert(sizeof(OctreeLeaf) == 0x18, "OctreeLeaf");
static_assert(offsetof(OctreeLeaf, tCube) == 0x0, "OctreeLeaf");
static_assert(offsetof(OctreeLeaf, nPrimitiveIndex) == 0x10, "OctreeLeaf");
static_assert(offsetof(OctreeLeaf, nPrimitiveCount) == 0x14, "OctreeLeaf");
static_assert(sizeof(OctreePrimitive) == 0x8, "OctreePrimitive");
static_assert(offsetof(OctreePrimitive, dwUserData) == 0x0, "OctreePrimitive");
static_assert(offsetof(OctreePrimitive, dwIndexListIndex) == 0x4, "OctreePrimitive");
static_assert(sizeof(Octree) == 0x48, "Octree");
static_assert(offsetof(Octree, nMaxNodes) == 0x0, "Octree");
static_assert(offsetof(Octree, nMaxLeaves) == 0x4, "Octree");
static_assert(offsetof(Octree, nMaxPrimitives) == 0x8, "Octree");
static_assert(offsetof(Octree, nNodeCount) == 0xc, "Octree");
static_assert(offsetof(Octree, nLeafCount) == 0x10, "Octree");
static_assert(offsetof(Octree, nPrimitiveCount) == 0x14, "Octree");
static_assert(offsetof(Octree, serNodes) == 0x18, "Octree");
static_assert(offsetof(Octree, ptNodes) == 0x20, "Octree");
static_assert(offsetof(Octree, serLeaves) == 0x28, "Octree");
static_assert(offsetof(Octree, ptLeaves) == 0x30, "Octree");
static_assert(offsetof(Octree, serPrimitives) == 0x38, "Octree");
static_assert(offsetof(Octree, ptPrimitives) == 0x40, "Octree");
