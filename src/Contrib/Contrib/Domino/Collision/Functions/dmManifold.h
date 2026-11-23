// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Collision/Functions/dmManifold.h

#pragma once

#include "../../Include/dmSerialize.h"
#include "../../Include/dmStructures.h"
#include "../../Include/dmShapes.h"

struct dmSATCache {  // size 0x8
    void Clear();
    dm_float32 separation;  // 0x00
    dm_uint8 indexA;  // 0x04
    dm_uint8 indexB;  // 0x05
    dm_uint8 type;  // 0x06
};
static_assert(sizeof(dmSATCache) == 0x8, "dmSATCache");


// Functions defined in this file (bodies are not in the DWARF):
//   line    26  void dmSATCache::Clear()  [inlined into 4 functions]
//   line    88  void dmManifold_Create(dmManifold * pManifold)  [inlined into 5 functions]
//   line   114  void dmManifold_ClearCache(dmManifold * pManifold)  [inlined into 1 functions]
//   line   147  void dmManifold_ClearNewFlags(dmManifold * pManifold)  [inlined into 2 functions]

inline void dmSATCache::Clear() { type = 0; }
inline void dmManifold_Create(dmManifold * pManifold)
{
    pManifold->m_pointCount = 0;
    pManifold->m_tangentImpulse = dmVecSet(0.0f);
    pManifold->m_rollingImpulse = dmVecSet(0.0f);
    pManifold->m_twistImpulse = 0.0f;
    for (dm_int32 i = 0; i < 4; ++i) {
        pManifold->m_points[i].normalImpulse = 0.0f;
        pManifold->m_points[i].id.key = 0;
        pManifold->m_points[i].isNew = true;
    }
}
// dmManifold.h:114; written from the copy inlined into dmMeshContact::ReduceManifolds (stores at lines 118, 120-130).
inline void dmManifold_ClearCache(dmManifold * pManifold)
{
    pManifold->m_tangentImpulse = dmVecSet(0.0f);
    pManifold->m_rollingImpulse = dmVecSet(0.0f);
    pManifold->m_twistImpulse = 0.0f;
    for (dm_int32 i = 0; i < 4; ++i) {
        pManifold->m_points[i].normalImpulse = 0.0f;
        pManifold->m_points[i].isNew = true;
    }
}
inline void dmManifold_ClearNewFlags(dmManifold * pManifold)
{
    for (dm_int32 i = 0; i < 4; ++i) pManifold->m_points[i].isNew = false;
}
struct dmGJKCache;
class dmPolytope;
void dmSSManifold(dmManifold *, const dmSphere *, const dmSphere *);
void dmCSManifold(dmManifold *, const dmCapsule *, const dmSphere *);
void dmCCManifold(dmManifold *, const dmCapsule *, const dmCapsule *);
void dmPSManifold(dmManifold *, dmGJKCache *, const dmPolytope *, const dmSphere *);
void dmPCManifold(dmManifold *, dmGJKCache *, const dmPolytope *, const dmCapsule *);
void dmPPManifold(dmManifold *, dmSATCache *, const dmPolytope *, const dmPolytope *, const dmTransform &);

void dmTSManifold(dmManifold *, dmGJKCache *, const dmTriangle *, const dmSphere *);
