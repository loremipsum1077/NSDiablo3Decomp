#pragma once

// advance / distance. Signatures from the mangled names: _advance<Iter,
// Distance>(Iter&, Distance, tag), the generic _distance is templated on the
// tag (_distance<rb_tree_const_iterator<unsigned long>,
// bidirectional_iterator_tag>), the random access one is not. In the binary
// _advance(random access) is `i += n` (line 33), _distance(random access)
// is `last - first` (line 57), and advance/distance dispatch on the
// iterator category (lines 39 and 64).

#include "config.h"
#include "iter_types.h"

namespace blz {

template <typename Iter, typename Distance>
inline void _advance(Iter& i, Distance n, input_iterator_tag) {
    for (; n > 0; --n)
        ++i;
}

template <typename Iter, typename Distance>
inline void _advance(Iter& i, Distance n, bidirectional_iterator_tag) {
    if (n >= 0)
        for (; n > 0; --n)
            ++i;
    else
        for (; n < 0; ++n)
            --i;
}

// line 31
template <typename Iter, typename Distance>
inline void _advance(Iter& i, Distance n, random_access_iterator_tag) {
    i += n;
}

// line 37
template <typename Iter, typename Distance>
inline void advance(Iter& i, Distance n) {
    _advance(i, n, typename iterator_traits<Iter>::iterator_category());
}

// line 45
template <typename Iter, typename Tag>
inline typename iterator_traits<Iter>::difference_type _distance(Iter first, Iter last, Tag) {
    typename iterator_traits<Iter>::difference_type n = 0;
    for (; first != last; ++first)
        ++n;
    return n;
}

// line 55
template <typename Iter>
inline typename iterator_traits<Iter>::difference_type _distance(Iter first, Iter last,
                                                                 random_access_iterator_tag) {
    return last - first;
}

// line 62
template <typename Iter>
inline typename iterator_traits<Iter>::difference_type distance(Iter first, Iter last) {
    return _distance(first, last, typename iterator_traits<Iter>::iterator_category());
}

template <typename Iter>
inline Iter next(Iter i, typename iterator_traits<Iter>::difference_type n = 1) {
    advance(i, n);
    return i;
}

template <typename Iter>
inline Iter prev(Iter i, typename iterator_traits<Iter>::difference_type n = 1) {
    advance(i, -n);
    return i;
}

}  // namespace blz
