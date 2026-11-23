#include "DominoBody.h"

#include "../../../Contrib/Contrib/Domino/Include/dmApi.h"
#include "../../../Contrib/Contrib/Domino/Include/dmCollision.h"
#include "../../../Contrib/Contrib/Domino/Include/dmMath.h"
#include "../../Sigma/Math/Plane.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Math/Vector.h"
#include "../Explosions.h"
#include "DominoClient.h"













static inline Vector3D sConvertVec3(dm_vec4Arg v)
{
    return Vector3D(v[0], v[1], v[2]);
}



static inline dm_vec4 sConvert(const Vector3D & v)
{
    return dmVecSet(v.x, v.y, v.z);
}










static inline dm_vec4 sConvert(const Quaternion & q)
{
    return dmVecSet(q.wvImag.x, q.wvImag.y, q.wvImag.z, q.wdReal);
}



RigidBody::Type RigidBody::GetType() const
{
    switch (dmBody_GetType(m_bodyId))
    {
    case dm_dynamicBody:
        return DYNAMIC;
    case dm_kinematicBody:
        return KINEMATIC;
    case dm_staticBody:
        return STATIC;
    default:
        return NONE;
    }
}






void RigidBody::SetType(RigidBody::Type eType)
{
    switch (eType)
    {
    case STATIC:
        dmBody_SetType(m_bodyId, dm_staticBody);
        break;
    case KINEMATIC:
        dmBody_SetType(m_bodyId, dm_kinematicBody);
        break;
    case DYNAMIC:
        dmBody_SetType(m_bodyId, dm_dynamicBody);
        break;
    default:
        break;
    }
}






void RigidBody::SetTransform(const PRTransform & transform)
{
    dmTransform xf;
    xf.q = sConvert(transform.q);
    xf.p = sConvert(transform.wp);
    dmBody_SetTransform(m_bodyId, xf);
}



void RigidBody::SetTransformNoSync(const PRTransform & transform)
{
    dmTransform xf;
    xf.q = sConvert(transform.q);
    xf.p = sConvert(transform.wp);
    dmBody_SetTransformNoSync(m_bodyId, xf);
}



void RigidBody::GetTransform(PRTransform & transform) const
{
    dm_vec4 q = dmBody_GetQuaternion(m_bodyId);
    dm_vec4 p = dmBody_GetPosition(m_bodyId);
    transform.q = Quaternion(Vector3D(q[0], q[1], q[2]), q[3]);
    transform.wp = sConvertVec3(p);
}



void RigidBody::SetPosition(const Vector3D & position)
{
    dm_vec4 p = sConvert(position);
    dmBody_SetPosition(m_bodyId, p);
}



Vector3D RigidBody::GetPosition() const
{
    dm_vec4 p = dmBody_GetPosition(m_bodyId);
    return sConvertVec3(p);
}



Vector3D RigidBody::GetCenterOfMassPosition() const
{
    dm_vec4 c = dmBody_GetCenter(m_bodyId);
    return sConvertVec3(c);
}



void RigidBody::StabilizeInertias(float flInertiaFactor, float flMassRatio)
{
    dmBody_StabilizeTree(m_bodyId, flInertiaFactor, flMassRatio);
}




float RigidBody::GetMass() const
{
    return dmBody_GetMass(m_bodyId);
}





















Vector3D RigidBody::GetCenterOfMassLocal() const
{
    dm_vec4 localCenter = dmBody_GetLocalCenter(m_bodyId);
    return sConvertVec3(localCenter);
}



BOOL RigidBody::IsSleeping() const
{
    return dmBody_IsSleeping(m_bodyId);
}



void RigidBody::SetSleeping(BOOL bFlag)
{
    dmBody_SetSleeping(m_bodyId, bFlag == TRUE);
}



void RigidBody::SetCenterOfMassVelocity(const Vector3D & wvVelocity)
{
    dm_vec4 linearVelocity = sConvert(wvVelocity);
    dmBody_SetLinearVelocity(m_bodyId, linearVelocity);
}



void RigidBody::SetAngularVelocity(const Vector3D & wvAngularVelocity)
{
    dm_vec4 angularVelocity = sConvert(wvAngularVelocity);
    dmBody_SetAngularVelocity(m_bodyId, angularVelocity);
}



Vector3D RigidBody::GetCenterOfMassVelocity() const
{
    dm_vec4 linearVelocity = dmBody_GetLinearVelocity(m_bodyId);
    return sConvertVec3(linearVelocity);
}



Vector3D RigidBody::GetAngularVelocity() const
{
    dm_vec4 angularVelocity = dmBody_GetAngularVelocity(m_bodyId);
    return sConvertVec3(angularVelocity);
}



Vector3D RigidBody::GetPointVelocity(const Vector3D & wp) const
{
    dm_vec4 worldPoint = sConvert(wp);
    dm_vec4 linearVelocity = dmBody_GetWorldPointLinearVelocity(m_bodyId, worldPoint);
    return sConvertVec3(linearVelocity);
}



void RigidBody::ApplyForce(const Vector3D & wvForce)
{
    dm_vec4 force = sConvert(wvForce);
    dm_vec4 point = dmBody_GetCenter(m_bodyId);
    dmBody_ApplyForce(m_bodyId, force, point, dm_leaveAsleep);
}



void RigidBody::ApplyForce(const Vector3D & wvForce, const Vector3D & wp)
{
    dm_vec4 force = sConvert(wvForce);
    dm_vec4 point = sConvert(wp);
    dmBody_ApplyForce(m_bodyId, force, point, dm_leaveAsleep);
}



void RigidBody::ApplyTorque(const Vector3D & wvTorque)
{
    dm_vec4 torque = sConvert(wvTorque);
    dmBody_ApplyTorque(m_bodyId, torque, dm_leaveAsleep);
}



void RigidBody::ApplyImpulse(const Vector3D & wvImpulse)
{
    dm_vec4 impulse = sConvert(wvImpulse);
    dm_vec4 point = dmBody_GetCenter(m_bodyId);
    dmBody_ApplyImpulse(m_bodyId, impulse, point, dm_leaveAsleep);
}












void RigidBody::ApplyWind(const Vector3D & wvWind)
{
    if (dmBody_GetType(m_bodyId) != dm_dynamicBody)
    {
        return;
    }

    dm_vec4 wind = sConvert(wvWind);
    if (dmIsFinite3(wind) == false || dmIsZeroSafe3(wind))
    {
        return;
    }

    dm_vec4 direction = dmNormalize3(wind);
    dm_vec4 area = dmBody_ComputeProjectedSurfaceArea(m_bodyId, direction);

    dm_vec4 impulse = dmMul(wind, dmMul(area, dm_vec4_oneHalf));
    dm_vec4 center = dmBody_GetCenter(m_bodyId);
    dmBody_ApplyImpulse(m_bodyId, impulse, center, dm_wakeIfChanged);
}



float RigidBody::ComputeSubmergedVolume(Vector3D * pCentroid, const Plane & plane) const
{
    dm_vec4 centroid;
    dm_vec4 normal = sConvert(plane.wvNormal);
    dm_vec4 offset = dmVecSet(-plane.wdD);
    dm_vec4 volume = dmBody_ComputeSubmergedVolume(m_bodyId, &centroid, normal, offset);
    *pCentroid = sConvertVec3(centroid);
    return volume[0];
}





void RigidBody::ComputeBoundingRadius()
{
    m_boundingRadius = 0.0f;

    dm_vec4 center = dmBody_GetCenter(m_bodyId);
    dmAABB aabb;
    aabb.m_lowerBound = center;
    aabb.m_upperBound = center;

    for (dmFixtureId id = dmBody_GetFirstFixture(m_bodyId); id != dm_nullFixtureId; id = dmBody_GetNextFixture(id))
    {
        dmAABB b = dmFixture_GetAABB(id);
        aabb.Add(b);
    }

    dm_vec4 lower = dmSub(aabb.m_lowerBound, center);
    dm_vec4 upper = dmSub(aabb.m_upperBound, center);
    dm_vec4 radiusSqr = dmMax(dmLengthSqr3(lower), dmLengthSqr3(upper));
    m_boundingRadius = dmF32(dmSqrt(radiusSqr));
}




void RigidBody::ApplyExplosion(const ExplosionImpulse & tExplosion, float flFactor, float flMaxDeltaVelocity)
{
    const float cflFudgeFactor = 0.3f;



    float maxRadius = tExplosion.m_flMinRadius + tExplosion.m_flDeltaRadius;
    dm_vec4 center = sConvert(tExplosion.m_wpCenter);
    dm_vec4 radius = dmVecSet(maxRadius);

    dmAABB explosionAABB;
    explosionAABB.m_lowerBound = dmSub(center, radius);
    explosionAABB.m_upperBound = dmAdd(center, radius);

    for (dmFixtureId id = dmBody_GetFirstFixture(m_bodyId); id != dm_nullFixtureId; id = dmBody_GetNextFixture(id))
    {
        dmFixtureType fixtureType = dmFixture_GetType(id);
        if (fixtureType == dm_treeMeshFixture || fixtureType == dm_heightFieldFixture)
        {
            continue;
        }

        dmAABB shapeAABB = dmFixture_GetAABB(id);

        if (dmTestOverlap(explosionAABB, shapeAABB) == false)
        {
            continue;
        }

        dm_vec4 closestPoint = dmFixture_GetClosestPoint(id, center);

        dm_vec4 d = dmSub(closestPoint, center);
        float distanceSqr = dmLengthSqr3_f32(d);
        if (distanceSqr > maxRadius * maxRadius)
        {
            continue;
        }


        Vector3D wpClosest = sConvertVec3(closestPoint);
        Vector3D wvImpulse = tExplosion.ComputeImpulse(wpClosest) * 60.0f;
        ShapeUserData * pUserData = (ShapeUserData *)dmFixture_GetUserData(id);
        float totalFactor = pUserData->flExplosionFactor * flFactor * m_slowExplosionFactor * cflFudgeFactor;

        if (tExplosion.m_eType == EXPLOSION_TYPE_RADIAL)
        {
            float area;


            if (dmIsZeroSafe3(d))
            {
                d = dm_vec4_unitZ;
            }
            else
            {
                d = dmNormalize3(d);
            }
            area = dmF32(dmFixture_GetProjectedArea(id, d));

            wvImpulse *= totalFactor * area;
            dmBody_ApplyImpulse(m_bodyId, sConvert(wvImpulse), closestPoint, dm_wakeIfChanged);
        }
        else
        {
            dm_vec4 direction = sConvert(wvImpulse);
            if (dmIsZeroSafe3(direction) == false)
            {
                direction = dmNormalize3(direction);
                float area = dmF32(dmFixture_GetProjectedArea(id, direction));
                wvImpulse *= totalFactor * area;

                dmBody_ApplyImpulse(m_bodyId, sConvert(wvImpulse), closestPoint, dm_wakeIfChanged);
            }
        }
    }
}


void RigidBody::ApplyImpulseToClosestPoint(const Vector3D & wvImpulse, const Vector3D & wpTarget)
{
    dm_vec4 target = sConvert(wpTarget);
    dm_vec4 impulse = sConvert(wvImpulse);
    dm_vec4 closestPoint = dmBody_GetClosestPoint(m_bodyId, target);
    dmBody_ApplyImpulse(m_bodyId, impulse, closestPoint, dm_wakeIfChanged);
}
