#include "Quaternion.h"

#include "MathGeneral.h"
#include "Matrix.h"

#include <stddef.h>
#include <stdio.h>
#include <strings.h>
#include "../TypeDescriptor/TypeDescriptorField.h"
#include "../TypeDescriptor/TypeDescriptor.h"

static TDVariant sQuatToVariant(const void * pData);
static BOOL sQuatFromVariant(void * pData, const TDVariant & tValue);
static BOOL sQuatVariantsEqual(const TDVariant & tValue1, const TDVariant & tValue2);

static const TypeDescriptorField Quaternion_Fields[] = {
    TypeDescriptorField("", DT_FLOAT, offsetof(Quaternion, wdReal), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(Quaternion, wvImag), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Quaternion), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Quaternion("Quaternion", 0, Quaternion_Fields, 3, NULL, 0, sQuatToVariant, sQuatFromVariant, sQuatVariantsEqual, NULL); const XTypeDescriptor * DT_QUATERNION = &TypeDescriptor_Quaternion;

static const TypeDescriptorField Quaternion16_Fields[] = {
    TypeDescriptorField("", DT_WORD, offsetof(Quaternion16, x), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(Quaternion16, y), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(Quaternion16, z), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(Quaternion16, w), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Quaternion16), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Quaternion16("Quaternion16", 0, Quaternion16_Fields, 5, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_QUATERNION16 = &TypeDescriptor_Quaternion16;

const Quaternion cgqIdentity(Vector3D(0.0f, 0.0f, 0.0f), 1.0f);

void Quaternion::ToMatrix(Matrix3x3& m) const {
    const float flXSqr = wvImag.x * wvImag.x;
    const float flYSqr = wvImag.y * wvImag.y;
    const float flZSqr = wvImag.z * wvImag.z;
    const float flXY = wvImag.x * wvImag.y;
    const float flXZ = wvImag.x * wvImag.z;
    const float flYZ = wvImag.y * wvImag.z;
    const float flWX = wdReal * wvImag.x;
    const float flWY = wdReal * wvImag.y;
    const float flWZ = wdReal * wvImag.z;

    m._11 = 1.0f - 2.0f * (flYSqr + flZSqr);
    m._12 = 2.0f * (flXY + flWZ);
    m._13 = 2.0f * (flXZ - flWY);

    m._21 = 2.0f * (flXY - flWZ);
    m._22 = 1.0f - 2.0f * (flXSqr + flZSqr);
    m._23 = 2.0f * (flYZ + flWX);

    m._31 = 2.0f * (flXZ + flWY);
    m._32 = 2.0f * (flYZ - flWX);
    m._33 = 1.0f - 2.0f * (flXSqr + flYSqr);
}

static inline float sQuaternionMinimizeRotation(const Quaternion& cq0, const Quaternion& cq1, Quaternion& qTemp) {
    float flDot = Quaternion::DotProduct(cq0, cq1);
    if (flDot < 0.0f) {
        flDot = -flDot;
        qTemp.wvImag = -cq0.wvImag;
        qTemp.wdReal = -cq0.wdReal;
    } else {
        qTemp = cq0;
    }
    return flDot;
}

static inline void sQuaternionLerp(const Quaternion& cq0, const Quaternion& cq1, float flT, Quaternion& qResult) {
    qResult.wvImag = cq0.wvImag + flT * (cq1.wvImag - cq0.wvImag);
    qResult.wdReal = cq0.wdReal + flT * (cq1.wdReal - cq0.wdReal);

    qResult.Normalize();
}

void Quaternion::Lerp(const Quaternion& cq0, const Quaternion& cq1, float flT, Quaternion& qResult) {
    Quaternion qTemp;
    sQuaternionMinimizeRotation(cq0, cq1, qTemp);

    sQuaternionLerp(qTemp, cq1, flT, qResult);
}

void Quaternion::Slerp(const Quaternion& cq0, const Quaternion& cq1, float flT, Quaternion& qResult) {
    Quaternion qTemp;
    float flDot = sQuaternionMinimizeRotation(cq0, cq1, qTemp);

    if (flDot < 0.95f) {
        Angle a = Math::ACos(flDot);
        float flInvSinA = 1.0f / Math::Sin(a);
        float flK0 = Math::Sin((1.0f - flT) * a) * flInvSinA;
        float flK1 = Math::Sin(flT * a) * flInvSinA;

        qResult.wvImag = qTemp.wvImag * flK0 + cq1.wvImag * flK1;
        qResult.wdReal = qTemp.wdReal * flK0 + cq1.wdReal * flK1;
    } else {
        sQuaternionLerp(qTemp, cq1, flT, qResult);
    }
}

Angle Quaternion::ToYaw() const {
    Vector3D wvForward = Rotate(cgwvXAxis);
    wvForward.z = 0.0f;
    wvForward.Normalize();

    return Vector3D::AngleBetween(cgwvXAxis, wvForward);
}

void Quaternion::FromYaw(const Angle& aYaw) {
    SetRotation(cgwvZAxis, aYaw);
}

void Quaternion::FromViewVector(const Vector3D& wvLook, const Vector3D& wvUpWorld,
                                const Vector3D& wvInitCameraForward, const Vector3D& wvInitCameraRight,
                                const Vector3D& wvInitCameraUp) {
    if (Vector3D::CrossProduct(wvLook, wvUpWorld).Magnitude() < EPSILON) {
        if (wvLook.z < 0.0f) {
            Quaternion qOffset;
            Set(cgwvNull, 1.0f);
            qOffset.SetRotation(cgwvYAxis, PI);
            *this = qOffset * *this;
        } else {
            Set(cgwvNull, 1.0f);
        }
        return;
    }

    Vector3D wvForward(wvLook);
    wvForward.Normalize();
    Vector3D wvRight = Vector3D::CrossProduct(wvForward, wvUpWorld);
    wvRight.Normalize();
    Vector3D wvUp = Vector3D::CrossProduct(wvRight, wvForward);

    FromChangeOfBasis(wvInitCameraForward, wvInitCameraRight, wvInitCameraUp, wvForward, wvRight, wvUp);
}

void Quaternion::FromYPR(Angle aYaw, Angle aPitch, Angle aRoll) {
    aYaw = Math::NormalizeAngle(aYaw);
    aPitch = Math::NormalizeAngle(aPitch);
    aRoll = Math::NormalizeAngle(aRoll);

    float aCosYaw = Math::Cos(aYaw * 0.5f);
    float aCosPitch = Math::Cos(aPitch * 0.5f);
    float aCosRoll = Math::Cos(aRoll * 0.5f);

    float aSinYaw = Math::Sin(aYaw * 0.5f);
    float aSinPitch = Math::Sin(aPitch * 0.5f);
    float aSinRoll = Math::Sin(aRoll * 0.5f);

    wdReal = aCosYaw * aCosPitch * aCosRoll - aSinYaw * aSinPitch * aSinRoll;
    wvImag.x = aCosYaw * aCosPitch * aSinRoll + aSinYaw * aSinPitch * aCosRoll;
    wvImag.y = aCosYaw * aSinPitch * aCosRoll + aSinYaw * aCosPitch * aSinRoll;
    wvImag.z = aSinYaw * aCosPitch * aCosRoll - aCosYaw * aSinPitch * aSinRoll;
}

static TDVariant sQuatToVariant(const void * pData)
{
    Quaternion * pQuat = (Quaternion *)pData;
    CRefString sQuat;

    sQuat.Format("%.3f %.3f %.3f %.3f",
                 pQuat->wvImag.x, pQuat->wvImag.y, pQuat->wvImag.z, pQuat->wdReal);

    return TDVariant(sQuat, 0.0f);
}

static BOOL sQuatFromVariant(void * pData, const TDVariant & tValue)
{
    Quaternion * pQuat = (Quaternion *)pData;
    // sic: the original passes the format first and the string to parse second.
    sscanf("%g %g %g %g", tValue.GetStr(),
           &pQuat->wvImag.x, &pQuat->wvImag.y, &pQuat->wvImag.z, &pQuat->wdReal);
    return TRUE;
}

static BOOL sQuatVariantsEqual(const TDVariant & tValue1, const TDVariant & tValue2)
{
    return strcasecmp(tValue1.GetStr(), tValue2.GetStr()) == 0;
}
