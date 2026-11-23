#include "Triangle.h"

#include "MathGeneral.h"

#include <stddef.h>
#include "../TypeDescriptor/TypeDescriptorField.h"
#include "../TypeDescriptor/TypeDescriptor.h"

static const TypeDescriptorField Triangle3D_Fields[] = {
    TypeDescriptorField("", DT_VECTOR3D, offsetof(Triangle3D, wp0), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(Triangle3D, wp1), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(Triangle3D, wp2), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Triangle3D), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Triangle3D("Triangle3D", 0, Triangle3D_Fields, 4, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_TRIANGLE3D = &TypeDescriptor_Triangle3D;

const float TRIANGLE_INTERSECT_EPSILON = 0.0001f;
const float TRIANGLE_INTERSECT_EPSILON_FACTOR = 1.0001f;

float Triangle3D::Area() const {
    Vector3D wvEdge1 = wp1 - wp0;
    Vector3D wvEdge2 = wp2 - wp0;

    float wdArea = Vector3D::CrossProduct(wvEdge1, wvEdge2).Magnitude() * 0.5f;
    return wdArea;
}

void Triangle3D::BarycentricFromPoint(const Vector3D& wp, float& flU, float& flV, float& flW) const {
    const Vector3D& v0 = wp0;
    const Vector3D& v1 = wp1;
    const Vector3D& v2 = wp2;

    Vector3D vPerp = Vector3D::CrossProduct(v1 - v0, v2 - v0);
    float flPerpMag = vPerp.Magnitude();
    if (flPerpMag < EPSILON) {
        flU = 1.0f;
        flV = flW = 0.0f;
        return;
    }

    float flInvPerpMag = 1.0f / flPerpMag;

    flU = Vector3D::CrossProduct(v1 - wp, v2 - wp).Magnitude() * flInvPerpMag;
    flV = Vector3D::CrossProduct(v2 - wp, v0 - wp).Magnitude() * flInvPerpMag;

    flU = Math::Max(flU, 0.0f);
    flV = Math::Max(flV, 0.0f);
    flW = Math::Max(1.0f - flU - flV, 0.0f);

    if (!(Math::Abs(flU + flV + flW) > 0.97f && Math::Abs(flU + flV + flW) < 1.03f)) {
        flU = flV = flW = 0.33f;
    }
}
