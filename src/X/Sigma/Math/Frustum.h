#pragma once

#include "../SigmaTypes.h"
#include "MathGeneral.h"
#include "Plane.h"

struct Vector3D;

enum {
    _PLANE_TOP = 0,
    _PLANE_RIGHT = 1,
    _PLANE_LEFT = 2,
    _PLANE_BOTTOM = 3,
    _PLANE_FAR = 4,
    _PLANE_NEAR = 5,
    NUM_VIEW_FRUSTUM_PLANES = 6,
};

enum FrustumCorner {
    FC_NEAR_TL = 0,
    FC_NEAR_TR = 1,
    FC_NEAR_BL = 2,
    FC_NEAR_BR = 3,
    FC_FAR_TL = 4,
    FC_FAR_TR = 5,
    FC_FAR_BL = 6,
    FC_FAR_BR = 7,
    NUM_FRUSTUM_CORNERS = 8,
};

class Frustum {
public:
    void Set(const Vector3D* wp);
    static void ComputeCorners(const Vector3D& wpLookFrom, const Vector3D& wpLookTo, const Vector3D& wpLookUp,
                               const Angle aFOV, const float flAspectRatio, const float flNearPlane,
                               const float flFarPlane, Vector3D* wp);
    void Set(const Vector3D& wpLookFrom, const Vector3D& wpLookTo, const Vector3D& wpLookUp, const Angle aFOV,
             const float flAspectRatio, const float flNearPlane, const float flFarPlane);
    BOOL ContainsPoint(const Vector3D& wp) const;
    const Plane* GetPlanes() const { return wplane; }

private:
    Plane wplane[NUM_VIEW_FRUSTUM_PLANES];
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(Frustum) == 0x60, "Frustum");
