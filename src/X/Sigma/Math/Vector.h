#pragma once

#include "../SigmaTypes.h"
#include "MathGeneral.h"

struct IVector2D {
    int32 x;
    int32 y;

    IVector2D() {}
    IVector2D(const int32 x, const int32 y) : x(x), y(y) {}

    BOOL operator==(const IVector2D& wpSrc) const;
    BOOL operator!=(const IVector2D& wpSrc) const;
    IVector2D operator+(const IVector2D& l) const;
    IVector2D operator-(const IVector2D& l) const;

    void Set(const int32 _x, const int32 _y);
};

struct Vector2D {
    float x;
    float y;

    Vector2D() {}
    Vector2D(const float x, const float y) : x(x), y(y) {}

    BOOL operator==(const Vector2D& wpSrc) const;
    BOOL operator!=(const Vector2D& wpSrc) const;
    Vector2D& operator=(const Vector2D& wpSrc);
    Vector2D operator+(const Vector2D& l) const;
    Vector2D& operator+=(const Vector2D& l);
    Vector2D operator-(const Vector2D& l) const;
    Vector2D& operator-=(const Vector2D& l);
    Vector2D& operator*=(const float flScale);
    Vector2D operator-() const;
    Angle ToYaw() const;

    const float* AsFloatArray() const;

    void Set(const float _x, const float _y);

    // The body is not in the DWARF; written from the copies inlined into
    // X/Client/UserInterface/Interface/UIToolTipManager.cpp (sAlignTipRectToRect).
    void SnapToIntegerCoords() {  // line 149
        x = Math::FloatTrunc(x);
        y = Math::FloatTrunc(y);
    }

    float Magnitude() const;
    float MagnitudeSquared() const;
    float Normalize();

    static float DistanceSquared(const Vector2D& wp0, const Vector2D& wp1);
    static float Distance(const Vector2D& wp0, const Vector2D& wp1);
    static float DotProduct(const Vector2D& wvA, const Vector2D& wvB);
    static float CrossProduct(const Vector2D& wvA, const Vector2D& wvB);
    static Angle AngleBetween(const Vector2D& wv0, const Vector2D& wv1);
    void Rotate(Angle angle);

};

struct Vector3D {
    float x;
    float y;
    float z;

    Vector3D() {}
    Vector3D(const Vector3D& wpSrc) : x(wpSrc.x), y(wpSrc.y), z(wpSrc.z) {}
    Vector3D(const float x, const float y, const float z) : x(x), y(y), z(z) {}

    Vector3D& operator=(const Vector3D& wpSrc);
    Vector2D& As2D() const { return *(Vector2D*)this; }
    BOOL operator==(const Vector3D& wpSrc) const;
    BOOL operator!=(const Vector3D& wpSrc) const;
    Vector3D operator+(const Vector3D& l) const;
    Vector3D& operator+=(const Vector3D& l);
    Vector3D operator-(const Vector3D& l) const;
    Vector3D& operator-=(const Vector3D& l);
    Vector3D& operator*=(const float flScale);
    Vector3D operator-() const;

    void Set(const float _x, const float _y, const float _z);
    void Bias(const Angle aAltitude, const Angle aAzimuth);

    static float DistanceSquared(const Vector3D& wp0, const Vector3D& wp1);
    static float Distance(const Vector3D& wp0, const Vector3D& wp1);
    static float Distance2DSquared(const Vector3D& wp0, const Vector3D& wp1);
    static float Distance2D(const Vector3D& wp0, const Vector3D& wp1);

    float Magnitude() const;
    float MagnitudeSquared() const;
    float Normalize();
    BOOL IsNormalized() const;  // Vector.h:274

    static float DotProduct(const Vector3D& wvA, const Vector3D& wvB);
    static Vector3D CrossProduct(const Vector3D& wvA, const Vector3D& wvB);
    static Angle AngleBetween(const Vector3D& wv0, const Vector3D& wv1);
    static void Rotate(const Vector3D& wvAxis, const Angle angle, Vector3D& wv);
    static Vector3D Min(const Vector3D& wvA, const Vector3D& wvB);
    static Vector3D Max(const Vector3D& wvA, const Vector3D& wvB);
    static Vector3D Abs(const Vector3D& wv);
    static Vector3D Reflect(const Vector3D& wv, const Vector3D& wvNormal);
    static Vector3D Projection(const Vector3D& wv, const Vector3D& wvProjectionAxis);
};

struct Vector4D {
    float x;
    float y;
    float z;
    float w;

    Vector4D() {}
    Vector4D(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) {}

    Vector4D operator-() const;

    void Set(const float _x, const float _y, const float _z, const float _w);
};

Vector2D operator*(const Vector2D& vec, const float flScale);
Vector2D operator*(const float flScale, const Vector2D& vec);
Vector3D operator*(const Vector3D& vec, const float flScale);
Vector3D operator*(const float flScale, const Vector3D& vec);

extern const Vector3D cgwpNull;
extern const IVector2D cgptNull;
extern const Vector2D cgptNull2d;
extern const Vector3D cgwvXAxis;
extern const Vector3D cgwvYAxis;
extern const Vector3D cgwvZAxis;
extern const Vector3D cgwvNull;
extern const Vector3D cgwvOne;

#include "Vector.inl"

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(IVector2D) == 0x8, "IVector2D");
static_assert(offsetof(IVector2D, x) == 0x0, "IVector2D");
static_assert(offsetof(IVector2D, y) == 0x4, "IVector2D");
static_assert(sizeof(Vector2D) == 0x8, "Vector2D");
static_assert(offsetof(Vector2D, x) == 0x0, "Vector2D");
static_assert(offsetof(Vector2D, y) == 0x4, "Vector2D");
static_assert(sizeof(Vector3D) == 0xc, "Vector3D");
static_assert(offsetof(Vector3D, x) == 0x0, "Vector3D");
static_assert(offsetof(Vector3D, y) == 0x4, "Vector3D");
static_assert(offsetof(Vector3D, z) == 0x8, "Vector3D");
static_assert(sizeof(Vector4D) == 0x10, "Vector4D");
static_assert(offsetof(Vector4D, x) == 0x0, "Vector4D");
static_assert(offsetof(Vector4D, y) == 0x4, "Vector4D");
static_assert(offsetof(Vector4D, z) == 0x8, "Vector4D");
static_assert(offsetof(Vector4D, w) == 0xc, "Vector4D");

// DWARF Vector.h:159 (DIE 0x79dc36; only Particle.cpp names it). Vector.h is not on DWARF lines; appended at the end
// so no existing line moves. Members 161-163, ctors 165/166, operator= 168, ==/!= 171/174, Set 177. The ctor bodies
// are inferred from the sibling IVector2D (empty default ctor, member-wise init list); the other members have no body
// evidence (nothing in the binary uses them) and are declared only.
struct IVector3D {
    int32 x;
    int32 y;
    int32 z;

    IVector3D() {}
    IVector3D(const int32 x, const int32 y, const int32 z) : x(x), y(y), z(z) {}

    IVector3D& operator=(const IVector3D& wpSrc);

    BOOL operator==(const IVector3D& wpSrc) const;
    BOOL operator!=(const IVector3D& wpSrc) const;

    void Set(const int32 _x, const int32 _y, const int32 _z);
};
static_assert(sizeof(IVector3D) == 0xc, "IVector3D");
static_assert(offsetof(IVector3D, x) == 0x0, "IVector3D");
static_assert(offsetof(IVector3D, y) == 0x4, "IVector3D");
static_assert(offsetof(IVector3D, z) == 0x8, "IVector3D");
