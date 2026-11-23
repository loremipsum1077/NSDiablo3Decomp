// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/PhysicsDefinition.h
// Definition layout and resident lock helpers are adopted for AttachedRagdoll.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"

class XMemoryAllocatorInterface;

enum {
    PHYSICSDEF_DO_NOT_INTEGRATE_BIT_DEPRECATED = 0,
    PHYSICSDEF_DO_NOT_AUTOSLEEP_BIT = 1,
    PHYSICSDEF_AFFECTED_BY_GRAVITY_BIT = 2,
    PHYSICSDEF_AFFECTED_BY_WIND_BIT = 3,
    PHYSICSDEF_AFFECTED_BY_EXPLOSIONS_BIT = 4,
    PHYSICSDEF_AFFECTED_BY_TURBULENCE_BIT_DEPRECATED = 5,
    PHYSICSDEF_CONSTRAINED_TO_GROUND_BIT_DEPRECATED = 6,
    PHYSICSDEF_AFFECTED_BY_FLOCKING_BIT_DEPRECATED = 7,
    PHYSICSDEF_HAS_MAXIMUM_ACCELERATION_BIT = 8,
    PHYSICSDEF_HAS_MAXIMUM_SPEED_BIT = 9,
    PHYSICSDEF_UNUSED_BIT = 10,
    PHYSICSDEF_CONSTRAINED_TO_ALTITUDE_BIT_DEPRECATED = 11,
    PHYSICSDEF_CONSTRAINED_TO_SYSTEM_RADIUS_BIT_DEPRECATED = 12,
    PHYSICSDEF_CONSTRAINED_TO_COLLISION_SPHERES_BIT = 13,
    PHYSICSDEF_AFFECTED_BY_RANDOM_ACCEL = 14,
    PHYSICSDEF_AFFECTED_BY_RIPPLING_BIT_DEPRECATED = 15,
    PHYSICSDEF_CONSTRAINED_TO_LOCAL_COLLISION_SPHERES_BIT_DEPRECATED = 16,
    PHYSICSDEF_COLLIDE_WITH_SAME_ACTOR_BIT = 17,
    PHYSICSDEF_AFFECTED_BY_BUOYANCY_BIT = 18,
    PHYSICSDEF_AFFECTED_BY_DISTURBANCE_BIT = 19,
    PHYSICSDEF_ANIMATED_PROP = 20,
};

struct PhysicsDefinition {  // size 0x44
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    int32 nMaxIterations;  // 0x10
    float flFriction;  // 0x14
    float flRollingResistance;  // 0x18
    float flRestitution;  // 0x1c
    float flLinearDamping;  // 0x20
    float flAngularDamping;  // 0x24
    uint16 wCollisionMask;  // 0x28
    float flDragCoeff;  // 0x2c
    float flAirBuoyancy;  // 0x30
    float flAccelerationMax;  // 0x34
    float flSpeedMax;  // 0x38
    float flRandomAccelMin;  // 0x3c
    float flRandomAccelDelta;  // 0x40
};
static_assert(sizeof(PhysicsDefinition) == 0x44, "PhysicsDefinition");

class PhysicsGroupDef : public GroupDefinition {  // size 0xa0
public:
    PhysicsGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 88
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 90
        return SNOPACKTYPE_CLIENTDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(PhysicsGroupDef) == 0xa0, "PhysicsGroupDef");

extern PhysicsGroupDef * gptGroupDefPhysics;  // PhysicsDefinition.cpp:63

class XTypeDescriptor;
extern const XTypeDescriptor * DT_PHYSICS;  // PhysicsDefinition.cpp:57

typedef ScopedDefinitionLockR<PhysicsDefinition> ScopedPhysicsDefinitionLockR;
// Original inline specializations at lines 118-119; physics uses the resident definition.
template <> inline const PhysicsDefinition * DefinitionLockR<PhysicsDefinition>(SNO sno) {
    return static_cast<const PhysicsDefinition *>(gptGroupDefPhysics->LockRFast(sno));
}
template <> inline void DefinitionUnlockR<PhysicsDefinition>(const PhysicsDefinition *) {}
