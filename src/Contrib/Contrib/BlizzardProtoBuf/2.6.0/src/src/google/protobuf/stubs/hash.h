// Subset of protobuf 2.6.0 stubs/hash.h: the GNU branch, with Blizzard's containers.
// hash_map and hash_set derive from blz::chained_hash_map / chained_hash_set (DWARF).

#pragma once

#include <string.h>

#include "common.h"
#include "../../../../../../../blz/include/blz/chained_hash_map.h"
#include "../../../../../../../blz/include/blz/chained_hash_set.h"
#include "../../../../../../../blz/include/blz/string.h"
#include "../../../../../../../blz/include/blz/util_pair.h"

namespace google {
namespace protobuf {

template <typename Key>
struct hash : public blz::hash<Key> {
};

// line 150
template <typename Key>
struct hash<const Key*> {
  inline size_t operator()(const Key* key) const {
    return reinterpret_cast<size_t>(key);
  }
};

// Unlike the old SGI version, the TR1 "hash" does not special-case char*.  So,
// we go ahead and provide our own implementation.
// line 159
template <>
struct hash<const char*> {
  inline size_t operator()(const char* str) const {
    size_t result = 0;
    for (; *str != '\0'; str++) {
      result = 5 * result + *str;
    }
    return result;
  }
};

// line 172
template <typename Key, typename Data,
          typename HashFcn = hash<Key>,
          typename EqualKey = blz::equal_to<Key> >
class hash_map : public blz::chained_hash_map<
    Key, Data, HashFcn, EqualKey> {
 public:
  hash_map(int = 0) {}
};

// line 181
template <typename Key,
          typename HashFcn = hash<Key>,
          typename EqualKey = blz::equal_to<Key> >
class hash_set : public blz::chained_hash_set<
    Key, HashFcn, EqualKey> {
 public:
  hash_set(int = 0) {}
};

// line 190
template <>
struct hash<string> {
  inline size_t operator()(const string& key) const {
    return hash<const char*>()(key.c_str());
  }

  static const size_t bucket_size = 4;
  static const size_t min_buckets = 8;
  inline size_t operator()(const string& a, const string& b) const {
    return a < b;
  }
};

// line 203
template <typename First, typename Second>
struct hash<blz::pair<First, Second> > {
  inline size_t operator()(const blz::pair<First, Second>& key) const {
    size_t first_hash = hash<First>()(key.first);
    size_t second_hash = hash<Second>()(key.second);

    // FIXME(kenton):  What is the best way to compute this hash?  I have
    // no idea!  This seems a bit better than an XOR.
    return first_hash * ((1 << 16) - 1) + second_hash;
  }

  static const size_t bucket_size = 4;
  static const size_t min_buckets = 8;
  inline size_t operator()(const blz::pair<First, Second>& a,
                           const blz::pair<First, Second>& b) const {
    return a < b;
  }
};

// Used by GCC/SGI STL only.  (Why isn't this provided by the standard
// library?  :( )
// line 223
struct streq {
  inline bool operator()(const char* a, const char* b) const {
    return strcmp(a, b) == 0;
  }
};

}  // namespace protobuf
}  // namespace google

// Layout checks generated from the DWARF by tools/layout_asserts.py.
// Not checked: hash (template); hash_map (template); hash_set (template)
static_assert(sizeof(google::protobuf::streq) == 0x1, "google::protobuf::streq");
