#pragma once

// blz::chained_hash_table: the separately chained hash table under
// unordered_map, unordered_set, chained_hash_map and chained_hash_set.
//
// From the DWARF and the binary:
// - chained_hash_node<T> is { next, val } (next at 0); it has a copy, a move
//   and a variadic constructor and an implicit destructor.
// - The table is { m_bucket_count, m_buckets, m_entry_count,
//   m_max_load_factor } (0x20 bytes) on a protected, empty
//   _base_unordered_assoc. m_bucket_count is always a power of two (at least
//   8): a key goes to bucket hash & (m_bucket_count - 1). m_buckets is
//   allocated lazily (_init_buckets), a new node goes at the end of its
//   chain, and the table doubles when load_factor() > m_max_load_factor.
// - Iterators are { node, bucket, end }; iterator derives from
//   const_iterator, and for sets (key_type == value_type) iterator is
//   const_iterator.
// Line numbers are those of the original header. Everything inlined or out of
// line in the binary was checked against it (tests/blz/unordered_*.cpp,
// chained_hash_table_inline.cpp). Not seen in the binary, so written the
// obvious way: the copy constructor and copy assignment, insert(il),
// insert(hint, v), erase(first, last), equal_range, bucket_size, bucket,
// max_load_factor(float), shrink_to_fit and _equals. The iterator selector
// (is_same<key_type, value_type>) is inferred from unordered_set returning
// const iterators. The bucket (local) iterators are never instantiated, so
// only their names are declared and begin(n)/end(n) are left out.

#include "config.h"
#include "type_traits.h"
#include "mem_alloc.h"
#include "func_hash.h"
#include "func_op.h"
#include "iter_types.h"
#include "util_pair.h"
#include "util_swap.h"
#include "internal/base_unordered_assoc.h"

#include <initializer_list>
#include <new>

namespace blz {

template <typename Traits, typename Hash, typename Pred, typename Alloc>
class chained_hash_table;

// line 30
template <typename T>
struct chained_hash_node {
    // line 32
    chained_hash_node(const T& val) : val(val) {}

    // line 33
    chained_hash_node(T&& val) : val(blz::move(val)) {}

    // line 36
    template <typename... Args>
    chained_hash_node(Args&&... args) : val(blz::forward<Args>(args)...) {}

    // line 83
    chained_hash_node* next;
    T val;
};

// Bucket iterators (local_iterator) are named by the table's typedefs but
// never instantiated in the binary.
template <typename T>
class chained_hash_table_const_local_iterator;

template <typename T>
class chained_hash_table_local_iterator;

// line 229
template <typename T>
class chained_hash_table_const_iterator {
    template <typename, typename, typename, typename>
    friend class chained_hash_table;

public:
    typedef forward_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef const value_type* pointer;
    typedef const value_type& reference;

protected:
    typedef chained_hash_node<T> node_type;

public:
    // line 246
    chained_hash_table_const_iterator() : node(NULL), bucket(NULL), end(NULL) {}

protected:
    // line 253
    chained_hash_table_const_iterator(node_type* node, node_type** bucket, node_type** end)
        : node(node),
          bucket(bucket),
          end(end) {}

public:
    // line 262
    bool operator==(const chained_hash_table_const_iterator& y) const {
        return node == y.node;
    }

    // line 268
    bool operator!=(const chained_hash_table_const_iterator& y) const {
        return node != y.node;
    }

    // line 274
    reference operator*() const {
        return node->val;
    }

    // line 280
    pointer operator->() const {
        return &node->val;
    }

    // line 286. The null test on node is what lets begin() call this on an
    // empty first bucket: inlined there, the binary goes straight to the
    // bucket scan (lines 294-298 here, 379-383 in the iterator's copy).
    chained_hash_table_const_iterator& operator++() {
        if (node != NULL)
            node = node->next;
        if (node == NULL) {
            for (;;) {
                ++bucket;
                if (bucket >= end)
                    break;
                node = *bucket;
                if (node != NULL)
                    break;
            }
        }
        return *this;
    }

    // line 306
    chained_hash_table_const_iterator operator++(int) {
        chained_hash_table_const_iterator tmp(*this);
        ++*this;
        return tmp;
    }

protected:
    // line 315
    node_type* node;
    node_type** bucket;
    node_type** end;

    // line 319
    void _update(node_type** new_bucket, node_type** new_end) {
        bucket = new_bucket;
        end = new_end;
    }
};

// line 328
template <typename T>
class chained_hash_table_iterator : public chained_hash_table_const_iterator<T> {
    template <typename, typename, typename, typename>
    friend class chained_hash_table;

public:
    typedef forward_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;

protected:
    typedef chained_hash_node<T> node_type;

public:
    // line 346
    chained_hash_table_iterator() {}

private:
    // line 352
    chained_hash_table_iterator(node_type* node, node_type** bucket, node_type** end)
        : chained_hash_table_const_iterator<T>(node, bucket, end) {}

public:
    // line 359
    reference operator*() const {
        return this->node->val;
    }

    // line 365
    pointer operator->() const {
        return &this->node->val;
    }

    // line 371
    chained_hash_table_iterator& operator++() {
        if (this->node != NULL)
            this->node = this->node->next;
        if (this->node == NULL) {
            for (;;) {
                ++this->bucket;
                if (this->bucket >= this->end)
                    break;
                this->node = *this->bucket;
                if (this->node != NULL)
                    break;
            }
        }
        return *this;
    }

    // line 391
    chained_hash_table_iterator operator++(int) {
        chained_hash_table_iterator tmp(*this);
        ++*this;
        return tmp;
    }
};

// line 407
template <typename Traits, typename Hash, typename Pred, typename Alloc>
class chained_hash_table : protected _base_unordered_assoc<Hash, Pred, Alloc> {
public:
    typedef Alloc allocator_type;
    typedef typename Traits::key_type key_type;
    typedef typename Traits::value_type value_type;
    typedef Hash hasher;
    typedef Pred key_equal;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

protected:
    // line 427
    typedef chained_hash_node<value_type> node_type;
    typedef typename Alloc::template rebind<node_type*>::other bucket_allocator_type;
    typedef typename Alloc::template rebind<node_type>::other node_allocator_type;

public:
    // line 435
    typedef typename conditional<is_same<key_type, value_type>::value,
                                 chained_hash_table_const_iterator<value_type>,
                                 chained_hash_table_iterator<value_type> >::type iterator;
    typedef chained_hash_table_const_iterator<value_type> const_iterator;
    typedef typename conditional<is_same<key_type, value_type>::value,
                                 chained_hash_table_const_local_iterator<value_type>,
                                 chained_hash_table_local_iterator<value_type> >::type local_iterator;
    typedef chained_hash_table_const_local_iterator<value_type> const_local_iterator;

    // line 445
    explicit chained_hash_table(size_type n, const hasher& hash, const key_equal& equal,
                                const allocator_type& alloc)
        : _base_unordered_assoc<Hash, Pred, Alloc>(hash, equal, alloc) {
        m_bucket_count = n;
        m_buckets = NULL;
        m_entry_count = 0;
        m_max_load_factor = 1.0f;
    }

    // line 455
    chained_hash_table(const chained_hash_table& x, const allocator_type& alloc)
        : _base_unordered_assoc<Hash, Pred, Alloc>(x._hash(), x._equal(), alloc) {
        m_bucket_count = x.m_bucket_count;
        m_buckets = NULL;
        m_entry_count = 0;
        m_max_load_factor = x.m_max_load_factor;
        for (const_iterator it = x.begin(); it != x.end(); ++it)
            insert(*it);
    }

    // line 484
    chained_hash_table(chained_hash_table&& x, const allocator_type& alloc)
        : _base_unordered_assoc<Hash, Pred, Alloc>(x._hash(), x._equal(), alloc) {
        m_buckets = NULL;
        _assign_rv(blz::move(x));
    }

    // line 492
    ~chained_hash_table() {
        _deallocate();
    }

    // line 498
    chained_hash_table& operator=(const chained_hash_table& x) {
        if (this != &x) {
            clear();
            m_max_load_factor = x.m_max_load_factor;
            for (const_iterator it = x.begin(); it != x.end(); ++it)
                insert(*it);
        }
        return *this;
    }

    // line 510
    chained_hash_table& operator=(chained_hash_table&& x) {
        if (this != &x) {
            _deallocate();
            _assign_rv(blz::move(x));
        }
        return *this;
    }

    // line 523
    chained_hash_table& operator=(std::initializer_list<value_type> il) {
        clear();
        insert(il);
        return *this;
    }

    // line 532
    allocator_type get_allocator() const {
        return this->_alloc();
    }

    // line 539
    bool empty() const {
        return m_entry_count == 0;
    }

    // line 545
    size_type size() const {
        return m_entry_count;
    }

    // line 551
    size_type max_size() const {
        return node_allocator_type(this->_alloc()).max_size();
    }

    // line 558
    iterator begin() {
        if (m_buckets != NULL) {
            iterator it(*m_buckets, m_buckets, m_buckets + m_bucket_count);
            if (it.node == NULL)
                ++it;
            return it;
        }
        return iterator();
    }

    // line 573
    iterator end() {
        return iterator(NULL, m_buckets + m_bucket_count, m_buckets + m_bucket_count);
    }

    // line 579
    const_iterator begin() const {
        if (m_buckets != NULL) {
            const_iterator it(*m_buckets, m_buckets, m_buckets + m_bucket_count);
            if (it.node == NULL)
                ++it;
            return it;
        }
        return const_iterator();
    }

    // line 594
    const_iterator end() const {
        return const_iterator(NULL, m_buckets + m_bucket_count, m_buckets + m_bucket_count);
    }

    // line 600
    const_iterator cbegin() const {
        if (m_buckets != NULL) {
            const_iterator it(*m_buckets, m_buckets, m_buckets + m_bucket_count);
            if (it.node == NULL)
                ++it;
            return it;
        }
        return const_iterator();
    }

    // line 615
    const_iterator cend() const {
        return const_iterator(NULL, m_buckets + m_bucket_count, m_buckets + m_bucket_count);
    }

    // line 656
    pair<iterator, bool> insert(const value_type& obj) {
        hasher hash = this->_hash();

        pair<iterator, bool> r = _insert_raw(Traits::get_key(obj), NULL);
        if (r.second) {
            ::new (static_cast<void*>(r.first.node)) node_type(obj);
            if (_checksize())
                r.first._update(_get_bucket(hash(Traits::get_key(obj))), m_buckets + m_bucket_count);
        }
        return r;
    }

    // line 671
    iterator insert(const_iterator, const value_type& obj) {
        return insert(obj).first;
    }

    // line 678
    template <typename InputIt>
    void insert(InputIt first, InputIt last) {
        for (; first != last; ++first)
            insert(*first);
    }

    // line 687
    template <typename P>
    typename enable_if<is_convertible<P, value_type>::value, pair<iterator, bool> >::type insert(P&& obj) {
        hasher hash = this->_hash();

        pair<iterator, bool> r = _insert_raw(Traits::get_key(obj), NULL);
        if (r.second) {
            ::new (static_cast<void*>(r.first.node)) node_type(blz::forward<P>(obj));
            if (_checksize())
                r.first._update(_get_bucket(hash(Traits::get_key(r.first.node->val))),
                                m_buckets + m_bucket_count);
        }
        return r;
    }

    // line 711
    void insert(std::initializer_list<value_type> il) {
        insert(il.begin(), il.end());
    }

    // line 718
    iterator erase(const_iterator position) {
        node_type* node;
        node_allocator_type alloc_node(this->_alloc());

        node_type** bucket = position.bucket;
        node_type** link = bucket;

        while ((node = *link) != NULL) {
            if (node == position.node) {
                iterator it(node, bucket, m_buckets + m_bucket_count);
                ++it;

                node_type* next = node->next;
                node->~node_type();
                alloc_node.deallocate(node);
                *link = next;

                --m_entry_count;
                return it;
            }
            link = &node->next;
        }

        return iterator(NULL, m_buckets + m_bucket_count, m_buckets + m_bucket_count);
    }

    // line 754
    size_type erase(const key_type& k) {
        hasher hash = this->_hash();
        node_allocator_type alloc_node(this->_alloc());

        if (m_buckets != NULL) {
            node_type** bucket = _get_bucket(hash(k));
            node_type** link = _find_node_link(bucket, k);
            node_type* node = *link;
            if (node != NULL) {
                node_type* next = node->next;
                node->~node_type();
                alloc_node.deallocate(node);
                *link = next;

                --m_entry_count;
                return 1;
            }
        }
        return 0;
    }

    // line 798
    iterator erase(const_iterator first, const_iterator last) {
        while (first != last)
            first = erase(first);
        return iterator(last.node, last.bucket, last.end);
    }

    // line 808
    void clear() {
        node_allocator_type alloc_node(this->_alloc());

        if (m_buckets != NULL) {
            for (size_type i = 0; i < m_bucket_count; ++i) {
                node_type* node = m_buckets[i];
                while (node != NULL) {
                    node_type* next = node->next;
                    node->~node_type();
                    alloc_node.deallocate(node);
                    node = next;
                }
                m_buckets[i] = NULL;
            }
        }
        m_entry_count = 0;
    }

    // line 833
    void swap(chained_hash_table& x) {
        this->_swap_hash(x);
        this->_swap_equal(x);
        this->_swap_alloc(x);
        blz::swap(m_bucket_count, x.m_bucket_count);
        blz::swap(m_buckets, x.m_buckets);
        blz::swap(m_entry_count, x.m_entry_count);
        blz::swap(m_max_load_factor, x.m_max_load_factor);
    }

    // line 848
    hasher hash_function() const {
        return this->_hash();
    }

    // line 854
    key_equal key_eq() const {
        return this->_equal();
    }

    // line 861
    iterator find(const key_type& k) {
        if (m_buckets != NULL) {
            hasher hash = this->_hash();
            node_type** bucket = _get_bucket(hash(k));
            node_type* node = *_find_node_link(bucket, k);
            if (node != NULL)
                return iterator(node, bucket, m_buckets + m_bucket_count);
        }
        return end();
    }

    // line 878
    const_iterator find(const key_type& k) const {
        if (m_buckets != NULL) {
            hasher hash = this->_hash();
            node_type** bucket = _get_bucket(hash(k));
            node_type* node = *_find_node_link(bucket, k);
            if (node != NULL)
                return const_iterator(node, bucket, m_buckets + m_bucket_count);
        }
        return end();
    }

    // line 895
    size_type count(const key_type& k) const {
        size_type n = 0;

        if (m_buckets != NULL) {
            size_type i = this->_hash()(k) & (m_bucket_count - 1);
            for (node_type* node = m_buckets[i]; node != NULL; node = node->next) {
                if (this->_equal()(k, Traits::get_key(node->val))) {
                    ++n;
                    break;
                }
            }
        }
        return n;
    }

    // line 922
    pair<iterator, iterator> equal_range(const key_type& k) {
        iterator first = find(k);
        iterator last = first;
        if (last != end())
            ++last;
        return pair<iterator, iterator>(first, last);
    }

    // line 938
    pair<const_iterator, const_iterator> equal_range(const key_type& k) const {
        const_iterator first = find(k);
        const_iterator last = first;
        if (last != end())
            ++last;
        return pair<const_iterator, const_iterator>(first, last);
    }

    // line 955
    size_type bucket_count() const {
        return m_bucket_count;
    }

    // line 961
    size_type max_bucket_count() const {
        return bucket_allocator_type(this->_alloc()).max_size();
    }

    // line 967
    size_type bucket_size(size_type n) const {
        size_type count = 0;
        if (m_buckets != NULL) {
            for (node_type* node = m_buckets[n]; node != NULL; node = node->next)
                ++count;
        }
        return count;
    }

    // line 982
    size_type bucket(const key_type& k) const {
        hasher hash = this->_hash();
        return hash(k) & (m_bucket_count - 1);
    }

    // line 1026
    float load_factor() const {
        return static_cast<float>(m_entry_count) / static_cast<float>(m_bucket_count);
    }

    // line 1032
    float max_load_factor() const {
        return m_max_load_factor;
    }

    // line 1038
    void max_load_factor(float z) {
        m_max_load_factor = z;
        _checksize();
    }

    // line 1044
    void rehash(size_type requested) {
        hasher hash = this->_hash();
        key_equal equal = this->_equal();  // recorded in the DWARF, unused in this build
        (void)equal;
        bucket_allocator_type alloc_bucket(this->_alloc());

        size_type min_size = static_cast<size_type>(m_entry_count / m_max_load_factor);
        if (min_size * m_max_load_factor < m_entry_count)
            ++min_size;
        if (min_size < requested)
            min_size = requested;

        size_type n = 8;
        while (n < min_size)
            n <<= 1;

        if (n == m_bucket_count && m_buckets != NULL)
            return;

        node_type** newBuckets = alloc_bucket.allocate(n);
        for (size_type i = 0; i < n; ++i)
            newBuckets[i] = NULL;

        if (m_buckets != NULL) {
            for (size_t i = 0; i < m_bucket_count; ++i) {
                node_type* node = m_buckets[i];
                while (node != NULL) {
                    const key_type& k = Traits::get_key(node->val);
                    size_t j = hash(k) & (n - 1);
                    node_type* next = node->next;

                    node->next = newBuckets[j];
                    newBuckets[j] = node;
                    node = next;
                }
            }
        }

        alloc_bucket.deallocate(m_buckets);
        m_bucket_count = n;
        m_buckets = newBuckets;
    }

    // line 1110
    void reserve(size_type n) {
        size_type buckets = static_cast<size_type>(n / m_max_load_factor + 0.5f);
        rehash(buckets);
    }

    // line 1117
    void shrink_to_fit() {
        rehash(0);
    }

protected:
    // line 1125
    void _init_buckets() {
        if (m_buckets == NULL)
            rehash(m_bucket_count);
    }

    // line 1132
    bool _checksize() {
        if (load_factor() > m_max_load_factor) {
            rehash(m_bucket_count * 2);
            return true;
        }
        return false;
    }

    // line 1143
    node_type** _get_bucket(size_type hash) const {
        return m_buckets + (hash & (m_bucket_count - 1));
    }

    // line 1150
    node_type** _find_node_link(node_type** bucket, const key_type& k) const {
        node_type* node;
        node_type** link = bucket;

        while ((node = *link) != NULL) {
            if (this->_equal()(k, Traits::get_key(node->val)))
                break;
            link = &node->next;
        }
        return link;
    }

    // line 1169
    pair<iterator, bool> _insert_raw(const key_type& k, node_type* new_node) {
        hasher hash = this->_hash();
        node_allocator_type alloc_node(this->_alloc());

        _init_buckets();

        node_type** bucket = _get_bucket(hash(k));
        node_type** link = _find_node_link(bucket, k);
        node_type* node = *link;
        if (node != NULL)
            return pair<iterator, bool>(iterator(node, bucket, m_buckets + m_bucket_count), false);

        if (new_node == NULL)
            new_node = alloc_node.allocate(1);
        new_node->next = NULL;
        *link = new_node;

        ++m_entry_count;
        return pair<iterator, bool>(iterator(new_node, bucket, m_buckets + m_bucket_count), true);
    }

    // line 1195
    void _assign_rv(chained_hash_table&& x) {
        m_bucket_count = x.m_bucket_count;
        m_max_load_factor = x.m_max_load_factor;
        m_entry_count = x.m_entry_count;
        m_buckets = x.m_buckets;
        x.m_buckets = NULL;
        x.m_entry_count = 0;
    }

    // line 1229
    void _deallocate() {
        node_allocator_type alloc_node(this->_alloc());
        bucket_allocator_type alloc_bucket(this->_alloc());

        if (m_buckets != NULL) {
            for (size_type i = 0; i < m_bucket_count; ++i) {
                node_type* node = m_buckets[i];
                while (node != NULL) {
                    node_type* next = node->next;
                    node->~node_type();
                    alloc_node.deallocate(node);
                    node = next;
                }
            }

            alloc_bucket.deallocate(m_buckets);
            m_buckets = NULL;
        }
    }

    // line 1254
    bool _equals(const chained_hash_table& x) const {
        if (m_entry_count != x.m_entry_count)
            return false;
        for (const_iterator it = begin(); it != end(); ++it) {
            const_iterator xit = x.find(Traits::get_key(*it));
            if (xit == x.end() || !(*xit == *it))
                return false;
        }
        return true;
    }

    // line 1294
    size_t m_bucket_count;
    node_type** m_buckets;
    size_t m_entry_count;
    float m_max_load_factor;
};

}  // namespace blz
