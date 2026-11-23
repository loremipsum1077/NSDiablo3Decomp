#include "../include/blz/math.h"
#include "../include/blz/float.h"

namespace blz {

// line 111
int floatclassify(double x) {
    F64 ieee;
    ieee.f = x;

    const unsigned int F64_EXPONENT_MAX = 0x7ff;
    const unsigned int F64_MANTISSA_HI_HIGH_BIT = 0x80000;

    unsigned int e = ieee.bits.exp;
    if (e == 0) {
        if (ieee.bits.mantissa_hi == 0 && ieee.bits.mantissa_lo == 0)
            return FP_CLASS_ZERO;
        return FP_CLASS_SUBNORMAL;
    }
    if (e == F64_EXPONENT_MAX) {
        if (ieee.bits.mantissa_hi == 0 && ieee.bits.mantissa_lo == 0)
            return FP_CLASS_INFINITE;
        if (ieee.bits.mantissa_hi & F64_MANTISSA_HI_HIGH_BIT) {
            if (ieee.bits.mantissa_hi == F64_MANTISSA_HI_HIGH_BIT && ieee.bits.mantissa_lo == 0 && ieee.bits.sign)
                return FP_CLASS_INDEFINITE;
            return FP_CLASS_QNAN;
        }
        return FP_CLASS_SNAN;
    }
    return FP_CLASS_NORMAL;
}

}  // namespace blz
