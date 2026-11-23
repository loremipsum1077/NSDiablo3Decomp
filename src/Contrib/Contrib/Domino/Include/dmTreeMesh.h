// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/Domino/Include/dmTreeMesh.h

#pragma once

#include "dmBuffer.h"
#include "dmMath.h"
#include "dmMesh.h"
#include "dmSerialize.h"

struct dmAABB;
struct dmMeshData;
struct dmMeshNode;
struct dmMeshTriangle;
struct dmRayCastInput;
struct dmRayCastOutput;
class dmTriangle;

class dmTreeMesh : public dmMesh {  // size 0xa0
public:
    dmTreeMesh();
    virtual ~dmTreeMesh() {}  // defined in the class body (DWARF dmTreeMesh.h:23); D0 is emitted with the vtable in dmTreeMesh.cpp
    void Create(const dmMeshData * pData, dm_vec4Arg scale);
    virtual void GetTriangle(dmTriangle * pTriangle, dm_int32 index) const;  // vtable slot 14
    virtual void GetTriangleTransformed(dmTriangle * pTriangle, dm_int32 index, const dmTransform & xf) const;  // vtable slot 15
    virtual void GetTriangleFull(dmTriangle * pTriangle, dm_int32 index) const;  // vtable slot 16
    const dmMeshTriangle * GetMeshTriangles() const;
    const dm_vec4 * GetMeshVertices() const;
    virtual dmAABB GetAABB(const dmMtx & transform) const;  // vtable slot 2
    dm_vec4 GetCenter() const;
    dm_vec4 GetScale() const;
    void SetScale(dm_vec4Arg);
    virtual void ApplyScale(dm_vec4Arg scale);  // vtable slot 12
    virtual void Query(dmBuffer<int, 128> * pBuffer, const dmAABB & aabbIn) const;  // vtable slot 17
    virtual bool RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & xf) const;  // vtable slot 4
    bool RayCastBruteForce(dmRayCastOutput *, const dmRayCastInput *, const dmTransform &) const;
    void WriteToDisk(const char *) const;
private:
    dm_vec4 GetNormal(dm_int32) const;
    dm_vec4 m_center;  // 0x20
    dm_vec4 m_extents;  // 0x30
    dm_vec4 m_unquantize;  // 0x40
    dm_vec4 m_scale;  // 0x50
    dm_vec4 m_invScale;  // 0x60
    const dmMeshNode * m_pRoot;  // 0x70
    const dm_vec4 * m_aVertices;  // 0x78
    const dmMeshTriangle * m_aTriangles;  // 0x80
    dm_int32 m_nodeCount;  // 0x88
    dm_int32 m_vertexCount;  // 0x8c
    dm_int32 m_height;  // 0x90
};
static_assert(sizeof(dmTreeMesh) == 0xa0, "dmTreeMesh");


// Functions defined in this file (bodies are not in the DWARF):
//   line    23  dmTreeMesh::~dmTreeMesh()  [0x7100bfa840]
//   line    97  dm_vec4 dmTreeMesh::GetScale() const  [inline]
