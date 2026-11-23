#pragma once

// IEEE-754 views of float and double. Layout from the DWARF.

#include "config.h"

namespace blz {

// line 20
typedef float float32_t;
typedef double float64_t;

// line 46
union F32 {
    float32_t f;
    uint32_t ui;
    int32_t si;
    struct {
        unsigned int mantissa : 23;
        unsigned int exp : 8;
        unsigned int sign : 1;
    } bits;
};
static_assert(sizeof(F32) == 0x4, "F32");

// line 66
union F64 {
    float64_t f;
    uint64_t ui;
    int64_t si;
    struct {
        uint32_t mantissa_lo;
        unsigned int mantissa_hi : 20;
        unsigned int exp : 11;
        unsigned int sign : 1;
    } bits;
};
static_assert(sizeof(F64) == 0x8, "F64");

}  // namespace blz
