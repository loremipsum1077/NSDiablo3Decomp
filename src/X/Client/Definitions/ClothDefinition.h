// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/ClothDefinition.h
// Subset: the group definition class and its global, which X/Common/Game.cpp registers
// (GroupDefinition::Create<ClothGroupDef>). The definition struct itself is not adopted yet.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"
#include "../../../X/Sigma/Math/Vector.h"

class XMemoryAllocatorInterface;

enum {
    CLOTH_PLANE_NONE = 0,
    CLOTH_PLANE_1 = 1,
    CLOTH_PLANE_2 = 2,
    CLOTH_PLANE_3 = 3,
    CLOTH_PLANE_4 = 4,
    CLOTH_PLANE_5 = 5,
    CLOTH_PLANE_6 = 6,
    CLOTH_PLANE_7 = 7,
    CLOTH_PLANE_8 = 8,
    CLOTH_MAX_PLANE_NAMES = 9,
};

enum {
    CLOTHDEF_UNUSED_BIT = 0,
    CLOTHDEF_COLLIDE_WITH_SPHERES_BIT = 1,
    CLOTHDEF_COLLIDE_WITH_LOCAL_SPHERES_BIT = 2,
};

enum ClothWindType {
    CLOTH_WIND_TYPE_GLOBAL = 0,
    CLOTH_WIND_TYPE_LOCAL = 1,
};

struct ClothDefinition {  // size 0x64
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    int32 nPreStepCount;  // 0x10
    float flDensity;  // 0x14
    float flVelocityFromParentFraction;  // 0x18
    float flStretchingStiffness;  // 0x1c
    float flHorizontalStiffness;  // 0x20
    float flBendingStiffness;  // 0x24
    float flExplosionFactor;  // 0x28
    float flWindFactor;  // 0x2c
    float flAirBuoyancy;  // 0x30
    float flStabilityFactor;  // 0x34
    float flDamping;  // 0x38
    float flFriction;  // 0x3c
    int32 anPlanes[4];  // 0x40
    SNO snoAmbientSound;  // 0x50
    ClothWindType eWindType;  // 0x54
    Vector3D wvLocalWind;  // 0x58
};
static_assert(sizeof(ClothDefinition) == 0x64, "ClothDefinition");

class ClothGroupDef : public GroupDefinition {  // size 0xa0
public:
    ClothGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 84
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_CLIENTDATA; }  // vtable slot 7, line 86
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(ClothGroupDef) == 0xa0, "ClothGroupDef");

extern ClothGroupDef * gptGroupDefCloth;  // ClothDefinition.cpp

template <> inline void DefinitionUnlockR<ClothDefinition>(const ClothDefinition *) {}

typedef ScopedDefinitionLockR<ClothDefinition> ScopedClothDefinitionLockR;

template <> inline const ClothDefinition * DefinitionLockR<ClothDefinition>(SNO sno) {  // line 113
    return (const ClothDefinition *)gptGroupDefCloth->LockRFast(sno);
}
