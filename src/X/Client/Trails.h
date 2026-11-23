// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Trails.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "AttachmentParent.h"
#include "Definitions/TrailDefinition.h"
#include "Gfx/GfxCommon.h"
#include "Gfx/GfxIB.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/RGBAColor.h"

struct InterpTime;
struct TexAnimState;
struct TrailCurvePoint;
struct TrailKnotPoint;

typedef int32 TrailType;

typedef int32 VBID;

typedef int32 IBID;

enum {
    MAX_TRAIL_ATTACH_POINTS = 2,
    MAX_TRAIL_CURVE_POINTS = 512,
};

enum {
    TRAILID_INVALID = -1,
};

typedef int32 TrailID;

struct TrailInterpState {  // size 0x1c
    float flTime;  // 0x00
    float flSystemDuration;  // 0x04
    BOOL bOutro;  // 0x08
    BOOL bInfiniteDuration;  // 0x0c
    float flOutroTime;  // 0x10
    float flOutroCrossFade;  // 0x14
    float flOutroDuration;  // 0x18
    void Init(BOOL _bInfiniteDuration, float _flSystemDuration, float _flOutroDuration);
    BOOL UpdateAndGetTime(InterpTime & tInterpTime, const float flDeltaTime);
};
static_assert(sizeof(TrailInterpState) == 0x1c, "TrailInterpState");

struct Trail {  // size 0xf8
    TrailID id;  // 0x00
    uint32 dwLastFrameSubmitted;  // 0x04
    SNO snoTrail;  // 0x08
    RenderContext eContext;  // 0x0c
    int32 nLength;  // 0x10
    int32 nCircleSegments;  // 0x14
    int32 nCurvePoints;  // 0x18
    TrailType nTrailType;  // 0x1c
    uint32 dwTrailFlags;  // 0x20
    IBID ibid;  // 0x24
    ParentData tParentData[2];  // 0x28
    TrailKnotPoint * tTrailKnotPoints;  // 0x80
    TrailCurvePoint * tTrailCurvePoints;  // 0x88
    float flCreateTime;  // 0x90
    TrailInterpState tInterpState;  // 0x94
    float flLastTimeBumped;  // 0xb0
    float flColorOverTimeInterp;  // 0xb4
    float flColorOverLengthInterp;  // 0xb8
    float flAlphaOverTimeInterp;  // 0xbc
    float flAlphaOverLengthInterp;  // 0xc0
    TexAnimState * tTexAnimStateList;  // 0xc8
    RGBAColor rgbaColor;  // 0xd0
    float flTrailAlpha;  // 0xd4
    Vector3D wvLastAlong;  // 0xd8
    Vector3D wvLastForward;  // 0xe4
    float flTimeParentWarped;  // 0xf0
};
static_assert(sizeof(Trail) == 0xf8, "Trail");


// Functions defined in this file (bodies are not in the DWARF):
//   line    64  Trail::Trail()  [inlined into 1 functions]
