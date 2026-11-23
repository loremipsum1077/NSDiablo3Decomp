#pragma once

#include "../SigmaTypes.h"
#include "MathGeneral.h"
#include "Vector.h"

struct Matrix3x3;
struct Matrix4x4;

struct Quaternion {
    Vector3D wvImag;
    float wdReal;

    Quaternion() {}
    Quaternion(const Vector3D& wvImag, const float wdReal) : wvImag(wvImag), wdReal(wdReal) {}

    // operator=: implicit (DWARF Quaternion.h:19 artificial, rows all at 19:8 = struct name column).





    BOOL operator==(const Quaternion& q) const;
    Quaternion operator+(const Quaternion& q) const;
    Quaternion operator-(const Quaternion& q) const;
    Quaternion& operator+=(const Quaternion& q);
    Quaternion operator*(const Quaternion& q) const;
    Quaternion& operator*=(const Quaternion& q);

    Vector3D Rotate(const Vector3D& p) const;
    Vector2D Rotate(const Vector2D& p) const;

    void Set(const Vector3D& cwvImag, const float cwdReal);
    void FromViewVector(const Vector3D& wvLook, const Vector3D& wvUpWorld, const Vector3D& wvInitCameraForward,
                        const Vector3D& wvInitCameraRight, const Vector3D& wvInitCameraUp);

    static float DotProduct(const Quaternion& q, const Quaternion& r); static Vector3D AngularVelocity(const Quaternion& q1In, const Quaternion& q2In, float flInvDt);  // AngularVelocity: DWARF Quaternion.inl:216 (static: no this)

    void Normalize();
    void SetRotation(const Vector3D& cwvAxis, const Angle caRot);
    Quaternion Conjugate() const;

    void ToMatrix(Matrix3x3& m) const;
    void FromMatrix(const Matrix3x3& m); void FromBasisVectors(const Vector3D& wv0, const Vector3D& wv1, const Vector3D& wv2);  // FromBasisVectors: DWARF Quaternion.h:42

    static void Lerp(const Quaternion& cq0, const Quaternion& cq1, float flT, Quaternion& qResult);
    static void Slerp(const Quaternion& cq0, const Quaternion& cq1, float flT, Quaternion& qResult);

    void FromArc(const Vector3D& wv0, const Vector3D& wv1);

    void FromChangeOfBasis(const Vector3D& wvSrcX, const Vector3D& wvSrcY, const Vector3D& wvSrcZ,
                           const Vector3D& wvDestX, const Vector3D& wvDestY, const Vector3D& wvDestZ);
    void FromChangeOfBasis(const Vector3D& wvSrc, const Vector3D& wvDest);

    Angle ToYaw() const;
    void FromYaw(const Angle& aYaw);
    void FromYPR(Angle aYaw, Angle aPitch, Angle aRoll);
};

struct Quaternion16 {
    short x;
    short y;
    short z;
    short w; Quaternion Decompress() const;  // Decompress: DWARF Quaternion.inl:448
};

extern const Quaternion cgqIdentity;

class XTypeDescriptor;

// Defined in Quaternion.cpp (lines 23 and 30).
extern const XTypeDescriptor * DT_QUATERNION;
extern const XTypeDescriptor * DT_QUATERNION16;

#include "Quaternion.inl"

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(Quaternion) == 0x10, "Quaternion");
static_assert(offsetof(Quaternion, wvImag) == 0x0, "Quaternion");
static_assert(offsetof(Quaternion, wdReal) == 0xc, "Quaternion");
static_assert(sizeof(Quaternion16) == 0x8, "Quaternion16");
static_assert(offsetof(Quaternion16, x) == 0x0, "Quaternion16");
static_assert(offsetof(Quaternion16, y) == 0x2, "Quaternion16");
static_assert(offsetof(Quaternion16, z) == 0x4, "Quaternion16");
static_assert(offsetof(Quaternion16, w) == 0x6, "Quaternion16");
