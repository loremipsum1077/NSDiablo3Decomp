#pragma once

#include "Vector.h"

struct TCBControlPointParams {
    float flTension;
    float flContinuity;
    float flBias;
    float flOutTangentCoeff0;
    float flOutTangentCoeff1;
    float flInTangentCoeff0;
    float flInTangentCoeff1;
};

typedef void (*HermiteEvaluateCallback)(IVector2D&, IVector2D&, void*);

void CurveCatmullRomEvaluate(const Vector3D& wpControl0, const Vector3D& wpControl1, const Vector3D& wpControl2,
                             const Vector3D& wpControl3, const float flT, Vector3D& wpResult);
void CurveCatmullRomEvaluate(const Vector3D* pwpControl, const float flT, Vector3D& wpResult);

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(TCBControlPointParams) == 0x1c, "TCBControlPointParams");
static_assert(offsetof(TCBControlPointParams, flTension) == 0x0, "TCBControlPointParams");
static_assert(offsetof(TCBControlPointParams, flContinuity) == 0x4, "TCBControlPointParams");
static_assert(offsetof(TCBControlPointParams, flBias) == 0x8, "TCBControlPointParams");
static_assert(offsetof(TCBControlPointParams, flOutTangentCoeff0) == 0xc, "TCBControlPointParams");
static_assert(offsetof(TCBControlPointParams, flOutTangentCoeff1) == 0x10, "TCBControlPointParams");
static_assert(offsetof(TCBControlPointParams, flInTangentCoeff0) == 0x14, "TCBControlPointParams");
static_assert(offsetof(TCBControlPointParams, flInTangentCoeff1) == 0x18, "TCBControlPointParams");
