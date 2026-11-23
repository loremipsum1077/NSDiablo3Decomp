// Original file: X/Client/Definitions/PhysicsDefinition.cpp

#include "PhysicsDefinition.h"

// For cgplaceNull, whose initializer the original runs in this file's static initializer.
#include "../../Common/WorldPlace.h"
// For the RGBAColor normals, which the original also initializes here; it reaches GfxVB.h
// through an include chain the DWARF does not record.
#include "../Gfx/GfxVB.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

enum {
    PHYSICS_SNO_VERSION = 37,
    PHYSICS_XML_VERSION = 3,
};

static const float cflDefaultFriction = 0.3f;  // line 25
static const float cflDefaultRollingResistance = 0.1f;
static const float cflDefaultRestitution = 0.0f;
static const float cflDefaultLinearDamping = 0.0f;
static const float cflDefaultAngularDamping = 0.01f;

static const uint16 cwDefaultCollisionMask = 0xffbf;  // line 31

static const TypeDescriptorField PhysicsDefinition_Fields[15] = {  // line 42
    TypeDescriptorField("", DT_INT, offsetof(PhysicsDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(PhysicsDefinition, nMaxIterations), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(PhysicsDefinition, flFriction), (const void *)0x3e99999a, 1),  // cflDefaultFriction
    TypeDescriptorField("", DT_FLOAT, offsetof(PhysicsDefinition, flRollingResistance), (const void *)0x3dcccccd, 1),  // cflDefaultRollingResistance
    TypeDescriptorField("", DT_FLOAT, offsetof(PhysicsDefinition, flRestitution), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(PhysicsDefinition, flLinearDamping), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(PhysicsDefinition, flAngularDamping), (const void *)0x3c23d70a, 1),  // cflDefaultAngularDamping
    TypeDescriptorField("", DT_WORD, offsetof(PhysicsDefinition, wCollisionMask), (const void *)(uintptr_t)cwDefaultCollisionMask, 0x80001),
    TypeDescriptorField("", DT_FLOAT, offsetof(PhysicsDefinition, flDragCoeff), NULL, 1),
    TypeDescriptorField("", DT_ACCEL, offsetof(PhysicsDefinition, flAirBuoyancy), NULL, 1),
    TypeDescriptorField("", DT_ACCEL, offsetof(PhysicsDefinition, flAccelerationMax), NULL, 1),
    TypeDescriptorField("", DT_VELOCITY, offsetof(PhysicsDefinition, flSpeedMax), NULL, 1),
    TypeDescriptorField("", DT_ACCEL, offsetof(PhysicsDefinition, flRandomAccelMin), NULL, 1),
    TypeDescriptorField("", DT_ACCEL, offsetof(PhysicsDefinition, flRandomAccelDelta), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PhysicsDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PhysicsDefinition("Physics", PHYSICS_XML_VERSION, PhysicsDefinition_Fields, 15, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 57
const XTypeDescriptor * DT_PHYSICS = &TypeDescriptor_PhysicsDefinition;

PhysicsGroupDef * gptGroupDefPhysics;  // line 63

// line 69
PhysicsGroupDef::PhysicsGroupDef()
    : GroupDefinition("Physics", PHYSICS_SNO, SNOVersions(PHYSICS_SNO_VERSION, 39, PHYSICS_SNO_VERSION, PHYSICS_SNO_VERSION, 38),
                      SNODATATYPE_TITLE, sizeof(PhysicsDefinition), DT_PHYSICS, 0x2005, 10) {
}

// line 88
uint32 PhysicsGroupDef::GetMaxCount() {
    return 64;
}

// line 100
void PhysicsGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(PhysicsDefinition));

    PhysicsDefinition * pdef = (PhysicsDefinition *)ptDefaultDef;
    pdef->dwFlags = (1 << PHYSICSDEF_AFFECTED_BY_GRAVITY_BIT) | (1 << PHYSICSDEF_COLLIDE_WITH_SAME_ACTOR_BIT);
    pdef->nMaxIterations = 3;
    pdef->flFriction = cflDefaultFriction;
    pdef->flRollingResistance = cflDefaultRollingResistance;
    pdef->flRestitution = cflDefaultRestitution;
    pdef->flLinearDamping = cflDefaultLinearDamping;
    pdef->flAngularDamping = cflDefaultAngularDamping;
    pdef->wCollisionMask = cwDefaultCollisionMask;
}
