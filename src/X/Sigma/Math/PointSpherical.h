#pragma once

#include "MathGeneral.h"

struct Vector3D;

struct PointSpherical {
    float wdRadius;
    Angle aPitch;
    Angle aYaw;

    void Set(const Vector3D& wp, const Vector3D& wpOrigin);
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(PointSpherical) == 0xc, "PointSpherical");
static_assert(offsetof(PointSpherical, wdRadius) == 0x0, "PointSpherical");
static_assert(offsetof(PointSpherical, aPitch) == 0x4, "PointSpherical");
static_assert(offsetof(PointSpherical, aYaw) == 0x8, "PointSpherical");
