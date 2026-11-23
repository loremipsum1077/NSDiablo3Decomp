// X/Client/ParticleRenderUpdate.cpp
// Functions sit on their original line numbers (2.6.2 DWARF line table).

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CGame.h"
#include "Particle.h"
#include "ParticleInternal.h"
#include "ParticleRenderUpdate.h"
#include "Definitions/ParticleDefinition.h"
#include "../Sigma/Math/Frustum.h"
#include "../Sigma/Math/Intersect.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Quaternion.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/Math/Vector.h"

// Scene.cpp:4633 (not declared in a header)
BOOL CScenesGetFloorNormal(SWorldID idSWorld, const Vector3D & wp, Vector3D & wvFloorNormal);
// Inference: initialiser 85.0f from the DWARF const_value 0x42aa0000 (variable decl line 21).

const float CLUTTER_DISTANCE_MAX = 85.0f;
// Inference: initial value true from the byte at 0x1037080 in the image (DWARF decl line 26, external).
#include "Gfx/GfxVB.h"
#include "TexAnimation.h"
TexAnimMatrixType TexAnimGetTexMatrixForBillboard(Matrix4x4 & mTex, const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, const TexAnimState * ptTexAnimState, const TextureFramesInfo & tTexFramesInfo);  // TexAnimation.h (DWARF); defined at TexAnimation.cpp:962
bool gbParticleTrim = true;
enum { BILLBOARD_BOTTOM_ALIGNED_BIT = 0, BILLBOARD_CUSTOM_ORIENTATION_BIT = 1, BILLBOARD_FOLIAGE_BIT = 2, BILLBOARD_COMPUTE_NORMALS_BIT = 3, };  // X/Client/Particle.h (DWARF), not in src/X/Client/Particle.h
static const ParticleQuad tDefaultParticleQuadPos = { 1.0f, 1.0f, 1.0f, 1.0f };  // values from the image at 0xe3c8a0




float ParticleGetClutterDistance()

{

    return CLUTTER_DISTANCE_MAX;
}
// Inference: the facing test is nFacing == GROUND || nFacing == GROUND_PROJECTION (Ghidra: nFacing - 9 < 2).
// Inference: the x/y inequality tests are the || of two != (fcmp on lines 76 and 77, no z test).
// sUnitQuaternionToUByte4: lines 51-54 are the four Pin calls, 56-59 the four conversions (fcvtzs; the lower clamp shows
// up as a csel to 0 after the conversion).
static UByte4 sUnitQuaternionToUByte4(
    const Quaternion & qRotation
) {
    UByte4 packed;

    float x, y, z, w;


    x = Math::Pin((qRotation.wvImag.x * 0.5f + 0.5f) * 255.99f, 0.0f, 255.0f);
    y = Math::Pin((qRotation.wvImag.y * 0.5f + 0.5f) * 255.99f, 0.0f, 255.0f);
    z = Math::Pin((qRotation.wvImag.z * 0.5f + 0.5f) * 255.99f, 0.0f, 255.0f);
    w = Math::Pin((qRotation.wdReal * 0.5f + 0.5f) * 255.99f, 0.0f, 255.0f);

    packed.r = (uint8)x;
    packed.g = (uint8)y;
    packed.b = (uint8)z;
    packed.a = (uint8)w;

    return packed;
}





void UpdateGroundNormal(
    const ParticleDefinition * pdefParticle,
    const Vector3D & wpParticle,
    ParticleParams & tParams
) {
    if ((pdefParticle->nFacing == PARTICLE_FACING_GROUND || pdefParticle->nFacing == PARTICLE_FACING_GROUND_PROJECTION)


        && (wpParticle.x != tParams.wpLastValidNormal.x ||
            wpParticle.y != tParams.wpLastValidNormal.y)) {

        if (!CScenesGetFloorNormal(CGameGetCurrentSWorldID(), wpParticle, tParams.wvGroundNormal)) {

            tParams.wvGroundNormal = cgwvZAxis;
        }
        tParams.wpLastValidNormal = wpParticle;
    }

}


// ParticleIsOnScreen: the bit test (ldrb [+9], bit 2 = PARTICLE_SYSTEM_PARTIALLY_VISIBLE_BIT) is on line 100, the
// sphere on 105-106, the FrustumAndSphere call on 107. Only systems flagged partially visible are tested per particle.


BOOL ParticleIsOnScreen(
    const ParticleSystem * ptParticleSystem,
    float flScale,
    const Vector3D & wp,
    const Frustum & tFrustum
) {

    if (!(ptParticleSystem->dwFlags & (1 << PARTICLE_SYSTEM_PARTIALLY_VISIBLE_BIT))) {
        return TRUE;
    }


    Sphere wsBounds;
    wsBounds.Set(wp, flScale * 0.5f);
    return Intersect::FrustumAndSphere(tFrustum, wsBounds);
}




// Inference: empty body (the original is a single ret; every parameter is unused).
void JitterRenderedPosition(
    const ParticleSystem * ptParticleSystem,
    const ParticleDefinition * pdefParticle,
    ParticleParams & tParams,
    Vector3D & wpPos,
    float flDeltaTime
) {

}
// Inference: the constants 6400 and 7225 are 80 and 85 squared, and the alpha ramp is (d - 80) / -5 + 1, so
// Min = Max - 5.0f and Delta = Min - Max (negative). Which of the three locals is computed from which is not
// visible in the line table. DistanceSquared takes (wpParticle, wpPlayer): the original subtracts player - particle.















































static float sGetParticlePlayerDistanceAlpha(
    const Vector3D & wpPlayer,
    const Vector3D & wpParticle
) {
    float flAlpha = 1.0f;

    const float flClutterDistanceMax = CLUTTER_DISTANCE_MAX;
    const float flClutterDistanceMin = flClutterDistanceMax - 5.0f;
    const float flClutterDistanceDelta = flClutterDistanceMin - flClutterDistanceMax;


    float flDistSqr = Vector3D::DistanceSquared(wpParticle, wpPlayer);
    if (flDistSqr > flClutterDistanceMax * flClutterDistanceMax) {
        flAlpha = 0.0f;


    } else if (flDistSqr > flClutterDistanceMin * flClutterDistanceMin) {

        float flDist = Math::Sqrt(flDistSqr);
        flAlpha = 1.0f + (flDist - flClutterDistanceMin) / flClutterDistanceDelta;
    }

    return flAlpha;
}





uint8 UpdateParticleAlpha(
    ParticleSystem * ptParticleSystem,
    const float flBaseAlpha,
    const Vector3D & wpPlayerPos,
    const Vector3D & wpParticle
) {
    float flAlpha = flBaseAlpha;

    if (ptParticleSystem->eType == PSTYPE_CLUTTER_BILLBOARD || ptParticleSystem->eType == PSTYPE_CLUTTER_DYNAMIC) {


        flAlpha = flBaseAlpha * sGetParticlePlayerDistanceAlpha(wpPlayerPos, wpParticle);
    }
// Inference: 0.999999 is 1.0f - EPSILON; the layout (255 return placed last) means the test is `flAlpha < 1.0f - EPSILON`
// guarding the rounded return, with `return 255` after it.
    if (flAlpha < EPSILON) {
        return 0;
    }

    if (flAlpha < 1.0f - EPSILON)

        return (uint8)Math::FloatRoundToInt(flAlpha * 255.0f);

    return 255;



}
// Inferences for this file's orientation code (ParticleRenderUpdate.cpp:234-707). Each FromBasisVectors argument
// order is read from the trace (_11 + _22 + _33) and branch terms of the inlined Quaternion::FromMatrix.
// Lines 239/241/253: right = CrossProduct(cgwvZAxis, wvFacing); return when its magnitude < EPSILON; basis (facing, right, up).

static void sParticlesUpdateActorBillboardOrientation(
    Quaternion & qOrientation,
    const Vector3D & wvCameraForward
) {
    Vector3D wvFacing = -wvCameraForward;
    Vector3D wvRight = Vector3D::CrossProduct(cgwvZAxis, wvFacing);
    float wdMagRight = wvRight.Normalize();
    if (wdMagRight < EPSILON) {
        return;
    }


    Vector3D wvFacingUp = Vector3D::CrossProduct(wvFacing, wvRight);
    wvFacingUp.Normalize();





    qOrientation.FromBasisVectors(wvFacing, wvRight, wvFacingUp);
}
// Lines 264-281: the two MagnitudeSquared() < EPSILON tests are nested; cgwvXAxis (271) is the fallback when
// both vectors are short, wvLastMotionNormalized (275) when only wvMotion is; else copy + Normalize (280-281).


static void sGetNormalizedMotion(
    const Vector3D & wvMotion,
    const Vector3D & wvLastMotionNormalized,
    Vector3D & wvMotionNormalized
) {
    if (wvMotion.MagnitudeSquared() < EPSILON) {

        if (wvLastMotionNormalized.MagnitudeSquared() < EPSILON) {




            wvMotionNormalized = cgwvXAxis;
        } else {


            wvMotionNormalized = wvLastMotionNormalized;
        }


    } else {
        wvMotionNormalized = wvMotion;
        wvMotionNormalized.Normalize();
    }



}

static void sParticlesUpdateActorOrientation(
    const Vector3D & wvMotion,
    const Vector3D & wvLastMotionNormalized,
    Quaternion & qOrientation
) {
    Vector3D wvMotionNormalized;
    sGetNormalizedMotion(wvMotion, wvLastMotionNormalized, wvMotionNormalized);



    Vector3D wvRight = Vector3D::CrossProduct(cgwvZAxis, wvMotionNormalized);
    float flMag = wvRight.Normalize();
    if (flMag < EPSILON) {
        return;
    }


    Vector3D wvFacingUp = Vector3D::CrossProduct(wvRight, wvMotionNormalized);
    wvFacingUp.Normalize();

    qOrientation.FromChangeOfBasis(
        cgwvXAxis,
        cgwvYAxis,
        cgwvZAxis,
        wvMotionNormalized,
        wvRight,
        -wvFacingUp);
}
// Line 300/332/436: `if (flMag < EPSILON) return;` (fcmp + b.mi to the epilogue after the inlined Normalize).
// Lines 308/341: the FromChangeOfBasis source axes are (X, Y, Z) for actors and (Z, Y, X) for actor posters (GOT loads).


static void sParticlesUpdateActorPosterOrientation(
    const Vector3D & wvMotion,
    const Vector3D & wvLastMotionNormalized,
    Quaternion & qOrientation
) {
    Vector3D wvMotionNormalized;
    sGetNormalizedMotion(wvMotion, wvLastMotionNormalized, wvMotionNormalized);



    Vector3D wvRight = Vector3D::CrossProduct(cgwvZAxis, wvMotionNormalized);
    float flMag = wvRight.Normalize();
    if (flMag < EPSILON) {
        return;
    }



    Vector3D wvFacingUp = Vector3D::CrossProduct(wvRight, wvMotionNormalized);
    wvFacingUp.Normalize();

    qOrientation.FromChangeOfBasis(
        cgwvZAxis,
        cgwvYAxis,
        cgwvXAxis,
        wvMotionNormalized,
        wvRight,
        wvFacingUp);
}
// Line 381: FromBasisVectors(-wvFacingUp, wvMotionNormalized, wvFacing); line 412 (frisbee):
// (wvRight, wvMotionNormalized, wvUp); line 449 (poster): (-wvRight, wvFacingUp, wvMotionNormalized).


static void sParticlesUpdateArrowOrientation(
    const BOOL fHasActors,
    const Vector3D & wvLastMotionNormalized,
    const Vector3D & wvMotion,
    Quaternion & qOrientation,
    const Vector3D & wvCameraForward
) {
    if (fHasActors) {

        sParticlesUpdateActorOrientation(wvMotion, wvLastMotionNormalized, qOrientation);
        return;
    }

    Vector3D wvMotionNormalized;
    sGetNormalizedMotion(wvMotion, wvLastMotionNormalized, wvMotionNormalized);



    Vector3D wvFacingUp = Vector3D::CrossProduct(-wvCameraForward, wvMotionNormalized);
    wvFacingUp.Normalize();
    Vector3D wvFacing = Vector3D::CrossProduct(wvMotionNormalized, wvFacingUp);
    wvFacing.Normalize();






    qOrientation.FromBasisVectors(-wvFacingUp, wvMotionNormalized, wvFacing);
}




static void sParticlesUpdateFrisbeeOrientation(
    const BOOL fHasActors,
    const Vector3D & wvLastMotionNormalized,
    const Vector3D & wvMotion,
    Quaternion & qOrientation
) {
    if (fHasActors) {

        sParticlesUpdateActorOrientation(wvMotion, wvLastMotionNormalized, qOrientation);
        return;
    }

    Vector3D wvMotionNormalized;
    sGetNormalizedMotion(wvMotion, wvLastMotionNormalized, wvMotionNormalized);

    Vector3D wvRight = Vector3D::CrossProduct(wvMotionNormalized, cgwvZAxis);
    wvRight.Normalize();
    Vector3D wvUp = Vector3D::CrossProduct(wvRight, wvMotionNormalized);
    wvUp.Normalize();






    qOrientation.FromBasisVectors(wvRight, wvMotionNormalized, wvUp);
}




void ParticlesUpdatePosterOrientation(
    const BOOL fHasActors,
    const Vector3D & wvLastMotionNormalized,
    const Vector3D & wvMotion,
    Quaternion & qOrientation
) {
    if (fHasActors) {

        sParticlesUpdateActorPosterOrientation(wvMotion, wvLastMotionNormalized, qOrientation);
        return;
    }

    Vector3D wvMotionNormalized;
    sGetNormalizedMotion(wvMotion, wvLastMotionNormalized, wvMotionNormalized);


    Vector3D wvRight = Vector3D::CrossProduct(wvMotionNormalized, cgwvZAxis);
    float flMag = wvRight.Normalize();
    if (flMag < EPSILON) {
        return;
    }


    Vector3D wvFacingUp = Vector3D::CrossProduct(wvRight, wvMotionNormalized);
    wvFacingUp.Normalize();






    qOrientation.FromBasisVectors(-wvRight, wvFacingUp, wvMotionNormalized);
}
// Lines 462/483/497: the motion vectors for sphere/cylinder/cylinder-billboard are particle - system position
// (ptParticleSystem->wp, offset 0x40) and -camera forward with z cleared (stores of 0 to z in the original).


static void sParticlesUpdateSphereOrientation(
    const BOOL fHasActors,
    const Vector3D & wvLastMotionNormalized,
    const Vector3D & wpCenter,
    const Vector3D & wpParticle,
    Quaternion & qOrientation
) {
    Vector3D wvMotion = wpParticle - wpCenter;

    ParticlesUpdatePosterOrientation(fHasActors, wvLastMotionNormalized, wvMotion, qOrientation);
}




static void sParticlesUpdateCylinderOrientation(
    const BOOL fHasActors,
    const Vector3D & wvLastMotionNormalized,
    const Vector3D & wpCenter,
    const Vector3D & wpParticle,
    Quaternion & qOrientation
) {
    Vector3D wpCenterFlat(wpCenter);
    wpCenterFlat.z = 0.0f;

    Vector3D wpParticleFlat(wpParticle);
    wpParticleFlat.z = 0.0f;

    Vector3D wvMotion = wpParticleFlat - wpCenterFlat;

    ParticlesUpdatePosterOrientation(fHasActors, wvLastMotionNormalized, wvMotion, qOrientation);
}




static void sParticlesUpdateCylinderBillboardOrientation(
    const BOOL fHasActors,
    const Vector3D & wvLastMotionNormalized,
    Quaternion & qOrientation,
    const Vector3D & wvCameraForward
) {
    Vector3D wvCameraForwardInv = -wvCameraForward;
    wvCameraForwardInv.z = 0.0f;

    ParticlesUpdatePosterOrientation(fHasActors, wvLastMotionNormalized, wvCameraForwardInv, qOrientation);
}
// Line 513: wvFacing = (-forward.x, -forward.y, 0); the DWARF has no operator- inlined at 513 (so not
// `-wvCameraForward`). Lines 533-548: the actor branch (539) uses (facing, right, up), the other (548) (right, up, facing).


static void sParticlesUpdateCameraLockedXYOrientation(
    const BOOL fHasActors,
    Quaternion & qOrientation,
    const Vector3D & wvCameraForward
) {


    Vector3D wvFacing(-wvCameraForward.x, -wvCameraForward.y, 0.0f);

    float flMag = wvFacing.Normalize();
    if (flMag < EPSILON) {
        return;
    }



    Vector3D wvRight = Vector3D::CrossProduct(cgwvZAxis, wvFacing);
    float wdMagRight = wvRight.Normalize();
    if (wdMagRight < EPSILON) {
        return;
    }



    Vector3D wvFacingUp = Vector3D::CrossProduct(wvFacing, wvRight);
    wvFacingUp.Normalize();

    if (fHasActors) {





        qOrientation.FromBasisVectors(wvFacing, wvRight, wvFacingUp);







    } else {
        qOrientation.FromBasisVectors(wvRight, wvFacingUp, wvFacing);
    }
}
// Lines 562/603: actors copy the relative orientation (Quaternion::operator= inlined) and return.
// Lines 566-568 rotate cgwvXAxis, cgwvYAxis, cgwvZAxis in that order (constant pattern of each inlined Rotate).


static void sParticlesUpdateStaticOrientation(
    const BOOL fHasActors,
    const Quaternion & qRelativeOrientation,
    Quaternion & qOrientation
) {
    if (fHasActors) {

        qOrientation = qRelativeOrientation;
        return;
    }

    Vector3D wvForward = qRelativeOrientation.Rotate(cgwvXAxis);
    Vector3D wvRight = qRelativeOrientation.Rotate(cgwvYAxis);
    Vector3D wvUp = qRelativeOrientation.Rotate(cgwvZAxis);






    qOrientation.FromBasisVectors(wvRight, wvUp, wvForward);
}

// Line 591: flMag is assigned and not tested (no compare after the inlined Normalize in the original).
// Line 599: qChange * qRelativeOrientation (operand order of the inlined operator* products).


static void sParticlesUpdateGroundOrientation(
    const BOOL fHasActors,
    const Vector3D & wvGroundNormal,
    const Quaternion & qRelativeOrientation,
    Quaternion & qOrientation
) {


    Vector3D wvNormal(wvGroundNormal);
    float flMag = wvNormal.Normalize();



    Vector3D wvRelativeOrientation = qRelativeOrientation.Rotate(cgwvZAxis);
    Quaternion qChange;
    qChange.FromArc(wvRelativeOrientation, wvNormal);

    Quaternion qNew = qChange * qRelativeOrientation;

    if (fHasActors) {

        qOrientation = qNew;
        return;
    }

    Vector3D wvNewXAxis = qNew.Rotate(cgwvXAxis);
    Vector3D wvNewYAxis = qNew.Rotate(cgwvYAxis);
    Vector3D wvNewZAxis = qNew.Rotate(cgwvZAxis);





    qOrientation.FromBasisVectors(wvNewXAxis, wvNewYAxis, wvNewZAxis);
}
// UpdateParticleOrientation matches since the Quaternion::Rotate body in Quaternion.inl was reshaped (return r).




static void sParticlesUpdateSpikyBallOrientation(
    const BOOL fHasActors,
    const Vector3D & wvLastMotionNormalized,
    const Vector3D & wpCenter,
    const Vector3D & wpParticle,
    Quaternion & qOrientation,
    const Vector3D & wvCameraForward
) {
    Vector3D wvMotion = wpParticle - wpCenter;

    sParticlesUpdateArrowOrientation(fHasActors, wvLastMotionNormalized, wvMotion, qOrientation, wvCameraForward);
}
// Lines 650-705: case layout from the call lines (655, 660, 668 ... 701); PARTICLE_FACING_UP and FULL_SCREEN share
// the jump-table entry of the epilogue, written as an empty case at 663-665 (exact lines not in the DWARF).
// Line 705: default reports DisplayInternalError(705) + ExitProgram; the range test comes before fHasActors (650).

void UpdateParticleOrientation(
    ParticleSystem * ptParticleSystem,
    const ParticleDefinition * pdefParticle,
    Quaternion & qOrientation,
    const Vector3D & wvMotion,
    const Vector3D & wvLastMotionNormalized,
    const Quaternion & qRelativeOrientation,
    const Vector3D & wvGroundNormal,
    const Vector3D & wpParticle,
    const Vector3D & wvCameraForward
) {
    BOOL fHasActors = (ptParticleSystem->dwFlags >> PARTICLE_SYSTEM_HAS_ACTORS) & 1;
    switch (pdefParticle->nFacing) {

    case PARTICLE_FACING_BILLBOARD:
        if (fHasActors) {

            sParticlesUpdateActorBillboardOrientation(qOrientation, wvCameraForward);
        }
        break;

    case PARTICLE_FACING_CAMERA_LOCKED_TO_XY:
        sParticlesUpdateCameraLockedXYOrientation(fHasActors, qOrientation, wvCameraForward);
        break;

    case PARTICLE_FACING_UP:
    case PARTICLE_FACING_FULL_SCREEN:
        break;

    case PARTICLE_FACING_FRISBEE:
        sParticlesUpdateFrisbeeOrientation(fHasActors, wvLastMotionNormalized, wvMotion, qOrientation);
        break;

    case PARTICLE_FACING_SIDE_OF_ARROW:
        sParticlesUpdateArrowOrientation(fHasActors, wvLastMotionNormalized, wvMotion, qOrientation, wvCameraForward);
        break;

    case PARTICLE_FACING_POSTER_ON_ARROW:
        ParticlesUpdatePosterOrientation(fHasActors, wvLastMotionNormalized, wvMotion, qOrientation);
        break;

    case PARTICLE_FACING_CYLINDER:
        sParticlesUpdateCylinderOrientation(fHasActors, wvLastMotionNormalized, ptParticleSystem->wp, wpParticle, qOrientation);
        break;

    case PARTICLE_FACING_SPHERE:
        sParticlesUpdateSphereOrientation(fHasActors, wvLastMotionNormalized, ptParticleSystem->wp, wpParticle, qOrientation);
        break;

    case PARTICLE_FACING_CYLINDER_BILLBOARD:
        sParticlesUpdateCylinderBillboardOrientation(fHasActors, wvLastMotionNormalized, qOrientation, wvCameraForward);
        break;

    case PARTICLE_FACING_STATIC:
        sParticlesUpdateStaticOrientation(fHasActors, qRelativeOrientation, qOrientation);
        break;

    case PARTICLE_FACING_GROUND:
    case PARTICLE_FACING_GROUND_PROJECTION:
        sParticlesUpdateGroundOrientation(fHasActors, wvGroundNormal, qRelativeOrientation, qOrientation);
        break;

    case PARTICLE_FACING_SPIKY_BALL:
        sParticlesUpdateSpikyBallOrientation(fHasActors, wvLastMotionNormalized, ptParticleSystem->wp, wpParticle, qOrientation, wvCameraForward);
        break;

    default:
        DisplayInternalError(NULL, "", 705, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}
// sGetUVCoordsForTexture: the switch on 734 is a range test 2..4 (rotate/translate-scale/rotate-translate) then == 1.
// [2] and [3] of each case share one tail in the original, so 754-755 and 768-769 leave no inlined Set of their own.


static void sGetUVCoordsForTexture(
    Vector2D * pwvUV,
    const TexFrame & tFrame,

    const CHAR * pTexAnimCompParams,
    uint32 dwTexAnimCompParamsSize,



    const TexAnimState * ptTexAnimState,
    const TextureFramesInfo & tTexFramesInfo
) {
    float fFrameUDiff = tFrame.flU1 - tFrame.flU0;
    float fFrameVDiff = tFrame.flV1 - tFrame.flV0;

    Matrix4x4 mTex;

    TexAnimMatrixType eMatrixType = TexAnimGetTexMatrixForBillboard(mTex, pTexAnimCompParams, dwTexAnimCompParamsSize, ptTexAnimState, tTexFramesInfo);




    switch (eMatrixType) {

        case TAMT_ROTATE_ONLY:
        case TAMT_TRANSLATE_SCALE:
        case TAMT_ROTATE_TRANSLATE:







        {
            float fUDiff11 = fFrameUDiff * mTex._11;
            float fUDiff12 = fFrameUDiff * mTex._12;
            float fVDiff21 = fFrameVDiff * mTex._21;
            float fVDiff22 = fFrameVDiff * mTex._22;

            pwvUV[0].Set(fUDiff11 + fVDiff21 + mTex._41, fUDiff12 + fVDiff22 + mTex._42);
            pwvUV[1].Set(fVDiff21 + mTex._41, fVDiff22 + mTex._42);
            pwvUV[2].Set(fUDiff11 + mTex._41, fUDiff12 + mTex._42);
            pwvUV[3].Set(mTex._41, mTex._42);
            break;
        }

        case TAMT_TRANSLATE_ONLY:
        {
            Vector3D wvTranslate = mTex.ExtractTranslation();

            float fUDiffTrans = fFrameUDiff + wvTranslate.x;
            float fVDiffTrans = fFrameVDiff + wvTranslate.y;

            pwvUV[0].Set(fUDiffTrans, fVDiffTrans);
            pwvUV[1].Set(wvTranslate.x, fVDiffTrans);
            pwvUV[2].Set(fUDiffTrans, wvTranslate.y);
            pwvUV[3].Set(wvTranslate.x, wvTranslate.y);
            break;
        }
        default:
            pwvUV[0].Set(fFrameUDiff, fFrameVDiff);
            pwvUV[1].Set(0.0f, fFrameVDiff);
            pwvUV[2].Set(fFrameUDiff, 0.0f);
            pwvUV[3].Set(0.0f, 0.0f);
            break;
    }
}

































































































// sParticleBillboardGenerateVertexData: per vertex the line table gives wpPos, vcAuxColor (column 30 = the `=`) and
// wpOffsetAngle/qBillboard; vcDiffuse sits on the line in between (its store merges with wpPos.z). Vertices 2 and 3
// add wvFoliageOffset when fFoliage (926/939); both arms' stores share one tail. 950-964: the UV loop.

static void sParticleBillboardGenerateVertexData(
    Angle aAng,
    VertexColor vcDiffuse,
    VertexColor vcAlphaAux,
    const Vector3D & wp,
    UByte4 packqBillboard,
    float wdWidth,
    float wdHeight,
    Vector2D (*wvUV)[4],
    BOOL fFoliage,
    const Vector3D & wvFoliageOffset,
    VertexBillboardParticle * ptVertices,


    const ParticleQuad * ptParticleQuadPos,
    const ParticleQuad * ptParticleQuadUv
) {












    ptVertices[0].wpPos.Set(ptParticleQuadPos->flShrinkMaxX * wdWidth, ptParticleQuadPos->flShrinkMinY * -wdHeight, 0.0f);
    ptVertices[0].vcDiffuse = vcDiffuse;
    ptVertices[0].vcAuxColor = vcAlphaAux;
    ptVertices[0].wpOffsetAngle.Set(wp.x, wp.y, wp.z, aAng);
    ptVertices[0].qBillboard = packqBillboard;

    ptVertices[1].wpPos.Set(ptParticleQuadPos->flShrinkMinX * -wdWidth, ptParticleQuadPos->flShrinkMinY * -wdHeight, 0.0f);
    ptVertices[1].vcDiffuse = vcDiffuse;
    ptVertices[1].vcAuxColor = vcAlphaAux;
    ptVertices[1].wpOffsetAngle.Set(wp.x, wp.y, wp.z, aAng);
    ptVertices[1].qBillboard = packqBillboard;

    ptVertices[2].wpPos.Set(ptParticleQuadPos->flShrinkMaxX * wdWidth, ptParticleQuadPos->flShrinkMaxY * wdHeight, 0.0f);
    ptVertices[2].vcDiffuse = vcDiffuse;
    ptVertices[2].vcAuxColor = vcAlphaAux;
    ptVertices[2].qBillboard = packqBillboard;
    if (fFoliage)
    {
        ptVertices[2].wpOffsetAngle.Set(wp.x + wvFoliageOffset.x, wp.y + wvFoliageOffset.y, wp.z + wvFoliageOffset.z, aAng);
    }
    else
    {
        ptVertices[2].wpOffsetAngle.Set(wp.x, wp.y, wp.z, aAng);
    }

    ptVertices[3].wpPos.Set(ptParticleQuadPos->flShrinkMinX * -wdWidth, ptParticleQuadPos->flShrinkMaxY * wdHeight, 0.0f);
    ptVertices[3].vcDiffuse = vcDiffuse;
    ptVertices[3].vcAuxColor = vcAlphaAux;
    ptVertices[3].qBillboard = packqBillboard;
    if (fFoliage)
    {
        ptVertices[3].wpOffsetAngle.Set(wp.x + wvFoliageOffset.x, wp.y + wvFoliageOffset.y, wp.z + wvFoliageOffset.z, aAng);
    }
    else
    {
        ptVertices[3].wpOffsetAngle.Set(wp.x, wp.y, wp.z, aAng);
    }



    for (int32 i = 0; i < 4; i++) {


        if (ptParticleQuadUv) {

            ptVertices[i].vpUV0 = Vector2DTexCoordToUByte4(Vector2D((i & 1) ? ptParticleQuadUv->flShrinkMinX : ptParticleQuadUv->flShrinkMaxX, (i & 2) ? ptParticleQuadUv->flShrinkMaxY : ptParticleQuadUv->flShrinkMinY));


        } else {

            ptVertices[i].vpUV0 = Vector2DTexCoordToUByte4(wvUV[0][i]);
        }
        ptVertices[i].vpUV1 = Vector2DTexCoordToUByte4(wvUV[1][i]);
        ptVertices[i].vpUV2 = Vector2DTexCoordToUByte4(wvUV[2][i]);
        ptVertices[i].vpUV3 = Vector2DTexCoordToUByte4(wvUV[3][i]);
    }
}






// ParticleBillboardVerticesSubmit inferences: 1010 is one condition (columns 34/81 are the first && and dwFrameCount);
// 1013: the AnimationComputeCurrentFrame result is unused and Pin (1014) clamps 0 (`and w8, w8, w8, asr #31` = min(n-1, 0));
// the call at column 27 fits `int32 nFrame = 0; ` before it. 1016 column order: height*V over width*U.
// 1045/1065 test the same pair with the defaults (1050-1056, 1070-1073) skipping the call at 1079.


void ParticleBillboardVerticesSubmit(
    ParticleSystemRenderUpdateParams & tParams,
    const Vector3D & wpParticle,
    const Quaternion & qOrientation,
    VertexBillboardParticle * ptVertices,
    const TextureFramesInfo * atTexFramesInfo,


    const blz::vector<ParticleQuad, blz::allocator<ParticleQuad> > & rtParticleQuadPos,
    const blz::vector<ParticleQuad, blz::allocator<ParticleQuad> > & rtParticleQuadUv
)

{
    const TexAnimState * ptTexAnimState = tParams.tTexAnimState[0];
    SNO snoTex = tParams.snoTexAnimStateTex[0];
    const TextureFramesInfo & tTextureFramesInfo = atTexFramesInfo[0];

    float flInvAspectRatio = 1.0f;
    const TexFrame * ptBillboardFrame = &cgtTexFrameDefault;




    const bool bHaveParticleQuads = rtParticleQuadPos.size() > 0;

    const bool bShouldParticleTrim = bHaveParticleQuads && gbParticleTrim;

    const ParticleQuad * ptParticleQuadPos = bShouldParticleTrim ? &rtParticleQuadPos[0] : &tDefaultParticleQuadPos;
    const ParticleQuad * ptParticleQuadUv = bShouldParticleTrim ? &rtParticleQuadUv[0] : NULL;


    if ((ptTexAnimState != NULL) && (snoTex != SNO_NONE) && (tTextureFramesInfo.dwFrameCount > 0)) {

        const TexAnimState & tTexAnimState = *ptTexAnimState;
        int32 nFrame = 0; AnimationComputeCurrentFrame(tTexAnimState.tAnimState);
        nFrame = Math::Pin(nFrame, 0, (int32)tTextureFramesInfo.dwFrameCount - 1);
        TexFrame& tFrame = tTextureFramesInfo.ptTexFrames[nFrame];
        flInvAspectRatio = (tTextureFramesInfo.dwHeight * (tFrame.flV1 - tFrame.flV0)) / (tTextureFramesInfo.dwWidth * (tFrame.flU1 - tFrame.flU0));

        ptBillboardFrame = &tFrame;

        if (bShouldParticleTrim) {

            ptParticleQuadPos = &rtParticleQuadPos[nFrame];
            ptParticleQuadUv = &rtParticleQuadUv[nFrame];
        }
    }


    Vector2D wvUV[4][4];
    float flSize = 0.5f * tParams.flScale;







    for (int32 i = 0; i < tParams.nTexStageCount; i++) {

        const TexFrame * ptFrame;

        if (i == 0) {



            if (tParams.pTexAnimCompParams[i] == NULL || tParams.tTexAnimState[i] == NULL) {




                float fFrameUDiff = ptBillboardFrame->flU1 - ptBillboardFrame->flU0;
                float fFrameVDiff = ptBillboardFrame->flV1 - ptBillboardFrame->flV0;

                wvUV[i][0].Set(fFrameUDiff, fFrameVDiff);
                wvUV[i][1].Set(0.0f, fFrameVDiff);
                wvUV[i][2].Set(fFrameUDiff, 0.0f);
                wvUV[i][3].Set(0.0f, 0.0f);
                continue;
            }
            ptFrame = ptBillboardFrame;
        } else {




            if (tParams.pTexAnimCompParams[i] == NULL || tParams.tTexAnimState[i] == NULL) {




                wvUV[i][0].Set(1.0f, 1.0f);
                wvUV[i][1].Set(0.0f, 1.0f);
                wvUV[i][2].Set(1.0f, 0.0f);
                wvUV[i][3].Set(0.0f, 0.0f);
                continue;
            }
            ptFrame = &cgtTexFrameDefault;
        }

        sGetUVCoordsForTexture(wvUV[i], *ptFrame, tParams.pTexAnimCompParams[i], tParams.dwTexAnimCompParamsSize[i], tParams.tTexAnimState[i], atTexFramesInfo[i]);
    }




    sParticleBillboardGenerateVertexData(tParams.aAng,

        VertexColor(tParams.rgbaColor), VertexColor(tParams.rgbaAlphaAux),
        wpParticle,
        sUnitQuaternionToUByte4(qOrientation),
        flSize,
        flInvAspectRatio * flSize * tParams.flStretch,
        wvUV,
        tParams.dwFlags & (1 << BILLBOARD_FOLIAGE_BIT),
        tParams.wvFoliageOffset,
        ptVertices,
        ptParticleQuadPos,
        ptParticleQuadUv);

















}
// sParticleGenerateVertices: 1137 is `(eRenderLayer != PROJECTED_ADDITIVE) &&` (column 76 = the &&), the on-screen
// test on 1138; both early exits clear the four vertices (one shared memset, line 0). 1168 column 25 = the `==`.


static void sParticleGenerateVertices(
    ParticleSystem * ptParticleSystem,
    VertexBillboardParticle * ptVertices,
    Particle * ptParticle,
    ParticleSystemRenderUpdateParams & tParams,
    const Vector3D & wpPlayer,
    const Frustum & tFrustum,
    const Vector3D & wvCameraForward,
    const TextureFramesInfo * atTexFramesInfo,
    float flDeltaTime
) {
    const ParticleDefinition * pdefParticle = ptParticleSystem->GetParticleDef();

    Vector3D wpParticle = ptParticle->wp;
    // Inference: line not in the DWARF (empty inline leaves no DIE); flDeltaTime reaches this function, so it has a use.
    JitterRenderedPosition(ptParticleSystem, pdefParticle, ptParticle->tParams, wpParticle, flDeltaTime);

    if ((ptParticleSystem->eRenderLayer != RENDERLAYER_PROJECTED_ADDITIVE) &&
         !ParticleIsOnScreen(ptParticleSystem, ptParticle->tParams.flScale, wpParticle, tFrustum)) {

        memset(ptVertices, 0, sizeof(VertexBillboardParticle) * 4);
        return;
    }



    uint8 dwParticleAlpha = UpdateParticleAlpha(ptParticleSystem, ptParticle->tParams.flAlpha, wpPlayer, wpParticle);
    ptParticle->tParams.rgbaColor.a = dwParticleAlpha;


    UpdateGroundNormal(pdefParticle, wpParticle, ptParticle->tParams);


    UpdateParticleOrientation(
        ptParticleSystem,
        pdefParticle,
        ptParticle->tParams.qOrientation,
        ptParticle->tParams.wvMotion,
        ptParticle->tParams.wvLastMotionNormalized,
        ptParticle->tParams.qRelativeOrientation,



        ptParticle->tParams.wvGroundNormal,
        wpParticle,
        wvCameraForward);


    if (dwParticleAlpha == 0) {

        memset(ptVertices, 0, sizeof(VertexBillboardParticle) * 4);
        return;
    }












    tParams.flScale = ptParticle->tParams.flScale;
    tParams.flStretch = ptParticle->tParams.fStretch;
    for (int32 i = 0; i < tParams.nTexStageCount; i++) {

        tParams.tTexAnimState[i] = ptParticle->tTexAnimState[i].snoTex > 0 ? &ptParticle->tTexAnimState[i] : NULL;
        tParams.snoTexAnimStateTex[i] = ptParticle->tTexAnimState[i].snoTex;
    }
    tParams.rgbaColor = ptParticle->tParams.rgbaColor;
    tParams.rgbaAlphaAux = ptParticle->tParams.rgbaAlphaAux;
    tParams.aAng = ptParticle->tParams.aAng;
    Vector2D wvFoliageOffset2D = ptParticle->tParams.tFoliageState.wvOffset;
    tParams.wvFoliageOffset.Set(wvFoliageOffset2D.x, wvFoliageOffset2D.y, 0.0f);

    ParticleBillboardVerticesSubmit(tParams,
        wpParticle,
        ptParticle->tParams.qOrientation,
        ptVertices,
        atTexFramesInfo,


        ptParticleSystem->tParticleQuads,
        ptParticleSystem->tParticleQuadUVs);


}




void ParticleSystemGenerateVertices(
    ParticleSystem * ptParticleSystem,
    VertexBillboardParticle * ptVertices,
    ParticleSystemRenderUpdateParams & tParams,
    const Vector3D & wpPlayer,
    const Frustum & tFrustum,
    const Vector3D & wvCameraForward,
    const TextureFramesInfo * atTexFramesInfo,
    float flDeltaTime
)
{
    Particle ** pParticleArray = ptParticleSystem->arCameraSortedParticles;
    int32 nCount = ptParticleSystem->tParticleList.size();
    if (pParticleArray) {

        for (int32 i = 0; i < nCount; i++) {

            Particle * ptParticle = pParticleArray[i];
            sParticleGenerateVertices(ptParticleSystem, ptVertices, ptParticle, tParams, wpPlayer, tFrustum, wvCameraForward, atTexFramesInfo, flDeltaTime);
            ptVertices += 4;
        }


    } else {

        for (int32 pos = 0; pos < nCount; pos++) {

            Particle * ptParticle = ptParticleSystem->tParticleList[pos];
            sParticleGenerateVertices(ptParticleSystem, ptVertices, ptParticle, tParams, wpPlayer, tFrustum, wvCameraForward, atTexFramesInfo, flDeltaTime);
            ptVertices += 4;
        }
    }
}
