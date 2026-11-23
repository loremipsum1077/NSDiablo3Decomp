// Original file: X/Sigma/Math/Constraint.cpp

#include "Constraint.h"

#include <stddef.h>
#include "../TypeDescriptor/TypeDescriptorField.h"
#include "../TypeDescriptor/TypeDescriptor.h"

static const TypeDescriptorField ConstraintParameters_Fields[] = {
    TypeDescriptorField("", DT_INT, offsetof(ConstraintParameters, dwHash), NULL, (1 << TDF_XBOXONE_ONLY) | (1 << TDF_PS4_ONLY) | (1 << TDF_NX64_ONLY)),
    TypeDescriptorField("", DT_INT, offsetof(ConstraintParameters, dwHashPadding), NULL, (1 << TDF_XBOXONE_ONLY) | (1 << TDF_PS4_ONLY) | (1 << TDF_NX64_ONLY)),
    TypeDescriptorField("", DT_INT, offsetof(ConstraintParameters, dwFlags), NULL, (1 << TDF_XML_SERIALIZATION) | (1 << TDF_BIT_FLAGS)),
    TypeDescriptorField("", DT_INT, offsetof(ConstraintParameters, eType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConstraintParameters, nBone1), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConstraintParameters, nBone2), NULL, 1),
    TypeDescriptorField("", DT_PRTRANSFORM, offsetof(ConstraintParameters, frame), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ConstraintParameters, twistAxis), NULL, 1),
    TypeDescriptorField("", DT_PRTRANSFORM, offsetof(ConstraintParameters, localFrame1), NULL, 1),
    TypeDescriptorField("", DT_PRTRANSFORM, offsetof(ConstraintParameters, localFrame2), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConstraintParameters, flRevoluteAngleLow), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConstraintParameters, flRevoluteAngleHigh), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConstraintParameters, flRagdollConeAngle), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConstraintParameters, flRagdollTwistLow), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConstraintParameters, flRagdollTwistHigh), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConstraintParameters, flFriction), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConstraintParameters, flCompliance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConstraintParameters, flAngularFrequencyHz), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConstraintParameters, flAngularDampingRatio), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConstraintParameters, flLinearFrequencyHz), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConstraintParameters, flLinearDampingRatio), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConstraintParameters, flMotorStrength), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConstraintParameters, flStrength), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(ConstraintParameters, szParticleSystem), "", 1, CONSTRAINT_PARTICLE_SYSTEM_NAME_LENGTH, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ConstraintParameters), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ConstraintParameters("ConstraintParameters", 1, ConstraintParameters_Fields, 25, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CONSTRAINTPARAMETERS = &TypeDescriptor_ConstraintParameters;
