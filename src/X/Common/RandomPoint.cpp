// X/Common/RandomPoint.cpp: uniform random points on a sphere, on a circle and on a
// dome, plus the annular radius sampler.  main.elf 0x005bc210..0x005bc51c.
//
// Sphere and dome share one recipe: the altitude is the arc-sine of a uniform value
// (so the points do not bunch at the poles), the azimuth comes from `RandAngle`, and
// the spherical-to-cartesian product is spelled out per component -- which is why
// `Cos` is called twice on the same altitude instead of once.
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Random.h"

// 0x005bc210 (RandomPoint.cpp:18) 316 bytes. Matches.
// The launch build keeps each `radius * trig` product in a fresh callee-saved register
// (s12/s13, so it also saves d12/d13). Operands, order and the five `fmul`s:
//   fRandV = rand*flInvDwordMax; flDelta 2 => fadd s0,s0,s0; + (-1.0)
//   Sin(azimuth) -> *wdRadius -> *Cos(altitude)  (x), Cos(azimuth) -> *wdRadius -> *Cos(altitude) (y), Sin(altitude) -> *wdRadius (z)
//   NormalizeAngle(-altitude) is Pin(a,-50.2655,50.2655) then PinWrap(a,0,FULL_CIRCLE).
void GetRandomVectorPointOnSphere(RNG& rng, WorldDistance wdRadius, Vector3D& wv)
{
    float fRandV = RandRangeFloat_Internal(rng, -1.0f, 2.0f);  // :25
    Angle aAltitude = Math::ASin(Math::Abs(fRandV));           // :26

    if (fRandV < 0.0f)                                         // :28
        aAltitude = Math::NormalizeAngle(-aAltitude);          // :30

    Angle aAzimuth = RandAngle(rng);                           // :33

    wv.Set(wdRadius * Math::Sin(aAzimuth) * Math::Cos(aAltitude),  // :36
           wdRadius * Math::Cos(aAzimuth) * Math::Cos(aAltitude),  // :37
           wdRadius * Math::Sin(aAltitude));                       // :38
}                                                              // :39

// 0x005bc350 (RandomPoint.cpp:44) 136 bytes. Matches. The radius is the square root of a
// value uniform on [fMinUnit^2, 1) scaled by fMax, so the samples cover the annulus evenly.
// The `fMax < EPSILON` early out writes `fmov s0, wzr`; the RandRangeFloat guard `b.eq` fires
// when `1.0f - fMinUnit*fMinUnit == 0.0f`.
WorldDistance GetRandomRadius(RNG& rng, WorldDistance wdRadiusMin, WorldDistance wdRadiusDelta)
{
    float fMax = wdRadiusMin + wdRadiusDelta;   // :52
    if (fMax < EPSILON)                         // :54
        return 0.0f;

    float fMinUnit = wdRadiusMin / fMax;        // :59
    fMinUnit *= fMinUnit;                       // :60

    float fRandR = RandRangeFloat_Internal(rng, fMinUnit, 1.0f - fMinUnit);  // :62
    return fMax * Math::Sqrt(fRandR);           // :63
}                                              // :66

// 0x005bc3e0 (RandomPoint.cpp:71) 120 bytes. Matches. The flat case: one angle and a
// literal zero for z (`str wzr`). Cos is evaluated before Sin (F2:498 then F2:489).
void GetRandomVectorPointOnCircle(RNG& rng, WorldDistance wdRadius, Vector3D& wv)
{
    Angle a = RandAngle(rng);          // :78

    wv.Set(wdRadius * Math::Cos(a),    // :81
           wdRadius * Math::Sin(a),    // :82
           0.0f);                     // :83
}                                     // :84

// 0x005bc460 (RandomPoint.cpp:89) 188 bytes. Matches. The upper half only: the
// uniform value is drawn from [0, 1) instead of [-1, 1), so the altitude is never negative and
// the mirror branch of the sphere disappears. The x/y products are the spherical ones and `Cos`
// is emitted twice per component, mirroring the sphere.
void GetRandomVectorPointOnDome(RNG& rng, WorldDistance wdRadius, Vector3D& wv)
{
    float fRandV = RandRangeFloat_Internal(rng, 0.0f, 1.0f);   // :96
    Angle aAltitude = Math::ASin(fRandV);                      // :97

    Angle aAzimuth = RandAngle(rng);                           // :104

    wv.Set(wdRadius * Math::Sin(aAzimuth) * Math::Cos(aAltitude),  // :107
           wdRadius * Math::Cos(aAzimuth) * Math::Cos(aAltitude),  // :108
           wdRadius * Math::Sin(aAltitude));                       // :109
}                                                                 // :110
