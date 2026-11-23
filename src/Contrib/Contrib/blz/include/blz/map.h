#pragma once

// blz::map: an rb_tree of pair<const Key, T> (rb_map_traits). operator[] and
// insert construct the value in place into the node that _insert_uniq
// allocates when it is given n == 0 (lines 156-286 of the original).

#include "config.h"
#include "type_traits.h"
#include "util_pair.h"
#include "func_op.h"
#include "mem_alloc.h"
#include "rb_tree.h"

#include <initializer_list>
#include <new>

namespace blz {

// line 16
template <typename Key, typename T>
struct rb_map_traits {
    typedef Key key_type;
    typedef pair<const Key, T> value_type;

    static const key_type& get_key(const value_type& v) {
        return v.first;
    }
};

// line 28
template <typename Key, typename T, typename Compare = less<Key>,
          typename Alloc = allocator<pair<const Key, T> > >
class map : public rb_tree<rb_map_traits<Key, T>, Compare, Alloc> {
    typedef rb_tree<rb_map_traits<Key, T>, Compare, Alloc> _tree;

public:
    typedef typename _tree::key_type key_type;
    typedef typename _tree::value_type value_type;
    typedef typename _tree::key_compare key_compare;
    typedef typename _tree::allocator_type allocator_type;
    typedef typename _tree::iterator iterator;
    typedef typename _tree::const_iterator const_iterator;
    typedef typename _tree::node_type node_type;

    // line 53
    map() : _tree(key_compare(), allocator_type()) {}

    explicit map(const key_compare& comp, const allocator_type& alloc = allocator_type())
        : _tree(comp, alloc) {}

    map(const map& x) : _tree(x, allocator_type()) {}

    explicit map(const allocator_type& alloc) : _tree(key_compare(), alloc) {}

    map(const map& x, const allocator_type& alloc) : _tree(x, alloc) {}

    map(map&& x) : _tree(blz::move(x), allocator_type()) {}

    map(map&& x, const allocator_type& alloc) : _tree(blz::move(x), alloc) {}

    map(std::initializer_list<value_type> il, const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type())
        : _tree(comp, alloc) {
        insert(il);
    }

    map(std::initializer_list<value_type> il, const allocator_type& alloc)
        : _tree(key_compare(), alloc) {
        insert(il);
    }

    map& operator=(const map& x) {
        _tree::operator=(x);
        return *this;
    }

    map& operator=(map&& x) {
        _tree::operator=(blz::move(x));
        return *this;
    }

    map& operator=(std::initializer_list<value_type> il) {
        this->clear();
        insert(il);
        return *this;
    }

    // line 156
    T& operator[](const key_type& k) {
        iterator it = this->lower_bound(k);
        if (it == this->end() || this->key_comp()(k, it->first)) {
            pair<iterator, bool> r = this->_insert_uniq(it, k, 0);
            if (r.second) {
                node_type* node = static_cast<node_type*>(r.first.n);
                ::new (const_cast<key_type*>(&node->val.first)) key_type(k);
                ::new (&node->val.second) T();
            }
            it = r.first;
        }
        return it->second;
    }

    // line 175
    T& operator[](key_type&& k) {
        iterator it = this->lower_bound(k);
        if (it == this->end() || this->key_comp()(k, it->first)) {
            pair<iterator, bool> r = this->_insert_uniq(it, k, 0);
            if (r.second) {
                node_type* node = static_cast<node_type*>(r.first.n);
                ::new (const_cast<key_type*>(&node->val.first)) key_type(blz::move(k));
                ::new (&node->val.second) T();
            }
            it = r.first;
        }
        return it->second;
    }

    T& at(const key_type& k) {
        iterator it = this->find(k);
        BLZ_ASSERT(it != this->end());
        return it->second;
    }

    const T& at(const key_type& k) const {
        const_iterator it = this->find(k);
        BLZ_ASSERT(it != this->end());
        return it->second;
    }

    // line 239
    pair<iterator, bool> insert(const value_type& x) {
        pair<iterator, bool> r = this->_insert_uniq(x.first, 0);
        if (r.second)
            ::new (static_cast<node_type*>(r.first.n)) node_type(x);
        return r;
    }

    // line 250
    template <typename V>
    typename enable_if<is_convertible<V, value_type>::value, pair<iterator, bool> >::type
    insert(V&& x) {
        pair<iterator, bool> r = this->_insert_uniq(x.first, 0);
        if (r.second)
            ::new (static_cast<node_type*>(r.first.n)) node_type(blz::forward<V>(x));
        return r;
    }

    iterator insert(const_iterator position, const value_type& x) {
        pair<iterator, bool> r = this->_insert_uniq(position, x.first, 0);
        if (r.second)
            ::new (static_cast<node_type*>(r.first.n)) node_type(x);
        return r.first;
    }

    // line 270
    template <typename V>
    typename enable_if<is_convertible<V, value_type>::value, iterator>::type
    insert(const_iterator position, V&& x) {
        pair<iterator, bool> r = this->_insert_uniq(position, x.first, 0);
        if (r.second)
            ::new (static_cast<node_type*>(r.first.n)) node_type(blz::forward<V>(x));
        return r.first;
    }

    void insert(std::initializer_list<value_type> il) {
        for (const value_type* i = il.begin(); i != il.end(); ++i)
            insert(*i);
    }
};

}  // namespace blz
