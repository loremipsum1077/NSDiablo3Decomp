#pragma once

// blz::list: doubly linked list with a sentinel node embedded in the list
// object (layout: m_end.prev 0x00, m_end.next 0x08, m_size 0x10). Nodes are
// list_node<T> = list_node_base + value at 0x10. The list privately inherits
// from its allocator. Line numbers in comments are the original ones from
// the DWARF. Members that have no evidence in the binary (no inlined copy
// with line info and no out-of-line instance) are written the way the
// standard container would do it; those are marked "unverified".

#include "config.h"
#include "type_traits.h"
#include "iter_types.h"
#include "iter_reverse.h"
#include "func_op.h"
#include "util_swap.h"
#include "mem_alloc.h"

#include <initializer_list>
#include <new>

namespace blz {

struct list_node_base {
    list_node_base* prev;  // 0x00
    list_node_base* next;  // 0x08
};

// line 34 (the destructor is the implicit one)
template <typename T>
struct list_node : public list_node_base {
    // line 38
    list_node(const T& val) : val(val) {}

    // line 39
    list_node(T&& val) : val(blz::move(val)) {}

    // line 42
    template <typename... Args>
    list_node(Args&&... args) : val(blz::forward<Args>(args)...) {}

    T val;  // 0x10
};

template <typename T, typename Alloc>
class list;

// line 94
template <typename T>
class list_const_iterator {
public:
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef bidirectional_iterator_tag iterator_category;
    typedef const value_type* pointer;
    typedef const value_type& reference;

    list_const_iterator() : node() {}

    explicit list_const_iterator(list_node_base* node) : node(node) {}

    // line 123
    bool operator==(const list_const_iterator& y) const {
        return node == y.node;
    }

    // line 129
    bool operator!=(const list_const_iterator& y) const {
        return node != y.node;
    }

    reference operator*() const {
        return static_cast<list_node<T>*>(node)->val;
    }

    pointer operator->() const {
        return &(operator*());
    }

    list_const_iterator& operator++() {
        node = node->next;
        return *this;
    }

    list_const_iterator operator++(int) {
        list_const_iterator tmp(*this);
        node = node->next;
        return tmp;
    }

    list_const_iterator& operator--() {
        node = node->prev;
        return *this;
    }

    list_const_iterator operator--(int) {
        list_const_iterator tmp(*this);
        node = node->prev;
        return tmp;
    }

protected:
    template <typename U, typename A>
    friend class list;

    list_node_base* node;  // 0x00
};

// line 183
template <typename T>
class list_iterator : public list_const_iterator<T> {
public:
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef bidirectional_iterator_tag iterator_category;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef list_node<T> node_type;

    list_iterator() {}

    // line 212
    reference operator*() const {
        return static_cast<node_type*>(this->node)->val;
    }

    pointer operator->() const {
        return &(operator*());
    }

    // line 224
    list_iterator& operator++() {
        this->node = this->node->next;
        return *this;
    }

    // line 231
    list_iterator operator++(int) {
        list_iterator tmp(*this);
        this->node = this->node->next;
        return tmp;
    }

    list_iterator& operator--() {
        this->node = this->node->prev;
        return *this;
    }

    list_iterator operator--(int) {
        list_iterator tmp(*this);
        this->node = this->node->prev;
        return tmp;
    }

private:
    template <typename U, typename A>
    friend class list;

    explicit list_iterator(list_node_base* node) : list_const_iterator<T>(node) {}
};

// line 260
template <typename T, typename Alloc = allocator<T> >
class list : private Alloc {
public:
    typedef T value_type;
    typedef Alloc allocator_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef list_node<T> node_type;
    typedef typename Alloc::template rebind<node_type>::other node_allocator_type;
    typedef list_iterator<T> iterator;
    typedef list_const_iterator<T> const_iterator;
    typedef blz::reverse_iterator<iterator> reverse_iterator;
    typedef blz::reverse_iterator<const_iterator> const_reverse_iterator;

    // line 288
    list() {
        _init();
    }

    // line 295
    explicit list(const Alloc& alloc) : Alloc(alloc) {
        _init();
    }

    // unverified
    explicit list(size_type n, const Alloc& alloc = Alloc()) : Alloc(alloc) {
        _init();
        for (; n != 0; --n)
            push_back();
    }

    // line 310
    list(size_type n, const T& value, const Alloc& alloc = Alloc()) : Alloc(alloc) {
        _init();
        resize(n, value);
    }

    // unverified
    list(const list& x) : Alloc(x) {
        _init();
        for (const_iterator it = x.begin(); it != x.end(); ++it)
            push_back(*it);
    }

    // unverified
    list(list&& x) : Alloc(x) {
        _init();
        _assign_rv(blz::move(x));
    }

    // unverified
    list(const list& x, const Alloc& alloc) : Alloc(alloc) {
        _init();
        for (const_iterator it = x.begin(); it != x.end(); ++it)
            push_back(*it);
    }

    // unverified
    list(list&& x, const Alloc& alloc) : Alloc(alloc) {
        _init();
        _assign_rv(blz::move(x));
    }

    // unverified
    list(std::initializer_list<T> il, const Alloc& alloc = Alloc()) : Alloc(alloc) {
        _init();
        for (const T* it = il.begin(); it != il.end(); ++it)
            push_back(*it);
    }

    // line 370
    ~list() {
        clear();
    }

    // unverified
    list& operator=(const list& x) {
        if (this != &x)
            assign(x.begin(), x.end());
        return *this;
    }

    // unverified
    list& operator=(list&& x) {
        if (this != &x) {
            clear();
            _assign_rv(blz::move(x));
        }
        return *this;
    }

    // unverified
    list& operator=(std::initializer_list<T> il) {
        assign(il);
        return *this;
    }

    // unverified
    void assign(size_type n, const T& value) {
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

    // line 437
    iterator begin() {
        return iterator(m_end.next);
    }

    const_iterator begin() const {
        return const_iterator(m_end.next);
    }

    iterator end() {
        return iterator(&m_end);
    }

    const_iterator end() const {
        return const_iterator(const_cast<list_node_base*>(&m_end));
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

    // line 510
    bool empty() const {
        return m_size == 0;
    }

    // line 516
    size_type size() const {
        return m_size;
    }

    size_type max_size() const {
        return static_cast<size_type>(-1) / sizeof(node_type);
    }

    void resize(size_type sz) {
        resize(sz, T());
    }

    // line 541
    void resize(size_type sz, const T& x) {
        while (m_size < sz) {
            node_type* n = _alloc_node();
            new (n) node_type(x);
            _insert_before(n, &m_end);
        }
        while (m_size > sz)
            pop_back();
    }

    // line 555
    reference front() {
        return static_cast<node_type*>(m_end.next)->val;
    }

    const_reference front() const {
        return static_cast<const node_type*>(m_end.next)->val;
    }

    // line 567
    reference back() {
        return static_cast<node_type*>(m_end.prev)->val;
    }

    const_reference back() const {
        return static_cast<const node_type*>(m_end.prev)->val;
    }

    // line 592
    void pop_front() {
        _erase(m_end.next);
    }

    void push_front(const T& x) {
        node_type* n = _alloc_node();
        new (n) node_type(x);
        _insert_before(n, m_end.next);
    }

    // line 620
    reference push_front() {
        node_type* n = _alloc_node();
        new (n) node_type();
        _insert_before(n, m_end.next);
        return n->val;
    }

    pointer push_front_raw() {
        node_type* n = _alloc_node();
        _insert_before(n, m_end.next);
        return &n->val;
    }

    void push_front(T&& x) {
        node_type* n = _alloc_node();
        new (n) node_type(blz::move(x));
        _insert_before(n, m_end.next);
    }

    // line 602 (the DWARF names the parameter pack `args`)
    template <typename... Args>
    reference emplace_back(Args&&... args) {
        node_type* n = _alloc_node();
        new (n) node_type(blz::forward<Args>(args)...);
        _insert_before(n, &m_end);
        return n->val;
    }

    // line 645
    void push_back(const T& x) {
        node_type* n = _alloc_node();
        new (n) node_type(x);
        _insert_before(n, &m_end);
    }

    // line 654
    reference push_back() {
        node_type* n = _alloc_node();
        new (n) node_type();
        _insert_before(n, &m_end);
        return n->val;
    }

    pointer push_back_raw() {
        node_type* n = _alloc_node();
        _insert_before(n, &m_end);
        return &n->val;
    }

    // line 674
    void push_back(T&& x) {
        node_type* n = _alloc_node();
        new (n) node_type(blz::move(x));
        _insert_before(n, &m_end);
    }

    // line 682
    void pop_back() {
        _erase(m_end.prev);
    }

    // unverified
    iterator insert(const_iterator position, const T& x) {
        node_type* n = _alloc_node();
        new (n) node_type(x);
        _insert_before(n, position.node);
        return iterator(n);
    }

    // unverified
    iterator insert(const_iterator position, T&& x) {
        node_type* n = _alloc_node();
        new (n) node_type(blz::move(x));
        _insert_before(n, position.node);
        return iterator(n);
    }

    // unverified
    iterator insert(const_iterator position, size_type n, const T& x) {
        iterator ret(position.node);
        for (; n != 0; --n) {
            node_type* nd = _alloc_node();
            new (nd) node_type(x);
            _insert_before(nd, position.node);
            if (ret.node == position.node)
                ret.node = nd;
        }
        return ret;
    }

    // unverified
    iterator insert(const_iterator position, std::initializer_list<T> il) {
        iterator ret(position.node);
        bool first = true;
        for (const T* it = il.begin(); it != il.end(); ++it) {
            node_type* nd = _alloc_node();
            new (nd) node_type(*it);
            _insert_before(nd, position.node);
            if (first) {
                ret.node = nd;
                first = false;
            }
        }
        return ret;
    }

    // line 760
    iterator erase(const_iterator position) {
        iterator ret(position.node->next);
        _erase(position.node);
        return ret;
    }

    // line 768
    iterator erase(const_iterator first, const_iterator last) {
        while (first.node != last.node)
            first = erase(first);
        return iterator(last.node);
    }

    // line 776
    void swap(list& x) {
        blz::swap(m_end, x.m_end);
        blz::swap(m_size, x.m_size);
        if (m_size == 0) {
            m_end.prev = m_end.next = &m_end;
        } else {
            m_end.next->prev = &m_end;
            m_end.prev->next = &m_end;
        }
        if (x.m_size == 0) {
            x.m_end.prev = x.m_end.next = &x.m_end;
        } else {
            x.m_end.next->prev = &x.m_end;
            x.m_end.prev->next = &x.m_end;
        }
    }

    // line 796
    void clear() {
        list_node_base* p = m_end.next;
        m_end.next = &m_end;
        m_end.prev = &m_end;
        m_size = 0;
        while (p != &m_end) {
            list_node_base* next = p->next;
            node_type* node = static_cast<node_type*>(p);
            node->~node_type();
            node_allocator_type(*this).deallocate(node);
            p = next;
        }
    }

    // line 817
    void splice(const_iterator position, list& x) {
        if (!x.empty())
            _splice(position.node, x, x.m_end.next, &x.m_end, x.m_size);
    }

    void splice(const_iterator position, list&& x) {
        splice(position, x);
    }

    // line 835
    void splice(const_iterator position, list& x, const_iterator i) {
        // no "next" local: the DWARF lists only position/x/i; line 838 loads i.node->next after the first compare
        if (position.node != i.node && position.node != i.node->next)
            _splice(position.node, x, i.node, i.node->next, 1);
    }

    void splice(const_iterator position, list&& x, const_iterator i) {
        splice(position, x, i);
    }

    // unverified
    void splice(const_iterator position, list& x, const_iterator first, const_iterator last) {
        if (first != last) {
            size_type count = 0;
            for (const_iterator it = first; it != last; ++it)
                ++count;
            _splice(position.node, x, first.node, last.node, count);
        }
    }

    void splice(const_iterator position, list&& x, const_iterator first, const_iterator last) {
        splice(position, x, first, last);
    }

    // line 867
    void remove(const T& value) {
        list_node_base* last = &m_end;
        list_node_base* first = m_end.next;
        node_type* val_node = 0;
        while (first != last) {
            list_node_base* next = first->next;
            node_type* first_node = static_cast<node_type*>(first);
            if (first_node->val == value) {
                if (&first_node->val == &value)
                    val_node = first_node;
                else
                    _erase(first);
            }
            first = next;
        }
        if (val_node)
            _erase(val_node);
    }

    // line 893
    template <typename Pred>
    void remove_if(Pred pred) {
        list_node_base* last = &m_end;
        list_node_base* first = m_end.next;
        while (first != last) {
            list_node_base* next = first->next;
            if (pred(static_cast<node_type*>(first)->val))
                _erase(first);
            first = next;
        }
    }

    // unverified
    void unique() {
        list_node_base* first = m_end.next;
        list_node_base* last = &m_end;
        if (first == last)
            return;
        list_node_base* next = first->next;
        while (next != last) {
            if (static_cast<node_type*>(first)->val == static_cast<node_type*>(next)->val) {
                _erase(next);
            } else {
                first = next;
            }
            next = first->next;
        }
    }

    void merge(list& x) {
        merge(x, less<T>());
    }

    void merge(list&& x) {
        merge(x, less<T>());
    }

    // line 947
    template <typename Compare>
    void merge(list& x, Compare comp) {
        if (&x == this)
            return;
        list_node_base* first1 = m_end.next;
        list_node_base* last1 = &m_end;
        list_node_base* first2 = x.m_end.next;
        list_node_base* last2 = &x.m_end;
        while (first1 != last1 && first2 != last2) {
            node_type* first1_node = static_cast<node_type*>(first1);
            node_type* first2_node = static_cast<node_type*>(first2);
            if (comp(first2_node->val, first1_node->val)) {
                list_node_base* mid2 = first2->next;
                _splice(first1, x, first2, mid2, 1);
                first2 = mid2;
            } else {
                first1 = first1->next;
            }
        }
        if (first2 != last2)
            _splice(last1, x, first2, last2, x.m_size);
    }

    void sort() {
        sort(less<T>());
    }

    // line 990
    template <typename Compare>
    void sort(Compare comp) {
        if (size() < 2)
            return;

        list tmp(static_cast<const Alloc&>(*this));
        list bin_list[64];
        size_t max_bin = 1;

        while (!empty()) {
            size_t bin;
            tmp._splice(tmp.m_end.next, *this, m_end.next, m_end.next->next, 1);
            for (bin = 0; bin < max_bin && !bin_list[bin].empty(); ++bin) {
                bin_list[bin].merge(tmp, comp);
                bin_list[bin].swap(tmp);
            }
            bin_list[bin].swap(tmp);
            if (bin == max_bin)
                ++max_bin;
        }

        for (size_t bin = 1; bin < max_bin; ++bin)
            bin_list[bin].merge(bin_list[bin - 1], comp);
        splice(begin(), bin_list[max_bin - 1]);
    }

    // unverified
    void reverse() {
        list_node_base* p = &m_end;
        do {
            list_node_base* next = p->next;
            p->next = p->prev;
            p->prev = next;
            p = next;
        } while (p != &m_end);
    }

protected:
    // line 1036
    void _init() {
        m_end.prev = &m_end;
        m_end.next = &m_end;
        m_size = 0;
    }

    // line 1044
    node_type* _alloc_node() {
        return node_allocator_type(*this).allocate(1);
    }

    // line 1051
    void _insert_before(list_node_base* n, list_node_base* p) {
        n->prev = p->prev;
        n->next = p;
        p->prev = n;
        n->prev->next = n;
        ++m_size;
    }

    // line 1061
    void _erase(list_node_base* p) {
        node_type* node = static_cast<node_type*>(p);
        p->prev->next = p->next;
        p->next->prev = p->prev;
        node->~node_type();
        node_allocator_type(*this).deallocate(node);
        --m_size;
    }

    // line 1075
    void _splice(list_node_base* p, list& x, list_node_base* first, list_node_base* last, size_type count) {
        m_size += count;
        x.m_size -= count;
        list_node_base* tmp;
        first->prev->next = last;
        last->prev->next = p;
        p->prev->next = first;
        tmp = p->prev;
        p->prev = last->prev;
        last->prev = first->prev;
        first->prev = tmp;
    }

    // unverified
    void _assign_rv(list&& x) {
        if (x.m_size != 0) {
            m_end = x.m_end;
            m_end.next->prev = &m_end;
            m_end.prev->next = &m_end;
            m_size = x.m_size;
            x._init();
        }
    }

    list_node_base m_end;  // 0x00
    size_t m_size;         // 0x10
};

}  // namespace blz

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
// Not checked: list_node (template); list_const_iterator (template); list_iterator (template); list (template)
static_assert(sizeof(blz::list_node_base) == 0x10, "blz::list_node_base");
static_assert(offsetof(blz::list_node_base, prev) == 0x0, "blz::list_node_base");
static_assert(offsetof(blz::list_node_base, next) == 0x8, "blz::list_node_base");
