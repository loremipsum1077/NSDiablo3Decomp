// Supplemental host geometry tests. Unimplemented virtual methods use trapping
// link fixtures; all geometry exercised here uses production bodies.
#include "Contrib/Contrib/Domino/Collision/Functions/dmDistance.h"
#include "Contrib/Contrib/Domino/Collision/Functions/dmManifold.h"
#include "Contrib/Contrib/Domino/Include/dmPolytope.h"
#include "Contrib/Contrib/Domino/Dynamics/Contacts/dmConvexContact.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>

bool dm_assertOnce = false;
void dmAssertFunction(const char* message, const char* file, dm_int32 line, void*, void*)
{
    std::fprintf(stderr, "%s:%d: %s\n", file, line, message);
    std::abort();
}
static void near(float actual, float expected)
{
    if (!(std::isfinite(actual) && std::fabs(actual-expected)<0.0002f)) {
        std::fprintf(stderr,"%g != %g\n",actual,expected); std::abort();
    }
}
static void vec(dm_vec4 actual, float x, float y, float z)
{
    near(actual[0],x); near(actual[1],y); near(actual[2],z);
}
class Box : public dmPolytope
{
public:
    Box(dm_vec4* vertices, dm_vec4* planes) {
        m_aVertices=vertices; m_vertexCount=8;
        m_aPlanes=planes; m_faceCount=6; m_radius=0.1f;
        // External storage: production destructor must not release it.
        m_isTriangle=true;
    }
};
static void polytopeSphere()
{
    dm_vec4 vertices[8];
    for(int i=0;i<8;++i) vertices[i]=dmVecSet((i&1)?1.f:-1.f,(i&2)?1.f:-1.f,(i&4)?1.f:-1.f,0.f);
    dm_vec4 planes[6]={dmVecSet(1,0,0,1),dmVecSet(-1,0,0,1),dmVecSet(0,1,0,1),
        dmVecSet(0,-1,0,1),dmVecSet(0,0,1,1),dmVecSet(0,0,-1,1)};
    Box box(vertices,planes); dmSphere sphere; sphere.m_radius=.5f;
    dmGJKCache cache; cache.Clear(); dmManifold m;
    sphere.m_p=dmVecSet(2,0,0,0);
    dmPSManifold(&m,&cache,&box,&sphere); assert(m.m_pointCount==0);
    assert(cache.GetCount()>0); // Rejection must still refresh GJK cache.
    sphere.m_p=dmVecSet(1.25f,0,0,0);
    for(int pass=0;pass<2;++pass) {
        dmPSManifold(&m,&cache,&box,&sphere); assert(m.m_pointCount==1);
        vec(m.m_normal,1,0,0); vec(m.m_points[0].point,.65f,0,0);
        near(m.m_points[0].separation,-.35f); assert(m.m_points[0].id.key==0);
    }
    // Deep interior uses closest plane; first plane wins the six-way tie.
    sphere.m_p=dmVecSet(0,0,0,0); cache.Clear();
    dmPSManifold(&m,&cache,&box,&sphere); assert(m.m_pointCount==1);
    vec(m.m_normal,1,0,0); near(m.m_normal[3],1);
    vec(m.m_points[0].point,-.6f,0,0); near(m.m_points[0].point[3],-.6f);
    near(m.m_points[0].separation,-1.6f);
    // Near-surface fallback chooses +Y instead of hard-coded first face.
    sphere.m_p=dmVecSet(0,.9995f,0,0); cache.Clear();
    dmPSManifold(&m,&cache,&box,&sphere);
    vec(m.m_normal,0,1,0); near(m.m_points[0].separation,-.6005f);
}
// Test-only input constructor: triangle owner lifecycle is not yet implemented.
// Only fields consumed by the production manifold are initialized below.
dmTriangle::dmTriangle() { m_type=e_triangle; m_radius=0.f; }

static void triangleSphere()
{
    dmTriangle triangle;
    triangle.m_p[0]=dmVecSet(-2,-2,0,0);
    triangle.m_p[1]=dmVecSet(2,-2,0,0);
    triangle.m_p[2]=dmVecSet(0,2,0,0);
    triangle.m_triangleIndex=73;
    dmSphere sphere; sphere.m_radius=.5f;
    dmManifold m; dmGJKCache cache; cache.Clear();
    sphere.m_p=dmVecSet(0,0,.25f,0);
    dmTSManifold(&m,&cache,&triangle,&sphere);
    assert(m.m_pointCount==1); vec(m.m_normal,0,0,1);
    near(m.m_points[0].separation,.25f-(.5f+.032808397f));
    assert(m.m_points[0].id.fp.triangleIndex==73);
    sphere.m_p=dmVecSet(0,0,-.01f,0);
    dmTSManifold(&m,&cache,&triangle,&sphere); assert(m.m_pointCount==0);
    sphere.m_p=dmVecSet(0,0,2,0);
    dmTSManifold(&m,&cache,&triangle,&sphere); assert(m.m_pointCount==0);
    sphere.m_p=dmVecSet(0,0,0,0); cache.Clear();
    dmTSManifold(&m,&cache,&triangle,&sphere); assert(m.m_pointCount==1);
    assert(m.m_points[0].id.fp.triangleIndex==73);
}
static void capsules()
{
    dmCapsule a,b; dmSphere sphere; dmManifold m;
    a.m_p1=dmVecSet(-1,0,0,0); a.m_p2=dmVecSet(1,0,0,0); a.m_radius=.5f;
    b.m_p1=dmVecSet(-1,.75f,0,0); b.m_p2=dmVecSet(1,.75f,0,0); b.m_radius=.5f;
    dmCCManifold(&m,&a,&b); assert(m.m_pointCount==1);
    vec(m.m_normal,0,1,0); near(m.m_points[0].separation,-.25f);
    b.m_p1=a.m_p1; b.m_p2=a.m_p2;
    dmCCManifold(&m,&a,&b); assert(m.m_pointCount==1);
    vec(m.m_normal,1,0,0); near(m.m_points[0].separation,-1.f);
    sphere.m_radius=.5f; sphere.m_p=dmVecSet(0,.75f,0,0);
    dmCSManifold(&m,&a,&sphere); assert(m.m_pointCount==1);
    vec(m.m_normal,0,1,0); vec(m.m_points[0].point,0,.375f,0);
    near(m.m_points[0].separation,-.25f);
    sphere.m_p=dmVecSet(0,0,0,0);
    dmCSManifold(&m,&a,&sphere); assert(m.m_pointCount==1);
    vec(m.m_normal,0,-1,0); near(m.m_points[0].separation,-1.f);
    a.m_p1=dmVecSet(0,0,-1,0); a.m_p2=dmVecSet(0,0,1,0);
    dmCSManifold(&m,&a,&sphere); vec(m.m_normal,1,0,0);
    sphere.m_p=dmVecSet(4,0,0,0);
    dmCSManifold(&m,&a,&sphere); assert(m.m_pointCount==0);
}
static void convexDispatch()
{
    dm_vec4 vertices[8];
    for(int i=0;i<8;++i) vertices[i]=dmVecSet((i&1)?1.f:-1.f,(i&2)?1.f:-1.f,(i&4)?1.f:-1.f,0.f);
    dm_vec4 planes[6]={dmVecSet(1,0,0,1),dmVecSet(-1,0,0,1),dmVecSet(0,1,0,1),
        dmVecSet(0,-1,0,1),dmVecSet(0,0,1,1),dmVecSet(0,0,-1,1)};
    Box box(vertices,planes);
    dmSphere sphere; sphere.Create(dmVecSet(0,0,0,0),.5f);
    dmTransform a=dmTransformIdentity(), b=dmTransformIdentity();
    a.q=dmVecSet(0,0,.707106781f,.707106781f); a.p=dmVecSet(3,-2,1,0);
    b.p=dmVecSet(3,-.75f,1,0);
    dmGJKCache cache; cache.Clear(); dmSATCache sat; sat.Clear();
    dmManifold old, m; dmManifold_Create(&old); dmManifold_Create(&m);
    old.m_tangentImpulse=dmVecSet(2,3,4,0); old.m_twistImpulse=7;
    assert(dmConvexContact::ComputeManifold(old,&m,&cache,&sat,&box,&sphere,a,b,
        dmShape::e_polytope,dmShape::e_sphere,false));
    vec(m.m_normal,0,1,0); vec(m.m_points[0].point,3,-1.35f,1);
    assert(m.m_points[0].isNew); near(m.m_points[0].normalImpulse,0);
    vec(m.m_tangentImpulse,2,3,4); near(m.m_twistImpulse,7);
    old=m; old.m_points[0].normalImpulse=9;
    assert(dmConvexContact::ComputeManifold(old,&m,&cache,&sat,&box,&sphere,a,b,
        dmShape::e_polytope,dmShape::e_sphere,false));
    assert(!m.m_points[0].isNew); near(m.m_points[0].normalImpulse,9);
    old.m_points[0].id.key=123;
    assert(dmConvexContact::ComputeManifold(old,&m,&cache,&sat,&box,&sphere,a,b,
        dmShape::e_polytope,dmShape::e_sphere,false));
    assert(m.m_points[0].isNew); near(m.m_points[0].normalImpulse,0);
    dmCapsule capsule; capsule.Create(dmVecSet(-1,0,0,0),dmVecSet(1,0,0,0),.5f);
    a=dmTransformIdentity(); b=dmTransformIdentity(); b.p=dmVecSet(0,.75f,0,0);
    assert(dmConvexContact::ComputeManifold(old,&m,&cache,&sat,&capsule,&sphere,a,b,
        dmShape::e_capsule,dmShape::e_sphere,false));
    vec(m.m_normal,0,1,0); near(m.m_points[0].separation,-.25f);
    b.p=dmVecSet(0,4,0,0);
    assert(!dmConvexContact::ComputeManifold(old,&m,&cache,&sat,&capsule,&sphere,a,b,
        dmShape::e_capsule,dmShape::e_sphere,false));
}
int main() { convexDispatch(); polytopeSphere(); triangleSphere(); capsules(); std::puts("Domino manifold geometry checks passed"); }
