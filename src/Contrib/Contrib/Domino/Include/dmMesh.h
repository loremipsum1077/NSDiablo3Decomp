// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Include/dmMesh.h

#pragma once

#include "dmBuffer.h"
#include "dmSerialize.h"
#include "dmShapes.h"

struct dmAABB;
struct dmTransform;

class dmMesh : public dmShape {  // size 0x18
public:
    enum MeshType {
        e_treeMesh = 0,
        e_heightField = 1,
        e_unknownMeshType = 2,
    };
public:
    dmMesh();
    // Both are defined in the class body (dmMesh.h:37 for IsSolid), which
    // leaves TestOverlap as the key function: the vtable belongs to dmMesh.cpp.
    virtual ~dmMesh() {}  // vtable slot 0
    virtual bool IsSolid(dm_int32 index) const { return true; }  // vtable slot 13
    virtual void GetTriangle(dmTriangle *, dm_int32) const = 0;  // vtable slot 14
    virtual void GetTriangleTransformed(dmTriangle *, dm_int32, const dmTransform &) const = 0;  // vtable slot 15
    virtual void GetTriangleFull(dmTriangle *, dm_int32) const = 0;  // vtable slot 16
    virtual void Query(dmBuffer<int, 128> *, const dmAABB &) const = 0;  // vtable slot 17
    virtual bool TestOverlap(const dmSphere & sphere) const;  // vtable slot 11
    dmMesh::MeshType m_meshType;  // 0x10
    dm_int32 m_triangleCount;  // 0x14
};
static_assert(sizeof(dmMesh) == 0x18, "dmMesh");

inline dmMesh::dmMesh()
{
    m_type = e_mesh;
    m_meshType = e_unknownMeshType;
    m_triangleCount = 0;
}
