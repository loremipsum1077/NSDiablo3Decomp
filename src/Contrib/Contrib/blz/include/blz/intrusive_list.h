#pragma once

// blz::intrusive_list: doubly linked list whose links (blz::list_link) live
// inside the elements. The list is named by the element type and a pointer to
// the link member, e.g. intrusive_list<XVarBase, &XVarBase::m_tDirtyLink>.
// The root is a list_link embedded in the list object (size 0x10); an
// unlinked list_link has both pointers null, an empty list's root points at
// itself. Line numbers in comments are the original ones from the DWARF;
// bodies are from the instruction line table of the inlined copies in
// XVar.cpp. Members with no inlined copy that carries evidence are marked
// "unverified".

#include "config.h"
#include "type_traits.h"
#include "iter_types.h"
#include "iter_reverse.h"
#include "object_from_member.h"
#include "util_swap.h"

namespace blz {

struct list_link {
    list_link* next;  // 0x00
    list_link* prev;  // 0x08

    // line 24
    list_link() : next(0), prev(0) {}

    // The links are not copied with their element: a copy starts unlinked.
    // unverified
    list_link(const list_link&) : next(0), prev(0) {}

    // unverified
    list_link(list_link&& o) : next(0), prev(0) {
        _take(o);
    }

    // line 49
    ~list_link() {
        unlink();
    }

    // unverified
    list_link& operator=(const list_link&) {
        return *this;
    }

    // unverified
    list_link& operator=(list_link&& o) {
        if (this != &o) {
            unlink();
            _take(o);
        }
        return *this;
    }

    // line 78
    bool is_linked() const {
        return next != 0;
    }

    // line 84
    void unlink() {
        if (next) {
            prev->next = next;
            next->prev = prev;
        }
        next = 0;
        prev = 0;
    }

private:
    // Takes over o's place in its list. unverified
    void _take(list_link& o) {
        if (o.next) {
            next = o.next;
            prev = o.prev;
            prev->next = this;
            next->prev = this;
            o.next = 0;
            o.prev = 0;
        }
    }
};

template <typename T, list_link T::* Link>
class intrusive_list;

// line 98
template <typename T, list_link T::* Link>
class intrusive_list_const_iterator {
public:
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef bidirectional_iterator_tag iterator_category;
    typedef const T* pointer;
    typedef const T& reference;

    intrusive_list_const_iterator() : node() {}

    bool operator==(const intrusive_list_const_iterator& y) const {
        return node == y.node;
    }

    bool operator!=(const intrusive_list_const_iterator& y) const {
        return node != y.node;
    }

    reference operator*() const {
        return *object_from_member(node, Link);
    }

    pointer operator->() const {
        return &(operator*());
    }

    intrusive_list_const_iterator& operator++() {
        node = node->next;
        return *this;
    }

    intrusive_list_const_iterator operator++(int) {
        intrusive_list_const_iterator tmp(*this);
        node = node->next;
        return tmp;
    }

    intrusive_list_const_iterator& operator--() {
        node = node->prev;
        return *this;
    }

    intrusive_list_const_iterator operator--(int) {
        intrusive_list_const_iterator tmp(*this);
        node = node->prev;
        return tmp;
    }

protected:
    explicit intrusive_list_const_iterator(list_link* node) : node(node) {}

    friend class intrusive_list<T, Link>;

    list_link* node;  // 0x00
};

// line 183
template <typename T, list_link T::* Link>
class intrusive_list_iterator : public intrusive_list_const_iterator<T, Link> {
public:
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef bidirectional_iterator_tag iterator_category;
    typedef T* pointer;
    typedef T& reference;

    intrusive_list_iterator() {}

    // line 213
    reference operator*() const {
        return *object_from_member(this->node, Link);
    }

    // line 219
    pointer operator->() const {
        return &(operator*());
    }

    // line 225
    intrusive_list_iterator& operator++() {
        this->node = this->node->next;
        return *this;
    }

    intrusive_list_iterator operator++(int) {
        intrusive_list_iterator tmp(*this);
        this->node = this->node->next;
        return tmp;
    }

    intrusive_list_iterator& operator--() {
        this->node = this->node->prev;
        return *this;
    }

    intrusive_list_iterator operator--(int) {
        intrusive_list_iterator tmp(*this);
        this->node = this->node->prev;
        return tmp;
    }

private:
    explicit intrusive_list_iterator(list_link* node) : intrusive_list_const_iterator<T, Link>(node) {}

    friend class intrusive_list<T, Link>;
};

// line 263
template <typename T, list_link T::* Link>
class intrusive_list {
public:
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef intrusive_list_iterator<T, Link> iterator;
    typedef intrusive_list_const_iterator<T, Link> const_iterator;
    typedef blz::reverse_iterator<iterator> reverse_iterator;
    typedef blz::reverse_iterator<const_iterator> const_reverse_iterator;

    // line 283
    intrusive_list() {
        _init();
    }

    // unverified
    intrusive_list(intrusive_list&& x) {
        _init();
        _take(x);
    }

    // line 304
    ~intrusive_list() {
        clear();
    }

    // unverified
    intrusive_list& operator=(intrusive_list&& x) {
        if (this != &x) {
            clear();
            _take(x);
        }
        return *this;
    }

    // line 331
    iterator begin() {
        return iterator(m_root.next);
    }

    const_iterator begin() const {
        return const_iterator(m_root.next);
    }

    iterator end() {
        return iterator(&m_root);
    }

    const_iterator end() const {
        return const_iterator(const_cast<list_link*>(&m_root));
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

    // line 404
    bool empty() const {
        return m_root.next == &m_root;
    }

    // unverified
    size_type size() const {
        size_type n = 0;
        for (const list_link* p = m_root.next; p != &m_root; p = p->next)
            ++n;
        return n;
    }

    // line 420
    reference front() {
        return *begin();
    }

    const_reference front() const {
        return *begin();
    }

    reference back() {
        return get_value(m_root.prev);
    }

    const_reference back() const {
        return get_value(m_root.prev);
    }

    // line 445
    void pop_front() {
        erase(front());
    }

    void push_front(reference x) {
        _insert_before(m_root.next, get_node(x));
    }

    // line 459
    void push_back(reference x) {
        _insert_before(&m_root, get_node(x));
    }

    void pop_back() {
        erase(back());
    }

    static iterator insert(const_iterator position, reference x) {
        _insert_before(position.node, get_node(x));
        return iterator(get_node(x));
    }

    // line 495
    static iterator erase(reference x) {
        list_link* node = get_node(x);
        iterator ret(node->next);
        node->unlink();
        return ret;
    }

    static iterator erase(const_iterator position) {
        return erase(get_value(position.node));
    }

    static iterator erase(const_iterator first, const_iterator last) {
        while (first != last)
            first = erase(first);
        return iterator(last.node);
    }

    // line 520
    void swap(intrusive_list& x) {
        blz::swap(*this, x);
    }

    // line 539
    void clear() {
        list_link* node = m_root.next;
        while (node != &m_root) {
            list_link* next = node->next;
            node->next = 0;
            node->prev = 0;
            node = next;
        }
        _init();
    }

    static void splice(const_iterator position, intrusive_list& x) {
        if (!x.empty())
            _splice(position.node, x.m_root.next, &x.m_root);
    }

    static void splice(const_iterator position, intrusive_list&, const_iterator i) {
        splice(position, i, const_iterator(i.node->next));
    }

    static void splice(const_iterator position, intrusive_list&, const_iterator first, const_iterator last) {
        splice(position, first, last);
    }

    static void splice(const_iterator position, const_iterator first, const_iterator last) {
        if (first != last)
            _splice(position.node, first.node, last.node);
    }

    // unverified
    void remove(const_reference value) {
        iterator it = begin();
        while (it != end()) {
            if (*it == value)
                it = erase(it);
            else
                ++it;
        }
    }

    // unverified
    void unique() {
        iterator it = begin();
        if (it == end())
            return;
        iterator next = it;
        ++next;
        while (next != end()) {
            if (*it == *next) {
                next = erase(next);
            } else {
                it = next;
                ++next;
            }
        }
    }

    // unverified
    void merge(intrusive_list& x) {
        if (this == &x)
            return;
        list_link* first1 = m_root.next;
        list_link* first2 = x.m_root.next;
        while (first1 != &m_root && first2 != &x.m_root) {
            if (get_value(first2) < get_value(first1)) {
                list_link* next2 = first2->next;
                _splice(first1, first2, next2);
                first2 = next2;
            } else {
                first1 = first1->next;
            }
        }
        if (first2 != &x.m_root)
            _splice(&m_root, first2, &x.m_root);
    }

    // unverified
    void sort() {
        if (m_root.next == &m_root || m_root.next->next == &m_root)
            return;
        intrusive_list carry;
        intrusive_list bin_list[64];
        size_t max_bin = 1;
        while (!empty()) {
            size_t bin;
            _splice(carry.m_root.next, m_root.next, m_root.next->next);
            for (bin = 0; bin < max_bin && !bin_list[bin].empty(); ++bin) {
                bin_list[bin].merge(carry);
                bin_list[bin].swap(carry);
            }
            bin_list[bin].swap(carry);
            if (bin == max_bin)
                ++max_bin;
        }
        for (size_t bin = 1; bin < max_bin; ++bin)
            bin_list[bin].merge(bin_list[bin - 1]);
        splice(begin(), bin_list[max_bin - 1]);
    }

    // unverified
    void reverse() {
        list_link* p = &m_root;
        do {
            list_link* next = p->next;
            p->next = p->prev;
            p->prev = next;
            p = next;
        } while (p != &m_root);
    }

    static iterator iterator_to(reference x) {
        return iterator(get_node(x));
    }

    static const_iterator iterator_to(const_reference x) {
        return const_iterator(const_cast<list_link*>(get_node(x)));
    }

protected:
    // line 754
    void _init() {
        m_root.next = &m_root;
        m_root.prev = &m_root;
    }

    // line 761
    static list_link* get_node(reference value) {
        return &(value.*Link);
    }

    static const list_link* get_node(const_reference value) {
        return &(value.*Link);
    }

    // line 773
    static reference get_value(list_link* n) {
        return *object_from_member(n, Link);
    }

    static const_reference get_value(const list_link* n) {
        return *object_from_member(n, Link);
    }

    // line 785
    static void _insert_before(list_link* position, list_link* x) {
        x->prev = position->prev;
        x->next = position;
        position->prev = x;
        x->prev->next = x;
    }

    // Moves [first, last) in front of position. unverified
    static void _splice(list_link* position, list_link* first, list_link* last) {
        list_link* tmp;
        first->prev->next = last;
        last->prev->next = position;
        position->prev->next = first;
        tmp = position->prev;
        position->prev = last->prev;
        last->prev = first->prev;
        first->prev = tmp;
    }

    // Copying is not possible. unverified
    intrusive_list(const intrusive_list&);
    intrusive_list& operator=(const intrusive_list&);

private:
    // Moves everything out of x into this (empty) list. unverified
    void _take(intrusive_list& x) {
        if (!x.empty()) {
            m_root.next = x.m_root.next;
            m_root.prev = x.m_root.prev;
            m_root.next->prev = &m_root;
            m_root.prev->next = &m_root;
            x._init();
        }
    }

    list_link m_root;  // 0x00
};

}  // namespace blz

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
// Not checked: intrusive_list_const_iterator (template); intrusive_list_iterator (template); intrusive_list (template)
static_assert(sizeof(blz::list_link) == 0x10, "blz::list_link");
static_assert(offsetof(blz::list_link, next) == 0x0, "blz::list_link");
static_assert(offsetof(blz::list_link, prev) == 0x8, "blz::list_link");
