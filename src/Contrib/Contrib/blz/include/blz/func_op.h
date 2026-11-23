#pragma once

// blz function objects. The DWARF records equal_to<T> (line 161), less<T>
// (line 233) and less<void> (line 243, a transparent comparator whose
// operator() is mangled with return type decltype(static_cast<T>(a) <
// static_cast<U>(b))). The arithmetic functors before them and the other
// comparisons are not recorded and are written in the same shape.

#include "config.h"

namespace blz {

template <typename T = void>
struct plus {
    T operator()(const T& a, const T& b) const { return a + b; }
};

template <typename T = void>
struct minus {
    T operator()(const T& a, const T& b) const { return a - b; }
};

template <typename T = void>
struct multiplies {
    T operator()(const T& a, const T& b) const { return a * b; }
};

template <typename T = void>
struct divides {
    T operator()(const T& a, const T& b) const { return a / b; }
};

template <typename T = void>
struct modulus {
    T operator()(const T& a, const T& b) const { return a % b; }
};

template <typename T = void>
struct negate {
    T operator()(const T& a) const { return -a; }
};

// line 161
template <typename T = void>
struct equal_to {
    bool operator()(const T& a, const T& b) const {
        return a == b;
    }
};

template <>
struct equal_to<void> {
    template <typename T, typename U>
    auto operator()(T&& a, U&& b) const -> decltype(static_cast<T>(a) == static_cast<U>(b)) {
        return static_cast<T>(a) == static_cast<U>(b);
    }
};

template <typename T = void>
struct not_equal_to {
    bool operator()(const T& a, const T& b) const {
        return a != b;
    }
};

template <>
struct not_equal_to<void> {
    template <typename T, typename U>
    auto operator()(T&& a, U&& b) const -> decltype(static_cast<T>(a) != static_cast<U>(b)) {
        return static_cast<T>(a) != static_cast<U>(b);
    }
};

template <typename T = void>
struct greater {
    bool operator()(const T& a, const T& b) const {
        return a > b;
    }
};

template <>
struct greater<void> {
    template <typename T, typename U>
    auto operator()(T&& a, U&& b) const -> decltype(static_cast<T>(a) > static_cast<U>(b)) {
        return static_cast<T>(a) > static_cast<U>(b);
    }
};

// line 233
template <typename T = void>
struct less {
    bool operator()(const T& a, const T& b) const {
        return a < b;
    }
};

// line 243
template <>
struct less<void> {
    template <typename T, typename U>
    auto operator()(T&& a, U&& b) const -> decltype(static_cast<T>(a) < static_cast<U>(b)) {
        return static_cast<T>(a) < static_cast<U>(b);
    }
};

template <typename T = void>
struct greater_equal {
    bool operator()(const T& a, const T& b) const {
        return a >= b;
    }
};

template <>
struct greater_equal<void> {
    template <typename T, typename U>
    auto operator()(T&& a, U&& b) const -> decltype(static_cast<T>(a) >= static_cast<U>(b)) {
        return static_cast<T>(a) >= static_cast<U>(b);
    }
};

template <typename T = void>
struct less_equal {
    bool operator()(const T& a, const T& b) const {
        return a <= b;
    }
};

template <>
struct less_equal<void> {
    template <typename T, typename U>
    auto operator()(T&& a, U&& b) const -> decltype(static_cast<T>(a) <= static_cast<U>(b)) {
        return static_cast<T>(a) <= static_cast<U>(b);
    }
};

template <typename T = void>
struct logical_and {
    bool operator()(const T& a, const T& b) const { return a && b; }
};

template <typename T = void>
struct logical_or {
    bool operator()(const T& a, const T& b) const { return a || b; }
};

template <typename T = void>
struct logical_not {
    bool operator()(const T& a) const { return !a; }
};

}  // namespace blz
