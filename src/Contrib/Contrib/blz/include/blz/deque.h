#pragma once

// blz::deque: a table of fixed-size blocks. Layout (size 0x20, privately
// inheriting from the element allocator):
//   m_table       0x00  T**; table of block pointers (null until used)
//   m_table_size  0x08  number of table slots, always a power of two (or 0)
//   m_head        0x10  index of the first element, counted from block 0 of the table
//   m_size        0x18  number of elements
// A block holds 8 elements (the binary allocates 8 * sizeof(T) bytes per
// block for the three sizes it uses). Element index n lives in
// m_table[(n / 8) & (m_table_size - 1)][n % 8]; the table wraps. The line
// numbers in comments are the original ones from the DWARF. Members with no
// inlined copy that carries evidence are marked "unverified".

#include "config.h"
#include "type_traits.h"
#include "iter_types.h"
#include "iter_reverse.h"
#include "util_swap.h"
#include "algo_memory.h"
#include "mem_alloc.h"

#include <initializer_list>
#include <string.h>

namespace blz {

// Elements per block: 64 bytes' worth, but at least 8 (SGameManager's deque<int> has
// 16-element blocks; the larger element types seen elsewhere have 8).
template <typename T>
struct _deque_block {
    static const size_t value = (64 / sizeof(T) > 8) ? 64 / sizeof(T) : 8;
};

template <typename T, typename Alloc>
class deque;

// line 36
template <typename C>
class deque_const_iterator {
public:
    typedef typename C::value_type value_type;
    typedef typename C::size_type size_type;
    typedef typename C::difference_type difference_type;
    typedef typename C::const_pointer pointer;
    typedef typename C::const_reference reference;
    typedef random_access_iterator_tag iterator_category;

    deque_const_iterator() : m_deque(0), m_offset(0) {}

    bool operator==(const deque_const_iterator& x) const {
        return m_deque == x.m_deque && m_offset == x.m_offset;
    }

    // line 74
    bool operator!=(const deque_const_iterator& x) const {
        return m_offset != x.m_offset;
    }

    bool operator<(const deque_const_iterator& x) const {
        return m_offset < x.m_offset;
    }

    bool operator>(const deque_const_iterator& x) const {
        return m_offset > x.m_offset;
    }

    bool operator<=(const deque_const_iterator& x) const {
        return m_offset <= x.m_offset;
    }

    bool operator>=(const deque_const_iterator& x) const {
        return m_offset >= x.m_offset;
    }

    reference operator*() const {
        return *m_deque->_pointer(m_offset);
    }

    pointer operator->() const {
        return &(operator*());
    }

    deque_const_iterator& operator++() {
        ++m_offset;
        return *this;
    }

    deque_const_iterator operator++(int) {
        deque_const_iterator tmp(*this);
        ++m_offset;
        return tmp;
    }

    deque_const_iterator& operator--() {
        --m_offset;
        return *this;
    }

    deque_const_iterator operator--(int) {
        deque_const_iterator tmp(*this);
        --m_offset;
        return tmp;
    }

    deque_const_iterator& operator+=(difference_type n) {
        m_offset += n;
        return *this;
    }

    deque_const_iterator operator+(difference_type n) const {
        deque_const_iterator tmp(*this);
        tmp += n;
        return tmp;
    }

    deque_const_iterator& operator-=(difference_type n) {
        m_offset -= n;
        return *this;
    }

    deque_const_iterator operator-(difference_type n) const {
        deque_const_iterator tmp(*this);
        tmp -= n;
        return tmp;
    }

    difference_type operator-(const deque_const_iterator& x) const {
        return static_cast<difference_type>(m_offset - x.m_offset);
    }

    reference operator[](difference_type n) const {
        return *(*this + n);
    }

protected:
    deque_const_iterator(const C* d, size_type offset) : m_deque(d), m_offset(offset) {}

    template <typename U, typename A>
    friend class deque;

    const C* m_deque;     // 0x00
    size_type m_offset;   // 0x08
};

// line 198
template <typename C>
class deque_iterator : public deque_const_iterator<C> {
public:
    typedef typename C::value_type value_type;
    typedef typename C::size_type size_type;
    typedef typename C::difference_type difference_type;
    typedef typename C::pointer pointer;
    typedef typename C::reference reference;
    typedef random_access_iterator_tag iterator_category;

    deque_iterator() {}

    // line 229
    reference operator*() const {
        return *this->m_deque->_pointer(this->m_offset);
    }

    pointer operator->() const {
        return &(operator*());
    }

    // line 241
    deque_iterator& operator++() {
        ++this->m_offset;
        return *this;
    }

    deque_iterator operator++(int) {
        deque_iterator tmp(*this);
        ++this->m_offset;
        return tmp;
    }

    deque_iterator& operator--() {
        --this->m_offset;
        return *this;
    }

    deque_iterator operator--(int) {
        deque_iterator tmp(*this);
        --this->m_offset;
        return tmp;
    }

    deque_iterator& operator+=(difference_type n) {
        this->m_offset += n;
        return *this;
    }

    deque_iterator operator+(difference_type n) const {
        deque_iterator tmp(*this);
        tmp += n;
        return tmp;
    }

    deque_iterator& operator-=(difference_type n) {
        this->m_offset -= n;
        return *this;
    }

    deque_iterator operator-(difference_type n) const {
        deque_iterator tmp(*this);
        tmp -= n;
        return tmp;
    }

    difference_type operator-(const deque_iterator& x) const {
        return static_cast<difference_type>(this->m_offset - x.m_offset);
    }

    reference operator[](difference_type n) const {
        return *(*this + n);
    }

private:
    deque_iterator(const C* d, size_type offset) : deque_const_iterator<C>(d, offset) {}

    template <typename U, typename A>
    friend class deque;
};

// line 323
template <typename T, typename Alloc = allocator<T> >
class deque : private Alloc {
public:
    typedef T value_type;
    typedef Alloc allocator_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef typename Alloc::template rebind<T*>::other table_allocator_type;
    typedef deque_iterator<deque> iterator;
    typedef deque_const_iterator<deque> const_iterator;
    typedef blz::reverse_iterator<iterator> reverse_iterator;
    typedef blz::reverse_iterator<const_iterator> const_reverse_iterator;

    // line 352
    deque() {
        _init();
    }

    // unverified
    explicit deque(const Alloc& alloc) : Alloc(alloc) {
        _init();
    }

    // unverified
    explicit deque(size_type n, const Alloc& alloc = Alloc()) : Alloc(alloc) {
        _init();
        for (; n != 0; --n)
            push_back();
    }

    // unverified
    deque(size_type n, const value_type& value, const Alloc& alloc = Alloc()) : Alloc(alloc) {
        _init();
        for (; n != 0; --n)
            push_back(value);
    }

    // unverified
    deque(const deque& x) : Alloc(x) {
        _init();
        for (const_iterator it = x.begin(); it != x.end(); ++it)
            push_back(*it);
    }

    // unverified
    deque(const deque& x, const Alloc& alloc) : Alloc(alloc) {
        _init();
        for (const_iterator it = x.begin(); it != x.end(); ++it)
            push_back(*it);
    }

    // unverified
    deque(deque&& x) : Alloc(x) {
        _init();
        _assign_rv(blz::move(x));
    }

    // unverified
    deque(deque&& x, const Alloc& alloc) : Alloc(alloc) {
        _init();
        _assign_rv(blz::move(x));
    }

    // unverified
    deque(std::initializer_list<T> il, const Alloc& alloc = Alloc()) : Alloc(alloc) {
        _init();
        for (const T* it = il.begin(); it != il.end(); ++it)
            push_back(*it);
    }

    // line 433
    ~deque() {
        _deallocate();
    }

    // unverified
    deque& operator=(const deque& x) {
        if (this != &x)
            assign(x.begin(), x.end());
        return *this;
    }

    // unverified
    deque& operator=(deque&& x) {
        if (this != &x) {
            _deallocate();
            _init();
            _assign_rv(blz::move(x));
        }
        return *this;
    }

    // unverified
    deque& operator=(std::initializer_list<T> il) {
        assign(il);
        return *this;
    }

    // unverified
    void assign(size_type n, const value_type& value) {
        clear();
        for (; n != 0; --n)
            push_back(value);
    }

    // unverified
    template <typename InputIt>
    void assign(InputIt first, InputIt last) {
        clear();
        for (; first != last; ++first)
            push_back(*first);
    }

    // unverified
    void assign(std::initializer_list<T> il) {
        clear();
        for (const T* it = il.begin(); it != il.end(); ++it)
            push_back(*it);
    }

    allocator_type get_allocator() const {
        return allocator_type(*this);
    }

    // line 517
    iterator begin() {
        return iterator(this, m_head);
    }

    const_iterator begin() const {
        return const_iterator(this, m_head);
    }

    // line 529
    iterator end() {
        return iterator(this, m_head + m_size);
    }

    const_iterator end() const {
        return const_iterator(this, m_head + m_size);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_iterator cbegin() const {
        return begin();
    }

    const_iterator cend() const {
        return end();
    }

    const_reverse_iterator crbegin() const {
        return rbegin();
    }

    const_reverse_iterator crend() const {
        return rend();
    }

    size_type size() const {
        return m_size;
    }

    size_type max_size() const {
        return static_cast<size_type>(-1) / sizeof(T);
    }

    // unverified
    void resize(size_type n) {
        resize(n, value_type());
    }

    // unverified
    void resize(size_type n, const value_type& value) {
        while (m_size > n)
            pop_back();
        while (m_size < n)
            push_back(value);
    }

    // unverified
    void shrink_to_fit() {}

    // line 637
    bool empty() const {
        return m_size == 0;
    }

    const_reference operator[](size_type n) const {
        return *_pointer(m_head + n);
    }

    reference operator[](size_type n) {
        return *_pointer(m_head + n);
    }

    const_reference at(size_type n) const {
        return *_pointer(m_head + n);
    }

    reference at(size_type n) {
        return *_pointer(m_head + n);
    }

    // line 671
    reference front() {
        return *_pointer(m_head);
    }

    const_reference front() const {
        return *_pointer(m_head);
    }

    reference back() {
        return *_pointer(m_head + m_size - 1);
    }

    const_reference back() const {
        return *_pointer(m_head + m_size - 1);
    }

    // unverified
    void push_front(const value_type& x) {
        pointer p = push_front_raw();
        _copy_construct(p, x, typename is_trivially_copy_constructible<T>::type());
    }

    // unverified
    reference push_front() {
        pointer p = push_front_raw();
        new (p) T();
        return *p;
    }

    // unverified
    void push_front(value_type&& x) {
        pointer p = push_front_raw();
        _move_construct(p, blz::move(x), typename is_trivially_move_constructible<T>::type());
    }

    // unverified
    pointer push_front_raw() {
        if (m_head % _deque_block<T>::value == 0 && m_table_size <= (m_size + _deque_block<T>::value) / _deque_block<T>::value)
            _grow();
        --m_head;
        ++m_size;
        return _get_or_alloc_pointer(m_head);
    }

    // line 779
    void push_back(const value_type& x) {
        pointer p;
        is_trivially_copy_constructible<T> trivial_copy;
        p = push_back_raw();
        _copy_construct(p, x, trivial_copy);
    }

    // line 788
    void push_back(value_type&& x) {
        is_trivially_move_constructible<T> trivial_move;
        pointer p;
        p = push_back_raw();
        _move_construct(p, blz::move(x), trivial_move);
    }

    // unverified
    reference push_back() {
        pointer p = push_back_raw();
        new (p) T();
        return *p;
    }

    // line 808
    pointer push_back_raw() {
        size_type offset;
        if ((m_head + m_size) % _deque_block<T>::value == 0 && m_table_size <= (m_size + _deque_block<T>::value) / _deque_block<T>::value)
            _grow();
        offset = m_head + m_size;
        ++m_size;
        return _get_or_alloc_pointer(offset);
    }

    // unverified
    iterator insert(const_iterator position, const value_type& x) {
        size_type index = position.m_offset - m_head;
        _insert(position, 1, x);
        return iterator(this, m_head + index);
    }

    // unverified
    iterator insert(const_iterator position, value_type&& x) {
        size_type index = position.m_offset - m_head;
        push_back(blz::move(x));
        _rotate_back(index);
        return iterator(this, m_head + index);
    }

    // unverified
    iterator insert(const_iterator position, size_type n, const value_type& x) {
        size_type index = position.m_offset - m_head;
        _insert(position, n, x);
        return iterator(this, m_head + index);
    }

    // unverified
    iterator insert(const_iterator position, std::initializer_list<T> il) {
        size_type index = position.m_offset - m_head;
        size_type count = 0;
        for (const T* it = il.begin(); it != il.end(); ++it, ++count)
            push_back(*it);
        _rotate_back(index, count);
        return iterator(this, m_head + index);
    }

    // line 909
    void pop_front() {
        pointer p = _pointer(m_head);
        _destroy(p, typename is_trivially_destructible<T>::type());
        --m_size;
        ++m_head;
    }

    // unverified
    void pop_back() {
        pointer p = _pointer(m_head + m_size - 1);
        _destroy(p, typename is_trivially_destructible<T>::type());
        --m_size;
    }

    // unverified
    iterator erase(const_iterator position) {
        return erase(position, position + 1);
    }

    // unverified
    iterator erase(const_iterator first, const_iterator last) {
        size_type index = first.m_offset - m_head;
        size_type n = last.m_offset - first.m_offset;
        for (size_type i = index; i + n < m_size; ++i)
            (*this)[i] = blz::move((*this)[i + n]);
        for (; n != 0; --n)
            pop_back();
        return iterator(this, m_head + index);
    }

    // unverified
    iterator unordered_erase(const_iterator position) {
        size_type index = position.m_offset - m_head;
        if (index + 1 != m_size)
            (*this)[index] = blz::move(back());
        pop_back();
        return iterator(this, m_head + index);
    }

    // line 985
    void swap(deque& x) {
        blz::swap(m_table, x.m_table);
        blz::swap(m_table_size, x.m_table_size);
        blz::swap(m_head, x.m_head);
        blz::swap(m_size, x.m_size);
    }

    // unverified
    void clear() {
        _destroy(begin(), end(), typename is_trivially_destructible<T>::type());
        m_head = 0;
        m_size = 0;
    }

    // line 1004
    pointer _pointer(size_type n) const {
        size_type block_index = (n / _deque_block<T>::value) & (m_table_size - 1);
        return m_table[block_index] + n % _deque_block<T>::value;
    }

protected:
    // line 1013
    pointer _get_or_alloc_pointer(size_type n) {
        size_type block_index = (n / _deque_block<T>::value) & (m_table_size - 1);
        if (m_table[block_index] == 0)
            m_table[block_index] = Alloc::allocate(_deque_block<T>::value);
        return m_table[block_index] + n % _deque_block<T>::value;
    }

    // line 1023
    void _init() {
        m_table = 0;
        m_table_size = 0;
        m_head = 0;
        m_size = 0;
    }

    // line 1032
    void _deallocate() {
        is_trivially_destructible<T> trivial_destructor;
        table_allocator_type alloc_table;
        _destroy(begin(), end(), trivial_destructor);
        for (size_type i = 0; i < m_table_size; ++i)
            Alloc::deallocate(m_table[i]);
        alloc_table.deallocate(m_table);
    }

    // unverified. Inserts n copies of x before position by appending them
    // and rotating them into place.
    void _insert(const_iterator position, size_type n, const value_type& x) {
        size_type index = position.m_offset - m_head;
        for (size_type i = 0; i < n; ++i)
            push_back(x);
        _rotate_back(index, n);
    }

    // Moves the last `count` elements in front of element `index`.
    // unverified; invented helper, the DWARF has none.
    void _rotate_back(size_type index, size_type count = 1) {
        size_type end_index = m_size;
        size_type mid = end_index - count;
        _reverse_elements(index, mid);
        _reverse_elements(mid, end_index);
        _reverse_elements(index, end_index);
    }

    void _reverse_elements(size_type first, size_type last) {
        while (first + 1 < last) {
            --last;
            blz::swap((*this)[first], (*this)[last]);
            ++first;
        }
    }

    // line 1071
    void _grow() {
        size_type new_size = m_table_size == 0 ? 8 : m_table_size;
        while (new_size < m_table_size + 1)
            new_size *= 2;
        T** new_table = table_allocator_type().allocate(new_size);
        size_type head_block_index = (m_head / _deque_block<T>::value) & (m_table_size - 1);
        memcpy(new_table, m_table + head_block_index, (m_table_size - head_block_index) * sizeof(T*));
        memcpy(new_table + m_table_size - head_block_index, m_table, head_block_index * sizeof(T*));
        memset(new_table + m_table_size, 0, (new_size - m_table_size) * sizeof(T*));
        table_allocator_type().deallocate(m_table);
        m_table = new_table;
        m_table_size = new_size;
        m_head = m_head % _deque_block<T>::value;
    }

    // unverified
    void _assign_rv(deque&& x) {
        m_table = x.m_table;
        m_table_size = x.m_table_size;
        m_head = x.m_head;
        m_size = x.m_size;
        x._init();
    }

    T** m_table;           // 0x00
    size_type m_table_size;  // 0x08
    size_type m_head;      // 0x10
    size_type m_size;      // 0x18
};

}  // namespace blz
