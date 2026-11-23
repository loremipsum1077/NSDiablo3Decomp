// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/ClothUpdate.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/Math/Cylinder.h"
#include "../Sigma/Math/Plane.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/Math/Vector.h"

enum {
    CLOTH_MAX_COLLISION_CAPSULES = 16,
    CLOTH_MAX_PLANES = 4,
};

struct ClothVertexSerializable {  // size 0x2c
    Vector3D wp;  // 0x00
    Vector3D wp0;  // 0x0c
    float flInvMass;  // 0x18
    int32 nParentIndex;  // 0x1c
    int32 nAttachmentDistance;  // 0x20
    int32 nFatVertexIndex;  // 0x24
    int32 nIslandIndex;  // 0x28
    BOOL IsDynamic() const;
    BOOL IsKinematic() const;
};
static_assert(sizeof(ClothVertexSerializable) == 0x2c, "ClothVertexSerializable");

struct ClothVertex {  // size 0x54
    Vector3D wp;  // 0x00
    Vector3D wvNormal;  // 0x0c
    Vector3D wp0;  // 0x18
    Vector3D wv;  // 0x24
    float flInvMass;  // 0x30
    int32 nParentIndex;  // 0x34
    int32 nAttachmentDistance;  // 0x38
    int32 nFatVertexIndex;  // 0x3c
    int32 nIslandIndex;  // 0x40
    Vector3D wvCollisionNormal;  // 0x44
    BOOL bCollided;  // 0x50
    BOOL IsDynamic() const;
    BOOL IsKinematic() const;
};
static_assert(sizeof(ClothVertex) == 0x54, "ClothVertex");

struct ClothFaceSerializable {  // size 0x10
    float flRestArea;  // 0x00
    int32 nVertex1;  // 0x04
    int32 nVertex2;  // 0x08
    int32 nVertex3;  // 0x0c
};
static_assert(sizeof(ClothFaceSerializable) == 0x10, "ClothFaceSerializable");

struct ClothFace {  // size 0x1c
    Vector3D wvNormal;  // 0x00
    float flRestArea;  // 0x0c
    int32 nVertex1;  // 0x10
    int32 nVertex2;  // 0x14
    int32 nVertex3;  // 0x18
};
static_assert(sizeof(ClothFace) == 0x1c, "ClothFace");

struct ClothConstraintSerializable {  // size 0x10
    int32 nVertex1;  // 0x00
    int32 nVertex2;  // 0x04
    float flRestLengthSquared;  // 0x08
    float flHorizontal;  // 0x0c
};
static_assert(sizeof(ClothConstraintSerializable) == 0x10, "ClothConstraintSerializable");

struct ClothConstraint {  // size 0x18
    int32 nVertex1;  // 0x00
    int32 nVertex2;  // 0x04
    float flRestLengthSquared;  // 0x08
    float flScale1;  // 0x0c
    float flScale2;  // 0x10
    float flHorizontal;  // 0x14
};
static_assert(sizeof(ClothConstraint) == 0x18, "ClothConstraint");

struct ClothIsland {  // size 0x58
    PRSTransform m_transform1;  // 0x00
    PRSTransform m_transform2;  // 0x20
    float m_maxDispacementSqr;  // 0x40
    int32 m_nStapleCount;  // 0x44
    int32 m_nMinVertexIndex;  // 0x48
    int32 m_nMaxVertexIndex;  // 0x4c
    int32 m_nVertexCount;  // 0x50
    BOOL m_fContiguous;  // 0x54
};
static_assert(sizeof(ClothIsland) == 0x58, "ClothIsland");

struct ClothUpdateInputParams {  // size 0x280
    int32 nVertexCount;  // 0x00
    int32 nFaceCount;  // 0x04
    ClothFace * ptFaces;  // 0x08
    float flWindFactor;  // 0x10
    Vector3D wvWind;  // 0x14
    float flAirBuoyancy;  // 0x20
    float flStretchingStiffness;  // 0x24
    float flHorizontalStiffness;  // 0x28
    float flBendingStiffness;  // 0x2c
    float flDamping;  // 0x30
    float flFriction;  // 0x34
    float flVelocityFromParentFraction;  // 0x38
    BOOL fRelax;  // 0x3c
    int32 nStapleCount;  // 0x40
    int32 nDistanceConstraintCount;  // 0x44
    ClothConstraint * ptDistanceConstraints;  // 0x48
    int32 nBendingConstraintCount;  // 0x50
    ClothConstraint * ptBendingConstraints;  // 0x58
    int32 nCapsuleCount;  // 0x60
    Capsule aCapsules[16];  // 0x64
    int32 nPlaneCount;  // 0x224
    Plane aPlanes[4];  // 0x228
    ClothIsland * ptIslands;  // 0x268
    int32 nIslandCount;  // 0x270
    Vector3D wvActorVelocity;  // 0x274
};
static_assert(sizeof(ClothUpdateInputParams) == 0x280, "ClothUpdateInputParams");

struct ClothUpdateInputOutputParams {  // size 0x20
    ClothVertex * ptVertices;  // 0x00
    float flAverageVelocity;  // 0x08
    Sphere wsBoundingSphere;  // 0x0c
};
static_assert(sizeof(ClothUpdateInputOutputParams) == 0x20, "ClothUpdateInputOutputParams");

struct ClothUpdateParams {  // size 0x2a0
    ClothUpdateInputParams tInput;  // 0x00
    ClothUpdateInputOutputParams tInputOutput;  // 0x280
};
static_assert(sizeof(ClothUpdateParams) == 0x2a0, "ClothUpdateParams");

void ClothUpdate(const ClothUpdateInputParams & tInput, ClothUpdateInputOutputParams & tInputOutput, float flDeltaTime);  // ClothUpdate.cpp


// Functions defined in this file (bodies are not in the DWARF):
//   line    70  BOOL ClothVertex::IsDynamic() const  [inline]
