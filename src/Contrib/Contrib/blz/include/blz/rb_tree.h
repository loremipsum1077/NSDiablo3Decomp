#pragma once

// blz::rb_tree: the red-black tree under blz::map and blz::set. Layout (a
// 0x20 byte header node followed by the size, 0x28 in all), method set and
// line order are from the DWARF; the bodies are written from the binary (the
// out-of-line _insert_uniq/_insert_fixup/_erase/_erase_fixup/_destroy_tree/
// _clone_tree instantiations and the inline line tables of their callers).
//
// The tree is the SGI/libstdc++ shape: m_header.parent is the root,
// m_header.left the leftmost node, m_header.right the rightmost, and the
// root's parent is the header. A freshly created node is RED (0). Nodes are
// allocated by _create_node without constructing the value: the caller
// (map::insert, set::insert, map::operator[]) passes n == 0 to _insert_uniq
// and placement-constructs the value into the returned node afterwards.
//
// Not from the DWARF: iterator_category / difference_type of the iterators
// (only used typedefs are recorded), the bodies of the functions that never
// appear out of line or in an inline tree (the move constructor, _assign_rv,
// the _insert_eq family, value_compare, count), and the exact line numbers
// of the methods between the ones the inline table names.

#include "config.h"
#include "type_traits.h"
#include "util_pair.h"
#include "util_swap.h"
#include "iter_types.h"
#include "iter_op.h"
#include "iter_reverse.h"
#include "mem_alloc.h"
#include "func_op.h"
#include "internal/base_assoc.h"

#include <new>

namespace blz {

// line 38
struct rb_node_base {
    rb_node_base* parent;
    rb_node_base* left;
    rb_node_base* right;
    bool color;

    // line 60
    rb_node_base* left_most() {
        rb_node_base* x = this;
        while (x->left != 0)
            x = x->left;
        return x;
    }

    // line 69
    rb_node_base* right_most() {
        rb_node_base* x = this;
        while (x->right != 0)
            x = x->right;
        return x;
    }

    // line 78: the in-order successor
    rb_node_base* next() {
        rb_node_base* x = this;
        if (x->right != 0) {
            x = x->right;
            while (x->left != 0)
                x = x->left;
        } else {
            rb_node_base* y = x->parent;
            while (x == y->right) {
                x = y;
                y = y->parent;
            }
            if (x->right != y)
                x = y;
        }
        return x;
    }

    // line 102: the in-order predecessor (the header's predecessor is the
    // rightmost node)
    rb_node_base* prev() {
        rb_node_base* x = this;
        if (x->color == RED && x->parent->parent == x) {
            x = x->right;
        } else if (x->left != 0) {
            rb_node_base* y = x->left;
            while (y->right != 0)
                y = y->right;
            x = y;
        } else {
            rb_node_base* y = x->parent;
            while (x == y->left) {
                x = y;
                y = y->parent;
            }
            x = y;
        }
        return x;
    }
};

// line 130
template <typename T>
struct rb_node : public rb_node_base {
    // line 134
    rb_node(const T& val) : val(val) {}

    // line 135
    rb_node(T&& val) : val(blz::move(val)) {}

    // line 138
    template <typename... Args>
    rb_node(Args&&... args) : val(blz::forward<Args>(args)...) {}

    T val;
};

template <typename Traits, typename Compare, typename Alloc>
class rb_tree;

template <typename T>
class rb_tree_iterator;

// Defined in set.h / map.h (default template arguments are given there); they
// construct node values in place and so reach the iterators' node pointer.
template <typename Value, typename Compare, typename Alloc>
class set;

template <typename Key, typename T, typename Compare, typename Alloc>
class map;

// line 190
template <typename T>
class rb_tree_const_iterator {
public:
    typedef T value_type;
    typedef const value_type& reference;
    typedef const value_type* pointer;
    typedef rb_node<T> node_type;
    // Not from the DWARF (used by iterator_traits / blz::distance).
    typedef bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;

    rb_tree_const_iterator() : n(0) {}

protected:
    explicit rb_tree_const_iterator(const rb_node_base* x) : n(const_cast<rb_node_base*>(x)) {}

public:
    // line 223
    bool operator==(const rb_tree_const_iterator& x) const {
        return n == x.n;
    }

    bool operator!=(const rb_tree_const_iterator& x) const {
        return n != x.n;
    }

    reference operator*() const {
        return static_cast<node_type*>(n)->val;
    }

    pointer operator->() const {
        return &(operator*());
    }

    // line 247
    rb_tree_const_iterator& operator++() {
        n = n->next();
        return *this;
    }

    rb_tree_const_iterator operator++(int) {
        rb_tree_const_iterator tmp(*this);
        n = n->next();
        return tmp;
    }

    rb_tree_const_iterator& operator--() {
        n = n->prev();
        return *this;
    }

    rb_tree_const_iterator operator--(int) {
        rb_tree_const_iterator tmp(*this);
        n = n->prev();
        return tmp;
    }

protected:
    rb_node_base* n;

    template <typename, typename, typename>
    friend class rb_tree;
    template <typename, typename, typename>
    friend class set;
    template <typename, typename, typename, typename>
    friend class map;
};

// line 283
template <typename T>
class rb_tree_iterator : public rb_tree_const_iterator<T> {
public:
    typedef T value_type;
    typedef value_type& reference;
    typedef value_type* pointer;
    typedef rb_node<T> node_type;

    rb_tree_iterator() {}

private:
    explicit rb_tree_iterator(rb_node_base* x) : rb_tree_const_iterator<T>(x) {}

public:
    reference operator*() const {
        return static_cast<node_type*>(this->n)->val;
    }

    // line 322
    pointer operator->() const {
        return &(operator*());
    }

    // line 328
    rb_tree_iterator& operator++() {
        this->n = this->n->next();
        return *this;
    }

    rb_tree_iterator operator++(int) {
        rb_tree_iterator tmp(*this);
        this->n = this->n->next();
        return tmp;
    }

    // line 343
    rb_tree_iterator& operator--() {
        this->n = this->n->prev();
        return *this;
    }

    rb_tree_iterator operator--(int) {
        rb_tree_iterator tmp(*this);
        this->n = this->n->prev();
        return tmp;
    }

    template <typename, typename, typename>
    friend class rb_tree;
    template <typename, typename, typename>
    friend class set;
    template <typename, typename, typename, typename>
    friend class map;
};

// line 364
template <typename Traits, typename Compare, typename Alloc>
class rb_tree : protected _base_assoc<Compare, Alloc, is_empty<Compare>::value && is_empty<Alloc>::value> {
    typedef _base_assoc<Compare, Alloc, is_empty<Compare>::value && is_empty<Alloc>::value> _base;

public:
    typedef typename Traits::key_type key_type;
    typedef typename Traits::value_type value_type;
    typedef size_t size_type;
    typedef Compare key_compare;
    typedef Alloc allocator_type;
    typedef rb_node<value_type> node_type;
    typedef typename Alloc::template rebind<node_type>::other allocator_node_type;
    typedef typename conditional<is_same<key_type, value_type>::value, rb_tree_const_iterator<value_type>,
                                 rb_tree_iterator<value_type> >::type iterator;
    typedef rb_tree_const_iterator<value_type> const_iterator;
    typedef blz::reverse_iterator<iterator> reverse_iterator;
    typedef blz::reverse_iterator<const_iterator> const_reverse_iterator;

    // Not from the DWARF (the unused typedef is not recorded).
    class value_compare {
    public:
        bool operator()(const value_type& a, const value_type& b) const {
            return comp(Traits::get_key(a), Traits::get_key(b));
        }

    protected:
        friend class rb_tree;
        value_compare(Compare c) : comp(c) {}
        Compare comp;
    };

    rb_tree() : _base(key_compare(), allocator_type()) {
        _init();
    }

    // line 418
    rb_tree(const key_compare& comp, const allocator_type& alloc) : _base(comp, alloc) {
        _init();
    }

    // line 425
    rb_tree(const rb_tree& x, const allocator_type& alloc) : _base(x._compare(), alloc) {
        _init();
        *this = x;
    }

    rb_tree(rb_tree&& x, const allocator_type& alloc) : _base(x._compare(), alloc) {
        _init();
        _assign_rv(blz::move(x));
    }

    // line 441
    ~rb_tree() {
        _destroy_tree(m_header.parent);
    }

    // line 447
    rb_tree& operator=(const rb_tree& x) {
        if (this != &x) {
            clear();
            if (x.m_header.parent) {
                m_header.parent = _clone_tree(x.m_header.parent, &m_header);
                m_header.left = m_header.parent->left_most();
                m_header.right = m_header.parent->right_most();
                m_size = x.m_size;
            }
        }
        return *this;
    }

    rb_tree& operator=(rb_tree&& x) {
        if (this != &x) {
            clear();
            _assign_rv(blz::move(x));
        }
        return *this;
    }

    allocator_type get_allocator() const {
        return this->_alloc();
    }

    // line 484
    iterator begin() {
        return iterator(m_header.left);
    }

    // line 490
    const_iterator begin() const {
        return const_iterator(m_header.left);
    }

    iterator end() {
        return iterator(&m_header);
    }

    const_iterator end() const {
        return const_iterator(&m_header);
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

    // line 557
    bool empty() const {
        return m_size == 0;
    }

    // line 563
    size_type size() const {
        return m_size;
    }

    size_type max_size() const {
        return allocator_node_type().max_size();
    }

    iterator erase(const_iterator position) {
        return iterator(_erase(position.n));
    }

    // line 581. Not declared inline: the protobuf CU has a concrete
    // out-of-line instance (set<const FileDescriptor*>, 0x7100ab77e0), called
    // from DescriptorBuilder::FindSymbol/BuildFile and ExtensionSet; without
    // the hint those callers match. Tested with every includer that calls
    // erase (incl. SubscriptionEvent.h users): no object changed except the
    // two protobuf files. old_size/the result read m_size directly (the
    // DWARF has no size() inlined here).
    size_type erase(const key_type& x) {
        pair<iterator, iterator> p = equal_range(x);
        const size_type old_size = m_size;
        erase(p.first, p.second);
        return old_size - m_size;
    }

    // line 590
    iterator erase(const_iterator first, const_iterator last) {
        if (first == begin() && last == end()) {
            clear();
        } else {
            while (first != last) {
                rb_node_base* n = first.n;
                ++first;
                _erase(n);
            }
        }
        return iterator(last.n);
    }

    // line 611
    void swap(rb_tree& x) {
        this->_swap_compare(x);
        this->_swap_alloc(x);
        blz::swap(m_header, x.m_header);
        blz::swap(m_size, x.m_size);
        if (m_size == 0)
            m_header.left = m_header.right = &m_header;
        else
            m_header.parent->parent = &m_header;
        if (x.m_size == 0)
            x.m_header.left = x.m_header.right = &x.m_header;
        else
            x.m_header.parent->parent = &x.m_header;
    }

    // line 632
    void clear() {
        _destroy_tree(m_header.parent);
        m_header.parent = 0;
        m_header.left = &m_header;
        m_header.right = &m_header;
        m_size = 0;
    }

    key_compare key_comp() const {
        return this->_compare();
    }

    value_compare value_comp() const {
        return value_compare(this->_compare());
    }

    // line 656
    // DWARF shape (locals compare@658, x@659, no iterator local; || at 660:32). The earlier compact
    // form kept _lower_bound/less un-deferred and stopped find inlining into FindWithDefault.
    iterator find(const key_type& k) {
        key_compare compare = this->_compare();
        rb_node_base* x = _lower_bound(m_header.parent, &m_header, k);
        return (x == &m_header || compare(k, get_key(x))) ? iterator(&m_header) : iterator(x);
    }

    // line 666
    // DWARF shape (locals compare@668, x@669; || at 670:32); explicit const_iterator(&m_header)
    // rather than end() gives the original's cmp operand order in FindWithDefault (map_util.h:127).
    const_iterator find(const key_type& k) const {
        key_compare compare = this->_compare();
        const rb_node_base* x = _lower_bound(m_header.parent, const_cast<rb_node_base*>(&m_header), k);
        return (x == &m_header || compare(k, get_key(x))) ? const_iterator(&m_header) : const_iterator(x);
    }

    // line 700
    size_type count(const key_type& k) const {
        pair<const_iterator, const_iterator> p = equal_range(k);
        const size_type n = blz::distance(p.first, p.second);
        return n;
    }

    // line 718
    iterator lower_bound(const key_type& k) {
        return iterator(_lower_bound(m_header.parent, &m_header, k));
    }

    // line 724
    const_iterator lower_bound(const key_type& k) const {
        return const_iterator(_lower_bound(m_header.parent, const_cast<rb_node_base*>(&m_header), k));
    }

    // line 746
    iterator upper_bound(const key_type& k) {
        return iterator(_upper_bound(m_header.parent, &m_header, k));
    }

    const_iterator upper_bound(const key_type& k) const {
        return const_iterator(_upper_bound(m_header.parent, const_cast<rb_node_base*>(&m_header), k));
    }

    // line 774
    pair<iterator, iterator> equal_range(const key_type& k) {
        pair<rb_node_base*, rb_node_base*> ns = _equal_range(k);
        return pair<iterator, iterator>(iterator(ns.first), iterator(ns.second));
    }

    // line 781
    pair<const_iterator, const_iterator> equal_range(const key_type& k) const {
        pair<rb_node_base*, rb_node_base*> ns = _equal_range(k);
        return pair<const_iterator, const_iterator>(const_iterator(ns.first), const_iterator(ns.second));
    }

protected:
    // line 809
    void _init() {
        m_header.parent = 0;
        m_header.left = &m_header;
        m_header.right = &m_header;
        m_header.color = RED;
        m_size = 0;
    }

    static const key_type& get_key(const rb_node_base* x) {
        return Traits::get_key(static_cast<const node_type*>(x)->val);
    }

    // line 825
    node_type* _create_node(bool color) {
        node_type* tmp = allocator_node_type().allocate(1);
        tmp->color = color;
        return tmp;
    }

    // line 835
    void _destroy_node(rb_node_base* x) {
        node_type* n = static_cast<node_type*>(x);
        n->val.~value_type();
        allocator_node_type().deallocate(n);
    }

    // line 845
    rb_node_base* _clone_tree(rb_node_base* x, rb_node_base* p) {
        node_type* y = 0;
        if (x != 0) {
            y = _create_node(x->color);
            y->parent = p;
            y->left = _clone_tree(x->left, y);
            y->right = _clone_tree(x->right, y);
            ::new (y) node_type(static_cast<node_type*>(x)->val);
        }
        return y;
    }

    // line 858
    void _destroy_tree(rb_node_base* x) {
        while (x != 0) {
            _destroy_tree(x->left);
            rb_node_base* y = x->right;
            _destroy_node(x);
            x = y;
        }
    }

    // line 871
    template <typename K>
    rb_node_base* _lower_bound(rb_node_base* x, rb_node_base* y, const K& k) const {
        key_compare compare = this->_compare();
        while (x != 0) {
            if (!compare(get_key(x), k)) {
                y = x;
                x = x->left;
            } else {
                x = x->right;
            }
        }
        return y;
    }

    // line 890
    template <typename K>
    rb_node_base* _upper_bound(rb_node_base* x, rb_node_base* y, const K& k) const {
        key_compare compare = this->_compare();
        while (x != 0) {
            if (compare(k, get_key(x))) {
                y = x;
                x = x->left;
            } else {
                x = x->right;
            }
        }
        return y;
    }

    // line 909: the tree has unique keys, so an equal key is a range of one
    template <typename K>
    pair<rb_node_base*, rb_node_base*> _equal_range(const K& k) const {
        key_compare compare = this->_compare();
        rb_node_base* y = const_cast<rb_node_base*>(&m_header);
        rb_node_base* x = m_header.parent;
        while (x != 0) {
            if (compare(get_key(x), k)) {
                x = x->right;
            } else if (compare(k, get_key(x))) {
                y = x;
                x = x->left;
            } else {
                return pair<rb_node_base*, rb_node_base*>(x, x->next());
            }
        }
        return pair<rb_node_base*, rb_node_base*>(y, y);
    }

    // line 939
    void _rotate_left(rb_node_base* x) {
        rb_node_base* y = x->right;
        x->right = y->left;
        if (y->left)
            y->left->parent = x;
        y->parent = x->parent;
        if (x == m_header.parent)
            m_header.parent = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    // line 960
    void _rotate_right(rb_node_base* x) {
        rb_node_base* y = x->left;
        x->left = y->right;
        if (y->right)
            y->right->parent = x;
        y->parent = x->parent;
        if (x == m_header.parent)
            m_header.parent = y;
        else if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    // line 981: n is the node to link in, or 0 to allocate one (its value is
    // then constructed by the caller)
    pair<iterator, bool> _insert_uniq(const key_type& k, rb_node_base* n) {
        bool comp = true;
        rb_node_base* x = m_header.parent;
        rb_node_base* where = &m_header;
        key_compare compare = this->_compare();
        while (x != 0) {
            where = x;
            comp = compare(k, get_key(x));
            if (comp)
                x = x->left;
            else
                x = x->right;
        }
        if (!comp) {
            if (compare(get_key(where), k))
                return pair<iterator, bool>(iterator(_insert_right(where, n)), true);
            return pair<iterator, bool>(iterator(where), false);
        } else {
            if (where == m_header.left)
                return pair<iterator, bool>(iterator(_insert_left(where, n)), true);
            rb_node_base* test = where->prev();
            if (compare(get_key(test), k))
                return pair<iterator, bool>(iterator(_insert_left(where, n)), true);
            return pair<iterator, bool>(iterator(test), false);
        }
    }

    // line 1022: with a hint
    pair<iterator, bool> _insert_uniq(const_iterator where, const key_type& k, rb_node_base* n) {
        rb_node_base* where_node = where.n;
        key_compare compare = this->_compare();
        if (m_size == 0)
            return pair<iterator, bool>(iterator(_insert_left(&m_header, n)), true);
        if (where_node == m_header.left) {
            if (compare(k, get_key(where_node)))
                return pair<iterator, bool>(iterator(_insert_left(where_node, n)), true);
        } else if (where_node == &m_header) {
            if (compare(get_key(m_header.right), k))
                return pair<iterator, bool>(iterator(_insert_right(m_header.right, n)), true);
        } else if (compare(k, get_key(where_node))) {
            rb_node_base* prev = where_node->prev();
            if (compare(get_key(prev), k)) {
                if (prev->right == 0)
                    return pair<iterator, bool>(iterator(_insert_right(prev, n)), true);
                return pair<iterator, bool>(iterator(_insert_left(where_node, n)), true);
            }
        } else if (compare(get_key(where_node), k)) {
            rb_node_base* next = where_node->next();
            if (next == &m_header || compare(k, get_key(next))) {
                if (where_node->right == 0)
                    return pair<iterator, bool>(iterator(_insert_right(where_node, n)), true);
                return pair<iterator, bool>(iterator(_insert_left(next, n)), true);
            }
        }
        return _insert_uniq(k, n);
    }

    // line 1068 (the node of each element is allocated by _insert_uniq and
    // the value constructed in place)
    template <typename InputIt>
    typename enable_if<!is_integral<InputIt>::value>::type _insert_uniq(InputIt first, InputIt last) {
        for (; first != last; ++first) {
            pair<iterator, bool> r = _insert_uniq(Traits::get_key(*first), 0);
            if (r.second)
                ::new (static_cast<node_type*>(r.first.n)) node_type(*first);
        }
    }

    // Not from the DWARF (never instantiated): a multi-key insert.
    rb_node_base* _insert_eq(rb_node_base* n) {
        key_compare compare = this->_compare();
        rb_node_base* y = &m_header;
        rb_node_base* x = m_header.parent;
        while (x != 0) {
            y = x;
            x = compare(get_key(n), get_key(x)) ? x->left : x->right;
        }
        if (y == &m_header || compare(get_key(n), get_key(y)))
            return _insert_left(y, n);
        return _insert_right(y, n);
    }

    rb_node_base* _insert_eq(const_iterator where, rb_node_base* n) {
        (void)where;
        return _insert_eq(n);
    }

    // line 1165
    rb_node_base* _insert_left(rb_node_base* p, rb_node_base* x) {
        ++m_size;
        if (x == 0)
            x = _create_node(RED);
        x->parent = p;
        x->left = x->right = 0;
        if (p == &m_header) {
            m_header.parent = x;
            m_header.left = x;
            m_header.right = x;
        } else {
            p->left = x;
            if (m_header.left == p)
                m_header.left = x;
        }
        _insert_fixup(x);
        return x;
    }

    // line 1192
    rb_node_base* _insert_right(rb_node_base* p, rb_node_base* x) {
        ++m_size;
        if (x == 0)
            x = _create_node(RED);
        x->parent = p;
        x->left = x->right = 0;
        if (p == &m_header) {
            m_header.parent = x;
            m_header.left = x;
            m_header.right = x;
        } else {
            p->right = x;
            if (m_header.right == p)
                m_header.right = x;
        }
        _insert_fixup(x);
        return x;
    }

    // line 1219
    void _insert_fixup(rb_node_base* x) {
        while (x != m_header.parent && x->parent->color == RED) {
            rb_node_base* xpp = x->parent->parent;
            if (x->parent == xpp->left) {
                rb_node_base* y = xpp->right;
                if (y && y->color == RED) {
                    x->parent->color = BLACK;
                    y->color = BLACK;
                    xpp->color = RED;
                    x = xpp;
                } else {
                    if (x == x->parent->right) {
                        x = x->parent;
                        _rotate_left(x);
                    }
                    x->parent->color = BLACK;
                    xpp->color = RED;
                    _rotate_right(xpp);
                }
            } else {
                rb_node_base* y = xpp->left;
                if (y && y->color == RED) {
                    x->parent->color = BLACK;
                    y->color = BLACK;
                    xpp->color = RED;
                    x = xpp;
                } else {
                    if (x == x->parent->left) {
                        x = x->parent;
                        _rotate_right(x);
                    }
                    x->parent->color = BLACK;
                    xpp->color = RED;
                    _rotate_left(xpp);
                }
            }
        }
        m_header.parent->color = BLACK;
    }

    // line 1283: unlinks and destroys x, returns its successor
    rb_node_base* _erase(rb_node_base* x) {
        rb_node_base* z = x->next();
        rb_node_base* y;
        rb_node_base* z_parent;
        if (x->left != 0 && x->right != 0) {
            y = z->right;
            x->left->parent = z;
            z->left = x->left;
            if (x->right != z) {
                z_parent = z->parent;
                if (y)
                    y->parent = z_parent;
                z->parent->left = y;
                z->right = x->right;
                x->right->parent = z;
            } else {
                z_parent = z;
            }
            if (m_header.parent == x)
                m_header.parent = z;
            else if (x->parent->left == x)
                x->parent->left = z;
            else
                x->parent->right = z;
            z->parent = x->parent;
            blz::swap(z->color, x->color);
        } else {
            z_parent = x->parent;
            if (x->left == 0)
                y = x->right;
            else
                y = x->left;
            if (y)
                y->parent = z_parent;
            if (m_header.parent == x)
                m_header.parent = y;
            else if (x->parent->left == x)
                x->parent->left = y;
            else
                x->parent->right = y;
            if (m_header.left == x) {
                if (x->right == 0)
                    m_header.left = x->parent;
                else
                    m_header.left = y->left_most();
            }
            if (m_header.right == x) {
                if (x->left == 0)
                    m_header.right = x->parent;
                else
                    m_header.right = y->right_most();
            }
        }
        if (x->color != RED)
            _erase_fixup(y, z_parent);
        _destroy_node(x);
        --m_size;
        return z;
    }

    // line 1369
    void _erase_fixup(rb_node_base* x, rb_node_base* x_parent) {
        while (x != m_header.parent && (x == 0 || x->color == BLACK)) {
            if (x == x_parent->left) {
                rb_node_base* w = x_parent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    x_parent->color = RED;
                    _rotate_left(x_parent);
                    w = x_parent->right;
                }
                if ((w->left == 0 || w->left->color == BLACK) &&
                    (w->right == 0 || w->right->color == BLACK)) {
                    w->color = RED;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->right == 0 || w->right->color == BLACK) {
                        w->left->color = BLACK;
                        w->color = RED;
                        _rotate_right(w);
                        w = x_parent->right;
                    }
                    w->color = x_parent->color;
                    x_parent->color = BLACK;
                    if (w->right)
                        w->right->color = BLACK;
                    _rotate_left(x_parent);
                    break;
                }
            } else {
                rb_node_base* w = x_parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x_parent->color = RED;
                    _rotate_right(x_parent);
                    w = x_parent->left;
                }
                if ((w->right == 0 || w->right->color == BLACK) &&
                    (w->left == 0 || w->left->color == BLACK)) {
                    w->color = RED;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->left == 0 || w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        _rotate_left(w);
                        w = x_parent->left;
                    }
                    w->color = x_parent->color;
                    x_parent->color = BLACK;
                    if (w->left)
                        w->left->color = BLACK;
                    _rotate_right(x_parent);
                    break;
                }
            }
        }
        if (x)
            x->color = BLACK;
    }

    // Not from the DWARF.
    void _assign_rv(rb_tree&& x) {
        if (x.m_size != 0) {
            m_header = x.m_header;
            m_size = x.m_size;
            m_header.parent->parent = &m_header;
            x._init();
        }
    }

    rb_node_base m_header;
    size_type m_size;
};

}  // namespace blz

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
// Not checked: rb_node (template); rb_tree_const_iterator (template); rb_tree_iterator (template); rb_tree (template)
static_assert(sizeof(blz::rb_node_base) == 0x20, "blz::rb_node_base");
static_assert(offsetof(blz::rb_node_base, parent) == 0x0, "blz::rb_node_base");
static_assert(offsetof(blz::rb_node_base, left) == 0x8, "blz::rb_node_base");
static_assert(offsetof(blz::rb_node_base, right) == 0x10, "blz::rb_node_base");
static_assert(offsetof(blz::rb_node_base, color) == 0x18, "blz::rb_node_base");
