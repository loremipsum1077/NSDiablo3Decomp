#pragma once

#include <math.h>

#include "../SigmaTypes.h"

typedef float WorldDistance;

typedef float Angle;

static const float EPSILON = 0.000001f;

static const WorldDistance MAX_WORLDDISTANCE = 3.402823466e+38f;

static const float PI = 3.14159265358979323846f;

static const Angle FULL_CIRCLE = PI * 2.0f;
static const Angle HALF_CIRCLE = PI;
static const Angle QUARTER_CIRCLE = PI * 0.5f;
static const Angle EIGHTH_CIRCLE = PI * 0.25f;

namespace Math {
uint32 MostSignificantBit(uint32 dw);
uint32 LogBase2(uint32 dw);
uint32 Ceiling2Power(uint32 dw);
BOOL Is2Power(uint32 dw);
uint32 NextLarger2Power(uint32 dw);

// Original line 118 in MathGeneral.h; the return is line 122 (add, sub, and, and the negated mask on columns 33, 18,
// 39 and 41). No `dwAlignment == 1` test: the copies inlined into sInitializeTrans / sInitializeScale
// (Animation.cpp:1024, 1070, 1071) mask unconditionally; FileSerialize's test is its own line 180.
inline uintptr Align(uintptr dwX, uintptr dwAlignment) {
    return (dwX + dwAlignment - 1) & ~(dwAlignment - 1);
}

Angle ACos(const float x);
Angle ASin(const float x);
Angle ATan(float y, float x);

template <typename T>
T Abs(const T x);
template <typename T>
T Max(const T x, const T y);
template <typename T>
T Min(const T x, const T y);
template <typename T, typename U>
T Pin(const T n, const U min, const U max);
template <typename T, typename U>
T PinWrap(const T n, const U min, const U max);

template <typename T>
void Swap(T& x, T& y);

uint32 CountOnes(uint32 n);
uint32 FloorLog2(uint32 n);
uint64 CountOnes(uint64 n);
uint64 FloorLog2(uint64 n);

BOOL FloatEqual(float x, float y);

template <typename T>
void Lerp(const T& t0, const T& t1, const float t, T& tResult);
template <typename T>
T Lerp(const T& t0, const T& t1, const float t);

float SmoothStep(const float flT);
float SmootherStep(const float flT);
template <typename T>
T SmoothStep(const T& t0, const T& t1, const float flT);

int32 FloatRoundToInt(float f);
int32 FloatFloorToInt(float f);
float FloatTrunc(float f);
int32 FloatToInt(float f);
float FloatFrac(const float x);
float FloatMod(const float x, const float y);
float PositiveFloatMod(const float x, const float y);

// Line 203, the FloatRoundToInt call at line 208. The body is not in the DWARF; the only local is
// flFactor, and callers with a constant nPlaces of 2 multiply and divide by 100.
inline float FloatRoundToXPlaces(float flNum, int32 nPlaces) {
    float flFactor = powf(10.0f, (float)nPlaces);
    return (float)FloatRoundToInt(flNum * flFactor) / flFactor;
}
Angle NormalizeAngle(Angle a);
Angle DegToAngle(float flDeg);
float AngleToDeg(Angle aAng);
Angle SignedAngularDifference(Angle a, Angle b);
Angle AngularDifference(Angle a, Angle b);
float Sin(Angle a);
float Cos(Angle a);
float Tan(Angle a);
float Sqrt(float s);

// MathGeneral.h:236 (inlined into SoundPlatform::SetReverbParams).
inline float Pow(const float x, const float y) {
    return powf(x, y);
}
}  // namespace Math

extern const float cgflSqrt2;
extern const float cgflSqrt3;
extern const float cgflSqrt2Over2;

#include "MathGeneral.inl"
