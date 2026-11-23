#pragma once

// Iterator tags, iterator_traits and the iterator base. Layout and line
// order from the DWARF.

#include "config.h"

namespace blz {

// line 14
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// line 22
template <typename Iter>
struct iterator_traits {
    typedef typename Iter::difference_type difference_type;
    typedef typename Iter::value_type value_type;
    typedef typename Iter::pointer pointer;
    typedef typename Iter::reference reference;
    typedef typename Iter::iterator_category iterator_category;
};

// line 33
template <typename T>
struct iterator_traits<T*> {
    typedef ptrdiff_t difference_type;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef random_access_iterator_tag iterator_category;
};

// line 44
template <typename T>
struct iterator_traits<const T*> {
    typedef ptrdiff_t difference_type;
    typedef T value_type;
    typedef const T* pointer;
    typedef const T& reference;
    typedef random_access_iterator_tag iterator_category;
};

// line 56
template <typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T*,
          typename Reference = T&>
struct iterator {
    typedef T value_type;
    typedef Distance difference_type;
    typedef Pointer pointer;
    typedef Reference reference;
    typedef Category iterator_category;
};

}  // namespace blz

// Layout checks generated from the DWARF by tools/layout_asserts.py.
// Not checked: iterator_traits (template); iterator (template)
static_assert(sizeof(blz::input_iterator_tag) == 0x1, "blz::input_iterator_tag");
static_assert(sizeof(blz::forward_iterator_tag) == 0x1, "blz::forward_iterator_tag");
static_assert(sizeof(blz::bidirectional_iterator_tag) == 0x1, "blz::bidirectional_iterator_tag");
static_assert(sizeof(blz::random_access_iterator_tag) == 0x1, "blz::random_access_iterator_tag");
