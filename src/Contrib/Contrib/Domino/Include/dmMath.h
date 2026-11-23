// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Include/dmMath.h

#pragma once

#include "dmSerialize.h"
#include <arm_neon.h>
#include <math.h>

typedef float32x4_t dm_vec4;

typedef const dm_vec4 & dm_vec4Arg;

union dmVecF32 {  // size 0x10
    dm_float32 f[4];  // 0x00
    dm_vec4 v;  // 0x00
};
static_assert(sizeof(dmVecF32) == 0x10, "dmVecF32");

union dmVecU32 {  // size 0x10
    dm_uint32 u[4];  // 0x00
    dm_vec4 v;  // 0x00
};
static_assert(sizeof(dmVecU32) == 0x10, "dmVecU32");

union dmVecI32 {  // size 0x10
    dm_int32 i[4];  // 0x00
    dm_vec4 v;  // 0x00
};
static_assert(sizeof(dmVecI32) == 0x10, "dmVecI32");

struct dmMtx {  // size 0x40
    dm_vec4 rx;  // 0x00
    dm_vec4 ry;  // 0x10
    dm_vec4 rz;  // 0x20
    dm_vec4 p;  // 0x30
};
static_assert(sizeof(dmMtx) == 0x40, "dmMtx");

struct dmTransform {  // size 0x20
    dm_vec4 q;  // 0x00
    dm_vec4 p;  // 0x10
};
static_assert(sizeof(dmTransform) == 0x20, "dmTransform");

// dmMath.inl: the dmVec4 constants live in Domino's math TU.
extern const dm_vec4 dm_vec4_one;
extern const dm_vec4 dm_vec4_two;
extern const dm_vec4 dm_vec4_oneHalf;
extern const dm_vec4 dm_vec4_zeroSafe;
extern const dm_vec4 dm_vec4_unitZ;
extern const dm_vec4 dm_vec4_unitY;
extern const dm_vec4 dm_vec4_finite4;
extern const dm_vec4 dm_vec4_maxFloat;
extern const dm_vec4 dm_vec4_three;
extern const dm_vec4 dm_vec4_oneThird;
extern const dm_vec4 dm_quat_identity;
extern const dm_vec4 dm_vec4_epsilon;

extern const dm_vec4 dm_vec4_unitX;
extern const dm_vec4 dm_vec4_2pi;
extern const dm_vec4 dm_vec4_pi;
#include "dmMath.inl"

dm_vec4 dmSolveSym22(const dmMtx & A, dm_vec4Arg x);
dmMtx dmInvertSym22(const dmMtx & A);
dmMtx dmInvertSym33(const dmMtx & A);
dm_vec4 dmSolveSym33(const dmMtx & A, dm_vec4Arg b);
dm_vec4 dmSolve33(const dmMtx & A, dm_vec4Arg b);
dmMtx dmInvertOrtho34(const dmMtx & A);
