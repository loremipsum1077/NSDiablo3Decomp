#pragma once

// blz::set: an rb_tree of the values themselves (set_traits). Constructors
// and inserts from the inline table (lines 49, 70, 180, 191, 200, 221); the
// ones the binary never shows (initializer_list, move) are written to match.

#include "config.h"
#include "type_traits.h"
#include "util_pair.h"
#include "func_op.h"
#include "mem_alloc.h"
#include "rb_tree.h"

#include <initializer_list>
#include <new>

namespace blz {

// line 15
template <typename Value>
struct set_traits {
    typedef Value key_type;
    typedef Value value_type;

    static const key_type& get_key(const value_type& v) {
        return v;
    }
};

// line 25
template <typename Value, typename Compare = less<Value>, typename Alloc = allocator<Value> >
class set : public rb_tree<set_traits<Value>, Compare, Alloc> {
    typedef rb_tree<set_traits<Value>, Compare, Alloc> _tree;

public:
    typedef typename _tree::value_type value_type;
    typedef typename _tree::key_compare key_compare;
    typedef typename _tree::allocator_type allocator_type;
    typedef typename _tree::iterator iterator;
    typedef typename _tree::const_iterator const_iterator;
    typedef typename _tree::node_type node_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    // line 49
    set() : _tree(key_compare(), allocator_type()) {}

    explicit set(const key_compare& comp, const allocator_type& alloc = allocator_type())
        : _tree(comp, alloc) {}

    // line 70
    set(const set& x) : _tree(x, allocator_type()) {}

    explicit set(const allocator_type& alloc) : _tree(key_compare(), alloc) {}

    set(const set& x, const allocator_type& alloc) : _tree(x, alloc) {}

    set(set&& x) : _tree(blz::move(x), allocator_type()) {}

    set(set&& x, const allocator_type& alloc) : _tree(blz::move(x), alloc) {}

    set(std::initializer_list<value_type> il, const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type())
        : _tree(comp, alloc) {
        insert(il.begin(), il.end());
    }

    set(std::initializer_list<value_type> il, const allocator_type& alloc)
        : _tree(key_compare(), alloc) {
        insert(il.begin(), il.end());
    }

    set& operator=(const set& x) {
        _tree::operator=(x);
        return *this;
    }

    set& operator=(set&& x) {
        _tree::operator=(blz::move(x));
        return *this;
    }

    set& operator=(std::initializer_list<value_type> il) {
        this->clear();
        insert(il.begin(), il.end());
        return *this;
    }

    // line 180
    pair<const_iterator, bool> insert(const value_type& x) {
        pair<const_iterator, bool> r = this->_insert_uniq(x, 0);
        if (r.second)
            ::new (static_cast<node_type*>(r.first.n)) node_type(x);
        return r;
    }

    // line 191
    template <typename V>
    typename enable_if<is_convertible<V, value_type>::value, pair<const_iterator, bool> >::type
    insert(V&& x) {
        pair<const_iterator, bool> r = this->_insert_uniq(x, 0);
        if (r.second)
            ::new (static_cast<node_type*>(r.first.n)) node_type(blz::forward<V>(x));
        return r;
    }

    // line 200
    iterator insert(const_iterator position, const value_type& x) {
        pair<const_iterator, bool> r = this->_insert_uniq(position, x, 0);
        if (r.second)
            ::new (static_cast<node_type*>(r.first.n)) node_type(x);
        return r.first;
    }

    // line 221
    template <typename InputIt>
    void insert(InputIt first, InputIt last) {
        this->_insert_uniq(first, last);
    }

    void insert(std::initializer_list<value_type> il) {
        insert(il.begin(), il.end());
    }
};

}  // namespace blz
