// Original file: X/Common/DetPaths.cpp
// Deterministic projectile paths (charged bolt, plague of toads, chakram, sin wave, spirals, pie wedge, firestorm).

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "DetPaths.h"
#include "ActorCommonData.h"
#include "AnimPlayer.h"
#include "Game.h"
#include "WorldPlace.h"
#include "WorldCommonCollision.h"
#include "Movement/ActorMovement.h"
#include "Attributes/FastAttribConstantsSwig.h"
#include "TagMaps/AnimTagsSwig.h"
#include "../Client/CGame.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Math/Quaternion.h"
#include "../Sigma/Random.h"
#include "../Sigma/ErrorManager/ErrorManager.h"













static void sChargedBoltStampPosition(
    ActorMovement & tMove,
    DeterministicPathData & tDPath,
    Angle aAng)
{
    WorldPlace placeCurr;
    tMove.GetPlace(placeCurr);

    tDPath.flVar0 = 0.0f;
    tDPath.flVar1 = aAng;
    tDPath.flVar2 = placeCurr.wp.x;
    tDPath.flVar3 = placeCurr.wp.y;
}

static void sChargedBoltStart(
    ActorMovement & tMove,
    DeterministicPathData & tDPath)
{
    if (tMove.IsServerProjectile())
    {
        sChargedBoltStampPosition(tMove, tDPath, tDPath.aOriginalYaw);
        tDPath.nVar0 = 1;
    }
}



static BOOL sChargedBoltUpdate(
    ActorMovement & tMove,
    Vector3D & vVel,
    DeterministicPathData & tDPath,
    float flUpdateTime)
{



    flUpdateTime /= tDPath.flSpeedMult;

    if (flUpdateTime < EPSILON)
    {
        return FALSE;
    }

    tDPath.flVar0 += flUpdateTime;
    float flUnitTravel = tDPath.flVar0 * 0.05f;  // re-aims every 20 ticks; column 41 is one past the '*' (spelling unknown)
    if (tDPath.nVar0)
    {
        flUnitTravel = 1.0f;
        tDPath.nVar0 = 0;
    }

    if (flUnitTravel >= 1.0f)
    {
        float aAng = RandRangeFloat_Internal(tDPath.tSeed, -0.39269909f, EIGHTH_CIRCLE);  // [-PI / 8, PI / 8)
        if (aAng < 0.0f)
        {
            aAng -= 0.39269909f;  // PI / 8 (table 0xe3cc28 indexed by aAng < 0)
        }
        else
        {
            aAng += 0.39269909f;
        }
        aAng = Math::NormalizeAngle(tDPath.aOriginalYaw + aAng);

        sChargedBoltStampPosition(tMove, tDPath, aAng);

        if (!tMove.IsServerProjectile())
        {
            flUnitTravel = 0.0f;
        }
    }

    flUnitTravel = Math::Pin(flUnitTravel, 0.0f, 1.0f);
    flUnitTravel = 1.0f - flUnitTravel;
    flUnitTravel = flUnitTravel * flUnitTravel * flUnitTravel;
    flUnitTravel = 1.0f - flUnitTravel;

    float flTotalDist = tMove.GetDesiredSpeed() * tDPath.flSpeedMult * 20.0f;
    float flDistTravel = flTotalDist * flUnitTravel;

    Vector3D vStart(tDPath.flVar2, tDPath.flVar3, 0.0f);
    Vector3D vDir(Math::Cos(tDPath.flVar1), Math::Sin(tDPath.flVar1), 0.0f);
    Vector3D vDelta = vDir * flDistTravel;

    WorldPlace placeCurr;
    tMove.GetPlace(placeCurr);

    Vector3D vDest = vStart + vDelta;
    vVel = vDest - placeCurr.wp;
    vVel.z = 0.0f;

    vVel *= 1.0f / flUpdateTime;

    return TRUE;
}



enum ToadFlags
{
    TOAD_FLAG_HOPPING,
    TOAD_FLAG_USE_STRAIGHT_MOVEMENT,
    TOAD_FLAG_USE_NEGATIVE_ANGLE,
    TOAD_FLAG_MANUALLY_AIMED,
    TOAD_FLAG_COUNT
};













static void sPlagueOfToadsChooseRandomAngle(
    DeterministicPathData & tDPath)
{
    if (tDPath.nVar0 & (1 << TOAD_FLAG_MANUALLY_AIMED))
    {
        tDPath.nVar0 &= ~(1 << TOAD_FLAG_MANUALLY_AIMED);
        tDPath.flVar2 = 0.0f;
    }
    else if (tDPath.nVar0 & (1 << TOAD_FLAG_USE_STRAIGHT_MOVEMENT))
    {
        // Straight toads alternate between the two sides of the aim direction.
        BOOL bUseNegAngle = tDPath.nVar0 & (1 << TOAD_FLAG_USE_NEGATIVE_ANGLE);
        if (bUseNegAngle)
        {
            tDPath.flVar2 = -0.39269909f;  // -PI / 8 (data 0xbec90fdb)
        }
        else
        {
            tDPath.flVar2 = 0.39269909f;  // PI / 8 (data 0x3ec90fdb)
        }
        // every row of line 173 is at column 9 (and/orr/csel/str): one statement in the original
        if (!bUseNegAngle) tDPath.nVar0 |= (1 << TOAD_FLAG_USE_NEGATIVE_ANGLE); else tDPath.nVar0 &= ~(1 << TOAD_FLAG_USE_NEGATIVE_ANGLE);
    }
    else
    {
        tDPath.flVar2 = RandRangeFloat_Internal(tDPath.tSeed, -1.1780972f, 2.3561945f);  // [-3 PI / 8, 3 PI / 8]
    }

    tDPath.flVar2 = Math::NormalizeAngle(tDPath.aOriginalYaw + tDPath.flVar2);
}


static void sPlagueOfToadsStartHopAnim(
    ActorMovement & tMove,
    DeterministicPathData & tDPath)
{



    tMove.SetNewFacing(tDPath.flVar2, FALSE, FALSE);



    ACDID idACD = tMove.GetACDID();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD && tDPath.flVar0 >= EPSILON && GameActiveGameCommonDataIsClient())
    {
        AnimationTiming tAnimTiming;
        AnimPlayerGetAnimationTiming(*ptACD, TAG_ACTOR_ANIMATION_WALK, ANIM_USE_ANIM_LENGTH, 1.0f, CGameGlobalsGet().rngLocal, tAnimTiming);
        float flAnimSpeedMult = (float)tAnimTiming.dwCycleTime / tDPath.flVar0;

        AnimPlayerGetAnimationTiming(*ptACD, TAG_ACTOR_ANIMATION_WALK, ANIM_USE_ANIM_LENGTH, flAnimSpeedMult, CGameGlobalsGet().rngLocal, tAnimTiming);

        AnimPlayerPlayAnimationsWithTiming(*ptACD, &tAnimTiming, 1, FALSE, ANIM_REASON_IDLE, FALSE, FALSE);
    }







}


static void sPlagueOfToadsStartHop(
    ActorMovement & tMove,
    DeterministicPathData & tDPath)
{

    sPlagueOfToadsChooseRandomAngle(tDPath);


    tDPath.flVar4 = tDPath.flVar3 * tDPath.flSpeedMult;


    tDPath.nVar0 |= (1 << TOAD_FLAG_HOPPING);


    tDPath.flVar1 = (float)RandRange_Internal(tDPath.tSeed, 25, 10);  // 25 + Rand % 11: a hop of 25 to 35 ticks
    tDPath.flVar0 += tDPath.flVar1;


    sPlagueOfToadsStartHopAnim(tMove, tDPath);
}


static void sPlagueOfToadsStartIdleAnim(
    ActorMovement & tMove)
{




    ACDID idACD = tMove.GetACDID();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD && GameActiveGameCommonDataIsClient())
    {
        AnimPlayerPlayIdle(*ptACD);
    }
}



static void sPlagueOfToadsStartIdle(
    ActorMovement & tMove,
    DeterministicPathData & tDPath)
{
    // Land the hop: advance the start position by the hop just finished.
    Quaternion qYaw;
    qYaw.FromYaw(tDPath.flVar2);
    Vector3D vMovementDir = qYaw.Rotate(cgwvXAxis);

    tDPath.vStartPos += vMovementDir * tDPath.flVar1 * tDPath.flVar4;

    // line 268 is all at column 5 (ldr/and/str): probably a bit macro in the original
    tDPath.nVar0 &= ~(1 << TOAD_FLAG_HOPPING);



    float flIdleTicks = (float)RandRange_Internal(tDPath.tSeed, 2, 9);
    tDPath.flVar0 += flIdleTicks;


    sPlagueOfToadsStartIdleAnim(tMove);


    tMove.ProjToadIdleResponse();
}


static void sPlagueOfToadsStartStraight(
    DeterministicPathData & tDPath)
{
    tDPath.nVar0 |= (1 << TOAD_FLAG_USE_STRAIGHT_MOVEMENT);
}


static void sPlagueOfToadsStart(
    ActorMovement & tMove,
    DeterministicPathData & tDPath)
{
    if (tMove.IsServerProjectile())
    {

        tDPath.flVar3 = tMove.GetDesiredSpeed();


        sPlagueOfToadsStartHop(tMove, tDPath);
    }
    else
    {



        if (tDPath.nVar0 & (1 << TOAD_FLAG_HOPPING))
        {
            sPlagueOfToadsStartHopAnim(tMove, tDPath);
        }
        else
        {
            sPlagueOfToadsStartIdleAnim(tMove);
        }
    }
}



static BOOL sPlagueOfToadsUpdate(
    ActorMovement & tMove,
    Vector3D & vVel,
    DeterministicPathData & tDPath,
    float flUpdateTime)
{








    flUpdateTime /= tDPath.flSpeedMult;

    ACDID idACD = tMove.GetACDID();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return FALSE;
    }


    tDPath.flVar0 -= flUpdateTime;
    while (tDPath.flVar0 <= 0.0f)
    {
        if (tDPath.nVar0 & (1 << TOAD_FLAG_HOPPING))
        {
            sPlagueOfToadsStartIdle(tMove, tDPath);
        }
        else
        {
            sPlagueOfToadsStartHop(tMove, tDPath);
        }
    }


    Quaternion qYaw;
    qYaw.FromYaw(tDPath.flVar2);
    Vector3D vMovementDir = qYaw.Rotate(cgwvXAxis);


    float flHopTimeElapsed = 0.0f;
    if (tDPath.nVar0 & (1 << TOAD_FLAG_HOPPING))
    {
        flHopTimeElapsed = tDPath.flVar1 - tDPath.flVar0;
    }


    WorldPlace wpCurrent;
    tMove.GetPlace(wpCurrent);

    Vector3D vWorldDest = tDPath.vStartPos + vMovementDir * flHopTimeElapsed * tDPath.flVar4;
    vVel = vWorldDest - wpCurrent.wp;
    vVel.z = 0.0f;

    vVel *= 1.0f / flUpdateTime;

    return TRUE;
}


static BOOL sPlagueOfToadsNavReflect(
    ActorMovement & tMove,
    DeterministicPathData & tDPath,
    const WorldCollResult & tCollisionResult)
{


    if (tMove.IsServerProjectile())
    {
        ACDID idACD = tMove.GetACDID();
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (!ptACD)
        {
            return FALSE;
        }

        int32 nReflectCount = ptACD->AttributesGetInt(ATTRIBUTE_PROJECTILE_DET_PATH_REFLECT_COUNT);
        if (nReflectCount > 3)  // `cmp w0, #3; b.gt` (DetPaths.cpp:400); a named maximum may stand here
        {
            return FALSE;
        }

        ptACD->AttributesSetInt(ATTRIBUTE_PROJECTILE_DET_PATH_REFLECT_COUNT, nReflectCount + 1);
    }



    Quaternion qYaw;
    qYaw.FromYaw(tDPath.flVar2);
    Vector3D vMovementDir = qYaw.Rotate(cgwvXAxis);


    Vector3D vStartPosReflectNormal = tCollisionResult.wvNormal;
    if (tDPath.nVar0 & (1 << TOAD_FLAG_HOPPING))
    {

        float flPreReflectHopDist = Vector3D::DotProduct(vMovementDir, tCollisionResult.wpContact - tDPath.vStartPos);
        float flPreReflectHopTimeElapsed = flPreReflectHopDist / tDPath.flVar4;
        tDPath.flVar0 = tDPath.flVar1 - flPreReflectHopTimeElapsed;
    }
    else
    {




        vStartPosReflectNormal = -vStartPosReflectNormal;
    }


    WorldPlace wpCurrent;
    tMove.GetPlace(wpCurrent);
    Vector3D vDirToCollision = wpCurrent.wp - tDPath.vStartPos;
    Vector3D vDirToCollisionReflected = -Vector3D::Reflect(vDirToCollision, vStartPosReflectNormal);  // the negation folds into line 437 (fadd), so it leaves no inline record
    tDPath.vStartPos = wpCurrent.wp - vDirToCollisionReflected;


    vMovementDir = -Vector3D::Reflect(vMovementDir, tCollisionResult.wvNormal);
    tDPath.flVar2 = vMovementDir.As2D().ToYaw();  // Vector2D::ToYaw per the DWARF; As2D emits no code


    tMove.SetNewFacing(tDPath.flVar2, TRUE, FALSE);


    qYaw.FromYaw(tDPath.aOriginalYaw);
    Vector3D vOriginalYawDir = qYaw.Rotate(cgwvXAxis);
    vOriginalYawDir = -Vector3D::Reflect(vOriginalYawDir, tCollisionResult.wvNormal);
    tDPath.aOriginalYaw = vOriginalYawDir.As2D().ToYaw();

    return TRUE;



}
static BOOL sSinWaveUpdate(
    ActorMovement & tMove,
    Vector3D & vVel,
    DeterministicPathData & tDPath,
    float flUpdateTime)
{
    ACDID idACD = tMove.GetACDID();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return FALSE;
    }

    if (flUpdateTime < EPSILON)
    {
        return FALSE;
    }

    if (tDPath.nVar0 == 0)
    {
        // nVar0 picks the side the wave starts on (2: phase shifted by half a turn)
        if (Rand_Internal(tDPath.tSeed, 1) == 0)
        {
            tDPath.nVar0 = 2;
        }
        else
        {
            tDPath.nVar0 = 1;
        }
    }

    DPathSinData & tSinData = tDPath.tSinData;
    tDPath.flVar0 += flUpdateTime;

    float flCurrentAngle = tDPath.flVar0 * tSinData.flSinIncPerTick;
    if (tDPath.nVar0 == 2)
    {
        flCurrentAngle += PI;
    }

    float flCurrentOffset = tDPath.flVar0 * tSinData.flSpeed;

    float flUnitLateral = (flCurrentOffset - tSinData.flLateralStartDistance) * tSinData.flOOLateralDistanceToScale;
    flUnitLateral = Math::Pin(flUnitLateral, 0.0f, 1.0f);

    float flLateralOffset = Math::Sin(flCurrentAngle)
                    * (tSinData.flLateralMaxDistance * flUnitLateral);

    WorldPlace placeCurr = ptACD->GetPlace();
    Vector3D vLocalForward(flCurrentOffset, flLateralOffset, 0.0f);

    Quaternion qYaw;
    qYaw.FromYaw(tDPath.aOriginalYaw);
    Vector3D vWorldForward = qYaw.Rotate(vLocalForward);

    Vector3D vWorldDest = tDPath.vStartPos + vWorldForward;
    vVel = vWorldDest - placeCurr.wp;
    vVel.z = 0.0f;

    vVel *= tDPath.flSpeedMult / flUpdateTime;

    return TRUE;
}







static BOOL sChakramUpdate(
    ActorMovement & tMove,
    Vector3D & vVel,
    DeterministicPathData & tDPath,
    float flUpdateTime,
    BOOL bRight)
{
    ACDID idACD = tMove.GetACDID();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return FALSE;
    }

    if (flUpdateTime < EPSILON)
    {
        return FALSE;
    }
    // 0.1f (data 0x3dcccccd at 0xe3be9c)
    float flSinInc = flUpdateTime * 0.1f;
    if (bRight)
    {
        tDPath.flVar0 -= flSinInc;
    }
    else
    {
        tDPath.flVar0 += flSinInc;
    }

    tDPath.flVar1 += flUpdateTime * 0.25f;

    float flAmplitude = (log(tDPath.flVar1) + 1.5f) * 2.5f;
    float flSinIdx = tDPath.flVar0 + PI;
    Vector3D vLocalPos(Math::Cos(flSinIdx), Math::Sin(flSinIdx), 0.0f);
    vLocalPos *= flAmplitude;
    // line 564 is header code only; cgwvXAxis is loaded (the multiplies by 1 and 0 are kept)
    tDPath.flVar2 += flUpdateTime * 0.4f;
    vLocalPos += cgwvXAxis * tDPath.flVar2;

    Quaternion qYaw;
    qYaw.FromYaw(tDPath.aOriginalYaw);
    Vector3D vLocalRotatedPos = qYaw.Rotate(vLocalPos);

    Vector3D vWorldPos = tDPath.vStartPos + vLocalRotatedPos;
    WorldPlace placeCurr = ptACD->GetPlace();
    vVel = vWorldPos - placeCurr.wp;
    vVel.z = 0.0f;

    vVel *= tDPath.flSpeedMult / flUpdateTime;

    return TRUE;
}



static BOOL sPedalCurveUpdate(
    ActorMovement & tMove,
    Vector3D & vVel,
    DeterministicPathData & tDPath,
    float flUpdateTime)
{
    ACDID idACD = tMove.GetACDID();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return FALSE;
    }

    if (flUpdateTime < EPSILON)
    {
        return FALSE;
    }

    float flSinInc = flUpdateTime * tDPath.tSinData.flSinIncPerTick;
    tDPath.flVar0 += flSinInc;

    float flCos = Math::Cos(tDPath.flVar0 + PI);
    float flSin = Math::Sin(tDPath.flVar0 + PI);
    Vector3D vLocalPos;
    vLocalPos.x = 1 + (1 - flCos) * flCos;  // integer literals: only "1 + (1 - flCos)" fits columns 19/21/24/26
    vLocalPos.y = 0 + (1 - flCos) * flSin;
    vLocalPos.z = 0;

    vLocalPos.x += 1;
    vLocalPos.y *= tDPath.tSinData.flLateralStartDistance;
    vLocalPos *= tDPath.tSinData.flLateralMaxDistance;

    Quaternion qYaw;
    qYaw.FromYaw(tDPath.aOriginalYaw);
    Vector3D vLocalRotatedPos = qYaw.Rotate(vLocalPos);

    Vector3D vWorldPos = tDPath.vStartPos + vLocalRotatedPos;
    WorldPlace placeCurr = ptACD->GetPlace();
    vVel = vWorldPos - placeCurr.wp;
    vVel.z = 0.0f;

    vVel *= tDPath.flSpeedMult / flUpdateTime;

    return TRUE;
}













static void sSpiralStart(
    ActorMovement & tMove,
    DeterministicPathData & tDPath)
{

    if (tMove.IsServerProjectile() && tDPath.tSinData.annOwner != ANN_INVALID)
    {
        ACDID idACDOwner = ACDFromANN(tDPath.tSinData.annOwner);
        const ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
        ActorCommonData *ptACDProjectile = ACDTryToGet(tMove.GetACDID());
        if (ptACDOwner && ptACDProjectile)
        {
            WorldPlace wpOwner = ptACDOwner->GetPlace();
            WorldPlace wpProjectile = ptACDProjectile->GetPlace();
            if (wpOwner.idSWorld == wpProjectile.idSWorld)
            {
                tDPath.flVar1 = wpProjectile.wp.z - wpOwner.wp.z;
            }
        }
    }


    Vector3D vDelta = tDPath.vTargetPos - tDPath.vStartPos;
    vDelta.z = 0.0f;  // CrossProduct below multiplies by a constant zero for vDelta.z

    Vector3D vFacingDir = Vector3D::CrossProduct(vDelta, cgwvZAxis);
    if (tDPath.tSinData.flSinIncPerTick < -EPSILON)
    {
        vFacingDir = -vFacingDir;
    }
    tMove.SetNewFacing(vFacingDir.As2D().ToYaw(), TRUE, FALSE);

}

static BOOL sSpiralUpdate(
    ActorMovement & tMove,
    Vector3D & vVel,
    DeterministicPathData & tDPath,
    float flUpdateTime)
{
    ACDID idACD = tMove.GetACDID();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return FALSE;
    }

    if (flUpdateTime < EPSILON)
    {
        return FALSE;
    }

    tDPath.flVar0 += flUpdateTime;

    Vector3D vDelta = tDPath.vTargetPos - tDPath.vStartPos;
    vDelta.z = 0.0f;  // the length below sums x*x + y*y + 0

    float flLength = 0.0f;
    if (vDelta.MagnitudeSquared() > 1e-12f)  // data 0x2b8cbccc (EPSILON squared)
    {
        flLength = vDelta.Normalize();
    }
    else
    {
        Quaternion qYaw;
        qYaw.FromYaw(tDPath.aOriginalYaw);
        vDelta = qYaw.Rotate(cgwvXAxis);
    }


    float flUnitLength = flLength * 0.025f;  // data 0x3ccccccd: full first stage at 40 units
    flUnitLength = Math::Pin(flUnitLength, 0.0f, 1.0f);
    float flFirstStageTicks = flUnitLength * 30.0f;



    if (tDPath.eDPath == DPATH_SPIRAL_IMMEDIATE)
    {
        // no outward stage: spiral at once, on the other side
        vDelta = -vDelta;
        flFirstStageTicks = 0.0f;
    }
    Vector3D vWorldPos;
    if (tDPath.flVar0 <= flFirstStageTicks && flFirstStageTicks > EPSILON)
    {
        // first stage: ease out along vDelta
        float flUnitFirstStage = tDPath.flVar0 / flFirstStageTicks;
        flUnitFirstStage = Math::Pin(flUnitFirstStage, 0.0f, 1.0f);
        float flSinPos = Math::Sin(flUnitFirstStage * QUARTER_CIRCLE);
        flSinPos *= flLength;
        vWorldPos = vDelta * flSinPos;
        vWorldPos += tDPath.vStartPos;
    }
    else
    {
        float flSpiralTime = tDPath.flVar0 - flFirstStageTicks;

        float flAmplitude = tDPath.tSinData.flLateralStartDistance + flSpiralTime * tDPath.tSinData.flSpeed;
        if (tDPath.tSinData.flSpeed >= 0.0f)
        {
            flAmplitude = Math::Min(flAmplitude, tDPath.tSinData.flLateralMaxDistance);
        }
        else
        {
            flAmplitude = Math::Max(flAmplitude, tDPath.tSinData.flLateralMaxDistance);
        }

        float flSinIdx = (0.5f * tDPath.tSinData.flSinIncAccel * flSpiralTime * flSpiralTime) + tDPath.tSinData.flSinIncPerTick * flSpiralTime;
        flSinIdx *= FULL_CIRCLE;
        Angle aBaseYaw = Math::ATan(vDelta.y, vDelta.x);
        Angle aYaw = aBaseYaw + flSinIdx;
        Vector3D vOffset(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
        vOffset *= flAmplitude;


        Vector3D vSpiralCenter = tDPath.vTargetPos;
        if (tDPath.tSinData.annOwner != ANN_INVALID)
        {
            BOOL bUsePreviousVelocity = TRUE;

            ACDID idACDOwner = ACDFromANN(tDPath.tSinData.annOwner);
            const ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
            if (ptACDOwner)
            {
                WorldPlace tPlaceOwner = ptACDOwner->GetPlace();
                if (tPlaceOwner.idSWorld == ptACD->GetPlace().idSWorld)
                {
                    vSpiralCenter = tPlaceOwner.wp;
                    vSpiralCenter.z += tDPath.flVar1;
                    bUsePreviousVelocity = FALSE;
                }
            }

            // no row: these exits all branch straight to the shared return TRUE
            if (bUsePreviousVelocity)
            {
                return TRUE;
            }
        }
        vWorldPos = vSpiralCenter + vOffset;
    }

    WorldPlace placeCurr = ptACD->GetPlace();
    vVel = vWorldPos - placeCurr.wp;

    if (tDPath.tSinData.annOwner == ANN_INVALID)
    {
        vVel.z = 0.0f;
    }

    vVel *= tDPath.flSpeedMult / flUpdateTime;

    Angle aFacingYaw = Math::ATan(vVel.y, vVel.x);
    tMove.SetNewFacing(aFacingYaw, FALSE, FALSE);

    return TRUE;
}



static void sPieWedgeStart(
    DeterministicPathData & tDPath)
{

    Vector3D vStartToFirstTarget = tDPath.vTargetPos - tDPath.vStartPos;
    tDPath.flVar0 = vStartToFirstTarget.Normalize();


    Vector3D vStartToSecondTarget = tDPath.tPieData.vTargetPos2 - tDPath.vStartPos;
    tDPath.flVar1 = vStartToSecondTarget.Normalize();


    tDPath.flVar2 = Math::ACos(Vector3D::DotProduct(vStartToFirstTarget, vStartToSecondTarget));

}


static void sPieWedgeComputePosition(
    Vector3D & vDestPos,
    const Vector3D & vSourcePos,
    const DeterministicPathData & tDPath,
    float flTimeToFirstTarget,
    float flTimeToSecondTarget,
    float flTimeToReturnStart,
    float flCurTime)
{
    if (flCurTime <= flTimeToFirstTarget)
    {
        Vector3D vToFirstTarget = tDPath.vTargetPos - tDPath.vStartPos;
        float s = (flCurTime / flTimeToFirstTarget);
        vDestPos = vSourcePos + vToFirstTarget * s;
    }
    else if (flCurTime <= flTimeToSecondTarget)
    {
        Vector3D vToFirstTarget = (tDPath.vTargetPos - tDPath.vStartPos) * (1.0f / tDPath.flVar0);
        Vector3D vToSecondTarget = (tDPath.tPieData.vTargetPos2 - tDPath.vStartPos) * (1.0f / tDPath.flVar1);

        float s = (flCurTime - flTimeToFirstTarget) / (flTimeToSecondTarget - flTimeToFirstTarget);
        float flRadius = tDPath.flVar0 + (tDPath.flVar1 - tDPath.flVar0) * s;
        float flCurRotation = tDPath.flVar2 * s;

        if (Vector3D::CrossProduct(vToFirstTarget, vToSecondTarget).z < 0.0f)
        {
            flCurRotation = -flCurRotation;
        }

        Quaternion qRot;
        qRot.SetRotation(Vector3D(0.0f, 0.0f, 1.0f), flCurRotation);  // a literal z axis: x and y multiply by a constant 0, not by cgwvZAxis loaded from memory

        Vector3D vToCurPos = qRot.Rotate(vToFirstTarget);
        vDestPos = vSourcePos + vToCurPos * flRadius;
    }
    else if (flCurTime <= flTimeToReturnStart)
    {
        Vector3D vToSecondTarget = tDPath.tPieData.vTargetPos2 - tDPath.vStartPos;
        float s = (flCurTime - flTimeToSecondTarget) / (flTimeToReturnStart - flTimeToSecondTarget);
        vDestPos = vSourcePos + vToSecondTarget * (1.0f - s);
    }
}

static BOOL sPieWedgeUpdate(
    ActorMovement & tMove,
    Vector3D & vVel,
    DeterministicPathData & tDPath,
    float flUpdateTime)
{
    ACDID idACD = tMove.GetACDID();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return FALSE;
    }

    if (flUpdateTime < EPSILON)
    {
        return FALSE;
    }






    tDPath.flVar3 += flUpdateTime;

    float flAverageRadius = (tDPath.flVar0 + tDPath.flVar1) * 0.5f;
    float flArcDist = flAverageRadius * FULL_CIRCLE * (tDPath.flVar2 / FULL_CIRCLE);  // first '*' is at column 34 in the original

    float flTimeToFirstTarget = tDPath.flVar0 / tDPath.tPieData.flStraightSpeed;
    float flTimeToSecondTarget = flTimeToFirstTarget + flArcDist / tDPath.tPieData.flArcSpeed;
    float flTimeToReturnStart = flTimeToSecondTarget + tDPath.flVar1 / tDPath.tPieData.flStraightSpeed;


    while (tDPath.flVar3 > flTimeToReturnStart)
    {
        tDPath.flVar3 -= flTimeToReturnStart;
    }


    Vector3D vDestPos;
    sPieWedgeComputePosition(vDestPos, tDPath.vStartPos, tDPath, flTimeToFirstTarget, flTimeToSecondTarget, flTimeToReturnStart, tDPath.flVar3);


    ACDID idACDOwner = ACDFromANN(tDPath.tPieData.annOwner);
    const ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (ptACDOwner)
    {
        Vector3D vOwnerDestPos;
        sPieWedgeComputePosition(vOwnerDestPos, ptACDOwner->GetPlace().wp, tDPath, flTimeToFirstTarget, flTimeToSecondTarget, flTimeToReturnStart, tDPath.flVar3);
        float s = (tDPath.flVar3 / flTimeToReturnStart);
        s = Math::Sqrt(s);
        vDestPos = vDestPos + (vOwnerDestPos - vDestPos) * s;
    }

    WorldPlace placeCurr = ptACD->GetPlace();
    vVel = vDestPos - placeCurr.wp;
    vVel.z = 0.0f;

    vVel *= tDPath.flSpeedMult / flUpdateTime;

    return TRUE;
}



static BOOL sCurvedOffsetUpdate(
    ActorMovement & tMove,
    Vector3D & vVel,
    DeterministicPathData & tDPath,
    float flUpdateTime)
{
    ACDID idACD = tMove.GetACDID();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return FALSE;
    }

    if (flUpdateTime < EPSILON)
    {
        return FALSE;
    }





    float flSmoothingTime = (Math::Abs(tDPath.flVar1) * 0.5f * PI) / tMove.GetDesiredSpeed();


    tDPath.flVar0 += flUpdateTime;


    float flSmoothingProgress = Math::Min(tDPath.flVar0 / flSmoothingTime, 1.0f);


    Angle aSmoothingAngle = QUARTER_CIRCLE * flSmoothingProgress;
    Vector3D vLocalPos(Math::Abs(tDPath.flVar1) * (1.0f - Math::Cos(aSmoothingAngle)), tDPath.flVar1 * Math::Sin(aSmoothingAngle), 0.0f);
    if (tDPath.flVar0 > flSmoothingTime)
    {
        vLocalPos.x += tMove.GetDesiredSpeed() * (tDPath.flVar0 - flSmoothingTime);
    }


    Quaternion qYaw;
    qYaw.FromYaw(tDPath.aOriginalYaw);
    Vector3D vWorldDest = tDPath.vStartPos + qYaw.Rotate(vLocalPos);


    vVel = vWorldDest - ptACD->GetPlace().wp;
    vVel.z = 0.0f;

    vVel *= tDPath.flSpeedMult / flUpdateTime;

    return TRUE;
}











static void sFirestormStart(
    ActorMovement & tMove,
    DeterministicPathData & tDPath)
{
    if (tMove.IsServerProjectile())
    {

        tDPath.flVar3 = tMove.GetDesiredSpeed();
    }
}

static BOOL sFirestormUpdate(
    ActorMovement & tMove,
    Vector3D & vVel,
    DeterministicPathData & tDPath,
    float flUpdateTime)
{





    ACDID idACD = tMove.GetACDID();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return FALSE;
    }

    if (flUpdateTime < EPSILON)
    {
        return FALSE;
    }


    tDPath.flVar0 -= flUpdateTime;
    while (tDPath.flVar0 <= EPSILON)
    {
        Angle aMoveAngle = tDPath.aOriginalYaw;

        int32 & nCurOffset = tDPath.nVar0;
        float flSegmentDuration = (float)RandRange_Internal(tDPath.tSeed, 9, 15);


        int32 nDirWeight = Rand_Internal(tDPath.tSeed, 5);
        if (abs(nCurOffset) >= 2)  // the CU has no Math::Abs<int>; cneg sits at column 23, the argument
        {
            // at the edge: only a step back toward the aim line is allowed
            if (nDirWeight >= 3)
            {
                if (nCurOffset < 0)
                {
                    flSegmentDuration = (float)RandRange_Internal(tDPath.tSeed, 7, 10);
                    aMoveAngle += 0.52359879f;  // PI / 6 (table 0xe3cc20 indexed by nCurOffset < 0)
                    ++nCurOffset;
                }
                else
                {
                    flSegmentDuration = (float)RandRange_Internal(tDPath.tSeed, 7, 10);
                    aMoveAngle -= 0.52359879f;
                    --nCurOffset;
                }
            }
        }
        else
        {


            if (nDirWeight >= 4)
            {
                flSegmentDuration = (float)RandRange_Internal(tDPath.tSeed, 7, 10);
                aMoveAngle += 0.52359879f;  // PI / 6 (data 0x3f060a92)
                ++nCurOffset;
            }
            // line 1062 holds the else-if (column 22)
            else if (nDirWeight >= 2)
            {
                flSegmentDuration = (float)RandRange_Internal(tDPath.tSeed, 7, 10);
                aMoveAngle -= 0.52359879f;
                --nCurOffset;
            }
        }



        Quaternion qYaw;
        qYaw.FromYaw(tDPath.flVar2);
        Vector3D vMovementDir = qYaw.Rotate(cgwvXAxis);
        tDPath.vStartPos += vMovementDir * tDPath.flVar1 * tDPath.flVar3;


        aMoveAngle = Math::NormalizeAngle(aMoveAngle);
        tMove.SetNewFacing(aMoveAngle, FALSE, FALSE);



        tDPath.flVar0 += flSegmentDuration;
        tDPath.flVar1 = flSegmentDuration;
        tDPath.flVar2 = aMoveAngle;
    }

    Quaternion qYaw;
    qYaw.FromYaw(tDPath.flVar2);
    Vector3D vMovementDir = qYaw.Rotate(cgwvXAxis);


    WorldPlace wpCurrent;
    tMove.GetPlace(wpCurrent);

    float flSegmentElapsedTime = tDPath.flVar1 - tDPath.flVar0;
    Vector3D vWorldDest = tDPath.vStartPos + vMovementDir * flSegmentElapsedTime * tDPath.flVar3;
    vVel = vWorldDest - wpCurrent.wp;
    vVel.z = 0.0f;

    vVel *= tDPath.flSpeedMult / flUpdateTime;

    return TRUE;
}



void DetPathStart(
    ActorMovement & tMove,
    DeterministicPathData & tDPath)
{
    switch (tDPath.eDPath)
    {
        case DPATH_CHARGED_BOLT:
            sChargedBoltStart(tMove, tDPath);
            break;
        case DPATH_PLAGUE_OF_TOADS:
            sPlagueOfToadsStart(tMove, tDPath);
            break;
        case DPATH_PLAGUE_OF_TOADS_STRAIGHT:
            sPlagueOfToadsStart(tMove, tDPath);
            sPlagueOfToadsStartStraight(tDPath);
            break;
        case DPATH_CHAKRAM_LEFT:
        case DPATH_CHAKRAM_RIGHT:
        case DPATH_SIN_WAVE:
        case DPATH_CURVED_OFFSET:
            break;
        case DPATH_PEDAL_CURVE:
        case DPATH_SPIRAL:
        case DPATH_SPIRAL_IMMEDIATE:
            sSpiralStart(tMove, tDPath);
            break;
        case DPATH_PIE_WEDGE:
            sPieWedgeStart(tDPath);
            break;


        case DPATH_FIRESTORM:
            sFirestormStart(tMove, tDPath);
            break;
        default:
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

}


// NON_MATCHING: 6316 bytes, same size; 2 instructions differ at +0x1758, in the inlined sFirestormUpdate (lines 1083-1085): the original pairs the flVar1/flVar2 stores (stp ... [x20, #0x30]) and stores flVar0 last, ours pairs flVar0/flVar1; tried flVar2 before flVar1, and "flVar0 +=" split over lines 1083-1084; writing flVar0 += after the two stores matches the bytes but not the line table (flVar0 is line 1083, the pair line 1084)
BOOL DetPathUpdate(
    ActorMovement & tMove,
    Vector3D & vVel,
    DeterministicPathData & tDPath,
    float flUpdateTime)
{
    BOOL bUpdated = FALSE;

    flUpdateTime *= tDPath.flSpeedMult;

    switch (tDPath.eDPath)
    {
        case DPATH_CHARGED_BOLT:
            bUpdated = sChargedBoltUpdate(tMove, vVel, tDPath, flUpdateTime);
            break;
        case DPATH_PLAGUE_OF_TOADS:
        case DPATH_PLAGUE_OF_TOADS_STRAIGHT:
            bUpdated = sPlagueOfToadsUpdate(tMove, vVel, tDPath, flUpdateTime);
            break;
        case DPATH_SIN_WAVE:
            bUpdated = sSinWaveUpdate(tMove, vVel, tDPath, flUpdateTime);
            break;
        case DPATH_CHAKRAM_LEFT:
            bUpdated = sChakramUpdate(tMove, vVel, tDPath, flUpdateTime, FALSE);
            break;
        case DPATH_CHAKRAM_RIGHT:
            bUpdated = sChakramUpdate(tMove, vVel, tDPath, flUpdateTime, TRUE);
            break;
        case DPATH_PEDAL_CURVE:
            bUpdated = sPedalCurveUpdate(tMove, vVel, tDPath, flUpdateTime);
            break;
        case DPATH_SPIRAL:
        case DPATH_SPIRAL_IMMEDIATE:
            bUpdated = sSpiralUpdate(tMove, vVel, tDPath, flUpdateTime);
            break;
        case DPATH_PIE_WEDGE:
            bUpdated = sPieWedgeUpdate(tMove, vVel, tDPath, flUpdateTime);
            break;
        case DPATH_CURVED_OFFSET:
            bUpdated = sCurvedOffsetUpdate(tMove, vVel, tDPath, flUpdateTime);
            break;
        case DPATH_FIRESTORM:
            bUpdated = sFirestormUpdate(tMove, vVel, tDPath, flUpdateTime);
            break;
        default:
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


    return bUpdated;
}



uint32 DetPathGetUpdateInterval(
    DeterministicPathType eDType)
{
    uint32 uTicks = 0;


    switch (eDType)
    {
        case DPATH_CHARGED_BOLT:
            uTicks = 20;
            break;
        case DPATH_PLAGUE_OF_TOADS:
        case DPATH_CHAKRAM_LEFT:
        case DPATH_CHAKRAM_RIGHT:
        case DPATH_SIN_WAVE:
        case DPATH_PLAGUE_OF_TOADS_STRAIGHT:
            uTicks = 10;
            break;
        case DPATH_PEDAL_CURVE:
        case DPATH_SPIRAL:
        case DPATH_PIE_WEDGE:
        case DPATH_CURVED_OFFSET:
        case DPATH_FIRESTORM:
        case DPATH_SPIRAL_IMMEDIATE:
            uTicks = 8;
            break;


        default:
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    return uTicks;
}






BOOL DetPathShouldSendMoveMessageBeforeServerUpdate(
    const DeterministicPathData & tDPath)
{
    switch (tDPath.eDPath)
    {
        case DPATH_CHARGED_BOLT:
            return FALSE;

        case DPATH_PLAGUE_OF_TOADS:
        case DPATH_CHAKRAM_LEFT:
        case DPATH_CHAKRAM_RIGHT:
        case DPATH_SIN_WAVE:
        case DPATH_PEDAL_CURVE:
        case DPATH_SPIRAL:
        case DPATH_PLAGUE_OF_TOADS_STRAIGHT:
        case DPATH_PIE_WEDGE:
        case DPATH_CURVED_OFFSET:
        case DPATH_FIRESTORM:
        case DPATH_SPIRAL_IMMEDIATE:
            return TRUE;

        default:
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}






BOOL DetPathProcessAdjustFacing(
    ActorMovement &,
    DeterministicPathData & tDPath,
    Angle aNewYaw)
{
    switch (tDPath.eDPath)
    {
        case DPATH_CHARGED_BOLT:
        case DPATH_CHAKRAM_LEFT:
        case DPATH_CHAKRAM_RIGHT:
            return FALSE;
        case DPATH_PLAGUE_OF_TOADS:
        case DPATH_PLAGUE_OF_TOADS_STRAIGHT:
            if (!(tDPath.nVar0 & (1 << TOAD_FLAG_HOPPING)))
            {
                tDPath.aOriginalYaw = aNewYaw;
                tDPath.nVar0 |= (1 << TOAD_FLAG_MANUALLY_AIMED);
                return TRUE;
            }
            return FALSE;

        case DPATH_SIN_WAVE:
        case DPATH_PEDAL_CURVE:
        case DPATH_SPIRAL:
        case DPATH_PIE_WEDGE:
        case DPATH_CURVED_OFFSET:
        case DPATH_FIRESTORM:
        case DPATH_SPIRAL_IMMEDIATE:
            return FALSE;





        default:
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    return FALSE;

}





BOOL DetPathProcessNavReflect(
    ActorMovement & tMove,
    DeterministicPathData & tDPath,
    const WorldCollResult & tCollisionResult)
{
    switch (tDPath.eDPath)
    {


    case DPATH_PLAGUE_OF_TOADS:
    case DPATH_PLAGUE_OF_TOADS_STRAIGHT:
        return sPlagueOfToadsNavReflect(tMove, tDPath, tCollisionResult);


    case DPATH_CHARGED_BOLT:
    case DPATH_CHAKRAM_LEFT:
    case DPATH_CHAKRAM_RIGHT:
    case DPATH_SIN_WAVE:
    case DPATH_PEDAL_CURVE:
    case DPATH_SPIRAL:
    case DPATH_PIE_WEDGE:
    case DPATH_CURVED_OFFSET:
    case DPATH_FIRESTORM:
    case DPATH_SPIRAL_IMMEDIATE:
        return FALSE;
    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    return FALSE;

}
