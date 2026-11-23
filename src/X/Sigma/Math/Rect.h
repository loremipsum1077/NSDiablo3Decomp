#pragma once

#include "../SigmaTypes.h"
#include "MathGeneral.h"
#include "Transform.h"
#include "Vector.h"

struct LineSeg3D;
struct Matrix3x3;
struct Plane;
struct Sphere;
struct Rect2D;
struct OBB;

enum {
    CORNER_TL = 0,
    CORNER_TR = 1,
    CORNER_BR = 2,
    CORNER_BL = 3,
    NUM_CORNERS = 4,
};

enum AlignPosition {
    AP_NONE = -1,
    AP_VERTICAL_ABOVE = 0,
    AP_VERTICAL_TOP = 1,
    AP_VERTICAL_CENTER = 2,
    AP_VERTICAL_BELOW = 3,
    AP_VERTICAL_BOTTOM = 4,
    AP_HORIZONTAL_LEFT = 5,
    AP_HORIZONTAL_INSIDE_LEFT = 6,
    AP_HORIZONTAL_CENTER = 7,
    AP_HORIZONTAL_RIGHT = 8,
    AP_HORIZONTAL_INSIDE_RIGHT = 9,
};

struct IRect2D {
    int32 left;
    int32 top;
    int32 right;
    int32 bottom;

    IRect2D() {}
    // The DWARF records Set inlined into this constructor.
    IRect2D(const int32 left, const int32 top, const int32 right, const int32 bottom) {
        Set(left, top, right, bottom);
    }

    void Set(const int32 _left, const int32 _top, const int32 _right, const int32 _bottom);
    uint32 Width() const;
    uint32 Height() const; BOOL IsEmpty() const;  // IsEmpty: DWARF Rect.h:118
    BOOL operator!=(const IRect2D& rect) const;
    BOOL ContainsPoint(const IVector2D & p) const;
    static BOOL Intersect(const IRect2D & rectA, const IRect2D & rectB, IRect2D * prectIntersect);

    void ToUnitCoords(const Rect2D& rectBasis, Rect2D& rectResult) const;
};

struct Rect2D {
    float left;
    float top;
    float right;
    float bottom;

    Rect2D() {}
    // The DWARF records Set inlined into this constructor.
    Rect2D(const float left, const float top, const float right, const float bottom) {
        Set(left, top, right, bottom);
    }

    const Rect2D& operator=(const Rect2D& rectSrc) {
        left = rectSrc.left;
        top = rectSrc.top;
        right = rectSrc.right;
        bottom = rectSrc.bottom;
        return *this;
    }

    void ToPoints(Vector2D* wpCorners, const int32 n90DegreeRotations) const;

    void Set(const float _left, const float _top, const float _right, const float _bottom);
    void Set(const IRect2D& rect);

    static BOOL Intersect(const Rect2D& rectA, const Rect2D& rectB, Rect2D* prectIntersect);

    BOOL ContainsPoint(const float x, const float y) const;
    BOOL ContainsPoint(const Vector2D& wp) const;
    BOOL IsEmpty() const;
    BOOL operator==(const Rect2D& rect) const;
    BOOL operator!=(const Rect2D& rect) const;
    void Offset(const Vector2D& ptOffset, Rect2D& rectDest) const;

    float Width() const;
    float Height() const;
    Vector2D Center() const { return Vector2D((right + left) * 0.5f, (bottom + top) * 0.5f); }  // line 180
    Vector2D Size() const { return Vector2D(Width(), Height()); }  // line 239

    void ToUnitCoords(const Rect2D& rectBasis, Rect2D& rectResult) const;
    void FromUnitCoords(const Rect2D& rectBasis, Rect2D& rectResult) const;
    void FromUnitCoords(const Rect2D& rectBasis, IRect2D& rectResult) const;
    void FromUnitCoords(const IRect2D& rectBasis, IRect2D& rectResult) const;

    void AlignToRect(const Rect2D& rectParent, const AlignPosition eHorizontal, const AlignPosition eVertical,
                     const float flPadding);
    void ConstrainToRect(const Rect2D& rectParent, const float flIndent);
// UniformScale: DWARF Rect.h:293 (in-class definition, DW_INL_inlined). Body lines 295-299 from the copies
// inlined into ReduceWithCone and GetHostileTargetFromList (X/Client/Targeting/ConsoleTargeting.cpp:94, :187):
// right (295), bottom (296), top (298), left (299); flScale - 1 folds to 0.14999998f for 1.15f.
#line 293
    void UniformScale(float flScale)
    {
        right *= flScale;
        bottom *= flScale;

        top -= (top * (flScale - 1.0f));
        left -= (left * (flScale - 1.0f));
    }
#line 106
};

struct AABB {
    Vector3D wp;
    Vector3D wvExt;

    void Set(const Vector3D& wp0, const Vector3D& wp1);
    // Rect.inl:361 (DWARF order).  Body inlined from SmartSceneGroupMarker.cpp:35.
    void Set(const Vector3D wp0, const float wdSize);
    void Set(const OBB& obb);
    BOOL ContainsPoint(const Vector3D& _wp) const;

    // Rect.inl:446 (DWARF order; the Intersect/other Union overloads are not
    // adopted yet).  Body inlined from SmartSceneGroupMarker.cpp:51.
    static void Union(const AABB& aabbA, const AABB& aabbB, AABB& aabbUnion);

    void ComputeBounds(Vector3D& wpMin, Vector3D& wpMax) const;
    void ComputeBounds(Vector3D* wpBounds) const;
    void ComputeEdges(LineSeg3D* wpEdges) const;
    void GetMinimalBoundingSphere(Sphere& ws) const;
    void Inflate(float flAmount);
};

struct OBB {
    PRTransform transform;
    Vector3D wvExt;

    void ComputeBounds(Vector3D* wpBounds) const;
    BOOL ContainsPoint(const Vector3D& wp) const;
    void ComputeEdges(LineSeg3D* wpEdges) const;
    void GetBasisVectors(Vector3D& wvU, Vector3D& wvV, Vector3D& wvW) const;
};

class XTypeDescriptor;

// Defined in Rect.cpp (lines 19 to 31).
extern const XTypeDescriptor * DT_AABB;
extern const XTypeDescriptor * DT_OBB;
extern const IRect2D cgirectNull;
extern const Rect2D cgrectNull;

#include "Rect.inl"

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(IRect2D) == 0x10, "IRect2D");
static_assert(offsetof(IRect2D, left) == 0x0, "IRect2D");
static_assert(offsetof(IRect2D, top) == 0x4, "IRect2D");
static_assert(offsetof(IRect2D, right) == 0x8, "IRect2D");
static_assert(offsetof(IRect2D, bottom) == 0xc, "IRect2D");
static_assert(sizeof(Rect2D) == 0x10, "Rect2D");
static_assert(offsetof(Rect2D, left) == 0x0, "Rect2D");
static_assert(offsetof(Rect2D, top) == 0x4, "Rect2D");
static_assert(offsetof(Rect2D, right) == 0x8, "Rect2D");
static_assert(offsetof(Rect2D, bottom) == 0xc, "Rect2D");
static_assert(sizeof(AABB) == 0x18, "AABB");
static_assert(offsetof(AABB, wp) == 0x0, "AABB");
static_assert(offsetof(AABB, wvExt) == 0xc, "AABB");
static_assert(sizeof(OBB) == 0x28, "OBB");
static_assert(offsetof(OBB, transform) == 0x0, "OBB");
static_assert(offsetof(OBB, wvExt) == 0x1c, "OBB");
