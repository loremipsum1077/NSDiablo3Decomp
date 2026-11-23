#pragma once

#include "../SigmaTypes.h"
#include "Vector.h"

struct Matrix3x3;
struct PRTransform;
struct Plane;

struct AxialCylinder {
    Vector3D wpBase;
    float wdHeight;
    float wdRadius;

    void Set(const Vector3D& _wpBase, const float _wdHeight, const float _wdRadius);
};

struct Cylinder {
    Vector3D wp1;
    Vector3D wp2;
    float flRadius;
};

struct Capsule {
    Vector3D wp1;
    Vector3D wp2;
    float flRadius;

    void Set(const PRTransform& transform, const float _flRadius, const float flHalfAxialLength);
    Vector3D PushToSurface(const Vector3D& wp) const;
};

class XTypeDescriptor;

// Defined in Cylinder.cpp (lines 20, 26 and 32).
extern const XTypeDescriptor * DT_AXIALCYLINDER;
extern const XTypeDescriptor * DT_CYLINDER;
extern const XTypeDescriptor * DT_CAPSULE;

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(AxialCylinder) == 0x14, "AxialCylinder");
static_assert(offsetof(AxialCylinder, wpBase) == 0x0, "AxialCylinder");
static_assert(offsetof(AxialCylinder, wdHeight) == 0xc, "AxialCylinder");
static_assert(offsetof(AxialCylinder, wdRadius) == 0x10, "AxialCylinder");
static_assert(sizeof(Cylinder) == 0x1c, "Cylinder");
static_assert(offsetof(Cylinder, wp1) == 0x0, "Cylinder");
static_assert(offsetof(Cylinder, wp2) == 0xc, "Cylinder");
static_assert(offsetof(Cylinder, flRadius) == 0x18, "Cylinder");
static_assert(sizeof(Capsule) == 0x1c, "Capsule");
static_assert(offsetof(Capsule, wp1) == 0x0, "Capsule");
static_assert(offsetof(Capsule, wp2) == 0xc, "Capsule");
static_assert(offsetof(Capsule, flRadius) == 0x18, "Capsule");
