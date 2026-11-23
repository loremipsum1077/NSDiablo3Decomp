#pragma once

#include "../SigmaTypes.h"
#include "Vector.h"

struct Triangle2D {
    Vector2D wp0;
    Vector2D wp1;
    Vector2D wp2;

    void Set(const Vector2D& _wp0, const Vector2D& _wp1, const Vector2D& _wp2);
    BOOL ContainsPoint(const Vector2D& p) const;
};

struct Triangle3D {
    Vector3D wp0;
    Vector3D wp1;
    Vector3D wp2;

    void Set(const Vector3D& _wp0, const Vector3D& _wp1, const Vector3D& _wp2);
    void GetFaceNormal(Vector3D& wvFaceNormal) const;
    void PointFromBarycentric(const float& flU, const float& flV, Vector3D& wp) const;
    void BarycentricFromPoint(const Vector3D& wp, float& flU, float& flV, float& flW) const;
    float Area() const;
};

extern const float TRIANGLE_INTERSECT_EPSILON;
extern const float TRIANGLE_INTERSECT_EPSILON_FACTOR;

class XTypeDescriptor;

// Defined in Triangle.cpp (line 20).
extern const XTypeDescriptor * DT_TRIANGLE3D;

#include "Triangle.inl"

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(Triangle2D) == 0x18, "Triangle2D");
static_assert(offsetof(Triangle2D, wp0) == 0x0, "Triangle2D");
static_assert(offsetof(Triangle2D, wp1) == 0x8, "Triangle2D");
static_assert(offsetof(Triangle2D, wp2) == 0x10, "Triangle2D");
static_assert(sizeof(Triangle3D) == 0x24, "Triangle3D");
static_assert(offsetof(Triangle3D, wp0) == 0x0, "Triangle3D");
static_assert(offsetof(Triangle3D, wp1) == 0xc, "Triangle3D");
static_assert(offsetof(Triangle3D, wp2) == 0x18, "Triangle3D");
