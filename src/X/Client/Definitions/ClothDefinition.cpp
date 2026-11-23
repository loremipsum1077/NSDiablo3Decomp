// Original file: X/Client/Definitions/ClothDefinition.cpp

#include "ClothDefinition.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer; it reaches them through include chains the DWARF does not record.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

extern const XTypeDescriptor * DT_SNO;  // D3DescribedType.cpp

enum {
    CLOTH_SNO_VERSION = 51,
    CLOTH_XML_VERSION = 5,
};

// Float defaults are passed as their bit patterns, as the binary has them.








static const TypeDescriptorField ClothDefinition_Fields[18] = {
    TypeDescriptorField("", DT_INT, offsetof(ClothDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(ClothDefinition, nPreStepCount), (const void *)25, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ClothDefinition, flDensity), (const void *)0x3f000000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ClothDefinition, flVelocityFromParentFraction), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ClothDefinition, flStretchingStiffness), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ClothDefinition, flHorizontalStiffness), (const void *)0x3f000000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ClothDefinition, flBendingStiffness), (const void *)0x3dcccccd, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ClothDefinition, flExplosionFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_ACCEL, offsetof(ClothDefinition, flAirBuoyancy), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ClothDefinition, flStabilityFactor), (const void *)0x3fc00000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ClothDefinition, flDamping), (const void *)0x3dcccccd, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ClothDefinition, flFriction), (const void *)0x3dcccccd, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(ClothDefinition, anPlanes), NULL, 1, 4, 0),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ClothDefinition, snoAmbientSound), "", AMBIENTSOUND_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(ClothDefinition, eWindType), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ClothDefinition, flWindFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ClothDefinition, wvLocalWind), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(ClothDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ClothDefinition("Cloth", CLOTH_XML_VERSION, ClothDefinition_Fields, 18, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CLOTH = &TypeDescriptor_ClothDefinition;




ClothGroupDef * gptGroupDefCloth;





ClothGroupDef::ClothGroupDef()
    : GroupDefinition("Cloth", CLOTH_SNO,
                      SNOVersions(CLOTH_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(ClothDefinition), DT_CLOTH, 0x2005, 8)







{
}




uint32 ClothGroupDef::GetMaxCount() {
    return 64;
}









void ClothGroupDef::GetDefaultData(void * ptDefaultDef) const
{
{
    SigmaMemoryClear(ptDefaultDef, sizeof(ClothDefinition));
    ClothDefinition * pdef = (ClothDefinition *)ptDefaultDef;
    pdef->dwFlags = 0;
    pdef->nPreStepCount = 25;
    for (int32 i = 0; i < 4; i++) pdef->anPlanes[i] = CLOTH_PLANE_NONE;
    pdef->flDensity = 0.5f;
    pdef->flVelocityFromParentFraction = 1.0f;
    pdef->flStretchingStiffness = 1.0f;
    pdef->flHorizontalStiffness = 0.5f;
    pdef->flBendingStiffness = 0.1f;
    pdef->flExplosionFactor = 1.0f;

    pdef->flWindFactor = 1.0f;
    pdef->flAirBuoyancy = 0.0f;
    pdef->flStabilityFactor = 1.5f;
    pdef->flDamping = 0.1f;
    pdef->flFriction = 0.1f;




    pdef->snoAmbientSound = SNO_NONE;
}
}
