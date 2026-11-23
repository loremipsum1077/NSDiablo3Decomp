/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2016, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

/* Original DWARF declarations consumed by conncache.c. Complete definitions
 * for every by-value type; pointer-only types are forward declarations.
 * hash.c keeps its own copies of these until it adopts this header.
 */
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_HASH_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_HASH_H

#include <stddef.h>
#include "Externals/curl/lib/llist.h"

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/hash.h:32 */
typedef size_t (*hash_function)(void *, size_t, size_t);

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/hash.h:39 */
typedef size_t (*comp_function)(void *, size_t, void *, size_t);

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/hash.h:44 */
typedef void (*curl_hash_dtor)(void *);

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/hash.h:46 */
struct curl_hash {
  struct curl_llist **table; /* 0x0 */
  hash_function hash_func; /* 0x8 */
  comp_function comp_func; /* 0x10 */
  curl_hash_dtor dtor; /* 0x18 */
  int slots; /* 0x20 */
  size_t size; /* 0x28 */
};

_Static_assert(sizeof(struct curl_hash) == 48, "curl_hash size");
_Static_assert(offsetof(struct curl_hash, table) == 0, "curl_hash.table");
_Static_assert(offsetof(struct curl_hash, hash_func) == 8, "curl_hash.hash_func");
_Static_assert(offsetof(struct curl_hash, comp_func) == 16, "curl_hash.comp_func");
_Static_assert(offsetof(struct curl_hash, dtor) == 24, "curl_hash.dtor");
_Static_assert(offsetof(struct curl_hash, slots) == 32, "curl_hash.slots");
_Static_assert(offsetof(struct curl_hash, size) == 40, "curl_hash.size");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/hash.h:65 */
struct curl_hash_iterator {
  struct curl_hash *hash; /* 0x0 */
  int slot_index; /* 0x8 */
  struct curl_llist_element *current_element; /* 0x10 */
};

_Static_assert(sizeof(struct curl_hash_iterator) == 24, "curl_hash_iterator size");
_Static_assert(offsetof(struct curl_hash_iterator, hash) == 0, "curl_hash_iterator.hash");
_Static_assert(offsetof(struct curl_hash_iterator, slot_index) == 8, "curl_hash_iterator.slot_index");
_Static_assert(offsetof(struct curl_hash_iterator, current_element) == 16, "curl_hash_iterator.current_element");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/hash.h:59 */
struct curl_hash_element {
  void *ptr; /* 0x0 */
  char *key; /* 0x8 */
  size_t key_len; /* 0x10 */
};

_Static_assert(sizeof(struct curl_hash_element) == 24, "curl_hash_element size");
_Static_assert(offsetof(struct curl_hash_element, ptr) == 0, "curl_hash_element.ptr");
_Static_assert(offsetof(struct curl_hash_element, key) == 8, "curl_hash_element.key");
_Static_assert(offsetof(struct curl_hash_element, key_len) == 16, "curl_hash_element.key_len");

struct curl_hash *Curl_hash_alloc(int slots,
                                  hash_function hfunc,
                                  comp_function comparator,
                                  curl_hash_dtor dtor);
void *Curl_hash_add(struct curl_hash *h, void *key, size_t key_len, void *p);
int Curl_hash_delete(struct curl_hash *h, void *key, size_t key_len);
void *Curl_hash_pick(struct curl_hash *h, void *key, size_t key_len);
void Curl_hash_destroy(struct curl_hash *h);
size_t Curl_hash_str(void *key, size_t key_length, size_t slots_num);
size_t Curl_str_key_compare(void *k1, size_t key1_len, void *k2,
                            size_t key2_len);
void Curl_hash_start_iterate(struct curl_hash *hash,
                             struct curl_hash_iterator *iter);
struct curl_hash_element *
Curl_hash_next_element(struct curl_hash_iterator *iter);

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_HASH_H */
