template <typename T>  // Original line 29 (x at 30): the generic template; its only instance is Abs<double> (UIColorPickerControl.cpp), fneg/fcmp/fcsel ge with rows 32:28 (negation) and 32:9 (compare and select).
inline T Math::Abs(const T x) {
    return x >= 0 ? x : -x;
}

// Original line 36 (the fabs lands at :40).
template <>
inline float Math::Abs<float>(const float x) {
    return fabsf(x);
}

template <typename T>
inline T Math::Max(const T x, const T y) {
    return x > y ? x : y;
}

template <typename T>
inline T Math::Min(const T x, const T y) {
    return x < y ? x : y;
}

// Original line 75.
template <typename T, typename U>
inline T Math::Pin(const T n, const U min, const U max) {
    return n < min ? min : (n > max ? max : n);
}

// Original line 85.
template <typename T, typename U>
inline T Math::PinWrap(const T n, const U min, const U max) {
    T value = n;
    while (value < min)
        value += max - min;
    while (value > max)
        value -= max - min;
    return value;
}

// Original line 106: the int32 specialization wraps into the half-open range [min, max). Body
// written from the copies inlined into X/Client/UserInterface/UI.cpp (sIncrementFocus).
template <>
inline int32 Math::PinWrap<int32, int32>(const int32 n, const int32 min, const int32 max) {
    int32 value = n;
    while (value < min)
        value = value + max - min;  // left-associated: the constant-bound copies in UIConsoleWhatsNewSeason.cpp OnDPadLeft/OnDPadRight match only with this form
    while (value >= max)
        value -= max - min;
    return value;
}

template <typename T>
inline void Math::Swap(T& x, T& y) {
    T temp = x;
    x = y;
    y = temp;
}

inline uint32 Math::CountOnes(uint32 n) {
    n = n - ((n >> 1) & 0x55555555);
    n = ((n >> 2) & 0x33333333) + (n & 0x33333333);
    n = (n + (n >> 4)) & 0x0F0F0F0F;
    n = n + (n >> 8);
    n = n + (n >> 16);
    return n & 0x3F;
}

inline uint32 Math::FloorLog2(uint32 n) {
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return CountOnes(n) - 1;
}

inline uint64 Math::CountOnes(uint64 n) {
    n = n - ((n >> 1) & 0x5555555555555555ULL);
    n = ((n >> 2) & 0x3333333333333333ULL) + (n & 0x3333333333333333ULL);
    n = (n + (n >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    n = n + (n >> 8);
    n = n + (n >> 16);
    n = n + (n >> 32);
    return n & 0x7F;
}

inline uint64 Math::FloorLog2(uint64 n) {
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    return CountOnes(n) - 1;
}

// Line 198. Written from CameraState::operator==: the difference of the bit patterns (line 206),
// a reject of 0x80000000 (207) and a test of the magnitude bits (211), so equal within one ulp.
inline BOOL Math::FloatEqual(float x, float y) {
    uint32 dwDiff = *(uint32*)&x - *(uint32*)&y;
    if (dwDiff == 0x80000000)
        return FALSE;
    return (dwDiff & 0x7FFFFFFF) <= 1;
}

template <typename T>
inline void Math::Lerp(const T& t0, const T& t1, const float t, T& tResult) {
    tResult = (T)((t1 - t0) * t) + t0;  // original MathGeneral.inl:224; cast/sub/mul/add columns 18/23/29/34 (UI.cpp Lerp<uchar>, float CUs 20/23/25/29/34)
}

template <>
inline void Math::Lerp<int32>(const int32& t0, const int32& t1, const float t, int32& tResult) {
    tResult = FloatRoundToInt((t1 - t0) * t) + t0;
}

template <typename T>
inline T Math::Lerp(const T& t0, const T& t1, const float t) {
    T tResult;
    Lerp(t0, t1, t, tResult);
    return tResult;
}

inline float Math::SmoothStep(const float flT) {
    return flT * flT * (3.0f - 2.0f * flT);
}

// Line 279 (body line 284); written from the copies inlined into FixedViewController::Update.
inline float Math::SmootherStep(const float flT) {
    return flT * flT * flT * (flT * (flT * 6.0f - 15.0f) + 10.0f);
}

template <typename T>
inline T Math::SmoothStep(const T& t0, const T& t1, const float flT) {
    return Lerp(t0, t1, SmoothStep(flT));
}

// Body reconstructed from the original's line table (MathGeneral.inl:301-325).
inline int32 Math::FloatRoundToInt(float f) {
    if (Abs(f) > 8388607.0f) {
        if (f < 0.0f)
            return (int32)ceilf(f - 0.5f);
        else
            return FloatFloorToInt(f + 0.5f);
    }
    // Adding 2^23 leaves the rounded integer in the mantissa bits.
    if (f > 0.0f) {
        f += 8388608.0f;
        return *(int32*)&f & 0x7FFFFF;
    } else if (f < 0.0f) {
        f -= 8388608.0f;
        return -(*(int32*)&f & 0x7FFFFF);
    }
    return 0;
}

inline int32 Math::FloatFloorToInt(float f) {
    int32 xf = FloatToInt(f);
    return f < 0.0f ? xf - 1 : xf;
}

inline float Math::FloatTrunc(float f) {
    return (float)FloatToInt(f);
}

inline int32 Math::FloatToInt(float f) {
    return (int32)f;
}

// Line 388. The body is not in the DWARF; FloatTrunc is inlined at line 391.
inline float Math::FloatFrac(const float x) {
    return x - FloatTrunc(x);
}

// Line 397. The body is not in the DWARF; FloatTrunc is inlined at line 402.
inline float Math::FloatMod(const float x, const float y) {
    return x - FloatTrunc(x / y) * y;
}

// Line 408 (x 409, y 410, flMod 415). Body written from the three copies inlined into
// X/Client/Particle.cpp sGetTiledInstances: FloatMod at 415, the test at 417:6 and the add at
// 419:13. Inference: `flMod = y + flMod` is the tab-indented spelling whose `+` falls on column 13
// (`flMod += y` would put the add on column 9); both compile to the same bytes.
inline float Math::PositiveFloatMod(const float x, const float y) {
    float flMod = FloatMod(x, y);
    if (flMod < 0.0f)
        flMod = y + flMod;
    return flMod;
}

// Line 428. The body is not in the DWARF; its only local is DEG_TO_RAD.
inline Angle Math::DegToAngle(float flDeg) {
    const float DEG_TO_RAD = PI / 180.0f;
    return flDeg * DEG_TO_RAD;
}

// Original line 448 (Pin at :451, PinWrap at :453); inlined at RandomPoint.cpp:30.
inline Angle Math::NormalizeAngle(Angle a) {
    a = Pin(a, -8.0f * FULL_CIRCLE, 8.0f * FULL_CIRCLE);
    return PinWrap(a, 0.0f, FULL_CIRCLE);
}
// Original line 459 (aResult :464). b - a: in all eight inlined copies the DWARF location of `a` is the subtrahend.
inline Angle Math::SignedAngularDifference(Angle a, Angle b) {
    Angle aResult = NormalizeAngle(b - a);
    if (aResult > HALF_CIRCLE) {
        aResult -= FULL_CIRCLE;
    }
    return aResult;
}

inline Angle Math::AngularDifference(Angle a, Angle b) {
    return Abs(SignedAngularDifference(a, b));
}

// Original line 486.
inline float Math::Sin(Angle a) {
    return sinf(a);
}

// Original line 495.
inline float Math::Cos(Angle a) {
    return cosf(a);
}

inline float Math::Tan(Angle a) {
    return tanf(a);
}

// Original line 513 (the NaN guard and the `sqrtf` PLT fallback follow the `fsqrt`).
inline float Math::Sqrt(float s) {
    return sqrtf(s);
}

// Original line 438; RAD_TO_DEG is recorded in DWARF.
inline float Math::AngleToDeg(Angle aAng) {
    const float RAD_TO_DEG = 180.0f / PI;
    return aAng * RAD_TO_DEG;
}
