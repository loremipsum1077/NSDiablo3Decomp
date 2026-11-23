#include "Buoyancy.h"

#include "DominoBody.h"
#include "DominoClient.h"

// Defined in X/Client/Physics/DominoClient.cpp.
extern DominoWorld * gptPhysicsAPIImpl;

// Body reconstructed from the call inlined into GetWaterPlane: whatever the
// original looked up from the scene, the compiled code only loads cgwvNull and
// stores it, so the two parameters are unused here.
static Vector3D sGetWaterVelocity(CSceneID idCScene, const Vector3D & wp)
{
    (void)idCScene;
    (void)wp;
    return cgwvNull;
}

BOOL GetWaterPlane(WaterPlane * ptWaterPlane, const Vector3D & wp)
{
    RayCastInput tInput;
    RayCastOutput tOutput;

    tInput.wpFrom = wp;
    tInput.wpFrom.z += 20.0f;
    tInput.wpTo = wp;
    tInput.wpTo.z -= 10.0f;
    tInput.uIncludeMask = 0x40;
    tInput.uExcludeMask = 0;

    BOOL bHit = gptPhysicsAPIImpl->RayCast(&tOutput, &tInput);
    if (!bHit)
    {
        return FALSE;
    }

    ptWaterPlane->tPlane.wvNormal = tOutput.wvNormal;
    ptWaterPlane->tPlane.wdD = -Vector3D::DotProduct(tOutput.wvNormal, tOutput.wpPoint);
    ptWaterPlane->wvVelocity = sGetWaterVelocity(tOutput.pRigidBody->m_idCScene, wp);
    return TRUE;
}

void ApplyBuoyancy(RigidBody * pRigidBody, const WaterPlane & tWaterPlane)
{
    float flLinearDrag;
    float flAngularDrag;
    Vector3D wpCentroid;
    float flSubmergedVolume;

    if (!pRigidBody->IsSleeping() &&
        pRigidBody->GetType() == RigidBody::DYNAMIC &&
        (flSubmergedVolume = pRigidBody->ComputeSubmergedVolume(&wpCentroid, tWaterPlane.tPlane)) > 0.0f &&
        pRigidBody->m_volume > 0.0f)
    {
        float flFudgeFactor = 1.5f;
        float flMass = pRigidBody->GetMass();
        float flRatio = flSubmergedVolume / pRigidBody->m_volume;
        float flBuoyancyForce = (flRatio * flFudgeFactor) * (flMass * 32.0f);

        Vector3D wvBuoyancyForce = flBuoyancyForce * tWaterPlane.tPlane.wvNormal;
        Vector3D wvVelocity = tWaterPlane.wvVelocity;
        float flPartialMass = flMass * flRatio;
        Vector3D vc = pRigidBody->GetPointVelocity(wpCentroid);

        flLinearDrag = 2.0f * flPartialMass;
        Vector3D wvDragForce = flLinearDrag * (wvVelocity - vc);
        Vector3D wvTotalForce = wvBuoyancyForce + wvDragForce;
        pRigidBody->ApplyForce(wvTotalForce, wpCentroid);

        flAngularDrag = flPartialMass * -0.05f * pRigidBody->m_area;
        Vector3D wvDragTorque = flAngularDrag * pRigidBody->GetAngularVelocity();
        pRigidBody->ApplyTorque(wvDragTorque);
    }
}
