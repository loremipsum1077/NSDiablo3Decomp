#pragma once

#include "MathGeneral.h"
#include "Vector.h"

struct Matrix4x4;

struct Matrix3x3 {
    float _11;
    float _12;
    float _13;
    float _21;
    float _22;
    float _23;
    float _31;
    float _32;
    float _33;

    Matrix3x3() {}
    Matrix3x3(const float _11, const float _12, const float _13, const float _21, const float _22, const float _23,
              const float _31, const float _32, const float _33)
        : _11(_11), _12(_12), _13(_13), _21(_21), _22(_22), _23(_23), _31(_31), _32(_32), _33(_33) {}

    Matrix3x3 operator*(const Matrix3x3& m) const;
    Matrix3x3 operator*(const float flScale) const;
    Matrix3x3& operator*=(const float flScale);

    float Determinant() const;
    void Adjoint(Matrix3x3& mResult) const;
    Matrix3x3 Inverse() const;
    static Vector2D Transform2x2(const Vector2D& v, const Matrix3x3& m);
    void SetAltAz(const Angle aAltitude, const Angle aAzimuth);  // Matrix.inl:319
    void SetScale(const float flScale);  // DWARF Matrix.h:84, body Matrix.inl:219
    void SetRotationZ(const Angle ang);  // body Matrix.inl:278
    void SetBasisVectors(const Vector3D& wv0, const Vector3D& wv1, const Vector3D& wv2);
    void SetBasisVectorX(const Vector3D& vBasis);
    void SetBasisVectorY(const Vector3D& vBasis);
    void SetBasisVectorZ(const Vector3D& vBasis);
    void SetChangeBasis(const Vector3D& wvSrcX, const Vector3D& wvSrcY, const Vector3D& wvSrcZ,
                        const Vector3D& wvDestX, const Vector3D& wvDestY, const Vector3D& wvDestZ);
};

struct Matrix4x4 {
    float _11;
    float _12;
    float _13;
    float _14;
    float _21;
    float _22;
    float _23;
    float _24;
    float _31;
    float _32;
    float _33;
    float _34;
    float _41;
    float _42;
    float _43;
    float _44;

    Matrix4x4() {}
    Matrix4x4(const float _11, const float _12, const float _13, const float _14, const float _21, const float _22,
              const float _23, const float _24, const float _31, const float _32, const float _33, const float _34,
              const float _41, const float _42, const float _43, const float _44)
        : _11(_11), _12(_12), _13(_13), _14(_14), _21(_21), _22(_22), _23(_23), _24(_24), _31(_31), _32(_32),
          _33(_33), _34(_34), _41(_41), _42(_42), _43(_43), _44(_44) {}

    Matrix4x4& operator=(const Matrix4x4& m);
    Matrix4x4 operator*(const Matrix4x4& m) const; Matrix4x4& operator*=(const Matrix4x4& m);  // operator*=: DWARF Matrix.h:80
    Matrix4x4 operator*(const float flScale) const;
    Matrix4x4& operator*=(const float flScale);
    static Vector3D Transform3x3(const Vector3D& v, const Matrix4x4& m);

    void Set(const Matrix3x3& mRotation, const Vector3D& wpTranslation);

    float Determinant() const;
    void Adjoint(Matrix4x4& mResult) const;
    Matrix4x4 Inverse() const;
    Vector3D ExtractBasisVectorZ() const; Vector3D ExtractTranslation() const;  // ExtractTranslation: DWARF Matrix.h:103
    Matrix4x4 Transpose() const;
};

Vector4D operator*(const Vector4D& vec, const Matrix4x4& m);

class XTypeDescriptor;

// Defined in Matrix.cpp (lines 27 and 46).
extern const XTypeDescriptor * DT_MATRIX3x3;
extern const XTypeDescriptor * DT_MATRIX4x4;

extern const Matrix3x3 cgm3x3Identity;
extern const Matrix3x3 cgm3x3Null;
extern const Matrix3x3 cgm3x3FlipHandedness;
extern const Matrix4x4 cgm4x4Identity;
extern const Matrix4x4 cgm4x4Null;
extern const Matrix4x4 cgm4x4FlipHandedness;
extern const Matrix4x4 cgm4x4NDCToProjection;
extern const Matrix4x4 cgm4x4ProjectionToNDC;

#include "Matrix.inl"

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(Matrix3x3) == 0x24, "Matrix3x3");
static_assert(offsetof(Matrix3x3, _11) == 0x0, "Matrix3x3");
static_assert(offsetof(Matrix3x3, _12) == 0x4, "Matrix3x3");
static_assert(offsetof(Matrix3x3, _13) == 0x8, "Matrix3x3");
static_assert(offsetof(Matrix3x3, _21) == 0xc, "Matrix3x3");
static_assert(offsetof(Matrix3x3, _22) == 0x10, "Matrix3x3");
static_assert(offsetof(Matrix3x3, _23) == 0x14, "Matrix3x3");
static_assert(offsetof(Matrix3x3, _31) == 0x18, "Matrix3x3");
static_assert(offsetof(Matrix3x3, _32) == 0x1c, "Matrix3x3");
static_assert(offsetof(Matrix3x3, _33) == 0x20, "Matrix3x3");
static_assert(sizeof(Matrix4x4) == 0x40, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _11) == 0x0, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _12) == 0x4, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _13) == 0x8, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _14) == 0xc, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _21) == 0x10, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _22) == 0x14, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _23) == 0x18, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _24) == 0x1c, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _31) == 0x20, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _32) == 0x24, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _33) == 0x28, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _34) == 0x2c, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _41) == 0x30, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _42) == 0x34, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _43) == 0x38, "Matrix4x4");
static_assert(offsetof(Matrix4x4, _44) == 0x3c, "Matrix4x4");
