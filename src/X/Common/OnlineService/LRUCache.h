// Original file: X/Common/OnlineService/LRUCache.h
// Subset: LRUCache and TimedLRUCache with the members X/Client/Console/PresenceManager.cpp uses.
// Layouts from the DWARF; bodies written from the copies in PresenceManager.cpp
// (LRUCache::operator[] at 0x710005e020, LRUCache::erase at 0x710005e2d0, and the inlined
// TimedLRUCache::operator[] in PresenceManagerInternal::UpdateSingleFriend). The comments give
// each member's original line.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/embedded_string.h"
#include "../../../Contrib/Contrib/blz/include/blz/list.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include <stddef.h>
#include <stdint.h>

namespace OnlineService {

// line 22
template <typename Key, typename T, typename Hash = blz::hash<Key>, typename Pred = blz::equal_to<Key> >
class LRUCache {
public:
    typedef Key key_type;
    typedef T value_type;
    typedef T * pointer;
    typedef size_t size_type;

    // line 33
    struct node_type {
        key_type key;
        value_type value;
    };

    typedef blz::list<node_type> list_type;
    typedef typename list_type::iterator iterator;
    typedef typename list_type::const_iterator const_iterator;
    typedef blz::unordered_map<Key, iterator, Hash, Pred> map_type;

    // line 53
    LRUCache(size_type uMaxSize)
        : m_uMaxSize(uMaxSize)
    {
    }

    // line 61
    size_type max_size() const { return m_uMaxSize; }

    // line 78
    size_type size() const { return m_cache.size(); }

    // line 162
    bool contains(const key_type & key) const
    {
        return find(key) != m_cache.end();
    }

    // line 182
    const_iterator find(const key_type & key) const
    {
        typename map_type::const_iterator it = m_lookupMap.find(key);
        if (it == m_lookupMap.end())
            return m_cache.end();
        return it->second;
    }

    // line 197
    pointer get(const key_type & key)
    {
        typename map_type::iterator it = m_lookupMap.find(key);
        if (it != m_lookupMap.end())
        {
            m_cache.splice(m_cache.begin(), m_cache, it->second);
            it->second = m_cache.begin();
            return &it->second->value;
        }

        return NULL;
    }

    // line 228
    // The block order (shared return right after the new-entry path) follows from list::splice
    // without a "next" local (list.h line 838).
    // Single return: the original's epilogue row is LRUCache.h:246:9 (a return statement, not the
    // closing brace), and its rows 237:13 (while), 243:25/23 and 244:13/30 give the 12-space
    // nesting of the new-entry code inside the if.
    pointer operator[](const key_type & key)
    {
        pointer value = get(key);
        if (!value)
        {
            while (size() >= max_size())
            {
                pop_oldest();
            }

            node_type & entry = m_cache.push_front();
            entry.key = key;
            m_lookupMap[key] = m_cache.begin();
            value = &entry.value;
        }
        return value;
    }

    // line 294
    bool erase(const key_type & key)
    {
        typename map_type::iterator it = m_lookupMap.find(key);
        if (it == m_lookupMap.end())
            return false;

        m_cache.erase(it->second);
        m_lookupMap.erase(it);
        return true;
    }

    // line 309
    bool pop_oldest()
    {
        if (m_cache.empty())
            return false;

        return erase(m_cache.back().key);
    }

private:
    size_type m_uMaxSize;
    list_type m_cache;
    map_type m_lookupMap;
};

// line 331
template <typename Key, typename T, typename Time, typename Hash = blz::hash<Key>, typename Pred = blz::equal_to<Key> >
class TimedLRUCache {
public:
    typedef Key key_type;
    typedef T value_type;
    typedef T * pointer;
    typedef Time time_type;
    typedef time_type (*time_func)();
    typedef size_t size_type;

    // line 344
    struct node_type {
        value_type value;
        time_type timestamp;
        time_type insert_timestamp;
        uint32 cache_hits;
    };

    typedef LRUCache<Key, node_type, Hash, Pred> cache_type;

    // line 364
    TimedLRUCache(size_type uMaxSize, time_type uTimeout, time_func pfnGetCurrentTime)
        : m_cache(uMaxSize),
          m_uTimeout(uTimeout),
          m_pfnGetCurrentTime(pfnGetCurrentTime)
    {
    }

    // line 507
    pointer get(const key_type & key)
    {
        typename cache_type::pointer node = m_cache.get(key);
        if (node == NULL)
            return NULL;

        time_type now = _current_time();
        if (now - node->timestamp > m_uTimeout)
        {
            m_cache.erase(key);
            return NULL;
        }

        node->timestamp = now;
        ++node->cache_hits;
        return &node->value;
    }

    // line 553
    pointer operator[](const key_type & key)
    {
        bool newEntry = !m_cache.contains(key);
        typename cache_type::pointer node = m_cache[key];

        node->timestamp = _current_time();
        if (newEntry)
        {
            node->insert_timestamp = node->timestamp;
            node->cache_hits = 0;
        }
        else
        {
            ++node->cache_hits;
        }

        return &node->value;
    }

private:
    // line 637
    time_type _current_time() const { return m_pfnGetCurrentTime(); }

    cache_type m_cache;
    time_type m_uTimeout;
    time_func m_pfnGetCurrentTime;
};

}  // namespace OnlineService
