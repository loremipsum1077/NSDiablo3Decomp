// X/Client/ParticleUpdate.cpp — recovered against the 2.6.2 DWARF and original instructions.
#include "ParticleInternal.h"
#include "ParticleUpdate.h"
#include "Definitions/ParticleDefinition.h"
#include "Attachments.h"
#include "CGame.h"
#include "../Common/Definitions/AttachmentDefinition.h"
// DWARF X/Client/Attachments.cpp:1602.
BOOL MsgTriggeredEventShouldTrigger(const MsgTriggeredEvent & tEvent, const KeyedMessage & tMsg);
// DWARF CWorld.cpp:1084, Scene.cpp:4553, and ParticleCommon.cpp:6.
WorldPlace ClientPointToPlace(const Vector3D & wp);
BOOL CScenesGetFloorHeight(SWorldID idSWorld, const Vector3D & wp, float & fFloorHeight);
void ParticleGetInterpTime(const ParticleSystem * ptParticleSystem, const ParticleParams & tParams, const Vector3D & wpParticle, InterpTime & tInterpTime);



















void ParticleAttachmentsOnMsg(
    ParticleSystem & tParticleSystem,
    const DataID idAttachedTo,
    const KeyedMessage & tMsg)
{
    const ParticleDefinition * pdefParticle = tParticleSystem.GetParticleDef();

    for (int32 i = 0; i < pdefParticle->nMsgTriggeredEventCount; ++i)
    {
        const MsgTriggeredEvent & tEvent = pdefParticle->ptMsgTriggeredEvents[i];

        if (!MsgTriggeredEventShouldTrigger(tEvent, tMsg))
        {
            continue;
        }
        // Inference: the original switch selects particle parents for create/delete/outro and system parents otherwise.
        ParentType nParentType;
        switch (tEvent.dwMsgKey)
        {
            case PARTICLE_MSGKEY_PARTICLE_CREATE:
            case PARTICLE_MSGKEY_PARTICLE_DELETE:
            case PARTICLE_MSGKEY_PARTICLE_OUTRO:






                // Inference: original bit 13 selects actor parent 0 instead of particle parent 2.
                nParentType = (tParticleSystem.dwFlags & (1 << PARTICLE_SYSTEM_HAS_ACTORS)) ? PARENT_RACTOR : PARENT_PARTICLE;
                break;
            default:
                nParentType = PARENT_PARTICLE_SYSTEM;
                break;
        }









        Blizzard::Lock::ScopedMutex mutex(CGameGet().tObjectUpdateGlobalLock);

        tParticleSystem.tRMsgList.SendMessage(nParentType, idAttachedTo, FALSE, tEvent.tTriggerEvent, tMsg.flImpulseMagnitude, tParticleSystem.bRemote);

    }

}



// Inference: branch spelling follows the line table; absent cylinder-motion flag returns without changing motion. No standalone original body is checkable; its inlined instructions are checked through ParticleSystemUpdateParams.
static void sUpdateCylindricalMotion(
    const ParticleSystem * ptParticleSystem,
    const ParticleDefinition * pdefParticle,
    const InterpTime & tInterpTime,
    const Vector3D & wpParticle,
    ParticleParams & tParams,
    const ParticleConstParams & tConstParams,
    Vector3D & wvMotion,
    float flDeltaTime)
{
    if (!(ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_CYLINDER_MOTION)))
    {
        return;
    }
    // Inference: original stack stores initialize the fallback axis to +Z.
    Vector3D wvAxis(0.0f, 0.0f, 1.0f);
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_CYLINDER_AXIS))
    {
        wvAxis = tConstParams.wvConstCylinderAxis;
    }
    else
    {
        Vector3D wvRotationAxis(0.0f, 0.0f, 1.0f);
        if (pdefParticle->pathMotionCylinderAxis)
        {
            pdefParticle->pathMotionCylinderAxis->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_CYLINDERAXIS, tInterpTime, wvRotationAxis);
        }

        float wdMagnitude = wvRotationAxis.Normalize();
        if (wdMagnitude > EPSILON)
        {
            wvAxis = wvRotationAxis;
        }
    }


    Quaternion qRot;
    qRot.FromArc(cgwvZAxis, wvAxis);
    Vector3D vecToSystemCenter = wpParticle - ptParticleSystem->wp;
    Vector3D vecAxisRelPosition = qRot.Conjugate().Rotate(vecToSystemCenter);

    Vector2D wvCurrent2DPosition(vecAxisRelPosition.x, vecAxisRelPosition.y);

    // Inference: near-axis particles reuse their previous radial direction, otherwise normalize the current projection.
    Vector2D wvCylinderRadial = wvCurrent2DPosition;
    if (wvCurrent2DPosition.MagnitudeSquared() < EPSILON)
    {

        wvCylinderRadial = tParams.wvLastCylinderRadial;
    }
    else
    {



        wvCylinderRadial.Normalize();
        tParams.wvLastCylinderRadial = wvCylinderRadial;
    }

    float fCylinderRadialVel = 0.0f;
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_CYLINDER_RADIAL_VEL))
    {
        fCylinderRadialVel = tConstParams.flConstCylinderRadialVel;
    }
    else
    {
        if (pdefParticle->pathMotionCylinderRadialVel)
        {
            pdefParticle->pathMotionCylinderRadialVel->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_CYLINDERRADIALVEL, tInterpTime, fCylinderRadialVel);
            fCylinderRadialVel *= 60.0f;
        }
    }

    float fCylinderRadialPos = 0.0f;
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_CYLINDER_RADIAL_POS))
    {
        fCylinderRadialPos = tConstParams.flConstCylinderRadialPos;
    }
    else
    {
        if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionCylinderRadialPos)
        {
            pdefParticle->ptExtendedPath_Console->pathMotionCylinderRadialPos->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_CYLINDERRADIALPOS, tInterpTime, fCylinderRadialPos);
        }
    }
    // Inference: preserve the original reciprocal-delta and 60-Hz divisions, including their operation order.
    fCylinderRadialVel += ((fCylinderRadialPos - tParams.fLastCylinderRadialPos) * (1.0f / flDeltaTime)) / (flDeltaTime * 60.0f);
    tParams.fLastCylinderRadialPos = fCylinderRadialPos;

    Vector2D wvCylinderRadialChange = wvCylinderRadial * fCylinderRadialVel * flDeltaTime;
    Vector3D wvAxialMotion;

    Angle aCylinderAngleVel = 0.0f;
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_CYLINDER_ANGLE_VEL))
    {
        aCylinderAngleVel = tConstParams.aConstCylinderAngleVel;
    }
    else
    {
        if (pdefParticle->pathMotionCylinderAngleVel)
        {
            pdefParticle->pathMotionCylinderAngleVel->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_CYLINDERANGLEVEL, tInterpTime, aCylinderAngleVel);
            aCylinderAngleVel *= 60.0f;
        }
    }

    Vector2D wvRotated2DPosition = wvCurrent2DPosition;
    wvRotated2DPosition.Rotate(aCylinderAngleVel * flDeltaTime);
    // Inference: the axial-motion temporary combines radial and angular displacement, with zero axial displacement.
    wvAxialMotion.x = wvCylinderRadialChange.x + (wvRotated2DPosition.x - wvCurrent2DPosition.x);
    wvAxialMotion.y = wvCylinderRadialChange.y + (wvRotated2DPosition.y - wvCurrent2DPosition.y);
    wvAxialMotion.z = 0.0f;

    wvMotion += qRot.Rotate(wvAxialMotion);
}



// Inference: original sphere-motion guard and near-origin fallback mirror the cylinder radial path. No standalone original body is checkable; its inlined instructions are checked through ParticleSystemUpdateParams.
static void sUpdateSphericalMotion(
    const ParticleSystem * ptParticleSystem,
    const ParticleDefinition * pdefParticle,
    const InterpTime & tInterpTime,
    const Vector3D & wpParticle,
    ParticleParams & tParams,
    const ParticleConstParams & tConstParams,
    Vector3D & wvMotion,
    float flDeltaTime)
{
    if (!(ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_SPHERE_MOTION)))
    {
        return;
    }

    Vector3D wvCurrent3DPosition = wpParticle - ptParticleSystem->wp;

    Vector3D wvSphereRadial = wvCurrent3DPosition;
    if (wvCurrent3DPosition.MagnitudeSquared() < EPSILON)
    {

        wvSphereRadial = tParams.wvLastSphereRadial;
    }
    else
    {



        wvSphereRadial.Normalize();
        tParams.wvLastSphereRadial = wvSphereRadial;
    }

    float fSphereRadialVel = 0.0f;
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_SPHERE_RADIAL_VEL))
    {
        fSphereRadialVel = tConstParams.flConstSphereRadialVel;
    }
    else
    {
        if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionSphereRadialVel)
        {
            pdefParticle->ptExtendedPath_Console->pathMotionSphereRadialVel->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_SPHERERADIALVEL, tInterpTime, fSphereRadialVel);
            fSphereRadialVel *= 60.0f;
        }
    }

    float fSphereRadialPos = 0.0f;
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_SPHERE_RADIAL_POS))
    {
        fSphereRadialPos = tConstParams.flConstSphereRadialPos;
    }
    else
    {
        if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionSphereRadialPos)
        {
            pdefParticle->ptExtendedPath_Console->pathMotionSphereRadialPos->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_SPHERERADIALPOS, tInterpTime, fSphereRadialPos);
        }
    }
    // Inference: original positional delta is converted to radial velocity with one reciprocal-time multiply.
    fSphereRadialVel += (fSphereRadialPos - tParams.fLastSphereRadialPos) * (1.0f / flDeltaTime);
    tParams.fLastSphereRadialPos = fSphereRadialPos;

    wvMotion += wvSphereRadial * fSphereRadialVel * flDeltaTime;
}



// Inference: original flag guard, zero path defaults, and Euler accumulation reconstructed from line-attributed vector operations. No standalone original body is checkable; its inlined instructions are checked through ParticleSystemUpdateParams.
static void sUpdateCartesianMotion(
    const ParticleSystem * ptParticleSystem,
    const ParticleDefinition * pdefParticle,
    const InterpTime & tInterpTime,
    ParticleParams & tParams,
    const ParticleConstParams & tConstParams,
    Vector3D & wvMotion,
    float & wdPositionUp,
    float flDeltaTime)
{
    if (!(ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_CARTESIAN_MOTION)))
    {
        return;
    }

    Vector3D wvCartesianVel(0.0f, 0.0f, 0.0f);
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_CARTESIAN_VEL))
    {
        wvCartesianVel = tConstParams.vConstCartesianVel;
    }
    else
    {
        if (pdefParticle->pathMotionCartesianVel)
        {
            pdefParticle->pathMotionCartesianVel->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_CARTESIANVEL, tInterpTime, wvCartesianVel);
            wvCartesianVel = wvCartesianVel * 60.0f;
        }
    }

    Vector3D wvCartesianPos(0.0f, 0.0f, 0.0f);
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_CARTESIAN_POS))
    {
        wvCartesianPos = tConstParams.vConstCartesianPos;
    }
    else
    {
        if (pdefParticle->pathMotionCartesianPos)
        {
            pdefParticle->pathMotionCartesianPos->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_CARTESIANPOS, tInterpTime, wvCartesianPos);
        }
    }
    // Inference: the observed reciprocal-time multiply adds position change to velocity before acceleration.
    wvCartesianVel += (wvCartesianPos - tParams.wvLastCartesianPos) * (1.0f / flDeltaTime);
    tParams.wvLastCartesianPos = wvCartesianPos;

    Vector3D wvCartesianAcc(0.0f, 0.0f, 0.0f);
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_CARTESIAN_ACC))
    {
        wvCartesianAcc = tConstParams.vConstCartesianAcc;
    }
    else
    {
        if (pdefParticle->pathMotionCartesianAcc)
        {
            pdefParticle->pathMotionCartesianAcc->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_CARTESIANACC, tInterpTime, wvCartesianAcc);
            wvCartesianAcc = wvCartesianAcc * 60.0f * 60.0f;
        }
    }

    tParams.wvTotalCartesianAcc += wvCartesianAcc * flDeltaTime;
    wvCartesianVel += tParams.wvTotalCartesianAcc;

    wvMotion += wvCartesianVel * flDeltaTime;
    wdPositionUp += wvCartesianPos.z;
}




// Inference: original flag guard, zero path defaults, and Euler accumulation reconstructed from line-attributed vector operations. No standalone original body is checkable; its inlined instructions are checked through ParticleSystemUpdateParams.
static void sUpdateRelativeCartesianMotion(
    const ParticleSystem * ptParticleSystem,
    const ParticleDefinition * pdefParticle,
    const InterpTime & tInterpTime,
    ParticleParams & tParams,
    const ParticleConstParams & tConstParams,
    Vector3D & wvMotion,
    float & wdPositionUp,
    float flDeltaTime)
{
    if (!(ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_RELATIVE_CARTESIAN_MOTION)))
    {
        return;
    }

    Vector3D wvRelativeCartesianVel(0.0f, 0.0f, 0.0f);
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_RELATIVE_CARTESIAN_VEL))
    {
        wvRelativeCartesianVel = tConstParams.vConstRelativeCartesianVel;
    }
    else
    {
        if (pdefParticle->pathMotionRelativeCartesianVel)
        {
            pdefParticle->pathMotionRelativeCartesianVel->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_RELATIVECARTESIANVEL, tInterpTime, wvRelativeCartesianVel);
            wvRelativeCartesianVel = wvRelativeCartesianVel * 60.0f;
        }
    }

    Vector3D wvRelativeCartesianPos(0.0f, 0.0f, 0.0f);
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_RELATIVE_CARTESIAN_POS))
    {
        wvRelativeCartesianPos = tConstParams.vConstRelativeCartesianPos;
    }
    else
    {
        if (pdefParticle->pathMotionRelativeCartesianPos)
        {
            pdefParticle->pathMotionRelativeCartesianPos->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_RELATIVECARTESIANPOS, tInterpTime, wvRelativeCartesianPos);
        }
    }
    // Inference: the observed reciprocal-time multiply adds position change to velocity before acceleration.
    wvRelativeCartesianVel += (wvRelativeCartesianPos - tParams.wvLastRelativeCartesianPos) * (1.0f / flDeltaTime);
    tParams.wvLastRelativeCartesianPos = wvRelativeCartesianPos;

    Vector3D wvRelativeCartesianAcc(0.0f, 0.0f, 0.0f);
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_RELATIVE_CARTESIAN_ACC))
    {
        wvRelativeCartesianAcc = tConstParams.vConstRelativeCartesianAcc;
    }
    else
    {
        if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionRelativeCartesianAcc)
        {
            pdefParticle->ptExtendedPath_Console->pathMotionRelativeCartesianAcc->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_RELATIVECARTESIANACC, tInterpTime, wvRelativeCartesianAcc);
            wvRelativeCartesianAcc = wvRelativeCartesianAcc * 60.0f * 60.0f;
        }
    }

    tParams.wvTotalRelativeCartesianAcc += wvRelativeCartesianAcc * flDeltaTime;
    wvRelativeCartesianVel += tParams.wvTotalRelativeCartesianAcc;


    // Inference: accumulated relative displacement is retained before the per-frame displacement is rotated into world space.
    tParams.wvTotalRelativeCartesianPos += wvRelativeCartesianVel * flDeltaTime;

    Vector3D wvRelativeCartesianVelSystemSpace = tParams.qRelativeOrientation.Rotate(wvRelativeCartesianVel * flDeltaTime);

    wvMotion += wvRelativeCartesianVelSystemSpace;
    wdPositionUp += wvRelativeCartesianPos.z;
}





// Inference: original attractor-motion guard and zero path defaults follow the recorded branches. No standalone original body is checkable; its inlined instructions are checked through ParticleSystemUpdateParams.
static void sUpdateAttractorMotion(
    const ParticleSystem * ptParticleSystem,
    const ParticleDefinition * pdefParticle,
    const InterpTime & tInterpTime,
    const Vector3D & wpParticle,
    ParticleParams & tParams,
    const ParticleConstParams & tConstParams,
    Vector3D & wvMotion,
    float flDeltaTime)
{
    if (!(ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_ATTRACTOR_MOTION)))
    {
        return;
    }

    Vector3D wvCurrentToAttractorNormalized = ptParticleSystem->wpAttractor - wpParticle;
    wvCurrentToAttractorNormalized.Normalize();

    float fAttractorVel = 0.0f;
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_ATTRACTOR_VEL))
    {
        fAttractorVel = tConstParams.flConstAttractorVel;
    }
    else
    {
        if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionAttractorVel)
        {
            pdefParticle->ptExtendedPath_Console->pathMotionAttractorVel->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_ATTRACTORVEL, tInterpTime, fAttractorVel);
            fAttractorVel *= 60.0f;
        }
    }
    // Inference: original tests dwFlags (offset 8), not dwDefFlags, against the definition bit 30.
    if (!(ptParticleSystem->dwFlags & (1 << PARTICLE_DEF_ATTRACTOR_VEL_WORLD_SPACE_BIT)))
    {
        Vector3D wvSpawnToAttractor = ptParticleSystem->wpAttractor - ptParticleSystem->wp;
        float wdSpawnToAttractor = wvSpawnToAttractor.Magnitude();

        float fAttractorPos = 0.0f;
        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_MOTION_ATTRACTOR_POS))
        {
            fAttractorPos = tConstParams.flConstAttractorPos;
        }
        else
        {
            if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathMotionAttractorPos)
            {
                pdefParticle->ptExtendedPath_Console->pathMotionAttractorPos->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_ATTRACTORPOS, tInterpTime, fAttractorPos);
            }
        }
        // Inference: position change contributes reciprocal-time velocity before multiplication by the spawn distance.
        fAttractorVel += (fAttractorPos - tParams.fLastAttractorPos) * (1.0f / flDeltaTime);
        tParams.fLastAttractorPos = fAttractorPos;

        fAttractorVel *= wdSpawnToAttractor;
    }

    wvMotion += wvCurrentToAttractorNormalized * fAttractorVel * flDeltaTime;
}



// Inference: retain the observed floor-snap success/failure paths; the unavailable floor restores only previous X/Y. No standalone original body is checkable; its inlined instructions are checked through ParticleSystemUpdateParams.
static BOOL sDoGroundHugging(
    ParticleParams & tParams,
    const RenderContext eContext,
    const float wdPositionUp,
    const Vector3D & wpParticlePrev,
    Vector3D & wpParticle)
{










    WorldPlace tPlaceSnapped = ClientPointToPlace(wpParticle);


    tPlaceSnapped.wp.z -= wdPositionUp;





    BOOL bSnapped = CScenesGetFloorHeight(tPlaceSnapped.idSWorld, tPlaceSnapped.wp, tPlaceSnapped.wp.z);

    if (bSnapped)
    {
        wpParticle = tPlaceSnapped.wp;


        wpParticle.z += wdPositionUp;
    }
    else
    {


        wpParticle.x = wpParticlePrev.x;
        wpParticle.y = wpParticlePrev.y;
    }

    return bSnapped;
}


// Inference: original spin flag guard and zero angular defaults; position-delta velocity is added before angle wrapping. No standalone original body is checkable; its inlined instructions are checked through ParticleSystemUpdateParams.
static void sUpdateSpin(
    const ParticleSystem * ptParticleSystem,
    const ParticleDefinition * pdefParticle,
    const InterpTime & tInterpTime,
    ParticleParams & tParams,
    const ParticleConstParams & tConstParams,
    float flDeltaTime)
{
    if (!(ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_SPIN)))
    {
        return;
    }

    Angle aAngVelocity = 0.0f;
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ANGULAR_VELOCITY))
    {
        aAngVelocity = tConstParams.aConstAngVel;
    }
    else
    {
        if (pdefParticle->pathAngularVelocity)
        {
            pdefParticle->pathAngularVelocity->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_ANG_VELOCITY, tInterpTime, aAngVelocity);
            aAngVelocity *= 60.0f;
        }
    }

    Angle aAngPosition = 0.0f;
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ANGULAR_POSITION))
    {
        aAngPosition = tConstParams.aConstAngPos;
    }
    else
    {
        if (pdefParticle->pathAngularPosition)
        {
            pdefParticle->pathAngularPosition->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_ANG_POSITION, tInterpTime, aAngPosition);
        }
    }

    Angle aPositionChange = (aAngPosition - tParams.aLastAngPosition) * (1.0f / flDeltaTime);
    tParams.aLastAngPosition = aAngPosition;

    Angle aTotalAngVelocity = aAngVelocity + aPositionChange;

    if (aTotalAngVelocity != 0.0f)
    {
        tParams.aAng = Math::NormalizeAngle(tParams.aAng + aTotalAngVelocity * flDeltaTime);
    }
}



// Inference: original scale default 1 and stretch fallback 1; actor scales are clamped to [0.0001, 999]. No standalone original body is checkable; its inlined instructions are checked through ParticleSystemUpdateParams.
static void sUpdateScaleAndStretch(
    const ParticleSystem * ptParticleSystem,
    const ParticleDefinition * pdefParticle,
    const InterpTime & tInterpTime,
    ParticleParams & tParams,
    const ParticleConstParams & tConstParams)
{
    float flScaleFrac = 1.0f;
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_SCALE))
    {
        flScaleFrac = tConstParams.flConstScale;
    }
    else
    {
        if (pdefParticle->pathParticleScale)
        {
            pdefParticle->pathParticleScale->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_SCALE, tInterpTime, flScaleFrac);
        }
    }

    tParams.flScale = tParams.flBaseScale * ptParticleSystem->flScaleFrac * flScaleFrac;

    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_STRETCH))
    {
        tParams.fStretch = tConstParams.flConstStretch;
    }
    else
    {
        if (pdefParticle->pathParticleStretch)
        {
            pdefParticle->pathParticleStretch->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_STRETCH, tInterpTime, tParams.fStretch);
        }
        else
        {
            tParams.fStretch = 1.0f;
        }
    }

    if (ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_HAS_ACTORS))
    {

        tParams.flScale = Math::Pin(tParams.flScale, 0.0001f, 999.0f);
    }
}



// Inference: original orientation guard, +Y fallback, and normalization threshold determine whether to reuse or replace the saved axis. No standalone original body is checkable; its inlined instructions are checked through ParticleSystemUpdateParams.
static void sUpdateOrientationMotion(
    const ParticleSystem * ptParticleSystem,
    const ParticleDefinition * pdefParticle,
    const InterpTime & tInterpTime,
    ParticleParams & tParams,
    const ParticleConstParams & tConstParams,
    float flDeltaTime)
{
    if (!(ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_USES_ORIENTATION_MOTION)))
    {
        return;
    }

    Vector3D wvRotationAxis(0.0f, 1.0f, 0.0f);
    if ((ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_HAS_ORIENTATION_AXIS)) && !(ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ORIENTATION_AXIS)))
    {

        if (pdefParticle->pathOrientationAxis)
        {
            pdefParticle->pathOrientationAxis->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_ORIENTATIONAXIS, tInterpTime, wvRotationAxis);
        }

        float wdMagnitude = wvRotationAxis.Normalize();
        if (wdMagnitude < EPSILON)
        {
            wvRotationAxis = tParams.wvOrientationAxis;
        }
        else
        {
            tParams.wvOrientationAxis = wvRotationAxis;
        }
    }
    else
    {
        wvRotationAxis = tParams.wvOrientationAxis;
    }

    Angle aRotationVel = 0.0f;
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ORIENTATION_VELOCITY))
    {
        aRotationVel = tConstParams.aConstOrientationVel;
    }
    else
    {
        if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathOrientationVelocity)
        {
            pdefParticle->ptExtendedPath_Console->pathOrientationVelocity->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_ORIENTATIONVEL, tInterpTime, aRotationVel);
            aRotationVel *= 60.0f;
        }
    }

    Angle aRotationPos = 0.0f;
    if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ORIENTATION_POSITION))
    {
        aRotationPos = tConstParams.aConstOrientationPos;
    }
    else
    {
        if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathOrientationPosition)
        {
            pdefParticle->ptExtendedPath_Console->pathOrientationPosition->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_ORIENTATIONPOS, tInterpTime, aRotationPos);
        }
    }
    // Inference: original subtraction is previous minus current rotation position, opposite the translational paths.
    aRotationVel += (tParams.aLastRotationPos - aRotationPos) * (1.0f / flDeltaTime);
    tParams.aLastRotationPos = aRotationPos;

    Quaternion qOrientationRotation;
    qOrientationRotation.SetRotation(wvRotationAxis, aRotationVel * flDeltaTime);
    // Inference: original quaternion product pre-multiplies the existing orientation by the per-frame rotation.
    tParams.qOrientation = qOrientationRotation * tParams.qOrientation;
    tParams.qOrientation.Normalize();
}
const MaterialTextureEntry * MaterialLookup(const UberMaterial & tUberMaterial, ShaderTex eShaderTex);  // Gfx/GfxMaterial.cpp:354 (local declaration, as in Particle.cpp)
uint32 TexAnimUpdate(const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, const ObjectTexAnimParam * ptObjTexAnimParam, TexAnimState & tTexAnimState, float flDeltaTime, float flClientTime);  // TexAnimation.cpp:316
void ParticleSystemUpdateBounds(float & wdSystemRadius, const Vector3D & wpSystemPos, const float flScale, const float flStretch, const Vector3D & wpPos);  // forward declaration of ParticleUpdate.cpp:1175 (called at 874)

void UpdateDynamicFoliage(
    ParticleSystem * ptParticleSystem,
    const Vector3D & wvWind3D,
    float flOscAmplitude,
    float flOscAngle,
    float flDeltaTime)
{



    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();

    const float cflOmega = 2.0f * PI * pdefParticle->flFoliageFrequency;
    const float cflDamping = 2.0f * pdefParticle->flFoliageDampingRatio * cflOmega;
    const float cflStiffness = cflOmega * cflOmega;
    float flFoliagePrevailingWindFactor = pdefParticle->flFoliagePrevailingWindFactor;
    float flFoliageOscWindFactor = pdefParticle->flFoliageOscWindFactor;
    float flFoliageMaxOffsetFactor = pdefParticle->flFoliageMaxOffsetFactor;

    if (ptParticleSystem->flFoliageUpdateStep == 0.0f)
    {
        int32 nCount = ptParticleSystem->tParticleList.size();
        for (int32 pos = 0; pos < nCount; pos++)
        {
            Particle * ptParticle = ptParticleSystem->tParticleList[pos];

            FoliageState * ptState = &ptParticle->tParams.tFoliageState;


            float flPhaseShift = 2.0f * PI * ptParticle->tParams.qRelativeOrientation.wdReal;
            float flScale = flOscAmplitude * Math::Cos(flOscAngle + flPhaseShift);
            float flNetFactor = flFoliagePrevailingWindFactor - (flScale * flFoliageOscWindFactor);
            Vector2D wvWind(flNetFactor * wvWind3D.x, flNetFactor * wvWind3D.y);
            // Inference: 0.016666668 in the original bytes, written as one sixtieth (60Hz step); no named constant in the CU.
            ptState->wvVelocityWind = wvWind * (1.0f / 60.0f);

            ptState->wvVelocity += ptState->wvVelocityWind + flDeltaTime * (-cflDamping * ptState->wvVelocity - cflStiffness * ptState->wvOffset);
            ptState->wvOffset += flDeltaTime * ptState->wvVelocity;


            float flMaxOffset = ptParticle->tParams.flScale * flFoliageMaxOffsetFactor;
            if (ptState->wvOffset.MagnitudeSquared() > flMaxOffset * flMaxOffset)
            {
                ptState->wvOffset.Normalize();
                ptState->wvOffset *= 0.95f * flMaxOffset;
                ptState->wvVelocity.Set(0.0f, 0.0f);
            }
        }
    }
    else
    {
        int32 nCount = ptParticleSystem->tParticleList.size();
        for (int32 pos = 0; pos < nCount; pos++)
        {
            Particle * ptParticle = ptParticleSystem->tParticleList[pos];

            FoliageState * ptState = &ptParticle->tParams.tFoliageState;

            ptState->wvVelocity += ptState->wvVelocityWind + flDeltaTime * (-cflDamping * ptState->wvVelocity - cflStiffness * ptState->wvOffset);
            ptState->wvOffset += flDeltaTime * ptState->wvVelocity;


            float flMaxOffset = ptParticle->tParams.flScale * pdefParticle->flFoliageMaxOffsetFactor;
            if (ptState->wvOffset.MagnitudeSquared() > flMaxOffset * flMaxOffset)
            {
                ptState->wvOffset.Normalize();
                ptState->wvOffset *= 0.95f * flMaxOffset;
                ptState->wvVelocity.Set(0.0f, 0.0f);
            }
        }
    }
}

























int32 UpdateParticlesForSystem(
    ParticleSystem * ptParticleSystem,
    DeletePartCallback pDeletePartFn,
    BOOL fFlagOnly,
    float & wdSystemRadius,
    BOOL bUpdateVisibleParams,
    float flDeltaTime)
{
    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();
    int32 nParticleUpdatedCount = 0;

    const MaterialTextureEntry * ptMatTex[NUM_BILLBOARD_TEX_STATES];
    for (int32 i = 0; i < NUM_BILLBOARD_TEX_STATES; ++i)
    {
        ShaderTex eShaderTex = (i == BTS_DIFFUSE) ? SHADERTEX_DIFFUSE : (i == BTS_ALPHA) ? SHADERTEX_ALPHA : (i == BTS_ALPHA_LAYER_1) ? SHADERTEX_ALPHA_LAYER_1 : SHADERTEX_ALPHA_LAYER_2;
        ptMatTex[i] = MaterialLookup(pdefParticle->tUberMaterial, eShaderTex);
    }

    float wdTempSystemRadius = wdSystemRadius;

    int32 nCount = ptParticleSystem->tParticleList.size();
    for (int32 pos = 0; pos < nCount; pos++)
    {
        Particle * ptParticle = ptParticleSystem->tParticleList[pos];

        Vector3D wpParticle;
        ParticleParams & tParams = ptParticle->tParams;

        BOOL fDelete = FALSE;

        uint32 dwResultFlags = 0;
        for (int32 j = 0; j < NUM_BILLBOARD_TEX_STATES; j++)
        {
            if (ptMatTex[j])
            {

                dwResultFlags = TexAnimUpdate(ptMatTex[j]->tMatTex.pTexAnimCompParams, ptMatTex[j]->tMatTex.serTexAnimCompParams.dwSize, NULL, ptParticle->tTexAnimState[j], flDeltaTime, 0.0f);
            }
        }



        // Inference: result bit 0 is the animation-finished flag, tested with a literal as in Rope.cpp; definition bit tested first (that operand order reproduces the original block layout).
        if ((ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_DELETE_PARTICLE_AT_ANIMATION_END_BIT)) && (dwResultFlags & 1))
        {

            if (!fFlagOnly)
            {
                ParticleAttachmentsOnMsg(*ptParticleSystem, ptParticle->id, KeyedMessage(PARTICLE_MSGKEY_PARTICLE_DELETE, 0.0f, Shape::DEFAULT_MATERIAL));
            }
            fDelete = TRUE;  // Inference: no row of its own; the shared delete call at 879 is reached from this branch.
        }
        else
        {
            wpParticle = ptParticle->wp;

            fDelete = ParticleSystemUpdateParams(ptParticleSystem, FALSE, ptParticle->id, tParams, wpParticle, fFlagOnly, bUpdateVisibleParams, flDeltaTime);
            nParticleUpdatedCount++;
            if (!fDelete)
            {



                ptParticle->wp = wpParticle;


                ParticleSystemUpdateBounds(wdTempSystemRadius, ptParticleSystem->wp, tParams.flScale, tParams.fStretch, wpParticle);
            }
        }
        if (fDelete)
        {
            pDeletePartFn(ptParticleSystem, ptParticle, fFlagOnly);
            if (!fFlagOnly)
            {
                pos--;
                nCount--;
            }
        }
    }

    wdSystemRadius = wdTempSystemRadius;

    return nParticleUpdatedCount;
}



// Inference: original degeneracy branches use identity, X-axis half-turn, or normalized cross-axis quaternions.
static void QuatFromXOnlyArc(
    Quaternion & quat,
    const Vector3D & wv1)
{


    Vector3D wvNorm1 = wv1;

    float wdMag1 = wvNorm1.Normalize();

    if (wdMag1 < EPSILON)
    {
        // Inference: DWARF records assignment from a temporary quaternion on line 909.
        quat = Quaternion(Vector3D(0.0f, 0.0f, 0.0f), 1.0f);
        return;
    }

    float wdDot = wvNorm1.x;


    // DWARF location constant 0x3a83126f is the original 0.001f initializer.
    const float QUATERNION_ARC_EPSILON = 0.001f;

    if (wdDot > 1.0f - QUATERNION_ARC_EPSILON)
    {
        quat = Quaternion(Vector3D(0.0f, 0.0f, 0.0f), 1.0f);

        return;
    }
    else if (wdDot < -1.0f + QUATERNION_ARC_EPSILON)
    {
        // DWARF records Quaternion::Set at line 928; original stores the X-axis and zero real part.
        quat.Set(Vector3D(1.0f, 0.0f, 0.0f), 0.0f);
        return;
    }

    Vector3D wvCross(0.0f, -wvNorm1.z, wvNorm1.y);
    // Inference: test the unsquared-root scale against epsilon before taking its root, as in the original branch.
    float flS = (wdDot + 1.0f) * 2.0f;
    if (flS >= EPSILON)
    {
        flS = Math::Sqrt(flS);
        float flInvS = 1.0f / flS;

        quat.wvImag = wvCross * flInvS;
        quat.wdReal = flS * 0.5f;
    }
    else
    {
        quat.wvImag = wvCross;
        quat.wvImag.Normalize();
        quat.wdReal = 0.0f;
    }
}



// Inference: deletion paths send the original delete message conditionally, orient from the last motion, and return TRUE.
BOOL ParticleSystemUpdateParams(
    ParticleSystem * ptParticleSystem,
    const BOOL fInitialUpdate,
    const DataID idInstance,
    ParticleParams & tParams,
    Vector3D & wpParticle,
    BOOL fFlagDeletionOnly,
    BOOL bUpdateVisibleParams,
    float flDeltaTime)
{
    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();

    Vector3D wpParticlePrev = wpParticle;





    tParams.flSeconds += flDeltaTime;

    if ((ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_DELETE_PARTICLE_AT_DURATION_END_BIT)) && tParams.flSeconds >= tParams.flDuration)
    {


        if (!fFlagDeletionOnly) {
            // Original constructor stores zero impulse and material; spell defaults absent from the adopted declaration.
            ParticleAttachmentsOnMsg(*ptParticleSystem, idInstance, KeyedMessage(PARTICLE_MSGKEY_PARTICLE_DELETE, 0.0f, Shape::DEFAULT_MATERIAL));
        }
        QuatFromXOnlyArc(tParams.qOrientation, tParams.wvLastMotionNormalized);
        return TRUE;
    }





    // Inference: zero or negative lifetime skips interpolation and returns FALSE, including the observed NaN comparison behavior.
    if (tParams.flDuration <= 0.0f)
    {
        return FALSE;
    }

    InterpTime tInterpTime;
    ParticleGetInterpTime(ptParticleSystem, tParams, wpParticle, tInterpTime);
    // Inference: original motion starts at zero, then uses either path integration or the physics velocity.
    Vector3D wvMotion(0.0f, 0.0f, 0.0f);

    if (ptParticleSystem->eType != PSTYPE_PHYS_BILLBOARD && ptParticleSystem->eType != PSTYPE_PHYS_ACTOR)
    {

        float wdPositionUp = 0.0f;



        sUpdateCylindricalMotion(ptParticleSystem, pdefParticle, tInterpTime, wpParticle, tParams, ptParticleSystem->tConstParams, wvMotion, flDeltaTime);
        sUpdateSphericalMotion(ptParticleSystem, pdefParticle, tInterpTime, wpParticle, tParams, ptParticleSystem->tConstParams, wvMotion, flDeltaTime);
        sUpdateCartesianMotion(ptParticleSystem, pdefParticle, tInterpTime, tParams, ptParticleSystem->tConstParams, wvMotion, wdPositionUp, flDeltaTime);
        sUpdateRelativeCartesianMotion(ptParticleSystem, pdefParticle, tInterpTime, tParams, ptParticleSystem->tConstParams, wvMotion, wdPositionUp, flDeltaTime);
        sUpdateAttractorMotion(ptParticleSystem, pdefParticle, tInterpTime, wpParticle, tParams, ptParticleSystem->tConstParams, wvMotion, flDeltaTime);

        wpParticle += wvMotion;
        tParams.wpLast = wpParticle;

        if (!tParams.tCreateParams.fIgnoreGroundHug) {
            // Inference: scalar enum conversion restores the DWARF RenderContext type omitted by the adopted ParticleSystem field.
            if ((ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_GROUND_HUGGING)) && !sDoGroundHugging(tParams, static_cast<RenderContext>(ptParticleSystem->eContext), wdPositionUp, wpParticlePrev, wpParticle))
            {
                return TRUE;
            }
        }
    }
    else
    {


        if (tParams.ptPointMass)
        {
            PointMass & pm = *tParams.ptPointMass;
            wvMotion = pm.wv;
        }
    }


    tParams.wvMotion = wvMotion;
    float flMagSq = wvMotion.MagnitudeSquared();
    if (flMagSq > EPSILON)
    {
        float flMag = Math::Sqrt(flMagSq);
        float wdReciprocalMag = 1.0f / flMag;
        tParams.wvLastMotionNormalized = wvMotion * wdReciprocalMag;
        tParams.wdParticleSpeed = flMag;
    }
    else
    {
        tParams.wdParticleSpeed = 0.0f;
    }




    // Inference: original XY-only bounds test sends the same deletion message and shares the deletion exit.
    if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_DELETE_WHEN_OUTSIDE_BOUNDING_RADIUS))
    {
        float wdSquared = Vector3D::Distance2DSquared(wpParticle, ptParticleSystem->wp);
        float wdBoundsSquared = pdefParticle->wdBoundingRadius * pdefParticle->wdBoundingRadius;
        if (wdSquared > wdBoundsSquared)
        {
            if (!fFlagDeletionOnly) {
                // Original constructor stores zero impulse and material; spell defaults absent from the adopted declaration.
                ParticleAttachmentsOnMsg(*ptParticleSystem, idInstance, KeyedMessage(PARTICLE_MSGKEY_PARTICLE_DELETE, 0.0f, Shape::DEFAULT_MATERIAL));
            }
            QuatFromXOnlyArc(tParams.qOrientation, tParams.wvLastMotionNormalized);
            return TRUE;
        }
    }




    sUpdateSpin(ptParticleSystem, pdefParticle, tInterpTime, tParams, ptParticleSystem->tConstParams, flDeltaTime);
    // Inference: noninitial invisible or effectively transparent systems skip visible parameters but still synchronize physics.
    if (!fInitialUpdate)
    {
        if (!(ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_VISIBLE_BIT)) ||
            ptParticleSystem->flAlpha < EPSILON)
        {

            tParams.flAlpha = 0.0f;
            bUpdateVisibleParams = FALSE;
        }
    }
    if (bUpdateVisibleParams)
    {
        const ParticleConstParams & tConstParams = ptParticleSystem->tConstParams;

        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_COLOR))
        {
            tParams.rgbaColor = tConstParams.rgbaConstColor;
        }
        else
        {
            if (pdefParticle->pathParticleColor)
            {
                pdefParticle->pathParticleColor->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_COLOR, tInterpTime, tParams.rgbaColor);
            }
            else
            {
                tParams.rgbaColor.Set(255, 255, 255, 255);
            }
        }

        if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_MESH_LIT_ON_CREATE))
        {
            RGBAColor rgbaColorTmp(ptParticleSystem->rgbaColorMesh.r, ptParticleSystem->rgbaColorMesh.r, ptParticleSystem->rgbaColorMesh.r, 255);
            tParams.rgbaColor = tParams.rgbaColor * rgbaColorTmp;
        }

        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ALPHA))
        {
            tParams.flAlpha = tConstParams.flConstAlpha * ptParticleSystem->flAlpha;
        }
        else
        {
            if (pdefParticle->pathParticleAlpha)
            {
                pdefParticle->pathParticleAlpha->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_ALPHA, tInterpTime, tParams.flAlpha);
            }
            else
            {
                tParams.flAlpha = 1.0f;
            }

            tParams.flAlpha *= ptParticleSystem->flAlpha;
        }
        // Inference: original auxiliary alpha rounds the scaled value to a byte, then replicates that byte to all channels.
        if (ptParticleSystem->dwConstFlags & (1 << PARTICLE_PATH_CONST_ALPHAAUX))
        {
            uint8 a = Math::FloatRoundToInt(tConstParams.flConstAlphaAux * 255.0f);
            tParams.rgbaAlphaAux.Set(a, a, a, a);
        }
        else
        {
            if (pdefParticle->ptExtendedPath_Console && pdefParticle->ptExtendedPath_Console->pathParticleAlphaAux)
            {
                float flAlphaRef;
                pdefParticle->ptExtendedPath_Console->pathParticleAlphaAux->GetInterpolatedValueByTime(tParams.dwInterpSeed, PARTICLE_INTERP_ALPHAAUX, tInterpTime, flAlphaRef);
                uint8 a = Math::FloatRoundToInt(flAlphaRef * 255.0f);
                tParams.rgbaAlphaAux.Set(a, a, a, a);
            }
            else
            {
                tParams.rgbaAlphaAux.Set(255, 255, 255, 255);
            }
        }

        sUpdateScaleAndStretch(ptParticleSystem, pdefParticle, tInterpTime, tParams, tConstParams);
        sUpdateOrientationMotion(ptParticleSystem, pdefParticle, tInterpTime, tParams, tConstParams, flDeltaTime);
    }

    // Inference: original physics path adds this frame's displacement to the point mass and copies the resulting position back.
    if (tParams.ptPointMass)
    {
        Vector3D wvPositionChange = wpParticle - wpParticlePrev;





        PointMass & pm = *tParams.ptPointMass;
        pm.wp += wvPositionChange;

        wpParticle = pm.wp;
    }

    return FALSE;
}




// Inference: the observed comparisons select stretched width first and new radius first in the two Math::Max calls.
void ParticleSystemUpdateBounds(
    float & wdSystemRadius,
    const Vector3D & wpSystemPos,
    const float flScale,
    const float flStretch,
    const Vector3D & wpPos)
{

    float wdParticleWidth = Math::Max(flScale * flStretch, flScale);








    // Inference: adopted Distance subtracts its first argument from its second; this order preserves the original system-minus-particle loads.
    float wdSeparation = Vector3D::Distance(wpPos, wpSystemPos);

    wdParticleWidth += wdSeparation;


    wdSystemRadius = Math::Max(wdParticleWidth, wdSystemRadius);






}
