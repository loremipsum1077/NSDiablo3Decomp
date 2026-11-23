#pragma once

// min / max / clamp. clamp(v, lo, hi) forwards to clamp(v, lo, hi, less<T>())
// (line 310 in the binary).

#include "config.h"
#include "func_op.h"

namespace blz {

// line 21
template <typename T>
inline const T& min(const T& a, const T& b) {
    return b < a ? b : a;
}

template <typename T, typename Compare>
inline const T& min(const T& a, const T& b, Compare comp) {
    return comp(b, a) ? b : a;
}

// line 49
template <typename T>
inline const T& max(const T& a, const T& b) {
    return a < b ? b : a;
}

template <typename T, typename Compare>
inline const T& max(const T& a, const T& b, Compare comp) {
    return comp(a, b) ? b : a;
}

template <typename ForwardIt, typename Compare>
inline ForwardIt min_element(ForwardIt first, ForwardIt last, Compare comp) {
    if (first == last)
        return last;
    ForwardIt result = first;
    while (++first != last)
        if (comp(*first, *result))
            result = first;
    return result;
}

template <typename ForwardIt>
inline ForwardIt min_element(ForwardIt first, ForwardIt last) {
    return min_element(first, last, less<>());
}

template <typename ForwardIt, typename Compare>
inline ForwardIt max_element(ForwardIt first, ForwardIt last, Compare comp) {
    if (first == last)
        return last;
    ForwardIt result = first;
    while (++first != last)
        if (comp(*result, *first))
            result = first;
    return result;
}

template <typename ForwardIt>
inline ForwardIt max_element(ForwardIt first, ForwardIt last) {
    return max_element(first, last, less<>());
}

// line 296
template <typename T, typename Compare>
inline const T& clamp(const T& v, const T& lo, const T& hi, Compare comp) {
    return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

// line 308
template <typename T>
inline const T& clamp(const T& v, const T& lo, const T& hi) {
    return clamp(v, lo, hi, less<T>());
}

}  // namespace blz
