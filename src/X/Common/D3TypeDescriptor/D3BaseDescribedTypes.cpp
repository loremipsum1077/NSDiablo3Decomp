// Original file: X/Common/D3TypeDescriptor/D3BaseDescribedTypes.cpp

#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// The header-level statics of these three headers (blz::ignore, SNONAME_NONE, cgplaceNull and the
// GfxVB.h normals) are initialized in this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// The three described vector types are plain Vector3D aliases, declared here per the DWARF.
typedef Vector3D VelocityVector3D;
typedef Vector3D AccelVector3D;
typedef Vector3D ImpulseVector3D;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares them yet.
extern const XTypeDescriptor * DT_VELOCITY;
extern const XTypeDescriptor * DT_ACCEL;
extern const XTypeDescriptor * DT_IMPULSE;

// The field names are "" in the binary.
static const TypeDescriptorField VelocityVector3D_Fields[4] = {  // line 13
    TypeDescriptorField("", DT_VELOCITY, offsetof(VelocityVector3D, x), NULL, 1),
    TypeDescriptorField("", DT_VELOCITY, offsetof(VelocityVector3D, y), NULL, 1),
    TypeDescriptorField("", DT_VELOCITY, offsetof(VelocityVector3D, z), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(VelocityVector3D), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_VelocityVector3D("VelocityVector3D", 0, VelocityVector3D_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 17
const XTypeDescriptor * DT_VELOCITYVECTOR3D = &TypeDescriptor_VelocityVector3D;

static const TypeDescriptorField AccelVector3D_Fields[4] = {  // line 20
    TypeDescriptorField("", DT_ACCEL, offsetof(AccelVector3D, x), NULL, 1),
    TypeDescriptorField("", DT_ACCEL, offsetof(AccelVector3D, y), NULL, 1),
    TypeDescriptorField("", DT_ACCEL, offsetof(AccelVector3D, z), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AccelVector3D), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AccelVector3D("AccelVector3D", 0, AccelVector3D_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 24
const XTypeDescriptor * DT_ACCELVECTOR3D = &TypeDescriptor_AccelVector3D;

static const TypeDescriptorField ImpulseVector3D_Fields[4] = {  // line 27
    TypeDescriptorField("", DT_IMPULSE, offsetof(ImpulseVector3D, x), NULL, 1),
    TypeDescriptorField("", DT_IMPULSE, offsetof(ImpulseVector3D, y), NULL, 1),
    TypeDescriptorField("", DT_IMPULSE, offsetof(ImpulseVector3D, z), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ImpulseVector3D), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ImpulseVector3D("ImpulseVector3D", 0, ImpulseVector3D_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 31
const XTypeDescriptor * DT_IMPULSEVECTOR3D = &TypeDescriptor_ImpulseVector3D;
