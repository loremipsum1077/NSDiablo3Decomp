#include "Intersect.h"

#include "MathGeneral.h"
#include "Matrix.h"
#include "Transform.h"

static BOOL sSlabIntersectionTest(const float wdE, const float wdF, const float wdHalfLength, float& wdTMin,
                                  float& wdTMax);

BOOL Intersect::Ray2DAndCircle(const Ray2D& wr, const Circle& wc, float& wdT) {
    Vector2D wvL = wc.wpCenter - wr.wpOrigin;

    float wdD = Vector2D::DotProduct(wvL, wr.wvDirection);
    float wdLMagSqr = wvL.MagnitudeSquared();
    float wdRSqr = wc.wdRadius * wc.wdRadius;

    if (wdD < 0.0f && wdLMagSqr > wdRSqr) {
        return FALSE;
    }

    float wdMSqr = wdLMagSqr - wdD * wdD;
    if (wdMSqr > wdRSqr) {
        return FALSE;
    }

    if (wdLMagSqr > wdRSqr) {
        wdT = wdD - Math::Sqrt(wdRSqr - wdMSqr);
    } else {
        wdT = wdD + Math::Sqrt(wdRSqr - wdMSqr);
    }

    wdT = Math::Max(wdT, 0.0f);
    return TRUE;
}

BOOL Intersect::Ray2DAndRect2D(const Ray2D& wr, const Rect2D& wrect, float& wdT, const float wdTMaxInitial) {
    Vector2D wvExt((wrect.right - wrect.left) * 0.5f, (wrect.bottom - wrect.top) * 0.5f);
    Vector2D wpCenter((wrect.right + wrect.left) * 0.5f, (wrect.bottom + wrect.top) * 0.5f);

    Vector2D wvP = wpCenter - wr.wpOrigin;

    float wdTMin = -MAX_WORLDDISTANCE;
    float wdTMax = wdTMaxInitial;

    {
        float wdE = wvP.x;
        float wdF = wr.wvDirection.x;
        if (!sSlabIntersectionTest(wdE, wdF, wvExt.x, wdTMin, wdTMax)) {
            return FALSE;
        }
    }

    {
        float wdE = wvP.y;
        float wdF = wr.wvDirection.y;
        if (!sSlabIntersectionTest(wdE, wdF, wvExt.y, wdTMin, wdTMax)) {
            return FALSE;
        }
    }

    wdT = Math::Max((wdTMin > EPSILON) ? wdTMin : wdTMax, 0.0f);
    return TRUE;
}

inline BOOL Intersect::Ray3DAndSphere(const Ray3D& wr, const Sphere& ws, float& wdT) {
    Vector3D wvL = ws.wpCenter - wr.wpOrigin;

    float wdD = Vector3D::DotProduct(wvL, wr.wvDirection);
    float wdLMagSqr = wvL.MagnitudeSquared();
    float wdRSqr = ws.wdRadius * ws.wdRadius;

    if (wdD < 0.0f && wdLMagSqr > wdRSqr) {
        return FALSE;
    }

    float wdMSqr = wdLMagSqr - wdD * wdD;
    if (wdMSqr > wdRSqr) {
        return FALSE;
    }

    float wdQ = Math::Sqrt(wdRSqr - wdMSqr);
    if (wdLMagSqr > wdRSqr) {
        wdT = wdD - wdQ;
    } else {
        wdT = wdD + wdQ;
    }

    wdT = Math::Max(wdT, 0.0f);
    return TRUE;
}

BOOL Intersect::Ray3DAndTriangle3D(const Ray3D& wr, const Triangle3D& wtri, float& wdT) {
    float flDet;
    float flInvDet;
    float flU;
    float flV;

    Vector3D wvEdge1 = wtri.wp1 - wtri.wp0;
    Vector3D wvEdge2 = wtri.wp2 - wtri.wp0;

    Vector3D wvP = Vector3D::CrossProduct(wr.wvDirection, wvEdge2);
    flDet = Vector3D::DotProduct(wvEdge1, wvP);

    if (Math::Abs(flDet) < TRIANGLE_INTERSECT_EPSILON) {
        return FALSE;
    }

    Vector3D wvS = wr.wpOrigin - wtri.wp0;

    flInvDet = 1.0f / flDet;

    flU = Vector3D::DotProduct(wvS, wvP) * flInvDet;
    if (flU < -TRIANGLE_INTERSECT_EPSILON || flU > 1.0f + TRIANGLE_INTERSECT_EPSILON) {
        return FALSE;
    }

    Vector3D wvQ = Vector3D::CrossProduct(wvS, wvEdge1);
    flV = Vector3D::DotProduct(wr.wvDirection, wvQ) * flInvDet;
    if (flV < -TRIANGLE_INTERSECT_EPSILON || flU + flV > 1.0f + TRIANGLE_INTERSECT_EPSILON) {
        return FALSE;
    }

    wdT = Vector3D::DotProduct(wvEdge2, wvQ) * flInvDet;

    return wdT > -TRIANGLE_INTERSECT_EPSILON;
}

BOOL Intersect::Ray3DAndTriangle3DCull(const Ray3D& wr, const Triangle3D& wtri, float& wdT) {
    float flDet;
    float flU;
    float flV;

    Vector3D wvEdge1 = wtri.wp1 - wtri.wp0;
    Vector3D wvEdge2 = wtri.wp2 - wtri.wp0;

    Vector3D wvP = Vector3D::CrossProduct(wr.wvDirection, wvEdge2);
    flDet = Vector3D::DotProduct(wvEdge1, wvP);

    if (flDet < TRIANGLE_INTERSECT_EPSILON) {
        return FALSE;
    }

    Vector3D wvS = wr.wpOrigin - wtri.wp0;

    flU = Vector3D::DotProduct(wvS, wvP);
    if (flU < -TRIANGLE_INTERSECT_EPSILON || flU > flDet * TRIANGLE_INTERSECT_EPSILON_FACTOR) {
        return FALSE;
    }

    Vector3D wvQ = Vector3D::CrossProduct(wvS, wvEdge1);
    flV = Vector3D::DotProduct(wr.wvDirection, wvQ);
    if (flV < -TRIANGLE_INTERSECT_EPSILON || flU + flV > flDet * TRIANGLE_INTERSECT_EPSILON_FACTOR) {
        return FALSE;
    }

    wdT = Vector3D::DotProduct(wvEdge2, wvQ);
    if (wdT > -TRIANGLE_INTERSECT_EPSILON) {
        wdT /= flDet;
        return TRUE;
    }

    return FALSE;
}

BOOL Intersect::Ray3DAndTriangle3D(const Ray3D& wr, const Triangle3D& wtri, float& flU, float& flV) {
    float flDet;
    float flInvDet;
    float wdT;

    Vector3D wvEdge1 = wtri.wp1 - wtri.wp0;
    Vector3D wvEdge2 = wtri.wp2 - wtri.wp0;

    Vector3D wvP = Vector3D::CrossProduct(wr.wvDirection, wvEdge2);
    flDet = Vector3D::DotProduct(wvEdge1, wvP);

    if (Math::Abs(flDet) < EPSILON) {
        return FALSE;
    }

    Vector3D wvS = wr.wpOrigin - wtri.wp0;

    flInvDet = 1.0f / flDet;

    flU = Vector3D::DotProduct(wvS, wvP) * flInvDet;
    if (flU < -TRIANGLE_INTERSECT_EPSILON || flU > 1.0f + TRIANGLE_INTERSECT_EPSILON) {
        return FALSE;
    }

    Vector3D wvQ = Vector3D::CrossProduct(wvS, wvEdge1);
    flV = Vector3D::DotProduct(wr.wvDirection, wvQ) * flInvDet;
    if (flV < -TRIANGLE_INTERSECT_EPSILON || flU + flV > 1.0f + TRIANGLE_INTERSECT_EPSILON) {
        return FALSE;
    }

    wdT = Vector3D::DotProduct(wvEdge2, wvQ) * flInvDet;

    return wdT > -TRIANGLE_INTERSECT_EPSILON;
}

BOOL Intersect::Ray3DAndAABB(const Ray3D& wr, const AABB& aabb, float& wdT, const float wdTMaxInitial) {
    float wdTMin = -MAX_WORLDDISTANCE;
    float wdTMax = wdTMaxInitial;

    Vector3D wvP = aabb.wp - wr.wpOrigin;

    {
        float wdE = wvP.x;
        float wdF = wr.wvDirection.x;
        if (!sSlabIntersectionTest(wdE, wdF, aabb.wvExt.x, wdTMin, wdTMax)) {
            return FALSE;
        }
    }

    {
        float wdE = wvP.y;
        float wdF = wr.wvDirection.y;
        if (!sSlabIntersectionTest(wdE, wdF, aabb.wvExt.y, wdTMin, wdTMax)) {
            return FALSE;
        }
    }

    {
        float wdE = wvP.z;
        float wdF = wr.wvDirection.z;
        if (!sSlabIntersectionTest(wdE, wdF, aabb.wvExt.z, wdTMin, wdTMax)) {
            return FALSE;
        }
    }

    wdT = Math::Max((wdTMin > EPSILON) ? wdTMin : wdTMax, 0.0f);
    return TRUE;
}

static inline BOOL sSlabIntersectionTest(const float wdE, const float wdF, const float wdHalfLength, float& wdTMin,
                                         float& wdTMax) {
    if (Math::Abs(wdF) > EPSILON) {
        float wdFRecip = 1.0f / wdF;
        float wdT1 = (wdE + wdHalfLength) * wdFRecip;
        float wdT2 = (wdE - wdHalfLength) * wdFRecip;

        if (wdT1 > wdT2) {
            Math::Swap(wdT1, wdT2);
        }

        if (wdT1 > wdTMin) {
            wdTMin = wdT1;
        }
        if (wdT2 < wdTMax) {
            wdTMax = wdT2;
        }

        if (wdTMin > wdTMax) {
            return FALSE;
        }
        if (wdTMax < 0.0f) {
            return FALSE;
        }
    } else if (-wdE - wdHalfLength > 0.0f || -wdE + wdHalfLength < 0.0f) {
        return FALSE;
    }

    return TRUE;
}

BOOL Intersect::LineSeg2DAndCircle(const LineSeg2D& wline, const Circle& wc, float& wdT, Vector2D* pwpIntersect) {
    if (wc.ContainsPoint(wline.wr.wpOrigin)) {
        wdT = 0.0f;
        if (pwpIntersect) {
            *pwpIntersect = wline.wr.wpOrigin;
        }
        return TRUE;
    }

    if (Ray2DAndCircle(wline.wr, wc, wdT) && wdT <= wline.wdLength) {
        if (pwpIntersect) {
            wline.wr.GetEndpoint(wdT, *pwpIntersect);
        }
        return TRUE;
    }

    return FALSE;
}

BOOL Intersect::LineSeg3DAndSphere(const LineSeg3D& wline, const Sphere& ws, float& wdT, Vector3D* pwpIntersect) {
    if (ws.ContainsPoint(wline.wr.wpOrigin)) {
        wdT = 0.0f;
        if (pwpIntersect) {
            *pwpIntersect = wline.wr.wpOrigin;
        }
        return TRUE;
    }

    if (Ray3DAndSphere(wline.wr, ws, wdT) && wdT <= wline.wdLength) {
        if (pwpIntersect) {
            wline.wr.GetEndpoint(wdT, *pwpIntersect);
        }
        return TRUE;
    }

    return FALSE;
}

BOOL Intersect::LineSeg3DAndTriangle3D(const LineSeg3D& wline, const Triangle3D& wtri, float& wdT,
                                       Vector3D* pwpIntersect) {
    if (Ray3DAndTriangle3D(wline.wr, wtri, wdT)) {
        if (wdT < wline.wdLength + TRIANGLE_INTERSECT_EPSILON) {
            if (pwpIntersect) {
                wline.wr.GetEndpoint(wdT, *pwpIntersect);
            }
            return TRUE;
        }
    }

    return FALSE;
}

BOOL Intersect::LineSeg3DAndTriangle3DCull(const LineSeg3D& wline, const Triangle3D& wtri, float& wdT,
                                           Vector3D* pwpIntersect) {
    if (Ray3DAndTriangle3DCull(wline.wr, wtri, wdT)) {
        if (wdT < wline.wdLength + TRIANGLE_INTERSECT_EPSILON) {
            if (pwpIntersect) {
                wline.wr.GetEndpoint(wdT, *pwpIntersect);
            }
            return TRUE;
        }
    }

    return FALSE;
}

BOOL Intersect::LineSeg3DAndAABB(const LineSeg3D& wline, const AABB& wb, float& wdT) {
    const Vector3D& wp0 = wline.wr.wpOrigin;
    Vector3D wp1;
    wline.wr.GetEndpoint(wline.wdLength, wp1);

    if (wp0.x < wb.wp.x - wb.wvExt.x && wp1.x < wb.wp.x - wb.wvExt.x) {
        return FALSE;
    }

    if (wp0.x > wb.wp.x + wb.wvExt.x && wp1.x > wb.wp.x + wb.wvExt.x) {
        return FALSE;
    }

    if (wp0.y < wb.wp.y - wb.wvExt.y && wp1.y < wb.wp.y - wb.wvExt.y) {
        return FALSE;
    }

    if (wp0.y > wb.wp.y + wb.wvExt.y && wp1.y > wb.wp.y + wb.wvExt.y) {
        return FALSE;
    }

    if (wp0.z < wb.wp.z - wb.wvExt.z && wp1.z < wb.wp.z - wb.wvExt.z) {
        return FALSE;
    }

    if (wp0.z > wb.wp.z + wb.wvExt.z && wp1.z > wb.wp.z + wb.wvExt.z) {
        return FALSE;
    }

    if (wb.ContainsPoint(wp0) ||
        wb.ContainsPoint(wp1)) {
        return TRUE;
    }

    return Ray3DAndAABB(wline.wr, wb, wdT, wline.wdLength) != FALSE;
}

BOOL Intersect::LineSeg2DAndOBB(const LineSeg2D& wline, const OBB& wb, float& wdT, Vector2D* pwpIntersect) {
    Ray2D wrTemp;
    Rect2D rect;
    BOOL bIsect;

    PRTransform transformInv = wb.transform.Inverse();
    wrTemp.wpOrigin = transformInv * wline.wr.wpOrigin;
    wrTemp.wvDirection = transformInv.Rotate(wline.wr.wvDirection);

    rect.left = -wb.wvExt.x;
    rect.top = -wb.wvExt.y;
    rect.right = wb.wvExt.x;
    rect.bottom = wb.wvExt.y;

    bIsect = Ray2DAndRect2D(wrTemp, rect, wdT, wline.wdLength);

    if (pwpIntersect && bIsect) {
        *pwpIntersect = wline.wr.wpOrigin + wline.wr.wvDirection * wdT;
    }

    return bIsect;
}

BOOL Intersect::Ray3DAndAxialCylinder(const Ray3D& wr, const AxialCylinder& wcyl, float& wdT) {
    BOOL bHit = FALSE;

    Vector2D vCbRb(wr.wpOrigin.x - wcyl.wpBase.x, wr.wpOrigin.y - wcyl.wpBase.y);
    float flC = vCbRb.MagnitudeSquared() - wcyl.wdRadius * wcyl.wdRadius;

    if (flC <= 0.0f && wr.wpOrigin.z >= wcyl.wpBase.z && wr.wpOrigin.z <= wcyl.wpBase.z + wcyl.wdHeight) {
        wdT = 0.0f;
        return TRUE;
    }

    wdT = 1.0f;

    Vector2D vRayDirectionXY(wr.wvDirection.x, wr.wvDirection.y);
    float flA = vRayDirectionXY.MagnitudeSquared();
    if (flA > EPSILON) {
        float flB = Vector2D::DotProduct(vCbRb, vRayDirectionXY);
        float flDet = flB * flB - flA * flC;
        if (flDet < 0.0f) {
            return FALSE;
        }

        float flSqrtDet = Math::Sqrt(flDet);
        if (flB > flSqrtDet) {
            return FALSE;
        }

        float flInvA = 1.0f / flA;

        wdT = (-flB - flSqrtDet) * flInvA;
        if (wdT >= 0.0f) {
            float flZ = wr.wpOrigin.z - wcyl.wpBase.z + wdT * wr.wvDirection.z;
            if (flZ >= 0.0f && flZ <= wcyl.wdHeight) {
                bHit = TRUE;
            }
        }

        if (!bHit) {
            wdT = (-flB + flSqrtDet) * flInvA;
            if (wdT >= 0.0f) {
                float flZ = wr.wpOrigin.z - wcyl.wpBase.z + wdT * wr.wvDirection.z;
                if (flZ >= 0.0f && flZ <= wcyl.wdHeight) {
                    bHit = TRUE;
                }
            }
        }
    }

    if (Math::Abs(wr.wvDirection.z) > EPSILON) {
        float wdTBase = (wcyl.wpBase.z - wr.wpOrigin.z) / wr.wvDirection.z;
        float wpX = wr.wpOrigin.x + wdTBase * wr.wvDirection.x;
        float wpY = wr.wpOrigin.y + wdTBase * wr.wvDirection.y;
        if ((wpX - wcyl.wpBase.x) * (wpX - wcyl.wpBase.x) + (wpY - wcyl.wpBase.y) * (wpY - wcyl.wpBase.y) <
            wcyl.wdRadius * wcyl.wdRadius) {
            wdT = Math::Min(wdT, wdTBase);
            bHit = TRUE;
        }

        float wdTTop = (wcyl.wpBase.z + wcyl.wdHeight - wr.wpOrigin.z) / wr.wvDirection.z;
        wpX = wr.wpOrigin.x + wdTTop * wr.wvDirection.x;
        wpY = wr.wpOrigin.y + wdTTop * wr.wvDirection.y;
        if ((wpX - wcyl.wpBase.x) * (wpX - wcyl.wpBase.x) + (wpY - wcyl.wpBase.y) * (wpY - wcyl.wpBase.y) <
            wcyl.wdRadius * wcyl.wdRadius) {
            wdT = Math::Min(wdT, wdTTop);
            bHit = TRUE;
        }
    }

    return bHit;
}

BOOL Intersect::LineSeg3DAndAxialCylinder(const LineSeg3D& wlineseg, const AxialCylinder& wcyl, float& wdT) {
    if (Ray3DAndAxialCylinder(wlineseg.wr, wcyl, wdT) && wdT <= wlineseg.wdLength) {
        return TRUE;
    }

    return FALSE;
}

BOOL Intersect::Ray3DAndPlane(const Ray3D& wr, const Plane& wplane, Vector3D& wpIntersection) {
    float flT;
    return Ray3DAndPlane(wr, wplane, wpIntersection, flT);
}

inline BOOL Intersect::Ray3DAndPlane(const Ray3D& wr, const Plane& wplane, Vector3D& wpIntersection, float& flT) {
    float flDist = wplane.ContainsPoint(wr.wpOrigin);
    float flDot = Vector3D::DotProduct(wplane.wvNormal, wr.wvDirection);

    flT = -flDist / flDot;
    if (flT < 0.0f) {
        return FALSE;
    }

    wpIntersection = wr.wpOrigin + wr.wvDirection * flT;
    return TRUE;
}

BOOL Intersect::LineSeg3DAndPlane(const LineSeg3D& wline, const Plane& wplane, Vector3D& wpIntersection) {
    float flT;
    Vector3D wpOriginalValue(wpIntersection);

    if (!Ray3DAndPlane(wline.wr, wplane, wpIntersection, flT)) {
        return FALSE;
    }

    if (flT <= wline.wdLength) {
        return TRUE;
    }

    wpIntersection = wpOriginalValue;
    return FALSE;
}

BOOL Intersect::CapsuleAndSphere(const Capsule& capsule, const Sphere& ws) {
    Sphere wsIntersect(ws);
    wsIntersect.wdRadius += capsule.flRadius;

    float wdT;

    LineSeg3D lineSeg;
    lineSeg.Set(capsule.wp1, capsule.wp2);

    return LineSeg3DAndSphere(lineSeg, wsIntersect, wdT, NULL);
}

BOOL Intersect::CapsuleAndAxialCylinder(const Capsule& capsule, const AxialCylinder& wcyl, float& wdT) {
    BOOL bRetVal;

    AxialCylinder wcylIntersect(wcyl);
    wcylIntersect.wpBase.z -= capsule.flRadius;
    wcylIntersect.wdHeight += 2.0f * capsule.flRadius;
    wcylIntersect.wdRadius += capsule.flRadius;

    LineSeg3D lineSeg;
    lineSeg.Set(capsule.wp1, capsule.wp2);

    if (lineSeg.wdLength > EPSILON) {
        bRetVal = LineSeg3DAndAxialCylinder(lineSeg, wcylIntersect, wdT);
    } else {
        Sphere sphere;
        sphere.Set(capsule.wp1, capsule.flRadius);
        bRetVal = AxialCylinderAndSphere(wcylIntersect, sphere);
        if (bRetVal) {
            wdT = 0.0f;
        }
    }

    return bRetVal;
}

BOOL Intersect::AxialCylinderAndSphere(const AxialCylinder& wcyl, const Sphere& ws) {
    float Sx = ws.wpCenter.x;
    float Sy = ws.wpCenter.y;
    float Cx = wcyl.wpBase.x;
    float Cy = wcyl.wpBase.y;
    float Sr = ws.wdRadius;
    float Cr = wcyl.wdRadius;

    if ((Sx - Cx) * (Sx - Cx) + (Sy - Cy) * (Sy - Cy) > (Sr + Cr) * (Sr + Cr)) {
        return FALSE;
    }

    float Sz = ws.wpCenter.z;
    float Cz0 = wcyl.wpBase.z;
    float Cz1 = Cz0 + wcyl.wdHeight;

    if (Sz >= Cz0 && Sz <= Cz1) {
        return TRUE;
    }

    Vector3D wpInterectionCandidate(Cx, Cy, 0.0f);
    Vector3D wvSxyMinusCxY(Sx - Cx, Sy - Cy, 0.0f);
    float wdAmplitudeSxyMinusCxY = wvSxyMinusCxY.Normalize();
    wpInterectionCandidate += wvSxyMinusCxY * Math::Min(Cr, wdAmplitudeSxyMinusCxY);

    if (Sz > Cz1) {
        wpInterectionCandidate.z = Cz1;
    } else {
        wpInterectionCandidate.z = Cz0;
    }

    return ws.ContainsPoint(wpInterectionCandidate);
}

BOOL Intersect::AxialCylinderAndAxialCylinder(const AxialCylinder& wcylA, const AxialCylinder& wcylB) {
    float wdTotalRadius = wcylA.wdRadius + wcylB.wdRadius;

    if (Vector3D::Distance2DSquared(wcylA.wpBase, wcylB.wpBase) > wdTotalRadius * wdTotalRadius) {
        return FALSE;
    }

    if (wcylB.wpBase.z > wcylA.wpBase.z + wcylA.wdHeight || wcylA.wpBase.z > wcylB.wpBase.z + wcylB.wdHeight) {
        return FALSE;
    }

    return TRUE;
}

BOOL Intersect::FrustumAndAABB(const Frustum& wf, const AABB& aabb) {
    const Plane* ptPlanes = wf.GetPlanes();

    for (int32 nPlaneIndex = NUM_VIEW_FRUSTUM_PLANES - 1; nPlaneIndex >= 0; nPlaneIndex--) {
        const Plane& wplane = ptPlanes[nPlaneIndex];

        Vector3D vecMin;
        vecMin.x = aabb.wp.x + ((wplane.wvNormal.x >= 0.0f) ? -aabb.wvExt.x : aabb.wvExt.x);
        vecMin.y = aabb.wp.y + ((wplane.wvNormal.y >= 0.0f) ? -aabb.wvExt.y : aabb.wvExt.y);
        vecMin.z = aabb.wp.z + ((wplane.wvNormal.z >= 0.0f) ? -aabb.wvExt.z : aabb.wvExt.z);

        if (wplane.ContainsPoint(vecMin) > 0.0f) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL Intersect::FrustumAndSphere(const Frustum& frustum, const Sphere& ws) {
    const Plane* ptPlanes = frustum.GetPlanes();

    for (int32 nPlaneIndex = NUM_VIEW_FRUSTUM_PLANES - 1; nPlaneIndex >= 0; nPlaneIndex--) {
        if (ptPlanes[nPlaneIndex].ContainsPoint(ws.wpCenter) > ws.wdRadius) {
            return FALSE;
        }
    }

    return TRUE;
}

Intersect::DetailedFrustumResult Intersect::DetailedFrustumAndSphere(const Frustum& frustum, const Sphere& ws) {
    const Plane* ptPlanes = frustum.GetPlanes();
    DetailedFrustumResult tResult = INTERSECT_INSIDE;

    for (int32 nPlaneIndex = NUM_VIEW_FRUSTUM_PLANES - 1; nPlaneIndex >= 0; nPlaneIndex--) {
        float fDistance = ptPlanes[nPlaneIndex].ContainsPoint(ws.wpCenter);
        if (fDistance > ws.wdRadius) {
            return INTERSECT_OUTSIDE;
        }
        if (fDistance > -ws.wdRadius) {
            tResult = INTERSECT_PARTIALLY;
        }
    }

    return tResult;
}

BOOL Intersect::AABBAndSphere(const AABB& aabb, const Sphere& ws) {
    float wdD = 0.0f;

    Vector3D wpMin = aabb.wp - aabb.wvExt;
    Vector3D wpMax = aabb.wp + aabb.wvExt;

    if (ws.wpCenter.x < wpMin.x) {
        float flDiff = ws.wpCenter.x - wpMin.x;
        wdD += flDiff * flDiff;
    } else if (ws.wpCenter.x > wpMax.x) {
        float flDiff = ws.wpCenter.x - wpMax.x;
        wdD += flDiff * flDiff;
    }

    if (ws.wpCenter.y < wpMin.y) {
        float flDiff = ws.wpCenter.y - wpMin.y;
        wdD += flDiff * flDiff;
    } else if (ws.wpCenter.y > wpMax.y) {
        float flDiff = ws.wpCenter.y - wpMax.y;
        wdD += flDiff * flDiff;
    }

    if (ws.wpCenter.z < wpMin.z) {
        float flDiff = ws.wpCenter.z - wpMin.z;
        wdD += flDiff * flDiff;
    } else if (ws.wpCenter.z > wpMax.z) {
        float flDiff = ws.wpCenter.z - wpMax.z;
        wdD += flDiff * flDiff;
    }

    if (wdD > ws.wdRadius * ws.wdRadius) {
        return FALSE;
    }

    return TRUE;
}

static inline void sClosestPtPointOBB(const Vector3D& p, const OBB& obb, Vector3D& closestPt) {
    Vector3D mtxObbBasis[3];
    obb.transform.q.ToMatrix(*(Matrix3x3*)mtxObbBasis);

    closestPt = obb.transform.wp;

    Vector3D d = p - obb.transform.wp;

    for (int32 i = 0; i < 3; i++) {
        float flDist = Vector3D::DotProduct(d, mtxObbBasis[i]);

        if (flDist > (&obb.wvExt.x)[i]) {
            flDist = (&obb.wvExt.x)[i];
        }
        if (flDist < -(&obb.wvExt.x)[i]) {
            flDist = -(&obb.wvExt.x)[i];
        }

        closestPt += flDist * mtxObbBasis[i];
    }
}

BOOL Intersect::SphereAndOBB(const Sphere& ws, const OBB& obb, Vector3D* pwpClosest) {
    Vector3D ptClosest;
    sClosestPtPointOBB(ws.wpCenter, obb, ptClosest);

    BOOL bIntersect = Vector3D::DistanceSquared(ptClosest, ws.wpCenter) <= ws.wdRadius * ws.wdRadius;

    if (pwpClosest) {
        *pwpClosest = ptClosest;
    }

    return bIntersect;
}
