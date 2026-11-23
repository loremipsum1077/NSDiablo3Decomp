#include "Frustum.h"

void Frustum::Set(const Vector3D* wp) {
    wplane[_PLANE_TOP].Set(wp[FC_NEAR_TL], wp[FC_FAR_TR], wp[FC_FAR_TL]);
    wplane[_PLANE_BOTTOM].Set(wp[FC_NEAR_BR], wp[FC_FAR_BL], wp[FC_FAR_BR]);
    wplane[_PLANE_RIGHT].Set(wp[FC_NEAR_TR], wp[FC_FAR_BR], wp[FC_FAR_TR]);
    wplane[_PLANE_LEFT].Set(wp[FC_NEAR_BL], wp[FC_FAR_TL], wp[FC_FAR_BL]);
    wplane[_PLANE_FAR].Set(wp[FC_FAR_TL], wp[FC_FAR_BR], wp[FC_FAR_BL]);
    wplane[_PLANE_NEAR].Set(wp[FC_NEAR_TL], wp[FC_NEAR_BL], wp[FC_NEAR_BR]);
}

void Frustum::ComputeCorners(const Vector3D& wpLookFrom, const Vector3D& wpLookTo, const Vector3D& wpLookUp,
                             const Angle aFOV, const float flAspectRatio, const float flNearPlane,
                             const float flFarPlane, Vector3D* wp) {
    float flTanHalfFOV = Math::Tan(aFOV * 0.5f);

    float flFarHeight = flFarPlane * flTanHalfFOV;
    float flNearHeight = flNearPlane * flTanHalfFOV;
    float flFarWidth = flFarHeight * flAspectRatio;
    float flNearWidth = flNearHeight * flAspectRatio;

    Vector3D wpPos(wpLookFrom);
    Vector3D wvView = wpLookTo - wpLookFrom;
    wvView.Normalize();
    Vector3D wvUp(wpLookUp);

    Vector3D wvRight = Vector3D::CrossProduct(wvView, wvUp);

    Vector3D wvScaledFarUp = wvUp * flFarHeight;
    Vector3D wvScaledFarRight = wvRight * flFarWidth;
    Vector3D wvScaledNearUp = wvUp * flNearHeight;
    Vector3D wvScaledNearRight = wvRight * flNearWidth;

    Vector3D wpFar = wpPos + wvView * flFarPlane;
    Vector3D wpNear = wpPos + wvView * flNearPlane;

    wp[FC_FAR_TL] = wpFar + wvScaledFarUp - wvScaledFarRight;
    wp[FC_FAR_TR] = wpFar + wvScaledFarUp + wvScaledFarRight;
    wp[FC_FAR_BL] = wpFar - wvScaledFarUp - wvScaledFarRight;
    wp[FC_FAR_BR] = wpFar - wvScaledFarUp + wvScaledFarRight;

    wp[FC_NEAR_TL] = wpNear + wvScaledNearUp - wvScaledNearRight;
    wp[FC_NEAR_TR] = wpNear + wvScaledNearUp + wvScaledNearRight;
    wp[FC_NEAR_BL] = wpNear - wvScaledNearUp - wvScaledNearRight;
    wp[FC_NEAR_BR] = wpNear - wvScaledNearUp + wvScaledNearRight;
}

void Frustum::Set(const Vector3D& wpLookFrom, const Vector3D& wpLookTo, const Vector3D& wpLookUp, const Angle aFOV,
                  const float flAspectRatio, const float flNearPlane, const float flFarPlane) {
    Vector3D wp[NUM_FRUSTUM_CORNERS];
    ComputeCorners(wpLookFrom, wpLookTo, wpLookUp, aFOV, flAspectRatio, flNearPlane, flFarPlane, wp);
    Set(wp);
}

BOOL Frustum::ContainsPoint(const Vector3D& wp) const {
    for (int32 nPlaneIndex = NUM_VIEW_FRUSTUM_PLANES - 1; nPlaneIndex >= 0; nPlaneIndex--) {
        if (wplane[nPlaneIndex].ContainsPoint(wp) > 0.0f) {
            return FALSE;
        }
    }
    return TRUE;
}
