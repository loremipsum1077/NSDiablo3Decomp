#include "Vector.h"

#include "MathGeneral.h"
#include "Quaternion.h"

// The NX SDK's <math.h> classifies a float by testing its exponent bits (__FLOAT_BITS), where the
// build container's libc expands isfinite() to a compiler builtin. Mirror the SDK definition here
// until the SDK headers are available to the build.
static inline unsigned int __FLOAT_BITS(float __f) {
    union {
        float __f;
        unsigned int __i;
    } __u;
    __u.__f = __f;
    return __u.__i;
}
#undef isfinite
#define isfinite(x) ((__FLOAT_BITS(x) & 0x7fffffff) < 0x7f800000)

const Vector3D cgwpNull(0.0f, 0.0f, 0.0f);
const IVector2D cgptNull(0, 0);

const Vector2D cgptNull2d(0.0f, 0.0f);

const Vector3D cgwvXAxis(1.0f, 0.0f, 0.0f);
const Vector3D cgwvYAxis(0.0f, 1.0f, 0.0f);
const Vector3D cgwvZAxis(0.0f, 0.0f, 1.0f);
const Vector3D cgwvNull(0.0f, 0.0f, 0.0f);
const Vector3D cgwvOne(1.0f, 1.0f, 1.0f);

void Vector3D::Bias(const Angle aAltitude, const Angle aAzimuth) {
    Vector3D wvAzAxis(*this);
    wvAzAxis.Normalize();

    Vector3D wvAltAxis;
    {
        Vector3D wvPerpAxis;
        if (y != 0.0f || z != 0.0f) {
            wvPerpAxis = Vector3D(1.0f, 0.0f, 0.0f);
        } else {
            wvPerpAxis = Vector3D(0.0f, 1.0f, 0.0f);
        }
        wvAltAxis = CrossProduct(wvAzAxis, wvPerpAxis);
        wvAltAxis.Normalize();
    }

    Quaternion qRot1;
    Quaternion qRot2;
    qRot1.SetRotation(wvAltAxis, aAltitude);
    qRot2.SetRotation(wvAzAxis, aAzimuth);
    Quaternion q = qRot2 * qRot1;

    *this = q.Rotate(*this);
}

Angle Vector3D::AngleBetween(const Vector3D& wv0, const Vector3D& wv1) {
    float wdMag0 = wv0.Magnitude();
    float wdMag1 = wv1.Magnitude();

    if (wdMag0 < EPSILON || !isfinite(wdMag0)) {
        return 0.0f;
    }

    if (wdMag1 < EPSILON || !isfinite(wdMag1)) {
        return 0.0f;
    }

    float wdDotProduct = DotProduct(wv0, wv1);

    float flCosAng = Math::Pin(wdDotProduct / (wdMag0 * wdMag1), -1.0f, 1.0f);
    float k = CrossProduct(wv0, wv1).z;
    Angle aAngle = Math::ACos(Math::Pin(Math::Abs(flCosAng), 0.0f, 1.0f));

    Angle aToOrientation;
    if (flCosAng > 0.0f) {
        if (k < 0.0f) {
            aToOrientation = FULL_CIRCLE - aAngle;
        } else {
            aToOrientation = aAngle;
        }
    } else {
        if (k < 0.0f) {
            aToOrientation = HALF_CIRCLE + aAngle;
        } else {
            aToOrientation = Math::AngularDifference(HALF_CIRCLE, aAngle);
        }
    }

    return Math::NormalizeAngle(aToOrientation);
}

void Vector3D::Rotate(const Vector3D& wvAxis, const Angle angle, Vector3D& wv) {
    Quaternion qRotation;
    qRotation.SetRotation(wvAxis, angle);
    wv = qRotation.Rotate(wv);
}

Angle Vector2D::AngleBetween(const Vector2D& wv0, const Vector2D& wv1) {
    float wdMag0 = wv0.Magnitude();
    float wdMag1 = wv1.Magnitude();

    if (wdMag0 < EPSILON) {
        return 0.0f;
    }

    if (wdMag1 < EPSILON) {
        return 0.0f;
    }

    float wdDotProduct = DotProduct(wv0, wv1);

    float flCosAng = Math::Pin(wdDotProduct / (wdMag0 * wdMag1), -1.0f, 1.0f);
    float k = CrossProduct(wv0, wv1);
    Angle aAngle = Math::ACos(Math::Abs(flCosAng));

    Angle aToOrientation;
    if (flCosAng > 0.0f) {
        if (k < 0.0f) {
            aToOrientation = FULL_CIRCLE - aAngle;
        } else {
            aToOrientation = aAngle;
        }
    } else {
        if (k < 0.0f) {
            aToOrientation = HALF_CIRCLE + aAngle;
        } else {
            aToOrientation = Math::AngularDifference(HALF_CIRCLE, aAngle);
        }
    }

    return Math::NormalizeAngle(aToOrientation);
}
