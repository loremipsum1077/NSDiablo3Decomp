// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Collision/Functions/dmHomingFunction.h

#pragma once

#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"

struct dmDistanceOutput;
class dmGJKProxy;

struct dmSweep {  // size 0x60
    dmTransform GetTransform(dm_vec4Arg eta) const
    {
        dmTransform xf;
        dm_vec4 c = dmAdd(m_center0, dmMul(eta, dmSub(m_center, m_center0)));
        xf.q = dmQuatNLerp(m_quaternion0, m_quaternion, eta);
        xf.p = dmSub(c, dmRotate(xf.q, m_localCenter));
        return xf;
    }
    void Advance(dm_vec4Arg alpha);
    dm_vec4 m_center;  // 0x00
    dm_vec4 m_quaternion;  // 0x10
    dm_vec4 m_localCenter;  // 0x20
    dm_vec4 m_center0;  // 0x30
    dm_vec4 m_quaternion0;  // 0x40
    dm_vec4 m_alpha0;  // 0x50
};
static_assert(sizeof(dmSweep) == 0x60, "dmSweep");

struct dmHomingFunction {  // size 0x120
    enum Type {
        e_points = 0,
        e_faceA = 1,
        e_faceB = 2,
        e_edges = 3,
    };
    void Initialize(const dmDistanceOutput * pDistance, const dmGJKProxy * pProxyA, const dmSweep & sweepA, const dmGJKProxy * pProxyB, const dmSweep & sweepB, bool useFastEdges, dm_vec4Arg t1);
    bool FindMinSeparation(dm_vec4 * pSeparation, dm_int32 * pIndexA, dm_int32 * pIndexB, dm_vec4Arg t);
    bool Evaluate(dm_vec4 * pSeparation, dm_int32 indexA, dm_int32 indexB, dm_vec4Arg t);
    const dmGJKProxy * m_pProxyA;  // 0x00
    const dmGJKProxy * m_pProxyB;  // 0x08
    dmSweep m_sweepA;  // 0x10
    dmSweep m_sweepB;  // 0x70
    dmHomingFunction::Type m_type;  // 0xd0
    dm_vec4 m_localPoint;  // 0xe0
    dm_vec4 m_axis;  // 0xf0
    dm_vec4 m_localAxisA;  // 0x100
    dm_vec4 m_localAxisB;  // 0x110
};
static_assert(sizeof(dmHomingFunction) == 0x120, "dmHomingFunction");


// Functions defined in this file (bodies are not in the DWARF):
//   line    25  dmTransform dmSweep::GetTransform(dm_vec4Arg eta) const  [inlined into 7 functions]
