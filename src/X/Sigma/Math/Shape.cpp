#include "Shape.h"

#include <stddef.h>
#include "../TypeDescriptor/TypeDescriptorField.h"
#include "../TypeDescriptor/TypeDescriptor.h"

// axialCylinder is not described. The float defaults are the bit pattern of 1.0f stored in the
// pointer itself.
static const TypeDescriptorField Shape_Fields[] = {
    TypeDescriptorField("", DT_INT, offsetof(Shape, dwFlags), NULL, (1 << TDF_XML_SERIALIZATION) | (1 << TDF_BIT_FLAGS)),
    TypeDescriptorField("", DT_INT, offsetof(Shape, eType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(Shape, eMaterial), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(Shape, nDegrade), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Shape, flDensityFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Shape, flExplosionFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Shape, flVelocityFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_CONVEXHULL, offsetof(Shape, hull), NULL, 1),
    TypeDescriptorField("", DT_OBB, offsetof(Shape, box), NULL, 1),
    TypeDescriptorField("", DT_SPHERE, offsetof(Shape, sphere), NULL, 1),
    TypeDescriptorField("", DT_CYLINDER, offsetof(Shape, cylinder), NULL, 1),
    TypeDescriptorField("", DT_CAPSULE, offsetof(Shape, capsule), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Shape), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Shape("Shape", 1, Shape_Fields, 13, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SHAPE = &TypeDescriptor_Shape;

float Shape::GetDensity(Shape::Material eMaterial) {
    switch (eMaterial) {
    case METAL_MATERIAL:
    case METAL2_MATERIAL:
    case METAL3_MATERIAL:
        return 3.75f;
    case WOOD_MATERIAL:
    case WOOD2_MATERIAL:
    case WOOD3_MATERIAL:
    case PAPER_MATERIAL:
        return 1.5625f;
    case STONE_MATERIAL:
    case STONE2_MATERIAL:
    case STONE3_MATERIAL:
    case GLASS_MATERIAL:
        return 2.5f;
    default:
        return 1.9375f;
    }
}
