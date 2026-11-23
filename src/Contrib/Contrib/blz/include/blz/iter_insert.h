#pragma once

// Insert iterators. Members from the DWARF: back_insert_iterator holds
// `container` (line 17), operator=(const_reference) pushes back at line 42;
// insert_iterator (line 145) holds `container` and `iter`, and its
// operator++(int) returns a reference.

#include "config.h"
#include "iter_types.h"
#include "type_traits.h"

namespace blz {

// line 17
template <typename Cont>
class back_insert_iterator {
protected:
    Cont* container;

public:
    typedef Cont container_type;
    typedef output_iterator_tag iterator_category;
    typedef void value_type;
    typedef void difference_type;
    typedef void pointer;
    typedef void reference;

    // line 34
    explicit back_insert_iterator(Cont& x) : container(&x) {}

    // line 40
    back_insert_iterator& operator=(typename Cont::const_reference value) {
        container->push_back(value);
        return *this;
    }

    // line 47
    back_insert_iterator& operator=(typename Cont::value_type&& value) {
        container->push_back(blz::move(value));
        return *this;
    }

    back_insert_iterator& operator*() {
        return *this;
    }

    back_insert_iterator& operator++() {
        return *this;
    }

    back_insert_iterator operator++(int) {
        return *this;
    }
};

template <typename Cont>
inline back_insert_iterator<Cont> back_inserter(Cont& x) {
    return back_insert_iterator<Cont>(x);
}

template <typename Cont>
class front_insert_iterator {
protected:
    Cont* container;

public:
    typedef Cont container_type;
    typedef output_iterator_tag iterator_category;
    typedef void value_type;
    typedef void difference_type;
    typedef void pointer;
    typedef void reference;

    explicit front_insert_iterator(Cont& x) : container(&x) {}

    front_insert_iterator& operator=(typename Cont::const_reference value) {
        container->push_front(value);
        return *this;
    }

    front_insert_iterator& operator=(typename Cont::value_type&& value) {
        container->push_front(blz::move(value));
        return *this;
    }

    front_insert_iterator& operator*() {
        return *this;
    }

    front_insert_iterator& operator++() {
        return *this;
    }

    front_insert_iterator operator++(int) {
        return *this;
    }
};

template <typename Cont>
inline front_insert_iterator<Cont> front_inserter(Cont& x) {
    return front_insert_iterator<Cont>(x);
}

// line 145
template <typename Cont>
class insert_iterator {
protected:
    Cont* container;
    typename Cont::iterator iter;

public:
    typedef Cont container_type;
    typedef output_iterator_tag iterator_category;
    typedef void value_type;
    typedef void difference_type;
    typedef void pointer;
    typedef void reference;

    // line 163
    insert_iterator(Cont& x, typename Cont::iterator i) : container(&x), iter(i) {}

    // line 170
    insert_iterator& operator=(typename Cont::const_reference value) {
        iter = container->insert(iter, value);
        ++iter;
        return *this;
    }

    // line 178
    insert_iterator& operator=(typename Cont::value_type&& value) {
        iter = container->insert(iter, blz::move(value));
        ++iter;
        return *this;
    }

    insert_iterator& operator*() {
        return *this;
    }

    insert_iterator& operator++() {
        return *this;
    }

    insert_iterator& operator++(int) {
        return *this;
    }
};

template <typename Cont>
inline insert_iterator<Cont> inserter(Cont& x, typename Cont::iterator i) {
    return insert_iterator<Cont>(x, i);
}

}  // namespace blz
