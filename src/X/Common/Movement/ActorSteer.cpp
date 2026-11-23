// Original file: X/Common/Movement/ActorSteer.cpp

#include "ActorSteer.h"
#include "ActorMovement.h"
#include "../Player.h"
#include "../WorldCommonCollision.h"

// Declared locally as in the other callers; DWARF definition: X/Common/WorldCommonCollision.cpp:147.
BOOL WorldCollSweptCircle(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Circle & wcStart, const Vector2D & wpDest, const WorldCollParams & tWorldCollParams, uint32 uCollTestFlags, uint32 uACDCollNarrowFlags, ACDID idACDIgnore, WorldCollResult & tResult);




































BOOL ActorSteer::Initialize(ActorMovement * ptMovement)
{



    m_ptMovement = ptMovement;
    m_uLookAheadTicks = 120;
    m_flMaxSteeringDistance = 15.0f;
    m_flUnitSteer = 1.0f;
    m_uTicksToHold180Steering = 100;
    ACDID idACD = m_ptMovement->GetACDID();
    ACDIsPlayer(idACD);
    ResetStuckUnitValue(); m_uIterations = 5;
    Reset180Steering(Vector2D(0.0f, 0.0f));
    return TRUE;
}



void ActorSteer::Reset180Steering(const Vector2D & vNewAccel)
{

    m_vLastSteering = vNewAccel;
    m_uNext180SteeringAllowedTime = m_ptMovement->GetTimeNow() + m_uTicksToHold180Steering;
}


// inferred: vVel (not vNewAccel) takes the += / Normalize at 114/121 (fadd vVel.y + vNewAccel.y; vNewAccel stays at its stack slot until the Slide* calls); 125 tests uTicksToMove first per the 125:31 || column
// NON_MATCHING: 1528 vs 1568 bytes with SlideOBB (285-481) written (it was 1484 without a body): the call list and control flow are the original's; frame 0x1b0 vs 0x1c0, the original spills idACD ([sp+0x44]) and keeps uTicksToCheck in w28 where ours does the reverse, and keeps the vAccel pointer in x23 (ours x20); about 10 instructions fewer. Not retried further.
SteerResult ActorSteer::ModifyAccelForSteering(
    Vector2D & vAccel,
    BOOL bUseCosAngLimit)
{
    if (!m_ptMovement)
    {
        return STEER_RESULT_NORMAL;
    }



    if (m_flUnitSteer <= 0.000001f)
    {
        return STEER_RESULT_NORMAL;
    }

    float flRadius;
    WorldPlace placeOrig;
    m_ptMovement->GetPlaceAndRadius(placeOrig, flRadius, TRUE);

    ACDID idACD = m_ptMovement->GetACDID();
    Vector2D vOrigDir = (m_ptMovement->m_vCurrPathDest - placeOrig.wp).As2D();

    uint32 uTicksToCheck = m_ptMovement->GetUpdateInterval() + m_uLookAheadTicks;
    if (m_ptMovement->IsUsingChargeSteering())
    {
        uTicksToCheck = 2;
    }

    SteerResult eSteerResult = STEER_RESULT_NORMAL;
    Vector2D vNewAccel = vAccel;
    for (uint32 i = 0; i < m_uIterations; i++)
    {
        Vector2D vVel = m_ptMovement->GetVelocity().As2D();
        BOOL bMoving = FALSE;
        if (vVel.MagnitudeSquared() > 0.000000000001f)
        {
            bMoving = TRUE;
        }
        vVel += vNewAccel;

        float flMagSqr = vVel.MagnitudeSquared();
        if (flMagSqr < 0.000000000001f)
        {
            return STEER_RESULT_NORMAL;
        }
        vVel.Normalize();

        float flDesiredSpeed = m_ptMovement->GetDesiredSpeed();
        uint32 uTicksToMove = m_ptMovement->GetUpdateInterval();
        if (uTicksToMove == 0 || uTicksToCheck == 0)
        {
            return STEER_RESULT_NORMAL;
        }

        WorldPlace placeCurr;
        m_ptMovement->GetPlace(placeCurr);

        Vector2D vToFinalDest = m_ptMovement->m_vCurrPathDest.As2D() - placeCurr.wp.As2D();
        float flTravelDist = flDesiredSpeed * uTicksToCheck;
        flTravelDist = Math::Max(flTravelDist, 0.5f);
        if (vToFinalDest.MagnitudeSquared() < flTravelDist * flTravelDist)
        {
            flTravelDist = vToFinalDest.Magnitude();
        }

        Vector2D vToDest = vVel * flTravelDist;

        Vector2D vDest = placeCurr.wp.As2D() + vToDest;
        ACDCollSettings tCollSettings;
        m_ptMovement->GetCollSettings(tCollSettings);
        WorldCollParams tWorldCollParams(tCollSettings);

        const NavCellHandle &hNavCellCurr = m_ptMovement->GetNavCell();

        Circle circleCurr;
        circleCurr.Set(placeCurr.wp.x, placeCurr.wp.y, flRadius);

        WorldCollResult tResult;
        tResult.idACDHit = ACDID_INVALID;
        BOOL bCollided = WorldCollSweptCircle(placeCurr.idSWorld, hNavCellCurr, circleCurr, vDest, tWorldCollParams, 3, 1, idACD, tResult);








        // line table: the ACDTryToGet result is tested right after the 155 call; 166 columns 23 (&&), 26 (ptACDClosest), 57 (IsInWorld)
        ActorCommonData *ptACDClosest = ACDTryToGet(tResult.idACDHit);
        if (bCollided && ptACDClosest && !ptACDClosest->IsInWorld())
        {
            break;
        }

        if (!bCollided)
        {
            break;
        }
        else
        {


            float flMaxSteeringDist = Math::Max(m_flMaxSteeringDistance, flDesiredSpeed);
            if (ptACDClosest)
            {




                Vector2D vDistCanTravel = vDest - placeCurr.wp.As2D();
                vDistCanTravel *= tResult.flT;

                if (ptACDClosest->id != m_ptMovement->GetTargetACD())
                {
                    if (m_ptMovement->PathBlockedByObject(ptACDClosest->id, vDistCanTravel))
                    {
                        return STEER_RESULT_STOP;
                    }

                    if (ptACDClosest->tCollSettings.eCollisionShape == COLLISION_SHAPE_BOX)
                    {
                        eSteerResult = SlideOBB(idACD, tWorldCollParams, *ptACDClosest, tResult, placeCurr, flRadius, m_ptMovement->GetCurrentDestinationPos().As2D(), TRUE, vToDest, bMoving, m_ptMovement->GetAccelMag(), flMaxSteeringDist, vNewAccel);
                    }
                    else
                    {
                        eSteerResult = SlideSphere(*ptACDClosest, tResult, placeCurr, vToDest, m_ptMovement->GetAccelMag(), flMaxSteeringDist, vNewAccel);
                    }
                }
            }
            else
            {
                eSteerResult = SlideStatic(tResult, placeCurr.wp, flRadius, bMoving, uTicksToMove, bUseCosAngLimit, flMaxSteeringDist, vToDest, vNewAccel);
            }

            float flDotWithSteerVector = 0.0f;
            if (m_ptMovement->GetTimeNow() < m_uNext180SteeringAllowedTime)
            {
                flDotWithSteerVector = Vector2D::DotProduct(vNewAccel, m_vLastSteering);
            }
            else
            {
                Reset180Steering(vNewAccel);
            }
            // inferred: 220-224 share one tail with 232 (only the 232 operator= record survives); 226:22/43 are the else-if operand and >= columns
            if (eSteerResult == STEER_RESULT_FULL_STEERING_ALLOW_180)
            {
                vAccel = vNewAccel;
            }

            else if (flDotWithSteerVector >= -0.000001f)
            {

                float flDotWithOrigVector = Vector2D::DotProduct(vOrigDir, vNewAccel);
                if (flDotWithOrigVector >= -0.000001f)
                {
                    vAccel = vNewAccel;
                }
            }
        }
    }

    return eSteerResult;
}



static BOOL sAABBAndCircle(
    Circle & wc,
    const Vector2D & vExt)
{
    Vector2D wpMin = -vExt;
    Vector2D wpMax = vExt;
    float wdD = 0.0f;

    if (wc.wpCenter.x < wpMin.x)
    {
        float flDiff = wc.wpCenter.x - wpMin.x;
        wdD += flDiff * flDiff;
    }
    else if (wc.wpCenter.x > wpMax.x)
    {
        float flDiff = wc.wpCenter.x - wpMax.x;
        wdD += flDiff * flDiff;
    }

    if (wc.wpCenter.y < wpMin.y)
    {
        float flDiff = wc.wpCenter.y - wpMin.y;
        wdD += flDiff * flDiff;
    }
    else if (wc.wpCenter.y > wpMax.y)
    {
        float flDiff = wc.wpCenter.y - wpMax.y;
        wdD += flDiff * flDiff;
    }
    // inferred: written as !(a > b): the original branches on b.le (not greater, or unordered) to the small block; wdD <= r * r gives b.ls
    return !(wdD > wc.wdRadius * wc.wdRadius);
}









// NON_MATCHING: 2288 bytes, 18 of 572 instructions differ: only register numbers (vNormal2DScoped x/y in s11/s10 vs s10/s11 from 436, vToProjPoint x/y at 452-453, their fmul/fadd operand order). Tried the 436 As2D copy, the 433 forms, DotProduct argument order, ternary vs if for flCosAngLimit, 452 operand order.
SteerResult ActorSteer::SlideOBB(
    ACDID idACD,
    const WorldCollParams & tWorldCollParams,
    ActorCommonData & tACDHit,
    const WorldCollResult & tResult,
    const WorldPlace & placeCurr,
    float flRadius,
    const Vector2D & vDest,
    BOOL bUseCosAngLimit,
    const Vector2D & vToDest,
    BOOL bMoving,
    float flAccelMag,
    float flMaxSteeringDist,
    Vector2D & vNewAccel)
{
    float flUnitSteer = 0.0f;
    SteerResult eResult = STEER_RESULT_NORMAL;


    Vector3D vCollisionSteer;
    WorldPlace placeClosest = tACDHit.GetPlace();
    Vector3D vToClosest = placeClosest.wp - placeCurr.wp;
    // inferred: zero-constructed, then x and y assigned (310:27 is the vToDest.x member column; z stays 0 in the dot product at 451)
    // inferred: vOrigAccel is a plain copy and flMag reads vNewAccel: with the (x, y) form and vOrigAccel.Magnitude() the small block reuses s22/s23 where the original reloads [x25]
    Vector3D vToDest3D(0.0f, 0.0f, 0.0f);
    vToDest3D.x = vToDest.x;
    vToDest3D.y = vToDest.y;
    Vector2D vOrigAccel = vNewAccel;
    // inferred: Circle built by aggregate initialisation (no Set in the DIE tree)

    const OBB & tOBB = tACDHit.GetCollSettings().GetOBB();
    PRTransform invTrans = tOBB.transform.Inverse();
    Vector3D vCurrPosOBBspace = invTrans * placeCurr.wp;
    Circle circleCurrPosBoxSpace = { vCurrPosOBBspace.As2D(), flRadius };

    if (sAABBAndCircle(circleCurrPosBoxSpace, tOBB.wvExt.As2D()))
    {
        eResult = STEER_RESULT_FULL_STEERING_ALLOW_180;  // inferred: every exit of this branch returns 3 (orr w0, wzr, #3)
        float flMag = vNewAccel.Magnitude();
        Vector2D vToBlocker(vToClosest.x, vToClosest.y);
        if (vToBlocker.MagnitudeSquared() > 0.000000000001f)
        {
            vToBlocker.Normalize();
            if (Vector2D::DotProduct(vToBlocker, vNewAccel) > 0.000001f)
            {
                vNewAccel = vToBlocker * -flMag;
            }
        }
    }
    else
    {



        // inferred: built from vToDest3D (one ldp of vToDest serves 310 and 341) with the (x, y) constructor (a copy loads integer pairs)
        Vector2D vDir2D(vToDest3D.x, vToDest3D.y);
        vDir2D.Normalize();

        Vector2D vNormal2D(tResult.wvNormal.x, tResult.wvNormal.y);
        if (Math::Abs(tResult.wvNormal.z) > 0.000001f)
        {
            vNormal2D.Normalize();
        }

        Vector2D vNormal = invTrans.Rotate(vNormal2D);

        Vector2D vWaypoint[2][2];
        Vector2D vExt = tOBB.wvExt.As2D();
        vExt.x += flRadius;
        vExt.y += flRadius;

        if (vNormal.x >= 0.5f)
        {
            vWaypoint[0][0].Set(vExt.x, vExt.y + 0.5f);
            vWaypoint[0][1].Set(-vExt.x, vExt.y);
            vWaypoint[1][0].Set(vExt.x, -vExt.y - 0.5f);
            vWaypoint[1][1].Set(-vExt.x, -vExt.y);
        }
        else if (vNormal.x <= -0.5f)
        {
            vWaypoint[0][0].Set(-vExt.x, vExt.y + 0.5f);
            vWaypoint[0][1].Set(vExt.x, vExt.y);
            vWaypoint[1][0].Set(-vExt.x, -vExt.y - 0.5f);
            vWaypoint[1][1].Set(vExt.x, -vExt.y);
        }
        else if (vNormal.y >= 0.5f)
        {
            vWaypoint[0][0].Set(vExt.x + 0.5f, vExt.y);
            vWaypoint[0][1].Set(vExt.x, -vExt.y);
            vWaypoint[1][0].Set(-vExt.x - 0.5f, vExt.y);
            vWaypoint[1][1].Set(-vExt.x, -vExt.y);
        }
        else
        {
            vWaypoint[0][0].Set(vExt.x + 0.5f, -vExt.y);
            vWaypoint[0][1].Set(vExt.x, vExt.y);
            vWaypoint[1][0].Set(-vExt.x - 0.5f, -vExt.y);
            vWaypoint[1][1].Set(-vExt.x, vExt.y);
        }

        int32 nWinningSet = 0;
        BOOL bHug = FALSE;
        Vector2D vOBBDest = invTrans * vDest;
        if (Math::Abs(vOBBDest.x) <= vExt.x &&
            Math::Abs(vOBBDest.y) <= vExt.y)
        {
            bHug = TRUE;
        }
        // inferred: bHug is set by the && at 388 and tested at 394; the && column 45 and b.hi (not-ole) show the condition is <= ... &&, with the waypoint block (396-427) emitted in the not-hugging branch
        if (!bHug)
        {
            nWinningSet = Vector2D::DistanceSquared(vWaypoint[1][1], vOBBDest) < Vector2D::DistanceSquared(vWaypoint[0][1], vOBBDest);





            vWaypoint[nWinningSet][0] = tOBB.transform * vWaypoint[nWinningSet][0];
            vWaypoint[nWinningSet][1] = tOBB.transform * vWaypoint[nWinningSet][1];












            vNewAccel = vWaypoint[nWinningSet][0] - placeCurr.wp.As2D();

            if (Vector2D::DotProduct(vNormal2D, vNewAccel) < 0.0f)
            {

                Vector3D vOrigDir(vNewAccel.x, vNewAccel.y, 0.0f);
                Vector3D vNormal3D(vNormal2D.x, vNormal2D.y, 0.0f);
                Vector3D vUp = Vector3D::CrossProduct(vOrigDir, vNormal3D);
                Vector3D vFinalDir = Vector3D::CrossProduct(vNormal3D, vUp);
                vNewAccel = vFinalDir.As2D();
            }
            eResult = STEER_RESULT_FULL_STEERING_ALLOW_180;

        }
        else
        {

            Vector2D vDir2DScoped(vToDest.x, vToDest.y);
            vDir2DScoped.Normalize();

            Vector2D vNormal2DScoped(tResult.wvNormal.x, tResult.wvNormal.y);
            if (Math::Abs(tResult.wvNormal.z) > 0.000001f)
            {
                vNormal2DScoped.Normalize();
            }

            float flProjDist = Vector2D::DotProduct(vDir2DScoped, vNormal2DScoped);
            float flCosAngLimit =
                bMoving ? -0.996195f : -0.961262f;




            if (!bUseCosAngLimit || (flProjDist > flCosAngLimit && flProjDist < -0.000001f))
            {
                float flProj = Vector3D::DotProduct(vToDest3D, tResult.wvNormal);
                Vector3D vToProjPoint = tResult.wvNormal * flProj;
                vToProjPoint = vToDest3D - vToProjPoint;

                if (vToProjPoint.MagnitudeSquared() > 0.000000000001f)
                {
                    vToProjPoint.Normalize();
                    vToProjPoint *= flAccelMag;
                }


                float flTotalDist = vToDest.Magnitude();
                float flClosestDist = tResult.flT * flTotalDist;
                flClosestDist -= 0.5f;
                flClosestDist /= flMaxSteeringDist;
                flClosestDist = Math::Pin(flClosestDist, 0.0f, 1.0f);
                flUnitSteer = 1.0f - flClosestDist;
                flUnitSteer *= m_flUnitSteer;

                vNewAccel = (flUnitSteer * vToProjPoint.As2D()) + (1.0f - flUnitSteer) * vOrigAccel;
            }

            if (flUnitSteer >= 0.999999f)
            {
                eResult = STEER_RESULT_FULL_STEERING;
            }
        }
    }

    return eResult;
}



SteerResult ActorSteer::SlideSphere(
    ActorCommonData & tACDHit,
    const WorldCollResult & tResult,
    const WorldPlace & placeCurr,
    const Vector2D & vToDest,
    float flAccelMag,
    float flMaxSteeringDist,
    Vector2D & vNewAccel)
{
    float flUnitSteer = 0.0f;
    SteerResult eResult = STEER_RESULT_NORMAL;


    Vector3D vCollisionSteer;
    WorldPlace placeClosest = tACDHit.GetPlace();
    Vector3D vToClosest = placeClosest.wp - placeCurr.wp;
    // inferred: the z of vToClosest is never computed (no 563 row in Vector3D operator-) and folds to 0 in the cross products and MagnitudeSquared
    vToClosest.z = 0.0f;
    Vector2D vOrigAccel(vNewAccel.x, vNewAccel.y);  // inferred: (x, y) form; a plain copy (503:27 is the vNewAccel column) is lowered to an i32 pair, the original keeps floats

    if (tResult.flT < 0.000001f)
    {
        eResult = STEER_RESULT_FULL_STEERING_ALLOW_180;  // inferred: every exit of this branch returns 3 (orr w0, wzr, #3)
        float flMag = vOrigAccel.Magnitude();
        Vector2D vToBlocker(vToClosest.x, vToClosest.y);
        if (vToBlocker.MagnitudeSquared() > 0.000000000001f)
        {
            vToBlocker.Normalize();
            if (Vector2D::DotProduct(vToBlocker, vNewAccel) > 0.000001f)
            {


                Vector3D vToDest3D(vToDest.x, vToDest.y, 0.0f);

                vCollisionSteer = Vector3D::CrossProduct(vToClosest, vToDest3D);
                vCollisionSteer = Vector3D::CrossProduct(vCollisionSteer, vToDest3D);
                if (vCollisionSteer.MagnitudeSquared() > 0.000000000001f)
                {
                    vCollisionSteer.Normalize();
                    vNewAccel = vCollisionSteer.As2D() * flMag;
                }
                else
                {
                    vNewAccel = vToBlocker * -flMag;
                }
            }
        }
    }
    else
    {



        // line table: 540:31 is the vToDest.x load (member column), so x and y are assigned on 540/541; z is the constant 0 used in the dot product
        Vector3D vToDest3D(0.0f, 0.0f, 0.0f);
        vToDest3D.x = vToDest.x;
        vToDest3D.y = vToDest.y;
        if (vToClosest.MagnitudeSquared() > 0.000000000001f)
        {
            vCollisionSteer = Vector3D::CrossProduct(vToClosest, cgwvZAxis);
            float flDot = Vector3D::DotProduct(vCollisionSteer, vToDest3D);
            if (flDot < 0.0f)
            {
                vCollisionSteer = -vCollisionSteer;
            }
        }
        else
        {
            vCollisionSteer = cgwvNull;
        }

        if (vCollisionSteer.MagnitudeSquared() > 0.000000000001f)
        {
            vCollisionSteer.Normalize();
            vCollisionSteer *= flAccelMag;
        }


        float flTotalDist = vToDest.Magnitude();
        float flClosestDist = tResult.flT * flTotalDist;
        flClosestDist -= 0.5f;
        flClosestDist /= flMaxSteeringDist;
        flClosestDist = Math::Pin(flClosestDist, 0.0f, 1.0f);
        flUnitSteer = 1.0f - flClosestDist;
        flUnitSteer *= m_flUnitSteer;

        vNewAccel = (flUnitSteer * vCollisionSteer.As2D()) + (1.0f - flUnitSteer) * vOrigAccel;

        if (flUnitSteer >= 0.999999f)
        {
            eResult = STEER_RESULT_FULL_STEERING;
        }
    }

    return eResult;
}




SteerResult ActorSteer::SlideStatic(const WorldCollResult & tResult, const Vector3D & vPos, float flRadius, BOOL bMoving, uint32 uTicksToMove, BOOL bUseCosAngLimit, float flMaxSteeringDist, const Vector2D & vToDest, Vector2D & vNewAccel)
{

















    // inferred: built with the (x, y) constructor (no instructions left in it, so no inline DIE); a copy or operator= is integer-typed before inlining and keeps vDir2D out of s10/s11
    Vector2D vDir2D(vToDest.x, vToDest.y);
    vDir2D.Normalize();

    Vector2D vToPosFromContact = vPos.As2D() - tResult.wpContact.As2D();
    float flDistAwayFromSurface = Vector2D::DotProduct(vToPosFromContact, tResult.wvNormal.As2D());
    float flMinDist = flRadius + 0.01f;
    if (flDistAwayFromSurface <= flMinDist)
    {

        Vector2D vDest2D = vPos.As2D() + vToDest;
        Vector2D vToDestFromCollPoint = vDest2D - tResult.wpContact.As2D();
        float flProjOnNormal = Vector2D::DotProduct(vToDestFromCollPoint, tResult.wvNormal.As2D());

        Vector2D vOffsetFromCollPoint = tResult.wvNormal.As2D() * (flMinDist - flProjOnNormal);
        vNewAccel = vToDestFromCollPoint + vOffsetFromCollPoint;
        return STEER_RESULT_FULL_STEERING;
    }


    float flUnitSteer = 0.0f;
    SteerResult eResult = STEER_RESULT_NORMAL;
    // inferred: (x, y) constructor as for vDir2D; the copy from As2D() reverses the Normalize fmul operands
    Vector2D vNormal2D(tResult.wvNormal.x, tResult.wvNormal.y);
    if (Math::Abs(tResult.wvNormal.z) > 0.000001f)
    {
        vNormal2D.Normalize();
    }

    float flProjDist = Vector2D::DotProduct(vDir2D, vNormal2D);
    float flCosAngLimit = -0.961262f;
    if (bMoving)
    {
        flCosAngLimit = -0.996195f;
    }
    // line table: 640 columns 26 (||) and 57 (&&) of this condition
    if (!bUseCosAngLimit || (flProjDist > flCosAngLimit && flProjDist < -0.000001f))
    {
        Vector2D vToProj = vNormal2D * flProjDist;
        Vector2D vCollisionSteer = vDir2D - vToProj;


        float flTotalDist = vToDest.Magnitude();
        vCollisionSteer *= flTotalDist;

        vCollisionSteer = vCollisionSteer * (1.0f / (float)uTicksToMove);

        float flClosestDist = tResult.flT * vToProj.Magnitude();
        flClosestDist -= (flRadius + 0.5f);
        flClosestDist /= flMaxSteeringDist;
        flClosestDist = Math::Pin(flClosestDist, 0.0f, 1.0f);
        flUnitSteer = 1.0f - flClosestDist;
        flUnitSteer *= m_flUnitSteer;

        vNewAccel = flUnitSteer * vCollisionSteer + (1.0f - flUnitSteer) * vNewAccel;
    }

    if (flUnitSteer >= 0.999999f)
    {
        eResult = STEER_RESULT_FULL_STEERING;
    }

    return eResult;
}














































































BOOL ActorSteer::HuggedThisTick()
{
    return m_uHugTargetLastTime == m_ptMovement->GetTimeNow();
}


















































