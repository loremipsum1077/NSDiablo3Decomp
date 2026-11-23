// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/InterpolationScalar.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/CommonTypes.h"
#include "../Sigma/SigmaTypes.h"

struct RActor;

enum ScalarFunction {
    SCALAR_FUNCTION_NONE = 0,
    SCALAR_FUNCTION_MSG_SCALAR = 1,
    SCALAR_FUNCTION_WEATHER_INTENSITY = 2,
    SCALAR_FUNCTION_FRACTIONAL_RADIAL_DIST = 3,
    SCALAR_FUNCTION_ATTACK_RATE = 4,
    SCALAR_FUNCTION_ACTOR_HEALTH_PERCENT = 5,
    SCALAR_FUNCTION_FRACTIONAL_HEIGHT = 6,
    SCALAR_FUNCTION_PLAYER_CLOSE_TO_DEATH = 7,
    SCALAR_FUNCTION_ACTOR_SCALE = 8,
    SCALAR_FUNCTION_SPAWNER_COUNTDOWN_PERCENT = 9,
    SCALAR_FUNCTION_RANDOM = 10,
    SCALAR_FUNCTION_VALUE_FROM_BONE_X = 11,
    SCALAR_FUNCTION_ACTOR_PRIMARY_RESOURCE_PERCENT = 12,
    SCALAR_FUNCTION_GOLD_PICKUP_RADIUS = 13,
    NUM_SCALAR_FUNCTIONS = 14,
};

struct InterpScalarParams {  // size 0x18
    InterpScalarParams();
    void Init();
    void InitParamsFromRActor(const RActor * ptRActor);
    BOOL Evaluate(const int32 nScalarFunction, float & flScalar) const;
    float flMsgScalar;  // 0x00
    float flRadialDistance;  // 0x04
    float flFractionalHeight;  // 0x08
    ACDID idParentACD;  // 0x0c
    RActorID idParentRActor;  // 0x10
    int32 nBone;  // 0x14
};
static_assert(sizeof(InterpScalarParams) == 0x18, "InterpScalarParams");

inline InterpScalarParams::InterpScalarParams() {  // line 40
    Init();
}

struct InterpolationScalar {  // size 0xc
    ScalarFunction nScalarFunction;  // 0x00
    float flScalarLerpZero;  // 0x04
    float flScalarLerpOne;  // 0x08
    InterpolationScalar() {}
    InterpolationScalar(const ScalarFunction, const float, const float);
    BOOL GetScalar(const InterpScalarParams & tParams, float & flFinalScalar) const;
};
static_assert(sizeof(InterpolationScalar) == 0xc, "InterpolationScalar");

inline InterpolationScalar::InterpolationScalar(const ScalarFunction nScalarFunction, const float flScalarLerpZero, const float flScalarLerpOne)  // line 73
    : nScalarFunction(nScalarFunction), flScalarLerpZero(flScalarLerpZero), flScalarLerpOne(flScalarLerpOne) {
}


// Functions defined in this file (bodies are not in the DWARF):
//   line    40  InterpScalarParams::InterpScalarParams()  [inlined into 18 functions]
//   line    73  InterpolationScalar::InterpolationScalar(const ScalarFunction nScalarFunction, const float flScalarLerpZero, const float flScalarLerpOne)  [inline]
