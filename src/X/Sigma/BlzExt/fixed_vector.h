// Original file: X/Sigma/blzext/fixed_vector.h
// Subset: blzext::fixed_vector<T, N>, an in-place vector of at most N elements, with the
// members X/Client/Console/ConsoleParty.cpp uses so far. Layout from the DWARF:
//   char m_storage[N * sizeof(T)]; T * m_elements; size_t m_size;

#pragma once

#include <new>
#include "../../../Contrib/Contrib/blz/include/blz/algo_memory.h"
#include "../../../Contrib/Contrib/blz/include/blz/algo_seq.h"
#include "../../../Contrib/Contrib/blz/include/blz/iter_reverse.h"
#include "../../../Contrib/Contrib/blz/include/blz/type_traits.h"
#include <stddef.h>

namespace blzext {

template <typename T, long unsigned int N> class fixed_vector {
public:
    typedef T value_type;
    typedef T * pointer;
    typedef const T * const_pointer;
    typedef T * iterator;
    typedef const T * const_iterator;
    typedef T & reference;
    typedef const T & const_reference;
    typedef size_t size_type;
    typedef blz::reverse_iterator<iterator> reverse_iterator;
    typedef blz::reverse_iterator<const_iterator> const_reverse_iterator;

    explicit fixed_vector() { _init(); }  // line 56
    ~fixed_vector() {  // line 105
        blz::_destroy(m_elements, m_elements + m_size, typename blz::is_trivially_destructible<T>::type());
    }

    iterator begin() { return m_elements; }  // line 189
    const_iterator begin() const { return m_elements; }  // line 195
    iterator end() { return m_elements + m_size; }  // line 201
    const_iterator end() const { return m_elements + m_size; }  // line 207
    reverse_iterator rbegin() { return reverse_iterator(end()); }  // line 213
    reverse_iterator rend() { return reverse_iterator(begin()); }
    pointer data() { return m_elements; }  // line 368
    size_type size() const { return m_size; }  // line 262
    bool empty() const { return m_size == 0; }  // line 302
    bool full() const { return m_size == N; }  // line 308
    reference operator[](size_type n) { return m_elements[n]; }  // line 315
    const_reference operator[](size_type n) const { return m_elements[n]; }

    void push_back(const T & x) {  // line 393
        blz::_copy_construct(m_elements + m_size, x, typename blz::is_trivially_copy_constructible<T>::type());
        ++m_size;
    }
    reference push_back() {  // line 409
        T * p = ::new (m_elements + m_size) T();
        ++m_size;
        return *p;
    }
    template <typename... Args> void emplace_back(Args &&... args) {  // line 383
        ::new (m_elements + m_size) T(args...);
        ++m_size;
    }
    iterator erase(const_iterator position) {  // line 542
        iterator position_ = const_cast<iterator>(position);
        blz::move(position_ + 1, end(), position_);
        --m_size;
        return position_;
    }
    void clear() {  // line 573
        blz::_destroy(m_elements, m_elements + m_size, typename blz::is_trivially_destructible<T>::type());
        m_size = 0;
    }

protected:
    void _init() {  // line 706
        m_elements = reinterpret_cast<T *>(m_storage);
        m_size = 0;
    }
    char m_storage[N * sizeof(T)];
    T * m_elements;
    size_type m_size;
};

}  // namespace blzext
