#include "MathGeneral.h"

const float cgflSqrt2 = 1.41421356237f;
const float cgflSqrt3 = 1.73205080757f;
const float cgflSqrt2Over2 = 0.70710678118f;

inline uint32 Math::MostSignificantBit(uint32 dw) {
    return LogBase2(dw);
}

uint32 Math::LogBase2(uint32 dw) {
    uint32 dwCount = 0;
    while (dw > 1) {
        dw >>= 1;
        dwCount++;
    }
    return dwCount;
}

uint32 Math::Ceiling2Power(uint32 dw) {
    if (dw == 0 || Is2Power(dw))
        return dw;
    return 2 << MostSignificantBit(dw);
}

inline BOOL Math::Is2Power(uint32 dw) {
    return dw != 0 && (dw == 1 || (dw & (dw - 1)) == 0);
}

uint32 Math::NextLarger2Power(uint32 dw) {
    if (Is2Power(dw))
        return dw;
    dw |= dw >> 1;
    dw |= dw >> 2;
    dw |= dw >> 4;
    dw |= dw >> 8;
    dw |= dw >> 16;
    return dw + 1;
}

static inline Angle sFastACos(float x) {
    x = Math::Pin(x, 0.0f, 1.0f);
    float fRoot = Math::Sqrt(1.0f - x);
    float fResult = -0.0187293f;
    fResult *= x;
    fResult += 0.0742610f;
    fResult *= x;
    fResult -= 0.2121144f;
    fResult *= x;
    fResult += 1.5707288f;
    fResult *= fRoot;
    return fResult;
}

Angle Math::ACos(const float x) {
    if (x < 0.0f) {
        return PI - sFastACos(-x);
    }
    return sFastACos(x);
}

Angle Math::ASin(const float x) {
    if (x < 0.0f) {
        return NormalizeAngle(-(QUARTER_CIRCLE - sFastACos(-x)));
    }
    return QUARTER_CIRCLE - sFastACos(x);
}

static inline Angle sApproxATan(float fT) {
    float fTSqr = fT * fT;
    float fResult = 0.0208351f;
    fResult *= fTSqr;
    fResult -= 0.085133f;
    fResult *= fTSqr;
    fResult += 0.180141f;
    fResult *= fTSqr;
    fResult -= 0.3302995f;
    fResult *= fTSqr;
    fResult += 0.999866f;
    fResult *= fT;
    return fResult;
}

Angle Math::ATan(float y, float x) {
    Angle aResult;
    if (Abs(x) > EPSILON) {
        float fT = y / x;
        if (fT > 1.0f) {
            aResult = QUARTER_CIRCLE - sApproxATan(1.0f / fT);
        } else if (fT < -1.0f) {
            aResult = -QUARTER_CIRCLE - sApproxATan(1.0f / fT);
        } else {
            aResult = sApproxATan(fT);
        }
        if (x < 0.0f) {
            aResult -= PI;
        }
        return NormalizeAngle(aResult);
    }
    if (y > 0.0f) {
        aResult = QUARTER_CIRCLE;
    } else {
        aResult = 3.0f * QUARTER_CIRCLE;
    }
    return aResult;
}
