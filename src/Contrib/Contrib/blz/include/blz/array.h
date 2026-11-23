#pragma once

// Fixed-size array. Members and line order from the DWARF.

#include "config.h"
#include "iter_reverse.h"
#include "util_swap.h"

namespace blz {

// line 19
template <typename T, size_t N>
struct array {
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef blz::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef blz::reverse_iterator<iterator> reverse_iterator;

    // line 43
    void fill(const value_type& u) {
        for (size_t i = 0; i < N; ++i)
            _elements[i] = u;
    }

    // line 49
    void swap(array& x) {
        for (size_t i = 0; i < N; ++i)
            blz::swap(_elements[i], x._elements[i]);
    }

    iterator begin() { return _elements; }
    const_iterator begin() const { return _elements; }
    iterator end() { return _elements + N; }
    const_iterator end() const { return _elements + N; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }
    const_reverse_iterator crbegin() const { return rbegin(); }
    const_reverse_iterator crend() const { return rend(); }

    size_t size() const { return N; }
    size_t max_size() const { return N; }
    bool empty() const { return N == 0; }

    // line 148
    reference operator[](size_t n) {
        return _elements[n];
    }

    // line 155
    const_reference operator[](size_t n) const {
        return _elements[n];
    }

    reference at(size_t n) { return _elements[n]; }
    const_reference at(size_t n) const { return _elements[n]; }

    reference front() { return _elements[0]; }
    const_reference front() const { return _elements[0]; }
    reference back() { return _elements[N - 1]; }
    const_reference back() const { return _elements[N - 1]; }

    T* data() { return _elements; }
    const T* data() const { return _elements; }

    T _elements[N];
};

template <typename T, size_t N>
inline bool operator==(const array<T, N>& x, const array<T, N>& y) {
    for (size_t i = 0; i < N; ++i)
        if (!(x._elements[i] == y._elements[i]))
            return false;
    return true;
}

template <typename T, size_t N>
inline bool operator!=(const array<T, N>& x, const array<T, N>& y) {
    return !(x == y);
}

template <typename T, size_t N>
inline void swap(array<T, N>& x, array<T, N>& y) {
    x.swap(y);
}

}  // namespace blz
