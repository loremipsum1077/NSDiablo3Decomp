#include "NavZoneIntersect.h"
#include "NavZoneIntersectPrim.h"
#include "NavZone.h"
#include "../WorldPlace.h"
#include "../Definitions/SceneDefinition.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "NavigationMesh.h"
#include <float.h>
#include "NavMeshStitch.h"
const Vector2D & AABBGetNormal(AABBCollideFeature eCollFeature);  // X/Common/Nav/NavZoneIntersectPrim.cpp:73
AABBCollideFeature SweptCircleAndAABB2D(const Circle & wc, const Vector2D & wvDir, const float flDirMag, const Vector2D & wvDirNormalized, const Vector2D & wpMin, const Vector2D & wpMax, float & flT, Vector2D & wpCollision);  // X/Common/Nav/NavZoneIntersectPrim.cpp:271
AABBCollideFeature SweptCircleAndRoundedAABB2D(const Circle & wc, const Vector2D & wvDir, const float flDirMag, const Vector2D & wvDirNormalized, const Vector2D & wpMin, const Vector2D & wpMax, uint16 uFlags, float & flT, Vector2D & wpCollision);  // X/Common/Nav/NavZoneIntersectPrim.cpp
static void sGetCollNormal(AABBCollideFeature eCollFeature, const Vector2D & wpEnd, const Vector2D & wpCollision, Vector2D & wvNormal);  // defined at line 2451, used earlier in the file
AABBCollideFeature CircleAndAABB2D(const Circle & wc, const Vector2D & wpMin, const Vector2D & wpMax, float & flSqrDistance, Vector2D & wpCollision);  // X/Common/Nav/NavZoneIntersectPrim.cpp
AABBCollideFeature Ray2DAndAABB2D(const Vector2D wpStart, const Vector2D wvDir, const Vector2D wpAABBMin, const Vector2D wpAABBMax, float & flT, Vector2D & wpIsect);  // X/Common/Nav/NavZoneIntersectPrim.cpp:115
AABBCollideFeature SweptPointAndAABB2D(const Vector2D & wpStart, const Vector2D & wvDir, const Vector2D & wpMin, const Vector2D & wpMax, float & flT, Vector2D & wpCollision);  // X/Common/Nav/NavZoneIntersectPrim.cpp:399
AABBCollideFeature SweptPointAndRoundedAABB2D(const Vector2D & wpStart, const Vector2D & wvDir, const Vector2D & wpMin, const Vector2D & wpMax, uint16 uFlags, float & flT, Vector2D & wpCollision);  // X/Common/Nav/NavZoneIntersectPrim.cpp:408
inline void AABBFromSweptCircle(const Circle & wc, const Vector2D & wpDest, Vector2D & tMin, Vector2D & tMax) { float flMinX, flMaxX, flMinY, flMaxY; if (wc.wpCenter.x < wpDest.x) { flMinX = wc.wpCenter.x; flMaxX = wpDest.x; } else { flMinX = wpDest.x; flMaxX = wc.wpCenter.x; } if (wc.wpCenter.y < wpDest.y) { flMinY = wc.wpCenter.y; flMaxY = wpDest.y; } else { flMinY = wpDest.y; flMaxY = wc.wpCenter.y; } tMin.x = flMinX - wc.wdRadius; tMin.y = flMinY - wc.wdRadius; tMax.x = flMaxX + wc.wdRadius; tMax.y = flMaxY + wc.wdRadius; }  // body written here: DWARF puts it at NavZoneIntersectPrim.h:222 (not in src header); the compares are at 228 and 239, the four stores at 250, 251, 253, 254
NavMeshStitchAABBLookup * NavMeshStitchScenesInAABB(SWorldID idSWorld, Vector2D wpAABBMin, Vector2D wpAABBMax);  // X/Common/Nav/NavMeshStitch.cpp:899

int32 NavMeshStitchScenesInLineSegment(SWorldID idSWorld, Vector2D wpStart, Vector2D wpEnd, SSceneID * arSceneIDs, int32 nMaxSceneIDCount);  // X/Common/Nav/NavMeshStitch.cpp:1065
static AABBCollideFeature sgarAABBCollFeatureOpposites[10] = { AABB_COLLIDE_NONE, AABB_COLLIDE_XMAX, AABB_COLLIDE_XMIN, AABB_COLLIDE_YMAX, AABB_COLLIDE_YMIN, AABB_COLLIDE_RND0, AABB_COLLIDE_RND1, AABB_COLLIDE_RND2, AABB_COLLIDE_RND3, AABB_COLLIDE_VERTEX };
static void sInitializeResult(NavCellCollideResult & tResult, const Vector2D & wpDest);  // defined at line 2382, used earlier in the file
static void sFinalizeResultBadStart(NavCellCollideResult & tResult, const NavCellHandle & hNavCellStart, const Vector2D & wpStart);  // defined at line 2543, used earlier in the file
static BOOL sValidatePointInCell(const NavZoneInstance * ptZoneInstance, const Vector2D & wp, const NavCellHandle & hNavCell);  // defined at line 2310, used earlier in the file
static void sFinalizeResult(NavCellCollideResult & tResult, const NavZoneInstance * ptZoneInstance, const Vector2D & wpStart, const Vector2D & wvDirection, const Vector2D & wpColl, float flISectT, const NavCellHandle & hNavCellColl, AABBCollideFeature eCollFeature, BOOL bUseCollAsEnd);  // defined at line 2470, used earlier in the file
static void sFinalizeResultNoCollision(NavCellCollideResult & tResult, const NavCellHandle & hNavCellStart, const NavZoneInstance * ptZoneInstance);  // defined at line 2398, used earlier in the file
AABBCollideFeature AABBGetClosestFeature(Vector2D & wpColl, const Vector2D & wpAABBMin, const Vector2D & wpAABBMax);  // X/Common/Nav/NavZoneIntersectPrim.cpp:88
#include "../SceneCommon.h"
#include "../../Sigma/MemoryPools.h"
















































static void sWorldPosToGridPos(const NavZoneDefinition * ptZoneDef, const Vector2D & wpWorldPos, IVector2D & tGridPos)

{


    tGridPos.x = Math::FloatFloorToInt(wpWorldPos.x * ptZoneDef->flOOGridSizeFeet);
    tGridPos.y = Math::FloatFloorToInt(wpWorldPos.y * ptZoneDef->flOOGridSizeFeet);
}


static void sGetGridPos(const NavZoneDefinition * ptZoneDef, const Vector2D & wpLocalPos, const IVector2D & wpGridSize, IVector2D & tGridPos)

{



    sWorldPosToGridPos(ptZoneDef, wpLocalPos, tGridPos);

    tGridPos.x = Math::Pin(tGridPos.x, 0, wpGridSize.x - 1);
    tGridPos.y = Math::Pin(tGridPos.y, 0, wpGridSize.y - 1);
}




void QueryGridCellBlocks(const NavZoneDefinition * ptZoneDef, NavGridSquare & tSquare, uint16 uOneRequiredFlags, uint16 uPreventFlags, uint32 uCollFlags, uint32 * arNavCells, uint32 & uNavCellCount, BOOL & bHitChild)

{







    if (tSquare.uCellFlags & (1 << CELL_STATIC_SUBSCENE))
    {
        bHitChild = TRUE;
    }

    if (!(tSquare.uCellFlags & uOneRequiredFlags) || (tSquare.uCellFlags & uPreventFlags))
    {

        const NavCellLookup * arLookups = &ptZoneDef->arCellLookups[tSquare.uLookupOffset];
        for (uint16 i = 0; i < tSquare.uCellCount; i++)
        {
            const NavCellLookup & tLookup = arLookups[i];
            if ((!(uCollFlags & (1 << NAV_ZONE_COLL_IGNORE_REQUIRED_FLAG)) && !(tLookup.uCellFlags & uOneRequiredFlags)) ||
                (tLookup.uCellFlags & uPreventFlags))
            {
                int32 nDWordIndex = tLookup.uCellIndex / 32;
                int32 nBitIndex = tLookup.uCellIndex % 32;

                arNavCells[nDWordIndex] |= 1 << nBitIndex;


                uNavCellCount++;
            }
        }
    }
}



void NavZoneCellQueryAABB(const NavZoneInstance * ptZoneInstance, const Vector2D & wpBoxMin, const Vector2D & wpBoxMax, uint16 uOneRequiredFlags, uint16 uPreventFlags, uint32 uCollFlags, uint32 * arNavCells, uint32 & uNavCellCount, BOOL & bHitChild)









{
    uNavCellCount = 0;



    const NavZoneDefinition * ptZoneDef = ptZoneInstance->GetZoneDef();
    IVector2D tMin, tMax;
    sGetGridPos(ptZoneDef, wpBoxMin, ptZoneDef->tGridSize, tMin);
    sGetGridPos(ptZoneDef, wpBoxMax, ptZoneDef->tGridSize, tMax);

    bHitChild = FALSE;



    for (int y = tMin.y; y <= tMax.y; y++)
    {
        int nIndex = y * ptZoneDef->tGridSize.x + tMin.x;

        NavGridSquare * ptCurSquare = &ptZoneDef->arGridSquares[nIndex];

        for (int x = tMin.x; x <= tMax.x; x++)
        {
            QueryGridCellBlocks(ptZoneDef, *ptCurSquare, uOneRequiredFlags, uPreventFlags, uCollFlags, arNavCells, uNavCellCount, bHitChild);
            ptCurSquare++;
        }
    }
}



AABBCollideFeature NavMeshStaticIntersectSweptCircleBlockList(const NavZoneInstance * ptZoneInstance, uint32 * arCellIsect, uint32 uBitArrayCount, uint32 uCellCount, const Circle & wLocalCircle, const Vector2D & wvDirection, const float flDirMag, const Vector2D & wvDirectionNormalized, uint32 uCollFlags, float & flISectT, NavCellHandle & hNavCellColl, Vector2D & wpColl)
{












    uint16 uRoundedCornersMask = (1 << CELL_STATIC_ROUNDED_CORNER_0) | (1 << CELL_STATIC_ROUNDED_CORNER_1) | (1 << CELL_STATIC_ROUNDED_CORNER_2) | (1 << CELL_STATIC_ROUNDED_CORNER_3);

    BOOL bIgnoreCollIfMovingAway = (uCollFlags & (1 << NAV_ZONE_COLL_IGNORE_COLLISION_IF_MOVING_AWAY)) != 0;
    float fltmin = FLT_MAX;
    uint32 uIndexOffset = 0;
    uint32 uCount = 0;
    AABBCollideFeature eCollFeatureClosest = AABB_COLLIDE_NONE;
    const NavZoneDefinition * ptZoneDef = ptZoneInstance->GetZoneDef();
    for (uint i = 0; i < uBitArrayCount; i++)
    {
        uint32 uBlocks = arCellIsect[i];
        uint32 uIndex = uIndexOffset;
        uint32 uMask = 1;

        while (uBlocks)
        {
            if (uBlocks & uMask)
            {
                float flT;
                const NavCell & tNavCell = ptZoneDef->arNavCells[uIndex];
                Vector2D wpISect;

                AABBCollideFeature eCollFeature;

                if ((uCollFlags & (1 << NAV_ZONE_COLL_ROUNDED_CORNERS)) && (tNavCell.uFlags & uRoundedCornersMask))
                {
                    eCollFeature = SweptCircleAndRoundedAABB2D(wLocalCircle, wvDirection, flDirMag, wvDirectionNormalized, tNavCell.tMin.As2D(), tNavCell.tMax.As2D(), tNavCell.uFlags, flT, wpISect);
                }
                else
                {
                    eCollFeature = SweptCircleAndAABB2D(wLocalCircle, wvDirection, flDirMag, wvDirectionNormalized, tNavCell.tMin.As2D(), tNavCell.tMax.As2D(), flT, wpISect);
                }

                if (eCollFeature != AABB_COLLIDE_NONE && flT < fltmin)
                {


                    BOOL bRecordColl = TRUE;

                    if (bIgnoreCollIfMovingAway && flT < 1e-06f)
                    {
                        Vector2D wvNormal;

                        sGetCollNormal(eCollFeature, wLocalCircle.wpCenter, wpISect, wvNormal);

                        if (Vector2D::DotProduct(wvNormal, wvDirectionNormalized) >= 0.0f)
                        {
                            bRecordColl = FALSE;
                        }
                    }

                    if (bRecordColl)
                    {


                        hNavCellColl.idSScene = ptZoneInstance->idSScene;
                        hNavCellColl.uCellIndex = uIndex;
                        wpColl = wpISect;
                        fltmin = flT;
                        eCollFeatureClosest = eCollFeature;
                    }
                }

                uCount++;
                uBlocks &= ~uMask;
            }

            uMask <<= 1;
            uIndex++;
        }

        if (uCount == uCellCount)
        {
            break;
        }

        uIndexOffset += 32;
    }

    if (fltmin < FLT_MAX)
    {
        flISectT = fltmin;
        return eCollFeatureClosest;
    }

    return AABB_COLLIDE_NONE;
}


AABBCollideFeature NavMeshStaticIntersectCircleBlockList(const NavZoneInstance * ptZoneInstance, uint32 * arCellIsect, uint32 uBitArrayCount, uint32 uCellCount, const Circle & wLocalCircle, float & flSqrDistance, NavCellHandle & hNavCellColl, Vector2D & wpColl)
{








    float flSqrDistanceClosest = FLT_MAX;
    uint32 uIndexOffset = 0;
    uint32 uCount = 0;
    AABBCollideFeature eCollFeatureClosest = AABB_COLLIDE_NONE;
    const NavZoneDefinition * ptZoneDef = ptZoneInstance->GetZoneDef();
    for (uint i = 0; i < uBitArrayCount; i++)
    {
        uint32 uBlocks = arCellIsect[i];
        uint32 uIndex = uIndexOffset;
        uint32 uMask = 1;

        while (uBlocks)
        {
            if (uBlocks & uMask)
            {
                float flSqrDistanceTest;
                const NavCell & tNavCell = ptZoneDef->arNavCells[uIndex];
                Vector2D wpISect;

                AABBCollideFeature eCollFeature;

                eCollFeature = CircleAndAABB2D(wLocalCircle, tNavCell.tMin.As2D(), tNavCell.tMax.As2D(), flSqrDistanceTest, wpISect);

                if (eCollFeature != AABB_COLLIDE_NONE

                    && flSqrDistanceTest < flSqrDistanceClosest)
                {

                    hNavCellColl.idSScene = ptZoneInstance->idSScene;
                    hNavCellColl.uCellIndex = uIndex;
                    wpColl = wpISect;
                    flSqrDistanceClosest = flSqrDistanceTest;
                    eCollFeatureClosest = eCollFeature;
                }

                uCount++;
                uBlocks &= ~uMask;
            }

            uMask <<= 1;
            uIndex++;
        }

        if (uCount == uCellCount)
        {
            break;
        }

        uIndexOffset += 32;
    }

    if (flSqrDistanceClosest < FLT_MAX)
    {
        flSqrDistance = flSqrDistanceClosest;
        return eCollFeatureClosest;
    }

    return AABB_COLLIDE_NONE;
}


AABBCollideFeature NavZoneCollideSweptCircleOverGrids(const NavZoneInstance * ptZoneInstance, const Circle & wc, const Vector2D & wvDirection, const float flDirMag, const Vector2D & wvDirectionNormalized, const Vector2D & tWorldTestAABBMin, const Vector2D & tWorldTestAABBMax, uint16 uOneRequiredFlag, uint16 uPreventFlags, uint32 uCollFlags, float & flISectT, NavCellHandle & hNavCellColl, Vector2D & wpColl)
{













    const NavZoneDefinition * ptZoneDef = ptZoneInstance->GetZoneDef();
    if (ptZoneDef->uNavCellCount == 0)
    {
        return AABB_COLLIDE_NONE;
    }

    uint32 uBitArrayCount = NavZoneGetBitArrayCount(ptZoneDef->uNavCellCount);
    uint32 uBitArraySize = uBitArrayCount * sizeof(uint32);
    uint32 * arCellIsect = (uint32 *)alloca(uBitArraySize);

    SigmaMemoryClear(arCellIsect, uBitArraySize);

    uint32 uCellCount;
    BOOL bHitChild;

    Vector2D tLocalTestAABBMin, tLocalTestAABBMax;

    tLocalTestAABBMin = tWorldTestAABBMin - ptZoneInstance->wpWorldMin;
    tLocalTestAABBMax = tWorldTestAABBMax - ptZoneInstance->wpWorldMin;













    NavZoneCellQueryAABB(ptZoneInstance, tLocalTestAABBMin, tLocalTestAABBMax, uOneRequiredFlag, uPreventFlags, uCollFlags, arCellIsect, uCellCount, bHitChild);










    AABBCollideFeature eCollFeatureClosest = AABB_COLLIDE_NONE;

    flISectT = FLT_MAX;

    if (bHitChild)
    {
        if (ptZoneInstance->GetChildInstanceCount() == 0)
        {



            float flT;
            Vector2D wpCollision;
            AABBCollideFeature eCollFeature = SweptCircleAndAABB2D(wc, wvDirection, flDirMag, wvDirectionNormalized, tWorldTestAABBMin, tWorldTestAABBMax, flT, wpCollision);










            flISectT = flT;

            hNavCellColl.idSScene = SSCENEID_INVALID; hNavCellColl.uCellIndex = (uint32)-1;
            wpColl = wpCollision;
            eCollFeatureClosest = eCollFeature;
        }
        NavZoneInstance * arChildInst[16];
        uint32 uChildCount = ptZoneInstance->GetChildInstances(tWorldTestAABBMin, tWorldTestAABBMax, arChildInst);
        for (uint32 i = 0; i < uChildCount; i++)
        {
            ScopedNavZoneDefLock tZoneLock(arChildInst[i]);
            AABBCollideFeature eCollFeature;

            float flT;
            NavCellHandle hNavCell;
            Vector2D wpCollision;

            eCollFeature = NavZoneCollideSweptCircleOverGrids(arChildInst[i], wc, wvDirection, flDirMag, wvDirectionNormalized, tWorldTestAABBMin, tWorldTestAABBMax, uOneRequiredFlag, uPreventFlags, uCollFlags, flT, hNavCell, wpCollision);














            if (eCollFeature != AABB_COLLIDE_NONE && flT < flISectT)
            {

                flISectT = flT;
                hNavCellColl = hNavCell;
                wpColl = wpCollision;
                eCollFeatureClosest = eCollFeature;
            }
        }
    }
    if (uCellCount)
    {
        Circle wLocalCircle = wc;

        wLocalCircle.wpCenter -= ptZoneInstance->wpWorldMin;

        float flT;
        NavCellHandle hNavCell;
        Vector2D wpCollision;



        AABBCollideFeature eCollFeature = NavMeshStaticIntersectSweptCircleBlockList(ptZoneInstance, arCellIsect, uBitArrayCount, uCellCount, wLocalCircle, wvDirection, flDirMag, wvDirectionNormalized, uCollFlags, flT, hNavCell, wpCollision);













        if (eCollFeature != AABB_COLLIDE_NONE && flT < flISectT)
        {

            flISectT = flT;
            hNavCellColl = hNavCell;
            wpColl = wpCollision + ptZoneInstance->wpWorldMin;
            eCollFeatureClosest = eCollFeature;
        }
    }
    return eCollFeatureClosest;
}


AABBCollideFeature NavZoneCollideCircleOverGrids(const NavZoneInstance * ptZoneInstance, const Circle & wc, const Vector2D & tWorldTestAABBMin, const Vector2D & tWorldTestAABBMax, uint16 uOneRequiredFlag, uint16 uPreventFlags, uint32 uCollFlags, float & flSqrDistance, NavCellHandle & hNavCellColl, Vector2D & wpColl)
{










    const NavZoneDefinition * ptZoneDef = ptZoneInstance->GetZoneDef();
    if (ptZoneDef->uNavCellCount == 0)
    {
        return AABB_COLLIDE_NONE;
    }

    uint32 uBitArrayCount = NavZoneGetBitArrayCount(ptZoneDef->uNavCellCount);
    uint32 uBitArraySize = uBitArrayCount * sizeof(uint32);
    uint32 * arCellIsect = (uint32 *)alloca(uBitArraySize);

    SigmaMemoryClear(arCellIsect, uBitArraySize);

    uint32 uCellCount;
    BOOL bHitChild;

    Vector2D tLocalTestAABBMin, tLocalTestAABBMax;

    tLocalTestAABBMin = tWorldTestAABBMin - ptZoneInstance->wpWorldMin;
    tLocalTestAABBMax = tWorldTestAABBMax - ptZoneInstance->wpWorldMin;




    NavZoneCellQueryAABB(ptZoneInstance, tLocalTestAABBMin, tLocalTestAABBMax, uOneRequiredFlag, uPreventFlags, uCollFlags, arCellIsect, uCellCount, bHitChild);










    AABBCollideFeature eCollFeatureClosest = AABB_COLLIDE_NONE;

    flSqrDistance = FLT_MAX;

    if (bHitChild)
    {
        NavZoneInstance * arChildInst[16];

        uint32 uChildCount = ptZoneInstance->GetChildInstances(tWorldTestAABBMin, tWorldTestAABBMax, arChildInst);

        for (uint32 i = 0; i < uChildCount; i++)
        {
            ScopedNavZoneDefLock tZoneLock(arChildInst[i]);
            AABBCollideFeature eCollFeature;

            float flSqrDistanceTest;
            NavCellHandle hNavCell;
            Vector2D wpCollision;

            eCollFeature = NavZoneCollideCircleOverGrids(arChildInst[i], wc, tWorldTestAABBMin, tWorldTestAABBMax, uOneRequiredFlag, uPreventFlags, uCollFlags, flSqrDistanceTest, hNavCell, wpCollision);











            if (eCollFeature != AABB_COLLIDE_NONE && flSqrDistanceTest < flSqrDistance)
            {

                flSqrDistance = flSqrDistanceTest;
                hNavCellColl = hNavCell;
                wpColl = wpCollision;
                eCollFeatureClosest = eCollFeature;
            }
        }
    }
    if (uCellCount)
    {
        Circle wLocalCircle = wc;

        wLocalCircle.wpCenter -= ptZoneInstance->wpWorldMin;

        float flSqrDistanceTest;
        NavCellHandle hNavCell;
        Vector2D wpCollision;



        AABBCollideFeature eCollFeature = NavMeshStaticIntersectCircleBlockList(ptZoneInstance, arCellIsect, uBitArrayCount, uCellCount, wLocalCircle, flSqrDistanceTest, hNavCell, wpCollision);









        if (eCollFeature != AABB_COLLIDE_NONE && flSqrDistanceTest < flSqrDistance)
        {

            flSqrDistance = flSqrDistanceTest;
            hNavCellColl = hNavCell;
            wpColl = wpCollision + ptZoneInstance->wpWorldMin;
            eCollFeatureClosest = eCollFeature;
        }
    }
    return eCollFeatureClosest;
}

// NON_MATCHING: same size (1596); 10 insns differ: x9/x10 swapped for the arCellLookups index/base at lines 852-858 in both unswitched copies of the cell loop; tried k + uLookupOffset (no change)
AABBCollideFeature NavZoneCollideSweptPointOverGrids(
    const NavZoneInstance * ptZoneInstance,
    const Vector2D & wpStart,
    const Vector2D & wpEnd,
    const Vector2D & wvDirection,
    uint16 uOneRequiredFlag,
    uint16 uPreventFlags,
    uint32 uCollFlags,
    float & flISectT,
    NavCellHandle & hNavCellColl,
    Vector2D & wpColl)
{
    const NavZoneDefinition * ptZoneDef = ptZoneInstance->GetZoneDef();



    Vector2D wpStartLocal = wpStart - ptZoneInstance->wpWorldMin;

    Vector2D wpStartGrid = wpStartLocal;
    Vector2D wpEndGrid = wpEnd - ptZoneInstance->wpWorldMin;

    Vector2D wpZoneMin; wpZoneMin.x = 0.0f; wpZoneMin.y = 0.0f;
    Vector2D wpZoneMax; wpZoneMax.x = ptZoneDef->flGridSizeFeet * ptZoneDef->tGridSize.x; wpZoneMax.y = ptZoneDef->flGridSizeFeet * ptZoneDef->tGridSize.y;
    // inferred: tests wpStartGrid (equal to wpStartLocal here; DWARF cannot tell). Testing wpStartGrid gives the original x-min/x-max/y-min/y-max branch order, wpStartLocal an fccmp chain
    if (!Point2DInsideAABB2D(wpStartGrid, wpZoneMin, wpZoneMax))
    {
        Vector2D wpISect;
        float flT;
        AABBCollideFeature eCollFeature = Ray2DAndAABB2D(wpStartGrid, wvDirection, wpZoneMin, wpZoneMax, flT, wpISect);


        if (eCollFeature == AABB_COLLIDE_NONE)
        {
            return AABB_COLLIDE_NONE;
        }

        wpStartGrid = wpISect;
    }

    if (!Point2DInsideAABB2D(wpEndGrid, wpZoneMin, wpZoneMax))
    {
        Vector2D wpISect;
        float flT;
        AABBCollideFeature eCollFeature = Ray2DAndAABB2D(wpEndGrid, -wvDirection, wpZoneMin, wpZoneMax, flT, wpISect);






        if (eCollFeature == AABB_COLLIDE_NONE)
        {
            return AABB_COLLIDE_NONE;
        }


        wpEndGrid = wpISect;
    }


    int i = Math::FloatFloorToInt(wpStartGrid.x * ptZoneDef->flOOGridSizeFeet);
    int j = Math::FloatFloorToInt(ptZoneDef->flOOGridSizeFeet * wpStartGrid.y);


    int iend = Math::FloatFloorToInt(wpEndGrid.x * ptZoneDef->flOOGridSizeFeet);
    int jend = Math::FloatFloorToInt(wpEndGrid.y * ptZoneDef->flOOGridSizeFeet);


    int di = (wpStartGrid.x < wpEndGrid.x) ? 1 : ((wpStartGrid.x > wpEndGrid.x) ? -1 : 0);
    int dj = (wpEndGrid.y > wpStartGrid.y) ? 1 : ((wpEndGrid.y < wpStartGrid.y) ? -1 : 0);

    float adx = Math::Abs(wpEndGrid.x - wpStartGrid.x);
    float ady = Math::Abs(wpEndGrid.y - wpStartGrid.y);





    float minx = ptZoneDef->flGridSizeFeet * floorf(wpStartGrid.x * ptZoneDef->flOOGridSizeFeet);
    float maxx = minx + ptZoneDef->flGridSizeFeet;
    float tx = (wpStartGrid.x > wpEndGrid.x ? wpStartGrid.x - minx : maxx - wpStartGrid.x) / adx;

    float miny = ptZoneDef->flGridSizeFeet * floorf(ptZoneDef->flOOGridSizeFeet * wpStartGrid.y);
    float maxy = miny + ptZoneDef->flGridSizeFeet;
    float ty = (wpStartGrid.y > wpEndGrid.y ? wpStartGrid.y - miny : maxy - wpStartGrid.y) / ady;




    float deltatx = ptZoneDef->flGridSizeFeet / adx;
    float deltaty = ptZoneDef->flGridSizeFeet / ady;

    int nOffset = i + j * ptZoneDef->tGridSize.x;
    int nGridSize = ptZoneDef->tGridSize.x * ptZoneDef->tGridSize.y;

    BOOL bRoundedCorners = uCollFlags & (1 << NAV_ZONE_COLL_ROUNDED_CORNERS);
    uint16 uSubScene = (1 << CELL_STATIC_SUBSCENE);
    NavZoneInstance * arChildInstTested[16];
    uint32 uChildInstTested = 0;

    flISectT = FLT_MAX;

    while (TRUE)
    {


        if (nOffset >= 0 && nOffset < nGridSize)
        {



            const NavGridSquare & tGridSquare = ptZoneDef->arGridSquares[nOffset];

            Vector2D wpCollClosest;
            AABBCollideFeature eCollFeatureClosest = AABB_COLLIDE_NONE;

            if (tGridSquare.uCellFlags & uSubScene)
            {
                NavZoneInstance * arChildInst[16];
                Vector2D tGridMin, tGridMax;

                tGridMin.x = ptZoneDef->flGridSizeFeet * i;
                tGridMin.y = ptZoneDef->flGridSizeFeet * j;

                tGridMin += ptZoneInstance->wpWorldMin;

                tGridMax.x = tGridMin.x + ptZoneDef->flGridSizeFeet;
                tGridMax.y = tGridMin.y + ptZoneDef->flGridSizeFeet;

                uint32 uChildCount = ptZoneInstance->GetChildInstances(tGridMin, tGridMax, arChildInst);
                if (uChildCount == 0)
                {



                    float flT;
                    Vector2D wpCollision;
                    AABBCollideFeature eCollFeature = SweptPointAndAABB2D(wpStartLocal, wvDirection, tGridMin, tGridMax, flT, wpCollision);







                    flISectT = flT;
                    wpCollClosest = wpCollision;
                    eCollFeatureClosest = eCollFeature;
                    // line table: the handle store is the row at 793, after flISectT (789) and wpCollClosest (790)
                    hNavCellColl.idSScene = SSCENEID_INVALID; hNavCellColl.uCellIndex = (uint32)-1;
                }
                else
                for (uint it1 = 0; it1 < uChildCount; it1++)
                {
                    BOOL bAlreadyChecked = FALSE;


                    for (uint it2 = 0; it2 < uChildInstTested; it2++)
                        {
                            if (arChildInstTested[it2] == arChildInst[it1])
                            {
                                bAlreadyChecked = TRUE;
                                break;
                            }
                        }

                        if (!bAlreadyChecked)
                        {
                            ScopedNavZoneDefLock tZoneLock(arChildInst[it1]);
                            arChildInstTested[uChildInstTested] = arChildInst[it1];
                            uChildInstTested++;

                            AABBCollideFeature eCollFeature;
                            float flT;
                            NavCellHandle hNavCell;
                            Vector2D wpCollision;

                            eCollFeature = NavZoneCollideSweptPointOverGrids(arChildInst[it1], wpStart, wpEnd, wvDirection, uOneRequiredFlag, uPreventFlags, uCollFlags, flT, hNavCell, wpCollision);











                            if (eCollFeature != AABB_COLLIDE_NONE && flT < flISectT)
                            {

                                flISectT = flT;
                                wpCollClosest = wpCollision;
                                hNavCellColl = hNavCell;
                                eCollFeatureClosest = eCollFeature;
                            }
                        }
                    }

            }
            if (!(tGridSquare.uCellFlags & uOneRequiredFlag) || (tGridSquare.uCellFlags & uPreventFlags))
            {

                uint32 uLookupOffset = tGridSquare.uLookupOffset;

                for (int k = 0; k < tGridSquare.uCellCount; k++)
                {
                    const NavCellLookup & tLookup = ptZoneDef->arCellLookups[uLookupOffset + k];


                    if (!(tLookup.uCellFlags & uOneRequiredFlag) ||
                        (tLookup.uCellFlags & uPreventFlags))
                    {
                        const NavCell & tNavCell = ptZoneDef->arNavCells[tLookup.uCellIndex];

                        AABBCollideFeature eCollFeature;
                        float flT;
                        Vector2D wpCollision;

                        if (bRoundedCorners)
                        {
                            eCollFeature = SweptPointAndRoundedAABB2D(wpStartLocal, wvDirection, tNavCell.tMin.As2D(), tNavCell.tMax.As2D(), tNavCell.uFlags, flT, wpCollision);
                        }
                        else
                        {
                            eCollFeature = SweptPointAndAABB2D(wpStartLocal, wvDirection, tNavCell.tMin.As2D(), tNavCell.tMax.As2D(), flT, wpCollision);
                        }

                        if (eCollFeature != AABB_COLLIDE_NONE && flT < flISectT)
                        {


                            flISectT = flT;
                            wpCollClosest = wpCollision + ptZoneInstance->wpWorldMin;
                            hNavCellColl.uCellIndex = tLookup.uCellIndex;
                            hNavCellColl.idSScene = ptZoneInstance->idSScene;
                            eCollFeatureClosest = eCollFeature;
                        }
                    }
                }
            }

            if (eCollFeatureClosest != AABB_COLLIDE_NONE)
            {
                wpColl = wpCollClosest + ptZoneInstance->wpWorldMin;
                return eCollFeatureClosest;
            }
        }

        if (tx <= ty)
        {
            if (i == iend)
            {
                return AABB_COLLIDE_NONE;
            }

            tx += deltatx;
            i += di;
            nOffset += di;
        }
        else
        {
            if (j == jend)
            {
                return AABB_COLLIDE_NONE;
            }

            ty += deltaty;
            j += dj;
            nOffset += ptZoneDef->tGridSize.x * dj;
        }
    }
}



// NON_MATCHING: 2164 vs 2172 bytes; the call list, constants and control flow are the original's; the blocked-start finalize block (977-987) is placed right after the zone-bounds compares where the original has it after the scene loops, the 4th AABB2DInsideAABB2D compare (1032) is out of line with a copy of eCollFeature (as in NavZoneCollideCircle), registers differ (wc in x25 vs x26, spill slots). The original's hNavCellEnd store is row 993:33 with no row at 990-992 (ours 992); tried wc.wdRadius + flMaxX in AABBFromSweptCircle (no change).
BOOL NavZoneCollideSweptCircle(
    const NavCellHandle & hNavCellStart,
    const Circle & wc,
    const Vector2D & wvDirection,
    uint16 uOneRequiredFlag,
    uint16 uPreventFlags,
    uint32 uCollFlags,
    NavCellCollideResult & tResult)
{



    float flISectT = 1.0f;
    Vector2D wpDest = wc.wpCenter + wvDirection;
    Vector2D tWorldTestAABBMin, tWorldTestAABBMax;

    sInitializeResult(tResult, wpDest);



    AABBFromSweptCircle(wc, wpDest, tWorldTestAABBMin, tWorldTestAABBMax);

    const NavZoneInstance * ptZoneInstance = NavCellHandleToZoneInstance(hNavCellStart);

    if (!ptZoneInstance)
    {
        sFinalizeResultBadStart(tResult, hNavCellStart, wc.wpCenter);
        return TRUE;
    }

    ScopedNavZoneDefLock tZoneDefLock(ptZoneInstance);
    const NavZoneDefinition * ptZoneDef = ptZoneInstance->GetZoneDef();


    if (!sValidatePointInCell(ptZoneInstance, wc.wpCenter, hNavCellStart))
    {
        tResult.flT = 0.0f;
        return FALSE;
    }


    Vector2D tLocalTestAABBMin = tWorldTestAABBMin - ptZoneInstance->wpWorldMin;
    Vector2D tLocalTestAABBMax = tWorldTestAABBMax - ptZoneInstance->wpWorldMin;

    {
        const NavCell * ptCurCell = NavCellHandleToNavCell(ptZoneDef, hNavCellStart);


        if ((!(ptCurCell->uFlags & uOneRequiredFlag) && !(uCollFlags & (1 << NAV_ZONE_COLL_IGNORE_REQUIRED_FLAG))) ||
            (ptCurCell->uFlags & uPreventFlags))
        {
            // inferred: the mask 3 is (1 << NAV_ZONE_COLL_ROUNDED_CORNERS) | (1 << NAV_ZONE_COLL_IGNORE_COLLISION_IF_MOVING_AWAY) (or the unshifted NAV_ZONE_COLL_IGNORE_INVALID_SCENES = 3); the original (asm 660378) skips the 990 test when this block falls through, hence the else if
            if (!(uCollFlags & ((1 << NAV_ZONE_COLL_ROUNDED_CORNERS) | (1 << NAV_ZONE_COLL_IGNORE_COLLISION_IF_MOVING_AWAY))))
            {

                AABBCollideFeature eCollFeature;
                Vector2D wpColl;

                wpColl = wc.wpCenter;


                eCollFeature = AABBGetClosestFeature(wpColl, ptCurCell->tMin.As2D(), ptCurCell->tMax.As2D());

                sFinalizeResult(tResult, ptZoneInstance, wc.wpCenter, wvDirection, wpColl, 0.0f, hNavCellStart, eCollFeature, FALSE);
                return TRUE;
            }
        }

        else if (AABB2DInsideAABB2D(tLocalTestAABBMin, tLocalTestAABBMax, ptCurCell->tMin.As2D(), ptCurCell->tMax.As2D()))
        {
            tResult.hNavCellEnd = hNavCellStart;
            return FALSE;
        }

    }


    Vector2D wvDirectionNormalized = wvDirection;
    float flDirMag;
    // inferred: flDirMag (1000) is declared apart from the Normalize call (1002), DWARF decl line 1000
    flDirMag = wvDirectionNormalized.Normalize();
    // inferred: fltmin starts at FLT_MAX (asm loads 0x7f7fffff) and 1184 tests it with >=, 1204 with <
    float fltmin = FLT_MAX;
    AABBCollideFeature eCollFeatureClosest = AABB_COLLIDE_NONE;
    NavCellHandle hNavCellCollClosest;
    Vector2D wpCollClosest;
    const NavZoneInstance * ptZoneInstanceClosest = NULL;
    const NavZoneInstance * ptZoneInstanceNoCollision = ptZoneInstance;
    // inferred: ptZoneInstanceNoCollision starts as ptZoneInstance (asm reloads [sp+0x38] at 1184)
    {
        AABBCollideFeature eCollFeature;
        NavCellHandle hNavCellColl;
        Vector2D wpColl;

        eCollFeature = NavZoneCollideSweptCircleOverGrids(ptZoneInstance, wc, wvDirection, flDirMag, wvDirectionNormalized, tWorldTestAABBMin, tWorldTestAABBMax, uOneRequiredFlag, uPreventFlags, uCollFlags, flISectT, hNavCellColl, wpColl);















        if (AABB2DInsideAABB2D(tWorldTestAABBMin, tWorldTestAABBMax, ptZoneInstance->wpWorldMin, ptZoneInstance->wpWorldMax))
        {
            if (eCollFeature != AABB_COLLIDE_NONE)
            {
                sFinalizeResult(tResult, ptZoneInstance, wc.wpCenter, wvDirection, wpColl, flISectT, hNavCellColl, eCollFeature, FALSE);
                return TRUE;
            }


            sFinalizeResultNoCollision(tResult, hNavCellStart, ptZoneInstance);
            return FALSE;
        }

        if (eCollFeature != AABB_COLLIDE_NONE)
        {

            fltmin = flISectT;
            eCollFeatureClosest = eCollFeature;
            hNavCellCollClosest = hNavCellColl;
            wpCollClosest = wpColl;
            ptZoneInstanceClosest = ptZoneInstance;
        }
    }



    SSceneID idSScene = hNavCellStart.idSScene;
    SWorldID idSWorld = ptZoneInstance->idSWorld;

    NavMeshStitchAABBLookup * ptAABBLookupRes = NavMeshStitchScenesInAABB(idSWorld, tWorldTestAABBMin, tWorldTestAABBMax);
    if (!ptAABBLookupRes)
    {
        return FALSE;
    }

    XPooledDataIDList listScenes(MemoryPoolsGetDATAIDListPool());
    int32 nSceneCount = ptAABBLookupRes->tSize.x * ptAABBLookupRes->tSize.y;
    for (int i = 0; i < nSceneCount; i++)
    {
        SSceneID idSSceneCurr = ptAABBLookupRes->arScenes[i];
        if (idSSceneCurr == idSScene)
        {
            continue;
        }


        SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSSceneCurr);
        if (!ptSCD)
        {


            int x = i % ptAABBLookupRes->tSize.x;
            int y = i / ptAABBLookupRes->tSize.x;

            Vector2D wpSceneMin, wpSceneMax;
            wpSceneMin = Vector2D(ptAABBLookupRes->vGridSize.x * x, ptAABBLookupRes->vGridSize.y * y) + ptAABBLookupRes->wpGridOffset;
            wpSceneMax = wpSceneMin + ptAABBLookupRes->vGridSize;

            float flT;
            Vector2D wpColl;
            AABBCollideFeature eCollFeature;

            eCollFeature = SweptCircleAndAABB2D(wc, wvDirection, flDirMag, wvDirectionNormalized, wpSceneMin, wpSceneMax, flT, wpColl);
            if (eCollFeature != AABB_COLLIDE_NONE && flT < fltmin)
            {
                fltmin = flT;
                ptZoneInstanceClosest = ptZoneInstance;
                hNavCellCollClosest.idSScene = SSCENEID_INVALID; hNavCellCollClosest.uCellIndex = (uint32)-1;
                eCollFeatureClosest = eCollFeature;
                wpCollClosest = wpColl;
            }
        }
        else
        {



            POS pos = listScenes.GetHeadPosition();
            while (pos)
            {
                SSceneID idSSceneInList = listScenes.GetNext(pos);
            }






            listScenes.AddTail_Tracked(idSSceneCurr);
            pos = ptSCD->tChildSceneIDs.GetHeadPosition();
            while (pos)
            {
                SSceneID idSSceneChild = ptSCD->tChildSceneIDs.GetNext(pos);
                listScenes.AddTail_Tracked(idSSceneChild);
            }
        }
    }

    POS pos = listScenes.GetHeadPosition();
    while (pos)
    {
        SSceneID idSSceneCur = listScenes.GetNext(pos);
        // inferred: bare block; the DWARF puts ptCurZoneInst (1136) in a block inside idSSceneCur's, and rows 1137-1169 are 4 columns right
        {
            const NavZoneInstance * ptCurZoneInst = NavZoneInstanceFromSSceneID(idSSceneCur);
            if (ptCurZoneInst)
            {
                ScopedNavZoneDefLock tCurZoneDefLock(ptCurZoneInst);

                float flT;
                AABBCollideFeature eCollFeature;
                NavCellHandle hNavCellColl;
                Vector2D wpColl;

                eCollFeature = NavZoneCollideSweptCircleOverGrids(ptCurZoneInst, wc, wvDirection, flDirMag, wvDirectionNormalized, tWorldTestAABBMin, tWorldTestAABBMax, uOneRequiredFlag, uPreventFlags, uCollFlags, flT, hNavCellColl, wpColl);














                if (eCollFeature != AABB_COLLIDE_NONE)
                {


                    if (flT < fltmin)
                    {
                        fltmin = flT;
                        eCollFeatureClosest = eCollFeature;
                        hNavCellCollClosest = hNavCellColl;
                        wpCollClosest = wpColl;
                        ptZoneInstanceClosest = ptCurZoneInst;
                    }
                }

                if (Point2DInsideAABB2D(wpDest, ptCurZoneInst->wpWorldMin, ptCurZoneInst->wpWorldMax))
                {
                    ptZoneInstanceNoCollision = ptCurZoneInst;
                }
            }
        }
    }

    // inferred: 1184:16 and 1185:111 are the >= and wpWorldMax columns of one condition split over two lines (&&)
    if (fltmin >= FLT_MAX &&
        !Point2DInsideAABB2D(tResult.wpEnd, ptZoneInstanceNoCollision->wpWorldMin, ptZoneInstanceNoCollision->wpWorldMax))
    {
        float flT;
        Vector2D wpColl;
        AABBCollideFeature eCollFeature;
        eCollFeature = SweptCircleAndAABB2D(wc, wvDirection, flDirMag, wvDirectionNormalized, ptZoneInstanceNoCollision->wpWorldMin, ptZoneInstanceNoCollision->wpWorldMax, flT, wpColl);


        if (eCollFeature != AABB_COLLIDE_NONE)
        {


            hNavCellCollClosest.idSScene = SSCENEID_INVALID; hNavCellCollClosest.uCellIndex = (uint32)-1;
            fltmin = flT;
            wpCollClosest = wpColl;
            eCollFeatureClosest = eCollFeature;
            ptZoneInstanceClosest = ptZoneInstance;
        }
    }
    if (fltmin < FLT_MAX)
    {
        flISectT = fltmin;

        sFinalizeResult(tResult, ptZoneInstanceClosest, wc.wpCenter, wvDirection, wpCollClosest, flISectT, hNavCellCollClosest, eCollFeatureClosest, FALSE);
        return TRUE;
    }










    sFinalizeResultNoCollision(tResult, hNavCellStart, ptZoneInstanceNoCollision);
    return FALSE;

}

// NON_MATCHING: 1300 vs 1288 bytes; statement layout from the line table columns (params on 1228-1233 and { at 1234 per DWARF, else if at 1337 from column 19, 2390 starts with hNavCellCollision from column 40); left: sInitializeResult store merging (ours merges flT with wvNormal.x), the 4th AABB2DInsideAABB2D compare (1324) placed out of line with a copy of eCollFeature (same as NavZoneCollideSweptCircle), the scene-loop latch not tail-duplicated after the lock scope, spill slots; tried if (!bAlreadyTested) { ... } instead of continue (no change)
BOOL NavZoneCollideCircle(
    const NavCellHandle & hNavCellStart,
    const Circle & wc,
    uint16 uOneRequiredFlag,
    uint16 uPreventFlags,
    uint32 uCollFlags,
    NavCellCollideResult & tResult)
{


    Vector2D tWorldTestAABBMin, tWorldTestAABBMax;
    Vector2D wvDirection(0.0f, 0.0f);

    sInitializeResult(tResult, wc.wpCenter);



    Vector2D vRadius; vRadius.x = wc.wdRadius; vRadius.y = wc.wdRadius;
    tWorldTestAABBMin = wc.wpCenter - vRadius;
    tWorldTestAABBMax = wc.wpCenter + vRadius;

    const NavZoneInstance * ptZoneInstance = NavCellHandleToZoneInstance(hNavCellStart);

    if (!ptZoneInstance)
    {
        sFinalizeResultBadStart(tResult, hNavCellStart, wc.wpCenter);
        return TRUE;
    }

    ScopedNavZoneDefLock tZoneDefLock(ptZoneInstance);
    const NavZoneDefinition * ptZoneDef = ptZoneInstance->GetZoneDef();


    if (!sValidatePointInCell(ptZoneInstance, wc.wpCenter, hNavCellStart))
    {
        tResult.flT = 0.0f;
        return FALSE;
    }


    Vector2D tLocalTestAABBMin = tWorldTestAABBMin - ptZoneInstance->wpWorldMin;
    Vector2D tLocalTestAABBMax = tWorldTestAABBMax - ptZoneInstance->wpWorldMin;

    {
        const NavCell * ptCurCell = NavCellHandleToNavCell(ptZoneDef, hNavCellStart);


        if ((!(ptCurCell->uFlags & uOneRequiredFlag) && !(uCollFlags & (1 << NAV_ZONE_COLL_IGNORE_REQUIRED_FLAG))) ||
            (ptCurCell->uFlags & uPreventFlags))
        {

            AABBCollideFeature eCollFeature;
            Vector2D wpColl;

            wpColl = wc.wpCenter;


            eCollFeature = AABBGetClosestFeature(wpColl, ptCurCell->tMin.As2D(), ptCurCell->tMax.As2D());

            sFinalizeResult(tResult, ptZoneInstance, wc.wpCenter, wvDirection, wpColl, 0.0f, hNavCellStart, eCollFeature, FALSE);
            return TRUE;
        }


        if (AABB2DInsideAABB2D(tLocalTestAABBMin, tLocalTestAABBMax, ptCurCell->tMin.As2D(), ptCurCell->tMax.As2D()))
        {
            tResult.hNavCellEnd = hNavCellStart;
            return FALSE;
        }
    }


    float flSqrDistanceClosest = FLT_MAX;
    AABBCollideFeature eCollFeatureClosest = AABB_COLLIDE_NONE;
    NavCellHandle hNavCellCollClosest;
    Vector2D wpCollClosest;
    const NavZoneInstance * ptZoneInstanceClosest = NULL;

    {
        AABBCollideFeature eCollFeature;
        NavCellHandle hNavCellColl;
        Vector2D wpColl;
        float flSqrDistance;

        eCollFeature = NavZoneCollideCircleOverGrids(ptZoneInstance, wc, tWorldTestAABBMin, tWorldTestAABBMax, uOneRequiredFlag, uPreventFlags, uCollFlags, flSqrDistance, hNavCellColl, wpColl);












        if (AABB2DInsideAABB2D(tWorldTestAABBMin, tWorldTestAABBMax, ptZoneInstance->wpWorldMin, ptZoneInstance->wpWorldMax))
        {
            if (eCollFeature != AABB_COLLIDE_NONE)
            {
                sFinalizeResult(tResult, ptZoneInstance, wc.wpCenter, wvDirection, wpColl, 0.0f, hNavCellColl, eCollFeature, FALSE);
                return TRUE;
            }

            sFinalizeResultNoCollision(tResult, hNavCellStart, ptZoneInstance);
            return FALSE;
        }


        else if (eCollFeature != AABB_COLLIDE_NONE)
        {


            hNavCellCollClosest = hNavCellColl;
            flSqrDistanceClosest = flSqrDistance;
            wpCollClosest = wpColl;
            ptZoneInstanceClosest = ptZoneInstance;
            eCollFeatureClosest = eCollFeature;
        }
    }


    const int32 MAX_SCENES_INTERSECT = 64;
    SSceneID arSScenes[MAX_SCENES_INTERSECT];
    int nSceneTestCount = 0;

    SSceneID idSScene = hNavCellStart.idSScene;
    SWorldID idSWorld = ptZoneInstance->idSWorld;

    NavMeshStitchAABBLookup * ptAABBLookupRes = NavMeshStitchScenesInAABB(idSWorld, tWorldTestAABBMin, tWorldTestAABBMax);
    if (!ptAABBLookupRes)
    {
        return FALSE;
    }
    // line table: nSceneCount (1363, columns 48/50) and the scene loop (1365, column 5) sit at function depth, so the NULL test above is an early return; operand order tSize.x * tSize.y from column 48
    int32 nSceneCount = ptAABBLookupRes->tSize.x * ptAABBLookupRes->tSize.y;

    for (int i = 0; i < nSceneCount; i++)
    {
        SSceneID idSSceneCur = ptAABBLookupRes->arScenes[i];

        if (idSSceneCur == idSScene)
        {
            continue;
        }

        // line table: this test is row 1375; the bAlreadyTested loop (1408, column 9) is at loop-body depth, so this block ends with continue (no else)
        if (idSSceneCur == SSCENEID_INVALID)
        {


            Vector2D wpSceneMin, wpSceneMax;
            // line table: x and y computed at 1381 (columns 23 '%', 48 tSize.x); y's division shares the sdiv
            int x = i % ptAABBLookupRes->tSize.x;
            int y = i / ptAABBLookupRes->tSize.x;

            wpSceneMin = Vector2D(ptAABBLookupRes->vGridSize.x * x, ptAABBLookupRes->vGridSize.y * y) + ptAABBLookupRes->wpGridOffset;
            wpSceneMax = wpSceneMin + ptAABBLookupRes->vGridSize;

            float flSqrDistance;
            Vector2D wpColl;
            AABBCollideFeature eCollFeature;

            eCollFeature = CircleAndAABB2D(wc, wpSceneMin, wpSceneMax, flSqrDistance, wpColl);

            if (eCollFeature != AABB_COLLIDE_NONE && flSqrDistance < flSqrDistanceClosest)
            {
                flSqrDistanceClosest = flSqrDistance;
                ptZoneInstanceClosest = ptZoneInstance;
                hNavCellCollClosest.idSScene = SSCENEID_INVALID;
                hNavCellCollClosest.uCellIndex = (uint32)-1;
                wpCollClosest = wpColl;
                eCollFeatureClosest = eCollFeature;
            }

            continue;
        }


        BOOL bAlreadyTested = FALSE;
        for (int j = 0; j < nSceneTestCount; j++)
        {
            if (arSScenes[j] == idSSceneCur)
            {
                bAlreadyTested = TRUE;
                break;
            }
        }

        if (bAlreadyTested)
        {
            continue;
        }


        arSScenes[nSceneTestCount] = idSSceneCur;
        nSceneTestCount++;

        {
            const NavZoneInstance * ptCurZoneInst = NavZoneInstanceFromSSceneID(idSSceneCur);
            ScopedNavZoneDefLock tCurZoneDefLock(ptCurZoneInst);

            float flSqrDistance;
            AABBCollideFeature eCollFeature;
            NavCellHandle hNavCellColl;
            Vector2D wpColl;

            eCollFeature = NavZoneCollideCircleOverGrids(ptCurZoneInst, wc, tWorldTestAABBMin, tWorldTestAABBMax, uOneRequiredFlag, uPreventFlags, uCollFlags, flSqrDistance, hNavCellColl, wpColl);











            if (eCollFeature != AABB_COLLIDE_NONE)
            {


                if (flSqrDistance < flSqrDistanceClosest)
                {
                    flSqrDistanceClosest = flSqrDistance;
                    ptZoneInstanceClosest = ptCurZoneInst;
                    hNavCellCollClosest = hNavCellColl;
                    wpCollClosest = wpColl;
                    eCollFeatureClosest = eCollFeature;
                }
            }
        }
    }

    if (flSqrDistanceClosest < FLT_MAX)
    {
        sFinalizeResult(tResult, ptZoneInstanceClosest, wc.wpCenter, wvDirection, wpCollClosest, 0.0f, hNavCellCollClosest, eCollFeatureClosest, FALSE);
        return TRUE;
    }

    sFinalizeResultNoCollision(tResult, hNavCellStart, ptZoneInstance);

    return FALSE;









}


BOOL sZoneInstanceIsValid(const NavZoneInstance * ptZoneInst)
{

    BOOL bValid = FALSE;
    // line table: rows 1489 (column 9, NULL test of ptZoneInst), 1493 (column 13, ptZoneDef), 1495 (column 28, uNavCellCount); lock destructor at 1500
    if (ptZoneInst)
    {
        ScopedNavZoneDefLock tZoneDefLock(ptZoneInst);
        const NavZoneDefinition * ptZoneDef = ptZoneInst->GetZoneDef();
        if (ptZoneDef)
        {
            if (ptZoneDef->uNavCellCount != 0)
            {
                bValid = TRUE;
            }
        }
    }

    return bValid;
}


BOOL NavZoneCollideSweptPoint(const NavCellHandle & hNavCellStart, const Vector2D & wpStart, const Vector2D & wvDirection, uint16 uOneRequiredFlag, uint16 uPreventFlags, uint32 uCollFlags, NavCellCollideResult & tResult)
{







    float flISectT = 1.0f;
    Vector2D wpDest = wpStart + wvDirection;
    Vector2D tWorldTestAABBMin, tWorldTestAABBMax;

    sInitializeResult(tResult, wpDest);


    AABBFromSweptPoint(wpStart, wpDest, tWorldTestAABBMin, tWorldTestAABBMax);

    const NavZoneInstance * ptZoneInstance = NavCellHandleToZoneInstance(hNavCellStart);
    // line table: 1526 tests only the inlined sZoneInstanceIsValid result; its NULL test (1489) covers ptZoneInstance
    if (!sZoneInstanceIsValid(ptZoneInstance))
    {
        sFinalizeResultBadStart(tResult, hNavCellStart, wpStart);
        return TRUE;
    }

    ScopedNavZoneDefLock tZoneDefLock(ptZoneInstance);
    const NavZoneDefinition * ptZoneDef = ptZoneInstance->GetZoneDef();


    if (!sValidatePointInCell(ptZoneInstance, wpStart, hNavCellStart))
    {
        tResult.flT = 0.0f;
        return FALSE;
    }


    Vector2D tLocalTestAABBMin = tWorldTestAABBMin - ptZoneInstance->wpWorldMin;
    Vector2D tLocalTestAABBMax = tWorldTestAABBMax - ptZoneInstance->wpWorldMin;

    {
        const NavCell * ptCurCell = NavCellHandleToNavCell(ptZoneDef, hNavCellStart);


        if ((!(ptCurCell->uFlags & uOneRequiredFlag) && !(uCollFlags & (1 << NAV_ZONE_COLL_IGNORE_REQUIRED_FLAG))) ||
            (ptCurCell->uFlags & uPreventFlags))
        {

            AABBCollideFeature eCollFeature;
            Vector2D wpColl;

            wpColl = wpStart;


            eCollFeature = AABBGetClosestFeature(wpColl, ptCurCell->tMin.As2D(), ptCurCell->tMax.As2D());

            sFinalizeResult(tResult, ptZoneInstance, wpStart, wvDirection, wpColl, 0.0f, hNavCellStart, eCollFeature, FALSE);
            return TRUE;
        }


        if (AABB2DInsideAABB2D(tLocalTestAABBMin, tLocalTestAABBMax, ptCurCell->tMin.As2D(), ptCurCell->tMax.As2D()))
        {
            tResult.hNavCellEnd = hNavCellStart;
            return FALSE;
        }
    }





    const int32 MAX_SCENES_INTERSECT = 64;
    SSceneID arSScenes[MAX_SCENES_INTERSECT];

    SWorldID idSWorld = ptZoneInstance->idSWorld;


    int32 nSceneCount = NavMeshStitchScenesInLineSegment(idSWorld, wpStart, wpDest, arSScenes, MAX_SCENES_INTERSECT);



    for (int i = 0; i < nSceneCount; i++)
    {
        const NavZoneInstance * ptCurZoneInst = NavZoneInstanceFromSSceneID(arSScenes[i]);

        NavZoneInstance * arChildInst[16];
        uint32 uChildCount = 0;
        if (ptCurZoneInst)
        {
            uChildCount = ptCurZoneInst->GetChildInstances(tWorldTestAABBMin, tWorldTestAABBMax, arChildInst);
        }

        if (uChildCount)
        {
            AABBCollideFeature eCollFeatureClosest = AABB_COLLIDE_NONE;
            float flClosestT = 1.0f;
            NavCellHandle hNavCellCollClosest;
            Vector2D wpCollClosest;
            uint32 uChildClosest = 0;  // DWARF: const_value 0, never reassigned
            for (uint32 j = 0; j < uChildCount; j++)
            {
                ScopedNavZoneDefLock tZoneLock(arChildInst[j]);
                float flT;
                NavCellHandle hNavCellColl;
                Vector2D wpColl;
                AABBCollideFeature eCollFeature = NavZoneCollideSweptPointOverGrids(
                    arChildInst[j], wpStart, wpDest, wvDirection, uOneRequiredFlag, uPreventFlags, uCollFlags, flT, hNavCellColl, wpColl);










                if (eCollFeature != AABB_COLLIDE_NONE && flT < flClosestT)
                {
                    flClosestT = flT;
                    eCollFeatureClosest = eCollFeature;
                    hNavCellCollClosest = hNavCellColl;
                    wpCollClosest = wpColl;
                    // inferred: uChildCount = j (not uChildClosest, which DWARF records as const 0): the original copies j into the loop-bound register w28 (mov w28, w23) before the latch compare
                    uChildCount = j;
                }
            }
            if (eCollFeatureClosest != AABB_COLLIDE_NONE)
            {
                sFinalizeResult(tResult,
                    // line table: row 1638 column 21 loads this argument
                    arChildInst[uChildClosest], wpStart, wvDirection, wpCollClosest, flClosestT, hNavCellCollClosest, eCollFeatureClosest, FALSE);
                return TRUE;
            }
        }







        else if (!sZoneInstanceIsValid(ptCurZoneInst))
        {








            const NavZoneInstance * ptLastZone = ptZoneInstance;
            if (i > 0)
            {
                ptLastZone = NavZoneInstanceFromSSceneID(arSScenes[i - 1]);
            }





            Vector2D wvOppDirection = -wvDirection;




            AABBCollideFeature eCollFeature;
            NavCellHandle hNavCellColl;
            Vector2D wpColl;

            eCollFeature = Ray2DAndAABB2D(wpDest, wvOppDirection, ptLastZone->wpWorldMin, ptLastZone->wpWorldMax, flISectT, wpColl);
            if (eCollFeature == AABB_COLLIDE_NONE)
            {
                break;
            }





            eCollFeature = sgarAABBCollFeatureOpposites[eCollFeature];



            flISectT = 1.0f - flISectT - 0.000001f;


            hNavCellColl.idSScene = SSCENEID_INVALID; hNavCellColl.uCellIndex = (uint32)-1;

            sFinalizeResult(tResult, ptLastZone, wpStart, wvDirection, wpColl, flISectT, hNavCellColl, eCollFeature, TRUE);










            tResult.hNavCellCollision = tResult.hNavCellEnd;
            return TRUE;
        }

        // inferred: the previous scene zone is carried in x23 (x23 <- x28 at the latch) and feeds ptLastZone (1659) and sFinalizeResultNoCollision (1754); placed here, before the lock, no row of its own
        ptZoneInstance = ptCurZoneInst;

        ScopedNavZoneDefLock tCurZoneDefLock(ptCurZoneInst);

        float flT;
        AABBCollideFeature eCollFeature;
        NavCellHandle hNavCellColl;
        Vector2D wpColl;

        eCollFeature = NavZoneCollideSweptPointOverGrids(ptCurZoneInst, wpStart, wpDest, wvDirection, uOneRequiredFlag, uPreventFlags, uCollFlags, flT, hNavCellColl, wpColl);













        if (eCollFeature != AABB_COLLIDE_NONE)
        {
            flISectT = flT;

            sFinalizeResult(tResult, ptCurZoneInst, wpStart, wvDirection, wpColl, flISectT, hNavCellColl, eCollFeature, FALSE);
            return TRUE;
        }









    }

    sFinalizeResultNoCollision(tResult, hNavCellStart, ptZoneInstance);
    return FALSE;
}
// NavigationMesh.cpp:2202, original DWARF declaration.
NavMeshSquare NavMeshIndexToSquare(const NavMeshDef* ptNavMeshDef, int32 nSquareIndex, BOOL fSpecialOnly);
static float sGetSquareHeight(const NavMeshDef& tNavMeshData, int32 i, int32 j)



{
    i = Math::Pin(i, 0, tNavMeshData.nSizeX - 1);
    j = Math::Pin(j, 0, tNavMeshData.nSizeY - 1);

    int32 nOffset = (j * tNavMeshData.nSizeX) + i;
    const NavMeshSquare& tSquare = NavMeshIndexToSquare(&tNavMeshData, nOffset, FALSE);
    return tSquare.wdSquareZ;
}



static BOOL sCheckPlaneCollision(const Vector3D& vStartLine, const Vector3D& vEndLine, const Vector3D& v0, const Vector3D& v1, const Vector3D& v2, float& flTIntersect, Vector3D& vCollPoint)







{
    flTIntersect = 1.0f;

    Vector3D vPlaneNormal = Vector3D::CrossProduct(v1 - v0, v2 - v0);
    if (vPlaneNormal.MagnitudeSquared() < 1e-12f)
    {
        return FALSE;
    }

    vPlaneNormal.Normalize();


    float flProjStart = Vector3D::DotProduct(vPlaneNormal, vStartLine - v0);
    float flProjEnd = Vector3D::DotProduct(vPlaneNormal, vEndLine - v0);



    if ((flProjStart <= 0.0f && flProjEnd >= 0.0f) ||
        (flProjStart >= 0.0f && flProjEnd <= 0.0f))
    {

        float flProjTotal = flProjEnd - flProjStart;
        if (Math::Abs(flProjTotal) > 1e-6f)
        {
            flTIntersect = -flProjStart / flProjTotal;
        }
        else
        {
            flTIntersect = 0.0f;
        }




        Vector3D vIntersect = vEndLine - vStartLine;
        vIntersect *= flTIntersect;
        vIntersect += vStartLine;

        vCollPoint = vIntersect;

        return TRUE;
    }

    return FALSE;





}


static BOOL sSweptPoint3DAndNavMesh(const Vector3D& wpStart, const Vector3D& wpEnd, const NavZoneInstance* ptNavZoneInst, uint16 uOneRequiredFlags, uint16 uPreventFlags, BOOL bIgnoreFlags, Vector3D& wpCollision)







{
    BOOL bCollision = FALSE;

    Vector3D wvDir = wpEnd - wpStart;
    const SceneDefinition* ptSceneDef = ptNavZoneInst->GetSceneDef();

    const NavMeshDef* ptNavMeshData = &ptSceneDef->tNavMeshData;





    int i = Math::FloatFloorToInt(wpStart.x / cflGridCellWidth);
    int j = Math::FloatFloorToInt(wpStart.y / cflGridCellWidth);


    int iend = Math::FloatFloorToInt(wpEnd.x / cflGridCellWidth);
    int jend = Math::FloatFloorToInt(wpEnd.y / cflGridCellWidth);


    int di = ((wpStart.x < wpEnd.x) ? 1 : ((wpStart.x > wpEnd.x) ? -1 : 0));
    int dj = ((wpStart.y < wpEnd.y) ? 1 : ((wpStart.y > wpEnd.y) ? -1 : 0));

    float adx = Math::Abs(wvDir.x);
    float ady = Math::Abs(wvDir.y);





    float minx = cflGridCellWidth * floorf(wpStart.x / cflGridCellWidth);
    float maxx = minx + cflGridCellWidth;
    float tx = 1.0f;
    if (adx > EPSILON)
    {
        tx = ((wpStart.x > wpEnd.x) ? (wpStart.x - minx) : (maxx - wpStart.x)) / adx;
    }

    float miny = cflGridCellWidth * floorf(wpStart.y / cflGridCellWidth);
    float maxy = miny + cflGridCellWidth;

    float ty = 1.0f;
    if (ady > EPSILON)
    {
        ty = ((wpStart.y > wpEnd.y) ? (wpStart.y - miny) : (maxy - wpStart.y)) / ady;
    }




    float deltatx = (adx > EPSILON) ? cflGridCellWidth / adx : 0.0f;
    float deltaty = (ady > EPSILON) ? cflGridCellWidth / ady : 0.0f;

    int nOffset = i + j * ptNavMeshData->nSizeX;

    float ptx = 0.0f;
    float pty = 0.0f;

    Vector3D v0(0.0f, 0.0f, 0.0f);
    Vector3D v1(cflGridCellWidth, 0.0f, 0.0f);
    Vector3D v2(0.0f, cflGridCellWidth, 0.0f);
    Vector3D v3(cflGridCellWidth, cflGridCellWidth, 0.0f);

    for (;;)
    {

        if (i >= 0 && i < ptNavMeshData->nSizeX && j >= 0 &&
                                                   j < ptNavMeshData->nSizeY)
        {


            const NavMeshSquare& tSquare = NavMeshIndexToSquare(ptNavMeshData, nOffset, FALSE);





            if (!(tSquare.dwSquareFlags & (1 << CELL_STATIC_SUBSCENE)))
            {
                if ((!(tSquare.dwSquareFlags & uOneRequiredFlags) ||
                     (tSquare.dwSquareFlags & uPreventFlags)) && !bIgnoreFlags)
                {


                    float flT = Math::Max(ptx, pty);
                    wpCollision = wpStart + wvDir * flT;
                    bCollision = TRUE;
                    break;
                }


                float flTimeStart = Math::Max(0.0f, Math::Max(ptx, pty));
                float flTimeEnd = Math::Min(1.0f, Math::Min(tx, ty));
                Vector3D vStartLine = wpStart + wvDir * flTimeStart;
                Vector3D vEndLine = wpStart + wvDir * flTimeEnd;








                v0.z = sGetSquareHeight(*ptNavMeshData, i, j);
                float flHighest = v0.z;
                v1.z = sGetSquareHeight(*ptNavMeshData, i + 1, j);
                flHighest = Math::Max(flHighest, v1.z);
                v2.z = sGetSquareHeight(*ptNavMeshData, i, j + 1);
                flHighest = Math::Max(flHighest, v2.z);
                v3.z = sGetSquareHeight(*ptNavMeshData, i + 1, j + 1);
                flHighest = Math::Max(flHighest, v3.z);


                if (vStartLine.z <= flHighest || vEndLine.z <= flHighest)
                {

                    Vector3D vSquarePos;
                    vSquarePos.x = i * cflGridCellWidth;
                    vSquarePos.y = j * cflGridCellWidth;
                    vSquarePos.z = 0.0f;
                    vStartLine = vStartLine - vSquarePos;
                    vEndLine = vEndLine - vSquarePos;

                    float flT0, flT1;
                    Vector3D vCollPoint0, vCollPoint1;

                    BOOL bColl0 = sCheckPlaneCollision(vStartLine, vEndLine, v0, v3, v2, flT0, vCollPoint0);
                    BOOL bColl1 = sCheckPlaneCollision(vStartLine, vEndLine, v0, v1, v3, flT1, vCollPoint1);
                    if (bColl0 && bColl1)
                    {

                        if (flT1 < flT0)
                        {
                            wpCollision = vCollPoint1;
                        }
                        else
                        {
                            wpCollision = vCollPoint0;
                        }
                        bCollision = TRUE;
                    }

                    else if (bColl0)
                    {
                        wpCollision = vCollPoint0;
                        bCollision = TRUE;
                    }
                    else if (bColl1)
                    {
                        wpCollision = vCollPoint1;
                        bCollision = TRUE;
                    }

                    if (bCollision)
                    {

                        wpCollision += vSquarePos;
                        break;
                    }
                }
            }
        }

        if (tx <= ty)
        {
            if (i == iend)
            {
                break;
            }
            ptx = tx;
            tx += deltatx;
            i += di;
            nOffset += di;
        }
        else
        {
            if (j == jend)
            {
                break;
            }
            pty = ty;
            ty += deltaty;
            j += dj;
            nOffset += dj * ptNavMeshData->nSizeX;
        }
    }

    return bCollision;
}


// NON_MATCHING: missing: how the original reaches the protected NavZoneInstance::ptSceneDef at 1846 (it loads +0x60 inline; GetSceneDef is declared but no CU has a body or inlined record, and the DWARF records no friend), so GetSceneDef is an invented inline getter in NavZone.h (out of line it was 2524 vs 2556 bytes). With it this is 3/639 insns off: the 1919 CELL_STATIC_SUBSCENE test is lsr + tbnz w8,#5 vs the original tbnz x0,#37.
static BOOL sSweptPoint3DAndNavZone(
    const NavZoneInstance* ptZoneInstance,
    const Vector3D& wpStart,
    const Vector3D& wvDirection,
    const Vector3D& wpEnd,
    Vector3D& wpColl,
    uint16 uOneRequiredFlags,
    uint16 uPreventFlags,
    BOOL bIgnoreCollFlags)
{
    BOOL bCollision = FALSE;
    const NavZoneDefinition* ptZoneDef = ptZoneInstance->GetZoneDef();



    Vector3D wpStartLocal = wpStart - ptZoneInstance->wpWorldMin3d;

    Vector3D wpStartGrid(wpStartLocal);
    Vector3D wpEndGrid = wpEnd - ptZoneInstance->wpWorldMin3d;

    Vector2D wpZoneMin(0.0f, 0.0f);
    Vector2D wpZoneMax(ptZoneDef->flGridSizeFeet * ptZoneDef->tGridSize.x, ptZoneDef->flGridSizeFeet * ptZoneDef->tGridSize.y);

    Vector3D wvOppDirection = -wvDirection;

    if (!Point2DInsideAABB2D(wpStartGrid.As2D(), wpZoneMin, wpZoneMax))
    {
        Vector2D wpISect;
        float flT;
        AABBCollideFeature eCollFeature = Ray2DAndAABB2D(wpStartGrid.As2D(), wvDirection.As2D(), wpZoneMin, wpZoneMax, flT, wpISect);


        if (eCollFeature != AABB_COLLIDE_NONE)
        {
            wpStartGrid = wpStartGrid + wvDirection * flT;
        }
    }

    if (!Point2DInsideAABB2D(wpEndGrid.As2D(), wpZoneMin, wpZoneMax))
    {
        Vector2D wpISect;
        float flT;
        AABBCollideFeature eCollFeature = Ray2DAndAABB2D(wpEndGrid.As2D(), wvOppDirection.As2D(), wpZoneMin, wpZoneMax, flT, wpISect);


        if (eCollFeature != AABB_COLLIDE_NONE)
        {
            wpEndGrid = wpEndGrid + wvOppDirection * flT;
        }
    }

    Vector3D wpCollClosest;
    float flClosestDistSqr = FLT_MAX;


    bCollision = sSweptPoint3DAndNavMesh(wpStartGrid, wpEndGrid, ptZoneInstance, uOneRequiredFlags, uPreventFlags, bIgnoreCollFlags, wpCollClosest);

    if (bCollision)
    {
        wpCollClosest += ptZoneInstance->wpWorldMin3d;
        flClosestDistSqr = Vector3D::DistanceSquared(wpStart, wpCollClosest);
    }



    Vector2D tWorldTestAABBMin, tWorldTestAABBMax;

    if (wpStartGrid.x < wpEndGrid.x)
    {
        tWorldTestAABBMin.x = wpStartGrid.x;
        tWorldTestAABBMax.x = wpEndGrid.x;
    }
    else
    {
        tWorldTestAABBMin.x = wpEndGrid.x;
        tWorldTestAABBMax.x = wpStartGrid.x;
    }

    if (wpStartGrid.y < wpEndGrid.y)
    {
        tWorldTestAABBMin.y = wpStartGrid.y;
        tWorldTestAABBMax.y = wpEndGrid.y;
    }
    else
    {
        tWorldTestAABBMin.y = wpEndGrid.y;
        tWorldTestAABBMax.y = wpStartGrid.y;
    }

    tWorldTestAABBMin += ptZoneInstance->wpWorldMin;
    tWorldTestAABBMax += ptZoneInstance->wpWorldMin;

    NavZoneInstance* arChildInst[16];

    uint32 uChildCount = ptZoneInstance->GetChildInstances(tWorldTestAABBMin, tWorldTestAABBMax, arChildInst);

    for (uint32 i = 0; i < uChildCount; ++i)
    {
        NavZoneInstance* ptChildInst = arChildInst[i];
        Vector3D wpChildColl;

        ScopedNavZoneDefLock tChildZoneDefLock(ptChildInst);



        float flT;
        AABBCollideFeature eCollFeature;

        eCollFeature = Ray2DAndAABB2D(wpStart.As2D(), wvDirection.As2D(), ptChildInst->wpWorldMin, ptChildInst->wpWorldMax, flT, wpColl.As2D());

        if (eCollFeature == AABB_COLLIDE_NONE)
        {
            continue;
        }

        if (sSweptPoint3DAndNavZone(ptChildInst, wpStart, wvDirection, wpEnd, wpChildColl, uOneRequiredFlags, uPreventFlags, bIgnoreCollFlags))
        {
            float flDistSqr = Vector3D::DistanceSquared(wpStart, wpChildColl);

            if (flDistSqr < flClosestDistSqr)
            {
                flClosestDistSqr = flDistSqr;
                wpCollClosest = wpChildColl;
            }
        }
    }

    if (flClosestDistSqr < FLT_MAX)
    {
        wpColl = wpCollClosest;
        return TRUE;
    }

    return FALSE;
}




BOOL NavZoneCollideSweptPoint3D(SWorldID idSWorld, const Vector3D& wpStart, const Vector3D& wvDirection, uint16 uOneRequiredFlag, uint16 uPreventFlags, uint32 uCollFlags, float& flISectT, Vector3D& wpCollision)








{


    Vector3D wvDir;
    Vector3D wpDest;

    wvDir = wvDirection;
    wpDest = wpStart + wvDir;

    flISectT = 1.0f;
    wpCollision = wpDest;

    BOOL bIgnoreCollFlags = (uCollFlags & (1 << NAV_ZONE_COLL_IGNORE_FLAGS)) != 0;
    BOOL bIgnoreInvalidScenes = (uCollFlags & (1 << NAV_ZONE_COLL_IGNORE_INVALID_SCENES)) != 0;
    float flClosestDistSqr = FLT_MAX;

    const int32 MAX_SCENES_INTERSECT = 64;
    SSceneID arSScenes[MAX_SCENES_INTERSECT];


    int nSceneCount = NavMeshStitchScenesInLineSegment(idSWorld, wpStart.As2D(), wpDest.As2D(), arSScenes, MAX_SCENES_INTERSECT);



    float flDirMag2D = wvDirection.As2D().Magnitude();

    for (int i = 0; i < nSceneCount; ++i)
    {
        const NavZoneInstance* ptCurZoneInst = NavZoneInstanceFromSSceneID(arSScenes[i]);

        if (!ptCurZoneInst)
        {
            if (bIgnoreInvalidScenes)
            {
                continue;
            }









            const NavZoneInstance* ptLastZone = NavZoneInstanceFromSSceneID(arSScenes[i - 1]);

            Vector2D wvOppDirection = -wvDir.As2D();




            Vector2D wpColl;
            AABBCollideFeature eCollFeature;

            eCollFeature = Ray2DAndAABB2D(wpDest.As2D(), wvOppDirection, ptLastZone->wpWorldMin, ptLastZone->wpWorldMax, flISectT, wpColl);









            float flT = 0.0f;
            if (flDirMag2D > 1e-6f)
            {
                flT = Vector2D::Distance(wpStart.As2D(), wpColl) / flDirMag2D;
            }
            Vector3D wpColl3d = wpStart + wvDirection * flT;
            float flDistSqr = Vector3D::DistanceSquared(wpStart, wpColl3d);



            wpColl3d.x = Math::Pin(wpColl3d.x, ptLastZone->wpWorldMin.x, ptLastZone->wpWorldMax.x);
            wpColl3d.y = Math::Pin(wpColl3d.y, ptLastZone->wpWorldMin.y, ptLastZone->wpWorldMax.y);

            if (flDistSqr < flClosestDistSqr)
            {
                flClosestDistSqr = flDistSqr;
                wpCollision = wpColl3d;
            }

            break;
        }

        ScopedNavZoneDefLock tCurZoneDefLock(ptCurZoneInst);
        Vector3D wpColl;

        BOOL bCollision = sSweptPoint3DAndNavZone(ptCurZoneInst, wpStart, wvDir, wpDest, wpColl, uOneRequiredFlag, uPreventFlags, bIgnoreCollFlags);



        if (bCollision)
        {
            float flDistSqr = Vector3D::DistanceSquared(wpStart, wpColl);

            if (flDistSqr < flClosestDistSqr)
            {
                flClosestDistSqr = flDistSqr;
                wpCollision = wpColl;
            }

            break;
        }
    }

    if (flClosestDistSqr < FLT_MAX)
    {
        float flDirMag = wvDirection.Magnitude();
        if (flDirMag > 1e-6f)
        {
            float flDist = Math::Sqrt(flClosestDistSqr);
            flISectT = flDist / flDirMag;
        }
        else
        {
            flISectT = 0.0f;
        }
        return TRUE;
    }


    return FALSE;
}


static BOOL sValidatePointInCell(const NavZoneInstance * ptZoneInstance, const Vector2D & wp, const NavCellHandle & hNavCell)



{
    const NavCell * ptNavCell = NavCellHandleToNavCell(ptZoneInstance->GetZoneDef(), hNavCell);
    Vector2D wpLocal = wp - ptZoneInstance->wpWorldMin;

    if (ptNavCell && Point2DInsideAABB2D(wpLocal, ptNavCell->tMin.As2D(), ptNavCell->tMax.As2D()))
    {
        return TRUE;
    }
    CRefString sScene;
    sScene = SNOToString(SCENE_SNO, ptZoneInstance->snoScene, FALSE);











    WorldPlace tPlace;
    tPlace.idSWorld = ptZoneInstance->idSWorld;
    tPlace.wp.Set(wp.x, wp.y, 0.0f);
    NavCellHandle hNavCellActual;
    NavCellHandleFromWorldPlace(tPlace, hNavCellActual);

    if (hNavCellActual.idSScene != SSCENEID_INVALID)
    {
        const NavZoneInstance * ptZoneInstanceLoc = NavCellHandleToZoneInstance(hNavCellActual);

        if (ptZoneInstanceLoc)
        {
            ScopedNavZoneDefLock tZoneDefLock(ptZoneInstanceLoc);
            const NavZoneDefinition * ptZoneDef = ptZoneInstanceLoc->GetZoneDef();

            const NavCell * ptNavCellActual = NavCellHandleToNavCell(ptZoneDef, hNavCellActual);


            sScene = SNOToString(SCENE_SNO, ptZoneInstanceLoc->snoScene, FALSE);










        }
    }










    return FALSE;


}


static void sInitializeResult(NavCellCollideResult & tResult, const Vector2D & wpDest)
{


    tResult.bCollided = FALSE;
    tResult.wpEnd = wpDest;
    tResult.hNavCellEnd.idSScene = SSCENEID_INVALID; tResult.hNavCellEnd.uCellIndex = (uint32)-1;

    tResult.hNavCellCollision.idSScene = SSCENEID_INVALID; tResult.hNavCellCollision.uCellIndex = (uint32)-1; tResult.wvNormal.x = 0.0f; tResult.wvNormal.y = 0.0f;

    tResult.flT = 1.0f;
    tResult.wpCollision.Set(0.0f, 0.0f);


}

static void sFinalizeResultNoCollision(NavCellCollideResult & tResult, const NavCellHandle & hNavCellStart, const NavZoneInstance * ptZoneInstance)



{
    if (Point2DInsideAABB2D(tResult.wpEnd, ptZoneInstance->wpWorldMin, ptZoneInstance->wpWorldMax))
    {
        ScopedNavZoneDefLock tZoneLock(ptZoneInstance);
        NavCellHandleFromPoint(ptZoneInstance, tResult.wpEnd, tResult.hNavCellEnd);

        if (tResult.hNavCellEnd.idSScene == SSCENEID_INVALID)
        {

            tResult.hNavCellEnd = hNavCellStart;
        }




    }
    else
    {

        WorldPlace tPlace;
        tPlace.idSWorld = ptZoneInstance->idSWorld;
        tPlace.wp.Set(tResult.wpEnd.x, tResult.wpEnd.y, 0.0f);

        NavCellHandleFromWorldPlace(tPlace, tResult.hNavCellEnd);

        if (tResult.hNavCellEnd.idSScene == SSCENEID_INVALID)
        {

            const float BORDER_BUFFER_DIST = 0.001f;

            tResult.wpEnd.x = Math::Pin(tResult.wpEnd.x, ptZoneInstance->wpWorldMin.x + BORDER_BUFFER_DIST, ptZoneInstance->wpWorldMax.x - BORDER_BUFFER_DIST);
            tResult.wpEnd.y = Math::Pin(tResult.wpEnd.y, ptZoneInstance->wpWorldMin.y + BORDER_BUFFER_DIST, ptZoneInstance->wpWorldMax.y - BORDER_BUFFER_DIST);
            tPlace.wp.x = tResult.wpEnd.x; tPlace.wp.y = tResult.wpEnd.y;

            NavCellHandleFromWorldPlace(tPlace, tResult.hNavCellEnd);
        }









    }
}


static void sGetCollNormal(AABBCollideFeature eCollFeature, const Vector2D & wpEnd, const Vector2D & wpCollision, Vector2D & wvNormal)
{




    if (eCollFeature == AABB_COLLIDE_VERTEX)
    {
        wvNormal = wpEnd - wpCollision;
        wvNormal.Normalize();
    }
    else
    {
        wvNormal = AABBGetNormal(eCollFeature);
    }
}



static void sFinalizeResult(NavCellCollideResult & tResult, const NavZoneInstance * ptZoneInstance, const Vector2D & wpStart, const Vector2D & wvDirection, const Vector2D & wpColl, float flISectT, const NavCellHandle & hNavCellColl, AABBCollideFeature eCollFeature, BOOL bUseCollAsEnd)
{











    tResult.bCollided = TRUE;
    tResult.flT = flISectT;
    if (bUseCollAsEnd)
    {
        tResult.wpEnd = wpColl;
    }
    else
    {
        tResult.wpEnd = wvDirection * flISectT + wpStart;
    }
    tResult.wpCollision = wpColl;
    tResult.hNavCellCollision = hNavCellColl;

    if (Point2DInsideAABB2D(tResult.wpEnd, ptZoneInstance->wpWorldMin, ptZoneInstance->wpWorldMax))
    {
        ScopedNavZoneDefLock tZoneLock(ptZoneInstance);
        NavCellHandleFromPoint(ptZoneInstance, tResult.wpEnd, tResult.hNavCellEnd);
    }
    else
    {

        WorldPlace tPlace;
        tPlace.idSWorld = ptZoneInstance->idSWorld;

        tPlace.wp.Set(tResult.wpEnd.x, tResult.wpEnd.y, 0.0f);

        NavCellHandleFromWorldPlace(tPlace, tResult.hNavCellEnd);

        if (tResult.hNavCellEnd.idSScene == SSCENEID_INVALID)
        {
            const float BORDER_BUFFER_DIST = 0.001f;


            tResult.wpEnd.x = Math::Pin(tResult.wpEnd.x, ptZoneInstance->wpWorldMin.x + BORDER_BUFFER_DIST, ptZoneInstance->wpWorldMax.x - BORDER_BUFFER_DIST);
            tResult.wpEnd.y = Math::Pin(tResult.wpEnd.y, ptZoneInstance->wpWorldMin.y + BORDER_BUFFER_DIST, ptZoneInstance->wpWorldMax.y - BORDER_BUFFER_DIST);
            tPlace.wp.x = tResult.wpEnd.x; tPlace.wp.y = tResult.wpEnd.y;

            NavCellHandleFromWorldPlace(tPlace, tResult.hNavCellEnd);


            if (tResult.hNavCellEnd.idSScene == SSCENEID_INVALID)
            {







                tResult.flT = 0.0f;
                tResult.wpCollision = wpStart;
                tResult.wpEnd = wpStart;
            }
        }
    }

    sGetCollNormal(eCollFeature, tResult.wpEnd, tResult.wpCollision, tResult.wvNormal);
}


static void sFinalizeResultBadStart(NavCellCollideResult & tResult, const NavCellHandle & hNavCellStart, const Vector2D & wpStart)
{



    tResult.bCollided = TRUE;
    tResult.flT = 0.0f;
    tResult.wpEnd = wpStart;
    tResult.hNavCellEnd = hNavCellStart;
    tResult.hNavCellCollision = hNavCellStart;
    tResult.wpCollision = wpStart;
    tResult.wvNormal.Set(1.0f, 0.0f);
}
