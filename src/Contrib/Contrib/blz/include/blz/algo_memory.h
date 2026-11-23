#pragma once

// Construction / destruction helpers on raw memory. Signatures are the
// mangled ones from the binary. Callers pick the overload themselves by
// passing a trait object as the tag, e.g.
//   _uninitialized_move(first, last, result, typename is_trivially_move_constructible<T>::type());
//   _destroy(first, last, typename is_trivially_destructible<T>::type());
// (vector.h, temp_buffer.h and fixed_vector.h call these directly; there is
// no dispatching wrapper in between in the inline trees.)
//
// From the binary: _construct of a fundamental type is a memset of the range
// (no zero test); the non-fundamental one placement-news T() in a loop
// (line 36), which clang turns into a memset for trivial types.
// _uninitialized_copy/_move of T* with true_type compute the byte count
// (lines 119 / 147) and memmove (lines 120 / 148).

#include "config.h"
#include "iter_types.h"
#include "type_traits.h"

#include <new>
#include <string.h>

namespace blz { struct placement_t {}; }
inline void* operator new(size_t, blz::placement_t, void* p) { return p; }
inline void operator delete(void*, blz::placement_t, void*) {}
namespace blz {

// line 26
template <typename T>
inline typename enable_if<is_fundamental<T>::value>::type _construct(T* first, T* last) {
    memset(first, 0, (last - first) * sizeof(T));
}

// line 33
template <typename T>
inline typename enable_if<!is_fundamental<T>::value>::type _construct(T* first, T* last) {
    for (; first != last; ++first)
        ::new (blz::placement_t(), first) T();
}

// line 41
template <typename T>
inline void _copy_construct(T* p, const T& val, true_type) {
    *p = val;
}

// line 48
template <typename T>
inline void _copy_construct(T* p, const T& val, false_type) {
    ::new (blz::placement_t(), p) T(val);
}

// line 55
template <typename T>
inline void _move_construct(T* p, T&& val, true_type) {
    *p = blz::move(val);
}

// line 62
template <typename T>
inline void _move_construct(T* p, T&& val, false_type) {
    ::new (blz::placement_t(), p) T(blz::move(val));
}

template <typename T>
inline void _destroy(T, true_type) {}

// line 75
template <typename T>
inline void _destroy(T p, false_type) {
    typedef typename iterator_traits<T>::value_type value_type;
    p->~value_type();
}

template <typename T>
inline void _destroy(T, T, true_type) {}

// line 89
template <typename T>
inline void _destroy(T first, T last, false_type) {
    typedef typename iterator_traits<T>::value_type value_type;
    for (; first != last; ++first)
        first->~value_type();
}

// line 98
template <typename ForwardIt, typename T>
inline void _uninitialized_fill(ForwardIt first, ForwardIt last, const T& value) {
    typedef typename iterator_traits<ForwardIt>::value_type value_type;
    for (; first != last; ++first)
        ::new (blz::placement_t(), &*first) value_type(value);
}

template <typename ForwardIt, typename Size, typename T>
inline ForwardIt _uninitialized_fill_n(ForwardIt first, Size n, const T& value) {
    typedef typename iterator_traits<ForwardIt>::value_type value_type;
    for (; n > 0; --n, ++first)
        ::new (blz::placement_t(), &*first) value_type(value);
    return first;
}

// line 117
template <typename T>
inline T* _uninitialized_copy(T* first, T* last, T* result, true_type) {
    size_t n = last - first;
    memmove(result, first, n * sizeof(T));
    return result + n;
}

// line 126
template <typename InputIt, typename OutputIt>
inline OutputIt _uninitialized_copy(InputIt first, InputIt last, OutputIt result, true_type) {
    typedef typename iterator_traits<OutputIt>::value_type value_type;
    for (; first != last; ++result, ++first)
        ::new (blz::placement_t(), &*result) value_type(*first);
    return result;
}

// line 135
template <typename InputIt, typename OutputIt>
inline OutputIt _uninitialized_copy(InputIt first, InputIt last, OutputIt result, false_type) {
    typedef typename iterator_traits<OutputIt>::value_type value_type;
    for (; first != last; ++result, ++first)
        ::new (blz::placement_t(), &*result) value_type(*first);
    return result;
}

// line 145
template <typename T>
inline T* _uninitialized_move(T* first, T* last, T* result, true_type) {
    size_t n = last - first;
    memmove(result, first, n * sizeof(T));
    return result + n;
}

template <typename InputIt, typename OutputIt>
inline OutputIt _uninitialized_move(InputIt first, InputIt last, OutputIt result, true_type) {
    typedef typename iterator_traits<OutputIt>::value_type value_type;
    for (; first != last; ++result, ++first)
        ::new (blz::placement_t(), &*result) value_type(blz::move(*first));
    return result;
}

// line 163
template <typename InputIt, typename OutputIt>
inline OutputIt _uninitialized_move(InputIt first, InputIt last, OutputIt result, false_type) {
    typedef typename iterator_traits<OutputIt>::value_type value_type;
    for (; first != last; ++result, ++first)
        ::new (blz::placement_t(), &*result) value_type(blz::move(*first));
    return result;
}

}  // namespace blz
