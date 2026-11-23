#pragma once

// blz::numeric_limits. Class layout and member values are from the DWARF
// (numeric_limits_integer_base_ / _signed_ / _unsigned_ instantiated for
// short..unsigned long long, numeric_limits<double>). The function bodies are
// the obvious ones; none of them leave code of their own in the binary.

#include "config.h"

namespace blz {

// line 18
enum float_denorm_style {
    denorm_indeterminate = -1,
    denorm_absent = 0,
    denorm_present = 1,
};

// line 26
enum float_round_style {
    round_indeterminate = -1,
    round_toward_zero = 0,
    round_to_nearest = 1,
    round_toward_infinity = 2,
    round_toward_neg_infinity = 3,
};

template <typename T>
class numeric_limits {
public:
    static const bool is_specialized = false;
    static T min() { return T(); }
    static T max() { return T(); }
    static T lowest() { return T(); }
};

// line 86
template <typename T>
class numeric_limits_integer_base_ {
public:
    static const bool is_integer = true;
    static const bool is_exact = true;

    static const int radix = 2;

    static const bool has_infinity = false;
    static const bool has_quiet_NaN = false;
    static const bool has_signaling_NaN = false;
    static const bool has_denorm_loss = false;
    static const bool is_iec559 = false;
    static const bool is_bounded = true;
    static const bool traps = true;
    static const bool tinyness_before = false;

    static const float_denorm_style has_denorm = denorm_absent;
    static const float_round_style round_style = round_toward_zero;

    static T epsilon() { return 0; }
    static T round_error() { return 0; }

    static T infinity() { return 0; }
    static T quiet_NaN() { return 0; }
    static T signaling_NaN() { return 0; }
    static T denorm_min() { return 0; }

    static const bool is_specialized = true;
    static const bool is_signed = T(-1) < T(0);
    static const bool is_modulo = !is_signed;
    static const int digits = static_cast<int>(sizeof(T) * 8 - is_signed);
    static const int digits10 = digits * 301 / 1000;
    static const int max_digits10 = 0;
};

// line 133
template <typename T, typename U>
class numeric_limits_integer_signed_ : public numeric_limits_integer_base_<T> {
public:
    static T min() { return min_val; }
    static T max() { return max_val; }
    static T lowest() { return min_val; }

    static const T min_val = static_cast<T>(static_cast<U>(1) << (sizeof(T) * 8 - 1));
    static const T max_val = static_cast<T>(~(static_cast<U>(1) << (sizeof(T) * 8 - 1)));
};

// line 148
template <typename T>
class numeric_limits_integer_unsigned_ : public numeric_limits_integer_base_<T> {
public:
    static T min() { return min_val; }
    static T max() { return max_val; }
    static T lowest() { return min_val; }

    static const T min_val = 0;
    static const T max_val = static_cast<T>(~static_cast<T>(0));
};

template <> class numeric_limits<bool> : public numeric_limits_integer_unsigned_<bool> {};
template <> class numeric_limits<char> : public numeric_limits_integer_signed_<char, unsigned char> {};
template <> class numeric_limits<signed char> : public numeric_limits_integer_signed_<signed char, unsigned char> {};
template <> class numeric_limits<unsigned char> : public numeric_limits_integer_unsigned_<unsigned char> {};
template <> class numeric_limits<wchar_t> : public numeric_limits_integer_unsigned_<wchar_t> {};
template <> class numeric_limits<short> : public numeric_limits_integer_signed_<short, unsigned short> {};
template <> class numeric_limits<unsigned short> : public numeric_limits_integer_unsigned_<unsigned short> {};
template <> class numeric_limits<int> : public numeric_limits_integer_signed_<int, unsigned int> {};
template <> class numeric_limits<unsigned int> : public numeric_limits_integer_unsigned_<unsigned int> {};
template <> class numeric_limits<long> : public numeric_limits_integer_signed_<long, unsigned long> {};
template <> class numeric_limits<unsigned long> : public numeric_limits_integer_unsigned_<unsigned long> {};
template <> class numeric_limits<long long> : public numeric_limits_integer_signed_<long long, unsigned long long> {};
template <> class numeric_limits<unsigned long long> : public numeric_limits_integer_unsigned_<unsigned long long> {};

template <>
class numeric_limits<float> {
public:
    static const bool is_specialized = true;
    static const bool is_signed = true;
    static const bool is_integer = false;
    static const bool is_exact = false;
    static const bool is_modulo = false;
    static const int digits = __FLT_MANT_DIG__;
    static const int digits10 = __FLT_DIG__;
    static const int max_digits10 = 9;
    static const int radix = __FLT_RADIX__;
    static const int min_exponent = __FLT_MIN_EXP__;
    static const int min_exponent10 = __FLT_MIN_10_EXP__;
    static const int max_exponent = __FLT_MAX_EXP__;
    static const int max_exponent10 = __FLT_MAX_10_EXP__;
    static const bool has_infinity = true;
    static const bool has_quiet_NaN = true;
    static const bool has_signaling_NaN = true;
    static const bool has_denorm_loss = true;
    static const bool is_iec559 = true;
    static const bool is_bounded = true;
    static const bool traps = false;
    static const bool tinyness_before = true;
    static const float_denorm_style has_denorm = denorm_present;
    static const float_round_style round_style = round_to_nearest;

    static float min() { return __FLT_MIN__; }
    static float max() { return __FLT_MAX__; }
    static float lowest() { return -__FLT_MAX__; }
    static float epsilon() { return __FLT_EPSILON__; }
    static float round_error() { return 0.5f; }

    static float infinity() { return __builtin_huge_valf(); }
    static float quiet_NaN() { return __builtin_nanf(""); }
    static float signaling_NaN() { return __builtin_nansf(""); }
    static float denorm_min() { return __FLT_DENORM_MIN__; }
};

// line 321
template <>
class numeric_limits<double> {
public:
    static const bool is_specialized = true;
    static const bool is_signed = true;
    static const bool is_integer = false;
    static const bool is_exact = false;
    static const bool is_modulo = false;
    static const int digits = 53;
    static const int digits10 = 15;
    static const int max_digits10 = 17;
    static const int radix = 2;
    static const int min_exponent = -1021;
    static const int min_exponent10 = -307;
    static const int max_exponent = 1024;
    static const int max_exponent10 = 308;
    static const bool has_infinity = true;
    static const bool has_quiet_NaN = true;
    static const bool has_signaling_NaN = true;
    static const bool has_denorm_loss = true;
    static const bool is_iec559 = true;
    static const bool is_bounded = true;
    static const bool traps = false;
    static const bool tinyness_before = true;
    static const float_denorm_style has_denorm = denorm_present;
    static const float_round_style round_style = round_to_nearest;

    static double min() { return __DBL_MIN__; }
    static double max() { return __DBL_MAX__; }
    static double lowest() { return -__DBL_MAX__; }
    static double epsilon() { return __DBL_EPSILON__; }
    static double round_error() { return 0.5; }

    static double infinity() { return __builtin_huge_val(); }
    static double quiet_NaN() { return __builtin_nan(""); }
    static double signaling_NaN() { return __builtin_nans(""); }
    static double denorm_min() { return __DBL_DENORM_MIN__; }
};

}  // namespace blz
