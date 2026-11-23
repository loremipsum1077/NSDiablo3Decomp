// Provisional owning header for X/Client/Rope.cpp.
//
// The 2.6.2 DWARF attributes RopeGet, RopeTryToGet, RopeRegisterParentObject and
// RopeGetHardpointOrientation to X/Client/Rope.cpp. The full class/skeleton for this
// header has not been adopted in this checkout, so only the types and members
// X/Client/AttachmentParent.cpp needs live here, at their documented DWARF offsets.

#pragma once

#include "AttachmentParent.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/Math/Vector.h"
#include "Gfx/GfxCommon.h"
#include "Physics/Physics.h"
#include "RMessage.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/RGBAColor.h"

struct TexAnimState;

enum RopeAttachType {  // Rope.h:24
    ROPE_ATTACH_NONE = 0,
    ROPE_ATTACH_PARENT_CENTER = 1,
    ROPE_ATTACH_PARENT = 2,
    ROPE_ATTACH_WORLDPOINT = 3,
};

enum {  // Rope.h:61
    ROPEID_INVALID = -1,
};

struct RopeAttachPoint {  // size 0x3c
    int32 nRopeAttachType;  // 0x00 RopeAttachType
    ParentData tParentData;  // 0x04
    Vector3D wpAttach;  // 0x30
};
static_assert(sizeof(RopeAttachPoint) == 0x3c, "RopeAttachPoint");

struct RopeCurvePoint {  // size 0x2c
    Vector3D wpCurvePoint;  // 0x00
    Vector2D wpScreenSpacePoint;  // 0x0c
    Vector2D wvScreenSpaceEdge[2];  // 0x14
    WorldDistance wdRopeWidth;  // 0x24
    RGBAColor rgbaCurvePointColor;  // 0x28
};
static_assert(sizeof(RopeCurvePoint) == 0x2c, "RopeCurvePoint");

struct RopeKnotPoint {  // size 0x20
    Vector3D wvJitter;  // 0x00
    Vector3D wvJitterTarget;  // 0x0c
    PointMass * ptPointMass;  // 0x18
};
static_assert(sizeof(RopeKnotPoint) == 0x20, "RopeKnotPoint");

struct Rope {  // size 0x250
    RopeID id;  // 0x00
    uint32 dwLastFrameSubmitted;  // 0x04
    SNO snoRope;  // 0x08
    Vector3D wpBase;  // 0x0c
    PhysicsSystem tPhysics;  // 0x18
    BOOL fFixedUpdate;  // 0x58
    RMessageList tRMsgList;  // 0x60
    XPooledDataIDList tAttachmentList;  // 0x98
    PointMass * ptPointMassHardpoint;  // 0xc0
    float flHardpointConstraintDistance;  // 0xc8
    PRTransform transformPin[2];  // 0xcc
    int32 nVertCount;  // 0x104
    Sphere ws;  // 0x108
    int32 nRopeKnots;  // 0x118
    int32 nCurvePoints;  // 0x11c
    RopeKnotPoint * ptKnotPoints;  // 0x120
    RopeCurvePoint * ptCurvePoints;  // 0x128
    DistanceConstraint * tConstraints;  // 0x130
    uint32 dwRopeInstFlags;  // 0x138
    float flTime;  // 0x13c
    BOOL bOutro;  // 0x140
    float flOutroTime;  // 0x144
    float flOutroCrossFade;  // 0x148
    float flSystemDuration;  // 0x14c
    float flOutroDuration;  // 0x150
    float flColorOverTimeInterp;  // 0x154
    float flColorOverLengthInterp;  // 0x158
    float flAlphaOverTimeInterp;  // 0x15c
    float flAlphaOverLengthInterp;  // 0x160
    float flWidthOverLengthInterp;  // 0x164
    float flWidthOverTimeInterp;  // 0x168
    float flSourcePointWorldUpMotionInterp;  // 0x16c
    float flSourcePointAlongRopeMotionInterp;  // 0x170
    float flSourcePointPerpendicularMotionInterp;  // 0x174
    float flDestPointWorldUpMotionInterp;  // 0x178
    float flDestPointAlongRopeMotionInterp;  // 0x17c
    float flDestPointPerpendicularMotionInterp;  // 0x180
    Vector3D wvJitterAmplitudeInterp;  // 0x184
    float flJitterRateInterp;  // 0x190
    float flSeparationInterp;  // 0x194
    float flXAmpOverLengthInterp;  // 0x198
    float flYAmpOverLengthInterp;  // 0x19c
    float flFreqOverTimeInterp;  // 0x1a0
    float flPhaseSpeedOverTimeInterp;  // 0x1a4
    float flRopeAlpha;  // 0x1a8
    float flJitterRateAccumulator;  // 0x1ac
    RGBAColor rgbaRopeColor;  // 0x1b0
    float flFreq;  // 0x1b4
    float flPhaseSpeed;  // 0x1b8
    TexAnimState * tTexAnimStateList;  // 0x1c0
    RenderContext eContext;  // 0x1c8
    RopeAttachPoint tRopeAttachPoints[2];  // 0x1cc
    float flLastUpdateTime;  // 0x244
    float flLastRegisterTime;  // 0x248
    int32 nSimTreeDepth;  // 0x24c
};
static_assert(sizeof(Rope) == 0x250, "Rope");

// X/Client/Rope.cpp
Rope * RopeGet(RopeID idRope);
Rope * RopeTryToGet(RopeID idRope);
int32 RopeRegisterParentObject(RopeID idRope);
void RopesDeleteAll();  // Rope.cpp:170
void RopeRegisterParentObjects(BOOL fFixedUpdateOnly);  // Rope.cpp:1092
void RopeDelete(RopeID id);  // Rope.cpp:1406
void RopePlayOutro(const RopeID idRope);  // Rope.cpp:1451
void RopeGetHardpointOrientation(const Rope * ptRope, int32 nKnotIndex, PRTransform & transform);
RopeID RopeAdd(const SNO snoRope, RopeAttachPoint * ptRopeAttachPoints, BOOL fIgnoreOwnerAlpha,
               const RenderContext eContext);  // Rope.cpp:1224

void RopeSetAttachPoint(RopeID idRope, BOOL bDest, RopeAttachPoint & tRopeAttachPoint);  // Rope.cpp:1354
