#include "Ray.h"

#include <stddef.h>
#include "../TypeDescriptor/TypeDescriptorField.h"
#include "../TypeDescriptor/TypeDescriptor.h"

static const TypeDescriptorField Ray3D_Fields[] = {
    TypeDescriptorField("", DT_VECTOR3D, offsetof(Ray3D, wpOrigin), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(Ray3D, wvDirection), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Ray3D), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Ray3D("Ray3D", 0, Ray3D_Fields, 3, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_RAY3D = &TypeDescriptor_Ray3D;

static const TypeDescriptorField LineSeg3D_Fields[] = {
    TypeDescriptorField("", DT_RAY3D, offsetof(LineSeg3D, wr), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(LineSeg3D, wdLength), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LineSeg3D), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LineSeg3D("LineSeg3D", 0, LineSeg3D_Fields, 3, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LINESEG3D = &TypeDescriptor_LineSeg3D;

float LineSeg3D::ClosestPoint(const Vector3D& wp, Vector3D& wpClosestPointOnLine) const {
    const Vector3D& wpLine1 = wr.wpOrigin;
    Vector3D wpLine2 = wpLine1 + wr.wvDirection * wdLength;

    float u = ((wp.x - wpLine1.x) * (wpLine2.x - wpLine1.x) +
               (wp.y - wpLine1.y) * (wpLine2.y - wpLine1.y) +
               (wp.z - wpLine1.z) * (wpLine2.z - wpLine1.z)) /
              Vector3D::DistanceSquared(wpLine1, wpLine2);

    if (u < 0.0f) {
        u = 0.0f;
    } else if (u > 1.0f) {
        u = 1.0f;
    }

    wpClosestPointOnLine = wpLine1 + (wpLine2 - wpLine1) * u;
    return Vector3D::Distance(wpClosestPointOnLine, wp);
}

Vector3D LineSeg3D::GetEndPoint() const {
    Vector3D wvNormalizedDir(wr.wvDirection);
    wvNormalizedDir.Normalize();
    return wr.wpOrigin + wvNormalizedDir * wdLength;
}
