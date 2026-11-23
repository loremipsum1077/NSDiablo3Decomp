// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Include/dmHeightField.h

#pragma once

#include "dmBuffer.h"
#include "dmMesh.h"
#include "dmSerialize.h"

struct dmAABB;
struct dmHeightData;
struct dmHeightSample;
struct dmMeshData;
struct dmMtx;
struct dmRayCastInput;
struct dmRayCastOutput;
struct dmTransform;
class dmTriangle;

class dmHeightField : public dmMesh {  // size 0x38
public:
    dmHeightField();
    void Create(const dmHeightData * pData);
    virtual bool IsSolid(dm_int32 index) const;  // vtable slot 13
    virtual void GetTriangle(dmTriangle * pTriangle, dm_int32 index) const;  // vtable slot 14
    virtual void GetTriangleTransformed(dmTriangle * pTriangle, dm_int32 index, const dmTransform & xf) const;  // vtable slot 15
    virtual void GetTriangleFull(dmTriangle * pTriangle, dm_int32 index) const;  // vtable slot 16
    dm_int32 GetMaterial(dm_int32 index) const;
    const dmHeightSample & GetHeightSample(dm_int32) const;
    virtual dmAABB GetAABB(const dmMtx & xf) const;  // vtable slot 2
    virtual void Query(dmBuffer<int, 128> * pBuffer, const dmAABB & aabb) const;  // vtable slot 17
    virtual bool RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & xf) const;  // vtable slot 4
    bool RayCastBruteForce(dmRayCastOutput *, const dmRayCastInput *, const dmTransform &) const;
    void CreateMesh(dmMeshData *) const;
    void WriteToDisk(const char *) const;
    dm_int32 m_countX;  // 0x18
    dm_int32 m_countY;  // 0x1c
    dm_float32 m_cellWidth;  // 0x20
    dm_float32 m_center;  // 0x24
    dm_float32 m_scale;  // 0x28
private:
    bool RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, dm_int32 ix1, dm_int32 iy1) const;
    const dmHeightSample * m_aSamples;  // 0x30
};
static_assert(sizeof(dmHeightField) == 0x38, "dmHeightField");


// Functions defined in this file (bodies are not in the DWARF):
//   line    31  dmHeightField::~dmHeightField()  [0x7100bf46e0]
