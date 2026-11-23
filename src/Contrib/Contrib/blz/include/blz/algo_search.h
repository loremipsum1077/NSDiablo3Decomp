#pragma once

// Binary searches on sorted ranges. From the binary (objdump -l):
// binary_search(value) forwards to binary_search(value, less<>()) at line
// 128; the comparator version loops `while (first != last)` (line 107), takes
// `mid` = first advanced by distance(first, last) / 2 (line 110), tests
// comp(*mid, value) (line 111) and moves first past mid (line 114), then
// tests comp(value, *mid) and returns true when neither holds.

#include "config.h"
#include "func_op.h"
#include "iter_op.h"
#include "iter_types.h"

namespace blz {

template <typename ForwardIt, typename T, typename Compare>
inline ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& value, Compare comp) {
    typename iterator_traits<ForwardIt>::difference_type len = distance(first, last);
    while (len > 0) {
        typename iterator_traits<ForwardIt>::difference_type half = len / 2;
        ForwardIt mid = first;
        advance(mid, half);
        if (comp(*mid, value)) {
            first = ++mid;
            len -= half + 1;
        } else {
            len = half;
        }
    }
    return first;
}

template <typename ForwardIt, typename T>
inline ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& value) {
    return lower_bound(first, last, value, less<>());
}

template <typename ForwardIt, typename T, typename Compare>
inline ForwardIt upper_bound(ForwardIt first, ForwardIt last, const T& value, Compare comp) {
    typename iterator_traits<ForwardIt>::difference_type len = distance(first, last);
    while (len > 0) {
        typename iterator_traits<ForwardIt>::difference_type half = len / 2;
        ForwardIt mid = first;
        advance(mid, half);
        if (!comp(value, *mid)) {
            first = ++mid;
            len -= half + 1;
        } else {
            len = half;
        }
    }
    return first;
}

template <typename ForwardIt, typename T>
inline ForwardIt upper_bound(ForwardIt first, ForwardIt last, const T& value) {
    return upper_bound(first, last, value, less<>());
}

// line 105
template <typename ForwardIt, typename T, typename Compare>
inline bool binary_search(ForwardIt first, ForwardIt last, const T& value, Compare comp) {
    while (first != last) {
        ForwardIt mid = first;
        advance(mid, distance(first, last) / 2);
        if (comp(*mid, value))
            first = ++mid;
        else if (comp(value, *mid))
            last = mid;
        else
            return true;
    }
    return false;
}

// line 126
template <typename ForwardIt, typename T>
inline bool binary_search(ForwardIt first, ForwardIt last, const T& value) {
    return binary_search(first, last, value, less<>());
}

}  // namespace blz
