// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/TrailUpdate.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "InterpolationTrack.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/RGBAColor.h"

struct Trail;
struct TrailDefinition;

enum {
    MAX_TRAIL_LENGTH = 128,
};

struct TrailCurvePoint {  // size 0x20
    Vector3D wpCurvePoint0;  // 0x00
    Vector3D wpCurvePoint1;  // 0x0c
    RGBAColor rgbaCurvePointColor;  // 0x18
    float flFractionAlongTrail;  // 0x1c
};
static_assert(sizeof(TrailCurvePoint) == 0x20, "TrailCurvePoint");

struct TrailKnotPoint {  // size 0x18
    Vector3D wpKnotPoint0;  // 0x00
    Vector3D wpKnotPoint1;  // 0x0c
};
static_assert(sizeof(TrailKnotPoint) == 0x18, "TrailKnotPoint");

struct TrailUpdateParams {  // size 0x38
    int32 nCurvePoints;  // 0x00
    int32 nLength;  // 0x04
    float flColorOverLengthInterp;  // 0x08
    float flAlphaOverLengthInterp;  // 0x0c
    BOOL fScaleTextureToLength;  // 0x10
    RGBAColor rgbaColor;  // 0x14
    const ColorPath * ptPathColorOverLength;  // 0x18
    const FloatPath * ptPathAlphaOverLength;  // 0x20
    TrailCurvePoint * ptTrailCurvePoints;  // 0x28
    TrailKnotPoint * ptTrailKnotPoints;  // 0x30
    void FromTrailAndDefinition(Trail * ptTrail, const TrailDefinition * pdefTrail);
};
static_assert(sizeof(TrailUpdateParams) == 0x38, "TrailUpdateParams");


void TrailUpdate(const TrailUpdateParams & tParams);  // TrailUpdate.cpp
