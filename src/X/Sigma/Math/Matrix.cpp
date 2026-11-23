#include "Matrix.h"

#include <stddef.h>
#include "../TypeDescriptor/TypeDescriptorField.h"
#include "../TypeDescriptor/TypeDescriptor.h"

static const TypeDescriptorField Matrix3x3_Fields[] = {
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix3x3, _11), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix3x3, _12), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix3x3, _13), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix3x3, _21), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix3x3, _22), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix3x3, _23), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix3x3, _31), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix3x3, _32), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix3x3, _33), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Matrix3x3), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Matrix3x3("Matrix3x3", 0, Matrix3x3_Fields, 10, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_MATRIX3x3 = &TypeDescriptor_Matrix3x3;

static const TypeDescriptorField Matrix4x4_Fields[] = {
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _11), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _12), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _13), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _14), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _21), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _22), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _23), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _24), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _31), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _32), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _33), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _34), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _41), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _42), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _43), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Matrix4x4, _44), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Matrix4x4), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Matrix4x4("Matrix4x4", 0, Matrix4x4_Fields, 17, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_MATRIX4x4 = &TypeDescriptor_Matrix4x4;

const Matrix3x3 cgm3x3Identity(1.0f, 0.0f, 0.0f,
                               0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 1.0f);

const Matrix3x3 cgm3x3Null(0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f);

const Matrix3x3 cgm3x3FlipHandedness(1.0f, 0.0f, 0.0f,
                                     0.0f, -1.0f, 0.0f,
                                     0.0f, 0.0f, 1.0f);

const Matrix4x4 cgm4x4Identity(1.0f, 0.0f, 0.0f, 0.0f,
                               0.0f, 1.0f, 0.0f, 0.0f,
                               0.0f, 0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);

const Matrix4x4 cgm4x4Null(0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f);

const Matrix4x4 cgm4x4FlipHandedness(1.0f, 0.0f, 0.0f, 0.0f,
                                     0.0f, 1.0f, 0.0f, 0.0f,
                                     0.0f, 0.0f, -1.0f, 0.0f,
                                     0.0f, 0.0f, 0.0f, 1.0f);

const Matrix4x4 cgm4x4NDCToProjection(2.0f, 0.0f, 0.0f, 0.0f,
                                      0.0f, -2.0f, 0.0f, 0.0f,
                                      0.0f, 0.0f, 1.0f, 0.0f,
                                      -1.0f, 1.0f, 0.0f, 1.0f);

const Matrix4x4 cgm4x4ProjectionToNDC(0.5f, 0.0f, 0.0f, 0.0f,
                                      0.0f, -0.5f, 0.0f, 0.0f,
                                      0.0f, 0.0f, 1.0f, 0.0f,
                                      0.5f, 0.5f, 0.0f, 1.0f);

static inline float sDeterminant3x3(float _11, float _12, float _13, float _21, float _22, float _23, float _31,
                                    float _32, float _33) {
    return _11 * _22 * _33 - _11 * _23 * _32 + _12 * _23 * _31 - _12 * _21 * _33 + _13 * _21 * _32 -
           _13 * _22 * _31;
}

float Matrix4x4::Determinant() const {
    return _11 * sDeterminant3x3(_22, _23, _24, _32, _33, _34, _42, _43, _44) -
           _12 * sDeterminant3x3(_21, _23, _24, _31, _33, _34, _41, _43, _44) +
           _13 * sDeterminant3x3(_21, _22, _24, _31, _32, _34, _41, _42, _44) -
           _14 * sDeterminant3x3(_21, _22, _23, _31, _32, _33, _41, _42, _43);
}

void Matrix4x4::Adjoint(Matrix4x4& mResult) const {
    mResult._11 = sDeterminant3x3(_22, _23, _24, _32, _33, _34, _42, _43, _44);
    mResult._12 = -sDeterminant3x3(_12, _13, _14, _32, _33, _34, _42, _43, _44);
    mResult._13 = sDeterminant3x3(_12, _13, _14, _22, _23, _24, _42, _43, _44);
    mResult._14 = -sDeterminant3x3(_12, _13, _14, _22, _23, _24, _32, _33, _34);

    mResult._21 = -sDeterminant3x3(_21, _23, _24, _31, _33, _34, _41, _43, _44);
    mResult._22 = sDeterminant3x3(_11, _13, _14, _31, _33, _34, _41, _43, _44);
    mResult._23 = -sDeterminant3x3(_11, _13, _14, _21, _23, _24, _41, _43, _44);
    mResult._24 = sDeterminant3x3(_11, _13, _14, _21, _23, _24, _31, _33, _34);

    mResult._31 = sDeterminant3x3(_21, _22, _24, _31, _32, _34, _41, _42, _44);
    mResult._32 = -sDeterminant3x3(_11, _12, _14, _31, _32, _34, _41, _42, _44);
    mResult._33 = sDeterminant3x3(_11, _12, _14, _21, _22, _24, _41, _42, _44);
    mResult._34 = -sDeterminant3x3(_11, _12, _14, _21, _22, _24, _31, _32, _34);

    mResult._41 = -sDeterminant3x3(_21, _22, _23, _31, _32, _33, _41, _42, _43);
    mResult._42 = sDeterminant3x3(_11, _12, _13, _31, _32, _33, _41, _42, _43);
    mResult._43 = -sDeterminant3x3(_11, _12, _13, _21, _22, _23, _41, _42, _43);
    mResult._44 = sDeterminant3x3(_11, _12, _13, _21, _22, _23, _31, _32, _33);
}
