#include "Sphere.h"

#include <stddef.h>
#include "../TypeDescriptor/TypeDescriptorField.h"
#include "../TypeDescriptor/TypeDescriptor.h"

static const TypeDescriptorField Circle_Fields[] = {
    TypeDescriptorField("", DT_VECTOR2D, offsetof(Circle, wpCenter), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Circle, wdRadius), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Circle), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Circle("Circle", 0, Circle_Fields, 3, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CIRCLE = &TypeDescriptor_Circle;

static const TypeDescriptorField Sphere_Fields[] = {
    TypeDescriptorField("", DT_VECTOR3D, offsetof(Sphere, wpCenter), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Sphere, wdRadius), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Sphere), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Sphere("Sphere", 0, Sphere_Fields, 3, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SPHERE = &TypeDescriptor_Sphere;
