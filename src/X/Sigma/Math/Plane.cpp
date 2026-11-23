#include "Plane.h"

#include <stddef.h>
#include "../TypeDescriptor/TypeDescriptorField.h"
#include "../TypeDescriptor/TypeDescriptor.h"

static const TypeDescriptorField Plane_Fields[] = {
    TypeDescriptorField("", DT_VECTOR3D, offsetof(Plane, wvNormal), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Plane, wdD), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Plane), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Plane("Plane", 0, Plane_Fields, 3, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PLANE = &TypeDescriptor_Plane;

const Plane cgwplaneNullZPlane = { Vector3D(0.0f, 0.0f, 1.0f), 0.0f };

Vector3D Plane::Reflect(const Vector3D& wp) const {
    float wdDist = ContainsPoint(wp);
    return wp - wvNormal * wdDist * 2.0f;
}
