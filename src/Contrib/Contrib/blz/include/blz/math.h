#pragma once

// blz floating point classification (floatclassify is out of line in
// blz/src/math.cpp).

#include "config.h"

namespace blz {

// line 50
enum {
    FP_CLASS_SUBNORMAL = -2,
    FP_CLASS_NORMAL = -1,
    FP_CLASS_ZERO = 0,
    FP_CLASS_INFINITE = 1,
    FP_CLASS_QNAN = 2,
    FP_CLASS_SNAN = 3,
    FP_CLASS_INDEFINITE = 4,
};

int floatclassify(double x);

}  // namespace blz
