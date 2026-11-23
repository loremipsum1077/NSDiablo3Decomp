#pragma once

// Mutating sequence algorithms. From the binary (objdump -l line info):
// copy is a plain loop (loop at line 25, assignment at line 26; clang turns it
// into an inline copy loop, not a memmove); swap_ranges calls swap at line 83;
// fill calls _fill(tag) at line 194 and the random access _fill calls fill_n
// at line 186; remove_if tests the predicate at lines 241 and 249;
// random_shuffle walks backwards from last - 1 calling swap(*i, first[rand() %
// count]) at line 502.

#include "config.h"
#include "iter_types.h"
#include "type_traits.h"
#include "util_swap.h"

#include <stdlib.h>

namespace blz {

// line 23
template <typename InputIt, typename OutputIt>
inline OutputIt copy(InputIt first, InputIt last, OutputIt result) {
    for (; first != last; ++result, ++first)
        *result = *first;
    return result;
}

template <typename InputIt, typename Size, typename OutputIt>
inline OutputIt copy_n(InputIt first, Size n, OutputIt result) {
    for (; n > 0; --n, ++first, ++result)
        *result = *first;
    return result;
}

template <typename InputIt, typename OutputIt, typename Pred>
inline OutputIt copy_if(InputIt first, InputIt last, OutputIt result, Pred pred) {
    for (; first != last; ++first) {
        if (pred(*first)) {
            *result = *first;
            ++result;
        }
    }
    return result;
}

template <typename BidirIt1, typename BidirIt2>
inline BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 result) {
    while (first != last)
        *--result = *--last;
    return result;
}

// line 62. Not declared inline: the original has a concrete out-of-line instance (move<unordered_map<EventKey,...>*>, 0x71006b1f90, called from vector::erase).
template <typename InputIt, typename OutputIt>
OutputIt move(InputIt first, InputIt last, OutputIt result) {
    for (; first != last; ++result, ++first)  // increment order from the line table: ++result at column 31, ++first at 41 (0x71006b1fe8)
        *result = blz::move(*first);
    return result;
}

// line 71
template <typename BidirIt1, typename BidirIt2>
inline BidirIt2 move_backward(BidirIt1 first, BidirIt1 last, BidirIt2 result) {
    while (first != last)
        *--result = blz::move(*--last);
    return result;
}

// line 80
template <typename ForwardIt1, typename ForwardIt2>
inline ForwardIt2 swap_ranges(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2) {
    for (; first1 != last1; ++first1, ++first2)
        swap(*first1, *first2);
    return first2;
}

template <typename ForwardIt1, typename ForwardIt2>
inline void iter_swap(ForwardIt1 a, ForwardIt2 b) {
    swap(*a, *b);
}

// line 96
template <typename InputIt, typename OutputIt, typename UnaryOp>
inline OutputIt transform(InputIt first, InputIt last, OutputIt result, UnaryOp op) {
    for (; first != last; ++first, ++result)
        *result = op(*first);
    return result;
}

template <typename InputIt1, typename InputIt2, typename OutputIt, typename BinaryOp>
inline OutputIt transform(InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt result,
                          BinaryOp op) {
    for (; first1 != last1; ++first1, ++first2, ++result)
        *result = op(*first1, *first2);
    return result;
}

template <typename ForwardIt, typename T>
inline void replace(ForwardIt first, ForwardIt last, const T& old_value, const T& new_value) {
    for (; first != last; ++first)
        if (*first == old_value)
            *first = new_value;
}

template <typename ForwardIt, typename Pred, typename T>
inline void replace_if(ForwardIt first, ForwardIt last, Pred pred, const T& new_value) {
    for (; first != last; ++first)
        if (pred(*first))
            *first = new_value;
}

// line 154
template <typename OutputIt, typename Size, typename T>
inline OutputIt fill_n(OutputIt first, Size n, const T& value) {
    for (; n > 0; --n, ++first)
        *first = value;
    return first;
}

template <typename ForwardIt, typename T>
inline void _fill(ForwardIt first, ForwardIt last, const T& value, forward_iterator_tag) {
    for (; first != last; ++first)
        *first = value;
}

// line 184
template <typename RandomIt, typename T>
inline void _fill(RandomIt first, RandomIt last, const T& value, random_access_iterator_tag) {
    fill_n(first, last - first, value);
}

// line 191
template <typename ForwardIt, typename T>
inline void fill(ForwardIt first, ForwardIt last, const T& value) {
    _fill(first, last, value, typename iterator_traits<ForwardIt>::iterator_category());
}

template <typename ForwardIt, typename Generator>
inline void generate(ForwardIt first, ForwardIt last, Generator gen) {
    for (; first != last; ++first)
        *first = gen();
}

template <typename ForwardIt, typename T>
inline ForwardIt remove(ForwardIt first, ForwardIt last, const T& value) {
    for (;;) {
        if (first == last)
            return first;
        if (*first == value)
            break;
        ++first;
    }
    for (ForwardIt i = first; ++i != last;) {
        if (!(*i == value)) {
            *first = blz::move(*i);
            ++first;
        }
    }
    return first;
}

// line 235
template <typename ForwardIt, typename Pred>
inline ForwardIt remove_if(ForwardIt first, ForwardIt last, Pred pred) {
    for (;;) {
        if (first == last)
            return first;
        if (pred(*first))
            break;
        ++first;
    }
    for (ForwardIt i = first; ++i != last;) {
        if (!pred(*i)) {
            // Original DWARF: postfix operator++(int), operator* and operator= all inlined at line 250.
            *first++ = blz::move(*i);
        }
    }
    return first;
}

template <typename ForwardIt>
inline ForwardIt unique(ForwardIt first, ForwardIt last) {
    if (first == last)
        return last;
    ForwardIt result = first;
    while (++first != last) {
        if (!(*result == *first))
            *++result = blz::move(*first);
    }
    return ++result;
}

template <typename BidirIt>
inline void reverse(BidirIt first, BidirIt last) {
    while (first != last && first != --last) {
        swap(*first, *last);
        ++first;
    }
}

// line 496
// Locals from the DWARF (sQuestsPickBounties CU): typedef D (line 498), D n (line 500),
// D i (line 501); the loop counts i down from n - 1 and draws rand() % (i + 1).
template <typename RAIter>
inline void random_shuffle(RAIter first, RAIter last) {
    typedef typename iterator_traits<RAIter>::difference_type D;
    D n = last - first;
    for (D i = n - 1; i > 0; --i)
        swap(first[i], first[rand() % (i + 1)]);
}

}  // namespace blz
