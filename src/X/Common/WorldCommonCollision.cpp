// Original file: X/Common/WorldCommonCollision.cpp
// Functions sit on their original lines (see the reference .cpp).


#include "WorldCommonCollision.h"
#include "ActorCommonCollision.h"
#include "ActorCommonData.h"
#include "Target.h"
#include "WorldPlace.h"
#include "Definitions/ActorDefinition.h"
#include "Movement/ActorMovement.h"
#include "Definitions/SceneDefinition.h"
#include "Nav/NavZone.h"
#include "Nav/NavZoneIntersect.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/Random.h"
#include "SceneCommon.h"
#include "Nav/NavigationMesh.h"
#include "Game.h"
#include "../Client/CGame.h"
RNG & SGameGetRNG();  /* X/Server/SGame.cpp:6157 */ void SceneGetPhysicalAABBInWorldSpace(const SceneCommonData & tSCD, AABB & aabb);  // SceneCommon.cpp

// Declared here from their definitions elsewhere (their headers do not carry them yet).
BOOL NavZoneCollideSweptPoint(const NavCellHandle & hNavCellStart, const Vector2D & wpStart, const Vector2D & wvDirection, uint16 uOneRequiredFlag, uint16 uPreventFlags, uint32 uCollFlags, NavCellCollideResult & tResult);  // X/Common/Nav/NavZoneIntersect.cpp:1506
BOOL NavZoneCollideSweptCircle(const NavCellHandle & hNavCellStart, const Circle & wc, const Vector2D & wvDirection, uint16 uOneRequiredFlag, uint16 uPreventFlags, uint32 uCollFlags, NavCellCollideResult & tResult);  // X/Common/Nav/NavZoneIntersect.cpp:922
BOOL NavZoneCollideCircle(const NavCellHandle & hNavCell, const Circle & wc, uint16 uOneRequiredFlag, uint16 uPreventFlags, uint32 uCollFlags, NavCellCollideResult & tResult);  // X/Common/Nav/NavZoneIntersect.cpp
BOOL NavZoneCollideSweptPoint3D(SWorldID idSWorld, const Vector3D & wpStart, const Vector3D & wvDirection, uint16 uOneRequiredFlag, uint16 uPreventFlags, uint32 uCollFlags, float & flISectT, Vector3D & wpCollision);  // X/Common/Nav/NavZoneIntersect.cpp:2173
BOOL ACDCollBroadCircle(SWorldID idSWorld, const Vector2D & wpCenter, float flRadius, const ACDCollParams & tCollParams, BOOL bAllowNonWorldACDs, ACDCollBroadResult & tResult, ACDID idOwner);  // X/Common/ActorCommonCollision.cpp:396
BOOL ACDCollBroadLine(SWorldID idSWorld, const Vector2D & wp0, const Vector2D & wp1, const ACDCollParams & tCollParams, ACDCollBroadResult & tResult);  // X/Common/ActorCommonCollision.cpp:486
BOOL ACDCollNarrowSweptPoint(const ACDCollBroadResult & tCollBroadResult, const Vector2D & wp0, const Vector2D & wp1, uint32 uFlags, ACDID idACDIgnore, ACDCollNarrowResult & tResult);  // X/Common/ActorCommonCollision.cpp:781
BOOL ACDCollNarrowSweptCircle(const ACDCollBroadResult & tCollBroadResult, const Circle & wcStart, const Vector2D & wpDest, uint32 uFlags, ACDID idACDIgnore, ACDCollNarrowResult & tResult);  // X/Common/ActorCommonCollision.cpp:982
BOOL ACDCollNarrowCircle(const ACDCollBroadResult & tCollBroadResult, const Circle & wc, ACDID idACDIgnore, ACDCollNarrowResult & tResult);  // X/Common/ActorCommonCollision.cpp:590
BOOL ACDCollCollideCircle(SWorldID idSWorld, const Circle & wc, const ACDCollParams & tCollParams, ACDID idACDIgnore, ACDCollNarrowResult & tResult);  // X/Common/ActorCommonCollision.cpp:1207


WorldCollParams::WorldCollParams()
{
    uOneRequiredStaticFlags = 0;
    uStaticPreventFlags = 0;
    eTeam = 1;
    uPad8 = 0;
    uPad16 = 0;
    uObjectNoOverlapFlags = 0;
    uSpecialFlags = 0;
}


WorldCollParams::WorldCollParams(const ACDCollSettings & tACDCollSettings)
{
    uPad8 = 0;
    uPad16 = 0;
    uSpecialFlags = 0;
    uOneRequiredStaticFlags = tACDCollSettings.uStaticOneRequiredFlags;
    uStaticPreventFlags = tACDCollSettings.uStaticPreventFlags;
    eTeam = tACDCollSettings.eTeam;
    uObjectNoOverlapFlags = tACDCollSettings.uDynamicFlagsNoOverlap;
}



BOOL WorldCollSweptPoint(
    SWorldID idSWorld,
    const NavCellHandle & hNavCellStart,
    const Vector2D & wpStart,
    const Vector2D & wpDest,
    const WorldCollParams & tWorldCollParams,
    uint32 uCollTestFlags,
    uint32 uACDCollNarrowFlags,
    ACDID idACDIgnore,
    WorldCollResult & tResult)
{
    NavCellCollideResult tStaticResult;
    Vector2D wvDirection = wpDest - wpStart;
    BOOL bStaticColl = FALSE, bDynamicColl = FALSE;

    tResult.bCollision = FALSE;

    NavZoneCollideSweptPoint(hNavCellStart, wpStart, wvDirection, tWorldCollParams.uOneRequiredStaticFlags, tWorldCollParams.uStaticPreventFlags, uCollTestFlags, tStaticResult);

    Vector2D wpEnd = wpDest;
    tResult.wpEnd.Set(wpDest.x, wpDest.y, 0.0f);
    tResult.flT = 1.0f;

    if (tStaticResult.bCollided)
    {
        wpEnd = tStaticResult.wpEnd;
        bStaticColl = TRUE;
    }


    ACDCollBroadResult tBroadResult;
    ACDCollNarrowResult tNarrowResult;
    ACDCollParams tACDCollParams;

    tACDCollParams.uObjectNoOverlapFlags = tWorldCollParams.uObjectNoOverlapFlags;
    tACDCollParams.eTeam = tWorldCollParams.eTeam;
    tACDCollParams.uSpecialFlags = tWorldCollParams.uSpecialFlags;


    if (ACDCollBroadLine(idSWorld, wpStart, wpEnd, tACDCollParams, tBroadResult))
    {
        bDynamicColl = ACDCollNarrowSweptPoint(tBroadResult, wpStart, wpDest, uACDCollNarrowFlags, idACDIgnore, tNarrowResult);
    }

    if (bStaticColl || bDynamicColl)
    {
        BOOL bStaticClosest = bStaticColl;

        tResult.bCollision = TRUE;

        if (bStaticColl && bDynamicColl)
        {
            bStaticClosest = tStaticResult.flT < tNarrowResult.flT;
        }

        if (bStaticClosest)
        {
            tResult.idACDHit = ACDID_INVALID;
            tResult.wpEnd.Set(tStaticResult.wpEnd.x, tStaticResult.wpEnd.y, 0.0f);
            tResult.hNavCellEnd = tStaticResult.hNavCellEnd;
            tResult.flT = Math::Pin(tStaticResult.flT, 0.0f, 1.0f);
            tResult.wpContact.Set(tStaticResult.wpCollision.x, tStaticResult.wpCollision.y, 0.0f);
            tResult.wvNormal.Set(tStaticResult.wvNormal.x, tStaticResult.wvNormal.y, 0.0f);
        }
        else
        {
            tResult.idACDHit = tBroadResult.arACDISect[tNarrowResult.nIndexHit].idACD;
            tResult.wpEnd = tNarrowResult.wpPosition;
            tResult.flT = Math::Pin(tNarrowResult.flT, 0.0f, 1.0f);
            tResult.wpContact = tNarrowResult.wpContact;
            tResult.wvNormal = tNarrowResult.wvNormal;
            WorldPlace tPlace;
            tPlace.idSWorld = idSWorld;
            tPlace.wp = tResult.wpEnd;
            NavCellHandleFromWorldPlace(tPlace, tResult.hNavCellEnd);
        }
    }

    return tResult.bCollision;
}





BOOL WorldCollSweptCircle(
    SWorldID idSWorld,
    const NavCellHandle & hNavCellStart,
    const Circle & wcStart,
    const Vector2D & wpDest,
    const WorldCollParams & tWorldCollParams,
    uint32 uCollTestFlags,
    uint32 uACDCollNarrowFlags,
    ACDID idACDIgnore,
    WorldCollResult & tResult)
{
    if (wcStart.wdRadius <= 0.01f)
    {
        return WorldCollSweptPoint(idSWorld, hNavCellStart, wcStart.wpCenter, wpDest, tWorldCollParams, uCollTestFlags, uACDCollNarrowFlags, idACDIgnore, tResult);
    }

    NavCellCollideResult tStaticResult;
    Vector2D wvDirection = wpDest - wcStart.wpCenter;
    BOOL bStaticColl = FALSE, bDynamicColl = FALSE;

    tResult.bCollision = FALSE;

    NavZoneCollideSweptCircle(hNavCellStart, wcStart, wvDirection, tWorldCollParams.uOneRequiredStaticFlags, tWorldCollParams.uStaticPreventFlags, uCollTestFlags, tStaticResult);

    Vector2D wpEnd = wpDest;
    tResult.wpEnd.Set(wpDest.x, wpDest.y, 0.0f);
    tResult.flT = 1.0f;

    if (tStaticResult.bCollided)
    {
        wpEnd = tStaticResult.wpEnd;
        bStaticColl = TRUE;
    }

    ACDCollBroadResult tBroadResult;
    ACDCollNarrowResult tNarrowResult;
    ACDCollParams tACDCollParams;

    tACDCollParams.uObjectNoOverlapFlags = tWorldCollParams.uObjectNoOverlapFlags;
    tACDCollParams.eTeam = tWorldCollParams.eTeam;
    tACDCollParams.uSpecialFlags = tWorldCollParams.uSpecialFlags;

    Vector2D wpBroadCenter = (wcStart.wpCenter + wpDest) * 0.5f;
    float flBroadRadius;


    flBroadRadius = Vector2D::Distance(wpBroadCenter, wcStart.wpCenter) + wcStart.wdRadius;

    if (ACDCollBroadCircle(idSWorld, wpBroadCenter, flBroadRadius, tACDCollParams, FALSE, tBroadResult, ACDID_INVALID))
    {
        bDynamicColl = ACDCollNarrowSweptCircle(tBroadResult, wcStart, wpDest, uACDCollNarrowFlags, idACDIgnore, tNarrowResult);
    }

    if (bStaticColl || bDynamicColl)
    {
        BOOL bStaticClosest = bStaticColl;

        tResult.bCollision = TRUE;

        if (bStaticColl && bDynamicColl)
        {
            bStaticClosest = tStaticResult.flT < tNarrowResult.flT;
        }

        if (bStaticClosest)
        {
            tResult.idACDHit = ACDID_INVALID;
            tResult.wpEnd.Set(tStaticResult.wpEnd.x, tStaticResult.wpEnd.y, 0.0f);
            tResult.hNavCellEnd = tStaticResult.hNavCellEnd;
            tResult.flT = Math::Pin(tStaticResult.flT, 0.0f, 1.0f);
            tResult.wpContact.Set(tStaticResult.wpCollision.x, tStaticResult.wpCollision.y, 0.0f);
            tResult.wvNormal.Set(tStaticResult.wvNormal.x, tStaticResult.wvNormal.y, 0.0f);
        }
        else
        {
            tResult.idACDHit = tBroadResult.arACDISect[tNarrowResult.nIndexHit].idACD;
            tResult.wpEnd = tNarrowResult.wpPosition;
            tResult.flT = Math::Pin(tNarrowResult.flT, 0.0f, 1.0f);
            tResult.wpContact = tNarrowResult.wpContact;
            tResult.wvNormal = tNarrowResult.wvNormal;
            WorldPlace tPlace;
            tPlace.idSWorld = idSWorld;
            tPlace.wp = tResult.wpEnd;
            NavCellHandleFromWorldPlace(tPlace, tResult.hNavCellEnd);
        }
    }

    return tResult.bCollision;
}





BOOL WorldCollSearchForLocation(
    const WorldPlace & placeStart,
    uint16 uOneRequiredStaticFlags,
    uint32 uObjectNoOverlapFlags,
    float flLocationRadius,
    float flSearchRadius,
    float flSearchRadiusMax,
    ACDID idACDIgnore,
    Vector3D & wpEnd,
    NavCellHandle & hNavCellEnd,
    Team eTeam,
    uint16 uObjectNoPassthroughFlags,
    ACDID idOwner)
{
    BOOL bFoundLocation = FALSE;
    NavCellCollideResult tResult;
    NavCellCollideResult tResultCircle;
    float flLocationDiameter = flLocationRadius * 2.0f;
    float flSearchRadiusDT;

    NavCellHandle hNavCellStart;
    NavCellHandleFromWorldPlace(placeStart, hNavCellStart);
    if (hNavCellStart.uCellIndex == (uint32)-1)
    {
        return FALSE;
    }

    BOOL bTryExactSearch = flSearchRadius <= 0.000001f;
    flSearchRadius = Math::Max(flSearchRadius, 1.25f);
    flSearchRadiusDT = Math::Max(flLocationDiameter, 2.5f);

    Sphere ws;




    Circle wc;
    wc.wdRadius = Math::Max(flLocationRadius, 0.01f);

    ACDCollBroadResult tCollBroadResult;
    ACDCollParams tCollParams;
    tCollParams.uObjectNoOverlapFlags = uObjectNoOverlapFlags;
    tCollParams.eTeam = eTeam;

    ACDCollBroadCircle(placeStart.idSWorld, placeStart.wp.As2D(), flSearchRadiusMax, tCollParams, TRUE, tCollBroadResult, idOwner);






    RNG & tRNG = GameActiveGameCommonDataIsServer() ? SGameGetRNG() : CGameGlobalsGet().rngLocal;






    float flSearchPerimiterDivisor = 1.0f / flLocationDiameter;

    while (!bFoundLocation && flSearchRadius < flSearchRadiusMax)
    {


        float flSearchCircumfrence = flSearchRadius * FULL_CIRCLE;
        const int MAX_NUM_TRIES_AROUND_CIRCLE = 32;
        const int MIN_NUM_TRIES_AROUND_CIRCLE = 4;
        int nNumTriesAroundCircle = Math::FloatToInt(flSearchCircumfrence * flSearchPerimiterDivisor);

        nNumTriesAroundCircle = Math::Pin(nNumTriesAroundCircle, MIN_NUM_TRIES_AROUND_CIRCLE, MAX_NUM_TRIES_AROUND_CIRCLE);


        float flAngle = RandFloat_Internal(tRNG) * FULL_CIRCLE;
        float flAngleDt = FULL_CIRCLE / nNumTriesAroundCircle;

        for (int i = 0; i < nNumTriesAroundCircle; i++)
        {
            Vector2D vDirection;

            flAngle += flAngleDt;

            if (bTryExactSearch)
            {

                vDirection.x = 0.0f; vDirection.y = 0.0f;
            }
            else
            {


                vDirection.x = flSearchRadius * Math::Cos(flAngle);
                vDirection.y = flSearchRadius * Math::Sin(flAngle);
            }
            bTryExactSearch = FALSE;

            if (!NavZoneCollideSweptPoint(hNavCellStart, placeStart.wp.As2D(), vDirection, uOneRequiredStaticFlags, 0, 0, tResult))
            {
                wc.wpCenter = tResult.wpEnd;
                if (NavZoneCollideCircle(tResult.hNavCellEnd, wc, uOneRequiredStaticFlags, 0, 0, tResultCircle))
                {
                    continue;
                }
            }
            else
            {



                if (tResult.flT == 0.0f)
                {

                    return FALSE;
                }

                continue;
            }


            if (uObjectNoPassthroughFlags != 0)
            {
                ACDCollBroadResult tBroadResult;
                ACDCollNarrowResult tNarrowResult;
                ACDCollParams tACDCollParams;
                tACDCollParams.uObjectNoOverlapFlags = uObjectNoPassthroughFlags;
                tACDCollParams.eTeam = eTeam;
                if (ACDCollBroadLine(placeStart.idSWorld, placeStart.wp.As2D(), tResult.wpEnd, tACDCollParams, tBroadResult))
                {
                    if (ACDCollNarrowSweptPoint(tBroadResult, placeStart.wp.As2D(), tResult.wpEnd, 0, idACDIgnore, tNarrowResult))
                    {
                        tResult.wpEnd = tNarrowResult.wpContact.As2D();
                        WorldPlace placeColl;
                        placeColl.idSWorld = placeStart.idSWorld;
                        placeColl.wp = tNarrowResult.wpContact;
                        NavCellHandleFromWorldPlace(placeColl, tResult.hNavCellEnd);
                    }
                }
            }

            {
                Vector2D wpCollEnd = tResult.wpEnd;
                Vector2D wpCollision;
                float flCollisionRadius = 0.0f;
                float flCollisionDist = 0.0f;
                int nNumDynamicTries = 0;
                const int nMaxNumDynamicTries = 4;
                BOOL bFoundCollision = FALSE;
                BOOL bOkToPush = TRUE;

                do
                {
                    bFoundCollision = FALSE;
                    {

                        ACDCollNarrowResult tCollResult;

                        if (ACDCollNarrowCircle(tCollBroadResult, wc, idACDIgnore, tCollResult))
                        {
                            bFoundCollision = TRUE;
                            flCollisionDist = tCollResult.flDistance;
                            wpCollision = tCollResult.wpPosition.As2D();
                            flCollisionRadius = tCollResult.flACDRadius;

                            ACDID idACDHit = tCollBroadResult.arACDISect[tCollResult.nIndexHit].idACD;
                            ActorCommonData & tACDHit = ACDGet(idACDHit);
                            const ACDCollSettings & tACDHitCollSettings = tACDHit.tCollSettings;
                            uint32 uFlagsOverlap = tACDHitCollSettings.uDynamicMyFlags & uObjectNoPassthroughFlags;
                            if (uFlagsOverlap != 0)
                            {
                                bOkToPush = FALSE;
                            }
                        }
                    }
                    if (!bFoundCollision)
                    {
                        bFoundLocation = TRUE;
                        break;
                    }
                    {
                        BOOL bStaticCollOk = FALSE;
                        if (bOkToPush)
                        {

                            Vector2D vPush = wpCollEnd - wpCollision;
                            float flPushDist = flCollisionRadius + flLocationRadius - flCollisionDist + 0.1f;

                            if (vPush.MagnitudeSquared() < 1e-12f)
                            {
                                Angle aRandYaw = RandAngle(tRNG);
                                vPush.Set(Math::Cos(aRandYaw), Math::Sin(aRandYaw));
                            }
                            else
                            {
                                vPush.Normalize();
                            }

                            vPush *= flPushDist;

                            NavCellHandle hNavCellTestStart = tResult.hNavCellEnd;

                            if (!NavZoneCollideSweptPoint(hNavCellTestStart, wpCollEnd, vPush, uOneRequiredStaticFlags, 0, 0, tResult))
                            {
                                Vector2D vOffset = tResult.wpEnd - placeStart.wp.As2D();
                                if (vOffset.MagnitudeSquared() > flSearchRadiusMax * flSearchRadiusMax)
                                {

                                }
                                else
                                {

                                    wc.wpCenter = tResult.wpEnd;

                                    if (!NavZoneCollideCircle(tResult.hNavCellEnd, wc, uOneRequiredStaticFlags, 0, 0, tResultCircle))
                                    {
                                        bStaticCollOk = TRUE;
                                    }
                                }
                            }
                        }

                        if (bStaticCollOk)
                        {
                            wpCollEnd = tResult.wpEnd;
                        }
                        else
                        {
                            break;
                        }
                    }








                } while (++nNumDynamicTries < nMaxNumDynamicTries);

            }
            if (bFoundLocation)
            {
                break;
            }
        }

        flSearchRadius += flSearchRadiusDT;
    }

    if (bFoundLocation)
    {
        NavMeshHandle tNavMeshEnd;
        WorldPlace tPlaceEnd;

        tPlaceEnd.idSWorld = placeStart.idSWorld;
        tPlaceEnd.wp.Set(tResult.wpEnd.x, tResult.wpEnd.y, 0.0f);



        SceneCommonData * ptSCD = SceneCommonDataTryToGet(tResult.hNavCellEnd.idSScene);
        if (ptSCD == NULL)
        {
            return FALSE;
        }






        AABB aabbScene;
        SceneGetPhysicalAABBInWorldSpace(*ptSCD, aabbScene);
        aabbScene.wvExt.x -= 0.000001f;
        aabbScene.wvExt.y -= 0.000001f;
        tPlaceEnd.wp.x = Math::Pin(tPlaceEnd.wp.x, aabbScene.wp.x - aabbScene.wvExt.x, aabbScene.wp.x + aabbScene.wvExt.x);
        tPlaceEnd.wp.y = Math::Pin(tPlaceEnd.wp.y, aabbScene.wp.y - aabbScene.wvExt.y, aabbScene.wp.y + aabbScene.wvExt.y);

        BOOL bFoundPlace = ScenesCommonNavMeshNodeFromPlace(tPlaceEnd, &tNavMeshEnd, FALSE, FALSE);
        if (!bFoundPlace)
        {
            return FALSE;
        }







        float flZ = NavMeshGetHeightWorldSpace(tNavMeshEnd);
        hNavCellEnd = tResult.hNavCellEnd;
        wpEnd.Set(tResult.wpEnd.x, tResult.wpEnd.y, flZ);
    }

    return bFoundLocation;




}


BOOL WorldCollSearchForLocationOnLineSegment(
    const WorldPlace & placeStart,
    uint16 uOneRequiredStaticFlags,
    uint32 uObjectNoOverlapFlags,
    float flLocationRadius,
    const Vector3D & vDest,
    ACDID idACDIgnore,
    float flDesiredDistBetweenChecks,
    int32 nMaxChecks,
    Vector3D & wpEnd,
    Team eTeam)
{
    if (nMaxChecks < 1)
    {
        return FALSE;
    }

    wpEnd = placeStart.wp;
    Vector3D vDelta = vDest - placeStart.wp;
    float flIncLength;
    if (nMaxChecks - 1 == 0 || vDelta.MagnitudeSquared() <= 1e-12f)
    {
        vDelta = cgwpNull;
        flIncLength = 0.0f;
        nMaxChecks = 1;
    }
    else
    {
        float flLength = vDelta.Normalize();
        if (flDesiredDistBetweenChecks > 0.000001f)
        {
            float flNumSegments = flLength / flDesiredDistBetweenChecks;
            int32 nNumSegments = Math::FloatToInt(flNumSegments);
            if ((float)nNumSegments < flNumSegments)
            {
                nNumSegments++;
            }
            if (nNumSegments <= nMaxChecks)
            {
                nMaxChecks = nNumSegments;
            }
        }
        flIncLength = flLength / (float)(nMaxChecks - 1);
    }

    float flOffsetLength = 0.0f;
    for (int32 i = 0; i < nMaxChecks; i++)
    {
        Vector3D vOffset = vDelta * flOffsetLength;
        flOffsetLength += flIncLength;

        NavCellHandle hNavCellCheck;
        WorldPlace placeCheck;
        placeCheck.idSWorld = placeStart.idSWorld;
        placeCheck.wp = placeStart.wp + vOffset;
        NavCellHandleFromWorldPlace(placeCheck, hNavCellCheck);
        Circle wc;
        wc.Set(placeCheck.wp.As2D(), flLocationRadius);

        NavCellCollideResult tResultCircle;
        if (NavZoneCollideCircle(hNavCellCheck, wc, uOneRequiredStaticFlags, 0, 0, tResultCircle))
        {
            continue;
        }

        ACDCollParams tACDCollParams;
        tACDCollParams.uObjectNoOverlapFlags = uObjectNoOverlapFlags;
        tACDCollParams.eTeam = eTeam;

        ACDCollNarrowResult tNarrowResult;
        if (ACDCollCollideCircle(placeStart.idSWorld, wc, tACDCollParams, idACDIgnore, tNarrowResult))
        {
            continue;
        }

        wpEnd = placeCheck.wp;
        return TRUE;
    }

    return FALSE;
}

BOOL WorldCollGetArbitraryPointInScene(
    const SSceneID idSScene,
    uint16 uOneRequiredStaticFlags,
    uint32 uObjectNoOverlapFlags,
    Vector3D & wpResult,
    NavCellHandle & hNavCellResult)
{
    const NavZoneInstance * ptZoneInst = NavZoneInstanceFromSSceneID(idSScene);
    BOOL bFoundPoint = FALSE;

    if (ptZoneInst)
    {
        ScopedNavZoneDefLock tZoneLock(ptZoneInst);
        const NavZoneDefinition * ptZoneDef = ptZoneInst->GetZoneDef();

        uint32 uCount = ptZoneDef->uNavCellCount;

        for (uint32 i = 0; i < uCount; i++)
        {
            const NavCell & tNavCell = ptZoneDef->arNavCells[i];

            if (tNavCell.uFlags & uOneRequiredStaticFlags)
            {

                ACDCollBroadResult tBroadResult;
                ACDCollParams tCollParams;
                float flRadius;
                Vector2D wpCenter;

                tCollParams.uObjectNoOverlapFlags = uObjectNoOverlapFlags;
                Vector2D wpNavCellCenter = (tNavCell.tMin.As2D() + tNavCell.tMax.As2D()) * 0.5f;
                wpCenter = ptZoneInst->wpWorldMin + wpNavCellCenter;
                flRadius = Vector2D::Distance(wpNavCellCenter, tNavCell.tMin.As2D());

                ACDCollBroadCircle(ptZoneInst->idSWorld, wpCenter, flRadius, tCollParams, FALSE, tBroadResult, ACDID_INVALID);

                ACDCollNarrowResult tNarrowRes;
                Circle wc;
                wc.wdRadius = 1.0f;

                Vector2D vCellWidth(2.5f, 2.5f);
                Vector2D vHalfCellWidth(1.25f, 1.25f);
                Vector2D wpStart = ptZoneInst->wpWorldMin + tNavCell.tMin.As2D() + vHalfCellWidth;
                Vector2D wpEnd = ptZoneInst->wpWorldMin + tNavCell.tMax.As2D() - vHalfCellWidth;

                for (float y = wpStart.y; y <= wpEnd.y; y += vCellWidth.y)
                {
                    wc.wpCenter.y = y;

                    for (float x = wpStart.x; x <= wpEnd.x; x += vCellWidth.x)
                    {
                        wc.wpCenter.x = x;

                        if (!ACDCollNarrowCircle(tBroadResult, wc, ACDID_INVALID, tNarrowRes))
                        {
                            wpResult.Set(wc.wpCenter.x, wc.wpCenter.y, tNavCell.tMax.z);
                            hNavCellResult.idSScene = idSScene; hNavCellResult.uCellIndex = i;
                            bFoundPoint = TRUE;
                            break;
                        }
                    }

                    if (bFoundPoint)
                    {
                        break;
                    }
                }
            }

            if (bFoundPoint)
            {
                break;
            }
        }
    }

    return bFoundPoint;
}





BOOL WorldCollProjectileLineOfSight(
    const ActorCommonData & tACDSource,
    const NavCellHandle & hNavCellStart,
    const WorldPlace & placeStart,
    const WorldPlace & placeEnd,
    ACDID idACDIgnore)
{
    WorldCollParams tCollParams;
    tCollParams.uOneRequiredStaticFlags = 0x400;
    tCollParams.uObjectNoOverlapFlags = (1 << COLL_DYN_LOS_BLOCKER);

    WorldCollResult tResult;
    BOOL bCollided = WorldCollSweptPoint(placeStart.idSWorld, hNavCellStart, placeStart.wp.As2D(),
                                         placeEnd.wp.As2D(), tCollParams, 0, 0, idACDIgnore, tResult);









    if (!bCollided)
    {
        WorldPlace placeCurr = placeStart;
        Vector3D vToDest = placeEnd.wp - placeCurr.wp;
        Vector3D vCollPos;
        float flT;


        if (placeEnd.wp.z > placeCurr.wp.z)
        {
            vToDest = -vToDest;
            placeCurr.wp = placeEnd.wp;
        }

        placeCurr.wp.z += 3.0f;

        bCollided = NavZoneCollideSweptPoint3D(placeCurr.idSWorld, placeCurr.wp, vToDest, 0x400, 0, 0, flT, vCollPos);
    }

    if (!bCollided)
    {
        const ActorMovement * ptMove = tACDSource.GetActorMovement();
        if (ptMove)
        {

            WorldPlace placeCurr = placeStart;
            placeCurr.wp.z += 3.0f;

            WorldPlace placeDest = placeEnd;
            placeDest.wp.z += 3.0f;

            Vector3D vIntersect;
            Vector3D vNormal;
            float flUnitTraveled;
            bCollided = ptMove->RaycastAgainstWorld(placeCurr, placeDest, vIntersect, vNormal, flUnitTraveled, 0x2c0);
        }
    }

    return !bCollided;
}

BOOL WorldCollProjectileLineOfSight(ACDID idACDSource, const Target & tTarget)


{
    ActorCommonData * ptACDSource = ACDTryToGet(idACDSource);
    if (!ptACDSource)
    {
        return FALSE;
    }

    WorldPlace placeCurr = ptACDSource->GetPlace();
    WorldPlace placeTarget;
    tTarget.GetPlace(placeTarget);
    ACDID idACDTarget = tTarget.GetACD();

    const NavCellHandle * ptNavCellHandle;
    NavCellHandle hCalculatedNavCell;
    ActorMovement * ptActorMovement = ptACDSource->GetActorMovement();
    if (ptActorMovement)
    {
        ptNavCellHandle = &ptActorMovement->GetNavCell();
    }
    else
    {
        NavCellHandleFromWorldPlace(placeCurr, hCalculatedNavCell);
        ptNavCellHandle = &hCalculatedNavCell;
    }

    return WorldCollProjectileLineOfSight(*ptACDSource, *ptNavCellHandle, placeCurr, placeTarget, idACDTarget);
}
