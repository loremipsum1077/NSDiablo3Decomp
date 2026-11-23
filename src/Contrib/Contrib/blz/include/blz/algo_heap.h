#pragma once

// Binary heap helpers (the shape of the classic SGI/libstdc++ ones). Recorded
// in line order: internal::push_heap (20), internal::adjust_heap (34),
// make_heap (105), sort_heap (133). Locals are the DWARF's: adjust_heap has
// `childIndex` and `topIndex`, make_heap `len`, `parent` and a block `v`,
// sort_heap a block `v`. Mangled template parameter order (see the binary):
//   push_heap<It, Distance, T, Compare>(first, holeIndex, topIndex, T&& v, comp)
//   adjust_heap<It, Distance, T, Compare>(first, holeIndex, len, T&& v, comp)

#include "config.h"
#include "iter_types.h"
#include "type_traits.h"

namespace blz {

namespace internal {

// line 20
template <typename RandomIt, typename Distance, typename T, typename Compare>
inline void push_heap(RandomIt first, Distance holeIndex, Distance topIndex, T&& v, Compare comp) {
    Distance parent = (holeIndex - 1) / 2;
    while (holeIndex > topIndex && comp(*(first + parent), v)) {
        *(first + holeIndex) = blz::move(*(first + parent));
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = blz::move(v);
}

// line 34
template <typename RandomIt, typename Distance, typename T, typename Compare>
void adjust_heap(RandomIt first, Distance holeIndex, Distance len, T&& v, Compare comp) {
    const Distance topIndex = holeIndex;
    Distance childIndex = holeIndex;
    while (2 * (childIndex + 1) < len) {
        childIndex = 2 * (childIndex + 1);
        if (comp(*(first + childIndex), *(first + (childIndex - 1))))
            --childIndex;
        *(first + holeIndex) = blz::move(*(first + childIndex));
        holeIndex = childIndex;
    }
    if (2 * (childIndex + 1) == len) {
        childIndex = 2 * (childIndex + 1);
        *(first + holeIndex) = blz::move(*(first + (childIndex - 1)));
        holeIndex = childIndex - 1;
    }
    push_heap(first, holeIndex, topIndex, blz::move(v), comp);
}

}  // namespace internal

// line 105
template <typename RandomIt, typename Compare>
void make_heap(RandomIt first, RandomIt last, Compare comp) {
    typedef typename iterator_traits<RandomIt>::difference_type D;
    typedef typename iterator_traits<RandomIt>::value_type T;
    const D len = last - first;
    if (len < 2)
        return;
    D parent = (len - 2) / 2;
    while (true) {
        T v = blz::move(*(first + parent));
        internal::adjust_heap(first, parent, len, blz::move(v), comp);
        if (parent == 0)
            return;
        --parent;
    }
}

// line 133
template <typename RandomIt, typename Compare>
void sort_heap(RandomIt first, RandomIt last, Compare comp) {
    typedef typename iterator_traits<RandomIt>::difference_type D;
    typedef typename iterator_traits<RandomIt>::value_type T;
    while (last - first > 1) {
        --last;
        T v = blz::move(*last);
        *last = blz::move(*first);
        internal::adjust_heap(first, D(0), D(last - first), blz::move(v), comp);
    }
}

}  // namespace blz
