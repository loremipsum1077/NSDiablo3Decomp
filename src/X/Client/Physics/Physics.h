// Subset of X/Client/Physics/Physics.h (the ScreenManager.h and Cylinder.h includes are left
// out): the types Particle.cpp touches, on their DWARF layouts.
// Original file: X/Client/Physics/Physics.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Vector.h"

enum {
    PHYSICSSYSTEM_AWAKE_BIT = 0,
    PHYSICSSYSTEM_INITIALIZED = 1,
};

enum {
    PHYSICSSYSTEM_MAX_COLLISION_CAPSULES = 32,
};

struct PointMass {  // size 0x28
    float flInvMass;  // 0x00
    Vector3D wp;  // 0x04
    Vector3D wp0;  // 0x10
    Vector3D wv;  // 0x1c
};
static_assert(sizeof(PointMass) == 0x28, "PointMass");

struct DistanceConstraint {  // size 0x18
    PointMass * ptPointMassA;  // 0x00
    PointMass * ptPointMassB;  // 0x08
    WorldDistance wdRestLength;  // 0x10
    float flRestoringForce;  // 0x14
};
static_assert(sizeof(DistanceConstraint) == 0x18, "DistanceConstraint");

typedef XPooledList<PointMass *, PointMass *> PointMassList;

typedef XListMemoryPool<PointMass *> PointMassListPool;

struct PhysicsSystem {  // size 0x40
    uint32 dwFlags;  // 0x00
    SNO snoPhysics;  // 0x04
    PointMassList listContacts;  // 0x08
    DistanceConstraint * ptConstraints;  // 0x30
    int32 nConstraintCount;  // 0x38
    RActorID idLocalRActor;  // 0x3c
};
static_assert(sizeof(PhysicsSystem) == 0x40, "PhysicsSystem");

// X/Client/Physics/Physics.cpp
void PhysicsSystemInitialize(PhysicsSystem & tPhysics, SNO snoPhysics, RActorID idLocalRActor);  // Physics.cpp:152
void PhysicsSystemDispose(PhysicsSystem & tPhysics);  // Physics.cpp:165
