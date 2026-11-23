// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Include/dmCollision.h

#pragma once

#include "dmMath.h"
#include "dmSerialize.h"

enum dmCollisionCode {
    dm_collideStop = 0,
    dm_collideClip = 1,
    dm_collideContinue = 2,
};

struct dmRayCastInput {  // size 0x50
    dmRayCastInput();
    dm_vec4 p1;  // 0x00
    dm_vec4 p2;  // 0x10
    dm_vec4 maxLambda;  // 0x20
    dm_vec4 slop;  // 0x30
    dm_uint16 triangleMask;  // 0x40
};
static_assert(sizeof(dmRayCastInput) == 0x50, "dmRayCastInput");

struct dmShapeCastOutput {  // size 0x40
    dm_vec4 point;  // 0x00
    dm_vec4 normal;  // 0x10
    dm_int32 triangleIndex;  // 0x20
    dm_vec4 lambda;  // 0x30
};
static_assert(sizeof(dmShapeCastOutput) == 0x40, "dmShapeCastOutput");

struct dmRayCastOutput {  // size 0x40
    dm_vec4 normal;  // 0x00
    dm_int32 triangleIndex;  // 0x10
    dm_uint16 materialID;  // 0x14
    dm_vec4 lambda;  // 0x20
    dm_int32 nodeVisitCount;  // 0x30
    dm_int32 leafVisitCount;  // 0x34
};
static_assert(sizeof(dmRayCastOutput) == 0x40, "dmRayCastOutput");

struct dmAABB {  // size 0x20
    bool IsValid() const;
    void Add(const dmAABB & other);
    void Merge(const dmAABB & aabb1, const dmAABB & aabb2);
    bool Intersect(const dmAABB &, const dmAABB &);
    bool Contains(dm_vec4Arg point) const;
    bool Contains(const dmAABB & aabb) const;
    dm_vec4 GetSurfaceArea() const;
    dm_float32 GetSurfaceArea_f32() const;
    dm_vec4 GetCenter() const;
    dm_vec4 GetExtents() const;
    void Fit(const dm_vec4 *, dm_int32);
    dm_vec4 m_lowerBound;  // 0x00
    dm_vec4 m_upperBound;  // 0x10
};
static_assert(sizeof(dmAABB) == 0x20, "dmAABB");

struct dmOBB {  // size 0x30
    dmTransform m_transform;  // 0x00
    dm_vec4 m_extents;  // 0x20
    dmAABB ComputeAABB();
    void Fit(const dm_vec4 *, dm_int32);
};
static_assert(sizeof(dmOBB) == 0x30, "dmOBB");


// dmCollision.h:30; the triangle mask (line 34) is the only store that
// survives in dmSphere::RayCast, every other member being assigned after it.
// Line 32 (maxLambda) is from dmWorld_RayCastClosest, where the load of
// dm_vec4_one at 0xbccb98/0xbccba0/0xbccbb0 carries dmCollision.h:32.
inline dmRayCastInput::dmRayCastInput()
{
    maxLambda = dm_vec4_one;
    slop = dmVecSet(0.0f);  // dmCollision.h:33: the zero store survives in dmHeightField::RayCast (stp xzr at 0xbf40ac, line 33); spelling inferred
    triangleMask = 0xFFFF;
}

inline void dmAABB::Add(const dmAABB & other)
{
    m_lowerBound = dmMin(m_lowerBound, other.m_lowerBound);
    m_upperBound = dmMax(m_upperBound, other.m_upperBound);
}

inline bool dmTestOverlap(const dmAABB & a, const dmAABB & b)
{
    dm_vec4 d1 = dmSub(a.m_upperBound, b.m_lowerBound);
    dm_vec4 d2 = dmSub(b.m_upperBound, a.m_lowerBound);
    dm_vec4 d = dmMin(d1, d2);
    return dmAll_GE3(d, vdupq_n_f32(0.0f));
}


// Functions defined in this file (bodies are not in the DWARF):
//   line    30  dmRayCastInput::dmRayCastInput()  [inlined into 3 functions]
//   line    95  dm_float32 dmAABB::GetSurfaceArea_f32() const  [inline]
//   line   127  bool dmAABB::IsValid() const  [inlined into 2 functions]
//   line   139  void dmAABB::Add(const dmAABB & other)  [inlined into 1 functions]
//   line   146  dm_vec4 dmAABB::GetSurfaceArea() const  [inlined into 2 functions]
//   line   157  dm_vec4 dmAABB::GetCenter() const  [inlined into 3 functions]
//   line   163  dm_vec4 dmAABB::GetExtents() const  [inlined into 3 functions]
//   line   169  void dmAABB::Merge(const dmAABB & aabb1, const dmAABB & aabb2)  [inlined into 4 functions]
//   line   184  bool dmAABB::Contains(const dmAABB & aabb) const  [inlined into 2 functions]
//   line   190  bool dmAABB::Contains(dm_vec4Arg point) const  [inline]
//   line   196  bool dmTestOverlap(const dmAABB & a, const dmAABB & b)  [inlined into 8 functions]

inline dm_vec4 dmAABB::GetCenter() const
{
    return dmMul(dm_vec4_oneHalf, dmAdd(m_lowerBound, m_upperBound));
}
inline dm_vec4 dmAABB::GetExtents() const
{
    return dmMul(dm_vec4_oneHalf, dmSub(m_upperBound, m_lowerBound));
}
inline void dmAABB::Merge(const dmAABB & aabb1, const dmAABB & aabb2)
{
    m_lowerBound = dmMin(aabb1.m_lowerBound, aabb2.m_lowerBound);
    m_upperBound = dmMax(aabb1.m_upperBound, aabb2.m_upperBound);
}
inline bool dmAABB::IsValid() const
{
    dm_vec4 d = dmSub(m_upperBound, m_lowerBound);
    if (dmF32(d) < 0.0f || dmGetY(d) < 0.0f) return false;
    return dmIsFinite3(m_lowerBound) && dmIsFinite3(m_upperBound);
}
inline bool dmAABB::Contains(const dmAABB & aabb) const
{
    return dmAll_LE3(m_lowerBound, aabb.m_lowerBound) && dmAll_LE3(aabb.m_upperBound, m_upperBound);
}

// Original polynomial, including its repeated z factors; verified by InsertLeaf.
inline dm_vec4 dmAABB::GetSurfaceArea() const {
    dm_vec4 d = dmSub(m_upperBound, m_lowerBound);
    dm_vec4 dx = dmSplatX(d), dy = dmSplatY(d);
    dm_vec4 area = dmMul(dx, dy);
    area = dmMulAdd(area, dmSplatZ(d), dx);
    area = dmMulAdd(area, dmSplatZ(d), dy);
    return dmAdd(area, area);
}
