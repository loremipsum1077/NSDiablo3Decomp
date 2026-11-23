// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Collision/Functions/dmDistance.h

#pragma once

#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"
#include "../../Include/dmSettings.h"

class dmShape;

struct dmGJKCache {  // size 0xc
    void Clear();
    dm_uint8 GetCount() const;
    dm_float32 metric;  // 0x00
    dm_uint8 indexAs[4];  // 0x04
    dm_uint8 indexBs[4];  // 0x08
};
static_assert(sizeof(dmGJKCache) == 0xc, "dmGJKCache");

class dmGJKProxy {  // size 0x20
public:
    dmGJKProxy();
    void Set(const dmShape * pPrimitive);
    void Set(const dm_vec4 * aVertices, dm_int32 count, dm_vec4Arg radius);
    dm_vec4 GetVertex(dm_int32 index) const;
    dm_int32 GetSupport(dm_vec4 * pVertex, dm_vec4Arg direction) const;
    dm_vec4 GetRadius() const;
private:
    const dm_vec4 * m_aVertices;  // 0x00
    dm_int32 m_count;  // 0x08
    dm_vec4 m_radius;  // 0x10
};
static_assert(sizeof(dmGJKProxy) == 0x20, "dmGJKProxy");

struct dmDistanceInput {  // size 0x90
    dmDistanceInput();
    dmGJKProxy proxyA;  // 0x00
    dmGJKProxy proxyB;  // 0x20
    dmTransform transformA;  // 0x40
    dmTransform transformB;  // 0x60
    dmGJKCache cache;  // 0x80
    bool useRadii;  // 0x8c
};
static_assert(sizeof(dmDistanceInput) == 0x90, "dmDistanceInput");

struct dmDistanceOutput {  // size 0x60
    dm_vec4 pointA;  // 0x00
    dm_vec4 pointB;  // 0x10
    dm_vec4 normal;  // 0x20
    dmGJKCache cache;  // 0x30
    dm_vec4 distance;  // 0x40
    dm_int32 iterationCount;  // 0x50
    dm_int32 exitCode;  // 0x54
};
static_assert(sizeof(dmDistanceOutput) == 0x60, "dmDistanceOutput");


// Functions defined in this file (bodies are not in the DWARF):
//   line    27  void dmGJKCache::Clear()  [inlined into 8 functions]
//   line    43  dmGJKProxy::dmGJKProxy()  [inlined into 7 functions]
//   line    49  void dmGJKProxy::Set(const dm_vec4 * aVertices, dm_int32 count, dm_vec4Arg radius)  [inlined into 2 functions]
//   line    67  dm_vec4 dmGJKProxy::GetVertex(dm_int32 index) const  [inlined into 3 functions]
//   line    74  dm_int32 dmGJKProxy::GetSupport(dm_vec4 * pVertex, dm_vec4Arg direction) const  [inlined into 3 functions]
//   line   104  dm_vec4 dmGJKProxy::GetRadius() const  [inlined into 1 functions]
//   line   116  dmDistanceInput::dmDistanceInput()  [inlined into 5 functions]
//   line   201  dm_uint8 dmGJKCache::GetCount() const  [inlined into 5 functions]

// Recovered from the inlined cache and support operations in dmDistance.
inline void dmGJKCache::Clear()
{
    indexAs[0] = indexAs[1] = indexAs[2] = indexAs[3] = 0xff;
}
inline dmGJKProxy::dmGJKProxy() : m_aVertices(NULL), m_count(0), m_radius(dmVecSet(0.0f)) {}
inline void dmGJKProxy::Set(const dm_vec4 * aVertices, dm_int32 count, dm_vec4Arg radius)
{
    m_aVertices = aVertices;
    m_count = count;
    m_radius = radius;
}
inline dm_vec4 dmGJKProxy::GetVertex(dm_int32 index) const
{
#line 69 "../../..\\Domino/Collision/Functions/dmDistance.h"
    dmAssert(0 <= index && index < m_count);
    return m_aVertices[index];
}
inline dm_int32 dmGJKProxy::GetSupport(dm_vec4 * pVertex, dm_vec4Arg direction) const
{
#line 76 "../../..\\Domino/Collision/Functions/dmDistance.h"
    dmAssert(m_count > 0);

    dm_vec4 bestVertex = m_aVertices[0];
    dm_vec4 bestDot = dmDot3(direction, bestVertex);
    dmVecI32 bestIndexVec, indexVec;
    bestIndexVec.v = dmVecSet(0.0f);
    indexVec.v = dmVecSet(0.0f);

    for (dm_int32 i = 1; i < m_count; ++i)
    {
        indexVec.i[0] = i;
        dm_vec4 v = m_aVertices[i];
        dm_vec4 dot = dmDot3(direction, v);
        dm_vec4 mask = dmCompareGT(dot, bestDot);
        bestVertex = dmSelect(mask, v, bestVertex);
        bestDot = dmSelect(mask, dot, bestDot);
        bestIndexVec.v = dmSelect(mask, indexVec.v, bestIndexVec.v);
    }

    dm_int32 bestIndex = bestIndexVec.i[0];  // DWARF locals bestIndexVec/indexVec (80), bestIndex (95, row 95:24); rows 84:37, 86:18, 87:16.
    *pVertex = bestVertex;
    return bestIndex;
}
inline dm_vec4 dmGJKProxy::GetRadius() const { return m_radius; }
inline dmDistanceInput::dmDistanceInput() { cache.Clear(); }
inline dm_uint8 dmGJKCache::GetCount() const
{
    if (indexAs[0] == 0xff) return 0;
    if (indexAs[1] == 0xff) return 1;
    if (indexAs[2] == 0xff) return 2;
    if (indexAs[3] == 0xff) return 3;
    return 4;
}
void dmDistance(dmDistanceOutput * pOutput, const dmDistanceInput * pInput);

class dmSphere;
class dmCapsule;
dm_vec4 dmDistance(dm_vec4 * pPointA, dm_vec4 * pPointB, const dmCapsule * pCapsuleA, const dmSphere * pSphereB);
dm_vec4 dmDistance(dm_vec4 * pPointA, dm_vec4 * pPointB, const dmCapsule * pCapsuleA, const dmCapsule * pCapsuleB);
