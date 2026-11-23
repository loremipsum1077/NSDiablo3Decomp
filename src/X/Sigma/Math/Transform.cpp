#include "Transform.h"

#include "MathGeneral.h"
#include "Matrix.h"

#include <stddef.h>
#include "../TypeDescriptor/TypeDescriptorField.h"
#include "../TypeDescriptor/TypeDescriptor.h"

static const TypeDescriptorField PRTransform_Fields[] = {
    TypeDescriptorField("", DT_QUATERNION, offsetof(PRTransform, q), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(PRTransform, wp), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PRTransform), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PRTransform("PRTransform", 0, PRTransform_Fields, 3, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PRTRANSFORM = &TypeDescriptor_PRTransform;

static const TypeDescriptorField PRSTransform_Fields[] = {
    TypeDescriptorField("", DT_QUATERNION, offsetof(PRSTransform, q), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(PRSTransform, wp), NULL, 1),
    // The default value is the bit pattern of 1.0f stored in the pointer itself.
    TypeDescriptorField("", DT_FLOAT, offsetof(PRSTransform, flScale), (const void *)0x3f800000, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PRSTransform), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PRSTransform("PRSTransform", 0, PRSTransform_Fields, 4, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PRSTRANSFORM = &TypeDescriptor_PRSTransform;

const PRTransform cgtPRTransformIdentity(Quaternion(Vector3D(0.0f, 0.0f, 0.0f), 1.0f), Vector3D(0.0f, 0.0f, 0.0f));
const PRSTransform cgtPRSTransformIdentity(Quaternion(Vector3D(0.0f, 0.0f, 0.0f), 1.0f), Vector3D(0.0f, 0.0f, 0.0f), 1.0f);
const float MIN_PRS_SCALE = 0.001f;

void PRTransform::Set(const Vector3D& wpLookFrom, const Vector3D& wpLookAt, const Vector3D& wvUp) {
    Vector3D wvLook = wpLookAt - wpLookFrom;

    wp = wpLookFrom;
    q.FromViewVector(wvLook, wvUp, cgwvZAxis, cgwvYAxis, cgwvXAxis);
}

void PRTransform::ToMatrix(Matrix4x4& m) const {
    Matrix3x3 mQuat;
    q.ToMatrix(mQuat);
    m.Set(mQuat, wp);
}

void PRTransform::Lerp(const PRTransform& transform0, const PRTransform& transform1, float flT,
                       PRTransform& transformResult) {
    Quaternion::Lerp(transform0.q, transform1.q, flT, transformResult.q);
    Math::Lerp(transform0.wp, transform1.wp, flT, transformResult.wp);
}

void PRTransform::SLerp(const PRTransform& transform0, const PRTransform& transform1, float flT,
                        PRTransform& transformResult) {
    Quaternion::Slerp(transform0.q, transform1.q, flT, transformResult.q);
    Math::Lerp(transform0.wp, transform1.wp, flT, transformResult.wp);
}

void PRSTransform::ToMatrix(Matrix4x4& m) const {
    const float flXSqr = q.wvImag.x * q.wvImag.x;
    const float flYSqr = q.wvImag.y * q.wvImag.y;
    const float flZSqr = q.wvImag.z * q.wvImag.z;
    const float flXY = q.wvImag.x * q.wvImag.y;
    const float flXZ = q.wvImag.x * q.wvImag.z;
    const float flYZ = q.wvImag.y * q.wvImag.z;
    const float flWX = q.wdReal * q.wvImag.x;
    const float flWY = q.wdReal * q.wvImag.y;
    const float flWZ = q.wdReal * q.wvImag.z;

    float flMult = flScale * 2.0f;

    m._11 = (1.0f - 2.0f * (flYSqr + flZSqr)) * flScale;
    m._12 = flMult * (flXY + flWZ);
    m._13 = flMult * (flXZ - flWY);
    m._14 = 0.0f;

    m._21 = flMult * (flXY - flWZ);
    m._22 = (1.0f - 2.0f * (flXSqr + flZSqr)) * flScale;
    m._23 = flMult * (flYZ + flWX);
    m._24 = 0.0f;

    m._31 = flMult * (flXZ + flWY);
    m._32 = flMult * (flYZ - flWX);
    m._33 = (1.0f - 2.0f * (flXSqr + flYSqr)) * flScale;
    m._34 = 0.0f;

    m._41 = wp.x;
    m._42 = wp.y;
    m._43 = wp.z;
    m._44 = 1.0f;
}

void PRSTransform::Lerp(const PRSTransform& transform0, const PRSTransform& transform1, float flT,
                        PRSTransform& transformResult) {
    Quaternion::Lerp(transform0.q, transform1.q, flT, transformResult.q);
    Math::Lerp(transform0.wp, transform1.wp, flT, transformResult.wp);
    Math::Lerp(transform0.flScale, transform1.flScale, flT, transformResult.flScale);
}
