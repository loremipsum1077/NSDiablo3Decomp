// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/Contacts/dmMeshContact.h

#pragma once

#include "../../Collision/Functions/dmDistance.h"
#include "../../Collision/Functions/dmManifold.h"
#include "../../Include/dmCollision.h"
#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"

class dmCapsule;
struct dmContact;
struct dmManifold;
class dmMesh;
struct dmMutex;
class dmPolytope;
class dmPoolAllocator;
class dmSphere;
struct dmSweep;
struct dmTOIOutput;
struct dmWorld;

struct dmTriangleCache {  // size 0x20
    dm_int32 triangleIndex;  // 0x00
    dmGJKCache gjkCache;  // 0x04
    dmSATCache satCache;  // 0x10
    dm_int16 ax;  // 0x18
    dm_int16 ay;  // 0x1a
    dm_int16 az;  // 0x1c
};
static_assert(sizeof(dmTriangleCache) == 0x20, "dmTriangleCache");

struct dmMeshContact {  // size 0x40
    void Create();
    void Destroy(dmWorld * pWorld);
    bool UpdateManifolds(dmWorld * pWorld, dmContact * pBase, dmMutex * pMutex, bool isTOIEvent);
    static void ReduceManifolds(dmManifold *& aBaseManifolds, dm_int32 & baseManifoldCount, const dmManifold * manifolds, dm_int32 manifoldCount, dm_vec4 quatRel, dmPoolAllocator * pPoolAllocator, dmMutex * pMutex);
    static bool ComputeManifoldsTrianglesSphere(const dmMesh * pMeshInput, dmTriangleCache * aCache, dm_int32 triangleCount, dmManifold *& aBaseManifolds, dm_int32 & baseManifoldCount, const dmSphere & sphereB, const dmTransform & xfA, dmPoolAllocator * pPoolAllocator, dmMutex * pMutex);
    static bool ComputeManifoldsTrianglesCapsule(const dmMesh * pMeshInput, dmTriangleCache * aCache, dm_int32 triangleCount, dmManifold *& aBaseManifolds, dm_int32 & baseManifoldCount, const dmCapsule & capsuleB, const dmTransform & xfA, dmPoolAllocator * pPoolAllocator, dmMutex * pMutex);
    static bool ComputeManifoldsTrianglesPolytope(const dmMesh * pMeshInput, dmTriangleCache * aCache, dm_int32 triangleCount, dmManifold *& aBaseManifolds, dm_int32 & baseManifoldCount, const dmPolytope * pPolytopeB, const dmTransform & transform, const dmTransform & xfA, dmPoolAllocator * pPoolAllocator, dmMutex * pMutex, bool isTOIEvent);
    dm_vec4 ShapeCast(dm_int32 *, dmWorld *, const dmContact *, const dmTransform &, const dmTransform &, dm_vec4Arg);
    void TimeOfImpact(dmTOIOutput * pOutput, dmWorld * pWorld, const dmContact * pBase, const dmSweep & sweepA, const dmSweep & sweepB, dmMutex * pMutex);
    void Distance(dmDistanceOutput *, dmWorld *, const dmContact *, dm_int32);
    void UpdateTriangleBuffer(dmWorld * pWorld, const dmContact * pBase, dmMutex * pMutex);
    void ClearBaseManifolds(dmWorld * pWorld, dmContact * pBase, dmMutex * pMutex);
    union {  // size 0x4
        dm_int32 m_index;  // 0x00
        dm_int32 m_next;  // 0x00
    };  // 0x00
    dmAABB m_fatAABB;  // 0x10
    dm_int32 m_triangleCount;  // 0x30
    dm_int32 m_triangleCapacity;  // 0x34
    dmTriangleCache * m_aCache;  // 0x38
};
static_assert(sizeof(dmMeshContact) == 0x40, "dmMeshContact");

