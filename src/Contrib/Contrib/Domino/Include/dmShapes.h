// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Include/dmShapes.h

#pragma once

#include "dmMath.h"
#include "dmSerialize.h"

class dmSphere;
struct dmAABB;
struct dmRayCastInput;
struct dmRayCastOutput;

struct dmMassData {  // size 0x60
    dmMtx I;  // 0x00
    dm_vec4 center;  // 0x40
    dm_float32 mass;  // 0x50
};
static_assert(sizeof(dmMassData) == 0x60, "dmMassData");

struct dmFeature {  // size 0x8
    enum {
        e_face = 0,
        e_edge = 1,
        e_vertex = 2,
        e_unknown = 3,
    };
    dm_int32 index;  // 0x00
    dm_int32 type;  // 0x04
};
static_assert(sizeof(dmFeature) == 0x8, "dmFeature");

class dmShape {  // size 0x10
    // vtable pointer
public:
    enum Type {
        e_sphere = 0,
        e_capsule = 1,
        e_triangle = 2,
        e_polytope = 3,
        e_mesh = 4,
        e_unknown = 5,
    };
public:
    dmShape();
    virtual ~dmShape();  // vtable slot 0
    virtual dmAABB GetAABB(const dmMtx &) const = 0;  // vtable slot 2
    virtual dmAABB GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const;  // vtable slot 3
    virtual bool RayCast(dmRayCastOutput *, const dmRayCastInput *, const dmTransform &) const = 0;  // vtable slot 4
    virtual void ComputeMass(dmMassData * pMassData, dm_float32 density) const;  // vtable slot 5
    virtual dm_vec4 ComputeCentroid() const;  // vtable slot 6
    virtual dm_vec4 ComputeSurfaceArea() const;  // vtable slot 7
    virtual dm_vec4 ComputeVolume() const;  // vtable slot 8
    virtual dm_vec4 ComputeProjectedSurfaceArea(dm_vec4Arg dir) const;  // vtable slot 9
    virtual dm_vec4 ComputeSubmergedVolume(dm_vec4 * pCentroid, const dmTransform & transform, dm_vec4Arg normal, dm_vec4Arg offset) const;  // vtable slot 10
    virtual bool TestOverlap(const dmSphere &) const = 0;  // vtable slot 11
    virtual void ApplyScale(dm_vec4Arg scale);  // vtable slot 12
    dmShape::Type m_type;  // 0x08
    dm_float32 m_radius;  // 0x0c
};
static_assert(sizeof(dmShape) == 0x10, "dmShape");

class dmSphere : public dmShape {  // size 0x20
public:
    dmSphere();
    void Create(dm_vec4Arg p, dm_float32 radius);
    virtual dmAABB GetAABB(const dmMtx & xf) const;  // vtable slot 2
    virtual dmAABB GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const;  // vtable slot 3
    virtual bool RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & transform) const;  // vtable slot 4
    bool RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput) const;
    virtual void ComputeMass(dmMassData * pMassData, dm_float32 density) const;  // vtable slot 5
    virtual dm_vec4 ComputeCentroid() const;  // vtable slot 6
    virtual dm_vec4 ComputeSurfaceArea() const;  // vtable slot 7
    virtual dm_vec4 ComputeVolume() const;  // vtable slot 8
    virtual dm_vec4 ComputeProjectedSurfaceArea(dm_vec4Arg dir) const;  // vtable slot 9
    virtual dm_vec4 ComputeSubmergedVolume(dm_vec4 * pCentroid, const dmTransform & transform, dm_vec4Arg normal, dm_vec4Arg offset) const;  // vtable slot 10
    virtual bool TestOverlap(const dmSphere & sphere) const;  // vtable slot 11
    virtual void ApplyScale(dm_vec4Arg scale);  // vtable slot 12
    dm_vec4 m_p;  // 0x10
};
static_assert(sizeof(dmSphere) == 0x20, "dmSphere");

class dmCapsule : public dmShape {  // size 0x30
public:
    dmCapsule();
    void Create(dm_vec4Arg p1, dm_vec4Arg p2, dm_float32 radius);
    void Fit(const dm_vec4 *, dm_int32);
    virtual dmAABB GetAABB(const dmMtx & transform) const;  // vtable slot 2
    virtual dmAABB GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const;  // vtable slot 3
    virtual bool RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & transform) const;  // vtable slot 4
    virtual void ComputeMass(dmMassData * pMassData, dm_float32 density) const;  // vtable slot 5
    virtual dm_vec4 ComputeCentroid() const;  // vtable slot 6
    virtual dm_vec4 ComputeSurfaceArea() const;  // vtable slot 7
    virtual dm_vec4 ComputeVolume() const;  // vtable slot 8
    virtual dm_vec4 ComputeProjectedSurfaceArea(dm_vec4Arg dir) const;  // vtable slot 9
    virtual dm_vec4 ComputeSubmergedVolume(dm_vec4 * pCentroid, const dmTransform & transform, dm_vec4Arg normal, dm_vec4Arg offset) const;  // vtable slot 10
    virtual bool TestOverlap(const dmSphere & sphere) const;  // vtable slot 11
    virtual void ApplyScale(dm_vec4Arg scale);  // vtable slot 12
    dm_vec4 m_p1;  // 0x10
    dm_vec4 m_p2;  // 0x20
};
static_assert(sizeof(dmCapsule) == 0x30, "dmCapsule");

class dmTriangle : public dmShape {  // size 0xa0
public:
    dmTriangle();
    dmAABB GetAABB() const;
    inline virtual dmAABB GetAABB(const dmMtx & transform) const;  // vtable slot 2
    inline virtual dmAABB GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const;  // vtable slot 3
    inline virtual dm_vec4 ComputeCentroid() const;  // vtable slot 6
    virtual bool RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & transform) const;  // vtable slot 4
    bool RayCast(dmRayCastOutput * pOutput, dm_vec4Arg p1, dm_vec4Arg p2, dm_vec4Arg maxLambda) const;
    virtual bool TestOverlap(const dmSphere & sphere) const;  // vtable slot 11
    bool IsBehind(dm_vec4Arg) const;
    dm_vec4 GetNormal() const;
    void GetWingNormals(dm_vec4 * aNormals) const;
    dm_vec4 m_p[3];  // 0x10
    dm_vec4 m_aWings[3];  // 0x40
    dm_int32 m_aIndices[3];  // 0x70
    dm_int32 m_aWingIndices[3];  // 0x7c
    bool m_aShared[3];  // 0x88
    dm_int32 m_triangleIndex;  // 0x8c
    dm_uint16 m_materialIndex;  // 0x90
    dm_uint16 m_gameFlags;  // 0x92
};
static_assert(sizeof(dmTriangle) == 0xa0, "dmTriangle");


// Functions defined in this file (bodies are not in the DWARF):
//   line    55  dmShape & dmShape::operator=(const dmShape &)  [inlined into 2 functions]
//   line    70  dmShape::dmShape()  [inlined into 1 functions]
//   line    71  dmShape::~dmShape()  [0x7100bf46d0]
//   line    79  dmAABB dmShape::GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const  [0x7100bf46f0]
//   line   102  void dmShape::ComputeMass(dmMassData * pMassData, dm_float32 density) const  [0x7100bf4740]
//   line   111  dm_vec4 dmShape::ComputeCentroid() const  [0x7100bf4760]
//   line   114  dm_vec4 dmShape::ComputeSurfaceArea() const  [0x7100bf4770]
//   line   117  dm_vec4 dmShape::ComputeVolume() const  [0x7100bf4780]
//   line   120  dm_vec4 dmShape::ComputeProjectedSurfaceArea(dm_vec4Arg dir) const  [0x7100bf4790]
//   line   123  dm_vec4 dmShape::ComputeSubmergedVolume(dm_vec4 * pCentroid, const dmTransform & transform, dm_vec4Arg normal, dm_vec4Arg offset) const  [0x7100bf47a0]
//   line   136  void dmShape::ApplyScale(dm_vec4Arg scale)  [0x7100bf47b0]
//   line   145  dmSphere::~dmSphere()  [0x7100bf9540]
//   line   148  dmSphere::dmSphere()  [inlined into 7 functions]
//   line   170  dm_vec4 dmSphere::ComputeCentroid() const  [0x7100bf9550]
//   line   194  dmCapsule & dmCapsule::operator=(const dmCapsule &)  [inlined into 2 functions]
//   line   194  dmCapsule::~dmCapsule()  [0x7100bf9560]
//   line   197  dmCapsule::dmCapsule()  [inlined into 11 functions]
//   line   245  dmTriangle & dmTriangle::operator=(const dmTriangle &)  [inlined into 1 functions]
//   line   245  dmTriangle::~dmTriangle()  [0x7100bf9570]
//   line   248  dmTriangle::dmTriangle()  [inlined into 8 functions]
//   line   279  dmAABB dmTriangle::GetAABB(const dmMtx & transform) const  [0x7100bf9580]
//   line   291  dmAABB dmTriangle::GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const  [0x7100bf95f0]
//   line   311  dm_vec4 dmTriangle::ComputeCentroid() const  [0x7100bf96d0]
//   line   336  dm_vec4 dmTriangle::GetNormal() const  [inlined into 4 functions]
//   line   345  void dmTriangle::GetWingNormals(dm_vec4 * aNormals) const  [inlined into 2 functions]

// dmShapes.h:70 sets type and radius (row 70:43 = the m_radius '=' after "{ m_type = e_unknown; ", the lone radius store in dmTCManifold); dmCapsule (197) sets only m_type (197:23). The original destructor is ret.
inline dmShape::dmShape() { m_type = e_unknown; m_radius = 0.0f; }
inline dmShape::~dmShape() {}
// dmCSManifold's inline constructor stores the sphere vptr and zero type/radius.
inline dmSphere::dmSphere() { m_type = e_sphere; m_radius = 0.0f; }

inline dm_vec4 dmTriangle::GetNormal() const
{
    dm_vec4 e1 = dmSub(m_p[1], m_p[0]);
    dm_vec4 e2 = dmSub(m_p[2], m_p[0]);
    dm_vec4 n = dmCross(e1, e2);
    return dmNormalize3(n);
}

// dmShapes.h:345. Inference: per-edge wing normal from the tail and head vertex
// minus the wing vertex (dmTCManifold.cpp ComputeFV's inlined copy: loads of
// m_aWings[i], m_p[(i + 1) % 3] and m_p[i]); one block per edge with e1/e2/n locals.
inline void dmTriangle::GetWingNormals(dm_vec4 * aNormals) const
{
    {
        dm_vec4 e1 = dmSub(m_p[1], m_aWings[0]);
        dm_vec4 e2 = dmSub(m_p[0], m_aWings[0]);
        dm_vec4 n = dmCross(e1, e2);
        aNormals[0] = dmNormalize3(n);
    }

    {
        dm_vec4 e1 = dmSub(m_p[2], m_aWings[1]);
        dm_vec4 e2 = dmSub(m_p[1], m_aWings[1]);
        dm_vec4 n = dmCross(e1, e2);
        aNormals[1] = dmNormalize3(n);
    }

    {
        dm_vec4 e1 = dmSub(m_p[0], m_aWings[2]);
        dm_vec4 e2 = dmSub(m_p[2], m_aWings[2]);
        dm_vec4 n = dmCross(e1, e2);
        aNormals[2] = dmNormalize3(n);
    }
}

inline dmCapsule::dmCapsule() { m_type = e_capsule; }

// dmShapes.h:117-136. The out-of-line copies sit in dmMesh.cpp, the first unit
// whose vtable uses them.
inline dm_vec4 dmShape::ComputeVolume() const { return dmVecSet(0.0f); }
inline dm_vec4 dmShape::ComputeProjectedSurfaceArea(dm_vec4Arg dir) const { return dmVecSet(0.0f); }
inline dm_vec4 dmShape::ComputeSubmergedVolume(dm_vec4 * pCentroid, const dmTransform & transform, dm_vec4Arg normal, dm_vec4Arg offset) const
{
    *pCentroid = dmVecSet(0.0f);
    return dmVecSet(0.0f);
}
inline void dmShape::ApplyScale(dm_vec4Arg scale) {}

// dmShapes.h:248.
inline dmTriangle::dmTriangle()
{
    m_type = e_triangle;
    m_radius = 0.032808397f;
    m_triangleIndex = -1;
    m_aIndices[0] = -1;
    m_aIndices[1] = -1;
    m_aIndices[2] = -1;
    m_aWings[0] = m_aWings[1] = m_aWings[2] = dmVecSet(0.0f);
    m_aWingIndices[0] = -1;
    m_aWingIndices[1] = -1;
    m_aWingIndices[2] = -1;
    m_aShared[0] = false;
    m_aShared[1] = false;
    m_aShared[2] = false;
    m_materialIndex = 0;
    m_gameFlags = 0;
}

#include "dmCollision.h"

// dmShapes.h:170.
inline dm_vec4 dmSphere::ComputeCentroid() const
{
    return m_p;
}

// dmShapes.h:279-311. Defined inline, which leaves dmTriangle's key function
// (and its vtable) in dmShapes.cpp.
inline dmAABB dmTriangle::GetAABB(const dmMtx & transform) const
{
    dm_vec4 p1 = dmMul34(transform, m_p[0]);
    dm_vec4 p2 = dmMul34(transform, m_p[1]);
    dm_vec4 p3 = dmMul34(transform, m_p[2]);

    dmAABB aabb;
    aabb.m_lowerBound = dmMin(p1, p2, p3);
    aabb.m_upperBound = dmMax(p1, p2, p3);
    return aabb;
}

inline dmAABB dmTriangle::GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const
{
    dm_vec4 p11 = dmMul34(xf1, m_p[0]);
    dm_vec4 p12 = dmMul34(xf1, m_p[1]);
    dm_vec4 p13 = dmMul34(xf1, m_p[2]);
    dm_vec4 p21 = dmMul34(xf2, m_p[0]);
    dm_vec4 p22 = dmMul34(xf2, m_p[1]);
    dm_vec4 p23 = dmMul34(xf2, m_p[2]);
    dm_vec4 lower1 = dmMin(p11, p12, p13);
    dm_vec4 lower2 = dmMin(p21, p22, p23);
    dm_vec4 upper1 = dmMax(p11, p12, p13);
    dm_vec4 upper2 = dmMax(p21, p22, p23);

    dmAABB aabb;
    aabb.m_lowerBound = dmMin(lower1, lower2);
    aabb.m_upperBound = dmMax(upper1, upper2);
    return aabb;
}

inline dm_vec4 dmTriangle::ComputeCentroid() const
{
    return dmMul(dm_vec4_oneThird, dmAdd(m_p[0], dmAdd(m_p[1], m_p[2])));
}

// dmShapes.h:79-116, written for dmHeightField's vtable (the first unit whose
// vtable uses them). GetSweptAABB's assert records the header path and line 83
// (original binary at 0x7100bf4724); aabb is declared on line 84 (DWARF).
#include <string.h>

#line 79 "../../..\\Domino/Include/dmShapes.h"
inline dmAABB dmShape::GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const
{


    dmAssert(false);
    dmAABB aabb;
    aabb.m_lowerBound = dmVecSet(0.0f);  // inferred: the original returns zero vectors (movi at line 87)
    aabb.m_upperBound = dmVecSet(0.0f);  // inferred, as above
    return aabb;
}













inline void dmShape::ComputeMass(dmMassData * pMassData, dm_float32 density) const
{

    pMassData->center = dmVecSet(0.0f);  // inferred (lines 105-107): three zeroings that MemCpyOpt merges into the
    pMassData->mass = 0.0f;  // one 0x54-byte memset the original calls with bl (no tail call); this order is the
    memset(&pMassData->I, 0, sizeof(dmMtx));  // one of six that puts that memset on line 107 as in the original
}


inline dm_vec4 dmShape::ComputeCentroid() const { return dmVecSet(0.0f); }


inline dm_vec4 dmShape::ComputeSurfaceArea() const { return dmVecSet(0.0f); }
