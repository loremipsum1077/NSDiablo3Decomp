#pragma once

// blz::vector. Layout, member order and declaration lines from the DWARF:
// m_elements, m_size and a 63/1 bit split of the third word into m_capacity
// and m_capacity_is_embedded (set by fixed_vector, whose storage follows the
// vector; such storage is never freed and never handed to another vector).
//
// Bodies are written from the out-of-line copies in the binary and from the
// inline trees (tools/dwarf_fn.py) plus objdump -l line info:
// - Members are used directly inside the class: size(), capacity(), begin()
//   and end() are never inlined into another vector member.
// - Every trait tag is a named local (trivial_copy, trivial_move,
//   trivial_destructor) where the DWARF records one, otherwise a temporary.
// - reserve() is _grow + _transfer + _set_elements; _grow grows by half the
//   current capacity (at least to the requested size) and stores the new
//   capacity before allocating.
// - _deallocate_elements frees only non-embedded storage, with no null test,
//   and clears m_elements.
// - insert_raw opens a hole of n elements and returns where it is: [begin,
//   mid) holds live (moved-from) elements to assign over, [mid, end) is raw
//   storage to construct into. All the inserts are built on it.
// - The push_backs grow with reserve(m_capacity + 1) when m_size ==
//   m_capacity: with m_size + 1 clang cannot prove the request exceeds the
//   63-bit capacity and keeps an extra overflow test the binary does not have.
// - Members with no inlined or out-of-line copy anywhere in the binary
//   (assign, operator=(initializer_list), shrink_to_fit, push_back_raw,
//   unordered_erase, the allocator-taking constructors) have invented bodies.
// Asserts are compiled out in this build; none of the bodies check bounds.

#include "config.h"
#include "algo_memory.h"
#include "algo_seq.h"
#include "iter_move.h"
#include "iter_op.h"
#include "iter_reverse.h"
#include "iter_types.h"
#include "mem_alloc.h"
#include "type_traits.h"
#include "util_swap.h"

#include <initializer_list>
#include <new>

namespace blz {

// line 28
template <typename T, typename Alloc = allocator<T> >
class vector : protected Alloc {
public:
    typedef Alloc allocator_type;                                   // line 33
    typedef T value_type;                                           // line 34
    typedef value_type* pointer;                                    // line 35
    typedef const value_type* const_pointer;                        // line 36
    typedef value_type* iterator;                                   // line 37
    typedef const value_type* const_iterator;                       // line 38
    typedef size_t size_type;
    typedef value_type& reference;                                  // line 40
    typedef const value_type& const_reference;                      // line 41
    typedef ptrdiff_t difference_type;
    typedef blz::reverse_iterator<const_iterator> const_reverse_iterator;  // line 45
    typedef blz::reverse_iterator<iterator> reverse_iterator;              // line 46

    // line 48
    struct insert_raw_ranges {
        pointer begin;  // line 52
        pointer mid;    // line 53
        pointer end;    // line 54
    };

    // line 64
    vector() {
        _init();
    }

    // line 70
    explicit vector(const allocator_type& alloc) : allocator_type(alloc) {
        _init();
    }

    // line 77
    explicit vector(size_type n, const allocator_type& alloc = allocator_type())
        : allocator_type(alloc), m_size(n), m_capacity(n), m_capacity_is_embedded(false) {
        pointer elements = this->allocate(n);
        blz::_construct(elements, elements + n);
        m_elements = elements;
    }

    // line 89
    vector(size_type n, const value_type& x, const allocator_type& alloc = allocator_type())
        : allocator_type(alloc), m_size(n), m_capacity(n), m_capacity_is_embedded(false) {
        pointer elements = this->allocate(n);
        _uninitialized_fill(elements, elements + n, x);
        m_elements = elements;
    }

    // line 102
    template <typename InputIt>
    vector(InputIt first, InputIt last, const allocator_type& alloc = allocator_type(),
           typename enable_if<!is_integral<InputIt>::value>::type* = 0)
        : allocator_type(alloc), m_elements(0), m_size(0), m_capacity(0),
          m_capacity_is_embedded(false) {
        _construct(first, last, typename iterator_traits<InputIt>::iterator_category());
    }

    // line 113
    vector(const vector& x)
        : allocator_type(x), m_elements(0), m_size(0), m_capacity(0),
          m_capacity_is_embedded(false) {
        _construct(x.m_elements, x.m_elements + x.m_size, forward_iterator_tag());
    }

    // line 123
    vector(vector&& x) : allocator_type(x) {
        _init();
        _assign_rv(blz::move(x));
    }

    // line 131
    vector(const vector& x, const allocator_type& alloc)
        : allocator_type(alloc), m_elements(0), m_size(0), m_capacity(0),
          m_capacity_is_embedded(false) {
        _construct(x.m_elements, x.m_elements + x.m_size, forward_iterator_tag());
    }

    // line 141
    vector(vector&& x, const allocator_type& alloc) : allocator_type(alloc) {
        _init();
        _assign_rv(blz::move(x));
    }

    // line 150
    vector(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type())
        : allocator_type(alloc), m_elements(0), m_size(0), m_capacity(0),
          m_capacity_is_embedded(false) {
        _construct(il.begin(), il.end(), forward_iterator_tag());
    }

    // line 159
    ~vector() {
        _destroy();
    }

    // line 165
    vector& operator=(const vector& x) {
        if (this != &x) {
            is_trivially_copy_constructible<T> trivial_copy;
            is_trivially_destructible<T> trivial_destructor;

            if (x.m_size <= m_size) {
                pointer end = blz::copy(x.m_elements, x.m_elements + x.m_size, m_elements);
                blz::_destroy(end, m_elements + m_size, trivial_destructor);
                m_size = x.m_size;
            } else if (x.m_size <= m_capacity) {
                pointer end = blz::copy(x.m_elements, x.m_elements + m_size, m_elements);
                _uninitialized_copy(x.m_elements + m_size, x.m_elements + x.m_size, end, trivial_copy);
                m_size = x.m_size;
            } else {
                blz::_destroy(m_elements, m_elements + m_size, trivial_destructor);
                _deallocate_elements();

                pointer elements = this->allocate(x.m_size);
                _uninitialized_copy(x.m_elements, x.m_elements + x.m_size, elements, trivial_copy);

                m_size = x.m_size;
                m_capacity = x.m_size;
                _set_elements(elements);
            }
        }

        return *this;
    }

    // line 202
    vector& operator=(vector&& x) {
        if (this != &x) {
            _destroy();
            _assign_rv(blz::move(x));
        }
        return *this;
    }

    // line 215
    vector& operator=(std::initializer_list<value_type> il) {
        clear();
        insert(m_elements, il);
        return *this;
    }

    // line 224
    void assign(size_type n, const value_type& x) {
        clear();
        insert(m_elements, n, x);
    }

    // line 267
    void assign(std::initializer_list<value_type> il) {
        clear();
        insert(m_elements, il);
    }

    // line 275
    allocator_type get_allocator() const {
        return *this;
    }

    // line 282
    iterator begin() {
        return m_elements;
    }

    // line 288
    const_iterator begin() const {
        return m_elements;
    }

    // line 294
    iterator end() {
        return m_elements + m_size;
    }

    // line 300
    const_iterator end() const {
        return m_elements + m_size;
    }

    // line 306
    reverse_iterator rbegin() {
        return reverse_iterator(m_elements + m_size);
    }

    // line 312
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(m_elements + m_size);
    }

    // line 318
    reverse_iterator rend() {
        return reverse_iterator(m_elements);
    }

    // line 324
    const_reverse_iterator rend() const {
        return const_reverse_iterator(m_elements);
    }

    // line 330
    const_iterator cbegin() const {
        return m_elements;
    }

    // line 336
    const_iterator cend() const {
        return m_elements + m_size;
    }

    // line 342
    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(m_elements + m_size);
    }

    // line 348
    const_reverse_iterator crend() const {
        return const_reverse_iterator(m_elements);
    }

    // line 355
    size_type size() const {
        return m_size;
    }

    // line 361
    size_type max_size() const {
        return allocator_type::max_size();
    }

    // line 367
    void resize(size_type sz) {
        if (sz < m_size) {
            erase(m_elements + sz, m_elements + m_size);
        } else if (sz > m_size) {
            reserve(sz);
            blz::_construct(m_elements + m_size, m_elements + sz);
            m_size = sz;
        }
    }

    // line 384
    void resize(size_type sz, const value_type& c) {
        if (sz < m_size)
            erase(m_elements + sz, m_elements + m_size);
        else if (sz > m_size)
            insert(m_elements + m_size, sz - m_size, c);
    }

    // line 395
    size_type capacity() const {
        return m_capacity;
    }

    // line 404
    bool capacity_is_embedded() const {
        return m_capacity_is_embedded;
    }

    // line 411
    bool empty() const {
        return m_size == 0;
    }

    // line 417
    void reserve(size_type n) {
        if (n > m_capacity) {
            pointer new_elements = _grow(n);
            _transfer(new_elements);
            _set_elements(new_elements);
        }
    }

    // line 430
    void shrink_to_fit() {
        if (!m_capacity_is_embedded && m_size < m_capacity) {
            pointer new_elements = this->allocate(m_size);
            _transfer(new_elements);
            _set_elements(new_elements);
            m_capacity = m_size;
        }
    }

    // line 447
    reference operator[](size_type n) {
        return m_elements[n];
    }

    // line 454
    const_reference operator[](size_type n) const {
        return m_elements[n];
    }

    // line 461
    reference at(size_type n) {
        return m_elements[n];
    }

    // line 468
    const_reference at(size_type n) const {
        return m_elements[n];
    }

    // line 475
    reference front() {
        return m_elements[0];
    }

    // line 481
    const_reference front() const {
        return m_elements[0];
    }

    // line 487
    reference back() {
        return m_elements[m_size - 1];
    }

    // line 493
    const_reference back() const {
        return m_elements[m_size - 1];
    }

    // line 500
    pointer data() {
        return m_elements;
    }

    // line 506
    const_pointer data() const {
        return m_elements;
    }

    // line 515
    template <typename... Args>
    reference emplace_back(Args&&... args) {
        if (m_size == m_capacity)
            reserve(m_capacity + 1);

        pointer p = m_elements + m_size;
        new (p) value_type(blz::forward<Args>(args)...);
        ++m_size;
        return *p;
    }

    // line 528
    void push_back(const value_type& x) {
        size_type offset = &x - m_elements;
        if (m_size == m_capacity)
            reserve(m_capacity + 1);

        is_trivially_copy_constructible<T> trivial_copy;
        pointer p = m_elements + m_size;
        if (offset < m_size)
            _copy_construct(p, m_elements[offset], trivial_copy);
        else
            _copy_construct(p, x, trivial_copy);
        ++m_size;
    }

    // line 545
    reference push_back() {
        if (m_size == m_capacity)
            reserve(m_capacity + 1);

        pointer p = m_elements + m_size;
        new (p) value_type();
        ++m_size;
        return *p;
    }

    // line 561
    pointer push_back_raw(size_type n) {
        if (m_size + n > m_capacity)
            reserve(m_size + n);

        pointer p = m_elements + m_size;
        m_size += n;
        return p;
    }

    // line 572
    void push_back(value_type&& x) {
        size_type offset = &x - m_elements;
        if (m_size == m_capacity)
            reserve(m_capacity + 1);

        is_trivially_move_constructible<T> trivial_move;
        pointer p = m_elements + m_size;
        if (offset < m_size)
            _move_construct(p, blz::move(m_elements[offset]), trivial_move);
        else
            _move_construct(p, blz::move(x), trivial_move);
        ++m_size;
    }

    // line 589
    void pop_back() {
        --m_size;
        blz::_destroy(m_elements + m_size, typename is_trivially_destructible<T>::type());
    }

    // line 616
    iterator insert(const_iterator position, const value_type& x) {
        insert_raw_ranges r = insert_raw(position, 1);
        if (r.begin == r.mid)
            _copy_construct(r.begin, x, typename is_trivially_copy_constructible<T>::type());
        else
            *r.begin = x;
        return r.begin;
    }

    // line 631
    iterator insert(const_iterator position, size_type n, const value_type& x) {
        insert_raw_ranges r = insert_raw(position, n);
        blz::fill(r.begin, r.mid, x);
        _uninitialized_fill(r.mid, r.end, x);
        return r.begin;
    }

    // line 644
    template <typename InputIt>
    typename enable_if<!is_integral<InputIt>::value, iterator>::type
    insert(const_iterator position, InputIt first, InputIt last) {
        return _insert(position, first, last, typename iterator_traits<InputIt>::iterator_category());
    }

    // line 655
    iterator insert(const_iterator position, value_type&& x) {
        insert_raw_ranges r = insert_raw(position, 1);
        if (r.begin == r.mid)
            _move_construct(r.begin, blz::move(x), typename is_trivially_move_constructible<T>::type());
        else
            *r.begin = blz::move(x);
        return r.begin;
    }

    // line 671
    void insert(const_iterator position, std::initializer_list<value_type> il) {
        _insert(position, il.begin(), il.end(), forward_iterator_tag());
    }

    // line 678
    iterator erase(const_iterator position) {
        T* position_ = const_cast<T*>(position);

        pointer p = blz::move(position_ + 1, m_elements + m_size, position_);
        blz::_destroy(p, m_elements + m_size, typename is_trivially_destructible<T>::type());
        --m_size;
        return position_;
    }

    // line 690
    iterator erase(const_iterator first, const_iterator last) {
        T* first_ = const_cast<T*>(first);
        T* last_ = const_cast<T*>(last);
        is_trivially_destructible<T> trivial_destructor;

        pointer p = blz::move(last_, m_elements + m_size, first_);
        blz::_destroy(p, m_elements + m_size, trivial_destructor);
        m_size = p - m_elements;
        return first_;
    }

    // line 704. Not inlined anywhere in the binary; body invented.
    iterator unordered_erase(const_iterator position) {
        T* position_ = const_cast<T*>(position);

        pointer last = m_elements + m_size - 1;
        if (position_ != last)
            *position_ = blz::move(*last);
        pop_back();
        return position_;
    }

    // line 721
    // The two embedded-bit tests merge into one `orr this, x` only when the
    // slow path reads x.m_capacity unconditionally (GVN hoists that load), so
    // the capacity test below starts with `m_size <= x.m_capacity`.
    void swap(vector& x) {
        if (!m_capacity_is_embedded && !x.m_capacity_is_embedded) {
            size_type tmp;
            blz::swap(m_elements, x.m_elements);
            blz::swap(m_size, x.m_size);
            tmp = m_capacity; m_capacity = x.m_capacity; x.m_capacity = tmp;
        } else {
            vector& larger = x.m_size > m_size ? x : *this;
            vector& smaller = x.m_size > m_size ? *this : x;

            if (m_size <= x.m_capacity && x.m_size <= m_capacity) {
                is_trivially_move_constructible<T> trivial_move;
                is_trivially_destructible<T> trivial_destructor;
                pointer end = blz::swap_ranges(smaller.m_elements, smaller.m_elements + smaller.m_size,
                                               larger.m_elements);
                _uninitialized_move(end, larger.m_elements + larger.m_size,
                                    smaller.m_elements + smaller.m_size, trivial_move);
                blz::_destroy(end, larger.m_elements + larger.m_size, trivial_destructor);
            } else if (larger.m_capacity_is_embedded) {
                is_trivially_move_constructible<T> trivial_move;
                pointer elements = this->allocate(larger.m_size);
                _uninitialized_move(larger.m_elements, larger.m_elements + larger.m_size, elements,
                                    trivial_move);
                _uninitialized_move(smaller.m_elements, smaller.m_elements + smaller.m_size,
                                    larger.m_elements, trivial_move);
                smaller._deallocate_elements();
                smaller._set_elements(elements);
                smaller.m_capacity = larger.m_size;
            } else {
                is_trivially_move_constructible<T> trivial_move;
                pointer elements = this->allocate(smaller.m_size);
                _uninitialized_move(smaller.m_elements, smaller.m_elements + smaller.m_size, elements,
                                    trivial_move);
                smaller._set_elements(larger.m_elements);
                smaller.m_capacity = larger.m_capacity;
                larger._set_elements(elements);
                larger.m_capacity = smaller.m_size;
            }

            blz::swap(m_size, x.m_size);
        }
    }

    // line 804
    void clear() {
        blz::_destroy(m_elements, m_elements + m_size, typename is_trivially_destructible<T>::type());
        m_size = 0;
    }

    // line 813
    insert_raw_ranges insert_raw(const_iterator position, size_type n) {
        insert_raw_ranges result;
        pointer position_ = const_cast<pointer>(position);
        size_type size = m_size;
        size_type idx = position_ - m_elements;
        is_trivially_move_constructible<T> trivial_move;
        is_trivially_destructible<T> trivial_destructor;

        m_size = size + n;

        if (m_size <= m_capacity) {
            if (idx + n < size) {
                _uninitialized_move(m_elements + size - n, m_elements + size, m_elements + size,
                                    trivial_move);
                blz::move_backward(position_, m_elements + size - n, m_elements + size);
                result.begin = position_;
                result.mid = position_ + n;
                result.end = position_ + n;
            } else {
                _uninitialized_move(position_, m_elements + size, m_elements + idx + n, trivial_move);
                result.begin = position_;
                result.mid = m_elements + size;
                result.end = m_elements + idx + n;
            }
        } else {
            pointer new_elements = _grow(m_size);
            pointer hole = _uninitialized_move(m_elements, position_, new_elements, trivial_move);
            _uninitialized_move(position_, m_elements + size, hole + n, trivial_move);

            blz::_destroy(m_elements, m_elements + size, trivial_destructor);
            _deallocate_elements();
            _set_elements(new_elements);

            result.begin = result.mid = hole;
            result.end = hole + n;
        }

        return result;
    }

protected:
    // line 872
    template <typename InputIt>
    void _construct(InputIt first, InputIt last, forward_iterator_tag) {
        is_trivially_copy_constructible<T> trivial_copy;  // DWARF local, line 874
        size_type n = distance(first, last);
        if (!m_capacity_is_embedded || n >= m_capacity) {
            pointer elements = this->allocate(n);
            _set_elements(elements);
            m_capacity = n;
        }
        _uninitialized_copy(first, last, m_elements, trivial_copy);
        m_size = n;
    }

    // line 907
    template <typename InputIt>
    iterator _insert(const_iterator position, InputIt first, InputIt last, forward_iterator_tag) {
        is_trivially_copy_constructible<T> trivial_copy;
        size_type idx = position - m_elements;
        size_type n = distance(first, last);
        insert_raw_ranges r = insert_raw(position, n);

        if (distance(r.begin, r.mid) != 0) {
            InputIt tmp = first;
            advance(tmp, static_cast<size_type>(distance(r.begin, r.mid)));
            blz::copy(first, tmp, r.begin);
            first = tmp;
        }

        _uninitialized_copy(first, last, r.mid, trivial_copy);

        return m_elements + idx;
    }

    // line 955
    void _init() {
        m_elements = 0; m_size = 0; m_capacity = 0; m_capacity_is_embedded = false;
    }

    // line 964
    void _assign_rv(vector&& x) {
        if (x.m_capacity_is_embedded) {
            m_size = 0;
            typename iterator_traits<move_iterator<pointer> >::iterator_category category;
            _construct(make_move_iterator(x.m_elements), make_move_iterator(x.m_elements + x.m_size),
                       category);
        } else {
            _set_elements(x.m_elements);
            m_size = x.m_size;
            m_capacity = x.m_capacity;

            x.m_elements = 0;
            x.m_size = 0;
            x.m_capacity = 0;
        }
    }

    // line 990
    pointer _grow(size_type size) {
        size_type new_capacity = m_capacity + m_capacity / 2;
        if (new_capacity < size)
            new_capacity = size;

        m_capacity = new_capacity;
        return this->allocate(new_capacity);
    }

    // line 1001
    void _deallocate_elements() {
        if (!m_capacity_is_embedded) {
            this->deallocate(m_elements);
            m_elements = 0;
        }
    }

    // line 1011
    void _set_elements(pointer elements) {
        m_elements = elements;
        m_capacity_is_embedded = false;
    }

    // line 1018
    void _transfer(pointer elements) {
        is_trivially_move_constructible<T> trivial_move;
        is_trivially_destructible<T> trivial_destructor;

        _uninitialized_move(m_elements, m_elements + m_size, elements, trivial_move);
        blz::_destroy(m_elements, m_elements + m_size, trivial_destructor);
        _deallocate_elements();
    }

    // line 1029
    void _destroy() {
        is_trivially_destructible<T> trivial_destructor;
        blz::_destroy(m_elements, m_elements + m_size, trivial_destructor);
        _deallocate_elements();
    }

    pointer m_elements;                    // line 1037
    size_type m_size;                      // line 1038
    size_type m_capacity : 63;             // line 1039
    size_type m_capacity_is_embedded : 1;  // line 1040
};

}  // namespace blz
