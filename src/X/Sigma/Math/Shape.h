#pragma once

#include "../SigmaTypes.h"
#include "ConvexHull.h"
#include "Cylinder.h"
#include "Rect.h"
#include "Sphere.h"

enum {
    SHAPE_KINEMATIC_BIT = 0,
    SHAPE_NON_SOLID_BIT = 1,
    SHAPE_FENCE_BIT = 2,
    SHAPE_SEE_THRU_BIT = 3,
};

struct Shape {  // size 0x128
    enum Type {
        NONE = -1,
        HULL = 0,
        BOX = 1,
        SPHERE = 2,
        CYLINDER = 3,
        CAPSULE = 4,
        AXIAL_CYLINDER = 5,
    };

    enum Material {
        INVALID_MATERIAL = -1,
        DEFAULT_MATERIAL = 0,
        METAL_MATERIAL = 1,
        WOOD_MATERIAL = 2,
        BONE_MATERIAL = 3,
        FLESH_MATERIAL = 4,
        WATER_MATERIAL = 5,
        STONE_MATERIAL = 6,
        GLASS_MATERIAL = 7,
        PAPER_MATERIAL = 8,
        METAL2_MATERIAL = 9,
        METAL3_MATERIAL = 10,
        WOOD2_MATERIAL = 11,
        WOOD3_MATERIAL = 12,
        STONE2_MATERIAL = 13,
        STONE3_MATERIAL = 14,
        BONE2_MATERIAL = 15,
        BONE3_MATERIAL = 16,
    };

    uint32 dwFlags;  // 0x00
    Shape::Type eType;  // 0x04
    Shape::Material eMaterial;  // 0x08
    int32 nDegrade;  // 0x0c
    float flDensityFactor;  // 0x10
    float flExplosionFactor;  // 0x14
    float flVelocityFactor;  // 0x18
    ConvexHull hull;  // 0x20
    OBB box;  // 0xa0
    Sphere sphere;  // 0xc8
    Cylinder cylinder;  // 0xd8
    AxialCylinder axialCylinder;  // 0xf4
    Capsule capsule;  // 0x108

    static float GetDensity(Shape::Material eMaterial);
};
static_assert(sizeof(Shape) == 0x128, "Shape");

class XTypeDescriptor;

// Defined in Shape.cpp (line 40).
extern const XTypeDescriptor * DT_SHAPE;
