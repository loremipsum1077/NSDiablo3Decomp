// Subset of protobuf 2.6.0 stubs/map_util.h: the lookup and insert helpers the 2.6.2 build
// instantiates (FindWithDefault, FindOrNull, FindPtrOrNull, InsertIfNotPresent).

#pragma once

#include "common.h"

namespace google {
namespace protobuf {

// Returns a const reference to the value associated with the given key if it
// exists. Crashes otherwise.
//
// This is intended as a replacement for operator[] as an rvalue (for reading)
// when the key is guaranteed to exist.
//
// operator[] for lookup is discouraged for several reasons:
//  * It has a side-effect of inserting missing keys
//  * It is not thread-safe (even when it is not inserting, it can still
//      choose to resize the underlying storage)
//  * It invalidates iterators (when it chooses to resize)
//  * It default constructs a value object even if it doesn't need to
//
// This version assumes the key is printable, and includes it in the fatal log
// message.
// line 123
template <class Collection>
const typename Collection::value_type::second_type&
FindWithDefault(const Collection& collection,
                const typename Collection::value_type::first_type& key,
                const typename Collection::value_type::second_type& value) {
  typename Collection::const_iterator it = collection.find(key);
  if (it == collection.end()) {
    return value;
  }
  return it->second;
}

// Returns a pointer to the const value associated with the given key if it
// exists, or NULL otherwise.
// line 137
template <class Collection>
const typename Collection::value_type::second_type*
FindOrNull(const Collection& collection,
           const typename Collection::value_type::first_type& key) {
  typename Collection::const_iterator it = collection.find(key);
  if (it == collection.end()) {
    return 0;
  }
  return &it->second;
}

// Same as above but returns a pointer to the non-const value.
// line 149
template <class Collection>
typename Collection::value_type::second_type*
FindOrNull(Collection& collection,  // NOLINT
           const typename Collection::value_type::first_type& key) {
  typename Collection::iterator it = collection.find(key);
  if (it == collection.end()) {
    return 0;
  }
  return &it->second;
}

// Returns the pointer value associated with the given key. If none is found,
// NULL is returned. The function is designed to be used with a map of keys to
// pointers.
//
// This function does not distinguish between a missing key and a key mapped
// to a NULL value.
// line 166
template <class Collection>
typename Collection::value_type::second_type
FindPtrOrNull(const Collection& collection,
              const typename Collection::value_type::first_type& key) {
  typename Collection::const_iterator it = collection.find(key);
  if (it == collection.end()) {
    return typename Collection::value_type::second_type();
  }
  return it->second;
}

// Same as above, except takes non-const reference to collection.
//
// This function is needed for containers that propagate constness to the
// pointee, such as boost::ptr_map.
// line 181
template <class Collection>
typename Collection::value_type::second_type
FindPtrOrNull(Collection& collection,  // NOLINT
              const typename Collection::value_type::first_type& key) {
  typename Collection::iterator it = collection.find(key);
  if (it == collection.end()) {
    return typename Collection::value_type::second_type();
  }
  return it->second;
}

// Inserts the given key and value into the given collection if and only if the
// given key did NOT already exist in the collection. If the key previously
// existed in the collection, the value is not changed. Returns true if the
// key-value pair was inserted; returns false if the key was already present.
// line 321
template <class Collection>
bool InsertIfNotPresent(Collection* const collection,
                        const typename Collection::value_type& vt) {
  return collection->insert(vt).second;
}

// Same as above except the key and value are passed separately.
// line 328
template <class Collection>
bool InsertIfNotPresent(
    Collection* const collection,
    const typename Collection::value_type::first_type& key,
    const typename Collection::value_type::second_type& value) {
  return InsertIfNotPresent(
      collection, typename Collection::value_type(key, value));
}

}  // namespace protobuf
}  // namespace google
