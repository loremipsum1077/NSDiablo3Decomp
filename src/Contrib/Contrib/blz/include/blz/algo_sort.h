#pragma once

// Sorting: heap-backed partial_sort, introsort (median-of-three quicksort that
// falls back to partial_sort when the depth budget runs out, finished by one
// insertion sort over the whole range; the loop stops at 32 elements), a
// merge sort that uses a scratch buffer, and the entry points sort and
// stable_sort. Shape and local names are the DWARF's, the algorithms are the
// classic SGI/libstdc++ ones; statement order inside each follows the binary.
//
// Mangled template parameter order:
//   partial_sort<It, Compare>(first, middle, last, comp)
//   internal::introsort_loop<It, Size, Compare>(first, last, limit, comp)
//   internal::median<T, Compare>(T a, T b, T c, Compare)       (T is the reference type)
//   internal::partition<It, T, Compare>(first, last, T pivot, comp)
//   merge_sort_buffer<It, T, Compare>(first, last, T* buffer, comp)

#include "config.h"
#include "algo_heap.h"
#include "algo_merge.h"
#include "func_op.h"
#include "iter_types.h"
#include "temp_buffer.h"
#include "util_swap.h"

namespace blz {

// line 21
template <typename RandomIt, typename Compare>
void partial_sort(RandomIt first, RandomIt middle, RandomIt last, Compare comp) {
    typedef typename iterator_traits<RandomIt>::difference_type D;
    typedef typename iterator_traits<RandomIt>::value_type T;
    make_heap(first, middle, comp);
    for (RandomIt i = middle; i < last; ++i) {
        if (comp(*i, *first)) {
            T tmp = blz::move(*i);
            *i = blz::move(*first);
            internal::adjust_heap(first, D(0), D(middle - first), blz::move(tmp), comp);
        }
    }
    sort_heap(first, middle, comp);
}

namespace internal {

// line 140
template <typename T>
T lg_2(T n) {
    T k;
    for (k = 0; n != 1; n >>= 1)
        ++k;
    return k;
}

// line 150
template <typename T, typename Compare>
const T median(T a, T b, T c, Compare comp) {
    if (comp(a, b)) {
        if (comp(b, c))
            return b;
        return comp(a, c) ? c : a;
    }
    if (comp(a, c))
        return a;
    return comp(b, c) ? c : b;
}

// line 171
template <typename RandomIt, typename T, typename Compare>
RandomIt partition(RandomIt first, RandomIt last, T pivot, Compare comp) {
    while (true) {
        while (comp(*first, pivot))
            ++first;
        --last;
        while (comp(pivot, *last))
            --last;
        if (!(first < last))
            return first;
        blz::swap(*first, *last);
        ++first;
    }
}

// line 192
template <typename RandomIt, typename Size, typename Compare>
void introsort_loop(RandomIt first, RandomIt last, Size limit, Compare comp) {
    typedef typename iterator_traits<RandomIt>::reference TRef;
    while (last - first > 32) {
        if (limit == 0) {
            partial_sort(first, last, last, comp);
            return;
        }
        {
            TRef pivot = median<TRef>(*first, *(first + (last - first) / 2), *(last - 1), comp);
            RandomIt p = partition(first, last, pivot, comp);
            introsort_loop(p, last, --limit, comp);
            last = p;
        }
    }
}

}  // namespace internal

// line 214
template <typename RandomIt, typename Compare>
void insertion_sort(RandomIt first, RandomIt last, Compare comp) {
    typedef typename iterator_traits<RandomIt>::value_type T;
    if (first == last)
        return;
    for (RandomIt sorted = first + 1; sorted != last; ++sorted) {
        T tmp = *sorted;
        RandomIt next = sorted;
        while (next != first && comp(tmp, *(next - 1))) {
            *next = *(next - 1);
            --next;
        }
        *next = tmp;
    }
}

// line 264
template <typename RandomIt, typename Compare>
void introsort(RandomIt first, RandomIt last, Compare comp) {
    if (first != last) {
        internal::introsort_loop(first, last, internal::lg_2(last - first) * 2, comp);
        insertion_sort(first, last, comp);
    }
}

// line 289
template <typename RandomIt, typename T, typename Compare>
void merge_sort_buffer(RandomIt first, RandomIt last, T* buffer, Compare comp) {
    typedef typename iterator_traits<RandomIt>::difference_type D;
    D len = last - first;
    if (len <= 32) {
        insertion_sort(first, last, comp);
        return;
    }
    D offset_mid = len / 2;
    D len1 = offset_mid;
    RandomIt mid = first + offset_mid;
    {
        D offset_mid1 = len1 / 2;
        RandomIt mid1 = first + offset_mid1;
        merge_sort_buffer(first, mid1, buffer, comp);
        merge_sort_buffer(mid1, mid, buffer + offset_mid1, comp);
        merge(first, mid1, mid1, mid, buffer, comp);
    }
    D len2 = last - mid;
    if (len2 > 1) {
        D offset_mid2 = (len + len2) / 2;
        RandomIt mid2 = first + offset_mid2;
        merge_sort_buffer(mid, mid2, buffer + offset_mid, comp);
        merge_sort_buffer(mid2, last, buffer + offset_mid2, comp);
        merge(mid, mid2, mid2, last, buffer + offset_mid, comp);
    } else {
        buffer[offset_mid] = *mid;
    }
    merge(buffer, buffer + offset_mid, buffer + offset_mid, buffer + len, first, comp);
}

// line 333
template <typename RandomIt, typename Compare>
void merge_sort(RandomIt first, RandomIt last, Compare comp) {
    typedef typename iterator_traits<RandomIt>::value_type T;
    if (last - first > 1) {
        temp_buffer<T> buffer(last - first);
        merge_sort_buffer(first, last, buffer.data(), comp);
    }
}

template <typename RandomIt, typename Compare>
void sort(RandomIt first, RandomIt last, Compare comp);

// line 355
template <typename RandomIt>
void sort(RandomIt first, RandomIt last) {
    sort(first, last, less<void>());
}

// line 362
template <typename RandomIt, typename Compare>
void sort(RandomIt first, RandomIt last, Compare comp) {
    introsort(first, last, comp);
}

// line 376
template <typename RandomIt, typename Compare>
void stable_sort(RandomIt first, RandomIt last, Compare comp) {
    merge_sort(first, last, comp);
}

}  // namespace blz
