#pragma once

#include "../SigmaTypes.h"
#include "Vector.h"

struct Circle {
    Vector2D wpCenter;
    float wdRadius;

    void Set(const Vector2D& _wpCenter, const float _wdRadius) {
        wpCenter = _wpCenter;
        wdRadius = _wdRadius;
    }

    void Set(const float _wdX, const float _wdY, const float _wdRadius) {
        wpCenter.Set(_wdX, _wdY);
        wdRadius = _wdRadius;
    }

    BOOL ContainsPoint(const Vector2D& wp) const {
        float wdSquared = Vector2D::DistanceSquared(wpCenter, wp);
        return wdSquared <= wdRadius * wdRadius;
    }
};

struct Sphere {
    Vector3D wpCenter;
    float wdRadius;

    void Set(const Vector3D& _wpCenter, const float _wdRadius) {
        wpCenter = _wpCenter;
        wdRadius = _wdRadius;
    }

    BOOL ContainsPoint(const Vector3D& wp) const {
        float wdSquared = Vector3D::DistanceSquared(wpCenter, wp);
        return wdSquared <= wdRadius * wdRadius;
    }
};

class XTypeDescriptor;

// Defined in Sphere.cpp (line 18 and 23).
extern const XTypeDescriptor * DT_CIRCLE;
extern const XTypeDescriptor * DT_SPHERE;

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(Circle) == 0xc, "Circle");
static_assert(offsetof(Circle, wpCenter) == 0x0, "Circle");
static_assert(offsetof(Circle, wdRadius) == 0x8, "Circle");
static_assert(sizeof(Sphere) == 0x10, "Sphere");
static_assert(offsetof(Sphere, wpCenter) == 0x0, "Sphere");
static_assert(offsetof(Sphere, wdRadius) == 0xc, "Sphere");
