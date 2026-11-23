#pragma once

// blz::pair. Members and line order from the DWARF. The copy and move
// constructors are defaulted: is_trivially_copy_constructible<pair<int, int>>
// is true in the binary, so pair of trivial types is passed in registers.
// The converting constructors are constrained with
// enable_if<is_convertible<U1, T1>::value && is_convertible<U2, T2>::value>
// as a trailing pointer parameter (from the mangled names), and make_pair
// returns pair<unref_wrap<T1>::type, unref_wrap<T2>::type>.

#include "config.h"
#include "type_traits.h"
#include "util_swap.h"

namespace blz {

// line 39
template <typename T1, typename T2>
struct pair {
    typedef T1 first_type;
    typedef T2 second_type;

    T1 first;
    T2 second;

    // line 49
    pair() : first(), second() {}

    // line 55. constexpr (here and at line 82): X/Common/Player.cpp's arParagonPortraits[19] is
    // constant data in the binary although that file's static initializer runs at start-up.
    constexpr pair(const T1& x, const T2& y) : first(x), second(y) {}

    // line 62
    pair(const pair& p) = default;

    // line 72
    pair(pair&& p) = default;

    // line 82
    template <typename U1, typename U2>
    constexpr pair(U1&& x, U2&& y,
         typename enable_if<is_convertible<U1, T1>::value && is_convertible<U2, T2>::value>::type* = 0)
        : first(blz::forward<U1>(x)), second(blz::forward<U2>(y)) {}

    template <typename U1, typename U2>
    pair(const pair<U1, U2>& p,
         typename enable_if<is_convertible<const U1&, T1>::value &&
                            is_convertible<const U2&, T2>::value>::type* = 0)
        : first(p.first), second(p.second) {}

    // line 102
    template <typename U1, typename U2>
    pair(pair<U1, U2>&& p,
         typename enable_if<is_convertible<U1, T1>::value && is_convertible<U2, T2>::value>::type* = 0)
        : first(blz::forward<U1>(p.first)), second(blz::forward<U2>(p.second)) {}

    // line 124
    pair& operator=(const pair& p) {
        first = p.first;
        second = p.second;
        return *this;
    }

    // line 133
    template <typename U1, typename U2>
    pair& operator=(const pair<U1, U2>& p) {
        first = p.first;
        second = p.second;
        return *this;
    }

    // line 141
    pair& operator=(pair&& p) {
        first = blz::forward<T1>(p.first);
        second = blz::forward<T2>(p.second);
        return *this;
    }

    // line 150
    template <typename U1, typename U2>
    pair& operator=(pair<U1, U2>&& p) {
        first = blz::forward<U1>(p.first);
        second = blz::forward<U2>(p.second);
        return *this;
    }

    // line 158
    void swap(pair& p) {
        blz::swap(first, p.first);
        blz::swap(second, p.second);
    }
};

// line 172
template <typename T1, typename T2>
inline bool operator==(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return x.first == y.first && x.second == y.second;
}

template <typename T1, typename T2>
inline bool operator!=(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return !(x == y);
}

// line 186
template <typename T1, typename T2>
inline bool operator<(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return x.first < y.first || (!(y.first < x.first) && x.second < y.second);
}

template <typename T1, typename T2>
inline bool operator>(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return y < x;
}

template <typename T1, typename T2>
inline bool operator<=(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return !(y < x);
}

template <typename T1, typename T2>
inline bool operator>=(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return !(x < y);
}

template <typename T1, typename T2>
inline void swap(pair<T1, T2>& x, pair<T1, T2>& y) {
    x.swap(y);
}

template <typename T>
struct unref_wrap {
    typedef typename decay<T>::type type;
};

// line 222
template <typename T1, typename T2>
inline pair<typename unref_wrap<T1>::type, typename unref_wrap<T2>::type> make_pair(T1&& x, T2&& y) {
    return pair<typename unref_wrap<T1>::type, typename unref_wrap<T2>::type>(blz::forward<T1>(x),
                                                                              blz::forward<T2>(y));
}

}  // namespace blz
