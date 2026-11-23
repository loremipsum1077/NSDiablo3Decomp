// Supplemental analytic checks for convex-contact shared helpers, not match proof.
#include "Contrib/Contrib/Domino/Dynamics/Contacts/dmConvexContact.h"
#include <cassert>
#include <cmath>
#include <cstdio>
static void near(float a, float b) { assert(std::isfinite(a) && std::fabs(a-b)<0.00002f); }
static void near4(dm_vec4 a, dm_vec4 b)
{
    float aa[4], bb[4]; vst1q_f32(aa,a); vst1q_f32(bb,b);
    for (int i=0;i<4;++i) near(aa[i],bb[i]);
}
int main()
{
    // Non-unit, nonsymmetric inputs expose wrong handedness and every product term.
    dm_vec4 a = dmVecSet(0.3f,-0.7f,0.2f,1.5f), b = dmVecSet(-0.4f,0.1f,1.2f,0.8f);
    near4(dmMulQQ(a,b),dmVecSet(-1.22f,-0.85f,1.71f,1.15f));
    near4(dmMulQQ(dm_quat_identity,a),a);
    near4(dmMulQQ(a,dm_quat_identity),a);
    near4(dmMulQQ(a,dmConjugate(a)),dmVecSet(0,0,0,2.87f));
    dmTransform xfA = dmTransformIdentity(), xfB = dmTransformIdentity();
    xfA.q=dmVecSet(0,0,0.707106781f,0.707106781f);
    xfA.p=dmVecSet(3,-2,7);
    xfB.q=dmVecSet(0.5f,0.5f,0.5f,0.5f);
    xfB.p=dmVecSet(-4,5,2);
    dmTransform relative=dmMulT(xfA,xfB);
    dm_vec4 p=dmVecSet(0.25f,-1.5f,2.75f);
    near4(dmMul(xfA,dmMul(relative,p)),dmMul(xfB,p));
    near4(dmMulT(xfA,dmMul(xfA,p)),p);
    dmManifold manifold;
    dmManifold_Create(&manifold);
    assert(manifold.m_pointCount==0);
    near4(manifold.m_tangentImpulse,dmVecSet(0));
    near4(manifold.m_rollingImpulse,dmVecSet(0));
    near(manifold.m_twistImpulse,0);
    for(int i=0;i<4;++i) {
        assert(manifold.m_points[i].isNew && manifold.m_points[i].id.key==0);
        near(manifold.m_points[i].normalImpulse,0);
    }
    dmManifold_ClearNewFlags(&manifold);
    for(int i=0;i<4;++i) assert(!manifold.m_points[i].isNew);
    dmSATCache cache; cache.type=3; cache.Clear(); assert(cache.type==0);
    near4(dmSetW(dmVecSet(1,2,3,4),dmVecSet(9,8,7,6)),dmVecSet(1,2,3,9));
    near(dmLength3_f32(dmVecSet(2,3,6,100)),7);
    std::puts("convex shared-helper analytic checks passed");
}
