#include "NavZoneIntersectPrim.h"
#include "NavigationMesh.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Sphere.h"

#include <float.h>

static Vector2D sgarvAABBCollideOffsets[5] = {
    Vector2D(0.0f, 0.0f),
    Vector2D(1.0f, 0.0f),
    Vector2D(-1.0f, 0.0f),
    Vector2D(0.0f, 1.0f),
    Vector2D(0.0f, -1.0f),
};

static Vector2D sgvCrnDeltaA[4] = {
    Vector2D(0.0f, cflGridCellWidth),
    Vector2D(0.0f, cflGridCellWidth),
    Vector2D(0.0f, -cflGridCellWidth),
    Vector2D(0.0f, -cflGridCellWidth),
};

static Vector2D sgvCrnDeltaB[4] = {
    Vector2D(cflGridCellWidth, 0.0f),
    Vector2D(-cflGridCellWidth, 0.0f),
    Vector2D(cflGridCellWidth, 0.0f),
    Vector2D(-cflGridCellWidth, 0.0f),
};

static const float cflOOSqrt2 = 0.70710678f;

static Vector2D sgvCrnNormal[4] = {
    Vector2D(-cflOOSqrt2, -cflOOSqrt2),
    Vector2D(cflOOSqrt2, -cflOOSqrt2),
    Vector2D(-cflOOSqrt2, cflOOSqrt2),
    Vector2D(cflOOSqrt2, cflOOSqrt2),
};

static Vector2D sgvCollideFeatureNormal[10] = {
    Vector2D(0.0f, 0.0f),
    Vector2D(-1.0f, 0.0f),
    Vector2D(1.0f, 0.0f),
    Vector2D(0.0f, -1.0f),
    Vector2D(0.0f, 1.0f),
    Vector2D(-cflOOSqrt2, -cflOOSqrt2),
    Vector2D(cflOOSqrt2, -cflOOSqrt2),
    Vector2D(-cflOOSqrt2, cflOOSqrt2),
    Vector2D(cflOOSqrt2, cflOOSqrt2),
    Vector2D(0.0f, 0.0f),
};

const Vector2D & AABBGetNormal(AABBCollideFeature eCollFeature) {
    return sgvCollideFeatureNormal[eCollFeature];
}

static BOOL sSlabIntersect(float flP, float flD, float flMin, float flMax, float & flTMin, float & flTMax, AABBCollideFeature & eCollFeature, AABBCollideFeature eCollFeatureBase) {
    if (Math::Abs(flD) < EPSILON) {
        // Ray is parallel to the slab. No hit if origin not within slab
        if (flP < flMin || flP > flMax)
            return FALSE;
    } else {
        // Compute intersection t value of ray with near and far plane of slab
        float ood = 1.0f / flD;
        float t1 = (flMin - flP) * ood;
        float t2 = (flMax - flP) * ood;
        int nCollFeatureOffset = 0;
        // Make t1 be intersection with near plane, t2 with far plane
        if (t1 > t2) {
            float tmp = t1;
            t1 = t2;
            t2 = tmp;
            nCollFeatureOffset = 1;
        }
        // Compute the intersection of slab intersection intervals
        if (t1 > flTMin) {
            flTMin = t1;
            eCollFeature = (AABBCollideFeature)(eCollFeatureBase + nCollFeatureOffset);
        }
        if (t2 < flTMax)
            flTMax = t2;
        // Exit with no collision as soon as slab intersection becomes empty
        if (flTMin > flTMax)
            return FALSE;
    }
    return TRUE;
}

AABBCollideFeature AABBGetClosestFeature(Vector2D & wpColl, const Vector2D & wpAABBMin, const Vector2D & wpAABBMax) {
    float dxmin = wpColl.x - wpAABBMin.x;
    float dxmax = wpAABBMax.x - wpColl.x;
    float dymin = wpColl.y - wpAABBMin.y;
    float dymax = wpAABBMax.y - wpColl.y;
    AABBCollideFeature eCollFeature;

    if (Math::Min(dxmin, dxmax) < Math::Min(dymin, dymax)) {
        if (dxmin < dxmax) {
            eCollFeature = AABB_COLLIDE_XMIN;
            wpColl.x = wpAABBMin.x;
        } else {
            eCollFeature = AABB_COLLIDE_XMAX;
            wpColl.x = wpAABBMax.x;
        }
    } else {
        if (dymin < dymax) {
            eCollFeature = AABB_COLLIDE_YMIN;
            wpColl.y = wpAABBMin.y;
        } else {
            eCollFeature = AABB_COLLIDE_YMAX;
            wpColl.y = wpAABBMax.y;
        }
    }
    return eCollFeature;
}

AABBCollideFeature Ray2DAndAABB2D(const Vector2D wpStart, const Vector2D wvDir, const Vector2D wpAABBMin, const Vector2D wpAABBMax, float & flT, Vector2D & wpIsect) {
    AABBCollideFeature eCollFeature = AABB_COLLIDE_NONE;
    flT = 0.0f;
    float tmax = FLT_MAX;

    if (!sSlabIntersect(wpStart.x, wvDir.x, wpAABBMin.x, wpAABBMax.x, flT, tmax, eCollFeature, AABB_COLLIDE_XMIN))
        return AABB_COLLIDE_NONE;
    if (!sSlabIntersect(wpStart.y, wvDir.y, wpAABBMin.y, wpAABBMax.y, flT, tmax, eCollFeature, AABB_COLLIDE_YMIN))
        return AABB_COLLIDE_NONE;

    if (eCollFeature == AABB_COLLIDE_NONE) {
        // Ray starts inside the AABB
        wpIsect = wpStart;
        eCollFeature = AABBGetClosestFeature(wpIsect, wpAABBMin, wpAABBMax);
    } else {
        wpIsect = wpStart + wvDir * flT;
        wpIsect.x = Math::Pin(wpIsect.x, wpAABBMin.x, wpAABBMax.x);
        wpIsect.y = Math::Pin(wpIsect.y, wpAABBMin.y, wpAABBMax.y);
    }
    return eCollFeature;
}























































































BOOL Ray2DAndCircle(
    const Vector2D & wpStart,
    const Vector2D & wvDir,
    const Circle & wc,
    float & flT)
{
    Vector2D m = wpStart - wc.wpCenter;
    float b = Vector2D::DotProduct(m, wvDir);
    float c = Vector2D::DotProduct(m, m) - wc.wdRadius * wc.wdRadius;

    // Exit if the ray's origin is outside the circle and the ray points away from it
    if ( c > 0.0f && b > 0.0f )
        return FALSE;



    float discr = b * b - c;

    // A negative discriminant means the ray misses the circle
    if ( discr < 0.0f )
        return FALSE;




    flT = -b - Math::Sqrt(discr);

    // If t is negative the ray started inside the circle, so clamp t to zero
    if ( flT < 0.0f )
        flT = 0.0f;


    return TRUE;

}


static void sGetCorner(
    Vector2D & wpCorner,
    const Vector2D & wpMin,
    const Vector2D & wpMax,
    int n)
{
    wpCorner.x = (n & 1) ? wpMax.x : wpMin.x;
    wpCorner.y = (n & 2) ? wpMax.y : wpMin.y;
}


AABBCollideFeature SweptCircleAndAABB2D(
    const Circle & wc,
    const Vector2D & wvDir,
    const float flDirMag,
    const Vector2D & wvDirNormalized,
    const Vector2D & wpMin,
    const Vector2D & wpMax,
    float & flT,
    Vector2D & wpCollision)
{
    AABBCollideFeature eCollFeature;

    // Expand the AABB by the circle radius and intersect the ray against it
    Vector2D wpMinE, wpMaxE;

    wpMinE.x = wpMin.x - wc.wdRadius;
    wpMinE.y = wpMin.y - wc.wdRadius;

    wpMaxE.x = wpMax.x + wc.wdRadius;
    wpMaxE.y = wpMax.y + wc.wdRadius;

    Vector2D wpCollAABB;
    eCollFeature = Ray2DAndAABB2D(wc.wpCenter, wvDir, wpMinE, wpMaxE, flT, wpCollAABB);
    if ( eCollFeature == AABB_COLLIDE_NONE || flT > 1.0f )
        eCollFeature = AABB_COLLIDE_NONE;
    else
    {
        // Find which Voronoi region of the original AABB contains the intersection.
        int32 u = 0, v = 0;

        if ( wpCollAABB.x < wpMin.x )
        {
            u |= 1;
        }
        else if ( wpCollAABB.x > wpMax.x )
        {
            v |= 1;
        }
        if ( wpCollAABB.y < wpMin.y )
        {
            u |= 2;
        }
        else if ( wpCollAABB.y > wpMax.y )
        {
            v |= 2;
        }

        int m = v | u;

        // Vertex region: intersect against the corner circle
        if ( m == 3 )
        {
            Circle wcVertex;

            wcVertex.wdRadius = wc.wdRadius;
            sGetCorner(wcVertex.wpCenter, wpMin, wpMax, v);

            float flCircleT;
            // The original rejects only distances greater than flDirMag (0x662e28).
            if ( Ray2DAndCircle(wc.wpCenter, wvDirNormalized, wcVertex, flCircleT) && !(flCircleT > flDirMag) )
            {




                flCircleT = (flDirMag > EPSILON) ? flCircleT / flDirMag : 0.0f;


                wpCollision = wcVertex.wpCenter;
                flT = flCircleT;
                eCollFeature = AABB_COLLIDE_VERTEX;
            }
            else
                eCollFeature = AABB_COLLIDE_NONE;
        }
        else {
            Vector2D wvOffset = sgarvAABBCollideOffsets[eCollFeature];

            wvOffset *= wc.wdRadius;

            wpCollision = wpCollAABB + wvOffset;
        }
    }
    return eCollFeature;

}


AABBCollideFeature CircleAndAABB2D(const Circle & wc, const Vector2D & wpMin, const Vector2D & wpMax, float & flSqrDistance, Vector2D & wpCollision) {
    float flRadiusSqr;
    Vector2D wvDiff;
    AABBCollideFeature eCollFeature;

    wpCollision.x = Math::Pin(wc.wpCenter.x, wpMin.x, wpMax.x);
    wpCollision.y = Math::Pin(wc.wpCenter.y, wpMin.y, wpMax.y);

    wvDiff = wpCollision - wc.wpCenter;

    flRadiusSqr = wc.wdRadius * wc.wdRadius;
    flSqrDistance = wvDiff.MagnitudeSquared();
    if (flSqrDistance > flRadiusSqr)
        return AABB_COLLIDE_NONE;

    eCollFeature = AABBGetClosestFeature(wpCollision, wpMin, wpMax);
    return eCollFeature;
}

// NON_MATCHING: 1460 = 1460, 49/365 differ: only the order of the four compares in each unrolled
// rounded-corner box test (original y<=max, y>=min, x>=min, x<=max; ours y>=min, x>=min, x<=max, y<=max).
// Tried: locals in DWARF order (nCorner after the cell width, wpRndMin/Max declared then assigned).
AABBCollideFeature SweptCircleAndRoundedAABB2D(const Circle & wc, const Vector2D & wvDir, const float flDirMag, const Vector2D & wvDirNormalized, const Vector2D & wpMin, const Vector2D & wpMax, uint16 uFlags, float & flT, Vector2D & wpCollision) {
    Vector2D wpCollAABB;
    AABBCollideFeature eCollFeature;
    int32 u = 0;
    int32 v = 0;
    int m;
    Vector2D wpMinE;
    Vector2D wpMaxE;

    // Expand the AABB by the circle radius and intersect the ray against it
    wpMinE.x = wpMin.x - wc.wdRadius;
    wpMinE.y = wpMin.y - wc.wdRadius;
    wpMaxE.x = wpMax.x + wc.wdRadius;
    wpMaxE.y = wpMax.y + wc.wdRadius;
    eCollFeature = Ray2DAndAABB2D(wc.wpCenter, wvDir, wpMinE, wpMaxE, flT, wpCollAABB);
    if (eCollFeature == AABB_COLLIDE_NONE || flT > 1.0f)
        return AABB_COLLIDE_NONE;

    // Find which voronoi region of the original AABB the intersection is in
    if (wpCollAABB.x < wpMin.x)
        u |= 1;
    else if (wpCollAABB.x > wpMax.x)
        v |= 1;
    if (wpCollAABB.y < wpMin.y)
        u |= 2;
    else if (wpCollAABB.y > wpMax.y)
        v |= 2;
    m = u | v;

    if (m == 3 && !(uFlags & (1 << (CELL_STATIC_ROUNDED_CORNER_0 + v)))) {
        // Vertex region of a square corner: intersect against the corner circle
        float flCircleT;
        Circle wcVertex;
        sGetCorner(wcVertex.wpCenter, wpMin, wpMax, v);
        wcVertex.wdRadius = wc.wdRadius;
        if (!Ray2DAndCircle(wc.wpCenter, wvDirNormalized, wcVertex, flCircleT))
            return AABB_COLLIDE_NONE;
        if (flCircleT > flDirMag)
            return AABB_COLLIDE_NONE;
        flCircleT = flDirMag > EPSILON ? flCircleT / flDirMag : 0.0f;
        wpCollision = wcVertex.wpCenter;
        flT = flCircleT;
        return AABB_COLLIDE_VERTEX;
    }

    {
        int nCorner = -1;
        Vector2D wpDiagonalA;
        Vector2D wpDiagonalB;
        Vector2D wvRadius(wc.wdRadius, wc.wdRadius);
        Vector2D wvGridCellWidth(cflGridCellWidth, cflGridCellWidth);

        // Find a rounded corner whose region the expanded hit lies in
        for (int i = 0; i < 4; i++) {
            if (uFlags & (1 << (CELL_STATIC_ROUNDED_CORNER_0 + i))) {
                Vector2D wpCorner((i & 1) ? wpMax.x : wpMin.x, (i & 2) ? wpMax.y : wpMin.y);
                Vector2D wvToMin((i & 1) ? wvGridCellWidth.x : wvRadius.x, (i & 2) ? wvGridCellWidth.y : wvRadius.y);
                Vector2D wvToMax((i & 1) ? wvRadius.x : wvGridCellWidth.x, (i & 2) ? wvRadius.y : wvGridCellWidth.y);
                Vector2D wpRndMin = wpCorner - wvToMin;
                Vector2D wpRndMax = wpCorner + wvToMax;

                if (Point2DInsideAABB2D(wpCollAABB, wpRndMin, wpRndMax)) {
                    nCorner = i;
                    wpDiagonalA = wpCorner + sgvCrnDeltaA[i];
                    wpDiagonalB = wpCorner + sgvCrnDeltaB[i];
                    break;
                }
            }
        }

        if (nCorner != -1) {
            // Intersect against the diagonal that cuts off the corner
            Vector2D wvNrm = sgvCrnNormal[nCorner];
            float d = Vector2D::DotProduct(wvNrm, wpDiagonalA);
            float flPlaneRadius;
            float dist = Vector2D::DotProduct(wvNrm, wc.wpCenter) - d;
            float flCircleT;
            Vector2D wpClosestCircle;

            if (Math::Abs(dist) <= wc.wdRadius) {
                flCircleT = 0.0f;
                wpClosestCircle = wc.wpCenter;
                flPlaneRadius = dist > 0.0f ? wc.wdRadius : -wc.wdRadius;
            } else {
                float denom = Vector2D::DotProduct(wvNrm, wvDir);
                if (denom * dist >= 0.0f)
                    return AABB_COLLIDE_NONE;
                flPlaneRadius = dist > 0.0f ? wc.wdRadius : -wc.wdRadius;
                flCircleT = (flPlaneRadius - dist) / denom;
                if (flCircleT < 0.0f || flCircleT > 1.0f)
                    return AABB_COLLIDE_NONE;
                wpClosestCircle = wc.wpCenter + flCircleT * wvDir;
            }

            Vector2D wvDiag = wpDiagonalB - wpDiagonalA;
            Vector2D wpAC = wpClosestCircle - wpDiagonalA;
            float flPrj = Vector2D::DotProduct(wvDiag, wpAC);
            if (flPrj <= 0.0f) {
                // Closest to end A of the diagonal
                Circle wcDiagonal;
                wcDiagonal.wpCenter = wpDiagonalA;
                wcDiagonal.wdRadius = wc.wdRadius;
                if (!Ray2DAndCircle(wc.wpCenter, wvDirNormalized, wcDiagonal, flCircleT))
                    return AABB_COLLIDE_NONE;
                if (flCircleT > flDirMag)
                    return AABB_COLLIDE_NONE;
                flCircleT = flDirMag > EPSILON ? flCircleT / flDirMag : 0.0f;
                wpCollision = wcDiagonal.wpCenter;
                flT = flCircleT;
                return AABB_COLLIDE_VERTEX;
            } else {
                float denom2 = Vector2D::DotProduct(wvDiag, wvDiag);
                if (flPrj >= denom2) {
                    // Closest to end B of the diagonal
                    Circle wcDiagonal;
                    wcDiagonal.wpCenter = wpDiagonalB;
                    wcDiagonal.wdRadius = wc.wdRadius;
                    if (!Ray2DAndCircle(wc.wpCenter, wvDirNormalized, wcDiagonal, flCircleT))
                        return AABB_COLLIDE_NONE;
                    if (flCircleT > flDirMag)
                        return AABB_COLLIDE_NONE;
                    flCircleT = flDirMag > EPSILON ? flCircleT / flDirMag : 0.0f;
                    wpCollision = wcDiagonal.wpCenter;
                    flT = flCircleT;
                    return AABB_COLLIDE_VERTEX;
                }
                wpCollision = wpClosestCircle - wvNrm * flPlaneRadius;
                flT = flCircleT;
                return (AABBCollideFeature)(AABB_COLLIDE_RND0 + nCorner);
            }
        }

        {
            // Face region: the expanded AABB hit is the answer
            Vector2D wvOffset = sgarvAABBCollideOffsets[eCollFeature];
            wvOffset *= wc.wdRadius;
            wpCollision = wpCollAABB + wvOffset;
        }
    }
    return eCollFeature;
}

inline float Signed2DTriArea(const Vector2D & wpA, const Vector2D & wpB, const Vector2D & wpC) {
    return (wpA.x - wpC.x) * (wpB.y - wpC.y) - (wpA.y - wpC.y) * (wpB.x - wpC.x);
}

BOOL SweptPoint2DAndLine2D(const Vector2D & wpStart, const Vector2D & wpEnd, const Vector2D & wpLineA, const Vector2D & wpLineB, float & flT) {
    // Signs of areas correspond to which side of the line the segment ends are on
    float a1 = Signed2DTriArea(wpStart, wpEnd, wpLineB);
    float a2 = Signed2DTriArea(wpStart, wpEnd, wpLineA);
    if (a1 * a2 < 0.0f) {
        float a3 = Signed2DTriArea(wpLineA, wpLineB, wpStart);
        float a4 = a3 + a2 - a1;
        if (a3 * a4 < 0.0f) {
            flT = a3 / (a3 - a4);
            return TRUE;
        }
    }
    return FALSE;
}

AABBCollideFeature SweptPointAndAABB2D(const Vector2D & wpStart, const Vector2D & wvDir, const Vector2D & wpMin, const Vector2D & wpMax, float & flT, Vector2D & wpCollision) {
    AABBCollideFeature eCollFeature = Ray2DAndAABB2D(wpStart, wvDir, wpMin, wpMax, flT, wpCollision);
    if (eCollFeature == AABB_COLLIDE_NONE || flT > 1.0f)
        return AABB_COLLIDE_NONE;
    return eCollFeature;
}

// Original has one shared return (row 792): the RND0 + nCorner store at 784 is at a 12-space column,
// inside both ifs; eCollFeature is decl-initialised at 727 and the other locals follow it.
AABBCollideFeature SweptPointAndRoundedAABB2D(const Vector2D & wpStart, const Vector2D & wvDir, const Vector2D & wpMin, const Vector2D & wpMax, uint16 uFlags, float & flT, Vector2D & wpCollision) {
    AABBCollideFeature eCollFeature = Ray2DAndAABB2D(wpStart, wvDir, wpMin, wpMax, flT, wpCollision);
    if (eCollFeature == AABB_COLLIDE_NONE || flT > 1.0f)
        return AABB_COLLIDE_NONE;

    // Locals in DWARF order: the diagonals (736), wvZero (737), wvGridCellWidth (738), nCorner (739)
    Vector2D wpDiagonalA, wpDiagonalB;
    Vector2D wvZero(0.0f, 0.0f);
    Vector2D wvGridCellWidth(cflGridCellWidth, cflGridCellWidth);
    int nCorner = -1;


    // Find a rounded corner whose region the hit lies in
    for (int i = 0; i < 4; i++) {
        if (uFlags & (1 << (CELL_STATIC_ROUNDED_CORNER_0 + i))) {
            Vector2D wpCorner((i & 1) ? wpMax.x : wpMin.x, (i & 2) ? wpMax.y : wpMin.y);
            Vector2D wvToMin((i & 1) ? wvGridCellWidth.x : wvZero.x, (i & 2) ? wvGridCellWidth.y : wvZero.y);
            Vector2D wvToMax((i & 1) ? wvZero.x : wvGridCellWidth.x, (i & 2) ? wvZero.y : wvGridCellWidth.y);
            Vector2D wpRndMin = wpCorner - wvToMin;
            Vector2D wpRndMax = wpCorner + wvToMax;

            if (Point2DInsideAABB2D(wpCollision, wpRndMin, wpRndMax)) {
                nCorner = i;
                wpDiagonalA = wpCorner + sgvCrnDeltaA[i];
                wpDiagonalB = wpCorner + sgvCrnDeltaB[i];
                break;
            }
        }
    }

    // One shared return; the else store has no row of its own (mov w8, wzr on line 0)
    if (nCorner != -1) {
        // Intersect against the diagonal that cuts off the corner
        Vector2D wpDest = wpStart + wvDir;
        if (SweptPoint2DAndLine2D(wpStart, wpDest, wpDiagonalA, wpDiagonalB, flT)) {
            eCollFeature = (AABBCollideFeature)(AABB_COLLIDE_RND0 + nCorner);
            wpCollision = wpStart + wvDir * flT;
        } else
            eCollFeature = AABB_COLLIDE_NONE;
    }
    return eCollFeature;
}



BOOL Point2DInsideRoundedAABB2D(const Vector2D & wpPoint, const Vector2D & wpNavCellMin, const Vector2D & wpNavCellMax, uint16 uFlags) {
    Vector2D wvZero(0.0f, 0.0f);
    Vector2D wvGridCellWidth(cflGridCellWidth, cflGridCellWidth);

    if (!Point2DInsideAABB2D(wpPoint, wpNavCellMin, wpNavCellMax))
        return FALSE;

    for (int32 i = 0; i < 4; i++) {
        if (uFlags & (1 << (CELL_STATIC_ROUNDED_CORNER_0 + i))) {
            Vector2D wpCorner((i & 1) ? wpNavCellMax.x : wpNavCellMin.x, (i & 2) ? wpNavCellMax.y : wpNavCellMin.y);
            Vector2D wvToMin((i & 1) ? wvGridCellWidth.x : wvZero.x, (i & 2) ? wvGridCellWidth.y : wvZero.y);
            Vector2D wvToMax((i & 1) ? wvZero.x : wvGridCellWidth.x, (i & 2) ? wvZero.y : wvGridCellWidth.y);
            Vector2D wpRndMin = wpCorner - wvToMin;
            Vector2D wpRndMax = wpCorner + wvToMax;

            if (Point2DInsideAABB2D(wpPoint, wpRndMin, wpRndMax)) {
                // Inside the corner box: reject if on the same side of the diagonal as the corner
                Vector2D vDiagonalA = wpCorner + sgvCrnDeltaA[i];
                Vector2D vDiagonalB = wpCorner + sgvCrnDeltaB[i];
                float a1 = Signed2DTriArea(vDiagonalA, vDiagonalB, wpCorner);
                float a2 = Signed2DTriArea(vDiagonalA, vDiagonalB, wpPoint);
                if (a1 * a2 >= 0.0f)
                    return FALSE;
            }
        }
    }
    return TRUE;
}
