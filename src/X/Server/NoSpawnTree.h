// Adopted from the original DWARF; layouts and declarations retained.
// Original file: X/Server/NoSpawnTree.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/SigmaTypes.h"

struct Circle;
struct Rect2D;
struct SWorld;
struct SceneCommonData;

typedef XListMemoryPool<Circle> NoSpawnCirclePool;

typedef XPooledList<Circle, const Circle &> NoSpawnCircleList;

class NoSpawnNode {  // size 0x48
public:
    void Clear();
    void AddCircle(const Circle & tCircle);
    static const uint32 NUM_CHILDREN = 4;
    NoSpawnNode * m_arChildren[4];  // 0x00
    NoSpawnCircleList m_listCircles;  // 0x20
};
static_assert(sizeof(NoSpawnNode) == 0x48, "NoSpawnNode");

typedef XListMemoryPool<NoSpawnNode> NoSpawnNodePool;

typedef XPooledList<NoSpawnNode, const NoSpawnNode &> NoSpawnNodeList;

typedef void (*NoSpawnCallback)(SceneCommonData &, const Circle &);

class NoSpawnTree {  // size 0x48
public:
    void Initialize();
    void Populate(SWorld & tSWorld);
    void Dispose();
    void AddCircle(const Circle & tCircle);
    void GetCirclesInRect(const Rect2D & tRect, SceneCommonData & tSCD, NoSpawnCallback pfCallback);
protected:
    NoSpawnNode * NewNode();
    void AddCircle(NoSpawnNode ** pptNode, const Circle & tCircle, const Vector2D & vCenter, float flWidth);
    void GetChildCenter(const Vector2D & vParentCenter, Vector2D & vChildCenter, int32 nChildIdx, float flChildWidth);
    BOOL CircleIsContained(const Circle & tCircle, const Vector2D & vCenter, float flWidth);
    BOOL CircleIntersectsRect(const Circle & tCircle, const Rect2D & tRect);
    BOOL NodeIntersectsRect(const Vector2D & vCenter, float flWidth, const Rect2D & tRect);
    BOOL CenterHalfWidthIntersectsRect(const Vector2D & vCenter, float flHalfWidth, const Rect2D & tRect);
    void CheckNodeForCollisions(NoSpawnNode * ptNode, const Rect2D & tRect, SceneCommonData & tSCD, NoSpawnCallback pfCallback, const Vector2D & vCenter, float flWidth);
    void FreeNode(NoSpawnNode ** pptNode);
    Vector2D m_vCenter;  // 0x00
    float m_flWidth;  // 0x08
    NoSpawnNode * m_ptTop;  // 0x10
    uint32 m_uCircleCount;  // 0x18
    uint32 m_uNodeCount;  // 0x1c
    NoSpawnNodeList m_listNodes;  // 0x20
};
static_assert(sizeof(NoSpawnTree) == 0x48, "NoSpawnTree");


