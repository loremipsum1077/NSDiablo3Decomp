#pragma once

// blz::unordered_map: a chained_hash_table of pair<const Key, T>. Members,
// line order and the shape of operator[] (the new node goes at the head of
// its bucket, key at line 202 and a value-initialized mapped_type at 203,
// then _checksize) are from the DWARF and the binary.

#include "config.h"
#include "type_traits.h"
#include "mem_alloc.h"
#include "func_hash.h"
#include "func_op.h"
#include "util_pair.h"
#include "chained_hash_table.h"

#include <initializer_list>
#include <new>

namespace blz {

// line 15
template <typename Key, typename T>
struct unordered_map_traits {
    typedef Key key_type;
    typedef pair<const Key, T> value_type;

    // line 20
    template <typename K, typename V>
    static const K& get_key(const pair<K, V>& v) {
        return v.first;
    }
};

// line 26
template <typename Key, typename T, typename Hash = hash<Key>, typename Pred = equal_to<Key>,
          typename Alloc = allocator<pair<const Key, T> > >
class unordered_map : public chained_hash_table<unordered_map_traits<Key, T>, Hash, Pred, Alloc> {
    typedef chained_hash_table<unordered_map_traits<Key, T>, Hash, Pred, Alloc> base_type;

public:
    typedef Alloc allocator_type;
    typedef Key key_type;
    typedef pair<const Key, T> value_type;
    typedef T mapped_type;
    typedef Hash hasher;
    typedef Pred key_equal;
    typedef typename base_type::size_type size_type;
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;

protected:
    typedef chained_hash_node<value_type> node_type;
    typedef typename base_type::node_allocator_type node_allocator_type;

public:
    // line 56
    unordered_map()
        : base_type(8, hasher(), key_equal(), allocator_type()) {}

    // line 61
    explicit unordered_map(size_type n, const hasher& hash = hasher(), const key_equal& equal = key_equal(),
                           const allocator_type& alloc = allocator_type())
        : base_type(n, hash, equal, alloc) {}

    // line 75
    unordered_map(const unordered_map& x)
        : base_type(x, allocator_type()) {}

    // line 81
    explicit unordered_map(const allocator_type& alloc)
        : base_type(8, hasher(), key_equal(), alloc) {}

    // line 87
    unordered_map(const unordered_map& x, const allocator_type& alloc)
        : base_type(x, alloc) {}

    // line 93
    unordered_map(unordered_map&& x)
        : base_type(blz::move(x), allocator_type()) {}

    // line 99
    unordered_map(unordered_map&& x, const allocator_type& alloc)
        : base_type(blz::move(x), alloc) {}

    // line 106
    unordered_map(std::initializer_list<value_type> il, size_type n = 8, const hasher& hash = hasher(),
                  const key_equal& equal = key_equal(), const allocator_type& alloc = allocator_type())
        : base_type(n, hash, equal, alloc) {
        this->insert(il);
    }

    // line 116
    unordered_map(size_type n, const allocator_type& alloc)
        : base_type(n, hasher(), key_equal(), alloc) {}

    // line 121
    unordered_map(size_type n, const hasher& hash, const allocator_type& alloc)
        : base_type(n, hash, key_equal(), alloc) {}

    // line 144
    unordered_map(std::initializer_list<value_type> il, size_type n, const allocator_type& alloc)
        : base_type(n, hasher(), key_equal(), alloc) {
        this->insert(il);
    }

    // line 154
    unordered_map(std::initializer_list<value_type> il, size_type n, const hasher& hash,
                  const allocator_type& alloc)
        : base_type(n, hash, key_equal(), alloc) {
        this->insert(il);
    }

    // line 163
    unordered_map& operator=(const unordered_map& x) {
        base_type::operator=(x);
        return *this;
    }

    // line 170
    unordered_map& operator=(unordered_map&& x) {
        base_type::operator=(blz::move(x));
        return *this;
    }

    // line 178
    unordered_map& operator=(std::initializer_list<value_type> il) {
        base_type::operator=(il);
        return *this;
    }

    // line 186
    mapped_type& operator[](const key_type& k) {
        node_type* node;
        hasher hash = this->_hash();
        node_allocator_type alloc_node(this->_alloc());

        this->_init_buckets();

        node_type** bucket = this->_get_bucket(hash(k));
        node_type** link = this->_find_node_link(bucket, k);
        node = *link;
        if (node == NULL) {
            node = alloc_node.allocate(1);
            node->next = *bucket;
            ::new (static_cast<void*>(const_cast<key_type*>(&node->val.first))) key_type(k);
            ::new (static_cast<void*>(&node->val.second)) mapped_type();
            *bucket = node;

            ++this->m_entry_count;
            this->_checksize();
        }
        return node->val.second;
    }

    // line 215
    mapped_type& operator[](key_type&& k) {
        node_type* node;
        hasher hash = this->_hash();
        node_allocator_type alloc_node(this->_alloc());

        this->_init_buckets();

        node_type** bucket = this->_get_bucket(hash(k));
        node_type** link = this->_find_node_link(bucket, k);
        node = *link;
        if (node == NULL) {
            node = alloc_node.allocate(1);
            node->next = *bucket;
            ::new (static_cast<void*>(const_cast<key_type*>(&node->val.first))) key_type(blz::move(k));
            ::new (static_cast<void*>(&node->val.second)) mapped_type();
            *bucket = node;

            ++this->m_entry_count;
            this->_checksize();
        }
        return node->val.second;
    }

    // line 244
    mapped_type& at(const key_type& k) {
        size_type i = this->_hash()(k) & (this->m_bucket_count - 1);
        node_type** bucket = this->m_buckets + i;

        for (node_type* node = *bucket;; node = node->next) {
            if (this->_equal()(k, node->val.first))
                return node->val.second;
        }
    }

    // line 264
    const mapped_type& at(const key_type& k) const {
        size_type i = this->_hash()(k) & (this->m_bucket_count - 1);
        node_type** bucket = this->m_buckets + i;

        for (node_type* node = *bucket;; node = node->next) {
            if (this->_equal()(k, node->val.first))
                return node->val.second;
        }
    }
};

}  // namespace blz
