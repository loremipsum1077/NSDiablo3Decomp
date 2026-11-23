// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/Math/Constraint.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Transform.h"
#include "Vector.h"

enum {
    CONSTRAINT_COLLIDE_CONNECTED = 1,
    CONSTRAINT_ANGLE_LIMIT = 2,
    CONSTRAINT_TWIST_LIMIT = 4,
    CONSTRAINT_CONE_LIMIT = 8,
    CONSTRAINT_KINEMATIC = 16,
};

enum ConstraintType {
    CONSTRAINT_TYPE_REVOLUTE = 0,
    CONSTRAINT_TYPE_RAGDOLL = 1,
    CONSTRAINT_TYPE_SPHERICAL = 2,
    CONSTRAINT_TYPE_WELD = 3,
    CONSTRAINT_TYPE_BREAKABLE = 4,
};

enum {
    CONSTRAINT_NAME_LENGTH = 64,
    CONSTRAINT_PARTICLE_SYSTEM_NAME_LENGTH = 64,
};

struct ConstraintParameters {  // size 0xec
    uint32 dwHash;  // 0x00
    uint32 dwHashPadding;  // 0x04
    uint32 dwFlags;  // 0x08
    ConstraintType eType;  // 0x0c
    int32 nBone1;  // 0x10
    int32 nBone2;  // 0x14
    PRTransform frame;  // 0x18
    Vector3D twistAxis;  // 0x34
    PRTransform localFrame1;  // 0x40
    PRTransform localFrame2;  // 0x5c
    float flRevoluteAngleLow;  // 0x78
    float flRevoluteAngleHigh;  // 0x7c
    float flRagdollConeAngle;  // 0x80
    float flRagdollTwistLow;  // 0x84
    float flRagdollTwistHigh;  // 0x88
    float flFriction;  // 0x8c
    float flCompliance;  // 0x90
    float flAngularFrequencyHz;  // 0x94
    float flAngularDampingRatio;  // 0x98
    float flLinearFrequencyHz;  // 0x9c
    float flLinearDampingRatio;  // 0xa0
    float flMotorStrength;  // 0xa4
    float flStrength;  // 0xa8
    char szParticleSystem[64];  // 0xac
};
static_assert(sizeof(ConstraintParameters) == 0xec, "ConstraintParameters");

class XTypeDescriptor;

// Defined in Constraint.cpp (line 53).
extern const XTypeDescriptor * DT_CONSTRAINTPARAMETERS;

