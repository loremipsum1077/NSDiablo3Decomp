#include "../../Include/dmShapes.h"
#include "../../Include/dmCollision.h"
#include "../../Include/dmSettings.h"
#include "../Functions/dmDistance.h"













void dmSphere::Create(dm_vec4Arg p, dm_float32 radius)
{
    m_p = p;
    m_radius = radius;
}


dmAABB dmSphere::GetAABB(const dmMtx & xf) const
{
    dm_vec4 r = dmV4(m_radius);
    dm_vec4 p = dmMul34(xf, m_p);
    dmAABB aabb;
    aabb.m_lowerBound = p - r;
    aabb.m_upperBound = p + r;
    return aabb;
}


dmAABB dmSphere::GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const
{
    dm_vec4 r = dmV4(m_radius);
    dm_vec4 p1 = dmMul34(xf1, m_p);
    dm_vec4 p2 = dmMul34(xf2, m_p);
    dmAABB aabb;
    aabb.m_lowerBound = dmMin(p1, p2) - r;
    aabb.m_upperBound = dmMax(p1, p2) + r;
    return aabb;
}



































bool dmSphere::RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput) const
{
    dm_vec4 s = pInput->p1 - m_p;
    dm_vec4 d = pInput->p2 - pInput->p1;


    if (dmIsZeroSafe3(d))
    {
        return false;
    }


    dm_vec4 radius = pInput->slop + dmV4(m_radius);
    dm_vec4 rr = radius * radius;
    if (dmLT(dmDot3(s, s), rr))
    {
        return false;
    }

    dm_vec4 u = dmNormalize3(d);
    dm_vec4 length = dmLength3(d);




    dm_vec4 c = -dmDot3(s, u);
    dm_vec4 cc = s + c * u;


    dm_vec4 alpha = dmDot3(cc, cc);


    if (dmGT(alpha, rr))
    {
        return false;
    }



    dm_vec4 beta = dmSqrt(rr - alpha);
    dm_vec4 lambda = c - beta;


    if (dmLTZ(lambda) || dmLT(length * pInput->maxLambda, lambda))
    {
        return false;
    }

    dm_vec4 normal = s + lambda * u;
    if (dmIsZeroSafe3(normal) == false)
    {
        normal = dmNormalize3(normal);
    }
    else
    {


        normal = -u;
    }

    pOutput->normal = normal;
    pOutput->lambda = lambda * dmSafeReciprocal(length);

    return true;
}


bool dmSphere::RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & transform) const
{
    dmRayCastInput input;
    input.p1 = dmMulT(transform, pInput->p1);
    input.p2 = dmMulT(transform, pInput->p2);
    input.maxLambda = pInput->maxLambda;
    input.slop = pInput->slop;

    bool hit = RayCast(pOutput, &input);
    if (hit)
    {
        pOutput->normal = dmRotate(transform.q, pOutput->normal);
    }
    return hit;
}


void dmSphere::ComputeMass(dmMassData * pMassData, dm_float32 density) const
{
    // 4/3 pi folds to 0x40860a92 (a 4.18879f literal would round to ...a91).
    pMassData->mass = (4.0f / 3.0f) * 3.14159265f * density * m_radius * m_radius * m_radius;
    pMassData->center = m_p;


    dm_float32 inertia = 0.4f * pMassData->mass * m_radius * m_radius;
    pMassData->I = inertia * dmMtxIdentity();



    pMassData->I = dmShiftInertiaToOrigin(pMassData->I, pMassData->center, pMassData->mass);
}


dm_vec4 dmSphere::ComputeSurfaceArea() const
{

    dm_vec4 r = dmV4(m_radius);
    dm_vec4 area = dm_vec4_two * dm_vec4_2pi * r * r;
    return area;
}


dm_vec4 dmSphere::ComputeVolume() const
{

    dm_vec4 r = dmV4(m_radius);
    return dm_vec4_oneThird * dm_vec4_two * dm_vec4_2pi * r * r * r;
}


dm_vec4 dmSphere::ComputeProjectedSurfaceArea(dm_vec4Arg dir) const
{



    dm_vec4 r = dmV4(m_radius);
    dm_vec4 area = dm_vec4_pi * r * r;
    return area;
}


dm_vec4 dmSphere::ComputeSubmergedVolume(dm_vec4 * pCentroid, const dmTransform & transform, dm_vec4Arg normal, dm_vec4Arg offset) const
{
    dm_vec4 c = dmMul(transform, m_p);
    dm_vec4 height = dmDot3(normal, c) - offset;
    dm_vec4 radius = dmV4(m_radius);

    if (dmLT(height, radius))
    {
        dm_vec4 h = dmMin(radius * dm_vec4_two, radius - height);
        dm_vec4 a = radius * dm_vec4_three - h;
        dm_vec4 b = radius * dm_vec4_two - h;

        dm_vec4 volume = dm_vec4_oneThird * dm_vec4_pi * h * h * a;

        *pCentroid = c - normal * (dmV4(0.75f) * b * b / a);
        return volume;
    }

    *pCentroid = dmV4(0.0f);
    return dmV4(0.0f);
}


bool dmSphere::TestOverlap(const dmSphere & sphere) const
{
    dm_vec4 d = m_p - sphere.m_p;
    dm_float32 rr = m_radius + sphere.m_radius;
    if (dmLengthSqr3_f32(d) < rr * rr)
    {
        return true;
    }

    return false;
}


void dmSphere::ApplyScale(dm_vec4Arg scale)
{
    dm_vec4 s = dm_vec4_oneThird * (dmSplatX(scale) + dmSplatY(scale) + dmSplatZ(scale));
    dm_vec4 radius = dmV4(m_radius);
    m_p = scale * m_p;
    m_radius = dmF32(radius * s);
}


void dmCapsule::Create(dm_vec4Arg p1, dm_vec4Arg p2, dm_float32 radius)
{
    m_p1 = p1;
    m_p2 = p2;
    m_radius = radius;
}




































































dmAABB dmCapsule::GetAABB(const dmMtx & transform) const
{
    dm_vec4 p1 = dmMul34(transform, m_p1);
    dm_vec4 p2 = dmMul34(transform, m_p2);

    dm_vec4 lower = dmMin(p1, p2);
    dm_vec4 upper = dmMax(p1, p2);
    dm_vec4 radius = dmV4(m_radius);

    dmAABB aabb;
    aabb.m_lowerBound = lower - radius;
    aabb.m_upperBound = upper + radius;
    return aabb;
}


dmAABB dmCapsule::GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const
{
    dm_vec4 p11 = dmMul34(xf1, m_p1);
    dm_vec4 p12 = dmMul34(xf1, m_p2);

    dm_vec4 p21 = dmMul34(xf2, m_p1);
    dm_vec4 p22 = dmMul34(xf2, m_p2);

    dm_vec4 lower = dmMin(dmMin(p11, p12), dmMin(p21, p22));
    dm_vec4 upper = dmMax(dmMax(p11, p12), dmMax(p21, p22));
    dm_vec4 radius = dmV4(m_radius);

    dmAABB aabb;
    aabb.m_lowerBound = lower - radius;
    aabb.m_upperBound = upper + radius;
    return aabb;
}



















































































































































































































































dm_vec4 dmCapsule::ComputeCentroid() const
{
    return dm_vec4_oneHalf * (m_p1 + m_p2);
}


dm_vec4 dmCapsule::ComputeSurfaceArea() const
{
    dm_vec4 radius = dmV4(m_radius);

    // Two hemispheres
    dm_vec4 sphereArea = dm_vec4_two * dm_vec4_2pi * radius * radius;

    // Cylinder side
    dm_vec4 length = dmLength3(m_p2 - m_p1);
    dm_vec4 cylinderArea = dm_vec4_2pi * radius * length;

    return sphereArea + cylinderArea;
}


dm_vec4 dmCapsule::ComputeVolume() const
{
    dm_vec4 radius = dmV4(m_radius);

    dm_vec4 sphereVolume = dm_vec4_two * dm_vec4_2pi * dm_vec4_oneThird * radius * radius * radius;

    dm_vec4 length = dmLength3(m_p2 - m_p1);
    dm_vec4 cylinderVolume = dm_vec4_pi * radius * radius * length;

    return sphereVolume + cylinderVolume;
}


dm_vec4 dmCapsule::ComputeProjectedSurfaceArea(dm_vec4Arg dir) const
{
    dm_vec4 radius = dmV4(m_radius);

    // Two hemispheres project to one disk
    dm_vec4 spherearea = dm_vec4_pi * radius * radius;

    // Cylinder side

    dm_vec4 d = m_p2 - m_p1;



    dm_vec4 sinMagV = dmCross(d, dir);
    dm_vec4 sinMag = dmLength3(sinMagV);
    dm_vec4 cylinderArea = sinMag * radius;
    dm_vec4 area = spherearea + cylinderArea;
    return area;
}


































bool dmCapsule::TestOverlap(const dmSphere & sphere) const
{
    dm_vec4 p = sphere.m_p;
    dm_vec4 d = m_p2 - m_p1;
    dm_vec4 dd = dmDot3(d, d);

    // Project the sphere center onto the segment
    dm_vec4 alpha = dmDot3(p - m_p1, d);

    dm_vec4 closestPoint;

    if (dmLEZ(alpha) || dmIsZeroSafe3(d))
    {
        closestPoint = m_p1;
    }

    // Beyond the second end point


    else if (dmGE(alpha, dd))
    {
        closestPoint = m_p2;
    }

    // Interior of the segment
    else
    {

        alpha = alpha / dd;
        closestPoint = m_p1 + alpha * d;
    }


    dm_vec4 v = p - closestPoint;
    dm_vec4 rr = dmV4(m_radius + sphere.m_radius);
    if (dmLT(dmLengthSqr3(v), rr * rr))
    {
        return true;
    }

    return false;
}


void dmCapsule::ApplyScale(dm_vec4Arg scale)
{
    dm_vec4 s = dm_vec4_oneThird * (dmSplatX(scale) + dmSplatY(scale) + dmSplatZ(scale));
    m_p1 = scale * m_p1;
    m_p2 = scale * m_p2;
    m_radius = dmF32(s) * m_radius;
}
























































bool dmTriangle::RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & transform) const
{
    dm_vec4 p1 = dmMulT(transform, pInput->p1);
    dm_vec4 p2 = dmMulT(transform, pInput->p2);
    bool hit = RayCast(pOutput, p1, p2, pInput->maxLambda);
    if (hit)
    {
        pOutput->normal = dmRotate(transform.q, pOutput->normal);
    }
    return hit;
}


bool dmTriangle::TestOverlap(const dmSphere & sphere) const
{
    dmDistanceInput input;
    input.cache.Clear();
    input.proxyA.Set(m_p, 3, dmVecSet(0.0f));
    input.proxyB.Set(&sphere.m_p, 1, dmV4(sphere.m_radius));
    input.useRadii = true;
    input.transformA = dmTransformIdentity();
    input.transformB = dmTransformIdentity();

    dmDistanceOutput output;
    dmDistance(&output, &input);

    if (dmEQZ(output.distance))
    {
        return true;
    }

    return false;
}

// Original dmMath.inl:3405; DW_INL_inlined only, kept owner-local without inline.
#line 3405 "../../..\\Domino/Include/dmMath.inl"
inline dm_vec4 dmQuatFromUnitVectors(dm_vec4Arg a, dm_vec4Arg b)  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    dm_vec4 m = dmMul(dm_vec4_oneHalf, dmAdd(a, b));
    dm_vec4 axis = dmCross(a, m);


    dm_vec4 w = dmDot3(a, m);
    dm_vec4 q = dmSetW(axis, w);

    if (dmIsZeroSafe4(q))
    {
        return dmNormalize4(q);
    }



    dm_vec4 ax = dmSplatX(a);
    if (dmGT(dmAbs(ax), dm_vec4_oneHalf))
    {
        dm_vec4 ay = dmSplatY(a);
        dm_vec4 qx = dmMul(ay, dm_vec4_unitX);
        dm_vec4 qy = dmMul(dmNegate(ax), dm_vec4_unitY);
        q = dmAdd(qx, qy);
    }
    else
    {
        dm_vec4 ay = dmSplatY(a);
        dm_vec4 az = dmSplatZ(a);
        dm_vec4 qy = dmMul(az, dm_vec4_unitY);
        dm_vec4 qz = dmMul(dmNegate(ay), dm_vec4_unitZ);
        q = dmAdd(qy, qz);
    }

    return dmNormalize4(q);
}
#define dm_epsilon 1.1920928955078125e-7f
#line 552 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmShapes.cpp"
// NON_MATCHING: equal size; scalar scheduling near lines 575-583 and the assertion trap encoding differ.
void dmCapsule::ComputeMass(dmMassData* pMassData, dm_float32 density) const
{
    dm_vec4 d = m_p2 - m_p1;
    dm_float32 length = dmF32(dmLength3(d));

    dmAssert(length > dm_epsilon);


    dm_float32 rr = m_radius * m_radius;
    dm_float32 rrr = m_radius * rr;
    dm_float32 ll = length * length;

    dm_float32 cylinderVolume = length * (rr * 3.14159265358979323846f);
    dm_float32 hemisphereVolume = (2.0f / 3.0f) * rrr;

    dm_float32 cylinderMass = density * cylinderVolume;
    dm_float32 hemisphereMass = density * hemisphereVolume;
    pMassData->mass = cylinderMass + 2.0f * hemisphereMass;

    dm_float32 hemisphereIxx = rr * (hemisphereMass * (83.0f / 320.0f));
    dm_float32 hemisphereIyy = rr * (hemisphereMass * (2.0f / 5.0f));

    dm_float32 hemisphereCenterY = 0.5f * length + (3.0f / 8.0f) * m_radius;

    dm_float32 cylinderIxx = (ll * (1.0f / 12.0f) + rr * 0.25f) * cylinderMass;
    dm_float32 cylinderIyy = rr * (cylinderMass * 0.5f);


    pMassData->center = (m_p1 + m_p2) * dmVecSet(0.5f);

    dm_float32 Ixx = cylinderIxx + 2.0f * (hemisphereIxx + hemisphereMass * hemisphereCenterY * hemisphereCenterY);
    dm_float32 Iyy = cylinderIyy + 2.0f * hemisphereIyy;

    dmMtx I;

    I.rx = dmVecSet(Ixx, 0.0f, 0.0f);
    I.ry = dmVecSet(0.0f, Iyy, 0.0f);
    I.rz = dmVecSet(0.0f, 0.0f, Ixx);

    dm_vec4 yAxis = dmNormalize3(d);
    dm_vec4 q = dmQuatFromUnitVectors(dmVecSet(0.0f, 1.0f, 0.0f), yAxis);
    dmMtx R = dmMtxFromQuat(q);

    pMassData->I = dmMulRART(R, I);



    pMassData->I = dmShiftInertiaToOrigin(pMassData->I, pMassData->center, pMassData->mass);
}
#undef dm_epsilon
dm_vec4 dmCapsule::ComputeSubmergedVolume(dm_vec4* pCentroid, const dmTransform& transform, dm_vec4Arg normal, dm_vec4Arg offset) const
{
    dmSphere s1;
    s1.m_p = m_p1;
    s1.m_radius = m_radius;
    dm_vec4 c1;
    dm_vec4 v1 = s1.ComputeSubmergedVolume(&c1, transform, normal, offset);
    dmSphere s2;
    s2.m_p = m_p2;
    s2.m_radius = m_radius;
    dm_vec4 c2;
    dm_vec4 v2 = s2.ComputeSubmergedVolume(&c2, transform, normal, offset);
    dm_vec4 volume = v1 + v2;
    if (dmGTZ(volume))
    {
        *pCentroid = (v1 * c1 + v2 * c2) / volume;
        dm_vec4 correctedVolume = (volume * dm_vec4_oneHalf) / s1.ComputeVolume();
        return correctedVolume * ComputeVolume();
    }
    *pCentroid = dmMul(transform, (m_p1 + m_p2) * dm_vec4_oneHalf);
    return dmV4(0.0f);
}
#line 745 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmShapes.cpp"
bool dmTriangle::RayCast(dmRayCastOutput* pOutput, dm_vec4Arg p1, dm_vec4Arg p2, dm_vec4Arg maxLambda) const
{
    dm_vec4 p = p1;
    dm_vec4 q = p2;

    dm_vec4 a = m_p[0];
    dm_vec4 b = m_p[1];
    dm_vec4 c = m_p[2];

    dm_vec4 pq = q - p;
    dm_vec4 pa = a - p;
    dm_vec4 pb = b - p;
    dm_vec4 pc = c - p;




    dm_vec4 u = dmScalarTripleProduct(pq, pc, pb);
    dm_vec4 v = dmScalarTripleProduct(pq, pa, pc);
    dm_vec4 w = dmScalarTripleProduct(pq, pb, pa);
    dm_vec4 m = dmMin(dmMin(u, v), w);
    if (dmLTZ(m))
    {
        return false;
    }

    dm_vec4 b1 = b - a;
    dm_vec4 b2 = c - a;
    dm_vec4 d = pa;
    dm_vec4 n = dmCross(b1, b2);
    dm_vec4 delta = -dmDot3(pq, n);


    if (dmLT(delta, dm_vec4_epsilon))
    {
        return false;
    }

    dm_vec4 lambda = -dmDot3(d, n);


    if (dmLE(dmV4(0.0f), lambda) && dmLE(lambda, maxLambda * delta))
    {
        pOutput->lambda = lambda / delta;

        pOutput->normal = dmNormalize3(n);
        return true;
    }
    return false;
}
#line 365 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmShapes.cpp"
bool dmCapsule::RayCast(dmRayCastOutput* pOutput, const dmRayCastInput* pInput, const dmTransform& transform) const
{

    dm_vec4 d = m_p2 - m_p1;


    if (dmIsZeroSafe3(d))
    {
        dmSphere sphere;
        sphere.m_p = dm_vec4_oneHalf * (m_p1 + m_p2);
        sphere.m_radius = m_radius;
        return sphere.RayCast(pOutput, pInput, transform);
    }


    dm_vec4 s = dmMulT(transform, pInput->p1) - m_p1;


    dm_vec4 e = dmInvRotate(transform.q, pInput->p2 - pInput->p1);


    if (dmIsZeroSafe3(e))
    {
        return false;
    }


    dm_vec4 u = dmNormalize3(d);
    dm_vec4 capsuleLength = dmLength3(d);


    dm_vec4 v = dmNormalize3(e);
    dm_vec4 rayLength = dmLength3(e);


    dm_vec4 su = dmDot3(s, u);
    dm_vec4 fu = dmDot3(s + pInput->maxLambda * e, u);


    dm_vec4 radius = pInput->slop + dmV4(m_radius);


    if (dmLT(su, -radius) && dmLT(fu, -radius))
    {
        return false;
    }


    if (dmGT(su, capsuleLength + radius) && dmGT(fu, capsuleLength + radius))
    {
        return false;
    }


    dm_vec4 cs = s - u * su;


    dm_vec4 rr = radius * radius;
    if (dmLT(dmDot3(cs, cs), rr))
    {

        dm_vec4 a1 = dmClamp(su, dmV4(0.0f), capsuleLength);
        dm_vec4 closestPoint = u * a1;


        dm_vec4 p = s - closestPoint;
        if (dmLT(dmDot3(p, p), rr)) return false;







        dmSphere sphere;
        sphere.m_p = m_p1 + closestPoint;
        sphere.m_radius = m_radius;
        return sphere.RayCast(pOutput, pInput, transform);
    }





















    dm_vec4 uv = dmDot3(u, v);
    dm_vec4 det = dm_vec4_one - uv * uv;


    if (dmLT(det, dm_vec4_epsilon))
    {
        return false;
    }



    dm_vec4 sv = dmDot3(s, v);
    dm_vec4 invDet = dmSafeReciprocal(det);
    dm_vec4 a1 = invDet * (su - uv * sv);
    dm_vec4 a2 = invDet * (su * uv - sv);

    dm_vec4 c1 = u * a1;
    dm_vec4 c2 = s + v * a2;


    dm_vec4 h = c2 - c1;
    dm_vec4 hh = dmDot3(h, h);


    if (dmGT(hh, rr))
    {
        return false;
    }




    dm_vec4 beta = dmSqrt(invDet * (rr - hh));


    dm_vec4 t = a2 - beta;


    dm_vec4 pu = su + uv * t;


    if (dmLTZ(pu))
    {

        dmSphere sphere;
        sphere.m_p = m_p1;
        sphere.m_radius = m_radius;
        return sphere.RayCast(pOutput, pInput, transform);
    }


    if (dmGT(pu, capsuleLength))
    {

        dmSphere sphere;
        sphere.m_p = m_p2;
        sphere.m_radius = m_radius;
        return sphere.RayCast(pOutput, pInput, transform);
    }


    if (dmLTZ(t) || dmLT(rayLength * pInput->maxLambda, t))
    {
        return false;
    }



    dm_vec4 p = s + v * t;


    dm_vec4 normal = p - u * pu;
    if (!dmIsZeroSafe3(normal))
    {
        normal = dmNormalize3(normal);
    }
    else
    {
        normal = -v;
    }

    pOutput->lambda = t / rayLength;
    pOutput->normal = dmRotate(transform.q, normal);

    return true;
}
