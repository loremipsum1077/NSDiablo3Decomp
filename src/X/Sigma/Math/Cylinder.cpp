#include "Cylinder.h"

#include "Transform.h"

#include <stddef.h>
#include "../TypeDescriptor/TypeDescriptorField.h"
#include "../TypeDescriptor/TypeDescriptor.h"

static const TypeDescriptorField AxialCylinder_Fields[] = {
    TypeDescriptorField("", DT_VECTOR3D, offsetof(AxialCylinder, wpBase), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AxialCylinder, wdHeight), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AxialCylinder, wdRadius), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AxialCylinder), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AxialCylinder("AxialCylinder", 0, AxialCylinder_Fields, 4, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_AXIALCYLINDER = &TypeDescriptor_AxialCylinder;

static const TypeDescriptorField Cylinder_Fields[] = {
    TypeDescriptorField("", DT_VECTOR3D, offsetof(Cylinder, wp1), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(Cylinder, wp2), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Cylinder, flRadius), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Cylinder), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Cylinder("Cylinder", 0, Cylinder_Fields, 4, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CYLINDER = &TypeDescriptor_Cylinder;

static const TypeDescriptorField Capsule_Fields[] = {
    TypeDescriptorField("", DT_VECTOR3D, offsetof(Capsule, wp1), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(Capsule, wp2), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Capsule, flRadius), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Capsule), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Capsule("Capsule", 0, Capsule_Fields, 4, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CAPSULE = &TypeDescriptor_Capsule;

void AxialCylinder::Set(const Vector3D& _wpBase, const float _wdHeight, const float _wdRadius) {
    wpBase = _wpBase;
    wdHeight = _wdHeight;
    wdRadius = _wdRadius;
}

void Capsule::Set(const PRTransform& transform, const float _flRadius, const float flHalfAxialLength) {
    Vector3D vecAxis = transform.Rotate(cgwvXAxis);
    Vector3D vecAxialOffset = vecAxis * flHalfAxialLength;
    wp1 = transform.wp - vecAxialOffset;
    wp2 = transform.wp + vecAxialOffset;
    flRadius = _flRadius;
}

Vector3D Capsule::PushToSurface(const Vector3D& wp) const {
    Vector3D u = wp2 - wp1;

    if (u.MagnitudeSquared() < EPSILON * EPSILON) {
        float flDistanceSquared = Vector3D::DistanceSquared(wp1, wp);
        if (flDistanceSquared < flRadius * flRadius) {
            Vector3D wv = wp - wp1;
            wv.Normalize();
            return wp1 + flRadius * wv;
        }
        return wp;
    }

    u.Normalize();

    float s1 = Vector3D::DotProduct(u, wp - wp1);
    float s2 = Vector3D::DotProduct(u, wp - wp2);

    if (s1 < EPSILON) {
        float flDistanceSquared = Vector3D::DistanceSquared(wp1, wp);
        if (flDistanceSquared < flRadius * flRadius) {
            Vector3D wv = wp - wp1;
            wv.Normalize();
            return wp1 + flRadius * wv;
        }
        return wp;
    }

    if (s2 > -EPSILON) {
        float flDistanceSquared = Vector3D::DistanceSquared(wp2, wp);
        if (flDistanceSquared < flRadius * flRadius) {
            Vector3D wv = wp - wp2;
            wv.Normalize();
            return wp2 + flRadius * wv;
        }
        return wp;
    }

    {
        Vector3D d = wp1 + s1 * u;
        float flDistanceSquared = Vector3D::DistanceSquared(d, wp);
        if (flDistanceSquared < flRadius * flRadius) {
            Vector3D r = wp - d;
            r.Normalize();
            return d + flRadius * r;
        }
    }

    return wp;
}
