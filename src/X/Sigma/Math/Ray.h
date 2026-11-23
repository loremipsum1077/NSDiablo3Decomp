#pragma once

#include "Vector.h"

struct Ray2D {
    Vector2D wpOrigin;
    Vector2D wvDirection;

    void GetEndpoint(const float& wd, Vector2D& wp) const {
        wp = wpOrigin + wvDirection * wd;
    }
};

struct LineSeg2D {
    Ray2D wr;
    float wdLength;

    void Set(const Vector2D& wp0, const Vector2D& wp1) {
        wr.wpOrigin = wp0;
        wr.wvDirection = wp1 - wp0;
        wdLength = wr.wvDirection.Normalize();
    }

    // Line 55; the DWARF inline tree (ACDCollNarrowOBB) shows Vector2D::Set at 59 and 60 and Normalize at 61.
    void Set(const Vector3D& wp0, const Vector3D& wp1) {
        wr.wpOrigin.Set(wp0.x, wp0.y);
        wr.wvDirection.Set(wp1.x - wp0.x, wp1.y - wp0.y);
        wdLength = wr.wvDirection.Normalize();
    }
};

struct Ray3D {
    Vector3D wpOrigin;
    Vector3D wvDirection;

    // Line 74; the DWARF inline tree (UIRActor::ClickIsEaten) shows the copies at 79-80 and
    // Normalize at 81.
    void Set(const Vector3D& _wpOrigin, const Vector3D& _wvDirection) {
        wpOrigin = _wpOrigin;
        wvDirection = _wvDirection;
        wvDirection.Normalize();
    }

    void GetEndpoint(const float& wd, Vector3D& wp) const {
        wp = wpOrigin + wvDirection * wd;
    }
};

struct LineSeg3D {
    Ray3D wr;
    float wdLength;

    LineSeg3D() : wr(), wdLength(0.0f) {}

    void Set(const Vector3D& wp0, const Vector3D& wp1) {
        wr.wpOrigin = wp0;
        wr.wvDirection = wp1 - wp0;
        wdLength = wr.wvDirection.Normalize();
    }

    Vector3D GetEndPoint() const;
    float ClosestPoint(const Vector3D& wp, Vector3D& wpClosestPointOnLine) const;
};

class XTypeDescriptor;

// Defined in Ray.cpp (lines 19 and 24).
extern const XTypeDescriptor * DT_RAY3D;
extern const XTypeDescriptor * DT_LINESEG3D;

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(Ray2D) == 0x10, "Ray2D");
static_assert(offsetof(Ray2D, wpOrigin) == 0x0, "Ray2D");
static_assert(offsetof(Ray2D, wvDirection) == 0x8, "Ray2D");
static_assert(sizeof(LineSeg2D) == 0x14, "LineSeg2D");
static_assert(offsetof(LineSeg2D, wr) == 0x0, "LineSeg2D");
static_assert(offsetof(LineSeg2D, wdLength) == 0x10, "LineSeg2D");
static_assert(sizeof(Ray3D) == 0x18, "Ray3D");
static_assert(offsetof(Ray3D, wpOrigin) == 0x0, "Ray3D");
static_assert(offsetof(Ray3D, wvDirection) == 0xc, "Ray3D");
static_assert(sizeof(LineSeg3D) == 0x1c, "LineSeg3D");
static_assert(offsetof(LineSeg3D, wr) == 0x0, "LineSeg3D");
static_assert(offsetof(LineSeg3D, wdLength) == 0x18, "LineSeg3D");
