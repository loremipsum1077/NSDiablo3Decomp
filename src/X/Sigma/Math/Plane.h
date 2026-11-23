#pragma once

#include "Vector.h"

struct Plane {
    Vector3D wvNormal;
    float wdD;

    void Set(const Vector3D& wp, const Vector3D& _wvNormal) {  // line 25
        wvNormal = _wvNormal;
        wdD = -Vector3D::DotProduct(wvNormal, wp);
    }

    void Set(const Vector3D& wp0, const Vector3D& wp1, const Vector3D& wp2) {
        Vector3D wv1 = wp1 - wp0;
        Vector3D wv2 = wp2 - wp0;
        wvNormal = Vector3D::CrossProduct(wv1, wv2);
        wvNormal.Normalize();
        wdD = -(wvNormal.x * wp0.x + wvNormal.y * wp0.y + wvNormal.z * wp0.z);
    }

    float ContainsPoint(const Vector3D& wp) const {
        return wp.x * wvNormal.x + wp.y * wvNormal.y + wp.z * wvNormal.z + wdD;
    }

    Vector3D Reflect(const Vector3D& wp) const;
};

class XTypeDescriptor;

// Defined in Plane.cpp (lines 20 and 26).
extern const XTypeDescriptor * DT_PLANE;
extern const Plane cgwplaneNullZPlane;

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(Plane) == 0x10, "Plane");
static_assert(offsetof(Plane, wvNormal) == 0x0, "Plane");
static_assert(offsetof(Plane, wdD) == 0xc, "Plane");
