#pragma once

// blz::basic_string. Layout from the DWARF: m_elements points either at the
// 16 byte m_storage buffer inside the object (m_capacity_is_embedded set,
// capacity BUF_SIZE - 1) or at a heap block of m_capacity + 1 chars.
// Members, parameter and local names are from the DWARF; the bodies follow
// the line info of the out-of-line instantiations (destructor, the move
// constructor, operator=(&&), _assign_rv, swap, append, replace,
// replace_all, format/formatv, the operator+ family) and of the inlined
// copies elsewhere. Every member is defined in the class, in the order of
// the original (the comments give the original line of each).
//
// The growth policy is _grow (line 1350): capacity * 3 / 2, at least the
// requested size. Writers that may read from their own buffer (assign,
// append, insert, replace) compute the source offset before reserving and
// move instead of copy when it lies inside the string.

#include "config.h"
#include "algo_seq.h"
#include "algo_string.h"
#include "char_traits.h"
#include "func_hash.h"
#include "iter_op.h"
#include "iter_reverse.h"
#include "iter_types.h"
#include "mem_alloc.h"
#include "string_view.h"
#include "type_traits.h"
#include "util_swap.h"

#include <stdarg.h>

#include <initializer_list>

namespace blz {

template <typename T, typename Traits> class basic_ostream;  // blz/ostream.h

// line 28
template <typename T, typename Traits, typename Alloc>
class basic_string : protected Alloc {
public:
    typedef Traits traits_type;
    typedef typename Traits::char_type value_type;
    typedef Alloc allocator_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    typedef blz::reverse_iterator<T*> reverse_iterator;
    typedef blz::reverse_iterator<const T*> const_reverse_iterator;

    static const size_type npos = static_cast<size_type>(-1);
    static const size_type BUF_SIZE = 16;

    // line 59
    basic_string()
    {
        _init();
    }

    // line 65
    explicit basic_string(const Alloc& alloc)
        : Alloc(alloc)
    {
        _init();
    }

    // line 72
    basic_string(const basic_string& str)
        : Alloc(str)
    {
        _init();
        assign(str, 0, npos);
    }

    // line 80
    basic_string(basic_string&& str)
        : Alloc(str)
    {
        _init();
        _assign_rv(blz::move(str));
    }

    basic_string(const basic_string& str, size_type pos, const Alloc& alloc = Alloc())
        : Alloc(alloc)
    {
        _init();
        assign(str, pos, npos);
    }

    basic_string(const basic_string& str, size_type pos, size_type n, const Alloc& alloc = Alloc())
        : Alloc(alloc)
    {
        _init();
        assign(str, pos, n);
    }

    // line 104
    basic_string(const T* s, size_type n, const Alloc& alloc = Alloc())
        : Alloc(alloc)
    {
        _init();
        assign(s, n);
    }

    // line 112
    basic_string(const T* s, const Alloc& alloc = Alloc())
        : Alloc(alloc)
    {
        _init();
        size_type n = traits_type::length(s);
        assign(s, n);
    }

    // line 126
    basic_string(size_type n, T c, const Alloc& alloc = Alloc())
        : Alloc(alloc)
    {
        _init();
        assign(n, c);
    }

    // line 135
    template <typename InputIterator>
    basic_string(InputIterator begin, InputIterator end, const Alloc& alloc = Alloc(),
                 typename enable_if<!is_integral<InputIterator>::value, void>::type* = 0)
        : Alloc(alloc)
    {
        _init();
        assign(begin, end);
    }

    basic_string(std::initializer_list<T> il, const Alloc& alloc = Alloc())
        : Alloc(alloc)
    {
        _init();
        assign(il.begin(), il.size());
    }

    basic_string(const basic_string& str, const Alloc& alloc)
        : Alloc(alloc)
    {
        _init();
        assign(str, 0, npos);
    }

    basic_string(basic_string&& str, const Alloc& alloc)
        : Alloc(alloc)
    {
        _init();
        _assign_rv(blz::move(str));
    }

    explicit basic_string(const basic_string_view<T, Traits>& sv, const Alloc& alloc = Alloc())
        : Alloc(alloc)
    {
        _init();
        assign(sv.data(), sv.size());
    }

    // line 178
    ~basic_string()
    {
        _deallocate_elements();
    }

    // line 184
    basic_string& operator=(const basic_string& str)
    {
        if (this != &str)
            assign(str);
        return *this;
    }

    // line 193
    basic_string& operator=(basic_string&& str)
    {
        if (this != &str)
            _assign_rv(blz::move(str));
        return *this;
    }

    // line 202
    basic_string& operator=(const T* s)
    {
        return assign(s, traits_type::length(s));
    }

    basic_string& operator=(T c)
    {
        return assign(1, c);
    }

    basic_string& operator=(std::initializer_list<T> il)
    {
        return assign(il.begin(), il.size());
    }

    // line 226
    iterator begin()
    {
        return m_elements;
    }

    const_iterator begin() const
    {
        return m_elements;
    }

    // line 238
    iterator end()
    {
        return m_elements + m_size;
    }

    const_iterator end() const
    {
        return m_elements + m_size;
    }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_iterator cbegin() const { return m_elements; }
    const_iterator cend() const { return m_elements + m_size; }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

    // line 299
    size_type size() const
    {
        return m_size;
    }

    // line 305
    size_type length() const
    {
        return m_size;
    }

    size_type max_size() const
    {
        return npos - 1;
    }

    // line 317
    void resize(size_type n, T c = T())
    {
        if (m_size > n)
            erase(n);
        else
            append(n - m_size, c);
    }

    // line 328
    size_type capacity() const
    {
        return m_capacity;
    }

    bool capacity_is_embedded() const
    {
        return m_capacity_is_embedded;
    }

    // line 344
    void reserve(size_type size)
    {
        if (size > m_capacity)
        {
            pointer new_elements = _grow(size);
            _transfer(new_elements);
            _set_elements(new_elements);
        }
    }

    // line 373
    void clear()
    {
        _set_size(0);
    }

    // line 379
    bool empty() const
    {
        return m_size == 0;
    }

    // line 386
    const_reference operator[](size_type pos) const
    {
        return m_elements[pos];
    }

    // line 393
    reference operator[](size_type pos)
    {
        return m_elements[pos];
    }

    // line 400
    const_reference at(size_type n) const
    {
        return m_elements[n];
    }

    reference at(size_type n)
    {
        return m_elements[n];
    }

    const_reference front() const { return m_elements[0]; }
    reference front() { return m_elements[0]; }
    const_reference back() const { return m_elements[m_size - 1]; }
    reference back() { return m_elements[m_size - 1]; }

    // line 443
    basic_string& operator+=(const basic_string& str)
    {
        return append(str, 0, npos);
    }

    // line 449
    basic_string& operator+=(const T* s)
    {
        return append(s);
    }

    // line 455
    basic_string& operator+=(T c)
    {
        push_back(c);
        return *this;
    }

    basic_string& operator+=(std::initializer_list<T> il)
    {
        return append(il.begin(), il.size());
    }

    // line 469
    basic_string& append(const basic_string& str)
    {
        return append(str, 0, npos);
    }

    // line 475
    basic_string& append(const basic_string& str, size_type pos, size_type n)
    {
        size_type rlen = str.m_size - pos;
        if (rlen > n)
            rlen = n;

        return append(str.m_elements + pos, rlen);
    }

    // line 487
    basic_string& append(const T* s, size_type n)
    {
        size_type offset = s - m_elements;
        size_type new_len = m_size + n;
        reserve(new_len);
        T* data = m_elements + m_size;

        if (offset < m_size)
            traits_type::move(data, m_elements + offset, n);
        else
            traits_type::copy(data, s, n);
        _set_size(new_len);
        return *this;
    }

    // line 503
    basic_string& append(const T* s)
    {
        return append(s, traits_type::length(s));
    }

    // line 509
    basic_string& append(size_type n, T c)
    {
        size_type new_len = m_size + n;
        reserve(new_len);
        traits_type::assign(m_elements + m_size, n, c);
        _set_size(new_len);
        return *this;
    }

    // line 521
    template <typename InputIterator>
    typename enable_if<!is_integral<InputIterator>::value, basic_string&>::type append(InputIterator first,
                                                                                     InputIterator last)
    {
        return append(basic_string(first, last));
    }

    basic_string& append(std::initializer_list<T> il)
    {
        return append(il.begin(), il.size());
    }

    // line 535
    void push_back(T c)
    {
        size_type new_len = m_size + 1;
        reserve(new_len);
        traits_type::assign(m_elements[m_size], c);
        _set_size(new_len);
    }

    // line 544
    basic_string& assign(const basic_string& str)
    {
        return assign(str.m_elements, str.m_size);
    }

    basic_string& assign(basic_string&& str)
    {
        if (this != &str)
            _assign_rv(blz::move(str));
        return *this;
    }

    // line 559
    basic_string& assign(const basic_string& str, size_type pos, size_type n)
    {
        size_type rlen = str.m_size - pos;
        if (rlen > n)
            rlen = n;

        return assign(str.m_elements + pos, rlen);
    }

    // line 571
    basic_string& assign(const T* s, size_type n)
    {
        size_type offset = s - m_elements;
        reserve(n);
        T* data = m_elements;

        if (offset < m_size)
            traits_type::move(data, data + offset, n);
        else
            traits_type::copy(data, s, n);
        _set_size(n);
        return *this;
    }

    // line 586
    basic_string& assign(const T* s)
    {
        return assign(s, traits_type::length(s));
    }

    // line 592
    basic_string& assign(size_type n, T c)
    {
        reserve(n);
        traits_type::assign(m_elements, n, c);
        _set_size(n);
        return *this;
    }

    // line 603
    template <typename InputIterator>
    typename enable_if<!is_integral<InputIterator>::value, basic_string&>::type assign(InputIterator first,
                                                                                     InputIterator last)
    {
        _assign(first, last, typename iterator_traits<InputIterator>::iterator_category());
        return *this;
    }

    basic_string& assign(std::initializer_list<T> il)
    {
        return assign(il.begin(), il.size());
    }

    basic_string& insert(size_type pos, const basic_string& str)
    {
        return insert(pos, str.m_elements, str.m_size);
    }

    // line 637
    basic_string& insert(size_type pos, const T* s, size_type n)
    {
        size_type offset = s - m_elements;
        size_type new_len = m_size + n;
        reserve(new_len);

        traits_type::move(m_elements + pos + n, m_elements + pos, m_size - pos);
        if (offset < m_size)
        {
            if (offset > pos)
                offset += n;
            traits_type::move(m_elements + pos, m_elements + offset, n);
        }
        else
        {
            traits_type::copy(m_elements + pos, s, n);
        }
        _set_size(new_len);
        return *this;
    }

    // line 662
    basic_string& insert(size_type pos, const T* s)
    {
        return insert(pos, s, traits_type::length(s));
    }

    // line 716
    basic_string& erase(size_type pos = 0, size_type n = npos)
    {
        size_type xlen = m_size - pos;
        if (xlen > n)
            xlen = n;

        if (xlen != 0)
        {
            T* data = m_elements + pos;
            size_type new_len = m_size - xlen;
            traits_type::move(data, data + xlen, new_len - pos);
            _set_size(new_len);
        }
        return *this;
    }

    iterator erase(const_iterator p)
    {
        size_type pos = p - m_elements;
        erase(pos, 1);
        return m_elements + pos;
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        size_type pos = first - m_elements;
        erase(pos, last - first);
        return m_elements + pos;
    }

    // line 754
    void pop_back()
    {
        _set_size(m_size - 1);
    }

    // line 782
    basic_string& replace(size_type pos, size_type n1, const T* s, size_type n2)
    {
        size_type offset = s - m_elements;
        size_type xlen = m_size - pos;
        if (xlen > n1)
            xlen = n1;

        size_type new_len = m_size - xlen + n2;
        reserve(new_len);
        T* data = m_elements;

        if (offset < m_size)
        {
            basic_string tmp(s, n2);
            traits_type::move(data + pos + n2, data + pos + xlen, new_len - pos - n2);
            traits_type::copy(data + pos, tmp.m_elements, n2);
        }
        else
        {
            traits_type::move(data + pos + n2, data + pos + xlen, new_len - pos - n2);
            traits_type::copy(data + pos, s, n2);
        }
        _set_size(new_len);
        return *this;
    }

    // line 812
    basic_string& replace(size_type pos, size_type n1, const T* s)
    {
        return replace(pos, n1, s, traits_type::length(s));
    }

    basic_string& replace(size_type pos, size_type n1, const basic_string& str)
    {
        return replace(pos, n1, str.m_elements, str.m_size);
    }

    // line 902
    basic_string& replace_all(const T* s1, const T* s2)
    {
        size_type length1 = traits_type::length(s1);
        size_type length2 = traits_type::length(s2);
        for (size_type pos = 0; (pos = find(s1, pos)) <= m_size; pos += length2)
            replace(pos, length1, s2);
        return *this;
    }

    // line 926
    // NON_MATCHING: one instruction: the binary tests both embedded bits with
    // `orr x10, x8, x9` (this, x); this compiles to `orr x10, x9, x8`.
    void swap(basic_string& x)
    {
        if (!(m_capacity_is_embedded | x.m_capacity_is_embedded))
        {
            blz::swap(m_elements, x.m_elements);
            blz::swap(m_size, x.m_size);
            { size_type tmp = m_capacity; m_capacity = x.m_capacity; x.m_capacity = tmp; }
        }
        else
        {
            basic_string& smaller = (x.m_size > m_size) ? *this : x;
            basic_string& larger = (x.m_size > m_size) ? x : *this;

            if (x.m_size <= m_capacity && m_size <= x.m_capacity)
            {
                pointer end = blz::swap_ranges(smaller.m_elements, smaller.m_elements + smaller.m_size,
                                               larger.m_elements);
                traits_type::copy(smaller.m_elements + smaller.m_size, end,
                                  (larger.m_elements + larger.m_size) - end);
                blz::swap(m_size, x.m_size);
            }
            else if (larger.m_capacity_is_embedded)
            {
                pointer elements = smaller.allocate(larger.m_size + 1);
                traits_type::copy(elements, larger.m_elements, larger.m_size);
                traits_type::copy(larger.m_elements, smaller.m_elements, smaller.m_size);
                smaller._deallocate_elements();
                smaller._set_elements(elements);
                smaller.m_capacity = larger.m_size;
                blz::swap(m_size, x.m_size);
            }
            else
            {
                pointer elements = larger.allocate(smaller.m_size + 1);
                traits_type::copy(elements, smaller.m_elements, smaller.m_size);
                smaller._set_elements(larger.m_elements);
                smaller.m_capacity = larger.m_capacity;
                larger._set_elements(elements);
                larger.m_capacity = smaller.m_size;
                blz::swap(m_size, x.m_size);
            }
            m_elements[m_size] = T();
            x.m_elements[x.m_size] = T();
        }
    }

    operator basic_string_view<T, Traits>() const
    {
        return basic_string_view<T, Traits>(m_elements, m_size);
    }

    // line 1006
    const T* c_str() const
    {
        return m_elements;
    }

    // line 1012
    const T* data() const
    {
        return m_elements;
    }

    // line 1018
    T* data()
    {
        return m_elements;
    }

    allocator_type get_allocator() const
    {
        return *this;
    }

    // line 1030
    size_type find(const basic_string& str, size_type pos = 0) const
    {
        return string_find<traits_type>(m_elements, m_size, str.m_elements, pos, str.m_size);
    }

    size_type find(const T* s, size_type pos, size_type n) const
    {
        return string_find<traits_type>(m_elements, m_size, s, pos, n);
    }

    // line 1044
    size_type find(const T* s, size_type pos = 0) const
    {
        return string_find<traits_type>(m_elements, m_size, s, pos,
                                        traits_type::length(s));
    }

    // line 1051
    size_type find(T c, size_type pos = 0) const
    {
        return string_find<traits_type>(m_elements, m_size, &c, pos, 1);
    }

    size_type find_first_of(const basic_string& str, size_type pos = 0) const
    {
        return string_find_first_of<traits_type>(m_elements, m_size, str.m_elements, pos, str.m_size);
    }

    // line 1093
    size_type find_first_of(const T* s, size_type pos, size_type n) const
    {
        return string_find_first_of<traits_type>(m_elements, m_size, s, pos, n);
    }

    // line 1100
    size_type find_first_of(const T* s, size_type pos = 0) const
    {
        return string_find_first_of<traits_type>(m_elements, m_size, s, pos, traits_type::length(s));
    }

    // line 1107
    size_type find_first_of(T c, size_type pos = 0) const
    {
        return string_find_first_of<traits_type>(m_elements, m_size, &c, pos, 1);
    }

    size_type find_last_of(const basic_string& str, size_type pos = npos) const
    {
        return string_find_last_of<traits_type>(m_elements, m_size, str.m_elements, pos, str.m_size);
    }

    size_type find_last_of(const T* s, size_type pos, size_type n) const
    {
        return string_find_last_of<traits_type>(m_elements, m_size, s, pos, n);
    }

    size_type find_last_of(const T* s, size_type pos = npos) const
    {
        return string_find_last_of<traits_type>(m_elements, m_size, s, pos, traits_type::length(s));
    }

    // line 1135
    size_type find_last_of(T c, size_type pos = npos) const
    {
        return string_find_last_of<traits_type>(m_elements, m_size, &c, pos, 1);
    }

    size_type find_first_not_of(const basic_string& str, size_type pos = 0) const
    {
        return string_find_first_not_of<traits_type>(m_elements, m_size, str.m_elements, pos, str.m_size);
    }

    // line 1149
    size_type find_first_not_of(const T* s, size_type pos, size_type n) const
    {
        return string_find_first_not_of<traits_type>(m_elements, m_size, s, pos, n);
    }

    // line 1156
    size_type find_first_not_of(const T* s, size_type pos = 0) const
    {
        return string_find_first_not_of<traits_type>(m_elements, m_size, s, pos, traits_type::length(s));
    }

    size_type find_first_not_of(T c, size_type pos = 0) const
    {
        return string_find_first_not_of<traits_type>(m_elements, m_size, &c, pos, 1);
    }

    // line 1198
    basic_string substr(size_type pos = 0, size_type n = npos) const
    {
        size_type rlen = m_size - pos;
        if (rlen > n)
            rlen = n;

        return basic_string(m_elements + pos, rlen);
    }

    // line 1210
    int compare(const basic_string& str) const
    {
        return string_compare<traits_type>(m_elements, m_size, 0, m_size,
                                           str.m_elements, str.m_size);
    }

    // line 1217
    int compare(size_type pos1, size_type n1, const basic_string& str) const
    {
        return string_compare<traits_type>(m_elements, m_size, pos1, n1,
                                           str.m_elements, str.m_size);
    }

    // line 1236
    int compare(const T* s) const
    {
        return string_compare<traits_type>(m_elements, m_size, 0, m_size, s,
                                           traits_type::length(s));
    }

    int compare(size_type pos1, size_type n1, const T* s) const
    {
        return string_compare<traits_type>(m_elements, m_size, pos1, n1, s, traits_type::length(s));
    }

    int compare(size_type pos1, size_type n1, const T* s, size_type n2) const
    {
        return string_compare<traits_type>(m_elements, m_size, pos1, n1, s, n2);
    }

    basic_string& append_format(const T* fmt, ...)
    {
        va_list arg_list;
        va_start(arg_list, fmt);
        append_formatv(fmt, arg_list);
        va_end(arg_list);
        return *this;
    }

    basic_string& append_formatv(const T* fmt, va_list arg_list)
    {
        va_list arg_list_copy;
        va_copy(arg_list_copy, arg_list);
        size_type old_size = m_size;
        size_type len = traits_type::vscprintf(fmt, arg_list);
        reserve(old_size + len);
        traits_type::vsprintf(m_elements + old_size, len + 1, fmt, arg_list_copy);
        _set_size(old_size + len);
        va_end(arg_list_copy);
        return *this;
    }

    // line 1283
    basic_string& format(const T* fmt, ...)
    {
        va_list arg_list;
        va_start(arg_list, fmt);
        formatv(fmt, arg_list);
        va_end(arg_list);
        return *this;
    }

    // line 1293
    basic_string& formatv(const T* fmt, va_list arg_list)
    {
        va_list arg_list_copy;
        va_copy(arg_list_copy, arg_list);
        size_type len = traits_type::vscprintf(fmt, arg_list);
        reserve(len);
        traits_type::vsprintf(m_elements, len + 1, fmt, arg_list_copy);
        _set_size(len);
        va_end(arg_list_copy);
        return *this;
    }

    // line 1308
    T* modify_data()
    {
        return m_elements;
    }

    // line 1315
    T* modify_data(size_type size)
    {
        reserve(size);
        T* p = m_elements;
        p[size] = T();
        m_size = size;
        return p;
    }

    // line 1325
    void set_size(size_type size)
    {
        _set_size(size);
    }

    void truncate(size_type size)
    {
        if (size < m_size)
            _set_size(size);
    }

protected:
    // line 1340
    void _init()
    {
        m_size = 0;
        m_elements = m_storage;
        m_capacity = BUF_SIZE - 1;
        m_capacity_is_embedded = 1;
        m_storage[0] = T();
    }

    // line 1350
    pointer _grow(size_type size)
    {
        size_type new_capacity = m_capacity + m_capacity / 2;
        if (new_capacity < size)
            new_capacity = size;

        m_capacity = new_capacity;
        return this->allocate(new_capacity + 1);
    }

    // line 1361
    void _deallocate_elements()
    {
        if (!m_capacity_is_embedded)
            this->deallocate(m_elements);
    }

    // line 1367
    void _set_elements(pointer elements)
    {
        m_elements = elements;
        m_capacity_is_embedded = 0;
    }

    // line 1374
    void _transfer(pointer elements)
    {
        traits_type::copy(elements, m_elements, m_size);
        _deallocate_elements();
    }

    // line 1396
    void _set_size(size_type size)
    {
        m_elements[size] = T();
        m_size = size;
    }

    // line 1414
    template <typename InputIterator>
    void _assign(InputIterator first, InputIterator last, forward_iterator_tag)
    {
        size_type n = blz::distance(first, last);
        T* p = modify_data(n);
        blz::copy(first, last, p);
    }

    // line 1422
    void _assign_rv(basic_string&& x)
    {
        if (x.m_capacity_is_embedded)
        {
            assign(x.m_elements, x.m_size);
        }
        else
        {
            _deallocate_elements();
            _set_elements(x.m_elements);
            m_size = x.m_size;
            m_capacity = x.m_capacity;
            x._init();
        }
    }

    pointer m_elements;  // 0x00
    size_type m_size;  // 0x08
    size_type m_capacity : 63;  // 0x10
    size_type m_capacity_is_embedded : 1;  // 0x10
    T m_storage[BUF_SIZE];  // 0x18
};

typedef basic_string<char, char_traits<char>, allocator<char> > string;
typedef basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > wstring;

// line 1452
template <typename T, typename Traits, typename Alloc>
basic_string<T, Traits, Alloc> operator+(const basic_string<T, Traits, Alloc>& x,
                                         const basic_string<T, Traits, Alloc>& y)
{
    return basic_string<T, Traits, Alloc>(x).append(y);
}

// line 1459
template <typename T, typename Traits, typename Alloc>
inline basic_string<T, Traits, Alloc>&& operator+(basic_string<T, Traits, Alloc>&& x,
                                                  const basic_string<T, Traits, Alloc>& y)
{
    return blz::move(x.append(y));
}

// line 1473
template <typename T, typename Traits, typename Alloc>
inline basic_string<T, Traits, Alloc>&& operator+(basic_string<T, Traits, Alloc>&& x,
                                                  basic_string<T, Traits, Alloc>&& y)
{
    return blz::move(x.append(y));
}

// line 1480
template <typename T, typename Traits, typename Alloc>
basic_string<T, Traits, Alloc> operator+(const T* x, const basic_string<T, Traits, Alloc>& y)
{
    return basic_string<T, Traits, Alloc>(x).append(y);
}

// line 1487
template <typename T, typename Traits, typename Alloc>
inline basic_string<T, Traits, Alloc>&& operator+(const T* x, basic_string<T, Traits, Alloc>&& y)
{
    return blz::move(y.insert(0, x));
}

// line 1508
template <typename T, typename Traits, typename Alloc>
basic_string<T, Traits, Alloc> operator+(const basic_string<T, Traits, Alloc>& x, const T* y)
{
    return basic_string<T, Traits, Alloc>(x).append(y);
}

// line 1515
template <typename T, typename Traits, typename Alloc>
inline basic_string<T, Traits, Alloc>&& operator+(basic_string<T, Traits, Alloc>&& x, const T* y)
{
    return blz::move(x.append(y));
}

// line 1536
template <typename T, typename Traits, typename Alloc>
inline bool operator==(const basic_string<T, Traits, Alloc>& x, const basic_string<T, Traits, Alloc>& y)
{
    return x.compare(y) == 0;
}

// line 1548
template <typename T, typename Traits, typename Alloc>
inline bool operator==(const basic_string<T, Traits, Alloc>& x, const T* y)
{
    return x.compare(y) == 0;
}

template <typename T, typename Traits, typename Alloc>
inline bool operator==(const T* x, const basic_string<T, Traits, Alloc>& y)
{
    return y.compare(x) == 0;
}

// line 1567
template <typename T, typename Traits, typename Alloc>
inline bool operator!=(const basic_string<T, Traits, Alloc>& x, const basic_string<T, Traits, Alloc>& y)
{
    return x.compare(y) != 0;
}

// line 1579
template <typename T, typename Traits, typename Alloc>
inline bool operator!=(const basic_string<T, Traits, Alloc>& x, const T* y)
{
    return x.compare(y) != 0;
}

template <typename T, typename Traits, typename Alloc>
inline bool operator!=(const T* x, const basic_string<T, Traits, Alloc>& y)
{
    return y.compare(x) != 0;
}

// line 1598
template <typename T, typename Traits, typename Alloc>
inline bool operator<(const basic_string<T, Traits, Alloc>& x, const basic_string<T, Traits, Alloc>& y)
{
    return x.compare(y) < 0;
}

template <typename T, typename Traits, typename Alloc>
inline bool operator>(const basic_string<T, Traits, Alloc>& x, const basic_string<T, Traits, Alloc>& y)
{
    return x.compare(y) > 0;
}

template <typename T, typename Traits, typename Alloc>
inline bool operator<=(const basic_string<T, Traits, Alloc>& x, const basic_string<T, Traits, Alloc>& y)
{
    return x.compare(y) <= 0;
}

template <typename T, typename Traits, typename Alloc>
inline bool operator>=(const basic_string<T, Traits, Alloc>& x, const basic_string<T, Traits, Alloc>& y)
{
    return x.compare(y) >= 0;
}

template <typename T, typename Traits, typename Alloc>
inline void swap(basic_string<T, Traits, Alloc>& x, basic_string<T, Traits, Alloc>& y)
{
    x.swap(y);
}

// line 1738
template <typename T, typename Traits, typename Alloc>
inline basic_ostream<T, Traits>& operator<<(basic_ostream<T, Traits>& os, const basic_string<T, Traits, Alloc>& str)
{
    return os.write(str.data(), str.size());
}

// line 1798
template <typename T, typename Traits, typename Alloc>
struct hash<basic_string<T, Traits, Alloc> > {
    size_t operator()(const basic_string<T, Traits, Alloc>& s) const
    {
        return hash_data(s.data(), s.size());
    }
};

template <typename T, typename Traits>
inline basic_string<T, Traits, allocator<T> > basic_string_view<T, Traits>::to_string() const
{
    return basic_string<T, Traits, allocator<T> >(m_data, m_size);
}

// Defined in blz/src/string.cpp.
int stoi(const string& str, size_t* idx = 0, int base = 10);
float stof(const string& str, size_t* idx = 0) __asm__("sub_710003c1b0");  // 2.7.7 only
string to_string(int val);
string to_string(long val);
string to_string(unsigned int val);
string to_string(unsigned long val);
string to_string(float val);
string make_stringf(const char* fmt, ...);

// Defined in blz/src/scan.cpp. Not in the DWARF which header declares it.
int stoi(const char* s, const char** end, int base, bool* range_error);
float stof(const char* s, const char** end, bool* range_error) __asm__("sub_710003c5a0");  // 2.7.7 only

}  // namespace blz
